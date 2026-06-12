// ===== UG AI Developer Panel Backend v5 =====
// Full-featured: Chat + Files + Logs + Settings + Auto-start
// Z.AI GLM Integration za SA:MP/open.mp server

const express = require('express');
const http = require('http');
const fs = require('fs');
const path = require('path');

// ===== CRASH PROTECTION =====
process.on('uncaughtException', (err) => {
  console.error('[UG-AI] UNCAUGHT:', err.message);
});
process.on('unhandledRejection', (reason) => {
  console.error('[UG-AI] UNHANDLED:', reason);
});

// ===== Z.AI SDK =====
let zaiSdk = null;
async function initZAISdk() {
  if (zaiSdk) return zaiSdk;
  try {
    const ZAIModule = require('z-ai-web-dev-sdk');
    const ZAIClass = ZAIModule.default || ZAIModule;
    zaiSdk = await new ZAIClass();
    console.log('[UG-AI v5] Z.AI SDK initialized OK');
    return zaiSdk;
  } catch (e) {
    console.error('[UG-AI v5] Z.AI SDK init failed:', e.message);
    return null;
  }
}

// ===== CONFIG =====
const PORT = parseInt(process.env.PORT) || 3777;
const PROJECT_ROOT = path.resolve(__dirname, '..');
const CEF_DIR = path.join(PROJECT_ROOT, 'scriptfiles', 'cef');
const ADMIN_KEY = process.env.ADMIN_PASSWORD || 'ug-admin-2024';
const LOG_FILE = path.join(__dirname, 'operations.log');

// ===== STATE =====
const sessions = new Map();
let operationLog = [];

function getSession(playerid) {
  if (!sessions.has(playerid)) {
    sessions.set(playerid, { messages: [], status: 'idle' });
  }
  return sessions.get(playerid);
}

function logOp(action, details) {
  const entry = { time: new Date().toISOString(), action, details: details || '' };
  operationLog.push(entry);
  if (operationLog.length > 500) operationLog = operationLog.slice(-250);
  try {
    fs.appendFileSync(LOG_FILE, JSON.stringify(entry) + '\n');
  } catch (e) {}
}

// ===== SYSTEM PROMPT =====
const SYSTEM_PROMPT = `Ti si UG AI Developer Assistant - ekspert za SA:MP/open.mp Pawn skriptiranje i CEF (HTML/CSS/JS) development.
Pomažeš developerima da prave i modificiraju gamemode sisteme, CEF panele, i Pawn skripte.
Odgovaraj na bosanskom/hrvatskom/srpskom jeziku ako korisnik piše na tom jeziku, inače na engleskom.
Kada daješ kod, koristi code blokove sa jezikom (npr. \`\`\`pawn, \`\`\`javascript, \`\`\`html).
Budi konkretan i daj direktne kod primjere.`;

// ===== EXPRESS =====
const app = express();
const server = http.createServer(app);
app.use(express.json({ limit: '10mb' }));

// CORS
app.use((req, res, next) => {
  res.header('Access-Control-Allow-Origin', '*');
  res.header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS');
  res.header('Access-Control-Allow-Headers', 'Content-Type, Authorization, X-Admin-Key');
  if (req.method === 'OPTIONS') return res.sendStatus(200);
  next();
});

// ===== MANUAL CEF FILE SERVING =====
const CEF_MIME = {
  '.html': 'text/html', '.css': 'text/css', '.js': 'application/javascript',
  '.json': 'application/json', '.png': 'image/png', '.jpg': 'image/jpeg',
  '.gif': 'image/gif', '.svg': 'image/svg+xml', '.ico': 'image/x-icon',
  '.woff': 'font/woff', '.woff2': 'font/woff2', '.ttf': 'font/ttf',
};

app.get('/cef/*', (req, res) => {
  const relPath = req.params[0];
  const fullPath = path.join(CEF_DIR, relPath);
  const resolved = path.resolve(fullPath);
  if (!resolved.startsWith(CEF_DIR)) return res.status(403).send('Forbidden');
  if (!fs.existsSync(resolved)) return res.status(404).send('Not found');
  const ext = path.extname(resolved).toLowerCase();
  res.setHeader('Content-Type', CEF_MIME[ext] || 'application/octet-stream');
  res.setHeader('Cache-Control', 'no-store, no-cache, must-revalidate');
  try { fs.createReadStream(resolved).pipe(res); } catch (e) { res.status(500).send('Error'); }
});

// ===== AUTH =====
function authCheck(req, res, next) {
  const key = req.headers['x-admin-key'] || req.query.key || '';
  if (key !== ADMIN_KEY) return res.status(403).json({ error: 'Unauthorized' });
  next();
}

// ===== SAFE PATH =====
function safePath(requestedPath) {
  const resolved = path.resolve(PROJECT_ROOT, requestedPath);
  if (!resolved.startsWith(PROJECT_ROOT)) return null;
  return resolved;
}

// ===== HEALTH =====
app.get('/api/health', (req, res) => {
  res.json({
    status: 'ok',
    sdkAvailable: !!zaiSdk,
    uptime: Math.floor(process.uptime()),
    sessions: sessions.size,
    projectRoot: PROJECT_ROOT,
  });
});

// ===== CHAT API =====
app.post('/api/chat', async (req, res) => {
  const { message, playerid, history } = req.body;
  if (!message) return res.status(400).json({ error: 'Message required' });

  if (!zaiSdk) { await initZAISdk(); }
  if (!zaiSdk) return res.status(503).json({ error: 'AI SDK not available. Restart backend.' });

  const pid = playerid || 0;
  const session = getSession(pid);

  try {
    const messages = [{ role: 'system', content: SYSTEM_PROMPT }];
    if (history && history.length > 0) {
      messages.push(...history.slice(-20));
    }
    messages.push({ role: 'user', content: message });

    const response = await zaiSdk.chat.completions.create({
      messages,
      temperature: 0.7,
      max_tokens: 4096,
    });

    const aiReply = response.choices?.[0]?.message?.content || 'Nisam mogao generirati odgovor.';

    session.messages.push({ role: 'user', content: message });
    session.messages.push({ role: 'assistant', content: aiReply });
    if (session.messages.length > 100) session.messages = session.messages.slice(-50);

    logOp('chat', { playerid: pid, msgLen: message.length, replyLen: aiReply.length });
    console.log('[UG-AI] Chat OK (player %d, %db in, %db out)', pid, message.length, aiReply.length);
    res.json({ response: aiReply, status: 'idle' });
  } catch (e) {
    console.error('[UG-AI] Chat error:', e.message);
    res.status(500).json({ error: e.message, status: 'error' });
  }
});

// ===== FILES API =====
app.get('/api/files', authCheck, (req, res) => {
  const dir = req.query.dir || '';
  const fullPath = safePath(dir);
  if (!fullPath) return res.status(400).json({ error: 'Invalid path' });
  if (!fs.existsSync(fullPath)) return res.status(404).json({ error: 'Not found' });

  try {
    const items = [];
    for (const entry of fs.readdirSync(fullPath, { withFileTypes: true })) {
      if (entry.name.startsWith('.') || entry.name === 'node_modules') continue;
      const rel = path.join(dir, entry.name);
      if (entry.isDirectory()) {
        items.push({ name: entry.name, path: rel, type: 'dir' });
      } else {
        const ext = path.extname(entry.name).toLowerCase();
        if (['.pwn','.inc','.html','.css','.js','.json','.cfg','.txt','.xml','.md','.amx','.log'].includes(ext) || entry.name === 'server.cfg') {
          try {
            const s = fs.statSync(path.join(fullPath, entry.name));
            items.push({ name: entry.name, path: rel, type: 'file', size: s.size, ext });
          } catch (e) {}
        }
      }
    }
    // Sort: dirs first, then files
    items.sort((a, b) => {
      if (a.type !== b.type) return a.type === 'dir' ? -1 : 1;
      return a.name.localeCompare(b.name);
    });
    res.json({ path: dir, items });
  } catch (e) {
    res.status(500).json({ error: e.message });
  }
});

app.get('/api/files/*', authCheck, (req, res) => {
  const filePath = req.params[0];
  const fullPath = safePath(filePath);
  if (!fullPath) return res.status(400).json({ error: 'Invalid path' });
  if (!fs.existsSync(fullPath)) return res.status(404).json({ error: 'Not found' });

  try {
    const content = fs.readFileSync(fullPath, 'utf-8');
    const stat = fs.statSync(fullPath);
    res.json({ path: filePath, content, size: stat.size, modified: stat.mtime.toISOString() });
  } catch (e) {
    res.status(500).json({ error: e.message });
  }
});

app.put('/api/files/*', authCheck, (req, res) => {
  const filePath = req.params[0];
  const { content } = req.body;
  if (content === undefined) return res.status(400).json({ error: 'Content required' });

  const fullPath = safePath(filePath);
  if (!fullPath) return res.status(400).json({ error: 'Invalid path' });

  // Block certain files
  const blocked = [/node_modules/i, /\.env$/i, /package-lock/i];
  for (const p of blocked) { if (p.test(filePath)) return res.status(403).json({ error: `Cannot modify: ${filePath}` }); }

  // Backup if file exists
  if (fs.existsSync(fullPath)) {
    const backupDir = path.join(__dirname, 'backups');
    if (!fs.existsSync(backupDir)) fs.mkdirSync(backupDir, { recursive: true });
    const ts = new Date().toISOString().replace(/[:.]/g, '-');
    const bakName = `${path.basename(filePath)}.${ts}.bak`;
    try { fs.copyFileSync(fullPath, path.join(backupDir, bakName)); } catch (e) {}
  }

  try {
    const dir = path.dirname(fullPath);
    if (!fs.existsSync(dir)) fs.mkdirSync(dir, { recursive: true });
    fs.writeFileSync(fullPath, content, 'utf-8');
    logOp('write', { file: filePath, size: content.length });
    res.json({ success: true, path: filePath });
  } catch (e) {
    res.status(500).json({ error: e.message });
  }
});

// ===== LOGS API =====
app.get('/api/logs', authCheck, (req, res) => {
  const limit = parseInt(req.query.limit) || 100;
  res.json({ logs: operationLog.slice(-limit) });
});

// ===== SESSION API =====
app.post('/api/session/clear', authCheck, (req, res) => {
  sessions.delete(req.body.playerid || 0);
  res.json({ success: true });
});

// ===== ROOT =====
app.get('/', (req, res) => {
  res.json({
    name: 'UG AI Dev Panel Backend v5',
    health: '/api/health',
    chat: 'POST /api/chat',
    cefPanel: '/cef/ai-dev-panel/index.html',
    sdkReady: !!zaiSdk,
  });
});

// ===== START =====
server.listen(PORT, '0.0.0.0', async () => {
  console.log('[UG-AI v5] Backend running on http://0.0.0.0:%d', PORT);
  console.log('[UG-AI v5] CEF panel: http://localhost:%d/cef/ai-dev-panel/index.html', PORT);
  console.log('[UG-AI v5] Health: http://localhost:%d/api/health', PORT);
  await initZAISdk();
});
