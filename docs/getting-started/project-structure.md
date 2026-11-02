# Project Structure

These are source entry points in this repository. The topic pages link to the actual files and functions.

| Path | Contents |
| --- | --- |
| [skynet-src/](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/) | C runtime, queues, handles, timers and sockets |
| [lualib/skynet.lua](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua) | Core Lua API |
| [lualib/skynet/manager.lua](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/manager.lua) | Management extensions |
| [lualib/skynet/socket.lua](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/socket.lua) | Lua socket library |
| [lualib-src/lua-socket.c](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib-src/lua-socket.c) | C socket driver registration |
| [service/](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service/) | Lua runtime services |
| [service-src/](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/) | C service modules |
| [examples/config](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/examples/config) | Interactive example configuration |
| [examples/config.path](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/examples/config.path) | Loader and library search paths |
| [test/run_autotest.sh](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/test/run_autotest.sh) | Service-test harness |
| [3rd/lua/lua.h](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/3rd/lua/lua.h) | Bundled Lua version |
| [Makefile](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/Makefile) | Build targets and source lists |

`skynet.manager` resolves to `lualib/skynet/manager.lua`, not `lualib/skynet/skynet.manager.lua`. socketdriver is registered in C rather than a same-named Lua implementation file.

[lualib-src/lua-socket.c:877](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib-src/lua-socket.c#L877)
