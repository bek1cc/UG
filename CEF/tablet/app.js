// ============================================================
//  UG TABLET CEF - JavaScript kontroler
//  Komunikacija: PAWN <-> CEF preko window.cef_emit / cef_subscribe
// ============================================================

// ========== STATE ==========
const state = {
    currentScreen: 'portal',
    username: 'Beka_Tiruriru',
    battery: 87,
    money: 125000,
    level: 15,
    onlinePlayers: 47,
    bountyPage: 0,
    bountyTotalPages: 1,
    bounties: [
        { id: 1, target: 'Mirko_Djordjevic', amount: 25000, placer: 'Anoniman', time: '2h', stacked: false },
        { id: 2, target: 'Pero_Peric', amount: 75000, placer: 'Beka_Tiruriru', time: '45m', stacked: true },
        { id: 3, target: 'Sloba_Jebach', amount: 150000, placer: 'Anoniman', time: '15m', stacked: false },
    ]
};

// ========== INIT ==========
document.addEventListener('DOMContentLoaded', () => {
    updateClock();
    setInterval(updateClock, 1000);
    createParticles();
    renderBountyList();
    updateBattery(state.battery);
    updateQuickInfo();

    // CEF: Subscribe to events from PAWN
    if (typeof window.cef_subscribe === 'function') {
        window.cef_subscribe('tablet:open', (data) => {
            const parsed = JSON.parse(data);
            state.username = parsed.username || 'Igrac';
            state.battery = parsed.battery || 87;
            state.money = parsed.money || 0;
            state.level = parsed.level || 1;
            state.onlinePlayers = parsed.online || 0;
            updateUI();
            showScreen('portal');
        });

        window.cef_subscribe('tablet:bounty:update', (data) => {
            const parsed = JSON.parse(data);
            state.bounties = parsed.bounties || [];
            renderBountyList();
        });

        window.cef_subscribe('tablet:toast', (data) => {
            const parsed = JSON.parse(data);
            showToast(parsed.message || 'Obavijest');
        });

        window.cef_subscribe('tablet:close', () => {
            // CEF browser will be hidden by PAWN
        });
    }
});

// ========== CLOCK ==========
function updateClock() {
    const now = new Date();
    const h = String(now.getHours()).padStart(2, '0');
    const m = String(now.getMinutes()).padStart(2, '0');
    document.getElementById('clock').textContent = `${h}:${m}`;
}

// ========== BATTERY ==========
function updateBattery(pct) {
    pct = Math.max(0, Math.min(100, pct));
    state.battery = pct;
    const fill = document.getElementById('battery-fill');
    const pctEl = document.getElementById('battery-pct');
    fill.style.width = pct + '%';
    pctEl.textContent = pct + '%';

    if (pct <= 20) {
        fill.style.background = 'var(--text-red)';
        pctEl.style.color = 'var(--text-red)';
    } else if (pct <= 50) {
        fill.style.background = 'var(--text-gold)';
        pctEl.style.color = 'var(--text-gold)';
    } else {
        fill.style.background = 'var(--text-green)';
        pctEl.style.color = 'var(--text-green)';
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
    // In CEF: emit event to PAWN to show dialog for PIN input
    if (typeof window.cef_emit === 'function') {
        window.cef_emit('tablet:login:request');
    } else {
        // Demo mode: just login
        showToast('Prijava uspjesna!');
        setTimeout(() => {
            showScreen('home');
        }, 500);
    }
}

function onLogin() {
    if (typeof window.cef_emit === 'function') {
        window.cef_emit('tablet:login:attempt');
    } else {
        showToast('Prijava uspjesna!');
        setTimeout(() => showScreen('home'), 500);
    }
}

function onRegister() {
    if (typeof window.cef_emit === 'function') {
        window.cef_emit('tablet:register:request');
    } else {
        showToast('Registracija uspjesna!');
        setTimeout(() => showScreen('home'), 500);
    }
}

function onLogout() {
    showScreen('portal');
    showToast('Odjavljen sa portala');
    if (typeof window.cef_emit === 'function') {
        window.cef_emit('tablet:logout');
    }
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
        showToast('App ' + app.toUpperCase() + - uskoro!');
    }
}

function updateUI() {
    document.getElementById('username-display').textContent = state.username;
    document.getElementById('welcome-user').textContent = 'Dobrodosao, ' + state.username.split('_')[0];
    updateBattery(state.battery);
    updateQuickInfo();
}

function updateQuickInfo() {
    document.getElementById('player-money').textContent = '$' + state.money.toLocaleString();
    document.getElementById('player-level').textContent = state.level;
    document.getElementById('online-players').textContent = state.onlinePlayers + '/200';
    document.getElementById('bounty-count').textContent = state.bounties.length;
    document.getElementById('active-bounties').textContent = state.bounties.length;
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
                <i class="fas fa-ghost"></i>
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
                        ${b.placer} &bull; ${b.time} ${b.stacked ? '&bull; <span style="color:var(--text-gold)">STACKED</span>' : ''}
                    </div>
                </div>
                <div class="row-amount">$${b.amount.toLocaleString()}</div>
            </div>
        `).join('');
    }

    document.getElementById('page-info').textContent = `${state.bountyPage + 1} / ${state.bountyTotalPages}`;
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

    document.getElementById('modal-bounty-detail').classList.add('active');
}

function closeBountyDetail() {
    document.getElementById('modal-bounty-detail').classList.remove('active');
}

// ========== PLACE BOUNTY ==========
function showPlaceBounty() {
    document.getElementById('bounty-target-id').value = '';
    document.getElementById('bounty-amount').value = '';
    document.getElementById('modal-place-bounty').classList.add('active');
}

function closePlaceBounty() {
    document.getElementById('modal-place-bounty').classList.remove('active');
}

function confirmPlaceBounty() {
    const targetId = parseInt(document.getElementById('bounty-target-id').value);
    const amount = parseInt(document.getElementById('bounty-amount').value);

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

    if (typeof window.cef_emit === 'function') {
        window.cef_emit('tablet:bounty:place', JSON.stringify({ targetId, amount }));
    } else {
        // Demo: Add locally
        state.bounties.unshift({
            id: state.bounties.length + 1,
            target: 'Igrac_' + targetId,
            amount: amount,
            placer: state.username,
            time: 'upravo',
            stacked: false
        });
        renderBountyList();
        showToast('Nagrada od $' + amount.toLocaleString() + ' postavljena!');
    }

    closePlaceBounty();
}

// ========== TOAST ==========
function showToast(message) {
    const toast = document.getElementById('toast');
    document.getElementById('toast-text').textContent = message;
    toast.classList.add('show');
    setTimeout(() => toast.classList.remove('show'), 3000);
}
