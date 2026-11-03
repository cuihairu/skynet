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
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <lauxlib.h>
#include <lua.h>

struct skynet_context;

/* Stubs for symbols referenced by malloc_hook.c but not needed here. */
uint32_t skynet_current_handle(void) { return 0; }
void skynet_error(struct skynet_context *ctx, const char *fmt, ...) {
	(void)ctx;
	(void)fmt;
}

#include "../skynet-src/malloc_hook.c"

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
	/* h1 was released and its slot recycled, so it no longer appears. */
	lua_pushinteger(L, h1);
	lua_rawget(L, -2);
	assert(lua_isnil(L, -1));
	lua_pop(L, 1);
	assert_entry(L, h2, 50);
	lua_pop(L, 1);

	/* Both handles fully released; the table returns to zero. */
	update_xmalloc_stat_free(h2, 50);
	used = malloc_used_memory();
	printf("used=%zu (expect 0)\n", used);
	assert(used == 0);
	assert(dump_mem_lua(L) == 1);
	assert(count_entries(L) == 0);
	lua_pop(L, 1);

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

	printf("test_malloc_hook: PASS\n");
	return 0;
}
