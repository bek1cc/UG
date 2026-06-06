// ===========================================================================
//  CEF TABLET INTEGRACIJA za SA-MP
//  File: pawno/include/cef_tablet.inc
//
//  Povezuje PAWN server sa CEF browserom za moderni tablet UI.
//  Koristi Pawn.CEF plugin za komunikaciju.
//
//  Komande:
//  /tablet      - otvori tablet (CEF)
//  /bounties    - otvori tablet na bounty stranici
//
//  Integracija: #include <cef_tablet> u fg-ogc.pwn
//  Potrebno: Pawn.CEF plugin instaliran na serveru
// ===========================================================================

// ===========================================================================
//  KONFIGURACIJA
// ===========================================================================
#define CEF_TABLET_URL        "CEF/tablet/index.html"
#define CEF_TABLET_WIDTH      640
#define CEF_TABLET_HEIGHT     480

// ===========================================================================
//  PLAYER DATA
// ===========================================================================
enum CefTabletData {
    bool:ct_IsOpen,
    ct_BrowserId,
    bool:ct_LoggedIn,
    ct_LoginAttempts,
    ct_PinCode
}

static CefTablet[MAX_PLAYERS][CefTabletData];

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

        new entry[256];
        format(entry, sizeof(entry),
            "{\"id\":%d,\"target\":\"%s\",\"amount\":%d,\"placer\":\"%s\",\"time\":\"%s\",\"stacked\":%s}",
            i,
            BountyList[i][bTargetName],
            BountyList[i][bAmount],
            BountyList[i][bAnonymous] ? "Anoniman" : BountyList[i][bPlacerName],
            Bounty_FormatTimeAgo(BountyList[i][bTimestamp]),
            BountyList[i][bAmount > 1 ? "true" : "false"
        );
        strcat(json, entry);
    }

    strcat(json, "]}");
    return json;
}

// ===========================================================================
//  OTVARANJE TABLETA
// ===========================================================================
stock CEF_TabletOpen(playerid) {
    if(CefTablet[playerid][ct_IsOpen]) return;
    if(!IsPlayerConnected(playerid)) return;

    // Kreiraj CEF browser
    CefTablet[playerid][ct_BrowserId] = CreateBrowser(playerid, CEF_TABLET_URL, false, false);

    // Postavi poziciju (centrirano na ekranu)
    SetBrowserSize(playerid, CefTablet[playerid][ct_BrowserId], CEF_TABLET_WIDTH, CEF_TABLET_HEIGHT);

    // Emituj podatke igraca
    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, sizeof(playerName));

    new data[256];
    format(data, sizeof(data),
        "{\"username\":\"%s\",\"battery\":%d,\"money\":%d,\"level\":%d,\"online\":%d}",
        playerName,
        Bounty_CalcBattery(playerid),
        GetPlayerMoney(playerid),
        GetPlayerScore(playerid),
        GetOnlinePlayers()
    );

    // Cekaj da se browser ucita, onda posalji podatke
    defer CEF_SendInitData(playerid, data);

    CefTablet[playerid][ct_IsOpen] = true;
    CefTablet[playerid][ct_LoggedIn] = false;

    // Freeze player dok je tablet otvoren
    TogglePlayerControllable(playerid, false);
}

// ===========================================================================
//  ZATVARANJE TABLETA
// ===========================================================================
stock CEF_TabletClose(playerid) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    DestroyBrowser(playerid, CefTablet[playerid][ct_BrowserId]);
    CefTablet[playerid][ct_IsOpen] = false;
    CefTablet[playerid][ct_LoggedIn] = false;

    TogglePlayerControllable(playerid, true);
}

// ===========================================================================
//  SLANJE PODATAKA U CEF (deferred - ceka ucitavanje browsera)
// ===========================================================================
timer CEF_SendInitData[500](playerid, data[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;
    EmitBrowserEvent(playerid, CefTablet[playerid][ct_BrowserId], "tablet:open", data);
}

// ===========================================================================
//  UPDATE BOUNTY LISTE U CEF
// ===========================================================================
stock CEF_UpdateBountyList(playerid) {
    if(!CefTablet[playerid][ct_IsOpen]) return;
    if(!CefTablet[playerid][ct_LoggedIn]) return;

    new json[2048];
    json = CEF_FormatBountyJSON();
    EmitBrowserEvent(playerid, CefTablet[playerid][ct_BrowserId], "tablet:bounty:update", json);
}

// ===========================================================================
//  SLANJE TOAST NOTIFIKACIJE U CEF
// ===========================================================================
stock CEF_ShowToast(playerid, message[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return;

    new data[128];
    format(data, sizeof(data), "{\"message\":\"%s\"}", message);
    EmitBrowserEvent(playerid, CefTablet[playerid][ct_BrowserId], "tablet:toast", data);
}

// ===========================================================================
//  PRIMANJE EVENTA IZ CEF (PAWN callback)
// ===========================================================================
forward OnCefTabletEvent(playerid, event[], data[]);
public OnCefTabletEvent(playerid, event[], data[]) {
    if(!CefTablet[playerid][ct_IsOpen]) return 1;

    // LOGIN ATTEMPT
    if(strcmp(event, "tablet:login:attempt") == 0) {
        // Provjeri PIN
        if(!Bounty_HasPIN(playerid)) {
            CEF_ShowToast(playerid, "Niste registrovani! Koristi REGISTRUJ SE");
            return 1;
        }

        // Prikazi dialog za PIN
        ShowPlayerDialog(playerid, BOUNTY_DLG_LOGIN, DIALOG_STYLE_INPUT,
            "{2E86DE}UG Tablet{FFFFFF} - Prijava",
            "{FFFFFF}Unesite vas PIN kod:",
            "Prijavi", "Odustani");
        return 1;
    }

    // LOGIN REQUEST (click na password field)
    if(strcmp(event, "tablet:login:request") == 0) {
        if(!Bounty_HasPIN(playerid)) {
            CEF_ShowToast(playerid, "Niste registrovani! Koristi REGISTRUJ SE");
            return 1;
        }
        ShowPlayerDialog(playerid, BOUNTY_DLG_LOGIN, DIALOG_STYLE_INPUT,
            "{2E86DE}UG Tablet{FFFFFF} - Prijava",
            "{FFFFFF}Unesite vas PIN kod:",
            "Prijavi", "Odustani");
        return 1;
    }

    // REGISTER REQUEST
    if(strcmp(event, "tablet:register:request") == 0) {
        if(Bounty_HasPIN(playerid)) {
            CEF_ShowToast(playerid, "Vec ste registrovani! Prijavite se.");
            return 1;
        }
        ShowPlayerDialog(playerid, BOUNTY_DLG_REGISTER, DIALOG_STYLE_INPUT,
            "{2E86DE}UG Tablet{FFFFFF} - Registracija",
            "{FFFFFF}Izaberite PIN kod (4 cifre):",
            "Registruj", "Odustani");
        return 1;
    }

    // LOGOUT
    if(strcmp(event, "tablet:logout") == 0) {
        CefTablet[playerid][ct_LoggedIn] = false;
        return 1;
    }

    // PLACE BOUNTY
    if(strcmp(event, "tablet:bounty:place") == 0) {
        // Parse JSON: {"targetId":5,"amount":25000}
        new targetIdStr[8], amountStr[16];

        // Simple JSON parsing (PAWN nema JSON library)
        // Format: {"targetId":5,"amount":25000}
        new startIdx = strfind(data, "\"targetId\":");
        if(startIdx != -1) {
            startIdx += strlen("\"targetId\":");
            new endIdx = startIdx;
            while(data[endIdx] != ',' && data[endIdx] != '\0') endIdx++;
            strmid(targetIdStr, data, startIdx, endIdx);
        }

        new amtStart = strfind(data, "\"amount\":");
        if(amtStart != -1) {
            amtStart += strlen("\"amount\":");
            new amtEnd = amtStart;
            while(data[amtEnd] != '}' && data[amtEnd] != '\0') amtEnd++;
            strmid(amountStr, data, amtStart, amtEnd);
        }

        new targetId = strval(targetIdStr);
        new amount = strval(amountStr);

        // Validacija
        if(targetId < 0 || targetId >= MAX_PLAYERS || !IsPlayerConnected(targetId)) {
            CEF_ShowToast(playerid, "Igrac nije online!");
            return 1;
        }

        if(amount < BOUNTY_MIN_AMOUNT) {
            CEF_ShowToast(playerid, "Minimum nagrade je $1,000!");
            return 1;
        }

        if(amount > BOUNTY_MAX_AMOUNT) {
            CEF_ShowToast(playerid, "Maksimum nagrade je $500,000!");
            return 1;
        }

        if(GetPlayerMoney(playerid) < amount) {
            CEF_ShowToast(playerid, "Nemate dovoljno novca!");
            return 1;
        }

        // Cooldown check
        if((gettime() - BountyCooldown[playerid]) < BOUNTY_COOLDOWN) {
            new remaining = BOUNTY_COOLDOWN - (gettime() - BountyCooldown[playerid]);
            new msg[64];
            format(msg, sizeof(msg), "Pricekaj %d sekundi!", remaining);
            CEF_ShowToast(playerid, msg);
            return 1;
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

        return 1;
    }

    return 0;
}

// ===========================================================================
//  HELPER: GetOnlinePlayers
// ===========================================================================
stock GetOnlinePlayers() {
    new count = 0;
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(IsPlayerConnected(i)) count++;
    }
    return count;
}

// ===========================================================================
//  ZCMD KOMANDE
// ===========================================================================
CMD:tablet(playerid, params[]) {
    if(CefTablet[playerid][ct_IsOpen]) {
        CEF_TabletClose(playerid);
        SendClientMessage(playerid, 0x54A0FFFF, "[TABLET]{FFFFFF} Tablet zatvoren.");
    } else {
        CEF_TabletOpen(playerid);
        SendClientMessage(playerid, 0x54A0FFFF, "[TABLET]{FFFFFF} Tablet otvoren. Koristi /tablet za zatvaranje.");
    }
    return 1;
}

CMD:bounties(playerid, params[]) {
    if(!CefTablet[playerid][ct_IsOpen]) {
        CEF_TabletOpen(playerid);
    }
    // TODO: Switch directly to bounty screen after login
    return 1;
}

// ===========================================================================
//  ON PLAYER DISCONNECT - cleanup
// ===========================================================================
stock CefTablet_OnPlayerDisconnect(playerid) {
    if(CefTablet[playerid][ct_IsOpen]) {
        CEF_TabletClose(playerid);
    }
    CefTablet[playerid][ct_LoginAttempts] = 0;
    CefTablet[playerid][ct_PinCode] = 0;
    CefTablet[playerid][ct_LoggedIn] = false;
}
