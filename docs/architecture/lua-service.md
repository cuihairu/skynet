# Lua Services

The loader locates a Lua service using LUA_SERVICE, loads the script, and passes startup arguments. skynet.start schedules initialization through the runtime. Service scripts normally register a protocol handler with skynet.dispatch.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [lualib/loader.lua:11](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/loader.lua#L11) | `LUA_SERVICE` |
| [lualib/skynet.lua:1080](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L1080) | `function skynet.start(` |
| [lualib/skynet.lua:854](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L854) | `function skynet.dispatch(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
