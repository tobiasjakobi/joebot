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

#ifndef __NAMES_H
#define __NAMES_H

#ifdef __MINGW32__
#include <list.h>
#else
#include <list>
#endif

#include "CParser.h"

#define BN_MAXNAMES 50
#define BN_MAXNAMELENGTH 22

#define BN_PERS_DEF 1
#define BN_PERS_NOR 2
#define BN_PERS_AGG 3

class CBotNamesItem{
public:
	CBotNamesItem();
	~CBotNamesItem();
	//const CBotNamesItem &operator=(const CBotNamesItem &);

	char m_szName[32];

	// maybe we somewhen can extend this class for using personalities, personal NN files, whatever
};

class CBotNames:private CParser  
{
public:
	CBotNames();
	virtual ~CBotNames();

	bool init(void);
	const CBotNamesItem *getName(void);

private:
	bool bInited;
	bool load(const char *);
	void mixIt(void);
	std::list<CBotNamesItem> m_LNames;
	std::list<CBotNamesItem>::iterator m_ICName;		// iterator for m_LNames, so you can go thru the names
};

#endif //__NAMES_H
