# skynet_timer.c Source Notes

The wheel has 256 near slots plus four levels of 64 slots. skynet_timeout creates a timeout; timer_execute and timer_shift process and advance the wheel. timer_create_timer is a static internal helper, not a public entry point. The monotonic clock is converted to centiseconds.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_timer.c:17](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L17) | `#define TIME_NEAR_SHIFT` |
| [skynet-src/skynet_timer.c:41](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L41) | `struct link_list t[4]` |
| [skynet-src/skynet_timer.c:205](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L205) | `skynet_timeout(` |
| [skynet-src/skynet_timer.c:153](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L153) | `timer_execute(` |
| [skynet-src/skynet_timer.c:112](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L112) | `timer_shift(` |
| [skynet-src/skynet_timer.c:239](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L239) | `CLOCK_MONOTONIC` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
