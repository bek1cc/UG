// gear_manager.inc – upravlja preuzimanjem rudarske opreme

#define GEAR_COOLDOWN 600 // 10 minutes in seconds

new GearCooldownTimer[MAX_PLAYERS];

// Pozove se kad igrač stigne do lokacije opreme
stock GiveMiningGear(playerid)
{
    if (GearCooldownTimer[playerid] > gettime())
    {
        SendClientMessage(playerid, 0xFF0000FF, "[Željezara] Još uvijek čekaš cooldown prije sljedeće opreme.");
        return 0;
    }
    // Daj pickaxe i kacigu (pretpostavimo item ID‑ove 3 i 4)
    AddItem(playerid, ITEM_PICKAXE, 1);
    AddItem(playerid, ITEM_HELMET, 1);
    GearCooldownTimer[playerid] = gettime() + GEAR_COOLDOWN;
    SendClientMessage(playerid, 0x00FF00FF, "[Željezara] Preuzeo si rudarsku opremu. Sada koristi /zeljezara.");
    return 1;
}
