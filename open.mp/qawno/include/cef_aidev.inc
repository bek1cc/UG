#if defined _INC_cef_aidev
        #endinput
#endif
#define _INC_cef_aidev

// ===== AI DEVELOPER PANEL v2 - CEF Integration =====
// Modular Job Engine + CEF Registry + Live Reload + GMX
// Z.AI GLM-5.1 AI Assistant za SA:MP/open.mp server
// Samo za vlasnika/server admina (level 6+ ili RCON admin)

#define AIDEV_BROWSER_ID       90
#define AIDEV_CEF_URL          "http://127.0.0.1:3777/cef/ai-dev-panel/index.html"

new bool:AiDevCEFActive[MAX_PLAYERS];

// ===== OPEN / CLOSE =====
stock CEF_AiDevPanelOpen(playerid)
{
        if(AiDevCEFActive[playerid]) return 0;

        // Sigurnosna provjera - samo owner (admin 6) ili RCON admin
        if(PlayerInfo[playerid][xAdmin] < 6 && !IsPlayerAdmin(playerid))
        {
                SendErrorMessage(playerid, "Samo vlasnik moze koristiti AI Dev Panel!");
                return 0;
        }

        AiDevCEFActive[playerid] = true;
        CEF_CreateBrowser(playerid, AIDEV_BROWSER_ID, AIDEV_CEF_URL, true);

        printf("[AI-DEV] Panel opened for player %d (Admin: %d)", playerid, PlayerInfo[playerid][xAdmin]);
        return 1;
}

stock CEF_AiDevPanelClose(playerid)
{
        if(!AiDevCEFActive[playerid]) return 0;
        AiDevCEFActive[playerid] = false;
        CEF_DestroyBrowser(playerid, AIDEV_BROWSER_ID);
        return 1;
}

stock CEF_AiDevPanelIsOpen(playerid)
{
        return AiDevCEFActive[playerid] ? true : false;
}

// ===== BROWSER CREATED CALLBACK =====
stock CefAiDev_OnBrowserCreated(playerid, browserid)
{
        if(browserid == AIDEV_BROWSER_ID)
        {
                printf("[AI-DEV] Browser created for player %d, sending init data", playerid);

                // Send init data: playerid, admin_key
                CEF_EmitEvent(playerid, AIDEV_BROWSER_ID, "aidev:init",
                        CEF_INT(playerid),
                        CEF_STR("ug-admin-2024"));

                CEF_FocusBrowser(playerid, AIDEV_BROWSER_ID, true);
        }
}

// ===== RELOAD CEF PANEL =====
// AI Dev Panel salje ovaj event kad treba reloadati odredjeni CEF panel
// browserId = ID browsera koji treba reload (npr. 20 za tablet)
// targetPlayer = playerid za kojeg treba reload (ako 0, reload za sve)
forward OnCefAiDevReloadCef(playerid, browserId, targetPlayer);
public OnCefAiDevReloadCef(playerid, browserId, targetPlayer)
{
        printf("[AI-DEV] Reload CEF request from player %d: Browser %d for player %d", playerid, browserId, targetPlayer);

        // Odredi za kojeg igraca radimo reload
        new reloadFor = targetPlayer;
        if(reloadFor <= 0) reloadFor = playerid;

        if(!IsPlayerConnected(reloadFor))
        {
                printf("[AI-DEV] Target player %d not connected, skipping reload", reloadFor);
                return 1;
        }

        // Unisti i ponovo kreiraj browser za tog igraca
        CEF_DestroyBrowser(reloadFor, browserId);

        // Map browser ID to URL for re-creation
        new url[128];
        switch(browserId)
        {
                case 10: format(url, sizeof(url), "http://cef/phone/index.html");
                case 20: format(url, sizeof(url), "http://cef/tablet/index.html");
                case 30: format(url, sizeof(url), "http://cef/inventory/index.html");
                case 40: format(url, sizeof(url), "http://cef/laptop/index.html");
                case 60: format(url, sizeof(url), "http://cef/portal/index.html");
                case 70: format(url, sizeof(url), "http://cef/amenu/index.html");
                case 80: format(url, sizeof(url), "http://cef/case/index.html");
                case 81: format(url, sizeof(url), "http://cef/cardcase/index.html");
                case 82: format(url, sizeof(url), "http://cef/dog/index.html");
                case 90: format(url, sizeof(url), "http://127.0.0.1:3777/cef/ai-dev-panel/index.html");
                case 95: format(url, sizeof(url), "http://cef/tstudio/index.html");
                default:
                {
                        printf("[AI-DEV] Unknown browser ID %d, cannot reload", browserId);
                        return 1;
                }
        }

        // Ne reloadaj AI Dev Panel samog (browser 90) - to bi zatvorilo panel
        if(browserId == AIDEV_BROWSER_ID)
        {
                printf("[AI-DEV] Skipping self-reload (Browser 90)");
                return 1;
        }

        // Kreiraj novi browser sa istim URL-om
        CEF_CreateBrowser(reloadFor, browserId, url, true);

        printf("[AI-DEV] Reloaded CEF browser %d for player %d → %s", browserId, reloadFor, url);
        return 1;
}

// ===== CEF EVENT CALLBACKS =====
forward OnCefAiDevClose(playerid);
public OnCefAiDevClose(playerid)
{
        printf("[AI-DEV] Player %d closed AI Dev Panel", playerid);
        CEF_AiDevPanelClose(playerid);
        return 1;
}

// ===== COMMANDS =====
CMD:aidev(playerid, params[])
{
        if(!PlayerLogged[playerid]) return SendErrorMessage(playerid, "Morate biti ulogovani!");

        // Samo owner (admin 6) ili RCON admin
        if(PlayerInfo[playerid][xAdmin] < 6 && !IsPlayerAdmin(playerid))
        {
                return SendErrorMessage(playerid, "Samo vlasnik moze koristiti AI Dev Panel!");
        }

        // Toggle CEF panel
        if(CEF_AiDevPanelIsOpen(playerid))
        {
                CEF_AiDevPanelClose(playerid);
        }
        else
        {
                CEF_AiDevPanelOpen(playerid);
        }
        return 1;
}

// ===== GAMEMODE INIT =====
stock CefAiDev_OnGameModeInit()
{
        CEF_AddResource("ai-dev-panel");

        // Register CEF events from browser
        CEF_RegisterEvent("aidev:close", "OnCefAiDevClose");
        CEF_RegisterEvent("aidev:reloadcef", "OnCefAiDevReloadCef", Argument_Integer, Argument_Integer);

        printf("[AI-DEV v2] Resource + events registered");
        printf("[AI-DEV v2] Commands: /aidev");
        printf("[AI-DEV v2] Backend: cd ai-backend && node server.js");
}

stock CefAiDev_OnPlayerDisconnect(playerid)
{
        if(AiDevCEFActive[playerid])
        {
                AiDevCEFActive[playerid] = false;
        }
}
