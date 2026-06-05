// ===========================================================================
//  SISTEM FABRIKE PLASTIKE v3.0 - MERGED: FORKLIFT + BONUS TRUCK DELIVERY
//  Autor: Beka (uz pomoc AI asistenta)
//  Opis: Kompletan, modularan sistem fabrike plastike za SA-MP server.
//        Sadrzi DVA posla u jednom fajlu:
//
//        1) FORKLIFT JOB (Vilicar - ponavljajuci):
//           Uzmi opremu -> Uzmi gajbu -> Stol (minigame) -> Masina ->
//           Vilicar sa boxom -> Skladiste = zarada. Bez cooldowna.
//
//        2) BONUS TRUCK JOB (Kamionska dostava - 30 min cooldown):
//           Uzmi opremu -> ALT kod kamiona -> Vozi do utovara (CTRL utovar cijevi) ->
//           Vozi do dostave -> CTRL istovar -> Vozi nazad u bazu = $20000 + 50 Plastika.
//           30 minutni cooldown nakon zavrsetka.
//
//  HOOK FUNKCIJE (pozivaju se iz fg-ogc.pwn):
//    Plastika_Init()              - OnGameModeInit
//    Plastika_OnConnect(playerid) - OnPlayerConnect
//    Plastika_OnDisconnect(playerid) - OnPlayerDisconnect
//    Plastika_OnDeath(playerid)   - OnPlayerDeath
//    Plastika_OnCP(playerid)      - OnPlayerEnterCheckpoint
//    Plastika_OnKeyStateChange(playerid, newkeys, oldkeys) - OnPlayerKeyStateChange
//    Plastika_OnUpdate(playerid)  - OnPlayerUpdate
//    Plastika_OnVehicleDeath(vehicleid) - OnVehicleDeath
//    Plastika_OnPlayerExitVehicle(playerid, vehicleid) - OnPlayerExitVehicle
//
//  ADMIN FUNKCIJE (pozivaju se iz fg-ogc.pwn):
//    Plastika_GivePlastika(playerid, kolicina)
//    Plastika_ResetCooldown(playerid)
//    Plastika_SetCooldown(playerid, seconds)
// ===========================================================================

#if defined _plastika_included
    #endinput
#endif
#define _plastika_included

#include <a_samp>
#include <streamer>

// ===========================================================================
//  KONFIGURACIJA - KOORDINATE (FORKLIFT JOB)
// ===========================================================================

#define PLAS_OPREMA_X       -72.8136
#define PLAS_OPREMA_Y       -1110.5228
#define PLAS_OPREMA_Z       1.0781

#define PLAS_GAJBA1_X       -40.2594
#define PLAS_GAJBA1_Y       -1147.8491
#define PLAS_GAJBA1_Z       1.0781

#define PLAS_GAJBA2_X       -56.8645
#define PLAS_GAJBA2_Y       -1139.4548
#define PLAS_GAJBA2_Z       1.0781

#define PLAS_STOL1_X        -40.9064
#define PLAS_STOL1_Y        -1156.6299
#define PLAS_STOL1_Z        1.0781

#define PLAS_STOL2_X        -44.9986
#define PLAS_STOL2_Y        -1154.5715
#define PLAS_STOL2_Z        1.0781

#define PLAS_STOL3_X        -58.5935
#define PLAS_STOL3_Y        -1148.4282
#define PLAS_STOL3_Z        1.0781

#define PLAS_STOL4_X        -62.0864
#define PLAS_STOL4_Y        -1146.8300
#define PLAS_STOL4_Z        1.0781

#define PLAS_MAS1_X         -47.3807
#define PLAS_MAS1_Y         -1152.2712
#define PLAS_MAS1_Z         1.0781

#define PLAS_MAS2_X         -51.2024
#define PLAS_MAS2_Y         -1150.3396
#define PLAS_MAS2_Z         1.0781

#define PLAS_MAS3_X         -54.7064
#define PLAS_MAS3_Y         -1148.6614
#define PLAS_MAS3_Z         1.0781

#define PLAS_VIL1_X         -60.8337
#define PLAS_VIL1_Y         -1142.0447
#define PLAS_VIL1_Z         1.0781
#define PLAS_VIL1_A         244.4730

#define PLAS_VIL2_X         -39.6434
#define PLAS_VIL2_Y         -1151.8370
#define PLAS_VIL2_Z         1.0781
#define PLAS_VIL2_A         61.6123

#define PLAS_SKL1_X         -41.4545
#define PLAS_SKL1_Y         -1119.0657
#define PLAS_SKL1_Z         1.6451

#define PLAS_SKL2_X         -35.1128
#define PLAS_SKL2_Y         -1121.9177
#define PLAS_SKL2_Z         1.6451

#define PLAS_SKL3_X         -30.4248
#define PLAS_SKL3_Y         -1125.7463
#define PLAS_SKL3_Z         1.6451

#define PLAS_SKL4_X         -33.2259
#define PLAS_SKL4_Y         -1131.6309
#define PLAS_SKL4_Z         1.6451

// ===========================================================================
//  KONFIGURACIJA - KOORDINATE (BONUS TRUCK JOB)
// ===========================================================================

#define PLAS_KAMION_X       -62.0223
#define PLAS_KAMION_Y       -1117.1177
#define PLAS_KAMION_Z       1.0781
#define PLAS_KAMION_A       350.3807

#define PLAS_UTOVAR_X       -73.5247
#define PLAS_UTOVAR_Y       -1137.6943
#define PLAS_UTOVAR_Z       1.0781

#define PLAS_DOSTAVA_X      -384.6674
#define PLAS_DOSTAVA_Y      -1057.7296
#define PLAS_DOSTAVA_Z      59.0066

// Baza return point (crveni CP - kamion nestaje, plata)
#define PLAS_BAZA_X         -67.3746
#define PLAS_BAZA_Y         -1133.2557
#define PLAS_BAZA_Z         1.0781

// ===========================================================================
//  KONFIGURACIJA - PODESAVANJA
// ===========================================================================

#define PLAS_OPREMA_SKIN    176
#define PLAS_OPREMA_DIST    3.0
#define PLAS_GAJBA_DIST     2.5
#define PLAS_STOL_DIST      2.5
#define PLAS_MAS_DIST       2.5
#define PLAS_VIL_DIST       3.0
#define PLAS_SKL_DIST       5.0
#define PLAS_KAMION_DIST    4.0
#define PLAS_UTOVAR_DIST    6.0
#define PLAS_DOSTAVA_DIST   6.0

#define PLAS_GAJBA_MODEL    19587
#define PLAS_BOX_MODEL      1271
#define PLAS_BIGBOX_MODEL   1685
#define PLAS_VILICAR_MODEL  530
#define PLAS_KAMION_MODEL   578
#define PLAS_CIJEVA_MODEL   3675

#define PLAS_MACHINE_TIME   7
#define PLAS_UTOVAR_TIME    5
#define PLAS_ISTOVAR_TIME   5

// Minigame - Kliziste-style (0-100 scale)
#define PLAS_MINI_SPEED1    7.0
#define PLAS_MINI_SPEED2    9.0
#define PLAS_MINI_SPEED3    12.0
#define PLAS_MINI_ZONE1     14
#define PLAS_MINI_ZONE2     10
#define PLAS_MINI_ZONE3     7
#define PLAS_MARKER_WIDTH   4.0

// Forklift job pay (tiered by minigame hits)
#define PLAS_PAY_1_MIN      30
#define PLAS_PAY_1_MAX      50
#define PLAS_PAY_2_MIN      150
#define PLAS_PAY_2_MAX      230
#define PLAS_PAY_3_MIN      230
#define PLAS_PAY_3_MAX      470
#define PLAS_PLAS_1         1
#define PLAS_PLAS_2_MIN     2
#define PLAS_PLAS_2_MAX     3
#define PLAS_PLAS_3_MIN     3
#define PLAS_PLAS_3_MAX     6

// Bonus truck job pay
#define PLAS_BONUS_PAY_MIN  15000
#define PLAS_BONUS_PAY_MAX  25000
#define PLAS_BONUS_PLAS_MIN 30
#define PLAS_BONUS_PLAS_MAX 50

// Cooldown (30 min = 1800 seconds)
#define PLAS_COOLDOWN_SEC   1800

#define PLAS_ITEM_PLASTIKA  "Plastika"
#define PLAS_MODEL_PLASTIKA 19587

#define PLAS_SND_OPREMA     33100
#define PLAS_SND_USPJEH     1057
#define PLAS_SND_NAPLATA    1058
#define PLAS_SND_STROJ      1150
#define PLAS_SND_FAIL       1085

#define PLAS_CLR_BLUE       0x1D75C4AA
#define PLAS_CLR_GOLD       0xFFD700AA
#define PLAS_CLR_RED        0xFF6347AA
#define PLAS_CLR_GREEN      0x00FF00AA

// Minigame TD layout - Kliziste-style bar
#define PLAS_BAR_VISUAL_X   192.0
#define PLAS_BAR_VISUAL_END 448.0
#define PLAS_BAR_VISUAL_W   256.0
#define PLAS_BAR_Y          258.0
#define PLAS_PNL_L          170.0
#define PLAS_PNL_T          150.0

// ===========================================================================
//  ENUMS
// ===========================================================================

enum e_PlasState {
    PLAS_NONE = 0,
    // Shared state - player has equipment, can choose either job
    PLAS_EQUIPPED,
    // Bonus job (truck delivery) states
    PLAS_TRUCK_DRIVING,     // Driving truck to utovar
    PLAS_TRUCK_LOADING,     // Loading pipe onto truck (progress bar)
    PLAS_TRUCK_DELIVERING,  // Driving to delivery point
    PLAS_TRUCK_UNLOADING,   // Unloading pipe at destination (progress bar)
    PLAS_TRUCK_RETURNING,   // Driving truck back to base
    PLAS_COOLDOWN,          // 30 min cooldown after bonus job
    // Forklift job states
    PLAS_CARRYING_GAJBA,
    PLAS_MINIGAME,
    PLAS_CARRYING_BOX,
    PLAS_MACHINE,
    PLAS_CARRYING_BIGBOX,
    PLAS_FORKLIFT,
    PLAS_STORING
}

enum e_PlasPlayer {
    e_PlasState:plasState,
    plasOldSkin,
    bool:plasEquipped,
    plasVehicleID,           // Used by BOTH truck and forklift
    plasBoxObjID,            // Cijev on truck OR box on forklift
    plasTimerMini,
    plasTimerMachine,
    plasTimerUtovar,         // Truck loading timer
    plasTimerIstovar,        // Truck unloading timer
    plasCooldownTime,        // GetTickCount timestamp for cooldown
    plasMiniPhase,
    plasMiniHits,
    Float:plasMarkerPos,        // 0.0 - 100.0 scale
    Float:plasMGDirection,      // 1.0 or -1.0
    Float:plasMGTargetPos,      // Target zone center (0-100)
    Float:plasMGZoneSize,       // Zone size (0-100 units)
    plasMachineProgress,
    plasUtovarProgress,      // Truck loading progress
    plasIstovarProgress,     // Truck unloading progress
    plasTargetSkladiste,
    plasToursDone,
    // Progress bar TDs (shared by both systems)
    PlayerText:plasPBarBg,
    PlayerText:plasPBarFill,
    PlayerText:plasPBarLabel,
    PlayerText:plasStatusTD,
    bool:plasTDShown,
    // Minigame TDs
    PlayerText:plasMiniPanel,
    PlayerText:plasMiniTitle,
    PlayerText:plasMiniIcons,
    PlayerText:plasMiniPhaseTD,
    PlayerText:plasMiniBarBg,
    PlayerText:plasMiniBarGreen,
    PlayerText:plasMiniMarker,
    PlayerText:plasMiniStatus,
    PlayerText:plasMiniStars,
    PlayerText:plasMiniTemp,
    bool:plasMiniShown
}

static PlastikaPlayerData[MAX_PLAYERS][e_PlasPlayer];

// Pickups
static PlasPickup_Oprema = -1;
static PlasPickup_Gajba1 = -1;
static PlasPickup_Gajba2 = -1;
// Stol - NO pickups, only 3D labels
// Masina - NO pickups, only 3D labels
static PlasPickup_Vil[2] = {-1, ...};
static PlasPickup_Kamion = -1;
// PlasPickup_Utovar removed - no pickup needed (CTRL in vehicle)
static PlasPickup_Dostava = -1;

// 3D Text Labels
static Text3D:Plas3D_Oprema = Text3D:-1;
static Text3D:Plas3D_Gajba1 = Text3D:-1;
static Text3D:Plas3D_Gajba2 = Text3D:-1;
static Text3D:Plas3D_Stol[4] = {Text3D:-1, ...};
static Text3D:Plas3D_Mas[3] = {Text3D:-1, ...};
static Text3D:Plas3D_Vil[2] = {Text3D:-1, ...};
static Text3D:Plas3D_Skl[4] = {Text3D:-1, ...};
static Text3D:Plas3D_Kamion = Text3D:-1;
static Text3D:Plas3D_Utovar = Text3D:-1;
static Text3D:Plas3D_Dostava = Text3D:-1;

static PlasMapIcon = -1;

#pragma unused PlasPickup_Oprema, PlasPickup_Gajba1, PlasPickup_Gajba2
#pragma unused Plas3D_Oprema, Plas3D_Gajba1, Plas3D_Gajba2, PlasMapIcon

forward PlasTimer_MiniUpdate(playerid);
forward PlasTimer_Machine(playerid);
forward PlasMini_NextPhase(playerid);
forward PlasTimer_Utovar(playerid);
forward PlasTimer_Istovar(playerid);

// ===========================================================================
//  HELPERS
// ===========================================================================

stock PlasMSG(playerid, msg[]) SendClientMessage(playerid, PLAS_CLR_BLUE, msg);
stock PlasGRESKA(playerid, msg[]) { SendClientMessage(playerid, PLAS_CLR_RED, msg); return 1; }
stock PlasINFO(playerid, msg[]) { SendClientMessage(playerid, PLAS_CLR_GOLD, msg); return 1; }
stock PlasSUCCESS(playerid, msg[]) { SendClientMessage(playerid, PLAS_CLR_GREEN, msg); return 1; }

stock Float:PlasDist(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2) {
    new Float:dx = x1-x2, Float:dy = y1-y2, Float:dz = z1-z2;
    return floatsqroot(dx*dx + dy*dy + dz*dz);
}

stock PlasRand(min, max) return min + random(max - min + 1);

stock bool:PlasValid(playerid) {
    if(playerid < 0 || playerid >= MAX_PLAYERS) return false;
    return bool:IsPlayerConnected(playerid);
}

stock PlasInv_Add(playerid, amt) {
    #if defined Inventory_Add
        return Inventory_Add(playerid, PLAS_ITEM_PLASTIKA, PLAS_MODEL_PLASTIKA, amt);
    #else
        return GivePlayerWeapon(playerid, 0, 0);
    #endif
}

stock PlasSkladPos(id, &Float:x, &Float:y, &Float:z) {
    switch(id) {
        case 1: { x=PLAS_SKL1_X; y=PLAS_SKL1_Y; z=PLAS_SKL1_Z; }
        case 2: { x=PLAS_SKL2_X; y=PLAS_SKL2_Y; z=PLAS_SKL2_Z; }
        case 3: { x=PLAS_SKL3_X; y=PLAS_SKL3_Y; z=PLAS_SKL3_Z; }
        default: { x=PLAS_SKL4_X; y=PLAS_SKL4_Y; z=PLAS_SKL4_Z; }
    }
}

stock bool:PlasNearGajba(playerid) {
    new Float:x, Float:y, Float:z; GetPlayerPos(playerid, x, y, z);
    if(PlasDist(x,y,z, PLAS_GAJBA1_X,PLAS_GAJBA1_Y,PLAS_GAJBA1_Z) <= PLAS_GAJBA_DIST) return true;
    if(PlasDist(x,y,z, PLAS_GAJBA2_X,PLAS_GAJBA2_Y,PLAS_GAJBA2_Z) <= PLAS_GAJBA_DIST) return true;
    return false;
}

stock bool:PlasNearStol(playerid) {
    new Float:x, Float:y, Float:z; GetPlayerPos(playerid, x, y, z);
    if(PlasDist(x,y,z, PLAS_STOL1_X,PLAS_STOL1_Y,PLAS_STOL1_Z) <= PLAS_STOL_DIST) return true;
    if(PlasDist(x,y,z, PLAS_STOL2_X,PLAS_STOL2_Y,PLAS_STOL2_Z) <= PLAS_STOL_DIST) return true;
    if(PlasDist(x,y,z, PLAS_STOL3_X,PLAS_STOL3_Y,PLAS_STOL3_Z) <= PLAS_STOL_DIST) return true;
    if(PlasDist(x,y,z, PLAS_STOL4_X,PLAS_STOL4_Y,PLAS_STOL4_Z) <= PLAS_STOL_DIST) return true;
    return false;
}

stock bool:PlasNearMasina(playerid) {
    new Float:x, Float:y, Float:z; GetPlayerPos(playerid, x, y, z);
    if(PlasDist(x,y,z, PLAS_MAS1_X,PLAS_MAS1_Y,PLAS_MAS1_Z) <= PLAS_MAS_DIST) return true;
    if(PlasDist(x,y,z, PLAS_MAS2_X,PLAS_MAS2_Y,PLAS_MAS2_Z) <= PLAS_MAS_DIST) return true;
    if(PlasDist(x,y,z, PLAS_MAS3_X,PLAS_MAS3_Y,PLAS_MAS3_Z) <= PLAS_MAS_DIST) return true;
    return false;
}

stock bool:PlasNearVilicar(playerid) {
    new Float:x, Float:y, Float:z; GetPlayerPos(playerid, x, y, z);
    if(PlasDist(x,y,z, PLAS_VIL1_X,PLAS_VIL1_Y,PLAS_VIL1_Z) <= PLAS_VIL_DIST) return true;
    if(PlasDist(x,y,z, PLAS_VIL2_X,PLAS_VIL2_Y,PLAS_VIL2_Z) <= PLAS_VIL_DIST) return true;
    return false;
}

stock bool:PlasNearKamion(playerid) {
    new Float:x, Float:y, Float:z; GetPlayerPos(playerid, x, y, z);
    if(PlasDist(x,y,z, PLAS_KAMION_X,PLAS_KAMION_Y,PLAS_KAMION_Z) <= PLAS_KAMION_DIST) return true;
    return false;
}

stock bool:PlasVehicleNearUtovar(playerid) {
    if(PlastikaPlayerData[playerid][plasVehicleID] == -1) return false;
    new Float:vx, Float:vy, Float:vz;
    GetVehiclePos(PlastikaPlayerData[playerid][plasVehicleID], vx, vy, vz);
    if(PlasDist(vx,vy,vz, PLAS_UTOVAR_X,PLAS_UTOVAR_Y,PLAS_UTOVAR_Z) <= PLAS_UTOVAR_DIST) return true;
    return false;
}

stock bool:PlasVehicleNearDostava(playerid) {
    if(PlastikaPlayerData[playerid][plasVehicleID] == -1) return false;
    new Float:vx, Float:vy, Float:vz;
    GetVehiclePos(PlastikaPlayerData[playerid][plasVehicleID], vx, vy, vz);
    if(PlasDist(vx,vy,vz, PLAS_DOSTAVA_X,PLAS_DOSTAVA_Y,PLAS_DOSTAVA_Z) <= PLAS_DOSTAVA_DIST) return true;
    return false;
}

stock bool:PlasVehicleNearKamionBaza(playerid) {
    if(PlastikaPlayerData[playerid][plasVehicleID] == -1) return false;
    new Float:vx, Float:vy, Float:vz;
    GetVehiclePos(PlastikaPlayerData[playerid][plasVehicleID], vx, vy, vz);
    if(PlasDist(vx,vy,vz, PLAS_BAZA_X,PLAS_BAZA_Y,PLAS_BAZA_Z) <= 8.0) return true;
    return false;
}

// ===========================================================================
//  PROGRESS BAR TEXTDRAWS
// ===========================================================================

stock Plas_CreateProgressTDs(playerid) {
    PlastikaPlayerData[playerid][plasPBarBg] = CreatePlayerTextDraw(playerid, 250.0, 400.0, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasPBarBg], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasPBarBg], 0.5, 1.5);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasPBarBg], -1);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasPBarBg], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasPBarBg], 1);
    PlayerTextDrawSetShadow(playerid, PlastikaPlayerData[playerid][plasPBarBg], 1);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasPBarBg], 1);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasPBarBg], 80);
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasPBarBg], 420.0, 0.0);

    PlastikaPlayerData[playerid][plasPBarFill] = CreatePlayerTextDraw(playerid, 251.0, 401.0, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasPBarFill], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasPBarFill], 0.48, 1.2);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasPBarFill], -1);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasPBarFill], 0);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasPBarFill], 1);
    PlayerTextDrawSetShadow(playerid, PlastikaPlayerData[playerid][plasPBarFill], 0);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasPBarFill], 1);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasPBarFill], 0x1D75C466);
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasPBarFill], 251.0, 0.0);

    PlastikaPlayerData[playerid][plasPBarLabel] = CreatePlayerTextDraw(playerid, 335.0, 385.0, "_");
    PlayerTextDrawAlignment(playerid, PlastikaPlayerData[playerid][plasPBarLabel], 2);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasPBarLabel], 2);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasPBarLabel], 0.3, 1.1);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasPBarLabel], 0x1D75C4EE);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasPBarLabel], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasPBarLabel], 1);

    PlastikaPlayerData[playerid][plasStatusTD] = CreatePlayerTextDraw(playerid, 20.0, 130.0, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasStatusTD], 2);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasStatusTD], 0.18, 0.9);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasStatusTD], 0x1D75C4DD);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasStatusTD], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasStatusTD], 1);
}

stock Plas_ShowProgress(playerid, Float:pct, label[]) {
    new Float:w = 251.0 + (168.0 * (pct / 100.0));
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasPBarFill], w, 0.0);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasPBarBg]);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasPBarFill]);
    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasPBarLabel], label);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasPBarLabel]);
    PlastikaPlayerData[playerid][plasTDShown] = true;
}

stock Plas_HideProgress(playerid) {
    if(!PlastikaPlayerData[playerid][plasTDShown]) return;
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasPBarBg]);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasPBarFill]);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasPBarLabel]);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasStatusTD]);
    PlastikaPlayerData[playerid][plasTDShown] = false;
}

stock Plas_Status(playerid, s[]) {
    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasStatusTD], s);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasStatusTD]);
}

// ===========================================================================
//  MINIGAME TEXTDRAWS (BOX-based, no overflow!)
// ===========================================================================

stock PlasMini_CreateTDs(playerid) {
    // Panel
    PlastikaPlayerData[playerid][plasMiniPanel] = CreatePlayerTextDraw(playerid, PLAS_PNL_L, PLAS_PNL_T, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniPanel], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniPanel], 0.0, 13.5);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasMiniPanel], 1);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasMiniPanel], 0x1A1A2EDD);
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasMiniPanel], PLAS_PNL_L+300.0, 0.0);

    // Title
    PlastikaPlayerData[playerid][plasMiniTitle] = CreatePlayerTextDraw(playerid, 320.0, 158.0, "TERMO PRESOVANJE");
    PlayerTextDrawAlignment(playerid, PlastikaPlayerData[playerid][plasMiniTitle], 2);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniTitle], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniTitle], 0.35, 1.5);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniTitle], 0x1D75C4FF);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasMiniTitle], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasMiniTitle], 1);

    // Icons
    PlastikaPlayerData[playerid][plasMiniIcons] = CreatePlayerTextDraw(playerid, 320.0, 182.0, "~b~~h~[GAJBA] ~w~~h~---> ~b~~h~[BOX]");
    PlayerTextDrawAlignment(playerid, PlastikaPlayerData[playerid][plasMiniIcons], 2);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniIcons], 2);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniIcons], 0.22, 1.0);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniIcons], -1);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasMiniIcons], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasMiniIcons], 1);

    // Phase
    PlastikaPlayerData[playerid][plasMiniPhaseTD] = CreatePlayerTextDraw(playerid, 320.0, 205.0, "FAZA 1/3: ZAGRIJAVANJE");
    PlayerTextDrawAlignment(playerid, PlastikaPlayerData[playerid][plasMiniPhaseTD], 2);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniPhaseTD], 2);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniPhaseTD], 0.25, 1.1);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniPhaseTD], 0xFFD700FF);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasMiniPhaseTD], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasMiniPhaseTD], 1);

    // Temp
    PlastikaPlayerData[playerid][plasMiniTemp] = CreatePlayerTextDraw(playerid, 320.0, 228.0, "TEMP: 45 C");
    PlayerTextDrawAlignment(playerid, PlastikaPlayerData[playerid][plasMiniTemp], 2);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniTemp], 2);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniTemp], 0.22, 1.0);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniTemp], 0x3498DBFF);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasMiniTemp], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasMiniTemp], 1);

    // Bar BG (dark track)
    PlastikaPlayerData[playerid][plasMiniBarBg] = CreatePlayerTextDraw(playerid, PLAS_BAR_VISUAL_X, PLAS_BAR_Y, "_");
    PlayerTextDrawBackgroundColor(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], 0);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], 0.500000, 2.200000);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], -1);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], 0);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], 1);
    PlayerTextDrawSetShadow(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], 0);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], 1);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], 0x2a2a42CC);
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], PLAS_BAR_VISUAL_END, 0.0);

    // Green zone - placeholder (created dynamically in PlasMini_CreateZoneTD)
    PlastikaPlayerData[playerid][plasMiniBarGreen] = CreatePlayerTextDraw(playerid, 0.0, 0.0, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0.01, 0.01);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0x00000000);

    // Marker - placeholder (created dynamically in PlasMini_CreateMarkerTD)
    PlastikaPlayerData[playerid][plasMiniMarker] = CreatePlayerTextDraw(playerid, 0.0, 0.0, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0.01, 0.01);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0x00000000);

    // Status
    PlastikaPlayerData[playerid][plasMiniStatus] = CreatePlayerTextDraw(playerid, 320.0, 285.0, "Pritisni SPACE za zaustavljanje!");
    PlayerTextDrawAlignment(playerid, PlastikaPlayerData[playerid][plasMiniStatus], 2);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniStatus], 2);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniStatus], 0.2, 0.9);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniStatus], 0xFFFFFFFF);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasMiniStatus], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasMiniStatus], 1);

    // Stars
    PlastikaPlayerData[playerid][plasMiniStars] = CreatePlayerTextDraw(playerid, 320.0, 308.0, "");
    PlayerTextDrawAlignment(playerid, PlastikaPlayerData[playerid][plasMiniStars], 2);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniStars], 2);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniStars], 0.25, 1.1);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniStars], -1);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasMiniStars], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasMiniStars], 1);

    PlastikaPlayerData[playerid][plasMiniShown] = false;
}

stock PlasMini_Show(playerid) {
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniPanel]);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniTitle]);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniIcons]);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniPhaseTD]);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniTemp]);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniBarBg]);
    // Zone and marker created dynamically
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniStatus]);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniStars]);
    PlastikaPlayerData[playerid][plasMiniShown] = true;
}

stock PlasMini_Hide(playerid) {
    if(!PlastikaPlayerData[playerid][plasMiniShown]) return;
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniPanel]);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniTitle]);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniIcons]);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniPhaseTD]);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniTemp]);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniBarBg]);
    // Destroy dynamic zone, recreate placeholder
    PlayerTextDrawDestroy(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen]);
    PlastikaPlayerData[playerid][plasMiniBarGreen] = CreatePlayerTextDraw(playerid, 0.0, 0.0, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0.01, 0.01);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0x00000000);
    // Destroy dynamic marker, recreate placeholder
    PlayerTextDrawDestroy(playerid, PlastikaPlayerData[playerid][plasMiniMarker]);
    PlastikaPlayerData[playerid][plasMiniMarker] = CreatePlayerTextDraw(playerid, 0.0, 0.0, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0.01, 0.01);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0x00000000);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniStatus]);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniStars]);
    PlastikaPlayerData[playerid][plasMiniShown] = false;
}

stock PlasMini_CreateZoneTD(playerid) {
    new Float:target = PlastikaPlayerData[playerid][plasMGTargetPos];
    new Float:halfZone = PlastikaPlayerData[playerid][plasMGZoneSize] / 2.0;

    new Float:zoneStartPct = (target - halfZone) / 100.0;
    new Float:zoneEndPct = (target + halfZone) / 100.0;
    if(zoneStartPct < 0.0) zoneStartPct = 0.0;
    if(zoneEndPct > 1.0) zoneEndPct = 1.0;

    new Float:zoneStartX = PLAS_BAR_VISUAL_X + zoneStartPct * PLAS_BAR_VISUAL_W;
    new Float:zoneEndX = PLAS_BAR_VISUAL_X + zoneEndPct * PLAS_BAR_VISUAL_W;

    PlayerTextDrawDestroy(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen]);

    PlastikaPlayerData[playerid][plasMiniBarGreen] = CreatePlayerTextDraw(playerid, zoneStartX, PLAS_BAR_Y + 1.0, "_");
    PlayerTextDrawBackgroundColor(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0.500000, 1.900000);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0x00000000);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 1);
    PlayerTextDrawSetShadow(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 1);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0x2ECC7188);
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], zoneEndX, 0.0);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen]);
}

stock PlasMini_CreateMarkerTD(playerid) {
    new Float:progress = PlastikaPlayerData[playerid][plasMarkerPos];
    new Float:target = PlastikaPlayerData[playerid][plasMGTargetPos];
    new Float:halfZone = PlastikaPlayerData[playerid][plasMGZoneSize] / 2.0;
    new bool:inZone = (progress >= (target - halfZone) && progress <= (target + halfZone));

    new Float:markerX = PLAS_BAR_VISUAL_X + (progress / 100.0) * PLAS_BAR_VISUAL_W;

    PlayerTextDrawDestroy(playerid, PlastikaPlayerData[playerid][plasMiniMarker]);

    new Float:mLeft = markerX - (PLAS_MARKER_WIDTH / 2.0);
    new Float:mRight = markerX + (PLAS_MARKER_WIDTH / 2.0);

    PlastikaPlayerData[playerid][plasMiniMarker] = CreatePlayerTextDraw(playerid, mLeft, PLAS_BAR_Y, "_");
    PlayerTextDrawBackgroundColor(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0.100000, 2.200000);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0x00000000);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 1);
    PlayerTextDrawSetShadow(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 1);
    if(inZone) {
        PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0xFFFF00FF); // Yellow - in zone
    } else {
        PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0xFF4444FF); // Red - outside zone
    }
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasMiniMarker], mRight, 0.0);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniMarker]);

    // Update status text
    if(inZone) {
        PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniStatus], "~g~~h~U ZONI! ~w~> ~y~~h~STISNI SPACE! ~w~<");
    } else {
        PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniStatus], "~r~~h~VANI ZONE~w~ - Cekaj zeleni");
    }
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniStatus]);
}

stock PlasMini_UpdateStars(playerid) {
    new str[32], pos = 0;
    new hits = PlastikaPlayerData[playerid][plasMiniHits];
    new phase = PlastikaPlayerData[playerid][plasMiniPhase];
    for(new i = 0; i < 3; i++) {
        if(i < phase) {
            if(i < hits) { str[pos++]='~'; str[pos++]='g'; str[pos++]='~'; str[pos++]='~'; str[pos++]='h'; str[pos++]='~'; str[pos++]='V'; str[pos++]=' '; }
            else { str[pos++]='~'; str[pos++]='r'; str[pos++]='~'; str[pos++]='X'; str[pos++]=' '; }
        } else { str[pos++]='~'; str[pos++]='d'; str[pos++]='~'; str[pos++]='-'; str[pos++]=' '; }
    }
    str[pos] = '\0';
    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniStars], str);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniStars]);
}

// ===========================================================================
//  MINIGAME LOGIC
// ===========================================================================

stock PlasMini_StartPhase(playerid) {
    new phase = PlastikaPlayerData[playerid][plasMiniPhase];
    new Float:zoneSize;
    new name[24];

    switch(phase) {
        case 0: { zoneSize=float(PLAS_MINI_ZONE1); name="ZAGRIJAVANJE"; }
        case 1: { zoneSize=float(PLAS_MINI_ZONE2); name="PRESOVANJE"; }
        case 2: { zoneSize=float(PLAS_MINI_ZONE3); name="HLADJENJE"; }
        default: { zoneSize=float(PLAS_MINI_ZONE1); name="ZAGRIJAVANJE"; }
    }

    // Random target position (0-100), ensure zone fits in bar
    new Float:halfZone = zoneSize / 2.0;
    new Float:minTarget = halfZone + 5.0;
    new Float:maxTarget = 100.0 - halfZone - 5.0;
    if(minTarget >= maxTarget) minTarget = 50.0;
    new Float:targetRange = maxTarget - minTarget;
    if(targetRange < 1.0) targetRange = 1.0;
    PlastikaPlayerData[playerid][plasMGTargetPos] = minTarget + float(random(floatround(targetRange)));
    PlastikaPlayerData[playerid][plasMGZoneSize] = zoneSize;

    // Random starting position (0-100)
    PlastikaPlayerData[playerid][plasMarkerPos] = float(random(100));
    // Random direction
    PlastikaPlayerData[playerid][plasMGDirection] = (random(2) == 0) ? 1.0 : -1.0;

    new ps[48]; format(ps, sizeof(ps), "FAZA %d/3: %s", phase+1, name);
    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniPhaseTD], ps);

    new ts[24];
    switch(phase) {
        case 0: ts = "TEMP: 45 C";
        case 1: ts = "TEMP: 180 C";
        case 2: ts = "TEMP: HLADJENJE";
    }
    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniTemp], ts);
    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniStatus], "Pritisni SPACE u zelenoj zoni!");

    PlasMini_CreateZoneTD(playerid);
    PlasMini_CreateMarkerTD(playerid);
    PlasMini_UpdateStars(playerid);

    PlastikaPlayerData[playerid][plasTimerMini] = SetTimerEx("PlasTimer_MiniUpdate", 100, true, "i", playerid);
}

stock PlasMini_StopPhase(playerid, bool:hit) {
    if(PlastikaPlayerData[playerid][plasTimerMini] != -1) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerMini]);
        PlastikaPlayerData[playerid][plasTimerMini] = -1;
    }
    if(hit) {
        PlastikaPlayerData[playerid][plasMiniHits]++;
        PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniStatus], "~g~~h~POGODAK!");
        PlayerPlaySound(playerid, PLAS_SND_USPJEH, 0.0, 0.0, 0.0);
    } else {
        PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniStatus], "~r~PROMASAJ!");
        PlayerPlaySound(playerid, PLAS_SND_FAIL, 0.0, 0.0, 0.0);
    }
    PlasMini_UpdateStars(playerid);
    PlastikaPlayerData[playerid][plasMiniPhase]++;
    SetTimerEx("PlasMini_NextPhase", 1500, false, "i", playerid);
}

public PlasMini_NextPhase(playerid) {
    if(!PlasValid(playerid)) return 0;
    if(PlastikaPlayerData[playerid][plasState] != PLAS_MINIGAME) return 0;
    if(PlastikaPlayerData[playerid][plasMiniPhase] >= 3) {
        PlasMini_Finish(playerid);
        return 1;
    }
    PlasMini_StartPhase(playerid);
    return 1;
}

stock PlasMini_Finish(playerid) {
    PlasMini_Hide(playerid);
    ClearAnimations(playerid);

    new hits = PlastikaPlayerData[playerid][plasMiniHits], str[128];

    if(hits >= 1) {
        format(str, sizeof(str), "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Presovanje zavrseno! (%d/3) Dobili ste plasticni box.", hits);
        PlasMSG(playerid, str);
        SetPlayerAttachedObject(playerid, 4, PLAS_BOX_MODEL, 5, 0.07, 0.02, 0.0, 0.0, 80.0, 0.0, 0.85, 0.85, 0.85, 0x1D75C4FF, 0x1D75C4FF);
        if(IsPlayerAttachedObjectSlotUsed(playerid, 3)) RemovePlayerAttachedObject(playerid, 3);

        PlastikaPlayerData[playerid][plasState] = PLAS_CARRYING_BOX;
        PlasINFO(playerid, "Nosite plasticni box. Idite do masine i stisnite ALT.");
        Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Nosi box do masine");
        SetPlayerCheckpoint(playerid, PLAS_MAS1_X, PLAS_MAS1_Y, PLAS_MAS1_Z, 3.0);
    } else {
        format(str, sizeof(str), "{FF6347}[FABRIKA PLASTIKE]{FFFFFF} Presovanje neuspjesno! (0/3) Gajba unistena.");
        PlasGRESKA(playerid, str);
        if(IsPlayerAttachedObjectSlotUsed(playerid, 3)) RemovePlayerAttachedObject(playerid, 3);

        PlastikaPlayerData[playerid][plasState] = PLAS_EQUIPPED;
        PlasINFO(playerid, "Uzmite novu gajbu i pokusajte ponovo.");
        Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Uzmi novu gajbu ili idi do kamiona");
    }
    PlayerPlaySound(playerid, hits >= 1 ? PLAS_SND_USPJEH : PLAS_SND_FAIL, 0.0, 0.0, 0.0);
}

// ===========================================================================
//  MINIGAME TIMER
// ===========================================================================

public PlasTimer_MiniUpdate(playerid) {
    if(!PlasValid(playerid) || PlastikaPlayerData[playerid][plasState] != PLAS_MINIGAME) {
        if(PlastikaPlayerData[playerid][plasTimerMini] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerMini]); PlastikaPlayerData[playerid][plasTimerMini]=-1; }
        return 0;
    }
    new Float:speed;
    switch(PlastikaPlayerData[playerid][plasMiniPhase]) {
        case 0: speed=PLAS_MINI_SPEED1;
        case 1: speed=PLAS_MINI_SPEED2;
        case 2: speed=PLAS_MINI_SPEED3;
        default: speed=PLAS_MINI_SPEED1;
    }

    // Move marker on 0-100 scale (ping-pong)
    PlastikaPlayerData[playerid][plasMarkerPos] += speed * PlastikaPlayerData[playerid][plasMGDirection];

    if(PlastikaPlayerData[playerid][plasMarkerPos] >= 100.0) {
        PlastikaPlayerData[playerid][plasMarkerPos] = 100.0;
        PlastikaPlayerData[playerid][plasMGDirection] = -1.0;
    }
    if(PlastikaPlayerData[playerid][plasMarkerPos] <= 0.0) {
        PlastikaPlayerData[playerid][plasMarkerPos] = 0.0;
        PlastikaPlayerData[playerid][plasMGDirection] = 1.0;
    }

    PlasMini_CreateMarkerTD(playerid);

    // Update temp display
    new ts[24];
    new Float:pos = PlastikaPlayerData[playerid][plasMarkerPos];
    switch(PlastikaPlayerData[playerid][plasMiniPhase]) {
        case 0: format(ts, sizeof(ts), "TEMP: %d C", floatround(pos*3.0));
        case 1: format(ts, sizeof(ts), "TEMP: %d C", 180+floatround(pos*0.5));
        case 2: format(ts, sizeof(ts), "TEMP: %d C", 230-floatround(pos*1.5));
    }
    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniTemp], ts);
    return 1;
}

// ===========================================================================
//  MACHINE TIMER (Forklift job)
// ===========================================================================

public PlasTimer_Machine(playerid) {
    if(!PlasValid(playerid) || PlastikaPlayerData[playerid][plasState] != PLAS_MACHINE) {
        if(PlastikaPlayerData[playerid][plasTimerMachine] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerMachine]); PlastikaPlayerData[playerid][plasTimerMachine]=-1; }
        return 0;
    }
    PlastikaPlayerData[playerid][plasMachineProgress]++;
    new Float:pct = (Float:PlastikaPlayerData[playerid][plasMachineProgress] / Float:PLAS_MACHINE_TIME) * 100.0;
    Plas_ShowProgress(playerid, pct, "PRERADA PLASTIKE...");

    if(PlastikaPlayerData[playerid][plasMachineProgress] >= PLAS_MACHINE_TIME) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerMachine]);
        PlastikaPlayerData[playerid][plasTimerMachine] = -1;
        Plas_HideProgress(playerid);

        PlayerPlaySound(playerid, PLAS_SND_USPJEH, 0.0, 0.0, 0.0);
        PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Masina zavrsila! Big plastic box spreman.");
        PlasINFO(playerid, "Idite do vilicara i stisnite ALT.");

        PlastikaPlayerData[playerid][plasState] = PLAS_CARRYING_BIGBOX;
        if(IsPlayerAttachedObjectSlotUsed(playerid, 4)) RemovePlayerAttachedObject(playerid, 4);

        // SetCP ka najblizem vilicaru
        new Float:x,Float:y,Float:z; GetPlayerPos(playerid,x,y,z);
        if(PlasDist(x,y,z,PLAS_VIL1_X,PLAS_VIL1_Y,PLAS_VIL1_Z) < PlasDist(x,y,z,PLAS_VIL2_X,PLAS_VIL2_Y,PLAS_VIL2_Z))
            SetPlayerCheckpoint(playerid, PLAS_VIL1_X,PLAS_VIL1_Y,PLAS_VIL1_Z, 3.0);
        else
            SetPlayerCheckpoint(playerid, PLAS_VIL2_X,PLAS_VIL2_Y,PLAS_VIL2_Z, 3.0);

        Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Idi do vilicara");
    }
    return 1;
}

// ===========================================================================
//  TRUCK UTOVAR TIMER (Bonus job - loading pipe)
// ===========================================================================

public PlasTimer_Utovar(playerid) {
    if(!PlasValid(playerid) || PlastikaPlayerData[playerid][plasState] != PLAS_TRUCK_LOADING) {
        if(PlastikaPlayerData[playerid][plasTimerUtovar] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerUtovar]); PlastikaPlayerData[playerid][plasTimerUtovar]=-1; }
        return 0;
    }
    PlastikaPlayerData[playerid][plasUtovarProgress]++;
    new Float:pct = (Float:PlastikaPlayerData[playerid][plasUtovarProgress] / Float:PLAS_UTOVAR_TIME) * 100.0;
    Plas_ShowProgress(playerid, pct, "UTOVAR CIJEVI...");

    if(PlastikaPlayerData[playerid][plasUtovarProgress] >= PLAS_UTOVAR_TIME) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerUtovar]);
        PlastikaPlayerData[playerid][plasTimerUtovar] = -1;
        Plas_HideProgress(playerid);

        // Attach plastic pipe to truck
        new cijevObj = CreateDynamicObject(PLAS_CIJEVA_MODEL, 0.0, 0.0, -1000.0, 0.0, 0.0, 0.0);
        SetDynamicObjectMaterial(cijevObj, 0, 14600, "paperchase_bits2", "ab_plasticBin", 0);
        AttachDynamicObjectToVehicle(cijevObj, PlastikaPlayerData[playerid][plasVehicleID], -0.496, 0.649, 1.315, -1.600, 78.999, 88.300);
        PlastikaPlayerData[playerid][plasBoxObjID] = cijevObj;

        // Unfreeze player after loading
        TogglePlayerControllable(playerid, 1);

        PlastikaPlayerData[playerid][plasState] = PLAS_TRUCK_DELIVERING;
        SetPlayerCheckpoint(playerid, PLAS_DOSTAVA_X, PLAS_DOSTAVA_Y, PLAS_DOSTAVA_Z, 8.0);

        PlayerPlaySound(playerid, PLAS_SND_USPJEH, 0.0, 0.0, 0.0);
        PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Cijev utovarena na kamion! Vozite do dostave.");
        Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Vozi do dostave");
    }
    return 1;
}

// ===========================================================================
//  TRUCK ISTOVAR TIMER (Bonus job - unloading pipe)
// ===========================================================================

public PlasTimer_Istovar(playerid) {
    if(!PlasValid(playerid) || PlastikaPlayerData[playerid][plasState] != PLAS_TRUCK_UNLOADING) {
        if(PlastikaPlayerData[playerid][plasTimerIstovar] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerIstovar]); PlastikaPlayerData[playerid][plasTimerIstovar]=-1; }
        return 0;
    }
    PlastikaPlayerData[playerid][plasIstovarProgress]++;
    new Float:pct = (Float:PlastikaPlayerData[playerid][plasIstovarProgress] / Float:PLAS_ISTOVAR_TIME) * 100.0;
    Plas_ShowProgress(playerid, pct, "ISTOVAR CIJEVI...");

    if(PlastikaPlayerData[playerid][plasIstovarProgress] >= PLAS_ISTOVAR_TIME) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerIstovar]);
        PlastikaPlayerData[playerid][plasTimerIstovar] = -1;
        Plas_HideProgress(playerid);

        // Destroy the pipe object
        if(PlastikaPlayerData[playerid][plasBoxObjID] != -1) {
            DestroyDynamicObject(PlastikaPlayerData[playerid][plasBoxObjID]);
            PlastikaPlayerData[playerid][plasBoxObjID] = -1;
        }

        // Unfreeze player after unloading
        TogglePlayerControllable(playerid, 1);

        PlastikaPlayerData[playerid][plasState] = PLAS_TRUCK_RETURNING;
        SetPlayerCheckpoint(playerid, PLAS_BAZA_X, PLAS_BAZA_Y, PLAS_BAZA_Z, 6.0);

        PlayerPlaySound(playerid, PLAS_SND_USPJEH, 0.0, 0.0, 0.0);
        PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Cijev istovarena! Vozite kamion nazad u bazu.");
        Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Vozi kamion nazad u bazu");
    }
    return 1;
}

// ===========================================================================
//  CORE JOB LOGIC - STOP JOB (cleans up BOTH systems)
// ===========================================================================

stock Plastika_StopJob(playerid) {
    // Kill ALL timers
    if(PlastikaPlayerData[playerid][plasTimerMini] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerMini]); PlastikaPlayerData[playerid][plasTimerMini]=-1; }
    if(PlastikaPlayerData[playerid][plasTimerMachine] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerMachine]); PlastikaPlayerData[playerid][plasTimerMachine]=-1; }
    if(PlastikaPlayerData[playerid][plasTimerUtovar] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerUtovar]); PlastikaPlayerData[playerid][plasTimerUtovar]=-1; }
    if(PlastikaPlayerData[playerid][plasTimerIstovar] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerIstovar]); PlastikaPlayerData[playerid][plasTimerIstovar]=-1; }

    // Unfreeze player if frozen during loading
    TogglePlayerControllable(playerid, 1);

    // Destroy vehicle (truck or forklift)
    if(PlastikaPlayerData[playerid][plasVehicleID] != -1) {
        new vid = PlastikaPlayerData[playerid][plasVehicleID];
        if(GetVehicleModel(vid) != 0) { for(new i=0;i<MAX_PLAYERS;i++) if(IsPlayerInVehicle(i,vid)) RemovePlayerFromVehicle(i); DestroyVehicle(vid); }
        PlastikaPlayerData[playerid][plasVehicleID] = -1;
    }
    // Destroy attached object (pipe on truck OR box on forklift)
    if(PlastikaPlayerData[playerid][plasBoxObjID] != -1) { DestroyDynamicObject(PlastikaPlayerData[playerid][plasBoxObjID]); PlastikaPlayerData[playerid][plasBoxObjID]=-1; }

    // Remove player attached objects
    if(IsPlayerAttachedObjectSlotUsed(playerid, 3)) RemovePlayerAttachedObject(playerid, 3);
    if(IsPlayerAttachedObjectSlotUsed(playerid, 4)) RemovePlayerAttachedObject(playerid, 4);

    Plas_HideProgress(playerid);
    PlasMini_Hide(playerid);
    DisablePlayerCheckpoint(playerid);

    if(PlastikaPlayerData[playerid][plasEquipped] && PlastikaPlayerData[playerid][plasOldSkin] != -1)
        SetPlayerSkin(playerid, PlastikaPlayerData[playerid][plasOldSkin]);

    ClearAnimations(playerid);

    PlastikaPlayerData[playerid][plasState] = PLAS_NONE;
    PlastikaPlayerData[playerid][plasEquipped] = false;
    PlastikaPlayerData[playerid][plasOldSkin] = -1;
    PlastikaPlayerData[playerid][plasMachineProgress] = 0;
    PlastikaPlayerData[playerid][plasMiniPhase] = 0;
    PlastikaPlayerData[playerid][plasMiniHits] = 0;
    PlastikaPlayerData[playerid][plasMarkerPos] = 0.0;
    PlastikaPlayerData[playerid][plasMGDirection] = 1.0;
    PlastikaPlayerData[playerid][plasUtovarProgress] = 0;
    PlastikaPlayerData[playerid][plasIstovarProgress] = 0;
    DeletePVar(playerid, "PlastikaWorking");
}

// ===========================================================================
//  /uzmiopremup COMMAND LOGIC
// ===========================================================================

stock Plastika_UzmiOpremu(playerid) {
    // If already equipped and not in a special state, don't allow re-equip
    if(PlastikaPlayerData[playerid][plasEquipped] && PlastikaPlayerData[playerid][plasState] != PLAS_COOLDOWN && PlastikaPlayerData[playerid][plasState] != PLAS_NONE)
        return PlasGRESKA(playerid, "Vec imate opremu! Kucajte /plastikainfo za pomoc.");

    // If in cooldown, check if expired
    if(PlastikaPlayerData[playerid][plasState] == PLAS_COOLDOWN) {
        if(GetTickCount() < PlastikaPlayerData[playerid][plasCooldownTime])
            return PlasGRESKA(playerid, "Na cooldown-u ste! Sacekajte da istekne vrijeme.");
        // Cooldown expired - allow re-entry
        PlastikaPlayerData[playerid][plasState] = PLAS_NONE;
        PlastikaPlayerData[playerid][plasCooldownTime] = 0;
    }

    if(PlastikaPlayerData[playerid][plasState] != PLAS_NONE && PlastikaPlayerData[playerid][plasState] != PLAS_COOLDOWN)
        return PlasGRESKA(playerid, "Vec ste u procesu rada u fabrici plastike.");

    new Float:x,Float:y,Float:z; GetPlayerPos(playerid,x,y,z);
    if(PlasDist(x,y,z, PLAS_OPREMA_X,PLAS_OPREMA_Y,PLAS_OPREMA_Z) > PLAS_OPREMA_DIST) return PlasGRESKA(playerid, "Niste na lokaciji za uzimanje opreme.");
    if(IsPlayerInAnyVehicle(playerid)) return PlasGRESKA(playerid, "Ne mozete uzeti opremu dok ste u vozilu.");

    PlastikaPlayerData[playerid][plasOldSkin] = GetPlayerSkin(playerid);
    SetPlayerSkin(playerid, PLAS_OPREMA_SKIN);
    PlastikaPlayerData[playerid][plasEquipped] = true;
    PlastikaPlayerData[playerid][plasState] = PLAS_EQUIPPED;
    PlastikaPlayerData[playerid][plasToursDone] = 0;
    SetPVarInt(playerid, "PlastikaWorking", 1);

    PlayerPlaySound(playerid, PLAS_SND_OPREMA, 0.0, 0.0, 0.0);
    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Uzeli ste opremu za rad u fabrici plastike!");
    PlasINFO(playerid, "Mozete raditi na vilicaru (gajbe) ili na kamionu (bonus). Kucajte /plastikainfo.");
    Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Gajbe(ALT) ili Kamion(ALT)");
    return 1;
}

// ===========================================================================
//  FORKLIFT JOB - Pick up crate
// ===========================================================================

stock Plas_UzmiGajbu(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLAS_EQUIPPED) return PlasGRESKA(playerid, "Ne mozete sada uzeti gajbu.");
    if(!PlasNearGajba(playerid)) return PlasGRESKA(playerid, "Niste blizu palete sa gajbama.");
    if(IsPlayerInAnyVehicle(playerid)) return PlasGRESKA(playerid, "Ne mozete uzeti gajbu dok ste u vozilu.");

    SetPlayerAttachedObject(playerid, 3, PLAS_GAJBA_MODEL, 6, 0.07, 0.02, 0.0, 0.0, 80.0, 0.0, 0.85, 0.85, 0.85, 0x1D75C4FF, 0x1D75C4FF);
    PlastikaPlayerData[playerid][plasState] = PLAS_CARRYING_GAJBA;
    DisablePlayerCheckpoint(playerid);

    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Uzeli ste gajbu! Idite do stola i stisnite ALT.");
    Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Nosi gajbu na stol");
    PlayerPlaySound(playerid, PLAS_SND_OPREMA, 0.0, 0.0, 0.0);
    return 1;
}

// ===========================================================================
//  FORKLIFT JOB - Start work at table (minigame)
// ===========================================================================

stock Plas_PocniStol(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLAS_CARRYING_GAJBA) return PlasGRESKA(playerid, "Ne mozete sada poceti rad na stolu.");
    if(!PlasNearStol(playerid)) return PlasGRESKA(playerid, "Niste blizu radnog stola.");
    if(IsPlayerInAnyVehicle(playerid)) return PlasGRESKA(playerid, "Ne mozete raditi na stolu dok ste u vozilu.");

    // FIX: Do NOT use ApplyAnimation with freeze=1 - it blocks SPACE key detection!
    // Just set state and let player stand still naturally
    PlastikaPlayerData[playerid][plasState] = PLAS_MINIGAME;
    PlastikaPlayerData[playerid][plasMiniPhase] = 0;
    PlastikaPlayerData[playerid][plasMiniHits] = 0;
    // Marker position set in PlasMini_StartPhase
    DisablePlayerCheckpoint(playerid);

    PlasMini_Show(playerid);
    PlasMini_StartPhase(playerid);

    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} TERMO PRESOVANJE! Pritisni SPACE u zelenoj zoni!");
    return 1;
}

// ===========================================================================
//  FORKLIFT JOB - Insert box into machine
// ===========================================================================

stock Plas_UbaciUMasinu(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLAS_CARRYING_BOX) return PlasGRESKA(playerid, "Ne mozete sada ubaciti box u masinu.");
    if(!PlasNearMasina(playerid)) return PlasGRESKA(playerid, "Niste blizu masine.");
    if(IsPlayerInAnyVehicle(playerid)) return PlasGRESKA(playerid, "Ne mozete ubaciti box dok ste u vozilu.");

    if(IsPlayerAttachedObjectSlotUsed(playerid, 4)) RemovePlayerAttachedObject(playerid, 4);

    PlastikaPlayerData[playerid][plasState] = PLAS_MACHINE;
    PlastikaPlayerData[playerid][plasMachineProgress] = 0;
    DisablePlayerCheckpoint(playerid);

    Plas_ShowProgress(playerid, 0.0, "PRERADA PLASTIKE...");
    PlayerPlaySound(playerid, PLAS_SND_STROJ, 0.0, 0.0, 0.0);
    PlastikaPlayerData[playerid][plasTimerMachine] = SetTimerEx("PlasTimer_Machine", 1000, true, "i", playerid);

    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Box ubacen u masinu! Prerada u toku...");
    Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Prerada u masini...");
    return 1;
}

// ===========================================================================
//  FORKLIFT JOB - Spawn forklift with box
// ===========================================================================

stock Plas_SpawnVilicar(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLAS_CARRYING_BIGBOX) return PlasGRESKA(playerid, "Ne mozete sada uzeti vilicar.");
    if(!PlasNearVilicar(playerid)) return PlasGRESKA(playerid, "Niste blizu vilicara.");
    if(IsPlayerInAnyVehicle(playerid)) return PlasGRESKA(playerid, "Vec ste u vozilu.");

    new Float:x,Float:y,Float:z,Float:a;
    GetPlayerPos(playerid,x,y,z);
    if(PlasDist(x,y,z,PLAS_VIL1_X,PLAS_VIL1_Y,PLAS_VIL1_Z) < PlasDist(x,y,z,PLAS_VIL2_X,PLAS_VIL2_Y,PLAS_VIL2_Z)) {
        x=PLAS_VIL1_X; y=PLAS_VIL1_Y; z=PLAS_VIL1_Z; a=PLAS_VIL1_A;
    } else {
        x=PLAS_VIL2_X; y=PLAS_VIL2_Y; z=PLAS_VIL2_Z; a=PLAS_VIL2_A;
    }

    new vid = CreateVehicle(PLAS_VILICAR_MODEL, x,y,z,a, 79,79,-1);
    if(vid == INVALID_VEHICLE_ID) return PlasGRESKA(playerid, "Greska pri kreiranju vilicara.");

    PlastikaPlayerData[playerid][plasVehicleID] = vid;
    PutPlayerInVehicle(playerid, vid, 0);

    // Blue color, full health, full fuel, repair, engine on
    ChangeVehicleColor(vid, 79, 79);
    SetVehicleHealth(vid, 1000.0);
    RepairVehicle(vid);
    #if defined Fuel
        Fuel[vid] = 100;
    #endif
    #if defined vCanDrive
        vCanDrive[vid] = 1;
    #endif
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vid, VEHICLE_PARAMS_ON, lights, alarm, doors, bonnet, boot, objective);

    new boxObj = CreateDynamicObject(PLAS_BIGBOX_MODEL, 0.0,0.0,-1000.0, 0.0,0.0,0.0);
    SetDynamicObjectMaterial(boxObj, 0, 1631, "landjump", "plasticdrum1_128", 0);
    AttachDynamicObjectToVehicle(boxObj, vid, 0.000, 0.870, 0.769, 0.000, 0.000, 0.000);
    PlastikaPlayerData[playerid][plasBoxObjID] = boxObj;

    PlastikaPlayerData[playerid][plasState] = PLAS_FORKLIFT;
    PlastikaPlayerData[playerid][plasTargetSkladiste] = 1 + random(4);

    new Float:sklX,Float:sklY,Float:sklZ;
    PlasSkladPos(PlastikaPlayerData[playerid][plasTargetSkladiste], sklX,sklY,sklZ);
    SetPlayerCheckpoint(playerid, sklX,sklY,sklZ, 4.0);

    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Vilicar sa big boxom spreman! Vozite do skladista.");
    Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Vozi do skladista");
    PlayerPlaySound(playerid, PLAS_SND_USPJEH, 0.0, 0.0, 0.0);
    return 1;
}

// ===========================================================================
//  FORKLIFT JOB - Deliver to storage
// ===========================================================================

stock Plas_OstaviUSkladiste(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLAS_FORKLIFT) return 0;
    if(!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleID(playerid) != PlastikaPlayerData[playerid][plasVehicleID])
        return PlasGRESKA(playerid, "Morate biti u vilicaru!");

    new Float:vx,Float:vy,Float:vz;
    GetVehiclePos(PlastikaPlayerData[playerid][plasVehicleID], vx,vy,vz);
    new Float:sklX,Float:sklY,Float:sklZ;
    PlasSkladPos(PlastikaPlayerData[playerid][plasTargetSkladiste], sklX,sklY,sklZ);
    if(PlasDist(vx,vy,vz, sklX,sklY,sklZ) > PLAS_SKL_DIST) return 0;

    if(PlastikaPlayerData[playerid][plasBoxObjID] != -1) { DestroyDynamicObject(PlastikaPlayerData[playerid][plasBoxObjID]); PlastikaPlayerData[playerid][plasBoxObjID]=-1; }
    if(PlastikaPlayerData[playerid][plasVehicleID] != -1) {
        new vid = PlastikaPlayerData[playerid][plasVehicleID];
        RemovePlayerFromVehicle(playerid);
        DestroyVehicle(vid);
        PlastikaPlayerData[playerid][plasVehicleID] = -1;
    }

    DisablePlayerCheckpoint(playerid);
    Plas_HideProgress(playerid);

    // Tiered reward based on minigame hits
    new hits = PlastikaPlayerData[playerid][plasMiniHits];
    new pay, plas;
    switch(hits) {
        case 1: { pay = PlasRand(PLAS_PAY_1_MIN, PLAS_PAY_1_MAX); plas = PLAS_PLAS_1; }
        case 2: { pay = PlasRand(PLAS_PAY_2_MIN, PLAS_PAY_2_MAX); plas = PlasRand(PLAS_PLAS_2_MIN, PLAS_PLAS_2_MAX); }
        case 3: { pay = PlasRand(PLAS_PAY_3_MIN, PLAS_PAY_3_MAX); plas = PlasRand(PLAS_PLAS_3_MIN, PLAS_PLAS_3_MAX); if(random(100) < 30) plas = PLAS_PLAS_3_MAX; }
        default: { pay = PlasRand(PLAS_PAY_1_MIN, PLAS_PAY_1_MAX); plas = PLAS_PLAS_1; }
    }
    GivePlayerMoney(playerid, pay);
    PlasInv_Add(playerid, plas);

    PlastikaPlayerData[playerid][plasToursDone]++;

    new str[128];
    format(str, sizeof(str), "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Box ostavljen! (%d/3) Zaradili ste {00FF00}$%d{FFFFFF}.", hits, pay);
    PlasMSG(playerid, str);
    format(str, sizeof(str), "Dobili ste {1D75C4}%d Plastike{FFFFFF} u inventory!", plas);
    PlasSUCCESS(playerid, str);
    format(str, sizeof(str), "Tura #%d zavrsena! Uzmite novu gajbu za slijedecu turu.", PlastikaPlayerData[playerid][plasToursDone]);
    PlasINFO(playerid, str);

    PlayerPlaySound(playerid, PLAS_SND_NAPLATA, 0.0, 0.0, 0.0);
    // Back to EQUIPPED - can immediately do another forklift run
    PlastikaPlayerData[playerid][plasState] = PLAS_EQUIPPED;
    Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Uzmi novu gajbu ili idi do kamiona");
    return 1;
}

// ===========================================================================
//  BONUS TRUCK JOB - Spawn truck (DFT-30)
// ===========================================================================

stock Plas_SpawnKamion(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLAS_EQUIPPED) return PlasGRESKA(playerid, "Ne mozete sada uzeti kamion.");
    if(!PlasNearKamion(playerid)) return PlasGRESKA(playerid, "Niste blizu kamiona.");
    if(IsPlayerInAnyVehicle(playerid)) return PlasGRESKA(playerid, "Vec ste u vozilu.");

    new vid = CreateVehicle(PLAS_KAMION_MODEL, PLAS_KAMION_X,PLAS_KAMION_Y,PLAS_KAMION_Z,PLAS_KAMION_A, 79,79,-1);
    if(vid == INVALID_VEHICLE_ID) return PlasGRESKA(playerid, "Greska pri kreiranju kamiona.");

    PlastikaPlayerData[playerid][plasVehicleID] = vid;
    PutPlayerInVehicle(playerid, vid, 0);

    // Blue color, full health, full fuel, repair, engine on
    ChangeVehicleColor(vid, 79, 79);
    SetVehicleHealth(vid, 1000.0);
    RepairVehicle(vid);
    #if defined Fuel
        Fuel[vid] = 100;
    #endif
    #if defined vCanDrive
        vCanDrive[vid] = 1;
    #endif

    // Set engine on
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vid, VEHICLE_PARAMS_ON, lights, alarm, doors, bonnet, boot, objective);

    PlastikaPlayerData[playerid][plasState] = PLAS_TRUCK_DRIVING;
    SetPlayerCheckpoint(playerid, PLAS_UTOVAR_X, PLAS_UTOVAR_Y, PLAS_UTOVAR_Z, 6.0);

    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE - BONUS]{FFFFFF} Kamion spreman! Vozite do utovara i stisnite CTRL.");
    Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Vozi kamion do utovara (CTRL)");
    PlayerPlaySound(playerid, PLAS_SND_OPREMA, 0.0, 0.0, 0.0);
    return 1;
}

// ===========================================================================
//  BONUS TRUCK JOB - Start loading (utovar)
// ===========================================================================

stock Plastika_PocniUtovar(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLAS_TRUCK_DRIVING) return 0;
    if(!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleID(playerid) != PlastikaPlayerData[playerid][plasVehicleID])
        return PlasGRESKA(playerid, "Morate biti u kamionu!");
    if(!PlasVehicleNearUtovar(playerid)) return 0;

    PlastikaPlayerData[playerid][plasState] = PLAS_TRUCK_LOADING;
    PlastikaPlayerData[playerid][plasUtovarProgress] = 0;
    DisablePlayerCheckpoint(playerid);

    // Freeze player during loading
    TogglePlayerControllable(playerid, 0);

    Plas_ShowProgress(playerid, 0.0, "UTOVAR CIJEVI...");
    PlayerPlaySound(playerid, PLAS_SND_STROJ, 0.0, 0.0, 0.0);
    PlastikaPlayerData[playerid][plasTimerUtovar] = SetTimerEx("PlasTimer_Utovar", 1000, true, "i", playerid);

    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE - BONUS]{FFFFFF} Utovar cijevi u toku...");
    Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Utovar cijevi...");
    return 1;
}

// ===========================================================================
//  BONUS TRUCK JOB - Start unloading (istovar at dostava)
// ===========================================================================

stock Plastika_PocniIstovar(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLAS_TRUCK_DELIVERING) return 0;
    if(!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleID(playerid) != PlastikaPlayerData[playerid][plasVehicleID])
        return PlasGRESKA(playerid, "Morate biti u kamionu!");
    if(!PlasVehicleNearDostava(playerid)) return PlasGRESKA(playerid, "Niste blizu dostave!");

    PlastikaPlayerData[playerid][plasState] = PLAS_TRUCK_UNLOADING;
    PlastikaPlayerData[playerid][plasIstovarProgress] = 0;
    DisablePlayerCheckpoint(playerid);

    // Freeze player during unloading
    TogglePlayerControllable(playerid, 0);

    Plas_ShowProgress(playerid, 0.0, "ISTOVAR CIJEVI...");
    PlayerPlaySound(playerid, PLAS_SND_STROJ, 0.0, 0.0, 0.0);
    PlastikaPlayerData[playerid][plasTimerIstovar] = SetTimerEx("PlasTimer_Istovar", 1000, true, "i", playerid);

    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE - BONUS]{FFFFFF} Istovar cijevi u toku...");
    Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Istovar cijevi...");
    return 1;
}

// ===========================================================================
//  BONUS TRUCK JOB - Return to base (reward + cooldown)
// ===========================================================================

stock Plastika_PovratakBaza(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLAS_TRUCK_RETURNING) return 0;
    if(!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleID(playerid) != PlastikaPlayerData[playerid][plasVehicleID])
        return PlasGRESKA(playerid, "Morate biti u kamionu!");
    if(!PlasVehicleNearKamionBaza(playerid)) return 0;

    // Destroy truck - crveni CP, kamion nestaje
    if(PlastikaPlayerData[playerid][plasBoxObjID] != -1) { DestroyDynamicObject(PlastikaPlayerData[playerid][plasBoxObjID]); PlastikaPlayerData[playerid][plasBoxObjID]=-1; }
    if(PlastikaPlayerData[playerid][plasVehicleID] != -1) {
        new vid = PlastikaPlayerData[playerid][plasVehicleID];
        RemovePlayerFromVehicle(playerid);
        #if defined Fuel
            Fuel[vid] = 0;
        #endif
        #if defined vCanDrive
            vCanDrive[vid] = 0;
        #endif
        DestroyVehicle(vid);
        PlastikaPlayerData[playerid][plasVehicleID] = -1;
    }

    DisablePlayerCheckpoint(playerid);
    Plas_HideProgress(playerid);

    // Reward (random between min and max)
    new bonusPay = PlasRand(PLAS_BONUS_PAY_MIN, PLAS_BONUS_PAY_MAX);
    new bonusPlas = PlasRand(PLAS_BONUS_PLAS_MIN, PLAS_BONUS_PLAS_MAX);
    GivePlayerMoney(playerid, bonusPay);
    PlasInv_Add(playerid, bonusPlas);

    new str[128];
    format(str, sizeof(str), "{1D75C4}[FABRIKA PLASTIKE - BONUS]{FFFFFF} Dostava zavrsena! Zaradili ste {00FF00}$%d{FFFFFF}.", bonusPay);
    PlasMSG(playerid, str);
    format(str, sizeof(str), "Dobili ste {1D75C4}%d Plastike{FFFFFF} u inventory!", bonusPlas);
    PlasSUCCESS(playerid, str);

    PlayerPlaySound(playerid, PLAS_SND_NAPLATA, 0.0, 0.0, 0.0);

    // Start 30 min cooldown
    PlastikaPlayerData[playerid][plasCooldownTime] = GetTickCount() + (PLAS_COOLDOWN_SEC * 1000);
    PlastikaPlayerData[playerid][plasState] = PLAS_COOLDOWN;

    new minLeft = PLAS_COOLDOWN_SEC / 60;
    format(str, sizeof(str), "Cooldown: %d minuta. Kucajte /uzmiopremup nakon sto istekne.", minLeft);
    PlasINFO(playerid, str);
    Plas_Status(playerid, "FABRIKA PLASTIKE~n~~w~Status: Cooldown - sacekajte");
    return 1;
}

// ===========================================================================
//  SKIN OPREMU / STOP JOB
// ===========================================================================

stock Plastika_SkinOpremu(playerid) {
    if(!PlastikaPlayerData[playerid][plasEquipped]) return PlasGRESKA(playerid, "Nemate opremu za skidanje.");
    if(PlastikaPlayerData[playerid][plasState] == PLAS_MINIGAME) return PlasGRESKA(playerid, "Ne mozete skinuti opremu tokom minigame-a!");
    if(PlastikaPlayerData[playerid][plasState] == PLAS_MACHINE) return PlasGRESKA(playerid, "Ne mozete skinuti opremu dok masina radi!");
    if(PlastikaPlayerData[playerid][plasState] == PLAS_FORKLIFT) return PlasGRESKA(playerid, "Ne mozete skinuti opremu dok vozite vilicar!");
    if(PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_LOADING) return PlasGRESKA(playerid, "Ne mozete skinuti opremu dok traje utovar!");
    if(PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_UNLOADING) return PlasGRESKA(playerid, "Ne mozete skinuti opremu dok traje istovar!");
    if(PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_DRIVING || PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_DELIVERING || PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_RETURNING)
        return PlasGRESKA(playerid, "Ne mozete skinuti opremu dok vozite kamion!");
    Plastika_StopJob(playerid);
    PlasSUCCESS(playerid, "Skinuli ste opremu i prestali raditi u fabrici plastike.");
    return 1;
}

// ===========================================================================
//  INIT
// ===========================================================================

stock Plastika_Init() {
    // Oprema - info pickup (1239 = info ikona)
    PlasPickup_Oprema = CreateDynamicPickup(1239, 1, PLAS_OPREMA_X,PLAS_OPREMA_Y,PLAS_OPREMA_Z);
    Plas3D_Oprema = CreateDynamic3DTextLabel("{1D75C4}[FABRIKA PLASTIKE - OPREMA]{FFFFFF}\nKucaj /uzmiopremup", 0xFFFFFFFF, PLAS_OPREMA_X,PLAS_OPREMA_Y,PLAS_OPREMA_Z+0.3, 10.0);

    // Gajbe - info pickup
    PlasPickup_Gajba1 = CreateDynamicPickup(1239, 1, PLAS_GAJBA1_X,PLAS_GAJBA1_Y,PLAS_GAJBA1_Z);
    Plas3D_Gajba1 = CreateDynamic3DTextLabel("{1D75C4}[GAJBE - STRANA 1]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} da uzmes gajbu", 0xFFFFFFFF, PLAS_GAJBA1_X,PLAS_GAJBA1_Y,PLAS_GAJBA1_Z+0.3, 8.0);

    PlasPickup_Gajba2 = CreateDynamicPickup(1239, 1, PLAS_GAJBA2_X,PLAS_GAJBA2_Y,PLAS_GAJBA2_Z);
    Plas3D_Gajba2 = CreateDynamic3DTextLabel("{1D75C4}[GAJBE - STRANA 2]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} da uzmes gajbu", 0xFFFFFFFF, PLAS_GAJBA2_X,PLAS_GAJBA2_Y,PLAS_GAJBA2_Z+0.3, 8.0);

    // Radni stolovi - NO pickups, only 3D text labels
    new Float:stolX[4] = {PLAS_STOL1_X,PLAS_STOL2_X,PLAS_STOL3_X,PLAS_STOL4_X};
    new Float:stolY[4] = {PLAS_STOL1_Y,PLAS_STOL2_Y,PLAS_STOL3_Y,PLAS_STOL4_Y};
    new Float:stolZ[4] = {PLAS_STOL1_Z,PLAS_STOL2_Z,PLAS_STOL3_Z,PLAS_STOL4_Z};
    for(new i = 0; i < 4; i++) {
        new lbl[128]; format(lbl, sizeof(lbl), "{FFD700}[RADNI STOL %d]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} za preradu", i+1);
        Plas3D_Stol[i] = CreateDynamic3DTextLabel(lbl, 0xFFFFFFFF, stolX[i],stolY[i],stolZ[i]+0.3, 5.0);
    }

    // Masine - NO pickups, only 3D text labels
    new Float:masX[3] = {PLAS_MAS1_X,PLAS_MAS2_X,PLAS_MAS3_X};
    new Float:masY[3] = {PLAS_MAS1_Y,PLAS_MAS2_Y,PLAS_MAS3_Y};
    new Float:masZ[3] = {PLAS_MAS1_Z,PLAS_MAS2_Z,PLAS_MAS3_Z};
    for(new i = 0; i < 3; i++) {
        new lbl[128]; format(lbl, sizeof(lbl), "{00FF00}[MASINA %d]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} za preradu", i+1);
        Plas3D_Mas[i] = CreateDynamic3DTextLabel(lbl, 0xFFFFFFFF, masX[i],masY[i],masZ[i]+0.3, 5.0);
    }

    // Vilicari - info pickup
    PlasPickup_Vil[0] = CreateDynamicPickup(1239, 1, PLAS_VIL1_X,PLAS_VIL1_Y,PLAS_VIL1_Z);
    Plas3D_Vil[0] = CreateDynamic3DTextLabel("{1D75C4}[VILICAR - STRANA 1]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} za vilicar", 0xFFFFFFFF, PLAS_VIL1_X,PLAS_VIL1_Y,PLAS_VIL1_Z+0.3, 5.0);

    PlasPickup_Vil[1] = CreateDynamicPickup(1239, 1, PLAS_VIL2_X,PLAS_VIL2_Y,PLAS_VIL2_Z);
    Plas3D_Vil[1] = CreateDynamic3DTextLabel("{1D75C4}[VILICAR - STRANA 2]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} za vilicar", 0xFFFFFFFF, PLAS_VIL2_X,PLAS_VIL2_Y,PLAS_VIL2_Z+0.3, 5.0);

    // Skladista - no pickups, just 3D labels
    new Float:sklX[4] = {PLAS_SKL1_X,PLAS_SKL2_X,PLAS_SKL3_X,PLAS_SKL4_X};
    new Float:sklY[4] = {PLAS_SKL1_Y,PLAS_SKL2_Y,PLAS_SKL3_Y,PLAS_SKL4_Y};
    new Float:sklZ[4] = {PLAS_SKL1_Z,PLAS_SKL2_Z,PLAS_SKL3_Z,PLAS_SKL4_Z};
    for(new i = 0; i < 4; i++) {
        new lbl[96]; format(lbl, sizeof(lbl), "{1D75C4}[SKLADISTE %d]{FFFFFF}\nOstavi box ovdje", i+1);
        Plas3D_Skl[i] = CreateDynamic3DTextLabel(lbl, 0xFFFFFFFF, sklX[i],sklY[i],sklZ[i]+0.3, 8.0);
    }

    // BONUS TRUCK JOB locations
    // Kamion - small blue marker (1237 = small blue sign)
    PlasPickup_Kamion = CreateDynamicPickup(1237, 1, PLAS_KAMION_X,PLAS_KAMION_Y,PLAS_KAMION_Z);
    Plas3D_Kamion = CreateDynamic3DTextLabel("{1D75C4}[BONUS - KAMION]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} za kamion", 0xFFFFFFFF, PLAS_KAMION_X,PLAS_KAMION_Y,PLAS_KAMION_Z+0.3, 8.0);

    // Utovar - NO pickup needed (CTRL interaction while in vehicle), just 3D label
    Plas3D_Utovar = CreateDynamic3DTextLabel("{1D75C4}[UTOVAR CIJEVI]{FFFFFF}\nStisni {FFD700}CTRL{FFFFFF} za utovar cijevi", 0xFFFFFFFF, PLAS_UTOVAR_X,PLAS_UTOVAR_Y,PLAS_UTOVAR_Z+0.3, 8.0);

    // Dostava - small blue marker (1237)
    PlasPickup_Dostava = CreateDynamicPickup(1237, 1, PLAS_DOSTAVA_X,PLAS_DOSTAVA_Y,PLAS_DOSTAVA_Z);
    Plas3D_Dostava = CreateDynamic3DTextLabel("{1D75C4}[DOSTAVA]{FFFFFF}\nStisni {FFD700}CTRL{FFFFFF} za istovar", 0xFFFFFFFF, PLAS_DOSTAVA_X,PLAS_DOSTAVA_Y,PLAS_DOSTAVA_Z+0.3, 10.0);

    PlasMapIcon = CreateDynamicMapIcon(PLAS_OPREMA_X,PLAS_OPREMA_Y,PLAS_OPREMA_Z, 55, 0);

    // Baza return point - crveni CP za bonus kamion
    CreateDynamicCP(PLAS_BAZA_X, PLAS_BAZA_Y, PLAS_BAZA_Z, 6.0, -1, -1, -1, 50.0);
    CreateDynamic3DTextLabel("{FF6347}[BAZA - PLATA]{FFFFFF}\nVozite kamion ovde", 0xFFFFFFFF, PLAS_BAZA_X, PLAS_BAZA_Y, PLAS_BAZA_Z+0.3, 15.0);

    print("[PLASTIKA v3.1] Sistem fabrike plastike (Forklift + Bonus Truck) uspjesno ucitan!");
    return 1;
}

// ===========================================================================
//  CONNECT / DISCONNECT / DEATH
// ===========================================================================

stock Plastika_OnConnect(playerid) {
    PlastikaPlayerData[playerid][plasState] = PLAS_NONE;
    PlastikaPlayerData[playerid][plasOldSkin] = -1;
    PlastikaPlayerData[playerid][plasEquipped] = false;
    PlastikaPlayerData[playerid][plasVehicleID] = -1;
    PlastikaPlayerData[playerid][plasBoxObjID] = -1;
    PlastikaPlayerData[playerid][plasTimerMini] = -1;
    PlastikaPlayerData[playerid][plasTimerMachine] = -1;
    PlastikaPlayerData[playerid][plasTimerUtovar] = -1;
    PlastikaPlayerData[playerid][plasTimerIstovar] = -1;
    PlastikaPlayerData[playerid][plasCooldownTime] = 0;
    PlastikaPlayerData[playerid][plasMiniPhase] = 0;
    PlastikaPlayerData[playerid][plasMiniHits] = 0;
    PlastikaPlayerData[playerid][plasMarkerPos] = 0.0;
    PlastikaPlayerData[playerid][plasMGDirection] = 1.0;
    PlastikaPlayerData[playerid][plasMGTargetPos] = 50.0;
    PlastikaPlayerData[playerid][plasMGZoneSize] = 18.0;
    PlastikaPlayerData[playerid][plasMachineProgress] = 0;
    PlastikaPlayerData[playerid][plasUtovarProgress] = 0;
    PlastikaPlayerData[playerid][plasIstovarProgress] = 0;
    PlastikaPlayerData[playerid][plasTargetSkladiste] = 0;
    PlastikaPlayerData[playerid][plasToursDone] = 0;
    PlastikaPlayerData[playerid][plasTDShown] = false;
    PlastikaPlayerData[playerid][plasMiniShown] = false;

    Plas_CreateProgressTDs(playerid);
    PlasMini_CreateTDs(playerid);
    return 1;
}

stock Plastika_OnDisconnect(playerid) {
    // Kill ALL timers
    if(PlastikaPlayerData[playerid][plasTimerMini] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerMini]); PlastikaPlayerData[playerid][plasTimerMini]=-1; }
    if(PlastikaPlayerData[playerid][plasTimerMachine] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerMachine]); PlastikaPlayerData[playerid][plasTimerMachine]=-1; }
    if(PlastikaPlayerData[playerid][plasTimerUtovar] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerUtovar]); PlastikaPlayerData[playerid][plasTimerUtovar]=-1; }
    if(PlastikaPlayerData[playerid][plasTimerIstovar] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerIstovar]); PlastikaPlayerData[playerid][plasTimerIstovar]=-1; }

    // Destroy vehicle and attached object
    if(PlastikaPlayerData[playerid][plasVehicleID] != -1) {
        new vid = PlastikaPlayerData[playerid][plasVehicleID];
        if(GetVehicleModel(vid) != 0) { for(new i=0;i<MAX_PLAYERS;i++) if(IsPlayerInVehicle(i,vid)) RemovePlayerFromVehicle(i); DestroyVehicle(vid); }
        PlastikaPlayerData[playerid][plasVehicleID] = -1;
    }
    if(PlastikaPlayerData[playerid][plasBoxObjID] != -1) { DestroyDynamicObject(PlastikaPlayerData[playerid][plasBoxObjID]); PlastikaPlayerData[playerid][plasBoxObjID]=-1; }

    if(IsPlayerAttachedObjectSlotUsed(playerid, 3)) RemovePlayerAttachedObject(playerid, 3);
    if(IsPlayerAttachedObjectSlotUsed(playerid, 4)) RemovePlayerAttachedObject(playerid, 4);

    Plas_HideProgress(playerid);
    PlasMini_Hide(playerid);
    DisablePlayerCheckpoint(playerid);

    if(PlastikaPlayerData[playerid][plasEquipped] && PlastikaPlayerData[playerid][plasOldSkin] != -1)
        SetPlayerSkin(playerid, PlastikaPlayerData[playerid][plasOldSkin]);

    PlastikaPlayerData[playerid][plasState] = PLAS_NONE;
    PlastikaPlayerData[playerid][plasEquipped] = false;
    DeletePVar(playerid, "PlastikaWorking");
    return 1;
}

stock Plastika_OnDeath(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLAS_NONE) {
        TogglePlayerControllable(playerid, 1);
        Plastika_StopJob(playerid);
        PlasGRESKA(playerid, "Prekinuli ste rad u fabrici plastike jer ste umrli.");
    }
    return 1;
}

// ===========================================================================
//  CHECKPOINT
// ===========================================================================

stock Plastika_OnCP(playerid) {
    if(PlastikaPlayerData[playerid][plasState] == PLAS_NONE) return 0;

    // Forklift - checkpoint at skladiste
    if(PlastikaPlayerData[playerid][plasState] == PLAS_FORKLIFT) {
        if(IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == PlastikaPlayerData[playerid][plasVehicleID]) {
            Plas_OstaviUSkladiste(playerid);
            return 1;
        }
    }

    // Truck - utovar handled via CTRL key (not CP)

    // Truck - return to base at checkpoint
    if(PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_RETURNING) {
        if(IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == PlastikaPlayerData[playerid][plasVehicleID]) {
            if(PlasVehicleNearKamionBaza(playerid)) {
                Plastika_PovratakBaza(playerid);
                return 1;
            }
        }
    }

    return 0;
}

// ===========================================================================
//  KEY STATE CHANGE
// ===========================================================================

stock Plastika_OnKeyStateChange(playerid, newkeys, oldkeys) {
    if(PlastikaPlayerData[playerid][plasState] == PLAS_NONE) return 0;

    // SPACE (KEY_SPRINT) za minigame
    if((newkeys & KEY_SPRINT) && !(oldkeys & KEY_SPRINT)) {
        if(PlastikaPlayerData[playerid][plasState] == PLAS_MINIGAME) {
            new Float:progress = PlastikaPlayerData[playerid][plasMarkerPos];
            new Float:target = PlastikaPlayerData[playerid][plasMGTargetPos];
            new Float:halfZone = PlastikaPlayerData[playerid][plasMGZoneSize] / 2.0;
            new bool:inZone = (progress >= (target - halfZone) && progress <= (target + halfZone));
            PlasMini_StopPhase(playerid, inZone);
            return 1;
        }
    }

    // ALT = KEY_WALK (na nogama)
    if((newkeys & KEY_WALK) && !(oldkeys & KEY_WALK)) {
        if(IsPlayerInAnyVehicle(playerid)) return 0;

        switch(PlastikaPlayerData[playerid][plasState]) {
            case PLAS_EQUIPPED: {
                if(PlasNearGajba(playerid)) { Plas_UzmiGajbu(playerid); return 1; }
                if(PlasNearKamion(playerid)) { Plas_SpawnKamion(playerid); return 1; }
            }
            case PLAS_CARRYING_GAJBA: {
                if(PlasNearStol(playerid)) { Plas_PocniStol(playerid); return 1; }
            }
            case PLAS_CARRYING_BOX: {
                if(PlasNearMasina(playerid)) { Plas_UbaciUMasinu(playerid); return 1; }
            }
            case PLAS_CARRYING_BIGBOX: {
                if(PlasNearVilicar(playerid)) { Plas_SpawnVilicar(playerid); return 1; }
            }
        }
    }

    // CTRL/KEY_HANDBRAKE u vozilu - utovar, istovar ili ostavljanje u skladistu
    if((newkeys & KEY_HANDBRAKE) && !(oldkeys & KEY_HANDBRAKE)) {
        // Truck - CTRL at utovar for loading pipe
        if(PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_DRIVING) {
            if(IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == PlastikaPlayerData[playerid][plasVehicleID]) {
                if(PlasVehicleNearUtovar(playerid)) {
                    Plastika_PocniUtovar(playerid); return 1;
                }
            }
        }
        // Truck - CTRL at dostava for unloading
        if(PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_DELIVERING) {
            if(IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == PlastikaPlayerData[playerid][plasVehicleID]) {
                Plastika_PocniIstovar(playerid); return 1;
            }
        }
        // Forklift - CTRL at skladiste
        if(PlastikaPlayerData[playerid][plasState] == PLAS_FORKLIFT) {
            if(IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == PlastikaPlayerData[playerid][plasVehicleID]) {
                Plas_OstaviUSkladiste(playerid); return 1;
            }
        }
    }

    // KEY_SUBMISSION also works for vehicle controls
    if((newkeys & KEY_SUBMISSION) && !(oldkeys & KEY_SUBMISSION)) {
        // Truck - KEY_SUBMISSION at utovar for loading pipe
        if(PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_DRIVING) {
            if(IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == PlastikaPlayerData[playerid][plasVehicleID]) {
                if(PlasVehicleNearUtovar(playerid)) {
                    Plastika_PocniUtovar(playerid); return 1;
                }
            }
        }
        // Truck - KEY_SUBMISSION at dostava for unloading
        if(PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_DELIVERING) {
            if(IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == PlastikaPlayerData[playerid][plasVehicleID]) {
                Plastika_PocniIstovar(playerid); return 1;
            }
        }
        // Forklift - KEY_SUBMISSION at skladiste
        if(PlastikaPlayerData[playerid][plasState] == PLAS_FORKLIFT) {
            if(IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == PlastikaPlayerData[playerid][plasVehicleID]) {
                Plas_OstaviUSkladiste(playerid); return 1;
            }
        }
    }

    return 0;
}

// ===========================================================================
//  ONUPDATE / VEHICLE EVENTS
// ===========================================================================

stock Plastika_OnUpdate(playerid) return 0;

stock Plastika_OnVehicleDeath(vehicleid) {
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(!IsPlayerConnected(i)) continue;
        if(PlastikaPlayerData[i][plasVehicleID] == vehicleid) {
            PlastikaPlayerData[i][plasVehicleID] = -1;
            if(PlastikaPlayerData[i][plasBoxObjID] != -1) { DestroyDynamicObject(PlastikaPlayerData[i][plasBoxObjID]); PlastikaPlayerData[i][plasBoxObjID]=-1; }
            if(PlastikaPlayerData[i][plasTimerMini] != -1) { KillTimer(PlastikaPlayerData[i][plasTimerMini]); PlastikaPlayerData[i][plasTimerMini]=-1; }
            if(PlastikaPlayerData[i][plasTimerMachine] != -1) { KillTimer(PlastikaPlayerData[i][plasTimerMachine]); PlastikaPlayerData[i][plasTimerMachine]=-1; }
            if(PlastikaPlayerData[i][plasTimerUtovar] != -1) { KillTimer(PlastikaPlayerData[i][plasTimerUtovar]); PlastikaPlayerData[i][plasTimerUtovar]=-1; }
            if(PlastikaPlayerData[i][plasTimerIstovar] != -1) { KillTimer(PlastikaPlayerData[i][plasTimerIstovar]); PlastikaPlayerData[i][plasTimerIstovar]=-1; }
            Plas_HideProgress(i);
            PlasMini_Hide(i);
            DisablePlayerCheckpoint(i);
            if(PlastikaPlayerData[i][plasEquipped] && PlastikaPlayerData[i][plasOldSkin] != -1) SetPlayerSkin(i, PlastikaPlayerData[i][plasOldSkin]);
            PlastikaPlayerData[i][plasState] = PLAS_NONE;
            PlastikaPlayerData[i][plasEquipped] = false;
            DeletePVar(i, "PlastikaWorking");

            // Different message depending on vehicle type
            new vmodel = GetVehicleModel(vehicleid);
            if(vmodel == PLAS_KAMION_MODEL)
                PlasGRESKA(i, "Kamion je unisten! Bonus posao prekinut.");
            else if(vmodel == PLAS_VILICAR_MODEL)
                PlasGRESKA(i, "Vilicar je unisten! Posao prekinut.");
            else
                PlasGRESKA(i, "Vozilo je unisteno! Posao prekinut.");
            break;
        }
    }
    return 1;
}

stock Plastika_OnPlayerExitVehicle(playerid, vehicleid) {
    if(PlastikaPlayerData[playerid][plasVehicleID] == vehicleid) {
        if(PlastikaPlayerData[playerid][plasState] == PLAS_FORKLIFT)
            PlasINFO(playerid, "Izasli ste iz vilicara! Vratite se da nastavite do skladista.");
        else if(PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_DRIVING)
            PlasINFO(playerid, "Izasli ste iz kamiona! Vratite se da nastavite do utovara.");
        else if(PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_DELIVERING)
            PlasINFO(playerid, "Izasli ste iz kamiona! Vratite se da nastavite do dostave.");
        else if(PlastikaPlayerData[playerid][plasState] == PLAS_TRUCK_RETURNING)
            PlasINFO(playerid, "Izasli ste iz kamiona! Vratite se da nastavite do baze.");
    }
    return 1;
}

// ===========================================================================
//  INFO / ADMIN
// ===========================================================================

stock Plastika_Info(playerid) {
    new str[1024];
    format(str, sizeof(str),
        "{1D75C4}=== FABRIKA PLASTIKE ===\n\
        {FFFFFF}\n\
        {FFD700}=== KOMANDE ==={FFFFFF}\n\
        /uzmiopremup - Uzmi opremu za rad u fabrici\n\
        /prekidaplastiku - Prekini posao i skini opremu\n\
        /plastikainfo - Prikazi ovaj info\n\
        \n\
        {FFD700}=== FORKLIFT JOB ==={FFFFFF}\n\
        ALT kod gajbi - Uzmi plasticnu gajbu\n\
        ALT kod stola - Pokreni TERMO PRESOVANJE (3 faze, SPACE)\n\
        ALT kod masine - Ubaci box u masinu za preradu\n\
        ALT kod vilicara - Pokupi vilicar sa big boxom\n\
        CTRL kod skladista - Ostavi box i primi platu\n\
        \n\
        {FF6347}=== BONUS TRUCK JOB (30 min cooldown) ==={FFFFFF}\n\
        ALT kod kamiona - Pokupi kamion sa cijevi\n\
        CTRL kod utovara - Utovari cijev na kamion\n\
        CTRL kod dostave - Istovari cijev na destinaciji\n\
        Voz kamion do baze (crveni CP) - Primi platu i plastiku");
    ShowPlayerDialog(playerid, 9117, DIALOG_STYLE_MSGBOX, "{1D75C4}Fabrika Plastike", str, "Zatvori", "");
    return 1;
}

stock Plastika_ResetCooldown(playerid) {
    if(!PlasValid(playerid)) return 0;
    PlastikaPlayerData[playerid][plasCooldownTime] = 0;
    if(PlastikaPlayerData[playerid][plasState] == PLAS_COOLDOWN) {
        PlastikaPlayerData[playerid][plasState] = PLAS_NONE;
        PlastikaPlayerData[playerid][plasEquipped] = false;
        if(PlastikaPlayerData[playerid][plasOldSkin] != -1) {
            SetPlayerSkin(playerid, PlastikaPlayerData[playerid][plasOldSkin]);
            PlastikaPlayerData[playerid][plasOldSkin] = -1;
        }
        DeletePVar(playerid, "PlastikaWorking");
        Plas_HideProgress(playerid);
        Plas_Status(playerid, "");
        PlasSUCCESS(playerid, "Cooldown resetovan! Mozete ponovo uzeti opremu.");
    }
    return 1;
}

stock Plastika_SetCooldown(playerid, seconds) {
    if(!PlasValid(playerid)) return 0;
    PlastikaPlayerData[playerid][plasCooldownTime] = GetTickCount() + (seconds * 1000);
    if(PlastikaPlayerData[playerid][plasState] == PLAS_NONE || PlastikaPlayerData[playerid][plasState] == PLAS_EQUIPPED) {
        PlastikaPlayerData[playerid][plasState] = PLAS_COOLDOWN;
        if(!PlastikaPlayerData[playerid][plasEquipped]) {
            PlastikaPlayerData[playerid][plasOldSkin] = GetPlayerSkin(playerid);
            SetPlayerSkin(playerid, PLAS_OPREMA_SKIN);
            PlastikaPlayerData[playerid][plasEquipped] = true;
            SetPVarInt(playerid, "PlastikaWorking", 1);
        }
        new str[64]; format(str, sizeof(str), "Cooldown postavljen: %d sekundi.", seconds);
        PlasINFO(playerid, str);
    }
    return 1;
}

stock Plastika_GivePlastika(playerid, kolicina) {
    if(!PlasValid(playerid)) return 0;
    return PlasInv_Add(playerid, kolicina);
}
