# Skynet Source Code Analysis Documentation

This is the source code analysis documentation for the Skynet game server framework, built with VitePress.

## Local Development

### Installing Dependencies

```bash
npm install
```

### Starting the Development Server

```bash
npm run docs:dev
```

Visit http://localhost:5173/skynet/ to view the documentation.

### Building the Production Version

```bash
npm run docs:build
```

### Previewing the Production Version

```bash
npm run docs:preview
```

## Documentation Structure

```
docs/
├── index.md                    # Home page
├── getting-started/            # Getting started
│   └── index.md
├── architecture/               # Architecture analysis
│   ├── overview.md            # Architecture overview
│   ├── actor-model.md         # Actor model
│   └── ...
├── analysis/                   # Source code analysis
│   ├── core-modules.md        # Core modules overview
│   ├── skynet-server.md       # skynet_server.c analysis
│   └── ...
├── public/                     # Static assets
│   └── logo.svg
└── .vitepress/                 # VitePress configuration
    ├── config.mjs             # Main config file
    └── theme/                 # Custom theme
        ├── index.js
        ├── style.css
        └── components/
            └── MermaidDiagram.vue
```

## Adding New Documentation

1. Create a `.md` file in the appropriate directory
2. Add a link in the sidebar of `docs/.vitepress/config.mjs`
3. Write content using Markdown syntax

## Deployment

The documentation can be deployed to platforms such as GitHub Pages, Vercel, and Netlify.

### GitHub Pages

1. Enable GitHub Pages in the repository settings
2. Select the `gh-pages` branch or the `docs/` directory
3. It will deploy automatically after you push code

### Vercel/Netlify

1. Connect your GitHub repository
2. Set the build command to `npm run docs:build`
3. Set the output directory to `docs/.vitepress/dist`

## Contributing

Pull Requests to improve the documentation are welcome!

## License

The documentation is released under the MIT License.
