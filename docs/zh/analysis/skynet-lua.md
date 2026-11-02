# skynet.lua 源码导航

Lua 运行层实现请求 session、协程挂起、分派与服务启动。response 返回延迟响应函数，retpack 发送序列化值。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [lualib/skynet.lua:728](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L728) | `function skynet.call(` |
| [lualib/skynet.lua:801](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L801) | `function skynet.response(` |
| [lualib/skynet.lua:843](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L843) | `function skynet.retpack(` |
| [lualib/skynet.lua:1080](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L1080) | `function skynet.start(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
