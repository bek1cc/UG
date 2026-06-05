#include <	a_samp	>
#include <	zcmd	>

#undef MAX_PLAYERS
#define MAX_PLAYERS     50 // Promeniti na Vase slotove Vaseg servera

#define TD_COLOR        0xFFA500FF // Boja - Moze biti promenjena
#define BUY_PRICE       500 // Cena Kupovine

new PlayerText:SkinMenuTD[ MAX_PLAYERS ][ 11 ];
new Stranica[ MAX_PLAYERS ];

public OnPlayerConnect(playerid)
{
    SkinMenuTD[ playerid ][ 0 ] = CreatePlayerTextDraw(playerid, 207.117721, 139.833343, "LD_SPAC:white");
	PlayerTextDrawLetterSize(playerid, SkinMenuTD[ playerid ][ 0 ], 0.000000, 0.000000);
	PlayerTextDrawTextSize(playerid, SkinMenuTD[ playerid ][ 0 ], 222.000000, 161.000000);
	PlayerTextDrawAlignment(playerid, SkinMenuTD[ playerid ][ 0 ], 1);
	PlayerTextDrawColor(playerid, SkinMenuTD[ playerid ][ 0 ], -5963521);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 0 ], 0);
	PlayerTextDrawSetOutline(playerid, SkinMenuTD[ playerid ][ 0 ], 0);
	PlayerTextDrawBackgroundColor(playerid, SkinMenuTD[ playerid ][ 0 ], 255);
	PlayerTextDrawFont(playerid, SkinMenuTD[ playerid ][ 0 ], 4);
	PlayerTextDrawSetProportional(playerid, SkinMenuTD[ playerid ][ 0 ], 0);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 0 ], 0);

	SkinMenuTD[ playerid ][ 1 ] = CreatePlayerTextDraw(playerid, 209.941238, 142.166656, "LD_SPAC:white");
	PlayerTextDrawLetterSize(playerid, SkinMenuTD[ playerid ][ 1 ], 0.000000, 0.000000);
	PlayerTextDrawTextSize(playerid, SkinMenuTD[ playerid ][ 1 ], 217.000000, 156.000000);
	PlayerTextDrawAlignment(playerid, SkinMenuTD[ playerid ][ 1 ], 1);
	PlayerTextDrawColor(playerid, SkinMenuTD[ playerid ][ 1 ], 255);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 1 ], 0);
	PlayerTextDrawSetOutline(playerid, SkinMenuTD[ playerid ][ 1 ], 0);
	PlayerTextDrawBackgroundColor(playerid, SkinMenuTD[ playerid ][ 1 ], 255);
	PlayerTextDrawFont(playerid, SkinMenuTD[ playerid ][ 1 ], 4);
	PlayerTextDrawSetProportional(playerid, SkinMenuTD[ playerid ][ 1 ], 0);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 1 ], 0);

	SkinMenuTD[ playerid ][ 2 ] = CreatePlayerTextDraw(playerid, 269.235565, 135.750015, "ld_beat:chit");
	PlayerTextDrawLetterSize(playerid, SkinMenuTD[ playerid ][ 2 ], 0.000000, 0.000000);
	PlayerTextDrawTextSize(playerid, SkinMenuTD[ playerid ][ 2 ], 27.000000, 31.000000);
	PlayerTextDrawAlignment(playerid, SkinMenuTD[ playerid ][ 2 ], 1);
	PlayerTextDrawColor(playerid, SkinMenuTD[ playerid ][ 2 ], -5963521);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 2 ], 0);
	PlayerTextDrawSetOutline(playerid, SkinMenuTD[ playerid ][ 2 ], 0);
	PlayerTextDrawBackgroundColor(playerid, SkinMenuTD[ playerid ][ 2 ], 255);
	PlayerTextDrawFont(playerid, SkinMenuTD[ playerid ][ 2 ], 4);
	PlayerTextDrawSetProportional(playerid, SkinMenuTD[ playerid ][ 2 ], 0);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 2 ], 0);

	SkinMenuTD[ playerid ][ 3 ] = CreatePlayerTextDraw(playerid, 336.529449, 135.750000, "ld_beat:chit");
	PlayerTextDrawLetterSize(playerid, SkinMenuTD[ playerid ][ 3 ], 0.000000, 0.000000);
	PlayerTextDrawTextSize(playerid, SkinMenuTD[ playerid ][ 3 ], 27.000000, 31.000000);
	PlayerTextDrawAlignment(playerid, SkinMenuTD[ playerid ][ 3 ], 1);
	PlayerTextDrawColor(playerid, SkinMenuTD[ playerid ][ 3 ], -5963521);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 3 ], 0);
	PlayerTextDrawSetOutline(playerid, SkinMenuTD[ playerid ][ 3 ], 0);
	PlayerTextDrawBackgroundColor(playerid, SkinMenuTD[ playerid ][ 3 ], 255);
	PlayerTextDrawFont(playerid, SkinMenuTD[ playerid ][ 3 ], 4);
	PlayerTextDrawSetProportional(playerid, SkinMenuTD[ playerid ][ 3 ], 0);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 3 ], 0);

	SkinMenuTD[ playerid ][ 4 ] = CreatePlayerTextDraw(playerid, 280.529327, 142.166641, "LD_SPAC:white");
	PlayerTextDrawLetterSize(playerid, SkinMenuTD[ playerid ][ 4 ], 0.000000, 0.000000);
	PlayerTextDrawTextSize(playerid, SkinMenuTD[ playerid ][ 4 ], 71.000000, 19.000000);
	PlayerTextDrawAlignment(playerid, SkinMenuTD[ playerid ][ 4 ], 1);
	PlayerTextDrawColor(playerid, SkinMenuTD[ playerid ][ 4 ], -5963521);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 4 ], 0);
	PlayerTextDrawSetOutline(playerid, SkinMenuTD[ playerid ][ 4 ], 0);
	PlayerTextDrawBackgroundColor(playerid, SkinMenuTD[ playerid ][ 4 ], 255);
	PlayerTextDrawFont(playerid, SkinMenuTD[ playerid ][ 4 ], 4);
	PlayerTextDrawSetProportional(playerid, SkinMenuTD[ playerid ][ 4 ], 0);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 4 ], 0);

	SkinMenuTD[ playerid ][ 5 ] = CreatePlayerTextDraw(playerid, 282.470642, 142.750045, "SKIN_MENU");
	PlayerTextDrawLetterSize(playerid, SkinMenuTD[ playerid ][ 5 ], 0.330823, 1.553333);
	PlayerTextDrawAlignment(playerid, SkinMenuTD[ playerid ][ 5 ], 1);
	PlayerTextDrawColor(playerid, SkinMenuTD[ playerid ][ 5 ], 255);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 5 ], 0);
	PlayerTextDrawSetOutline(playerid, SkinMenuTD[ playerid ][ 5 ], 0);
	PlayerTextDrawBackgroundColor(playerid, SkinMenuTD[ playerid ][ 5 ], 255);
	PlayerTextDrawFont(playerid, SkinMenuTD[ playerid ][ 5 ], 2);
	PlayerTextDrawSetProportional(playerid, SkinMenuTD[ playerid ][ 5 ], 1);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 5 ], 0);

	SkinMenuTD[ playerid ][ 6 ] = CreatePlayerTextDraw(playerid, 270.647094, 173.666656, "");
	PlayerTextDrawLetterSize(playerid, SkinMenuTD[ playerid ][ 6 ], 0.000000, 0.000000);
	PlayerTextDrawTextSize(playerid, SkinMenuTD[ playerid ][ 6 ], 89.000000, 88.000000);
	PlayerTextDrawAlignment(playerid, SkinMenuTD[ playerid ][ 6 ], 1);
	PlayerTextDrawColor(playerid, SkinMenuTD[ playerid ][ 6 ], -1);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 6 ], 0);
	PlayerTextDrawSetOutline(playerid, SkinMenuTD[ playerid ][ 6 ], 0);
	PlayerTextDrawBackgroundColor(playerid, SkinMenuTD[ playerid ][ 6 ], 0x00000000);
	PlayerTextDrawFont(playerid, SkinMenuTD[ playerid ][ 6 ], 5);
	PlayerTextDrawSetProportional(playerid, SkinMenuTD[ playerid ][ 6 ], 0);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 6 ], 0);
	PlayerTextDrawSetPreviewModel(playerid, SkinMenuTD[ playerid ][ 6 ], 3);
	PlayerTextDrawSetPreviewRot(playerid, SkinMenuTD[ playerid ][ 6 ], 0.000000, 0.000000, 0.000000, 1.000000);

	SkinMenuTD[ playerid ][ 7 ] = CreatePlayerTextDraw(playerid, 260.294097, 204.583297, "ld_beat:left");
	PlayerTextDrawLetterSize(playerid, SkinMenuTD[ playerid ][ 7 ], 0.000000, 0.000000);
	PlayerTextDrawTextSize(playerid, SkinMenuTD[ playerid ][ 7 ], 18.000000, 16.000000);
	PlayerTextDrawAlignment(playerid, SkinMenuTD[ playerid ][ 7 ], 1);
	PlayerTextDrawColor(playerid, SkinMenuTD[ playerid ][ 7 ], -5963521);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 7 ], 0);
	PlayerTextDrawSetOutline(playerid, SkinMenuTD[ playerid ][ 7 ], 0);
	PlayerTextDrawBackgroundColor(playerid, SkinMenuTD[ playerid ][ 7 ], 255);
	PlayerTextDrawFont(playerid, SkinMenuTD[ playerid ][ 7 ], 4);
	PlayerTextDrawSetProportional(playerid, SkinMenuTD[ playerid ][ 7 ], 0);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 7 ], 0);
	PlayerTextDrawSetSelectable(playerid, SkinMenuTD[ playerid ][ 7 ], true);

	SkinMenuTD[ playerid ][ 8 ] = CreatePlayerTextDraw(playerid, 353.940948, 204.583282, "ld_beat:right");
	PlayerTextDrawLetterSize(playerid, SkinMenuTD[ playerid ][ 8 ], 0.000000, 0.000000);
	PlayerTextDrawTextSize(playerid, SkinMenuTD[ playerid ][ 8 ], 18.000000, 16.000000);
	PlayerTextDrawAlignment(playerid, SkinMenuTD[ playerid ][ 8 ], 1);
	PlayerTextDrawColor(playerid, SkinMenuTD[ playerid ][ 8 ], -5963521);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 8 ], 0);
	PlayerTextDrawSetOutline(playerid, SkinMenuTD[ playerid ][ 8 ], 0);
	PlayerTextDrawBackgroundColor(playerid, SkinMenuTD[ playerid ][ 8 ], 255);
	PlayerTextDrawFont(playerid, SkinMenuTD[ playerid ][ 8 ], 4);
	PlayerTextDrawSetProportional(playerid, SkinMenuTD[ playerid ][ 8 ], 0);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 8 ], 0);
	PlayerTextDrawSetSelectable(playerid, SkinMenuTD[ playerid ][ 8 ], true);

	SkinMenuTD[ playerid ][ 9 ] = CreatePlayerTextDraw(playerid, 286.176391, 278.666687, "LD_SPAC:white");
	PlayerTextDrawLetterSize(playerid, SkinMenuTD[ playerid ][ 9 ], 0.000000, 0.000000);
	PlayerTextDrawTextSize(playerid, SkinMenuTD[ playerid ][ 9 ], 59.000000, 14.000000);
	PlayerTextDrawAlignment(playerid, SkinMenuTD[ playerid ][ 9 ], 1);
	PlayerTextDrawColor(playerid, SkinMenuTD[ playerid ][ 9 ], -5963521);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 9 ], 0);
	PlayerTextDrawSetOutline(playerid, SkinMenuTD[ playerid ][ 9 ], 0);
	PlayerTextDrawBackgroundColor(playerid, SkinMenuTD[ playerid ][ 9 ], 255);
	PlayerTextDrawFont(playerid, SkinMenuTD[ playerid ][ 9 ], 4);
	PlayerTextDrawSetProportional(playerid, SkinMenuTD[ playerid ][ 9 ], 0);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 9 ], 0);
	PlayerTextDrawSetSelectable(playerid, SkinMenuTD[ playerid ][ 9 ], true);

	SkinMenuTD[ playerid ][ 10 ] = CreatePlayerTextDraw(playerid, 299.882324, 278.083160, "BUY");
	PlayerTextDrawLetterSize(playerid, SkinMenuTD[ playerid ][ 10 ], 0.384469, 1.477499);
	PlayerTextDrawAlignment(playerid, SkinMenuTD[ playerid ][ 10 ], 1);
	PlayerTextDrawColor(playerid, SkinMenuTD[ playerid ][ 10 ], 255);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 10 ], 0);
	PlayerTextDrawSetOutline(playerid, SkinMenuTD[ playerid ][ 10 ], 0);
	PlayerTextDrawBackgroundColor(playerid, SkinMenuTD[ playerid ][ 10 ], 255);
	PlayerTextDrawFont(playerid, SkinMenuTD[ playerid ][ 10 ], 2);
	PlayerTextDrawSetProportional(playerid, SkinMenuTD[ playerid ][ 10 ], 1);
	PlayerTextDrawSetShadow(playerid, SkinMenuTD[ playerid ][ 10 ], 0);
	
	return 1;
}
public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
	if(playertextid == SkinMenuTD[ playerid ][ 7 ])
	{
		if(Stranica[ playerid ] == 1)
		{
			Stranica[ playerid ] = 311;
			PlayerTextDrawHide(playerid, SkinMenuTD[ playerid ][ 6 ]);
			PlayerTextDrawSetPreviewModel(playerid, SkinMenuTD[ playerid ][ 6 ], 311);
			PlayerTextDrawShow(playerid, SkinMenuTD[ playerid ][ 6 ]);
		}
		Stranica[ playerid ] -= 1;
		PlayerTextDrawHide(playerid, SkinMenuTD[ playerid ][ 6 ]);
		PlayerTextDrawSetPreviewModel(playerid, SkinMenuTD[ playerid ][ 6 ], Stranica[ playerid ]);
		PlayerTextDrawShow(playerid, SkinMenuTD[ playerid ][ 6 ]);
	}
	else if(playertextid == SkinMenuTD[ playerid ][ 8 ])
	{
	    if(Stranica[ playerid ] == 311)
		{
			Stranica[ playerid ] = 1;
			PlayerTextDrawHide(playerid, SkinMenuTD[ playerid ][ 6 ]);
			PlayerTextDrawSetPreviewModel(playerid, SkinMenuTD[ playerid ][ 6 ], 1);
			PlayerTextDrawShow(playerid, SkinMenuTD[ playerid ][ 6 ]);
		}
		Stranica[ playerid ] += 1;
		PlayerTextDrawHide(playerid, SkinMenuTD[ playerid ][ 6 ]);
		PlayerTextDrawSetPreviewModel(playerid, SkinMenuTD[ playerid ][ 6 ], Stranica[ playerid ]);
		PlayerTextDrawShow(playerid, SkinMenuTD[ playerid ][ 6 ]);
	}
	else if(playertextid == SkinMenuTD[ playerid ][ 9 ])
	{
		for(new i; i < 11; i++) { PlayerTextDrawHide(playerid, SkinMenuTD[ playerid ][ i ]); CancelSelectTextDraw(playerid); }
	    if(GetPlayerMoney(playerid) < BUY_PRICE) { SendClientMessage(playerid, TD_COLOR, "[SMByShomy] {FFFFFF}Nemate dovoljno novca."); Stranica[ playerid ] = 0; }
		else { SetPlayerSkin(playerid, Stranica[ playerid ]); Stranica[ playerid ] = 0; }
	}
	return 1;
}

CMD:kupiskin(playerid, params[])
{
	if(Stranica[ playerid ] == 0) { SendClientMessage(playerid, TD_COLOR, "[SMByShomy] {FFFFFF}Da zatvorite menu, kucajte /kupiskin"); for(new i; i < 11; i++) { PlayerTextDrawSetPreviewModel(playerid, SkinMenuTD[ playerid ][ 6 ], 1); PlayerTextDrawShow(playerid, SkinMenuTD[ playerid ][ i ]); SelectTextDraw(playerid, TD_COLOR); Stranica[ playerid ] = 1; } }
	else { for(new i; i < 11; i++) { PlayerTextDrawHide(playerid, SkinMenuTD[ playerid ][ i ]); CancelSelectTextDraw(playerid); Stranica[ playerid ] = 0; } }
	return 1;
}
