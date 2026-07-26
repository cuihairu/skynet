---
layout: home

hero:
  name: "Skynet 源码解析"
  text: "深入理解游戏服务器框架的核心设计"
  tagline: "从消息队列到 Actor 模型，从网络 I/O 到 Lua 集成，全面解析 Skynet 的架构精髓"
  actions:
    - theme: brand
      text: 开始阅读
      link: /architecture/overview
    - theme: alt
      text: 源码分析
      link: /analysis/core-modules

features:
  - title: Actor 模型先行
    details: 每个服务都是独立的 Actor，通过消息传递通信，避免共享状态的复杂性。
  - title: 消息驱动架构
    details: 所有逻辑都在消息处理中完成，消息队列是系统的核心调度单元。
  - title: 轻量级设计
    details: C 核心 + Lua 脚本的分层架构，核心保持精简，业务逻辑用 Lua 实现。
  - title: 高性能网络
    details: 基于 epoll/kqueue 的异步 I/O，配合 Lua 协程实现同步编程体验。
  - title: 热更新支持
    details: Lua 服务支持热更新，无需停服即可修改业务逻辑。
  - title: 集群能力
    details: 通过 Harbor 机制实现多节点集群，支持跨节点服务调用。
---

<div class="arch-hero">

**核心理念**：Skynet 是一个轻量级的游戏服务器框架，核心设计思想是 **Actor 模型** + **消息驱动**。从实际项目经验看，这种设计避免了共享状态带来的复杂性，让系统更易于理解和维护。

</div>

## 架构全景

<div class="topology-grid">
  <div class="topology-card">
    <h3>消息队列</h3>
    <p>每个服务一个消息队列，全局队列负责调度，实现公平的消息分发。</p>
  </div>
  <div class="topology-card">
    <h3>服务调度</h3>
    <p>多工作线程从全局队列取消息队列，处理其中的消息，实现并行处理。</p>
  </div>
  <div class="topology-card">
    <h3>定时器系统</h3>
    <p>基于时间轮的定时器实现，支持毫秒级定时任务。</p>
  </div>
  <div class="topology-card">
    <h3>网络 I/O</h3>
    <p>统一的 socket 抽象，支持 TCP/UDP，异步操作通过消息通知服务。</p>
  </div>
  <div class="topology-card">
    <h3>Lua 虚拟机</h3>
    <p>每个 Lua 服务一个独立的 Lua VM，保证隔离性，支持热更新。</p>
  </div>
  <div class="topology-card">
    <h3>Harbor 集群</h3>
    <p>跨节点服务调用，支持分布式部署，实现水平扩展。</p>
  </div>
</div>

## 阅读顺序

### 基础篇
1. [架构概览](/architecture/overview) - 了解整体架构设计
2. [Actor 模型](/architecture/actor-model) - 理解核心设计范式
3. [消息驱动设计](/architecture/message-driven) - 掌握消息处理机制
4. [服务与模块](/architecture/service-module) - 认识服务的生命周期

### 核心机制篇
5. [消息队列](/architecture/message-queue) - 深入消息队列实现
6. [服务调度](/architecture/service-scheduling) - 理解工作线程调度
7. [定时器系统](/architecture/timer-system) - 时间轮的实现原理
8. [网络 I/O](/architecture/network-io) - 异步网络编程模型

### 通信篇
9. [Harbor 集群](/architecture/harbor-cluster) - 跨节点通信机制
10. [Socket 抽象](/architecture/socket-abstraction) - 统一的网络接口
11. [Gate 服务](/architecture/gate-service) - 网关服务实现

### Lua 集成篇
12. [Lua 虚拟机管理](/architecture/lua-vm) - VM 的创建与管理
13. [Lua 服务](/architecture/lua-service) - Lua 服务的实现
14. [热更新机制](/architecture/hot-reload) - 如何实现不停服更新

### 运行时篇
15. [线程模型](/architecture/thread-model) - 多线程协作机制
16. [内存管理](/architecture/memory-management) - 内存分配策略
17. [日志系统](/architecture/logging) - 日志记录机制
18. [监控与调试](/architecture/monitoring) - 运行时状态观测

## 核心设计原则

- **消息驱动**：所有逻辑都在消息处理中完成，没有直接的函数调用
- **服务隔离**：每个服务独立运行，互不干扰，通过消息通信
- **同步编程**：Lua 协程让异步操作看起来像同步代码
- **最小核心**：C 实现核心机制，Lua 实现业务逻辑
- **可组合性**：小服务组合成大系统，易于理解和维护

## 适合谁阅读

- 游戏服务器开发者，想深入理解 Skynet 的设计原理
- 对 Actor 模型和消息驱动架构感兴趣的开发者
- 想学习高性能网络编程和 Lua 集成的开发者
- 希望理解游戏服务器框架设计的架构师
