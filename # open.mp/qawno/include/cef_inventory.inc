#if defined _INC_cef_inventory
        #endinput
#endif
#define _INC_cef_inventory

#define CEF_INV_BROWSER_ID      30
#define CEF_INV_URL             "http://cef/inventory/index.html"

static bool:InventoryOpen[MAX_PLAYERS];

stock CEF_InventoryOpen(playerid)
{
        InventoryOpen[playerid] = true;
        CEF_CreateBrowser(playerid, CEF_INV_BROWSER_ID, CEF_INV_URL, true);
}

stock CEF_InventoryClose(playerid)
{
        InventoryOpen[playerid] = false;
        CEF_DestroyBrowser(playerid, CEF_INV_BROWSER_ID);
}

stock bool:CEF_IsInventoryOpen(playerid)
{
        return InventoryOpen[playerid];
}

stock CEF_InvInitSubscriptions()
{
        CEF_RegisterEvent("inventory:use", "OnCefInvUse", Argument_Integer);
        CEF_RegisterEvent("inventory:drop", "OnCefInvDrop", Argument_Integer);
        CEF_RegisterEvent("inventory:close", "OnCefInvClose");
}

stock CEF_InvOnGameModeInit()
{
        CEF_AddResource("inventory");
        printf("[CEF-INV] Resource registered: inventory -> scriptfiles/cef/inventory/");
}

stock CEF_InvOnPlayerDisconnect(playerid)
{
        InventoryOpen[playerid] = false;
}

stock CEF_InvAddItem(playerid, itemid, const name[], amount)
{
        CEF_EmitEvent(playerid, CEF_INV_BROWSER_ID, "inventory:add", CEF_INT(itemid), CEF_STR(name), CEF_INT(amount));
}

stock CEF_InvRemoveItem(playerid, itemid)
{
        CEF_EmitEvent(playerid, CEF_INV_BROWSER_ID, "inventory:remove", CEF_INT(itemid));
}

stock CefInv_OnCefReady(playerid)
{
        #pragma unused playerid
}

stock CefInv_OnBrowserCreated(playerid, browserid)
{
        if(browserid != CEF_INV_BROWSER_ID) return;
        printf("[CEF-INV] Inventory browser created for player %d", playerid);

        // Postavi vidljivost i fokus
        CEF_SetBrowserVisible(playerid, CEF_INV_BROWSER_ID, true);
        CEF_FocusBrowser(playerid, CEF_INV_BROWSER_ID, true);

        // Posalji init podatke inventaru
        new playerName[MAX_PLAYER_NAME];
        GetPlayerName(playerid, playerName, sizeof(playerName));
        CEF_EmitEvent(playerid, CEF_INV_BROWSER_ID, "inventory:playerInfo",
                CEF_STR(playerName),
                CEF_INT(playerid),
                CEF_INT(PlayerInfo[playerid][xLevel]),
                CEF_INT(PlayerInfo[playerid][xNovac]),
                CEF_INT(PlayerInfo[playerid][xRespekt]);
}

forward OnCefInvUse(playerid, itemid);
forward OnCefInvDrop(playerid, itemid);
forward OnCefInvClose(playerid);
