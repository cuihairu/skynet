# 服务与模块

C 模块加载器通过 cpath 查找模块。snlua 是承载 Lua 状态的 C 服务模块，Lua 服务脚本由 loader 和 luaservice 配置查找。skynet.newservice 请求 launcher 创建服务，skynet.uniqueservice 走服务管理器。加载模块与创建服务实例是不同操作。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_module.c:104](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_module.c#L104) | `skynet_module_query(` |
| [service-src/service_snlua.c:517](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_snlua.c#L517) | `snlua_create(` |
| [lualib/skynet.lua:994](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L994) | `function skynet.newservice(` |
| [lualib/skynet.lua:998](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L998) | `function skynet.uniqueservice(` |
| [lualib/loader.lua:11](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/loader.lua#L11) | `LUA_SERVICE` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
