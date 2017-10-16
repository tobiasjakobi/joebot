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
// ChatHost.cpp: implementation of the CChatHost class.
//
//////////////////////////////////////////////////////////////////////

#include <iostream.h>

#include "extdll.h"
#include "util.h"

#ifdef USE_METAMOD
#define SDK_UTIL_H  // util.h already included
#include "meta_api.h"
#endif /* USE_METAMOD */

#include "ChatHost.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChatHost::CChatHost()
{
	pData = 0;
}

CChatHost::~CChatHost()
{
	CChatHostData *p;

	p = pData;
	if(p){
	while(p->next){
		delete p->next;
	}
	delete p;
	pData = 0;
	}

/*	while(p){
		p = p->prev;
		delete p->next;
	}*/
}

CChat *CChatHost :: GetChat(const char *szNameP){
	char szName[256];

	UTIL_BuildFileName(szName, sizeof(szName), "joebot/chat/%s", szNameP);

	//cout << "wanting "<<szName<<endl;
	CChatHostData *p = 0,*pp = 0;

	p = pData;
	while(p){
		if(FStrEq(p->szFile,szName)){
			p->iUsed ++;
			return p->pChat;
		}
		pp = p;
		p = p->next;
	}
	// p is 0 ... we need to make a new data elem and load the file
	p = new CChatHostData;
	if(!pData)
		pData = p;
	if(pp){
		pp->next = p;
		pp->next->prev = pp;
	}

	if(!(p->pChat->LoadFile(szName))){
		if(p == pData)
			pData = 0;
		delete p;
		return 0;
	}
	strcpy(p->szFile,szName);

	p->iUsed ++;
	return p->pChat;
}

int CChatHost :: DisconnectChat(const char *szNameP){
	char szName[80];

	UTIL_BuildFileName(szName, sizeof(szName), "joebot/chat/%s", szNameP);

	CChatHostData 
		*p = 0,
		*pp = 0,
		*pF=0;

	p = pData;
	while(p){
		if(FStrEq(p->szFile,szName)){
			pF = p;
			break;
		}
		pp = p;
		p = p->next;
	}
	if(pF){
		pF->iUsed --;		// decrement the counter
		if(!pF->iUsed){
			LOG_MESSAGE(PLID, "Unloading chat file: %s", szName);
			delete pF;
			if(pF == pData)
				pData = 0;
			return 1;
		}
	}
	return 0;
}
