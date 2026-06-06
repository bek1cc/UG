/* ===========================================================================
   UG Tablet - CEF JavaScript v2.0
   Koristi pravi samp-cef API: window.cef.emit() / window.cef.subscribe()
   Dark/Light theme, SVG ikone, modern UI
   =========================================================================== */

// ===========================================================================
//  GLOBAL STATE
// ===========================================================================
var playerData = {
    username: '',
    battery: 100,
    money: 0,
    level: 1,
    online: 0,
    phone: 0,
    loggedIn: false
};

var bounties = [];
var currentScreen = 'lockScreen';
var isDarkTheme = true;

// ===========================================================================
//  SAMP-CEF API WRAPPER
//  Proverava da li je CEF plugin dostupan i koristi pravi API
// ===========================================================================
function cefIsAvailable() {
    return typeof window.cef !== 'undefined' && window.cef !== null;
}

function cefEmit(eventName, data) {
    if (cefIsAvailable()) {
        if (typeof data === 'object') {
            window.cef.emit(eventName, JSON.stringify(data));
        } else {
            window.cef.emit(eventName, String(data));
        }
        console.log('[CEF] Emit: ' + eventName, data);
    } else {
        console.log('[CEF DEBUG] Emit: ' + eventName, data);
    }
}

function cefSubscribe(eventName, callback) {
    if (cefIsAvailable()) {
        window.cef.subscribe(eventName, callback);
        console.log('[CEF] Subscribed: ' + eventName);
    } else {
        console.log('[CEF DEBUG] Subscribe: ' + eventName);
    }
}

// ===========================================================================
//  THEME TOGGLE
// ===========================================================================
function toggleTheme() {
    isDarkTheme = !isDarkTheme;
    var html = document.documentElement;
    var toggle = document.getElementById('themeToggle');
    var desc = document.getElementById('themeDesc');
    var icon = document.getElementById('themeIcon');

    if (isDarkTheme) {
        html.setAttribute('data-theme', 'dark');
        toggle.classList.remove('off');
        desc.textContent = 'Aktivna';
        icon.innerHTML = '<path d="M21 12.79A9 9 0 1111.21 3 7 7 0 0021 12.79z"/>';
    } else {
        html.setAttribute('data-theme', 'light');
        toggle.classList.add('off');
        desc.textContent = 'Svijetla tema';
        icon.innerHTML = '<circle cx="12" cy="12" r="5"/><line x1="12" y1="1" x2="12" y2="3"/><line x1="12" y1="21" x2="12" y2="23"/><line x1="4.22" y1="4.22" x2="5.64" y2="5.64"/><line x1="18.36" y1="18.36" x2="19.78" y2="19.78"/><line x1="1" y1="12" x2="3" y2="12"/><line x1="21" y1="12" x2="23" y2="12"/><line x1="4.22" y1="19.78" x2="5.64" y2="18.36"/><line x1="18.36" y1="5.64" x2="19.78" y2="4.22"/>';
    }
}

// ===========================================================================
//  SCREEN MANAGEMENT
// ===========================================================================
function showScreen(screenId) {
    var screens = document.querySelectorAll('.screen');
    for (var i = 0; i < screens.length; i++) {
        screens[i].classList.remove('active');
    }
    var target = document.getElementById(screenId);
    if (target) {
        target.classList.add('active');
        currentScreen = screenId;
    }
}

function showLogin() {
    showScreen('loginScreen');
    // Reset PIN dots
    var dots = document.querySelectorAll('.pin-dot');
    for (var i = 0; i < dots.length; i++) {
        dots[i].classList.remove('filled');
    }
}

function goHome() {
    showScreen('homeScreen');
    updateTime();
}

function openApp(appName) {
    if (!playerData.loggedIn && appName !== 'settings') {
        showToast('Morate se prijaviti!');
        showLogin();
        return;
    }

    switch (appName) {
        case 'bounty':
            showScreen('bountyApp');
            break;
        case 'phone':
            showScreen('phoneApp');
            break;
        case 'sms':
            showScreen('smsApp');
            break;
        case 'settings':
            showScreen('settingsApp');
            updateSettingsInfo();
            break;
        default:
            showToast(appName + ' - Uskoro!');
            break;
    }
}

function closeTablet() {
    cefEmit('tablet:close', '');
}

// ===========================================================================
//  LOGIN / REGISTER
// ===========================================================================
function attemptLogin() {
    var pin = document.getElementById('pinInput').value;
    if (pin.length !== 4 || isNaN(pin)) {
        showLoginMessage('PIN mora biti 4 cifre!', 'error');
        return;
    }
    cefEmit('tablet:login:attempt', { pin: pin });
}

function attemptRegister() {
    var pin = document.getElementById('pinInput').value;
    if (pin.length !== 4 || isNaN(pin)) {
        showLoginMessage('PIN mora biti 4 cifre!', 'error');
        return;
    }
    cefEmit('tablet:register:attempt', { pin: pin });
}

function showLoginMessage(msg, type) {
    var el = document.getElementById('loginMessage');
    el.textContent = msg;
    el.className = 'login-message ' + type;
    setTimeout(function() {
        el.textContent = '';
        el.className = 'login-message';
    }, 3000);
}

// Update PIN dots visual
function updatePinDots(length) {
    var dots = document.querySelectorAll('.pin-dot');
    for (var i = 0; i < dots.length; i++) {
        if (i < length) {
            dots[i].classList.add('filled');
        } else {
            dots[i].classList.remove('filled');
        }
    }
}

// ===========================================================================
//  BOUNTY SYSTEM
// ===========================================================================
function placeBounty() {
    var target = document.getElementById('bountyTarget').value.trim();
    var amount = parseInt(document.getElementById('bountyAmount').value);

    if (!target) {
        showToast('Unesite ime igraca!');
        return;
    }
    if (!amount || amount < 1000) {
        showToast('Minimum nagrade je $1,000!');
        return;
    }
    if (amount > 500000) {
        showToast('Maksimum nagrade je $500,000!');
        return;
    }

    cefEmit('tablet:bounty:place', { targetName: target, amount: amount });

    document.getElementById('bountyTarget').value = '';
    document.getElementById('bountyAmount').value = '';
}

function updateBountyList(data) {
    bounties = data.bounties || [];

    document.getElementById('bountyCount').textContent = bounties.length;
    var total = 0;
    for (var i = 0; i < bounties.length; i++) {
        total += bounties[i].amount;
    }
    document.getElementById('bountyTotal').textContent = '$' + total.toLocaleString();

    var listEl = document.getElementById('bountyList');
    if (bounties.length === 0) {
        listEl.innerHTML = '<div class="bounty-empty">Nema aktivnih nagrada</div>';
        return;
    }

    var html = '';
    for (var i = 0; i < bounties.length; i++) {
        var b = bounties[i];
        html += '<div class="bounty-item">';
        html += '  <div style="flex:1">';
        html += '    <div class="bounty-target">' + escapeHtml(b.target) + '</div>';
        html += '    <div class="bounty-placer">Postavio: ' + escapeHtml(b.placer) + '</div>';
        html += '    <div class="bounty-time">' + escapeHtml(b.time) + '</div>';
        html += '  </div>';
        html += '  <div class="bounty-amount">$' + b.amount.toLocaleString() + '</div>';
        if (b.stacked) {
            html += '  <span class="bounty-stacked">STACKED</span>';
        }
        html += '</div>';
    }
    listEl.innerHTML = html;
}

// ===========================================================================
//  PHONE KEYPAD
// ===========================================================================
var dialNumber = '';

function dialKey(key) {
    dialNumber += key;
    document.getElementById('phoneDial').value = dialNumber;
}

function dialClear() {
    dialNumber = dialNumber.slice(0, -1);
    document.getElementById('phoneDial').value = dialNumber;
}

function makeCall() {
    if (dialNumber.length === 0) {
        showToast('Unesite broj!');
        return;
    }
    cefEmit('tablet:phone:call', { number: dialNumber });
    showToast('Pozivam ' + dialNumber + '...');
    dialNumber = '';
    document.getElementById('phoneDial').value = '';
}

// ===========================================================================
//  SMS
// ===========================================================================
function sendSMS() {
    var target = document.getElementById('smsTarget').value.trim();
    var message = document.getElementById('smsMessage').value.trim();

    if (!target) {
        showToast('Unesite broj!');
        return;
    }
    if (!message) {
        showToast('Unesite poruku!');
        return;
    }

    cefEmit('tablet:phone:sms', { target: target, message: message });
    showToast('SMS poslan!');
    document.getElementById('smsTarget').value = '';
    document.getElementById('smsMessage').value = '';
}

// ===========================================================================
//  SETTINGS INFO
// ===========================================================================
function updateSettingsInfo() {
    var phoneEl = document.getElementById('settingsPhone');
    if (phoneEl) phoneEl.textContent = playerData.phone || '---';
    var moneyEl = document.getElementById('settingsMoney');
    if (moneyEl) moneyEl.textContent = '$' + (playerData.money || 0).toLocaleString();
}

// ===========================================================================
//  TOAST NOTIFICATION
// ===========================================================================
function showToast(message) {
    var toast = document.getElementById('toast');
    toast.textContent = message;
    toast.classList.add('show');
    setTimeout(function() {
        toast.classList.remove('show');
    }, 2500);
}

// ===========================================================================
//  TIME UPDATE
// ===========================================================================
function updateTime() {
    var now = new Date();
    var hours = now.getHours().toString().padStart(2, '0');
    var minutes = now.getMinutes().toString().padStart(2, '0');
    var timeStr = hours + ':' + minutes;

    var lockTimeEl = document.getElementById('lockTime');
    if (lockTimeEl) lockTimeEl.textContent = timeStr;

    var statusTimeEl = document.getElementById('statusTime');
    if (statusTimeEl) statusTimeEl.textContent = timeStr;

    var days = ['Nedjelja', 'Ponedjeljak', 'Utorak', 'Srijeda', 'Cetvrtak', 'Petak', 'Subota'];
    var months = ['Sijecnja', 'Veljace', 'Ozujka', 'Travnja', 'Svibnja', 'Lipnja', 'Srpnja', 'Kolovoza', 'Rujna', 'Listopada', 'Studenog', 'Prosinca'];
    var dateStr = days[now.getDay()] + ', ' + now.getDate() + '. ' + months[now.getMonth()];
    var lockDateEl = document.getElementById('lockDate');
    if (lockDateEl) lockDateEl.textContent = dateStr;
}

// ===========================================================================
//  BATTERY UPDATE
// ===========================================================================
function updateBattery(percent) {
    var lockBatteryEl = document.getElementById('lockBattery');
    var lockBatteryFillEl = document.getElementById('lockBatteryFill');
    var statusBatteryEl = document.getElementById('statusBattery');
    var statusBatteryFillEl = document.getElementById('statusBatteryFill');

    var color = percent > 20 ? '#30d158' : '#ff453a';

    if (lockBatteryEl) {
        lockBatteryEl.textContent = percent + '%';
        lockBatteryEl.style.color = color;
    }
    if (lockBatteryFillEl) {
        lockBatteryFillEl.style.width = percent + '%';
        lockBatteryFillEl.style.background = color;
    }
    if (statusBatteryEl) {
        statusBatteryEl.textContent = percent + '%';
        statusBatteryEl.style.color = color;
    }
    if (statusBatteryFillEl) {
        statusBatteryFillEl.style.width = percent + '%';
        statusBatteryFillEl.style.background = color;
    }
}

// ===========================================================================
//  HELPER: HTML escape
// ===========================================================================
function escapeHtml(str) {
    if (!str) return '';
    var div = document.createElement('div');
    div.textContent = str;
    return div.innerHTML;
}

// ===========================================================================
//  SUBSCRIBE NA EVENTE IZ PAWN-A (samp-cef API)
// ===========================================================================
function initSubscriptions() {
    // tablet:init - server salje inicijalne podatke
    cefSubscribe('tablet:init', function(data) {
        try {
            var parsed = JSON.parse(data);
            playerData.username = parsed.username || '';
            playerData.battery = parsed.battery || 100;
            playerData.money = parsed.money || 0;
            playerData.level = parsed.level || 1;
            playerData.online = parsed.online || 0;
            playerData.phone = parsed.phone || 0;

            // Update UI
            updateBattery(playerData.battery);
            document.getElementById('loginUsername').textContent = playerData.username;
            var welcomeEl = document.getElementById('homeWelcome');
            if (welcomeEl) welcomeEl.textContent = 'Dobrodosao, ' + playerData.username;

            updateTime();
            console.log('[CEF] Init data received:', parsed);
        } catch (e) {
            console.log('[CEF] Init parse error:', e);
        }
    });

    // tablet:login:result - rezultat prijave/registracije
    cefSubscribe('tablet:login:result', function(data) {
        try {
            var parsed = JSON.parse(data);
            if (parsed.success) {
                playerData.loggedIn = true;
                showLoginMessage(parsed.message, 'success');
                setTimeout(function() {
                    showScreen('homeScreen');
                    var welcomeEl = document.getElementById('homeWelcome');
                    if (welcomeEl) welcomeEl.textContent = 'Dobrodosao, ' + playerData.username;
                }, 500);
            } else {
                showLoginMessage(parsed.message, 'error');
            }
        } catch (e) {
            console.log('[CEF] Login result parse error:', e);
        }
    });

    // tablet:bounty:update - azurirana bounty lista
    cefSubscribe('tablet:bounty:update', function(data) {
        try {
            var parsed = JSON.parse(data);
            updateBountyList(parsed);
        } catch (e) {
            console.log('[CEF] Bounty update parse error:', e);
        }
    });

    // tablet:toast - toast notifikacija
    cefSubscribe('tablet:toast', function(data) {
        try {
            var parsed = JSON.parse(data);
            showToast(parsed.message);
        } catch (e) {
            showToast(data);
        }
    });
}

// ===========================================================================
//  INITIALIZATION
// ===========================================================================
document.addEventListener('DOMContentLoaded', function() {
    console.log('[UG TABLET] Initializing v2.0...');

    // Setup subscriptions za samp-cef evente
    initSubscriptions();

    // Update time
    updateTime();
    setInterval(updateTime, 30000);

    // Show lock screen
    showScreen('lockScreen');

    // PIN input handling
    var pinInput = document.getElementById('pinInput');
    if (pinInput) {
        pinInput.addEventListener('keydown', function(e) {
            if (e.key === 'Enter') {
                attemptLogin();
            }
        });
        pinInput.addEventListener('input', function() {
            this.value = this.value.replace(/[^0-9]/g, '').slice(0, 4);
            updatePinDots(this.value.length);
        });
    }

    console.log('[UG TABLET] Ready. CEF API available: ' + cefIsAvailable());
});
