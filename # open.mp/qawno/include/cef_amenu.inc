#if defined _INC_cef_amenu
        #endinput
#endif
#define _INC_cef_amenu

#define CEF_AMENU_BROWSER_ID    70
#define CEF_AMENU_URL           "http://cef/amenu/index.html"

stock CEF_AmenuOpen(playerid, targetid)
{
        CEF_CreateBrowser(playerid, CEF_AMENU_BROWSER_ID, CEF_AMENU_URL, true);
        CEF_EmitEvent(playerid, CEF_AMENU_BROWSER_ID, "amenu:open", CEF_INT(targetid));
}

stock CefAmenu_OnGameModeInit()
{
        // Register amenu resource
        CEF_AddResource("cef/amenu");
        printf("[CEF-AMENU] Resource registered: cef/amenu -> cef://amenu/index.html");
}

stock CefAmenu_OnPlayerDisconnect(playerid)
{
        #pragma unused playerid
}

stock CefAmenu_OnCefReady(playerid)
{
        #pragma unused playerid
}

stock CefAmenu_OnBrowserCreated(playerid, browserid)
{
        #pragma unused playerid, browserid
}
