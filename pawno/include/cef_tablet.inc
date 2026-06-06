// ===========================================================================
//  CEF TABLET INTEGRACIJA za open.mp (omp-cef plugin)
//  File: pawno/include/cef_tablet.inc
//
//  Koristi omp-cef API:
//  PAWN: CEF_CreateBrowser, CEF_EmitEvent, CEF_RegisterEvent, CEF_AddResource
//  JS:   cef.emit(), cef.on()
//
//  Komande:
//  /tablet      - otvori/zatvori tablet (CEF ili textdraw fallback)
//  /bounties    - otvori tablet na bounty stranici
//  /phone       - otvori tablet/telefon (koristi CEF ako je dostupan)
//
//  Integracija:
//    #define USE_CEF
//    #include <cef_tablet>
//
//  OMP-CEF PLUGIN INSTALACIJA:
//  1. Preuzmi sa https://github.com/aurora-mp/omp-cef/releases
//  2. Stavi Cef.so u components/ folder (Linux) ili Cef.dll u components/ (Windows)
//  3. Stavi cef.inc u pawno/include/ ili qawno/include/
//  4. Definisi USE_CEF PRIJE #include <cef_tablet>
//  5. CEF fajlove stavi u scriptfiles/cef/tablet/
//  6. Otvori UDP port server_port + 2 (default: 7779)
//
//  BEZ CEF PLUGINA:
//  - Kod se kompajlira bez gresaka (nativne funkcije se ne pozivaju)
//  - /tablet i /phone koriste textdraw telefon kao fallback
// ===========================================================================

#define _cef_tablet_included

// ===========================================================================
//  KONFIGURACIJA
// ===========================================================================
#define CEF_TABLET_RESOURCE  "cef/tablet"
#define CEF_TABLET_WIDTH     800.0
#define CEF_TABLET_HEIGHT    600.0
#define CEF_BROWSER_ID       1

// ===========================================================================
//  PLAYER DATA
// ===========================================================================
enum CefTabletData {
    bool:ct_IsOpen,
    ct_BrowserId,
    bool:ct_LoggedIn,
    ct_LoginAttempts,
    ct_PinCode,
    bool:ct_CefReady
}

static CefTablet[MAX_PLAYERS][CefTabletData];

// ===========================================================================
//  HELPER: GetOnlinePlayers
// ===========================================================================
stock CEF_GetOnlinePlayers() {
    new count = 0;
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(IsPlayerConnected(i)) count++;
    }
    return count;
}

// ===========================================================================
//  OMP-CEF VERZIJA
// ===========================================================================
#if defined USE_CEF

// Omp-CEF native funkcije (iz cef.inc)
// Ako cef.inc nije vec ukljucen, ukljuci ga ovdje
#if !defined _INC_cef
    #include <cef>
#endif

// ===========================================================================
//  HELPER: Format bounty data za CEF
// ===========================================================================
stock CEF_FormatBountyJSON() {
    new json[2048];
    format(json, sizeof(json), "{\"bounties\":[");

    new first = 1;
    for(new i = 0; i < BOUNTY_MAX; i++) {
        if(!BountyList[i][bExists]) continue;

        if(!first) strcat(json, ",");
        first = 0;

        new placerStr[MAX_PLAYER_NAME + 12];
        if(BountyList[i][bAnonymous]) {
            format(placerStr, sizeof(placerStr), "Anoniman");
        } else {
            format(placerStr, sizeof(placerStr), "%s", BountyList[i][bPlacerName]);
        }
        new stackedStr[8];
        if(BountyList[i][bAmount] > 1) format(stackedStr, sizeof(stackedStr), "true");
        else format(stackedStr, sizeof(stackedStr), "false");

        new entry[256];
        format(entry, sizeof(entry),
            "{\"id\":%d,\"target\":\"%s\",\"amount\":%d,\"placer\":\"%s\",\"time\":\"%s\",\"stacked\":%s}",
            i,
            BountyList[i][bTargetName],
            BountyList[i][bAmount],
            placerStr,
            Bounty_FormatTimeAgo(BountyList[i][bTimestamp]),
            stackedStr
        );
        strcat(json, entry);
    }

    strcat(json, "]}");
    return json;
}

// ===========================================================================
//  OTVARANJE TABLETA (OMP-CEF)
// ===========================================================================
stock CEF_TabletOpen(playerid) {
    if(CefTablet[playerid][ct_IsOpen]) return;
    if(!IsPlayerConnected(playerid)) return;

    // Provjeri da li igrac ima CEF plugin na klijentu
    if(!CEF_PlayerHasPlugin(playerid)) {
        SendClientMessage(playerid, 0xFF453AFF, "[TABLET]{FFFFFF} Nemate CEF plugin! Koristite /phone za textdraw telefon.");
        return;
    }

    // Kreiraj CEF browser
    // CEF_CreateBrowser(playerid, browserid, url, focused, controls_chat, width, height)
    CefTablet[playerid][ct_BrowserId] = CEF_BROWSER_ID;
    CEF_CreateBrowser(playerid, CefTablet[playerid][ct_BrowserId], "http://web/tablet/index.html", true, true, CEF_TABLET_WIDTH, CEF_TABLET_HEIGHT);

    CefTablet[playerid][ct_IsOpen] = true;
    CefTablet[playerid][ct_LoggedIn] = false;
    CefTablet[playerid][ct_CefReady] = false;

    // Freeze player dok je tablet otvoren
    TogglePlayerControllable(playerid, false);

    SendClientMessage(playerid, 0x54A0FFFF, "[TABLET]{FFFFFF} CEF Tablet otvoren. Koristi /tablet za zatvaranje.");
}

// ===========================================================================
//  ZATVARANJE TABLETA (OMP-CEF)
// ===========================================================================
stock CEF_TabletClose(playerid) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    CEF_DestroyBrowser(playerid, CefTablet[playerid][ct_BrowserId]);
    CefTablet[playerid][ct_IsOpen] = false;
    CefTablet[playerid][ct_LoggedIn] = false;
    CefTablet[playerid][ct_CefReady] = false;

    TogglePlayerControllable(playerid, true);
}

// ===========================================================================
//  SLANJE INICIJALNIH PODATAKA U CEF (poziva se kad je browser spreman)
// ===========================================================================
stock CEF_TabletSendInit(playerid) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, sizeof(playerName));

    // Koristimo CEF_EmitEvent sa typed argumentima
    CEF_EmitEvent(playerid, CefTablet[playerid][ct_BrowserId], "tablet:init",
        CEF_STR(playerName),
        CEF_INT(Bounty_CalcBattery(playerid)),
        CEF_INT(GetPlayerMoney(playerid)),
        CEF_INT(GetPlayerScore(playerid)),
        CEF_INT(CEF_GetOnlinePlayers()),
        CEF_INT(iPhone_GetNumber(playerid))
    );
}

// ===========================================================================
//  UPDATE BOUNTY LISTE U CEF
// ===========================================================================
stock CEF_UpdateBountyList(playerid) {
    if(!CefTablet[playerid][ct_IsOpen]) return;
    if(!CefTablet[playerid][ct_LoggedIn]) return;

    new json[2048];
    json = CEF_FormatBountyJSON();
    CEF_EmitEvent(playerid, CefTablet[playerid][ct_BrowserId], "tablet:bounty:update",
        CEF_STR(json)
    );
}

// ===========================================================================
//  SLANJE TOAST NOTIFIKACIJE U CEF
// ===========================================================================
stock CEF_ShowToast(playerid, message[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    CEF_EmitEvent(playerid, CefTablet[playerid][ct_BrowserId], "tablet:toast",
        CEF_STR(message)
    );
}

// ===========================================================================
//  SLANJE STATUSA LOGINA U CEF
// ===========================================================================
stock CEF_SendLoginStatus(playerid, bool:success, message[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    CEF_EmitEvent(playerid, CefTablet[playerid][ct_BrowserId], "tablet:login:result",
        CEF_BOOL(success),
        CEF_STR(message)
    );
}

// ===========================================================================
//  CEF INIT - Registruj evente i resurse
// ===========================================================================
stock CEF_InitSubscriptions() {
    // Dodaj resurs za download od strane klijenata
    CEF_AddResource(CEF_TABLET_RESOURCE);

    // Registruj evente koje JS salje preko cef.emit()
    // Format: CEF_RegisterEvent(eventName, callback, argTypes...)
    CEF_RegisterEvent("tablet:login:attempt", "OnTabletLoginAttempt", Argument_String);
    CEF_RegisterEvent("tablet:register:attempt", "OnTabletRegisterAttempt", Argument_String);
    CEF_RegisterEvent("tablet:logout", "OnTabletLogout");
    CEF_RegisterEvent("tablet:bounty:place", "OnTabletBountyPlace", Argument_String);
    CEF_RegisterEvent("tablet:close", "OnTabletClose");
    CEF_RegisterEvent("tablet:phone:call", "OnTabletPhoneCall", Argument_String);
    CEF_RegisterEvent("tablet:phone:sms", "OnTabletPhoneSMS", Argument_String);
    CEF_RegisterEvent("tablet:requestStats", "OnTabletRequestStats");
    CEF_RegisterEvent("tablet:requestBounties", "OnTabletRequestBounties");
    CEF_RegisterEvent("tablet:call911", "OnTabletCall911");
    CEF_RegisterEvent("tablet:callTaxi", "OnTabletCallTaxi");
    CEF_RegisterEvent("tablet:callMehanicar", "OnTabletCallMehanicar");

    print("[CEF TABLET] Subscriptions registrovane (omp-cef API).");
}

// ===========================================================================
//  CEF EVENT CALLBACKS
// ===========================================================================

forward OnTabletLoginAttempt(playerid, pin[]);
public OnTabletLoginAttempt(playerid, pin[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    new pinNum = strval(pin);

    if(!Bounty_HasPIN(playerid)) {
        CEF_SendLoginStatus(playerid, false, "Niste registrovani! Koristi REGISTRUJ SE.");
        return;
    }

    new savedPin = Bounty_LoadPIN(playerid);
    if(savedPin == pinNum) {
        CefTablet[playerid][ct_LoggedIn] = true;
        CefTablet[playerid][ct_LoginAttempts] = 0;
        CEF_SendLoginStatus(playerid, true, "Uspjesna prijava!");

        // Posalji bounty listu nakon login-a
        new json[2048];
        json = CEF_FormatBountyJSON();
        CEF_EmitEvent(playerid, CefTablet[playerid][ct_BrowserId], "tablet:bounty:update", CEF_STR(json));
    } else {
        CefTablet[playerid][ct_LoginAttempts]++;
        if(CefTablet[playerid][ct_LoginAttempts] >= 3) {
            CEF_SendLoginStatus(playerid, false, "Previse pokusaja! Tablet se zatvara.");
            CEF_TabletClose(playerid);
        } else {
            new msg[64];
            format(msg, sizeof(msg), "Pogresan PIN! Pokusaj: %d/3", CefTablet[playerid][ct_LoginAttempts]);
            CEF_SendLoginStatus(playerid, false, msg);
        }
    }
}

forward OnTabletRegisterAttempt(playerid, pin[]);
public OnTabletRegisterAttempt(playerid, pin[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    if(Bounty_HasPIN(playerid)) {
        CEF_SendLoginStatus(playerid, false, "Vec ste registrovani! Prijavite se.");
        return;
    }

    new pinNum = strval(pin);
    if(pinNum < 1000 || pinNum > 9999) {
        CEF_SendLoginStatus(playerid, false, "PIN mora biti 4 cifre!");
        return;
    }

    Bounty_SavePIN(playerid, pinNum);
    CefTablet[playerid][ct_LoggedIn] = true;
    CEF_SendLoginStatus(playerid, true, "Uspjesna registracija!");

    CEF_UpdateBountyList(playerid);
}

forward OnTabletLogout(playerid);
public OnTabletLogout(playerid) {
    if(!CefTablet[playerid][ct_IsOpen]) return;
    CefTablet[playerid][ct_LoggedIn] = false;
    CEF_ShowToast(playerid, "Uspjesno ste se odjavili.");
}

forward OnTabletClose(playerid);
public OnTabletClose(playerid) {
    CEF_TabletClose(playerid);
    SendClientMessage(playerid, 0x54A0FFFF, "[TABLET]{FFFFFF} Tablet zatvoren.");
}

forward OnTabletBountyPlace(playerid, data[]);
public OnTabletBountyPlace(playerid, data[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    new targetName[MAX_PLAYER_NAME];
    new amountStr[16];

    new startIdx = strfind(data, "\"targetName\":\"");
    if(startIdx != -1) {
        startIdx += strlen("\"targetName\":\"");
        new endIdx = startIdx;
        while(data[endIdx] != '"' && data[endIdx] != '\0') endIdx++;
        strmid(targetName, data, startIdx, endIdx);
    }

    new amtStart = strfind(data, "\"amount\":");
    if(amtStart != -1) {
        amtStart += strlen("\"amount\":");
        new amtEnd = amtStart;
        while(data[amtEnd] != '}' && data[amtEnd] != ',' && data[amtEnd] != '\0') amtEnd++;
        strmid(amountStr, data, amtStart, amtEnd);
    }

    new amount = strval(amountStr);

    new targetId = INVALID_PLAYER_ID;
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(IsPlayerConnected(i)) {
            new pName[MAX_PLAYER_NAME];
            GetPlayerName(i, pName, sizeof(pName));
            if(strcmp(pName, targetName, true) == 0) {
                targetId = i;
                break;
            }
        }
    }

    if(targetId == INVALID_PLAYER_ID) {
        CEF_ShowToast(playerid, "Igrac nije online!");
        return;
    }

    if(amount < BOUNTY_MIN_AMOUNT) {
        CEF_ShowToast(playerid, "Minimum nagrade je $1,000!");
        return;
    }

    if(amount > BOUNTY_MAX_AMOUNT) {
        CEF_ShowToast(playerid, "Maksimum nagrade je $500,000!");
        return;
    }

    if(GetPlayerMoney(playerid) < amount) {
        CEF_ShowToast(playerid, "Nemate dovoljno novca!");
        return;
    }

    if((gettime() - BountyCooldown[playerid]) < BOUNTY_COOLDOWN) {
        new remaining = BOUNTY_COOLDOWN - (gettime() - BountyCooldown[playerid]);
        new msg[64];
        format(msg, sizeof(msg), "Pricekaj %d sekundi!", remaining);
        CEF_ShowToast(playerid, msg);
        return;
    }

    GivePlayerMoney(playerid, -amount);
    Bounty_Add(targetId, playerid, amount, false);
    BountyCooldown[playerid] = gettime();

    CEF_ShowToast(playerid, "Nagrada uspjesno postavljena!");
    CEF_UpdateBountyList(playerid);

    for(new p = 0; p < MAX_PLAYERS; p++) {
        if(p != playerid && CefTablet[p][ct_IsOpen] && CefTablet[p][ct_LoggedIn]) {
            CEF_UpdateBountyList(p);
        }
    }
}

forward OnTabletPhoneCall(playerid, number[]);
public OnTabletPhoneCall(playerid, number[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;
    new msg[64];
    format(msg, sizeof(msg), "Pozivam %s...", number);
    CEF_ShowToast(playerid, msg);
}

forward OnTabletPhoneSMS(playerid, data[]);
public OnTabletPhoneSMS(playerid, data[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;
    CEF_ShowToast(playerid, "SMS poslan!");
}

forward OnTabletRequestStats(playerid);
public OnTabletRequestStats(playerid) {
    if(!CefTablet[playerid][ct_IsOpen]) return;
    CEF_TabletSendInit(playerid);
}

forward OnTabletRequestBounties(playerid);
public OnTabletRequestBounties(playerid) {
    if(!CefTablet[playerid][ct_IsOpen]) return;
    CEF_UpdateBountyList(playerid);
}

forward OnTabletCall911(playerid);
public OnTabletCall911(playerid) {
    CEF_ShowToast(playerid, "Pozivam 911...");
}

forward OnTabletCallTaxi(playerid);
public OnTabletCallTaxi(playerid) {
    CEF_ShowToast(playerid, "Pozivam taxi...");
}

forward OnTabletCallMehanicar(playerid);
public OnTabletCallMehanicar(playerid) {
    CEF_ShowToast(playerid, "Pozivam mehanicara...");
}

// ===========================================================================
//  OnCefReady - poziva se kad je CEF klijent spreman
// ===========================================================================
stock CefTablet_OnCefReady(playerid) {
    // Ako igrac ima tablet otvoren, posalji init podatke
    if(CefTablet[playerid][ct_IsOpen]) {
        CefTablet[playerid][ct_CefReady] = true;
        CEF_TabletSendInit(playerid);
    }
}

// ===========================================================================
//  OnCefBrowserCreated - poziva se kad je browser kreiran
// ===========================================================================
stock CefTablet_OnBrowserCreated(playerid, browserid) {
    if(browserid == CEF_BROWSER_ID && CefTablet[playerid][ct_IsOpen]) {
        // Browser je kreiran, fokusiraj ga
        CEF_FocusBrowser(playerid, browserid, true);
    }
}

// ===========================================================================
//  CEF OnGameModeInit
// ===========================================================================
stock CEF_OnGameModeInit() {
    CEF_InitSubscriptions();
    print("[CEF TABLET] CEF tablet sistem inicijaliziran (omp-cef API).");
}

#else

// ===========================================================================
//  FALLBACK VERZIJA - bez CEF plugina
// ===========================================================================

stock CEF_TabletOpen(playerid) {
    iPhone_Open(playerid);
    SendClientMessage(playerid, 0x54A0FFFF, "[TABLET]{FFFFFF} CEF plugin nije instaliran. Koristite textdraw telefon (/phone).");
}

stock CEF_TabletClose(playerid) {
    iPhone_Close(playerid);
}

stock CEF_UpdateBountyList(playerid) {
    #pragma unused playerid
}

stock CEF_ShowToast(playerid, message[]) {
    new msg[128];
    format(msg, sizeof(msg), "[TABLET]{FFFFFF} %s", message);
    SendClientMessage(playerid, 0x54A0FFFF, msg);
}

stock CEF_SendLoginStatus(playerid, bool:success, message[]) {
    #pragma unused playerid
    #pragma unused success
    #pragma unused message
}

stock CEF_OnGameModeInit() {
    print("[CEF TABLET] CEF plugin NIJE detektovan. Koristi se textdraw fallback.");
    print("[CEF TABLET] Za CEF podrsku: instaliraj omp-cef plugin i definisi USE_CEF.");
}

#endif // USE_CEF

// ===========================================================================
//  ZCMD KOMANDE
// ===========================================================================
CMD:tablet(playerid, params[]) {
    #pragma unused params
    if(CefTablet[playerid][ct_IsOpen]) {
        CEF_TabletClose(playerid);
        SendClientMessage(playerid, 0x54A0FFFF, "[TABLET]{FFFFFF} Tablet zatvoren.");
    } else {
        CEF_TabletOpen(playerid);
    }
    return 1;
}

// ===========================================================================
//  ON PLAYER DISCONNECT
// ===========================================================================
stock CefTablet_OnPlayerDisconnect(playerid) {
    if(CefTablet[playerid][ct_IsOpen]) {
        CEF_TabletClose(playerid);
    }
    CefTablet[playerid][ct_LoginAttempts] = 0;
    CefTablet[playerid][ct_PinCode] = 0;
    CefTablet[playerid][ct_LoggedIn] = false;
}

// ===========================================================================
//  HELPER: Provjeri da li je CEF tablet otvoren
// ===========================================================================
stock CEF_IsTabletOpen(playerid) {
    return CefTablet[playerid][ct_IsOpen];
}
