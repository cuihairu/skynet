# Socket 抽象

Lua socket 库通过 skynet.socketdriver 提供 open、listen、read、write 操作。socket driver 是 lua-socket.c 注册的 C 模块，不是 socketdriver.lua 文件。socket_server 中 TCP 与 UDP 有独立入口。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [lualib/skynet/socket.lua:243](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/socket.lua#L243) | `function socket.open(` |
| [lualib/skynet/socket.lua:409](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/socket.lua#L409) | `function socket.listen(` |
| [lualib-src/lua-socket.c:877](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib-src/lua-socket.c#L877) | `luaopen_skynet_socketdriver(` |
| [skynet-src/socket_server.c:2138](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L2138) | `socket_server_udp(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
