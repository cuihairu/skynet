/* Unit test for lua-sharedata.
 *
 * Covers two areas:
 *
 * 1. A nested convtable failure must not leak.  setvalue() converts a nested
 *    table by calling convtable through lua_pcall.  If that conversion raises
 *    (e.g. an invalid float key in countsize), the partially built struct table
 *    -- and any descendants -- must be freed and ctx->tbl restored before the
 *    error propagates.  Previously the table was leaked and ctx->tbl left
 *    dangling.
 *
 * 2. The read API exported by luaopen_skynet_sharedata_core: index, nextkey,
 *    len, hashlen, box, needupdate, update, markdirty, isdirty, getref,
 *    incref, decref and delete, including their error paths.  The hash part of
 *    the test conf has exactly three entries, 100, 106 and "foo", so the
 *    integer keys 100 and 106 both hash to keyhash % 3 == 1 and exercise the
 *    colliding bucket chain (fillcolliding and lookup_key's next chain)
 *    deterministically rather than by hash luck.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <lauxlib.h>
#include <lua.h>

#include "../lualib-src/lua-sharedata.c"

/* Call sharedata.<fn> with `nargs` arguments already on the stack.
 * Returns the pcall status; on success nret results are left on the stack. */
static int
call_mod(lua_State *L, const char *fn, int nargs, int nret) {
	lua_getglobal(L, "sharedata");
	lua_getfield(L, -1, fn);
	lua_remove(L, -2);		// drop the sharedata table
	lua_insert(L, -(nargs + 1));	// move the function below its arguments
	return lua_pcall(L, nargs, nret, 0);
}

/* Assert that sharedata.<fn> raises an error; when `want` is non-NULL the
 * message must contain it.  The error message is removed from the stack. */
static void
expect_error(lua_State *L, const char *fn, int nargs, const char *want) {
	int rc = call_mod(L, fn, nargs, 1);
	assert(rc != LUA_OK);
	const char *msg = lua_tostring(L, -1);
	assert(msg != NULL);
	if (want != NULL)
		assert(strstr(msg, want) != NULL);
	lua_pop(L, 1);
}

/* Push a conf source table: array part [1..5] holding one value of each
 * scalar type plus a nested table, and a 3-entry hash part with colliding
 * integer keys. */
static void
push_conf(lua_State *L) {
	lua_createtable(L, 0, 4);

	lua_pushinteger(L, 10);
	lua_rawseti(L, -2, 1);
	lua_pushliteral(L, "hello");
	lua_rawseti(L, -2, 2);
	lua_pushboolean(L, 1);
	lua_rawseti(L, -2, 3);
	lua_pushnumber(L, 3.14);
	lua_rawseti(L, -2, 4);

	lua_createtable(L, 2, 0);
	lua_pushinteger(L, 100);
	lua_rawseti(L, -2, 1);
	lua_pushliteral(L, "nested");
	lua_rawseti(L, -2, 2);
	lua_rawseti(L, -2, 5);

	lua_pushliteral(L, "a");
	lua_rawseti(L, -2, 100);
	lua_pushliteral(L, "b");
	lua_rawseti(L, -2, 106);
	lua_pushliteral(L, "foo");
	lua_pushliteral(L, "bar");
	lua_rawset(L, -3);
}

int
main(void) {
	lua_State *L = luaL_newstate();
	assert(L);

	luaL_requiref(L, "sharedata", luaopen_skynet_sharedata_core, 1);
	lua_pop(L, 1);

	/* --- 1. nested convtable failure must not leak --------------------- */

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

	/* --- 2. read API ---------------------------------------------------- */

	push_conf(L);
	lua_setglobal(L, "CONF");

	/* new() rejects a non-table argument */
	lua_pushinteger(L, 42);
	expect_error(L, "new", 1, NULL);

	lua_getglobal(L, "CONF");
	assert(call_mod(L, "new", 1, 1) == LUA_OK);
	void *conf = (void *)lua_touserdata(L, -1);
	assert(conf != NULL);
	lua_pop(L, 1);

	/* a second, distinct conf object is what update() points to */
	lua_getglobal(L, "CONF");
	assert(call_mod(L, "new", 1, 1) == LUA_OK);
	void *conf2 = (void *)lua_touserdata(L, -1);
	assert(conf2 != NULL && conf2 != conf);
	lua_pop(L, 1);

	/* length accessors */
	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "len", 1, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 5);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "hashlen", 1, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 3);
	lua_pop(L, 1);

	/* array values, one per scalar type */
	lua_pushlightuserdata(L, conf);
	lua_pushinteger(L, 1);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_isinteger(L, -1) && lua_tointeger(L, -1) == 10);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	lua_pushinteger(L, 2);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(strcmp(lua_tostring(L, -1), "hello") == 0);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	lua_pushinteger(L, 3);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_isboolean(L, -1) && lua_toboolean(L, -1) == 1);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	lua_pushinteger(L, 4);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(!lua_isinteger(L, -1));
	assert(lua_tonumber(L, -1) > 3.1399 && lua_tonumber(L, -1) < 3.1401);
	lua_pop(L, 1);

	/* a table value is reported as a lightuserdata handle */
	lua_pushlightuserdata(L, conf);
	lua_pushinteger(L, 5);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_islightuserdata(L, -1) && lua_touserdata(L, -1) != NULL);
	lua_pop(L, 1);

	/* hash part: both colliding integer keys are reachable */
	lua_pushlightuserdata(L, conf);
	lua_pushinteger(L, 100);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(strcmp(lua_tostring(L, -1), "a") == 0);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	lua_pushinteger(L, 106);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(strcmp(lua_tostring(L, -1), "b") == 0);
	lua_pop(L, 1);

	/* string key */
	lua_pushlightuserdata(L, conf);
	lua_pushliteral(L, "foo");
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(strcmp(lua_tostring(L, -1), "bar") == 0);
	lua_pop(L, 1);

	/* misses: nil key, array slot 0, absent integer and absent string keys */
	lua_pushlightuserdata(L, conf);
	lua_pushnil(L);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_isnil(L, -1));
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	lua_pushinteger(L, 0);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_isnil(L, -1));
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	lua_pushinteger(L, 300);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_isnil(L, -1));
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	lua_pushliteral(L, "zz");
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_isnil(L, -1));
	lua_pop(L, 1);

	/* index rejects a float key */
	lua_pushlightuserdata(L, conf);
	lua_pushnumber(L, 1.5);
	expect_error(L, "index", 2, "Invalid key");

	/* index rejects a first argument that is not a conf object */
	lua_pushinteger(L, 42);
	expect_error(L, "index", 1, "Need a conf object");

	/* nextkey: the first call yields the first array slot */
	lua_pushlightuserdata(L, conf);
	lua_pushnil(L);
	assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
	assert(lua_isinteger(L, -1) && lua_tointeger(L, -1) == 1);
	lua_pop(L, 1);

	/* nextkey walks every key exactly once: 5 array entries plus the 3 hash
	 * entries, in array order then hash slot order. */
	int keys[8];
	int seen = 1;
	keys[0] = 1;
	int nstr = 0;
	int cur_int = 1;
	int cur_str = 0;
	int stop = 0;
	while (!stop) {
		lua_pushlightuserdata(L, conf);
		if (cur_str)
			lua_pushliteral(L, "foo");
		else
			lua_pushinteger(L, cur_int);
		assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
		if (lua_isnil(L, -1)) {
			stop = 1;
			lua_pop(L, 1);
			continue;
		}
		if (lua_isinteger(L, -1)) {
			assert(seen < 8);
			keys[seen++] = (int)lua_tointeger(L, -1);
			cur_int = (int)lua_tointeger(L, -1);
			cur_str = 0;
		} else {
			assert(strcmp(lua_tostring(L, -1), "foo") == 0);
			assert(seen < 8);
			keys[seen++] = 0;		/* marker for the string key */
			nstr = 1;
			cur_str = 1;
		}
		lua_pop(L, 1);
	}
	assert(seen == 8 && nstr == 1);
	assert(keys[1] == 2 && keys[2] == 3 && keys[3] == 4 && keys[4] == 5);
	int hits = 0;
	for (int i = 5; i < 8; i++) {
		if (keys[i] == 100 || keys[i] == 106)
			hits++;
	}
	assert(hits == 2);

	/* nextkey from a hash key returns either the next key or nil */
	lua_pushlightuserdata(L, conf);
	lua_pushinteger(L, 100);
	assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
	lua_pop(L, 1);

	/* nextkey rejects a non-integer number key and an absent integer key */
	lua_pushlightuserdata(L, conf);
	lua_pushnumber(L, 1.5);
	assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
	assert(lua_isnil(L, -1));
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	lua_pushinteger(L, 999);
	assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
	assert(lua_isnil(L, -1));
	lua_pop(L, 1);

	/* reference counting starts clean and increments / decrements by one */
	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "getref", 1, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 0);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "incref", 1, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 1);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "incref", 1, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 2);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "decref", 1, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 1);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "decref", 1, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 0);
	lua_pop(L, 1);

	/* dirty flag */
	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "isdirty", 1, 1) == LUA_OK);
	assert(lua_toboolean(L, -1) == 0);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "markdirty", 1, 0) == LUA_OK);

	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "isdirty", 1, 1) == LUA_OK);
	assert(lua_toboolean(L, -1) == 1);
	lua_pop(L, 1);

	/* box / needupdate / update */
	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "box", 1, 1) == LUA_OK);
	assert(lua_isuserdata(L, -1) && !lua_islightuserdata(L, -1));
	lua_pushvalue(L, -1);
	lua_setglobal(L, "CTRL");
	lua_pop(L, 1);

	/* boxing takes a reference on the conf state */
	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "getref", 1, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 1);
	lua_pop(L, 1);

	/* no update registered yet */
	lua_getglobal(L, "CTRL");
	assert(call_mod(L, "needupdate", 1, 1) == LUA_OK);
	assert(lua_isnil(L, -1));
	lua_pop(L, 1);

	/* register an update pointing at a different conf object */
	lua_getglobal(L, "CTRL");
	lua_pushlightuserdata(L, conf2);
	lua_getglobal(L, "CONF");
	assert(call_mod(L, "update", 3, 0) == LUA_OK);

	lua_getglobal(L, "CTRL");
	assert(call_mod(L, "needupdate", 1, 2) == LUA_OK);
	assert(lua_islightuserdata(L, -2) && lua_touserdata(L, -2) == conf2);
	assert(lua_istable(L, -1));
	lua_rawequal(L, -1, lua_getglobal(L, "CONF"));
	assert(lua_toboolean(L, -1) == 1);
	lua_pop(L, 2);

	/* updating with the ctrl's own root is rejected */
	lua_getglobal(L, "CTRL");
	lua_pushlightuserdata(L, conf);
	lua_getglobal(L, "CONF");
	expect_error(L, "update", 3, "You should update");

	/* update validates the type of every argument */
	lua_pushinteger(L, 42);
	lua_pushlightuserdata(L, conf2);
	lua_getglobal(L, "CONF");
	expect_error(L, "update", 3, NULL);

	/* dropping the ctrl runs releaseobj, which releases the box reference */
	lua_pushnil(L);
	lua_setglobal(L, "CTRL");
	lua_gc(L, LUA_GCCOLLECT, 0);

	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "getref", 1, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 0);
	lua_pop(L, 1);

	/* delete validates its argument */
	lua_pushinteger(L, 42);
	expect_error(L, "delete", 1, "Need a conf object");

	/* a conf whose array part has a nil gap: index reports nil for the empty
	 * slot and nextkey skips it.  Which length the interpreter reports as
	 * the array border decides whether slot 3 is array or hash, so branch on
	 * the observed length rather than assuming one. */
	lua_createtable(L, 0, 2);
	lua_pushinteger(L, 1);
	lua_rawseti(L, -2, 1);
	lua_pushinteger(L, 3);
	lua_rawseti(L, -2, 3);
	assert(call_mod(L, "new", 1, 1) == LUA_OK);
	void *gap = (void *)lua_touserdata(L, -1);
	assert(gap != NULL);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, gap);
	lua_pushinteger(L, 1);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 1);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, gap);
	assert(call_mod(L, "len", 1, 1) == LUA_OK);
	int glen = (int)lua_tointeger(L, -1);
	lua_pop(L, 1);
	assert(glen == 1 || glen == 3);

	if (glen == 3) {
		/* slot 2 stayed NIL */
		lua_pushlightuserdata(L, gap);
		lua_pushinteger(L, 2);
		assert(call_mod(L, "index", 2, 1) == LUA_OK);
		assert(lua_isnil(L, -1));
		lua_pop(L, 1);

		lua_pushlightuserdata(L, gap);
		lua_pushinteger(L, 3);
		assert(call_mod(L, "index", 2, 1) == LUA_OK);
		assert(lua_tointeger(L, -1) == 3);
		lua_pop(L, 1);

		/* nextkey skips the nil slot, then terminates on the array end */
		lua_pushlightuserdata(L, gap);
		lua_pushinteger(L, 1);
		assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
		assert(lua_tointeger(L, -1) == 3);
		lua_pop(L, 1);

		lua_pushlightuserdata(L, gap);
		lua_pushinteger(L, 3);
		assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
		assert(lua_isnil(L, -1));
		lua_pop(L, 1);
	} else {
		/* key 3 went to the hash part, which then holds a single entry */
		lua_pushlightuserdata(L, gap);
		assert(call_mod(L, "hashlen", 1, 1) == LUA_OK);
		assert(lua_tointeger(L, -1) == 1);
		lua_pop(L, 1);

		lua_pushlightuserdata(L, gap);
		lua_pushinteger(L, 3);
		assert(call_mod(L, "index", 2, 1) == LUA_OK);
		assert(lua_tointeger(L, -1) == 3);
		lua_pop(L, 1);

		/* an integer key outside the array border falls through to the hash
		 * part and is found there */
		lua_pushlightuserdata(L, gap);
		lua_pushinteger(L, 1);
		assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
		assert(lua_tointeger(L, -1) == 3);
		lua_pop(L, 1);

		lua_pushlightuserdata(L, gap);
		lua_pushinteger(L, 3);
		assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
		assert(lua_isnil(L, -1));
		lua_pop(L, 1);
	}

	lua_pushlightuserdata(L, gap);

	/* a conf with no hash part at all: index on a string key hits the
	 * sizehash == 0 fast path and nextkey stops at the array end. */
	lua_createtable(L, 2, 0);
	lua_pushboolean(L, 1);
	lua_rawseti(L, -2, 1);
	lua_pushboolean(L, 0);
	lua_rawseti(L, -2, 2);
	assert(call_mod(L, "new", 1, 1) == LUA_OK);
	void *plain = (void *)lua_touserdata(L, -1);
	assert(plain != NULL);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, plain);
	assert(call_mod(L, "hashlen", 1, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 0);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, plain);
	lua_pushliteral(L, "x");
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_isnil(L, -1));
	lua_pop(L, 1);

	lua_pushlightuserdata(L, plain);
	lua_pushinteger(L, 1);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_isboolean(L, -1) && lua_toboolean(L, -1) == 1);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, plain);
	lua_pushinteger(L, 2);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_isboolean(L, -1) && lua_toboolean(L, -1) == 0);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, plain);
	lua_pushinteger(L, 1);
	assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 2);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, plain);
	lua_pushinteger(L, 2);
	assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
	assert(lua_isnil(L, -1));
	lua_pop(L, 1);

	lua_pushlightuserdata(L, plain);
	assert(call_mod(L, "delete", 1, 0) == LUA_OK);

	/* a conf built from a hash-only table: the first nextkey call reports the
	 * first hash key because there is no array part at all. */
	lua_createtable(L, 0, 1);
	lua_pushliteral(L, "k");
	lua_pushliteral(L, "v");
	lua_rawset(L, -3);
	assert(call_mod(L, "new", 1, 1) == LUA_OK);
	void *hashonly = (void *)lua_touserdata(L, -1);
	assert(hashonly != NULL);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, hashonly);
	assert(call_mod(L, "hashlen", 1, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 1);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, hashonly);
	assert(call_mod(L, "len", 1, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 0);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, hashonly);
	lua_pushliteral(L, "k");
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(strcmp(lua_tostring(L, -1), "v") == 0);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, hashonly);
	lua_pushnil(L);
	assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
	assert(strcmp(lua_tostring(L, -1), "k") == 0);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, hashonly);
	assert(call_mod(L, "delete", 1, 0) == LUA_OK);

	/* a nested table stored in the hash part must be freed recursively */
	lua_createtable(L, 0, 1);
	lua_pushliteral(L, "k");
	lua_createtable(L, 2, 0);
	lua_pushinteger(L, 1);
	lua_rawseti(L, -2, 1);
	lua_pushliteral(L, "n");
	lua_rawseti(L, -2, 2);
	lua_rawset(L, -3);
	assert(call_mod(L, "new", 1, 1) == LUA_OK);
	void *hashnested = (void *)lua_touserdata(L, -1);
	assert(hashnested != NULL);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, hashnested);
	lua_pushliteral(L, "k");
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_islightuserdata(L, -1) && lua_touserdata(L, -1) != NULL);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, hashnested);
	assert(call_mod(L, "delete", 1, 0) == LUA_OK);

	/* an integer key that hashes into the colliding bucket but is not present
	 * walks the chain to its end and misses. */
	lua_pushlightuserdata(L, conf);
	lua_pushinteger(L, 103);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_isnil(L, -1));
	lua_pop(L, 1);

	/* a nil array slot (a gap) is reported as nil and skipped by nextkey.
	 * Whether the interpreter keeps the border at 5 depends on its border
	 * search, so branch on the observed length. */
	lua_createtable(L, 5, 0);
	for (int i = 1; i <= 5; i++) {
		lua_pushinteger(L, i);
		lua_rawseti(L, -2, i);
	}
	lua_pushnil(L);
	lua_rawseti(L, -2, 3);
	assert(call_mod(L, "new", 1, 1) == LUA_OK);
	void *nilgap = (void *)lua_touserdata(L, -1);
	assert(nilgap != NULL);
	lua_pop(L, 1);

	lua_pushlightuserdata(L, nilgap);
	assert(call_mod(L, "len", 1, 1) == LUA_OK);
	int nlen = (int)lua_tointeger(L, -1);
	lua_pop(L, 1);
	assert(nlen == 5 || nlen == 2);

	lua_pushlightuserdata(L, nilgap);
	lua_pushinteger(L, 1);
	assert(call_mod(L, "index", 2, 1) == LUA_OK);
	assert(lua_tointeger(L, -1) == 1);
	lua_pop(L, 1);

	if (nlen == 5) {
		lua_pushlightuserdata(L, nilgap);
		lua_pushinteger(L, 3);
		assert(call_mod(L, "index", 2, 1) == LUA_OK);
		assert(lua_isnil(L, -1));
		lua_pop(L, 1);

		lua_pushlightuserdata(L, nilgap);
		lua_pushinteger(L, 4);
		assert(call_mod(L, "index", 2, 1) == LUA_OK);
		assert(lua_tointeger(L, -1) == 4);
		lua_pop(L, 1);

		/* nextkey skips the empty slot */
		lua_pushlightuserdata(L, nilgap);
		lua_pushinteger(L, 2);
		assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
		assert(lua_tointeger(L, -1) == 4);
		lua_pop(L, 1);

		lua_pushlightuserdata(L, nilgap);
		lua_pushinteger(L, 5);
		assert(call_mod(L, "nextkey", 2, 1) == LUA_OK);
		assert(lua_isnil(L, -1));
		lua_pop(L, 1);
	} else {
		/* keys 4 and 5 went to the hash part */
		lua_pushlightuserdata(L, nilgap);
		assert(call_mod(L, "hashlen", 1, 1) == LUA_OK);
		assert(lua_tointeger(L, -1) == 2);
		lua_pop(L, 1);

		lua_pushlightuserdata(L, nilgap);
		lua_pushinteger(L, 4);
		assert(call_mod(L, "index", 2, 1) == LUA_OK);
		assert(lua_tointeger(L, -1) == 4);
		lua_pop(L, 1);

		lua_pushlightuserdata(L, nilgap);
		lua_pushinteger(L, 5);
		assert(call_mod(L, "index", 2, 1) == LUA_OK);
		assert(lua_tointeger(L, -1) == 5);
		lua_pop(L, 1);
	}

	lua_pushlightuserdata(L, nilgap);
	assert(call_mod(L, "delete", 1, 0) == LUA_OK);

	/* a value of an unsupported type raises out of new() */
	lua_createtable(L, 1, 0);
	lua_pushcfunction(L, convtable);
	lua_rawseti(L, -2, 1);
	expect_error(L, "new", 1, "Unsupport value type");

	/* a key of an unsupported type raises out of new() */
	lua_createtable(L, 0, 1);
	lua_pushboolean(L, 1);
	lua_pushliteral(L, "x");
	lua_rawset(L, -3);
	expect_error(L, "new", 1, "Invalid key type");

	lua_pushlightuserdata(L, gap);
	assert(call_mod(L, "delete", 1, 0) == LUA_OK);
	lua_pushlightuserdata(L, conf2);
	assert(call_mod(L, "delete", 1, 0) == LUA_OK);
	lua_pushlightuserdata(L, conf);
	assert(call_mod(L, "delete", 1, 0) == LUA_OK);

	printf("sharedata: all assertions passed\n");
	lua_close(L);
	return 0;
}
