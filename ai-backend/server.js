// ===== UG AI Developer Panel Backend v3 =====
// Modular Job Engine + CEF Registry + Live Reload + GMX
// Z.AI GLM-5.1 Integration za SA:MP/open.mp server
// Uses z-ai-web-dev-sdk for auto-authentication

require('dotenv').config();
const express = require('express');
const http = require('http');
const { WebSocketServer } = require('ws');
const fs = require('fs');
const path = require('path');
const Diff = require('diff');
const { execSync } = require('child_process');

// ===== Z.AI SDK (ESM dynamic import) =====
let zaiSdk = null;
async function initZAISdk() {
  if (zaiSdk) return zaiSdk;
  try {
    const mod = await import('z-ai-web-dev-sdk');
    const ZAI = mod.default;
    zaiSdk = await ZAI.create();
    console.log('[UG-AI v3] Z.AI SDK initialized (auto-auth)');
    return zaiSdk;
  } catch (e) {
    console.error('[UG-AI v3] Z.AI SDK init failed:', e.message);
    return null;
  }
}
// Init on startup
initZAISdk();

// ===== CRASH PROTECTION =====
process.on('uncaughtException', (err) => {
  console.error('[UG-AI] UNCAUGHT EXCEPTION:', err.message);
  console.error(err.stack);
});
process.on('unhandledRejection', (reason, promise) => {
  console.error('[UG-AI] UNHANDLED REJECTION:', reason);
});

// ===== CONFIG =====
const CONFIG = {
  port: parseInt(process.env.PORT) || 3777,
  projectRoot: process.env.PROJECT_ROOT || path.resolve(__dirname, '..'),
  apiKey: process.env.ZAI_API_KEY || '',
  baseUrl: (process.env.ZAI_BASE_URL || 'https://api.z.ai/api/paas/v4').replace(/\/+$/, ''),
  model: process.env.ZAI_MODEL || 'glm-5.1',
  pawnCompiler: process.env.PAWN_COMPILER || '/tmp/pawncc',
  gamemodePath: process.env.GAMEMODE_PATH || 'gamemodes/fg-ogc.pwn',
  adminPassword: process.env.ADMIN_PASSWORD || 'ug-admin-2024',
  maxBackups: parseInt(process.env.MAX_BACKUPS) || 50,
  rconPassword: process.env.RCON_PASSWORD || '',
  rconHost: process.env.RCON_HOST || '127.0.0.1',
  rconPort: parseInt(process.env.RCON_PORT) || 7777,
};

const BACKUP_DIR = path.join(__dirname, 'backups');
const LOG_DIR = path.join(__dirname, 'logs');
const PATCH_DIR = path.join(__dirname, 'patches');
[BACKUP_DIR, LOG_DIR, PATCH_DIR].forEach(d => { if (!fs.existsSync(d)) fs.mkdirSync(d, { recursive: true }); });

// ===== CEF REGISTRY - Map panel names to browser IDs and file paths =====
const CEF_REGISTRY = {
  phone:      { browserId: 10, path: 'scriptfiles/cef/phone/index.html',      name: 'Phone' },
  tablet:     { browserId: 20, path: 'scriptfiles/cef/tablet/index.html',      name: 'Tablet' },
  inventory:  { browserId: 30, path: 'scriptfiles/cef/inventory/index.html',   name: 'Inventory' },
  laptop:     { browserId: 40, path: 'scriptfiles/cef/laptop/index.html',      name: 'Laptop' },
  portal:     { browserId: 60, path: 'scriptfiles/cef/portal/index.html',      name: 'Portal' },
  amenu:      { browserId: 70, path: 'scriptfiles/cef/amenu/index.html',       name: 'Admin Menu' },
  case:       { browserId: 80, path: 'scriptfiles/cef/case/index.html',        name: 'Case Opening' },
  cardcase:   { browserId: 81, path: 'scriptfiles/cef/cardcase/index.html',    name: 'Card Case' },
  dog:        { browserId: 82, path: 'scriptfiles/cef/dog/index.html',         name: 'Dog Panel' },
  aidev:      { browserId: 90, path: 'scriptfiles/cef/ai-dev-panel/index.html', name: 'AI Dev Panel' },
};

// Build reverse mapping: file path prefix → panel key
const CEF_PATH_MAP = {};
for (const [key, info] of Object.entries(CEF_REGISTRY)) {
  CEF_PATH_MAP[info.path.replace('/index.html', '')] = key;
}

// Auto-discover additional CEF panels from filesystem
function discoverCefPanels() {
  const cefDir = path.join(CONFIG.projectRoot, 'scriptfiles', 'cef');
  if (!fs.existsSync(cefDir)) return;

  const entries = fs.readdirSync(cefDir, { withFileTypes: true });
  for (const entry of entries) {
    if (!entry.isDirectory()) continue;
    const dirName = entry.name.toLowerCase();
    if (CEF_REGISTRY[dirName]) continue; // Already registered

    // Also check if this directory is already mapped by another key
    const dirPath = `scriptfiles/cef/${entry.name}/index.html`;
    const alreadyMapped = Object.values(CEF_REGISTRY).some(v => v.path === dirPath);
    if (alreadyMapped) continue;

    const indexPath = `scriptfiles/cef/${entry.name}/index.html`;
    const fullIndexPath = path.join(CONFIG.projectRoot, indexPath);
    if (fs.existsSync(fullIndexPath)) {
      // Find next available browser ID (start from 100)
      let bid = 100;
      const usedIds = new Set(Object.values(CEF_REGISTRY).map(i => i.browserId));
      while (usedIds.has(bid)) bid++;

      CEF_REGISTRY[dirName] = {
        browserId: bid,
        path: indexPath,
        name: entry.name.charAt(0).toUpperCase() + entry.name.slice(1),
      };
      CEF_PATH_MAP[indexPath.replace('/index.html', '')] = dirName;
    }
  }
}
discoverCefPanels();

// ===== JOB ENGINE =====
const JobType = {
  CEF_ONLY: 'cef_only',       // CEF file changes → Apply → Live reload
  PAWN_ONLY: 'pawn_only',     // Pawn file changes → Apply → Compile → GMX?
  MIXED: 'mixed',             // Both CEF + Pawn changes
  ANALYSIS: 'analysis',       // Read-only, no file changes
  COMPILE_ONLY: 'compile_only', // Just compile
};

class Job {
  constructor(playerid, type, changes, aiMessage) {
    this.id = `job-${Date.now()}-${Math.random().toString(36).slice(2, 6)}`;
    this.playerid = playerid;
    this.type = type;              // JobType
    this.changes = changes;        // { files: [...], message, requiresCompile, requiresCefReload, riskLevel }
    this.aiMessage = aiMessage;
    this.status = 'pending';       // pending → approved → applying → reloading/compiling → done/error
    this.affectedPanels = [];      // CEF panels that need reload
    this.created = new Date().toISOString();
    this.result = null;
  }

  // Determine which CEF panels are affected by file changes
  detectAffectedPanels() {
    this.affectedPanels = [];
    if (!this.changes?.files) return;

    for (const file of this.changes.files) {
      const fp = file.path.toLowerCase();

      // Check if file is in a CEF panel directory
      for (const [prefix, panelKey] of Object.entries(CEF_PATH_MAP)) {
        if (fp.startsWith(prefix) || fp.includes(`cef/${prefix.split('/').pop()}/`)) {
          if (!this.affectedPanels.includes(panelKey)) {
            this.affectedPanels.push(panelKey);
          }
        }
      }

      // Also check by directory name pattern
      const cefMatch = fp.match(/cef\/([^\/]+)\//i);
      if (cefMatch) {
        const panelName = cefMatch[1].toLowerCase();
        if (CEF_REGISTRY[panelName] && !this.affectedPanels.includes(panelName)) {
          this.affectedPanels.push(panelName);
        }
      }
    }

    return this.affectedPanels;
  }

  // Auto-detect job type from file changes
  detectJobType() {
    if (!this.changes?.files || this.changes.files.length === 0) {
      this.type = JobType.ANALYSIS;
      return;
    }

    let hasCef = false, hasPawn = false;
    for (const f of this.changes.files) {
      const ext = path.extname(f.path).toLowerCase();
      if (['.pwn', '.inc'].includes(ext)) hasPawn = true;
      if (['.html', '.css', '.js'].includes(ext) && f.path.toLowerCase().includes('cef/')) hasCef = true;
    }

    if (hasCef && hasPawn) this.type = JobType.MIXED;
    else if (hasCef) this.type = JobType.CEF_ONLY;
    else if (hasPawn) this.type = JobType.PAWN_ONLY;
    else this.type = JobType.CEF_ONLY; // Default
  }
}

// Active jobs
const jobs = new Map();

// ===== STATE =====
const sessions = new Map();
let operationLog = [];

// ===== EXPRESS =====
const app = express();
const server = http.createServer(app);
app.use(express.json({ limit: '10mb' }));

app.use((req, res, next) => {
  res.header('Access-Control-Allow-Origin', '*');
  res.header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS');
  res.header('Access-Control-Allow-Headers', 'Content-Type, Authorization, X-Admin-Key');
  if (req.method === 'OPTIONS') return res.sendStatus(200);
  next();
});

function authCheck(req, res, next) {
  const key = req.headers['x-admin-key'] || req.query.key || '';
  if (key !== CONFIG.adminPassword) return res.status(403).json({ error: 'Unauthorized' });
  next();
}

function safePath(requestedPath) {
  const resolved = path.resolve(CONFIG.projectRoot, requestedPath);
  if (!resolved.startsWith(CONFIG.projectRoot)) return null;
  return resolved;
}

function logOp(action, details, playerid) {
  const entry = { time: new Date().toISOString(), playerid: playerid || 0, action, details };
  operationLog.push(entry);
  if (operationLog.length > 1000) operationLog = operationLog.slice(-500);
  try {
    fs.appendFileSync(path.join(LOG_DIR, `ops-${new Date().toISOString().slice(0, 10)}.log`), JSON.stringify(entry) + '\n');
  } catch (e) {}
}

// ===== BACKUP =====
function createBackup(filePath, playerid) {
  const fullPath = safePath(filePath);
  if (!fullPath || !fs.existsSync(fullPath)) return null;
  const content = fs.readFileSync(fullPath, 'utf-8');
  const ts = new Date().toISOString().replace(/[:.]/g, '-');
  const bakName = `${path.basename(filePath)}.${ts}.bak`;
  fs.writeFileSync(path.join(BACKUP_DIR, bakName), content);
  fs.writeFileSync(path.join(BACKUP_DIR, bakName + '.meta'), JSON.stringify({ originalPath: filePath, ts, playerid, size: content.length }));
  // Cleanup
  const baks = fs.readdirSync(BACKUP_DIR).filter(f => f.endsWith('.bak')).sort();
  while (baks.length > CONFIG.maxBackups) {
    const old = baks.shift();
    fs.unlinkSync(path.join(BACKUP_DIR, old));
    try { fs.unlinkSync(path.join(BACKUP_DIR, old + '.meta')); } catch(e) {}
  }
  logOp('backup', { file: filePath, backup: bakName }, playerid);
  return bakName;
}

// ===== FILE OPS =====
function readFile(fp) {
  const full = safePath(fp);
  if (!full) return { error: 'Invalid path' };
  if (!fs.existsSync(full)) return { error: 'File not found' };
  try {
    const c = fs.readFileSync(full, 'utf-8');
    const s = fs.statSync(full);
    return { content: c, size: s.size, modified: s.mtime.toISOString() };
  } catch (e) { return { error: e.message }; }
}

function writeFile(fp, content, playerid) {
  const full = safePath(fp);
  if (!full) return { error: 'Path traversal blocked' };
  const blocked = [/node_modules/i, /\.env$/i, /package-lock/i];
  for (const p of blocked) { if (p.test(fp)) return { error: `Cannot modify: ${fp}` }; }
  if (fs.existsSync(full)) createBackup(fp, playerid);
  try {
    const dir = path.dirname(full);
    if (!fs.existsSync(dir)) fs.mkdirSync(dir, { recursive: true });
    fs.writeFileSync(full, content, 'utf-8');
    logOp('write', { file: fp, size: content.length }, playerid);
    return { success: true, path: fp };
  } catch (e) { return { error: e.message }; }
}

function listFiles(dirPath, depth = 0, maxDepth = 3) {
  const full = safePath(dirPath || '');
  if (!full) return [];
  const results = [];
  try {
    for (const entry of fs.readdirSync(full, { withFileTypes: true })) {
      if (entry.name.startsWith('.') || entry.name === 'node_modules' || entry.name === '.git') continue;
      const rel = path.join(dirPath || '', entry.name);
      if (entry.isDirectory()) {
        results.push({ name: entry.name, path: rel, type: 'dir' });
        if (depth < maxDepth) results.push(...listFiles(rel, depth + 1, maxDepth));
      } else {
        const ext = path.extname(entry.name).toLowerCase();
        if (['.pwn','.inc','.html','.css','.js','.json','.cfg','.txt','.xml','.md','.amx'].includes(ext) || entry.name === 'server.cfg') {
          const s = fs.statSync(path.join(full, entry.name));
          results.push({ name: entry.name, path: rel, type: 'file', size: s.size, ext });
        }
      }
    }
  } catch(e) {}
  return results;
}

// ===== Z.AI API (via z-ai-web-dev-sdk) =====
async function chatZAI(messages) {
  // Try SDK first (auto-auth)
  if (zaiSdk) {
    try {
      const response = await zaiSdk.chat.completions.create({
        messages,
        temperature: 0.7,
        max_tokens: 8192,
      });
      return response.choices?.[0]?.message?.content || '';
    } catch (e) {
      console.error('[UG-AI] SDK error:', e.message);
      // Fall through to raw API
    }
  }

  // Fallback: raw API with configured key
  if (!CONFIG.apiKey || CONFIG.apiKey === 'your-z-ai-api-key-here') {
    throw new Error('Z.AI SDK not available and API key not configured! Restart the backend or set ZAI_API_KEY in .env');
  }
  const url = `${CONFIG.baseUrl}/chat/completions`;
  const resp = await fetch(url, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json', 'Authorization': `Bearer ${CONFIG.apiKey}` },
    body: JSON.stringify({ model: CONFIG.model, messages, temperature: 0.7, max_tokens: 8192 }),
  });
  if (!resp.ok) throw new Error(`Z.AI API error ${resp.status}: ${await resp.text()}`);
  const data = await resp.json();
  return data.choices?.[0]?.message?.content || '';
}

// ===== AI SYSTEM PROMPT (enhanced with CEF registry) =====
function buildSystemPrompt(context) {
  // Build CEF registry info for AI
  let cefInfo = 'CEF PANELS ON THIS SERVER:\n';
  for (const [key, info] of Object.entries(CEF_REGISTRY)) {
    cefInfo += `  - "${key}" (Browser ID: ${info.browserId}, Path: ${info.path}, Name: ${info.name})\n`;
  }
  cefInfo += '\nWhen modifying CEF panel files, always include "requiresCefReload": true and specify which panel(s) in "affectedPanels" array.\n';

  return `You are UG-AI, an expert SA:MP/open.mp developer assistant integrated into the game server. You help the server owner develop and modify the server in real-time.

${cefInfo}
PROJECT CONTEXT:
- Server: open.mp (SA:MP compatible)
- Gamemode: fg-ogc.pwn
- CEF system: open.mp CEF module with multiple panels
- Custom models: AddSimpleModel/AddCharModel
- Pawn compiler: ${CONFIG.pawnCompiler}

CURRENT FILES CONTEXT:
${context || 'No files loaded yet. Use the Files tab to browse.'}

YOUR CAPABILITIES:
- Read project files (.pwn, .inc, .html, .css, .js, .json, .cfg)
- Analyze server logs and compiler errors
- Write/modify Pawn code and CEF panels
- Create new CEF HTML panels
- Fix bugs and suggest improvements
- Generate complete systems

MODULAR JOB SYSTEM:
When you modify files, you MUST respond with structured JSON so the job engine can process changes correctly:

\`\`\`json
{
  "message": "Human-readable explanation",
  "files": [
    {
      "path": "relative/path/to/file",
      "action": "create|modify",
      "content": "COMPLETE file content here"
    }
  ],
  "requiresCompile": true/false,
  "requiresCefReload": true/false,
  "affectedPanels": ["panel_key1", "panel_key2"],
  "requiresGmx": true/false,
  "riskLevel": "low|medium|high"
}
\`\`\`

JOB TYPE DETECTION (automatic):
- If only CEF files (HTML/CSS/JS in cef/ folder) → CEF_ONLY job: Apply → Instant live reload, no restart
- If only Pawn files (.pwn/.inc) → PAWN_ONLY job: Apply → Compile → GMX if needed
- If both → MIXED job: Apply → Reload CEF → Compile → GMX if needed
- If no files → ANALYSIS: Just explanation, no changes

IMPORTANT RULES:
- ALWAYS include COMPLETE file contents (no "..." placeholders)
- For CEF panels: include full HTML with inline CSS/JS, no external file references
- For Pawn: include all necessary includes and forwards
- Set requiresCefReload=true when modifying ANY file in scriptfiles/cef/
- Set affectedPanels to the panel keys (e.g., ["tablet", "dog"]) so the system knows which browsers to reload
- Set requiresGmx=true only for critical gamemode changes that need server restart
- Set riskLevel: "low" for CSS/design, "medium" for JS/logic, "high" for Pawn/gamemode core
- Use Bosnian/Croatian/Serbian when the user writes in that language
- NEVER delete critical files without explicit confirmation
- When user says "izmijeni tablet dizajn", find the tablet CEF files and modify them`;
}

function buildFileContext(requestedFiles) {
  let ctx = '';
  const files = requestedFiles || [];
  // Only include files that were explicitly requested - don't auto-load the huge gamemode
  if (files.length === 0) return ctx;
  const allFiles = [...new Set([...files])];
  for (const f of allFiles) {
    const r = readFile(f);
    if (r.content) {
      // Limit each file to 8000 chars to avoid memory issues
      const t = r.content.length > 8000 ? r.content.slice(0, 8000) + '\n... [truncated, use Files tab to see full content]' : r.content;
      ctx += `\n--- FILE: ${f} ---\n${t}\n--- END ---\n`;
    }
  }
  return ctx;
}

// ===== COMPILE =====
function compilePawn(playerid) {
  try {
    logOp('compile_start', { file: CONFIG.gamemodePath }, playerid);
    const cmd = `cd "${CONFIG.projectRoot}" && ${CONFIG.pawnCompiler} -iopen.mp/qawno/include ${CONFIG.gamemodePath} 2>&1`;
    const output = execSync(cmd, { timeout: 60000, encoding: 'utf-8' });
    const hasErrors = output.includes('error') && !output.includes('0 errors');
    logOp('compile_end', { success: !hasErrors }, playerid);
    return { success: !hasErrors, output: output.slice(-2000) };
  } catch (e) {
    const output = (e.stdout || e.message || '').slice(-2000);
    logOp('compile_error', {}, playerid);
    return { success: false, output };
  }
}

// ===== GMX / RESTART =====
function sendRconCommand(cmd) {
  if (!CONFIG.rconPassword) {
    logOp('rcon_skip', { reason: 'No RCON password configured' });
    return false;
  }
  try {
    // Use open.mp HTTP API for RCON if available
    const url = `http://${CONFIG.rconHost}:${CONFIG.rconPort + 1000}/rcon`;
    // open.mp uses port+1000 for HTTP by default
    execSync(`curl -s -X POST "${url}" -H "Content-Type: application/json" -d '{"command":"${cmd}","password":"${CONFIG.rconPassword}"}' 2>&1`, { timeout: 5000 });
    logOp('rcon_sent', { command: cmd });
    return true;
  } catch (e) {
    // Fallback: write command file for Pawn to read
    try {
      const cmdDir = path.join(CONFIG.projectRoot, 'scriptfiles', 'ai-cmds');
      if (!fs.existsSync(cmdDir)) fs.mkdirSync(cmdDir, { recursive: true });
      fs.writeFileSync(path.join(cmdDir, `cmd-${Date.now()}.txt`), cmd);
      logOp('rcon_file', { command: cmd });
      return true;
    } catch (e2) {
      logOp('rcon_fail', { error: e2.message });
      return false;
    }
  }
}

function executeGmx() {
  logOp('gmx', {});
  return sendRconCommand('gmx');
}

// ===== SESSION =====
function getSession(pid) {
  if (!sessions.has(pid)) sessions.set(pid, { messages: [], currentJob: null, status: 'idle' });
  return sessions.get(pid);
}

// ===== WEBSOCKET =====
const wss = new WebSocketServer({ server, path: '/ws' });
const wsClients = new Set();

function broadcastWS(data) {
  const msg = JSON.stringify(data);
  wss.clients.forEach(c => { if (c.readyState === 1) c.send(msg); });
}

wss.on('connection', (ws, req) => {
  const url = new URL(req.url, `http://${req.headers.host}`);
  if (url.searchParams.get('key') !== CONFIG.adminPassword) { ws.close(4001, 'Unauthorized'); return; }
  wsClients.add(ws);
  ws.on('close', () => wsClients.delete(ws));
  ws.on('message', async (raw) => {
    try {
      const d = JSON.parse(raw);
      if (d.type === 'ping') ws.send(JSON.stringify({ type: 'pong' }));
    } catch(e) {}
  });
});

// ===== API ROUTES =====

// Serve AI Dev Panel in browser (for remote/home use)
app.get('/', (req, res) => {
  const panelPath = path.join(CONFIG.projectRoot, 'scriptfiles', 'cef', 'ai-dev-panel', 'index.html');
  if (fs.existsSync(panelPath)) {
    res.sendFile(panelPath);
  } else {
    res.json({ status: 'ok', model: CONFIG.model, info: 'AI Dev Panel HTML not found. Access via /api/ endpoints.' });
  }
});

app.get('/api/health', (req, res) => {
  res.json({ status: 'ok', model: CONFIG.model, projectRoot: CONFIG.projectRoot, apiKeyConfigured: !!CONFIG.apiKey && CONFIG.apiKey !== 'your-z-ai-api-key-here', sdkAvailable: !!zaiSdk });
});

// CEF Registry
app.get('/api/cef-registry', authCheck, (req, res) => {
  res.json({ panels: CEF_REGISTRY });
});

// Chat with AI
app.post('/api/chat', authCheck, async (req, res) => {
  const { message, playerid, context_files } = req.body;
  if (!message) return res.status(400).json({ error: 'Message required' });

  // Auto-retry SDK init if not available yet
  if (!zaiSdk) await initZAISdk();

  const session = getSession(playerid);
  session.status = 'generating';

  try {
    const fileContext = buildFileContext(context_files);
    const systemPrompt = buildSystemPrompt(fileContext);
    const messages = [
      { role: 'system', content: systemPrompt },
      ...session.messages.slice(-20),
      { role: 'user', content: message },
    ];

    const aiResponse = await chatZAI(messages);
    session.messages.push({ role: 'user', content: message });
    session.messages.push({ role: 'assistant', content: aiResponse });

    // Parse JSON response
    let parsedResponse = null;
    const jsonMatch = aiResponse.match(/```json\s*\n?([\s\S]*?)\n?```/);
    if (jsonMatch) {
      try {
        parsedResponse = JSON.parse(jsonMatch[1]);
        // Create a job
        const job = new Job(playerid, JobType.ANALYSIS, parsedResponse, aiResponse);
        job.detectAffectedPanels();
        job.detectJobType();
        parsedResponse.affectedPanels = job.affectedPanels;
        parsedResponse.jobType = job.type;
        session.currentJob = job;
        session.status = 'waiting_approval';
      } catch (e) { session.status = 'idle'; }
    } else {
      session.status = 'idle';
    }

    logOp('chat', { playerid, msgLen: message.length }, playerid);
    res.json({ response: aiResponse, parsed: parsedResponse, status: session.status });
  } catch (e) {
    session.status = 'error';
    logOp('chat_error', { playerid, error: e.message }, playerid);
    res.status(500).json({ error: e.message, status: 'error' });
  }
});

// Preview
app.post('/api/preview', authCheck, (req, res) => {
  const { playerid } = req.body;
  const session = getSession(playerid);
  const job = session.currentJob;
  if (!job?.changes?.files) return res.status(400).json({ error: 'No pending changes' });

  const diffs = [];
  for (const file of job.changes.files) {
    const existing = readFile(file.path);
    const oldContent = existing.content || '';
    diffs.push({
      path: file.path,
      action: file.action,
      diff: Diff.createPatch(file.path, oldContent, file.content, 'original', 'modified'),
      riskLevel: job.changes.riskLevel || 'low',
    });
  }

  res.json({
    message: job.changes.message,
    diffs,
    jobType: job.type,
    affectedPanels: job.affectedPanels,
    requiresCompile: job.changes.requiresCompile || false,
    requiresCefReload: job.changes.requiresCefReload || job.affectedPanels.length > 0,
    requiresGmx: job.changes.requiresGmx || false,
    riskLevel: job.changes.riskLevel || 'low',
  });
});

// ===== APPLY - Core of the modular job engine =====
app.post('/api/apply', authCheck, async (req, res) => {
  const { playerid, forceGmx } = req.body;
  const session = getSession(playerid);
  const job = session.currentJob;
  if (!job?.changes?.files) return res.status(400).json({ error: 'No pending changes' });

  job.status = 'applying';
  const results = [];

  // Step 1: Write all files
  for (const file of job.changes.files) {
    const result = writeFile(file.path, file.content, playerid);
    results.push({ path: file.path, ...result });
  }

  // Save patch
  fs.writeFileSync(path.join(PATCH_DIR, `patch-${Date.now()}.json`), JSON.stringify(job.changes, null, 2));

  const jobResult = { applied: true, results, cefReloaded: [], compileResult: null, gmxSent: false, rolledBack: false };

  // Step 2: CEF RELOAD (instant, no restart needed)
  if (job.affectedPanels.length > 0) {
    for (const panelKey of job.affectedPanels) {
      const panelInfo = CEF_REGISTRY[panelKey];
      if (panelInfo) {
        // Send WebSocket signal to AI Dev Panel CEF to trigger reload
        broadcastWS({
          type: 'reload_cef_panel',
          panelKey,
          browserId: panelInfo.browserId,
          panelName: panelInfo.name,
          playerid,
        });
        jobResult.cefReloaded.push({ key: panelKey, name: panelInfo.name, browserId: panelInfo.browserId });
        logOp('cef_reload', { panel: panelKey, browserId: panelInfo.browserId }, playerid);
      }
    }
  }

  // Step 3: COMPILE (if Pawn files were modified)
  if (job.changes.requiresCompile || job.type === JobType.PAWN_ONLY || job.type === JobType.MIXED) {
    job.status = 'compiling';
    const compileResult = compilePawn(playerid);
    jobResult.compileResult = compileResult;

    if (!compileResult.success) {
      // ROLLBACK!
      logOp('compile_failed_rollback', { playerid }, playerid);
      for (const file of job.changes.files) {
        if (path.extname(file.path).toLowerCase() === '.amx') continue; // Don't rollback .amx
        const baks = fs.readdirSync(BACKUP_DIR).filter(f => f.startsWith(path.basename(file.path)) && f.endsWith('.bak')).sort();
        if (baks.length > 0) {
          const content = fs.readFileSync(path.join(BACKUP_DIR, baks[baks.length - 1]), 'utf-8');
          const fp = safePath(file.path);
          if (fp) fs.writeFileSync(fp, content, 'utf-8');
        }
      }
      jobResult.applied = false;
      jobResult.rolledBack = true;
      jobResult.message = 'Compile failed! Changes rolled back.';
      job.status = 'error';
      session.currentJob = null;
      session.status = 'error';
      return res.json(jobResult);
    }
  }

  // Step 4: GMX (if required or forced)
  if (job.changes.requiresGmx || forceGmx) {
    jobResult.gmxSent = executeGmx();
  }

  job.status = 'done';
  session.currentJob = null;
  session.status = 'applied';
  logOp('apply', { playerid, fileCount: results.length, jobType: job.type, cefReload: jobResult.cefReloaded.length }, playerid);

  res.json({
    ...jobResult,
    message: buildResultMessage(jobResult),
    status: 'applied',
  });
});

function buildResultMessage(r) {
  let msg = 'Changes applied!';
  if (r.cefReloaded.length > 0) msg += ` CEF reloaded: ${r.cefReloaded.map(p => p.name).join(', ')}`;
  if (r.compileResult?.success) msg += ' Compile passed!';
  if (r.gmxSent) msg += ' GMX sent!';
  if (r.rolledBack) msg = 'Compile failed, rolled back.';
  return msg;
}

// Reject
app.post('/api/reject', authCheck, (req, res) => {
  const { playerid } = req.body;
  const session = getSession(playerid);
  session.currentJob = null;
  session.status = 'idle';
  res.json({ success: true });
});

// Revert
app.post('/api/revert', authCheck, (req, res) => {
  const { playerid, filepath } = req.body;
  if (filepath) {
    const baks = fs.readdirSync(BACKUP_DIR).filter(f => f.startsWith(path.basename(filepath)) && f.endsWith('.bak')).sort();
    if (!baks.length) return res.status(404).json({ error: 'No backup found' });
    const content = fs.readFileSync(path.join(BACKUP_DIR, baks[baks.length - 1]), 'utf-8');
    const fp = safePath(filepath);
    if (!fp) return res.status(400).json({ error: 'Invalid path' });
    fs.writeFileSync(fp, content, 'utf-8');
    logOp('revert', { file: filepath }, playerid);
    return res.json({ success: true, message: `Reverted ${filepath}` });
  }
  // Revert last patch
  const patches = fs.readdirSync(PATCH_DIR).filter(f => f.endsWith('.json')).sort();
  if (!patches.length) return res.status(404).json({ error: 'No patches' });
  const patchData = JSON.parse(fs.readFileSync(path.join(PATCH_DIR, patches[patches.length - 1]), 'utf-8'));
  for (const f of patchData.files || []) {
    const baks = fs.readdirSync(BACKUP_DIR).filter(b => b.startsWith(path.basename(f.path)) && b.endsWith('.bak')).sort();
    if (baks.length) {
      const content = fs.readFileSync(path.join(BACKUP_DIR, baks[baks.length - 1]), 'utf-8');
      const fp = safePath(f.path);
      if (fp) fs.writeFileSync(fp, content, 'utf-8');
    }
  }
  fs.unlinkSync(path.join(PATCH_DIR, patches[patches.length - 1]));
  logOp('revert_patch', {}, playerid);
  res.json({ success: true });
});

// Files
app.get('/api/files', authCheck, (req, res) => {
  res.json({ files: listFiles(req.query.dir || ''), root: CONFIG.projectRoot });
});
app.get('/api/files/*', authCheck, (req, res) => {
  const r = readFile(req.params[0]);
  if (r.error) return res.status(404).json(r);
  res.json(r);
});

// Logs
app.get('/api/logs', authCheck, (req, res) => {
  res.json({ logs: operationLog.slice(-(parseInt(req.query.limit) || 100)) });
});

// Analyze error
app.post('/api/analyze-error', authCheck, async (req, res) => {
  const { error_text, playerid } = req.body;
  try {
    const slog = readFile('server_log.txt');
    let ctx = `Error:\n${error_text}`;
    if (slog.content) ctx += `\n\nServer log (last 50 lines):\n${slog.content.split('\n').slice(-50).join('\n')}`;
    const analysis = await chatZAI([
      { role: 'system', content: 'You are an expert SA:MP/open.mp debugger. Analyze the error and provide a fix. Use Bosnian/Croatian/Serbian if the user writes in that language.' },
      { role: 'user', content: ctx },
    ]);
    res.json({ analysis });
  } catch (e) { res.status(500).json({ error: e.message }); }
});

// Compile only
app.post('/api/compile', authCheck, (req, res) => {
  res.json(compilePawn(req.body.playerid));
});

// Reload specific CEF panel
app.post('/api/reload-cef', authCheck, (req, res) => {
  const { panelKey, playerid } = req.body;
  const panelInfo = CEF_REGISTRY[panelKey];
  if (!panelInfo) return res.status(404).json({ error: 'Panel not found in registry' });
  broadcastWS({ type: 'reload_cef_panel', panelKey, browserId: panelInfo.browserId, panelName: panelInfo.name, playerid });
  logOp('cef_reload', { panel: panelKey }, playerid);
  res.json({ success: true });
});

// GMX
app.post('/api/gmx', authCheck, (req, res) => {
  const sent = executeGmx();
  res.json({ success: sent, message: sent ? 'GMX command sent' : 'RCON not configured, wrote command file' });
});

// Session
app.get('/api/session/:playerid', authCheck, (req, res) => {
  const s = getSession(parseInt(req.params.playerid));
  res.json({
    status: s.status,
    currentJob: s.currentJob ? {
      id: s.currentJob.id,
      type: s.currentJob.type,
      status: s.currentJob.status,
      affectedPanels: s.currentJob.affectedPanels,
      fileCount: s.currentJob.changes?.files?.length || 0,
      requiresCompile: s.currentJob.changes?.requiresCompile,
      requiresCefReload: s.currentJob.changes?.requiresCefReload || s.currentJob.affectedPanels?.length > 0,
      requiresGmx: s.currentJob.changes?.requiresGmx,
      riskLevel: s.currentJob.changes?.riskLevel,
    } : null,
    messageCount: s.messages.length,
  });
});
app.post('/api/session/clear', authCheck, (req, res) => {
  sessions.delete(req.body.playerid);
  res.json({ success: true });
});

// ===== START =====
server.listen(CONFIG.port, '0.0.0.0', () => {
  console.log(`[UG-AI v3] Backend running on http://127.0.0.1:${CONFIG.port}`);
  console.log(`[UG-AI v3] Model: ${CONFIG.model} | SDK: ${zaiSdk ? 'auto-auth OK' : 'waiting...'} | API Key: ${CONFIG.apiKey && CONFIG.apiKey !== 'your-z-ai-api-key-here' ? 'configured' : 'not set (using SDK)'}`);
  console.log(`[UG-AI v3] Project: ${CONFIG.projectRoot}`);
  console.log(`[UG-AI v3] CEF Registry: ${Object.keys(CEF_REGISTRY).length} panels`);
  for (const [k, v] of Object.entries(CEF_REGISTRY)) {
    console.log(`[UG-AI v3]   ${k}: Browser ${v.browserId} → ${v.path}`);
  }
  console.log(`[UG-AI v3] RCON: ${CONFIG.rconPassword ? 'configured' : 'not set (GMX will use file-based)'}`);
});
