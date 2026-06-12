// ===== UG AI Developer Panel Backend v4.1 =====
// Robust, crash-proof, serves CEF files + API on same port
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
    const ZAI = ZAIModule.default || ZAIModule;
    zaiSdk = await ZAI.create();
    console.log('[UG-AI v4] Z.AI SDK initialized OK');
    return zaiSdk;
  } catch (e) {
    console.error('[UG-AI v4] Z.AI SDK init failed:', e.message);
    return null;
  }
}

// ===== CONFIG =====
const PORT = parseInt(process.env.PORT) || 3777;
const PROJECT_ROOT = path.resolve(__dirname, '..');
const CEF_DIR = path.join(PROJECT_ROOT, 'scriptfiles', 'cef');

// ===== STATE =====
const sessions = new Map();

function getSession(playerid) {
  if (!sessions.has(playerid)) {
    sessions.set(playerid, { messages: [], status: 'idle' });
  }
  return sessions.get(playerid);
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
// (express.static was causing crashes - using manual streaming instead)
const CEF_MIME_TYPES = {
  '.html': 'text/html',
  '.css': 'text/css',
  '.js': 'application/javascript',
  '.json': 'application/json',
  '.png': 'image/png',
  '.jpg': 'image/jpeg',
  '.gif': 'image/gif',
  '.svg': 'image/svg+xml',
  '.ico': 'image/x-icon',
  '.woff': 'font/woff',
  '.woff2': 'font/woff2',
  '.ttf': 'font/ttf',
};

app.get('/cef/*', (req, res) => {
  const relPath = req.params[0];
  // Prevent path traversal
  const fullPath = path.join(CEF_DIR, relPath);
  const resolved = path.resolve(fullPath);
  if (!resolved.startsWith(CEF_DIR)) {
    return res.status(403).send('Forbidden');
  }
  if (!fs.existsSync(resolved)) {
    return res.status(404).send('Not found');
  }

  const ext = path.extname(resolved).toLowerCase();
  const mime = CEF_MIME_TYPES[ext] || 'application/octet-stream';
  res.setHeader('Content-Type', mime);
  res.setHeader('Cache-Control', 'no-store, no-cache, must-revalidate');

  try {
    fs.createReadStream(resolved).pipe(res);
  } catch (e) {
    res.status(500).send('Error reading file');
  }
});

// ===== HEALTH =====
app.get('/api/health', (req, res) => {
  res.json({
    status: 'ok',
    sdkAvailable: !!zaiSdk,
    uptime: process.uptime(),
    sessions: sessions.size,
  });
});

// ===== CHAT API =====
app.post('/api/chat', async (req, res) => {
  const { message, playerid, history } = req.body;
  if (!message) return res.status(400).json({ error: 'Message required' });

  // Auto-retry SDK init
  if (!zaiSdk) {
    await initZAISdk();
    if (!zaiSdk) return res.status(503).json({ error: 'AI SDK not available. Restart backend.' });
  }

  const pid = playerid || 0;
  const session = getSession(pid);

  try {
    // Build messages
    const messages = [
      { role: 'system', content: SYSTEM_PROMPT },
    ];
    // Add session history
    if (session.messages.length > 0) {
      messages.push(...session.messages.slice(-20));
    }
    // Add current message
    messages.push({ role: 'user', content: message });

    // Call Z.AI
    const response = await zaiSdk.chat.completions.create({
      messages,
      temperature: 0.7,
      max_tokens: 4096,
    });

    const aiReply = response.choices?.[0]?.message?.content || 'Nisam mogao generirati odgovor.';

    // Save to session
    session.messages.push({ role: 'user', content: message });
    session.messages.push({ role: 'assistant', content: aiReply });

    // Trim session
    if (session.messages.length > 100) {
      session.messages = session.messages.slice(-50);
    }

    console.log('[UG-AI] Chat OK (player %d, %db in, %db out)', pid, message.length, aiReply.length);
    res.json({ response: aiReply, status: 'idle' });
  } catch (e) {
    console.error('[UG-AI] Chat error:', e.message);
    res.status(500).json({ error: e.message, status: 'error' });
  }
});

// ===== SESSION =====
app.post('/api/session/clear', (req, res) => {
  sessions.delete(req.body.playerid || 0);
  res.json({ success: true });
});

// ===== ROOT =====
app.get('/', (req, res) => {
  res.json({
    name: 'UG AI Dev Panel Backend v4.1',
    health: '/api/health',
    chat: 'POST /api/chat',
    cefPanel: '/cef/ai-dev-panel/index.html',
    sdkReady: !!zaiSdk,
  });
});

// ===== START =====
server.listen(PORT, '0.0.0.0', async () => {
  console.log('[UG-AI v4] Backend running on http://0.0.0.0:%d', PORT);
  console.log('[UG-AI v4] CEF panel: http://localhost:%d/cef/ai-dev-panel/index.html', PORT);
  console.log('[UG-AI v4] Health: http://localhost:%d/api/health', PORT);
  await initZAISdk();
});
