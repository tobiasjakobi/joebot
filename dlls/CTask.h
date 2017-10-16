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
#ifndef __CTASK_H
#define __CTASK_H

#include "extdll.h"
#include "waypoint.h"

#define _MAX_TASKS 20

class CTaskItem{
public:
	friend class CTask;

	CTaskItem();
	~CTaskItem();

	long lType;			// type of task
	float fLive2;		// fulfill task till
	float fAdd;			// additional time information
	long lAdd;			// additional information, counter or everything else
	edict_t *p;			// a pointer ... for example edict_t * to point to another player
	CWay *pRWay;
	Vector VT;
private:
};

class CTask{
public:
	CTask();
	~CTask();

	void NextValidTask(void);
	bool AddTask(long, float, long, void *,float);
	bool AddTaskI(long, float, long, void *,float);
	bool NextTask(void);
	void Reset(void);
	long SearchP(void *);
	long SearchT(long);
	long RemoveP(void *);
	long RemoveT(long);
	bool Important(void);
	bool CheckTLife(void);
	CTaskItem *GetTask(long);
	long Current(void);
	bool IsCurrent(long);

	CTaskItem *current;
	CTaskItem *current_main;
	long lNOT;
protected:
	CTaskItem FTask[_MAX_TASKS];
private:
};

// defs of bot's tasks
#define BT_COVER		(1<<0)		// bot should cover specified entity
#define BT_HELP			(1<<1)		// bot should run and help specified entity
#define BT_FOLLOW		(1<<2)		// bot should follow specified entity
#define BT_ROAMTEAM		(1<<3)		// bot should go along with others. Note that this 'order' is given to the bot by himself, therefore it can be canceled every time. The time the bot made this decision is stored
#define BT_GUARD		(1<<4)		// bot should guard sth. This can be a teamm8 planting, defusing the bomb etc - the time the bot can stop guarding is stored
#define BT_CAMPATGOAL	(1<<5)		// bot should go normal to far_goal and camp there
// p -> should bot say i'm in position ?
// lAdd -> wp
// fAdd -> time to camp
#define BT_HOLDPOS		(1<<6)		// bot should wait
#define BT_WAIT4TM8		(1<<7)		// wait for teamm8
#define BT_GOTO			(1<<8)		// goto a location. lAdd is destination, p is "long" and wanted div
#define BT_PICKUP		(1<<9)		// this is a little pickup mission ;) -> combine with goto "|"
#define BT_FLEE			(1<<10)		// flee from location given by VT
#define BT_HIDE			(1<<11)		// hide from entity given by p or if p is 0 from location def by VT - hide till fLive2
#define BT_RELOAD		(1<<12)		// Reload weapon .... when this task is to be fullfilled, denywchange has to be set and the bot has to press reload first
// --- init
// lAdd is 0
// --- reload is pressed
// lAdd is 1
//#define BT_PAUSE		(1<<13)		// Bot should pause for fLive2 seconds (lAdd = 0)
									// Bot should pause till fLive2 (lAdd = 1)
#define BT_GOBUTTON		(1<<14)		// Go and press the button specified in p
#define BT_HUNT			(1<<15)		// hunt down s.o.
// p -> enemy to hunt down :D

#define BT_FIGHT		(1<<16)
#define BT_CAMP			(1<<17)
#define BT_BLINDED		(1<<18)
#define BT_IGNOREENEMY	(1<<19)		// ignore enemy, and proceed tasks

// additional flags for the bitfields

#define BT_CROUCH		(1<<28)		// should some order be fulfilled crouched ?
#define BT_LOCKED		(1<<29)		// lock task system so no new task can be added
#define BT_TMP			(1<<30)
#define BT_DEL			(1<<31)

#endif //__CTASK_H
