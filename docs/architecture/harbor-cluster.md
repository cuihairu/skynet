# Harbor and Cluster

Harbor routes messages between configured harbor nodes and maintains global-name operations. The cluster library is a separate Lua-facing interface with node configuration, call, send and reload operations. Neither path alone provides automatic service-state migration.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_harbor.c:20](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_harbor.c#L20) | `skynet_harbor_send(` |
| [service-src/service_harbor.c:732](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_harbor.c#L732) | `harbor_init(` |
| [lualib/skynet/cluster.lua:60](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/cluster.lua#L60) | `function cluster.call(` |
| [lualib/skynet/cluster.lua:88](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/cluster.lua#L88) | `function cluster.reload(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
