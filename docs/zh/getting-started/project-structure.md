# 项目结构

下面列出当前仓库的源码入口。路径与函数依据见各主题页。

| Path | 内容 |
| --- | --- |
| [skynet-src/](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/) | C 运行时、队列、句柄、定时器与 socket |
| [lualib/skynet.lua](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua) | 核心 Lua API |
| [lualib/skynet/manager.lua](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/manager.lua) | 管理扩展 |
| [lualib/skynet/socket.lua](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/socket.lua) | Lua socket 库 |
| [lualib-src/lua-socket.c](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib-src/lua-socket.c) | C socket driver 注册 |
| [service/](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service/) | Lua 运行服务 |
| [service-src/](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/) | C 服务模块 |
| [examples/config](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/examples/config) | 交互示例配置 |
| [examples/config.path](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/examples/config.path) | loader 与库搜索路径 |
| [test/run_autotest.sh](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/test/run_autotest.sh) | 服务测试 harness |
| [3rd/lua/lua.h](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/3rd/lua/lua.h) | 内置 Lua 版本 |
| [Makefile](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/Makefile) | 构建目标与源码清单 |

`skynet.manager` 的文件是 `lualib/skynet/manager.lua`，不是 `lualib/skynet/skynet.manager.lua`。socketdriver 由 C 注册，不存在所列的同名 Lua 实现文件。

[lualib-src/lua-socket.c:877](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib-src/lua-socket.c#L877)
