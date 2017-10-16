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
// LListElem.h: interface for the CLListElem class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __LLISTELEM_H
#define __LLISTELEM_H

class CLListElem  
{
public:
	virtual void Unlink(void);
	virtual CLListElem * NewItem(void);
	virtual CLListElem * AllocNewItem(void);
	CLListElem();
	virtual ~CLListElem();

	CLListElem *next,*prev;
protected :
};

#endif //__LLISTELEM_H
