# Lua 虚拟机管理

每个 snlua 服务实例通过 lua_newstate 创建 Lua 状态，并在释放时关闭。内置 Lua 头文件标明 5.5.1。独立 VM 不会因为文件变化就自动替换运行中的闭包或迁移服务数据。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [service-src/service_snlua.c:517](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_snlua.c#L517) | `snlua_create(` |
| [service-src/service_snlua.c:529](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_snlua.c#L529) | `snlua_release(` |
| [3rd/lua/lua.h:22](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/3rd/lua/lua.h#L22) | `#define LUA_VERSION_RELEASE_N` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
