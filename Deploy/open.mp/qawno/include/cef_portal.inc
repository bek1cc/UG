#if defined _INC_cef_portal
        #endinput
#endif
#define _INC_cef_portal

#define CEF_PORTAL_BROWSER_ID   60
#define CEF_PORTAL_URL          "http://cef/portal/index.html"

// SA-MP Dialog IDs za portal
#define DIALOG_PORTAL_LOGIN     2001
#define DIALOG_PORTAL_REGISTER  2002

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

                // Ucitaj account podatke
                ManualReadAccount(playerid);

                new isRegistered = PlayerInfo[playerid][xRegistrovan];

                // Fallback: ako xRegistrovan=0 ali lozinka postoji, smatraj ga registrovanim
                if(!isRegistered && strlen(PlayerInfo[playerid][xLozinka]) > 0) {
                        isRegistered = 1;
                        PlayerInfo[playerid][xRegistrovan] = 1;
                }

                printf("[CEF-PORTAL] Player %s: isRegistered=%d, passLen=%d",
                        playerName, isRegistered, strlen(PlayerInfo[playerid][xLozinka]));

                // Posalji ime i registrovan status portalu
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:init",
                        CEF_STR(playerName),
                        CEF_INT(isRegistered));

                CEF_FocusBrowser(playerid, CEF_PORTAL_BROWSER_ID, true);
        }
}

stock CefPortal_InitSubscriptions()
{
        // Igrac klikne IGRAJ SADA u CEF portalu - pokazi SA-MP dijalog
        CEF_RegisterEvent("portal:requestAuth", "OnCefPortalRequestAuth");
}

stock CefPortal_OnGameModeInit()
{
        CEF_AddResource("portal");
        CefPortal_InitSubscriptions();
        printf("[CEF-PORTAL] Resource registered + events: portal -> scriptfiles/cef/portal/");
}

stock CefPortal_OnPlayerDisconnect(playerid)
{
        PortalOpen[playerid] = false;
}

// ===== Pokazi SA-MP dijalog za login ili register =====
stock CefPortal_ShowDialog(playerid)
{
        // Ponovo ucitaj account podatke
        ManualReadAccount(playerid);

        new isRegistered = PlayerInfo[playerid][xRegistrovan];
        if(!isRegistered && strlen(PlayerInfo[playerid][xLozinka]) > 0) {
                isRegistered = 1;
        }

        new playerName[MAX_PLAYER_NAME];
        GetPlayerName(playerid, playerName, sizeof(playerName));

        if(isRegistered)
        {
                // Pokazi LOGIN dijalog
                new caption[128], line1[256];
                format(caption, sizeof(caption), "{0088FF}UNICATE GAMING OGC{FFFFFF} - Prijava");
                format(line1, sizeof(line1),
                        "{FFFFFF}Dobrodosao natrag, {0088FF}%s{FFFFFF}!\n\n\
                        Unesi svoju lozinku da bi nastavio.\n\
                        {AAAAAA}Ako imas problema, kontaktiraj admin tim.", playerName);
                ShowPlayerDialog(playerid, DIALOG_PORTAL_LOGIN, DIALOG_STYLE_PASSWORD,
                        caption, line1, "Prijavi se", "Izlaz");
        }
        else
        {
                // Pokazi REGISTER dijalog
                new caption[128], line1[512];
                format(caption, sizeof(caption), "{0088FF}UNICATE GAMING OGC{FFFFFF} - Registracija");
                format(line1, sizeof(line1),
                        "{FFFFFF}Dobrodosao, {0088FF}%s{FFFFFF}!\n\n\
                        Registruj se da bi zapoceo svoju avanturu.\n\n\
                        {AAAAAA}Unesi lozinku (najmanje 6 karaktera).\n\
                        {AAAAAA}Ova lozinka ce ti biti potrebna svaki put kad udjes na server.", playerName);
                ShowPlayerDialog(playerid, DIALOG_PORTAL_REGISTER, DIALOG_STYLE_PASSWORD,
                        caption, line1, "Registruj se", "Izlaz");
        }
}

// ===== Callback: Igrac kliknuo IGRAJ SADA u CEF =====
forward OnCefPortalRequestAuth(playerid);
public OnCefPortalRequestAuth(playerid)
{
        printf("[CEF-PORTAL] Player %d clicked IGRAJ SADA - showing SA-MP dialog", playerid);

        // Pokazi SA-MP dijalog (CEF ostaje u pozadini, zamagljen)
        CefPortal_ShowDialog(playerid);
        return 1;
}

// ===== Obradi SA-MP dijalog odgovor =====
stock CefPortal_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
        if(dialogid == DIALOG_PORTAL_LOGIN)
        {
                if(!response)
                {
                        // Igrac kliknuo Izlaz - vratite portal u normalno stanje
                        CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:dialogDone", CEF_INT(0));
                        return 1;
                }

                // Validiraj lozinku
                ManualReadAccount(playerid);

                if(isnull(inputtext) || strcmp(inputtext, PlayerInfo[playerid][xLozinka], false) != 0)
                {
                        printf("[CEF-PORTAL] Wrong password for player %d", playerid);

                        // Ponovo pokazi login dijalog sa greskom
                        new playerName[MAX_PLAYER_NAME];
                        GetPlayerName(playerid, playerName, sizeof(playerName));
                        new caption[128], line1[256];
                        format(caption, sizeof(caption), "{0088FF}UNICATE GAMING OGC{FFFFFF} - Prijava");
                        format(line1, sizeof(line1),
                                "{FF4444}Pogresna lozinka! Pokusaj ponovo.{FFFFFF}\n\n\
                                Dobrodosao natrag, {0088FF}%s{FFFFFF}!\n\n\
                                Unesi svoju lozinku da bi nastavio.", playerName);
                        ShowPlayerDialog(playerid, DIALOG_PORTAL_LOGIN, DIALOG_STYLE_PASSWORD,
                                caption, line1, "Prijavi se", "Izlaz");
                        return 1;
                }

                printf("[CEF-PORTAL] Correct password for player %d - spawning", playerid);

                // Obavijesti CEF da je gotovo - zatvori portal
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:dialogDone", CEF_INT(1));

                // Sacekaj kratko pa zatvori browser i spawnaj
                SetTimerEx("CefPortalLoginDelay", 800, false, "d", playerid);
                return 1;
        }

        if(dialogid == DIALOG_PORTAL_REGISTER)
        {
                if(!response)
                {
                        // Igrac kliknuo Izlaz - vratite portal u normalno stanje
                        CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:dialogDone", CEF_INT(0));
                        return 1;
                }

                // Validacija lozinke
                if(isnull(inputtext) || strlen(inputtext) < 6)
                {
                        new playerName[MAX_PLAYER_NAME];
                        GetPlayerName(playerid, playerName, sizeof(playerName));
                        new caption[128], line1[512];
                        format(caption, sizeof(caption), "{0088FF}UNICATE GAMING OGC{FFFFFF} - Registracija");
                        format(line1, sizeof(line1),
                                "{FF4444}Lozinka mora imati najmanje 6 karaktera!{FFFFFF}\n\n\
                                Dobrodosao, {0088FF}%s{FFFFFF}!\n\n\
                                Registruj se da bi zapoceo svoju avanturu.\n\n\
                                {AAAAAA}Unesi lozinku (najmanje 6 karaktera).", playerName);
                        ShowPlayerDialog(playerid, DIALOG_PORTAL_REGISTER, DIALOG_STYLE_PASSWORD,
                                caption, line1, "Registruj se", "Izlaz");
                        return 1;
                }

                // Sacuvaj lozinku i registruj
                strmid(PlayerInfo[playerid][xLozinka], inputtext, 0, strlen(inputtext), 50);
                PlayerInfo[playerid][xRegistrovan] = 1;

                printf("[CEF-PORTAL] Registration complete for player %d - spawning", playerid);

                // Obavijesti CEF da je gotovo
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:dialogDone", CEF_INT(1));

                // Sacekaj kratko pa zatvori browser i spawnaj
                SetTimerEx("CefPortalRegDelay", 800, false, "d", playerid);
                return 1;
        }

        return 0; // Nije nas dijalog
}
