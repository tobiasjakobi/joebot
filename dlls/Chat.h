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
#ifndef __CHAT_H
#define __CHAT_H

#include "CBotBase.h"
#include "CParser.h"

#define _MAX_SENTENCES_PER_PART 100

#define MAXL_KEY 80
#define MAXL_TEXT 80

// #define categories
#define T_BombPl	0
#define T_HDown		1
#define T_Kill		2
#define T_Dead		3
#define T_TK		4
#define T_Welcome	5

// #define chat events
#define E_Dead		(1<<T_Dead)
#define E_Kill		(1<<T_Kill)
#define E_HDown		(1<<T_HDown)
#define E_BombPl	(1<<T_BombPl)
#define E_TK		(1<<T_TK)
#define E_WELCOME	(1<<T_Welcome)

#define _MAX_TAGS 6

class CChatBot;
class CChat;
class CChatIKeyElem;
class CChatITextElem;

struct CChatElem{
	char szText[MAXL_TEXT];
};

class CChatIElem{
	friend class CChat;
public:
	CChatIElem();
	~CChatIElem();

	bool Cmp2Key(const char *);
	CChatIKeyElem *AddKey(const char *);
	CChatITextElem *AddText(const char *);
	CChatITextElem *GetRandomText(void);
protected:
	CChatIElem *prev,*next;
	CChatIKeyElem *pKeys;
	CChatITextElem *pTexts;
private:
};

class CChatIKeyElem{
	friend class CChat;
	friend class CChatIElem;
public:
	CChatIKeyElem();
	~CChatIKeyElem();
protected:
	CChatIKeyElem *prev,*next;
	char szKey[MAXL_KEY];
private:
};

class CChatITextElem{
	friend class CChat;
	friend class CChatIElem;
public:
	CChatITextElem();
	~CChatITextElem();
protected:
	CChatITextElem *prev,*next;
	char szText[MAXL_TEXT];
	float fTime;
private:
};

class CChat:private CParser{
	friend class CChatBot;
public:
	CChatIElem *AddIElem(void);
	CChat();
	~CChat();
	//int Talk(CBotBase *);
	bool to1337(char *szText);
	void InitChat(void);
	int Load(CBotBase *);
	int LoadFile(const char *);
	void ConvertName(char *szChangP);
//protected:
	int ProcessChatString(CBotBase *,char *);

	char *Associate(const char *);

private:
	bool bNeed2Init;
	CChatElem szTexts[3][_MAX_TAGS][_MAX_SENTENCES_PER_PART];
	float fTime[3][_MAX_TAGS][_MAX_SENTENCES_PER_PART];
	long lCounter[3][_MAX_TAGS];

	CChatIElem *pIChatData;
};

#endif //__BOT_CHAT_H
