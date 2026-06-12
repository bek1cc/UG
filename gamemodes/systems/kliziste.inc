// ===========================================================================
//  SISTEM KLIZISTA - SA-MP
//  Verzija: 2.0
//  Autor: Beka (uz pomoc AI asistenta)
//  Opis: Kompletan, modularan sistem klizista (odrona stijena) za SA-MP server.
//        Kliziste se automatski pokrece svaka 2 sata servera ili admin komandom.
//        Blokira autoput Beograd-Sarajevo, prikazuje upozorenje svim igracima,
//        tresti ekran svim igracima pri aktivaciji. Igraci dolaze na lokaciju,
//        uzimaju alat (cekic/pajser), idu na sekvencijalne iskopine (jedna po jedna),
//        igraju minigame (horizontalni bar sa zelenom zonom + SPACE u pravom trenutku).
//        Svaki uspjesan minigame daje 1-3 kamena u inventory + $88-400.
//        Potrebno 200 uspjesnih minigame-a od SVIH igraca zajedno da se ocisti.
//        Bonus $10,000 kad se ocisti + $5,000 za aktivne radnike.
//
//  KOMANDE:
//    /prekidakliziste  - Prekini rad na klizistu
//    /skiniopremu      - Skini opremu i odjavi se sa klizista
//
//  KORISCENJE (hookovi u fg-ogc.pwn):
//    - Kliziste_Init()         - poziva se u OnGameModeInit
//    - Kliziste_OnConnect()    - poziva se u OnPlayerConnect
//    - Kliziste_OnDisconnect() - poziva se u OnPlayerDisconnect
//    - Kliziste_OnDeath()      - poziva se u OnPlayerDeath
//    - Kliziste_OnKeyStateChange() - poziva se u OnPlayerKeyStateChange
//    - Kliziste_OnUpdate()     - poziva se u OnPlayerUpdate
//
//  ADMIN KOMANDE (u fg-ogc.pwn):
//    /pokrenikliziste   - Pokrene kliziste rucno
//    /zaustavikliziste  - Zaustavi kliziste rucno
//    /klizistetime      - Vrijeme do sljedeceg klizista
// ===========================================================================

#if defined _kliziste_included
    #endinput
#endif
#define _kliziste_included

// ===========================================================================
//  KONFIGURACIJA
// ===========================================================================

// Glavna lokacija klizista (mjesto gdje igrac uzima/oprema/vraca opremu)
#define KLIZISTE_LOC_X           1709.7611
#define KLIZISTE_LOC_Y           -445.5623
#define KLIZISTE_LOC_Z           33.6878

// Podesavanja
#define KLIZISTE_OPREMA_DIST     4.0     // Domet za uzimanje/vracanje opreme
#define KLIZISTE_DIG_DIST        3.0     // Domet za iskopinu
#define KLIZISTE_AUTO_INTERVAL   7200    // Sekunde izmedu auto-aktivacija (2 sata)
#define KLIZISTE_TOTAL_DIGS      200     // Ukupan broj uspjesnih minigame-a za ciscenje
#define KLIZISTE_STONE_MIN       1       // Min kamena po uspjesnom minigame-u
#define KLIZISTE_STONE_MAX       3       // Max kamena po uspjesnom minigame-u
#define KLIZISTE_PAY_MIN         88      // Min isplata $ po uspjesnom minigame-u
#define KLIZISTE_PAY_MAX         400     // Max isplata $ po uspjesnom minigame-u
#define KLIZISTE_BONUS_AMOUNT    10000   // Bonus $ kad se ocisti kliziste (svi iskopi)
#define KLIZISTE_SHAKE_DURATION  2000    // Milisekunde trajanja trezanja ekrana (2 sec)
#define KLIZISTE_SHAKE_LEVEL     5000    // Nivo DrunkLevel za tresti ekran (vidljiv shake 2-3 sec)
#define KLIZISTE_MINIGAME_SPEED  3.0     // Brzina pomjeranja markera (po ticku, 0-100 skala)
#define KLIZISTE_MINIGAME_ZONE   25      // Velicina zone pogotka (0-100, manje = teze)
#define KLIZISTE_OPREMA_SKIN     27      // Skin za radnika
#define KLIZISTE_WEAPON_ALAT     6       // Shovel (lopata/alatka)

// Minigame bar - BOX-based pristup (pixel-perfect, bez tilde bugova)
#define KLIZISTE_BAR_VISUAL_X   192.0   // Lijeva ivica bar vizuelnog prostora
#define KLIZISTE_BAR_VISUAL_END 448.0   // Desna ivica bar vizuelnog prostora
#define KLIZISTE_BAR_VISUAL_W   256.0   // Sirina bara u SA-MP jedinicama (448-192)
#define KLIZISTE_MARKER_WIDTH   4.0     // Sirina markera u SA-MP jedinicama

// Item za inventory
#define KLIZISTE_ITEM_STONE      "Kamen"
#define KLIZISTE_MODEL_STONE     909     // Rock/stone ikona u inventory (pravi kamen)

// Zvukovi
#define KLIZISTE_SOUND_START     34000   // Zemljotres/explozija zvuk
#define KLIZISTE_SOUND_OPREMA    33100   // Pickup opreme
#define KLIZISTE_SOUND_DIG       33700   // Kopanje/pickaxe
#define KLIZISTE_SOUND_SUCCESS   1057    // Uspjeh
#define KLIZISTE_SOUND_FAIL      1085    // Neuspjeh minigame
#define KLIZISTE_SOUND_CLEAR     1058    // Kliziste ocisceno

// Boje za poruke
#define KLIZISTE_CLR_MSG         0xFF4500AA   // Crveno-narandzasta (kliziste)
#define KLIZISTE_CLR_INFO        0xFFD700AA   // Zlatna
#define KLIZISTE_CLR_ERR         0xFF6347AA   // Crvena
#define KLIZISTE_CLR_SUCCESS     0x00FF00AA   // Zelena

// 11 iskopina - koordinate
#define KLIZISTE_DIG_SPOTS      11

static const Float:KlizDigCoords[KLIZISTE_DIG_SPOTS][3] = {
    {1675.8663, -430.2977, 38.5326},   // 1
    {1686.9738, -429.0655, 39.9105},   // 2
    {1683.5287, -438.2557, 33.2855},   // 3
    {1695.9578, -435.6753, 34.0470},   // 4
    {1695.0750, -430.7414, 37.5874},   // 5
    {1704.5243, -429.0216, 38.8165},   // 6
    {1707.4004, -431.6657, 35.1996},   // 7
    {1702.9473, -433.0618, 33.9590},   // 8
    {1687.1003, -439.3870, 33.2472},   // 9
    {1683.7937, -437.9643, 33.3089},   // 10
    {1711.2848, -430.2581, 38.3685}    // 11
};

// ===========================================================================
//  ENUMERACIJE I PROMJENLJIVE
// ===========================================================================

// Stanja igraca u sistemu
enum e_KlizistePlayerState {
    KLIZISTE_PSTATE_NONE = 0,       // Ne radi nista
    KLIZISTE_PSTATE_EQUIPPED,      // Uzeo opremu, ide do iskopine
    KLIZISTE_PSTATE_DIGGING,       // Igra minigame na iskopini
    KLIZISTE_PSTATE_COOLDOWN       // Cooldown nakon uspjesnog kopanja
}

// Podaci o igracu
enum e_KlizistePlayer {
    e_KlizistePlayerState:klizPState,
    bool:klizHasEquipment,          // Da li ima opremu
    klizOldSkin,                    // Stari skin
    klizCurrentSpot,                // Trenutna iskopina (0-10), -1 = nijedna
    klizCooldownTime,               // Cooldown (GetTickCount)
    bool:klizMinigameActive,        // Da li je minigame aktivan
    Float:klizMGProgress,           // Pozicija markera u minigame (0.0 - 100.0)
    Float:klizMGTargetPos,          // Centar ciljne zone (0.0 - 100.0)
    Float:klizMGDirection,           // Pravac kretanja (1.0 = naprijed, -1.0 = nazad)
    klizMGTimer,                    // Timer za minigame rotaciju
    PlayerText:klizMGBgPanel,       // Minigame glavni panel (tamna pozadina)
    PlayerText:klizMGHeaderLine,    // Minigame narandzasti header accent
    PlayerText:klizMGFooterLine,    // Minigame footer border linija
    PlayerText:klizMGTitle,         // Minigame naslov TD
    PlayerText:klizMGBarTrack,      // Minigame bar pozadina (track)
    PlayerText:klizMGBarText,       // Minigame bar tekst (zona + marker)
    PlayerText:klizMGStatus,        // Minigame status tekst
    PlayerText:klizMGKeyHint,       // Minigame SPACE instrukcija
    PlayerText:klizMGReward,        // Minigame nagrada info
    PlayerText:klizProgressTD,      // Globalni progress TD
    PlayerText:klizStatusTD,        // Status TD (gore lijevo)
    bool:klizTDShown,               // Da li su TDs prikazani
    klizLastAltPress,               // Debounce za ALT key
    klizLastTDUpdate                // Throttle TD updates
}

static KlizPlayerData[MAX_PLAYERS][e_KlizistePlayer];

// Globalno stanje klizista
static bool:KlizActive = false;             // Da li je kliziste aktivno
static KlizTotalDigs = 0;                   // Ukupan broj uspjesnih minigame-a
static KlizAutoTimer = -1;                  // Timer za auto-aktivaciju
static KlizNextActivation = 0;              // GetTickCount vrijeme sljedece aktivacije
static KlizShakeTimer = -1;                 // Timer za tresti ekran

// Globalni objekti klizista (stijene na autoputu)
static Text3D:Kliz3D_Location = Text3D:-1;  // 3D label na lokaciji opreme
static KlizPickup_Location = -1;            // Pickup na lokaciji opreme

// Per-player 3D label za trenutnu iskopinu (sekvencijalna - jedna po jedna)
static Text3D:KlizDigLabel[MAX_PLAYERS] = {Text3D:-1, ...};
static KlizDigPickup[MAX_PLAYERS] = {-1, ...};

// Map icon
static KlizMapIcon = -1;

// Per-player shake timer (za kameru kod ENB kompatibilnosti)
static KlizShakeTimers[MAX_PLAYERS] = {-1, ...};
static KlizShakeTicks[MAX_PLAYERS] = {0, ...};

// Pragma unused za static varijable
#pragma unused Kliz3D_Location
#pragma unused KlizPickup_Location
#pragma unused KlizMapIcon
#pragma unused KlizAutoTimer
#pragma unused KlizShakeTimer

// Forward timer callbacka
forward KlizTimer_AutoActivation();
forward KlizTimer_MGRotate(playerid);
forward KlizTimer_ShakeEnd(playerid);
forward KlizTimer_Cooldown(playerid);
// KlizTimer_ShakeTick uklonjen - vise ne koristimo SetPlayerCameraPos

// ===========================================================================
//  HELPER FUNKCIJE
// ===========================================================================

stock KlizMSG(playerid, msg[]) {
    SendClientMessage(playerid, KLIZISTE_CLR_MSG, msg);
}

stock KlizGRESKA(playerid, msg[]) {
    SendClientMessage(playerid, KLIZISTE_CLR_ERR, msg);
    return 1;
}

stock KlizINFO(playerid, msg[]) {
    SendClientMessage(playerid, KLIZISTE_CLR_INFO, msg);
    return 1;
}

stock KlizSUCCESS(playerid, msg[]) {
    SendClientMessage(playerid, KLIZISTE_CLR_SUCCESS, msg);
    return 1;
}

stock Float:KlizDist(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2) {
    new Float:dx = x1 - x2;
    new Float:dy = y1 - y2;
    new Float:dz = z1 - z2;
    return floatsqroot(dx*dx + dy*dy + dz*dz);
}

stock KlizRandomRange(min, max) {
    return min + random(max - min + 1);
}

stock bool:KlizValidPlayer(playerid) {
    if(playerid < 0 || playerid >= MAX_PLAYERS) return false;
    if(!IsPlayerConnected(playerid)) return false;
    return true;
}

// ===========================================================================
//  INVENTORY INTEGRACIJA
// ===========================================================================

stock KlizInv_AddStone(playerid, kolicina) {
    #if defined Inventory_Add
        return Inventory_Add(playerid, KLIZISTE_ITEM_STONE, KLIZISTE_MODEL_STONE, kolicina);
    #else
        return GivePlayerWeapon(playerid, 0, 0);
    #endif
}

// ===========================================================================
//  TEXTDRAWS - Moderni Minigame Bar + Info TD
// ===========================================================================

stock Kliz_CreatePlayerTDs(playerid) {
    // =====================================================
    //  MINIGAME TDs - BOX-BASED DESIGN v5.0
    //  Umjesto teksta sa tilde bojama (koji buguje),
    //  koristimo BOX textdraws za zonu i marker.
    //  Pixel-perfect, nikad ne preliva van okvira!
    // =====================================================

    // 1. GLAVNI PANEL - tamna pozadina
    KlizPlayerData[playerid][klizMGBgPanel] = CreatePlayerTextDraw(playerid, 170.000000, 155.000000, "_");
    PlayerTextDrawBackgroundColor(playerid, KlizPlayerData[playerid][klizMGBgPanel], 0);
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGBgPanel], 1);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGBgPanel], 0.600000, 13.000000);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGBgPanel], -1);
    PlayerTextDrawSetOutline(playerid, KlizPlayerData[playerid][klizMGBgPanel], 0);
    PlayerTextDrawSetProportional(playerid, KlizPlayerData[playerid][klizMGBgPanel], 1);
    PlayerTextDrawSetShadow(playerid, KlizPlayerData[playerid][klizMGBgPanel], 0);
    PlayerTextDrawUseBox(playerid, KlizPlayerData[playerid][klizMGBgPanel], 1);
    PlayerTextDrawBoxColor(playerid, KlizPlayerData[playerid][klizMGBgPanel], 0x141428DD);
    PlayerTextDrawTextSize(playerid, KlizPlayerData[playerid][klizMGBgPanel], 470.000000, 0.000000);

    // 2. HEADER LINIJA - narandzasti accent na vrhu
    KlizPlayerData[playerid][klizMGHeaderLine] = CreatePlayerTextDraw(playerid, 170.000000, 155.000000, "_");
    PlayerTextDrawBackgroundColor(playerid, KlizPlayerData[playerid][klizMGHeaderLine], 0);
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGHeaderLine], 1);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGHeaderLine], 0.600000, 0.350000);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGHeaderLine], -1);
    PlayerTextDrawSetOutline(playerid, KlizPlayerData[playerid][klizMGHeaderLine], 0);
    PlayerTextDrawSetProportional(playerid, KlizPlayerData[playerid][klizMGHeaderLine], 1);
    PlayerTextDrawSetShadow(playerid, KlizPlayerData[playerid][klizMGHeaderLine], 0);
    PlayerTextDrawUseBox(playerid, KlizPlayerData[playerid][klizMGHeaderLine], 1);
    PlayerTextDrawBoxColor(playerid, KlizPlayerData[playerid][klizMGHeaderLine], 0xFF4500FF);
    PlayerTextDrawTextSize(playerid, KlizPlayerData[playerid][klizMGHeaderLine], 470.000000, 0.000000);

    // 3. FOOTER LINIJA - border na dnu
    KlizPlayerData[playerid][klizMGFooterLine] = CreatePlayerTextDraw(playerid, 170.000000, 261.000000, "_");
    PlayerTextDrawBackgroundColor(playerid, KlizPlayerData[playerid][klizMGFooterLine], 0);
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGFooterLine], 1);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGFooterLine], 0.600000, 0.200000);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGFooterLine], -1);
    PlayerTextDrawSetOutline(playerid, KlizPlayerData[playerid][klizMGFooterLine], 0);
    PlayerTextDrawSetProportional(playerid, KlizPlayerData[playerid][klizMGFooterLine], 1);
    PlayerTextDrawSetShadow(playerid, KlizPlayerData[playerid][klizMGFooterLine], 0);
    PlayerTextDrawUseBox(playerid, KlizPlayerData[playerid][klizMGFooterLine], 1);
    PlayerTextDrawBoxColor(playerid, KlizPlayerData[playerid][klizMGFooterLine], 0xFF450088);
    PlayerTextDrawTextSize(playerid, KlizPlayerData[playerid][klizMGFooterLine], 470.000000, 0.000000);

    // 4. NASLOV
    KlizPlayerData[playerid][klizMGTitle] = CreatePlayerTextDraw(playerid, 320.000000, 164.000000, "~y~~h~KLIZISTE ~w~MINIGAME");
    PlayerTextDrawAlignment(playerid, KlizPlayerData[playerid][klizMGTitle], 2);
    PlayerTextDrawBackgroundColor(playerid, KlizPlayerData[playerid][klizMGTitle], 0);
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGTitle], 2);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGTitle], 0.280000, 1.300000);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGTitle], 0xFFFFFFFF);
    PlayerTextDrawSetOutline(playerid, KlizPlayerData[playerid][klizMGTitle], 1);
    PlayerTextDrawSetProportional(playerid, KlizPlayerData[playerid][klizMGTitle], 1);
    PlayerTextDrawSetShadow(playerid, KlizPlayerData[playerid][klizMGTitle], 0);

    // 5. BAR TRACK - tamna pozadina trake (box)
    KlizPlayerData[playerid][klizMGBarTrack] = CreatePlayerTextDraw(playerid, KLIZISTE_BAR_VISUAL_X, 193.000000, "_");
    PlayerTextDrawBackgroundColor(playerid, KlizPlayerData[playerid][klizMGBarTrack], 0);
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGBarTrack], 1);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGBarTrack], 0.500000, 2.200000);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGBarTrack], -1);
    PlayerTextDrawSetOutline(playerid, KlizPlayerData[playerid][klizMGBarTrack], 0);
    PlayerTextDrawSetProportional(playerid, KlizPlayerData[playerid][klizMGBarTrack], 1);
    PlayerTextDrawSetShadow(playerid, KlizPlayerData[playerid][klizMGBarTrack], 0);
    PlayerTextDrawUseBox(playerid, KlizPlayerData[playerid][klizMGBarTrack], 1);
    PlayerTextDrawBoxColor(playerid, KlizPlayerData[playerid][klizMGBarTrack], 0x2a2a42CC);
    PlayerTextDrawTextSize(playerid, KlizPlayerData[playerid][klizMGBarTrack], KLIZISTE_BAR_VISUAL_END, 0.000000);

    // 6. ZONA BOX - placeholder (pravi TD se kreira dinamicki u Kliziste_CreateZoneTD)
    KlizPlayerData[playerid][klizMGBarText] = CreatePlayerTextDraw(playerid, 0.0, 0.0, "_");
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGBarText], 1);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGBarText], 0.01, 0.01);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGBarText], 0x00000000);

    // 7. STATUS - "U ZONI!" / "VANI ZONE"
    KlizPlayerData[playerid][klizMGStatus] = CreatePlayerTextDraw(playerid, 320.000000, 226.000000, "_");
    PlayerTextDrawAlignment(playerid, KlizPlayerData[playerid][klizMGStatus], 2);
    PlayerTextDrawBackgroundColor(playerid, KlizPlayerData[playerid][klizMGStatus], 0);
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGStatus], 2);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGStatus], 0.260000, 1.300000);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGStatus], 0xFFFFFFFF);
    PlayerTextDrawSetOutline(playerid, KlizPlayerData[playerid][klizMGStatus], 1);
    PlayerTextDrawSetProportional(playerid, KlizPlayerData[playerid][klizMGStatus], 1);
    PlayerTextDrawSetShadow(playerid, KlizPlayerData[playerid][klizMGStatus], 0);

    // 8. KEY HINT
    KlizPlayerData[playerid][klizMGKeyHint] = CreatePlayerTextDraw(playerid, 320.000000, 243.000000, "~w~Pritisni ~y~~h~[SPACE]~w~ u ~g~~h~zelenoj zoni");
    PlayerTextDrawAlignment(playerid, KlizPlayerData[playerid][klizMGKeyHint], 2);
    PlayerTextDrawBackgroundColor(playerid, KlizPlayerData[playerid][klizMGKeyHint], 0);
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGKeyHint], 2);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGKeyHint], 0.200000, 1.000000);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGKeyHint], 0xCCCCCCFF);
    PlayerTextDrawSetOutline(playerid, KlizPlayerData[playerid][klizMGKeyHint], 1);
    PlayerTextDrawSetProportional(playerid, KlizPlayerData[playerid][klizMGKeyHint], 1);
    PlayerTextDrawSetShadow(playerid, KlizPlayerData[playerid][klizMGKeyHint], 0);

    // 9. MARKER - placeholder (pravi TD se kreira dinamicki u Kliziste_CreateMarkerTD)
    KlizPlayerData[playerid][klizMGReward] = CreatePlayerTextDraw(playerid, 0.0, 0.0, "_");
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGReward], 1);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGReward], 0.01, 0.01);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGReward], 0x00000000);

    // === GLOBALNI STATUS TDs ===

    // Progress (desno gore)
    KlizPlayerData[playerid][klizProgressTD] = CreatePlayerTextDraw(playerid, 620.000000, 160.000000, "_");
    PlayerTextDrawAlignment(playerid, KlizPlayerData[playerid][klizProgressTD], 3);
    PlayerTextDrawBackgroundColor(playerid, KlizPlayerData[playerid][klizProgressTD], 255);
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizProgressTD], 2);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizProgressTD], 0.200000, 0.900000);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizProgressTD], 0xFF4500CC);
    PlayerTextDrawSetOutline(playerid, KlizPlayerData[playerid][klizProgressTD], 1);
    PlayerTextDrawSetProportional(playerid, KlizPlayerData[playerid][klizProgressTD], 1);
    PlayerTextDrawSetShadow(playerid, KlizPlayerData[playerid][klizProgressTD], 0);

    // Status TD (lijevo gore)
    KlizPlayerData[playerid][klizStatusTD] = CreatePlayerTextDraw(playerid, 20.000000, 130.000000, "_");
    PlayerTextDrawBackgroundColor(playerid, KlizPlayerData[playerid][klizStatusTD], 150);
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizStatusTD], 2);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizStatusTD], 0.180000, 0.900000);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizStatusTD], 0xFF4500DD);
    PlayerTextDrawSetOutline(playerid, KlizPlayerData[playerid][klizStatusTD], 1);
    PlayerTextDrawSetProportional(playerid, KlizPlayerData[playerid][klizStatusTD], 1);
    PlayerTextDrawSetShadow(playerid, KlizPlayerData[playerid][klizStatusTD], 0);
}

stock Kliz_ShowMinigame(playerid) {
    PlayerTextDrawShow(playerid, KlizPlayerData[playerid][klizMGBgPanel]);
    PlayerTextDrawShow(playerid, KlizPlayerData[playerid][klizMGHeaderLine]);
    PlayerTextDrawShow(playerid, KlizPlayerData[playerid][klizMGFooterLine]);
    PlayerTextDrawShow(playerid, KlizPlayerData[playerid][klizMGTitle]);
    PlayerTextDrawShow(playerid, KlizPlayerData[playerid][klizMGBarTrack]);
    // Zona i marker se ne prikazuju ovdje - kreiraju se dinamicki
    PlayerTextDrawShow(playerid, KlizPlayerData[playerid][klizMGStatus]);
    PlayerTextDrawShow(playerid, KlizPlayerData[playerid][klizMGKeyHint]);
    KlizPlayerData[playerid][klizTDShown] = true;
}

stock Kliz_HideMinigame(playerid) {
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizMGBgPanel]);
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizMGHeaderLine]);
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizMGFooterLine]);
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizMGTitle]);
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizMGBarTrack]);
    // Unisti dinamicku zonu i marker, vrati placeholder
    PlayerTextDrawDestroy(playerid, KlizPlayerData[playerid][klizMGBarText]);
    KlizPlayerData[playerid][klizMGBarText] = CreatePlayerTextDraw(playerid, 0.0, 0.0, "_");
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGBarText], 1);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGBarText], 0.01, 0.01);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGBarText], 0x00000000);

    PlayerTextDrawDestroy(playerid, KlizPlayerData[playerid][klizMGReward]);
    KlizPlayerData[playerid][klizMGReward] = CreatePlayerTextDraw(playerid, 0.0, 0.0, "_");
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGReward], 1);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGReward], 0.01, 0.01);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGReward], 0x00000000);

    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizMGStatus]);
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizMGKeyHint]);
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizProgressTD]);
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizStatusTD]);
    KlizPlayerData[playerid][klizTDShown] = false;
}

stock Kliz_UpdateProgressTD(playerid) {
    if(!KlizActive) return;
    new remaining = KLIZISTE_TOTAL_DIGS - KlizTotalDigs;
    if(remaining < 0) remaining = 0;
    new str[128];
    format(str, sizeof(str), "Kliziste: %d/%d~n~Preostalo: %d", KlizTotalDigs, KLIZISTE_TOTAL_DIGS, remaining);
    PlayerTextDrawSetString(playerid, KlizPlayerData[playerid][klizProgressTD], str);
    PlayerTextDrawShow(playerid, KlizPlayerData[playerid][klizProgressTD]);
}

stock Kliz_UpdateStatusTD(playerid, status[]) {
    PlayerTextDrawSetString(playerid, KlizPlayerData[playerid][klizStatusTD], status);
    PlayerTextDrawShow(playerid, KlizPlayerData[playerid][klizStatusTD]);
}

// ===========================================================================
//  MINIGAME - BOX-BASED Vizuelni prikaz (pixel-perfect!)
//  Zona = zeleni box preko bara, Marker = tanka linija
//  Nema tilde kodova, nema bugova sa tekstom!
// ===========================================================================

// Kreiraj zeleni box za zonu pogotka (jednom po minigame-u)
stock Kliziste_CreateZoneTD(playerid) {
    new Float:target = KlizPlayerData[playerid][klizMGTargetPos];
    new Float:halfZone = float(KLIZISTE_MINIGAME_ZONE) / 2.0;

    new Float:zoneStartPct = (target - halfZone) / 100.0;
    new Float:zoneEndPct = (target + halfZone) / 100.0;
    if(zoneStartPct < 0.0) zoneStartPct = 0.0;
    if(zoneEndPct > 1.0) zoneEndPct = 1.0;

    new Float:zoneStartX = KLIZISTE_BAR_VISUAL_X + zoneStartPct * KLIZISTE_BAR_VISUAL_W;
    new Float:zoneEndX = KLIZISTE_BAR_VISUAL_X + zoneEndPct * KLIZISTE_BAR_VISUAL_W;

    // Unisti stari placeholder/td
    PlayerTextDrawDestroy(playerid, KlizPlayerData[playerid][klizMGBarText]);

    // Kreiraj zeleni box na poziciji zone
    KlizPlayerData[playerid][klizMGBarText] = CreatePlayerTextDraw(playerid, zoneStartX, 194.0, "_");
    PlayerTextDrawBackgroundColor(playerid, KlizPlayerData[playerid][klizMGBarText], 0);
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGBarText], 1);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGBarText], 0.500000, 1.900000);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGBarText], 0x00000000);
    PlayerTextDrawSetOutline(playerid, KlizPlayerData[playerid][klizMGBarText], 0);
    PlayerTextDrawSetProportional(playerid, KlizPlayerData[playerid][klizMGBarText], 1);
    PlayerTextDrawSetShadow(playerid, KlizPlayerData[playerid][klizMGBarText], 0);
    PlayerTextDrawUseBox(playerid, KlizPlayerData[playerid][klizMGBarText], 1);
    PlayerTextDrawBoxColor(playerid, KlizPlayerData[playerid][klizMGBarText], 0x00FF0088); // Poluprovidna zelena
    PlayerTextDrawTextSize(playerid, KlizPlayerData[playerid][klizMGBarText], zoneEndX, 0.0);
    PlayerTextDrawShow(playerid, KlizPlayerData[playerid][klizMGBarText]);
}

// Kreiraj marker na trenutnoj poziciji (svakih 100ms)
stock Kliziste_CreateMarkerTD(playerid) {
    new Float:progress = KlizPlayerData[playerid][klizMGProgress];
    new Float:target = KlizPlayerData[playerid][klizMGTargetPos];
    new Float:halfZone = float(KLIZISTE_MINIGAME_ZONE) / 2.0;
    new bool:inZone = (progress >= (target - halfZone) && progress <= (target + halfZone));

    new Float:markerX = KLIZISTE_BAR_VISUAL_X + (progress / 100.0) * KLIZISTE_BAR_VISUAL_W;

    // Unisti stari marker
    PlayerTextDrawDestroy(playerid, KlizPlayerData[playerid][klizMGReward]);

    // Kreiraj novi marker kao tanku vertikalnu liniju (box)
    new Float:mLeft = markerX - (KLIZISTE_MARKER_WIDTH / 2.0);
    new Float:mRight = markerX + (KLIZISTE_MARKER_WIDTH / 2.0);

    KlizPlayerData[playerid][klizMGReward] = CreatePlayerTextDraw(playerid, mLeft, 193.0, "_");
    PlayerTextDrawBackgroundColor(playerid, KlizPlayerData[playerid][klizMGReward], 0);
    PlayerTextDrawFont(playerid, KlizPlayerData[playerid][klizMGReward], 1);
    PlayerTextDrawLetterSize(playerid, KlizPlayerData[playerid][klizMGReward], 0.100000, 2.200000);
    PlayerTextDrawColor(playerid, KlizPlayerData[playerid][klizMGReward], 0x00000000);
    PlayerTextDrawSetOutline(playerid, KlizPlayerData[playerid][klizMGReward], 0);
    PlayerTextDrawSetProportional(playerid, KlizPlayerData[playerid][klizMGReward], 1);
    PlayerTextDrawSetShadow(playerid, KlizPlayerData[playerid][klizMGReward], 0);
    PlayerTextDrawUseBox(playerid, KlizPlayerData[playerid][klizMGReward], 1);
    // Boja markera zavisi da li je u zoni
    if(inZone) {
        PlayerTextDrawBoxColor(playerid, KlizPlayerData[playerid][klizMGReward], 0xFFFF00FF); // Zuta - u zoni
    } else {
        PlayerTextDrawBoxColor(playerid, KlizPlayerData[playerid][klizMGReward], 0xFF4444FF); // Crvena - van zone
    }
    PlayerTextDrawTextSize(playerid, KlizPlayerData[playerid][klizMGReward], mRight, 0.0);
    PlayerTextDrawShow(playerid, KlizPlayerData[playerid][klizMGReward]);

    // Azuriraj status tekst
    if(inZone) {
        PlayerTextDrawSetString(playerid, KlizPlayerData[playerid][klizMGStatus], "~g~~h~U ZONI! ~w~> ~y~~h~STISNI SPACE! ~w~<");
    } else {
        PlayerTextDrawSetString(playerid, KlizPlayerData[playerid][klizMGStatus], "~r~~h~VANI ZONE~w~ - Cekaj zeleni");
    }
    PlayerTextDrawShow(playerid, KlizPlayerData[playerid][klizMGStatus]);
}

// ===========================================================================
//  INICIJALIZACIJA SISTEMA
// ===========================================================================

stock Kliziste_Init() {
    KlizNextActivation = GetTickCount() + (KLIZISTE_AUTO_INTERVAL * 1000);
    KlizAutoTimer = SetTimer("KlizTimer_AutoActivation", KLIZISTE_AUTO_INTERVAL * 1000, true);

    print("[KLIZISTE] Sistem klizista v2.0 uspjesno ucitan!");
    print("[KLIZISTE] Auto-aktivacija svakih 2 sata. Admin: /pokrenikliziste, /zaustavikliziste, /klizistetime");
    print("[KLIZISTE] Igraci: /prekidakliziste, /skiniopremu");
    return 1;
}

// ===========================================================================
//  AKTIVACIJA / DEAKTIVACIJA KLIZISTA
// ===========================================================================

stock Kliziste_Activate() {
    if(KlizActive) return 0;

    KlizActive = true;
    KlizTotalDigs = 0;

    // Kreiraj opremu pickup i 3D label na lokaciji
    KlizPickup_Location = CreateDynamicPickup(19631, 23, KLIZISTE_LOC_X, KLIZISTE_LOC_Y, KLIZISTE_LOC_Z);
    Kliz3D_Location = CreateDynamic3DTextLabel("{FF4500}[KLIZISTE - OPREMA]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} da uzmes alat\nStisni {FFD700}ALT{FFFFFF} sa opremom da je vratis",
        0xFFFFFFFF, KLIZISTE_LOC_X, KLIZISTE_LOC_Y, KLIZISTE_LOC_Z + 0.5, 15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1);

    // Map icon
    KlizMapIcon = CreateDynamicMapIcon(KLIZISTE_LOC_X, KLIZISTE_LOC_Y, KLIZISTE_LOC_Z, 55, 0);

    // Kreiraj stijene na autoputu (iz kliziste_mapa.inc)
    #if defined Kliziste_CreateMapObjects
        Kliziste_CreateMapObjects();
    #endif

    // UPOZORENJE za sve igrace - crveni GameText
    GameTextForAll("~r~UPOZORENJE~n~~w~AKTIVIRANO JE KLIZISTE~n~NA AUTOPUTU~n~Beograd - Sarajevo~n~~r~CESTA JE ZATVORENA", 10000, 3);

    // Tresti ekran svim igracima - koristimo SAMO DrunkLevel (bez SetPlayerCameraPos)
    // SetPlayerCameraPos izaziva bug gdje kamera ostane zaglavljena
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(!IsPlayerConnected(i)) continue;
        if(IsPlayerNPC(i)) continue;

        // DrunkLevel daje prirodan shake efekat kamere
        SetPlayerDrunkLevel(i, KLIZISTE_SHAKE_LEVEL);
        PlayerPlaySound(i, KLIZISTE_SOUND_START, 0.0, 0.0, 0.0);

        // Postavi timer za kraj trezanja i vracanje kamere
        SetTimerEx("KlizTimer_ShakeEnd", KLIZISTE_SHAKE_DURATION, false, "i", i);

        // Prikazi progress TD svim igracima
        Kliz_UpdateProgressTD(i);
    }

    // Poruka svim igracima
    SendClientMessageToAll(KLIZISTE_CLR_MSG, "{FF4500}[KLIZISTE]{FFFFFF} Aktivirano je kliziste na autoputu Beograd - Sarajevo!");
    SendClientMessageToAll(KLIZISTE_CLR_INFO, "{FFD700}[KLIZISTE]{FFFFFF} Cesta je zatvorena! Dodjite na lokaciju i pomozite ocistiti kliziste.");
    SendClientMessageToAll(KLIZISTE_CLR_INFO, "{FFD700}[KLIZISTE]{FFFFFF} Stisnite ALT na lokaciji opreme da uzmete alat.");

    print("[KLIZISTE] Kliziste aktivirano! Autoput Beograd-Sarajevo zatvoren.");
    return 1;
}

stock Kliziste_Deactivate() {
    if(!KlizActive) return 0;

    KlizActive = false;

    // Unisti opremu pickup i 3D label
    if(KlizPickup_Location != -1) {
        DestroyDynamicPickup(KlizPickup_Location);
        KlizPickup_Location = -1;
    }
    if(Kliz3D_Location != Text3D:-1) {
        DestroyDynamic3DTextLabel(Kliz3D_Location);
        Kliz3D_Location = Text3D:-1;
    }

    // Unisti map icon
    if(KlizMapIcon != -1) {
        DestroyDynamicMapIcon(KlizMapIcon);
        KlizMapIcon = -1;
    }

    // Unisti stijene (iz kliziste_mapa.inc)
    #if defined Kliziste_DestroyMapObjects
        Kliziste_DestroyMapObjects();
    #endif

    // Resetuj sve igrace koji rade na klizistu
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(!IsPlayerConnected(i)) continue;
        if(KlizPlayerData[i][klizPState] != KLIZISTE_PSTATE_NONE) {
            Kliziste_StopPlayer(i);
            KlizMSG(i, "{FF4500}[KLIZISTE]{FFFFFF} Kliziste je zaustavljeno. Vratili ste se u normalno stanje.");
        }

        // Unisti per-player iskopinu label/pickup
        if(KlizDigLabel[i] != Text3D:-1) {
            DestroyDynamic3DTextLabel(KlizDigLabel[i]);
            KlizDigLabel[i] = Text3D:-1;
        }
        if(KlizDigPickup[i] != -1) {
            DestroyDynamicPickup(KlizDigPickup[i]);
            KlizDigPickup[i] = -1;
        }

        // Sakrij TDs
        Kliz_HideMinigame(i);

        // Resetuj DrunkLevel ako je jos uvijek povecan
        if(GetPlayerDrunkLevel(i) > 0) {
            SetPlayerDrunkLevel(i, 0);
            SetCameraBehindPlayer(i);
        }

        // Zaustavi shake timere
        if(KlizShakeTimers[i] != -1) {
            KillTimer(KlizShakeTimers[i]);
            KlizShakeTimers[i] = -1;
        }
        KlizShakeTicks[i] = 0;
    }

    // Obavijesti sve
    if(KlizTotalDigs >= KLIZISTE_TOTAL_DIGS) {
        GameTextForAll("~g~KLIZISTE OCISCENO!~n~~w~Autoput Beograd-Sarajevo~n~je ponovo otvoren!", 8000, 3);
        SendClientMessageToAll(KLIZISTE_CLR_SUCCESS, "{00FF00}[KLIZISTE]{FFFFFF} Kliziste je ocisceno! Autoput Beograd-Sarajevo je ponovo otvoren!");
        for(new j = 0; j < MAX_PLAYERS; j++) {
            if(!IsPlayerConnected(j)) continue;
            if(IsPlayerNPC(j)) continue;
            PlayerPlaySound(j, KLIZISTE_SOUND_CLEAR, 0.0, 0.0, 0.0);
        }
    }

    KlizTotalDigs = 0;

    // Postavi novi timer za auto-aktivaciju
    KlizNextActivation = GetTickCount() + (KLIZISTE_AUTO_INTERVAL * 1000);

    print("[KLIZISTE] Kliziste deaktivirano.");
    return 1;
}

// Zaustavi igraca - kompletni reset
stock Kliziste_StopPlayer(playerid) {
    // Ubij minigame timer
    if(KlizPlayerData[playerid][klizMGTimer] != -1) {
        KillTimer(KlizPlayerData[playerid][klizMGTimer]);
        KlizPlayerData[playerid][klizMGTimer] = -1;
    }

    // Unisti per-player iskopinu label/pickup
    if(KlizDigLabel[playerid] != Text3D:-1) {
        DestroyDynamic3DTextLabel(KlizDigLabel[playerid]);
        KlizDigLabel[playerid] = Text3D:-1;
    }
    if(KlizDigPickup[playerid] != -1) {
        DestroyDynamicPickup(KlizDigPickup[playerid]);
        KlizDigPickup[playerid] = -1;
    }

    // Sakrij TDs
    Kliz_HideMinigame(playerid);

    // Ukloni checkpoint
    DisablePlayerCheckpoint(playerid);

    // Vrati skin
    if(KlizPlayerData[playerid][klizHasEquipment] && KlizPlayerData[playerid][klizOldSkin] != -1) {
        SetPlayerSkin(playerid, KlizPlayerData[playerid][klizOldSkin]);
    }

    // Skini alat
    ResetPlayerWeapons(playerid);

    // Clear animacije
    ClearAnimations(playerid);

    // Unfreeze
    TogglePlayerControllable(playerid, true);

    // Reset stanja
    KlizPlayerData[playerid][klizPState] = KLIZISTE_PSTATE_NONE;
    KlizPlayerData[playerid][klizHasEquipment] = false;
    KlizPlayerData[playerid][klizOldSkin] = -1;
    KlizPlayerData[playerid][klizCurrentSpot] = -1;
    KlizPlayerData[playerid][klizMinigameActive] = false;

    // Ukloni PVar
    DeletePVar(playerid, "KlizisteWorking");
}

// ===========================================================================
//  UZIMANJE / VRACANJE OPREME
// ===========================================================================

stock Kliziste_UzmiOpremu(playerid) {
    if(!KlizActive) {
        return KlizGRESKA(playerid, "Kliziste trenutno nije aktivno.");
    }
    if(KlizPlayerData[playerid][klizHasEquipment]) {
        return KlizGRESKA(playerid, "Vec imate opremu! Idite do iskopine i stisnite ALT.");
    }
    if(KlizPlayerData[playerid][klizPState] != KLIZISTE_PSTATE_NONE) {
        return KlizGRESKA(playerid, "Vec ste u procesu rada na klizistu.");
    }
    if(IsPlayerInAnyVehicle(playerid)) {
        return KlizGRESKA(playerid, "Ne mozete uzeti opremu dok ste u vozilu.");
    }

    // Provjera udaljenosti
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    if(KlizDist(x, y, z, KLIZISTE_LOC_X, KLIZISTE_LOC_Y, KLIZISTE_LOC_Z) > KLIZISTE_OPREMA_DIST) {
        return KlizGRESKA(playerid, "Niste na lokaciji za uzimanje opreme.");
    }

    // Postavi opremu
    KlizPlayerData[playerid][klizOldSkin] = GetPlayerSkin(playerid);
    SetPlayerSkin(playerid, KLIZISTE_OPREMA_SKIN);
    KlizPlayerData[playerid][klizHasEquipment] = true;
    KlizPlayerData[playerid][klizPState] = KLIZISTE_PSTATE_EQUIPPED;

    // Daj alat (shovel)
    GivePlayerWeapon(playerid, KLIZISTE_WEAPON_ALAT, 1);

    // Anti-cheat PVar
    SetPVarInt(playerid, "KlizisteWorking", 1);

    // Postavi prvu iskopinu
    Kliziste_SetNextDigSpot(playerid);

    // Zvuk i poruka
    PlayerPlaySound(playerid, KLIZISTE_SOUND_OPREMA, 0.0, 0.0, 0.0);
    KlizMSG(playerid, "{FF4500}[KLIZISTE]{FFFFFF} Uzeli ste alat za ciscenje klizista!");
    KlizINFO(playerid, "Idite do iskopine i stisnite ALT da pocnete kopati.");
    KlizINFO(playerid, "Kucaj /prekidakliziste ili /skiniopremu da prestanete raditi.");
    KlizINFO(playerid, "Vrati se na lokaciju opreme i stisni ALT da vratis opremu.");

    // Status TD
    Kliz_UpdateStatusTD(playerid, "{FF4500}KLIZISTE~n~~w~Status: Idi do iskopine");

    return 1;
}

// Vracanje opreme - skidanje opreme na lokaciji
stock Kliziste_SkinjiOpremu(playerid) {
    if(!KlizPlayerData[playerid][klizHasEquipment] && KlizPlayerData[playerid][klizPState] == KLIZISTE_PSTATE_NONE) {
        return KlizGRESKA(playerid, "Nemate opremu za skinuti.");
    }

    // Ako je u minigame, ne dozvoli
    if(KlizPlayerData[playerid][klizMinigameActive]) {
        return KlizGRESKA(playerid, "Ne mozete skinuti opremu tokom minigame-a!");
    }

    // Provjera udaljenosti od lokacije opreme
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    if(KlizDist(x, y, z, KLIZISTE_LOC_X, KLIZISTE_LOC_Y, KLIZISTE_LOC_Z) > KLIZISTE_OPREMA_DIST + 2.0) {
        return KlizGRESKA(playerid, "Morate biti na lokaciji opreme da je vratite!");
    }

    // Zaustavi igraca (vrati skin, skini alat, itd.)
    Kliziste_StopPlayer(playerid);

    // Zvuk i poruka
    PlayerPlaySound(playerid, KLIZISTE_SOUND_OPREMA, 0.0, 0.0, 0.0);
    KlizMSG(playerid, "{FF4500}[KLIZISTE]{FFFFFF} Vratili ste opremu i odjavili se sa klizista.");

    // Sakrij status TD
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizStatusTD]);
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizProgressTD]);

    return 1;
}

// ===========================================================================
//  SEKVENCIJALNE ISKOPINE - jedna po jedna
// ===========================================================================

stock Kliziste_SetNextDigSpot(playerid) {
    // Unisti prethodnu iskopinu label/pickup
    if(KlizDigLabel[playerid] != Text3D:-1) {
        DestroyDynamic3DTextLabel(KlizDigLabel[playerid]);
        KlizDigLabel[playerid] = Text3D:-1;
    }
    if(KlizDigPickup[playerid] != -1) {
        DestroyDynamicPickup(KlizDigPickup[playerid]);
        KlizDigPickup[playerid] = -1;
    }

    // Random odabir sljedece iskopine
    new spotIdx = random(KLIZISTE_DIG_SPOTS);
    KlizPlayerData[playerid][klizCurrentSpot] = spotIdx;

    new Float:sx = KlizDigCoords[spotIdx][0];
    new Float:sy = KlizDigCoords[spotIdx][1];
    new Float:sz = KlizDigCoords[spotIdx][2];

    // Kreiraj 3D label na iskopini (samo za ovog igraca)
    new labelTxt[128];
    format(labelTxt, sizeof(labelTxt), "{FFD700}[ISKOPINA]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} da pocnes kopati");
    KlizDigLabel[playerid] = CreateDynamic3DTextLabel(labelTxt,
        0xFFFFFFFF, sx, sy, sz + 0.5, 15.0, playerid, INVALID_VEHICLE_ID, 0, -1, -1);

    // Kreiraj pickup marker na iskopini (samo za ovog igraca)
    KlizDigPickup[playerid] = CreateDynamicPickup(1318, 23, sx, sy, sz, -1, -1, playerid);

    // Postavi checkpoint ka iskopini
    SetPlayerCheckpoint(playerid, sx, sy, sz, 2.5);

    // Poruka
    KlizINFO(playerid, "{FFD700}[KLIZISTE]{FFFFFF} Slijedeca iskopina! Idite do zutog markera.");
}

// ===========================================================================
//  MINIGAME - Horizontalni bar sa zelenom zonom + SPACE
// ===========================================================================

stock Kliziste_StartMinigame(playerid) {
    if(!KlizActive) {
        return KlizGRESKA(playerid, "Kliziste vise nije aktivno.");
    }
    if(KlizPlayerData[playerid][klizPState] != KLIZISTE_PSTATE_EQUIPPED) {
        return KlizGRESKA(playerid, "Ne mozete sada poceti kopati.");
    }
    if(KlizPlayerData[playerid][klizMinigameActive]) {
        return KlizGRESKA(playerid, "Vec igrate minigame! Stisnite SPACE na vrijeme.");
    }
    if(KlizPlayerData[playerid][klizCurrentSpot] == -1) {
        return KlizGRESKA(playerid, "Niste na iskopini.");
    }

    // Provjera udaljenosti od trenutne iskopine
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    new spotIdx = KlizPlayerData[playerid][klizCurrentSpot];
    if(KlizDist(x, y, z, KlizDigCoords[spotIdx][0], KlizDigCoords[spotIdx][1], KlizDigCoords[spotIdx][2]) > KLIZISTE_DIG_DIST) {
        return KlizGRESKA(playerid, "Niste blizu iskopine! Slijedite marker.");
    }

    if(IsPlayerInAnyVehicle(playerid)) {
        return KlizGRESKA(playerid, "Ne mozete kopati dok ste u vozilu.");
    }

    // Postavi stanje
    KlizPlayerData[playerid][klizPState] = KLIZISTE_PSTATE_DIGGING;
    KlizPlayerData[playerid][klizMinigameActive] = true;

    // Random pocetna pozicija markera (0.0 - 100.0)
    KlizPlayerData[playerid][klizMGProgress] = float(random(100));

    // Random ciljna zona (0.0 - 100.0) - osiguraj da zona stane u bar
    new Float:halfZone = float(KLIZISTE_MINIGAME_ZONE) / 2.0;
    new Float:minTarget = halfZone + 5.0;
    new Float:maxTarget = 100.0 - halfZone - 5.0;
    if(minTarget >= maxTarget) minTarget = 50.0;
    new Float:targetRange = maxTarget - minTarget;
    if(targetRange < 1.0) targetRange = 1.0;
    KlizPlayerData[playerid][klizMGTargetPos] = minTarget + float(random(floatround(targetRange)));

    // Random pravac kretanja
    KlizPlayerData[playerid][klizMGDirection] = (random(2) == 0) ? 1.0 : -1.0;

    // Freeze igraca tokom minigame
    TogglePlayerControllable(playerid, false);

    // Zvuk kopanja
    PlayerPlaySound(playerid, KLIZISTE_SOUND_DIG, 0.0, 0.0, 0.0);

    // Postavi animaciju kopanja
    ApplyAnimation(playerid, "BASEBALL", "Bat_1", 4.0, 1, 0, 0, 0, 0);

    // Prikazi minigame TDs i kreiraj zonu + marker
    Kliz_ShowMinigame(playerid);
    Kliziste_CreateZoneTD(playerid);
    Kliziste_CreateMarkerTD(playerid);

    // Pokreni timer za kretanje markera (svakih 100ms)
    KlizPlayerData[playerid][klizMGTimer] = SetTimerEx("KlizTimer_MGRotate", 100, true, "i", playerid);

    // Status TD
    Kliz_UpdateStatusTD(playerid, "{FF4500}KLIZISTE~n~~w~MINIGAME ~y~STISNI SPACE!");

    return 1;
}

// Igrac pritisnuo SPACE tokom minigame - provjera pogotka
stock Kliziste_MinigamePress(playerid) {
    if(!KlizPlayerData[playerid][klizMinigameActive]) return 0;

    new Float:progress = KlizPlayerData[playerid][klizMGProgress];
    new Float:target = KlizPlayerData[playerid][klizMGTargetPos];
    new Float:halfZone = float(KLIZISTE_MINIGAME_ZONE) / 2.0;

    // Izracunaj da li je marker u zoni
    new bool:inZone = (progress >= (target - halfZone) && progress <= (target + halfZone));

    // Zaustavi minigame
    KillTimer(KlizPlayerData[playerid][klizMGTimer]);
    KlizPlayerData[playerid][klizMGTimer] = -1;
    KlizPlayerData[playerid][klizMinigameActive] = false;

    // Unfreeze
    TogglePlayerControllable(playerid, true);
    ClearAnimations(playerid);

    // Sakrij minigame TDs
    Kliz_HideMinigame(playerid);

    if(inZone) {
        Kliziste_MinigameSuccess(playerid);
    } else {
        Kliziste_MinigameFail(playerid);
    }
    return 1;
}

stock Kliziste_MinigameSuccess(playerid) {
    // Povecaj globalni brojac
    KlizTotalDigs++;

    // Daj 1-3 kamena u inventory
    new stones = KlizRandomRange(KLIZISTE_STONE_MIN, KLIZISTE_STONE_MAX);
    KlizInv_AddStone(playerid, stones);

    // Daj random novac $88-$400 sa vecom sansom za nize iznose
    // 70% sansa za $88-180, 20% za $180-300, 10% za $300-400
    new payment;
    new roll = random(100);
    if(roll < 70) {
        payment = KlizRandomRange(88, 180);      // Najcesci opseg
    } else if(roll < 90) {
        payment = KlizRandomRange(180, 300);     // Srednji opseg
    } else {
        payment = KlizRandomRange(300, 400);     // Rijetki veci iznos
    }
    GivePlayerMoney(playerid, payment);

    // Zvuk uspjeha
    PlayerPlaySound(playerid, KLIZISTE_SOUND_SUCCESS, 0.0, 0.0, 0.0);

    // Vizuelni efekt uspjeha - GameText flash
    GameTextForPlayer(playerid, "~g~~h~USPJEH!", 1500, 4);

    new str[156];
    format(str, sizeof(str), "{00FF00}[KLIZISTE]{FFFFFF} Uspjesno iskopano! Dobili ste {FFD700}%d Kamen(a){FFFFFF} i {2E8B57}$%d{FFFFFF}.", stones, payment);
    KlizSUCCESS(playerid, str);

    // Progress poruka
    new remaining = KLIZISTE_TOTAL_DIGS - KlizTotalDigs;
    if(remaining < 0) remaining = 0;
    format(str, sizeof(str), "{FF4500}[KLIZISTE]{FFFFFF} Progress: {FFD700}%d/%d{FFFFFF} - Preostalo jos: {FFD700}%d", KlizTotalDigs, KLIZISTE_TOTAL_DIGS, remaining);
    KlizINFO(playerid, str);

    // Azuriraj progress TD svim igracima
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(!IsPlayerConnected(i)) continue;
        if(KlizActive) Kliz_UpdateProgressTD(i);
    }

    // Provjera da li je kliziste ocisceno
    if(KlizTotalDigs >= KLIZISTE_TOTAL_DIGS) {
        // Daj bonus $10,000 igracu koji je posljednji iskopao
        GivePlayerMoney(playerid, KLIZISTE_BONUS_AMOUNT);
        new bonusStr[128];
        format(bonusStr, sizeof(bonusStr), "{FFD700}[KLIZISTE]{FFFFFF} Bonus za posljednji iskop: {2E8B57}$%d{FFFFFF}!", KLIZISTE_BONUS_AMOUNT);
        KlizSUCCESS(playerid, bonusStr);

        // Daj bonus SVIM igracima koji su radili na klizistu
        for(new b = 0; b < MAX_PLAYERS; b++) {
            if(!IsPlayerConnected(b)) continue;
            if(b == playerid) continue;
            if(GetPVarInt(b, "KlizisteWorking") == 1) {
                GivePlayerMoney(b, KLIZISTE_BONUS_AMOUNT / 2);
            }
        }

        Kliziste_Deactivate();
        return 1;
    }

    // Odmah postavi novu iskopinu - bez cekanja
    KlizPlayerData[playerid][klizPState] = KLIZISTE_PSTATE_EQUIPPED;
    Kliziste_SetNextDigSpot(playerid);

    // Status TD
    Kliz_UpdateStatusTD(playerid, "{FF4500}KLIZISTE~n~~g~USPJEH! ~w~Idi na novu iskopinu");

    // Pokazi progress TD i status TD i dalje
    Kliz_UpdateProgressTD(playerid);
}

stock Kliziste_MinigameFail(playerid) {
    // Zvuk neuspjeha
    PlayerPlaySound(playerid, KLIZISTE_SOUND_FAIL, 0.0, 0.0, 0.0);

    // Vizuelni efekt neuspjeha - GameText flash
    GameTextForPlayer(playerid, "~r~~h~PROMASAJ!", 1500, 4);

    KlizGRESKA(playerid, "{FF4500}[KLIZISTE]{FFFFFF} Promasili ste! Pokusajte ponovo (stisni ALT).");

    // Odmah moze probati ponovo - bez cekanja
    KlizPlayerData[playerid][klizPState] = KLIZISTE_PSTATE_EQUIPPED;

    // Status TD
    Kliz_UpdateStatusTD(playerid, "{FF4500}KLIZISTE~n~~r~PROMASAJ! ~w~Pokusaj ponovo");

    // Pokazi progress TD i status TD i dalje
    Kliz_UpdateProgressTD(playerid);
}

// ===========================================================================
//  TIMERS
// ===========================================================================

// Auto-aktivacija klizista
public KlizTimer_AutoActivation() {
    if(!KlizActive) {
        Kliziste_Activate();
    }
    KlizNextActivation = GetTickCount() + (KLIZISTE_AUTO_INTERVAL * 1000);
}

// Kretanje markera u minigame
public KlizTimer_MGRotate(playerid) {
    if(!KlizValidPlayer(playerid)) return 0;
    if(!KlizPlayerData[playerid][klizMinigameActive]) {
        KillTimer(KlizPlayerData[playerid][klizMGTimer]);
        KlizPlayerData[playerid][klizMGTimer] = -1;
        return 0;
    }

    // Pomjeri marker (0-100 skala, ping-pong)
    KlizPlayerData[playerid][klizMGProgress] += KLIZISTE_MINIGAME_SPEED * KlizPlayerData[playerid][klizMGDirection];

    // Ping-pong: kad dodje do kraja, promijeni pravac
    if(KlizPlayerData[playerid][klizMGProgress] >= 100.0) {
        KlizPlayerData[playerid][klizMGProgress] = 100.0;
        KlizPlayerData[playerid][klizMGDirection] = -1.0;
    }
    if(KlizPlayerData[playerid][klizMGProgress] <= 0.0) {
        KlizPlayerData[playerid][klizMGProgress] = 0.0;
        KlizPlayerData[playerid][klizMGDirection] = 1.0;
    }

    // Azuriraj marker poziciju (destroy + recreate)
    Kliziste_CreateMarkerTD(playerid);
    return 1;
}

// Kraj trezanja ekrana - resetujemo DrunkLevel i vracamo kameru
public KlizTimer_ShakeEnd(playerid) {
    if(!KlizValidPlayer(playerid)) return 0;

    // Resetuj DrunkLevel - ovo zaustavlja shake
    SetPlayerDrunkLevel(playerid, 0);

    // Vrati kameru iza igraca - MORA se pozvati jer DrunkLevel moze ostaviti
    // kameru u cudnom polozaju na nekim klijentima
    SetCameraBehindPlayer(playerid);

    // Zaustavi shake tick timer ako je jos aktivan
    if(KlizShakeTimers[playerid] != -1) {
        KillTimer(KlizShakeTimers[playerid]);
        KlizShakeTimers[playerid] = -1;
    }
    KlizShakeTicks[playerid] = 0;
    return 1;
}

// Cooldown nakon uspjesnog/neuspjesnog minigame
public KlizTimer_Cooldown(playerid) {
    if(!KlizValidPlayer(playerid)) return 0;
    if(KlizPlayerData[playerid][klizPState] != KLIZISTE_PSTATE_COOLDOWN) return 0;

    if(!KlizActive) {
        Kliziste_StopPlayer(playerid);
        return 0;
    }

    // Vrati u equipped stanje i postavi novu iskopinu
    KlizPlayerData[playerid][klizPState] = KLIZISTE_PSTATE_EQUIPPED;
    Kliziste_SetNextDigSpot(playerid);

    // Status TD
    Kliz_UpdateStatusTD(playerid, "{FF4500}KLIZISTE~n~~w~Status: Idi do iskopine");

    return 1;
}

// ===========================================================================
//  ONKEYSTATECHANGE - ALT za opremu/iskopinu, SPACE za minigame
// ===========================================================================

stock Kliziste_OnKeyStateChange(playerid, newkeys, oldkeys) {
    if(!KlizActive) return 0;

    // ALT (KEY_WALK) za uzimanje opreme, vracanje opreme ili pocetak kopanja
    if((newkeys & KEY_WALK) && !(oldkeys & KEY_WALK)) {
        // Debounce
        new now = GetTickCount();
        if(now - KlizPlayerData[playerid][klizLastAltPress] < 800) return 1;
        KlizPlayerData[playerid][klizLastAltPress] = now;

        // Ako nema opremu - pokusaj uzeti
        if(!KlizPlayerData[playerid][klizHasEquipment] && KlizPlayerData[playerid][klizPState] == KLIZISTE_PSTATE_NONE) {
            Kliziste_UzmiOpremu(playerid);
            return 1;
        }

        // Ako ima opremu i blizu lokacije opreme - vrati opremu (skini)
        if(KlizPlayerData[playerid][klizHasEquipment] && !KlizPlayerData[playerid][klizMinigameActive]) {
            new Float:x, Float:y, Float:z;
            GetPlayerPos(playerid, x, y, z);
            if(KlizDist(x, y, z, KLIZISTE_LOC_X, KLIZISTE_LOC_Y, KLIZISTE_LOC_Z) <= KLIZISTE_OPREMA_DIST + 2.0) {
                Kliziste_SkinjiOpremu(playerid);
                return 1;
            }
        }

        // Ako ima opremu i na iskopini je - pokreni minigame
        if(KlizPlayerData[playerid][klizPState] == KLIZISTE_PSTATE_EQUIPPED && KlizPlayerData[playerid][klizCurrentSpot] != -1) {
            Kliziste_StartMinigame(playerid);
            return 1;
        }
    }

    // KEY_HANDBRAKE kao alternativa za ALT u vozilu
    if((newkeys & KEY_HANDBRAKE) && !(oldkeys & KEY_HANDBRAKE)) {
        new now2 = GetTickCount();
        if(now2 - KlizPlayerData[playerid][klizLastAltPress] < 800) return 1;
        KlizPlayerData[playerid][klizLastAltPress] = now2;

        if(!KlizPlayerData[playerid][klizHasEquipment] && KlizPlayerData[playerid][klizPState] == KLIZISTE_PSTATE_NONE) {
            Kliziste_UzmiOpremu(playerid);
            return 1;
        }
        if(KlizPlayerData[playerid][klizHasEquipment] && !KlizPlayerData[playerid][klizMinigameActive]) {
            new Float:x, Float:y, Float:z;
            GetPlayerPos(playerid, x, y, z);
            if(KlizDist(x, y, z, KLIZISTE_LOC_X, KLIZISTE_LOC_Y, KLIZISTE_LOC_Z) <= KLIZISTE_OPREMA_DIST + 2.0) {
                Kliziste_SkinjiOpremu(playerid);
                return 1;
            }
        }
        if(KlizPlayerData[playerid][klizPState] == KLIZISTE_PSTATE_EQUIPPED && KlizPlayerData[playerid][klizCurrentSpot] != -1) {
            Kliziste_StartMinigame(playerid);
            return 1;
        }
    }

    // SPACE (KEY_SPRINT) za minigame pogodak
    if((newkeys & KEY_SPRINT) && !(oldkeys & KEY_SPRINT)) {
        if(KlizPlayerData[playerid][klizMinigameActive]) {
            Kliziste_MinigamePress(playerid);
            return 1;
        }
    }

    return 0;
}

// ===========================================================================
//  ONPLAYERCONNECT / DISCONNECT
// ===========================================================================

stock Kliziste_OnConnect(playerid) {
    KlizPlayerData[playerid][klizPState] = KLIZISTE_PSTATE_NONE;
    KlizPlayerData[playerid][klizHasEquipment] = false;
    KlizPlayerData[playerid][klizOldSkin] = -1;
    KlizPlayerData[playerid][klizCurrentSpot] = -1;
    KlizPlayerData[playerid][klizCooldownTime] = 0;
    KlizPlayerData[playerid][klizMinigameActive] = false;
    KlizPlayerData[playerid][klizMGProgress] = 0.0;
    KlizPlayerData[playerid][klizMGTargetPos] = 50.0;
    KlizPlayerData[playerid][klizMGDirection] = 1.0;
    KlizPlayerData[playerid][klizMGTimer] = -1;
    KlizPlayerData[playerid][klizTDShown] = false;
    KlizPlayerData[playerid][klizLastAltPress] = 0;

    KlizDigLabel[playerid] = Text3D:-1;
    KlizDigPickup[playerid] = -1;

    Kliz_CreatePlayerTDs(playerid);

    if(KlizActive) {
        Kliz_UpdateProgressTD(playerid);
    }
    return 1;
}

stock Kliziste_OnDisconnect(playerid) {
    // Ubij minigame timer
    if(KlizPlayerData[playerid][klizMGTimer] != -1) {
        KillTimer(KlizPlayerData[playerid][klizMGTimer]);
        KlizPlayerData[playerid][klizMGTimer] = -1;
    }

    // Unisti per-player iskopinu label/pickup
    if(KlizDigLabel[playerid] != Text3D:-1) {
        DestroyDynamic3DTextLabel(KlizDigLabel[playerid]);
        KlizDigLabel[playerid] = Text3D:-1;
    }
    if(KlizDigPickup[playerid] != -1) {
        DestroyDynamicPickup(KlizDigPickup[playerid]);
        KlizDigPickup[playerid] = -1;
    }

    // Sakrij TDs
    Kliz_HideMinigame(playerid);

    // Ubij shake timer
    if(KlizShakeTimers[playerid] != -1) {
        KillTimer(KlizShakeTimers[playerid]);
        KlizShakeTimers[playerid] = -1;
    }
    KlizShakeTicks[playerid] = 0;

    // Reset
    KlizPlayerData[playerid][klizPState] = KLIZISTE_PSTATE_NONE;
    KlizPlayerData[playerid][klizHasEquipment] = false;
    KlizPlayerData[playerid][klizMinigameActive] = false;

    DeletePVar(playerid, "KlizisteWorking");
    return 1;
}

stock Kliziste_OnDeath(playerid) {
    if(KlizPlayerData[playerid][klizPState] != KLIZISTE_PSTATE_NONE) {
        Kliziste_StopPlayer(playerid);
        KlizGRESKA(playerid, "Prekinuli ste rad na klizistu jer ste umrli.");
    }
    return 1;
}

// ===========================================================================
//  ONPLAYERUPDATE
// ===========================================================================

stock Kliziste_OnUpdate(playerid) {
    if(!KlizActive) return 0;

    if(KlizPlayerData[playerid][klizPState] != KLIZISTE_PSTATE_NONE) {
        // Throttle TD updates to every 500ms instead of per-frame
        new tick = GetTickCount();
        if(tick - KlizPlayerData[playerid][klizLastTDUpdate] >= 500) {
            KlizPlayerData[playerid][klizLastTDUpdate] = tick;
            Kliz_UpdateProgressTD(playerid);
        }
    }

    return 0;
}

// ===========================================================================
//  ADMIN HELPER FUNKCIJE (komande su u fg-ogc.pwn)
// ===========================================================================

stock Kliziste_AdminActivate(playerid) {
    if(KlizActive) return 0;
    Kliziste_Activate();
    new admname[MAX_PLAYER_NAME];
    GetPlayerName(playerid, admname, sizeof(admname));
    new str[128];
    format(str, sizeof(str), "{FF4500}[KLIZISTE]{FFFFFF} Admin %s je rucno pokrenuo kliziste!", admname);
    SendClientMessageToAll(KLIZISTE_CLR_MSG, str);
    return 1;
}

stock Kliziste_AdminDeactivate(playerid) {
    if(!KlizActive) return 0;
    Kliziste_Deactivate();
    new admname[MAX_PLAYER_NAME];
    GetPlayerName(playerid, admname, sizeof(admname));
    new str[128];
    format(str, sizeof(str), "{FF4500}[KLIZISTE]{FFFFFF} Admin %s je zaustavio kliziste.", admname);
    SendClientMessageToAll(KLIZISTE_CLR_MSG, str);
    return 1;
}

stock Kliziste_GetTimeInfo(playerid) {
    new str[128];
    if(KlizActive) {
        new remaining = KLIZISTE_TOTAL_DIGS - KlizTotalDigs;
        if(remaining < 0) remaining = 0;
        format(str, sizeof(str), "Kliziste je aktivno! Progress: %d/%d (preostalo: %d)", KlizTotalDigs, KLIZISTE_TOTAL_DIGS, remaining);
    } else {
        new preostaloSek = (KlizNextActivation - GetTickCount()) / 1000;
        if(preostaloSek < 0) preostaloSek = 0;
        new minuta = preostaloSek / 60;
        new sekundi = preostaloSek % 60;
        format(str, sizeof(str), "Kliziste nije aktivno. Sledeca aktivacija za: %d min %d sek", minuta, sekundi);
    }
    SendClientMessage(playerid, 0xFFD700AA, str);
    return 1;
}

// ===========================================================================
//  KOMANDE
// ===========================================================================

// Prekid rada na klizistu
CMD:prekidakliziste(playerid, params[]) {
    #pragma unused params
    if(KlizPlayerData[playerid][klizPState] == KLIZISTE_PSTATE_NONE) {
        return KlizGRESKA(playerid, "Ne radite na klizistu.");
    }
    if(KlizPlayerData[playerid][klizMinigameActive]) {
        return KlizGRESKA(playerid, "Ne mozete prekinuti tokom minigame-a! Sacekajte da zavrsi.");
    }
    Kliziste_StopPlayer(playerid);
    KlizMSG(playerid, "{FF4500}[KLIZISTE]{FFFFFF} Prekinuli ste rad na klizistu i vratili opremu.");
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizStatusTD]);
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizProgressTD]);
    return 1;
}

// Skidanje opreme i odjava
CMD:skiniopremu(playerid, params[]) {
    #pragma unused params
    if(!KlizPlayerData[playerid][klizHasEquipment] && KlizPlayerData[playerid][klizPState] == KLIZISTE_PSTATE_NONE) {
        return KlizGRESKA(playerid, "Nemate opremu za skinuti.");
    }
    if(KlizPlayerData[playerid][klizMinigameActive]) {
        return KlizGRESKA(playerid, "Ne mozete skinuti opremu tokom minigame-a!");
    }
    Kliziste_StopPlayer(playerid);
    KlizMSG(playerid, "{FF4500}[KLIZISTE]{FFFFFF} Skinuli ste opremu i odjavili se sa klizista.");
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizStatusTD]);
    PlayerTextDrawHide(playerid, KlizPlayerData[playerid][klizProgressTD]);
    return 1;
}

// ===========================================================================
//  DOSTUPNE FUNKCIJE ZA VANJSKE SISTEME
// ===========================================================================

stock bool:Kliziste_IsActive() {
    return KlizActive;
}

stock Kliziste_GetProgress() {
    return KlizTotalDigs;
}

stock Kliziste_GetRemaining() {
    new rem = KLIZISTE_TOTAL_DIGS - KlizTotalDigs;
    if(rem < 0) rem = 0;
    return rem;
}
