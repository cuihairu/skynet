# 框架对比范围

本文档没有提供受控的跨框架压测，不给出吞吐或延迟排名。部署选型需要对实际业务、协程交错与本地阻塞行为做对照测量。下面只列出 Skynet 调度与等待的源码入口。

## 源码依据

| 源码 | 符号或配置 |
| --- | --- |
| [skynet-src/skynet_start.c:157](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L157) | `thread_worker(` |
| [lualib/skynet.lua:517](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L517) | `function skynet.sleep(` |

链接固定到本轮核对的源码版本。这里描述列出的执行路径，不承诺业务性能或自动状态迁移。
