#!/usr/bin/env node
/**
 * Verify that every internal link in the docs sources resolves to a real file.
 *
 * Checks, per markdown file under docs/ (build output excluded):
 *   - inline markdown links and images, [text](target)
 *   - reference-style definitions,        [text]: target
 *   - vitepress frontmatter links,        link: /some/page
 *
 * Externals (http/https/mailto/tel/protocol-relative) are skipped; pure
 * in-page anchors (#frag) are skipped. Everything else must resolve to an
 * existing file: markdown pages accept both an explicit .md and a
 * directory route backed by index.md, and assets are looked up in docs/
 * and in the VitePress publicDir (docs/public).
 *
 * Usage: node scripts/check-docs-links.mjs   (exit 1 when dead links exist)
 */
import { existsSync, readdirSync, readFileSync, statSync } from 'node:fs';
import path from 'node:path';

const DOCS = path.resolve('docs');
const SKIP_DIRS = new Set(['dist', '.cache', 'node_modules', '.vitepress', 'assets']);

const INLINE_LINK = /\[[^\]]*\]\(\s*([^)\s]+)(?:\s+"[^"]*")?\s*\)/g;
const REF_DEF = /^[ \t]{0,3}\[[^\]]+\]:[ \t]+(\S+)/gm;
const FM_LINK = /^[ \t]*(?:link|href):\s*["']?([^"'\s#]+)["']?[ \t]*$/gm;

function walk(dir, out = []) {
  for (const entry of readdirSync(dir, { withFileTypes: true })) {
    if (SKIP_DIRS.has(entry.name)) continue;
    const full = path.join(dir, entry.name);
    if (entry.isDirectory()) walk(full, out);
    else if (entry.isFile() && /\.md$/.test(entry.name)) out.push(full);
  }
  return out;
}

function isExternal(url) {
  return /^(?:[a-z][a-z0-9+.-]*:|\/\/)/i.test(url);
}

/** Candidate filesystem paths for a resolved markdown page path. */
function candidates(base) {
  const list = [];
  list.push(base);
  if (!/\.[a-z0-9]+$/i.test(base)) {
    list.push(base + '.md');
    list.push(path.join(base, 'index.md'));
    list.push(base + '/');
    list.push(base.replace(/\/$/, '') + '.md');
  }
  // VitePress publicDir assets, e.g. /logo.svg -> docs/public/logo.svg
  if (base.startsWith('/') && /\.[a-z0-9]+$/i.test(base)) {
    list.push(path.join(DOCS, 'public', base.slice(1)));
  }
  return list;
}

function resolve(file, raw) {
  const cleaned = raw.split('#')[0].split('?')[0].split(' ')[0].trim();
  if (!cleaned) return null;              // pure anchor or empty
  if (isExternal(cleaned)) return null;   // external, not verifiable here
  const abs = cleaned.startsWith('/')
    ? path.join(DOCS, cleaned)
    : path.normalize(path.join(path.dirname(file), cleaned));
  return candidates(abs);
}

function extract(file) {
  const text = readFileSync(file, 'utf8');
  const found = [];
  for (const [idx, regex] of [
    [0, INLINE_LINK],
    [1, REF_DEF],
    [2, FM_LINK]
  ]) {
    regex.lastIndex = 0;
    let m;
    while ((m = regex.exec(text))) {
      const line = text.slice(0, m.index).split('\n').length;
      const base = resolve(file, m[1]);
      if (base) found.push({ line, raw: m[1], base });
    }
  }
  return found;
}

const files = walk(DOCS).sort();
let checked = 0;
const dead = [];

for (const file of files) {
  for (const { line, raw, base } of extract(file)) {
    checked += 1;
    const ok = base.some((c) => {
      try { return statSync(c).isFile(); } catch { return false; }
    });
    if (!ok) dead.push({ file: path.relative(process.cwd(), file), line, raw, tried: base });
  }
}

const rel = (p) => path.relative(DOCS, p) || '.';
for (const file of files) {
  const links = extract(file).filter((l) => l.base && !l.base.some((c) => { try { return statSync(c).isFile(); } catch { return false; } }));
  if (links.length) {
    process.stdout.write(`${rel(file)}: ${links.length} dead link(s)\n`);
    for (const { line, raw } of links) {
      process.stdout.write(`    L${line} -> ${raw}\n`);
    }
  }
}

process.stdout.write(`\nfiles=${files.length} internal_links_checked=${checked} dead=${dead.length}\n`);
process.exit(dead.length ? 1 : 0);
