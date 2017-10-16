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
#ifndef __CLTMEM_H
#define __CLTMEM_H

#include "bot_globaldefs.h"

#include "basemem.h"

#define _MAX_LTMEM 20

#define LTM_NONE	(1<<0)
#define LTM_KILLED  (1<<1)
#define LTM_KILL	(1<<2)
#define LTM_TKILL	(1<<3)

class CBotBase;

class CLTMemItem{
public:
	CLTMemItem();
	~CLTMemItem();

	bool bUsed;
	int iWPIndex;
	Vector VAddI;		// additional information
	Vector VAddI2;
	edict_t *pEdict;
	long lType;
	long lCount;		// if u need to count sth.
	float fTime;
protected:
private:
};

class CLTMem{
public:
	CLTMem(CBotBase *);
	~CLTMem();
	int Add(long lEvent,edict_t *pEdict,Vector &VAdd,Vector &VAdd2);

	CLTMemItem FMItem[_MAX_LTMEM];
	int iCount;
	float f_NUpdate;
protected:
private:
};

#endif //__CLTMEM_H
