# Core API and Configuration

These are verified common entry points, not an exhaustive export list.

| API | Behavior | Source |
| --- | --- | --- |
| `skynet.start(func)` | Register and schedule service initialization. | [lualib/skynet.lua:1080](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L1080) |
| `skynet.dispatch(type, fn)` | Install a protocol handler. | [lualib/skynet.lua:854](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L854) |
| `skynet.send(addr, type, ...)` | Send a one-way message. | [lualib/skynet.lua:694](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L694) |
| `skynet.call(addr, type, ...)` | Yield the current coroutine until a reply or error. | [lualib/skynet.lua:728](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L728) |
| `skynet.retpack(...)` | Pack values and send a reply. | [lualib/skynet.lua:843](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L843) |
| `skynet.response(pack)` | Return a delayed-response function; call it once with a success flag and values. | [lualib/skynet.lua:801](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L801) |
| `skynet.ignoreret()` | Clear the current session association. | [lualib/skynet.lua:796](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L796) |
| `skynet.sleep(ticks, token)` | Suspend for ticks, with 100 ticks per second. | [lualib/skynet.lua:517](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L517) |
| `skynet.timeout(ticks, fn)` | Schedule a callback using the same 10ms unit. | [lualib/skynet.lua:497](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L497) |
| `skynet.newservice(name, ...)` | Ask launcher to create a service. | [lualib/skynet.lua:994](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L994) |
| `skynet.exit()` | Exit the current service; this does not register an exit callback. | [lualib/skynet.lua:649](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L649) |

`skynet.atexit` is not part of this Lua API. Put cleanup in the service exit flow instead of relying on that callback. Import `skynet.manager` before using register/name/kill/abort.

`skynet.pack` returns a lightuserdata pointer plus a size, not a Lua string. `skynet.unpack` consumes the serialized data. Use `skynet.packstring` when a string is required.

[lualib/skynet/manager.lua:53](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet/manager.lua#L53); [lualib/skynet.lua:710](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L710); [lualib-src/lua-seri.c:566](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib-src/lua-seri.c#L566); [lualib/skynet.lua:711](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/lualib/skynet.lua#L711)

## Runtime configuration defaults

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

Pass the config file as a positional argument: `./skynet examples/config`. Examples may override these defaults. Service and library paths are configured in the example below; they are not separate CLI flags.

[skynet-src/skynet_main.c:128](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_main.c#L128); [examples/config.path:2](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/examples/config.path#L2)
