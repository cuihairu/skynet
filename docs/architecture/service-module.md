# 服务与模块

服务是 Skynet 中的核心概念，每个服务都是一个独立的 Actor。在实际项目中，我们发现理解服务的创建和管理是正确使用 Skynet 的关键。

## 服务的概念

### 什么是服务

在 Skynet 中，**服务 (Service)** 是：
- 一个独立的执行单元，拥有自己的生命周期
- 拥有自己的消息队列，通过消息与其他服务交互
- 可以处理消息并发送消息，实现业务逻辑
- 通过回调函数定义行为，支持动态替换

### 服务 vs 线程

```
传统多线程：
┌─────────┐ ┌─────────┐ ┌─────────┐
│ 线程 1  │ │ 线程 2  │ │ 线程 3  │
│ (共享内存) │ │ (共享内存) │ │ (共享内存) │
└─────────┘ └─────────┘ └─────────┘

Skynet 服务：
┌─────────┐ ┌─────────┐ ┌─────────┐
│ 服务 1  │ │ 服务 2  │ │ 服务 3  │
│ (独立内存) │ │ (独立内存) │ │ (独立内存) │
└────┬────┘ └────┬────┘ └────┬────┘
     │           │           │
     └───────────┼───────────┘
                 │
           ┌─────▼─────┐
           │ 消息传递  │
           └───────────┘
```

## 服务类型

### 1. C 服务

用 C 语言实现的服务，性能高，适合底层功能：

```c
// C 服务接口
struct skynet_module {
    const char *name;
    void *module;
    void *(*init)(const char *args);      // 初始化
    void (*release)(void *inst);          // 释放
    void (*signal)(void *inst, int signal); // 信号处理
};
```

**示例**：logger、harbor、gate 等

### 2. Lua 服务

用 Lua 实现的服务，开发效率高，支持热更新：

```lua
-- Lua 服务模板
local skynet = require "skynet"

skynet.start(function()
    -- 初始化代码
    
    skynet.dispatch("lua", function(session, address, cmd, ...)
        -- 消息处理逻辑
    end)
end)
```

**示例**：游戏逻辑、业务处理等

## 服务生命周期

### 状态转换

```
        ┌─────────────────────────────────────────────┐
        │                                             │
        ▼                                             │
   ┌─────────┐    创建服务    ┌─────────┐            │
   │  不存在 │ ─────────────→ │  初始化 │            │
   └─────────┘               └────┬────┘            │
                                   │ 初始化完成      │
                                   ▼                  │
                              ┌─────────┐            │
                              │  运行中 │            │
                              └────┬────┘            │
                                   │                  │
        ┌──────────────────────────┼──────────────────┘
        │                          │
        │                          │ 服务退出
        ▼                          ▼
   ┌─────────┐               ┌─────────┐
   │  挂起   │               │  销毁   │
   └─────────┘               └─────────┘
```

### 服务创建

```c
// skynet_server.c 中的服务创建

struct skynet_context * skynet_context_new(const char *name, const char *param) {
    // 1. 加载模块
    struct skynet_module *mod = skynet_module_query(name);
    
    // 2. 创建实例
    void *inst = skynet_module_instance_create(mod, param);
    
    // 3. 创建上下文
    struct skynet_context *ctx = skynet_malloc(sizeof(*ctx));
    ctx->mod = mod;
    ctx->instance = inst;
    
    // 4. 创建消息队列
    ctx->queue = skynet_mq_create(ctx->handle);
    
    // 5. 注册服务
    skynet_handle_register(ctx);
    
    return ctx;
}
```

### 服务销毁

```c
// skynet_server.c 中的服务销毁

void skynet_context_release(struct skynet_context *ctx) {
    // 减少引用计数
    if (__sync_sub_and_fetch(&ctx->ref, 1) == 0) {
        // 释放实例
        skynet_module_instance_release(ctx->mod, ctx->instance);
        
        // 释放消息队列
        skynet_mq_release(ctx->queue);
        
        // 注销服务
        skynet_handle_retire(ctx->handle);
        
        // 释放内存
        skynet_free(ctx);
    }
}
```

## 服务管理

### 服务句柄

每个服务都有一个唯一的句柄：

```c
struct handle_storage {
    struct rwlock lock;
    uint32_t harbor;           // Harbor ID
    int slot_size;             // 槽位大小
    struct skynet_context **slot; // 服务数组
};
```

### 句柄查找

```c
// 按句柄查找服务
struct skynet_context * skynet_handle_grab(uint32_t handle) {
    struct handle_storage *s = H;
    rwlock_rlock(&s->lock);
    
    struct skynet_context *result = NULL;
    int index = handle & (s->slot_size - 1);
    struct skynet_context *ctx = s->slot[index];
    
    if (ctx && ctx->handle == handle) {
        result = ctx;
        skynet_context_grab(result);
    }
    
    rwlock_runlock(&s->lock);
    return result;
}
```

### 名称服务

服务可以通过名称注册和查找：

```lua
-- 注册服务名称
skynet.register("player_service")

-- 按名称查找服务
local player_service = skynet.localname("player_service")

-- 全局名称服务（跨节点）
skynet.name("player_service", skynet.self())
local player_service = skynet.queryservice("player_service")
```

## 服务间通信

### 直接通信

```lua
-- 发送消息
skynet.send(target_handle, "lua", cmd, ...)

-- 调用并等待结果
local result = skynet.call(target_handle, "lua", cmd, ...)
```

### 通过名称通信

```lua
-- 通过名称发送
local target = skynet.localname("service_name")
skynet.send(target, "lua", cmd, ...)

-- 通过名称调用
local target = skynet.localname("service_name")
local result = skynet.call(target, "lua", cmd, ...)
```

### 广播消息

```lua
-- 向所有服务广播
skynet.send_broadcast("lua", "announcement", message)
```

## 服务配置

### 启动配置

```lua
-- config 文件
skynet.start(function()
    -- 启动服务
    local logger = skynet.newservice("logger")
    local gate = skynet.newservice("gate", "ws://0.0.0.0:8001")
    local game = skynet.newservice("game")
    
    -- 设置全局服务
    skynet.name(".logger", logger)
    skynet.name(".gate", gate)
    skynet.name(".game", game)
end)
```

### 服务参数

```lua
-- 创建服务时传递参数
local player = skynet.newservice("player", player_id, player_data)

-- 服务接收参数
local skynet = require "skynet"
local player_id, player_data = ...

skynet.start(function()
    -- 使用参数初始化
    init_player(player_id, player_data)
end)
```

## 服务模块

### 模块加载

```c
// skynet_module.c 中的模块加载

struct skynet_module * skynet_module_query(const char *name) {
    // 1. 查找已加载的模块
    struct skynet_module *mod = find_module(name);
    if (mod) return mod;
    
    // 2. 加载新模块
    mod = skynet_malloc(sizeof(*mod));
    mod->name = name;
    mod->module = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
    
    // 3. 获取函数指针
    mod->init = dlsym(mod->module, "skynet_module_create");
    mod->release = dlsym(mod->module, "skynet_module_release");
    mod->signal = dlsym(mod->module, "skynet_module_signal");
    
    return mod;
}
```

### 模块接口

```c
// C 模块必须实现的接口

// 创建实例
void * skynet_module_create(const char *args) {
    struct my_module *inst = malloc(sizeof(*inst));
    // 初始化...
    return inst;
}

// 释放实例
void skynet_module_release(void *inst) {
    struct my_module *m = inst;
    // 清理资源...
    free(m);
}

// 信号处理
void skynet_module_signal(void *inst, int signal) {
    struct my_module *m = inst;
    // 处理信号...
}
```

## 服务示例

### 1. Logger 服务

```lua
-- logger.lua
local skynet = require "skynet"

local log_file

skynet.start(function()
    log_file = io.open("server.log", "a")
    
    skynet.dispatch("lua", function(session, address, ...)
        local msg = table.concat({...}, " ")
        local timestamp = os.date("%Y-%m-%d %H:%M:%S")
        log_file:write(string.format("[%s] %s\n", timestamp, msg))
        log_file:flush()
    end)
end)
```

### 2. 数据库服务

```lua
-- db_service.lua
local skynet = require "skynet"
local mysql = require "skynet.db.mysql"

local db

skynet.start(function()
    db = mysql.connect({
        host = "127.0.0.1",
        port = 3306,
        database = "game",
        user = "root",
        password = "password"
    })
    
    skynet.dispatch("lua", function(session, address, cmd, ...)
        if cmd == "query" then
            local sql = ...
            local result = db:query(sql)
            skynet.ret(skynet.pack(result))
        elseif cmd == "execute" then
            local sql = ...
            local result = db:execute(sql)
            skynet.ret(skynet.pack(result))
        end
    end)
end)
```

### 3. 玩家服务

```lua
-- player_service.lua
local skynet = require "skynet"

local players = {}

skynet.start(function()
    skynet.dispatch("lua", function(session, address, cmd, ...)
        if cmd == "login" then
            local player_id, player_data = ...
            players[player_id] = {
                address = address,
                data = player_data
            }
            skynet.ret(skynet.pack(true))
        elseif cmd == "logout" then
            local player_id = ...
            players[player_id] = nil
            skynet.ret(skynet.pack(true))
        elseif cmd == "get_player" then
            local player_id = ...
            skynet.ret(skynet.pack(players[player_id]))
        end
    end)
end)
```

## 服务间协作

### 请求-响应模式

```lua
-- 服务 A
local result = skynet.call(service_b, "lua", "process", data)

-- 服务 B
skynet.dispatch("lua", function(session, address, cmd, data)
    if cmd == "process" then
        local result = heavy_processing(data)
        skynet.ret(skynet.pack(result))
    end
end)
```

### 流水线模式

```lua
-- 服务 A → 服务 B → 服务 C
local step1 = skynet.call(service_a, "lua", "step1", data)
local step2 = skynet.call(service_b, "lua", "step2", step1)
local step3 = skynet.call(service_c, "lua", "step3", step2)
```

### 并行处理

```lua
-- 并行调用多个服务
local co1 = skynet.fcall(service_a, "lua", "task1")
local co2 = skynet.fcall(service_b, "lua", "task2")
local co3 = skynet.fcall(service_c, "lua", "task3")

-- 等待所有结果
local result1 = skynet.wait(co1)
local result2 = skynet.wait(co2)
local result3 = skynet.wait(co3)
```

## 最佳实践

### 1. 服务粒度

```lua
-- 好：职责单一的服务
local player_service = {}      -- 只处理玩家逻辑
local inventory_service = {}   -- 只处理背包逻辑
local quest_service = {}       -- 只处理任务逻辑

-- 差：职责混乱的服务
local god_service = {}         -- 处理所有逻辑
```

### 2. 错误处理

```lua
-- 服务应该处理错误而不是崩溃
skynet.dispatch("lua", function(session, address, cmd, ...)
    local ok, result = pcall(function()
        return handle_command(cmd, ...)
    end)
    
    if ok then
        skynet.ret(skynet.pack(result))
    else
        skynet.ret(skynet.pack(nil, result))
        log("Error in service: " .. tostring(result))
    end
end)
```

### 3. 资源管理

```lua
-- 在服务退出时清理资源
skynet.start(function()
    -- 初始化资源
    local resource = acquire_resource()
    
    skynet.dispatch("lua", function(session, address, cmd, ...)
        -- 使用资源
    end)
    
    -- 注册退出清理
    skynet.atexit(function()
        release_resource(resource)
    end)
end)
```

## 下一步

- [消息队列实现](/architecture/message-queue) - 深入消息队列的实现
- [服务调度](/architecture/service-scheduling) - 理解服务如何被调度
- [skynet_server.c 分析](/analysis/skynet-server) - 服务管理的源码分析
