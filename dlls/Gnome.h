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
// Gnome.h: interface for the CGnome class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GNOME_H
#define __GNOME_H

#include "extdll.h"
#include "LListElem.h"

class CGnome : public CLListElem  
{
public:
	CGnome();
	virtual ~CGnome();
	virtual CLListElem * AllocNewItem(void);
	virtual void Think(void);
	virtual void Move(void);
	virtual void AvoidWall(void);
	virtual void CheckLife(void);
	virtual void CheckInteresting(void);
	virtual void Autowaypoint(void);

	Vector v_origin;
	Vector v_historicorigin;
	Vector v_move;
	Vector v_look;
	bool bCrouched;
	long lFrame;
	long lEnergy;
	int iLWP;
};

#endif //__GNOME_H
