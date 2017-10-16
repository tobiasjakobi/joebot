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

#include "CBotDOD.h"

#include "bot.h"
#include "bot_modid.h"
#include "Commandfunc.h"
#include "globalvars.h"
#include "NNWeapon.h"

#include "NeuralNet.h"

void CBotDOD :: Fight(void){
	DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin + Vector(0,0,50),10,0,200,000,000,200,10);
	bool bDefensive = false;
	edict_t *pFlashbang=0;
	Vector v_enemy = pBotEnemy->v.origin - pEdict->v.origin;	// difference vector from en and bot
	fEnemyDist = v_enemy.Length();
	
	bool bMeIVC = FInViewCone(&(pEdict->v.origin),pBotEnemy);
	
	if(!(Action.lAction & BA_FIGHT)		// check if it's a new fight session
		&&f_LastFight + 2 < gpGlobals->time){	// don't allow delay in fight, when en just ran out of view
		f_Delay = gpGlobals->time + f_ReactionTime;	
	}
	
	if(pBotEnemy != pBotOldEnemy){		// enemy changed
		int index = UTIL_ClientIndex(pBotEnemy);
		if(index!=-1){
			if(f_ES[index] + _ESTIME < gpGlobals->time){		// i.e. not seen for a while
				if(CVAR_BOOL(jb_msgradio))
					SendRadioCommand(RADIO_ENEMY_SPOTTED);
				int iETeam = UTIL_GetTeam(pBotEnemy);

				for (int i = 0; i < gpGlobals->maxClients; i++){
					edict_t *pEnt = INDEXENT(i + 1);
					if (!FNullEnt(pEnt) && (!pEnt->free)){
						if(iETeam == UTIL_GetTeam(pEnt)){				// set time back for every player of other team
							f_ES[i] = gpGlobals->time - _ESTIME /3.0;		// don't say es too often, when groups hit each other
						}
					}
				}
			}
			f_ES[index] = gpGlobals->time;
		}
	}
	
	// del old ways
	//iGoal = -1;
	
	// till 'not going back to sniper point bug' not fixed ...
	//Task.RemoveT(BT_CAMP);
	f_noCamp = 0;
	if(!GOrder.lTypeoG)
		f_LookTo = 0;
	
	// don't continue snipering when ya've no real sniper weapon
	if(HasSniper()){
		if(Task.SearchT(BT_CAMP) != -1){		// dont move at sniper spot
			f_move_speed = 0;
		}
		else{						// slow down with sniper weapon, to get a better accuracy
			f_move_speed /= 1.5f;
		}
	}
	
	if(f_NNNUpdate < gpGlobals->time){
		//Vector bot_angles = UTIL_VecToAngles( v_enemy );	// convert ya absolute dimensions to angles
		//bot_angles.x>180.0?bot_angles.x -=360.0:1==1;		// just make it to[-180;180]
		
		f_Situation =UTIL_FightingAgainst(pEdict,bot_teamnm,&pNEn) -
			UTIL_FightingAgainst(pBotEnemy,UTIL_GetTeam(pBotEnemy),&pNTeam)
			+ d_Manner
			+ f_Aggressivity;
		
		lEnWeapon = WeaponModel2ID(STRING(pBotEnemy->v.weaponmodel));
		
		/* Get Data for NN*/
		dCombatNNIn[IWeapon]		= WeaponDefs.dpWeaV[CSTRIKE_DLL][current_weapon.iId];
		dCombatNNIn[IHealth]		= double(bot_health)/50.0-1.0;
		dCombatNNIn[IDistance]		= ConvertDistance(fEnemyDist);
		dCombatNNIn[IEWeapon]		= WeaponDefs.dpWeaV[CSTRIKE_DLL][lEnWeapon]/*bot_angles.x/180.0*/;
		dCombatNNIn[IAmmo]			= (double(current_weapon.iClip) / double(WeaponDefs.ipClipSize[DOD_DLL][current_weapon.iId]))*2.0-1.0;
		dCombatNNIn[ISituation]		= tanh(f_Situation/2.f);
		
		if (lEnWeapon==-1){
			dCombatNNIn[IEWeapon]	= 0;
			FakeClientCommand(pEdict,"say %s",STRING(pBotEnemy->v.weaponmodel));
		}
		
		NNCombat->SetInput(dCombatNNIn);
		NNCombat->Propagate();
		NNCombat->GetOutput(dCombatNNOut);	// copy the outputs of the output layers to this field
		
		f_NNNUpdate = gpGlobals->time + (1.0f / jb_nnupdaterate->value);
		// collect data for SOM
		/*if(dCombatNNIn[IAmmo] <= 1
		&& dCombatNNIn[IAmmo] >= -1){
		SP.AddPattern(dCombatNNIn);
	}*/
		
		// log nn vars for reinforcement learning
		/*memcpy(sbNNLog.dppNet[_LOGNETINPUT][sbNNLog.lAct],dCombatNNIn,sizeof(double)*IEND);
		memcpy(sbNNLog.dppNet[_LOGNETOUTPUT][sbNNLog.lAct],dCombatNNOut,sizeof(double)*OEND);
		sbNNLog.lNum ++;
		sbNNLog.lAct ++;
		if(sbNNLog.lAct >= _MAXNNLOG){
		sbNNLog.lAct = 0;
	}*/		
		
		// don't crouch long after hiding - no other functions use ducktill for such long time ...
		if(f_ducktill > gpGlobals->time + 1.0f
			&& f_ducktill < gpGlobals->time + _HIDECAMPTIME -1.0f){
			f_ducktill = 0;
		}
		
		// don't check stuck for some time
		f_dont_check_stuck = gpGlobals->time + .5;
		
		if(fEnemyDist>150.0&&fEnemyDist<1400.0
			&& bot_weapons & (1<<(DOD_WEAPON_HANDGRENADE|DOD_WEAPON_STICKGRENADE))
			&& GOrder.fStart - gpGlobals->time > 5.f
			//&& ( !bMeIVC )){
			&& (pNTeam != pEdict || !bMeIVC )){
			//Change2Weapon(CS_WEAPON_HEGRENADE);
			if(!GOrder.lTypeoG){
				if(bot_weapons & DOD_WEAPON_HANDGRENADE){
					GOrder.Attack(pBotEnemy->v.origin,DOD_WEAPON_HANDGRENADE);
				}else{
					GOrder.Attack(pBotEnemy->v.origin,DOD_WEAPON_STICKGRENADE);
				}
				GOrder.pAim = pBotEnemy;
				f_DenyWChange = gpGlobals -> time + 1.0f;
			}
		}
		
		// check ammo
		if(!IsCWeaponGrenade()){		// don't check ammo with grenades - lol
			if((m_rgAmmo[current_weapon.iAmmo1]+current_weapon.iClip) == 0){// if ya clip  and ya reserve is empty
				ChangeToLWeapon();
				if(f_UsedRadio < gpGlobals->time - _RADIO_FREQ
					&&CVAR_BOOL(jb_msgradio)){
					SendRadioCommand(RADIO_NEED_BACKUP);
				}
			}
			else if(current_weapon.iClip == 0){		// only ya clip is empty, so you can propably reload, if the en is far away
				ChangeToLWeapon();							// use another weapon, worst case the knife - no special decisions concerning distance of en
				if(f_UsedRadio < gpGlobals->time - _RADIO_FREQ
					&& CVAR_BOOL(jb_msgradio)){
					SendRadioCommand(RADIO_NEED_BACKUP);
				}
			}
		}
		else{		// got grenade
			if(fEnemyDist < 150){
				Change2Weapon(HasSecondary());
			}
		}
		}
		// ODuck
		if(dCombatNNOut[ODuck] > 0.5){
			//lButton |= IN_DUCK;
			f_ducktill = gpGlobals->time +.5;
		}
		// OJump
		//cout << dCombatNNOut[OJump] << endl;
		if(dCombatNNOut[OJump] > 0.75){
			//DEBUG_CLIENTCOMMAND(pEdict,"say NNJUMP");
			if(!(pEdict->v.button & IN_JUMP)
				&& RANDOM_LONG(0,100) > 50
				&& f_move_speed > 1.0)
				lButton |= IN_JUMP;
		}
		
		// OHide
		if(dCombatNNOut[OHide] > .65
			|| IsCWeaponKnife()){
			bDefensive=true;
		}
		
		// OMoveType
		if(!IsCWeaponKnife()){
			if(dCombatNNOut[OMoveType] > 0.5){
				// do nothing cause default is max speed
			}
			if(dCombatNNOut[OMoveType] < 0.5
				&&dCombatNNOut[OMoveType] > -0.5){
				// walk ??
			}
			if(dCombatNNOut[OMoveType] <-0.5){
				f_move_speed = 0;
			}
		}
		
		// OStrafe
		if(dCombatNNOut[OStrafe] > 0.5){
			f_strafe += _MAXSTRAFE;
		}
		if(dCombatNNOut[OStrafe] <-0.5){
			f_strafe -= _MAXSTRAFE;
		}
		
		// ct bots shouldn't hide when the VIP is near to them
		if(bDefensive && g_iMapType == MT_AS){
			if(g_pVIP){
				if((pEdict->v.origin-g_pVIP->v.origin).Length() < 400.0)
					bDefensive = false;
			}
		}
		
		if (pEdict->v.movetype == MOVETYPE_FLY)		// if on ladder, try to get away from there
			bDefensive = true;
		
		// handle stuff with runnign with knife
		if(f_RWKnife > gpGlobals->time){
			if(!IsCWeaponPrimary() && !IsCWeaponSecondary()){
				if(HasPrimary()){
					Change2Weapon(HasPrimary());
					bDefensive = false;
					if(f_RWKnife > gpGlobals->time){
						f_RWKnife = gpGlobals->time-1.1;
					}
				}
				else if(HasSecondary()){
					Change2Weapon(HasSecondary());
					bDefensive = false;
					if(f_RWKnife > gpGlobals->time){
						f_RWKnife = gpGlobals->time-1.1;
					}
				}
			}
		}
		
		// check if bot's trying to get to a safer or better location
		//if(f_GotoHidingPlace > gpGlobals->time){
		if(Task.current&&Task.current->lType & BT_HIDE){
			// copy point to edict hiding from to bot_t.pHidefrom;
			//if(pEdictPlayer)WaypointDrawBeam(pEdictPlayer,pEdict->v.origin,VHidingPlace,100,10,255,255,255,100,50);
			//DEBUG_CLIENTCOMMAND(pEdict,"say hiding ...");
			// sometimes jump
			if(RANDOM_LONG(0,100) < 8
				&&moved_distance > 8.0f
				&&!(Action.lAction & BA_TKAVOID) ){		// only jump when avoiding tks when nn wants it
				lButton |= IN_JUMP;
			}
			GotoHidingPlace();
		}
		else{
			// bot's not trying to reach a safe place - but just test now if it would be better if he does ...
			//bDefensive = BotCheckDefensive(pBot);
			if(bDefensive											// is it important to get into a more defensive mode ?
				|| pFlashbang
				//&& f_Hide < gpGlobals->time - 5.0f	// don't play hide and seek for hours
				/*&&f_GotoHidingPlace > gpGlobals->time+.5*/){	// just wait one sec between two hidings to shoot shortly at the enemy
				edict_t *pHidefrom;
			/*if(!pFlashbang)*/
			pHidefrom = pBotEnemy;
			/*else
			pHidefrom = pFlashbang;*/
			int iDest;
			if((iDest=SearchHidingWP(pHidefrom))!=-1){	// then search for a good place
				// found a nice place to camp for a while ...
				// set camp time based on current nn output
				f_HidingTime = (dCombatNNOut[OHide]-.25) * 1.6 * _HIDECAMPTIME;
				//					f_GotoHidingPlace = gpGlobals->time + 2.0;
				//Task.AddTask(BT_HIDE, gpGlobals->time + 2.0, 0, pBotEnemy,0);
				if(Task.current&&!(Task.current->lType & BT_HIDE)){
					if(dCombatNNIn[IAmmo] < -.5f
						&&Task.SearchT(BT_RELOAD) == -1){
						Task.AddTask(BT_RELOAD,gpGlobals->time + 30.f,0,0,0);
					}
					Task.AddTask(BT_GOTO|BT_CAMPATGOAL|BT_IGNOREENEMY|BT_HIDE|BT_LOCKED, gpGlobals->time + 10.0, iDest, pBotEnemy,4);
					ResetWPlanning();
				}
				//					pHidefrom = pBotEnemy;
				//DEBUG_CLIENTCOMMAND(pEdict,"say hiding cause of shitty circumstances ....");
			}
			}
		}
		
		/*if(pEdict->v.punchangle.Length() > 3.5){		//  test the amount of recoil
		f_PauseShoot = gpGlobals->time + WeaponDefs.dpWeaponPauseTime[current_weapon.iId];
		}*/
		
		if(!bMeIVC// enemy doesn't see current bot
			//&& !IsCWeaponSniper(pBot)						// don't try to hide with sniper weapon
			//&& bDefensive									// bot's defensive
			&& dCombatNNIn[IDistance]>0.0				// when near, attack in evry case
			&& f_AimHead < .9							// begin to shoot when accurate
			&& !IsCWeaponGrenade()){					// don't wait throwing grenades
			f_Delay = gpGlobals->time +.01;		// don't shoot this time ...
			
			/*float ffps = 1000.0 / msecval;
			if(ffps > 60)
				ffps = 60;*/
			//f_AimHead = pow(f_AimHead,dsqrt(ffps/75.0));
			if(f_AimHead < 1.0){
				f_AimHead += gpGlobals->frametime;
			}
			f_move_speed /= 2.0f;						// slow down to be less obvious ...
			//DEBUG_CLIENTCOMMAND(pEdict,"say aiming");
		}
		else
			if(bMeIVC){
				f_AimHead -= g_msecval*.004 * f_AimHead;
			}
			
			// Set variable for knowing that last call was fight
			Action.lAction |= BA_FIGHT;
			
			// shoot at aim
			if(CVAR_BOOL(jb_shoot)){		// the friendly mode
				if(IsCWeaponKnife()){
					if(dCombatNNIn[IDistance] < -.5){
						ShootAtEnemy();			// attack enemy with knife only when near
						// Set time of last fight
						f_LastFight = gpGlobals->time;
					}	
				}
				else{
					if(!PauseShoot()){			// look at ya type of weapon and decide to pause sometimes
						f_Pause = gpGlobals->time + .5;
						ShootAtEnemy();			// attack enemy
						
						// Set time of last fight
						f_LastFight = gpGlobals->time;
					}
					else{
						//DEBUG_CLIENTCOMMAND(pEdict,"say Paused weapon");		// sometimes the bots don't shoot after a map change - Why ??
					}
				}
			}
			else{
				//DEBUG_CLIENTCOMMAND(pEdict,"say bots are friendly");
			}
			
			/*FILE *fhd = fopen("punchangle.txt","a");fprintf(fhd,"%s\t%f\n",STRING(pEdict->v.netname),(pEdict->v.punchangle).Length());fclose(fhd);
			if ((pEdict->v.punchangle).Length() > .1)
			DEBUG_CLIENTCOMMAND(pEdict,"say %f",(pEdict->v.punchangle).Length());*/
			/*	}
			catch(...){
			DEBUG_CLIENTCOMMAND(pEdict,"say asdlkjglkjjljkglkjgljk");
			FILE *fhd = fopen("scheisse.txt","a");fprintf(fhd,"scheisse in combat\n");fclose(fhd);
	}*/
}

bool CBotDOD :: FireWeapon( Vector &v_enemy )
{
	// Use the Vector argument of the emeny to aim at the enemy and fire
	// here.  Return TRUE if bot fired weapon (enough ammo, etc.) otherwise
	// return FALSE to indicate failure (maybe bot would runaway then).
	if(f_Delay > gpGlobals->time)
		return false;
	
	// tk avoid
	Vector v_shoot;
	Vector v_gun;
	
	v_gun = GetGunPosition(pEdict);
	
	v_shoot = pEdict->v.angles + pEdict->v.punchangle;
	v_shoot.z = 0;  // reset roll to 0 (straight up and down)
	
	UTIL_MakeVectors( v_shoot );
	
	v_shoot = gpGlobals->v_forward.Normalize() * 1000.0;	// 3000 should be sufficient
	
	TraceResult tr;
	
	UTIL_TraceLine(v_gun,v_gun+v_shoot,dont_ignore_monsters,ignore_glass,pEdict,&tr);
	if(// tr.flFraction < .9
		UTIL_ClientIndex(tr.pHit) != -1
		&& (tr.pHit != pBotEnemy)){// line is probably fract by enemy
		f_strafe += i_avoidstrafe * pEdict->v.maxspeed;
		Action.lAction |= BA_TKAVOID;			// set flag for only short hiding or stop hiding when no teamm8 is in sight
		//DEBUG_CLIENTCOMMAND(pEdict,"say tk avoid");
		return FALSE;
	}
	else{
		if(IsProne(pEdict)){
			f_Pause = gpGlobals->time + .5;
		}
		if(Action.lAction & BA_TKAVOID){	
			// bot's currently avoid a team attack, but now he can shoot without hurting team8s
			Task.RemoveT(BT_HIDE);
			Action.lAction &=~ BA_TKAVOID;
		}
		if(!(pEdict->v.button&IN_ATTACK)){
			lButton |= IN_ATTACK;
		}
		return TRUE;
	}
}
