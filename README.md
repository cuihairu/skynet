# Skynet

<p align="center"><img src="docs/public/logo.svg" width="96" alt="Skynet logo"></p>

<p align="center"><a href="https://cuihairu.github.io/skynet/"><img src="https://img.shields.io/badge/docs-English-blue" alt="English documentation"></a> <a href="https://cuihairu.github.io/skynet/zh/"><img src="https://img.shields.io/badge/docs-简体中文-blue" alt="简体中文文档"></a> <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-green" alt="MIT license"></a></p>

This repository is a fork of [cloudwu/skynet](https://github.com/cloudwu/skynet), a C/Lua actor framework, with English and Chinese source-reading notes. The notes cover selected runtime paths; they are not a complete API specification or an automatic hot-update system.

[简体中文](README.zh-CN.md)

## Documentation

- [English documentation](https://cuihairu.github.io/skynet/)
- [Build from source](docs/getting-started/build-from-source.md)
- [Core API and configuration](docs/getting-started/api-reference.md)
- [Architecture](docs/architecture/overview.md)
- [Source entry points](docs/analysis/core-modules.md)

The published site uses `/skynet/`; Chinese pages use `/skynet/zh/`. The verified public host is GitHub Pages. A custom-domain mirror is not advertised here because its TLS connection could not be verified in this audit.

## Build and test

On Linux, install a C compiler, GNU Make and autoconf, then:

```sh
git clone https://github.com/cuihairu/skynet.git
cd skynet
make linux
bash test/run_autotest.sh testcrypt testbson testsha testcoroutine testtimeout testresponse
```

The test helper starts temporary Skynet nodes. For an interactive example, run `./skynet examples/config` and `./3rd/lua/lua examples/client.lua` in separate terminals. Other platform targets are listed in [platform.mk](platform.mk); their presence is not a claim that every platform passed this local Linux audit.

The bundled Lua source reports **5.5.1**, in [3rd/lua/lua.h](3rd/lua/lua.h).

## Local documentation

pnpm drives the docs toolchain. Install pnpm from [pnpm.io](https://pnpm.io/installation); on Node.js 22.12–24, bundled Corepack can also be enabled with `corepack enable pnpm`. Node.js 25+ no longer bundles Corepack. Then:

```sh
pnpm install --frozen-lockfile
pnpm run docs:check
pnpm run docs:dev
```

After the development server starts, open the `/skynet/` path on the port it prints. The default requested port is 5173; an existing service may already occupy it. Build with `pnpm run docs:build` and preview with `pnpm run docs:preview`.

## Upstream and license

- [Upstream wiki](https://github.com/cloudwu/skynet/wiki)
- [Upstream discussions](https://github.com/cloudwu/skynet/discussions)
- [MIT license](LICENSE)
