const { app, BrowserWindow, ipcMain, shell, dialog } = require('electron');
const path = require('path');
const fs = require('fs');
const https = require('https');
const http = require('http');
const { exec, execFile, spawn, execSync } = require('child_process');
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
const LOG_FILE = path.join(app.isPackaged ? path.dirname(app.getPath('exe')) : path.join(__dirname, '..'), 'launcher_debug.log');
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
try { fs.writeFileSync(LOG_FILE, '=== Unicate Gaming Launcher v3.12 ===\n'); } catch(e) {}

log('Launcher v3.12 starting...');

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
const LAUNCHER_VERSION = '3.12.0';

const OMP_CEF_ASI_URL = 'https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/cef.asi';
const OMP_CEF_CLIENT_URL = 'https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/client-files-v1.2.0.zip';
const ASI_LOADER_URL = 'https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/v9.7.2/Ultimate-ASI-Loader.zip';
const SAMP_CLIENT_URL = 'https://files.sa-mp.com/sa-mp-0.3.7-R4-install.exe';

// In development (npm start), __dirname is src/ so we go up one level
// In production (packaged), app.getPath('exe') is the correct dir
const LAUNCHER_DIR = app.isPackaged ? path.dirname(app.getPath('exe')) : path.join(__dirname, '..');
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
      // Delete corrupted samp.exe and its backup before reinstalling
      const sampExePath = path.join(gtaPath, 'samp.exe');
      const sampDllPath = path.join(gtaPath, 'samp.dll');
      try { if (fs.existsSync(sampExePath)) fs.unlinkSync(sampExePath); } catch(e) {}
      try { if (fs.existsSync(sampExePath + '.ug_backup')) fs.unlinkSync(sampExePath + '.ug_backup'); } catch(e) {}
      try { if (fs.existsSync(sampDllPath + '.ug_backup')) fs.unlinkSync(sampDllPath + '.ug_backup'); } catch(e) {}
      log('Auto-install: Cleaned up old SA-MP files before reinstall');

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
//  SA-MP REGISTRY SETUP (GENTLE - only touch what we need)
// ============================================================
function setupSampRegistry(gtaPath, ip, port, nickname) {
  try {
    const { execSync } = require('child_process');
    
    // GENTLE approach: Only set/update the values we need
    // DO NOT nuke the entire registry key - SA-MP stores important
    // settings there (audio, chat, display, etc.) that the game needs
    
    // Set PlayerName
    execSync(`reg add "HKCU\\Software\\SAMP" /v "PlayerName" /t REG_SZ /d "${nickname}" /f`, { windowsHide: true });
    // Set LastServer
    execSync(`reg add "HKCU\\Software\\SAMP" /v "LastServer" /t REG_SZ /d "${ip}:${port}" /f`, { windowsHide: true });
    // Set gta_sa_exe path
    execSync(`reg add "HKCU\\Software\\SAMP" /v "gta_sa_exe" /t REG_SZ /d "${gtaPath}\\gta_sa.exe" /f`, { windowsHide: true });
    
    // ONLY delete the Password value (this is what causes "Wrong server password")
    try {
      execSync(`reg delete "HKCU\\Software\\SAMP" /v "Password" /f`, { windowsHide: true, stdio: 'pipe' });
      log('Deleted Password value from registry');
    } catch (e) { 
      log('No Password value to delete (good)'); 
    }
    
    log('Registry setup OK (gentle - only deleted Password)');
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

// Delete gta_sa.set ONLY if it's corrupted (causes crash at 0x00746929)
// But DON'T always delete it - valid settings help the game run stable
function deleteGtaSetIfNeeded() {
  const paths = [
    path.join(process.env.USERPROFILE || '', 'Documents', 'GTA San Andreas User Files', 'gta_sa.set'),
    path.join(process.env.USERPROFILE || '', 'Documents', 'GTA SA User Files', 'gta_sa.set')
  ];
  let deleted = false;
  for (const p of paths) {
    try {
      if (fs.existsSync(p)) {
        const stat = fs.statSync(p);
        // Only delete if file is suspiciously small (< 100 bytes = likely corrupt)
        // Normal gta_sa.set is several KB
        if (stat.size < 100) {
          fs.unlinkSync(p);
          log('Deleted CORRUPT gta_sa.set (too small): ' + p);
          deleted = true;
        } else {
          log('Keeping valid gta_sa.set: ' + p + ' (' + stat.size + ' bytes)');
        }
      }
    } catch (e) {
      log('Could not check gta_sa.set: ' + e.message);
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

// ============================================================
//  UG SPLASH SCREEN PATCHER (v2 - SAFE)
//  Only patches samp.dll (patching samp.exe CORRUPTS it!)
//  Verifies PE integrity after patching - auto-restores backup if broken
//  If Resource Hacker can't list resources, SKIP patching (don't guess)
// ============================================================
const RH_URL = 'http://www.angusj.com/resourcehacker/resource_hacker.zip';

// Check if a file is a valid PE (Portable Executable)
// Checks MZ header AND PE signature (Resource Hacker can corrupt
// the internal PE structure while keeping the MZ header intact)
function isValidPE(fpath) {
  try {
    const fd = fs.openSync(fpath, 'r');
    const buf = Buffer.alloc(512);
    fs.readSync(fd, buf, 0, 512, 0);
    fs.closeSync(fd);

    // Check MZ header (DOS header)
    if (buf[0] !== 0x4D || buf[1] !== 0x5A) return false;

    // Check PE signature - this is what Resource Hacker breaks!
    // e_lfanew at offset 0x3C points to PE signature
    const peOffset = buf.readUInt32LE(0x3C);
    if (peOffset < 0x40 || peOffset > 400) return false;
    if (buf[peOffset] !== 0x50 || buf[peOffset + 1] !== 0x45 ||
        buf[peOffset + 2] !== 0x00 || buf[peOffset + 3] !== 0x00) return false;

    return true;
  } catch (e) {
    return false;
  }
}

// Restore clean original files from .ug_backup
// Since v3.9 no longer patches samp.exe with Resource Hacker,
// ANY existing backup is the ORIGINAL clean file from before corruption.
// We ALWAYS restore from backup if it exists - don't trust the current file.
function restoreFromBackup(gtaPath) {
  const filesToCheck = ['samp.exe', 'samp.dll'];
  let restored = [];
  for (const fname of filesToCheck) {
    const fpath = path.join(gtaPath, fname);
    const backup = fpath + '.ug_backup';

    if (!fs.existsSync(backup)) continue;

    // Always restore from backup if backup exists and is a valid PE
    // The backup was created BEFORE any Resource Hacker patching,
    // so it's always the clean original
    if (isValidPE(backup)) {
      fs.copyFileSync(backup, fpath);
      log('Restored ' + fname + ' from backup (clean original)');
      restored.push(fname);
    } else {
      // Backup is also corrupted! Delete it and mark for reinstall
      log('WARNING: Backup for ' + fname + ' is also corrupted! Will need reinstall.');
      try { fs.unlinkSync(backup); } catch (e) {}
    }
  }
  return restored;
}

async function patchSampSplash(gtaPath) {
  const ugSplashBmp = path.join(LAUNCHER_DIR, 'ug_splash.bmp');
  const rhExe = path.join(LAUNCHER_DIR, 'ResourceHacker.exe');

  // STEP 1: Always restore clean files from backup first
  const restored = restoreFromBackup(gtaPath);
  if (restored.length > 0) {
    log('Splash: Restored clean files from backup: ' + restored.join(', '));
  }

  // Check prerequisites
  if (!fs.existsSync(ugSplashBmp)) {
    log('UG Splash: ug_splash.bmp not found, skipping');
    return false;
  }
  if (!gtaPath || !fs.existsSync(gtaPath)) {
    log('UG Splash: GTA path not found, skipping');
    return false;
  }

  // Verify BMP format
  try {
    const d = fs.readFileSync(ugSplashBmp);
    if (d[0] !== 0x42 || d[1] !== 0x4D) {
      log('UG Splash: Not a valid BMP file!');
      return false;
    }
    log('UG Splash: BMP format = ' + d.readUInt16LE(28) + '-bit');
  } catch (e) {
    log('UG Splash: BMP check error: ' + e.message);
  }

  // Download Resource Hacker if needed
  if (!fs.existsSync(rhExe)) {
    log('UG Splash: Downloading Resource Hacker...');
    try {
      const tmpZip = path.join(LAUNCHER_DIR, 'tmp_rh.zip');
      await downloadFile(RH_URL, tmpZip, (pct) => {
        if (mainWindow) mainWindow.webContents.send('install-progress', {
          component: 'UG Splash Patcher', pct, downloaded: 0, total: 0, speed: 0
        });
      });
      const zip = new AdmZip(tmpZip);
      for (const entry of zip.getEntries()) {
        const fn = entry.entryName.split('/').pop();
        if (fn.toLowerCase() === 'resourcehacker.exe' && !entry.isDirectory) {
          fs.writeFileSync(rhExe, entry.getData());
          log('UG Splash: ResourceHacker.exe extracted');
          break;
        }
      }
      try { fs.unlinkSync(tmpZip); } catch (e) {}
    } catch (e) {
      log('UG Splash: Failed to download Resource Hacker: ' + e.message);
      return false;
    }
  }

  if (!fs.existsSync(rhExe)) {
    log('UG Splash: ResourceHacker.exe not found');
    return false;
  }

  // Try patching BOTH samp.exe and samp.dll
  // The SA-MP splash bitmap is typically in samp.exe (the connection dialog).
  // samp.dll might also have bitmaps.
  // We try each file with PE signature verification - if patch corrupts it, we restore.
  const filesToTry = ['samp.exe', 'samp.dll'];
  let anyPatched = false;

  for (const fname of filesToTry) {
    const fpath = path.join(gtaPath, fname);
    if (!fs.existsSync(fpath)) {
      log('Splash: ' + fname + ' not found, skipping');
      continue;
    }

    // Restore clean original from backup before patching
    const backup = fpath + '.ug_backup';
    if (fs.existsSync(backup) && isValidPE(backup)) {
      fs.copyFileSync(backup, fpath);
      log('Splash: Restored clean ' + fname + ' from backup before patching');
    } else if (!fs.existsSync(backup)) {
      fs.copyFileSync(fpath, backup);
      log('Splash: Created backup of ' + fname);
    }

    // Verify file is valid PE before touching it
    if (!isValidPE(fpath)) {
      log('Splash: ' + fname + ' is not a valid PE, skipping');
      continue;
    }

    log('Splash: Attempting to patch ' + fname + '...');

    // Try to list bitmap resources
    const resFile = path.join(LAUNCHER_DIR, fname + '_res.txt');
    try { try { fs.unlinkSync(resFile); } catch(e) {} } catch(e) {}

    let ids = [];
    try {
      try {
        const result = execSync('"' + rhExe + '" -open "' + fpath + '" -save "' + resFile + '" -action list -mask ,,', {
          timeout: 15000, windowsHide: true, encoding: 'utf8'
        });
        log('Splash: RH list for ' + fname + ': ' + (result || '').substring(0, 300));
      } catch (e) {
        log('Splash: RH list for ' + fname + ' exit=' + (e.status || '?'));
      }
    } catch (e) {}

    if (fs.existsSync(resFile)) {
      try {
        const txt = fs.readFileSync(resFile, 'utf8');
        log('Splash: ' + fname + ' resources: ' + txt.substring(0, 500));
        const matches = [...txt.matchAll(/BITMAP[^\d]*(\d+)/gi)];
        if (matches.length > 0) ids = matches.map(m => parseInt(m[1]));
      } catch (e) {}
      try { fs.unlinkSync(resFile); } catch (e) {}
    }

    if (ids.length === 0) {
      // RH couldn't list or no bitmaps found - try common IDs
      ids = [100, 101, 102, 103];
      log('Splash: ' + fname + ' - trying common bitmap IDs: ' + ids.join(', '));
    } else {
      log('Splash: ' + fname + ' - found bitmap IDs: ' + ids.join(', '));
    }

    // Try patching each ID until one works
    for (const id of ids) {
      const tmp = path.join(LAUNCHER_DIR, 'ug_tmp_' + fname);
      try { try { fs.unlinkSync(tmp); } catch(e) {} } catch(e) {}

      try {
        try {
          execSync('"' + rhExe + '" -open "' + fpath + '" -save "' + tmp + '" -action addoverwrite -res "' + ugSplashBmp + '" -mask BITMAP,' + id + ',0', {
            timeout: 30000, windowsHide: true, encoding: 'utf8'
          });
        } catch (e) {
          log('Splash: RH patch ' + fname + ' ID=' + id + ' exit=' + (e.status || '?'));
        }

        // CRITICAL: Verify PE signature after patching
        if (fs.existsSync(tmp) && fs.statSync(tmp).size > 10000 && isValidPE(tmp)) {
          const origSize = fs.statSync(fpath).size;
          const patchSize = fs.statSync(tmp).size;
          log('Splash: ' + fname + ' ID=' + id + ' PE OK (orig=' + origSize + ' patched=' + patchSize + ' diff=' + (patchSize - origSize) + ')');
          fs.copyFileSync(tmp, fpath);
          log('Splash: SUCCESS! Patched ' + fname + ' BITMAP,' + id);
          anyPatched = true;
          try { fs.unlinkSync(tmp); } catch (e) {}
          break;
        } else {
          log('Splash: ' + fname + ' ID=' + id + ' FAILED PE check - restoring backup');
          if (fs.existsSync(backup) && isValidPE(backup)) {
            fs.copyFileSync(backup, fpath);
          }
          try { if (fs.existsSync(tmp)) fs.unlinkSync(tmp); } catch (e) {}
        }
      } catch (e) {
        log('Splash: ' + fname + ' ID=' + id + ' error: ' + e.message);
        try { if (fs.existsSync(tmp)) fs.unlinkSync(tmp); } catch (e2) {}
      }
    }

    // If this file was patched successfully, no need to try the other
    if (anyPatched) break;
  }

  if (!anyPatched) {
    log('Splash: WARNING - could not patch splash in any file');
    // Restore all from backup to be safe
    for (const fname of filesToTry) {
      const fpath = path.join(gtaPath, fname);
      const backup = fpath + '.ug_backup';
      if (fs.existsSync(backup) && isValidPE(backup)) {
        fs.copyFileSync(backup, fpath);
      }
    }
  }

  return anyPatched;
}

ipcMain.handle('launch-game', async (event, nickname) => {
  const gtaPath = findGtaPath();
  if (!gtaPath) return { error: 'GTA:SA putanja nije pronadjena! Idi u Podesavanja i izaberi GTA:SA folder.' };

  // AUTO-INSTALL: Check what's missing and install automatically
  const status = getStatus(gtaPath);
  if (status.missing.length > 0) {
    log('Auto-install: Missing components detected: ' + status.missing.join(', '));
    if (mainWindow) mainWindow.webContents.send('install-progress', {
      component: 'Instalacija komponenti...', pct: 0, downloaded: 0, total: 0, speed: 0
    });
    try {
      await autoInstall(gtaPath, status.missing);
      log('Auto-install: All missing components installed!');
    } catch (e) {
      log('Auto-install: Failed: ' + e.message);
    }
  }

  // Verify samp.exe exists
  const sampExe = path.join(gtaPath, 'samp.exe');
  if (!fs.existsSync(sampExe)) return { error: 'samp.exe nije pronadjen! Auto-instalacija nije uspjela.' };

  // Check for samp.img - warn but don't block (some installs don't have it in GTA dir)
  const sampImg = path.join(gtaPath, 'samp.img');
  const hasSampImg = fs.existsSync(sampImg);
  if (!hasSampImg) {
    log('WARNING: samp.img not found in GTA dir (may be OK for some installs)');
  }

  if (!nickname || nickname.length < 3) nickname = 'Player';
  // Samo filtriraj nedozvoljene znakove - BEZ ikakvih modifikacija imena!
  // Ne dodajemo _Player, ne kapitaliziramo, ne mijenjamo nista.
  // Igrac sam bira svoje ime tacno kako hoce.
  nickname = nickname.replace(/[^a-zA-Z0-9_\[\]]/g, '_');
  if (nickname.length > 20) nickname = nickname.substring(0, 20);

  // PRE-LAUNCH FIXES
  log('Running pre-launch crash fixes...');
  const killedZombie = killZombieProcesses();
  const deletedSet = deleteGtaSetIfNeeded();
  const removedCompat = removeCompatibilityMode(gtaPath);
  log('Pre-launch: zombie=' + killedZombie + ' set=' + deletedSet + ' compat=' + removedCompat);

  const settings = loadSettings();
  const srv = getActiveServer();
  const cefEnabled = settings.cef_enabled !== false; // default: true

  if (srv.mode === 'production') {
    const prodStatus = getStatus(gtaPath);
    if (!prodStatus.ready) return { error: 'Nisu sve komponente instalirane! Pokreni auto-instalaciju.' };
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

  // === UG SPLASH: Automatically patch samp.dll with UG splash bitmap ===
  try {
    await patchSampSplash(gtaPath);
  } catch (e) {
    log('UG Splash: Error: ' + e.message);
  }

  // === CEF LOADING SCREEN + PORTAL: Copy to GTA/cef folder ===
  try {
    const cefLoadingSrc = path.join(LAUNCHER_DIR, 'cef_content', 'loading');
    const cefPortalSrc = path.join(LAUNCHER_DIR, 'cef_content', 'portal');
    const cefDest = path.join(gtaPath, 'cef');

    if (fs.existsSync(cefLoadingSrc) || fs.existsSync(cefPortalSrc)) {
      // Ensure cef directory exists
      if (!fs.existsSync(cefDest)) fs.mkdirSync(cefDest, { recursive: true });

      // Copy loading screen
      if (fs.existsSync(cefLoadingSrc)) {
        const loadingDest = path.join(cefDest, 'loading');
        if (fs.existsSync(loadingDest)) fs.rmSync(loadingDest, { recursive: true, force: true });
        fs.cpSync(cefLoadingSrc, loadingDest, { recursive: true });
        log('CEF: Loading screen installed to GTA/cef/loading/');
      }

      // Copy portal
      if (fs.existsSync(cefPortalSrc)) {
        const portalDest = path.join(cefDest, 'portal');
        if (fs.existsSync(portalDest)) fs.rmSync(portalDest, { recursive: true, force: true });
        fs.cpSync(cefPortalSrc, portalDest, { recursive: true });
        log('CEF: Portal installed to GTA/cef/portal/');
      }
    }
  } catch (e) {
    log('CEF Content: Could not copy loading/portal: ' + e.message);
  }

  log('Launching: ' + srv.ip + ':' + srv.port + ' nick=' + nickname + ' cef=' + cefEnabled);

  // FINAL CHECK: Verify samp.exe is still a valid PE after splash patching
  // If splash patch corrupted it, restore backup before launching
  if (fs.existsSync(sampExe) && !isValidPE(sampExe)) {
    log('CRITICAL: samp.exe corrupted after splash patch! Restoring backup...');
    const backup = sampExe + '.ug_backup';
    if (fs.existsSync(backup) && isValidPE(backup)) {
      fs.copyFileSync(backup, sampExe);
      log('Restored samp.exe from backup before launch');
    } else {
      // Backup is also bad - reinstall SA-MP client
      log('No valid backup - auto-reinstalling SA-MP client...');
      try { fs.unlinkSync(sampExe); } catch(e) {}
      try { if (fs.existsSync(backup)) fs.unlinkSync(backup); } catch(e) {}
      await autoInstall(gtaPath, ['client']);
      if (!fs.existsSync(sampExe) || !isValidPE(sampExe)) {
        return { error: 'samp.exe je pokvaren i ne moze se popraviti!' };
      }
    }
  }

  try {
    setupSampRegistry(gtaPath, srv.ip, srv.port, nickname);
    
    // samp:// protocol - ONLY method that doesn't cause "Wrong server password"
    await new Promise(resolve => setTimeout(resolve, 500));
    
    const sampUrl = `samp://${srv.ip}:${srv.port}`;
    log('Launching via samp://: ' + sampUrl);
    
    shell.openExternal(sampUrl);
    
    log('Launch OK via samp://');
    const msg = 'SA-MP pokrenut! Spajanje na ' + srv.ip + ':' + srv.port + '...';
    return { success: true, message: msg };
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

  // Restore clean original files from backup first
  const restored = restoreFromBackup(gtaPath);
  if (restored.length > 0) {
    log('Auto-install: Restored clean files from backup: ' + restored.join(', '));
  }

  const status = getStatus(gtaPath);
  // Also check if samp.exe is corrupted even if it "exists"
  const sampExePath = path.join(gtaPath, 'samp.exe');
  if (fs.existsSync(sampExePath) && !isValidPE(sampExePath)) {
    // samp.exe is corrupted - need to reinstall
    if (!status.missing.includes('client')) status.missing.push('client');
    log('Auto-install: samp.exe is corrupted, adding client to reinstall list');
  }

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
