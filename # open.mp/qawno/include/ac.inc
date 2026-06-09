/*==============================================================
    ac.inc - Anti-Cheat Wrapper Include
    Provides forward declarations for ac_* functions
    that are defined in the gamemode.
==============================================================*/

#if defined _INC_ac
    #endinput
#endif
#define _INC_ac

// Forward declarations for public ac_ functions
forward ac_WC_CreateVehicle(scriptid, tip, orga, posao, firma, modelid, Float:x, Float:y, Float:z, Float:angle, color1, color2, respawn_delay);
forward ac_WC_DestroyVehicle(vehicleid);
forward ac_SetPlayerPos(playerid, Float:x, Float:y, Float:z);
forward ac_SetSpawnInfo(playerid);
forward ac_SetVehiclePos(vehicleid, Float:x, Float:y, Float:z);
forward ac_Kick(playerid);

// Native-style declarations for stock ac_ functions
// (These are defined later in the gamemode)
native ac_KillTimer(timerid);
native ac_SetPlayerCheckpoint(playerid, Float:x, Float:y, Float:z, Float:size);
native ac_DisablePlayerCheckpoint(playerid);
native ac_SetPlayerWantedLevel(playerid, broj);
native ac_IsNotWeaponNoAmmo(weaponid);
native ac_IsGiveVehicleWeapon(weaponid);
native ac_SetPlayerHealth(playerid, Float:Zivot);
native ac_SetPlayerArmour(playerid, Float:armor);
native ac_AttachTrailerToVehicle(trailerid, vehicleid);
native ac_PutPlayerInVehicle(playerid, vehicleid, seatid);
native ac_RemovePlayerFromVehicle(playerid);
native ac_SetVehicleHealth(vehicleid, Float:vHealth);
native ac_RepairVehicle(playerid, vehicleid);
