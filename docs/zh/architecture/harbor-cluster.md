# Harbor 与 Cluster

Harbor 在已配置节点间路由消息，并提供全局名字相关操作。cluster 库是另一套 Lua 接口，提供节点配置、call、send 和 reload。两条路径本身都不提供业务状态自动迁移。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_harbor.c:20](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_harbor.c#L20) | `skynet_harbor_send(` |
| [service-src/service_harbor.c:732](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_harbor.c#L732) | `harbor_init(` |
| [lualib/skynet/cluster.lua:60](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/cluster.lua#L60) | `function cluster.call(` |
| [lualib/skynet/cluster.lua:88](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/cluster.lua#L88) | `function cluster.reload(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
