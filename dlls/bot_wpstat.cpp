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
#include <iostream.h>

#include "extdll.h"
#include "util.h"

#ifdef USE_METAMOD
#define SDK_UTIL_H  // util.h already included
#include "meta_api.h"
#endif /* USE_METAMOD */

#include "bot_wpstat.h"

#include "bot.h"
#include "bot_func.h"
#include "bot_names.h"
#include "bot_wpdir.h"
#include "bot_globaldefs.h"
#include "CLTMem.h"
#include "CState.h"
#include "CTask.h"
#include "CPersonality.h"
#include "CRadio.h"
#include "CSDecals.h"
#include "CSkill.h"
#include "CSTMem.h"
#include "Commandfunc.h"
#include "globalvars.h"
#include "NNWeapon.h"

#include "NeuralNet.h"
#include "som.h"

// statistical information based on wps
CWPStat WPStat;

bool FVisible( const Vector &vecOrigin, edict_t *pEdict );

#define AVERAGE_C_DIST 10.0

#define _WPSTAT_SLICE_SIZE 300

CWPStat :: CWPStat(){
	*wpsHeader.szMapname	= 0;
	strcpy(wpsHeader.szFTName,_FTNAME);
	wpsHeader.lVersion = _WPSVERSION;
	lKillMax = 0;
	lKilledMax = 0;

	bRecalcWPV = true;
	pWPV = 0;
	lWPSlice = 0;
	lNWPfWPV = 0;
	lPercent = 0;

	Init();
}

CWPStat :: ~CWPStat(){
	//	Save(); // thx to wookie

	if(pWPV){
		delete [] pWPV;
		pWPV = 0;
	}
}

void CWPStat :: Init(void){
	long lschl=0;
	for(;lschl < MAX_WAYPOINTS;lschl++){
		InitWP(lschl);
	}
	bRecalcWPV = true;
	d.lKill = 0;
	d.lKilled = 0;
	lKillMax = 0;
	lKilledMax = 0;
	lWPSlice = 0;
	lPercent = 0;
	if(pWPV)
		delete [] pWPV;
	pWPV = 0;
}

void CWPStat :: InitWP(long lWP){
	long lWP1=0;
	if(lWP>MAX_WAYPOINTS){
		LOG_DEBUG("WaypointStatDebug: ERROR!!!");
		*((long*)lWP1) = 0;
	}

	d.FItem[lWP].lKill = 0;
	d.FItem[lWP].lKilled = 0;
	for(lWP1=0;lWP1 < MAX_TIMES_PWP;lWP1++){
		d.FItem[lWP].fpKilledTime[lWP1]=0.0;
		d.FItem[lWP].fpKillTime[lWP1]=0.0;
	}
	d.FItem[lWP].lKilledT = 0;
	d.FItem[lWP].lKillT = 0;
	d.FItem[lWP].fADEn = 0;
	d.FItem[lWP].iNVWp = 0;
	d.FItem[lWP].fADVWp = 0;
	d.FItem[lWP].fMaxDVWp = 0;
	d.FItem[lWP].fMinDVWp = 100000;
	d.FItem[lWP].fAHDVWp = 0;
	for(int i=0;i < 2;i++){
		d.FItem[lWP].lTraffic[i] = 0;
	}

	bRecalcWPV = true;
	lWPSlice = 0;
}

int CWPStat :: AddKill(edict_t *pEdictKiller, edict_t *pEdictVictim){
	int iKTeam=-1,iVTeam=-1;
	int iKInd=-1,iVInd=-1;
	float f=AVERAGE_C_DIST;
	
	if(!FNullEnt(pEdictKiller)){
		iKTeam = UTIL_GetTeam(pEdictKiller);
		iKInd = ENTINDEX(pEdictKiller);
	}
	if(!FNullEnt(pEdictVictim)){
		iVTeam = UTIL_GetTeam(pEdictVictim);
		iVInd = ENTINDEX(pEdictVictim);
	}
	
	if(pEdictKiller && pEdictKiller != pEdictVictim && iKTeam != iVTeam&&iKInd!=-1){		// dont count suicides, tk and kills by worldspan
		int iNVictim = -1;
		int iNKiller = -1;
		
		if(pEdictVictim)
			iNVictim = WaypointFindNearest(pEdictVictim->v.origin,pEdictVictim,500,-1);
		if(pEdictKiller) 
			iNKiller = WaypointFindNearest(pEdictKiller->v.origin,pEdictKiller,500,-1);
		
		if(iNVictim != -1
			&& iNVictim<num_waypoints){
			if(d.FItem[iNVictim].lKilledT>=MAX_TIMES_PWP||d.FItem[iNVictim].lKilledT<0)					// just to fix bad files etc
				d.FItem[iNVictim].lKilledT = 0;
			d.lKilled++;
			d.FItem[iNVictim].lKilled++;
			if(d.FItem[iNVictim].lKilled > lKilledMax)
				lKilledMax = d.FItem[iNVictim].lKilled;
			d.FItem[iNVictim].fpKilledTime[d.FItem[iNVictim].lKilledT] = g_fRoundTime;
			d.FItem[iNVictim].lKilledT ++;
			if(d.FItem[iNVictim].lKilledT>=MAX_TIMES_PWP)
				d.FItem[iNVictim].lKilledT = 0;
		}
		if(iNKiller != -1
			&& iNKiller < num_waypoints){
			if(d.FItem[iNKiller].lKillT>=MAX_TIMES_PWP||d.FItem[iNKiller].lKillT<0)					// just to fix bad files etc
				d.FItem[iNKiller].lKillT = 0;
			d.lKill++;
			d.FItem[iNKiller].lKill++;
			if(d.FItem[iNKiller].lKill++ > lKillMax)
				lKillMax=d.FItem[iNKiller].lKill++;
			d.FItem[iNKiller].fpKillTime[d.FItem[iNKiller].lKillT] = g_fRoundTime;
			d.FItem[iNKiller].lKillT ++;
			if(d.FItem[iNKiller].lKillT>=MAX_TIMES_PWP)
				d.FItem[iNKiller].lKillT = 0;
		}
		
		if(pEdictKiller
			&&pEdictVictim
			&&iNKiller != -1
			&&iNVictim != -1
			&&iNKiller<num_waypoints
			&&iNVictim<num_waypoints){
			
			//TraceResult tr;		// trace a line from the origins of the waypoints. If this line is fractioned, don't do something about distance stuff, cause this isn't useful to do for making the bots camp there
			
			//UTIL_TraceLine(waypoints[iNKiller].origin,waypoints[iNVictim].origin,ignore_monsters,ignore_glass,pEdictKiller,&tr);	// ent to ignore just a valid pointer to an entity ... this is not of importance, cause ignore_monsters
			
			if(WPStat.GetVisible(iNKiller,iNVictim)){	// nothing hit ...
				float fDistance = (pEdictKiller->v.origin - pEdictVictim->v.origin).Length();
				if(long(d.FItem[iNKiller].fADEn) == 0){
					d.FItem[iNKiller].fADEn = fDistance;
				}
				else{
					d.FItem[iNKiller].fADEn = (f*d.FItem[iNKiller].fADEn + fDistance)/(f+1.0f);
				}
				
				if(long(d.FItem[iNVictim].fADEn) == 0){
					d.FItem[iNVictim].fADEn = fDistance;
				}
				else{
					d.FItem[iNVictim].fADEn = (f*d.FItem[iNVictim].fADEn + fDistance)/(f+1.0f);
				}
			}
		}
	}
	return true;
}

void CWPStat :: AddTraffic(int iTeam,int iWP){
	if(iWP != -1
		&& iTeam >= 0
		&& iTeam < 2){
		d.FItem[iWP].lTraffic[iTeam]++;
	}
}

int CWPStat::Load(void){
	char filename[256];
	char dirname[80];
	char mapname[30];
	bool bright = true;
	bRecalcWPV = true;
	lWPSlice = 0;
	WPSHEADER wpsHeaderLoad;
	FILE *fhd;

	Init();
	
	strcpy(wpsHeader.szMapname, STRING(gpGlobals->mapname));		// copy mapname there
	snprintf(mapname, sizeof(mapname), "%s.wpj", STRING(gpGlobals->mapname));
	WaypointGetDir(mapname,dirname);
	snprintf(filename, sizeof(filename), "%s/routes/%s.wst", dirname, STRING(gpGlobals->mapname));

	if(fhd = fopen(filename,"rb")){
		fread(&wpsHeaderLoad,sizeof(WPSHEADER),1,fhd);
		if(!FStrEq(wpsHeaderLoad.szFTName,_FTNAME)){
			bright = false;
		}
		if(wpsHeaderLoad.lVersion != _WPSVERSION){
			bright = false;
		}
		if(!bright){
			LOG_MESSAGE(PLID, "Incompatible .wst file, recreating and continuing ....");
			Init();
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
			if(pWPV){
				delete [] pWPV;
				pWPV = 0;
			}
			if(lNWPfWPV>0){
				pWPV = new long[lNWPfWPV*lNWPfWPV/16+16];
				fread(pWPV,sizeof(long),lNWPfWPV*lNWPfWPV/16+16,fhd);
			}
			//printf("%li,%li",d.lKill,d.lKilled);
			fclose(fhd);
			
			if (IS_DEDICATED_SERVER())
				LOG_MESSAGE(PLID, "Loading statistic file: %s - %li|%li", filename,d.lKill,d.lKilled);
			else
				UTIL_ConsoleMessage(NULL, "Loading statistic file: %s - %li|%li\n", filename,d.lKill,d.lKilled);

			return true;
		}
	}
	else{
		if (IS_DEDICATED_SERVER())
			LOG_MESSAGE(PLID, "Couldn't load statistic file: %s - resetting statistics", filename);
		else
			UTIL_ConsoleMessage(NULL, "Couldn't load statistic file: %s - resetting statistics\n", filename);

		Init();
		return false;
	}
}

int CWPStat::Save(void){
	if(strlen(wpsHeader.szMapname)==0)
		return false;

	char filename[256];
	char dirname[80];
	char mapname[30];
	FILE *fhd;

	snprintf(mapname, sizeof(mapname), "%s.wpj", wpsHeader.szMapname);
	WaypointGetDir(mapname,dirname);
	snprintf(filename, sizeof(filename), "%s/routes/%s.wst", dirname, wpsHeader.szMapname);
	
	if(fhd = fopen(filename,"wb")){
		fwrite(&wpsHeader,sizeof(WPSHEADER),1,fhd);

		fwrite(&lNWPfWPV,sizeof(long),1,fhd);
		fwrite(&lWPSlice,sizeof(long),1,fhd);
		fwrite(&bRecalcWPV,sizeof(bool),1,fhd);

		if (IS_DEDICATED_SERVER())
			LOG_MESSAGE(PLID, "Saving statistic file: %s", filename);
		fwrite(&lKillMax,sizeof(long),1,fhd);
		fwrite(&lKilledMax,sizeof(long),1,fhd);

		//fwrite(&d,sizeof(WPStatDATA),1,fhd);
		fwrite(&d.lKill,sizeof(long),1,fhd);
		fwrite(&d.lKilled,sizeof(long),1,fhd);
		fwrite(&d.FItem,sizeof(WPStatITEM),lNWPfWPV,fhd);
		if(lNWPfWPV>0)
			fwrite(pWPV,sizeof(long),lNWPfWPV*lNWPfWPV/16+16,fhd);
		//cout << lNWPfWPV << endl;

		fclose(fhd);
		return true;
	}
	else
	{
		if (IS_DEDICATED_SERVER())
			LOG_MESSAGE(PLID, "Not able to write saving statistic file: %s", filename);
		return false;
	}
}

int CWPStat::Search(int iTeam,int iNearest,int iType, float fMin, float fMax,
					float fParam,float fParam2,edict_t *pEdict, bool bVisible){		// type : sniper, camp etc ...
	int index;
	int indexes[50];
	int count = 0;
	float fDistance;
	
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
		if ((iTeam != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != iTeam))
			continue;

		fDistance = WaypointDistanceFromTo(iNearest,index,iTeam);

		if(fDistance > fMax)		// too far away ?
			continue;
		if(fDistance < fMin)		// too near  ?
			continue;

		if(bVisible
			&&!FNullEnt(pEdict)){
			if(!FVisible(waypoints[index].origin,pEdict))
				continue;
		}

		// test if condition is right when regarding the stat
		if(iType == WPS_SEARCH_SNIPER){
			if (waypoints[index].flags & W_FL_LADDER)
				continue;  // skip any ladder waypoints
			bool bTemp = false;
			/*if(d.FItem[index].fADEn < 800.0)
				if(WPStat.d.FItem[index].fADVWp < 700 || WPStat.d.FItem[index].fAHDVWp < 150)
					continue;*/
			if(d.FItem[index].fADEn > 800.0)
				bTemp |= true;
			else if(WPStat.d.FItem[index].fADVWp > 700
				&& WPStat.d.FItem[index].fAHDVWp > 150)
				bTemp |= true;
			else if(WPStat.d.FItem[index].fADVWp > 1100)
				bTemp |= true;

			if(!bTemp)
				continue;
		}
		else if(iType == WPS_SEARCH_CAMP){
			if (waypoints[index].flags & W_FL_LADDER)
				continue;  // skip any ladder waypoints
			if(d.FItem[index].fADEn > 300.0 )
				if(WPStat.d.FItem[index].fADVWp > 300)
					continue;
		}
		else if(iType == WPS_SEARCH_ENEMYESP){
			bool btemp = false;
			int ischl;
			for(ischl = 0;ischl < MAX_TIMES_PWP;ischl++){
				if(TEq(d.FItem[index].fpKilledTime[ischl],fParam,fParam2)){
					btemp = true;
					break;
				}
			}
			if(!btemp)
				continue;
		}
		else if(iType == WPS_SEARCH_ENEMYESPTT){
			bool btemp = false;
			int ischl;
			float fTT = WaypointDistanceFromTo(iNearest,index,iTeam) / 180.f;
			if(fTT > fParam)
				continue;
			for(ischl = 0;ischl < MAX_TIMES_PWP;ischl++){
				if(TEq(d.FItem[index].fpKilledTime[ischl],gpGlobals->time + fTT,fParam2)){
					btemp = true;
					break;
				}
			}
			if(!btemp)
				continue;
		}
		
		if (count < 50)
		{
			indexes[count] = index;
			
			count++;
		}
		else
			break;
	}
	
	if (!count)  // no matching waypoints found
		return -1;
	
	index = RANDOM_LONG(0, count - 1);
	
	return indexes[index];
}

int CWPStat::SearchNearest(int iTeam,int iNearest, int iType, float fMax){
	int index, min_index;
	int distance, min_distance;
	
	if (num_waypoints < 1)
		return -1;
	
	// find the nearest waypoint with the matching flags...
	
	min_index = -1;
	min_distance = 99999;
	
	for (index=0; index < num_waypoints; index++)
	{
		if (index == iNearest)
			continue;  // skip the source waypoint
		
		if (waypoints[index].flags & W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[index].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints
		
		// skip this waypoint if it's team specific and teams don't match...
		if ((iTeam != -1) && (waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[index].flags & W_FL_TEAM) != iTeam))
			continue;

		// test if condition is right when regarding the stat
		if(iType == WPS_SEARCH_SNIPER){
			if (waypoints[index].flags & W_FL_LADDER)
				continue;  // skip any ladder waypoints
			if(d.FItem[index].fADEn < 1000.0)
				continue;
		}
		else if(iType == WPS_SEARCH_CAMP){
			if(d.FItem[index].fADEn > 300.0)
				continue;
		}
		
		distance = WaypointDistanceFromTo(iNearest, index, iTeam);
		
		if (distance < min_distance)
		{
			min_index = index;
			min_distance = distance;
		}
	}
	
	return min_index;
}

bool CWPStat :: GetWayInfo(int iTeam,int iSIndex, int iEIndex,WAYINFO &wayinfo){
	CWay Way;

	ConvertFloyd2Way(iTeam,iSIndex,iEIndex,&Way);

	return GetWayInfo(Way,wayinfo);
}

bool CWPStat :: GetWayInfo(CWay &Way,WAYINFO &wayinfo){
	register long lschl;

	for(lschl=0;lschl < Way.iNum;lschl++){
		if(Way.iIndices[lschl]>num_waypoints)
			WaypointDebug();
		if(waypoints[Way.iIndices[lschl]].flags & W_FL_BLOCKHOSTAGE){
			wayinfo.bHostageable = false;
		}
		wayinfo.lStatKilled += WPStat.d.FItem[Way.iIndices[lschl]].lKilled;
		wayinfo.lStatKill += WPStat.d.FItem[Way.iIndices[lschl]].lKill;
	}
	return true;
}

WPStatITEM &CWPStat :: operator [] (long lI){
	return ((WPStatITEM &)(d.FItem[lI]));
}

void CWPStat ::CalcSlice(void){
	long lschl;

	if((!pWPV
		|| !lWPSlice)
		&&bRecalcWPV){

		//prev_pWPV = pWPV;

		if(pWPV){
			delete [] pWPV;
			pWPV = 0;
		}
		lWPSlice = 0;
		lPercent = 0;
		lNWPfWPV = num_waypoints;
		pWPV = new long[lNWPfWPV*lNWPfWPV/16+16];

		//LOG_DEBUG(UTIL_VarArgs("CWPStat::CalcSlice: %p ::: %p", prev_pWPV, pWPV));

		//pWPV = (long*)malloc(lNWPfWPV*(lNWPfWPV/16) * sizeof(long));
		//if(listenserver_edict){DEBUG_CLIENTCOMMAND(listenserver_edict,"say %li",lNWPfWPV);}
		memset(pWPV,0,(lNWPfWPV*lNWPfWPV/16+16) * sizeof(long));

		for(lschl = 0;lschl < num_waypoints;lschl++){
			d.FItem[lschl].iNVWp = 0;
			d.FItem[lschl].fADVWp = 0;
			d.FItem[lschl].fAHDVWp = 0;
			d.FItem[lschl].fMaxDVWp = 0;
			d.FItem[lschl].fMinDVWp = 100000;
		}
	}
	if(!bRecalcWPV
		|| !num_waypoints)
		return;

	long lWP,lWP2;
	/*long lSWP,lSWP2;

	lSWP = lschl / lNWPfWPV;
	lSWP2 = lschl - lWP * lNWPfWPV;*/

	for(lschl = lWPSlice;	lschl < lWPSlice + _WPSTAT_SLICE_SIZE
							&&lschl < (lNWPfWPV+1)*lNWPfWPV
						;	lschl++){
		while(lPercent<lschl){
			LOG_MESSAGE( PLID, "JoeBOT %3.0f percent visibility table complete",100.*float(lPercent)/float(((lNWPfWPV+1)*lNWPfWPV)));
			lPercent += (lNWPfWPV+1)*lNWPfWPV/10;
		}
		lWP = lschl / lNWPfWPV;
		lWP2 = lschl - lWP * lNWPfWPV;

		/*if(lWP2<lWP){
			lWPSlice++;
			continue;
		}*/

		CalcWP(lWP,lWP2);
	}

	lWPSlice += _WPSTAT_SLICE_SIZE;

	if(lWPSlice > (lNWPfWPV+1)*lNWPfWPV){
		LOG_MESSAGE( PLID, "JoeBOT %li/%li visibility table complete",lNWPfWPV,lNWPfWPV);
		bRecalcWPV = false;

		// divide sums to get average values
		for(lschl = 0;lschl < lNWPfWPV;lschl++){
			if(d.FItem[lschl].iNVWp){
				d.FItem[lschl].fADVWp /= float(d.FItem[lschl].iNVWp);
				d.FItem[lschl].fAHDVWp /= float(d.FItem[lschl].iNVWp);
			}
		}
	}
}

void CWPStat :: CalcWP(long lWP,long lWP2){
	if(waypoints[lWP].flags & W_FL_DELETED)
		return;

	long lSet;
	float fDistance;
	TraceResult tr1;
	bool b1;
	Vector VWP,VWPD;
	
	VWPD = waypoints[lWP].origin;
	VWP = VWPD + Vector(0,0,17);
	
	lSet = 0;
	if(waypoints[lWP2].flags & W_FL_DELETED)
		return;
	/*UTIL_TraceLine(VWP,waypoints[lWP2].origin,ignore_monsters,ignore_glass,0,&tr1);
	b1 = tr1.flFraction == 1.0;*/
	b1 = FVisibleEx(VWP,waypoints[lWP2].origin,false,false,true,true,0);
	//b1 = FVisibleMT(VWP,waypoints[lWP2].origin,2,0);
	
	if(b1){
		lSet |= 1;

		fDistance = (waypoints[lWP].origin-waypoints[lWP2].origin).Length();
		
		d.FItem[lWP].iNVWp ++;
		d.FItem[lWP].fADVWp += fDistance;
		d.FItem[lWP].fAHDVWp += (waypoints[lWP].origin.z-waypoints[lWP2].origin.z);
		if(fDistance > 1 && fDistance < d.FItem[lWP].fMinDVWp)
			d.FItem[lWP].fMinDVWp = fDistance;
		if(fDistance > d.FItem[lWP].fMaxDVWp)
			d.FItem[lWP].fMaxDVWp = fDistance;
	}
	
	//UTIL_TraceLine(VWPD,waypoints[lWP2].origin,ignore_monsters,ignore_glass,0,&tr1);
	b1 = FVisibleEx(VWPD,waypoints[lWP2].origin,false,false,true,true,0);
	//b1 = FVisibleMT(VWPD,waypoints[lWP2].origin,2,0);
	
	if(b1){
		lSet |= 2;
	}
	SetVisible(lWP,lWP2,lSet);
	//SetVisible(lWP2,lWP,lSet);
#ifdef _DEBUG
	if(lSet != GetVisible(lWP,lWP2))
		cout << "error" << endl;
#endif
}

int CWPStat :: GetVisible(long l1,long l2){
	if(!pWPV)
		return 0;

	if(l1>=lNWPfWPV
		|| l2>=lNWPfWPV
		|| l1<0
		|| l2<0){
		return 0;
	}

	int iReturn;
	long lElem = l1 * lNWPfWPV + l2;

	long index = lElem / 16;
	long lshift = (lElem - index * 16) * 2;

	iReturn = pWPV[index];
	iReturn>>=lshift;

	return iReturn&3;
}

int CWPStat :: SetVisible(long l1,long l2, long lS){
	//return 0;
	if(!pWPV)
		return -1;

	if(l1>=lNWPfWPV
		||l2>=lNWPfWPV){
		return -1;
	}

	long lDel = 1|2;
	long lElem = l1 * lNWPfWPV + l2;

	lS &= 3;

	long index = lElem / 16;
	long lshift = (lElem - index * 16) * 2;

	lDel <<= lshift;
	lS <<= lshift;

	pWPV[index] &= ~lDel;
	pWPV[index] |= lS;

	return 1;
}
