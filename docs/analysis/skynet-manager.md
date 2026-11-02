# skynet.manager Source Notes

Requiring skynet.manager adds management operations to the skynet table, including launch, kill, abort, register and name. Those operations should not be assumed to exist before that import.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [lualib/skynet/manager.lua:16](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/manager.lua#L16) | `function skynet.launch(` |
| [lualib/skynet/manager.lua:23](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/manager.lua#L23) | `function skynet.kill(` |
| [lualib/skynet/manager.lua:53](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/manager.lua#L53) | `function skynet.register(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
