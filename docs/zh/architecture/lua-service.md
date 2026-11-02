# Lua 服务

loader 根据 LUA_SERVICE 定位并加载 Lua 服务脚本，传入启动参数。skynet.start 通过运行时安排初始化，服务脚本通常用 skynet.dispatch 注册协议处理函数。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [lualib/loader.lua:11](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/loader.lua#L11) | `LUA_SERVICE` |
| [lualib/skynet.lua:1080](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L1080) | `function skynet.start(` |
| [lualib/skynet.lua:854](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L854) | `function skynet.dispatch(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
