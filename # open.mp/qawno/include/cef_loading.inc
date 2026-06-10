#if defined _INC_cef_loading
        #endinput
#endif
#define _INC_cef_loading

// Loading screen je POTPUNO UKLONJEN
// Nema browser ID-a, nema URL-a, nema resursa
// Ove funkcije ostaju kao potpuni stub-ovi da se ne bi kompajlacija break-ovala

stock CEF_LoadingOpen(playerid)
{
        #pragma unused playerid
        // Loading screen je obrisan - ne radi nista
}

stock CEF_LoadingClose(playerid)
{
        #pragma unused playerid
}

stock CefLoading_OnCefReady(playerid)
{
        #pragma unused playerid
        // Loading screen ne postoji - ne radi nista
}

stock CefLoading_OnBrowserCreated(playerid, browserid)
{
        #pragma unused playerid, browserid
}

stock CefLoading_InitSubscriptions()
{
        // Loading event se vise ne koristi
}

stock CefLoading_OnGameModeInit()
{
        // NE REGISTRUJEMO loading resurs - obrisan je
        // CEF_AddResource("loading") - UKLONJENO
        printf("[CEF-LOADING] Loading screen completely removed - no resource registered");
}

stock CefLoading_OnPlayerDisconnect(playerid)
{
        #pragma unused playerid
}

// Zadrzavamo forward za kompatibilnost, ali se ne poziva vise
forward OnCefLoadingComplete(playerid);
public OnCefLoadingComplete(playerid)
{
        #pragma unused playerid
        // Loading screen je obrisan - ignorisi
}
