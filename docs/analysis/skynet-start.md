# skynet_start.c Source Notes

skynet_start initializes the runtime and bootstrap service. start creates the auxiliary and worker threads. thread_timer polls after usleep(2500); timer ticks still use centiseconds.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_start.c:266](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L266) | `skynet_start(` |
| [skynet-src/skynet_start.c:187](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L187) | `start(int thread)` |
| [skynet-src/skynet_start.c:140](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L140) | `usleep(2500)` |
| [skynet-src/skynet_timer.c:232](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L232) | `ti.tv_nsec / 10000000` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
