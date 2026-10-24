---
layout: home

hero:
  name: "Skynet Source Code Analysis"
  text: "Deep understanding of the core design of the game server framework"
  tagline: "From message queues to the Actor model, from network I/O to Lua integration — a comprehensive analysis of Skynet's architectural essence"
  actions:
    - theme: brand
      text: Start Reading
      link: /architecture/overview
    - theme: alt
      text: Source Analysis
      link: /analysis/core-modules

features:
  - title: Actor-Model First
    details: Each service is an independent Actor, communicating through message passing, avoiding the complexity of shared state.
  - title: Message-Driven Architecture
    details: All logic is completed within message handling; the message queue is the core scheduling unit of the system.
  - title: Lightweight Design
    details: A layered architecture of C core + Lua scripting; the core stays lean while business logic is implemented in Lua.
  - title: High-Performance Networking
    details: Asynchronous I/O based on epoll/kqueue, combined with Lua coroutines for a synchronous programming experience.
  - title: Hot Reload Support
    details: Lua services support hot reloading; business logic can be modified without stopping the server.
  - title: Clustering
    details: Multi-node clusters via the Harbor mechanism, supporting cross-node service calls.
---

<div class="arch-hero">

**Core Philosophy**: Skynet is a lightweight game server framework whose core design philosophy is the **Actor model** + **message-driven** architecture. Based on real-world project experience, this design avoids the complexity of shared state, making the system easier to understand and maintain.

</div>

## Architecture Overview

<div class="topology-grid">
  <div class="topology-card">
    <h3>Message Queue</h3>
    <p>Each service has its own message queue; a global queue handles scheduling, achieving fair message distribution.</p>
  </div>
  <div class="topology-card">
    <h3>Service Scheduling</h3>
    <p>Multiple worker threads pull message queues from the global queue and process the messages within them, enabling parallel processing.</p>
  </div>
  <div class="topology-card">
    <h3>Timer System</h3>
    <p>A timer implementation based on a timing wheel, supporting millisecond-level scheduled tasks.</p>
  </div>
  <div class="topology-card">
    <h3>Network I/O</h3>
    <p>A unified socket abstraction supporting TCP/UDP; asynchronous operations notify services through messages.</p>
  </div>
  <div class="topology-card">
    <h3>Lua Virtual Machine</h3>
    <p>Each Lua service has its own independent Lua VM, ensuring isolation and supporting hot reloading.</p>
  </div>
  <div class="topology-card">
    <h3>Harbor Cluster</h3>
    <p>Cross-node service calls supporting distributed deployment and horizontal scaling.</p>
  </div>
</div>

## Reading Order

### Basics
1. [Architecture Overview](/architecture/overview) - Understand the overall architecture design
2. [Actor Model](/architecture/actor-model) - Understand the core design paradigm
3. [Message-Driven Design](/architecture/message-driven) - Master the message handling mechanism
4. [Services and Modules](/architecture/service-module) - Learn about the service lifecycle

### Core Mechanisms
5. [Message Queue](/architecture/message-queue) - Deep dive into the message queue implementation
6. [Service Scheduling](/architecture/service-scheduling) - Understand worker thread scheduling
7. [Timer System](/architecture/timer-system) - The principle behind the timing wheel implementation
8. [Network I/O](/architecture/network-io) - The asynchronous network programming model

### Communication
9. [Harbor Cluster](/architecture/harbor-cluster) - Cross-node communication mechanism
10. [Socket Abstraction](/architecture/socket-abstraction) - A unified network interface
11. [Gate Service](/architecture/gate-service) - Gateway service implementation

### Lua Integration
12. [Lua VM Management](/architecture/lua-vm) - VM creation and management
13. [Lua Services](/architecture/lua-service) - Lua service implementation
14. [Hot Reload Mechanism](/architecture/hot-reload) - How to update without downtime

### Runtime
15. [Thread Model](/architecture/thread-model) - Multi-threaded collaboration mechanism
16. [Memory Management](/architecture/memory-management) - Memory allocation strategies
17. [Logging System](/architecture/logging) - Log recording mechanism
18. [Monitoring and Debugging](/architecture/monitoring) - Observing runtime state

## Core Design Principles

- **Message-Driven**: All logic is completed within message handling, with no direct function calls
- **Service Isolation**: Each service runs independently without interfering with others, communicating through messages
- **Synchronous Programming**: Lua coroutines make asynchronous operations look like synchronous code
- **Minimal Core**: The core mechanisms are implemented in C, with business logic in Lua
- **Composability**: Small services combine into large systems, making them easy to understand and maintain

## Who Should Read This

- Game server developers who want to deeply understand Skynet's design principles
- Developers interested in the Actor model and message-driven architecture
- Developers who want to learn high-performance network programming and Lua integration
- Architects hoping to understand game server framework design
