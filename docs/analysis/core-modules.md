# Core Modules Overview

The core of Skynet is implemented in C. While reading the source code, we organized the responsibilities and dependencies of each module to make the overall architecture easier to understand.

## Module Dependencies

```mermaid
graph TB
    Start[skynet_start.c<br/>System initialization entry]

    Start --> Server[skynet_server.c<br/>Service management]
    Start --> Timer[skynet_timer.c<br/>Timer]
    Start --> Harbor[skynet_harbor.c<br/>Cluster]
    Start --> Socket[socket_server.c<br/>Network I/O abstraction layer]

    Server --> MQ[skynet_mq.c<br/>Message queue]
    Server --> Handle[skynet_handle.c<br/>Service handles]
    Server --> Module[skynet_module.c<br/>Dynamic modules]
```

socket_server.c is reached through skynet_socket.c; it is used directly by skynet_start.c (socket thread) and skynet_log.c, not by the queue, handle, module, timer or harbor modules.

## Core Module List

### 1. skynet_start.c - System Startup

**Responsibility**: initialize the entire Skynet system

**Key functions**:
- `skynet_start()`: system entry point
- `create_thread()`: create a thread (wraps pthread_create)
- `thread_worker()`: worker thread main loop

**Startup flow**:
1. Parse the configuration
2. Initialize each subsystem
3. Create worker threads
4. Load the bootstrap service
5. Enter the main loop

### 2. skynet_server.c - Service Management

**Responsibility**: service creation, destruction, and message handling

**Core data structures**:
```c
struct skynet_context {
    void *instance;           // service instance
    struct skynet_module *mod; // module pointer
    void *cb_ud;              // callback user data
    skynet_cb cb;             // message callback function
    struct message_queue *queue; // message queue
    char result[32];          // result buffer
    uint32_t handle;          // service handle
    int session_id;           // session ID
    int ref;                  // reference count
};
```
Service names are stored in the handle table (skynet_handle.c), not in `struct skynet_context`.

**Key functions**:
- `skynet_context_new()`: create a new service
- `skynet_context_grab()`: increment the reference count
- `skynet_context_release()`: decrement the reference count
- `skynet_send()`: send a message
- `skynet_callback()`: set the callback function

### 3. skynet_mq.c - Message Queue

**Responsibility**: manage the message queues of services

**Core data structures**:
```c
struct message_queue {
    struct spinlock lock;     // spinlock
    uint32_t handle;          // handle of the owning service
    int cap;                  // queue capacity
    int head;                 // queue head
    int tail;                 // queue tail
    int in_global;           // whether it is in the global queue
    struct skynet_message *queue; // message array
};
```

**Key functions**:
- `skynet_mq_create()`: create a message queue
- `skynet_mq_push()`: enqueue a message
- `skynet_mq_pop()`: dequeue a message
- `skynet_mq_mark_release()`: mark the queue for release

**Global queue**:
```c
struct global_queue {
    struct message_queue *head; // head of the queue linked list
    struct message_queue *tail; // tail of the queue linked list
    struct spinlock lock;       // spinlock
};
```

### 4. skynet_handle.c - Service Handles

**Responsibility**: manage the mapping of service handles

**Core data structures**:
```c
struct handle_storage {
    struct rwlock lock;        // read-write lock
    uint32_t harbor;          // Harbor ID
    int slot_size;            // slot size
    struct skynet_context **slot; // service array
};
```

**Key functions**:
- `skynet_handle_register()`: register a service
- `skynet_handle_retire()`: retire a service
- `skynet_handle_grab()`: grab a service
- `skynet_handle_findname()`: look up a service by name

### 5. skynet_module.c - Dynamic Modules

**Responsibility**: load and manage C service modules

**Module interface**:
```c
struct skynet_module {
    const char *name;         // module name
    void *module;             // dynamic library handle
    void (*init)(const char *args); // initialization function
    void (*release)(void *inst);     // release function
    void (*signal)(void *inst, int signal); // signal handling
};
```

**Key functions**:
- `skynet_module_instance_create()`: create a module instance
- `skynet_module_instance_init()`: initialize a module instance
- `skynet_module_instance_release()`: release a module instance
- `skynet_module_query()`: query a module

### 6. skynet_timer.c - Timer

**Responsibility**: provide timer functionality

**Core data structures**:
```c
struct timer {
    struct link_list near[TIME_NEAR]; // 256 near slots (timing wheel)
    struct link_list t[4][TIME_LEVEL]; // 4 levels of 64 slots
    struct spinlock lock;         // spinlock
    uint32_t time;               // current time
    uint32_t starttime;          // start time
    uint64_t current;            // current timestamp
    uint64_t current_point;      // current time point
};
```

**Key functions**:
- `skynet_timer_init()`: initialize the timer
- `skynet_timeout()`: set a timeout (centiseconds)
- `timer_update()`: advance and process the timing wheel (static)

### 7. socket_server.c - Network I/O

**Responsibility**: provide a unified network I/O abstraction

**Core data structures**:
```c
struct socket {
    uintptr_t opaque;          // service handle owning this socket
    struct wb_list high;       // high-priority write buffer
    struct wb_list low;        // low-priority write buffer
    int64_t wb_size;           // write buffer size
    struct socket_stat stat;   // per-socket statistics
    int fd;                    // file descriptor
    int id;                    // socket ID
    int type;                  // socket type
    union {
        int size;              // read buffer size
        uint8_t udp_address[UDP_ADDRESS_SIZE];
    } p;                       // socket-specific payload
};
```

**Key functions**:
- `socket_server_create()`: create the socket server
- `socket_server_listen()`: listen for connections
- `socket_server_send()`: send data
- `socket_server_poll()`: poll for events

### 8. skynet_harbor.c - Cluster

**Responsibility**: support cross-node service calls

**Key functions**:
- `skynet_harbor_start()`: start Harbor
- `skynet_harbor_send()`: send a cross-node message
- `skynet_harbor_message_isremote()`: test whether a handle is on a remote node

### 9. skynet_daemon.c - Daemon

**Responsibility**: run Skynet as a daemon

**Key functions**:
- `daemon_init()`: initialize the daemon
- `daemon_exit()`: exit the daemon

### 10. skynet_log.c - Logging

**Responsibility**: log recording

**Key functions**:
- `skynet_error()`: log an error message
- `skynet_log_open()`: open a per-service log file
- `skynet_log_close()`: close a per-service log file

## Inter-Module Communication

### Message Passing Flow

```mermaid
graph TB
    subgraph "Service Layer"
        SA[Service A]
        SB[Service B]
    end

    subgraph "Message Queue Layer"
        MQA[Message Queue A]
        MQB[Message Queue B]
    end

    subgraph "Global Scheduling Layer"
        GQ[Global Message Queue]
        W[Worker Thread]
    end

    SA -->|skynet_send| MQA
    SB -->|skynet_send| MQB
    MQA -->|skynet_mq_push| GQ
    MQB -->|skynet_mq_push| GQ
    GQ -->|skynet_mq_pop| W
    W -->|dispatch| SA
    W -->|dispatch| SB
```

**Summary of experience**:
- Each service has its own message queue, which avoids lock contention
- The global queue handles scheduling and guarantees fairness
- Worker threads take a service queue out of the global queue and process the messages inside it

### Cross-Node Communication

```mermaid
graph LR
    subgraph "Node A"
        HA[Harbor Service]
        SA1[Service 1]
        SA2[Service 2]
    end

    subgraph "Node B"
        HB[Harbor Service]
        SB1[Service 3]
        SB2[Service 4]
    end

    HA <-->|TCP connection| HB
    SA1 --> HA
    SA2 --> HA
    SB1 --> HB
    SB2 --> HB
```

**Summary of experience**:
- The Harbor service is responsible for forwarding messages across nodes
- Nodes communicate with each other over TCP connections
- A local service does not need to know which node the target service lives on

## Key Design Patterns

### 1. Reference Counting
Services use reference counting to manage their lifecycle:
```c
// Increment the reference count
skynet_context_grab(ctx);

// Decrement the reference count
skynet_context_release(ctx);
```

### 2. Spinlock
Message queues are protected with spinlocks:
```c
spinlock_lock(&q->lock);
// Critical section operations
spinlock_unlock(&q->lock);
```

### 3. Read-Write Lock
The service handle table uses a read-write lock:
```c
// Read operation
rwlock_rlock(&s->lock);
// Perform the read
rwlock_runlock(&s->lock);

// Write operation
rwlock_wlock(&s->lock);
// Perform the write
rwlock_wunlock(&s->lock);
```

### 4. Callback Function
Services handle messages through callback functions:
```c
typedef int (*skynet_cb)(struct skynet_context *ctx,
    void *ud,
    int type,
    int session,
    uint32_t source,
    const void *msg,
    size_t sz);
```

## Source references

| Source | Symbol or setting |
| --- | --- |
| [skynet-src/skynet_server.c:42](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_server.c#L42) | `struct skynet_context` |
| [skynet-src/skynet_mq.c:12](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L12) | `DEFAULT_QUEUE_SIZE 64` |
| [skynet-src/skynet_mq.c:19](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_mq.c#L19) | `MQ_OVERLOAD 1024` |
| [skynet-src/skynet_timer.c:17](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L17) | `TIME_NEAR_SHIFT 8` (256 near slots) |
| [skynet-src/skynet_timer.c:19](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L19) | `TIME_LEVEL_SHIFT 6` (64 slots per level) |
| [skynet-src/skynet_timer.c:205](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L205) | `skynet_timeout(` |
| [skynet-src/skynet_timer.c:166](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_timer.c#L166) | `timer_update(` |
| [skynet-src/skynet_start.c:48](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_start.c#L48) | `create_thread(` |
| [skynet-src/skynet_module.c:134](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_module.c#L134) | `skynet_module_instance_create(` |
| [skynet-src/skynet_harbor.c:26](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_harbor.c#L26) | `skynet_harbor_message_isremote(` |
| [skynet-src/skynet_log.c:9](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/skynet_log.c#L9) | `skynet_log_open(` |
| [skynet-src/socket_server.c:90](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/skynet-src/socket_server.c#L90) | `struct socket` |

Struct listings above are simplified; check the pinned source for the full field set. References are pinned to the audited commit.

## Next Steps

- [Detailed analysis of skynet_server.c](/analysis/skynet-server) - a deep dive into service management
- [Detailed analysis of skynet_mq.c](/analysis/skynet-mq) - implementation details of the message queue
- [Detailed analysis of skynet_timer.c](/analysis/skynet-timer) - how the timer works
- [Detailed analysis of socket_server.c](/analysis/socket-server) - implementation of network I/O
