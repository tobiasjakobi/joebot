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
#include <stdio.h>
#include <string.h>

#include "extdll.h"
#include "util.h"

#ifdef USE_METAMOD
#define SDK_UTIL_H  // util.h already included
#include "meta_api.h"
#endif /* USE_METAMOD */

#include "CSDecals.h"

#include "bot_modid.h"
#include "bot_globaldefs.h"

#define SPRAYDECALFILE "spray.txt"

CSDecals :: CSDecals (){
	Init();
}

CSDecals :: ~CSDecals (){
}

void CSDecals :: Init(void){
	lNum = 0;

	AddDecal("{biohaz");
	AddDecal("{graf001");
	AddDecal("{graf002");
	AddDecal("{graf003");
	AddDecal("{graf004");
	AddDecal("{graf005");
	AddDecal("{lambda06");
	AddDecal("{target");
	AddDecal("{hand1");
	AddDecal("{spit1");
	AddDecal("{bloodhand5");
	AddDecal("{bloodhand6");
	AddDecal("{foot_l");
	AddDecal("{c1a3_grn1");
	AddDecal("{c2a2_b");
	AddDecal("{crack1");
	AddDecal("{scorch1");
	AddDecal("{turn1a");
}

char * CSDecals ::GetRandomDecal(void){
	return szSDecals[RANDOM_LONG(0,lNum-1)];
}

void CSDecals :: AddDecal(const char *szP){
	strcpy(szSDecals[lNum],szP);
	lNum++;
}

void CSDecals :: Load(void){
	char *szFileContent,*szAct,*szTmp,szFileName[80];
	long lFileSize;
	//long lToReadLength;
	FILE *fhd;
	
	UTIL_BuildFileName(szFileName, sizeof(szFileName), "joebot/%s", SPRAYDECALFILE);
	
	if(lFileSize = GetFileSize(szFileName))
	{
		szFileContent = new char[lFileSize+2];
		memset(szFileContent,0,sizeof(char)*(lFileSize+2));
		
		if(fhd = fopen(szFileName,"r")){
			lNum = 0;
			LOG_MESSAGE(PLID, "Loading spray decal data from %s", szFileName);
			
			//fread(szFileContent,sizeof(char),lFileSize,fhd);
			char *p = szFileContent;
			while (!feof(fhd)){
				*p = fgetc(fhd);
				p ++;
			}
			
			fclose(fhd);
			Preprocess(szFileContent);		// remove uninteresting stuff
			//cout << szFileContent<<endl;
		
			szAct = szFileContent;
			
			while(*szAct && szAct < (szFileContent + lFileSize*sizeof(char))){	// stop if 0
				while(!IsInstr(*szAct) && *szAct)	// goto next text ...
					szAct++;

				szTmp = strstr(szAct,"\n");
				if(szTmp - szFileContent < lFileSize
					&& szTmp
					&& szTmp != szAct){
					strncpy(szSDecals[lNum],szAct,szTmp-szAct);
					//cout << szSDecals[lNum]<<endl;
					lNum++;

					szAct = szTmp;
				}
				else
					szAct++;
				
				while(!IsInstr(*szAct) && *szAct)	// goto next text ...
					szAct++;
			}
		}
		delete [] szFileContent;
	}
	else
		LOG_MESSAGE(PLID, "Spray decal data file %s couldn't be loaded. Using default spraypaints", szFileName);
}
