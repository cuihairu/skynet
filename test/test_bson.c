/* Unit test for lua-bson: reject negative field lengths before they desync
 * the reader, and cover the encode side through encode/decode round trips.
 *
 * A crafted BSON document can encode a negative int32 length for BINARY,
 * DBPOINTER, JSCODE/SYMBOL and CODEWS fields.  read_bytes previously moved
 * the reader pointer backwards and grew its size (reader desync), enabling
 * out-of-bounds reads; the sz+12 / sz-4 offsets could also mask a negative
 * length into a small positive one.  The fix rejects any negative length at
 * the read_bytes source and at each call site before the offset arithmetic.
 *
 * This test drives bson.decode (via a raw string), bson:makeindex (via a
 * crafted userdata) and bson.encode / bson.encode_order (via the exported
 * Lua API), asserting malformed documents raise errors rather than reading
 * out of bounds and that every encodable value round-trips unchanged.
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

/* Encode the value produced by the `value` chunk with bson.encode, decode it
 * back and run the `check` chunk on the decoded table. */
static void
enc_roundtrip(lua_State *L, const char *value, const char *check) {
	assert(luaL_dostring(L, value) == LUA_OK);
	lua_getglobal(L, "bson");
	lua_getfield(L, -1, "encode");
	lua_remove(L, -2);
	lua_insert(L, -2);
	assert(lua_pcall(L, 1, 1, 0) == LUA_OK);
	lua_getfield(L, -1, "decode");
	lua_insert(L, -2);
	assert(lua_pcall(L, 1, 1, 0) == LUA_OK);
	assert(luaL_loadstring(L, check) == LUA_OK);
	lua_insert(L, -2);
	assert(lua_pcall(L, 1, 1, 0) == LUA_OK);
	assert(lua_toboolean(L, -1) == 1);
	lua_pop(L, 1);
}

/* Assert encoding the value produced by `value` raises an error containing
 * `msg`. */
static void
expect_encode_error(lua_State *L, const char *value, const char *msg) {
	assert(luaL_dostring(L, value) == LUA_OK);
	lua_getglobal(L, "bson");
	lua_getfield(L, -1, "encode");
	lua_remove(L, -2);
	lua_insert(L, -2);
	assert(lua_pcall(L, 1, 0, 0) == LUA_ERRRUN);
	assert(strstr(lua_tostring(L, -1), msg) != NULL);
	lua_pop(L, 1);
}

/* Call a bson member with `nargs` pushed arguments (below nothing), leaving
 * the results on the stack.  Asserts success. */
static void
call_bson(lua_State *L, const char *name, int nargs, int nres) {
	lua_getglobal(L, "bson");
	lua_getfield(L, -1, name);
	lua_remove(L, -2);
	lua_insert(L, -(nargs + 1));
	assert(lua_pcall(L, nargs, nres, 0) == LUA_OK);
}

/* Call a bson member with `nargs` pushed arguments and assert the error
 * message contains `msg`. */
static void
expect_bson_error(lua_State *L, const char *name, int nargs, const char *msg) {
	lua_getglobal(L, "bson");
	lua_getfield(L, -1, name);
	lua_remove(L, -2);
	lua_insert(L, -(nargs + 1));
	assert(lua_pcall(L, nargs, 0, 0) == LUA_ERRRUN);
	assert(strstr(lua_tostring(L, -1), msg) != NULL);
	lua_pop(L, 1);
}

struct enc_case {
	const char *name;
	const char *value;
	const char *check;
};

static const struct enc_case enc_cases[] = {
	/* Numbers: int32, int32 boundaries, int64 (out of int32 range) and
	 * doubles. */
	{ "int32", "return {a=1}", "local v = ... return v.a == 1" },
	{ "int32 max", "return {a=2147483647}", "local v = ... return v.a == 2147483647" },
	{ "int64 beyond int32", "return {a=2147483648}", "local v = ... return v.a == 2147483648" },
	{ "int64 negative", "return {a=-2147483649}", "local v = ... return v.a == -2147483649" },
	{ "double", "return {a=1.5}", "local v = ... return v.a == 1.5" },
	/* Strings, booleans and a utf8 multi-byte key (utf8_copy multi-byte
	 * path). */
	{ "string", "return {a='hello'}", "local v = ... return v.a == 'hello'" },
	{ "empty string", "return {a=''}", "local v = ... return v.a == ''" },
	{ "booleans", "return {a=true,b=false}", "local v = ... return v.a == true and v.b == false" },
	{ "utf8 key", "return {['héllo']=1}", "local v = ... return v['héllo'] == 1" },
	/* Tables: arrays, nesting, dicts with mixed value types, empty. */
	{ "array", "return {list={1,2,3}}", "local v = ... return #v.list == 3 and v.list[1] == 1 and v.list[3] == 3" },
	{ "nested documents", "return {a={b={c=1}}}", "local v = ... return v.a.b.c == 1" },
	{ "mixed dict", "return {a=1,b='x',c={d=true}}", "local v = ... return v.a == 1 and v.b == 'x' and v.c.d == true" },
	{ "empty dict", "return {e={}}", "local v = ... return next(v.e) == nil" },
	/* Array longer than MAX_NUMBER (1024): bson_numstr falls back to
	 * sprintf for indexes >= 1024. */
	{ "array beyond numstr cache",
	  "local t={} for i=1,1100 do t[i]=i end return {list=t}",
	  "local v = ... return #v.list == 1100 and v.list[1] == 1 and v.list[1024] == 1024 and v.list[1100] == 1100" },
	/* Extended type objects: date, int64, timestamp, regex, binary,
	 * objectid and the passthrough minkey/maxkey/null strings. */
	{ "date",
	  "return {d=bson.date(456)}",
	  "local v = ... return #v.d == 6 and v.d:byte(1) == 0 and v.d:byte(2) == 9 and v.d:sub(3):byte(1) == 200" },
	{ "int64 object", "return {i=bson.int64(1099511627776)}", "local v = ... return v.i == 1099511627776" },
	{ "timestamp",
	  "return {t=bson.timestamp(123,7)}",
	  "local v = ... return #v.t == 10 and v.t:byte(2) == 17 and v.t:byte(3) == 7 and v.t:byte(7) == 123" },
	{ "regex",
	  "return {r=bson.regex('pat','i')}",
	  "local v = ... return #v.r == 8 and v.r:byte(2) == 11 and v.r:sub(3,5) == 'pat' and v.r:byte(6) == 0 and v.r:sub(7,7) == 'i' and v.r:byte(8) == 0" },
	{ "binary",
	  "return {b=bson.binary('data')}",
	  "local v = ... return #v.b == 7 and v.b:byte(2) == 5 and v.b:byte(3) == 0 and v.b:sub(4) == 'data'" },
	{ "objectid",
	  "return {o=bson.objectid()}",
	  "local v = ... return #v.o == 14 and v.o:byte(1) == 0 and v.o:byte(2) == 7" },
	{ "null passthrough", "return {n=bson.null}", "local v = ... return v.n == bson.null" },
	{ "minkey passthrough", "return {m=bson.minkey}", "local v = ... return v.m == bson.minkey" },
	{ "maxkey passthrough", "return {x=bson.maxkey}", "local v = ... return v.x == bson.maxkey" },
	/* A table with a __pairs metamethod walks pack_meta_dict. */
	{ "metapairs dict",
	  "local t={} setmetatable(t,{__pairs=function(t) local i=0 return function() i=i+1 if i<=2 then return 'k'..i, i*10 end end, t, nil end}) return {m=t}",
	  "local v = ... return v.m.k1 == 10 and v.m.k2 == 20" },
	{ NULL, NULL, NULL },
};

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

	/* makeindex must reject a top-level document whose declared length is
	 * < 5 (no room for the trailing NUL and at least one byte): get_length
	 * - 5 yields a negative br->size that previously cast to a huge size_t
	 * in check_reader and let read_byte/read_cstring read out of bounds. */
	{
		/* length 4: only the length field, no trailing NUL. */
		uint8_t tiny[4] = { 4, 0, 0, 0 };
		assert(call_c(L, lmakeindex, tiny, sizeof(tiny), 1) == LUA_ERRRUN);
	}
	{
		/* length 0: empty prefix. */
		uint8_t zero[4] = { 0, 0, 0, 0 };
		assert(call_c(L, lmakeindex, zero, sizeof(zero), 1) == LUA_ERRRUN);
	}
	{
		/* negative declared length. */
		uint8_t neg[4];
		put_i32(neg, -1);
		assert(call_c(L, lmakeindex, neg, sizeof(neg), 1) == LUA_ERRRUN);
	}

	/* Encode side: every encodable value round-trips. */
	for (const struct enc_case *c = enc_cases; c->name != NULL; c++) {
		enc_roundtrip(L, c->value, c->check);
	}

	/* encode_order pairs keys and values in argument order. */
	{
		lua_pushstring(L, "k1");
		lua_pushinteger(L, 1);
		lua_pushstring(L, "k2");
		lua_pushstring(L, "v");
		call_bson(L, "encode_order", 4, 1);
		lua_getfield(L, -1, "decode");
		lua_insert(L, -2);
		assert(lua_pcall(L, 1, 1, 0) == LUA_OK);
		lua_getfield(L, -1, "k1");
		assert(lua_tointeger(L, -1) == 1);
		lua_pop(L, 1);
		lua_getfield(L, -1, "k2");
		assert(strcmp(lua_tostring(L, -1), "v") == 0);
		lua_pop(L, 2);
	}

	/* bson.type maps plain values and extended objects to type names. */
	{
		lua_pushinteger(L, 5);
		call_bson(L, "type", 1, 2);
		assert(lua_isstring(L, -2));
		assert(lua_tointeger(L, -1) == 5);
		lua_pop(L, 2);
	}
	{
		/* A date object maps to ("date", seconds). */
		assert(luaL_dostring(L, "return bson.date(456)") == LUA_OK);
		call_bson(L, "type", 1, 2);
		assert(strcmp(lua_tostring(L, -2), "date") == 0);
		assert(lua_tointeger(L, -1) == 456);
		lua_pop(L, 2);
	}
	{
		/* A binary object maps to ("binary", payload, subtype). */
		assert(luaL_dostring(L, "return bson.binary('data')") == LUA_OK);
		call_bson(L, "type", 1, 3);
		assert(strcmp(lua_tostring(L, -3), "binary") == 0);
		assert(strcmp(lua_tostring(L, -2), "data") == 0);
		assert(lua_tointeger(L, -1) == 0);
		lua_pop(L, 3);
	}
	{
		/* nil maps to the type name "nil". */
		lua_pushnil(L);
		call_bson(L, "type", 1, 1);
		assert(strcmp(lua_tostring(L, -1), "nil") == 0);
		lua_pop(L, 1);
	}
	{
		/* Unsupported types raise. */
		lua_pushlightuserdata(L, (void *)main);
		expect_bson_error(L, "type", 1, "Invalid type");
	}

	/* Encoding errors: every rejection path in the append_* walkers. */
	expect_encode_error(L, "return {1,2}", "Bson dictionary's key can't be number");
	expect_encode_error(L, "return {[true]=1}", "Invalid key type : boolean");
	expect_encode_error(L, "return {f=function() end}", "Invalid value type : function");
	expect_encode_error(L, "return {['\xff\xfe']=1}", "Invalid utf8 string");
	expect_encode_error(L,
		"local t={a=1} for i=1,129 do t={a=t} end return t",
		"Too depth while encoding bson");
	expect_encode_error(L,
		"return {list=setmetatable({},{__len=function() return 'x' end})}",
		"__len should return integer");
	expect_encode_error(L, "return {s='\\0\\99'}", "Invalid subtype 99");
	expect_encode_error(L, "return {o='\\0\\7short'}", "Invalid object id");
	expect_encode_error(L, "return {d='\\0\\9ab'}", "Invalid date");
	expect_encode_error(L, "return {t='\\0\\17ab'}", "Invalid timestamp");
	expect_encode_error(L, "return {i='\\0\\18ab'}", "Invalid int64");

	/* encode_order rejects odd argument counts and non-string keys. */
	{
		lua_pushstring(L, "a");
		lua_pushinteger(L, 1);
		lua_pushstring(L, "b");
		expect_bson_error(L, "encode_order", 3, "Invalid ordered dict");
	}
	{
		/* The second key is not convertible to a string. */
		lua_pushstring(L, "a");
		lua_pushinteger(L, 1);
		lua_newtable(L);
		lua_pushinteger(L, 2);
		expect_bson_error(L, "encode_order", 4, "need a string");
	}

	printf("bson: all assertions passed\n");
	lua_close(L);
	return 0;
}
