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
#include "cwpstuff.h"

#include <windows.h>

WAYPOINT waypoints[MAX_WAYPOINTS];

// statistical information based on wps
CWPStat WPStat;
bool bInitWST=false;

// number of waypoints currently in use
int num_waypoints;

// declare the array of head pointers to the path structures...
PATH *paths[MAX_WAYPOINTS];
WAYPOINT_HDR header;

bool g_waypoint_paths = false;		// have any paths been allocated?
bool g_waypoint_on = false;
bool g_auto_waypoint = false;
bool g_auto_addpath = true;
bool g_path_waypoint = false;

void WaypointFree(void)
{
	for (int i=0; i < MAX_WAYPOINTS; i++)
	{
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
			}
			
			paths[i] = NULL;
		}
	}
}

void WaypointInit(void)
{
	int i;
	
	// have any waypoint path nodes been allocated yet?
	if (g_waypoint_paths)
		WaypointFree();  // must free previously allocated path memory
	
	for (i=0; i < MAX_WAYPOINTS; i++)
	{
		waypoints[i].flags = 0;
		waypoints[i].origin = Vector(0,0,0);		
		
		paths[i] = NULL;  // no paths allocated yet
	}
	
	num_waypoints = 0;
}

void WaypointAddPath(short int add_index, short int path_index)
{
	PATH *p, *prev;
	int i;
	int count = 0;
	
	p = paths[add_index];
	prev = NULL;
	
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
	}
	
	p = (PATH *)malloc(sizeof(PATH));
	//p = new PATH;
	
	if (p == NULL)
	{
		//ALERT(at_error, "Joe_bot - Error allocating memory for path!");
	}
	
	p->index[0] = path_index;
	p->index[1] = -1;
	p->index[2] = -1;
	p->index[3] = -1;
	p->next = NULL;
	
	if (prev != NULL)
		prev->next = p;  // link new node into existing list
	
	if (paths[add_index] == NULL)
		paths[add_index] = p;  // save head point if necessary
}

CWPStat :: CWPStat(){
	*wpsHeader.szMapname	= 0;
	strcpy(wpsHeader.szFTName,_FTNAME);
	wpsHeader.lVersion = _WPSVERSION;
}

CWPStat :: ~CWPStat(){
}

int CWPStat :: GetVisible(long l1,long l2){
	if(!pWPV)
		return 0;

	if(l1>=lNWPfWPV
		||l2>=lNWPfWPV
		||l1<0
		||l2<0){
		return 0;
	}

	int iReturn;
	long index;
	long lshift;
	long lElem = l1 * lNWPfWPV + l2;

	index = lElem / 16;
	lshift = (lElem - index * 16) * 2;

	iReturn = pWPV[index]>>lshift;
	//iReturn>>=lshift;

	return iReturn&3;
}

int CWPStat::Load(char *szName){
	char filename[256];
	//char dirname[80];
	//char mapname[30];
	bool bright = true;
	WPSHEADER wpsHeaderLoad;
	FILE *fhd;
	
	strcpy(filename,szName);

	if(fhd = fopen(filename,"rb")){
		fread(&wpsHeaderLoad,sizeof(WPSHEADER),1,fhd);
		if(strcmp(wpsHeaderLoad.szFTName,_FTNAME)){
			bright = false;
		}
		if(wpsHeaderLoad.lVersion != _WPSVERSION){
			bright = false;
		}
		if(!bright){
			MessageBox(0,"JoeBOT : incompatible .wst file, recreating and continuing ....\n",0,0);
			memset(&d,0,sizeof(WPStatDATA));
			fclose(fhd);
			return true;
		}
		else{
			fread(&lNWPfWPV,sizeof(long),1,fhd);
			fread(&lWPSlice,sizeof(long),1,fhd);
			fread(&bRecalcWPV,sizeof(bool),1,fhd);

			fread(&lKillMax,sizeof(long),1,fhd);
			fread(&lKilledMax,sizeof(long),1,fhd);
			//fread(&d,sizeof(WPStatDATA),1,fhd);
			fread(&d.lKill,sizeof(long),1,fhd);
			fread(&d.lKilled,sizeof(long),1,fhd);
			fread(&d.FItem,sizeof(WPStatITEM),lNWPfWPV,fhd);
			if(pWPV)
				delete [] pWPV;
			pWPV = new long[lNWPfWPV*lNWPfWPV/16+16];
			fread(pWPV,sizeof(long),lNWPfWPV*lNWPfWPV/16+16,fhd);
			//printf("%li,%li",d.lKill,d.lKilled);
			fclose(fhd);
			return true;
		}
	}
	else{
		MessageBox(0,"JoeBOT WPStat : FNF\n",0,0);
		memset(&d,0,sizeof(WPStatDATA));
		if(pWPV)
			delete [] pWPV;
		pWPV = 0;
		return false;
	}
}


bool WaypointLoad(const char *szLoad)
{
//	char mapname[64];
	char filename[256];
//	char dirname[80];
	//WAYPOINT_HDR header;
//	char msg[80];
	int index, i;
	short int num;
	short int path_index;
	
	WaypointInit();

	strcpy(filename, szLoad);
	//strcat(filename,".wpj");
	
	FILE *bfp = fopen(filename, "rb");
	
	// if file exists, read the waypoint structure from it
	if (bfp != NULL)
	{
		fread(&header, sizeof(header), 1, bfp);
		
		header.filetype[7] = 0;
		if ((strcmp(header.filetype, "Joe_bot") == 0))
		{
			/*if (header.waypoint_file_version != WAYPOINT_VERSION)
			{			
				fclose(bfp);
				return false;
			}*/
			
			header.mapname[31] = 0;
			
			//WaypointInit();  // remove any existing waypoints
			
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

			// just ignore all this recorded movement stuff here and return
			
			g_waypoint_paths = true;  // keep track so path can be freed
		}
		else
		{	
			fclose(bfp);
			return false;
		}
		
		fclose(bfp);
		
		//WaypointRouteInit();
	}
	else
	{	
		MessageBox(0,"couldnt find file","",MB_OK);
		return false;
	}
	
	return true;
	//return WaypointLoadStat(szLoad);
}

