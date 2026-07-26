## ![skynet logo](https://github.com/cloudwu/skynet/wiki/image/skynet_metro.jpg)

Skynet is a multi-user Lua framework supporting the actor model, often used in games.

[It is heavily used in the Chinese game industry](https://github.com/cloudwu/skynet/wiki/Uses), but is also now spreading to other industries, and to English-centric developers. To visit related sites, visit the Chinese pages using something like Google or Deepl translate.

The community is friendly and almost all contributors can speak English, so English speakers are welcome to ask questions in [Discussion](https://github.com/cloudwu/skynet/discussions), or submit issues in English.

## 源码解析文档

本项目包含完整的源码解析文档，基于 VitePress 构建。文档深入分析 Skynet 的架构设计、核心机制和实现细节。

### 本地运行文档

```bash
# 安装依赖
npm install

# 启动文档服务器
npm run docs:dev
```

访问 http://localhost:5173/skynet/ 查看文档。

### 文档结构

- **快速开始**：构建指南、项目结构、核心 API
- **架构解析**：Actor 模型、消息驱动、服务调度、网络 I/O
- **源码分析**：核心模块的详细源码分析

### 文档内容

- [架构概览](docs/architecture/overview.md) - 整体架构设计
- [Actor 模型](docs/architecture/actor-model.md) - 核心设计范式
- [消息驱动设计](docs/architecture/message-driven.md) - 消息系统设计
- [服务与模块](docs/architecture/service-module.md) - 服务管理机制
- [核心模块概览](docs/analysis/core-modules.md) - C 核心模块分析

## Build

For Linux, install autoconf first for jemalloc:

```
git clone https://github.com/cloudwu/skynet.git
cd skynet
make 'PLATFORM'  # PLATFORM can be linux, macosx, freebsd, openbsd now
```

Or:

```
export PLAT=linux
make
```

For FreeBSD , use gmake instead of make.

## Test

Run these in different consoles:

```
./skynet examples/config	# Launch first skynet node  (Gate server) and a skynet-master (see config for standalone option)
./3rd/lua/lua examples/client.lua 	# Launch a client, and try to input hello.
```

## About Lua version

Skynet now uses a modified version of lua 5.5.0 ( https://github.com/ejoy/lua/tree/skynet55 ) for multiple lua states.

Official Lua versions can also be used as long as the Makefile is edited.

## How To Use

* Read Wiki for documents https://github.com/cloudwu/skynet/wiki (Written in both English and Chinese)
* The FAQ in wiki https://github.com/cloudwu/skynet/wiki/FAQ (In Chinese, but you can visit them using something like Google or Deepl translate.)
