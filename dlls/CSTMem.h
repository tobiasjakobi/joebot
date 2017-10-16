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
#ifndef __STMEM_H
#define __STMEM_H

#include "extdll.h"

#include "waypoint.h"

// func def in bot.h therefore just cpied it
bool IsAlive(edict_t *pEdict);
bool FVisible( const Vector &vecOrigin, edict_t *pEdict );

class CSTMemItem{
public:
	CSTMemItem();
	~CSTMemItem();

	void Calc(edict_t *,bool);

	Vector	VLSVelocity;
	Vector	VCalcedOrigin;
	bool	bNeed2Calc;
	bool	bSolid;
	bool bUsed;
	edict_t *pEdict;		// related edict
	long lType;				// type of memorized event
	float fTime;			// time of event
	float fLive2;			// time 2 live ( 0 -> eternal life )
	Vector VOrigin;			// place
	int iNWP;				// nearest wp ( really needed ? )
protected:
private:
};

class CSTMem{
public:
	CSTMem();
	virtual ~CSTMem();
	CSTMemItem &operator[](long);

	bool Process(edict_t *pEdict,bool);
	int Update(edict_t *pEdict2Up);
	int UpdateP(edict_t *pEdict2Up);
	int GetWayInfo(CWay &Way,WAYINFO &wayinfo);
	int IsInMem(edict_t *pEdict2S);

	virtual void Shift(void);

	float f_NUpdate;		// next update
	float fDefT2L;			//default time to live
	int iCount;
	int iMax;

	CSTMemItem *pData;

protected:
private:
};

#endif //__STMEM_H
