# skynet_handle.c Source Notes

Handle registration, retirement and lookup live here. The implementation includes per-thread reader slots and custom handle_rlock/handle_wlock helpers; the old example using only direct rwlock calls is not a literal copy of this source.

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_handle.c:15](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_handle.c#L15) | `struct handle_reader_slot` |
| [skynet-src/skynet_handle.c:51](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_handle.c#L51) | `handle_rlock(` |
| [skynet-src/skynet_handle.c:77](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_handle.c#L77) | `handle_wlock(` |
| [skynet-src/skynet_handle.c:91](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_handle.c#L91) | `skynet_handle_register(` |
| [skynet-src/skynet_handle.c:195](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_handle.c#L195) | `skynet_handle_grab(` |

References are pinned to the audited commit. These notes describe the listed code paths; they do not promise application-level performance or automatic state migration.
