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

#include "dialog.h"

char szWeapons[][32]={
		"none",
		"weapon_scout",
		"weapon_xm1014",
		"weapon_mac10",
		"weapon_aug",
		"weapon_ump45",
		"weapon_sg550",
		"weapon_awp",
		"weapon_mp5navy",
		"weapon_m249",
		"weapon_m3",
		"weapon_m4a1",
		"weapon_tmp",
		"weapon_g3sg1",
		"weapon_sg552",
		"weapon_ak47",
		"weapon_p90",
		"\0"
};

#include "../dlls/CParser.h"

#define BN_MAXNAMES 50
#define BN_MAXNAMELENGTH 22

#define BN_PERS_DEF 1
#define BN_PERS_NOR 2
#define BN_PERS_AGG 3

//long WeaponClass2ID(const char *szClassname);		// in bot_combat.h

typedef struct{
	char szName[BN_MAXNAMELENGTH];
} Name;

class CBotNames : private CParser{
public:
	CBotNames();
	~CBotNames();
	
	int Init(void);
	int Load(const char *);
	void MixIt(void);
	Name *GetName(void);
	
	Name Names[BN_MAXNAMES];
	long lNum;
	long lLReturn;
	bool bInited;
};


class CPersonality : CParser{
public:
	CPersonality();
	~CPersonality();
	void Load(char *);
	void Save(char *);
	
	char szChatfile[32];
	char szPrefWeapon[32];
	int iSkill;				// = -1 when nothing is set
	float fAggressiveness;	// 0 is default
	int iPrefTeam;
	int iPrefClass;
	int iSpeakLeet;
	
	bool bLoaded;
protected:
private:
};

CPersonality CPDefault;
CPersonality *CPNames;
static CPersonality *CPAct;
static CBotNames Names;
static long lSelNum = -1;
HWND hDlg;
char szBuffer[200];
long lschl;
bool bTemp;

void GetIt(void){
	if(lSelNum != -1){
		if(lSelNum){
			CPAct = &(CPNames[lSelNum-1]);
		}else{
			for(lschl=0;lschl < Names.lNum;lschl++){
				if(CPNames[lschl].iSkill == CPDefault.iSkill
					&& CPNames[lschl].fAggressiveness == CPDefault.fAggressiveness
					&& CPNames[lschl].iPrefClass == CPDefault.iPrefClass
					&& CPNames[lschl].iPrefTeam == CPDefault.iPrefTeam
					&& CPNames[lschl].iSpeakLeet == CPDefault.iSpeakLeet
					&& !strcmp(CPNames[lschl].szChatfile,CPDefault.szChatfile)
					&& !strcmp(CPNames[lschl].szPrefWeapon,CPDefault.szPrefWeapon)){
					// everything is the same as the default ... mark to be changed
					CPNames[lschl].bLoaded = false;
				}
				else{
					CPNames[lschl].bLoaded = true;
				}
			}
			CPAct = &CPDefault;
		}
		
		strcpy(CPAct->szPrefWeapon,szWeapons[SendDlgItemMessage(hDlg,IDC_CWEAPON,CB_GETCURSEL,0,0L)]);
		GetDlgItemText(hDlg,IDC_ESKILL,szBuffer,200);
		sscanf(szBuffer,"%i",&CPAct->iSkill);
		GetDlgItemText(hDlg,IDC_ECHAT,szBuffer,200);
		sscanf(szBuffer,"%s",CPAct->szChatfile);
		CPAct->fAggressiveness = (float)(- ( SendDlgItemMessage(hDlg,IDC_CAGGR,CB_GETCURSEL,0,0L)-2));
		CPAct->iPrefClass = SendDlgItemMessage(hDlg,IDC_CPREFC,CB_GETCURSEL,0,0L);
		if(!CPAct->iPrefClass)
			CPAct->iPrefClass = -1;
		CPAct->iPrefTeam = SendDlgItemMessage(hDlg,IDC_CPTEAM,CB_GETCURSEL,0,0L);
		if(!CPAct->iPrefTeam){
			CPAct->iPrefTeam = 5;
		}
		CPAct->iSpeakLeet = SendDlgItemMessage(hDlg,IDC_CSLEET,CB_GETCURSEL,0,0L)-1;
		
		/*for(lschl=0;lschl < Names.lNum;lschl++){
			CPNames[lschl].Load(Names.Names[lschl].szName);
			if(!CPNames[lschl].bLoaded){
				CPNames[lschl].fAggressiveness = CPDefault.fAggressiveness;		// copy fromd default
				CPNames[lschl].iSkill = CPDefault.iSkill;
				CPNames[lschl].iPrefClass = CPDefault.iPrefClass;
				strcpy(CPNames[lschl].szChatfile,CPDefault.szChatfile);
				strcpy(CPNames[lschl].szPrefWeapon,CPDefault.szPrefWeapon);
				CPNames[lschl].bLoaded = true;
			}
		}*/
	}
}

BOOL CALLBACK Dlgprog (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//static HINSTANCE	hInstance
	
	switch (iMsg)
	{
	case WM_INITDIALOG :
		//hInstance=((LPCREATESTRUCT)lParam)->hInstance;
		
		SendDlgItemMessage(hDlg,IDC_CNAME,CB_ADDSTRING,0,(LPARAM)"--- default ---");
		
		lschl=0;
		while(szWeapons[lschl][0]){
			SendDlgItemMessage(hDlg,IDC_CWEAPON,CB_ADDSTRING,0,(LPARAM)szWeapons[lschl]);
			lschl++;
		}
		for(lschl=0;lschl < Names.lNum;lschl++){
			SendDlgItemMessage(hDlg,IDC_CNAME,CB_ADDSTRING,0,(LPARAM)Names.Names[lschl].szName);
		}
		
		SendDlgItemMessage(hDlg,IDC_CAGGR,CB_ADDSTRING,0,(LPARAM)"very aggressive");
		SendDlgItemMessage(hDlg,IDC_CAGGR,CB_ADDSTRING,0,(LPARAM)"aggressive");
		SendDlgItemMessage(hDlg,IDC_CAGGR,CB_ADDSTRING,0,(LPARAM)"normal");
		SendDlgItemMessage(hDlg,IDC_CAGGR,CB_ADDSTRING,0,(LPARAM)"defensive");
		SendDlgItemMessage(hDlg,IDC_CAGGR,CB_ADDSTRING,0,(LPARAM)"very defensive");
		
		SendDlgItemMessage(hDlg,IDC_CPREFC,CB_ADDSTRING,0,(LPARAM)"none");
		SendDlgItemMessage(hDlg,IDC_CPREFC,CB_ADDSTRING,0,(LPARAM)"1");
		SendDlgItemMessage(hDlg,IDC_CPREFC,CB_ADDSTRING,0,(LPARAM)"2");
		SendDlgItemMessage(hDlg,IDC_CPREFC,CB_ADDSTRING,0,(LPARAM)"3");
		SendDlgItemMessage(hDlg,IDC_CPREFC,CB_ADDSTRING,0,(LPARAM)"4");

		SendDlgItemMessage(hDlg,IDC_CPTEAM,CB_ADDSTRING,0,(LPARAM)"na");
		SendDlgItemMessage(hDlg,IDC_CPTEAM,CB_ADDSTRING,0,(LPARAM)"1");
		SendDlgItemMessage(hDlg,IDC_CPTEAM,CB_ADDSTRING,0,(LPARAM)"2");

		SendDlgItemMessage(hDlg,IDC_CSLEET,CB_ADDSTRING,0,(LPARAM)"na");
		SendDlgItemMessage(hDlg,IDC_CSLEET,CB_ADDSTRING,0,(LPARAM)"no");
		SendDlgItemMessage(hDlg,IDC_CSLEET,CB_ADDSTRING,0,(LPARAM)"yes");
		
		CPDefault.Load("default");
		CPNames = new CPersonality[Names.lNum];
		for(lschl=0;lschl < Names.lNum;lschl++){
			CPNames[lschl].Load(Names.Names[lschl].szName);
			if(!CPNames[lschl].bLoaded){
				CPNames[lschl].fAggressiveness = CPDefault.fAggressiveness;		// copy fromd default
				CPNames[lschl].iSkill = CPDefault.iSkill;
				strcpy(CPNames[lschl].szChatfile,CPDefault.szChatfile);
				strcpy(CPNames[lschl].szPrefWeapon,CPDefault.szPrefWeapon);
				CPNames[lschl].bLoaded = true;
				CPNames[lschl].iPrefClass = CPDefault.iPrefClass;
				CPNames[lschl].iPrefTeam = CPDefault.iPrefTeam;
				CPNames[lschl].iSpeakLeet = CPDefault.iSpeakLeet;
			}
		}
		
		/*sprintf(szBuffer,"%li",lMaxLoops);
		SetDlgItemText(hDlg,IDC_RESETS,szBuffer);*/
		return false;
		
	case WM_CLOSE:
		EndDialog(hDlg,0);
		return 0;
		
	case WM_COMMAND :
		switch(LOWORD(wParam))
		{
		/*case IDC_ACCEPT:
		::hDlg = hDlg;
		GetIt();
			break;*/
		case IDOK:	// save stuff
			::hDlg = hDlg;
			GetIt();
			
			CPDefault.Save("default");
			for(lschl=0;lschl < Names.lNum;lschl++){
				if(CPNames[lschl].iSkill == CPDefault.iSkill
					&& CPNames[lschl].fAggressiveness == CPDefault.fAggressiveness
					&& CPNames[lschl].iPrefClass == CPDefault.iPrefClass
					&& CPNames[lschl].iPrefTeam == CPDefault.iPrefTeam
					&& CPNames[lschl].iSpeakLeet == CPDefault.iSpeakLeet
					&& !strcmp(CPNames[lschl].szChatfile,CPDefault.szChatfile)
					&& !strcmp(CPNames[lschl].szPrefWeapon,CPDefault.szPrefWeapon)){
					// everything is the same as the default ... no need to save it
				}
				else{
					CPNames[lschl].Save(Names.Names[lschl].szName);
				}
			}
			EndDialog(hDlg,0);
			return 0;
		case IDCANCEL:
			EndDialog(hDlg,0);
			return 0;
		case IDC_CNAME:
			switch((int)HIWORD(wParam))
			{
			case CB_SETCURSEL:
			case CBN_SELCHANGE:
				//////////////////////////////////////////////////////////////////////////////
				::hDlg = hDlg;
				GetIt();
				//////////////////////////////////////////////////////////////////////////////
				//MessageBox(0,0,0,0);
				lSelNum=SendDlgItemMessage(hDlg,IDC_CNAME,CB_GETCURSEL,0,0L);
				if(lSelNum){
					CPAct = &(CPNames[lSelNum-1]);
				}else{
					CPAct = &CPDefault;
				}
				
				lschl=0;
				bTemp = false;
				while(szWeapons[lschl][0]){
					if(!strcmp(szWeapons[lschl],CPAct->szPrefWeapon)){
						SendDlgItemMessage(hDlg,IDC_CWEAPON,CB_SETCURSEL,lschl,true);
						bTemp = true;
						break;
					}
					lschl++;
				}
				if(!bTemp){
					SendDlgItemMessage(hDlg,IDC_CWEAPON,CB_SETCURSEL,0,true);
				}
				sprintf(szBuffer,"%i",CPAct->iSkill);
				SetDlgItemText(hDlg,IDC_ESKILL,szBuffer);
				SetDlgItemText(hDlg,IDC_ECHAT,CPAct->szChatfile);
				if(CPAct->fAggressiveness < -1.5){
					SendDlgItemMessage(hDlg,IDC_CAGGR,CB_SETCURSEL,0,true);
				}
				else if(CPAct->fAggressiveness < -.5){
					SendDlgItemMessage(hDlg,IDC_CAGGR,CB_SETCURSEL,1,true);
				}
				else if(CPAct->fAggressiveness < 0.5){
					SendDlgItemMessage(hDlg,IDC_CAGGR,CB_SETCURSEL,2,true);
				}
				else if(CPAct->fAggressiveness < -1.5){
					SendDlgItemMessage(hDlg,IDC_CAGGR,CB_SETCURSEL,3,true);
				}
				else {
					SendDlgItemMessage(hDlg,IDC_CAGGR,CB_SETCURSEL,4,true);
				}
				if(CPAct->iPrefClass>0){
					SendDlgItemMessage(hDlg,IDC_CPREFC,CB_SETCURSEL,CPAct->iPrefClass,true);
				}
				else
					SendDlgItemMessage(hDlg,IDC_CPREFC,CB_SETCURSEL,0,true);

				if(CPAct->iPrefTeam<3 && CPAct->iPrefTeam>0){
					SendDlgItemMessage(hDlg,IDC_CPTEAM,CB_SETCURSEL,CPAct->iPrefTeam,true);
				}
				else
					SendDlgItemMessage(hDlg,IDC_CPTEAM,CB_SETCURSEL,0,true);
				SendDlgItemMessage(hDlg,IDC_CSLEET,CB_SETCURSEL,CPAct->iSpeakLeet + 1,true);
			}
			break;	
		}
		break ;
		
	}
	return FALSE ;
}


CBotNames :: CBotNames(){
	lNum = 0;
	lLReturn = 0;
	bInited = false;
	for(long lschl=0;lschl < BN_MAXNAMES;lschl++){
		memset(Names[lschl].szName,0,sizeof(char) * BN_MAXNAMELENGTH);
	}
	Init();
}

int CBotNames :: Init(void){
	return Load("bot_names.txt");
}

CBotNames :: ~CBotNames(){
}

int CBotNames :: Load(const char *szFileName){			// load names into ram ...
	char *szFileContent,*szAct,*szTmp,szName[200];
	long lFileSize;
	long lToReadLength;
	FILE *fhd;
	//char szType[200],szWeapon[200];
	
	if(lFileSize = GetFileSize(szFileName))
	{
		szFileContent = new char[lFileSize+1];
		memset(szFileContent,0,sizeof(char)*(lFileSize+1));
		
		if(fhd = fopen(szFileName,"r")){
			bInited = true;
			
			fread(szFileContent,sizeof(char),lFileSize,fhd);
			
			fclose(fhd);
			
			Preprocess(szFileContent);		// remove uninteresting stuff
			
			szAct = szFileContent;
			
			while(*szAct){	// stop if 0
				while(!IsInstr(*szAct) && *szAct)	// goto next text ...
					szAct++;
				
				if (!(szTmp=strchr(szAct,'\n')))
					break;
				
				memset(szName,0,sizeof(char) * 200);
				lToReadLength = strchr(szAct,'\n') - szAct;
				strncpy(szName,szAct,sizeof(char) * lToReadLength);
				if(strlen(szName) < BN_MAXNAMELENGTH){
					strcpy(Names[lNum].szName,szName);
					//cout << szAdd << endl;
				}
				else{
					szAct = strchr(szAct,'\n');
					continue;
				}
				szAct = szAct + lToReadLength * sizeof(char);
				
				while(!IsInstr(*szAct) && *szAct)	// goto next text ...
					szAct++;
				
				lNum ++;
				if(lNum>BN_MAXNAMES)		// just ignore some names ...
					lNum--;
			}
		}
		delete [] szFileContent;
	}
	
	return true;
}

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CPersonality :: CPersonality(){
	*szChatfile = 0;
	*szPrefWeapon = 0;
	iSkill = -1;
	fAggressiveness = 0;
	bLoaded = false;
	iPrefTeam = -1;
	iPrefClass = -1;
	iSpeakLeet = -1;
}

CPersonality :: ~CPersonality(){
}

void CPersonality :: Load(char *szName){
	char szFilename[32];
	char szWFilename[32];
	char *szFile,*szAct;
	FILE *fhd;
	
	strcpy(szFilename,"per\\");
	strcat(szFilename,szName);
	strcat(szFilename,".per");
	
	//cout << "JoeBOT : Loading personality file for " << szName<< " : " <<szWFilename << endl;
	
	long lSize;
	strcpy(szWFilename,szFilename);
	
	lSize = GetFileSize(szWFilename);
	if(!(fhd = fopen(szWFilename,"r")))
		return;
	
	szFile = new char[lSize+1];
	memset(szFile,0,sizeof(char)*(lSize+1));
	
	fread(szFile,sizeof(char),lSize,fhd);
	
	Preprocess(szFile);		// remove uninteresting stuff
	
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
	
	// process content
	delete [] szFile;
	bLoaded = true;
}

void CPersonality :: Save(char *szName){
	char szFilename[32];
	char szWFilename[32];
	FILE *fhd;
	
	strcpy(szFilename,"per\\");
	strcat(szFilename,szName);
	strcat(szFilename,".per");

	strcpy(szWFilename,szFilename);
	
	cout << "JoeBOT : Saving personality file for " << szName<< " : " << szWFilename << endl;
	
	if(!(fhd = fopen(szWFilename,"w")))
		return;
	//MessageBox(0,0,0,0);
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
	fprintf(fhd,"# CS_WEAPON_M4A1	<--> CS_WEAPON_AK47;\n");
	fprintf(fhd,"# CS_WEAPON_TMP		<--> CS_WEAPON_MAC10;\n");
	fprintf(fhd,"# CS_WEAPON_SG550	<--> CS_WEAPON_G3SG1;\n");
	fprintf(fhd,"# CS_WEAPON_AUG		<--> CS_WEAPON_SG552;\n");
	fprintf(fhd,"# \n");
	fprintf(fhd,"# weapon names are :\n");
	fprintf(fhd,"# \n");
	fprintf(fhd,"# weapon_scout\n");
	fprintf(fhd,"# weapon_xm1014\n");
	fprintf(fhd,"# weapon_mac10\n");
	fprintf(fhd,"# weapon_aug\n");
	fprintf(fhd,"# weapon_ump45\n");
	fprintf(fhd,"# weapon_sg550\n");
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