# Core API

Skynet's API design is relatively simple. Once you have mastered these core APIs, you can complete most of the development work. This page collects some of the most commonly used interfaces in real projects.

## Service Lifecycle API

### skynet.start(func)

Start a service and register an initialization function:

```lua
local skynet = require "skynet"

skynet.start(function()
    -- Service initialization code
    
    -- Register a message handler
    skynet.dispatch("lua", function(session, address, cmd, ...)
        -- Message handling logic
    end)
end)
```

### skynet.exit()

Exit the current service:

```lua
skynet.dispatch("lua", function(session, address, cmd, ...)
    if cmd == "shutdown" then
        -- Clean up resources
        cleanup()
        
        -- Exit the service
        skynet.exit()
    end
end)
```

### skynet.atexit(func)

Register a cleanup function for when the service exits:

```lua
skynet.start(function()
    local resource = acquire_resource()
    
    skynet.atexit(function()
        release_resource(resource)
    end)
end)
```

## Message Communication API

### skynet.send(addr, type, ...)

Send a message (one-way, no reply is awaited):

```lua
-- Send a message
skynet.send(target, "lua", "update", data)

-- Send to a specific address
skynet.send(0x1000001, "lua", "ping")
```

**Parameters**:
- `addr`: Target service address
- `type`: Message type (usually "lua")
- `...`: Message arguments

### skynet.call(addr, type, ...)

Send a message and wait for a reply (synchronous call):

```lua
-- Send a message and wait for a reply
local result = skynet.call(target, "lua", "query", key)

-- Handle the result
if result then
    print("Query result:", result)
end
```

**Parameters**:
- `addr`: Target service address
- `type`: Message type
- `...`: Message arguments

**Return value**: The return value from the target service

### skynet.ret(...)

Return a result from within a message handler:

```lua
skynet.dispatch("lua", function(session, address, cmd, ...)
    if cmd == "query" then
        local key = ...
        local result = get_value(key)
        
        -- Return the result
        skynet.ret(skynet.pack(result))
    end
end)
```

### skynet.response()

Ignore the message without returning a result:

```lua
skynet.dispatch("lua", function(session, address, cmd, ...)
    if cmd == "fire_and_forget" then
        -- Ignore the response
        skynet.response()
        
        -- Process the message without returning a result
        process(data)
    end
end)
```

## Packing and Unpacking

### skynet.pack(...)

Pack data into a string:

```lua
-- Pack multiple values
local data = skynet.pack(value1, value2, value3)

-- Pack a table
local data = skynet.pack({key = "value"})
```

### skynet.unpack(data)

Unpack a string back into its original values:

```lua
-- Unpack data
local value1, value2, value3 = skynet.unpack(data)

-- Unpack a table
local t = skynet.unpack(data)
```

## Service Management API

### skynet.newservice(name, ...)

Create a new service:

```lua
-- Create a service
local player = skynet.newservice("player", player_id)

-- Create a service and pass arguments
local game = skynet.newservice("game", "room_1", 100)
```

**Parameters**:
- `name`: Service name (corresponding to a file under the service/ directory)
- `...`: Arguments passed to the service

**Return value**: The address of the new service

### skynet.self()

Get the address of the current service:

```lua
local my_address = skynet.self()
print("My address:", my_address)
```

### skynet.address(addr)

Convert an address into a human-readable string:

```lua
local addr_str = skynet.address(addr)
print("Address:", addr_str)  -- Output: :00000001
```

## Name Service API

### skynet.register(name)

Register a local name for the current service:

```lua
skynet.start(function()
    -- Register the service name
    skynet.register("player_service")
    
    skynet.dispatch("lua", function(session, address, cmd, ...)
        -- Handle messages
    end)
end)
```

### skynet.localname(name)

Look up a service by local name:

```lua
-- Look up a local service
local player_service = skynet.localname("player_service")

-- Send a message
skynet.send(player_service, "lua", "update", data)
```

### skynet.name(name, addr)

Register a global name (cross-node):

```lua
skynet.start(function()
    -- Register a global name
    skynet.name("player_service", skynet.self())
end)
```

### skynet.queryservice(name)

Query a global service:

```lua
-- Query a global service
local player_service = skynet.queryservice("player_service")

-- Send a message
skynet.send(player_service, "lua", "update", data)
```

## Timer API

### skynet.timeout(ti, func)

Set a timer:

```lua
-- Execute after 100 milliseconds
skynet.timeout(100, function()
    print("Timeout!")
end)

-- Repeating timer
local function heartbeat()
    -- Handle the heartbeat
    process_heartbeat()
    
    -- Execute again after 5 seconds
    skynet.timeout(500, heartbeat)
end

skynet.start(function()
    heartbeat()
end)
```

**Parameters**:
- `ti`: Timeout duration (in milliseconds)
- `func`: Timeout callback function

### skynet.now()

Get the current time (in units of 0.01 seconds):

```lua
local current_time = skynet.now()
print("Current time:", current_time)
```

### skynet.time()

Get the current time (in seconds):

```lua
local current_time = skynet.time()
print("Current time:", current_time)
```

### skynet.sleep(ti)

Sleep for a specified duration:

```lua
-- Sleep for 1 second
skynet.sleep(100)

-- Sleep inside a coroutine
skynet.fork(function()
    skynet.sleep(100)
    print("Wake up!")
end)
```

## Coroutine API

### skynet.fork(func)

Create a new coroutine:

```lua
-- Create a new coroutine
skynet.fork(function()
    print("Running in coroutine")
    skynet.sleep(100)
    print("After sleep")
end)

-- Continue execution
print("Main thread continues")
```

### skynet.fcall(addr, type, ...)

Asynchronous call (non-blocking):

```lua
-- Asynchronous call
local co = skynet.fcall(target, "lua", "query", key)

-- Do something else
do_something_else()

-- Wait for the result
local result = skynet.wait(co)
```

### skynet.wait(co)

Wait for a coroutine's result:

```lua
-- Wait for the result of an asynchronous call
local result = skynet.wait(co)
```

### skynet.wakeup(co)

Wake up a coroutine:

```lua
-- Wake up a sleeping coroutine
skynet.wakeup(co)
```

## Cluster API

### skynet.call(addr, type, ...) (Cross-Node)

Cross-node call:

```lua
-- Cross-node call
local result = skynet.call("node2@player_service", "lua", "query", key)
```

### skynet.send(addr, type, ...) (Cross-Node)

Cross-node send:

```lua
-- Cross-node send
skynet.send("node2@player_service", "lua", "update", data)
```

## Debugging API

### skynet.trace(flag)

Enable/disable message tracing:

```lua
-- Enable tracing
skynet.trace("msg")

-- Disable tracing
skynet.trace(false)
```

### skynet.error(...)

Log an error:

```lua
-- Log an error
skynet.error("Something went wrong:", error_msg)
```

## Practical Utilities

### skynet.pack and skynet.unpack

Used for serializing and deserializing messages:

```lua
-- Pack
local data = skynet.pack(arg1, arg2, arg3)

-- Unpack
local arg1, arg2, arg3 = skynet.unpack(data)
```

### skynet.tostring

Convert an address into a string:

```lua
local addr = skynet.self()
local addr_str = skynet.tostring(addr)
print("Address:", addr_str)
```

## Complete Examples

### A Simple Echo Service

```lua
-- echo.lua
local skynet = require "skynet"

skynet.start(function()
    print("Echo service started")
    
    skynet.dispatch("lua", function(session, address, ...)
        -- Echo the message
        skynet.ret(skynet.pack(...))
    end)
end)
```

### A Simple Counter Service

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

### Usage Example

```lua
-- main.lua
local skynet = require "skynet"

skynet.start(function()
    -- Create services
    local echo = skynet.newservice("echo")
    local counter = skynet.newservice("counter")
    
    -- Use the Echo service
    local result = skynet.call(echo, "lua", "hello", "world")
    print("Echo result:", result)
    
    -- Use the counter service
    local count = skynet.call(counter, "lua", "increment")
    print("Count:", count)
    
    local count = skynet.call(counter, "lua", "increment")
    print("Count:", count)
    
    local count = skynet.call(counter, "lua", "get")
    print("Current count:", count)
end)
```

## API Quick Reference

| API | Description | Blocking |
|-----|------|----------|
| `skynet.start(func)` | Start a service | No |
| `skynet.exit()` | Exit a service | No |
| `skynet.send(addr, type, ...)` | Send a message | No |
| `skynet.call(addr, type, ...)` | Call and wait | Yes |
| `skynet.ret(...)` | Return a result | No |
| `skynet.newservice(name, ...)` | Create a service | Yes |
| `skynet.self()` | Get own address | No |
| `skynet.register(name)` | Register a local name | No |
| `skynet.localname(name)` | Look up a local service | No |
| `skynet.timeout(ti, func)` | Set a timer | No |
| `skynet.sleep(ti)` | Sleep | Yes |
| `skynet.fork(func)` | Create a coroutine | No |
| `skynet.fcall(addr, type, ...)` | Asynchronous call | No |
| `skynet.wait(co)` | Wait for a coroutine | Yes |
| `skynet.pack(...)` | Pack data | No |
| `skynet.unpack(data)` | Unpack data | No |

## Next Steps

- [Architecture Overview](/architecture/overview) - Understand Skynet's overall architecture
- [Actor Model](/architecture/actor-model) - Deeply understand the Actor model
- [Message-Driven Design](/architecture/message-driven) - Master the messaging system
