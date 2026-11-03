/* Unit test for lua-sharedata: nested convtable failure must not leak.
 *
 * setvalue() converts a nested table by calling convtable through lua_call.
 * If that conversion raises (e.g. an invalid float key in countsize), the
 * partially built struct table -- and any descendants -- must be freed and
 * ctx->tbl restored before the error propagates.  Previously the table was
 * leaked and ctx->tbl left dangling.
 */
#include <assert.h>
#include <stdio.h>

#include <lauxlib.h>
#include <lua.h>

#include "../lualib-src/lua-sharedata.c"

int
main(void) {
	lua_State *L = luaL_newstate();
	assert(L);

	luaL_requiref(L, "sharedata", luaopen_skynet_sharedata_core, 1);
	lua_pop(L, 1);

	/* Build { { [1.5] = "x" } }.  The inner table has a float key, which
	 * makes countsize() raise inside the nested convtable. */
	lua_getglobal(L, "sharedata");
	lua_getfield(L, -1, "new");
	lua_createtable(L, 1, 0);
	lua_createtable(L, 0, 1);
	lua_pushnumber(L, 1.5);
	lua_pushliteral(L, "x");
	lua_rawset(L, -3);
	lua_rawseti(L, -2, 1);

	int rc = lua_pcall(L, 1, 0, 0);
	assert(rc == LUA_ERRRUN);
	lua_pop(L, 1);

	/* The module function table remains usable after the error. */
	lua_getfield(L, -1, "new");
	lua_createtable(L, 0, 0);
	rc = lua_pcall(L, 1, 1, 0);
	assert(rc == LUA_OK);
	lua_pop(L, 1);

	printf("sharedata: all assertions passed\n");
	lua_close(L);
	return 0;
}
