#if defined _INC_cef_portal
        #endinput
#endif
#define _INC_cef_portal

#define CEF_PORTAL_BROWSER_ID   60
#define CEF_PORTAL_URL          "http://cef/portal/index.html"

static bool:PortalOpen[MAX_PLAYERS];

// ==========================================================================
// SAMOSTALNO CITANJE LOZINKE IZ INI FAJLA
// Ne zavisi od ManualReadAccount, ImeIgraca, I_FILE niti ikoje druge funkcije
// Ovo je 100% nezavisno - cita DIREKTNO iz fajla
// ==========================================================================
stock PortalReadAccount(playerid, out_password[], out_password_len, &out_registered)
{
        out_password[0] = '\0';
        out_registered = 0;

        new pName[MAX_PLAYER_NAME];
        GetPlayerName(playerid, pName, sizeof(pName));

        new filepath[128];
        format(filepath, sizeof(filepath), "Korisnici/%s.ini", pName);

        printf("[PORTAL-READ] Reading account for '%s' from '%s'", pName, filepath);

        if(!fexist(filepath))
        {
                printf("[PORTAL-READ] File does NOT exist: %s", filepath);
                return 0;
        }

        new File:f = fopen(filepath, io_read);
        if(!f)
        {
                printf("[PORTAL-READ] Cannot open file: %s", filepath);
                return 0;
        }

        new line[256], key[64], val[128], eqPos, idx;
        new bool:foundLozinka = false;

        while(fread(f, line))
        {
                // Strip newline/carriage return
                idx = strlen(line);
                while(idx > 0 && (line[idx-1] == '\n' || line[idx-1] == '\r'))
                        line[--idx] = '\0';

                // Skip empty lines, section headers, comments
                if(!line[0] || line[0] == '[' || line[0] == ';' || line[0] == '#') continue;

                // Find '=' separator
                eqPos = -1;
                for(idx = 0; line[idx]; idx++)
                {
                        if(line[idx] == '=') { eqPos = idx; break; }
                }
                if(eqPos == -1) continue;

                // Extract and trim key
                strmid(key, line, 0, eqPos);
                idx = strlen(key);
                while(idx > 0 && key[idx-1] == ' ') key[--idx] = '\0';

                // Extract and trim value (leading spaces)
                strmid(val, line, eqPos + 1, strlen(line));
                idx = 0;
                while(val[idx] == ' ') idx++;
                if(idx > 0) {
                        new j = 0;
                        while((val[j] = val[idx + j])) j++;
                }
                // Trim trailing spaces
                idx = strlen(val);
                while(idx > 0 && val[idx-1] == ' ') val[--idx] = '\0';

                // Parse ONLY Lozinka and Registrovan
                if(strcmp(key, "Lozinka") == 0) {
                        strmid(out_password, val, 0, strlen(val), out_password_len);
                        foundLozinka = true;
                        printf("[PORTAL-READ] Found Lozinka = '%s' (len=%d)", out_password, strlen(out_password));
                }
                else if(strcmp(key, "Registrovan") == 0) {
                        out_registered = strval(val);
                        printf("[PORTAL-READ] Found Registrovan = %d", out_registered);
                }
        }

        fclose(f);

        if(!foundLozinka && out_registered == 0) {
                printf("[PORTAL-READ] No Lozinka or Registrovan found in file - new player");
        }

        return 1;
}

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

                // CITAJ DIREKTNO IZ FAJLA - ne zavisi od PlayerInfo
                new storedPassword[50];
                new isRegistered;
                PortalReadAccount(playerid, storedPassword, sizeof(storedPassword), isRegistered);

                printf("[CEF-PORTAL] Player %s: isRegistered=%d, storedPassLen=%d",
                        playerName, isRegistered, strlen(storedPassword));

                // Posalji ime i da li je registrovan - BEZ lozinke!
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:init",
                        CEF_STR(playerName),
                        CEF_INT(isRegistered));

                // Focus the portal so player can type
                CEF_FocusBrowser(playerid, CEF_PORTAL_BROWSER_ID, true);
        }
}

stock CefPortal_InitSubscriptions()
{
        CEF_RegisterEvent("portal:loginAttempt", "OnCefPortalLogin", Argument_String);
        CEF_RegisterEvent("portal:registerAttempt", "OnCefPortalRegister",
                Argument_String, Argument_String, Argument_Integer, Argument_Integer, Argument_Integer);
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

// ==========================================================================
// LOGIN PREKO PORTALA
// ==========================================================================
forward OnCefPortalLogin(playerid, const password[]);
public OnCefPortalLogin(playerid, const password[])
{
        printf("[CEF-PORTAL] === LOGIN ATTEMPT === player %d, typed password len: %d", playerid, strlen(password));

        // CITAJ LOZINKU DIREKTNO IZ FAJLA - potpuno nezavisno od PlayerInfo
        new storedPassword[50];
        new isRegistered;
        PortalReadAccount(playerid, storedPassword, sizeof(storedPassword), isRegistered);

        printf("[CEF-PORTAL] File data: isRegistered=%d, storedPass='%s' (len=%d)",
                isRegistered, storedPassword, strlen(storedPassword));
        printf("[CEF-PORTAL] Player typed: '%s' (len=%d)", password, strlen(password));

        // Provjera da li je uopste registrovan
        if(!isRegistered && strlen(storedPassword) == 0)
        {
                printf("[CEF-PORTAL] Player %d is NOT registered - cannot login", playerid);
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:loginResponse",
                        CEF_BOOL(false), CEF_STR("Nisi registrovan! Registruj se prvo."));
                return 1;
        }

        // Provjera prazne lozinke
        if(isnull(password))
        {
                printf("[CEF-PORTAL] Empty password from player %d", playerid);
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:loginResponse",
                        CEF_BOOL(false), CEF_STR("Lozinka ne moze biti prazna!"));
                return 1;
        }

        // POREDJENJE LOZINKE - direktno iz fajla
        if(strcmp(password, storedPassword, false) != 0)
        {
                printf("[CEF-PORTAL] WRONG PASSWORD for player %d", playerid);
                CEF_EmitEvent(playerid, CEF_PORTAL_BROWSER_ID, "portal:loginResponse",
                        CEF_BOOL(false), CEF_STR("Pogresna lozinka! Pokusaj ponovo."));
                return 1;
        }

        printf("[CEF-PORTAL] CORRECT PASSWORD for player %d - spawning", playerid);

        // Ucitaj account podatke u PlayerInfo (za ostale sisteme)
        ManualReadAccount(playerid);

        // Zatvori portal
        CEF_PortalClose(playerid);

        // Spawnaj igraca
        CefPortalLoginSpawn(playerid);

        return 1;
}

// ==========================================================================
// REGISTER PREKO PORTALA
// ==========================================================================
forward OnCefPortalRegister(playerid, const password[], const email[], godine, drzava, spol);
public OnCefPortalRegister(playerid, const password[], const email[], godine, drzava, spol)
{
        printf("[CEF-PORTAL] === REGISTER ATTEMPT === player %d, pass len=%d, email=%s, godine=%d, drzava=%d, spol=%d",
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

        printf("[CEF-PORTAL] Registration valid for player %d - saving, spawning", playerid);

        // Sacuvaj podatke u PlayerInfo
        strmid(PlayerInfo[playerid][xLozinka], password, 0, strlen(password), 50);
        strmid(PlayerInfo[playerid][xEmail], email, 0, strlen(email), 34);
        PlayerInfo[playerid][xGodine] = godine;
        PlayerInfo[playerid][xDrzava] = drzava;
        PlayerInfo[playerid][xSpol] = spol;
        PlayerInfo[playerid][xRegistrovan] = 1;

        // Zatvori portal
        CEF_PortalClose(playerid);

        // Spawnaj igraca
        CefPortalRegisterSpawn(playerid);

        return 1;
}

// ==========================================================================
// SPAWN FUNKCIJE - deklarisane u fg-ogc.pwn
// ==========================================================================
// forward CefPortalLoginSpawn(playerid);
// forward CefPortalRegisterSpawn(playerid);
