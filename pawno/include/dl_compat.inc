// ============================================================
//  SA-MP 0.3.DL Compatibility Layer
//  Zamjenjuje open.mp / YSF native funkcije sa Pawn fallbackovima
//  Dodaj: #include <dl_compat> PRIJE ostalih include-a
// ============================================================

#if defined _INC_dl_compat
    #endinput
#endif
#define _INC_dl_compat

// ============================================================
//  YSF Natives - Fallback implementacije za SA-MP 0.3.DL
// ============================================================

// ---- frename ----
// YSF native: frename(oldname[], newname[])
// Fallback: koristi CMD rename preko OS-a
#if defined _YSF_INC
    // YSF je vec inkludovan, frename postoji kao native
#else
    stock frename(const oldname[], const newname[]) {
        new cmd[512];
        format(cmd, sizeof(cmd), "ren \"%s\" \"%s\"", oldname, newname);
        // Pokusaj sa rename command
        #if defined WINDOWS
            return !system(cmd);
        #else
            format(cmd, sizeof(cmd), "mv \"%s\" \"%s\"", oldname, newname);
            return !system(cmd);
        #endif
    }
#endif

// ---- IsPlayerSpawned ----
// YSF native: IsPlayerSpawned(playerid)
// Fallback: pratimo spawn state u Pawn
#if defined _YSF_INC
    // YSF je vec inkludovan
#else
    new bool:g_PlayerSpawned[MAX_PLAYERS] = {false, ...};

    stock bool:IsPlayerSpawned(playerid) {
        if(playerid < 0 || playerid >= MAX_PLAYERS) return false;
        return g_PlayerSpawned[playerid];
    }

    // Hook OnPlayerSpawn da trackamo spawn state
    #if defined _ALS_OnPlayerSpawn
        #undef OnPlayerSpawn
    #else
        #define _ALS_OnPlayerSpawn
    #endif
    forward OnPlayerSpawn(playerid);
    forward dl_compat_OnPlayerSpawn(playerid);
    public OnPlayerSpawn(playerid) {
        g_PlayerSpawned[playerid] = true;
        #if defined dl_compat_OnPlayerSpawn
            CallLocalFunction("dl_compat_OnPlayerSpawn", "d", playerid);
        #endif
    }
    #define OnPlayerSpawn dl_compat_OnPlayerSpawn

    // Hook OnPlayerDeath da resetamo spawn state
    #if defined _ALS_OnPlayerDeath
        #undef OnPlayerDeath
    #else
        #define _ALS_OnPlayerDeath
    #endif
    forward OnPlayerDeath(playerid, killerid, reason);
    forward dl_compat_OnPlayerDeath(playerid, killerid, reason);
    public OnPlayerDeath(playerid, killerid, reason) {
        g_PlayerSpawned[playerid] = false;
        #if defined dl_compat_OnPlayerDeath
            CallLocalFunction("dl_compat_OnPlayerDeath", "ddd", playerid, killerid, reason);
        #endif
    }
    #define OnPlayerDeath dl_compat_OnPlayerDeath

    // Hook OnPlayerDisconnect da resetamo spawn state
    #if defined _ALS_OnPlayerDisconnect2
        #undef OnPlayerDisconnect
    #else
        #define _ALS_OnPlayerDisconnect2
    #endif
    forward OnPlayerDisconnect(playerid, reason);
    forward dl_compat_OnPlayerDisconnect(playerid, reason);
    public OnPlayerDisconnect(playerid, reason) {
        g_PlayerSpawned[playerid] = false;
        #if defined dl_compat_OnPlayerDisconnect
            CallLocalFunction("dl_compat_OnPlayerDisconnect", "dd", playerid, reason);
        #endif
    }
    #define OnPlayerDisconnect dl_compat_OnPlayerDisconnect
#endif

// ---- GetVehicleColor ----
// YSF native: GetVehicleColor(vehicleid, &color1, &color2)
// Vec postoji GetVehicleColor.inc ali koristi open.mp native ako je definisan
// Ovaj fallback koristi property storage system
#if !defined _YSF_INC
    // GetVehicleColor je vec definisan kroz GetVehicleColor.inc
    // Ako nije, pravimo nas fallback
    #if !defined _Included_GetVehicleColor
        stock GetVehicleColor(vehicleid, &color1, &color2) {
            new colorStr[24];
            getproperty(_, "", ((((vehicleid) * ((vehicleid) << 1)) << 2) + 65536), colorStr);
            new dashPos = strfind(colorStr, "-");
            if(dashPos != -1) {
                color1 = strval(colorStr[0]);
                color2 = strval(colorStr[dashPos + 1]);
            } else {
                color1 = 0;
                color2 = 0;
            }
            return 1;
        }
    #endif
#endif

// ---- BeginObjectEditing / EndObjectEditing ----
// open.mp natives za object editing
// Fallback: koristimo SA-MP SelectObject / CancelEdit
stock BeginObjectEditing(playerid, objectid) {
    // Na SA-MP 0.3.DL koristimo EditObject
    EditObject(playerid, objectid);
    return 1;
}

stock EndObjectEditing(playerid) {
    // Na SA-MP 0.3.DL koristimo CancelEdit
    CancelEdit(playerid);
    return 1;
}

// ============================================================
//  CEF Natives - Fallback implementacije (no-op na SA-MP 0.3.DL)
//  CEF radi kao CLIENT-SIDE ASI plugin, server-side natives su
//  potrebni samo za kreiranje browsera i emitovanje evenata.
//  Na SA-MP 0.3.DL bez server-side CEF plugina, ovo su no-ops.
// ============================================================

#if !defined _INC_cef
    // CEF include nije inkludovan - definisemo dummy natives
    // Ovo se desi kada je USE_CEF definisan ali cef.inc nema pravi plugin

    // CEF Browser constants
    #define INVALID_BROWSER_ID (-1)

    // Dummy CEF argument type macros
    #define CEF_INT(%0) %0
    #define CEF_FLOAT(%0) %0
    #define CEF_STR(%0) %0

    // CEF Enums (dummy)
    enum E_CEF_INIT_REASON {
        CEF_INIT_OK = 0,
        CEF_INIT_TIMEOUT,
        CEF_INIT_VERSION_MISMATCH,
        CEF_INIT_IP_MISMATCH,
        CEF_INIT_FAILED
    }

    enum E_CEF_ARGUMENT_TYPE {
        CEF_ARG_INT = 0,
        CEF_ARG_FLOAT,
        CEF_ARG_STRING
    }

    // Dummy native declarations - no-op na SA-MP 0.3.DL bez server-side CEF plugina
    stock bool:CEF_PlayerHasPlugin(playerid) { return false; }
    stock CEF_AddResource(const resourceName[]) { return 0; }
    stock CEF_CreateBrowser(playerid, browserid, const url[], bool:focused, bool:controls_chat = true, Float:width = 0.0, Float:height = 0.0) { return 0; }
    stock CEF_CreateWorldBrowser(playerid, browserid, const url[], const textureName[], Float:width, Float:height) { return 0; }
    stock CEF_CreateWorld2DBrowser(playerid, browserid, const url[], Float:worldX, Float:worldY, Float:worldZ, Float:width = 0.0, Float:height = 0.0, Float:offsetZ = 0.0, Float:pivotX = 0.0, Float:pivotY = 0.0) { return 0; }
    stock CEF_SetWorld2DBrowserPos(playerid, browserid, Float:worldX, Float:worldY, Float:worldZ) { return 0; }
    stock CEF_SetBrowserVisible(playerid, browserid, bool:visible) { return 0; }
    stock CEF_DestroyBrowser(playerid, browserid) { return 0; }
    stock CEF_RegisterEvent(const eventName[], const callback[], {E_CEF_ARGUMENT_TYPE, _}:...) { return 0; }
    stock CEF_EmitEvent(playerid, browserid, const eventName[], {E_CEF_ARGUMENT_TYPE, Float, _}:...) { return 0; }
    stock CEF_ReloadBrowser(playerid, browserid, bool:ignore_cache = false) { return 0; }
    stock CEF_FocusBrowser(playerid, id, bool:focused) { return 0; }
    stock CEF_EnableDevTools(playerid, browserid, bool:enabled) { return 0; }
    stock CEF_AttachBrowserToObject(playerid, browserid, objectid) { return 0; }
    stock CEF_DetachBrowserFromObject(playerid, browserid, objectid) { return 0; }
    stock CEF_SetBrowserMuted(playerid, browserid, bool:mute) { return 0; }
    stock CEF_SetBrowserAudioMode(playerid, browserid, E_CEF_AUDIO_MODE:mode) { return 0; }
    stock CEF_SetBrowserAudioSettings(playerid, browserid, Float:max_distance, Float:reference_distance) { return 0; }
    stock CEF_ToggleHudComponent(playerid, componentid, bool:toggle) { return 0; }
    stock CEF_ToggleSpawnScreen(playerid, bool:toggle) { return 0; }
    stock CEF_ClearChat(playerid) { return 0; }
    stock CEF_ToggleChatInput(playerid, bool:toggle) { return 0; }
    stock bool:CEF_IsChatInputOpen(playerid) { return false; }
    stock CEF_EnableKey(playerid, key, bool:enabled) { return 0; }
    stock CEF_SetKeyCapture(playerid, bool:enabled) { return 0; }
    stock CEF_ExitGame(playerid) { return 0; }

    // CEF forward declarations (da ne prijavljuje error za missing callback)
    forward OnCefInitialize(playerid, bool:success, E_CEF_INIT_REASON:reason, const message[]);
    forward OnCefDownloadStart(playerid);
    forward OnCefDownloadFinish(playerid);

#endif
