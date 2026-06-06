// ============================================================
//  UG TABLET CEF - JavaScript kontroler
//  Komunikacija: PAWN <-> CEF preko samp-cef API
//  (cef.emit / cef.subscribe)
// ============================================================

// ========== STATE ==========
const state = {
    currentScreen: 'portal',
    username: 'Igrac',
    battery: 87,
    money: 0,
    level: 1,
    onlinePlayers: 0,
    bountyPage: 0,
    bountyTotalPages: 1,
    bounties: []
};

// ========== INIT ==========
document.addEventListener('DOMContentLoaded', () => {
    updateClock();
    setInterval(updateClock, 1000);
    createParticles();
    renderBountyList();
    updateBattery(state.battery);
    updateQuickInfo();

    // samp-cef API: Subscribe to events from PAWN server
    // koristimo window.cef (samp-cef plugin) ili fallback za demo
    if (typeof window.cef !== 'undefined' && typeof window.cef.subscribe === 'function') {
        // Samp-CEF plugin API
        window.cef.subscribe('tablet:open', (data) => {
            try {
                const parsed = JSON.parse(data);
                state.username = parsed.username || 'Igrac';
                state.battery = parsed.battery || 87;
                state.money = parsed.money || 0;
                state.level = parsed.level || 1;
                state.onlinePlayers = parsed.online || 0;
            } catch(e) {}
            updateUI();
            showScreen('portal');
        });

        window.cef.subscribe('tablet:bounty:update', (data) => {
            try {
                const parsed = JSON.parse(data);
                state.bounties = parsed.bounties || [];
            } catch(e) {}
            renderBountyList();
        });

        window.cef.subscribe('tablet:toast', (data) => {
            try {
                const parsed = JSON.parse(data);
                showToast(parsed.message || 'Obavijest');
            } catch(e) {
                showToast('Obavijest');
            }
        });

        window.cef.subscribe('tablet:close', () => {
            showScreen('portal');
        });
    } else if (typeof window.cef_subscribe === 'function') {
        // Starija verzija CEF API (fallback)
        window.cef_subscribe('tablet:open', (data) => {
            try {
                const parsed = JSON.parse(data);
                state.username = parsed.username || 'Igrac';
                state.battery = parsed.battery || 87;
                state.money = parsed.money || 0;
                state.level = parsed.level || 1;
                state.onlinePlayers = parsed.online || 0;
            } catch(e) {}
            updateUI();
            showScreen('portal');
        });

        window.cef_subscribe('tablet:bounty:update', (data) => {
            try {
                const parsed = JSON.parse(data);
                state.bounties = parsed.bounties || [];
            } catch(e) {}
            renderBountyList();
        });

        window.cef_subscribe('tablet:toast', (data) => {
            try {
                const parsed = JSON.parse(data);
                showToast(parsed.message || 'Obavijest');
            } catch(e) {}
        });

        window.cef_subscribe('tablet:close', () => {
            showScreen('portal');
        });
    }
});

// ========== HELPER: Emit event to PAWN ==========
function emitToPawn(eventName, data) {
    // samp-cef plugin API (najnovija verzija)
    if (typeof window.cef !== 'undefined' && typeof window.cef.emit === 'function') {
        window.cef.emit(eventName, data || '');
    }
    // Starija verzija API
    else if (typeof window.cef_emit === 'function') {
        window.cef_emit(eventName, data || '');
    }
}

// ========== CLOCK ==========
function updateClock() {
    const now = new Date();
    const h = String(now.getHours()).padStart(2, '0');
    const m = String(now.getMinutes()).padStart(2, '0');
    const clockEl = document.getElementById('clock');
    if (clockEl) clockEl.textContent = `${h}:${m}`;
}

// ========== BATTERY ==========
function updateBattery(pct) {
    pct = Math.max(0, Math.min(100, pct));
    state.battery = pct;
    const fill = document.getElementById('battery-fill');
    const pctEl = document.getElementById('battery-pct');
    if (fill) fill.style.width = pct + '%';
    if (pctEl) pctEl.textContent = pct + '%';

    if (fill && pctEl) {
        if (pct <= 20) {
            fill.style.background = 'var(--text-red, #FF453A)';
            pctEl.style.color = 'var(--text-red, #FF453A)';
        } else if (pct <= 50) {
            fill.style.background = 'var(--text-gold, #FFD60A)';
            pctEl.style.color = 'var(--text-gold, #FFD60A)';
        } else {
            fill.style.background = 'var(--text-green, #30D158)';
            pctEl.style.color = 'var(--text-green, #30D158)';
        }
    }
}

// ========== PARTICLES ==========
function createParticles() {
    const container = document.getElementById('particles');
    if (!container) return;
    for (let i = 0; i < 30; i++) {
        const p = document.createElement('div');
        p.className = 'particle';
        p.style.left = Math.random() * 100 + '%';
        p.style.animationDelay = Math.random() * 6 + 's';
        p.style.animationDuration = (4 + Math.random() * 4) + 's';
        container.appendChild(p);
    }
}

// ========== SCREENS ==========
function showScreen(name) {
    document.querySelectorAll('.screen').forEach(s => s.classList.remove('active'));
    const el = document.getElementById('screen-' + name);
    if (el) {
        el.classList.add('active');
        state.currentScreen = name;
    }
}

// ========== PORTAL / LOGIN ==========
function onPasswordClick() {
    emitToPawn('tablet:login:request');
    // Demo mode (bez CEF): prikazi toast i idi na home
    if (typeof window.cef === 'undefined' && typeof window.cef_subscribe === 'undefined') {
        showToast('Prijava uspjesna! (Demo)');
        setTimeout(() => showScreen('home'), 500);
    }
}

function onLogin() {
    emitToPawn('tablet:login:attempt');
    if (typeof window.cef === 'undefined' && typeof window.cef_subscribe === 'undefined') {
        showToast('Prijava uspjesna! (Demo)');
        setTimeout(() => showScreen('home'), 500);
    }
}

function onRegister() {
    emitToPawn('tablet:register:request');
    if (typeof window.cef === 'undefined' && typeof window.cef_subscribe === 'undefined') {
        showToast('Registracija uspjesna! (Demo)');
        setTimeout(() => showScreen('home'), 500);
    }
}

function onLogout() {
    showScreen('portal');
    showToast('Odjavljen sa portala');
    emitToPawn('tablet:logout');
}

// ========== HOME ==========
function goHome() {
    showScreen('home');
}

function openApp(app) {
    if (app === 'bounty') {
        showScreen('bounty');
        renderBountyList();
    } else {
        showToast('App ' + app.toUpperCase() + ' - uskoro!');
    }
}

function updateUI() {
    const usernameEl = document.getElementById('username-display');
    const welcomeEl = document.getElementById('welcome-user');
    if (usernameEl) usernameEl.textContent = state.username;
    if (welcomeEl) welcomeEl.textContent = 'Dobrodosao, ' + state.username.split('_')[0];
    updateBattery(state.battery);
    updateQuickInfo();
}

function updateQuickInfo() {
    const moneyEl = document.getElementById('player-money');
    const levelEl = document.getElementById('player-level');
    const onlineEl = document.getElementById('online-players');
    const countEl = document.getElementById('bounty-count');
    const activeEl = document.getElementById('active-bounties');
    if (moneyEl) moneyEl.textContent = '$' + state.money.toLocaleString();
    if (levelEl) levelEl.textContent = state.level;
    if (onlineEl) onlineEl.textContent = state.onlinePlayers + '/200';
    if (countEl) countEl.textContent = state.bounties.length;
    if (activeEl) activeEl.textContent = state.bounties.length;
}

// ========== BOUNTY BOARD ==========
const BOUNTY_PAGE_SIZE = 6;

function renderBountyList() {
    const list = document.getElementById('bounty-list');
    if (!list) return;

    const start = state.bountyPage * BOUNTY_PAGE_SIZE;
    const end = start + BOUNTY_PAGE_SIZE;
    const pageBounties = state.bounties.slice(start, end);
    state.bountyTotalPages = Math.max(1, Math.ceil(state.bounties.length / BOUNTY_PAGE_SIZE));

    if (state.bounties.length === 0) {
        list.innerHTML = `
            <div class="bounty-empty">
                Nema aktivnih nagrada<br>
                <small>Budi prvi koji ce naruciti ubojstvo!</small>
            </div>`;
    } else {
        list.innerHTML = pageBounties.map((b, i) => `
            <div class="bounty-row" onclick="showBountyDetail(${start + i})">
                <div class="row-rank">${start + i + 1}</div>
                <div class="row-info">
                    <div class="row-name">${b.target}</div>
                    <div class="row-meta">
                        ${b.placer} &bull; ${b.time} ${b.stacked ? '&bull; <span style="color:#FFD60A">STACKED</span>' : ''}
                    </div>
                </div>
                <div class="row-amount">$${b.amount.toLocaleString()}</div>
            </div>
        `).join('');
    }

    const pageInfoEl = document.getElementById('page-info');
    if (pageInfoEl) pageInfoEl.textContent = `${state.bountyPage + 1} / ${state.bountyTotalPages}`;
    updateQuickInfo();
}

function nextPage() {
    if (state.bountyPage < state.bountyTotalPages - 1) {
        state.bountyPage++;
        renderBountyList();
    }
}

function prevPage() {
    if (state.bountyPage > 0) {
        state.bountyPage--;
        renderBountyList();
    }
}

function showBountyDetail(index) {
    const b = state.bounties[index];
    if (!b) return;

    const body = document.getElementById('bounty-detail-body');
    if (!body) return;
    body.innerHTML = `
        <div class="detail-row">
            <span class="detail-label">Meta</span>
            <span class="detail-value">${b.target}</span>
        </div>
        <div class="detail-row">
            <span class="detail-label">Nagrada</span>
            <span class="detail-value gold">$${b.amount.toLocaleString()}</span>
        </div>
        <div class="detail-row">
            <span class="detail-label">Narucio</span>
            <span class="detail-value">${b.placer}</span>
        </div>
        <div class="detail-row">
            <span class="detail-label">Prije</span>
            <span class="detail-value">${b.time}</span>
        </div>
        <div class="detail-row">
            <span class="detail-label">Stacked</span>
            <span class="detail-value">${b.stacked ? 'Da' : 'Ne'}</span>
        </div>
    `;

    const modal = document.getElementById('modal-bounty-detail');
    if (modal) modal.classList.add('active');
}

function closeBountyDetail() {
    const modal = document.getElementById('modal-bounty-detail');
    if (modal) modal.classList.remove('active');
}

// ========== PLACE BOUNTY ==========
function showPlaceBounty() {
    const targetInput = document.getElementById('bounty-target-id');
    const amountInput = document.getElementById('bounty-amount');
    if (targetInput) targetInput.value = '';
    if (amountInput) amountInput.value = '';
    const modal = document.getElementById('modal-place-bounty');
    if (modal) modal.classList.add('active');
}

function closePlaceBounty() {
    const modal = document.getElementById('modal-place-bounty');
    if (modal) modal.classList.remove('active');
}

function confirmPlaceBounty() {
    const targetId = parseInt((document.getElementById('bounty-target-id') || {}).value || '0');
    const amount = parseInt((document.getElementById('bounty-amount') || {}).value || '0');

    if (isNaN(targetId) || targetId < 0) {
        showToast('Unesi ispravan ID igraca!');
        return;
    }
    if (isNaN(amount) || amount < 1000) {
        showToast('Minimum nagrade je $1,000!');
        return;
    }
    if (amount > 500000) {
        showToast('Maksimum nagrade je $500,000!');
        return;
    }

    // Emituj event ka PAWN serveru
    emitToPawn('tablet:bounty:place', JSON.stringify({ targetId, amount }));

    // Demo mode (bez CEF): dodaj lokalno
    if (typeof window.cef === 'undefined' && typeof window.cef_subscribe === 'undefined') {
        state.bounties.unshift({
            id: state.bounties.length + 1,
            target: 'Igrac_' + targetId,
            amount: amount,
            placer: state.username,
            time: 'upravo',
            stacked: false
        });
        renderBountyList();
        showToast('Nagrada od $' + amount.toLocaleString() + ' postavljena! (Demo)');
    }

    closePlaceBounty();
}

// ========== TOAST ==========
function showToast(message) {
    const toast = document.getElementById('toast');
    const textEl = document.getElementById('toast-text');
    if (textEl) textEl.textContent = message;
    if (toast) {
        toast.classList.add('show');
        setTimeout(() => toast.classList.remove('show'), 3000);
    }
}
