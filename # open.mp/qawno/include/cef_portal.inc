#if defined _INC_cef_portal
        #endinput
#endif
#define _INC_cef_portal

#define CEF_PORTAL_BROWSER_ID   60
#define CEF_PORTAL_URL          "http://cef/portal/index.html"

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
                printf("[CEF-PORTAL] Portal browser created for player %d", playerid);

                new playerName[MAX_PLAYER_NAME];
                GetPlayerName(playerid, playerName, sizeof(playerName));

                // Ponovo ucitaj account - koristi ManualReadAccount iz fg-ogc.pwn
                ManualReadAccount(playerid);

                new isRegistered = PlayerInfo[playerid][xRegistrovan];

                // Fallback: ako xRegistrovan=0 ali lozinka postoji, smatraj ga registrovanim
                if(!isRegistered && strlen(PlayerInfo[playerid][xLozinka]) > 0) {
                        isRegistered = 1;
                        PlayerInfo[playerid][xRegistrovan] = 1;
                }

                printf("[CEF-PORTAL] Player %s: isRegistered=%d, passLen=%d",
                        playerName, isRegistered, strlen(PlayerInfo[playerid][xLozinka]));

                // Posalji ime i registrovan status portalu (BEZ lozinke!)
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:init",
                        CEF_STR(playerName),
                        CEF_INT(isRegistered));

                CEF_FocusBrowser(playerid, CEF_PORTAL_BROWSER_ID, true);

                // =====================================================
                // PRIKAZI SA-MP DIALOG ZA UNOS - CEF input ne radi!
                // SA-MP dialog UVIJEK prima tastaturu ispravno
                // =====================================================
                if(isRegistered)
                {
                        // LOGIN - prikazi dialog za lozinku
                        new loginStr[256];
                        format(loginStr, sizeof(loginStr),
                                "{FFFFFF}Dobrodosao nazad, {0074c2}%s{FFFFFF}!\n\nUnesi svoju lozinku da bi nastavio.",
                                playerName);
                        ShowPlayerDialog(playerid, dialog_CEF_LOGIN, DIALOG_STYLE_PASSWORD,
                                "{0074c2}UG GAMING - Prijava", loginStr, "Prijava", "Izlaz");
                }
                else
                {
                        // REGISTER - prikazi dialog za lozinku
                        new regStr[256];
                        format(regStr, sizeof(regStr),
                                "{FFFFFF}Zdravo, {0074c2}%s{FFFFFF}!\n\nIzaberi lozinku za svoj nalog.\n{FF0000}Napomena:{FFFFFF} Lozinka mora imati minimalno 6 karaktera.",
                                playerName);
                        ShowPlayerDialog(playerid, dialog_CEF_REGISTER, DIALOG_STYLE_INPUT,
                                "{0074c2}UG GAMING - Registracija", regStr, "Nastavi", "Izlaz");
                }
        }
}

stock CefPortal_InitSubscriptions()
{
        // Eventi vise nisu potrebni za login/register - koristimo SA-MP dialog
        // Ostavljamo za mogucu buducu upotrebu
}

stock CefPortal_OnGameModeInit()
{
        CEF_AddResource("portal");
        CefPortal_InitSubscriptions();
}

stock CefPortal_OnPlayerDisconnect(playerid)
{
        PortalOpen[playerid] = false;
}
