#if defined _INC_dog
        #endinput
#endif
#define _INC_dog

// ===== PAS SISTEM (Dog/Pet System) =====
// Koristi open.mp CustomModels modul za automatski download modela
// Igraci ne moraju nista rucno skidati - model se downloada automatski!
//
// SETUP: Stavi wmyva2.dff i wmyva2.txd u:
//   open.mp/scriptfiles/artwork/
//
// Kad DFF/TXD fajlovi nisu dostupni, koristi se base model 3526 (CJ_BEAR_DOG)
// kao placeholder - igraci ce vidjeti mali plisani pas/medo objekt.

// ===== open.mp NATIVE za CustomModels =====
#if !defined _AddSimpleModel_included
    native AddSimpleModel(virtualworld, baseid, newid, const dff[], const txd[]);
    #define _AddSimpleModel_included
#endif

// Custom object model ID (-1000) za pas model
// Base model 3526 = CJ_BEAR_DOG (mali plisani pas/medo u GTA SA)
// Kad se dodaju custom DFF/TXD u artwork/, AddSimpleModel ce zamijeniti base sa custom modelom
#define DOG_MODEL_ID       -1000
#define DOG_BASE_MODEL     3526    // CJ_BEAR_DOG - placeholder model

// DFF/TXD imena - fajlovi iz moda (staviti u artwork/ folder)
#define DOG_MODEL_DFF      "wmyva2.dff"
#define DOG_MODEL_TXD      "wmyva2.txd"

// Dog types (za sad samo HUSKY za test, ostalo kasnije)
#define DOG_TYPE_NONE       0
#define DOG_TYPE_HUSKY      1

// Dog states
#define DOG_STATE_NONE      0
#define DOG_STATE_IDLE      0
#define DOG_STATE_FOLLOW    1
#define DOG_STATE_SIT       2
#define DOG_STATE_STAY      3
#define DOG_STATE_RUN       4
#define DOG_STATE_DEAD      5

// Dog CEF browser ID (koristi se i u dog.inc i u cef_dog.inc)
#define DOG_BROWSER_ID      82

// Dog colors per type (RGBA hex) - za material coloring
#define DOG_COLOR_HUSKY         0xC8BDB0FF

// Dog names per type - GLOBAL
new const DogTypeNames[][] = {
        {"Nema"},
        {"Siberian Husky"}
};

// Per-player dog data - GLOBAL (PlayerObject pristup, pouzdaniji u open.mp)
new DogObject[MAX_PLAYERS] = {INVALID_OBJECT_ID, ...};
new DogLabel[MAX_PLAYERS] = {INVALID_3DTEXT_ID, ...};
new bool:DogActive[MAX_PLAYERS];
new DogType[MAX_PLAYERS];
new DogState[MAX_PLAYERS];
new DogName[MAX_PLAYERS][32];
new DogTimer[MAX_PLAYERS] = {-1, ...};
new Float:DogStayX[MAX_PLAYERS];
new Float:DogStayY[MAX_PLAYERS];
new Float:DogStayZ[MAX_PLAYERS];
new DogHP[MAX_PLAYERS];
new DogSkinColor[MAX_PLAYERS];
new bool:DogModelLoaded = false;
new DogModelToUse = DOG_BASE_MODEL;

// Forward timer callbacks
forward Dog_UpdateTimer(playerid);
forward Dog_RespawnTimer(playerid);

// ===== MODEL REGISTRATION =====
stock Dog_OnGameModeInit()
{
        // Register custom dog model via open.mp CustomModels (AddSimpleModel)
        // AddSimpleModel registrira OBJECT model sa custom ID
        // open.mp automatski skida DFF/TXD fajlove iz artwork/ foldera igracima!
        // Ako DFF/TXD ne postoje, igraci ce vidjeti base model (3526 = CJ_BEAR_DOG)
        AddSimpleModel(-1, DOG_BASE_MODEL, DOG_MODEL_ID, DOG_MODEL_DFF, DOG_MODEL_TXD);

        DogModelLoaded = true;
        DogModelToUse = DOG_MODEL_ID;
        printf("[DOG] Custom dog model registriran! (Model ID: %d, Base: %d)", DOG_MODEL_ID, DOG_BASE_MODEL);
        printf("[DOG] Ako DFF/TXD ne postoje u artwork/, igraci ce vidjeti placeholder model (CJ_BEAR_DOG)");
        return 1;
}

stock Dog_OnPlayerDisconnect(playerid)
{
        if(DogActive[playerid])
        {
                Dog_Destroy(playerid);
        }
}

// ===== SPAWN / DESPAWN =====
stock Dog_Spawn(playerid, type = DOG_TYPE_HUSKY, name[] = "Buddy")
{
        if(DogActive[playerid]) return 0;
        if(!(1 <= type <= 1)) return 0; // Za sad samo HUSKY (1)

        new Float:x, Float:y, Float:z, Float:a;
        GetPlayerPos(playerid, x, y, z);
        GetPlayerFacingAngle(playerid, a);

        // Offset spawn position behind player
        x += 2.0 * floatsin(-a, degrees);
        y += 2.0 * floatcos(-a, degrees);

        // Create dog as PlayerObject - automatski je u istom virtualnom svijetu kao igrac
        // Koristimo custom model ID (-1000) ako je ucitan, inace base model (3526)
        DogObject[playerid] = CreatePlayerObject(playerid, DogModelToUse, x, y, z - 0.3, 0.0, 0.0, a, 300.0);

        if(DogObject[playerid] == INVALID_OBJECT_ID)
        {
                // Fallback - pokusaj sa base modelom direktno
                printf("[DOG] Custom model %d failed, trying base model %d", DogModelToUse, DOG_BASE_MODEL);
                DogObject[playerid] = CreatePlayerObject(playerid, DOG_BASE_MODEL, x, y, z - 0.3, 0.0, 0.0, a, 300.0);
        }

        if(DogObject[playerid] == INVALID_OBJECT_ID)
        {
                printf("[DOG] Failed to create dog object for player %d", playerid);
                return 0;
        }

        // Create 3D label iznad psa
        new labelStr[64];
        format(labelStr, sizeof(labelStr), "[%s]\n%s", name, DogTypeNames[type]);
        DogLabel[playerid] = Create3DTextLabel(labelStr, 0xFFFFFFFF, x, y, z + 0.8, 10.0, GetPlayerVirtualWorld(playerid), 0);

        // Set dog data
        DogActive[playerid] = true;
        DogType[playerid] = type;
        DogState[playerid] = DOG_STATE_FOLLOW;
        format(DogName[playerid], 32, "%s", name);
        DogHP[playerid] = 100;
        DogSkinColor[playerid] = DOG_COLOR_HUSKY;
        DogStayX[playerid] = x;
        DogStayY[playerid] = y;
        DogStayZ[playerid] = z;

        // Start follow timer (every 500ms)
        DogTimer[playerid] = SetTimerEx("Dog_UpdateTimer", 500, true, "i", playerid);

        printf("[DOG] Player %d spawned dog: %s (%s) [Model: %d]", playerid, name, DogTypeNames[type], DogModelToUse);
        return 1;
}

stock Dog_Destroy(playerid)
{
        if(!DogActive[playerid]) return 0;

        if(DogObject[playerid] != INVALID_OBJECT_ID)
        {
                DestroyPlayerObject(playerid, DogObject[playerid]);
                DogObject[playerid] = INVALID_OBJECT_ID;
        }
        if(DogLabel[playerid] != INVALID_3DTEXT_ID)
        {
                Delete3DTextLabel(DogLabel[playerid]);
                DogLabel[playerid] = INVALID_3DTEXT_ID;
        }
        if(DogTimer[playerid] != -1)
        {
                KillTimer(DogTimer[playerid]);
                DogTimer[playerid] = -1;
        }

        DogActive[playerid] = false;
        DogState[playerid] = DOG_STATE_NONE;
        DogHP[playerid] = 0;

        return 1;
}

// ===== DOG AI / MOVEMENT =====
public Dog_UpdateTimer(playerid)
{
        if(!DogActive[playerid] || !IsPlayerConnected(playerid)) return;
        if(DogState[playerid] == DOG_STATE_DEAD) return;
        if(DogObject[playerid] == INVALID_OBJECT_ID) return;

        new Float:px, Float:py, Float:pz;
        new Float:dx, Float:dy, Float:dz;
        new Float:dist;

        GetPlayerPos(playerid, px, py, pz);

        if(DogState[playerid] == DOG_STATE_STAY)
        {
                dx = DogStayX[playerid];
                dy = DogStayY[playerid];
                dz = DogStayZ[playerid];
        }
        else if(DogState[playerid] == DOG_STATE_SIT || DogState[playerid] == DOG_STATE_IDLE)
        {
                // Ne mrdaj kad sjedi / idle - samo azuriraj label
                if(DogLabel[playerid] != INVALID_3DTEXT_ID)
                {
                        new Float:ax, Float:ay, Float:az;
                        GetPlayerObjectPos(playerid, DogObject[playerid], ax, ay, az);
                        new labelStr[80];
                        new stateStr[16];
                        switch(DogState[playerid])
                        {
                                case DOG_STATE_SIT: stateStr = "Sjedi";
                                default: stateStr = "";
                        }
                        format(labelStr, sizeof(labelStr), "[%s] %s\n~y~%s~w~ | HP: %d", DogName[playerid], DogTypeNames[DogType[playerid]], stateStr, DogHP[playerid]);
                        Update3DTextLabelText(DogLabel[playerid], 0xFFFFFFFF, labelStr);
                        Delete3DTextLabel(DogLabel[playerid]);
                        DogLabel[playerid] = Create3DTextLabel(labelStr, 0xFFFFFFFF, ax, ay, az + 0.8, 10.0, GetPlayerVirtualWorld(playerid), 0);
                }
                return;
        }
        else // FOLLOW or RUN
        {
                dx = px;
                dy = py;
                dz = pz;
        }

        // Get current dog position
        new Float:cx, Float:cy, Float:cz;
        GetPlayerObjectPos(playerid, DogObject[playerid], cx, cy, cz);

        // Calculate distance
        dist = floatsqroot((dx-cx)*(dx-cx) + (dy-cy)*(dy-cy));

        // Follow distance
        new Float:followDist = 2.0;

        if(DogState[playerid] == DOG_STATE_RUN)
        {
                followDist = 1.5;
        }

        if(dist > followDist)
        {
                // Lerp towards target
                new Float:moveX, Float:moveY, Float:moveZ;
                new Float:lerpFactor = (DogState[playerid] == DOG_STATE_RUN) ? 0.35 : 0.2;

                moveX = cx + (dx - cx) * lerpFactor;
                moveY = cy + (dy - cy) * lerpFactor;
                moveZ = (DogState[playerid] == DOG_STATE_STAY) ? DogStayZ[playerid] : pz;

                // Calculate facing angle towards target
                new Float:angle = atan2(dy - cy, dx - cx);

                // Move player object to new position
                SetPlayerObjectPos(playerid, DogObject[playerid], moveX, moveY, moveZ - 0.3);
                SetPlayerObjectRot(playerid, DogObject[playerid], 0.0, 0.0, angle);
        }

        // Update 3D label position
        if(DogLabel[playerid] != INVALID_3DTEXT_ID)
        {
                new Float:ax, Float:ay, Float:az;
                GetPlayerObjectPos(playerid, DogObject[playerid], ax, ay, az);

                new labelStr[80];
                new stateStr[16];
                switch(DogState[playerid])
                {
                        case DOG_STATE_FOLLOW: stateStr = "Slijedi";
                        case DOG_STATE_RUN:    stateStr = "Trci";
                        case DOG_STATE_STAY:   stateStr = "Ceka";
                        case DOG_STATE_SIT:    stateStr = "Sjedi";
                        default: stateStr = "";
                }
                format(labelStr, sizeof(labelStr), "[%s] %s\n~y~%s~w~ | HP: %d", DogName[playerid], DogTypeNames[DogType[playerid]], stateStr, DogHP[playerid]);
                Update3DTextLabelText(DogLabel[playerid], 0xFFFFFFFF, labelStr);
                Delete3DTextLabel(DogLabel[playerid]);
                DogLabel[playerid] = Create3DTextLabel(labelStr, 0xFFFFFFFF, ax, ay, az + 0.8, 10.0, GetPlayerVirtualWorld(playerid), 0);
        }
}

// ===== DOG COMMANDS =====

// /pas - Opens CEF dog panel
CMD:pas(playerid, params[])
{
        if(!PlayerLogged[playerid]) return SendErrorMessage(playerid, "Morate biti ulogovani!");

        // Toggle CEF panel
        if(CEF_DogPanelIsOpen(playerid))
        {
                CEF_DogPanelClose(playerid);
        }
        else
        {
                CEF_DogPanelOpen(playerid);
        }
        return 1;
}

// /pasinfo - Opens CEF dog info panel (same as /pas but explicit)
CMD:pasinfo(playerid, params[])
{
        if(!PlayerLogged[playerid]) return SendErrorMessage(playerid, "Morate biti ulogovani!");
        CEF_DogPanelOpen(playerid);
        return 1;
}

// Quick commands (also update CEF panel if open)
CMD:pasfollow(playerid)
{
        if(!DogActive[playerid]) return SendErrorMessage(playerid, "Nemas psa! Kucaj /pas da ga dohvatis.");
        if(DogState[playerid] == DOG_STATE_DEAD) return SendErrorMessage(playerid, "Tvoj pas je mrtav!");
        DogState[playerid] = DOG_STATE_FOLLOW;
        SendClientMessage(playerid, 0x22c55eFF, " Pas te slijedi!");
        CEF_DogUpdateState(playerid);
        return 1;
}

CMD:passjedi(playerid)
{
        if(!DogActive[playerid]) return SendErrorMessage(playerid, "Nemas psa! Kucaj /pas da ga dohvatis.");
        if(DogState[playerid] == DOG_STATE_DEAD) return SendErrorMessage(playerid, "Tvoj pas je mrtav!");
        DogState[playerid] = DOG_STATE_SIT;
        SendClientMessage(playerid, 0xeab308FF, " Pas sjedi.");
        CEF_DogUpdateState(playerid);
        return 1;
}

CMD:pascekaj(playerid)
{
        if(!DogActive[playerid]) return SendErrorMessage(playerid, "Nemas psa! Kucaj /pas da ga dohvatis.");
        if(DogState[playerid] == DOG_STATE_DEAD) return SendErrorMessage(playerid, "Tvoj pas je mrtav!");
        if(DogObject[playerid] != INVALID_OBJECT_ID)
        {
                new Float:x, Float:y, Float:z;
                GetPlayerObjectPos(playerid, DogObject[playerid], x, y, z);
                DogStayX[playerid] = x;
                DogStayY[playerid] = y;
                DogStayZ[playerid] = z;
        }
        DogState[playerid] = DOG_STATE_STAY;
        SendClientMessage(playerid, 0x06b6d4FF, " Pas ceka ovdje!");
        CEF_DogUpdateState(playerid);
        return 1;
}

CMD:pastrci(playerid)
{
        if(!DogActive[playerid]) return SendErrorMessage(playerid, "Nemas psa! Kucaj /pas da ga dohvatis.");
        if(DogState[playerid] == DOG_STATE_DEAD) return SendErrorMessage(playerid, "Tvoj pas je mrtav!");
        DogState[playerid] = DOG_STATE_RUN;
        SendClientMessage(playerid, 0xef4444FF, " Pas trci za tobom!");
        CEF_DogUpdateState(playerid);
        return 1;
}

CMD:pasotpusti(playerid)
{
        if(!DogActive[playerid]) return SendErrorMessage(playerid, "Nemas psa!");
        Dog_Destroy(playerid);
        SendClientMessage(playerid, 0xFFAA00FF, " Otpustio si psa.");
        // Update CEF panel to show no-dog state
        if(CEF_DogPanelIsOpen(playerid))
        {
                CEF_EmitEvent(playerid, DOG_BROWSER_ID, "dog:init", CEF_STR(""), CEF_STR(""), CEF_INT(0), CEF_STR(""), CEF_INT(0));
        }
        return 1;
}

// Dog damage (when player takes damage, dog can help defend)
stock Dog_OnPlayerDamage(playerid, issuerid, Float:amount)
{
        if(!DogActive[playerid]) return;
        if(DogState[playerid] == DOG_STATE_DEAD) return;

        // If someone attacks the dog owner, dog takes some damage
        if(issuerid != INVALID_PLAYER_ID && DogActive[playerid])
        {
                // Dog takes 10% of the damage meant for owner
                DogHP[playerid] -= floatround(amount * 0.1);
                if(DogHP[playerid] <= 0)
                {
                        DogHP[playerid] = 0;
                        DogState[playerid] = DOG_STATE_DEAD;
                        SendClientMessage(playerid, 0xFF4444FF, " Tvoj pas je ubijen! Respawn za 30 sekundi...");
                        SetTimerEx("Dog_RespawnTimer", 30000, false, "i", playerid);
                }
        }
}

public Dog_RespawnTimer(playerid)
{
        if(!IsPlayerConnected(playerid)) return;
        if(!DogActive[playerid]) return;

        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);

        DogHP[playerid] = 100;
        DogState[playerid] = DOG_STATE_FOLLOW;
        if(DogObject[playerid] != INVALID_OBJECT_ID)
        {
                SetPlayerObjectPos(playerid, DogObject[playerid], x + 2.0, y + 2.0, z - 0.3);
        }
        SendClientMessage(playerid, 0x22c55eFF, " Tvoj pas je respawnovao!");
}
