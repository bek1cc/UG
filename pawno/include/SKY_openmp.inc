// ============================================================
//  SKY_openmp.inc - Open.mp compatible SKY plugin shim
//
//  SKY plugin uses memory hacking which doesn't work on open.mp
//  This file replaces all SKY native functions with stock
//  functions that use open.mp compatible alternatives or no-ops
//
//  USAGE: Add -DOPENMP_BUILD to your compiler options
//         Or define OPENMP_BUILD before including this file
//  This file is automatically included by SKY.inc when
//  OPENMP_BUILD is defined
// ============================================================

#if defined _INC_SKY
    #endinput
#endif
#define _INC_SKY

// Flag so code can check if running on open.mp
#define SKY_OPENMP_SHIM 1

// SpawnPlayerForWorld - On open.mp, just use SpawnPlayer
stock SpawnPlayerForWorld(playerid)
{
    SpawnPlayer(playerid);
}

// FreezeSyncData - No open.mp equivalent, just do nothing
stock FreezeSyncData(playerid, bool:toggle)
{
    #pragma unused playerid, toggle
}

// SetFakeHealth - No direct equivalent, just set real health
stock SetFakeHealth(playerid, Float:health)
{
    SetPlayerHealth(playerid, health);
}

// SetFakeArmour - No direct equivalent, just set real armour
stock SetFakeArmour(playerid, Float:armour)
{
    SetPlayerArmour(playerid, armour);
}

// SetFakeFacingAngle - Use SetPlayerFacingAngle instead
stock SetFakeFacingAngle(playerid, Float:angle)
{
    SetPlayerFacingAngle(playerid, angle);
}

// SetKnifeSync - No equivalent, do nothing
stock SetKnifeSync(toggle)
{
    #pragma unused toggle
}

// SendDeath - No equivalent, do nothing
stock SendDeath(playerid)
{
    #pragma unused playerid
}

// SetLastAnimationData - No equivalent, do nothing
stock SetLastAnimationData(playerid, data)
{
    #pragma unused playerid, data
}

// SendLastSyncData - No equivalent, do nothing
stock SendLastSyncData(playerid, toplayerid, animation = 0)
{
    #pragma unused playerid, toplayerid, animation
}

// ClearAnimationsForPlayer - Use ClearAnimations for the player
stock ClearAnimationsForPlayer(playerid, forplayerid)
{
    #pragma unused forplayerid
    ClearAnimations(playerid);
}

// SetDisableSyncBugs - No equivalent, do nothing
stock SetDisableSyncBugs(toggle)
{
    #pragma unused toggle
}

// SetInfiniteAmmoSync - No equivalent, do nothing
stock SetInfiniteAmmoSync(playerid, toggle)
{
    #pragma unused playerid, toggle
}

// SetKeySyncBlocked - No equivalent, do nothing
stock SetKeySyncBlocked(playerid, toggle)
{
    #pragma unused playerid, toggle
}

// TextDrawSetPosition - No direct equivalent, do nothing
stock TextDrawSetPosition(Text:text, Float:x, Float:y)
{
    #pragma unused text, x, y
}

// PlayerTextDrawSetPosition - No direct equivalent, do nothing
stock PlayerTextDrawSetPosition(playerid, PlayerText:text, Float:x, Float:y)
{
    #pragma unused playerid, text, x, y
}

// TextDrawSetStringForPlayer - No direct equivalent, do nothing
stock TextDrawSetStringForPlayer(Text:text, playerid, const string[])
{
    #pragma unused text, playerid, string
}
