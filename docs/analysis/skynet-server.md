# skynet_server.c 源码分析

`skynet_server.c` 是 Skynet 框架的核心模块，负责服务的创建、销毁、消息处理和调度。在阅读源码的过程中，我们发现这个模块的设计对理解 Skynet 的运行机制至关重要。

## 文件概览

**职责**：服务管理、消息处理、服务调度

**核心数据结构**：
- `skynet_context` - 服务上下文
- `skynet_node` - 节点信息

**关键函数**：
- `skynet_context_new()` - 创建服务
- `skynet_context_release()` - 释放服务
- `skynet_send()` - 发送消息
- `skynet_callback()` - 设置回调函数

## 核心数据结构

### skynet_context - 服务上下文

```c
struct skynet_context {
    void * instance;              // 服务实例（C 模块或 Lua VM）
    struct skynet_module * mod;   // 所属模块
    void * cb_ud;                 // 回调用户数据
    skynet_cb cb;                 // 消息回调函数
    struct message_queue *queue;  // 消息队列
    ATOM_POINTER logfile;         // 日志文件
    uint64_t cpu_cost;           // CPU 消耗（微秒）
    uint64_t cpu_start;          // 开始时间
    char result[32];             // 结果缓冲区
    uint32_t handle;             // 服务句柄（唯一标识）
    int session_id;              // 会话 ID 计数器
    ATOM_INT ref;                // 引用计数
    size_t message_count;        // 消息计数
    bool init;                   // 是否已初始化
    bool endless;                // 是否死循环
    bool profile;                // 是否开启性能分析
};
```

**设计要点**：

1. **引用计数**：使用原子操作的引用计数管理生命周期
2. **独立消息队列**：每个服务有自己的消息队列
3. **性能监控**：内置 CPU 消耗统计
4. **回调函数**：通过回调函数处理消息

### skynet_node - 节点信息

```c
struct skynet_node {
    ATOM_INT total;          // 服务总数
    int init;                // 初始化标志
    uint32_t monitor_exit;   // 监控退出的服务
    pthread_key_t handle_key; // 线程本地存储
    bool profile;            // 是否开启性能分析
};
```

## 服务生命周期

### 1. 服务创建 - skynet_context_new()

```c
uint32_t
skynet_context_new(const char * name, const char *param) {
    // 1. 查询模块
    struct skynet_module * mod = skynet_module_query(name);
    if (mod == NULL)
        return 0;
    
    // 2. 创建模块实例
    void *inst = skynet_module_instance_create(mod);
    if (inst == NULL)
        return 0;
    
    // 3. 分配上下文
    struct skynet_context * ctx = skynet_malloc(sizeof(*ctx));
    
    // 4. 初始化上下文
    ctx->mod = mod;
    ctx->instance = inst;
    ATOM_INIT(&ctx->ref, 2);  // 两个引用：register + init
    ctx->cb = NULL;
    ctx->cb_ud = NULL;
    ctx->session_id = 0;
    ctx->init = false;
    ctx->endless = false;
    ctx->cpu_cost = 0;
    ctx->cpu_start = 0;
    ctx->message_count = 0;
    ctx->profile = G_NODE.profile;
    
    // 5. 注册句柄
    ctx->handle = 0;
    const uint32_t handle = skynet_handle_register(ctx);
    ctx->handle = handle;
    
    // 6. 创建消息队列
    struct message_queue * queue = ctx->queue = skynet_mq_create(handle);
    
    // 7. 初始化模块
    context_inc();
    int r = skynet_module_instance_init(mod, inst, ctx, param);
    
    if (r == 0) {
        // 8. 初始化成功
        ctx->init = true;
        skynet_globalmq_push(queue);  // 加入全局队列
        skynet_error(ctx, "LAUNCH %s %s", name, param ? param : "");
        skynet_context_release(ctx);  // 释放初始化引用
        return handle;
    } else {
        // 9. 初始化失败
        skynet_error(ctx, "error: launch %s FAILED", name);
        uint32_t handle = ctx->handle;
        skynet_context_release(ctx);
        skynet_handle_retire(handle);
        struct drop_t d = { handle };
        skynet_mq_release(queue, drop_message, &d);
        return 0;
    }
}
```

**流程图**：

```
┌─────────────────────────────────────────────────────────────┐
│                    服务创建流程                             │
├─────────────────────────────────────────────────────────────┤
│  skynet_context_new(name, param)                           │
│       │                                                     │
│       ▼                                                     │
│  ┌─────────────────┐                                       │
│  │ 查询模块        │ skynet_module_query(name)             │
│  └────────┬────────┘                                       │
│           │                                                  │
│           ▼                                                  │
│  ┌─────────────────┐                                       │
│  │ 创建模块实例    │ skynet_module_instance_create(mod)     │
│  └────────┬────────┘                                       │
│           │                                                  │
│           ▼                                                  │
│  ┌─────────────────┐                                       │
│  │ 分配上下文      │ skynet_malloc(sizeof(*ctx))           │
│  └────────┬────────┘                                       │
│           │                                                  │
│           ▼                                                  │
│  ┌─────────────────┐                                       │
│  │ 注册句柄        │ skynet_handle_register(ctx)           │
│  └────────┬────────┘                                       │
│           │                                                  │
│           ▼                                                  │
│  ┌─────────────────┐                                       │
│  │ 创建消息队列    │ skynet_mq_create(handle)              │
│  └────────┬────────┘                                       │
│           │                                                  │
│           ▼                                                  │
│  ┌─────────────────┐                                       │
│  │ 初始化模块      │ skynet_module_instance_init(...)       │
│  └────────┬────────┘                                       │
│           │                                                  │
│     ┌─────┴─────┐                                          │
│     │           │                                          │
│     ▼           ▼                                          │
│  成功          失败                                         │
│     │           │                                          │
│     ▼           ▼                                          │
│  加入全局队列  清理资源                                     │
│  返回句柄      返回 0                                       │
└─────────────────────────────────────────────────────────────┘
```

### 2. 服务销毁 - skynet_context_release()

```c
static void
delete_context(struct skynet_context *ctx) {
    // 关闭日志文件
    FILE *f = (FILE *)ATOM_LOAD(&ctx->logfile);
    if (f) {
        fclose(f);
    }
    
    // 释放模块实例
    skynet_module_instance_release(ctx->mod, ctx->instance);
    
    // 标记消息队列为释放状态
    skynet_mq_mark_release(ctx->queue);
    
    // 释放上下文
    skynet_free(ctx);
    
    // 减少节点服务计数
    context_dec();
}

void
skynet_context_release(struct skynet_context *ctx) {
    // 原子减少引用计数
    if (ATOM_FDEC(&ctx->ref) == 1) {
        // 引用计数为 0，删除上下文
        delete_context(ctx);
    }
}
```

**引用计数机制**：

```
引用计数变化：
    初始值: 2 (register + init)
    
    创建后: 2
        ↓ skynet_handle_register
        2
        ↓ skynet_context_release (init 完成)
        1
        ↓ 其他服务 grab
        2
        ↓ 其他服务 release
        1
        ↓ 自身 release
        0 → 删除
```

### 3. 引用管理 - skynet_context_grab()

```c
void
skynet_context_grab(struct skynet_context *ctx) {
    ATOM_FINC(&ctx->ref);
}

void
skynet_context_reserve(struct skynet_context *ctx) {
    skynet_context_grab(ctx);
    // 不计入总数，因为 reserved context 会在最后释放
    context_dec();
}
```

## 消息处理

### 1. 发送消息 - skynet_send()

```c
int
skynet_send(struct skynet_context * context, uint32_t source, uint32_t destination, int type, int session, void * data, size_t sz) {
    // 检查消息类型标志
    if ((sz & MESSAGE_TYPE_MASK) != sz) {
        skynet_error(context, "The message to %x is too large", destination);
        if (type & PTYPE_TAG_DONTCOPY) {
            skynet_free(data);
        }
        return -2;
    }
    
    // 检查是否是本地消息
    if (source == 0) {
        source = context->handle;
    }
    
    // 检查是否需要释放
    if (skynet_harbor_message_isremote(destination)) {
        // 远程消息，通过 Harbor 发送
        struct skynet_message smsg;
        smsg.source = source;
        smsg.session = session;
        smsg.data = data;
        smsg.sz = sz;
        skynet_harbor_send(&smsg, destination);
    } else {
        // 本地消息，直接推入队列
        struct skynet_message smsg;
        smsg.source = source;
        smsg.session = session;
        smsg.data = data;
        smsg.sz = sz;
        skynet_context_push(destination, &smsg);
    }
    
    return session;
}
```

**消息发送流程**：

```
┌─────────────────────────────────────────────────────────────┐
│                    消息发送流程                             │
├─────────────────────────────────────────────────────────────┤
│  skynet_send(ctx, source, dest, type, session, data, sz)   │
│       │                                                     │
│       ▼                                                     │
│  ┌─────────────────┐                                       │
│  │ 检查消息大小    │ (sz & MESSAGE_TYPE_MASK) != sz        │
│  └────────┬────────┘                                       │
│           │                                                  │
│           ▼                                                  │
│  ┌─────────────────┐                                       │
│  │ 设置源地址      │ source = context->handle              │
│  └────────┬────────┘                                       │
│           │                                                  │
│     ┌─────┴─────┐                                          │
│     │           │                                          │
│     ▼           ▼                                          │
│  远程消息     本地消息                                      │
│     │           │                                          │
│     ▼           ▼                                          │
│  Harbor 发送  直接入队                                      │
└─────────────────────────────────────────────────────────────┘
```

### 2. 消息推入队列 - skynet_context_push()

```c
int
skynet_context_push(uint32_t handle, struct skynet_message *message) {
    // 获取服务上下文
    struct skynet_context * ctx = skynet_handle_grab(handle);
    if (ctx == NULL) {
        return -1;
    }
    
    // 推入消息队列
    skynet_mq_push(ctx->queue, message);
    
    // 释放上下文引用
    skynet_context_release(ctx);
    
    return 0;
}
```

### 3. 消息分发 - skynet_context_message_dispatch()

```c
struct skynet_message * 
skynet_context_message_dispatch(struct skynet_monitor *sm, struct skynet_message *msg, int weight) {
    // 检查是否是退出消息
    if (msg->source == 0) {
        // 系统消息
        if (msg->session == 0) {
            // 退出消息
            skynet_handle_retire(msg->destination);
            return NULL;
        }
    }
    
    // 获取目标服务
    struct skynet_context * ctx = skynet_handle_grab(msg->destination);
    if (ctx == NULL) {
        // 目标服务不存在，丢弃消息
        skynet_free(msg->data);
        return NULL;
    }
    
    // 检查是否是死循环服务
    if (ctx->endless) {
        // 死循环服务，不处理消息
        skynet_free(msg->data);
        skynet_context_release(ctx);
        return NULL;
    }
    
    // 开始处理消息
    int type = msg->sz >> MESSAGE_TYPE_SHIFT;
    size_t sz = msg->sz & MESSAGE_TYPE_MASK;
    
    // 性能监控
    if (ctx->profile) {
        ctx->cpu_start = skynet_thread_time();
    }
    
    // 调用回调函数
    CHECKCALLING_BEGIN(ctx)
    int err = ctx->cb(ctx, ctx->cb_ud, type, msg->session, msg->source, msg->data, sz);
    CHECKCALLING_END(ctx)
    
    // 更新性能统计
    if (ctx->profile) {
        ctx->cpu_cost += skynet_thread_time() - ctx->cpu_start;
    }
    
    // 更新消息计数
    ++ctx->message_count;
    
    // 释放上下文引用
    skynet_context_release(ctx);
    
    return NULL;
}
```

## 回调函数管理

### 设置回调函数 - skynet_callback()

```c
void
skynet_callback(struct skynet_context * context, void *ud, skynet_cb cb) {
    // 设置用户数据
    context->cb_ud = ud;
    
    // 设置回调函数
    context->cb = cb;
}
```

**回调函数类型**：

```c
typedef int (*skynet_cb)(
    struct skynet_context * ctx,   // 服务上下文
    void * ud,                      // 用户数据
    int type,                       // 消息类型
    int session,                    // 会话 ID
    uint32_t source,               // 消息来源
    const void * msg,              // 消息内容
    size_t sz                      // 消息大小
);
```

## 服务查询

### 按句柄查询 - skynet_handle_grab()

```c
// 在 skynet_handle.c 中实现
struct skynet_context * 
skynet_handle_grab(uint32_t handle) {
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

### 按名称查询 - skynet_handle_findname()

```c
uint32_t
skynet_handle_findname(const char * name) {
    struct handle_storage *s = H;
    rwlock_rlock(&s->lock);
    
    uint32_t handle = 0;
    int i;
    for (i=0; i<s->name_count; i++) {
        if (strcmp(s->name[i].name, name) == 0) {
            handle = s->name[i].handle;
            break;
        }
    }
    
    rwlock_runlock(&s->lock);
    return handle;
}
```

## 死循环检测

### 标记死循环 - skynet_context_endless()

```c
void
skynet_context_endless(uint32_t handle) {
    struct skynet_context * ctx = skynet_handle_grab(handle);
    if (ctx == NULL) {
        return;
    }
    ctx->endless = true;
    skynet_context_release(ctx);
}
```

### 检查死循环

```c
// 在消息分发中检查
if (ctx->endless) {
    // 死循环服务，不处理消息
    skynet_free(msg->data);
    skynet_context_release(ctx);
    return NULL;
}
```

## 性能监控

### CPU 消耗统计

```c
// 在消息处理前后统计
if (ctx->profile) {
    ctx->cpu_start = skynet_thread_time();
}

// 调用回调函数
int err = ctx->cb(ctx, ctx->cb_ud, type, msg->session, msg->source, msg->data, sz);

if (ctx->profile) {
    ctx->cpu_cost += skynet_thread_time() - ctx->cpu_start;
}

++ctx->message_count;
```

### 获取性能数据

```c
// 在 skynet_monitor.c 中实现
void
skynet_monitor_check(struct skynet_monitor *sm) {
    // 检查各个服务的 CPU 消耗
    // ...
}
```

## 错误处理

### 消息丢弃处理

```c
struct drop_t {
    uint32_t handle;
};

static void
drop_message(struct skynet_message *msg, void *ud) {
    struct drop_t *d = ud;
    
    // 释放消息数据
    skynet_free(msg->data);
    
    // 向消息源发送错误通知
    uint32_t source = d->handle;
    assert(source);
    skynet_send(NULL, source, msg->source, PTYPE_ERROR, msg->session, NULL, 0);
}
```

### 创建失败处理

```c
if (r == 0) {
    // 初始化成功
    ctx->init = true;
    skynet_globalmq_push(queue);
    skynet_error(ctx, "LAUNCH %s %s", name, param ? param : "");
    skynet_context_release(ctx);
    return handle;
} else {
    // 初始化失败
    skynet_error(ctx, "error: launch %s FAILED", name);
    uint32_t handle = ctx->handle;
    skynet_context_release(ctx);
    skynet_handle_retire(handle);
    struct drop_t d = { handle };
    skynet_mq_release(queue, drop_message, &d);
    return 0;
}
```

## 线程安全

### 原子操作

```c
// 引用计数使用原子操作
ATOM_INT ref;

// 增加引用
void skynet_context_grab(struct skynet_context *ctx) {
    ATOM_FINC(&ctx->ref);
}

// 减少引用
void skynet_context_release(struct skynet_context *ctx) {
    if (ATOM_FDEC(&ctx->ref) == 1) {
        delete_context(ctx);
    }
}
```

### 读写锁

```c
// 句柄表使用读写锁保护
rwlock_rlock(&s->lock);   // 读操作
rwlock_runlock(&s->lock);
rwlock_wlock(&s->lock);   // 写操作
rwlock_wunlock(&s->lock);
```

## 设计模式

### 1. 引用计数模式

```c
// 生命周期管理
struct skynet_context *ctx = skynet_handle_grab(handle);
if (ctx) {
    // 使用 ctx
    skynet_context_release(ctx);
}
```

### 2. 回调函数模式

```c
// 注册回调
skynet_callback(context, ud, callback_function);

// 调用回调
ctx->cb(ctx, ctx->cb_ud, type, session, source, data, sz);
```

### 3. 生产者-消费者模式

```c
// 生产者：发送消息
skynet_send(context, source, dest, type, session, data, sz);

// 消费者：处理消息
skynet_context_message_dispatch(sm, msg, weight);
```

## 性能优化

### 1. 批量处理

```c
// 一次处理多个消息
for (i = 0; i < n; i++) {
    struct skynet_message *msg = skynet_mq_pop(q);
    if (msg) {
        skynet_context_message_dispatch(sm, msg, weight);
    }
}
```

### 2. 权重调度

```c
// 根据权重决定处理消息数量
int weight = skynet_mq_length(q) / global_mq_count;
if (weight == 0) weight = 1;
```

## 与其他模块的交互

### 与 skynet_mq.c 的交互

```c
// 创建消息队列
ctx->queue = skynet_mq_create(handle);

// 推入消息
skynet_mq_push(ctx->queue, message);

// 弹出消息
struct skynet_message *msg = skynet_mq_pop(ctx->queue);
```

### 与 skynet_handle.c 的交互

```c
// 注册句柄
ctx->handle = skynet_handle_register(ctx);

// 查找服务
struct skynet_context *ctx = skynet_handle_grab(handle);

// 注销句柄
skynet_handle_retire(handle);
```

### 与 skynet_module.c 的交互

```c
// 查询模块
struct skynet_module *mod = skynet_module_query(name);

// 创建实例
void *inst = skynet_module_instance_create(mod);

// 初始化实例
int r = skynet_module_instance_init(mod, inst, ctx, param);

// 释放实例
skynet_module_instance_release(ctx->mod, ctx->instance);
```

## 关键设计决策

### 1. 为什么使用引用计数？

- **线程安全**：原子操作保证线程安全
- **自动释放**：引用计数为 0 时自动释放资源
- **避免循环引用**：通过设计避免循环引用

### 2. 为什么使用回调函数？

- **解耦**：服务逻辑与框架解耦
- **灵活性**：可以动态设置回调函数
- **多态性**：不同类型的服务可以有不同的回调

### 3. 为什么每个服务有独立的消息队列？

- **隔离性**：服务之间互不干扰
- **公平调度**：每个服务公平地获得处理机会
- **简化设计**：避免复杂的锁竞争

## 下一步

- [skynet_mq.c 分析](/analysis/skynet-mq) - 消息队列的实现细节
- [skynet_handle.c 分析](/analysis/skynet-handle) - 句柄管理的实现
- [服务调度](/architecture/service-scheduling) - 理解工作线程如何调度服务
