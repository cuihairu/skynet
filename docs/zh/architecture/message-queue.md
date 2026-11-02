# 消息队列

message_queue 用环形缓冲存放一个服务的消息，全局队列存放可运行的服务队列。服务缓冲初始容量为 64，满时扩容。队列操作使用自旋锁，不是无锁实现。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_mq.c:12](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L12) | `#define DEFAULT_QUEUE_SIZE` |
| [skynet-src/skynet_mq.c:44](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L44) | `skynet_globalmq_push(` |
| [skynet-src/skynet_mq.c:190](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L190) | `skynet_mq_push(` |
| [skynet-src/skynet_mq.c:47](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L47) | `SPIN_LOCK(q)` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
