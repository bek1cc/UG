// ===========================================================================
//  BOUNTY BOARD SISTEM v3.0 za SA-MP - TABLET PORTAL UI
//  File: pawno/include/bounty.inc
//
//  Moderni TABLET PORTAL UI sa textdrawovima
//  - Login portal sa "UNICATE GAMING" brandingom
//  - Battery percentage, plave nijanse, 3D detalji
//  - Bounty board unutar tableta - naruci ubojstvo
//
//  Komande:
//  /bounty [id] [iznos]    - stavi nagradu na igrača
//  /bounties               - otvori tablet (portal)
//  /bountyme               - vidi da li imaš nagradu na sebi
//
//  Integracija: #include <bounty> u fg-ogc.pwn
// ===========================================================================

// ===========================================================================
//  KONFIGURACIJA
// ===========================================================================
#define BOUNTY_MAX              30
#define BOUNTY_MIN_AMOUNT       1000
#define BOUNTY_MAX_AMOUNT       500000
#define BOUNTY_COOLDOWN         300
#define BOUNTY_PAGE_SIZE        6
#define BOUNTY_DIALOG_BASE      2800
#define BNT_PIN_FILE            "Tablet/%s.ini"

#define BOUNTY_DLG_REGISTER     (BOUNTY_DIALOG_BASE + 1)
#define BOUNTY_DLG_REGISTER2    (BOUNTY_DIALOG_BASE + 2)
#define BOUNTY_DLG_LOGIN        (BOUNTY_DIALOG_BASE + 3)
#define BOUNTY_DLG_PLACE        (BOUNTY_DIALOG_BASE + 4)
#define BOUNTY_DLG_CONFIRM      (BOUNTY_DIALOG_BASE + 5)
#define BOUNTY_DLG_DETAIL       (BOUNTY_DIALOG_BASE + 6)

// ===========================================================================
//  BOJE (Plava Portal Paleta - Blue Shades)
// ===========================================================================
#define BNT_BG_OVERLAY          0x000000AA  // Tamni overlay iza tableta
#define BNT_BG_DARK             0x0A1628FF  // Najtamnija pozadina
#define BNT_BG_MAIN             0x0F2040FF  // Glavna pozadina tableta
#define BNT_BG_LIGHT            0x152D50FF  // Svjetlija pozadina
#define BNT_BG_ROW              0x1A3A6AFF  // Red pozadina
#define BNT_BG_ROW_ALT          0x122A4CFF  // Alternativni red
#define BNT_ACCENT              0x2E86DEFF  // Akcent plava
#define BNT_BRIGHT              0x54A0FFFF  // Sjajna plava
#define BNT_CYAN                0x00D2FFFF  // Cyan akcent
#define BNT_BORDER              0x2E86DE80  // Poluprovidna plava ivica
#define BNT_BORDER_SOLID        0x1E5FAAFF  // Solidna plava ivica
#define BNT_TEXT_WHITE          0xFFFFFFFF
#define BNT_TEXT_LIGHT          0x8CB4E0FF  // Svijetlo plava siva
#define BNT_TEXT_GRAY           0x5B7FA5FF  // Tamnija siva plava
#define BNT_TEXT_GOLD           0xFFD60AFF
#define BNT_TEXT_RED            0xFF453AFF
#define BNT_TEXT_GREEN          0x30D158FF
#define BNT_BTN_BLUE            0x2E86DECC  // Dugme plava
#define BNT_BTN_GREEN           0x30D158CC
#define BNT_BTN_RED             0xFF453ACC
#define BNT_INPUT_BG            0x0C1E36FF  // Input polje pozadina
#define BNT_BATTERY_BG          0x0A1628CC  // Baterija pozadina
#define BNT_BATTERY_FILL        0x30D158FF  // Baterija punjenje (zeleno)
#define BNT_BATTERY_LOW         0xFF453AFF  // Baterija nisko (crveno)
#define BNT_GLOW               0x54A0FF40  // Sjajni efekat
#define BNT_SHADOW              0x0A162880  // Sjenka

// ===========================================================================
//  SCREEN STATES
// ===========================================================================
enum BountyScreen {
    BNT_SCREEN_NONE,
    BNT_SCREEN_PORTAL,
    BNT_SCREEN_MAIN,
    BNT_SCREEN_BOUNTY
}

// ===========================================================================
//  PODACI
// ===========================================================================
enum BountyData {
    bool:bExists,
    bTargetId,
    bTargetName[MAX_PLAYER_NAME],
    bPlacerId,
    bPlacerName[MAX_PLAYER_NAME],
    bAmount,
    bTimestamp,
    bool:bAnonymous
}

new BountyList[BOUNTY_MAX][BountyData];
static BountyCount = 0;
new BountyCooldown[MAX_PLAYERS];
static BountyOnHead[MAX_PLAYERS];

// TD varijable - 53 textdrawova
enum BountyTDData {
    PlayerText:bnt_TD[53],
    bool:bnt_TDCreated,
    bool:bnt_IsOpen,
    BountyScreen:bnt_Screen,
    bnt_Page,
    bnt_TotalPages,
    bnt_Battery,
    bnt_LoginAttempts,
    bnt_PinCode,
    bool:bnt_PinEntered
}

static BountyPlayerTD[MAX_PLAYERS][BountyTDData];

// ===========================================================================
//  HELPER FUNKCIJE
// ===========================================================================
stock Bounty_GetTotalOnHead(playerid) return BountyOnHead[playerid];

stock Bounty_FormatTimeAgo(timestamp) {
    new diff = gettime() - timestamp;
    new str[32];
    if(diff < 60) format(str, sizeof(str), "upravo");
    else if(diff < 3600) format(str, sizeof(str), "%dm", diff / 60);
    else if(diff < 86400) format(str, sizeof(str), "%dh", diff / 3600);
    else format(str, sizeof(str), "%dd", diff / 86400);
    return str;
}

stock Bounty_FormatMoney(amount) {
    new str[16];
    if(amount >= 1000000) format(str, sizeof(str), "$%.1fM", Float:amount / 1000000.0);
    else if(amount >= 1000) format(str, sizeof(str), "$%dK", amount / 1000);
    else format(str, sizeof(str), "$%d", amount);
    return str;
}

stock Bounty_CountActive() {
    new count = 0;
    for(new i = 0; i < BOUNTY_MAX; i++) {
        if(BountyList[i][bExists]) count++;
    }
    return count;
}

// ===========================================================================
//  PIN / FILE SISTEM
// ===========================================================================
stock Bounty_HasPIN(playerid) {
    new name[MAX_PLAYER_NAME], file[64];
    GetPlayerName(playerid, name, sizeof(name));
    format(file, sizeof(file), BNT_PIN_FILE, name);
    return dini_Exists(file);
}

stock Bounty_LoadPIN(playerid) {
    new name[MAX_PLAYER_NAME], file[64];
    GetPlayerName(playerid, name, sizeof(name));
    format(file, sizeof(file), BNT_PIN_FILE, name);
    if(!dini_Exists(file)) return 0;
    return dini_Int(file, "PIN");
}

stock Bounty_SavePIN(playerid, pin) {
    new name[MAX_PLAYER_NAME], file[64];
    GetPlayerName(playerid, name, sizeof(name));
    format(file, sizeof(file), BNT_PIN_FILE, name);
    dini_IntSet(file, "PIN", pin);
    return 1;
}

// ===========================================================================
//  BATERIJA
// ===========================================================================
stock Bounty_CalcBattery(playerid) {
    // Baterija se prazni sa vremenom - 1% svakih 5 minuta
    new minutes = GetPlayerScore(playerid); // Koristimo score kao proxy za vrijeme
    new battery = 100 - (minutes % 100);
    if(battery < 5) battery = 5;
    if(battery > 100) battery = 100;
    BountyPlayerTD[playerid][bnt_Battery] = battery;
    return battery;
}

// ===========================================================================
//  DODAJ BOUNTY
// ===========================================================================
stock Bounty_Add(targetid, placerid, amount, bool:anonymous = false) {
    // Ako meta već ima bounty, dodaj na postojeći
    for(new i = 0; i < BOUNTY_MAX; i++) {
        if(BountyList[i][bExists] && BountyList[i][bTargetId] == targetid) {
            BountyList[i][bAmount] += amount;
            BountyOnHead[targetid] += amount;

            new msg[128];
            format(msg, sizeof(msg), "[BOUNTY]{FF453A} Nagrada na tvojoj glavi je povecana na {FFD60A}$%d{FF453A}!", BountyList[i][bAmount]);
            SendClientMessage(targetid, BNT_TEXT_RED, msg);

            format(msg, sizeof(msg), "[BOUNTY]{FFFFFF} Nagrada na glavi igraca %s je sada {FFD60A}$%d{FFFFFF}!", BountyList[i][bTargetName], BountyList[i][bAmount]);
            SendClientMessageToAll(BNT_TEXT_GOLD, msg);

            for(new p = 0; p < MAX_PLAYERS; p++) {
                if(BountyPlayerTD[p][bnt_IsOpen] && BountyPlayerTD[p][bnt_Screen] == BNT_SCREEN_BOUNTY)
                    Bounty_RefreshList(p);
            }
            return 1;
        }
    }

    // Pronadji prazan slot
    new slot = -1;
    for(new i = 0; i < BOUNTY_MAX; i++) {
        if(!BountyList[i][bExists]) { slot = i; break; }
    }

    if(slot == -1) {
        new oldestTime = 0x7FFFFFFF, oldestIdx = 0;
        for(new i = 0; i < BOUNTY_MAX; i++) {
            if(BountyList[i][bExists] && BountyList[i][bTimestamp] < oldestTime) {
                oldestTime = BountyList[i][bTimestamp];
                oldestIdx = i;
            }
        }
        BountyOnHead[BountyList[oldestIdx][bTargetId]] -= BountyList[oldestIdx][bAmount];
        if(BountyOnHead[BountyList[oldestIdx][bTargetId]] < 0)
            BountyOnHead[BountyList[oldestIdx][bTargetId]] = 0;
        BountyList[oldestIdx][bExists] = false;
        slot = oldestIdx;
    }

    BountyList[slot][bExists] = true;
    BountyList[slot][bTargetId] = targetid;
    GetPlayerName(targetid, BountyList[slot][bTargetName], MAX_PLAYER_NAME);
    BountyList[slot][bPlacerId] = placerid;
    if(placerid >= 0 && placerid < MAX_PLAYERS && IsPlayerConnected(placerid))
        GetPlayerName(placerid, BountyList[slot][bPlacerName], MAX_PLAYER_NAME);
    else
        format(BountyList[slot][bPlacerName], MAX_PLAYER_NAME, "Anoniman");
    BountyList[slot][bAmount] = amount;
    BountyList[slot][bTimestamp] = gettime();
    BountyList[slot][bAnonymous] = anonymous;
    BountyOnHead[targetid] += amount;
    BountyCount++;

    new msg[128];
    format(msg, sizeof(msg), "[BOUNTY]{FF453A} Neko je stavio nagradu od {FFD60A}$%d{FF453A} na tvoju glavu!", amount);
    SendClientMessage(targetid, BNT_TEXT_RED, msg);
    format(msg, sizeof(msg), "[BOUNTY]{FFFFFF} Nagrada od {FFD60A}$%d{FFFFFF} stavljena na glavu igraca %s!", amount, BountyList[slot][bTargetName]);
    SendClientMessageToAll(BNT_TEXT_GOLD, msg);
    PlayerPlaySound(targetid, 1057, 0.0, 0.0, 0.0);

    for(new p = 0; p < MAX_PLAYERS; p++) {
        if(BountyPlayerTD[p][bnt_IsOpen] && BountyPlayerTD[p][bnt_Screen] == BNT_SCREEN_BOUNTY)
            Bounty_RefreshList(p);
    }
    return 1;
}

// ===========================================================================
//  ON PLAYER DEATH
// ===========================================================================
stock Bounty_OnPlayerDeath(playerid, killerid) {
    if(killerid == INVALID_PLAYER_ID || killerid == playerid) return 0;
    if(BountyOnHead[playerid] <= 0) return 0;

    new totalReward = 0;
    for(new i = 0; i < BOUNTY_MAX; i++) {
        if(!BountyList[i][bExists]) continue;
        if(BountyList[i][bTargetId] != playerid) continue;
        totalReward += BountyList[i][bAmount];
        BountyList[i][bExists] = false;
    }

    if(totalReward > 0) {
        BountyOnHead[playerid] = 0;
        BountyCount--;
        GivePlayerMoney(killerid, totalReward);

        new killerName[MAX_PLAYER_NAME], targetName[MAX_PLAYER_NAME];
        GetPlayerName(killerid, killerName, sizeof(killerName));
        GetPlayerName(playerid, targetName, sizeof(targetName));

        new msg[128];
        format(msg, sizeof(msg), "[BOUNTY]{30D158} %s je pokupio nagradu od {FFD60A}$%d{30D158} za ubistvo %s!", killerName, totalReward, targetName);
        SendClientMessageToAll(BNT_TEXT_GREEN, msg);
        format(msg, sizeof(msg), "[BOUNTY]{30D158} Pokupio si nagradu od {FFD60A}$%d{30D158}!", totalReward);
        SendClientMessage(killerid, BNT_TEXT_GREEN, msg);

        PlayerPlaySound(killerid, 1058, 0.0, 0.0, 0.0);
        new gtMsg[64];
        format(gtMsg, sizeof(gtMsg), "~b~BOUNTY~n~~y~$%d", totalReward);
        GameTextForPlayer(killerid, gtMsg, 3000, 3);

        for(new p = 0; p < MAX_PLAYERS; p++) {
            if(BountyPlayerTD[p][bnt_IsOpen] && BountyPlayerTD[p][bnt_Screen] == BNT_SCREEN_BOUNTY)
                Bounty_RefreshList(p);
        }
        return 1;
    }
    return 0;
}

// ===========================================================================
//  TEXTDRAW KREIRANJE (svih 53 TD-ova)
// ===========================================================================
stock Bounty_CreateTDs(playerid) {
    if(BountyPlayerTD[playerid][bnt_TDCreated]) return;
    BountyPlayerTD[playerid][bnt_TDCreated] = true;

    new td = 0;

    // ======================================================================
    //  SHARED ELEMENTI (0-4) - uvijek vidljivi kad je tablet otvoren
    // ======================================================================

    // 0: Screen overlay (tamna pozadina iza tableta)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 0.0, 0.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 59.0);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 640.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BG_OVERLAY);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 1: Tablet body background (glavna plava pozadina)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 80.0, 20.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 49.5);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 560.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BG_MAIN);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 2: Tablet border/frame (sjajna plava ivica - 3D efekt)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 79.0, 19.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 50.0);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 561.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BORDER_SOLID);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 3: Top accent bar (plava traka na vrhu - 3D efekat)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 81.0, 21.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 0.8);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 559.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_ACCENT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 4: Bottom accent bar (plava traka na dnu)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 81.0, 437.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 0.5);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 559.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_ACCENT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // ======================================================================
    //  PORTAL / LOGIN SCREEN (5-18)
    // ======================================================================

    // 5: Battery bar background
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 490.0, 30.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 1.0);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 545.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BATTERY_BG);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 6: Battery bar fill (sirina ovisi o %)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 491.0, 31.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 0.6);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 544.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BATTERY_FILL);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 7: Battery percentage text "87%"
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 555.0, 28.0, "87%");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.2, 1.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_GREEN);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetOutline(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 8: Time display (vrijeme u status baru)
    new timeStr[8], h, m, s;
    gettime(h, m, s);
    format(timeStr, sizeof(timeStr), "%02d:%02d", h, m);
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 320.0, 28.0, timeStr);
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.25, 1.1);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 9: Signal indicator "UG 4G"
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 95.0, 28.0, "UG 4G");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.2, 1.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_LIGHT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 10: "UNICATE GAMING" naslov (veliki, sjajni efekat)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 320.0, 85.0, "UNICATE GAMING");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.55, 2.8);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BRIGHT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawSetOutline(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBackgroundColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_ACCENT);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 11: "SECURE PORTAL" podnaslov
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 320.0, 115.0, "SECURE PORTAL v2.0");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.22, 1.1);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_LIGHT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 12: Dekorativna linija ispod naslova
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 160.0, 138.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 0.2);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 480.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BORDER);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 13: "KORISNIK:" label
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 130.0, 165.0, "KORISNIK");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.2, 1.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_LIGHT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 14: Username input polje (prikazuje ime, klikabilno za informaciju)
    new pName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, pName, sizeof(pName));
    new nameDisplay[48];
    format(nameDisplay, sizeof(nameDisplay), "  %s", pName);
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 130.0, 180.0, nameDisplay);
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 3.2);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 510.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_INPUT_BG);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 15: "LOZINKA:" label
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 130.0, 220.0, "LOZINKA");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.2, 1.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_LIGHT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 16: Password input polje (maskirano, klikabilno)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 130.0, 235.0, "  ****");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 3.2);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 510.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_INPUT_BG);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 17: "PRIJAVI SE" dugme (plavo)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 130.0, 295.0, "PRIJAVI SE");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.28, 2.5);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 510.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BTN_BLUE);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 18: "REGISTRUJ SE" dugme (zeleno)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 130.0, 345.0, "REGISTRUJ SE");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.28, 2.5);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 510.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BTN_GREEN);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // ======================================================================
    //  MAIN TABLET HOME SCREEN (19-32)
    // ======================================================================

    // 19: Header bar background
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 81.0, 22.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 3.5);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 559.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BG_DARK);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 20: "UG TABLET" header text
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 320.0, 27.0, "UG TABLET");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.35, 1.8);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BRIGHT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 21: User welcome text
    new welcomeStr[64];
    format(welcomeStr, sizeof(welcomeStr), "Dobrodosao, %s", pName);
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 320.0, 75.0, welcomeStr);
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.3, 1.5);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 22: BOUNTY BOARD app icon background (crveno/plava - hit contract)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 130.0, 130.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 8.0);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 220.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0xC0392BFF);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 23: BOUNTY BOARD icon text "B"
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 175.0, 135.0, "B");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.6, 3.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 24: BOUNTY BOARD label
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 175.0, 210.0, "BOUNTY BOARD");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.2, 1.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_LIGHT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 25: PROFILE icon background
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 260.0, 130.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 8.0);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 350.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_ACCENT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 26: PROFILE icon text "P"
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 305.0, 135.0, "P");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.6, 3.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 27: PROFILE label
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 305.0, 210.0, "PROFIL");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.2, 1.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_LIGHT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 28: SETTINGS icon background
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 390.0, 130.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 8.0);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 480.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BG_LIGHT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 29: SETTINGS icon text "S"
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 435.0, 135.0, "S");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.6, 3.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 30: SETTINGS label
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 435.0, 210.0, "PODESAVANJA");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.2, 1.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_LIGHT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 31: "ODJAVI SE" logout dugme (crveno)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 200.0, 310.0, "ODJAVI SE");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.28, 2.5);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 440.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BTN_RED);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 32: Bottom info text
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 320.0, 380.0, "UNICATE GAMING - Secure Network");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.18, 0.9);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_GRAY);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // ======================================================================
    //  BOUNTY BOARD SCREEN (33-52)
    // ======================================================================

    // 33: Bounty header bar background
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 81.0, 22.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 3.5);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 559.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0xC0392BFF); // Crvena za hit
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 34: "BOUNTY BOARD" header text
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 320.0, 27.0, "BOUNTY BOARD");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.4, 2.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 35: Back button "< NATRAG"
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 95.0, 60.0, "< NATRAG");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.22, 1.2);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 185.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BTN_BLUE);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 36: Count display "Aktivno: 0/30"
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 540.0, 62.0, "0/30");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.22, 1.1);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_LIGHT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 37: Column headers
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 95.0, 85.0, "META                   NAGRADA      STATUS   PRIJE");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.17, 0.9);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_GRAY);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 38-43: Row backgrounds (6 redova, klikabilni)
    for(new row = 0; row < BOUNTY_PAGE_SIZE; row++) {
        new Float:rowY = 100.0 + (row * 48.0);
        BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 90.0, rowY, "_");
        PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 5.3);
        PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 550.0, 0.0);
        PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
        PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], (row % 2 == 0) ? BNT_BG_ROW : BNT_BG_ROW_ALT);
        PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
        PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        td++;
    }

    // 44-49: Row text (ime mete + iznos + status + vrijeme)
    for(new row = 0; row < BOUNTY_PAGE_SIZE; row++) {
        new Float:rowY = 103.0 + (row * 48.0);
        BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 100.0, rowY, " ");
        PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.22, 1.2);
        PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
        PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
        PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        td++;
    }

    // 50: "NARUCI UBOJSTVO" dugme (crveno, istaknuto)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 130.0, 395.0, "NARUCI UBOJSTVO");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.3, 2.5);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 510.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0xC0392BCC);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 51: Page info text
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 320.0, 425.0, "Strana 1/1");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.2, 1.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_GRAY);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 52: Empty state text
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, 320.0, 230.0, "NEMA AKTIVNIH NAGRADA~n~~b~Klikni 'Naruci ubojstvo' za narudzbu");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.28, 1.4);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_LIGHT);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;
}

// ===========================================================================
//  TEXTDRAW UNIŠTAVANJE
// ===========================================================================
stock Bounty_DestroyTDs(playerid) {
    if(!BountyPlayerTD[playerid][bnt_TDCreated]) return;
    for(new i = 0; i < 53; i++) {
        PlayerTextDrawDestroy(playerid, BountyPlayerTD[playerid][bnt_TD][i]);
    }
    BountyPlayerTD[playerid][bnt_TDCreated] = false;
}

// ===========================================================================
//  SCREEN MANAGEMENT - PRIKAZI / SAKRIJ EKRANE
// ===========================================================================

// Sakrij sve screen-specific TD-ove
stock Bounty_HideScreenTDs(playerid) {
    // Portal (5-18)
    for(new i = 5; i <= 18; i++) {
        PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][i]);
    }
    // Main (19-32)
    for(new i = 19; i <= 32; i++) {
        PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][i]);
    }
    // Bounty (33-52)
    for(new i = 33; i <= 52; i++) {
        PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][i]);
    }
}

// Prikaži shared elemente (0-4)
stock Bounty_ShowShared(playerid) {
    for(new i = 0; i <= 4; i++) {
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][i]);
    }
}

// PORTAL ekran (login/registracija)
stock Bounty_ShowPortal(playerid) {
    Bounty_HideScreenTDs(playerid);
    BountyPlayerTD[playerid][bnt_Screen] = BNT_SCREEN_PORTAL;

    // Update baterija
    new battery = Bounty_CalcBattery(playerid);
    new battStr[8];
    format(battStr, sizeof(battStr), "%d%%", battery);
    PlayerTextDrawSetString(playerid, BountyPlayerTD[playerid][bnt_TD][7], battStr);

    // Update battery fill sirina
    new Float:fillWidth = 52.0 * (Float:battery / 100.0);
    if(fillWidth < 2.0) fillWidth = 2.0;
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][6], 492.0 + fillWidth, 0.0);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][6],
        (battery <= 20) ? BNT_BATTERY_LOW : BNT_BATTERY_FILL);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][7],
        (battery <= 20) ? BNT_TEXT_RED : BNT_TEXT_GREEN);

    // Update time
    new timeStr[8], h, m, s;
    gettime(h, m, s);
    format(timeStr, sizeof(timeStr), "%02d:%02d", h, m);
    PlayerTextDrawSetString(playerid, BountyPlayerTD[playerid][bnt_TD][8], timeStr);

    // Prikazi portal elemente
    for(new i = 5; i <= 18; i++) {
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][i]);
    }

    // Ako ima PIN, sakrij "REGISTRUJ SE", prikazi "PRIJAVI SE"
    if(Bounty_HasPIN(playerid)) {
        PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][18]); // Reg dugme
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][17]); // Login dugme
        PlayerTextDrawSetString(playerid, BountyPlayerTD[playerid][bnt_TD][16], "  ****");
    } else {
        PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][17]); // Login dugme
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][18]); // Reg dugme
        PlayerTextDrawSetString(playerid, BountyPlayerTD[playerid][bnt_TD][16], "  Nije postavljen");
    }
}

// MAIN tablet home ekran
stock Bounty_ShowMain(playerid) {
    Bounty_HideScreenTDs(playerid);
    BountyPlayerTD[playerid][bnt_Screen] = BNT_SCREEN_MAIN;

    // Prikazi main elemente
    for(new i = 19; i <= 32; i++) {
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][i]);
    }
}

// BOUNTY BOARD ekran
stock Bounty_ShowBounty(playerid) {
    Bounty_HideScreenTDs(playerid);
    BountyPlayerTD[playerid][bnt_Screen] = BNT_SCREEN_BOUNTY;
    BountyPlayerTD[playerid][bnt_Page] = 0;

    // Prikazi bounty elemente
    for(new i = 33; i <= 52; i++) {
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][i]);
    }

    Bounty_RefreshList(playerid);
}

// ===========================================================================
//  OTVORI / ZATVORI UI
// ===========================================================================
stock Bounty_OpenUI(playerid) {
    if(BountyPlayerTD[playerid][bnt_IsOpen]) return;

    BountyPlayerTD[playerid][bnt_IsOpen] = true;
    BountyPlayerTD[playerid][bnt_PinEntered] = false;
    BountyPlayerTD[playerid][bnt_LoginAttempts] = 0;

    Bounty_CreateTDs(playerid);
    Bounty_ShowShared(playerid);
    Bounty_ShowPortal(playerid);

    SelectTextDraw(playerid, 0x2E86DEFF);
    PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
}

stock Bounty_CloseUI(playerid) {
    if(!BountyPlayerTD[playerid][bnt_IsOpen]) return;

    BountyPlayerTD[playerid][bnt_IsOpen] = false;
    BountyPlayerTD[playerid][bnt_Screen] = BNT_SCREEN_NONE;
    BountyPlayerTD[playerid][bnt_PinEntered] = false;

    // Sakrij sve
    for(new i = 0; i < 53; i++) {
        PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][i]);
    }
    Bounty_DestroyTDs(playerid);
    CancelSelectTextDraw(playerid);
}

// ===========================================================================
//  OSVJEŽI LISTU BOUNTYJA
// ===========================================================================
stock Bounty_RefreshList(playerid) {
    if(!BountyPlayerTD[playerid][bnt_IsOpen]) return;
    if(BountyPlayerTD[playerid][bnt_Screen] != BNT_SCREEN_BOUNTY) return;

    new activeCount = Bounty_CountActive();
    new totalPages = (activeCount + BOUNTY_PAGE_SIZE - 1) / BOUNTY_PAGE_SIZE;
    if(totalPages == 0) totalPages = 1;

    BountyPlayerTD[playerid][bnt_TotalPages] = totalPages;

    // Update count
    new countStr[16];
    format(countStr, sizeof(countStr), "%d/%d", activeCount, BOUNTY_MAX);
    PlayerTextDrawSetString(playerid, BountyPlayerTD[playerid][bnt_TD][36], countStr);

    // Update page info
    new pageStr[32];
    format(pageStr, sizeof(pageStr), "Strana %d/%d", BountyPlayerTD[playerid][bnt_Page] + 1, totalPages);
    PlayerTextDrawSetString(playerid, BountyPlayerTD[playerid][bnt_TD][51], pageStr);

    if(activeCount == 0) {
        // Pokaži empty state
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][52]);
        // Sakrij sve redove
        for(new row = 0; row < BOUNTY_PAGE_SIZE; row++) {
            PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][38 + row]); // row bg
            PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][44 + row]); // row text
        }
        return;
    }

    // Sakrij empty state
    PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][52]);

    // Popuni redove za trenutnu stranicu
    new startIdx = BountyPlayerTD[playerid][bnt_Page] * BOUNTY_PAGE_SIZE;
    new displayed = 0;
    new listIdx = 0;

    for(new i = 0; i < BOUNTY_MAX && displayed < BOUNTY_PAGE_SIZE; i++) {
        if(!BountyList[i][bExists]) continue;
        if(listIdx < startIdx) { listIdx++; continue; }

        new row = displayed;

        // Status
        new statusStr[16];
        if(IsPlayerConnected(BountyList[i][bTargetId]) && BountyList[i][bTargetId] != INVALID_PLAYER_ID)
            statusStr = "{30D158}ONLINE";
        else
            statusStr = "{FF453A}OFFLINE";

        // Kombinovani tekst za red
        new rowStr[128];
        format(rowStr, sizeof(rowStr), "{FFD60A}%s     {30D158}$%d     %s     {5B7FA5}%s",
            BountyList[i][bTargetName],
            BountyList[i][bAmount],
            statusStr,
            Bounty_FormatTimeAgo(BountyList[i][bTimestamp]));
        PlayerTextDrawSetString(playerid, BountyPlayerTD[playerid][bnt_TD][44 + row], rowStr);
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][44 + row]);

        // Pokaži red bg
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][38 + row]);

        displayed++;
        listIdx++;
    }

    // Sakrij prazne redove
    for(new row = displayed; row < BOUNTY_PAGE_SIZE; row++) {
        PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][38 + row]);
        PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][44 + row]);
    }
}

// ===========================================================================
//  CLICK HANDLER
// ===========================================================================
stock Bounty_OnClickTD(playerid, PlayerText:playertextid) {
    if(!BountyPlayerTD[playerid][bnt_IsOpen]) return 0;

    // ===== PORTAL SCREEN =====
    if(BountyPlayerTD[playerid][bnt_Screen] == BNT_SCREEN_PORTAL) {
        // 14: Username field (samo informacija)
        if(playertextid == BountyPlayerTD[playerid][bnt_TD][14]) {
            PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
            return 1;
        }

        // 16: Password field (otvara dialog za unos PIN-a)
        if(playertextid == BountyPlayerTD[playerid][bnt_TD][16]) {
            if(!Bounty_HasPIN(playerid)) {
                SendClientMessage(playerid, BNT_TEXT_RED, "[TABLET]{FFFFFF} Prvo se registrujte!");
                return 1;
            }
            ShowPlayerDialog(playerid, BOUNTY_DLG_LOGIN, DIALOG_STYLE_INPUT,
                "{54A0FF}UG Tablet{FFFFFF} - Prijava",
                "{FFFFFF}Unesite vas 4-cifreni PIN kod:",
                "Prijava", "Odustani");
            return 1;
        }

        // 17: "PRIJAVI SE" dugme
        if(playertextid == BountyPlayerTD[playerid][bnt_TD][17]) {
            if(!Bounty_HasPIN(playerid)) {
                SendClientMessage(playerid, BNT_TEXT_RED, "[TABLET]{FFFFFF} Prvo se registrujte!");
                return 1;
            }
            ShowPlayerDialog(playerid, BOUNTY_DLG_LOGIN, DIALOG_STYLE_INPUT,
                "{54A0FF}UG Tablet{FFFFFF} - Prijava",
                "{FFFFFF}Unesite vas 4-cifreni PIN kod:",
                "Prijava", "Odustani");
            return 1;
        }

        // 18: "REGISTRUJ SE" dugme
        if(playertextid == BountyPlayerTD[playerid][bnt_TD][18]) {
            if(Bounty_HasPIN(playerid)) {
                SendClientMessage(playerid, BNT_TEXT_RED, "[TABLET]{FFFFFF} Vec ste registrovani! Prijavite se.");
                return 1;
            }
            ShowPlayerDialog(playerid, BOUNTY_DLG_REGISTER, DIALOG_STYLE_INPUT,
                "{54A0FF}UG Tablet{FFFFFF} - Registracija",
                "{FFFFFF}Postavite vas 4-cifreni PIN kod:\n{8CB4E0}Koristicete ga za prijavu na tablet.",
                "Postavi", "Odustani");
            return 1;
        }

        return 1; // Ne dozvoli druge klikove na portalu
    }

    // ===== MAIN TABLET HOME =====
    if(BountyPlayerTD[playerid][bnt_Screen] == BNT_SCREEN_MAIN) {
        // 22: BOUNTY BOARD ikona
        if(playertextid == BountyPlayerTD[playerid][bnt_TD][22]) {
            PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
            Bounty_ShowBounty(playerid);
            return 1;
        }

        // 25: PROFILE ikona
        if(playertextid == BountyPlayerTD[playerid][bnt_TD][25]) {
            PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
            new msg[128], pName2[MAX_PLAYER_NAME];
            GetPlayerName(playerid, pName2, sizeof(pName2));
            format(msg, sizeof(msg), "[TABLET]{FFFFFF} Profil: %s | Novac: {30D158}$%d{FFFFFF} | Bounty na glavi: {FFD60A}$%d",
                pName2, GetPlayerMoney(playerid), BountyOnHead[playerid]);
            SendClientMessage(playerid, BNT_BRIGHT, msg);
            return 1;
        }

        // 28: SETTINGS ikona
        if(playertextid == BountyPlayerTD[playerid][bnt_TD][28]) {
            PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
            SendClientMessage(playerid, BNT_TEXT_LIGHT, "[TABLET]{FFFFFF} Podesavanja uskoro...");
            return 1;
        }

        // 31: "ODJAVI SE" dugme
        if(playertextid == BountyPlayerTD[playerid][bnt_TD][31]) {
            PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
            Bounty_CloseUI(playerid);
            SendClientMessage(playerid, BNT_TEXT_LIGHT, "[TABLET]{FFFFFF} Uspjesno ste se odjavili.");
            return 1;
        }

        return 1;
    }

    // ===== BOUNTY BOARD SCREEN =====
    if(BountyPlayerTD[playerid][bnt_Screen] == BNT_SCREEN_BOUNTY) {
        // 35: Back button "< NATRAG"
        if(playertextid == BountyPlayerTD[playerid][bnt_TD][35]) {
            PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
            Bounty_ShowMain(playerid);
            return 1;
        }

        // 50: "NARUCI UBOJSTVO" dugme
        if(playertextid == BountyPlayerTD[playerid][bnt_TD][50]) {
            PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
            Bounty_CloseUI(playerid);
            ShowPlayerDialog(playerid, BOUNTY_DLG_PLACE, DIALOG_STYLE_INPUT,
                "{C0392B}Bounty Board{FFFFFF} - Naruci ubojstvo",
                "{FFFFFF}Unesite: ID_Igraca Iznos\nPrimjer: 5 10000\n\n{8CB4E0}Min: $1,000 | Max: $500,000",
                "Naruci", "Odustani");
            return 1;
        }

        // 38-43: Klik na red (prikaži detalje)
        for(new row = 0; row < BOUNTY_PAGE_SIZE; row++) {
            if(playertextid == BountyPlayerTD[playerid][bnt_TD][38 + row]) {
                new startIdx = BountyPlayerTD[playerid][bnt_Page] * BOUNTY_PAGE_SIZE;
                new listIdx = 0;
                new foundSlot = -1;

                for(new i = 0; i < BOUNTY_MAX; i++) {
                    if(!BountyList[i][bExists]) continue;
                    if(listIdx == startIdx + row) { foundSlot = i; break; }
                    listIdx++;
                }

                if(foundSlot >= 0) {
                    new msg[256];
                    new placerDisplay[MAX_PLAYER_NAME + 12];
                    if(BountyList[foundSlot][bAnonymous]) {
                        format(placerDisplay, sizeof(placerDisplay), "{8CB4E0}???");
                    } else {
                        format(placerDisplay, sizeof(placerDisplay), "%s", BountyList[foundSlot][bPlacerName]);
                    }
                    format(msg, sizeof(msg),
                        "{FFFFFF}Meta: {FF453A}%s\n{FFFFFF}Nagrada: {FFD60A}$%d\n{FFFFFF}Postavio: %s\n{5B7FA5}Prije: %s",
                        BountyList[foundSlot][bTargetName],
                        BountyList[foundSlot][bAmount],
                        placerDisplay,
                        Bounty_FormatTimeAgo(BountyList[foundSlot][bTimestamp]));
                    ShowPlayerDialog(playerid, BOUNTY_DLG_DETAIL, DIALOG_STYLE_MSGBOX,
                        "{C0392B}Bounty Detalji", msg, "Zatvori", "");
                }
                return 1;
            }
        }

        return 1;
    }

    return 0;
}

// ESC handler (pozvati iz OnPlayerClickTextDraw)
stock Bounty_OnCancelSelect(playerid) {
    if(BountyPlayerTD[playerid][bnt_IsOpen]) {
        Bounty_CloseUI(playerid);
        return 1;
    }
    return 0;
}

// ===========================================================================
//  KOMANDE
// ===========================================================================
stock CMD_Bounty_Place(playerid, params[]) {
    new targetid, amount;

    if(sscanf(params, "ii", targetid, amount)) {
        SendClientMessage(playerid, BNT_TEXT_WHITE, "[BOUNTY]{FFFFFF} Koristi: {FFD60A}/bounty [ID] [Iznos]");
        SendClientMessage(playerid, BNT_TEXT_GRAY, "[BOUNTY] Min: $1,000 | Max: $500,000");
        return 1;
    }

    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid)) {
        SendClientMessage(playerid, BNT_TEXT_RED, "[BOUNTY]{FFFFFF} Igrac nije online!");
        return 1;
    }
    if(targetid == playerid) {
        SendClientMessage(playerid, BNT_TEXT_RED, "[BOUNTY]{FFFFFF} Ne mozes staviti nagradu na sebe!");
        return 1;
    }
    if(amount < BOUNTY_MIN_AMOUNT) {
        new msg[64]; format(msg, sizeof(msg), "[BOUNTY]{FFFFFF} Minimalni iznos je {FFD60A}$%d{FFFFFF}!", BOUNTY_MIN_AMOUNT);
        SendClientMessage(playerid, BNT_TEXT_RED, msg);
        return 1;
    }
    if(amount > BOUNTY_MAX_AMOUNT) {
        new msg[64]; format(msg, sizeof(msg), "[BOUNTY]{FFFFFF} Maksimalni iznos je {FFD60A}$%d{FFFFFF}!", BOUNTY_MAX_AMOUNT);
        SendClientMessage(playerid, BNT_TEXT_RED, msg);
        return 1;
    }
    if(GetPlayerMoney(playerid) < amount) {
        SendClientMessage(playerid, BNT_TEXT_RED, "[BOUNTY]{FFFFFF} Nemas dovoljno novca!");
        return 1;
    }
    if(BountyCooldown[playerid] > gettime()) {
        new remain = BountyCooldown[playerid] - gettime();
        new msg[64]; format(msg, sizeof(msg), "[BOUNTY]{FFFFFF} Sacekaj jos %d sekundi.", remain);
        SendClientMessage(playerid, 0xFF9F0AFF, msg);
        return 1;
    }

    new targetName[MAX_PLAYER_NAME];
    GetPlayerName(targetid, targetName, sizeof(targetName));

    new confirmStr[256];
    format(confirmStr, sizeof(confirmStr),
        "{FFFFFF}Da li zelite naruciti ubojstvo:\n\n{FF453A}%s (ID:%d)\n\n{FFFFFF}Iznos nagrade: {FFD60A}$%d\n\n{5B7FA5}Novac ce biti skinut sa racuna.",
        targetName, targetid, amount);

    SetPVarInt(playerid, "BountyTarget", targetid);
    SetPVarInt(playerid, "BountyAmount", amount);

    ShowPlayerDialog(playerid, BOUNTY_DLG_CONFIRM, DIALOG_STYLE_MSGBOX,
        "{C0392B}Bounty Board - Potvrda", confirmStr, "Naruci", "Odustani");
    return 1;
}

stock CMD_Bounty_List(playerid) {
    Bounty_OpenUI(playerid);
    return 1;
}

stock CMD_Bounty_Me(playerid) {
    if(BountyOnHead[playerid] <= 0) {
        SendClientMessage(playerid, BNT_TEXT_GREEN, "[BOUNTY]{FFFFFF} Nemas nikakvu nagradu na svojoj glavi!");
        return 1;
    }
    new msg[128];
    format(msg, sizeof(msg), "[BOUNTY]{FF453A} Imas nagradu od {FFD60A}$%d{FF453A} na svojoj glavi! Budi oprezan!", BountyOnHead[playerid]);
    SendClientMessage(playerid, BNT_TEXT_RED, msg);
    return 1;
}

// ===========================================================================
//  DIALOG HANDLER
// ===========================================================================
stock Bounty_OnDialogResponse(playerid, dialogid, response, listitem, const inputtext[]) {
    switch(dialogid) {
        // Registracija - unos PIN-a
        case BOUNTY_DLG_REGISTER: {
            if(!response) return 1;

            new pin;
            if(sscanf(inputtext, "i", pin)) {
                SendClientMessage(playerid, BNT_TEXT_RED, "[TABLET]{FFFFFF} Unesite 4-cifreni broj!");
                ShowPlayerDialog(playerid, BOUNTY_DLG_REGISTER, DIALOG_STYLE_INPUT,
                    "{54A0FF}UG Tablet{FFFFFF} - Registracija",
                    "{FFFFFF}Postavite vas 4-cifreni PIN kod:\n{FF453A}Greska: Unesite broj!",
                    "Postavi", "Odustani");
                return 1;
            }

            if(pin < 1000 || pin > 9999) {
                SendClientMessage(playerid, BNT_TEXT_RED, "[TABLET]{FFFFFF} PIN mora biti 4-cifreni broj (1000-9999)!");
                ShowPlayerDialog(playerid, BOUNTY_DLG_REGISTER, DIALOG_STYLE_INPUT,
                    "{54A0FF}UG Tablet{FFFFFF} - Registracija",
                    "{FFFFFF}Postavite vas 4-cifreni PIN kod:\n{FF453A}Greska: Mora biti 4 cifre!",
                    "Postavi", "Odustani");
                return 1;
            }

            SetPVarInt(playerid, "BountyRegPin", pin);
            ShowPlayerDialog(playerid, BOUNTY_DLG_REGISTER2, DIALOG_STYLE_INPUT,
                "{54A0FF}UG Tablet{FFFFFF} - Potvrda PIN-a",
                "{FFFFFF}Ponovo unesite vas PIN kod za potvrdu:",
                "Potvrdi", "Odustani");
            return 1;
        }

        // Registracija - potvrda PIN-a
        case BOUNTY_DLG_REGISTER2: {
            if(!response) {
                DeletePVar(playerid, "BountyRegPin");
                return 1;
            }

            new pin;
            if(sscanf(inputtext, "i", pin)) {
                ShowPlayerDialog(playerid, BOUNTY_DLG_REGISTER2, DIALOG_STYLE_INPUT,
                    "{54A0FF}UG Tablet{FFFFFF} - Potvrda PIN-a",
                    "{FFFFFF}Ponovo unesite vas PIN kod za potvrdu:\n{FF453A}Greska: Unesite broj!",
                    "Potvrdi", "Odustani");
                return 1;
            }

            new regPin = GetPVarInt(playerid, "BountyRegPin");
            DeletePVar(playerid, "BountyRegPin");

            if(pin != regPin) {
                SendClientMessage(playerid, BNT_TEXT_RED, "[TABLET]{FFFFFF} PIN kodovi se ne poklapaju! Pokusajte ponovo.");
                ShowPlayerDialog(playerid, BOUNTY_DLG_REGISTER, DIALOG_STYLE_INPUT,
                    "{54A0FF}UG Tablet{FFFFFF} - Registracija",
                    "{FFFFFF}Postavite vas 4-cifreni PIN kod:\n{FF453A}Greska: PIN se ne poklapa!",
                    "Postavi", "Odustani");
                return 1;
            }

            // Sacuvaj PIN
            Bounty_SavePIN(playerid, pin);
            BountyPlayerTD[playerid][bnt_PinCode] = pin;
            BountyPlayerTD[playerid][bnt_PinEntered] = true;

            SendClientMessage(playerid, BNT_TEXT_GREEN, "[TABLET]{FFFFFF} Uspjesno ste registrovani! PIN je sacuvan.");
            PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);

            // Automatski idi na main screen
            Bounty_ShowMain(playerid);
            return 1;
        }

        // Login - unos PIN-a
        case BOUNTY_DLG_LOGIN: {
            if(!response) return 1;

            new pin;
            if(sscanf(inputtext, "i", pin)) {
                SendClientMessage(playerid, BNT_TEXT_RED, "[TABLET]{FFFFFF} Unesite ispravan PIN!");
                return 1;
            }

            new savedPin = Bounty_LoadPIN(playerid);

            if(pin != savedPin) {
                BountyPlayerTD[playerid][bnt_LoginAttempts]++;
                new msg[64];
                format(msg, sizeof(msg), "[TABLET]{FF453A} Pogresan PIN! Pokusaj: %d/3", BountyPlayerTD[playerid][bnt_LoginAttempts]);
                SendClientMessage(playerid, BNT_TEXT_RED, msg);

                if(BountyPlayerTD[playerid][bnt_LoginAttempts] >= 3) {
                    SendClientMessage(playerid, BNT_TEXT_RED, "[TABLET]{FFFFFF} Previse pogresnih pokusaja! Tablet zakljucan.");
                    Bounty_CloseUI(playerid);
                    PlayerPlaySound(playerid, 1085, 0.0, 0.0, 0.0);
                    return 1;
                }

                ShowPlayerDialog(playerid, BOUNTY_DLG_LOGIN, DIALOG_STYLE_INPUT,
                    "{54A0FF}UG Tablet{FFFFFF} - Prijava",
                    "{FFFFFF}Unesite vas 4-cifreni PIN kod:\n{FF453A}Pogresan PIN!",
                    "Prijava", "Odustani");
                return 1;
            }

            // Uspjesna prijava
            BountyPlayerTD[playerid][bnt_PinEntered] = true;
            BountyPlayerTD[playerid][bnt_PinCode] = pin;
            BountyPlayerTD[playerid][bnt_LoginAttempts] = 0;

            SendClientMessage(playerid, BNT_TEXT_GREEN, "[TABLET]{FFFFFF} Uspjesno ste prijavljeni!");
            PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);

            // Idi na main screen
            Bounty_ShowMain(playerid);
            return 1;
        }

        // Place bounty input
        case BOUNTY_DLG_PLACE: {
            if(!response) { Bounty_OpenUI(playerid); return 1; }
            new targetid, amount;
            if(sscanf(inputtext, "ii", targetid, amount)) {
                SendClientMessage(playerid, BNT_TEXT_RED, "[BOUNTY]{FFFFFF} Format: ID Iznos (npr. 5 10000)");
                return 1;
            }
            new pStr[32];
            format(pStr, sizeof(pStr), "%d %d", targetid, amount);
            CMD_Bounty_Place(playerid, pStr);
            return 1;
        }

        // Confirm bounty
        case BOUNTY_DLG_CONFIRM: {
            if(!response) {
                DeletePVar(playerid, "BountyTarget");
                DeletePVar(playerid, "BountyAmount");
                return 1;
            }

            new targetid = GetPVarInt(playerid, "BountyTarget");
            new amount = GetPVarInt(playerid, "BountyAmount");
            DeletePVar(playerid, "BountyTarget");
            DeletePVar(playerid, "BountyAmount");

            if(!IsPlayerConnected(targetid)) {
                SendClientMessage(playerid, BNT_TEXT_RED, "[BOUNTY]{FFFFFF} Igrac vise nije online!");
                return 1;
            }
            if(GetPlayerMoney(playerid) < amount) {
                SendClientMessage(playerid, BNT_TEXT_RED, "[BOUNTY]{FFFFFF} Nemas vise dovoljno novca!");
                return 1;
            }

            GivePlayerMoney(playerid, -amount);
            Bounty_Add(targetid, playerid, amount, false);
            BountyCooldown[playerid] = gettime() + BOUNTY_COOLDOWN;

            new msg[64];
            format(msg, sizeof(msg), "[BOUNTY]{30D158} Narucio si ubojstvo za {FFD60A}$%d{30D158}!", amount);
            SendClientMessage(playerid, BNT_TEXT_GREEN, msg);
            return 1;
        }
    }
    return 0;
}

// ===========================================================================
//  CONNECT / DISCONNECT
// ===========================================================================
stock Bounty_OnPlayerConnect(playerid) {
    BountyCooldown[playerid] = 0;
    BountyOnHead[playerid] = 0;
    BountyPlayerTD[playerid][bnt_TDCreated] = false;
    BountyPlayerTD[playerid][bnt_IsOpen] = false;
    BountyPlayerTD[playerid][bnt_Screen] = BNT_SCREEN_NONE;
    BountyPlayerTD[playerid][bnt_Page] = 0;
    BountyPlayerTD[playerid][bnt_Battery] = 100;
    BountyPlayerTD[playerid][bnt_LoginAttempts] = 0;
    BountyPlayerTD[playerid][bnt_PinEntered] = false;
    BountyPlayerTD[playerid][bnt_PinCode] = 0;

    for(new i = 0; i < BOUNTY_MAX; i++) {
        if(!BountyList[i][bExists]) continue;
        new name[MAX_PLAYER_NAME];
        GetPlayerName(playerid, name, sizeof(name));
        if(strcmp(BountyList[i][bTargetName], name) == 0) {
            BountyList[i][bTargetId] = playerid;
            BountyOnHead[playerid] += BountyList[i][bAmount];
        }
    }
    if(BountyOnHead[playerid] > 0) {
        new msg[128];
        format(msg, sizeof(msg), "[BOUNTY]{FF453A} Imas nagradu od {FFD60A}$%d{FF453A} na svojoj glavi!", BountyOnHead[playerid]);
        SendClientMessage(playerid, BNT_TEXT_RED, msg);
    }
    return 1;
}

stock Bounty_OnPlayerDisconnect(playerid) {
    BountyCooldown[playerid] = 0;
    if(BountyPlayerTD[playerid][bnt_IsOpen]) Bounty_CloseUI(playerid);
    return 1;
}

// ===========================================================================
//  INIT
// ===========================================================================
stock Bounty_OnGameModeInit() {
    for(new i = 0; i < BOUNTY_MAX; i++) {
        BountyList[i][bExists] = false;
    }
    BountyCount = 0;
    print("[BOUNTY v3.0] Bounty Board sa Tablet Portal UI ucitan!");
    return 1;
}
