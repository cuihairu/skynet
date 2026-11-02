# Network I/O

socket_server polls sockets and issues events. skynet_socket translates those events into service messages. The socket polling abstraction selects a platform backend; the Lua socket API manages waiting coroutines. Blocking application code can still block a worker.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/socket_server.c:1708](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L1708) | `socket_server_poll(` |
| [skynet-src/skynet_socket.c:79](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_socket.c#L79) | `skynet_socket_poll(` |
| [skynet-src/socket_poll.h:1](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_poll.h#L1) | `#if` |
| [lualib/skynet/socket.lua:305](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/socket.lua#L305) | `function socket.read(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
