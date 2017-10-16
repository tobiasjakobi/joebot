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
//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// waypoint.h
//

#ifndef WAYPOINT_H
#define WAYPOINT_H

#ifdef _WIN32
#include <limits.h>
#endif

#include "extdll.h"

#define _MAXTEMPDIVPRE 200
#define _MAXTEMPDIV 30

#define MAX_WAYPOINTS 2048

#define REACHABLE_RANGE_DEFAULT 400

extern float REACHABLE_RANGE;													// ???? to change some time

#define WAYPOINT_UNREACHABLE   USHRT_MAX
#define WAYPOINT_MAX_DISTANCE (USHRT_MAX-1)

// defines for waypoint flags field (32 bits are available)
#define W_FL_TEAM			((1<<0) + (1<<1))  /* allow for 4 teams (0-3) */
#define W_FL_TEAM_SPECIFIC	(1<<2)  /* waypoint only for specified team */
#define W_FL_CROUCH			(1<<3)  /* must crouch to reach this waypoint */
#define W_FL_LADDER			(1<<4)  /* waypoint on a ladder */
#define W_FL_LIFT			(1<<5)  /* wait for lift to be down before approaching this waypoint */
#define W_FL_PAUSE			(1<<6)  /* pause here (wait for door to open, etc.) */
#define W_FL_HEALTH			(1<<7)  /* health kit (or wall mounted) location */
#define W_FL_ARMOR			(1<<8)  /* armor (or HEV) location */
#define W_FL_AMMO			(1<<9) /* ammo location */
#define W_FL_SNIPER			(1<<10) /* sniper waypoint (a good sniper spot) */
#define W_FL_FLAG			(1<<11) /* flag position (or hostage or president) */
#define W_FL_BOMB			W_FL_FLAG
#define W_FL_HOSTAGE		W_FL_FLAG
#define W_FL_VIP_RESCUE		W_FL_FLAG
#define W_FL_TARGET			W_FL_FLAG
#define W_FL_FLAG_GOAL		(1<<12) /* flag return position (or rescue zone) */
#define W_FL_RESCUE			W_FL_FLAG_GOAL
#define W_FL_VISIT			(1<<13) /* locations to visit every once in a while */
#define W_FL_AIMING			(1<<14) /* locations to look at while camping (additional to others)*/
#define W_FL_DONTAVOID		(1<<15) /**/
#define W_FL_RESET			(1<<16) /**/
#define W_FL_DAF			(1<<17) /*dontavoid fall*/
#define W_FL_JUMP			(1<<18)
#define W_FL_BLOCKHOSTAGE	(1<<19)

#define W_FL_DELETED     (1<<31) /* used by waypoint allocation code */

#define WAYPOINT_VERSION 2

#ifdef _DEBUG
#define DEBUG_DRAWBEAM(pEntity,start,end,width,noise,red,green,blue,brightness,speed) \
	WaypointDrawBeam(pEntity,start,end,width,noise,red,green,blue,brightness,speed)
#else
#define DEBUG_DRAWBEAM(pEntity,start,end,width,noise,red,green,blue,brightness,speed) /* */
#endif

// define the waypoint file header structure...
typedef struct {
   char filetype[8];  // should be "Joe_bot\0"
   int  waypoint_file_version;
   int  waypoint_file_flags;  // not currently used
   int  number_of_waypoints;
   char mapname[32];  // name of map for these waypoints
} WAYPOINT_HDR;

// define the route file header structure...
typedef struct {
   char filetype[32];  // should be "Joe_bot\0"
} ROUTE_HDR;

#define DEFAULT_ROUTE_HDR "JoeBOT route file 0001\0"

// define the structure for waypoints...
typedef struct {
   int    flags;    // button, lift, flag, health, ammo, etc.
   Vector origin;   // location
} WAYPOINT;

#define MAX_PATH_INDEX 4

// define the structure for waypoint paths (paths are connections between
// two waypoint nodes that indicates the bot can get from point A to point B.
// note that paths DON'T have to be two-way.  sometimes they are just one-way
// connections between two points.  There is an array called "paths" that
// contains head pointers to these structures for each waypoint index.
typedef struct path {
   short int index[MAX_PATH_INDEX];  // indexes of waypoints (index -1 means not used)
   short int advindex[MAX_PATH_INDEX];  // indexes of advanced movements structures (index -1 means not used)
   struct path *next;   // link to next structure
} PATH;

/*typedef struct way{
	short int iIndices[MAX_WAYPOINTS];
	int iNum;
}WAY;*/

class CWay{
public:
	CWay();
	~CWay();
	short int operator [] (int iIndex);
	int FindItem(short int);

	short int iIndices[MAX_WAYPOINTS];
	int iNum;
protected:
private:
};

typedef struct wayinfo{
	bool bHostageable;			// is the related way hostageable ?
	long lStatKill;			// kills on this route from stat
	long lStatKilled;			// killed players on this route from stat
	long lSTMVisible;			// number of waypoints visible to STM items
}WAYINFO;

typedef struct awp_entlogitem{
	long lButton;
	long lFlags;
	long movetype;
	int iLastWP;
	float fLTime;
	float fLJumpTime;
	float fHeight;			// height of entity's origin from ground -> 36 normal 17 duck
	Vector VLastOrigin;

	Vector VLastJumpStartOrigin;			// start of last jump
	int iLastJumpStartWPIndex;				// waypoint index of next jump wp within a certain radius when starting to jump
	bool bLastJumpStartDuck;				// is player ducked when it started to jump ?

	float fStartRec;
	float fLastRec;
}AWP_EntLogItem;

typedef class adv_recitem{
public:
	adv_recitem ();
	~adv_recitem ();

	float fTime;		// time offset from beginning of recording
	long lButton;
	Vector v_origin;
	Vector v_velocity;
	Vector v_v_angle;
	Vector v_angles;
}ADV_RecItem;

#define _MAXRECPERSEC	10
#define _MAXRECDUR		2
#define _MAXRECITEMS	_MAXRECPERSEC*_MAXRECDUR

typedef class adv_wprecmove{
public:
	adv_wprecmove();
	~adv_wprecmove();

	int iIndexStartWP;
	int iIndexEndWP;
	int iNum;				// caontains the number of recorded items
	ADV_RecItem Rec[_MAXRECITEMS];
}ADV_WPRecMove;

// waypoint function prototypes...
void WaypointInit(void);
int  WaypointFindPath(PATH **pPath, int *path_index, int waypoint_index, int team);
//int  WaypointFindNearest(edict_t *pEntity, float distance, int team);
int WaypointFindNearest(edict_t *pEntity, float range, int team=-1, float fMin = -1,bool bVisib=false, bool bIVC=false, bool bReachable = false);

int  WaypointFindNearest(Vector v_src, edict_t *pEntity, float range, int team,bool bVisible = true);
int  WaypointFindNearestGoal(edict_t *pEntity, int src, int team, int flags);
int  WaypointFindNearestGoal(Vector v_src, edict_t *pEntity, float range, int team, int flags,float fMin = -1);
int  WaypointFindNearestGoal(edict_t *pEntity, int src, int team, int flags,float fMin = -1,int*iField=0,int iNumField=1);
int  WaypointFindRandomGoal(edict_t *pEntity, int team, int flags);
int  WaypointFindRandomGoal(Vector v_src, edict_t *pEntity, float range, int team, int flags);
int  WaypointFindNearestAiming(Vector v_origin);

ADV_WPRecMove *WaypointGetADV(short int origin_index, short int aim_index);
void WaypointAddADV(short int origin_index, short int aim_index, ADV_WPRecMove *pData);

void WaypointDebug(void); 

int  WaypointAdd(edict_t *pEntity);
long WaypointAdd(Vector &VOrigin,long lFlag,bool bpaths = false,bool bSEnt = false);
void WaypointAddPath(short int add_index, short int path_index);
void WaypointAddAiming(edict_t *pEntity);
void WaypointDelete(edict_t *pEntity);
void WaypointDelete(int index,edict_t *pEntity=0);
void WaypointCreatePath(edict_t *pEntity, int cmd);
void WaypointRemovePath(edict_t *pEntity, int cmd);
bool WaypointLoad(edict_t *pEntity,const char *szDir = 0);
bool WaypointSave(edict_t *pEdict,const char *szDir = 0);
bool WaypointReachable(Vector v_srv, Vector v_dest, edict_t *pEntity);
int  WaypointFindReachable(edict_t *pEntity, float range, int team);
void WaypointPrintInfo(edict_t *pEntity,char *szText=0);
void WaypointPrintTest(edict_t *pEntity);
void WaypointThink(edict_t *pEntity);
void WaypointFloyds(short *shortest_path, short *from_to);
void WaypointRouteInit(void);
unsigned short  WaypointRouteFromTo(int src, int dest, int team);
int  WaypointDistanceFromTo(int src, int dest, int team);

bool WaypointGetDir(char *szFile,char *szDest);

//int WaypointFindNearest(edict_t *pEntity, float range, int team);
//int WaypointFindNearest(Vector v_src, edict_t *pEntity, float range, int team,bool bVisible = true);
//int WaypointFindNearestVisible(edict_t *pEntity, float range, int team);
//int WaypointFindNearestVisible(edict_t *pEntity, float range, int team,float fMin);
//int WaypointFindNearestVisibleVC(edict_t *pEntity, float range, int team);

// visu
bool WaypointDrawBeam(edict_t *,Vector,Vector,int width,int noise, int red, int green, int blue, int brightness, int speed);
bool WaypointDrawBeamDebug(edict_t *,Vector,Vector,int width,int noise, int red, int green, int blue, int brightness, int speed);
bool WaypointDrawSpark(edict_t *pEntity,Vector start);
bool WaypointAddStdWP(edict_t *pEntity);

bool IsConnected(int i1,int i2);

// way stuff
void DeleteBotWays(void);
//long IsInWay(WAY *pFW,short int shS);

bool Hostagable(CWay *);
/*bool GetWayInfo(int iTeam,int iSIndex, int iEIndex,WAYINFO &wayinfo);
bool GetWayInfo(WAY &Way,WAYINFO &wayinfo);*/
int ConvertFloyd2Way(int iTeam,int iSrc, int iDest, CWay *pWay);

// number of waypoints currently in use
extern int num_waypoints;
extern WAYPOINT waypoints[MAX_WAYPOINTS];
extern PATH *paths[MAX_WAYPOINTS];
//extern char szPWPDir[][20];

long WaypointFlood(Vector VOrigin,long);
long WaypointFloodF(Vector &VOrigin,long lR);

int WaypointAddStuff(void);

long WaypointClean(void);

#endif //WAYPOINT_H

