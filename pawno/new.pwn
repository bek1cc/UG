/*
 *
 *      Drop FilterScript for SA-MP
 *          Creator: 002
 *              Date:20.08.2019
 *
 *
*/

//--//
#include <a_samp>
#include <streamer>
//--//

enum DropI {
    ID,
    Oruzije,
    Ammo
}

new DropInfo[MAX_PICKUPS][DropI];
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
public OnFilterScriptInit()
{
    ReloadPickups();
    print("DROP >> Drop system uspesno ucitan!");
    return(true);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
public OnFilterScriptExit()
{
    ReloadPickups();
    BrisiPikapove();
    return(true);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
public OnPlayerDeath(playerid)
{
    Dropgun(playerid);
    return(true);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
public OnPlayerPickUpDynamicPickup(playerid, pickupid) {
    new string[117];
    new hladokrvooruzije[32];
    for(new i = 0; i < MAX_PICKUPS; i++) {
        if(pickupid == DropInfo[i][ID] && DropInfo[i][ID] != -1)
        {
            DestroyDynamicPickup(DropInfo[i][ID]);
            GivePlayerWeapon(playerid, DropInfo[i][Oruzije], DropInfo[i][Ammo]);
            GetWeaponName(DropInfo[i][Oruzije], hladokrvooruzije, sizeof(hladokrvooruzije));
            format(string, sizeof(string), "{6E0A35}DROP >>{FFFFFF} Pokupio si {6E0A35}%s{FFFFFF} ( {6E0A35}%d{FFFFFF} ammo ).", hladokrvooruzije, DropInfo[i][Ammo]);
            SendClientMessage(playerid, -1, string);
            DropInfo[i][Oruzije] = 0;
            DropInfo[i][ID] = -1;
            PlayerPlaySound(playerid, 1150, 0.0, 0.0, 10.0);
        }
    }
    return 1;
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
Dropgun(playerid)
{
    new Float: Pos[3],
    gunid,
    ammo,
    id;
    GetPlayerPos(playerid, Pos[0], Pos[1], Pos[2]);

    for(new i = 0; i < 12; i++) {
        GetPlayerWeaponData(playerid, i, gunid, ammo);
        if(gunid != 0) {
            id = IDCheck();
            DropInfo[id][ID] = CreateDynamicPickup(IDPickupa(gunid), 23, Pos[0], Pos[1]+2, Pos[2], -1);
            DropInfo[id][Oruzije] = gunid;
            DropInfo[id][Ammo] = ammo;
        }
    }
    ResetPlayerWeapons(playerid);
    //--//
    return(true);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
IDCheck() {
    for(new i = 0; i < MAX_PICKUPS; i++) {
        if(DropInfo[i][ID] == -1) return i;
    }
    return(true);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
IDPickupa(id)
{
    switch(id)
    {
        case 1: return 331;
        case 2: return 332;
        case 3: return 333;
        case 4: return 335;
        case 5: return 334;
        case 6: return 335;
        case 7: return 336;
        case 10: return 321;
        case 11: return 322;
        case 12: return 323;
        case 13: return 324;
        case 14: return 325;
        case 15: return 326;
        case 23: return 347;
        case 24: return 348;
        case 25: return 349;
        case 26: return 350;
        case 27: return 351;
        case 28: return 352;
        case 29: return 353;
        case 30: return 355;
        case 31: return 356;
        case 32: return 372;
        case 33: return 357;
        case 34: return 358;
        case 41: return 365;
        case 42: return 366;
        case 43: return 367;
    }
    return(true);
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
BrisiPikapove() {
    for(new i = 0; i < MAX_PICKUPS; i++)
    {
        if(DropInfo[i][ID] != -1)
        {
            DestroyDynamicPickup(DropInfo[i][ID]);
            DropInfo[i][ID] = -1;
        }
    }
    return(true);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
ReloadPickups() {
    for(new i = 0; i < MAX_PICKUPS; i++) {
        if(DropInfo[i][ID] != -1) DropInfo[i][ID] = -1;
    }
    return(true);
}

/*
 *
 *
 *
 *          End of filterscript
 */
