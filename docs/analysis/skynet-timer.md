# skynet_timer.c 源码分析

`skynet_timer.c` 实现了 Skynet 的定时器系统，基于时间轮算法。在阅读源码的过程中，我们发现这个模块的设计对定时任务的精度和效率有重要影响。

## 文件概览

**职责**：定时器管理、超时处理

**核心数据结构**：
- `timer` - 定时器
- `timer_node` - 定时器节点
- `link_list` - 链表

**关键函数**：
- `skynet_timeout()` - 设置超时
- `skynet_updatetime()` - 更新时间
- `skynet_now()` - 获取当前时间

## 核心数据结构

### timer - 定时器

```c
struct timer {
    struct link_list near[TIME_NEAR];      // 近期定时器（0-255）
    struct link_list t[4][TIME_LEVEL];     // 远期定时器（4 级，每级 64）
    struct spinlock lock;                  // 自旋锁
    uint32_t time;                         // 当前时间
    uint32_t starttime;                    // 启动时间
    uint64_t current;                      // 当前时间（厘秒）
    uint64_t current_point;                // 当前时间点
};
```

**设计要点**：

1. **时间轮算法**：使用多级时间轮管理定时器
2. **分级存储**：近期定时器和远期定时器分开存储
3. **自旋锁保护**：保证线程安全

### timer_node - 定时器节点

```c
struct timer_node {
    struct timer_node *next;  // 链表指针
    uint32_t expire;          // 过期时间
};
```

### timer_event - 定时器事件

```c
struct timer_event {
    uint32_t handle;  // 服务句柄
    int session;      // 会话 ID
};
```

### link_list - 链表

```c
struct link_list {
    struct timer_node head;   // 头节点
    struct timer_node *tail;  // 尾指针
};
```

## 时间轮算法

### 时间轮结构

```
┌─────────────────────────────────────────────────────────────┐
│                    时间轮结构                               │
├─────────────────────────────────────────────────────────────┤
│  near[256] - 近期定时器 (0-255 厘秒)                       │
│  ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐        │
│  │  0  │  1  │  2  │ ... │ 254 │ 255 │     │     │        │
│  └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘        │
│                                                              │
│  t[4][64] - 远期定时器                                      │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ Level 0: 256 - 16383 厘秒                          │   │
│  │ ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐ │   │
│  │ │  0  │  1  │  2  │ ... │  62 │  63 │     │     │ │   │
│  │ └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘ │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │ Level 1: 16384 - 1048575 厘秒                      │   │
│  │ ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐ │   │
│  │ │  0  │  1  │  2  │ ... │  62 │  63 │     │     │ │   │
│  │ └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘ │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │ Level 2: 1048576 - 67108863 厘秒                   │   │
│  │ ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐ │   │
│  │ │  0  │  1  │  2  │ ... │  62 │  63 │     │     │ │   │
│  │ └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘ │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │ Level 3: 67108864+ 厘秒                           │   │
│  │ ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐ │   │
│  │ │  0  │  1  │  2  │ ... │  62 │  63 │     │     │ │   │
│  │ └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘ │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### 时间范围

- **near**：0 - 255 厘秒（0 - 2.55 秒）
- **Level 0**：256 - 16383 厘秒（2.56 - 163.83 秒）
- **Level 1**：16384 - 1048575 厘秒（163.84 - 10485.75 秒）
- **Level 2**：1048576 - 67108863 厘秒（10485.76 - 671088.63 秒）
- **Level 3**：67108864+ 厘秒（671088.64+ 秒）

## 定时器生命周期

### 1. 创建定时器 - timer_create_timer()

```c
static struct timer *
timer_create_timer() {
    struct timer *r = (struct timer *)skynet_malloc(sizeof(struct timer));
    memset(r, 0, sizeof(*r));
    
    int i, j;
    
    // 初始化近期定时器
    for (i = 0; i < TIME_NEAR; i++) {
        link_clear(&r->near[i]);
    }
    
    // 初始化远期定时器
    for (i = 0; i < 4; i++) {
        for (j = 0; j < TIME_LEVEL; j++) {
            link_clear(&r->t[i][j]);
        }
    }
    
    SPIN_INIT(r)
    
    r->current = 0;
    
    return r;
}
```

### 2. 初始化定时器系统 - skynet_timer_init()

```c
void 
skynet_timer_init(void) {
    // 创建定时器
    TI = timer_create_timer();
    
    // 获取系统时间
    uint32_t current = 0;
    systime(&TI->starttime, &current);
    
    // 设置当前时间
    TI->current = current;
    TI->current_point = gettime();
}
```

## 定时器操作

### 1. 添加定时器 - timer_add()

```c
static void
timer_add(struct timer *T, void *arg, size_t sz, int time) {
    // 1. 分配节点
    struct timer_node *node = (struct timer_node *)skynet_malloc(sizeof(*node) + sz);
    memcpy(node + 1, arg, sz);
    
    SPIN_LOCK(T);
    
    // 2. 设置过期时间
    node->expire = time + T->time;
    
    // 3. 添加到时间轮
    add_node(T, node);
    
    SPIN_UNLOCK(T);
}
```

### 2. 添加节点 - add_node()

```c
static void
add_node(struct timer *T, struct timer_node *node) {
    uint32_t time = node->expire;
    uint32_t current_time = T->time;
    
    // 1. 检查是否在近期范围
    if ((time | TIME_NEAR_MASK) == (current_time | TIME_NEAR_MASK)) {
        // 添加到近期定时器
        link(&T->near[time & TIME_NEAR_MASK], node);
    } else {
        // 2. 检查在哪个级别
        int i;
        uint32_t mask = TIME_NEAR << TIME_LEVEL_SHIFT;
        
        for (i = 0; i < 3; i++) {
            if ((time | (mask - 1)) == (current_time | (mask - 1))) {
                break;
            }
            mask <<= TIME_LEVEL_SHIFT;
        }
        
        // 3. 添加到对应级别
        link(&T->t[i][((time >> (TIME_NEAR_SHIFT + i * TIME_LEVEL_SHIFT)) & TIME_LEVEL_MASK)], node);
    }
}
```

**添加流程**：

```
┌─────────────────────────────────────────────────────────────┐
│                    定时器添加流程                           │
├─────────────────────────────────────────────────────────────┤
│  timer_add(T, arg, sz, time)                               │
│       │                                                     │
│       ▼                                                     │
│  ┌─────────────────┐                                       │
│  │ 分配节点        │ skynet_malloc                         │
│  └────────┬────────┘                                       │
│           │                                                  │
│           ▼                                                  │
│  ┌─────────────────┐                                       │
│  │ 设置过期时间    │ node->expire = time + T->time         │
│  └────────┬────────┘                                       │
│           │                                                  │
│           ▼                                                  │
│  ┌─────────────────┐                                       │
│  │ 计算时间差      │ diff = expire - current_time          │
│  └────────┬────────┘                                       │
│           │                                                  │
│     ┌─────┴─────┐                                          │
│     │           │                                          │
│     ▼           ▼                                          │
│  diff < 256   diff >= 256                                   │
│     │           │                                          │
│     ▼           ▼                                          │
│  near 数组    t 数组                                        │
└─────────────────────────────────────────────────────────────┘
```

### 3. 移动节点 - move_list()

```c
static void
move_list(struct timer *T, int level, int idx) {
    // 1. 清空链表
    struct timer_node *current = link_clear(&T->t[level][idx]);
    
    // 2. 重新添加所有节点
    while (current) {
        struct timer_node *temp = current->next;
        add_node(T, current);
        current = temp;
    }
}
```

### 4. 时间推进 - timer_shift()

```c
static void
timer_shift(struct timer *T) {
    int mask = TIME_NEAR;
    uint32_t ct = ++T->time;
    
    if (ct == 0) {
        // 时间溢出，移动 Level 3
        move_list(T, 3, 0);
    } else {
        uint32_t time = ct >> TIME_NEAR_SHIFT;
        int i = 0;
        
        // 检查是否需要移动
        while ((ct & (mask - 1)) == 0) {
            int idx = time & TIME_LEVEL_MASK;
            
            if (idx != 0) {
                // 移动到近期定时器
                move_list(T, i, idx);
                break;
            }
            
            mask <<= TIME_LEVEL_SHIFT;
            time >>= TIME_LEVEL_SHIFT;
            ++i;
        }
    }
}
```

**时间推进流程**：

```
┌─────────────────────────────────────────────────────────────┐
│                    时间推进流程                             │
├─────────────────────────────────────────────────────────────┤
│  timer_shift(T)                                            │
│       │                                                     │
│       ▼                                                     │
│  ┌─────────────────┐                                       │
│  │ 时间加 1        │ ++T->time                             │
│  └────────┬────────┘                                       │
│           │                                                  │
│     ┌─────┴─────┐                                          │
│     │           │                                          │
│     ▼           ▼                                          │
│  溢出          未溢出                                       │
│     │           │                                          │
│     ▼           ▼                                          │
│  移动 Level 3  检查是否需要移动                            │
│                    │                                          │
│                    ▼                                          │
│              ┌─────────────────┐                           │
│              │ 低 8 位全 0 ?   │                           │
│              └────────┬────────┘                           │
│                       │                                      │
│                 ┌─────┴─────┐                              │
│                 │           │                              │
│                 ▼           ▼                              │
│              是           否                                │
│                 │           │                              │
│                 ▼           ▼                              │
│           移动到 near    不移动                            │
└─────────────────────────────────────────────────────────────┘
```

### 5. 执行定时器 - timer_execute()

```c
static inline void
timer_execute(struct timer *T) {
    int idx = T->time & TIME_NEAR_MASK;
    
    // 处理所有到期的定时器
    while (T->near[idx].head.next) {
        struct timer_node *current = link_clear(&T->near[idx]);
        SPIN_UNLOCK(T);
        
        // 分发消息（不需要锁）
        dispatch_list(current);
        
        SPIN_LOCK(T);
    }
}
```

### 6. 分发消息 - dispatch_list()

```c
static inline void
dispatch_list(struct timer_node *current) {
    do {
        // 1. 获取事件
        struct timer_event * event = (struct timer_event *)(current + 1);
        
        // 2. 构造消息
        struct skynet_message message;
        message.source = 0;
        message.session = event->session;
        message.data = NULL;
        message.sz = (size_t)PTYPE_RESPONSE << MESSAGE_TYPE_SHIFT;
        
        // 3. 推入消息队列
        skynet_context_push(event->handle, &message);
        
        // 4. 释放节点
        struct timer_node * temp = current;
        current = current->next;
        skynet_free(temp);
    } while (current);
}
```

### 7. 更新定时器 - timer_update()

```c
static void 
timer_update(struct timer *T) {
    SPIN_LOCK(T);
    
    // 1. 执行超时 0 的定时器
    timer_execute(T);
    
    // 2. 时间推进
    timer_shift(T);
    
    // 3. 执行到期的定时器
    timer_execute(T);
    
    SPIN_UNLOCK(T);
}
```

## 公开接口

### 1. 设置超时 - skynet_timeout()

```c
int
skynet_timeout(uint32_t handle, int time, int session) {
    if (time <= 0) {
        // 立即发送消息
        struct skynet_message message;
        message.source = 0;
        message.session = session;
        message.data = NULL;
        message.sz = (size_t)PTYPE_RESPONSE << MESSAGE_TYPE_SHIFT;
        
        if (skynet_context_push(handle, &message)) {
            return -1;
        }
    } else {
        // 添加定时器
        struct timer_event event;
        event.handle = handle;
        event.session = session;
        timer_add(TI, &event, sizeof(event), time);
    }
    
    return session;
}
```

### 2. 更新时间 - skynet_updatetime()

```c
void
skynet_updatetime(void) {
    uint64_t cp = gettime();
    
    if (cp < TI->current_point) {
        // 时间回退错误
        skynet_error(NULL, "time diff error: change from %lld to %lld", cp, TI->current_point);
        TI->current_point = cp;
    } else if (cp != TI->current_point) {
        // 计算时间差
        uint32_t diff = (uint32_t)(cp - TI->current_point);
        TI->current_point = cp;
        TI->current += diff;
        
        // 更新定时器
        int i;
        for (i = 0; i < diff; i++) {
            timer_update(TI);
        }
    }
}
```

### 3. 获取当前时间 - skynet_now()

```c
uint64_t 
skynet_now(void) {
    return TI->current;
}
```

### 4. 获取启动时间 - skynet_starttime()

```c
uint32_t
skynet_starttime(void) {
    return TI->starttime;
}
```

## 时间获取

### 系统时间 - systime()

```c
static void
systime(uint32_t *sec, uint32_t *cs) {
    struct timespec ti;
    clock_gettime(CLOCK_REALTIME, ti);
    *sec = (uint32_t)ti.tv_sec;
    *cs = (uint32_t)(ti.tv_nsec / 10000000);  // 转换为厘秒
}
```

### 单调时间 - gettime()

```c
static uint64_t
gettime() {
    uint64_t t;
    struct timespec ti;
    clock_gettime(CLOCK_MONOTONIC, ti);
    t = (uint64_t)ti.tv_sec * 100;  // 转换为厘秒
    t += ti.tv_nsec / 10000000;
    return t;
}
```

### 线程时间 - skynet_thread_time()

```c
uint64_t
skynet_thread_time(void) {
    struct timespec ti;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, ti);
    
    return (uint64_t)ti.tv_sec * MICROSEC + (uint64_t)ti.tv_nsec / (NANOSEC / MICROSEC);
}
```

## 链表操作

### 清空链表 - link_clear()

```c
static inline struct timer_node *
link_clear(struct link_list *list) {
    struct timer_node * ret = list->head.next;
    list->head.next = 0;
    list->tail = &(list->head);
    
    return ret;
}
```

### 添加节点 - link()

```c
static inline void
link(struct link_list *list, struct timer_node *node) {
    list->tail->next = node;
    list->tail = node;
    node->next = 0;
}
```

## 线程安全

### 自旋锁使用

```c
SPIN_LOCK(T);
// ... 临界区操作 ...
SPIN_UNLOCK(T);
```

### 锁分离

```c
// timer_execute 中，分发消息时释放锁
while (T->near[idx].head.next) {
    struct timer_node *current = link_clear(&T->near[idx]);
    SPIN_UNLOCK(T);  // 释放锁
    
    dispatch_list(current);  // 分发消息
    
    SPIN_LOCK(T);  // 重新获取锁
}
```

## 性能优化

### 1. 批量处理

```c
// 一次处理多个时间单位
for (i = 0; i < diff; i++) {
    timer_update(TI);
}
```

### 2. 锁分离

分发消息时释放锁，减少锁竞争。

### 3. 缓存友好

链表节点连续分配，提高缓存命中率。

## 设计模式

### 1. 时间轮算法

```
时间轮特点：
- O(1) 添加定时器
- O(1) 推进时间
- 分级存储，减少内存使用
```

### 2. 链表队列

```
链表特点：
- 动态大小
- O(1) 头部插入
- O(1) 尾部追加
```

### 3. 生产者-消费者

```
定时器作为生产者：
    skynet_timeout() → 添加定时器
    timer_update() → 分发消息

服务作为消费者：
    接收超时消息
```

## 与其他模块的交互

### 与 skynet_server.c 的交互

```c
// 设置超时
skynet_timeout(handle, time, session);

// 推入消息
skynet_context_push(handle, &message);
```

### 与 skynet_start.c 的交互

```c
// 初始化定时器
skynet_timer_init();

// 定期更新时间
skynet_updatetime();
```

## 关键设计决策

### 1. 为什么使用时间轮？

- **高效**：O(1) 添加和推进
- **低内存**：分级存储，减少内存使用
- **简单**：实现简单，易于理解

### 2. 为什么使用厘秒？

- **精度足够**：游戏定时器通常不需要毫秒级精度
- **减少更新频率**：每 10 毫秒更新一次
- **减少 CPU 消耗**：降低定时器更新的开销

### 3. 为什么使用自旋锁？

- **低延迟**：定时器操作很快
- **避免上下文切换**：减少线程切换开销
- **简单**：实现简单，易于理解

## 常见问题

### 1. 时间回退怎么办？

记录错误并继续运行。

### 2. 定时器溢出怎么办？

时间回绕到 0，移动 Level 3 的节点。

### 3. 如何保证精度？

使用单调时间，避免系统时间调整影响。

## 下一步

- [socket_server.c 分析](/analysis/socket-server) - 网络 I/O 的实现
- [skynet_start.c 分析](/analysis/skynet-start) - 系统启动的实现
- [定时器系统](/architecture/timer-system) - 定时器的设计原理
