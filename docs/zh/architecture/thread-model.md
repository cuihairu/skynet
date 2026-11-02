# 线程模型

start 创建 monitor、timer、socket 与 worker 线程。worker 默认数为 8，并按 SKYNET_MAXTHREAD 检查范围。monitor 检查各 worker 后，分五次各睡眠一秒，间隔检查退出条件。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_start.c:187](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L187) | `start(int thread)` |
| [skynet-src/skynet_start.c:95](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L95) | `thread_monitor(` |
| [skynet-src/skynet_main.c:160](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_main.c#L160) | `config.thread =` |
| [skynet-src/skynet_main.c:161](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_main.c#L161) | `config.thread < 1` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
