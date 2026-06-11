#if defined _INC_cef_case
        #endinput
#endif
#define _INC_cef_case

#define CARDCASE_BROWSER_ID   81
#define CARDCASE_URL          "http://cef/case/index.html"
#define CARDCASE_PRICE        5000

// Reward types
#define CARDCASE_TYPE_VEHICLE   1
#define CARDCASE_TYPE_MONEY     2
#define CARDCASE_TYPE_LEVEL     3
#define CARDCASE_TYPE_RESPECT   4
#define CARDCASE_TYPE_WEAPON    5
#define CARDCASE_TYPE_VIP       6

// Rarity levels
#define CARDCASE_RARITY_COMMON     1
#define CARDCASE_RARITY_RARE       2
#define CARDCASE_RARITY_EPIC       3
#define CARDCASE_RARITY_LEGENDARY  4

// Rarity percentages: Common 50%, Rare 30%, Epic 15%, Legendary 5%

static bool:CardCaseOpen[MAX_PLAYERS];
static CardCaseRewardType[MAX_PLAYERS];
static CardCaseRewardRarity[MAX_PLAYERS];
static CardCaseRewardName[MAX_PLAYERS][64];
static CardCaseRewardLabel[MAX_PLAYERS][64];
static CardCaseRewardAmount[MAX_PLAYERS];

stock CEF_CardCaseOpen(playerid)
{
        if(CardCaseOpen[playerid]) return 0;

        // Check money
        if(GetPlayerMoney(playerid) < CARDCASE_PRICE)
        {
                SendClientMessage(playerid, 0xFF4444FF, " Nemate dovoljno novca! Card case kosta $5,000.");
                return 0;
        }

        // Take money
        GivePlayerMoney(playerid, -CARDCASE_PRICE);

        CardCaseOpen[playerid] = true;

        // Generate random rarity
        new rarity = CARDCASE_RARITY_COMMON;
        new rand = random(100);
        if(rand < 50) rarity = CARDCASE_RARITY_COMMON;
        else if(rand < 80) rarity = CARDCASE_RARITY_RARE;
        else if(rand < 95) rarity = CARDCASE_RARITY_EPIC;
        else rarity = CARDCASE_RARITY_LEGENDARY;

        CardCaseRewardRarity[playerid] = rarity;

        // Generate random reward based on rarity
        new rewardType = CARDCASE_TYPE_VEHICLE;
        new rewardName[64] = "Mystery Reward";
        new rewardLabel[64] = "Nagrada";
        new rewardAmount = 1;

        switch(rarity)
        {
                case CARDCASE_RARITY_LEGENDARY:
                {
                        // Legendary: premium vehicles, big money, VIP
                        new legType = random(3);
                        if(legType == 0) // Vehicle
                        {
                                rewardType = CARDCASE_TYPE_VEHICLE;
                                new legRewards[][] = {
                                        {"Elegy", "Sportsko vozilo"},
                                        {"Sultan", "Sportsko vozilo"},
                                        {"Infernus", "Super auto"},
                                        {"Bullet", "Sportsko vozilo"},
                                        {"Cheetah", "Super auto"},
                                        {"Turismo", "Super auto"}
                                };
                                new idx = random(sizeof(legRewards));
                                format(rewardName, sizeof(rewardName), "%s", legRewards[idx][0]);
                                format(rewardLabel, sizeof(rewardLabel), "%s", legRewards[idx][1]);
                                rewardAmount = 1;
                        }
                        else if(legType == 1) // Big Money
                        {
                                rewardType = CARDCASE_TYPE_MONEY;
                                rewardAmount = 25000 + random(25001); // $25,000 - $50,000
                                format(rewardName, sizeof(rewardName), "$%d", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Legendarne pare");
                        }
                        else // VIP
                        {
                                rewardType = CARDCASE_TYPE_VIP;
                                rewardAmount = 7; // 7 days VIP
                                format(rewardName, sizeof(rewardName), "VIP %d dana", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Premium clanstvo");
                        }
                }
                case CARDCASE_RARITY_EPIC:
                {
                        new epicType = random(3);
                        if(epicType == 0) // Vehicle
                        {
                                rewardType = CARDCASE_TYPE_VEHICLE;
                                new epicRewards[][] = {
                                        {"Banshee", "Sportsko vozilo"},
                                        {"ZR-350", "Sportsko vozilo"},
                                        {"Jester", "Sportsko vozilo"},
                                        {"Uranus", "Sportsko vozilo"},
                                        {"Flash", "Sportsko vozilo"},
                                        {"Stratum", "Sportski sedan"}
                                };
                                new idx = random(sizeof(epicRewards));
                                format(rewardName, sizeof(rewardName), "%s", epicRewards[idx][0]);
                                format(rewardLabel, sizeof(rewardLabel), "%s", epicRewards[idx][1]);
                                rewardAmount = 1;
                        }
                        else if(epicType == 1) // Money
                        {
                                rewardType = CARDCASE_TYPE_MONEY;
                                rewardAmount = 10000 + random(15001); // $10,000 - $25,000
                                format(rewardName, sizeof(rewardName), "$%d", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Epske pare");
                        }
                        else // Weapon
                        {
                                rewardType = CARDCASE_TYPE_WEAPON;
                                rewardAmount = 1;
                                format(rewardName, sizeof(rewardName), "Epic Oruzje");
                                format(rewardLabel, sizeof(rewardLabel), "Snazno oruzje");
                        }
                }
                case CARDCASE_RARITY_RARE:
                {
                        new rareType = random(3);
                        if(rareType == 0) // Vehicle
                        {
                                rewardType = CARDCASE_TYPE_VEHICLE;
                                new rareRewards[][] = {
                                        {"Sabre", "Muscle auto"},
                                        {"Tampa", "Muscle auto"},
                                        {"Sunrise", "Klasik"},
                                        {"Premier", "Sedan"},
                                        {"Merit", "Sedan"}
                                };
                                new idx = random(sizeof(rareRewards));
                                format(rewardName, sizeof(rewardName), "%s", rareRewards[idx][0]);
                                format(rewardLabel, sizeof(rewardLabel), "%s", rareRewards[idx][1]);
                                rewardAmount = 1;
                        }
                        else if(rareType == 1) // Money
                        {
                                rewardType = CARDCASE_TYPE_MONEY;
                                rewardAmount = 3000 + random(7001); // $3,000 - $10,000
                                format(rewardName, sizeof(rewardName), "$%d", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Rijetke pare");
                        }
                        else // Respect
                        {
                                rewardType = CARDCASE_TYPE_RESPECT;
                                rewardAmount = 5 + random(11); // 5-15 respect
                                format(rewardName, sizeof(rewardName), "%d Respect", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Postovanje");
                        }
                }
                case CARDCASE_RARITY_COMMON:
                {
                        new comType = random(3);
                        if(comType == 0) // Vehicle
                        {
                                rewardType = CARDCASE_TYPE_VEHICLE;
                                new commonRewards[][] = {
                                        {"Bravura", "Gradski auto"},
                                        {"Sentinel", "Sedan"},
                                        {"Admiral", "Sedan"},
                                        {"Washington", "Sedan"},
                                        {"Vincent", "Klasik"}
                                };
                                new idx = random(sizeof(commonRewards));
                                format(rewardName, sizeof(rewardName), "%s", commonRewards[idx][0]);
                                format(rewardLabel, sizeof(rewardLabel), "%s", commonRewards[idx][1]);
                                rewardAmount = 1;
                        }
                        else if(comType == 1) // Money
                        {
                                rewardType = CARDCASE_TYPE_MONEY;
                                rewardAmount = 500 + random(2501); // $500 - $3,000
                                format(rewardName, sizeof(rewardName), "$%d", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Male pare");
                        }
                        else // Level
                        {
                                rewardType = CARDCASE_TYPE_LEVEL;
                                rewardAmount = 1 + random(3); // 1-3 levels
                                format(rewardName, sizeof(rewardName), "+%d Level", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Iskustvo");
                        }
                }
        }

        // Save reward data
        CardCaseRewardType[playerid] = rewardType;
        format(CardCaseRewardName[playerid], sizeof(CardCaseRewardName[]), "%s", rewardName);
        format(CardCaseRewardLabel[playerid], sizeof(CardCaseRewardLabel[]), "%s", rewardLabel);
        CardCaseRewardAmount[playerid] = rewardAmount;

        // Create CEF browser
        CEF_CreateBrowser(playerid, CARDCASE_BROWSER_ID, CARDCASE_URL, true);

        SendClientMessage(playerid, 0x8b5cf6FF, " Card Case otvoren! Izaberi kartu...");
        return 1;
}

stock CEF_CardCaseClose(playerid)
{
        if(!CardCaseOpen[playerid]) return 0;
        CardCaseOpen[playerid] = false;
        CEF_DestroyBrowser(playerid, CARDCASE_BROWSER_ID);
        return 1;
}

stock bool:CEF_CardCaseIsOpen(playerid)
{
        return CardCaseOpen[playerid];
}

// Called when browser is created - send reward data to CEF
stock CefCardCase_OnBrowserCreated(playerid, browserid)
{
        if(browserid == CARDCASE_BROWSER_ID)
        {
                printf("[CARDCASE] Browser created for player %d, sending reward data", playerid);

                // Rarity string
                new rarityStr[16];
                switch(CardCaseRewardRarity[playerid])
                {
                        case CARDCASE_RARITY_COMMON:    format(rarityStr, sizeof(rarityStr), "common");
                        case CARDCASE_RARITY_RARE:      format(rarityStr, sizeof(rarityStr), "rare");
                        case CARDCASE_RARITY_EPIC:      format(rarityStr, sizeof(rarityStr), "epic");
                        case CARDCASE_RARITY_LEGENDARY: format(rarityStr, sizeof(rarityStr), "legendary");
                        default: format(rarityStr, sizeof(rarityStr), "common");
                }

                // Type string
                new typeStr[16];
                switch(CardCaseRewardType[playerid])
                {
                        case CARDCASE_TYPE_VEHICLE: format(typeStr, sizeof(typeStr), "vehicle");
                        case CARDCASE_TYPE_MONEY:   format(typeStr, sizeof(typeStr), "money");
                        case CARDCASE_TYPE_LEVEL:   format(typeStr, sizeof(typeStr), "level");
                        case CARDCASE_TYPE_RESPECT: format(typeStr, sizeof(typeStr), "respect");
                        case CARDCASE_TYPE_WEAPON:  format(typeStr, sizeof(typeStr), "weapon");
                        case CARDCASE_TYPE_VIP:     format(typeStr, sizeof(typeStr), "vip");
                        default: format(typeStr, sizeof(typeStr), "money");
                }

                // Vehicle image URL
                new imgUrl[128];
                if(CardCaseRewardType[playerid] == CARDCASE_TYPE_VEHICLE)
                {
                        new lowerName[64];
                        format(lowerName, sizeof(lowerName), "%s", CardCaseRewardName[playerid]);
                        // Lowercase the name for URL
                        for(new i = 0; lowerName[i] != '\0'; i++)
                        {
                                if(lowerName[i] >= 'A' && lowerName[i] <= 'Z')
                                        lowerName[i] = lowerName[i] + 32;
                        }
                        format(imgUrl, sizeof(imgUrl), "https://gtamag.com/images/gta-sa-%s.png", lowerName);
                }
                else
                {
                        imgUrl = "";
                }

                // Send to CEF: cardcase:init(name, type, rarity, label, image)
                CEF_EmitEvent(playerid, CARDCASE_BROWSER_ID, "cardcase:init",
                        CEF_STR(CardCaseRewardName[playerid]),
                        CEF_STR(typeStr),
                        CEF_STR(rarityStr),
                        CEF_STR(CardCaseRewardLabel[playerid]),
                        CEF_STR(imgUrl));

                CEF_FocusBrowser(playerid, CARDCASE_BROWSER_ID, true);
        }
}

// Give the actual reward when player collects
stock CefCardCase_GiveReward(playerid)
{
        if(!CardCaseOpen[playerid]) return 0;

        new type = CardCaseRewardType[playerid];
        new amount = CardCaseRewardAmount[playerid];

        switch(type)
        {
                case CARDCASE_TYPE_VEHICLE:
                {
                        // TODO: Spawn vehicle for player
                        // For now just message
                        new msg[128];
                        format(msg, sizeof(msg), " Osvojio si vozilo: %s!", CardCaseRewardName[playerid]);
                        SendClientMessage(playerid, 0xFFAA00FF, msg);
                }
                case CARDCASE_TYPE_MONEY:
                {
                        GivePlayerMoney(playerid, amount);
                        new msg[128];
                        format(msg, sizeof(msg), " Osvojio si $%d!", amount);
                        SendClientMessage(playerid, 0x22c55eFF, msg);
                }
                case CARDCASE_TYPE_LEVEL:
                {
                        // TODO: SetPlayerScore(playerid, GetPlayerScore(playerid) + amount)
                        new msg[128];
                        format(msg, sizeof(msg), " Osvojio si +%d Level!", amount);
                        SendClientMessage(playerid, 0xeab308FF, msg);
                }
                case CARDCASE_TYPE_RESPECT:
                {
                        // TODO: Add respect points
                        new msg[128];
                        format(msg, sizeof(msg), " Osvojio si +%d Respect!", amount);
                        SendClientMessage(playerid, 0xef4444FF, msg);
                }
                case CARDCASE_TYPE_WEAPON:
                {
                        // TODO: Give weapon
                        SendClientMessage(playerid, 0xf97316FF, " Osvojio si Epic oruzje!");
                }
                case CARDCASE_TYPE_VIP:
                {
                        // TODO: Set VIP days
                        new msg[128];
                        format(msg, sizeof(msg), " Osvojio si VIP na %d dana!", amount);
                        SendClientMessage(playerid, 0x06b6d4FF, msg);
                }
        }

        printf("[CARDCASE] Player %d collected reward: %s (type=%d, rarity=%d, amount=%d)",
                playerid, CardCaseRewardName[playerid], type, CardCaseRewardRarity[playerid], amount);

        return 1;
}

// Register CEF resource + events
stock CefCardCase_OnGameModeInit()
{
        CEF_AddResource("case");

        // Register CEF events from browser
        CEF_RegisterEvent("cardcase:close", "OnCefCardCaseClose");
        CEF_RegisterEvent("cardcase:collect", "OnCefCardCaseCollect");

        printf("[CARDCASE] Resource + events registered: case -> scriptfiles/cef/case/");
}

stock CefCardCase_OnPlayerDisconnect(playerid)
{
        if(CardCaseOpen[playerid])
        {
                CardCaseOpen[playerid] = false;
        }
}

// ===== CEF Event Callbacks =====
forward OnCefCardCaseClose(playerid);
public OnCefCardCaseClose(playerid)
{
        printf("[CARDCASE] Player %d closed card case", playerid);
        CEF_CardCaseClose(playerid);
        return 1;
}

forward OnCefCardCaseCollect(playerid);
public OnCefCardCaseCollect(playerid)
{
        printf("[CARDCASE] Player %d collected reward", playerid);
        CefCardCase_GiveReward(playerid);
        return 1;
}
