#if defined _INC_cef_amenu
        #endinput
#endif
#define _INC_cef_amenu

#define CEF_AMENU_BROWSER_ID    70
#define CEF_AMENU_URL           "http://cef/amenu/index.html"

// Store the target playerid for amenu so we can send it after browser is created
static AmenuTargetID[MAX_PLAYERS] = {-1, ...};

stock CEF_AmenuOpen(playerid, targetid)
{
        // Sacuvaj targetid za kasnije - event se salje tek kad je browser kreiran
        AmenuTargetID[playerid] = targetid;
        CEF_CreateBrowser(playerid, CEF_AMENU_BROWSER_ID, CEF_AMENU_URL, true);
}

stock CefAmenu_OnGameModeInit()
{
        // Register amenu resource
        CEF_AddResource("amenu");
        CefAmenu_InitSubscriptions();
        printf("[CEF-AMENU] Resource registered + events: amenu -> scriptfiles/cef/amenu/");
}

stock CefAmenu_InitSubscriptions()
{
        CEF_RegisterEvent("amenu:close", "OnCefAmenuClose");
        CEF_RegisterEvent("amenu:addItem", "OnCefAmenuAddItem", Argument_Integer, Argument_Integer);
        CEF_RegisterEvent("amenu:clearInventory", "OnCefAmenuClearInventory");
        CEF_RegisterEvent("amenu:setPin", "OnCefAmenuSetPin", Argument_String);
        CEF_RegisterEvent("amenu:deletePin", "OnCefAmenuDeletePin");
        CEF_RegisterEvent("amenu:checkPin", "OnCefAmenuCheckPin", Argument_String);
        CEF_RegisterEvent("amenu:requestPlayers", "OnCefAmenuRequestPlayers");
        CEF_RegisterEvent("amenu:loadItems", "OnCefAmenuLoadItems");
}

stock CefAmenu_OnPlayerDisconnect(playerid)
{
        AmenuTargetID[playerid] = -1;
}

stock CefAmenu_OnCefReady(playerid)
{
        #pragma unused playerid
}

stock CefAmenu_OnBrowserCreated(playerid, browserid)
{
        if(browserid != CEF_AMENU_BROWSER_ID) return;
        printf("[CEF-AMENU] Amenu browser created for player %d", playerid);

        // Postavi vidljivost i fokus
        CEF_SetBrowserVisible(playerid, CEF_AMENU_BROWSER_ID, true);
        CEF_FocusBrowser(playerid, CEF_AMENU_BROWSER_ID, true);

        // Sada je browser spreman - posalji amenu:open sa targetid-om
        new targetid = AmenuTargetID[playerid];
        if(targetid >= 0 && targetid < MAX_PLAYERS && IsPlayerConnected(targetid))
        {
                // Posalji podatke o target igracu
                new targetName[MAX_PLAYER_NAME];
                GetPlayerName(targetid, targetName, sizeof(targetName));
                CEF_EmitEvent(playerid, CEF_AMENU_BROWSER_ID, "amenu:open",
                        CEF_INT(targetid),
                        CEF_STR(targetName),
                        CEF_INT(PlayerInfo[targetid][xLevel]),
                        CEF_INT(PlayerInfo[targetid][xNovac] + PlayerInfo[targetid][xBRacun]));
        }
        else
        {
                CEF_EmitEvent(playerid, CEF_AMENU_BROWSER_ID, "amenu:open",
                        CEF_INT(targetid), CEF_STR("Unknown"), CEF_INT(0), CEF_INT(0));
        }

        // Resetuj targetid
        AmenuTargetID[playerid] = -1;
}

stock CEF_AmenuClose(playerid)
{
        CEF_DestroyBrowser(playerid, CEF_AMENU_BROWSER_ID);
}

forward OnCefAmenuClose(playerid);
forward OnCefAmenuAddItem(playerid, itemid, quantity);
forward OnCefAmenuClearInventory(playerid);
forward OnCefAmenuSetPin(playerid, const pin[]);
forward OnCefAmenuDeletePin(playerid);
forward OnCefAmenuCheckPin(playerid, const pin[]);
forward OnCefAmenuRequestPlayers(playerid);
forward OnCefAmenuLoadItems(playerid);
