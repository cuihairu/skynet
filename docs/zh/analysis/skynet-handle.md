# skynet_handle.c 源码导航

本文件实现句柄注册、注销与查找。当前实现包含每线程 reader slot 以及 handle_rlock、handle_wlock 包装，旧文档中仅使用直接 rwlock 调用的示例不是当前源码的原样抄录。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_handle.c:15](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_handle.c#L15) | `struct handle_reader_slot` |
| [skynet-src/skynet_handle.c:51](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_handle.c#L51) | `handle_rlock(` |
| [skynet-src/skynet_handle.c:77](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_handle.c#L77) | `handle_wlock(` |
| [skynet-src/skynet_handle.c:91](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_handle.c#L91) | `skynet_handle_register(` |
| [skynet-src/skynet_handle.c:195](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_handle.c#L195) | `skynet_handle_grab(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
