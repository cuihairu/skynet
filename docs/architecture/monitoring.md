# Monitoring and Debugging

The monitor compares worker execution markers to detect a message handler that appears stuck. Lua debug commands expose runtime inspection and injected execution. Detection and debug access do not impose a timeout that safely interrupts arbitrary business code.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_monitor.c:38](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_monitor.c#L38) | `skynet_monitor_check(` |
| [skynet-src/skynet_start.c:95](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L95) | `thread_monitor(` |
| [lualib/skynet/debug.lua:84](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/debug.lua#L84) | `function dbgcmd.RUN(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
