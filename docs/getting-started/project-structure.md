# Project Structure

When first getting started with Skynet, understanding the project structure is the first step. This page lists the role of every directory and file so you can locate code quickly.

## Root Directory Structure

```
skynet/
├── skynet-src/         # C core source code
├── lualib/             # Lua libraries
├── lualib-src/         # Lua C extensions
├── service/            # Lua service examples
├── service-src/        # C service source code
├── examples/           # Example configs and scripts
├── test/               # Test code
├── 3rd/                # Third-party dependencies
├── platform.mk         # Platform configuration
├── Makefile            # Main build file
└── README.md           # Project description
```

## Core Source Code Directory

### skynet-src/

The core framework implemented in C:

```
skynet-src/
├── skynet.h                # Main header file
├── skynet_main.c           # Program entry point
├── skynet_start.c          # System startup
├── skynet_server.c         # Service management
├── skynet_mq.c             # Message queue
├── skynet_handle.c         # Service handle management
├── skynet_module.c         # Dynamic module loading
├── skynet_timer.c          # Timers
├── skynet_socket.c         # Network I/O
├── skynet_harbor.c         # Cluster support
├── skynet_daemon.c         # Daemon process
├── skynet_log.c            # Logging system
├── skynet_env.c            # Environment variables
├── skynet_error.c          # Error handling
├── socket_server.c         # Socket server
├── socket_poll.h           # Socket polling abstraction
├── socket_epoll.h          # epoll implementation
├── socket_kqueue.h         # kqueue implementation
├── socket_info.h           # Socket information
├── socket_buffer.h         # Socket buffer
├── malloc_hook.c           # Memory allocation hook
├── mem_info.c              # Memory information
├── spinlock.h              # Spinlock
├── rwlock.h                # Read-write lock
└── atomic.h                # Atomic operations
```

#### Core Module Responsibilities

| File | Responsibility |
|------|------|
| `skynet_main.c` | Program entry point, parses command-line arguments, loads configuration |
| `skynet_start.c` | Initializes the various subsystems and creates the worker threads |
| `skynet_server.c` | Creating, destroying, and message handling for services |
| `skynet_mq.c` | Implementation of the message queue |
| `skynet_handle.c` | Registration and lookup of service handles |
| `skynet_module.c` | Loading of dynamic modules (.so) |
| `skynet_timer.c` | Implementation of timers |
| `skynet_socket.c` | Wrapping of network I/O |
| `skynet_harbor.c` | Cross-node service calls |
| `socket_server.c` | Low-level socket operations |

## Lua Library Directory

### lualib/

The Lua libraries provided by Skynet:

```
lualib/
├── skynet/                 # Core modules
│   ├── skynet.lua          # Main module
│   ├── skynet.coroutine.lua # Coroutine support
│   ├── skynet.manager.lua  # Manager functions
│   ├── skynet.monitor.lua  # Monitoring functions
│   ├── skynet.cluster.lua  # Cluster support
│   └── skynet.harbor.lua   # Harbor support
├── snax.lua                # SNAX framework
├── snax_interface.lua      # SNAX interface
├── proto.lua               # Protocol handling
├── bson.lua                # BSON encoding/decoding
├── md5.lua                 # MD5 hashing
├── netpack.lua             # Network packet handling
├── socketdriver.lua        # Socket driver
├── sockethelper.lua        # Socket helper functions
├── string                  # String handling
│   ├── hashkey.lua         # Hash key
│   └── urlencode.lua       # URL encoding
├── debugger.lua            # Debugger
├── interactive.lua         # Interactive debugging
├── mongo.lua               # MongoDB client
├── redis.lua               # Redis client
├── mysql.lua               # MySQL client
└── http                    # HTTP support
    ├── httpd.lua           # HTTP server
    ├── httpc.lua           # HTTP client
    └── websocket.lua       # WebSocket support
```

#### Core Lua Modules

**skynet.lua** - Main module

```lua
local skynet = require "skynet"

-- Core API
skynet.start(func)          -- Start a service
skynet.dispatch(type, func) -- Register a message handler
skynet.send(addr, type, ...) -- Send a message
skynet.call(addr, type, ...) -- Call and wait for the result
skynet.ret(...)             -- Return a result
skynet.newservice(name, ...) -- Create a new service
skynet.self()               -- Get the current service's address
skynet.exit()               -- Exit a service
skynet.timeout(ti, func)    -- Timer
```

**skynet.coroutine.lua** - Coroutine support

```lua
-- Coroutine operations
skynet.fcall(addr, type, ...) -- Asynchronous call
skynet.wait(co)              -- Wait for a coroutine's result
skynet.wakeup(co)            -- Wake up a coroutine
skynet.yield()               -- Yield execution
```

### lualib-src/

C extension modules for Lua:

```
lualib-src/
├── lua-skynet.c            # Skynet Lua binding
├── lua-seri.c              # Serialization
├── lua-socket.c            # Socket binding
├── lua-netpack.c           # Network packet handling
├── lua-mongo.c             # MongoDB binding
├── lua-redis.c             # Redis binding
├── lua-bson.c              # BSON binding
├── lua-md5.c               # MD5 binding
├── lua-sharetable.c        # Shared table
└── lpeg/                   # LPEG pattern matching
    ├── lpeg.c
    └── lpcode.c
```

## Service Directory

### service/

Lua service implementations:

```
service/
├── service.lua             # Base service
├── logger.lua              # Logging service
├── gate.lua                # Gateway service
├── master.lua              # Master control service
├── slave.lua               # Slave service
├── harbor.lua              # Harbor service
├── debug_console.lua       # Debug console
├── cmaster.lua             # Cluster master
├── cslave.lua              # Cluster slave
└── snaxd.lua               # SNAX service daemon
```

### service-src/

C service implementations:

```
service-src/
├── service_logger.c        # Logging service
├── service_gate.c          # Gateway service
├── service_harbor.c        # Harbor service
└── service_master.c        # Master control service
```

## Examples Directory

### examples/

Example configurations and scripts:

```
examples/
├── config                  # Example configuration file
├── config.test             # Test configuration
├── config.log              # Logging configuration
├── client.lua              # Client example
├── main.lua                # Main service example
├── proto.lua               # Protocol definitions
├── watchdog.lua            # Watchdog service
├── echo.lua                # Echo service
├── simpledb.lua            # Simple database
└── login.lua               # Login service
```

#### Example Configuration File

```lua
-- examples/config
include "config.path"

-- Service startup configuration
start = "main"  -- Main service

-- Logging configuration
logpath = "log"
logservice = "logger"

-- Network configuration
thread = 8
harbor = 0
```

## Test Directory

### test/

Test code:

```
test/
├── test.lua                # Main test script
├── teststring.lua          # String tests
├── testcoroutine.lua       # Coroutine tests
├── testsocket.lua          # Socket tests
├── testtimer.lua           # Timer tests
├── testmemory.lua          # Memory tests
└── testmulticast.lua       # Multicast tests
```

## Third-Party Dependencies

### 3rd/

Third-party libraries:

```
3rd/
├── lua/                    # Lua 5.5 (modified version)
│   ├── lua-5.5.0/
│   ├── Makefile
│   └── ...
├── jemalloc/               # jemalloc memory allocator
│   ├── jemalloc-5.3.0/
│   └── Makefile
├── lpeg/                   # LPEG pattern matching
│   ├── lpeg-1.0.2/
│   └── Makefile
└── pb/                     # Protocol Buffers
    ├── protobuf/
    └── Makefile
```

## Build Artifacts

Files generated after building:

```
skynet/
├── skynet                  # Main program
├── lua                     # Lua interpreter
├── luac                    # Lua compiler
├── cservice/               # C service modules
│   ├── logger.so
│   ├── gate.so
│   ├── harbor.so
│   └── ...
├── luaclib/                # Lua C modules
│   ├── bson.so
│   ├── md5.so
│   ├── lpeg.so
│   └── ...
└── log/                    # Log directory
```

## Key File Descriptions

### Configuration Files

**config.path** - Path configuration

```lua
-- Service paths
lua_path = root .. "lualib/?.lua;" .. root .. "lualib/?/init.lua"
lua_cpath = root .. "luaclib/?.so"
lua_service = root .. "service/?.lua;" .. root .. "examples/?.lua"
```

**config** - Main configuration

```lua
-- Startup service
start = "main"

-- Network configuration
thread = 8
harbor = 0

-- Logging configuration
logpath = "log"
logservice = "logger"
```

### Protocol Files

**proto.lua** - Protocol definitions

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

## Development Environment Recommendations

### Recommended Directory Structure

For a real project, the following structure is recommended:

```
my_game/
├── skynet/                 # Skynet framework (as a submodule)
├── service/                # Game services
│   ├── game/              # Game logic
│   ├── login/             # Login service
│   └── gate/              # Gateway service
├── lualib/                 # Game Lua libraries
├── proto/                  # Protocol definitions
├── config/                 # Configuration files
└── Makefile                # Build file
```

## Next Steps

- [Core API](/getting-started/api-reference) - Learn Skynet's core API
- [Architecture Overview](/architecture/overview) - Understand Skynet's overall architecture
- [Source Code Analysis](/analysis/core-modules) - Deep dive into the core source code
