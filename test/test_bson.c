/* Unit test for lua-bson: reject negative field lengths before they desync
 * the reader or drive OOB reads.
 *
 * A crafted BSON document can encode a negative int32 length for BINARY,
 * DBPOINTER, JSCODE/SYMBOL and CODEWS fields.  read_bytes previously moved
 * the reader pointer backwards and grew its size (reader desync), enabling
 * out-of-bounds reads; the sz+12 / sz-4 offsets could also mask a negative
 * length into a small positive one.  The fix rejects any negative length at
 * the read_bytes source and at each call site before the offset arithmetic.
 *
 * This test drives bson.decode (via a raw string) and bson:makeindex (via a
 * crafted userdata) and asserts each malformed document raises an error
 * rather than reading out of bounds.
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "../lualib-src/lua-bson.c"

/* Append a 4-byte little-endian int32. */
static void
put_i32(uint8_t *p, int32_t v) {
	p[0] = (uint8_t)v;
	p[1] = (uint8_t)(v >> 8);
	p[2] = (uint8_t)(v >> 16);
	p[3] = (uint8_t)(v >> 24);
}

/* Build a top-level BSON document: [len][type][key\0][payload...][end=0].
 * payload is the field data after the key. */
static int
build_doc(uint8_t *buf, uint8_t type, const char *key, int32_t payload_len,
	const uint8_t *payload) {
	int klen = (int)strlen(key) + 1;
	int n = 0;
	/* placeholder length */
	put_i32(buf + n, 0);
	n += 4;
	buf[n++] = type;
	memcpy(buf + n, key, klen);
	n += klen;
	if (payload_len > 0) {
		memcpy(buf + n, payload, payload_len);
	}
	n += payload_len;
	buf[n++] = 0;
	put_i32(buf, n);
	return n;
}

/* Push a userdata holding `len` bytes with the bson metatable, then call the
 * given C function.  Returns the pcall status. */
static int
call_c(lua_State *L, lua_CFunction fn, const uint8_t *buf, int len, int nres) {
	void * ud = lua_newuserdatauv(L, len, 1);
	memcpy(ud, buf, len);
	luaL_getmetatable(L, "bson");
	lua_setmetatable(L, -2);
	lua_pushcfunction(L, fn);
	lua_insert(L, -2);
	int rc = lua_pcall(L, 1, nres, 0);
	if (rc != LUA_OK) {
		lua_pop(L, 1); /* error msg */
	}
	return rc;
}

int
main(void) {
	lua_State *L = luaL_newstate();
	assert(L);
	luaL_openlibs(L);

	lua_getglobal(L, "bson"); /* not yet loaded */
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		lua_pushcfunction(L, luaopen_bson);
		assert(lua_pcall(L, 0, 1, 0) == LUA_OK);
		lua_setglobal(L, "bson");
	}
	/* luaopen_bson does not register the "bson" metatable (that happens in
	 * lencode).  Register it once via bson_meta on a throwaway userdata. */
	{
		lua_newuserdatauv(L, 0, 0);
		bson_meta(L);
		lua_pop(L, 1);
	}

	uint8_t buf[256];
	uint8_t payload[4];

	/* BSON_BINARY with sz = -1: read_bytes(-1) moved the reader backwards. */
	put_i32(payload, -1);
	{
		int n = build_doc(buf, BSON_BINARY, "k", 4, payload);
		assert(call_c(L, ldecode, buf, n, 0) == LUA_ERRRUN);
	}

	/* BSON_DBPOINTER with sz = -1: sz+12 masked the negative to 11. */
	put_i32(payload, -1);
	{
		int n = build_doc(buf, BSON_DBPOINTER, "k", 4, payload);
		assert(call_c(L, ldecode, buf, n, 0) == LUA_ERRRUN);
	}

	/* BSON_JSCODE with sz = -1. */
	put_i32(payload, -1);
	{
		int n = build_doc(buf, BSON_JSCODE, "k", 4, payload);
		assert(call_c(L, ldecode, buf, n, 0) == LUA_ERRRUN);
	}

	/* BSON_SYMBOL with sz = -1. */
	put_i32(payload, -1);
	{
		int n = build_doc(buf, BSON_SYMBOL, "k", 4, payload);
		assert(call_c(L, ldecode, buf, n, 0) == LUA_ERRRUN);
	}

	/* BSON_CODEWS with sz = 1: sz-4 = -3 moved the reader backwards. */
	put_i32(payload, 1);
	{
		int n = build_doc(buf, BSON_CODEWS, "k", 4, payload);
		assert(call_c(L, ldecode, buf, n, 0) == LUA_ERRRUN);
	}

	/* A valid document still decodes (regression guard). */
	{
		put_i32(payload, 42);
		int n = build_doc(buf, BSON_INT32, "k", 4, payload);
		assert(call_c(L, ldecode, buf, n, 1) == LUA_OK);
		assert(lua_istable(L, -1));
		lua_getfield(L, -1, "k");
		assert(lua_tointeger(L, -1) == 42);
		lua_pop(L, 2);
	}

	/* makeindex must reject a BSON_STRING length of 0 (unpack_dict uses
	 * sz <= 0, but lmakeindex previously accepted sz == 0). */
	{
		put_i32(payload, 0);
		int n = build_doc(buf, BSON_STRING, "k", 4, payload);
		assert(call_c(L, lmakeindex, buf, n, 1) == LUA_ERRRUN);
	}

	/* makeindex must not overflow on an INT32_MAX-length DBPOINTER (sz+12)
	 * or BINARY (sz+1): the size_t arithmetic must reject it before it can
	 * wrap around to a small positive read. */
	{
		put_i32(payload, INT32_MAX);
		int n = build_doc(buf, BSON_DBPOINTER, "k", 4, payload);
		assert(call_c(L, lmakeindex, buf, n, 1) == LUA_ERRRUN);
	}
	{
		put_i32(payload, INT32_MAX);
		int n = build_doc(buf, BSON_BINARY, "k", 4, payload);
		assert(call_c(L, lmakeindex, buf, n, 1) == LUA_ERRRUN);
	}

	printf("bson: all assertions passed\n");
	lua_close(L);
	return 0;
}
