#if defined _INC_cef_tstudio
        #endinput
#endif
#define _INC_cef_tstudio

// ===== TEXTURE STUDIO 2.1 - CEF Integration =====
// CEF panel za map management, object properties, texture browser
// Browser ID: 95

#define TSTUDIO_CEF_BROWSER_ID    95
#define TSTUDIO_CEF_URL           "http://cef/tstudio/index.html"

new bool:TStudioCEFActive[MAX_PLAYERS];

// ===== OPEN / CLOSE =====
stock CEF_TStudioOpen(playerid)
{
        if(TStudioCEFActive[playerid]) return 0;
        if(!TStudio_HasAccess(playerid)) return 0;

        TStudioCEFActive[playerid] = true;
        CEF_CreateBrowser(playerid, TSTUDIO_CEF_BROWSER_ID, TSTUDIO_CEF_URL, true);
        printf("[TSTUDIO-CEF] Panel opened for player %d", playerid);
        return 1;
}

stock CEF_TStudioClose(playerid)
{
        if(!TStudioCEFActive[playerid]) return 0;
        TStudioCEFActive[playerid] = false;
        CEF_DestroyBrowser(playerid, TSTUDIO_CEF_BROWSER_ID);
        return 1;
}

stock CEF_TStudioIsOpen(playerid)
{
        return TStudioCEFActive[playerid];
}

// ===== BROWSER CREATED CALLBACK =====
stock CefTStudio_OnBrowserCreated(playerid, browserid)
{
        if(browserid == TSTUDIO_CEF_BROWSER_ID)
        {
                printf("[TSTUDIO-CEF] Browser created for player %d, sending init", playerid);

                // Send init data
                CEF_EmitEvent(playerid, TSTUDIO_CEF_BROWSER_ID, "tstudio:init",
                        CEF_INT(playerid),
                        CEF_INT(PlayerInfo[playerid][xAdmin]));

                CEF_FocusBrowser(playerid, TSTUDIO_CEF_BROWSER_ID, true);

                // Send map list
                TStudio_SendMapList(playerid);
        }
}

// ===== CEF EVENT CALLBACKS =====
forward OnCefTStudioClose(playerid);
public OnCefTStudioClose(playerid)
{
        printf("[TSTUDIO-CEF] Player %d closed CEF panel", playerid);
        TStudio_Close(playerid);
        return 1;
}

// CEF requests map list
forward OnCefTStudioGetMaps(playerid);
public OnCefTStudioGetMaps(playerid)
{
        TStudio_SendMapList(playerid);
        return 1;
}

// CEF creates a new map
forward OnCefTStudioNewMap(playerid, mapname[]);
public OnCefTStudioNewMap(playerid, mapname[])
{
        new result = TStudio_CreateMap(playerid, mapname);
        if(result >= 0)
        {
                new msg[128];
                format(msg, sizeof(msg), " Mapa '%s' kreirana!", mapname);
                SendClientMessage(playerid, 0x10b981FF, msg);
        }
        TStudio_SendMapList(playerid);
        return 1;
}

// CEF loads a map
forward OnCefTStudioLoadMap(playerid, mapindex);
public OnCefTStudioLoadMap(playerid, mapindex)
{
        if(mapindex < 0 || mapindex >= MAX_TSTUDIO_MAPS || !MapData[mapindex][MapExists]) return 1;
        TStudio_LoadMap(mapindex);
        TStudio_SendMapList(playerid);
        return 1;
}

// CEF unloads a map
forward OnCefTStudioUnloadMap(playerid, mapindex);
public OnCefTStudioUnloadMap(playerid, mapindex)
{
        if(mapindex < 0 || mapindex >= MAX_TSTUDIO_MAPS) return 1;
        TStudio_UnloadMap(mapindex);
        TStudio_SendMapList(playerid);
        return 1;
}

// CEF deletes a map
forward OnCefTStudioDeleteMap(playerid, mapindex);
public OnCefTStudioDeleteMap(playerid, mapindex)
{
        if(mapindex < 0 || mapindex >= MAX_TSTUDIO_MAPS) return 1;
        TStudio_DeleteMap(mapindex);
        TStudio_SendMapList(playerid);
        return 1;
}

// CEF selects a map as active
forward OnCefTStudioSelectMap(playerid, mapindex);
public OnCefTStudioSelectMap(playerid, mapindex)
{
        if(mapindex < 0 || mapindex >= MAX_TSTUDIO_MAPS || !MapData[mapindex][MapExists]) return 1;
        PlayerTS[playerid][TsCurrentMap] = mapindex;
        TStudio_SendMapList(playerid);
        return 1;
}

// CEF saves a map
forward OnCefTStudioSaveMap(playerid, mapindex);
public OnCefTStudioSaveMap(playerid, mapindex)
{
        if(mapindex < 0 || mapindex >= MAX_TSTUDIO_MAPS) return 1;
        TStudio_SaveMap(mapindex);
        TStudio_SendMapList(playerid);
        return 1;
}

// CEF exports a map
forward OnCefTStudioExportMap(playerid, mapindex);
public OnCefTStudioExportMap(playerid, mapindex)
{
        if(mapindex < 0 || mapindex >= MAX_TSTUDIO_MAPS) return 1;
        TStudio_ExportMap(mapindex);
        return 1;
}

// CEF creates an object
forward OnCefTStudioCreateObj(playerid, modelid);
public OnCefTStudioCreateObj(playerid, modelid)
{
        new mapindex = PlayerTS[playerid][TsCurrentMap];
        if(mapindex < 0) return 1;

        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        GetXYInFrontOfPlayer(playerid, x, y, 5.0);

        new slot = TStudio_CreateObject(playerid, modelid, x, y, z);
        if(slot >= 0)
        {
                PlayerTS[playerid][TsSelectedObj] = slot;
        }
        return 1;
}

// CEF selects an object
forward OnCefTStudioSelectObj(playerid, objslot);
public OnCefTStudioSelectObj(playerid, objslot)
{
        TStudio_SelectObject(playerid, objslot);
        return 1;
}

// CEF deletes an object
forward OnCefTStudioDeleteObj(playerid, objslot);
public OnCefTStudioDeleteObj(playerid, objslot)
{
        TStudio_DeleteObject(playerid, objslot);
        return 1;
}

// CEF sets texture
forward OnCefTStudioSetTex(playerid, jsondata[]);
public OnCefTStudioSetTex(playerid, jsondata[])
{
        // Parse: objslot,matindex,modelid,txd,texture,color
        new objslot, matindex, modelid, txdname[MAX_TEX_NAME], texname[MAX_TEX_NAME], color;

        // Simple comma-separated parse
        new idx = 0, start = 0, partIdx = 0;
        new parts[6][MAX_TEX_NAME];
        new len = strlen(jsondata);

        while(idx <= len && partIdx < 6)
        {
                new k = 0;
                while(idx < len && jsondata[idx] != ',' && k < MAX_TEX_NAME - 1)
                        parts[partIdx][k++] = jsondata[idx++];
                parts[partIdx][k] = '\0';
                partIdx++;
                if(idx < len) idx++;
        }

        if(partIdx >= 5)
        {
                objslot = strval(parts[0]);
                matindex = strval(parts[1]);
                modelid = strval(parts[2]);
                txdname = parts[3];
                texname = parts[4];
                color = (partIdx > 5) ? strval(parts[5]) : 0;

                TStudio_SetObjectTexture(playerid, objslot, matindex, modelid, txdname, texname, color);
        }
        return 1;
}

// ===== GAMEMODE INIT =====
stock CefTStudio_OnGameModeInit()
{
        CEF_AddResource("tstudio");

        // Register CEF events from browser
        CEF_RegisterEvent("tstudio:close", "OnCefTStudioClose");
        CEF_RegisterEvent("tstudio:getmaps", "OnCefTStudioGetMaps");
        CEF_RegisterEvent("tstudio:newmap", "OnCefTStudioNewMap", Argument_String);
        CEF_RegisterEvent("tstudio:loadmap", "OnCefTStudioLoadMap", Argument_Integer);
        CEF_RegisterEvent("tstudio:unloadmap", "OnCefTStudioUnloadMap", Argument_Integer);
        CEF_RegisterEvent("tstudio:deletemap", "OnCefTStudioDeleteMap", Argument_Integer);
        CEF_RegisterEvent("tstudio:selectmap", "OnCefTStudioSelectMap", Argument_Integer);
        CEF_RegisterEvent("tstudio:savemap", "OnCefTStudioSaveMap", Argument_Integer);
        CEF_RegisterEvent("tstudio:exportmap", "OnCefTStudioExportMap", Argument_Integer);
        CEF_RegisterEvent("tstudio:createobj", "OnCefTStudioCreateObj", Argument_Integer);
        CEF_RegisterEvent("tstudio:selectobj", "OnCefTStudioSelectObj", Argument_Integer);
        CEF_RegisterEvent("tstudio:deleteobj", "OnCefTStudioDeleteObj", Argument_Integer);
        CEF_RegisterEvent("tstudio:settex", "OnCefTStudioSetTex", Argument_String);

        printf("[TSTUDIO-CEF] Resource + events registered (Browser %d)", TSTUDIO_CEF_BROWSER_ID);
}

stock CefTStudio_OnPlayerDisconnect(playerid)
{
        if(TStudioCEFActive[playerid])
        {
                TStudioCEFActive[playerid] = false;
        }
}
