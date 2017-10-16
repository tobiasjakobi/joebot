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
// WorldGnome.cpp: implementation of the CWorldGnome class.
//
//////////////////////////////////////////////////////////////////////

#include "extdll.h"
#include "util.h"

#include "WorldGnome.h"

#include "bot.h"
#include "globalvars.h"

void Garbage(void *p);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern long lGnomes,lGnomesTemp;

CWorldGnome::CWorldGnome()
{
	next = 0;
}

CWorldGnome::~CWorldGnome()
{

}

CLListElem * CWorldGnome::Add1stGnome()
{
	CGnome *p;

	p = new CGnome;

	next = p;

	p->prev = 0;

	lGnomes = 0;

	return p;
}

CLListElem * CWorldGnome::AddGnome()
{
	return next->NewItem();
}

void CWorldGnome::Think()
{
	lGnomesTemp = 0;
	if(next)
		((CGnome *)(next))->Think();
	
	lGnomes = lGnomesTemp;
	
	if(lGnomes){
		if(RANDOM_LONG(0,100) < 20)
			if(pEdictPlayer)FakeClientCommand(pEdictPlayer,"say %li",lGnomes);
	}
}
