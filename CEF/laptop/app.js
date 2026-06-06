// ===========================================================================
//  Unicate Gaming Laptop - CEF App (samp-cef API)
//  Realistic laptop OS with apps: Dark Web, Bank, BountyNet, Email, Terminal
// ===========================================================================

var currentApp = null;
var startMenuOpen = false;
var isDarkTheme = true;
var terminalHistory = '';

document.addEventListener('DOMContentLoaded', function() {
    initClock();
    subscribeToEvents();
    loadDemoData();
});

function emitToServer(eventName) {
    var args = Array.prototype.slice.call(arguments, 1);
    try {
        if (window.cef && window.cef.emit) {
            window.cef.emit.apply(window.cef, [eventName].concat(args));
        }
    } catch(e) { console.error('[CEF emit error]', e); }
}

function subscribeToEvents() {
    try {
        if (window.cef && window.cef.subscribe) {
            window.cef.subscribe('laptop:init', function(json) {
                try {
                    var data = JSON.parse(json);
                    document.getElementById('startUser').textContent = data.username || 'Korisnik';
                } catch(e) {}
            });
            window.cef.subscribe('laptop:notify', function(msg) { showToast(msg); });
            window.cef.subscribe('laptop:bankBalance', function(bal) {
                var el = document.getElementById('bankBalance');
                if (el) el.textContent = '$' + Number(bal).toLocaleString();
            });
        } else { loadDemoData(); }
    } catch(e) { loadDemoData(); }
}

function initClock() {
    updateClock();
    setInterval(updateClock, 1000);
}

function updateClock() {
    var now = new Date();
    var t = String(now.getHours()).padStart(2,'0') + ':' + String(now.getMinutes()).padStart(2,'0');
    document.getElementById('osClock').textContent = t;
    document.getElementById('taskClock').textContent = t;
}

function toggleTheme() {
    isDarkTheme = !isDarkTheme;
    document.documentElement.setAttribute('data-theme', isDarkTheme ? 'dark' : 'light');
}

function toggleStartMenu() {
    startMenuOpen = !startMenuOpen;
    document.getElementById('startMenu').style.display = startMenuOpen ? 'block' : 'none';
}

// ---- App Management ----
function openApp(appName) {
    currentApp = appName;
    var win = document.getElementById('appWindow');
    win.style.display = 'flex';
    document.getElementById('windowTitle').textContent = getAppTitle(appName);
    loadAppContent(appName);
    updateTaskbar();
}

function closeApp() {
    document.getElementById('appWindow').style.display = 'none';
    currentApp = null;
    updateTaskbar();
}

function getAppTitle(name) {
    var titles = {
        'darkweb': 'Dark Web Marketplace',
        'bank': 'UG Banka Online',
        'bountynet': 'BountyNet Portal',
        'email': 'UG Email',
        'files': 'Fajl Menadzer',
        'terminal': 'UG Terminal',
        'settings': 'Podesavanja Sistema'
    };
    return titles[name] || name;
}

function updateTaskbar() {
    var el = document.getElementById('taskbarApps');
    el.innerHTML = '';
    if (currentApp) {
        el.innerHTML = '<span class="task-app" onclick="focusApp()">' + getAppTitle(currentApp) + '</span>';
    }
}

function focusApp() {
    var win = document.getElementById('appWindow');
    if (win.style.display === 'none' && currentApp) {
        win.style.display = 'flex';
    }
}

// ---- App Content Loaders ----
function loadAppContent(appName) {
    var content = document.getElementById('windowContent');
    content.className = 'window-content';

    switch(appName) {
        case 'darkweb': loadDarkWeb(content); content.classList.add('app-darkweb'); break;
        case 'bank': loadBank(content); content.classList.add('app-bank'); break;
        case 'bountynet': loadBountyNet(content); break;
        case 'email': loadEmail(content); break;
        case 'files': loadFiles(content); break;
        case 'terminal': loadTerminal(content); break;
        case 'settings': loadSettings(content); break;
        default: content.innerHTML = '<p>Aplikacija nije dostupna.</p>';
    }
}

function loadDarkWeb(el) {
    var items = [
        { name: 'Pistol 9mm', price: '$5,000', cmd: 'pistol' },
        { name: 'Deagle', price: '$15,000', cmd: 'deagle' },
        { name: 'AK-47', price: '$25,000', cmd: 'ak47' },
        { name: 'M4 Rifle', price: '$30,000', cmd: 'm4' },
        { name: 'Sniper Rifle', price: '$50,000', cmd: 'sniper' },
        { name: 'Marihuana x5', price: '$3,000', cmd: 'weed' },
        { name: 'Kokain x5', price: '$8,000', cmd: 'coke' },
        { name: 'Tracker Uredaj', price: '$10,000', cmd: 'tracker' },
        { name: 'Lazna Dozvola', price: '$20,000', cmd: 'fakeid' },
        { name: 'Hack Usluga', price: '$15,000', cmd: 'hack' }
    ];

    var html = '<h3 style="color:#ff453a;margin-bottom:8px;">&#9760; DARK WEB MARKETPLACE</h3>' +
        '<p style="color:#5b7fa5;font-size:10px;margin-bottom:10px;">Anonimne narudzbe - isporuka na tajnu lokaciju</p>';

    items.forEach(function(item) {
        html += '<div class="dw-item" onclick="darkwebBuy(\'' + item.cmd + '\')">' +
            '<div class="dw-name">' + item.name + '</div>' +
            '<div class="dw-price">' + item.price + ' - Klikni za narudzbu</div></div>';
    });

    el.innerHTML = html;
}

function darkwebBuy(cmd) {
    emitToServer('laptop:darkweb:buy', cmd);
    showToast('Narudzba poslata! Cekaj isporuku...');
}

function loadBank(el) {
    var html = '<div class="balance-card">' +
        '<div class="balance-label">STANJE RACUNA</div>' +
        '<div class="balance-value" id="bankBalance">$125,000</div></div>' +
        '<h4 style="margin-bottom:6px;">Novac u dzepu</h4>' +
        '<p style="color:#5b7fa5;font-size:11px;margin-bottom:10px;" id="pocketMoney">$5,000</p>' +
        '<h4 style="margin-bottom:6px;">Uplata na racun</h4>' +
        '<input type="number" class="app-input" id="depositAmt" placeholder="Iznos za uplatu">' +
        '<button class="app-btn" onclick="bankDeposit()">Uplati</button>' +
        '<h4 style="margin-top:10px;margin-bottom:6px;">Isplata sa racuna</h4>' +
        '<input type="number" class="app-input" id="withdrawAmt" placeholder="Iznos za isplatu">' +
        '<button class="app-btn danger" onclick="bankWithdraw()">Isplati</button>' +
        '<h4 style="margin-top:10px;margin-bottom:6px;">Transfer</h4>' +
        '<input type="text" class="app-input" id="transferTo" placeholder="Ime primaoca">' +
        '<input type="number" class="app-input" id="transferAmt" placeholder="Iznos">' +
        '<button class="app-btn" onclick="bankTransfer()">Posalji</button>';

    el.innerHTML = html;
}

function bankDeposit() { var a = document.getElementById('depositAmt').value; emitToServer('laptop:bank:deposit', a); }
function bankWithdraw() { var a = document.getElementById('withdrawAmt').value; emitToServer('laptop:bank:withdraw', a); }
function bankTransfer() {
    var to = document.getElementById('transferTo').value;
    var amt = document.getElementById('transferAmt').value;
    emitToServer('laptop:bank:transfer', to, amt);
}

function loadBountyNet(el) {
    el.innerHTML = '<h3 style="color:#ffd60a;margin-bottom:8px;">&#9876; BOUNTYNET PORTAL</h3>' +
        '<p style="color:#5b7fa5;font-size:10px;margin-bottom:10px;">Mrezna lista narucenih ubistava</p>' +
        '<div id="bountyNetList">Ucitavanje...</div>' +
        '<h4 style="margin-top:10px;margin-bottom:6px;">Postavi Bounty</h4>' +
        '<input type="text" class="app-input" id="bnTarget" placeholder="Ime mete">' +
        '<input type="number" class="app-input" id="bnAmount" placeholder="Iznos nagrade ($)"">' +
        '<label style="display:flex;align-items:center;gap:6px;font-size:11px;color:#8cb4e0;margin-bottom:6px;">' +
        '<input type="checkbox" id="bnAnon"> Anonimno</label>' +
        '<button class="app-btn" onclick="bountyPlace()">Postavi</button>';

    emitToServer('laptop:bounty:request');
}

function bountyPlace() {
    var t = document.getElementById('bnTarget').value;
    var a = document.getElementById('bnAmount').value;
    var anon = document.getElementById('bnAnon').checked;
    emitToServer('laptop:bounty:place', JSON.stringify({targetName:t, amount:Number(a), anonymous:anon}));
    showToast('Bounty poslat!');
}

function loadEmail(el) {
    el.innerHTML = '<h3 style="margin-bottom:8px;">&#9993; UG EMAIL</h3>' +
        '<div id="emailList" style="margin-bottom:10px;"><p style="color:#5b7fa5;">Nema novih poruka</p></div>' +
        '<h4 style="margin-bottom:6px;">Nova poruka</h4>' +
        '<input type="text" class="app-input" id="emailTo" placeholder="Primaoc">' +
        '<input type="text" class="app-input" id="emailSubj" placeholder="Predmet">' +
        '<textarea class="app-input" id="emailBody" rows="3" placeholder="Tekst poruke..." style="resize:none;"></textarea>' +
        '<button class="app-btn" onclick="sendEmail()">Posalji</button>';
}

function sendEmail() {
    emitToServer('laptop:email:send',
        document.getElementById('emailTo').value,
        document.getElementById('emailSubj').value,
        document.getElementById('emailBody').value);
    showToast('Email poslat!');
}

function loadFiles(el) {
    el.innerHTML = '<h3 style="margin-bottom:8px;">&#128193; FAJL MENADZER</h3>' +
        '<div style="display:flex;flex-direction:column;gap:6px;">' +
        '<div class="dw-item" style="border-color:rgba(46,134,222,0.2);background:rgba(46,134,222,0.08);">' +
        '<div class="dw-name" style="color:#54a0ff;">&#128193; Dokumenti</div><div class="dw-price" style="color:#5b7fa5;">3 fajla</div></div>' +
        '<div class="dw-item" style="border-color:rgba(46,134,222,0.2);background:rgba(46,134,222,0.08);">' +
        '<div class="dw-name" style="color:#54a0ff;">&#128193; Slike</div><div class="dw-price" style="color:#5b7fa5;">12 fajlova</div></div>' +
        '<div class="dw-item" style="border-color:rgba(46,134,222,0.2);background:rgba(46,134,222,0.08);">' +
        '<div class="dw-name" style="color:#54a0ff;">&#128193; Preuzimanja</div><div class="dw-price" style="color:#5b7fa5;">1 fajl</div></div>' +
        '</div>';
}

function loadTerminal(el) {
    terminalHistory = 'UG Terminal v2.0\nUnicate Gaming Network\n\n$ ';
    el.innerHTML = '<div class="terminal-output" id="termOutput">' + terminalHistory + '</div>' +
        '<input type="text" class="app-input" id="termInput" placeholder="Unesi komandu..." ' +
        'onkeydown="if(event.key===\'Enter\')terminalExec()" style="font-family:Consolas,monospace;">';

    document.getElementById('termInput').focus();
}

function terminalExec() {
    var input = document.getElementById('termInput');
    var cmd = input.value.trim();
    input.value = '';

    terminalHistory += cmd + '\n';

    if (cmd === 'help') {
        terminalHistory += 'Komande: help, status, ping, whoami, clear, logout\n';
    } else if (cmd === 'status') {
        terminalHistory += 'Server: ONLINE\nIP: ' + SERVER_IP + '\nIgraci: 42/200\n';
    } else if (cmd === 'ping') {
        terminalHistory += 'Pinging... 32ms\n';
    } else if (cmd === 'whoami') {
        terminalHistory += 'Korisnik: ' + (document.getElementById('startUser').textContent) + '\n';
    } else if (cmd === 'clear') {
        terminalHistory = '$ ';
    } else if (cmd === 'logout') {
        closeLaptop(); return;
    } else if (cmd.length > 0) {
        terminalHistory += 'Nepoznata komanda: ' + cmd + '\n';
        emitToServer('laptop:terminal', cmd);
    }

    terminalHistory += '$ ';
    document.getElementById('termOutput').textContent = terminalHistory;
    document.getElementById('termOutput').scrollTop = 99999;
}

function loadSettings(el) {
    el.innerHTML = '<h3 style="margin-bottom:10px;">&#9881; PODESAVANJA</h3>' +
        '<div style="margin-bottom:8px;">' +
        '<label style="display:flex;align-items:center;justify-content:space-between;font-size:12px;color:#8cb4e0;">' +
        'Tamna tema <input type="checkbox" checked onchange="toggleTheme()"></label></div>' +
        '<div style="margin-bottom:8px;">' +
        '<label style="display:flex;align-items:center;justify-content:space-between;font-size:12px;color:#8cb4e0;">' +
        'Zvuk obavijesti <input type="checkbox" checked></label></div>' +
        '<div style="margin-bottom:16px;">' +
        '<label style="display:flex;align-items:center;justify-content:space-between;font-size:12px;color:#8cb4e0;">' +
        'Automatski update <input type="checkbox"></label></div>' +
        '<p style="font-size:10px;color:#5b7fa5;text-align:center;">UG OS v2.0 | Unicate Gaming</p>';
}

function closeLaptop() { emitToServer('laptop:close'); }

function showToast(msg) {
    var existing = document.querySelector('.toast');
    if (existing) existing.remove();
    var toast = document.createElement('div');
    toast.className = 'toast'; toast.textContent = msg;
    document.body.appendChild(toast);
    setTimeout(function() { toast.classList.add('show'); }, 10);
    setTimeout(function() { toast.classList.remove('show'); setTimeout(function() { toast.remove(); }, 300); }, 2500);
}

function loadDemoData() {
    document.getElementById('startUser').textContent = 'Demo_Igrac';
}

function escapeHTML(str) {
    if (!str) return '';
    var div = document.createElement('div');
    div.textContent = str;
    return div.innerHTML;
}
