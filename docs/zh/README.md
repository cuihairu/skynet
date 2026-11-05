# 文档开发与部署

本站使用仓库现有的 VitePress 配置，中英文页面分别位于 docs/ 与 docs/zh/。

文档工具链使用 pnpm。可通过 `corepack enable pnpm`（Node.js 22.12+）启用，或按 [pnpm.io](https://pnpm.io/zh/installation) 安装。然后执行：

```sh
pnpm install --frozen-lockfile
pnpm run docs:check
pnpm run docs:dev
```

打开启动命令实际输出端口下的 `/skynet/zh/`。默认端口被占用时不要把其他应用当作本站。

```sh
pnpm run docs:build
pnpm run docs:preview
```

构建目录是 `docs/.vitepress/dist`。基础路径为 `/skynet/`，中文入口为 `/skynet/zh/`；logo 位于 `docs/public/logo.svg`。

[package.json:7](https://github.com/cuihairu/skynet/blob/37e742e044229a6c7eed3eb1eecef1ded80e868a/package.json#L7); [docs/.vitepress/config.mjs:214](https://github.com/cuihairu/skynet/blob/37e742e044229a6c7eed3eb1eecef1ded80e868a/docs/.vitepress/config.mjs#L214)

## GitHub Pages 发布

当前工作流在 master 分支的文档相关路径变化或手动触发时执行。它运行 pnpm install --frozen-lockfile、链接检查和文档构建，再上传 Pages artifact 并使用 deploy-pages 发布。仓库 Pages 设置应使用 GitHub Actions；不需要另建 gh-pages 分支，也不应执行旧文档中的强制推送脚本。

[.github/workflows/deploy-docs.yml:52](https://github.com/cuihairu/skynet/blob/37e742e044229a6c7eed3eb1eecef1ded80e868a/.github/workflows/deploy-docs.yml#L52); [.github/workflows/deploy-docs.yml:61](https://github.com/cuihairu/skynet/blob/37e742e044229a6c7eed3eb1eecef1ded80e868a/.github/workflows/deploy-docs.yml#L61)

英文入口：https://cuihairu.github.io/skynet/

中文入口：https://cuihairu.github.io/skynet/zh/

自定义域名 cuihairu.site 在本轮未通过 TLS 检查，不列为已验证镜像。部署到其他平台时需要保留基础路径，或先修改配置并重新构建；这里不宣称已经部署到这些平台。
