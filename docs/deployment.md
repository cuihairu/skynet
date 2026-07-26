# 部署指南

本文档介绍如何将 Skynet 源码解析文档部署到各种平台。

## 本地开发

### 启动开发服务器

```bash
# 安装依赖
npm install

# 启动开发服务器
npm run docs:dev
```

访问 http://localhost:5173/skynet/ 查看文档。

### 使用快速启动脚本

```bash
./docs.sh
```

## 构建生产版本

```bash
# 构建
npm run docs:build

# 预览构建结果
npm run docs:preview
```

构建产物位于 `docs/.vitepress/dist/` 目录。

## GitHub Pages

### 1. 启用 GitHub Pages

1. 进入仓库设置 (Settings)
2. 找到 Pages 选项
3. 选择 Source 为 "GitHub Actions"

### 2. 创建 GitHub Actions 工作流

创建 `.github/workflows/deploy.yml`：

```yaml
name: Deploy Docs

on:
  push:
    branches:
      - master

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout
        uses: actions/checkout@v4

      - name: Setup Node.js
        uses: actions/setup-node@v4
        with:
          node-version: '18'

      - name: Install dependencies
        run: npm install

      - name: Build docs
        run: npm run docs:build

      - name: Deploy to GitHub Pages
        uses: peaceiris/actions-gh-pages@v3
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          publish_dir: docs/.vitepress/dist
```

### 3. 推送代码

```bash
git add .
git commit -m "Add docs deployment"
git push origin master
```

文档将自动部署到 `https://<username>.github.io/skynet/`。

## Vercel

### 1. 连接仓库

1. 访问 [vercel.com](https://vercel.com)
2. 点击 "New Project"
3. 导入 GitHub 仓库

### 2. 配置项目

- **Framework Preset**: VitePress
- **Build Command**: `npm run docs:build`
- **Output Directory**: `docs/.vitepress/dist`
- **Install Command**: `npm install`

### 3. 部署

点击 "Deploy" 按钮即可完成部署。

## Netlify

### 1. 连接仓库

1. 访问 [netlify.com](https://netlify.com)
2. 点击 "New site from Git"
3. 选择 GitHub 仓库

### 2. 配置构建

- **Build command**: `npm run docs:build`
- **Publish directory**: `docs/.vitepress/dist`

### 3. 部署

点击 "Deploy site" 按钮即可完成部署。

## Cloudflare Pages

### 1. 连接仓库

1. 访问 Cloudflare Dashboard
2. 进入 Pages
3. 点击 "Create a project"
4. 连接 GitHub 仓库

### 2. 配置构建

- **Production branch**: `master`
- **Build command**: `npm run docs:build`
- **Build output directory**: `docs/.vitepress/dist`

### 3. 部署

点击 "Save and Deploy" 按钮即可完成部署。

## 自定义域名

### GitHub Pages

1. 在仓库设置中找到 Pages 选项
2. 输入自定义域名
3. 配置 DNS CNAME 记录指向 `<username>.github.io`

### Vercel

1. 在项目设置中找到 Domains
2. 添加自定义域名
3. 配置 DNS 记录

### Netlify

1. 在站点设置中找到 Domain management
2. 添加自定义域名
3. 配置 DNS 记录

## 部署脚本

### 部署到 GitHub Pages

```bash
#!/bin/bash

# 构建文档
npm run docs:build

# 进入构建目录
cd docs/.vitepress/dist

# 初始化 Git
git init
git add -A
git commit -m 'deploy'

# 推送到 gh-pages 分支
git push -f git@github.com:<username>/skynet.git master:gh-pages

cd -
```

### 部署到 Netlify

```bash
#!/bin/bash

# 构建文档
npm run docs:build

# 使用 Netlify CLI 部署
npx netlify deploy --prod --dir=docs/.vitepress/dist
```

## 部署检查清单

- [ ] 文档能在本地正常构建
- [ ] 所有链接都能正常访问
- [ ] 图片和静态资源能正常加载
- [ ] 移动端显示正常
- [ ] 搜索功能正常工作
- [ ] 代码高亮正常显示
- [ ] Mermaid 图表正常渲染

## 常见问题

### 1. 资源加载失败

检查 `base` 配置是否正确：

```js
// docs/.vitepress/config.mjs
export default {
  base: "/skynet/",  // 确保与部署路径一致
}
```

### 2. 404 错误

确保 `cleanUrls` 配置正确：

```js
export default {
  cleanUrls: false,  // 根据平台调整
}
```

### 3. 样式问题

清除缓存后重新构建：

```bash
rm -rf docs/.vitepress/cache
npm run docs:build
```

## 性能优化

### 1. 启用压缩

Vercel 和 Netlify 默认启用 gzip 压缩。

### 2. 配置缓存

在部署平台配置静态资源缓存：

```
# Netlify _headers
/*
  Cache-Control: public, max-age=31536000
```

### 3. 使用 CDN

部署平台通常自带 CDN，无需额外配置。

## 监控与分析

### 1. Google Analytics

在 `.vitepress/config.mjs` 中添加：

```js
export default {
  head: [
    ['script', { async: true, src: 'https://www.googletagmanager.com/gtag/js?id=G-XXXXXXXXXX' }],
    ['script', {}, `
      window.dataLayer = window.dataLayer || [];
      function gtag(){dataLayer.push(arguments);}
      gtag('js', new Date());
      gtag('config', 'G-XXXXXXXXXX');
    `]
  ]
}
```

### 2. 百度统计

类似地添加百度统计代码。

## 下一步

- [本地开发](#本地开发) - 在本地运行文档
- [GitHub Pages](#github-pages) - 部署到 GitHub Pages
- [Vercel](#vercel) - 部署到 Vercel
