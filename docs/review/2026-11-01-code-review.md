# 代码深审报告（2026-11-01）

全仓深度代码审查：安全静态扫描 + 隐藏 bug 猎捕 + 质量门。发现按严重度排序，每条带 `file:line` 证据与验证状态。

## 结论总账

| 桶 | 数量 | 明细 |
|---|---|---|
| 发现总数 | 16 REAL + 3 存疑/说明 = 19 | 另有 1 条 REFUTED（误报） |
| P0 | 0 | — |
| P1 已当场修 | 4 | harbor 越界读 / logfile UAF / pkcs7 越界读 / workflow 红线 |
| P2 记 todo | 4 | malloc_hook 哈希碰撞 / seri 预分配 DoS / bson 负长度 / logger freopen |
| P3 记 todo | 11 | 见文末清单 |

质量门基线：`make linux` 0 警告；`test/run_autotest.sh` 25/25 PASS（修后复跑两次）；仓库无 lint/类型检查配置（纯 C，`-Wall` 全开即为门）。

## 审查方法与范围

1. **fork 溯源**：与 cloudwu/skynet upstream master 逐文件 diff。核心 C 文件（skynet_server/mq/timer/handle/start/module/socket/env/main/harbor/error/monitor.c、service-src/*.c、lua-bson/netpack/socket/crypt 除外的大部分 lualib-src、skynet.lua + 22 个框架 Lua）与上游字节一致。**fork 独有**：`malloc_hook.c`、`mem_info.c/h`、`lua-memory.c`、`test/run_autotest.sh`、`.github/workflows`。上游相同部分的问题同样记录（修复应尽量回流上游）。
2. **安全静态扫描**：硬编码密钥/凭据（仅 examples/test 演示用假凭据，P3 说明）、shell 注入（无）、SQL 拼接（无）、eval/load（Lua 沙箱 API 按设计提供）、反序列化（bson/seri/netpack 逐字节核对）、路径穿越（config/daemon 路径来自运维配置，非外部输入）。
3. **隐藏 bug 猎捕**：错误处理缺失、竞态、越界、资源泄漏、off-by-one、并发。所有 agent 报告的发现均逐条人工核对源码后定性；1 条误报已剔除（见 REFUTED）。

---

## P1 — 已当场修复（一问题一 commit，测试绿）

### P1-1 harbor 帧长度下界缺失 → 堆越界读 【已修 e8ebdbe】
- **证据**：`service-src/service_harbor.c:477`（原）只校验 `s->size[0] != 0`，`s->length`（= size[1..3] 三字节拼成）可为 0–11；`forward_local_messsage`（:315 `cookie += sz - HEADER_COOKIE_LENGTH`、:323 `skynet_send(..., sz-HEADER_COOKIE_LENGTH)`，size_t 无符号回绕）在长度 < `HEADER_COOKIE_LENGTH`(=12, :28) 时堆下溢读并携带超大 size 派发。
- **修复**：长度下界检查，镜像既有"过长"分支惯用法（error + `close_harbor` + return，且置于 `recv_buffer` 分配之前）。
- **测试**：autotest 25/25 PASS。

### P1-2 服务 logfile LOGOFF 竞态 → FILE* 双重释放/UAF 【已修 f1e38df】
- **证据**：`skynet-src/skynet_server.c`（原）`cmd_logoff` 无条件 `fclose(ctx->logfile)`；若某 dispatch 已通过 `skynet_log_open`/`ctx->logfile` 正在写（dispatch 持 ref 但 logoff 不等 dispatch），后续写发生 UAF；`cmd_logon` 再次打开后旧 FILE* 被 double-fclose。
- **修复**：新增 `struct retired_logfile` 无锁 CAS 退休链，`cmd_logoff` CAS 成功后将 FILE* 挂入 `ctx->logfile_retired`（写 close 标记后不再使用该指针）；`delete_context` 在 ref==0（无并发 dispatch，静止点）统一 `fclose` 排空。热路径 `dispatch_message` 与 `cmd_logon`（自开自关、文件从未对外可见）零改动。
- **测试**：autotest 25/25 PASS。

### P1-3 pkcs7 padding 回扫越界 【已修 058b195】
- **证据**：`lualib-src/lua-crypt.c:409-419`（原）`padding_remove_pkcs7` 循环 `for (i=padding; i>0; i--)` 在 padding 值异常时向前回扫最多 padding 字节；配合上层 `padding <= 0 || padding > 8` 检查之外的路径可回读 `buffer[0]` 之前内存。
- **修复**：回扫上界 `for (i=1; i<padding && i<8; i++)`——合法 pkcs7 padding 仅 1..8 字节，扫描永不越过 `buffer[0]`（DES 块保证 `textsz>=8`）；调用方 :512 的 `padding > 8` 拒绝语义不变（不会把非法 padding 洗白成合法）。
- **测试**：`test/testcrypt.lua`（:21-25 pkcs7 往返用例）通过，autotest 25/25。

### P1-4 发版 workflow 违反版本发布红线 【已修 32b88be】
- **证据**：`.github/workflows/build-release.yml`（原）`on.push.tags: ['v*']` 触发 + `release:` job（softprops/action-gh-release，`prerelease: false`）——任何 `v*` tag 自动创建**正式 GitHub Release**，违反全局红线"版本号只能由用户手动给出并触发/未验收版本绝不出现于 Releases 页"。
- **修复**：删除 `tags` 触发与整个 `release:` job；保留 push-master 构建产物（artifact 仅上传，不发布）。
- **说明**：本仓当前无 daily/nightly schedule workflow；日后新增须走 schedule/workflow_dispatch 形态。

---

## P2 — 记 todo（真实缺陷，建议排期修）

### P2-1 mem_stats 服务句柄哈希碰撞 → 统计错乱/无符号下溢
- **证据**：`skynet-src/malloc_hook.c:35`（`SLOT_SIZE 0x10000`）、:43（`h = handle & 0xFFFF`）、:61（`ATOM_STORE(&data->handle, handle)` 无 compare-exchange）、:66-68（`update_xmalloc_stat_free` 不校验槽内 handle 归属即递减）。碰撞条件：单节点同时存活 >65536 个服务（handle 24 位 index 空间 16.7M，理论可达）。后果：`dump_mem_lua`（:350-360）`alloc - free` 为 uint64，减到负数即下溢成天文数字；统计失真。
- **边界**：`service_snlua.c` 的 memlimit 走 per-service `l->mem`（精确），不受影响——仅统计报表失真，故 P2 非 P1。
- **缓解**：碰撞需单节点 6.5 万活服务，实际部署罕见。

### P2-2 seri 反序列化无界预分配 → DoS
- **证据**：`lualib-src/lua-seri.c:447-461` `unpack_table`：`array_size` 来自线上的变长整数（`get_integer`，cookie 路径最大可到 QWORD），直接 `lua_createtable(L, array_size, 0)` 预分配。攻击面：跨节点 harbor 消息 / netpack 前置的 remote 序列化流，伪造 `array_size ≈ 2^31+` 即触发 ~8GB+ 预分配，单包 OOM。
- **对比**：序列化端 `wb_table_array`（:220-224）对 `>= MAX_COOKIE-1` 有编码边界，但解码端未设上限。

### P2-3 bson 负长度字段 → reader 失步 + 小幅 OOB 读
- **证据**：`lualib-src/lua-bson.c` 多处 `read_bytes(L,&t,sz)` 的 `sz` 取自流内 int32 且未验非负：
  - `:648-657` BSON_BINARY：`sz=-1` → reader 指针回退、size 虚增；随后 `luaL_addlstring(huge)` 走 `3rd/lua/lauxlib.c` `newbuffsize` 的 `sz >= MAX_SIZE - B->n` → 优雅报错 "resulting string too large"（已核实，非崩溃）。
  - `:719-724` BSON_JSCODE/BSON_SYMBOL（`make_object(..., sz+4)`）、`:727-731` BSON_CODEWS（`sz-4`）、`:712-716` BSON_DBPOINTER（`sz+12`、`make_object(..., sz+16)`）、skip 路径 `:768-782`：负 sz 使 reader 失步 1–5 字节，子文档位于 blob 末尾时可越界读 1–5 字节。
  - **对照**：BSON_STRING 有 `sz <= 0` 守卫（:636-638），证明其余分支属遗漏。
- **同类 REFUTED**：`unpack_dict`（:605-608）`sz<5` 曾被报为越界——核实 `struct bson_reader.size` 为**有符号 int**，`check_reader` 在任何解引用前 luaL_error，长跳终止解码；仅是健壮性瑕疵（见 P3 清单），非内存不安全。

### P2-4 service_logger freopen 失败后向 NULL 句柄写
- **证据**：`service-src/service_logger.c:53` `inst->handle = freopen(inst->filename, "a", inst->handle);` 返回 NULL 不复查；后续每条 `PTYPE_TEXT` 走 :60-66 `fprintf/fwrite/fflush(NULL)`。glibc 下 fprintf(NULL) 不崩但 UB；日志静默丢失。
- **触发**：日志文件所在目录被删/权限变更后 `LOGOUT`+重新打开路径。

---

## P3 — 记 todo（低危/健壮性/信息项）

1. `lualib/skynet/dns.lua:110-148` `parse_hosts` io.open 后所有 return 路径（:148）不 `f:close()`；同文件 `parse_resolv_conf`（:157-169）有关闭，属遗漏。
2. `lualib-src/lua-mongo.c:160-201` OP_MSG：`data_len < sizeof(*h)` 允许 `data_len == 16` 恰等 → `*section`（h+1）读 1 字节越界，`get_length` 再读 4 字节越界。
3. `lualib-src/lua-mongo.c:214-218` reply 解析 `memcpy(&rawlen, rawlen_str, sizeof(int))` 前不校验字符串长度 ≥4。
4. `lualib-src/lua-socket.c:750-759` `ludp_address` `memcpy(&port, addr+1, 2)` 先于 sz 长度校验。
5. `lualib-src/lua-sharedata.c:151-174` 嵌套表 `convtable` 递归中 `lua_call` 抛错则 `ctx->tbl` 已指向新 malloc 的 table、旧 `tbl` 被覆盖，新 tbl 的子级分配在 unwind 中无人回收（根 teardown 只 free 顶层）。
6. `skynet-src/skynet_module.c:120-125` `open_sym` 失败路径：dl 已存入 `M->m[index]` 但 count 未增，`dlclose` 永不调用（进程生命周期内泄漏，模块加载失败属罕见路径）。
7. `skynet-src/skynet_daemon.c:58-61` fprintf 失败分支 close(fd) 但 `fdopen` 的 FILE* 未 fclose。
8. `skynet-src/skynet_error.c:34-37` 静态 `logger` 句柄多线程读写无原子修饰（实际由 handle 管理器串行化写入路径，良性，建议补 ATOM_ 消除数据竞争 UB）。
9. `skynet-src/malloc_hook.c:200`（`(PREFIX_SIZE+size-1)/size` size=0 除零）、`:232` `je_aligned_alloc`、`:240-241` `posix_memalign` abort-vs-return 语义——均为无调用方的死 API（grep 全仓仅头文件声明），建议删除或标注。
10. 【说明，非缺陷】`NOUSE_JEMALLOC` 平台（macosx/openbsd/mingw）无 malloc interpose：无 cookie/统计/abort-on-OOM，与上游一致的设计取舍；memlimit 在这些平台靠 per-service `l->mem` 仍生效。
11. 【存疑，未定性】`skynet-src/skynet_handle.c` slot 扩容（:127 起，4→0x40000000）期间读侧锁粒度、以及 fork 独有 `handle_reader_slot`（:15-54）TLS 槽在服务高密度创建/销毁下的回收时序——静态审查未发现可触发现象，标记存疑待压测验证。

## 安全扫描其余结论（无发现）

- 硬编码密钥：仅 `examples/` 与 `test/` 演示用假凭据（mongo 连接串、test 密钥），不构成泄露。
- shell 注入 / SQL 拼接：全仓无 `system()`/`popen` 拼接外部输入，无 SQL。
- `load()`/`loadstring`：`debug_console`、`cluster` 代理按设计执行远端 Lua（管理面，需运维边界保护）——架构固有，非漏洞。
- 路径穿越：`skynet_daemon`/log 路径全部来自启动配置文件（运维控制），不接收外部输入。

## REFUTED（误报存档）

- **bson `unpack_dict` sz<5 越界**：报称 `read_bytes(L, br, sz-5)` 负长度导致 OOB 读。核实：`struct bson_reader { const uint8_t *ptr; int size; }` size 为有符号 int，`t.size = sz-5 < 0` 时 `check_reader` 在解引用前 `luaL_error`，longjmp 终止。不构成内存不安全。

## 修复 commit 清单

| commit | 内容 |
|---|---|
| e8ebdbe | fix: reject undersized harbor frames |
| f1e38df | fix: defer logfile close past concurrent dispatch |
| 058b195 | fix: bound pkcs7 padding scan to one block |
| 32b88be | ci: stop auto-publishing releases on version tags |
