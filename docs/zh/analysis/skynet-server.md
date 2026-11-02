# skynet_server.c 源码导航

skynet_context_new 创建服务上下文，skynet_send 路由消息，skynet_context_message_dispatch 从队列执行回调。句柄查找委托给 skynet_handle.c。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_server.c:125](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L125) | `skynet_context_new(` |
| [skynet-src/skynet_server.c:121](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L121) | `skynet_send(` |
| [skynet-src/skynet_server.c:293](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L293) | `skynet_context_message_dispatch(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
