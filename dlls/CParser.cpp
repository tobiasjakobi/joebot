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

//#include <iostream.h>
#include <stdio.h>
#include <string.h>

#include "CParser.h"

CParser :: CParser(){
}

CParser :: ~CParser(){
}

long CParser :: GetFileSize(const char *szFileName){
	long ltemp=0;
	FILE *fhd;
	//char sztemp[1];
	
	fhd = fopen (szFileName,"rb");
	if(!fhd)
		return 0;
	
	while (!feof(fhd)){
		fgetc(fhd);
		ltemp ++;
		//cout << ltemp << endl;
		//ltemp+=fread(sztemp,sizeof(char),1,fhd);
	}
	fclose(fhd);
	return ltemp;
}

int CParser :: Preprocess(char *szOText){
//	char *szPText;
	char *szAct,*szTmp;
	
	//szPText = new char [strlen(szOText)+1];
	//memset(szPText,0,sizeof(char) * strlen(szOText));
	
	// remove comments
	szAct = szOText;
	while (*szAct){
		if(*szAct == '#'){					// skip comment
			szTmp = strchr(szAct,'\n');
			memset(szAct,' ',sizeof(char) * (szTmp - szAct));
			szAct = szTmp+1;
			continue;
		}
		//strncat(szPText,szAct,sizeof(char) * 1);
		szAct++;
	}
	*szAct = 0;

	//strcpy(szOText,szPText);
	//delete [] szPText;
	return 1;
}
/*int CParser :: Preprocess(char *szOText){
	char *szPText;
	char *szAct;
	
	szPText = new char [strlen(szOText)+1];
	memset(szPText,0,sizeof(char) * strlen(szOText));
	
	// remove comments
	szAct = szOText;
	while (*szAct){
		if(*szAct == '#'){					// skip comment
			szAct = strchr(szAct,'\n')+1;
			continue;
		}
		strncat(szPText,szAct,sizeof(char) * 1);
		szAct++;
	}
	*szAct = 0;
	
	strcpy(szOText,szPText);
	delete [] szPText;
	return 1;
}*/


bool CParser ::IsInstr(char cParam)
{
	switch(cParam)
	{
	case '\n':
	case '\v':
	case '\t':
	case '\r':
	case '\f':
	case ' ':
	case '\a':
	case ';':
		return false;
	default:
		return true;
	}
}
