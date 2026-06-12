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
// Mod "Dog MOD serkan1969" - svi folderi imaju iste fajlove: wmyva2.dff / wmyva2.txd
// Za sad koristimo Siberian Husky za test, kasnije dodajemo vise pasmina

// ===== open.mp NATIVE za CustomModels =====
// AddSimpleModel je open.mp native, ali stariji kompajler ga ne prepoznaje
// Deklariramo ga ovdje ako vec nije definiran
#if !defined _AddSimpleModel_included
    native AddSimpleModel(virtualworld, baseid, newid, const dffpath[], const txdpath[]);
    #define _AddSimpleModel_included
#endif

// Custom model ID (-1000 do -29999 za custom objekte u open.mp)
#define DOG_MODEL_ID        -1000

// wmyva2 je skin ID 138 u GTA SA - to je base model koji mod zamjenjuje
// Ali mi koristimo AddSimpleModel sa base objektom 1608 (ajkula) kao placeholder
// jer pas mod zamjenjuje PED skin, a mi ga koristimo kao objekt
#define DOG_BASE_MODEL      1608        // Fallback base model (ajkula)

// DFF/TXD imena - fajlovi iz moda (staviti u artwork/ folder)
#define DOG_MODEL_DFF       "wmyva2.dff"
#define DOG_MODEL_TXD       "wmyva2.txd"

// Dog types (za sad samo HUSKY za test, ostalo kasnije)
#define DOG_TYPE_NONE       0
#define DOG_TYPE_HUSKY      1
// Dodati kasnije:
// #define DOG_TYPE_ROTTWEILER 2
// #define DOG_TYPE_PITBULL    3
// #define DOG_TYPE_SHEPHERD   4
// #define DOG_TYPE_GOLDEN     5

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

// Dog names per type - GLOBAL (ne static, da cef_dog.inc i gamemode mogu pristupiti)
new const DogTypeNames[][] = {
        {"Nema"},
        {"Siberian Husky"}
};

// Per-player dog data - GLOBAL (ne static, da cef_dog.inc i gamemode mogu pristupiti)
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

// Forward timer callbacks
forward Dog_UpdateTimer(playerid);
forward Dog_RespawnTimer(playerid);

// ===== MODEL REGISTRATION =====
stock Dog_OnGameModeInit()
{
        // Register custom dog model via open.mp CustomModels
        // open.mp automatski skida DFF/TXD fajlove iz artwork/ foldera igracima!
        // Fajlovi: open.mp/scriptfiles/artwork/wmyva2.dff i wmyva2.txd
        new ret = AddSimpleModel(-1, DOG_BASE_MODEL, DOG_MODEL_ID, DOG_MODEL_DFF, DOG_MODEL_TXD);
        if(ret)
        {
                DogModelLoaded = true;
                printf("[DOG] Custom dog model registriran! (Model ID: %d, Base: %d, DFF: %s, TXD: %s)", DOG_MODEL_ID, DOG_BASE_MODEL, DOG_MODEL_DFF, DOG_MODEL_TXD);
                printf("[DOG] Igraci ce automatski skinuti model - nema rucnog downloada!");
        }
        else
        {
                DogModelLoaded = false;
                printf("[DOG] Custom dog model NIJE registriran! Koristim fallback model %d", DOG_BASE_MODEL);
                printf("[DOG] Provjeri da li su wmyva2.dff i wmyva2.txd u artwork/ folderu!");
        }
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

        // Create dog object - koristi custom model ako je ucitan, inace fallback
        new modelToUse = DogModelLoaded ? DOG_MODEL_ID : DOG_BASE_MODEL;
        DogObject[playerid] = CreateObject(modelToUse, x, y, z - 1.0, 0.0, 0.0, a);

        if(DogObject[playerid] == INVALID_OBJECT_ID)
        {
                printf("[DOG] Failed to create dog object for player %d", playerid);
                return 0;
        }

        // Set dog color based on type (material coloring)
        new color = DOG_COLOR_HUSKY;
        SetObjectMaterial(DogObject[playerid], 0, -1, "none", "none", color);

        // Create 3D label iznad psa
        new labelStr[64];
        format(labelStr, sizeof(labelStr), "[%s]\n%s", name, DogTypeNames[type]);
        DogLabel[playerid] = Create3DTextLabel(labelStr, 0xFFFFFFFF, x, y, z + 0.8, 10.0, 0, 0);

        // Set dog data
        DogActive[playerid] = true;
        DogType[playerid] = type;
        DogState[playerid] = DOG_STATE_FOLLOW;
        format(DogName[playerid], 32, "%s", name);
        DogHP[playerid] = 100;
        DogSkinColor[playerid] = color;
        DogStayX[playerid] = x;
        DogStayY[playerid] = y;
        DogStayZ[playerid] = z;

        // Start follow timer (every 500ms)
        DogTimer[playerid] = SetTimerEx("Dog_UpdateTimer", 500, true, "i", playerid);

        printf("[DOG] Player %d spawned dog: %s (%s) [Model: %d]", playerid, name, DogTypeNames[type], modelToUse);
        return 1;
}

stock Dog_Destroy(playerid)
{
        if(!DogActive[playerid]) return 0;

        if(DogObject[playerid] != INVALID_OBJECT_ID)
        {
                DestroyObject(DogObject[playerid]);
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
        new Float:dist, Float:angle;

        GetPlayerPos(playerid, px, py, pz);

        if(DogState[playerid] == DOG_STATE_STAY)
        {
                dx = DogStayX[playerid];
                dy = DogStayY[playerid];
                dz = DogStayZ[playerid];
        }
        else if(DogState[playerid] == DOG_STATE_SIT || DogState[playerid] == DOG_STATE_IDLE)
        {
                // Ne mrdaj kad sjedi / idle
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
        GetObjectPos(DogObject[playerid], cx, cy, cz);

        // Calculate distance
        dist = floatsqroot((dx-cx)*(dx-cx) + (dy-cy)*(dy-cy));

        // Follow distance and speed
        new Float:followDist = 2.0;
        new Float:speed = 5.0; // MoveObject speed

        if(DogState[playerid] == DOG_STATE_RUN)
        {
                followDist = 1.5;
                speed = 10.0;
        }

        if(dist > followDist)
        {
                // Calculate angle towards target
                angle = atan2(dy - cy, dx - cx);

                // Move towards target with proper speed
                new Float:moveX, Float:moveY;
                moveX = cx + (dx - cx) * 0.3; // Lerp 30% ka targetu
                moveY = cy + (dy - cy) * 0.3;

                // Get ground Z
                new Float:targetZ = (DogState[playerid] == DOG_STATE_STAY) ? DogStayZ[playerid] : pz;

                // Smooth Z transition
                new Float:zDiff = targetZ - cz;
                if(zDiff > 1.0) targetZ = cz + 0.5;
                else if(zDiff < -1.0) targetZ = cz - 0.5;

                MoveObject(DogObject[playerid], moveX, moveY, targetZ - 1.0, speed, 0.0, 0.0, angle);

                // Update 3D label text
                if(DogLabel[playerid] != INVALID_3DTEXT_ID)
                {
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
                }
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
        new Float:x, Float:y, Float:z;
        GetObjectPos(DogObject[playerid], x, y, z);
        DogStayX[playerid] = x;
        DogStayY[playerid] = y;
        DogStayZ[playerid] = z;
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
        SetObjectPos(DogObject[playerid], x + 2.0, y + 2.0, z - 1.0);
        SendClientMessage(playerid, 0x22c55eFF, " Tvoj pas je respawnovao!");
}
