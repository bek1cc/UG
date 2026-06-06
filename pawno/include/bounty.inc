// ===========================================================================
//  BOUNTY BOARD SISTEM v1.0 za SA-MP
//  File: pawno/include/bounty.inc
//
//  Javna tablica nagrada - igrači stavljaju novac na glavu drugih igrača
//  Ko god ubije metu, dobija novac.
//
//  Komande:
//  /bounty [id] [iznos]    - stavi nagradu na igrača
//  /bounties               - pogledaj sve aktivne nagrade
//  /bountyme               - vidi da li imaš nagradu na sebi
//
//  Integracija: #include <bounty> u fg-ogc.pwn
// ===========================================================================

// ===========================================================================
//  KONFIGURACIJA
// ===========================================================================
#define BOUNTY_MAX              30      // Max aktivnih bountyja
#define BOUNTY_MIN_AMOUNT       1000    // Min $1,000 za bounty
#define BOUNTY_MAX_AMOUNT       500000  // Max $500,000 za bounty
#define BOUNTY_COOLDOWN         300     // 5 min cooldown između stavljanja
#define BOUNTY_DIALOG_BASE      2800

#define BOUNTY_DLG_LIST         (BOUNTY_DIALOG_BASE + 1)
#define BOUNTY_DLG_PLACE        (BOUNTY_DIALOG_BASE + 2)
#define BOUNTY_DLG_CONFIRM      (BOUNTY_DIALOG_BASE + 3)
#define BOUNTY_DLG_OWN          (BOUNTY_DIALOG_BASE + 4)

// ===========================================================================
//  BOJE
// ===========================================================================
#define BOUNTY_CLR_RED          0xFF453AFF
#define BOUNTY_CLR_GOLD         0xFFD60AFF
#define BOUNTY_CLR_GREEN        0x30D158FF
#define BOUNTY_CLR_WHITE        0xFFFFFFFF
#define BOUNTY_CLR_GRAY         0x8E8E93FF
#define BOUNTY_CLR_ORANGE       0xFF9F0AFF

// ===========================================================================
//  PODACI
// ===========================================================================
enum BountyData {
    bool:bExists,
    bTargetId,             // ID meta (koji je igrač)
    bTargetName[MAX_PLAYER_NAME], // Ime mete
    bPlacerId,             // ID ko je stavio bounty
    bPlacerName[MAX_PLAYER_NAME], // Ime ko je stavio
    bAmount,               // Iznos nagrade
    bTimestamp,            // Kad je stavljen
    bool:bAnonymous        // Anoniman bounty?
}

static BountyList[BOUNTY_MAX][BountyData];
static BountyCount = 0;

// Per-player cooldown
static BountyCooldown[MAX_PLAYERS];

// Per-player: koliko je ukupno nagrada na njihovoj glavi
static BountyOnHead[MAX_PLAYERS];

// ===========================================================================
//  HELPER FUNKCIJE
// ===========================================================================
stock Bounty_GetTotalOnHead(playerid) return BountyOnHead[playerid];

stock Bounty_FormatTimeAgo(timestamp) {
    new diff = gettime() - timestamp;
    new str[32];
    if(diff < 60) format(str, sizeof(str), "upravo");
    else if(diff < 3600) format(str, sizeof(str), "%d min", diff / 60);
    else if(diff < 86400) format(str, sizeof(str), "%d h", diff / 3600);
    else format(str, sizeof(str), "%d dana", diff / 86400);
    return str;
}

// ===========================================================================
//  INTERNAL: Dodaj bounty u listu
// ===========================================================================
stock Bounty_Add(targetid, placerid, amount, bool:anonymous = false) {
    // Pronađi prazan slot
    new slot = -1;
    for(new i = 0; i < BOUNTY_MAX; i++) {
        if(!BountyList[i][bExists]) { slot = i; break; }
    }

    // Ako nema praznog slota, ukloni najstariji
    if(slot == -1) {
        new oldestTime = 0x7FFFFFFF;
        new oldestIdx = 0;
        for(new i = 0; i < BOUNTY_MAX; i++) {
            if(BountyList[i][bExists] && BountyList[i][bTimestamp] < oldestTime) {
                oldestTime = BountyList[i][bTimestamp];
                oldestIdx = i;
            }
        }
        // Ukloni najstariji
        BountyOnHead[BountyList[oldestIdx][bTargetId]] -= BountyList[oldestIdx][bAmount];
        if(BountyOnHead[BountyList[oldestIdx][bTargetId]] < 0) BountyOnHead[BountyList[oldestIdx][bTargetId]] = 0;
        BountyList[oldestIdx][bExists] = false;
        slot = oldestIdx;
    }

    // Ako meta već ima bounty, dodaj na postojeći (ako je isti target)
    for(new i = 0; i < BOUNTY_MAX; i++) {
        if(BountyList[i][bExists] && BountyList[i][bTargetId] == targetid) {
            // Dodaj na postojeći bounty
            BountyList[i][bAmount] += amount;
            BountyOnHead[targetid] += amount;

            // Obavijesti metu
            new msg[128];
            format(msg, sizeof(msg), "[BOUNTY]{FF453A} Nagrada na tvojoj glavi je povećana na {FFD60A}$%d{FF453A}!", BountyList[i][bAmount]);
            SendClientMessage(targetid, BOUNTY_CLR_RED, msg);

            // Globalna obavijest
            format(msg, sizeof(msg), "[BOUNTY]{FFFFFF} Nagrada na glavi igrača %s je sada {FFD60A}$%d{FFFFFF}!", BountyList[i][bTargetName], BountyList[i][bAmount]);
            SendClientMessageToAll(BOUNTY_CLR_GOLD, msg);

            return 1;
        }
    }

    // Novi bounty
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

    // Obavijesti metu
    new msg[128];
    format(msg, sizeof(msg), "[BOUNTY]{FF453A} Neko je stavio nagradu od {FFD60A}$%d{FF453A} na tvoju glavu!", amount);
    SendClientMessage(targetid, BOUNTY_CLR_RED, msg);

    // Globalna obavijest
    format(msg, sizeof(msg), "[BOUNTY]{FFFFFF} Nagrada od {FFD60A}$%d{FFFFFF} stavljena na glavu igrača %s!", amount, BountyList[slot][bTargetName]);
    SendClientMessageToAll(BOUNTY_CLR_GOLD, msg);

    // Zvuk
    PlayerPlaySound(targetid, 1057, 0.0, 0.0, 0.0);

    return 1;
}

// ===========================================================================
//  INTERNAL: Igrač ubijen - provjera bountyja
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

        // Daj novac ubici
        GivePlayerMoney(killerid, totalReward);

        // Obavijesti
        new killerName[MAX_PLAYER_NAME], targetName[MAX_PLAYER_NAME];
        GetPlayerName(killerid, killerName, sizeof(killerName));
        GetPlayerName(playerid, targetName, sizeof(targetName));

        new msg[128];
        format(msg, sizeof(msg), "[BOUNTY]{30D158} %s je pokupio nagradu od {FFD60A}$%d{30D158} za ubistvo %s!", killerName, totalReward, targetName);
        SendClientMessageToAll(BOUNTY_CLR_GREEN, msg);

        format(msg, sizeof(msg), "[BOUNTY]{30D158} Pokupio si nagradu od {FFD60A}$%d{30D158}!", totalReward);
        SendClientMessage(killerid, BOUNTY_CLR_GREEN, msg);

        format(msg, sizeof(msg), "[BOUNTY]{FF453A} %s je pokupio nagradu na tvojoj glavi od {FFD60A}$%d{FF453A}!", killerName, totalReward);
        SendClientMessage(playerid, BOUNTY_CLR_RED, msg);

        // Zvuk
        PlayerPlaySound(killerid, 1058, 0.0, 0.0, 0.0);

        // Pokaži GameText
        new gtMsg[64];
        format(gtMsg, sizeof(gtMsg), "~g~BOUNTY~n~~y~$%d", totalReward);
        GameTextForPlayer(killerid, gtMsg, 3000, 3);

        return 1;
    }

    return 0;
}

// ===========================================================================
//  INTERNAL: Cleanup kad igrač disconnecta (samo obriši reference, ne i bounty)
// ===========================================================================
stock Bounty_OnPlayerDisconnect(playerid) {
    BountyCooldown[playerid] = 0;
    // NE brišemo bounty - ostaje aktivan dok neko ne ubije metu
    // Ako meta disconnecta, bounty ostaje za sljedeći put kad se uloguje
    return 1;
}

// ===========================================================================
//  INTERNAL: Reconnect - provjeri da li igrač ima bounty na glavi
// ===========================================================================
stock Bounty_OnPlayerConnect(playerid) {
    BountyCooldown[playerid] = 0;
    BountyOnHead[playerid] = 0; // Reset, ponovo izračunaj

    // Prebroj bountye na ovoj meti
    for(new i = 0; i < BOUNTY_MAX; i++) {
        if(!BountyList[i][bExists]) continue;
        // Match po imenu jer je ID drugačiji nakon reconnecta
        new name[MAX_PLAYER_NAME];
        GetPlayerName(playerid, name, sizeof(name));
        if(strcmp(BountyList[i][bTargetName], name) == 0) {
            BountyList[i][bTargetId] = playerid; // Update ID
            BountyOnHead[playerid] += BountyList[i][bAmount];
        }
    }

    // Obavijesti ako ima bounty
    if(BountyOnHead[playerid] > 0) {
        new msg[128];
        format(msg, sizeof(msg), "[BOUNTY]{FF453A} Imaš nagradu od {FFD60A}$%d{FF453A} na svojoj glavi! Budi oprezan!", BountyOnHead[playerid]);
        SendClientMessage(playerid, BOUNTY_CLR_RED, msg);
    }

    return 1;
}

// ===========================================================================
//  DIALOG: Lista svih aktivnih bountyja
// ===========================================================================
stock Bounty_ShowList(playerid) {
    new listStr[2048];
    new found = 0;

    format(listStr, sizeof(listStr), "{FFD60A}%-20s\t{30D158}%-12s\t{8E8E93}%-8s\t{FFFFFF}Postavio\n",
        "Meta", "Nagrada", "Prije");
    strcat(listStr, "{8E8E93}─────────────────────────────────────────────────\n");

    for(new i = 0; i < BOUNTY_MAX; i++) {
        if(!BountyList[i][bExists]) continue;

        found++;
        new line[128];

        // Status mete (online/offline)
        new statusStr[16];
        if(IsPlayerConnected(BountyList[i][bTargetId]) && BountyList[i][bTargetId] != INVALID_PLAYER_ID)
            statusStr = "{30D158}●";
        else
            statusStr = "{FF453A}●";

        // Ko je postavio
        new placerStr[24];
        if(BountyList[i][bAnonymous])
            format(placerStr, sizeof(placerStr), "{8E8E93}???");
        else
            format(placerStr, sizeof(placerStr), "{FFFFFF}%s", BountyList[i][bPlacerName]);

        format(line, sizeof(line), "%s %-18s\t{FFD60A}$%-11d\t{8E8E93}%-8s\t%s\n",
            statusStr,
            BountyList[i][bTargetName],
            BountyList[i][bAmount],
            Bounty_FormatTimeAgo(BountyList[i][bTimestamp]),
            placerStr);
        strcat(listStr, line);
    }

    if(!found) {
        strcat(listStr, "\n{8E8E93}Nema aktivnih nagrada trenutno.\n");
        strcat(listStr, "\n{FFFFFF}Koristi {FFD60A}/bounty [ID] [Iznos]{FFFFFF} da postaviš nagradu!");
    }

    ShowPlayerDialog(playerid, BOUNTY_DLG_LIST, DIALOG_STYLE_TABLIST_HEADERS,
        "{FFD60A}Bounty Board - Tablica Nagrada",
        listStr,
        "Zatvori", "");
}

// ===========================================================================
//  DIALOG: Provjera vlastitog bountyja
// ===========================================================================
stock Bounty_ShowOwn(playerid) {
    if(BountyOnHead[playerid] <= 0) {
        SendClientMessage(playerid, BOUNTY_CLR_GREEN, "[BOUNTY]{FFFFFF} Nemaš nikakvu nagradu na svojoj glavi. Slobodan si!");
        return 1;
    }

    new msg[256];
    format(msg, sizeof(msg), "\n{FF453A}NAGRADA NA TVOJOJ GLAVI: {FFD60A}$%d\n\n", BountyOnHead[playerid]);
    strcat(msg, "{8E8E93}─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─\n\n");

    for(new i = 0; i < BOUNTY_MAX; i++) {
        if(!BountyList[i][bExists]) continue;
        if(BountyList[i][bTargetId] != playerid) continue;

        new line[64];
        format(line, sizeof(line), "{FFFFFF}$%d {8E8E93}· postavljen prije %s\n",
            BountyList[i][bAmount],
            Bounty_FormatTimeAgo(BountyList[i][bTimestamp]));
        strcat(msg, line);
    }

    strcat(msg, "\n{FF453A}Budi oprezan - neko te lovi!");

    ShowPlayerDialog(playerid, BOUNTY_DLG_OWN, DIALOG_STYLE_MSGBOX,
        "{FF453A}Bounty - Tvoja Nagrada",
        msg,
        "Zatvori", "");

    return 1;
}

// ===========================================================================
//  KOMANDA: /bounty [id] [iznos]
// ===========================================================================
stock CMD_Bounty_Place(playerid, params[]) {
    new targetid, amount;

    if(sscanf(params, "ii", targetid, amount)) {
        SendClientMessage(playerid, BOUNTY_CLR_WHITE, "[BOUNTY]{FFFFFF} Koristi: {FFD60A}/bounty [ID Igrača] [Iznos]");
        SendClientMessage(playerid, BOUNTY_CLR_GRAY, "[BOUNTY] Min: $1,000 | Max: $500,000");
        return 1;
    }

    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid)) {
        SendClientMessage(playerid, BOUNTY_CLR_RED, "[BOUNTY]{FFFFFF} Igrač nije online!");
        return 1;
    }

    if(targetid == playerid) {
        SendClientMessage(playerid, BOUNTY_CLR_RED, "[BOUNTY]{FFFFFF} Ne možeš staviti nagradu na sebe!");
        return 1;
    }

    if(amount < BOUNTY_MIN_AMOUNT) {
        new msg[64];
        format(msg, sizeof(msg), "[BOUNTY]{FFFFFF} Minimalni iznos je {FFD60A}$%d{FFFFFF}!", BOUNTY_MIN_AMOUNT);
        SendClientMessage(playerid, BOUNTY_CLR_RED, msg);
        return 1;
    }

    if(amount > BOUNTY_MAX_AMOUNT) {
        new msg[64];
        format(msg, sizeof(msg), "[BOUNTY]{FFFFFF} Maksimalni iznos je {FFD60A}$%d{FFFFFF}!", BOUNTY_MAX_AMOUNT);
        SendClientMessage(playerid, BOUNTY_CLR_RED, msg);
        return 1;
    }

    if(GetPlayerMoney(playerid) < amount) {
        SendClientMessage(playerid, BOUNTY_CLR_RED, "[BOUNTY]{FFFFFF} Nemaš dovoljno novca!");
        return 1;
    }

    // Cooldown
    if(BountyCooldown[playerid] > gettime()) {
        new remain = BountyCooldown[playerid] - gettime();
        new msg[64];
        format(msg, sizeof(msg), "[BOUNTY]{FFFFFF} Sačekaj još %d sekundi prije sljedećeg bountyja.", remain);
        SendClientMessage(playerid, BOUNTY_CLR_ORANGE, msg);
        return 1;
    }

    // Confirm dialog
    new targetName[MAX_PLAYER_NAME];
    GetPlayerName(targetid, targetName, sizeof(targetName));

    new confirmStr[256];
    format(confirmStr, sizeof(confirmStr),
        "{FFFFFF}Da li želiš staviti nagradu na igrača:\n\n{FF453A}%s (ID:%d)\n\n{FFFFFF}Iznos: {FFD60A}$%d\n\n{8E8E93}Novac će biti skinut sa tvog računa.\nKo god ubije ovog igrača, dobiće nagradu.",
        targetName, targetid, amount);

    // Spremi podatke za confirm
    SetPVarInt(playerid, "BountyTarget", targetid);
    SetPVarInt(playerid, "BountyAmount", amount);

    ShowPlayerDialog(playerid, BOUNTY_DLG_CONFIRM, DIALOG_STYLE_MSGBOX,
        "{FFD60A}Bounty Board - Potvrda",
        confirmStr,
        "Postavi", "Odustani");

    return 1;
}

// ===========================================================================
//  KOMANDA: /bounties - lista svih bountyja
// ===========================================================================
stock CMD_Bounty_List(playerid) {
    Bounty_ShowList(playerid);
    return 1;
}

// ===========================================================================
//  KOMANDA: /bountyme - provjera vlastitog bountyja
// ===========================================================================
stock CMD_Bounty_Me(playerid) {
    Bounty_ShowOwn(playerid);
    return 1;
}

// ===========================================================================
//  DIALOG HANDLER
// ===========================================================================
stock Bounty_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {
    switch(dialogid) {
        case BOUNTY_DLG_LIST: {
            // Samo zatvaranje
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

            // Ponovna provjera
            if(!IsPlayerConnected(targetid)) {
                SendClientMessage(playerid, BOUNTY_CLR_RED, "[BOUNTY]{FFFFFF} Igrač više nije online!");
                return 1;
            }

            if(GetPlayerMoney(playerid) < amount) {
                SendClientMessage(playerid, BOUNTY_CLR_RED, "[BOUNTY]{FFFFFF} Nemaš više dovoljno novca!");
                return 1;
            }

            // Skini novac
            GivePlayerMoney(playerid, -amount);

            // Postavi bounty
            Bounty_Add(targetid, playerid, amount, false);

            // Cooldown
            BountyCooldown[playerid] = gettime() + BOUNTY_COOLDOWN;

            // Obavijesti onom ko je stavio
            new msg[64];
            format(msg, sizeof(msg), "[BOUNTY]{30D158} Postavio si nagradu od {FFD60A}$%d{30D158}!", amount);
            SendClientMessage(playerid, BOUNTY_CLR_GREEN, msg);

            return 1;
        }

        case BOUNTY_DLG_OWN: {
            return 1;
        }
    }

    return 0;
}

// ===========================================================================
//  INIT
// ===========================================================================
stock Bounty_OnGameModeInit() {
    // Reset sve
    for(new i = 0; i < BOUNTY_MAX; i++) {
        BountyList[i][bExists] = false;
    }
    BountyCount = 0;
    print("[BOUNTY v1.0] Bounty Board sistem ucitan!");
    return 1;
}
