# Skynet

<p align="center"><img src="docs/public/logo.svg" width="96" alt="Skynet logo"></p>

<p align="center"><a href="https://cuihairu.github.io/skynet/"><img src="https://img.shields.io/badge/docs-English-blue" alt="English documentation"></a> <a href="https://cuihairu.github.io/skynet/zh/"><img src="https://img.shields.io/badge/docs-简体中文-blue" alt="简体中文文档"></a> <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-green" alt="MIT license"></a></p>

本仓库 fork 自 [cloudwu/skynet](https://github.com/cloudwu/skynet)，底座是 C/Lua actor 框架。本仓补充中英文源码阅读笔记，覆盖部分运行路径，不是完整 API 规范，也不提供业务状态自动热迁移保证。

[English](README.md)

## 文档

- [中文文档](https://cuihairu.github.io/skynet/zh/)
- [从源码构建](docs/zh/getting-started/build-from-source.md)
- [核心 API 与配置](docs/zh/getting-started/api-reference.md)
- [架构概览](docs/zh/architecture/overview.md)
- [源码入口](docs/zh/analysis/core-modules.md)

站点基础路径是 `/skynet/`，中文页位于 `/skynet/zh/`。本轮核实的公开入口是 GitHub Pages。自定义域名镜像未能通过 TLS 连通性验证，因此不列为可用入口。

## 构建与测试

Linux 安装 C 编译器、GNU Make 和 autoconf 后执行：

```sh
git clone https://github.com/cuihairu/skynet.git
cd skynet
make linux
bash test/run_autotest.sh testcrypt testbson testsha testcoroutine testtimeout testresponse
```

测试 helper 会启动临时 Skynet 节点。交互示例需要在两个终端分别运行 `./skynet examples/config` 和 `./3rd/lua/lua examples/client.lua`。其他平台目标见 [platform.mk](platform.mk)，存在目标不表示本轮 Linux 环境已经验证所有平台。

内置 Lua 源码版本为 **5.5.1**，依据是 [3rd/lua/lua.h](3rd/lua/lua.h)。

## 本地文档

文档工具链使用 pnpm。请按 [pnpm.io](https://pnpm.io/zh/installation) 安装；Node.js 22.12–24 也可用内置 Corepack 执行 `corepack enable pnpm` 启用，Node.js 25+ 不再内置 Corepack。然后执行：

```sh
pnpm install --frozen-lockfile
pnpm run docs:check
pnpm run docs:dev
```

启动后打开终端所示端口的 `/skynet/zh/` 路径。默认请求端口是 5173，若已有服务占用，应使用实际分配的端口。生产构建运行 `pnpm run docs:build`，预览运行 `pnpm run docs:preview`。

## 上游与许可证

- [上游 Wiki](https://github.com/cloudwu/skynet/wiki)
- [上游讨论区](https://github.com/cloudwu/skynet/discussions)
- [MIT 许可证](LICENSE)
