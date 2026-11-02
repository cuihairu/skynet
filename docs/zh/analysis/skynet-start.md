# skynet_start.c 源码导航

skynet_start 初始化运行时并启动 bootstrap 服务，start 创建辅助线程与 worker。thread_timer 使用 usleep(2500) 轮询，定时器 tick 仍以百分之一秒为单位。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_start.c:266](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L266) | `skynet_start(` |
| [skynet-src/skynet_start.c:187](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L187) | `start(int thread)` |
| [skynet-src/skynet_start.c:140](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L140) | `usleep(2500)` |
| [skynet-src/skynet_timer.c:232](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L232) | `ti.tv_nsec / 10000000` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
