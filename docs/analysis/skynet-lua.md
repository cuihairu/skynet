# skynet.lua Source Notes

The Lua runtime implements request sessions, coroutine suspension, dispatch and service startup. response returns a delayed response function; retpack sends serialized values.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [lualib/skynet.lua:728](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L728) | `function skynet.call(` |
| [lualib/skynet.lua:801](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L801) | `function skynet.response(` |
| [lualib/skynet.lua:843](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L843) | `function skynet.retpack(` |
| [lualib/skynet.lua:1080](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L1080) | `function skynet.start(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
