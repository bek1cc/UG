// ===========================================================================
//  CEF LOADING SCREEN za open.mp (omp-cef plugin)
//  File: pawno/include/cef_loading.inc
//
//  Prikazuje moderan loading screen kada igrac sa CEF-om udje na server.
//  Nakon loadinga, automatski prelazi na portal (cef_portal.inc).
//
//  Integracija:
//    #define USE_CEF
//    #include <cef_loading>
// ===========================================================================

#if defined _cef_loading_included
    #endinput
#endif
#define _cef_loading_included

#define CEF_LOADING_RESOURCE     "loading"
#define CEF_LOADING_BROWSER_ID   10
#define CEF_LOADING_WIDTH        0.0   // fullscreen (-1)
#define CEF_LOADING_HEIGHT       0.0   // fullscreen (-1)

enum LoadingPlayerData {
    bool:ld_IsOpen,
    ld_BrowserId
}

static LoadingData[MAX_PLAYERS][LoadingPlayerData];

#if defined USE_CEF

#if !defined _INC_cef
    #include <cef>
#endif

// ===========================================================================
//  GAME MODE INIT - Registracija resursa i eventa
// ===========================================================================
stock CefLoading_OnGameModeInit() {
    CEF_AddResource(CEF_LOADING_RESOURCE);
    CEF_RegisterEvent("loading:complete", "OnCefLoadingComplete");
    printf("[CEF-LOADING] Resource '%s' registriran, event 'loading:complete' -> OnCefLoadingComplete", CEF_LOADING_RESOURCE);
}

// ===========================================================================
//  PRIKAZI LOADING SCREEN
// ===========================================================================
stock CefLoading_Show(playerid) {
    if(LoadingData[playerid][ld_IsOpen]) return;
    if(!IsPlayerConnected(playerid)) return;
    if(!CEF_PlayerHasPlugin(playerid)) return;

    // Sakrij default spawn screen
    CEF_ToggleSpawnScreen(playerid, false);

    // Sakrij HUD
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_ALL, false);

    // Kreiraj fullscreen browser za loading
    LoadingData[playerid][ld_BrowserId] = CEF_LOADING_BROWSER_ID;
    CEF_CreateBrowser(playerid, LoadingData[playerid][ld_BrowserId], "http://cef/loading/index.html", true, false);

    LoadingData[playerid][ld_IsOpen] = true;
    printf("[CEF-LOADING] Prikazan loading screen za player %d (browser ID: %d)", playerid, LoadingData[playerid][ld_BrowserId]);
}

// ===========================================================================
//  ZATVORI LOADING SCREEN
// ===========================================================================
stock CefLoading_Hide(playerid) {
    if(!LoadingData[playerid][ld_IsOpen]) return;

    CEF_DestroyBrowser(playerid, LoadingData[playerid][ld_BrowserId]);
    LoadingData[playerid][ld_IsOpen] = false;
    printf("[CEF-LOADING] Sklonjen loading screen za player %d", playerid);
}

// ===========================================================================
//  ON CEF READY - Pokazi loading screen cim je CEF spreman
// ===========================================================================
stock CefLoading_OnCefReady(playerid) {
    CefLoading_Show(playerid);
}

// ===========================================================================
//  ON BROWSER CREATED - Posalji init podatke
// ===========================================================================
stock CefLoading_OnBrowserCreated(playerid, browserid) {
    if(browserid != CEF_LOADING_BROWSER_ID) return;
    if(!LoadingData[playerid][ld_IsOpen]) return;

    printf("[CEF-LOADING] Browser created for player %d (browserid: %d) - sending loading:init", playerid, browserid);

    CEF_FocusBrowser(playerid, LoadingData[playerid][ld_BrowserId], true);

    // Posalji ping i cache status
    new ping = GetPlayerPing(playerid);
    // Ako je ping <= 50, vjerovatno ima lokalne cache fajlove
    new hasCache = (ping <= 50) ? 1 : 0;
    CEF_EmitEvent(playerid, LoadingData[playerid][ld_BrowserId], "loading:init",
        CEF_INT(ping),
        CEF_INT(hasCache)
    );
    printf("[CEF-LOADING] Sent loading:init to player %d (ping: %d, hasCache: %d)", playerid, ping, hasCache);
}

// ===========================================================================
//  CALLBACK - Loading zavrsen (JS emituje "loading:complete")
// ===========================================================================
forward OnCefLoadingComplete(playerid);
public OnCefLoadingComplete(playerid) {
    printf("[CEF-LOADING] OnCefLoadingComplete CALLED for player %d (ld_IsOpen=%d)", playerid, LoadingData[playerid][ld_IsOpen]);

    if(!LoadingData[playerid][ld_IsOpen]) {
        printf("[CEF-LOADING] WARNING: ld_IsOpen je false za player %d - preskacem", playerid);
        return 1;
    }

    printf("[CEF-LOADING] Loading zavrsen za player %d - prelazim na portal", playerid);

    // Zatvori loading screen
    CefLoading_Hide(playerid);

    // Otvori portal koristeci CallLocalFunction
    // RAZLOG: Ne mozemo koristiti #if defined _cef_portal_included jer je
    // cef_loading.inc inkludan PRIJE cef_portal.inc - PAWN kompajler ne vidi
    // _cef_portal_included u trenutku kompajlacije ovog fajla.
    // CallLocalFunction radi late-binding i pronalazi funkciju po imenu.
    // VAZNO: CallLocalFunction moze pozivati SAMO public funkcije!
    // CefPortal_ShowPublic je public wrapper u cef_portal.inc
    new portalResult = CallLocalFunction("CefPortal_ShowPublic", "d", playerid);
    printf("[CEF-LOADING] CallLocalFunction CefPortal_ShowPublic za player %d (result=%d)", playerid, portalResult);

    return 1;
}

// ===========================================================================
//  ON PLAYER DISCONNECT - Cleanup
// ===========================================================================
stock CefLoading_OnPlayerDisconnect(playerid) {
    if(LoadingData[playerid][ld_IsOpen]) {
        CefLoading_Hide(playerid);
    }
    LoadingData[playerid][ld_IsOpen] = false;
    LoadingData[playerid][ld_BrowserId] = -1;
}

#endif // USE_CEF
