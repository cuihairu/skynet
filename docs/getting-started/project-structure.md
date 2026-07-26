# 项目结构

初次接触 Skynet 时，了解项目结构是第一步。这里整理了各个目录和文件的作用，方便快速定位代码。

## 根目录结构

```
skynet/
├── skynet-src/         # C 核心源码
├── lualib/             # Lua 库
├── lualib-src/         # Lua C 扩展
├── service/            # Lua 服务示例
├── service-src/        # C 服务源码
├── examples/           # 示例配置和脚本
├── test/               # 测试代码
├── 3rd/                # 第三方依赖
├── platform.mk         # 平台配置
├── Makefile            # 主构建文件
└── README.md           # 项目说明
```

## 核心源码目录

### skynet-src/

C 语言实现的核心框架：

```
skynet-src/
├── skynet.h                # 主头文件
├── skynet_main.c           # 程序入口
├── skynet_start.c          # 系统启动
├── skynet_server.c         # 服务管理
├── skynet_mq.c             # 消息队列
├── skynet_handle.c         # 服务句柄管理
├── skynet_module.c         # 动态模块加载
├── skynet_timer.c          # 定时器
├── skynet_socket.c         # 网络 I/O
├── skynet_harbor.c         # 集群支持
├── skynet_daemon.c         # 守护进程
├── skynet_log.c            # 日志系统
├── skynet_env.c            # 环境变量
├── skynet_error.c          # 错误处理
├── socket_server.c         # Socket 服务器
├── socket_poll.h           # Socket 轮询抽象
├── socket_epoll.h          # epoll 实现
├── socket_kqueue.h         # kqueue 实现
├── socket_info.h           # Socket 信息
├── socket_buffer.h         # Socket 缓冲区
├── malloc_hook.c           # 内存分配钩子
├── mem_info.c              # 内存信息
├── spinlock.h              # 自旋锁
├── rwlock.h                # 读写锁
└── atomic.h                # 原子操作
```

#### 核心模块职责

| 文件 | 职责 |
|------|------|
| `skynet_main.c` | 程序入口，解析命令行参数，加载配置 |
| `skynet_start.c` | 初始化各子系统，创建工作线程 |
| `skynet_server.c` | 服务的创建、销毁、消息处理 |
| `skynet_mq.c` | 消息队列的实现 |
| `skynet_handle.c` | 服务句柄的注册和查找 |
| `skynet_module.c` | 动态模块（.so）的加载 |
| `skynet_timer.c` | 定时器的实现 |
| `skynet_socket.c` | 网络 I/O 的封装 |
| `skynet_harbor.c` | 跨节点服务调用 |
| `socket_server.c` | 底层 Socket 操作 |

## Lua 库目录

### lualib/

Skynet 提供的 Lua 库：

```
lualib/
├── skynet/                 # 核心模块
│   ├── skynet.lua          # 主模块
│   ├── skynet.coroutine.lua # 协程支持
│   ├── skynet.manager.lua  # 管理功能
│   ├── skynet.monitor.lua  # 监控功能
│   ├── skynet.cluster.lua  # 集群支持
│   └── skynet.harbor.lua   # Harbor 支持
├── snax.lua                # SNAX 框架
├── snax_interface.lua      # SNAX 接口
├── proto.lua               # 协议处理
├── bson.lua                # BSON 编解码
├── md5.lua                 # MD5 哈希
├── netpack.lua             # 网络包处理
├── socketdriver.lua        # Socket 驱动
├── sockethelper.lua        # Socket 辅助函数
├── string                  # 字符串处理
│   ├── hashkey.lua         # 哈希键
│   └── urlencode.lua       # URL 编码
├── debugger.lua            # 调试器
├── interactive.lua         # 交互式调试
├── mongo.lua               # MongoDB 客户端
├── redis.lua               # Redis 客户端
├── mysql.lua               # MySQL 客户端
└── http                    # HTTP 支持
    ├── httpd.lua           # HTTP 服务器
    ├── httpc.lua           # HTTP 客户端
    └── websocket.lua       # WebSocket 支持
```

#### 核心 Lua 模块

**skynet.lua** - 主模块

```lua
local skynet = require "skynet"

-- 核心 API
skynet.start(func)          -- 启动服务
skynet.dispatch(type, func) -- 注册消息处理
skynet.send(addr, type, ...) -- 发送消息
skynet.call(addr, type, ...) -- 调用并等待结果
skynet.ret(...)             -- 返回结果
skynet.newservice(name, ...) -- 创建新服务
skynet.self()               -- 获取当前服务地址
skynet.exit()               -- 退出服务
skynet.timeout(ti, func)    -- 定时器
```

**skynet.coroutine.lua** - 协程支持

```lua
-- 协程操作
skynet.fcall(addr, type, ...) -- 异步调用
skynet.wait(co)              -- 等待协程结果
skynet.wakeup(co)            -- 唤醒协程
skynet.yield()               -- 让出执行
```

### lualib-src/

Lua 的 C 扩展模块：

```
lualib-src/
├── lua-skynet.c            # Skynet Lua 绑定
├── lua-seri.c              # 序列化
├── lua-socket.c            # Socket 绑定
├── lua-netpack.c           # 网络包处理
├── lua-mongo.c             # MongoDB 绑定
├── lua-redis.c             # Redis 绑定
├── lua-bson.c              # BSON 绑定
├── lua-md5.c               # MD5 绑定
├── lua-sharetable.c        # 共享表
└── lpeg/                   # LPEG 模式匹配
    ├── lpeg.c
    └── lpcode.c
```

## 服务目录

### service/

Lua 服务实现：

```
service/
├── service.lua             # 基础服务
├── logger.lua              # 日志服务
├── gate.lua                # 网关服务
├── master.lua              # 主控服务
├── slave.lua               # 从属服务
├── harbor.lua              # Harbor 服务
├── debug_console.lua       # 调试控制台
├── cmaster.lua             # 集群主控
├── cslave.lua              # 集群从属
└── snaxd.lua               # SNAX 服务守护
```

### service-src/

C 服务实现：

```
service-src/
├── service_logger.c        # 日志服务
├── service_gate.c          # 网关服务
├── service_harbor.c        # Harbor 服务
└── service_master.c        # 主控服务
```

## 示例目录

### examples/

示例配置和脚本：

```
examples/
├── config                  # 配置文件示例
├── config.test             # 测试配置
├── config.log              # 日志配置
├── client.lua              # 客户端示例
├── main.lua                # 主服务示例
├── proto.lua               # 协议定义
├── watchdog.lua            # 看门狗服务
├── echo.lua                # 回显服务
├── simpledb.lua            # 简单数据库
└── login.lua               # 登录服务
```

#### 配置文件示例

```lua
-- examples/config
include "config.path"

-- 服务启动配置
start = "main"  -- 主服务

-- 日志配置
logpath = "log"
logservice = "logger"

-- 网络配置
thread = 8
harbor = 0
```

## 测试目录

### test/

测试代码：

```
test/
├── test.lua                # 主测试脚本
├── teststring.lua          # 字符串测试
├── testcoroutine.lua       # 协程测试
├── testsocket.lua          # Socket 测试
├── testtimer.lua           # 定时器测试
├── testmemory.lua          # 内存测试
└── testmulticast.lua       # 组播测试
```

## 第三方依赖

### 3rd/

第三方库：

```
3rd/
├── lua/                    # Lua 5.5 (修改版)
│   ├── lua-5.5.0/
│   ├── Makefile
│   └── ...
├── jemalloc/               # jemalloc 内存分配器
│   ├── jemalloc-5.3.0/
│   └── Makefile
├── lpeg/                   # LPEG 模式匹配
│   ├── lpeg-1.0.2/
│   └── Makefile
└── pb/                     # Protocol Buffers
    ├── protobuf/
    └── Makefile
```

## 构建产物

构建后生成的文件：

```
skynet/
├── skynet                  # 主程序
├── lua                     # Lua 解释器
├── luac                    # Lua 编译器
├── cservice/               # C 服务模块
│   ├── logger.so
│   ├── gate.so
│   ├── harbor.so
│   └── ...
├── luaclib/                # Lua C 模块
│   ├── bson.so
│   ├── md5.so
│   ├── lpeg.so
│   └── ...
└── log/                    # 日志目录
```

## 关键文件说明

### 配置文件

**config.path** - 路径配置

```lua
-- 服务路径
lua_path = root .. "lualib/?.lua;" .. root .. "lualib/?/init.lua"
lua_cpath = root .. "luaclib/?.so"
lua_service = root .. "service/?.lua;" .. root .. "examples/?.lua"
```

**config** - 主配置

```lua
-- 启动服务
start = "main"

-- 网络配置
thread = 8
harbor = 0

-- 日志配置
logpath = "log"
logservice = "logger"
```

### 协议文件

**proto.lua** - 协议定义

```lua
local proto = {}

proto.c2s = skynet.proto_request {
    name = "c2s",
    id = 1,
}

proto.s2c = skynet.proto_response {
    name = "s2c",
    id = 2,
}

return proto
```

## 开发环境建议

### 推荐目录结构

对于实际项目，建议这样的结构：

```
my_game/
├── skynet/                 # Skynet 框架（作为子模块）
├── service/                # 游戏服务
│   ├── game/              # 游戏逻辑
│   ├── login/             # 登录服务
│   └── gate/              # 网关服务
├── lualib/                 # 游戏 Lua 库
├── proto/                  # 协议定义
├── config/                 # 配置文件
└── Makefile                # 构建文件
```

## 下一步

- [核心 API](/getting-started/api-reference) - 学习 Skynet 的核心 API
- [架构概览](/architecture/overview) - 理解 Skynet 的整体架构
- [源码分析](/analysis/core-modules) - 深入分析核心源码
