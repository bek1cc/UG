const { app, BrowserWindow, ipcMain, shell, dialog } = require('electron');
const path = require('path');
const fs = require('fs');
const https = require('https');
const http = require('http');
const { exec, execFile, spawn } = require('child_process');
const dgram = require('dgram');
const AdmZip = require('adm-zip');

// ============================================================
//  FAST STARTUP OPTIMIZATIONS (MUST BE BEFORE app.whenReady)
// ============================================================
app.commandLine.appendSwitch('no-sandbox');
app.commandLine.appendSwitch('disable-gpu-sandbox');
// DO NOT disable hardware acceleration - it makes rendering 3x faster
// app.disableHardwareAcceleration();  // REMOVED for speed
app.commandLine.appendSwitch('enable-gpu-rasterization');
app.commandLine.appendSwitch('enable-zero-copy');
app.commandLine.appendSwitch('ignore-gpu-blocklist');

// ============================================================
//  ASYNC LOG (non-blocking) - sync writes slow down startup
// ============================================================
const LOG_FILE = path.join(path.dirname(app.getPath('exe')), 'launcher_debug.log');
let logBuffer = [];
let logFlushing = false;

function log(msg) {
  const ts = new Date().toISOString();
  const line = `[${ts}] ${msg}\n`;
  console.log(line.trim());
  logBuffer.push(line);
  flushLog();
}

function flushLog() {
  if (logFlushing || logBuffer.length === 0) return;
  logFlushing = true;
  const data = logBuffer.join('');
  logBuffer = [];
  fs.writeFile(LOG_FILE, data, { flag: 'a' }, (err) => {
    logFlushing = false;
    if (logBuffer.length > 0) flushLog();
  });
}

// Clear old log (async)
try { fs.writeFileSync(LOG_FILE, '=== Unicate Gaming Launcher v3.5 ===\n'); } catch(e) {}

log('Launcher v3.5 starting...');

// ============================================================
//  CRASH PROTECTION
// ============================================================
process.on('uncaughtException', (err) => { log('[UNCAUGHT] ' + err.message); });
process.on('unhandledRejection', (err) => { log('[UNHANDLED] ' + err); });

// ============================================================
//  CONFIG
// ============================================================
const SERVERS = {
  production: { ip: '135.125.156.197', port: 7777, name: 'Unicate Gaming RPG' },
  local: { ip: '127.0.0.1', port: 7777, name: 'Unicate Gaming TEST' }
};

function getActiveServer() {
  const settings = loadSettings();
  const mode = settings.server_mode || 'production';
  return { ...SERVERS[mode], mode };
}

const SERVER_NAME = 'Unicate Gaming RPG';
const WEBSITE_URL = 'https://ug-ogc.com';
const DISCORD_URL = 'https://discord.gg/unicategaming';
const LAUNCHER_VERSION = '3.5.0';

const OMP_CEF_ASI_URL = 'https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/cef.asi';
const OMP_CEF_CLIENT_URL = 'https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/client-files-v1.2.0.zip';
const ASI_LOADER_URL = 'https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/v9.7.2/Ultimate-ASI-Loader.zip';
const SAMP_CLIENT_URL = 'https://files.sa-mp.com/sa-mp-0.3.7-R4-install.exe';

const LAUNCHER_DIR = path.dirname(app.getPath('exe'));
const SETTINGS_FILE = path.join(LAUNCHER_DIR, 'settings.json');

let mainWindow = null;

// ============================================================
//  SETTINGS (cached)
// ============================================================
let _settingsCache = null;
function loadSettings() {
  if (_settingsCache) return _settingsCache;
  try {
    if (fs.existsSync(SETTINGS_FILE)) {
      _settingsCache = JSON.parse(fs.readFileSync(SETTINGS_FILE, 'utf8'));
      return _settingsCache;
    }
  } catch (e) { log('Settings error: ' + e.message); }
  _settingsCache = {};
  return _settingsCache;
}

function saveSettings(data) {
  _settingsCache = data;
  try { fs.writeFileSync(SETTINGS_FILE, JSON.stringify(data, null, 2)); } catch (e) {}
}

// ============================================================
//  GTA:SA PATH DETECTION (cached)
// ============================================================
let _gtaPathCache = null;
function findGtaPath() {
  if (_gtaPathCache !== null) return _gtaPathCache;
  const settings = loadSettings();
  if (settings.gta_path && fs.existsSync(path.join(settings.gta_path, 'gta_sa.exe'))) {
    _gtaPathCache = settings.gta_path;
    return _gtaPathCache;
  }
  const common = [
    'C:\\Program Files (x86)\\Rockstar Games\\GTA San Andreas',
    'D:\\GTA San Andreas', 'D:\\GTA SA', 'D:\\Games\\GTA SA',
    'D:\\SAMP TEST SERVER',
    'E:\\GTA San Andreas', 'E:\\GTA SA',
    'C:\\GTA San Andreas', 'C:\\GTA SA'
  ];
  for (const p of common) {
    if (fs.existsSync(path.join(p, 'gta_sa.exe'))) {
      _gtaPathCache = p;
      return p;
    }
  }
  _gtaPathCache = null;
  return null;
}

// ============================================================
//  STATUS CHECK (fast - uses cached path)
// ============================================================
function getStatus(gtaPath) {
  const settings = loadSettings();
  const mode = settings.server_mode || 'production';
  const isLocal = mode === 'local';

  const s = {
    gta_path: gtaPath,
    has_samp: false,
    cef_ok: false,
    cef_msg: '-',
    has_asi: false,
    ready: false,
    missing: [],
    server_mode: mode
  };
  if (gtaPath && fs.existsSync(gtaPath)) {
    s.has_samp = fs.existsSync(path.join(gtaPath, 'samp.exe'));
    const hasAsi = fs.existsSync(path.join(gtaPath, 'cef.asi'));
    const hasCef = fs.existsSync(path.join(gtaPath, 'cef'));
    s.has_asi = fs.existsSync(path.join(gtaPath, 'dsound.dll')) || fs.existsSync(path.join(gtaPath, 'dinput8.dll'));
    if (hasAsi && hasCef) { s.cef_ok = true; s.cef_msg = 'CEF OK'; }
    else if (hasAsi) { s.cef_msg = 'Fali cef/ folder'; }
    else if (hasCef) { s.cef_msg = 'Fali cef.asi'; }
    else { s.cef_msg = 'Nije instaliran'; }
    if (!s.has_samp) s.missing.push('client');
    if (!isLocal) {
      if (!hasAsi) s.missing.push('cef_asi');
      if (!hasCef) s.missing.push('cef_runtime');
      if (!s.has_asi) s.missing.push('asi_loader');
      s.ready = s.has_samp && s.cef_ok && s.has_asi;
    } else {
      s.ready = s.has_samp;
    }
  }
  return s;
}

// ============================================================
//  SAMP SERVER QUERY (fixed parser + faster timeout)
// ============================================================
function querySampServer(ip, port) {
  return new Promise((resolve) => {
    try {
      const sock = dgram.createSocket('udp4');
      const timeout = setTimeout(() => { try { sock.close(); } catch(e) {} resolve({ online: false }); }, 3000);
      const ipParts = ip.split('.').map(Number);
      const pkt = Buffer.concat([
        Buffer.from('SAMP'),
        Buffer.from([ipParts[0], ipParts[1], ipParts[2], ipParts[3]]),
        Buffer.from([port & 0xFF, (port >> 8) & 0xFF]),
        Buffer.from('i')
      ]);
      sock.send(pkt, port, ip, (err) => {
        if (err) { clearTimeout(timeout); try { sock.close(); } catch(e) {} resolve({ online: false }); }
      });
      sock.on('message', (data) => {
        clearTimeout(timeout);
        try { sock.close(); } catch(e) {}
        try {
          // SA-MP 'i' (info) response format:
          // Header: SAMP(4) + IP(4) + Port(2) + 'i'(1) = 11 bytes
          // Then: IsPassworded(1) + Players(2) + MaxPlayers(2) + NameLen(4) + Name + ModeLen(4) + Mode + MapLen(4) + Map
          if (data.length < 11) { resolve({ online: false }); return; }
          let off = 11;

          // IsPassworded: 1 byte (uint8) - NOT 2 bytes!
          if (off + 1 > data.length) { resolve({ online: false }); return; }
          const isPassworded = data.readUInt8(off); off += 1;

          // Players: 2 bytes (uint16 LE)
          if (off + 2 > data.length) { resolve({ online: false }); return; }
          const players = data.readUInt16LE(off); off += 2;

          // Max Players: 2 bytes (uint16 LE)
          if (off + 2 > data.length) { resolve({ online: false }); return; }
          const maxp = data.readUInt16LE(off); off += 2;

          // Server Name: length(4) + string
          if (off + 4 > data.length) { resolve({ online: false }); return; }
          const nlen = data.readUInt32LE(off); off += 4;
          if (off + nlen > data.length) { resolve({ online: false }); return; }
          const name = data.slice(off, off + nlen).toString('latin1'); off += nlen;

          // Game Mode: length(4) + string
          let mode = 'RPG';
          if (off + 4 <= data.length) {
            const mlen = data.readUInt32LE(off); off += 4;
            if (off + mlen <= data.length) {
              mode = data.slice(off, off + mlen).toString('latin1'); off += mlen;
            }
          }

          log('Query OK: ' + players + '/' + maxp + ' name=' + name + ' mode=' + mode);
          resolve({ players, max_players: maxp, name, gamemode: mode, online: true, password: isPassworded === 1 });
        } catch (parseErr) {
          log('Query parse error: ' + parseErr.message);
          // Server responded but we couldn't parse - still online
          resolve({ players: 0, max_players: 200, name: 'SA-MP Server', gamemode: 'RPG', online: true });
        }
      });
      sock.on('error', () => { clearTimeout(timeout); try { sock.close(); } catch(e) {} resolve({ online: false }); });
    } catch (e) {
      resolve({ online: false });
    }
  });
}

// ============================================================
//  DOWNLOAD WITH PROGRESS
// ============================================================
function downloadFile(url, dest, onProgress, maxRedirects = 10) {
  return new Promise((resolve, reject) => {
    if (maxRedirects <= 0) { reject(new Error('Too many redirects')); return; }

    const mod = url.startsWith('https') ? https : http;
    let totalBytes = 0;
    let downloadedBytes = 0;
    let startTime = Date.now();

    mod.get(url, { timeout: 60000, headers: { 'User-Agent': 'UnicateGamingLauncher/3.3' } }, (response) => {
      if (response.statusCode >= 300 && response.statusCode < 400 && response.headers.location) {
        const newUrl = response.headers.location;
        response.resume();
        return downloadFile(newUrl, dest, onProgress, maxRedirects - 1).then(resolve).catch(reject);
      }

      if (response.statusCode !== 200) {
        response.resume();
        return reject(new Error(`HTTP ${response.statusCode} for ${url}`));
      }

      const file = fs.createWriteStream(dest);
      totalBytes = parseInt(response.headers['content-length'] || '0', 10);

      response.pipe(file);
      response.on('data', (chunk) => {
        downloadedBytes += chunk.length;
        const pct = totalBytes > 0 ? (downloadedBytes / totalBytes) * 100 : 0;
        const elapsed = (Date.now() - startTime) / 1000;
        const speed = elapsed > 0 ? (downloadedBytes / 1024) / elapsed : 0;
        if (onProgress) onProgress(pct, downloadedBytes, totalBytes, speed);
      });

      file.on('finish', () => {
        file.close();
        if (!fs.existsSync(dest) || fs.statSync(dest).size === 0) {
          try { fs.unlinkSync(dest); } catch (e) {}
          return reject(new Error('Downloaded file is empty'));
        }
        resolve(dest);
      });

      file.on('error', (err) => {
        file.close();
        try { fs.unlinkSync(dest); } catch (e) {}
        reject(err);
      });
    }).on('error', (err) => {
      reject(err);
    });
  });
}

// ============================================================
//  AUTO-INSTALL
// ============================================================
async function autoInstall(gtaPath, missing) {
  for (const comp of missing) {
    if (comp === 'client') {
      const tmpExe = path.join(LAUNCHER_DIR, 'tmp_samp_install.exe');
      log('Downloading SA-MP R4 client...');
      await downloadFile(SAMP_CLIENT_URL, tmpExe, (pct, dl, total, speed) => {
        if (mainWindow) mainWindow.webContents.send('install-progress', {
          component: 'SA-MP R4 Client', pct, downloaded: dl, total, speed
        });
      });
      try {
        const { execSync } = require('child_process');
        execSync(`"${tmpExe}" /S /D=${gtaPath}`, { timeout: 60000, windowsHide: true });
        log('SA-MP R4 installed silently');
      } catch (e) {
        log('Silent install failed, launching installer: ' + e.message);
        const child = spawn(tmpExe, [], { detached: true, stdio: 'ignore' });
        child.unref();
        await new Promise(resolve => setTimeout(resolve, 5000));
      }
      try { fs.unlinkSync(tmpExe); } catch (e) {}
    }
    else if (comp === 'asi_loader') {
      const tmpZip = path.join(LAUNCHER_DIR, 'tmp_asi.zip');
      await downloadFile(ASI_LOADER_URL, tmpZip, (pct, dl, total, speed) => {
        if (mainWindow) mainWindow.webContents.send('install-progress', {
          component: 'ASI Loader', pct, downloaded: dl, total, speed
        });
      });
      const zip = new AdmZip(tmpZip);
      const entries = zip.getEntries();
      for (const entry of entries) {
        const fileName = entry.entryName.split('/').pop();
        if (fileName.toLowerCase().endsWith('.dll') && !entry.isDirectory) {
          fs.writeFileSync(path.join(gtaPath, fileName), entry.getData());
        }
      }
      try { fs.unlinkSync(tmpZip); } catch (e) {}
    }
    else if (comp === 'cef_asi') {
      await downloadFile(OMP_CEF_ASI_URL, path.join(gtaPath, 'cef.asi'), (pct, dl, total, speed) => {
        if (mainWindow) mainWindow.webContents.send('install-progress', {
          component: 'CEF Plugin', pct, downloaded: dl, total, speed
        });
      });
    }
    else if (comp === 'cef_runtime') {
      const tmpZip = path.join(LAUNCHER_DIR, 'tmp_cef.zip');
      await downloadFile(OMP_CEF_CLIENT_URL, tmpZip, (pct, dl, total, speed) => {
        if (mainWindow) mainWindow.webContents.send('install-progress', {
          component: 'CEF Runtime', pct, downloaded: dl, total, speed
        });
      });
      const zip = new AdmZip(tmpZip);
      zip.extractAllTo(gtaPath, true);
      try { fs.unlinkSync(tmpZip); } catch (e) {}
    }
  }
  if (mainWindow) mainWindow.webContents.send('install-complete', {});
}

// ============================================================
//  SA-MP REGISTRY SETUP
// ============================================================
function setupSampRegistry(gtaPath, ip, port, nickname) {
  try {
    const { execSync } = require('child_process');
    execSync(`reg add "HKCU\\Software\\SAMP" /v "PlayerName" /t REG_SZ /d "${nickname}" /f`, { windowsHide: true });
    execSync(`reg add "HKCU\\Software\\SAMP" /v "LastServer" /t REG_SZ /d "${ip}:${port}" /f`, { windowsHide: true });
    execSync(`reg add "HKCU\\Software\\SAMP" /v "gta_sa_exe" /t REG_SZ /d "${gtaPath}\\gta_sa.exe" /f`, { windowsHide: true });
    log('Registry setup OK');
    return true;
  } catch (err) {
    log('Registry error: ' + err.message);
    return false;
  }
}

// ============================================================
//  CREATE WINDOW (optimized - show immediately, load async)
// ============================================================
function createWindow() {
  try {
    const srcDir = __dirname;
    const indexPath = path.join(srcDir, 'index.html');
    const preloadPath = path.join(srcDir, 'preload.js');
    const iconIco = path.join(srcDir, 'ug_icon.ico');
    const iconPng = path.join(srcDir, 'ug_logo.png');
    let iconPath = null;
    if (fs.existsSync(iconIco)) iconPath = iconIco;
    else if (fs.existsSync(iconPng)) iconPath = iconPng;

    mainWindow = new BrowserWindow({
      width: 1280,
      height: 800,
      minWidth: 1100,
      minHeight: 700,
      frame: false,
      transparent: false,
      resizable: true,
      title: 'Unicate Gaming - Launcher',
      backgroundColor: '#0b0f1a',
      icon: iconPath || undefined,
      show: false,  // Don't show until ready - prevents white flash
      webPreferences: {
        nodeIntegration: false,
        contextIsolation: true,
        preload: preloadPath,
        sandbox: false,
        backgroundThrottling: false  // Keep fast even when minimized
      }
    });

    // Show window only when content is ready - eliminates white flash
    mainWindow.once('ready-to-show', () => {
      mainWindow.show();
      log('Window shown (ready-to-show)');
    });

    mainWindow.loadFile(indexPath);

    mainWindow.webContents.on('render-process-gone', (event, details) => {
      log('Renderer GONE! reason: ' + details.reason);
    });

    mainWindow.on('closed', () => {
      mainWindow = null;
    });

    log('Window created');
  } catch (err) {
    log('ERROR creating window: ' + err.message);
  }
}

// ============================================================
//  IPC HANDLERS
// ============================================================
ipcMain.handle('get-version', () => LAUNCHER_VERSION);

ipcMain.handle('get-server-info', async () => {
  const srv = getActiveServer();
  const result = await querySampServer(srv.ip, srv.port);
  
  // TCP fallback for localhost (only if UDP query failed)
  if (!result.online && srv.mode === 'local') {
    const tcpCheck = await new Promise((resolve) => {
      try {
        const net = require('net');
        const sock = net.createConnection(srv.port, srv.ip);
        sock.setTimeout(2000);
        sock.on('connect', () => { sock.destroy(); resolve(true); });
        sock.on('error', () => { sock.destroy(); resolve(false); });
        sock.on('timeout', () => { sock.destroy(); resolve(false); });
      } catch(e) { resolve(false); }
    });
    if (tcpCheck) {
      return { online: true, players: 0, max_players: 200, name: srv.name, gamemode: 'RPG (Local Test)', local_fallback: true };
    }
  }
  return result;
});

ipcMain.handle('get-status', () => {
  const gtaPath = findGtaPath();
  return getStatus(gtaPath);
});

ipcMain.handle('get-config', () => {
  const srv = getActiveServer();
  return { SERVER_IP: srv.ip, SERVER_PORT: srv.port, SERVER_NAME: srv.name, WEBSITE_URL, DISCORD_URL, LAUNCHER_VERSION, server_mode: srv.mode };
});

ipcMain.handle('set-server-mode', (event, mode) => {
  if (mode !== 'production' && mode !== 'local') return { error: 'Invalid mode' };
  const settings = loadSettings();
  settings.server_mode = mode;
  saveSettings(settings);
  const srv = getActiveServer();
  return { success: true, ip: srv.ip, port: srv.port, name: srv.name, mode: srv.mode };
});

ipcMain.handle('browse-gta', async () => {
  const result = await dialog.showOpenDialog(mainWindow, {
    title: 'Izaberi GTA:SA folder',
    properties: ['openDirectory']
  });
  if (!result.canceled && result.filePaths.length > 0) {
    const selected = result.filePaths[0];
    if (fs.existsSync(path.join(selected, 'gta_sa.exe'))) {
      const settings = loadSettings();
      settings.gta_path = selected;
      saveSettings(settings);
      _gtaPathCache = selected; // Update cache
      return { path: selected, status: getStatus(selected) };
    }
    return { error: 'U ovom folderu nije pronadjen gta_sa.exe!' };
  }
  return null;
});

// ============================================================
//  RENAME CEF/ASI FILES (prevent crash in local mode)
//  In local mode, CEF client files can crash SA-MP.
//  We temporarily rename them before launch.
// ============================================================
function toggleCefFiles(gtaPath, disable) {
  const filesToToggle = [
    { name: 'cef.asi', disabled: 'cef.asi.disabled' },
    { name: 'dsound.dll', disabled: 'dsound.dll.disabled' },
    { name: 'dinput8.dll', disabled: 'dinput8.dll.disabled' }
  ];
  const toggled = [];
  for (const f of filesToToggle) {
    const normalPath = path.join(gtaPath, f.name);
    const disabledPath = path.join(gtaPath, f.disabled);
    try {
      if (disable) {
        if (fs.existsSync(normalPath) && !fs.existsSync(disabledPath)) {
          fs.renameSync(normalPath, disabledPath);
          toggled.push(f.name + ' -> ' + f.disabled);
          log('Disabled: ' + f.name);
        }
      } else {
        if (fs.existsSync(disabledPath)) {
          fs.renameSync(disabledPath, normalPath);
          toggled.push(f.disabled + ' -> ' + f.name);
          log('Re-enabled: ' + f.name);
        }
      }
    } catch (e) {
      log('Toggle error for ' + f.name + ': ' + e.message);
    }
  }
  return toggled;
}

// Check if CEF files are currently active or disabled
function getCefState(gtaPath) {
  if (!gtaPath) return { enabled: false, files: {} };
  const cefAsi = fs.existsSync(path.join(gtaPath, 'cef.asi'));
  const cefAsiDis = fs.existsSync(path.join(gtaPath, 'cef.asi.disabled'));
  const dsound = fs.existsSync(path.join(gtaPath, 'dsound.dll'));
  const dsoundDis = fs.existsSync(path.join(gtaPath, 'dsound.dll.disabled'));
  const dinput = fs.existsSync(path.join(gtaPath, 'dinput8.dll'));
  const dinputDis = fs.existsSync(path.join(gtaPath, 'dinput8.dll.disabled'));
  const cefFolder = fs.existsSync(path.join(gtaPath, 'cef'));

  const hasAnyActive = cefAsi || dsound || dinput;
  const hasAnyDisabled = cefAsiDis || dsoundDis || dinputDis;

  return {
    enabled: hasAnyActive,
    has_files: hasAnyActive || hasAnyDisabled,
    cef_folder: cefFolder,
    files: { cefAsi, cefAsiDis, dsound, dsoundDis, dinput, dinputDis, cefFolder }
  };
}

// ============================================================
//  PRE-LAUNCH FIXES (prevent common SA-MP crashes)
// ============================================================

// Kill any zombie gta_sa.exe processes (causes crash at 0x00746929)
function killZombieProcesses() {
  try {
    const result = execSync('tasklist /FI "IMAGENAME eq gta_sa.exe" /NH', { encoding: 'utf8', windowsHide: true });
    if (result.includes('gta_sa.exe')) {
      log('Found zombie gta_sa.exe process, killing...');
      try {
        execSync('taskkill /F /IM gta_sa.exe', { windowsHide: true });
        log('Killed zombie gta_sa.exe');
      } catch (e) {
        log('Could not kill gta_sa.exe: ' + e.message);
      }
      return true;
    }
  } catch (e) { /* tasklist not found or no processes */ }
  return false;
}

// Delete gta_sa.set (corrupt settings cause crashes)
function deleteGtaSet() {
  const paths = [
    path.join(process.env.USERPROFILE || '', 'Documents', 'GTA San Andreas User Files', 'gta_sa.set'),
    path.join(process.env.USERPROFILE || '', 'Documents', 'GTA SA User Files', 'gta_sa.set')
  ];
  let deleted = false;
  for (const p of paths) {
    try {
      if (fs.existsSync(p)) {
        fs.unlinkSync(p);
        log('Deleted gta_sa.set: ' + p);
        deleted = true;
      }
    } catch (e) {
      log('Could not delete gta_sa.set: ' + e.message);
    }
  }
  return deleted;
}

// Remove compatibility mode from gta_sa.exe and samp.exe
function removeCompatibilityMode(gtaPath) {
  const files = ['gta_sa.exe', 'samp.exe'];
  let fixed = false;
  for (const f of files) {
    const fullPath = path.join(gtaPath, f);
    if (!fs.existsSync(fullPath)) continue;
    try {
      // Check and remove compatibility mode from registry
      const { execSync } = require('child_process');
      // Remove from HKCU\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers
      try {
        execSync(`reg delete "HKCU\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers" /v "${fullPath}" /f`, { windowsHide: true, stdio: 'pipe' });
        log('Removed compatibility mode for: ' + f);
        fixed = true;
      } catch (e) {
        // No compat entry = good, nothing to remove
      }
    } catch (e) { /* ignore */ }
  }
  return fixed;
}

ipcMain.handle('launch-game', async (event, nickname) => {
  const gtaPath = findGtaPath();
  if (!gtaPath) return { error: 'GTA:SA putanja nije pronadjena! Idi u Podesavanja i izaberi GTA:SA folder.' };

  const sampExe = path.join(gtaPath, 'samp.exe');
  if (!fs.existsSync(sampExe)) return { error: 'samp.exe nije pronadjen! Instaliraj SA-MP klijent (R4/R5).' };

  // Check for samp.img (missing = instant crash)
  const sampImg = path.join(gtaPath, 'samp.img');
  if (!fs.existsSync(sampImg)) {
    log('WARNING: samp.img not found - will cause crash!');
    return { error: 'samp.img nije pronadjen u GTA folderu! Ovaj fajl je potreban za SA-MP. reinstaliraj SA-MP klijent.' };
  }

  if (!nickname || nickname.length < 3) nickname = 'Unicate_Player';
  nickname = nickname.replace(/[^a-zA-Z0-9_\[\]]/g, '_');
  if (nickname.length > 20) nickname = nickname.substring(0, 20);

  // PRE-LAUNCH FIXES
  log('Running pre-launch crash fixes...');
  const killedZombie = killZombieProcesses();
  const deletedSet = deleteGtaSet();
  const removedCompat = removeCompatibilityMode(gtaPath);
  log('Pre-launch: zombie=' + killedZombie + ' set=' + deletedSet + ' compat=' + removedCompat);

  const settings = loadSettings();
  const srv = getActiveServer();
  const cefEnabled = settings.cef_enabled !== false; // default: true

  if (srv.mode === 'production') {
    const status = getStatus(gtaPath);
    if (!status.ready) return { error: 'Nisu sve komponente instalirane! Pokreni auto-instalaciju.' };
    // Production always needs CEF - make sure it's enabled
    if (!cefEnabled) {
      toggleCefFiles(gtaPath, false);
      log('Production mode: re-enabling CEF files');
    }
  } else {
    // Local mode: respect user's CEF toggle setting
    if (!cefEnabled) {
      log('Local mode: CEF disabled by user, disabling CEF/ASI files...');
      toggleCefFiles(gtaPath, true);
    } else {
      // CEF enabled - make sure files are active
      const cefState = getCefState(gtaPath);
      if (!cefState.enabled && cefState.has_files) {
        log('Local mode: CEF enabled by user, re-enabling CEF/ASI files...');
        toggleCefFiles(gtaPath, false);
      }
      log('Local mode: CEF enabled - all CEF features active (tablet, inventar, TD)');
    }
  }

  log('Launching: ' + srv.ip + ':' + srv.port + ' nick=' + nickname + ' cef=' + cefEnabled);

  try {
    setupSampRegistry(gtaPath, srv.ip, srv.port, nickname);
    
    const child = spawn(sampExe, [srv.ip, srv.port.toString(), nickname], {
      cwd: gtaPath,
      detached: true,
      stdio: 'ignore'
    });
    child.unref();
    
    log('Launch OK (PID: ' + child.pid + ')');
    const fixes = [];
    if (killedZombie) fixes.push('ubijen dupli proces');
    if (deletedSet) fixes.push('obrisan gta_sa.set');
    if (removedCompat) fixes.push('iskljucen compatibility mode');
    const fixMsg = fixes.length > 0 ? ' (Fix: ' + fixes.join(', ') + ')' : '';
    const msg = cefEnabled 
      ? 'SA-MP pokrenut! CEF ukljucen (tablet, inventar, TD rade).' + fixMsg
      : 'SA-MP pokrenut! CEF iskljucen (osnovni mod).' + fixMsg;
    return { success: true, pid: child.pid, message: msg };
  } catch (err) {
    log('Launch FAILED: ' + err.message);
    return { error: 'Greska pri pokretanju: ' + err.message };
  }
});

ipcMain.handle('toggle-cef', (event, enabled) => {
  const settings = loadSettings();
  settings.cef_enabled = enabled;
  saveSettings(settings);
  const gtaPath = findGtaPath();
  if (gtaPath) {
    toggleCefFiles(gtaPath, !enabled);
  }
  log('CEF toggle: ' + (enabled ? 'ON' : 'OFF'));
  return { success: true, cef_enabled: enabled };
});

ipcMain.handle('get-cef-state', () => {
  const gtaPath = findGtaPath();
  const settings = loadSettings();
  const cefState = getCefState(gtaPath);
  return {
    enabled: cefState.enabled,
    has_files: cefState.has_files,
    cef_folder: cefState.cef_folder,
    setting: settings.cef_enabled !== false,
    files: cefState.files
  };
});

ipcMain.handle('auto-install', async () => {
  const gtaPath = findGtaPath();
  if (!gtaPath) return { error: 'GTA:SA putanja nije pronadjena!' };
  const status = getStatus(gtaPath);
  if (status.missing.length === 0) return { success: true, message: 'Sve je vec instalirano!' };

  try {
    await autoInstall(gtaPath, status.missing);
    return { success: true };
  } catch (e) {
    return { error: e.message };
  }
});

ipcMain.handle('open-url', (event, url) => { shell.openExternal(url); });
ipcMain.handle('minimize-window', () => { if (mainWindow) mainWindow.minimize(); });
ipcMain.handle('maximize-window', () => { if (mainWindow) { mainWindow.isMaximized() ? mainWindow.unmaximize() : mainWindow.maximize(); } });
ipcMain.handle('close-window', () => { if (mainWindow) mainWindow.close(); });

// ============================================================
//  APP EVENTS
// ============================================================
app.whenReady().then(() => {
  createWindow();
}).catch(err => { log('App ready FAILED: ' + err.message); });

app.on('window-all-closed', () => { app.quit(); });
app.on('activate', () => { if (BrowserWindow.getAllWindows().length === 0) createWindow(); });
