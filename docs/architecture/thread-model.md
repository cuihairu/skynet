# Thread Model

The start function creates monitor, timer, socket and worker threads. The worker count defaults to 8 and is range checked against SKYNET_MAXTHREAD. The monitor loops over workers and then sleeps in five one-second steps, checking shutdown between steps.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_start.c:187](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L187) | `start(int thread)` |
| [skynet-src/skynet_start.c:95](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L95) | `thread_monitor(` |
| [skynet-src/skynet_main.c:160](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_main.c#L160) | `config.thread =` |
| [skynet-src/skynet_main.c:161](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_main.c#L161) | `config.thread < 1` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
