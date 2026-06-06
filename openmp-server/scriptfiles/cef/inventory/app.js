// ===========================================================================
//  Unicate Gaming Inventory - CEF App (samp-cef API)
// ===========================================================================

var inventoryItems = [];
var selectedItem = null;
var currentCategory = 'all';

// Item icon mapping
var ICON_MAP = {
    // Weapons
    '9mm': '&#128296;', 'Silenced': '&#128296;', 'Deagle': '&#128296;',
    'Shotgun': '&#128296;', 'AK-47': '&#128296;', 'M4': '&#128296;',
    'MP5': '&#128296;', 'Sniper': '&#128296;', 'Rifle': '&#128296;',
    'weapon': '&#9876;',
    // Items
    'phone': '&#128241;', 'radio': '&#128266;', 'mask': '&#127910;',
    'armor': '&#128737;', 'medkit': '&#9971;', 'bandage': '&#9971;',
    'item': '&#128230;',
    // Keys
    'key': '&#128273;', 'house_key': '&#127968;', 'car_key': '&#128663;',
    'business_key': '&#127970;',
    // Drugs
    'marijuana': '&#127810;', 'cocaine': '&#128142;', 'heroin': '&#128142;',
    'drug': '&#128142;',
    // Food
    'pizza': '&#127829;', 'burger': '&#127828;', 'water': '&#127861;',
    'food': '&#127869;',
    // Other
    'money_bag': '&#128176;', 'materials': '&#9881;', 'fuel': '&#9981;',
    'toolkit': '&#128295;'
};

// Category colors
var CAT_COLORS = {
    'weapon': 'cat-weapon', 'item': 'cat-item', 'key': 'cat-key',
    'drug': 'cat-drug', 'food': 'cat-food'
};

document.addEventListener('DOMContentLoaded', function() {
    subscribeToEvents();
    loadDemoData();
});

function emitToServer(eventName) {
    var args = Array.prototype.slice.call(arguments, 1);
    try {
        if (window.cef && window.cef.emit) {
            window.cef.emit.apply(window.cef, [eventName].concat(args));
        } else {
            console.log('[CEF emit]', eventName, args);
        }
    } catch (e) { console.error('[CEF emit error]', e); }
}

function subscribeToEvents() {
    try {
        if (window.cef && window.cef.subscribe) {
            window.cef.subscribe('inventory:data', function(json) {
                try { inventoryItems = JSON.parse(json); } catch(e) {}
                renderInventory();
            });
            window.cef.subscribe('inventory:update', function(json) {
                try { inventoryItems = JSON.parse(json); } catch(e) {}
                renderInventory();
            });
            window.cef.subscribe('inventory:notify', function(msg) {
                showToast(msg);
            });
        } else {
            loadDemoData();
        }
    } catch(e) { loadDemoData(); }
}

function getItemIcon(item) {
    if (item.icon) return item.icon;
    for (var key in ICON_MAP) {
        if (item.name && item.name.toLowerCase().indexOf(key.toLowerCase()) !== -1) {
            return ICON_MAP[key];
        }
    }
    return ICON_MAP[item.category] || '&#128230;';
}

function renderInventory() {
    var grid = document.getElementById('inventoryGrid');
    grid.innerHTML = '';

    var filtered = currentCategory === 'all'
        ? inventoryItems
        : inventoryItems.filter(function(i) { return i.category === currentCategory; });

    // Max 20 slots
    var maxSlots = 20;
    for (var i = 0; i < maxSlots; i++) {
        var item = filtered[i] || null;
        var slot = document.createElement('div');

        if (item) {
            slot.className = 'inv-slot ' + (CAT_COLORS[item.category] || '');
            if (selectedItem && selectedItem.id === item.id) slot.classList.add('selected');
            slot.innerHTML =
                '<div class="inv-icon">' + getItemIcon(item) + '</div>' +
                '<div class="inv-name">' + escapeHTML(item.name) + '</div>' +
                (item.quantity > 1 ? '<div class="inv-quantity">x' + item.quantity + '</div>' : '');
            slot.onclick = (function(it) { return function() { selectItem(it); }; })(item);
        } else {
            slot.className = 'inv-slot empty';
        }

        grid.appendChild(slot);
    }

    // Update weight
    var totalWeight = inventoryItems.reduce(function(sum, i) { return sum + (i.weight || 0); }, 0);
    document.getElementById('weightBadge').textContent = totalWeight.toFixed(1) + '/20 kg';
}

function filterCategory(cat) {
    currentCategory = cat;
    selectedItem = null;

    document.querySelectorAll('.cat-tab').forEach(function(t) {
        t.classList.toggle('active', t.getAttribute('data-cat') === cat);
    });

    closeDetail();
    renderInventory();
}

function selectItem(item) {
    selectedItem = item;
    renderInventory();
    showDetail(item);
}

function showDetail(item) {
    var panel = document.getElementById('detailPanel');
    panel.style.display = 'block';

    document.getElementById('detailIcon').innerHTML = getItemIcon(item);
    document.getElementById('detailName').textContent = item.name;
    document.getElementById('detailType').textContent = item.category || 'Predmet';
    document.getElementById('detailDesc').textContent = item.description || 'Nema opisa.';

    // Stats
    var statsHtml = '';
    if (item.damage) statsHtml += '<div class="detail-stat"><strong>Steta:</strong> ' + item.damage + '</div>';
    if (item.ammo) statsHtml += '<div class="detail-stat"><strong>Municija:</strong> ' + item.ammo + '</div>';
    if (item.weight) statsHtml += '<div class="detail-stat"><strong>Tezina:</strong> ' + item.weight + 'kg</div>';
    if (item.value) statsHtml += '<div class="detail-stat"><strong>Vrijednost:</strong> $' + item.value + '</div>';
    document.getElementById('detailStats').innerHTML = statsHtml;

    // Actions
    var actionsHtml = '';
    if (item.category === 'weapon') {
        actionsHtml += '<button class="btn-use" onclick="useItem(' + item.id + ')">Opremi</button>';
    } else if (item.category === 'food') {
        actionsHtml += '<button class="btn-use" onclick="useItem(' + item.id + ')">Koristi</button>';
    } else if (item.category === 'drug') {
        actionsHtml += '<button class="btn-use" onclick="useItem(' + item.id + ')">Koristi</button>';
    } else {
        actionsHtml += '<button class="btn-use" onclick="useItem(' + item.id + ')">Koristi</button>';
    }
    actionsHtml += '<button class="btn-give" onclick="giveItem(' + item.id + ')">Daj</button>';
    actionsHtml += '<button class="btn-drop" onclick="dropItem(' + item.id + ')">Baci</button>';
    document.getElementById('detailActions').innerHTML = actionsHtml;
}

function closeDetail() {
    document.getElementById('detailPanel').style.display = 'none';
    selectedItem = null;
    renderInventory();
}

function useItem(id) { emitToServer('inventory:use', id); showToast('Koristis predmet...'); closeDetail(); }
function dropItem(id) { emitToServer('inventory:drop', id); showToast('Bacis predmet...'); closeDetail(); }
function giveItem(id) { emitToServer('inventory:give', id); showToast('Kome das predmet?'); closeDetail(); }
function closeInventory() { emitToServer('inventory:close'); }

function showToast(message) {
    var existing = document.querySelector('.toast');
    if (existing) existing.remove();
    var toast = document.createElement('div');
    toast.className = 'toast';
    toast.textContent = message;
    document.body.appendChild(toast);
    setTimeout(function() { toast.classList.add('show'); }, 10);
    setTimeout(function() { toast.classList.remove('show'); setTimeout(function() { toast.remove(); }, 300); }, 2500);
}

function loadDemoData() {
    inventoryItems = [
        { id: 1, name: '9mm Pistol', category: 'weapon', quantity: 1, ammo: 50, damage: 15, weight: 1.0, description: 'Standardni pistolj sa 9mm municijom.' },
        { id: 2, name: 'AK-47', category: 'weapon', quantity: 1, ammo: 120, damage: 35, weight: 3.5, description: 'Automatska puska Kalasnjikov.' },
        { id: 3, name: 'Telefon', category: 'item', quantity: 1, weight: 0.2, description: 'Tvoj iPhone sa UG mrezom.' },
        { id: 4, name: 'Radio', category: 'item', quantity: 1, weight: 0.3, description: 'Radio za komunikaciju sa organizacijom.' },
        { id: 5, name: 'Kucni kljuc', category: 'key', quantity: 1, weight: 0.1, description: 'Kljuc od tvoje kuce #42.' },
        { id: 6, name: 'Auto kljuc', category: 'key', quantity: 1, weight: 0.1, description: 'Kljuc od vozila Sultan.' },
        { id: 7, name: 'Marihuana', category: 'drug', quantity: 5, weight: 0.5, value: 3000, description: '5g marihuane.' },
        { id: 8, name: 'Pizza', category: 'food', quantity: 2, weight: 0.4, description: 'Vruc pizza iz Pizzerie.' },
        { id: 9, name: 'Complete kit', category: 'item', quantity: 1, weight: 1.0, description: 'Komplet za popravku vozila.' },
        { id: 10, name: 'Sniper Rifle', category: 'weapon', quantity: 1, ammo: 20, damage: 80, weight: 5.0, description: 'Snajperska puska za dugometne mete.' },
        { id: 11, name: 'Bandage', category: 'item', quantity: 3, weight: 0.2, description: 'Zavoj za saniranje rana.' },
        { id: 12, name: 'Materijali', category: 'item', quantity: 50, weight: 2.5, value: 500, description: '50 paketa materijala za oruzje.' },
    ];

    document.getElementById('invPlayerName').textContent = 'Demo Igrac';
    document.getElementById('invPlayerPhone').textContent = 'Tel: 123456';
    document.getElementById('invMoney').textContent = '$125,000';

    renderInventory();
}

function escapeHTML(str) {
    if (!str) return '';
    var div = document.createElement('div');
    div.textContent = str;
    return div.innerHTML;
}
