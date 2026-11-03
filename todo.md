# todo

P2/P3 items from the 2026-11-01 code review (docs/review/2026-11-01-code-review.md).

## P2

- [x] malloc_hook: hash collision on 24-bit handle slots corrupts mem_stats (uint64 underflow in dump_mem_lua) — fixed 4be20e7 (skynet-src/malloc_hook.c:35,51-79,92-113; test/test_malloc_hook.c)
- [x] seri: unbounded lua_createtable prealloc from decoded array_size enables OOM via remote stream — lualib-src/lua-seri.c:447-461
- [x] bson: negative int32 field lengths desync bson_reader and allow 1-5 byte OOB reads (JSCODE/SYMBOL/CODEWS/DBPOINTER/BINARY + skip path) — lualib-src/lua-bson.c:648-660,712-733,768-782
- [x] logger: freopen return not re-checked, subsequent writes go to NULL — service-src/service_logger.c:53-66

## P3

- [x] dns: parse_hosts leaks io.open handle on all return paths — lualib/skynet/dns.lua:110-148
- [x] mongo: OP_MSG data_len == sizeof(header) reads past buffer — lualib-src/lua-mongo.c:160-201
- [x] mongo: reply rawlen memcpy without min-length check — lualib-src/lua-mongo.c:214-218
- [ ] socket: ludp_address reads port before size validation — lualib-src/lua-socket.c:750-759
- [ ] sharedata: nested convtable leaks ctx->tbl children when lua_call raises — lualib-src/lua-sharedata.c:151-174
- [ ] module: open_sym failure leaks dlopen handle — skynet-src/skynet_module.c:120-125
- [ ] daemon: fprintf-failure path never fcloses the fdopen FILE* — skynet-src/skynet_daemon.c:58-61
- [ ] error: static logger handle read/written without atomics — skynet-src/skynet_error.c:34-37
- [ ] malloc_hook: dead APIs with latent bugs (div-by-zero, aligned_alloc, posix_memalign) — skynet-src/malloc_hook.c:200,232,240
- [ ] handle: verify slot-grow reader safety and handle_reader_slot TLS recycle under pressure (speculative) — skynet-src/skynet_handle.c:127+
