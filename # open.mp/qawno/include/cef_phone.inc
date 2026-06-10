#if defined _INC_cef_phone
        #endinput
#endif
#define _INC_cef_phone

#define CEF_PHONE_BROWSER_ID    10
#define CEF_PHONE_URL           "http://cef/phone/index.html"

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

stock CEF_PhoneOnGameModeInit()
{
        CEF_AddResource("phone");
        printf("[CEF-PHONE] Resource registered: phone -> scriptfiles/cef/phone/");
}

stock CEF_PhoneOnPlayerDisconnect(playerid)
{
        PhoneOpen[playerid] = false;
}

stock CefPhone_OnCefReady(playerid)
{
        #pragma unused playerid
}

stock CefPhone_OnBrowserCreated(playerid, browserid)
{
        if(browserid != CEF_PHONE_BROWSER_ID) return;
        printf("[CEF-PHONE] Phone browser created for player %d", playerid);

        // Postavi vidljivost i fokus
        CEF_SetBrowserVisible(playerid, CEF_PHONE_BROWSER_ID, true);
        CEF_FocusBrowser(playerid, CEF_PHONE_BROWSER_ID, true);

        // Posalji init podatke telefonu
        // HTML ocekuje: phone:init(playerName, phoneNumber, money, battery)
        new playerName[MAX_PLAYER_NAME];
        GetPlayerName(playerid, playerName, sizeof(playerName));
        new phoneNumber = PlayerInfo[playerid][xBrojTelefona];
        CEF_EmitEvent(playerid, CEF_PHONE_BROWSER_ID, "phone:init",
                CEF_STR(playerName),
                CEF_INT(phoneNumber),
                CEF_INT(PlayerInfo[playerid][xNovac]),
                CEF_INT(100));
}

forward OnCefPhoneCall(playerid, const number[]);
forward OnCefPhoneSms(playerid, const number[], const message[]);
forward OnCefPhoneClose(playerid);
