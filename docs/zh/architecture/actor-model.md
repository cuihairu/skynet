# Actor 模型

服务对应上下文、句柄、回调和队列，不是独占的操作系统线程。worker 从服务队列分派消息。Lua 处理函数调用 skynet.call 或 skynet.sleep 时会让出协程，恢复前状态可能已经改变；隔离不保证整个请求没有交错。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_server.c:42](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L42) | `struct skynet_context {` |
| [skynet-src/skynet_server.c:293](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L293) | `skynet_context_message_dispatch(` |
| [lualib/skynet.lua:728](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L728) | `function skynet.call(` |
| [lualib/skynet.lua:517](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L517) | `function skynet.sleep(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
