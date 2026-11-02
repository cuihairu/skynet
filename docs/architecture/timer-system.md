# Timer System

The timer has 256 near slots and four upper levels of 64 slots. Timeouts use centiseconds (10ms), including the arguments to skynet.sleep and skynet.timeout. Dispatch delay also depends on service scheduling; this is not a real-time timer.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_timer.c:17](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L17) | `#define TIME_NEAR_SHIFT` |
| [skynet-src/skynet_timer.c:19](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L19) | `#define TIME_LEVEL_SHIFT` |
| [skynet-src/skynet_timer.c:41](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L41) | `struct link_list t[4]` |
| [skynet-src/skynet_timer.c:205](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L205) | `skynet_timeout(` |
| [lualib/skynet.lua:497](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L497) | `function skynet.timeout(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
