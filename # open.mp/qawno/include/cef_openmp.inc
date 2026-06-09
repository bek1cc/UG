// ============================================================
//  cef_openmp.inc - Open.mp compatible CEF plugin shim
//
//  CEF (Chromium Embedded Framework) plugin requires the
//  Cef.dll/Cef.so component which may not be installed on
//  all open.mp servers. This file replaces all CEF native
//  functions with no-op stock stubs so the gamemode compiles
//  and runs without crashing.
//
//  When OPENMP_BUILD is defined, cef.inc includes this file
//  instead of declaring native functions.
// ============================================================

#if defined _INC_cef
    #endinput
#endif
#define _INC_cef

// Flag so code can check if CEF is running as shim
#define CEF_OPENMP_SHIM 1

// ============================================================
//  ENUMS (must be defined since they're used by CEF macros/includes)
// ============================================================

enum E_CEF_INIT_REASON
{
    CEF_INIT_OK = 0,
    CEF_INIT_TIMEOUT,
    CEF_INIT_VERSION_MISMATCH,
    CEF_INIT_IP_MISMATCH,
    CEF_INIT_HANDSHAKE_FAILED,
    CEF_INIT_UNKNOWN
};

enum E_CEF_CREATE_STATUS
{
    SUCCESS,
    ERROR_GENERIC,
    ERROR_ID_ALREADY_IN_USE
};

enum E_CEF_ARGUMENT_TYPE 
{
    Argument_String,
    Argument_Integer,
    Argument_Float,
    Argument_Bool
};

// Helper macros (same as original cef.inc)
#define CEF_STR(%0)     Argument_String, %0
#define CEF_INT(%0)     Argument_Integer, %0
#define CEF_FLOAT(%0)   Argument_Float, %0
#define CEF_BOOL(%0)    Argument_Bool, %0

enum E_CEF_AUDIO_MODE {
    AUDIO_MODE_WORLD,
    AUDIO_MODE_UI
};

enum E_HUD_COMPONENT
{
    HUD_COMPONENT_ALL,
    HUD_COMPONENT_AMMO,
    HUD_COMPONENT_ARMOUR,
    HUD_COMPONENT_BREATH,
    HUD_COMPONENT_CROSSHAIR,
    HUD_COMPONENT_HEALTH,
    HUD_COMPONENT_MONEY,
    HUD_COMPONENT_RADAR,
    HUD_COMPONENT_WANTED_STARS,
    HUD_COMPONENT_WEAPON
};

// ============================================================
//  STUB NATIVE FUNCTIONS (all no-ops)
// ============================================================

stock bool:CEF_PlayerHasPlugin(playerid)
{
    #pragma unused playerid
    return false;
}

stock CEF_AddResource(const resourceName[])
{
    #pragma unused resourceName
}

stock CEF_CreateBrowser(playerid, browserid, const url[], bool:focused, bool:controls_chat = true, Float:width = 0.0, Float:height = 0.0)
{
    #pragma unused playerid, browserid, url, focused, controls_chat, width, height
}

stock CEF_CreateWorldBrowser(playerid, browserid, const url[], const textureName[], Float:width, Float:height)
{
    #pragma unused playerid, browserid, url, textureName, width, height
}

stock CEF_CreateWorld2DBrowser(playerid, browserid, const url[], Float:worldX, Float:worldY, Float:worldZ, Float:width = 0.0, Float:height = 0.0, Float:offsetZ = 0.0, Float:pivotX = 0.0, Float:pivotY = 0.0)
{
    #pragma unused playerid, browserid, url, worldX, worldY, worldZ, width, height, offsetZ, pivotX, pivotY
}

stock CEF_SetWorld2DBrowserPos(playerid, browserid, Float:worldX, Float:worldY, Float:worldZ)
{
    #pragma unused playerid, browserid, worldX, worldY, worldZ
}

stock CEF_SetBrowserVisible(playerid, browserid, bool:visible)
{
    #pragma unused playerid, browserid, visible
}

stock CEF_DestroyBrowser(playerid, browserid)
{
    #pragma unused playerid, browserid
}

stock CEF_RegisterEvent(const eventName[], const callback[], E_CEF_ARGUMENT_TYPE:...)
{
    #pragma unused eventName, callback
}

stock CEF_EmitEvent(playerid, browserid, const eventName[], {E_CEF_ARGUMENT_TYPE, Float, _}:...)
{
    #pragma unused playerid, browserid, eventName
}

stock CEF_ReloadBrowser(playerid, browserid, bool:ignore_cache = false)
{
    #pragma unused playerid, browserid, ignore_cache
}

stock CEF_FocusBrowser(playerid, id, bool:focused)
{
    #pragma unused playerid, id, focused
}

stock CEF_EnableDevTools(playerid, browserid, bool:enabled)
{
    #pragma unused playerid, browserid, enabled
}

stock CEF_AttachBrowserToObject(playerid, browserid, objectid)
{
    #pragma unused playerid, browserid, objectid
}

stock CEF_DetachBrowserFromObject(playerid, browserid, objectid)
{
    #pragma unused playerid, browserid, objectid
}

// Audio stubs
stock CEF_SetBrowserMuted(playerid, browserid, bool:mute)
{
    #pragma unused playerid, browserid, mute
}

stock CEF_SetBrowserAudioMode(playerid, browserid, E_CEF_AUDIO_MODE:mode)
{
    #pragma unused playerid, browserid, mode
}

stock CEF_SetBrowserAudioSettings(playerid, browserid, Float:max_distance, Float:reference_distance)
{
    #pragma unused playerid, browserid, max_distance, reference_distance
}

// Utils stubs
stock CEF_ToggleHudComponent(playerid, E_HUD_COMPONENT:componentid, bool:toggle)
{
    #pragma unused playerid, componentid, toggle
}

stock CEF_ToggleSpawnScreen(playerid, bool:toggle)
{
    #pragma unused playerid, toggle
}

stock CEF_ClearChat(playerid)
{
    #pragma unused playerid
}

stock CEF_ToggleChatInput(playerid, bool:toggle)
{
    #pragma unused playerid, toggle
}

stock CEF_IsChatInputOpen(playerid)
{
    #pragma unused playerid
    return 0;
}

stock CEF_EnableKey(playerid, key, bool:enabled)
{
    #pragma unused playerid, key, enabled
}

stock CEF_SetKeyCapture(playerid, bool:enabled)
{
    #pragma unused playerid, enabled
}

stock CEF_ExitGame(playerid)
{
    #pragma unused playerid
}

// Callbacks (forwarded so the gamemode can implement them)
forward OnCefInitialize(playerid, bool:success, E_CEF_INIT_REASON:reason, const message[]);
forward OnCefDownloadStart(playerid);
forward OnCefDownloadFinish(playerid);
forward OnCefReady(playerid);
forward OnCefBrowserCreated(playerid, browserid, bool:success, E_CEF_CREATE_STATUS:code, const reason[]);
forward OnCefPressKey(playerid, key, scancode, modifiers, bool:down, bool:repeat);
forward OnCefChatInputState(playerid, bool:open);
