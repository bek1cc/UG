// ===========================================================================
//  Unicate Gaming Tablet - CEF App (samp-cef API)
//  Uses: window.cef.emit() / window.cef.subscribe()
// ===========================================================================

// ---- Global State ----
var currentTab = 'home';
var bounties = [];
var messages = [];
var isDarkTheme = true;
var playerData = { name: 'Igrac', money: 0, level: 0, kills: 0, phone: 0 };

// ---- Initialize ----
document.addEventListener('DOMContentLoaded', function() {
    initClock();
    initTabs();
    initTheme();
    subscribeToServerEvents();
    showToast('Tablet povezan!');
});

// ---- Clock ----
function initClock() {
    updateClock();
    setInterval(updateClock, 1000);
}

function updateClock() {
    var now = new Date();
    var h = String(now.getHours()).padStart(2, '0');
    var m = String(now.getMinutes()).padStart(2, '0');
    document.getElementById('clock').textContent = h + ':' + m;
}

// ---- Navigation Tabs ----
function initTabs() {
    var tabs = document.querySelectorAll('.nav-tab');
    tabs.forEach(function(tab) {
        tab.addEventListener('click', function() {
            switchTab(this.getAttribute('data-tab'));
        });
    });
}

function switchTab(tabName) {
    document.querySelectorAll('.nav-tab').forEach(function(t) {
        t.classList.remove('active');
    });
    document.querySelectorAll('.tab-content').forEach(function(c) {
        c.classList.remove('active');
    });

    document.querySelector('[data-tab="' + tabName + '"]').classList.add('active');
    document.getElementById('tab-' + tabName).classList.add('active');
    currentTab = tabName;

    if (tabName === 'bounties') requestBounties();
    else if (tabName === 'home') requestPlayerStats();
    else if (tabName === 'messages') requestMessages();
}

// ---- Theme Toggle ----
function initTheme() {
    var saved = localStorage.getItem('ug_theme');
    if (saved === 'light') {
        isDarkTheme = false;
        document.documentElement.setAttribute('data-theme', 'light');
        document.getElementById('darkModeToggle').checked = false;
        document.getElementById('themeToggle').innerHTML = '&#9788;';
    }
}

function toggleTheme() {
    isDarkTheme = !isDarkTheme;
    if (isDarkTheme) {
        document.documentElement.setAttribute('data-theme', 'dark');
        localStorage.setItem('ug_theme', 'dark');
        document.getElementById('themeToggle').innerHTML = '&#9789;';
    } else {
        document.documentElement.setAttribute('data-theme', 'light');
        localStorage.setItem('ug_theme', 'light');
        document.getElementById('themeToggle').innerHTML = '&#9788;';
    }
}

document.getElementById('themeToggle').addEventListener('click', function() {
    var checkbox = document.getElementById('darkModeToggle');
    checkbox.checked = !checkbox.checked;
    toggleTheme();
});

// ---- CEF Communication ----
function emitToServer(eventName) {
    var args = Array.prototype.slice.call(arguments, 1);
    try {
        if (window.cef && window.cef.emit) {
            window.cef.emit.apply(window.cef, [eventName].concat(args));
        } else {
            console.log('[CEF emit]', eventName, args);
        }
    } catch (e) {
        console.error('[CEF emit error]', e);
    }
}

function subscribeToServerEvents() {
    try {
        if (window.cef && window.cef.subscribe) {
            // Player stats
            window.cef.subscribe('tablet:playerStats', function(name, money, level, kills) {
                updatePlayerStats(name, money, level, kills);
            });

            // Bounty list (JSON string)
            window.cef.subscribe('tablet:bountyList', function(bountyJSON) {
                updateBountyList(bountyJSON);
            });

            // Messages (JSON string)
            window.cef.subscribe('tablet:messages', function(msgJSON) {
                updateMessages(msgJSON);
            });

            // Toast notifications
            window.cef.subscribe('tablet:notify', function(message) {
                showToast(message);
            });

            // Bounty placed confirmation
            window.cef.subscribe('tablet:bountyPlaced', function(target, amount) {
                showToast('Bounty postavljen na ' + target + ' za $' + amount);
                requestBounties();
            });

            // Message sent confirmation
            window.cef.subscribe('tablet:messageSent', function(to) {
                showToast('Poruka poslata za ' + to);
                document.getElementById('msgTo').value = '';
                document.getElementById('msgText').value = '';
            });

            // Init data from server (JSON)
            window.cef.subscribe('tablet:init', function(dataJSON) {
                try {
                    var data = JSON.parse(dataJSON);
                    playerData = data;
                    applyPlayerData(data);
                } catch(e) {
                    console.error('Parse init error', e);
                }
            });

            // Login result
            window.cef.subscribe('tablet:login:result', function(dataJSON) {
                try {
                    var data = JSON.parse(dataJSON);
                    if (data.success) {
                        showToast('Uspjesna prijava!');
                    } else {
                        showToast(data.message || 'Greska pri prijavi');
                    }
                } catch(e) {}
            });

            // Bounty update (JSON)
            window.cef.subscribe('tablet:bounty:update', function(json) {
                updateBountyList(json);
            });

            // Toast
            window.cef.subscribe('tablet:toast', function(dataJSON) {
                try {
                    var data = JSON.parse(dataJSON);
                    showToast(data.message || dataJSON);
                } catch(e) {
                    showToast(dataJSON);
                }
            });

            console.log('[CEF] Subscriptions registered');
        } else {
            console.log('[CEF] No cef API - demo mode');
            loadDemoData();
        }
    } catch (e) {
        console.error('[CEF subscribe error]', e);
        loadDemoData();
    }
}

// ---- Request Data ----
function requestPlayerStats() { emitToServer('tablet:requestStats'); }
function requestBounties() { emitToServer('tablet:requestBounties'); }
function requestMessages() { emitToServer('tablet:requestMessages'); }

// ---- Update UI ----
function applyPlayerData(data) {
    document.getElementById('playerName').textContent = data.username || data.name || 'Igrac';
    document.getElementById('playerAvatar').textContent = (data.username || data.name || '?')[0].toUpperCase();
    document.getElementById('statMoney').textContent = '$' + Number(data.money || 0).toLocaleString();
    document.getElementById('statLevel').textContent = data.level || 0;
    document.getElementById('statKills').textContent = data.kills || 0;
    document.getElementById('profileName').textContent = data.username || data.name || '-';
    document.getElementById('profilePhone').textContent = data.phone || '-';
    document.getElementById('profileLevel').textContent = data.level || '-';

    // Battery
    if (data.battery !== undefined) {
        var pct = Math.max(5, Math.min(100, data.battery));
        document.getElementById('batteryText').textContent = pct + '%';
        document.getElementById('batteryFill').style.width = pct + '%';
        if (pct < 20) {
            document.getElementById('batteryFill').style.background = '#ff453a';
        }
    }

    // Online players
    if (data.online !== undefined) {
        document.getElementById('statBounties').textContent = data.online;
    }
}

function updatePlayerStats(name, money, level, kills) {
    playerData = { name: name, money: money, level: level, kills: kills };
    document.getElementById('playerName').textContent = name;
    document.getElementById('playerAvatar').textContent = name[0].toUpperCase();
    document.getElementById('statMoney').textContent = '$' + Number(money).toLocaleString();
    document.getElementById('statLevel').textContent = level;
    document.getElementById('statKills').textContent = kills;
}

function updateBountyList(bountyJSON) {
    try {
        var data = JSON.parse(bountyJSON);
        bounties = data.bounties || data;
    } catch (e) {
        bounties = [];
    }

    var listEl = document.getElementById('bountyList');
    listEl.innerHTML = '';

    if (!bounties || bounties.length === 0) {
        listEl.innerHTML = '<div class="empty-state"><div class="empty-icon">&#9876;</div><p>Nema aktivnih bounty-ja</p></div>';
        return;
    }

    bounties.forEach(function(b) {
        var item = document.createElement('div');
        item.className = 'bounty-item';
        item.innerHTML =
            '<div class="bounty-info">' +
                '<span class="bounty-name">&#9876; ' + escapeHTML(b.target || b.name || b.targetName) + '</span>' +
                '<span class="bounty-reason">' + escapeHTML(b.placer ? 'Postavio: ' + b.placer : (b.reason || '')) + '</span>' +
            '</div>' +
            '<span class="bounty-amount">$' + Number(b.amount).toLocaleString() + '</span>';
        listEl.appendChild(item);
    });
}

function updateMessages(msgJSON) {
    try {
        messages = JSON.parse(msgJSON);
    } catch (e) {
        messages = [];
    }

    var listEl = document.getElementById('messageList');
    listEl.innerHTML = '';

    if (!messages || messages.length === 0) {
        listEl.innerHTML = '<div class="empty-state"><div class="empty-icon">&#9993;</div><p>Nema poruka</p></div>';
        return;
    }

    messages.forEach(function(m) {
        var item = document.createElement('div');
        item.className = 'message-item';
        item.style.flexDirection = 'column';
        item.style.alignItems = 'flex-start';
        item.innerHTML =
            '<div class="message-header">' +
                '<span class="message-sender">' + escapeHTML(m.from || m.sender) + '</span>' +
                '<span class="message-time">' + escapeHTML(m.time || '') + '</span>' +
            '</div>' +
            '<div class="message-body">' + escapeHTML(m.text || m.message) + '</div>';
        listEl.appendChild(item);
    });
}

// ---- Actions ----
function call911() { emitToServer('tablet:call911'); showToast('Pozivam 911...'); }
function callTaxi() { emitToServer('tablet:callTaxi'); showToast('Pozivam taxi...'); }
function callMehanicar() { emitToServer('tablet:callMehanicar'); showToast('Pozivam mehanicara...'); }
function openBounties() { switchTab('bounties'); }

function placeBounty() {
    var target = document.getElementById('bountyTarget').value.trim();
    var amount = document.getElementById('bountyAmount').value.trim();
    var anon = document.getElementById('bountyAnon').checked;

    if (!target || !amount || Number(amount) <= 0) {
        showToast('Unesi ime i iznos!');
        return;
    }

    var data = JSON.stringify({ targetName: target, amount: Number(amount), anonymous: anon });
    emitToServer('tablet:bounty:place', data);
}

function sendMessage() {
    var to = document.getElementById('msgTo').value.trim();
    var text = document.getElementById('msgText').value.trim();

    if (!to || !text) {
        showToast('Unesi primaoca i poruku!');
        return;
    }

    emitToServer('tablet:sendMessage', to, text);
}

function refreshBounties() {
    requestBounties();
    showToast('Osvjezavanje...');
}

function closeTablet() { emitToServer('tablet:close'); }

// ---- Toast ----
function showToast(message) {
    var existing = document.querySelector('.toast');
    if (existing) existing.remove();

    var toast = document.createElement('div');
    toast.className = 'toast';
    toast.textContent = message;
    document.body.appendChild(toast);

    setTimeout(function() { toast.classList.add('show'); }, 10);
    setTimeout(function() {
        toast.classList.remove('show');
        setTimeout(function() { toast.remove(); }, 300);
    }, 2500);
}

// ---- Demo Data (for browser testing) ----
function loadDemoData() {
    updatePlayerStats('Demo Igrac', 125000, 15, 342);

    var demoBounties = [
        { target: 'Niko_Belic', amount: 50000, placer: 'Anoniman' },
        { target: 'Carl_Johnson', amount: 25000, placer: 'Tommy_V' },
        { target: 'Tommy_Vercetti', amount: 100000, placer: 'Anoniman' }
    ];
    updateBountyList(JSON.stringify(demoBounties));

    var demoMessages = [
        { from: 'Admin', text: 'Dobrodosao na Unicate Gaming!', time: '14:30' },
        { from: 'LSPD', text: 'Imamo novi zadatak za tebe.', time: '13:15' }
    ];
    updateMessages(JSON.stringify(demoMessages));

    applyPlayerData({ username: 'Demo Igrac', battery: 87, money: 125000, level: 15, phone: 123456, online: 42 });
}

// ---- Utility ----
function escapeHTML(str) {
    if (!str) return '';
    var div = document.createElement('div');
    div.textContent = str;
    return div.innerHTML;
}
