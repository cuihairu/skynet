# Message Queue

A message_queue stores a ring buffer for one service. The global queue holds runnable service queues. The initial per-service buffer capacity is 64 and expands when full. Spinlocks protect queue operations; this implementation is not lock-free.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_mq.c:12](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L12) | `#define DEFAULT_QUEUE_SIZE` |
| [skynet-src/skynet_mq.c:44](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L44) | `skynet_globalmq_push(` |
| [skynet-src/skynet_mq.c:190](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L190) | `skynet_mq_push(` |
| [skynet-src/skynet_mq.c:47](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L47) | `SPIN_LOCK(q)` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
