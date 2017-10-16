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
/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
// JoeBot
// by Johannes Lampel
// Johannes.Lampel@gmx.de
// http://joebot.counter-strike.de

//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// util.cpp
//
/*

===== util.cpp ========================================================

  Utility code.  Really not optional after all.

*/

#include <ctype.h>

#include "extdll.h"
#include "util.h"
#include "engine.h"

#ifdef USE_METAMOD
#define SDK_UTIL_H  // util.h already included
#include "meta_api.h"
#endif /* USE_METAMOD */

#include "bot.h"
#include "bot_func.h"
#include "bot_modid.h"
#include "CBotBase.h"
#include "CBotCS.h"
#include "Commandfunc.h"
#include "globalvars.h"
#include "bot_globaldefs.h"

#ifdef _WIN32
extern HINSTANCE h_Library;
#else
extern void * h_Library;
#endif

int gmsgTextMsg = 0;
int gmsgSayText = 0;
int gmsgShowMenu = 0;

#define _GTOSSHEIGHT 300

#ifdef	DEBUG
edict_t *DBG_EntOfVars( const entvars_t *pev )
{
	if (pev->pContainingEntity != NULL)
		return pev->pContainingEntity;
	ALERT(at_console, "entvars_t pContainingEntity is NULL, calling into engine");
	edict_t* pent = FIND_ENTITY_BY_VARS((entvars_t*)pev);
	if (pent == NULL)
		ALERT(at_console, "DAMN!  Even the engine couldn't FindEntityByVars!");
	((entvars_t *)pev)->pContainingEntity = pent;
	return pent;
}
#endif //DEBUG


Vector UTIL_VecToAngles( const Vector &vec )
{
	float rgflVecOut[3];
	VEC_TO_ANGLES(vec, rgflVecOut);
	return Vector(rgflVecOut);
}


edict_t *UTIL_FindEntityInSphere( edict_t *pentStart, const Vector &vecCenter, float flRadius )
{
	edict_t *pentEntity = FIND_ENTITY_IN_SPHERE( pentStart, vecCenter, flRadius);
	return ( FNullEnt(pentEntity) ? NULL : pentEntity );
}


edict_t *UTIL_FindEntityByString( edict_t *pentStart, const char *szKeyword, const char *szValue )
{
	edict_t	*pentEntity = FIND_ENTITY_BY_STRING( pentStart, szKeyword, szValue );
	return ( FNullEnt(pentEntity) ? NULL : pentEntity );
}

edict_t *UTIL_FindEntityByClassname( edict_t *pentStart, const char *szName )
{
	return UTIL_FindEntityByString( pentStart, "classname", szName );
}

edict_t *UTIL_FindEntityByTargetname( edict_t *pentStart, const char *szName )
{
	return UTIL_FindEntityByString( pentStart, "targetname", szName );
}


void UTIL_MakeVectors( const Vector &vecAngles )
{
	MAKE_VECTORS( vecAngles );
}


static unsigned short FixedUnsigned16( float value, float scale )
{
	int output;

	output = value * scale;
	if ( output < 0 )
		output = 0;
	if ( output > 0xFFFF )
		output = 0xFFFF;

	return (unsigned short)output;
}

static short FixedSigned16( float value, float scale )
{
	int output;

	output = value * scale;

	if ( output > 32767 )
		output = 32767;

	if ( output < -32768 )
		output = -32768;

	return (short)output;
}


void UTIL_ClientPrintAll( int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4 )
{
	if (gmsgTextMsg == 0)
		gmsgTextMsg = REG_USER_MSG( "TextMsg", -1 );
	
	MESSAGE_BEGIN( MSG_ALL, gmsgTextMsg );
		WRITE_BYTE( msg_dest );
		WRITE_STRING( msg_name );
	
		if ( param1 )
			WRITE_STRING( param1 );
		if ( param2 )
			WRITE_STRING( param2 );
		if ( param3 )
			WRITE_STRING( param3 );
		if ( param4 )
			WRITE_STRING( param4 );
	
	MESSAGE_END();
}

void ClientPrint( entvars_t *client, int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4 )
{
	if (gmsgTextMsg == 0)
		gmsgTextMsg = REG_USER_MSG( "TextMsg", -1 );
	
	MESSAGE_BEGIN( MSG_ONE, gmsgTextMsg, NULL, client );
		WRITE_BYTE( msg_dest );
		WRITE_STRING( msg_name );
	
		if ( param1 )
			WRITE_STRING( param1 );
		if ( param2 )
			WRITE_STRING( param2 );
		if ( param3 )
			WRITE_STRING( param3 );
		if ( param4 )
			WRITE_STRING( param4 );
	
	MESSAGE_END();
}

void UTIL_SayText( const char *pText, edict_t *pEdict )
{
	if (gmsgSayText == 0)
		gmsgSayText = REG_USER_MSG( "SayText", -1 );
	
	MESSAGE_BEGIN( MSG_ONE, gmsgSayText, NULL, pEdict );
		WRITE_BYTE( ENTINDEX(pEdict) );
		WRITE_STRING( pText );
	MESSAGE_END();
}

// Overloaded to add IGNORE_GLASS
void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_LINE( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE) | (ignoreGlass?0x100:0), pentIgnore, ptr );
}


void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_LINE( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE), pentIgnore, ptr );
}

void UTIL_TraceHull( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon,int iHullNumber, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_HULL( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE),iHullNumber, pentIgnore, ptr );
}

void UTIL_TraceHull( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass,int iHullNumber, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_HULL( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE) | (ignoreGlass?0x100:0),iHullNumber, pentIgnore, ptr );
}


void UTIL_SetSize( entvars_t *pev, const Vector &vecMin, const Vector &vecMax )
{
	SET_SIZE( ENT(pev), vecMin, vecMax );
}


void UTIL_SetOrigin( entvars_t *pev, const Vector &vecOrigin )
{
	SET_ORIGIN(ENT(pev), vecOrigin );
}


char *UTIL_VarArgs( char *format, ... )
{
	va_list		argptr;
	static char		string[1024];

	va_start ( argptr, format );
	vsnprintf ( string, sizeof(string), format, argptr );
	va_end   ( argptr );

	return string;
}

int UTIL_PointContents( const Vector &vec )
{
	return POINT_CONTENTS(vec);
}

/*
==============
UTIL_PlayerDecalTrace

A player is trying to apply his custom decal for the spray can.
Tell connected clients to display it, or use the default spray can decal
if the custom can't be loaded.
==============
*/
void UTIL_PlayerDecalTrace( TraceResult *pTrace, int playernum, int decalNumber, BOOL bIsCustom )
{
	int index;

	/*if (!bIsCustom)
	{
		if ( decalNumber < 0 )
			return;

		index = gDecals[ decalNumber ].index;
		if ( index < 0 )
			return;
	}
	else*/
		index = decalNumber;

	if (pTrace->flFraction == 1.0)
		return;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_PLAYERDECAL );
		WRITE_BYTE ( playernum );
		WRITE_COORD( pTrace->vecEndPos.x );
		WRITE_COORD( pTrace->vecEndPos.y );
		WRITE_COORD( pTrace->vecEndPos.z );
		WRITE_SHORT( (short)ENTINDEX(pTrace->pHit) );
		WRITE_BYTE( index );
	MESSAGE_END();
}

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintf( const char *fmt, ... )
{
	va_list		argptr;
	static char		string[1024];

	va_start ( argptr, fmt );
	vsnprintf ( string, sizeof(string), fmt, argptr );
	va_end   ( argptr );

	// Print to server console
	ALERT( at_logged, "%s", string );
}

void UTIL_HostSay( edict_t *pEntity, int teamonly, char *message )
{
	int   j;
	char  text[128];
	char *pc;
	int   sender_team, player_team;
	edict_t *client;
	bool bIsBAlive = IsAlive(pEntity);
	//cout << message << endl;
	
	// make sure the text has content
	for ( pc = message; pc != NULL && *pc != 0; pc++ )
	{
		if ( isprint( *pc ) && !isspace( *pc ) )
		{
			pc = NULL;   // we've found an alphanumeric character,  so text is valid
			break;
		}
	}
	
	if ( pc != NULL )
		return;  // no character found, so say nothing
	
	// turn on color set 2  (color on,  no sound)
	if(IsAlive(pEntity))
		if ( teamonly )
			if(mod_id != CSTRIKE_DLL)
				snprintf( text, sizeof(text), "%c(TEAM) %s : ", 2, STRING( pEntity->v.netname ) );
			else{
				if(UTIL_GetTeam(pEntity) == CS_TEAM_CT){
					snprintf( text, sizeof(text), "%c(Counter-Terrorist) %s : ", 2, STRING( pEntity->v.netname ) );
				}
				else
					snprintf( text, sizeof(text), "%c(Terrorist) %s : ", 2, STRING( pEntity->v.netname ) );
			}
		else
			snprintf( text, sizeof(text), "%c%s : ", 2, STRING( pEntity->v.netname ) );
	else
		if ( teamonly )
			if(mod_id != CSTRIKE_DLL)
				snprintf( text, sizeof(text), "%c*DEAD* (TEAM) %s : ", 2, STRING( pEntity->v.netname ) );
			else{
				if(UTIL_GetTeam(pEntity) == CS_TEAM_CT){
					snprintf( text, sizeof(text), "%c*DEAD* (Counter-Terrorist) %s : ", 2, STRING( pEntity->v.netname ) );
				}
				else{
					snprintf( text, sizeof(text), "%c*DEAD* (Terrorist) %s : ", 2, STRING( pEntity->v.netname ) );
				}
			}
		else
			snprintf( text, sizeof(text), "%c*DEAD* %s : ", 2, STRING( pEntity->v.netname ) );
	
	j = sizeof(text) - 2 - strlen(text);  // -2 for /n and null terminator
	if ( (int)strlen(message) > j )
		message[j] = 0;
	
	strcat( text, message );
	strcat( text, "\n" );
	
	// loop through all players
	// Start with the first player.
	// This may return the world in single player if the client types something between levels or during spawn
	// so check it, or it will infinite loop
	
	/*if (gmsgSayText == 0)
		gmsgSayText = REG_USER_MSG( "SayText", -1 );*/
	
	sender_team = UTIL_GetTeam(pEntity);
	
	client = NULL;
	while ( ((client = UTIL_FindEntityByClassname( client, "player" )) != NULL) &&
		(!FNullEnt(client)) ) 
	{
		if ( client == pEntity )  // skip sender of message
			continue;

		if(!bIsBAlive == IsAlive(client))	// skip sending mess from dead bots to living ones ...
			continue;
		
		player_team = UTIL_GetTeam(client);
		
		if ( teamonly && (sender_team != player_team) )
			continue;
		
		/*MESSAGE_BEGIN( MSG_ONE, gmsgSayText, NULL, client );
			WRITE_BYTE( ENTINDEX(pEntity) );
			WRITE_STRING( text );
		MESSAGE_END();*/
		UTIL_SayText(text, client);
	}
	
	// print to the sending client
	/*MESSAGE_BEGIN( MSG_ONE, gmsgSayText, NULL, pEntity );
		WRITE_BYTE( ENTINDEX(pEntity) );
		WRITE_STRING( text );
	MESSAGE_END();*/
	UTIL_SayText(text, pEntity);
}

int UTIL_ClientIndex(edict_t *pEdict){
	for (int index = 0; index < gpGlobals->maxClients; index++){
		edict_t *pEnt = INDEXENT(index + 1);
		
		// skip invalid players and skip self (i.e. this bot)
		if ((pEnt) && (!pEnt->free) && pEnt == pEdict){
			return index;
		}
	}

	return -1;
}

// return team number 0 through 3 based what MOD uses for team numbers
int UTIL_GetTeam(edict_t *pEntity)
{
	if (mod_id == CSTRIKE_DLL)
	{
		char *infobuffer;
		char model_name[32];
		
		infobuffer = GET_INFOKEYBUFFER( pEntity );
		strcpy(model_name, INFOKEY_VALUE(infobuffer, "model"));
		
		/*if ((FStrEq(model_name, "terror")) ||  // Phoenix Connektion
			(FStrEq(model_name, "arab")) ||    // L337 Krew
			(FStrEq(model_name, "artic")) ||   // Artic Avenger
			(FStrEq(model_name, "guerilla")))  // Gorilla Warfare
		{
			return TE;
		}
		else */if ((FStrEq(model_name, "urban")) ||  // Seal Team 6
			(FStrEq(model_name, "gsg9")) ||   // German GSG-9
			(FStrEq(model_name, "sas")) ||    // UK SAS
			(FStrEq(model_name, "gign")) ||   // French GIGN
			(FStrEq(model_name, "vip")))      // VIP
		{
			return CS_TEAM_CT;
		}

		/*if ( (*(int *)model_name == ('r'<<24+'r'<<16+'e'<<8+'t')) ||
			(*(int *)model_name == ('b'<<24+'a'<<16+'r'<<8+'a')) ||
			(*(int *)model_name == ('i'<<24+'t'<<16+'r'<<8+'a')) ||
			(*(int *)model_name == ('r'<<24+'e'<<16+'u'<<8+'g')) )    
			return 0;		// it's a terror
		else
			return CS_TEAM_CT;*/

			/*

			if ( (*(int *)model_name == ('r'<<24+'r'<<16+'e'<<8+'t')) ||
			(*(int *)model_name == ('b'<<24+'a'<<16+'r'<<8+'a')) ||
			(*(int *)model_name == ('i'<<24+'t'<<16+'r'<<8+'a')) ||
			(*(int *)model_name == ('r'<<24+'e'<<16+'u'<<8+'g')) )    
			return CS_TEAM_TE;		// it's a terror
			
			*/
		
		return CS_TEAM_TE;  // return TE otherwise
	}
	else
	{
		int team = pEntity->v.team;  // assume all others are 0-3 based
		
		if ((team < 0) || (team > 3))
			team = 0;
		
		return team;
	}
}

bool UTIL_IsVIP(edict_t *pEntity){
	if (mod_id == CSTRIKE_DLL)
	{
		char *infobuffer;
		char model_name[32];
		
		infobuffer = GET_INFOKEYBUFFER( pEntity );
		strcpy(model_name, INFOKEY_VALUE(infobuffer, "model"));
		return FStrEq(model_name, "vip") ? true : false;
	}
	else
		return false;
}

int UTIL_GetBotIndex(const edict_t *pEdict)
{
	for (int index=0; index < 32; index++){
		if(bots[index]){
			if (bots[index]->pEdict == pEdict){
				return index;
			}
		}
	}
	return -1;  // return -1 if edict is not a bot
}

CBotBase *UTIL_GetBotPointer(const edict_t *pEdict)
{
	int index = UTIL_GetBotIndex(pEdict);
	
	if (index != -1)
		return (bots[index]);
	
	return NULL;  // return NULL if edict is not a bot
}

void UTIL_ShowMenu( edict_t *pEdict, int slots, int displaytime, bool needmore, char *pText )
{
   if (gmsgShowMenu == 0)
      gmsgShowMenu = REG_USER_MSG( "ShowMenu", -1 );

   MESSAGE_BEGIN( MSG_ONE, gmsgShowMenu, NULL, pEdict );
	   WRITE_SHORT( slots );
	   WRITE_CHAR( displaytime );
	   WRITE_BYTE( needmore );
	   WRITE_STRING( pText );
   MESSAGE_END();
}

void UTIL_ShowText( edict_t *pEntity, const hudtextparms_t &textparms, const char *pMessage )
{
	if (!pEntity)
		return;
	
	MESSAGE_BEGIN( MSG_ONE, SVC_TEMPENTITY, NULL, pEntity );
		WRITE_BYTE( TE_TEXTMESSAGE );
		WRITE_BYTE( textparms.channel & 0xFF );

		WRITE_SHORT( FixedSigned16( textparms.x, 1<<13 ) );
		WRITE_SHORT( FixedSigned16( textparms.y, 1<<13 ) );
		WRITE_BYTE( textparms.effect );

		WRITE_BYTE( textparms.r1 );
		WRITE_BYTE( textparms.g1 );
		WRITE_BYTE( textparms.b1 );
		WRITE_BYTE( textparms.a1 );

		WRITE_BYTE( textparms.r2 );
		WRITE_BYTE( textparms.g2 );
		WRITE_BYTE( textparms.b2 );
		WRITE_BYTE( textparms.a2 );

		WRITE_SHORT( FixedUnsigned16( textparms.fadeinTime, 1<<8 ) );
		WRITE_SHORT( FixedUnsigned16( textparms.fadeoutTime, 1<<8 ) );
		WRITE_SHORT( FixedUnsigned16( textparms.holdTime, 1<<8 ) );

		if ( textparms.effect == 2 )
			WRITE_SHORT( FixedUnsigned16( textparms.fxTime, 1<<8 ) );
		
		if ( strlen( pMessage ) < 512 )
		{
			WRITE_STRING( pMessage );
		}
		else
		{
			char tmp[512];
			strncpy( tmp, pMessage, 511 );
			tmp[511] = 0;
			WRITE_STRING( tmp );
		}
	MESSAGE_END();
}


bool IsAlive(edict_t *pEdict)
{
	return ((pEdict->v.deadflag == DEAD_NO) &&
		(pEdict->v.health > 0) && (pEdict->v.movetype != MOVETYPE_NOCLIP));
}

bool TEq(float f1,float f2,float fD){
	if(f1 < f2 + fD
		&& f1 > f2 - fD)
		return true;
	else
		return false;
}

int UTIL_PlayerCount(count_t countType)
{
	int iCount = 0;

    for ( int iIndex = 0; iIndex < gpGlobals->maxClients; iIndex++ )
	{
        edict_t *pPlayer = INDEXENT( iIndex + 1 );

        if ( pPlayer == NULL ) continue;
        if ( FNullEnt( pPlayer ) ) continue;
        if ( FStrEq( STRING( pPlayer->v.netname ), "" ) ) continue;

		switch (countType)
		{
			case COUNT_FAKE:
				if ( pPlayer->v.flags & FL_FAKECLIENT ) iCount++;
				break;
			case COUNT_HUMAN:
				if ( pPlayer->v.flags & FL_FAKECLIENT ) continue;
				iCount++;
				break;
			case COUNT_ALL:
				iCount++;
				break;
			default:
				;
		}
	}

	return iCount;
}

long UTIL_FightingAgainst(edict_t *pEdictToBeSeen,int iTeam,edict_t **pNearest,bool bDuckOnly){		// team -> what teammembers should be counted, which can view pEdictToBeSeen ??
	long lNumber = 0;
	
	edict_t *pEnt = 0;
	float fMin = 100000.0,fDistance;
	*pNearest=0;
	
	for (int i = 0; i < gpGlobals->maxClients; i++){
		edict_t *pEnt = INDEXENT(i + 1);
		
		// skip invalid players and skip self (i.e. this bot)
		if ((pEnt) && (!pEnt->free) && (pEnt != pEdictToBeSeen))
		{
			if(!IsAlive(pEnt))
				continue;

			if(bDuckOnly){
				if(!(pEnt->v.button&IN_DUCK))
					continue;
			}
			
			if(iTeam!=-1){
				if(UTIL_GetTeam(pEnt) == iTeam)
					continue;
			}

			if(FVisible(pEdictToBeSeen->v.origin,pEnt)){
				lNumber ++;
				fDistance = (pEdictToBeSeen->v.origin-pEnt->v.origin).Length();
				if(fDistance < fMin){
					fMin = fDistance;
					*pNearest = pEnt;
				}
			}
		}
	}

	return lNumber;
}

bool FInViewCone(Vector *pOrigin, edict_t *pEdict)
{
	Vector2D vec2LOS;
	float    flDot;
	
	UTIL_MakeVectors ( pEdict->v.angles );
	
	vec2LOS = ( *pOrigin - pEdict->v.origin ).Make2D();
	vec2LOS = vec2LOS.Normalize();
	
	flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );
	
	if ( flDot > 0.50 )  // 60 degree field of view 
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool FVisible( const Vector &vecOrigin, edict_t *pEdict )
{
	TraceResult tr;
	Vector      vecLookerOrigin;
	
	// look through caller's eyes
	vecLookerOrigin = pEdict->v.origin + pEdict->v.view_ofs;
	
	UTIL_TraceLine(vecLookerOrigin, vecOrigin, ignore_monsters, ignore_glass, pEdict, &tr);
	
	if (tr.flFraction != 1.0)
	{
		return FALSE;// Line of sight is not established
	}
	else
	{
		return TRUE;// line of sight is valid.
	}
}

bool FVisibleMT( const Vector &vecOrigin,const Vector &VAim,float fSQ,edict_t *pEdict2I)
{
	const int _RO = 2;
	const int _NT = 3;
	Vector v_right,v_up,v_offset;
	int ix, iy;

	fSQ /= (_NT*2.f);

	UTIL_MakeVectors(UTIL_VecToAngles(VAim - vecOrigin));

	v_right = gpGlobals->v_right;
	v_up = Vector(0,0,1);
	
	for( ix = -_NT; ix <=_NT; ix ++){
		for( iy = -_NT; iy <= _NT; iy ++){
			TraceResult tr;

			v_offset = ix * v_right * fSQ + iy * v_up * fSQ ;
			
			UTIL_TraceLine(vecOrigin+v_offset+Vector(RANDOM_LONG(-_RO,_RO),RANDOM_LONG(-_RO,_RO),RANDOM_LONG(-_RO,_RO)), VAim+v_offset+Vector(RANDOM_LONG(-_RO,_RO),RANDOM_LONG(-_RO,_RO),RANDOM_LONG(-_RO,_RO)), ignore_monsters, ignore_glass, pEdict2I, &tr);
			DEBUG_DRAWBEAM(listenserver_edict,vecOrigin+v_offset, VAim+v_offset,3,0,250,250,250,250,10);
			
			if (tr.flFraction != 1.0){
				// couldn't be established
				return false;
			}
			else{
				//continue loop
			}
		}
	}
	return true;
}

bool FVisibleEx(Vector v_start, Vector v_end, bool bignore_doors, bool bignore_breakables, bool bignore_monsters, bool bignore_glass,edict_t *pEdictIgnore){
	// traces a line to see if something is visible, but this function ignores doors
	TraceResult tr1,tr2;
	char	szDoor[] = "func_door",
			szBreakable[] = "func_breakable";
	
	UTIL_TraceLine(v_start,v_end,bignore_monsters?ignore_monsters:dont_ignore_monsters,bignore_glass?ignore_glass:dont_ignore_glass,pEdictIgnore,&tr1);
	//UTIL_TraceHull(v_start,v_end,bignore_monsters?ignore_monsters:dont_ignore_monsters,bignore_glass?ignore_glass:dont_ignore_glass,iHull,pEdictIgnore,&tr1);

	if(tr1.flFraction == 1.f)
		return true;

	Vector v_dir = (v_end - v_start).Normalize();
	
	if(tr1.pHit){
		if(bignore_doors
			&&!strncmp(STRING(tr1.pHit->v.classname),szDoor,9)){
			// it's a door
			// trace vice versa
			UTIL_TraceLine(v_end,v_start,bignore_monsters?ignore_monsters:dont_ignore_monsters,bignore_glass?ignore_glass:dont_ignore_glass,pEdictIgnore,&tr2);
			
			if(tr1.pHit == tr2.pHit)		// the same door
				return true;

			return FVisibleEx(tr1.vecEndPos + v_dir*.001,tr2.vecEndPos - v_dir * .001,bignore_doors,bignore_breakables,bignore_monsters,bignore_glass,pEdictIgnore);
		}
		else if(bignore_breakables
			&&!strncmp(STRING(tr1.pHit->v.classname),szBreakable,14)){
			// it's a breakable
			if(tr1.pHit->v.takedamage == DAMAGE_YES){		// can be destroyed
				UTIL_TraceLine(v_end,v_start,bignore_monsters?ignore_monsters:dont_ignore_monsters,bignore_glass?ignore_glass:dont_ignore_glass,pEdictIgnore,&tr2);
				//UTIL_TraceHull(v_end,v_start,bignore_monsters?ignore_monsters:dont_ignore_monsters,bignore_glass?ignore_glass:dont_ignore_glass,iHull,pEdictIgnore,&tr2);
				if(tr1.pHit == tr2.pHit)		// the same breakable
					return true;

				return FVisibleEx(tr1.vecEndPos + v_dir*.001,tr2.vecEndPos - v_dir * .001,bignore_doors,bignore_breakables,bignore_monsters,bignore_glass,pEdictIgnore);
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

Vector GetGunPosition(edict_t *pEdict)
{
	return (pEdict->v.origin + pEdict->v.view_ofs);
}

void UTIL_SelectItem(edict_t *pEdict, char *item_name)
{
	FakeClientCommand(pEdict, "%s", item_name);
}

Vector VecBModelOrigin(edict_t *pEdict)
{
	return pEdict->v.absmin + (pEdict->v.size * 0.5);
}

Vector VecBModelOrigin(entvars_s *v)
{
	return v->absmin + (v->size * 0.5);
}

void UTIL_BuildFileName(char *str, size_t size, const char *format, ...)
{
	va_list		argptr;

	va_start ( argptr, format );
	vsnprintf ( str, size, format, argptr );
	va_end   ( argptr );

	snprintf(str, size, UTIL_VarArgs("%s/%s", mod_name, str));
}

edict_t *GetNearestPlayer(edict_t *pEdict,int iTeam,float &fMin,bool bVisible,bool bIVC,float fMax){
	edict_t *pNearestEdict=0;
	float fDistance;

	fMin = 100000000;
	
	for (int i = 0; i < gpGlobals->maxClients; i++){
		edict_t *pEnt = INDEXENT(i + 1);
		
		// skip invalid players and skip self (i.e. this bot)
		if ((pEnt) && (!pEnt->free) && (pEnt != pEdict))
		{
			if(!IsAlive(pEnt))
				continue;
			if(iTeam != -1){
				if(UTIL_GetTeam(pEnt) != iTeam)
					continue;
			}
			if(bVisible){
				if(!FVisible(pEnt->v.origin,pEdict))
					continue;
			}
			if(bIVC){
				if(!FInViewCone(&pEnt->v.origin,pEdict))
					continue;
			}

			fDistance = (pEdict->v.origin - pEnt->v.origin).Length();
			if(fMin > fDistance
				&& fDistance > 1.0f
				&& fDistance < fMax){
				fMin = fDistance;
				pNearestEdict = pEnt;
			}
		}
	}
	return pNearestEdict;
}

edict_t *UTIL_BestPlayer(void){
	edict_t *pBestEdict=0;
	float iFrag = -1;
	
	for (int i = 0; i < gpGlobals->maxClients; i++){
		edict_t *pEnt = INDEXENT(i + 1);
		
		// skip invalid players and skip self (i.e. this bot)
		if ((pEnt) && (!pEnt->free))
		{
			if(iFrag < pEnt->v.frags){
				iFrag = pEnt->v.frags;
				pBestEdict = pEnt;
			}
		}
	}
	return pBestEdict;
}

edict_t *GetNearestPlayer(Vector VOrigin,int iTeam,float &fMin,float fMax,edict_t *pNot){
	edict_t *pNearestEdict=0;
	float fDistance;
	
	fMin = 100000000;
	
	for (int i = 0; i < gpGlobals->maxClients; i++){
		edict_t *pEnt = INDEXENT(i + 1);
		
		// skip invalid players and skip self (i.e. this bot)
		if ((pEnt) && (!pEnt->free) && (pEnt != pNot))
		{
			if(!IsAlive(pEnt))
				continue;
			if(iTeam != -1){
				if(UTIL_GetTeam(pEnt) != iTeam)
					continue;
			}

			fDistance = (VOrigin - pEnt->v.origin).Length();
			if(fMin > fDistance
				&& fDistance > 1.0f
				&& fDistance < fMax){
				fMin = fDistance;
				pNearestEdict = pEnt;
			}
		}
	}
	return pNearestEdict;
}

edict_t *GetRandomPlayer(edict_t *pNot,int iTeam,int iAlive){
	edict_t *pFE[32],*pEnt;
	long lCount;
	long lreturn;

	lCount = 0;
	for (int i = 0; i < gpGlobals->maxClients; i++){
		pEnt = INDEXENT(i + 1);
		if ((pEnt) && (!pEnt->free) && (pEnt != pNot)){
			if(iTeam!=-1){
				if(iTeam != UTIL_GetTeam(pEnt))
					continue;
			}
			if(iAlive!=-1){
				if((!IsAlive(pEnt)) && (iAlive == 1))
					continue;
				else if((IsAlive(pEnt)) && (iAlive == 0))
					continue;
			}
			pFE[lCount] = pEnt;
			lCount++;
		}
	}
	
	if(lCount){
		lreturn=RANDOM_LONG(0,lCount-1);
		return pFE[lreturn];
	}
	else
		return 0;
}

edict_t *GetRandomPlayerWO(edict_t *pNot,int iTeam,int iAlive, long lWithout,edict_t *pEdict){
	edict_t *pFE[32],*pEnt;
	long lCount;
	long lreturn;
	
	lCount = 0;
	for (int i = 0; i < gpGlobals->maxClients; i++){
		pEnt = INDEXENT(i + 1);
		if ((pEnt) && (!pEnt->free) && (pEnt != pNot)){
			if(iTeam!=-1){
				if(iTeam != UTIL_GetTeam(pEnt))
					continue;
			}
			if(iAlive!=-1){
				if((!IsAlive(pEnt)) && (iAlive == 1))
					continue;
				else if((IsAlive(pEnt)) && (iAlive == 0))
					continue;
			}
			CBotBase *pB = UTIL_GetBotPointer(pEnt);
			if(pB){
				if(pB->Task.SearchP( pEdict) != -1
					|| pB->Task.SearchT( lWithout ) != -1){
					continue;
				}
			}
			pFE[lCount] = pEnt;
			lCount++;
		}
	}
	
	if(lCount){
		lreturn=RANDOM_LONG(0,lCount-1);
		return pFE[lreturn];
	}
	else
		return 0;
}

//
// VecCheckToss - returns the velocity at which an object should be lobbed from vecspot1 to land near vecspot2.
// returns g_vecZero if toss is not feasible.
// 
Vector VecCheckToss ( entvars_t *pev, const Vector &vecSpot1, Vector vecSpot2, float flGravityAdj )
{
	TraceResult		tr;
	Vector			vecMidPoint;// halfway point between Spot1 and Spot2
	Vector			vecApex;// highest point 
	Vector			vecScale;
	Vector			vecGrenadeVel;
	Vector			vecTemp;
	float			flGravity = CVAR_GET_FLOAT("sv_gravity") * flGravityAdj;
	
	if (vecSpot2.z - vecSpot1.z > _GTOSSHEIGHT)
	{
		// to high, fail
		return g_vecZero;
	}
	
	UTIL_MakeVectors (pev->angles);
	
	// toss a little bit to the left or right, not right down on the enemy's bean (head). 
	vecSpot2 = vecSpot2 + gpGlobals->v_right * ( RANDOM_FLOAT(-8,8) + RANDOM_FLOAT(-16,16) );
	vecSpot2 = vecSpot2 + gpGlobals->v_forward * ( RANDOM_FLOAT(-8,8) + RANDOM_FLOAT(-16,16) );
	
	// calculate the midpoint and apex of the 'triangle'
	// UNDONE: normalize any Z position differences between spot1 and spot2 so that triangle is always RIGHT
	
	// How much time does it take to get there?
	
	// get a rough idea of how high it can be thrown
	vecMidPoint = vecSpot1 + (vecSpot2 - vecSpot1) * 0.5;
	UTIL_TraceLine(vecMidPoint, vecMidPoint + Vector(0,0,_GTOSSHEIGHT), ignore_monsters, ENT(pev), &tr);
	DEBUG_DRAWBEAM(listenserver_edict,vecMidPoint,vecMidPoint + Vector(0,0,_GTOSSHEIGHT),10,10,0,200,200,200,10);
	vecMidPoint = tr.vecEndPos;
	// (subtract 15 so the grenade doesn't hit the ceiling)
	vecMidPoint.z -= 15;
	
	if (vecMidPoint.z < vecSpot1.z || vecMidPoint.z < vecSpot2.z)
	{
		// to not enough space, fail
		return g_vecZero;
	}
	
	// How high should the grenade travel to reach the apex
	float distance1 = (vecMidPoint.z - vecSpot1.z);
	float distance2 = (vecMidPoint.z - vecSpot2.z);
	
	// How long will it take for the grenade to travel this distance
	float time1 = sqrt( distance1 / (0.5 * flGravity) );
	float time2 = sqrt( distance2 / (0.5 * flGravity) );
	
	if (time1 < 0.1)
	{
		// too close
		return g_vecZero;
	}
	
	// how hard to throw sideways to get there in time.
	vecGrenadeVel = (vecSpot2 - vecSpot1) / (time1 + time2);
	// how hard upwards to reach the apex at the right time.
	vecGrenadeVel.z = flGravity * time1 * .75f;
	
	// find the apex
	vecApex  = vecSpot1 + vecGrenadeVel * time1;
	vecApex.z = vecMidPoint.z;
	
	UTIL_TraceLine(vecSpot1, vecApex, dont_ignore_monsters, ENT(pev), &tr);
	DEBUG_DRAWBEAM(listenserver_edict,vecSpot1, vecApex,10,10,0,200,200,200,10);
	if (tr.flFraction != 1.0)
	{
		// fail!
		return g_vecZero;
	}
	
	// UNDONE: either ignore monsters or change it to not care if we hit our enemy
	UTIL_TraceLine(vecSpot2, vecApex, ignore_monsters, ENT(pev), &tr); 
	DEBUG_DRAWBEAM(listenserver_edict,vecSpot2, vecApex,10,10,0,200,200,200,10);
	if (tr.flFraction != 1.0)
	{
		// fail!
		return g_vecZero;
	}
	
	return vecGrenadeVel;
}


//
// VecCheckThrow - returns the velocity vector at which an object should be thrown from vecspot1 to hit vecspot2.
// returns g_vecZero if throw is not feasible.
//
Vector VecCheckThrow ( entvars_t *pev, const Vector &vecSpot1, Vector vecSpot2, float flSpeed, float flGravityAdj )
{
	float flGravity = CVAR_GET_FLOAT("sv_gravity") * flGravityAdj;
	
	Vector vecGrenadeVel = (vecSpot2 - vecSpot1);
	
	// throw at a constant time
	float time = vecGrenadeVel.Length( ) / flSpeed;
	vecGrenadeVel = vecGrenadeVel * (1.0 / time);
	
	// adjust upward toss to compensate for gravity loss
	vecGrenadeVel.z += flGravity * time * 0.5;
	
	Vector vecApex = vecSpot1 + (vecSpot2 - vecSpot1) * 0.5;
	vecApex.z += 0.5 * flGravity * (time * 0.5) * (time * 0.5);
	
	TraceResult tr;
	UTIL_TraceLine(vecSpot1, vecApex, dont_ignore_monsters, ENT(pev), &tr);
	DEBUG_DRAWBEAM(listenserver_edict,vecSpot1, vecApex,10,10,100,200,200,200,10);

	if (tr.flFraction != 1.0)
	{
		// fail!
		return g_vecZero;
	}
	
	UTIL_TraceLine(vecSpot2, vecApex, ignore_monsters, ENT(pev), &tr);
	DEBUG_DRAWBEAM(listenserver_edict,vecSpot2, vecApex,10,10,100,200,200,200,10);
	if (tr.flFraction != 1.0)
	{
		// fail!
		return g_vecZero;
	}
	
	return vecGrenadeVel;
}

float GetHosVel(edict_t *pEdict){
	long lNumH = 0;
	while(lNumH<_MAXHOSTAGEREC){
		if(gHossi[lNumH].p == pEdict)
			return gHossi[lNumH].fVelocity;
		lNumH++;
	}
	return 0;
}

void UTIL_strlwr(char *p){
	while(*p){
		*p=tolower(*p);
		p ++;
	}
}

void UTIL_LogMessage( const char* plid, const char *fmt, ... )
{
	// Adapted from Will Day's metamod
	va_list ap;
	char buf[1024];

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	if (plid)
		ALERT( at_logged, "[%s] %s\n", buf );
	else
		ALERT( at_logged, "[JOEBOT] %s\n", buf );
}

void UTIL_ConsoleMessage( edict_t *pEdict, const char* fmt, ... )
{
	va_list		argptr;
	static char		string[1024];

	va_start ( argptr, fmt );
	vsnprintf ( string, sizeof(string), fmt, argptr );
	va_end   ( argptr );

	if (pEdict)
		CLIENT_PRINTF( pEdict, print_console, string );
	else if (listenserver_edict)
		CLIENT_PRINTF( listenserver_edict, print_console, string );
	else if (IS_DEDICATED_SERVER())
		SERVER_PRINT( string );
}

qboolean UTIL_CallGameEntity( const char *entStr, entvars_t *pev)
{
	LINK_ENTITY_FUNC pfnEntity = NULL;

	pfnEntity = (LINK_ENTITY_FUNC)GetProcAddress(h_Library, entStr);
	if(!pfnEntity)
		return false;

	(*pfnEntity)(pev);
	return true;
}

#ifdef _DEBUG
void UTIL_LogFile(const char* szFilename, const char *szLogMsg)
{
	static FILE *fp;

	if (FStrEq(szFilename, "engine.log") && !CVAR_BOOL(jb_debugengine))
		return;

	if (fp = fopen(szFilename, "a"))
	{
		fprintf(fp, "%s\n", szLogMsg);
		fclose(fp);
	}
}
#endif
