# socket_server.c 源码导航

此模块处理 socket 请求并轮询平台事件，公开入口包括 poll、listen、connect 与 UDP 操作。面向业务的 Lua 库与 socket C driver 属于不同层。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/socket_server.c:1708](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L1708) | `socket_server_poll(` |
| [skynet-src/socket_server.c:2069](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L2069) | `socket_server_listen(` |
| [skynet-src/socket_server.c:1865](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L1865) | `socket_server_connect(` |
| [skynet-src/socket_server.c:2138](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L2138) | `socket_server_udp(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
