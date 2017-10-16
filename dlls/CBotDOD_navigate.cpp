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

#include <iostream.h>

#include "extdll.h"
#include "util.h"

#include "CBotDOD.h"

#include "bot.h"
#include "bot_wpstat.h"
#include "Commandfunc.h"
#include "globalvars.h"

bool CBotDOD :: GoToSpEnt(void){
	bool breturn=false;
	if(f_CheckSpEnt < gpGlobals->time){
		breturn = HeadTowardSpEnt();
		if( breturn ){
		}
		else{
			f_CheckSpEnt = gpGlobals->time + 1.0;
		}
	}
	
	/*if(breturn)
	AvoidCollision();*/
	
	return breturn;
}

bool CBotDOD :: HeadTowardSpEnt(void){
	// this function searches the surrounding for interesting ents like hostages
	// or dropped bombs. It returns true, if it has found something and false, if it needs no
	// control. Although bomb targets are ents, they are not handled here, 'cause it's too
	// cpu demanding - that's handled with wp stuff
	
	edict_t *pEnt,
		*pNearP;
	float fDistance;
	float fPDistance/*,fRDistance*/;
	char szClassname[100];
	
	if(pEdict->v.movetype == MOVETYPE_FLY)
		return false;
	
	pNearP = GetNearestPlayer(pEdict,bot_teamnm,fPDistance,true);
	
	pEnt = 0;
	while(pEnt = UTIL_FindEntityInSphere(pEnt,pEdict->v.origin,350)){
		fDistance = Vector(pEnt->v.origin - pEdict->v.origin).Length2D();
		strcpy( szClassname , STRING(pEnt->v.classname) );
		if (FStrEq(szClassname,"dod_control_point") )
		{
			if(FVisible(pEnt->v.origin,pEdict)){
				int iFlag;
				Vector entity_origin = VecBModelOrigin(VARS(pEnt));

				if(FStrEq(STRING(pEnt->v.model),"models/w_gflag.mdl")){
					iFlag = DOD_FLAG_AXIS;
				}
				else if(FStrEq(STRING(pEnt->v.model),"models/w_aflag.mdl")){
					iFlag = DOD_FLAG_ALLIES;
				}
				else{
					iFlag = DOD_FLAG_NONE;
				}
				if(iFlag != bot_teamnm){
					VATurnTo(entity_origin - pEdict->v.origin);
				}
			}
		}
		else if (FStrEq(szClassname,"grenade") ){
			f_LookTo = gpGlobals->time + 3.0;
//			f_GoBack = gpGlobals->time + 3.0;
			//f_noWP = gpGlobals->time + 2.0;
			VLookTo = pEnt->v.origin;
			VRunningTo = pEdict->v.origin + (pEdict->v.origin - pEnt->v.origin - pEdict->v.origin) * 50.0;
		}
		else if (FStrEq(szClassname,"func_button") )
		{
			if(Task.current){
				if(Task.current->p == pEnt && Task.current->lType & BT_GOBUTTON){
					Vector entity_origin = VecBModelOrigin(VARS(pEnt));
					
					fDistance = (entity_origin - pEdict->v.origin).Length();
					
					TraceResult tr;
					UTIL_TraceLine(pEdict->v.origin+pEdict->v.view_ofs,entity_origin,dont_ignore_monsters,ignore_glass,pEdict,&tr);
					if(!FStrEq(STRING(tr.pHit->v.classname),"func_button")){
						continue;
					}
					
					// use the button about 100% of the time, if haven't
					// used a button in at least 5 seconds...
					if ((RANDOM_LONG(1, 100) <= 100) &&
						((f_use_button_time + 5) < gpGlobals->time))
					{
						VATurnTo(entity_origin - pEdict->v.origin);
						// check if close enough and facing it directly...
						
						// if close to button...
						if (fDistance < 50){
							// don't avoid walls for a while
							f_dont_avoid = gpGlobals->time + 1.0;
							VATurnTo(entity_origin - pEdict->v.origin);
							lButton |= IN_USE;
							//Action.lAction&=~BA_GOBUTTON;
							f_use_button_time = gpGlobals->time;
							//iGoal = iFarGoal;
							//iFarGoal = iButtonAGoal;
							Task.NextTask();
						}
						return true;
					}
				}
			}
		}
	}
return false;
}

bool CBotDOD :: HeadTowardWaypoint(void){
	// Do whatever you want here to get the bot to head towards the current
	// waypoint.  Remember to handle movement on land, underwater, on ladders,
	// and in the air (jetpacks) if necessary.  Return TRUE if the bot can
	// still get to the next waypoint, or FALSE if the next waypoint is now
	// unreachable for some reason.
	
	int iNextWP;
	//char szBuffer[100];
	//	short int /*iWPIndex,*/;
	float fMin = 1000/*,fNDistance*/;
	float fCDistance;
	
	// to have some time without going to WPs
	if(f_noWP > gpGlobals->time){
		//DEBUG_CLIENTCOMMAND(pEdict,"say not using wp");
		return false;
	}
	
	/*	if(fTest< gpGlobals->time){
	fTest += .1f;
	return true;
}*/
	
	if(Task.current && Task.current ->lType & BT_CAMP){
		if(Camp())
			return true;
	}
	else{
		WLookTo.iNum = 0;
	}
	
	/*// avoid to get stuck trying to reach a waypoint
	if(f_lastWP + MAXTIMEPERWP < gpGlobals->time){
	iGoal = -1;
	//iFarGoal = -1;
	f_lastWP = gpGlobals -> time;
}*/
	
	if(iFarGoal != -1){			// Bot has destination
		if(i_CurrWP == -1){		// bot has no wp currently aiming to
			i_CurrWP = WaypointFindNearest(pEdict,100,bot_teamnm,0,true,true,true);
			if(i_CurrWP == -1){
				i_CurrWP = WaypointFindNearest(pEdict,200,bot_teamnm,0,true,false,true);
				if(i_CurrWP == -1){
					i_CurrWP = WaypointFindNearest(pEdict,500,bot_teamnm,0,false,false,true);
				}
			}
			if(i_CurrWP == -1){			// if there's no wp
				iGoal = -1;
				return false;
			}
		}
		
		if(iGoal == -1){			// if there's no temp goal, just use the next wp as goal
			iGoal = i_CurrWP;
		}
		
		if(iOnLadder == 0){			// only when not on ladder
			HeadToward(waypoints[i_CurrWP].origin);	
			//VRunningTo = waypoints[i_CurrWP].origin; //should have been copied when next wp
		}
		
		if(g_b5th)
			iNearWP = WaypointFindNearest(pEdict,_NEAR,bot_teamnm);
		
		if(iNearWP != -1){
			//fNDistance = (waypoints[iNearWP].origin-pEdict->v.origin).Length2D();
			fCDistance = (waypoints[i_CurrWP].origin-pEdict->v.origin).Length2D();
			
			f_D2C2 = f_D2C1;
			f_D2C1 = fCDistance;
			
			bool bReturn;
			if(HandleNearWP(iNearWP,bReturn))
				return bReturn;
			
			if(iNearWP == iGoal){
				//WaypointDrawBeam(pEdictPlayer,pEdict->v.origin,pEdict->v.origin+Vector(0,0,50),10,10,0,200,0,200,10);
				// bot has reached temporary goal. That means that now has to be tested if this is the far goal, too
				if(iNearWP == iFarGoal){
					//WaypointDrawBeam(pEdictPlayer,pEdict->v.origin,pEdict->v.origin+Vector(0,50,50),10,10,0,200,0,200,10);
					//cout << "reached fargoal" << endl;
					if(Task.current
						&& (Task.current->lType & BT_CAMPATGOAL)
						&& Task.current->lAdd == iNearWP){
						float fDuration = gpGlobals->time + Task.current->fAdd;
						Task.NextTask();
						//Task.current->lType |= BT_DEL;
						if(f_noCamp < gpGlobals->time){
							//cout << "reached campatgoal" << endl;
							Task.AddTask(BT_CAMP,fDuration,0,0,0);
							//DEBUG_CLIENTCOMMAND(pEdict,"say %f",Task.current->fAdd);
							InitCamp();
							if(CVAR_BOOL(jb_msgradio) && Task.current->p){
								SendRadioCommand(RADIO_IM_IN_POSITION);
							}
							if(f_RWKnife > gpGlobals->time)
								f_RWKnife = gpGlobals->time;
						}
						return false;
					}
					else if(Task.SearchT(BT_FLEE) != -1){
						//f_Camp = gpGlobals->time + 1000.0f; // just stay there till the bomb explodes
						Task.AddTask(BT_CAMP,gpGlobals->time + 1000.0,0,0,0);

						//cout << "waiting for bomb to explode" << endl;
					}
					if(Task.current){
						if(Task.current->lType & BT_GOTO){
							if(Task.current->lAdd == iFarGoal){
								//cout << "reached task goal" << endl;
								Task.NextTask();
							}
						}
					}
					ResetWPlanning();
				}
				else{
					FindRoute(iNearWP);
				}
				//}
				return false;
			}
			
			if(/*iNearWP == i_CurrWP
				|| */f_D2C1 > f_D2C2			// f_D2C2 is the older distnace
				||fCDistance < 10
				||(fCDistance < 50 && !(waypoints[i_CurrWP].flags & W_FL_JUMP) && !b_neardoor)){		// the bot has reached his temporary aim/wp
				if(fCDistance < 50){
					//lNextWP = IsInWay(FW_Cur,i_CurrWP);		// look where this near wp is in the calced way
					
					//DEBUG_CLIENTCOMMAND(pEdict,"say %f",fCDistance-f_D2C);
					iNextWP = WaypointRouteFromTo(iNearWP,iGoal,bot_teamnm);
					
					if(waypoints[iNearWP].flags & W_FL_JUMP){
						bool b1near=false;
						float fMin;
						lButton |= IN_JUMP;
						
						ADV_WPRecMove *pADVM;
						pADVM = WaypointGetADV(i_CurrWP,iNextWP);
						if(pADVM){
							fMin=0;
							edict_t *pNearest = GetNearestPlayer(pEdict,-1,fMin,false,false,60);
							if(pNearest)
								b1near |= true;
							fMin=0;
							pNearest = GetNearestPlayer(waypoints[pADVM->iIndexEndWP].origin,-1,fMin,60,pEdict);
							if(pNearest)
								b1near |= true;
							
							if(!b1near){
								pWPAMPlay = pADVM;
								bReplay = true;
								fStartADVM = gpGlobals->time;
								pEdict->v.origin = pWPAMPlay->Rec[0].v_origin;
							}
							else{
								f_Pause = gpGlobals->time + 1.f;
								f_noWP = gpGlobals->time + 1.f;
								f_move_speed = 0;
								lButton &= ~IN_JUMP;
								DEBUG_CLIENTCOMMAND(pEdict,"say waiting because there is already a motherfucker in the way");

								f_D2C1 = 1000;
								return false;
							}
						}
						else{
							f_ducktill = gpGlobals->time + .5;
						}
					}
					
					if(iNextWP == WAYPOINT_UNREACHABLE
						|| iNextWP == iNearWP
						|| iNextWP > WAYPOINT_UNREACHABLE/2){
						DEBUG_CLIENTCOMMAND(pEdict,"say lost way");
						//cout << "unreachable" << endl;
						ResetWPlanning();
						
						if(Task.current){
							if(Task.current->lType & BT_GOTO){
								Task.NextTask();
							}
						}
						
						return false;
					}
					else{
						DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin+Vector(0,0,30),waypoints[iNextWP].origin+Vector(0,0,30),20,0,200,200,200,100,20);
					}
					
					i_LastWP = i_CurrWP;
					i_CurrWP = iNextWP; 
					f_D2C1 = f_D2C2 = 10000.0;		// that means far away
					
					VRunningTo = waypoints[i_CurrWP].origin;		// run there
					
					HeadToward(VRunningTo);
					
					// update time of last WP - visit
					f_lastWP = gpGlobals->time;
					
					//if(pEdictPlayer)WaypointDrawBeam(pEdictPlayer,pEdict->v.origin,Vector(0,0,0),10,10,200,200,200,200,1);
					
					if(	  !g_bBombPlanted
						&& fM[bot_teamnm]<0.0
						&& g_fRoundTime < 60.0
						&& g_fRoundTime > 15.0
						&& f_noCamp < gpGlobals->time
						&& iNearWP != -1
						&&!HasSniper()
						&&!(waypoints[iNearWP].flags & W_FL_LADDER)
						&& f_LastFight < gpGlobals->time - 5.0){		// look if it's good to camp here
						if(RANDOM_LONG(0,100)<70){
							//f_noCamp = gpGlobals->time + 10.0f;
							int i,i1,index=-1;
							float distance;
							bool bTTime;
							
							GetNearestPlayer(pEdict,bot_teamnm,fMin);
							if(fMin>50){
								for (i=0; i < num_waypoints; i++)
								{
									if ((waypoints[i].flags & W_FL_DELETED) == W_FL_DELETED)
										continue;  // skip any deleted waypoints
									
									if (waypoints[i].flags & W_FL_AIMING)
										continue;  // skip any aiming waypoints
									
									/*if(!FVisible(waypoints[i].origin,pEdict))
										continue;*/
									if(!WPStat.GetVisible(i,iNearWP))
										continue;
									
									Vector2D vec2LOS;
									float    flDot;
									
									UTIL_MakeVectors ( pEdict->v.angles );
									
									vec2LOS = ( waypoints[i].origin - pEdict->v.origin ).Make2D();
									vec2LOS = vec2LOS.Normalize();
									
									flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );
									
									if ( flDot < 0.0)  // 180 degree field of view 
										continue;
									
									distance = (waypoints[i].origin - pEdict->v.origin).Length();
									
									if(WPStat[i].fADEn > 800)
										continue;
									
									if(distance > 600)
										continue;
									
									bTTime = false;
									for(i1=0;i1<_MAX_LTMEM;i1++){
										if(WPStat[i].fpKilledTime[i1]>1.0
											&&(TEq(g_fRoundTime+5,WPStat[i].fpKilledTime[i1],7.0))){
											bTTime = true;
											break;
										}
									}
									if(!bTTime){
										for(i1=0;i1<_MAX_LTMEM;i1++){
											if(WPStat[i].fpKillTime[i1]>1.0
												&&(TEq(g_fRoundTime+5,WPStat[i].fpKillTime[i1],7.0))){
												bTTime = true;
												break;
											}
										}
									}
									
									if(!bTTime && WPStat[i].lKilled < WPStat.lKilledMax*2/3)
										continue;
									
									index = i;
									break;
								}
								if(index != -1){
									HeadToward(waypoints[i].origin);
									Task.AddTask(BT_CAMP, gpGlobals->time + 14.0,0,0,0);		// set time to camp
									//f_Hide = gpGlobals->time + 14.f;
									//									pHidefrom = 0;
									f_StrCamp=gpGlobals->time;
									
									sLookToChange = 1;
									InitCamp();
									f_CaLooktoWP = gpGlobals->time + 1.5f;
									f_RWKnife = gpGlobals->time - 1.0;
									if(RANDOM_LONG(0,100) <70){
									/*Change2Weapon(CS_WEAPON_HEGRENADE);
										bThrowHEGrenade = true;*/
										// todo find a location where an enemy can be exspected soon
										int i2TT = WPStat.Search(bot_teamnm,iNearWP,WPS_SEARCH_ENEMYESP,0,1000,gpGlobals->time,10.f,pEdict,true);
										if ( i2TT != -1){
											GOrder.lTypeoG = 0;
											if(RANDOM_LONG(0,100) < 40 && bot_weapons & (1<<CS_WEAPON_SMOKEGRENADE))
												GOrder.lTypeoG = CS_WEAPON_SMOKEGRENADE;
											else if (bot_weapons & (1<<CS_WEAPON_HEGRENADE))
												GOrder.lTypeoG = CS_WEAPON_HEGRENADE;
											GOrder.VAim = waypoints[i2TT].origin;
											GOrder.lState = 1;
											//cout << "----------------------------- enemyesp ------------------------------"<<endl;
										}
									}
								}
							}
						}
						}
						return true;
				}
				else{		// bot get's more far from current aim waypoint, but he is > 50 unit's away
					i_CurrWP = -1;
				}
			}
			else{		// hasn't reached aim yet
			}
			//f_D2C = fCDistance;		// copy distance to current wp to bot structure
		}
		return true;
	}
	else{					// Bot has no way
		if(Task.current){
			if(Task.current->lType & BT_FOLLOW){
				if(UTIL_ClientIndex(Task.current->p)!=-1){
					float fDistance = (pEdict->v.origin-((edict_t *)Task.current->p)->v.origin).Length();
					if(fDistance < 300){
						if(fDistance > 100){
							HeadToward(((edict_t *)Task.current->p)->v.origin);
							VRunningTo = ((edict_t *)Task.current->p)->v.origin;
							return true;
						}
						else{
							Vector VAdd = pEdict->v.origin - ((edict_t *)Task.current->p)->v.origin;		// circle
							VAdd = CrossProduct(VAdd,Vector(0,0,1));
							VAdd.Normalize();
							VAdd = ((edict_t *)Task.current->p)->v.origin + VAdd * 70.0;
							HeadToward(VAdd);
							VRunningTo = VAdd;
							return true;
						}
					}
				}
			}
		}
		//DEBUG_CLIENTCOMMAND(pEdict,"say -");
		i_CurrWP = WaypointFindNearest(pEdict,100,bot_teamnm,0,true,true,true);
		if(i_CurrWP == -1){
			i_CurrWP = WaypointFindNearest(pEdict,200,bot_teamnm,0,true,false,true);
			if(i_CurrWP == -1){
				i_CurrWP = WaypointFindNearest(pEdict,500,bot_teamnm,0,false,false,true);
			}
		}
				
		if(i_CurrWP == -1)
			return false;

		iFarGoal = iGoal = i_CurrWP;
		VRunningTo = waypoints[i_CurrWP].origin;
		
		//DEBUG_CLIENTCOMMAND(pEdict,"say heading to");
		//if( (pEdict->v.origin - waypoints[i_CurrWP].origin).Length() < _NEAR ){
		//if( WaypointFindNearest(pEdict,_NEAR,bot_teamnm) != -1 ){
		if( (waypoints[i_CurrWP].origin - pEdict->v.origin).Length() < _NEAR*2 ){
			// Near a waypoint - create a way
			// but before, test on map type, team ...
			DecideOnWay();
		}
		HeadToward(waypoints[i_CurrWP].origin);
		VRunningTo = waypoints[i_CurrWP].origin;
		return true;
	}
	return FALSE;  // couldn't get to next waypoint
}

void CBotDOD :: FindRoute(int iNearWP){
	struct foundway{
		short int index;
		CWay Way1;
		CWay Way2;
		WAYINFO wayinfo1;
		WAYINFO wayinfo2;
	};
	//cout << sizeof(foundway) * 250<<endl;
	edict_t *pToUse;
	if(iNearWP == iFarGoal){
		// bot has reached his 'final' destination, therefore delete this and return
		//cout << "findroute : reached goal location " << endl;
		ResetWPlanning();
		return;
	}
	else{
		// bot has just reached a temporary goal. Look if the way should be divided ...
		if(iWantedDiv > 0 ){
			// way should be divided
			int iDistS2FG = WaypointDistanceFromTo(iNearWP,iFarGoal,bot_teamnm);
			if(iDistS2FG == 0){
				iDistS2FG = 500;
			}
			if(iDistS2FG < 400){		// don't divide for near goals
				iWantedDiv=0;
				iGoal = iFarGoal;
			}
			int iDistD2FG,iDistS2D;
			
			int index;

			int count = 0;
			int precount = 0;
			int iStore;
			PATH *ppath = 0;
			int p;
			int iTest;
			int ischl;
			long lMinKill=LONG_MAX,
				lMaxKill=LONG_MIN,
				lMinKilled=LONG_MAX,
				lMaxKilled=LONG_MAX;
			float fThreshold;

			static foundway indexes[_MAXTEMPDIV];
			static short int preindexes[_MAXTEMPDIVPRE];
			
			// find all the waypoints with the matching flags ...
			// and fitting into some conditions ( in between iNearWP and iFarGoal )
			
			for (index=0; index < num_waypoints; index++)
			{
				if (waypoints[index].flags & W_FL_DELETED)
					continue;  // skip any deleted waypoints
				
				if (waypoints[index].flags & W_FL_AIMING)
					continue;  // skip any aiming waypoints
				
				// skip this waypoint if it's team specific and teams don't match...
				if ((waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
					((waypoints[index].flags & W_FL_TEAM) != bot_teamnm))
					continue;
				
				iDistD2FG = WaypointDistanceFromTo(index,iFarGoal,bot_teamnm);
				iDistS2D  = WaypointDistanceFromTo(iNearWP,index,bot_teamnm);
				
				if(iDistD2FG + iDistS2D > iDistS2FG * 5 / 2)		// don't go too far away
					continue;
				
				if(iDistD2FG < iDistS2FG/3 
					|| iDistS2D < iDistS2FG/3)	// is not out of this area
					continue;
				
				if (precount < _MAXTEMPDIVPRE)			// add and get further information
				{
					preindexes[precount] = index;

					precount++;
				}
				else{
					//cout << "counting overflow in findroute - "<<_MAXTEMPDIV<<endl;
				}
			}
			//cout << "count has reached : "<<count<<endl;
			// remove near wps
			
			index = 0;
			while(index < precount){		// just all stored wps
				if(preindexes[index]!=-1){
					ppath = paths[preindexes[index]];	// test all paths
					while(ppath){
						p=0;
						while(p < MAX_PATH_INDEX){
							iTest = ppath->index[p];
							if(iTest != -1){
								for(ischl=0;ischl < precount;ischl++){
									if(preindexes[ischl] == iTest
										&& index != ischl){		// the connected wp is in this list, so remove it
										preindexes[ischl] = -1;
										break;
									}
								}
							}
							p++;
						}
						ppath = ppath->next;
					}
				}
				index++;
			}

			// convert the waypoints got this way to ways to make them usable for getting
			// statistical information and information about their 'hostageability'

			index = 0;
			count = 0;
			iStore = 0;
			while(index < precount){		// just all stored wps
				if(preindexes[index] != -1){
					if(count < _MAXTEMPDIV){
						iStore = count;
						count ++;
					}
					else{
						iStore = RANDOM_LONG(0,_MAXTEMPDIV - 1);
					}
	
					indexes[iStore].index = preindexes[index];				
				}
				index ++;
			}

			//cout << precount << " : "<<count<<" : ";
			index = 0;
			while(index < count){
				ConvertFloyd2Way(bot_teamnm,iNearWP,indexes[index].index,&indexes[index].Way1);
				ConvertFloyd2Way(bot_teamnm,indexes[index].index,iFarGoal,&indexes[index].Way2);
				
				indexes[index].wayinfo1.lStatKill = 0;
				indexes[index].wayinfo1.lStatKilled = 0;
				indexes[index].wayinfo1.lSTMVisible = 0;
				indexes[index].wayinfo1.bHostageable = true;
				WPStat.GetWayInfo(indexes[index].Way1,indexes[index].wayinfo1);
				STMem.GetWayInfo(indexes[index].Way1,indexes[index].wayinfo1);
				indexes[index].wayinfo2.lStatKill = 0;
				indexes[index].wayinfo2.lStatKilled = 0;
				indexes[index].wayinfo2.lSTMVisible = 0;
				indexes[index].wayinfo2.bHostageable = true;
				WPStat.GetWayInfo(indexes[index].Way2,indexes[index].wayinfo2);
				STMem.GetWayInfo(indexes[index].Way2,indexes[index].wayinfo2);
				
				// // // // // // // // // // // // // HACKHACK;
				// just add to killed the value of seen stuff -> makes bot takes a way which is not in view of enemy
				indexes[index].wayinfo1.lStatKilled+=indexes[index].wayinfo1.lSTMVisible*WPStat.lKilledMax;

				if(indexes[index].Way1.iNum > 3		// sackgasse ?
					&& indexes[index].Way2.iNum > 3){
					if(indexes[index].Way1.iIndices[indexes[index].Way1.iNum-2] == indexes[index].Way2.iIndices[1]){
						//DEBUG_CLIENTCOMMAND(pEdict,"say going back - we don't really want that, are we ?");
						indexes[index].index = -1;
					}
				}
				
				index ++;
			}
			
			// end remove near wps
			
			// find minima and maxima
			for(index=0;index < count;index++){
				if(indexes[index].index == -1)
					continue;
				
				if(indexes[index].wayinfo1.lStatKill > lMaxKill){
					lMaxKill = indexes[index].wayinfo1.lStatKill;
				}
				if(indexes[index].wayinfo1.lStatKill > lMinKill){
					lMinKill = indexes[index].wayinfo1.lStatKill;
				}
				if(indexes[index].wayinfo1.lStatKilled > lMaxKilled){
					lMaxKilled = indexes[index].wayinfo1.lStatKilled;
				}
				if(indexes[index].wayinfo1.lStatKilled > lMinKilled){
					lMinKilled = indexes[index].wayinfo1.lStatKilled;
				}
			}
			
			// if the bots wants a special way, which is p.ex. safe, dangerous or whatever, remove the ways, which don't fit
			if(lTypeOWW != WW_NONE){
				if(lTypeOWW == WW_VAGG||lTypeOWW == WW_AGG){
					if(lTypeOWW == WW_VAGG){
						fThreshold = float(lMaxKilled) * .8f;
					}
					else if(lTypeOWW == WW_AGG){
						fThreshold = float(lMaxKilled) * .5f;
					}
					
					for(index=0;index < count;index++){
						if(indexes[index].index == -1)
							continue;
						if(indexes[index].wayinfo1.lStatKilled < fThreshold){
							indexes[index].index = -1;
						}
					}
				}
				if(lTypeOWW == WW_VDEF||lTypeOWW == WW_DEF){
					if(lTypeOWW == WW_VDEF){
						fThreshold = float(lMinKilled) / .8f;
					}
					else if(lTypeOWW == WW_DEF){
						fThreshold = float(lMinKilled) / .5f;
					}
					
					for(index=0;index < count;index++){
						if(indexes[index].index == -1)
							continue;
						if(indexes[index].wayinfo1.lStatKilled > fThreshold){
							indexes[index].index = -1;
						}
					}
				}
			}

			// remove the -1s;
			index = count-1;
			while(index >= 0 && index < count){
				if(indexes[index].index == -1){
					count --;
					
					if(index != count){
						indexes[index] = indexes[count];
					}
				}
				index--;
			}
			//cout << count << endl;
			
			if(count){
				iGoal = indexes[RANDOM_LONG(0, count - 1)].index;
			}
			else{
					iGoal = iFarGoal;
			}
			
			iWantedDiv --;
			
			/*delete [] indexes;
			delete [] preindexes;*/
			
			TestOnButtonWay(&pToUse);
			
			if(pToUse){
				int iDest = WaypointFindNearest(VecBModelOrigin(VARS(pToUse)),pEdict,400,bot_teamnm);
				if(iDest != -1
					&&Task.SearchT(BT_GOBUTTON) == -1){
					Task.AddTask(BT_GOTO|BT_GOBUTTON,-1,iDest,pToUse,0);
				}
				//DEBUG_CLIENTCOMMAND(pEdict,"say wanting to go where pressing a button is needed");
			}
	}
	else{
		// no divisions wanted, just make the final goal the current one
	
		iGoal = iFarGoal;
	}
}
}


bool CBotDOD :: Bored(void){
	//return false;
	float fbias = gpGlobals->time - TIMETOBEBORED;
	
	if(iOnLadder != 0)		// don't be bored on ladders
		return false;
	
	if(f_LastFight < fbias	// only do this after 30s without combat
		&& f_Bored < fbias){// only do 'boring attacks' every 30s
		f_BoredTill = gpGlobals->time + TIMEBEINGBORED;
		f_Bored = gpGlobals->time + RANDOM_FLOAT(-10.0,10.0);
		i_BorA = int(RANDOM_FLOAT(0,4));
	}
	
	if(f_BoredTill > gpGlobals->time){// still bored ??
		if(i_BorA == 1){
			edict_t *pEnt;
			TraceResult tr;
			Vector entity_origin;
			
			pEnt = 0;
			while(pEnt = UTIL_FindEntityByClassname(pEnt,"func_breakable")){	// shoot @ breakable items ...
				if(pEnt->v.takedamage == DAMAGE_YES){
					entity_origin = VecBModelOrigin(VARS(pEnt));
					//check if'ts visible
					
					UTIL_TraceLine(pEdict->v.origin + pEdict->v.view_ofs, entity_origin,
						dont_ignore_monsters, ignore_glass, pEdict->v.pContainingEntity, &tr);
					if(FVisible(entity_origin,pEdict)){
						if(tr.flFraction == 1.0f||FStrEq(STRING(tr.pHit->v.classname),"func_breakable")){
							// aim
							Vector v_enemy = entity_origin - GetGunPosition(pEdict);
							
							pEdict->v.v_angle = UTIL_VecToAngles( v_enemy );
							
							// Paulo-La-Frite - START bot aiming bug fix
							if (pEdict->v.v_angle.x > 180)
								pEdict->v.v_angle.x -=360;
							
							// set the body angles to point the gun correctly
							pEdict->v.angles.x = pEdict->v.v_angle.x / 3;
							pEdict->v.angles.y = pEdict->v.v_angle.y;
							pEdict->v.angles.z = 0;
							
							// adjust the view angle pitch to aim correctly (MUST be after body v.angles stuff)
							pEdict->v.v_angle.x = -pEdict->v.v_angle.x;
							// Paulo-La-Frite - END
							
							HeadToward(entity_origin);
							
							lButton |= IN_ATTACK;
							//BotFireWeapon(entity_origin,pBot);
							//DEBUG_CLIENTCOMMAND(pEdict,"say bored");
							return true;
						}
					}
				}
			}
		}
		else if(i_BorA == 2){			// Bot is bored, so he behaves in a social way and wants to meet teammates ;)
			if(!Task.Important()){
				Task.AddTask(BT_ROAMTEAM,gpGlobals->time + 20.0,0,GetRandomPlayer(pEdict,bot_teamnm,1),0);
			}
		}
		else if(i_BorA == 3){						// just wait some time there
			if(!Task.Important()){
				Task.AddTask(BT_CAMP, gpGlobals->time + 5.0f,0,0,0);
				//f_Hide = gpGlobals->time + 5.0f;
//				pHidefrom = 0;
				InitCamp();
			}
		}
	}
	return false;
}

bool CBotDOD :: DecideOnWay(void){
	int iNearest = WaypointFindNearest(pEdict,300,bot_teamnm);
	
	//DEBUG_CLIENTCOMMAND(pEdict,"say generating new way");
	
	iWantedDiv = 0;		// by default the are no wanted divisions
	
	lTypeOWW = WW_NONE;		// default ... do nothing 'bout experiences
	
	if(d_Manner > 1.0 || RANDOM_LONG(0,100)<20){
		lTypeOWW = WW_AGG;
	}
	else if(d_Manner <= -1.0 || RANDOM_LONG(0,100)<20){
		lTypeOWW = WW_DEF;
	}
	
	if(pEdict->v.health < 50 || RANDOM_LONG(0,100)<20){
		lTypeOWW = WW_VDEF;
	}
	
	//cout << Task.current << " - " << Task.lNOT << endl;
	ResetWPlanning();
	
	if(Task.current){			// first of all -> is there a order to fulfill ??
		//cout << "there is a task : " << Task.current->lType <<endl;
		if(Task.current->lType & BT_GOTO){
			ResetWPlanning();
			
			iFarGoal = Task.current -> lAdd;
			if(Task.current->lType & BT_GOBUTTON
				||Task.current->lType & BT_HIDE){
				iWantedDiv = 0;
			}
			else{
				iWantedDiv = (int)(Task.current->p);
			}
			//cout << "performing task : goto" << endl;
			return true;
		}
		else{
			// just take the next task. if u don't want a task to be deleted by this routine just add a if(){ structure above
			//Task.NextTask();
			//cout << "nimptask -> next " << endl;
		}
	}
	else{
		int iDestination = -1;
		// is there something in the bots short term mem ?
		if(STMem.iCount){
			int iMemI=0;

			while(iDestination==-1&&iMemI<STMem.iCount){
				iDestination = WaypointFindNearest(STMem[iMemI].pEdict,500,bot_teamnm,0,true,false);		//hackhack bot doesn't know the exact position of enemy
				iMemI ++;
			}
			iWantedDiv=0;
			if(iDestination != -1){
				Task.AddTask(BT_GOTO,0,iDestination,(void*)iWantedDiv,0);
				return true;
			}
		}
		
		if(HasSniper()			// go to sniper point, if ya've a good sniper weapon
			&& RANDOM_LONG(0,100) < 50){
			if(RANDOM_LONG(0,100) < 70 && WPStat.d.lKill > 20){
				iDestination = WPStat.Search(bot_teamnm,iNearest,WPS_SEARCH_SNIPER,400,2000);
				if(iDestination != -1 && !Task.Important()){
					Task.AddTask(BT_CAMPATGOAL,20,iDestination,(void*)iWantedDiv,0);
				}
			}
			else{
				if(RANDOM_LONG(0,100) < 70)
					iDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
				else
					iDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
				if(iDestination != -1){
					Task.AddTask(BT_GOTO,0,iDestination,(void*)iWantedDiv,0);
					return true;
				}
			}
			if(iDestination != -1){
				Task.AddTask(BT_GOTO,0,iDestination,(void*)iWantedDiv,0);
				return true;
			}
		}
		// just to go nearest flag if there is one
		else
		{
			if(g_DODFlags[0].pEdict){
				Vector VNearest;
				float fDistance,fNearest = 10000.0;
				bool bFound = false;
				int ischl = 0;
				int iWPN = WaypointFindNearest(pEdict,500);
				for(ischl = 0;g_DODFlags[ischl].pEdict;ischl++){
					//cout << "-";cout.flush();
					if(!g_DODFlags[ischl].iFlag 
						|| g_DODFlags[ischl].iFlag != bot_teamnm){
						fDistance = WaypointDistanceFromTo(iWPN,WaypointFindNearest(g_DODFlags[ischl].vOrigin,pEdict,10000,bot_teamnm),bot_teamnm);
						if(fDistance < fNearest){
							fNearest = fDistance;
							VNearest = g_DODFlags[ischl].vOrigin;
							bFound = true;
							//cout << "found one"<<endl;
						}
					}
				}
				if(bFound){
					iDestination = WaypointFindNearest(VNearest,pEdict,100,bot_teamnm);
				}
				else{
					if(RANDOM_LONG(0,100)<50){
						// choose a random location
						iDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,10000,bot_teamnm,0);
					}
					else{
						long lCount = 0;
						for(ischl = 0;g_DODFlags[ischl].pEdict;ischl++)
							lCount++;
						iDestination = WaypointFindNearest(g_DODFlags[RANDOM_LONG(0,lCount)].vOrigin,pEdict,1000,bot_teamnm);
					}
				}
			}
			else{	// no flags there
				iDestination = WaypointFindRandomGoal (pEdict->v.origin,pEdict,10000,bot_teamnm,W_FL_TARGET);
			}
			if(iDestination != -1){
				Task.AddTask(BT_GOTO,0,iDestination,(void*)iWantedDiv,0);
				return true;
			}
			else
				cout << "noflagstuff,cause of -1" << endl;
			return true;
		}
	}
	
	ResetWPlanning();
	return false;
}

void CBotDOD :: UpdateFlags(void){
	if(fDODFlagU>gpGlobals->time-_DOD_TIMETOUPDATEFLAGS)
		return;
	int ischl;
	for(ischl=0;ischl < _MAXFLAGS ;ischl++){
		g_DODFlags[ischl].pEdict = 0;
	}
	fDODFlagU = gpGlobals->time;
	ischl = 0;
	edict_t *pEnt;
	pEnt = 0;
	//if(listenserver_edict){DEBUG_CLIENTCOMMAND(listenserver_edict,"say updating flags");}

	while(pEnt = UTIL_FindEntityByClassname(pEnt,"dod_control_point")){
		if(FStrEq(STRING(pEnt->v.model),"models/w_gflag.mdl")){
			g_DODFlags[ischl].iFlag = DOD_FLAG_AXIS;
		}
		else if(FStrEq(STRING(pEnt->v.model),"models/w_aflag.mdl")){
			g_DODFlags[ischl].iFlag = DOD_FLAG_ALLIES;
		}
		else{
			g_DODFlags[ischl].iFlag = DOD_FLAG_NONE;
		}

		g_DODFlags[ischl].pEdict = pEnt;
		g_DODFlags[ischl].vOrigin = pEnt->v.origin;

		ischl ++;
		if(ischl >= _MAXFLAGS)
			break;
	}
}

bool CBotDOD :: HandleNearWP(int iNearest, bool &bReturn){
	return CBotBase :: HandleNearWP(iNearest,bReturn);
}

bool CBotDOD :: IsWeaponBetterCurrent(long lWeaponID){
		return false;
}
