
#define MAX_SERVERMESSAGE 10
#define INVALID_SERVERMESSAGE_ID -1
#define SERVERMESSAGE_HEADER_COLOR 0xFFAF00AA
#define SERVERMESSAGE_TEXT_COLOR 0x00000088
#define SERVERMESSAGE_SOUND 40405

#if MAX_SERVERMESSAGE > 11
	#error You can not use more than 11 ServerMessage at once !
	#endif

forward ServerMessageCheck();

new Text:sServerMessage[MAX_SERVERMESSAGE][2];
new ServerMessageTimer;

new pServerMessageHe[MAX_PLAYERS][MAX_SERVERMESSAGE][24];
new pServerMessageTxt[MAX_PLAYERS][MAX_SERVERMESSAGE][64];
new pServerMessageDur[MAX_PLAYERS][MAX_SERVERMESSAGE];
new PlayerText:pServerMessageID[MAX_PLAYERS][MAX_SERVERMESSAGE][2];

//------------------------------------------------------------------------------

public ServerMessageCheck() {

	for(new i=0;i<MAX_PLAYERS;i++) {
	    if(IsPlayerConnected(i) && IsActiveServerMessage(i)) {

			for(new p=0;p<MAX_SERVERMESSAGE;p++) {

				if(pServerMessageDur[i][p] == 0) { break; }

			    if(pServerMessageDur[i][p] > 1) {
					pServerMessageDur[i][p]--;

				}else if(pServerMessageDur[i][p] == 1) {

				    pServerMessageDur[i][p] = 0;

					new smessageid = p;
					while(smessageid < MAX_SERVERMESSAGE-1) {
						format(pServerMessageHe[i][smessageid], 24, "%s", pServerMessageHe[i][smessageid+1]);
					    format(pServerMessageTxt[i][smessageid], 64, "%s", pServerMessageTxt[i][smessageid+1]);
	                    PlayerTextDrawSetString(i, pServerMessageID[i][smessageid][0], pServerMessageHe[i][smessageid]);
						PlayerTextDrawSetString(i, pServerMessageID[i][smessageid][1], pServerMessageTxt[i][smessageid]);
						pServerMessageDur[i][smessageid] = pServerMessageDur[i][smessageid+1];
						smessageid++;
                        } } }

			for(new p=0;p<MAX_SERVERMESSAGE;p++) {
			    if(pServerMessageDur[i][p] == 0) {
					PlayerTextDrawHide(i, pServerMessageID[i][p][0]);
                    PlayerTextDrawHide(i, pServerMessageID[i][p][1]);
                    TextDrawHideForPlayer(i, sServerMessage[p][0]);
                    TextDrawHideForPlayer(i, sServerMessage[p][1]); } } } }

	return 1; }

//------------------------------------------------------------------------------

stock SendServerMessage(playerid, header[], text[], duration) {

	if(GetFreeSMessageSlot(playerid) == INVALID_SERVERMESSAGE_ID) { return 1; }

	new smessageid = GetFreeSMessageSlot(playerid);
	pServerMessageDur[playerid][smessageid] = duration+1;

	PlayerPlaySound(playerid, SERVERMESSAGE_SOUND, 0, 0, 0);
	format(pServerMessageHe[playerid][smessageid], 24, "%s", header);
	format(pServerMessageTxt[playerid][smessageid], 64, "%s", text);
	PlayerTextDrawSetString(playerid, pServerMessageID[playerid][smessageid][0], pServerMessageHe[playerid][smessageid]);
	PlayerTextDrawSetString(playerid, pServerMessageID[playerid][smessageid][1], pServerMessageTxt[playerid][smessageid]);

	for(new i=0;i<2;i++) { TextDrawShowForPlayer(playerid, sServerMessage[smessageid][i]); }
	for(new i=0;i<2;i++) { PlayerTextDrawShow(playerid, pServerMessageID[playerid][smessageid][i]); }

	return 1; }

//------------------------------------------------------------------------------

stock LoadPlayerSMessage(playerid) {

    for(new i=0;i<MAX_SERVERMESSAGE;i++) {

        pServerMessageDur[playerid][i] = 0;

	    new minus_num = i*32;
		pServerMessageID[playerid][i][0] = CreatePlayerTextDraw(playerid,586,416-minus_num,"~w~Naslov");
		PlayerTextDrawLetterSize(playerid,pServerMessageID[playerid][i][0],0.349999,0.799999);
		PlayerTextDrawAlignment(playerid,pServerMessageID[playerid][i][0],2);
		PlayerTextDrawBackgroundColor(playerid,pServerMessageID[playerid][i][0], 0x000000FF);
		PlayerTextDrawFont(playerid,pServerMessageID[playerid][i][0],0);
		PlayerTextDrawUseBox(playerid,pServerMessageID[playerid][i][0],0);
		PlayerTextDrawSetOutline(playerid,pServerMessageID[playerid][i][0],1);
		PlayerTextDrawSetProportional(playerid,pServerMessageID[playerid][i][0],1);
		PlayerTextDrawSetShadow(playerid,pServerMessageID[playerid][i][0],0);

		pServerMessageID[playerid][i][1] = CreatePlayerTextDraw(playerid,588,425-minus_num,"~w~Text~n~Text~n~Text");
		PlayerTextDrawLetterSize(playerid,pServerMessageID[playerid][i][1],0.199999,0.699999);
		PlayerTextDrawAlignment(playerid,pServerMessageID[playerid][i][1],2);
		PlayerTextDrawBackgroundColor(playerid,pServerMessageID[playerid][i][1], 0x000000FF);
		PlayerTextDrawFont(playerid,pServerMessageID[playerid][i][1],2);
		PlayerTextDrawUseBox(playerid,pServerMessageID[playerid][i][1],0);
		PlayerTextDrawSetOutline(playerid,pServerMessageID[playerid][i][1],1);
		PlayerTextDrawSetProportional(playerid,pServerMessageID[playerid][i][1],1);
		PlayerTextDrawSetShadow(playerid,pServerMessageID[playerid][i][1],0);
		}

	return 1; }

//------------------------------------------------------------------------------

stock LoadServerSMessage() {

    ServerMessageTimer = SetTimer("ServerMessageCheck", 1000, 1);

    for(new i=0;i<MAX_SERVERMESSAGE;i++) {

		new minus_num = i*32;
		sServerMessage[i][0] = TextDrawCreate(535,426-minus_num,"_");
		TextDrawLetterSize(sServerMessage[i][0],0.500000,2.100000);
		TextDrawAlignment(sServerMessage[i][0],0);
		TextDrawBackgroundColor(sServerMessage[i][0],0x000000FF);
		TextDrawFont(sServerMessage[i][0],3);
		TextDrawUseBox(sServerMessage[i][0],1);
		TextDrawBoxColor(sServerMessage[i][0], SERVERMESSAGE_TEXT_COLOR);
		TextDrawTextSize(sServerMessage[i][0],635.000000,436.000000);
		TextDrawSetOutline(sServerMessage[i][0],0);
		TextDrawSetProportional(sServerMessage[i][0],1);
		TextDrawSetShadow(sServerMessage[i][0],1);

		sServerMessage[i][1] = TextDrawCreate(535,418-minus_num,"_");
		TextDrawLetterSize(sServerMessage[i][1],0.500000,0.49999);
		TextDrawAlignment(sServerMessage[i][1],0);
		TextDrawBackgroundColor(sServerMessage[i][1],0x000000FF);
		TextDrawFont(sServerMessage[i][1],3);
		TextDrawUseBox(sServerMessage[i][1],1);
		TextDrawBoxColor(sServerMessage[i][1], SERVERMESSAGE_HEADER_COLOR);
		TextDrawTextSize(sServerMessage[i][1],635.000000,424.000000);
		TextDrawSetOutline(sServerMessage[i][1],0);
		TextDrawSetProportional(sServerMessage[i][1],1);
		TextDrawSetShadow(sServerMessage[i][1],1);

		}

	for(new i=0;i<MAX_PLAYERS;i++) {
	    if(IsPlayerConnected(i)) { LoadPlayerSMessage(i); } }

	return 1; }

//------------------------------------------------------------------------------

stock ExitServerSMessage() {
	KillTimer(ServerMessageTimer);

	for(new i=0;i<MAX_SERVERMESSAGE;i++) {
		TextDrawDestroy(sServerMessage[i][0]);
		TextDrawDestroy(sServerMessage[i][1]);}

	for(new i=0;i<MAX_PLAYERS;i++) {
	    if(IsPlayerConnected(i)) { ExitPlayerSMessage(i); } }

	return 1; }

//------------------------------------------------------------------------------

stock ExitPlayerSMessage(playerid) {

	for(new i=0;i<MAX_SERVERMESSAGE;i++) {
		PlayerTextDrawDestroy(playerid, pServerMessageID[playerid][i][0]);
		PlayerTextDrawDestroy(playerid, pServerMessageID[playerid][i][1]); }

	return 1; }

//------------------------------------------------------------------------------

stock GetFreeSMessageSlot(playerid) {

	for(new i=0;i<MAX_SERVERMESSAGE;i++) {
		if(pServerMessageDur[playerid][i] == 0) { return i; } }

	return INVALID_SERVERMESSAGE_ID; }

//------------------------------------------------------------------------------

stock IsActiveServerMessage(playerid) {

	if(pServerMessageDur[playerid][0] != 0) { return true; }

	return false; }

//------------------------------------------------------------------------------

