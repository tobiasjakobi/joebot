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
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// engine.cpp
//

#include "extdll.h"
#include "util.h"
#include "engine.h"

#ifdef USE_METAMOD
#define SDK_UTIL_H  // util.h already included
#include "meta_api.h"
#endif /* USE_METAMOD */

#include "bot.h"
#include "bot_client.h"
#include "bot_modid.h"
#include "CBotCS.h"
#include "Commandfunc.h"
#include "globalvars.h"

extern enginefuncs_t g_engfuncs;

void (*botMsgFunction)(void *, int) = NULL;
int botMsgIndex;

#ifndef USE_METAMOD
int pfnPrecacheModel(char* s)
{
	LOG_ENGINE(UTIL_VarArgs("pfnPrecacheModel: s=%s", s));
	return (*g_engfuncs.pfnPrecacheModel)(s);
}
int pfnPrecacheSound(char* s)
{
	LOG_ENGINE(UTIL_VarArgs("pfnPrecacheSound: s=%s", s));
	return (*g_engfuncs.pfnPrecacheSound)(s);
}
void pfnSetModel(edict_t *e, const char *m)
{
	LOG_ENGINE(UTIL_VarArgs("pfnSetModel: e=%x, m=%s", e, m));
	(*g_engfuncs.pfnSetModel)(e, m);
}
int pfnModelIndex(const char *m)
{
	//LOG_ENGINE(UTIL_VarArgs("pfnModelIndex: m=%s", m));
	return (*g_engfuncs.pfnModelIndex)(m);
}
int pfnModelFrames(int modelIndex)
{
	LOG_ENGINE("pfnModelFrames:");
	return (*g_engfuncs.pfnModelFrames)(modelIndex);
}
void pfnSetSize(edict_t *e, const float *rgflMin, const float *rgflMax)
{
	LOG_ENGINE(UTIL_VarArgs("pfnSetSize: e=%x", e));
	(*g_engfuncs.pfnSetSize)(e, rgflMin, rgflMax);
}
#endif /* not USE_METAMOD */
void pfnChangeLevel(char* s1, char* s2)
{
	LOG_ENGINE("pfnChangeLevel:");
	
	// kick any bot off of the server after time/frag limit...
	for (int index = 0; index < 32; index++)
	{
		if (bots[index])  // is this slot used?
		{
			SBInfo[index].respawn_state = RESPAWN_NEED_TO_RESPAWN;
			SERVER_COMMAND(UTIL_VarArgs("kick \"%s\"\n", bots[index]->name));  // kick the bot using (kick "name")
		}
	}
	gf_5th = 0;
	
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnChangeLevel)(s1, s2);
#endif /* USE_METAMOD */
}
#ifndef USE_METAMOD
void pfnGetSpawnParms(edict_t *ent)
{
	LOG_ENGINE("pfnGetSpawnParms:");
	(*g_engfuncs.pfnGetSpawnParms)(ent);
}
void pfnSaveSpawnParms(edict_t *ent)
{
	LOG_ENGINE("pfnSaveSpawnParms:");
	(*g_engfuncs.pfnSaveSpawnParms)(ent);
}
float pfnVecToYaw(const float *rgflVector)
{
	//LOG_ENGINE("pfnVecToYaw:");
	return (*g_engfuncs.pfnVecToYaw)(rgflVector);
}
void pfnVecToAngles(const float *rgflVectorIn, float *rgflVectorOut)
{
	//LOG_ENGINE("pfnVecToAngles:");
	(*g_engfuncs.pfnVecToAngles)(rgflVectorIn, rgflVectorOut);
}
void pfnMoveToOrigin(edict_t *ent, const float *pflGoal, float dist, int iMoveType)
{
	LOG_ENGINE("pfnMoveToOrigin:");
	(*g_engfuncs.pfnMoveToOrigin)(ent, pflGoal, dist, iMoveType);
}
void pfnChangeYaw(edict_t* ent)
{
	//LOG_ENGINE("pfnChangeYaw:");
	(*g_engfuncs.pfnChangeYaw)(ent);
}
void pfnChangePitch(edict_t* ent)
{
	//LOG_ENGINE("pfnChangePitch:");
	(*g_engfuncs.pfnChangePitch)(ent);
}
edict_t* pfnFindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue)
{
	//LOG_ENGINE(UTIL_VarArgs("pfnFindEntityByString: pszValue=%s", pszValue));
	return (*g_engfuncs.pfnFindEntityByString)(pEdictStartSearchAfter, pszField, pszValue);
}
int pfnGetEntityIllum(edict_t* pEnt)
{
	LOG_ENGINE("pfnGetEntityIllum:");
	return (*g_engfuncs.pfnGetEntityIllum)(pEnt);
}
edict_t* pfnFindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad)
{
	LOG_ENGINE("pfnFindEntityInSphere:");
	return (*g_engfuncs.pfnFindEntityInSphere)(pEdictStartSearchAfter, org, rad);
}
edict_t* pfnFindClientInPVS(edict_t *pEdict)
{
	LOG_ENGINE("pfnFindClientInPVS:");
	return (*g_engfuncs.pfnFindClientInPVS)(pEdict);
}
edict_t* pfnEntitiesInPVS(edict_t *pplayer)
{
	LOG_ENGINE("pfnEntitiesInPVS:");
	return (*g_engfuncs.pfnEntitiesInPVS)(pplayer);
}
void pfnMakeVectors(const float *rgflVector)
{
	//LOG_ENGINE("pfnMakeVectors:");
	(*g_engfuncs.pfnMakeVectors)(rgflVector);
}
void pfnAngleVectors(const float *rgflVector, float *forward, float *right, float *up)
{
	//LOG_ENGINE("pfnAngleVectors:");
	(*g_engfuncs.pfnAngleVectors)(rgflVector, forward, right, up);
}
edict_t* pfnCreateEntity(void)
{
	edict_t *pent = (*g_engfuncs.pfnCreateEntity)();
	LOG_ENGINE(UTIL_VarArgs("pfnCreateEntity: pent=%x", pent));
	return pent;
}
void pfnRemoveEntity(edict_t* e)
{
	//LOG_ENGINE(UTIL_VarArgs("pfnRemoveEntity: e=%x", e));
	LOG_ENGINE(UTIL_VarArgs("pfnRemoveEntity: e=%x, e->v.model=%s", e, e ? STRING(e->v.model) : ""));
	(*g_engfuncs.pfnRemoveEntity)(e);
}
edict_t* pfnCreateNamedEntity(int className)
{
	edict_t *pent = (*g_engfuncs.pfnCreateNamedEntity)(className);
	LOG_ENGINE(UTIL_VarArgs("pfnCreateNamedEntity: pent=%x, name=%s", pent, STRING(className)));
	return pent;
}
void pfnMakeStatic(edict_t *ent)
{
	LOG_ENGINE("pfnMakeStatic:");
	(*g_engfuncs.pfnMakeStatic)(ent);
}
int pfnEntIsOnFloor(edict_t *e)
{
	LOG_ENGINE("pfnEntIsOnFloor:");
	return (*g_engfuncs.pfnEntIsOnFloor)(e);
}
int pfnDropToFloor(edict_t* e)
{
	LOG_ENGINE("pfnDropToFloor:");
	return (*g_engfuncs.pfnDropToFloor)(e);
}
int pfnWalkMove(edict_t *ent, float yaw, float dist, int iMode)
{
	LOG_ENGINE("pfnWalkMove:");
	return (*g_engfuncs.pfnWalkMove)(ent, yaw, dist, iMode);
}
void pfnSetOrigin(edict_t *e, const float *rgflOrigin)
{
	LOG_ENGINE("pfnSetOrigin:");
	(*g_engfuncs.pfnSetOrigin)(e, rgflOrigin);
}
void pfnEmitSound(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch)
{
	LOG_ENGINE(UTIL_VarArgs("pfnEmitSound: channel=%d, sample=%s", channel, sample));
	(*g_engfuncs.pfnEmitSound)(entity, channel, sample, volume, attenuation, fFlags, pitch);
}
void pfnEmitAmbientSound(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
	LOG_ENGINE("pfnEmitAmbientSound:");
	(*g_engfuncs.pfnEmitAmbientSound)(entity, pos, samp, vol, attenuation, fFlags, pitch);
}
void pfnTraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	//LOG_ENGINE("pfnTraceLine:");
	(*g_engfuncs.pfnTraceLine)(v1, v2, fNoMonsters, pentToSkip, ptr);
}
void pfnTraceToss(edict_t* pent, edict_t* pentToIgnore, TraceResult *ptr)
{
	//LOG_ENGINE("pfnTraceToss:");
	(*g_engfuncs.pfnTraceToss)(pent, pentToIgnore, ptr);
}
int pfnTraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	//LOG_ENGINE("pfnTraceMonsterHull:");
	return (*g_engfuncs.pfnTraceMonsterHull)(pEdict, v1, v2, fNoMonsters, pentToSkip, ptr);
}
void pfnTraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
{
	//LOG_ENGINE("pfnTraceHull:");
	(*g_engfuncs.pfnTraceHull)(v1, v2, fNoMonsters, hullNumber, pentToSkip, ptr);
}
void pfnTraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
{
	//LOG_ENGINE("pfnTraceModel:");
	(*g_engfuncs.pfnTraceModel)(v1, v2, hullNumber, pent, ptr);
}
const char *pfnTraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2 )
{
	//LOG_ENGINE("pfnTraceTexture:");
	return (*g_engfuncs.pfnTraceTexture)(pTextureEntity, v1, v2);
}
void pfnTraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
{
	//LOG_ENGINE("pfnTraceSphere:");
	(*g_engfuncs.pfnTraceSphere)(v1, v2, fNoMonsters, radius, pentToSkip, ptr);
}
void pfnGetAimVector(edict_t* ent, float speed, float *rgflReturn)
{
	//LOG_ENGINE("pfnGetAimVector:");
	(*g_engfuncs.pfnGetAimVector)(ent, speed, rgflReturn);
}
void pfnServerCommand(char* str)
{
	LOG_ENGINE(UTIL_VarArgs("pfnServerCommand: str=%s", str));
	(*g_engfuncs.pfnServerCommand)(str);
}
void pfnServerExecute(void)
{
	LOG_ENGINE("pfnServerExecute:");
	(*g_engfuncs.pfnServerExecute)();
}
#endif /* not USE_METAMOD */
void pfnClientCommand(edict_t* pEdict, char* szFmt, ...)
{
	LOG_ENGINE(UTIL_VarArgs("pfnClientCommand: szFmt=%s", szFmt));
	
	if ( !(pEdict->v.flags & FL_FAKECLIENT) )
	{
		char tempFmt[256];
		va_list argp;
		va_start(argp, szFmt);
		vsnprintf(tempFmt, sizeof(tempFmt), szFmt, argp);
		(*g_engfuncs.pfnClientCommand)(pEdict, tempFmt);
		va_end(argp);
	}
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	return;
#endif /* USE_METAMOD */
}
#ifndef USE_METAMOD
void pfnParticleEffect(const float *org, const float *dir, float color, float count)
{
	//LOG_ENGINE("pfnParticleEffect:");
	(*g_engfuncs.pfnParticleEffect)(org, dir, color, count);
}
void pfnLightStyle(int style, char* val)
{
	//LOG_ENGINE("pfnLightStyle:");
	(*g_engfuncs.pfnLightStyle)(style, val);
}
int pfnDecalIndex(const char *name)
{
	//LOG_ENGINE("pfnDecalIndex:");
	return (*g_engfuncs.pfnDecalIndex)(name);
}
int pfnPointContents(const float *rgflVector)
{
	//LOG_ENGINE("pfnPointContents:");
	return (*g_engfuncs.pfnPointContents)(rgflVector);
}
#endif /* not USE_METAMOD */
void pfnMessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	if (gpGlobals->deathmatch)
	{
		LOG_ENGINE(UTIL_VarArgs("pfnMessageBegin: ed=%x, msg_dest=%d, msg_type=%s(%d)", ed, msg_dest, GET_USER_MSG_NAME(PLID, msg_type, NULL), msg_type));

		RegBotMsgs();
		msg_state = 0; // reset state on message begin

		// is this message for a player?
		if (ed)
		{
			botMsgIndex = UTIL_GetBotIndex(ed);

         	// is this message for a bot?
			if (ed->v.flags & FL_FAKECLIENT || botMsgIndex > -1)
				botMsgFunction = botmsgs[msg_type];
		}
		else
		{
			botMsgFunction = NULL; // no msg function until known otherwise
			botMsgIndex = -1; // index of bot receiving message (none)

			switch(msg_dest)
			{
				case MSG_INIT:
					if (msg_type == GET_USER_MSG_ID(PLID, "WeaponList", NULL))
						botMsgFunction = botmsgs[msg_type];
					break;
				case MSG_ALL:
					botMsgFunction = botmsgs[msg_type];
					break;
				default:
					;
			}
		}
	}

#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ed);
#endif /* USE_METAMOD */
}

void pfnMessageEnd(void)
{
	if (gpGlobals->deathmatch)
	{
		LOG_ENGINE("pfnMessageEnd:");
		
		botMsgFunction = NULL; // clear out the bot message function pointer...

		msg_state = 0; // reset state on message end
	}
	
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnMessageEnd)();
#endif /* USE_METAMOD */
}

void pfnWriteByte(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		LOG_ENGINE(UTIL_VarArgs("pfnWriteByte: iValue=%d", iValue));
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgIndex);

		msg_state++; // increment message state
	}
	
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnWriteByte)(iValue);
#endif /* USE_METAMOD */
}
void pfnWriteChar(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		LOG_ENGINE(UTIL_VarArgs("pfnWriteChar: iValue=%d", iValue));
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgIndex);

		msg_state++; // increment message state
	}
	
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnWriteChar)(iValue);
#endif /* USE_METAMOD */
}
void pfnWriteShort(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		LOG_ENGINE(UTIL_VarArgs("prnWriteShort: iValue=%d", iValue));
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgIndex);

		msg_state++; // increment message state
	}
	
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnWriteShort)(iValue);
#endif /* USE_METAMOD */
}
void pfnWriteLong(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		LOG_ENGINE(UTIL_VarArgs("pfnWriteLong: iValue=%d", iValue));
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgIndex);

		msg_state++; // increment message state
	}
	
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnWriteLong)(iValue);
#endif /* USE_METAMOD */
}
void pfnWriteAngle(float flValue)
{
	if (gpGlobals->deathmatch)
	{
		LOG_ENGINE(UTIL_VarArgs("pfnWriteAngle: flValue=%f", flValue));
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&flValue, botMsgIndex);

		msg_state++; // increment message state
	}
	
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnWriteAngle)(flValue);
#endif /* USE_METAMOD */
}
void pfnWriteCoord(float flValue)
{
	if (gpGlobals->deathmatch)
	{
		LOG_ENGINE(UTIL_VarArgs("pfnWriteCoord: flValue=%f", flValue));
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&flValue, botMsgIndex);

		msg_state++; // increment message state
	}
	
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnWriteCoord)(flValue);
#endif /* USE_METAMOD */
}
void pfnWriteString(const char *sz)
{
	if (gpGlobals->deathmatch)
	{
		LOG_ENGINE(UTIL_VarArgs("pfnWriteString: sz=%s", sz));
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)sz, botMsgIndex);

		msg_state++; // increment message state
	}
	
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnWriteString)(sz);
#endif /* USE_METAMOD */
}
void pfnWriteEntity(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		LOG_ENGINE(UTIL_VarArgs("pfnWriteEntity: iValue=%d", iValue));
		
		// if this message is for a bot, call the client message function...
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgIndex);

		msg_state++; // increment message state
	}
	
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnWriteEntity)(iValue);
#endif /* USE_METAMOD */
}
#ifndef USE_METAMOD
void pfnCVarRegister(cvar_t *pCvar)
{
	LOG_ENGINE("pfnCVarRegister:");
	(*g_engfuncs.pfnCVarRegister)(pCvar);
}
float pfnCVarGetFloat(const char *szVarName)
{
	//LOG_ENGINE(UTIL_VarArgs("pfnCVarGetFloat: szVarName=%s", szVarName));
	return (*g_engfuncs.pfnCVarGetFloat)(szVarName);
}
const char* pfnCVarGetString(const char *szVarName)
{
	//LOG_ENGINE("pfnCVarGetString:");
	return (*g_engfuncs.pfnCVarGetString)(szVarName);
}
void pfnCVarSetFloat(const char *szVarName, float flValue)
{
	//LOG_ENGINE("pfnCVarSetFloat:");
	(*g_engfuncs.pfnCVarSetFloat)(szVarName, flValue);
}
void pfnCVarSetString(const char *szVarName, const char *szValue)
{
	//LOG_ENGINE("pfnCVarSetString:");
	(*g_engfuncs.pfnCVarSetString)(szVarName, szValue);
}
#endif /* not USE_METAMOD */

void pfnAlertMessage(ALERT_TYPE atype, char *szFmt, ...)
{
	LOG_ENGINE("pfnAlertMessage:");
	va_list		argptr;
	static char		string[1024];
	char *cp;

	if (atype == at_logged)
	{
		va_start ( argptr, szFmt );
		vsnprintf ( string, sizeof(string), szFmt, argptr );
		va_end   ( argptr );

		cp = string;

		// World triggered ...
		if (cp && (cp[0] == 'W'))
		{
			// ... Round_Start
			if (!strncmp(cp + 6, "triggered \"Round_Start\"", 23))
			{
				g_fRoundStart = gpGlobals->time;
				g_fFreezeTimeEnd = g_fRoundStart;
			}
			// ... Round_End
			else if (!strncmp(cp + 6, "triggered \"Round_End\"", 21))
			{
				g_fFreezeTimeStart = gpGlobals->time + 5;
				g_fFreezeTimeEnd = g_fFreezeTimeStart + g_mp_freezetime->value;
				g_bInitRoundStart= true; // set flag that new round will start ....

				//CalcDistances();
			}
			// ... Game_Commencing
			else if (!strncmp(cp + 6, "triggered \"Game_Commencing\"", 27))
			{
			}
		}
	}
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnAlertMessage)(atype, string);
#endif /* USE_METAMOD */
}

#ifndef USE_METAMOD
void pfnEngineFprintf(void *pfile, char *szFmt, ...)
{
	LOG_ENGINE("pfnEngineFprintf:");
	va_list		argptr;
	static char		string[1024];

	va_start ( argptr, szFmt );
	vsnprintf ( string, sizeof(string), szFmt, argptr );
	va_end   ( argptr );

	(*g_engfuncs.pfnEngineFprintf)(pfile, string);
}
void* pfnPvAllocEntPrivateData(edict_t *pEdict, int32 cb)
{
	LOG_ENGINE("pfnPvAllocEntPrivateData:");
	return (*g_engfuncs.pfnPvAllocEntPrivateData)(pEdict, cb);
}
void* pfnPvEntPrivateData(edict_t *pEdict)
{
	LOG_ENGINE("pfnPvEntPrivateData:");
	return (*g_engfuncs.pfnPvEntPrivateData)(pEdict);
}
void pfnFreeEntPrivateData(edict_t *pEdict)
{
	LOG_ENGINE("pfnFreeEntPrivateData:");
	(*g_engfuncs.pfnFreeEntPrivateData)(pEdict);
}
const char* pfnSzFromIndex(int iString)
{
	LOG_ENGINE("pfnSzFromIndex:");
	return (*g_engfuncs.pfnSzFromIndex)(iString);
}
int pfnAllocString(const char *szValue)
{
	LOG_ENGINE("pfnAllocString:");
	return (*g_engfuncs.pfnAllocString)(szValue);
}
entvars_t* pfnGetVarsOfEnt(edict_t *pEdict)
{
	LOG_ENGINE("pfnGetVarsOfEnt:");
	return (*g_engfuncs.pfnGetVarsOfEnt)(pEdict);
}
edict_t* pfnPEntityOfEntOffset(int iEntOffset)
{
	//LOG_ENGINE("pfnPEntityOfEntOffset:");
	return (*g_engfuncs.pfnPEntityOfEntOffset)(iEntOffset);
}
int pfnEntOffsetOfPEntity(const edict_t *pEdict)
{
	//LOG_ENGINE(UTIL_VarArgs("pfnEntOffsetOfPEntity: pEdict=%x", pEdict));
	return (*g_engfuncs.pfnEntOffsetOfPEntity)(pEdict);
}
int pfnIndexOfEdict(const edict_t *pEdict)
{
	//LOG_ENGINE(UTIL_VarArgs("pfnIndexOfEdict: pEdict=%x", pEdict));
	return (*g_engfuncs.pfnIndexOfEdict)(pEdict);
}
edict_t* pfnPEntityOfEntIndex(int iEntIndex)
{
	//LOG_ENGINE("pfnPEntityOfEntIndex:");
	return (*g_engfuncs.pfnPEntityOfEntIndex)(iEntIndex);
}
edict_t* pfnFindEntityByVars(entvars_t* pvars)
{
	//LOG_ENGINE("pfnFindEntityByVars:");
	return (*g_engfuncs.pfnFindEntityByVars)(pvars);
}
void* pfnGetModelPtr(edict_t* pEdict)
{
	//LOG_ENGINE(UTIL_VarArgs("pfnGetModelPtr: pEdict=%x", pEdict));
	return (*g_engfuncs.pfnGetModelPtr)(pEdict);
}
int pfnRegUserMsg(const char *pszName, int iSize)
{
	if (gpGlobals->deathmatch)
	{
		int msg = REG_USER_MSG(pszName, iSize);
		LOG_ENGINE(UTIL_VarArgs("pfnRegUserMsg: pszName=%s, msg_id=%d", pszName, msg));
		AddUserMsg(pszName, msg, iSize);

		return msg;
	}
   
	return (*g_engfuncs.pfnRegUserMsg)(pszName, iSize);
}
void pfnAnimationAutomove(const edict_t* pEdict, float flTime)
{
	//LOG_ENGINE("pfnAnimationAutomove:");
	(*g_engfuncs.pfnAnimationAutomove)(pEdict, flTime);
}
void pfnGetBonePosition(const edict_t* pEdict, int iBone, float *rgflOrigin, float *rgflAngles )
{
	//LOG_ENGINE("pfnGetBonePosition:");
	(*g_engfuncs.pfnGetBonePosition)(pEdict, iBone, rgflOrigin, rgflAngles);
}
uint32 pfnFunctionFromName( const char *pName )
{
	LOG_ENGINE("pfnFunctionFromName:");
	return (*g_engfuncs.pfnFunctionFromName)(pName);
}
const char *pfnNameForFunction( uint32 function )
{
	LOG_ENGINE("pfnNameForFunction:");
	return (*g_engfuncs.pfnNameForFunction)(function);
}
#endif /* not USE_METAMOD */

void pfnClientPrintf( edict_t* pEdict, PRINT_TYPE ptype, const char *szMsg )
{
	LOG_ENGINE("pfnClientPrintf:");
	if ( pEdict->v.flags & FL_FAKECLIENT )
#ifdef USE_METAMOD
		RETURN_META(MRES_SUPERCEDE);

	RETURN_META(MRES_IGNORED);
#else
		return;

	(*g_engfuncs.pfnClientPrintf)(pEdict, ptype, szMsg);
#endif
}

#ifndef USE_METAMOD
void pfnServerPrint( const char *szMsg )
{
	LOG_ENGINE("pfnServerPrint:");
	(*g_engfuncs.pfnServerPrint)(szMsg);
}
void pfnGetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles )
{
	//LOG_ENGINE("pfnGetAttachment:");
	(*g_engfuncs.pfnGetAttachment)(pEdict, iAttachment, rgflOrigin, rgflAngles);
}
#endif /* not USE_METAMOD */

const char *pfnCmd_Args( void )
{
	if (g_bFakeClientCommand)
#ifdef USE_METAMOD
		RETURN_META_VALUE(MRES_SUPERCEDE, &g_argv[0]);

	RETURN_META_VALUE(MRES_IGNORED, NULL);
#else /* not USE_METAMOD */
		return &g_argv[0];

	return (*g_engfuncs.pfnCmd_Args)();
#endif /* USE_METAMOD */
}

const char *pfnCmd_Argv( int argc )
{
	if (g_bFakeClientCommand)
#ifdef USE_METAMOD
		RETURN_META_VALUE(MRES_SUPERCEDE, GetField (g_argv, argc));

	RETURN_META_VALUE(MRES_IGNORED, NULL);
#else /* not USE_METAMOD */
		return (GetField (g_argv, argc)); // returns the requested argument

	return (*g_engfuncs.pfnCmd_Argv)(argc);
#endif /* USE_METAMOD */
}

int pfnCmd_Argc( void )
{
	if (g_bFakeClientCommand)
#ifdef USE_METAMOD
		RETURN_META_VALUE(MRES_SUPERCEDE, fake_arg_count);

	RETURN_META_VALUE(MRES_IGNORED, 0);
#else /* not USE_METAMOD */
		return fake_arg_count;

	return (*g_engfuncs.pfnCmd_Argc)();
#endif /* USE_METAMOD */
}

#ifndef USE_METAMOD
void pfnCRC32_Init(CRC32_t *pulCRC)
{
	LOG_ENGINE("pfnCRC32_Init:");
	(*g_engfuncs.pfnCRC32_Init)(pulCRC);
}
void pfnCRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len)
{
	LOG_ENGINE("pfnCRC32_ProcessBuffer:");
	(*g_engfuncs.pfnCRC32_ProcessBuffer)(pulCRC, p, len);
}
void pfnCRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch)
{
	LOG_ENGINE("pfnCRC32_ProcessByte:");
	(*g_engfuncs.pfnCRC32_ProcessByte)(pulCRC, ch);
}
CRC32_t pfnCRC32_Final(CRC32_t pulCRC)
{
	LOG_ENGINE("pfnCRC32_Final:");
	return (*g_engfuncs.pfnCRC32_Final)(pulCRC);
}
int32 pfnRandomLong(int32 lLow, int32 lHigh)
{
	//LOG_ENGINE(UTIL_VarArgs("pfnRandomLong: lLow=%d, lHigh=%d", lLow, lHigh));
	return (*g_engfuncs.pfnRandomLong)(lLow, lHigh);
}
float pfnRandomFloat(float flLow, float flHigh)
{
	//LOG_ENGINE("pfnRandomFloat:");
	return (*g_engfuncs.pfnRandomFloat)(flLow, flHigh);
}
void pfnSetView(const edict_t *pClient, const edict_t *pViewent )
{
	LOG_ENGINE("pfnSetView:");
	(*g_engfuncs.pfnSetView)(pClient, pViewent);
}
float pfnTime( void )
{
	LOG_ENGINE("pfnTime:");
	return (*g_engfuncs.pfnTime)();
}
void pfnCrosshairAngle(const edict_t *pClient, float pitch, float yaw)
{
	LOG_ENGINE("pfnCrosshairAngle:");
	(*g_engfuncs.pfnCrosshairAngle)(pClient, pitch, yaw);
}
byte *pfnLoadFileForMe(char *filename, int *pLength)
{
	LOG_ENGINE(UTIL_VarArgs("pfnLoadFileForMe: filename=%s", filename));
	return (*g_engfuncs.pfnLoadFileForMe)(filename, pLength);
}
void pfnFreeFile(void *buffer)
{
	LOG_ENGINE("pfnFreeFile:");
	(*g_engfuncs.pfnFreeFile)(buffer);
}
void pfnEndSection(const char *pszSectionName)
{
	LOG_ENGINE("pfnEndSection:");
	(*g_engfuncs.pfnEndSection)(pszSectionName);
}
int pfnCompareFileTime(char *filename1, char *filename2, int *iCompare)
{
	LOG_ENGINE("pfnCompareFileTime:");
	return (*g_engfuncs.pfnCompareFileTime)(filename1, filename2, iCompare);
}
void pfnGetGameDir(char *szGetGameDir)
{
	LOG_ENGINE("pfnGetGameDir:");
	(*g_engfuncs.pfnGetGameDir)(szGetGameDir);
}
void pfnCvar_RegisterVariable(cvar_t *variable)
{
	LOG_ENGINE("pfnCvar_RegisterVariable:");
	(*g_engfuncs.pfnCvar_RegisterVariable)(variable);
}
void pfnFadeClientVolume(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds)
{
	LOG_ENGINE("pfnFadeClientVolume:");
	(*g_engfuncs.pfnFadeClientVolume)(pEdict, fadePercent, fadeOutSeconds, holdTime, fadeInSeconds);
}
#endif /* not USE_METAMOD */

void pfnSetClientMaxspeed(const edict_t *pEdict, float fNewMaxspeed)
{
	CBotBase *pB = UTIL_GetBotPointer(pEdict);

	if(pB){
		pB->f_max_speed = fNewMaxspeed;
		pB->pEdict->v.maxspeed = fNewMaxspeed; // backward compatibility with CS1.5

		if(mod_id == DOD_DLL)
			pB->f_max_speed /= 2;		// cause the other is sprinting speed
	}
	
	LOG_ENGINE(UTIL_VarArgs("pfnSetClientMaxspeed: pEdict=%x, fNewMaxspeed=%f", pEdict, fNewMaxspeed));
#ifdef USE_METAMOD
	RETURN_META(MRES_IGNORED);
#else /* not USE_METAMOD */
	(*g_engfuncs.pfnSetClientMaxspeed)(pEdict, fNewMaxspeed);
#endif /* USE_METAMOD */
}

#ifndef USE_METAMOD
edict_t * pfnCreateFakeClient(const char *netname)
{
	LOG_ENGINE("pfnCreateFakeClient:");
	return (*g_engfuncs.pfnCreateFakeClient)(netname);
}
void pfnRunPlayerMove(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec )
{
	LOG_ENGINE("pfnRunPlayerMove:");
	(*g_engfuncs.pfnRunPlayerMove)(fakeclient, viewangles, forwardmove, sidemove, upmove, buttons, impulse, msec);
}
int pfnNumberOfEntities(void)
{
	LOG_ENGINE("pfnNumberOfEntities:");
	return (*g_engfuncs.pfnNumberOfEntities)();
}
char* pfnGetInfoKeyBuffer(edict_t *e)
{
	LOG_ENGINE("pfnGetInfoKeyBuffer:");
	return (*g_engfuncs.pfnGetInfoKeyBuffer)(e);
}
char* pfnInfoKeyValue(char *infobuffer, char *key)
{
	LOG_ENGINE(UTIL_VarArgs("pfnInfoKeyValue: infobuffer=%s, key=%s", infobuffer, key));
	return (*g_engfuncs.pfnInfoKeyValue)(infobuffer, key);
}
void pfnSetKeyValue(char *infobuffer, char *key, char *value)
{
	LOG_ENGINE(UTIL_VarArgs("pfnSetKeyValue: infobuffer=%s, key=%s", key, value));
	(*g_engfuncs.pfnSetKeyValue)(infobuffer, key, value);
}
void pfnSetClientKeyValue(int clientIndex, char *infobuffer, char *key, char *value)
{
	LOG_ENGINE(UTIL_VarArgs("pfnSetClientKeyValue: key=%s, value=%s", key, value));
	(*g_engfuncs.pfnSetClientKeyValue)(clientIndex, infobuffer, key, value);
}
int pfnIsMapValid(char *filename)
{
	LOG_ENGINE("pfnIsMapValid:");
	return (*g_engfuncs.pfnIsMapValid)(filename);
}
void pfnStaticDecal( const float *origin, int decalIndex, int entityIndex, int modelIndex )
{
	LOG_ENGINE("pfnStaticDecal:");
	(*g_engfuncs.pfnStaticDecal)(origin, decalIndex, entityIndex, modelIndex);
}
int pfnPrecacheGeneric(char* s)
{
	LOG_ENGINE(UTIL_VarArgs("pfnPrecacheGeneric: s=%s", s));
	return (*g_engfuncs.pfnPrecacheGeneric)(s);
}
#endif /* not USE_METAMOD */

int pfnGetPlayerUserId(edict_t *e )
{
	if (gpGlobals->deathmatch)
	{
		LOG_ENGINE(UTIL_VarArgs("pfnGetPlayerUserId: e=%x", e));
		
		if (mod_id == GEARBOX_DLL)
		{
			// is this edict a bot?
			if (UTIL_GetBotIndex( e ) > -1)
#ifdef USE_METAMOD
				RETURN_META_VALUE(MRES_SUPERCEDE, 0);
#else /* not USE_METAMOD */
				return 0;  // don't return a valid index (so bot won't get kicked)
#endif /* USE_METAMOD */
		}
	}
	
#ifdef USE_METAMOD
	RETURN_META_VALUE(MRES_IGNORED, 0);
#else /* not USE_METAMOD */
	return (*g_engfuncs.pfnGetPlayerUserId)(e);
#endif /* USE_METAMOD */
}

#ifndef USE_METAMOD
void pfnBuildSoundMsg(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	LOG_ENGINE("pfnBuildSoundMsg:");
	(*g_engfuncs.pfnBuildSoundMsg)(entity, channel, sample, volume, attenuation, fFlags, pitch, msg_dest, msg_type, pOrigin, ed);
}
int pfnIsDedicatedServer(void)
{
	LOG_ENGINE("pfnIsDedicatedServer:");
	return (*g_engfuncs.pfnIsDedicatedServer)();
}
cvar_t* pfnCVarGetPointer(const char *szVarName)
{
	LOG_ENGINE(UTIL_VarArgs("pfnCVarGetPointer: szVarName=%s", szVarName));
	return (*g_engfuncs.pfnCVarGetPointer)(szVarName);
}
unsigned int pfnGetPlayerWONId(edict_t *e)
{
	LOG_ENGINE(UTIL_VarArgs("pfnGetPlayerWONId: e=%x", e));
	return (*g_engfuncs.pfnGetPlayerWONId)(e);
}

// new stuff for SDK 2.0

void pfnInfo_RemoveKey(char *s, const char *key)
{
	LOG_ENGINE("pfnInfo_RemoveKey:");
	(*g_engfuncs.pfnInfo_RemoveKey)(s, key);
}
const char *pfnGetPhysicsKeyValue(const edict_t *pClient, const char *key)
{
	LOG_ENGINE("pfnGetPhysicsKeyValue:");
	return (*g_engfuncs.pfnGetPhysicsKeyValue)(pClient, key);
}
void pfnSetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value)
{
	LOG_ENGINE("pfnSetPhysicsKeyValue:");
	(*g_engfuncs.pfnSetPhysicsKeyValue)(pClient, key, value);
}
const char *pfnGetPhysicsInfoString(const edict_t *pClient)
{
	LOG_ENGINE("pfnGetPhysicsInfoString:");
	return (*g_engfuncs.pfnGetPhysicsInfoString)(pClient);
}
unsigned short pfnPrecacheEvent(int type, const char *psz)
{
	LOG_ENGINE("pfnPrecacheEvent:");
	return (*g_engfuncs.pfnPrecacheEvent)(type, psz);
}
void pfnPlaybackEvent(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay,
					  float *origin, float *angles, float fparam1,float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
	LOG_ENGINE("pfnPlaybackEvent:");
	(*g_engfuncs.pfnPlaybackEvent)(flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
}
unsigned char *pfnSetFatPVS(float *org)
{
	LOG_ENGINE("pfnSetFatPVS:");
	return (*g_engfuncs.pfnSetFatPVS)(org);
}
unsigned char *pfnSetFatPAS(float *org)
{
	LOG_ENGINE("pfnSetFatPAS:");
	return (*g_engfuncs.pfnSetFatPAS)(org);
}
int pfnCheckVisibility(const edict_t *entity, unsigned char *pset)
{
	//LOG_ENGINE("pfnCheckVisibility:");
	return (*g_engfuncs.pfnCheckVisibility)(entity, pset);
}
void pfnDeltaSetField(struct delta_s *pFields, const char *fieldname)
{
	//LOG_ENGINE("pfnDeltaSetField:");
	(*g_engfuncs.pfnDeltaSetField)(pFields, fieldname);
}
void pfnDeltaUnsetField(struct delta_s *pFields, const char *fieldname)
{
	//LOG_ENGINE("pfnDeltaUnsetField:");
	(*g_engfuncs.pfnDeltaUnsetField)(pFields, fieldname);
}
void pfnDeltaAddEncoder(char *name, void (*conditionalencode)( struct delta_s *pFields, const unsigned char *from, const unsigned char *to))
{
	//LOG_ENGINE("pfnDeltaAddEncoder:");
	(*g_engfuncs.pfnDeltaAddEncoder)(name, conditionalencode);
}
int pfnGetCurrentPlayer(void)
{
	LOG_ENGINE("pfnGetCurrentPlayer:");
	return (*g_engfuncs.pfnGetCurrentPlayer)();
}
int pfnCanSkipPlayer(const edict_t *player)
{
	LOG_ENGINE("pfnCanSkipPlayer:");
	return (*g_engfuncs.pfnCanSkipPlayer)(player);
}
int pfnDeltaFindField(struct delta_s *pFields, const char *fieldname)
{
	//LOG_ENGINE("pfnDeltaFindField:");
	return (*g_engfuncs.pfnDeltaFindField)(pFields, fieldname);
}
void pfnDeltaSetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
	LOG_ENGINE("pfnDeltaSetFieldByIndex:");
	(*g_engfuncs.pfnDeltaSetFieldByIndex)(pFields, fieldNumber);
}
void pfnDeltaUnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
	//LOG_ENGINE("pfnDeltaUnsetFieldByIndex:");
	(*g_engfuncs.pfnDeltaUnsetFieldByIndex)(pFields, fieldNumber);
}
void pfnSetGroupMask(int mask, int op)
{
	LOG_ENGINE("pfnSetGroupMask:");
	(*g_engfuncs.pfnSetGroupMask)(mask, op);
}
int pfnCreateInstancedBaseline(int classname, struct entity_state_s *baseline)
{
	LOG_ENGINE("pfnCreateInstancedBaseline:");
	return (*g_engfuncs.pfnCreateInstancedBaseline)(classname, baseline);
}
void pfnCvar_DirectSet(struct cvar_s *var, char *value)
{
	LOG_ENGINE("pfnCvar_DirectSet:");
	(*g_engfuncs.pfnCvar_DirectSet)(var, value);
}
void pfnForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename)
{
	LOG_ENGINE("pfnForceUnmodified:");
	(*g_engfuncs.pfnForceUnmodified)(type, mins, maxs, filename);
}
void pfnGetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss)
{
	LOG_ENGINE("pfnGetPlayerStats:");
	(*g_engfuncs.pfnGetPlayerStats)(pClient, ping, packet_loss);
}
void pfnAddServerCommand(char *cmd_name, void (*function)(void))
{
   LOG_ENGINE(UTIL_VarArgs("pfnAddServerCommand: cmd_name=%s, function=%x", cmd_name, function));
   (*g_engfuncs.pfnAddServerCommand)(cmd_name, function);
}

// hl1108
qboolean pfnVoice_GetClientListening(int iReceiver, int iSender)
{
	return (*g_engfuncs.pfnVoice_GetClientListening)(iReceiver, iSender); 
}
qboolean pfnVoice_SetClientListening(int iReceiver, int iSender, qboolean bListen)
{
	return (*g_engfuncs.pfnVoice_SetClientListening)(iReceiver, iSender, bListen); 
}
#endif /* not USE_METAMOD */
// hl1109
const char *pfnGetPlayerAuthId(edict_t *e)
{
	if (e->v.flags & FL_FAKECLIENT)
#ifdef USE_METAMOD
		RETURN_META_VALUE(MRES_SUPERCEDE, "0");
#else /* not USE_METAMOD */
		return "0";
#endif /* USE_METAMOD */

#ifdef USE_METAMOD
	RETURN_META_VALUE(MRES_IGNORED, NULL);
#else /* not USE_METAMOD */
	return (*g_engfuncs.pfnGetPlayerAuthId)(e); 
#endif /* USE_METAMOD */
}
#ifndef USE_METAMOD
#ifdef HLSDK_20031110
// 2003/11/10
void *pfnSequenceGet(const char* fileName, const char* entryName)
{
	return (*g_engfuncs.pfnSequenceGet)(fileName, entryName);
}
void *pfnSequencePickSentence(const char* groupName, int pickMethod, int *picked)
{
	return (*g_engfuncs.pfnSequencePickSentence)(groupName, pickMethod, picked);
}
int pfnGetFileSize(char *filename)
{
	return (*g_engfuncs.pfnGetFileSize)(filename);
}
unsigned int pfnGetApproxWavePlayLen(const char *filepath)
{
	return (*g_engfuncs.pfnGetApproxWavePlayLen)(filepath);
}
int pfnIsCareerMatch(void)
{
	return (*g_engfuncs.pfnIsCareerMatch)();
}
int pfnGetLocalizedStringLength(const char *label)
{
	return (*g_engfuncs.pfnGetLocalizedStringLength)(label);
}
void pfnRegisterTutorMessageShown(int mid)
{
	(*g_engfuncs.pfnRegisterTutorMessageShown)(mid);
}
int pfnGetTimesTutorMessageShown(int mid)
{
	return (*g_engfuncs.pfnGetTimesTutorMessageShown)(mid);
}
void pfnProcessTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
	(*g_engfuncs.pfnProcessTutorMessageDecayBuffer)(buffer, bufferLength);
}
void pfnConstructTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
	(*g_engfuncs.pfnConstructTutorMessageDecayBuffer)(buffer, bufferLength);
}
void pfnResetTutorMessageDecayData(void)
{
	(*g_engfuncs.pfnResetTutorMessageDecayData)();
}
#endif /* HLSDK_20031110 */
#endif /* not USE_METAMOD */

#ifdef USE_METAMOD
enginefuncs_t meta_engfuncs = {
	NULL,			// pfnPrecacheModel()
	NULL,			// pfnPrecacheSound()
	NULL,			// pfnSetModel()
	NULL,			// pfnModelIndex()
	NULL,			// pfnModelFrames()

	NULL,			// pfnSetSize()
	pfnChangeLevel,	// pfnChangeLevel()
	NULL,			// pfnGetSpawnParms()
	NULL,			// pfnSaveSpawnParms()

	NULL,			// pfnVecToYaw()
	NULL,			// pfnVecToAngles()
	NULL,			// pfnMoveToOrigin()
	NULL,			// pfnChangeYaw()
	NULL,			// pfnChangePitch()

	NULL,			// pfnFindEntityByString()
	NULL,			// pfnGetEntityIllum()
	NULL,			// pfnFindEntityInSphere()
	NULL,			// pfnFindClientInPVS()
	NULL,			// pfnEntitiesInPVS()

	NULL,			// pfnMakeVectors()
	NULL,			// pfnAngleVectors()

	NULL,			// pfnCreateEntity()
	NULL,			// pfnRemoveEntity()
	NULL,			// pfnCreateNamedEntity()

	NULL,			// pfnMakeStatic()
	NULL,			// pfnEntIsOnFloor()
	NULL,			// pfnDropToFloor()

	NULL,			// pfnWalkMove()
	NULL,			// pfnSetOrigin()

	NULL,			// pfnEmitSound()
	NULL,			// pfnEmitAmbientSound()

	NULL,			// pfnTraceLine()
	NULL,			// pfnTraceToss()
	NULL,			// pfnTraceMonsterHull()
	NULL,			// pfnTraceHull()
	NULL,			// pfnTraceModel()
	NULL,			// pfnTraceTexture()
	NULL,			// pfnTraceSphere()
	NULL,			// pfnGetAimVector()

	NULL,			// pfnServerCommand()
	NULL,			// pfnServerExecute()
	pfnClientCommand,	// pfnClientCommand()

	NULL,			// pfnParticleEffect()
	NULL,			// pfnLightStyle()
	NULL,			// pfnDecalIndex()
	NULL,			// pfnPointContents()

	pfnMessageBegin,	// pfnMessageBegin()
	pfnMessageEnd,	// pfnMessageEnd()

	pfnWriteByte,	// pfnWriteByte()
	pfnWriteChar,	// pfnWriteChar()
	pfnWriteShort,	// pfnWriteShort()
	pfnWriteLong,	// pfnWriteLong()
	pfnWriteAngle,	// pfnWriteAngle()
	pfnWriteCoord,	// pfnWriteCoord()
	pfnWriteString,	// pfnWriteString()
	pfnWriteEntity,	// pfnWriteEntity()

	NULL,			// pfnCVarRegister()
	NULL,			// pfnCVarGetFloat()
	NULL,			// pfnCVarGetString()
	NULL,			// pfnCVarSetFloat()
	NULL,			// pfnCVarSetString()

	pfnAlertMessage,	// pfnAlertMessage()
	NULL,			// pfnEngineFprintf()

	NULL,			// pfnPvAllocEntPrivateData()
	NULL,			// pfnPvEntPrivateData()
	NULL,			// pfnFreeEntPrivateData()

	NULL,			// pfnSzFromIndex()
	NULL,			// pfnAllocString()

	NULL,		 	// pfnGetVarsOfEnt()
	NULL,			// pfnPEntityOfEntOffset()
	NULL,			// pfnEntOffsetOfPEntity()
	NULL,			// pfnIndexOfEdict()
	NULL,			// pfnPEntityOfEntIndex()
	NULL,			// pfnFindEntityByVars()
	NULL,			// pfnGetModelPtr()

	NULL,			// pfnRegUserMsg()

	NULL,			// pfnAnimationAutomove()
	NULL,			// pfnGetBonePosition()

	NULL,			// pfnFunctionFromName()
	NULL,			// pfnNameForFunction()

	pfnClientPrintf,	// pfnClientPrintf()			//! JOHN: engine callbacks so game DLL can print messages to individual clients
	NULL,			// pfnServerPrint()

	pfnCmd_Args,	// pfnCmd_Args()	//! these 3 added 
	pfnCmd_Argv,	// pfnCmd_Argv()	//! so game DLL can easily 
	pfnCmd_Argc,	// pfnCmd_Argc()	//! access client 'cmd' strings

	NULL,			// pfnGetAttachment()

	NULL,			// pfnCRC32_Init()
	NULL,			// pfnCRC32_ProcessBuffer()
	NULL,			// pfnCRC32_ProcessByte()
	NULL,			// pfnCRC32_Final()

	NULL,			// pfnRandomLong()
	NULL,			// pfnRandomFloat()

	NULL,			// pfnSetView()
	NULL,			// pfnTime()
	NULL,			// pfnCrosshairAngle()

	NULL,			// pfnLoadFileForMe()
	NULL,			// pfnFreeFile()

	NULL,			// pfnEndSection()				//! trigger_endsection
	NULL,			// pfnCompareFileTime()
	NULL,			// pfnGetGameDir()
	NULL,			// pfnCvar_RegisterVariable()
	NULL,			// pfnFadeClientVolume()
	pfnSetClientMaxspeed,	// pfnSetClientMaxspeed()
	NULL,			// pfnCreateFakeClient() 		//! returns NULL if fake client can't be created
	NULL,			// pfnRunPlayerMove()
	NULL,			// pfnNumberOfEntities()

	NULL,			// pfnGetInfoKeyBuffer()		//! passing in NULL gets the serverinfo
	NULL,			// pfnInfoKeyValue()
	NULL,			// pfnSetKeyValue()
	NULL,			// pfnSetClientKeyValue()

	NULL,			// pfnIsMapValid()
	NULL,			// pfnStaticDecal()
	NULL,			// pfnPrecacheGeneric()
	pfnGetPlayerUserId,	// pfnGetPlayerUserId()			//! returns the server assigned userid for this player.
	NULL,			// pfnBuildSoundMsg()
	NULL,			// pfnIsDedicatedServer()		//! is this a dedicated server?
	NULL,			// pfnCVarGetPointer()
	NULL,			// pfnGetPlayerWONId()			//! returns the server assigned WONid for this player.

	//! YWB 8/1/99 TFF Physics additions
	NULL,			// pfnInfo_RemoveKey()
	NULL,			// pfnGetPhysicsKeyValue()
	NULL,			// pfnSetPhysicsKeyValue()
	NULL,			// pfnGetPhysicsInfoString()
	NULL,			// pfnPrecacheEvent()
	NULL,			// pfnPlaybackEvent()

	NULL,			// pfnSetFatPVS()
	NULL,			// pfnSetFatPAS()

	NULL,			// pfnCheckVisibility()

	NULL,			// pfnDeltaSetField()
	NULL,			// pfnDeltaUnsetField()
	NULL,			// pfnDeltaAddEncoder()
	NULL,			// pfnGetCurrentPlayer()
	NULL,			// pfnCanSkipPlayer()
	NULL,			// pfnDeltaFindField()
	NULL,			// pfnDeltaSetFieldByIndex()
	NULL,			// pfnDeltaUnsetFieldByIndex()

	NULL,			// pfnSetGroupMask()

	NULL,			// pfnCreateInstancedBaseline()		// d'oh, CreateInstancedBaseline in dllapi too
	NULL,			// pfnCvar_DirectSet()

	NULL,			// pfnForceUnmodified()

	NULL,			// pfnGetPlayerStats()

	NULL,			// pfnAddServerCommand()

	NULL,			// pfnVoice_GetClientListening()
	NULL,			// pfnVoice_SetClientListening()
	pfnGetPlayerAuthId,	// pfnGetPlayerAuthId()

	NULL,			// pfnSequenceGet()
	NULL,			// pfnSequencePickSentence()
	NULL,			// pfnGetFileSize()
	NULL,			// pfnGetApproxWavePlayLen()
	NULL,			// pfnIsCareerMatch()
	NULL,			// pfnGetLocalizedStringLength()
	NULL,			// pfnRegisterTutorMessageShown()
	NULL,			// pfnGetTimesTutorMessageShown()
	NULL,			// pfnProcessTutorMessageDecayBuffer()
	NULL,			// pfnConstructTutorMessageDecayBuffer()
	NULL,			// pfnResetTutorMessageDecayData()
};

int GetEngineFunctions(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion ) 
{
	if(!pengfuncsFromEngine) {
		LOG_ERROR(PLID, "GetEngineFunctions called with null pengfuncsFromEngine");
		return(FALSE);
	}
	else if(*interfaceVersion != ENGINE_INTERFACE_VERSION) {
		LOG_ERROR(PLID, "GetEngineFunctions version mismatch; requested=%d ours=%d", *interfaceVersion, ENGINE_INTERFACE_VERSION);
		// Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = ENGINE_INTERFACE_VERSION;
		return(FALSE);
	}
	memcpy(pengfuncsFromEngine, &meta_engfuncs, sizeof(enginefuncs_t));
	return TRUE;
}
#endif /* USE_METAMOD */
