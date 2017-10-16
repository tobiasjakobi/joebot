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

#include "CTask.h"

CTaskItem :: CTaskItem(){
	lType = 0;
	fLive2 = 0;
	p = 0;
	lAdd = 0;
	pRWay = 0;
}

CTaskItem :: ~CTaskItem(){
	if(pRWay){
		delete pRWay;
	}
}

CTask :: CTask (){
	lNOT = 0;
	current = 0;
	current_main = 0;
}

CTask :: ~CTask (){
}

bool CTask :: CheckTLife(void){
	long lschl;
	bool bFound1 = false;

	for(lschl = 0 ; lschl< lNOT;lschl++){
		if(FTask[lschl].fLive2 > 0){		// do not kill any tasks which have infinite life
			if(FTask[lschl].fLive2 < gpGlobals->time){
				FTask[lschl].lType |= BT_DEL;
				bFound1 |= true;
			}
		}
	}
	NextValidTask();
	return bFound1;
}

void CTask :: NextValidTask(void){
	if(lNOT > 0){
		while(FTask[lNOT - 1].lType & BT_DEL
			&& lNOT > 0){
			NextTask();
		}
	}
}

bool CTask :: AddTask(long lPType, float fPTime, long lPAdd, void * pP,float fAddP){
	//cout << "addtask" << endl;
	if(lNOT >= _MAX_TASKS
		|| (current&&current->lType & BT_LOCKED))
		return false;

	if(lNOT < 0)
		lNOT = 0;
	
	if(!(lPType & BT_TMP)){
		while(current
			&& (current->lType & BT_TMP)
			&& lNOT > 0){
			lNOT --;
			current = &(FTask[lNOT-1]);
		}
	}

	current = &(FTask[lNOT]);

	current -> fLive2	= fPTime;
	current -> lAdd		= lPAdd;
	current -> lType	= lPType;
	current -> p		= (edict_t *)pP;
	current -> fAdd		= fAddP;

	if(!(current->lType & BT_TMP)){
		current_main = current;
	}

	lNOT ++;

	return true;
}

bool CTask :: AddTaskI(long lPType, float fPTime, long lPAdd, void * pP,float fAddP){
	if(!AddTask(lPType,fPTime,lPAdd,pP,fAddP)){			// if there cannot be added a new task, just overwrite the actual one
		current -> fLive2	= fPTime;
		current -> lAdd		= lPAdd;
		current -> lType	= lPType;
		current -> p		= (edict_t *)pP;
		current -> fAdd		= fAddP;
	}

	return true;
}

bool CTask :: NextTask(void){
	lNOT --;
	if(lNOT>0){
		if(current ->lType & BT_TMP){
			current = &(FTask[lNOT-1]);
		}
		else{
			current_main = current = &(FTask[lNOT-1]);
		}

		if(current->lType & BT_DEL){
			return NextTask();
		}
		return true;
	}
	else{
		current = 0;
		current_main = 0;
		return false;
	}
}

void CTask :: Reset(void){
	//cout << "reset" << endl;
	lNOT = 0;
	current = 0;
}

long CTask :: SearchP(void *pP){
	for(long lschl = 0;lschl < lNOT;lschl++){
		if(FTask[lschl].p == pP
			&&!(FTask[lschl].lType & BT_DEL)){
			return lschl;
		}
	}
	return -1;
}

long CTask :: SearchT(long lP){
	for(long lschl = 0;lschl < lNOT;lschl++){
		if(FTask[lschl].lType & lP
			&&!(FTask[lschl].lType&BT_DEL)){
			return lschl;
		}
	}
	return -1;
}

long CTask :: RemoveP(void *pP){
	long lR = 0;
	for(long lschl = 0;lschl < lNOT;lschl++){
		if(FTask[lschl].p == pP){
			FTask[lschl].lType |= BT_DEL;
			lR ++;
		}
	}
	NextValidTask();
	return lR;
}

long CTask :: RemoveT(long lP){
	long lR = 0;
	for(long lschl = 0;lschl < lNOT;lschl++){
		if(FTask[lschl].lType & lP){
			FTask[lschl].lType |= BT_DEL;
			lR ++;
		}
	}
	NextValidTask();
	return lR;
}

bool CTask :: Important(void){
	if(current){
		if(lNOT > 0){
			while(FTask[lNOT - 1].lType & BT_DEL
				&& lNOT > 0){
				NextTask();
			}
		}
		if(current -> lType == BT_GOTO
			||current -> lType == BT_ROAMTEAM
			||current -> lType == BT_RELOAD)
			return false;
		return true;
	}
	return false;
}

CTaskItem *CTask :: GetTask(long lGet){
	if(lGet < lNOT){
		return &FTask[lGet];
	}
	else
		return 0;
}

long CTask :: Current(void){
	if (current)
		return current->lType;
	else
		return 0;
}

bool CTask :: IsCurrent(long lTask){
	return (Current() & lTask) ? true : false;
}
