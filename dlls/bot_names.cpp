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

#include "bot_names.h"
#include "Commandfunc.h"

CBotNamesItem g_DefaultName;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBotNamesItem::CBotNamesItem(){
	*m_szName = 0;
}
CBotNamesItem::~CBotNamesItem(){
}


CBotNames::CBotNames(){
	bInited = false;

	strcpy(g_DefaultName.m_szName,"JoeBOT");
}

CBotNames::~CBotNames(){
}

bool CBotNames::init(void){
	char szfilename[80];

	if (bInited)
		return true;

	UTIL_BuildFileName(szfilename, sizeof(szfilename), "joebot/bot_names.txt");
	bool bSuccess = load(szfilename);
	if (CVAR_BOOL(jb_mixnames)) mixIt(); // randomize order?
	m_ICName = m_LNames.begin(); // set list iterator

	return bSuccess;
}

bool CBotNames::load(const char *szFileName){
	char *szFileContent,*szAct,*szTmp,szName[200];
	long lFileSize;
	long lToReadLength;
	FILE *fhd;
	//char szType[200],szWeapon[200];
	
	if(lFileSize = GetFileSize(szFileName))
	{
		szFileContent = new char[lFileSize+2];
		memset(szFileContent,0,sizeof(char)*(lFileSize+2));
		
		if(fhd = fopen(szFileName,"r")){
			bInited = true;
			if (IS_DEDICATED_SERVER())
				LOG_MESSAGE(PLID, "Loading names from file: %s", szFileName);

			//fread(szFileContent,sizeof(char),lFileSize,fhd);

			char *p = szFileContent;
			while (!feof(fhd)){
				*p = fgetc(fhd);
				p ++;
			}

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
				if(!lToReadLength)
					break;
				strncpy(szName,szAct,sizeof(char) * lToReadLength);
				STRIP_CR(szName);
				if(strlen(szName) < BN_MAXNAMELENGTH){
					//strcpy(Names[lNum].szName,szName);
					//cout << szAdd << endl;

					CBotNamesItem tempName;

					strcpy(tempName.m_szName,szName);
					m_LNames.push_back(tempName);
				}
				else{
					szAct = strchr(szAct,'\n');
					continue;
				}
				szAct = szAct + lToReadLength * sizeof(char);
				
				while(!IsInstr(*szAct) && *szAct)	// goto next text ...
					szAct++;
			}
		}
		delete [] szFileContent;
	}
	// if file wasn't filled up ... or file not found ...

	return true;
}

void CBotNames::mixIt(void){
	int i1, i2,i;
	std::list<CBotNamesItem>::iterator iter1,iter2;
	CBotNamesItem temp;

	for(i=m_LNames.size()*5; i ; i--){
		i1 = RANDOM_LONG(0,m_LNames.size()-1);
		i2 = RANDOM_LONG(0,m_LNames.size()-1);

		if(i1 == i2)			// no need to swap
			continue;

		iter1 = iter2 = m_LNames.begin();

		for(;i1; i1 --){
			iter1++;
		}
		for(;i2; i2 --){
			iter2++;
		}
		/*if(iter1 == m_LNames.end()
			||iter2 == m_LNames.end()){
			cout << "---------" << endl;
			continue;
		}*/

		temp = *iter1;			// swap
		*iter1 = *iter2;
		*iter2 = temp;
	}
}

const CBotNamesItem *CBotNames::getName(void){
	//cout << m_LNames.size() << " names"<<endl;
	CBotNamesItem *nItem;

	if(!m_LNames.empty()){
		nItem = &(*m_ICName++);
		if(m_ICName == m_LNames.end()){
			m_ICName = m_LNames.begin();
		}
	}
	else
		nItem = &g_DefaultName;

	return nItem;
}
