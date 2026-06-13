// ============================================================
//  SA-MP 0.3.DL Compatibility Layer
//  Replaces open.mp includes with standard SA-MP 0.3.DL natives
//  Activated when SAMP_03DL is defined before #include <a_samp>
// ============================================================

#if defined _INC_samp_03dl_compat
    #endinput
#endif
#define _INC_samp_03dl_compat

// ============================================================
//  Define all omp guard macros so _open_mp.inc checks pass
// ============================================================
#define _INC_omp_core
#define _INC_omp_player
#define _INC_omp_actor
#define _INC_omp_checkpoint
#define _INC_omp_class
#define _INC_omp_database
#define _INC_omp_dialog
#define _INC_omp_gangzone
#define _INC_omp_http
#define _INC_omp_menu
#define _INC_omp_network
#define _INC_omp_object
#define _INC_omp_pickup
#define _INC_omp_textdraw
#define _INC_omp_variable
#define _INC_omp_vehicle
#define _INC_omp_textlabel

// ============================================================
//  Constants
// ============================================================
#if !defined MAX_PLAYERS
    const MAX_PLAYERS = 1000;
    #define MAX_PLAYERS 1000
#endif
#if !defined MAX_PLAYER_NAME
    const MAX_PLAYER_NAME = 24;
    #define MAX_PLAYER_NAME 24
#endif
const MAX_PLAYER_ATTACHED_OBJECTS = 10;
#define MAX_PLAYER_ATTACHED_OBJECTS 10
const MAX_CHATBUBBLE_LENGTH = 144;
#define MAX_CHATBUBBLE_LENGTH 144
const NO_TEAM = 0xFF;
#define NO_TEAM 255
const INVALID_PLAYER_ID = 0xFFFF;
#define INVALID_PLAYER_ID 65535
const MAX_VEHICLES = 2000;
#define MAX_VEHICLES 2000
const MAX_OBJECTS = 1000;
#define MAX_OBJECTS 1000
const MAX_GANGZONES = 1024;
#define MAX_GANGZONES 1024
const MAX_TEXT_DRAWS = 2048;
#define MAX_TEXT_DRAWS 2048
const MAX_PLAYER_TEXT_DRAWS = 256;
#define MAX_PLAYER_TEXT_DRAWS 256
const MAX_MENUS = 128;
#define MAX_MENUS 128
const MAX_3DTEXT_GLOBAL = 1024;
#define MAX_3DTEXT_GLOBAL 1024
const MAX_3DTEXT_PLAYER = 256;
#define MAX_3DTEXT_PLAYER 256
const MAX_PICKUPS = 4096;
#define MAX_PICKUPS 4096
const INVALID_VEHICLE_ID = 0xFFFF;
#define INVALID_VEHICLE_ID 65535
const INVALID_OBJECT_ID = 0xFFFF;
#define INVALID_OBJECT_ID 65535
const INVALID_ACTOR_ID = 0xFFFF;
#define INVALID_ACTOR_ID 65535
const INVALID_GANG_ZONE = 0xFFFF;
#define INVALID_GANG_ZONE 65535
const INVALID_TEXT_DRAW = 0xFFFF;
#define INVALID_TEXT_DRAW 65535
const INVALID_PLAYER_TEXT_DRAW = 0xFFFF;
#define INVALID_PLAYER_TEXT_DRAW 65535
const INVALID_MENU = 0xFF;
#define INVALID_MENU 255
const INVALID_3DTEXT_ID = 0xFFFF;
#define INVALID_3DTEXT_ID 65535
const INVALID_PICKUP = 0xFFFF;
#define INVALID_PICKUP 65535

// ============================================================
//  Enums
// ============================================================
enum PLAYER_STATE {
    PLAYER_STATE_NONE = 0,
    PLAYER_STATE_ONFOOT,
    PLAYER_STATE_DRIVER,
    PLAYER_STATE_PASSENGER,
    PLAYER_STATE_EXIT_VEHICLE,
    PLAYER_STATE_ENTER_VEHICLE_DRIVER,
    PLAYER_STATE_ENTER_VEHICLE_PASSENGER,
    PLAYER_STATE_WASTED,
    PLAYER_STATE_SPAWNED,
    PLAYER_STATE_SPECTATING
}
#pragma unused PLAYER_STATE

enum KEY {
    KEY_ACTION = 1,
    KEY_CROUCH = 2,
    KEY_FIRE = 4,
    KEY_SPRINT = 8,
    KEY_SECONDARY_ATTACK = 16,
    KEY_JUMP = 32,
    KEY_LOOK_RIGHT = 64,
    KEY_HANDBRAKE = 128,
    KEY_LOOK_LEFT = 256,
    KEY_SUBMISSION = 512,
    KEY_LOOK_BEHIND = 512,
    KEY_WALK = 1024,
    KEY_ANALOG_UP = 2048,
    KEY_ANALOG_DOWN = 4096,
    KEY_ANALOG_LEFT = 8192,
    KEY_ANALOG_RIGHT = 16384,
    KEY_YES = 65536,
    KEY_NO = 131072,
    KEY_CTRL_BACK = 262144
}
#pragma unused KEY

enum MAPICON {
    MAPICON_LOCAL = 0,
    MAPICON_LOCAL_CHECKPOINT,
    MAPICON_GLOBAL_CHECKPOINT,
    MAPICON_LOCAL_RING,
    MAPICON_GLOBAL_RING
}
#pragma unused MAPICON

enum WEAPONSKILL {
    WEAPONSKILL_PISTOL = 0,
    WEAPONSKILL_PISTOL_SILENCED,
    WEAPONSKILL_DESERT_EAGLE,
    WEAPONSKILL_SHOTGUN,
    WEAPONSKILL_SAWNOFF_SHOTGUN,
    WEAPONSKILL_SPAS12_SHOTGUN,
    WEAPONSKILL_MICRO_UZI,
    WEAPONSKILL_MP5,
    WEAPONSKILL_AK47,
    WEAPONSKILL_M4,
    WEAPONSKILL_SNIPERRIFLE
}
#pragma unused WEAPONSKILL

enum SPECIAL_ACTION {
    SPECIAL_ACTION_NONE = 0,
    SPECIAL_ACTION_DUCK = 1,
    SPECIAL_ACTION_USEJETPACK = 2,
    SPECIAL_ACTION_ENTER_VEHICLE = 3,
    SPECIAL_ACTION_EXIT_VEHICLE = 5,
    SPECIAL_ACTION_DANCE1 = 6,
    SPECIAL_ACTION_DANCE2 = 7,
    SPECIAL_ACTION_DANCE3 = 8,
    SPECIAL_ACTION_DANCE4 = 9,
    SPECIAL_ACTION_HANDSUP = 10,
    SPECIAL_ACTION_USECELLPHONE = 11,
    SPECIAL_ACTION_SITTING = 12,
    SPECIAL_ACTION_DRINK_BEER = 20,
    SPECIAL_ACTION_SMOKE_CIGGY = 21,
    SPECIAL_ACTION_DRINK_WINE = 22,
    SPECIAL_ACTION_DRINK_SPRUNK = 23,
    SPECIAL_ACTION_CARRY = 25,
    SPECIAL_ACTION_PISSING = 68
}
#pragma unused SPECIAL_ACTION

enum FIGHT_STYLE {
    FIGHT_STYLE_NORMAL = 4,
    FIGHT_STYLE_BOXING = 5,
    FIGHT_STYLE_KUNGFU = 6,
    FIGHT_STYLE_KNEEHEAD = 7,
    FIGHT_STYLE_GRABKICK = 15,
    FIGHT_STYLE_ELBOW = 16
}
#pragma unused FIGHT_STYLE

enum WEAPON {
    WEAPON_UNARMED = 0,
    WEAPON_BRASSKNUCKLE = 1,
    WEAPON_GOLFCLUB = 2,
    WEAPON_NITESTICK = 3,
    WEAPON_KNIFE = 4,
    WEAPON_BAT = 5,
    WEAPON_SHOVEL = 6,
    WEAPON_POOLSTICK = 7,
    WEAPON_KATANA = 8,
    WEAPON_CHAINSAW = 9,
    WEAPON_DILDO = 10,
    WEAPON_DILDO2 = 11,
    WEAPON_VIBRATOR = 12,
    WEAPON_VIBRATOR2 = 13,
    WEAPON_FLOWER = 14,
    WEAPON_CANE = 15,
    WEAPON_GRENADE = 16,
    WEAPON_TEARGAS = 17,
    WEAPON_MOLTOV = 18,
    WEAPON_COLT45 = 22,
    WEAPON_SILENCED = 23,
    WEAPON_DEAGLE = 24,
    WEAPON_SHOTGUN = 25,
    WEAPON_SAWEDOFF = 26,
    WEAPON_AUTOSHOTGUN = 27,
    WEAPON_UZI = 28,
    WEAPON_MP5 = 29,
    WEAPON_AK47 = 30,
    WEAPON_M4 = 31,
    WEAPON_TEC9 = 32,
    WEAPON_RIFLE = 33,
    WEAPON_SNIPER = 34,
    WEAPON_ROCKETLAUNCHER = 35,
    WEAPON_HEATSEEKER = 36,
    WEAPON_FLAMETHROWER = 37,
    WEAPON_MINIGUN = 38,
    WEAPON_SATCHEL = 39,
    WEAPON_BOMB = 40,
    WEAPON_SPRAYCAN = 41,
    WEAPON_FIREEXTINGUISHER = 42,
    WEAPON_CAMERA = 43,
    WEAPON_NIGHTVISION = 44,
    WEAPON_INFRARED = 45,
    WEAPON_PARACHUTE = 46,
    WEAPON_VEHICLE = 49,
    WEAPON_DROWN = 53,
    WEAPON_COLLISION = 54
}
#pragma unused WEAPON

enum BULLET_HIT_TYPE {
    BULLET_HIT_TYPE_NONE = 0,
    BULLET_HIT_TYPE_PLAYER = 1,
    BULLET_HIT_TYPE_VEHICLE = 2,
    BULLET_HIT_TYPE_OBJECT = 3,
    BULLET_HIT_TYPE_PLAYER_OBJECT = 4
}
#pragma unused BULLET_HIT_TYPE

enum EDIT_RESPONSE {
    EDIT_RESPONSE_CANCEL = 0,
    EDIT_RESPONSE_FINAL = 1,
    EDIT_RESPONSE_UPDATE = 2
}
#pragma unused EDIT_RESPONSE

enum OBJECT_MATERIAL_SIZE {
    OBJECT_MATERIAL_SIZE_32x32 = 10,
    OBJECT_MATERIAL_SIZE_64x32 = 20,
    OBJECT_MATERIAL_SIZE_64x64 = 30,
    OBJECT_MATERIAL_SIZE_128x32 = 40,
    OBJECT_MATERIAL_SIZE_128x64 = 50,
    OBJECT_MATERIAL_SIZE_128x128 = 60,
    OBJECT_MATERIAL_SIZE_256x32 = 70,
    OBJECT_MATERIAL_SIZE_256x64 = 80,
    OBJECT_MATERIAL_SIZE_256x128 = 90,
    OBJECT_MATERIAL_SIZE_256x256 = 100,
    OBJECT_MATERIAL_SIZE_512x64 = 110,
    OBJECT_MATERIAL_SIZE_512x128 = 120,
    OBJECT_MATERIAL_SIZE_512x256 = 130,
    OBJECT_MATERIAL_SIZE_512x512 = 140
}
#pragma unused OBJECT_MATERIAL_SIZE

enum OBJECT_MATERIAL_TEXT_ALIGN {
    OBJECT_MATERIAL_TEXT_ALIGN_LEFT = 0,
    OBJECT_MATERIAL_TEXT_ALIGN_CENTER = 1,
    OBJECT_MATERIAL_TEXT_ALIGN_RIGHT = 2
}
#pragma unused OBJECT_MATERIAL_TEXT_ALIGN

enum PLAYER_RECORDING_TYPE {
    PLAYER_RECORDING_TYPE_NONE = 0,
    PLAYER_RECORDING_TYPE_DRIVER = 1,
    PLAYER_RECORDING_TYPE_ONFOOT = 2
}
#pragma unused PLAYER_RECORDING_TYPE

enum TEXT_DRAW_FONT {
    TEXT_DRAW_FONT_0 = 0,
    TEXT_DRAW_FONT_1 = 1,
    TEXT_DRAW_FONT_2 = 2,
    TEXT_DRAW_FONT_3 = 3,
    TEXT_DRAW_FONT_4 = 4,
    TEXT_DRAW_FONT_5 = 5
}
#pragma unused TEXT_DRAW_FONT

enum DIALOG_STYLE {
    DIALOG_STYLE_MSGBOX = 0,
    DIALOG_STYLE_INPUT = 1,
    DIALOG_STYLE_LIST = 2,
    DIALOG_STYLE_PASSWORD = 3,
    DIALOG_STYLE_TABLIST = 4,
    DIALOG_STYLE_TABLIST_HEADERS = 5
}
#pragma unused DIALOG_STYLE

// Camera cut modes
const CAMERA_CUT = 2;
const CAMERA_MOVE = 1;

// Vehicle params
const VEHICLE_PARAMS_UNSET = -1;
const VEHICLE_PARAMS_OFF = 0;
const VEHICLE_PARAMS_ON = 1;

// ============================================================
//  SA-MP 0.3.DL Core Natives
// ============================================================
native SendClientMessage(playerid, color, const format[], {Float,_}:...);
native SendClientMessageToAll(color, const format[], {Float,_}:...);
native SendPlayerMessageToPlayer(playerid, senderid, const message[]);
native SendPlayerMessageToAll(senderid, const message[]);
native SendDeathMessage(killer, killee, weapon);
native SendDeathMessageToPlayer(playerid, killer, killee, weapon);
native GameTextForPlayer(playerid, const string[], time, style);
native GameTextForAll(const string[], time, style);
native SetTimer(const func[], interval, repeating);
native SetTimerEx(const func[], interval, repeating, const format[], {Float,_}:...);
native KillTimer(timerid);
native GetTickCount();
native GetMaxPlayers();
native VectorSize(Float:x, Float:y, Float:z);
native GetPlayerPoolSize();
native GetVehiclePoolSize();
native GetActorPoolSize();

// ============================================================
//  Player Natives (SA-MP 0.3.DL)
// ============================================================
native SetPlayerVirtualWorld(playerid, worldid);
native GetPlayerVirtualWorld(playerid);
native SetPlayerInterior(playerid, interiorid);
native GetPlayerInterior(playerid);
native SetPlayerPos(playerid, Float:x, Float:y, Float:z);
native GetPlayerPos(playerid, &Float:x, &Float:y, &Float:z);
native SetPlayerPosFindZ(playerid, Float:x, Float:y, Float:z);
native SetPlayerFacingAngle(playerid, Float:angle);
native GetPlayerFacingAngle(playerid, &Float:angle);
native IsPlayerInRangeOfPoint(playerid, Float:range, Float:x, Float:y, Float:z);
native GetPlayerDistanceFromPoint(playerid, Float:x, Float:y, Float:z);
native IsPlayerStreamedIn(playerid, forplayerid);
native SetPlayerHealth(playerid, Float:health);
native GetPlayerHealth(playerid, &Float:health);
native SetPlayerArmour(playerid, Float:armour);
native GetPlayerArmour(playerid, &Float:armour);
native SetPlayerAmmo(playerid, weaponslot, ammo);
native GetPlayerAmmo(playerid);
native GetPlayerWeaponState(playerid);
native GetPlayerTargetPlayer(playerid);
native GetPlayerTargetActor(playerid);
native SetPlayerTeam(playerid, teamid);
native GetPlayerTeam(playerid);
native SetPlayerScore(playerid, score);
native GetPlayerScore(playerid);
native GetPlayerDrunkLevel(playerid);
native SetPlayerDrunkLevel(playerid, level);
native SetPlayerColor(playerid, color);
native GetPlayerColor(playerid);
native SetPlayerSkin(playerid, skinid);
native GetPlayerSkin(playerid);
native GetPlayerCustomSkin(playerid);
native GivePlayerWeapon(playerid, weaponid, ammo);
native ResetPlayerWeapons(playerid);
native SetPlayerArmedWeapon(playerid, weaponid);
native GetPlayerWeaponData(playerid, slot, &weapons, &ammo);
native GivePlayerMoney(playerid, money);
native ResetPlayerMoney(playerid);
native SetPlayerName(playerid, const name[]);
native GetPlayerMoney(playerid);
native GetPlayerState(playerid);
native GetPlayerIp(playerid, ip[], len = sizeof(ip));
native GetPlayerPing(playerid);
native GetPlayerWeapon(playerid);
native GetPlayerKeys(playerid, &keys, &ud, &lr);
native GetPlayerName(playerid, name[], len = sizeof(name));
native SetPlayerTime(playerid, hour, minute);
native GetPlayerTime(playerid, &hour, &minute);
native SetPlayerWeather(playerid, weather);
native TogglePlayerClock(playerid, toggle);
native SetPlayerWantedLevel(playerid, level);
native GetPlayerWantedLevel(playerid);
native SetPlayerFightingStyle(playerid, style);
native GetPlayerFightingStyle(playerid);
native SetPlayerVelocity(playerid, Float:x, Float:y, Float:z);
native GetPlayerVelocity(playerid, &Float:x, &Float:y, &Float:z);
native PlayCrimeReportForPlayer(playerid, suspectid, crime);
native PlayAudioStreamForPlayer(playerid, const url[], Float:posX = 0.0, Float:posY = 0.0, Float:posZ = 0.0, Float:distance = 50.0, usepos = 0);
native StopAudioStreamForPlayer(playerid);
native SetPlayerShopName(playerid, const shopname[]);
native SetPlayerSkillLevel(playerid, skill, level);
native GetPlayerSkillLevel(playerid, skill);
native SetPlayerChatBubble(playerid, const text[], color, Float:drawdistance, expiretime);
native GetPlayerSurfingVehicleID(playerid);
native GetPlayerSurfingObjectID(playerid);
native RemoveBuildingForPlayer(playerid, modelid, Float:fX, Float:fY, Float:fZ, Float:fRadius);
native GetPlayerLastShotVectors(playerid, &Float:fOriginX, &Float:fOriginY, &Float:fOriginZ, &Float:fHitPosX, &Float:fHitPosY, &Float:fHitPosZ);
native SetPlayerAttachedObject(playerid, index, modelid, bone, Float:fOffsetX = 0.0, Float:fOffsetY = 0.0, Float:fOffsetZ = 0.0, Float:fRotX = 0.0, Float:fRotY = 0.0, Float:fRotZ = 0.0, Float:fScaleX = 1.0, Float:fScaleY = 1.0, Float:fScaleZ = 1.0, materialcolor1 = 0, materialcolor2 = 0);
native RemovePlayerAttachedObject(playerid, index);
native IsPlayerAttachedObjectSlotUsed(playerid, index);
native EditAttachedObject(playerid, index);
native CreatePlayerTextDraw(playerid, Float:x, Float:y, const text[]);
native PlayerTextDrawDestroy(playerid, Text:text);
native PlayerTextDrawLetterSize(playerid, Text:text, Float:x, Float:y);
native PlayerTextDrawTextSize(playerid, Text:text, Float:x, Float:y);
native PlayerTextDrawAlignment(playerid, Text:text, alignment);
native PlayerTextDrawColor(playerid, Text:text, color);
native PlayerTextDrawUseBox(playerid, Text:text, use);
native PlayerTextDrawBoxColor(playerid, Text:text, color);
native PlayerTextDrawSetShadow(playerid, Text:text, size);
native PlayerTextDrawSetOutline(playerid, Text:text, size);
native PlayerTextDrawBackgroundColor(playerid, Text:text, color);
native PlayerTextDrawFont(playerid, Text:text, font);
native PlayerTextDrawSetProportional(playerid, Text:text, set);
native PlayerTextDrawSetSelectable(playerid, Text:text, set);
native PlayerTextDrawShow(playerid, Text:text);
native PlayerTextDrawHide(playerid, Text:text);
native PlayerTextDrawSetString(playerid, Text:text, const string[]);
native PlayerTextDrawSetPreviewModel(playerid, Text:text, modelindex);
native PlayerTextDrawSetPreviewRot(playerid, Text:text, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fZoom = 1.0);
native PlayerTextDrawSetPreviewVehCol(playerid, Text:text, color1, color2);
native SetPlayerMapIcon(playerid, iconid, Float:x, Float:y, Float:z, markertype, color, style = 0);
native RemovePlayerMapIcon(playerid, iconid);
native SetPlayerCameraPos(playerid, Float:x, Float:y, Float:z);
native GetPlayerCameraPos(playerid, &Float:x, &Float:y, &Float:z);
native SetPlayerCameraLookAt(playerid, Float:x, Float:y, Float:z, cut = CAMERA_CUT);
native GetPlayerCameraFrontVector(playerid, &Float:x, &Float:y, &Float:z);
native GetPlayerCameraMode(playerid);
native GetPlayerCameraTargetPlayer(playerid);
native GetPlayerCameraTargetActor(playerid);
native GetPlayerCameraTargetVehicle(playerid);
native GetPlayerCameraTargetObject(playerid);
native Float:GetPlayerCameraAspectRatio(playerid);
native Float:GetPlayerCameraZoom(playerid);
native SetCameraBehindPlayer(playerid);
native AllowPlayerTeleport(playerid, allow);
native IsPlayerConnected(playerid);
native IsPlayerInVehicle(playerid, vehicleid);
native IsPlayerInAnyVehicle(playerid);
native IsPlayerInCheckpoint(playerid);
native IsPlayerInRaceCheckpoint(playerid);
native SetPlayerVirtualWorld(playerid, worldid);
native EnableStuntBonusForPlayer(playerid, enable);
native EnableStuntBonusForAll(enable);
native TogglePlayerControllable(playerid, toggle);
native PlayerPlaySound(playerid, soundid, Float:x, Float:y, Float:z);
native SetPlayerWorldBounds(playerid, Float:x_max, Float:x_min, Float:y_max, Float:y_min);
native ClearPlayerWorldBounds(playerid);
native SetPlayerMarkerForPlayer(playerid, targetid, color);
native ShowPlayerNameTagForPlayer(playerid, targetid, show);
native SetPlayerChatBubble(playerid, const text[], color, Float:drawdistance, expiretime);
native PutPlayerInVehicle(playerid, vehicleid, seatid);
native GetPlayerVehicleID(playerid);
native GetPlayerVehicleSeat(playerid);
native RemovePlayerFromVehicle(playerid);
native TogglePlayerSpectating(playerid, toggle);
native PlayerSpectatePlayer(playerid, targetplayerid, mode = 0);
native PlayerSpectateVehicle(playerid, targetvehicleid, mode = 0);
native StartRecordingPlayerData(playerid, recordtype, const recordname[]);
native StopRecordingPlayerData(playerid);
native SetPlayerGravity(playerid, Float:gravity);
native GetPlayerGravity(playerid);
native SetPlayerSpecialAction(playerid, actionid);
native GetPlayerSpecialAction(playerid);
native DisableRemoteVehicleCollisions(playerid, disable);
native SetPlayerTeam(playerid, teamid);
native ApplyAnimation(playerid, const animlib[], const animname[], Float:fDelta, loop, lockx, locky, freeze, time);
native ClearAnimations(playerid, forcesync = 0);
native GetPlayerAnimationIndex(playerid);
native GetAnimationName(index, animlib[], len1 = sizeof(animlib), animname[], len2 = sizeof(animname));
native GetPlayerAnimationLooped(playerid, index);
native GetPlayerAnimationTime(playerid, index);
native InterpolateCameraPos(playerid, Float:fromX, Float:fromY, Float:fromZ, Float:toX, Float:toY, Float:toZ, time, cut = CAMERA_CUT);
native InterpolateCameraLookAt(playerid, Float:fromX, Float:fromY, Float:fromZ, Float:toX, Float:toY, Float:toZ, time, cut = CAMERA_CUT);
native IsPlayerNPC(playerid);
native Kick(playerid);
native Ban(playerid);
native BanEx(playerid, const reason[]);
native SendRconCommand(const command[]);
native GetPlayerNetworkStats(playerid, const stats[], len = sizeof(stats));
native GetNetworkStats(const stats[], len = sizeof(stats));
native GetPlayerVersion(playerid, const version[], len = sizeof(version));

// ============================================================
//  Vehicle Natives (SA-MP 0.3.DL)
// ============================================================
native CreateVehicle(vehicletype, Float:x, Float:y, Float:z, Float:rotation, color1, color2, respawn_delay, addsiren = 0);
native DestroyVehicle(vehicleid);
native IsVehicleStreamedIn(vehicleid, forplayerid);
native GetVehiclePos(vehicleid, &Float:x, &Float:y, &Float:z);
native SetVehiclePos(vehicleid, Float:x, Float:y, Float:z);
native GetVehicleZAngle(vehicleid, &Float:angle);
native GetVehicleRotationQuat(vehicleid, &Float:w, &Float:x, &Float:y, &Float:z);
native SetVehicleZAngle(vehicleid, Float:angle);
native SetVehicleParamsForPlayer(vehicleid, playerid, objective, doorslocked);
native ManualVehicleEngineAndLights();
native SetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
native GetVehicleParamsEx(vehicleid, &engine, &lights, &alarm, &doors, &bonnet, &boot, &objective);
native SetVehicleToRespawn(vehicleid);
native LinkVehicleToInterior(vehicleid, interiorid);
native AddVehicleComponent(vehicleid, componentid);
native RemoveVehicleComponent(vehicleid, componentid);
native ChangeVehicleColor(vehicleid, color1, color2);
native ChangeVehiclePaintjob(vehicleid, paintjobid);
native SetVehicleHealth(vehicleid, Float:health);
native GetVehicleHealth(vehicleid, &Float:health);
native AttachTrailerToVehicle(trailerid, vehicleid);
native DetachTrailerFromVehicle(vehicleid);
native IsTrailerAttachedToVehicle(vehicleid);
native GetVehicleTrailer(vehicleid);
native SetVehicleNumberPlate(vehicleid, const numberplate[]);
native GetVehicleModel(vehicleid);
native GetVehicleComponentInSlot(vehicleid, slot);
native GetVehicleComponentType(component);
native RepairVehicle(vehicleid);
native GetVehicleVelocity(vehicleid, &Float:x, &Float:y, &Float:z);
native SetVehicleVelocity(vehicleid, Float:x, Float:y, Float:z);
native SetVehicleAngularVelocity(vehicleid, Float:x, Float:y, Float:z);
native GetVehicleDamageStatus(vehicleid, &panels, &doors, &lights, &tires);
native UpdateVehicleDamageStatus(vehicleid, panels, doors, lights, tires);
native SetVehicleVirtualWorld(vehicleid, worldid);
native GetVehicleVirtualWorld(vehicleid);
native GetVehicleModelInfo(vehiclemodel, infotype, &Float:x, &Float:y, &Float:z);
native AddStaticVehicle(modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:angle, color1, color2);
native AddStaticVehicleEx(modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:angle, color1, color2, respawn_delay, addsiren = 0);
native SetVehicleParamsCarDoors(vehicleid, driver, passenger, backleft, backright);
native GetVehicleParamsCarDoors(vehicleid, &driver, &passenger, &backleft, &backright);
native SetVehicleParamsCarWindows(vehicleid, driver, passenger, backleft, backright);
native GetVehicleParamsCarWindows(vehicleid, &driver, &passenger, &backleft, &backright);
native SetVehicleParamsSirenState(vehicleid, sirenState);
native GetVehicleParamsSirenState(vehicleid);

// ============================================================
//  Object Natives (SA-MP 0.3.DL)
// ============================================================
native CreateObject(modelid, Float:x, Float:y, Float:z, Float:rotX, Float:rotY, Float:rotZ, Float:drawdistance = 0.0);
native DestroyObject(objectid);
native IsValidObject(objectid);
native MoveObject(objectid, Float:X, Float:Y, Float:Z, Float:Speed, Float:RotX = -1000.0, Float:RotY = -1000.0, Float:RotZ = -1000.0);
native StopObject(objectid);
native IsObjectMoving(objectid);
native SetObjectPos(objectid, Float:x, Float:y, Float:z);
native GetObjectPos(objectid, &Float:x, &Float:y, &Float:z);
native SetObjectRot(objectid, Float:rotX, Float:rotY, Float:rotZ);
native GetObjectRot(objectid, &Float:rotX, &Float:rotY, &Float:rotZ);
native GetObjectModel(objectid);
native SetObjectNoCameraCollision(objectid);
native SetObjectMaterial(objectid, materialindex, modelid, const txdname[], const texturename[], materialcolor = 0);
native SetObjectMaterialText(objectid, const text[], materialindex = 0, materialsize = OBJECT_MATERIAL_SIZE_256x128, const fontface[] = "Arial", fontsize = 24, bold = 1, fontcolor = 0xFFFFFFFF, backcolor = 0, textalignment = 0);
native AttachObjectToVehicle(objectid, vehicleid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ);
native AttachObjectToObject(objectid, attachtoid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, SyncRotation = 1);
native AttachObjectToPlayer(objectid, playerid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ);
native EditObject(playerid, objectid);
native EditPlayerObject(playerid, objectid);
native SelectObject(playerid);
native CancelEdit(playerid);
native CreatePlayerObject(playerid, modelid, Float:x, Float:y, Float:z, Float:rotX, Float:rotY, Float:rotZ, Float:drawdistance = 0.0);
native DestroyPlayerObject(playerid, objectid);
native IsValidPlayerObject(playerid, objectid);
native MovePlayerObject(playerid, objectid, Float:X, Float:Y, Float:Z, Float:Speed, Float:RotX = -1000.0, Float:RotY = -1000.0, Float:RotZ = -1000.0);
native StopPlayerObject(playerid, objectid);
native IsPlayerObjectMoving(playerid, objectid);
native SetPlayerObjectPos(playerid, objectid, Float:x, Float:y, Float:z);
native GetPlayerObjectPos(playerid, objectid, &Float:x, &Float:y, &Float:z);
native SetPlayerObjectRot(playerid, objectid, Float:rotX, Float:rotY, Float:rotZ);
native GetPlayerObjectRot(playerid, objectid, &Float:rotX, &Float:rotY, &Float:rotZ);
native GetPlayerObjectModel(playerid, objectid);
native SetPlayerObjectNoCameraCollision(playerid, objectid);
native SetPlayerObjectMaterial(playerid, objectid, materialindex, modelid, const txdname[], const texturename[], materialcolor = 0);
native SetPlayerObjectMaterialText(playerid, objectid, const text[], materialindex = 0, materialsize = OBJECT_MATERIAL_SIZE_256x128, const fontface[] = "Arial", fontsize = 24, bold = 1, fontcolor = 0xFFFFFFFF, backcolor = 0, textalignment = 0);
native AttachPlayerObjectToVehicle(playerid, objectid, vehicleid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ);
native SetObjectNoCameraCol(objectid);
native SetPlayerObjectNoCameraCol(playerid, objectid);

// ============================================================
//  Actor Natives (SA-MP 0.3.DL)
// ============================================================
native CreateActor(modelid, Float:X, Float:Y, Float:Z, Float:Rotation);
native DestroyActor(actorid);
native IsActorStreamedIn(actorid, forplayerid);
native SetActorVirtualWorld(actorid, vworld);
native GetActorVirtualWorld(actorid);
native ApplyActorAnimation(actorid, const animlib[], const animname[], Float:fDelta, loop, lockx, locky, freeze, time);
native ClearActorAnimations(actorid);
native SetActorPos(actorid, Float:X, Float:Y, Float:Z);
native GetActorPos(actorid, &Float:X, &Float:Y, &Float:Z);
native SetActorFacingAngle(actorid, Float:Angle);
native GetActorFacingAngle(actorid, &Float:Angle);
native SetActorHealth(actorid, Float:health);
native GetActorHealth(actorid, &Float:health);
native SetActorInvulnerable(actorid, invulnerable = true);
native IsActorInvulnerable(actorid);
native IsValidActor(actorid);
native SetActorSkin(actorid, skinid);
native GetActorSkin(actorid);

// ============================================================
//  TextDraw Natives (SA-MP 0.3.DL)
// ============================================================
native Text:TextDrawCreate(Float:x, Float:y, const text[]);
native TextDrawDestroy(Text:text);
native TextDrawLetterSize(Text:text, Float:x, Float:y);
native TextDrawTextSize(Text:text, Float:x, Float:y);
native TextDrawAlignment(Text:text, alignment);
native TextDrawColor(Text:text, color);
native TextDrawUseBox(Text:text, use);
native TextDrawBoxColor(Text:text, color);
native TextDrawSetShadow(Text:text, size);
native TextDrawSetOutline(Text:text, size);
native TextDrawBackgroundColor(Text:text, color);
native TextDrawFont(Text:text, font);
native TextDrawSetProportional(Text:text, set);
native TextDrawSetSelectable(Text:text, set);
native TextDrawShowForPlayer(playerid, Text:text);
native TextDrawHideForPlayer(playerid, Text:text);
native TextDrawShowForAll(Text:text);
native TextDrawHideForAll(Text:text);
native TextDrawSetString(Text:text, const string[]);
native TextDrawSetPreviewModel(Text:text, modelindex);
native TextDrawSetPreviewRot(Text:text, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fZoom = 1.0);
native TextDrawSetPreviewVehCol(Text:text, color1, color2);

// ============================================================
//  Gang Zone Natives
// ============================================================
native GangZoneCreate(Float:minx, Float:miny, Float:maxx, Float:maxy);
native GangZoneDestroy(zoneid);
native GangZoneShowForPlayer(playerid, zoneid, color);
native GangZoneShowForAll(zoneid, color);
native GangZoneHideForPlayer(playerid, zoneid);
native GangZoneHideForAll(zoneid);
native GangZoneFlashForPlayer(playerid, zoneid, color);
native GangZoneFlashForAll(zoneid, color);
native GangZoneStopFlashForPlayer(playerid, zoneid);
native GangZoneStopFlashForAll(zoneid);

// ============================================================
//  3D Text Label Natives
// ============================================================
native Create3DTextLabel(const text[], color, Float:x, Float:y, Float:z, Float:drawdistance, virtualworld, testlos = 0);
native Delete3DTextLabel(id);
native Attach3DTextLabelToPlayer(id, playerid, Float:offsetX, Float:offsetY, Float:offsetZ);
native Attach3DTextLabelToVehicle(id, vehicleid, Float:offsetX, Float:offsetY, Float:offsetZ);
native Update3DTextLabelText(id, color, const text[]);
native CreatePlayer3DTextLabel(playerid, const text[], color, Float:x, Float:y, Float:z, Float:drawdistance, attachedplayer = INVALID_PLAYER_ID, attachedvehicle = INVALID_VEHICLE_ID, testlos = 0);
native DeletePlayer3DTextLabel(playerid, id);
native UpdatePlayer3DTextLabelText(playerid, id, color, const text[]);

// ============================================================
//  Menu Natives
// ============================================================
native Menu:CreateMenu(const title[], columns, Float:x, Float:y, Float:col1width, Float:col2width);
native DestroyMenu(Menu:menuid);
native AddMenuItem(Menu:menuid, column, const menutext[]);
native SetMenuColumnHeader(Menu:menuid, column, const columntext[]);
native ShowMenuForPlayer(Menu:menuid, playerid);
native HideMenuForPlayer(Menu:menuid, playerid);
native IsValidMenu(Menu:menuid);
native DisableMenu(Menu:menuid);
native DisableMenuRow(Menu:menuid, row);

// ============================================================
//  Pickup Natives
// ============================================================
native CreatePickup(model, type, Float:x, Float:y, Float:z, virtualworld = 0);
native DestroyPickup(pickupid);
native AddStaticPickup(model, type, Float:x, Float:y, Float:z, virtualworld = 0);

// ============================================================
//  Dialog Native
// ============================================================
native ShowPlayerDialog(playerid, dialogid, style, const caption[], const info[], const button1[], const button2[]);

// ============================================================
//  Class Selection Natives
// ============================================================
native AddPlayerClass(modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
native AddPlayerClassEx(teamid, modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
native AddPlayerClassWithSpawnInfo(teamid, modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo, spawn1_x, Float:spawn1_y, Float:spawn1_z, Float:spawn1_angle);
native SetSpawnInfo(playerid, team, skin, Float:x, Float:y, Float:z, Float:rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
native SpawnPlayer(playerid);

// ============================================================
//  Server Natives
// ============================================================
native SetGameModeText(const string[]);
native SetTeamCount(count);
native AddPlayerClass(modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:z_angle, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo);
native AddStaticVehicle(modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:angle, color1, color2);
native AddStaticVehicleEx(modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:angle, color1, color2, respawn_delay, addsiren = 0);
native ShowPlayerMarkers(mode);
native ShowNameTags(show);
native ShowPlayerNameTagForPlayer(playerid, targetid, show);
native SetWorldTime(hour);
native EnableVehicleFriendlyFire();
native UsePlayerPedAnims();
native DisableInteriorEnterExits();
native SetNameTagDrawDistance(Float:distance);
native DisableNameTagLOS();
native LimitGlobalChatRadius(Float:chat_radius);
native LimitPlayerMarkerRadius(Float:marker_radius);
native ConnectNPC(const name[], const script[]);
native IsPlayerNPC(playerid);
native AllowAdminTeleport(allow);
native SetDeathDropAmount(amount);
native CreateExplosion(Float:x, Float:y, Float:z, type, Float:radius);
native CreateExplosionForPlayer(playerid, Float:x, Float:y, Float:z, type, Float:radius);
native SetDisabledWeapons(...);
native EnableTirePopping(enable);
native EnableZoneNames(enable);
native AllowInteriorWeapons(allow);
native ShowPlayerDialog(playerid, dialogid, style, const caption[], const info[], const button1[], const button2[]);
native GetPlayerDialog(playerid);
native SetWeather(weatherid);
native SetGravity(Float:gravity);
native GetGravity();
native GetWeaponName(weaponid, const weaponname[], len = sizeof(weaponname));
native AllowPlayerTeleport(playerid, allow);
native IsPlayerAdmin(playerid);
native Kick(playerid);
native Ban(playerid);
native BanEx(playerid, const reason[]);
native SendRconCommand(const command[]);
native GetServerVarAsString(const varname[], const buffer[], len = sizeof(buffer));
native GetServerVarAsInt(const varname[]);
native GetServerVarAsBool(const varname[]);
native GetConsoleVarAsString(const varname[], const buffer[], len = sizeof(buffer));
native GetConsoleVarAsInt(const varname[]);
native GetConsoleVarAsBool(const varname[]);
native GetServerSettings(&showplayermarkes, &shownametags, &stuntbonus, &useplayerpedanims, &bLimitchatradius, &disableinteriorenterexits, &nametaglos, &manualvehicleengine, &limitplayermarkers, &vehiclefriendlyfire, &defaultcameracollision, &Float:fGlobalchatradius, &Float:fNameTagDrawDistance, &Float:fPlayermarkerslimit);
native NetStats_GetConnectedTime(playerid);
native NetStats_MessagesReceived(playerid);
native NetStats_BytesReceived(playerid);
native NetStats_MessagesSent(playerid);
native NetStats_BytesSent(playerid);
native NetStats_MessagesRecvPerSecond(playerid);
native Float:NetStats_PacketLossPercent(playerid);
native NetStats_ConnectionStatus(playerid);
native NetStats_GetIpPort(playerid, ip_port[], ip_port_len = sizeof(ip_port));
native NetStats_GetConnectedTime(playerid);
native SHA256_PassHash(const password[], const salt[], output[], len = sizeof(output));

// ============================================================
//  SA-MP 0.3.DL Specific: AddSimpleModel
// ============================================================
native AddSimpleModel(virtualworld, baseid, newid, const dffname[], const txdname[]);
native AddSimpleModelTimed(virtualworld, baseid, newid, const dffname[], const txdname[], timeon, timeoff);
native DownloadClientModel(playerid, modelid);

// ============================================================
//  HTTP Native
// ============================================================
native HTTP(index, type, const url[], const data[], const callback[]);

// ============================================================
//  Database Natives (SA-MP built-in)
// ============================================================
native DB:db_open(name[]);
native db_close(DB:db);
native DBResult:db_query(DB:db, query[]);
native db_free_result(DBResult:result);
native db_num_rows(DBResult:result);
native db_next_row(DBResult:result);
native db_num_fields(DBResult:result);
native db_field_name(DBResult:result, field, result[], maxlength);
native db_get_field(DBResult:result, field, result[], maxlength);
native db_get_field_assoc(DBResult:result, const field[], result[], maxlength);
native db_get_field_int(DBResult:result, field = 0);
native db_get_field_assoc_int(DBResult:result, const field[]);
native db_get_field_float(DBResult:result, field = 0);
native db_get_field_assoc_float(DBResult:result, const field[]);
native db_get_mem_handle(DB:db);
native db_get_result_mem_handle(DBResult:result);
native db_debug_openfiles();
native db_debug_openresults();

// ============================================================
//  Forwards / Callbacks (standard SA-MP)
// ============================================================
forward OnGameModeInit();
forward OnGameModeExit();
forward OnPlayerConnect(playerid);
forward OnPlayerDisconnect(playerid, reason);
forward OnPlayerSpawn(playerid);
forward OnPlayerDeath(playerid, killerid, reason);
forward OnVehicleSpawn(vehicleid);
forward OnVehicleDeath(vehicleid, killerid);
forward OnPlayerText(playerid, text[]);
forward OnPlayerCommandText(playerid, cmdtext[]);
forward OnPlayerRequestClass(playerid, classid);
forward OnPlayerEnterVehicle(playerid, vehicleid, ispassenger);
forward OnPlayerExitVehicle(playerid, vehicleid);
forward OnPlayerStateChange(playerid, newstate, oldstate);
forward OnPlayerEnterCheckpoint(playerid);
forward OnPlayerLeaveCheckpoint(playerid);
forward OnPlayerEnterRaceCheckpoint(playerid);
forward OnPlayerLeaveRaceCheckpoint(playerid);
forward OnRconCommand(cmd[]);
forward OnRconLoginAttempt(ip[], password[], success);
forward OnPlayerStreamIn(playerid, forplayerid);
forward OnPlayerStreamOut(playerid, forplayerid);
forward OnVehicleStreamIn(vehicleid, forplayerid);
forward OnVehicleStreamOut(vehicleid, forplayerid);
forward OnActorStreamIn(actorid, forplayerid);
forward OnActorStreamOut(actorid, forplayerid);
forward OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);
forward OnPlayerClickPlayer(playerid, clickedplayerid, source);
forward OnPlayerClickTextDraw(playerid, Text:clickedid);
forward OnPlayerClickPlayerTextDraw(playerid, Text:playertextid);
forward OnPlayerEditObject(playerid, playerobject, objectid, response, Float:x, Float:y, Float:z, Float:rotX, Float:rotY, Float:rotZ);
forward OnPlayerEditAttachedObject(playerid, response, index, modelid, boneid, Float:fOffsetX, Float:fOffsetY, Float:fOffsetZ, Float:fRotX, Float:fRotY, Float:fRotZ, Float:fScaleX, Float:fScaleY, Float:fScaleZ);
forward OnPlayerSelectObject(playerid, type, objectid, modelid, Float:x, Float:y, Float:z);
forward OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ);
forward OnVehicleDamageStatusUpdate(vehicleid, playerid);
forward OnUnoccupiedVehicleUpdate(vehicleid, playerid, passenger_seat, Float:new_x, Float:new_y, Float:new_z, Float:vel_x, Float:vel_y, Float:vel_z);
forward OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart);
forward OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart);
forward OnPlayerGiveDamageActor(playerid, damaged_actorid, Float:amount, weaponid, bodypart);
forward OnIncomingConnection(playerid, ip[], port);
forward OnTrailerUpdate(playerid, vehicleid);

// ============================================================
//  Standard SA-MP native aliases for compatibility
//  (open.mp renames some functions, we provide compatibility)
// ============================================================
// ChangeVehicleColours → ChangeVehicleColor alias (SA-MP uses Color)
// Already declared above as ChangeVehicleColor

// EditObject and CancelEdit are standard SA-MP natives (already declared above)
// In open.mp, EditObject = BeginObjectEditing and CancelEdit = EndObjectEditing
// We keep them as standard natives in SA-MP 0.3.DL

// ============================================================
//  open.mp-specific functions provided as STOCK fallbacks
//  These are functions that exist in open.mp but not in SA-MP 0.3.DL
//  The dl_compat.inc provides more comprehensive fallbacks
// ============================================================

// IsPlayerSpawned - already in dl_compat.inc as stock
// GetVehicleColor - already in GetVehicleColor.inc as stock
// BeginObjectEditing / EndObjectEditing - already in dl_compat.inc as stock
// frename - already in dl_compat.inc as stock
// CEF functions - already in dl_compat.inc as stock (no-op)

// ============================================================
//  Define guard macros for compatibility
//  These prevent duplicate native declarations from other includes
// ============================================================
#define _samp_included
#define _players_included
#define _vehicles_included
#define _objects_included
#define _actor_included
#define _textdraw_included
#define _dialog_included
#define _gangzone_included
#define _checkpoint_included
#define _pickup_included
#define _menu_included
#define _textlabel_included
#define _http_included
#define _sampdb_included

// ============================================================
//  open.mp-only natives provided as STOCK fallbacks
//  These functions exist only in open.mp, not in SA-MP 0.3.DL
// ============================================================

// IsValidVehicle - open.mp native, not in SA-MP 0.3.DL
// weapon-config.inc uses: native WC_IsValidVehicle(vehicleid) = IsValidVehicle;
stock IsValidVehicle(vehicleid) {
    if (vehicleid <= 0 || vehicleid >= MAX_VEHICLES) return 0;
    // Provjeri da li vozilo postoji - u SA-MP-u mozemo provjeriti model
    // Ako GetVehicleModel vrati 0, vozilo ne postoji
    return GetVehicleModel(vehicleid) != 0;
}

// MAX_ACTORS - open.mp defines this, SA-MP 0.3.DL doesn't
#if !defined MAX_ACTORS
    const MAX_ACTORS = 1000;
    #define MAX_ACTORS 1000
#endif
