# Gate 服务

C gate 服务与 Lua snax.gateserver 库是不同入口。C 模块初始化协议与 socket 状态，snax.gateserver 为 Lua 服务提供 start、openclient、closeclient 入口。需要选择匹配的协议与示例，不应把它当作通用业务网关。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [service-src/service_gate.c:344](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_gate.c#L344) | `gate_init(` |
| [lualib/snax/gateserver.lua:33](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/snax/gateserver.lua#L33) | `function gateserver.start(` |
| [lualib/snax/gateserver.lua:19](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/snax/gateserver.lua#L19) | `function gateserver.openclient(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
