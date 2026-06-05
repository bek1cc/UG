// ===========================================================================
//  SISTEM FABRIKE PLASTIKE v2.0 - SKLADISTENJE PLASTIKE
//  Autor: Beka (uz pomoc AI asistenta)
//  Opis: Kompletan, modularan sistem fabrike plastike za SA-MP server.
//        2. dio posla - "Skladistenje Plastike" (bonus posao).
//        Igrac uzima opremu, uzima gajbu, ide na stol, igra TERMO PRESOVANJE
//        minigame (3 faze), nosi box do masine, masina preradjuje u big box,
//        spawn vilicar sa boxom, vozi do skladista, ostavlja = zarada.
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
// ===========================================================================

#if defined _plastika_included
    #endinput
#endif
#define _plastika_included

#include <a_samp>
#include <streamer>

// ===========================================================================
//  KONFIGURACIJA - KOORDINATE
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
//  KONFIGURACIJA - PODESAVANJA
// ===========================================================================

#define PLAS_OPREMA_SKIN    176
#define PLAS_OPREMA_DIST    3.0
#define PLAS_GAJBA_DIST     2.5
#define PLAS_STOL_DIST      2.5
#define PLAS_MAS_DIST       2.5
#define PLAS_VIL_DIST       3.0
#define PLAS_SKL_DIST       5.0

#define PLAS_GAJBA_MODEL    19587
#define PLAS_BOX_MODEL      1271
#define PLAS_BIGBOX_MODEL   1685
#define PLAS_VILICAR_MODEL  530

#define PLAS_MACHINE_TIME   7
#define PLAS_MINI_SPEED1    3.5
#define PLAS_MINI_SPEED2    5.0
#define PLAS_MINI_SPEED3    7.0
#define PLAS_GREEN1         0.22
#define PLAS_GREEN2         0.17
#define PLAS_GREEN3         0.12

#define PLAS_PAY_MIN        88
#define PLAS_PAY_MAX        389
#define PLAS_PLAS_MIN       4
#define PLAS_PLAS_MAX       9

#define PLAS_ITEM_PLASTIKA  "Plastika"
#define PLAS_MODEL_PLASTIKA 2652

#define PLAS_SND_OPREMA     33100
#define PLAS_SND_USPJEH     1057
#define PLAS_SND_NAPLATA    1058
#define PLAS_SND_STROJ      1150
#define PLAS_SND_FAIL       1085

#define PLAS_CLR_BLUE       0x1D75C4AA
#define PLAS_CLR_GOLD       0xFFD700AA
#define PLAS_CLR_RED        0xFF6347AA
#define PLAS_CLR_GREEN      0x00FF00AA

// Minigame TD layout
#define PLAS_BAR_L          200.0
#define PLAS_BAR_T          258.0
#define PLAS_BAR_W          240.0
#define PLAS_PNL_L          170.0
#define PLAS_PNL_T          150.0

// ===========================================================================
//  ENUMS
// ===========================================================================

enum e_PlasState {
    PLAS_NONE = 0,
    PLAS_EQUIPPED,
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
    plasVehicleID,
    plasBoxObjID,
    plasTimerMini,
    plasTimerMachine,
    plasMiniPhase,
    plasMiniHits,
    Float:plasMarkerPos,
    plasMarkerDir,
    Float:plasGreenStart,
    Float:plasGreenWidth,
    plasMachineProgress,
    plasTargetSkladiste,
    plasToursDone,
    PlayerText:plasPBarBg,
    PlayerText:plasPBarFill,
    PlayerText:plasPBarLabel,
    PlayerText:plasStatusTD,
    bool:plasTDShown,
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

static PlasPickup_Oprema = -1;
static PlasPickup_Gajba1 = -1;
static PlasPickup_Gajba2 = -1;
static PlasPickup_Stol[4] = {-1, ...};
static PlasPickup_Mas[3] = {-1, ...};
static PlasPickup_Vil[2] = {-1, ...};

static Text3D:Plas3D_Oprema = Text3D:-1;
static Text3D:Plas3D_Gajba1 = Text3D:-1;
static Text3D:Plas3D_Gajba2 = Text3D:-1;
static Text3D:Plas3D_Stol[4] = {Text3D:-1, ...};
static Text3D:Plas3D_Mas[3] = {Text3D:-1, ...};
static Text3D:Plas3D_Vil[2] = {Text3D:-1, ...};
static Text3D:Plas3D_Skl[4] = {Text3D:-1, ...};

static PlasMapIcon = -1;

#pragma unused PlasPickup_Oprema, PlasPickup_Gajba1, PlasPickup_Gajba2
#pragma unused Plas3D_Oprema, Plas3D_Gajba1, Plas3D_Gajba2, PlasMapIcon

forward PlasTimer_MiniUpdate(playerid);
forward PlasTimer_Machine(playerid);
forward PlasMini_NextPhase(playerid);

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

    // Bar BG (dark gray)
    PlastikaPlayerData[playerid][plasMiniBarBg] = CreatePlayerTextDraw(playerid, PLAS_BAR_L, PLAS_BAR_T, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], 0.0, 0.9);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], 1);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], 0x333333FF);
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasMiniBarBg], PLAS_BAR_L+PLAS_BAR_W, 0.0);

    // Green zone (box, positioned over bar bg)
    PlastikaPlayerData[playerid][plasMiniBarGreen] = CreatePlayerTextDraw(playerid, PLAS_BAR_L, PLAS_BAR_T, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0.0, 0.9);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 1);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0x2ECC71AA);
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], PLAS_BAR_L+50.0, 0.0);

    // Marker (thin yellow box)
    PlastikaPlayerData[playerid][plasMiniMarker] = CreatePlayerTextDraw(playerid, PLAS_BAR_L, PLAS_BAR_T-3.0, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0.0, 1.3);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 1);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0xF39C12FF);
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasMiniMarker], PLAS_BAR_L+5.0, 0.0);

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
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen]);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniMarker]);
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
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen]);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniMarker]);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniStatus]);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniStars]);
    PlastikaPlayerData[playerid][plasMiniShown] = false;
}

stock PlasMini_UpdateGreen(playerid) {
    new Float:gx = PLAS_BAR_L + (PlastikaPlayerData[playerid][plasGreenStart] * PLAS_BAR_W);
    new Float:gw = gx + (PlastikaPlayerData[playerid][plasGreenWidth] * PLAS_BAR_W);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen]);
    PlayerTextDrawDestroy(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen]);
    PlastikaPlayerData[playerid][plasMiniBarGreen] = CreatePlayerTextDraw(playerid, gx, PLAS_BAR_T, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0.0, 0.9);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 1);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], 0x2ECC71AA);
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen], gw, 0.0);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniBarGreen]);
}

stock PlasMini_UpdateMarker(playerid) {
    new Float:mx = PLAS_BAR_L + (PlastikaPlayerData[playerid][plasMarkerPos] * PLAS_BAR_W);
    PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasMiniMarker]);
    PlayerTextDrawDestroy(playerid, PlastikaPlayerData[playerid][plasMiniMarker]);
    PlastikaPlayerData[playerid][plasMiniMarker] = CreatePlayerTextDraw(playerid, mx, PLAS_BAR_T-3.0, "_");
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0.0, 1.3);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 1);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasMiniMarker], 0xF39C12FF);
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasMiniMarker], mx+5.0, 0.0);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasMiniMarker]);
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
    new Float:gw;
    new name[24];

    switch(phase) {
        case 0: { gw=PLAS_GREEN1; name="ZAGRIJAVANJE"; }
        case 1: { gw=PLAS_GREEN2; name="PRESOVANJE"; }
        case 2: { gw=PLAS_GREEN3; name="HLADJENJE"; }
        default: { gw=PLAS_GREEN1; name="ZAGRIJAVANJE"; }
    }

    PlastikaPlayerData[playerid][plasGreenStart] = float(random(100))/100.0 * (1.0-gw-0.05) + 0.03;
    PlastikaPlayerData[playerid][plasGreenWidth] = gw;
    PlastikaPlayerData[playerid][plasMarkerPos] = 0.0;
    PlastikaPlayerData[playerid][plasMarkerDir] = 1;

    new ps[48]; format(ps, sizeof(ps), "FAZA %d/3: %s", phase+1, name);
    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniPhaseTD], ps);

    new ts[24];
    switch(phase) {
        case 0: ts = "TEMP: 45 C";
        case 1: ts = "TEMP: 180 C";
        case 2: ts = "TEMP: HLADJENJE";
    }
    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniTemp], ts);
    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniStatus], "Pritisni SPACE za zaustavljanje!");

    PlasMini_UpdateGreen(playerid);
    PlasMini_UpdateMarker(playerid);
    PlasMini_UpdateStars(playerid);

    PlastikaPlayerData[playerid][plasTimerMini] = SetTimerEx("PlasTimer_MiniUpdate", 150, true, "i", playerid);
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

    if(hits >= 2) {
        format(str, sizeof(str), "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Uspjesno presovanje! (%d/3) Dobili ste plasticni box.", hits);
        PlasMSG(playerid, str);
        SetPlayerAttachedObject(playerid, 4, PLAS_BOX_MODEL, 5, 0.1, 0.05, 0.0, 0.0, 90.0, 0.0);
        if(IsPlayerAttachedObjectSlotUsed(playerid, 3)) RemovePlayerAttachedObject(playerid, 3);

        PlastikaPlayerData[playerid][plasState] = PLAS_CARRYING_BOX;
        PlasINFO(playerid, "Nosite plasticni box. Idite do masine i stisnite ALT.");
        Plas_Status(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Nosi box do masine");
        SetPlayerCheckpoint(playerid, PLAS_MAS1_X, PLAS_MAS1_Y, PLAS_MAS1_Z, 3.0);
    } else {
        format(str, sizeof(str), "{FF6347}[FABRIKA PLASTIKE]{FFFFFF} Presovanje neuspjesno! (%d/3) Gajba unistena.", hits);
        PlasGRESKA(playerid, str);
        if(IsPlayerAttachedObjectSlotUsed(playerid, 3)) RemovePlayerAttachedObject(playerid, 3);

        PlastikaPlayerData[playerid][plasState] = PLAS_EQUIPPED;
        PlasINFO(playerid, "Uzmite novu gajbu i pokusajte ponovo.");
        Plas_Status(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Uzmi novu gajbu");
    }
    PlayerPlaySound(playerid, hits >= 2 ? PLAS_SND_USPJEH : PLAS_SND_FAIL, 0.0, 0.0, 0.0);
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
    new Float:pos = PlastikaPlayerData[playerid][plasMarkerPos];
    new dir = PlastikaPlayerData[playerid][plasMarkerDir];
    pos += Float:dir * speed * 0.15;
    if(pos >= 1.0) { pos=1.0; PlastikaPlayerData[playerid][plasMarkerDir]=-1; }
    if(pos <= 0.0) { pos=0.0; PlastikaPlayerData[playerid][plasMarkerDir]=1; }
    PlastikaPlayerData[playerid][plasMarkerPos] = pos;
    PlasMini_UpdateMarker(playerid);

    new ts[24];
    switch(PlastikaPlayerData[playerid][plasMiniPhase]) {
        case 0: format(ts, sizeof(ts), "TEMP: %d C", floatround(pos*300.0));
        case 1: format(ts, sizeof(ts), "TEMP: %d C", 180+floatround(pos*50.0));
        case 2: format(ts, sizeof(ts), "TEMP: %d C", 230-floatround(pos*150.0));
    }
    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasMiniTemp], ts);
    return 1;
}

// ===========================================================================
//  MACHINE TIMER
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

        Plas_Status(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Idi do vilicara");
    }
    return 1;
}

// ===========================================================================
//  CORE JOB LOGIC
// ===========================================================================

stock Plastika_StopJob(playerid) {
    if(PlastikaPlayerData[playerid][plasTimerMini] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerMini]); PlastikaPlayerData[playerid][plasTimerMini]=-1; }
    if(PlastikaPlayerData[playerid][plasTimerMachine] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerMachine]); PlastikaPlayerData[playerid][plasTimerMachine]=-1; }

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

    ClearAnimations(playerid);

    PlastikaPlayerData[playerid][plasState] = PLAS_NONE;
    PlastikaPlayerData[playerid][plasEquipped] = false;
    PlastikaPlayerData[playerid][plasOldSkin] = -1;
    PlastikaPlayerData[playerid][plasMachineProgress] = 0;
    PlastikaPlayerData[playerid][plasMiniPhase] = 0;
    PlastikaPlayerData[playerid][plasMiniHits] = 0;
    DeletePVar(playerid, "PlastikaWorking");
}

stock Plastika_UzmiOpremu(playerid) {
    if(PlastikaPlayerData[playerid][plasEquipped]) return PlasGRESKA(playerid, "Vec imate opremu! Kucajte /plastikainfo za pomoc.");
    if(PlastikaPlayerData[playerid][plasState] != PLAS_NONE) return PlasGRESKA(playerid, "Vec ste u procesu rada u fabrici plastike.");

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
    PlasINFO(playerid, "Uzmite gajbu sa palete i nosite je na stol. Kucajte /plastikainfo za komande.");
    Plas_Status(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Uzmi gajbu sa palete");
    return 1;
}

stock Plas_UzmiGajbu(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLAS_EQUIPPED) return PlasGRESKA(playerid, "Ne mozete sada uzeti gajbu.");
    if(!PlasNearGajba(playerid)) return PlasGRESKA(playerid, "Niste blizu palete sa gajbama.");
    if(IsPlayerInAnyVehicle(playerid)) return PlasGRESKA(playerid, "Ne mozete uzeti gajbu dok ste u vozilu.");

    SetPlayerAttachedObject(playerid, 3, PLAS_GAJBA_MODEL, 1, 0.15, 0.3, 0.0, 0.0, 90.0, 0.0);
    PlastikaPlayerData[playerid][plasState] = PLAS_CARRYING_GAJBA;
    DisablePlayerCheckpoint(playerid);

    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Uzeli ste gajbu! Idite do stola i stisnite ALT.");
    Plas_Status(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Nosi gajbu na stol");
    PlayerPlaySound(playerid, PLAS_SND_OPREMA, 0.0, 0.0, 0.0);
    return 1;
}

stock Plas_PocniStol(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLAS_CARRYING_GAJBA) return PlasGRESKA(playerid, "Ne mozete sada poceti rad na stolu.");
    if(!PlasNearStol(playerid)) return PlasGRESKA(playerid, "Niste blizu radnog stola.");
    if(IsPlayerInAnyVehicle(playerid)) return PlasGRESKA(playerid, "Ne mozete raditi na stolu dok ste u vozilu.");

    // NE koristimo TogglePlayerControllable(false) - blokira detekciju tipki!
    // Umjesto toga koristimo ApplyAnimation sa freeze=1 (dozvoljava tipke)
    ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Loop", 4.1, 1, 0, 0, 0, 0, 1);
    PlastikaPlayerData[playerid][plasState] = PLAS_MINIGAME;
    PlastikaPlayerData[playerid][plasMiniPhase] = 0;
    PlastikaPlayerData[playerid][plasMiniHits] = 0;
    PlastikaPlayerData[playerid][plasMarkerPos] = 0.0;
    PlastikaPlayerData[playerid][plasMarkerDir] = 1;
    DisablePlayerCheckpoint(playerid);

    PlasMini_Show(playerid);
    PlasMini_StartPhase(playerid);

    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} TERMO PRESOVANJE! Pritisni SPACE u zelenoj zoni!");
    return 1;
}

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
    Plas_Status(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Prerada u masini...");
    return 1;
}

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

    new vid = CreateVehicle(PLAS_VILICAR_MODEL, x,y,z,a, -1,-1,-1);
    if(vid == INVALID_VEHICLE_ID) return PlasGRESKA(playerid, "Greska pri kreiranju vilicara.");

    PlastikaPlayerData[playerid][plasVehicleID] = vid;
    PutPlayerInVehicle(playerid, vid, 0);

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
    Plas_Status(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Vozi do skladista");
    PlayerPlaySound(playerid, PLAS_SND_USPJEH, 0.0, 0.0, 0.0);
    return 1;
}

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

    new pay = PlasRand(PLAS_PAY_MIN, PLAS_PAY_MAX);
    new plas = PlasRand(PLAS_PLAS_MIN, PLAS_PLAS_MAX);
    GivePlayerMoney(playerid, pay);
    PlasInv_Add(playerid, plas);

    PlastikaPlayerData[playerid][plasToursDone]++;

    new str[128];
    format(str, sizeof(str), "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Box ostavljen! Zaradili ste {00FF00}$%d{FFFFFF}.", pay);
    PlasMSG(playerid, str);
    format(str, sizeof(str), "Dobili ste {1D75C4}%d Plastike{FFFFFF} u inventory!", plas);
    PlasSUCCESS(playerid, str);
    format(str, sizeof(str), "Tura #%d zavrsena! Uzmite novu gajbu za slijedecu turu.", PlastikaPlayerData[playerid][plasToursDone]);
    PlasINFO(playerid, str);

    PlayerPlaySound(playerid, PLAS_SND_NAPLATA, 0.0, 0.0, 0.0);
    PlastikaPlayerData[playerid][plasState] = PLAS_EQUIPPED;
    Plas_Status(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Uzmi novu gajbu");
    return 1;
}

stock Plastika_SkinOpremu(playerid) {
    if(!PlastikaPlayerData[playerid][plasEquipped]) return PlasGRESKA(playerid, "Nemate opremu za skidanje.");
    if(PlastikaPlayerData[playerid][plasState] == PLAS_MINIGAME) return PlasGRESKA(playerid, "Ne mozete skinuti opremu tokom minigame-a!");
    if(PlastikaPlayerData[playerid][plasState] == PLAS_MACHINE) return PlasGRESKA(playerid, "Ne mozete skinuti opremu dok masina radi!");
    if(PlastikaPlayerData[playerid][plasState] == PLAS_FORKLIFT) return PlasGRESKA(playerid, "Ne mozete skinuti opremu dok vozite vilicar!");
    Plastika_StopJob(playerid);
    PlasSUCCESS(playerid, "Skinuli ste opremu i prestali raditi u fabrici plastike.");
    return 1;
}

// ===========================================================================
//  INIT
// ===========================================================================

stock Plastika_Init() {
    // Oprema - mali info pickup (1239 = info ikona)
    PlasPickup_Oprema = CreateDynamicPickup(1239, 1, PLAS_OPREMA_X,PLAS_OPREMA_Y,PLAS_OPREMA_Z);
    Plas3D_Oprema = CreateDynamic3DTextLabel("{1D75C4}[FABRIKA PLASTIKE - OPREMA]{FFFFFF}\nKucaj /uzmiopremup", 0xFFFFFFFF, PLAS_OPREMA_X,PLAS_OPREMA_Y,PLAS_OPREMA_Z+0.5, 10.0);

    // Gajbe - mali info pickup
    PlasPickup_Gajba1 = CreateDynamicPickup(1239, 1, PLAS_GAJBA1_X,PLAS_GAJBA1_Y,PLAS_GAJBA1_Z);
    Plas3D_Gajba1 = CreateDynamic3DTextLabel("{1D75C4}[GAJBE - STRANA 1]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} da uzmes gajbu", 0xFFFFFFFF, PLAS_GAJBA1_X,PLAS_GAJBA1_Y,PLAS_GAJBA1_Z+0.5, 8.0);

    PlasPickup_Gajba2 = CreateDynamicPickup(1239, 1, PLAS_GAJBA2_X,PLAS_GAJBA2_Y,PLAS_GAJBA2_Z);
    Plas3D_Gajba2 = CreateDynamic3DTextLabel("{1D75C4}[GAJBE - STRANA 2]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} da uzmes gajbu", 0xFFFFFFFF, PLAS_GAJBA2_X,PLAS_GAJBA2_Y,PLAS_GAJBA2_Z+0.5, 8.0);

    // Radni stolovi - mali info pickup
    new Float:stolX[4] = {PLAS_STOL1_X,PLAS_STOL2_X,PLAS_STOL3_X,PLAS_STOL4_X};
    new Float:stolY[4] = {PLAS_STOL1_Y,PLAS_STOL2_Y,PLAS_STOL3_Y,PLAS_STOL4_Y};
    new Float:stolZ[4] = {PLAS_STOL1_Z,PLAS_STOL2_Z,PLAS_STOL3_Z,PLAS_STOL4_Z};
    for(new i = 0; i < 4; i++) {
        PlasPickup_Stol[i] = CreateDynamicPickup(1239, 1, stolX[i],stolY[i],stolZ[i]);
        new lbl[128]; format(lbl, sizeof(lbl), "{FFD700}[RADNI STOL %d]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} za preradu", i+1);
        Plas3D_Stol[i] = CreateDynamic3DTextLabel(lbl, 0xFFFFFFFF, stolX[i],stolY[i],stolZ[i]+0.5, 5.0);
    }

    // Masine - mali info pickup
    new Float:masX[3] = {PLAS_MAS1_X,PLAS_MAS2_X,PLAS_MAS3_X};
    new Float:masY[3] = {PLAS_MAS1_Y,PLAS_MAS2_Y,PLAS_MAS3_Y};
    new Float:masZ[3] = {PLAS_MAS1_Z,PLAS_MAS2_Z,PLAS_MAS3_Z};
    for(new i = 0; i < 3; i++) {
        PlasPickup_Mas[i] = CreateDynamicPickup(1239, 1, masX[i],masY[i],masZ[i]);
        new lbl[128]; format(lbl, sizeof(lbl), "{00FF00}[MASINA %d]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} za preradu", i+1);
        Plas3D_Mas[i] = CreateDynamic3DTextLabel(lbl, 0xFFFFFFFF, masX[i],masY[i],masZ[i]+0.5, 5.0);
    }

    // Vilicari - mali info pickup
    PlasPickup_Vil[0] = CreateDynamicPickup(1239, 1, PLAS_VIL1_X,PLAS_VIL1_Y,PLAS_VIL1_Z);
    Plas3D_Vil[0] = CreateDynamic3DTextLabel("{1D75C4}[VILICAR - STRANA 1]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} za vilicar", 0xFFFFFFFF, PLAS_VIL1_X,PLAS_VIL1_Y,PLAS_VIL1_Z+0.5, 5.0);

    PlasPickup_Vil[1] = CreateDynamicPickup(1239, 1, PLAS_VIL2_X,PLAS_VIL2_Y,PLAS_VIL2_Z);
    Plas3D_Vil[1] = CreateDynamic3DTextLabel("{1D75C4}[VILICAR - STRANA 2]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} za vilicar", 0xFFFFFFFF, PLAS_VIL2_X,PLAS_VIL2_Y,PLAS_VIL2_Z+0.5, 5.0);

    // Skladista
    new Float:sklX[4] = {PLAS_SKL1_X,PLAS_SKL2_X,PLAS_SKL3_X,PLAS_SKL4_X};
    new Float:sklY[4] = {PLAS_SKL1_Y,PLAS_SKL2_Y,PLAS_SKL3_Y,PLAS_SKL4_Y};
    new Float:sklZ[4] = {PLAS_SKL1_Z,PLAS_SKL2_Z,PLAS_SKL3_Z,PLAS_SKL4_Z};
    for(new i = 0; i < 4; i++) {
        new lbl[96]; format(lbl, sizeof(lbl), "{1D75C4}[SKLADISTE %d]{FFFFFF}\nOstavi box ovdje", i+1);
        Plas3D_Skl[i] = CreateDynamic3DTextLabel(lbl, 0xFFFFFFFF, sklX[i],sklY[i],sklZ[i]+0.5, 8.0);
    }

    PlasMapIcon = CreateDynamicMapIcon(PLAS_OPREMA_X,PLAS_OPREMA_Y,PLAS_OPREMA_Z, 55, 0);
    print("[PLASTIKA v2.0] Sistem skladistenja plastike uspjesno ucitan!");
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
    PlastikaPlayerData[playerid][plasMiniPhase] = 0;
    PlastikaPlayerData[playerid][plasMiniHits] = 0;
    PlastikaPlayerData[playerid][plasMarkerPos] = 0.0;
    PlastikaPlayerData[playerid][plasMarkerDir] = 1;
    PlastikaPlayerData[playerid][plasMachineProgress] = 0;
    PlastikaPlayerData[playerid][plasTargetSkladiste] = 0;
    PlastikaPlayerData[playerid][plasToursDone] = 0;
    PlastikaPlayerData[playerid][plasTDShown] = false;
    PlastikaPlayerData[playerid][plasMiniShown] = false;

    Plas_CreateProgressTDs(playerid);
    PlasMini_CreateTDs(playerid);
    return 1;
}

stock Plastika_OnDisconnect(playerid) {
    if(PlastikaPlayerData[playerid][plasTimerMini] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerMini]); PlastikaPlayerData[playerid][plasTimerMini]=-1; }
    if(PlastikaPlayerData[playerid][plasTimerMachine] != -1) { KillTimer(PlastikaPlayerData[playerid][plasTimerMachine]); PlastikaPlayerData[playerid][plasTimerMachine]=-1; }

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
    if(PlastikaPlayerData[playerid][plasState] == PLAS_FORKLIFT) {
        if(IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == PlastikaPlayerData[playerid][plasVehicleID]) {
            Plas_OstaviUSkladiste(playerid);
            return 1;
        }
    }
    return 0;
}

// ===========================================================================
//  KEY STATE CHANGE
// ===========================================================================

stock Plastika_OnKeyStateChange(playerid, newkeys, oldkeys) {
    if(PlastikaPlayerData[playerid][plasState] == PLAS_NONE) return 0;

    // SPACE (KEY_SPRINT) za minigame - na nogama SPACE je KEY_SPRINT!
    if((newkeys & KEY_SPRINT) && !(oldkeys & KEY_SPRINT)) {
        if(PlastikaPlayerData[playerid][plasState] == PLAS_MINIGAME) {
            new Float:pos = PlastikaPlayerData[playerid][plasMarkerPos];
            new Float:gs = PlastikaPlayerData[playerid][plasGreenStart];
            new Float:ge = gs + PlastikaPlayerData[playerid][plasGreenWidth];
            PlasMini_StopPhase(playerid, (pos >= gs && pos <= ge));
            return 1;
        }
    }

    // ALT = KEY_WALK (na nogama)
    if((newkeys & KEY_WALK) && !(oldkeys & KEY_WALK)) {
        if(IsPlayerInAnyVehicle(playerid)) return 0;

        switch(PlastikaPlayerData[playerid][plasState]) {
            case PLAS_EQUIPPED: {
                if(PlasNearGajba(playerid)) { Plas_UzmiGajbu(playerid); return 1; }
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

    // CTRL/KEY_HANDBRAKE u vilicaru
    if((newkeys & KEY_HANDBRAKE) && !(oldkeys & KEY_HANDBRAKE)) {
        if(PlastikaPlayerData[playerid][plasState] == PLAS_FORKLIFT) {
            if(IsPlayerInAnyVehicle(playerid) && GetPlayerVehicleID(playerid) == PlastikaPlayerData[playerid][plasVehicleID]) {
                Plas_OstaviUSkladiste(playerid); return 1;
            }
        }
    }

    if((newkeys & KEY_SUBMISSION) && !(oldkeys & KEY_SUBMISSION)) {
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
            Plas_HideProgress(i);
            PlasMini_Hide(i);
            DisablePlayerCheckpoint(i);
            if(PlastikaPlayerData[i][plasEquipped] && PlastikaPlayerData[i][plasOldSkin] != -1) SetPlayerSkin(i, PlastikaPlayerData[i][plasOldSkin]);
            PlastikaPlayerData[i][plasState] = PLAS_NONE;
            PlastikaPlayerData[i][plasEquipped] = false;
            DeletePVar(i, "PlastikaWorking");
            PlasGRESKA(i, "Vilicar je unisten! Posao prekinut.");
            break;
        }
    }
    return 1;
}

stock Plastika_OnPlayerExitVehicle(playerid, vehicleid) {
    if(PlastikaPlayerData[playerid][plasVehicleID] == vehicleid) {
        if(PlastikaPlayerData[playerid][plasState] == PLAS_FORKLIFT)
            PlasINFO(playerid, "Izasli ste iz vilicara! Vratite se da nastavite do skladista.");
    }
    return 1;
}

// ===========================================================================
//  INFO / ADMIN
// ===========================================================================

stock Plastika_Info(playerid) {
    new str[768];
    format(str, sizeof(str),
        "{1D75C4}=== FABRIKA PLASTIKE - SKLADISTENJE ===\n\
        {FFFFFF}Posao: Prerada i skladistenje plastike\n\
        \n\
        {FFD700}Koraci:{FFFFFF}\n\
        1. /uzmiopremup - Uzmi opremu\n\
        2. ALT kod gajbi - Uzmi plastičnu gajbu\n\
        3. ALT kod stola - Pokreni TERMO PRESOVANJE minigame\n\
        4. SPACE u zelenoj zoni - 3 faze presovanja\n\
        5. ALT kod masine - Ubaci box u masinu (7s)\n\
        6. ALT kod vilicara - Podigni big box\n\
        7. Vozi do skladista, CTRL da ostavis\n\
        \n\
        {FF6347}/prekidaplastiku{FFFFFF} - Prekini posao i skini opremu\n\
        \n\
        {00FF00}Nagrada: $%d-$%d + %d-%d Plastike po turi\n\
        3/3 = savrsen box | 2/3 = dobar box\n\
        0-1/3 = gajba unistena!",
        PLAS_PAY_MIN, PLAS_PAY_MAX, PLAS_PLAS_MIN, PLAS_PLAS_MAX);
    ShowPlayerDialog(playerid, 9117, DIALOG_STYLE_MSGBOX, "{1D75C4}Fabrika Plastike", str, "Zatvori", "");
    return 1;
}

stock Plastika_ResetCooldown(playerid) { if(!PlasValid(playerid)) return 0; return 1; }
stock Plastika_SetCooldown(playerid, seconds) { if(!PlasValid(playerid)) return 0; return 1; }
stock Plastika_GivePlastika(playerid, kolicina) { if(!PlasValid(playerid)) return 0; return PlasInv_Add(playerid, kolicina); }
