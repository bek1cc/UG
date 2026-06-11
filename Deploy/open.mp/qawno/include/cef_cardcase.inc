#if defined _INC_cef_cardcase
        #endinput
#endif
#define _INC_cef_cardcase

#define CEF_CARDCASE_BROWSER_ID    81
#define CEF_CARDCASE_URL           "http://cef/cardcase/index.html"

static bool:CardCaseOpen[MAX_PLAYERS];
static CardCaseRewardType[MAX_PLAYERS];
static CardCaseRewardRarity[MAX_PLAYERS];
static CardCaseRewardName[MAX_PLAYERS][64];
static CardCaseRewardValue[MAX_PLAYERS][64];
static CardCaseRewardLabel[MAX_PLAYERS][64];
static Float:CardCaseRewardSpeed[MAX_PLAYERS];
static Float:CardCaseRewardAccel[MAX_PLAYERS];
static Float:CardCaseRewardHandling[MAX_PLAYERS];
static CardCaseRewardAmount[MAX_PLAYERS];

// Reward types
#define CARDCASE_TYPE_VEHICLE      1
#define CARDCASE_TYPE_MONEY        2
#define CARDCASE_TYPE_LEVEL        3
#define CARDCASE_TYPE_RESPECT      4
#define CARDCASE_TYPE_WEAPON       5
#define CARDCASE_TYPE_SKIN         6
#define CARDCASE_TYPE_VIP          7

// Rarity levels
#define CARDCASE_RARITY_COMMON     1
#define CARDCASE_RARITY_RARE       2
#define CARDCASE_RARITY_EPIC       3
#define CARDCASE_RARITY_LEGENDARY  4

// Prices
#define CARDCASE_PRICE             5000

stock CEF_CardCaseOpen(playerid)
{
        if(CardCaseOpen[playerid])
        {
                SendClientMessage(playerid, 0xFF4444FF, "[CARD CASE] Vec imas otvorenu kartu!");
                return 0;
        }

        // Provjeri da li igrac ima novac
        if(GetPlayerMoney(playerid) < CARDCASE_PRICE)
        {
                SendClientMessage(playerid, 0xFF4444FF, "[CARD CASE] Nemas dovoljno novca! Cena: $5,000");
                return 0;
        }

        // Oduzmi novac
        GivePlayerMoney(playerid, -CARDCASE_PRICE);

        CardCaseOpen[playerid] = true;

        // ===== RANDOM RARITY =====
        // Common=50%, Rare=30%, Epic=15%, Legendary=5%
        new rarity = CARDCASE_RARITY_COMMON;
        new rand = random(100);
        if(rand < 50) rarity = CARDCASE_RARITY_COMMON;
        else if(rand < 80) rarity = CARDCASE_RARITY_RARE;
        else if(rand < 95) rarity = CARDCASE_RARITY_EPIC;
        else rarity = CARDCASE_RARITY_LEGENDARY;

        CardCaseRewardRarity[playerid] = rarity;

        // ===== RANDOM REWARD TYPE BASED ON RARITY =====
        new rewardType = CARDCASE_TYPE_VEHICLE;
        new typeRand = random(100);

        switch(rarity)
        {
                case CARDCASE_RARITY_LEGENDARY:
                {
                        if(typeRand < 60) rewardType = CARDCASE_TYPE_VEHICLE;
                        else if(typeRand < 80) rewardType = CARDCASE_TYPE_VIP;
                        else if(typeRand < 90) rewardType = CARDCASE_TYPE_MONEY;
                        else rewardType = CARDCASE_TYPE_RESPECT;
                }
                case CARDCASE_RARITY_EPIC:
                {
                        if(typeRand < 50) rewardType = CARDCASE_TYPE_VEHICLE;
                        else if(typeRand < 70) rewardType = CARDCASE_TYPE_WEAPON;
                        else if(typeRand < 85) rewardType = CARDCASE_TYPE_MONEY;
                        else rewardType = CARDCASE_TYPE_LEVEL;
                }
                case CARDCASE_RARITY_RARE:
                {
                        if(typeRand < 40) rewardType = CARDCASE_TYPE_VEHICLE;
                        else if(typeRand < 60) rewardType = CARDCASE_TYPE_MONEY;
                        else if(typeRand < 80) rewardType = CARDCASE_TYPE_RESPECT;
                        else rewardType = CARDCASE_TYPE_LEVEL;
                }
                case CARDCASE_RARITY_COMMON:
                {
                        if(typeRand < 40) rewardType = CARDCASE_TYPE_MONEY;
                        else if(typeRand < 60) rewardType = CARDCASE_TYPE_RESPECT;
                        else if(typeRand < 80) rewardType = CARDCASE_TYPE_VEHICLE;
                        else rewardType = CARDCASE_TYPE_LEVEL;
                }
        }

        CardCaseRewardType[playerid] = rewardType;

        // ===== GENERATE REWARD =====
        new rewardName[64], rewardLabel[64], rewardValue[64];
        new Float:speed = 5.0, Float:accel = 5.0, Float:handling = 5.0;
        new amount = 0;

        switch(rewardType)
        {
                case CARDCASE_TYPE_VEHICLE:
                {
                        switch(rarity)
                        {
                                case CARDCASE_RARITY_LEGENDARY:
                                {
                                        new rewards[][] = {
                                                {"Infernus", "Super auto"},
                                                {"Cheetah", "Super auto"},
                                                {"Turismo", "Super auto"},
                                                {"Bullet", "Sportsko vozilo"}
                                        };
                                        new idx = random(sizeof(rewards));
                                        format(rewardName, 64, "%s", rewards[idx][0]);
                                        format(rewardLabel, 64, "%s", rewards[idx][1]);
                                        speed = 9.0 + (random(10) / 10.0);
                                        accel = 8.5 + (random(10) / 10.0);
                                        handling = 8.5 + (random(10) / 10.0);
                                }
                                case CARDCASE_RARITY_EPIC:
                                {
                                        new rewards[][] = {
                                                {"Sultan", "Sportsko vozilo"},
                                                {"Elegy", "Sportsko vozilo"},
                                                {"Banshee", "Sportsko vozilo"},
                                                {"Jester", "Sportsko vozilo"}
                                        };
                                        new idx = random(sizeof(rewards));
                                        format(rewardName, 64, "%s", rewards[idx][0]);
                                        format(rewardLabel, 64, "%s", rewards[idx][1]);
                                        speed = 7.5 + (random(10) / 10.0);
                                        accel = 7.0 + (random(10) / 10.0);
                                        handling = 7.5 + (random(10) / 10.0);
                                }
                                case CARDCASE_RARITY_RARE:
                                {
                                        new rewards[][] = {
                                                {"Sabre", "Muscle auto"},
                                                {"Tampa", "Muscle auto"},
                                                {"ZR-350", "Sportsko vozilo"},
                                                {"Uranus", "Sportsko vozilo"}
                                        };
                                        new idx = random(sizeof(rewards));
                                        format(rewardName, 64, "%s", rewards[idx][0]);
                                        format(rewardLabel, 64, "%s", rewards[idx][1]);
                                        speed = 6.0 + (random(10) / 10.0);
                                        accel = 5.5 + (random(10) / 10.0);
                                        handling = 6.0 + (random(10) / 10.0);
                                }
                                case CARDCASE_RARITY_COMMON:
                                {
                                        new rewards[][] = {
                                                {"Bravura", "Gradski auto"},
                                                {"Sentinel", "Sedan"},
                                                {"Admiral", "Sedan"},
                                                {"Washington", "Sedan"}
                                        };
                                        new idx = random(sizeof(rewards));
                                        format(rewardName, 64, "%s", rewards[idx][0]);
                                        format(rewardLabel, 64, "%s", rewards[idx][1]);
                                        speed = 4.0 + (random(10) / 10.0);
                                        accel = 4.0 + (random(10) / 10.0);
                                        handling = 4.5 + (random(10) / 10.0);
                                }
                        }
                        format(rewardValue, 64, "%s", rewardName);
                }
                case CARDCASE_TYPE_MONEY:
                {
                        switch(rarity)
                        {
                                case CARDCASE_RARITY_LEGENDARY: amount = 50000 + random(50000);
                                case CARDCASE_RARITY_EPIC: amount = 20000 + random(20000);
                                case CARDCASE_RARITY_RARE: amount = 10000 + random(10000);
                                case CARDCASE_RARITY_COMMON: amount = 1000 + random(4000);
                        }
                        format(rewardName, 64, "$%d", amount);
                        format(rewardLabel, 64, "Novac");
                        format(rewardValue, 64, "%d", amount);
                }
                case CARDCASE_TYPE_LEVEL:
                {
                        switch(rarity)
                        {
                                case CARDCASE_RARITY_LEGENDARY: amount = 5;
                                case CARDCASE_RARITY_EPIC: amount = 3;
                                case CARDCASE_RARITY_RARE: amount = 2;
                                case CARDCASE_RARITY_COMMON: amount = 1;
                        }
                        format(rewardName, 64, "Level %d", amount);
                        format(rewardLabel, 64, "Iskustvo");
                        format(rewardValue, 64, "%d", amount);
                }
                case CARDCASE_RARITY_RESPECT:
                {
                        switch(rarity)
                        {
                                case CARDCASE_RARITY_LEGENDARY: amount = 50 + random(50);
                                case CARDCASE_RARITY_EPIC: amount = 25 + random(25);
                                case CARDCASE_RARITY_RARE: amount = 10 + random(15);
                                case CARDCASE_RARITY_COMMON: amount = 5 + random(10);
                        }
                        format(rewardName, 64, "%d Respekta", amount);
                        format(rewardLabel, 64, "Respekt");
                        format(rewardValue, 64, "%d", amount);
                }
                case CARDCASE_TYPE_WEAPON:
                {
                        switch(rarity)
                        {
                                case CARDCASE_RARITY_LEGENDARY:
                                {
                                        format(rewardName, 64, "Minigun");
                                        format(rewardLabel, 64, "Oruzje");
                                        amount = 500;
                                }
                                case CARDCASE_RARITY_EPIC:
                                {
                                        new wpns[][] = {{"RPG", "Eksploziv"}, {"Sniper Rifle", "Snajper"}, {"SPAS-12", "Shotgun"}};
                                        new idx = random(sizeof(wpns));
                                        format(rewardName, 64, "%s", wpns[idx][0]);
                                        format(rewardLabel, 64, "%s", wpns[idx][1]);
                                        amount = 200;
                                }
                                default:
                                {
                                        new wpns[][] = {{"AK-47", "Automat"}, {"M4", "Automat"}, {"Deagle", "Pistolj"}};
                                        new idx = random(sizeof(wpns));
                                        format(rewardName, 64, "%s", wpns[idx][0]);
                                        format(rewardLabel, 64, "%s", wpns[idx][1]);
                                        amount = 100;
                                }
                        }
                        format(rewardValue, 64, "%d", amount);
                }
                case CARDCASE_TYPE_VIP:
                {
                        format(rewardName, 64, "VIP Zlato");
                        format(rewardLabel, 64, "VIP Status");
                        format(rewardValue, 64, "7");
                        amount = 7;
                }
        }

        // Sacuvaj reward
        format(CardCaseRewardName[playerid], 64, "%s", rewardName);
        format(CardCaseRewardValue[playerid], 64, "%s", rewardValue);
        format(CardCaseRewardLabel[playerid], 64, "%s", rewardLabel);
        CardCaseRewardSpeed[playerid] = speed;
        CardCaseRewardAccel[playerid] = accel;
        CardCaseRewardHandling[playerid] = handling;
        CardCaseRewardAmount[playerid] = amount;

        // Kreiraj CEF browser
        CEF_CreateBrowser(playerid, CEF_CARDCASE_BROWSER_ID, CEF_CARDCASE_URL, true);

        return 1;
}

stock CEF_CardCaseClose(playerid)
{
        CardCaseOpen[playerid] = false;
        CEF_DestroyBrowser(playerid, CEF_CARDCASE_BROWSER_ID);
}

stock bool:CEF_CardCaseIsOpen(playerid)
{
        return CardCaseOpen[playerid];
}

stock CefCardCase_OnBrowserCreated(playerid, browserid)
{
        if(browserid == CEF_CARDCASE_BROWSER_ID)
        {
                printf("[CEF-CARDCASE] Card case browser created for player %d", playerid);

                // Rarity string
                new rarityStr[16];
                switch(CardCaseRewardRarity[playerid])
                {
                        case CARDCASE_RARITY_COMMON: format(rarityStr, 16, "common");
                        case CARDCASE_RARITY_RARE: format(rarityStr, 16, "rare");
                        case CARDCASE_RARITY_EPIC: format(rarityStr, 16, "epic");
                        case CARDCASE_RARITY_LEGENDARY: format(rarityStr, 16, "legendary");
                }

                // Type string
                new typeStr[16];
                switch(CardCaseRewardType[playerid])
                {
                        case CARDCASE_TYPE_VEHICLE: format(typeStr, 16, "vehicle");
                        case CARDCASE_TYPE_MONEY: format(typeStr, 16, "money");
                        case CARDCASE_TYPE_LEVEL: format(typeStr, 16, "level");
                        case CARDCASE_TYPE_RESPECT: format(typeStr, 16, "respect");
                        case CARDCASE_TYPE_WEAPON: format(typeStr, 16, "weapon");
                        case CARDCASE_TYPE_VIP: format(typeStr, 16, "vip");
                        default: format(typeStr, 16, "money");
                }

                // Speed/accel/handling strings
                new speedStr[8], accelStr[8], handlingStr[8];
                format(speedStr, 8, "%.1f", CardCaseRewardSpeed[playerid]);
                format(accelStr, 8, "%.1f", CardCaseRewardAccel[playerid]);
                format(handlingStr, 8, "%.1f", CardCaseRewardHandling[playerid]);

                // Posalji podatke CEF-u
                CEF_EmitEvent(playerid, CEF_CARDCASE_BROWSER_ID, "cardcase:init",
                        CEF_STR(CardCaseRewardName[playerid]),
                        CEF_STR(typeStr),
                        CEF_STR(rarityStr),
                        CEF_STR(CardCaseRewardLabel[playerid]),
                        CEF_STR(CardCaseRewardValue[playerid]),
                        CEF_STR(speedStr),
                        CEF_STR(accelStr),
                        CEF_STR(handlingStr));

                CEF_FocusBrowser(playerid, CEF_CARDCASE_BROWSER_ID, true);
        }
}

stock CefCardCase_GiveReward(playerid)
{
        if(!CardCaseOpen[playerid]) return 0;

        switch(CardCaseRewardType[playerid])
        {
                case CARDCASE_TYPE_VEHICLE:
                {
                        // TODO: Spawn vozilo za igraca
                        // Za sada samo poruka
                        new msg[128];
                        format(msg, 128, "[CARD CASE] Dobio si vozilo: %s!", CardCaseRewardName[playerid]);
                        SendClientMessage(playerid, 0xFFD700FF, msg);
                }
                case CARDCASE_TYPE_MONEY:
                {
                        GivePlayerMoney(playerid, CardCaseRewardAmount[playerid]);
                        new msg[128];
                        format(msg, 128, "[CARD CASE] Dobio si $%d!", CardCaseRewardAmount[playerid]);
                        SendClientMessage(playerid, 0x2ECC71FF, msg);
                }
                case CARDCASE_TYPE_LEVEL:
                {
                        // TODO: Dodaj level
                        new msg[128];
                        format(msg, 128, "[CARD CASE] Dobio si %d level(a)!", CardCaseRewardAmount[playerid]);
                        SendClientMessage(playerid, 0xF39C12FF, msg);
                }
                case CARDCASE_TYPE_RESPECT:
                {
                        // TODO: Dodaj respekt
                        new msg[128];
                        format(msg, 128, "[CARD CASE] Dobio si %d respekta!", CardCaseRewardAmount[playerid]);
                        SendClientMessage(playerid, 0x9B59B6FF, msg);
                }
                case CARDCASE_TYPE_WEAPON:
                {
                        // TODO: Daj oruzje
                        new msg[128];
                        format(msg, 128, "[CARD CASE] Dobio si oruzje: %s!", CardCaseRewardName[playerid]);
                        SendClientMessage(playerid, 0xE74C3CFF, msg);
                }
                case CARDCASE_TYPE_VIP:
                {
                        // TODO: Dodaj VIP
                        SendClientMessage(playerid, 0xFFD700FF, "[CARD CASE] Dobio si VIP Zlato na 7 dana!");
                }
        }

        return 1;
}

stock CefCardCase_InitSubscriptions()
{
        CEF_RegisterEvent("cardcase:close", "OnCefCardCaseClose");
        CEF_RegisterEvent("cardcase:collect", "OnCefCardCaseCollect");
}

stock CefCardCase_OnGameModeInit()
{
        CEF_AddResource("cardcase");
        CefCardCase_InitSubscriptions();
        printf("[CEF-CARDCASE] Resource registered + events -> scriptfiles/cef/cardcase/");
}

stock CefCardCase_OnPlayerDisconnect(playerid)
{
        CardCaseOpen[playerid] = false;
}

// ===== CALLBACKs =====
forward OnCefCardCaseClose(playerid);
public OnCefCardCaseClose(playerid)
{
        printf("[CEF-CARDCASE] Player %d closed card case", playerid);
        CEF_CardCaseClose(playerid);
        return 1;
}

forward OnCefCardCaseCollect(playerid);
public OnCefCardCaseCollect(playerid)
{
        printf("[CEF-CARDCASE] Player %d collected reward", playerid);
        CefCardCase_GiveReward(playerid);
        CEF_CardCaseClose(playerid);
        return 1;
}
