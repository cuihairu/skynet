export default {
  lang: "zh-CN",
  title: "Skynet 源码解析",
  description: "深入解析 Skynet 游戏服务器框架的架构设计与核心实现",
  base: "/skynet/",
  cleanUrls: false,
  lastUpdated: true,
  ignoreDeadLinks: [
    /localhost/,
    /\/analysis\/skynet-socket-lua/
  ],
  markdown: {
    container: {
      tipLabel: "提示",
      warningLabel: "注意",
      dangerLabel: "风险",
      infoLabel: "信息",
      detailsLabel: "详细信息"
    }
  },
  themeConfig: {
    logo: "/logo.svg",
    siteTitle: "Skynet 源码解析",
    search: {
      provider: "local"
    },
    nav: [
      { text: "总览", link: "/" },
      { text: "快速开始", link: "/getting-started/" },
      { text: "架构解析", link: "/architecture/overview" },
      { text: "源码分析", link: "/analysis/core-modules" }
    ],
    sidebar: {
      "/getting-started/": [
        {
          text: "快速开始",
          items: [
            { text: "概述", link: "/getting-started/" },
            { text: "从源码构建", link: "/getting-started/build-from-source" },
            { text: "项目结构", link: "/getting-started/project-structure" },
            { text: "核心 API", link: "/getting-started/api-reference" }
          ]
        }
      ],
      "/architecture/": [
        {
          text: "整体架构",
          items: [
            { text: "架构概览", link: "/architecture/overview" },
            { text: "Actor 模型", link: "/architecture/actor-model" },
            { text: "消息驱动设计", link: "/architecture/message-driven" },
            { text: "服务与模块", link: "/architecture/service-module" }
          ]
        },
        {
          text: "核心机制",
          collapsed: false,
          items: [
            { text: "消息队列", link: "/architecture/message-queue" },
            { text: "服务调度", link: "/architecture/service-scheduling" },
            { text: "定时器系统", link: "/architecture/timer-system" },
            { text: "网络 I/O", link: "/architecture/network-io" }
          ]
        },
        {
          text: "通信与协议",
          collapsed: false,
          items: [
            { text: "Harbor 集群", link: "/architecture/harbor-cluster" },
            { text: "Socket 抽象", link: "/architecture/socket-abstraction" },
            { text: "Gate 服务", link: "/architecture/gate-service" }
          ]
        },
        {
          text: "Lua 集成",
          collapsed: false,
          items: [
            { text: "Lua 虚拟机管理", link: "/architecture/lua-vm" },
            { text: "Lua 服务", link: "/architecture/lua-service" },
            { text: "热更新机制", link: "/architecture/hot-reload" }
          ]
        },
        {
          text: "运行时工程",
          collapsed: false,
          items: [
            { text: "线程模型", link: "/architecture/thread-model" },
            { text: "内存管理", link: "/architecture/memory-management" },
            { text: "日志系统", link: "/architecture/logging" },
            { text: "监控与调试", link: "/architecture/monitoring" }
          ]
        },
        {
          text: "参考资料",
          collapsed: true,
          items: [
            { text: "设计哲学", link: "/architecture/design-philosophy" },
            { text: "与其他框架对比", link: "/architecture/comparison" }
          ]
        }
      ],
      "/analysis/": [
        {
          text: "源码分析",
          items: [
            { text: "核心模块概览", link: "/analysis/core-modules" },
            { text: "skynet_server.c", link: "/analysis/skynet-server" },
            { text: "skynet_start.c", link: "/analysis/skynet-start" },
            { text: "skynet_mq.c", link: "/analysis/skynet-mq" },
            { text: "skynet_handle.c", link: "/analysis/skynet-handle" },
            { text: "skynet_timer.c", link: "/analysis/skynet-timer" },
            { text: "socket_server.c", link: "/analysis/socket-server" }
          ]
        },
        {
          text: "Lua 层分析",
          collapsed: false,
          items: [
            { text: "skynet.lua", link: "/analysis/skynet-lua" },
            { text: "skynet.manager", link: "/analysis/skynet-manager" },
            { text: "socket 模块", link: "/analysis/socket-lua" },
            { text: "driver 模块", link: "/analysis/driver-lua" }
          ]
        },
        {
          text: "服务实现",
          collapsed: false,
          items: [
            { text: "Gate 服务", link: "/analysis/gate-service" },
            { text: "Harbor 服务", link: "/analysis/harbor-service" },
            { text: "Logger 服务", link: "/analysis/logger-service" }
          ]
        }
      ]
    },
    socialLinks: [
      { icon: "github", link: "https://github.com/cuihairu/skynet" }
    ],
    footer: {
      message: "基于源码的 Skynet 架构解析与设计原理分析",
      copyright: "Skynet Source Code Analysis"
    }
  }
}
