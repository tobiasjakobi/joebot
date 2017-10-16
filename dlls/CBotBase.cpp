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

#include "CBotBase.h"

#include "bot.h"
#include "bot_modid.h"
#include "Commandfunc.h"
#include "globalvars.h"

#include "NeuralNet.h"
#include "nVec.h"

//#define __COLLLINE

FEItem :: FEItem(){
	fLastSeen = -1;
	fRecogn = -1;
	fTime2seeit = -1;

	fDistance = 0;
	fDotView = 0;
	bBehindSG = false;
	bProne = false;
	bSooting = false;
	lButton = false;
	lFlags = 0;
}

FEItem :: ~FEItem(){
}

GrenadeOrder :: GrenadeOrder(){
	lTypeoG = 0;
	lState = 0;
	pAim = 0;
}

GrenadeOrder::~GrenadeOrder(){
}

void GrenadeOrder::Attack(Vector &VP, long lType){
	//cout << "gorder.attack" << endl;
	VAim = VP;
	lTypeoG = lType;
	lState = 1;
	pAim = 0;
	fStart = gpGlobals->time;
}

CBotBase :: CBotBase():LTMem(this){
	// init some global vars, they are nvtl recaluclated before next think
	g_msecnum = 0;
	g_msecdel = 0;
	g_msecval = 0;

	// reset index of LKW
	for(iCLKW = _MAXLKW;iCLKW;iCLKW--){
		FLKW[iCLKW-1]=-1;
	} // and now iCLKW is 0
	iCLKW = 0;

	pEdict=0;
	need_to_initialize =0;
	*name=0;
	*skin=0;

	not_started = true;
	start_action = 0;
	
	f_Delay = 0;
	f_PauseShoot = 0;
	f_DenyWChange = 0;
	f_TKPunish = 0;
	f_StrCamp = 0;
	
	iOnLadder = 0;
	pELadder = 0;
	
	lButton = 0;
	
	bot_team = 0;
	bot_teamnm = 0;
	bot_class = 0;
	bot_health = 0;
	bot_armor = 0;
	bot_weapons = 0;
	bot_skill = 0;
	
	bFlashlight = 0;
	
	fViewSpeedYaw = 0;
	fViewSpeedPitch = 0;
	fIdealAnglePitch = 0;
	fIdealAngleYaw = 0;
	fAngleSpeedPitch = 0;
	fAngleSpeedYaw = 0;
	
	f_ReactionTime = 0;
	
	yaw_degrees = 0;
	pitch_degrees = 0;
	old_yaw_degrees = 0;
	old_pitch_degrees = 0;
	
	lTypeOWW = 0;
	iGoal = -1;
	iFarGoal = -1;
	iWantedDiv = 0;
	
	pNEn = 0;
	pNTeam = 0;
	
	pBotEnemy = 0;
	pLastGlobalKill = 0;
	f_SearchEnemy = 0;
	pBotOldEnemy = 0;
	f_Situation = 0;
	v_Offset = Vector(0,0,0);
	f_AMomentum = 0;
	f_ASpeed = 0;
	f_AimHead = 0;
	lEnWeapon = 0;
	
	Action.lAction = 0;
	Ordered.lAction = 0;
	
	f_RWKnife = 0;
	f_NJumpTill=0;
	f_TimeBJumps=1;

	f_MaxRecoil = 20;

	v_avoid = Vector(0,0,0);
	
	f_IOrigD = 0;
	f_LTSFIP = 0;
	f_LookTo = 0;
//	f_GoBack = 0;
	GOrder.lTypeoG = 0;
	VRunningTo = Vector(0,0,0);
	VLookTo = Vector(0,0,0);
	
	VSuspEn = Vector(0,0,0);
	f_SuspRem = 0;
	f_CheckSpEnt = 0;
	
	killer_edict = 0;
	pEKilled = 0;
	pELVictim = 0;
	LastEnemyOrigin = Vector(0,0,0);
	VHidingPlace = Vector(0,0,0);
	f_UsedRadio = 0;
	f_shootbox = 0;
	bSeeThru = 0;
//	f_GotoHidingPlace = 0;
	iJumpOC = 0;
	//f_Hide = 0;
	f_HidingTime = 0;
//	pHidefrom = 0;
	
	f_max_speed = 0;
	f_strafe = 0;
	prev_speed = 0;
	v_prev_origin = Vector(0,0,0);
	f_ducktill = 0;
	f_lastWP = -1;
	f_ResetWP = 0;
	f_noWP = 0;
	f_dont_check_stuck = 0;
	f_bot_stuck = 0;
	lStuckC = 0;
	f_noStuck = 0;
	//f_Camp = 0;
	f_noCamp = 0;
	f_CaLooktoWP = 0;
	WLookTo.iNum = 0;
	sLookToChange = 0;
	f_Pause = 0;
	f_blinded_time = 0;
	f_round_time = 0;
	f_end_use_ladder_time = 0;
	f_start_use_ladder_time = 0;
	f_use_button_time = 0;
	
	f_dont_avoid = 0;
//	f_avoid_strafe = 0;
	i_avoidstrafe = 0;
	f_gotohostage = 0;
	f_Bored = 0;
	i_BorA = 0;
	f_BoredTill = 0;
	f_NNNUpdate = 0;
	memset(&dCombatNNIn,0,sizeof(double)*_NNBInput);
	memset(&dCombatNNOut,0,sizeof(double)*_NNBOutput);
	memset(&dCollNNIn,0,sizeof(double)*3);
	memset(&dCollNNOut,0,sizeof(double)*1);
	f_Aggressivity = 0;
	d_Manner = 0;
	lPWeapon = 0;
	*szPWeapon = 0;

	// stuff for replaying movement data
	pWPAMPlay = 0;
	fStartADVM = 0;
	bReplay = 0;
	
	f_move_speed = 0;
	moved_distance = 0;
	
	i1C = 0;
	v_diff_angles = Vector(0,0,0);
	f_LastFight = 0;
	f_IgnoreFall = 0;
	i_FOV = 0;
	
	fRightDist=fMiddleDist=fLeftDist = 0;
	
	l_cState = 0;
	
	bRSInit = 0;
	
	i_CurrWP = -1;
	f_D2C1 = 0;
	f_D2C2 = 0;
	i_LastWP = -1;
	
	memset(&m_rgAmmo,0,sizeof(int)*MAX_AMMO_SLOTS);
}

CBotBase :: ~CBotBase(){
	Chat.UnLoadChat(this);
}

void CBotBase :: KilledBy(edict_t *pEdictKiller,long lWeaponKiller){
	if(lWeaponKiller != -1){
		Personality.iWeaponPriority[lWeaponKiller]++;
	}
	if(HasPrimary()){
		Personality.iWeaponPriority[HasPrimary()]--;
	}

	killer_edict = pEdictKiller;

	// store information with which weapon the bot has been killed
	FLKW[iCLKW] = lWeaponKiller;
	iCLKW ++;
	if(iCLKW >= CBotBase::_MAXLKW){
		iCLKW = 0;
	}
	
	// update LTM of bot
	if(pEdictKiller){
		int killer_team = UTIL_GetTeam(pEdictKiller);
		if(killer_team != bot_teamnm){		// no tk
			LTMem.Add(LTM_KILLED,pEdictKiller,pEdictKiller->v.origin,pEdict->v.origin);
		}
		else{
			if(RANDOM_LONG(0,100) < 50){	
				Vector VTemp = Vector(0,0,0);
				LTMem.Add(LTM_TKILL,pEdictKiller,pEdictKiller->v.origin,VTemp);
			}
		}
	}
}

void CBotBase :: KilledSO(edict_t *pEdictKilled,long lWeaponKiller){
	if(lWeaponKiller != -1){
		Personality.iWeaponPriority[lWeaponKiller]++;
	}
	
	pEKilled = pEdictKilled;

	// update LTM of bot
	if(pEdictKilled){
		int victim_team = UTIL_GetTeam(pEdictKilled);
		if(victim_team != bot_teamnm){		// no tk
			LTMem.Add(LTM_KILLED,pEdictKilled,pEdictKilled->v.origin,pEdict->v.origin);
		}
		else{
			if(RANDOM_LONG(0,100) < 50){	
				Vector VTemp = Vector(0,0,0);
				LTMem.Add(LTM_TKILL,pEdictKilled,pEdictKilled->v.origin,VTemp);
			}
		}
	}
}

bool CBotBase :: Jump(void){

	if(f_NJumpTill < gpGlobals->time &&
		g_fFreezeTimeEnd < gpGlobals->time)	// don't jump during freeze time
	{
		lButton |= IN_JUMP;
		
		f_NJumpTill = gpGlobals->time + f_TimeBJumps;
		
		return true;
	}

	return false;
}

bool CBotBase :: Wander(void){
	// the next stuff just test if the bot runs against a wall
	Vector v_diff_norm = v_diff.Normalize();		// normalized vector of the bot's last move
	v_diff_norm.z=0;								// don't look at pitch - just make it 2D
	Vector v_view_norm;
	
	double dAngle = pEdict->v.v_angle.y;
	
	v_view_norm.y = dAngle;
	v_view_norm.x = v_view_norm.z = 0;
	UTIL_MakeVectors(v_view_norm);
	v_view_norm = gpGlobals->v_forward;
	
	if(v_diff.Length() > 2){
		VRunningTo = pEdict->v.origin - v_diff_norm * 1000;
		DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,VRunningTo,2,2,200,200,200,200,0);
		HeadToward(VRunningTo);
	}
	
	if((moved_distance < 1.0)
		&& (prev_speed > 1.5)// stopped, cause the speed was high last time
		&& (f_dont_check_stuck < gpGlobals->time)
		&& f_Pause < gpGlobals->time-.5){// running against wall etc.
		//f_old_direction = (pEdict->v.angles.y - v_diff_angles.y - 8);
		//DEBUG_CLIENTCOMMAND(pEdict,"say muh die kuh");
		if(l_cState==0){		// crashed into wall for the first time ...
			l_cState ++;
			
			if(RANDOM_LONG(0,100) < 80){		// .. so jump
				//DEBUG_CLIENTCOMMAND(pEdict,"say no waypoint found and jumping cause of wall");
				Jump();
				f_ducktill = gpGlobals->time + .4f;
			}
		}
		else if(l_cState == 1){			// ... or turn later
			/*DEBUG_CLIENTCOMMAND(pEdict,"say collision (%f)",moved_distance);*/
			v_diff_angles = UTIL_VecToAngles(v_diff);
			pEdict->v.ideal_yaw = v_diff_angles.y + RANDOM_LONG(-20,20);
			
			l_cState ++;
		}
		else{
			l_cState = 0;
		}
		FixIdealYaw();
	}
	else
		l_cState =0;
	return false;
}

long CBotBase :: RoamteamFindWP(edict_t *pEdictTF){
	int i, min_index = -1,iNearWPTF;
	float fMinFN = 0;
	edict_t *pEdictNearest = GetNearestPlayer(pEdict,-1,fMinFN,false,false,10000);
	float distance,sdistance;

	Vector2D vec2LOS,vec2M;
	Vector VFOrigin;
	float    flDot;

	float fValue,
		fLD = 400,fMin;

	if (num_waypoints < 1)
		return -1;

	if(pEdictNearest != 0 && pEdictNearest != pEdictTF)
		fLD *= 2.f;

	VFOrigin = pEdictTF->v.origin;
	
	// find the nearest waypoint...
	
	min_index = -1;
	fMin = -10000000;

	vec2M = pEdictTF->v.velocity.Make2D();
	if(vec2M.Length() < 10.f){
		UTIL_MakeVectors ( pEdictTF->v.angles );
		vec2M = gpGlobals->v_forward.Make2D();
	}
	vec2M = vec2M.Normalize();

	iNearWPTF = WaypointFindNearest(pEdictTF,100000,bot_teamnm,1,false,false,false);
	
	for (i=0; i < num_waypoints; i++)
	{
		if ((waypoints[i].flags & W_FL_DELETED) == W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		if (waypoints[i].flags & W_FL_AIMING)
			continue;  // skip any aiming waypoints

		// skip this waypoint if it's team specific and teams don't match...
		if ((waypoints[i].flags & W_FL_TEAM_SPECIFIC) &&
			((waypoints[i].flags & W_FL_TEAM) != bot_teamnm))
			continue;
		
		if(!WPStat.GetVisible(i,iNearWPTF))		// skip if not visible ...
				continue;

		distance = WaypointDistanceFromTo(iNearWPTF,i,bot_teamnm);

		if(distance > fLD*2)
			continue;

		sdistance = WaypointDistanceFromTo(i,iNearWPTF,bot_teamnm);

		if(sdistance > distance + 100.0f)
			continue;
		
		UTIL_MakeVectors ( pEdictTF->v.angles );
		
		vec2LOS = ( waypoints[i].origin - VFOrigin ).Make2D();
		vec2LOS = vec2LOS.Normalize();
		
		flDot = DotProduct (vec2LOS , vec2M );

		fValue = (1.f/(fabs(flDot-.85)+.1))*(100.f/(fabs(distance-fLD)+.1));
		
		if (fMin < fValue){
			fMin = fValue;
			min_index = i;
		}
	}
	return min_index;
}

void CBotBase :: PreprocessTasks(void){
	if(Task.current){
		if(Task.current->lType & BT_GOTO)
			return;

		int iWantedDiv=-1,
			iFarGoal=-1,
			iGoal=-1;

		if(Task.current->lType & BT_HUNT){
			//DEBUG_CLIENTCOMMAND(pEdict,"say hunt");
			if(FNullEnt(Task.current->p)){
				Task.NextTask();
				//return false;
			}
			else if(!IsAlive(Task.current->p)){
				Task.NextTask();
				//return false;
			}
			else{
				ResetWPlanning();
				
				iFarGoal = WaypointFindNearest(Task.current->p,10000,bot_teamnm);
				iWantedDiv = 1;
				
				if(iFarGoal != -1){
					Task.AddTask(BT_GOTO|BT_TMP,gpGlobals->time + 30.f,iFarGoal,(void *) iWantedDiv,0);
				}
			}
		}
		else if(Task.current->lType & BT_FLEE){
			ResetWPlanning();
			
			iFarGoal = Task.current -> lAdd;
			iWantedDiv = (int)(Task.current->p);

			if(iFarGoal != -1){
				Task.AddTask(BT_GOTO|BT_TMP,-1,iFarGoal,(void *) iWantedDiv,0);
			}
		}
		else if(Task.current->lType & BT_COVER){
			//DEBUG_CLIENTCOMMAND(pEdict,"say cover");
			if(FNullEnt(Task.current->p)){
				Task.NextTask();
			}
			else if(!IsAlive(Task.current->p)){
				Task.NextTask();
			}
			else{
				ResetWPlanning();

				int iIndices[101];
				long lNum = 0;
				float fDistance,fMDistance;
				int index;
				
				for (index=0; index < num_waypoints; index++)
				{
					if ((waypoints[index].flags & W_FL_DELETED) == W_FL_DELETED)
						continue;  // skip any deleted waypoints
					
					if (waypoints[index].flags & W_FL_AIMING)
						continue;  // skip any aiming waypoints
					
					if(!FVisible(waypoints[index].origin,(edict_t *)Task.current->p))
						continue;
					
					if(FInViewCone(&waypoints[index].origin,(edict_t *)Task.current->p)){
						fMDistance = 500.0f;
					}
					else{
						fMDistance = 150.0f;
					}
					
					fDistance = (waypoints[index].origin - ((edict_t *)Task.current->p)->v.origin).Length();
					if(fDistance < fMDistance
						&& lNum < 100){
						iIndices[lNum] = index;
						lNum ++;
					}
				}
				//FindWayWP(pEdict,iIndices[RANDOM_LONG(0,lNum-1)],FW_Cur,WayDecideShortest);
				if(lNum > 1)
					iFarGoal = iIndices[RANDOM_LONG(0,lNum-1)];
				if(iFarGoal == WaypointFindNearest(pEdict,400,bot_teamnm,0,false,false,false)
					|| RANDOM_LONG(0,100) < 10){
					f_Pause = gpGlobals->time + 1.f;
					f_noWP = gpGlobals->time + .9f;
				}
				
				if(iFarGoal != -1){
					Task.AddTask(BT_GOTO|BT_TMP,Task.current->fLive2,iFarGoal,(void *) 0/*iWantedDiv*/,0);
				}
			}
		}
		else if(Task.current->lType & BT_FOLLOW){
			if(FNullEnt(Task.current->p)){
				Task.NextTask();
			}
			else if((pEdict->v.origin-((edict_t *)Task.current->p)->v.origin).Length() > 300){
				//FindWayVector(pEdict,Order.pREdict->v.origin,FW_Cur,WayDecideShortest);
				iFarGoal = WaypointFindNearestGoal(pEdict->v.origin,pEdict,500,bot_teamnm,0);
				
				if(iFarGoal != -1){
					Task.AddTask(BT_GOTO|BT_TMP,-1,iFarGoal,(void *) /*iWantedDiv*/0,0);
				}
			}
		}
		else if(Task.current->lType & BT_HELP){
			if(FNullEnt(Task.current->p)){
				Task.NextTask();
			}
			else if(!IsAlive((edict_t *)Task.current->p)){
				Task.NextTask();
			}
			else if( ( ((edict_t *)Task.current->p)->v.origin - pEdict->v.origin).Length() < 300.f
				&& FVisible(((edict_t *)Task.current->p)->v.origin,pEdict)){				// i.e. near enough and visible ( is imp for maps like labyrints )
				Task.NextTask();
				Task.AddTask(BT_CAMP, gpGlobals->time + 6.0f,0,0,0);
				//Task.RemoveT(BT_HELP);
				ResetWPlanning();
			}
			else{
				ResetWPlanning();
				iFarGoal = WaypointFindNearestGoal(((edict_t *)Task.current->p)->v.origin,pEdict,1000,bot_teamnm,0);
				
				if(iFarGoal != -1){
					Task.AddTask(BT_GOTO|BT_TMP,Task.current->fLive2,iFarGoal,(void *) 0/*iWantedDiv*/,0);
				}
			}
			//return true;
		}
		else if(Task.current->lType & BT_ROAMTEAM){
			//DEBUG_CLIENTCOMMAND(pEdict,"say roamteam");
			
			if(g_fRoundTime < 7)
				Task.AddTask(BT_CAMP,gpGlobals->time + 7.25 - g_fRoundTime,0,0,0);
			else{
				
				if(FNullEnt(Task.current->p)){		// no edict ?
					// just get a new teammember
					Task.current->p = GetRandomPlayerWO(pEdict,bot_teamnm,1,BT_COVER|BT_ROAMTEAM,pEdict);
				}
				if(FNullEnt(Task.current->p)){
					Task.NextTask();
					return;
				}
				else if(!IsAlive(Task.current->p)){
					Task.NextTask();
					return;
				}
				else{
					int iWP=-1/*,iWPN,ischl*/;
					ResetWPlanning();
					CBotBase *pB = UTIL_GetBotPointer((edict_t*)Task.current->p);
					if(pB){
						//if(((CBotCS*)pB)->Task.SearchT( BT_ROAMTEAM|BT_COVER) != -1){	// just to make it easy ... although i don't like this
						if(pB->Task.SearchP( pEdict ) !=1){	// just to make it easy ... although i don't like this
							//Task.NextTask();
							//cout << "--------------------------------------------------------------------testing this " << endl;
							//return false;
							Task.current->lAdd ++;
							Task.current->p = GetRandomPlayerWO(pEdict,bot_teamnm,1,BT_COVER|BT_ROAMTEAM,pEdict);
							if(Task.current->lAdd > 2){
								Task.current->p = 0;
								Task.NextTask();
								return;
								//cout << "fuckin roamteam" << endl;
								//DEBUG_CLIENTCOMMAND(pEdict,"say fuckin roamteam");
							}
						}
						else
							Task.current->lAdd = 0;
					}
					iWP = -1;
					if(Task.current){
						if(FNullEnt(Task.current->p)){		// found no edict
							Task.NextTask();
							return;
						}
						else{
							iWP = RoamteamFindWP(Task.current->p);		// has not to be visib, just in view cone, to get sth which is is rather in front to get a straight movement
							
							if(iWP == iNearWP){
								f_Pause = gpGlobals->time + 1.f;
								f_noWP = gpGlobals->time + .9f;
							}
						}
					}
					
					if(iWP != -1){
						DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,waypoints[iWP].origin,10,0,0,250,0,200,10);
						DEBUG_DRAWBEAM(listenserver_edict,Task.current->p->v.origin,waypoints[iWP].origin,10,0,0,250,0,200,10);
						iGoal = iFarGoal = iWP;
						Task.AddTask(BT_GOTO|BT_TMP,gpGlobals->time + 20.f,iWP,(void *) 0/*iWantedDiv*/,0);
					}
					else{
						if(num_waypoints){
							Task.AddTask(BT_CAMP|BT_TMP,gpGlobals->time + 2.0,0,0,0);
						}
						else{
							Task.NextTask();
						}
					}
				}
			}
			// true;
		}
		else if(Task.current->lType & BT_GUARD){
			// guard sth whose origin is stored or an edict .. so just test ...
			Vector VToGuard;
			if(Task.current->p){		// guard edict
				VToGuard = ((edict_t *)Task.current->p)->v.origin;
			}
			else{							// guard static location
				VToGuard = Task.current->VT;
			}
			int iIndices[101];
			long lNum = 0;
			float fDistance,fMDistance;
			int index;
			TraceResult tr;
			
			for (index=0; index < num_waypoints; index++)
			{
				if ((waypoints[index].flags & W_FL_DELETED) == W_FL_DELETED)
					continue;  // skip any deleted waypoints
				
				if (waypoints[index].flags & (W_FL_AIMING|W_FL_JUMP))
					continue;  // skip any aiming waypoints
				
				if(WPStat[index].lKill > WPStat.lKillMax/4)
					continue;		// skip dangerous places
				
				UTIL_TraceLine(VToGuard,waypoints[index].origin,ignore_monsters,pEdict,&tr);
				
				if(tr.flFraction != 1.0)
					continue;
				
				fMDistance = 600.0f;
				
				fDistance = (waypoints[index].origin - VToGuard).Length();
				
				if(fDistance < fMDistance
					&& lNum < 100){
					iIndices[lNum] = index;
					lNum ++;
				}
			}
			//FindWayWP(pEdict,iIndices[RANDOM_LONG(0,lNum-1)],FW_Cur,WayDecideShortest);
			if(lNum > 1)
				iFarGoal = iIndices[RANDOM_LONG(0,lNum-1)];
			
			/*if(FVisible(VToGuard,pEdict)
				&&f_noCamp < gpGlobals->time +5){
				float till = gpGlobals->time + RANDOM_FLOAT(3,6);
				Task.AddTask(BT_CAMP|BT_TMP|BT_CROUCH, till,0,0,0);
				InitCamp();
			}
			else */if(iFarGoal != -1){
				if(iFarGoal == iNearWP){
					Task.AddTask(BT_CAMP,gpGlobals->time + 5,0,0,0);
				}
				else
					Task.AddTask(BT_CAMPATGOAL|BT_GOTO,Task.current->fLive2,iFarGoal,(void *) iWantedDiv,RANDOM_LONG(3,6));
			}
		}
	}
}

void CBotBase :: Think(void){
}

void CBotBase :: Think5th(void){
}

void CBotBase :: Think1(void){
}

bool CBotBase :: AvoidStuck(void){
	edict_t *pEnt;
	char szDoor[] = "func_door";
	int iLenSD = strlen(szDoor)*sizeof(char);
	bool bDoor;
	
	if (!bReplay
		&&((moved_distance < 1.0 && !(pEdict->v.button&IN_DUCK))
			||(moved_distance < .1 && (pEdict->v.button&IN_DUCK) )  )
		&& (prev_speed > 1.5)// stopped, cause the speed was high last time
		&& (f_dont_check_stuck < gpGlobals->time)
		&& f_Pause < gpGlobals->time-.5
		&& fViewSpeedYaw < 3)
	{
		if(CanJumpUp()){
			Jump();
		}
		else{
			if(f_bot_stuck < gpGlobals->time - .5){
				lStuckC ++ ;		// count up the stuck times
			}
			else{
				lStuckC = 0;		// reset it if there isn't stuck
			}
			f_bot_stuck = gpGlobals->time;
			// the bot must be stuck!
			// sometimes jump
			if(RANDOM_LONG(0,100) > 50){
				Jump();
				//DEBUG_CLIENTCOMMAND(pEdict,"say wpstuckducktill");
				f_ducktill = gpGlobals->time +.4f;
			}
			else{		// why ?
				if (RANDOM_LONG(1, 100) <= 50)
					pEdict->v.ideal_yaw += RANDOM_LONG(10, 20);
				else
					pEdict->v.ideal_yaw -= RANDOM_LONG(10, 20);
				FixIdealYaw();
			}
			if(moved_distance1 < 40 && prev_speed1 > 50 || lStuckC > 3){			// not the first time the bot is stuck
				// check if the door can be oped with a button
				TestOnButtonWay(&pEnt);
				if(pEnt){		// there is an entity which could block this way ....
					iFarGoal = iGoal;
					Vector VEntityOrigin = VecBModelOrigin(VARS(pEnt));
					int iDest = WaypointFindNearest(VEntityOrigin,pEdict,400,bot_teamnm);
					if(iDest != -1){
						Task.AddTask(BT_GOTO|BT_GOBUTTON,-1,iDest,pEnt,0);
					}
					//DEBUG_CLIENTCOMMAND(pEdict,"say asd");
					return false;
				}
				
				// check if a door is nearby
				if(b_neardoor){
					pEnt = 0;
					bDoor = false;
					while(pEnt = UTIL_FindEntityInSphere(pEnt,pEdict->v.origin,50.0f)){ //todo
						if(!strncmp(STRING(pEnt->v.classname),szDoor,iLenSD)){
							bDoor = true;
							break;
						}
					}
					if(bDoor){
						edict_t *pNearestPT;
						float fMin;	// distance to next player of own team
						Vector VDoor = VecBModelOrigin(VARS(pEnt));
						
						pNearestPT = GetNearestPlayer(pEdict,bot_teamnm,fMin);
						
						if(fMin > 150){		// i.e. there is no near player ... door is probably blocked by an enemy -> shoot there
							if(IsCWeaponRifle()
								|| IsCWeaponSniper()){	// shoot there if weapon is ok
								if(f_shootbox < gpGlobals->time - 1.0){
									f_shootbox = gpGlobals->time + 3.0;		// just to shoot not too often
									UTIL_MakeVectors(pEdict->v.angles);
									
									Vector VFireAt = pEdict -> v.origin + gpGlobals -> v_forward;
									
									FireWeapon(VFireAt);
								}
							}
						}
						else{	// same team player ... just find something to decide who has to go away
							if(ENTINDEX(pEdict)<ENTINDEX(pNearestPT)){	// this bot has to go
								f_LookTo = gpGlobals->time + 2.0;
//								f_GoBack = gpGlobals->time + 2.0;
								VLookTo = VDoor;
								// find a wp which is visible and a bit away frm the door
								int min_index;
								
								min_index = WaypointFindNearest(pEdict,300,bot_teamnm,50,true,false);
								if(min_index != -1){
									i_CurrWP = min_index;		// copy near wp to current wp
									VRunningTo = waypoints[i_CurrWP].origin;
								}
							}
							else{	// the other not
							}
						}
					}
				}
				else{
					// check if there is a another bot near to you
					edict_t *pNearestPT;
					float fMin;	// distance to next player of own team
					int iWPB = -1;			// waypoint to go back to
					
					pNearestPT = GetNearestPlayer(pEdict,bot_teamnm,fMin,false,false,100);
					if(pNearestPT){
						Vector VDiffBoth = pNearestPT -> v.origin - pEdict->v.origin;
						Vector VDiffBothN = VDiffBoth.Normalize();
						// is it not a bot ? then go back
						// is the number of the other bot higher than mine ? then go back
						if(!(pNearestPT->v.flags & FL_FAKECLIENT)
							|| ENTINDEX(pEdict)<ENTINDEX(pNearestPT)){
							if(iNearWP == -1){
								iNearWP = WaypointFindNearest(pEdict,1000,bot_teamnm,0,false,false,false);
							}
							if(iNearWP != -1){
								int	pi,
									iWP;
								float fDistanceMax = 0,fDistance;
								PATH *p;
								Vector VDiffWP;

								p = paths[iNearWP];
								
								while(p){
									pi=0;
									while(pi<MAX_PATH_INDEX){
										iWP = p->index[pi];
										if(iWP != -1){
											VDiffWP = waypoints[iWP].origin - pEdict->v.origin;
											if(DotProduct(VDiffWP.Normalize(),VDiffBothN) < 0){
												fDistance = VDiffWP.Length();
												if(fDistance > fDistanceMax){
													fDistanceMax = fDistance;
													iWPB = iWP;
													/*p = 0;
													break;*/
												}
											}
										}
										pi ++;
									}
									if(p)
										p = p->next;
								}
								if(iWPB != -1){
									p = paths[iWPB];
									
									while(p){
										pi=0;
										while(pi<MAX_PATH_INDEX){
											iWP = p->index[pi];
											if(iWP != -1){
												VDiffWP = waypoints[iWP].origin - pEdict->v.origin;
												if(DotProduct(VDiffWP.Normalize(),VDiffBothN) < 0){
													fDistance = VDiffWP.Length();
													if(fDistance > fDistanceMax){
														fDistanceMax = fDistance;
														iWPB = iWP;
														/*p = 0;
														break;*/
													}
												}
											}
											pi ++;
										}
										if(p)
											p = p->next;
									}
								}
							}
							if(iWPB != -1){
								i_CurrWP = iWPB;
								DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,waypoints[iWPB].origin,30,0,250,0,0,250,0);
								f_D2C1 = f_D2C2 = 10000.0;
							}
						}
					}
				}
			}
		}
		return true;
	}
	return false;
}

bool CBotBase :: AvoidCollision(void){
	if(iOnLadder != 0				// don't avoid collisions on ladders ;-)
		|| bReplay)				// no collsion avoidance when replaying
		return false;
	
	Vector	VStart,					// start for all traces
		V_VelAngles,			// converted velocity to angles
		VEndLeft,				// left aim
		VEndMiddle,				// straight ahead
		VEndRight;				// right aim

	//char buffer[100];
	//float fleft,fmiddle,fright;		// length of traces without fraction
	TraceResult tr;					// temp storage of results
	
	if(g_b5th){
		if(pEdict->v.velocity.Length2D() < 30.f){
			//return false;
			V_VelAngles = pEdict->v.angles;
		}
		else{
			V_VelAngles = UTIL_VecToAngles(pEdict->v.velocity);
		}
		V_VelAngles.x = V_VelAngles.z = 0;
		double dOAngle = V_VelAngles.y * M_PI / 180.0,
			dAngle;

		float fLength = _CAL;

		if(f_dont_avoid > gpGlobals->time || b_neardoor)
			fLength = _CAL / 4.f;

		nVec nVCollIn(3);
		
		nVCollIn.SetMemVec(dCollNNIn);		// set mem vec to nninput
		
		VStart = pEdict->v.origin + pEdict->v.view_ofs;	// position of bot's eyes
		
		// make every vector to the same height  - calc the ends of the bot's 'feelers'
		VEndRight.z = VEndMiddle.z = VEndLeft.z = VStart.z;
		
		VEndMiddle.x = fLength * cos(dOAngle) + VStart.x;
		VEndMiddle.y = fLength * sin(dOAngle) + VStart.y;
		
		dAngle = dOAngle + _CAA * M_PI / 180.0;
		
		VEndLeft.x = fLength * cos(dAngle) + VStart.x;
		VEndLeft.y = fLength * sin(dAngle) + VStart.y;
		
		dAngle = dOAngle - _CAA * M_PI / 180.0;
		
		VEndRight.x = fLength * cos(dAngle) + VStart.x;
		VEndRight.y = fLength * sin(dAngle) + VStart.y;
		
		// trace lines
		UTIL_TraceLine(VStart, VEndLeft, dont_ignore_monsters,pEdict->v.pContainingEntity, &tr);
		fLeftDist = tr.flFraction;
#ifdef __COLLLINE
		if(fLeftDist != 1||1){
			WaypointDrawBeam(listenserver_edict, VStart, VEndLeft, 30, 0, 0, 0, 255, 250, 5);
		}
#endif
		
		UTIL_TraceLine(VStart, VEndMiddle, dont_ignore_monsters,pEdict->v.pContainingEntity, &tr);
		fMiddleDist = tr.flFraction;
		
		// is the middle trace blocked by a breakable ent ???
		if(fMiddleDist!=1.0){
			if(f_LastFight+1.0f < gpGlobals->time){
				if(FStrEq(STRING(tr.pHit->v.classname),"func_breakable")){
					if(tr.pHit->v.takedamage == DAMAGE_YES){		// boxes in dust ?
						if(IsCWeaponGrenade()){	//dont try to destroy func_breakable with grenades - can be unhealthy ( wooden boxes )
							if(HasSecondary()		// try first with secondary
								&&m_rgAmmo[weapon_defs[HasSecondary()].iAmmo1]>0){
								Change2Weapon(HasSecondary());
							}
							else if(HasPrimary()	// if no sec or no sec ammo, try primary
								&&m_rgAmmo[weapon_defs[HasPrimary()].iAmmo1]>0){
								Change2Weapon(HasPrimary());
							}
							else{						// try to destroy it with a knife
								Change2Weapon(HasKnife());
							}
						}
						else{
							if(tr.flFraction*_CAL < 20.0){
								//f_move_speed = -pEdict->v.maxspeed;
							}
							//pEdict->v.angles.y = pEdict->v.v_angle.y;
							if(!PauseShoot()){ //BotFireWeapon(tr.pHit->v.origin,pBot);
								Vector v_destr = VEndMiddle - GetGunPosition(pEdict);
								
								VATurnTo(v_destr);
								
								// select the best weapon to use at this distance and fire...
								//if(f_Delay < gpGlobals->time)
								FireWeapon( v_destr );		// dod is also for cs sufficient
								//lButton |= IN_ATTACK;
							}
						}
					}
				}
				//DEBUG_CLIENTCOMMAND(pEdict,"say func_breakable");
			}
		}
		
#ifdef __COLLLINE
		if(fMiddleDist != 1||1){
				WaypointDrawBeam(listenserver_edict, VStart, VEndMiddle, 30, 0, 0, 127, 255, 250, 5);
		}
#endif
		
		UTIL_TraceLine(VStart, VEndRight, dont_ignore_monsters,pEdict->v.pContainingEntity, &tr);
		fRightDist = tr.flFraction;
#ifdef __COLLLINE
		if(fRightDist != 1||1){
			WaypointDrawBeam(listenserver_edict, VStart, VEndRight, 30, 0, 0, 255, 255, 250, 5);
		}
#endif
		
		if(fLeftDist == 1				// if there's no obstacle
			&& fMiddleDist == 1
			&& fRightDist == 1){
			//f_avoid_strafe = 0;
			v_avoid = g_vecZero;
			return true;
		}
		else{
			// convert [-1;1]
			dCollNNIn[ICI_Left]		= fLeftDist * 2.0 - 1.0;
			dCollNNIn[ICI_Middle]	= fMiddleDist * 2.0 - 1.0;
			dCollNNIn[ICI_Right]	= fRightDist * 2.0 - 1.0;
			
			nVCollIn.Normalize();		// normalize it;
			
			NNColl->SetInput(dCollNNIn);
			NNColl->Propagate();
			NNColl->GetOutput(dCollNNOut);
			
			if(dCollNNOut[0] > 0.5){
				//f_avoid_strafe = _MAXSTRAFE * 1.5f;
				
				//f_move_speed *= ( 2.0f / 3.0f );		// just slow down a little bit

				UTIL_MakeVectors(V_VelAngles);

				v_avoid = gpGlobals->v_right;
			}
			else if(dCollNNOut[0] < -0.5){
				//f_avoid_strafe = - _MAXSTRAFE * 1.5f;
				
				//f_move_speed *= ( 2.0f / 3.0f );		// just slow down a little bit
				UTIL_MakeVectors(V_VelAngles);

				v_avoid = -gpGlobals->v_right;
			}
			else
				v_avoid = g_vecZero;
		}
	}
	//DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin+Vector(0,0,30),pEdict->v.origin+Vector(0,0,30)+v_avoid * 100,3,0,0,0,255,255,0);
	// check if the way to the next wp is blocked by a breakable ent ...
	if(i_CurrWP != -1){
		Vector VEndWP = waypoints[i_CurrWP].origin;
		VStart = pEdict->v.origin + pEdict->v.view_ofs;	// position of bot's eyes
		UTIL_TraceLine(VStart, VEndWP, ignore_monsters,pEdict->v.pContainingEntity, &tr);
		
		if(f_LastFight+1.0f < gpGlobals->time){
			if(FStrEq(STRING(tr.pHit->v.classname),"func_breakable")){
				if(tr.pHit->v.takedamage == DAMAGE_YES){		// boxes in dust ?
					if(IsCWeaponGrenade()){	//dont try to destroy func_breakable with grenades - can be unhealthy ( wooden boxes )
						if(HasSecondary()		// try first with secondary
							&&m_rgAmmo[weapon_defs[HasSecondary()].iAmmo1]>0){
							Change2Weapon(HasSecondary());
						}
						else if(HasPrimary()	// if no sec or no sec ammo, try primary
							&&m_rgAmmo[weapon_defs[HasPrimary()].iAmmo1]>0){
							Change2Weapon(HasPrimary());
						}
						else{						// try to destroy it with a knife
							Change2Weapon(HasKnife());
						}
					}
					else{
						if(tr.flFraction*_CAL < 20.0){
							f_move_speed = -pEdict->v.maxspeed;
						}
						if(current_weapon.iAmmo1 == 0){
							Change2Weapon(HasKnife());
						}
						//pEdict->v.angles.y = pEdict->v.v_angle.y;
						if(!PauseShoot()){ //BotFireWeapon(tr.pHit->v.origin,pBot);
							Vector v_destr = VEndWP - GetGunPosition(pEdict);
							
							VATurnTo(v_destr);
							
							// select the best weapon to use at this distance and fire...
							//if(f_Delay < gpGlobals->time)
							FireWeapon( v_destr );
							//lButton|=IN_ATTACK;
						}
					}
				}
			}
		}
	}

	return false;
}

bool CBotBase :: CanJumpUp(void){
	// What I do here is trace 3 lines straight out, one unit higher than
	// the highest normal jumping distance.  I trace once at the center of
	// the body, once at the right side, and once at the left side.  If all
	// three of these TraceLines don't hit an obstruction then I know the
	// area to jump to is clear.  I then need to trace from head level,
	// above where the bot will jump to, downward to see if there is anything
	// blocking the jump.  There could be a narrow opening that the body
	// will not fit into.  These horizontal and vertical TraceLines seem
	// to catch most of the problems with falsely trying to jump on something
	// that the bot can not get onto.
	
	TraceResult tr;
	Vector v_jump, v_source, v_dest;
	
	// convert current view angle to vectors for TraceLine math...
	
	v_jump = pEdict->v.v_angle;
	v_jump.x = 0;  // reset pitch to 0 (level horizontally)
	v_jump.z = 0;  // reset roll to 0 (straight up and down)
	
	UTIL_MakeVectors( v_jump );
	
	// use center of the body first...
	
	// maximum jump height is 45, so check one unit above that (46)
	v_source = pEdict->v.origin + Vector(0, 0, -36 + 46);
	v_dest = v_source + gpGlobals->v_forward * 24;
	
	// trace a line forward at maximum jump height...
	UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
		pEdict->v.pContainingEntity, &tr);
	
	// if trace hit something, return FALSE
	if (tr.flFraction < 1.0)
		return FALSE;
	
	// now check same height to one side of the bot...
	v_source = pEdict->v.origin + gpGlobals->v_right * 16 + Vector(0, 0, -36 + 46);
	v_dest = v_source + gpGlobals->v_forward * 24;
	
	// trace a line forward at maximum jump height...
	UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
		pEdict->v.pContainingEntity, &tr);
	
	// if trace hit something, return FALSE
	if (tr.flFraction < 1.0)
		return FALSE;
	
	// now check same height on the other side of the bot...
	v_source = pEdict->v.origin + gpGlobals->v_right * -16 + Vector(0, 0, -36 + 46);
	v_dest = v_source + gpGlobals->v_forward * 24;
	
	// trace a line forward at maximum jump height...
	UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
		pEdict->v.pContainingEntity, &tr);
	
	// if trace hit something, return FALSE
	if (tr.flFraction < 1.0)
		return FALSE;
	
	// now trace from head level downward to check for obstructions...
	
	// start of trace is 24 units in front of bot, 72 units above head...
	v_source = pEdict->v.origin + gpGlobals->v_forward * 24;
	
	// offset 72 units from top of head (72 + 36)
	v_source.z = v_source.z + 108;
	
	// end point of trace is 99 units straight down from start...
	// (99 is 108 minus the jump limit height which is 45 - 36 = 9)
	v_dest = v_source + Vector(0, 0, -99);
	
	// trace a line straight down toward the ground...
	UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
		pEdict->v.pContainingEntity, &tr);
	
	// if trace hit something, return FALSE
	if (tr.flFraction < 1.0)
		return FALSE;
	
	// now check same height to one side of the bot...
	v_source = pEdict->v.origin + gpGlobals->v_right * 16 + gpGlobals->v_forward * 24;
	v_source.z = v_source.z + 108;
	v_dest = v_source + Vector(0, 0, -99);
	
	// trace a line straight down toward the ground...
	UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
		pEdict->v.pContainingEntity, &tr);
	
	// if trace hit something, return FALSE
	if (tr.flFraction < 1.0)
		return FALSE;
	
	// now check same height on the other side of the bot...
	v_source = pEdict->v.origin + gpGlobals->v_right * -16 + gpGlobals->v_forward * 24;
	v_source.z = v_source.z + 108;
	v_dest = v_source + Vector(0, 0, -99);
	
	// trace a line straight down toward the ground...
	UTIL_TraceLine( v_source, v_dest, dont_ignore_monsters,
		pEdict->v.pContainingEntity, &tr);
	
	// if trace hit something, return FALSE
	if (tr.flFraction < 1.0)
		return FALSE;
	
	return TRUE;
}

void CBotBase :: FixIdealYaw(void){
	// check for wrap around of angle...
	if (pEdict->v.ideal_yaw > 180)
		pEdict->v.ideal_yaw -= 360;
	
	if (pEdict->v.ideal_yaw < -180)
		pEdict->v.ideal_yaw += 360;
}

void CBotBase :: TestOnButtonWay(edict_t **pToUse){
	int iNearWP = WaypointFindNearest(pEdict,1000,bot_teamnm);
	CWay pWay;

	ConvertFloyd2Way(bot_teamnm,iNearWP,iGoal,&pWay);		// convert to way

	TestOnButtonWay(pWay,pToUse);
}

void CBotBase :: TestOnButtonWay(CWay &pWay,edict_t **pToUse){
	// look if there is something (door lift etc) in between which has to be 
	// opened by a button etc
	int i,is;
	//WAY *pWay = new WAY;
	edict_t *pEnt;
	TraceResult tr;
	edict_t *pButton[10];
	int iCount,iOCount;
	int distance[10];
	int iNWP;
	//char szClassname[100];
	
	*pToUse = 0;
	
	for(i=0,is=1;is < pWay.iNum;i++,is++){
		/*if(WPStat.GetVisible(pWay->iIndices[i],pWay->iIndices[is]))
			continue;*/

		UTIL_TraceLine(waypoints[pWay.iIndices[i]].origin,waypoints[pWay.iIndices[is]].origin,ignore_monsters,0,&tr);
		
		if(tr.pHit != 0){		// hit an ent
			//FILE *fhdf;fhdf=fopen("toopen.txt","a");fprintf(fhdf,"%s\t%s\t%s\n",STRING(tr.pHit->v.classname),STRING(tr.pHit->v.target),STRING(tr.pHit->v.targetname));fclose(fhdf);
			pButton[0] = 0;
			iCount = 0;
			pEnt = 0;
			while(pEnt = UTIL_FindEntityByString( pEnt, "target", STRING( tr.pHit->v.targetname ) )){	// look for sth which has this as target
				//if(FStrEq(STRING(pEnt<->v.classname),"func_button")){		// only buttons
				iNWP = WaypointFindNearest(VecBModelOrigin(VARS(pEnt)),pEdict,300,bot_teamnm);

				distance[iCount] = WaypointDistanceFromTo(iNearWP,iNWP,bot_teamnm);
				pButton[iCount] = pEnt;
				//*pToUse = pEnt;
				if(iCount < 10-1)
					iCount++;
			}		// todo
			iOCount = iCount;
			int ischl;
			for(ischl=0;ischl < iCount;ischl++){
				iNWP = WaypointFindNearest(VecBModelOrigin(VARS(pButton[ischl])),pEdict,300,bot_teamnm);
				if(BoolTestOnButtonWay(iNearWP,iNWP)){
					*pToUse = pButton[ischl];
					break;
				}
			}

			if(*pToUse){
				//if(pEdictPlayer)
				//WaypointDrawBeam(pEdictPlayer,waypoints[pWay->iIndices[i]].origin,waypoints[pWay->iIndices[is]].origin,10,10,255,255,255,255,10);
				break;
			}
			else{
				if(iOCount){
					iFarGoal = -1;
					iGoal = -1;
					//DEBUG_CLIENTCOMMAND(pEdict,"say no poss to get there");
				}
				//if(pEdictPlayer)WaypointDrawBeam(pEdictPlayer,waypoints[pWay->iIndices[i]].origin,waypoints[pWay->iIndices[is]].origin,10,10,255,255,100,10,10);
			}
		}
	}
}

long CBotBase :: Change2Weapon(const long lWeapon){
	if(f_DenyWChange < gpGlobals->time && lWeapon < 32&& lWeapon >= 0){
		FakeClientCommand(pEdict, "%s", weapon_defs[lWeapon].szClassname);
		return lWeapon;
	}
	else{
		return -1;
	}
}

long CBotBase :: ChangeToLWeapon(void){
	if(IsCWeaponPrimary()){		// changing from primary to secondary
		if(HasSecondary()){			// is this weapon sec ?
			// change to this weapon
			Change2Weapon(HasSecondary());
		}
	}
	else{
		if(!IsCWeaponKnife())	// if current weapon secondary switch to knife
			FakeClientCommand(pEdict,"weapon_knife");
	}
	return 0;
}

#define _CORR .7

void CBotBase :: ShootAtEnemy( void ){	
	//float fDiffYaw,fDiffPitch;
	// aim for the head and/or body
	VLookTo = BodyTarget( pBotEnemy );
	Vector v_enemy = ( VLookTo ) - GetGunPosition(pEdict)
		,v_enemyN;

	if(GOrder.lTypeoG){
		GOrder.lState = GO_QUIT;
	}
	
	Vector v_angle = UTIL_VecToAngles( v_enemy ) + v_Offset;
	
	// Paulo-La-Frite - START bot aiming bug fix
	if (v_angle.x > 180)
		v_angle.x -=360;

	// set the body angles to point the gun correctly
	Vector angles;
	angles.x = v_angle.x / 3.0;
	angles.y = v_angle.y;
	angles.z = 0;
	
	// adjust the view angle pitch to aim correctly (MUST be after body v.angles stuff)
	v_angle.x = -v_angle.x;
	// Paulo-La-Frite - END

	FixIdealYaw();
	FixIdealPitch();
	
	pEdict->v.ideal_yaw = v_angle.y;
	pEdict->v.idealpitch = v_angle.x;

	fIdealAngleYaw = angles.y;
	fIdealAnglePitch = angles.x;

	pEdict->v.angles.z = 0;
	/*fDiffPitch = fIdealAnglePitch - pEdict->v.angles.x;
	if(fDiffPitch > 20)
		fDiffPitch = 20;
	fIdealAnglePitch = angles.x + fDiffPitch * _CORR;
	fDiffYaw = fIdealAngleYaw - pEdict->v.angles.y;
	if(fDiffYaw > 20)
		fDiffYaw = 20;
	fIdealAngleYaw = angles.y + fDiffYaw * _CORR;*/

	// see if it's already time to shoot or if bot should even wait for some time ... do this with dotproduct
	v_enemyN = v_enemy.Normalize();
	Vector v_shoot;
	v_shoot = pEdict->v.angles;
	v_shoot.x = -v_shoot.x * 3;
	v_shoot.z = 0;  // reset roll to 0 (straight up and down)
	
	UTIL_MakeVectors( v_shoot );
	float fDotP = DotProduct(gpGlobals->v_forward,v_enemyN);
	
	/*if(g_b5th){
	WaypointDrawBeamDebug(listenserver_edict,pEdict->v.origin,pEdict->v.origin+v_enemyN*100,10,10,0,250,250,200,10);
	WaypointDrawBeamDebug(listenserver_edict,pEdict->v.origin,pEdict->v.origin+gpGlobals->v_forward*100,10,10,250,0,250,200,10);
}*/
	
	// if ya've a sniper weapon, go slowly and zoom, if not already done
	if (IsCWeaponSniper()){
		if(fDotP < .99
			&& fEnemyDist > 100){
			/*if(pEdictPlayer){
				WaypointDrawBeam(pEdictPlayer,pEdict->v.origin,pEdict->v.origin+Vector(0,0,50),10,10,0,250,250,200,10);
			}*/
			f_Delay = gpGlobals->time + .01;

			//DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin + Vector(40,40,100),10,0,255,0,255,255,0);
		}
		if(i_FOV <90){	// do some stuff about second zooming state or ...
		}
		else{					// not already zooming
			lButton |= IN_ATTACK2;		// just do it
			if(mod_id == CSTRIKE_DLL){
				if(current_weapon.iId == CS_WEAPON_AWP||
					current_weapon.iId == CS_WEAPON_G3SG1||
					current_weapon.iId == CS_WEAPON_SG550){
					if(f_Delay < gpGlobals->time - 1.f){
						f_Delay = gpGlobals->time + .2f;
			//DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin + Vector(40,40,50),10,0,255,0,255,255,0);
					}
				}
				else{
					if(f_Delay < gpGlobals->time - 1.f){
						f_Delay = gpGlobals->time + .1f;
		//	DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin + Vector(40,40,150),10,0,255,0,255,255,0);
					}
				}
			}
			//cout << i_FOV << " - temp" << endl;
		}
	}
	else{
		if(fDotP < .9
			&& fEnemyDist > 100){
			/*if(pEdictPlayer){
				WaypointDrawBeam(pEdictPlayer,pEdict->v.origin,pEdict->v.origin+Vector(0,0,50),10,10,0,250,250,200,10);
			}*/
			f_Delay = gpGlobals->time + .01;
		}

			//DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin + Vector(40,40,150),10,0,255,255,255,255,0);
	}
	// select the best weapon to use at this distance and fire...
	FireWeapon( v_enemy );
}

Vector CBotBase :: BodyTarget( edict_t *pBotEnemy){
	Vector target;
	float f_distance;
	/*float f_scale;
	float fSpeed = f_AMomentum;*/
	
	f_distance = (pBotEnemy->v.origin - pEdict->v.origin).Length();
	
	/*if (f_distance > 1000)
		f_scale = 1.0;
	else if (f_distance > 100)
		f_scale = f_distance / 1000.0;
	else
		f_scale = 0.1;*/

// HACKHACK : TODO : Fav weapon handling, sniper weapon handling. This has to be a bit more sophisticated, because the siming is a process, cannot be done a simply changing one shitty variable in one function

/*	if(pBot->lPWeapon==pBot->current_weapon.iId){	// favourite weapons will be more accurate
		fAccuracy /= 1.1f;
	}*/

	target = pBotEnemy->v.origin + pBotEnemy->v.view_ofs * (.5+f_AimHead/2.0);
	
	return target;
}

bool CBotBase :: BoolTestOnButtonWay(int iNearWP,int iGoal){
	CWay pWay;
	ConvertFloyd2Way(bot_teamnm,iNearWP,iGoal,&pWay);		// convert to way
	return BoolTestOnButtonWay(pWay,iNearWP,iGoal);
}

bool CBotBase :: BoolTestOnButtonWay(CWay &pWay,int iNearWP,int iGoal){
	// look if there is something (door lift etc) in between which has to be opened 
	// by a button etc
	int i,is;
	edict_t *pEnt;
	TraceResult tr;
	//char szClassname[100];
	
	for(i=0,is=1;is < pWay.iNum;i++,is++){
		/*if(WPStat.GetVisible(pWay.iIndices[i],pWay.iIndices[is]))
			continue;*/

		UTIL_TraceLine(waypoints[pWay.iIndices[i]].origin,waypoints[pWay.iIndices[is]].origin,ignore_monsters,0,&tr);
		
		if(tr.pHit != 0){		// hit an ent
			pEnt = UTIL_FindEntityByString( 0, "target", STRING( tr.pHit->v.targetname ) ); // todo
			if(pEnt)
				return false;
		}
	}
	return true;
}

bool CBotBase :: PauseShoot(void){
	if(IsCWeaponSecondary()||IsCWeaponGrenade()){
		if(pEdict->v.button & IN_ATTACK){
			return true;
		}
		else{
			return false;
		}
	}
	else{
		if(f_PauseShoot > gpGlobals->time){
			return true;
		}
		else{
			/*if(RANDOM_LONG(0,700) < WeaponDefs.dpWeaponPause[CSTRIKE_DLL][current_weapon.iId]
				&&f_PauseShoot+1.0f<gpGlobals->time){
				f_PauseShoot = gpGlobals->time + WeaponDefs.dpWeaponPauseTime[CSTRIKE_DLL][current_weapon.iId];
			}*/
			return false;
		}
	}
}

void CBotBase :: VATurnTo(const Vector &vector){
	// vector has to be the diff vector from bot to aim
	pEdict->v.v_angle = UTIL_VecToAngles( vector );
	
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
	
	pEdict->v.ideal_yaw = pEdict->v.v_angle.y;
	pEdict->v.idealpitch = pEdict->v.v_angle.x;
	
	FixIdealYaw();
}

bool CBotBase :: FireWeapon( Vector &v_enemy )
{
	// Use the Vector argument of the emeny to aim at the enemy and fire
	// here.  Return TRUE if bot fired weapon (enough ammo, etc.) otherwise
	// return FALSE to indicate failure (maybe bot would runaway then).
	if(f_Delay > gpGlobals->time)
		return false;
	
	lButton |= IN_ATTACK;
	return true;
}

long CBotBase :: IsCWeaponPrimary(void){
	return IsPrimaryWeapon(1<<current_weapon.iId);
}

long CBotBase :: IsCWeaponSecondary(void){
	return IsSecondaryWeapon(1<<current_weapon.iId);
}

long CBotBase :: IsCWeaponShotgun(void){
	return IsShotgunWeapon(1<<current_weapon.iId);
}

long CBotBase :: IsCWeaponSubM(void){
	return IsSubMWeapon(1<<current_weapon.iId);
}

long CBotBase :: IsCWeaponRifle(void){
	return IsRifleWeapon(1<<current_weapon.iId);
}

long CBotBase :: IsCWeaponSniper(void){
	return IsSniperWeapon(1<<current_weapon.iId);
}

long CBotBase :: IsCWeaponMachineGun(void){
	return IsMachineGunWeapon(1<<current_weapon.iId);
}

long CBotBase :: IsCWeaponGrenade(void){
	return IsGrenadeWeapon(1<<current_weapon.iId);
}

long CBotBase :: IsCWeaponKnife(void){
	return IsKnifeWeapon(1<<current_weapon.iId);
}

long CBotBase :: HasSniper(void){
	return IsSniperWeapon(bot_weapons);
}

long CBotBase :: HasMachineGun(void){
	return IsMachineGunWeapon(bot_weapons);
}

long CBotBase :: HasGrenade(void){
	return IsGrenadeWeapon(bot_weapons);
}

long CBotBase :: HasPrimary(void){
	return IsPrimaryWeapon(bot_weapons);
}

long CBotBase :: HasSecondary(void){
	return IsSecondaryWeapon(bot_weapons);
}

long CBotBase :: HasShotgun(void){
	return IsShotgunWeapon(bot_weapons);
}

long CBotBase :: HasKnife(void){
	return IsKnifeWeapon(bot_weapons);
}

long CBotBase :: HasSubM(void){
	return IsSubMWeapon(bot_weapons);
}

long CBotBase :: HasRifle(void){
	return IsRifleWeapon(bot_weapons);
}

bool CBotBase :: HasWeapon(long lbit){
	return bot_weapons&lbit?true:false;
}

long CBotBase :: WeaponModel2ID(const char *szModel){
	return -1;
}

bool CBotBase :: HandleOrders(void){
	return false;
}

void CBotBase :: AdaptAvoid(void){
	float fDiff = fLeftDist - fRightDist;
	if(fDiff > .5){
		i_avoidstrafe = -1;
	}
	else if(fDiff < -.5){
		i_avoidstrafe = 1;
	}
}

edict_t *CBotBase :: FindEnemy(){
	return 0;
}

void CBotBase :: Fight(){
}

netvar_t CBotBase :: ConvertDistance(const netvar_t &dDistance){			// convert distance to [-1;1]
	netvar_t dReturn;
	
	const netvar_t dnear = 200;
	const netvar_t dmid = 1000;
	
	if(dDistance > 0 && dDistance < dnear){
		dReturn = dDistance / dnear - 1;
	}
	else if(dDistance > dnear && dDistance < dmid){
		dReturn = (dDistance-dnear) / (dmid-dnear);
	}
	else{
		dReturn = 1;
	}
	return dReturn;
}

bool CBotBase :: GotoHidingPlace(void){
	// detect in which direction u've to strafe and determine the best backward/forward velocity
	// forward backward - dotproduct of normalized vectors of view and locationdiff to bot's origin
	// is positive if bot should go forward and negative, if bot should go backward ...
	Vector VView;
	Vector VLDiff = pEdict->v.origin - VHidingPlace;		// differnce of bot's origin to 'safe' location
	Vector VCrossProduct;
	float fDotProduct;
	
	VView.z = 0;
	VLDiff.z = 0;		// make 2d
	
						/*VView = Vector(cos(pEdict->v.angles.y * M_PI / 180.0)
	,sin(pEdict->v.angles.y * M_PI / 180.0),0);*/
	VView = pEdict->v.origin-pBotEnemy->v.origin;
	VView.Normalize();				// really necessary ?
	VLDiff.Normalize();				//         ||
	
	fDotProduct = DotProduct(VView,VLDiff);
	VCrossProduct = CrossProduct(VView,VLDiff);
	
	if(fDotProduct == 0.0f){// is zero ... don't do anything for this time ...
	}
	else if(fDotProduct < 0.0f){	// pos -> go forward ...
		f_move_speed = pEdict->v.maxspeed / 1.5f;
	}
	else{							// neg -> go backward ...
		f_move_speed = -pEdict->v.maxspeed / 1.5f;
	}
	
	if(VCrossProduct.z == 0.0f){		// seems to be pretty near ...
	}
	else if(VCrossProduct.z > 0){	// pos
		f_strafe -= _MAXSTRAFE*2;	// go left
	}
	else{							// neg
		f_strafe += _MAXSTRAFE*2;	// go right
	}
	return true;
}

bool CBotBase :: SearchHidingPlace(edict_t *pEnemy){
	// this function should determine a place to the left or the right, where the bot can hide
	// i.e. where the enemy pEnemy cannot see the bot
	//
	// This should be done by tracing to some points to the left and the right
	
	Vector VLeRi;	// a vector to left or right ( crossproduct of view_angles and (0,0,1) )
	Vector VBotView;
	Vector VHide1,VHide2;
	bool bHideFound1 = false,bHideFound2=false;
	TraceResult tr1,tr2,trdown;
	float fSpace1,fSpace2,
		fDistance1,fDistance2,
		fDepth,fLDepth;
	
	VBotView = pEdict->v.v_angle;
	VBotView = Vector(cos(pEdict->v.v_angle.y * M_PI / 180.0)
		,sin(pEdict->v.v_angle.y * M_PI / 180.0),0);
	VBotView.z=0;
	VBotView.Normalize();
	
	//det vector to left and right ...
	VLeRi = CrossProduct(VBotView,Vector(0,0,1));
	VLeRi.Normalize();
	
	// trace _LERITRACE to left and right to determine space
	UTIL_TraceLine(pEdict->v.origin,pEdict->v.origin + _LERITRACE * VLeRi,dont_ignore_monsters,
		dont_ignore_glass,pEdict->v.pContainingEntity,&tr1);
	fSpace1 = tr1.flFraction * _LERITRACE;
	UTIL_TraceLine(pEdict->v.origin,pEdict->v.origin - _LERITRACE * VLeRi,dont_ignore_monsters,
		dont_ignore_glass,pEdict->v.pContainingEntity,&tr2);
	fSpace2 = tr2.flFraction * _LERITRACE;
	
	fLDepth = 0;
	
	for(fDistance1=0;fDistance1 < fSpace1;fDistance1 += _HIDESTEP){	// first trace to tr1 side
		VHide1 = pEdict->v.origin + fDistance1 * VLeRi;
		UTIL_TraceLine(VHide1,VHide1 - Vector(0,0,1000),dont_ignore_monsters,
			dont_ignore_glass,pEdict->v.pContainingEntity,&trdown);
		
		fDepth = trdown.flFraction * 1000.0f;
		if(fabs(fDepth - fLDepth) > 200){	// if bot can fall deep down ...
			break;
		}
		
		fLDepth = fDepth;
		
		if(!FVisible(VHide1,pEnemy)){
			bHideFound1 = true;
			break;
		}
	}
	fLDepth = 0;
	for(fDistance2=0;fDistance2 < fSpace2;fDistance2 += _HIDESTEP){	// then trace to tr2 side
		VHide2 = pEdict->v.origin - fDistance2 * VLeRi;
		UTIL_TraceLine(VHide2,VHide2 - Vector(0,0,1000),dont_ignore_monsters,
			dont_ignore_glass,pEdict->v.pContainingEntity,&trdown);
		
		fDepth = trdown.flFraction * 1000.0f;
		if(fabs(fDepth - fLDepth) > 200){	// if bot can fall deep down ...
			break;
		}
		
		fLDepth = fDepth;
		
		if(!FVisible(VHide2,pEnemy)){
			bHideFound2 = true;
			break;
		}
	}
	if(bHideFound1 && bHideFound2){	// both possible ?
		if(fDistance1 < fDistance2){	// decide on the shortest one ...
			bHideFound2 = false;		// if 1 is nearer -> only use 1
		}
		else{
			bHideFound1 = false;		// if 2 is nearer -> only use 2
		}
	}
	
	if(bHideFound1){
		VHidingPlace = VHide1;
		return true;
	}
	else if(bHideFound2){
		VHidingPlace = VHide2;
		return true;
	}
	else
		return false;
}

int CBotBase :: SearchHidingWP(edict_t *pEnemy,int iL){
	// this function should determine a waypoint, where the bot can hide
	// i.e. where the enemy pEnemy cannot see the bot
	//
	// This should be done by searching the the nearest invisible waypoint to the enemies nearest
	// waypoint and then getting the nearest invisible waypoint to this waypoint and so forth


	// todo : all way check, the last wp isn't sufficient :)

	float fMin=0;
	int ischl;
	//int iHideWP;

	int iNIVWP=-1,iNTemp;
	float fDist,fTempD;
	int index,iVindex;
	bool bVisible;
	CWay WHide;
	WHide.iNum = 1;
	if(iL == -2)
		iL = RANDOM_LONG(0,1);

	int iENWp = iNearWP = WaypointFindNearest(pEnemy,1000,-1,fMin,false,false,false);

	fMin = 0;

	WHide.iIndices[0] = iNIVWP = WaypointFindNearest(pEdict,1000,bot_teamnm,fMin,false,false,false);

	for ( ischl = iL;ischl > -1;ischl--){
		fDist = 1000000;
		iNTemp = -1;
		for(index = 0; index < num_waypoints ; index ++){
			if(waypoints[index].flags & W_FL_DELETED)
				continue;
			if(waypoints[index].flags & W_FL_LADDER)
				continue;
			if(waypoints[index].flags & W_FL_AIMING)
				continue;
			if ((waypoints[index].flags & W_FL_TEAM_SPECIFIC) &&
				((waypoints[index].flags & W_FL_TEAM) != bot_teamnm))
				continue;
			
			if(!WPStat.GetVisible(iENWp,index)){
				bVisible = false;
				for(iVindex = 0; iVindex < WHide.iNum;iVindex++){
					if(WPStat.GetVisible(WHide.iIndices[iVindex],index)){
						bVisible =true;
						break;
					}
				}
				if(bVisible)
					continue;
			}
			else{
				continue;
			}

			if(WPStat.GetVisible(iNIVWP,index))
				continue;

			fTempD = WaypointDistanceFromTo(iNIVWP,index,bot_teamnm);
			if(fTempD < fDist
				&& fTempD < 800){
				fDist = fTempD;
				iNTemp = index;
			}
		}
#ifdef _DEBUG
		if(iNIVWP != -1 && iNTemp != -1){DEBUG_DRAWBEAM(listenserver_edict,waypoints[iNIVWP].origin+Vector(0,0,bot_teamnm*10),waypoints[iNTemp].origin+Vector(0,0,bot_teamnm*10),20,0,200,0,bot_teamnm*200,200,20);}
#endif
		if(iNTemp == -1)
			break;

		WHide.iIndices[WHide.iNum] = iNIVWP = iNTemp;
		WHide.iNum++;
	}
	return iNIVWP;
}

float CBotBase :: ChangePitch( void )
{
	float ideal;
	float current;
	
	// turn from the current v_angle Pitch to the ideal_Pitch by selecting
	// the quickest way to turn to face that direction
	
	current = pEdict->v.v_angle.x;
	
	ideal = pEdict->v.idealpitch;
	
	float a=f_AMomentum,idealSpeed,c=f_ASpeed,aNow;
	
	idealSpeed = (ideal - current);
	idealSpeed = idealSpeed*c;
	aNow = exp(log(a) * g_msecval / 50.f);
	fViewSpeedPitch = fViewSpeedPitch*aNow + idealSpeed*(1.f-aNow);	// a+b=1, etwa a = 0.8, b=0.2
	
	current += fViewSpeedPitch * 1/*pEdict->v.pitch_speed*/ * (g_msecval/50.f);
	
	// check for wrap around of angle...
	while (current > 180)
		current -= 360;
	while (current < -180)
		current += 360;
	
	pEdict->v.v_angle.x = current;
	
	return 1;  // return number of degrees turned
}

float CBotBase :: ChangeBodyPitch( void )
{
	float ideal;
	float current;
	
	// turn from the current v_angle Pitch to the ideal_Pitch by selecting
	// the quickest way to turn to face that direction
	
	if(fIdealAnglePitch > 180)
		fIdealAnglePitch -= 360;
	
	current = pEdict->v.angles.x;
	
	ideal = fIdealAnglePitch;
	
	// find the difference in the current and ideal angle
	//fADiff = abs(current - ideal);
		  
	float a=f_AMomentum,idealSpeed,c=f_ASpeed,aNow;
	
	idealSpeed = (ideal - current);
	idealSpeed = idealSpeed*c;
	aNow = exp(log(a) * g_msecval / 50.0f);
	fAngleSpeedPitch = fAngleSpeedPitch*aNow + idealSpeed*(1.0f-aNow);	// a+b=1, etwa a = 0.8, b=0.2
	
	current += fAngleSpeedPitch * 1.0f * (g_msecval/50.0f);
	
	//DEBUG_CLIENTCOMMAND(pEdict,"say %f",current);
	
	// check for wrap around of angle...
	while (current > 180)
		current -= 360;
	while (current < -180)
		current += 360;
	
	pEdict->v.angles.x = current;
	
	return 1;  // return number of degrees turned
}

float CBotBase :: ChangeYaw( void ){
	float ideal;
	float current;
	//float current_180;  // current +/- 180 degrees
	float fADiff;
	//float fAccY;
	
	// turn from the current v_angle yaw to the ideal_yaw by selecting
	// the quickest way to turn to face that direction
	
	current = pEdict->v.v_angle.y;
	
	ideal = pEdict->v.ideal_yaw;

/*#ifdef _DEBUG
	if(pBotEnemy){
		Vector VT;
		VT.x = VT.y = VT.z = 0;
		VT.y = current;
		UTIL_MakeVectors(VT);
		DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin+gpGlobals->v_forward*100,8,0,000,200,0,200,0);
		UTIL_MakeVectors(VT);
		VT.y = ideal;
		DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin+gpGlobals->v_forward*100,8,0,200,000,0,200,0);
	}
#endif*/
	
	// find the difference in the current and ideal angle
	fADiff = fabs(current - ideal);
	
	if(fADiff>180.0f){
		if(ideal > 0){
			ideal -= 360.0f;
		}
		else{
			ideal += 360.0f;
		}
	}
	float a=f_AMomentum,idealSpeed,c=1,aNow;
	
	fADiff = fabs(current - ideal);
	
	idealSpeed = (ideal - current);
	idealSpeed = idealSpeed*c;
	aNow = exp(log(a) * g_msecval / 50.0f);
	fViewSpeedYaw = fViewSpeedYaw*aNow + idealSpeed*(1.0f-aNow);	// a+b=1, etwa a = 0.8, b=0.2
	
	current += fViewSpeedYaw * /*pEdict->v.yaw_speed*/1.0f * (g_msecval/50.0f);
	
	// check for wrap around of angle...
	while (current > 180)
		current -= 360;
	while (current < -180)
		current += 360;
	
	pEdict->v.v_angle.y = current;
	
	return 1;  // return number of degrees turned
}

float CBotBase :: ChangeBodyYaw( void ){
	float ideal;
	float current;
	//float current_180;  // current +/- 180 degrees
	float fADiff;
	//float fAccY;
	
	// turn from the current v_angle yaw to the ideal_yaw by selecting
	// the quickest way to turn to face that direction
	
	if(fIdealAngleYaw > 180)
		fIdealAngleYaw -= 360;
	
	current = pEdict->v.angles.y;

	ideal = fIdealAngleYaw;
	
/*#ifdef _DEBUG
	if(pBotEnemy){
		Vector VT;
		VT.x = VT.y = VT.z = 0;
		VT.y = current;
		UTIL_MakeVectors(VT);
		DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin+gpGlobals->v_forward*100,8,0,0,0,200,200,0);
		UTIL_MakeVectors(VT);
		VT.y = ideal;
		DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin+gpGlobals->v_forward*100,8,0,0,200,0,200,0);
	}
#endif*/
	
	// find the difference in the current and ideal angle
	fADiff = fabs(current - ideal);
	
	if(fADiff>180.0f){
		if(ideal > 0){
			ideal -= 360.0f;
		}
		else{
			ideal += 360.0f;
		}
	}
	
	float a=f_AMomentum,idealSpeed,c=f_ASpeed,aNow;
	
	fADiff = fabs(current - ideal);
	
	idealSpeed = (ideal - current);
	idealSpeed = idealSpeed*c;
	aNow = exp(log(a) * g_msecval / 50.0f);
	fAngleSpeedYaw = fAngleSpeedYaw*aNow + idealSpeed*(1.0f-aNow);	// a+b=1, etwa a = 0.8, b=0.2
	
	current += fAngleSpeedYaw * 1.0f * (g_msecval/50.0f);
	
	// check for wrap around of angle...
	while (current > 180)
		current -= 360;
	while (current < -180)
		current += 360;
	
	pEdict->v.angles.y = current;
	
	return 1;  // return number of degrees turned
}

void CBotBase :: UpdateSkill(void){
	CSkillItem temp;
	char szName[32];
	
	InstantTurn();
	
	Skill.GetSkill(bot_skill,temp);
	
	f_AMomentum			= temp.fVSM/100.0;
	f_ASpeed			= temp.fVSC/100.0;
	f_ReactionTime		= temp.fReact;
	bSeeThru			= temp.fShootThru > 90.0;
	f_InDistance		= temp.fInDistance;
	f_TPD				= temp.fTPD;
	f_ProneInDistance	= temp.fProneInDistance;
	f_ProneTPD			= temp.fProneTPD;
	i_ITP				= int(temp.fITurnProb);
	f_MaxRecoil			= temp.fMaxRecoil;

	char *infobuffer;
	int clientIndex;
	
	infobuffer = GET_INFOKEYBUFFER( pEdict );
	clientIndex = ENTINDEX( pEdict );
	
	MakeName(szName,name,bot_skill,d_Manner);
	
	SET_CLIENT_KEYVALUE( clientIndex, infobuffer, "name", szName );
}

void CBotBase :: InstantTurn(void){
	// head directly to ( supressing aiming sys )
	pEdict->v.angles.y = fIdealAngleYaw;
	pEdict->v.angles.x = fIdealAnglePitch;
	pEdict->v.v_angle.y = pEdict->v.ideal_yaw;
	pEdict->v.v_angle.x = pEdict->v.idealpitch;
	fViewSpeedYaw=0;
	fViewSpeedPitch = 0;
	fAngleSpeedPitch = 0;
	fAngleSpeedYaw = 0;
}

void CBotBase :: MakeName(char *szName,const char *szBotName,int iSkill,float fAgg){
	if(CVAR_BOOL(jb_prefixaggression)){
		if(fAgg < -.5){
			sprintf(szName,"%s%s",jb_prefixdefensive->string,szBotName);
		}
		else if(fAgg > .5){
			sprintf(szName,"%s%s",jb_prefixaggressive->string,szBotName);
		}
		else{
			sprintf(szName,"%s%s",jb_prefixnormal->string,szBotName);
		}
	}
	else{
		sprintf(szName,"%s",szBotName);
	}
	
	if(CVAR_BOOL(jb_suffixskill)){
		char szSkill[10];
		snprintf(szSkill,sizeof(szSkill),"(%i)",iSkill);
		strcat(szName,szSkill);
	}
}

bool CBotBase :: InitCamp(void){
/*pBot->WLookTo.iNum = 0;
	return false;*/
	CWay LookTo;
	TraceResult tr;
	int i,is;
	Vector vAngles,vDiff;
	float *pfAngles/*,fDiffAngle*/,fDistance;
	
	LookTo.iNum = 0;	// del way
	
	for (i=0; i < num_waypoints; i++)
	{
		if ((waypoints[i].flags & W_FL_DELETED) == W_FL_DELETED)
			continue;  // skip any deleted waypoints
		
		// DON'T IGNORE AIMING STUFF !!!!!!!
		
		fDistance =(pEdict->v.origin - waypoints[i].origin).Length();
		
		if(fDistance > 200.0f){
			// if waypoint is visible from current position (even behind head)...
			UTIL_TraceLine( pEdict->v.origin + pEdict->v.view_ofs, waypoints[i].origin,
				ignore_monsters, ignore_glass,pEdict->v.pContainingEntity, &tr );

			if (tr.flFraction >= 1.0){	// visible
				if(LookTo.iNum < MAX_WAYPOINTS){
					LookTo.iIndices[LookTo.iNum] = i;		// add wp
					LookTo.iNum++;
					continue;
				}
			}
			
			UTIL_TraceLine( pEdict->v.origin , waypoints[i].origin,
				ignore_monsters, ignore_glass,pEdict, &tr );
			
			if (tr.flFraction >= 1.0){	// visible without view offset ...
				if(LookTo.iNum < MAX_WAYPOINTS){
					LookTo.iIndices[LookTo.iNum] = i;		// add wp
					LookTo.iNum++;
					continue;
				}
			}
		}
	}
	if(LookTo.iNum){
		pfAngles = new float[MAX_WAYPOINTS];		// alloc mem for angles - // ? fix crash ?
		
		for(i=0;i<LookTo.iNum;i++){
			vDiff = pEdict->v.origin - waypoints[LookTo.iIndices[i]].origin;
			vAngles = UTIL_VecToAngles(vDiff);
			pfAngles[i] = vAngles.y;
		}
		
		// look for too small angles diffs ...
		/*for(i=0;i<LookTo.iNum;i++){
		for(is=0;is < LookTo.iNum;is++){
		if(LookTo.iNum > 2 && i!=is){
		fDiffAngle = fabs(pfAngles[i] - pfAngles[is]);
		fDiffAngle>180.0f?fDiffAngle-=180.0f:fDiffAngle=fDiffAngle;	// don't wrap around ...
		
		  if(fDiffAngle > 0.1f && fDiffAngle < _ANGLEDIFFCAMP){		// too close -> remove one ...
		  // copy last to this one and dec number
		  LookTo.iIndices[is] = LookTo.iIndices[LookTo.iNum];
		  pfAngles[is] = pfAngles[LookTo.iNum];
		  LookTo.iNum--;
		  }
		  }
		  else break;
		  }
	}*/
		
		// sort the wp indices ... just bsort, cause it's easy                       ------OPTIMIZE !!!!!!!!!!!!!
		bool bC = true;
		while(bC){
			bC = false;
			for(is=0;is < LookTo.iNum-1;is++){
				if(pfAngles[is] < pfAngles[is+1]){	// if not the wannabe sorted order ...
					float ftemp;
					short int itemp;
					ftemp = pfAngles[is];
					pfAngles[is] = pfAngles[is+1];
					pfAngles[is+1] = ftemp;
					
					itemp = LookTo.iIndices[is];
					LookTo.iIndices[is] = LookTo.iIndices[is+1];
					LookTo.iIndices[is+1] = itemp;

					bC = true;
				}
			}
		}
		/*FILE *fhd;fhd=fopen("testsort.txt","a");fprintf(fhd,"\n-------------\n");for(i=0;i<LookTo.iNum;i++){fprintf(fhd,"%f\n",pfAngles[i]);}fclose(fhd);*/
		
		// copy to bot structure ...
		WLookTo = LookTo;
		sLookToChange = 1;
		
		delete [] pfAngles;						// free angle ram ...
		
		return true;
	}
	return false;
}

bool CBotBase :: SearchForIL(void){			// searchin for an intersesting lcoation, i.e. p.ex. when u run on along a big open place, look there to see enemies and not just run alond the wall ( -> e.g. cs_assault )
	if(f_LastFight > gpGlobals->time - 1.f)
		return false;
	if(f_LTSFIP < gpGlobals->time - 1.0){
		//TraceResult tr;
		Vector VEdictOrigin = pEdict->v.origin + pEdict->v.view_ofs +pEdict->v.velocity/2.0f;
		f_LTSFIP = gpGlobals->time;
		int i;
		Vector VRunningN = VRunningTo - pEdict->v.origin;
		Vector VToWPN;
		float fMaxFInd=0;			// max distance to an wp, calculated with 
		int iMaxFDWP=-1,
			iWP = i_CurrWP;
		float fThisFD,fThisRD,fDot;
		//VRunningN.z = 0;
		VRunningN = VRunningN.Normalize();

		if(iWP == -1){
			iWP = WaypointFindNearest(VEdictOrigin,pEdict,10000,bot_teamnm,false);
		}
		
		for (i=num_waypoints;i; i--)			// go through all wps
		{
			if ((waypoints[i].flags & W_FL_DELETED) == W_FL_DELETED)
				continue;  // skip any deleted waypoints
			
			/*UTIL_TraceLine(VEdictOrigin,waypoints[i].origin,ignore_monsters,ignore_glass,pEdict,&tr);
			if(tr.flFraction != 1.0f)		// skip if not visible ...
				continue;*/
			if(!WPStat.GetVisible(iWP,i))
				continue;

			VToWPN = waypoints[i].origin - VEdictOrigin;
			fThisRD = VToWPN.Length();
			if(fThisRD > 300){
				VToWPN = VToWPN.Normalize();
				fDot = DotProduct(VToWPN,VRunningN);
				if(fDot < -.5)		// ignore wps lying behind
					fDot = -1.0;
				
				fThisFD = (1.0-fabs(fDot))			// take the view angle into account
					* fThisRD						// distance
					* (1.0 + float(WPStat.d.FItem[i].lKill) / float(WPStat.lKillMax));	// is there something dangerous ?
				if(fThisFD > fMaxFInd){
					fMaxFInd = fThisFD;
					iMaxFDWP = i;
				}
			}
		}
		if(iMaxFDWP != -1){
			if(fMaxFInd > f_IOrigD*1.5f){
				if(f_LookTo < gpGlobals->time-2.5f){
					VLookTo = waypoints[iMaxFDWP].origin;
					f_LookTo = gpGlobals->time + 1.0;
					//DEBUG_CLIENTCOMMAND(pEdict,"say sfai");
				}
			}
			if(fMaxFInd > f_IOrigD*2.5f){
			}
			f_IOrigD = fMaxFInd;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool CBotBase ::HeadToward(Vector &vAim){
	Vector v_diff = vAim - pEdict->v.origin;
	Vector bot_angles = UTIL_VecToAngles( v_diff );
	
	pEdict->v.ideal_yaw = bot_angles.y;
	pEdict->v.idealpitch = -bot_angles.x;
	
	FixIdealYaw();
	FixIdealPitch();
	return true;
}

void CBotBase :: FixIdealPitch(void)
{
	// check for wrap around of angle...
	if (pEdict->v.idealpitch > 180)
		pEdict->v.idealpitch -= 360;
	
	if (pEdict->v.idealpitch < -180)
		pEdict->v.idealpitch += 360;
}

bool CBotBase :: GoToSpEnt( void ){
	return false;
}

bool CBotBase :: Bored(void){
	return false;
}

bool CBotBase :: HeadTowardWaypoint( void ){
	return false;
}

bool CBotBase :: HandleNearWP(int iNearWP, bool &bReturn){
	bReturn = false;
	if(iNearWP != -1){
		float fMin,fDistance;
		// crouch ??
		if(waypoints[iNearWP].flags & W_FL_CROUCH
			&& !bReplay){
			f_ducktill = gpGlobals->time + 1.0f;		// duck for one second to get under the 'thing'
		}
		
		if(waypoints[iNearWP].flags & W_FL_DONTAVOID){
			f_dont_avoid = gpGlobals->time + .5;
			f_Pause = gpGlobals->time - 1.0;
		}
		if(waypoints[iNearWP].flags & W_FL_DAF){
			f_IgnoreFall = gpGlobals->time + .5;
		}
		if(waypoints[iNearWP].flags & W_FL_RESET){
			if(f_ResetWP < gpGlobals->time - 1.0){
				i_CurrWP = -1;
				f_ResetWP = gpGlobals->time;
			}
		}
		// sniper ??
		if((waypoints[iNearWP].flags & W_FL_SNIPER
			&& (iGoal == iNearWP||iFarGoal == iNearWP))	/// is that the place we wanna go and camp ?
			&& f_noCamp < gpGlobals->time){
			//cout << "sniperroutine" << endl;
			fDistance = (waypoints[iNearWP].origin - pEdict->v.origin).Length();
			
			GetNearestPlayer(pEdict,bot_teamnm,fMin);
			if(fMin > 150.0){
				if(f_noCamp < gpGlobals->time
					&& !g_bBombPlanted
					&& (/*(HasPrimary()&&bot_health < 25) || */HasSniper())){			// see if bot is already allowed to sniper and if it has primary weapon
					fDistance = (waypoints[iNearWP].origin - pEdict->v.origin).Length();
					if(fDistance > 20){	// go near to wp
						HeadToward(waypoints[iNearWP].origin);
						VRunningTo = waypoints[iNearWP].origin;
						if(!(pEdict->v.button&IN_DUCK)){
							f_move_speed = pEdict->v.maxspeed / 1.5;
						}
						prev_speed = 0;		// ... to make other routines not to think, that bot is stuck
						f_dont_avoid = gpGlobals->time + 1.0f;
						return true;
					}
					else{
						if(Task.SearchT(BT_CAMP) != -1){
							ResetWPlanning();
							Task.AddTask(BT_CAMP,gpGlobals->time + _CAMPTIME * RANDOM_FLOAT(.5,2),0,0,0);
							InitCamp();
							if(CVAR_BOOL(jb_msgradio))
								FakeClientCommand(pEdict,"radio3;menuselect 5");
							if(f_RWKnife > gpGlobals->time)
								f_RWKnife = gpGlobals->time;
							f_UsedRadio = gpGlobals->time;
							bReturn = true;
							return true;
						}
					}
				}
			}
			else{		// if there's already a bot near to
				ResetWPlanning();
				f_noCamp = gpGlobals->time + 5.0;		// don't just around and try it a second time
				DecideOnWay();							// find a new way
			}
		}
		if(waypoints[i_CurrWP].flags & W_FL_LADDER){		// going down ladder
			f_dont_avoid = gpGlobals->time + 1.0;
			f_IgnoreFall = gpGlobals->time + 1.0f;
			//f_dont_check_stuck = gpGlobals->time + 1.0f;;
			if(!iOnLadder){
				if(!(pEdict->v.button&IN_DUCK)){
					f_move_speed /= 1.5f;					// slow down, when not on ladder
				}
			}
			
			lButton |= IN_FORWARD;
			if(pEdict->v.origin.z > waypoints[i_CurrWP].origin.z){	// going down ladder
				//f_ducktill = gpGlobals->time +.5;
			}
			if(!pELadder){
				while(pELadder = UTIL_FindEntityInSphere(pELadder,pEdict->v.origin,(waypoints[iNearWP].origin - pEdict->v.origin).Length()+50.0f)){
					if(FStrEq(STRING(pELadder->v.classname),"func_ladder"))
						break;
				}
			}
			if(pELadder){
				AdjustLadder();
				if(!iOnLadder){
					if(g_b5th){
						// look if there's another player near to the ladder. If the other player
						// is nearer, stop, otherwise go on ... only check this, if not on ladder
						edict_t *pEnt = 0,
							*pNearestEdict;
						float fLDistance;
						
						float fDNear = 100000000;
						Vector VLadderOrigin = (pELadder->v.absmin + pELadder->v.absmax)/2.0;
						
						for (int i = 0; i < gpGlobals->maxClients; i++){
							pEnt = INDEXENT(i + 1);
							
							// skip invalid players
							if ((pEnt) && (!pEnt->free) && (pEnt != pEdict))
							{
								if(!IsAlive(pEnt))
									continue;
								
								if(UTIL_GetTeam(pEnt) != bot_teamnm)
									continue;
								
								fLDistance = (pEnt->v.origin - VLadderOrigin).Length2D();
								
								if(fDNear > fLDistance){
									fDNear = fLDistance;
									pNearestEdict = pEnt;
								}
							}
						}
						if((pEdict->v.origin - VLadderOrigin).Length2D() > fDNear){	// bot isn't the nearest
							f_Pause = f_ducktill = gpGlobals->time + .5f;
							
							/*DEBUG_CLIENTCOMMAND(pEdict,"say stopping %s;%f-%f",STRING(pNearestEdict->v.netname),fDNear,(pEdict->v.origin - VLadderOrigin).Length2D());*/
						}
						else{			// no changes
						}
					}
				}
			}
		}
		if(waypoints[iNearWP].flags & W_FL_JUMP){	// highest priority
			if(f_ducktill > gpGlobals->time){
				f_ducktill = 0;
				lButton &=~ IN_DUCK;
			}
		}
		
	}
	else{
	}
	return false;
}

void CBotBase :: AdjustLadder( void ){
	Vector ladderLeft = pELadder->v.absmin;
	Vector ladderRight = pELadder->v.absmax;
	
	/*FILE *fhd;
	fhd=fopen("---logent.txt","a");
	fprintf(fhd,"%.0f\t%.0f\t%.0f\t\t",ladderRight.x,ladderRight.y,ladderRight.z);
	fprintf(fhd,"%.0f\t%.0f\t%.0f\n",ladderLeft.x,ladderLeft.y,ladderLeft.z);
	fclose(fhd);*/
	
	ladderLeft.z = ladderRight.z;
	Vector ladderSide = ladderLeft - ladderRight;
	Vector normal = CrossProduct( ladderSide, Vector(0,0,1) );
	normal = normal.Normalize();
	Vector LadderMiddle = (ladderLeft+ladderRight)/2.0f;
	Vector VLBDiff = pEdict->v.origin - LadderMiddle;
	Vector VLBDiffN = VLBDiff.Normalize();
	if(VLBDiff.Length() > 30){
		VLBDiff.z = ladderSide.z;
		float fDotLB = DotProduct(VLBDiffN,normal);
		
		if(fDotLB<0){
			normal =- normal;
			fDotLB = DotProduct(VLBDiffN,normal);
		}
		normal.z=VLBDiff.z=0;
		Vector VCross = CrossProduct(VLBDiff,normal);
		
		if(fabs(VCross.z)>0){
			if(VCross.z>0){
				f_strafe=_MAXSTRAFE;
				//DEBUG_CLIENTCOMMAND(pEdict,"say right");
			}
			else{
				f_strafe=-_MAXSTRAFE;
				//DEBUG_CLIENTCOMMAND(pEdict,"say left");
			}
		}
	}
}

// botman's ladder func
void CBotBase :: OnLadder( void )
{
	Vector v_src, v_dest, view_angles;
	TraceResult tr;
	float angle = 0.0;
	bool done = FALSE;
	
	// get entity of ladder, if not done ...
	if(!pELadder){
		while(pELadder = UTIL_FindEntityInSphere(pELadder,pEdict->v.origin,100)){
			if(FStrEq(STRING(pELadder->v.classname),"func_ladder"))
				break;
		}
	}
	
	// check if the bot has JUST touched this ladder...
	if (iOnLadder == LADDER_UNKNOWN)
	{
		// try to square up the bot on the ladder...
		while ((!done) && (angle < 180.0))
		{
			// try looking in one direction (forward + angle)
			view_angles = pEdict->v.v_angle;
			view_angles.y = pEdict->v.v_angle.y + angle;
			
			if (view_angles.y < 0.0)
				view_angles.y += 360.0;
			if (view_angles.y > 360.0)
				view_angles.y -= 360.0;
			
			UTIL_MakeVectors( view_angles );
			
			v_src = pEdict->v.origin + pEdict->v.view_ofs;
			v_dest = v_src + gpGlobals->v_forward * 30;
			
			UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters,
				pEdict->v.pContainingEntity, &tr);
			
			if (tr.flFraction < 1.0)  // hit something?
			{
				if (FStrEq("func_wall", STRING(tr.pHit->v.classname)))
				{
					// square up to the wall...
					view_angles = UTIL_VecToAngles(tr.vecPlaneNormal);
					
					// Normal comes OUT from wall, so flip it around...
					view_angles.y += 180;
					
					if (view_angles.y > 180)
						view_angles.y -= 360;
					
					pEdict->v.ideal_yaw = view_angles.y;
					
					FixIdealYaw();
					
					done = TRUE;
				}
			}
			else
			{
				// try looking in the other direction (forward - angle)
				view_angles = pEdict->v.v_angle;
				view_angles.y = pEdict->v.v_angle.y - angle;
				
				if (view_angles.y < 0.0)
					view_angles.y += 360.0;
				if (view_angles.y > 360.0)
					view_angles.y -= 360.0;
				
				UTIL_MakeVectors( view_angles );
				
				v_src = pEdict->v.origin + pEdict->v.view_ofs;
				v_dest = v_src + gpGlobals->v_forward * 30;
				
				UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters,
					pEdict->v.pContainingEntity, &tr);
				
				if (tr.flFraction < 1.0)  // hit something?
				{
					if (FStrEq("func_wall", STRING(tr.pHit->v.classname)))
					{
						// square up to the wall...
						view_angles = UTIL_VecToAngles(tr.vecPlaneNormal);
						
						// Normal comes OUT from wall, so flip it around...
						view_angles.y += 180;
						
						if (view_angles.y > 180)
							view_angles.y -= 360;
						
						pEdict->v.ideal_yaw = view_angles.y;
						
						FixIdealYaw();
						
						done = TRUE;
					}
				}
			}
			
			angle += 10;
		}
		
		if (!done)  // if didn't find a wall, just reset ideal_yaw...
		{
			// set ideal_yaw to current yaw (so bot won't keep turning)
			pEdict->v.ideal_yaw = pEdict->v.v_angle.y;
			
			FixIdealYaw();
		}
	}
	
	// added by @$3.1415rin - to be perpendicular to the middle of the ladder
	if(pELadder){
		//DEBUG_CLIENTCOMMAND(pEdict,"say ladderstuffnew");
		
		//BotAdjustLadder();
		/*Vector ladderLeft = pELadder->v.absmin;
		Vector ladderRight = pELadder->v.absmax;
		Vector LadderMiddle = ladderLeft + ladderRight / 2.0f;
		float fLength = fabs(ladderLeft.z - ladderRight.z);
		if(fLength>100){			// don't do this on short ladders
		ladderLeft.z = ladderRight.z;
		Vector ladderSide = ladderLeft - ladderRight;
		//Vector normal = CrossProduct( ladderSide, Vector(0,0,1) );
		//DEBUG_CLIENTCOMMAND(pEdict,"say --%.2f - %.2f",fLength,fabs(LadderMiddle.z-pEdict->v.origin.z));
		
		  if(fabs(pELadder->v.absmin.z-pEdict->v.origin.z) > 30
		  &&fabs(pELadder->v.absmax.z-pEdict->v.origin.z) > 30){	// don't do this @ the end of the ladder				
		  LadderMiddle.z = pEdict->v.origin.z;
		  HeadToward(pEdict,LadderMiddle);
		  }
	}*/
	}
	
	// moves the bot up or down a ladder.  if the bot can't move
	// (i.e. get's stuck with someone else on ladder), the bot will
	// change directions and go the other way on the ladder.
	
	if (iOnLadder == LADDER_UP)  // is the bot currently going up?
	{
		// check if bot's current view is free, and if he's on top of the ladder. if it's not free, jump
		if(pELadder->v.absmax.z < pEdict->v.origin.z){	// at top
			Vector	VStart,					// start for all traces
				VEndMiddle;				// straight ahead
			double dOAngle = pEdict->v.angles.y * M_PI / 180.0;
			TraceResult tr;
			
			VStart = pEdict->v.origin + pEdict->v.view_ofs;	// position of bot's eyes
			
			VEndMiddle.z = VStart.z;
			
			VEndMiddle.x = 100.0 * cos(dOAngle) + VStart.x;
			VEndMiddle.y = 100.0 * sin(dOAngle) + VStart.y;
			
			UTIL_TraceLine(VStart, VEndMiddle, dont_ignore_monsters,pEdict->v.pContainingEntity, &tr);
			if(tr.flFraction*100.0 < 20){	// if there's a wall nearby
				Jump();
				//DEBUG_CLIENTCOMMAND(pEdict,"say getting off top");
			}
		}
		// end check
		pEdict->v.v_angle.x = UPLOOK;  // look upwards
		
		// check if the bot hasn't moved much since the last location...
		if ((moved_distance <= .5) && (prev_speed >= 2.0))
		{
			// the bot must be stuck, change directions...

			pEdict->v.v_angle.x = DOWNLOOK;  // look downwards
			iOnLadder = LADDER_DOWN;
		}

		if(i_CurrWP != -1){
			if(waypoints[i_CurrWP].flags&W_FL_LADDER){
			}
			else{
				HeadToward(waypoints[i_CurrWP].origin);
			}
		}
	}
	else if (iOnLadder == LADDER_DOWN)  // is the bot currently going down?
	{
		pEdict->v.v_angle.x = DOWNLOOK;  // look downwards
		
		// check if near to ground
		TraceResult tr;
		UTIL_TraceLine(pEdict->v.origin,pEdict->v.origin+Vector(0,0,-100),dont_ignore_monsters,pEdict,&tr);
		if(tr.flFraction * 100.0 < 50.0f){
			Jump();
			//DEBUG_CLIENTCOMMAND(pEdict,"say getting off");
		}
		
		// check if the bot hasn't moved much since the last location...
		if ((moved_distance <= .5) && (prev_speed >= 2.0))
		{
			// the bot must be stuck, change directions...
			
			pEdict->v.v_angle.x = UPLOOK;  // look upwards
			iOnLadder = LADDER_UP;
		}
		if(i_CurrWP != -1){
			if(waypoints[i_CurrWP].flags&W_FL_LADDER){
			}
			else{
				HeadToward(waypoints[i_CurrWP].origin);
			}
		}
	}
	else  // the bot hasn't picked a direction yet, try going up...
	{
		if(num_waypoints){
			pEdict->v.v_angle.x = UPLOOK;  // look upwards
			iOnLadder = LADDER_UP;
			int iNext = WaypointRouteFromTo(WaypointFindNearest(pEdict,500,bot_teamnm),iGoal,bot_teamnm);
			if(iNext > num_waypoints || iNext == WAYPOINT_UNREACHABLE)
				iNext = i_CurrWP;
			
			if(waypoints[iNext].origin.z < pEdict->v.origin.z - 40){
				pEdict->v.v_angle.x = DOWNLOOK;  // look down
				iOnLadder = LADDER_DOWN;
			}
			else{
				pEdict->v.v_angle.x = UPLOOK;  // look upwards
				iOnLadder = LADDER_UP;
			}
		}
		else{
			pEdict->v.v_angle.x = UPLOOK;  // look upwards
			iOnLadder = LADDER_UP;
		}
	}
	
	// move forward (i.e. in the direction the bot is looking, up or down)
	pEdict->v.button |= IN_FORWARD;
}

bool CBotBase :: DecideOnWay(void){
	iFarGoal = iGoal = -1;
	return false;
}

long CBotBase :: WeaponClass2ID(const char *szClassname){
	long lschl;
	for(lschl=0;lschl < 32;lschl ++){
		if(FStrEq(szClassname,weapon_defs[lschl].szClassname)){
			return lschl;
		}
	}
	return -1;
}

void CBotBase :: ResetWPlanning(void){
	//cout << "ResetWPlanning" << endl;
	iFarGoal = -1;
	iGoal = -1;
}

bool CBotBase :: Camp(void){
	int iNearWP;
	
	f_Camp = gpGlobals->time;

	if(g_b5th){
		iNearWP = WaypointFindNearest(pEdict,1000,bot_teamnm);
		if((iNearWP != -1 && num_waypoints&&waypoints[iNearWP].flags & W_FL_CROUCH)
			|| (Task.current && Task.current->lType&BT_CROUCH)){
			f_ducktill=gpGlobals->time + .3;
		}
		
		if ((pEdict->v.waterlevel == 2) ||
			(pEdict->v.waterlevel == 3)){
			Task.RemoveT(BT_CAMP);
			return false;
		}
		if(f_RWKnife>gpGlobals->time){
			f_RWKnife = gpGlobals->time - .01;
		}
		// dont camp with secondary or grenade when having a primary
		if(IsCWeaponPrimary() != HasPrimary()&&!GOrder.lTypeoG){
			Change2Weapon(HasPrimary());
		}
		if(f_UsedRadio < gpGlobals->time - 2.0){
			if(Ordered.lAction & BO_AFF
				&& CVAR_BOOL(jb_msgradio)){
				SendRadioCommand(RADIO_YOU_TAKE_THE_POINT);
				Ordered.lAction=0;
			}
		}
		if (g_iMapType == MT_AS
			&&g_pVIP){		// stop camping on as maps as CT when the VIP is far away or not visible
			if(Task.current && Task.current->lType & BT_IGNOREENEMY){
				if(bot_teamnm == CS_TEAM_CT){
					if((g_pVIP->v.origin-pEdict->v.origin).Length() > 4000
						&& !FVisible(g_pVIP->v.origin,pEdict)){
						Task.RemoveT(BT_CAMP);		// stop camping
					}
				}
				else{
					if((g_pVIP->v.origin-pEdict->v.origin).Length() > 5000
						&& !FVisible(g_pVIP->v.origin,pEdict)){
						Task.RemoveT(BT_CAMP);		// stop camping
					}
				}
			}
			else{
				if((g_pVIP->v.origin-pEdict->v.origin).Length() > 300
					&& !FVisible(g_pVIP->v.origin,pEdict)){
					Task.RemoveT(BT_CAMP);		// stop camping
					Task.RemoveT(BT_HIDE);		// stop hiding
				}
			}
		}
	}
	f_move_speed = 0;
	f_strafe = 0;
	f_noCamp = gpGlobals->time + 15.0f;	// don't go to a sniper point for 10 s to get away
	f_dont_avoid = gpGlobals->time +.5f;			// just stay where u are
	f_dont_check_stuck = gpGlobals->time +.5f;		// stuck is wanted !
	if(f_SuspRem > gpGlobals->time - 1.0f){		// is less than 1 sec ago ...
		// look there ...
		HeadToward(VSuspEn);
		VRunningTo = VSuspEn;
		return true;		// that's all
	}
	else{												// susp sound is 1-2 sec ago ...
		// look if a viewangle change is neccessary
		if(Task.current&&Task.current->lType & BT_HIDE){		// if hiding, just look sometimes to suspen, normally to last position of enemy
			HeadToward(LastEnemyOrigin);
			VRunningTo = LastEnemyOrigin;
		}
		else{
			if(WLookTo.iNum){
				if(f_CaLooktoWP < gpGlobals->time){
					if(fabs(sLookToChange) != 1){
						DEBUG_CLIENTCOMMAND(pEdict,"say asd---");
					}
					int iNewCWP = WLookTo.FindItem(i_CurrWP);
					if(iNewCWP == -1){
						// not in way ...
						i_CurrWP = WLookTo.iIndices[0];
					}
					else{
						iNewCWP += sLookToChange;
						if(iNewCWP < WLookTo.iNum&&iNewCWP >= 0){	// no "overflow"
						}
						else{
							sLookToChange = -sLookToChange;
							iNewCWP += sLookToChange;
							if(iNewCWP < WLookTo.iNum&&iNewCWP >= 0){
								iNewCWP = WLookTo.iNum/2;
							}
						}
						i_CurrWP = WLookTo.iIndices[iNewCWP];
					}
					f_CaLooktoWP = gpGlobals->time + double(SNIPER_ROTATE_TIME)/double(WLookTo.iNum);
					if(f_RWKnife > gpGlobals->time)
						f_RWKnife = gpGlobals->time;		// dont camp with a knife
				}
				HeadToward(waypoints[i_CurrWP].origin);
			}
			else{		// no wp to look @ ...
				// just turn ...
				pEdict->v.ideal_yaw = (gpGlobals->time / SNIPER_ROTATE_TIME - float(long(gpGlobals->time / SNIPER_ROTATE_TIME))) * 360;			// + doesn't mean clockwise - rotate one time in SNIPER_ROTATE_TIME s
				FixIdealYaw ();
			}
		}
		return true;
	}
}

bool CBotBase :: CheckGrenadeThrowing(void){
	return false;
}

void CBotBase :: GrenadeThrown(void){
}

void CBotBase :: HandleGOrder(void){
	edict_t *pent;
	if(GOrder.lTypeoG){
		Vector VLDiff,VRDiff;
		if(!FNullEnt(GOrder.pAim)){
			GOrder.VAim = GOrder.pAim->v.origin;
		}
		else{
			GOrder.pAim = 0;
		}
		if(GOrder.lState){
			if(GOrder.fStart < gpGlobals->time - 5){
				GOrder.lState = GO_QUIT;
			}
		}
		switch(GOrder.lState){
		case 0:
		case GO_START:		//1
			GOrder.fStart = gpGlobals->time;
			f_Pause = gpGlobals->time + 1.f;
			lButton &=~ IN_ATTACK;
			GOrder.lState ++;
		case GO_SELECT:		//2
			lButton &=~ IN_ATTACK;
			if(GOrder.lTypeoG
				&& (bot_weapons & (1<<GOrder.lTypeoG))){
				FakeClientCommand(pEdict,"%s",weapon_defs[GOrder.lTypeoG].szClassname);
				f_DenyWChange = gpGlobals->time + 3.0;
				GOrder.lState ++;
			}
			else{
				// just skip
				GOrder.lState = GO_QUIT;
			}
			break;
		case GO_WAITFS:		//3
			lButton &=~ IN_ATTACK;
			//cout << "grenade state 2" << endl;
			if(IsCWeaponGrenade() == GOrder.lTypeoG){
				f_LookTo = gpGlobals->time + 3.0;
				//f_GoBack =
				VLookTo = GOrder.VAim;
				GOrder.fStart = gpGlobals->time;
				lButton |= IN_ATTACK;
				GOrder.lState ++;
			}
			else{
				GOrder.lState = GO_SELECT;
			}
			break;
		case GO_PREAIM:
			lButton &=~ IN_ATTACK;
			if(gpGlobals->time - GOrder.fStart > .0)
				GOrder.lState ++;
			break;
		case GO_AIM:			//4
			//cout << "grenade state 3" << endl;
			lButton |= IN_ATTACK;
			
			if(f_LookTo - .5 < gpGlobals->time){
				//cout << "grenade state 3 - leaving cause of noaim" << endl;
				// just skip
				GOrder.lState = GO_QUIT;
			}
			
			UTIL_MakeVectors(pEdict->v.angles);
			VLDiff = gpGlobals->v_forward;
			VLDiff.z = 0;
			VLDiff = VLDiff.Normalize();
			
			VRDiff = GOrder.VAim - pEdict->v.origin;
			VRDiff.z = 0.0;
			VRDiff = VRDiff.Normalize();
			
			if(DotProduct(VLDiff,VRDiff) > .9f
				&&gpGlobals->time - GOrder.fStart > .4){
				GOrder.lState ++;
			}
			
			if(gpGlobals->time - GOrder.fStart > 2){
				// just skip
				GOrder.lState = GO_QUIT;
			}
			break;
		case GO_RELEASE1:{		//5
			Vector v_nvelocity = g_vecZero;
			if(FVisible(GOrder.VAim,pEdict)&&RANDOM_LONG(0,100) < 75){
				float fDistance = (GOrder.VAim - pEdict->v.origin).Length();
				
				if(fDistance>800)fDistance=800;
				else if(fDistance<200)fDistance=200;
				
				v_nvelocity = VecCheckThrow(&pEdict->v,pEdict->v.origin,GOrder.VAim,fDistance,g_fGravityAdj);
			}
			else{
				v_nvelocity = VecCheckToss(&pEdict->v,pEdict->v.origin,GOrder.VAim,g_fGravityAdj);
			}
			if(v_nvelocity.Length() < 10){
				GOrder.lState = GO_QUIT;
			}
			else{
				lButton &=~ IN_ATTACK;
				GOrder.lState ++;
			}
			break;
						 }
		case GO_RELEASE2:		//6
			lButton &=~ IN_ATTACK;
			GOrder.lState ++;
			break;
		case GO_FINDGREN:				// 7
			if(pEdict->v.button&IN_ATTACK)			//hackhack
				lButton &=~IN_ATTACK;
			else
				lButton |=IN_ATTACK;
			pent = 0;
			while(pent = UTIL_FindEntityByClassname(pent,"grenade")){
				if(pent->v.owner){
					if(pent->v.owner == pEdict){
						DEBUG_DRAWBEAM(listenserver_edict,GOrder.VAim,pent->v.origin,15,10,200,200,0,200,10);
						if(listenserver_edict){
							DEBUG_DRAWBEAM(listenserver_edict,listenserver_edict->v.origin,pent->v.origin,10,0,200,200,200,200,10);
						}
						DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pent->v.origin,10,0,0,0,200,200,10);
						
						// set the wanted velocity of the grenade :)
						Vector v_nvelocity;
						if(FVisible(GOrder.VAim,pEdict)&&RANDOM_LONG(0,100) < 75){
							float fDistance = (GOrder.VAim - pEdict->v.origin).Length();
							
							if(fDistance>700)fDistance=700;
							else if(fDistance<200)fDistance=200;
							
							v_nvelocity = VecCheckThrow(&pent->v,pent->v.origin,GOrder.VAim,fDistance,g_fGravityAdj);
						}
						else{
							v_nvelocity = VecCheckToss(&pent->v,pent->v.origin,GOrder.VAim,g_fGravityAdj);
						}
						if(v_nvelocity.Length() > 10){
							pent->v.velocity = v_nvelocity;
							GrenadeThrown();
							DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin+v_nvelocity,10,0,0,200,0,200,10);
						}
						GOrder.lState = GO_QUIT;
						break;
					}
				}
			}
			break;
		case GO_QUIT:
		default:
			GOrder.lState = 0;
			GOrder.lTypeoG = 0;
			f_DenyWChange = 0;
			if(HasPrimary()){
				Change2Weapon(HasPrimary());
			}
			else if(HasSecondary()){
				Change2Weapon(HasSecondary());
			}
			else Change2Weapon(HasKnife());
			break;
		}
	}
}

void CBotBase :: HandleReplay(void){
	if(bReplay){
		float fOffset = gpGlobals->time - fStartADVM;
		int i = 0,i2 = 0;
		float fTimeDiff;
		float fPart;
#ifdef _DEBUG
		FILE *fhds;fhds=fopen("rec_move_a.txt","a");
		if(fhds){
			fprintf(fhds,"%f - %i\n",fOffset,pWPAMPlay->iNum);
			fclose(fhds);
		}
#endif
		
		while(i < pWPAMPlay->iNum){
			if(pWPAMPlay->Rec[i].fTime > fOffset){		// i.e. that's the last recorded item
				if(i!=0){
					i2 = i;
					i --;
				}
				fTimeDiff = pWPAMPlay->Rec[i2].fTime - pWPAMPlay->Rec[i].fTime;
				fPart = (fOffset - pWPAMPlay->Rec[i].fTime) / fTimeDiff;

				// copy stuff from recorded data to bot
				if(pWPAMPlay->Rec[i].lButton&IN_DUCK){
					//DEBUG_CLIENTCOMMAND(pEdict,"say duckin from advm");
				}
				//DEBUG_CLIENTCOMMAND(pEdict,"say -");
				lButton = pWPAMPlay->Rec[i].lButton;
				if(pWPAMPlay->Rec[i].v_origin.Length() < .1 || pWPAMPlay->Rec[i2].v_origin.Length() < .1)
					break;
				//pEdict->v.origin = pWPAMPlay->Rec[i].v_origin + fPart*(pWPAMPlay->Rec[i2].v_origin-pWPAMPlay->Rec[i].v_origin);			// rewrite this fuckin' origin stuff
				//Vector VIdeal = pWPAMPlay->Rec[i].v_origin + fPart*(pWPAMPlay->Rec[i2].v_origin-pWPAMPlay->Rec[i].v_origin);
				pEdict->v.angles = pWPAMPlay->Rec[i].v_angles;
				pEdict->v.v_angle = pWPAMPlay->Rec[i].v_v_angle;
				//pEdict->v.velocity = pWPAMPlay->Rec[i].v_velocity + fPart*(pWPAMPlay->Rec[i2].v_velocity-pWPAMPlay->Rec[i].v_velocity);
				pEdict->v.velocity = (pWPAMPlay->Rec[i].v_origin-pEdict->v.origin).Normalize() * ( pWPAMPlay->Rec[i].v_velocity.Length()/* + (VIdeal - pEdict->v.origin).Length()*.7*/);
				f_move_speed = pEdict->v.velocity.Length();
				
				// set ideal values to this ones ( instant turn vice versa )
				fIdealAngleYaw = pEdict->v.angles.y;
				fIdealAnglePitch = pEdict->v.angles.x;
				pEdict->v.ideal_yaw = pEdict->v.v_angle.y;
				pEdict->v.idealpitch = pEdict->v.v_angle.x;
				fViewSpeedYaw=0;
				fViewSpeedPitch = 0;
				fAngleSpeedPitch = 0;
				fAngleSpeedYaw = 0;
				
				if(lButton & IN_MOVELEFT){
					f_strafe = -200;
				}
				else if(lButton & IN_MOVERIGHT){
					f_strafe = 200;
				}
#ifdef _DEBUG
				FILE *fhds;fhds=fopen("rec_move.txt","a");
				if(fhds){fprintf(fhds,"%i - %f - %f\n",\
					pWPAMPlay->iNum,fOffset,pWPAMPlay->Rec[i].fTime);fclose(fhds);}
#endif
				break;
			}
			i++;
		}
		if(i >= pWPAMPlay->iNum){
			//DEBUG_CLIENTCOMMAND(pEdict,"say stoppin advm");
			/*if((pWPAMPlay->Rec[pWPAMPlay->iNum].v_origin - pEdict->v.origin).Length()>50.f){
				DEBUG_CLIENTCOMMAND(pEdict,"say fuck, hasnt worked this time neither");
				ResetWPlanning();
				f_Pause = gpGlobals->time + .6f;
			}*/
			bReplay = false;
		}
	}
}

bool CBotBase :: IsWeaponBetterCurrent(long lWeaponID){
	return false;
}

int CBotBase :: GoNextWPOnPath(void){
	return WAYPOINT_UNREACHABLE;
}

int CBotBase :: GetNextWPOnPath(void){
	return WAYPOINT_UNREACHABLE;
}
