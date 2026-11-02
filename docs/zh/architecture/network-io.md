# 网络 I/O

socket_server 轮询 socket 并产生事件，skynet_socket 将事件转换为服务消息。轮询抽象选择平台后端，Lua socket API 管理等待协程。业务里的阻塞代码仍会阻塞 worker。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/socket_server.c:1708](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L1708) | `socket_server_poll(` |
| [skynet-src/skynet_socket.c:79](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_socket.c#L79) | `skynet_socket_poll(` |
| [skynet-src/socket_poll.h:1](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_poll.h#L1) | `#if` |
| [lualib/skynet/socket.lua:305](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/socket.lua#L305) | `function socket.read(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
