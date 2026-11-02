# 从源码构建

平台目标由 platform.mk 定义。本轮本地构建验证 Linux；macOS、FreeBSD、OpenBSD 和 MinGW 需要各自的工具链与运行验证。

```sh
git clone https://github.com/cuihairu/skynet.git
cd skynet
make linux
```

Linux 需要 C 编译器、GNU Make 与供 jemalloc 使用的 autoconf。缺少 jemalloc 时 Makefile 会初始化其子模块。内置 Lua 头文件版本为 5.5.1，macOS 与 OpenBSD 在 platform.mk 中禁用 jemalloc。

## 实际目标与选项

- 平台构建：`make linux`、`make macosx`、`gmake freebsd`、`gmake openbsd`、`make mingw`，需具备相应平台或交叉工具链。
- `make cleanall` 删除生成的构建产物，需要清理重建时再使用。
- `MYCFLAGS` 追加 C 编译参数，`LDFLAGS` 参与 Skynet 链接。覆盖构建变量应使用 Make 命令行参数。
- 根 Makefile 没有 `lua`、`lpeg`、`test` 或 `install` 目标。`cservice` 与 `luaclib` 是目录目标，不是强制重编全部模块的聚合目标。
- 根 Makefile 不读取 `DEBUG=1`、`SANITIZE=...`、`LUA_VERSION=...` 这些开关。Sanitizer 需要单独制定编译与链接配置，这里不提供不存在的一键选项。

## 测试

```sh
bash test/run_autotest.sh testcrypt testbson testsha testcoroutine testtimeout testresponse
```

脚本为每个选定 Lua 测试启动独立节点，检查退出码与 helper 日志标记。默认全集排除了依赖外部服务及故意无限循环的测试；它是服务测试 harness，不代表全部 API 或平台均被覆盖。仓库没有 `test/test.lua` 或编译后的 `test/test` 入口。

源码：[platform.mk:2](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/platform.mk#L2); [Makefile:8](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/Makefile#L8); [Makefile:28](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/Makefile#L28); [3rd/lua/lua.h:22](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/3rd/lua/lua.h#L22); [test/run_autotest.sh:20](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/test/run_autotest.sh#L20)
