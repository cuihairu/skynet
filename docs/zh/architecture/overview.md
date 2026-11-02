# 架构概览

Skynet 是 fork 自 cloudwu/skynet 的 C/Lua actor 框架。服务上下文拥有消息队列，worker 分派队列中的消息。Lua 请求可以通过协程挂起，期间同一服务的其他请求仍可能运行。服务隔离不意味着 C 层共享数据不需要同步。

worker 默认数是 **8**，不会自动取 CPU 核数。定时器以 **10ms** 为单位；定时线程每 2.5ms 轮询不代表回调有 2.5ms 精度。消息日志有单独的开启路径，不默认记录所有交互。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_server.c:125](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L125) | `skynet_context_new(` |
| [skynet-src/skynet_server.c:293](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L293) | `skynet_context_message_dispatch(` |
| [skynet-src/skynet_main.c:160](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_main.c#L160) | `config.thread =` |
| [skynet-src/skynet_timer.c:232](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L232) | `ti.tv_nsec / 10000000` |
| [skynet-src/skynet_start.c:140](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L140) | `usleep(2500)` |
| [skynet-src/skynet_log.c:9](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_log.c#L9) | `skynet_log_open(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
