#if defined _INC_cef_case
        #endinput
#endif
#define _INC_cef_case

#define CEF_CASE_BROWSER_ID    80
#define CEF_CASE_URL           "http://cef/case/index.html"

static bool:CaseOpen[MAX_PLAYERS];
static CaseRewardType[MAX_PLAYERS];
static CaseRewardRarity[MAX_PLAYERS];
static CaseRewardName[MAX_PLAYERS][64];
static CaseRewardLabel[MAX_PLAYERS][64];
static Float:CaseRewardSpeed[MAX_PLAYERS];
static Float:CaseRewardAccel[MAX_PLAYERS];
static Float:CaseRewardHandling[MAX_PLAYERS];

// Case reward types
#define CASE_TYPE_VEHICLE      1
#define CASE_TYPE_WEAPON       2
#define CASE_TYPE_MONEY        3
#define CASE_TYPE_SKIN         4
#define CASE_TYPE_VIP          5

// Case rarity levels
#define CASE_RARITY_COMMON     1
#define CASE_RARITY_RARE       2
#define CASE_RARITY_EPIC       3
#define CASE_RARITY_LEGENDARY  4

stock CEF_CaseOpen(playerid)
{
        CaseOpen[playerid] = true;

        // Odredi nagradu (random)
        new rewardType = CASE_TYPE_VEHICLE;
        new rarity = CASE_RARITY_LEGENDARY;
        new rewardName[64] = "Elegy";
        new rewardLabel[64] = "Sportsko vozilo";
        new Float:speed = 8.4;
        new Float:accel = 7.2;
        new Float:handling = 9.1;

        // Random rarity (common=50%, rare=30%, epic=15%, legendary=5%)
        new rand = random(100);
        if(rand < 50) rarity = CASE_RARITY_COMMON;
        else if(rand < 80) rarity = CASE_RARITY_RARE;
        else if(rand < 95) rarity = CASE_RARITY_EPIC;
        else rarity = CASE_RARITY_LEGENDARY;

        // Random reward based on rarity
        switch(rarity)
        {
                case CASE_RARITY_LEGENDARY:
                {
                        new legRewards[][] = {
                                {"Elegy", "Sportsko vozilo"},
                                {"Sultan", "Sportsko vozilo"},
                                {"Infernus", "Super auto"},
                                {"Bullet", "Sportsko vozilo"}
                        };
                        new idx = random(sizeof(legRewards));
                        format(rewardName, sizeof(rewardName), "%s", legRewards[idx][0]);
                        format(rewardLabel, sizeof(rewardLabel), "%s", legRewards[idx][1]);
                        speed = 9.0 + (random(10) / 10.0);
                        accel = 8.0 + (random(10) / 10.0);
                        handling = 8.5 + (random(10) / 10.0);
                }
                case CASE_RARITY_EPIC:
                {
                        new epicRewards[][] = {
                                {"Banshee", "Sportsko vozilo"},
                                {"ZR-350", "Sportsko vozilo"},
                                {"Jester", "Sportsko vozilo"},
                                {"Uranus", "Sportsko vozilo"}
                        };
                        new idx = random(sizeof(epicRewards));
                        format(rewardName, sizeof(rewardName), "%s", epicRewards[idx][0]);
                        format(rewardLabel, sizeof(rewardLabel), "%s", epicRewards[idx][1]);
                        speed = 7.5 + (random(10) / 10.0);
                        accel = 7.0 + (random(10) / 10.0);
                        handling = 7.5 + (random(10) / 10.0);
                }
                case CASE_RARITY_RARE:
                {
                        new rareRewards[][] = {
                                {"Sabre", "Muscle auto"},
                                {"Tampa", "Muscle auto"},
                                {"Sunrise", "Klasik"},
                                {"Premier", "Sedan"}
                        };
                        new idx = random(sizeof(rareRewards));
                        format(rewardName, sizeof(rewardName), "%s", rareRewards[idx][0]);
                        format(rewardLabel, sizeof(rewardLabel), "%s", rareRewards[idx][1]);
                        speed = 6.0 + (random(10) / 10.0);
                        accel = 5.5 + (random(10) / 10.0);
                        handling = 6.0 + (random(10) / 10.0);
                }
                case CASE_RARITY_COMMON:
                {
                        new commonRewards[][] = {
                                {"Bravura", "Gradski auto"},
                                {"Sentinel", "Sedan"},
                                {"Admiral", "Sedan"},
                                {"Washington", "Sedan"}
                        };
                        new idx = random(sizeof(commonRewards));
                        format(rewardName, sizeof(rewardName), "%s", commonRewards[idx][0]);
                        format(rewardLabel, sizeof(rewardLabel), "%s", commonRewards[idx][1]);
                        speed = 4.0 + (random(10) / 10.0);
                        accel = 4.0 + (random(10) / 10.0);
                        handling = 4.5 + (random(10) / 10.0);
                }
        }

        // Sacuvaj nagradu u static varijable
        CaseRewardType[playerid] = rewardType;
        CaseRewardRarity[playerid] = rarity;
        format(CaseRewardName[playerid], sizeof(CaseRewardName[]), "%s", rewardName);
        format(CaseRewardLabel[playerid], sizeof(CaseRewardLabel[]), "%s", rewardLabel);
        CaseRewardSpeed[playerid] = speed;
        CaseRewardAccel[playerid] = accel;
        CaseRewardHandling[playerid] = handling;

        printf("[CEF-CASE] Player %d opening case - reward: %s (%s), rarity: %d",
                playerid, rewardName, rewardLabel, rarity);

        // Kreiraj CEF browser
        CEF_CreateBrowser(playerid, CEF_CASE_BROWSER_ID, CEF_CASE_URL, true);
}

stock CEF_CaseClose(playerid)
{
        CaseOpen[playerid] = false;
        CEF_DestroyBrowser(playerid, CEF_CASE_BROWSER_ID);
}

stock bool:CEF_CaseIsOpen(playerid)
{
        return CaseOpen[playerid];
}

stock CefCase_OnBrowserCreated(playerid, browserid)
{
        if(browserid == CEF_CASE_BROWSER_ID)
        {
                printf("[CEF-CASE] Case browser created for player %d, sending reward data", playerid);

                // Rarity string
                new rarityStr[16];
                switch(CaseRewardRarity[playerid])
                {
                        case CASE_RARITY_COMMON:    format(rarityStr, sizeof(rarityStr), "common");
                        case CASE_RARITY_RARE:      format(rarityStr, sizeof(rarityStr), "rare");
                        case CASE_RARITY_EPIC:      format(rarityStr, sizeof(rarityStr), "epic");
                        case CASE_RARITY_LEGENDARY: format(rarityStr, sizeof(rarityStr), "legendary");
                        default: format(rarityStr, sizeof(rarityStr), "common");
                }

                // Type string
                new typeStr[16];
                switch(CaseRewardType[playerid])
                {
                        case CASE_TYPE_VEHICLE: format(typeStr, sizeof(typeStr), "vehicle");
                        case CASE_TYPE_WEAPON:  format(typeStr, sizeof(typeStr), "weapon");
                        case CASE_TYPE_MONEY:   format(typeStr, sizeof(typeStr), "money");
                        case CASE_TYPE_SKIN:    format(typeStr, sizeof(typeStr), "skin");
                        case CASE_TYPE_VIP:     format(typeStr, sizeof(typeStr), "vip");
                        default: format(typeStr, sizeof(typeStr), "vehicle");
                }

                // Speed strings
                new speedStr[16], accelStr[16], handlingStr[16];
                format(speedStr, sizeof(speedStr), "%.1f", CaseRewardSpeed[playerid]);
                format(accelStr, sizeof(accelStr), "%.1f", CaseRewardAccel[playerid]);
                format(handlingStr, sizeof(handlingStr), "%.1f", CaseRewardHandling[playerid]);

                // Posalji podatke o nagradi CEF-u
                // case:init(name, type, rarity, label, speed, accel, handling)
                CEF_EmitEvent(playerid, CEF_CASE_BROWSER_ID, "case:init",
                        CEF_STR(CaseRewardName[playerid]),
                        CEF_STR(typeStr),
                        CEF_STR(rarityStr),
                        CEF_STR(CaseRewardLabel[playerid]),
                        CEF_STR(speedStr),
                        CEF_STR(accelStr),
                        CEF_STR(handlingStr));

                CEF_FocusBrowser(playerid, CEF_CASE_BROWSER_ID, true);
        }
}

stock CefCase_InitSubscriptions()
{
        // Igrac zatvara case UI
        CEF_RegisterEvent("case:close", "OnCefCaseClose");
}

stock CefCase_OnGameModeInit()
{
        CEF_AddResource("case");
        CefCase_InitSubscriptions();
        printf("[CEF-CASE] Resource registered + events: case -> scriptfiles/cef/case/");
}

stock CefCase_OnPlayerDisconnect(playerid)
{
        CaseOpen[playerid] = false;
}

// ===== Callback za CEF event =====
forward OnCefCaseClose(playerid);
public OnCefCaseClose(playerid)
{
        printf("[CEF-CASE] Player %d closed case UI", playerid);
        CEF_CaseClose(playerid);
        return 1;
}
