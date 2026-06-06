// ===========================================================================
//  CEF TABLET INTEGRACIJA za SA-MP (samp-cef plugin)
//  File: pawno/include/cef_tablet.inc
//
//  Koristi pravi samp-cef API:
//  PAWN: cef_create_browser, cef_emit_event, cef_subscribe
//  JS:   window.cef.emit(), window.cef.subscribe()
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
//  CEF PLUGIN INSTALACIJA:
//  1. Preuzmi sa https://github.com/samp-cef/CEF/releases
//  2. Stavi CEF.dll (Win) ili CEF.so (Linux) u /plugins/
//  3. Dodaj "CEF" u plugins liniju u server.cfg
//  4. Definisi USE_CEF PRIJE #include <cef_tablet>
//  5. Stavi CEF/ folder sa tablet fajlovima u server root
//
//  BEZ CEF PLUGINA:
//  - Kod se kompajlira bez gresaka (nativne funkcije se ne pozivaju)
//  - /tablet i /phone koriste textdraw telefon kao fallback
// ===========================================================================

#define _cef_tablet_included

// ===========================================================================
//  KONFIGURACIJA
// ===========================================================================
#define CEF_TABLET_URL        "CEF/tablet/index.html"
#define CEF_TABLET_WIDTH      800
#define CEF_TABLET_HEIGHT     600
#define CEF_BROWSER_ID        1

// ===========================================================================
//  PLAYER DATA
// ===========================================================================
enum CefTabletData {
    bool:ct_IsOpen,
    ct_BrowserId,
    bool:ct_LoggedIn,
    ct_LoginAttempts,
    ct_PinCode,
    ct_InitTimer
}

static CefTablet[MAX_PLAYERS][CefTabletData];
static ct_InitData[MAX_PLAYERS][512];

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
//  CEF PLUGIN VERZIJA - koristi pravi samp-cef API
// ===========================================================================
#if defined USE_CEF

// Samp-CEF native funkcije
native cef_create_browser(playerid, browserid, const url[], bool:hidden = false, bool:fullscreen = false);
native cef_destroy_browser(playerid, browserid);
native cef_hide_browser(playerid, browserid, bool:hide);
native cef_set_browser_size(playerid, browserid, width, height);
native cef_set_browser_pos(playerid, browserid, x, y);
native cef_emit_event(const name[], const data[]);
native cef_subscribe(const name[], const callback[]);
native cef_player_has_plugin(playerid);

// ===========================================================================
//  OTVARANJE TABLETA (CEF)
// ===========================================================================
stock CEF_TabletOpen(playerid) {
    if(CefTablet[playerid][ct_IsOpen]) return;
    if(!IsPlayerConnected(playerid)) return;

    // Provjeri da li igrac ima CEF plugin na klijentu
    if(!cef_player_has_plugin(playerid)) {
        SendClientMessage(playerid, 0xFF453AFF, "[TABLET]{FFFFFF} Nemate CEF plugin! Koristite /phone za textdraw telefon.");
        return;
    }

    // Kreiraj CEF browser (hidden=true, prikazemo nakon postavljanja pozicije)
    CefTablet[playerid][ct_BrowserId] = CEF_BROWSER_ID;
    cef_create_browser(playerid, CefTablet[playerid][ct_BrowserId], CEF_TABLET_URL, true, false);

    // Postavi poziciju i velicinu (centrirano na ekranu)
    cef_set_browser_pos(playerid, CefTablet[playerid][ct_BrowserId], 160, 40);
    cef_set_browser_size(playerid, CefTablet[playerid][ct_BrowserId], CEF_TABLET_WIDTH, CEF_TABLET_HEIGHT);

    // Prikaži browser
    cef_hide_browser(playerid, CefTablet[playerid][ct_BrowserId], false);

    // Pripremi podatke igraca za slanje
    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, sizeof(playerName));

    format(ct_InitData[playerid], sizeof(ct_InitData[]),
        "{\"username\":\"%s\",\"battery\":%d,\"money\":%d,\"level\":%d,\"online\":%d,\"phone\":%d}",
        playerName,
        Bounty_CalcBattery(playerid),
        GetPlayerMoney(playerid),
        GetPlayerScore(playerid),
        CEF_GetOnlinePlayers(),
        iPhone_GetNumber(playerid)
    );

    // Cekaj 1 sekund da se browser ucita, onda posalji podatke
    CefTablet[playerid][ct_InitTimer] = SetTimerEx("CEF_SendInitData", 1000, false, "i", playerid);

    CefTablet[playerid][ct_IsOpen] = true;
    CefTablet[playerid][ct_LoggedIn] = false;

    // Freeze player dok je tablet otvoren
    TogglePlayerControllable(playerid, false);

    SendClientMessage(playerid, 0x54A0FFFF, "[TABLET]{FFFFFF} CEF Tablet otvoren. Koristi /tablet za zatvaranje.");
}

// ===========================================================================
//  ZATVARANJE TABLETA (CEF)
// ===========================================================================
stock CEF_TabletClose(playerid) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    // Kill timer ako jos radi
    if(CefTablet[playerid][ct_InitTimer] != 0) {
        KillTimer(CefTablet[playerid][ct_InitTimer]);
        CefTablet[playerid][ct_InitTimer] = 0;
    }

    cef_destroy_browser(playerid, CefTablet[playerid][ct_BrowserId]);
    CefTablet[playerid][ct_IsOpen] = false;
    CefTablet[playerid][ct_LoggedIn] = false;

    TogglePlayerControllable(playerid, true);
}

// ===========================================================================
//  SLANJE INICIJALNIH PODATAKA U CEF (timer callback)
// ===========================================================================
forward CEF_SendInitData(playerid);
public CEF_SendInitData(playerid) {
    if(!CefTablet[playerid][ct_IsOpen]) return;
    CefTablet[playerid][ct_InitTimer] = 0;

    // Posalji inicijalne podatke igraca
    cef_emit_event("tablet:init", ct_InitData[playerid]);
}

// ===========================================================================
//  UPDATE BOUNTY LISTE U CEF
// ===========================================================================
stock CEF_UpdateBountyList(playerid) {
    if(!CefTablet[playerid][ct_IsOpen]) return;
    if(!CefTablet[playerid][ct_LoggedIn]) return;

    new json[2048];
    json = CEF_FormatBountyJSON();
    cef_emit_event("tablet:bounty:update", json);
}

// ===========================================================================
//  SLANJE TOAST NOTIFIKACIJE U CEF
// ===========================================================================
stock CEF_ShowToast(playerid, message[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    new data[256];
    format(data, sizeof(data), "{\"message\":\"%s\"}", message);
    cef_emit_event("tablet:toast", data);
}

// ===========================================================================
//  SLANJE STATUSA LOGINA U CEF
// ===========================================================================
stock CEF_SendLoginStatus(playerid, bool:success, message[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    new data[256];
    format(data, sizeof(data), "{\"success\":%s,\"message\":\"%s\"}", success ? "true" : "false", message);
    cef_emit_event("tablet:login:result", data);
}

// ===========================================================================
//  CEF INIT - subscribe na evente iz browsera (samp-cef API)
// ===========================================================================
stock CEF_InitSubscriptions() {
    // Subscribe na evente koje JS salje preko window.cef.emit()
    cef_subscribe("tablet:login:attempt", "OnTabletLoginAttempt");
    cef_subscribe("tablet:register:attempt", "OnTabletRegisterAttempt");
    cef_subscribe("tablet:logout", "OnTabletLogout");
    cef_subscribe("tablet:bounty:place", "OnTabletBountyPlace");
    cef_subscribe("tablet:close", "OnTabletClose");
    cef_subscribe("tablet:phone:call", "OnTabletPhoneCall");
    cef_subscribe("tablet:phone:sms", "OnTabletPhoneSMS");
    print("[CEF TABLET] Subscriptions registrovane (samp-cef API).");
}

// ===========================================================================
//  CEF EVENT CALLBACKS - poziva ih samp-cef kad JS emituje event
// ===========================================================================

forward OnTabletLoginAttempt(playerid, pin[]);
public OnTabletLoginAttempt(playerid, pin[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    new pinNum = strval(pin);

    // Provjeri da li je igrac registrovan
    if(!Bounty_HasPIN(playerid)) {
        CEF_SendLoginStatus(playerid, false, "Niste registrovani! Koristi REGISTRUJ SE.");
        return;
    }

    // Provjeri PIN
    new savedPin = Bounty_LoadPIN(playerid);
    if(savedPin == pinNum) {
        CefTablet[playerid][ct_LoggedIn] = true;
        CefTablet[playerid][ct_LoginAttempts] = 0;
        CEF_SendLoginStatus(playerid, true, "Uspjesna prijava!");

        // Posalji bounty listu nakon login-a
        new json[2048];
        json = CEF_FormatBountyJSON();
        cef_emit_event("tablet:bounty:update", json);
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

    // Posalji bounty listu
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

    // Simple JSON parsing for {"targetName":"...","amount":...}
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

    // Nadji target playerid po imenu
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

    // Validacija
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

    // Cooldown check
    if((gettime() - BountyCooldown[playerid]) < BOUNTY_COOLDOWN) {
        new remaining = BOUNTY_COOLDOWN - (gettime() - BountyCooldown[playerid]);
        new msg[64];
        format(msg, sizeof(msg), "Pricekaj %d sekundi!", remaining);
        CEF_ShowToast(playerid, msg);
        return;
    }

    // Place bounty
    GivePlayerMoney(playerid, -amount);
    Bounty_Add(targetId, playerid, amount, false);
    BountyCooldown[playerid] = gettime();

    CEF_ShowToast(playerid, "Nagrada uspjesno postavljena!");
    CEF_UpdateBountyList(playerid);

    // Update svim drugim igracima koji imaju tablet otvoren
    for(new p = 0; p < MAX_PLAYERS; p++) {
        if(p != playerid && CefTablet[p][ct_IsOpen] && CefTablet[p][ct_LoggedIn]) {
            CEF_UpdateBountyList(p);
        }
    }
}

forward OnTabletPhoneCall(playerid, number[]);
public OnTabletPhoneCall(playerid, number[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;
    new targetNum = strval(number);
    // TODO: Integriraj sa iPhone call sistemom
    new msg[64];
    format(msg, sizeof(msg), "Pozivam %s...", number);
    CEF_ShowToast(playerid, msg);
}

forward OnTabletPhoneSMS(playerid, data[]);
public OnTabletPhoneSMS(playerid, data[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;
    // TODO: Integriraj sa iPhone SMS sistemom
    CEF_ShowToast(playerid, "SMS poslan!");
}

// ===========================================================================
//  CEF OnGameModeInit - inicijalizacija
// ===========================================================================
stock CEF_OnGameModeInit() {
    CEF_InitSubscriptions();
    print("[CEF TABLET] CEF tablet sistem inicijaliziran (samp-cef API).");
}

#else

// ===========================================================================
//  FALLBACK VERZIJA - bez CEF plugina
//  /tablet koristi textdraw telefon (iphone.inc)
// ===========================================================================

stock CEF_TabletOpen(playerid) {
    // Fallback: koristi textdraw telefon
    iPhone_Open(playerid);
    SendClientMessage(playerid, 0x54A0FFFF, "[TABLET]{FFFFFF} CEF plugin nije instaliran. Koristite textdraw telefon (/phone).");
}

stock CEF_TabletClose(playerid) {
    // Fallback: zatvori textdraw telefon
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
    print("[CEF TABLET] Za CEF podrsku: instaliraj samp-cef plugin i definisi USE_CEF.");
}

forward CEF_SendInitData(playerid);
public CEF_SendInitData(playerid) {
    #pragma unused playerid
}

#endif // USE_CEF

// ===========================================================================
//  ZCMD KOMANDE (zajednicke za obje verzije)
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
//  ON PLAYER DISCONNECT - cleanup (zajednicko)
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
