# Memory Management

The default Linux build links jemalloc; platform.mk disables it on macOS and OpenBSD. Allocation wrappers maintain service memory statistics. Lua snlua uses its own allocator callback and memory-limit reporting; memory attribution does not remove the need to release native resources.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [Makefile:28](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/Makefile#L28) | `JEMALLOC_STATICLIB :=` |
| [platform.mk:39](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/platform.mk#L39) | `openbsd macosx : SKYNET_DEFINES` |
| [skynet-src/malloc_hook.c:174](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/malloc_hook.c#L174) | `skynet_malloc(` |
| [service-src/service_snlua.c:483](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/service-src/service_snlua.c#L483) | `lalloc(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
