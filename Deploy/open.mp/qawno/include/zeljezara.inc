// ===========================================================================
//  SISTEM ZELJEZARE - SA-MP
//  Verzija: 2.0
//  Autor: Beka (uz pomoc AI asistenta)
//  Opis: Kompletan, modularan sistem zeljezare za SA-MP server.
//        Igrac dolazi na lokaciju za opremu, uzima opremu, kopa metal
//        na random checkpointovima, zatim ide u halu na preradu.
//        Prerada daje Metal i Metalni Otpad (Scrap Metal) koji se
//        spremaju u inventory.
//
//  KORISCENJE:
//    - Zeljezara_Init()       - poziva se u OnGameModeInit
//    - Zeljezara_OnConnect()  - poziva se u OnPlayerConnect
//    - Zeljezara_OnDisconnect()- poziva se u OnPlayerDisconnect
//    - Zeljezara_OnSpawn()    - poziva se u OnPlayerSpawn
//    - Zeljezara_OnUpdate()   - poziva se u OnPlayerUpdate (za distance check)
//    - Zeljezara_OnDeath()    - poziva se u OnPlayerDeath
//    - Zeljezara_OnDialog()   - poziva se u OnDialogResponse
// ===========================================================================

#if defined _zeljezara_included
    #endinput
#endif
#define _zeljezara_included

// ===========================================================================
//  KONFIGURACIJA - Lakse menjanje bez diranja koda
// ===========================================================================

#define MAX_CHECKPOINTS_Z              10      // Max aktivnih checkpointova
#define ZELJEZARA_MIN_CHECKPOINTS      5       // Min checkpointova za kopanje
#define ZELJEZARA_MAX_CHECKPOINTS      10      // Max checkpointova za kopanje
#define ZELJEZARA_CP_RANGE             1.5     // Domet checkpointa za aktivaciju (blize objektu)
#define ZELJEZARA_KOPANJE_VRIJEME      8       // Sekunde kopanja po checkpointu (stari sistem)
#define ZELJEZARA_CLICKS_PER_CP        3       // Broj klikova (KEY_FIRE) po checkpointu
#define ZELJEZARA_CLICK_COOLDOWN       1500    // Milisekunde cooldown izmedu klikova (1.5 sec)
#define ZELJEZARA_PRERADA_VRIJEME      12      // Sekunde prerade metala
#define ZELJEZARA_COOLDOWN             300     // Sekunde cooldowna (5 min)
#define ZELJEZARA_MIN_METAL            3       // Min sirovog metala po checkpointu
#define ZELJEZARA_MAX_METAL            8       // Max sirovog metala po checkpointu
#define ZELJEZARA_METAL_PRERADA        60      // Procenat dobijanja Metal (ostalo je Scrap)
#define ZELJEZARA_MAX_RAW_METAL        100     // Max sirovog metala koji igrac moze nositi
#define ZELJEZARA_OPREMA_SKIN          16      // Skin kad nosi opremu
#define ZELJEZARA_OPREMA_DIST          3.0     // Domet za uzimanje opreme
#define ZELJEZARA_PRERADA_DIST         4.0     // Domet za masinu prerade
#define ZELJEZARA_TRMOVE_SPEED         0.5     // Brzina kretanja objekta na traci

// Item imena (koriste se u inventory sistemu)
#define ZELJEZARA_ITEM_METAL           "Metal"
#define ZELJEZARA_ITEM_SCRAP           "Metalni Otpad"
#define ZELJEZARA_ITEM_RAW_METAL       "Sirovi Metal"

// Item modeli za inventory prikaz (SA-MP object model IDs za DIALOG_STYLE_PREVIEW)
#define ZELJEZARA_MODEL_METAL          19631   // Metal ingot ikona u inventory
#define ZELJEZARA_MODEL_SCRAP          19942   // Scrap metal ikona u inventory
#define ZELJEZARA_MODEL_RAW_METAL      19940   // Sirovi metal ikona (zuta)
#define ZELJEZARA_MODEL_METAL_BELT     19846   // Metal na traci model

// Zvukovi
#define ZELJEZARA_SOUND_KOPANJE        33700   // Pickaxe sound
#define ZELJEZARA_SOUND_PRERADA        33401   // Factory machine sound
#define ZELJEZARA_SOUND_OPREMA         33100   // Pickup sound
#define ZELJEZARA_SOUND_GOTOVO         1057    // Complete sound
#define ZELJEZARA_SOUND_STROJ          1150    // Conveyor belt sound

// Animacije - koristimo BASEBALL zamah za kopanje (radi s oruzjem u ruci)
#define ZELJEZARA_ANIM_KOPANJE_LIB     "BASEBALL"
#define ZELJEZARA_ANIM_KOPANJE_NAME    "Bat_1"
#define ZELJEZARA_ANIM_KOPANJE_TIME    4.0
#define ZELJEZARA_ANIM_STOJI_LIB       "PED"
#define ZELJEZARA_ANIM_STOJI_NAME      "IDLE_STANCED"

// Oprema za kopanje
#define ZELJEZARA_WEAPON_KOPANJE       6       // Shovel (lopata/alatka za kopanje)

// ===========================================================================
//  ENUMERACIJE I PROMJENLJIVE
// ===========================================================================

// Stanja sistema za igraca
enum e_ZeljezaraState {
    ZELJEZARA_STATE_NONE = 0,       // Ne radi nista
    ZELJEZARA_STATE_EQUIPPED,      // Uzeo opremu, spreman za kopanje
    ZELJEZARA_STATE_MINING,        // Aktivan sistem kopanja
    ZELJEZARA_STATE_PROCESSING,    // U hali za preradu
    ZELJEZARA_STATE_CONVEYOR,      // Metal ide na traci
    ZELJEZARA_STATE_COOLDOWN       // Cooldown nakon zavrsetka
}

// Podaci o igracu u sistemu
enum e_ZeljezaraPlayer {
    e_ZeljezaraState:zeljState,
    zeljRawMetal,                   // Kolicina sirovog metala
    zeljCheckpointIdx,              // Trenutni checkpoint index
    zeljCheckpointsTotal,           // Ukupno checkpointova
    zeljCheckpointsDone,            // Zavrseni checkpointi
    zeljActiveCP0, zeljActiveCP1, zeljActiveCP2, zeljActiveCP3, zeljActiveCP4,
    zeljActiveCP5, zeljActiveCP6, zeljActiveCP7, zeljActiveCP8, zeljActiveCP9,
    bool:zeljHasEquipment,          // Da li ima opremu
    zeljOldSkin,                    // Stari skin prije opreme
    zeljCooldownTime,               // Vrijeme cooldowna (Gettickcount)
    zeljTimerKopanje,               // Timer za kopanje
    zeljTimerPrerada,               // Timer za preradu
    zeljTimerTraka,                 // Timer za traku
    Float:zeljTrakaX,               // Trenutna X pozicija objekta na traci
    Float:zeljTrakaY,               // Trenutna Y pozicija objekta na traci
    Float:zeljTrakaZ,               // Trenutna Z pozicija objekta na traci
    Float:zeljTrakaTX,              // Target X
    Float:zeljTrakaTY,              // Target Y
    Float:zeljTrakaTZ,              // Target Z
    zeljTrakaStep,                  // Korak na traci (0=start, 1=sredina, 2=kraj)
    zeljTrakaObj,                   // Objekat na traci
    zeljTrakaIdx,                   // Koja traka (0 ili 1)
    bool:zeljDigging,               // Da li trenutno kopa (za freeze)
    PlayerText:zeljProgressBar,     // Progress bar TD
    PlayerText:zeljProgressBgr,     // Progress bar background TD
    PlayerText:zeljInfoTD,          // Info textdraw
    PlayerText:zeljRawMetalTD,      // Prikaz sirovog metala
    PlayerText:zeljProgressLabel,   // Label iznad progress bara (KOPANJE...)
    bool:zeljTDShown,               // Da li su TD prikazani
    bool:zeljAbuseFlag,             // Anti-abuse flag
    zeljLastCPTime,                 // Zadnje vrijeme CP aktivacije
    zeljAbuseWarnings,              // Broj upozorenja za abuse
    zeljMiningPickup,               // Dynamic pickup ID za zuti marker na kopanju
    zeljContainerObj,               // Dynamic object ID za zeljezni objekat na trenutnom CP
    zeljClickCount,                 // Broj klikova na trenutnom CP
    bool:zeljWaitingClick,          // Da li ceka klik za kopanje
    zeljLastClickTime               // Vrijeme zadnjeg klika (GetTickCount) za cooldown
}

// Pool koordinata za kopanje - IZVORNE koordinate iz mape za 19846 objekte
// Ovo su TACNE pozicije gdje su 19846 objekti bili na originalnoj mapi
// Ne smiju se mijenjati - objekti moraju biti na istom mjestu kao na mapi!
static const Float:ZeljMiningCoords[][] = {
    {-963.204345, -523.833312, 31.467267},   // 1
    {-964.449768, -525.572753, 31.801979},   // 2
    {-964.885070, -529.436706, 31.929422},   // 3
    {-964.782226, -534.410095, 31.918121},   // 4
    {-961.768798, -537.980163, 31.612485},   // 5
    {-961.370239, -534.117004, 31.746240},   // 6
    {-961.345336, -529.133972, 31.160903},   // 7
    {-963.345031, -540.422485, 30.910140},   // 8
    {-947.112548, -523.833312, 31.467267},   // 9
    {-948.357971, -525.572753, 31.801979},   // 10
    {-948.793273, -529.436706, 31.929422},   // 11
    {-948.690429, -534.410095, 31.918121},   // 12
    {-945.677001, -537.980163, 31.612485},   // 13
    {-945.278442, -534.117004, 31.746240},   // 14
    {-945.253540, -529.133972, 31.160903},   // 15
    {-947.253234, -540.422485, 30.910140},   // 16
    {-961.857666, -507.332275, 31.599847},   // 17
    {-960.858093, -508.765411, 31.762411},   // 18
    {-957.642272, -509.233276, 31.917024},   // 19
    {-952.628356, -509.531036, 31.358137},   // 20
    {-949.086181, -506.245178, 31.931331},   // 21
    {-952.961547, -505.737060, 31.687076},   // 22
    {-957.928588, -505.841400, 31.329877},   // 23
    {-946.290527, -507.563812, 31.643850}    // 24
};

// Rotacije za 19846 objekte - IZVORNE rotacije iz mape
// Svaki objekt ima svoju specifiknu rotaciju na mapi!
static const Float:ZeljMiningRot[][] = {
    {-75.200050, -2.700000, 0.699998},       // 1
    {-75.200050, -2.700000, 90.699996},      // 2
    {-75.200050, -2.700000, 90.699996},      // 3
    {-94.000076, -3.399999, 90.699996},      // 4
    {-94.200019, -2.700029, -89.299964},     // 5
    {-105.400100, -2.700029, -89.299964},    // 6
    {-85.999916, -183.399871, 90.700080},    // 7
    {-94.200019, -2.700029, 176.500030},     // 8
    {-75.200050, -2.699970, 0.700027},       // 9
    {-75.200035, -2.700000, 90.699974},      // 10
    {-75.200035, -2.700000, 90.699974},      // 11
    {-85.999916, -183.399978, -89.300010},   // 12
    {-85.799980, 177.299957, 90.700042},     // 13
    {-74.599884, 177.299957, 90.700042},     // 14
    {-85.999916, -183.399871, 90.700057},    // 15
    {-85.799980, -182.699920, -3.499896},    // 16
    {-75.200035, -2.699911, 90.700035},      // 17
    {-75.200012, -2.700031, -179.300003},    // 18
    {-75.200012, -2.700031, -179.300003},    // 19
    {-85.999916, -183.399871, 0.700114},     // 20
    {-85.799980, 177.299850, -179.300003},   // 21
    {-74.599845, 177.299926, -179.299926},   // 22
    {-85.999916, -183.399978, -179.300003},  // 23
    {-85.799980, -182.699707, 86.500274}     // 24
};

// Lokacija za uzimanje opreme
static const Float:ZeljEquipmentPos[4] = {-955.9962, -499.9709, 25.9883, 162.4834};

// Lokacija za preradu (masina - pocetak trake)
static const Float:ZeljProcessPos[2][4] = {
    {-945.9368, -487.6866, 25.9683, 0.3031},
    {-946.2823, -491.7970, 25.9683, 182.8736}
};

// Traka putanja - pozicije za animaciju kretanja metala
// Korisnik /save na sredini trake: -942.9901,-486.6010,26.8679
// Traka 1: Y=-486.70, Z=26.32 (isto kao 920 objekti na traci u mapi)
// Traka 2: Y=-492.80, Z=26.32
// Z spusten sa 26.87 na 26.32 - objekat mora lezati RAVNO po traci, ne u zraku!
static const Float:ZeljConveyorPath[2][2][3] = {
    {
        {-945.6589, -486.7000, 26.3200},   // Traka 1 - pocetak (ubacivanje)
        {-937.7733, -486.7000, 26.3200}    // Traka 1 - kraj (preuzimanje)
    },
    {
        {-946.1083, -492.8000, 26.3200},   // Traka 2 - pocetak (ubacivanje)
        {-938.4128, -492.8000, 26.3200}    // Traka 2 - kraj (preuzimanje)
    }
};

// CP pozicije za preuzimanje preradjenog metala (kraj trake)
static const Float:ZeljPickupPos[2][4] = {
    {-937.7733, -486.7000, 26.3200, 72.6683},   // Traka 1 - kraj
    {-938.4128, -492.8000, 26.3200, 181.5322}   // Traka 2 - kraj
};

// 3D Text labelovi
static Text3D:Zelj3D_Oprema = Text3D:-1;
static Text3D:Zelj3D_Prerada[2] = {Text3D:-1, ...};

// Pickups
static ZeljPickup_Oprema = -1;
static ZeljPickup_Prerada[2] = {-1, ...};

// Map icon
static ZeljMapIcon = -1;

// Mining 3D label (per-player, za CP marker iznad zeljeznog objekta)
static Text3D:ZeljMiningLabel[MAX_PLAYERS] = {Text3D:-1, ...};

// Kontejner objekti - UVIJEK vidljivi na svim mining lokacijama (kreativni objekti)
static ZeljContainerObjs[24] = {-1, ...};

// Player data
static ZeljPlayerData[MAX_PLAYERS][e_ZeljezaraPlayer];

// Unused pragma za static varijable (koriste se interno)
#pragma unused Zelj3D_Oprema
#pragma unused Zelj3D_Prerada
#pragma unused ZeljPickup_Oprema
#pragma unused ZeljPickup_Prerada
#pragma unused ZeljMapIcon
#pragma unused ZeljContainerObjs

// Visual mining marker model (plavi marker na kopanju)
#define ZELJEZARA_MODEL_MINING_CP     19607

// Modeli za zeljezo/zeljezni objekti na kopanju (random izbor za raznolikost)
// Koristimo objekte slicne onima u zeljezari (935 sa metal teksturama)
#define ZELJEZARA_IRON_MODEL_COUNT    6
static const ZeljIronModels[ZELJEZARA_IRON_MODEL_COUNT] = {935, 935, 935, 920, 934, 853};
// 935 = metalni komad (isti kao na traci u zeljezari, sa metal teksturama)
// 920 = metalna kutija, 934 = metalni kontejner, 853 = metalni otpad

// Forwardanje timer callbacka
forward ZeljTimer_Kopanje(playerid);
forward ZeljTimer_Prerada(playerid);
forward ZeljTimer_Traka(playerid);
forward ZeljTimer_Cooldown(playerid);

// ===========================================================================
//  HELPER FUNKCIJE
// ===========================================================================

// Stock za slanje poruka (kompatibilan sa serverom)
stock ZeljMSG(playerid, msg[]) {
    SendClientMessage(playerid, 0x00BFFFAA, msg);
}

stock ZeljGRESKA(playerid, msg[]) {
    SendClientMessage(playerid, 0xFF6347AA, msg);
    return 1;
}

stock ZeljINFO(playerid, msg[]) {
    SendClientMessage(playerid, 0xFFD700AA, msg);
    return 1;
}

// Float udaljenost
stock Float:ZeljDist(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2) {
    new Float:dx = x1 - x2;
    new Float:dy = y1 - y2;
    new Float:dz = z1 - z2;
    return floatsqroot(dx*dx + dy*dy + dz*dz);
}

// Provjera da li je igrac u blizini zeljezare (generalno podrucje)
stock bool:ZeljIsInArea(playerid) {
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    if(x < -970.0 || x > -900.0) return false;
    if(y < -555.0 || y > -480.0) return false;
    return true;
}

// Random broj u opsegu
stock ZeljRandomRange(min, max) {
    return min + random(max - min + 1);
}

// Provjera validnosti igraca
stock bool:ZeljValidPlayer(playerid) {
    if(playerid < 0 || playerid >= MAX_PLAYERS) return false;
    if(!IsPlayerConnected(playerid)) return false;
    return true;
}

// Helper za pristup aktivnim checkpointima (umjesto niza u enumu)
stock Zelj_ResetActiveCPs(playerid) {
    ZeljPlayerData[playerid][zeljActiveCP0] = -1;
    ZeljPlayerData[playerid][zeljActiveCP1] = -1;
    ZeljPlayerData[playerid][zeljActiveCP2] = -1;
    ZeljPlayerData[playerid][zeljActiveCP3] = -1;
    ZeljPlayerData[playerid][zeljActiveCP4] = -1;
    ZeljPlayerData[playerid][zeljActiveCP5] = -1;
    ZeljPlayerData[playerid][zeljActiveCP6] = -1;
    ZeljPlayerData[playerid][zeljActiveCP7] = -1;
    ZeljPlayerData[playerid][zeljActiveCP8] = -1;
    ZeljPlayerData[playerid][zeljActiveCP9] = -1;
}

stock Zelj_SetActiveCP(playerid, index, value) {
    switch(index) {
        case 0: ZeljPlayerData[playerid][zeljActiveCP0] = value;
        case 1: ZeljPlayerData[playerid][zeljActiveCP1] = value;
        case 2: ZeljPlayerData[playerid][zeljActiveCP2] = value;
        case 3: ZeljPlayerData[playerid][zeljActiveCP3] = value;
        case 4: ZeljPlayerData[playerid][zeljActiveCP4] = value;
        case 5: ZeljPlayerData[playerid][zeljActiveCP5] = value;
        case 6: ZeljPlayerData[playerid][zeljActiveCP6] = value;
        case 7: ZeljPlayerData[playerid][zeljActiveCP7] = value;
        case 8: ZeljPlayerData[playerid][zeljActiveCP8] = value;
        case 9: ZeljPlayerData[playerid][zeljActiveCP9] = value;
    }
}

stock Zelj_GetActiveCP(playerid, index) {
    switch(index) {
        case 0: return ZeljPlayerData[playerid][zeljActiveCP0];
        case 1: return ZeljPlayerData[playerid][zeljActiveCP1];
        case 2: return ZeljPlayerData[playerid][zeljActiveCP2];
        case 3: return ZeljPlayerData[playerid][zeljActiveCP3];
        case 4: return ZeljPlayerData[playerid][zeljActiveCP4];
        case 5: return ZeljPlayerData[playerid][zeljActiveCP5];
        case 6: return ZeljPlayerData[playerid][zeljActiveCP6];
        case 7: return ZeljPlayerData[playerid][zeljActiveCP7];
        case 8: return ZeljPlayerData[playerid][zeljActiveCP8];
        case 9: return ZeljPlayerData[playerid][zeljActiveCP9];
    }
    return -1;
}

// ===========================================================================
//  INVENTORY INTEGRACIJA
//  Koristi postojece funkcije: Inventory_Add, Inventory_Remove, 
//  Inventory_HasItem, Inventory_Count
// ===========================================================================

// Dodaj Metal u inventory
stock ZeljInv_AddMetal(playerid, kolicina) {
    return CallLocalFunction("Inventory_Add", "dsdd", playerid, ZELJEZARA_ITEM_METAL, ZELJEZARA_MODEL_METAL, kolicina);
}

// Dodaj Scrap Metal u inventory
stock ZeljInv_AddScrap(playerid, kolicina) {
    return CallLocalFunction("Inventory_Add", "dsdd", playerid, ZELJEZARA_ITEM_SCRAP, ZELJEZARA_MODEL_SCRAP, kolicina);
}

// Dodaj Sirovi Metal u inventory
stock ZeljInv_AddRaw(playerid, kolicina) {
    return CallLocalFunction("Inventory_Add", "dsdd", playerid, ZELJEZARA_ITEM_RAW_METAL, ZELJEZARA_MODEL_RAW_METAL, kolicina);
}

// Provjeri kolicinu sirovog metala
stock ZeljInv_CountRaw(playerid) {
    return CallLocalFunction("Inventory_Count", "ds", playerid, ZELJEZARA_ITEM_RAW_METAL);
}

// Ukloni sirovi metal iz inventoryja
stock ZeljInv_RemoveRaw(playerid, kolicina) {
    return CallLocalFunction("Inventory_Remove", "dsd", playerid, ZELJEZARA_ITEM_RAW_METAL, kolicina);
}

// ===========================================================================
//  TEXTDRAWS - Progress bar i Info TD za sistem
// ===========================================================================

stock Zelj_CreatePlayerTDs(playerid) {
    // Progress bar background
    ZeljPlayerData[playerid][zeljProgressBgr] = CreatePlayerTextDraw(playerid, 250.000000, 400.000000, "_");
    PlayerTextDrawBackgroundColor(playerid, ZeljPlayerData[playerid][zeljProgressBgr], 255);
    PlayerTextDrawFont(playerid, ZeljPlayerData[playerid][zeljProgressBgr], 1);
    PlayerTextDrawLetterSize(playerid, ZeljPlayerData[playerid][zeljProgressBgr], 0.500000, 1.500000);
    PlayerTextDrawColor(playerid, ZeljPlayerData[playerid][zeljProgressBgr], -1);
    PlayerTextDrawSetOutline(playerid, ZeljPlayerData[playerid][zeljProgressBgr], 1);
    PlayerTextDrawSetProportional(playerid, ZeljPlayerData[playerid][zeljProgressBgr], 1);
    PlayerTextDrawSetShadow(playerid, ZeljPlayerData[playerid][zeljProgressBgr], 1);
    PlayerTextDrawUseBox(playerid, ZeljPlayerData[playerid][zeljProgressBgr], 1);
    PlayerTextDrawBoxColor(playerid, ZeljPlayerData[playerid][zeljProgressBgr], 80);
    PlayerTextDrawTextSize(playerid, ZeljPlayerData[playerid][zeljProgressBgr], 420.000000, 0.000000);

    // Progress bar (popunjavanje)
    ZeljPlayerData[playerid][zeljProgressBar] = CreatePlayerTextDraw(playerid, 251.000000, 401.000000, "_");
    PlayerTextDrawBackgroundColor(playerid, ZeljPlayerData[playerid][zeljProgressBar], 255);
    PlayerTextDrawFont(playerid, ZeljPlayerData[playerid][zeljProgressBar], 1);
    PlayerTextDrawLetterSize(playerid, ZeljPlayerData[playerid][zeljProgressBar], 0.479166, 1.200000);
    PlayerTextDrawColor(playerid, ZeljPlayerData[playerid][zeljProgressBar], -1);
    PlayerTextDrawSetOutline(playerid, ZeljPlayerData[playerid][zeljProgressBar], 0);
    PlayerTextDrawSetProportional(playerid, ZeljPlayerData[playerid][zeljProgressBar], 1);
    PlayerTextDrawSetShadow(playerid, ZeljPlayerData[playerid][zeljProgressBar], 0);
    PlayerTextDrawUseBox(playerid, ZeljPlayerData[playerid][zeljProgressBar], 1);
    PlayerTextDrawBoxColor(playerid, ZeljPlayerData[playerid][zeljProgressBar], 0x00BFFF66);
    PlayerTextDrawTextSize(playerid, ZeljPlayerData[playerid][zeljProgressBar], 251.000000, 0.000000);

    // Info textdraw (gore desno)
    ZeljPlayerData[playerid][zeljInfoTD] = CreatePlayerTextDraw(playerid, 620.000000, 130.000000, "_");
    PlayerTextDrawAlignment(playerid, ZeljPlayerData[playerid][zeljInfoTD], 3);
    PlayerTextDrawBackgroundColor(playerid, ZeljPlayerData[playerid][zeljInfoTD], 255);
    PlayerTextDrawFont(playerid, ZeljPlayerData[playerid][zeljInfoTD], 2);
    PlayerTextDrawLetterSize(playerid, ZeljPlayerData[playerid][zeljInfoTD], 0.220000, 1.000000);
    PlayerTextDrawColor(playerid, ZeljPlayerData[playerid][zeljInfoTD], 0x00BFFFCC);
    PlayerTextDrawSetOutline(playerid, ZeljPlayerData[playerid][zeljInfoTD], 1);
    PlayerTextDrawSetProportional(playerid, ZeljPlayerData[playerid][zeljInfoTD], 1);
    PlayerTextDrawSetShadow(playerid, ZeljPlayerData[playerid][zeljInfoTD], 0);

    // Raw metal counter
    ZeljPlayerData[playerid][zeljRawMetalTD] = CreatePlayerTextDraw(playerid, 620.000000, 145.000000, "_");
    PlayerTextDrawAlignment(playerid, ZeljPlayerData[playerid][zeljRawMetalTD], 3);
    PlayerTextDrawBackgroundColor(playerid, ZeljPlayerData[playerid][zeljRawMetalTD], 255);
    PlayerTextDrawFont(playerid, ZeljPlayerData[playerid][zeljRawMetalTD], 2);
    PlayerTextDrawLetterSize(playerid, ZeljPlayerData[playerid][zeljRawMetalTD], 0.200000, 0.900000);
    PlayerTextDrawColor(playerid, ZeljPlayerData[playerid][zeljRawMetalTD], 0xFFD700CC);
    PlayerTextDrawSetOutline(playerid, ZeljPlayerData[playerid][zeljRawMetalTD], 1);
    PlayerTextDrawSetProportional(playerid, ZeljPlayerData[playerid][zeljRawMetalTD], 1);
    PlayerTextDrawSetShadow(playerid, ZeljPlayerData[playerid][zeljRawMetalTD], 0);

    // Progress bar label (KOPANJE... / PRERADA... iznad bara)
    ZeljPlayerData[playerid][zeljProgressLabel] = CreatePlayerTextDraw(playerid, 335.000000, 385.000000, "_");
    PlayerTextDrawAlignment(playerid, ZeljPlayerData[playerid][zeljProgressLabel], 2);
    PlayerTextDrawBackgroundColor(playerid, ZeljPlayerData[playerid][zeljProgressLabel], 150);
    PlayerTextDrawFont(playerid, ZeljPlayerData[playerid][zeljProgressLabel], 2);
    PlayerTextDrawLetterSize(playerid, ZeljPlayerData[playerid][zeljProgressLabel], 0.300000, 1.100000);
    PlayerTextDrawColor(playerid, ZeljPlayerData[playerid][zeljProgressLabel], 0x00BFFFEE);
    PlayerTextDrawSetOutline(playerid, ZeljPlayerData[playerid][zeljProgressLabel], 1);
    PlayerTextDrawSetProportional(playerid, ZeljPlayerData[playerid][zeljProgressLabel], 1);
    PlayerTextDrawSetShadow(playerid, ZeljPlayerData[playerid][zeljProgressLabel], 0);
}

stock Zelj_ShowProgress(playerid, Float:percent, info[]) {
    new Float:barWidth = 251.0 + (168.0 * (percent / 100.0));
    PlayerTextDrawTextSize(playerid, ZeljPlayerData[playerid][zeljProgressBar], barWidth, 0.000000);
    PlayerTextDrawBoxColor(playerid, ZeljPlayerData[playerid][zeljProgressBar], 0x00BFFF66);
    PlayerTextDrawShow(playerid, ZeljPlayerData[playerid][zeljProgressBgr]);
    PlayerTextDrawShow(playerid, ZeljPlayerData[playerid][zeljProgressBar]);

    // Prikazi label iznad bara (info tekst npr. KOPANJE... / PRERADA...)
    PlayerTextDrawSetString(playerid, ZeljPlayerData[playerid][zeljProgressLabel], info);
    PlayerTextDrawShow(playerid, ZeljPlayerData[playerid][zeljProgressLabel]);

    PlayerTextDrawSetString(playerid, ZeljPlayerData[playerid][zeljInfoTD], info);
    PlayerTextDrawShow(playerid, ZeljPlayerData[playerid][zeljInfoTD]);
    ZeljPlayerData[playerid][zeljTDShown] = true;
}

stock Zelj_HideProgress(playerid) {
    if(ZeljPlayerData[playerid][zeljTDShown]) {
        PlayerTextDrawHide(playerid, ZeljPlayerData[playerid][zeljProgressBgr]);
        PlayerTextDrawHide(playerid, ZeljPlayerData[playerid][zeljProgressBar]);
        PlayerTextDrawHide(playerid, ZeljPlayerData[playerid][zeljProgressLabel]);
        PlayerTextDrawHide(playerid, ZeljPlayerData[playerid][zeljInfoTD]);
        PlayerTextDrawHide(playerid, ZeljPlayerData[playerid][zeljRawMetalTD]);
        ZeljPlayerData[playerid][zeljTDShown] = false;
    }
}

stock Zelj_UpdateRawMetalTD(playerid) {
    new str[64];
    format(str, sizeof(str), "Sirovi Metal: %d/%d", ZeljPlayerData[playerid][zeljRawMetal], ZELJEZARA_MAX_RAW_METAL);
    PlayerTextDrawSetString(playerid, ZeljPlayerData[playerid][zeljRawMetalTD], str);
    PlayerTextDrawShow(playerid, ZeljPlayerData[playerid][zeljRawMetalTD]);
}

// ===========================================================================
//  INICIJALIZACIJA SISTEMA
// ===========================================================================

stock Zeljezara_Init() {
    // Kreiraj pickup za opremu
    ZeljPickup_Oprema = CreateDynamicPickup(18634, 23, ZeljEquipmentPos[0], ZeljEquipmentPos[1], ZeljEquipmentPos[2]);
    
    // Kreiraj pickup za preradu
    ZeljPickup_Prerada[0] = CreateDynamicPickup(19631, 23, ZeljProcessPos[0][0], ZeljProcessPos[0][1], ZeljProcessPos[0][2]);
    ZeljPickup_Prerada[1] = CreateDynamicPickup(19631, 23, ZeljProcessPos[1][0], ZeljProcessPos[1][1], ZeljProcessPos[1][2]);

    // 3D Text labelovi
    Zelj3D_Oprema = CreateDynamic3DTextLabel("{00BFFF}[ZELEZARA - OPREMA]{FFFFFF}\nPritisni ALT da uzmes opremu\nKucaj /uzmiopremu", 
        0xFFFFFFFF, ZeljEquipmentPos[0], ZeljEquipmentPos[1], ZeljEquipmentPos[2], 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1);
    
    Zelj3D_Prerada[0] = CreateDynamic3DTextLabel("{FFD700}[ZELEZARA - PRERADA]{FFFFFF}\nKucaj /preradi da preradis sirovi metal", 
        0xFFFFFFFF, ZeljProcessPos[0][0], ZeljProcessPos[0][1], ZeljProcessPos[0][2], 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1);
    Zelj3D_Prerada[1] = CreateDynamic3DTextLabel("{FFD700}[ZELEZARA - PRERADA]{FFFFFF}\nKucaj /preradi da preradis sirovi metal", 
        0xFFFFFFFF, ZeljProcessPos[1][0], ZeljProcessPos[1][1], ZeljProcessPos[1][2], 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1);

    // Map icon
    ZeljMapIcon = CreateDynamicMapIcon(ZeljEquipmentPos[0], ZeljEquipmentPos[1], ZeljEquipmentPos[2], 44, 0);

    // Kontejneri (3722) su sada u zeljezara_mapa.inc - UVIJEK vidljivi
    // Ne kreiramo ih ovdje vise!

    print("[ZELEZARA] Sistem zeljezare uspjesno ucitan!");
    print("[ZELEZARA] Lokacija opreme, prerade i 24 koordinate za kopanje postavljene.");
    return 1;
}

// ===========================================================================
//  ONPLAYERCONNECT / DISCONNECT
// ===========================================================================

stock Zeljezara_OnConnect(playerid) {
    // Resetuj sve podatke
    ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_NONE;
    ZeljPlayerData[playerid][zeljRawMetal] = 0;
    ZeljPlayerData[playerid][zeljCheckpointIdx] = -1;
    ZeljPlayerData[playerid][zeljCheckpointsTotal] = 0;
    ZeljPlayerData[playerid][zeljCheckpointsDone] = 0;
    ZeljPlayerData[playerid][zeljHasEquipment] = false;
    ZeljPlayerData[playerid][zeljOldSkin] = -1;
    ZeljPlayerData[playerid][zeljCooldownTime] = 0;
    ZeljPlayerData[playerid][zeljTimerKopanje] = -1;
    ZeljPlayerData[playerid][zeljTimerPrerada] = -1;
    ZeljPlayerData[playerid][zeljTimerTraka] = -1;
    ZeljPlayerData[playerid][zeljTrakaObj] = -1;
    ZeljPlayerData[playerid][zeljTrakaIdx] = 0;
    ZeljPlayerData[playerid][zeljDigging] = false;
    ZeljPlayerData[playerid][zeljTDShown] = false;
    ZeljPlayerData[playerid][zeljAbuseFlag] = false;
    ZeljPlayerData[playerid][zeljLastCPTime] = 0;
    ZeljPlayerData[playerid][zeljAbuseWarnings] = 0;
    ZeljPlayerData[playerid][zeljMiningPickup] = -1;
    ZeljPlayerData[playerid][zeljContainerObj] = -1;
    ZeljPlayerData[playerid][zeljClickCount] = 0;
    ZeljPlayerData[playerid][zeljWaitingClick] = false;
    ZeljPlayerData[playerid][zeljLastClickTime] = 0;

    Zelj_ResetActiveCPs(playerid);

    // Kreiraj TDs
    Zelj_CreatePlayerTDs(playerid);
    return 1;
}

stock Zeljezara_OnDisconnect(playerid) {
    // Ubij sve timere
    if(ZeljPlayerData[playerid][zeljTimerKopanje] != -1) {
        KillTimer(ZeljPlayerData[playerid][zeljTimerKopanje]);
        ZeljPlayerData[playerid][zeljTimerKopanje] = -1;
    }
    if(ZeljPlayerData[playerid][zeljTimerPrerada] != -1) {
        KillTimer(ZeljPlayerData[playerid][zeljTimerPrerada]);
        ZeljPlayerData[playerid][zeljTimerPrerada] = -1;
    }
    if(ZeljPlayerData[playerid][zeljTimerTraka] != -1) {
        KillTimer(ZeljPlayerData[playerid][zeljTimerTraka]);
        ZeljPlayerData[playerid][zeljTimerTraka] = -1;
    }

    // Uništi traka objekat ako postoji
    if(ZeljPlayerData[playerid][zeljTrakaObj] != -1 && IsValidObject(ZeljPlayerData[playerid][zeljTrakaObj])) {
        DestroyObject(ZeljPlayerData[playerid][zeljTrakaObj]);
        ZeljPlayerData[playerid][zeljTrakaObj] = -1;
    }

    // Sakrij TDs
    Zelj_HideProgress(playerid);

    // Disable checkpoint
    DisablePlayerCheckpoint(playerid);

    // Uništi mining pickup ako postoji
    if(ZeljPlayerData[playerid][zeljMiningPickup] != -1) {
        DestroyDynamicPickup(ZeljPlayerData[playerid][zeljMiningPickup]);
        ZeljPlayerData[playerid][zeljMiningPickup] = -1;
    }

    // Uništi mining 3D label ako postoji
    if(ZeljMiningLabel[playerid] != Text3D:-1) {
        DestroyDynamic3DTextLabel(ZeljMiningLabel[playerid]);
        ZeljMiningLabel[playerid] = Text3D:-1;
    }

    // Uništi zeljezni objekat ako postoji
    if(ZeljPlayerData[playerid][zeljContainerObj] != -1) {
        DestroyDynamicObject(ZeljPlayerData[playerid][zeljContainerObj]);
        ZeljPlayerData[playerid][zeljContainerObj] = -1;
    }

    // Reset stanja
    ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_NONE;
    ZeljPlayerData[playerid][zeljHasEquipment] = false;
    // Ukloni PVar za anti-cheat
    DeletePVar(playerid, "ZeljezaraMining");
    return 1;
}

stock Zeljezara_OnSpawn(playerid) {
    return 1;
}

stock Zeljezara_OnDeath(playerid) {
    // Prekini sve ako igrac umre
    if(ZeljPlayerData[playerid][zeljState] != ZELJEZARA_STATE_NONE) {
        Zelj_StopMining(playerid);
        ZeljGRESKA(playerid, "Prekinuli ste rad u zeljezari jer ste umrli.");
    }
    return 1;
}

// ===========================================================================
//  GLAVNE FUNKCIJE SISTEMA
// ===========================================================================

// Reset i stop mining
stock Zelj_StopMining(playerid) {
    // Ubij timere
    if(ZeljPlayerData[playerid][zeljTimerKopanje] != -1) {
        KillTimer(ZeljPlayerData[playerid][zeljTimerKopanje]);
        ZeljPlayerData[playerid][zeljTimerKopanje] = -1;
    }
    if(ZeljPlayerData[playerid][zeljTimerPrerada] != -1) {
        KillTimer(ZeljPlayerData[playerid][zeljTimerPrerada]);
        ZeljPlayerData[playerid][zeljTimerPrerada] = -1;
    }
    // FIX: Ubij i prerada progress timer
    new preradaProgressTimer = GetPVarInt(playerid, "zeljPreradaProgressTimer");
    if(preradaProgressTimer > 0) KillTimer(preradaProgressTimer);
    DeletePVar(playerid, "zeljPreradaProgress");
    DeletePVar(playerid, "zeljPreradaProgressTimer");
    if(ZeljPlayerData[playerid][zeljTimerTraka] != -1) {
        KillTimer(ZeljPlayerData[playerid][zeljTimerTraka]);
        ZeljPlayerData[playerid][zeljTimerTraka] = -1;
    }

    // Uništi objekat na traci
    if(ZeljPlayerData[playerid][zeljTrakaObj] != -1 && IsValidObject(ZeljPlayerData[playerid][zeljTrakaObj])) {
        DestroyObject(ZeljPlayerData[playerid][zeljTrakaObj]);
        ZeljPlayerData[playerid][zeljTrakaObj] = -1;
    }

    // Uništi mining pickup ako postoji
    if(ZeljPlayerData[playerid][zeljMiningPickup] != -1) {
        DestroyDynamicPickup(ZeljPlayerData[playerid][zeljMiningPickup]);
        ZeljPlayerData[playerid][zeljMiningPickup] = -1;
    }

    // Uništi mining 3D label ako postoji
    if(ZeljMiningLabel[playerid] != Text3D:-1) {
        DestroyDynamic3DTextLabel(ZeljMiningLabel[playerid]);
        ZeljMiningLabel[playerid] = Text3D:-1;
    }

    // Uništi zeljezni objekat ako postoji
    if(ZeljPlayerData[playerid][zeljContainerObj] != -1) {
        DestroyDynamicObject(ZeljPlayerData[playerid][zeljContainerObj]);
        ZeljPlayerData[playerid][zeljContainerObj] = -1;
    }

    // Sakrij TDs
    Zelj_HideProgress(playerid);

    // Disable checkpoint
    DisablePlayerCheckpoint(playerid);

    // Vrati skin
    if(ZeljPlayerData[playerid][zeljHasEquipment] && ZeljPlayerData[playerid][zeljOldSkin] != -1) {
        SetPlayerSkin(playerid, ZeljPlayerData[playerid][zeljOldSkin]);
    }

    // Skini alatku za kopanje - koristi anti-cheat safe funkciju
    ResetAllWeapons(playerid);
    // Ukloni PVar - vise ne kopa, anti-cheat moze ponovo detektovati weapon hack
    DeletePVar(playerid, "ZeljezaraMining");

    // Unfreeze ako je zamrznut
    if(ZeljPlayerData[playerid][zeljDigging]) {
        TogglePlayerControllable(playerid, true);
        ZeljPlayerData[playerid][zeljDigging] = false;
    }

    // Clear animacije
    ClearAnimations(playerid);

    // Reset stanja
    ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_NONE;
    ZeljPlayerData[playerid][zeljHasEquipment] = false;
    ZeljPlayerData[playerid][zeljCheckpointIdx] = -1;
    ZeljPlayerData[playerid][zeljCheckpointsTotal] = 0;
    ZeljPlayerData[playerid][zeljCheckpointsDone] = 0;
    ZeljPlayerData[playerid][zeljRawMetal] = 0;
    ZeljPlayerData[playerid][zeljOldSkin] = -1;

    ZeljPlayerData[playerid][zeljAbuseFlag] = false;
    ZeljPlayerData[playerid][zeljClickCount] = 0;
    ZeljPlayerData[playerid][zeljWaitingClick] = false;
    ZeljPlayerData[playerid][zeljLastClickTime] = 0;

    Zelj_ResetActiveCPs(playerid);
}

// Uzimanje opreme
stock Zelj_UzmiOpremu(playerid) {
    // Provjere
    if(ZeljPlayerData[playerid][zeljHasEquipment]) {
        return ZeljGRESKA(playerid, "Vec imate opremu! Kucaj /kopajmetal da pocnes.");
    }
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_COOLDOWN) {
        new preostalo = (ZeljPlayerData[playerid][zeljCooldownTime] - GetTickCount()) / 1000;
        if(preostalo > 0) {
            new str[128];
            format(str, sizeof(str), "Morate sacekati jos %d sekundi prije nego sto ponovo mozete raditi.", preostalo);
            return ZeljGRESKA(playerid, str);
        }
        ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_NONE;
    }
    if(ZeljPlayerData[playerid][zeljState] != ZELJEZARA_STATE_NONE) {
        return ZeljGRESKA(playerid, "Vec ste u procesu rada u zeljezari.");
    }

    // Provjera udaljenosti
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    if(ZeljDist(x, y, z, ZeljEquipmentPos[0], ZeljEquipmentPos[1], ZeljEquipmentPos[2]) > ZELJEZARA_OPREMA_DIST) {
        return ZeljGRESKA(playerid, "Niste na lokaciji za uzimanje opreme.");
    }

    // Provjera da li je u vozilu
    if(IsPlayerInAnyVehicle(playerid)) {
        return ZeljGRESKA(playerid, "Ne mozete uzeti opremu dok ste u vozilu.");
    }

    // Postavi opremu
    ZeljPlayerData[playerid][zeljOldSkin] = GetPlayerSkin(playerid);
    SetPlayerSkin(playerid, ZELJEZARA_OPREMA_SKIN);
    ZeljPlayerData[playerid][zeljHasEquipment] = true;
    ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_EQUIPPED;
    ZeljPlayerData[playerid][zeljRawMetal] = 0;

    // Postavi PVar da anti-cheat zna da igrac radi u zeljezari (ne kickaj za weapon hack)
    SetPVarInt(playerid, "ZeljezaraMining", 1);

    // Zvuk i poruka
    PlayerPlaySound(playerid, ZELJEZARA_SOUND_OPREMA, 0.0, 0.0, 0.0);
    ZeljMSG(playerid, "{00BFFF}[ZELEZARA]{FFFFFF} Uzeli ste opremu za rad u zeljezari!");
    ZeljINFO(playerid, "Kucaj /kopajmetal da pocnete kopati sirovi metal.");
    ZeljINFO(playerid, "Slijedite plave pickupove do mjesta kopanja!");
    ZeljINFO(playerid, "Kucaj /prekidazeljezaru da prestanete raditi i vratite opremu.");
    return 1;
}

// Pokretanje kopanja
stock Zelj_PocniKopanje(playerid) {
    // Provjere
    if(!ZeljPlayerData[playerid][zeljHasEquipment]) {
        return ZeljGRESKA(playerid, "Nemate opremu! Idite na lokaciju i kucajte /uzmiopremu.");
    }
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_MINING) {
        return ZeljGRESKA(playerid, "Vec kopate metal!");
    }
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_PROCESSING || ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_CONVEYOR) {
        return ZeljGRESKA(playerid, "Vec prerate metal!");
    }
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_COOLDOWN) {
        new preostalo = (ZeljPlayerData[playerid][zeljCooldownTime] - GetTickCount()) / 1000;
        if(preostalo > 0) {
            new str[128];
            format(str, sizeof(str), "Morate sacekati jos %d sekundi prije nego sto ponovo mozete raditi.", preostalo);
            return ZeljGRESKA(playerid, str);
        }
        ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_EQUIPPED;
    }
    if(ZeljPlayerData[playerid][zeljState] != ZELJEZARA_STATE_EQUIPPED) {
        return ZeljGRESKA(playerid, "Ne mozete sada poceti kopati.");
    }
    if(ZeljPlayerData[playerid][zeljRawMetal] >= ZELJEZARA_MAX_RAW_METAL) {
        return ZeljINFO(playerid, "Vec imate maksimalnu kolicinu sirovog metala! Idite na preradu (/preradi).");
    }
    if(IsPlayerInAnyVehicle(playerid)) {
        return ZeljGRESKA(playerid, "Ne mozete kopati dok ste u vozilu.");
    }

    // Generisi random broj checkpointa
    new totalCP = ZeljRandomRange(ZELJEZARA_MIN_CHECKPOINTS, ZELJEZARA_MAX_CHECKPOINTS);
    ZeljPlayerData[playerid][zeljCheckpointsTotal] = totalCP;
    ZeljPlayerData[playerid][zeljCheckpointsDone] = 0;
    ZeljPlayerData[playerid][zeljCheckpointIdx] = 0;

    // Nasumicni odabir koordinata - random shuffle
    new indices[24];
    for(new i = 0; i < 24; i++) indices[i] = i;
    
    // Fisher-Yates shuffle
    for(new i = 23; i > 0; i--) {
        new j = random(i + 1);
        new tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }

    // Postavi aktivne CP nasumicno
    for(new i = 0; i < totalCP; i++) {
        Zelj_SetActiveCP(playerid, i, indices[i]);
    }

    ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_MINING;

    // Postavi prvi checkpoint
    Zelj_SetNextCheckpoint(playerid);

    // Poruka i zvuk
    new str[128];
    format(str, sizeof(str), "{00BFFF}[ZELEZARA]{FFFFFF} Poceli ste kopanje! Slijedite plave pickupove (%d ukupno).", totalCP);
    ZeljMSG(playerid, str);
    Zelj_UpdateRawMetalTD(playerid);
    return 1;
}

// Postavljanje sljedeceg checkpointa
stock Zelj_SetNextCheckpoint(playerid) {
    new idx = ZeljPlayerData[playerid][zeljCheckpointIdx];
    if(idx >= ZeljPlayerData[playerid][zeljCheckpointsTotal]) {
        // Svi checkpointi zavrseni - idi na preradu
        ZeljMSG(playerid, "{00BFFF}[ZELEZARA]{FFFFFF} Svi checkpointovi zavrseni!");
        ZeljMSG(playerid, "{FFD700}[ZELEZARA]{FFFFFF} Idite u halu za preradu i kucajte /preradi");
        ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_EQUIPPED;
        DisablePlayerCheckpoint(playerid);
        Zelj_HideProgress(playerid);

        // Uništi zeljezni objekat - zavrseno kopanje
        if(ZeljPlayerData[playerid][zeljContainerObj] != -1) {
            DestroyDynamicObject(ZeljPlayerData[playerid][zeljContainerObj]);
            ZeljPlayerData[playerid][zeljContainerObj] = -1;
        }

        // Postavi checkpoint ka hali za preradu
        SetPlayerCheckpoint(playerid, ZeljProcessPos[0][0], ZeljProcessPos[0][1], ZeljProcessPos[0][2], 4.0);
        return 1;
    }

    new cpIdx = Zelj_GetActiveCP(playerid, idx);

    // Uništi prethodni mining pickup ako postoji
    if(ZeljPlayerData[playerid][zeljMiningPickup] != -1) {
        DestroyDynamicPickup(ZeljPlayerData[playerid][zeljMiningPickup]);
        ZeljPlayerData[playerid][zeljMiningPickup] = -1;
    }

    // Uništi prethodni 3D label ako postoji
    if(ZeljMiningLabel[playerid] != Text3D:-1) {
        DestroyDynamic3DTextLabel(ZeljMiningLabel[playerid]);
        ZeljMiningLabel[playerid] = Text3D:-1;
    }

    // Uništi prethodni zeljezni objekat ako postoji (prije kreiranja novog)
    if(ZeljPlayerData[playerid][zeljContainerObj] != -1) {
        DestroyDynamicObject(ZeljPlayerData[playerid][zeljContainerObj]);
        ZeljPlayerData[playerid][zeljContainerObj] = -1;
    }

    // Kreiraj zeljezni objekat (19846) na trenutnom checkpointu - pojavljuje se jedan po jedan
    // Koristimo model 19846 (zeljezni panel/lim) - vidljiv samo igracu koji kopa
    // Pozicija i rotacija su IZVORNE iz mape - objekat mora biti tacno gdje je na mapi!
    ZeljPlayerData[playerid][zeljContainerObj] = CreateDynamicObject(ZELJEZARA_MODEL_METAL_BELT,
        ZeljMiningCoords[cpIdx][0],
        ZeljMiningCoords[cpIdx][1],
        ZeljMiningCoords[cpIdx][2],  // Tacna Z pozicija iz mape - na podu, ne u zraku!
        ZeljMiningRot[cpIdx][0],      // rx - originalna rotacija iz mape
        ZeljMiningRot[cpIdx][1],      // ry - originalna rotacija iz mape
        ZeljMiningRot[cpIdx][2],      // rz - originalna rotacija iz mape
        -1, -1, playerid, 300.0, 300.0);  // Samo za ovog igraca

    // Kreiraj plavi pickup (19607) na mining lokaciji - VIZUELNI MARKER
    // Koristimo type 23 (pickup se vidi ali ne ulazi u inventory)
    // Z + 0.3 - pickup tik iznad objekta, igrac mora biti blizu da kopa
    ZeljPlayerData[playerid][zeljMiningPickup] = CreateDynamicPickup(
        ZELJEZARA_MODEL_MINING_CP, 23,
        ZeljMiningCoords[cpIdx][0],
        ZeljMiningCoords[cpIdx][1],
        ZeljMiningCoords[cpIdx][2] + 0.3,
        -1, -1, playerid);  // Samo za ovog igraca

    // 3D Text label iznad pickupa - jasna oznaka gdje se kopa
    new cpStr[64];
    format(cpStr, sizeof(cpStr), "{FFD700}[ KOPAJ OVDJE ]{FFFFFF}\nCheckpoint %d/%d", idx + 1, ZeljPlayerData[playerid][zeljCheckpointsTotal]);
    ZeljMiningLabel[playerid] = CreateDynamic3DTextLabel(cpStr, 0xFFFFFFFF,
        ZeljMiningCoords[cpIdx][0],
        ZeljMiningCoords[cpIdx][1],
        ZeljMiningCoords[cpIdx][2] + 1.5,
        15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);

    // NEMA crvenog checkpointa - plavi pickup je dovoljan za navigaciju
    // Auto-trigger u Zeljezara_OnUpdate ce pokrenuti kopanje kad igrac dodje do pickupa

    new str[128];
    format(str, sizeof(str), "Checkpoint %d/%d", idx + 1, ZeljPlayerData[playerid][zeljCheckpointsTotal]);
    Zelj_ShowProgress(playerid, 0.0, str);
    return 1;
}

// Kopanje na checkpointu - poziva se kad igrac dodje do mining lokacije
stock Zelj_OnCheckpointEnter(playerid) {
    if(ZeljPlayerData[playerid][zeljState] != ZELJEZARA_STATE_MINING) return 0;
    
    // FIX: Sprijeci dupli trigger - ako vec kopa/ceka klik, ne pokreci ponovo
    if(ZeljPlayerData[playerid][zeljDigging]) return 0;
    
    // Anti-abuse: provjera vremena
    new tick = GetTickCount();
    if(ZeljPlayerData[playerid][zeljLastCPTime] != 0 && (tick - ZeljPlayerData[playerid][zeljLastCPTime]) < 2000) {
        ZeljPlayerData[playerid][zeljAbuseWarnings]++;
        if(ZeljPlayerData[playerid][zeljAbuseWarnings] >= 3) {
            Zelj_StopMining(playerid);
            ZeljGRESKA(playerid, "Prekinuti ste zbog sumnje na abuse (previse brzo).");
            return 1;
        }
        ZeljGRESKA(playerid, "Sporije! Pricekajte malo prije sljedeceg checkpointa.");
    }
    ZeljPlayerData[playerid][zeljLastCPTime] = tick;

    // Provjera udaljenosti (da ne moze teleportovati)
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);
    new idx = ZeljPlayerData[playerid][zeljCheckpointIdx];
    new cpIdx = Zelj_GetActiveCP(playerid, idx);
    new Float:dist = ZeljDist(px, py, pz, ZeljMiningCoords[cpIdx][0], ZeljMiningCoords[cpIdx][1], ZeljMiningCoords[cpIdx][2]);
    
    if(dist > 8.0) {
        ZeljPlayerData[playerid][zeljAbuseFlag] = true;
        ZeljGRESKA(playerid, "Predaleko ste od checkpointa! Priblizite se.");
        return 1;
    }

    // Provjera vozila
    if(IsPlayerInAnyVehicle(playerid)) {
        return ZeljGRESKA(playerid, "Ne mozete kopati iz vozila!");
    }

    // Provjera da li je u zeljezari zoni
    if(!ZeljIsInArea(playerid)) {
        Zelj_StopMining(playerid);
        return ZeljGRESKA(playerid, "Napustili ste zonu zeljezare! Rad prekinut.");
    }

    // NE freezaj igraca! TogglePlayerControllable(false) BLOKIRA KEY_FIRE evente!
    // Igrac mora moci klikati (LMB) da bi kopao. Umjesto toga, sprijecavamo kretanje
    // tako sto cuvamo poziciju i vracamo igraca ako odmakne (u OnUpdate)
    GetPlayerPos(playerid, ZeljPlayerData[playerid][zeljTrakaX], ZeljPlayerData[playerid][zeljTrakaY], ZeljPlayerData[playerid][zeljTrakaZ]);
    ZeljPlayerData[playerid][zeljDigging] = true;

    // Postavi mode: ceka klik od igraca
    ZeljPlayerData[playerid][zeljClickCount] = 0;
    ZeljPlayerData[playerid][zeljWaitingClick] = true;
    ZeljPlayerData[playerid][zeljLastClickTime] = 0;  // Reset cooldown za novi CP

    // Uništi mining pickup - kopa se, marker vise nije potreban
    if(ZeljPlayerData[playerid][zeljMiningPickup] != -1) {
        DestroyDynamicPickup(ZeljPlayerData[playerid][zeljMiningPickup]);
        ZeljPlayerData[playerid][zeljMiningPickup] = -1;
    }

    // Uništi 3D label - kopa se
    if(ZeljMiningLabel[playerid] != Text3D:-1) {
        DestroyDynamic3DTextLabel(ZeljMiningLabel[playerid]);
        ZeljMiningLabel[playerid] = Text3D:-1;
    }

    // Daj alatku za kopanje (shovel) u ruke - vizuelni efekt
    // MORA biti GiveWeaponToPlayer (sa anti-cheat tracking), inace biva kick za weapon hack!
    GiveWeaponToPlayer(playerid, ZELJEZARA_WEAPON_KOPANJE, 1);
    // Postavi PVar da anti-cheat zna da igrac kopa u zeljezari (ne kickaj za weapon hack)
    SetPVarInt(playerid, "ZeljezaraMining", 1);

    // Preload animaciju biblioteke - MORA prvo null pa onda animacija
    ApplyAnimation(playerid, "BASEBALL", "null", 4.1, 0, 0, 0, 0, 0); // Preload

    // Pokazi poruku i progress bar - ceka klik
    new str[128];
    format(str, sizeof(str), "Kliknite da kopate! (0/%d)", ZELJEZARA_CLICKS_PER_CP);
    Zelj_ShowProgress(playerid, 0.0, str);
    ZeljINFO(playerid, "Stisnite LMB (lijevi klik) da kopate!");

    return 1;
}

// OnPlayerKeyStateChange handler - klik za kopanje
stock Zeljezara_OnKeyStateChange(playerid, newkeys, oldkeys) {
    // ========================================================
    // ALT tipka (KEY_WALK) za uzimanje/skidanje opreme
    // ========================================================
    if((newkeys & KEY_WALK) && !(oldkeys & KEY_WALK)) {
        if(IsPlayerInAnyVehicle(playerid)) return 0;

        // Ako igrac NEMA opremu - probaj uzeti opremu
        if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_NONE && !ZeljPlayerData[playerid][zeljHasEquipment]) {
            new Float:x, Float:y, Float:z;
            GetPlayerPos(playerid, x, y, z);
            if(ZeljDist(x, y, z, ZeljEquipmentPos[0], ZeljEquipmentPos[1], ZeljEquipmentPos[2]) <= ZELJEZARA_OPREMA_DIST) {
                Zelj_UzmiOpremu(playerid);
                return 1;
            }
        }

        // Ako igrac IMA opremu ali ne kopa/preradjuje - skini opremu
        if(ZeljPlayerData[playerid][zeljHasEquipment] && 
           (ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_EQUIPPED || ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_COOLDOWN)) {
            new Float:x, Float:y, Float:z;
            GetPlayerPos(playerid, x, y, z);
            if(ZeljDist(x, y, z, ZeljEquipmentPos[0], ZeljEquipmentPos[1], ZeljEquipmentPos[2]) <= ZELJEZARA_OPREMA_DIST) {
                Zelj_Prekini(playerid);
                return 1;
            }
        }
    }

    // ========================================================
    // KEY_FIRE (LMB) za kopanje - klik sistema
    // ========================================================
    if(!ZeljPlayerData[playerid][zeljWaitingClick]) return 0;
    if(ZeljPlayerData[playerid][zeljState] != ZELJEZARA_STATE_MINING) return 0;
    
    // Detektuj KEY_FIRE (LMB / lijevi klik) - novi pritisak, ne drzanje
    if((newkeys & KEY_FIRE) && !(oldkeys & KEY_FIRE)) {
        // Cooldown provjera - sprijeci previse brzo klikanje
        new tick = GetTickCount();
        if(ZeljPlayerData[playerid][zeljLastClickTime] != 0 && (tick - ZeljPlayerData[playerid][zeljLastClickTime]) < ZELJEZARA_CLICK_COOLDOWN) {
            return 1;  // Tiho ignorisi - ne spamaj poruke
        }
        ZeljPlayerData[playerid][zeljLastClickTime] = tick;
        
        // Increment click counter
        ZeljPlayerData[playerid][zeljClickCount]++;
        new clicks = ZeljPlayerData[playerid][zeljClickCount];
        
        // Izracunaj progress
        new Float:percent = (float(clicks) / float(ZELJEZARA_CLICKS_PER_CP)) * 100.0;
        if(percent > 100.0) percent = 100.0;
        
        // Pokreni kratku animaciju kopanja - svaki klik = jedan zamah
        // BASEBALL Bat_1 = kratki zamah, duration 4.1ms, ne loop, lockX/Y za smooth, freeze=0
        ApplyAnimation(playerid, ZELJEZARA_ANIM_KOPANJE_LIB, ZELJEZARA_ANIM_KOPANJE_NAME, 4.1, 0, 1, 1, 0, 0, 1);
        PlayerPlaySound(playerid, ZELJEZARA_SOUND_KOPANJE, 0.0, 0.0, 0.0);
        
        // Azuriraj progress bar
        new str[128];
        if(clicks >= ZELJEZARA_CLICKS_PER_CP) {
            // Svi klikovi gotovi - checkpoint zavrsen!
            format(str, sizeof(str), "KOPANJE ZAVRSENO! %d/%d", clicks, ZELJEZARA_CLICKS_PER_CP);
            Zelj_ShowProgress(playerid, 100.0, str);
            
            // Ne ceka vise klik
            ZeljPlayerData[playerid][zeljWaitingClick] = false;
            
            // Izvrsi zavrsetak kopanja (nakon kratke pauze da animacija zavrsi)
            ZeljPlayerData[playerid][zeljTimerKopanje] = SetTimerEx("ZeljTimer_Kopanje", 800, false, "i", playerid);
        } else {
            // Jos klikova treba - azuriraj progress
            format(str, sizeof(str), "Kopanje... (%d/%d)", clicks, ZELJEZARA_CLICKS_PER_CP);
            Zelj_ShowProgress(playerid, percent, str);
        }
        
        return 1;
    }
    
    return 0;
}

// Glavni timer za kopanje - poziva se nakon sto igrac klikne 3 puta
public ZeljTimer_Kopanje(playerid) {
    if(!ZeljValidPlayer(playerid) || ZeljPlayerData[playerid][zeljState] != ZELJEZARA_STATE_MINING) {
        return 0;
    }

    ZeljPlayerData[playerid][zeljTimerKopanje] = -1;

    // Reset click counter za sljedeci CP
    ZeljPlayerData[playerid][zeljClickCount] = 0;
    ZeljPlayerData[playerid][zeljWaitingClick] = false;
    ZeljPlayerData[playerid][zeljLastClickTime] = 0;

    // Nasumicna kolicina sirovog metala
    new metal = ZeljRandomRange(ZELJEZARA_MIN_METAL, ZELJEZARA_MAX_METAL);
    
    // Provjera max
    if(ZeljPlayerData[playerid][zeljRawMetal] + metal > ZELJEZARA_MAX_RAW_METAL) {
        metal = ZELJEZARA_MAX_RAW_METAL - ZeljPlayerData[playerid][zeljRawMetal];
    }

    ZeljPlayerData[playerid][zeljRawMetal] += metal;
    ZeljPlayerData[playerid][zeljCheckpointsDone]++;
    ZeljPlayerData[playerid][zeljCheckpointIdx]++;

    // Dodaj sirovi metal u inventory
    ZeljInv_AddRaw(playerid, metal);

    new str[128];
    format(str, sizeof(str), "{00BFFF}[ZELEZARA]{FFFFFF} Iskopali ste %d sirovog metala! (Ukupno: %d)", metal, ZeljPlayerData[playerid][zeljRawMetal]);
    ZeljMSG(playerid, str);
    PlayerPlaySound(playerid, ZELJEZARA_SOUND_GOTOVO, 0.0, 0.0, 0.0);

    Zelj_UpdateRawMetalTD(playerid);

    // Skini alatku za kopanje iz ruku - anti-cheat safe
    ResetAllWeapons(playerid);
    // Ukloni PVar - vise ne kopa, anti-cheat moze ponovo detektovati weapon hack
    DeletePVar(playerid, "ZeljezaraMining");

    // Kopanje zavrseno - vise ne kopa
    ZeljPlayerData[playerid][zeljDigging] = false;

    // Clear animacije
    ClearAnimations(playerid);

    // Sljedeci checkpoint
    Zelj_SetNextCheckpoint(playerid);
    return 1;
}

// Prerada metala
stock Zelj_PocniPreradu(playerid) {
    // Provjere
    if(!ZeljPlayerData[playerid][zeljHasEquipment]) {
        return ZeljGRESKA(playerid, "Nemate opremu!");
    }
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_PROCESSING || ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_CONVEYOR) {
        return ZeljGRESKA(playerid, "Vec prerate metal!");
    }
    if(ZeljPlayerData[playerid][zeljRawMetal] <= 0 && ZeljInv_CountRaw(playerid) <= 0) {
        return ZeljGRESKA(playerid, "Nemate sirovog metala za preradu! Prvo kopajte (/kopajmetal).");
    }
    if(IsPlayerInAnyVehicle(playerid)) {
        return ZeljGRESKA(playerid, "Ne mozete preradjivati metal dok ste u vozilu.");
    }

    // Provjera udaljenosti od masine
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    new Float:dist1 = ZeljDist(x, y, z, ZeljProcessPos[0][0], ZeljProcessPos[0][1], ZeljProcessPos[0][2]);
    new Float:dist2 = ZeljDist(x, y, z, ZeljProcessPos[1][0], ZeljProcessPos[1][1], ZeljProcessPos[1][2]);
    
    if(dist1 > ZELJEZARA_PRERADA_DIST && dist2 > ZELJEZARA_PRERADA_DIST) {
        return ZeljGRESKA(playerid, "Niste pored masine za preradu! Idite u halu za preradu.");
    }

    // Sinhronizuj sirovi metal iz inventoryja
    new invRaw = ZeljInv_CountRaw(playerid);
    if(ZeljPlayerData[playerid][zeljRawMetal] <= 0 && invRaw > 0) {
        ZeljPlayerData[playerid][zeljRawMetal] = invRaw;
    }

    if(ZeljPlayerData[playerid][zeljRawMetal] <= 0) {
        return ZeljGRESKA(playerid, "Nemate sirovog metala za preradu!");
    }

    // Zapocni preradu
    ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_PROCESSING;
    DisablePlayerCheckpoint(playerid);

    // Odredi koju traku koristiti (0 ili 1)
    if(dist1 <= dist2) {
        ZeljPlayerData[playerid][zeljTrakaIdx] = 0;
    } else {
        ZeljPlayerData[playerid][zeljTrakaIdx] = 1;
    }
    new trakaIdx = ZeljPlayerData[playerid][zeljTrakaIdx];

    // NE freezaj igraca - moze se kretati dok metal putuje masinom
    // Igrac mora sam doci do kraja trake da pokupi preradjeni metal

    // Pokreni zvuk masine
    PlayerPlaySound(playerid, ZELJEZARA_SOUND_STROJ, 0.0, 0.0, 0.0);

    // Kreiraj objekat metala (19846) na pocetku trake - centriran po sredini trake
    // Objekat lezi ravno na traci: rx=0, ry=0, rz=0 (default flat orijentacija modela 19846)
    // rx=-90 je STAVLJAO objekat VERTIKALNO na ivicu - zato je ispadao iz trake!
    ZeljPlayerData[playerid][zeljTrakaObj] = CreateObject(ZELJEZARA_MODEL_METAL_BELT, 
        ZeljConveyorPath[trakaIdx][0][0], ZeljConveyorPath[trakaIdx][0][1], ZeljConveyorPath[trakaIdx][0][2], 
        0.0, 0.0, 0.0);

    new str[128];
    format(str, sizeof(str), "{00BFFF}[ZELEZARA]{FFFFFF} Ubacujete %d sirovog metala u masinu...", ZeljPlayerData[playerid][zeljRawMetal]);
    ZeljMSG(playerid, str);
    ZeljMSG(playerid, "{FFD700}[ZELEZARA]{FFFFFF} Metal se preradjuje na traci. Sacekajte da stigne do kraja!");

    Zelj_ShowProgress(playerid, 0.0, "PRERADA metala...");
    
    // Pokreni traku animaciju - koristimo MoveObject za smooth kretanje
    // Pomjeri metal od pocetka do kraja trake - bez Z offseta, lezi ravno na traci
    if(ZeljPlayerData[playerid][zeljTrakaObj] != -1 && IsValidObject(ZeljPlayerData[playerid][zeljTrakaObj])) {
        MoveObject(ZeljPlayerData[playerid][zeljTrakaObj], 
            ZeljConveyorPath[trakaIdx][1][0], ZeljConveyorPath[trakaIdx][1][1], ZeljConveyorPath[trakaIdx][1][2],
            0.5);  // Brzina kretanja
    }

    // Azuriraj poziciju za timer
    ZeljPlayerData[playerid][zeljTrakaStep] = 0;
    ZeljPlayerData[playerid][zeljTrakaX] = ZeljConveyorPath[trakaIdx][0][0];
    ZeljPlayerData[playerid][zeljTrakaY] = ZeljConveyorPath[trakaIdx][0][1];
    ZeljPlayerData[playerid][zeljTrakaZ] = ZeljConveyorPath[trakaIdx][0][2];
    ZeljPlayerData[playerid][zeljTrakaTX] = ZeljConveyorPath[trakaIdx][1][0];
    ZeljPlayerData[playerid][zeljTrakaTY] = ZeljConveyorPath[trakaIdx][1][1];
    ZeljPlayerData[playerid][zeljTrakaTZ] = ZeljConveyorPath[trakaIdx][1][2];

    ZeljPlayerData[playerid][zeljTimerTraka] = SetTimerEx("ZeljTimer_Traka", 500, true, "i", playerid);

    // Pokreni prerada timer
    ZeljPlayerData[playerid][zeljTimerPrerada] = SetTimerEx("ZeljTimer_Prerada", ZELJEZARA_PRERADA_VRIJEME * 1000, false, "i", playerid);

    // Progress bar update - cuvamo timer ID
    SetPVarInt(playerid, "zeljPreradaProgress", 0);
    new preradaPTimer = SetTimerEx("ZeljTimer_PreradaProgress", 1000, true, "i", playerid);
    SetPVarInt(playerid, "zeljPreradaProgressTimer", preradaPTimer);
    return 1;
}

// Timer za progress bar prerade
forward ZeljTimer_PreradaProgress(playerid);
public ZeljTimer_PreradaProgress(playerid) {
    if(ZeljPlayerData[playerid][zeljState] != ZELJEZARA_STATE_PROCESSING && ZeljPlayerData[playerid][zeljState] != ZELJEZARA_STATE_CONVEYOR) {
        // FIX: Ubij repeating timer
        new preradaPTimer = GetPVarInt(playerid, "zeljPreradaProgressTimer");
        if(preradaPTimer > 0) KillTimer(preradaPTimer);
        DeletePVar(playerid, "zeljPreradaProgress");
        DeletePVar(playerid, "zeljPreradaProgressTimer");
        return 0;
    }
    
    new progress = GetPVarInt(playerid, "zeljPreradaProgress") + 1;
    SetPVarInt(playerid, "zeljPreradaProgress", progress);
    
    new Float:percent = (float(progress) / float(ZELJEZARA_PRERADA_VRIJEME)) * 100.0;
    if(percent > 100.0) percent = 100.0;
    
    new str[64];
    format(str, sizeof(str), "PRERADA metala... %d%%", floatround(percent));
    Zelj_ShowProgress(playerid, percent, str);

    // Zvuk masine periodicno
    if(progress % 3 == 0) {
        PlayerPlaySound(playerid, ZELJEZARA_SOUND_PRERADA, 0.0, 0.0, 0.0);
    }

    if(progress >= ZELJEZARA_PRERADA_VRIJEME) {
        // FIX: Ubij repeating timer kad je gotovo
        new preradaPTimer = GetPVarInt(playerid, "zeljPreradaProgressTimer");
        if(preradaPTimer > 0) KillTimer(preradaPTimer);
        DeletePVar(playerid, "zeljPreradaProgress");
        DeletePVar(playerid, "zeljPreradaProgressTimer");
    }
    return 1;
}

// Timer za traku - periodicni check da li je objekat stigao na kraj
public ZeljTimer_Traka(playerid) {
    if(!ZeljValidPlayer(playerid) || (ZeljPlayerData[playerid][zeljState] != ZELJEZARA_STATE_PROCESSING && ZeljPlayerData[playerid][zeljState] != ZELJEZARA_STATE_CONVEYOR)) {
        if(ZeljPlayerData[playerid][zeljTimerTraka] != -1) {
            KillTimer(ZeljPlayerData[playerid][zeljTimerTraka]);
            ZeljPlayerData[playerid][zeljTimerTraka] = -1;
        }
        if(ZeljPlayerData[playerid][zeljTrakaObj] != -1 && IsValidObject(ZeljPlayerData[playerid][zeljTrakaObj])) {
            DestroyObject(ZeljPlayerData[playerid][zeljTrakaObj]);
            ZeljPlayerData[playerid][zeljTrakaObj] = -1;
        }
        return 0;
    }

    // Provjeri da li je objekat stigao blizu kraja trake
    new trakaIdx = ZeljPlayerData[playerid][zeljTrakaIdx];
    new Float:endX = ZeljConveyorPath[trakaIdx][1][0];
    new Float:endY = ZeljConveyorPath[trakaIdx][1][1];
    new Float:endZ = ZeljConveyorPath[trakaIdx][1][2];

    if(ZeljPlayerData[playerid][zeljTrakaObj] != -1 && IsValidObject(ZeljPlayerData[playerid][zeljTrakaObj])) {
        new Float:objX, Float:objY, Float:objZ;
        GetObjectPos(ZeljPlayerData[playerid][zeljTrakaObj], objX, objY, objZ);
        new Float:dist = ZeljDist(objX, objY, objZ, endX, endY, endZ);

        // Ako je blizu kraja, postavi CP za preuzimanje i promijeni stanje
        if(dist < 1.0) {
            // Ubij traka timer - metal je stigao
            if(ZeljPlayerData[playerid][zeljTimerTraka] != -1) {
                KillTimer(ZeljPlayerData[playerid][zeljTimerTraka]);
                ZeljPlayerData[playerid][zeljTimerTraka] = -1;
            }

            // Ubij prerada progress timer
            new preradaPTimer = GetPVarInt(playerid, "zeljPreradaProgressTimer");
            if(preradaPTimer > 0) KillTimer(preradaPTimer);
            DeletePVar(playerid, "zeljPreradaProgress");
            DeletePVar(playerid, "zeljPreradaProgressTimer");

            // Promijeni stanje - metal je na kraju trake, ceka se preuzimanje
            ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_CONVEYOR;

            // Postavi checkpoint na kraj trake za preuzimanje
            SetPlayerCheckpoint(playerid, ZeljPickupPos[trakaIdx][0], ZeljPickupPos[trakaIdx][1], ZeljPickupPos[trakaIdx][2], 2.5);

            ZeljINFO(playerid, "{00BFFF}[ZELEZARA]{FFFFFF} Metal je stigao na kraj trake! Dodjite do checkpointa da pokupite preradjeni metal.");
            Zelj_ShowProgress(playerid, 100.0, "PREUZMITE METAL!");
        }
    }

    return 1;
}

// Timer za preradu - safety timer, osigurava da metal stigne na kraj
// Ako traka timer vec nije detektovao kraj, ovo forsira prijelaz u CONVEYOR stanje
public ZeljTimer_Prerada(playerid) {
    if(!ZeljValidPlayer(playerid)) return 0;

    ZeljPlayerData[playerid][zeljTimerPrerada] = -1;

    // Ako je vec u CONVEYOR stanju, ne radi nista (igrac treba pokupiti metal)
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_CONVEYOR) return 1;

    // Ako je jos u PROCESSING stanju, metal mozda nije stigao - forsiraj prijelaz
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_PROCESSING) {
        // Ubij traka timer
        if(ZeljPlayerData[playerid][zeljTimerTraka] != -1) {
            KillTimer(ZeljPlayerData[playerid][zeljTimerTraka]);
            ZeljPlayerData[playerid][zeljTimerTraka] = -1;
        }

        // Ubij progress timer
        new preradaPTimer = GetPVarInt(playerid, "zeljPreradaProgressTimer");
        if(preradaPTimer > 0) KillTimer(preradaPTimer);
        DeletePVar(playerid, "zeljPreradaProgress");
        DeletePVar(playerid, "zeljPreradaProgressTimer");

        // Pomjeri objekat na kraj trake ako postoji
        new trakaIdx = ZeljPlayerData[playerid][zeljTrakaIdx];
        if(ZeljPlayerData[playerid][zeljTrakaObj] != -1 && IsValidObject(ZeljPlayerData[playerid][zeljTrakaObj])) {
            SetObjectPos(ZeljPlayerData[playerid][zeljTrakaObj],
                ZeljConveyorPath[trakaIdx][1][0],
                ZeljConveyorPath[trakaIdx][1][1],
                ZeljConveyorPath[trakaIdx][1][2]);
        }

        // Promijeni stanje u CONVEYOR - ceka se preuzimanje
        ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_CONVEYOR;

        // Postavi checkpoint na kraj trake za preuzimanje
        SetPlayerCheckpoint(playerid, ZeljPickupPos[trakaIdx][0], ZeljPickupPos[trakaIdx][1], ZeljPickupPos[trakaIdx][2], 2.5);

        ZeljINFO(playerid, "{00BFFF}[ZELEZARA]{FFFFFF} Metal je preradjen! Dodjite do checkpointa da pokupite preradjeni metal.");
        Zelj_ShowProgress(playerid, 100.0, "PREUZMITE METAL!");
    }

    return 1;
}

// Funkcija za davanje rezultata prerade - poziva se kad igrac pokupi metal
stock Zelj_GiveProcessingResults(playerid) {
    // Izracunaj rezultate prerade
    new rawMetal = ZeljPlayerData[playerid][zeljRawMetal];
    new metal = floatround(rawMetal * (ZELJEZARA_METAL_PRERADA / 100.0));
    new scrap = rawMetal - metal;

    // Ukloni sirovi metal iz inventoryja
    ZeljInv_RemoveRaw(playerid, rawMetal);

    // Dodaj preradjeni metal i scrap u inventory
    if(metal > 0) ZeljInv_AddMetal(playerid, metal);
    if(scrap > 0) ZeljInv_AddScrap(playerid, scrap);

    // Cleanup trake
    if(ZeljPlayerData[playerid][zeljTimerTraka] != -1) {
        KillTimer(ZeljPlayerData[playerid][zeljTimerTraka]);
        ZeljPlayerData[playerid][zeljTimerTraka] = -1;
    }
    if(ZeljPlayerData[playerid][zeljTrakaObj] != -1 && IsValidObject(ZeljPlayerData[playerid][zeljTrakaObj])) {
        DestroyObject(ZeljPlayerData[playerid][zeljTrakaObj]);
        ZeljPlayerData[playerid][zeljTrakaObj] = -1;
    }

    // Ubij prerada timer ako je jos aktivan
    if(ZeljPlayerData[playerid][zeljTimerPrerada] != -1) {
        KillTimer(ZeljPlayerData[playerid][zeljTimerPrerada]);
        ZeljPlayerData[playerid][zeljTimerPrerada] = -1;
    }

    // Ubij progress timer
    new preradaPTimer = GetPVarInt(playerid, "zeljPreradaProgressTimer");
    if(preradaPTimer > 0) KillTimer(preradaPTimer);
    DeletePVar(playerid, "zeljPreradaProgress");
    DeletePVar(playerid, "zeljPreradaProgressTimer");

    // Poruke
    new str[256];
    format(str, sizeof(str), "{00BFFF}[ZELEZARA]{FFFFFF} Uspjesno je preradjen metal! Od %d sirovog metala dobili ste:", rawMetal);
    ZeljMSG(playerid, str);
    format(str, sizeof(str), "{00FF00}Metal: %d | {FF6347}Metalni Otpad: %d", metal, scrap);
    ZeljMSG(playerid, str);
    ZeljMSG(playerid, "{FFD700}[ZELEZARA]{FFFFFF} Materijali su spremljeni u vas inventory (/inv).");

    PlayerPlaySound(playerid, ZELJEZARA_SOUND_GOTOVO, 0.0, 0.0, 0.0);

    // Postavi cooldown
    ZeljPlayerData[playerid][zeljRawMetal] = 0;
    ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_COOLDOWN;
    ZeljPlayerData[playerid][zeljCooldownTime] = GetTickCount() + (ZELJEZARA_COOLDOWN * 1000);

    // Vrati skin
    if(ZeljPlayerData[playerid][zeljOldSkin] != -1) {
        SetPlayerSkin(playerid, ZeljPlayerData[playerid][zeljOldSkin]);
    }
    ZeljPlayerData[playerid][zeljHasEquipment] = false;
    ClearAnimations(playerid);

    // Sakrij TDs
    Zelj_HideProgress(playerid);
    DisablePlayerCheckpoint(playerid);

    // Pokreni cooldown timer
    SetTimerEx("ZeljTimer_Cooldown", ZELJEZARA_COOLDOWN * 1000, false, "i", playerid);

    new cdStr[64];
    format(cdStr, sizeof(cdStr), "Cooldown: %d min", ZELJEZARA_COOLDOWN / 60);
    ZeljINFO(playerid, cdStr);

    return 1;
}

// Cooldown timer
public ZeljTimer_Cooldown(playerid) {
    if(!ZeljValidPlayer(playerid)) return 0;
    
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_COOLDOWN) {
        ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_NONE;
        ZeljMSG(playerid, "{00BFFF}[ZELEZARA]{FFFFFF} Cooldown zavrsen! Mozete ponovo raditi u zeljezari.");
        ZeljPlayerData[playerid][zeljCooldownTime] = 0;
    }
    return 1;
}

// Prekid rada
stock Zelj_Prekini(playerid) {
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_NONE && !ZeljPlayerData[playerid][zeljHasEquipment]) {
        return ZeljGRESKA(playerid, "Ne radite trenutno u zeljezari.");
    }

    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_PROCESSING) {
        return ZeljGRESKA(playerid, "Ne mozete prekinuti dok traje prerada! Sacekajte da se zavrsi.");
    }

    // Ako ima sirovog metala, sacuvaj ga u inventory
    if(ZeljPlayerData[playerid][zeljRawMetal] > 0) {
        ZeljMSG(playerid, "{FFD700}[ZELEZARA]{FFFFFF} Sirovi metal je sacuvan u vasem inventoryju.");
    }

    Zelj_StopMining(playerid);
    ZeljMSG(playerid, "{00BFFF}[ZELEZARA]{FFFFFF} Prekinuli ste rad u zeljezari. Oprema je vracena.");
    return 1;
}

// OnPlayerUpdate - distance check za anti-abuse i auto-trigger za mining CP
stock Zeljezara_OnUpdate(playerid) {
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_MINING) {
        // Provjera da li je igrac napustio zonu
        if(!ZeljIsInArea(playerid)) {
            Zelj_StopMining(playerid);
            ZeljGRESKA(playerid, "Napustili ste zonu zeljezare! Rad prekinut.");
            return 1;
        }

        // Ako igrac kopa (ceka klik) - sprijeci kretanje ali dozvoli klikanje
        // NE freezamo igraca jer to blokira KEY_FIRE evente!
        // Umjesto toga, vracamo ga na poziciju ako odmakne
        if(ZeljPlayerData[playerid][zeljDigging] && ZeljPlayerData[playerid][zeljWaitingClick]) {
            new Float:px, Float:py, Float:pz;
            GetPlayerPos(playerid, px, py, pz);
            new Float:dist = ZeljDist(px, py, pz, 
                ZeljPlayerData[playerid][zeljTrakaX],
                ZeljPlayerData[playerid][zeljTrakaY],
                ZeljPlayerData[playerid][zeljTrakaZ]);
            
            // Ako se udaljio vise od 2.0 jedinice - vrati ga nazad
            if(dist > 2.0) {
                SetPlayerPos(playerid, 
                    ZeljPlayerData[playerid][zeljTrakaX],
                    ZeljPlayerData[playerid][zeljTrakaY],
                    ZeljPlayerData[playerid][zeljTrakaZ]);
            }
        }

        // AUTO-TRIGGER: Ako je igrac blizu mining pozicije i nije vec u procesu kopanja,
        // automatski pokreni kopanje (backup za checkpoint koji propada kroz objekte)
        if(!ZeljPlayerData[playerid][zeljDigging]) {
            new Float:px, Float:py, Float:pz;
            GetPlayerPos(playerid, px, py, pz);
            new idx = ZeljPlayerData[playerid][zeljCheckpointIdx];
            new cpIdx = Zelj_GetActiveCP(playerid, idx);
            // FIX: Bounds checking - sprijeci crash ako je cpIdx -1
            if(cpIdx < 0 || cpIdx >= sizeof(ZeljMiningCoords)) return 0;
            new Float:dist = ZeljDist(px, py, pz,
                ZeljMiningCoords[cpIdx][0],
                ZeljMiningCoords[cpIdx][1],
                ZeljMiningCoords[cpIdx][2]);

            // Ako je unutar 2.0 jedinica od checkpointa, pokreni kopanje (blize objektu)
            if(dist <= ZELJEZARA_CP_RANGE + 0.5) {
                Zelj_OnCheckpointEnter(playerid);
                return 1;
            }
        }
    }
    return 0;
}

// OnPlayerCheckpoint handler - return 1 ako je zeljezara CP
stock Zeljezara_OnCP(playerid) {
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_MINING) {
        Zelj_OnCheckpointEnter(playerid);
        return 1;
    }
    // Ako je opremljen i ide ka hali za preradu (checkpoint nakon mininga)
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_EQUIPPED && ZeljPlayerData[playerid][zeljRawMetal] > 0) {
        DisablePlayerCheckpoint(playerid);
        ZeljINFO(playerid, "Stigli ste do hale za preradu! Kucajte /preradi da preradite sirovi metal.");
        return 1;
    }
    // Ako je metal stigao na kraj trake - preuzimanje preradjenog metala
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_CONVEYOR) {
        Zelj_GiveProcessingResults(playerid);
        return 1;
    }
    return 0;
}

// ===========================================================================
//  KOMANDE - Ove funkcije treba pozvati iz CMD: hookova u gamemodeu
// ===========================================================================

stock ZeljCmd_Oprema(playerid) {
    return Zelj_UzmiOpremu(playerid);
}

stock ZeljCmd_KopajMetal(playerid) {
    return Zelj_PocniKopanje(playerid);
}

stock ZeljCmd_Preradi(playerid) {
    return Zelj_PocniPreradu(playerid);
}

stock ZeljCmd_Prekini(playerid) {
    return Zelj_Prekini(playerid);
}

// ===========================================================================
//  INFORMACIJE O SISTEMU
// ===========================================================================

stock ZeljCmd_Info(playerid) {
    ZeljMSG(playerid, "{00BFFF}=============================================");
    ZeljMSG(playerid, "{00BFFF}           SISTEM ZELJEZARE - INFO");
    ZeljMSG(playerid, "{00BFFF}=============================================");
    ZeljMSG(playerid, "{FFFFFF}1. Idite na lokaciju i kucajte /uzmiopremu");
    ZeljMSG(playerid, "{FFFFFF}2. Kucajte /kopajmetal da pocnete kopati");
    ZeljMSG(playerid, "{FFFFFF}3. Slijedite plave pickupove i kopajte metal");
    ZeljMSG(playerid, "{FFFFFF}4. Idite u halu i kucajte /preradi");
    ZeljMSG(playerid, "{FFFFFF}5. Metal i Metalni Otpad idu u inventory");
    ZeljMSG(playerid, "{FFD700}Komande: /uzmiopremu, /kopajmetal, /preradi, /prekidazeljezaru, /zeljezarainfo");
    ZeljMSG(playerid, "{00BFFF}Napomena: Slijedite plave pickupove do mjesta kopanja!");
    
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_COOLDOWN) {
        new preostalo = (ZeljPlayerData[playerid][zeljCooldownTime] - GetTickCount()) / 1000;
        if(preostalo > 0) {
            new str[64];
            format(str, sizeof(str), "{FF6347}Cooldown: %d sekundi", preostalo);
            ZeljMSG(playerid, str);
        }
    }
    return 1;
}

// ===========================================================================
//  DIALOG HANDLER - Za potvrdu prerade
// ===========================================================================

stock Zeljezara_OnDialog(playerid, dialogid, response, listitem, inputtext[]) {
    #pragma unused listitem
    #pragma unused inputtext
    return 0; // Nema dialoga u ovom sistemu za sada
}

// ===========================================================================
//  SAVE/LOAD - Ako treba spasiti state (opciono)
// ===========================================================================

stock Zelj_SaveData(playerid) {
    // Cooldown se cuva
    // Za sada ne treba poseban save jer inventory vec ima svoj save sistem
    #pragma unused playerid
    return 1;
}

stock Zelj_LoadData(playerid) {
    #pragma unused playerid
    return 1;
}

// ===========================================================================
//  ADMIN FUNKCIJE - za pristup izvan include-a
// ===========================================================================

// Resetuj cooldown za zeljezaru
stock Zeljezara_ResetCooldown(playerid) {
    if(playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    if(!IsPlayerConnected(playerid)) return 0;
    ZeljPlayerData[playerid][zeljCooldownTime] = 0;
    if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_COOLDOWN) {
        ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_NONE;
    }
    return 1;
}

// Postavi cooldown za zeljezaru (u sekundama, 0 = reset)
stock Zeljezara_SetCooldown(playerid, seconds) {
    if(playerid < 0 || playerid >= MAX_PLAYERS) return 0;
    if(!IsPlayerConnected(playerid)) return 0;
    if(seconds <= 0) {
        ZeljPlayerData[playerid][zeljCooldownTime] = 0;
        if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_COOLDOWN) {
            ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_NONE;
        }
    } else {
        ZeljPlayerData[playerid][zeljCooldownTime] = GetTickCount() + (seconds * 1000);
        if(ZeljPlayerData[playerid][zeljState] == ZELJEZARA_STATE_NONE) {
            ZeljPlayerData[playerid][zeljState] = ZELJEZARA_STATE_COOLDOWN;
        }
    }
    return 1;
}
