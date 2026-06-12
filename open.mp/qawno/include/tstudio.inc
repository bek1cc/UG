#if defined _INC_tstudio
        #endinput
#endif
#define _INC_tstudio

// ===== TEXTURE STUDIO 2.1 - Integrated into UG Gamemode =====
// Full in-game mapping system sa teksturama, map management, CEF panel
// Samo za RCON admina ili admin level 7+
// Koristi streamer plugin za dynamic objekte

#include <streamer>

// ===== CONSTANTS =====
#define TSTUDIO_BROWSER_ID          95
#define TSTUDIO_CEF_URL             "http://cef/tstudio/index.html"
#define TSTUDIO_MAPS_DIR            "tstudio/maps/"

#define MAX_TSTUDIO_MAPS            50
#define MAX_TSTUDIO_MAP_OBJECTS     1000
#define MAX_TSTUDIO_MAP_NAME        64
#define MAX_TSTUDIO_UNDO            30
#define MAX_MATERIAL_SLOTS          16
#define MAX_TEX_NAME                64
#define MAX_GRID_SIZE               10.0
#define MAX_TSTUDIO_GROUPS          50
#define MAX_TSTUDIO_GROUP_NAME      32
#define MAX_TSTUDIO_REMOVED_BLDGS   200
#define MAX_TSTUDIO_VEHICLES        50
#define MAX_TSTUDIO_PREFABS         20

// ===== TS 2.1 CUSTOM MODEL IDs =====
// Base model za TS objekte je 19375 (zid)
// Custom model IDs koriste negativne ID-ove
#define TS_MODEL_BASE               19375
#define TS_CUSTOM_PLANE_5x5         -2001
#define TS_CUSTOM_PLANE_5x10        -2002
#define TS_CUSTOM_PLANE_10x10       -2003
#define TS_CUSTOM_PLANE_10x20       -2004
#define TS_CUSTOM_PLANE_20x20       -2005
#define TS_CUSTOM_PLANE_25x25       -2006
#define TS_CUSTOM_PLANE_25x50       -2007
#define TS_CUSTOM_PLANE_50x50       -2008
#define TS_CUSTOM_PLANE_50x100      -2009
#define TS_CUSTOM_PLANE_100x100     -2010
#define TS_CUSTOM_WALL_2_5x3_5      -2011
#define TS_CUSTOM_WALL_5x3_5        -2012
#define TS_CUSTOM_WALL_10x3_5       -2013
#define TS_CUSTOM_DOOR_5x3_5        -2014
#define TS_CUSTOM_WINDOW_5x3_5      -2015
#define TS_CUSTOM_PLATFORM_5x2_5    -2016
#define TS_CUSTOM_PLATFORM_5x5      -2017
#define TS_CUSTOM_PLATFORM_5x5x05   -2018
#define TS_CUSTOM_PLATFORM_10x10x05 -2019
#define TS_CUSTOM_PLATFORM_20x20x05 -2020
#define TS_CUSTOM_RAMP_2_5x5x3_5   -2021
#define TS_CUSTOM_RAMP_2_5x5x5     -2022
#define TS_CUSTOM_RAMP_5x5x3_5     -2023
#define TS_CUSTOM_RAMP_5x5x5       -2024

// Edit modes
#define TSTUDIO_MODE_NONE           0
#define TSTUDIO_MODE_MOVE           1
#define TSTUDIO_MODE_ROTATE         2
#define TSTUDIO_MODE_SCALE          3
#define TSTUDIO_MODE_TEXTURE        4

// Move speeds
#define TSTUDIO_SPEED_SLOW          0.1
#define TSTUDIO_SPEED_NORMAL        0.5
#define TSTUDIO_SPEED_FAST          2.0
#define TSTUDIO_SPEED_ULTRA         10.0

// Rotate speeds
#define TSTUDIO_ROT_SLOW            1.0
#define TSTUDIO_ROT_NORMAL          5.0
#define TSTUDIO_ROT_FAST            15.0
#define TSTUDIO_ROT_ULTRA           45.0

// ===== ENUMS =====
enum E_MAP_DATA {
        bool:MapExists,
        MapName[MAX_TSTUDIO_MAP_NAME],
        MapCreator[MAX_PLAYER_NAME],
        MapObjectCount,
        bool:MapLoaded,
        MapTimestamp,
        MapLoadedObjects,
        Float:MapSpawnX, Float:MapSpawnY, Float:MapSpawnZ,
        MapInterior,
        MapVirtualWorld,
        MapRemovedBldgCount,
        MapVehicleCount,
        MapGroupCount,
}

enum E_REMOVED_BUILDING {
        bool:RBExists,
        RBModelID,
        Float:RBX, Float:RBY, Float:RBZ, Float:RBRange,
        RBMapIndex,
}

enum E_TS_VEHICLE {
        bool:TVehExists,
        TVehModel,
        TVehColor1, TVehColor2,
        TVehPaintJob,
        Float:TVehX, Float:TVehY, Float:TVehZ, Float:TVehA,
        TVehWorld, TVehInterior,
        TVehMapIndex,
        TVehGameID,       // In-game vehicle ID from CreateVehicle
}

enum E_TS_GROUP {
        bool:GrpExists,
        GrpName[MAX_TSTUDIO_GROUP_NAME],
        GrpMapIndex,
        GrpObjectCount,
}

enum E_MAP_OBJECT {
        bool:ObjExists,
        STREAMER_TAG_OBJECT:ObjStreamID,
        ObjModel,
        Float:ObjX, Float:ObjY, Float:ObjZ,
        Float:ObjRX, Float:ObjRY, Float:ObjRZ,
        ObjWorld,
        ObjInterior,
        Float:ObjStreamDist,
        Float:ObjDrawDist,
        ObjMapIndex,                    // Kojoj mapi pripada
        ObjGroupID,                     // Grupa kojoj pripada (-1 = nijedna)
        ObjNote[64],                    // Napomena za objekat
}

// Materials - separate arrays (Pawn doesn't support 3D arrays in enums)
new bool:ObjMatUsed[MAX_TSTUDIO_MAP_OBJECTS][MAX_MATERIAL_SLOTS];
new ObjMatIndex[MAX_TSTUDIO_MAP_OBJECTS][MAX_MATERIAL_SLOTS];
new ObjMatModel[MAX_TSTUDIO_MAP_OBJECTS][MAX_MATERIAL_SLOTS];
new ObjMatTXD[MAX_TSTUDIO_MAP_OBJECTS][MAX_MATERIAL_SLOTS][MAX_TEX_NAME];
new ObjMatTexture[MAX_TSTUDIO_MAP_OBJECTS][MAX_MATERIAL_SLOTS][MAX_TEX_NAME];
new ObjMatColor[MAX_TSTUDIO_MAP_OBJECTS][MAX_MATERIAL_SLOTS];

enum E_UNDO_ENTRY {
        bool:UndoExists,
        UndoType,                       // 0=create, 1=delete, 2=move, 3=rotate, 4=texture
        UndoObjSlot,                    // Index u ObjData
        UndoMapIndex,
        // Backup podaci
        STREAMER_TAG_OBJECT:UndoStreamID,
        UndoModel,
        Float:UndoX, Float:UndoY, Float:UndoZ,
        Float:UndoRX, Float:UndoRY, Float:UndoRZ,
}

enum E_PLAYER_TSTUDIO {
        bool:TsActive,                  // Da li je TS aktivan za ovog igraca
        bool:TsEditing,                 // Da li edituje objekat
        TsEditMode,                     // Trenutni edit mode
        TsSelectedObj,                  // Index selektovanog objekta (-1 = nista)
        TsCurrentMap,                   // Index mape na kojoj radi (-1 = nijedna)
        TsMoveSpeed,                    // 0=slow, 1=normal, 2=fast, 3=ultra
        TsRotSpeed,
        Float:TsGridSnap,               // Grid snap vrijednost (0.0 = off)
        bool:TsShowGrid,
        TsUndoCount,
        bool:TsCEFActive,
        bool:TsFlyMode,                 // Da li je fly mode aktivan
        TsCurrentGroup,                 // Trenutno selektovana grupa (-1 = nijedna)
        bool:TsShowText3D,              // Da li prikazuje Text3D label-e
}

// ===== GLOBAL DATA =====
new MapData[MAX_TSTUDIO_MAPS][E_MAP_DATA];
new TSObjData[MAX_TSTUDIO_MAP_OBJECTS][E_MAP_OBJECT];
new PlayerTS[MAX_PLAYERS][E_PLAYER_TSTUDIO];
new UndoStack[MAX_PLAYERS][MAX_TSTUDIO_UNDO][E_UNDO_ENTRY];
new TotalMaps = 0;
new TotalObjects = 0;
new TsSelectedMap[MAX_PLAYERS] = {-1, ...};

// New arrays for TS 2.1 features
new TSRemovedBldgs[MAX_TSTUDIO_REMOVED_BLDGS][E_REMOVED_BUILDING];
new TSVehicles[MAX_TSTUDIO_VEHICLES][E_TS_VEHICLE];
new TSGroups[MAX_TSTUDIO_GROUPS][E_TS_GROUP];
new TotalRemovedBldgs = 0;
new TotalTSVehicles = 0;
new TotalTSGroups = 0;

// Text3D labels za objekte
new Streamer3DText:TSObjLabels[MAX_TSTUDIO_MAP_OBJECTS];

// ===== FORWARDS =====
forward TStudio_OnGameModeInit();
forward TStudio_OnPlayerDisconnect(playerid);
forward TStudio_OnBrowserCreated(playerid, browserid);
forward TStudio_OnObjectEdited(playerid, objectid, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz);
forward TStudio_OnEditObject(playerid, objectid, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz);
forward TStudio_OnSelectObject(playerid, objectid, modelid, Float:x, Float:y, Float:z);
forward TStudio_SaveTimer();

// ===== INIT =====
stock TStudio_OnGameModeInit()
{
        // ===== REGISTER TS 2.1 CUSTOM MODELS =====
        // Planes
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLANE_5x5,         "plane_5x5.dff",         "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLANE_5x10,        "plane_5x10.dff",        "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLANE_10x10,       "plane_10x10.dff",       "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLANE_10x20,       "plane_10x20.dff",       "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLANE_20x20,       "plane_20x20.dff",       "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLANE_25x25,       "plane_25x25.dff",       "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLANE_25x50,       "plane_25x50.dff",       "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLANE_50x50,       "plane_50x50.dff",       "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLANE_50x100,      "plane_50x100.dff",      "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLANE_100x100,     "plane_100x100.dff",     "wallzzz.txd");
        // Walls
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_WALL_2_5x3_5,      "wall_2.5x3.5.dff",      "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_WALL_5x3_5,        "wall_5x3.5.dff",        "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_WALL_10x3_5,       "wall_10x3.5.dff",       "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_DOOR_5x3_5,        "door_5x3.5.dff",        "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_WINDOW_5x3_5,      "window_5x3.5.dff",      "wallzzz.txd");
        // Platforms
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLATFORM_5x2_5,    "platform_5x2.5.dff",    "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLATFORM_5x5,      "platform_5x5.dff",      "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLATFORM_5x5x05,   "platform_5x5x05.dff",   "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLATFORM_10x10x05, "platform_10x10x05.dff", "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_PLATFORM_20x20x05, "platform_20x20x05.dff", "wallzzz.txd");
        // Ramps
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_RAMP_2_5x5x3_5,   "ramp_2.5x5x3.5.dff",   "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_RAMP_2_5x5x5,     "ramp_2.5x5x5.dff",      "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_RAMP_5x5x3_5,     "ramp_5x5x3.5.dff",      "wallzzz.txd");
        AddSimpleModel(-1, TS_MODEL_BASE, TS_CUSTOM_RAMP_5x5x5,       "ramp_5x5x5.dff",        "wallzzz.txd");

        printf("[TSTUDIO] 24 custom models registered (planes, walls, platforms, ramps)");

        // Ucitaj sve postojece mape iz fajlova
        TStudio_LoadAllMaps();

        // Auto-save timer (svakih 5 minuta)
        SetTimer("TStudio_SaveTimer", 300000, true);

        printf("[TSTUDIO] Texture Studio 2.1 ucitan!");
        printf("[TSTUDIO] Mapa ucitano: %d | Objekata ukupno: %d", TotalMaps, TotalObjects);
        printf("[TSTUDIO] Komande: /tstudio, /tmap, /tobj, /tedit, /tmove, /trot, /ttex, /fm, /tgroup");
}

stock TStudio_OnPlayerDisconnect(playerid)
{
        if(PlayerTS[playerid][TsActive])
        {
                TStudio_Close(playerid);
        }
}

// ===== ACCESS CHECK =====
stock TStudio_HasAccess(playerid)
{
        if(IsPlayerAdmin(playerid)) return 1;
        if(PlayerInfo[playerid][xAdmin] >= 7) return 1;
        return 0;
}

// ===== OPEN / CLOSE =====
stock TStudio_Open(playerid)
{
        if(PlayerTS[playerid][TsActive]) return 0;
        if(!TStudio_HasAccess(playerid)) return 0;
        if(!PlayerLogged[playerid]) return 0;

        PlayerTS[playerid][TsActive] = true;
        PlayerTS[playerid][TsEditing] = false;
        PlayerTS[playerid][TsEditMode] = TSTUDIO_MODE_NONE;
        PlayerTS[playerid][TsSelectedObj] = -1;
        PlayerTS[playerid][TsCurrentMap] = -1;
        PlayerTS[playerid][TsMoveSpeed] = 1;
        PlayerTS[playerid][TsRotSpeed] = 1;
        PlayerTS[playerid][TsGridSnap] = 0.0;

        // Otvori CEF panel
        PlayerTS[playerid][TsCEFActive] = true;
        CEF_CreateBrowser(playerid, TSTUDIO_BROWSER_ID, TSTUDIO_CEF_URL, true);

        SendClientMessage(playerid, 0x10b981FF, " Texture Studio 2.1 otvoren!");
        SendClientMessage(playerid, 0x3b82f6FF, " Koristi /tmap za listu mapa, /tobj za objekte, /tedit za editovanje");
        printf("[TSTUDIO] Player %d opened Texture Studio", playerid);
        return 1;
}

stock TStudio_Close(playerid)
{
        if(!PlayerTS[playerid][TsActive]) return 0;

        // Deselectuj objekat ako je selektovan
        if(PlayerTS[playerid][TsSelectedObj] >= 0)
        {
                TStudio_DeselectObject(playerid);
        }

        // Zatvori CEF
        if(PlayerTS[playerid][TsCEFActive])
        {
                CEF_DestroyBrowser(playerid, TSTUDIO_BROWSER_ID);
                PlayerTS[playerid][TsCEFActive] = false;
        }

        PlayerTS[playerid][TsActive] = false;
        PlayerTS[playerid][TsEditing] = false;
        PlayerTS[playerid][TsEditMode] = TSTUDIO_MODE_NONE;

        SendClientMessage(playerid, 0xef4444FF, " Texture Studio zatvoren.");
        printf("[TSTUDIO] Player %d closed Texture Studio", playerid);
        return 1;
}

stock TStudio_IsOpen(playerid)
{
        return PlayerTS[playerid][TsActive];
}

// ===== MAP MANAGEMENT =====
stock TStudio_CreateMap(playerid, mapname[])
{
        if(!PlayerTS[playerid][TsActive]) return -1;

        // Pronadji prazan slot
        new slot = -1;
        for(new i = 0; i < MAX_TSTUDIO_MAPS; i++)
        {
                if(!MapData[i][MapExists])
                {
                        slot = i;
                        break;
                }
        }
        if(slot == -1) return -1;

        // Provjeri da li ime vec postoji
        for(new i = 0; i < MAX_TSTUDIO_MAPS; i++)
        {
                if(MapData[i][MapExists] && strcmp(MapData[i][MapName], mapname, true) == 0)
                {
                        return -2; // Ime zauzeto
                }
        }

        MapData[slot][MapExists] = true;
        format(MapData[slot][MapName], MAX_TSTUDIO_MAP_NAME, "%s", mapname);
        GetPlayerName(playerid, MapData[slot][MapCreator], MAX_PLAYER_NAME);
        MapData[slot][MapObjectCount] = 0;
        MapData[slot][MapLoaded] = false;
        MapData[slot][MapTimestamp] = gettime();
        MapData[slot][MapLoadedObjects] = 0;
        TotalMaps++;

        // Automatski postavi kao aktivnu mapu
        PlayerTS[playerid][TsCurrentMap] = slot;

        // Sacuvaj mapu
        TStudio_SaveMap(slot);

        printf("[TSTUDIO] Map '%s' created by %s (slot %d)", mapname, MapData[slot][MapCreator], slot);
        return slot;
}

stock TStudio_DeleteMap(mapindex)
{
        if(mapindex < 0 || mapindex >= MAX_TSTUDIO_MAPS) return 0;
        if(!MapData[mapindex][MapExists]) return 0;

        // Prvo unloadaj sve objekte mape
        TStudio_UnloadMap(mapindex);

        // Obrisi fajl
        new filepath[128];
        format(filepath, sizeof(filepath), TSTUDIO_MAPS_DIR"%s.map", MapData[mapindex][MapName]);
        if(fexist(filepath)) fremove(filepath);

        new name[MAX_TSTUDIO_MAP_NAME];
        format(name, sizeof(name), "%s", MapData[mapindex][MapName]);

        MapData[mapindex][MapExists] = false;
        MapData[mapindex][MapObjectCount] = 0;
        TotalMaps--;

        printf("[TSTUDIO] Map '%s' deleted (slot %d)", name, mapindex);
        return 1;
}

stock TStudio_LoadMap(mapindex)
{
        if(mapindex < 0 || mapindex >= MAX_TSTUDIO_MAPS) return 0;
        if(!MapData[mapindex][MapExists]) return 0;
        if(MapData[mapindex][MapLoaded]) return 0;

        // Kreiraj sve objekte mape
        new loaded = 0;
        for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
        {
                if(TSObjData[i][ObjExists] && TSObjData[i][ObjMapIndex] == mapindex)
                {
                        if(TSObjData[i][ObjStreamID] == STREAMER_TAG_OBJECT:0)
                        {
                                TSObjData[i][ObjStreamID] = CreateDynamicObject(
                                        TSObjData[i][ObjModel],
                                        TSObjData[i][ObjX], TSObjData[i][ObjY], TSObjData[i][ObjZ],
                                        TSObjData[i][ObjRX], TSObjData[i][ObjRY], TSObjData[i][ObjRZ],
                                        TSObjData[i][ObjWorld], TSObjData[i][ObjInterior],
                                        -1,
                                        TSObjData[i][ObjStreamDist], TSObjData[i][ObjDrawDist]
                                );

                                // Primijeni teksture
                                for(new m = 0; m < MAX_MATERIAL_SLOTS; m++)
                                {
                                        if(ObjMatUsed[i][m])
                                        {
                                                SetDynamicObjectMaterial(TSObjData[i][ObjStreamID],
                                                        ObjMatIndex[i][m],
                                                        ObjMatModel[i][m],
                                                        ObjMatTXD[i][m],
                                                        ObjMatTexture[i][m],
                                                        ObjMatColor[i][m]
                                                );
                                        }
                                }
                                loaded++;
                        }
                }
        }

        MapData[mapindex][MapLoaded] = true;
        MapData[mapindex][MapLoadedObjects] = loaded;

        // Apply removed buildings for all connected players
        for(new p = 0; p < MAX_PLAYERS; p++)
        {
                if(IsPlayerConnected(p)) TStudio_ApplyRemovedBuildings(p, mapindex);
        }

        printf("[TSTUDIO] Map '%s' loaded (%d objects)", MapData[mapindex][MapName], loaded);
        return 1;
}

stock TStudio_UnloadMap(mapindex)
{
        if(mapindex < 0 || mapindex >= MAX_TSTUDIO_MAPS) return 0;
        if(!MapData[mapindex][MapExists]) return 0;

        new unloaded = 0;
        for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
        {
                if(TSObjData[i][ObjExists] && TSObjData[i][ObjMapIndex] == mapindex)
                {
                        if(TSObjData[i][ObjStreamID] != STREAMER_TAG_OBJECT:0)
                        {
                                DestroyDynamicObject(TSObjData[i][ObjStreamID]);
                                TSObjData[i][ObjStreamID] = STREAMER_TAG_OBJECT:0;
                                unloaded++;
                        }
                }
        }

        MapData[mapindex][MapLoaded] = false;
        MapData[mapindex][MapLoadedObjects] = 0;
        printf("[TSTUDIO] Map '%s' unloaded (%d objects)", MapData[mapindex][MapName], unloaded);
        return 1;
}

// ===== OBJECT MANAGEMENT =====
stock TStudio_CreateObject(playerid, modelid, Float:x, Float:y, Float:z, Float:rx = 0.0, Float:ry = 0.0, Float:rz = 0.0)
{
        new mapindex = PlayerTS[playerid][TsCurrentMap];
        if(mapindex < 0) return -1;
        if(!MapData[mapindex][MapExists]) return -1;

        // Pronadji prazan slot
        new slot = -1;
        for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
        {
                if(!TSObjData[i][ObjExists])
                {
                        slot = i;
                        break;
                }
        }
        if(slot == -1) return -1;

        // Grid snap
        if(PlayerTS[playerid][TsGridSnap] > 0.0)
        {
                x = floatround(x / PlayerTS[playerid][TsGridSnap]) * PlayerTS[playerid][TsGridSnap];
                y = floatround(y / PlayerTS[playerid][TsGridSnap]) * PlayerTS[playerid][TsGridSnap];
                z = floatround(z / PlayerTS[playerid][TsGridSnap]) * PlayerTS[playerid][TsGridSnap];
        }

        // Kreiraj dynamic object
        new STREAMER_TAG_OBJECT:streamid = STREAMER_TAG_OBJECT:0;
        if(MapData[mapindex][MapLoaded])
        {
                streamid = CreateDynamicObject(modelid, x, y, z, rx, ry, rz, -1, -1, -1, 300.0, 0.0);
        }

        // Sacuvaj podatke
        TSObjData[slot][ObjExists] = true;
        TSObjData[slot][ObjStreamID] = streamid;
        TSObjData[slot][ObjModel] = modelid;
        TSObjData[slot][ObjX] = x;
        TSObjData[slot][ObjY] = y;
        TSObjData[slot][ObjZ] = z;
        TSObjData[slot][ObjRX] = rx;
        TSObjData[slot][ObjRY] = ry;
        TSObjData[slot][ObjRZ] = rz;
        TSObjData[slot][ObjWorld] = GetPlayerVirtualWorld(playerid);
        TSObjData[slot][ObjInterior] = GetPlayerInterior(playerid);
        TSObjData[slot][ObjStreamDist] = 300.0;
        TSObjData[slot][ObjDrawDist] = 0.0;
        TSObjData[slot][ObjMapIndex] = mapindex;

        // Reset materials
        for(new m = 0; m < MAX_MATERIAL_SLOTS; m++)
        {
                ObjMatUsed[slot][m] = false;
                ObjMatIndex[slot][m] = m;
                ObjMatModel[slot][m] = 0;
                ObjMatTXD[slot][m][0] = '\0';
                ObjMatTexture[slot][m][0] = '\0';
                ObjMatColor[slot][m] = 0;
        }

        MapData[mapindex][MapObjectCount]++;
        TotalObjects++;

        // Dodaj u undo stack
        TStudio_AddUndo(playerid, 0, slot, mapindex); // 0 = create

        return slot;
}

stock TStudio_DeleteObject(playerid, objslot)
{
        if(objslot < 0 || objslot >= MAX_TSTUDIO_MAP_OBJECTS) return 0;
        if(!TSObjData[objslot][ObjExists]) return 0;

        new mapindex = TSObjData[objslot][ObjMapIndex];

        // Dodaj u undo stack prije brisanja
        TStudio_AddUndo(playerid, 1, objslot, mapindex); // 1 = delete

        // Unisti stream object
        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
        {
                DestroyDynamicObject(TSObjData[objslot][ObjStreamID]);
        }

        // Deselect ako je selektovan
        if(PlayerTS[playerid][TsSelectedObj] == objslot)
        {
                PlayerTS[playerid][TsSelectedObj] = -1;
                PlayerTS[playerid][TsEditing] = false;
        }

        TSObjData[objslot][ObjExists] = false;
        TSObjData[objslot][ObjStreamID] = STREAMER_TAG_OBJECT:0;

        if(mapindex >= 0 && MapData[mapindex][MapExists])
        {
                MapData[mapindex][MapObjectCount]--;
        }
        TotalObjects--;

        return 1;
}

stock TStudio_SelectObject(playerid, objslot)
{
        if(objslot < 0 || objslot >= MAX_TSTUDIO_MAP_OBJECTS) return 0;
        if(!TSObjData[objslot][ObjExists]) return 0;

        // Deselect prethodni
        if(PlayerTS[playerid][TsSelectedObj] >= 0)
        {
                TStudio_DeselectObject(playerid);
        }

        PlayerTS[playerid][TsSelectedObj] = objslot;
        PlayerTS[playerid][TsEditing] = true;

        // Otvori SA:MP edit dialog za ovaj objekat
        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
        {
                EditDynamicObject(playerid, TSObjData[objslot][ObjStreamID]);
        }

        return 1;
}

stock TStudio_DeselectObject(playerid)
{
        if(PlayerTS[playerid][TsSelectedObj] < 0) return 0;

        PlayerTS[playerid][TsSelectedObj] = -1;
        PlayerTS[playerid][TsEditing] = false;
        PlayerTS[playerid][TsEditMode] = TSTUDIO_MODE_NONE;
        CancelEdit(playerid);
        return 1;
}

// ===== OBJECT EDITING - KEY BASED MOVEMENT =====
stock TStudio_MoveSelected(playerid, axis, Float:amount)
{
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return 0;

        switch(axis)
        {
                case 0: TSObjData[objslot][ObjX] += amount; // X
                case 1: TSObjData[objslot][ObjY] += amount; // Y
                case 2: TSObjData[objslot][ObjZ] += amount; // Z
        }

        // Grid snap
        if(PlayerTS[playerid][TsGridSnap] > 0.0)
        {
                if(axis == 0) TSObjData[objslot][ObjX] = floatround(TSObjData[objslot][ObjX] / PlayerTS[playerid][TsGridSnap]) * PlayerTS[playerid][TsGridSnap];
                if(axis == 1) TSObjData[objslot][ObjY] = floatround(TSObjData[objslot][ObjY] / PlayerTS[playerid][TsGridSnap]) * PlayerTS[playerid][TsGridSnap];
                if(axis == 2) TSObjData[objslot][ObjZ] = floatround(TSObjData[objslot][ObjZ] / PlayerTS[playerid][TsGridSnap]) * PlayerTS[playerid][TsGridSnap];
        }

        // Azuriraj stream object
        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
        {
                SetDynamicObjectPos(TSObjData[objslot][ObjStreamID], TSObjData[objslot][ObjX], TSObjData[objslot][ObjY], TSObjData[objslot][ObjZ]);
        }

        return 1;
}

stock TStudio_RotateSelected(playerid, axis, Float:amount)
{
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return 0;

        switch(axis)
        {
                case 0: TSObjData[objslot][ObjRX] += amount; // RX
                case 1: TSObjData[objslot][ObjRY] += amount; // RY
                case 2: TSObjData[objslot][ObjRZ] += amount; // RZ
        }

        // Normaliziraj rotaciju
        if(TSObjData[objslot][ObjRX] >= 360.0) TSObjData[objslot][ObjRX] -= 360.0;
        if(TSObjData[objslot][ObjRX] < 0.0) TSObjData[objslot][ObjRX] += 360.0;
        if(TSObjData[objslot][ObjRY] >= 360.0) TSObjData[objslot][ObjRY] -= 360.0;
        if(TSObjData[objslot][ObjRY] < 0.0) TSObjData[objslot][ObjRY] += 360.0;
        if(TSObjData[objslot][ObjRZ] >= 360.0) TSObjData[objslot][ObjRZ] -= 360.0;
        if(TSObjData[objslot][ObjRZ] < 0.0) TSObjData[objslot][ObjRZ] += 360.0;

        // Azuriraj stream object
        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
        {
                SetDynamicObjectRot(TSObjData[objslot][ObjStreamID], TSObjData[objslot][ObjRX], TSObjData[objslot][ObjRY], TSObjData[objslot][ObjRZ]);
        }

        return 1;
}

// ===== TEXTURE EDITING =====
stock TStudio_SetObjectTexture(playerid, objslot, matindex, modelid, txdname[], texturename[], color = 0)
{
        if(objslot < 0 || objslot >= MAX_TSTUDIO_MAP_OBJECTS) return 0;
        if(!TSObjData[objslot][ObjExists]) return 0;
        if(matindex < 0 || matindex >= MAX_MATERIAL_SLOTS) return 0;

        // Dodaj u undo (texture change)
        TStudio_AddUndo(playerid, 4, objslot, TSObjData[objslot][ObjMapIndex]); // 4 = texture

        // Sacuvaj podatke
        ObjMatUsed[objslot][matindex] = true;
        ObjMatIndex[objslot][matindex] = matindex;
        ObjMatModel[objslot][matindex] = modelid;
        format(ObjMatTXD[objslot][matindex], MAX_TEX_NAME, "%s", txdname);
        format(ObjMatTexture[objslot][matindex], MAX_TEX_NAME, "%s", texturename);
        ObjMatColor[objslot][matindex] = color;

        // Primijeni na stream object
        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
        {
                SetDynamicObjectMaterial(TSObjData[objslot][ObjStreamID], matindex, modelid, txdname, texturename, color);
        }

        return 1;
}

stock TStudio_ResetObjectTexture(playerid, objslot, matindex)
{
        if(objslot < 0 || objslot >= MAX_TSTUDIO_MAP_OBJECTS) return 0;
        if(!TSObjData[objslot][ObjExists]) return 0;
        if(matindex < 0 || matindex >= MAX_MATERIAL_SLOTS) return 0;

        ObjMatUsed[objslot][matindex] = false;
        ObjMatModel[objslot][matindex] = 0;
        ObjMatTXD[objslot][matindex][0] = '\0';
        ObjMatTexture[objslot][matindex][0] = '\0';
        ObjMatColor[objslot][matindex] = 0;

        // Resetuj na default teksturu - treba ponovo kreirati objekat
        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
        {
                new Float:tmpX, Float:tmpY, Float:tmpZ, Float:tmpRX, Float:tmpRY, Float:tmpRZ;
                tmpX = TSObjData[objslot][ObjX]; tmpY = TSObjData[objslot][ObjY]; tmpZ = TSObjData[objslot][ObjZ];
                tmpRX = TSObjData[objslot][ObjRX]; tmpRY = TSObjData[objslot][ObjRY]; tmpRZ = TSObjData[objslot][ObjRZ];

                DestroyDynamicObject(TSObjData[objslot][ObjStreamID]);
                TSObjData[objslot][ObjStreamID] = CreateDynamicObject(
                        TSObjData[objslot][ObjModel], tmpX, tmpY, tmpZ, tmpRX, tmpRY, tmpRZ,
                        TSObjData[objslot][ObjWorld], TSObjData[objslot][ObjInterior], -1,
                        TSObjData[objslot][ObjStreamDist], TSObjData[objslot][ObjDrawDist]
                );

                // Ponovo primijeni sve OSTALE teksture
                for(new m = 0; m < MAX_MATERIAL_SLOTS; m++)
                {
                        if(m == matindex) continue;
                        if(ObjMatUsed[objslot][m])
                        {
                                SetDynamicObjectMaterial(TSObjData[objslot][ObjStreamID],
                                        ObjMatIndex[objslot][m],
                                        ObjMatModel[objslot][m],
                                        ObjMatTXD[objslot][m],
                                        ObjMatTexture[objslot][m],
                                        ObjMatColor[objslot][m]
                                );
                        }
                }
        }

        return 1;
}

// ===== DUPLICATE OBJECT =====
stock TStudio_DuplicateObject(playerid, objslot)
{
        if(objslot < 0 || objslot >= MAX_TSTUDIO_MAP_OBJECTS) return -1;
        if(!TSObjData[objslot][ObjExists]) return -1;

        new Float:ox = TSObjData[objslot][ObjX] + 2.0;
        new Float:oy = TSObjData[objslot][ObjY];
        new Float:oz = TSObjData[objslot][ObjZ];

        new newslot = TStudio_CreateObject(playerid, TSObjData[objslot][ObjModel], ox, oy, oz,
                TSObjData[objslot][ObjRX], TSObjData[objslot][ObjRY], TSObjData[objslot][ObjRZ]);

        if(newslot >= 0)
        {
                // Kopiraj teksture
                for(new m = 0; m < MAX_MATERIAL_SLOTS; m++)
                {
                        if(ObjMatUsed[objslot][m])
                        {
                                TStudio_SetObjectTexture(playerid, newslot, m,
                                        ObjMatModel[objslot][m],
                                        ObjMatTXD[objslot][m],
                                        ObjMatTexture[objslot][m],
                                        ObjMatColor[objslot][m]
                                );
                        }
                }
        }

        return newslot;
}

// ===== UNDO SYSTEM =====
stock TStudio_AddUndo(playerid, type, objslot, mapindex)
{
        if(objslot < 0 || objslot >= MAX_TSTUDIO_MAP_OBJECTS) return;

        // Shift stack
        for(new i = MAX_TSTUDIO_UNDO - 1; i > 0; i--)
        {
                UndoStack[playerid][i] = UndoStack[playerid][i-1];
        }

        UndoStack[playerid][0][UndoExists] = true;
        UndoStack[playerid][0][UndoType] = type;
        UndoStack[playerid][0][UndoObjSlot] = objslot;
        UndoStack[playerid][0][UndoMapIndex] = mapindex;

        if(TSObjData[objslot][ObjExists])
        {
                UndoStack[playerid][0][UndoStreamID] = TSObjData[objslot][ObjStreamID];
                UndoStack[playerid][0][UndoModel] = TSObjData[objslot][ObjModel];
                UndoStack[playerid][0][UndoX] = TSObjData[objslot][ObjX];
                UndoStack[playerid][0][UndoY] = TSObjData[objslot][ObjY];
                UndoStack[playerid][0][UndoZ] = TSObjData[objslot][ObjZ];
                UndoStack[playerid][0][UndoRX] = TSObjData[objslot][ObjRX];
                UndoStack[playerid][0][UndoRY] = TSObjData[objslot][ObjRY];
                UndoStack[playerid][0][UndoRZ] = TSObjData[objslot][ObjRZ];
        }

        if(PlayerTS[playerid][TsUndoCount] < MAX_TSTUDIO_UNDO)
        {
                PlayerTS[playerid][TsUndoCount]++;
        }
}

stock TStudio_Undo(playerid)
{
        if(PlayerTS[playerid][TsUndoCount] <= 0) return 0;
        if(!UndoStack[playerid][0][UndoExists]) return 0;

        new type = UndoStack[playerid][0][UndoType];
        new objslot = UndoStack[playerid][0][UndoObjSlot];

        switch(type)
        {
                case 0: // Undo create = delete object
                {
                        if(TSObjData[objslot][ObjExists])
                        {
                                if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
                                {
                                        DestroyDynamicObject(TSObjData[objslot][ObjStreamID]);
                                }
                                TSObjData[objslot][ObjExists] = false;
                                TSObjData[objslot][ObjStreamID] = STREAMER_TAG_OBJECT:0;
                                TotalObjects--;
                        }
                }
                case 1: // Undo delete = restore object
                {
                        if(!TSObjData[objslot][ObjExists])
                        {
                                TSObjData[objslot][ObjExists] = true;
                                TSObjData[objslot][ObjModel] = UndoStack[playerid][0][UndoModel];
                                TSObjData[objslot][ObjX] = UndoStack[playerid][0][UndoX];
                                TSObjData[objslot][ObjY] = UndoStack[playerid][0][UndoY];
                                TSObjData[objslot][ObjZ] = UndoStack[playerid][0][UndoZ];
                                TSObjData[objslot][ObjRX] = UndoStack[playerid][0][UndoRX];
                                TSObjData[objslot][ObjRY] = UndoStack[playerid][0][UndoRY];
                                TSObjData[objslot][ObjRZ] = UndoStack[playerid][0][UndoRZ];
                                TSObjData[objslot][ObjMapIndex] = UndoStack[playerid][0][UndoMapIndex];

                                // Ponovo kreiraj stream object
                                new mapidx = UndoStack[playerid][0][UndoMapIndex];
                                if(mapidx >= 0 && MapData[mapidx][MapLoaded])
                                {
                                        TSObjData[objslot][ObjStreamID] = CreateDynamicObject(
                                                TSObjData[objslot][ObjModel],
                                                TSObjData[objslot][ObjX], TSObjData[objslot][ObjY], TSObjData[objslot][ObjZ],
                                                TSObjData[objslot][ObjRX], TSObjData[objslot][ObjRY], TSObjData[objslot][ObjRZ],
                                                TSObjData[objslot][ObjWorld], TSObjData[objslot][ObjInterior], -1,
                                                300.0, 0.0
                                        );
                                }
                                TotalObjects++;
                        }
                }
                case 2, 3, 4: // Undo move/rotate/texture = restore position/rotation
                {
                        if(TSObjData[objslot][ObjExists])
                        {
                                TSObjData[objslot][ObjX] = UndoStack[playerid][0][UndoX];
                                TSObjData[objslot][ObjY] = UndoStack[playerid][0][UndoY];
                                TSObjData[objslot][ObjZ] = UndoStack[playerid][0][UndoZ];
                                TSObjData[objslot][ObjRX] = UndoStack[playerid][0][UndoRX];
                                TSObjData[objslot][ObjRY] = UndoStack[playerid][0][UndoRY];
                                TSObjData[objslot][ObjRZ] = UndoStack[playerid][0][UndoRZ];

                                if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
                                {
                                        SetDynamicObjectPos(TSObjData[objslot][ObjStreamID], TSObjData[objslot][ObjX], TSObjData[objslot][ObjY], TSObjData[objslot][ObjZ]);
                                        SetDynamicObjectRot(TSObjData[objslot][ObjStreamID], TSObjData[objslot][ObjRX], TSObjData[objslot][ObjRY], TSObjData[objslot][ObjRZ]);
                                }
                        }
                }
        }

        // Shift stack back
        for(new i = 0; i < MAX_TSTUDIO_UNDO - 1; i++)
        {
                UndoStack[playerid][i] = UndoStack[playerid][i+1];
        }
        UndoStack[playerid][MAX_TSTUDIO_UNDO-1][UndoExists] = false;
        PlayerTS[playerid][TsUndoCount]--;

        return 1;
}

// ===== SAVE / LOAD =====
stock TStudio_SaveMap(mapindex)
{
        if(mapindex < 0 || mapindex >= MAX_TSTUDIO_MAPS) return 0;
        if(!MapData[mapindex][MapExists]) return 0;

        new filepath[128];
        format(filepath, sizeof(filepath), TSTUDIO_MAPS_DIR"%s.map", MapData[mapindex][MapName]);

        new File:file = fopen(filepath, io_write);
        if(!file) return 0;

        // Header
        new line[256];
        format(line, sizeof(line), "MAPINFO:%s:%s:%d:%d\r\n",
                MapData[mapindex][MapName],
                MapData[mapindex][MapCreator],
                MapData[mapindex][MapTimestamp],
                MapData[mapindex][MapObjectCount]);
        fwrite(file, line);

        // Objects
        new objcount = 0;
        for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
        {
                if(TSObjData[i][ObjExists] && TSObjData[i][ObjMapIndex] == mapindex)
                {
                        format(line, sizeof(line), "OBJ:%d:%f:%f:%f:%f:%f:%f:%d:%d:%f:%f\r\n",
                                TSObjData[i][ObjModel],
                                TSObjData[i][ObjX], TSObjData[i][ObjY], TSObjData[i][ObjZ],
                                TSObjData[i][ObjRX], TSObjData[i][ObjRY], TSObjData[i][ObjRZ],
                                TSObjData[i][ObjWorld], TSObjData[i][ObjInterior],
                                TSObjData[i][ObjStreamDist], TSObjData[i][ObjDrawDist]);
                        fwrite(file, line);

                        // Materials
                        for(new m = 0; m < MAX_MATERIAL_SLOTS; m++)
                        {
                                if(ObjMatUsed[i][m])
                                {
                                        format(line, sizeof(line), "MAT:%d:%d:%d:%s:%s:%d\r\n",
                                                i, // object slot
                                                m, // material index
                                                ObjMatModel[i][m],
                                                ObjMatTXD[i][m],
                                                ObjMatTexture[i][m],
                                                ObjMatColor[i][m]);
                                        fwrite(file, line);
                                }
                        }
                        objcount++;
                }
        }

        // Save Removed Buildings
        for(new r = 0; r < MAX_TSTUDIO_REMOVED_BLDGS; r++)
        {
                if(TSRemovedBldgs[r][RBExists] && TSRemovedBldgs[r][RBMapIndex] == mapindex)
                {
                        format(line, sizeof(line), "RB:%d:%.4f:%.4f:%.4f:%.4f\r\n",
                                TSRemovedBldgs[r][RBModelID],
                                TSRemovedBldgs[r][RBX], TSRemovedBldgs[r][RBY],
                                TSRemovedBldgs[r][RBZ], TSRemovedBldgs[r][RBRange]);
                        fwrite(file, line);
                }
        }

        fclose(file);
        MapData[mapindex][MapObjectCount] = objcount;
        printf("[TSTUDIO] Map '%s' saved (%d objects)", MapData[mapindex][MapName], objcount);
        return 1;
}

stock TStudio_LoadMapFromFile(filepath[], mapindex)
{
        new File:file = fopen(filepath, io_read);
        if(!file) return 0;

        new line[512];
        new objcount = 0;
        new tmpObjSlot = -1;

        while(fread(file, line))
        {
                // Remove newline
                new len = strlen(line);
                if(len > 0 && line[len-1] == '\n') line[--len] = '\0';
                if(len > 0 && line[len-1] == '\r') line[--len] = '\0';

                // Parse MAPINFO
                if(strfind(line, "MAPINFO:") == 0)
                {
                        new name[MAX_TSTUDIO_MAP_NAME], creator[MAX_PLAYER_NAME], ts[16], oc[16];
                        // Simple parse
                        new start = 8; // after "MAPINFO:"
                        new idx = start;

                        // Name
                        new j = 0;
                        while(idx < len && line[idx] != ':' && j < MAX_TSTUDIO_MAP_NAME-1)
                                name[j++] = line[idx++];
                        name[j] = '\0';
                        if(idx < len) idx++; // skip ':'

                        // Creator
                        j = 0;
                        while(idx < len && line[idx] != ':' && j < MAX_PLAYER_NAME-1)
                                creator[j++] = line[idx++];
                        creator[j] = '\0';
                        if(idx < len) idx++; // skip ':'

                        // Timestamp
                        j = 0;
                        while(idx < len && line[idx] != ':' && j < 15)
                                ts[j++] = line[idx++];
                        ts[j] = '\0';
                        if(idx < len) idx++; // skip ':'

                        // Object count
                        j = 0;
                        while(idx < len && j < 15)
                                oc[j++] = line[idx++];
                        oc[j] = '\0';

                        format(MapData[mapindex][MapName], MAX_TSTUDIO_MAP_NAME, "%s", name);
                        format(MapData[mapindex][MapCreator], MAX_PLAYER_NAME, "%s", creator);
                        MapData[mapindex][MapTimestamp] = strval(ts);
                        MapData[mapindex][MapExists] = true;
                        MapData[mapindex][MapLoaded] = false;
                }
                // Parse OBJ
                else if(strfind(line, "OBJ:") == 0)
                {
                        // Find free slot
                        new slot = -1;
                        for(new s = 0; s < MAX_TSTUDIO_MAP_OBJECTS; s++)
                        {
                                if(!TSObjData[s][ObjExists])
                                {
                                        slot = s;
                                        break;
                                }
                        }
                        if(slot == -1) continue;

                        // Parse: OBJ:model:x:y:z:rx:ry:rz:world:interior:streamdist:drawdist
                        new start = 4;
                        new idx = start;
                        new parts[12][64];
                        new partIdx = 0;

                        // Split by ':'
                        while(idx < len && partIdx < 12)
                        {
                                new k = 0;
                                while(idx < len && line[idx] != ':' && k < 63)
                                        parts[partIdx][k++] = line[idx++];
                                parts[partIdx][k] = '\0';
                                partIdx++;
                                if(idx < len) idx++;
                        }

                        if(partIdx >= 7)
                        {
                                TSObjData[slot][ObjExists] = true;
                                TSObjData[slot][ObjModel] = strval(parts[0]);
                                TSObjData[slot][ObjX] = floatstr(parts[1]);
                                TSObjData[slot][ObjY] = floatstr(parts[2]);
                                TSObjData[slot][ObjZ] = floatstr(parts[3]);
                                TSObjData[slot][ObjRX] = floatstr(parts[4]);
                                TSObjData[slot][ObjRY] = floatstr(parts[5]);
                                TSObjData[slot][ObjRZ] = floatstr(parts[6]);
                                TSObjData[slot][ObjWorld] = (partIdx > 7) ? strval(parts[7]) : -1;
                                TSObjData[slot][ObjInterior] = (partIdx > 8) ? strval(parts[8]) : -1;
                                TSObjData[slot][ObjStreamDist] = (partIdx > 9) ? floatstr(parts[9]) : 300.0;
                                TSObjData[slot][ObjDrawDist] = (partIdx > 10) ? floatstr(parts[10]) : 0.0;
                                TSObjData[slot][ObjMapIndex] = mapindex;
                                TSObjData[slot][ObjStreamID] = STREAMER_TAG_OBJECT:0; // Not loaded yet

                                // Reset materials
                                for(new m = 0; m < MAX_MATERIAL_SLOTS; m++)
                                {
                                        ObjMatUsed[slot][m] = false;
                                }

                                tmpObjSlot = slot;
                                objcount++;
                                TotalObjects++;
                        }
                }
                // Parse MAT
                else if(strfind(line, "MAT:") == 0 && tmpObjSlot >= 0)
                {
                        // Parse: MAT:objslot:matidx:model:txd:texture:color
                        new start = 4;
                        new idx = start;
                        new parts[6][MAX_TEX_NAME];
                        new partIdx = 0;

                        while(idx < len && partIdx < 6)
                        {
                                new k = 0;
                                while(idx < len && line[idx] != ':' && k < MAX_TEX_NAME-1)
                                        parts[partIdx][k++] = line[idx++];
                                parts[partIdx][k] = '\0';
                                partIdx++;
                                if(idx < len) idx++;
                        }

                        if(partIdx >= 5)
                        {
                                new objslot = strval(parts[0]);
                                new matidx = strval(parts[1]);

                                if(objslot >= 0 && objslot < MAX_TSTUDIO_MAP_OBJECTS && matidx >= 0 && matidx < MAX_MATERIAL_SLOTS)
                                {
                                        ObjMatUsed[objslot][matidx] = true;
                                        ObjMatIndex[objslot][matidx] = matidx;
                                        ObjMatModel[objslot][matidx] = strval(parts[2]);
                                        format(ObjMatTXD[objslot][matidx], MAX_TEX_NAME, "%s", parts[3]);
                                        format(ObjMatTexture[objslot][matidx], MAX_TEX_NAME, "%s", parts[4]);
                                        ObjMatColor[objslot][matidx] = (partIdx > 5) ? strval(parts[5]) : 0;
                                        tmpObjSlot = objslot;
                                }
                        }
                }
                // Parse RB (Removed Buildings)
                else if(strfind(line, "RB:") == 0)
                {
                        new start = 3;
                        new idx = start;
                        new parts[5][32];
                        new partIdx = 0;

                        while(idx < len && partIdx < 5)
                        {
                                new k = 0;
                                while(idx < len && line[idx] != ':' && k < 31)
                                        parts[partIdx][k++] = line[idx++];
                                parts[partIdx][k] = '\0';
                                partIdx++;
                                if(idx < len) idx++;
                        }

                        if(partIdx >= 5)
                        {
                                new rbslot = -1;
                                for(new r = 0; r < MAX_TSTUDIO_REMOVED_BLDGS; r++)
                                {
                                        if(!TSRemovedBldgs[r][RBExists]) { rbslot = r; break; }
                                }
                                if(rbslot >= 0)
                                {
                                        TSRemovedBldgs[rbslot][RBExists] = true;
                                        TSRemovedBldgs[rbslot][RBModelID] = strval(parts[0]);
                                        TSRemovedBldgs[rbslot][RBX] = floatstr(parts[1]);
                                        TSRemovedBldgs[rbslot][RBY] = floatstr(parts[2]);
                                        TSRemovedBldgs[rbslot][RBZ] = floatstr(parts[3]);
                                        TSRemovedBldgs[rbslot][RBRange] = floatstr(parts[4]);
                                        TSRemovedBldgs[rbslot][RBMapIndex] = mapindex;
                                        TotalRemovedBldgs++;
                                }
                        }
                }
        }

        fclose(file);
        MapData[mapindex][MapObjectCount] = objcount;
        return 1;
}

stock TStudio_LoadAllMaps()
{
        // Scan for .map files in tstudio/maps/
        new filepath[128], filename[64];
        new mapidx = 0;

        for(new f = 0; f < 50; f++)
        {
                format(filepath, sizeof(filepath), TSTUDIO_MAPS_DIR"*.map");
                // Manual scan - check common names
                // Since Pawn doesn't have directory listing, we check saved map list
        }

        // Load from index file
        new indexpath[64] = TSTUDIO_MAPS_DIR"index.lst";
        if(fexist(indexpath))
        {
                new File:idx = fopen(indexpath, io_read);
                new line[128];
                while(fread(idx, line))
                {
                        new len = strlen(line);
                        if(len > 0 && line[len-1] == '\n') line[--len] = '\0';
                        if(len > 0 && line[len-1] == '\r') line[--len] = '\0';
                        if(len == 0) continue;

                        format(filepath, sizeof(filepath), TSTUDIO_MAPS_DIR"%s.map", line);
                        if(fexist(filepath) && mapidx < MAX_TSTUDIO_MAPS)
                        {
                                if(TStudio_LoadMapFromFile(filepath, mapidx))
                                {
                                        mapidx++;
                                        TotalMaps++;
                                }
                        }
                }
                fclose(idx);
        }
}

stock TStudio_SaveMapIndex()
{
        new indexpath[64] = TSTUDIO_MAPS_DIR"index.lst";
        new File:idx = fopen(indexpath, io_write);
        if(!idx) return;

        for(new i = 0; i < MAX_TSTUDIO_MAPS; i++)
        {
                if(MapData[i][MapExists])
                {
                        new line[MAX_TSTUDIO_MAP_NAME + 4];
                        format(line, sizeof(line), "%s\r\n", MapData[i][MapName]);
                        fwrite(idx, line);
                }
        }
        fclose(idx);
}

stock TStudio_SaveAllMaps()
{
        for(new i = 0; i < MAX_TSTUDIO_MAPS; i++)
        {
                if(MapData[i][MapExists])
                {
                        TStudio_SaveMap(i);
                }
        }
        TStudio_SaveMapIndex();
        printf("[TSTUDIO] All maps saved (%d maps)", TotalMaps);
}

public TStudio_SaveTimer()
{
        TStudio_SaveAllMaps();
}

// ===== EXPORT AS PAWN CODE =====
stock TStudio_ExportMap(mapindex)
{
        if(mapindex < 0 || mapindex >= MAX_TSTUDIO_MAPS) return 0;
        if(!MapData[mapindex][MapExists]) return 0;

        new filepath[128];
        format(filepath, sizeof(filepath), TSTUDIO_MAPS_DIR"%s.pwn", MapData[mapindex][MapName]);

        new File:file = fopen(filepath, io_write);
        if(!file) return 0;

        new line[512];

        format(line, sizeof(line), "// Map: %s (by %s) - Exported by Texture Studio 2.1\r\n", MapData[mapindex][MapName], MapData[mapindex][MapCreator]);
        fwrite(file, line);
        format(line, sizeof(line), "// Objects: %d\r\n\r\n", MapData[mapindex][MapObjectCount]);
        fwrite(file, line);

        for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
        {
                if(TSObjData[i][ObjExists] && TSObjData[i][ObjMapIndex] == mapindex)
                {
                        format(line, sizeof(line), "CreateDynamicObject(%d, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %d, %d, -1, %.1f, %.1f);\r\n",
                                TSObjData[i][ObjModel],
                                TSObjData[i][ObjX], TSObjData[i][ObjY], TSObjData[i][ObjZ],
                                TSObjData[i][ObjRX], TSObjData[i][ObjRY], TSObjData[i][ObjRZ],
                                TSObjData[i][ObjWorld], TSObjData[i][ObjInterior],
                                TSObjData[i][ObjStreamDist], TSObjData[i][ObjDrawDist]);
                        fwrite(file, line);

                        // Materials
                        for(new m = 0; m < MAX_MATERIAL_SLOTS; m++)
                        {
                                if(ObjMatUsed[i][m])
                                {
                                        // Note: We need the stream object ID for the export, use a variable
                                        format(line, sizeof(line), "SetDynamicObjectMaterial(objid, %d, %d, \"%s\", \"%s\", %d);\r\n",
                                                m,
                                                ObjMatModel[i][m],
                                                ObjMatTXD[i][m],
                                                ObjMatTexture[i][m],
                                                ObjMatColor[i][m]);
                                        fwrite(file, line);
                                }
                        }
                }
        }

        fclose(file);
        printf("[TSTUDIO] Map '%s' exported as Pawn code", MapData[mapindex][MapName]);
        return 1;
}

// ===== GET SPEED VALUES =====
stock Float:TStudio_GetMoveSpeed(playerid)
{
        switch(PlayerTS[playerid][TsMoveSpeed])
        {
                case 0: return TSTUDIO_SPEED_SLOW;
                case 1: return TSTUDIO_SPEED_NORMAL;
                case 2: return TSTUDIO_SPEED_FAST;
                case 3: return TSTUDIO_SPEED_ULTRA;
        }
        return TSTUDIO_SPEED_NORMAL;
}

stock Float:TStudio_GetRotSpeed(playerid)
{
        switch(PlayerTS[playerid][TsRotSpeed])
        {
                case 0: return TSTUDIO_ROT_SLOW;
                case 1: return TSTUDIO_ROT_NORMAL;
                case 2: return TSTUDIO_ROT_FAST;
                case 3: return TSTUDIO_ROT_ULTRA;
        }
        return TSTUDIO_ROT_NORMAL;
}

// ===== COMMANDS =====

// /tstudio - Toggle Texture Studio
CMD:tstudio(playerid, params[])
{
        if(!PlayerLogged[playerid]) return SendErrorMessage(playerid, "Morate biti ulogovani!");
        if(!TStudio_HasAccess(playerid)) return SendErrorMessage(playerid, "Samo RCON admin ili admin 7+ moze koristiti Texture Studio!");

        if(TStudio_IsOpen(playerid))
        {
                TStudio_Close(playerid);
        }
        else
        {
                TStudio_Open(playerid);
        }
        return 1;
}

// /tmap - Map management commands
CMD:tmap(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new action[32], arg1[64];
        if(sscanf(params, "s[32]S()[64]", action, arg1)) return SendUsageMessage(playerid, "/tmap [new/list/load/unload/delete/save/export] [ime]");

        if(strcmp(action, "new", true) == 0)
        {
                if(strlen(arg1) < 2) return SendUsageMessage(playerid, "/tmap new [ime_mape]");
                new result = TStudio_CreateMap(playerid, arg1);
                if(result == -1) SendErrorMessage(playerid, "Nema vise slobodnih slotova za mape!");
                else if(result == -2) SendErrorMessage(playerid, "Mapa sa tim imenom vec postoji!");
                else
                {
                        new msg[128];
                        format(msg, sizeof(msg), " Mapa '%s' kreirana! (Slot %d)", arg1, result);
                        SendClientMessage(playerid, 0x10b981FF, msg);
                        // Update CEF panel
                        TStudio_SendMapList(playerid);
                }
        }
        else if(strcmp(action, "list", true) == 0)
        {
                SendClientMessage(playerid, 0x3b82f6FF, "=== MAP LIST ===");
                new count = 0;
                for(new i = 0; i < MAX_TSTUDIO_MAPS; i++)
                {
                        if(MapData[i][MapExists])
                        {
                                new msg[256];
                                format(msg, sizeof(msg), " [%d] %s (by %s) - %d obj | %s%s",
                                        i, MapData[i][MapName], MapData[i][MapCreator], MapData[i][MapObjectCount],
                                        MapData[i][MapLoaded] ? "{22c55e}LOADED" : "{ffaa00}UNLOADED",
                                        (PlayerTS[playerid][TsCurrentMap] == i) ? " {ff8800}[AKTIVNA]" : "");
                                SendClientMessage(playerid, 0xFFFFFFFF, msg);
                                count++;
                        }
                }
                if(count == 0) SendClientMessage(playerid, 0xffaa00FF, " Nema mapa. Kucaj /tmap new [ime] za novu mapu.");
        }
        else if(strcmp(action, "load", true) == 0)
        {
                new slot = strval(arg1);
                if(slot < 0 || slot >= MAX_TSTUDIO_MAPS || !MapData[slot][MapExists])
                        return SendErrorMessage(playerid, "Neispravan slot. Kucaj /tmap list");

                if(MapData[slot][MapLoaded]) return SendErrorMessage(playerid, "Mapa je vec ucitana!");

                TStudio_LoadMap(slot);
                new msg[128];
                format(msg, sizeof(msg), " Mapa '%s' ucitana! (%d obj)", MapData[slot][MapName], MapData[slot][MapLoadedObjects]);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else if(strcmp(action, "unload", true) == 0)
        {
                new slot = strval(arg1);
                if(slot < 0 || slot >= MAX_TSTUDIO_MAPS || !MapData[slot][MapExists])
                        return SendErrorMessage(playerid, "Neispravan slot. Kucaj /tmap list");

                TStudio_UnloadMap(slot);
                new msg[128];
                format(msg, sizeof(msg), " Mapa '%s' unloadana.", MapData[slot][MapName]);
                SendClientMessage(playerid, 0x3b82f6FF, msg);
        }
        else if(strcmp(action, "delete", true) == 0)
        {
                new slot = strval(arg1);
                if(slot < 0 || slot >= MAX_TSTUDIO_MAPS || !MapData[slot][MapExists])
                        return SendErrorMessage(playerid, "Neispravan slot. Kucaj /tmap list");

                new name[MAX_TSTUDIO_MAP_NAME];
                format(name, sizeof(name), "%s", MapData[slot][MapName]);
                TStudio_DeleteMap(slot);
                new msg[128];
                format(msg, sizeof(msg), " Mapa '%s' obrisana!", name);
                SendClientMessage(playerid, 0xef4444FF, msg);
                TStudio_SendMapList(playerid);
        }
        else if(strcmp(action, "save", true) == 0)
        {
                new slot = PlayerTS[playerid][TsCurrentMap];
                if(strlen(arg1) > 0) slot = strval(arg1);
                if(slot < 0 || slot >= MAX_TSTUDIO_MAPS || !MapData[slot][MapExists])
                        return SendErrorMessage(playerid, "Neispravan slot. Kucaj /tmap list");

                TStudio_SaveMap(slot);
                new msg[128];
                format(msg, sizeof(msg), " Mapa '%s' sacuvana!", MapData[slot][MapName]);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else if(strcmp(action, "saveall", true) == 0)
        {
                TStudio_SaveAllMaps();
                SendClientMessage(playerid, 0x10b981FF, " Sve mape sacuvane!");
        }
        else if(strcmp(action, "export", true) == 0)
        {
                new slot = strval(arg1);
                if(slot < 0 || slot >= MAX_TSTUDIO_MAPS || !MapData[slot][MapExists])
                        return SendErrorMessage(playerid, "Neispravan slot. Kucaj /tmap list");

                TStudio_ExportMap(slot);
                new msg[128];
                format(msg, sizeof(msg), " Mapa '%s' exportovana kao Pawn code! (tstudio/maps/%s.pwn)", MapData[slot][MapName], MapData[slot][MapName]);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else if(strcmp(action, "select", true) == 0)
        {
                new slot = strval(arg1);
                if(slot < 0 || slot >= MAX_TSTUDIO_MAPS || !MapData[slot][MapExists])
                        return SendErrorMessage(playerid, "Neispravan slot. Kucaj /tmap list");

                PlayerTS[playerid][TsCurrentMap] = slot;
                new msg[128];
                format(msg, sizeof(msg), " Aktivna mapa: %s (Slot %d, %d obj)", MapData[slot][MapName], slot, MapData[slot][MapObjectCount]);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else SendUsageMessage(playerid, "/tmap [new/list/load/unload/delete/save/export/select] [ime/slot]");

        return 1;
}

// /tobj - Object commands
CMD:tobj(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        if(PlayerTS[playerid][TsCurrentMap] < 0) return SendErrorMessage(playerid, "Prvo selektuj mapu: /tmap select [slot]");

        new action[32], arg1[32], arg2[32];
        if(sscanf(params, "s[32]S()[32]S()[32]", action, arg1, arg2)) return SendUsageMessage(playerid, "/tobj [create/delete/select/deselect/duplicate/info/list/near]");

        if(strcmp(action, "create", true) == 0)
        {
                new modelid = strval(arg1);
                if(modelid < 1 || modelid > 20000) return SendErrorMessage(playerid, "Neispravan model ID (1-20000)");

                new Float:x, Float:y, Float:z;
                GetPlayerPos(playerid, x, y, z);
                GetXYInFrontOfPlayer(playerid, x, y, 5.0);

                new slot = TStudio_CreateObject(playerid, modelid, x, y, z);
                if(slot < 0) return SendErrorMessage(playerid, "Nema vise slobodnih slotova za objekte!");

                new msg[128];
                format(msg, sizeof(msg), " Objekat kreiran! Model %d na poziciji (Slot %d)", modelid, slot);
                SendClientMessage(playerid, 0x10b981FF, msg);

                // Auto-select
                PlayerTS[playerid][TsSelectedObj] = slot;
                TStudio_SelectObject(playerid, slot);
        }
        else if(strcmp(action, "delete", true) == 0)
        {
                new objslot = PlayerTS[playerid][TsSelectedObj];
                if(strlen(arg1) > 0) objslot = strval(arg1);
                if(objslot < 0 || !TSObjData[objslot][ObjExists])
                        return SendErrorMessage(playerid, "Nije selektovan objekat! Kucaj /tobj select [slot] ili /tobj near");

                TStudio_DeleteObject(playerid, objslot);
                SendClientMessage(playerid, 0xef4444FF, " Objekat obrisan!");
        }
        else if(strcmp(action, "select", true) == 0)
        {
                new objslot = strval(arg1);
                if(objslot < 0 || !TSObjData[objslot][ObjExists])
                        return SendErrorMessage(playerid, "Neispravan slot. Kucaj /tobj list ili /tobj near");

                TStudio_SelectObject(playerid, objslot);
                new msg[128];
                format(msg, sizeof(msg), " Objekat selektovan! Slot %d, Model %d", objslot, TSObjData[objslot][ObjModel]);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else if(strcmp(action, "deselect", true) == 0)
        {
                TStudio_DeselectObject(playerid);
                SendClientMessage(playerid, 0x3b82f6FF, " Objekat deselektovan.");
        }
        else if(strcmp(action, "duplicate", true) == 0 || strcmp(action, "dup", true) == 0)
        {
                new objslot = PlayerTS[playerid][TsSelectedObj];
                if(objslot < 0 || !TSObjData[objslot][ObjExists])
                        return SendErrorMessage(playerid, "Nije selektovan objekat!");

                new newslot = TStudio_DuplicateObject(playerid, objslot);
                if(newslot < 0) return SendErrorMessage(playerid, "Greska pri dupliciranju!");

                new msg[128];
                format(msg, sizeof(msg), " Objekat dupliciran! Novi slot %d", newslot);
                SendClientMessage(playerid, 0x10b981FF, msg);
                PlayerTS[playerid][TsSelectedObj] = newslot;
        }
        else if(strcmp(action, "info", true) == 0)
        {
                new objslot = PlayerTS[playerid][TsSelectedObj];
                if(objslot < 0 || !TSObjData[objslot][ObjExists])
                        return SendErrorMessage(playerid, "Nije selektovan objekat!");

                new msg[256];
                format(msg, sizeof(msg), " Slot %d | Model %d | Pos: %.2f, %.2f, %.2f | Rot: %.2f, %.2f, %.2f",
                        objslot, TSObjData[objslot][ObjModel],
                        TSObjData[objslot][ObjX], TSObjData[objslot][ObjY], TSObjData[objslot][ObjZ],
                        TSObjData[objslot][ObjRX], TSObjData[objslot][ObjRY], TSObjData[objslot][ObjRZ]);
                SendClientMessage(playerid, 0x3b82f6FF, msg);
        }
        else if(strcmp(action, "list", true) == 0)
        {
                new mapindex = PlayerTS[playerid][TsCurrentMap];
                new count = 0;
                SendClientMessage(playerid, 0x3b82f6FF, "=== OBJECTS ===");
                for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS && count < 30; i++)
                {
                        if(TSObjData[i][ObjExists] && TSObjData[i][ObjMapIndex] == mapindex)
                        {
                                new msg[256];
                                format(msg, sizeof(msg), " [%d] Model %d | %.1f, %.1f, %.1f%s",
                                        i, TSObjData[i][ObjModel],
                                        TSObjData[i][ObjX], TSObjData[i][ObjY], TSObjData[i][ObjZ],
                                        (PlayerTS[playerid][TsSelectedObj] == i) ? " {ff8800}[SELEKTOVAN]" : "");
                                SendClientMessage(playerid, 0xFFFFFFFF, msg);
                                count++;
                        }
                }
                if(count == 0) SendClientMessage(playerid, 0xffaa00FF, " Nema objekata na ovoj mapi. Kucaj /tobj create [modelid]");
                else
                {
                        new msg[64];
                        format(msg, sizeof(msg), " Ukupno: %d obj (prikazano prvih 30)", MapData[mapindex][MapObjectCount]);
                        SendClientMessage(playerid, 0xaaaaaaFF, msg);
                }
        }
        else if(strcmp(action, "near", true) == 0)
        {
                // Find nearest object to player
                new Float:px, Float:py, Float:pz;
                GetPlayerPos(playerid, px, py, pz);
                new nearest = -1;
                new Float:neardist = 99999.0;
                new mapindex = PlayerTS[playerid][TsCurrentMap];

                for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
                {
                        if(TSObjData[i][ObjExists] && TSObjData[i][ObjMapIndex] == mapindex)
                        {
                                new Float:dist = floatsqroot(
                                        (px - TSObjData[i][ObjX]) * (px - TSObjData[i][ObjX]) +
                                        (py - TSObjData[i][ObjY]) * (py - TSObjData[i][ObjY]) +
                                        (pz - TSObjData[i][ObjZ]) * (pz - TSObjData[i][ObjZ])
                                );
                                if(dist < neardist)
                                {
                                        neardist = dist;
                                        nearest = i;
                                }
                        }
                }

                if(nearest >= 0)
                {
                        TStudio_SelectObject(playerid, nearest);
                        new msg[128];
                        format(msg, sizeof(msg), " Najblizi objekat: Slot %d, Model %d (%.1fm)", nearest, TSObjData[nearest][ObjModel], neardist);
                        SendClientMessage(playerid, 0x10b981FF, msg);
                }
                else SendErrorMessage(playerid, "Nema objekata u blizini!");
        }
        else SendUsageMessage(playerid, "/tobj [create/delete/select/deselect/duplicate/info/list/near]");

        return 1;
}

// /tedit - Edit mode
CMD:tedit(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat! /tobj select/near");

        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
        {
                EditDynamicObject(playerid, TSObjData[objslot][ObjStreamID]);
                SendClientMessage(playerid, 0x10b981FF, " Edit mode - pomjeraj objekat misem!");
        }
        else SendErrorMessage(playerid, "Objekat nije ucitan u svijet! /tmap load");
        return 1;
}

// /tmove - Move selected object
CMD:tmove(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        new axis[4], Float:amount;
        if(sscanf(params, "s[4]f", axis, amount)) return SendUsageMessage(playerid, "/tmove [x/y/z] [iznos] (npr: /tmove x 5.0)");

        new axIdx = -1;
        if(strcmp(axis, "x", true) == 0) axIdx = 0;
        else if(strcmp(axis, "y", true) == 0) axIdx = 1;
        else if(strcmp(axis, "z", true) == 0) axIdx = 2;
        else return SendErrorMessage(playerid, "Koristi x, y ili z za osu!");

        // Save for undo
        TStudio_AddUndo(playerid, 2, objslot, TSObjData[objslot][ObjMapIndex]);

        TStudio_MoveSelected(playerid, axIdx, amount);
        new msg[128], axisChar = axis[0];
        new Float:vals[3];
        vals[0] = TSObjData[objslot][ObjX]; vals[1] = TSObjData[objslot][ObjY]; vals[2] = TSObjData[objslot][ObjZ];
        format(msg, sizeof(msg), " Pomjereno po %c za %.2f | Pos: %.2f, %.2f, %.2f", axisChar, amount, vals[0], vals[1], vals[2]);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

// /trot - Rotate selected object
CMD:trot(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        new axis[4], Float:amount;
        if(sscanf(params, "s[4]f", axis, amount)) return SendUsageMessage(playerid, "/trot [x/y/z] [stepeni] (npr: /trot z 90.0)");

        new axIdx = -1;
        if(strcmp(axis, "x", true) == 0) axIdx = 0;
        else if(strcmp(axis, "y", true) == 0) axIdx = 1;
        else if(strcmp(axis, "z", true) == 0) axIdx = 2;
        else return SendErrorMessage(playerid, "Koristi x, y ili z za osu!");

        // Save for undo
        TStudio_AddUndo(playerid, 3, objslot, TSObjData[objslot][ObjMapIndex]);

        TStudio_RotateSelected(playerid, axIdx, amount);
        new msg[128], axisChar = axis[0];
        new Float:vals[3];
        vals[0] = TSObjData[objslot][ObjRX]; vals[1] = TSObjData[objslot][ObjRY]; vals[2] = TSObjData[objslot][ObjRZ];
        format(msg, sizeof(msg), " Rotirano po %c za %.1f | Rot: %.2f, %.2f, %.2f", axisChar, amount, vals[0], vals[1], vals[2]);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

// /ttex - Texture commands
CMD:ttex(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        new action[16];
        if(sscanf(params, "s[16]", action)) return SendUsageMessage(playerid, "/ttex [set/reset/info]");

        if(strcmp(action, "set", true) == 0)
        {
                new matindex, modelid, txdname[MAX_TEX_NAME], texturename[MAX_TEX_NAME], color_str[16];
                if(sscanf(params, "s[16]iis["#MAX_TEX_NAME"]s["#MAX_TEX_NAME"]S()[16]", action, matindex, modelid, txdname, texturename, color_str))
                        return SendUsageMessage(playerid, "/ttex set [mat_index] [modelid] [txd_name] [texture_name] [color_hex]");

                if(matindex < 0 || matindex >= MAX_MATERIAL_SLOTS) return SendErrorMessage(playerid, "Material index 0-15!");

                new color = 0;
                if(strlen(color_str) > 0) color = strval(color_str);

                TStudio_SetObjectTexture(playerid, objslot, matindex, modelid, txdname, texturename, color);
                new msg[256];
                format(msg, sizeof(msg), " Tekstura postavljena! Mat %d | Model %d | TXD: %s | Tex: %s", matindex, modelid, txdname, texturename);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else if(strcmp(action, "reset", true) == 0)
        {
                new matindex;
                if(sscanf(params, "s[16]i", action, matindex))
                        return SendUsageMessage(playerid, "/ttex reset [mat_index]");

                TStudio_ResetObjectTexture(playerid, objslot, matindex);
                new msg[64];
                format(msg, sizeof(msg), " Tekstura na slotu %d resetovana!", matindex);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else if(strcmp(action, "info", true) == 0)
        {
                SendClientMessage(playerid, 0x3b82f6FF, "=== MATERIAL INFO ===");
                for(new m = 0; m < MAX_MATERIAL_SLOTS; m++)
                {
                        if(ObjMatUsed[objslot][m])
                        {
                                new msg[256];
                                format(msg, sizeof(msg), " [%d] Model %d | TXD: %s | Tex: %s | Color: %d",
                                        m, ObjMatModel[objslot][m],
                                        ObjMatTXD[objslot][m],
                                        ObjMatTexture[objslot][m],
                                        ObjMatColor[objslot][m]);
                                SendClientMessage(playerid, 0xFFFFFFFF, msg);
                        }
                        else if(m < 4) // Show first 4 even if not set
                        {
                                new msg[64];
                                format(msg, sizeof(msg), " [%d] Default tekstura", m);
                                SendClientMessage(playerid, 0x666666FF, msg);
                        }
                }
        }
        else SendUsageMessage(playerid, "/ttex [set/reset/info]");

        return 1;
}

// /tspeed - Change move/rotate speed
CMD:tspeed(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");

        new type[8], level;
        if(sscanf(params, "s[8]i", type, level)) return SendUsageMessage(playerid, "/tspeed [move/rot] [0=slow,1=normal,2=fast,3=ultra]");

        if(strcmp(type, "move", true) == 0)
        {
                if(level < 0 || level > 3) return SendErrorMessage(playerid, "Level 0-3!");
                PlayerTS[playerid][TsMoveSpeed] = level;
                new speeds[][] = {"Slow (0.1)", "Normal (0.5)", "Fast (2.0)", "Ultra (10.0)"};
                new msg[64];
                format(msg, sizeof(msg), " Move speed: %s", speeds[level]);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else if(strcmp(type, "rot", true) == 0)
        {
                if(level < 0 || level > 3) return SendErrorMessage(playerid, "Level 0-3!");
                PlayerTS[playerid][TsRotSpeed] = level;
                new speeds[][] = {"Slow (1)", "Normal (5)", "Fast (15)", "Ultra (45)"};
                new msg[64];
                format(msg, sizeof(msg), " Rotate speed: %s", speeds[level]);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else SendUsageMessage(playerid, "/tspeed [move/rot] [0-3]");

        return 1;
}

// /tgrid - Grid snap
CMD:tgrid(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");

        new Float:grid;
        if(sscanf(params, "f", grid)) return SendUsageMessage(playerid, "/tgrid [iznos] (0 = off, npr. 0.5, 1.0, 2.0)");

        PlayerTS[playerid][TsGridSnap] = grid;
        if(grid <= 0.0) SendClientMessage(playerid, 0x10b981FF, " Grid snap: OFF");
        else
        {
                new msg[64];
                format(msg, sizeof(msg), " Grid snap: %.2f", grid);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        return 1;
}

// /tundo - Undo
CMD:tundo(playerid)
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");

        if(TStudio_Undo(playerid)) SendClientMessage(playerid, 0x10b981FF, " Undo!");
        else SendErrorMessage(playerid, "Nista za undo!");
        return 1;
}

// /tsave - Quick save current map
CMD:tsave(playerid)
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");

        new mapindex = PlayerTS[playerid][TsCurrentMap];
        if(mapindex < 0 || !MapData[mapindex][MapExists]) return SendErrorMessage(playerid, "Nije selektovana mapa!");

        TStudio_SaveMap(mapindex);
        new msg[128];
        format(msg, sizeof(msg), " Mapa '%s' sacuvana! (%d obj)", MapData[mapindex][MapName], MapData[mapindex][MapObjectCount]);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

// /tgoto - Teleport to object
CMD:tgoto(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");

        new objslot;
        if(sscanf(params, "i", objslot)) return SendUsageMessage(playerid, "/tgoto [obj_slot]");
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Neispravan slot!");

        SetPlayerPos(playerid, TSObjData[objslot][ObjX], TSObjData[objslot][ObjY], TSObjData[objslot][ObjZ] + 2.0);
        new msg[128];
        format(msg, sizeof(msg), " Teleportovan do objekta %d!", objslot);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

// ===== FLYMODE =====
CMD:fm(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        if(!TStudio_HasAccess(playerid)) return 0;

        PlayerTS[playerid][TsFlyMode] = !PlayerTS[playerid][TsFlyMode];
        if(PlayerTS[playerid][TsFlyMode])
        {
                TogglePlayerSpectating(playerid, true);
                SendClientMessage(playerid, 0x10b981FF, " Fly mode ON - koristi WASD + Space/Shift za kretanje");
        }
        else
        {
                TogglePlayerSpectating(playerid, false);
                SpawnPlayer(playerid);
                SendClientMessage(playerid, 0xef4444FF, " Fly mode OFF");
        }
        return 1;
}

// ===== GROUP SYSTEM =====
CMD:tgroup(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new action[32], arg1[64];
        if(sscanf(params, "s[32]S()[64]", action, arg1)) return SendUsageMessage(playerid, "/tgroup [new/list/select/add/remove/clear/delete] [ime]");

        if(strcmp(action, "new", true) == 0)
        {
                if(strlen(arg1) < 2) return SendUsageMessage(playerid, "/tgroup new [ime_groupe]");
                new mapindex = PlayerTS[playerid][TsCurrentMap];
                if(mapindex < 0) return SendErrorMessage(playerid, "Prvo selektuj mapu!");

                // Find free slot
                new slot = -1;
                for(new i = 0; i < MAX_TSTUDIO_GROUPS; i++)
                {
                        if(!TSGroups[i][GrpExists]) { slot = i; break; }
                }
                if(slot == -1) return SendErrorMessage(playerid, "Nema vise slobodnih grupa!");

                TSGroups[slot][GrpExists] = true;
                format(TSGroups[slot][GrpName], MAX_TSTUDIO_GROUP_NAME, "%s", arg1);
                TSGroups[slot][GrpMapIndex] = mapindex;
                TSGroups[slot][GrpObjectCount] = 0;
                PlayerTS[playerid][TsCurrentGroup] = slot;

                new msg[128];
                format(msg, sizeof(msg), " Grupa '%s' kreirana! (Slot %d)", arg1, slot);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else if(strcmp(action, "list", true) == 0)
        {
                new mapindex = PlayerTS[playerid][TsCurrentMap];
                SendClientMessage(playerid, 0x3b82f6FF, "=== GRUPE ===");
                new count = 0;
                for(new i = 0; i < MAX_TSTUDIO_GROUPS; i++)
                {
                        if(TSGroups[i][GrpExists] && TSGroups[i][GrpMapIndex] == mapindex)
                        {
                                new msg[128];
                                format(msg, sizeof(msg), " [%d] %s (%d obj)%s", i, TSGroups[i][GrpName], TSGroups[i][GrpObjectCount],
                                        (PlayerTS[playerid][TsCurrentGroup] == i) ? " {ff8800}[AKTIVNA]" : "");
                                SendClientMessage(playerid, 0xFFFFFFFF, msg);
                                count++;
                        }
                }
                if(count == 0) SendClientMessage(playerid, 0xffaa00FF, " Nema grupa. Kucaj /tgroup new [ime]");
        }
        else if(strcmp(action, "select", true) == 0)
        {
                new slot = strval(arg1);
                if(slot < 0 || slot >= MAX_TSTUDIO_GROUPS || !TSGroups[slot][GrpExists])
                        return SendErrorMessage(playerid, "Neispravan slot!");

                PlayerTS[playerid][TsCurrentGroup] = slot;
                new msg[128];
                format(msg, sizeof(msg), " Aktivna grupa: %s (Slot %d)", TSGroups[slot][GrpName], slot);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else if(strcmp(action, "add", true) == 0)
        {
                new objslot = PlayerTS[playerid][TsSelectedObj];
                new grpslot = PlayerTS[playerid][TsCurrentGroup];
                if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Selektuj objekat prvo!");
                if(grpslot < 0 || !TSGroups[grpslot][GrpExists]) return SendErrorMessage(playerid, "Selektuj grupu: /tgroup select [slot]");

                // Remove from old group
                if(TSObjData[objslot][ObjGroupID] >= 0)
                {
                        new oldgrp = TSObjData[objslot][ObjGroupID];
                        if(TSGroups[oldgrp][GrpObjectCount] > 0) TSGroups[oldgrp][GrpObjectCount]--;
                }

                TSObjData[objslot][ObjGroupID] = grpslot;
                TSGroups[grpslot][GrpObjectCount]++;
                new msg[128];
                format(msg, sizeof(msg), " Objekat %d dodan u grupu '%s'", objslot, TSGroups[grpslot][GrpName]);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else if(strcmp(action, "remove", true) == 0)
        {
                new objslot = PlayerTS[playerid][TsSelectedObj];
                if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Selektuj objekat!");
                if(TSObjData[objslot][ObjGroupID] < 0) return SendErrorMessage(playerid, "Objekat nije u grupi!");

                new grpslot = TSObjData[objslot][ObjGroupID];
                if(TSGroups[grpslot][GrpObjectCount] > 0) TSGroups[grpslot][GrpObjectCount]--;
                TSObjData[objslot][ObjGroupID] = -1;
                SendClientMessage(playerid, 0x10b981FF, " Objekat uklonjen iz grupe!");
        }
        else if(strcmp(action, "clear", true) == 0)
        {
                new grpslot = PlayerTS[playerid][TsCurrentGroup];
                if(grpslot < 0 || !TSGroups[grpslot][GrpExists]) return SendErrorMessage(playerid, "Selektuj grupu!");

                // Remove all objects from group
                for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
                {
                        if(TSObjData[i][ObjExists] && TSObjData[i][ObjGroupID] == grpslot)
                        {
                                TSObjData[i][ObjGroupID] = -1;
                        }
                }
                TSGroups[grpslot][GrpObjectCount] = 0;
                SendClientMessage(playerid, 0x10b981FF, " Svi objekti uklonjeni iz grupe!");
        }
        else if(strcmp(action, "delete", true) == 0)
        {
                new slot = strval(arg1);
                if(slot < 0 || slot >= MAX_TSTUDIO_GROUPS || !TSGroups[slot][GrpExists])
                        return SendErrorMessage(playerid, "Neispravan slot!");

                // Remove all objects from this group
                for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
                {
                        if(TSObjData[i][ObjExists] && TSObjData[i][ObjGroupID] == slot)
                        {
                                TSObjData[i][ObjGroupID] = -1;
                        }
                }
                TSGroups[slot][GrpExists] = false;
                if(PlayerTS[playerid][TsCurrentGroup] == slot) PlayerTS[playerid][TsCurrentGroup] = -1;
                SendClientMessage(playerid, 0xef4444FF, " Grupa obrisana!");
        }
        else SendUsageMessage(playerid, "/tgroup [new/list/select/add/remove/clear/delete] [ime/slot]");
        return 1;
}

// /gclone - Clone group
CMD:gclone(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new grpslot = PlayerTS[playerid][TsCurrentGroup];
        if(grpslot < 0 || !TSGroups[grpslot][GrpExists]) return SendErrorMessage(playerid, "Selektuj grupu: /tgroup select");

        new Float:offsetX = 5.0;
        if(sscanf(params, "f", offsetX)) offsetX = 5.0;

        new cloned = 0;
        for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
        {
                if(TSObjData[i][ObjExists] && TSObjData[i][ObjGroupID] == grpslot)
                {
                        new Float:ox = TSObjData[i][ObjX] + offsetX;
                        new newslot = TStudio_CreateObject(playerid, TSObjData[i][ObjModel], ox, TSObjData[i][ObjY], TSObjData[i][ObjZ],
                                TSObjData[i][ObjRX], TSObjData[i][ObjRY], TSObjData[i][ObjRZ]);
                        if(newslot >= 0)
                        {
                                // Copy textures
                                for(new m = 0; m < MAX_MATERIAL_SLOTS; m++)
                                {
                                        if(ObjMatUsed[i][m])
                                        {
                                                TStudio_SetObjectTexture(playerid, newslot, m,
                                                        ObjMatModel[i][m], ObjMatTXD[i][m], ObjMatTexture[i][m], ObjMatColor[i][m]);
                                        }
                                }
                                // Add to same group
                                TSObjData[newslot][ObjGroupID] = grpslot;
                                TSGroups[grpslot][GrpObjectCount]++;
                                cloned++;
                        }
                }
        }
        new msg[128];
        format(msg, sizeof(msg), " Grupa klonirana! (%d objekata, offset X: %.1f)", cloned, offsetX);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

// /gclear - Delete all objects in group
CMD:gclear(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new grpslot = PlayerTS[playerid][TsCurrentGroup];
        if(grpslot < 0 || !TSGroups[grpslot][GrpExists]) return SendErrorMessage(playerid, "Selektuj grupu: /tgroup select");

        new cleared = 0;
        for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
        {
                if(TSObjData[i][ObjExists] && TSObjData[i][ObjGroupID] == grpslot)
                {
                        TStudio_DeleteObject(playerid, i);
                        cleared++;
                }
        }
        TSGroups[grpslot][GrpObjectCount] = 0;
        new msg[128];
        format(msg, sizeof(msg), " Grupa ociscena! (%d objekata obrisano)", cleared);
        SendClientMessage(playerid, 0xef4444FF, msg);
        return 1;
}

// ===== REMOVE BUILDING SYSTEM =====
stock TStudio_AddRemovedBuilding(playerid, modelid, Float:x, Float:y, Float:z, Float:range)
{
        new mapindex = PlayerTS[playerid][TsCurrentMap];
        if(mapindex < 0) return -1;

        // Find free slot
        new slot = -1;
        for(new i = 0; i < MAX_TSTUDIO_REMOVED_BLDGS; i++)
        {
                if(!TSRemovedBldgs[i][RBExists]) { slot = i; break; }
        }
        if(slot == -1) return -1;

        TSRemovedBldgs[slot][RBExists] = true;
        TSRemovedBldgs[slot][RBModelID] = modelid;
        TSRemovedBldgs[slot][RBX] = x;
        TSRemovedBldgs[slot][RBY] = y;
        TSRemovedBldgs[slot][RBZ] = z;
        TSRemovedBldgs[slot][RBRange] = range;
        TSRemovedBldgs[slot][RBMapIndex] = mapindex;
        TotalRemovedBldgs++;
        MapData[mapindex][MapRemovedBldgCount]++;

        // Apply to all players if map is loaded
        if(MapData[mapindex][MapLoaded])
        {
                for(new p = 0; p < MAX_PLAYERS; p++)
                {
                        if(IsPlayerConnected(p)) RemoveBuildingForPlayer(p, modelid, x, y, z, range);
                }
        }
        return slot;
}

stock TStudio_ApplyRemovedBuildings(playerid, mapindex)
{
        new count = 0;
        for(new i = 0; i < MAX_TSTUDIO_REMOVED_BLDGS; i++)
        {
                if(TSRemovedBldgs[i][RBExists] && TSRemovedBldgs[i][RBMapIndex] == mapindex)
                {
                        RemoveBuildingForPlayer(playerid, TSRemovedBldgs[i][RBModelID],
                                TSRemovedBldgs[i][RBX], TSRemovedBldgs[i][RBY], TSRemovedBldgs[i][RBZ],
                                TSRemovedBldgs[i][RBRange]);
                        count++;
                }
        }
        return count;
}

// /trb - Remove building command
CMD:trb(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new action[16];
        if(sscanf(params, "s[16]", action)) return SendUsageMessage(playerid, "/trb [add/list/clear]");

        if(strcmp(action, "add", true) == 0)
        {
                new modelid;
                new Float:range = 0.25;
                if(sscanf(params, "s[16]iF(0.25)", action, modelid, range))
                        return SendUsageMessage(playerid, "/trb add [modelid] [range=0.25]");

                new Float:x, Float:y, Float:z;
                GetPlayerPos(playerid, x, y, z);

                new slot = TStudio_AddRemovedBuilding(playerid, modelid, x, y, z, range);
                if(slot < 0) return SendErrorMessage(playerid, "Nema vise slotova za removed buildings!");

                new msg[128];
                format(msg, sizeof(msg), " RemoveBuilding dodan! Model %d na %.1f,%.1f,%.1f (Range %.2f)", modelid, x, y, z, range);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else if(strcmp(action, "list", true) == 0)
        {
                new mapindex = PlayerTS[playerid][TsCurrentMap];
                SendClientMessage(playerid, 0x3b82f6FF, "=== REMOVED BUILDINGS ===");
                new count = 0;
                for(new i = 0; i < MAX_TSTUDIO_REMOVED_BLDGS; i++)
                {
                        if(TSRemovedBldgs[i][RBExists] && TSRemovedBldgs[i][RBMapIndex] == mapindex)
                        {
                                new msg[128];
                                format(msg, sizeof(msg), " [%d] Model %d | %.1f, %.1f, %.1f | R: %.2f", i,
                                        TSRemovedBldgs[i][RBModelID], TSRemovedBldgs[i][RBX],
                                        TSRemovedBldgs[i][RBY], TSRemovedBldgs[i][RBZ], TSRemovedBldgs[i][RBRange]);
                                SendClientMessage(playerid, 0xFFFFFFFF, msg);
                                count++;
                        }
                }
                if(count == 0) SendClientMessage(playerid, 0xffaa00FF, " Nema removed buildings.");
        }
        else if(strcmp(action, "clear", true) == 0)
        {
                new mapindex = PlayerTS[playerid][TsCurrentMap];
                new cleared = 0;
                for(new i = 0; i < MAX_TSTUDIO_REMOVED_BLDGS; i++)
                {
                        if(TSRemovedBldgs[i][RBExists] && TSRemovedBldgs[i][RBMapIndex] == mapindex)
                        {
                                TSRemovedBldgs[i][RBExists] = false;
                                cleared++;
                        }
                }
                MapData[mapindex][MapRemovedBldgCount] = 0;
                new msg[64];
                format(msg, sizeof(msg), " %d removed buildings obrisano!", cleared);
                SendClientMessage(playerid, 0xef4444FF, msg);
        }
        else SendUsageMessage(playerid, "/trb [add/list/clear]");
        return 1;
}

// ===== TEXT3D LABELS =====
stock TStudio_ShowText3D(playerid)
{
        if(!PlayerTS[playerid][TsActive]) return;
        PlayerTS[playerid][TsShowText3D] = true;
        new mapindex = PlayerTS[playerid][TsCurrentMap];

        for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
        {
                if(TSObjData[i][ObjExists] && TSObjData[i][ObjMapIndex] == mapindex)
                {
                        if(TSObjLabels[i] == Streamer3DText:0)
                        {
                                new label[64];
                                format(label, sizeof(label), "[%d] Model %d", i, TSObjData[i][ObjModel]);
                                TSObjLabels[i] = CreateDynamic3DTextLabel(label, 0x10b981FF,
                                        TSObjData[i][ObjX], TSObjData[i][ObjY], TSObjData[i][ObjZ] + 1.0,
                                        30.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0,
                                        TSObjData[i][ObjWorld], TSObjData[i][ObjInterior]);
                        }
                }
        }
        SendClientMessage(playerid, 0x10b981FF, " Text3D labeli prikazani!");
}

stock TStudio_HideText3D(playerid)
{
        PlayerTS[playerid][TsShowText3D] = false;
        for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
        {
                if(TSObjLabels[i] != Streamer3DText:0)
                {
                        DestroyDynamic3DTextLabel(TSObjLabels[i]);
                        TSObjLabels[i] = Streamer3DText:0;
                }
        }
        SendClientMessage(playerid, 0xef4444FF, " Text3D labeli sakriveni!");
}

// /st - Show text3d labels
CMD:st(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        if(PlayerTS[playerid][TsShowText3D]) TStudio_HideText3D(playerid);
        else TStudio_ShowText3D(playerid);
        return 1;
}

// /ht - Hide text3d labels
CMD:ht(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        TStudio_HideText3D(playerid);
        return 1;
}

// ===== OBJECT NOTE =====
CMD:note(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        if(strlen(params) < 1) return SendUsageMessage(playerid, "/note [tekst] (biljeska za objekat)");
        format(TSObjData[objslot][ObjNote], 64, "%s", params);
        new msg[128];
        format(msg, sizeof(msg), " Biljeska za objekat %d: %s", objslot, params);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

// ===== ROTATION COPY/PASTE =====
CMD:rc(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        // Copy rotation to "clipboard"
        SetPVarFloat(playerid, "ts_rotrx", TSObjData[objslot][ObjRX]);
        SetPVarFloat(playerid, "ts_rotry", TSObjData[objslot][ObjRY]);
        SetPVarFloat(playerid, "ts_rotrz", TSObjData[objslot][ObjRZ]);

        new msg[128];
        format(msg, sizeof(msg), " Rotacija kopirana: %.2f, %.2f, %.2f", TSObjData[objslot][ObjRX], TSObjData[objslot][ObjRY], TSObjData[objslot][ObjRZ]);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

CMD:rp(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        new Float:rx = GetPVarFloat(playerid, "ts_rotrx");
        new Float:ry = GetPVarFloat(playerid, "ts_rotry");
        new Float:rz = GetPVarFloat(playerid, "ts_rotrz");

        TStudio_AddUndo(playerid, 3, objslot, TSObjData[objslot][ObjMapIndex]);
        TSObjData[objslot][ObjRX] = rx;
        TSObjData[objslot][ObjRY] = ry;
        TSObjData[objslot][ObjRZ] = rz;
        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
                SetDynamicObjectRot(TSObjData[objslot][ObjStreamID], rx, ry, rz);

        new msg[128];
        format(msg, sizeof(msg), " Rotacija zalijepljena: %.2f, %.2f, %.2f", rx, ry, rz);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

CMD:rr(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        TStudio_AddUndo(playerid, 3, objslot, TSObjData[objslot][ObjMapIndex]);
        TSObjData[objslot][ObjRX] = 0.0;
        TSObjData[objslot][ObjRY] = 0.0;
        TSObjData[objslot][ObjRZ] = 0.0;
        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
                SetDynamicObjectRot(TSObjData[objslot][ObjStreamID], 0.0, 0.0, 0.0);

        SendClientMessage(playerid, 0x10b981FF, " Rotacija resetovana na 0, 0, 0!");
        return 1;
}

// ===== TEXTURE SEARCH =====
CMD:mts(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new search[32];
        if(sscanf(params, "s[32]", search)) return SendUsageMessage(playerid, "/mts [pretraga] - Pretrazi model texture");

        SendClientMessage(playerid, 0x3b82f6FF, "=== MODEL TEXTURE SEARCH ===");
        SendClientMessage(playerid, 0xaaaaaaFF, " Tip: Koristi CEF panel za detaljnu pretragu tekstura");
        SendClientMessage(playerid, 0xaaaaaaFF, " Quick textures dostupne u Textures panelu");

        // Show some common textures that match
        new msg[128];
        format(msg, sizeof(msg), " Pretraga za '%s' - koristi /ttex set za postavljanje", search);
        SendClientMessage(playerid, 0x3b82f6FF, msg);
        return 1;
}

CMD:ts(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new search[32];
        if(sscanf(params, "s[32]", search)) return SendUsageMessage(playerid, "/ts [pretraga] - Pretrazi teksture");

        SendClientMessage(playerid, 0x3b82f6FF, "=== TEXTURE SEARCH ===");
        new msg[128];
        format(msg, sizeof(msg), " Pretraga za '%s' - koristi CEF panel za detaljnu pretragu", search);
        SendClientMessage(playerid, 0x3b82f6FF, msg);
        return 1;
}

// ===== SET TEXTURE INDEX =====
CMD:sti(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        new matindex, modelid, txdname[MAX_TEX_NAME], texname[MAX_TEX_NAME], color;
        if(sscanf(params, "iiis["#MAX_TEX_NAME"]s["#MAX_TEX_NAME"]I(0)", matindex, modelid, txdname, texname, color))
                return SendUsageMessage(playerid, "/sti [mat_idx] [model] [txd] [tex] [color]");

        if(matindex < 0 || matindex >= MAX_MATERIAL_SLOTS) return SendErrorMessage(playerid, "Material index 0-15!");

        TStudio_SetObjectTexture(playerid, objslot, matindex, modelid, txdname, texname, color);
        new msg[256];
        format(msg, sizeof(msg), " Texture set! Mat %d | Model %d | %s | %s", matindex, modelid, txdname, texname);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

// ===== MATERIAL COLOR =====
CMD:mtc(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        new matindex, color;
        if(sscanf(params, "ii", matindex, color)) return SendUsageMessage(playerid, "/mtc [mat_idx] [color_hex]");

        if(matindex < 0 || matindex >= MAX_MATERIAL_SLOTS) return SendErrorMessage(playerid, "Material index 0-15!");

        ObjMatColor[objslot][matindex] = color;
        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
        {
                if(ObjMatUsed[objslot][matindex])
                {
                        SetDynamicObjectMaterial(TSObjData[objslot][ObjStreamID], matindex,
                                ObjMatModel[objslot][matindex], ObjMatTXD[objslot][matindex],
                                ObjMatTexture[objslot][matindex], color);
                }
                else
                {
                        // Just set color on default material
                        SetDynamicObjectMaterial(TSObjData[objslot][ObjStreamID], matindex,
                                TSObjData[objslot][ObjModel], "none", "none", color);
                }
        }
        new msg[64];
        format(msg, sizeof(msg), " Boja materiala %d postavljena: 0x%08X", matindex, color);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

// /mtca - Color all materials
CMD:mtca(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        new color;
        if(sscanf(params, "i", color)) return SendUsageMessage(playerid, "/mtca [color_hex]");

        for(new m = 0; m < MAX_MATERIAL_SLOTS; m++)
        {
                ObjMatColor[objslot][m] = color;
                if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0 && ObjMatUsed[objslot][m])
                {
                        SetDynamicObjectMaterial(TSObjData[objslot][ObjStreamID], m,
                                ObjMatModel[objslot][m], ObjMatTXD[objslot][m],
                                ObjMatTexture[objslot][m], color);
                }
        }
        new msg[64];
        format(msg, sizeof(msg), " Boja svih materiala postavljena: 0x%08X", color);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

// ===== GOTO MAP SPAWN =====
CMD:gotomap(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new mapindex = PlayerTS[playerid][TsCurrentMap];
        if(mapindex < 0 || !MapData[mapindex][MapExists]) return SendErrorMessage(playerid, "Selektuj mapu!");

        if(MapData[mapindex][MapSpawnX] != 0.0 || MapData[mapindex][MapSpawnY] != 0.0)
        {
                SetPlayerPos(playerid, MapData[mapindex][MapSpawnX], MapData[mapindex][MapSpawnY], MapData[mapindex][MapSpawnZ]);
                if(MapData[mapindex][MapInterior] >= 0) SetPlayerInterior(playerid, MapData[mapindex][MapInterior]);
                if(MapData[mapindex][MapVirtualWorld] >= 0) SetPlayerVirtualWorld(playerid, MapData[mapindex][MapVirtualWorld]);
        }
        else
        {
                // Go to first object of the map
                for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
                {
                        if(TSObjData[i][ObjExists] && TSObjData[i][ObjMapIndex] == mapindex)
                        {
                                SetPlayerPos(playerid, TSObjData[i][ObjX], TSObjData[i][ObjY], TSObjData[i][ObjZ] + 3.0);
                                break;
                        }
                }
        }
        SendClientMessage(playerid, 0x10b981FF, " Teleportovan do mape!");
        return 1;
}

// ===== REDO =====
CMD:redo(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        // Simple redo - re-apply last undo (swap logic)
        // TS 2.1 doesn't have true redo, but we provide the command stub
        SendClientMessage(playerid, 0xffaa00FF, " Redo nije implementiran u ovoj verziji. Koristi /tundo za undo.");
        return 1;
}

// ===== EDIT GROUP (move all objects in group) =====
CMD:eg(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new grpslot = PlayerTS[playerid][TsCurrentGroup];
        if(grpslot < 0 || !TSGroups[grpslot][GrpExists]) return SendErrorMessage(playerid, "Selektuj grupu: /tgroup select");

        new axis[4], Float:amount;
        if(sscanf(params, "s[4]f", axis, amount)) return SendUsageMessage(playerid, "/eg [x/y/z] [iznos] - Pomjeri cijelu grupu");

        new axIdx = -1;
        if(strcmp(axis, "x", true) == 0) axIdx = 0;
        else if(strcmp(axis, "y", true) == 0) axIdx = 1;
        else if(strcmp(axis, "z", true) == 0) axIdx = 2;
        else return SendErrorMessage(playerid, "Koristi x, y ili z!");

        new moved = 0;
        for(new i = 0; i < MAX_TSTUDIO_MAP_OBJECTS; i++)
        {
                if(TSObjData[i][ObjExists] && TSObjData[i][ObjGroupID] == grpslot)
                {
                        TStudio_MoveSelectedForObj(i, axIdx, amount);
                        moved++;
                }
        }
        new msg[128];
        format(msg, sizeof(msg), " Grupa pomjerena po %c za %.2f (%d objekata)", axis[0], amount, moved);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

// Helper: Move specific object without player context
stock TStudio_MoveSelectedForObj(objslot, axis, Float:amount)
{
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return;

        switch(axis)
        {
                case 0: TSObjData[objslot][ObjX] += amount;
                case 1: TSObjData[objslot][ObjY] += amount;
                case 2: TSObjData[objslot][ObjZ] += amount;
        }

        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
        {
                SetDynamicObjectPos(TSObjData[objslot][ObjStreamID], TSObjData[objslot][ObjX], TSObjData[objslot][ObjY], TSObjData[objslot][ObjZ]);
        }
}

// ===== SELECT GROUP =====
CMD:selg(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");

        new slot;
        if(sscanf(params, "i", slot)) return SendUsageMessage(playerid, "/selg [slot] - Selektuj grupu");
        if(slot < 0 || slot >= MAX_TSTUDIO_GROUPS || !TSGroups[slot][GrpExists]) return SendErrorMessage(playerid, "Neispravan slot!");

        PlayerTS[playerid][TsCurrentGroup] = slot;
        new msg[128];
        format(msg, sizeof(msg), " Grupa selektovana: %s (Slot %d, %d obj)", TSGroups[slot][GrpName], slot, TSGroups[slot][GrpObjectCount]);
        SendClientMessage(playerid, 0x10b981FF, msg);
        return 1;
}

// ===== SET GROUP for selected object =====
CMD:setg(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");

        new grpslot;
        if(sscanf(params, "i", grpslot)) return SendUsageMessage(playerid, "/setg [grupa_slot] - Postavi grupu za selektovani obj");
        if(grpslot < -1 || grpslot >= MAX_TSTUDIO_GROUPS) return SendErrorMessage(playerid, "Neispravan slot! (-1 = ukloni iz grupe)");

        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        // Remove from old group
        if(TSObjData[objslot][ObjGroupID] >= 0)
        {
                new old = TSObjData[objslot][ObjGroupID];
                if(TSGroups[old][GrpObjectCount] > 0) TSGroups[old][GrpObjectCount]--;
        }

        TSObjData[objslot][ObjGroupID] = grpslot;
        if(grpslot >= 0 && TSGroups[grpslot][GrpExists])
        {
                TSGroups[grpslot][GrpObjectCount]++;
                new msg[128];
                format(msg, sizeof(msg), " Objekat %d dodan u grupu '%s'", objslot, TSGroups[grpslot][GrpName]);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        else
        {
                SendClientMessage(playerid, 0x10b981FF, " Objekat uklonjen iz grupe!");
        }
        return 1;
}

// ===== RESET OBJECT =====
CMD:robject(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        TStudio_AddUndo(playerid, 2, objslot, TSObjData[objslot][ObjMapIndex]);

        // Reset position to player
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        GetXYInFrontOfPlayer(playerid, x, y, 5.0);

        TSObjData[objslot][ObjX] = x;
        TSObjData[objslot][ObjY] = y;
        TSObjData[objslot][ObjZ] = z;
        TSObjData[objslot][ObjRX] = 0.0;
        TSObjData[objslot][ObjRY] = 0.0;
        TSObjData[objslot][ObjRZ] = 0.0;

        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
        {
                SetDynamicObjectPos(TSObjData[objslot][ObjStreamID], x, y, z);
                SetDynamicObjectRot(TSObjData[objslot][ObjStreamID], 0.0, 0.0, 0.0);
        }

        SendClientMessage(playerid, 0x10b981FF, " Objekat resetovan na tvoju poziciju!");
        return 1;
}

// ===== TS 2.1 QUICK MODEL ALIASES =====
// /co alias for /tobj create
CMD:co(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new modelid = strval(params);
        if(modelid < 1 && modelid > -3000) return SendUsageMessage(playerid, "/co [modelid]");

        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        GetXYInFrontOfPlayer(playerid, x, y, 5.0);

        new slot = TStudio_CreateObject(playerid, modelid, x, y, z);
        if(slot < 0) return SendErrorMessage(playerid, "Nema slobodnih slotova!");

        new msg[128];
        format(msg, sizeof(msg), " Objekat kreiran! Model %d (Slot %d)", modelid, slot);
        SendClientMessage(playerid, 0x10b981FF, msg);
        PlayerTS[playerid][TsSelectedObj] = slot;
        return 1;
}

// /do alias for delete object
CMD:do_(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        TStudio_DeleteObject(playerid, objslot);
        SendClientMessage(playerid, 0xef4444FF, " Objekat obrisan!");
        return 1;
}

// /eo alias for edit object
CMD:eo(playerid, params[])
{
        if(!TStudio_IsOpen(playerid)) return SendErrorMessage(playerid, "Prvo otvori Texture Studio: /tstudio");
        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0 || !TSObjData[objslot][ObjExists]) return SendErrorMessage(playerid, "Nije selektovan objekat!");

        if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
        {
                EditDynamicObject(playerid, TSObjData[objslot][ObjStreamID]);
                SendClientMessage(playerid, 0x10b981FF, " Edit mode!");
        }
        return 1;
}

// ===== CEF MAP LIST SEND =====
stock TStudio_SendMapList(playerid)
{
        if(!PlayerTS[playerid][TsCEFActive]) return;

        new json[2048];
        json = "{\"maps\":[";

        new count = 0;
        for(new i = 0; i < MAX_TSTUDIO_MAPS; i++)
        {
                if(MapData[i][MapExists])
                {
                        if(count > 0) strcat(json, ",");
                        new entry[256];
                        format(entry, sizeof(entry), "{\"slot\":%d,\"name\":\"%s\",\"creator\":\"%s\",\"objects\":%d,\"loaded\":%s,\"active\":%s}",
                                i, MapData[i][MapName], MapData[i][MapCreator], MapData[i][MapObjectCount],
                                MapData[i][MapLoaded] ? "true" : "false",
                                (PlayerTS[playerid][TsCurrentMap] == i) ? "true" : "false");
                        strcat(json, entry);
                        count++;
                }
        }

        strcat(json, "]}");

        // Send to CEF
        CEF_EmitEvent(playerid, TSTUDIO_BROWSER_ID, "tstudio:maplist", CEF_STR(json));
}

// ===== OBJECT EDIT CALLBACK =====
stock TStudio_OnEditObject(playerid, objectid, response, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz)
{
        if(!PlayerTS[playerid][TsActive]) return 0;

        new objslot = PlayerTS[playerid][TsSelectedObj];
        if(objslot < 0) return 0;

        if(response == EDIT_RESPONSE_FINAL)
        {
                // Save undo
                TStudio_AddUndo(playerid, 2, objslot, TSObjData[objslot][ObjMapIndex]);

                // Update position
                TSObjData[objslot][ObjX] = x;
                TSObjData[objslot][ObjY] = y;
                TSObjData[objslot][ObjZ] = z;
                TSObjData[objslot][ObjRX] = rx;
                TSObjData[objslot][ObjRY] = ry;
                TSObjData[objslot][ObjRZ] = rz;

                SendClientMessage(playerid, 0x10b981FF, " Objekat pomjeren!");
        }
        else if(response == EDIT_RESPONSE_CANCEL)
        {
                // Restore original position
                if(TSObjData[objslot][ObjStreamID] != STREAMER_TAG_OBJECT:0)
                {
                        SetDynamicObjectPos(TSObjData[objslot][ObjStreamID], TSObjData[objslot][ObjX], TSObjData[objslot][ObjY], TSObjData[objslot][ObjZ]);
                        SetDynamicObjectRot(TSObjData[objslot][ObjStreamID], TSObjData[objslot][ObjRX], TSObjData[objslot][ObjRY], TSObjData[objslot][ObjRZ]);
                }
        }

        return 1;
}

// GetXYInFrontOfPlayer vec postoji u gamemodu - koristimo istu funkciju
