# 核心 API

Skynet 的 API 设计比较简洁，掌握了这些核心 API，就能完成大部分开发工作。这里整理了实际项目中最常用的一些接口。

## 服务生命周期 API

### skynet.start(func)

启动服务，注册初始化函数：

```lua
local skynet = require "skynet"

skynet.start(function()
    -- 服务初始化代码
    
    -- 注册消息处理函数
    skynet.dispatch("lua", function(session, address, cmd, ...)
        -- 消息处理逻辑
    end)
end)
```

### skynet.exit()

退出当前服务：

```lua
skynet.dispatch("lua", function(session, address, cmd, ...)
    if cmd == "shutdown" then
        -- 清理资源
        cleanup()
        
        -- 退出服务
        skynet.exit()
    end
end)
```

### skynet.atexit(func)

注册服务退出时的清理函数：

```lua
skynet.start(function()
    local resource = acquire_resource()
    
    skynet.atexit(function()
        release_resource(resource)
    end)
end)
```

## 消息通信 API

### skynet.send(addr, type, ...)

发送消息（单向，不等待回复）：

```lua
-- 发送消息
skynet.send(target, "lua", "update", data)

-- 发送给指定地址
skynet.send(0x1000001, "lua", "ping")
```

**参数说明**：
- `addr`：目标服务地址
- `type`：消息类型（通常是 "lua"）
- `...`：消息参数

### skynet.call(addr, type, ...)

发送消息并等待回复（同步调用）：

```lua
-- 发送消息并等待回复
local result = skynet.call(target, "lua", "query", key)

-- 处理结果
if result then
    print("Query result:", result)
end
```

**参数说明**：
- `addr`：目标服务地址
- `type`：消息类型
- `...`：消息参数

**返回值**：目标服务的返回值

### skynet.ret(...)

在消息处理函数中返回结果：

```lua
skynet.dispatch("lua", function(session, address, cmd, ...)
    if cmd == "query" then
        local key = ...
        local result = get_value(key)
        
        -- 返回结果
        skynet.ret(skynet.pack(result))
    end
end)
```

### skynet.response()

忽略消息，不返回结果：

```lua
skynet.dispatch("lua", function(session, address, cmd, ...)
    if cmd == "fire_and_forget" then
        -- 忽略响应
        skynet.response()
        
        -- 处理消息但不返回结果
        process(data)
    end
end)
```

## 打包与解包

### skynet.pack(...)

打包数据为字符串：

```lua
-- 打包多个值
local data = skynet.pack(value1, value2, value3)

-- 打包表
local data = skynet.pack({key = "value"})
```

### skynet.unpack(data)

解包字符串为原始值：

```lua
-- 解包数据
local value1, value2, value3 = skynet.unpack(data)

-- 解包表
local t = skynet.unpack(data)
```

## 服务管理 API

### skynet.newservice(name, ...)

创建新的服务：

```lua
-- 创建服务
local player = skynet.newservice("player", player_id)

-- 创建服务并传递参数
local game = skynet.newservice("game", "room_1", 100)
```

**参数说明**：
- `name`：服务名称（对应 service/ 目录下的文件）
- `...`：传递给服务的参数

**返回值**：新服务的地址

### skynet.self()

获取当前服务的地址：

```lua
local my_address = skynet.self()
print("My address:", my_address)
```

### skynet.address(addr)

将地址转换为可读字符串：

```lua
local addr_str = skynet.address(addr)
print("Address:", addr_str)  -- 输出: :00000001
```

## 名称服务 API

### skynet.register(name)

注册当前服务的本地名称：

```lua
skynet.start(function()
    -- 注册服务名称
    skynet.register("player_service")
    
    skynet.dispatch("lua", function(session, address, cmd, ...)
        -- 处理消息
    end)
end)
```

### skynet.localname(name)

按本地名称查找服务：

```lua
-- 查找本地服务
local player_service = skynet.localname("player_service")

-- 发送消息
skynet.send(player_service, "lua", "update", data)
```

### skynet.name(name, addr)

注册全局名称（跨节点）：

```lua
skynet.start(function()
    -- 注册全局名称
    skynet.name("player_service", skynet.self())
end)
```

### skynet.queryservice(name)

查询全局服务：

```lua
-- 查询全局服务
local player_service = skynet.queryservice("player_service")

-- 发送消息
skynet.send(player_service, "lua", "update", data)
```

## 定时器 API

### skynet.timeout(ti, func)

设置定时器：

```lua
-- 100 毫秒后执行
skynet.timeout(100, function()
    print("Timeout!")
end)

-- 循环定时器
local function heartbeat()
    -- 处理心跳
    process_heartbeat()
    
    -- 5 秒后再次执行
    skynet.timeout(500, heartbeat)
end

skynet.start(function()
    heartbeat()
end)
```

**参数说明**：
- `ti`：超时时间（毫秒）
- `func`：超时回调函数

### skynet.now()

获取当前时间（0.01 秒为单位）：

```lua
local current_time = skynet.now()
print("Current time:", current_time)
```

### skynet.time()

获取当前时间（秒）：

```lua
local current_time = skynet.time()
print("Current time:", current_time)
```

### skynet.sleep(ti)

休眠指定时间：

```lua
-- 休眠 1 秒
skynet.sleep(100)

-- 在协程中休眠
skynet.fork(function()
    skynet.sleep(100)
    print("Wake up!")
end)
```

## 协程 API

### skynet.fork(func)

创建新协程：

```lua
-- 创建新协程
skynet.fork(function()
    print("Running in coroutine")
    skynet.sleep(100)
    print("After sleep")
end)

-- 继续执行
print("Main thread continues")
```

### skynet.fcall(addr, type, ...)

异步调用（不阻塞）：

```lua
-- 异步调用
local co = skynet.fcall(target, "lua", "query", key)

-- 做其他事情
do_something_else()

-- 等待结果
local result = skynet.wait(co)
```

### skynet.wait(co)

等待协程结果：

```lua
-- 等待异步调用结果
local result = skynet.wait(co)
```

### skynet.wakeup(co)

唤醒协程：

```lua
-- 唤醒休眠的协程
skynet.wakeup(co)
```

## 集群 API

### skynet.call(addr, type, ...) (跨节点)

跨节点调用：

```lua
-- 跨节点调用
local result = skynet.call("node2@player_service", "lua", "query", key)
```

### skynet.send(addr, type, ...) (跨节点)

跨节点发送：

```lua
-- 跨节点发送
skynet.send("node2@player_service", "lua", "update", data)
```

## 调试 API

### skynet.trace(flag)

开启/关闭消息追踪：

```lua
-- 开启追踪
skynet.trace("msg")

-- 关闭追踪
skynet.trace(false)
```

### skynet.error(...)

记录错误日志：

```lua
-- 记录错误
skynet.error("Something went wrong:", error_msg)
```

## 实用工具

### skynet.pack 与 skynet.unpack

用于消息的序列化和反序列化：

```lua
-- 打包
local data = skynet.pack(arg1, arg2, arg3)

-- 解包
local arg1, arg2, arg3 = skynet.unpack(data)
```

### skynet.tostring

将地址转换为字符串：

```lua
local addr = skynet.self()
local addr_str = skynet.tostring(addr)
print("Address:", addr_str)
```

## 完整示例

### 简单的 Echo 服务

```lua
-- echo.lua
local skynet = require "skynet"

skynet.start(function()
    print("Echo service started")
    
    skynet.dispatch("lua", function(session, address, ...)
        -- 回显消息
        skynet.ret(skynet.pack(...))
    end)
end)
```

### 简单的计数器服务

```lua
-- counter.lua
local skynet = require "skynet"

local count = 0

skynet.start(function()
    print("Counter service started")
    
    skynet.dispatch("lua", function(session, address, cmd, ...)
        if cmd == "increment" then
            count = count + 1
            skynet.ret(skynet.pack(count))
        elseif cmd == "get" then
            skynet.ret(skynet.pack(count))
        elseif cmd == "reset" then
            count = 0
            skynet.ret(skynet.pack(true))
        end
    end)
end)
```

### 使用示例

```lua
-- main.lua
local skynet = require "skynet"

skynet.start(function()
    -- 创建服务
    local echo = skynet.newservice("echo")
    local counter = skynet.newservice("counter")
    
    -- 使用 Echo 服务
    local result = skynet.call(echo, "lua", "hello", "world")
    print("Echo result:", result)
    
    -- 使用计数器服务
    local count = skynet.call(counter, "lua", "increment")
    print("Count:", count)
    
    local count = skynet.call(counter, "lua", "increment")
    print("Count:", count)
    
    local count = skynet.call(counter, "lua", "get")
    print("Current count:", count)
end)
```

## API 速查表

| API | 说明 | 是否阻塞 |
|-----|------|----------|
| `skynet.start(func)` | 启动服务 | 否 |
| `skynet.exit()` | 退出服务 | 否 |
| `skynet.send(addr, type, ...)` | 发送消息 | 否 |
| `skynet.call(addr, type, ...)` | 调用并等待 | 是 |
| `skynet.ret(...)` | 返回结果 | 否 |
| `skynet.newservice(name, ...)` | 创建服务 | 是 |
| `skynet.self()` | 获取自身地址 | 否 |
| `skynet.register(name)` | 注册本地名称 | 否 |
| `skynet.localname(name)` | 查找本地服务 | 否 |
| `skynet.timeout(ti, func)` | 设置定时器 | 否 |
| `skynet.sleep(ti)` | 休眠 | 是 |
| `skynet.fork(func)` | 创建协程 | 否 |
| `skynet.fcall(addr, type, ...)` | 异步调用 | 否 |
| `skynet.wait(co)` | 等待协程 | 是 |
| `skynet.pack(...)` | 打包数据 | 否 |
| `skynet.unpack(data)` | 解包数据 | 否 |

## 下一步

- [架构概览](/architecture/overview) - 理解 Skynet 的整体架构
- [Actor 模型](/architecture/actor-model) - 深入理解 Actor 模型
- [消息驱动设计](/architecture/message-driven) - 掌握消息系统
