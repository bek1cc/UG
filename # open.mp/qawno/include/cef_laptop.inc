#if defined _INC_cef_laptop
        #endinput
#endif
#define _INC_cef_laptop

#define CEF_LAPTOP_BROWSER_ID   40
#define CEF_LAPTOP_URL          "http://cef/laptop/index.html"

static bool:LaptopOpen[MAX_PLAYERS];

stock CEF_LaptopOpen(playerid)
{
        LaptopOpen[playerid] = true;
        CEF_CreateBrowser(playerid, CEF_LAPTOP_BROWSER_ID, CEF_LAPTOP_URL, true);
}

stock CEF_LaptopClose(playerid)
{
        LaptopOpen[playerid] = false;
        CEF_DestroyBrowser(playerid, CEF_LAPTOP_BROWSER_ID);
}

stock bool:CEF_IsLaptopOpen(playerid)
{
        return LaptopOpen[playerid];
}

stock CEF_LpInitSubscriptions()
{
        CEF_RegisterEvent("laptop:close", "OnCefLaptopClose");
}

stock CEF_LpOnPlayerDisconnect(playerid)
{
        LaptopOpen[playerid] = false;
}

stock CefLp_OnGameModeInit()
{
        CEF_AddResource("cef/laptop");
        printf("[CEF-LAPTOP] Resource registered: cef/laptop -> cef://laptop/index.html");
}

stock CefLp_OnCefReady(playerid)
{
        #pragma unused playerid
}

stock CefLp_OnBrowserCreated(playerid, browserid, bool:success)
{
        #pragma unused playerid, browserid, success
}

forward OnCefLaptopClose(playerid);
