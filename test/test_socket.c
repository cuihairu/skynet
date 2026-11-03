/* Unit test for lua-socket: ludp_address must validate size before reading.
 *
 * ludp_address previously memcpy'd the port from addr+1 and used addr+3 for
 * the address before checking sz, so a short (or empty) string caused an
 * out-of-bounds read.  The fix rejects any size that is not exactly an IPv4
 * or IPv6 UDP address before touching the bytes.
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lauxlib.h>
#include <lua.h>

/* Stub skynet_context (lua-socket.c references skynet.h types). */
struct skynet_context;
struct skynet_socket_message;
struct socket_sendbuffer;

/* Provide symbols referenced by the non-static parts of lua-socket.c. */
int skynet_socket_sendbuffer(struct skynet_context *ctx, struct socket_sendbuffer *b) { (void)ctx; (void)b; return 0; }
int skynet_socket_sendbuffer_lowpriority(struct skynet_context *ctx, struct socket_sendbuffer *b) { (void)ctx; (void)b; return 0; }
int skynet_socket_listen(struct skynet_context *ctx, const char *h, int p, int b) { (void)ctx; (void)h; (void)p; (void)b; return 0; }
int skynet_socket_connect(struct skynet_context *ctx, const char *h, int p) { (void)ctx; (void)h; (void)p; return 0; }
int skynet_socket_bind(struct skynet_context *ctx, int fd) { (void)ctx; (void)fd; return 0; }
void skynet_socket_close(struct skynet_context *ctx, int id) { (void)ctx; (void)id; }
void skynet_socket_shutdown(struct skynet_context *ctx, int id) { (void)ctx; (void)id; }
void skynet_socket_start(struct skynet_context *ctx, int id) { (void)ctx; (void)id; }
void skynet_socket_pause(struct skynet_context *ctx, int id) { (void)ctx; (void)id; }
void skynet_socket_nodelay(struct skynet_context *ctx, int id) { (void)ctx; (void)id; }
int skynet_socket_udp(struct skynet_context *ctx, const char *a, int p) { (void)ctx; (void)a; (void)p; return 0; }
int skynet_socket_udp_connect(struct skynet_context *ctx, int id, const char *a, int p) { (void)ctx; (void)id; (void)a; (void)p; return 0; }
int skynet_socket_udp_dial(struct skynet_context *ctx, const char *a, int p) { (void)ctx; (void)a; (void)p; return 0; }
int skynet_socket_udp_listen(struct skynet_context *ctx, const char *a, int p) { (void)ctx; (void)a; (void)p; return 0; }
int skynet_socket_udp_sendbuffer(struct skynet_context *ctx, const char *a, struct socket_sendbuffer *b) { (void)ctx; (void)a; (void)b; return 0; }
const char * skynet_socket_udp_address(struct skynet_socket_message *m, int *sz) { (void)m; (void)sz; return NULL; }
struct socket_info * skynet_socket_info(void) { return NULL; }
void skynet_socket_init(void) {}
void skynet_socket_exit(void) {}
void skynet_socket_free(void) {}
int skynet_socket_poll(void) { return 0; }
void skynet_socket_updatetime(void) {}
void skynet_error(struct skynet_context *ctx, const char *fmt, ...) { (void)ctx; (void)fmt; }

#include "../lualib-src/lua-socket.c"

static void
expect_error(lua_State *L, const char *arg, size_t len) {
	lua_pushcfunction(L, ludp_address);
	lua_pushlstring(L, arg, len);
	int rc = lua_pcall(L, 1, 0, 0);
	assert(rc == LUA_ERRRUN);
	lua_pop(L, 1);
}

int
main(void) {
	lua_State *L = luaL_newstate();
	assert(L);

	/* Empty and short strings must be rejected without reading OOB. */
	expect_error(L, "", 0);
	expect_error(L, "ab", 2);
	expect_error(L, "abcdef", 6);

	/* A valid IPv4 address (family=AF_INET, port, 4-byte addr) works. */
	{
		uint8_t buf[7];
		buf[0] = 0; /* AF_INET */
		buf[1] = 0x00; buf[2] = 0x50; /* port 80 big-endian */
		buf[3] = 127; buf[4] = 0; buf[5] = 0; buf[6] = 1;
		lua_pushcfunction(L, ludp_address);
		lua_pushlstring(L, (const char *)buf, 7);
		assert(lua_pcall(L, 1, 2, 0) == LUA_OK);
		assert(strcmp(lua_tostring(L, -2), "127.0.0.1") == 0);
		assert(lua_tointeger(L, -1) == 80);
		lua_pop(L, 2);
	}

	printf("socket: all assertions passed\n");
	lua_close(L);
	return 0;
}

void
socket_info_release(struct socket_info *si) { (void)si; }
