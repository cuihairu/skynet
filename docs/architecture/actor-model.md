# Actor Model

A service is a context with a handle, callback and queue, rather than a dedicated OS thread. Workers dispatch messages from service queues. A Lua handler that calls skynet.call or skynet.sleep yields its coroutine, so state can change before it resumes. Isolation is not a guarantee that a whole request runs without interleaving.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_server.c:42](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L42) | `struct skynet_context {` |
| [skynet-src/skynet_server.c:293](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L293) | `skynet_context_message_dispatch(` |
| [lualib/skynet.lua:728](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L728) | `function skynet.call(` |
| [lualib/skynet.lua:517](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L517) | `function skynet.sleep(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
