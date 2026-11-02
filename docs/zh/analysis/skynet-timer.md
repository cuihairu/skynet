# skynet_timer.c 源码导航

定时轮有 256 个近端槽及四层各 64 个上层槽。skynet_timeout 创建超时，timer_execute 与 timer_shift 执行并推进定时轮。timer_create_timer 是内部静态辅助函数，非公开入口；单调时钟换算为百分之一秒。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_timer.c:17](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L17) | `#define TIME_NEAR_SHIFT` |
| [skynet-src/skynet_timer.c:41](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L41) | `struct link_list t[4]` |
| [skynet-src/skynet_timer.c:205](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L205) | `skynet_timeout(` |
| [skynet-src/skynet_timer.c:153](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L153) | `timer_execute(` |
| [skynet-src/skynet_timer.c:112](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L112) | `timer_shift(` |
| [skynet-src/skynet_timer.c:239](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L239) | `CLOCK_MONOTONIC` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
