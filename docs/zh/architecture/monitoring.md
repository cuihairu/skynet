# 监控与调试

monitor 比较 worker 的执行标记，以检测疑似卡住的消息处理。Lua debug 命令提供运行态检查与注入执行。检测与调试入口不等于可以安全中断任意业务代码的超时机制。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_monitor.c:38](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_monitor.c#L38) | `skynet_monitor_check(` |
| [skynet-src/skynet_start.c:95](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L95) | `thread_monitor(` |
| [lualib/skynet/debug.lua:84](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/debug.lua#L84) | `function dbgcmd.RUN(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
