# 设计边界

源码分别实现服务上下文、消息分派与 Lua 协程。制定业务并发规则前，应先检查这些机制。请求可能在协程让出期间交错，运行时代码注入也不等于状态自动迁移。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_server.c:125](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L125) | `skynet_context_new(` |
| [lualib/skynet.lua:728](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L728) | `function skynet.call(` |
| [lualib/skynet/debug.lua:84](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/debug.lua#L84) | `function dbgcmd.RUN(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
