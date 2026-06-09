// ===========================================================================
//  CEF PHONE INTEGRACIJA za open.mp (omp-cef plugin)
//  File: pawno/include/cef_phone.inc
//
//  Komande:
//  /telefon    - otvori/zatvori CEF telefon
//  /phone      - isto
//
//  Integracija:
//    #define USE_CEF
//    #include <cef_phone>
// ===========================================================================

#if defined _cef_phone_included
    #endinput
#endif
#define _cef_phone_included

#define CEF_PHONE_RESOURCE    "phone"
#define CEF_PHONE_WIDTH       380.0
#define CEF_PHONE_HEIGHT      680.0
#define CEF_PHONE_BROWSER_ID  4

enum PhonePlayerData {
    bool:ph_IsOpen,
    ph_BrowserId,
    bool:ph_OnCall,
    ph_CallTarget[MAX_PLAYER_NAME],
    ph_CallNumber
}

static PhoneData[MAX_PLAYERS][PhonePlayerData];

#if defined USE_CEF

// Omp-CEF native funkcije
#if !defined _INC_cef
    #include <cef>
#endif

// ===========================================================================
//  OTVARANJE TELEFONA (OMP-CEF)
// ===========================================================================
stock CEF_PhoneOpen(playerid) {
    if(PhoneData[playerid][ph_IsOpen]) return;
    if(!IsPlayerConnected(playerid)) return;

    // Ako igrac nema CEF plugin, koristi textdraw telefon kao fallback
    if(!CEF_PlayerHasPlugin(playerid)) {
        #if defined _iphone_included
            iPhone_Open(playerid);
            SendClientMessage(playerid, 0xF59E0BFF, "[TELEFON]{FFFFFF} CEF plugin nije detektovan - koristite textdraw telefon.");
        #else
            SendClientMessage(playerid, 0xFF453AFF, "[TELEFON]{FFFFFF} CEF plugin nije detektovan! Proverite da li vam je cef.asi instaliran u GTA folder.");
        #endif
        return;
    }

    PhoneData[playerid][ph_BrowserId] = CEF_PHONE_BROWSER_ID;
    CEF_CreateBrowser(playerid, PhoneData[playerid][ph_BrowserId], "http://cef/phone/index.html", true, false, CEF_PHONE_WIDTH, CEF_PHONE_HEIGHT);

    PhoneData[playerid][ph_IsOpen] = true;
    PhoneData[playerid][ph_OnCall] = false;

    // Ne freeze igraca - telefon je u kutu, igrac moze hodati
    SendClientMessage(playerid, 0x54A0FFFF, "[TELEFON]{FFFFFF} CEF Telefon otvoren. Koristi /telefon ili Y za zatvaranje.");
}

// ===========================================================================
//  ZATVARANJE TELEFONA
// ===========================================================================
stock CEF_PhoneClose(playerid) {
    if(!PhoneData[playerid][ph_IsOpen]) return;
    CEF_DestroyBrowser(playerid, PhoneData[playerid][ph_BrowserId]);
    PhoneData[playerid][ph_IsOpen] = false;
    PhoneData[playerid][ph_OnCall] = false;
    TogglePlayerControllable(playerid, true);
}

// ===========================================================================
//  INIT DATA
// ===========================================================================
stock CEF_PhoneSendInit(playerid) {
    if(!PhoneData[playerid][ph_IsOpen]) return;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, sizeof(playerName));

    CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:init",
        CEF_STR(playerName),
        CEF_INT(iPhone_GetNumber(playerid)),
        CEF_INT(GetPlayerMoney(playerid)),
        CEF_INT(Bounty_CalcBattery(playerid))
    );

    // Provjeri da li igrac ima PIN i obavijesti CEF
    // PhonePin se cuva u PhonePins/ folderu (odvojeno od Korisnici/) da YINI ne brise PIN
    new pinFile[64];
    format(pinFile, sizeof(pinFile), "PhonePins/%s.ini", playerName);
    if(dini_Exists(pinFile)) {
        new savedPin[16];
        format(savedPin, sizeof(savedPin), "%s", dini_Get(pinFile, "PhonePin"));
        if(strlen(savedPin) < 4) {
            // Nema PIN - novi igrac, treba registracija
            CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:pinRegister");
        }
    } else {
        // Nema fajl - sigurno novi igrac
        CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:pinRegister");
    }
}

// ===========================================================================
//  SUBSCRIPTIONS (OMP-CEF)
// ===========================================================================
stock CEF_PhoneInitSubscriptions() {
    CEF_AddResource(CEF_PHONE_RESOURCE);

    CEF_RegisterEvent("phone:close", "OnPhoneClose");
    CEF_RegisterEvent("phone:call", "OnPhoneCall", Argument_String);
    CEF_RegisterEvent("phone:sms", "OnPhoneSMS", Argument_String);
    CEF_RegisterEvent("phone:call911", "OnPhoneCall911");
    CEF_RegisterEvent("phone:callTaxi", "OnPhoneCallTaxi");
    CEF_RegisterEvent("phone:callMehanicar", "OnPhoneCallMehanicar");
    CEF_RegisterEvent("phone:bounty:place", "OnPhoneBountyPlace", Argument_String);
    CEF_RegisterEvent("phone:takePhoto", "OnPhoneTakePhoto");
    CEF_RegisterEvent("phone:requestLocation", "OnPhoneRequestLocation");
    CEF_RegisterEvent("phone:appInstalled", "OnPhoneAppInstalled", Argument_String);
    CEF_RegisterEvent("phone:instalife:post", "OnPhoneInstalifePost", Argument_String);
    CEF_RegisterEvent("phone:pin", "OnPhonePin", Argument_String);
    print("[CEF PHONE] Subscriptions registrovane (omp-cef API).");
}

// ===========================================================================
//  EVENT CALLBACKS
// ===========================================================================
forward OnPhoneClose(playerid);
public OnPhoneClose(playerid) {
    CEF_PhoneClose(playerid);
    SendClientMessage(playerid, 0x54A0FFFF, "[TELEFON]{FFFFFF} Telefon zatvoren.");
}

forward OnPhoneCall(playerid, number[]);
public OnPhoneCall(playerid, number[]) {
    if(!PhoneData[playerid][ph_IsOpen]) return;
    new msg[64];
    format(msg, sizeof(msg), "Pozivam %s...", number);
    CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:toast", CEF_STR(msg));
    PhoneData[playerid][ph_OnCall] = true;
    format(PhoneData[playerid][ph_CallTarget], MAX_PLAYER_NAME, "%s", number);
}

forward OnPhoneSMS(playerid, data[]);
public OnPhoneSMS(playerid, data[]) {
    if(!PhoneData[playerid][ph_IsOpen]) return;
    CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:toast", CEF_STR("SMS poslan!"));
}

forward OnPhoneCall911(playerid);
public OnPhoneCall911(playerid) {
    CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:toast", CEF_STR("Pozivam 911..."));
    // TODO: Implementirati 911 sistem
}

forward OnPhoneCallTaxi(playerid);
public OnPhoneCallTaxi(playerid) {
    CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:toast", CEF_STR("Pozivam taxi..."));
    // TODO: Implementirati taxi poziv
}

forward OnPhoneCallMehanicar(playerid);
public OnPhoneCallMehanicar(playerid) {
    CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:toast", CEF_STR("Pozivam mehanicara..."));
    // TODO: Implementirati mehanicar poziv
}

forward OnPhoneBountyPlace(playerid, data[]);
public OnPhoneBountyPlace(playerid, data[]) {
    if(!PhoneData[playerid][ph_IsOpen]) return;
    #if defined _cef_tablet_included
        OnTabletBountyPlace(playerid, data);
    #endif
}

// ===========================================================================
//  PIN SYSTEM - Registracija i login preko telefona
// ===========================================================================
forward OnPhonePin(playerid, pin[]);
public OnPhonePin(playerid, pin[]) {
    if(!PhoneData[playerid][ph_IsOpen]) return;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, sizeof(playerName));
    new pinFile[64];
    format(pinFile, sizeof(pinFile), "PhonePins/%s.ini", playerName);

    // Provjeri da li igrac ima sacuvan PIN
    if(dini_Exists(pinFile)) {
        new savedPin[16];
        format(savedPin, sizeof(savedPin), "%s", dini_Get(pinFile, "PhonePin"));

        if(strlen(savedPin) < 4) {
            // Nema PIN - ovo je registracija novog PIN-a
            dini_Set(pinFile, "PhonePin", pin);
            CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:pinResult",
                CEF_INT(1), CEF_INT(1));
            SendClientMessage(playerid, 0x22c55eFF, "[TELEFON]{FFFFFF} PIN uspjesno postavljen!");
            printf("[CEF PHONE] Igrac %s registrio PIN.", playerName);
        } else {
            // Ima PIN - provjeri da li je tacan
            if(strcmp(pin, savedPin) == 0) {
                CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:pinResult",
                    CEF_INT(1), CEF_INT(0));
                SendClientMessage(playerid, 0x22c55eFF, "[TELEFON]{FFFFFF} Uspjesna prijava!");
                printf("[CEF PHONE] Igrac %s se prijavio na telefon.", playerName);
            } else {
                CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:pinResult",
                    CEF_INT(0), CEF_INT(0));
                SendClientMessage(playerid, 0xef4444FF, "[TELEFON]{FFFFFF} Pogresan PIN!");
            }
        }
    } else {
        // Igrac nema PIN fajl - registruj novi PIN
        dini_Create(pinFile);
        dini_Set(pinFile, "PhonePin", pin);
        CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:pinResult",
            CEF_INT(1), CEF_INT(1));
        SendClientMessage(playerid, 0x22c55eFF, "[TELEFON]{FFFFFF} PIN uspjesno postavljen!");
        printf("[CEF PHONE] Igrac %s registrio PIN (novi fajl).", playerName);
    }
}

// ===========================================================================
//  CAMERA - Simulirana fotografija sa game state podatcima
// ===========================================================================
forward OnPhoneTakePhoto(playerid);
public OnPhoneTakePhoto(playerid) {
    if(!PhoneData[playerid][ph_IsOpen]) return;

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    // Lokacija
    new zone[64];
    GetZoneName(x, y, zone, sizeof(zone));

    // Obližnji igrači
    new nearby[128] = "";
    new count = 0;
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(i == playerid || !IsPlayerConnected(i)) continue;
        new Float:px, Float:py, Float:pz;
        GetPlayerPos(i, px, py, pz);
        if(floatabs(x-px) < 30.0 && floatabs(y-py) < 30.0 && floatabs(z-pz) < 30.0) {
            if(count > 0) strcat(nearby, ", ");
            new name[MAX_PLAYER_NAME];
            GetPlayerName(i, name, sizeof(name));
            strcat(nearby, name);
            count++;
            if(count >= 5) break; // Max 5 obližnjih
        }
    }
    if(count == 0) strcat(nearby, "Niko u blizini");

    // Skin i vozilo
    new skinStr[16];
    format(skinStr, sizeof(skinStr), "Skin %d", GetPlayerSkin(playerid));

    new vehicleStr[32] = "Pješak";
    if(IsPlayerInAnyVehicle(playerid)) {
        new vid = GetPlayerVehicleID(playerid);
        format(vehicleStr, sizeof(vehicleStr), "Vozilo %d", GetVehicleModel(vid));
    }

    // Vrijeme
    new timeStr[16];
    new h, m, s;
    gettime(h, m, s);
    format(timeStr, sizeof(timeStr), "%02d:%02d", h, m);

    // Šalji podatke u CEF
    CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:photoData",
        CEF_STR(zone),
        CEF_STR(nearby),
        CEF_STR(skinStr),
        CEF_STR(vehicleStr),
        CEF_STR(timeStr)
    );
}

forward OnPhoneRequestLocation(playerid);
public OnPhoneRequestLocation(playerid) {
    if(!PhoneData[playerid][ph_IsOpen]) return;
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    new zone[64];
    GetZoneName(x, y, zone, sizeof(zone));
    CEF_EmitEvent(playerid, PhoneData[playerid][ph_BrowserId], "phone:photoData",
        CEF_STR(zone), CEF_STR(""), CEF_STR(""), CEF_STR(""), CEF_STR("")
    );
}

// ===========================================================================
//  APP STORE - Čuva instalirane aplikacije
// ===========================================================================
forward OnPhoneAppInstalled(playerid, appId[]);
public OnPhoneAppInstalled(playerid, appId[]) {
    if(!PhoneData[playerid][ph_IsOpen]) return;
    // TODO: Sačuvati instaliranu aplikaciju u igračev INI fajl
    // new file[64]; format(file, sizeof(file), "players/%s.ini", playerName);
    // INI_WriteString(file, "installedApps", allApps);
    printf("[CEF PHONE] Igrac %d instalirao app: %s", playerid, appId);
}

// ===========================================================================
//  INSTALIFE - Instagram-klon post sistem
// ===========================================================================
forward OnPhoneInstalifePost(playerid, data[]);
public OnPhoneInstalifePost(playerid, data[]) {
    if(!PhoneData[playerid][ph_IsOpen]) return;
    // TODO: Sačuvati post u bazu/fajl
    // Može se koristiti JSON parsing za caption, location itd.
    printf("[CEF PHONE] Instalife post od igraca %d: %s", playerid, data);
}

// ===========================================================================
//  OnCefReady / OnCefBrowserCreated za phone
// ===========================================================================
stock CefPhone_OnCefReady(playerid) {
    if(PhoneData[playerid][ph_IsOpen]) {
        CEF_PhoneSendInit(playerid);
    }
}

stock CefPhone_OnBrowserCreated(playerid, browserid) {
    if(browserid == CEF_PHONE_BROWSER_ID && PhoneData[playerid][ph_IsOpen]) {
        CEF_FocusBrowser(playerid, browserid, true);
    }
}

#else

// Fallback bez CEF
stock CEF_PhoneOpen(playerid) {
    iPhone_Open(playerid);
    SendClientMessage(playerid, 0x54A0FFFF, "[TELEFON]{FFFFFF} CEF plugin nije instaliran. Koristite textdraw telefon.");
}
stock CEF_PhoneClose(playerid) {
    iPhone_Close(playerid);
}
stock CEF_PhoneInitSubscriptions() {}

#endif // USE_CEF

// ===========================================================================
//  ZCMD KOMANDE
//  Napomena: CMD:phone je u gamemodu (prevazilazi iPhone textdraw)
// ===========================================================================
CMD:telefon(playerid, params[]) {
    #pragma unused params
    if(PhoneData[playerid][ph_IsOpen]) {
        CEF_PhoneClose(playerid);
        SendClientMessage(playerid, 0x54A0FFFF, "[TELEFON]{FFFFFF} Telefon zatvoren.");
    } else {
        CEF_PhoneOpen(playerid);
    }
    return 1;
}

// ===========================================================================
//  ON PLAYER DISCONNECT
// ===========================================================================
stock CEF_PhoneOnPlayerDisconnect(playerid) {
    if(PhoneData[playerid][ph_IsOpen]) {
        CEF_PhoneClose(playerid);
    }
    PhoneData[playerid][ph_OnCall] = false;
}

// ===========================================================================
//  HELPER
// ===========================================================================
stock CEF_IsPhoneOpen(playerid) {
    return PhoneData[playerid][ph_IsOpen];
}

// ===========================================================================
//  TOGGLE EXPAND (za /prosirenitelefon komandu)
// ===========================================================================
#if defined USE_CEF
stock CEF_PhoneToggleExpand(playerid) {
    if(!PhoneData[playerid][ph_IsOpen]) return 0;

    if(PhoneData[playerid][ph_BrowserId] == CEF_PHONE_BROWSER_ID) {
        // Currently normal size -> expand (bigger phone frame)
        CEF_DestroyBrowser(playerid, PhoneData[playerid][ph_BrowserId]);
        PhoneData[playerid][ph_BrowserId] = CEF_PHONE_BROWSER_ID + 1;
        CEF_CreateBrowser(playerid, PhoneData[playerid][ph_BrowserId], "http://cef/phone/index.html?expanded=1", true, true, 480.0, 860.0);
        CEF_FocusBrowser(playerid, PhoneData[playerid][ph_BrowserId], true);
        SendClientMessage(playerid, 0x54A0FFFF, "[TELEFON]{FFFFFF} Prošireni prikaz telefona.");
    } else {
        // Currently expanded -> back to normal
        CEF_DestroyBrowser(playerid, PhoneData[playerid][ph_BrowserId]);
        PhoneData[playerid][ph_BrowserId] = CEF_PHONE_BROWSER_ID;
        CEF_CreateBrowser(playerid, PhoneData[playerid][ph_BrowserId], "http://cef/phone/index.html", true, false, CEF_PHONE_WIDTH, CEF_PHONE_HEIGHT);
        CEF_FocusBrowser(playerid, PhoneData[playerid][ph_BrowserId], true);
        CEF_PhoneSendInit(playerid);
        SendClientMessage(playerid, 0x54A0FFFF, "[TELEFON]{FFFFFF} Normalni prikaz telefona.");
    }
    return 1;
}
#else
stock CEF_PhoneToggleExpand(playerid) {
    SendClientMessage(playerid, 0xFF453AFF, "[TELEFON]{FFFFFF} CEF plugin nije dostupan.");
    return 0;
}
#endif
