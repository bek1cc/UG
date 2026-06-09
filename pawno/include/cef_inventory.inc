// ===========================================================================
//  CEF INVENTORY SISTEM v2.0 za open.mp (omp-cef plugin)
//  File: pawno/include/cef_inventory.inc
//
//  Komande:
//  /inventory  - otvori/zatvori CEF inventar
//  /inv        - isto
//
//  Integracija:
//    #define USE_CEF
//    #include <cef_inventory>
//
//  Kategorije: weapon, drug, material, food, tool, armor, key, license,
//             money, special, phone, seed
// ===========================================================================

#if defined _cef_inventory_included
    #endinput
#endif
#define _cef_inventory_included

#define CEF_INV_RESOURCE     "inventory"
#define CEF_INV_WIDTH        720.0
#define CEF_INV_HEIGHT       560.0
#define CEF_INV_BROWSER_ID   2
#define MAX_INV_SLOTS        30

// ===========================================================================
//  ITEM CATEGORIES (strings match CEF side)
// ===========================================================================
#define INV_CAT_WEAPON      0   // "weapon"
#define INV_CAT_DRUG        1   // "drug"
#define INV_CAT_MATERIAL    2   // "material"
#define INV_CAT_FOOD        3   // "food"
#define INV_CAT_TOOL        4   // "tool"
#define INV_CAT_ARMOR       5   // "armor" (also helmets, vests, misc equip)
#define INV_CAT_KEY         6   // "key"
#define INV_CAT_LICENSE     7   // "license"
#define INV_CAT_MONEY       8   // "money"
#define INV_CAT_SPECIAL     9   // "special" (laptop, dynamite, mask, etc.)
#define INV_CAT_PHONE       10  // "phone"
#define INV_CAT_SEED        11  // "seed"

// ===========================================================================
//  ITEM ICON KEYS (must match CEF ICONS object)
// ===========================================================================
// WEAPONS
#define ICON_BRASS_KNUCKLES  0
#define ICON_PISTOL          1
#define ICON_RIFLE           2
#define ICON_SHOTGUN         3
#define ICON_MELEE           4
#define ICON_EXPLOSIVE       5
// DRUGS
#define ICON_MARIJUANA       10
#define ICON_COCAINE         11
#define ICON_HEROIN          12
#define ICON_METH            13
#define ICON_CRACK           14
#define ICON_LSD             15
// MATERIALS
#define ICON_STONE           20
#define ICON_METAL           21
#define ICON_WOOD            22
#define ICON_IRON            23
#define ICON_PLASTIC         24
#define ICON_SCRAP           25
// FOOD
#define ICON_PIZZA           30
#define ICON_HAMBURGER       31
#define ICON_CHICKEN         32
#define ICON_JUICE           33
#define ICON_BEER            34
// TOOLS
#define ICON_SCREWDRIVER     40
#define ICON_CROWBAR         41
#define ICON_HAMMER          42
#define ICON_FUELCAN         43
#define ICON_DYNAMITE        44
#define ICON_LOCKPICK        45
#define ICON_WRENCH          46
// ARMOR / EQUIP
#define ICON_VEST            50
#define ICON_HELMET          51
#define ICON_MASK            52
// MONEY
#define ICON_WALLET          60
#define ICON_GOLD            61
// KEYS
#define ICON_KEY             70
#define ICON_CAR_KEY         71
// LICENSES
#define ICON_LICENSE         80
#define ICON_GUN_LICENSE     81
#define ICON_ID_CARD         82
// PHONE
#define ICON_PHONE           90
// SEEDS
#define ICON_SEED            100
// SPECIAL
#define ICON_LAPTOP          110
#define ICON_CIGARETTE       111
#define ICON_LIGHTER         112
#define ICON_WATCH           113
#define ICON_INJECTION       114
#define ICON_BOOMBOX         115
#define ICON_INGREDIENT      116
#define ICON_FRUIT           117
#define ICON_WATER           118
#define ICON_GRASS           119
#define ICON_CAMPFIRE        120
#define ICON_ROPE            121
#define ICON_TOOLKIT         122
#define ICON_BAG             123
#define ICON_JEWELRY         124
#define ICON_OPIUM           125

// ===========================================================================
//  ITEM DATA STRUCTURE
// ===========================================================================
enum InvItemData {
    bool:inv_Exists,
    inv_Name[32],
    inv_Category,       // INV_CAT_*
    inv_IconKey,        // ICON_*
    inv_Quantity,
    Float:inv_Weight,
    inv_Value,          // Price / worth
    inv_Ammo,           // Weapon ammo
    inv_Rarity,         // 0=common,1=uncommon,2=rare,3=epic,4=legendary
    inv_Desc[64],
    inv_ScriptID        // For weapons: weapon ID; for items: object model ID
}

// Player inventory data
enum InvPlayerData {
    bool:inv_IsOpen,
    inv_BrowserId,
    Float:inv_TotalWeight
}

static InvData[MAX_PLAYERS][InvPlayerData];
static InvItems[MAX_PLAYERS][MAX_INV_SLOTS][InvItemData];

#if defined USE_CEF

// Omp-CEF native funkcije
#if !defined _INC_cef
    #include <cef>
#endif

// ===========================================================================
//  CATEGORY STRING HELPER
// ===========================================================================
stock Inv_GetCatString(cat, str[], len) {
    switch(cat) {
        case INV_CAT_WEAPON:   format(str, len, "weapon");
        case INV_CAT_DRUG:     format(str, len, "drug");
        case INV_CAT_MATERIAL: format(str, len, "material");
        case INV_CAT_FOOD:     format(str, len, "food");
        case INV_CAT_TOOL:     format(str, len, "tool");
        case INV_CAT_ARMOR:    format(str, len, "armor");
        case INV_CAT_KEY:      format(str, len, "key");
        case INV_CAT_LICENSE:  format(str, len, "license");
        case INV_CAT_MONEY:    format(str, len, "money");
        case INV_CAT_SPECIAL:  format(str, len, "special");
        case INV_CAT_PHONE:    format(str, len, "phone");
        case INV_CAT_SEED:     format(str, len, "seed");
        default:               format(str, len, "special");
    }
}

// ===========================================================================
//  ICON KEY STRING HELPER
// ===========================================================================
stock Inv_GetIconString(iconKey, str[], len) {
    switch(iconKey) {
        case ICON_BRASS_KNUCKLES: format(str, len, "brass_knuckles");
        case ICON_PISTOL:        format(str, len, "pistol");
        case ICON_RIFLE:         format(str, len, "rifle");
        case ICON_SHOTGUN:       format(str, len, "shotgun");
        case ICON_MELEE:         format(str, len, "melee");
        case ICON_EXPLOSIVE:     format(str, len, "explosive");
        case ICON_MARIJUANA:     format(str, len, "marijuana");
        case ICON_COCAINE:       format(str, len, "cocaine");
        case ICON_HEROIN:        format(str, len, "heroin");
        case ICON_METH:          format(str, len, "meth");
        case ICON_CRACK:         format(str, len, "crack");
        case ICON_LSD:           format(str, len, "lsd");
        case ICON_STONE:         format(str, len, "stone");
        case ICON_METAL:         format(str, len, "metal");
        case ICON_WOOD:          format(str, len, "wood");
        case ICON_IRON:          format(str, len, "iron");
        case ICON_PLASTIC:       format(str, len, "plastic");
        case ICON_SCRAP:         format(str, len, "scrap");
        case ICON_PIZZA:         format(str, len, "pizza");
        case ICON_HAMBURGER:     format(str, len, "hamburger");
        case ICON_CHICKEN:       format(str, len, "chicken");
        case ICON_JUICE:         format(str, len, "juice");
        case ICON_BEER:          format(str, len, "beer");
        case ICON_SCREWDRIVER:   format(str, len, "screwdriver");
        case ICON_CROWBAR:       format(str, len, "crowbar");
        case ICON_HAMMER:        format(str, len, "hammer");
        case ICON_FUELCAN:       format(str, len, "fuelcan");
        case ICON_DYNAMITE:      format(str, len, "dynamite");
        case ICON_LOCKPICK:      format(str, len, "lockpick");
        case ICON_WRENCH:        format(str, len, "wrench");
        case ICON_VEST:          format(str, len, "vest");
        case ICON_HELMET:        format(str, len, "helmet");
        case ICON_MASK:          format(str, len, "mask");
        case ICON_WALLET:        format(str, len, "wallet");
        case ICON_GOLD:          format(str, len, "gold");
        case ICON_KEY:           format(str, len, "key");
        case ICON_CAR_KEY:       format(str, len, "car_key");
        case ICON_LICENSE:       format(str, len, "license");
        case ICON_GUN_LICENSE:   format(str, len, "gun_license");
        case ICON_ID_CARD:       format(str, len, "id_card");
        case ICON_PHONE:         format(str, len, "phone");
        case ICON_SEED:          format(str, len, "seed");
        case ICON_LAPTOP:        format(str, len, "laptop");
        case ICON_CIGARETTE:     format(str, len, "cigarette");
        case ICON_LIGHTER:       format(str, len, "lighter");
        case ICON_WATCH:         format(str, len, "watch");
        case ICON_INJECTION:     format(str, len, "injection");
        case ICON_BOOMBOX:       format(str, len, "boombox");
        case ICON_INGREDIENT:    format(str, len, "ingredient");
        case ICON_FRUIT:         format(str, len, "fruit");
        case ICON_WATER:         format(str, len, "water");
        case ICON_GRASS:         format(str, len, "grass");
        case ICON_CAMPFIRE:      format(str, len, "campfire");
        case ICON_ROPE:          format(str, len, "rope");
        case ICON_TOOLKIT:       format(str, len, "toolkit");
        case ICON_BAG:           format(str, len, "bag");
        case ICON_JEWELRY:       format(str, len, "jewelry");
        case ICON_OPIUM:         format(str, len, "opium");
        default:                 format(str, len, "ingredient");
    }
}

// ===========================================================================
//  OTVARANJE INVENTORYJA (OMP-CEF)
// ===========================================================================
stock CEF_InventoryOpen(playerid) {
    if(InvData[playerid][inv_IsOpen]) return;
    if(!IsPlayerConnected(playerid)) return;

    if(!CEF_PlayerHasPlugin(playerid)) {
        SendClientMessage(playerid, 0xF59E0BFF, "[INVENTORY]{FFFFFF} CEF plugin nije detektovan - koristite /inv za textdraw inventory.");
        return;
    }

    InvData[playerid][inv_BrowserId] = CEF_INV_BROWSER_ID;
    CEF_CreateBrowser(playerid, InvData[playerid][inv_BrowserId], "http://cef/inventory/index.html", true, true, CEF_INV_WIDTH, CEF_INV_HEIGHT);

    InvData[playerid][inv_IsOpen] = true;
    TogglePlayerControllable(playerid, false);
    SendClientMessage(playerid, 0x54A0FFFF, "[INVENTORY]{FFFFFF} CEF Inventar otvoren. /inv za zatvaranje.");
}

// ===========================================================================
//  ZATVARANJE INVENTORYJA
// ===========================================================================
stock CEF_InventoryClose(playerid) {
    if(!InvData[playerid][inv_IsOpen]) return;
    CEF_DestroyBrowser(playerid, InvData[playerid][inv_BrowserId]);
    InvData[playerid][inv_IsOpen] = false;
    TogglePlayerControllable(playerid, true);
}

// ===========================================================================
//  SLANJE PODATAKA U CEF (sa svim PlayerInfo itemima)
// ===========================================================================
stock CEF_InvSendData(playerid) {
    if(!InvData[playerid][inv_IsOpen]) return;

    // Prvo posalji player info
    new pName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, pName, sizeof(pName));
    CEF_EmitEvent(playerid, InvData[playerid][inv_BrowserId], "inventory:playerInfo",
        CEF_STR(pName),
        CEF_INT(playerid),
        CEF_INT(0),  // TODO: level
        CEF_INT(GetPlayerMoney(playerid)),
        CEF_INT(0)   // TODO: xp
    );

    // Posalji tezinu
    CEF_EmitEvent(playerid, InvData[playerid][inv_BrowserId], "inventory:weight",
        CEF_INT(floatround(InvData[playerid][inv_TotalWeight])),
        CEF_INT(30)
    );

    // Posalji iteme kao JSON
    new json[4096];
    json = "[";
    new first = 1;
    new catStr[16], iconStr[24];

    for(new i = 0; i < MAX_INV_SLOTS; i++) {
        if(!InvItems[playerid][i][inv_Exists]) continue;
        if(!first) strcat(json, ",");
        first = 0;

        Inv_GetCatString(InvItems[playerid][i][inv_Category], catStr, sizeof(catStr));
        Inv_GetIconString(InvItems[playerid][i][inv_IconKey], iconStr, sizeof(iconStr));

        new entry[256];
        format(entry, sizeof(entry),
            "{\"id\":%d,\"name\":\"%s\",\"category\":\"%s\",\"icon\":\"%s\",\"quantity\":%d,\"weight\":%.1f,\"value\":%d,\"ammo\":%d,\"rarity\":%d,\"desc\":\"%s\"}",
            i,
            InvItems[playerid][i][inv_Name],
            catStr,
            iconStr,
            InvItems[playerid][i][inv_Quantity],
            InvItems[playerid][i][inv_Weight],
            InvItems[playerid][i][inv_Value],
            InvItems[playerid][i][inv_Ammo],
            InvItems[playerid][i][inv_Rarity],
            InvItems[playerid][i][inv_Desc]
        );
        strcat(json, entry);
    }

    strcat(json, "]");
    CEF_EmitEvent(playerid, InvData[playerid][inv_BrowserId], "inventory:init", CEF_STR(json));
}

// ===========================================================================
//  DODAJ PREDMET U INVENTAR
// ===========================================================================
stock CEF_InvAddItem(playerid, name[], category, iconKey, quantity, Float:weight, value = 0, ammo = 0, rarity = 0, desc[] = "") {
    // Prvo probaj stackat na postojeci item (ako je ista kategorija + ime)
    for(new i = 0; i < MAX_INV_SLOTS; i++) {
        if(InvItems[playerid][i][inv_Exists] &&
           strcmp(InvItems[playerid][i][inv_Name], name, true) == 0 &&
           InvItems[playerid][i][inv_Category] == category) {
            InvItems[playerid][i][inv_Quantity] += quantity;
            InvData[playerid][inv_TotalWeight] += weight * quantity;
            if(InvData[playerid][inv_IsOpen]) CEF_InvSendData(playerid);
            return i;
        }
    }

    // Nadji prazan slot
    new slot = -1;
    for(new i = 0; i < MAX_INV_SLOTS; i++) {
        if(!InvItems[playerid][i][inv_Exists]) { slot = i; break; }
    }

    if(slot == -1) {
        SendClientMessage(playerid, 0xFF453AFF, "[INVENTORY]{FFFFFF} Inventar je pun! (30/30)");
        return -1;
    }

    InvItems[playerid][slot][inv_Exists] = true;
    format(InvItems[playerid][slot][inv_Name], 32, "%s", name);
    InvItems[playerid][slot][inv_Category] = category;
    InvItems[playerid][slot][inv_IconKey] = iconKey;
    InvItems[playerid][slot][inv_Quantity] = quantity;
    InvItems[playerid][slot][inv_Weight] = weight;
    InvItems[playerid][slot][inv_Value] = value;
    InvItems[playerid][slot][inv_Ammo] = ammo;
    InvItems[playerid][slot][inv_Rarity] = rarity;
    format(InvItems[playerid][slot][inv_Desc], 64, "%s", desc);
    InvData[playerid][inv_TotalWeight] += weight * quantity;

    if(InvData[playerid][inv_IsOpen]) CEF_InvSendData(playerid);
    return slot;
}

// ===========================================================================
//  UKLONI PREDMET IZ INVENTARA
// ===========================================================================
stock CEF_InvRemoveItem(playerid, slot) {
    if(slot < 0 || slot >= MAX_INV_SLOTS) return 0;
    if(!InvItems[playerid][slot][inv_Exists]) return 0;

    InvData[playerid][inv_TotalWeight] -= InvItems[playerid][slot][inv_Weight] * InvItems[playerid][slot][inv_Quantity];
    if(InvData[playerid][inv_TotalWeight] < 0.0) InvData[playerid][inv_TotalWeight] = 0.0;

    InvItems[playerid][slot][inv_Exists] = false;
    InvItems[playerid][slot][inv_Name][0] = '\0';
    InvItems[playerid][slot][inv_Quantity] = 0;

    if(InvData[playerid][inv_IsOpen]) CEF_InvSendData(playerid);
    return 1;
}

// ===========================================================================
//  UKLONI ODREDJENU KOLICINU IZ SLOT-a
// ===========================================================================
stock CEF_InvRemoveQuantity(playerid, slot, amount) {
    if(slot < 0 || slot >= MAX_INV_SLOTS) return 0;
    if(!InvItems[playerid][slot][inv_Exists]) return 0;
    if(InvItems[playerid][slot][inv_Quantity] < amount) return 0;

    InvItems[playerid][slot][inv_Quantity] -= amount;
    InvData[playerid][inv_TotalWeight] -= InvItems[playerid][slot][inv_Weight] * amount;
    if(InvData[playerid][inv_TotalWeight] < 0.0) InvData[playerid][inv_TotalWeight] = 0.0;

    if(InvItems[playerid][slot][inv_Quantity] <= 0) {
        InvItems[playerid][slot][inv_Exists] = false;
        InvItems[playerid][slot][inv_Name][0] = '\0';
    }

    if(InvData[playerid][inv_IsOpen]) CEF_InvSendData(playerid);
    return 1;
}

// ===========================================================================
//  NADJI ITEM PO IMENU
// ===========================================================================
stock CEF_InvFindItem(playerid, name[]) {
    for(new i = 0; i < MAX_INV_SLOTS; i++) {
        if(InvItems[playerid][i][inv_Exists] && strcmp(InvItems[playerid][i][inv_Name], name, true) == 0)
            return i;
    }
    return -1;
}

// ===========================================================================
//  DOHVATI KOLICINU ITEM-a
// ===========================================================================
stock CEF_InvGetQuantity(playerid, name[]) {
    new slot = CEF_InvFindItem(playerid, name);
    if(slot == -1) return 0;
    return InvItems[playerid][slot][inv_Quantity];
}

// ===========================================================================
//  SYNC IZ PLAYERINFO VARIJABLI U INVENTAR
//  Pozvati pri otvaranju inventara da se svi PlayerInfo itemi prikazu
// ===========================================================================
stock CEF_InvSyncFromPlayerInfo(playerid) {
    // Ova funkcija se poziva iz gamemoda da prebaci sve PlayerInfo
    // varijable u CEF inventar. Implementira se u gamemodu
    // koristeci CEF_InvAddItem().
    // Ovaj stock je placeholder - gamemode ga override-a.
}

// ===========================================================================
//  SUBSCRIPTIONS (OMP-CEF)
// ===========================================================================
stock CEF_InvInitSubscriptions() {
    CEF_AddResource(CEF_INV_RESOURCE);

    CEF_RegisterEvent("inventory:close", "OnInvClose");
    CEF_RegisterEvent("inventory:use", "OnInvUseItem", Argument_Integer);
    CEF_RegisterEvent("inventory:drop", "OnInvDropItem", Argument_Integer);
    CEF_RegisterEvent("inventory:give", "OnInvGiveItem", Argument_Integer);
    CEF_RegisterEvent("inventory:select", "OnInvSelectItem", Argument_Integer);
    print("[CEF INVENTORY v2.0] Subscriptions registrovane (omp-cef API).");
}

forward OnInvClose(playerid);
public OnInvClose(playerid) {
    CEF_InventoryClose(playerid);
}

forward OnInvSelectItem(playerid, slot);
public OnInvSelectItem(playerid, slot) {
    // Samo feedback - moze se koristiti za tooltip info
}

forward OnInvUseItem(playerid, slot);
public OnInvUseItem(playerid, slot) {
    if(slot < 0 || slot >= MAX_INV_SLOTS) return;
    if(!InvItems[playerid][slot][inv_Exists]) return;

    new name[32];
    format(name, sizeof(name), "%s", InvItems[playerid][slot][inv_Name]);

    // Pozovi legacy inventory UseItem handler - koristi ime itema
    // da bi gamemode znao sta da radi
    CallLocalFunction("UseInventoryItem", "ds", playerid, name);
}

forward OnInvDropItem(playerid, slot);
public OnInvDropItem(playerid, slot) {
    if(slot < 0 || slot >= MAX_INV_SLOTS) return;
    if(!InvItems[playerid][slot][inv_Exists]) return;

    new name[32];
    format(name, sizeof(name), "%s", InvItems[playerid][slot][inv_Name]);

    #if defined OnInvDropItemHandler
        OnInvDropItemHandler(playerid, slot, name);
    #endif

    CEF_InvRemoveItem(playerid, slot);
    new msg[64];
    format(msg, sizeof(msg), "[INVENTORY]{FFFFFF} Bacio si: %s", name);
    SendClientMessage(playerid, 0xFF453AFF, msg);
}

forward OnInvGiveItem(playerid, slot);
public OnInvGiveItem(playerid, slot) {
    if(slot < 0 || slot >= MAX_INV_SLOTS) return;
    if(!InvItems[playerid][slot][inv_Exists]) return;
    SendClientMessage(playerid, 0x54A0FFFF, "[INVENTORY]{FFFFFF} Kome zelis dati predmet? Koristi /daj [id]");
}

// ===========================================================================
//  OnCefReady / OnCefBrowserCreated za inventory
// ===========================================================================
stock CefInv_OnCefReady(playerid) {
    if(InvData[playerid][inv_IsOpen]) {
        CEF_InvSendData(playerid);
    }
}

stock CefInv_OnBrowserCreated(playerid, browserid) {
    if(browserid == CEF_INV_BROWSER_ID && InvData[playerid][inv_IsOpen]) {
        CEF_FocusBrowser(playerid, browserid, true);
    }
}

#else

// Fallback bez CEF
stock CEF_InventoryOpen(playerid) {
    SendClientMessage(playerid, 0x54A0FFFF, "[INVENTORY]{FFFFFF} CEF plugin nije instaliran. Koristite textdraw inventar.");
}
stock CEF_InventoryClose(playerid) {}
stock CEF_InvAddItem(playerid, name[], category, iconKey, quantity, Float:weight, value = 0, ammo = 0, rarity = 0, desc[] = "") { return -1; }
stock CEF_InvRemoveItem(playerid, slot) { return 0; }
stock CEF_InvRemoveQuantity(playerid, slot, amount) { return 0; }
stock CEF_InvFindItem(playerid, name[]) { return -1; }
stock CEF_InvGetQuantity(playerid, name[]) { return 0; }
stock CEF_InvInitSubscriptions() {}
stock CEF_InvSyncFromPlayerInfo(playerid) {}

#endif // USE_CEF

// ===========================================================================
//  ON PLAYER DISCONNECT
// ===========================================================================
stock CEF_InvOnPlayerDisconnect(playerid) {
    if(InvData[playerid][inv_IsOpen]) {
        CEF_InventoryClose(playerid);
    }
    // Reset svih slotova
    for(new i = 0; i < MAX_INV_SLOTS; i++) {
        InvItems[playerid][i][inv_Exists] = false;
    }
    InvData[playerid][inv_TotalWeight] = 0.0;
}

// ===========================================================================
//  HELPER: Provjeri da li je inventar otvoren
// ===========================================================================
stock CEF_IsInventoryOpen(playerid) {
    return InvData[playerid][inv_IsOpen];
}

// ===========================================================================
//  ZCMD KOMANDE - premjestene u gamemode (fg-ogc.pwn) sa dodatnim provjerama
//  Ovdje su maknute jer su duplirane u gamemodu i izazivaju error 021
// ===========================================================================
