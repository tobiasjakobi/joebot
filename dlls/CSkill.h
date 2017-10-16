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
#ifndef __CSKILL_H
#define __CSKILL_H

#include "CParser.h"

struct CSkillItem{
	int iSkill;				// related skill
	float fVSC;				// view change speed
	float fVSM;				// momentum
	float fReact;			// time to react
	float fShootThru;		// able to shoot thru objects without estimating ?
	float fITurnProb;		// instant turn probability on fighting
	float fInDistance;		// max distance at which enemies are seen instantly
	float fTPD;				// how much time it takes to recognize enemies above this distance ... this value is milliseconds per 1000 units
	float fProneInDistance;	// max distance at which enemies are seen instantly - prone
	float fProneTPD;		// how much time it takes to recognize enemies above this distance ... this value is milliseconds per 1000 units - prone
	float fMaxRecoil;		// maximum ammount of recoil before the bots stops shooting for some time ( burst fire )
};

class CSkill:private CParser{
public:
	CSkill();
	~CSkill();

	void GetSkill(int,CSkillItem&);

	void Load(void);
protected:
	CSkillItem SData[10];
	int iCount;
private:
};

#endif //__CSKILL_H
