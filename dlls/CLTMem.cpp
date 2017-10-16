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
#include "extdll.h"
#include "util.h"

#include "CLTMem.h"

CLTMemItem :: CLTMemItem(){
	bUsed		= false;
	iWPIndex	= -1;
	VAddI		= Vector(0,0,0);
	VAddI2		= Vector(0,0,0);
	pEdict		= 0;
	lType		= 0;
	lCount		= 0;
	fTime		= 0;
}

CLTMemItem :: ~CLTMemItem(){
}


CLTMem :: CLTMem(CBotBase *){
	iCount = 0;
}

CLTMem :: ~CLTMem(){
}

int CLTMem :: Add(long lEvent,edict_t *pEdict,Vector &VAdd,Vector &VAdd2){
	//DEBUG_CLIENTCOMMAND(pEdict,"say ---");

	FMItem[iCount].bUsed = true;
	FMItem[iCount].fTime = gpGlobals->time;
	FMItem[iCount].lType = lEvent;
	FMItem[iCount].VAddI = VAdd;
	FMItem[iCount].VAddI2 = VAdd2;			// that means when killed, the origin of killer, when killing the origin of victim
	FMItem[iCount].pEdict = pEdict;
	FMItem[iCount].iWPIndex = -1;
	iCount++;
	if(iCount>=_MAX_LTMEM)
		iCount = 0;
	
	return 0;
}

