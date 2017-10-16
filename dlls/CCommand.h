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
#ifndef _CCOMMAND_H
#define _CCOMMAND_H

#include "extdll.h"

#ifdef __MINGW32__
#include <list.h>
#else
#include <list>
#endif

#define CM_CONSOLE		(1<<0)
#define CM_DEDICATED	(1<<1)
#define CM_SCRIPT		(1<<2)
#define CM_ALL			(1<<0)|(1<<1)|(1<<2)

class CCommand{
	friend class CCommands;
public:
	CCommand();
	~CCommand();

	//virtual CLListElem * AllocNewItem(void);

	bool (*pFunc) (edict_t *p,int iType,const char *arg1,const char *arg2,const char *arg3, const char *arg4);
	char szName[32];
	char sz2Name[32];
	int iType;
	//CCommand *prev,*next;
};

class CCommands{
public:
	CCommands();
	~CCommands();
	bool Exec(edict_t*,int,const char*,const char *,const char *,const char *,const char *);
	bool AddCommand(const char *,const char *,void *,int);

	std::list<CCommand> m_LCommands;
};

#endif //_CCOMMAND_H
