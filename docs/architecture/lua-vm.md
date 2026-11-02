# Lua VM Management

Each snlua service instance creates a Lua state through lua_newstate and closes it during release. The bundled Lua header reports 5.5.1. An independent VM does not automatically replace running closures or migrate service data when a file changes.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [service-src/service_snlua.c:517](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_snlua.c#L517) | `snlua_create(` |
| [service-src/service_snlua.c:529](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_snlua.c#L529) | `snlua_release(` |
| [3rd/lua/lua.h:22](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/3rd/lua/lua.h#L22) | `#define LUA_VERSION_RELEASE_N` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
