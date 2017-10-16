/******************************************************************************

    JoeBOT - a bot for Counter-Strike
    Copyright (C) 2000-2002  Johannes Lampel

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

******************************************************************************/
// JoeBot
// by Johannes Lampel
// Johannes.Lampel@gmx.de
// http://joebot.counter-strike.de

//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot.h
//

#ifndef BOT_H
#define BOT_H

//#define __LOG

#include "extdll.h"
#include "bot_globaldefs.h"
#include "waypoint.h"

// stuff for Win32 vs. Linux builds

// define some function prototypes...
BOOL ClientConnect( edict_t *pEntity, const char *pszName,
                    const char *pszAddress, char szRejectReason[ 128 ] );
void ClientPutInServer( edict_t *pEntity );
void ClientCommand( edict_t *pEntity );

void FakeClientCommand(edict_t *pBot, const char *szFmt, ...);
const char *GetField (const char *string, int field_number);
extern bool g_bFakeClientCommand;
extern int fake_arg_count;
extern char *g_argv;


void InitGlobalRS(void);

// type of map
extern int g_iMapType;

// is the bomb planted ?
extern bool		g_bBombPlanted;
extern bool		g_bBombDropped;

#define LANG_EN	0
#define LANG_DE	1
#define LANG_FR	2

// times for enemy seen
extern float f_ES[32];

// last time a radio command was used
extern float f_LastRadio;

extern float g_fRoundTime;
extern float g_fRoundStart;

extern float gf_5th;
extern bool g_b5th;

// autowaypointing ... recording movements
extern AWP_EntLogItem AWP_ED[32];
extern ADV_WPRecMove ADVL[32];
extern bool bRec[32];
extern bool bStopRec[32];

//extern char szTemp[100];

typedef struct{
	double dppNet[2][_MAXNNLOG][5];
	long lNum;
	long lAct;
} bot_nn_log;

typedef struct{
	long lAction;
	float fTime;
	edict_t *pREdict;
	Vector VOrigin;
}sState;

#define DOD_FLAG_NONE	0
#define DOD_FLAG_ALLIES	1
#define DOD_FLAG_AXIS	2

typedef struct{
	int respawn_state;
	float kick_time;
	char name[BOT_NAME_LEN+1];
	char skin[BOT_SKIN_LEN+1];
	int bot_skill;
	int bot_team;
	int bot_class;
}SInfo;

extern SInfo SBInfo[32];

extern edict_t *g_pVIP;
extern float fM[2];

//void BotVATurnTo(edict_t *pEdict,const Vector &vector);
//Vector Center(edict_t *pEdict);
//bool UpdateSounds(edict_t *pEdict, edict_t *pPlayer);
void CalcDistances(void);

#endif //BOT_H
