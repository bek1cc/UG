// ===========================================================================
//  SISTEM FABRIKE PLASTIKE - SA-MP
//  Verzija: 1.1
//  Autor: Beka (uz pomoc AI asistenta)
//  Opis: Kompletan, modularan sistem fabrike plastike za SA-MP server.
//        Ovo je 1. dio posla - "Odvoz plastike na lokaciju" (bonus posao).
//        Igrac uzima opremu, spawn-a kamion (ID 578), ide do utovara cijevi,
//        utovara cijev na kamion (progress bar), vozi do dostavne lokacije,
//        istovaruje, te vozi kamion nazad u bazu da bi dobio nagradu.
//        Cijev (3675) se nakon utovara attach-a na kamion.
//
//  KORISCENJE:
//    - Plastika_Init()         - poziva se u OnGameModeInit
//    - Plastika_OnConnect()    - poziva se u OnPlayerConnect
//    - Plastika_OnDisconnect() - poziva se u OnPlayerDisconnect
//    - Plastika_OnDeath()      - poziva se u OnPlayerDeath
//    - Plastika_OnCP()         - poziva se u OnPlayerEnterCheckpoint
//    - Plastika_OnKeyStateChange() - poziva se u OnPlayerKeyStateChange
//    - Plastika_OnUpdate()     - poziva se u OnPlayerUpdate
//    - Plastika_OnVehicleDeath()- poziva se u OnVehicleDeath
//    - Plastika_OnPlayerExitVehicle()- poziva se u OnPlayerExitVehicle
// ===========================================================================

#if defined _plastika_included
    #endinput
#endif
#define _plastika_included

// ===========================================================================
//  KONFIGURACIJA
// ===========================================================================

// Koordinate
#define PLASTIKA_OPREMA_X        -72.8136
#define PLASTIKA_OPREMA_Y        -1110.5228
#define PLASTIKA_OPREMA_Z        1.0781

#define PLASTIKA_KAMION_X        -62.0223
#define PLASTIKA_KAMION_Y        -1117.1177
#define PLASTIKA_KAMION_Z        1.0781
#define PLASTIKA_KAMION_A        350.3807

#define PLASTIKA_UTOVAR_X        -73.5247
#define PLASTIKA_UTOVAR_Y        -1137.6943
#define PLASTIKA_UTOVAR_Z        1.0781

#define PLASTIKA_DOSTAVA_X       -384.6674
#define PLASTIKA_DOSTAVA_Y       -1057.7296
#define PLASTIKA_DOSTAVA_Z       59.0066

// Podesavanja
#define PLASTIKA_OPREMA_SKIN     176     // Skin za posao
#define PLASTIKA_OPREMA_DIST     3.0     // Domet za uzimanje opreme
#define PLASTIKA_KAMION_DIST     3.5     // Domet za spawn kamiona (CP)
#define PLASTIKA_UTOVAR_DIST     10.0    // Domet za utovar cijevi
#define PLASTIKA_DOSTAVA_DIST    8.0     // Domet za dostavu
#define PLASTIKA_POVRAAT_DIST    8.0     // Domet za povratak kamiona u bazu
#define PLASTIKA_UTOVAR_VRIJEME  8       // Sekunde utovara
#define PLASTIKA_ISTOVAR_VRIJEME 5       // Sekunde istovara
#define PLASTIKA_COOLDOWN        1800    // Sekunde cooldowna (30 min - bonus posao)
#define PLASTIKA_NAPLATA         20000   // Fiksna naplata za dostavu (bonus posao)
#define PLASTIKA_NAGRADE_PLASTIKA 50     // Kolicina plastike u inventory
#define PLASTIKA_KEY_DEBOUNCE    800     // Milisekunde debounce za key detekciju u vozilu

// Vozilo
#define PLASTIKA_KAMION_MODEL    578     // DFT-30 kamion

// Cijev model i attach podaci (iz CijevNaKombiju.pwn)
#define PLASTIKA_CIJEV_MODEL     3675
#define PLASTIKA_CIJEV_MAT_TEX   14600
#define PLASTIKA_CIJEV_MAT_NAME  "paperchase_bits2"
#define PLASTIKA_CIJEV_MAT_BINT  "ab_plasticBin"

// Attach offseti za cijev na kamionu (iz CijevNaKombiju.pwn)
#define PLASTIKA_CIJEV_OFF_X     -0.496
#define PLASTIKA_CIJEV_OFF_Y     0.649
#define PLASTIKA_CIJEV_OFF_Z     1.315
#define PLASTIKA_CIJEV_OFF_RX    -1.600
#define PLASTIKA_CIJEV_OFF_RY    78.999
#define PLASTIKA_CIJEV_OFF_RZ    88.300

// Item imena za inventory
#define PLASTIKA_ITEM_PLASTIKA   "Plastika"
#define PLASTIKA_MODEL_PLASTIKA  2652    // Plavi plasticni barrel (ikona)

// Zvukovi
#define PLASTIKA_SOUND_OPREMA    33100
#define PLASTIKA_SOUND_UTOVAR    1150    // Conveyor/stroj zvuk
#define PLASTIKA_SOUND_GOTOVO    1057
#define PLASTIKA_SOUND_NAPLATA   1058    // Cash register zvuk

// Boje za poruke
#define PLASTIKA_CLR_MSG         0x1D75C4AA   // Plava (fabrika plastike)
#define PLASTIKA_CLR_INFO        0xFFD700AA   // Zlatna
#define PLASTIKA_CLR_ERR         0xFF6347AA   // Crvena
#define PLASTIKA_CLR_SUCCESS     0x00FF00AA   // Zelena

// ===========================================================================
//  ENUMERACIJE I PROMJENLJIVE
// ===========================================================================

// Stanja sistema za igraca
enum e_PlastikaState {
    PLASTIKA_STATE_NONE = 0,       // Ne radi nista
    PLASTIKA_STATE_EQUIPPED,      // Uzeo opremu, ide do kamiona
    PLASTIKA_STATE_DRIVING,       // U kamionu, ide do utovara
    PLASTIKA_STATE_LOADING,       // Utovar cijevi na kamion (progress)
    PLASTIKA_STATE_DELIVERING,    // VozI cijev do dostave
    PLASTIKA_STATE_UNLOADING,     // Istovar na destinaciji (progress)
    PLASTIKA_STATE_RETURNING,     // Vraca kamion u bazu nakon istovara
    PLASTIKA_STATE_COOLDOWN       // Cooldown nakon zavrsetka
}

// Podaci o igracu u sistemu
enum e_PlastikaPlayer {
    e_PlastikaState:plasState,
    plasOldSkin,                        // Stari skin prije opreme
    bool:plasHasEquipment,              // Da li ima opremu
    plasVehicleID,                      // ID kamiona (job vozilo)
    plasCijevObjID,                     // ID attachovane cijevi na kamionu
    plasCooldownTime,                   // Vrijeme cooldowna (GetTickCount)
    plasTimerUtovar,                    // Timer za utovar
    plasTimerIstovar,                   // Timer za istovar
    plasUtovarProgress,                 // Progress utovara (0-100)
    plasIstovarProgress,                // Progress istovara (0-100)
    PlayerText:plasProgressBar,         // Progress bar TD
    PlayerText:plasProgressBgr,         // Progress bar pozadina TD
    PlayerText:plasInfoTD,              // Info textdraw
    PlayerText:plasProgressLabel,       // Label iznad bara
    PlayerText:plasStatusTD,            // Status TD (gori lijevo)
    bool:plasTDShown,                   // Da li su TD prikazani
    plasLastAltPress                    // Zadnje vrijeme ALT pritiska (debounce za vozilo)
}

static PlastikaPlayerData[MAX_PLAYERS][e_PlastikaPlayer];

// Globalni pickupi i 3D labelovi
static Text3D:Plas3D_Oprema = Text3D:-1;
static Text3D:Plas3D_Kamion = Text3D:-1;
static Text3D:Plas3D_Utovar = Text3D:-1;
static Text3D:Plas3D_Dostava = Text3D:-1;

static PlasPickup_Oprema = -1;
static PlasPickup_Kamion = -1;
static PlasPickup_Utovar = -1;
static PlasPickup_Dostava = -1;

static PlasMapIcon = -1;

// Pragma unused za static varijable (koriste se interno)
#pragma unused Plas3D_Oprema
#pragma unused Plas3D_Kamion
#pragma unused Plas3D_Utovar
#pragma unused Plas3D_Dostava
#pragma unused PlasPickup_Oprema
#pragma unused PlasPickup_Kamion
#pragma unused PlasPickup_Utovar
#pragma unused PlasPickup_Dostava
#pragma unused PlasMapIcon

// Forwardanje timer callbacka
forward PlasTimer_Utovar(playerid);
forward PlasTimer_Istovar(playerid);

// ===========================================================================
//  HELPER FUNKCIJE
// ===========================================================================

stock PlasMSG(playerid, msg[]) {
    SendClientMessage(playerid, PLASTIKA_CLR_MSG, msg);
}

stock PlasGRESKA(playerid, msg[]) {
    SendClientMessage(playerid, PLASTIKA_CLR_ERR, msg);
    return 1;
}

stock PlasINFO(playerid, msg[]) {
    SendClientMessage(playerid, PLASTIKA_CLR_INFO, msg);
    return 1;
}

stock PlasSUCCESS(playerid, msg[]) {
    SendClientMessage(playerid, PLASTIKA_CLR_SUCCESS, msg);
    return 1;
}

// Float udaljenost
stock Float:PlasDist(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2) {
    new Float:dx = x1 - x2;
    new Float:dy = y1 - y2;
    new Float:dz = z1 - z2;
    return floatsqroot(dx*dx + dy*dy + dz*dz);
}

// Random broj u opsegu
stock PlasRandomRange(min, max) {
    return min + random(max - min + 1);
}

// Provjera validnosti igraca
stock bool:PlasValidPlayer(playerid) {
    if(playerid < 0 || playerid >= MAX_PLAYERS) return false;
    if(!IsPlayerConnected(playerid)) return false;
    return true;
}

// Da li je igrac u job kamionu
stock bool:PlasIsInJobVehicle(playerid) {
    if(PlastikaPlayerData[playerid][plasVehicleID] == -1) return false;
    if(!IsPlayerInAnyVehicle(playerid)) return false;
    if(GetPlayerVehicleID(playerid) != PlastikaPlayerData[playerid][plasVehicleID]) return false;
    if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return false;
    return true;
}

// ===========================================================================
//  INVENTORY INTEGRACIJA
// ===========================================================================

stock PlasInv_AddPlastika(playerid, kolicina) {
    #if defined Inventory_Add
        return Inventory_Add(playerid, PLASTIKA_ITEM_PLASTIKA, PLASTIKA_MODEL_PLASTIKA, kolicina);
    #else
        return GivePlayerWeapon(playerid, 0, 0); // fallback
    #endif
}

// ===========================================================================
//  TEXTDRAWS - Progress bar i Info TD
// ===========================================================================

stock Plas_CreatePlayerTDs(playerid) {
    // Progress bar background
    PlastikaPlayerData[playerid][plasProgressBgr] = CreatePlayerTextDraw(playerid, 250.000000, 400.000000, "_");
    PlayerTextDrawBackgroundColor(playerid, PlastikaPlayerData[playerid][plasProgressBgr], 255);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasProgressBgr], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasProgressBgr], 0.500000, 1.500000);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasProgressBgr], -1);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasProgressBgr], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasProgressBgr], 1);
    PlayerTextDrawSetShadow(playerid, PlastikaPlayerData[playerid][plasProgressBgr], 1);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasProgressBgr], 1);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasProgressBgr], 80);
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasProgressBgr], 420.000000, 0.000000);

    // Progress bar (popunjavanje) - plava boja za plastiku
    PlastikaPlayerData[playerid][plasProgressBar] = CreatePlayerTextDraw(playerid, 251.000000, 401.000000, "_");
    PlayerTextDrawBackgroundColor(playerid, PlastikaPlayerData[playerid][plasProgressBar], 255);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasProgressBar], 1);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasProgressBar], 0.479166, 1.200000);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasProgressBar], -1);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasProgressBar], 0);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasProgressBar], 1);
    PlayerTextDrawSetShadow(playerid, PlastikaPlayerData[playerid][plasProgressBar], 0);
    PlayerTextDrawUseBox(playerid, PlastikaPlayerData[playerid][plasProgressBar], 1);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasProgressBar], 0x1D75C466);
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasProgressBar], 251.000000, 0.000000);

    // Info textdraw (gore desno)
    PlastikaPlayerData[playerid][plasInfoTD] = CreatePlayerTextDraw(playerid, 620.000000, 130.000000, "_");
    PlayerTextDrawAlignment(playerid, PlastikaPlayerData[playerid][plasInfoTD], 3);
    PlayerTextDrawBackgroundColor(playerid, PlastikaPlayerData[playerid][plasInfoTD], 255);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasInfoTD], 2);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasInfoTD], 0.220000, 1.000000);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasInfoTD], 0x1D75C4CC);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasInfoTD], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasInfoTD], 1);
    PlayerTextDrawSetShadow(playerid, PlastikaPlayerData[playerid][plasInfoTD], 0);

    // Progress bar label (UTOVAR... / ISTOVAR... iznad bara)
    PlastikaPlayerData[playerid][plasProgressLabel] = CreatePlayerTextDraw(playerid, 335.000000, 385.000000, "_");
    PlayerTextDrawAlignment(playerid, PlastikaPlayerData[playerid][plasProgressLabel], 2);
    PlayerTextDrawBackgroundColor(playerid, PlastikaPlayerData[playerid][plasProgressLabel], 150);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasProgressLabel], 2);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasProgressLabel], 0.300000, 1.100000);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasProgressLabel], 0x1D75C4EE);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasProgressLabel], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasProgressLabel], 1);
    PlayerTextDrawSetShadow(playerid, PlastikaPlayerData[playerid][plasProgressLabel], 0);

    // Status TD (gore lijevo - mini HUD)
    PlastikaPlayerData[playerid][plasStatusTD] = CreatePlayerTextDraw(playerid, 20.000000, 130.000000, "_");
    PlayerTextDrawBackgroundColor(playerid, PlastikaPlayerData[playerid][plasStatusTD], 150);
    PlayerTextDrawFont(playerid, PlastikaPlayerData[playerid][plasStatusTD], 2);
    PlayerTextDrawLetterSize(playerid, PlastikaPlayerData[playerid][plasStatusTD], 0.180000, 0.900000);
    PlayerTextDrawColor(playerid, PlastikaPlayerData[playerid][plasStatusTD], 0x1D75C4DD);
    PlayerTextDrawSetOutline(playerid, PlastikaPlayerData[playerid][plasStatusTD], 1);
    PlayerTextDrawSetProportional(playerid, PlastikaPlayerData[playerid][plasStatusTD], 1);
    PlayerTextDrawSetShadow(playerid, PlastikaPlayerData[playerid][plasStatusTD], 0);
}

stock Plas_ShowProgress(playerid, Float:percent, info[]) {
    new Float:barWidth = 251.0 + (168.0 * (percent / 100.0));
    PlayerTextDrawTextSize(playerid, PlastikaPlayerData[playerid][plasProgressBar], barWidth, 0.000000);
    PlayerTextDrawBoxColor(playerid, PlastikaPlayerData[playerid][plasProgressBar], 0x1D75C466);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasProgressBgr]);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasProgressBar]);

    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasProgressLabel], info);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasProgressLabel]);

    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasInfoTD], info);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasInfoTD]);
    PlastikaPlayerData[playerid][plasTDShown] = true;
}

stock Plas_HideProgress(playerid) {
    if(PlastikaPlayerData[playerid][plasTDShown]) {
        PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasProgressBgr]);
        PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasProgressBar]);
        PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasProgressLabel]);
        PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasInfoTD]);
        PlayerTextDrawHide(playerid, PlastikaPlayerData[playerid][plasStatusTD]);
        PlastikaPlayerData[playerid][plasTDShown] = false;
    }
}

stock Plas_UpdateStatusTD(playerid, status[]) {
    PlayerTextDrawSetString(playerid, PlastikaPlayerData[playerid][plasStatusTD], status);
    PlayerTextDrawShow(playerid, PlastikaPlayerData[playerid][plasStatusTD]);
}

// ===========================================================================
//  INICIJALIZACIJA SISTEMA
// ===========================================================================

stock Plastika_Init() {
    // Kreiraj pickup za opremu (plavi barrel 2652)
    PlasPickup_Oprema = CreateDynamicPickup(2652, 23, PLASTIKA_OPREMA_X, PLASTIKA_OPREMA_Y, PLASTIKA_OPREMA_Z);

    // Kreiraj pickup za kamion (crveni CP marker 1318)
    PlasPickup_Kamion = CreateDynamicPickup(1318, 23, PLASTIKA_KAMION_X, PLASTIKA_KAMION_Y, PLASTIKA_KAMION_Z);

    // NE kreiraj pickup za utovar - koristi se game checkpoint (SetPlayerCheckpoint)
    // Igrac vozacki udje u checkpoint i utovar se automatski pokrece
    // PlasPickup_Utovar = -1; // nije potreban

    // Kreiraj pickup za dostavu
    PlasPickup_Dostava = CreateDynamicPickup(1275, 23, PLASTIKA_DOSTAVA_X, PLASTIKA_DOSTAVA_Y, PLASTIKA_DOSTAVA_Z);

    // 3D Text labelovi - utovar label malo iznad poda (ne u zraku)
    Plas3D_Oprema = CreateDynamic3DTextLabel("{1D75C4}[FABRIKA PLASTIKE - OPREMA]{FFFFFF}\nKucaj /uzmiopremup da uzmes opremu",
        0xFFFFFFFF, PLASTIKA_OPREMA_X, PLASTIKA_OPREMA_Y, PLASTIKA_OPREMA_Z + 0.5, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1);

    Plas3D_Kamion = CreateDynamic3DTextLabel("{1D75C4}[FABRIKA PLASTIKE - KAMION]{FFFFFF}\nStisni {FFD700}ALT{FFFFFF} da spawnas kamion",
        0xFFFFFFFF, PLASTIKA_KAMION_X, PLASTIKA_KAMION_Y, PLASTIKA_KAMION_Z + 0.5, 8.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1);

    Plas3D_Utovar = CreateDynamic3DTextLabel("{FFD700}[FABRIKA PLASTIKE - UTOVAR]{FFFFFF}\nUdji sa kamionom da utovaris cijev",
        0xFFFFFFFF, PLASTIKA_UTOVAR_X, PLASTIKA_UTOVAR_Y, PLASTIKA_UTOVAR_Z + 0.3, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1);

    Plas3D_Dostava = CreateDynamic3DTextLabel("{00FF00}[FABRIKA PLASTIKE - DOSTAVA]{FFFFFF}\nUdji sa kamionom da istovaris cijev",
        0xFFFFFFFF, PLASTIKA_DOSTAVA_X, PLASTIKA_DOSTAVA_Y, PLASTIKA_DOSTAVA_Z + 0.3, 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1);

    // Map icon
    PlasMapIcon = CreateDynamicMapIcon(PLASTIKA_OPREMA_X, PLASTIKA_OPREMA_Y, PLASTIKA_OPREMA_Z, 55, 0);

    print("[PLASTIKA] Sistem fabrike plastike uspjesno ucitan!");
    print("[PLASTIKA] Lokacija opreme, kamiona, utovara, dostave i povratka postavljene.");
    return 1;
}

// ===========================================================================
//  ONPLAYERCONNECT / DISCONNECT
// ===========================================================================

stock Plastika_OnConnect(playerid) {
    PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_NONE;
    PlastikaPlayerData[playerid][plasOldSkin] = -1;
    PlastikaPlayerData[playerid][plasHasEquipment] = false;
    PlastikaPlayerData[playerid][plasVehicleID] = -1;
    PlastikaPlayerData[playerid][plasCijevObjID] = -1;
    PlastikaPlayerData[playerid][plasCooldownTime] = 0;
    PlastikaPlayerData[playerid][plasTimerUtovar] = -1;
    PlastikaPlayerData[playerid][plasTimerIstovar] = -1;
    PlastikaPlayerData[playerid][plasUtovarProgress] = 0;
    PlastikaPlayerData[playerid][plasIstovarProgress] = 0;
    PlastikaPlayerData[playerid][plasTDShown] = false;
    PlastikaPlayerData[playerid][plasLastAltPress] = 0;

    Plas_CreatePlayerTDs(playerid);
    return 1;
}

stock Plastika_OnDisconnect(playerid) {
    // Ubij timere
    if(PlastikaPlayerData[playerid][plasTimerUtovar] != -1) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerUtovar]);
        PlastikaPlayerData[playerid][plasTimerUtovar] = -1;
    }
    if(PlastikaPlayerData[playerid][plasTimerIstovar] != -1) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerIstovar]);
        PlastikaPlayerData[playerid][plasTimerIstovar] = -1;
    }

    // UniStti cijev objekat ako postoji
    if(PlastikaPlayerData[playerid][plasCijevObjID] != -1) {
        DestroyDynamicObject(PlastikaPlayerData[playerid][plasCijevObjID]);
        PlastikaPlayerData[playerid][plasCijevObjID] = -1;
    }

    // UniStti kamion ako postoji
    if(PlastikaPlayerData[playerid][plasVehicleID] != -1) {
        new vid = PlastikaPlayerData[playerid][plasVehicleID];
        if(GetVehicleModel(vid) != 0) {
            // Makni igraca iz vozila prije unistenja
            for(new i = 0; i < MAX_PLAYERS; i++) {
                if(IsPlayerInVehicle(i, vid)) {
                    RemovePlayerFromVehicle(i);
                }
            }
            DestroyVehicle(vid);
        }
        PlastikaPlayerData[playerid][plasVehicleID] = -1;
    }

    // Sakrij TDs
    Plas_HideProgress(playerid);

    // Disable checkpoint
    DisablePlayerCheckpoint(playerid);

    // Vrati skin ako ima opremu
    if(PlastikaPlayerData[playerid][plasHasEquipment] && PlastikaPlayerData[playerid][plasOldSkin] != -1) {
        SetPlayerSkin(playerid, PlastikaPlayerData[playerid][plasOldSkin]);
    }

    // Reset
    PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_NONE;
    PlastikaPlayerData[playerid][plasHasEquipment] = false;

    // Ukloni PVar
    DeletePVar(playerid, "PlastikaWorking");
    return 1;
}

stock Plastika_OnDeath(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLASTIKA_STATE_NONE) {
        Plastika_StopJob(playerid);
        PlasGRESKA(playerid, "Prekinuli ste rad u fabrici plastike jer ste umrli.");
    }
    return 1;
}

// ===========================================================================
//  GLAVNE FUNKCIJE SISTEMA
// ===========================================================================

// Kompletni reset i stop posla
stock Plastika_StopJob(playerid) {
    // Ubij timere
    if(PlastikaPlayerData[playerid][plasTimerUtovar] != -1) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerUtovar]);
        PlastikaPlayerData[playerid][plasTimerUtovar] = -1;
    }
    if(PlastikaPlayerData[playerid][plasTimerIstovar] != -1) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerIstovar]);
        PlastikaPlayerData[playerid][plasTimerIstovar] = -1;
    }

    // UniStti cijev objekat ako postoji
    if(PlastikaPlayerData[playerid][plasCijevObjID] != -1) {
        DestroyDynamicObject(PlastikaPlayerData[playerid][plasCijevObjID]);
        PlastikaPlayerData[playerid][plasCijevObjID] = -1;
    }

    // UniStti kamion ako postoji
    if(PlastikaPlayerData[playerid][plasVehicleID] != -1) {
        new vid = PlastikaPlayerData[playerid][plasVehicleID];
        if(GetVehicleModel(vid) != 0) {
            for(new i = 0; i < MAX_PLAYERS; i++) {
                if(IsPlayerInVehicle(i, vid)) {
                    RemovePlayerFromVehicle(i);
                }
            }
            DestroyVehicle(vid);
        }
        PlastikaPlayerData[playerid][plasVehicleID] = -1;
    }

    // Sakrij TDs
    Plas_HideProgress(playerid);

    // Disable checkpoint
    DisablePlayerCheckpoint(playerid);

    // Vrati skin
    if(PlastikaPlayerData[playerid][plasHasEquipment] && PlastikaPlayerData[playerid][plasOldSkin] != -1) {
        SetPlayerSkin(playerid, PlastikaPlayerData[playerid][plasOldSkin]);
    }

    // Unfreeze ako je zamrznut
    TogglePlayerControllable(playerid, true);

    // Clear animacije
    ClearAnimations(playerid);

    // Reset stanja
    PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_NONE;
    PlastikaPlayerData[playerid][plasHasEquipment] = false;
    PlastikaPlayerData[playerid][plasOldSkin] = -1;
    PlastikaPlayerData[playerid][plasUtovarProgress] = 0;
    PlastikaPlayerData[playerid][plasIstovarProgress] = 0;

    // Ukloni PVar
    DeletePVar(playerid, "PlastikaWorking");
}

// Uzimanje opreme
stock Plastika_UzmiOpremu(playerid) {
    // Provjere
    if(PlastikaPlayerData[playerid][plasHasEquipment]) {
        return PlasGRESKA(playerid, "Vec imate opremu! Idite do kamiona i stisnite ALT.");
    }
    if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_COOLDOWN) {
        new preostalo = (PlastikaPlayerData[playerid][plasCooldownTime] - GetTickCount()) / 1000;
        if(preostalo > 0) {
            new str[128];
            format(str, sizeof(str), "Morate sacekati jos %d sekundi prije nego sto ponovo mozete raditi.", preostalo);
            return PlasGRESKA(playerid, str);
        }
        PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_NONE;
    }
    if(PlastikaPlayerData[playerid][plasState] != PLASTIKA_STATE_NONE) {
        return PlasGRESKA(playerid, "Vec ste u procesu rada u fabrici plastike.");
    }

    // Provjera udaljenosti
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    if(PlasDist(x, y, z, PLASTIKA_OPREMA_X, PLASTIKA_OPREMA_Y, PLASTIKA_OPREMA_Z) > PLASTIKA_OPREMA_DIST) {
        return PlasGRESKA(playerid, "Niste na lokaciji za uzimanje opreme.");
    }

    // Provjera da li je u vozilu
    if(IsPlayerInAnyVehicle(playerid)) {
        return PlasGRESKA(playerid, "Ne mozete uzeti opremu dok ste u vozilu.");
    }

    // Postavi opremu
    PlastikaPlayerData[playerid][plasOldSkin] = GetPlayerSkin(playerid);
    SetPlayerSkin(playerid, PLASTIKA_OPREMA_SKIN);
    PlastikaPlayerData[playerid][plasHasEquipment] = true;
    PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_EQUIPPED;

    // Postavi PVar za anti-cheat
    SetPVarInt(playerid, "PlastikaWorking", 1);

    // Postavi checkpoint ka kamionu
    SetPlayerCheckpoint(playerid, PLASTIKA_KAMION_X, PLASTIKA_KAMION_Y, PLASTIKA_KAMION_Z, 3.0);

    // Zvuk i poruka
    PlayerPlaySound(playerid, PLASTIKA_SOUND_OPREMA, 0.0, 0.0, 0.0);
    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Uzeli ste opremu za rad u fabrici plastike!");
    PlasINFO(playerid, "Idite do kamiona i stisnite ALT da spawnate vozilo.");
    PlasINFO(playerid, "Kucaj /prekidaplastiku da prestanete raditi i vratite opremu.");

    // Status TD
    Plas_UpdateStatusTD(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Idi do kamiona");
    return 1;
}

// Spawn kamiona - poziva se kad igrac stisne ALT kod kamion pickupa
stock Plastika_SpawnKamion(playerid) {
    if(!PlastikaPlayerData[playerid][plasHasEquipment]) {
        return PlasGRESKA(playerid, "Nemate opremu! Idite na lokaciju i kucajte /uzmiopremup.");
    }
    if(PlastikaPlayerData[playerid][plasState] != PLASTIKA_STATE_EQUIPPED) {
        return PlasGRESKA(playerid, "Ne mozete sada spawnati kamion.");
    }
    if(PlastikaPlayerData[playerid][plasVehicleID] != -1) {
        return PlasGRESKA(playerid, "Vec imate kamion! Otidjite do utovara.");
    }

    // Provjera udaljenosti
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    if(PlasDist(x, y, z, PLASTIKA_KAMION_X, PLASTIKA_KAMION_Y, PLASTIKA_KAMION_Z) > PLASTIKA_KAMION_DIST) {
        return PlasGRESKA(playerid, "Niste na lokaciji za spawn kamiona.");
    }

    // Kreiraj kamion - bez cijevi na njemu
    new vid = CreateVehicle(PLASTIKA_KAMION_MODEL, PLASTIKA_KAMION_X, PLASTIKA_KAMION_Y, PLASTIKA_KAMION_Z, PLASTIKA_KAMION_A, -1, -1, -1);
    if(vid == INVALID_VEHICLE_ID) {
        return PlasGRESKA(playerid, "Greska pri kreiranju kamiona. Pokusajte ponovo.");
    }

    PlastikaPlayerData[playerid][plasVehicleID] = vid;

    // Stavi igraca u kamion
    PutPlayerInVehicle(playerid, vid, 0);

    // Postavi stanje - vozi ka utovaru
    PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_DRIVING;

    // Postavi checkpoint ka utovaru
    SetPlayerCheckpoint(playerid, PLASTIKA_UTOVAR_X, PLASTIKA_UTOVAR_Y, PLASTIKA_UTOVAR_Z, 5.0);

    // Poruka
    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Kamion spawnan! Vozite do utovara cijevi.");
    PlasINFO(playerid, "Slijedite checkpoint - utovar ce se automatski pokrenuti kad stignete.");

    // Status TD
    Plas_UpdateStatusTD(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Vozi do utovara");

    PlayerPlaySound(playerid, PLASTIKA_SOUND_GOTOVO, 0.0, 0.0, 0.0);
    return 1;
}

// Utovar cijevi - poziva se kad igrac stisne ALT kod utovar pickupa
stock Plastika_PocniUtovar(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLASTIKA_STATE_DRIVING) {
        return PlasGRESKA(playerid, "Ne mozete sada utovariti cijev.");
    }

    // Mora biti u job kamionu kao vozac
    if(!PlasIsInJobVehicle(playerid)) {
        return PlasGRESKA(playerid, "Morate biti u kamionu na mjestu vozaca!");
    }

    // Provjera udaljenosti kamiona od utovara
    new Float:vx, Float:vy, Float:vz;
    GetVehiclePos(PlastikaPlayerData[playerid][plasVehicleID], vx, vy, vz);
    if(PlasDist(vx, vy, vz, PLASTIKA_UTOVAR_X, PLASTIKA_UTOVAR_Y, PLASTIKA_UTOVAR_Z) > PLASTIKA_UTOVAR_DIST) {
        return PlasGRESKA(playerid, "Kamion nije na lokaciji za utovar! Odvezite se do utovara.");
    }

    // Postavi stanje
    PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_LOADING;
    PlastikaPlayerData[playerid][plasUtovarProgress] = 0;

    // Freeze kamion (ne moze se micati tokom utovara)
    new vid = PlastikaPlayerData[playerid][plasVehicleID];
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vid, 0, lights, alarm, doors, bonnet, boot, objective);

    // Sakrij checkpoint
    DisablePlayerCheckpoint(playerid);

    // Pokreni progress
    Plas_ShowProgress(playerid, 0.0, "UTOVAR CIJEVI...");

    // Zvuk utovara
    PlayerPlaySound(playerid, PLASTIKA_SOUND_UTOVAR, 0.0, 0.0, 0.0);

    // Pokreni timer za utovar (svaku sekundu)
    PlastikaPlayerData[playerid][plasTimerUtovar] = SetTimerEx("PlasTimer_Utovar", 1000, true, "i", playerid);

    // Status TD
    Plas_UpdateStatusTD(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Utovar cijevi...");

    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Utovar cijevi u toku... Sacekajte.");
    return 1;
}

// Timer za utovar cijevi
public PlasTimer_Utovar(playerid) {
    if(!PlasValidPlayer(playerid)) return 0;
    if(PlastikaPlayerData[playerid][plasState] != PLASTIKA_STATE_LOADING) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerUtovar]);
        PlastikaPlayerData[playerid][plasTimerUtovar] = -1;
        return 0;
    }

    PlastikaPlayerData[playerid][plasUtovarProgress]++;
    new Float:percent = (Float:PlastikaPlayerData[playerid][plasUtovarProgress] / Float:PLASTIKA_UTOVAR_VRIJEME) * 100.0;
    Plas_ShowProgress(playerid, percent, "UTOVAR CIJEVI...");

    // Provjera da li je igrac jos uvijek u kamionu
    if(!PlasIsInJobVehicle(playerid)) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerUtovar]);
        PlastikaPlayerData[playerid][plasTimerUtovar] = -1;
        PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_DRIVING;
        PlastikaPlayerData[playerid][plasUtovarProgress] = 0;
        Plas_HideProgress(playerid);
        PlasGRESKA(playerid, "Izasli ste iz kamiona! Utovar prekinut. Vratite se u kamion i pokusajte ponovo.");
        SetPlayerCheckpoint(playerid, PLASTIKA_UTOVAR_X, PLASTIKA_UTOVAR_Y, PLASTIKA_UTOVAR_Z, 5.0);

        // Ponovo upali motor
        new vid = PlastikaPlayerData[playerid][plasVehicleID];
        new engine, lights, alarm, doors, bonnet, boot, objective;
        GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
        SetVehicleParamsEx(vid, VEHICLE_PARAMS_ON, lights, alarm, doors, bonnet, boot, objective);
        return 0;
    }

    // Utovar zavrsen
    if(PlastikaPlayerData[playerid][plasUtovarProgress] >= PLASTIKA_UTOVAR_VRIJEME) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerUtovar]);
        PlastikaPlayerData[playerid][plasTimerUtovar] = -1;

        // Attachuj cijev na kamion
        new vid = PlastikaPlayerData[playerid][plasVehicleID];
        new cijevObj = CreateDynamicObject(PLASTIKA_CIJEV_MODEL, 0.0, 0.0, -1000.0, 0.0, 0.0, 0.0);
        SetDynamicObjectMaterial(cijevObj, 0, PLASTIKA_CIJEV_MAT_TEX, PLASTIKA_CIJEV_MAT_NAME, PLASTIKA_CIJEV_MAT_BINT);
        AttachDynamicObjectToVehicle(cijevObj, vid, PLASTIKA_CIJEV_OFF_X, PLASTIKA_CIJEV_OFF_Y, PLASTIKA_CIJEV_OFF_Z,
            PLASTIKA_CIJEV_OFF_RX, PLASTIKA_CIJEV_OFF_RY, PLASTIKA_CIJEV_OFF_RZ);

        PlastikaPlayerData[playerid][plasCijevObjID] = cijevObj;

        // Postavi stanje - dostava
        PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_DELIVERING;

        // Pokreni motor kamiona
        new engine, lights, alarm, doors, bonnet, boot, objective;
        GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
        SetVehicleParamsEx(vid, VEHICLE_PARAMS_ON, lights, alarm, doors, bonnet, boot, objective);

        // Postavi checkpoint ka dostavi
        SetPlayerCheckpoint(playerid, PLASTIKA_DOSTAVA_X, PLASTIKA_DOSTAVA_Y, PLASTIKA_DOSTAVA_Z, 8.0);

        // Poruka
        Plas_HideProgress(playerid);
        PlayerPlaySound(playerid, PLASTIKA_SOUND_GOTOVO, 0.0, 0.0, 0.0);
        PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Cijev utovarena na kamion!");
        PlasINFO(playerid, "Vozite do destinacije - istovar ce se automatski pokrenuti kad stignete.");

        // Status TD
        Plas_UpdateStatusTD(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Vozi do dostave");
    }
    return 1;
}

// Istovar cijevi - poziva se kad igrac stisne ALT kod dostava pickupa
stock Plastika_PocniIstovar(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLASTIKA_STATE_DELIVERING) {
        return PlasGRESKA(playerid, "Ne mozete sada istovariti cijev.");
    }

    // Mora biti u job kamionu
    if(!PlasIsInJobVehicle(playerid)) {
        return PlasGRESKA(playerid, "Morate biti u kamionu na mjestu vozaca!");
    }

    // Provjera udaljenosti kamiona od dostave
    new Float:vx, Float:vy, Float:vz;
    GetVehiclePos(PlastikaPlayerData[playerid][plasVehicleID], vx, vy, vz);
    if(PlasDist(vx, vy, vz, PLASTIKA_DOSTAVA_X, PLASTIKA_DOSTAVA_Y, PLASTIKA_DOSTAVA_Z) > PLASTIKA_DOSTAVA_DIST) {
        return PlasGRESKA(playerid, "Kamion nije na lokaciji za dostavu!");
    }

    // Postavi stanje
    PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_UNLOADING;
    PlastikaPlayerData[playerid][plasIstovarProgress] = 0;

    // Freeze kamion
    new vid = PlastikaPlayerData[playerid][plasVehicleID];
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vid, 0, lights, alarm, doors, bonnet, boot, objective);

    // Sakrij checkpoint
    DisablePlayerCheckpoint(playerid);

    // Pokreni progress
    Plas_ShowProgress(playerid, 0.0, "ISTOVAR CIJEVI...");

    // Zvuk
    PlayerPlaySound(playerid, PLASTIKA_SOUND_UTOVAR, 0.0, 0.0, 0.0);

    // Pokreni timer
    PlastikaPlayerData[playerid][plasTimerIstovar] = SetTimerEx("PlasTimer_Istovar", 1000, true, "i", playerid);

    // Status TD
    Plas_UpdateStatusTD(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Istovar cijevi...");

    PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Istovar cijevi u toku... Sacekajte.");
    return 1;
}

// Timer za istovar
public PlasTimer_Istovar(playerid) {
    if(!PlasValidPlayer(playerid)) return 0;
    if(PlastikaPlayerData[playerid][plasState] != PLASTIKA_STATE_UNLOADING) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerIstovar]);
        PlastikaPlayerData[playerid][plasTimerIstovar] = -1;
        return 0;
    }

    PlastikaPlayerData[playerid][plasIstovarProgress]++;
    new Float:percent = (Float:PlastikaPlayerData[playerid][plasIstovarProgress] / Float:PLASTIKA_ISTOVAR_VRIJEME) * 100.0;
    Plas_ShowProgress(playerid, percent, "ISTOVAR CIJEVI...");

    // Provjera da li je igrac jos uvijek u kamionu
    if(!PlasIsInJobVehicle(playerid)) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerIstovar]);
        PlastikaPlayerData[playerid][plasTimerIstovar] = -1;
        PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_DELIVERING;
        PlastikaPlayerData[playerid][plasIstovarProgress] = 0;
        Plas_HideProgress(playerid);
        PlasGRESKA(playerid, "Izasli ste iz kamiona! Istovar prekinut. Vratite se u kamion.");
        SetPlayerCheckpoint(playerid, PLASTIKA_DOSTAVA_X, PLASTIKA_DOSTAVA_Y, PLASTIKA_DOSTAVA_Z, 8.0);

        new vid = PlastikaPlayerData[playerid][plasVehicleID];
        new engine, lights, alarm, doors, bonnet, boot, objective;
        GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
        SetVehicleParamsEx(vid, VEHICLE_PARAMS_ON, lights, alarm, doors, bonnet, boot, objective);
        return 0;
    }

    // Istovar zavrsen - sada igrac mora vratiti kamion u bazu!
    if(PlastikaPlayerData[playerid][plasIstovarProgress] >= PLASTIKA_ISTOVAR_VRIJEME) {
        KillTimer(PlastikaPlayerData[playerid][plasTimerIstovar]);
        PlastikaPlayerData[playerid][plasTimerIstovar] = -1;

        // Ukloni cijev sa kamiona
        if(PlastikaPlayerData[playerid][plasCijevObjID] != -1) {
            DestroyDynamicObject(PlastikaPlayerData[playerid][plasCijevObjID]);
            PlastikaPlayerData[playerid][plasCijevObjID] = -1;
        }

        // Postavi stanje - vraca kamion u bazu
        PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_RETURNING;

        // Pokreni motor kamiona
        new vid = PlastikaPlayerData[playerid][plasVehicleID];
        new engine, lights, alarm, doors, bonnet, boot, objective;
        GetVehicleParamsEx(vid, engine, lights, alarm, doors, bonnet, boot, objective);
        SetVehicleParamsEx(vid, VEHICLE_PARAMS_ON, lights, alarm, doors, bonnet, boot, objective);

        // Postavi checkpoint ka bazi (lokacija kamiona)
        SetPlayerCheckpoint(playerid, PLASTIKA_KAMION_X, PLASTIKA_KAMION_Y, PLASTIKA_KAMION_Z, 5.0);

        // Poruka
        Plas_HideProgress(playerid);
        PlayerPlaySound(playerid, PLASTIKA_SOUND_GOTOVO, 0.0, 0.0, 0.0);
        PlasMSG(playerid, "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Cijev istovarena! Sada vratite kamion u bazu.");
        PlasINFO(playerid, "Vozite kamion nazad do baze da biste dobili platu.");

        // Status TD
        Plas_UpdateStatusTD(playerid, "{1D75C4}FABRIKA PLASTIKE~n~~w~Status: Vrati kamion u bazu");
    }
    return 1;
}

// Povratak kamiona u bazu - daje nagradu
stock Plastika_PovratakBaza(playerid) {
    if(PlastikaPlayerData[playerid][plasState] != PLASTIKA_STATE_RETURNING) {
        return 0;
    }

    // Mora biti u job kamionu
    if(!PlasIsInJobVehicle(playerid)) {
        return PlasGRESKA(playerid, "Morate biti u kamionu da biste ga vratili u bazu!");
    }

    // Provjera udaljenosti kamiona od baze
    new Float:vx, Float:vy, Float:vz;
    GetVehiclePos(PlastikaPlayerData[playerid][plasVehicleID], vx, vy, vz);
    if(PlasDist(vx, vy, vz, PLASTIKA_KAMION_X, PLASTIKA_KAMION_Y, PLASTIKA_KAMION_Z) > PLASTIKA_POVRAAT_DIST) {
        return 0; // Nije jos kod baze
    }

    // UniStti kamion
    if(PlastikaPlayerData[playerid][plasVehicleID] != -1) {
        new vid = PlastikaPlayerData[playerid][plasVehicleID];
        RemovePlayerFromVehicle(playerid);
        DestroyVehicle(vid);
        PlastikaPlayerData[playerid][plasVehicleID] = -1;
    }

    // Naplata - fiksna $20,000 (bonus posao)
    GivePlayerMoney(playerid, PLASTIKA_NAPLATA);

    // Dodaj plastiku u inventory
    PlasInv_AddPlastika(playerid, PLASTIKA_NAGRADE_PLASTIKA);

    // Cooldown
    PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_COOLDOWN;
    PlastikaPlayerData[playerid][plasCooldownTime] = GetTickCount() + (PLASTIKA_COOLDOWN * 1000);

    // Vrati skin
    if(PlastikaPlayerData[playerid][plasHasEquipment] && PlastikaPlayerData[playerid][plasOldSkin] != -1) {
        SetPlayerSkin(playerid, PlastikaPlayerData[playerid][plasOldSkin]);
    }
    PlastikaPlayerData[playerid][plasHasEquipment] = false;

    // Ukloni PVar
    DeletePVar(playerid, "PlastikaWorking");

    // Poruka i efekti
    Plas_HideProgress(playerid);
    DisablePlayerCheckpoint(playerid);
    PlayerPlaySound(playerid, PLASTIKA_SOUND_NAPLATA, 0.0, 0.0, 0.0);

    new str[128];
    format(str, sizeof(str), "{1D75C4}[FABRIKA PLASTIKE]{FFFFFF} Kamion vracen u bazu! Zaradili ste {00FF00}$%d{FFFFFF}.", PLASTIKA_NAPLATA);
    PlasMSG(playerid, str);
    format(str, sizeof(str), "Dobili ste {1D75C4}%d Plastike{FFFFFF} u inventory!", PLASTIKA_NAGRADE_PLASTIKA);
    PlasSUCCESS(playerid, str);
    format(str, sizeof(str), "Slijedeca dostava za %d minuta (bonus cooldown).", PLASTIKA_COOLDOWN / 60);
    PlasINFO(playerid, str);

    return 1;
}

// ===========================================================================
//  CHECKPOINT HANDLER
// ===========================================================================

stock Plastika_OnCP(playerid) {
    if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_NONE) return 0;
    if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_COOLDOWN) return 0;

    // Kad udje u CP za utovar - AUTOMATSKI pokreni utovar
    // (SA-MP ne detektuje ALT/KEY_WALK u vozilu, pa koristimo checkpoint)
    if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_DRIVING) {
        Plastika_PocniUtovar(playerid);
        return 1;
    }

    // Kad udje u CP za dostavu - AUTOMATSKI pokreni istovar
    if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_DELIVERING) {
        Plastika_PocniIstovar(playerid);
        return 1;
    }

    // Kad udje u CP za povratak u bazu - AUTOMATSKI vrati kamion
    if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_RETURNING) {
        Plastika_PovratakBaza(playerid);
        return 1;
    }

    return 0;
}

// ===========================================================================
//  KEY STATE CHANGE HANDLER - ALT (KEY_WALK) za interakciju na nogama
//  ZA VOZILO: Ne koristimo tipke - sve se automatski okida preko checkpointa
//  (SA-MP ne detektuje KEY_WALK u vozilu, pa koristimo OnPlayerEnterCheckpoint)
// ===========================================================================

stock Plastika_OnKeyStateChange(playerid, newkeys, oldkeys) {
    if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_NONE) return 0;
    if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_COOLDOWN) return 0;
    if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_LOADING) return 0;
    if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_UNLOADING) return 0;

    // ALT = KEY_WALK (walk key) - RADI SAMO NA NOGAMA (za spawn kamiona)
    if(!(newkeys & KEY_WALK)) return 0;
    if(oldkeys & KEY_WALK) return 0;

    // Ako je u vozilu, ignorisi - checkpointi rade za vozilo
    if(IsPlayerInAnyVehicle(playerid)) return 0;

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    // Stanje: EQUIPPED - moze spawnati kamion (NA NOGAMA)
    if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_EQUIPPED) {
        if(PlasDist(x, y, z, PLASTIKA_KAMION_X, PLASTIKA_KAMION_Y, PLASTIKA_KAMION_Z) <= PLASTIKA_KAMION_DIST) {
            Plastika_SpawnKamion(playerid);
            return 1;
        }
    }

    return 0;
}

// ===========================================================================
//  ONUPDATE - Vise nije potreban za key detekciju u vozilu!
//  Sve se sada automatski okida preko OnPlayerEnterCheckpoint.
//  Ostavljamo samo za eventualne buduce provjere.
// ===========================================================================

stock Plastika_OnUpdate(playerid) {
    // Key detekcija za vozilo je uklonjena - koriste se checkpointi
    // Ova funkcija je ostavljena za kompatibilnost sa hook-om u gamemode-u
    return 0;
}

// ===========================================================================
//  VEHICLE EVENTS
// ===========================================================================

stock Plastika_OnVehicleDeath(vehicleid) {
    // Provjeri da li je ovo job kamion nekog igraca
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(!IsPlayerConnected(i)) continue;
        if(PlastikaPlayerData[i][plasVehicleID] == vehicleid) {
            PlastikaPlayerData[i][plasVehicleID] = -1;

            // UniStti cijev objekat
            if(PlastikaPlayerData[i][plasCijevObjID] != -1) {
                DestroyDynamicObject(PlastikaPlayerData[i][plasCijevObjID]);
                PlastikaPlayerData[i][plasCijevObjID] = -1;
            }

            // Ubij timere
            if(PlastikaPlayerData[i][plasTimerUtovar] != -1) {
                KillTimer(PlastikaPlayerData[i][plasTimerUtovar]);
                PlastikaPlayerData[i][plasTimerUtovar] = -1;
            }
            if(PlastikaPlayerData[i][plasTimerIstovar] != -1) {
                KillTimer(PlastikaPlayerData[i][plasTimerIstovar]);
                PlastikaPlayerData[i][plasTimerIstovar] = -1;
            }

            Plas_HideProgress(i);
            DisablePlayerCheckpoint(i);

            // Vrati skin i resetuj
            if(PlastikaPlayerData[i][plasHasEquipment] && PlastikaPlayerData[i][plasOldSkin] != -1) {
                SetPlayerSkin(i, PlastikaPlayerData[i][plasOldSkin]);
            }
            PlastikaPlayerData[i][plasState] = PLASTIKA_STATE_NONE;
            PlastikaPlayerData[i][plasHasEquipment] = false;
            DeletePVar(i, "PlastikaWorking");

            PlasGRESKA(i, "Kamion je unisten! Posao prekinut.");
            break;
        }
    }
    return 1;
}

stock Plastika_OnPlayerExitVehicle(playerid, vehicleid) {
    if(PlastikaPlayerData[playerid][plasVehicleID] == vehicleid) {
        // Ako je u loading ili unloading stanju - timeri ce se pobrinuti
        // Ako je u DRIVING stanju - moze se vratiti
        if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_DRIVING) {
            PlasINFO(playerid, "Izasli ste iz kamiona! Vratite se u kamion da nastavite.");
        }
        if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_DELIVERING) {
            PlasINFO(playerid, "Izasli ste iz kamiona! Vratite se u kamion da nastavite dostavu.");
        }
        if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_RETURNING) {
            PlasINFO(playerid, "Izasli ste iz kamiona! Vratite se u kamion da ga vratite u bazu.");
        }
    }
    return 1;
}

// ===========================================================================
//  INFO KOMANDA
// ===========================================================================

stock Plastika_Info(playerid) {
    new str[512];
    format(str, sizeof(str),
        "{1D75C4}=== FABRIKA PLASTIKE - INFO ===\n\
        {FFFFFF}Posao: Odvoz plasticne cijevi na lokaciju (bonus posao)\n\
        \n\
        {FFD700}Koraci:{FFFFFF}\n\
        1. /uzmiopremup - Uzmi opremu na lokaciji\n\
        2. Idi do kamiona i stisni ALT da spawnas vozilo\n\
        3. Vozi do utovara - utovar se automatski pokrece\n\
        4. Vozi cijev do dostavne lokacije - istovar automatski\n\
        5. Vrati kamion u bazu - plata automatski\n\
        \n\
        {FF6347}/prekidaplastiku{FFFFFF} - Prekini posao u bilo kom trenutku\n\
        \n\
        {00FF00}Nagrada: $%d + %d Plastike u inventory\n\
        Cooldown: %d minuta",
        PLASTIKA_NAPLATA, PLASTIKA_NAGRADE_PLASTIKA, PLASTIKA_COOLDOWN / 60);

    ShowPlayerDialog(playerid, 9117, DIALOG_STYLE_MSGBOX, "{1D75C4}Fabrika Plastike - Info", str, "Zatvori", "");
    return 1;
}

// ===========================================================================
//  ADMIN FUNKCIJE - za pristup izvan include-a
// ===========================================================================

// Resetuj cooldown za plastiku
stock Plastika_ResetCooldown(playerid) {
    if(!PlasValidPlayer(playerid)) return 0;
    PlastikaPlayerData[playerid][plasCooldownTime] = 0;
    if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_COOLDOWN) {
        PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_NONE;
    }
    return 1;
}

// Postavi cooldown za plastiku (u sekundama, 0 = reset)
stock Plastika_SetCooldown(playerid, seconds) {
    if(!PlasValidPlayer(playerid)) return 0;
    if(seconds <= 0) {
        PlastikaPlayerData[playerid][plasCooldownTime] = 0;
        if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_COOLDOWN) {
            PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_NONE;
        }
    } else {
        PlastikaPlayerData[playerid][plasCooldownTime] = GetTickCount() + (seconds * 1000);
        if(PlastikaPlayerData[playerid][plasState] == PLASTIKA_STATE_NONE) {
            PlastikaPlayerData[playerid][plasState] = PLASTIKA_STATE_COOLDOWN;
        }
    }
    return 1;
}

// Daj plastiku igracu u inventory
stock Plastika_GivePlastika(playerid, kolicina) {
    if(!PlasValidPlayer(playerid)) return 0;
    return PlasInv_AddPlastika(playerid, kolicina);
}
