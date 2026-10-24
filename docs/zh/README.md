# Skynet 源码解析文档

这是 Skynet 游戏服务器框架的源码解析文档，使用 VitePress 构建。

## 本地开发

### 安装依赖

```bash
npm install
```

### 启动开发服务器

```bash
npm run docs:dev
```

访问 http://localhost:5173/skynet/ 查看文档。

### 构建生产版本

```bash
npm run docs:build
```

### 预览生产版本

```bash
npm run docs:preview
```

## 文档结构

```
docs/
├── index.md                    # 首页
├── getting-started/            # 快速开始
│   └── index.md
├── architecture/               # 架构解析
│   ├── overview.md            # 架构概览
│   ├── actor-model.md         # Actor 模型
│   └── ...
├── analysis/                   # 源码分析
│   ├── core-modules.md        # 核心模块概览
│   ├── skynet-server.md       # skynet_server.c 分析
│   └── ...
├── public/                     # 静态资源
│   └── logo.svg
└── .vitepress/                 # VitePress 配置
    ├── config.mjs             # 主配置文件
    └── theme/                 # 自定义主题
        ├── index.js
        ├── style.css
        └── components/
            └── MermaidDiagram.vue
```

## 添加新文档

1. 在相应目录下创建 `.md` 文件
2. 在 `docs/.vitepress/config.mjs` 的 sidebar 中添加链接
3. 使用 Markdown 语法编写内容

## 部署

文档可以部署到 GitHub Pages、Vercel、Netlify 等平台。

### GitHub Pages

1. 在仓库设置中启用 GitHub Pages
2. 选择 `gh-pages` 分支或 `docs/` 目录
3. 推送代码后会自动部署

### Vercel/Netlify

1. 连接 GitHub 仓库
2. 设置构建命令为 `npm run docs:build`
3. 设置输出目录为 `docs/.vitepress/dist`

## 贡献

欢迎提交 Pull Request 来完善文档！

## 许可证

文档采用 MIT 许可证。
