// Auto-fix Electron binary installation
// This runs before npm start to ensure Electron can launch
const fs = require('fs');
const path = require('path');
const { execSync } = require('child_process');

const electronDir = path.join(__dirname, 'node_modules', 'electron');
const distDir = path.join(electronDir, 'dist');
const pathTxt = path.join(electronDir, 'path.txt');
const exePath = path.join(distDir, 'electron.exe');

// Check if everything is already OK
if (fs.existsSync(pathTxt) && fs.existsSync(exePath)) {
  process.exit(0);
}

console.log('[setup] Fixing Electron binary...');

// Try running Electron's install script first
try {
  execSync('node "' + path.join(electronDir, 'install.js') + '"', {
    timeout: 120000,
    stdio: 'ignore'
  });
} catch (e) {
  // Install script failed, try manual extraction
}

// Check if install script fixed it
if (fs.existsSync(pathTxt) && fs.existsSync(exePath)) {
  console.log('[setup] Electron OK!');
  process.exit(0);
}

// Manual extraction from cache
if (!fs.existsSync(exePath)) {
  const cacheDir = path.join(process.env.LOCALAPPDATA || '', 'electron', 'Cache');
  if (fs.existsSync(cacheDir)) {
    let zipPath = '';
    try {
      const dirs = fs.readdirSync(cacheDir);
      for (const d of dirs) {
        const p = path.join(cacheDir, d);
        try {
          if (fs.statSync(p).isDirectory()) {
            const files = fs.readdirSync(p);
            const zip = files.find(f => f.includes('win32-x64') && f.endsWith('.zip'));
            if (zip) {
              zipPath = path.join(p, zip);
              break;
            }
          }
        } catch (e) {}
      }
    } catch (e) {}
    
    if (zipPath && fs.existsSync(zipPath)) {
      console.log('[setup] Extracting Electron from cache...');
      if (!fs.existsSync(distDir)) fs.mkdirSync(distDir, { recursive: true });
      try {
        execSync('powershell -Command "Expand-Archive -Path \'' + zipPath + '\' -DestinationPath \'' + distDir + '\' -Force"', {
          timeout: 120000,
          stdio: 'ignore'
        });
      } catch (e) {
        console.log('[setup] PowerShell extraction failed, trying Node.js...');
        // Fallback: use adm-zip
        try {
          const AdmZip = require('adm-zip');
          const zip = new AdmZip(zipPath);
          zip.extractAllTo(distDir, true);
        } catch (e2) {
          console.log('[setup] Failed to extract Electron:', e2.message);
        }
      }
    }
  }
}

// Create path.txt
if (fs.existsSync(exePath)) {
  fs.writeFileSync(pathTxt, 'electron.exe', 'utf8');
  console.log('[setup] Electron fixed!');
} else {
  console.log('[setup] WARNING: Could not fix Electron. Try: rmdir /s /q node_modules && npm install');
}
