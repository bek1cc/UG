#pragma warning disable 239
#pragma warning disable 203

// ===========================================================================
//  iPHONE TELEFON SISTEM v2.0 za SA-MP
//  File: pawno/include/iphone.inc
//
//  Moderan iPhone-style UI sa aplikacijama:
//  - Phone (pozivi), Messages (SMS), Dark Web, Contacts
//  - Bank, GPS, Oglasi, Kiosk, Settings
//  - Hitne sluzbe (911/555/777)
//
//  Integracija: #include <iphone> u fg-ogc.pwn
// ===========================================================================

// ===========================================================================
//  KONFIGURACIJA
// ===========================================================================
#define IPHONE_MAX_CONTACTS     20
#define IPHONE_MAX_SMS          30
#define IPHONE_MAX_RECENT       10
#define IPHONE_DIALOG_BASE      2700

// Dialog ID-ovi
#define IPHONE_DLG_DIAL         (IPHONE_DIALOG_BASE + 1)
#define IPHONE_DLG_SMS_NUM      (IPHONE_DIALOG_BASE + 2)
#define IPHONE_DLG_SMS_MSG      (IPHONE_DIALOG_BASE + 3)
#define IPHONE_DLG_SMS_INBOX    (IPHONE_DIALOG_BASE + 4)
#define IPHONE_DLG_SMS_CONV     (IPHONE_DIALOG_BASE + 5)
#define IPHONE_DLG_DARKWEB      (IPHONE_DIALOG_BASE + 6)
#define IPHONE_DLG_DARKWEB_BUY  (IPHONE_DIALOG_BASE + 7)
#define IPHONE_DLG_SETTINGS     (IPHONE_DIALOG_BASE + 8)
#define IPHONE_DLG_CONTACTS     (IPHONE_DIALOG_BASE + 9)
#define IPHONE_DLG_ADD_CONT     (IPHONE_DIALOG_BASE + 10)
#define IPHONE_DLG_BANK         (IPHONE_DIALOG_BASE + 11)
#define IPHONE_DLG_BANK_TRAN    (IPHONE_DIALOG_BASE + 12)
#define IPHONE_DLG_GPS          (IPHONE_DIALOG_BASE + 13)
#define IPHONE_DLG_OGLASI       (IPHONE_DIALOG_BASE + 14)
#define IPHONE_DLG_OGLASI_POST  (IPHONE_DIALOG_BASE + 15)
#define IPHONE_DLG_KIOSK        (IPHONE_DIALOG_BASE + 16)
#define IPHONE_DLG_RECENT       (IPHONE_DIALOG_BASE + 17)
#define IPHONE_DLG_TRACK        (IPHONE_DIALOG_BASE + 18)

// ===========================================================================
//  BOJE (iPhone Dark Mode paleta)
// ===========================================================================
#define IPHONE_BG               0x000000FF  // Crni background
#define IPHONE_STATUS_BG        0x1C1C1EFF  // Tamno sivi status bar
#define IPHONE_APP_BG           0x2C2C2EFF  // App content bg
#define IPHONE_NOTCH            0x000000FF  // Notch boja
#define IPHONE_TEXT_WHITE       0xFFFFFFFF
#define IPHONE_TEXT_GRAY        0x8E8E93FF
#define IPHONE_TEXT_GREEN       0x30D158FF
#define IPHONE_TEXT_BLUE        0x0A84FFFF
#define IPHONE_TEXT_RED         0xFF453AFF
#define IPHONE_TEXT_GOLD        0xFFD60AFF
#define IPHONE_TEXT_ORANGE      0xFF9F0AFF
#define IPHONE_TEXT_PURPLE      0xBF5AF2FF
#define IPHONE_TEXT_CYAN        0x64D2FFFF
#define IPHONE_DOCK_BG          0x1C1C1EB0  // Polu-transparenti dock
#define IPHONE_SEPARATOR        0x38383AFF  // Separator linija
#define IPHONE_HOME_BAR         0xFFFFFF80  // Home indicator

// App ikona boje
#define IPHONE_ICON_PHONE       0x30D158FF  // Zelena
#define IPHONE_ICON_SMS         0x0A84FFFF  // Plava
#define IPHONE_ICON_DARKWEB     0xFF453AFF  // Crvena
#define IPHONE_ICON_SETTINGS    0x8E8E93FF  // Siva
#define IPHONE_ICON_CONTACTS    0x64D2FFFF  // Cyan
#define IPHONE_ICON_BANK        0xFFD60AFF  // Zlata
#define IPHONE_ICON_GPS         0xBF5AF2FF  // Purple
#define IPHONE_ICON_OGLASI      0xFF9F0AFF  // Orange
#define IPHONE_ICON_KIOSK       0x30D158AA  // Zelena transparent
#define IPHONE_ICON_911         0xFF453ABB  // Crvena transparent
#define IPHONE_ICON_555         0xFFD60ABB  // Zlata transparent
#define IPHONE_ICON_777         0xBF5AF2BB  // Purple transparent

// ===========================================================================
//  ENUMERACIJE
// ===========================================================================
enum iPhoneApp {
    IPHONE_APP_HOME,
    IPHONE_APP_PHONE,
    IPHONE_APP_SMS,
    IPHONE_APP_DARKWEB,
    IPHONE_APP_CONTACTS,
    IPHONE_APP_BANK,
    IPHONE_APP_GPS,
    IPHONE_APP_OGLASI,
    IPHONE_APP_KIOSK,
    IPHONE_APP_911,
    IPHONE_APP_555,
    IPHONE_APP_777,
    IPHONE_APP_SETTINGS,
    IPHONE_APP_CALLING
}

enum iPhoneScreenState {
    IPHONE_SCREEN_LOCK,
    IPHONE_SCREEN_HOME,
    IPHONE_SCREEN_APP
}

enum iPhoneContactData {
    iPhoneContactName[24],
    iPhoneContactNumber
}

enum iPhoneSMSData {
    iPhoneSMSFrom,
    iPhoneSMSTo,
    iPhoneSMSText[128],
    iPhoneSMSTime
}

enum iPhoneRecentCall {
    iPhoneRecentNumber,
    iPhoneRecentType, // 0=outgoing, 1=incoming, 2=missed
    iPhoneRecentTime
}

enum iPhonePlayerData {
    bool:iPhone_HasPhone,
    iPhone_Number,
    iPhone_Credit,
    iPhone_Model,
    bool:iPhone_Online,
    iPhoneScreenState:iPhone_Screen,
    iPhoneApp:iPhone_CurrentApp,
    iPhone_OpenTime,
    // Contacts
    iPhone_ContactCount,
    // SMS
    iPhone_SMSCount,
    // Recent calls
    iPhone_RecentCount,
    // Textdraw IDs
    PlayerText:iPhone_TD[55],
    bool:iPhone_TDCreated,
    bool:iPhone_Opened,
    // Calling
    iPhone_SMSInputNumber,
    iPhone_SMSConvWith,
    // Dark Web cooldown
    iPhone_DWCooldown,
    // GPS tracking
    iPhone_TrackTarget
}

// ===========================================================================
//  GLOBALNE VARIJABLE
// ===========================================================================
static iPhoneData[MAX_PLAYERS][iPhonePlayerData];
static g_iPhoneContacts[MAX_PLAYERS][IPHONE_MAX_CONTACTS][iPhoneContactData];
static g_iPhoneSMS[MAX_PLAYERS][IPHONE_MAX_SMS][iPhoneSMSData];
static g_iPhoneRecent[MAX_PLAYERS][IPHONE_MAX_RECENT][iPhoneRecentCall];

// ===========================================================================
//  LOKACIJE ZA DARK WEB DOSTAVU
// ===========================================================================
enum DWLocData {
    Float:dwX,
    Float:dwY,
    Float:dwZ,
    dwName[32]
}
static const iPhone_DWLocations[][DWLocData] = {
    {2155.0, -1765.0, 13.5, "Iza benzinske LS"},
    {-72.0, -1130.0, 1.0, "Dokovi SF"},
    {2645.0, -2025.0, 13.5, "Izasposta LS"},
    {822.0, -1350.0, 13.5, "Parking LS"},
    {-1990.0, 260.0, 35.0, "Zgrada SF"},
    {2425.0, -1520.0, 24.0, "Ulica LS"},
    {-2725.0, 375.0, 4.0, "Luka SF"},
    {520.0, -1480.0, 14.0, "Prolaz LS"},
    {1825.0, -1870.0, 13.5, "Gužva LS"},
    {-1710.0, 410.0, 7.0, "Skloniste SF"}
};

// Dark Web items
enum DarkWebItem {
    DW_Name[32],
    DW_Price,
    DW_Command[48]
}
static const DarkWebItems[][DarkWebItem] = {
    {"Pistol (9mm)", 5000, "/giveweapon 23 50"},
    {"Silenced Pistol", 8000, "/giveweapon 24 50"},
    {"Deagle", 15000, "/giveweapon 25 30"},
    {"Shotgun", 12000, "/giveweapon 25 25"},
    {"AK-47", 25000, "/giveweapon 30 120"},
    {"M4", 30000, "/giveweapon 31 120"},
    {"Sniper Rifle", 50000, "/giveweapon 34 20"},
    {"Marihuana x5", 3000, "/givedrugs weed 5"},
    {"Kokain x5", 8000, "/givedrugs coke 5"},
    {"Tracker uredaj", 10000, "/givetracker"},
    {"Lazna dozvola", 20000, "/fakeid"},
    {"Hack usluga", 15000, "/hacktrack"}
};

// ===========================================================================
//  KOORDINATE ZA PHONE LAYOUT (640x480 SA-MP grid)
// ===========================================================================

// Phone body
#define IPHONE_BODY_X           395.0
#define IPHONE_BODY_Y           20.0
#define IPHONE_BODY_W           210.0
#define IPHONE_BODY_H           420.0

// Status bar
#define IPHONE_STATUS_Y         22.0
#define IPHONE_STATUS_H         22.0

// Notch
#define IPHONE_NOTCH_X          460.0
#define IPHONE_NOTCH_Y          20.0
#define IPHONE_NOTCH_W          80.0
#define IPHONE_NOTCH_H          16.0

// App grid
#define IPHONE_GRID_START_X     412.0
#define IPHONE_GRID_START_Y     60.0
#define IPHONE_GRID_ICON_W      55.0
#define IPHONE_GRID_ICON_H      55.0
#define IPHONE_GRID_COL_SPACE   65.0
#define IPHONE_GRID_ROW_SPACE   80.0

// Dock
#define IPHONE_DOCK_Y           340.0
#define IPHONE_DOCK_ICON_W      42.0
#define IPHONE_DOCK_ICON_H      42.0
#define IPHONE_DOCK_COL_SPACE   48.0

// Home bar
#define IPHONE_HOME_Y           425.0

// ===========================================================================
//  HELPER FUNKCIJE
// ===========================================================================
stock iPhone_GetNumber(playerid) return iPhoneData[playerid][iPhone_Number];
stock iPhone_GetCredit(playerid) return iPhoneData[playerid][iPhone_Credit];
stock iPhone_IsOpen(playerid) return iPhoneData[playerid][iPhone_Opened];

stock iPhone_SetNumber(playerid, num) {
    iPhoneData[playerid][iPhone_Number] = num;
    iPhoneData[playerid][iPhone_HasPhone] = (num != 0);
}

stock iPhone_SetCredit(playerid, credit) {
    iPhoneData[playerid][iPhone_Credit] = credit;
    if(iPhoneData[playerid][iPhone_Opened] && iPhoneData[playerid][iPhone_Screen] == IPHONE_SCREEN_HOME)
        iPhone_UpdateStatusBar(playerid);
}

stock iPhone_SetOnline(playerid, bool:on) {
    iPhoneData[playerid][iPhone_Online] = on;
}

stock iPhone_FormatNumber(number) {
    new str[16];
    if(number == 0) { format(str, sizeof(str), "Nema"); return str; }
    format(str, sizeof(str), "%d", number);
    return str;
}

stock iPhone_FormatTime(timestamp) {
    new str[32], h, m;
    h = (timestamp / 3600) % 24;
    m = (timestamp / 60) % 60;
    format(str, sizeof(str), "%02d:%02d", h, m);
    return str;
}

// ===========================================================================
//  SMS SISTEM
// ===========================================================================
stock iPhone_AddSMS(playerid, from, to, text[]) {
    new idx = iPhoneData[playerid][iPhone_SMSCount];
    if(idx >= IPHONE_MAX_SMS) {
        // Shift all messages up
        for(new i = 0; i < IPHONE_MAX_SMS - 1; i++) {
            g_iPhoneSMS[playerid][i] = g_iPhoneSMS[playerid][i+1];
        }
        idx = IPHONE_MAX_SMS - 1;
    }
    g_iPhoneSMS[playerid][idx][iPhoneSMSFrom] = from;
    g_iPhoneSMS[playerid][idx][iPhoneSMSTo] = to;
    format(g_iPhoneSMS[playerid][idx][iPhoneSMSText], 128, "%s", text);
    g_iPhoneSMS[playerid][idx][iPhoneSMSTime] = gettime();
    if(iPhoneData[playerid][iPhone_SMSCount] < IPHONE_MAX_SMS)
        iPhoneData[playerid][iPhone_SMSCount]++;
}

stock iPhone_AddRecentCall(playerid, number, type) {
    new idx = iPhoneData[playerid][iPhone_RecentCount];
    if(idx >= IPHONE_MAX_RECENT) {
        for(new i = 0; i < IPHONE_MAX_RECENT - 1; i++) {
            g_iPhoneRecent[playerid][i] = g_iPhoneRecent[playerid][i+1];
        }
        idx = IPHONE_MAX_RECENT - 1;
    }
    g_iPhoneRecent[playerid][idx][iPhoneRecentNumber] = number;
    g_iPhoneRecent[playerid][idx][iPhoneRecentType] = type;
    g_iPhoneRecent[playerid][idx][iPhoneRecentTime] = gettime();
    if(iPhoneData[playerid][iPhone_RecentCount] < IPHONE_MAX_RECENT)
        iPhoneData[playerid][iPhone_RecentCount]++;
}

stock iPhone_AddContact(playerid, name[], number) {
    if(iPhoneData[playerid][iPhone_ContactCount] >= IPHONE_MAX_CONTACTS) return 0;
    new idx = iPhoneData[playerid][iPhone_ContactCount];
    format(g_iPhoneContacts[playerid][idx][iPhoneContactName], 24, "%s", name);
    g_iPhoneContacts[playerid][idx][iPhoneContactNumber] = number;
    iPhoneData[playerid][iPhone_ContactCount]++;
    return 1;
}

stock iPhone_FindContactByNumber(playerid, number) {
    for(new i = 0; i < iPhoneData[playerid][iPhone_ContactCount]; i++) {
        if(g_iPhoneContacts[playerid][i][iPhoneContactNumber] == number)
            return i;
    }
    return -1;
}

// ===========================================================================
//  TEXTDRAW KREIRANJE (on-demand, kreira se kad se telefon otvori)
// ===========================================================================
stock iPhone_CreateTDs(playerid) {
    if(iPhoneData[playerid][iPhone_TDCreated]) return;
    iPhoneData[playerid][iPhone_TDCreated] = true;

    new td = 0; // counter

    // 0: Phone body background (black)
    iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, IPHONE_BODY_X, IPHONE_BODY_Y, "_");
    PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.0, 51.5);
    PlayerTextDrawTextSize(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_BODY_W + IPHONE_BODY_X, 0.0);
    PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawUseBox(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_BG);
    PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    td++;

    // 1: Phone border (subtle gray outline)
    iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, IPHONE_BODY_X - 1.0, IPHONE_BODY_Y - 1.0, "_");
    PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.0, 52.0);
    PlayerTextDrawTextSize(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_BODY_W + IPHONE_BODY_X + 2.0, 0.0);
    PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawUseBox(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0x3A3A3CFF);
    PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    td++;

    // 2: Notch (Dynamic Island)
    iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, IPHONE_NOTCH_X, IPHONE_NOTCH_Y, "_");
    PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.0, 1.5);
    PlayerTextDrawTextSize(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_NOTCH_X + IPHONE_NOTCH_W, 0.0);
    PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawUseBox(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_NOTCH);
    PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    td++;

    // 3: Time display (status bar center)
    new timeStr[8];
    new h, m, s;
    gettime(h, m, s);
    format(timeStr, sizeof(timeStr), "%02d:%02d", h, m);
    iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, 485.0, IPHONE_STATUS_Y + 2.0, timeStr);
    PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.25, 1.1);
    PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 2);
    PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_TEXT_WHITE);
    PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    td++;

    // 4: Battery/Credit display (status bar right)
    new creditStr[32];
    format(creditStr, sizeof(creditStr), "$%d", iPhoneData[playerid][iPhone_Credit]);
    iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, 580.0, IPHONE_STATUS_Y + 2.0, creditStr);
    PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.2, 0.9);
    PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 2);
    PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_TEXT_GREEN);
    PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    td++;

    // 5: Signal/Carrier (status bar left)
    iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, 410.0, IPHONE_STATUS_Y + 2.0, "UG");
    PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.2, 0.9);
    PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_TEXT_WHITE);
    PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    td++;

    // 6: Home indicator bar
    iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, 460.0, IPHONE_HOME_Y, "_");
    PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.0, 0.3);
    PlayerTextDrawTextSize(playerid, iPhoneData[playerid][iPhone_TD][td], 540.0, 0.0);
    PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawUseBox(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_HOME_BAR);
    PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetSelectable(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    td++;

    // ===================== APP GRID (3x4 = 12 ikona) =====================
    // Grid positions: Row 0-3, Col 0-2
    // App order for home screen:
    // Row 0: Contacts, Bank, GPS
    // Row 1: Oglasi, Kiosk, 911
    // Row 2: 555, 777, (empty)
    // Row 3: (empty area for dock)

    // App icon definitions: [name, color, row, col]
    // We'll create them in a loop for cleanliness

    new appName[12][16] = {
        "Kontakt", "Banka", "GPS",
        "Oglasi", "Kiosk", "911",
        "Mehanicar", "Taxi", "",
        "", "", ""
    };
    new appColors[12] = {
        IPHONE_ICON_CONTACTS, IPHONE_ICON_BANK, IPHONE_ICON_GPS,
        IPHONE_ICON_OGLASI, IPHONE_ICON_KIOSK, IPHONE_ICON_911,
        IPHONE_ICON_555, IPHONE_ICON_777, 0x00000000,
        0x00000000, 0x00000000, 0x00000000
    };

    // 7-18: App icon backgrounds (3x4 grid)
    for(new row = 0; row < 4; row++) {
        for(new col = 0; col < 3; col++) {
            new idx = row * 3 + col;
            if(strlen(appName[idx]) == 0) {
                // Empty slot - create invisible
                iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, 0.0, 0.0, "_");
                PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
                PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.0, 0.0);
                PlayerTextDrawTextSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.0, 0.0);
                PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
                PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
                td++;
                continue;
            }

            new Float:iconX = IPHONE_GRID_START_X + (col * IPHONE_GRID_COL_SPACE);
            new Float:iconY = IPHONE_GRID_START_Y + (row * IPHONE_GRID_ROW_SPACE);

            // Icon background (colored square with rounded effect using box)
            iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, iconX, iconY, "_");
            PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
            PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.0, 6.5);
            PlayerTextDrawTextSize(playerid, iPhoneData[playerid][iPhone_TD][td], iconX + IPHONE_GRID_ICON_W, 0.0);
            PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
            PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
            PlayerTextDrawUseBox(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
            PlayerTextDrawBoxColor(playerid, iPhoneData[playerid][iPhone_TD][td], appColors[idx]);
            PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
            PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
            PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
            PlayerTextDrawSetSelectable(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
            td++;
        }
    }

    // 19-30: App icon text labels
    new appEmojis[12][8] = {
        "C", "B", "G",
        "O", "K", "!",
        "M", "T", "",
        "", "", ""
    };

    for(new row = 0; row < 4; row++) {
        for(new col = 0; col < 3; col++) {
            new idx = row * 3 + col;
            if(strlen(appName[idx]) == 0) {
                // Invisible label
                iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, 0.0, 0.0, " ");
                PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
                PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.0, 0.0);
                PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
                PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
                td++;
                continue;
            }

            new Float:iconX = IPHONE_GRID_START_X + (col * IPHONE_GRID_COL_SPACE);
            new Float:iconY = IPHONE_GRID_START_Y + (row * IPHONE_GRID_ROW_SPACE);

            // Icon letter/emoji in center of box
            new labelStr[8];
            format(labelStr, sizeof(labelStr), "%s", appEmojis[idx]);
            iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, iconX + IPHONE_GRID_ICON_W / 2.0, iconY + 5.0, labelStr);
            PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
            PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.5, 2.5);
            PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 2);
            PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_TEXT_WHITE);
            PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
            PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
            PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
            PlayerTextDrawSetProportional(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
            td++;
        }
    }

    // 31-42: App icon text labels BELOW the icons
    for(new row = 0; row < 4; row++) {
        for(new col = 0; col < 3; col++) {
            new idx = row * 3 + col;
            if(strlen(appName[idx]) == 0) {
                iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, 0.0, 0.0, " ");
                PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
                PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.0, 0.0);
                PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
                PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
                td++;
                continue;
            }

            new Float:iconX = IPHONE_GRID_START_X + (col * IPHONE_GRID_COL_SPACE);
            new Float:iconY = IPHONE_GRID_START_Y + (row * IPHONE_GRID_ROW_SPACE);

            iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, iconX + IPHONE_GRID_ICON_W / 2.0, iconY + IPHONE_GRID_ICON_H + 2.0, appName[idx]);
            PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
            PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.18, 0.8);
            PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 2);
            PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_TEXT_WHITE);
            PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
            PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
            PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
            PlayerTextDrawSetProportional(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
            td++;
        }
    }

    // ===================== DOCK (4 ikone) =====================
    // 43-46: Dock icon backgrounds
    // Dock apps: Phone, SMS, Dark Web, Settings
    // Dock labels (unused in textdraw creation, kept for reference)
    // dockName: Telefon, SMS, DarkWeb, Podesi
    new dockColors[4] = { IPHONE_ICON_PHONE, IPHONE_ICON_SMS, IPHONE_ICON_DARKWEB, IPHONE_ICON_SETTINGS };
    new dockLetters[4][4] = { "T", "S", "D", "P" };

    // 43: Dock background separator
    iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, IPHONE_BODY_X + 5.0, IPHONE_DOCK_Y - 8.0, "_");
    PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.0, 0.15);
    PlayerTextDrawTextSize(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_BODY_X + IPHONE_BODY_W - 5.0, 0.0);
    PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawUseBox(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_SEPARATOR);
    PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    td++;

    // 44-47: Dock icon backgrounds
    for(new i = 0; i < 4; i++) {
        new Float:iconX = 410.0 + (i * IPHONE_DOCK_COL_SPACE);
        iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, iconX, IPHONE_DOCK_Y, "_");
        PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
        PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.0, 5.0);
        PlayerTextDrawTextSize(playerid, iPhoneData[playerid][iPhone_TD][td], iconX + IPHONE_DOCK_ICON_W, 0.0);
        PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
        PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
        PlayerTextDrawUseBox(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
        PlayerTextDrawBoxColor(playerid, iPhoneData[playerid][iPhone_TD][td], dockColors[i]);
        PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
        PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
        PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
        PlayerTextDrawSetSelectable(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
        td++;
    }

    // 48-51: Dock icon text (letter in center)
    for(new i = 0; i < 4; i++) {
        new Float:iconX = 410.0 + (i * IPHONE_DOCK_COL_SPACE);
        iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, iconX + IPHONE_DOCK_ICON_W / 2.0, IPHONE_DOCK_Y + 3.0, dockLetters[i]);
        PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
        PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.45, 2.2);
        PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 2);
        PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_TEXT_WHITE);
        PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
        PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
        PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
        PlayerTextDrawSetProportional(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
        td++;
    }

    // 52-54: App content screen elements (hidden by default, used when inside an app)
    // 52: App title bar background
    iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, IPHONE_BODY_X + 2.0, IPHONE_STATUS_Y + IPHONE_STATUS_H + 2.0, "_");
    PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.0, 2.5);
    PlayerTextDrawTextSize(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_BODY_X + IPHONE_BODY_W - 2.0, 0.0);
    PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawUseBox(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawBoxColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_STATUS_BG);
    PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawHide(playerid, iPhoneData[playerid][iPhone_TD][td]);
    td++;

    // 53: App title text
    iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, 500.0, IPHONE_STATUS_Y + IPHONE_STATUS_H + 5.0, "APP");
    PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.3, 1.4);
    PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 2);
    PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_TEXT_WHITE);
    PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawHide(playerid, iPhoneData[playerid][iPhone_TD][td]);
    td++;

    // 54: Back button
    iPhoneData[playerid][iPhone_TD][td] = CreatePlayerTextDraw(playerid, IPHONE_BODY_X + 8.0, IPHONE_STATUS_Y + IPHONE_STATUS_H + 5.0, "< Natrag");
    PlayerTextDrawFont(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawLetterSize(playerid, iPhoneData[playerid][iPhone_TD][td], 0.22, 1.1);
    PlayerTextDrawAlignment(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawColor(playerid, iPhoneData[playerid][iPhone_TD][td], IPHONE_TEXT_BLUE);
    PlayerTextDrawSetShadow(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetOutline(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawBackgroundColor(playerid, iPhoneData[playerid][iPhone_TD][td], 0);
    PlayerTextDrawSetProportional(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawSetSelectable(playerid, iPhoneData[playerid][iPhone_TD][td], 1);
    PlayerTextDrawHide(playerid, iPhoneData[playerid][iPhone_TD][td]);
    td++;
}

// ===========================================================================
//  TEXTDRAW UNIŠTAVANJE
// ===========================================================================
stock iPhone_DestroyTDs(playerid) {
    if(!iPhoneData[playerid][iPhone_TDCreated]) return;
    for(new i = 0; i < 55; i++) {
        PlayerTextDrawDestroy(playerid, iPhoneData[playerid][iPhone_TD][i]);
    }
    iPhoneData[playerid][iPhone_TDCreated] = false;
}

// ===========================================================================
//  TEXTDRAW SHOW / HIDE
// ===========================================================================
stock iPhone_ShowHomeScreen(playerid) {
    iPhoneData[playerid][iPhone_Screen] = IPHONE_SCREEN_HOME;
    iPhoneData[playerid][iPhone_CurrentApp] = IPHONE_APP_HOME;

    // Show phone frame elements (0-6)
    for(new i = 0; i <= 6; i++) {
        PlayerTextDrawShow(playerid, iPhoneData[playerid][iPhone_TD][i]);
    }

    // Show app grid icons (7-42) - backgrounds, center letters, and labels
    for(new i = 7; i <= 42; i++) {
        PlayerTextDrawShow(playerid, iPhoneData[playerid][iPhone_TD][i]);
    }

    // Show dock (43-51)
    for(new i = 43; i <= 51; i++) {
        PlayerTextDrawShow(playerid, iPhoneData[playerid][iPhone_TD][i]);
    }

    // Hide app content elements (52-54)
    for(new i = 52; i <= 54; i++) {
        PlayerTextDrawHide(playerid, iPhoneData[playerid][iPhone_TD][i]);
    }

    iPhone_UpdateStatusBar(playerid);
}

stock iPhone_ShowAppScreen(playerid, iPhoneApp:app, title[]) {
    iPhoneData[playerid][iPhone_Screen] = IPHONE_SCREEN_APP;
    iPhoneData[playerid][iPhone_CurrentApp] = app;

    // Show phone frame (0-6)
    for(new i = 0; i <= 6; i++) {
        PlayerTextDrawShow(playerid, iPhoneData[playerid][iPhone_TD][i]);
    }

    // Hide home screen grid and dock
    for(new i = 7; i <= 51; i++) {
        PlayerTextDrawHide(playerid, iPhoneData[playerid][iPhone_TD][i]);
    }

    // Show app title bar (52), title text (53), back button (54)
    PlayerTextDrawShow(playerid, iPhoneData[playerid][iPhone_TD][52]);
    PlayerTextDrawSetString(playerid, iPhoneData[playerid][iPhone_TD][53], title);
    PlayerTextDrawShow(playerid, iPhoneData[playerid][iPhone_TD][53]);
    PlayerTextDrawShow(playerid, iPhoneData[playerid][iPhone_TD][54]);

    iPhone_UpdateStatusBar(playerid);
}

stock iPhone_HideAllTDs(playerid) {
    if(!iPhoneData[playerid][iPhone_TDCreated]) return;
    for(new i = 0; i < 55; i++) {
        PlayerTextDrawHide(playerid, iPhoneData[playerid][iPhone_TD][i]);
    }
}

// ===========================================================================
//  STATUS BAR UPDATE
// ===========================================================================
stock iPhone_UpdateStatusBar(playerid) {
    if(!iPhoneData[playerid][iPhone_Opened]) return;

    // Update time
    new timeStr[8], h, m, s;
    gettime(h, m, s);
    format(timeStr, sizeof(timeStr), "%02d:%02d", h, m);
    PlayerTextDrawSetString(playerid, iPhoneData[playerid][iPhone_TD][3], timeStr);

    // Update credit
    new creditStr[32];
    format(creditStr, sizeof(creditStr), "$%d", iPhoneData[playerid][iPhone_Credit]);
    PlayerTextDrawSetString(playerid, iPhoneData[playerid][iPhone_TD][4], creditStr);
}

// ===========================================================================
//  OTVARANJE / ZATVARANJE TELEFONA
// ===========================================================================
stock iPhone_Open(playerid) {
    if(iPhoneData[playerid][iPhone_Opened]) return;

    // Auto-grant phone ako igrac nema telefon
    if(!iPhoneData[playerid][iPhone_HasPhone]) {
        iPhoneData[playerid][iPhone_HasPhone] = true;
        iPhoneData[playerid][iPhone_Online] = true;
        if(iPhoneData[playerid][iPhone_Number] == 0) {
            iPhoneData[playerid][iPhone_Number] = 100000 + random(899999);
        }
        iPhoneData[playerid][iPhone_Credit] = 100;
        new phoneMsg[64];
        format(phoneMsg, sizeof(phoneMsg), "[IPHONE]{FFFFFF} Dobili ste telefon! Broj: %d", iPhoneData[playerid][iPhone_Number]);
        SendClientMessage(playerid, 0x30D158FF, phoneMsg);
    }

    if(!iPhoneData[playerid][iPhone_Online]) {
        iPhoneData[playerid][iPhone_Online] = true;
        SendClientMessage(playerid, 0x0A84FFFF, "[IPHONE]{FFFFFF} Telefon ukljucen.");
    }

    iPhoneData[playerid][iPhone_Opened] = true;
    iPhoneData[playerid][iPhone_OpenTime] = GetTickCount();

    // Create textdraws on demand
    iPhone_CreateTDs(playerid);

    // Show home screen
    iPhone_ShowHomeScreen(playerid);

    // Enable textdraw selection
    SelectTextDraw(playerid, 0xFFFFFFFF);
}

stock iPhone_Close(playerid) {
    if(!iPhoneData[playerid][iPhone_Opened]) return;

    iPhoneData[playerid][iPhone_Opened] = false;
    iPhoneData[playerid][iPhone_Screen] = IPHONE_SCREEN_LOCK;
    iPhoneData[playerid][iPhone_CurrentApp] = IPHONE_APP_HOME;

    // Hide and destroy textdraws
    iPhone_HideAllTDs(playerid);
    iPhone_DestroyTDs(playerid);

    // Cancel textdraw selection mode
    CancelSelectTextDraw(playerid);
}

// ===========================================================================
//  APP: PHONE (Pozivi)
// ===========================================================================
stock iPhone_AppPhone(playerid) {
    iPhone_ShowAppScreen(playerid, IPHONE_APP_PHONE, "Telefon");

    new listStr[512];
    format(listStr, sizeof(listStr),
        "Unesi broj za poziv\n");

    // Add recent calls
    if(iPhoneData[playerid][iPhone_RecentCount] > 0) {
        strcat(listStr, "\n{8E8E93}-- Nedavni pozivi --\n");
        for(new i = iPhoneData[playerid][iPhone_RecentCount] - 1; i >= 0; i--) {
            new line[64], typeStr[16];
            switch(g_iPhoneRecent[playerid][i][iPhoneRecentType]) {
                case 0: typeStr = "{30D158}Out";
                case 1: typeStr = "{0A84FF}In";
                case 2: typeStr = "{FF453A}Miss";
            }
            format(line, sizeof(line), "%s %s\n", typeStr, iPhone_FormatNumber(g_iPhoneRecent[playerid][i][iPhoneRecentNumber]));
            strcat(listStr, line);
        }
    }

    ShowPlayerDialog(playerid, IPHONE_DLG_DIAL, DIALOG_STYLE_INPUT,
        "{0A84FF}iPhone{FFFFFF} - Poziv",
        listStr,
        "Pozovi", "Natrag");
}

// ===========================================================================
//  APP: SMS
// ===========================================================================
stock iPhone_AppSMS(playerid) {
    iPhone_ShowAppScreen(playerid, IPHONE_APP_SMS, "Poruke");

    new listStr[1024];
    if(iPhoneData[playerid][iPhone_SMSCount] == 0) {
        format(listStr, sizeof(listStr), "{8E8E93}Nemate poruka.\nNova poruka");
    } else {
        format(listStr, sizeof(listStr), "Nova poruka\n");
        strcat(listStr, "{8E8E93}-- Poruke --\n");

        // Show unique conversations (last message from each)
        for(new i = iPhoneData[playerid][iPhone_SMSCount] - 1; i >= 0; i--) {
            new from = g_iPhoneSMS[playerid][i][iPhoneSMSFrom];
            new line[128];
            new contactIdx = iPhone_FindContactByNumber(playerid, from);
            if(contactIdx >= 0)
                format(line, sizeof(line), "%s: {FFFFFF}%s\n",
                    g_iPhoneContacts[playerid][contactIdx][iPhoneContactName],
                    g_iPhoneSMS[playerid][i][iPhoneSMSText]);
            else
                format(line, sizeof(line), "%s: {FFFFFF}%s\n",
                    iPhone_FormatNumber(from),
                    g_iPhoneSMS[playerid][i][iPhoneSMSText]);
            strcat(listStr, line);
        }
    }

    ShowPlayerDialog(playerid, IPHONE_DLG_SMS_INBOX, DIALOG_STYLE_LIST,
        "{0A84FF}iPhone{FFFFFF} - Poruke",
        listStr,
        "Otvori", "Natrag");
}

stock iPhone_NewSMS(playerid) {
    ShowPlayerDialog(playerid, IPHONE_DLG_SMS_NUM, DIALOG_STYLE_INPUT,
        "{0A84FF}iPhone{FFFFFF} - Nova poruka",
        "{FFFFFF}Unesite broj telefona:\n(ili unesite ime kontakta)",
        "Dalje", "Natrag");
}

stock iPhone_SendSMSMessage(playerid, targetNum) {
    iPhoneData[playerid][iPhone_SMSInputNumber] = targetNum;
    ShowPlayerDialog(playerid, IPHONE_DLG_SMS_MSG, DIALOG_STYLE_INPUT,
        "{0A84FF}iPhone{FFFFFF} - Poruka",
        "{FFFFFF}Unesite poruku za slanje:",
        "Posalji", "Natrag");
}

// ===========================================================================
//  APP: DARK WEB
// ===========================================================================
stock iPhone_AppDarkWeb(playerid) {
    iPhone_ShowAppScreen(playerid, IPHONE_APP_DARKWEB, "Dark Web");

    new listStr[1024];
    format(listStr, sizeof(listStr), "{FF453A}DARK WEB{FFFFFF}\n");
    strcat(listStr, "{8E8E93}Sve transakcije su anonimne.\nDostava na random lokaciju u roku od 60s.\n\n");

    for(new i = 0; i < sizeof(DarkWebItems); i++) {
        new line[64];
        format(line, sizeof(line), "%s - {30D158}$%d\n", DarkWebItems[i][DW_Name], DarkWebItems[i][DW_Price]);
        strcat(listStr, line);
    }

    // Cooldown check
    if(iPhoneData[playerid][iPhone_DWCooldown] > gettime()) {
        new remain = iPhoneData[playerid][iPhone_DWCooldown] - gettime();
        new cdStr[64];
        format(cdStr, sizeof(cdStr), "\n{FF453A}Cooldown: %d sekundi\n", remain);
        strcat(listStr, cdStr);
    }

    ShowPlayerDialog(playerid, IPHONE_DLG_DARKWEB, DIALOG_STYLE_LIST,
        "{FF453A}Dark Web{FFFFFF}",
        listStr,
        "Kupi", "Natrag");
}

stock iPhone_DarkWebBuy(playerid, itemIdx) {
    if(itemIdx < 0 || itemIdx >= sizeof(DarkWebItems)) return;

    // Cooldown check
    if(iPhoneData[playerid][iPhone_DWCooldown] > gettime()) {
        new remain = iPhoneData[playerid][iPhone_DWCooldown] - gettime();
        new msg[128];
        format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Moras cekati jos %d sekundi.", remain);
        SendClientMessage(playerid, 0xFF453AFF, msg);
        return;
    }

    new price = DarkWebItems[itemIdx][DW_Price];
    new confirmStr[256];
    format(confirmStr, sizeof(confirmStr),
        "{FFFFFF}Da li zelite kupiti:\n\n{FFD60A}%s{FFFFFF}\n\nCijena: {30D158}$%d\n\nDostava ce biti obavljena na random lokaciji za 60 sekundi.",
        DarkWebItems[itemIdx][DW_Name], price);

    ShowPlayerDialog(playerid, IPHONE_DLG_DARKWEB_BUY, DIALOG_STYLE_MSGBOX,
        "{FF453A}Dark Web{FFFFFF} - Potvrda",
        confirmStr,
        "Kupi", "Odustani");
}

// ===========================================================================
//  APP: CONTACTS
// ===========================================================================
stock iPhone_AppContacts(playerid) {
    iPhone_ShowAppScreen(playerid, IPHONE_APP_CONTACTS, "Kontakti");

    new listStr[1024];
    format(listStr, sizeof(listStr), "+ Dodaj kontakt\n");

    for(new i = 0; i < iPhoneData[playerid][iPhone_ContactCount]; i++) {
        new line[64];
        format(line, sizeof(line), "%s - %s\n",
            g_iPhoneContacts[playerid][i][iPhoneContactName],
            iPhone_FormatNumber(g_iPhoneContacts[playerid][i][iPhoneContactNumber]));
        strcat(listStr, line);
    }

    if(iPhoneData[playerid][iPhone_ContactCount] == 0) {
        strcat(listStr, "{8E8E93}Nemate kontakata.\n");
    }

    ShowPlayerDialog(playerid, IPHONE_DLG_CONTACTS, DIALOG_STYLE_LIST,
        "{64D2FF}iPhone{FFFFFF} - Kontakti",
        listStr,
        "Otvori", "Natrag");
}

stock iPhone_AddContactDialog(playerid) {
    ShowPlayerDialog(playerid, IPHONE_DLG_ADD_CONT, DIALOG_STYLE_INPUT,
        "{64D2FF}iPhone{FFFFFF} - Dodaj kontakt",
        "{FFFFFF}Unesite: Ime_BrojTelefona\nPrimjer: Marko_064123456",
        "Dodaj", "Natrag");
}

// ===========================================================================
//  APP: BANK
// ===========================================================================
stock iPhone_AppBank(playerid) {
    iPhone_ShowAppScreen(playerid, IPHONE_APP_BANK, "Banka");

    new listStr[512];
    format(listStr, sizeof(listStr),
        "Stanje racuna\nPrenesi novac\nPlati kredit\n");

    ShowPlayerDialog(playerid, IPHONE_DLG_BANK, DIALOG_STYLE_LIST,
        "{FFD60A}iPhone{FFFFFF} - Banka",
        listStr,
        "Otvori", "Natrag");
}

// ===========================================================================
//  APP: GPS
// ===========================================================================
stock iPhone_AppGPS(playerid) {
    iPhone_ShowAppScreen(playerid, IPHONE_APP_GPS, "GPS");

    new listStr[512];
    format(listStr, sizeof(listStr),
        "Pronadji igraca (Tracker)\nPostavi waypoint\nUkloni waypoint\n");

    if(iPhoneData[playerid][iPhone_TrackTarget] != INVALID_PLAYER_ID) {
        new name[MAX_PLAYER_NAME];
        GetPlayerName(iPhoneData[playerid][iPhone_TrackTarget], name, sizeof(name));
        new trackLine[64];
        format(trackLine, sizeof(trackLine), "\n{30D158}Pratite: %s", name);
        strcat(listStr, trackLine);
    }

    ShowPlayerDialog(playerid, IPHONE_DLG_GPS, DIALOG_STYLE_LIST,
        "{BF5AF2}iPhone{FFFFFF} - GPS",
        listStr,
        "Otvori", "Natrag");
}

// ===========================================================================
//  APP: OGLASI
// ===========================================================================
stock iPhone_AppOglasi(playerid) {
    iPhone_ShowAppScreen(playerid, IPHONE_APP_OGLASI, "Oglasi");

    new listStr[512];
    format(listStr, sizeof(listStr),
        "Postavi oglas\nPogledaj oglase\n");

    ShowPlayerDialog(playerid, IPHONE_DLG_OGLASI, DIALOG_STYLE_LIST,
        "{FF9F0A}iPhone{FFFFFF} - Oglasi",
        listStr,
        "Otvori", "Natrag");
}

// ===========================================================================
//  APP: KIOSK (Kupovina bona)
// ===========================================================================
stock iPhone_AppKiosk(playerid) {
    iPhone_ShowAppScreen(playerid, IPHONE_APP_KIOSK, "Kiosk");

    new listStr[256];
    format(listStr, sizeof(listStr),
        "$20 bona - $20\n$50 bona - $50\n$100 bona - $100\n$500 bona - $500\n");

    ShowPlayerDialog(playerid, IPHONE_DLG_KIOSK, DIALOG_STYLE_LIST,
        "{30D158}iPhone{FFFFFF} - Kiosk",
        listStr,
        "Kupi", "Natrag");
}

// ===========================================================================
//  APP: SETTINGS
// ===========================================================================
stock iPhone_AppSettings(playerid) {
    iPhone_ShowAppScreen(playerid, IPHONE_APP_SETTINGS, "Podesavanja");

    new onOffStr[24];
    if(iPhoneData[playerid][iPhone_Online]) format(onOffStr, sizeof(onOffStr), "{30D158}UKLJUCEN");
    else format(onOffStr, sizeof(onOffStr), "{FF453A}ISKLJUCEN");

    new listStr[512];
    format(listStr, sizeof(listStr),
        "Telefon: %s\nMoji broj: %s\nModel: %d\nKredit: $%d\n\nO telefonu\n",
        onOffStr,
        iPhone_FormatNumber(iPhoneData[playerid][iPhone_Number]),
        iPhoneData[playerid][iPhone_Model],
        iPhoneData[playerid][iPhone_Credit]);

    ShowPlayerDialog(playerid, IPHONE_DLG_SETTINGS, DIALOG_STYLE_LIST,
        "{8E8E93}iPhone{FFFFFF} - Podesavanja",
        listStr,
        "Otvori", "Natrag");
}

// ===========================================================================
//  HITNE SLUZBE
// ===========================================================================
stock iPhone_Call911(playerid) {
    iPhone_Close(playerid);
    CallLocalFunction("cmd_call", "is", playerid, "911");
}

stock iPhone_Call555(playerid) {
    iPhone_Close(playerid);
    CallLocalFunction("cmd_call", "is", playerid, "555");
}

stock iPhone_Call777(playerid) {
    iPhone_Close(playerid);
    CallLocalFunction("cmd_call", "is", playerid, "777");
}

// ===========================================================================
//  CLICK HANDLER - Obrada klikova na textdrawove
// ===========================================================================
stock iPhone_OnClickTD(playerid, PlayerText:playertextid) {
    if(!iPhoneData[playerid][iPhone_Opened]) return 0;

    // Home indicator bar (index 6) - always goes home
    if(playertextid == iPhoneData[playerid][iPhone_TD][6]) {
        iPhone_ShowHomeScreen(playerid);
        return 1;
    }

    // Back button (index 54) - goes home
    if(playertextid == iPhoneData[playerid][iPhone_TD][54]) {
        iPhone_ShowHomeScreen(playerid);
        return 1;
    }

    // Home screen - app grid clicks
    if(iPhoneData[playerid][iPhone_Screen] == IPHONE_SCREEN_HOME) {

        // App grid icons (7-18) - backgrounds
        // Row 0: Contacts(7), Bank(8), GPS(9)
        // Row 1: Oglasi(10), Kiosk(11), 911(12)
        // Row 2: 555(13), 777(14), empty(15)

        if(playertextid == iPhoneData[playerid][iPhone_TD][7])  { iPhone_AppContacts(playerid); return 1; }
        if(playertextid == iPhoneData[playerid][iPhone_TD][8])  { iPhone_AppBank(playerid); return 1; }
        if(playertextid == iPhoneData[playerid][iPhone_TD][9])  { iPhone_AppGPS(playerid); return 1; }
        if(playertextid == iPhoneData[playerid][iPhone_TD][10]) { iPhone_AppOglasi(playerid); return 1; }
        if(playertextid == iPhoneData[playerid][iPhone_TD][11]) { iPhone_AppKiosk(playerid); return 1; }
        if(playertextid == iPhoneData[playerid][iPhone_TD][12]) { iPhone_Call911(playerid); return 1; }
        if(playertextid == iPhoneData[playerid][iPhone_TD][13]) { iPhone_Call555(playerid); return 1; }
        if(playertextid == iPhoneData[playerid][iPhone_TD][14]) { iPhone_Call777(playerid); return 1; }

        // Dock icons (44-47)
        if(playertextid == iPhoneData[playerid][iPhone_TD][44]) { iPhone_AppPhone(playerid); return 1; }
        if(playertextid == iPhoneData[playerid][iPhone_TD][45]) { iPhone_AppSMS(playerid); return 1; }
        if(playertextid == iPhoneData[playerid][iPhone_TD][46]) { iPhone_AppDarkWeb(playerid); return 1; }
        if(playertextid == iPhoneData[playerid][iPhone_TD][47]) { iPhone_AppSettings(playerid); return 1; }
    }

    return 0;
}

// ===========================================================================
//  DIALOG HANDLER - Obrada dialog responsa
// ===========================================================================
stock iPhone_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {
    if(!iPhoneData[playerid][iPhone_Opened]) return 0;

    switch(dialogid) {
        // ---- PHONE APP ----
        case IPHONE_DLG_DIAL: {
            if(!response) { iPhone_ShowHomeScreen(playerid); return 1; }

            new targetNum = strval(inputtext);
            if(targetNum == 0) {
                SendClientMessage(playerid, 0xFF453AFF, "[IPHONE]{FFFFFF} Netacan broj!");
                iPhone_AppPhone(playerid);
                return 1;
            }

            // Close phone and make the call via existing command
            iPhone_Close(playerid);
            new pStr[16]; format(pStr, sizeof(pStr), "%d", targetNum);
            CallLocalFunction("cmd_call", "is", playerid, pStr);
            return 1;
        }

        // ---- SMS APP ----
        case IPHONE_DLG_SMS_INBOX: {
            if(!response) { iPhone_ShowHomeScreen(playerid); return 1; }

            if(listitem == 0) {
                // New SMS
                iPhone_NewSMS(playerid);
            } else {
                // View conversation (for now, just show the message)
                // Could expand to full conversation view
                new idx = iPhoneData[playerid][iPhone_SMSCount] - 1 - (listitem - 1);
                if(idx >= 0 && idx < iPhoneData[playerid][iPhone_SMSCount]) {
                    new msg[256];
                    format(msg, sizeof(msg),
                        "{FFFFFF}Od: %s\nPoruka: %s\n\nPosalji odgovor?",
                        iPhone_FormatNumber(g_iPhoneSMS[playerid][idx][iPhoneSMSFrom]),
                        g_iPhoneSMS[playerid][idx][iPhoneSMSText]);

                    iPhoneData[playerid][iPhone_SMSConvWith] = g_iPhoneSMS[playerid][idx][iPhoneSMSFrom];
                    ShowPlayerDialog(playerid, IPHONE_DLG_SMS_CONV, DIALOG_STYLE_MSGBOX,
                        "{0A84FF}iPhone{FFFFFF} - Poruka",
                        msg,
                        "Odgovori", "Natrag");
                }
            }
            return 1;
        }

        case IPHONE_DLG_SMS_CONV: {
            if(!response) {
                iPhone_AppSMS(playerid);
            } else {
                // Reply - go to SMS input
                iPhoneData[playerid][iPhone_SMSInputNumber] = iPhoneData[playerid][iPhone_SMSConvWith];
                ShowPlayerDialog(playerid, IPHONE_DLG_SMS_MSG, DIALOG_STYLE_INPUT,
                    "{0A84FF}iPhone{FFFFFF} - Odgovor",
                    "{FFFFFF}Unesite poruku:",
                    "Posalji", "Natrag");
            }
            return 1;
        }

        case IPHONE_DLG_SMS_NUM: {
            if(!response) { iPhone_AppSMS(playerid); return 1; }

            new targetNum = strval(inputtext);
            if(targetNum == 0) {
                // Check if it's a contact name
                for(new i = 0; i < iPhoneData[playerid][iPhone_ContactCount]; i++) {
                    if(strfind(g_iPhoneContacts[playerid][i][iPhoneContactName], inputtext, true) == 0) {
                        targetNum = g_iPhoneContacts[playerid][i][iPhoneContactNumber];
                        break;
                    }
                }
                if(targetNum == 0) {
                    SendClientMessage(playerid, 0xFF453AFF, "[IPHONE]{FFFFFF} Netacan broj ili kontakt!");
                    iPhone_NewSMS(playerid);
                    return 1;
                }
            }

            iPhone_SendSMSMessage(playerid, targetNum);
            return 1;
        }

        case IPHONE_DLG_SMS_MSG: {
            if(!response) { iPhone_AppSMS(playerid); return 1; }

            if(strlen(inputtext) < 1) {
                SendClientMessage(playerid, 0xFF453AFF, "[IPHONE]{FFFFFF} Poruka ne moze biti prazna!");
                iPhone_SendSMSMessage(playerid, iPhoneData[playerid][iPhone_SMSInputNumber]);
                return 1;
            }

            // Send SMS via existing command
            new pStr[160];
            format(pStr, sizeof(pStr), "%d %s", iPhoneData[playerid][iPhone_SMSInputNumber], inputtext);
            iPhone_Close(playerid);
            CallLocalFunction("cmd_sms", "is", playerid, pStr);
            return 1;
        }

        // ---- DARK WEB APP ----
        case IPHONE_DLG_DARKWEB: {
            if(!response) { iPhone_ShowHomeScreen(playerid); return 1; }

            iPhone_DarkWebBuy(playerid, listitem);
            return 1;
        }

        case IPHONE_DLG_DARKWEB_BUY: {
            if(!response) { iPhone_AppDarkWeb(playerid); return 1; }

            // Process Dark Web purchase
            // itemIdx tracking uses listitem from dialog response

            SendClientMessage(playerid, 0xFF453AFF, "[DARK WEB]{FFFFFF} Kupovina u toku... Dostava za 60 sekundi.");

            // Set cooldown (2 minutes)
            iPhoneData[playerid][iPhone_DWCooldown] = gettime() + 120;

            // Pick random delivery location
            new locIdx = random(sizeof(iPhone_DWLocations));
            new Float:locX = iPhone_DWLocations[locIdx][dwX];
            new Float:locY = iPhone_DWLocations[locIdx][dwY];
            new Float:locZ = iPhone_DWLocations[locIdx][dwZ];

            // Set checkpoint for delivery
            SetPlayerCheckpoint(playerid, locX, locY, locZ, 5.0);

            new msg[128];
            format(msg, sizeof(msg), "[DARK WEB]{FFFFFF} Dostava na: %s. Imate 120s da pokupite.", iPhone_DWLocations[locIdx][dwName]);
            SendClientMessage(playerid, 0xFF9F0AFF, msg);

            // Set map icon
            SetPlayerMapIcon(playerid, 2, locX, locY, locZ, 0, 0xFF453AFF, MAPICON_LOCAL);

            // Timer for delivery expiration (120 seconds)
            SetTimerEx("iPhone_DWDelivery", 120000, false, "d", playerid);

            iPhone_ShowHomeScreen(playerid);
            return 1;
        }

        // ---- CONTACTS APP ----
        case IPHONE_DLG_CONTACTS: {
            if(!response) { iPhone_ShowHomeScreen(playerid); return 1; }

            if(listitem == 0) {
                // Add contact
                iPhone_AddContactDialog(playerid);
            } else {
                // Contact selected - call or SMS
                new idx = listitem - 1;
                if(idx >= 0 && idx < iPhoneData[playerid][iPhone_ContactCount]) {
                    new msg[128];
                    format(msg, sizeof(msg),
                        "{FFFFFF}Kontakt: %s\nBroj: %s\n\nIzaberite akciju:",
                        g_iPhoneContacts[playerid][idx][iPhoneContactName],
                        iPhone_FormatNumber(g_iPhoneContacts[playerid][idx][iPhoneContactNumber]));

                    // Store selected contact for action
                    iPhoneData[playerid][iPhone_SMSInputNumber] = g_iPhoneContacts[playerid][idx][iPhoneContactNumber];

                    ShowPlayerDialog(playerid, IPHONE_DLG_RECENT, DIALOG_STYLE_MSGBOX,
                        "{64D2FF}iPhone{FFFFFF} - Kontakt",
                        msg,
                        "Pozovi", "SMS");
                }
            }
            return 1;
        }

        case IPHONE_DLG_ADD_CONT: {
            if(!response) { iPhone_AppContacts(playerid); return 1; }

            // Parse input: Name_Number
            new name[24], numStr[16];
            new underscorePos = strfind(inputtext, "_");
            if(underscorePos == -1) {
                SendClientMessage(playerid, 0xFF453AFF, "[IPHONE]{FFFFFF} Format: Ime_Broj (npr. Marko_064123456)");
                iPhone_AddContactDialog(playerid);
                return 1;
            }

            strmid(name, inputtext, 0, underscorePos, 24);
            strmid(numStr, inputtext, underscorePos + 1, strlen(inputtext), 16);
            new num = strval(numStr);

            if(strlen(name) < 2 || num == 0) {
                SendClientMessage(playerid, 0xFF453AFF, "[IPHONE]{FFFFFF} Netacan format! Primjer: Marko_064123456");
                iPhone_AddContactDialog(playerid);
                return 1;
            }

            if(iPhone_AddContact(playerid, name, num)) {
                new msg[64];
                format(msg, sizeof(msg), "[IPHONE]{30D158} Kontakt '%s' dodan!", name);
                SendClientMessage(playerid, 0x0A84FFFF, msg);
            } else {
                SendClientMessage(playerid, 0xFF453AFF, "[IPHONE]{FFFFFF} Lista kontakata je puna!");
            }

            iPhone_AppContacts(playerid);
            return 1;
        }

        case IPHONE_DLG_RECENT: {
            // Contact action: Call or SMS
            if(response) {
                // Call
                new pStr[16]; format(pStr, sizeof(pStr), "%d", iPhoneData[playerid][iPhone_SMSInputNumber]);
                iPhone_Close(playerid);
                CallLocalFunction("cmd_call", "is", playerid, pStr);
            } else {
                // SMS
                ShowPlayerDialog(playerid, IPHONE_DLG_SMS_MSG, DIALOG_STYLE_INPUT,
                    "{0A84FF}iPhone{FFFFFF} - Poruka",
                    "{FFFFFF}Unesite poruku:",
                    "Posalji", "Natrag");
            }
            return 1;
        }

        // ---- BANK APP ----
        case IPHONE_DLG_BANK: {
            if(!response) { iPhone_ShowHomeScreen(playerid); return 1; }

            switch(listitem) {
                case 0: {
                    // Check balance - use existing command
                    iPhone_Close(playerid);
                    CallLocalFunction("cmd_balance", "is", playerid, "");
                }
                case 1: {
                    // Transfer money
                    ShowPlayerDialog(playerid, IPHONE_DLG_BANK_TRAN, DIALOG_STYLE_INPUT,
                        "{FFD60A}iPhone{FFFFFF} - Prenos",
                        "{FFFFFF}Unesite: ID_Igraca_Iznos\nPrimjer: 5_5000",
                        "Prenesi", "Natrag");
                }
                case 2: {
                    // Pay loan
                    iPhone_Close(playerid);
                    SendClientMessage(playerid, 0xFFD60AFF, "[BANKA]{FFFFFF} Koristite /payloan komandu u banci.");
                }
            }
            return 1;
        }

        case IPHONE_DLG_BANK_TRAN: {
            if(!response) { iPhone_AppBank(playerid); return 1; }

            // Parse: ID_Amount
            new idStr[8], amtStr[16];
            new underscorePos = strfind(inputtext, "_");
            if(underscorePos == -1) {
                SendClientMessage(playerid, 0xFF453AFF, "[IPHONE]{FFFFFF} Format: ID_Iznos");
                iPhone_AppBank(playerid);
                return 1;
            }

            strmid(idStr, inputtext, 0, underscorePos, 8);
            strmid(amtStr, inputtext, underscorePos + 1, strlen(inputtext), 16);

            new targetId = strval(idStr);
            new amount = strval(amtStr);

            if(targetId < 0 || targetId >= MAX_PLAYERS || !IsPlayerConnected(targetId)) {
                SendClientMessage(playerid, 0xFF453AFF, "[IPHONE]{FFFFFF} Igrac nije online!");
                iPhone_AppBank(playerid);
                return 1;
            }

            // Transfer via existing command
            new pStr[32]; format(pStr, sizeof(pStr), "%d %d", targetId, amount);
            iPhone_Close(playerid);
            CallLocalFunction("cmd_transfer", "is", playerid, pStr);
            return 1;
        }

        // ---- GPS APP ----
        case IPHONE_DLG_GPS: {
            if(!response) { iPhone_ShowHomeScreen(playerid); return 1; }

            switch(listitem) {
                case 0: {
                    // Track player
                    ShowPlayerDialog(playerid, IPHONE_DLG_TRACK, DIALOG_STYLE_INPUT,
                        "{BF5AF2}iPhone{FFFFFF} - Tracker",
                        "{FFFFFF}Unesite ID igraca za pracenje:\n(Trebate tracker uredaj iz Dark Weba)",
                        "Prati", "Natrag");
                }
                case 1: {
                    // Set waypoint
                    iPhone_Close(playerid);
                    SendClientMessage(playerid, 0xBF5AF2FF, "[GPS]{FFFFFF} Koristite /gps komandu za postavljanje waypointa.");
                }
                case 2: {
                    // Remove waypoint
                    DisablePlayerCheckpoint(playerid);
                    RemovePlayerMapIcon(playerid, 2);
                    iPhoneData[playerid][iPhone_TrackTarget] = INVALID_PLAYER_ID;
                    SendClientMessage(playerid, 0xBF5AF2FF, "[GPS]{FFFFFF} Waypoint i tracker uklonjeni.");
                    iPhone_AppGPS(playerid);
                }
            }
            return 1;
        }

        case IPHONE_DLG_TRACK: {
            if(!response) { iPhone_AppGPS(playerid); return 1; }

            new targetId = strval(inputtext);
            if(targetId < 0 || targetId >= MAX_PLAYERS || !IsPlayerConnected(targetId)) {
                SendClientMessage(playerid, 0xFF453AFF, "[GPS]{FFFFFF} Igrac nije online!");
                iPhone_AppGPS(playerid);
                return 1;
            }

            new Float:x, Float:y, Float:z;
            GetPlayerPos(targetId, x, y, z);
            SetPlayerMapIcon(playerid, 2, x, y, z, 0, 0xBF5AF2FF, MAPICON_LOCAL);
            iPhoneData[playerid][iPhone_TrackTarget] = targetId;

            new name[MAX_PLAYER_NAME];
            GetPlayerName(targetId, name, sizeof(name));
            new msg[128];
            format(msg, sizeof(msg), "[GPS]{FFFFFF} Pratite: %s (ID:%d). Lokacija oznacena na mapi.", name, targetId);
            SendClientMessage(playerid, 0xBF5AF2FF, msg);

            iPhone_ShowHomeScreen(playerid);
            return 1;
        }

        // ---- OGLASI APP ----
        case IPHONE_DLG_OGLASI: {
            if(!response) { iPhone_ShowHomeScreen(playerid); return 1; }

            switch(listitem) {
                case 0: {
                    // Post ad
                    ShowPlayerDialog(playerid, IPHONE_DLG_OGLASI_POST, DIALOG_STYLE_INPUT,
                        "{FF9F0A}iPhone{FFFFFF} - Oglas",
                        "{FFFFFF}Unesite tekst oglasa:",
                        "Postavi", "Natrag");
                }
                case 1: {
                    // View ads - use existing command
                    iPhone_Close(playerid);
                    CallLocalFunction("cmd_oglasi", "is", playerid, "");
                }
            }
            return 1;
        }

        case IPHONE_DLG_OGLASI_POST: {
            if(!response) { iPhone_AppOglasi(playerid); return 1; }

            if(strlen(inputtext) < 3) {
                SendClientMessage(playerid, 0xFF453AFF, "[IPHONE]{FFFFFF} Oglas je prekratak!");
                iPhone_AppOglasi(playerid);
                return 1;
            }

            // Post via existing command
            new pStr[160];
            format(pStr, sizeof(pStr), "%s", inputtext);
            iPhone_Close(playerid);
            CallLocalFunction("cmd_smsoglas", "is", playerid, pStr);
            return 1;
        }

        // ---- KIOSK APP ----
        case IPHONE_DLG_KIOSK: {
            if(!response) { iPhone_ShowHomeScreen(playerid); return 1; }

            new amounts[] = {20, 50, 100, 500};
            if(listitem < 0 || listitem >= 4) return 1;

            new amount = amounts[listitem];

            // Check if player has enough money
            if(GetPlayerMoney(playerid) < amount) {
                SendClientMessage(playerid, 0xFF453AFF, "[KIOSK]{FFFFFF} Nemate dovoljno novca!");
                iPhone_AppKiosk(playerid);
                return 1;
            }

            GivePlayerMoney(playerid, -amount);
            iPhoneData[playerid][iPhone_Credit] += amount;

            new msg[64];
            format(msg, sizeof(msg), "[KIOSK]{30D158} Kupili ste $%d bona. Stanje: $%d", amount, iPhoneData[playerid][iPhone_Credit]);
            SendClientMessage(playerid, 0x30D158FF, msg);

            iPhone_UpdateStatusBar(playerid);
            iPhone_AppKiosk(playerid);
            return 1;
        }

        // ---- SETTINGS APP ----
        case IPHONE_DLG_SETTINGS: {
            if(!response) { iPhone_ShowHomeScreen(playerid); return 1; }

            switch(listitem) {
                case 0: {
                    // Toggle phone on/off
                    iPhoneData[playerid][iPhone_Online] = !iPhoneData[playerid][iPhone_Online];
                    if(!iPhoneData[playerid][iPhone_Online]) {
                        iPhone_Close(playerid);
                        SendClientMessage(playerid, 0xFF453AFF, "[IPHONE]{FFFFFF} Telefon iskljucen.");
                    } else {
                        SendClientMessage(playerid, 0x30D158FF, "[IPHONE]{FFFFFF} Telefon ukljucen.");
                        iPhone_AppSettings(playerid);
                    }
                }
                default: {
                    iPhone_AppSettings(playerid);
                }
            }
            return 1;
        }
    }

    return 0;
}

// ===========================================================================
//  DARK WEB DELIVERY TIMER
// ===========================================================================
forward iPhone_DWDelivery(playerid);
public iPhone_DWDelivery(playerid) {
    DisablePlayerCheckpoint(playerid);
    RemovePlayerMapIcon(playerid, 2);
    iPhoneData[playerid][iPhone_TrackTarget] = INVALID_PLAYER_ID;
    SendClientMessage(playerid, 0xFF453AFF, "[DARK WEB]{FFFFFF} Vrijeme za dostavu je isteklo!");
    return 1;
}

// ===========================================================================
//  INCOMING CALL NOTIFICATION (poziv od drugog igraca)
// ===========================================================================
stock iPhone_IncomingCall(playerid, callerid) {
    if(!iPhoneData[playerid][iPhone_HasPhone] || !iPhoneData[playerid][iPhone_Online]) return;

    new callerName[MAX_PLAYER_NAME];
    GetPlayerName(callerid, callerName, sizeof(callerName));

    new msg[128];
    format(msg, sizeof(msg), "\n\n{30D158}INCOMING CALL\n\n{FFFFFF}Od: %s (ID:%d)\n\n{0A84FF}Otvori telefon da odgovoris!", callerName, callerid);
    GameTextForPlayer(playerid, msg, 5000, 3);
}

// ===========================================================================
//  INCOMING SMS NOTIFICATION
// ===========================================================================
stock iPhone_IncomingSMS(playerid, fromNum, text[]) {
    if(!iPhoneData[playerid][iPhone_HasPhone] || !iPhoneData[playerid][iPhone_Online]) return;

    // Add to SMS history
    iPhone_AddSMS(playerid, fromNum, iPhoneData[playerid][iPhone_Number], text);

    new contactIdx = iPhone_FindContactByNumber(playerid, fromNum);
    new fromName[32];
    if(contactIdx >= 0)
        format(fromName, sizeof(fromName), "%s", g_iPhoneContacts[playerid][contactIdx][iPhoneContactName]);
    else
        format(fromName, sizeof(fromName), "%s", iPhone_FormatNumber(fromNum));

    new msg[160];
    format(msg, sizeof(msg), "[SMS] %s: %s", fromName, text);
    SendClientMessage(playerid, 0x0A84FFFF, msg);

    // Play notification sound
    PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
}

// ===========================================================================
//  INIT / CONNECT / DISCONNECT
// ===========================================================================
stock iPhone_OnGameModeInit() {
    print("[iPHONE v2.0] iPhone telefon sistem ucitan!");
    return 1;
}

stock iPhone_OnPlayerConnect(playerid) {
    // Reset all data
    // Svi igraci automatski dobijaju telefon
    iPhoneData[playerid][iPhone_HasPhone] = true;
    iPhoneData[playerid][iPhone_Number] = 10000 + playerid; // Auto-generisan broj
    iPhoneData[playerid][iPhone_Credit] = 0;
    iPhoneData[playerid][iPhone_Model] = 0;
    iPhoneData[playerid][iPhone_Online] = true;
    iPhoneData[playerid][iPhone_Screen] = IPHONE_SCREEN_LOCK;
    iPhoneData[playerid][iPhone_CurrentApp] = IPHONE_APP_HOME;
    iPhoneData[playerid][iPhone_Opened] = false;
    iPhoneData[playerid][iPhone_TDCreated] = false;
    iPhoneData[playerid][iPhone_ContactCount] = 0;
    iPhoneData[playerid][iPhone_SMSCount] = 0;
    iPhoneData[playerid][iPhone_RecentCount] = 0;
    iPhoneData[playerid][iPhone_DWCooldown] = 0;
    iPhoneData[playerid][iPhone_TrackTarget] = INVALID_PLAYER_ID;
    iPhoneData[playerid][iPhone_SMSInputNumber] = 0;
    iPhoneData[playerid][iPhone_SMSConvWith] = 0;
    iPhoneData[playerid][iPhone_OpenTime] = 0;
    return 1;
}

stock iPhone_OnPlayerDisconnect(playerid) {
    if(iPhoneData[playerid][iPhone_Opened]) {
        iPhone_Close(playerid);
    }
    return 1;
}

// ===========================================================================
//  KEY HANDLER - ESC za zatvaranje, Y za otvaranje
// ===========================================================================
stock iPhone_OnKeyStateChange(playerid, newkeys, oldkeys) {
    // ESC key (no) to close phone
    if((newkeys & KEY_NO) && !(oldkeys & KEY_NO)) {
        if(iPhoneData[playerid][iPhone_Opened]) {
            iPhone_Close(playerid);
            return 1;
        }
    }

    // Y key (yes) to open phone - only if not in other interaction
    if((newkeys & KEY_YES) && !(oldkeys & KEY_YES)) {
        if(!iPhoneData[playerid][iPhone_Opened] && iPhoneData[playerid][iPhone_HasPhone]) {
            iPhone_Open(playerid);
            return 1;
        }
    }

    return 0;
}

// ===========================================================================
//  COMMAND: /phone - Otvaranje/zatvaranje telefona
// ===========================================================================
stock iPhone_CommandPhone(playerid) {
    // Ako je CEF tablet dostupan, koristi CEF umjesto textdrawova
    #if defined USE_CEF
        if(CefTablet[playerid][ct_IsOpen]) {
            CEF_TabletClose(playerid);
            SendClientMessage(playerid, 0x54A0FFFF, "[PHONE]{FFFFFF} Tablet zatvoren.");
        } else {
            // Auto-grant phone prije otvaranja CEF tableta
            if(!iPhoneData[playerid][iPhone_HasPhone]) {
                iPhoneData[playerid][iPhone_HasPhone] = true;
                iPhoneData[playerid][iPhone_Online] = true;
                if(iPhoneData[playerid][iPhone_Number] == 0) {
                    iPhoneData[playerid][iPhone_Number] = 100000 + random(899999);
                }
                iPhoneData[playerid][iPhone_Credit] = 100;
            }
            if(!iPhoneData[playerid][iPhone_Online]) {
                iPhoneData[playerid][iPhone_Online] = true;
            }
            CEF_TabletOpen(playerid);
        }
    #else
        if(iPhoneData[playerid][iPhone_Opened]) {
            iPhone_Close(playerid);
        } else {
            iPhone_Open(playerid);
        }
    #endif
    return 1;
}

// ===========================================================================
//  SAVE / LOAD (za trajno cuvanje kontakata i poruka)
// ===========================================================================
stock iPhone_SaveData(playerid) {
    new file[64];
    format(file, sizeof(file), "iPhone/%d.ini", iPhoneData[playerid][iPhone_Number]);
    if(!fexist(file)) return;

    new iniFile = ini_openFile(file);

    // Save contacts
    ini_setInteger(iniFile, "ContactCount", iPhoneData[playerid][iPhone_ContactCount]);
    for(new i = 0; i < iPhoneData[playerid][iPhone_ContactCount]; i++) {
        new key[32];
        format(key, sizeof(key), "ContactName_%d", i);
        ini_setString(iniFile, key, g_iPhoneContacts[playerid][i][iPhoneContactName]);
        format(key, sizeof(key), "ContactNum_%d", i);
        ini_setInteger(iniFile, key, g_iPhoneContacts[playerid][i][iPhoneContactNumber]);
    }

    ini_closeFile(iniFile);
}

stock iPhone_LoadData(playerid) {
    if(iPhoneData[playerid][iPhone_Number] == 0) return;

    new file[64];
    format(file, sizeof(file), "iPhone/%d.ini", iPhoneData[playerid][iPhone_Number]);
    if(!fexist(file)) return;

    iPhoneData[playerid][iPhone_ContactCount] = dini_Int(file, "ContactCount");
    if(iPhoneData[playerid][iPhone_ContactCount] > IPHONE_MAX_CONTACTS)
        iPhoneData[playerid][iPhone_ContactCount] = IPHONE_MAX_CONTACTS;

    for(new i = 0; i < iPhoneData[playerid][iPhone_ContactCount]; i++) {
        new key[32];
        format(key, sizeof(key), "ContactName_%d", i);
        format(g_iPhoneContacts[playerid][i][iPhoneContactName], 24, "%s", dini_Get(file, key));
        format(key, sizeof(key), "ContactNum_%d", i);
        g_iPhoneContacts[playerid][i][iPhoneContactNumber] = dini_Int(file, key);
    }
}

// ===========================================================================
//  SYNC - Poziva se iz fg-ogc.pwn kad se ucitavaju podaci igraca
// ===========================================================================
stock iPhone_SyncPlayerData(playerid, number, credit, model, bool:online) {
    // Svi igraci UVIJEK imaju telefon - ako nema broj, automatski dodijeli
    if(number == 0) {
        number = 10000 + playerid;
    }
    iPhoneData[playerid][iPhone_Number] = number;
    iPhoneData[playerid][iPhone_Credit] = credit;
    iPhoneData[playerid][iPhone_Model] = model;
    iPhoneData[playerid][iPhone_Online] = true; // Uvijek online po defaultu
    iPhoneData[playerid][iPhone_HasPhone] = true; // Svi uvijek imaju telefon

    // Load saved contacts/SMS
    iPhone_LoadData(playerid);
}
