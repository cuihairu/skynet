/* Unit test for lua-mongo: reject truncated OP_MSG and short reply lengths.
 *
 * unpack_reply read the payload type byte and a 4-byte document length at
 * section = h+1 even when data_len == sizeof(header), reading past the
 * buffer.  reply_length memcpy'd 4 bytes from a string that could be shorter
 * than 4 bytes.  Both are now rejected with a length check first.
 *
 * This test drives unpack_reply and reply_length directly: the full error
 * matrix (short reply, wrong opcode, checksum flag, unknown flags, unknown
 * payload type, multi-section document length) plus the success path and an
 * op_msg round trip.
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

/* Store a little-endian int32. */
static void
put_i32(char *p, int32_t v) {
	p[0] = (char)(v & 0xff);
	p[1] = (char)((v >> 8) & 0xff);
	p[2] = (char)((v >> 16) & 0xff);
	p[3] = (char)((v >> 24) & 0xff);
}

/* Read a little-endian int32. */
static int32_t
get_i32(const char *p) {
	const uint8_t *b = (const uint8_t *)p;
	return (int32_t)(b[0] | b[1] << 8 | b[2] << 16 | (uint32_t)b[3] << 24);
}

/* Build a full reply: 16-byte header (request/response/opcode/flags, the
 * 4-byte message length prefix is stripped by the caller in lua-mongo)
 * followed by the section body. */
static size_t
build_reply(char *buf, int32_t request_id, int32_t response_to,
	int32_t opcode, int32_t flags, const char *body, size_t body_len) {
	int n = 0;
	put_i32(buf + n, request_id);
	n += 4;
	put_i32(buf + n, response_to);
	n += 4;
	put_i32(buf + n, opcode);
	n += 4;
	put_i32(buf + n, flags);
	n += 4;
	memcpy(buf + n, body, body_len);
	return n + (int)body_len;
}

/* Call unpack_reply with the string kept on the stack (index `idx`) so the
 * returned document pointer stays comparable against it. */
static int
call_reply_keep(lua_State *L, const char *buf, size_t len, int nres) {
	int idx = lua_gettop(L) + 1;
	lua_pushlstring(L, buf, len);
	lua_pushcfunction(L, unpack_reply);
	lua_pushvalue(L, idx);
	return lua_pcall(L, 1, nres, 0);
}

int
main(void) {
	lua_State *L = luaL_newstate();
	assert(L);

	char buf[64];
	char body[16];

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

	/* Reply shorter than the header: unpack_reply must return false instead
	 * of reading the opcode out of bounds. */
	{
		size_t n = build_reply(buf, 1, 2, OP_MSG, 0, NULL, 0);
		assert(n == 16);
		assert(call_reply_keep(L, buf, n - 1, 1) == LUA_OK);
		assert(lua_isboolean(L, -1) && !lua_toboolean(L, -1));
		lua_pop(L, 2);
	}

	/* Wrong opcode is rejected with the opcode value. */
	{
		size_t n = build_reply(buf, 1, 2, OP_COMPRESSED, 0, NULL, 0);
		assert(call_reply_keep(L, buf, n, 0) == LUA_ERRRUN);
		assert(strstr(lua_tostring(L, -1), "Unsupported opcode:2012"));
		lua_pop(L, 2);
	}

	/* checksumPresent flag is rejected. */
	{
		size_t n = build_reply(buf, 1, 2, OP_MSG, MSG_CHECKSUM_PRESENT, NULL, 0);
		assert(call_reply_keep(L, buf, n, 0) == LUA_ERRRUN);
		assert(strstr(lua_tostring(L, -1), "Unsupported OP_MSG flag checksumPresent"));
		lua_pop(L, 2);
	}

	/* Any flag other than moreToCome is rejected. */
	{
		size_t n = build_reply(buf, 1, 2, OP_MSG, 4, NULL, 0);
		assert(call_reply_keep(L, buf, n, 0) == LUA_ERRRUN);
		assert(strstr(lua_tostring(L, -1), "Unsupported OP_MSG flag:4"));
		lua_pop(L, 2);
	}

	/* Unknown payload type is rejected. */
	{
		memset(body, 0, sizeof(body));
		body[0] = 1;
		put_i32(body + 1, 5);
		body[5] = 0;
		size_t n = build_reply(buf, 1, 2, OP_MSG, 0, body, 10);
		assert(call_reply_keep(L, buf, n, 0) == LUA_ERRRUN);
		assert(strstr(lua_tostring(L, -1), "Unsupported OP_MSG payload type: 1"));
		lua_pop(L, 2);
	}

	/* A declared document length that does not span exactly the section is
	 * rejected (more than one section). */
	{
		memset(body, 0, sizeof(body));
		body[0] = 0;
		put_i32(body + 1, 4);	/* doc claims 4 bytes but 5 follow */
		body[5] = 0;
		size_t n = build_reply(buf, 1, 2, OP_MSG, 0, body, 10);
		assert(call_reply_keep(L, buf, n, 0) == LUA_ERRRUN);
		assert(strstr(lua_tostring(L, -1), "Unsupported OP_MSG reply: >1 section"));
		lua_pop(L, 2);
	}

	/* A valid reply (flags 0) returns true, the response_to id and a document
	 * pointer pointing just past the payload type byte. */
	{
		memset(body, 0, sizeof(body));
		body[0] = 0;
		put_i32(body + 1, 5);
		body[5] = 0;
		size_t n = build_reply(buf, 9, 42, OP_MSG, 0, body, 6);
		assert(n == 22);
		assert(call_reply_keep(L, buf, n, 3) == LUA_OK);
		assert(lua_toboolean(L, -3) == 1);
		assert(lua_tointeger(L, -2) == 42);
		size_t rlen = 0;
		const char *data = lua_tolstring(L, 1, &rlen);
		assert(rlen == n);
		assert(lua_touserdata(L, -1) == data + 17);
		lua_pop(L, 4);
	}

	/* moreToCome alone is accepted and takes the same success path. */
	{
		memset(body, 0, sizeof(body));
		body[0] = 0;
		put_i32(body + 1, 5);
		body[5] = 0;
		size_t n = build_reply(buf, 9, 43, OP_MSG, MSG_MORE_TO_COME, body, 6);
		assert(call_reply_keep(L, buf, n, 3) == LUA_OK);
		assert(lua_toboolean(L, -3) == 1);
		assert(lua_tointeger(L, -2) == 43);
		lua_pop(L, 4);
	}

	/* op_msg without a command document is rejected. */
	{
		lua_pushcfunction(L, op_msg);
		lua_pushinteger(L, 7);
		lua_pushinteger(L, 0);
		assert(lua_pcall(L, 2, 0, 0) == LUA_ERRRUN);
		assert(strstr(lua_tostring(L, -1), "opmsg require cmd document"));
		lua_pop(L, 1);
	}

	/* op_msg builds the wire frame: message length, opcode, flags, payload
	 * type byte and the command document bytes. */
	{
		/* A minimal empty bson document. */
		static const char doc[5] = { 5, 0, 0, 0, 0 };
		lua_pushcfunction(L, op_msg);
		lua_pushinteger(L, 7);	/* request id */
		lua_pushinteger(L, 0);	/* flags */
		lua_pushlightuserdata(L, (void *)doc);
		assert(lua_pcall(L, 3, 1, 0) == LUA_OK);
		size_t rlen = 0;
		const char *msg = lua_tolstring(L, -1, &rlen);
		assert(rlen == 26);
		assert(get_i32(msg) == 26);		/* message_length */
		assert(get_i32(msg + 4) == 7);	/* request_id */
		assert(get_i32(msg + 8) == 0);	/* response_to */
		assert(get_i32(msg + 12) == OP_MSG);
		assert(get_i32(msg + 16) == 0);	/* flags */
		assert((uint8_t)msg[20] == 0);	/* payload type: body */
		assert(memcmp(msg + 21, doc, sizeof(doc)) == 0);
		lua_pop(L, 1);
	}

	printf("mongo: all assertions passed\n");
	lua_close(L);
	return 0;
}
