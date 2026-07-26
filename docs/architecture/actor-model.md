# Actor 模型

Actor 模型是 Skynet 的核心设计范式。在实际项目中，我们发现理解这个模型对于正确使用 Skynet 至关重要。

## 什么是 Actor 模型

Actor 模型是 Carl Hewitt 在 1973 年提出的并发计算模型。从实际应用角度看，它的核心思想是：

- **Actor** 是计算的基本单元，每个 Actor 独立运行
- 每个 Actor 拥有私有状态，不与其他 Actor 共享
- Actor 之间通过消息传递通信，避免了锁的复杂性
- 每个 Actor 一次处理一条消息，保证了线程安全

### Actor 的行为

一个 Actor 可以执行以下操作：

1. **发送消息**：向其他 Actor 发送消息
2. **创建 Actor**：创建新的 Actor
3. **改变状态**：修改自己的私有状态
4. **指定行为**：决定如何处理下一条消息

```
┌─────────────────────────────────────────┐
│                Actor                    │
│  ┌─────────┐                           │
│  │  状态   │                           │
│  └─────────┘                           │
│       │                                │
│       ▼                                │
│  ┌─────────┐    ┌─────────┐           │
│  │ 邮箱   │ → │ 处理逻辑 │           │
│  └─────────┘    └─────────┘           │
│                      │                 │
│         ┌────────────┼────────────┐    │
│         ▼            ▼            ▼    │
│    发送消息     创建 Actor    修改状态  │
└─────────────────────────────────────────┘
```

## Skynet 中的 Actor 实现

### 服务即 Actor

在 Skynet 中，每个 **服务 (Service)** 就是一个 Actor：

```c
struct skynet_context {
    void *instance;           // Actor 实例（状态）
    struct skynet_module *mod; // Actor 类型
    struct message_queue *queue; // Actor 邮箱
    skynet_cb cb;             // Actor 行为（消息处理函数）
    // ...
};
```

### 消息传递

Actor 之间通过 `skynet_send()` 发送消息：

```c
int skynet_send(
    struct skynet_context *context,  // 发送者
    uint32_t source,                // 源地址
    uint32_t destination,           // 目标地址
    int type,                       // 消息类型
    int session,                    // 会话 ID
    void *data,                     // 消息数据
    size_t size                     // 消息大小
);
```

### 消息处理

每个 Actor 通过回调函数处理消息：

```c
typedef int (*skynet_cb)(
    struct skynet_context *ctx,  // Actor 上下文
    void *ud,                    // 用户数据
    int type,                    // 消息类型
    int session,                 // 会话 ID
    uint32_t source,             // 消息来源
    const void *msg,             // 消息内容
    size_t sz                    // 消息大小
);
```

## Lua 中的 Actor

### 创建 Actor

```lua
local skynet = require "skynet"

-- 创建一个新的 Actor（服务）
local actor = skynet.newservice("my_actor")
```

### 定义 Actor 行为

```lua
-- my_actor.lua
local skynet = require "skynet"

-- Actor 的状态
local state = {
    count = 0,
    data = {}
}

-- Actor 的消息处理函数
skynet.start(function()
    skynet.dispatch("lua", function(session, address, cmd, ...)
        if cmd == "add" then
            state.count = state.count + 1
            skynet.ret(skynet.pack(state.count))
        elseif cmd == "get" then
            skynet.ret(skynet.pack(state.count))
        elseif cmd == "set" then
            local key, value = ...
            state.data[key] = value
            skynet.ret(skynet.pack(true))
        end
    end)
end)
```

### 与 Actor 通信

```lua
-- 发送消息并等待回复
local count = skynet.call(actor, "lua", "add")
local value = skynet.call(actor, "lua", "get")

-- 单向发送（不等待回复）
skynet.send(actor, "lua", "set", "key", "value")
```

## Actor 模型的优势

### 1. 避免共享状态

传统多线程编程中，多个线程访问共享数据需要加锁：

```c
// 传统方式：需要锁
pthread_mutex_lock(&mutex);
shared_data++;
pthread_mutex_unlock(&mutex);

// Actor 方式：无需锁
skynet.send(actor, "lua", "increment")
```

### 2. 天然并发

每个 Actor 独立运行，可以并行处理消息：

```
时间线：
Actor A: ──[处理消息1]──[处理消息2]──[处理消息3]──
Actor B: ──[处理消息1]──[处理消息2]──[处理消息3]──
Actor C: ──[处理消息1]──[处理消息2]──[处理消息3]──
```

### 3. 易于理解和测试

Actor 的行为完全由消息处理逻辑决定，没有副作用，易于测试：

```lua
-- 测试 Actor
function test_actor()
    local actor = skynet.newservice("test_actor")
    
    -- 发送消息并验证结果
    local result = skynet.call(actor, "lua", "add")
    assert(result == 1, "Expected 1, got " .. result)
    
    local result = skynet.call(actor, "lua", "add")
    assert(result == 2, "Expected 2, got " .. result)
end
```

## Actor 模式的应用场景

### 1. 游戏实体

每个游戏实体（玩家、NPC、怪物）可以是一个 Actor：

```lua
-- player.lua
local skynet = require "skynet"

local player = {
    id = nil,
    name = nil,
    hp = 100,
    position = {x = 0, y = 0, z = 0}
}

skynet.start(function()
    skynet.dispatch("lua", function(session, address, cmd, ...)
        if cmd == "move" then
            local x, y, z = ...
            player.position = {x = x, y = y, z = z}
            -- 广播给周围的玩家
            broadcast_nearby("player_move", player.id, x, y, z)
        elseif cmd == "attack" then
            local target_id = ...
            -- 处理攻击逻辑
        end
    end)
end)
```

### 2. 管理器服务

各种管理器可以实现为 Actor：

```lua
-- room_manager.lua
local skynet = require "skynet"

local rooms = {}

skynet.start(function()
    skynet.dispatch("lua", function(session, address, cmd, ...)
        if cmd == "create_room" then
            local room_id = create_room(...)
            skynet.ret(skynet.pack(room_id))
        elseif cmd == "join_room" then
            local room_id, player_id = ...
            join_room(room_id, player_id)
        end
    end)
end)
```

### 3. 网关服务

网关可以作为 Actor 处理客户端连接：

```lua
-- gate.lua
local skynet = require "skynet"
local socket = require "skynet.socket"

local connections = {}

skynet.start(function()
    skynet.dispatch("lua", function(session, address, cmd, ...)
        if cmd == "new_connection" then
            local fd = ...
            handle_new_connection(fd)
        elseif cmd == "disconnect" then
            local fd = ...
            handle_disconnect(fd)
        end
    end)
end)
```

## 与其他并发模型的对比

| 特性 | Actor 模型 | 线程模型 | 协程模型 |
|------|-----------|---------|---------|
| 共享状态 | 无 | 有 | 有限 |
| 锁需求 | 无 | 需要 | 视情况 |
| 并发粒度 | 消息级 | 线程级 | 协程级 |
| 调度方式 | 消息驱动 | 抢占式 | 协作式 |
| 适用场景 | 游戏、聊天 | 通用 | IO密集 |

## 最佳实践

### 1. 单一职责

每个 Actor 应该只负责一个功能：

```lua
-- 好：职责单一
local player_actor = {}      -- 只处理玩家逻辑
local inventory_actor = {}   -- 只处理背包逻辑
local quest_actor = {}       -- 只处理任务逻辑

-- 差：职责混乱
local god_actor = {}         -- 处理所有逻辑
```

### 2. 消息设计

消息应该简洁明了：

```lua
-- 好：明确的消息
skynet.call(actor, "lua", "get_player_info", player_id)
skynet.call(actor, "lua", "update_hp", player_id, new_hp)

-- 差：模糊的消息
skynet.call(actor, "lua", "do_something", "player", player_id, "hp", new_hp)
```

### 3. 错误处理

Actor 应该优雅地处理错误：

```lua
skynet.dispatch("lua", function(session, address, cmd, ...)
    local ok, result = pcall(function()
        return handle_command(cmd, ...)
    end)
    
    if ok then
        skynet.ret(skynet.pack(result))
    else
        skynet.ret(skynet.pack(nil, result))  -- 返回错误信息
    end
end)
```

## 下一步

- [消息驱动设计](/architecture/message-driven) - 深入理解消息系统
- [服务与模块](/architecture/service-module) - 服务的创建和管理
- [skynet_server.c 分析](/analysis/skynet-server) - 源码层面的 Actor 实现
