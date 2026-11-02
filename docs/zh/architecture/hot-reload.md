# 运行时代码注入的边界

debug 的 RUN 命令通过 skynet.inject 执行注入源码。这是调试机制，不是透明热更新保证。替换既有闭包、保持不变量与迁移运行态需要业务自己的流程。这里不宣称修改 Lua 文件就会更新所有运行中的服务。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [lualib/skynet/debug.lua:84](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/debug.lua#L84) | `function dbgcmd.RUN(` |
| [lualib/skynet/inject.lua:6](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/inject.lua#L6) | `return` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
