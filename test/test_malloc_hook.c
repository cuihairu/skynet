/* Unit test for malloc_hook mem_stats hash-collision handling.
 *
 * A fixed direct-mapped slot table (slot = handle & 0xFFFF) lets a second
 * handle that hashes to the same slot overwrite the first handle's counters;
 * a subsequent free of the first handle then decrements the second handle's
 * counter and corrupts mem_stats (alloc - free underflows to a huge value).
 *
 * This test drives the internal update_* paths directly (the test includes
 * malloc_hook.c so the static helpers stay reachable) and asserts that two
 * distinct handles sharing a hash slot keep independent, balanced counters.
 */
#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <lauxlib.h>
#include <lua.h>

struct skynet_context;

/* Stubs for symbols referenced by malloc_hook.c but not needed here. */
static uint32_t CURRENT_HANDLE = 0;
uint32_t skynet_current_handle(void) { return CURRENT_HANDLE; }
void skynet_error(struct skynet_context *ctx, const char *fmt, ...) {
	(void)ctx;
	(void)fmt;
}

#include "../skynet-src/malloc_hook.c"

static void *
concurrent_worker(void *arg) {
	uint32_t handle = (uint32_t)(uintptr_t)arg;
	/* Interleave alloc/free for this handle while other threads may be
	 * releasing the same slot or claiming a fresh one. */
	for (int i = 0; i < 500; i++) {
		update_xmalloc_stat_alloc(handle, 16);
		update_xmalloc_stat_free(handle, 16);
	}
	return NULL;
}

static struct mem_data * merge_slot = NULL;

static void *
merge_worker(void *arg) {
	(void)arg;
	/* Hammer atomic_meminfo_merge and assert a dump never sees alloc < free. */
	for (int i = 0; i < 2000; i++) {
		MemInfo m = {};
		atomic_meminfo_merge(&m, &merge_slot->info);
		assert(m.alloc >= m.free);
	}
	return NULL;
}

/* Count entries in the table at the top of the stack. */
static int
count_entries(lua_State *L) {
	int count = 0;
	lua_checkstack(L, 3);
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		++count;
		lua_pop(L, 1);
	}
	return count;
}

/* Assert table[handle] == expect.  The table must be at stack top. */
static void
assert_entry(lua_State *L, uint32_t handle, lua_Integer expect) {
	lua_pushinteger(L, handle);
	lua_rawget(L, -2);
	assert(lua_isinteger(L, -1));
	assert(lua_tointeger(L, -1) == expect);
	lua_pop(L, 1);
}

int
main(void) {
	/* Two distinct handles sharing the low 16 bits (the old direct-mapped
	 * slot) so the pre-fix table collides: h2 = h1 + k*SLOT_SIZE.  Pick a k
	 * that also keeps the new mixed hash apart so the fix separates them. */
	uint32_t h1 = 1;
	uint32_t h2 = 0;
	for (uint32_t k = 1; k < 16; k++) {
		uint32_t cand = h1 + k * SLOT_SIZE;
		if ((cand & (SLOT_SIZE - 1)) == (h1 & (SLOT_SIZE - 1)) &&
		    hash_handle(cand) != hash_handle(h1)) {
			h2 = cand;
			break;
		}
	}
	assert(h2 != 0 && h2 != h1);
	/* Pre-fix collision condition and post-fix separation. */
	assert((h2 & (SLOT_SIZE - 1)) == (h1 & (SLOT_SIZE - 1)));
	assert(hash_handle(h2) != hash_handle(h1));
	printf("handles: h1=%08x h2=%08x (slot0=%08x h1hash=%08x h2hash=%08x)\n",
		h1, h2, h1 & (SLOT_SIZE - 1), hash_handle(h1), hash_handle(h2));

	lua_State *L = luaL_newstate();
	assert(L);

	/* First handle allocates 100 bytes, second allocates 50 bytes. */
	update_xmalloc_stat_alloc(h1, 100);
	update_xmalloc_stat_alloc(h2, 50);

	/* Both handles must own separate slots: dump_mem_lua reports two entries
	 * (h1=100, h2=50).  A direct-mapped table collapsed both into one slot,
	 * so only the last handle survived and h1's counter was lost. */
	assert(dump_mem_lua(L) == 1);
	assert(count_entries(L) == 2);
	assert_entry(L, h1, 100);
	assert_entry(L, h2, 50);
	lua_pop(L, 1);

	/* Free h1: its own counter returns to zero, h2 is untouched.  The old
	 * bug decremented the merged slot instead, corrupting h2's count. */
	update_xmalloc_stat_free(h1, 100);

	size_t used = malloc_used_memory();
	printf("used=%zu (expect 50)\n", used);
	assert(used == 50);
	assert(dump_mem_lua(L) == 1);
	/* h1's slot is now balanced but not cleared (no asynchronous reclaim),
	 * so h1 still appears with a zero count. */
	assert_entry(L, h1, 0);
	assert_entry(L, h2, 50);
	lua_pop(L, 1);

	/* Both handles fully released; the table reports zero used. */
	update_xmalloc_stat_free(h2, 50);
	used = malloc_used_memory();
	printf("used=%zu (expect 0)\n", used);
	assert(used == 0);
	assert(dump_mem_lua(L) == 1);
	assert(count_entries(L) == 2);
	lua_pop(L, 1);

	/* A new handle takes a fresh slot (the table is far from full here). */
	uint32_t h3 = h1 + 2 * SLOT_SIZE;
	update_xmalloc_stat_alloc(h3, 7);
	used = malloc_used_memory();
	printf("used=%zu (expect 7)\n", used);
	assert(used == 7);

	/* Slots are retained after their counters balance out (no lazy reclaim),
	 * so h1 keeps its slot and a fresh handle always gets a fresh slot while
	 * the table has room.  h1's balanced slot must still be owned by h1. */
	{
		struct mem_data *d1 = find_mem_stat(h1, 1);
		assert(d1 != NULL);
		uint32_t cur = (uint32_t)ATOM_LOAD(&d1->handle);
		assert(cur == h1);
		/* h1 allocated 100 and freed 100, so its counters are balanced. */
		assert(ATOM_LOAD(&d1->info.alloc_count) == ATOM_LOAD(&d1->info.free_count));
		/* Reserve a new handle: it gets a fresh slot, not h1's slot. */
		uint32_t newh = 0x77777777;
		update_xmalloc_stat_alloc(newh, 3);
		used = malloc_used_memory();
		printf("used=%zu (expect 10)\n", used);
		assert(used == 10);
	}

	lua_close(L);

	/* Dead-but-public aligned APIs: exercise the latent-bug fixes. */
	void *p = skynet_calloc(4, 0);
	assert(p == NULL);

	p = skynet_calloc(4, 8);
	assert(p != NULL);
	memset(p, 0xAB, 32);
	skynet_free(p);

	p = skynet_aligned_alloc(16, 10);
	assert(p != NULL);
	assert(((uintptr_t)p & 15) == 0);
	memset(p, 0xCD, 10);
	skynet_free(p);

	p = skynet_memalign(16, 10);
	assert(p != NULL);
	assert(((uintptr_t)p & 15) == 0);
	memset(p, 0xEF, 10);
	skynet_free(p);

	void *mp = NULL;
	int err = skynet_posix_memalign(&mp, 16, 10);
	assert(err == 0);
	assert(mp != NULL);
	assert(((uintptr_t)mp & 15) == 0);
	memset(mp, 0x12, 10);
	skynet_free(mp);

	/* Concurrent dump vs alloc/free: merge must never observe alloc < free
	 * (underflow).  One thread interleaves atomic alloc/free on a live slot
	 * while several other threads hammer atomic_meminfo_merge on the same
	 * slot and assert alloc >= free.  The dump reads free before alloc, so
	 * underflow is impossible for the monotonic, always-free<=alloc counters. */
	{
		#define NTHREAD 4
		#define NITER 2000
		uint32_t h = 0x111;
		merge_slot = find_mem_stat(h, 1);
		assert(merge_slot != NULL);

		pthread_t th[NTHREAD + 1];
		/* One writer thread interleaves alloc/free. */
		pthread_create(&th[0], NULL, concurrent_worker, (void *)(uintptr_t)h);
		/* Reader threads hammer merge and assert no underflow. */
		for (int i = 0; i < NTHREAD; i++) {
			pthread_create(&th[i + 1], NULL, merge_worker, NULL);
		}
		for (int i = 0; i < NTHREAD + 1; i++) {
			pthread_join(th[i], NULL);
		}
		ATOM_STORE(&merge_slot->handle, 0);
	}

	/* True multi-threaded alloc/free across distinct handles, plus one
	 * thread that frees another thread's handle (cross-thread release). */
	{
		pthread_t th[5];
		for (int i = 0; i < 4; i++) {
			pthread_create(&th[i], NULL, concurrent_worker,
				(void *)(uintptr_t)(0x1000 + i));
		}
		pthread_create(&th[4], NULL, concurrent_worker,
			(void *)(uintptr_t)0x1000);	/* cross-thread frees of h=0x1000 */
		for (int i = 0; i < 5; i++) {
			pthread_join(th[i], NULL);
		}
		/* After balanced alloc/free, used memory for these handles is 0. */
		uint32_t h = 0x1000;
		CURRENT_HANDLE = h;
		assert(malloc_current_memory() == 0);
		CURRENT_HANDLE = 0;
	}

	/* Cross-thread free must not corrupt a slot claimed by another handle. */
	{
		uint32_t owner = 0x5001;
		uint32_t other = 0x6001;
		update_xmalloc_stat_alloc(owner, 64);
		update_xmalloc_stat_free(owner, 64);	/* balanced, cross-thread style */
		struct mem_data *owner_slot = find_mem_stat(owner, 1);
		assert(owner_slot != NULL);
		update_xmalloc_stat_alloc(other, 32);
		CURRENT_HANDLE = owner;
		assert(malloc_current_memory() == 0);
		CURRENT_HANDLE = other;
		assert(malloc_current_memory() == 32);
		CURRENT_HANDLE = 0;
		assert(malloc_memory_block() >= 1);
		/* reserve=0 misses when the handle owns no slot. */
		assert(find_mem_stat(0x7fffffff, 0) == NULL);
	}

	printf("test_malloc_hook: PASS\n");
	return 0;
}
