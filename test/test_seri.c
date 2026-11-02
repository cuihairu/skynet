/* Unit test for lua-seri: bound decoded array_size before preallocating.
 *
 * A remote stream encodes a table header with a huge array_size (e.g.
 * INT64_MAX / INT32_MAX).  unpack_table previously forwarded that value
 * straight to lua_createtable, which preallocates the array part and can
 * trigger an OOM.  The fix rejects any array_size that cannot fit in the
 * remaining stream (each array element needs at least one byte) before it
 * reaches lua_createtable.
 *
 * This test drives luaseri_unpack directly and asserts:
 *   - a huge array_size stream is rejected as an invalid stream
 *   - a legitimate table round-trips through pack/unpack unchanged
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lauxlib.h>
#include <lua.h>

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

int
main(void) {
	lua_State *L = luaL_newstate();
	assert(L);

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

	/* A legitimate array table round-trips: pack {10,20,30} then unpack. */
	{
		lua_newtable(L);
		lua_pushinteger(L, 10); lua_rawseti(L, -2, 1);
		lua_pushinteger(L, 20); lua_rawseti(L, -2, 2);
		lua_pushinteger(L, 30); lua_rawseti(L, -2, 3);

		lua_pushcfunction(L, luaseri_pack);
		lua_insert(L, -2);
		assert(lua_pcall(L, 1, 2, 0) == LUA_OK);
		assert(lua_islightuserdata(L, -2));
		assert(lua_isinteger(L, -1));

		lua_pushcfunction(L, luaseri_unpack);
		lua_insert(L, -3);
		assert(lua_pcall(L, 2, 1, 0) == LUA_OK);
		assert(lua_istable(L, -1));
		assert(lua_rawlen(L, -1) == 3);
		for (int i = 1; i <= 3; i++) {
			lua_rawgeti(L, -1, i);
			assert(lua_tointeger(L, -1) == i * 10);
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}

	printf("seri: all assertions passed\n");
	lua_close(L);
	return 0;
}
