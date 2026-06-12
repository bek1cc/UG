#if defined _INC_cef_dog
        #endinput
#endif
#define _INC_cef_dog

// DOG_BROWSER_ID je definiran u dog.inc (koji se includa prije ovog fajla)
// Ako dog.inc nije includan, javi gresku
#if !defined DOG_BROWSER_ID
        #error "cef_dog.inc requires dog.inc to be included first! Use: #include <dog> before #include <cef_dog>"
#endif

#define DOG_CEF_URL          "http://cef/dog/index.html"

new bool:DogCEFActive[MAX_PLAYERS];

// ===== OPEN / CLOSE =====
stock CEF_DogPanelOpen(playerid)
{
        if(DogCEFActive[playerid]) return 0;
        DogCEFActive[playerid] = true;

        CEF_CreateBrowser(playerid, DOG_BROWSER_ID, DOG_CEF_URL, true);

        printf("[DOG-CEF] Panel opened for player %d", playerid);
        return 1;
}

stock CEF_DogPanelClose(playerid)
{
        if(!DogCEFActive[playerid]) return 0;
        DogCEFActive[playerid] = false;
        CEF_DestroyBrowser(playerid, DOG_BROWSER_ID);
        return 1;
}

stock CEF_DogPanelIsOpen(playerid)
{
        return DogCEFActive[playerid] ? true : false;
}

// ===== SEND DATA TO CEF =====
stock CEF_DogSendData(playerid)
{
        if(!DogCEFActive[playerid]) return;

        new stateStr[16];
        switch(DogState[playerid])
        {
                case DOG_STATE_FOLLOW: stateStr = "Slijedi";
                case DOG_STATE_SIT:    stateStr = "Sjedi";
                case DOG_STATE_STAY:   stateStr = "Ceka";
                case DOG_STATE_RUN:    stateStr = "Trci";
                case DOG_STATE_DEAD:   stateStr = "Mrtav";
                default: stateStr = "Nepoznato";
        }

        // dog:init(name, breed, hp, state, active)
        CEF_EmitEvent(playerid, DOG_BROWSER_ID, "dog:init",
                CEF_STR(DogName[playerid]),
                CEF_STR(DogTypeNames[DogType[playerid]]),
                CEF_INT(DogHP[playerid]),
                CEF_STR(stateStr),
                CEF_INT(DogActive[playerid] ? 1 : 0));

        CEF_FocusBrowser(playerid, DOG_BROWSER_ID, true);
}

// Update dog data in CEF (called when state changes)
stock CEF_DogUpdateState(playerid)
{
        if(!DogCEFActive[playerid]) return;
        if(!DogActive[playerid]) return;

        new stateStr[16];
        switch(DogState[playerid])
        {
                case DOG_STATE_FOLLOW: stateStr = "Slijedi";
                case DOG_STATE_SIT:    stateStr = "Sjedi";
                case DOG_STATE_STAY:   stateStr = "Ceka";
                case DOG_STATE_RUN:    stateStr = "Trci";
                case DOG_STATE_DEAD:   stateStr = "Mrtav";
                default: stateStr = "Nepoznato";
        }

        // dog:update(name, breed, hp, state, active)
        CEF_EmitEvent(playerid, DOG_BROWSER_ID, "dog:update",
                CEF_STR(DogName[playerid]),
                CEF_STR(DogTypeNames[DogType[playerid]]),
                CEF_INT(DogHP[playerid]),
                CEF_STR(stateStr),
                CEF_INT(1)); // active
}

// ===== BROWSER CREATED CALLBACK =====
stock CefDog_OnBrowserCreated(playerid, browserid)
{
        if(browserid == DOG_BROWSER_ID)
        {
                printf("[DOG-CEF] Browser created for player %d, sending dog data", playerid);
                CEF_DogSendData(playerid);
        }
}

// ===== CEF EVENT CALLBACKS (from browser) =====
forward OnCefDogClose(playerid);
public OnCefDogClose(playerid)
{
        printf("[DOG-CEF] Player %d closed dog panel", playerid);
        CEF_DogPanelClose(playerid);
        return 1;
}

forward OnCefDogCommand(playerid, command[]);
public OnCefDogCommand(playerid, command[])
{
        printf("[DOG-CEF] Player %d sent command: %s", playerid, command);

        if(!DogActive[playerid]) return 1;
        if(DogState[playerid] == DOG_STATE_DEAD)
        {
                // Send update to show dead state
                CEF_DogUpdateState(playerid);
                return 1;
        }

        if(strcmp(command, "follow") == 0)
        {
                DogState[playerid] = DOG_STATE_FOLLOW;
                CEF_DogUpdateState(playerid);
        }
        else if(strcmp(command, "sit") == 0)
        {
                DogState[playerid] = DOG_STATE_SIT;
                CEF_DogUpdateState(playerid);
        }
        else if(strcmp(command, "stay") == 0)
        {
                new Float:x, Float:y, Float:z;
                GetObjectPos(DogObject[playerid], x, y, z);
                DogStayX[playerid] = x;
                DogStayY[playerid] = y;
                DogStayZ[playerid] = z;
                DogState[playerid] = DOG_STATE_STAY;
                CEF_DogUpdateState(playerid);
        }
        else if(strcmp(command, "run") == 0)
        {
                DogState[playerid] = DOG_STATE_RUN;
                CEF_DogUpdateState(playerid);
        }

        return 1;
}

forward OnCefDogRename(playerid, name[]);
public OnCefDogRename(playerid, name[])
{
        printf("[DOG-CEF] Player %d wants to rename dog to: %s", playerid, name);

        if(!DogActive[playerid])
        {
                CEF_EmitEvent(playerid, DOG_BROWSER_ID, "dog:renameResult", CEF_INT(0), CEF_STR("Nemas psa!"));
                return 1;
        }

        new len = strlen(name);
        if(len < 2 || len > 20)
        {
                CEF_EmitEvent(playerid, DOG_BROWSER_ID, "dog:renameResult", CEF_INT(0), CEF_STR("Ime mora biti 2-20 karaktera!"));
                return 1;
        }

        format(DogName[playerid], 32, "%s", name);
        CEF_EmitEvent(playerid, DOG_BROWSER_ID, "dog:renameResult", CEF_INT(1), CEF_STR(name));
        CEF_DogUpdateState(playerid);

        return 1;
}

forward OnCefDogRelease(playerid);
public OnCefDogRelease(playerid)
{
        printf("[DOG-CEF] Player %d released dog via panel", playerid);
        if(DogActive[playerid])
        {
                Dog_Destroy(playerid);
                // Update panel to show no-dog state
                CEF_EmitEvent(playerid, DOG_BROWSER_ID, "dog:init", CEF_STR(""), CEF_STR(""), CEF_INT(0), CEF_STR(""), CEF_INT(0));
        }
        return 1;
}

forward OnCefDogSpawn(playerid);
public OnCefDogSpawn(playerid)
{
        printf("[DOG-CEF] Player %d wants to spawn dog via panel", playerid);

        if(DogActive[playerid])
        {
                CEF_EmitEvent(playerid, DOG_BROWSER_ID, "dog:init",
                        CEF_STR(DogName[playerid]),
                        CEF_STR(DogTypeNames[DogType[playerid]]),
                        CEF_INT(DogHP[playerid]),
                        CEF_STR("Slijedi"),
                        CEF_INT(1));
                return 1;
        }

        // Spawn default Husky
        Dog_Spawn(playerid, DOG_TYPE_HUSKY, "Buddy");

        // Send spawn result to CEF
        CEF_EmitEvent(playerid, DOG_BROWSER_ID, "dog:spawnResult",
                CEF_INT(1), CEF_STR("Buddy"), CEF_STR(DogTypeNames[DOG_TYPE_HUSKY]), CEF_INT(100), CEF_STR("Slijedi"));

        return 1;
}

// ===== GAMEMODE INIT =====
stock CefDog_OnGameModeInit()
{
        CEF_AddResource("dog");

        // Register CEF events from browser
        CEF_RegisterEvent("dog:close", "OnCefDogClose");
        CEF_RegisterEvent("dog:command", "OnCefDogCommand", Argument_String);
        CEF_RegisterEvent("dog:rename", "OnCefDogRename", Argument_String);
        CEF_RegisterEvent("dog:release", "OnCefDogRelease");
        CEF_RegisterEvent("dog:spawn", "OnCefDogSpawn");

        printf("[DOG-CEF] Resource + events registered: dog -> scriptfiles/cef/dog/");
}

stock CefDog_OnPlayerDisconnect(playerid)
{
        if(DogCEFActive[playerid])
        {
                DogCEFActive[playerid] = false;
        }
}
