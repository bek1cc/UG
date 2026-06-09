#if defined _INC_cef_phone
	#endinput
#endif
#define _INC_cef_phone

#define CEF_PHONE_BROWSER_ID    10
#define CEF_PHONE_URL           "cef://phone/index.html"

static bool:PhoneOpen[MAX_PLAYERS];

stock CEF_PhoneOpen(playerid)
{
	PhoneOpen[playerid] = true;
	CEF_CreateBrowser(playerid, CEF_PHONE_BROWSER_ID, CEF_PHONE_URL, true);
}

stock CEF_PhoneClose(playerid)
{
	PhoneOpen[playerid] = false;
	CEF_DestroyBrowser(playerid, CEF_PHONE_BROWSER_ID);
}

stock bool:CEF_IsPhoneOpen(playerid)
{
	return PhoneOpen[playerid];
}

stock CEF_PhoneToggleExpand(playerid)
{
	#pragma unused playerid
}

stock CEF_PhoneInitSubscriptions()
{
	CEF_RegisterEvent("phone:call", "OnCefPhoneCall", Argument_String);
	CEF_RegisterEvent("phone:sms", "OnCefPhoneSms", Argument_String, Argument_String);
	CEF_RegisterEvent("phone:close", "OnCefPhoneClose");
}

stock CEF_PhoneOnPlayerDisconnect(playerid)
{
	PhoneOpen[playerid] = false;
}

stock CefPhone_OnCefReady(playerid)
{
	#pragma unused playerid
}

stock CefPhone_OnBrowserCreated(playerid, browserid, bool:success)
{
	#pragma unused playerid, browserid, success
}

forward OnCefPhoneCall(playerid, const number[]);
forward OnCefPhoneSms(playerid, const number[], const message[]);
forward OnCefPhoneClose(playerid);
