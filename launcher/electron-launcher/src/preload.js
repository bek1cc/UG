const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('launcherAPI', {
  getVersion: () => ipcRenderer.invoke('get-version'),
  getServerInfo: () => ipcRenderer.invoke('get-server-info'),
  getStatus: () => ipcRenderer.invoke('get-status'),
  getConfig: () => ipcRenderer.invoke('get-config'),
  browseGta: () => ipcRenderer.invoke('browse-gta'),
  launchGame: (nickname) => ipcRenderer.invoke('launch-game', nickname),
  autoInstall: () => ipcRenderer.invoke('auto-install'),
  openUrl: (url) => ipcRenderer.invoke('open-url', url),
  minimizeWindow: () => ipcRenderer.invoke('minimize-window'),
  maximizeWindow: () => ipcRenderer.invoke('maximize-window'),
  closeWindow: () => ipcRenderer.invoke('close-window'),

  // Event listeners
  onInstallProgress: (callback) => {
    ipcRenderer.on('install-progress', (event, data) => callback(data));
  },
  onInstallComplete: (callback) => {
    ipcRenderer.on('install-complete', (event, data) => callback(data));
  }
});
