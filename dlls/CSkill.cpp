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

#include <iostream.h>

#include "extdll.h"
#include "util.h"

#ifdef USE_METAMOD
#define SDK_UTIL_H  // util.h already included
#include "meta_api.h"
#endif /* USE_METAMOD */

#include "CSkill.h"

CSkill :: CSkill(){
	iCount=0;
}

CSkill :: ~CSkill(){
}

void CSkill :: Load(void){
	char *szFileContent,*szAct,*szRead,*szNext;
	long lFileSize;
	FILE *fhd;

	char szFileName[80];
	UTIL_BuildFileName(szFileName, sizeof(szFileName), "joebot/bot_skill.txt");

	CSkillItem SITemp;
	
	if(lFileSize = GetFileSize(szFileName))
	{
		szFileContent = new char[lFileSize+2];
		memset(szFileContent,0,sizeof(char)*(lFileSize+2));
		
		if(fhd = fopen(szFileName,"r")){
			LOG_MESSAGE(PLID, "Loading skill file: %s", szFileName);
			
			//fread(szFileContent,sizeof(char),lFileSize,fhd);
			
			char *p = szFileContent;
			while (!feof(fhd)){
				*p = fgetc(fhd);
				p ++;
			}
			
			fclose(fhd);
		
			Preprocess(szFileContent);		// remove uninteresting stuff
			
			szAct = szFileContent;

			//cout << szAct << endl;
			while(szAct = strstr(szAct,"SKILL")){
				SITemp.fReact = 0;
				SITemp.fVSC = 0;
				SITemp.fVSM = 0;
				SITemp.iSkill = 0;
				SITemp.fITurnProb = 0;
				SITemp.fInDistance = 10000;
				SITemp.fProneInDistance = 10000;
				SITemp.fProneTPD = 0;
				SITemp.fTPD = 0;
				SITemp.fShootThru = 0;
				SITemp.fMaxRecoil = 20;

				szNext = strstr(szAct,"SKILL");
				sscanf(szAct,"SKILL %i",&SITemp.iSkill);
				
				szRead = szAct;
				szRead = strstr(szRead,"VSC");
				if(szRead){
					sscanf(szRead,"VSC %f",&SITemp.fVSC);
				}

				szRead = szAct;
				szRead = strstr(szRead,"VSM");
				if(szRead){
					sscanf(szRead,"VSM %f",&SITemp.fVSM);
				}

				szRead = szAct;
				szRead = strstr(szRead,"REA");
				if(szRead){
					sscanf(szRead,"REA %f",&SITemp.fReact);
				}

				szRead = szAct;
				szRead = strstr(szRead,"STH");
				if(szRead){
					sscanf(szRead,"STH %f",&SITemp.fShootThru);
				}

				szRead = szAct;
				szRead = strstr(szRead,"STH");
				if(szRead){
					sscanf(szRead,"STH %f",&SITemp.fShootThru);
				}

				szRead = szAct;
				szRead = strstr(szRead,"ITP");
				if(szRead){
					sscanf(szRead,"ITP %f",&SITemp.fITurnProb);
				}

				szRead = szAct;
				szRead = strstr(szRead,"DIS");
				if(szRead){
					sscanf(szRead,"DIS %f",&SITemp.fInDistance);
				}

				szRead = szAct;
				szRead = strstr(szRead,"TPD");
				if(szRead){
					sscanf(szRead,"TPD %f",&SITemp.fTPD);
				}

				szRead = szAct;
				szRead = strstr(szRead,"PDI");
				if(szRead){
					sscanf(szRead,"PDI %f",&SITemp.fProneInDistance);
				}

				szRead = szAct;
				szRead = strstr(szRead,"PTD");
				if(szRead){
					sscanf(szRead,"PTD %f",&SITemp.fProneTPD);
				}

				szRead = szAct;
				szRead = strstr(szRead,"REC");
				if(szRead){
					sscanf(szRead,"REC %f",&SITemp.fMaxRecoil);
				}

				szAct ++;
				
				// copy data to class
				SData[iCount] = SITemp;
				if(iCount < 9)
					iCount++;
				else
					LOG_MESSAGE(PLID, "WARNING: loading skill file (2nd stage) - Too many elements");
			}
		}
		else{
			LOG_MESSAGE(PLID, "ERROR: loading skill file (2nd stage): %s", szFileName);
		}
		delete [] szFileContent;
	}
	else{
		LOG_MESSAGE(PLID, "ERROR: loading skill file (1st stage): %s", szFileName);
	}
}

void CSkill :: GetSkill(int iSkill,CSkillItem &Copy2){
	int ischl;
	for(ischl=0;ischl < iCount;ischl++){
		if(SData[ischl].iSkill>iSkill){
			break;
		}
	}
	if(ischl!=0){	// interpolate
		Copy2.iSkill = iSkill;
		Copy2.fVSC = SData[ischl-1].fVSC + ((SData[ischl].fVSC-SData[ischl-1].fVSC)
			/float(SData[ischl].iSkill-SData[ischl-1].iSkill))
			*(iSkill-SData[ischl-1].iSkill);
		Copy2.fVSM = SData[ischl-1].fVSM + ((SData[ischl].fVSM-SData[ischl-1].fVSM)
			/float(SData[ischl].iSkill-SData[ischl-1].iSkill))
			*(iSkill-SData[ischl-1].iSkill);
		Copy2.fReact = SData[ischl-1].fReact + ((SData[ischl].fReact-SData[ischl-1].fReact)
			/float(SData[ischl].iSkill-SData[ischl-1].iSkill))
			*(iSkill-SData[ischl-1].iSkill);
		Copy2.fShootThru = SData[ischl-1].fShootThru + ((SData[ischl].fShootThru-SData[ischl-1].fShootThru)
			/float(SData[ischl].iSkill-SData[ischl-1].iSkill))
			*(iSkill-SData[ischl-1].iSkill);
		Copy2.fITurnProb = SData[ischl-1].fITurnProb + ((SData[ischl].fITurnProb-SData[ischl-1].fITurnProb)
			/float(SData[ischl].iSkill-SData[ischl-1].iSkill))
			*(iSkill-SData[ischl-1].iSkill);
		Copy2.fInDistance = SData[ischl-1].fInDistance + ((SData[ischl].fInDistance-SData[ischl-1].fInDistance)
			/float(SData[ischl].iSkill-SData[ischl-1].iSkill))
			*(iSkill-SData[ischl-1].iSkill);
		Copy2.fTPD = SData[ischl-1].fTPD + ((SData[ischl].fTPD-SData[ischl-1].fTPD)
			/float(SData[ischl].iSkill-SData[ischl-1].iSkill))
			*(iSkill-SData[ischl-1].iSkill);
		Copy2.fMaxRecoil = SData[ischl-1].fMaxRecoil + ((SData[ischl].fMaxRecoil-SData[ischl-1].fMaxRecoil)
			/float(SData[ischl].iSkill-SData[ischl-1].iSkill))
			*(iSkill-SData[ischl-1].iSkill);
	}
	else{			// means that it's at the end of the fuckin field
		Copy2 = SData[0];
	}
}
