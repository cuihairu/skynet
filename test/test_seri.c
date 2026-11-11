/* Unit test for lua-seri: bound decoded array_size before preallocating,
 * and cover the encode side (all value encodings, table walkers and error
 * paths) through pack/unpack round trips.
 *
 * A remote stream encodes a table header with a huge array_size (e.g.
 * INT64_MAX / INT32_MAX).  unpack_table previously forwarded that value
 * straight to lua_createtable, which preallocates the array part and can
 * trigger an OOM.  The fix rejects any array_size that cannot fit in the
 * remaining stream (each array element needs at least one byte) before it
 * reaches lua_createtable.
 *
 * This test drives luaseri_pack / luaseri_unpack directly and asserts:
 *   - a huge array_size stream is rejected as an invalid stream
 *   - a table header that is not a number cookie is rejected
 *   - legitimate values round-trip through pack/unpack unchanged
 *   - packing errors (unsupported type, too deep, __pairs failure) raise
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "../lualib-src/lua-seri.c"

/* Build the stream for a table whose array_size is `value`, encoded as a
 * qword/dword number following the MAX_COOKIE-1 table marker. */
static int
build_table_stream(char *buf, uint8_t cookie, const void *value, size_t valsz) {
	int n = 0;
	buf[n++] = COMBINE_TYPE(TYPE_TABLE, MAX_COOKIE - 1);
	buf[n++] = COMBINE_TYPE(TYPE_NUMBER, cookie);
	memcpy(buf + n, value, valsz);
	n += (int)valsz;
	return n;
}

/* Assert luaseri_unpack raises "Invalid serialize stream". */
static void
expect_invalid(lua_State *L, const char *buf, int len) {
	lua_pushcfunction(L, luaseri_unpack);
	lua_pushlstring(L, buf, len);
	int rc = lua_pcall(L, 1, 0, 0);
	assert(rc == LUA_ERRRUN);
	assert(strstr(lua_tostring(L, -1), "Invalid serialize stream") != NULL);
	lua_pop(L, 1);
}

/* Pack/unpack round trip: `value` is a lua chunk producing the value to
 * pack, `check` is a lua chunk receiving the unpacked value (as `...`) and
 * returning true when it is unchanged.  Frees the packed buffer. */
static void
roundtrip(lua_State *L, const char *value, const char *check) {
	assert(luaL_dostring(L, value) == LUA_OK);

	lua_pushcfunction(L, luaseri_pack);
	lua_insert(L, -2);
	assert(lua_pcall(L, 1, 2, 0) == LUA_OK);
	assert(lua_islightuserdata(L, -2));
	assert(lua_isinteger(L, -1));
	void *ptr = lua_touserdata(L, -2);

	lua_pushcfunction(L, luaseri_unpack);
	lua_insert(L, -3);
	assert(lua_pcall(L, 2, 1, 0) == LUA_OK);
	skynet_free(ptr);

	assert(luaL_loadstring(L, check) == LUA_OK);
	lua_insert(L, -2);
	assert(lua_pcall(L, 1, 1, 0) == LUA_OK);
	assert(lua_toboolean(L, -1) == 1);
	lua_pop(L, 1);
}

/* Assert packing the value produced by `value` raises an error containing
 * `msg`. */
static void
expect_pack_error(lua_State *L, const char *value, const char *msg) {
	assert(luaL_dostring(L, value) == LUA_OK);
	lua_pushcfunction(L, luaseri_pack);
	lua_insert(L, -2);
	assert(lua_pcall(L, 1, 0, 0) == LUA_ERRRUN);
	assert(strstr(lua_tostring(L, -1), msg) != NULL);
	lua_pop(L, 1);
}

struct rt_case {
	const char *name;
	const char *value;
	const char *check;
};

static const struct rt_case rt_cases[] = {
	/* Every wb_integer encoding: zero, byte, word, dword (positive and
	 * negative), qword. */
	{ "integer 0", "return 0", "local v = ... return v == 0" },
	{ "integer 1", "return 1", "local v = ... return v == 1" },
	{ "integer 255", "return 255", "local v = ... return v == 255" },
	{ "integer 256", "return 256", "local v = ... return v == 256" },
	{ "integer 65535", "return 65535", "local v = ... return v == 65535" },
	{ "integer 65536", "return 65536", "local v = ... return v == 65536" },
	{ "integer int32 max", "return 2147483647", "local v = ... return v == 2147483647" },
	{ "integer -1", "return -1", "local v = ... return v == -1" },
	{ "integer int32 min", "return -2147483648", "local v = ... return v == -2147483648" },
	{ "integer 2^31 (qword)", "return 2147483648", "local v = ... return v == 2147483648" },
	{ "integer maxinteger", "return math.maxinteger", "local v = ... return v == math.maxinteger" },
	{ "integer mininteger", "return math.mininteger", "local v = ... return v == math.mininteger" },
	/* Real. */
	{ "real 3.5", "return 3.5", "local v = ... return v == 3.5" },
	/* Booleans (both hibits). */
	{ "boolean true", "return true", "local v = ... return v == true" },
	{ "boolean false", "return false", "local v = ... return v == false" },
	/* Strings: zero length, short (< MAX_COOKIE) and the long-string
	 * boundaries at the 2-byte and 4-byte length encodings. */
	{ "empty string", "return ''", "local v = ... return v == ''" },
	{ "string len 31", "return string.rep('a', 31)", "local v = ... return v == string.rep('a', 31)" },
	{ "string len 32", "return string.rep('a', 32)", "local v = ... return v == string.rep('a', 32)" },
	{ "string len 65535", "return string.rep('b', 65535)", "local v = ... return v == string.rep('b', 65535)" },
	{ "string len 65536", "return string.rep('c', 65536)", "local v = ... return v == string.rep('c', 65536)" },
	/* Tables: array (inline size), size-header arrays at the MAX_COOKIE-1
	 * boundary and beyond, hash part, sparse arrays and nesting. */
	{ "array table", "return {10,20,30}", "local v = ... return #v == 3 and v[1] == 10 and v[2] == 20 and v[3] == 30" },
	{ "array 30 (inline size)", "local t={} for i=1,30 do t[i]=i end return t", "local v = ... return #v == 30 and v[30] == 30" },
	{ "array 31 (size header)", "local t={} for i=1,31 do t[i]=i end return t", "local v = ... return #v == 31 and v[31] == 31" },
	{ "array 32 (size header)", "local t={} for i=1,32 do t[i]=i end return t", "local v = ... return #v == 32 and v[32] == 32" },
	{ "hash table", "return {a=1,b='x',c=true}", "local v = ... return v.a == 1 and v.b == 'x' and v.c == true and next(v) ~= nil and #v == 0" },
	{ "sparse array", "local t={[3]='x'} return t", "local v = ... return v[3] == 'x' and v[1] == nil and v[2] == nil" },
	{ "integer key beyond array", "return {[10]=20}", "local v = ... return v[10] == 20 and v[1] == nil and v[9] == nil" },
	{ "mixed array and hash", "local t={'one'} t.a=1 return t", "local v = ... return #v == 1 and v[1] == 'one' and v.a == 1" },
	{ "float key in hash", "return {[1.5]='x'}", "local v = ... return v[1.5] == 'x'" },
	{ "nested tables", "return {1,{2,{3}}}", "local v = ... return v[1] == 1 and v[2][1] == 2 and v[2][2][1] == 3" },
	{ "empty table", "return {}", "local v = ... return #v == 0 and next(v) == nil" },
	/* Metapairs walker. */
	{ "metapairs table",
	  "local t={} setmetatable(t,{__pairs=function(t) local i=0 return function() i=i+1 if i<=2 then return 'k'..i, i*10 end end, t, nil end}) return t",
	  "local v = ... return v.k1 == 10 and v.k2 == 20 and #v == 0 and v.k3 == nil" },
	{ NULL, NULL, NULL },
};

int
main(void) {
	lua_State *L = luaL_newstate();
	assert(L);
	luaL_openlibs(L);

	char buf[64];

	/* Huge qword array_size (INT64_MAX): pre-fix would preallocate ~2^63
	 * slots and OOM.  With only the header present, remaining len is 0, so
	 * the bound check must reject it. */
	{
		int64_t huge = INT64_MAX;
		int n = build_table_stream(buf, TYPE_NUMBER_QWORD, &huge, sizeof(huge));
		expect_invalid(L, buf, n);
	}

	/* Huge dword array_size (INT32_MAX): same rejection path. */
	{
		int32_t huge = INT32_MAX;
		int n = build_table_stream(buf, TYPE_NUMBER_DWORD, &huge, sizeof(huge));
		expect_invalid(L, buf, n);
	}

	/* Negative array_size must also be rejected. */
	{
		int64_t neg = -1;
		int n = build_table_stream(buf, TYPE_NUMBER_QWORD, &neg, sizeof(neg));
		expect_invalid(L, buf, n);
	}

	/* A size-header table marker followed by a non-number cookie is an
	 * invalid stream: unpack_table reads the size type byte and requires
	 * (type & 7) == TYPE_NUMBER. */
	{
		buf[0] = COMBINE_TYPE(TYPE_TABLE, MAX_COOKIE - 1);
		buf[1] = COMBINE_TYPE(TYPE_BOOLEAN, 0);
		expect_invalid(L, buf, 2);
	}

	/* A real cookie is rejected too (array sizes must be integers). */
	{
		buf[0] = COMBINE_TYPE(TYPE_TABLE, MAX_COOKIE - 1);
		buf[1] = COMBINE_TYPE(TYPE_NUMBER, TYPE_NUMBER_REAL);
		expect_invalid(L, buf, 2);
	}

	/* Pack/unpack round trips over every value encoding. */
	for (const struct rt_case *c = rt_cases; c->name != NULL; c++) {
		roundtrip(L, c->value, c->check);
	}

	/* Light userdata round trips as a raw pointer. */
	{
		lua_pushlightuserdata(L, (void *)(uintptr_t)0x1234);
		lua_pushcfunction(L, luaseri_pack);
		lua_insert(L, -2);
		assert(lua_pcall(L, 1, 2, 0) == LUA_OK);
		void *ptr = lua_touserdata(L, -2);
		lua_pushcfunction(L, luaseri_unpack);
		lua_insert(L, -3);
		assert(lua_pcall(L, 2, 1, 0) == LUA_OK);
		skynet_free(ptr);
		assert(lua_islightuserdata(L, -1));
		assert(lua_touserdata(L, -1) == (void *)(uintptr_t)0x1234);
		lua_pop(L, 1);
	}

	/* Unsupported value types must raise, not silently truncate the
	 * stream. */
	expect_pack_error(L, "return {f=function() end}", "Unsupport type function");

	/* Tables nested deeper than MAX_DEPTH (32) must raise instead of
	 * recursing until the C stack overflows. */
	{
		char chunk[128];
		snprintf(chunk, sizeof(chunk),
			"local t={1} for i=1,%d do t={t} end return t", MAX_DEPTH + 1);
		expect_pack_error(L, chunk, "serialize can't pack too depth table");
	}

	/* A __pairs metamethod raising must propagate (wb_table_metapairs
	 * returns failure and pack_one re-raises the pending error). */
	expect_pack_error(L,
		"local t={} setmetatable(t,{__pairs=function() error('pairs boom') end}) return t",
		"pairs boom");

	/* The iterator returned by __pairs raising mid-walk must also
	 * propagate. */
	expect_pack_error(L,
		"local t={} setmetatable(t,{__pairs=function(t) return function() error('next boom') end, t, nil end}) return t",
		"next boom");

	printf("seri: all assertions passed\n");
	lua_close(L);
	return 0;
}
