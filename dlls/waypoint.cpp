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

//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// waypoint.cpp
//

#include <iostream.h>
#include <time.h>

#ifdef _WIN32
#include <io.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>
#ifdef __linux__
#include <unistd.h>
#endif

#include "extdll.h"
#include "util.h"

#ifdef USE_METAMOD
#define SDK_UTIL_H  // util.h already included
#include "meta_api.h"
#endif /* USE_METAMOD */

#include "waypoint.h"

#include "bot_modid.h"
#include "bot_wpdir.h"
#include "CBotBase.h"
#include "Commandfunc.h"
#include "globalvars.h"

extern int m_spriteTexture;

float REACHABLE_RANGE = REACHABLE_RANGE_DEFAULT;

//#define _DEBUG

long lbeam;

const float _MTSQUARE=20;

// waypoints with information bits (flags)
WAYPOINT waypoints[MAX_WAYPOINTS];

// number of waypoints currently in use
int num_waypoints;

// declare the array of head pointers to the path structures...
PATH *paths[MAX_WAYPOINTS];

//recorded advanced movements
ADV_WPRecMove *WPJRec[MAX_WAYPOINTS];
int num_recadvm;

// time that this waypoint was displayed (while editing)
float wp_display_time[MAX_WAYPOINTS];

bool g_waypoint_paths = FALSE;		// have any paths been allocated?
Vector last_waypoint;
float f_path_time = 0.0;

char szWPCreator[80];

#define _AWP_NNWPDIST 150		// min distance to last wp when a wp should be added
#define _AWP_LNWPDIST 100		// min distance to last wp when a wp should be added on a ladder

AWP_EntLogItem AWP_ED[32];
ADV_WPRecMove ADVL[32];
float fStartRec[32];
bool bRec[32];
bool bStopRec[32];

unsigned int route_num_waypoints;
unsigned short *shortest_path[4] = {NULL, NULL, NULL, NULL};
unsigned short *from_to[4] = {NULL, NULL, NULL, NULL};

int g_FW_iNum;
CWay *g_FW_FOUND=0;
int g_FW_Team;

CWay :: CWay (){
}

CWay :: ~CWay (){
}

short int CWay ::operator [](int iIndex){
	if(iIndex < iNum
		&& iIndex >= 0)
		return iIndices[iIndex];
	else
		return -1;
}

int CWay::FindItem(short int iWP){
	int ischl;
	for(ischl = 0;ischl < iNum; ischl ++){
		if(iIndices[ischl] == iWP)
			return ischl;
	}
	return -1;
}

void DeleteBotWays(void){
	for(long lschl=0;lschl < 32;lschl++){
		if(bots[lschl]){
			bots[lschl]->iGoal = -1;
		}
	}
}

long IsInWay(CWay *pFW,short int shS){
	register long lschl;
	for(lschl=pFW->iNum-1;lschl >=0 ;lschl--){
		if(pFW->iIndices[lschl] == shS)
			return lschl;
	}
	return -1;
}

bool Hostagable(CWay *p_FW){
	//return true;
	for(register long lschl=0;lschl < p_FW->iNum;lschl++){
		if(waypoints[p_FW->iIndices[lschl]].flags & W_FL_BLOCKHOSTAGE){
			return false;
		}
	}
	return true;
}

// find the nearest waypoint to the player and return the index (-1 if not found)

int WaypointFindNearest(edict_t *pEntity, float range, int team, float fMin,bool bVisib, bool bIVC,bool bReachable)
{
	int i, min_index;
	float distance;
	float min_distance;
	TraceResult tr;
	
	if (num_waypoints < 1)
		return -1;
	
	// find the nearest waypoint...
	
	min_index = -1;
	//min_distance = 9999999.0;

	fMin = fMin * fMin;
	range = range * range;

	min_distance = range;
	
	for (i=0; i < num_waypoints; i++)
	{
		if ((waypoints[i].flags & W_FL_DELETED) == W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[i].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[i].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[i].flags & W_FL_TEAM) != team))
			continue;
		
		distance = (waypoints[i].origin - pEntity->v.origin).LengthSq();
		
		if ((distance < min_distance) &&(distance>fMin))
		{
			if(bVisib&&pEntity){
				if(!FVisible(waypoints[i].origin,pEntity))		// skip if not visible ...
					continue;
			}
			
			if(bIVC&&pEntity){
				if(!FInViewCone(&waypoints[i].origin,pEntity))
					continue;
			}
			if(bReachable&&pEntity){
				if(!WaypointReachable(pEntity->v.origin,waypoints[i].origin,pEntity)){
					continue;
				}
			}
			
			min_index = i;
			min_distance = distance;
		}
	}
	
	return min_index;
}

void WaypointDebug(void)
{
#ifdef _DEBUG
	int y = 1, x = 1;
	
	LOG_DEBUG("WaypointDebug: LINKED LIST ERROR!!!");
	
	x = x - 1;  // x is zero
	y = y / x;  // cause an divide by zero exception
#endif
}


// free the linked list of waypoint path nodes...
void WaypointFree(void)
{
	for (int i=0; i < MAX_WAYPOINTS; i++)
	{
		if(WPJRec[i]){		// free adv move mem
			delete WPJRec[i];
			WPJRec[i] = 0;
		}
		
		int count = 0;

		if (paths[i])
		{
			PATH *p = paths[i];
			PATH *p_next;
			
			while (p)  // free the linked list
			{
				p_next = p->next;  // save the link to next
				free(p);
				p = p_next;
				
#ifdef _DEBUG
				count++;
				if (count > 1000) WaypointDebug();
#endif
			}
			
			paths[i] = NULL;
		}
	}
}


// initialize the waypoint structures...
void WaypointInit(void)
{
	int i;
	
	// have any waypoint path nodes been allocated yet?
	if (g_waypoint_paths)
		WaypointFree();  // must free previously allocated path memory
	
	for (i=0; i < 4; i++)
	{
		if (shortest_path[i] != NULL)
			free(shortest_path[i]);
		
		if (from_to[i] != NULL)
			free(from_to[i]);
	}
	
	for (i=0; i < MAX_WAYPOINTS; i++)
	{
		waypoints[i].flags = 0;
		waypoints[i].origin = Vector(0,0,0);
		
		wp_display_time[i] = 0.0;
		
		paths[i] = NULL;  // no paths allocated yet
		WPJRec[i] = NULL;
	}
	
	f_path_time = 0.0;  // reset waypoint path display time
	
	num_waypoints = 0;
	num_recadvm = 0;
	
	last_waypoint = Vector(0,0,0);
	
	for (i=0; i < 4; i++)
	{
		shortest_path[i] = NULL;
		from_to[i] = NULL;
	}
	memset(AWP_ED,0,sizeof(AWP_EntLogItem)*32);
}

ADV_WPRecMove *WaypointGetADV(short int origin_index, short int aim_index){
	PATH *p, *prev;
	int i;
	int count = 0;
	bool bBreak;
	
	p = paths[origin_index];
	prev = NULL;
	
	while (p != NULL)
	{
		i = 0;
		bBreak = false;
		
		while (i < MAX_PATH_INDEX)
		{
			if(p->index[i] == aim_index){
				bBreak = true;
				break;
			}
			
			i++;
		}
		if(bBreak)
			break;
		
		prev = p;     // save the previous node in linked list
		p = p->next;  // go to next node in linked list
		
#ifdef _DEBUG
		count++;
		if (count > 100) WaypointDebug();
#endif
	}
	if(p&&bBreak && p->advindex[i] != -1){
		// found an adv entry ...
		if(p->advindex[i]>=0
			&&p->advindex[i]<MAX_WAYPOINTS)
			return WPJRec[p->advindex[i]];
		else
			return 0;
	}
	else
		return 0;
}

void WaypointAddADV(short int origin_index, short int aim_index, ADV_WPRecMove *pData){
	PATH *p, *prev;
	int i;
	int count = 0;
	bool bBreak;
	
	// check if paths index is out of range
	if (origin_index > MAX_PATH_INDEX)
		return;

	p = paths[origin_index];
	prev = NULL;
	
	while (p != NULL)
	{
		i = 0;
		bBreak = false;
		
		while (i < MAX_PATH_INDEX)
		{
			if(p->index[i] == aim_index){
				bBreak = true;
				break;
			}
			
			i++;
		}
		if(bBreak)
			break;
		
		prev = p;     // save the previous node in linked list
		p = p->next;  // go to next node in linked list
		
#ifdef _DEBUG
		count++;
		if (count > 100) WaypointDebug();
#endif
	}
	if(bBreak&&p){
		// add an adv entry ...
		// find one
		//int iFound=-1;
		int iS;
		if(p->advindex[i] == -1){
			//cout << num_recadvm << endl;
			for(iS = 0; iS < num_recadvm;iS++){
				if(iS >= MAX_WAYPOINTS)
					return;
				if(!WPJRec[iS])
					break;
			}
		}
		else{
			iS = p->advindex[i];
		}
		
		if(iS >= num_recadvm){
			//iS = num_recadvm;
			num_recadvm ++;
		}
		if(iS > MAX_WAYPOINTS)
			return;

		WPJRec[iS] = new ADV_WPRecMove;		// alloc new memory
		
		/*memcpy(&(WPJRec[iS]->Rec),pData->Rec,sizeof(ADV_RecItem)*_MAXRECITEMS);
		WPJRec[iS]->iIndexStartWP = origin_index;
		WPJRec[iS]->iIndexEndWP = aim_index;
		WPJRec[iS]->iNum = pData->iNum;*/
		memcpy(WPJRec[iS],pData,sizeof(ADV_WPRecMove));
		WPJRec[iS]->iIndexStartWP = origin_index;
		WPJRec[iS]->iIndexEndWP = aim_index;
		p->advindex[i] = iS;
	}
	else{
		// there should already a path when adding a adv move, therefore just add one and call this function
#ifdef _DEBUG
		FILE *fhd = fopen("scheisse.txt","a");fprintf(fhd,"if this is often, there is an error in waypointaddadv ( recursive calls doesn't end )\n");fclose(fhd);
#endif
		WaypointAddPath(origin_index,aim_index);
		WaypointAddADV(origin_index,aim_index,pData);
	}
}

// add a path from one waypoint (add_index) to another (path_index)...
void WaypointAddPath(short int add_index, short int path_index)
{
	PATH *p, *prev;
	int i;
	int count = 0;
	
	p = paths[add_index];
	prev = NULL;

	if(IsConnected(add_index,path_index))
		return;
	
	// find an empty slot for new path_index...
	while (p != NULL)
	{
		i = 0;
		
		while (i < MAX_PATH_INDEX)
		{
			if (p->index[i] == -1)
			{
				p->index[i] = path_index;
				
				return;
			}
			
			i++;
		}
		
		prev = p;     // save the previous node in linked list
		p = p->next;  // go to next node in linked list
		
#ifdef _DEBUG
		count++;
		if (count > 100) WaypointDebug();
#endif
	}
	
	p = (PATH *)malloc(sizeof(PATH));
	//p = new PATH;
	
	if (p == NULL)
	{
		ALERT(at_error, "[JOEBOT] Error allocating memory for path!");
	}
	
	for(int ischl=0;ischl < MAX_PATH_INDEX;ischl++){
		p->index[ischl] = -1;
		p->advindex[ischl] = -1;
	}
	
	p->index[0] = path_index;
	p->next = NULL;
	
	if (prev != NULL)
		prev->next = p;  // link new node into existing list
	
	if (paths[add_index] == NULL)
		paths[add_index] = p;  // save head point if necessary
}


// delete all paths to this waypoint index...
void WaypointDeletePath(short int del_index)
{
	PATH *p;
	int index, i;
	
	// search all paths for this index...
	for (index=0; index < num_waypoints; index++)
	{
		p = paths[index];
		
		int count = 0;
		
		// search linked list for del_index...
		while (p != NULL)
		{
			i = 0;
			
			while (i < MAX_PATH_INDEX)
			{
				if (p->index[i] == del_index)
				{
					p->index[i] = -1;  // unassign this path
					
					if(p->advindex[i] != -1){			// and delete the advm if there is onw
						delete WPJRec[p->advindex[i]];
						WPJRec[p->advindex[i]] = 0;
						p->advindex[i] = -1;
					}
				}
				
				i++;
			}
			
			p = p->next;  // go to next node in linked list
			
#ifdef _DEBUG
			count++;
			if (count > 100) WaypointDebug();
#endif
		}
	}
}


// delete a path from a waypoint (path_index) to another waypoint
// (del_index)...
void WaypointDeletePath(short int path_index, short int del_index)
{
	PATH *p;
	int i;
	int count = 0;
	
	p = paths[path_index];
	
	// search linked list for del_index...
	while (p != NULL)
	{
		i = 0;
		
		while (i < MAX_PATH_INDEX)
		{
			if (p->index[i] == del_index)
			{
				p->index[i] = -1;  // unassign this path
			}
			
			i++;
		}
		
		p = p->next;  // go to next node in linked list
		
#ifdef _DEBUG
		count++;
		if (count > 100) WaypointDebug();
#endif
	}
}


// find a path from the current waypoint. (pPath MUST be NULL on the
// initial call. subsequent calls will return other paths if they exist.)
int WaypointFindPath(PATH **pPath, int *path_index, int waypoint_index, int team)
{
	int index;
	int count = 0;
	
	if (*pPath == NULL)
	{
		*pPath = paths[waypoint_index];
		*path_index = 0;
	}
	
	if (*path_index == MAX_PATH_INDEX)
	{
		*path_index = 0;
		
		*pPath = (*pPath)->next;  // go to next node in linked list
	}
	
	while (*pPath != NULL)
	{
		while (*path_index < MAX_PATH_INDEX)
		{
			if ((*pPath)->index[*path_index] != -1)  // found a path?
			{
				// save the return value
				index = (*pPath)->index[*path_index];
				
				// skip this path if next waypoint is team specific and NOT this team
				if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
					((waypoints[index].flags & W_FL_TEAM) != team))
				{
					(*path_index)++;
					continue;
				}
				
				// set up stuff for subsequent calls...
				(*path_index)++;
				
				return index;
			}
			
			(*path_index)++;
		}
		
		*path_index = 0;
		
		*pPath = (*pPath)->next;  // go to next node in linked list
		
#ifdef _DEBUG
		count++;
		if (count > 100) WaypointDebug();
#endif
	}
	
	return -1;
}


// find the nearest waypoint to the player and return the index
// (-1 if not found)...

// find the nearest waypoint to the source postition and return the index
// of that waypoint...
int WaypointFindNearest(Vector v_src, edict_t *pEntity, float range, int team,bool bVisible)
{
	int index, min_index;
	float distance;
	float min_distance;
	TraceResult tr;
	
	if (num_waypoints < 1)
		return -1;
	
	// find the nearest waypoint...
	
	min_index = -1;
	min_distance = 9999999.0;

	range = range * range;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		distance = (waypoints[index].origin - v_src).LengthSq();
		
		if ((distance < min_distance) && (distance < range))
		{
			// if waypoint is visible from source position...
			if(bVisible&&pEntity){
				if(pEntity )UTIL_TraceLine( v_src, waypoints[index].origin, ignore_monsters,pEntity->v.pContainingEntity, &tr );
				else UTIL_TraceLine( v_src, waypoints[index].origin, ignore_monsters,0, &tr );
				
				if (tr.flFraction >= 1.0)
				{
					min_index = index;
					min_distance = distance;
				}
			}
			else{
				min_index = index;
				min_distance = distance;
			}
		}
	}
	
	return min_index;
}


// find the goal nearest to the player matching the "flags" bits and return
// the index of that waypoint...
/*int WaypointFindNearestGoal(edict_t *pEntity, int src, int team, int flags)
{
	int index, min_index;
	int distance, min_distance;
	
	if (num_waypoints < 1)
		return -1;
	
	// find the nearest waypoint with the matching flags...
	
	min_index = -1;
	min_distance = 9999999;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (index == src)
			continue;  // skip the source waypoint
		
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		distance = WaypointDistanceFromTo(src, index, team);
		
		if (distance < min_distance)
		{
			min_index = index;
			min_distance = distance;
		}
	}
	
	return min_index;
}*/

int WaypointFindNearestGoal(edict_t *pEntity, int src, int team, int flags,float fMin,int*piField,int iNumField)
{
	int index, min_index;
	int distance, min_distance,i;
	bool bAvoid = false;
	
	if (num_waypoints < 1)
		return -1;
	
	// find the nearest waypoint with the matching flags...
	
	min_index = -1;
	min_distance = 9999999;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (index == src)
			continue;  // skip the source waypoint
		
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		bAvoid=false;
		if(piField){
			for(i=0;i<iNumField;i++){
				if(piField[i]==index)
					bAvoid=true;
			}
		}
		if(bAvoid)
			continue;
		
		distance = WaypointDistanceFromTo(src, index, team);
		
		if(distance < fMin)
			continue;
		
		if (distance < min_distance){
			min_index = index;
			min_distance = distance;
		}
	}
	
	return min_index;
}


// find the goal nearest to the source position (v_src) matching the "flags"
// bits and return the index of that waypoint...
int WaypointFindNearestGoal(Vector v_src, edict_t *pEntity, float range, int team, int flags,float fMin)
{
	int index, min_index;
	float distance, min_distance;
	
	if (num_waypoints < 1)
		return -1;
	
	// find the nearest waypoint with the matching flags...
	
	min_index = -1;
	min_distance = 999999999;

	range = range * range;
	fMin = fMin * fMin;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		distance = (waypoints[index].origin - v_src).LengthSq();
		
		if (distance < range
			&& distance < min_distance
			&& distance > fMin)
		{
			min_index = index;
			min_distance = distance;
		}
	}
	
	return min_index;
}


// find a random goal matching the "flags" bits and return the index of
// that waypoint...
int WaypointFindRandomGoal(edict_t *pEntity, int team, int flags)
{
	int index;
	int indexes[50];
	int count = 0;
	
	if (num_waypoints < 1)
		return -1;
	
	// find all the waypoints with the matching flags...
	
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		if (count < 50)
		{
			indexes[count] = index;
			
			count++;
		}
		else
			break;
	}
	
	if (count == 0)  // no matching waypoints found
		return -1;
	
	index = RANDOM_LONG(0, count - 1);
	
	return indexes[index];
}


// find a random goal within a range of a position (v_src) matching the
// "flags" bits and return the index of that waypoint...
int WaypointFindRandomGoal(Vector v_src, edict_t *pEntity, float range, int team, int flags)
{
	int index;
	int indexes[50];
	int count = 0;
	float distance;
	
	if (num_waypoints < 1)
		return -1;
	
	// find all the waypoints with the matching flags...
	
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != team))
			continue;
		
		if ((waypoints[index].flags & flags) != flags)
			continue;  // skip this waypoint if the flags don't match
		
		distance = (waypoints[index].origin - v_src).Length();

		if(distance > range)
			continue;
		
		if (count < 50)
		{
			indexes[count] = index;
			
			count++;
		}
		else
			break;
	}
	
	if (count == 0)  // no matching waypoints found
		return -1;
	
	index = RANDOM_LONG(0, count - 1);
	
	return indexes[index];
}


// find the nearest "special" aiming waypoint (for sniper aiming)...
int WaypointFindNearestAiming(Vector v_origin)
{
	int index;
	int min_index = -1;
	float min_distance = 9999999.0;
	float distance;
	
	if (num_waypoints < 1)
		return -1;
	
	// search for nearby aiming waypoint...
	for (index=0; index < num_waypoints; index++)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if ((waypoints[index].flags & W_FL_AIMING) == 0)
			continue;  // skip any NON aiming waypoints
		
		distance = (v_origin - waypoints[index].origin).LengthSq();
		
		if ((distance < min_distance) && (distance < 3600))
		{
			min_index = index;
			min_distance = distance;
		}
	}
	
	return min_index;
}

bool WaypointDrawSpark(edict_t *pEntity,Vector start){
	if(lbeam > 100 || !pEntity)
		return 1;
	lbeam++;
	
	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, NULL, pEntity);
	WRITE_BYTE( TE_SPARKS);
	WRITE_COORD(start.x);
	WRITE_COORD(start.y);
	WRITE_COORD(start.z);
	MESSAGE_END();
	
	return 0;
}


bool WaypointDrawBeam(edict_t *pEntity, Vector start, Vector end, int width,
					  int noise, int red, int green, int blue, int brightness, int speed)
{
	if(lbeam > 100 || !pEntity)
		return 1;
	lbeam++;
	
	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, NULL, pEntity);
	WRITE_BYTE( TE_BEAMPOINTS);
	WRITE_COORD(start.x);
	WRITE_COORD(start.y);
	WRITE_COORD(start.z);
	WRITE_COORD(end.x);
	WRITE_COORD(end.y);
	WRITE_COORD(end.z);
	WRITE_SHORT( m_spriteTexture );
	WRITE_BYTE( 1 ); // framestart
	WRITE_BYTE( 10 ); // framerate
	WRITE_BYTE( 10 ); // life in 0.1's
	WRITE_BYTE( width ); // width
	WRITE_BYTE( noise );  // noise
	
	WRITE_BYTE( red );   // r
	WRITE_BYTE( green );   // g
	WRITE_BYTE( blue );   // b
	
	WRITE_BYTE( brightness );   // brightness
	WRITE_BYTE( speed );    // speed
	MESSAGE_END();
	
	return 0;
}

bool ConnectEntities(edict_t *pEntity,edict_t *p1Entity,edict_t *p2Entity)
{
	if(lbeam > 100 || !pEntity)
		return 1;
	lbeam++;
	
	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, NULL, pEntity);
	WRITE_BYTE( TE_BEAMENTS);

	// short (start entity) 
	WRITE_SHORT( ENTINDEX(p1Entity) );
// short (end entity) 
	WRITE_SHORT( ENTINDEX(p2Entity) );
// short (sprite index) 
	WRITE_SHORT( m_spriteTexture );
// byte (starting frame) 
	WRITE_BYTE( 0 );
// byte (frame rate in 0.1's) 
	WRITE_BYTE( 10 );
// byte (life in 0.1's) 
	WRITE_BYTE( 50 );
// byte (line width in 0.1's) 
	WRITE_BYTE( 50 );
// byte (noise amplitude in 0.01's) 
	WRITE_BYTE( 10 );
// byte,byte,byte (color)
	WRITE_BYTE( 0 );
	WRITE_BYTE( 255 );
	WRITE_BYTE( 0 );
// byte (brightness)
	WRITE_BYTE( 200 );
// byte (scroll speed in 0.1's)
	WRITE_BYTE( 1 );
	MESSAGE_END();
	
	return 0;
}

bool WaypointDrawBeamDebug(edict_t *pEntity, Vector start, Vector end, int width,
					  int noise, int red, int green, int blue, int brightness, int speed)
{
	if(lbeam > 100 || !pEntity)
		return 1;
	lbeam++;
	
	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, NULL, pEntity);
	WRITE_BYTE( TE_BEAMPOINTS);
	WRITE_COORD(start.x);
	WRITE_COORD(start.y);
	WRITE_COORD(start.z);
	WRITE_COORD(end.x);
	WRITE_COORD(end.y);
	WRITE_COORD(end.z);
	WRITE_SHORT( m_spriteTexture );
	WRITE_BYTE( 1 ); // framestart
	WRITE_BYTE( 10 ); // framerate
	WRITE_BYTE( 255 ); // life in 0.1's
	WRITE_BYTE( width ); // width
	WRITE_BYTE( noise );  // noise
	
	WRITE_BYTE( red );   // r
	WRITE_BYTE( green );   // g
	WRITE_BYTE( blue );   // b
	
	WRITE_BYTE( brightness );   // brightness
	WRITE_BYTE( speed );    // speed
	MESSAGE_END();
	
	return 0;
}

Vector v[4];
float dradius = 100;

long WaypointFlood(Vector VOrigin,long lR){
	long breturn;
	float dz = dradius / 2;
	float freachable;
	
	v[0] = Vector(dradius,0,dz);
	v[1] = Vector(-dradius,0,dz);
	v[2] = Vector(0,dradius,dz);
	v[3] = Vector(0,-dradius,dz);
	
	freachable = REACHABLE_RANGE;
	REACHABLE_RANGE = sqrt(2*100*100)+1;
	breturn = WaypointFloodF(VOrigin,lR);
	REACHABLE_RANGE = freachable;
	return breturn;
}

long WaypointFloodF(Vector &VOrigin,long lR){
	lR ++;
	if(lR > 100)
		return 0;
	Vector V2;
	TraceResult tr;
	int i;
	int wpindex;
	
	for(i=0;i<4;i++){
		V2 = VOrigin + v[i];
		UTIL_TraceLine(VOrigin,V2,ignore_monsters,ignore_glass,0,&tr);
		if(tr.flFraction == 1.f){
			if(WaypointFindNearest(V2,0,dradius-1.f,-1,false) == -1){
				wpindex = WaypointAdd(V2,0,true);
				if(wpindex != -1){
					V2 = waypoints[wpindex].origin;
					wp_display_time[wpindex] = gpGlobals->time + RANDOM_FLOAT(-1,1);
					
					WaypointFloodF(V2,lR);
					continue;
				}
			}
		}
		else
			continue;
	}
	return 0;
}

long WaypointAdd(Vector &VOrigin,long lFlag,bool bpaths,bool bSEnt){
	TraceResult tr;
	VOrigin = VOrigin;
	Vector VEnd = VOrigin - Vector(0,0,1000);
	UTIL_TraceLine(VOrigin,VEnd,ignore_monsters,0,&tr);
	Vector VGround = VOrigin - Vector(0,0,tr.flFraction*1000);
	
	VEnd = VGround + Vector(0,0,75);
	UTIL_TraceLine(VGround,VEnd,ignore_monsters,0,&tr);
	if(tr.flFraction == 1.f){
		VGround = VGround + Vector(0,0,35);
	}
	else{
		if(tr.flFraction * 75 < 20)
			return -1;
		VGround = VGround + Vector(0,0,18);
		lFlag |= W_FL_CROUCH;
	}
	
	if (num_waypoints >= MAX_WAYPOINTS)
		return -1;
	
	int index = 0;
	//if((waypoints[WaypointFindNearest(VGround,0,1000,-1)].origin - VGround - Vector(0,0,35)).Length() > 200){
	
	// find the next available slot for the new waypoint...
	while (index < num_waypoints)
	{
		if ((waypoints[index].flags & W_FL_DELETED) == W_FL_DELETED)
			break;
		
		index++;
	}
	
	waypoints[index].flags = lFlag;
	
	// store the origin (location) of this waypoint (use entity origin)
	waypoints[index].origin = VGround;
	wp_display_time[index] = gpGlobals->time;
	if (index == num_waypoints)
		num_waypoints++;
	
	if(bpaths){
		for (int i=0; i < num_waypoints; i++)
		{
			if (i == index)
				continue;  // skip the waypoint that was just added
			
			if (waypoints[i].flags & W_FL_DELETED)
				continue;
			
			if (waypoints[i].flags & W_FL_AIMING)
				continue;  // skip any aiming waypoints
			
			if (waypoints[i].flags & W_FL_LADDER)
				continue;  // skip any ladder waypoints
			
			if (waypoints[i].flags & W_FL_JUMP)
				continue;  // skip any jump waypoints
			
			// check if the waypoint is reachable from the new one (one-way)
			if ( WaypointReachable(waypoints[index].origin, waypoints[i].origin, 0) )
			{
					WaypointAddPath(index, i);
			}
			
			// check if the new one is reachable from the waypoint (other way)
			if ( WaypointReachable(waypoints[i].origin, waypoints[index].origin, 0) )
			{
					WaypointAddPath(i, index);
			}
		}
	}
	if(bSEnt){
		edict_t *pent = 0;
		char item_name[80];
		while ((pent = UTIL_FindEntityInSphere( pent, VOrigin, 75 )) != NULL){
			strcpy(item_name, STRING(pent->v.classname));
			
			if (FStrEq("hostage_entity", item_name)){							// hostages
				waypoints[index].flags |= W_FL_FLAG;
			}
			if (FStrEq("func_hostage_rescue", item_name)){						// rescue zone
				waypoints[index].flags |= W_FL_FLAG_GOAL;
			}
			if (FStrEq("func_bomb_target", item_name)){						// bomb target
				waypoints[index].flags |= W_FL_FLAG;
			}
		}
	}
	WPStat.InitWP(index);
	return index;
}

long WaypointClean(void){
	int index, sindex;
	index = 0;
	long lC= 0;
	while (index < num_waypoints){
		if ((waypoints[index].flags & W_FL_DELETED) == W_FL_DELETED){
			sindex = num_waypoints-1;
			while(sindex >= 0){
				if ((waypoints[sindex].flags & W_FL_DELETED) != W_FL_DELETED){
					
					waypoints[index].flags = waypoints[sindex].flags;
					waypoints[index].origin = waypoints[sindex].origin;
					num_waypoints = sindex;
					lC++;
					break;
				}
				sindex --;
			}
		}
		index ++;
	}
	return lC;
}

int WaypointAddStuff(void){
	int index = 0;
	char item_name[80];
	long lDel = 0;
	//if((waypoints[WaypointFindNearest(VGround,0,1000,-1)].origin - VGround - Vector(0,0,35)).Length() > 200){
	
	// find the next available slot for the new waypoint...
	/*while (index < num_waypoints)
	{
	if ((waypoints[index].flags & W_FL_DELETED) == W_FL_DELETED)
	continue;
	
	  // add paths
	  for (int i=0; i < num_waypoints; i++)
	  {
	  if (i == index)
	  continue;
	  if (waypoints[i].flags & W_FL_DELETED)
	  continue;
	  
		if((waypoints[i].origin-waypoints[index].origin).LengthCBD()<2.){
		WaypointDelete(i);
		lDel ++;
		index --;
		break;
		}
		}
		
		  index++;
}*/
	
	edict_t *pent = 0;
	float radius = 75;
	long lflag;
	bool bSet = false;
	while ((pent = UTIL_FindEntityInSphere( pent, waypoints[index].origin, radius )) != NULL)
	{
		strcpy(item_name, STRING(pent->v.classname));
		
		if (FStrEq("hostage_entity", item_name)){							// hostages
			lflag |= W_FL_FLAG;
			bSet = true;
		}
		if (FStrEq("func_hostage_rescue", item_name)){						// rescue zone
			lflag |= W_FL_FLAG_GOAL;
			bSet = true;
		}
		if (FStrEq("func_bomb_target", item_name)){						// bomb target
			lflag |= W_FL_FLAG;
			bSet = true;
		}
		if(bSet){
			bSet = false;
			index =0 ;
			float fDistance;
			while (index < num_waypoints)
			{
				if ((waypoints[index].flags & W_FL_DELETED) == W_FL_DELETED)
					continue;
				
				fDistance = (pent->v.origin - waypoints[index].origin).Length();
				if(fDistance < radius)
					waypoints[index].flags |= lflag;
				index++;
			}
		}
	}
	
	return 1;
}

int WaypointAdd(edict_t *pEntity)
{
	int index;
	edict_t *pent = NULL;
	float radius = 40;
	char item_name[64];
	
	if (num_waypoints >= MAX_WAYPOINTS
		|| !pEntity)
		return -1;
	
	index = 0;
	
	// find the next available slot for the new waypoint...
	while (index < num_waypoints)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			break;
		
		index++;
	}
	
	waypoints[index].flags = 0;
	
	// store the origin (location) of this waypoint (use entity origin)
	waypoints[index].origin = pEntity->v.origin;
	
	// store the last used waypoint for the auto waypoint code...
	last_waypoint = pEntity->v.origin;
	
	// set the time that this waypoint was originally displayed...
	wp_display_time[index] = gpGlobals->time;
	
	
	Vector start, end;
	
	start = pEntity->v.origin - Vector(0, 0, 34);
	end = start + Vector(0, 0, 68);
	
	if ((pEntity->v.flags & FL_DUCKING) == FL_DUCKING)
	{
		waypoints[index].flags |= W_FL_CROUCH;  // crouching waypoint
		
		start = pEntity->v.origin - Vector(0, 0, 17);
		end = start + Vector(0, 0, 34);
	}
	
	if (pEntity->v.movetype == MOVETYPE_FLY)
		waypoints[index].flags |= W_FL_LADDER;  // waypoint on a ladder
	
	
	//********************************************************
	// look for lift, ammo, flag, health, armor, etc.
	//********************************************************
	
	while ((pent = UTIL_FindEntityInSphere( pent, pEntity->v.origin, radius )) != NULL)
	{
		strcpy(item_name, STRING(pent->v.classname));
		
		if (FStrEq("hostage_entity", item_name)){							// hostages
			UTIL_ConsoleMessage(pEntity, "hostage_entity\n");
			waypoints[index].flags |= W_FL_FLAG;
		}
		if (FStrEq("func_hostage_rescue", item_name)){						// rescue zone
			UTIL_ConsoleMessage(pEntity, "func_hostage_rescue\n");
			waypoints[index].flags |= W_FL_FLAG_GOAL;
		}
		if (FStrEq("func_bomb_target", item_name)){						// bomb target
			UTIL_ConsoleMessage(pEntity, "func_bomb_target\n");
			waypoints[index].flags |= W_FL_FLAG;
		}
	}
	
	// draw a blue waypoint
	WaypointDrawBeam(pEntity, start, end, 30, 0, 0, 0, 255, 250, 5);
	
	if(CVAR_BOOL(jb_wpsound))
		EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "weapons/xbow_hit1.wav", 1.0,	ATTN_NORM, 0, 100);
	
	// increment total number of waypoints if adding at end of array...
	if (index == num_waypoints)
		num_waypoints++;
	
	// calculate all the paths to this new waypoint
	if(waypoints[index].flags & W_FL_LADDER)	// dont add to ladders
		return index;
	
	if(CVAR_BOOL(jb_wpautopath)){
		for (int i=0; i < num_waypoints; i++)
		{
			if (i == index)
				continue;  // skip the waypoint that was just added
			
			if (waypoints[i].flags & W_FL_DELETED)
				continue;
			
			if (waypoints[i].flags & W_FL_AIMING)
				continue;  // skip any aiming waypoints
			
			if (waypoints[i].flags & W_FL_LADDER)
				continue;  // skip any ladder waypoints
			
			if (waypoints[i].flags & W_FL_JUMP)
				continue;  // skip any ladder waypoints
			
			// check if the waypoint is reachable from the new one (one-way)
			if ( WaypointReachable(pEntity->v.origin, waypoints[i].origin, pEntity) )
			{
				// check this waypoint with more tracelines to get camouflage nets e.g.
				if(FVisibleMT(pEntity->v.origin,waypoints[i].origin,_MTSQUARE,pEntity))
					WaypointAddPath(index, i);
			}
			
			// check if the new one is reachable from the waypoint (other way)
			if ( WaypointReachable(waypoints[i].origin, pEntity->v.origin, pEntity) )
			{
				// check this waypoint with more tracelines to get camouflage nets e.g.
				if(FVisibleMT(waypoints[i].origin,pEntity->v.origin,_MTSQUARE,pEntity))
					WaypointAddPath(i, index);
			}
		}
	}
	if(pEdictPlayer){
		WaypointDrawSpark(pEdictPlayer,pEntity->v.origin);
	}
	WPStat.InitWP(index);
	return index;
}

bool WaypointAddStdWP(edict_t *pEntity){
	edict_t *pent=0;
	
	while(pent = UTIL_FindEntityByClassname(pent,"info_player_start")){			// ct start
		if((waypoints[WaypointFindNearest(pent->v.origin,pEntity,1000,-1)].origin - pent->v.origin).Length() > _ADDSTDWPDIST){
			WaypointAdd(pent->v.origin,0,true);
			if(pEntity)UTIL_ConsoleMessage( pEntity, "Added wp on ct starting point\n");
		}
	}
	pent = 0;
	while(pent = UTIL_FindEntityByClassname(pent,"info_player_deathmatch")){	// te start
		if((waypoints[WaypointFindNearest(pent->v.origin,pEntity,1000,-1)].origin - pent->v.origin).Length() > _ADDSTDWPDIST){
			WaypointAdd(pent->v.origin,0,true);
			if(pEntity)UTIL_ConsoleMessage( pEntity, "Added wp on te starting point\n");
		}
	}
	pent = 0;
	while(pent = UTIL_FindEntityByClassname(pent,"hostage_entity")){
		if((waypoints[WaypointFindNearest(pent->v.origin,pEntity,1000,-1)].origin - pent->v.origin).Length() > _ADDSTDWPDIST){
			WaypointAdd(pent->v.origin,W_FL_FLAG,true);
			if(pEntity)UTIL_ConsoleMessage( pEntity, "Added wp at hostages\n");
		}
	}
	/*pent = 0;
	while(pent = UTIL_FindEntityByClassname(pent,"func_bomb_target")){
	if((waypoints[WaypointFindNearest(pent->v.origin,pEntity,1000,-1)].origin - pent->v.origin).Length() > 200){
	WaypointAdd(pent->v.origin,W_FL_FLAG);
	UTIL_ConsoleMessage( pEntity, "Added wp bomb target\n");
	}
}*/
	pent = 0;
	while(pent = UTIL_FindEntityByClassname(pent,"func_hostage_rescue")){
		if((waypoints[WaypointFindNearest(pent->v.origin,pEntity,1000,-1)].origin - pent->v.origin).Length() > _ADDSTDWPDIST){
			WaypointAdd(pent->v.origin,W_FL_FLAG_GOAL,true);
			if(pEntity)UTIL_ConsoleMessage( pEntity, "Added wp for rescue zone\n");
		}
	}
	return true;
}

void WaypointAddAiming(edict_t *pEntity)
{
	int index;
	edict_t *pent = NULL;
	
	if (num_waypoints >= MAX_WAYPOINTS)
		return;
	
	index = 0;
	
	// find the next available slot for the new waypoint...
	while (index < num_waypoints)
	{
		if (waypoints[index].flags & W_FL_DELETED)
			break;
		
		index++;
	}
	
	waypoints[index].flags = W_FL_AIMING;  // aiming waypoint
	
	Vector v_angle = pEntity->v.v_angle;
	
	v_angle.x = 0;  // reset pitch to horizontal
	v_angle.z = 0;  // reset roll to level
	
	UTIL_MakeVectors(v_angle);
	
	// store the origin (location) of this waypoint (use entity origin)
	waypoints[index].origin = pEntity->v.origin + gpGlobals->v_forward * 25;
	
	// set the time that this waypoint was originally displayed...
	wp_display_time[index] = gpGlobals->time;
	
	
	Vector start, end;
	
	start = pEntity->v.origin - Vector(0, 0, 10);
	end = start + Vector(0, 0, 14);
	
	// draw a blue waypoint
	WaypointDrawBeam(pEntity, start, end, 30, 0, 0, 0, 255, 250, 5);
	
	if(CVAR_BOOL(jb_wpsound))
		EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "weapons/xbow_hit1.wav", 1.0,ATTN_NORM, 0, 100);
	
	// increment total number of waypoints if adding at end of array...
	if (index == num_waypoints)
		num_waypoints++;
}


void WaypointDelete(edict_t *pEntity)
{
	int index;
	int count = 0;
	
	if (num_waypoints < 1 || !pEntity)
		return;
	
	int i, min_index;
	float distance;
	float min_distance;
	TraceResult tr;
	
	if (num_waypoints < 1)
		index = -1;
	else{
		
		// find the nearest waypoint...
		
		min_index = -1;
		min_distance = 9999999.0;
		
		for (i=0; i < num_waypoints; i++)
		{
			if (waypoints[i].flags & W_FL_DELETED)
				continue;  // skip any deleted waypoints
			
			// skip this waypoint if it's team specific and teams don't match...
			if ((-1 != -1) && (waypoints[i].flags & W_FL_TEAM_SPECIFIC) &&
				((waypoints[i].flags & W_FL_TEAM) != -1))
				continue;
			
			distance = (waypoints[i].origin - pEntity->v.origin).Length();
			
			if ((distance < min_distance) && (distance < 50.0))
			{
				// if waypoint is visible from current position (even behind head)...
				UTIL_TraceLine( pEntity->v.origin + pEntity->v.view_ofs, waypoints[i].origin,
					ignore_monsters, pEntity->v.pContainingEntity, &tr );
				
				if (tr.flFraction >= 1.0)
				{
					min_index = i;
					min_distance = distance;
				}
			}
		}
		
		index =  min_index;
	}
	
	if (index == -1)
		return;
	
	if ((waypoints[index].flags & W_FL_SNIPER))
	{
		int i;
		int min_index = -1;
		float min_distance = 9999999.0;
		float distance;
		
		// search for nearby aiming waypoint and delete it also...
		for (i=0; i < num_waypoints; i++)
		{
			if (waypoints[i].flags & W_FL_DELETED)
				continue;  // skip any deleted waypoints
			
			if ((waypoints[i].flags & W_FL_AIMING) == 0)
				continue;  // skip any NON aiming waypoints
			
			distance = (waypoints[i].origin - waypoints[index].origin).Length();
			
			if ((distance < min_distance) && (distance < 40))
			{
				min_index = i;
				min_distance = distance;
			}
		}
		
		if (min_index != -1)
		{
			waypoints[min_index].flags = W_FL_DELETED;  // not being used
			waypoints[min_index].origin = Vector(0,0,0);
			
			wp_display_time[min_index] = 0.0;
		}
	}
	
	WaypointDelete(index,pEntity);
}

void WaypointDelete(int index,edict_t *pEntity){
	int i;
#ifdef _DEBUG
	int count = 0;
#endif
	// delete any paths that lead to this index...
	WaypointDeletePath(index);
	
	// free the path for this index...
	
	if (paths[index] != NULL)
	{
		PATH *p = paths[index];
		PATH *p_next;
		
		while (p)  // free the linked list
		{
			i = 0;
			
			while (i < MAX_PATH_INDEX)
			{
				if (p->advindex[i] != -1
					&& p->advindex[i] >= 0
					&& p->advindex[i]<MAX_WAYPOINTS)
				{
					delete WPJRec[p->advindex[i]];
					WPJRec[p->advindex[i]] = 0;
					p->advindex[i] = -1;
				}
				
				i++;
			}
			
			p_next = p->next;  // save the link to next
			free(p);
			p = p_next;
			
#ifdef _DEBUG
			count++;
			if (count > 100) WaypointDebug();
#endif
		}
		
		paths[index] = NULL;
	}
	
	waypoints[index].flags = W_FL_DELETED;  // not being used
	waypoints[index].origin = Vector(0,0,0);
	
	wp_display_time[index] = 0.0;
	
	if(CVAR_BOOL(jb_wpsound))
		if(pEntity)
			EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "weapons/mine_activate.wav", 1.0,ATTN_NORM, 0, 100);
		
		// del statistics for this wp
		WPStat.InitWP(index);
}


// allow player to manually create a path from one waypoint to another
void WaypointCreatePath(edict_t *pEntity, int cmd)
{
	static int waypoint1 = -1;  // initialized to unassigned
	static int waypoint2 = -1;  // initialized to unassigned
	if(!pEntity)
		return;
	
	if (cmd == 1)  // assign source of path
	{
		waypoint1 = WaypointFindNearest(pEntity, 50.0, -1);
		
		if (waypoint1 == -1)
		{
			// play "cancelled" sound...
			if(CVAR_BOOL(jb_wpsound))
				EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_moveselect.wav", 1.0,ATTN_NORM, 0, 100);
			
			return;
		}
		
		// play "start" sound...
		if(CVAR_BOOL(jb_wpsound))
			EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_hudoff.wav", 1.0,ATTN_NORM, 0, 100);
		
		return;
	}
	
	if (cmd == 2)  // assign dest of path and make path
	{
		waypoint2 = WaypointFindNearest(pEntity, 50.0, -1);
		
		if ((waypoint1 == -1) || (waypoint2 == -1))
		{
			// play "error" sound...
			if(CVAR_BOOL(jb_wpsound))
				EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_denyselect.wav", 1.0,ATTN_NORM, 0, 100);
			
			return;
		}
		
		WaypointAddPath(waypoint1, waypoint2);
		
		// play "done" sound...
		if(CVAR_BOOL(jb_wpsound))
			EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_hudon.wav", 1.0,ATTN_NORM, 0, 100);
	}
}


// allow player to manually remove a path from one waypoint to another
void WaypointRemovePath(edict_t *pEntity, int cmd)
{
	static int waypoint1 = -1;  // initialized to unassigned
	static int waypoint2 = -1;  // initialized to unassigned
	
	if (cmd == 1)  // assign source of path
	{
		waypoint1 = WaypointFindNearest(pEntity, 50.0, -1);
		
		if (waypoint1 == -1)
		{
			// play "cancelled" sound...
			if(CVAR_BOOL(jb_wpsound))
				EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_moveselect.wav", 1.0,ATTN_NORM, 0, 100);
			
			return;
		}
		
		// play "start" sound...
		if(CVAR_BOOL(jb_wpsound))
			EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_hudoff.wav", 1.0,ATTN_NORM, 0, 100);
		
		return;
	}
	
	if (cmd == 2)  // assign dest of path and make path
	{
		waypoint2 = WaypointFindNearest(pEntity, 50.0, -1);
		
		if ((waypoint1 == -1) || (waypoint2 == -1))
		{
			// play "error" sound...
			if(CVAR_BOOL(jb_wpsound))
				EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_denyselect.wav", 1.0,ATTN_NORM, 0, 100);
			
			return;
		}
		
		WaypointDeletePath(waypoint1, waypoint2);
		
		// play "done" sound...
		if(CVAR_BOOL(jb_wpsound))
			EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, "common/wpn_hudon.wav", 1.0,ATTN_NORM, 0, 100);
	}
}

bool WaypointLoad(edict_t *pEntity, const char *szDir)
{
	if(pEntity&&CVAR_BOOL(jb_wpsound))
		CLIENT_COMMAND(pEntity,"speak \"loading\"\n");
	
	char mapname[64];
	char filename[256];
	char dirname[80];
	WAYPOINT_HDR header;
	int index, i;
	short int num;
	short int path_index;
	
	if(!szDir || !strlen(szDir)){
		snprintf(mapname, sizeof(mapname), "%s.wpj", STRING(gpGlobals->mapname));
		WaypointGetDir(mapname,dirname);
	}
	else{
		UTIL_BuildFileName(dirname, sizeof(dirname), "joebot/wpjs/%s/", szDir);
	}
	
	snprintf(filename, sizeof(filename), "%s/%s.wpj", dirname, STRING(gpGlobals->mapname));
	
	if (IS_DEDICATED_SERVER())
		LOG_MESSAGE(PLID, "Loading waypoint file: %s", filename);
	
	FILE *bfp = fopen(filename, "rb");
	
	// if file exists, read the waypoint structure from it
	if (bfp != NULL)
	{
		fread(&header, sizeof(header), 1, bfp);
		
		header.filetype[7] = 0;
		if (FStrEq(header.filetype, _WPFILEDESC))
		{
			CVAR_SET_STRING("jb_wpfilename", UTIL_VarArgs("%s,v%d", filename, header.waypoint_file_version));

			if (header.waypoint_file_version == 1)
			{
				LOG_MESSAGE(PLID, "Loading waypoint file type 1");
				/*if (pEntity)
				UTIL_ConsoleMessage(pEntity, "Incompatible Joe bot waypoint file version!\nWaypoints not loaded!\n");
				
				  fclose(bfp);
				return FALSE;*/
				
				header.mapname[31] = 0;
				
				if (!FStrEq(header.mapname, STRING(gpGlobals->mapname)))
				{
					if (pEntity)
					{
						UTIL_ConsoleMessage(pEntity, "WARNING: %s Joebot waypoints are originally not for this map!\n", filename);
					}
				}
				WaypointInit();  // remove any existing waypoints
				
				for (i=0; i < header.number_of_waypoints; i++)
				{
					fread(&waypoints[i], sizeof(waypoints[0]), 1, bfp);
					if(waypoints[i].origin.Length() == 0){
						waypoints[i].flags |= W_FL_DELETED;
					}
					num_waypoints++;
				}
				
				// read and add waypoint paths...
				for (index=0; index < num_waypoints; index++)
				{
					// read the number of paths from this node...
					fread(&num, sizeof(num), 1, bfp);
					
					for (i=0; i < num; i++)
					{
						fread(&path_index, sizeof(path_index), 1, bfp);
						
						WaypointAddPath(index, path_index);
					}
				}
			}
			else if (header.waypoint_file_version == 2){
				header.mapname[31] = 0;
				
				if (!FStrEq(header.mapname, STRING(gpGlobals->mapname)))
				{
					if (pEntity)
					{
						UTIL_ConsoleMessage(pEntity, "WARNING: %s JoeBOT waypoints are not for this map!\n", filename);
					}
				}
				WaypointInit();  // remove any existing waypoints
				
				for (i=0; i < header.number_of_waypoints; i++)
				{
					fread(&waypoints[i], sizeof(waypoints[0]), 1, bfp);
					num_waypoints++;
				}
				
				// read and add waypoint paths...
				for (index=0; index < num_waypoints; index++)
				{
					// read the number of paths from this node...
					fread(&num, sizeof(num), 1, bfp);
					
					for (i=0; i < num; i++)
					{
						fread(&path_index, sizeof(path_index), 1, bfp);
						
						WaypointAddPath(index, path_index);
					}
				}
				short int inumberofadv;
				ADV_WPRecMove temp;
				
				fread(&inumberofadv, sizeof(short int), 1, bfp);
				
				for (index=0; index < inumberofadv; index++)
				{
					fread(&temp, sizeof(ADV_WPRecMove), 1, bfp);
					WaypointAddADV(temp.iIndexStartWP,temp.iIndexEndWP,&temp);
				}
			}
			
			g_waypoint_paths = TRUE;  // keep track so path can be freed
		}
		else
		{
			if (pEntity)
			{
				UTIL_ConsoleMessage(pEntity, "%s is not a JoeBOT waypoint file!\n", filename);
			}
			
			fclose(bfp);
			return FALSE;
		}
		
		fclose(bfp);
		
		WaypointRouteInit();
	}
	else
	{
		if (pEntity)
		{
			UTIL_ConsoleMessage(pEntity, "Waypoint file %s does not exist!\n", filename);
		}
		
		if (IS_DEDICATED_SERVER())
			LOG_MESSAGE(PLID, "waypoint file %s not found!", filename);
		
		LOG_DEBUG(UTIL_VarArgs("WaypointLoad: joebot waypoint file \"%s\" not found", filename));

		return FALSE;
	}
	
	return TRUE;
}

bool WaypointSave(edict_t *pEntity,const char *szDir)
{	
	char filename[256];
	char mapname[64];
	char dirname[80];
	WAYPOINT_HDR header;
	int index, i;
	short int num;
	PATH *p;
	ADV_WPRecMove *pRec;
	
	strcpy(header.filetype, _WPFILEDESC);
	
	header.waypoint_file_version = WAYPOINT_VERSION;
	
	header.waypoint_file_flags = 0;  // not currently used
	
	header.number_of_waypoints = num_waypoints;
	
	memset(header.mapname, 0, sizeof(header.mapname));
	strncpy(header.mapname, STRING(gpGlobals->mapname), 31);
	header.mapname[31] = 0;
	
	if(szDir && strlen(szDir)){
		UTIL_BuildFileName(dirname, sizeof(dirname), "joebot/wpjs/%s/", szDir);
	}
	else{
		snprintf(mapname, sizeof(mapname), "%s.wpj", STRING(gpGlobals->mapname));
		WaypointGetDir(mapname,dirname);
	}
	
	snprintf(filename, sizeof(filename), "%s/%s.wpj", dirname, header.mapname);
	
	FILE *bfp = fopen(filename, "wb");
	if(bfp){
		// write the waypoint header to the file...
		fwrite(&header, sizeof(header), 1, bfp);
		
		// write the waypoint data to the file...
		for (index=0; index < num_waypoints; index++)
		{
			fwrite(&waypoints[index], sizeof(waypoints[0]), 1, bfp);
		}
		
		// save the waypoint paths...
		for (index=0; index < num_waypoints; index++)
		{
			/*------------------------- path stuff ---------------------------------------*/
			// count the number of paths from this node...
			
			p = paths[index];
			num = 0;
			
			while (p != NULL)
			{
				i = 0;
				
				while (i < MAX_PATH_INDEX)
				{
					if (p->index[i] != -1)
						num++;  // count path node if it's used
					
					i++;
				}
				
				p = p->next;  // go to next node in linked list
			}
			
			fwrite(&num, sizeof(num), 1, bfp);  // write the count
			
			// now write out each path index...
			
			p = paths[index];
			
			while (p != NULL)
			{
				i = 0;
				
				while (i < MAX_PATH_INDEX)
				{
					if (p->index[i] != -1)  // save path node if it's used
						fwrite(&p->index[i], sizeof(p->index[0]), 1, bfp);
					
					i++;
				}
				
				p = p->next;  // go to next node in linked list
			}
		}
		// save the waypoint adv movements
		num = 0;
		for (index=0; index < num_recadvm; index++)
		{
			/*------------------------- ADVM stuff ---------------------------------------*/
			// count the number of paths from this node...
			
			pRec = WPJRec[index];
			
			if(pRec)
				num++;  // count wprec node if it's used
		}
		fwrite(&num, sizeof(num), 1, bfp);  // write the count
		
		// now write out each adv movement index...
		for (index=0; index < num_recadvm; index++)
		{
			// write stuff to file
			
			pRec = WPJRec[index];
			
			if(pRec){
				fwrite(pRec, sizeof(ADV_WPRecMove), 1, bfp);
			}
		}
		
		fclose(bfp);

		if(pEntity
			&&CVAR_BOOL(jb_wpsound))
			CLIENT_COMMAND(pEntity,"speak \"save\"\n");
		return true;
	}
	else{
		if (IS_DEDICATED_SERVER())
			LOG_MESSAGE(PLID, "Failed saving %s",filename);
		else{
			UTIL_ConsoleMessage(NULL, "Failed saving %s\n",filename);
		}
		return false;
	}
}
/*bool WaypointSave(const char *szDir)
{
char filename[256];
char mapname[64];
char dirname[80];
WAYPOINT_HDR header;
int index, i;
short int num;
PATH *p;

  strcpy(header.filetype, "Joe_bot");
  
	header.waypoint_file_version = WAYPOINT_VERSION;
	
	  header.waypoint_file_flags = 0;  // not currently used
	  
		header.number_of_waypoints = num_waypoints;
		
		  memset(header.mapname, 0, sizeof(header.mapname));
		  strncpy(header.mapname, STRING(gpGlobals->mapname), 31);
		  header.mapname[31] = 0;
		  
			strcpy(mapname, STRING(gpGlobals->mapname));
			strcat(mapname, ".wpj");
			
			  if(szDir && strlen(szDir)){
			  #ifdef _WIN32
			  strcpy(dirname, "cstrike\\joebot\\wpjs\\");
			  #else
			  strcpy(dirname, "cstrike/joebot/wpjs/");
			  #endif
			  strcat(dirname, szDir);
			  #ifdef _WIN32
			  strcat(dirname, "\\");
			  #else
			  strcat(dirname, "/");
			  #endif
			  }
			  else{
			  WaypointGetDir(mapname,dirname);
			  }
			  
				strcpy(filename, dirname);
				strcat(filename, header.mapname);
				strcat(filename, ".wpj");
				
				  FILE *bfp = fopen(filename, "wb");
				  if(bfp){
				  // write the waypoint header to the file...
				  fwrite(&header, sizeof(header), 1, bfp);
				  
					// write the waypoint data to the file...
					for (index=0; index < num_waypoints; index++)
					{
					fwrite(&waypoints[index], sizeof(waypoints[0]), 1, bfp);
					}
					
					  // save the waypoint paths...
					  for (index=0; index < num_waypoints; index++)
					  {
					  // count the number of paths from this node...
					  
						p = paths[index];
						num = 0;
						
						  while (p != NULL)
						  {
						  i = 0;
						  
							while (i < MAX_PATH_INDEX)
							{
							if (p->index[i] != -1)
							num++;  // count path node if it's used
							
							  i++;
							  }
							  
								p = p->next;  // go to next node in linked list
								}
								
								  fwrite(&num, sizeof(num), 1, bfp);  // write the count
								  
									// now write out each path index...
									
									  p = paths[index];
									  
										while (p != NULL)
										{
										i = 0;
										
										  while (i < MAX_PATH_INDEX)
										  {
										  if (p->index[i] != -1)  // save path node if it's used
										  fwrite(&p->index[i], sizeof(p->index[0]), 1, bfp);
										  
											i++;
											}
											
											  p = p->next;  // go to next node in linked list
											  }
											  }
											  
												fclose(bfp);
												return true;
												}
												else{
												return false;
												}
												}
*/

bool WaypointReachable(Vector v_src, Vector v_dest, edict_t *pEntity)
{
	TraceResult tr;
	bool bVisibleEx;
	float curr_height, last_height;
	
	float distance = (v_dest - v_src).Length();
	
	// is the destination close enough?
	if (distance < REACHABLE_RANGE)
	{
		// check if this waypoint is "visible"...
		
		/*if(pEntity){
		UTIL_TraceLine( v_src, v_dest, ignore_monsters,
		pEntity->v.pContainingEntity, &tr );
		}
		else{
		UTIL_TraceLine( v_src, v_dest, ignore_monsters,
		0, &tr );
	}*/
		
		if(pEntity){
			bVisibleEx = FVisibleEx(v_src,v_dest,true,true,true,true,pEntity->v.pContainingEntity);
		}
		else{
			bVisibleEx = FVisibleEx(v_src,v_dest,true,true,true,true,0);
		}
		
		
		// if waypoint is visible from current position (even behind head)...
		//if (tr.flFraction >= 1.0)
		if (bVisibleEx)
		{
			// check for special case of both waypoints being underwater...
			if ((POINT_CONTENTS( v_src ) == CONTENTS_WATER) &&
				(POINT_CONTENTS( v_dest ) == CONTENTS_WATER))
			{
				return TRUE;
			}
			
			// check for special case of waypoint being suspended in mid-air...
			
			// is dest waypoint higher than src? (45 is max jump height)
			if (v_dest.z > (v_src.z + 45.0))
			{
				Vector v_new_src = v_dest;
				Vector v_new_dest = v_dest;
				
				v_new_dest.z = v_new_dest.z - 50;  // straight down 50 units
				
				if(pEntity){
					UTIL_TraceLine(v_new_src, v_new_dest, dont_ignore_monsters,
						pEntity->v.pContainingEntity, &tr);
				}
				else{
					UTIL_TraceLine(v_new_src, v_new_dest, dont_ignore_monsters,
						0, &tr);
				}
				
				// check if we didn't hit anything, if not then it's in mid-air
				if (tr.flFraction >= 1.0)
				{
					return FALSE;  // can't reach this one
				}
			}
			
			// check if distance to ground increases more than jump height
			// at points between source and destination...
			
			Vector v_direction = (v_dest - v_src).Normalize();  // 1 unit long
			Vector v_check = v_src;
			Vector v_down = v_src;
			
			v_down.z = v_down.z - 1000.0;  // straight down 1000 units
			
			if(pEntity){
				UTIL_TraceLine(v_check, v_down, ignore_monsters,
					pEntity->v.pContainingEntity, &tr);
			}
			else{
				UTIL_TraceLine(v_check, v_down, ignore_monsters,
					0, &tr);
			}
			
			last_height = tr.flFraction * 1000.0;  // height from ground
			
			distance = (v_dest - v_check).Length();  // distance from goal
			
			while (distance >= 10.0)
			{
				// move 10 units closer to the goal...
				v_check = v_check + (v_direction * 10.0);
				
				v_down = v_check;
				v_down.z = v_down.z - 1000.0;  // straight down 1000 units
				
				if(pEntity){
					UTIL_TraceLine(v_check, v_down, ignore_monsters,
						pEntity->v.pContainingEntity, &tr);
				}
				else{
					UTIL_TraceLine(v_check, v_down, ignore_monsters,
						0, &tr);
				}
				
				curr_height = tr.flFraction * 1000.0;  // height from ground
				
				// is the difference in the last height and the current height
				// higher that the jump height?
				if (fabs(last_height - curr_height) > 45.0)
				{
					// can't get there from here...
					return FALSE;
				}
				
				last_height = curr_height;
				
				distance = (v_dest - v_check).Length();  // distance from goal
			}
			
			return TRUE;
		}
	}
	
	return FALSE;
}

// find the nearest reachable waypoint
int WaypointFindReachable(edict_t *pEntity, float range, int team)
{
	int i, min_index;
	float distance;
	float min_distance;
	TraceResult tr;
	
	if(!pEntity)
		return -1;
	
	// find the nearest waypoint...
	
	min_distance = 9999999.0;
	
	for (i=0; i < num_waypoints; i++)
	{
		if (waypoints[i].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[i].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((team != -1) && (waypoints[i].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[i].flags & W_FL_TEAM) != team))
			continue;
		
		distance = (waypoints[i].origin - pEntity->v.origin).Length();
		
		if (distance < min_distance)
		{
			// if waypoint is visible from current position (even behind head)...
			//UTIL_TraceLine( pEntity->v.origin + pEntity->v.view_ofs, waypoints[i].origin,ignore_monsters, pEntity->v.pContainingEntity, &tr );
			UTIL_TraceHull( pEntity->v.origin + pEntity->v.view_ofs, waypoints[i].origin,
				ignore_monsters,head_hull, pEntity->v.pContainingEntity, &tr );
			
			if (tr.flFraction >= 1.0)
			{
				if (WaypointReachable(pEntity->v.origin, waypoints[i].origin, pEntity))
				{
					min_index = i;
					min_distance = distance;
				}
			}
		}
	}
	
	// if not close enough to a waypoint then just return
	if (min_distance > range)
		return -1;
	
	return min_index;
}


void WaypointPrintInfo(edict_t *pEntity, char *szText)		// szText is max 1000
{
	char msg[80];
	int index;
	int flags;
	long lNum=0;
	
	index = 0;
	while (index < num_waypoints)
	{
		if ((waypoints[index].flags & W_FL_DELETED) == W_FL_DELETED){
		}
		else{
			lNum++;
		}
		
		index++;
	}
	
	// find the nearest waypoint...
	index = WaypointFindNearest(pEntity, 50.0, -1);
	
	if (index == -1)
		return;
	
	snprintf(msg,sizeof(msg),"Waypoint %d of %d total (%li)\n\n", index, num_waypoints,lNum);
	if(szText){
		if(strlen(szText) + strlen(msg) < 1000){
			strcat(szText,msg);
		}
	}
	else
		UTIL_ConsoleMessage(pEntity, msg);
	
	
	flags = waypoints[index].flags;
	
	if (flags & W_FL_TEAM_SPECIFIC)
	{
		if ((flags & W_FL_TEAM) == 0)
            strcpy(msg, "Waypoint is for TEAM 1\n");
		else if ((flags & W_FL_TEAM) == 1)
            strcpy(msg, "Waypoint is for TEAM 2\n");
		else if ((flags & W_FL_TEAM) == 2)
            strcpy(msg, "Waypoint is for TEAM 3\n");
		else if ((flags & W_FL_TEAM) == 3)
            strcpy(msg, "Waypoint is for TEAM 4\n");
		
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	
	if (flags & W_FL_CROUCH){
		snprintf(msg,sizeof(msg),"This is a crouch waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_LIFT){
		snprintf(msg,sizeof(msg),"Bot will wait for lift before approaching\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_LADDER){
		snprintf(msg,sizeof(msg),"This waypoint is on a ladder\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_HEALTH){
		snprintf(msg,sizeof(msg),"There is health near this waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_ARMOR){
		snprintf(msg,sizeof(msg),"There is armor near this waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_AMMO){
		snprintf(msg,sizeof(msg),"There is ammo near this waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_SNIPER){
		snprintf(msg,sizeof(msg),"This is a sniper waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_FLAG){
		snprintf(msg,sizeof(msg),"This is a hostage/bomb/viprescue waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_FLAG_GOAL){
		snprintf(msg,sizeof(msg),"This is a hostage rescue waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_VISIT){
		snprintf(msg,sizeof(msg),"This is a visit waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_DONTAVOID){
		snprintf(msg,sizeof(msg),"This is a dontavoid waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_RESET){
		snprintf(msg,sizeof(msg),"This is a reset waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_AIMING){
		snprintf(msg,sizeof(msg),"This is a aiming waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_DAF){
		snprintf(msg,sizeof(msg),"This is a dontavoidfall waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_JUMP){
		snprintf(msg,sizeof(msg),"This is a jump waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	if (flags & W_FL_BLOCKHOSTAGE){
		snprintf(msg,sizeof(msg),"This is a blockhostage waypoint\n");
		if(szText){
			if(strlen(szText) + strlen(msg) < 512){
				strcat(szText,msg);
			}
		}
		else
			UTIL_ConsoleMessage(pEntity, msg);
	}
	
	snprintf(msg,sizeof(msg),"\nKill/Killed : %3li/%3li\n",WPStat.d.FItem[index].lKill,WPStat.d.FItem[index].lKilled);
	if(szText){
		if(strlen(szText) + strlen(msg) < 512){
			strcat(szText,msg);
		}
	}
	else
		UTIL_ConsoleMessage(pEntity, msg);
	
	snprintf(msg,sizeof(msg),"fADEn : %3.0f\n",WPStat.d.FItem[index].fADEn);
	if(szText){
		if(strlen(szText) + strlen(msg) < 512){
			strcat(szText,msg);
		}
	}
	else
		UTIL_ConsoleMessage(pEntity, msg);
}

void WaypointPrintTest(edict_t *pEntity){
	int i;
	
	UTIL_ConsoleMessage(pEntity, "Running Joebot waypoint analysis ....\n");
	UTIL_ConsoleMessage(pEntity, "Total of %i waypoints\n",num_waypoints);
	
	int iTeam = 2;
	
	int iDeleted = 0;
	int iWTeam[3] = {0,0,0};
	int iAiming[3] = {0,0,0};
	int iFlag[3] = {0,0,0};
	int iFlagGoal[3] = {0,0,0};
	int iSniper[3] = {0,0,0};
	int iVisit[3] = {0,0,0};
	int iLadder[3] = {0,0,0};
	int iCrouch[3] = {0,0,0};
	
	for (i=0; i < num_waypoints; i++){
		if(waypoints[i].flags & W_FL_DELETED){
			iDeleted ++;
			continue;
		}
		if(waypoints[i].flags & W_FL_TEAM_SPECIFIC){
			iTeam = waypoints[i].flags & W_FL_TEAM;
		}
		else{
			iTeam = 2;
		}
		
		iWTeam[iTeam] ++;
		
		if(waypoints[i].flags & W_FL_AIMING){
			iAiming[iTeam] ++;
			continue;
		}
		if(waypoints[i].flags & W_FL_FLAG){
			iFlag[iTeam] ++;
		}
		if(waypoints[i].flags & W_FL_FLAG_GOAL){
			iFlagGoal[iTeam] ++;
		}
		if(waypoints[i].flags & W_FL_VISIT){
			iVisit[iTeam] ++;
		}
		if(waypoints[i].flags & W_FL_SNIPER){
			iSniper[iTeam] ++;
		}
		if(waypoints[i].flags & W_FL_LADDER){
			iLadder[iTeam] ++;
		}
		if(waypoints[i].flags & W_FL_CROUCH){
			iCrouch[iTeam] ++;
		}
	}
	
	UTIL_ConsoleMessage(pEntity, "Total of %i deleted waypoints\n",iDeleted);
	
	for(i=0;i < 3;i++){
		switch(i){
		case 0:
			UTIL_ConsoleMessage(pEntity, "--- Team specific wps for terrors, %i\n",iWTeam[i]);
			break;
		case 1:
			UTIL_ConsoleMessage(pEntity, "--- Team specific wps for counter terrorists, %i\n",iWTeam[i]);
			break;
		case 2:
			UTIL_ConsoleMessage(pEntity, "--- Non team specific waypoints, %i\n",iWTeam[i]);
			break;
		}
		UTIL_ConsoleMessage(pEntity, "%i Aiming waypoints\n",iAiming[i]);
		UTIL_ConsoleMessage(pEntity, "%i Flag waypoints (i.e. hostage, bomb, etc.)\n",iFlag[i]);
		UTIL_ConsoleMessage(pEntity, "%i Flag Goal waypoints (i.e. rescue zone )\n",iFlagGoal[i]);
		UTIL_ConsoleMessage(pEntity, "%i Visit waypoints\n",iVisit[i]);
		UTIL_ConsoleMessage(pEntity, "%i Sniper waypoints\n",iSniper[i]);
		UTIL_ConsoleMessage(pEntity, "%i Ladder waypoints\n",iLadder[i]);
		UTIL_ConsoleMessage(pEntity, "%i Crouch waypoints\n",iCrouch[i]);
	}
	bool bComment = false;
	
	UTIL_ConsoleMessage(pEntity, "--- Comments: \n");
	if(!strncmp(STRING(gpGlobals->mapname),"cs",sizeof(char) * 2)
		&& iFlag[2] == 0){
		UTIL_ConsoleMessage(pEntity, "Is there no hostage to rescue ?\n");
		bComment = true;
	}
	if(!strncmp(STRING(gpGlobals->mapname),"cs",sizeof(char) * 2)
		&& iFlag[2] == 1){
		UTIL_ConsoleMessage(pEntity, "Are all hostages in one place ?\n");
		bComment = true;
	}
	if(!strncmp(STRING(gpGlobals->mapname),"cs",sizeof(char) * 2)
		&& iFlagGoal[2] == 0){
		UTIL_ConsoleMessage(pEntity, "Is there no rescue zone ?\n");
		bComment = true;
	}
	if(!strncmp(STRING(gpGlobals->mapname),"de",sizeof(char) * 2)
		&& iFlag[2] == 0){
		UTIL_ConsoleMessage(pEntity, "Are there no bomb targets ?\n");
		bComment = true;
	}
	if(!strncmp(STRING(gpGlobals->mapname),"de",sizeof(char) * 2)
		&& iFlag[2] == 0){
		UTIL_ConsoleMessage(pEntity, "Normally there are 2 bomb targets !\n");
		bComment = true;
	}
	if(!strncmp(STRING(gpGlobals->mapname),"as",sizeof(char) * 2)
		&& iFlag[2]+iFlag[1]+iFlag[0] == 0){
		UTIL_ConsoleMessage(pEntity, "There has to be at least one rescue point !\n");
		bComment = true;
	}
	
	if(strncmp(STRING(gpGlobals->mapname),"as",sizeof(char) * 2)
		&& ((iSniper[CS_TEAM_CT] == 0 && iSniper[2] == 0) || (iSniper[CS_TEAM_TE] != 0 && iSniper[2] == 0))  ){
		UTIL_ConsoleMessage(pEntity, "It would be better if u would add some sniper spots for both teams.\n");
		bComment = true;
	}
	
	if(!bComment){
		UTIL_ConsoleMessage(pEntity, "none\n");
	}
}

bool IsConnected(int i1,int i2){
	PATH *p;
	int pi;
	
	p=paths[i1];
	while(p){
		pi=0;
		while(pi<MAX_PATH_INDEX){
			if(p->index[pi] != -1){
				if(i2 == p->index[pi])
					return true;
			}
			pi++;
		}
		p=p->next;
	}
	return false;
}

bool EdictJumps(int i){
	edict_t *pEnt = INDEXENT(i + 1);
	if(pEnt->v.button&IN_DUCK){
		if(AWP_ED[i].fHeight > 18.1f)
			return true;
	}
	else{
		if(AWP_ED[i].fHeight > 36.1f)
			return true;
	}
	return false;
}

void WaypointThink(edict_t *pEntity)
{
	float dfDistance, dsDistance, min_distance,fDirC;
	Vector start, end,endteam,eend;
	int i, index;
	int p;
	static bool bwpon;
	
	if (CVAR_BOOL(jb_wpauto))  // is auto waypoint on?
	{
		edict_t *pEnt;
		int iNearL,iNear,iAdd=-1;
		TraceResult tr;
		
		for (i = 0; i < gpGlobals->maxClients; i++){
			pEnt = INDEXENT(i + 1);
			
			if(!CVAR_BOOL(jb_wpautobots)){		// only humans can autowaypoint
				if(UTIL_GetBotIndex(pEnt) == -1){
					continue;
				}
			}
			
			// skip invalid players
			if ((pEnt) && (!pEnt->free)&&(pEntity==pEnt))
			{
				// skip this player if not alive (i.e. dead or dying)
				if (!IsAlive(pEnt))
					continue;
				
				iAdd = -1;
				iNear = WaypointFindNearest(pEnt->v.origin,pEnt,5000,UTIL_GetTeam(pEnt));
				
				if((pEnt->v.movetype == MOVETYPE_FLY) != (AWP_ED[i].movetype== MOVETYPE_FLY)
					&&!bRec[i]){
					// change in movetype, i.e. went on a ladder p.ex
					if(pEnt->v.movetype == MOVETYPE_FLY){	// went on ladder
						iNearL = WaypointFindNearestGoal(pEnt->v.origin,pEnt,50,UTIL_GetTeam(pEnt),W_FL_LADDER);
						if(iNearL==-1){
							iAdd = WaypointAdd(pEnt);
						}
					}
					else{									// went off ladder
						if((waypoints[iNear].origin-pEnt->v.origin).Length() > 50)
							iAdd = WaypointAdd(pEnt);
					}
				}
				if(pEnt->v.button & IN_JUMP && CVAR_BOOL(jb_wpautojump) && !bRec[i]){
					AWP_ED[i].fLJumpTime = gpGlobals->time;
					iNearL = WaypointFindNearestGoal(pEnt->v.origin,pEnt,50,UTIL_GetTeam(pEnt),W_FL_JUMP);

					AWP_ED[i].VLastJumpStartOrigin	= pEnt -> v.origin;		// store begin of jump
					AWP_ED[i].iLastJumpStartWPIndex	= iNearL;				// and evtl. the nearest wp
					AWP_ED[i].bLastJumpStartDuck = (pEnt->v.button & IN_DUCK)!=0;
					// start recording
					bRec[i] = true;
					fStartRec[i] = gpGlobals->time;
					ADVL[i].iIndexStartWP = (iNearL==-1?iAdd:iNearL);		// set start wp
					ADVL[i].iNum = 0;
				}
				
				if(bRec[i]){		// i.e. is recording
					// record frames
					if(!ADVL[i].iNum
						|| ADVL[i].Rec[ADVL[i].iNum-1].fTime < gpGlobals->time - fStartRec[i] - 1.f/_MAXRECPERSEC){
						// it's the first recorded frame or some time is over and u r allowed to record the next frame
						
						ADVL[i].Rec[ADVL[i].iNum].fTime			= gpGlobals->time - fStartRec[i];
						ADVL[i].Rec[ADVL[i].iNum].lButton		= pEnt->v.button;
						ADVL[i].Rec[ADVL[i].iNum].v_angles		= pEnt->v.angles;
						ADVL[i].Rec[ADVL[i].iNum].v_origin		= pEnt->v.origin;
						ADVL[i].Rec[ADVL[i].iNum].v_v_angle		= pEnt->v.v_angle;
						ADVL[i].Rec[ADVL[i].iNum].v_velocity	= pEnt->v.velocity;
						
						ADVL[i].iNum ++;
#ifdef _DEBUG
						FILE *fhds;fhds=fopen("rec.txt","a");
						if(fhds)fprintf(fhds,"%i - %f - %f\n",ADVL[i].iNum,gpGlobals->time,ADVL[i].Rec[ADVL[i].iNum-1].fTime);
						if(fhds)fclose(fhds);
#endif
					}
					// check if recording has to be finished
					if(ADVL[i].iNum >= _MAXRECITEMS){
						bStopRec[i] = true;			// set flag to stop recording
					}
					else if(!EdictJumps(i)
						&& ADVL[i].Rec[0].fTime+.3 < gpGlobals->time-fStartRec[i]){
						bStopRec[i] = true;			// set flag to stop recording when edict reached the ground
					}
				}
				
				if(bStopRec[i]
					&&bRec[i]){
					// check if it's useful to use jump waypoints here ... is a jump waypoint really needed ?
					TraceResult tr;
					Vector VStart,VEnd;
					
					if(AWP_ED[i].bLastJumpStartDuck){
						VStart = AWP_ED[i].VLastJumpStartOrigin - Vector(0,0,7);
					}
					else{
						VStart = AWP_ED[i].VLastJumpStartOrigin - Vector(0,0,24);
					}
					if(pEnt->v.button & IN_DUCK){					// check if bot is ducked now
						VEnd = pEnt -> v.origin - Vector(0,0,7);
					}
					else{
						VEnd = pEnt -> v.origin - Vector(0,0,24);
					}
					UTIL_TraceLine(VStart,VEnd,ignore_monsters,dont_ignore_glass,pEnt,&tr);
					if(!CVAR_BOOL(jb_wpautojumptest)
						|| tr.flFraction != 1.0
						|| !WaypointReachable(AWP_ED[i].VLastJumpStartOrigin,pEnt->v.origin,pEnt)){		// i.e. line is fract and there is the need of using a jump wp or the location isn't reachable
						// add wp at start
						if(AWP_ED[i].iLastJumpStartWPIndex == -1){
							float fMin = 0;
							float pval = jb_wpautopath->value;
							CVAR_SET_FLOAT("jb_wpautopath", 0);
							if((iAdd=WaypointFindNearest(pEnt,20,UTIL_GetTeam(pEnt),fMin,false,false,false)) != -1){
								waypoints[iAdd].flags|= W_FL_JUMP;
							}
							else{
								iAdd = WaypointAdd(AWP_ED[i].VLastJumpStartOrigin,W_FL_JUMP);
							}
							
							// cause wpadd traces down to the ground, set the origin a second time
							waypoints[iAdd].origin = AWP_ED[i].VLastJumpStartOrigin;
							CVAR_SET_FLOAT("jb_wpautopath", pval);
						}
						else{
							waypoints[AWP_ED[i].iLastJumpStartWPIndex].flags|= W_FL_JUMP;
						}
						
						ADVL[i].iIndexStartWP = AWP_ED[i].iLastJumpStartWPIndex==-1?iAdd:AWP_ED[i].iLastJumpStartWPIndex;
						
						if(AWP_ED[i].iLastWP!=-1){		// create a path
							if(!IsConnected(AWP_ED[i].iLastWP,ADVL[i].iIndexStartWP)){
								WaypointAddPath(AWP_ED[i].iLastWP,ADVL[i].iIndexStartWP);
							}
						}
						
						AWP_ED[i].iLastWP = ADVL[i].iIndexStartWP;
						
						// look for a near waypoint or add one for if needed
						iNearL = WaypointFindNearest(pEnt->v.origin,pEnt,30,UTIL_GetTeam(pEnt));
						if(iNearL==-1){
							float pval = jb_wpautopath->value;
							CVAR_SET_FLOAT("jb_wpautopath", 0);
							iAdd = WaypointAdd(pEnt);
							CVAR_SET_FLOAT("jb_wpautopath", pval);
							
							iNearL = iAdd;
						}
						
						ADVL[i].iIndexEndWP = iNearL;
						
						if(CVAR_BOOL(jb_wpautoadvanced))
							WaypointAddADV(ADVL[i].iIndexStartWP,ADVL[i].iIndexEndWP,&ADVL[i]);
					}
					else{
						//WaypointAdd(AWP_ED[i].VLastJumpStartOrigin,0);
						//WaypointAdd(pEnt);
					}
					bRec[i] = bStopRec[i] = false;
				}
				
				if(AWP_ED[i].iLastWP != -1){
					if(!FVisible(waypoints[AWP_ED[i].iLastWP].origin,pEnt)){			// add waypoint when running around corners
						iNearL = WaypointFindNearest(pEnt->v.origin,pEnt,75,UTIL_GetTeam(pEnt));
						if(iNearL==-1){
							iAdd = WaypointAdd(pEnt);
							if(iAdd!=-1)
								waypoints[iAdd].origin = AWP_ED[i].VLastOrigin;
						}
					}
				}
				
				if(!bRec[i]){
					iNear = WaypointFindNearest(pEnt->v.origin,pEnt,5000,UTIL_GetTeam(pEnt));
					if(pEnt->v.movetype == MOVETYPE_FLY){
						if((waypoints[iNear].origin-pEnt->v.origin).Length() > _AWP_LNWPDIST)
							iAdd = WaypointAdd(pEnt);
					}
					else{
						if((waypoints[iNear].origin-pEnt->v.origin).Length() > _AWP_NNWPDIST)
							iAdd = WaypointAdd(pEnt);
					}
				}
				
				AWP_ED[i].movetype = pEnt->v.movetype;
				AWP_ED[i].lButton = pEnt->v.button;
				AWP_ED[i].fLTime = gpGlobals->time;
				AWP_ED[i].lFlags = pEnt->v.flags;
				AWP_ED[i].VLastOrigin = pEnt->v.origin;
				UTIL_TraceLine(pEntity->v.origin,pEntity->v.origin-Vector(0,0,1000),ignore_monsters,pEntity,&tr);
				AWP_ED[i].fHeight = tr.flFraction * 1000.0f;
				
				if(iAdd !=-1){
					if(AWP_ED[i].iLastWP>0){
						if(!IsConnected(AWP_ED[i].iLastWP,iAdd)){
							WaypointAddPath(AWP_ED[i].iLastWP,iAdd);
						}
						if(waypoints[AWP_ED[i].iLastWP].flags&W_FL_LADDER&&waypoints[iAdd].flags&W_FL_FLAG){
							if(!IsConnected(iAdd,AWP_ED[i].iLastWP)){
								WaypointAddPath(iAdd,AWP_ED[i].iLastWP);
							}
						}
					}
					AWP_ED[i].iLastWP = iAdd;
				}
				else{
					iNear = WaypointFindNearest(pEnt->v.origin,pEnt,50,UTIL_GetTeam(pEnt));
					if(AWP_ED[i].iLastWP>0 && iNear!=-1){
						if(iNear != AWP_ED[i].iLastWP){
							if(!IsConnected(AWP_ED[i].iLastWP,iNear)
								&& FVisible(waypoints[AWP_ED[i].iLastWP].origin,pEnt))
								WaypointAddPath(AWP_ED[i].iLastWP,iNear);
						}
					}
					if(iNear !=-1){
						AWP_ED[i].iLastWP = iNear;
					}
				}
			}
		}
	}
	
	min_distance = 9999999.0;
	float fAddProp,fAddProp2;
	float fTeamr,fTeamg;
	int iWidth = 30;
	PATH *ppath;
	bool b1ND;
	
	if(bwpon != CVAR_BOOL(jb_wp)){
		for (i=0; i < num_waypoints; i++)
			wp_display_time[i] = 0;
	}
	
	if (CVAR_BOOL(jb_wp) && g_b5th)  // display the waypoints if turned on...
	{
		// display information :
		if(listenserver_edict && listenserver_edict == pEntity){
			char szWPInfo[1000];
			strcpy(szWPInfo,"Waypoint Information :\n\n");
			WaypointPrintInfo(pEntity,szWPInfo);
			hudtextparms_t message_params;

			szWPInfo[510] = 0;
			
			message_params.x = 0;
			message_params.y = 0;
			message_params.effect = 0;
			message_params.r1 = 0;
			message_params.g1 = 255;
			message_params.b1 = 0;
			message_params.a1 = 1;
			message_params.r2 = message_params.r1;
			message_params.g2 = message_params.g1;
			message_params.b2 = message_params.b1;
			message_params.a2 = 1;
			message_params.fadeinTime = 0.00;
			message_params.fadeoutTime = 0;
			message_params.holdTime = gf_5thd+0.02f;
			message_params.fxTime = 0;
			message_params.channel = 4;
			
			UTIL_ShowText(listenserver_edict,message_params,szWPInfo);
		}
		//bool bnot;
		bwpon = CVAR_BOOL(jb_wp);
		b1ND = 0;
		if(pEntity == pEdictPlayer)
			for (i=0; i < num_waypoints; i++)
			{
				if ((waypoints[i].flags & W_FL_DELETED) == W_FL_DELETED)
					continue;

				/*if( WPStat.d.FItem[i].fADVWp < 700 || WPStat.d.FItem[i].fAHDVWp < 150 )
					continue;*/
				
				dfDistance = (waypoints[i].origin - pEntity->v.origin).Length();
				
				if (dfDistance < 300)
				{
					if (dfDistance < min_distance)
					{
						index = i; // store index of nearest waypoint
						min_distance = dfDistance;
					}
					
					if ((wp_display_time[i] + 1.0) < gpGlobals->time)
					{
						if(waypoints[i].flags & W_FL_TEAM_SPECIFIC){
							if ((waypoints[i].flags & W_FL_TEAM) == CS_TEAM_TE){
								fTeamr = 255;
								fTeamg = 0;
							}
							else // ct
							{
								fTeamr = 0;
								fTeamg = 255;
							}
						}
						else{		// not team specific
							fTeamr = 0;
							fTeamg = 0;
						}
						
						fAddProp2 = 255;
						
						if (waypoints[i].flags & W_FL_CROUCH)
						{
							start = waypoints[i].origin - Vector(0, 0, 17);
							end = start + Vector(0, 0, 34);
							fAddProp = 255;
						}
						else
						{
							start = waypoints[i].origin - Vector(0, 0, 34);
							end = start + Vector(0, 0, 68);
							fAddProp = 0;
						}
						
						if (waypoints[i].flags & W_FL_JUMP){
							fAddProp2 = 0;
							fAddProp = 255;
						}
						
						start = waypoints[i].origin - Vector(0, 0, 34);
						end = start + Vector(0, 0, 68);
						endteam = end + Vector(0,0,15);
						
						// draw a waypoint
						b1ND |= WaypointDrawBeam(pEntity, start, end, iWidth, 0, 0, int(fAddProp), int(fAddProp2), 250, 5);
						
						b1ND |= WaypointDrawBeam(pEntity, end, endteam, iWidth, 0, int(fTeamr), int(fTeamg), 255, 250, 5);
						
						if(waypoints[i].flags & W_FL_FLAG){				// red cross on top
							start = waypoints[i].origin + Vector(15,0,44);
							eend = waypoints[i].origin + Vector(-15,0,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 255,0,0, 250, 5);
							
							start = waypoints[i].origin + Vector(0,15,44);
							eend = waypoints[i].origin + Vector(0,-15,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 255,0,0, 250, 5);
						}
						if(waypoints[i].flags & W_FL_AIMING){
							start = waypoints[i].origin + Vector(0,-40,0);
							eend = waypoints[i].origin + Vector(0,40,0);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 0,0,255, 250, 5);
							
							start = waypoints[i].origin + Vector(-40,0,0);
							eend = waypoints[i].origin + Vector(40,0,0);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 0,0,255, 250, 5);
						}
						if(waypoints[i].flags & W_FL_FLAG_GOAL){	// green cross on top
							start = waypoints[i].origin + Vector(15,0,44);
							eend = waypoints[i].origin + Vector(-15,0,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 0,255,0, 250, 5);
							
							start = waypoints[i].origin + Vector(0,15,44);
							eend = waypoints[i].origin + Vector(0,-15,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 0,255,0, 250, 5);
						}
						if(waypoints[i].flags & W_FL_SNIPER){		// two blue lines on top
							start = waypoints[i].origin + Vector(15,10,44);
							eend = waypoints[i].origin + Vector(-15,10,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 0,0,255, 250, 5);
							
							start = waypoints[i].origin + Vector(15,-10,44);
							eend = waypoints[i].origin + Vector(-15,-10,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 0,0,255, 250, 5);
						}
						if(waypoints[i].flags & W_FL_DONTAVOID){		// two red lines on top
							start = waypoints[i].origin + Vector(15,10,44);
							eend = waypoints[i].origin + Vector(-15,10,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 255, 255, 0, 250, 5);
							
							start = waypoints[i].origin + Vector(15,-10,44);
							eend = waypoints[i].origin + Vector(-15,-10,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 255,255,0, 250, 5);
						}
						if(waypoints[i].flags & W_FL_VISIT){		// two green lines on top
							start = waypoints[i].origin + Vector(15,10,44);
							eend = waypoints[i].origin + Vector(-15,10,44);
							b1ND |= WaypointDrawBeam(pEntity, eend, end, iWidth, 2, 0,255,0, 250, 5);
							b1ND |= WaypointDrawBeam(pEntity, start, end, iWidth, 2, 0,255,0, 250, 5);
							
							start = waypoints[i].origin + Vector(15,-10,44);
							eend = waypoints[i].origin + Vector(-15,-10,44);
							b1ND |= WaypointDrawBeam(pEntity, eend, eend, iWidth, 2, 0,255,0, 250, 5);
							b1ND |= WaypointDrawBeam(pEntity, start, end, iWidth, 2, 0,255,0, 250, 5);
						}
						if(waypoints[i].flags & W_FL_LADDER){		// blue cross on top
							start = waypoints[i].origin + Vector(11,11,44);
							eend = waypoints[i].origin + Vector(-11,-11,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 0,0,255, 250, 5);
							
							start = waypoints[i].origin + Vector(-11,11,44);
							eend = waypoints[i].origin + Vector(11,-11,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 0,0,255, 250, 5);
						}
						if(waypoints[i].flags & W_FL_RESET){		// white cross on top
							start = waypoints[i].origin + Vector(15,0,44);
							eend = waypoints[i].origin + Vector(-15,0,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 255,255,255, 250, 5);
							
							start = waypoints[i].origin + Vector(0,15,44);
							eend = waypoints[i].origin + Vector(0,-15,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 255,255,255, 250, 5);
						}
						if(waypoints[i].flags & W_FL_DAF){		// two lines on top
							start = waypoints[i].origin + Vector(15,0,44);
							eend = waypoints[i].origin + Vector(-15,0,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 255,255,255, 250, 5);
							
							start = waypoints[i].origin + Vector(0,15,44);
							eend = waypoints[i].origin + Vector(0,-15,44);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 255,0,0, 250, 5);
						}
						if(waypoints[i].flags & W_FL_BLOCKHOSTAGE){
							start = waypoints[i].origin + Vector(30,0,44);
							eend = waypoints[i].origin + Vector(0,0,22);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 0,255,0, 250, 5);
							
							start = waypoints[i].origin + Vector(-30,0,44);
							eend = waypoints[i].origin + Vector(0,0,22);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth, 2, 0,255,0, 250, 5);
						}
#ifdef _DEBUG
						if(CVAR_BOOL(jb_wpstats)){
							start = waypoints[i].origin + Vector(20,0,-20);
							eend = waypoints[i].origin + Vector(20,0,0);
							b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 0,255,0, 50, 5);

							if(WPStat.d.FItem[i].fADVWp > 800){
								if(WPStat.d.FItem[i].fADVWp > 1200){
									start = waypoints[i].origin + Vector(20,5,-20);
									eend = waypoints[i].origin + Vector(20,-5,-20);
									b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 0,255,0, 250, 5);
									
									start = waypoints[i].origin + Vector(15,0,-20);
									eend = waypoints[i].origin + Vector(25,0,-20);
									b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 0,255,0, 250, 5);
								}
								else{
									start = waypoints[i].origin + Vector(20,5,-20);
									eend = waypoints[i].origin + Vector(20,-5,-20);
									b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 0,0,255, 250, 5);
									
									start = waypoints[i].origin + Vector(15,0,-20);
									eend = waypoints[i].origin + Vector(25,0,-20);
									b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 0,0,255, 250, 5);
								}
							}
							else if(WPStat.d.FItem[i].fADVWp < 300){
								start = waypoints[i].origin + Vector(20,5,-20);
								eend = waypoints[i].origin + Vector(20,-5,-20);
								b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 255,0,0, 250, 5);

								start = waypoints[i].origin + Vector(15,0,-20);
								eend = waypoints[i].origin + Vector(25,0,-20);
								b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 255,0,0, 250, 5);
							}
							float fRatio = float(WPStat.d.FItem[i].lKill)/float(WPStat.d.lKill);
							if(fRatio > .1){
								if(fRatio > .3){
									start = waypoints[i].origin + Vector(20,5,-10);
									eend = waypoints[i].origin + Vector(20,-5,-10);
									b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 255,0,0, 250, 5);
									
									start = waypoints[i].origin + Vector(15,0,-10);
									eend = waypoints[i].origin + Vector(25,0,-10);
									b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 255,0,0, 250, 5);
								}
								else if(fRatio > .2){
									start = waypoints[i].origin + Vector(20,5,-10);
									eend = waypoints[i].origin + Vector(20,-5,-10);
									b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 255,0,0, 250, 5);
									
									start = waypoints[i].origin + Vector(25,5,-10);
									eend = waypoints[i].origin + Vector(25,-5,-10);
									b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 255,0,0, 250, 5);
								}
								else{
									start = waypoints[i].origin + Vector(20,5,-10);
									eend = waypoints[i].origin + Vector(20,-5,-10);
									b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 255,0,0, 250, 5);
								}
							}
							if(WPStat.d.FItem[i].fADEn > 600){
								if(WPStat.d.FItem[i].fADEn < 800){
									start = waypoints[i].origin + Vector(20,5,0);
									eend = waypoints[i].origin + Vector(20,-5,0);
									b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 0,255,0, 250, 5);
								}
								else{
									start = waypoints[i].origin + Vector(20,5,0);
									eend = waypoints[i].origin + Vector(20,-5,0);
									b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 0,255,0, 250, 5);
									start = waypoints[i].origin + Vector(25,5,0);
									eend = waypoints[i].origin + Vector(25,-5,0);
									b1ND |= WaypointDrawBeam(pEntity, start, eend, iWidth/2, 2, 0,255,0, 250, 5);
								}
							}
						}
#endif
						
						// for beginning of paths
						
						start = waypoints[i].origin - Vector(0, 0, 34);
						end = start + Vector(0, 0, 68);
						
						// and red paths	^(joe)
						ppath = paths[i];
						
						while(ppath){
							p=0;
							while(p<MAX_PATH_INDEX){
								if(ppath->index[p] != -1){
									if(waypoints[ppath->index[p]].flags & W_FL_DELETED)		// just to be save
										continue;
									dsDistance =(waypoints[ppath->index[p]].origin - pEntity->v.origin).Length();
									
									start = waypoints[i].origin - Vector(0, 0, 34);
									end = start + Vector(0, 0, 68);
									
									if (waypoints[ppath->index[p]].flags & W_FL_CROUCH){
										start = waypoints[ppath->index[p]].origin + Vector(0, 0, 17);
									}
									else{
										start = waypoints[ppath->index[p]].origin + Vector(0, 0, 34);
									}
									
									if(dsDistance < dfDistance){
										fDirC = 0;
										start = start - Vector(0,0,10);
										end = end - Vector(0,0,10);
									}
									else{
										fDirC = 100;
									}
									
									// draw a path
									b1ND |= WaypointDrawBeam(pEntity, start, end, iWidth/3, 2, 255, int(fDirC), 0, 250, 40);
									
									if (WaypointGetADV(i,ppath->index[p])){
										start = start + Vector(0,0,20);
										end = end + Vector(0,0,20);
										b1ND |= WaypointDrawBeam(pEntity, start, end, iWidth/3, 2, 255, int(fDirC), 250, 250, 40);
									}
									
								};
								p++;
							}
							ppath = ppath->next;
						};
						// end joe
						wp_display_time[i] = gpGlobals->time;
						if(b1ND){
							wp_display_time[i] -= .168974551264;
							break;
						}
				}
         }
      }
	  
      // check if path waypointing is on...
      if (CVAR_BOOL(jb_wppath))
      {
		  // check if player is close enough to a waypoint and time to draw path...
		  if ((min_distance <= 50) && (f_path_time <= gpGlobals->time))
		  {
			  PATH *p;
			  
			  f_path_time = gpGlobals->time + 1.0;
			  
			  p = paths[index];
			  
			  while (p != NULL)
			  {
				  i = 0;
				  
				  while (i < MAX_PATH_INDEX)
				  {
					  if (p->index[i] != -1)
					  {
						  Vector v_src = waypoints[index].origin;
						  Vector v_dest = waypoints[p->index[i]].origin;
						  
						  // draw a white line to this index's waypoint
						  WaypointDrawBeam(pEntity, v_src, v_dest, 10, 2, 250, 250, 250, 200, 10);
					  }
					  
					  i++;
				  }
				  
				  p = p->next;  // go to next node in linked list
			  }
		  }
      }
   }
}


// run Floyd's algorithm on the waypoint list to generate the least cost
// path matrix...
void WaypointFloyds(unsigned short *shortest_path, unsigned short *from_to)
{
	unsigned int x, y, z;
	int changed = 1;
	int distance;
	
	for (y=0; y < route_num_waypoints; y++)
	{
		for (z=0; z < route_num_waypoints; z++)
		{
			from_to[y * route_num_waypoints + z] = z;
		}
	}
	
	while (changed)
	{
		changed = 0;
		
		for (x=0; x < route_num_waypoints; x++)
		{
			for (y=0; y < route_num_waypoints; y++)
			{
				for (z=0; z < route_num_waypoints; z++)
				{
					if ((shortest_path[y * route_num_waypoints + x] == WAYPOINT_UNREACHABLE) ||
						(shortest_path[x * route_num_waypoints + z] == WAYPOINT_UNREACHABLE))
						continue;
					
					distance = shortest_path[y * route_num_waypoints + x] +
						shortest_path[x * route_num_waypoints + z];
					
					if (distance > WAYPOINT_MAX_DISTANCE)
						distance = WAYPOINT_MAX_DISTANCE;
					
					if ((distance < shortest_path[y * route_num_waypoints + z]) ||
						(shortest_path[y * route_num_waypoints + z] == WAYPOINT_UNREACHABLE))
					{
						shortest_path[y * route_num_waypoints + z] = distance;
						from_to[y * route_num_waypoints + z] = from_to[y * route_num_waypoints + x];
						changed = 1;
					}
				}
			}
		}
	}
}

bool WaypointGetDir(char *szFile,char *szDest){
	char szBaseDir[200],szDir[200],szFilename[200];
	int iDir = 0;
	*szDest=0;
	
	UTIL_BuildFileName(szBaseDir, sizeof(szBaseDir), "joebot/wpjs");
	
	while(g_WPDir.szPDir[iDir].szDir[0]){
		snprintf(szDir, sizeof(szDir), "%s/%s", szBaseDir, g_WPDir.szPDir[iDir].szDir);
		snprintf(szFilename, sizeof(szFilename), "%s/%s", szDir, szFile);

		struct stat buf;
		if (stat(szFilename, &buf) == 0)
		{
			strcpy(szDest,szDir);
			return true;
		}
		iDir ++;
	}

	snprintf(szDest, sizeof(szDest), "%s/std", szBaseDir);
	return false;
}

// load the waypoint route files (.wp1, .wp2, etc.) or generate them if
// they don't exist...
void WaypointRouteInit(void)
{
	unsigned int index;
	bool build_matrix[4];
	int matrix;
	unsigned int array_size;
	unsigned int row;
	int i, offset;
	unsigned int a, b;
	float distance;
	unsigned short *pShortestPath, *pFromTo;
	unsigned int num_items;
	FILE *bfp;
	char filename[256];
	char filename2[256];
	char mapname[256];
	//	char dirname[32];
	char szRoutes[256];
	
	clock_t ct_start_distances;
	clock_t ct_end_distances;
	clock_t ct_start_floyd;
	clock_t ct_end_floyd;
	clock_t ct_start_misc;
	clock_t ct_end_misc;
	
	if (num_waypoints == 0)
		return;
	
	// save number of current waypoints in case waypoints get added later
	route_num_waypoints = num_waypoints;
	
	snprintf(mapname, sizeof(mapname), "%s.wpj", STRING(gpGlobals->mapname));
	WaypointGetDir(mapname,filename);
	snprintf(szRoutes, sizeof(szRoutes), "%s/routes", filename);
	strcat(filename, mapname);
	
	build_matrix[0] = TRUE;  // always build matrix 0 (non-team and team 1) and matrix 1 ( only for cs )
	build_matrix[1] = TRUE;
	build_matrix[2] = FALSE;
	build_matrix[3] = FALSE;
	
	// find out how many route matrixes to create...
	/*for (index=0; index < route_num_waypoints; index++)
	{
	if (waypoints[index].flags & W_FL_TEAM_SPECIFIC)
	{
	if ((waypoints[index].flags & W_FL_TEAM) == 0x01)  // team 2?
				build_matrix[1] = TRUE;
				
				  if ((waypoints[index].flags & W_FL_TEAM) == 0x02)  // team 3?
				  build_matrix[2] = TRUE;
				  
					if ((waypoints[index].flags & W_FL_TEAM) == 0x03)  // team 4?
					build_matrix[3] = TRUE;
					}
}*/
	
	array_size = route_num_waypoints * route_num_waypoints;
	
	for (matrix=0; matrix < 4; matrix++)
	{
		if (build_matrix[matrix])
		{
			int file1, file2;
			struct stat stat1, stat2;
			
			snprintf(filename2, sizeof(filename2), "%s/%s.wp%d", szRoutes, STRING(gpGlobals->mapname), matrix+1);
			
			if (access(filename2, 0) == 0)  // does the .wpX file exist?
			{
				file1 = open(filename, O_RDONLY);
				file2 = open(filename2, O_RDONLY);
				
				fstat(file1, &stat1);
				fstat(file2, &stat2);
				
				close(file1);
				close(file2);
				
				if (stat1.st_mtime < stat2.st_mtime)  // is .wpt older than .wpX file?
				{
					//UTIL_ConsoleMessage(NULL, "Loading JoeBOT waypoint paths for team %d\n", matrix+1);
					if (IS_DEDICATED_SERVER())
						LOG_MESSAGE(PLID, "Loading waypoint route file: %s", filename2);
					else{
						UTIL_ConsoleMessage(NULL, "Loading waypoint route file: %s\n", filename2);
					}
					
					shortest_path[matrix] = (unsigned short *)malloc(sizeof(unsigned short) * array_size);
					
					if (shortest_path[matrix] == NULL)
						ALERT(at_error, "[JOEBOT] Error allocating memory for shortest path!");
					
					from_to[matrix] = (unsigned short *)malloc(sizeof(unsigned short) * array_size);
					
					if (from_to[matrix] == NULL)
						ALERT(at_error, "[JOEBOT] Error allocating memory for from to matrix!");
					
					bfp = fopen(filename2, "rb");
					
					if (bfp != NULL)
					{
						ROUTE_HDR route_hdr;
						fread(&route_hdr,sizeof(ROUTE_HDR),1,bfp);

						num_items = fread(shortest_path[matrix], sizeof(unsigned short), array_size, bfp);
						
						if (num_items != array_size || !FStrEq(route_hdr.filetype,DEFAULT_ROUTE_HDR))
						{
							// if couldn't read enough data, free memory to recalculate it
							
							UTIL_ConsoleMessage(NULL, "Error reading enough path items, recalculating...\n");
							
							free(shortest_path[matrix]);
							shortest_path[matrix] = NULL;
							
							free(from_to[matrix]);
							from_to[matrix] = NULL;
						}
						else
						{
							num_items = fread(from_to[matrix], sizeof(unsigned short), array_size, bfp);
							
							if (num_items != array_size)
							{
								// if couldn't read enough data, free memory to recalculate it
								
								ALERT(at_console, "Error reading enough path items, recalculating...\n");
								
								free(shortest_path[matrix]);
								shortest_path[matrix] = NULL;
								
								free(from_to[matrix]);
								from_to[matrix] = NULL;
							}
						}
					}
					else
					{
						ALERT(at_console, "[JOEBOT] Error reading waypoint paths!\n");
						
						free(shortest_path[matrix]);
						shortest_path[matrix] = NULL;
						
						free(from_to[matrix]);
						from_to[matrix] = NULL;
					}
					
					fclose(bfp);
				}
			}
			
			if (shortest_path[matrix] == NULL)
			{
				//ALERT(at_console, UTIL_VarArgs("calculating Joe_bot waypoint paths for team %d...\n", matrix+1));
				
				if (IS_DEDICATED_SERVER())
					LOG_MESSAGE(PLID, "Creating waypoint route file: %s", filename2);
				
				shortest_path[matrix] = (unsigned short *)malloc(sizeof(unsigned short) * array_size);
				
				if (shortest_path[matrix] == NULL)
					ALERT(at_error, "[JOEBOT] Error allocating memory for shortest path!");
				
				from_to[matrix] = (unsigned short *)malloc(sizeof(unsigned short) * array_size);
				
				if (from_to[matrix] == NULL)
					ALERT(at_error, "[JOEBOT] Error allocating memory for from to matrix!");
				
				pShortestPath = shortest_path[matrix];
				pFromTo = from_to[matrix];
				
				ct_start_distances = clock();
				
				for (index=0; index < array_size; index++)
					pShortestPath[index] = WAYPOINT_UNREACHABLE;
				
				for (index=0; index < route_num_waypoints; index++)
					pShortestPath[index * route_num_waypoints + index] = 0;  // zero diagonal
				
				for (row=0; row < route_num_waypoints; row++)
				{
					if (paths[row] != NULL)
					{
						PATH *p = paths[row];
						
						while (p)
						{
							i = 0;
							
							while (i < MAX_PATH_INDEX)
							{
								if (p->index[i] != -1)
								{
									index = p->index[i];
									
									// check if this is NOT team specific OR matches this team
									if (!(waypoints[index].flags & W_FL_TEAM_SPECIFIC) ||
										((waypoints[index].flags & W_FL_TEAM) == matrix))
									{
										distance = (waypoints[row].origin - waypoints[index].origin).Length();

										if(waypoints[row].flags & W_FL_JUMP){
											distance += 75.f;
										}
										
										if (distance > (float)WAYPOINT_MAX_DISTANCE)
											distance = (float)WAYPOINT_MAX_DISTANCE;
										
										if (distance > REACHABLE_RANGE)
										{
											ALERT(at_console, UTIL_VarArgs("WARNING: Waypoint path distance > %4.1f at from %d to %d\n",float(REACHABLE_RANGE), row, index));
										}
										offset = row * route_num_waypoints + index;
										
										pShortestPath[offset] = (unsigned short)distance;
									}
								}
								
								i++;
							}
							
							p = p->next;  // go to next node in linked list
						}
					}
				}
				
				ct_start_floyd = ct_end_distances = clock();
				
				// run Floyd's Algorithm to generate the from_to matrix...
				WaypointFloyds(pShortestPath, pFromTo);
				ct_start_misc = ct_end_floyd = clock();
				
				unsigned short *pShortestPath_a;
				for (a=0; a < route_num_waypoints; a++)
				{
					pShortestPath_a = &pShortestPath[a * route_num_waypoints];
					for (b=0; b < route_num_waypoints; b++)
						if (pShortestPath_a[b] == WAYPOINT_UNREACHABLE)
							pFromTo[a * route_num_waypoints + b] = WAYPOINT_UNREACHABLE;
				}
				
				bfp = fopen(filename2, "wb");
	
				if (bfp != NULL)
				{
					ALERT(at_console, UTIL_VarArgs("Writing floyd table to %s\n",filename2));

					ROUTE_HDR route_hdr;
					memset(&route_hdr,'\n',sizeof(ROUTE_HDR));
					strcpy(route_hdr.filetype,DEFAULT_ROUTE_HDR);
					fwrite (&route_hdr,sizeof(ROUTE_HDR),1,bfp);

					num_items = fwrite(shortest_path[matrix], sizeof(unsigned short), array_size, bfp);
					
					if (num_items != array_size)
					{
						// if couldn't write enough data, close file and delete it
						
						fclose(bfp);
						unlink(filename2);
					}
					else
					{
						num_items = fwrite(from_to[matrix], sizeof(unsigned short), array_size, bfp);
						
						fclose(bfp);
						
						if (num_items != array_size)
						{
							// if couldn't write enough data, delete file
							unlink(filename2);
						}
					}
				}
				else
				{
					ALERT(at_console, "[JOEBOT] Error writing waypoint paths!\n");
				}
				ct_end_misc = clock();
				
				ALERT( at_console, UTIL_VarArgs("Time (dist,floyd,misc): %.2f,%.2f,%.2f\n",(ct_end_distances - ct_start_distances)/float(CLOCKS_PER_SEC),(ct_end_floyd - ct_start_floyd)/float(CLOCKS_PER_SEC),(ct_end_misc - ct_start_misc)/float(CLOCKS_PER_SEC)) );
				/*if(IS_DEDICATED_SERVER())
					LOG_MESSAGE(PLID, "Time (dist,floyd,misc): %.2f,%.2f,%.2f",(ct_end_distances - ct_start_distances)/float(CLOCKS_PER_SEC),(ct_end_floyd - ct_start_floyd)/float(CLOCKS_PER_SEC),(ct_end_misc - ct_start_misc)/float(CLOCKS_PER_SEC));*/
				
				ALERT( at_console, UTIL_VarArgs("Waypoint path calculations for team %d complete!\n",matrix+1) );
				/*if(IS_DEDICATED_SERVER())
					LOG_MESSAGE(PLID, "Waypoint path calculations for team %d complete!",matrix+1);*/
         }
      }
   }  
}


// return the next waypoint index for a path from the Floyd matrix when
// going from a source waypoint index (src) to a destination waypoint
// index (dest)...
unsigned short WaypointRouteFromTo(int src, int dest, int team)
{
	if((unsigned int)src < route_num_waypoints&&(unsigned int)dest < route_num_waypoints){
		unsigned short *pFromTo;
		
		if ((team < -1) || (team > 3))
			return WAYPOINT_UNREACHABLE;
		
		if (team == -1)  // -1 means non-team play
			team = 0;
		
		if (from_to[team] == NULL)  // if no team specific waypoints use team 0
			team = 0;
		
		if (from_to[team] == NULL)  // if no route information just return
			return WAYPOINT_UNREACHABLE;
		
		pFromTo = from_to[team];

		return pFromTo[src * route_num_waypoints + dest];
	}
	else
		return WAYPOINT_UNREACHABLE;
}


// return the total distance (based on the Floyd matrix) of a path from
// the source waypoint index (src) to the destination waypoint index
// (dest)...
int WaypointDistanceFromTo(int src, int dest, int team)
{
	if((unsigned int)src < route_num_waypoints&&(unsigned int)dest < route_num_waypoints){
		unsigned short *pShortestPath;
		
		if ((team < -1) || (team > 1))
			return -1;
		
		if (team == -1)  // -1 means non-team play
			team = 0;
		
		if (from_to[team] == NULL)  // if no team specific waypoints use team 0
			team = 0;
		
		if (from_to[team] == NULL)  // if no route information just return
			return -1;
		
		pShortestPath = shortest_path[team];
		
		return (int)(pShortestPath[src * route_num_waypoints + dest]);
	}
	else
		return 1000;
}

// convert floyd to old way struct

int ConvertFloyd2Way(int iTeam,int iSrc, int iDest, CWay *pWay){
	int iPointer = iSrc;
	int iNext;
	//int iNext;
	
	pWay->iNum = 0;
	
	if(iSrc < 0 || iSrc > num_waypoints || iDest < 0 || iDest > num_waypoints)
		return 0;
	
	if(WaypointRouteFromTo(iSrc,iDest,iTeam) != WAYPOINT_UNREACHABLE){
		while(iPointer != iDest){
			pWay -> iIndices[pWay->iNum] = iPointer;
			(pWay -> iNum) ++;
			
			iNext = WaypointRouteFromTo(iPointer,iDest,iTeam);
			iPointer = iNext;
		};
		pWay -> iIndices[pWay->iNum] = iPointer;
		pWay -> iNum ++;
	}
	
	return pWay->iNum;
}
