#if defined _INC_cef_inventory
	#endinput
#endif
#define _INC_cef_inventory

#define CEF_INV_BROWSER_ID      30
#define CEF_INV_URL             "cef://inventory/index.html"

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

stock CefInv_OnBrowserCreated(playerid, browserid, bool:success)
{
	#pragma unused playerid, browserid, success
}

forward OnCefInvUse(playerid, itemid);
forward OnCefInvDrop(playerid, itemid);
forward OnCefInvClose(playerid);
