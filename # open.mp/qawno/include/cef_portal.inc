#if defined _INC_cef_portal
        #endinput
#endif
#define _INC_cef_portal

#define CEF_PORTAL_BROWSER_ID   60
#define CEF_PORTAL_URL          "cef://portal/index.html"

static bool:PortalOpen[MAX_PLAYERS];

stock CEF_PortalOpen(playerid)
{
        PortalOpen[playerid] = true;
        CEF_CreateBrowser(playerid, CEF_PORTAL_BROWSER_ID, CEF_PORTAL_URL, true);
}

stock CEF_PortalClose(playerid)
{
        PortalOpen[playerid] = false;
        CEF_DestroyBrowser(playerid, CEF_PORTAL_BROWSER_ID);
}

stock bool:CefPortal_WaitingSpawn(playerid)
{
        return PortalOpen[playerid];
}

stock CefPortal_OnBrowserCreated(playerid, browserid)
{
        if(browserid == CEF_PORTAL_BROWSER_ID)
        {
                printf("[CEF-PORTAL] Portal browser created successfully for player %d", playerid);

                // Send player info to portal JS
                new playerName[MAX_PLAYER_NAME];
                GetPlayerName(playerid, playerName, sizeof(playerName));
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:init", CEF_STR(playerName));

                // Focus the portal so player can type in login fields
                CEF_FocusBrowser(playerid, CEF_PORTAL_BROWSER_ID, true);
        }
}

stock CefPortal_InitSubscriptions()
{
        // Register events that portal JS can emit
        CEF_RegisterEvent("portal:loginAttempt", "OnCefPortalLogin", Argument_String, Argument_String);
        CEF_RegisterEvent("portal:registerAttempt", "OnCefPortalRegister", Argument_String, Argument_String, Argument_String);
}

stock CefPortal_OnGameModeInit()
{
        // Register portal resource so CEF can find it
        CEF_AddResource("cef/portal");
        CefPortal_InitSubscriptions();
}

stock CefPortal_OnPlayerDisconnect(playerid)
{
        PortalOpen[playerid] = false;
}

// Forward declaration - OnPortalFinished is implemented in the main gamemode
forward OnPortalFinished(playerid);

// Called when player clicks Login on the portal
forward OnCefPortalLogin(playerid, const username[], const password[]);
public OnCefPortalLogin(playerid, const username[], const password[])
{
        printf("[CEF-PORTAL] Login attempt from player %d, username: %s", playerid, username);

        // Close the portal - player is going into the game
        CEF_PortalClose(playerid);

        // Call OnPortalFinished which handles login/register and spawning
        OnPortalFinished(playerid);
}

// Called when player clicks Register on the portal
forward OnCefPortalRegister(playerid, const username[], const email[], const password[]);
public OnCefPortalRegister(playerid, const username[], const email[], const password[])
{
        printf("[CEF-PORTAL] Register attempt from player %d, username: %s, email: %s", playerid, username, email);

        // Close the portal - player is going into the game
        CEF_PortalClose(playerid);

        // Call OnPortalFinished which handles login/register and spawning
        OnPortalFinished(playerid);
}
