// ===========================================================================
//  CEF INVENTORY SISTEM za open.mp (omp-cef plugin)
//  File: pawno/include/cef_inventory.inc
//
//  Komande:
//  /inventory  - otvori/zatvori CEF inventar
//  /inv        - isto
//
//  Integracija:
//    #define USE_CEF
//    #include <cef_inventory>
// ===========================================================================

#if defined _cef_inventory_included
    #endinput
#endif
#define _cef_inventory_included

#define CEF_INV_RESOURCE     "cef/inventory"
#define CEF_INV_WIDTH        600.0
#define CEF_INV_HEIGHT       500.0
#define CEF_INV_BROWSER_ID   2
#define MAX_INV_SLOTS        20

// Item categories
#define INV_CAT_WEAPON      0
#define INV_CAT_ITEM        1
#define INV_CAT_KEY         2
#define INV_CAT_DRUG        3
#define INV_CAT_FOOD        4

// Item data
enum InvItemData {
    bool:inv_Exists,
    inv_Name[32],
    inv_Category,
    inv_Quantity,
    Float:inv_Weight,
    Float:inv_Value,
    inv_Ammo,
    inv_Desc[64],
    inv_ScriptID
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
//  OTVARANJE INVENTORYJA (OMP-CEF)
// ===========================================================================
stock CEF_InventoryOpen(playerid) {
    if(InvData[playerid][inv_IsOpen]) return;
    if(!IsPlayerConnected(playerid)) return;

    if(!CEF_PlayerHasPlugin(playerid)) {
        SendClientMessage(playerid, 0xFF453AFF, "[INVENTORY]{FFFFFF} Nemate CEF plugin! Koristite /inv za textdraw.");
        return;
    }

    InvData[playerid][inv_BrowserId] = CEF_INV_BROWSER_ID;
    CEF_CreateBrowser(playerid, InvData[playerid][inv_BrowserId], "http://web/inventory/index.html", true, true, CEF_INV_WIDTH, CEF_INV_HEIGHT);

    InvData[playerid][inv_IsOpen] = true;

    TogglePlayerControllable(playerid, false);
    SendClientMessage(playerid, 0x54A0FFFF, "[INVENTORY]{FFFFFF} CEF Inventar otvoren.");
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
//  SLANJE PODATAKA U CEF
// ===========================================================================
stock CEF_InvSendData(playerid) {
    if(!InvData[playerid][inv_IsOpen]) return;
    new json[2048];
    json = CEF_InvFormatJSON(playerid);
    CEF_EmitEvent(playerid, InvData[playerid][inv_BrowserId], "inventory:data", CEF_STR(json));
}

stock CEF_InvFormatJSON(playerid) {
    new json[2048];
    format(json, sizeof(json), "[");
    new first = 1;

    for(new i = 0; i < MAX_INV_SLOTS; i++) {
        if(!InvItems[playerid][i][inv_Exists]) continue;

        if(!first) strcat(json, ",");
        first = 0;

        new catStr[16];
        switch(InvItems[playerid][i][inv_Category]) {
            case INV_CAT_WEAPON: catStr = "weapon";
            case INV_CAT_ITEM: catStr = "item";
            case INV_CAT_KEY: catStr = "key";
            case INV_CAT_DRUG: catStr = "drug";
            case INV_CAT_FOOD: catStr = "food";
            default: catStr = "item";
        }

        new entry[256];
        format(entry, sizeof(entry),
            "{\"id\":%d,\"name\":\"%s\",\"category\":\"%s\",\"quantity\":%d,\"weight\":%.1f,\"damage\":0,\"ammo\":%d,\"description\":\"%s\"}",
            i,
            InvItems[playerid][i][inv_Name],
            catStr,
            InvItems[playerid][i][inv_Quantity],
            InvItems[playerid][i][inv_Weight],
            InvItems[playerid][i][inv_Ammo],
            InvItems[playerid][i][inv_Desc]
        );
        strcat(json, entry);
    }

    strcat(json, "]");
    return json;
}

// ===========================================================================
//  DODAJ PREDMET U INVENTAR
// ===========================================================================
stock CEF_InvAddItem(playerid, name[], category, quantity, Float:weight, ammo = 0, desc[] = "") {
    new slot = -1;
    for(new i = 0; i < MAX_INV_SLOTS; i++) {
        if(!InvItems[playerid][i][inv_Exists]) { slot = i; break; }
    }

    if(slot == -1) {
        SendClientMessage(playerid, 0xFF453AFF, "[INVENTORY]{FFFFFF} Inventar je pun!");
        return -1;
    }

    for(new i = 0; i < MAX_INV_SLOTS; i++) {
        if(InvItems[playerid][i][inv_Exists] &&
           strcmp(InvItems[playerid][i][inv_Name], name, true) == 0 &&
           InvItems[playerid][i][inv_Category] == category) {
            InvItems[playerid][i][inv_Quantity] += quantity;
            InvData[playerid][inv_TotalWeight] += weight;
            if(InvData[playerid][inv_IsOpen]) CEF_InvSendData(playerid);
            return i;
        }
    }

    InvItems[playerid][slot][inv_Exists] = true;
    format(InvItems[playerid][slot][inv_Name], 32, "%s", name);
    InvItems[playerid][slot][inv_Category] = category;
    InvItems[playerid][slot][inv_Quantity] = quantity;
    InvItems[playerid][slot][inv_Weight] = weight;
    InvItems[playerid][slot][inv_Ammo] = ammo;
    format(InvItems[playerid][slot][inv_Desc], 64, "%s", desc);
    InvData[playerid][inv_TotalWeight] += weight;

    if(InvData[playerid][inv_IsOpen]) CEF_InvSendData(playerid);
    return slot;
}

// ===========================================================================
//  UKLONI PREDMET IZ INVENTARA
// ===========================================================================
stock CEF_InvRemoveItem(playerid, slot) {
    if(slot < 0 || slot >= MAX_INV_SLOTS) return 0;
    if(!InvItems[playerid][slot][inv_Exists]) return 0;

    InvData[playerid][inv_TotalWeight] -= InvItems[playerid][slot][inv_Weight];
    if(InvData[playerid][inv_TotalWeight] < 0.0) InvData[playerid][inv_TotalWeight] = 0.0;

    InvItems[playerid][slot][inv_Exists] = false;
    InvItems[playerid][slot][inv_Name][0] = '\0';
    InvItems[playerid][slot][inv_Quantity] = 0;

    if(InvData[playerid][inv_IsOpen]) CEF_InvSendData(playerid);
    return 1;
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
    print("[CEF INVENTORY] Subscriptions registrovane (omp-cef API).");
}

forward OnInvClose(playerid);
public OnInvClose(playerid) {
    CEF_InventoryClose(playerid);
}

forward OnInvUseItem(playerid, slot);
public OnInvUseItem(playerid, slot) {
    if(slot < 0 || slot >= MAX_INV_SLOTS) return;
    if(!InvItems[playerid][slot][inv_Exists]) return;

    new msg[64];
    format(msg, sizeof(msg), "[INVENTORY]{FFFFFF} Koristis: %s", InvItems[playerid][slot][inv_Name]);
    SendClientMessage(playerid, 0x54A0FFFF, msg);
}

forward OnInvDropItem(playerid, slot);
public OnInvDropItem(playerid, slot) {
    if(slot < 0 || slot >= MAX_INV_SLOTS) return;
    if(!InvItems[playerid][slot][inv_Exists]) return;

    new msg[64];
    format(msg, sizeof(msg), "[INVENTORY]{FFFFFF} Bacio si: %s", InvItems[playerid][slot][inv_Name]);
    SendClientMessage(playerid, 0xFF453AFF, msg);
    CEF_InvRemoveItem(playerid, slot);
}

forward OnInvGiveItem(playerid, slot);
public OnInvGiveItem(playerid, slot) {
    if(slot < 0 || slot >= MAX_INV_SLOTS) return;
    if(!InvItems[playerid][slot][inv_Exists]) return;
    SendClientMessage(playerid, 0x54A0FFFF, "[INVENTORY]{FFFFFF} Kome zelis dati predmet? Koristi /giveitem [id]");
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
stock CEF_InvAddItem(playerid, name[], category, quantity, Float:weight, ammo = 0, desc[] = "") { return -1; }
stock CEF_InvRemoveItem(playerid, slot) { return 0; }
stock CEF_InvInitSubscriptions() {}

#endif // USE_CEF

// ===========================================================================
//  ON PLAYER DISCONNECT
// ===========================================================================
stock CEF_InvOnPlayerDisconnect(playerid) {
    if(InvData[playerid][inv_IsOpen]) {
        CEF_InventoryClose(playerid);
    }
}

// ===========================================================================
//  HELPER: Provjeri da li je inventar otvoren
// ===========================================================================
stock CEF_IsInventoryOpen(playerid) {
    return InvData[playerid][inv_IsOpen];
}
