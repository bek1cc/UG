// ============================================================
//  MINIMALNI TEST GAMEMODE za open.mp
//  Koristi se za provjeru da li je crash u gamemodu ili u open.mp
// ============================================================

#include <a_samp>

main()
{
    print("\n----------------------------------");
    print("  TEST GAMEMODE za open.mp");
    print("----------------------------------\n");
}

public OnGameModeInit()
{
    SetGameModeText("Test OMP");
    
    // Dodaj osnovne klase
    AddPlayerClass(0, 1958.3783, 1343.1572, 15.3746, 269.1425, 0, 0, 0, 0, 0, 0);
    AddPlayerClass(1, 1958.3783, 1343.1572, 15.3746, 269.1425, 0, 0, 0, 0, 0, 0);
    AddPlayerClass(2, 1958.3783, 1343.1572, 15.3746, 269.1425, 0, 0, 0, 0, 0, 0);
    
    // Osnovna vozila
    AddStaticVehicle(411, 1958.3783, 1343.1572, 15.3746, 269.1425, -1, -1);
    AddStaticVehicle(560, 1960.3783, 1343.1572, 15.3746, 269.1425, -1, -1);
    
    print("[TEST] Gamemode ucitan!");
    return 1;
}

public OnPlayerConnect(playerid)
{
    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));
    
    new msg[128];
    format(msg, sizeof(msg), "[TEST] Dobrodosao %s! Ovo je test server.", name);
    SendClientMessage(playerid, -1, msg);
    SendClientMessage(playerid, 0x00FF00FF, "Ako vidis ovu poruku, server i klijent rade ispravno!");
    
    print("[TEST] Igrac konektovan:");
    return 1;
}

public OnPlayerSpawn(playerid)
{
    SetPlayerHealth(playerid, 100.0);
    SetPlayerArmour(playerid, 0.0);
    SendClientMessage(playerid, 0x00FF00FF, "[TEST] Spawnan si! Sve radi!");
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    print("[TEST] Igrac diskonektovan");
    return 1;
}

public OnPlayerText(playerid, text[])
{
    new name[MAX_PLAYER_NAME], msg[128];
    GetPlayerName(playerid, name, sizeof(name));
    format(msg, sizeof(msg), "%s: %s", name, text);
    SendClientMessageToAll(-1, msg);
    return 1;
}
