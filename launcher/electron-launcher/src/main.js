const { app, BrowserWindow, ipcMain, shell, dialog } = require('electron');
const path = require('path');
const fs = require('fs');
const https = require('https');
const http = require('http');
const { exec, execFile } = require('child_process');
const dgram = require('dgram');
const AdmZip = require('adm-zip');

// ============================================================
//  CONFIG
// ============================================================
const SERVER_IP = '135.125.156.197';
const SERVER_PORT = 7777;
const SERVER_NAME = 'Unicate Gaming RPG';
const WEBSITE_URL = 'https://ug-ogc.com';
const DISCORD_URL = 'https://discord.gg/unicategaming';
const LAUNCHER_VERSION = '3.0.0';

const OMP_CEF_ASI_URL = 'https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/cef.asi';
const OMP_CEF_CLIENT_URL = 'https://github.com/aurora-mp/omp-cef/releases/download/v1.2.0/client-files-v1.2.0.zip';
const ASI_LOADER_URL = 'https://github.com/ThirteenAG/Ultimate-ASI-Loader/releases/download/v4.76/dsound.zip';

const LAUNCHER_DIR = path.dirname(app.getPath('exe'));
const SETTINGS_FILE = path.join(LAUNCHER_DIR, 'settings.json');

let mainWindow = null;

// ============================================================
//  SETTINGS
// ============================================================
function loadSettings() {
  try {
    if (fs.existsSync(SETTINGS_FILE)) {
      return JSON.parse(fs.readFileSync(SETTINGS_FILE, 'utf8'));
    }
  } catch (e) {}
  return {};
}

function saveSettings(data) {
  try {
    fs.writeFileSync(SETTINGS_FILE, JSON.stringify(data, null, 2));
  } catch (e) {}
}

// ============================================================
//  GTA:SA PATH DETECTION
// ============================================================
function findGtaPath() {
  const settings = loadSettings();
  if (settings.gta_path && fs.existsSync(path.join(settings.gta_path, 'gta_sa.exe'))) {
    return settings.gta_path;
  }
  // Common paths
  const common = [
    'C:\\Program Files (x86)\\Rockstar Games\\GTA San Andreas',
    'D:\\GTA San Andreas', 'D:\\GTA SA', 'D:\\Games\\GTA SA',
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
function getStatus(gtaPath) {
  const s = {
    gta_path: gtaPath,
    has_samp: false,
    cef_ok: false,
    cef_msg: '-',
    has_asi: false,
    ready: false,
    missing: []
  };
  if (gtaPath && fs.existsSync(gtaPath)) {
    s.has_samp = fs.existsSync(path.join(gtaPath, 'samp.exe'));
    const hasAsi = fs.existsSync(path.join(gtaPath, 'cef.asi'));
    const hasCef = fs.existsSync(path.join(gtaPath, 'cef'));
    s.has_asi = fs.existsSync(path.join(gtaPath, 'dsound.dll'));
    if (hasAsi && hasCef) { s.cef_ok = true; s.cef_msg = 'CEF OK'; }
    else if (hasAsi) { s.cef_msg = 'Fali cef/ folder'; }
    else if (hasCef) { s.cef_msg = 'Fali cef.asi'; }
    else { s.cef_msg = 'Nije instaliran'; }
    if (!s.has_samp) s.missing.push('client');
    if (!hasAsi) s.missing.push('cef_asi');
    if (!hasCef) s.missing.push('cef_runtime');
    if (!s.has_asi) s.missing.push('asi_loader');
    s.ready = s.has_samp && s.cef_ok && s.has_asi;
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
      const timeout = setTimeout(() => { sock.close(); resolve({ online: false }); }, 3000);
      const ipParts = ip.split('.').map(Number);
      const pkt = Buffer.concat([
        Buffer.from('SAMP'),
        Buffer.from([ipParts[0], ipParts[1], ipParts[2], ipParts[3]]),
        Buffer.from([port & 0xFF, (port >> 8) & 0xFF]),
        Buffer.from('i')
      ]);
      sock.send(pkt, port, ip, (err) => {
        if (err) { clearTimeout(timeout); sock.close(); resolve({ online: false }); }
      });
      sock.on('message', (data) => {
        clearTimeout(timeout);
        sock.close();
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
    } catch (e) {
      resolve({ online: false });
    }
  });
}

// ============================================================
//  DOWNLOAD WITH PROGRESS
// ============================================================
function downloadFile(url, dest, onProgress) {
  return new Promise((resolve, reject) => {
    const file = fs.createWriteStream(dest);
    const mod = url.startsWith('https') ? https : http;
    let totalBytes = 0;
    let downloadedBytes = 0;
    let startTime = Date.now();

    mod.get(url, { timeout: 30000 }, (response) => {
      // Handle redirects
      if (response.statusCode >= 300 && response.statusCode < 400 && response.headers.location) {
        file.close();
        fs.unlinkSync(dest);
        return downloadFile(response.headers.location, dest, onProgress).then(resolve).catch(reject);
      }
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
        resolve(dest);
      });
    }).on('error', (err) => {
      file.close();
      try { fs.unlinkSync(dest); } catch (e) {}
      reject(err);
    });
  });
}

// ============================================================
//  AUTO-INSTALL
// ============================================================
async function autoInstall(gtaPath, missing) {
  for (const comp of missing) {
    if (comp === 'asi_loader') {
      const tmpZip = path.join(LAUNCHER_DIR, 'tmp_asi.zip');
      await downloadFile(ASI_LOADER_URL, tmpZip, (pct, dl, total, speed) => {
        if (mainWindow) mainWindow.webContents.send('install-progress', {
          component: 'ASI Loader', pct, downloaded: dl, total, speed
        });
      });
      // Extract dsound.dll
      const zip = new AdmZip(tmpZip);
      const entry = zip.getEntries().find(e => e.entryName.toLowerCase().endsWith('dsound.dll'));
      if (entry) {
        fs.writeFileSync(path.join(gtaPath, 'dsound.dll'), entry.getData());
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
      // Extract all
      const zip = new AdmZip(tmpZip);
      zip.extractAllTo(gtaPath, true);
      try { fs.unlinkSync(tmpZip); } catch (e) {}
    }
  }
  if (mainWindow) mainWindow.webContents.send('install-complete', {});
}

// ============================================================
//  CREATE WINDOW
// ============================================================
function createWindow() {
  // Resolve paths correctly - __dirname is the src/ folder
  const srcDir = __dirname;
  const indexPath = path.join(srcDir, 'index.html');
  const preloadPath = path.join(srcDir, 'preload.js');
  // Try both .ico and .png for icon
  const iconIco = path.join(srcDir, 'ug_icon.ico');
  const iconPng = path.join(srcDir, 'ug_logo.png');
  let iconPath = null;
  if (fs.existsSync(iconIco)) iconPath = iconIco;
  else if (fs.existsSync(iconPng)) iconPath = iconPng;

  console.log('[LAUNCHER] __dirname:', srcDir);
  console.log('[LAUNCHER] index.html exists:', fs.existsSync(indexPath));
  console.log('[LAUNCHER] icon path:', iconPath);
  console.log('[LAUNCHER] icon exists:', iconPath ? fs.existsSync(iconPath) : false);

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
    webPreferences: {
      nodeIntegration: false,
      contextIsolation: true,
      preload: preloadPath
    }
  });

  // Open DevTools for debugging (remove in production)
  mainWindow.webContents.openDevTools();

  // Load the index.html
  mainWindow.loadFile(indexPath).catch(err => {
    console.error('[LAUNCHER] Failed to load index.html:', err);
  });

  // Log any console errors from renderer
  mainWindow.webContents.on('console-message', (event, level, message) => {
    console.log('[RENDERER]', message);
  });

  mainWindow.on('closed', () => {
    mainWindow = null;
  });
}

// ============================================================
//  IPC HANDLERS
// ============================================================
ipcMain.handle('get-version', () => LAUNCHER_VERSION);

ipcMain.handle('get-server-info', async () => {
  return await querySampServer(SERVER_IP, SERVER_PORT);
});

ipcMain.handle('get-status', () => {
  const gtaPath = findGtaPath();
  return getStatus(gtaPath);
});

ipcMain.handle('get-config', () => {
  return { SERVER_IP, SERVER_PORT, SERVER_NAME, WEBSITE_URL, DISCORD_URL, LAUNCHER_VERSION };
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
  if (!gtaPath) return { error: 'GTA:SA putanja nije pronadjena!' };
  const status = getStatus(gtaPath);
  if (!status.ready) return { error: 'Nisu sve komponente instalirane! Pokreni auto-instalaciju.' };

  const sampExe = path.join(gtaPath, 'samp.exe');
  return new Promise((resolve) => {
    execFile(sampExe, [SERVER_IP, SERVER_PORT.toString(), nickname], { cwd: gtaPath }, (err) => {
      if (err) resolve({ error: 'Greska pri pokretanju: ' + err.message });
      else resolve({ success: true });
    });
  });
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
app.whenReady().then(createWindow);
app.on('window-all-closed', () => app.quit());
app.on('activate', () => { if (BrowserWindow.getAllWindows().length === 0) createWindow(); });
