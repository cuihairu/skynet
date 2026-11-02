# Build from Source

The supported target names are declared in platform.mk. This local audit builds Linux; macOS, FreeBSD, OpenBSD and MinGW need their own toolchains and runtime verification.

```sh
git clone https://github.com/cuihairu/skynet.git
cd skynet
make linux
```

Linux needs a C compiler, GNU Make and autoconf for jemalloc. The Makefile initializes the jemalloc submodule when needed. The bundled Lua header reports 5.5.1. macOS and OpenBSD disable jemalloc in platform.mk.

## Actual targets and options

- Platform builds: `make linux`, `make macosx`, `gmake freebsd`, `gmake openbsd`, `make mingw` with the required platform/cross toolchain.
- `make cleanall` removes generated build artifacts; use it deliberately before a clean rebuild.
- `MYCFLAGS` appends C compiler flags; `LDFLAGS` is used for the Skynet link step. Use command-line Make variables when overriding build settings.
- There are no root `lua`, `lpeg`, `test` or `install` targets. `cservice` and `luaclib` are directory targets, not aggregate module rebuild commands.
- `DEBUG=1`, `SANITIZE=...` and `LUA_VERSION=...` are not build switches consumed by the root Makefile. Sanitizer builds require a deliberate compiler/linker configuration; no one-flag recipe is promised here.

## Tests

```sh
bash test/run_autotest.sh testcrypt testbson testsha testcoroutine testtimeout testresponse
```

The script launches each selected Lua test in a fresh node and checks exit status and helper log markers. The full default list excludes tests needing external services or deliberate infinite loops. It is a service-test harness, not proof that every API or platform is covered. There is no `test/test.lua` or compiled `test/test` runner.

Sources: [platform.mk:2](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/platform.mk#L2); [Makefile:8](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/Makefile#L8); [Makefile:28](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/Makefile#L28); [3rd/lua/lua.h:22](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/3rd/lua/lua.h#L22); [test/run_autotest.sh:20](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/test/run_autotest.sh#L20)
