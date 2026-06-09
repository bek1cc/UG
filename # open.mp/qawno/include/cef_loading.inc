#if defined _INC_cef_loading
        #endinput
#endif
#define _INC_cef_loading

#define CEF_LOADING_BROWSER_ID  50
#define CEF_LOADING_URL         "http://cef/loading/index.html"

// Loading screen je UKLONJEN - portal se prikazuje direktno
// Ove funkcije ostaju kao stub-ovi da se ne bi kompajlacija break-ovala

stock CEF_LoadingOpen(playerid)
{
        #pragma unused playerid
        // NE OTVARAJ loading screen - portal se otvara direktno iz OnCefReady
}

stock CEF_LoadingClose(playerid)
{
        #pragma unused playerid
}

stock CefLoading_OnCefReady(playerid)
{
        // UKLONJENO: Ne prikazuje se vise loading screen
        // Portal se otvara direktno iz OnCefReady u fg-ogc.pwn
        printf("[CEF-LOADING] Skipping loading screen, portal opens directly for player %d", playerid);
}

stock CefLoading_OnBrowserCreated(playerid, browserid)
{
        #pragma unused playerid, browserid
}

stock CefLoading_InitSubscriptions()
{
        // Loading event se vise ne koristi, ali ostavljamo registraciju
        // da ne bi break-ovali kod ako se negdje poziva
}

stock CefLoading_OnGameModeInit()
{
        // I dalje registrujemo resurs za CEF, cak i ako loading screen nije u upotrebi
        CEF_AddResource("loading");
        CefLoading_InitSubscriptions();
        printf("[CEF-LOADING] Resource registered (disabled): loading -> scriptfiles/cef/loading/");
}

stock CefLoading_OnPlayerDisconnect(playerid)
{
        #pragma unused playerid
}

// Zadrzavamo forward za kompatibilnost, ali se ne poziva vise
forward OnCefLoadingComplete(playerid);
public OnCefLoadingComplete(playerid)
{
        printf("[CEF-LOADING] OnCefLoadingComplete called for player %d - but loading screen is disabled, ignoring", playerid);
}
