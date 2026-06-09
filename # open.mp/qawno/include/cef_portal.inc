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
                printf("[CEF-PORTAL] Portal browser created successfully for player %d", playerid);

                // Send player info to portal JS - playerName, isRegistered, storedPassword
                new playerName[MAX_PLAYER_NAME];
                GetPlayerName(playerid, playerName, sizeof(playerName));

                new isRegistered = PlayerInfo[playerid][xRegistrovan];

                // Posalji ime, da li je registrovan, i lozinku (za login validaciju)
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:init",
                        CEF_STR(playerName),
                        CEF_INT(isRegistered),
                        CEF_STR(PlayerInfo[playerid][xLozinka]));

                // Focus the portal so player can type in login fields
                CEF_FocusBrowser(playerid, CEF_PORTAL_BROWSER_ID, true);
        }
}

stock CefPortal_InitSubscriptions()
{
        // Login: samo lozinka
        CEF_RegisterEvent("portal:loginAttempt", "OnCefPortalLogin", Argument_String);
        // Register: lozinka, email, godine, drzava, spol
        CEF_RegisterEvent("portal:registerAttempt", "OnCefPortalRegister",
                Argument_String, Argument_String, Argument_Integer, Argument_Integer, Argument_Integer);
}

stock CefPortal_OnGameModeInit()
{
        // Register portal resource so CEF can find it
        CEF_AddResource("portal");
        CefPortal_InitSubscriptions();
}

stock CefPortal_OnPlayerDisconnect(playerid)
{
        PortalOpen[playerid] = false;
}

// ==========================================================================
// LOGIN PREKO PORTALA - igrac unio samo lozinku
// ==========================================================================
forward OnCefPortalLogin(playerid, const password[]);
public OnCefPortalLogin(playerid, const password[])
{
        printf("[CEF-PORTAL] Login attempt from player %d, password length: %d", playerid, strlen(password));

        // Validiraj lozinku
        if(isnull(password) || strcmp(password, PlayerInfo[playerid][xLozinka], false) != 0)
        {
                printf("[CEF-PORTAL] Wrong password for player %d", playerid);
                // Posalji gresku nazad u portal JS
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:loginResponse",
                        CEF_BOOL(false), CEF_STR("Pogresna lozinka! Pokusaj ponovo."));
                return 1;
        }

        printf("[CEF-PORTAL] Correct password for player %d - closing portal, spawning", playerid);

        // Zatvori portal
        CEF_PortalClose(playerid);

        // Pozovi portal login spawn funkciju
        CefPortalLoginSpawn(playerid);

        return 1;
}

// ==========================================================================
// REGISTER PREKO PORTALA - igrac unio lozinka, email, godine, drzava, spol
// ==========================================================================
forward OnCefPortalRegister(playerid, const password[], const email[], godine, drzava, spol);
public OnCefPortalRegister(playerid, const password[], const email[], godine, drzava, spol)
{
        printf("[CEF-PORTAL] Register attempt from player %d, pass len=%d, email=%s, godine=%d, drzava=%d, spol=%d",
                playerid, strlen(password), email, godine, drzava, spol);

        // Validacija
        if(strlen(password) < 6)
        {
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:registerResponse",
                        CEF_BOOL(false), CEF_STR("Lozinka mora imati najmanje 6 karaktera!"));
                return 1;
        }
        if(godine < 12 || godine > 70)
        {
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:registerResponse",
                        CEF_BOOL(false), CEF_STR("Godine moraju biti izmedju 12 i 70!"));
                return 1;
        }
        if(drzava < 1 || drzava > 6)
        {
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:registerResponse",
                        CEF_BOOL(false), CEF_STR("Izaberite drzavu!"));
                return 1;
        }
        if(spol < 1 || spol > 2)
        {
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:registerResponse",
                        CEF_BOOL(false), CEF_STR("Izaberite spol!"));
                return 1;
        }

        printf("[CEF-PORTAL] Registration valid for player %d - saving data, closing portal", playerid);

        // Sacuvaj podatke
        strmid(PlayerInfo[playerid][xLozinka], password, 0, strlen(password), 50);
        strmid(PlayerInfo[playerid][xEmail], email, 0, strlen(email), 34);
        PlayerInfo[playerid][xGodine] = godine;
        PlayerInfo[playerid][xDrzava] = drzava;
        PlayerInfo[playerid][xSpol] = spol;
        PlayerInfo[playerid][xRegistrovan] = 1;

        // Zatvori portal
        CEF_PortalClose(playerid);

        // Pozovi portal register spawn funkciju
        CefPortalRegisterSpawn(playerid);

        return 1;
}

// ==========================================================================
// SPAWN FUNKCIJE - direktno spawnanje nakon portala
// ==========================================================================

// Login spawn - koristi se nakon sto je lozinka validirana u portalu
// Deklarisano u fg-ogc.pwn
// forward CefPortalLoginSpawn(playerid);

// Register spawn - koristi se nakon sto su svi podaci sacuvani iz portala
// Deklarisano u fg-ogc.pwn
// forward CefPortalRegisterSpawn(playerid);
