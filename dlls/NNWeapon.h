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
#ifndef __NNWEAPON_H
#define __NNWEAPON_H

#include "bot_modid.h"
#include "CParser.h"

class NNWeapon:public CParser{
public:
	NNWeapon();
	void Init(void);
	~NNWeapon();

	void Load(const char *);
	void LoadFile(const char *);

	double dpWeaV[MAX_MOD][32];
	double dpWeaponPause[MAX_MOD][32];
	double dpWeaponPauseTime[MAX_MOD][32];
	//double dpWeaponDelay[32];
	int ipClipSize[MAX_MOD][32];
	int iCost[MAX_MOD][32];
	int iTeam[MAX_MOD][32];
	float fPBuyProb[MAX_MOD][32];
protected:
private:
};

#endif //__NNWEAPON_H
