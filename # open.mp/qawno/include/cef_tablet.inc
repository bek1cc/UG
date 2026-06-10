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

stock CefTablet_InitSubscriptions()
{
        CEF_RegisterEvent("tablet:loginAttempt", "OnCefTabletLogin", Argument_String);
        CEF_RegisterEvent("tablet:registerAttempt", "OnCefTabletRegister", Argument_String);
        CEF_RegisterEvent("tablet:requestStats", "OnCefTabletRequestStats");
        CEF_RegisterEvent("tablet:requestBounties", "OnCefTabletRequestBounties");
        CEF_RegisterEvent("tablet:close", "OnCefTabletClose");
}

stock CefTablet_OnCefReady(playerid)
{
        #pragma unused playerid
}

stock CefTablet_OnBrowserCreated(playerid, browserid)
{
        if(browserid != CEF_TABLET_BROWSER_ID) return;
        printf("[CEF-TABLET] Tablet browser created for player %d", playerid);

        // Postavi vidljivost i fokus
        CEF_SetBrowserVisible(playerid, CEF_TABLET_BROWSER_ID, true);
        CEF_FocusBrowser(playerid, CEF_TABLET_BROWSER_ID, true);

        // Posalji init podatke tabletu
        CEF_EmitEvent(playerid, CEF_TABLET_BROWSER_ID, "tablet:init",
                CEF_STR(PlayerInfo[playerid][xLozinka]),
                CEF_INT(PlayerInfo[playerid][xNovac]),
                CEF_INT(PlayerInfo[playerid][xLevel]),
                CEF_INT(PlayerInfo[playerid][xBrojTelefona]));
}

stock CefTablet_OnPlayerDisconnect(playerid)
{
        TabletOpen[playerid] = false;
}

stock CefTablet_OnGameModeInit()
{
        CEF_AddResource("tablet");
        CefTablet_InitSubscriptions();
        printf("[CEF-TABLET] Resource registered + events: tablet -> scriptfiles/cef/tablet/");
}

forward OnCefTabletLogin(playerid, const pin[]);
forward OnCefTabletRegister(playerid, const pin[]);
forward OnCefTabletRequestStats(playerid);
forward OnCefTabletRequestBounties(playerid);
forward OnCefTabletClose(playerid);
