# Service Scheduling

Each worker repeatedly calls skynet_context_message_dispatch and sleeps on a condition variable when it has no queue. The weight argument changes how many queued messages are processed in a dispatch batch; it is not an OS thread priority or a strict latency/fairness guarantee.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_start.c:157](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L157) | `thread_worker(` |
| [skynet-src/skynet_server.c:293](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L293) | `skynet_context_message_dispatch(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
