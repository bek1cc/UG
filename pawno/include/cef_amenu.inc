// ===========================================================================
//  CEF ADMIN MENU (/amenu) za open.mp (omp-cef plugin)
//  File: pawno/include/cef_amenu.inc
//
//  Moderan CEF admin panel za upravljanje inventoryem igraca
//  Komanda: /amenu (otvara panel odma, bez ID parametra)
// ===========================================================================

#if defined _cef_amenu_included
    #endinput
#endif
#define _cef_amenu_included

#define CEF_AMENU_RESOURCE   "amenu"
#define CEF_AMENU_WIDTH      800.0
#define CEF_AMENU_HEIGHT     600.0
#define CEF_AMENU_BROWSER_ID 3

enum CefAmenuData {
    bool:am_IsOpen,
    am_BrowserId,
    am_TargetID
}

static CefAmenu[MAX_PLAYERS][CefAmenuData];

// ===========================================================================
//  OTVARANJE ADMIN MENU (CEF)
// ===========================================================================
stock CEF_AmenuOpen(playerid, targetid) {
    if(CefAmenu[playerid][am_IsOpen]) return;
    if(!IsPlayerConnected(playerid)) return;
    if(!IsPlayerConnected(targetid)) return;

    if(!CEF_PlayerHasPlugin(playerid)) {
        SendClientMessage(playerid, 0xF59E0BFF, "[AMENU]{FFFFFF} CEF plugin nije detektovan! Admin menu zahtijeva CEF plugin. Proverite da li vam je cef.asi instaliran u GTA folder.");
        return;
    }

    CefAmenu[playerid][am_BrowserId] = CEF_AMENU_BROWSER_ID;
    CefAmenu[playerid][am_TargetID] = targetid;
    CEF_CreateBrowser(playerid, CefAmenu[playerid][am_BrowserId], "http://cef/amenu/index.html", true, true, CEF_AMENU_WIDTH, CEF_AMENU_HEIGHT);

    CefAmenu[playerid][am_IsOpen] = true;
    TogglePlayerControllable(playerid, false);
}

// ===========================================================================
//  ZATVARANJE ADMIN MENU
// ===========================================================================
stock CEF_AmenuClose(playerid) {
    if(!CefAmenu[playerid][am_IsOpen]) return;

    CEF_DestroyBrowser(playerid, CefAmenu[playerid][am_BrowserId]);
    CefAmenu[playerid][am_IsOpen] = false;
    CefAmenu[playerid][am_TargetID] = -1;
    TogglePlayerControllable(playerid, true);
}

// ===========================================================================
//  SLANJE INIT PODATAKA U CEF
// ===========================================================================
stock CEF_AmenuSendInit(playerid) {
    if(!CefAmenu[playerid][am_IsOpen]) return;

    new targetid = CefAmenu[playerid][am_TargetID];
    if(!IsPlayerConnected(targetid)) return;

    new targetName[MAX_PLAYER_NAME];
    GetPlayerName(targetid, targetName, sizeof(targetName));

    CEF_EmitEvent(playerid, CefAmenu[playerid][am_BrowserId], "amenu:init",
        CEF_STR(targetName),
        CEF_INT(GetPlayerScore(targetid)),
        CEF_INT(GetPlayerMoney(targetid))
    );
}

// ===========================================================================
//  SLANJE TOAST NOTIFIKACIJE
// ===========================================================================
stock CEF_AmenuToast(playerid, message[], type[] = "success") {
    if(!CefAmenu[playerid][am_IsOpen]) return;
    CEF_EmitEvent(playerid, CefAmenu[playerid][am_BrowserId], "amenu:toast",
        CEF_STR(message),
        CEF_STR(type)
    );
}

// ===========================================================================
//  CEF INIT - Registruj evente i resurse
// ===========================================================================
stock CEF_AmenuInitSubscriptions() {
    CEF_AddResource(CEF_AMENU_RESOURCE);

    CEF_RegisterEvent("amenu:close", "OnAmenuClose");
    CEF_RegisterEvent("amenu:addItem", "OnAmenuAddItem", Argument_String, Argument_Integer);
    CEF_RegisterEvent("amenu:clearInventory", "OnAmenuClearInventory");
    CEF_RegisterEvent("amenu:setPin", "OnAmenuSetPin", Argument_String);
    CEF_RegisterEvent("amenu:deletePin", "OnAmenuDeletePin");
    CEF_RegisterEvent("amenu:checkPin", "OnAmenuCheckPin");
    CEF_RegisterEvent("amenu:switchTarget", "OnAmenuSwitchTarget", Argument_Integer);
    CEF_RegisterEvent("amenu:requestPlayers", "OnAmenuRequestPlayers");

    print("[CEF AMENU] Subscriptions registrovane.");
}

// ===========================================================================
//  CEF EVENT CALLBACKS
// ===========================================================================
forward OnAmenuClose(playerid);
public OnAmenuClose(playerid) {
    CEF_AmenuClose(playerid);
}

forward OnAmenuAddItem(playerid, itemName[], quantity);
public OnAmenuAddItem(playerid, itemName[], quantity) {
    if(!CefAmenu[playerid][am_IsOpen]) return;
    new targetid = CefAmenu[playerid][am_TargetID];
    if(!IsPlayerConnected(targetid)) {
        CEF_AmenuToast(playerid, "Igrac vise nije online!", "error");
        return;
    }
    if(quantity < 1 || quantity > 99999) {
        CEF_AmenuToast(playerid, "Kolicina mora biti izmedju 1 i 99999!", "error");
        return;
    }

    // ===== FOOD & DRINK =====
    if(!strcmp(itemName, "Pizza")) { PlayerInfo[targetid][xPaketic] += quantity; }
    else if(!strcmp(itemName, "Hamburger")) { PlayerInfo[targetid][xPaketic] += quantity; }
    else if(!strcmp(itemName, "Chicken")) { PlayerInfo[targetid][xPaketic] += quantity; }
    else if(!strcmp(itemName, "Juice")) { PlayerInfo[targetid][xSodaB] += quantity; }
    else if(!strcmp(itemName, "Beer")) { PlayerInfo[targetid][xSodaB] += quantity; }
    else if(!strcmp(itemName, "Campfire")) { PlayerInfo[targetid][xKampKucica] += quantity; }
    else if(!strcmp(itemName, "Water")) { PlayerInfo[targetid][xDesVoda] += quantity; }
    else if(!strcmp(itemName, "Fruit")) { PlayerInfo[targetid][xPaketic] += quantity; }

    // ===== TOOLS =====
    else if(!strcmp(itemName, "Screwdriver")) { PlayerInfo[targetid][xTurpija] += quantity; }
    else if(!strcmp(itemName, "Crowbar")) { PlayerInfo[targetid][xTurpija] += quantity; }
    else if(!strcmp(itemName, "Hammer")) { PlayerInfo[targetid][xTurpija] += quantity; }
    else if(!strcmp(itemName, "FuelCan")) { PlayerInfo[targetid][xPaketic] += quantity; }
    else if(!strcmp(itemName, "Lockpick")) { PlayerInfo[targetid][xKljucKapije] += quantity; }
    else if(!strcmp(itemName, "Rope")) { PlayerInfo[targetid][xPaketic] += quantity; }
    else if(!strcmp(itemName, "Toolkit")) { PlayerInfo[targetid][xToolkit] += quantity; }
    else if(!strcmp(itemName, "Boombox")) { PlayerInfo[targetid][xBoombox] += quantity; }
    else if(!strcmp(itemName, "Bag")) { PlayerInfo[targetid][xTasna] += quantity; }

    // ===== MATERIALS =====
    else if(!strcmp(itemName, "Metal")) { PlayerInfo[targetid][xMaterijali] += quantity; }
    else if(!strcmp(itemName, "Scrap")) { PlayerInfo[targetid][xMaterijali] += quantity; }
    else if(!strcmp(itemName, "Iron")) { PlayerInfo[targetid][xMaterijali] += quantity; }
    else if(!strcmp(itemName, "Plastic")) { PlayerInfo[targetid][xIGResursi] += quantity; }
    else if(!strcmp(itemName, "Stone")) { PlayerInfo[targetid][xMaterijali] += quantity; }
    else if(!strcmp(itemName, "Wood")) { PlayerInfo[targetid][xMaterijali] += quantity; }
    else if(!strcmp(itemName, "Wrench")) { PlayerInfo[targetid][xMDelovi] += quantity; }

    // ===== DRUGS =====
    else if(!strcmp(itemName, "Marijuana")) { PlayerInfo[targetid][xMarihuana] += quantity; }
    else if(!strcmp(itemName, "Cocaine")) { PlayerInfo[targetid][xKokain] += quantity; }
    else if(!strcmp(itemName, "Crack")) { PlayerInfo[targetid][xCrveniF] += quantity; }
    else if(!strcmp(itemName, "LSD")) { PlayerInfo[targetid][xLysAcid] += quantity; }
    else if(!strcmp(itemName, "Opium")) { PlayerInfo[targetid][xOpium] += quantity; }
    else if(!strcmp(itemName, "SolnaKiselina")) { PlayerInfo[targetid][xSolna] += quantity; }
    else if(!strcmp(itemName, "Pseudoefedrin")) { PlayerInfo[targetid][xPseudo] += quantity; }
    else if(!strcmp(itemName, "CrveniFosfor")) { PlayerInfo[targetid][xCrveniF] += quantity; }
    else if(!strcmp(itemName, "Kalcijum")) { PlayerInfo[targetid][xKalcij] += quantity; }
    else if(!strcmp(itemName, "AmonijumHlorid")) { PlayerInfo[targetid][xAmonijH] += quantity; }
    else if(!strcmp(itemName, "Methylamina")) { PlayerInfo[targetid][xMetilAmin] += quantity; }

    // ===== SPECIAL =====
    else if(!strcmp(itemName, "Mask")) { PlayerInfo[targetid][xPaketic] += quantity; }
    else if(!strcmp(itemName, "Laptop")) { PlayerInfo[targetid][xPaketic] += quantity; }
    else if(!strcmp(itemName, "Cigarette")) { PlayerInfo[targetid][xCigare] += quantity; }
    else if(!strcmp(itemName, "Lighter")) { PlayerInfo[targetid][xUpaljac] += quantity; }
    else if(!strcmp(itemName, "Helmet")) { PlayerInfo[targetid][xPaketic] += quantity; }
    else if(!strcmp(itemName, "Watch")) { PlayerInfo[targetid][xSat] += quantity; }
    else if(!strcmp(itemName, "Injection")) { PlayerInfo[targetid][xInekcija] += quantity; }
    else if(!strcmp(itemName, "Dynamite")) { PlayerInfo[targetid][xPaketic] += quantity; }
    else if(!strcmp(itemName, "Jewelry")) { PlayerInfo[targetid][xNakit] += quantity; }

    // ===== WEAPONS =====
    else if(!strcmp(itemName, "BrassKnuckles")) { PlayerInfo[targetid][xPaketic] += quantity; }

    else {
        CEF_AmenuToast(playerid, "Nepoznat item!", "error");
        return;
    }

    new msg[128];
    format(msg, sizeof(msg), "Dodao si %dx %s %s.", quantity, itemName, ImeIgraca(targetid));
    CEF_AmenuToast(playerid, msg, "success");
}

forward OnAmenuClearInventory(playerid);
public OnAmenuClearInventory(playerid) {
    if(!CefAmenu[playerid][am_IsOpen]) return;
    new targetid = CefAmenu[playerid][am_TargetID];
    if(!IsPlayerConnected(targetid)) {
        CEF_AmenuToast(playerid, "Igrac vise nije online!", "error");
        return;
    }

    // Reset all inventory items to 0
    PlayerInfo[targetid][xPaketic] = 0;
    PlayerInfo[targetid][xSodaB] = 0;
    PlayerInfo[targetid][xKampKucica] = 0;
    PlayerInfo[targetid][xDesVoda] = 0;
    PlayerInfo[targetid][xTurpija] = 0;
    PlayerInfo[targetid][xKljucKapije] = 0;
    PlayerInfo[targetid][xToolkit] = 0;
    PlayerInfo[targetid][xBoombox] = 0;
    PlayerInfo[targetid][xTasna] = 0;
    PlayerInfo[targetid][xMaterijali] = 0;
    PlayerInfo[targetid][xIGResursi] = 0;
    PlayerInfo[targetid][xMDelovi] = 0;
    PlayerInfo[targetid][xMarihuana] = 0;
    PlayerInfo[targetid][xKokain] = 0;
    PlayerInfo[targetid][xCrveniF] = 0;
    PlayerInfo[targetid][xLysAcid] = 0;
    PlayerInfo[targetid][xOpium] = 0;
    PlayerInfo[targetid][xSolna] = 0;
    PlayerInfo[targetid][xPseudo] = 0;
    PlayerInfo[targetid][xKalcij] = 0;
    PlayerInfo[targetid][xAmonijH] = 0;
    PlayerInfo[targetid][xMetilAmin] = 0;
    PlayerInfo[targetid][xCigare] = 0;
    PlayerInfo[targetid][xUpaljac] = 0;
    PlayerInfo[targetid][xSat] = 0;
    PlayerInfo[targetid][xInekcija] = 0;
    PlayerInfo[targetid][xNakit] = 0;
    PlayerInfo[targetid][xZlato] = 0;

    CEF_AmenuToast(playerid, "Ociscen cio inventory!", "success");
}

forward OnAmenuSetPin(playerid, pin[]);
public OnAmenuSetPin(playerid, pin[]) {
    if(!CefAmenu[playerid][am_IsOpen]) return;
    new targetid = CefAmenu[playerid][am_TargetID];
    if(!IsPlayerConnected(targetid)) {
        CEF_AmenuToast(playerid, "Igrac vise nije online!", "error");
        return;
    }

    new pinLen = strlen(pin);
    if(pinLen != 4) {
        CEF_AmenuToast(playerid, "PIN mora imati 4 cifre!", "error");
        return;
    }

    for(new i = 0; i < pinLen; i++) {
        if(pin[i] < '0' || pin[i] > '9') {
            CEF_AmenuToast(playerid, "PIN smije sadrzavati samo brojeve!", "error");
            return;
        }
    }

    new pinNum = strval(pin);
    Bounty_SavePIN(targetid, pinNum);

    new msg[128];
    format(msg, sizeof(msg), "Postavio si tablet PIN za %s na %04d", ImeIgraca(targetid), pinNum);
    CEF_AmenuToast(playerid, msg, "success");
}

forward OnAmenuDeletePin(playerid);
public OnAmenuDeletePin(playerid) {
    if(!CefAmenu[playerid][am_IsOpen]) return;
    new targetid = CefAmenu[playerid][am_TargetID];
    if(!IsPlayerConnected(targetid)) {
        CEF_AmenuToast(playerid, "Igrac vise nije online!", "error");
        return;
    }

    new name[MAX_PLAYER_NAME], file[64];
    GetPlayerName(targetid, name, sizeof(name));
    format(file, sizeof(file), "Tablet/%s.ini", name);
    if(dini_Exists(file)) {
        dini_Remove(file);
        CEF_AmenuToast(playerid, "Obrisan tablet PIN!", "success");
    } else {
        CEF_AmenuToast(playerid, "Taj igrac nema tablet PIN.", "error");
    }
}

forward OnAmenuCheckPin(playerid);
public OnAmenuCheckPin(playerid) {
    if(!CefAmenu[playerid][am_IsOpen]) return;
    new targetid = CefAmenu[playerid][am_TargetID];
    if(!IsPlayerConnected(targetid)) {
        CEF_AmenuToast(playerid, "Igrac vise nije online!", "error");
        return;
    }

    if(Bounty_HasPIN(targetid)) {
        new pin = Bounty_LoadPIN(targetid);
        new msg[64];
        format(msg, sizeof(msg), "Tablet PIN za %s je: %04d", ImeIgraca(targetid), pin);
        CEF_AmenuToast(playerid, msg, "success");
    } else {
        CEF_AmenuToast(playerid, "Taj igrac nema tablet PIN.", "error");
    }
}

// ===========================================================================
//  SLANJE LISTE ONLINE IGRACA U CEF
// ===========================================================================
stock CEF_AmenuSendPlayerList(playerid) {
    if(!CefAmenu[playerid][am_IsOpen]) return;

    new listJson[2048];
    listJson = "[";
    new first = 1;
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(!IsPlayerConnected(i)) continue;
        new pName[MAX_PLAYER_NAME];
        GetPlayerName(i, pName, sizeof(pName));
        if(!first) strcat(listJson, ",");
        new entry[64];
        format(entry, sizeof(entry), "{\"id\":%d,\"name\":\"%s\"}", i, pName);
        strcat(listJson, entry);
        first = 0;
    }
    strcat(listJson, "]");

    CEF_EmitEvent(playerid, CefAmenu[playerid][am_BrowserId], "amenu:playerList",
        CEF_STR(listJson)
    );
}

// ===========================================================================
//  SWITCH TARGET - Promjena target igraca iz CEF panela
// ===========================================================================
forward OnAmenuSwitchTarget(playerid, newTargetId);
public OnAmenuSwitchTarget(playerid, newTargetId) {
    if(!CefAmenu[playerid][am_IsOpen]) return;
    if(!IsPlayerConnected(newTargetId)) {
        CEF_AmenuToast(playerid, "Taj igrac nije na serveru!", "error");
        return;
    }

    CefAmenu[playerid][am_TargetID] = newTargetId;
    CEF_AmenuSendInit(playerid);
    CEF_AmenuToast(playerid, "Prebaceno na drugog igraca!", "success");
}

// ===========================================================================
//  REQUEST PLAYERS - Admin trazi listu online igraca
// ===========================================================================
forward OnAmenuRequestPlayers(playerid);
public OnAmenuRequestPlayers(playerid) {
    CEF_AmenuSendPlayerList(playerid);
}

// ===========================================================================
//  CEF OnCefBrowserCreated
// ===========================================================================
stock CefAmenu_OnBrowserCreated(playerid, browserid) {
    if(browserid == CEF_AMENU_BROWSER_ID && CefAmenu[playerid][am_IsOpen]) {
        CEF_FocusBrowser(playerid, browserid, true);
    }
}

// ===========================================================================
//  CEF OnCefReady - posalji init podatke
// ===========================================================================
stock CefAmenu_OnCefReady(playerid) {
    if(CefAmenu[playerid][am_IsOpen]) {
        CEF_AmenuSendInit(playerid);
        CEF_AmenuSendPlayerList(playerid);
    }
}

// ===========================================================================
//  OnGameModeInit - registruj CEF resurse
// ===========================================================================
stock CefAmenu_OnGameModeInit() {
    CEF_AmenuInitSubscriptions();
}

// ===========================================================================
//  OnPlayerDisconnect - zatvori panel
// ===========================================================================
stock CefAmenu_OnPlayerDisconnect(playerid) {
    if(CefAmenu[playerid][am_IsOpen]) {
        CefAmenu[playerid][am_IsOpen] = false;
        CefAmenu[playerid][am_TargetID] = -1;
    }
}
