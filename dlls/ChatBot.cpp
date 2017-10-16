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
// ChatBot.cpp: implementation of the CChatBot class.
//
//////////////////////////////////////////////////////////////////////

#include "extdll.h"
#include "util.h"

#include "ChatBot.h"
#include "ChatHost.h"
#include "CBotBase.h"
#include "Commandfunc.h"
#include "globalvars.h"

float _SCHATTIME = 10;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChatBot::CChatBot()
{
	b1337chat = RANDOM_LONG(0,100) < int(jb_chat1337->value);
	pChat = 0;
	f_LastChat = 20;
	f_SayTime = 0;

	bLoadFailed  = false;
}

CChatBot::~CChatBot()
{

}

int CChatBot :: Talk(CBotBase *pBBot){
	int iCat;
	int iTeam;
	int iSentence;
	edict_t *pEdict = pBBot->pEdict;

	if(!CVAR_BOOL(jb_chat) || bLoadFailed)
		return false;

	if(!pChat && !bLoadFailed)
		LoadChat(pBBot);

	if(!pChat){
		bLoadFailed = true;
		return false;
	}
	
	if(strlen(szChatText)){
		if(f_SayTime < gpGlobals->time){
			//cout << "wanting to say sth."<<endl;
			UTIL_HostSay(pBBot->pEdict,0,szChatText);
			//cout << STRING(pEdict->v.netname) << ":" << szChatText << endl;
			*szChatText = 0;
		}
	}
	if(f_LastChat + jb_chatfreq->value< gpGlobals->time){
		if(l_ChatEvent & E_Kill){
			iCat = T_Kill;
		}
		else if(l_ChatEvent & E_HDown){
			iCat = T_HDown;
		}
		else if(l_ChatEvent & E_BombPl){
			iCat = T_BombPl;
		}
		else if(l_ChatEvent & E_TK){
			iCat = T_TK;
		}
		else if(l_ChatEvent & E_WELCOME){
			iCat = T_Welcome;
		}
		else if( !IsAlive(pEdict) && RANDOM_LONG(0,100) < 10){
			if(RANDOM_LONG(0,100) < 50){		// don't chat too much when dead
				f_LastChat += jb_chatfreq->value;
				return false;
			}
			else{
				iCat = T_Dead;
			}
		}
		else{
			// check for chat texts from others :)
			if(int(jb_chati->value) == IC_DEAD && IsAlive(pBBot->pEdict))
				return false;
			else if(int(jb_chati->value) == IC_ALIVE && !IsAlive(pBBot->pEdict))
				return false;
			else if(int(jb_chati->value) == IC_NONE)
				return false;

			char *szSayText;
			if(IsAlive(pBBot->pEdict)){
				szSayText = szLastSayText;
			}
			else{
				szSayText = szDeadLastSayText;
			}
			if(strlen(szSayText)){
				// process this text and copy something to szChatText :)
				char *szAssociate = pChat->Associate(szSayText);
				if(szAssociate){
					strcpy(szChatText,szAssociate);
					pChat->ProcessChatString(pBBot,szChatText);
					
					if(b1337chat){
						pChat->to1337(szChatText);
					}
					
					f_SayTime = gpGlobals->time + RANDOM_FLOAT(1,4.0);
					f_LastChat = gpGlobals->time + RANDOM_FLOAT(-jb_chatfreq->value/2.0,jb_chatfreq->value/2.0);
					*szSayText = 0;	// just delete it :)
					return true;
				}
				else
					return false;
			}
			else
				return false;
		}
		
		l_ChatEvent = 0;			// reset chat event flag
		
		if(pBBot->bot_teamnm == CS_TEAM_CT){
			iTeam=0;
		}
		else{
			iTeam=1;
		}
		if(RANDOM_LONG(0,pChat->lCounter[iTeam][iCat]
			+ pChat->lCounter[2][iCat]) < pChat->lCounter[2][iCat]){		// just for equal appearance of all and team spec messages
			iTeam = 2;		//all
		}
		if(pChat->lCounter[iTeam][iCat] > 0){		// is a message in this category ?
			iSentence = RANDOM_LONG(0,pChat->lCounter[iTeam][iCat]);
			if(!TEq(pChat->fTime[iTeam][iCat][iSentence],gpGlobals->time,_SCHATTIME)){
				strcpy(szChatText,pChat->szTexts[iTeam][iCat][iSentence].szText);
				
				pChat->ProcessChatString(pBBot,szChatText);
				
				if(b1337chat){
					pChat->to1337(szChatText);
				}
				
				pChat->fTime[iTeam][iCat][iSentence] = gpGlobals->time;
				f_SayTime = gpGlobals->time + RANDOM_FLOAT(0,2.0);
				f_LastChat = gpGlobals->time + RANDOM_FLOAT(-jb_chatfreq->value/2.0,jb_chatfreq->value/2.0);
				return true;
			}
		}
		else
			return false;
	}
	else{
		l_ChatEvent = 0;	// del chat event
		return false;
	}
	return false;
}

int CChatBot :: LoadChat(CBotBase *pBBot){
	pChat = g_ChatHost.GetChat(pBBot->Personality.szChatfile);
	if(pChat)
		return 1;
	else
		return 0;
}

int CChatBot :: UnLoadChat(CBotBase *pBBot){
	if(pChat){
		g_ChatHost.DisconnectChat(pBBot->Personality.szChatfile);
		return 1;
	}
	else
		return 0;
}
