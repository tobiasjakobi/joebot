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
// LListElem.cpp: implementation of the CLListElem class.
//
//////////////////////////////////////////////////////////////////////

#include "LListElem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLListElem::CLListElem()
{
	next = 0;
	prev = 0;
}

CLListElem::~CLListElem()
{
	Unlink();
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

CLListElem * CLListElem::AllocNewItem()
{
	return new CLListElem;
}

CLListElem * CLListElem::NewItem()
{
	if(next){
		return next->NewItem();
	}
	else{
		next = AllocNewItem();
		if(next) next->prev = this;
		return next;
	}
}

void CLListElem :: Unlink(void){
	if(next)
		next -> prev = prev;
	if(prev)
		prev -> next = next;

	prev = 0;
	next = 0;
}
