# Logging

skynet.error sends runtime log text; service_logger writes logger messages. Per-service message logging has separate open/close operations in skynet_log.c. The existence of those operations is not evidence that all service traffic is persisted by default.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [lualib/skynet.lua:1026](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L1026) | `skynet.error =` |
| [service-src/service_logger.c:48](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_logger.c#L48) | `logger_cb(` |
| [skynet-src/skynet_log.c:9](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_log.c#L9) | `skynet_log_open(` |
| [skynet-src/skynet_log.c:31](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_log.c#L31) | `skynet_log_close(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
