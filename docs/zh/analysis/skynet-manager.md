# skynet.manager 源码导航

require skynet.manager 会向 skynet 表补充 launch、kill、abort、register 与 name 等管理操作；不能假设未导入该模块时这些入口已经存在。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [lualib/skynet/manager.lua:16](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/manager.lua#L16) | `function skynet.launch(` |
| [lualib/skynet/manager.lua:23](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/manager.lua#L23) | `function skynet.kill(` |
| [lualib/skynet/manager.lua:53](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/manager.lua#L53) | `function skynet.register(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
