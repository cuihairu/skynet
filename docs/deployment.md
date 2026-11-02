# Documentation Development and Deployment

This site uses the checked-in VitePress configuration. English pages live under docs/ and Chinese pages under docs/zh/.

```sh
npm ci
npm run docs:check
npm run docs:dev
```

Open `/skynet/` on the port printed by the development command. If the default port is occupied, do not confuse another application with this site.

```sh
npm run docs:build
npm run docs:preview
```

The build output is `docs/.vitepress/dist`. The base path is `/skynet/`, with `/skynet/zh/` for Chinese. The logo is `docs/public/logo.svg`.

[package.json:7](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/package.json#L7); [docs/.vitepress/config.mjs:214](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/docs/.vitepress/config.mjs#L214)

## GitHub Pages publication

The current workflow runs for documentation-related paths on master and for manual dispatch. It installs with npm ci, checks links, builds, uploads a Pages artifact and deploys with deploy-pages. Configure repository Pages to use GitHub Actions. No separate gh-pages branch or force-push deployment script is needed.

[.github/workflows/deploy-docs.yml:12](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/.github/workflows/deploy-docs.yml#L12); [.github/workflows/deploy-docs.yml:63](https://github.com/cuihairu/skynet/blob/f9f8ae60b9ad287a69280f04ce6097e5c4c9f58b/.github/workflows/deploy-docs.yml#L63)

English: https://cuihairu.github.io/skynet/

Chinese: https://cuihairu.github.io/skynet/zh/

The cuihairu.site mirror did not pass TLS checks in this audit and is not listed as verified. For another host, preserve the base path or change the configuration and rebuild. No deployment to another platform is claimed here.
