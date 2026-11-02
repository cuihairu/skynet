# 日志

skynet.error 发送运行日志文本，service_logger 写 logger 消息。单服务消息日志在 skynet_log.c 中有独立的开启与关闭入口；不能因为存在这些入口就声称默认保存所有服务通信。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [lualib/skynet.lua:1026](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L1026) | `skynet.error =` |
| [service-src/service_logger.c:48](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_logger.c#L48) | `logger_cb(` |
| [skynet-src/skynet_log.c:9](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_log.c#L9) | `skynet_log_open(` |
| [skynet-src/skynet_log.c:31](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_log.c#L31) | `skynet_log_close(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
