# Socket Abstraction

The Lua socket library exposes open, listen, read and write operations over skynet.socketdriver. The socket driver is a C module registered by lua-socket.c, not a socketdriver.lua file. TCP and UDP have separate entry points in socket_server.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [lualib/skynet/socket.lua:243](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/socket.lua#L243) | `function socket.open(` |
| [lualib/skynet/socket.lua:409](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/socket.lua#L409) | `function socket.listen(` |
| [lualib-src/lua-socket.c:877](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib-src/lua-socket.c#L877) | `luaopen_skynet_socketdriver(` |
| [skynet-src/socket_server.c:2138](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L2138) | `socket_server_udp(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
