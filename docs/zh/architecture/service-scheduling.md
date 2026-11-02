# 服务调度

worker 循环调用 skynet_context_message_dispatch，没有队列时等待条件变量。weight 影响一次分派批次处理的消息数量，不是操作系统线程优先级，也不提供严格的延迟或公平性保证。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_start.c:157](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L157) | `thread_worker(` |
| [skynet-src/skynet_server.c:293](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L293) | `skynet_context_message_dispatch(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
