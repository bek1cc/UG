// ===========================================================================
//  CEF LAPTOP SISTEM za open.mp (omp-cef plugin)
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

#define CEF_LP_RESOURCE      "cef/laptop"
#define CEF_LP_WIDTH         700.0
#define CEF_LP_HEIGHT        550.0
#define CEF_LP_BROWSER_ID    3

enum LaptopPlayerData {
    bool:lp_IsOpen,
    lp_BrowserId,
    lp_DWCooldown,
    bool:lp_BankLoggedIn
}

static LaptopData[MAX_PLAYERS][LaptopPlayerData];

#if defined USE_CEF

// Omp-CEF native funkcije
#if !defined _INC_cef
    #include <cef>
#endif

// ===========================================================================
//  OTVARANJE LAPTOPA (OMP-CEF)
// ===========================================================================
stock CEF_LaptopOpen(playerid) {
    if(LaptopData[playerid][lp_IsOpen]) return;
    if(!IsPlayerConnected(playerid)) return;

    if(!CEF_PlayerHasPlugin(playerid)) {
        SendClientMessage(playerid, 0xFF453AFF, "[LAPTOP]{FFFFFF} Nemate CEF plugin!");
        return;
    }

    LaptopData[playerid][lp_BrowserId] = CEF_LP_BROWSER_ID;
    CEF_CreateBrowser(playerid, LaptopData[playerid][lp_BrowserId], "http://web/laptop/index.html", true, true, CEF_LP_WIDTH, CEF_LP_HEIGHT);

    LaptopData[playerid][lp_IsOpen] = true;

    TogglePlayerControllable(playerid, false);
    SendClientMessage(playerid, 0x54A0FFFF, "[LAPTOP]{FFFFFF} CEF Laptop otvoren. Koristi /laptop za zatvaranje.");
}

// ===========================================================================
//  ZATVARANJE LAPTOPA
// ===========================================================================
stock CEF_LaptopClose(playerid) {
    if(!LaptopData[playerid][lp_IsOpen]) return;
    CEF_DestroyBrowser(playerid, LaptopData[playerid][lp_BrowserId]);
    LaptopData[playerid][lp_IsOpen] = false;
    LaptopData[playerid][lp_BankLoggedIn] = false;
    TogglePlayerControllable(playerid, true);
}

// ===========================================================================
//  INIT DATA (slanje nakon sto je browser spreman)
// ===========================================================================
stock CEF_LpSendInit(playerid) {
    if(!LaptopData[playerid][lp_IsOpen]) return;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, sizeof(playerName));

    CEF_EmitEvent(playerid, LaptopData[playerid][lp_BrowserId], "laptop:init",
        CEF_STR(playerName),
        CEF_INT(GetPlayerMoney(playerid)),
        CEF_INT(GetPlayerMoney(playerid)) // TODO: Pravi bank balance
    );
}

// ===========================================================================
//  SUBSCRIPTIONS (OMP-CEF)
// ===========================================================================
stock CEF_LpInitSubscriptions() {
    CEF_AddResource(CEF_LP_RESOURCE);

    CEF_RegisterEvent("laptop:close", "OnLaptopClose");
    CEF_RegisterEvent("laptop:darkweb:buy", "OnLaptopDarkWebBuy", Argument_String);
    CEF_RegisterEvent("laptop:bank:deposit", "OnLaptopBankDeposit", Argument_Integer);
    CEF_RegisterEvent("laptop:bank:withdraw", "OnLaptopBankWithdraw", Argument_Integer);
    CEF_RegisterEvent("laptop:bank:transfer", "OnLaptopBankTransfer", Argument_String, Argument_Integer);
    CEF_RegisterEvent("laptop:bounty:place", "OnLaptopBountyPlace", Argument_String);
    CEF_RegisterEvent("laptop:email:send", "OnLaptopEmailSend", Argument_String, Argument_String);
    CEF_RegisterEvent("laptop:terminal", "OnLaptopTerminal", Argument_String);
    print("[CEF LAPTOP] Subscriptions registrovane (omp-cef API).");
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

    if((gettime() - LaptopData[playerid][lp_DWCooldown]) < 300) {
        new remaining = 300 - (gettime() - LaptopData[playerid][lp_DWCooldown]);
        new msg[64];
        format(msg, sizeof(msg), "Pricekaj %d sekundi!", remaining);
        CEF_EmitEvent(playerid, LaptopData[playerid][lp_BrowserId], "laptop:notify", CEF_STR(msg));
        return;
    }

    LaptopData[playerid][lp_DWCooldown] = gettime();

    new msg[128];
    if(strcmp(cmd, "pistol", true) == 0) {
        if(GetPlayerMoney(playerid) < 5000) { CEF_EmitEvent(playerid, LaptopData[playerid][lp_BrowserId], "laptop:notify", CEF_STR("Nemate dovoljno novca!")); return; }
        GivePlayerMoney(playerid, -5000);
        GivePlayerWeapon(playerid, 22, 50);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste Pistol 9mm za $5,000. Isporuka na tajnoj lokaciji!");
    } else if(strcmp(cmd, "deagle", true) == 0) {
        if(GetPlayerMoney(playerid) < 15000) { CEF_EmitEvent(playerid, LaptopData[playerid][lp_BrowserId], "laptop:notify", CEF_STR("Nemate dovoljno novca!")); return; }
        GivePlayerMoney(playerid, -15000);
        GivePlayerWeapon(playerid, 24, 30);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste Deagle za $15,000.");
    } else if(strcmp(cmd, "ak47", true) == 0) {
        if(GetPlayerMoney(playerid) < 25000) { CEF_EmitEvent(playerid, LaptopData[playerid][lp_BrowserId], "laptop:notify", CEF_STR("Nemate dovoljno novca!")); return; }
        GivePlayerMoney(playerid, -25000);
        GivePlayerWeapon(playerid, 30, 120);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste AK-47 za $25,000.");
    } else if(strcmp(cmd, "m4", true) == 0) {
        if(GetPlayerMoney(playerid) < 30000) { CEF_EmitEvent(playerid, LaptopData[playerid][lp_BrowserId], "laptop:notify", CEF_STR("Nemate dovoljno novca!")); return; }
        GivePlayerMoney(playerid, -30000);
        GivePlayerWeapon(playerid, 31, 120);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste M4 za $30,000.");
    } else if(strcmp(cmd, "sniper", true) == 0) {
        if(GetPlayerMoney(playerid) < 50000) { CEF_EmitEvent(playerid, LaptopData[playerid][lp_BrowserId], "laptop:notify", CEF_STR("Nemate dovoljno novca!")); return; }
        GivePlayerMoney(playerid, -50000);
        GivePlayerWeapon(playerid, 34, 20);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste Sniper Rifle za $50,000.");
    } else if(strcmp(cmd, "weed", true) == 0) {
        if(GetPlayerMoney(playerid) < 3000) { CEF_EmitEvent(playerid, LaptopData[playerid][lp_BrowserId], "laptop:notify", CEF_STR("Nemate dovoljno novca!")); return; }
        GivePlayerMoney(playerid, -3000);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste Marihuanu x5 za $3,000.");
    } else if(strcmp(cmd, "coke", true) == 0) {
        if(GetPlayerMoney(playerid) < 8000) { CEF_EmitEvent(playerid, LaptopData[playerid][lp_BrowserId], "laptop:notify", CEF_STR("Nemate dovoljno novca!")); return; }
        GivePlayerMoney(playerid, -8000);
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Kupili ste Kokain x5 za $8,000.");
    } else {
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Artikal nije pronadjen.");
    }

    SendClientMessage(playerid, 0xFF453AFF, msg);
}

forward OnLaptopBankDeposit(playerid, amount);
public OnLaptopBankDeposit(playerid, amount) {
    if(!LaptopData[playerid][lp_IsOpen]) return;
    if(amount <= 0) { CEF_EmitEvent(playerid, LaptopData[playerid][lp_BrowserId], "laptop:notify", CEF_STR("Nevazeci iznos!")); return; }
    if(GetPlayerMoney(playerid) < amount) { CEF_EmitEvent(playerid, LaptopData[playerid][lp_BrowserId], "laptop:notify", CEF_STR("Nemate dovoljno novca!")); return; }
    GivePlayerMoney(playerid, -amount);
    new msg[64];
    format(msg, sizeof(msg), "[BANKA]{FFFFFF} Uplaceno $%d na racun.", amount);
    SendClientMessage(playerid, 0x30D158FF, msg);
}

forward OnLaptopBankWithdraw(playerid, amount);
public OnLaptopBankWithdraw(playerid, amount) {
    if(!LaptopData[playerid][lp_IsOpen]) return;
    if(amount <= 0) { CEF_EmitEvent(playerid, LaptopData[playerid][lp_BrowserId], "laptop:notify", CEF_STR("Nevazeci iznos!")); return; }
    GivePlayerMoney(playerid, amount);
    new msg[64];
    format(msg, sizeof(msg), "[BANKA]{FFFFFF} Isplaceno $%d sa racuna.", amount);
    SendClientMessage(playerid, 0x30D158FF, msg);
}

forward OnLaptopBankTransfer(playerid, targetName[], amount);
public OnLaptopBankTransfer(playerid, targetName[], amount) {
    if(!LaptopData[playerid][lp_IsOpen]) return;
    // TODO: Implementirati transfer
    SendClientMessage(playerid, 0x54A0FFFF, "[BANKA]{FFFFFF} Transfer u obradi...");
}

forward OnLaptopBountyPlace(playerid, data[]);
public OnLaptopBountyPlace(playerid, data[]) {
    if(!LaptopData[playerid][lp_IsOpen]) return;
    #if defined _cef_tablet_included
        OnTabletBountyPlace(playerid, data);
    #endif
}

forward OnLaptopEmailSend(playerid, to[], subj[]);
public OnLaptopEmailSend(playerid, to[], subj[]) {
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

// ===========================================================================
//  OnCefReady / OnCefBrowserCreated za laptop
// ===========================================================================
stock CefLp_OnCefReady(playerid) {
    if(LaptopData[playerid][lp_IsOpen]) {
        CEF_LpSendInit(playerid);
    }
}

stock CefLp_OnBrowserCreated(playerid, browserid) {
    if(browserid == CEF_LP_BROWSER_ID && LaptopData[playerid][lp_IsOpen]) {
        CEF_FocusBrowser(playerid, browserid, true);
    }
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
