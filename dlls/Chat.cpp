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

#include "Chat.h"

#include "bot_modid.h"
#include "globalvars.h"

char szTags[_MAX_TAGS][15] = {"[bombplanted]","[hostdown]","[kill]","[dead]","[teamattack]","[welcome]"};
char szReplyTag[] = "[reply]\0";
char szKeyTag[] = "@KEY\0";
char szTeams[3][15] = {"<ct>","<te>","<all>"};
char	szBOTNAME[]			="BOTNAME",
		szTEAMMATEALIVE[]	="TEAMMATEALIVE",
		szENEMYALIVE[]		="ENEMYALIVE",
		szDEAD[]			="DEAD",
		szALIVE[]			="ALIVE",
		szVICTIM[]			="VICTIM",
		szMAP[]				="MAP",
		szMOD[]				="MOD",
		szRANDOM[]			="RANDOM",
		szWEAPON[]			="WEAPON",
		szTKBOT[]			="TKPLAYER",
		szTIMERE[]			="TIMERE",
		szBESTP[]			="BESTPLAYER",
		szLASTJOINED[]		="LASTJOINED",
		szCHATTER[]			="CHATTER";

edict_t *GetRandomPlayer(edict_t *pNot,int iTeam,int iAlive);

CChatIElem :: CChatIElem(){
	prev = next = 0;
	pKeys = 0;
	pTexts = 0;
}

CChatIElem :: ~CChatIElem(){
	if(pKeys){
		while(pKeys->next)
			delete pKeys->next;
		delete pKeys;
		pKeys = 0;
	}

	if(pTexts){
		while(pTexts->next)
			delete pTexts->next;
		delete pTexts;
		pTexts = 0;
	}

	if(next)
		next->prev = prev;
	if(prev)
		prev->next = next;
}

bool CChatIElem :: Cmp2Key(const char *szPKey){
	// check all keys of this class if they are equal to szPKey
	CChatIKeyElem *p;
	
	p = pKeys;
	while(p){
		//cout << p->szKey<<endl;
		if(strstr(szPKey,p->szKey))
			return true;
		p = p->next;
	}
	return false;
}

CChatIKeyElem *CChatIElem :: AddKey(const char *szPKey){
	// adds a key to this class
	if(strlen(szPKey)>MAXL_KEY)
		return 0;
	CChatIKeyElem *p,*pp;
	
	p = pKeys;
	if(p){
		while(p){
			pp = p;
			p = p->next;
		}
		p=pp;
		p->next = new CChatIKeyElem;
		strcpy(p->next->szKey,szPKey);
		p->next->prev = p;
		p = p->next;
	}
	else{
		pKeys = p = new CChatIKeyElem;
		strcpy(p->szKey,szPKey);
	}
	// make it lowercase;
	UTIL_strlwr(p->szKey);
	return p;
}

CChatITextElem *CChatIElem :: AddText(const char *szPText){
	// adds a text item to this class
	if(strlen(szPText)>MAXL_TEXT)
		return 0;
	CChatITextElem *p,*pp;
	
	p = pTexts;
	if(p){
		while(p){
			pp = p;
			p = p->next;
		}
		p = pp;
		p->next = new CChatITextElem;
		strcpy(p->next->szText,szPText);
		p->next->prev = p;
		p = p->next;
	}
	else{
		pTexts = p = new CChatITextElem;
		strcpy(p->szText,szPText);
	}
	return p;
}

CChatITextElem *CChatIElem :: GetRandomText(void){
	// returns a random text from an interactive category
	long lNum = 0,lRandom;
	CChatITextElem *p;
	
	p = pTexts;
	while(p){
		lNum++;
		p = p->next;
	}
	lRandom = RANDOM_LONG(0,lNum-1);
	
	p = pTexts;
	lNum = 0;
	while(p){
		if(lNum == lRandom)
			return p;
		lNum++;
		p = p->next;
	}
	return pTexts;
}

CChatIKeyElem :: CChatIKeyElem(){
	prev = next = 0;
	*szKey = 0;
}

CChatIKeyElem :: ~CChatIKeyElem(){
	if(next)
		next->prev = prev;
	if(prev)
		prev->next = next;
}

CChatITextElem :: CChatITextElem(){
	prev = next = 0;
	fTime = 0;
	*szText = 0;
}

CChatITextElem :: ~CChatITextElem(){
	if(next)
		next->prev = prev;
	if(prev)
		prev->next = next;
}

CChat :: CChat (){
	//memset(szTexts,0,3*_MAX_TAGS*_MAX_SENTENCES_PER_PART*80*sizeof(char));
	InitChat();
	bNeed2Init = true;
	pIChatData = 0;
}

CChat :: ~CChat (){
	if(pIChatData){
		while(pIChatData->next)
			delete pIChatData->next;
		delete pIChatData;
		pIChatData = 0;
	}
}

char *CChat :: Associate(const char *szP){
	// see if there's a key according to the text szP
	const long _MAXAP = 10;
	CChatIElem *p;
	char *szReturn;
	char *szPoss[_MAXAP];
	float *pfTime[_MAXAP];
	long lNum = 0;
	long lChoice;

	szReturn = 0;
	CChatITextElem *pCITE;

	memset(szPoss,0,sizeof(char *)*_MAXAP);
	
	p = pIChatData;
	while(p){
		if(p->Cmp2Key(szP)){
			// a key was right :)
			
			// now get a text from there :)
			if(lNum < _MAXAP ){
				pCITE = p->GetRandomText();
				szPoss[lNum] = pCITE->szText;
				pfTime[lNum] = &(pCITE->fTime);
				lNum++;
			}
			else{
				pCITE = p->GetRandomText();
				lChoice = RANDOM_LONG(0,_MAXAP-1);
				szPoss[lChoice] = pCITE->szText;
				pfTime[lChoice] = &(pCITE->fTime);
			}
		}
		p = p->next;
	}
	if(lNum){
		lChoice = RANDOM_LONG(0,lNum-1);
		if(!TEq(*pfTime[lChoice],gpGlobals->time,_SCHATTIME)){
			szReturn = szPoss[lChoice];
			if(pfTime[lChoice])
				*pfTime[lChoice] = gpGlobals->time;
		}
	}
	return szReturn;
}

CChatIElem *CChat :: AddIElem(void){
	// adding related interactive chat elem
	CChatIElem *p,*pp;
	
	p = pIChatData;
	if(p){
		while(p){
			pp = p;
			p = p->next;
		}
		p = pp;
		p->next = new CChatIElem;
		p->next->prev = p;
		p = p->next;
	}
	else{
		pIChatData = p = new CChatIElem;
	}
	return p;
}

int CChat :: ProcessChatString(CBotBase *pBBot,char *szChatText){
	// this function replaces strings in the chat text with something else
	char szTemp[100]="\0",*szFound;
	char szName[100];
	bool bLoopAgain;
	edict_t *pEnt=0;
	edict_t *pEdict = pBBot->pEdict;
	
	do{
		bLoopAgain = false;
		if(szFound=strstr(szChatText,szBOTNAME)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			strcpy(szName,STRING(pEdict->v.netname));
			if(RANDOM_LONG(0,100) <50)
				ConvertName(szName);
			strcat(szTemp,szName);
			strcat(szTemp,szFound + strlen(szBOTNAME));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szTEAMMATEALIVE)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get name of a teammate 
			pEnt = GetRandomPlayer(pEdict,UTIL_GetTeam(pEdict),1);
			if(pEnt&&UTIL_ClientIndex(pEnt)!=-1){
				strcpy(szName,STRING(pEnt->v.netname));
				ConvertName(szName);
				strcat(szTemp,szName);
			}
			else{
				*szChatText=0;
				return 0;
			}
			strcat(szTemp,szFound + strlen(szTEAMMATEALIVE));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szENEMYALIVE)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get name of an enemy
			pEnt = GetRandomPlayer(pEdict,!(UTIL_GetTeam(pEdict)),1);
			if(pEnt&&UTIL_ClientIndex(pEnt)!=-1){
				strcpy(szName,STRING(pEnt->v.netname));
				ConvertName(szName);
				strcat(szTemp,szName);
			}
			else{
				*szChatText=0;
				return 0;
			}
			strcat(szTemp,szFound + strlen(szENEMYALIVE));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szALIVE)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get name of an enemy
			pEnt = GetRandomPlayer(pEdict,-1,1);
			if(pEnt&&UTIL_ClientIndex(pEnt)!=-1){
				strcpy(szName,STRING(pEnt->v.netname));
				ConvertName(szName);
				strcat(szTemp,szName);
			}
			else{
				*szChatText=0;
				return 0;
			}
			strcat(szTemp,szFound + strlen(szALIVE));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szLASTJOINED)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get name of an enemy
			pEnt = pEdictLastJoined;
			if(pEnt&&UTIL_ClientIndex(pEnt)!=-1){
				strcpy(szName,STRING(pEnt->v.netname));
				ConvertName(szName);
				strcat(szTemp,szName);
			}
			else{
				*szChatText=0;
				return 0;
			}
			strcat(szTemp,szFound + strlen(szLASTJOINED));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szDEAD)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get name of a dead
			pEnt = GetRandomPlayer(pEdict,-1,0);
			if(!FNullEnt(pEnt)
				&&UTIL_ClientIndex(pEnt)!=-1){
				strcpy(szName,STRING(pEnt->v.netname));
				ConvertName(szName);
				strcat(szTemp,szName);
			}
			else{
				*szChatText=0;
				return 0;
			}
			strcat(szTemp,szFound + strlen(szDEAD));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szVICTIM)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get name of the last victim
			pEnt = pBBot->pELVictim;
			if(pEnt&&UTIL_ClientIndex(pEnt)!=-1){
				strcpy(szName,STRING(pEnt->v.netname));
				ConvertName(szName);
				strcat(szTemp,szName);
			}
			else{
				*szChatText=0;
				return 0;
			}
			strcat(szTemp,szFound + strlen(szVICTIM));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szCHATTER)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get name of the last chatter
			if(IsAlive(pBBot->pEdict))
				pEnt = INDEXENT(iLastSayClient);
			else
				pEnt = INDEXENT(iDeadLastSayClient);
			if(pEnt&&UTIL_ClientIndex(pEnt)!=-1){
				strcpy(szName,STRING(pEnt->v.netname));
				ConvertName(szName);
				strcat(szTemp,szName);
			}
			else{
				*szChatText=0;
				return 0;
			}
			strcat(szTemp,szFound + strlen(szCHATTER));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szTIMERE)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get time to end of round
			char szTime[80];
			snprintf(szTime,sizeof(szTime),"%i seconds",int(CVAR_GET_FLOAT("mp_roundtime")*60.0 - g_fRoundTime));
			strcat(szTemp,szTime);
			strcat(szTemp,szFound + strlen(szTIMERE));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szMAP)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get name of map
			strcat(szTemp,STRING(gpGlobals->mapname));
			strcat(szTemp,szFound + strlen(szMAP));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szMOD)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get name of mod
			if(mod_id == CSTRIKE_DLL){
				strcat(szTemp,"cstrike");
			}
			else if(mod_id == DOD_DLL){
				strcat(szTemp,"dod");
			}
			else{
			}

			strcat(szTemp,szFound + strlen(szMAP));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szBESTP)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get name of the last chatter
			pEnt = UTIL_BestPlayer();
			if(pEnt&&UTIL_ClientIndex(pEnt)!=-1){
				strcpy(szName,STRING(pEnt->v.netname));
				ConvertName(szName);
				strcat(szTemp,szName);
			}
			else{
				*szChatText=0;
				return 0;
			}
			strcat(szTemp,szFound + strlen(szBESTP));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szWEAPON)){
			int iWeapon = pBBot->WeaponModel2ID(STRING(pBBot->pEdict->v.weaponmodel));
			char *szWeaponDef;

			if(iWeapon == -1){
				*szChatText=0;
				return 0;
			}

			szWeaponDef = weapon_defs[iWeapon].szClassname;

			if(!*szWeaponDef){
				*szChatText=0;
				return 0;
			}

			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get name of map
			char szWeapon[30];
			strcpy(szWeapon,szWeaponDef);
			strcat(szTemp,szWeapon+strlen("weapon_")*sizeof(char));
			strcat(szTemp,szFound + strlen(szWEAPON));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szRANDOM)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			// get name of a player
			pEnt = GetRandomPlayer(pEdict,-1,-1);
			if(pEnt&&UTIL_ClientIndex(pEnt)!=-1){
				strcpy(szName,STRING(pEnt->v.netname));
				ConvertName(szName);
				strcat(szTemp,szName);
			}
			else{
				*szChatText=0;
				return 0;
			}
			strcat(szTemp,szFound + strlen(szRANDOM));
			strcpy(szChatText,szTemp);
		}
		else if(szFound=strstr(szChatText,szTKBOT)){
			bLoopAgain = true;
			*szTemp=0;
			strncat(szTemp,szChatText,szFound-szChatText);
			if(pBBot->bot_IRadioM.pECalling&&UTIL_ClientIndex(pBBot->bot_IRadioM.pECalling)!=-1){
				strcpy(szName,STRING(pBBot->bot_IRadioM.pECalling->v.netname));
				ConvertName(szName);
				strcat(szTemp,szName);
			}
			else{
				*szChatText=0;
				return 0;
			}
			strcat(szTemp,szFound + strlen(szTKBOT));
			strcpy(szChatText,szTemp);
		}
	}while(bLoopAgain);
	
	return 1;
}

void CChat :: InitChat (void){
	if(bNeed2Init){
		long lTeam;
		long lTag;
		int iSentence;
		for(lTeam=0;lTeam < 3;lTeam++){
			for(lTag=0;lTag < _MAX_TAGS;lTag++){
				lCounter[lTeam][lTag] = 0;
				for(iSentence=0;iSentence < _MAX_SENTENCES_PER_PART;iSentence ++){
					szTexts[lTeam][lTag][iSentence].szText[0] = 0;
					fTime[lTeam][lTag][iSentence] = 0;
				}
			}
		}
		bNeed2Init = false;
	}
}

int CChat :: LoadFile(const char *szFileNameP){
	//return false;
	//cout << "CChat :: LoadFile loading " << szFileNameP << endl;
	char szFileName[80];
	char *szReplies;		// pointer to start of replies
	strcpy(szFileName,szFileNameP);
	long lSize;
	FILE *fhd;
	char *szFile;
	
	InitChat();
	
	lSize = GetFileSize(szFileName);
	if(!(fhd = fopen(szFileName,"r"))){
		return false;
	}
	
	szFile = new char[lSize+2];
	memset(szFile,0,sizeof(char)*(lSize+2));
	
	//fread(szFile,sizeof(char),lSize,fhd);

	char *p = szFile;
	while (!feof(fhd)){
		*p = fgetc(fhd);
		p ++;
	}

	fclose(fhd);
	fhd = 0;
	
	Preprocess(szFile);		// remove uninteresting stuff
	/*cout << szFile << endl;
	Sleep (10000);
	return false;*/
	
	// process content
	char *szAct,*szPa;
	char szAdd[300];
	long lTag,lTeam;
	long iCCount = 0,iKCount = 0;
	
	szReplies = strstr(szFile,szReplyTag);
	
	for(lTag=0;lTag < _MAX_TAGS;lTag++){
		szAct = szFile;
		szAct = strstr(szFile,szTags[lTag]);	// go to start of tag
		if(szAct){
			szAct += strlen(szTags[lTag]);
			
			while(!IsInstr(*szAct) && *szAct)
				szAct++;
			
			szPa = szAct;
			//cout << szAct << endl;
			
			for(lTeam=0;lTeam < 3;lTeam++){
				szAct = szPa;
				//cout <<"---"<< szAct << endl;
				szAct = strstr(szAct,szTeams[lTeam]);	// go to start of tag
				if(szAct){
					szAct += strlen(szTeams[lTeam]);
					
					while(!IsInstr(*szAct) && *szAct)
						szAct++;
					
					while((*szAct != '<') && (*szAct != '[')){
						if (!strchr(szAct,'\n'))
							break;
						if(szAct > szReplies && szReplies > 0)
							break;
						memset(szAdd,0,sizeof(char)*300);
						strncpy(szAdd,szAct,sizeof(char) * (strchr(szAct,'\n') - szAct));
						if(strlen(szAdd) < 80){
							strcpy(szTexts[lTeam][lTag][lCounter[lTeam][lTag]].szText,szAdd);
							//cout << "-+- " << szAdd << endl;
							lCounter[lTeam][lTag] ++;
							iCCount ++;
						}
						szAct = strchr(szAct,'\n');
						while(!IsInstr(*szAct) && *szAct)
							szAct++;
					}
				}
				else
					LOG_MESSAGE(PLID, "%s:%s does not exists", szTags[lTag], szTeams[lTeam]);
			}
		}
	}

	//process replies :)
	if(szReplies){
		char *szRCatS,
			*szRCatE,
			*szELine,
			*szComma,
			*szEnd,
			szKey[MAXL_KEY];
		CChatIElem *pIElem;
		
		szRCatE = szRCatS = strstr(szReplies,szKeyTag);
		
		//cout << "starting interactive data loading" << endl;
		
		while(szRCatS = szRCatE){		// start of key category
			szRCatE = strstr(szRCatS+sizeof(char),szKeyTag);			// end of key category
			
			pIElem = AddIElem();
			
			szAct = szRCatS + strlen(szKeyTag) * sizeof(char);		// skip keytag
			/*for(lschl = strlen(szKeyTag);lschl;lschl--){
				szAct++;
			}*/
			
			szELine = strstr(szAct,"\n");
#ifdef _DEBUG
			cout << "Keys : ";cout.flush();
#endif
			while(szComma = strstr(szAct,",")){
				if(szComma > szELine){
					szComma = szAct;
					break;
				}
				/*while(!IsInstr(*szAct) && *szAct)		// goto text
					szAct++;*/
				szAct = strchr(szAct,'\"');
				szAct++;		// do not take the "
				szEnd = strchr(szAct,'\"');

				if(szAct > szELine
					|| szEnd > szELine){
					szComma++;
					szAct = szComma;
					continue;
				}
				
				if((szEnd-szAct)/sizeof(char) > MAXL_KEY){
					LOG_MESSAGE(PLID, "Maximum key length is %d", MAXL_KEY);
				}
				else{
					memset(szKey,0,sizeof(char)*MAXL_KEY);
					strncpy(szKey,szAct,szEnd-szAct);
					
					pIElem->AddKey(szKey);
					iKCount ++;
#ifdef _DEBUG
					cout <<"\""<< szKey << "\"|";
#endif
				}
				szComma++;
				szAct = szComma;
			}
			while(!IsInstr(*szAct) && *szAct)		// goto text
				szAct++;
			
			memset(szKey,0,sizeof(char)*20);
			strncpy(szKey,szAct,szELine-szAct);
			
			pIElem->AddKey(szKey);
			
#ifdef _DEBUG
			cout << szKey << "\n";
#endif
			
			szAct = szELine;
			
			while(*szAct
				&& (*szAct != '<')
				&& (*szAct != '[')
				&& (*szAct != '@')){
				if(szRCatE)
					if(szAct > szRCatE-1)
						break;
					if (!strchr(szAct,'\n'))
						break;
					while(!IsInstr(*szAct) && *szAct)		// goto text
						szAct++;
					
					memset(szAdd,0,sizeof(char)*299);
					strncpy(szAdd,szAct,sizeof(char) * (strchr(szAct,'\n') - szAct));
					pIElem->AddText(szAdd);
					iCCount ++;
#ifdef _DEBUG
					cout << " - "<<szAdd << endl;
#endif
					szAct = strchr(szAct,'\n');
					while(!IsInstr(*szAct) && *szAct)
						szAct++;
			}
			
			if(!pIElem->pTexts){
				pIElem->AddText("There is a reply in the chat file with keys, but without texts");
				LOG_MESSAGE(PLID, "There is a reply in the chat file with keys, but without texts");
			}
		}
	}

	LOG_MESSAGE(PLID, "Loading chat file: %s; %d items/%d Keys.", szFileName, iCCount, iKCount);
	delete [] szFile;
	bNeed2Init = false;
	
	return true;
}

bool CChat :: to1337(char *szText){
	char szOrig[1000];
	int iSearch;
	strcpy(szOrig,szText);
	//strlwr(szOrig);
	UTIL_strlwr(szOrig);
	*szText = 0;
	char *szOrigP;
	
	char szConvert[][2][10]={
		{"four","4"},
		{"for","4"},
		{"two","2"},
		{"to","2"},
		{"too","2"},
		{"three","3"},
		{"five","5"},
		{"six","6"},
		{"seven","7"},
		{"eight","8"},
		{"nine","9"},
		{"ten","10"},
		{"you","j00"},
		{"ate","8"},
		{"l","1"},
		{"e","3"},
		{"t","7"},
		{"i","|"},
		{"o","0"},
		{"c","("},
		{"a","4"},
		{"g","9"},
		//{"k","|<"},
		//{"m","/\\/\\"},
		{"n","/\\/"},
		{"s","$"},
		//{"v","\\/"},
		//{"w","\\/\\/"},
		{"x",")("},
		//{"pi","3.1415"}, // :D
		{"\0","\0"}};
		
	szOrigP = szOrig;
	
	while(*szOrigP){
		iSearch = 0;
		while(*szConvert[iSearch][0]){
			if(!strncmp(szOrigP,szConvert[iSearch][0],sizeof(char)*strlen(szConvert[iSearch][0]))){
				strcat(szText,szConvert[iSearch][1]);
				szOrigP += sizeof(char)*strlen(szConvert[iSearch][0]);
				break;
			}
			iSearch++;
		}
		if(!*szConvert[iSearch][0]){
			strncat(szText,szOrigP,sizeof(char)*1);
			szOrigP++;
		}
	}
	
	return true;
}

void CChat :: ConvertName(char *szChangP){
	//return;
	//cout << szChangP << " - ";
	char szChanged[100],szTemp[100],*szB,*szE,*szEnd;
	long lAnz;
	
	if(strlen(szChangP) < 3){
		return;
	}
	
	// copy
	strcpy(szChanged,szChangP);
	// make lowercase
	UTIL_strlwr(szChanged);
	
	//cout << szChanged << endl;
	strcpy(szTemp,szChanged);
	szEnd  = szChanged + sizeof(char) * strlen(szChanged);
	// cut out (...) stuff
	if(szB=strchr(szChanged,'(')){
		if(szB < szEnd){
			//cout <<"szb : "<<szB<<endl;
			if(szE=strchr(szChanged,')')){
				//cout <<"sze : "<<szE<<endl;
				if(szE < szEnd){
					if(szE && szB){
						if(szE>szB){
							strcpy(szTemp,szChanged);
							strcpy(szTemp + (szB - szChanged)*sizeof(char),szE+1);
							
						}
						else{
							strcpy(szTemp,szChanged);
							strcpy(szTemp + (szE - szChanged)*sizeof(char),szB+1);
						}
					}
				}
			}
		}
	}
	
	if(strlen(szTemp) > 2)
		strcpy(szChanged,szTemp);
	
	strcpy(szTemp,szChanged);
	szEnd  = szChanged + sizeof(char) * strlen(szChanged);
	// cut out [...] stuff
	if(szB=strchr(szChanged,'[')){
		//cout <<"szb : "<<szB<<endl;
		if(szB < szEnd){
			if(szE=strchr(szChanged,']')){
				if(szE < szEnd){
					//cout <<"sze : "<<szE<<endl;
					if(szE && szB){
						if(szE>szB){
							strcpy(szTemp,szChanged);
							strcpy(szTemp + (szB - szChanged)*sizeof(char),szE+1);
						}
						else{
							strcpy(szTemp,szChanged);
							strcpy(szTemp + (szE - szChanged)*sizeof(char),szB+1);
						}
					}
				}
			}
		}
	}
	if(strlen(szTemp) > 2)
		strcpy(szChanged,szTemp);
	
	strcpy(szTemp,szChanged);
	szEnd  = szChanged + sizeof(char) * strlen(szChanged);
	// cut out {...} stuff
	if(szB=strchr(szChanged,'{')){
		//cout <<"szb : "<<szB<<endl;
		if(szB < szEnd){
			if(szE=strchr(szChanged,'}')){
				if(szE < szEnd){
					//cout <<"sze : "<<szE<<endl;
					if(szE && szB){
						if(szE>szB){
							strcpy(szTemp,szChanged);
							strcpy(szTemp + (szB - szChanged),szE+1);
						}
						else{
							strcpy(szTemp,szChanged);
							strcpy(szTemp + (szE - szChanged),szB+1);
						}
					}
				}
			}
		}
	}
	if(strlen(szTemp) > 2)
		strcpy(szChanged,szTemp);
	// just forget one letter .... sometimes
	
	lAnz = strlen(szChanged);
	if(lAnz>4 && RANDOM_FLOAT(0,100) < 10){
		long lLose = long(RANDOM_LONG(1,lAnz-2))+1;
		strcpy ( szChanged+(lLose-1)*sizeof(char),szChanged+lLose*sizeof(char));
	}
	
	// copy back
	strcpy(szChangP,szChanged);
	//cout << szChangP << endl;
}
