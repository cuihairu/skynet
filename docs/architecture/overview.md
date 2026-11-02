# Architecture Overview

Skynet is a C/Lua actor framework forked from cloudwu/skynet. A service context owns a message queue; worker threads dispatch queued messages. Lua services use coroutines to suspend a request while another request can run. Service isolation does not remove the need to protect shared C data.

The configured worker default is **8**, not an automatic CPU count. The timer uses **10ms ticks**; the timer thread polling every 2.5ms does not provide 2.5ms callback precision. Message logging is a separate path, not a record of every interaction by default.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_server.c:125](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L125) | `skynet_context_new(` |
| [skynet-src/skynet_server.c:293](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L293) | `skynet_context_message_dispatch(` |
| [skynet-src/skynet_main.c:160](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_main.c#L160) | `config.thread =` |
| [skynet-src/skynet_timer.c:232](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L232) | `ti.tv_nsec / 10000000` |
| [skynet-src/skynet_start.c:140](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L140) | `usleep(2500)` |
| [skynet-src/skynet_log.c:9](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_log.c#L9) | `skynet_log_open(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
