/* Unit test for lua-mongo: reject truncated OP_MSG and short reply lengths.
 *
 * unpack_reply read the payload type byte and a 4-byte document length at
 * section = h+1 even when data_len == sizeof(header), reading past the
 * buffer.  reply_length memcpy'd 4 bytes from a string that could be shorter
 * than 4 bytes.  Both are now rejected with a length check first.
 *
 * This test drives unpack_reply and reply_length directly.
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lauxlib.h>
#include <lua.h>

#include "../lualib-src/lua-mongo.c"

static void
expect_error(lua_State *L, lua_CFunction fn, const char *arg, size_t len) {
	lua_pushcfunction(L, fn);
	lua_pushlstring(L, arg, len);
	int rc = lua_pcall(L, 1, 0, 0);
	assert(rc == LUA_ERRRUN);
	lua_pop(L, 1);
}

int
main(void) {
	lua_State *L = luaL_newstate();
	assert(L);

	/* OP_MSG with data_len == sizeof(header): the payload type byte and
	 * document length would be read past the buffer. */
	{
		struct header_t h;
		memset(&h, 0, sizeof(h));
		h.opcode = little_endian(OP_MSG);
		expect_error(L, unpack_reply, (const char *)&h, sizeof(h));
	}

	/* reply_length with a 1-byte string: memcpy would read 4 bytes. */
	{
		expect_error(L, reply_length, "\x01", 1);
	}

	/* A 3-byte string is also too short. */
	{
		expect_error(L, reply_length, "\x01\x02\x03", 3);
	}

	/* A valid 4-byte reply length still works. */
	{
		uint32_t v = 100;
		lua_pushcfunction(L, reply_length);
		lua_pushlstring(L, (const char *)&v, 4);
		assert(lua_pcall(L, 1, 1, 0) == LUA_OK);
		/* length field is stored little-endian in the stream; little_endian
		 * on the host gives back the encoded value, then we subtract 4. */
		assert(lua_tointeger(L, -1) == (lua_Integer)little_endian((int32_t)v) - 4);
		lua_pop(L, 1);
	}

	printf("mongo: all assertions passed\n");
	lua_close(L);
	return 0;
}
