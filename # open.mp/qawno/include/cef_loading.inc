#if defined _INC_cef_loading
        #endinput
#endif
#define _INC_cef_loading

#define CEF_LOADING_BROWSER_ID  50
#define CEF_LOADING_URL         "http://cef/loading/index.html"

stock CEF_LoadingOpen(playerid)
{
        CEF_CreateBrowser(playerid, CEF_LOADING_BROWSER_ID, CEF_LOADING_URL, false);
}

stock CEF_LoadingClose(playerid)
{
        CEF_DestroyBrowser(playerid, CEF_LOADING_BROWSER_ID);
}

stock CefLoading_OnCefReady(playerid)
{
        // CEF is ready - show the loading screen immediately
        printf("[CEF-LOADING] CefReady for player %d - opening loading screen", playerid);
        CEF_LoadingOpen(playerid);
}

stock CefLoading_OnBrowserCreated(playerid, browserid)
{
        if(browserid == CEF_LOADING_BROWSER_ID)
        {
                printf("[CEF-LOADING] Loading browser created successfully for player %d", playerid);
        }
}

stock CefLoading_InitSubscriptions()
{
        // Register the OnLoadingComplete event from the loading screen JS
        // When loading screen reaches 100%, it calls: cef.emit('OnLoadingComplete')
        CEF_RegisterEvent("OnLoadingComplete", "OnCefLoadingComplete");
}

stock CefLoading_OnGameModeInit()
{
        // Register loading screen resource so CEF can find it
        CEF_AddResource("cef/loading");
        CefLoading_InitSubscriptions();
}

stock CefLoading_OnPlayerDisconnect(playerid)
{
        #pragma unused playerid
}

// Called when the loading screen JS fires cef.emit('OnLoadingComplete')
forward OnCefLoadingComplete(playerid);
public OnCefLoadingComplete(playerid)
{
        printf("[CEF-LOADING] Loading complete for player %d - closing loading screen, opening portal", playerid);

        // Close the loading screen browser
        CEF_LoadingClose(playerid);

        // Now open the portal (login/register screen)
        CEF_PortalOpen(playerid);
}
