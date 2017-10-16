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

#include "CBotCS.h"

#include "bot.h"
#include "bot_wpstat.h"
#include "Commandfunc.h"
#include "globalvars.h"

float g_fmin_std_swp = 10;
float g_fmax_std_swp = 30;

void CBotCS :: Goto(Vector &VGoto){
	// should be, that bot goes there via wps, but right now it's just headto
	HeadToward(VGoto);
	VRunningTo = VGoto;
}

bool CBotCS :: GoToSpEnt( void ){
	bool breturn=false;

	if(f_CheckSpEnt > gpGlobals->time){
		return breturn;
	}
	f_CheckSpEnt = gpGlobals->time + 1.0;
	if(m_bIsVIP){
		//DEBUG_CLIENTCOMMAND(pEdict,"say i'm a vip");
		return breturn;
	}
	if(Task.current&&Task.current->lType&BT_IGNOREENEMY){
		//DEBUG_CLIENTCOMMAND(pEdict,"say ignorenemy is there");
		return breturn;
	}
	if(f_LastFight > gpGlobals->time - 1){
		//DEBUG_CLIENTCOMMAND(pEdict,"say last fight is too near");
		return breturn;
	}
	
	breturn = HeadTowardSpEnt();
	if( breturn ){
		f_CheckSpEnt = gpGlobals->time-1.0;
	}
	else{
	}
	
	return breturn;
}

bool CBotCS :: HasAlreadyHostage(const edict_t *pentHostage){
	for(int ischl=0;ischl < m_iHostagesUsed;ischl++){
		if(m_rgpentHostages[ischl] == pentHostage)
			return true;
	}
	return false;
}

bool CBotCS :: HeadTowardSpEnt(void){
	// this function searches the surrounding for interesting ents like hostages
	// or dropped bombs. It returns true, if it has found something and false, if it needs no
	// control. Although bomb targets are ents, they are not handled here, 'cause it's too
	// cpu demanding - that's handled with wp stuff
	
	edict_t *pEnt,
		*pNearP;
	float fDistance;
	float fPDistance,fRDistance;
	char szClassname[100];
	
	if(pEdict->v.movetype == MOVETYPE_FLY)		/// not on ladders
		return false;

	DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin+Vector(0,0,100),5,10,250,250,250,250,50);
	
	pEnt = 0;
	long lNumF=0;
	while(gFlash[lNumF].bUsed
		&&lNumF<_MAXGRENADEREC){
		if(FNullEnt(gFlash[lNumF].p))
			gFlash[lNumF].p = 0;
		if(gFlash[lNumF].p){
			pEnt = gFlash[lNumF].p;
			fDistance = Vector(gFlash[lNumF].VOrigin - pEdict->v.origin).Length();
			//fDistance = Vector(pEnt->v.origin - pEdict->v.origin).Length();
			if(fDistance < 1500
				&& FVisible(gFlash[lNumF].VOrigin,pEdict)){
				if(Task.current&&(Task.current->lType&(BT_HIDE|BT_LOCKED))!=(BT_HIDE|BT_LOCKED)){
					// it's near and visible
					
					Vector v_diff = pEdict->v.origin - gFlash[lNumF].VOrigin;
					Vector bot_angles = UTIL_VecToAngles( v_diff );
					
					pEdict->v.ideal_yaw = bot_angles.y;
					pEdict->v.idealpitch = bot_angles.x;
					
					FixIdealYaw();
					FixIdealPitch();
					DEBUG_CLIENTCOMMAND(pEdict,"say head away");
					return true;
				}
			}
		}
		lNumF++;
	}
	
	pNearP = GetNearestPlayer(pEdict,bot_teamnm,fPDistance,true);
	
	b_neardoor = false;
	
	pEnt = 0;
	while(pEnt = UTIL_FindEntityInSphere(pEnt,pEdict->v.origin,250)){
		fDistance = Vector(pEnt->v.origin - pEdict->v.origin).Length2D();
		strcpy( szClassname , STRING(pEnt->v.classname) );

		if (FStrEq(szClassname,"hostage_entity")){		// hostages
			if(bot_teamnm == CS_TEAM_CT){
				if(fPDistance > 300						// don't 'steal' hostages
					&& GetHosVel(pEnt) < 1.f
					&& m_iHostagesUsed < _MAXHOSTAGES			// don't take more than _MAXHOSTAGES
					&& IsAlive(pEnt)						// fix of rescue point bug ???
					&& FVisible(pEnt->v.origin,pEdict)){	// visible
					
					if (HasAlreadyHostage(pEnt)){	// do not try to take a hostage twice
						continue;
					}
					if(f_gotohostage + 10.0f < gpGlobals->time){	// new try to get hostage
						//DEBUG_CLIENTCOMMAND(pEdict,"say_team got hostage");
						if(bot_teamnm==CS_TEAM_CT)UTIL_HostSay(pEdict,1,"got hostage");
						f_gotohostage = gpGlobals->time;
					}
					
					if(f_gotohostage + 9.0f > gpGlobals->time){	// do not try longer than 9s
						if(/*RANDOM_LONG(0,100) < 10
							||*/f_gotohostage == gpGlobals->time
							||fDistance < 70.0f){
							VATurnTo(pEnt->v.origin + pEnt->v.view_ofs - pEdict->v.origin);
						}
						
						if(fDistance < 60.0f){
							// use hostage
							lButton |= IN_USE;
							
							m_rgpentHostages[m_iHostagesUsed] = pEnt;	// add hostage ent
							m_iHostagesUsed ++;		// inc number of 'used' hostages
							f_gotohostage = 0;						// make this for maing the bot able to go to another hostage
							
							ResetWPlanning();
							Task.RemoveT(BT_GOTO|BT_ROAMTEAM|BT_HUNT);
						}
						return true;
					}
				}
			}
			else{		// TE
				/*if(g_b5th){
					if(fPDistance > 400){
						if(GetHosVel(pEnt) < .1f){
							if(WaypointDistanceFromTo(WaypointFindNearest(pEdict,1000,bot_teamnm),WaypointFindNearestGoal(pEdict->v.origin,pEdict,1000,bot_teamnm,W_FL_HOSTAGE),bot_teamnm)>400){
								if(Task.SearchT(BT_GUARD)==-1){
									if(Task.AddTask(BT_GUARD,gpGlobals->time + 20,0,0,0)){
										// task has been added and is now Task.current
										if(Task.current){
											ResetWPlanning();
											Task.current->VT = pEnt->v.origin;
										}
									}
								}
							}
						}
					}
				}*/
			}
		}
		else if(!strncmp(szClassname,"func_door",9*sizeof(char))){
			b_neardoor = true;
		}
		else if(!strncmp(szClassname,"weaponbox",strlen("weaponbox")) ||
			!strncmp(szClassname,"weapon_shield",strlen("weapon_shield")))
		{
			if (pEnt->v.effects & EF_NODRAW) // spectator or not drawn?
				continue;

			if(FVisible(pEnt->v.origin,pEdict) && !(Action.lAction & BA_DEFUSE)){
				long lWeaponID = WeaponModel2ID(STRING(pEnt->v.model));

				if(lWeaponID == CS_WEAPON_C4){
					if(bot_teamnm == CS_TEAM_TE){		//te
						Goto(pEnt->v.origin);	// problem offset between old owner and current position
						return true;
					}
					else{							// ct
						if(f_noCamp < gpGlobals->time				// is it allowed to sniper
							&& pNearP
							&& fPDistance > 300.0f){					// is there already another ct
							if(f_noCamp < gpGlobals->time){
								if(Task.SearchT(BT_GUARD) == -1){
									Task.AddTask(BT_GUARD,gpGlobals->time + 15.0f,0,pNearP,0);
								}
							}
							return false;
						}
					}
				}
				else{	// other weapons than c4
					if(lWeaponID != -1){	// only 'see' primaries
						/*if(!HasPrimary()){				// bot has no primary weapon
							Goto(pEnt->v.origin);	// go there
							return true;
						}
						else*/{		// only drop and pickup weapons when not fighting ...
							if(f_LastFight+6.0f < gpGlobals->time){
								if(IsWeaponBetterCurrent(lWeaponID)){			// current weapon is not the favourite one
									if(f_RWKnife > gpGlobals->time)
										f_RWKnife = 0;
									if(!(Action.lAction & BA_PICKUP) ||1){		// first frame
										if(IsSecondaryWeapon(1<<lWeaponID)){
											if(HasSecondary() != current_weapon.iId){	// is secondary selected ?? 
												f_DenyWChange = 0;
												Change2Weapon(HasSecondary());		// if not change it
											}
										}
										else if(HasPrimary() != current_weapon.iId){	// is primary selected ?? 
											f_DenyWChange = 0;
											Change2Weapon(HasPrimary());		// if not change it
										}
									}
									Goto(pEnt->v.origin);	// go there
									Action.lAction |= BA_PICKUP;
									if(fDistance < 70.0){
										if( (IsPrimaryWeapon(1<<lWeaponID) && ( HasPrimary() == current_weapon.iId || HasShield() )) ||
											(IsSecondaryWeapon(1<<lWeaponID) && HasSecondary() == current_weapon.iId) )
										{
											FakeClientCommand(pEdict,"drop");
											Action.lAction &=~BA_PICKUP;
										}
										else if(IsShieldWeapon(1<<lWeaponID)){
											Action.lAction &=~BA_PICKUP;
										}
									}
									return true;
								}
							}
						}
					}
				}
			}
		}
		else if(FStrEq(szClassname,"grenade")){
			if(bot_teamnm == CS_TEAM_CT){
				if(FStrEq(STRING(pEnt->v.model),"models/w_c4.mdl")){	// only planted c4
					// is the origin of player 45 units above the ground ??
					//fRDistance = Vector(pEnt->v.origin - (pEdict->v.origin - Vector(0,0,45))).Length();
					fRDistance = fDistance;
					if(fRDistance > _MAXC4DEFDIST
						|| !FVisible(pEnt->v.origin,pEdict))			// if it's not too far and if it's visible
						continue;
					
					f_dont_check_stuck = gpGlobals->time + 1.0f;
					f_dont_avoid = gpGlobals->time + 1.0f;
					
					if(fPDistance < 150.0f
						&&pNearP
						&& pNearP->v.button & IN_USE){
						// guard the defuser
						if(Task.SearchT(BT_GUARD) == -1){
							Task.AddTaskI(BT_GUARD,gpGlobals->time + 15.0,0,pNearP,0);
							Task.current ->VT = pNearP->v.origin;
						}
						continue;
					}
					if(fRDistance > 100){
						Action.lAction&=~BA_DEFUSE;		// reset defusing flag, if too far away
						return false;
					}
					if(!(Action.lAction&BA_DEFUSE)
						&& f_UsedRadio < gpGlobals->time - 3.f){
						SendRadioCommand(RADIO_NEED_BACKUP);
					}
					
					if(fRDistance > 40 && !(Action.lAction&BA_DEFUSE)){	// go near to wp and continue defusing, if began
						Goto(pEnt->v.origin);
						f_move_speed /= 2.0f;
						prev_speed = 0;		// ... to make other routines not to think, that bot is stuck
						//f_dont_avoid = gpGlobals->time +1.0f;
						return true;
					}
					else{
						Goto(pEnt->v.origin);
						pEdict->v.idealpitch = 30.0f;				// look down
						VATurnTo(pEnt->v.origin-pEdict->v.origin);
						iGoal = -1;									// delete old ways
						lButton |= IN_USE;							// defuse
						Action.lAction |= BA_DEFUSE;
						f_ducktill=gpGlobals->time + 1.0;			// duck down
						f_move_speed = 0.0f;						// stay there
						return true;
					}
				}
			}
		}
		else if(FStrEq(szClassname,"item_thighpack")){
			if(bot_teamnm == CS_TEAM_CT
				&& !(Action.lAction & BA_DEFKIT)){
				Goto(pEnt->v.origin);
			}
		}
		else if (FStrEq(szClassname,"func_button") )
		{
			//if(Action.lAction & BA_GOBUTTON && pEnt==pButton2Use){
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
						if (fDistance < 50)
						{
							// don't avoid walls for a while
							f_dont_avoid = gpGlobals->time + 1.0;
							VATurnTo(entity_origin - pEdict->v.origin);
							lButton |= IN_USE;
							//Action.lAction&=~BA_GOBUTTON;
							f_use_button_time = gpGlobals->time;
							Task.NextTask();
						}
						return true;
					}
				}
			}
		}
		else if(FStrEq(szClassname,"armory_entity")){
			//cout << "-";
			if(FStrEq("models/w_hegrenade.mdl",STRING(pEnt->v.model))
				&& !(bot_weapons & (1<<CS_WEAPON_HEGRENADE))){
				Goto(pEnt->v.origin);
				return true;
			}
			else if(FStrEq("models/w_flashbang.mdl",STRING(pEnt->v.model))
				&& !(bot_weapons & (1<<CS_WEAPON_FLASHBANG))){
				Goto(pEnt->v.origin);
				return true;
			}
			else if(FStrEq("models/w_smokegrenade.mdl",STRING(pEnt->v.model))
				&& !(bot_weapons & (1<<CS_WEAPON_SMOKEGRENADE))){
				Goto(pEnt->v.origin);
				return true;
			}
			else if(FStrEq("models/w_kevlar.mdl",STRING(pEnt->v.model))
				&& bot_armor < 80){
				Goto(pEnt->v.origin);
				return true;
			}
			else if(!HasPrimary()){
				if (HasShield()){ // opt for the weapon instead of the shield
					// primary weapons that cost 2500 or more
					if(FStrEq("models/w_xm1014.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_ak47.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_m4a1.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_aug.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_scout.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_awp.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_sg552.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_g3sg1.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_sg550.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_m249.mdl",STRING(pEnt->v.model))
						)
					{
						Goto(pEnt->v.origin);
						return true;
					}
				}
				else {
					// primary weapons that cost less than 2500
					if(!HasShield() // get the weapon if we don't have the shield
						|| FStrEq("models/w_mp5navy.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_m3.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_tmp.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_mac10.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_ump45.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_galil.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_famas.mdl",STRING(pEnt->v.model))
						|| FStrEq("models/w_p90.mdl",STRING(pEnt->v.model))
						)
					{
						Goto(pEnt->v.origin);
						return true;
					}
				}
			}
		}	// endif cmp szclassname
	}
	return false;
}

bool CBotCS :: DistanceSight(void){
	//return false;
	edict_t *pEnt;
	float fDistance;
	float fPDistance;
	float fRDistance;
	float fN;
	long lDestination;
	char szClassname[80];
	Vector PVOrigin = pEdict->v.origin;
	edict_t *pNearP = GetNearestPlayer(pEdict,bot_teamnm,fPDistance,true);
	
	pEnt = 0;
	while(pEnt = UTIL_FindEntityInSphere(pEnt,PVOrigin,1500)){
		// to copy to every blabla  -> fDistance = Vector(pEnt->v.origin - pEdict->v.origin).Length2D();
		strcpy( szClassname , STRING(pEnt->v.classname) );
		
		if(!strncmp(szClassname,"weaponbox",strlen("weaponbox")) ||
			!strncmp(szClassname,"weapon_shield",strlen("weapon_shield")))
		{
			if (pEnt->v.effects & EF_NODRAW) // spectator or not drawn?
				continue;

			if(FVisible(pEnt->v.origin,pEdict)){
				long lWeaponID = strncmp(szClassname, "weapon_shield", 13) ? WeaponClass2ID(szClassname) : CS_WEAPON_SHIELD;
				
				if(lWeaponID == CS_WEAPON_C4){
					if(bot_teamnm == CS_TEAM_TE){		//te
						lDestination = WaypointFindNearest(pEnt->v.owner,500,bot_teamnm,fN,true,false);
						if(lDestination != -1
							&& Task.SearchT(BT_PICKUP) == -1){
							Task.AddTask(BT_GOTO|BT_PICKUP,gpGlobals->time + 15.0f,lDestination,0,0);
							f_Aggressivity += 3.0;
							ResetWPlanning();
						}
						return true;
					}
					else{							// ct
						if(f_noCamp < gpGlobals->time				// is it allowed to sniper
							&& pNearP
							&& fPDistance > 300.0f){					// is there already another ct
							if(f_noCamp < gpGlobals->time){
								if(Task.SearchT(BT_GUARD) == -1){
									Task.AddTask(BT_GUARD,gpGlobals->time + 15.0f,0,0,0);
									if(Task.current)
										Task.current->VT = pEnt->v.owner->v.origin + Vector(0,0,10);
									ResetWPlanning();
								}
							}
							return false;
						}
					}
				}
				else{	// other weapons than c4
					if(lWeaponID != -1
						&&IsPrimaryWeapon(1<<lWeaponID)){	// only 'see' primaries
						if(!m_bIsVIP){
							if(!HasPrimary()){				// bot has no primary weapon
								lDestination = WaypointFindNearest(pEnt->v.owner,500,bot_teamnm,fN,true,false);
								if(lDestination != -1
									&& Task.SearchT(BT_PICKUP) == -1){
									Task.AddTask(BT_GOTO|BT_PICKUP,gpGlobals->time + 15.0f,lDestination,0,0);
									ResetWPlanning();
								}
								return true;
							}
							else{		// only drop and pickup weapons when not fighting ...
								if(f_LastFight+6.0f < gpGlobals->time){
									if(IsWeaponBetterCurrent(lWeaponID)){			// current weapon is not the favourite one
										lDestination = WaypointFindNearest(pEnt->v.owner,500,bot_teamnm,fN,true,false);
										if(lDestination != -1
											&& Task.SearchT(BT_PICKUP) == -1){
											Task.AddTask(BT_GOTO|BT_PICKUP,gpGlobals->time + 15.0f,lDestination,0,0);
											if(!HasPrimary()){
												f_Aggressivity += 3.0;
											}
											ResetWPlanning();
										}
										return true;
									}
								}
							}
						}
					}
				}
			}
		}
		else if (FStrEq(szClassname,"hostage_entity")){		// hostages
			if(bot_teamnm == CS_TEAM_CT){
				if(fDistance > 400 &&						// don't 'steal' hostages
					fPDistance > 400 &&
					GetHosVel(pEnt) < 1.f&&
					m_iHostagesUsed < _MAXHOSTAGES		// don't take more than _MAXHOSTAGES
					&& IsAlive(pEnt)						// fix of rescue point bug ???
					&& FVisible(pEnt->v.origin,pEdict)){	// visible
					
					if (HasAlreadyHostage(pEnt)){	// do not try to take a hostage twice
						continue;
					}
					lDestination = WaypointFindNearest(pEnt,500,bot_teamnm,fN,true,false);
					if(lDestination != -1
						&& Task.SearchT(BT_PICKUP) == -1){
						Task.AddTask(BT_GOTO|BT_PICKUP,gpGlobals->time + 15.0f,lDestination,0,0);
						ResetWPlanning();
					}
				}
			}
			else{
			}
		}
		/*else if(FStrEq(szClassname,"item_thighpack")){
			if(bot_teamnm == CS_TEAM_CT
			&& FVisible(pEnt->v.origin,pEdict)
			&& !(Action.lAction & BA_DEFKIT)){
				fN = 0;
				lDestination = WaypointFindNearest(pEnt,500,bot_teamnm,fN,true,false);
				if(lDestination != -1
					&& Task.SearchT(BT_PICKUP) == -1){
					Task.AddTask(BT_GOTO|BT_PICKUP,0,lDestination,0);
					ResetWPlanning();
				}
			}
		}*/
		else if(FStrEq(szClassname,"grenade")){
			if(bot_teamnm == CS_TEAM_CT){
				if(FStrEq(STRING(pEnt->v.model),"models/w_c4.mdl")){	// only planted c4
					fDistance = Vector(pEnt->v.origin - pEdict->v.origin).Length2D();
					
					// is the origin of player 45 units above the ground ??
					//fRDistance = Vector(pEnt->v.origin - (pEdict->v.origin - Vector(0,0,45))).Length();
					fRDistance = fDistance;
					if(!FVisible(pEnt->v.origin + Vector(0,0,30),pEdict))			// if it's not too far and if it's visible
						continue;
					
					fN = 0;
					lDestination = WaypointFindNearest(pEnt,500,bot_teamnm,fN,false,false);
					if(lDestination != -1
						&& Task.SearchT(BT_PICKUP) == -1){
						Task.AddTask(BT_GOTO|BT_PICKUP,gpGlobals->time + 15.0f,lDestination,0,0);
						ResetWPlanning();
					}
				}
			}
		}	// endif cmp szclassname
	}
	return false;
}

bool CBotCS :: Bored(void){
	//return false;
	
	if(iOnLadder != 0)		// don't be bored on ladders
		return false;
	if(Task.current&&Task.current->lType&BT_IGNOREENEMY)
		return false;

	float fbias = gpGlobals->time - TIMETOBEBORED;
	
	if(f_LastFight < fbias	// only do this after 30s without combat
		&& f_Bored < fbias){// only do 'boring attacks' every ~30s
		f_BoredTill = gpGlobals->time + TIMEBEINGBORED;
		f_Bored = gpGlobals->time + RANDOM_FLOAT(-10.0,10.0);
		i_BorA = int(RANDOM_FLOAT(0,7));
	}
	
	if(f_BoredTill > gpGlobals->time){// still bored ??
		if(i_BorA == 0){
			if((current_weapon.iId == CS_WEAPON_M4A1||
				current_weapon.iId == CS_WEAPON_USP)){
				lButton |= IN_ATTACK2;
			}
			f_BoredTill = 0;		// don't do this again now
			
			return false;		// return false, cause there's been no change to movements
		}
		else if(i_BorA == 1){
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

					if(/*FVisible(entity_origin,pEdict)*/1){
						if(tr.flFraction == 1.0f
							||FStrEq(STRING(tr.pHit->v.classname),"func_breakable")){
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
							
							//lButton |= IN_ATTACK;
							Vector VAim=entity_origin-pEdict->v.origin;
							FireWeapon(VAim);
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
				Task.AddTask(BT_ROAMTEAM,gpGlobals->time + 60.0,0,GetRandomPlayerWO(pEdict,bot_teamnm,1,0,pEdict),0);
			}
		}
		else if(i_BorA == 3
			||  i_BorA == 4){		// spray
			if(CVAR_BOOL(jb_spraypaint)){
				TraceResult tr;
				Vector v1,v2;
				v1 = pEdict -> v.origin + pEdict -> v.view_ofs;
				v2 = v1 + Vector(0,0,-1000);
				//WaypointDrawBeam(pEdict,v1,v2,20,1,200,200,0,200,100);
				UTIL_TraceLine(v1,v2,ignore_monsters,dont_ignore_glass,pEdict,&tr);
				if(tr.pHit){
					int itndex = ENTINDEX(tr.pHit);
					char *szDecal = SDecals.GetRandomDecal();
					//cout << szDecal << endl;
					long idndex = DECAL_INDEX(szDecal);
					
					if(idndex != -1 && tr.flFraction != 1.0){
						UTIL_PlayerDecalTrace(&tr,ENTINDEX(pEdict),idndex,true);
						EMIT_SOUND_DYN2(pEdict,CHAN_VOICE,"player/sprayer.wav",1,ATTN_NORM,0,100);
					}
				}
			}
			
			f_BoredTill = 0;		// don't do this again now
		}
		else if(i_BorA == 5){						// just wait some time there
			if(!Task.Important() &&!(Action.lAction & BA_BOMBPL)){
				Task.AddTask(BT_CAMP,gpGlobals->time + 5.0f,0,0,0);
				//f_Hide = gpGlobals->time + 5.0f;
				//				pHidefrom = 0;
				//InitCamp();
			}
			f_BoredTill = 0;		// don't do this again now
		}
		else if(i_BorA == 6){			// Bot is bored, so why not throw a flashbang at the base ?
			if(RANDOM_LONG(0,100) < 10
				&& g_fRoundTime < 20.f
				&& bot_weapons&(1<<CS_WEAPON_FLASHBANG)){
				GOrder.Attack(pEdict->v.origin,CS_WEAPON_FLASHBANG);
				
				Chat.f_SayTime = gpGlobals->time + RANDOM_FLOAT(1,4.0);
				Chat.f_LastChat = gpGlobals->time + RANDOM_FLOAT(-jb_chatfreq->value/2.0,jb_chatfreq->value/2.0);
				strcpy(Chat.szChatText,"hehe, a flashbang for you motherfuckin' teammates :p\n\0");
			}
			f_BoredTill = 0;
		}
		else{
			f_BoredTill = 0;		// don't do this again now
		}
	}
	return false;
}

bool CBotCS :: HandleNearWP(int iNearWP, bool &bReturn){
	// reached goal ??
	if(iNearWP !=-1){
		if(waypoints[iNearWP].flags & W_FL_FLAG){
			if(strncmp(STRING(gpGlobals->mapname),"cs",sizeof(char)*2) == 0){			// cs
			}
			else if(strncmp(STRING(gpGlobals->mapname),"de",sizeof(char)*2) == 0){		// de
				if(bot_teamnm == CS_TEAM_TE){												// te - planting
					if( bot_weapons & (1<<CS_WEAPON_C4)){// test if the bot has c4
						if(current_weapon.iId == CS_WEAPON_C4){		// already changed to c4 ??
							//bPlantDefBomb = true;			// set flag that bot's currently planting bomb
							f_DenyWChange = gpGlobals->time + 5.0;
							Action.lAction |= BA_PLANT;
							lButton |= IN_ATTACK;
							//lButton |= IN_DUCK;
							f_ducktill = gpGlobals->time + 3.0f;
							f_Pause = gpGlobals->time + 3.0f;
							f_move_speed = 0;
							f_dont_check_stuck= gpGlobals->time + 1.0;
							//DEBUG_CLIENTCOMMAND(pEdict,"say_team planting bomb");
							//UTIL_HostSay(pEdict,1,"planting bomb");
							bReturn = true;
							return true;
						}
						else{	// change to c4
							Change2Weapon(CS_WEAPON_C4);
							f_RWKnife = gpGlobals->time;
							f_move_speed /= 2.0;
						}
					}
					else{		// no c4, just proceed
						//if(bPlantDefBomb){
						if(Action.lAction & BA_PLANT){
							Task.AddTaskI(BT_GUARD,gpGlobals->time + 20.0,0,0,0);
							if(Task.current)Task.current->VT = pEdict->v.origin;
							
							Action.lAction &= ~BA_PLANT;
						}
					}
				}
				else{// ct - defusing should be done by FindSpEnt
					if(g_bBombPlanted){		// searching the bomb
						bool bBomb  = false;
						edict_t *pEnt=0;
						/*while(pEnt=UTIL_FindEntityInSphere(pEnt,pEdict->v.origin,500)){
							if(FStrEq(STRING(pEnt->v.classname),"grenade")){
								bBomb = true;
								break;
							}
						}*/
						while(pEnt = UTIL_FindEntityByClassname(pEnt,"grenade")){
							if((pEnt->v.origin - pEdict->v.origin).Length() < 500.f){
								bBomb = true;
								break;
							}
						}
						if(!bBomb
							&&f_UsedRadio+5.0 < gpGlobals->time
							&& f_LastRadio + 3.0f < gpGlobals->time
							&& CVAR_BOOL(jb_msgradio)){
							SendRadioCommand(RADIO_SECTOR_CLEAR);
						}
					}
				}
			}
		}// end W_FL_FLAG
		
		if(waypoints[iNearWP].flags & W_FL_FLAG_GOAL){			// just say, no hostage
			if(m_iHostagesUsed
				&& bot_teamnm == CS_TEAM_CT){
				Task.AddTask(BT_CAMP, gpGlobals->time + 5.0,0,0,0);
				if(RANDOM_LONG(0,100) < 70)
					Jump();
				m_iHostagesUsed = 0;
				ResetWPlanning();
				
				bReturn = false;
				return true;
			}
		}
	}
	
	return CBotBase :: HandleNearWP(iNearWP,bReturn);
}

bool CBotCS :: HeadTowardWaypoint( void ){
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

	if(Task.current && Task.current ->lType & BT_CAMP){
		if(Task.current->lAdd == 0){
			InitCamp();
			Task.current->lAdd = 1;
		}
		if(Camp())
			return true;
	}
	else{
		WLookTo.iNum = 0;
	}
	
	// to have some time without going to WPs
	if(f_noWP > gpGlobals->time){
		//DEBUG_CLIENTCOMMAND(pEdict,"say not using wp");
		return false;
	}
	
	/*	if(fTest< gpGlobals->time){
	fTest += .1f;
	return true;
}*/
	
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
			VRunningTo = waypoints[i_CurrWP].origin; //should have been copied when next wp
			//DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,waypoints[i_CurrWP].origin,10,10,200,0,0,200,10);
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
				f_Pause = gpGlobals->time + .5f;
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
							//InitCamp();
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
					iNextWP = GoNextWPOnPath();
					
					if(waypoints[iNearWP].flags & W_FL_JUMP && !bReplay){
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
								//pEdict->v.origin = pWPAMPlay->Rec[0].v_origin;
								float fDot;
								int ischl,iMin = 0;
								for(ischl = 0;ischl < pWPAMPlay->iNum;ischl ++){
									fDot = DotProduct(pWPAMPlay->Rec[0].v_origin-pEdict->v.origin,
										pWPAMPlay->Rec[pWPAMPlay->iNum-1].v_origin-pEdict->v.origin);
									if(fDot > 0){
										iMin = ischl;
										break;
									}
								}
								fStartADVM += pWPAMPlay->Rec[iMin].fTime;
								DEBUG_CLIENTCOMMAND(pEdict,UTIL_VarArgs("say %f",pWPAMPlay->Rec[iMin].fTime));
							}
							else{
								f_Pause = gpGlobals->time + 1.5f;
								f_noWP = gpGlobals->time + 1.5f;
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
						//DEBUG_CLIENTCOMMAND(pEdict,"say lost way");
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
					
					// search a waypoint where a kill was around this time in the ground. if we find one, let's stay
					// there for some time, wait for the enemy and maybe throw a grenade
					if(	  !g_bBombPlanted
						&& fM[bot_teamnm]<0.0
						&& g_fRoundTime < 60.0 && g_fRoundTime > 15.0
						&& f_noCamp < gpGlobals->time
						&& iNearWP != -1
						&&!HasSniper()
						&&!m_bIsVIP
						&&!(waypoints[iNearWP].flags & W_FL_LADDER)
						&& f_LastFight < gpGlobals->time - 5.0){		// look if it's good to camp here
						if(RANDOM_LONG(0,100)<30){
							//f_noCamp = gpGlobals->time + 10.0f;
							int i,i1,index=-1;
							float distance;
							bool bTTime;
							
							//GetNearestPlayer(pEdict,bot_teamnm,fMin);
							fMin = 100;
							if(fMin>50){
								int iI = RANDOM_LONG(2,4);
								for (i=0; i < num_waypoints; i+=iI)
								{
									if ((waypoints[i].flags & W_FL_DELETED) == W_FL_DELETED)
										continue;  // skip any deleted waypoints
									
									if (waypoints[i].flags & W_FL_AIMING)
										continue;  // skip any aiming waypoints
									
									/*if(!FVisible(waypoints[i].origin,pEdict))
										continue;*/
									if(!WPStat.GetVisible(i,iNearWP))
										continue;

									if(WPStat[i].fADEn > 800)
										continue;

									//distance = (waypoints[i].origin - pEdict->v.origin).Length();
									distance = WaypointDistanceFromTo(i,iNearWP,bot_teamnm);
									
									if(distance > 600)
										continue;
									
									Vector2D vec2LOS;
									float    flDot;
									
									UTIL_MakeVectors ( pEdict->v.angles );
									
									vec2LOS = ( waypoints[i].origin - pEdict->v.origin ).Make2D();
									vec2LOS = vec2LOS.Normalize();
									
									flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );
									
									if ( flDot < 0.0)  // 180 degree field of view 
										continue;
									
									bTTime = false;
									for(i1=0;i1<MAX_TIMES_PWP;i1++){
										if(WPStat[i].fpKilledTime[i1]>1.f
											&&(TEq(g_fRoundTime+5.f,WPStat[i].fpKilledTime[i1],7.f))){
											bTTime = true;
											break;
										}
									}
									if(!bTTime){
										for(i1=0;i1<MAX_TIMES_PWP;i1++){
											if(WPStat[i].fpKillTime[i1]>1.f
												&&(TEq(g_fRoundTime+5.f,WPStat[i].fpKillTime[i1],7.f))){
												bTTime = true;
												break;
											}
										}
									}
									
									if(!bTTime
										&& WPStat[i].lKilled < WPStat.lKilledMax*2/3)
										continue;
									
									index = i;
									break;
								}
								if(index != -1){
									HeadToward(waypoints[i].origin);

									// no initcamp bt_camp call ( lAdd = 1 )
									Task.AddTask(BT_CAMP, gpGlobals->time + 14.0,1,0,0);		// set time to camp
									//f_Hide = gpGlobals->time + 14.f;
									//									pHidefrom = 0;
									f_StrCamp=gpGlobals->time;
									
									sLookToChange = 1;
									InitCamp();
									f_CaLooktoWP = gpGlobals->time + 1.5f;
									f_RWKnife = gpGlobals->time - 1.0;
									if(RANDOM_LONG(0,100) <70){
										// check if we can throw a grenade there
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

void CBotCS :: FindRoute(int iNearWP){
	struct foundway{
		short int index;
		CWay Way1;
		CWay Way2;
		WAYINFO wayinfo1;
		WAYINFO wayinfo2;
	};

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
				lMaxKilled=LONG_MAX,
				lSelection = -1;
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
				WPStat.GetWayInfo(indexes[index].Way1,indexes[index].wayinfo1);		// statistical information about kills, etc.
				STMem.GetWayInfo(indexes[index].Way1,indexes[index].wayinfo1);		// STMEM -> sort term memory
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
					if(indexes[index].Way1.iIndices[indexes[index].Way1.iNum-1] == indexes[index].Way2.iIndices[1]){
						//DEBUG_CLIENTCOMMAND(pEdict,"say going back - we don't really want that, are we ?");
						indexes[index].index = -1;
					}
				}
				
				index ++;
			}
			
			// end remove near wps
			// if hostages test on hostageability
			if(m_iHostagesUsed){
				for(index=0;index < count;index++){
					if(!indexes[index].wayinfo1.bHostageable){
						indexes[index].index = -1;
					}
				}
			}
			
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
			
			// if the bots wants a special way, which is p.ex. safe, dangerous or whatever,
			// remove the ways, which don't fit
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
				lSelection = RANDOM_LONG(0, count - 1);
				iGoal = indexes[lSelection].index;
			}
			else{
				if(m_iHostagesUsed){
					iFarGoal = -1;
					//DEBUG_CLIENTCOMMAND(pEdict,"say no way for hostages");
				}
				else{
					iGoal = iFarGoal;
				}
			}
			
			iWantedDiv --;
			
			/*delete [] indexes;
			delete [] preindexes;*/
			
			TestOnButtonWay(indexes[lSelection].Way1,&pToUse);
			
			if(pToUse){
				int iDest = WaypointFindNearest(VecBModelOrigin(VARS(pToUse)),pEdict,400,bot_teamnm);
				if(iDest != -1
					&& !(Task.current&&Task.current->lType&BT_GOBUTTON)){
					//&&Task.SearchT(BT_GOBUTTON) == -1){
					Task.AddTask(BT_GOTO|BT_GOBUTTON|BT_TMP,-1,iDest,pToUse,0);
				}
				//DEBUG_CLIENTCOMMAND(pEdict,"say wanting to go where pressing a button is needed");
			}
	}
	else{
		// no divisions wanted, just make the final goal the current one
		// but beforehand test if there are hostages or buttons
		if(WaypointDistanceFromTo(iNearWP,iFarGoal,bot_teamnm) > 650
			||m_iHostagesUsed){
			CWay *pWay = new CWay;
			ConvertFloyd2Way(bot_teamnm,iNearWP,iFarGoal,pWay);
			if(m_iHostagesUsed){
				if(!Hostagable(pWay)){
					iGoal = -1;
					iWantedDiv++;
					delete pWay;
					return;
				}
			}
			if(1||pWay->iNum > 4){
				TestOnButtonWay(*pWay,&pToUse);
				if(pToUse){
					if(Task.current
						&& Task.current->p == pToUse
						&& Task.current->lType&BT_GOBUTTON){	// that is to press the button the button should have been pressed ... that doesn't work and therefore delete ya orders
						ResetWPlanning();
						Task.RemoveT(BT_GOTO);
						return;
					}
					int iDest = WaypointFindNearest(VecBModelOrigin(VARS(pToUse)),pEdict,400,bot_teamnm);
					if(iDest != -1
						&& !(Task.current&&Task.current->lType&BT_GOBUTTON)){
						//&&Task.SearchT(BT_GOBUTTON) == -1){
						Task.AddTask(BT_GOTO|BT_GOBUTTON|BT_TMP,-1,iDest,pToUse,0);
					}
					//DEBUG_CLIENTCOMMAND(pEdict,"say wanting to go where pressing a button is needed");
				}
			}
			delete pWay;
		}
		iGoal = iFarGoal;
	}
}
}

bool CBotCS :: DecideOnWay(void){
	int iNearest = WaypointFindNearest(pEdict,300,bot_teamnm);
	
	//DEBUG_CLIENTCOMMAND(pEdict,"say generating new way");
	
	iWantedDiv = 0;		// by default the are no wanted divisions
	
	lTypeOWW = WW_NONE;		// default ... do nothing bout experiences
	
	if(d_Manner > 1.0 || RANDOM_LONG(0,100)<20){
		lTypeOWW = WW_AGG;
	}
	else if(d_Manner < -1.0 || RANDOM_LONG(0,100)<20){
		lTypeOWW = WW_DEF;
	}
	
	if(pEdict->v.health < 50 || RANDOM_LONG(0,100)<20){
		lTypeOWW = WW_VDEF;
	}
	
	if(m_bIsVIP){		// vip's way should be safe
		lTypeOWW = WW_DEF;
	}
	
	//cout << Task.current << " - " << Task.lNOT << endl;
	ResetWPlanning();
	
	CheckTasksOnReachability(iNearest);
	
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
		long lDestination;
		// is there something in the bots short term mem ?
		if(STMem.iCount){
			int iMemI=0;
			lDestination = -1;
			while(lDestination==-1
				&&iMemI<STMem.iCount){
				lDestination = WaypointFindNearest(STMem[iMemI].pEdict,500,bot_teamnm,0,true,false);		//hackhack bot doesn't know the exact position of enemy
				iMemI ++;
			}
			iWantedDiv=0;
			if(lDestination != -1){
				Task.AddTask(BT_GOTO,-1,lDestination,(void *) iWantedDiv,0);
				return true;
			}
		}
		
		if (g_iMapType == MT_CS){		// cs
			//DEBUG_CLIENTCOMMAND(pEdict,"say cs");
			if(bot_teamnm == CS_TEAM_TE){								// te
				if(g_fRoundTime < 20
					&& RANDOM_LONG(0,100) < 20
					&&!HasSniper()){
					lDestination = WPStat.Search(bot_teamnm,iNearest,WPS_SEARCH_ENEMYESPTT,400,2000,50,5);
					if(lDestination != -1){
						Task.AddTask(BT_GOTO|BT_CAMPATGOAL|BT_CROUCH,-1,lDestination,(void*)iWantedDiv,15);//todo
						return true;
					}
				}
				else if(HasSniper()			// go to sniper point, if ya've a good sniper weapon
					&& RANDOM_LONG(0,100) < 50){
					if(RANDOM_LONG(0,100) < 90 && WPStat.d.lKill > 20){
						lDestination = WPStat.Search(bot_teamnm,iNearest,WPS_SEARCH_SNIPER,400,3000);
						if(lDestination != -1){
							Task.AddTask(BT_GOTO|BT_CAMPATGOAL,0,lDestination,(void*)iWantedDiv,20);//todo
						}
					}
					else{
						if(RANDOM_LONG(0,100) < 70)
							lDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
						else
							lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
						if(lDestination != -1){
							Task.AddTask(BT_GOTO|BT_CAMPATGOAL,0,lDestination,(void *) iWantedDiv,RANDOM_LONG(long(g_fmin_std_swp),long(g_fmax_std_swp)));
							return true;
						}
					}
				}
				else if(RANDOM_LONG(0,100)>70){
					lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_FLAG);
					iWantedDiv = 1;
				}
				else if(RANDOM_LONG(0,100)>50){
					if(HasPrimary())
						if(RANDOM_LONG(0,100)>50)
							lDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
						else
							lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
				}
				else if(RANDOM_LONG(0,100)>80){
					lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_VISIT);
					iWantedDiv = 1;
				}
				else{
					lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_FLAG_GOAL);
					iWantedDiv = 1;
				}
				if(lDestination != -1){
					Task.AddTask(BT_GOTO,0,lDestination,(void *) iWantedDiv,0);
					return true;
				}
				return true;
			}
			else{														// ct
				if(m_iHostagesUsed>0){		// bring hostages back home
					lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_RESCUE);
					iWantedDiv = 1;
					//DEBUG_CLIENTCOMMAND(pEdict,"say cs-ct-wf");
				}
				else if(HasSniper()			// go to sniper point, if ya've a good sniper weapon
					&& RANDOM_LONG(0,100) < 50){
					if(RANDOM_LONG(0,100) < 70 && WPStat.d.lKill > 20){
						lDestination = WPStat.Search(bot_teamnm,iNearest,WPS_SEARCH_SNIPER,400,2000);
						if(lDestination != -1){
							Task.AddTask(BT_GOTO|BT_CAMPATGOAL,0,lDestination,(void*)iWantedDiv,20);
							return true;
						}
					}
					else{
						if(RANDOM_LONG(0,100) < 70)
							lDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
						else
							lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
						if(lDestination != -1){
							Task.AddTask(BT_GOTO|BT_CAMPATGOAL,0,lDestination,(void *) iWantedDiv,RANDOM_LONG(long(g_fmin_std_swp),long(g_fmax_std_swp)));
							return true;
						}
					}
				}
				else if(RANDOM_LONG(0,100) > 50){
					lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_FLAG);
					iWantedDiv = 1;
				}
				else if(RANDOM_LONG(0,100)>50){
					if(HasPrimary()){
						if(RANDOM_LONG(0,100)>45)
							lDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
						else
							lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
					}
				}
				else if(RANDOM_LONG(0,100)>80){
					lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_VISIT);
					iWantedDiv = 1;
				}
				else{
					lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_FLAG_GOAL);
					iWantedDiv = 1;
				}
				
				if(lDestination != -1){
					Task.AddTask(BT_GOTO,0,lDestination,(void *) iWantedDiv,0);
					return true;
				}
				return true;
			}
		}
		else if (g_iMapType == MT_DE){	// de
			//DEBUG_CLIENTCOMMAND(pEdict,"say de");
			if(bot_teamnm == CS_TEAM_TE){								// te
				if(HasSniper()			// go to sniper point, if ya've a good sniper weapon
					&& RANDOM_LONG(0,100) < 50
					&& !(bot_weapons & 1<<CS_WEAPON_C4)){
					if(RANDOM_LONG(0,100) < 70 
						&& WPStat.d.lKill > 20){
						lDestination = WPStat.Search(bot_teamnm,iNearest,WPS_SEARCH_SNIPER,400,2000);
						if(lDestination != -1){
							Task.AddTask(BT_GOTO|BT_CAMPATGOAL,0,lDestination,(void*)iWantedDiv,20);
							return true;
						}
					}
					else{
						if(RANDOM_LONG(0,100) < 50)
							lDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
						else
							lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
						if(lDestination != -1){
						Task.AddTask(BT_GOTO|BT_CAMPATGOAL,0,lDestination,(void *) iWantedDiv,RANDOM_LONG(long(g_fmin_std_swp),long(g_fmax_std_swp)));
						return true;
					}
					}
				}
				else if(bot_weapons & 1<<CS_WEAPON_C4					// if ya've c4 and U#ve already played some time ...
					&& RANDOM_LONG(0,100) < 50){
					lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_FLAG);
					iWantedDiv = 0;
				}
				else if(bot_weapons & 1<<CS_WEAPON_C4					// if ya've c4 and U#ve already played some time ...
					&&g_fRoundTime > 60.0f){
					lDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_FLAG);
					iWantedDiv = 0;
				}
				else if(RANDOM_LONG(0,100)>50
					&&g_fRoundTime < 60.f){	// new
					lDestination = WPStat.Search(bot_teamnm,iNearest,WPS_SEARCH_ENEMYESPTT,400,20000,60,10);
					iWantedDiv = 1;
				}
				else if(RANDOM_LONG(0,100)>70){
					lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_VISIT);
					iWantedDiv = 1;
				}
				else if(RANDOM_LONG(0,100) < 15){
					if(g_bBombDropped){
						edict_t *pEnt = 0;
						while(pEnt=UTIL_FindEntityByClassname(pEnt,"weapon_c4")){
							break;
						}
						if(pEnt &&
							pEnt->v.owner)
							lDestination = WaypointFindNearest(pEnt->v.owner->v.origin,pEdict,200,bot_teamnm);
						else
							lDestination = -1;
					}
				}
				else if(RANDOM_LONG(0,100) < 30){
					lDestination = WaypointFindNearest(pEdict,10000,bot_teamnm,RANDOM_LONG(400,700));
					iWantedDiv = 1;
				}
				else{
					lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_FLAG);
					iWantedDiv = 1;
				}
				if(lDestination != -1){
					Task.AddTask(BT_GOTO,0,lDestination,(void *) iWantedDiv,0);
					return true;
				}
				return true;
			}
			else{													// ct
				if(g_bBombPlanted){
					//FindWayFlag(pEdict,W_FL_FLAG,FW_Cur,WayDecideShortest);
					if(iNearWP == -1)
						iNearWP = WaypointFindNearest(pEdict,5000,bot_teamnm);

					lDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_FLAG,400);		// todo ... IMPROVE !!!!

					if(lDestination != -1){
						Task.AddTask(BT_GOTO,0,lDestination,(void *) iWantedDiv,0);
						return true;
					}
					else{
						lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,1000000,bot_teamnm,W_FL_FLAG);
						Task.AddTask(BT_GOTO,0,lDestination,(void *) iWantedDiv,0);
						return true;
					}
				}
				if(g_fRoundTime < 20
					&& RANDOM_LONG(0,100) < 30
					&&!HasSniper()){
					lDestination = WPStat.Search(bot_teamnm,iNearest,WPS_SEARCH_ENEMYESPTT,400,2000,50,5);
					if(lDestination != -1){
						//cout << "test" << endl;
						Task.AddTask(BT_GOTO|BT_CAMPATGOAL|BT_CROUCH,-1,lDestination,(void*)iWantedDiv,15);//todo
						return true;
					}
				}
				else if(HasSniper()			// go to sniper point, if ya've a good sniper weapon
					&& RANDOM_LONG(0,100) < 50){
					if(RANDOM_LONG(0,100) < 70 && WPStat.d.lKill > 20){
						lDestination = WPStat.Search(bot_teamnm,iNearest,WPS_SEARCH_SNIPER,400,2000);
						if(lDestination != -1){
							Task.AddTask(BT_GOTO|BT_CAMPATGOAL,0,lDestination,(void*)iWantedDiv,20);
							return true;
						}
					}
					else{
						if(RANDOM_LONG(0,100) < 50)
							lDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
						else
							lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
						if(lDestination != -1){
						Task.AddTask(BT_GOTO|BT_CAMPATGOAL,0,lDestination,(void *) iWantedDiv,RANDOM_LONG(long(g_fmin_std_swp),long(g_fmax_std_swp)));
						return true;
					}
					}
				}
				else if(RANDOM_LONG(0,100) > 97){
					if(HasPrimary()){
						lDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
						iWantedDiv = 1;
					}
				}
				else if(RANDOM_LONG(0,100)>60
					&&g_fRoundTime < 60.f){	// new
					lDestination = WPStat.Search(bot_teamnm,iNearest,WPS_SEARCH_ENEMYESPTT,400,20000,60,10);
					iWantedDiv = 1;
				}
				else if(RANDOM_LONG(0,100)>70){
					lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_VISIT);
					iWantedDiv = 1;
				}
				else if(RANDOM_LONG(0,100) < 20){
					lDestination = WaypointFindNearest(pEdict,10000,bot_teamnm,RANDOM_LONG(400,700));
					iWantedDiv = 1;
				}
				else{
					if(RANDOM_LONG(0,100) < 85){
						if(waypoints[iNearest].flags & W_FL_FLAG){
							if(RANDOM_LONG(0,100) < 50 || (RANDOM_LONG(0,100)<80&&g_fRoundTime < 30.0f)){
								lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_FLAG);
								iWantedDiv = 1;
							}
							else{
								lDestination = iNearest;
								iWantedDiv = 2;
							}
						}
						else{
							lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_FLAG);
							iWantedDiv = 1;
						}
					}
					else{
						lDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_FLAG);
						iWantedDiv = 1;
					}
				}
				if(lDestination != -1){
					Task.AddTask(BT_GOTO,0,lDestination,(void *) iWantedDiv,0);
					return true;
				}
				return true;
			}
	}
	else if(g_iMapType == MT_AS){
		//if(RANDOM_LONG(0,100) < 10){DEBUG_CLIENTCOMMAND(pEdict,"say ---------------");}
		if(bot_teamnm == CS_TEAM_CT){
			if(m_bIsVIP){
				lDestination = WaypointFindRandomGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_VIP_RESCUE);
				iWantedDiv = 1;
				//DEBUG_CLIENTCOMMAND(pEdict,"say vip");
				if(lDestination != -1){
					Task.AddTask(BT_GOTO,0,lDestination,(void *) iWantedDiv,0);
					return true;
				}
				else
					return false;
			}
			else{
				if(g_pVIP){
					if(HasSniper() && RANDOM_LONG(0,100) < 60){
						if(RANDOM_LONG(0,100)<80){
							int iNearestVIP = WaypointFindNearest(g_pVIP,1000,bot_teamnm);
							lDestination = WPStat.Search(bot_teamnm,iNearestVIP,WPS_SEARCH_SNIPER,400,10000);
							if(lDestination != -1){
								Task.AddTask(BT_GOTO|BT_CAMPATGOAL,0,lDestination,(void*)iWantedDiv,20);
								return true;
							}
						}
						else{
							lDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
						}
					}
					else{
						if(RANDOM_LONG(0,100) < 20)
							lDestination = WaypointFindNearest(g_pVIP,1000,bot_teamnm,400);
						else
							lDestination = WaypointFindNearest(g_pVIP,1000,bot_teamnm,400,false,true);
						iWantedDiv = 1;
					}
					if(lDestination != -1){
						Task.AddTask(BT_GOTO|BT_CAMPATGOAL,0,lDestination,(void *) iWantedDiv,RANDOM_LONG(long(g_fmin_std_swp),long(g_fmax_std_swp)));
						return true;
					}
				}
				/*else
				lDestination = WaypointFindNearest(pEdict,1000,bot_teamnm,400);*/
				if(lDestination != -1){
					Task.AddTask(BT_GOTO,0,lDestination,(void *) iWantedDiv,0);
					return true;
				}
				return true;
			}
		}
		else{		// te
			if(g_pVIP){
				if(HasSniper() && RANDOM_LONG(0,100) < 80){
					if(RANDOM_LONG(0,100) < 80){
						int iNearestVIP = WaypointFindNearest(g_pVIP,1000,bot_teamnm);
						lDestination = WPStat.Search(bot_teamnm,iNearestVIP,WPS_SEARCH_SNIPER,400,10000);
						if(lDestination != -1){
							Task.AddTask(BT_GOTO|BT_CAMPATGOAL,0,lDestination,(void*)iWantedDiv,20);
							return true;
						}
					}
					else{
						lDestination = WaypointFindNearestGoal(pEdict->v.origin,pEdict,100000,bot_teamnm,W_FL_SNIPER);
					}
					if(lDestination != -1){
						Task.AddTask(BT_GOTO|BT_CAMPATGOAL,0,lDestination,(void *) iWantedDiv,RANDOM_LONG(long(g_fmin_std_swp),long(g_fmax_std_swp)));
						return true;
					}
				}
				else{
					lDestination = WaypointFindNearest(g_pVIP,1300,bot_teamnm,500);
				}
			}
			if(lDestination != -1){
				Task.AddTask(BT_GOTO,0,lDestination,(void *) iWantedDiv,0);
				return true;
			}
			return true;
		}
		return false;
	}
	}
	
	/*DEBUG_CLIENTCOMMAND(pEdict,"say This type of map is not yet implementated");
	ResetWPlanning();*/
	return false;
}

void CBotCS :: CheckTasksOnReachability(int iNearWP){
	return;

	long lschl;
	int iNextWP;
	
	for(lschl = 0;lschl < Task.lNOT;lschl ++){
		CTaskItem *current = Task.GetTask(lschl);
		
		if(current->lType & BT_GOTO
			||current->lType & BT_CAMPATGOAL){
			iNextWP = WaypointRouteFromTo(iNearWP,current->lAdd,bot_teamnm);
			
			if(iNearWP == current->lAdd
				|| iNextWP == WAYPOINT_UNREACHABLE
				|| iNextWP > WAYPOINT_UNREACHABLE/2){
				current->lType |= BT_DEL;
			}
		}
	}
	
	Task.AddTask(0,0,0,0,0);		// just to make sure there is no bt_del task at top of all
	Task.NextTask();
}

bool CBotCS :: IsWeaponBetterCurrent(long lWeaponID){
	/*if(((HasSubM() && IsRifleWeapon(1<<lWeaponID))		// bot has a sub and gets a rifle
		|| (HasSecondary() != CS_WEAPON_ELITE && lWeaponID == CS_WEAPON_ELITE
		||(HasShotgun() == CS_WEAPON_M3 && lWeaponID == CS_WEAPON_XM1014)		// get a xm1014 when having a m3
		||(HasSniper() && HasSniper() != CS_WEAPON_AWP && lWeaponID == CS_WEAPON_AWP))		// bot has a sniper and picks up the awp
		&& !HasWeapon(1<<lPWeapon))
		)
		return true;
	else
		return false;*/
	
	if(IsPrimaryWeapon(1<<lWeaponID)){
		int iMyPGun = HasPrimary();
		if(!iMyPGun)
			return true;
		if(iMyPGun != lPWeapon
			&& lWeaponID == lPWeapon){
			return true;
		}
		if(!IsMachineGunWeapon(1<<lWeaponID)){
			if(HasSubM() && IsRifleWeapon(1<<lWeaponID))
				return true;
			if((HasRifle()||HasMachineGun()) && IsSubMWeapon(1<<lWeaponID))
				return false;
		}
		// let's just handle primary weapon stuff here first
		if( (HasSniper()&&IsSniperWeapon(1<<lWeaponID))
			|| (!HasSniper()&&!IsSniperWeapon(1<<lWeaponID))){
			if(Personality.iWeaponPriority[iMyPGun] < Personality.iWeaponPriority[lWeaponID]){
				return true;
			}
		}
	}
	else if(IsSecondaryWeapon(1<<lWeaponID)){
		int iMySGun = HasSecondary();
		if(!iMySGun)
			return true;
		if(!IsKnifeWeapon(1<<lWeaponID) && !HasShield()){
			if(Personality.iWeaponPriority[iMySGun] < Personality.iWeaponPriority[lWeaponID]){
				return true;
			}
		}
	}
	else if(IsShieldWeapon(1<<lWeaponID)){
		if (!HasPrimary() && !HasShield()){
			if (HasSecondary() != CS_WEAPON_ELITE)
				return true;
		}
	}
	return false;
}

int CBotCS :: GoNextWPOnPath(void){
	return GetNextWPOnPath();
}

int CBotCS :: GetNextWPOnPath(void){
	return WaypointRouteFromTo(iNearWP,iGoal,bot_teamnm);
}
