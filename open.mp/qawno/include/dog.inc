#if defined _INC_dog
        #endinput
#endif
#define _INC_dog

// ===== PAS SISTEM (Dog/Pet System) =====
// Koristi open.mp CustomModels modul za automatski download modela
// Igraci ne moraju nista rucno skidati - model se downloada automatski!

// Custom model ID (start from -1000 for custom objects)
#define DOG_MODEL_ID        -1000
#define DOG_BASE_MODEL      1608        // Fallback: shark model (ajkula) - dok se ne doda pravi dog.dff/dog.txd
#define DOG_MODEL_DFF       "dog.dff"
#define DOG_MODEL_TXD       "dog.txd"

// Dog types
#define DOG_TYPE_NONE       0
#define DOG_TYPE_ROTTWEILER 1
#define DOG_TYPE_HUSKY      2
#define DOG_TYPE_GOLDEN     3
#define DOG_TYPE_PITBULL    4
#define DOG_TYPE_SHEPHERD   5

// Dog states
#define DOG_STATE_IDLE      0
#define DOG_STATE_FOLLOW    1
#define DOG_STATE_SIT       2
#define DOG_STATE_STAY      3
#define DOG_STATE_RUN       4
#define DOG_STATE_DEAD      5

// Dog colors per type (RGBA hex)
#define DOG_COLOR_ROTTWEILER    0x3B2210FF
#define DOG_COLOR_HUSKY         0xC8BDB0FF
#define DOG_COLOR_GOLDEN        0xC4962AFF
#define DOG_COLOR_PITBULL       0x6B5440FF
#define DOG_COLOR_SHEPHERD      0x5A4A3AFF

// Dog names per type
static const DogTypeNames[][] = {
        {"Nema"},
        {"Rottweiler"},
        {"Husky"},
        {"Golden Retriever"},
        {"Pitbull"},
        {"Njemacki Ovcarski"}
};

// Per-player dog data
static DogObject[MAX_PLAYERS] = {INVALID_OBJECT_ID, ...};
static DogLabel[MAX_PLAYERS] = {INVALID_3DTEXT_ID, ...};
static bool:DogActive[MAX_PLAYERS];
static DogType[MAX_PLAYERS];
static DogState[MAX_PLAYERS];
static DogName[MAX_PLAYERS][32];
static DogTimer[MAX_PLAYERS] = {-1, ...};
static Float:DogStayX[MAX_PLAYERS];
static Float:DogStayY[MAX_PLAYERS];
static Float:DogStayZ[MAX_PLAYERS];
static DogHP[MAX_PLAYERS];
static DogSkinColor[MAX_PLAYERS];
static bool:DogModelLoaded = false;

// Forward timer callback
forward Dog_UpdateTimer(playerid);
forward Dog_RespawnTimer(playerid);

// ===== MODEL REGISTRATION =====
stock Dog_OnGameModeInit()
{
        // Register custom dog model via open.mp CustomModels
        // This automatically downloads the model to players - no manual install needed!
        // If dog.dff/dog.txd exist in artwork/ folder, they'll be used
        // If not, the base model (shark 1608) will be used as fallback
        if(AddSimpleModel(-1, DOG_BASE_MODEL, DOG_MODEL_ID, DOG_MODEL_DFF, DOG_MODEL_TXD))
        {
                DogModelLoaded = true;
                printf("[DOG] Custom dog model registered (ID: %d, base: %d)", DOG_MODEL_ID, DOG_BASE_MODEL);
        }
        else
        {
                DogModelLoaded = false;
                printf("[DOG] WARNING: Custom dog model failed to register, using base model %d", DOG_BASE_MODEL);
        }
}

stock Dog_OnPlayerDisconnect(playerid)
{
        if(DogActive[playerid])
        {
                Dog_Destroy(playerid);
        }
}

// ===== SPAWN / DESPAWN =====
stock Dog_Spawn(playerid, type = DOG_TYPE_ROTTWEILER, name[] = "Buddy")
{
        if(DogActive[playerid]) return 0;
        if(!(1 <= type <= 5)) return 0;

        new Float:x, Float:y, Float:z, Float:a;
        GetPlayerPos(playerid, x, y, z);
        GetPlayerFacingAngle(playerid, a);

        // Offset spawn position behind player
        x += 2.0 * floatsin(-a, degrees);
        y += 2.0 * floatcos(-a, degrees);

        // Create dog object
        new modelToUse = DogModelLoaded ? DOG_MODEL_ID : DOG_BASE_MODEL;
        DogObject[playerid] = CreateObject(modelToUse, x, y, z - 1.0, 0.0, 0.0, a);

        if(DogObject[playerid] == INVALID_OBJECT_ID)
        {
                printf("[DOG] Failed to create dog object for player %d", playerid);
                return 0;
        }

        // Set dog color based on type (uses material to color the model)
        new color;
        switch(type)
        {
                case DOG_TYPE_ROTTWEILER: color = DOG_COLOR_ROTTWEILER;
                case DOG_TYPE_HUSKY:      color = DOG_COLOR_HUSKY;
                case DOG_TYPE_GOLDEN:     color = DOG_COLOR_GOLDEN;
                case DOG_TYPE_PITBULL:    color = DOG_COLOR_PITBULL;
                case DOG_TYPE_SHEPHERD:   color = DOG_COLOR_SHEPHERD;
                default: color = DOG_COLOR_ROTTWEILER;
        }
        // Apply color via material (index 0 = main texture)
        SetObjectMaterial(DogObject[playerid], 0, -1, "none", "none", color);

        // Create 3D label
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

        printf("[DOG] Player %d spawned dog: %s (%s)", playerid, name, DogTypeNames[type]);
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
                GetObjectPos(DogObject[playerid], dx, dy, dz);
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

        // Determine follow distance based on state
        new Float:followDist = 2.0;
        new Float:speed = 0.08; // Movement speed

        if(DogState[playerid] == DOG_STATE_RUN)
        {
                followDist = 1.5;
                speed = 0.15;
        }
        else if(DogState[playerid] == DOG_STATE_SIT)
        {
                // Don't move when sitting
                return;
        }

        if(dist > followDist)
        {
                // Calculate angle towards target
                angle = atan2(dy - cy, dx - cx);

                // Move towards target
                new Float:moveX, Float:moveY;
                moveX = cx + speed * floatcos(angle, degrees) * (dist > 10.0 ? 3.0 : 1.0);
                moveY = cy + speed * floatsin(angle, degrees) * (dist > 10.0 ? 3.0 : 1.0);

                // Get ground Z (approximate - use player Z as base)
                new Float:targetZ = (DogState[playerid] == DOG_STATE_STAY) ? DogStayZ[playerid] : pz;

                // Smooth Z transition
                new Float:zDiff = targetZ - cz;
                if(zDiff > 1.0) targetZ = cz + 0.3;
                else if(zDiff < -1.0) targetZ = cz - 0.3;

                MoveObject(DogObject[playerid], moveX, moveY, targetZ - 1.0, speed * 100.0, 0.0, 0.0, angle);

                // Update 3D label position
                if(DogLabel[playerid] != INVALID_3DTEXT_ID)
                {
                        new labelStr[64];
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
                        // Move label slightly offset from dog
                        new Float:labelX = moveX + 0.5 * floatcos(angle + 90.0, degrees);
                        new Float:labelY = moveY + 0.5 * floatsin(angle + 90.0, degrees);
                        // Can't move 3D labels attached to nothing easily, just update text
                }
        }
}

// ===== DOG COMMANDS =====

// /pas - Main dog menu
CMD:pas(playerid, params[])
{
        if(!PlayerLogged[playerid]) return SendErrorMessage(playerid, "Morate biti ulogovani!");

        // If player has no dog, show spawn dialog
        if(!DogActive[playerid])
        {
                SPD(playerid, dialog_DOG_SPAWN, DIALOG_STYLE_LIST,
                        "Pas - Izaberi psa",
                        "Rottweiler\nHusky\nGolden Retriever\nPitbull\nNjemacki Ovcarski",
                        "Izaberi", "Odustani");
                return 1;
        }

        // If player has a dog, show control dialog
        new stateStr[32];
        switch(DogState[playerid])
        {
                case DOG_STATE_FOLLOW: stateStr = "Slijedi te";
                case DOG_STATE_SIT:    stateStr = "Sjedi";
                case DOG_STATE_STAY:   stateStr = "Ceka na mjestu";
                case DOG_STATE_RUN:    stateStr = "Trci";
                case DOG_STATE_DEAD:   stateStr = "Mrtav";
                default: stateStr = "Nepoznato";
        }

        new dialogStr[256];
        format(dialogStr, sizeof(dialogStr),
                "Stanje psa: %s\nIme: %s\nVrsta: %s\nHP: %d\n\n\
                Slijedi me\nSjedi\nCekaj ovdje\nTrci za mnom\nPreimenuj psa\nOtpusti psa",
                stateStr, DogName[playerid], DogTypeNames[DogType[playerid]], DogHP[playerid]);

        SPD(playerid, dialog_DOG_CONTROL, DIALOG_STYLE_LIST,
                "Pas - Komande", dialogStr, "Izaberi", "Zatvori");
        return 1;
}

// Quick commands
CMD:pasfollow(playerid)
{
        if(!DogActive[playerid]) return SendErrorMessage(playerid, "Nemas psa! Kucaj /pas da ga dohvatis.");
        if(DogState[playerid] == DOG_STATE_DEAD) return SendErrorMessage(playerid, "Tvoj pas je mrtav!");
        DogState[playerid] = DOG_STATE_FOLLOW;
        SendClientMessage(playerid, 0x22c55eFF, " Pas te slijedi!");
        return 1;
}

CMD:passjedi(playerid)
{
        if(!DogActive[playerid]) return SendErrorMessage(playerid, "Nemas psa! Kucaj /pas da ga dohvatis.");
        if(DogState[playerid] == DOG_STATE_DEAD) return SendErrorMessage(playerid, "Tvoj pas je mrtav!");
        DogState[playerid] = DOG_STATE_SIT;
        SendClientMessage(playerid, 0xeab308FF, " Pas sjedi.");
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
        return 1;
}

CMD:pastrci(playerid)
{
        if(!DogActive[playerid]) return SendErrorMessage(playerid, "Nemas psa! Kucaj /pas da ga dohvatis.");
        if(DogState[playerid] == DOG_STATE_DEAD) return SendErrorMessage(playerid, "Tvoj pas je mrtav!");
        DogState[playerid] = DOG_STATE_RUN;
        SendClientMessage(playerid, 0xef4444FF, " Pas trci za tobom!");
        return 1;
}

CMD:pasotpusti(playerid)
{
        if(!DogActive[playerid]) return SendErrorMessage(playerid, "Nemas psa!");
        Dog_Destroy(playerid);
        SendClientMessage(playerid, 0xFFAA00FF, " Otpustio si psa.");
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
