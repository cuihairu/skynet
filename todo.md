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
- [x] socket: ludp_address reads port before size validation — fixed 22c5507 (lualib-src/lua-socket.c:750-765; test/test_socket.c)
- [x] sharedata: nested convtable leaks ctx->tbl children when lua_call raises — fixed 91a058a (lualib-src/lua-sharedata.c:151-182; test/test_sharedata.c)
- [x] module: open_sym failure leaks dlopen handle — fixed cbe40d5 (skynet-src/skynet_module.c:120-130; test/test_module.c)
- [x] daemon: fprintf-failure path never fcloses the fdopen FILE* — fixed bda6d0e (skynet-src/skynet_daemon.c:58-65; test/test_daemon.c)
- [x] error: static logger handle read/written without atomics — fixed 307e815 (skynet-src/skynet_error.c:34-55; test/test_error.c)
- [x] malloc_hook: dead APIs with latent bugs (div-by-zero, aligned_alloc, posix_memalign) — fixed 24961cb (skynet-src/malloc_hook.c:246-300; test/test_malloc_hook.c)
- [ ] handle: verify slot-grow reader safety and handle_reader_slot TLS recycle under pressure (speculative) — skynet-src/skynet_handle.c:127+

## 审核 2026-11-03（c9bed64a..HEAD）

- [ ] 审核发现：find_mem_stat 认领槽位后 atomic_meminfo_init 非原子清零（先 alloc 后 free），而 atomic_meminfo_merge 先读 free 后读 alloc，并发 dump 可读到 alloc=0/free=旧值，dump_mem_lua 的 info.alloc-info.free 与 malloc_used_memory 的 total.alloc-total.free 瞬时下溢为巨大值，重新引入本提交声称消除的症状（skynet-src/malloc_hook.c:75,340,403；skynet-src/mem_info.c:56-62）
- [ ] 审核发现：回收逻辑注释假定“同一 handle 的 alloc/free 由所属服务线程串行”，但 clean_prefix 用 cookie 中记录的原分配 handle，块可被其他服务线程释放；当 H 最后一块被跨线程释放使槽位回收（handle=0）并立即被 handle K 经 CAS+清零重新认领时，H 线程仍持有的过期槽指针会把 H 的分配记到 K 的计数上，随后 find_mem_stat(H,0) 找不到槽位而丢弃 free，导致单服务 mem_stats 永久偏差（skynet-src/malloc_hook.c:105-115）
- [ ] 审核发现：find_mem_stat 表满或 handle 缺失时静默返回 NULL 丢弃统计；free 路径在 handle 缺失时需扫描全部 65536 槽位（约 4MB 缓存行），存在统计少报与性能陡降风险（skynet-src/malloc_hook.c:61-81）
- [ ] 审核发现：make test 目标硬编码 -Iskynet-src -I$(JEMALLOC_INC) 并链接 $(MALLOC_STATICLIB)，NOUSE_JEMALLOC/非 jemalloc 平台构建会失败，且未传递 $(SKYNET_DEFINES)（如 MEMORY_CHECK/THREAD_SAFE），与主构建不一致（Makefile:128-138）
- [ ] 审核发现：新增测试为单线程，未覆盖槽位回收后由其他 handle 重新认领、reserve=0 未命中路径、malloc_current_memory、malloc_memory_block 及表满返回 NULL 丢弃路径；修复点恰在并发/碰撞逻辑上，上述竞态窗口均未覆盖（test/test_malloc_hook.c:1-122）
