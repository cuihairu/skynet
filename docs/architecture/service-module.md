# Services and Modules

The C module loader resolves a module through cpath. snlua is the C service module used to host a Lua state; Lua service scripts are selected through the loader and luaservice configuration. skynet.newservice asks the launcher to start a service; skynet.uniqueservice uses the service manager. Loading a module is distinct from creating a service instance.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_module.c:104](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_module.c#L104) | `skynet_module_query(` |
| [service-src/service_snlua.c:517](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_snlua.c#L517) | `snlua_create(` |
| [lualib/skynet.lua:994](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L994) | `function skynet.newservice(` |
| [lualib/skynet.lua:998](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L998) | `function skynet.uniqueservice(` |
| [lualib/loader.lua:11](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/loader.lua#L11) | `LUA_SERVICE` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
