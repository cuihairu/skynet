# skynet_mq.c Source Notes

A service queue starts with capacity 64 and expands as needed. Both global and service queue operations use spinlocks. The overload threshold starts at 1024. These are queue mechanics, not a throughput guarantee.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_mq.c:12](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L12) | `#define DEFAULT_QUEUE_SIZE` |
| [skynet-src/skynet_mq.c:19](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L19) | `#define MQ_OVERLOAD` |
| [skynet-src/skynet_mq.c:138](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L138) | `skynet_mq_pop(` |
| [skynet-src/skynet_mq.c:190](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L190) | `skynet_mq_push(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
