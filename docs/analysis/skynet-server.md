# skynet_server.c Source Notes

Service contexts are created by skynet_context_new. skynet_send routes messages, and skynet_context_message_dispatch runs callbacks from queues. Handle lookup is delegated to skynet_handle.c.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_server.c:125](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L125) | `skynet_context_new(` |
| [skynet-src/skynet_server.c:121](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L121) | `skynet_send(` |
| [skynet-src/skynet_server.c:293](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L293) | `skynet_context_message_dispatch(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
