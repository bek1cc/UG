#if defined _INC_cef_loading
	#endinput
#endif
#define _INC_cef_loading

#define CEF_LOADING_BROWSER_ID  50
#define CEF_LOADING_URL         "cef://loading/index.html"

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
	#pragma unused playerid
}

stock CefLoading_OnBrowserCreated(playerid, browserid, bool:success)
{
	#pragma unused playerid, browserid, success
}

stock CefLoading_OnGameModeInit()
{
	// Register loading screen resource
}

stock CefLoading_OnPlayerDisconnect(playerid)
{
	#pragma unused playerid
}
