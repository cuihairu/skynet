# 核心 API 与配置

这里列出已核对的常用入口，不是全部导出清单。

| API | 行为 | 源码 |
| --- | --- | --- |
| `skynet.start(func)` | 注册并安排服务初始化。 | [lualib/skynet.lua:1080](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L1080) |
| `skynet.dispatch(type, fn)` | 安装协议处理函数。 | [lualib/skynet.lua:854](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L854) |
| `skynet.send(addr, type, ...)` | 发送单向消息。 | [lualib/skynet.lua:694](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L694) |
| `skynet.call(addr, type, ...)` | 让出当前协程，等待响应或错误。 | [lualib/skynet.lua:728](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L728) |
| `skynet.retpack(...)` | 打包值并发送响应。 | [lualib/skynet.lua:843](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L843) |
| `skynet.response(pack)` | 返回延迟响应函数，使用成功标志与值调用一次。 | [lualib/skynet.lua:801](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L801) |
| `skynet.ignoreret()` | 清除当前 session 关联。 | [lualib/skynet.lua:796](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L796) |
| `skynet.sleep(ticks, token)` | 挂起指定 tick，100 tick 为一秒。 | [lualib/skynet.lua:517](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L517) |
| `skynet.timeout(ticks, fn)` | 按同样的 10ms 单位安排回调。 | [lualib/skynet.lua:497](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L497) |
| `skynet.newservice(name, ...)` | 请求 launcher 创建服务。 | [lualib/skynet.lua:994](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L994) |
| `skynet.exit()` | 退出当前服务，不是注册退出回调。 | [lualib/skynet.lua:649](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L649) |

`skynet.atexit` 不在当前 Lua API 中。需要清理时由服务自己的退出流程负责，不能依赖不存在的回调。管理接口 register/name/kill/abort 需要先 `require "skynet.manager"`。

`skynet.pack` 返回 lightuserdata 指针与长度，不是 Lua 字符串；`skynet.unpack` 消费相应的序列化数据。需要字符串时使用 `skynet.packstring`。

[lualib/skynet/manager.lua:53](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/manager.lua#L53); [lualib/skynet.lua:710](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L710); [lualib-src/lua-seri.c:566](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib-src/lua-seri.c#L566); [lualib/skynet.lua:711](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L711)

## 运行配置默认值

| Key | Default |
| --- | --- |
| thread | 8 |
| harbor | 1 |
| cpath | ./cservice/?.so |
| bootstrap | snlua bootstrap |
| logger | nil |
| logservice | logger |
| daemon | nil |
| profile | true |

[skynet-src/skynet_main.c:160](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_main.c#L160)

配置文件必须作为位置参数传入：`./skynet examples/config`。示例文件可能显式覆盖默认值。服务库路径的配置见下列示例，不是独立的 CLI flags。

[skynet-src/skynet_main.c:128](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_main.c#L128); [examples/config.path:2](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/examples/config.path#L2)
