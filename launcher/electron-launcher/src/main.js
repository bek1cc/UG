const { app, BrowserWindow, ipcMain, shell, dialog } = require('electron');
const path = require('path');
const fs = require('fs');
const https = require('https');
const http = require('http');
const { exec, execFile, spawn } = require('child_process');
const dgram = require('dgram');
const AdmZip = require('adm-zip');

// ============================================================
//  ELECTRON COMPATIBILITY FIXES (MUST BE BEFORE app.whenReady)
// ============================================================
app.commandLine.appendSwitch('no-sandbox');
app.commandLine.appendSwitch('disable-gpu-sandbox');
app.commandLine.appendSwitch('disable-software-rasterizer');
app.disableHardwareAcceleration();

// ============================================================
//  DEBUG LOG TO FILE
// ============================================================
const LOG_FILE = path.join(path.dirname(app.getPath('exe')), 'launcher_debug.log');
function log(msg) {
  const ts = new Date().toISOString();
  const line = `[${ts}] ${msg}\n`;
  console.log(line.trim());
  try { fs.appendFileSync(LOG_FILE, line); } catch(e) {}
}

// Clear old log
try { fs.writeFileSync(LOG_FILE, '=== Unicate Gaming Launcher Debug Log ===\n'); } catch(e) {}

log('Starting Unicate Gaming Launcher v3.2');
log('Electron: ' + process.versions.electron);
log('Node: ' + process.versions.node);
log('Chrome: ' + process.versions.chrome);
log('Platform: ' + process.platform + ' ' + process.arch);
log('__dirname: ' + __dirname);
log('LAUNCHER_DIR: ' + path.dirname(app.getPath('exe')));

// ============================================================
//  CRASH PROTECTION
// ============================================================
process.on('uncaughtException', (err) => {
  log('[UNCAUGHT ERROR] ' + err.message);
  log(err.stack);
});

process.on('unhandledRejection', (err) => {
  log('[UNHANDLED REJECTION] ' + err);
});

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
const LAUNCHER_VERSION = '3.2.0';

const OMP_CEF_ASI_URL = 'https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/cef.asi';
const OMP_CEF_CLIENT_URL = 'https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/client-files-v1.2.0.zip';
const ASI_LOADER_URL = 'https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/v9.7.2/Ultimate-ASI-Loader.zip';
const SAMP_CLIENT_URL = 'https://files.sa-mp.com/sa-mp-0.3.7-R4-install.exe';

const LAUNCHER_DIR = path.dirname(app.getPath('exe'));
const SETTINGS_FILE = path.join(LAUNCHER_DIR, 'settings.json');

let mainWindow = null;

log('LAUNCHER_DIR: ' + LAUNCHER_DIR);

// ============================================================
//  SETTINGS
// ============================================================
function loadSettings() {
  try {
    if (fs.existsSync(SETTINGS_FILE)) {
      return JSON.parse(fs.readFileSync(SETTINGS_FILE, 'utf8'));
    }
  } catch (e) {
    log('Error loading settings: ' + e.message);
  }
  return {};
}

function saveSettings(data) {
  try {
    fs.writeFileSync(SETTINGS_FILE, JSON.stringify(data, null, 2));
  } catch (e) {
    log('Error saving settings: ' + e.message);
  }
}

// ============================================================
//  GTA:SA PATH DETECTION
// ============================================================
function findGtaPath() {
  const settings = loadSettings();
  if (settings.gta_path && fs.existsSync(path.join(settings.gta_path, 'gta_sa.exe'))) {
    return settings.gta_path;
  }
  const common = [
    'C:\\Program Files (x86)\\Rockstar Games\\GTA San Andreas',
    'D:\\GTA San Andreas', 'D:\\GTA SA', 'D:\\Games\\GTA SA',
    'D:\\SAMP TEST SERVER',
    'E:\\GTA San Andreas', 'E:\\GTA SA',
    'C:\\GTA San Andreas', 'C:\\GTA SA'
  ];
  for (const p of common) {
    if (fs.existsSync(path.join(p, 'gta_sa.exe'))) return p;
  }
  return null;
}

// ============================================================
//  STATUS CHECK
// ============================================================
function checkSampVersion(gtaPath) {
  const sampExe = path.join(gtaPath, 'samp.exe');
  if (!fs.existsSync(sampExe)) return { installed: false, version: null };
  try {
    // Read SA-MP version from the executable metadata or file size
    const stats = fs.statSync(sampExe);
    const size = stats.size;
    log('samp.exe size: ' + size + ' bytes');
    // R4 samp.exe is ~2.5MB, R2 is ~2.3MB
    // We can't reliably detect version from size alone, so just check if it exists
    return { installed: true, version: 'installed', size: size };
  } catch (e) {
    return { installed: true, version: 'unknown' };
  }
}

function getStatus(gtaPath) {
  const settings = loadSettings();
  const mode = settings.server_mode || 'production';
  const isLocal = mode === 'local';

  const s = {
    gta_path: gtaPath,
    has_samp: false,
    samp_version_ok: false,
    cef_ok: false,
    cef_msg: '-',
    has_asi: false,
    ready: false,
    missing: [],
    server_mode: mode
  };
  if (gtaPath && fs.existsSync(gtaPath)) {
    const sampInfo = checkSampVersion(gtaPath);
    s.has_samp = sampInfo.installed;
    s.samp_version_ok = sampInfo.installed; // Will be verified on launch
    
    const hasAsi = fs.existsSync(path.join(gtaPath, 'cef.asi'));
    const hasCef = fs.existsSync(path.join(gtaPath, 'cef'));
    s.has_asi = fs.existsSync(path.join(gtaPath, 'dsound.dll')) || fs.existsSync(path.join(gtaPath, 'dinput8.dll'));
    if (hasAsi && hasCef) { s.cef_ok = true; s.cef_msg = 'CEF OK'; }
    else if (hasAsi) { s.cef_msg = 'Fali cef/ folder'; }
    else if (hasCef) { s.cef_msg = 'Fali cef.asi'; }
    else { s.cef_msg = 'Nije instaliran'; }
    if (!s.has_samp) s.missing.push('client');
    // CEF and ASI are only required for production mode
    // Local test mode only needs samp.exe
    if (!isLocal) {
      if (!hasAsi) s.missing.push('cef_asi');
      if (!hasCef) s.missing.push('cef_runtime');
      if (!s.has_asi) s.missing.push('asi_loader');
      s.ready = s.has_samp && s.cef_ok && s.has_asi;
    } else {
      // Local mode: ready if samp.exe exists
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
      const timeout = setTimeout(() => { try { sock.close(); } catch(e) {} resolve({ online: false }); }, 5000);
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
        if (data.length < 11) { resolve({ online: false }); return; }
        let off = 11;
        const pwlen = data.readUInt16LE(off); off += 2 + pwlen;
        const players = data.readUInt16LE(off); off += 2;
        const maxp = data.readUInt16LE(off); off += 2;
        const nlen = data.readUInt32LE(off); off += 4;
        const name = data.slice(off, off + nlen).toString('latin1'); off += nlen;
        const mlen = data.readUInt32LE(off); off += 4;
        const mode = data.slice(off, off + mlen).toString('latin1');
        resolve({ players, max_players: maxp, name, gamemode: mode, online: true });
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

    mod.get(url, { timeout: 60000, headers: { 'User-Agent': 'UnicateGamingLauncher/3.2' } }, (response) => {
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
      // Download and install SA-MP 0.3.7-R4 client
      const tmpExe = path.join(LAUNCHER_DIR, 'tmp_samp_install.exe');
      log('Downloading SA-MP 0.3.7-R4 client...');
      await downloadFile(SAMP_CLIENT_URL, tmpExe, (pct, dl, total, speed) => {
        if (mainWindow) mainWindow.webContents.send('install-progress', {
          component: 'SA-MP R4 Client', pct, downloaded: dl, total, speed
        });
      });
      
      // Run the installer silently - extract samp.dll and samp.exe to GTA folder
      // SA-MP installer is NSIS-based, we can extract with 7zip or just run it
      log('SA-MP R4 downloaded, running installer...');
      
      // Try to run installer silently
      try {
        const { execSync } = require('child_process');
        // Run the installer with /S for silent install and /D for destination
        execSync(`"${tmpExe}" /S /D=${gtaPath}`, { 
          timeout: 60000,
          windowsHide: true 
        });
        log('SA-MP R4 installed silently');
      } catch (e) {
        log('Silent install failed, launching installer for user: ' + e.message);
        // If silent install fails, just run the installer normally
        const child = spawn(tmpExe, [], { detached: true, stdio: 'ignore' });
        child.unref();
        // Wait a bit for user to install
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
          log('ASI Loader: extracting ' + fileName);
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
    
    // Set PlayerName in SA-MP registry
    const regCmd1 = `reg add "HKCU\\Software\\SAMP" /v "PlayerName" /t REG_SZ /d "${nickname}" /f`;
    log('Registry: Setting PlayerName = ' + nickname);
    execSync(regCmd1, { windowsHide: true });
    
    // Set LastServer
    const serverAddr = `${ip}:${port}`;
    const regCmd2 = `reg add "HKCU\\Software\\SAMP" /v "LastServer" /t REG_SZ /d "${serverAddr}" /f`;
    log('Registry: Setting LastServer = ' + serverAddr);
    execSync(regCmd2, { windowsHide: true });
    
    // Set GTA path in SA-MP registry (important for finding gta_sa.exe)
    const regCmd3 = `reg add "HKCU\\Software\\SAMP" /v "gta_sa_exe" /t REG_SZ /d "${gtaPath}\\gta_sa.exe" /f`;
    log('Registry: Setting gta_sa_exe = ' + gtaPath + '\\gta_sa.exe');
    execSync(regCmd3, { windowsHide: true });
    
    log('SA-MP registry setup complete');
    return true;
  } catch (err) {
    log('Registry setup error: ' + err.message);
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

    log('Creating window...');
    log('index.html: ' + indexPath + ' exists: ' + fs.existsSync(indexPath));
    log('preload.js: ' + preloadPath + ' exists: ' + fs.existsSync(preloadPath));

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
      show: true,
      webPreferences: {
        nodeIntegration: false,
        contextIsolation: true,
        preload: preloadPath,
        sandbox: false
      }
    });

    log('Loading index.html...');
    mainWindow.loadFile(indexPath).then(() => {
      log('index.html loaded successfully');
    }).catch(err => {
      log('FAILED to load index.html: ' + err.message);
    });

    mainWindow.webContents.on('console-message', (event, level, message) => {
      log('[RENDERER] ' + message);
    });

    mainWindow.webContents.on('render-process-gone', (event, details) => {
      log('Renderer process GONE! reason: ' + details.reason + ' exitCode: ' + details.exitCode);
    });

    mainWindow.on('closed', () => {
      log('Window closed');
      mainWindow = null;
    });

    log('Window created successfully');
  } catch (err) {
    log('ERROR creating window: ' + err.message);
    log(err.stack);
  }
}

// ============================================================
//  IPC HANDLERS
// ============================================================
ipcMain.handle('get-version', () => LAUNCHER_VERSION);

ipcMain.handle('get-server-info', async () => {
  const srv = getActiveServer();
  log('Querying server: ' + srv.ip + ':' + srv.port + ' mode=' + srv.mode);
  const result = await querySampServer(srv.ip, srv.port);
  log('Query result: online=' + result.online + (result.online ? ' players=' + result.players : ''));
  
  // If SAMP query fails on local mode, try TCP connect check as fallback
  if (!result.online && srv.mode === 'local') {
    log('Trying TCP fallback for local server...');
    const tcpCheck = await new Promise((resolve) => {
      const net = require('net');
      const sock = net.createConnection(srv.port, srv.ip);
      sock.setTimeout(3000);
      sock.on('connect', () => { sock.destroy(); resolve(true); });
      sock.on('error', (e) => { log('TCP fallback error: ' + e.message); sock.destroy(); resolve(false); });
      sock.on('timeout', () => { sock.destroy(); resolve(false); });
    });
    if (tcpCheck) {
      log('TCP fallback: server is ONLINE');
      return { online: true, players: 0, max_players: 200, name: srv.name, gamemode: 'RPG (Local Test)', local_fallback: true };
    }
    log('TCP fallback: server is OFFLINE');
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
      return { path: selected, status: getStatus(selected) };
    }
    return { error: 'U ovom folderu nije pronadjen gta_sa.exe!' };
  }
  return null;
});

ipcMain.handle('launch-game', async (event, nickname) => {
  const gtaPath = findGtaPath();
  if (!gtaPath) return { error: 'GTA:SA putanja nije pronadjena! Idi u Podesavanja i izaberi GTA:SA folder.' };

  const sampExe = path.join(gtaPath, 'samp.exe');
  if (!fs.existsSync(sampExe)) return { error: 'samp.exe nije pronadjen u ' + gtaPath + '! Instaliraj SA-MP klijent (R4).' };

  // Validate nickname
  if (!nickname || nickname.length < 3) {
    nickname = 'Unicate_Player';
  }
  // SA-MP nickname rules: only alphanumeric, underscore, and brackets
  nickname = nickname.replace(/[^a-zA-Z0-9_\[\]]/g, '_');
  if (nickname.length > 20) nickname = nickname.substring(0, 20);

  const srv = getActiveServer();

  // For local test mode, only samp.exe is needed
  // For production mode, check all components
  if (srv.mode === 'production') {
    const status = getStatus(gtaPath);
    if (!status.ready) return { error: 'Nisu sve komponente instalirane! Pokreni auto-instalaciju u Podesavanja.' };
  }

  log('=== LAUNCHING GAME ===');
  log('GTA Path: ' + gtaPath);
  log('samp.exe: ' + sampExe);
  log('Server: ' + srv.ip + ':' + srv.port);
  log('Nickname: ' + nickname);
  log('Mode: ' + srv.mode);

  try {
    // Step 1: Setup SA-MP registry with nickname and server info
    log('Step 1: Setting up SA-MP registry...');
    setupSampRegistry(gtaPath, srv.ip, srv.port, nickname);
    
    // Step 2: Launch samp.exe
    // SA-MP 0.3.7-R4 accepts: samp.exe <IP> <PORT> <NICKNAME>
    log('Step 2: Spawning samp.exe...');
    
    const args = [srv.ip, srv.port.toString(), nickname];
    log('Launch args: ' + JSON.stringify(args));
    log('Working dir: ' + gtaPath);
    
    const child = spawn(sampExe, args, {
      cwd: gtaPath,
      detached: true,
      stdio: 'ignore'
    });
    child.unref();
    
    log('Launch SUCCESS (PID: ' + child.pid + ')');
    return { success: true, pid: child.pid };
  } catch (err) {
    log('Launch FAILED: ' + err.message);
    log(err.stack);
    return { error: 'Greska pri pokretanju: ' + err.message };
  }
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

ipcMain.handle('open-url', (event, url) => {
  shell.openExternal(url);
});

ipcMain.handle('minimize-window', () => {
  if (mainWindow) mainWindow.minimize();
});

ipcMain.handle('maximize-window', () => {
  if (mainWindow) {
    if (mainWindow.isMaximized()) mainWindow.unmaximize();
    else mainWindow.maximize();
  }
});

ipcMain.handle('close-window', () => {
  if (mainWindow) mainWindow.close();
});

// ============================================================
//  APP EVENTS
// ============================================================
log('Waiting for app.whenReady...');
app.whenReady().then(() => {
  log('App ready, creating window...');
  createWindow();
}).catch(err => {
  log('App ready FAILED: ' + err.message);
  log(err.stack);
});

app.on('window-all-closed', () => {
  log('All windows closed, quitting...');
  app.quit();
});

app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) createWindow();
});

app.on('before-quit', () => {
  log('App about to quit...');
});
