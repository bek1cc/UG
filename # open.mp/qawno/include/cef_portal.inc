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
                // Portal sam prikazuje login/register na osnovu ovoga
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:init",
                        CEF_STR(playerName),
                        CEF_INT(isRegistered));

                CEF_FocusBrowser(playerid, CEF_PORTAL_BROWSER_ID, true);
        }
}

stock CefPortal_InitSubscriptions()
{
        // Login event - string argument (lozinka)
        CEF_RegisterEvent("portal:requestLogin", "OnCefPortalLogin", Argument_String);

        // Register event - string, string, int, int, int (lozinka, email, godine, drzava, spol)
        CEF_RegisterEvent("portal:requestRegister", "OnCefPortalRegister",
                Argument_String, Argument_String, Argument_Integer, Argument_Integer, Argument_Integer);
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

// ===== Callback funkcije za CEF evente =====

// Igrac je kliknuo PRIJAVI SE u CEF portalu sa lozinkom
forward OnCefPortalLogin(playerid, const password[]);
public OnCefPortalLogin(playerid, const password[])
{
        printf("[CEF-PORTAL] Login attempt from player %d, password len=%d", playerid, strlen(password));

        // Ponovo ucitaj account iz fajla
        ManualReadAccount(playerid);

        // Validiraj lozinku
        if(isnull(password) || strcmp(password, PlayerInfo[playerid][xLozinka], false) != 0)
        {
                printf("[CEF-PORTAL] Wrong password for player %d", playerid);

                // Obavijesti portal da je lozinka pogresna
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:loginResponse",
                        CEF_BOOL(false), CEF_STR("Pogresna lozinka! Pokusaj ponovo."));
                return 1;
        }

        printf("[CEF-PORTAL] Correct password for player %d - spawning", playerid);

        // Obavijesti portal da je login uspjesan
        CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:loginResponse",
                CEF_BOOL(true), CEF_STR("Prijava uspjesna!"));

        // Sacekaj kratko da CEF prikaze poruku, onda zatvori i spawnaj
        SetTimerEx("CefPortalLoginDelay", 1200, false, "d", playerid);
        return 1;
}

// Igrac je kliknuo REGISTRIRAJ SE u CEF portalu sa svim podacima
forward OnCefPortalRegister(playerid, const password[], const email[], age, country, gender);
public OnCefPortalRegister(playerid, const password[], const email[], age, country, gender)
{
        printf("[CEF-PORTAL] Register attempt from player %d, passLen=%d, emailLen=%d, age=%d, country=%d, gender=%d",
                playerid, strlen(password), strlen(email), age, country, gender);

        // Validacija
        if(strlen(password) < 6)
        {
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:registerResponse",
                        CEF_BOOL(false), CEF_STR("Lozinka mora imati najmanje 6 karaktera!"));
                return 1;
        }

        if(strlen(email) < 5)
        {
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:registerResponse",
                        CEF_BOOL(false), CEF_STR("Unesi ispravnu email adresu!"));
                return 1;
        }

        if(age < 12 || age > 70)
        {
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:registerResponse",
                        CEF_BOOL(false), CEF_STR("Godine moraju biti izmedju 12 i 70!"));
                return 1;
        }

        // Sacuvaj podatke
        strmid(PlayerInfo[playerid][xLozinka], password, 0, strlen(password), 50);
        strmid(PlayerInfo[playerid][xEmail], email, 0, strlen(email), 34);
        PlayerInfo[playerid][xGodine] = age;
        PlayerInfo[playerid][xDrzava] = country;
        PlayerInfo[playerid][xSpol] = gender;
        PlayerInfo[playerid][xRegistrovan] = 1;

        printf("[CEF-PORTAL] Registration complete for player %d - spawning", playerid);

        // Obavijesti portal da je registracija uspjesna
        CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:registerResponse",
                CEF_BOOL(true), CEF_STR("Registracija uspjesna!"));

        // Sacekaj kratko da CEF prikaze poruku, onda zatvori i spawnaj
        SetTimerEx("CefPortalRegDelay", 1200, false, "d", playerid);
        return 1;
}
