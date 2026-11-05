# Documentation Development and Deployment

This site uses the checked-in VitePress configuration. English pages live under docs/ and Chinese pages under docs/zh/.

pnpm drives the toolchain. Enable it with `corepack enable pnpm` (Node.js 22.12+) or install it from [pnpm.io](https://pnpm.io/installation). Then:

```sh
pnpm install --frozen-lockfile
pnpm run docs:check
pnpm run docs:dev
```

Open `/skynet/` on the port printed by the development command. If the default port is occupied, do not confuse another application with this site.

```sh
pnpm run docs:build
pnpm run docs:preview
```

The build output is `docs/.vitepress/dist`. The base path is `/skynet/`, with `/skynet/zh/` for Chinese. The logo is `docs/public/logo.svg`.

[package.json:7](https://github.com/cuihairu/skynet/blob/37e742e044229a6c7eed3eb1eecef1ded80e868a/package.json#L7); [docs/.vitepress/config.mjs:214](https://github.com/cuihairu/skynet/blob/37e742e044229a6c7eed3eb1eecef1ded80e868a/docs/.vitepress/config.mjs#L214)

## GitHub Pages publication

The current workflow runs for documentation-related paths on master and for manual dispatch. It installs with pnpm install --frozen-lockfile, checks links, builds, uploads a Pages artifact and deploys with deploy-pages. Configure repository Pages to use GitHub Actions. No separate gh-pages branch or force-push deployment script is needed.

[.github/workflows/deploy-docs.yml:52](https://github.com/cuihairu/skynet/blob/37e742e044229a6c7eed3eb1eecef1ded80e868a/.github/workflows/deploy-docs.yml#L52); [.github/workflows/deploy-docs.yml:61](https://github.com/cuihairu/skynet/blob/37e742e044229a6c7eed3eb1eecef1ded80e868a/.github/workflows/deploy-docs.yml#L61)

English: https://cuihairu.github.io/skynet/

Chinese: https://cuihairu.github.io/skynet/zh/

The cuihairu.site mirror did not pass TLS checks in this audit and is not listed as verified. For another host, preserve the base path or change the configuration and rebuild. No deployment to another platform is claimed here.
