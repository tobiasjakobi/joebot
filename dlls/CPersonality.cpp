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
#include <iostream.h>
#include <string.h>

#include "extdll.h"
#include "util.h"

#ifdef USE_METAMOD
#define SDK_UTIL_H  // util.h already included
#include "meta_api.h"
#endif /* USE_METAMOD */

#include "CPersonality.h"

#include "bot_modid.h"
#include "bot_weapons.h"
#include "globalvars.h"

char *szDefaultChatFile = "texts.txt";

CPersonality :: CPersonality(){
	strcpy(szChatfile,szDefaultChatFile);
	*szPrefWeapon = 0;
	iSkill = -1;
	fAggressiveness = 0;
	bLoaded = false;
	iPrefTeam = -1;
	iPrefClass = -1;
	iSpeakLeet = -1;

	memset(iWeaponPriority,0,sizeof(int)*32);
	if(mod_id == CSTRIKE_DLL){
		//iWeaponPriority[CS_WEAPON_P228] += 100;

		iWeaponPriority[CS_WEAPON_ELITE] += 1;
		iWeaponPriority[CS_WEAPON_M4A1] += 2;
		iWeaponPriority[CS_WEAPON_AK47] += 1;
		iWeaponPriority[CS_WEAPON_AWP] += 3;
		iWeaponPriority[CS_WEAPON_M3] -= 1;
		iWeaponPriority[CS_WEAPON_M249] += 2;
	}
	short sschl;
	for(sschl = 0;sschl < 32;sschl++){
		iWeaponPriority[sschl] += RANDOM_LONG(-1,1);
	}
	/*FILE *fhd;
	fhd=fopen("log.txt","a");
	for(sschl = 0;sschl < 32;sschl++){
		fprintf(fhd,"%i\t%i\n",int(sschl),iWeaponPriority[sschl]);
	}
	fprintf(fhd,"\n--------------------------\n");
	fclose(fhd);*/
}

CPersonality :: ~CPersonality(){
	/*FILE *fhd;
	fhd=fopen("log.txt","a");
	short sschl;
	for(sschl = 0;sschl < 32;sschl++){
		fprintf(fhd,"%i\t%i\n",int(sschl),iWeaponPriority[sschl]);
	}
	fprintf(fhd,"\n--------------------------\n");
	fclose(fhd);*/
}

void CPersonality :: Load(char *szName){
	char szWFilename[256];
	char *szFile,*szAct;
	FILE *fhd;
	
	UTIL_BuildFileName(szWFilename, sizeof(szWFilename), "joebot/per/%s.per", szName);
	
	if(!(fhd = fopen(szWFilename,"r"))){
		UTIL_BuildFileName(szWFilename, sizeof(szWFilename), "joebot/per/default.per");
	}
	else{
		fclose(fhd);
	}
	//LOG_MESSAGE(PLID, "Loading personality file for %s : %s", szName, szWFilename);
	
	long lSize;
	
	lSize = GetFileSize(szWFilename);
	if(!(fhd = fopen(szWFilename,"r")))
		return;
	
	szFile = new char[lSize+1];
	memset(szFile,0,sizeof(char)*(lSize+1));
	
	//fread(szFile,sizeof(char),lSize,fhd);
	char *p = szFile;
	while (!feof(fhd)){
		*p = fgetc(fhd);
		p ++;
	}
	
	Preprocess(szFile);		// remove uninteresting stuff

	// process content

	if(szAct = strstr(szFile,"chatfile")){
		sscanf(szAct,"chatfile %s",szChatfile);
	}
	if(szAct = strstr(szFile,"skill")){
		sscanf(szAct,"skill %i",&iSkill);
	}
	if(szAct = strstr(szFile,"aggressiveness")){
		sscanf(szAct,"aggressiveness %f",&fAggressiveness);
	}
	if(szAct = strstr(szFile,"prefweapon")){
		sscanf(szAct,"prefweapon %s",szPrefWeapon);
	}
	if(szAct = strstr(szFile,"prefclass")){
		sscanf(szAct,"prefclass %i",&iPrefClass);
	}
	if(szAct = strstr(szFile,"prefteam")){
		sscanf(szAct,"prefteam %i",&iPrefTeam);
	}
	if(szAct = strstr(szFile,"speakleet")){
		sscanf(szAct,"speakleet %i",&iSpeakLeet);
	}

	delete [] szFile;
	bLoaded = true;

	fclose(fhd);
}

void CPersonality :: Save(char *szName){
	char szWFilename[256];
	//	char *szFile,*szAct;
	FILE *fhd;
	
	UTIL_BuildFileName(szWFilename, sizeof(szWFilename), "joebot/per/%s.per", szName);
	
	LOG_MESSAGE(PLID, "Saving personality file for %s : %s", szName, szWFilename);
	
	if(!(fhd = fopen(szWFilename,"w")))
		return;
	
	fprintf(fhd,"############################\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# JoeBOT personality file\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"############################\n");
	fprintf(fhd,"\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# what's the chatfile the bot should use ?\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"\n");
	fprintf(fhd,"chatfile %s\n",szChatfile);
	fprintf(fhd,"\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# What should the skill of this bot should be ? ( -1 means no information -> the given settings from bot.cfg will be used )\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"\n");
	fprintf(fhd,"skill %i\n",iSkill);
	fprintf(fhd,"\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# aggressiveness; -1 defensive | 0 - normal | 1 aggressive\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"\n");
	fprintf(fhd,"aggressiveness %f\n",fAggressiveness);
	fprintf(fhd,"\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# prefweapon -> preferred weapon.\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# team dependant weapons which cannot be bought, are converted to a \n");
	fprintf(fhd,"# similar weapon of the own team.\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# CS_WEAPON_FAMAS <--> CS_WEAPON_GALIL;\n");
	fprintf(fhd,"# CS_WEAPON_M4A1  <--> CS_WEAPON_AK47;\n");
	fprintf(fhd,"# CS_WEAPON_TMP   <--> CS_WEAPON_MAC10;\n");
	fprintf(fhd,"# CS_WEAPON_SG550 <--> CS_WEAPON_G3SG1;\n");
	fprintf(fhd,"# CS_WEAPON_AUG   <--> CS_WEAPON_SG552;\n");
	fprintf(fhd,"# \n");
	fprintf(fhd,"# weapon names are :\n");
	fprintf(fhd,"# \n");
	fprintf(fhd,"# weapon_scout\n");
	fprintf(fhd,"# weapon_xm1014\n");
	fprintf(fhd,"# weapon_mac10\n");
	fprintf(fhd,"# weapon_aug\n");
	fprintf(fhd,"# weapon_ump45\n");
	fprintf(fhd,"# weapon_sg550\n");
	fprintf(fhd,"# weapon_galil\n");
	fprintf(fhd,"# weapon_famas\n");
	fprintf(fhd,"# weapon_awp\n");
	fprintf(fhd,"# weapon_mp5navy\n");
	fprintf(fhd,"# weapon_m249\n");
	fprintf(fhd,"# weapon_m3\n");
	fprintf(fhd,"# weapon_m4a1\n");
	fprintf(fhd,"# weapon_tmp\n");
	fprintf(fhd,"# weapon_g3sg1\n");
	fprintf(fhd,"# weapon_sg552\n");
	fprintf(fhd,"# weapon_ak47\n");
	fprintf(fhd,"# weapon_p90\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"\n");
	fprintf(fhd,"prefweapon %s\n",szPrefWeapon);
	fprintf(fhd,"\n");
	fprintf(fhd,"\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# prefteam -> preferred team\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# CS : 1 -> TE\n");
	fprintf(fhd,"#      2 -> CT\n");
	fprintf(fhd,"#      5 -> random\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# DoD : 1 -> Axis\n");
	fprintf(fhd,"#       2 -> Allies\n");
	fprintf(fhd,"#       5 -> random\n");
	fprintf(fhd,"\n");
	fprintf(fhd,"prefteam %i\n",iPrefTeam);
	fprintf(fhd,"\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# prefclass -> preferred class\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# cs -> models\n");
	fprintf(fhd,"# dod -> classes\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# -1 means no information ( -> then more or less random )\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"\n");
	fprintf(fhd,"prefclass %i\n",iPrefClass);
	fprintf(fhd,"\n");
	fprintf(fhd,"\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# speakleet -> should the bot speak 1337 ?\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"# 0  -> no\n");
	fprintf(fhd,"# 1  -> yes\n");
	fprintf(fhd,"# -1 -> apply normal 1337 possibility setting from bot.cfg\n");
	fprintf(fhd,"#\n");
	fprintf(fhd,"\n");
	fprintf(fhd,"speakleet %i\n",iSpeakLeet);
	fprintf(fhd,"\n");
	fclose(fhd);
}
