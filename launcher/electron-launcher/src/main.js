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
app.commandLine.appendSwitch('enable-gpu-rasterization');
app.commandLine.appendSwitch('enable-zero-copy');
app.commandLine.appendSwitch('ignore-gpu-blocklist');

// ============================================================
//  ASYNC LOG (non-blocking)
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

try { fs.writeFileSync(LOG_FILE, '=== Unicate Gaming Launcher v5.0 ===\n'); } catch(e) {}
log('Launcher v5.0 starting...');

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
const LAUNCHER_VERSION = '5.0.0';

const OMP_CEF_ASI_URL = 'https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/cef.asi';
const OMP_CEF_CLIENT_URL = 'https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/client-files-v1.2.0.zip';
const ASI_LOADER_URL = 'https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/v9.7.2/Ultimate-ASI-Loader.zip';
const SAMP_CLIENT_URL = 'https://files.sa-mp.com/sa-mp-0.3.7-R4-install.exe';

const LAUNCHER_DIR = app.isPackaged ? path.dirname(app.getPath('exe')) : path.join(__dirname, '..');
const SETTINGS_FILE = path.join(LAUNCHER_DIR, 'settings.json');

// CEF content is in extraResources when packaged (process.resourcesPath),
// but in the project root when running in dev mode
const CEF_CONTENT_DIR = app.isPackaged
  ? path.join(process.resourcesPath, 'cef_content')
  : path.join(LAUNCHER_DIR, 'cef_content');

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
    'D:\\GTASA',
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
//  STATUS CHECK
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
    const hasCefFolder = fs.existsSync(path.join(gtaPath, 'cef'));
    // CEF runtime check: libcef.dll and client.dll are the critical CEF engine files
    const hasCefRuntime = fs.existsSync(path.join(gtaPath, 'cef', 'libcef.dll')) && fs.existsSync(path.join(gtaPath, 'cef', 'client.dll'));
    s.has_asi = fs.existsSync(path.join(gtaPath, 'dsound.dll')) || fs.existsSync(path.join(gtaPath, 'dinput8.dll'));
    if (hasAsi && hasCefRuntime) { s.cef_ok = true; s.cef_msg = 'CEF OK'; }
    else if (hasAsi && hasCefFolder) { s.cef_msg = 'CEF runtime fali (libcef.dll)'; }
    else if (hasAsi) { s.cef_msg = 'Fali cef/ folder'; }
    else if (hasCefFolder) { s.cef_msg = 'Fali cef.asi'; }
    else { s.cef_msg = 'Nije instaliran'; }
    if (!s.has_samp) s.missing.push('client');
    if (!isLocal) {
      if (!hasAsi) s.missing.push('cef_asi');
      if (!hasCefRuntime) s.missing.push('cef_runtime');
      if (!s.has_asi) s.missing.push('asi_loader');
      s.ready = s.has_samp && s.cef_ok && s.has_asi;
    } else {
      s.ready = s.has_samp;
    }
  }
  return s;
}

// ============================================================
//  SAMP SERVER QUERY
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
          if (data.length < 11) { resolve({ online: false }); return; }
          let off = 11;
          if (off + 1 > data.length) { resolve({ online: false }); return; }
          const isPassworded = data.readUInt8(off); off += 1;
          if (off + 2 > data.length) { resolve({ online: false }); return; }
          const players = data.readUInt16LE(off); off += 2;
          if (off + 2 > data.length) { resolve({ online: false }); return; }
          const maxp = data.readUInt16LE(off); off += 2;
          if (off + 4 > data.length) { resolve({ online: false }); return; }
          const nlen = data.readUInt32LE(off); off += 4;
          if (off + nlen > data.length) { resolve({ online: false }); return; }
          const name = data.slice(off, off + nlen).toString('latin1'); off += nlen;
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

    mod.get(url, { timeout: 60000, headers: { 'User-Agent': 'UnicateGamingLauncher/5.0' } }, (response) => {
      if (response.statusCode >= 300 && response.statusCode < 400 && response.headers.location) {
        const newUrl = response.headers.location;
        response.resume();
        return downloadFile(newUrl, dest, onProgress, maxRedirects - 1).then(resolve).catch(reject);
      }
      if (response.statusCode !== 200) {
        response.resume();
        return reject(new Error('HTTP ' + response.statusCode + ' for ' + url));
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
    }).on('error', (err) => { reject(err); });
  });
}

// ============================================================
//  AUTO-INSTALL
// ============================================================
async function autoInstall(gtaPath, missing) {
  for (const comp of missing) {
    if (comp === 'client') {
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
        execSync('"' + tmpExe + '" /S /D=' + gtaPath, { timeout: 60000, windowsHide: true });
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
    execSync('reg add "HKCU\\Software\\SAMP" /v "PlayerName" /t REG_SZ /d "' + nickname + '" /f', { windowsHide: true });
    execSync('reg add "HKCU\\Software\\SAMP" /v "LastServer" /t REG_SZ /d "' + ip + ':' + port + '" /f', { windowsHide: true });
    execSync('reg add "HKCU\\Software\\SAMP" /v "gta_sa_exe" /t REG_SZ /d "' + gtaPath + '\\gta_sa.exe" /f', { windowsHide: true });
    try {
      execSync('reg delete "HKCU\\Software\\SAMP" /v "Password" /f', { windowsHide: true, stdio: 'pipe' });
      log('Deleted Password value from registry');
    } catch (e) { log('No Password value to delete (good)'); }
    log('Registry setup OK');
    return true;
  } catch (err) {
    log('Registry error: ' + err.message);
    return false;
  }
}

// ============================================================
//  CREATE WINDOW
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
      show: false,
      webPreferences: {
        nodeIntegration: false,
        contextIsolation: true,
        preload: preloadPath,
        sandbox: false,
        backgroundThrottling: false
      }
    });

    mainWindow.once('ready-to-show', () => {
      mainWindow.show();
      log('Window shown (ready-to-show)');
    });

    mainWindow.loadFile(indexPath);
    mainWindow.webContents.on('render-process-gone', (event, details) => {
      log('Renderer GONE! reason: ' + details.reason);
    });
    mainWindow.on('closed', () => { mainWindow = null; });
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
      _gtaPathCache = selected;
      return { path: selected, status: getStatus(selected) };
    }
    return { error: 'U ovom folderu nije pronadjen gta_sa.exe!' };
  }
  return null;
});

// ============================================================
//  CEF FILE TOGGLE
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
        }
      } else {
        if (fs.existsSync(disabledPath)) {
          fs.renameSync(disabledPath, normalPath);
          toggled.push(f.disabled + ' -> ' + f.name);
        }
      }
    } catch (e) { log('Toggle error for ' + f.name + ': ' + e.message); }
  }
  return toggled;
}

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
//  PRE-LAUNCH FIXES
// ============================================================
function killZombieProcesses() {
  try {
    const result = execSync('tasklist /FI "IMAGENAME eq gta_sa.exe" /NH', { encoding: 'utf8', windowsHide: true });
    if (result.includes('gta_sa.exe')) {
      log('Found zombie gta_sa.exe process, killing...');
      try {
        execSync('taskkill /F /IM gta_sa.exe', { windowsHide: true });
        log('Killed zombie gta_sa.exe');
      } catch (e) { log('Could not kill gta_sa.exe: ' + e.message); }
      return true;
    }
  } catch (e) {}
  return false;
}

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
        if (stat.size < 100) {
          fs.unlinkSync(p);
          log('Deleted CORRUPT gta_sa.set: ' + p);
          deleted = true;
        }
      }
    } catch (e) {}
  }
  return deleted;
}

function removeCompatibilityMode(gtaPath) {
  const files = ['gta_sa.exe', 'samp.exe'];
  let fixed = false;
  for (const f of files) {
    const fullPath = path.join(gtaPath, f);
    if (!fs.existsSync(fullPath)) continue;
    try {
      execSync('reg delete "HKCU\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers" /v "' + fullPath + '" /f', { windowsHide: true, stdio: 'pipe' });
      log('Removed compatibility mode for: ' + f);
      fixed = true;
    } catch (e) {}
  }
  return fixed;
}

// ============================================================
//  PE VALIDATION + BACKUP RESTORE
//  Resource Hacker corrupted files by adding new resource
//  sections. We detect this by checking file size - if samp.exe
//  grew by ~450KB (the BMP size), it's corrupted.
// ============================================================
function isValidPE(fpath) {
  try {
    const stat = fs.statSync(fpath);
    // samp.exe should be ~412KB, samp.dll ~1.2MB
    // If file is >2MB, definitely corrupted by Resource Hacker
    if (stat.size > 2000000) {
      log('isValidPE: ' + path.basename(fpath) + ' is ' + stat.size + ' bytes (>2MB) - CORRUPTED by Resource Hacker');
      return false;
    }

    const fd = fs.openSync(fpath, 'r');
    const buf = Buffer.alloc(512);
    fs.readSync(fd, buf, 0, 512, 0);
    fs.closeSync(fd);
    if (buf[0] !== 0x4D || buf[1] !== 0x5A) return false;
    const peOffset = buf.readUInt32LE(0x3C);
    if (peOffset < 0x40 || peOffset > 400) return false;
    if (buf[peOffset] !== 0x50 || buf[peOffset + 1] !== 0x45 ||
        buf[peOffset + 2] !== 0x00 || buf[peOffset + 3] !== 0x00) return false;
    return true;
  } catch (e) {
    return false;
  }
}

function restoreFromBackup(gtaPath) {
  const filesToCheck = ['samp.exe', 'samp.dll'];
  let restored = [];
  for (const fname of filesToCheck) {
    const fpath = path.join(gtaPath, fname);
    const backup = fpath + '.ug_backup';
    if (!fs.existsSync(backup)) continue;
    if (isValidPE(backup)) {
      fs.copyFileSync(backup, fpath);
      log('Restored ' + fname + ' from backup (clean original)');
      restored.push(fname);
    } else {
      log('WARNING: Backup for ' + fname + ' is also corrupted!');
      try { fs.unlinkSync(backup); } catch (e) {}
    }
  }
  return restored;
}

// Check if gta_sa.exe is running
function isGtaRunning() {
  try {
    const result = execSync('tasklist /FI "IMAGENAME eq gta_sa.exe" /NH', {
      encoding: 'utf8', windowsHide: true, timeout: 3000
    });
    return result.includes('gta_sa.exe');
  } catch (e) { return false; }
}

// ============================================================
//  UG SPLASH SCREEN - FULLSCREEN Electron overlay
//  Completely covers the SA-MP connection dialog with our UG splash.
//  Uses 'screen-saver' alwaysOnTop level to stay above EVERYTHING.
//  Monitors for gta_sa.exe and auto-closes when game starts.
// ============================================================
// UG SPLASH SCREEN - REMOVED
// Players now go directly from SA-MP splash to CEF loading screen.
// No intermediate Electron overlay needed since launcher installs CEF automatically.
function showUgSplash() {
  log('Splash: UG splash DISABLED - skipping, CEF loading screen will take over');
  return null;
}

// ============================================================
//  LAUNCH GAME
// ============================================================
ipcMain.handle('launch-game', async (event, nickname) => {
  const gtaPath = findGtaPath();
  if (!gtaPath) return { error: 'GTA:SA putanja nije pronadjena! Idi u Podesavanja i izaberi GTA:SA folder.' };

  // Restore any files corrupted by old launcher versions (Resource Hacker)
  restoreFromBackup(gtaPath);

  // ============================================================
  // FORCE VERIFY: Always check critical CEF files before launching
  // Even if status says OK, verify the actual runtime files exist
  // ============================================================
  const forceMissing = [];
  
  // Check ASI Loader (dsound.dll or dinput8.dll)
  const hasAsiLoader = fs.existsSync(path.join(gtaPath, 'dsound.dll')) || fs.existsSync(path.join(gtaPath, 'dinput8.dll'));
  if (!hasAsiLoader) forceMissing.push('asi_loader');
  
  // Check cef.asi
  if (!fs.existsSync(path.join(gtaPath, 'cef.asi'))) forceMissing.push('cef_asi');
  
  // Check CEF runtime (libcef.dll + client.dll) - THIS IS THE CRITICAL CHECK
  // Without these, cef.asi cannot render anything!
  const hasCefRuntime = fs.existsSync(path.join(gtaPath, 'cef', 'libcef.dll')) && fs.existsSync(path.join(gtaPath, 'cef', 'client.dll'));
  if (!hasCefRuntime) forceMissing.push('cef_runtime');
  
  // Check SA-MP client
  if (!fs.existsSync(path.join(gtaPath, 'samp.exe'))) forceMissing.push('client');

  if (forceMissing.length > 0) {
    log('FORCE VERIFY: Missing critical files: ' + forceMissing.join(', '));
    if (mainWindow) mainWindow.webContents.send('install-progress', {
      component: 'Provjera i instalacija fajlova...', pct: 0, downloaded: 0, total: 0, speed: 0
    });
    try {
      await autoInstall(gtaPath, forceMissing);
      log('FORCE VERIFY: All critical files installed!');
    } catch (e) {
      log('FORCE VERIFY: Failed: ' + e.message);
      return { error: 'Instalacija neuspjesna: ' + e.message };
    }
  } else {
    log('FORCE VERIFY: All critical CEF files present - OK');
  }

  // Also check for any other missing components from standard status check
  const status = getStatus(gtaPath);
  if (status.missing.length > 0) {
    log('Auto-install: Additional missing components: ' + status.missing.join(', '));
    try {
      await autoInstall(gtaPath, status.missing);
    } catch (e) {
      log('Auto-install: Failed: ' + e.message);
    }
  }

  const sampExe = path.join(gtaPath, 'samp.exe');
  if (!fs.existsSync(sampExe)) return { error: 'samp.exe nije pronadjen! Auto-instalacija nije uspjela.' };

  if (!nickname || nickname.length < 3) nickname = 'Player';
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
  const cefEnabled = settings.cef_enabled !== false;

  if (srv.mode === 'production') {
    const prodStatus = getStatus(gtaPath);
    if (!prodStatus.ready) return { error: 'Nisu sve komponente instalirane! Pokreni auto-instalaciju.' };
    if (!cefEnabled) {
      toggleCefFiles(gtaPath, false);
      log('Production mode: re-enabling CEF files');
    }
  } else {
    if (!cefEnabled) {
      toggleCefFiles(gtaPath, true);
      log('Local mode: CEF disabled by user');
    } else {
      const cefState = getCefState(gtaPath);
      if (!cefState.enabled && cefState.has_files) {
        toggleCefFiles(gtaPath, false);
        log('Local mode: re-enabling CEF files');
      }
    }
  }

  // === SHOW FULLSCREEN UG SPLASH (covers SA-MP dialog) ===
  const splashWin = showUgSplash();

  // === CEF LOADING SCREEN + PORTAL: Copy to GTA/cef folder ===
  try {
    const cefLoadingSrc = path.join(CEF_CONTENT_DIR, 'loading');
    const cefPortalSrc = path.join(CEF_CONTENT_DIR, 'portal');
    const cefDest = path.join(gtaPath, 'cef');

    if (fs.existsSync(cefLoadingSrc) || fs.existsSync(cefPortalSrc)) {
      if (!fs.existsSync(cefDest)) fs.mkdirSync(cefDest, { recursive: true });
      if (fs.existsSync(cefLoadingSrc)) {
        const loadingDest = path.join(cefDest, 'loading');
        if (fs.existsSync(loadingDest)) fs.rmSync(loadingDest, { recursive: true, force: true });
        fs.cpSync(cefLoadingSrc, loadingDest, { recursive: true });
        log('CEF: Loading screen installed to GTA/cef/loading/');
      }
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

  // Verify samp.exe is valid before launching
  if (fs.existsSync(sampExe) && !isValidPE(sampExe)) {
    log('WARNING: samp.exe is not a valid PE, restoring from backup...');
    const restored = restoreFromBackup(gtaPath);
    if (restored.length === 0 || !isValidPE(sampExe)) {
      log('Auto-reinstalling SA-MP client...');
      try { fs.unlinkSync(sampExe); } catch(e) {}
      try { if (fs.existsSync(sampExe + '.ug_backup')) fs.unlinkSync(sampExe + '.ug_backup'); } catch(e) {}
      await autoInstall(gtaPath, ['client']);
      if (!fs.existsSync(sampExe) || !isValidPE(sampExe)) {
        return { error: 'samp.exe je pokvaren! Reinstaliraj SA-MP rucno.' };
      }
    }
  }

  try {
    setupSampRegistry(gtaPath, srv.ip, srv.port, nickname);
    await new Promise(resolve => setTimeout(resolve, 500));

    const sampUrl = 'samp://' + srv.ip + ':' + srv.port;
    log('Launching via samp://: ' + sampUrl);
    shell.openExternal(sampUrl);

    log('Launch OK via samp://');
    return { success: true, message: 'SA-MP pokrenut! Spajanje na ' + srv.ip + ':' + srv.port + '...' };
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
  if (gtaPath) toggleCefFiles(gtaPath, !enabled);
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

  const restored = restoreFromBackup(gtaPath);
  if (restored.length > 0) {
    log('Auto-install: Restored clean files from backup: ' + restored.join(', '));
  }

  const status = getStatus(gtaPath);
  const sampExePath = path.join(gtaPath, 'samp.exe');
  if (fs.existsSync(sampExePath) && !isValidPE(sampExePath)) {
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
//  VERIFY & REPAIR - Force re-check all critical CEF files
// ============================================================
ipcMain.handle('verify-repair', async () => {
  const gtaPath = findGtaPath();
  if (!gtaPath) return { error: 'GTA:SA putanja nije pronadjena!' };

  log('VERIFY & REPAIR: Starting full verification...');

  // List of all critical files to check
  const criticalFiles = [
    { path: 'cef.asi', name: 'CEF Plugin (cef.asi)', component: 'cef_asi' },
    { path: 'cef/libcef.dll', name: 'CEF Engine (libcef.dll)', component: 'cef_runtime' },
    { path: 'cef/client.dll', name: 'CEF Client (client.dll)', component: 'cef_runtime' },
    { path: 'samp.exe', name: 'SA-MP Client (samp.exe)', component: 'client' },
  ];

  // Check ASI loader separately (either dsound.dll or dinput8.dll)
  const hasAsiLoader = fs.existsSync(path.join(gtaPath, 'dsound.dll')) || fs.existsSync(path.join(gtaPath, 'dinput8.dll'));

  const missing = [];
  const present = [];

  for (const file of criticalFiles) {
    const fullPath = path.join(gtaPath, file.path);
    if (fs.existsSync(fullPath)) {
      const size = fs.statSync(fullPath).size;
      present.push(file.name + ' (' + (size / 1024 / 1024).toFixed(1) + 'MB)');
    } else {
      missing.push(file.component);
      present.push(file.name + ' - FALI!');
    }
  }

  if (!hasAsiLoader) missing.push('asi_loader');

  log('VERIFY: Present: ' + present.join(', '));
  log('VERIFY: Missing components: ' + (missing.length > 0 ? missing.join(', ') : 'NONE'));

  if (missing.length === 0) {
    return { success: true, message: 'Svi fajlovi su prisutni!\n\n' + present.join('\n'), files: present };
  }

  // Download and install missing components
  log('VERIFY & REPAIR: Installing missing: ' + missing.join(', '));
  if (mainWindow) mainWindow.webContents.send('install-progress', {
    component: 'Popravka fajlova...', pct: 0, downloaded: 0, total: 0, speed: 0
  });

  try {
    await autoInstall(gtaPath, missing);
    log('VERIFY & REPAIR: Complete!');

    // Re-verify after install
    const stillMissing = [];
    for (const file of criticalFiles) {
      if (!fs.existsSync(path.join(gtaPath, file.path))) {
        stillMissing.push(file.name);
      }
    }

    if (stillMissing.length > 0) {
      return { error: 'Neki fajlovi i dalje fale: ' + stillMissing.join(', '), files: present };
    }

    return { success: true, message: 'Svi fajlovi uspjesno instalirani!', files: present };
  } catch (e) {
    log('VERIFY & REPAIR: Failed: ' + e.message);
    return { error: 'Popravka neuspjesna: ' + e.message };
  }
});

// ============================================================
//  APP EVENTS
// ============================================================
app.whenReady().then(() => {
  createWindow();
}).catch(err => { log('App ready FAILED: ' + err.message); });

app.on('window-all-closed', () => { app.quit(); });
app.on('activate', () => { if (BrowserWindow.getAllWindows().length === 0) createWindow(); });
