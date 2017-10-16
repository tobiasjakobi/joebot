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


//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// h_export.cpp
//
#include <sys/stat.h>

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "engine.h"

#include "bot_modid.h"
#include "CBotBase.h"
#include "globalvars.h"

#ifdef _WIN32

HINSTANCE h_Library = NULL;
HGLOBAL h_global_argv = NULL;
void FreeNameFuncGlobals(void);
void LoadSymbols(char *filename);

#else

void *h_Library = NULL;
char h_global_argv[1024];

#endif

enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;
char *g_argv;

#ifndef USE_METAMOD
GETENTITYAPI other_GetEntityAPI = NULL;
GETNEWDLLFUNCTIONS other_GetNewDLLFunctions = NULL;
GIVEFNPTRSTODLL other_GiveFnptrsToDll = NULL;
SERVER_GETBLENDINGINTERFACE other_Server_GetBlendingInterface = NULL;
#endif /* not USE_METAMOD */

int mod_id = -1;
char mod_name[32];

// Adapted from metamod game_support.cpp
typedef struct game_modinfo_s {
	char *name;		// name (the game dir)
	char *linux_so;	// filename of linux shared lib
	char *win_dll;	// filename of win32 dll
	char *desc;		// our long-name description
	int mod_id;		// mod ID
} game_modinfo_t;

typedef game_modinfo_t game_modlist_t[];

game_modlist_t known_games = {
	{"valve",	"hl_i386.so",	"hl.dll",	"Half-Life Deathmatch", VALVE_DLL},
	{"tfc",	"tfc_i386.so",	"tfc.dll",	"Team Fortress Classic", TFC_DLL},
	{"cstrike",	"cs_i386.so",	"mp.dll",	"Counter-Strike", CSTRIKE_DLL},
	{"retrocs",	"rcs_i386.so",	"rcs.dll",	"Retro Counter-Strike", CSTRIKE_DLL},
	{"csclassic",	"cs_i386.so",	"mp.dll",	"Counter-Strike", CSCLASSIC_DLL},
	{"gearbox",	"opfor_i386.so",	"opfor.dll",	"Opposing Force", GEARBOX_DLL},
	{"dod",	"dod_i386.so",	"dod.dll",	"Day of Defeat", DOD_DLL},
	{"frontline",	"front_i386.so",	"frontline.dll",	"Frontline Force", FRONTLINE_DLL},
	{NULL, NULL, NULL, NULL}
};

// Find a modinfo corresponding to the given game name.
game_modinfo_t *lookup_game(const char *name) {
	game_modinfo_t *imod;
	int i;
	for(i=0; known_games[i].name; i++) {
		imod=&known_games[i];
		if(FStrEq(imod->name, name))
			return(imod);
	}
	// no match found
	return(NULL);
}

#ifdef _WIN32

// Required DLL entry point
BOOL WINAPI DllMain(
   HINSTANCE hinstDLL,
   DWORD fdwReason,
   LPVOID lpvReserved)
{
   if (fdwReason == DLL_PROCESS_ATTACH)
   {
   }
   else if (fdwReason == DLL_PROCESS_DETACH)
   {
      FreeNameFuncGlobals();  // Free exported symbol table

      if (h_Library)
         FreeLibrary(h_Library);

      if (h_global_argv)
      {
         GlobalUnlock(h_global_argv);
         GlobalFree(h_global_argv);
      }
   }
   return TRUE;
}

#endif

#ifdef _WIN32
#if defined(__BORLANDC__) || defined(__MINGW32__)
extern "C" DLLEXPORT void EXPORT GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
#else
void DLLEXPORT GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
#endif
#else
extern "C" DLLEXPORT void GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
#endif
{
   char game_dir[256];
   char game_name[32];
   char game_dll[256];

   // get the engine functions from the engine...
   
   // check if we're running under Steam
   struct stat buf;
   if (stat("valve/steam.inf", &buf) == 0 || stat("FileSystem_Steam.dll", &buf) == 0)
      g_bIsSteam = true;

   if (g_bIsSteam)
      memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
   else
      memcpy(&g_engfuncs, pengfuncsFromEngine, 144 * sizeof(uint32));
  		 
   gpGlobals = pGlobals;
   
   // find the directory name of the currently running MOD...
   GET_GAME_DIR(game_dir);
   UTIL_normalize_pathname(game_dir);
   char *cp = game_dir;
   
   if (strstr(game_dir, "/") != NULL)
	   cp = strrchr(game_dir, '/') + 1;

   if (!strlen(cp))
   {
      // Error getting directory name!
      ALERT( at_error, "[JOEBOT] Error determining MOD directory name!\n" );
	  return;
   }
   strcpy(game_name, cp);
   game_modinfo_t *known = lookup_game(game_name);

   if (known)
   {
      mod_id = known->mod_id;
	  strcpy(mod_name, game_name);
#ifdef _WIN32
      snprintf(game_dll, sizeof(game_dll), "%s/dlls/%s", known->name, known->win_dll);
      h_Library = LoadLibrary(game_dll);
      h_global_argv = GlobalAlloc(GMEM_SHARE, 1024);
      g_argv = (char *)GlobalLock(h_global_argv);
#else
      snprintf(game_dll, sizeof(game_dll), "%s/dlls/%s", known->name, known->linux_so);
      h_Library = dlopen(game_dll, RTLD_NOW);
      g_argv = (char *)h_global_argv;
#endif
   }
   else
   {
      // Directory error or Unsupported MOD!
      ALERT( at_error, "[JOEBOT] MOD dll not found (or unsupported MOD)!\n" );
      return;
   }

#ifndef USE_METAMOD
   other_GetEntityAPI = (GETENTITYAPI)GetProcAddress(h_Library, "GetEntityAPI");

   if (other_GetEntityAPI == NULL)
   {
      // Can't find GetEntityAPI!

		ALERT( at_error, "[JOEBOT] Can't get MOD's GetEntityAPI!\n" );
   }

   other_GetNewDLLFunctions = (GETNEWDLLFUNCTIONS)GetProcAddress(h_Library, "GetNewDLLFunctions");

//   if (other_GetNewDLLFunctions == NULL)
//   {
//      // Can't find GetNewDLLFunctions!
//
//		ALERT( at_error, "JoeBOT - Can't get MOD's GetNewDLLFunctions!" );
//   }

   other_GiveFnptrsToDll = (GIVEFNPTRSTODLL)GetProcAddress(h_Library, "GiveFnptrsToDll");

   if (other_GiveFnptrsToDll == NULL)
   {
      // Can't find GiveFnptrsToDll!

		ALERT( at_error, "[JOEBOT] Can't get MOD's GiveFnptrsToDll!\n" );
   }

   other_Server_GetBlendingInterface = (SERVER_GETBLENDINGINTERFACE)GetProcAddress(h_Library, "Server_GetBlendingInterface");

   if (other_Server_GetBlendingInterface == NULL)
   {
      // Can't find Server_GetBlendingInterface!

		ALERT( at_error, "[JOEBOT] Can't get MOD's Server_GetBlendingInterface!\n" );
   }

#ifdef _WIN32
   LoadSymbols(game_dll);  // Load exported symbol table
#endif

   pengfuncsFromEngine->pfnPrecacheModel = pfnPrecacheModel;
   pengfuncsFromEngine->pfnPrecacheSound = pfnPrecacheSound;
   pengfuncsFromEngine->pfnSetModel = pfnSetModel;
   pengfuncsFromEngine->pfnModelIndex = pfnModelIndex;
   pengfuncsFromEngine->pfnModelFrames = pfnModelFrames;
   pengfuncsFromEngine->pfnSetSize = pfnSetSize;
   pengfuncsFromEngine->pfnChangeLevel = pfnChangeLevel;
   pengfuncsFromEngine->pfnGetSpawnParms = pfnGetSpawnParms;
   pengfuncsFromEngine->pfnSaveSpawnParms = pfnSaveSpawnParms;
   pengfuncsFromEngine->pfnVecToYaw = pfnVecToYaw;
   pengfuncsFromEngine->pfnVecToAngles = pfnVecToAngles;
   pengfuncsFromEngine->pfnMoveToOrigin = pfnMoveToOrigin;
   pengfuncsFromEngine->pfnChangeYaw = pfnChangeYaw;
   pengfuncsFromEngine->pfnChangePitch = pfnChangePitch;
   pengfuncsFromEngine->pfnFindEntityByString = pfnFindEntityByString;
   pengfuncsFromEngine->pfnGetEntityIllum = pfnGetEntityIllum;
   pengfuncsFromEngine->pfnFindEntityInSphere = pfnFindEntityInSphere;
   pengfuncsFromEngine->pfnFindClientInPVS = pfnFindClientInPVS;
   pengfuncsFromEngine->pfnEntitiesInPVS = pfnEntitiesInPVS;
   pengfuncsFromEngine->pfnMakeVectors = pfnMakeVectors;
   pengfuncsFromEngine->pfnAngleVectors = pfnAngleVectors;
   pengfuncsFromEngine->pfnCreateEntity = pfnCreateEntity;
   pengfuncsFromEngine->pfnRemoveEntity = pfnRemoveEntity;
   pengfuncsFromEngine->pfnCreateNamedEntity = pfnCreateNamedEntity;
   pengfuncsFromEngine->pfnMakeStatic = pfnMakeStatic;
   pengfuncsFromEngine->pfnEntIsOnFloor = pfnEntIsOnFloor;
   pengfuncsFromEngine->pfnDropToFloor = pfnDropToFloor;
   pengfuncsFromEngine->pfnWalkMove = pfnWalkMove;
   pengfuncsFromEngine->pfnSetOrigin = pfnSetOrigin;
   pengfuncsFromEngine->pfnEmitSound = pfnEmitSound;
   pengfuncsFromEngine->pfnEmitAmbientSound = pfnEmitAmbientSound;
   pengfuncsFromEngine->pfnTraceLine = pfnTraceLine;
   pengfuncsFromEngine->pfnTraceToss = pfnTraceToss;
   pengfuncsFromEngine->pfnTraceMonsterHull = pfnTraceMonsterHull;
   pengfuncsFromEngine->pfnTraceHull = pfnTraceHull;
   pengfuncsFromEngine->pfnTraceModel = pfnTraceModel;
   pengfuncsFromEngine->pfnTraceTexture = pfnTraceTexture;
   pengfuncsFromEngine->pfnTraceSphere = pfnTraceSphere;
   pengfuncsFromEngine->pfnGetAimVector = pfnGetAimVector;
   pengfuncsFromEngine->pfnServerCommand = pfnServerCommand;
   pengfuncsFromEngine->pfnServerExecute = pfnServerExecute;
   pengfuncsFromEngine->pfnClientCommand = pfnClientCommand;
   pengfuncsFromEngine->pfnParticleEffect = pfnParticleEffect;
   pengfuncsFromEngine->pfnLightStyle = pfnLightStyle;
   pengfuncsFromEngine->pfnDecalIndex = pfnDecalIndex;
   pengfuncsFromEngine->pfnPointContents = pfnPointContents;
   pengfuncsFromEngine->pfnMessageBegin = pfnMessageBegin;
   pengfuncsFromEngine->pfnMessageEnd = pfnMessageEnd;
   pengfuncsFromEngine->pfnWriteByte = pfnWriteByte;
   pengfuncsFromEngine->pfnWriteChar = pfnWriteChar;
   pengfuncsFromEngine->pfnWriteShort = pfnWriteShort;
   pengfuncsFromEngine->pfnWriteLong = pfnWriteLong;
   pengfuncsFromEngine->pfnWriteAngle = pfnWriteAngle;
   pengfuncsFromEngine->pfnWriteCoord = pfnWriteCoord;
   pengfuncsFromEngine->pfnWriteString = pfnWriteString;
   pengfuncsFromEngine->pfnWriteEntity = pfnWriteEntity;
   pengfuncsFromEngine->pfnCVarRegister = pfnCVarRegister;
   pengfuncsFromEngine->pfnCVarGetFloat = pfnCVarGetFloat;
   pengfuncsFromEngine->pfnCVarGetString = pfnCVarGetString;
   pengfuncsFromEngine->pfnCVarSetFloat = pfnCVarSetFloat;
   pengfuncsFromEngine->pfnCVarSetString = pfnCVarSetString;
   pengfuncsFromEngine->pfnAlertMessage = pfnAlertMessage;
   pengfuncsFromEngine->pfnEngineFprintf = pfnEngineFprintf;
   pengfuncsFromEngine->pfnPvAllocEntPrivateData = pfnPvAllocEntPrivateData;
   pengfuncsFromEngine->pfnPvEntPrivateData = pfnPvEntPrivateData;
   pengfuncsFromEngine->pfnFreeEntPrivateData = pfnFreeEntPrivateData;
   pengfuncsFromEngine->pfnSzFromIndex = pfnSzFromIndex;
   pengfuncsFromEngine->pfnAllocString = pfnAllocString;
   pengfuncsFromEngine->pfnGetVarsOfEnt = pfnGetVarsOfEnt;
   pengfuncsFromEngine->pfnPEntityOfEntOffset = pfnPEntityOfEntOffset;
   pengfuncsFromEngine->pfnEntOffsetOfPEntity = pfnEntOffsetOfPEntity;
   pengfuncsFromEngine->pfnIndexOfEdict = pfnIndexOfEdict;
   pengfuncsFromEngine->pfnPEntityOfEntIndex = pfnPEntityOfEntIndex;
   pengfuncsFromEngine->pfnFindEntityByVars = pfnFindEntityByVars;
   pengfuncsFromEngine->pfnGetModelPtr = pfnGetModelPtr;
   pengfuncsFromEngine->pfnRegUserMsg = pfnRegUserMsg;
   pengfuncsFromEngine->pfnAnimationAutomove = pfnAnimationAutomove;
   pengfuncsFromEngine->pfnGetBonePosition = pfnGetBonePosition;
   pengfuncsFromEngine->pfnFunctionFromName = pfnFunctionFromName;
   pengfuncsFromEngine->pfnNameForFunction = pfnNameForFunction;
   pengfuncsFromEngine->pfnClientPrintf = pfnClientPrintf;
   pengfuncsFromEngine->pfnServerPrint = pfnServerPrint;
   pengfuncsFromEngine->pfnCmd_Args = pfnCmd_Args;
   pengfuncsFromEngine->pfnCmd_Argv = pfnCmd_Argv;
   pengfuncsFromEngine->pfnCmd_Argc = pfnCmd_Argc;
   pengfuncsFromEngine->pfnGetAttachment = pfnGetAttachment;
   pengfuncsFromEngine->pfnCRC32_Init = pfnCRC32_Init;
   pengfuncsFromEngine->pfnCRC32_ProcessBuffer = pfnCRC32_ProcessBuffer;
   pengfuncsFromEngine->pfnCRC32_ProcessByte = pfnCRC32_ProcessByte;
   pengfuncsFromEngine->pfnCRC32_Final = pfnCRC32_Final;
   pengfuncsFromEngine->pfnRandomLong = pfnRandomLong;
   pengfuncsFromEngine->pfnRandomFloat = pfnRandomFloat;
   pengfuncsFromEngine->pfnSetView = pfnSetView;
   pengfuncsFromEngine->pfnTime = pfnTime;
   pengfuncsFromEngine->pfnCrosshairAngle = pfnCrosshairAngle;
   pengfuncsFromEngine->pfnLoadFileForMe = pfnLoadFileForMe;
   pengfuncsFromEngine->pfnFreeFile = pfnFreeFile;
   pengfuncsFromEngine->pfnEndSection = pfnEndSection;
   pengfuncsFromEngine->pfnCompareFileTime = pfnCompareFileTime;
   pengfuncsFromEngine->pfnGetGameDir = pfnGetGameDir;
   pengfuncsFromEngine->pfnCvar_RegisterVariable = pfnCvar_RegisterVariable;
   pengfuncsFromEngine->pfnFadeClientVolume = pfnFadeClientVolume;
   pengfuncsFromEngine->pfnSetClientMaxspeed = pfnSetClientMaxspeed;
   pengfuncsFromEngine->pfnCreateFakeClient = pfnCreateFakeClient;
   pengfuncsFromEngine->pfnRunPlayerMove = pfnRunPlayerMove;
   pengfuncsFromEngine->pfnNumberOfEntities = pfnNumberOfEntities;
   pengfuncsFromEngine->pfnGetInfoKeyBuffer = pfnGetInfoKeyBuffer;
   pengfuncsFromEngine->pfnInfoKeyValue = pfnInfoKeyValue;
   pengfuncsFromEngine->pfnSetKeyValue = pfnSetKeyValue;
   pengfuncsFromEngine->pfnSetClientKeyValue = pfnSetClientKeyValue;
   pengfuncsFromEngine->pfnIsMapValid = pfnIsMapValid;
   pengfuncsFromEngine->pfnStaticDecal = pfnStaticDecal;
   pengfuncsFromEngine->pfnPrecacheGeneric = pfnPrecacheGeneric;
   pengfuncsFromEngine->pfnGetPlayerUserId = pfnGetPlayerUserId;
   pengfuncsFromEngine->pfnBuildSoundMsg = pfnBuildSoundMsg;
   pengfuncsFromEngine->pfnIsDedicatedServer = pfnIsDedicatedServer;
   pengfuncsFromEngine->pfnCVarGetPointer = pfnCVarGetPointer;
   pengfuncsFromEngine->pfnGetPlayerWONId = pfnGetPlayerWONId;

   // SDK 2.0 additions...
   pengfuncsFromEngine->pfnInfo_RemoveKey = pfnInfo_RemoveKey;
   pengfuncsFromEngine->pfnGetPhysicsKeyValue = pfnGetPhysicsKeyValue;
   pengfuncsFromEngine->pfnSetPhysicsKeyValue = pfnSetPhysicsKeyValue;
   pengfuncsFromEngine->pfnGetPhysicsInfoString = pfnGetPhysicsInfoString;
   pengfuncsFromEngine->pfnPrecacheEvent = pfnPrecacheEvent;
   pengfuncsFromEngine->pfnPlaybackEvent = pfnPlaybackEvent;
   pengfuncsFromEngine->pfnSetFatPVS = pfnSetFatPVS;
   pengfuncsFromEngine->pfnSetFatPAS = pfnSetFatPAS;
   pengfuncsFromEngine->pfnCheckVisibility = pfnCheckVisibility;
   pengfuncsFromEngine->pfnDeltaSetField = pfnDeltaSetField;
   pengfuncsFromEngine->pfnDeltaUnsetField = pfnDeltaUnsetField;
   pengfuncsFromEngine->pfnDeltaAddEncoder = pfnDeltaAddEncoder;
   pengfuncsFromEngine->pfnGetCurrentPlayer = pfnGetCurrentPlayer;
   pengfuncsFromEngine->pfnCanSkipPlayer = pfnCanSkipPlayer;
   pengfuncsFromEngine->pfnDeltaFindField = pfnDeltaFindField;
   pengfuncsFromEngine->pfnDeltaSetFieldByIndex = pfnDeltaSetFieldByIndex;
   pengfuncsFromEngine->pfnDeltaUnsetFieldByIndex = pfnDeltaUnsetFieldByIndex;
   pengfuncsFromEngine->pfnSetGroupMask = pfnSetGroupMask;
   pengfuncsFromEngine->pfnCreateInstancedBaseline = pfnCreateInstancedBaseline;
   pengfuncsFromEngine->pfnCvar_DirectSet = pfnCvar_DirectSet;
   pengfuncsFromEngine->pfnForceUnmodified = pfnForceUnmodified;
   pengfuncsFromEngine->pfnGetPlayerStats = pfnGetPlayerStats;
   pengfuncsFromEngine->pfnAddServerCommand = pfnAddServerCommand;

   // hl1108
   pengfuncsFromEngine->pfnVoice_GetClientListening = pfnVoice_GetClientListening;
   pengfuncsFromEngine->pfnVoice_SetClientListening = pfnVoice_SetClientListening;

   // hl1109
   pengfuncsFromEngine->pfnGetPlayerAuthId = pfnGetPlayerAuthId;

#ifdef HLSDK_20031110
   // 2003/11/10
   pengfuncsFromEngine->pfnSequenceGet = pfnSequenceGet;
   pengfuncsFromEngine->pfnSequencePickSentence = pfnSequencePickSentence;
   pengfuncsFromEngine->pfnGetFileSize = pfnGetFileSize;
   pengfuncsFromEngine->pfnGetApproxWavePlayLen = pfnGetApproxWavePlayLen;
   pengfuncsFromEngine->pfnIsCareerMatch = pfnIsCareerMatch;
   pengfuncsFromEngine->pfnGetLocalizedStringLength = pfnGetLocalizedStringLength;
   pengfuncsFromEngine->pfnRegisterTutorMessageShown = pfnRegisterTutorMessageShown;
   pengfuncsFromEngine->pfnGetTimesTutorMessageShown = pfnGetTimesTutorMessageShown;
   pengfuncsFromEngine->pfnProcessTutorMessageDecayBuffer = pfnProcessTutorMessageDecayBuffer;
   pengfuncsFromEngine->pfnConstructTutorMessageDecayBuffer = pfnConstructTutorMessageDecayBuffer;
   pengfuncsFromEngine->pfnResetTutorMessageDecayData = pfnResetTutorMessageDecayData;
#endif /* HLSDK_20031110 */

   // give the engine functions to the other DLL...
   (*other_GiveFnptrsToDll)(pengfuncsFromEngine, pGlobals);
#endif /* not USE_METAMOD */
}
