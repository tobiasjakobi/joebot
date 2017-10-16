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
// JoeBot
// by Johannes Lampel
// Johannes.Lampel@gmx.de
// http://joebot.counter-strike.de

#include "extdll.h"
#include "util.h"

#include "CSTMem.h"

#define _MAXSTMITEMS 6

CSTMemItem :: CSTMemItem(){
}

CSTMemItem :: ~CSTMemItem(){
}

void CSTMemItem::Calc(edict_t *pEdictP,bool bSeeThru){
	// Process this item -> look where the entity moved or could have been moved
	if(bNeed2Calc){
		if(bSeeThru){
			TraceResult trWBB,trBE;
			Vector VPPos;
			
			VPPos = VCalcedOrigin = pEdict->v.origin;	// calc probable position

			UTIL_TraceLine(pEdictP->v.origin,VPPos,ignore_monsters,ignore_glass,pEdict,&trBE);
			UTIL_TraceLine(VPPos,pEdictP->v.origin,ignore_monsters,ignore_glass,pEdict,&trWBB);
			//pBot->STMem.FMItem[ischl].bSolid = (trBE.fAllSolid!=0);	// is this trace blocked by a solid entity ?
			bSolid = ((trWBB.vecEndPos - trBE.vecEndPos).Length() > 100.0);

			if(trBE.flFraction == 1.f)
				bUsed = false;
		}
		else{
			TraceResult trWB,trWBB,trBE;
			Vector VPPos;
			
			VPPos = VOrigin + (gpGlobals->time - fTime) * VLSVelocity;	// calc probable position

			// trace line to see if enemy can go further
			UTIL_TraceLine(VOrigin,VPPos,ignore_monsters,ignore_glass,pEdict,&trWB);
			
			if(trWB.flFraction != 1.0){		// way is blocked -> just say that this hasn't be to calced on and on
				bNeed2Calc = false;
			}
			
			// trace line from bot to this prop location. If it's visible, just say that
			// old probable position is right, no need for further calculating. When the bot
			// sees this enemy another time this'll be updated by BotFindEnemy
			UTIL_TraceLine(pEdictP->v.origin,VPPos,ignore_monsters,ignore_glass,pEdict,&trBE);
			
			if(trBE.flFraction == 1.0){		// i.e. visible
				bNeed2Calc = false;
				return;
			}
			VCalcedOrigin = VPPos;	// copy this location to structure
			
			UTIL_TraceLine(VPPos,pEdictP->v.origin,ignore_monsters,ignore_glass,pEdict,&trWBB);
			//pBot->STMem.FMItem[ischl].bSolid = (trBE.fAllSolid!=0);	// is this trace blocked by a solid entity ?
			bSolid = ((trWBB.vecEndPos - trBE.vecEndPos).Length() > 100.0);
		}
	}
}

//--------------------------------------------------------

CSTMem :: CSTMem(){
	iCount=0;
	iMax=0;
	f_NUpdate = 0;
	fDefT2L = 20.0;		// default t2l in stmem is 20seconds
	iMax = _MAXSTMITEMS;

	pData = new CSTMemItem[iMax];

	//printf("constr %p\n",pData);
}

CSTMem :: ~CSTMem(){
	if(pData) delete[]pData;
}

int CSTMem :: IsInMem(edict_t *pEdict2S){
	int i;
	for(i=0;i<iCount;i++){
		if(pData[i].pEdict == pEdict2S){
			return i;
		}
	}
	return -1;
}
#include <iostream.h>
bool CSTMem :: Process(edict_t *pEdictP,bool bSeeThru){
	if(f_NUpdate < gpGlobals->time){
		f_NUpdate = gpGlobals->time + .2;

		int i;
		//float ftemp;
		
		for(i=0;i<iCount;i++){
			if(pData[i].fLive2 < gpGlobals->time-pData[i].fTime)
				pData[i].bUsed = false;

			if(!IsAlive(pData[i].pEdict))
				pData[i].bUsed = false;

			if(pData[i].bUsed)
				pData[i].Calc(pEdictP,bSeeThru);
		}
		Shift();
		for(i=0;i<iCount;i++){
			if(!IsAlive(pData[i].pEdict))
				cout << "!" << endl;
		}
		return true;
	}
	else
		return false;
}

int CSTMem :: Update(edict_t *pEdict2Up){
	// add to stm ...
	int index;
	if((index = IsInMem(pEdict2Up))==-1){		// that means new item
		if(iCount+1<iMax){		// there is place !
			index = iCount;
			iCount++;
		}
		else{
			index = iCount;		// just overwrite last item
		}
	}
	else{
		// do nothing when item is already in list ... this'll be updated
	}

	pData[index].fLive2			= fDefT2L;
	pData[index].bUsed			= true;
	pData[index].bNeed2Calc		= true;					// recalcing is needed
	pData[index].bSolid			= false;				// nothing is solid when starting
	pData[index].fTime			= gpGlobals->time;
	pData[index].pEdict			= pEdict2Up;
	pData[index].VCalcedOrigin	= pEdict2Up->v.origin;	// now, the propable location is the seen one
	pData[index].VOrigin		= pEdict2Up->v.origin;
	pData[index].VLSVelocity	= pEdict2Up->v.velocity;

	return true;
}

int CSTMem :: UpdateP(edict_t *pEdict2Up){
	if(iCount==0){		// that means no items before
		iCount++;
	}
	// just overwrite 0
	pData[0].fLive2			= fDefT2L;
	pData[0].bUsed			= true;
	pData[0].bNeed2Calc		= true;					// recalcing is needed
	pData[0].bSolid			= false;				// nothing is solid when starting
	pData[0].fTime			= gpGlobals->time;
	pData[0].pEdict			= pEdict2Up;
	pData[0].VCalcedOrigin	= pEdict2Up->v.origin;	// now, the propable location is the seen one
	pData[0].VOrigin		= pEdict2Up->v.origin;
	pData[0].VLSVelocity	= pEdict2Up->v.velocity;

	return true;
}

CSTMemItem &CSTMem :: operator[](long l){
	return((CSTMemItem&)(pData[l]));
}

void CSTMem :: Shift(void){
	int i;
	for(i=iCount-1;i>=0;i--){		// shift everything to start of field
		if(!pData[i].bUsed){
			if(i == iCount-1)
				iCount --;
			else{
				pData[i] = pData[iCount-1];
				iCount --;
			}
		}
	}
}

int CSTMem :: GetWayInfo(CWay &Way,WAYINFO &wayinfo){
	// see if the STM has something to say to this way :D
	register long lschl;
	int iMI;
	int iReturn = 0;

	for(lschl=0;lschl < Way.iNum;lschl++){
		for(iMI=0;iMI<iCount;iMI++){
			if(FVisible(waypoints[Way[lschl]].origin,pData[iMI].pEdict)){
				wayinfo.lSTMVisible ++;
			}
		}
	}
	return iReturn;
}
