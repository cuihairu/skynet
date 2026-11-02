# socket_server.c Source Notes

The server processes socket requests and polls platform events. Public entry points include poll, listen, connect and UDP operations. The application-facing Lua library and the socket C driver are separate layers.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/socket_server.c:1708](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L1708) | `socket_server_poll(` |
| [skynet-src/socket_server.c:2069](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L2069) | `socket_server_listen(` |
| [skynet-src/socket_server.c:1865](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L1865) | `socket_server_connect(` |
| [skynet-src/socket_server.c:2138](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L2138) | `socket_server_udp(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
