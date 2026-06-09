#if defined _INC_cef_tablet
        #endinput
#endif
#define _INC_cef_tablet

#define CEF_TABLET_BROWSER_ID   20
#define CEF_TABLET_URL          "http://cef/tablet/index.html"

static bool:TabletOpen[MAX_PLAYERS];

stock CEF_TabletOpen(playerid)
{
        TabletOpen[playerid] = true;
        CEF_CreateBrowser(playerid, CEF_TABLET_BROWSER_ID, CEF_TABLET_URL, true);
}

stock CEF_TabletClose(playerid)
{
        TabletOpen[playerid] = false;
        CEF_DestroyBrowser(playerid, CEF_TABLET_BROWSER_ID);
}

stock bool:CEF_IsTabletOpen(playerid)
{
        return TabletOpen[playerid];
}

// NOTE: cef_subscribe is already defined in cef.inc - do not redefine here

stock CefTablet_OnCefReady(playerid)
{
        #pragma unused playerid
}

stock CefTablet_OnBrowserCreated(playerid, browserid, bool:success)
{
        #pragma unused playerid, browserid, success
}

stock CefTablet_OnPlayerDisconnect(playerid)
{
        TabletOpen[playerid] = false;
}

stock CefTablet_OnGameModeInit()
{
        CEF_AddResource("cef/tablet");
        printf("[CEF-TABLET] Resource registered: cef/tablet -> cef://tablet/index.html");
}
