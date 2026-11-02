# skynet_mq.c 源码导航

服务队列初始容量为 64，需要时扩容。全局队列和服务队列操作使用自旋锁，过载阈值起始为 1024。这些是队列机制，不是吞吐保证。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_mq.c:12](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L12) | `#define DEFAULT_QUEUE_SIZE` |
| [skynet-src/skynet_mq.c:19](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L19) | `#define MQ_OVERLOAD` |
| [skynet-src/skynet_mq.c:138](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L138) | `skynet_mq_pop(` |
| [skynet-src/skynet_mq.c:190](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L190) | `skynet_mq_push(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
