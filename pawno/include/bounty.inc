// ===========================================================================
//  BOUNTY BOARD SISTEM v2.0 za SA-MP
//  File: pawno/include/bounty.inc
//
//  Javna tablica nagrada - igrači stavljaju novac na glavu drugih igrača
//  Moderan TABLET UI sa textdrawovima
//
//  Komande:
//  /bounty [id] [iznos]    - stavi nagradu na igrača
//  /bounties               - pogledaj sve aktivne nagrade (otvara tablet UI)
//  /bountyme               - vidi da li imaš nagradu na sebi
//
//  Integracija: #include <bounty> u fg-ogc.pwn
// ===========================================================================

// ===========================================================================
//  KONFIGURACIJA
// ===========================================================================
#define BOUNTY_MAX              30      // Max aktivnih bountyja
#define BOUNTY_MIN_AMOUNT       1000    // Min $1,000
#define BOUNTY_MAX_AMOUNT       500000  // Max $500,000
#define BOUNTY_COOLDOWN         300     // 5 min cooldown
#define BOUNTY_PAGE_SIZE        6       // 6 redova po stranici u TD
#define BOUNTY_DIALOG_BASE      2800

#define BOUNTY_DLG_PLACE        (BOUNTY_DIALOG_BASE + 1)
#define BOUNTY_DLG_CONFIRM      (BOUNTY_DIALOG_BASE + 2)

// ===========================================================================
//  BOJE (Dark Mode Tablet paleta)
// ===========================================================================
#define BNT_BG                  0x1C1C1EFF  // Tamni bg
#define BNT_HEADER_BG           0x2C2C2EFF  // Header bg
#define BNT_ROW_EVEN            0x252528FF  // Par red
#define BNT_ROW_ODD             0x1C1C1EFF  // Neparni red
#define BNT_ROW_HOVER           0x3A3A3CFF  // Hover red
#define BNT_BORDER              0x3A3A3CFF  // Border boja
#define BNT_TEXT_WHITE          0xFFFFFFFF
#define BNT_TEXT_GRAY           0x8E8E93FF
#define BNT_TEXT_RED            0xFF453AFF
#define BNT_TEXT_GREEN          0x30D158FF
#define BNT_TEXT_GOLD           0xFFD60AFF
#define BNT_TEXT_BLUE           0x0A84FFFF
#define BNT_BTN_RED             0xFF453ABB
#define BNT_BTN_GREEN           0x30D158BB
#define BNT_BTN_BLUE            0x0A84FFBB
#define BNT_BTN_GRAY            0x48484AFF

// ===========================================================================
//  TABLET LAYOUT KOORDINATE (640x480 SA-MP grid)
// ===========================================================================
#define BNT_TAB_X               80.0    // Tableta lijeva ivica
#define BNT_TAB_Y               30.0    // Tableta gornja ivica
#define BNT_TAB_W               480.0   // Tableta sirina
#define BNT_TAB_H              400.0    // Tableta visina

// Header
#define BNT_HDR_Y               32.0
#define BNT_HDR_H               35.0

// Lista redova (6 redova)
#define BNT_ROW_START_Y         80.0
#define BNT_ROW_H               45.0
#define BNT_ROW_X               90.0
#define BNT_ROW_W               460.0

// Footer dugmad
#define BNT_FTR_Y              365.0
#define BNT_FTR_H              30.0

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

static BountyList[BOUNTY_MAX][BountyData];
static BountyCount = 0;
static BountyCooldown[MAX_PLAYERS];
static BountyOnHead[MAX_PLAYERS];

// TD varijable
enum BountyTDData {
    PlayerText:bnt_TD[32],     // 32 textdraw slotova
    bool:bnt_TDCreated,
    bool:bnt_IsOpen,
    bnt_Page,                   // Trenutna stranica (0-based)
    bnt_TotalPages
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

// Prebroj aktivne bountyje
stock Bounty_CountActive() {
    new count = 0;
    for(new i = 0; i < BOUNTY_MAX; i++) {
        if(BountyList[i][bExists]) count++;
    }
    return count;
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

            // Refresh UI ako je otvoren
            for(new p = 0; p < MAX_PLAYERS; p++) {
                if(BountyPlayerTD[p][bnt_IsOpen]) Bounty_RefreshList(p);
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
        if(BountyOnHead[BountyList[oldestIdx][bTargetId]] < 0) BountyOnHead[BountyList[oldestIdx][bTargetId]] = 0;
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

    // Refresh UI
    for(new p = 0; p < MAX_PLAYERS; p++) {
        if(BountyPlayerTD[p][bnt_IsOpen]) Bounty_RefreshList(p);
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
        format(msg, sizeof(msg), "[BOUNTY]{FF453A} %s je pokupio nagradu na tvojoj glavi od {FFD60A}$%d{FF453A}!", killerName, totalReward);
        SendClientMessage(playerid, BNT_TEXT_RED, msg);

        PlayerPlaySound(killerid, 1058, 0.0, 0.0, 0.0);
        new gtMsg[64];
        format(gtMsg, sizeof(gtMsg), "~g~BOUNTY~n~~y~$%d", totalReward);
        GameTextForPlayer(killerid, gtMsg, 3000, 3);

        // Refresh svih otvorenih UI-ja
        for(new p = 0; p < MAX_PLAYERS; p++) {
            if(BountyPlayerTD[p][bnt_IsOpen]) Bounty_RefreshList(p);
        }
        return 1;
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
    BountyPlayerTD[playerid][bnt_Page] = 0;

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
//  TEXTDRAW KREIRANJE
// ===========================================================================
stock Bounty_CreateTDs(playerid) {
    if(BountyPlayerTD[playerid][bnt_TDCreated]) return;
    BountyPlayerTD[playerid][bnt_TDCreated] = true;

    new td = 0;

    // 0: Tablet body - dark background
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X, BNT_TAB_Y, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 48.5);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TAB_X + BNT_TAB_W, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BG);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetOutline(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 1: Tablet border
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X - 1.0, BNT_TAB_Y - 1.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 49.0);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TAB_X + BNT_TAB_W + 2.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BORDER);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 2: Header background (crveni gradient header)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X + 1.0, BNT_HDR_Y, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 4.0);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TAB_X + BNT_TAB_W - 1.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0x8B0000FF); // Tamno crvena
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 3: Header icon (skull)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X + 15.0, BNT_HDR_Y + 5.0, "WANTED");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.35, 1.8);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_GOLD);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 4: Header title
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X + BNT_TAB_W / 2.0, BNT_HDR_Y + 8.0, "BOUNTY BOARD");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.4, 2.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 5: Header - broj aktivnih bountyja
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X + BNT_TAB_W - 15.0, BNT_HDR_Y + 10.0, "0/30");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.25, 1.2);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_GRAY);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 6: Separator ispod headera
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X + 1.0, BNT_HDR_Y + BNT_HDR_H, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 0.3);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TAB_X + BNT_TAB_W - 1.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BORDER);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 7: Column headers (Meta / Nagrada / Status / Postavio)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X + 15.0, 72.0, "META                  NAGRADA      STATUS   PRIJE");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.18, 0.9);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_GRAY);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 8-13: Row backgrounds (6 redova) - naizmjenične boje
    for(new row = 0; row < BOUNTY_PAGE_SIZE; row++) {
        new Float:rowY = BNT_ROW_START_Y + (row * BNT_ROW_H);
        BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_ROW_X, rowY, "_");
        PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 5.2);
        PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_ROW_X + BNT_ROW_W, 0.0);
        PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
        PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], (row % 2 == 0) ? BNT_ROW_EVEN : BNT_ROW_ODD);
        PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
        PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        td++;
    }

    // 14-19: Row text (ime mete + iznos + status)
    for(new row = 0; row < BOUNTY_PAGE_SIZE; row++) {
        new Float:rowY = BNT_ROW_START_Y + (row * BNT_ROW_H) + 5.0;
        BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_ROW_X + 10.0, rowY, " ");
        PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.22, 1.2);
        PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
        PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
        PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        td++;
    }

    // 20-25: Row amount text (nagrada - gold boja)
    for(new row = 0; row < BOUNTY_PAGE_SIZE; row++) {
        new Float:rowY = BNT_ROW_START_Y + (row * BNT_ROW_H) + 18.0;
        BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_ROW_X + 10.0, rowY, " ");
        PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.2, 1.0);
        PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_GRAY);
        PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
        PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
        td++;
    }

    // 26: "Nema bountyja" empty state
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X + BNT_TAB_W / 2.0, 200.0, "NEMA AKTIVNIH NAGRADA~n~~w~Koristi /bounty [ID] [Iznos]");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.3, 1.5);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_GRAY);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][td]);
    td++;

    // 27: Footer separator
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X + 1.0, BNT_FTR_Y - 5.0, "_");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.0, 0.2);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TAB_X + BNT_TAB_W - 1.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BORDER);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    td++;

    // 28: Page info text
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X + BNT_TAB_W / 2.0, BNT_FTR_Y + 2.0, "Strana 1/1");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.22, 1.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 2);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_GRAY);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 29: dugme ZATVORI (lijevo)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X + 15.0, BNT_FTR_Y, "ZATVORI");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.22, 1.3);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TAB_X + 95.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BTN_RED);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 30: dugme POSTAVI (sredina)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X + 110.0, BNT_FTR_Y, "POSTAVI");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.22, 1.3);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TAB_X + 205.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BTN_GREEN);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;

    // 31: dugme NATRAG (desno - za paginaciju)
    BountyPlayerTD[playerid][bnt_TD][td] = CreatePlayerTextDraw(playerid, BNT_TAB_X + BNT_TAB_W - 110.0, BNT_FTR_Y, "< NATRAG");
    PlayerTextDrawFont(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0.2, 1.1);
    PlayerTextDrawTextSize(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TAB_X + BNT_TAB_W - 15.0, 0.0);
    PlayerTextDrawAlignment(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_TEXT_WHITE);
    PlayerTextDrawUseBox(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, BountyPlayerTD[playerid][bnt_TD][td], BNT_BTN_GRAY);
    PlayerTextDrawSetShadow(playerid, BountyPlayerTD[playerid][bnt_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    PlayerTextDrawSetSelectable(playerid, BountyPlayerTD[playerid][bnt_TD][td], 1);
    td++;
}

// ===========================================================================
//  TEXTDRAW UNIŠTAVANJE
// ===========================================================================
stock Bounty_DestroyTDs(playerid) {
    if(!BountyPlayerTD[playerid][bnt_TDCreated]) return;
    for(new i = 0; i < 32; i++) {
        PlayerTextDrawDestroy(playerid, BountyPlayerTD[playerid][bnt_TD][i]);
    }
    BountyPlayerTD[playerid][bnt_TDCreated] = false;
}

// ===========================================================================
//  OTVORI / ZATVORI UI
// ===========================================================================
stock Bounty_OpenUI(playerid) {
    if(BountyPlayerTD[playerid][bnt_IsOpen]) return;

    BountyPlayerTD[playerid][bnt_IsOpen] = true;
    BountyPlayerTD[playerid][bnt_Page] = 0;

    Bounty_CreateTDs(playerid);
    Bounty_RefreshList(playerid);

    // Pokaži sve TD-ove
    for(new i = 0; i < 32; i++) {
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][i]);
    }

    SelectTextDraw(playerid, 0xFFFFFFFF);
    PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
}

stock Bounty_CloseUI(playerid) {
    if(!BountyPlayerTD[playerid][bnt_IsOpen]) return;

    BountyPlayerTD[playerid][bnt_IsOpen] = false;

    for(new i = 0; i < 32; i++) {
        PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][i]);
    }
    Bounty_DestroyTDs(playerid);
    CancelSelectTextDraw(playerid);
}

// ===========================================================================
//  OSVJEŽI LISTU
// ===========================================================================
stock Bounty_RefreshList(playerid) {
    if(!BountyPlayerTD[playerid][bnt_IsOpen]) return;

    new activeCount = Bounty_CountActive();
    new totalPages = (activeCount + BOUNTY_PAGE_SIZE - 1) / BOUNTY_PAGE_SIZE;
    if(totalPages == 0) totalPages = 1;

    BountyPlayerTD[playerid][bnt_TotalPages] = totalPages;

    // Update count
    new countStr[16];
    format(countStr, sizeof(countStr), "%d/%d", activeCount, BOUNTY_MAX);
    PlayerTextDrawSetString(playerid, BountyPlayerTD[playerid][bnt_TD][5], countStr);

    // Update page info
    new pageStr[32];
    format(pageStr, sizeof(pageStr), "Strana %d/%d", BountyPlayerTD[playerid][bnt_Page] + 1, totalPages);
    PlayerTextDrawSetString(playerid, BountyPlayerTD[playerid][bnt_TD][28], pageStr);

    if(activeCount == 0) {
        // Pokaži empty state
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][26]);
        // Sakrij sve redove
        for(new row = 0; row < BOUNTY_PAGE_SIZE; row++) {
            PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][8 + row]);   // row bg
            PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][14 + row]);  // row name
            PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][20 + row]);  // row detail
        }
        return;
    }

    // Sakrij empty state
    PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][26]);

    // Popuni redove za trenutnu stranicu
    new startIdx = BountyPlayerTD[playerid][bnt_Page] * BOUNTY_PAGE_SIZE;
    new displayed = 0;
    new listIdx = 0;

    for(new i = 0; i < BOUNTY_MAX && displayed < BOUNTY_PAGE_SIZE; i++) {
        if(!BountyList[i][bExists]) continue;
        if(listIdx < startIdx) { listIdx++; continue; }

        new row = displayed;
        new Float:rowY = BNT_ROW_START_Y + (row * BNT_ROW_H);

        // Status
        new statusStr[16];
        if(IsPlayerConnected(BountyList[i][bTargetId]) && BountyList[i][bTargetId] != INVALID_PLAYER_ID)
            statusStr = "{30D158}ONLINE";
        else
            statusStr = "{FF453A}OFFLINE";

        // Ime mete
        new nameStr[64];
        format(nameStr, sizeof(nameStr), "{FFD60A}%s", BountyList[i][bTargetName]);
        PlayerTextDrawSetString(playerid, BountyPlayerTD[playerid][bnt_TD][14 + row], nameStr);
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][14 + row]);

        // Detalji (iznos + status + vrijeme)
        new detailStr[128];
        format(detailStr, sizeof(detailStr), "{30D158}$%d  %s  {8E8E93}prije %s",
            BountyList[i][bAmount], statusStr, Bounty_FormatTimeAgo(BountyList[i][bTimestamp]));
        PlayerTextDrawSetString(playerid, BountyPlayerTD[playerid][bnt_TD][20 + row], detailStr);
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][20 + row]);

        // Pokaži red bg
        PlayerTextDrawShow(playerid, BountyPlayerTD[playerid][bnt_TD][8 + row]);

        displayed++;
        listIdx++;
    }

    // Sakrij prazne redove
    for(new row = displayed; row < BOUNTY_PAGE_SIZE; row++) {
        PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][8 + row]);
        PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][14 + row]);
        PlayerTextDrawHide(playerid, BountyPlayerTD[playerid][bnt_TD][20 + row]);
    }
}

// ===========================================================================
//  CLICK HANDLER
// ===========================================================================
stock Bounty_OnClickTD(playerid, PlayerText:playertextid) {
    if(!BountyPlayerTD[playerid][bnt_IsOpen]) return 0;

    // Zatvori dugme (29)
    if(playertextid == BountyPlayerTD[playerid][bnt_TD][29]) {
        Bounty_CloseUI(playerid);
        return 1;
    }

    // Postavi dugme (30)
    if(playertextid == BountyPlayerTD[playerid][bnt_TD][30]) {
        Bounty_CloseUI(playerid);
        ShowPlayerDialog(playerid, BOUNTY_DLG_PLACE, DIALOG_STYLE_INPUT,
            "{FFD60A}Bounty Board - Postavi nagradu",
            "{FFFFFF}Unesite: ID_Igraca Iznos\nPrimjer: 5 10000\n\n{8E8E93}Min: $1,000 | Max: $500,000",
            "Postavi", "Odustani");
        return 1;
    }

    // Natrag dugme (31)
    if(playertextid == BountyPlayerTD[playerid][bnt_TD][31]) {
        Bounty_CloseUI(playerid);
        return 1;
    }

    // Klik na red (8-13) - prikaži detalje o tom bountyju
    for(new row = 0; row < BOUNTY_PAGE_SIZE; row++) {
        if(playertextid == BountyPlayerTD[playerid][bnt_TD][8 + row]) {
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
                format(msg, sizeof(msg),
                    "{FFFFFF}Meta: {FF453A}%s\n{FFFFFF}Nagrada: {FFD60A}$%d\n{FFFFFF}Postavio: %s\n{8E8E93}Prije: %s",
                    BountyList[foundSlot][bTargetName],
                    BountyList[foundSlot][bAmount],
                    (BountyList[foundSlot][bAnonymous] ? ("{8E8E93}???") : BountyList[foundSlot][bPlacerName]),
                    Bounty_FormatTimeAgo(BountyList[foundSlot][bTimestamp]));
                ShowPlayerDialog(playerid, BOUNTY_DLG_CONFIRM + 1, DIALOG_STYLE_MSGBOX,
                    "{FFD60A}Bounty Detalji", msg, "Zatvori", "");
            }
            return 1;
        }
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
        "{FFFFFF}Da li zelite staviti nagradu na:\n\n{FF453A}%s (ID:%d)\n\n{FFFFFF}Iznos: {FFD60A}$%d\n\n{8E8E93}Novac ce biti skinut sa racuna.",
        targetName, targetid, amount);

    SetPVarInt(playerid, "BountyTarget", targetid);
    SetPVarInt(playerid, "BountyAmount", amount);

    ShowPlayerDialog(playerid, BOUNTY_DLG_CONFIRM, DIALOG_STYLE_MSGBOX,
        "{FFD60A}Bounty Board - Potvrda", confirmStr, "Postavi", "Odustani");
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
stock Bounty_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {
    switch(dialogid) {
        case BOUNTY_DLG_PLACE: {
            if(!response) { Bounty_OpenUI(playerid); return 1; }
            // Parse input
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
            format(msg, sizeof(msg), "[BOUNTY]{30D158} Postavio si nagradu od {FFD60A}$%d{30D158}!", amount);
            SendClientMessage(playerid, BNT_TEXT_GREEN, msg);
            return 1;
        }
    }
    return 0;
}

// ===========================================================================
//  INIT
// ===========================================================================
stock Bounty_OnGameModeInit() {
    for(new i = 0; i < BOUNTY_MAX; i++) {
        BountyList[i][bExists] = false;
    }
    BountyCount = 0;
    print("[BOUNTY v2.0] Bounty Board sistem sa Tablet UI ucitan!");
    return 1;
}
