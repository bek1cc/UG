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

// Forward declarations for stock ac_ functions
// (These are defined later in the gamemode)
forward ac_KillTimer(timerid);
forward ac_SetPlayerCheckpoint(playerid, Float:x, Float:y, Float:z, Float:size);
forward ac_DisablePlayerCheckpoint(playerid);
forward ac_SetPlayerWantedLevel(playerid, broj);
forward ac_IsNotWeaponNoAmmo(weaponid);
forward ac_IsGiveVehicleWeapon(weaponid);
forward ac_SetPlayerHealth(playerid, Float:Zivot);
forward ac_SetPlayerArmour(playerid, Float:armor);
forward ac_AttachTrailerToVehicle(trailerid, vehicleid);
forward ac_PutPlayerInVehicle(playerid, vehicleid, seatid);
forward ac_RemovePlayerFromVehicle(playerid);
forward ac_SetVehicleHealth(vehicleid, Float:vHealth);
forward ac_RepairVehicle(playerid, vehicleid);
