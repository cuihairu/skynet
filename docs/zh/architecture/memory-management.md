# 内存管理

默认 Linux 构建链接 jemalloc；platform.mk 在 macOS 和 OpenBSD 上禁用它。分配包装层维护服务内存统计，snlua 使用分配回调及内存限制报告。内存归属统计不能替代本地资源释放。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [Makefile:28](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/Makefile#L28) | `JEMALLOC_STATICLIB :=` |
| [platform.mk:39](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/platform.mk#L39) | `openbsd macosx : SKYNET_DEFINES` |
| [skynet-src/malloc_hook.c:174](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/malloc_hook.c#L174) | `skynet_malloc(` |
| [service-src/service_snlua.c:483](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_snlua.c#L483) | `lalloc(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
