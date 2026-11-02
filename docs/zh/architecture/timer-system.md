# 定时器系统

定时轮有 256 个近端槽和四层各 64 个上层槽。超时参数以百分之一秒，即 10ms 为单位，skynet.sleep 和 skynet.timeout 也采用这一单位。消息分派还受服务调度影响，不是实时定时器。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_timer.c:17](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L17) | `#define TIME_NEAR_SHIFT` |
| [skynet-src/skynet_timer.c:19](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L19) | `#define TIME_LEVEL_SHIFT` |
| [skynet-src/skynet_timer.c:41](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L41) | `struct link_list t[4]` |
| [skynet-src/skynet_timer.c:205](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L205) | `skynet_timeout(` |
| [lualib/skynet.lua:497](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L497) | `function skynet.timeout(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
