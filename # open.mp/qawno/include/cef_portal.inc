#if defined _INC_cef_portal
	#endinput
#endif
#define _INC_cef_portal

#define CEF_PORTAL_BROWSER_ID   60
#define CEF_PORTAL_URL          "cef://portal/index.html"

static bool:PortalActive[MAX_PLAYERS];

stock CEF_PortalOpen(playerid)
{
	PortalActive[playerid] = true;
	CEF_CreateBrowser(playerid, CEF_PORTAL_BROWSER_ID, CEF_PORTAL_URL, true);
}

stock CEF_PortalClose(playerid)
{
	PortalActive[playerid] = false;
	CEF_DestroyBrowser(playerid, CEF_PORTAL_BROWSER_ID);
}

stock bool:CefPortal_WaitingSpawn(playerid)
{
	return PortalActive[playerid];
}

stock CefPortal_OnBrowserCreated(playerid, browserid, bool:success)
{
	#pragma unused playerid, browserid, success
}

stock CefPortal_OnGameModeInit()
{
	// Register portal resource
}

stock CefPortal_OnPlayerDisconnect(playerid)
{
	PortalActive[playerid] = false;
}
