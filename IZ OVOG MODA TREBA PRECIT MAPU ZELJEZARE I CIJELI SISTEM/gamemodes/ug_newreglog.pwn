//==============================================================================//
//    Unicate Gaming RPG - Premium Register/Login TextDraw System v3.0          //
//    Developer: Beka  |  Vlasnici: Beka & Sloba                               //
//    Premium 2025 UI/UX Design                                                 //
//==============================================================================//

#define RG_SCREEN_NONE   0
#define RG_SCREEN_REG    1
#define RG_SCREEN_LOGIN  2

#define RG_TD_MAX_LOGIN    60
#define RG_TD_MAX_REGISTER 72

new RG_Screen[MAX_PLAYERS];

stock NewLoginTDCreate(playerid)
{
    // ===== LEFT PANEL =====
    // [0] Full screen dark background
    NewLoginTD[playerid][0] = CreatePlayerTextDraw(playerid, 0.000000, 0.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][0], 640.000000, 480.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][0], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][0], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][0], 0x060620FF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][0], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][0], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][0], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][0], 0);

    // [1] Left panel background
    NewLoginTD[playerid][1] = CreatePlayerTextDraw(playerid, 0.000000, 0.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][1], 220.000000, 480.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][1], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][1], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][1], 0x0c1025FF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][1], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][1], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][1], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][1], 0);

    // [2] Vertical divider glow (smanjena vidljivost - manje plave linije)
    NewLoginTD[playerid][2] = CreatePlayerTextDraw(playerid, 218.000000, 0.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][2], 2.000000, 480.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][2], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][2], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][2], 0x0099ff40);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][2], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][2], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][2], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][2], 0);

    // [3] "UNICATE" logo text
    NewLoginTD[playerid][3] = CreatePlayerTextDraw(playerid, 22.000000, 55.000000, "UNICATE");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][3], 0.750000, 2.800000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][3], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][3], 0x00bbffFF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][3], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][3], 0x0044ff33);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][3], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][3], 1);

    // [4] "GAMING" logo text
    NewLoginTD[playerid][4] = CreatePlayerTextDraw(playerid, 22.000000, 90.000000, "GAMING");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][4], 0.750000, 2.800000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][4], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][4], -1);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][4], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][4], 0x00000033);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][4], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][4], 1);

    // [5] "R P G" subtitle
    NewLoginTD[playerid][5] = CreatePlayerTextDraw(playerid, 22.000000, 125.000000, "R O L E P L A Y");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][5], 0.130000, 0.700000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][5], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][5], 0x0099ff80);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][5], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][5], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][5], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][5], 1);

    // [6] Accent line (left panel)
    NewLoginTD[playerid][6] = CreatePlayerTextDraw(playerid, 22.000000, 148.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][6], 176.000000, 3.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][6], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][6], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][6], 0x0099ff40);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][6], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][6], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][6], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][6], 0);

    // [7] Bottom accent line (left panel)
    NewLoginTD[playerid][7] = CreatePlayerTextDraw(playerid, 22.000000, 320.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][7], 176.000000, 2.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][7], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][7], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][7], 0x0099ff40);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][7], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][7], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][7], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][7], 0);

    // [8] "VLASNICI" label
    NewLoginTD[playerid][8] = CreatePlayerTextDraw(playerid, 22.000000, 163.000000, "VLASNICI");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][8], 0.130000, 0.650000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][8], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][8], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][8], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][8], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][8], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][8], 1);

    // [9] "Beka & Sloba" names
    NewLoginTD[playerid][9] = CreatePlayerTextDraw(playerid, 22.000000, 177.000000, "Beka & Sloba");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][9], 0.250000, 1.200000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][9], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][9], -1);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][9], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][9], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][9], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][9], 1);

    // [10] "DEVELOPER" label
    NewLoginTD[playerid][10] = CreatePlayerTextDraw(playerid, 22.000000, 200.000000, "DEVELOPER");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][10], 0.130000, 0.650000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][10], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][10], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][10], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][10], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][10], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][10], 1);

    // [11] "Beka" name
    NewLoginTD[playerid][11] = CreatePlayerTextDraw(playerid, 22.000000, 214.000000, "Beka");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][11], 0.250000, 1.200000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][11], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][11], -1);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][11], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][11], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][11], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][11], 1);

    // [12] "MAPPER" label
    NewLoginTD[playerid][12] = CreatePlayerTextDraw(playerid, 22.000000, 237.000000, "MAPPER");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][12], 0.130000, 0.650000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][12], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][12], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][12], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][12], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][12], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][12], 1);

    // [13] "Beka" name
    NewLoginTD[playerid][13] = CreatePlayerTextDraw(playerid, 22.000000, 251.000000, "Beka");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][13], 0.250000, 1.200000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][13], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][13], -1);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][13], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][13], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][13], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][13], 1);

    // [14] Website
    NewLoginTD[playerid][14] = CreatePlayerTextDraw(playerid, 22.000000, 335.000000, "www.ug-ogc.com");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][14], 0.140000, 0.750000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][14], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][14], 0x6688aaFF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][14], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][14], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][14], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][14], 1);

    // [15] Left decorative diamond 1
    NewLoginTD[playerid][15] = CreatePlayerTextDraw(playerid, 35.000000, 275.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][15], 16.000000, 16.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][15], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][15], 0x0099ff20);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][15], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][15], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][15], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][15], 0);

    // [16] Left decorative diamond 2
    NewLoginTD[playerid][16] = CreatePlayerTextDraw(playerid, 60.000000, 275.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][16], 16.000000, 16.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][16], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][16], 0x0099ff15);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][16], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][16], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][16], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][16], 0);

    // [17] Left decorative diamond 3
    NewLoginTD[playerid][17] = CreatePlayerTextDraw(playerid, 85.000000, 275.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][17], 16.000000, 16.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][17], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][17], 0x0099ff10);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][17], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][17], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][17], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][17], 0);

    // ===== RIGHT CARD =====
    // [18] Card background
    NewLoginTD[playerid][18] = CreatePlayerTextDraw(playerid, 235.000000, 50.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][18], 385.000000, 380.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][18], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][18], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][18], 0x12182fE6);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][18], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][18], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][18], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][18], 0);

    // [19] Card top accent bar (smanjena vidljivost)
    NewLoginTD[playerid][19] = CreatePlayerTextDraw(playerid, 235.000000, 50.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][19], 385.000000, 2.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][19], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][19], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][19], 0x0099ff60);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][19], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][19], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][19], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][19], 0);

    // [20] Card bottom accent bar (smanjena vidljivost)
    NewLoginTD[playerid][20] = CreatePlayerTextDraw(playerid, 235.000000, 426.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][20], 385.000000, 2.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][20], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][20], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][20], 0x0099ff60);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][20], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][20], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][20], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][20], 0);

    // [21] Card left border (smanjena vidljivost)
    NewLoginTD[playerid][21] = CreatePlayerTextDraw(playerid, 235.000000, 50.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][21], 1.000000, 380.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][21], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][21], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][21], 0x0099ff40);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][21], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][21], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][21], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][21], 0);

    // [22] Card right border (smanjena vidljivost)
    NewLoginTD[playerid][22] = CreatePlayerTextDraw(playerid, 618.000000, 50.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][22], 1.000000, 380.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][22], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][22], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][22], 0x0099ff40);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][22], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][22], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][22], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][22], 0);

    // [23] Corner decorations
    NewLoginTD[playerid][23] = CreatePlayerTextDraw(playerid, 235.000000, 50.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][23], 12.000000, 12.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][23], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][23], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][23], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][23], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][23], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][23], 0);

    NewLoginTD[playerid][24] = CreatePlayerTextDraw(playerid, 608.000000, 50.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][24], 12.000000, 12.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][24], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][24], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][24], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][24], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][24], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][24], 0);

    NewLoginTD[playerid][25] = CreatePlayerTextDraw(playerid, 235.000000, 418.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][25], 12.000000, 12.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][25], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][25], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][25], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][25], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][25], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][25], 0);

    NewLoginTD[playerid][26] = CreatePlayerTextDraw(playerid, 608.000000, 418.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][26], 12.000000, 12.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][26], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][26], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][26], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][26], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][26], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][26], 0);

    // [27] "DOBRODOSLI NA" header
    NewLoginTD[playerid][27] = CreatePlayerTextDraw(playerid, 260.000000, 68.000000, "DOBRODOSLI NA");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][27], 0.180000, 0.800000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][27], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][27], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][27], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][27], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][27], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][27], 1);

    // [28] "UNICATE GAMING" title
    NewLoginTD[playerid][28] = CreatePlayerTextDraw(playerid, 260.000000, 86.000000, "UNICATE GAMING");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][28], 0.420000, 1.600000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][28], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][28], -1);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][28], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][28], 0x00000033);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][28], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][28], 1);

    // [29] Header separator
    NewLoginTD[playerid][29] = CreatePlayerTextDraw(playerid, 260.000000, 110.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][29], 335.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][29], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][29], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][29], 0x0099ff30);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][29], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][29], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][29], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][29], 0);

    // [30] "PRIJAVA" heading
    NewLoginTD[playerid][30] = CreatePlayerTextDraw(playerid, 260.000000, 125.000000, "PRIJAVA");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][30], 0.500000, 2.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][30], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][30], -1);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][30], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][30], 0x00000033);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][30], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][30], 1);

    // [31] "Unesite vasu lozinku" subtitle
    NewLoginTD[playerid][31] = CreatePlayerTextDraw(playerid, 260.000000, 155.000000, "Unesite vasu lozinku da nastavite na server");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][31], 0.150000, 0.650000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][31], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][31], 0x7a8ba8FF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][31], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][31], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][31], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][31], 1);

    // [32] "IME" label
    NewLoginTD[playerid][32] = CreatePlayerTextDraw(playerid, 260.000000, 180.000000, "IME");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][32], 0.150000, 0.700000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][32], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][32], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][32], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][32], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][32], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][32], 1);

    // [33] Player name
    NewLoginTD[playerid][33] = CreatePlayerTextDraw(playerid, 300.000000, 180.000000, "Ime_Prezime");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][33], 0.230000, 0.950000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][33], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][33], -1);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][33], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][33], 0x00000033);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][33], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][33], 1);

    // [34] Name underline
    NewLoginTD[playerid][34] = CreatePlayerTextDraw(playerid, 260.000000, 197.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][34], 335.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][34], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][34], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][34], 0x1a2544FF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][34], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][34], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][34], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][34], 0);

    // [35] "LOZINKA" label
    NewLoginTD[playerid][35] = CreatePlayerTextDraw(playerid, 260.000000, 215.000000, "LOZINKA");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][35], 0.150000, 0.700000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][35], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][35], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][35], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][35], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][35], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][35], 1);

    // [36] Password field background
    NewLoginTD[playerid][36] = CreatePlayerTextDraw(playerid, 260.000000, 233.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][36], 335.000000, 26.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][36], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][36], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][36], 0x1c2340E0);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][36], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][36], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][36], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][36], 0);
    PlayerTextDrawSetSelectable(playerid, NewLoginTD[playerid][36], true);

    // [37] Password field left accent
    NewLoginTD[playerid][37] = CreatePlayerTextDraw(playerid, 260.000000, 233.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][37], 3.000000, 26.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][37], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][37], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][37], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][37], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][37], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][37], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][37], 0);

    // [38] Password placeholder / click area
    NewLoginTD[playerid][38] = CreatePlayerTextDraw(playerid, 275.000000, 237.000000, "Kliknite da unesete lozinku...");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][38], 0.180000, 0.900000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][38], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][38], 0x5a6a80FF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][38], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][38], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][38], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][38], 1);
    PlayerTextDrawSetSelectable(playerid, NewLoginTD[playerid][38], true);

    // [39] Timer text label
    NewLoginTD[playerid][39] = CreatePlayerTextDraw(playerid, 260.000000, 275.000000, "VREME: 60");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][39], 0.280000, 1.300000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][39], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][39], 0x00ff88FF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][39], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][39], 0x00000033);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][39], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][39], 1);

    // [40] Timer info
    NewLoginTD[playerid][40] = CreatePlayerTextDraw(playerid, 260.000000, 265.000000, "Imate 60 sekundi da se ulogujete.");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][40], 0.130000, 0.600000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][40], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][40], 0x5a6a80FF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][40], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][40], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][40], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][40], 1);

    // [41] Login button background
    NewLoginTD[playerid][41] = CreatePlayerTextDraw(playerid, 260.000000, 310.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][41], 335.000000, 30.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][41], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][41], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][41], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][41], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][41], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][41], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][41], 0);
    PlayerTextDrawSetSelectable(playerid, NewLoginTD[playerid][41], true);

    // [42] "ULOGUJ SE" button text
    NewLoginTD[playerid][42] = CreatePlayerTextDraw(playerid, 427.000000, 313.000000, "ULOGUJ SE");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][42], 0.350000, 1.400000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][42], 2);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][42], -1);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][42], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][42], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][42], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][42], 1);
    PlayerTextDrawSetSelectable(playerid, NewLoginTD[playerid][42], true);

    // [43] Switch link
    NewLoginTD[playerid][43] = CreatePlayerTextDraw(playerid, 427.000000, 355.000000, "Nemate nalog? Registruj se");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][43], 0.150000, 0.700000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][43], 2);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][43], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][43], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][43], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][43], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][43], 1);
    PlayerTextDrawSetSelectable(playerid, NewLoginTD[playerid][43], true);

    // [44] Footer line
    NewLoginTD[playerid][44] = CreatePlayerTextDraw(playerid, 235.000000, 440.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][44], 385.000000, 2.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][44], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][44], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][44], 0x0099ff20);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][44], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][44], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][44], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][44], 0);

    // [45] Version/branding
    NewLoginTD[playerid][45] = CreatePlayerTextDraw(playerid, 260.000000, 405.000000, "Unicate Gaming RPG ~ 2025");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][45], 0.110000, 0.550000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][45], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][45], 0x3a4a5fFF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][45], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][45], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][45], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][45], 1);

    // [46] Arrow icon for password
    NewLoginTD[playerid][46] = CreatePlayerTextDraw(playerid, 268.000000, 237.000000, "~>~");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][46], 0.200000, 0.900000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][46], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][46], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][46], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][46], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][46], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][46], 1);

    // [47]-[49] Left panel decorative lines
    NewLoginTD[playerid][47] = CreatePlayerTextDraw(playerid, 10.000000, 38.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][47], 200.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][47], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][47], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][47], 0x0099ff15);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][47], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][47], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][47], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][47], 0);

    NewLoginTD[playerid][48] = CreatePlayerTextDraw(playerid, 22.000000, 307.000000, "UG-RP");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][48], 0.100000, 0.500000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][48], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][48], 0x1a254480);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][48], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][48], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][48], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][48], 1);

    // [49] Glow line behind card
    NewLoginTD[playerid][49] = CreatePlayerTextDraw(playerid, 235.000000, 430.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][49], 385.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][49], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][49], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][49], 0x0099ff10);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][49], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][49], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][49], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][49], 0);

    // [50] Left panel decorative diamond row
    NewLoginTD[playerid][50] = CreatePlayerTextDraw(playerid, 105.000000, 420.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][50], 12.000000, 12.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][50], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][50], 0x0099ff08);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][50], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][50], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][50], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][50], 0);

    // [51] Left panel small decorative line top
    NewLoginTD[playerid][51] = CreatePlayerTextDraw(playerid, 10.000000, 425.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][51], 200.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][51], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][51], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][51], 0x0099ff10);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][51], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][51], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][51], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][51], 0);

    // [52] Card inner glow line 1
    NewLoginTD[playerid][52] = CreatePlayerTextDraw(playerid, 260.000000, 300.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][52], 335.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][52], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][52], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][52], 0x0099ff08);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][52], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][52], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][52], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][52], 0);

    // [53] Card inner glow line 2
    NewLoginTD[playerid][53] = CreatePlayerTextDraw(playerid, 260.000000, 370.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][53], 335.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][53], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][53], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][53], 0x0099ff08);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][53], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][53], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][53], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][53], 0);

    // [54] Right side decorative vertical line
    NewLoginTD[playerid][54] = CreatePlayerTextDraw(playerid, 598.000000, 55.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][54], 1.000000, 370.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][54], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][54], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][54], 0x0099ff08);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][54], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][54], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][54], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][54], 0);

    // [55] Left panel small "v3.0" watermark
    NewLoginTD[playerid][55] = CreatePlayerTextDraw(playerid, 22.000000, 445.000000, "v3.0");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][55], 0.090000, 0.400000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][55], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][55], 0x0a1525FF);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][55], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][55], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][55], 2);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][55], 1);

    // [56] Card bottom glow accent
    NewLoginTD[playerid][56] = CreatePlayerTextDraw(playerid, 235.000000, 428.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][56], 385.000000, 2.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][56], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][56], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][56], 0x0099ff15);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][56], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][56], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][56], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][56], 0);

    // [57] Left panel decorative dots pattern 1
    NewLoginTD[playerid][57] = CreatePlayerTextDraw(playerid, 185.000000, 350.000000, "~n~~n~.");
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][57], 0.500000, 0.500000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][57], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][57], 0x0099ff10);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][57], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][57], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][57], 1);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][57], 1);

    // [58] Button shadow
    NewLoginTD[playerid][58] = CreatePlayerTextDraw(playerid, 260.000000, 312.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][58], 335.000000, 2.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][58], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][58], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][58], 0x0099ff40);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][58], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][58], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][58], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][58], 0);

    // [59] Password field bottom accent
    NewLoginTD[playerid][59] = CreatePlayerTextDraw(playerid, 260.000000, 259.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewLoginTD[playerid][59], 335.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewLoginTD[playerid][59], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewLoginTD[playerid][59], 1);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][59], 0x0099ff10);
    PlayerTextDrawSetShadow(playerid, NewLoginTD[playerid][59], 0);
    PlayerTextDrawBackgroundColor(playerid, NewLoginTD[playerid][59], 0x00000000);
    PlayerTextDrawFont(playerid, NewLoginTD[playerid][59], 4);
    PlayerTextDrawSetProportional(playerid, NewLoginTD[playerid][59], 0);
}

// ============================================================================= //
//                            REGISTER TD SYSTEM                                  //
// ============================================================================= //

stock NewRegisterTDCreate(playerid)
{
    // INDEX MAP for click handlers:
    // [28]/[29] = Password field click  -> dialog_REGISTER (DIALOG_STYLE_INPUT)
    // [31]/[32] = Email field click      -> dialog_MAIL (DIALOG_STYLE_INPUT)
    // [34]/[35] = Age field click        -> dialog_AGE (DIALOG_STYLE_INPUT)
    // [37]/[38] = Country field click    -> dialog_COUNTRY (DIALOG_STYLE_LIST)
    // [40]/[41] = Gender field click     -> dialog_SPOL (DIALOG_STYLE_LIST)
    // [42]/[43] = Checkbox               -> HandleRegisterCheckbox
    // [44]/[45] = Register button        -> Register validation
    // [46]     = Switch to login          -> ShowLoginScreen

    // ===== LEFT PANEL =====
    // [0] Full screen dark background
    NewRegisterTD[playerid][0] = CreatePlayerTextDraw(playerid, 0.000000, 0.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][0], 640.000000, 480.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][0], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][0], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][0], 0x060620FF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][0], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][0], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][0], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][0], 0);

    // [1] Left panel background
    NewRegisterTD[playerid][1] = CreatePlayerTextDraw(playerid, 0.000000, 0.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][1], 220.000000, 480.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][1], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][1], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][1], 0x0c1025FF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][1], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][1], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][1], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][1], 0);

    // [2] Vertical divider (smanjena vidljivost - manje plave linije)
    NewRegisterTD[playerid][2] = CreatePlayerTextDraw(playerid, 218.000000, 0.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][2], 2.000000, 480.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][2], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][2], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][2], 0x0099ff40);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][2], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][2], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][2], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][2], 0);

    // [3] "UNICATE" logo
    NewRegisterTD[playerid][3] = CreatePlayerTextDraw(playerid, 22.000000, 35.000000, "UNICATE");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][3], 0.750000, 2.800000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][3], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][3], 0x00bbffFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][3], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][3], 0x0044ff33);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][3], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][3], 1);

    // [4] "GAMING" logo
    NewRegisterTD[playerid][4] = CreatePlayerTextDraw(playerid, 22.000000, 70.000000, "GAMING");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][4], 0.750000, 2.800000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][4], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][4], -1);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][4], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][4], 0x00000033);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][4], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][4], 1);

    // [5] "ROLEPLAY" subtitle
    NewRegisterTD[playerid][5] = CreatePlayerTextDraw(playerid, 22.000000, 105.000000, "R O L E P L A Y");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][5], 0.130000, 0.700000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][5], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][5], 0x0099ff80);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][5], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][5], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][5], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][5], 1);

    // [6] Left accent line
    NewRegisterTD[playerid][6] = CreatePlayerTextDraw(playerid, 22.000000, 130.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][6], 176.000000, 3.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][6], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][6], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][6], 0x0099ff40);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][6], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][6], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][6], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][6], 0);

    // [7] "VLASNICI" label
    NewRegisterTD[playerid][7] = CreatePlayerTextDraw(playerid, 22.000000, 143.000000, "VLASNICI");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][7], 0.130000, 0.650000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][7], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][7], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][7], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][7], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][7], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][7], 1);

    // [8] "Beka & Sloba"
    NewRegisterTD[playerid][8] = CreatePlayerTextDraw(playerid, 22.000000, 157.000000, "Beka & Sloba");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][8], 0.250000, 1.200000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][8], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][8], -1);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][8], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][8], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][8], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][8], 1);

    // [9] "DEVELOPER" label
    NewRegisterTD[playerid][9] = CreatePlayerTextDraw(playerid, 22.000000, 180.000000, "DEVELOPER");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][9], 0.130000, 0.650000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][9], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][9], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][9], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][9], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][9], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][9], 1);

    // [10] "Beka"
    NewRegisterTD[playerid][10] = CreatePlayerTextDraw(playerid, 22.000000, 194.000000, "Beka");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][10], 0.250000, 1.200000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][10], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][10], -1);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][10], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][10], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][10], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][10], 1);

    // [11] "MAPPER" label
    NewRegisterTD[playerid][11] = CreatePlayerTextDraw(playerid, 22.000000, 217.000000, "MAPPER");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][11], 0.130000, 0.650000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][11], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][11], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][11], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][11], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][11], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][11], 1);

    // [12] "Beka"
    NewRegisterTD[playerid][12] = CreatePlayerTextDraw(playerid, 22.000000, 231.000000, "Beka");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][12], 0.250000, 1.200000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][12], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][12], -1);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][12], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][12], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][12], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][12], 1);

    // [13] Bottom line left panel
    NewRegisterTD[playerid][13] = CreatePlayerTextDraw(playerid, 22.000000, 253.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][13], 176.000000, 2.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][13], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][13], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][13], 0x0099ff40);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][13], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][13], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][13], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][13], 0);

    // [14] Website
    NewRegisterTD[playerid][14] = CreatePlayerTextDraw(playerid, 22.000000, 260.000000, "www.ug-ogc.com");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][14], 0.140000, 0.750000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][14], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][14], 0x6688aaFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][14], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][14], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][14], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][14], 1);

    // [15-17] Left panel diamonds
    NewRegisterTD[playerid][15] = CreatePlayerTextDraw(playerid, 35.000000, 280.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][15], 16.000000, 16.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][15], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][15], 0x0099ff20);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][15], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][15], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][15], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][15], 0);

    NewRegisterTD[playerid][16] = CreatePlayerTextDraw(playerid, 60.000000, 280.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][16], 16.000000, 16.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][16], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][16], 0x0099ff15);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][16], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][16], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][16], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][16], 0);

    NewRegisterTD[playerid][17] = CreatePlayerTextDraw(playerid, 85.000000, 280.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][17], 16.000000, 16.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][17], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][17], 0x0099ff10);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][17], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][17], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][17], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][17], 0);

    // [18] Left panel watermark
    NewRegisterTD[playerid][18] = CreatePlayerTextDraw(playerid, 5.000000, 455.000000, "UG-RP");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][18], 0.100000, 0.500000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][18], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][18], 0x1a254860);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][18], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][18], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][18], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][18], 1);

    // ===== RIGHT CARD =====
    // [19] Card background
    NewRegisterTD[playerid][19] = CreatePlayerTextDraw(playerid, 225.000000, 5.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][19], 405.000000, 465.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][19], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][19], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][19], 0x12182fE6);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][19], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][19], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][19], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][19], 0);

    // [20] Card top accent (smanjena vidljivost)
    NewRegisterTD[playerid][20] = CreatePlayerTextDraw(playerid, 225.000000, 5.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][20], 405.000000, 2.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][20], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][20], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][20], 0x0099ff60);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][20], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][20], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][20], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][20], 0);

    // [21] Card bottom accent (smanjena vidljivost)
    NewRegisterTD[playerid][21] = CreatePlayerTextDraw(playerid, 225.000000, 466.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][21], 405.000000, 2.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][21], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][21], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][21], 0x0099ff60);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][21], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][21], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][21], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][21], 0);

    // [22] Card left border (smanjena vidljivost)
    NewRegisterTD[playerid][22] = CreatePlayerTextDraw(playerid, 225.000000, 5.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][22], 1.000000, 465.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][22], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][22], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][22], 0x0099ff40);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][22], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][22], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][22], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][22], 0);

    // [23] Card right border (smanjena vidljivost)
    NewRegisterTD[playerid][23] = CreatePlayerTextDraw(playerid, 628.000000, 5.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][23], 1.000000, 465.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][23], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][23], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][23], 0x0099ff40);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][23], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][23], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][23], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][23], 0);

    // [24-27] Corner decorations
    NewRegisterTD[playerid][24] = CreatePlayerTextDraw(playerid, 225.000000, 5.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][24], 12.000000, 12.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][24], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][24], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][24], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][24], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][24], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][24], 0);

    NewRegisterTD[playerid][25] = CreatePlayerTextDraw(playerid, 618.000000, 5.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][25], 12.000000, 12.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][25], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][25], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][25], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][25], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][25], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][25], 0);

    NewRegisterTD[playerid][26] = CreatePlayerTextDraw(playerid, 225.000000, 458.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][26], 12.000000, 12.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][26], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][26], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][26], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][26], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][26], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][26], 0);

    NewRegisterTD[playerid][27] = CreatePlayerTextDraw(playerid, 618.000000, 458.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][27], 12.000000, 12.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][27], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][27], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][27], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][27], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][27], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][27], 0);

    // [28] "DOBRODOSLI NA" header
    NewRegisterTD[playerid][28] = CreatePlayerTextDraw(playerid, 245.000000, 15.000000, "DOBRODOSLI NA");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][28], 0.180000, 0.800000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][28], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][28], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][28], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][28], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][28], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][28], 1);

    // [29] "UNICATE GAMING RPG" title
    NewRegisterTD[playerid][29] = CreatePlayerTextDraw(playerid, 245.000000, 33.000000, "UNICATE GAMING RPG");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][29], 0.400000, 1.600000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][29], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][29], -1);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][29], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][29], 0x00000033);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][29], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][29], 1);

    // REMAINDER: Indices 30 onwards = same structure but must keep click handler indices:
    // Password: [31]/[32], Email: [34]/[35], Age: [37]/[38], Country: [40]/[41]
    // Gender: [43]/[44], Checkbox: [45]/[46], Button: [47]/[48], Switch: [49]

    // [30] Header separator
    NewRegisterTD[playerid][30] = CreatePlayerTextDraw(playerid, 245.000000, 55.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][30], 365.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][30], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][30], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][30], 0x0099ff30);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][30], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][30], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][30], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][30], 0);

    // ---- PASSWORD FIELD ----
    // [31] "LOZINKA" label (was [27])
    NewRegisterTD[playerid][31] = CreatePlayerTextDraw(playerid, 245.000000, 63.000000, "LOZINKA");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][31], 0.150000, 0.700000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][31], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][31], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][31], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][31], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][31], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][31], 1);

    // [32] Password field bg
    NewRegisterTD[playerid][32] = CreatePlayerTextDraw(playerid, 245.000000, 78.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][32], 365.000000, 24.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][32], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][32], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][32], 0x1c2340E0);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][32], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][32], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][32], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][32], 0);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][32], true);

    // [33] Password left accent
    NewRegisterTD[playerid][33] = CreatePlayerTextDraw(playerid, 245.000000, 78.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][33], 3.000000, 24.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][33], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][33], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][33], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][33], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][33], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][33], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][33], 0);

    // [34] Password placeholder
    NewRegisterTD[playerid][34] = CreatePlayerTextDraw(playerid, 260.000000, 82.000000, "~>~ Kliknite za lozinku");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][34], 0.175000, 0.900000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][34], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][34], 0x5a6a80FF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][34], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][34], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][34], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][34], 1);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][34], true);

    // ---- EMAIL FIELD ----
    // [35] "EMAIL" label
    NewRegisterTD[playerid][35] = CreatePlayerTextDraw(playerid, 245.000000, 110.000000, "EMAIL");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][35], 0.150000, 0.700000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][35], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][35], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][35], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][35], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][35], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][35], 1);

    // [36] Email field bg
    NewRegisterTD[playerid][36] = CreatePlayerTextDraw(playerid, 245.000000, 125.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][36], 365.000000, 24.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][36], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][36], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][36], 0x1c2340E0);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][36], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][36], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][36], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][36], 0);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][36], true);

    // [37] Email left accent
    NewRegisterTD[playerid][37] = CreatePlayerTextDraw(playerid, 245.000000, 125.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][37], 3.000000, 24.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][37], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][37], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][37], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][37], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][37], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][37], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][37], 0);

    // [38] Email placeholder
    NewRegisterTD[playerid][38] = CreatePlayerTextDraw(playerid, 260.000000, 129.000000, "~>~ Kliknite za email");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][38], 0.175000, 0.900000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][38], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][38], 0x5a6a80FF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][38], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][38], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][38], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][38], 1);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][38], true);

    // ---- AGE FIELD ----
    // [39] "GODINE" label
    NewRegisterTD[playerid][39] = CreatePlayerTextDraw(playerid, 245.000000, 157.000000, "GODINE");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][39], 0.150000, 0.700000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][39], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][39], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][39], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][39], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][39], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][39], 1);

    // [40] Age field bg
    NewRegisterTD[playerid][40] = CreatePlayerTextDraw(playerid, 245.000000, 172.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][40], 365.000000, 24.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][40], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][40], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][40], 0x1c2340E0);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][40], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][40], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][40], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][40], 0);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][40], true);

    // [41] Age left accent
    NewRegisterTD[playerid][41] = CreatePlayerTextDraw(playerid, 245.000000, 172.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][41], 3.000000, 24.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][41], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][41], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][41], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][41], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][41], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][41], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][41], 0);

    // [42] Age placeholder
    NewRegisterTD[playerid][42] = CreatePlayerTextDraw(playerid, 260.000000, 176.000000, "~>~ Unesite godine");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][42], 0.175000, 0.900000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][42], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][42], 0x5a6a80FF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][42], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][42], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][42], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][42], 1);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][42], true);

    // ---- COUNTRY FIELD ----
    // [43] "DRZAVLJANSTVO" label
    NewRegisterTD[playerid][43] = CreatePlayerTextDraw(playerid, 245.000000, 204.000000, "DRZAVLJANSTVO");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][43], 0.150000, 0.700000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][43], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][43], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][43], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][43], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][43], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][43], 1);

    // [44] Country field bg
    NewRegisterTD[playerid][44] = CreatePlayerTextDraw(playerid, 245.000000, 219.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][44], 365.000000, 24.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][44], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][44], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][44], 0x1c2340E0);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][44], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][44], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][44], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][44], 0);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][44], true);

    // [45] Country left accent
    NewRegisterTD[playerid][45] = CreatePlayerTextDraw(playerid, 245.000000, 219.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][45], 3.000000, 24.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][45], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][45], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][45], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][45], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][45], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][45], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][45], 0);

    // [46] Country placeholder
    NewRegisterTD[playerid][46] = CreatePlayerTextDraw(playerid, 260.000000, 223.000000, "~>~ Odaberite drzavljanstvo  ~r~~~");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][46], 0.175000, 0.900000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][46], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][46], 0x5a6a80FF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][46], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][46], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][46], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][46], 1);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][46], true);

    // ---- GENDER FIELD ----
    // [47] "SPOL" label
    NewRegisterTD[playerid][47] = CreatePlayerTextDraw(playerid, 245.000000, 251.000000, "SPOL");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][47], 0.150000, 0.700000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][47], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][47], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][47], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][47], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][47], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][47], 1);

    // [48] Gender field bg
    NewRegisterTD[playerid][48] = CreatePlayerTextDraw(playerid, 245.000000, 266.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][48], 365.000000, 24.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][48], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][48], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][48], 0x1c2340E0);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][48], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][48], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][48], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][48], 0);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][48], true);

    // [49] Gender left accent
    NewRegisterTD[playerid][49] = CreatePlayerTextDraw(playerid, 245.000000, 266.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][49], 3.000000, 24.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][49], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][49], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][49], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][49], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][49], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][49], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][49], 0);

    // [50] Gender placeholder
    NewRegisterTD[playerid][50] = CreatePlayerTextDraw(playerid, 260.000000, 270.000000, "~>~ Odaberite spol  ~r~~~");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][50], 0.175000, 0.900000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][50], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][50], 0x5a6a80FF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][50], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][50], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][50], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][50], 1);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][50], true);

    // ---- CHECKBOX & BUTTON ----
    // [51] Separator line
    NewRegisterTD[playerid][51] = CreatePlayerTextDraw(playerid, 245.000000, 298.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][51], 365.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][51], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][51], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][51], 0x0099ff20);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][51], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][51], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][51], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][51], 0);

    // [52] Checkbox box
    NewRegisterTD[playerid][52] = CreatePlayerTextDraw(playerid, 245.000000, 308.000000, "]_");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][52], 0.250000, 1.100000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][52], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][52], 0x5a6a80FF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][52], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][52], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][52], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][52], 1);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][52], true);

    // [53] Checkbox text
    NewRegisterTD[playerid][53] = CreatePlayerTextDraw(playerid, 263.000000, 309.000000, "Prihvatam pravila servera i uslove koriscenja");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][53], 0.135000, 0.600000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][53], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][53], 0x5a6a80FF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][53], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][53], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][53], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][53], 1);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][53], true);

    // [54] Register button background
    NewRegisterTD[playerid][54] = CreatePlayerTextDraw(playerid, 245.000000, 335.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][54], 365.000000, 30.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][54], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][54], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][54], 0x0099ffFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][54], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][54], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][54], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][54], 0);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][54], true);

    // [55] "REGISTRUJ SE" button text
    NewRegisterTD[playerid][55] = CreatePlayerTextDraw(playerid, 427.000000, 339.000000, "REGISTRUJ SE");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][55], 0.350000, 1.400000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][55], 2);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][55], -1);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][55], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][55], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][55], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][55], 1);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][55], true);

    // [56] Switch to login link
    NewRegisterTD[playerid][56] = CreatePlayerTextDraw(playerid, 427.000000, 375.000000, "Vec imate nalog? Uloguj se");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][56], 0.150000, 0.700000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][56], 2);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][56], 0x0099ffCC);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][56], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][56], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][56], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][56], 1);
    PlayerTextDrawSetSelectable(playerid, NewRegisterTD[playerid][56], true);

    // [57] Version branding
    NewRegisterTD[playerid][57] = CreatePlayerTextDraw(playerid, 245.000000, 400.000000, "Unicate Gaming RPG ~ Premium Design 2025");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][57], 0.110000, 0.550000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][57], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][57], 0x3a4a5fFF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][57], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][57], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][57], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][57], 1);

    // [58] Footer line
    NewRegisterTD[playerid][58] = CreatePlayerTextDraw(playerid, 225.000000, 440.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][58], 405.000000, 2.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][58], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][58], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][58], 0x0099ff20);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][58], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][58], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][58], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][58], 0);

    // [59] Decorative glow line
    NewRegisterTD[playerid][59] = CreatePlayerTextDraw(playerid, 225.000000, 5.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][59], 405.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][59], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][59], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][59], 0x0099ff30);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][59], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][59], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][59], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][59], 0);

    // [60] Left panel top line decoration
    NewRegisterTD[playerid][60] = CreatePlayerTextDraw(playerid, 10.000000, 28.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][60], 200.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][60], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][60], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][60], 0x0099ff15);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][60], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][60], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][60], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][60], 0);

    // [61] "Registracija" subtitle under header
    NewRegisterTD[playerid][61] = CreatePlayerTextDraw(playerid, 245.000000, 50.000000, "Registrujte se da biste zapoceli igranje na nasem serveru");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][61], 0.135000, 0.600000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][61], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][61], 0x5a6a80FF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][61], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][61], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][61], 1);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][61], 1);

    // [62] Left panel bottom line decoration
    NewRegisterTD[playerid][62] = CreatePlayerTextDraw(playerid, 10.000000, 430.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][62], 200.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][62], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][62], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][62], 0x0099ff10);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][62], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][62], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][62], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][62], 0);

    // [63] Card inner separator line 1 (between password and email)
    NewRegisterTD[playerid][63] = CreatePlayerTextDraw(playerid, 250.000000, 105.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][63], 365.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][63], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][63], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][63], 0x0099ff08);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][63], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][63], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][63], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][63], 0);

    // [64] Card inner separator line 2 (between email and age)
    NewRegisterTD[playerid][64] = CreatePlayerTextDraw(playerid, 250.000000, 150.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][64], 365.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][64], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][64], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][64], 0x0099ff08);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][64], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][64], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][64], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][64], 0);

    // [65] Card inner separator line 3 (between age and country)
    NewRegisterTD[playerid][65] = CreatePlayerTextDraw(playerid, 250.000000, 195.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][65], 365.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][65], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][65], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][65], 0x0099ff08);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][65], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][65], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][65], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][65], 0);

    // [66] Card inner separator line 4 (between country and gender)
    NewRegisterTD[playerid][66] = CreatePlayerTextDraw(playerid, 250.000000, 240.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][66], 365.000000, 1.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][66], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][66], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][66], 0x0099ff08);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][66], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][66], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][66], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][66], 0);

    // [67] Left panel "v3.0" watermark
    NewRegisterTD[playerid][67] = CreatePlayerTextDraw(playerid, 22.000000, 445.000000, "v3.0");
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][67], 0.090000, 0.400000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][67], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][67], 0x0a1525FF);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][67], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][67], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][67], 2);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][67], 1);

    // [68] Card right side decorative line
    NewRegisterTD[playerid][68] = CreatePlayerTextDraw(playerid, 598.000000, 10.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][68], 1.000000, 450.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][68], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][68], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][68], 0x0099ff08);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][68], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][68], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][68], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][68], 0);

    // [69] Button shadow line
    NewRegisterTD[playerid][69] = CreatePlayerTextDraw(playerid, 245.000000, 365.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][69], 365.000000, 2.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][69], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][69], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][69], 0x0099ff40);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][69], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][69], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][69], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][69], 0);

    // [70] Card bottom glow
    NewRegisterTD[playerid][70] = CreatePlayerTextDraw(playerid, 225.000000, 465.000000, "LD_SPAC:white");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][70], 405.000000, 2.000000);
    PlayerTextDrawLetterSize(playerid, NewRegisterTD[playerid][70], 0.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][70], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][70], 0x0099ff15);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][70], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][70], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][70], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][70], 0);

    // [71] Left panel center diamond decoration
    NewRegisterTD[playerid][71] = CreatePlayerTextDraw(playerid, 95.000000, 310.000000, "LD_BEAT:CHIT");
    PlayerTextDrawTextSize(playerid, NewRegisterTD[playerid][71], 18.000000, 18.000000);
    PlayerTextDrawAlignment(playerid, NewRegisterTD[playerid][71], 1);
    PlayerTextDrawColor(playerid, NewRegisterTD[playerid][71], 0x0099ff08);
    PlayerTextDrawSetShadow(playerid, NewRegisterTD[playerid][71], 0);
    PlayerTextDrawBackgroundColor(playerid, NewRegisterTD[playerid][71], 0x00000000);
    PlayerTextDrawFont(playerid, NewRegisterTD[playerid][71], 4);
    PlayerTextDrawSetProportional(playerid, NewRegisterTD[playerid][71], 0);
}

stock NewLoginTDShow(playerid)
{
    for(new i = 0; i < RG_TD_MAX_LOGIN; i++)
        PlayerTextDrawShow(playerid, NewLoginTD[playerid][i]);
}

stock NewLoginTDHide(playerid)
{
    for(new i = 0; i < RG_TD_MAX_LOGIN; i++)
    {
        PlayerTextDrawHide(playerid, NewLoginTD[playerid][i]);
        PlayerTextDrawDestroy(playerid, NewLoginTD[playerid][i]);
        NewLoginTD[playerid][i] = PlayerText:INVALID_TEXT_DRAW;
    }
}

stock NewRegisterTDShow(playerid)
{
    for(new i = 0; i < RG_TD_MAX_REGISTER; i++)
        PlayerTextDrawShow(playerid, NewRegisterTD[playerid][i]);
}

stock NewRegisterTDHide(playerid)
{
    for(new i = 0; i < RG_TD_MAX_REGISTER; i++)
    {
        PlayerTextDrawHide(playerid, NewRegisterTD[playerid][i]);
        PlayerTextDrawDestroy(playerid, NewRegisterTD[playerid][i]);
        NewRegisterTD[playerid][i] = PlayerText:INVALID_TEXT_DRAW;
    }
}

stock ShowLoginScreen(playerid)
{
    RG_Screen[playerid] = RG_SCREEN_LOGIN;

    for(new i = 0; i < 52; i++)
        PlayerTextDrawHide(playerid, RegisterTD[playerid][i]);

    NewRegisterTDHide(playerid);

    NewLoginTDCreate(playerid);

    new string[25];
    format(string, sizeof string, "%s", ImeIgraca(playerid));
    PlayerTextDrawSetString(playerid, NewLoginTD[playerid][33], string);

    LoginTimerCount[playerid] = 60;
    format(string, sizeof string, "VREME: %d", LoginTimerCount[playerid]);
    PlayerTextDrawSetString(playerid, NewLoginTD[playerid][39], string);
    PlayerTextDrawColor(playerid, NewLoginTD[playerid][39], 0x00ff88FF);
    PlayerTextDrawShow(playerid, NewLoginTD[playerid][39]);

    NewLoginTDShow(playerid);
    SelectTextDraw(playerid, 0x0099ffFF);
}

stock ShowRegisterScreen(playerid)
{
    RG_Screen[playerid] = RG_SCREEN_REG;

    for(new i = 0; i < RG_TD_MAX_LOGIN; i++)
        PlayerTextDrawHide(playerid, NewLoginTD[playerid][i]);

    NewLoginTDHide(playerid);

    NewRegisterTDCreate(playerid);

    RG_RulesAccepted[playerid] = false;
    PlayerTextDrawSetString(playerid, NewRegisterTD[playerid][52], "]_");

    NewRegisterTDShow(playerid);
    SelectTextDraw(playerid, 0x0099ffFF);
}

stock NewRegisterTDControl(playerid, bool:show)
{
    if(show == true)
    {
        ShowRegisterScreen(playerid);
    }
    else if(show == false)
    {
        RG_Screen[playerid] = RG_SCREEN_NONE;

        for(new i = 0; i < 52; i++)
        {
            PlayerTextDrawHide(playerid, RegisterTD[playerid][i]);
            PlayerTextDrawDestroy(playerid, RegisterTD[playerid][i]);
            RegisterTD[playerid][i] = PlayerText:INVALID_TEXT_DRAW;
        }

        NewRegisterTDHide(playerid);
        NewLoginTDHide(playerid);

        ShowedRegister[playerid] = false;
    }
}

stock HandleRegisterCheckbox(playerid)
{
    if(RG_Screen[playerid] != RG_SCREEN_REG) return;

    RG_RulesAccepted[playerid] = !RG_RulesAccepted[playerid];

    if(RG_RulesAccepted[playerid])
        PlayerTextDrawSetString(playerid, NewRegisterTD[playerid][52], "~g~]~w~");
    else
        PlayerTextDrawSetString(playerid, NewRegisterTD[playerid][52], "]_");

    PlayerTextDrawShow(playerid, NewRegisterTD[playerid][52]);
}