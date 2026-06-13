// ============================================================
//  UNICATE GAMING LAUNCHER - RENDERER
// ============================================================

const API = window.launcherAPI;

// ---------- STATE ----------
let config = null;
let serverInfo = null;
let statusData = null;
let currentPage = 'home';

// ---------- INIT ----------
async function init() {
  // Load config and status first (fast), then server info (slow - network)
  try {
    config = await API.getConfig();
    statusData = await API.getStatus();
  } catch (e) {
    console.error('Init error:', e);
    config = {
      SERVER_IP: '217.156.22.164', SERVER_PORT: 7774,
      SERVER_NAME: 'Unicate Gaming RPG',
      WEBSITE_URL: 'https://ug-ogc.com',
      DISCORD_URL: 'https://discord.gg/unicategaming',
      LAUNCHER_VERSION: '3.5.0'
    };
    statusData = { gta_path: null, has_samp: false, cef_ok: false, has_asi: false, ready: false, missing: ['client'] };
  }

  // Update UI immediately with cached data
  updateUI();
  setupEventListeners();
  setupInstallListeners();
  initParticles();

  // Server query is slow - do it in background, update UI when ready
  API.getServerInfo().then(info => {
    serverInfo = info;
    updateServerStatus();
  });

  // Refresh server info every 20 seconds
  setInterval(async () => {
    serverInfo = await API.getServerInfo();
    updateServerStatus();
  }, 20000);
}

// ---------- UPDATE UI ----------
function updateUI() {
  updateServerStatus();
  updateComponents();
  updateGtaPath();
  updateServerMode();
}

function updateServerStatus() {
  const heroDot = document.getElementById('heroDot');
  const heroStatus = document.getElementById('heroStatus');
  const topDot = document.getElementById('topDot');
  const topStatus = document.getElementById('topStatus');

  if (serverInfo && serverInfo.online) {
    heroDot.className = 'pulse-dot green';
    heroStatus.textContent = `Online - ${serverInfo.players}/${serverInfo.max_players} igraca`;
    heroStatus.style.color = 'var(--green)';
    topDot.className = 'pulse-dot green';
    topStatus.textContent = `Online ${serverInfo.players}/${serverInfo.max_players}`;

    document.getElementById('statPlayers').textContent = serverInfo.players;
    document.getElementById('statMax').textContent = serverInfo.max_players;
    document.getElementById('statMode').textContent = serverInfo.gamemode || 'RPG';
    document.getElementById('statPing').textContent = 'Online';
    document.getElementById('statPing').style.color = 'var(--green)';
    document.getElementById('serverPlayers').textContent = `${serverInfo.players}/${serverInfo.max_players}`;
    document.getElementById('serverMode').textContent = serverInfo.gamemode || 'RPG';
    document.getElementById('serverStatus').textContent = 'Online';
    document.getElementById('serverStatus').style.color = 'var(--green)';
    document.getElementById('bottomPlayers').textContent = `${serverInfo.players}/${serverInfo.max_players} igraca`;
  } else {
    heroDot.className = 'pulse-dot red';
    heroStatus.textContent = 'Server offline';
    heroStatus.style.color = 'var(--red)';
    topDot.className = 'pulse-dot orange';
    topStatus.textContent = 'Offline';
    document.getElementById('statPing').textContent = 'Offline';
    document.getElementById('statPing').style.color = 'var(--red)';
    document.getElementById('serverStatus').textContent = 'Offline';
    document.getElementById('serverStatus').style.color = 'var(--red)';
    document.getElementById('bottomPlayers').textContent = '0/1000 igraca';
  }
}

function updateComponents() {
  const isLocal = config && config.server_mode === 'local';

  const comps = [
    { dot: 'dotSamp', status: 'statusSamp', ok: statusData.has_samp, name: 'SA-MP Client' },
    { dot: 'dotAsi', status: 'statusAsi', ok: isLocal ? true : statusData.has_asi, name: 'ASI Loader' },
    { dot: 'dotCef', status: 'statusCef', ok: isLocal ? true : statusData.cef_ok, name: 'CEF Plugin' },
    { dot: 'dotRuntime', status: 'statusRuntime', ok: isLocal ? true : (statusData.gta_path && statusData.cef_ok), name: 'Chromium RT' }
  ];

  comps.forEach(c => {
    const dot = document.getElementById(c.dot);
    const st = document.getElementById(c.status);
    if (dot && st) {
      dot.className = `comp-dot ${c.ok ? 'ok' : 'missing'}`;
      st.className = `comp-status ${c.ok ? 'ok' : 'missing'}`;
      st.textContent = c.ok ? 'OK' : 'X';
    }
  });

  // Update right panel mini components
  const compMini = document.getElementById('compMini');
  if (compMini) {
    compMini.innerHTML = comps.map(c => `
      <div class="comp-mini-row">
        <span class="comp-dot ${c.ok ? 'ok' : 'missing'}"></span>
        <span>${c.name}</span>
        <span class="comp-status ${c.ok ? 'ok' : 'missing'}" style="margin-left:auto;">${c.ok ? 'OK' : 'X'}</span>
      </div>
    `).join('');
  }

  // Show/hide install button
  const installBtn = document.getElementById('btnInstall');
  if (installBtn && statusData) {
    installBtn.style.display = statusData.ready ? 'none' : 'flex';
  }
}

function updateServerMode() {
  if (!config) return;
  const mode = config.server_mode || 'production';
  const btnProd = document.getElementById('btnModeProduction');
  const btnLocal = document.getElementById('btnModeLocal');
  const hint = document.getElementById('modeHint');
  const panelIP = document.getElementById('panelServerIP');
  const serverIP = document.getElementById('serverIP');

  if (btnProd && btnLocal) {
    btnProd.classList.toggle('active', mode === 'production');
    btnLocal.classList.toggle('active', mode === 'local');
  }
  if (hint) {
    hint.textContent = mode === 'production' 
      ? 'Konektujes se na produkcijski server' 
      : 'Konektujes se na lokalni test server (127.0.0.1)';
    hint.style.color = mode === 'local' ? 'var(--orange)' : 'var(--dim)';
  }
  if (panelIP) panelIP.textContent = `${config.SERVER_IP}:${config.SERVER_PORT}`;
  if (serverIP) serverIP.textContent = `${config.SERVER_IP}:${config.SERVER_PORT}`;
}

function updateGtaPath() {
  const pathEl = document.getElementById('gtaPath');
  const rightPath = document.getElementById('rightGtaPath');
  const pathText = statusData.gta_path || 'Nije pronadjen!';
  const pathColor = statusData.gta_path ? 'var(--green)' : 'var(--red)';

  if (pathEl) { pathEl.textContent = pathText; pathEl.style.color = pathColor; }
  if (rightPath) { rightPath.textContent = pathText; rightPath.style.color = pathColor; }
}

function updateCefStatus(cefState) {
  const statusEl = document.getElementById('cefStatus');
  const warningEl = document.getElementById('cefWarning');
  if (!cefState || !statusEl) return;

  if (!cefState.has_files) {
    statusEl.textContent = 'CEF fajlovi nisu instalirani';
    statusEl.style.color = 'var(--dim)';
    if (warningEl) warningEl.style.display = 'none';
  } else if (cefState.enabled) {
    const parts = [];
    if (cefState.files.cefAsi) parts.push('cef.asi');
    if (cefState.files.dsound) parts.push('dsound.dll');
    if (cefState.files.dinput) parts.push('dinput8.dll');
    if (cefState.files.cefFolder) parts.push('cef/');
    statusEl.textContent = 'CEF UKLJUCEN - ' + parts.join(', ');
    statusEl.style.color = 'var(--green)';
    if (warningEl) warningEl.style.display = 'block';
  } else {
    const parts = [];
    if (cefState.files.cefAsiDis) parts.push('cef.asi.disabled');
    if (cefState.files.dsoundDis) parts.push('dsound.dll.disabled');
    if (cefState.files.dinputDis) parts.push('dinput8.dll.disabled');
    statusEl.textContent = 'CEF ISKLJUCEN - ' + (parts.length ? parts.join(', ') : 'nema fajlova');
    statusEl.style.color = 'var(--orange)';
    if (warningEl) warningEl.style.display = 'none';
  }
}

// ---------- EVENT LISTENERS ----------
function setupEventListeners() {
  // Window controls
  document.getElementById('btnMin')?.addEventListener('click', () => API.minimizeWindow());
  document.getElementById('btnMax')?.addEventListener('click', () => API.maximizeWindow());
  document.getElementById('btnClose')?.addEventListener('click', () => API.closeWindow());

  // Navigation
  document.querySelectorAll('.nav-btn, .sidebar-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      const page = btn.dataset.page;
      if (page) switchPage(page);
    });
  });

  // Connect button
  document.getElementById('btnConnect')?.addEventListener('click', async () => {
    const btn = document.getElementById('btnConnect');
    const nick = document.getElementById('inputNick')?.value?.trim();
    if (!nick || nick.length < 3) {
      alert('Unesi nickname (minimum 3 karaktera)!');
      return;
    }
    
    // Visual feedback
    btn.classList.add('launching');
    const origHTML = btn.querySelector('span').textContent;
    btn.querySelector('span').textContent = 'POKRECEM SA-MP...';
    btn.style.pointerEvents = 'none';
    
    const timeout = new Promise(resolve => setTimeout(() => resolve({ error: 'Launcher ne odgovara! Pokreni ponovo launcher.' }), 30000));
    const result = await Promise.race([API.launchGame(nick), timeout]);
    
    btn.classList.remove('launching');
    btn.querySelector('span').textContent = origHTML;
    btn.style.pointerEvents = '';
    
    if (result && result.error) {
      console.error('Launch error:', result.error);
      alert('GRESKA: ' + result.error);
    } else if (result && result.success) {
      console.log('Game launched successfully (PID: ' + result.pid + ')');
      // Show brief success notification
      const hint = document.querySelector('.connect-hint');
      if (hint) {
        const origHint = hint.textContent;
        const origColor = hint.style.color;
        hint.textContent = result.message || 'SA-MP pokrenut! Konektovanje na server...';
        hint.style.color = 'var(--green)';
        setTimeout(() => {
          hint.textContent = origHint;
          hint.style.color = origColor || '';
        }, 5000);
      }
    }
  });

  // Browse buttons
  document.getElementById('btnBrowse')?.addEventListener('click', browseGta);
  document.getElementById('btnBrowse2')?.addEventListener('click', browseGta);

  // Install button
  document.getElementById('btnInstall')?.addEventListener('click', async () => {
    const btn = document.getElementById('btnInstall');
    btn.disabled = true;
    btn.innerHTML = `<svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" stroke-width="2" class="spin"><path d="M21 12a9 9 0 11-6.219-8.56"/></svg> INSTALIRAM...`;

    const result = await API.autoInstall();
    if (result && result.error) {
      alert('Greska: ' + result.error);
    }

    // Refresh status
    statusData = await API.getStatus();
    updateComponents();
    updateGtaPath();

    btn.disabled = false;
    btn.innerHTML = `<svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" stroke-width="2"><path d="M21 15v4a2 2 0 01-2 2H5a2 2 0 01-2-2v-4"/><polyline points="7 10 12 15 17 10"/><line x1="12" y1="15" x2="12" y2="3"/></svg> AUTO-INSTALACIJA`;
  });

  // Verify & Repair button
  document.getElementById('btnVerify')?.addEventListener('click', async () => {
    const btn = document.getElementById('btnVerify');
    btn.disabled = true;
    btn.innerHTML = `<svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" stroke-width="2" class="spin"><path d="M21 12a9 9 0 11-6.219-8.56"/></svg> PROVJERAVAM...`;

    const result = await API.verifyRepair();
    if (result && result.error) {
      alert('Greska: ' + result.error);
    } else if (result && result.success) {
      alert(result.message);
    }

    // Refresh status
    statusData = await API.getStatus();
    updateComponents();
    updateGtaPath();

    btn.disabled = false;
    btn.innerHTML = `<svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" stroke-width="2"><path d="M9 12l2 2 4-4"/><circle cx="12" cy="12" r="10"/></svg> PROVJERI FAJLOVE`;
  });

  // Server mode buttons
  document.querySelectorAll('.mode-btn').forEach(btn => {
    btn.addEventListener('click', async () => {
      const mode = btn.dataset.mode;
      if (!mode) return;
      const result = await API.setServerMode(mode);
      if (result && result.success) {
        config.SERVER_IP = result.ip;
        config.SERVER_PORT = result.port;
        config.SERVER_NAME = result.name;
        config.server_mode = result.mode;
        updateServerMode();
        // Refresh server info for new IP
        serverInfo = await API.getServerInfo();
        updateServerStatus();
      }
    });
  });

  // Social buttons
  document.getElementById('btnDiscord')?.addEventListener('click', () => API.openUrl(config?.DISCORD_URL || 'https://discord.gg/unicategaming'));
  document.getElementById('btnWebsite')?.addEventListener('click', () => API.openUrl(config?.WEBSITE_URL || 'https://ug-ogc.com'));

  // CEF Toggle
  const cefToggle = document.getElementById('cefToggle');
  if (cefToggle) {
    // Load current CEF state
    API.getCefState().then(cefState => {
      cefToggle.checked = cefState.setting;
      updateCefStatus(cefState);
    });

    cefToggle.addEventListener('change', async () => {
      const enabled = cefToggle.checked;
      const result = await API.toggleCef(enabled);
      if (result && result.success) {
        const cefState = await API.getCefState();
        updateCefStatus(cefState);
      }
    });
  }

  // Nickname sync
  document.getElementById('inputNick')?.addEventListener('input', (e) => {
    const rightNick = document.getElementById('rightNick');
    if (rightNick) rightNick.textContent = e.target.value;
  });
}

async function browseGta() {
  const result = await API.browseGta();
  if (result) {
    if (result.error) {
      alert(result.error);
    } else if (result.path) {
      statusData = result.status;
      updateGtaPath();
      updateComponents();
    }
  }
}

function switchPage(page) {
  currentPage = page;
  document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.nav-btn, .sidebar-btn').forEach(b => b.classList.remove('active'));

  const pageEl = document.getElementById(`page-${page}`);
  if (pageEl) pageEl.classList.add('active');

  document.querySelectorAll(`[data-page="${page}"]`).forEach(b => b.classList.add('active'));
}

// ---------- INSTALL LISTENERS ----------
function setupInstallListeners() {
  API.onInstallProgress((data) => {
    const container = document.getElementById('installProgress');
    const bar = document.getElementById('progressBar');
    const glow = document.getElementById('progressGlow');
    const text = document.getElementById('progressText');
    const detail = document.getElementById('progressDetail');

    if (container) container.style.display = 'block';
    if (bar) bar.style.width = data.pct + '%';
    if (glow) glow.style.left = data.pct + '%';
    if (text) text.textContent = `Skidam ${data.component}... ${data.pct.toFixed(1)}%`;

    if (data.total > 0) {
      const dlMB = (data.downloaded / 1024 / 1024).toFixed(1);
      const totalMB = (data.total / 1024 / 1024).toFixed(1);
      const speedKB = (data.speed).toFixed(0);
      if (detail) detail.textContent = `${dlMB} / ${totalMB} MB | ${speedKB} KB/s`;
    }
  });

  API.onInstallComplete(() => {
    const text = document.getElementById('progressText');
    if (text) text.textContent = 'Instalacija zavrsena!';
    const detail = document.getElementById('progressDetail');
    if (detail) detail.textContent = '';

    // Refresh status after a short delay
    setTimeout(async () => {
      statusData = await API.getStatus();
      updateComponents();
      updateGtaPath();
    }, 1000);
  });
}

// ---------- PARTICLES BACKGROUND ----------
function initParticles() {
  const canvas = document.getElementById('particles-bg');
  if (!canvas) return;

  const ctx = canvas.getContext('2d');
  let particles = [];
  let animFrame;

  function resize() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
  }

  resize();
  window.addEventListener('resize', resize);

  class Particle {
    constructor() {
      this.reset();
    }

    reset() {
      this.x = Math.random() * canvas.width;
      this.y = Math.random() * canvas.height;
      this.vx = (Math.random() - 0.5) * 0.3;
      this.vy = -Math.random() * 0.4 - 0.1;
      this.size = Math.random() * 2.5 + 0.5;
      this.life = Math.random() * 400 + 200;
      this.maxLife = this.life;
      this.hue = Math.random() * 40 + 190; // Blue range
    }

    update() {
      this.x += this.vx;
      this.y += this.vy;
      this.life--;

      if (this.life <= 0 || this.x < 0 || this.x > canvas.width || this.y < 0 || this.y > canvas.height) {
        this.reset();
      }
    }

    draw() {
      const alpha = (this.life / this.maxLife) * 0.6;
      const s = this.size * (this.life / this.maxLife);
      if (s < 0.3) return;

      ctx.beginPath();
      ctx.arc(this.x, this.y, s, 0, Math.PI * 2);
      ctx.fillStyle = `hsla(${this.hue}, 80%, 60%, ${alpha})`;
      ctx.fill();

      // Glow
      ctx.beginPath();
      ctx.arc(this.x, this.y, s * 3, 0, Math.PI * 2);
      ctx.fillStyle = `hsla(${this.hue}, 80%, 60%, ${alpha * 0.15})`;
      ctx.fill();
    }
  }

  // Create particles (reduced from 50 to 30 for performance)
  for (let i = 0; i < 30; i++) {
    particles.push(new Particle());
  }

  // Draw grid (cached - only redraw on resize)
  let gridCanvas = null;
  function drawGrid() {
    if (!gridCanvas || gridCanvas.width !== canvas.width || gridCanvas.height !== canvas.height) {
      gridCanvas = document.createElement('canvas');
      gridCanvas.width = canvas.width;
      gridCanvas.height = canvas.height;
      const gctx = gridCanvas.getContext('2d');
      gctx.strokeStyle = 'rgba(6, 13, 24, 0.5)';
      gctx.lineWidth = 0.5;
      for (let x = 0; x < canvas.width; x += 80) {
        gctx.beginPath(); gctx.moveTo(x, 0); gctx.lineTo(x, canvas.height); gctx.stroke();
      }
      for (let y = 0; y < canvas.height; y += 80) {
        gctx.beginPath(); gctx.moveTo(0, y); gctx.lineTo(canvas.width, y); gctx.stroke();
      }
    }
    ctx.drawImage(gridCanvas, 0, 0);
  }

  function animate() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    drawGrid();

    particles.forEach(p => {
      p.update();
      p.draw();
    });

    // Draw connections between nearby particles
    for (let i = 0; i < particles.length; i++) {
      for (let j = i + 1; j < particles.length; j++) {
        const dx = particles[i].x - particles[j].x;
        const dy = particles[i].y - particles[j].y;
        const dist = Math.sqrt(dx * dx + dy * dy);
        if (dist < 120) {
          const alpha = (1 - dist / 120) * 0.08;
          ctx.beginPath();
          ctx.moveTo(particles[i].x, particles[i].y);
          ctx.lineTo(particles[j].x, particles[j].y);
          ctx.strokeStyle = `rgba(0, 153, 255, ${alpha})`;
          ctx.lineWidth = 0.5;
          ctx.stroke();
        }
      }
    }

    animFrame = requestAnimationFrame(animate);
  }

  animate();
}

// ---------- SPIN ANIMATION FOR LOADING ----------
const style = document.createElement('style');
style.textContent = `
  @keyframes spin { from { transform: rotate(0deg); } to { transform: rotate(360deg); } }
  .spin { animation: spin 1s linear infinite; }
`;
document.head.appendChild(style);

// ---------- START ----------
document.addEventListener('DOMContentLoaded', init);
