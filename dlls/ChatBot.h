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
// ChatBot.h: interface for the CChatBot class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CHATBOT_H
#define __CHATBOT_H

class CBotBase;
class CChat;

class CChatBot  
{
public:
	CChatBot();
	virtual ~CChatBot();
	int LoadChat(CBotBase *);
	int UnLoadChat(CBotBase *);
	int Talk(CBotBase *);

	CChat *pChat;
	bool bLoadFailed;

	float f_LastChat;			// ...
	bool b1337chat;
	long  l_ChatEvent;			// ...
	char szChatText[200];		// text to say
	float f_SayTime;			// ..
};

#endif //__CHATBOT_H
