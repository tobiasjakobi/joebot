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
#ifndef __BOT_WPDir_H
#define __BOT_WPDir_H

#include "bot_globaldefs.h"
#include "CParser.h"

typedef struct{
	char szDir[20];
} WPDir;

//#define MAX_WP_DIR 50

class CBotWPDir : private CParser{
public:
	CBotWPDir();
	~CBotWPDir();

	int Init(void);
	int Load(const char *);

	void MixIt(void);

	WPDir szPDir[MAX_WP_DIR];
protected:
private:
	bool bInited;
	long lNum;
};

#endif //__BOT_WPDir_H
