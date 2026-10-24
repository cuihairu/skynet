# Deployment Guide

This document describes how to deploy the Skynet source code analysis documentation to various platforms.

## Local Development

### Starting the Development Server

```bash
# Install dependencies
npm install

# Start the development server
npm run docs:dev
```

Visit http://localhost:5173/skynet/ to view the documentation.

### Using the Quick Start Script

```bash
./docs.sh
```

## Building the Production Version

```bash
# Build
npm run docs:build

# Preview the build result
npm run docs:preview
```

The build output is located in the `docs/.vitepress/dist/` directory.

## GitHub Pages

### 1. Enable GitHub Pages

1. Go to the repository Settings
2. Find the Pages option
3. Set Source to "GitHub Actions"

### 2. Create a GitHub Actions Workflow

Create `.github/workflows/deploy.yml`:

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

### 3. Push the Code

```bash
git add .
git commit -m "Add docs deployment"
git push origin master
```

The documentation will be automatically deployed to `https://<username>.github.io/skynet/`.

## Vercel

### 1. Connect the Repository

1. Visit [vercel.com](https://vercel.com)
2. Click "New Project"
3. Import your GitHub repository

### 2. Configure the Project

- **Framework Preset**: VitePress
- **Build Command**: `npm run docs:build`
- **Output Directory**: `docs/.vitepress/dist`
- **Install Command**: `npm install`

### 3. Deploy

Click the "Deploy" button to complete the deployment.

## Netlify

### 1. Connect the Repository

1. Visit [netlify.com](https://netlify.com)
2. Click "New site from Git"
3. Select your GitHub repository

### 2. Configure the Build

- **Build command**: `npm run docs:build`
- **Publish directory**: `docs/.vitepress/dist`

### 3. Deploy

Click the "Deploy site" button to complete the deployment.

## Cloudflare Pages

### 1. Connect the Repository

1. Visit the Cloudflare Dashboard
2. Go to Pages
3. Click "Create a project"
4. Connect your GitHub repository

### 2. Configure the Build

- **Production branch**: `master`
- **Build command**: `npm run docs:build`
- **Build output directory**: `docs/.vitepress/dist`

### 3. Deploy

Click the "Save and Deploy" button to complete the deployment.

## Custom Domains

### GitHub Pages

1. Find the Pages option in the repository settings
2. Enter your custom domain
3. Configure a DNS CNAME record pointing to `<username>.github.io`

### Vercel

1. Find Domains in the project settings
2. Add your custom domain
3. Configure DNS records

### Netlify

1. Find Domain management in the site settings
2. Add your custom domain
3. Configure DNS records

## Deployment Scripts

### Deploying to GitHub Pages

```bash
#!/bin/bash

# Build the documentation
npm run docs:build

# Enter the build directory
cd docs/.vitepress/dist

# Initialize Git
git init
git add -A
git commit -m 'deploy'

# Push to the gh-pages branch
git push -f git@github.com:<username>/skynet.git master:gh-pages

cd -
```

### Deploying to Netlify

```bash
#!/bin/bash

# Build the documentation
npm run docs:build

# Deploy using the Netlify CLI
npx netlify deploy --prod --dir=docs/.vitepress/dist
```

## Deployment Checklist

- [ ] The documentation builds correctly locally
- [ ] All links are accessible
- [ ] Images and static assets load correctly
- [ ] Mobile display works correctly
- [ ] Search functionality works
- [ ] Code highlighting displays correctly
- [ ] Mermaid diagrams render correctly

## Frequently Asked Questions

### 1. Asset Loading Failure

Check whether the `base` configuration is correct:

```js
// docs/.vitepress/config.mjs
export default {
  base: "/skynet/",  // Make sure it matches the deployment path
}
```

### 2. 404 Errors

Ensure the `cleanUrls` configuration is correct:

```js
export default {
  cleanUrls: false,  // Adjust according to the platform
}
```

### 3. Styling Issues

Clear the cache and rebuild:

```bash
rm -rf docs/.vitepress/cache
npm run docs:build
```

## Performance Optimization

### 1. Enable Compression

Vercel and Netlify enable gzip compression by default.

### 2. Configure Caching

Configure static asset caching on the deployment platform:

```
# Netlify _headers
/*
  Cache-Control: public, max-age=31536000
```

### 3. Use a CDN

Deployment platforms usually come with a CDN; no extra configuration is needed.

## Monitoring and Analytics

### 1. Google Analytics

Add the following to `.vitepress/config.mjs`:

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

### 2. Baidu Analytics

Similarly add Baidu Analytics code.

## Next Steps

- [Local Development](#local-development) - Run the documentation locally
- [GitHub Pages](#github-pages) - Deploy to GitHub Pages
- [Vercel](#vercel) - Deploy to Vercel
