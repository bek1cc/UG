#if defined _INC_cef_tablet
	#endinput
#endif
#define _INC_cef_tablet

#define CEF_TABLET_BROWSER_ID   20
#define CEF_TABLET_URL          "cef://tablet/index.html"

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

stock cef_subscribe(const eventName[], const callback[], E_CEF_ARGUMENT_TYPE:...)
{
	CEF_RegisterEvent(eventName, callback, E_CEF_ARGUMENT_TYPE:...);
}

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
