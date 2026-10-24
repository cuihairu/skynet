import { withMermaid } from "vitepress-plugin-mermaid";

const enSidebar = {
  "/getting-started/": [
    {
      text: "Getting Started",
      items: [
        { text: "Overview", link: "/getting-started/" },
        { text: "Build from Source", link: "/getting-started/build-from-source" },
        { text: "Project Structure", link: "/getting-started/project-structure" },
        { text: "Core API", link: "/getting-started/api-reference" }
      ]
    }
  ],
  "/architecture/": [
    {
      text: "Architecture",
      items: [
        { text: "Architecture Overview", link: "/architecture/overview" },
        { text: "Actor Model", link: "/architecture/actor-model" },
        { text: "Message-Driven Design", link: "/architecture/message-driven" },
        { text: "Services and Modules", link: "/architecture/service-module" }
      ]
    },
    {
      text: "Core Mechanics",
      collapsed: false,
      items: [
        { text: "Message Queue", link: "/architecture/message-queue" },
        { text: "Service Scheduling", link: "/architecture/service-scheduling" },
        { text: "Timer System", link: "/architecture/timer-system" },
        { text: "Network I/O", link: "/architecture/network-io" }
      ]
    },
    {
      text: "Communication and Protocols",
      collapsed: false,
      items: [
        { text: "Harbor Cluster", link: "/architecture/harbor-cluster" },
        { text: "Socket Abstraction", link: "/architecture/socket-abstraction" },
        { text: "Gate Service", link: "/architecture/gate-service" }
      ]
    },
    {
      text: "Lua Integration",
      collapsed: false,
      items: [
        { text: "Lua VM Management", link: "/architecture/lua-vm" },
        { text: "Lua Services", link: "/architecture/lua-service" },
        { text: "Hot Reload", link: "/architecture/hot-reload" }
      ]
    },
    {
      text: "Runtime Engineering",
      collapsed: false,
      items: [
        { text: "Thread Model", link: "/architecture/thread-model" },
        { text: "Memory Management", link: "/architecture/memory-management" },
        { text: "Logging", link: "/architecture/logging" },
        { text: "Monitoring and Debugging", link: "/architecture/monitoring" }
      ]
    },
    {
      text: "Reference",
      collapsed: true,
      items: [
        { text: "Design Philosophy", link: "/architecture/design-philosophy" },
        { text: "Comparison with Other Frameworks", link: "/architecture/comparison" }
      ]
    }
  ],
  "/analysis/": [
    {
      text: "Source Analysis",
      items: [
        { text: "Core Modules", link: "/analysis/core-modules" },
        { text: "skynet_server.c", link: "/analysis/skynet-server" },
        { text: "skynet_start.c", link: "/analysis/skynet-start" },
        { text: "skynet_mq.c", link: "/analysis/skynet-mq" },
        { text: "skynet_handle.c", link: "/analysis/skynet-handle" },
        { text: "skynet_timer.c", link: "/analysis/skynet-timer" },
        { text: "socket_server.c", link: "/analysis/socket-server" }
      ]
    },
    {
      text: "Lua Layer",
      collapsed: false,
      items: [
        { text: "skynet.lua", link: "/analysis/skynet-lua" },
        { text: "skynet.manager", link: "/analysis/skynet-manager" },
        { text: "socket Module", link: "/analysis/socket-lua" },
        { text: "driver Module", link: "/analysis/driver-lua" }
      ]
    },
    {
      text: "Service Implementations",
      collapsed: false,
      items: [
        { text: "Gate Service", link: "/analysis/gate-service" },
        { text: "Harbor Service", link: "/analysis/harbor-service" },
        { text: "Logger Service", link: "/analysis/logger-service" }
      ]
    }
  ]
};

const zhSidebar = {
  "/zh/getting-started/": [
    {
      text: "快速开始",
      items: [
        { text: "概述", link: "/zh/getting-started/" },
        { text: "从源码构建", link: "/zh/getting-started/build-from-source" },
        { text: "项目结构", link: "/zh/getting-started/project-structure" },
        { text: "核心 API", link: "/zh/getting-started/api-reference" }
      ]
    }
  ],
  "/zh/architecture/": [
    {
      text: "整体架构",
      items: [
        { text: "架构概览", link: "/zh/architecture/overview" },
        { text: "Actor 模型", link: "/zh/architecture/actor-model" },
        { text: "消息驱动设计", link: "/zh/architecture/message-driven" },
        { text: "服务与模块", link: "/zh/architecture/service-module" }
      ]
    },
    {
      text: "核心机制",
      collapsed: false,
      items: [
        { text: "消息队列", link: "/zh/architecture/message-queue" },
        { text: "服务调度", link: "/zh/architecture/service-scheduling" },
        { text: "定时器系统", link: "/zh/architecture/timer-system" },
        { text: "网络 I/O", link: "/zh/architecture/network-io" }
      ]
    },
    {
      text: "通信与协议",
      collapsed: false,
      items: [
        { text: "Harbor 集群", link: "/zh/architecture/harbor-cluster" },
        { text: "Socket 抽象", link: "/zh/architecture/socket-abstraction" },
        { text: "Gate 服务", link: "/zh/architecture/gate-service" }
      ]
    },
    {
      text: "Lua 集成",
      collapsed: false,
      items: [
        { text: "Lua 虚拟机管理", link: "/zh/architecture/lua-vm" },
        { text: "Lua 服务", link: "/zh/architecture/lua-service" },
        { text: "热更新机制", link: "/zh/architecture/hot-reload" }
      ]
    },
    {
      text: "运行时工程",
      collapsed: false,
      items: [
        { text: "线程模型", link: "/zh/architecture/thread-model" },
        { text: "内存管理", link: "/zh/architecture/memory-management" },
        { text: "日志系统", link: "/zh/architecture/logging" },
        { text: "监控与调试", link: "/zh/architecture/monitoring" }
      ]
    },
    {
      text: "参考资料",
      collapsed: true,
      items: [
        { text: "设计哲学", link: "/zh/architecture/design-philosophy" },
        { text: "与其他框架对比", link: "/zh/architecture/comparison" }
      ]
    }
  ],
  "/zh/analysis/": [
    {
      text: "源码分析",
      items: [
        { text: "核心模块概览", link: "/zh/analysis/core-modules" },
        { text: "skynet_server.c", link: "/zh/analysis/skynet-server" },
        { text: "skynet_start.c", link: "/zh/analysis/skynet-start" },
        { text: "skynet_mq.c", link: "/zh/analysis/skynet-mq" },
        { text: "skynet_handle.c", link: "/zh/analysis/skynet-handle" },
        { text: "skynet_timer.c", link: "/zh/analysis/skynet-timer" },
        { text: "socket_server.c", link: "/zh/analysis/socket-server" }
      ]
    },
    {
      text: "Lua 层分析",
      collapsed: false,
      items: [
        { text: "skynet.lua", link: "/zh/analysis/skynet-lua" },
        { text: "skynet.manager", link: "/zh/analysis/skynet-manager" },
        { text: "socket 模块", link: "/zh/analysis/socket-lua" },
        { text: "driver 模块", link: "/zh/analysis/driver-lua" }
      ]
    },
    {
      text: "服务实现",
      collapsed: false,
      items: [
        { text: "Gate 服务", link: "/zh/analysis/gate-service" },
        { text: "Harbor 服务", link: "/zh/analysis/harbor-service" },
        { text: "Logger 服务", link: "/zh/analysis/logger-service" }
      ]
    }
  ]
};

export default withMermaid({
  title: "Skynet",
  description: "Source code analysis of the Skynet game server framework",
  base: "/skynet/",
  cleanUrls: false,
  lastUpdated: true,
  ignoreDeadLinks: [
    /localhost/
  ],
  locales: {
    root: { label: "English", lang: "en-US" },
    zh:   { label: "简体中文", lang: "zh-CN", link: "/zh/" }
  },
  mermaid: {
    theme: "default",
  },
  mermaidPlugin: {
    class: "mermaid",
  },
  themeConfig: {
    logo: "/logo.svg",
    search: {
      provider: "local"
    },
    socialLinks: [
      { icon: "github", link: "https://github.com/cuihairu/skynet" }
    ],
    locales: {
      root: {
        siteTitle: "Skynet Source Analysis",
        nav: [
          { text: "Overview", link: "/" },
          { text: "Getting Started", link: "/getting-started/" },
          { text: "Architecture", link: "/architecture/overview" },
          { text: "Source Analysis", link: "/analysis/core-modules" }
        ],
        sidebar: enSidebar,
        footer: {
          message: "Skynet architecture analysis based on the source code",
          copyright: "Skynet Source Code Analysis"
        }
      },
      zh: {
        siteTitle: "Skynet 源码解析",
        nav: [
          { text: "总览", link: "/zh/" },
          { text: "快速开始", link: "/zh/getting-started/" },
          { text: "架构解析", link: "/zh/architecture/overview" },
          { text: "源码分析", link: "/zh/analysis/core-modules" }
        ],
        sidebar: zhSidebar,
        footer: {
          message: "基于源码的 Skynet 架构解析与设计原理分析",
          copyright: "Skynet Source Code Analysis"
        }
      }
    }
  }
})
