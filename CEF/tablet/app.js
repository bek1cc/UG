/* ===========================================================================
   UG Tablet - CEF JavaScript
   Koristi pravi samp-cef API: window.cef.emit() / window.cef.subscribe()
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

// ===========================================================================
//  SAMP-CEF API WRAPPER
//  Proverava da li je CEF plugin dostupan i koristi pravi API
// ===========================================================================
function cefIsAvailable() {
    return typeof window.cef !== 'undefined' && window.cef !== null;
}

function cefEmit(eventName, data) {
    if (cefIsAvailable()) {
        // Pravi samp-cef API: window.cef.emit(event, data)
        if (typeof data === 'object') {
            window.cef.emit(eventName, JSON.stringify(data));
        } else {
            window.cef.emit(eventName, String(data));
        }
        console.log('[CEF] Emit: ' + eventName, data);
    } else {
        // Debug mode - bez CEF plugina
        console.log('[CEF DEBUG] Emit: ' + eventName, data);
    }
}

function cefSubscribe(eventName, callback) {
    if (cefIsAvailable()) {
        // Pravi samp-cef API: window.cef.subscribe(event, callback)
        window.cef.subscribe(eventName, callback);
        console.log('[CEF] Subscribed: ' + eventName);
    } else {
        console.log('[CEF DEBUG] Subscribe: ' + eventName);
    }
}

// ===========================================================================
//  SCREEN MANAGEMENT
// ===========================================================================
function showScreen(screenId) {
    // Hide all screens
    var screens = document.querySelectorAll('.screen');
    for (var i = 0; i < screens.length; i++) {
        screens[i].classList.remove('active');
    }
    // Show target screen
    var target = document.getElementById(screenId);
    if (target) {
        target.classList.add('active');
        currentScreen = screenId;
    }
}

function showLogin() {
    showScreen('loginScreen');
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

    // Clear form
    document.getElementById('bountyTarget').value = '';
    document.getElementById('bountyAmount').value = '';
}

function updateBountyList(data) {
    bounties = data.bounties || [];

    // Update stats
    document.getElementById('bountyCount').textContent = bounties.length;
    var total = 0;
    for (var i = 0; i < bounties.length; i++) {
        total += bounties[i].amount;
    }
    document.getElementById('bountyTotal').textContent = '$' + total.toLocaleString();

    // Update list
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

    // Lock screen
    var lockTimeEl = document.getElementById('lockTime');
    if (lockTimeEl) lockTimeEl.textContent = timeStr;

    // Status bar
    var statusTimeEl = document.getElementById('statusTime');
    if (statusTimeEl) statusTimeEl.textContent = timeStr;

    // Lock screen date
    var days = ['Nedjelja', 'Ponedjeljak', 'Utorak', 'Srijeda', 'Cetvrtak', 'Petak', 'Subota'];
    var months = ['Sijecnja', 'Veljace', 'Ozujka', 'Travnja', 'Svibnja', 'Lipnja', 'Srpnja', 'Kolovoza', 'Rujna', 'Listopada', 'Studenog', 'Prosinca'];
    var dateStr = days[now.getDay()] + ', ' + now.getDate() + '. ' + months[now.getMonth()];
    var lockDateEl = document.getElementById('lockDate');
    if (lockDateEl) lockDateEl.textContent = dateStr;
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
            document.getElementById('lockBattery').textContent = playerData.battery;
            document.getElementById('statusBattery').textContent = playerData.battery + '%';
            document.getElementById('loginUsername').textContent = playerData.username;

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
                }, 500);
            } else {
                showLoginMessage(parsed.message, 'error');
            }
        } catch (e) {
            console.log('[CEF] Login result parse error:', e);
        }
    });

    // tablet:login:success - login uspjesan sa bounty podacima
    cefSubscribe('tablet:login:success', function(data) {
        try {
            var parsed = JSON.parse(data);
            playerData.loggedIn = true;
            if (parsed.bounties) {
                updateBountyList({ bounties: parsed.bounties });
            }
        } catch (e) {
            console.log('[CEF] Login success parse error:', e);
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
    console.log('[UG TABLET] Initializing...');

    // Setup subscriptions za samp-cef evente
    initSubscriptions();

    // Update time
    updateTime();
    setInterval(updateTime, 30000);

    // Show lock screen
    showScreen('lockScreen');

    // PIN input - enter za submit
    var pinInput = document.getElementById('pinInput');
    if (pinInput) {
        pinInput.addEventListener('keydown', function(e) {
            if (e.key === 'Enter') {
                attemptLogin();
            }
        });
        // Dozvoli samo brojeve
        pinInput.addEventListener('input', function() {
            this.value = this.value.replace(/[^0-9]/g, '').slice(0, 4);
        });
    }

    console.log('[UG TABLET] Ready. CEF API available: ' + cefIsAvailable());
});
