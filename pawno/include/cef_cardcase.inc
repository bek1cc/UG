#if defined _INC_cef_cardcase
        #endinput
#endif
#define _INC_cef_cardcase

#define CARDCASE_BROWSER_ID   81
#define CARDCASE_URL          "http://cef/cardcase/index.html"
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

        // Generate random reward based on rarity - MODOVANA VOZILA!
        new rewardType = CARDCASE_TYPE_VEHICLE;
        new rewardName[64] = "Mystery Reward";
        new rewardLabel[64] = "Nagrada";
        new rewardAmount = 1;

        switch(rarity)
        {
                case CARDCASE_RARITY_LEGENDARY:
                {
                        // Legendary: premium modded vehicles, big money, VIP
                        new legType = random(4);
                        if(legType == 0) // Modded Vehicle
                        {
                                rewardType = CARDCASE_TYPE_VEHICLE;
                                new legRewards[][] = {
                                        {"BMW M5 F90", "M-Power Beast"},
                                        {"Mercedes AMG GT", "AMG Performance"},
                                        {"Lamborghini Urus", "Super SUV"},
                                        {"Audi RS6 Avant", "RS Wagon"},
                                        {"Porsche 911 Turbo S", "Porsche Legend"},
                                        {"BMW M8 Competition", "V12 Power"}
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
                        else if(legType == 2) // VIP
                        {
                                rewardType = CARDCASE_TYPE_VIP;
                                rewardAmount = 7; // 7 days VIP
                                format(rewardName, sizeof(rewardName), "VIP %d dana", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Premium clanstvo");
                        }
                        else // Respect
                        {
                                rewardType = CARDCASE_TYPE_RESPECT;
                                rewardAmount = 20 + random(16); // 20-35 respect
                                format(rewardName, sizeof(rewardName), "+%d Respect", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Legendarno postovanje");
                        }
                }
                case CARDCASE_RARITY_EPIC:
                {
                        new epicType = random(4);
                        if(epicType == 0) // Modded Vehicle
                        {
                                rewardType = CARDCASE_TYPE_VEHICLE;
                                new epicRewards[][] = {
                                        {"BMW M3 G80", "M-Power Sport"},
                                        {"Mercedes C63 AMG", "AMG Sedan"},
                                        {"Audi RS5 Sportback", "RS Sport"},
                                        {"VW Golf R MK8", "Hot Hatch King"},
                                        {"BMW M4 Competition", "Coupe Beast"},
                                        {"Mercedes E63 AMG", "AMG Luxury"}
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
                        else if(epicType == 2) // Weapon
                        {
                                rewardType = CARDCASE_TYPE_WEAPON;
                                rewardAmount = 1;
                                format(rewardName, sizeof(rewardName), "Epic Oruzje");
                                format(rewardLabel, sizeof(rewardLabel), "Snazno oruzje");
                        }
                        else // Respect
                        {
                                rewardType = CARDCASE_TYPE_RESPECT;
                                rewardAmount = 10 + random(11); // 10-20 respect
                                format(rewardName, sizeof(rewardName), "+%d Respect", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Epicko postovanje");
                        }
                }
                case CARDCASE_RARITY_RARE:
                {
                        new rareType = random(4);
                        if(rareType == 0) // Modded Vehicle
                        {
                                rewardType = CARDCASE_TYPE_VEHICLE;
                                new rareRewards[][] = {
                                        {"BMW 340i", "Tuned Sedan"},
                                        {"Mercedes C300 AMG Line", "AMG Style"},
                                        {"Audi S4 B9", "S-Line Sport"},
                                        {"VW Golf GTI MK7", "Hot Hatch"},
                                        {"BMW 435i", "Tuned Coupe"}
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
                        else if(rareType == 2) // Respect
                        {
                                rewardType = CARDCASE_TYPE_RESPECT;
                                rewardAmount = 5 + random(11); // 5-15 respect
                                format(rewardName, sizeof(rewardName), "+%d Respect", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Postovanje");
                        }
                        else // Level
                        {
                                rewardType = CARDCASE_TYPE_LEVEL;
                                rewardAmount = 2 + random(4); // 2-5 levels
                                format(rewardName, sizeof(rewardName), "+%d Level", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Iskustvo");
                        }
                }
                case CARDCASE_RARITY_COMMON:
                {
                        new comType = random(4);
                        if(comType == 0) // Modded Vehicle
                        {
                                rewardType = CARDCASE_TYPE_VEHICLE;
                                new commonRewards[][] = {
                                        {"BMW 320d", "Tuned Daily"},
                                        {"Mercedes A200", "Compact Sport"},
                                        {"Audi A3 Sportback", "Tuned Compact"},
                                        {"VW Golf MK7", "Tuned Hatch"},
                                        {"BMW 118i M Sport", "Entry M"}
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
                        else if(comType == 2) // Level
                        {
                                rewardType = CARDCASE_TYPE_LEVEL;
                                rewardAmount = 1 + random(3); // 1-3 levels
                                format(rewardName, sizeof(rewardName), "+%d Level", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Iskustvo");
                        }
                        else // Respect
                        {
                                rewardType = CARDCASE_TYPE_RESPECT;
                                rewardAmount = 1 + random(5); // 1-5 respect
                                format(rewardName, sizeof(rewardName), "+%d Respect", rewardAmount);
                                format(rewardLabel, sizeof(rewardLabel), "Malo postovanje");
                        }
                }
        }

        // Save reward data
        CardCaseRewardType[playerid] = rewardType;
        format(CardCaseRewardName[playerid], sizeof(CardCaseRewardName[]), "%s", rewardName);
        format(CardCaseRewardLabel[playerid], sizeof(CardCaseRewardLabel[]), "%s", rewardLabel);
        CardCaseRewardAmount[playerid] = rewardAmount;

        printf("[CARDCASE] Player %d opening case - reward: %s (%s), rarity: %d",
                playerid, rewardName, rewardLabel, rarity);

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

                // Vehicle image URL - for modded cars no external image available
                // SVG car icon is used in the HTML instead
                new imgUrl[128];
                imgUrl[0] = EOS;

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
                        // TODO: Spawn modded vehicle for player
                        // Vehicle model ID will need to be mapped from vehicle name
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
        CEF_AddResource("cardcase");

        // Register CEF events from browser
        CEF_RegisterEvent("cardcase:close", "OnCefCardCaseClose");
        CEF_RegisterEvent("cardcase:collect", "OnCefCardCaseCollect");

        printf("[CARDCASE] Resource + events registered: cardcase -> scriptfiles/cef/cardcase/");
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
