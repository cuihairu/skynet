# Runtime Code Injection Boundaries

The debug RUN command executes injected source through skynet.inject. This is a debugging mechanism, not a guarantee of transparent hot reload. Updating existing closures, preserving invariants and migrating live state require an application-specific procedure. This documentation does not claim that editing a Lua file updates all running services.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [lualib/skynet/debug.lua:84](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/debug.lua#L84) | `function dbgcmd.RUN(` |
| [lualib/skynet/inject.lua:6](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/inject.lua#L6) | `return` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
