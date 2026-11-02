# 消息驱动设计

skynet.send 发送单向消息；skynet.call 关联请求 session，并让出协程等待响应。处理函数可以用 skynet.retpack 返回序列化值。skynet.response 返回用于延迟响应的函数，不是通用的“忽略消息”接口；skynet.ignoreret 清除当前请求的 session 关联。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [lualib/skynet.lua:694](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L694) | `function skynet.send(` |
| [lualib/skynet.lua:728](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L728) | `function skynet.call(` |
| [lualib/skynet.lua:843](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L843) | `function skynet.retpack(` |
| [lualib/skynet.lua:801](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L801) | `function skynet.response(` |
| [lualib/skynet.lua:796](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L796) | `function skynet.ignoreret(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
