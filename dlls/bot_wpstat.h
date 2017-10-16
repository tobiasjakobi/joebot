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
#ifndef __BOT_WPStat_H
#define __BOT_WPStat_H

#include "waypoint.h"

// define search stuff

#define WPS_SEARCH_SNIPER		1
#define WPS_SEARCH_CAMP			2
#define WPS_SEARCH_ENEMYESP		3
#define WPS_SEARCH_ENEMYESPTT	4

// struct and class defs --------------------------------------------------------------

#define _FTNAME "JoeBOT StatFile\0"
#define _WPSVERSION 14

typedef struct wpsheader{
	char szFTName[20];
	// store map name here. because at every global init of this structure the 
	// gpGlobals->mapname is the right stuff, this can be set while loading. 
	// because it's not clear which content this variable has when a new map 
	// has started, or let's say the var has the name of the new map, the save 
	// function just stores the data to the name defined in here
	char szMapname[40];
	long lVersion;
}WPSHEADER;

#define MAX_TIMES_PWP 4
// structure to contain statistical information
typedef struct WPStatitem{
	long lKill;							// number of kills
	long lKilled;						// number of bots being killed
	float fpKilledTime[MAX_TIMES_PWP];	// time of kills
	int lKilledT;
	float fpKillTime[MAX_TIMES_PWP];	// time of kills
	int lKillT;
	float fADEn;						// average distance to enemy (if 0 set to distance, else (nx+cdist)/(n+1))

	int iNVWp;							// number of visible waypoints
	float fADVWp;						// average distance to visible waypoints
	float fMaxDVWp;						// Max distance to next visible WP
	float fMinDVWp;						// Min Distance to next visible WP
	float fAHDVWp;						// average height difference of visible waypoints to this waypoint

	long lTraffic[2];					// for the two teams each one variable
}WPStatITEM;

typedef struct WPStatdata{
	long lKill;
	long lKilled;
	WPStatITEM FItem[MAX_WAYPOINTS];
}WPStatDATA;

class CWPStat{
public:
	CWPStat();
	~CWPStat();

	void Init(void);
	void InitWP(long lWP);
	int AddKill(edict_t *pEdictKiller, edict_t *pEdictVictim);
	void AddTraffic(int iTeam,int iWP);

	int Load(void);
	int Save(void);
	int Search(int,int,int,float,float,float fParam =0,float fParam2 = 0,edict_t *pEdict = 0,bool bVisible = false);
	int SearchNearest(int,int,int,float);
	bool GetWayInfo(int iTeam,int iSIndex, int iEIndex,WAYINFO &wayinfo);
	bool GetWayInfo(CWay &Way,WAYINFO &wayinfo);

	void CalcSlice(void);
	void CalcWP(long lWP,long lWP2);
	int GetVisible(long l1,long l2);
	int SetVisible(long l1,long l2,long lS);

	WPSHEADER wpsHeader;
	WPStatDATA d;

	long *pWPV;
	bool bRecalcWPV;
	long lNWPfWPV;	// number of waypoints for table pWPV
	long lWPSlice;
	long lPercent;		// just for messages

	WPStatITEM &operator [] (long);

	long lKillMax;
	long lKilledMax;
protected:
private:
};

#endif  //__BOT_WPStat_H
