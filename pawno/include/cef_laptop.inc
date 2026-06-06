// ===========================================================================
//  CEF LAPTOP SISTEM za SA-MP (samp-cef plugin)
//  File: pawno/include/cef_laptop.inc
//
//  Realisticki laptop UI sa aplikacijama:
//  - Dark Web Marketplace (anonimne narudzbe oruzja/droga)
//  - Banka (uplata/isplata/transfer)
//  - BountyNet (mrezni bounty portal)
//  - Email, Terminal, Settings
//
//  Komande:
//  /laptop     - otvori/zatvori laptop
//  /lp         - skraceno
//
//  Integracija:
//    #define USE_CEF
//    #include <cef_laptop>
// ===========================================================================

#if defined _cef_laptop_included
    #endinput
#endif
#define _cef_laptop_included

#define CEF_LP_URL          "CEF/laptop/index.html"
#define CEF_LP_WIDTH        700
#define CEF_LP_HEIGHT       550
#define CEF_LP_BROWSER_ID   3

enum LaptopPlayerData {
    bool:lp_IsOpen,
    lp_BrowserId,
    lp_DWCooldown,
    bool:lp_BankLoggedIn
}

static LaptopData[MAX_PLAYERS][LaptopPlayerData];

#if defined USE_CEF

#if !defined _cef_natives_defined
native cef_create_browser(playerid, browserid, const url[], bool:hidden = false, bool:fullscreen = false);
native cef_destroy_browser(playerid, browserid);
native cef_hide_browser(playerid, browserid, bool:hide);
native cef_set_browser_size(playerid, browserid, width, height);
native cef_set_browser_pos(playerid, browserid, x, y);
native cef_emit_event(const name[], const data[]);
native cef_subscribe(const name[], const callback[]);
native cef_player_has_plugin(playerid);
#define _cef_natives_defined
#endif

// ===========================================================================
//  OTVARANJE LAPTOPA (CEF)
// ===========================================================================
stock CEF_LaptopOpen(playerid) {
    if(LaptopData[playerid][lp_IsOpen]) return;
    if(!IsPlayerConnected(playerid)) return;

    if(!cef_player_has_plugin(playerid)) {
        SendClientMessage(playerid, 0xFF453AFF, "[LAPTOP]{FFFFFF} Nemate CEF plugin!");
        return;
    }

    LaptopData[playerid][lp_BrowserId] = CEF_LP_BROWSER_ID;
    cef_create_browser(playerid, LaptopData[playerid][lp_BrowserId], CEF_LP_URL, true, false);
    cef_set_browser_pos(playerid, LaptopData[playerid][lp_BrowserId], 120, 20);
    cef_set_browser_size(playerid, LaptopData[playerid][lp_BrowserId], CEF_LP_WIDTH, CEF_LP_HEIGHT);
    cef_hide_browser(playerid, LaptopData[playerid][lp_BrowserId], false);

    LaptopData[playerid][lp_IsOpen] = true;

    // Posalji init podatke
    SetTimerEx("CEF_LpSendInit", 1000, false, "i", playerid);

    TogglePlayerControllable(playerid, false);
    SendClientMessage(playerid, 0x54A0FFFF, "[LAPTOP]{FFFFFF} CEF Laptop otvoren. Koristi /laptop za zatvaranje.");
}

// ===========================================================================
//  ZATVARANJE LAPTOPA
// ===========================================================================
stock CEF_LaptopClose(playerid) {
    if(!LaptopData[playerid][lp_IsOpen]) return;
    cef_destroy_browser(playerid, LaptopData[playerid][lp_BrowserId]);
    LaptopData[playerid][lp_IsOpen] = false;
    LaptopData[playerid][lp_BankLoggedIn] = false;
    TogglePlayerControllable(playerid, true);
}

// ===========================================================================
//  INIT DATA
// ===========================================================================
forward CEF_LpSendInit(playerid);
public CEF_LpSendInit(playerid) {
    if(!LaptopData[playerid][lp_IsOpen]) return;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, sizeof(playerName));

    new json[256];
    format(json, sizeof(json), "{\"username\":\"%s\",\"money\":%d,\"bank\":%d}",
        playerName, GetPlayerMoney(playerid), GetPlayerMoney(playerid)); // TODO: Pravi bank balance

    cef_emit_event(playerid, "laptop:init", json);
}

// ===========================================================================
//  SUBSCRIPTIONS
// ===========================================================================
stock CEF_LpInitSubscriptions() {
    cef_subscribe("laptop:close", "OnLaptopClose");
    cef_subscribe("laptop:darkweb:buy", "OnLaptopDarkWebBuy");
    cef_subscribe("laptop:bank:deposit", "OnLaptopBankDeposit");
    cef_subscribe("laptop:bank:withdraw", "OnLaptopBankWithdraw");
    cef_subscribe("laptop:bank:transfer", "OnLaptopBankTransfer");
    cef_subscribe("laptop:bounty:place", "OnLaptopBountyPlace");
    cef_subscribe("laptop:email:send", "OnLaptopEmailSend");
    cef_subscribe("laptop:terminal", "OnLaptopTerminal");
    print("[CEF LAPTOP] Subscriptions registrovane.");
}

// ===========================================================================
//  EVENT CALLBACKS
// ===========================================================================
forward OnLaptopClose(playerid);
public OnLaptopClose(playerid) {
    CEF_LaptopClose(playerid);
    SendClientMessage(playerid, 0x54A0FFFF, "[LAPTOP]{FFFFFF} Laptop iskljucen.");
}

forward OnLaptopDarkWebBuy(playerid, cmd[]);
public OnLaptopDarkWebBuy(playerid, cmd[]) {
    if(!LaptopData[playerid][lp_IsOpen]) return;

    // Cooldown check (5 min)
    if((gettime() - LaptopData[playerid][lp_DWCooldown]) < 300) {
        new remaining = 300 - (gettime() - LaptopData[playerid][lp_DWCooldown]);
        new msg[64];
        format(msg, sizeof(msg), "Pricekaj %d sekundi!", remaining);
        cef_emit_event(playerid, "laptop:notify", msg);
        return;
    }

    LaptopData[playerid][lp_DWCooldown] = gettime();

    // Process purchase
    new msg[128];
    if(strcmp(cmd, "pistol", true) == 0) {
        if(GetPlayerMoney(playerid) < 5000) { cef_emit_event(playerid, "laptop:notify", "Nemate dovoljno novca!"); return; }
        GivePlayerMoney(playerid, -5000);
        GivePlayerWeapon(playerid, 22, 50);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste Pistol 9mm za $5,000. Isporuka na tajnoj lokaciji!");
    } else if(strcmp(cmd, "deagle", true) == 0) {
        if(GetPlayerMoney(playerid) < 15000) { cef_emit_event(playerid, "laptop:notify", "Nemate dovoljno novca!"); return; }
        GivePlayerMoney(playerid, -15000);
        GivePlayerWeapon(playerid, 24, 30);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste Deagle za $15,000.");
    } else if(strcmp(cmd, "ak47", true) == 0) {
        if(GetPlayerMoney(playerid) < 25000) { cef_emit_event(playerid, "laptop:notify", "Nemate dovoljno novca!"); return; }
        GivePlayerMoney(playerid, -25000);
        GivePlayerWeapon(playerid, 30, 120);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste AK-47 za $25,000.");
    } else if(strcmp(cmd, "m4", true) == 0) {
        if(GetPlayerMoney(playerid) < 30000) { cef_emit_event(playerid, "laptop:notify", "Nemate dovoljno novca!"); return; }
        GivePlayerMoney(playerid, -30000);
        GivePlayerWeapon(playerid, 31, 120);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste M4 za $30,000.");
    } else if(strcmp(cmd, "sniper", true) == 0) {
        if(GetPlayerMoney(playerid) < 50000) { cef_emit_event(playerid, "laptop:notify", "Nemate dovoljno novca!"); return; }
        GivePlayerMoney(playerid, -50000);
        GivePlayerWeapon(playerid, 34, 20);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste Sniper Rifle za $50,000.");
    } else if(strcmp(cmd, "weed", true) == 0) {
        if(GetPlayerMoney(playerid) < 3000) { cef_emit_event(playerid, "laptop:notify", "Nemate dovoljno novca!"); return; }
        GivePlayerMoney(playerid, -3000);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste Marihuanu x5 za $3,000.");
    } else if(strcmp(cmd, "coke", true) == 0) {
        if(GetPlayerMoney(playerid) < 8000) { cef_emit_event(playerid, "laptop:notify", "Nemate dovoljno novca!"); return; }
        GivePlayerMoney(playerid, -8000);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste Kokain x5 za $8,000.");
    } else {
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Artikal nije pronadjen.");
    }

    SendClientMessage(playerid, 0xFF453AFF, msg);
}

forward OnLaptopBankDeposit(playerid, amountStr[]);
public OnLaptopBankDeposit(playerid, amountStr[]) {
    if(!LaptopData[playerid][lp_IsOpen]) return;
    new amount = strval(amountStr);
    if(amount <= 0) { cef_emit_event(playerid, "laptop:notify", "Nevazeci iznos!"); return; }
    if(GetPlayerMoney(playerid) < amount) { cef_emit_event(playerid, "laptop:notify", "Nemate dovoljno novca!"); return; }
    // TODO: Dodaj na bank racun
    GivePlayerMoney(playerid, -amount);
    new msg[64];
    format(msg, sizeof(msg), "[BANKA]{FFFFFF} Uplaceno $%d na racun.", amount);
    SendClientMessage(playerid, 0x30D158FF, msg);
}

forward OnLaptopBankWithdraw(playerid, amountStr[]);
public OnLaptopBankWithdraw(playerid, amountStr[]) {
    if(!LaptopData[playerid][lp_IsOpen]) return;
    new amount = strval(amountStr);
    if(amount <= 0) { cef_emit_event(playerid, "laptop:notify", "Nevazeci iznos!"); return; }
    // TODO: Provjeri bank stanje, skinis sa racuna
    GivePlayerMoney(playerid, amount);
    new msg[64];
    format(msg, sizeof(msg), "[BANKA]{FFFFFF} Isplaceno $%d sa racuna.", amount);
    SendClientMessage(playerid, 0x30D158FF, msg);
}

forward OnLaptopBankTransfer(playerid, data[]);
public OnLaptopBankTransfer(playerid, data[]) {
    if(!LaptopData[playerid][lp_IsOpen]) return;
    // Parse: targetName,amount
    // TODO: Implementirati transfer
    SendClientMessage(playerid, 0x54A0FFFF, "[BANKA]{FFFFFF} Transfer u obradi...");
}

forward OnLaptopBountyPlace(playerid, data[]);
public OnLaptopBountyPlace(playerid, data[]) {
    if(!LaptopData[playerid][lp_IsOpen]) return;
    // Koristi isti sistem kao cef_tablet
    #if defined _cef_tablet_included
        OnTabletBountyPlace(playerid, data);
    #endif
}

forward OnLaptopEmailSend(playerid, to[], subj[], body[]);
public OnLaptopEmailSend(playerid, to[], subj[], body[]) {
    if(!LaptopData[playerid][lp_IsOpen]) return;
    // TODO: Implementirati email sistem
    SendClientMessage(playerid, 0x54A0FFFF, "[EMAIL]{FFFFFF} Poruka poslata!");
}

forward OnLaptopTerminal(playerid, cmd[]);
public OnLaptopTerminal(playerid, cmd[]) {
    if(!LaptopData[playerid][lp_IsOpen]) return;
    // TODO: Implementirati terminal komande
    print("[LAPTOP TERMINAL] Command from player");
}

#else

// Fallback bez CEF
stock CEF_LaptopOpen(playerid) {
    SendClientMessage(playerid, 0x54A0FFFF, "[LAPTOP]{FFFFFF} CEF plugin nije instaliran.");
}
stock CEF_LaptopClose(playerid) {}
stock CEF_LpInitSubscriptions() {}

#endif // USE_CEF

// ===========================================================================
//  KOMANDE
// ===========================================================================
CMD:laptop(playerid, params[]) {
    #pragma unused params
    if(LaptopData[playerid][lp_IsOpen]) {
        CEF_LaptopClose(playerid);
    } else {
        CEF_LaptopOpen(playerid);
    }
    return 1;
}

CMD:lp(playerid, params[]) {
    return cmd_laptop(playerid, params);
}

// ===========================================================================
//  ON PLAYER DISCONNECT
// ===========================================================================
stock CEF_LpOnPlayerDisconnect(playerid) {
    if(LaptopData[playerid][lp_IsOpen]) {
        CEF_LaptopClose(playerid);
    }
}

stock CEF_IsLaptopOpen(playerid) {
    return LaptopData[playerid][lp_IsOpen];
}
