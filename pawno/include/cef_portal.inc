// ===========================================================================
//  CEF PORTAL / LAUNCHER za open.mp (omp-cef plugin)
//  File: pawno/include/cef_portal.inc
//
//  In-game portal/launcher sa server info, novosti, komponente, IGRAJ dugme.
//  Igrac moze vidjeti status, promijeniti nick, i kliknuti IGRAJ da udje u igru.
//
//  Integracija:
//    #define USE_CEF
//    #include <cef_portal>
// ===========================================================================

#if defined _cef_portal_included
    #endinput
#endif
#define _cef_portal_included

#define CEF_PORTAL_RESOURCE     "portal"
#define CEF_PORTAL_BROWSER_ID   11
#define CEF_PORTAL_WIDTH        0.0   // fullscreen
#define CEF_PORTAL_HEIGHT       0.0   // fullscreen

enum PortalPlayerData {
    pt_IsOpen,
    pt_BrowserId,
    pt_WaitingSpawn
}

static PortalData[MAX_PLAYERS][PortalPlayerData];

#if defined USE_CEF

#if !defined _INC_cef
    #include <cef>
#endif

// ===========================================================================
//  GAME MODE INIT - Registracija resursa i eventa
// ===========================================================================
stock CefPortal_OnGameModeInit() {
    CEF_AddResource(CEF_PORTAL_RESOURCE);

    // Registriraj sve JS->Pawn evente
    CEF_RegisterEvent("portal:play",              "OnCefPortalPlay",              Argument_String);
    CEF_RegisterEvent("portal:refreshStatus",     "OnCefPortalRefreshStatus");
    CEF_RegisterEvent("portal:download",          "OnCefPortalDownload");
    CEF_RegisterEvent("portal:reinstall",         "OnCefPortalReinstall");
    CEF_RegisterEvent("portal:openDiscord",       "OnCefPortalOpenDiscord");
    CEF_RegisterEvent("portal:openWebsite",       "OnCefPortalOpenWebsite");
    CEF_RegisterEvent("portal:radio",             "OnCefPortalRadio",             Argument_Integer);

    // Login/Register eventi (portal sa formama umjesto SA-MP dialoga)
    CEF_RegisterEvent("portal:loginAttempt",      "OnCefPortalLoginAttempt",      Argument_String, Argument_String);
    CEF_RegisterEvent("portal:registerAttempt",   "OnCefPortalRegisterAttempt",   Argument_String, Argument_String, Argument_String);

    print("[CEF-PORTAL] Resource registriran, eventi postavljeni (sa login/register)");
}

// ===========================================================================
//  PRIKAZI PORTAL
// ===========================================================================
stock CefPortal_Show(playerid) {
    printf("[CEF-PORTAL] CefPortal_Show called for player %d (pt_IsOpen=%d, CEF_PlayerHasPlugin=%d)", playerid, PortalData[playerid][pt_IsOpen], CEF_PlayerHasPlugin(playerid));

    if(PortalData[playerid][pt_IsOpen]) {
        printf("[CEF-PORTAL] WARNING: Portal vec otvoren za player %d - preskacem", playerid);
        return;
    }
    if(!IsPlayerConnected(playerid)) {
        printf("[CEF-PORTAL] WARNING: Player %d nije konektovan - preskacem", playerid);
        return;
    }
    if(!CEF_PlayerHasPlugin(playerid)) {
        // Nema CEF - odmah spawnaj igraca
        printf("[CEF-PORTAL] Player %d nema CEF plugin - pokazujem spawn screen", playerid);
        CEF_ToggleSpawnScreen(playerid, true);
        CEF_ToggleHudComponent(playerid, HUD_COMPONENT_ALL, true);
        return;
    }

    // Sakrij spawn screen dok je portal otvoren
    CEF_ToggleSpawnScreen(playerid, false);

    // Kreiraj fullscreen browser za portal
    PortalData[playerid][pt_BrowserId] = CEF_PORTAL_BROWSER_ID;
    CEF_CreateBrowser(playerid, PortalData[playerid][pt_BrowserId], "http://cef/portal/index.html", true, false);

    PortalData[playerid][pt_IsOpen] = true;
    PortalData[playerid][pt_WaitingSpawn] = true;
    printf("[CEF-PORTAL] Prikazan portal za player %d (browser ID: %d)", playerid, PortalData[playerid][pt_BrowserId]);
}

// ===========================================================================
//  ZATVORI PORTAL
// ===========================================================================
stock CefPortal_Hide(playerid) {
    if(!PortalData[playerid][pt_IsOpen]) return;

    CEF_DestroyBrowser(playerid, PortalData[playerid][pt_BrowserId]);
    PortalData[playerid][pt_IsOpen] = false;
    printf("[CEF-PORTAL] Sklonjen portal za player %d", playerid);
}

// ===========================================================================
//  POSALJI INIT PODATKE (poziva se kad je browser kreiran)
// ===========================================================================
stock CefPortal_SendInit(playerid) {
    if(!PortalData[playerid][pt_IsOpen]) return;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, sizeof(playerName));

    new onlineCount = 0;
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(IsPlayerConnected(i)) onlineCount++;
    }

    // CEF i ASI status
    new cefOk = CEF_PlayerHasPlugin(playerid) ? 1 : 0;
    new asiOk = 1; // TODO: provjeri dinput8.dll na klijentu - za sada pretpostavljamo DA

    CEF_EmitEvent(playerid, PortalData[playerid][pt_BrowserId], "portal:init",
        CEF_STR(playerName),
        CEF_INT(onlineCount),
        CEF_INT(1000),  // max slots
        CEF_INT(cefOk),
        CEF_INT(asiOk)
    );
}

// ===========================================================================
//  ON BROWSER CREATED
// ===========================================================================
stock CefPortal_OnBrowserCreated(playerid, browserid) {
    if(browserid != CEF_PORTAL_BROWSER_ID) return;
    if(!PortalData[playerid][pt_IsOpen]) return;

    printf("[CEF-PORTAL] Browser created for player %d (browserid: %d) - sending portal:init", playerid, browserid);
    CEF_FocusBrowser(playerid, PortalData[playerid][pt_BrowserId], true);
    CefPortal_SendInit(playerid);
}

// ===========================================================================
//  CALLBACK: IGRAC KLIKNUO IGRAJ
// ===========================================================================
forward OnCefPortalPlay(playerid, nick[]);
public OnCefPortalPlay(playerid, nick[]) {
    printf("[CEF-PORTAL] Player %d kliknuo IGRAJ sa nick: '%s'", playerid, nick);

    // Ako je nick promijenjen, pokusaj ga postaviti
    new currentName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, currentName, sizeof(currentName));

    if(strcmp(currentName, nick, false) != 0) {
        // Pokusaj promijeniti nick
        new ret = SetPlayerName(playerid, nick);
        if(ret == -1) {
            // Nick vec postoji - nastavi sa trenutnim
            printf("[CEF-PORTAL] Nick '%s' vec zauzet, ostajem na '%s'", nick, currentName);
        } else if(ret == 0) {
            // Nick nije validan
            printf("[CEF-PORTAL] Nick '%s' nije validan, ostajem na '%s'", nick, currentName);
        } else {
            printf("[CEF-PORTAL] Nick promijenjen na '%s'", nick);
        }
    }

    // Zatvori portal
    CefPortal_Hide(playerid);

    // Pokazi HUD i spawn screen
    CEF_ToggleSpawnScreen(playerid, true);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_ALL, true);

    // Portal zavrsen - login/register ce se pokrenuti automatski
    PortalData[playerid][pt_WaitingSpawn] = false;

    // Obavijesti gamemode da je portal zavrsen (za PortalActive = false)
    CallLocalFunction("OnPortalFinished", "d", playerid);
}

// ===========================================================================
//  CALLBACK: OSVJEZI STATUS
// ===========================================================================
forward OnCefPortalRefreshStatus(playerid);
public OnCefPortalRefreshStatus(playerid) {
    if(!PortalData[playerid][pt_IsOpen]) return 1;

    new onlineCount = 0;
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(IsPlayerConnected(i)) onlineCount++;
    }

    CEF_EmitEvent(playerid, PortalData[playerid][pt_BrowserId], "portal:updateStatus",
        CEF_INT(1),         // online
        CEF_INT(onlineCount)
    );
    return 1;
}

// ===========================================================================
//  CALLBACK: PREUZMI FAJLOVE
// ===========================================================================
forward OnCefPortalDownload(playerid);
public OnCefPortalDownload(playerid) {
    printf("[CEF-PORTAL] Player %d trazi preuzimanje fajlova", playerid);
    // TODO: Implementirati download logiku
    // Za sada saljemo poruku
    CEF_EmitEvent(playerid, PortalData[playerid][pt_BrowserId], "portal:downloadProgress",
        CEF_INT(100),
        CEF_STR("Sve komponente su vec instalirane!")
    );
}

// ===========================================================================
//  CALLBACK: REINSTALIRAJ FAJLOVE
// ===========================================================================
forward OnCefPortalReinstall(playerid);
public OnCefPortalReinstall(playerid) {
    printf("[CEF-PORTAL] Player %d trazi reinstalaciju fajlova", playerid);
    SendClientMessage(playerid, 0xFF6B6BFF, "[PORTAL]{FFFFFF} Reinstalacija pokrenuta! Pokrenite launcher vani za kompletnu reinstalaciju.");
    // CEF fajlovi se automatski ponovo skidaju pri sljedecem connectu
    // Za kompletnu reinstalaciju, igrac treba koristiti launcher
    CEF_EmitEvent(playerid, PortalData[playerid][pt_BrowserId], "portal:downloadProgress",
        CEF_INT(100),
        CEF_STR("Pokrenite launcher za reinstalaciju!")
    );
}

// ===========================================================================
//  CALLBACK: OTVORI DISCORD
// ===========================================================================
forward OnCefPortalOpenDiscord(playerid);
public OnCefPortalOpenDiscord(playerid) {
    SendClientMessage(playerid, 0x5865F2FF, "[DISCORD]{FFFFFF} Pridruzi se nasem Discordu: https://discord.gg/unicategaming");
}

// ===========================================================================
//  CALLBACK: OTVORI WEBSITE
// ===========================================================================
forward OnCefPortalOpenWebsite(playerid);
public OnCefPortalOpenWebsite(playerid) {
    SendClientMessage(playerid, 0x00D4FFFF, "[WEBSITE]{FFFFFF} Posjeti nas website: https://ug-ogc.com");
}

// ===========================================================================
//  CALLBACK: RADIO
// ===========================================================================
forward OnCefPortalRadio(playerid, radioState);
public OnCefPortalRadio(playerid, radioState) {
    if(radioState == 1) {
        SendClientMessage(playerid, 0x00D4FFFF, "[RADIO]{FFFFFF} Unicate Radio pokrenut!");
    } else {
        SendClientMessage(playerid, 0xFF6B6BFF, "[RADIO]{FFFFFF} Unicate Radio zaustavljen.");
    }
}

// ===========================================================================
//  CALLBACK: LOGIN POKUŠAJ (iz portala - igrac kliknuo "Prijavi se")
// ===========================================================================
forward OnCefPortalLoginAttempt(playerid, username[], password[]);
public OnCefPortalLoginAttempt(playerid, username[], password[]) {
    printf("[CEF-PORTAL] Login attempt: player=%d, username=%s", playerid, username);

    if(!PortalData[playerid][pt_IsOpen]) {
        printf("[CEF-PORTAL] WARNING: Login attempt but portal not active for player %d", playerid);
        return 0;
    }

    // Pozovi gamemode login handler - gamemode treba definisati OnCefPortalLogin
    CallLocalFunction("OnCefPortalLogin", "dss", playerid, username, password);
    return 1;
}

// ===========================================================================
//  CALLBACK: REGISTER POKUŠAJ (iz portala - igrac kliknuo "Registruj se")
// ===========================================================================
forward OnCefPortalRegisterAttempt(playerid, username[], email[], password[]);
public OnCefPortalRegisterAttempt(playerid, username[], email[], password[]) {
    printf("[CEF-PORTAL] Register attempt: player=%d, username=%s, email=%s", playerid, username, email);

    if(!PortalData[playerid][pt_IsOpen]) {
        printf("[CEF-PORTAL] WARNING: Register attempt but portal not active for player %d", playerid);
        return 0;
    }

    // Pozovi gamemode register handler - gamemode treba definisati OnCefPortalRegister
    CallLocalFunction("OnCefPortalRegister", "dsss", playerid, username, email, password);
    return 1;
}

// ===========================================================================
//  POŠALJI LOGIN ODGOVOR NATRAG U PORTAL (CEF)
// ===========================================================================
stock CefPortal_SendLoginResponse(playerid, bool:success, message[]) {
    if(!PortalData[playerid][pt_IsOpen]) return;
    CEF_EmitEvent(playerid, PortalData[playerid][pt_BrowserId], "portal:loginResponse",
        CEF_INT(success ? 1 : 0),
        CEF_STR(message)
    );
}

// ===========================================================================
//  POŠALJI REGISTER ODGOVOR NATRAG U PORTAL (CEF)
// ===========================================================================
stock CefPortal_SendRegisterResponse(playerid, bool:success, message[]) {
    if(!PortalData[playerid][pt_IsOpen]) return;
    CEF_EmitEvent(playerid, PortalData[playerid][pt_BrowserId], "portal:registerResponse",
        CEF_INT(success ? 1 : 0),
        CEF_STR(message)
    );
}

// ===========================================================================
//  PORTAL FINISHED - zatvori portal i spawnaj igraca
// ===========================================================================
stock CefPortal_Finish(playerid) {
    printf("[CEF-PORTAL] Finish for player %d", playerid);

    CefPortal_Hide(playerid);
    CEF_ToggleSpawnScreen(playerid, true);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_ALL, true);
    PortalData[playerid][pt_WaitingSpawn] = false;

    // Obavijesti gamemode
    CallLocalFunction("OnPortalFinished", "d", playerid);
}

// ===========================================================================
//  PROVJERA DA LI JE PORTAL OTVOREN
// ===========================================================================
stock CefPortal_IsOpen(playerid) {
    return PortalData[playerid][pt_IsOpen];
}

// ===========================================================================
//  PROVJERA DA LI IGRAC CEKA SPAWN (portal je otvoren, nije kliknuo IGRAJ)
// ===========================================================================
stock CefPortal_WaitingSpawn(playerid) {
    return PortalData[playerid][pt_WaitingSpawn];
}

// ===========================================================================
//  DA LI JE IGRAC UPRAVO KLIKNUO IGRAJ (portal je upravo zatvoren)
// ===========================================================================
stock CefPortal_JustPlayed(playerid) {
    return (!PortalData[playerid][pt_IsOpen] && !PortalData[playerid][pt_WaitingSpawn]);
}

// ===========================================================================
//  ON PLAYER DISCONNECT - Cleanup
// ===========================================================================
stock CefPortal_OnPlayerDisconnect(playerid) {
    if(PortalData[playerid][pt_IsOpen]) {
        CefPortal_Hide(playerid);
    }
    PortalData[playerid][pt_IsOpen] = false;
    PortalData[playerid][pt_BrowserId] = -1;
    PortalData[playerid][pt_WaitingSpawn] = false;
}

// ===========================================================================
//  PUBLIC WRAPPER - za CallLocalFunction iz cef_loading.inc
//  CallLocalFunction moze pozivati samo public funkcije, ne stock
// ===========================================================================
forward CefPortal_ShowPublic(playerid);
public CefPortal_ShowPublic(playerid) {
    CefPortal_Show(playerid);
    return 1;
}

#endif // USE_CEF
