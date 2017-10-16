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
#include "Chat.h"
#include "Commandfunc.h"
#include "globalvars.h"
#include "NNWeapon.h"

DOD_Flag g_DODFlags[_MAXFLAGS];
float fDODFlagU;

CBotDOD :: CBotDOD(){
	fDODFlagU = 0;
	
	f_pronetill = 0;
	f_changedprone = 0;
	
	bTNT = false;
	
	//Chat.Load(this);
}

CBotDOD :: ~CBotDOD(){
}

void CBotDOD :: HandleMenu( void )
{
	char c_class[32];
	
	if (start_action == MSG_DOD_IDLE){
		if (bot_team == 3 && UTIL_PlayerCount(COUNT_HUMAN))
			FakeClientCommand(pEdict, "changeteam");
	}
	else if (start_action == MSG_DOD_TEAM_SELECT){
		//start_action = MSG_DOD_CLASS_SELECT;  // switch back to idle
		start_action = MSG_DOD_IDLE;  // switch back to idle
		
		if ((bot_team != 1) && (bot_team != 2) && (bot_team != 5))
		{
			if (!CVAR_BOOL(jb_jointeam))
			{
				bot_team = UTIL_PlayerCount(COUNT_HUMAN) ? 5 : 3;
			}
			else
				bot_team = 5; // auto-assign if invalid
		}
		// select the team the bot wishes to join...
		FakeClientCommand(pEdict, "jointeam %d", bot_team);
		//FakeClientCommand(pEdict, "menuselect %d", bot_team);
		return;
	}
	
	else if (start_action == MSG_DOD_CLASS_SELECT_AL)
	{
		start_action = MSG_DOD_IDLE;  // switch back to idle
		//create_time = gpGlobals->time;  // reset
		// select the class the bot wishes to use...
		if ((bot_class < 1) || (bot_class > 5))
			bot_class = -1;  // use random if invalid
		
		/*if (bot_class == -1)
			bot_class = RANDOLONG(1,5);*/

		// select the class the bot wishes to use...
		if (bot_class == 1)
			strcpy(c_class, "lightinf");
		else if (bot_class == 2)
			strcpy(c_class, "assaultinf");
		else if (bot_class == 3)
			strcpy(c_class, "sniper");
		else if (bot_class == 4)
			strcpy(c_class, "heavyweapons");
		else if (bot_class == 5)
			strcpy(c_class, "machinegunner");
		else if (bot_class == -1)
			strcpy(c_class, "randompc");

		FakeClientCommand(pEdict, "%s", c_class);
		//FakeClientCommand(pEdict, "menuselect %i", bot_class);
		
		// bot has now joined the game (doesn't need to be started)
		//not_started = false;
		return;
	}
	else if (start_action == MSG_DOD_CLASS_SELECT_AX)
	{
		start_action = MSG_DOD_IDLE;  // switch back to idle
		//create_time = gpGlobals->time;  // reset
		// select the class the bot wishes to use...
		if ((bot_class < 1) || (bot_class > 5))
			bot_class = -1;  // use random if invalid

		// select the class the bot wishes to use...
		if (bot_class == 1)
			strcpy(c_class, "leichtinf");
		else if (bot_class == 2)
			strcpy(c_class, "sturmtrup");
		else if (bot_class == 3)
			strcpy(c_class, "germansniper");
		else if (bot_class == 4)
			strcpy(c_class, "heavyweaponsger");
		else if (bot_class == 5)
			strcpy(c_class, "machinegunnerger");
		else if (bot_class == -1)
			strcpy(c_class, "randompc");

		FakeClientCommand(pEdict, "%s", c_class);
		//FakeClientCommand(pEdict, "menuselect %i", bot_class);
		
		// bot has now joined the game (doesn't need to be started)
		//not_started = false;
		return;
	}

	else if (start_action == MSG_DOD_CLASS_SELECT_AL_PARA)
	{
		start_action = MSG_DOD_IDLE;  // switch back to idle
		//create_time = gpGlobals->time;  // reset
		// select the class the bot wishes to use...
		if ((bot_class < 1) || (bot_class > 7))
			bot_class = -1;  // use random if invalid
		
		// select the class the bot wishes to use...
		if (bot_class == 1)
			strcpy(c_class, "para_garand");
		else if (bot_class == 2)
			strcpy(c_class, "para_carbine");
		else if (bot_class == 3)
			strcpy(c_class, "para_thompson");
		else if (bot_class == 4)
			strcpy(c_class, "para_greasegun");
		else if (bot_class == 5)
			strcpy(c_class, "para_spring");
		else if (bot_class == 6)
			strcpy(c_class, "para_bar");
		else if (bot_class == 7)
			strcpy(c_class, "para_30cal");
		else if (bot_class == -1)
			strcpy(c_class, "para_random");

		FakeClientCommand(pEdict, "%s", c_class);
		
		// bot has now joined the game (doesn't need to be started)
		//not_started = false;
		return;
	}
	else if (start_action == MSG_DOD_CLASS_SELECT_AX_PARA)
	{
		start_action = MSG_DOD_IDLE;  // switch back to idle
		//create_time = gpGlobals->time;  // reset
		// select the class the bot wishes to use...
		if ((bot_class < 1) || (bot_class > 8))
			bot_class = -1;  // use random if invalid

		// select the class the bot wishes to use...
		if (bot_class == 1)
			strcpy(c_class, "para_kar");
		else if (bot_class == 2)
			strcpy(c_class, "para_k43");
		else if (bot_class == 3)
			strcpy(c_class, "para_scopedkar");
		else if (bot_class == 4)
			strcpy(c_class, "para_mp40");
		else if (bot_class == 5)
			strcpy(c_class, "para_mp44");
		else if (bot_class == 6)
			strcpy(c_class, "para_fg42");
		else if (bot_class == 7)
			strcpy(c_class, "para_mg34");
		else if (bot_class == 8)
			strcpy(c_class, "para_mg42");
		else if (bot_class == -1)
			strcpy(c_class, "para_random");

		FakeClientCommand(pEdict, "%s", c_class);
		
		// bot has now joined the game (doesn't need to be started)
		//not_started = false;
		return;
	}

	else if (start_action == MSG_DOD_WPN_SELECT_SERGEANT)
	{
		if(RANDOM_LONG(0,100) < 50){
			FakeClientCommand(pEdict,"selectwpn1");
		}
		else
			FakeClientCommand(pEdict,"selectwpn2");
		start_action = MSG_DOD_IDLE;  // switch back to idle
	}
	else if (start_action == MSG_DOD_WPN_SELECT_MACHINE)
	{
		if(RANDOM_LONG(0,100) < 50){
			FakeClientCommand(pEdict,"selectwpn1");
		}
		else
			FakeClientCommand(pEdict,"selectwpn2");
		start_action = MSG_DOD_IDLE;  // switch back to idle
	}
	else if (start_action == MSG_DOD_WPN_SELECT_GRENADIER)
	{
		if(RANDOM_LONG(0,100) < 50){
			FakeClientCommand(pEdict,"selectwpn1");
		}
		else
			FakeClientCommand(pEdict,"selectwpn2");
		start_action = MSG_DOD_IDLE;  // switch back to idle
	}
	else if (start_action == MSG_DOD_WPN_SELECT_RIFLEMAN)
	{
		if(RANDOM_LONG(0,100) < 50){
			FakeClientCommand(pEdict,"selectwpn1");
		}
		else
			FakeClientCommand(pEdict,"selectwpn2");
		start_action = MSG_DOD_IDLE;  // switch back to idle
	}
	else if (start_action == MSG_DOD_WPN_SELECT_FG42)
	{
		if(RANDOM_LONG(0,100) < 50){
			FakeClientCommand(pEdict,"selectwpn1");
		}
		else
			FakeClientCommand(pEdict,"selectwpn2");
		start_action = MSG_DOD_IDLE;  // switch back to idle
	}
}

long CBotDOD :: IsSecondaryWeapon(long lbit){
	
	if(lbit & 1<<DOD_WEAPON_LUGER){
		return DOD_WEAPON_LUGER;
	}
	else if(lbit & 1<<DOD_WEAPON_COLT){
		return DOD_WEAPON_COLT;
	}
	return 0;
}

long CBotDOD :: IsPrimaryWeapon(long lbit){
	
	if(lbit & 1<<DOD_WEAPON_KAR){
		return DOD_WEAPON_KAR;
	}
	else if(lbit & 1<<DOD_WEAPON_MP40){
		return DOD_WEAPON_MP40;
	}
	else if(lbit & 1<<DOD_WEAPON_MP44){
		return DOD_WEAPON_MP44;
	}
	else if(lbit & 1<<DOD_WEAPON_GEWEHR){
		return DOD_WEAPON_GEWEHR;
	}
	else if(lbit & 1<<DOD_WEAPON_GARAND){
		return DOD_WEAPON_GARAND;
	}
	else if(lbit & 1<<DOD_WEAPON_THOMPSON){
		return DOD_WEAPON_THOMPSON;
	}
	else if(lbit & 1<<DOD_WEAPON_SPRING){
		return DOD_WEAPON_SPRING;
	}
	else if(lbit & 1<<DOD_WEAPON_BAR){
		return DOD_WEAPON_BAR;
	}
	return 0;
}

long CBotDOD :: IsShotgunWeapon(long lbit){
	return 0;
}

long CBotDOD :: IsSubMWeapon(long lbit){
	if(lbit & 1<<DOD_WEAPON_MP40){
		return DOD_WEAPON_MP40;
	}
	else if(lbit & 1<<DOD_WEAPON_MP44){
		return DOD_WEAPON_MP44;
	}
	else if(lbit & 1<<DOD_WEAPON_THOMPSON){
		return DOD_WEAPON_THOMPSON;
	}
	else if(lbit & 1<<DOD_WEAPON_BAR){
		return DOD_WEAPON_BAR;
	}
	else if(lbit & 1<<DOD_WEAPON_M1CARBINE){
		return DOD_WEAPON_M1CARBINE;
	}
	return 0;
}

long CBotDOD :: IsMachineGunWeapon(long lbit){
	if(lbit & 1<<DOD_WEAPON_30CAL){
		return DOD_WEAPON_30CAL;
	}
	else if(lbit & 1<<DOD_WEAPON_MG34){
		return DOD_WEAPON_MG34;
	}
	else if(lbit & 1<<DOD_WEAPON_MG42){
		return DOD_WEAPON_MG42;
	}
	else
		return 0;
}

long CBotDOD :: IsRifleWeapon(long lbit){
	return 0;
}

long CBotDOD :: IsKnifeWeapon(long lbit){
	if(lbit & 1<<DOD_WEAPON_AMERKNIFE){
		return DOD_WEAPON_AMERKNIFE;
	}
	else if(lbit & 1<<DOD_WEAPON_GERKNIFE){
		return DOD_WEAPON_GERKNIFE;
	}
	else if(lbit & 1<<DOD_WEAPON_SPADE){
		return DOD_WEAPON_SPADE;
	}
	else
		return 0;
}

long CBotDOD :: IsSniperWeapon(long lbit){				// return weapons, who allow you to zoom
	if(lbit & 1<<DOD_WEAPON_GEWEHR){
		return DOD_WEAPON_GEWEHR;
	}
	else if(lbit & 1<<DOD_WEAPON_SPRING){
		return DOD_WEAPON_SPRING;
	}
	return 0;
}

long CBotDOD :: IsGrenadeWeapon(long lbit){
	return 0;
}

void CBotDOD :: SpawnInit(void)
{
	fDODFlagU = 0;
	Task.Reset();
	
	v_prev_origin = Vector(9999.0, 9999.0, 9999.0);
	
	// init some global vars, they are nvtl recaluclated before next think
	g_msecnum = 0;
	g_msecdel = 0.0;
	g_msecval = 0.0;
	
	bot_health = 0;
	bot_armor = 0;
	bot_weapons = 0;
	
	//f_max_speed = pEdict->v.maxspeed
	
	prev_speed = 0.0;  // fake "paused" since bot is NOT stuck
	
	pBotEnemy = NULL;  // no enemy yet
	
	//InitBotVars();
	
	//bFight = false;
	
	iGoal = iFarGoal = i_CurrWP = -1;
	
	//WeaponDefs.Init();
	
	memset(&(current_weapon), 0, sizeof(current_weapon));
	memset(&(m_rgAmmo), 0, sizeof(m_rgAmmo));
	
	need_to_initialize = FALSE;
}

void CBotDOD :: UpdateSkill(void){
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
	
	MakeName(szName,name,bot_skill,f_Aggressivity);
	
	SET_CLIENT_KEYVALUE( clientIndex, infobuffer, "name", szName );
}

long CBotDOD :: WeaponModel2ID(const char *szModel){
	if(!szModel||!*szModel)
		return -1;
	
	if(FStrEq(szModel,"models/p_k98kl.mdl"))
		return DOD_WEAPON_KAR;
	if(FStrEq(szModel,"models/p_k98s.mdl"))
		return DOD_WEAPON_KAR;
	if(FStrEq(szModel,"models/p_m1carb.mdl"))
		return DOD_WEAPON_M1CARBINE;
	if(FStrEq(szModel,"models/p_bar.mdl"))
		return DOD_WEAPON_BAR;
	if(FStrEq(szModel,"models/p_colt.mdl"))
		return DOD_WEAPON_COLT;
	
	if(FStrEq(szModel,"models/p_garand.mdl"))
		return DOD_WEAPON_GARAND;
	if(FStrEq(szModel,"models/p_gewehr43.mdl"))
		return DOD_WEAPON_GEWEHR;
	if(FStrEq(szModel,"models/p_grenade.mdl"))
		return DOD_WEAPON_HANDGRENADE;
	if(FStrEq(szModel,"models/p_kar.mdl"))
		return DOD_WEAPON_KAR;
	if(FStrEq(szModel,"models/p_luger.mdl"))
		return DOD_WEAPON_LUGER;
	if(FStrEq(szModel,"models/p_mp40.mdl"))
		return DOD_WEAPON_MP40;
	if(FStrEq(szModel,"models/p_mp44.mdl"))
		return DOD_WEAPON_MP44;
	if(FStrEq(szModel,"models/p_spring.mdl")
		||FStrEq(szModel,"models/p_springl.mdl"))
		return DOD_WEAPON_SPRING;
	if(FStrEq(szModel,"models/p_stick.mdl"))
		return DOD_WEAPON_STICKGRENADE;
	
	if(FStrEq(szModel,"models/p_tommy.mdl"))
		return DOD_WEAPON_THOMPSON;
	if(FStrEq(szModel,"models/p_youthk.mdl"))
		return DOD_WEAPON_GERKNIFE;
	if(FStrEq(szModel,"models/p_amerk.mdl"))
		return DOD_WEAPON_AMERKNIFE;
	if(FStrEq(szModel,"models/p_30cal.mdl"))
		return DOD_WEAPON_30CAL;
	
	if(FStrEq(szModel,"models/p_mg42bu.mdl")
		||FStrEq(szModel,"models/p_mg42bd.mdl")
		||FStrEq(szModel,"models/p_mg42pr.mdl"))
		return DOD_WEAPON_MG42;
	if(FStrEq(szModel,"models/p_mg34bu.mdl")
		||FStrEq(szModel,"models/p_mg34bd.mdl")
		||FStrEq(szModel,"models/p_mg34pr.mdl"))
		return DOD_WEAPON_MG42;
	
	if(listenserver_edict)FakeClientCommand(listenserver_edict,"say %s",szModel);
	
	return -1;
}

edict_t *CBotDOD :: FindEnemy(){
/*
#define FL_ES_DELAY_SG		(1<<0)
#define FL_ES_DELAY_PRONE	(1<<1)
#define FL_ES_DELAY_DUCK	(1<<2)
#define FL_ES_DELAY_FIRE	(1<<3)
#define FL_ES_DELAY_NFIRE	(1<<4)
	*/
	if(f_SearchEnemy < gpGlobals->time){
		f_SearchEnemy = gpGlobals->time + 1.0/_SEARCHEN;
		int i;
		float nearestdistance;  // max distance of enemy
		static float is_team_play;
		Vector vecEnd;
		edict_t *pNewEnemy = NULL;
		bool bSTM;
		edict_t *pPlayer;
		edict_t *edict_old = 0;
		FEItem *FFEP;
		bool bNotNew = false,bBehindSG = false;
		float _ERSTime = 5.0;
		
		// set max distance
		if(HasSniper()){
			nearestdistance = f_InDistance * 2.0 ;		// would mean, they can view more far away stuff instantly
		}
		else{
			nearestdistance = f_InDistance;		// their view is a bit limited ...
		}
		
		// process short term memory ...
		bSTM = STMem.Process(pEdict,bSeeThru);
		
		// test if the bot is blinded
		if(f_blinded_time < gpGlobals->time){
			// search the world for players...
			is_team_play = 1.0;
			
			for (i = 0; i < gpGlobals->maxClients; i++){
				FFEP = &FFE[i];
				edict_old = FFEP->pEdict;
				pPlayer = INDEXENT(i + 1);
				FFEP->pEdict = 0;
				
				// skip invalid players and skip self (i.e. this bot)
				if ((pPlayer) && (!pPlayer->free) && (pPlayer != pEdict))
				{
					// skip this player if not alive (i.e. dead or dying)
					if (!IsAlive(pPlayer)){
						continue;
					}
					
					// is team play enabled?
					int player_team = UTIL_GetTeam(pPlayer);
					
					// don't target your teammates...
					if (bot_teamnm == player_team){
						continue;
					}
					
					vecEnd = pPlayer->v.origin + pPlayer->v.view_ofs;
					
					// see if bot can see the player...
					if (FInViewCone( &vecEnd, pEdict ) &&
						FVisible( vecEnd, pEdict ))
					{
						FFEP->pEdict = pPlayer;
						
						FFEP->fDistance= (pPlayer->v.origin - pEdict->v.origin).Length();
						
						/*bBehindSG = IsBehindSG(pPlayer);
						if(bBehindSG == false
						&& FFEP->bBehindSG == true){		// enemy is't anymore covered by SG
						FFEP->fTime2seeit -= TI_ES_DELAY_SG;
						}
						
						  if(bBehindSG == true
						  && FFEP->bBehindSG == false){
						  FFEP->fTime2seeit += 1.0;
					}*/
						
						if(FFEP->pEdict->v.button & (IN_ATTACK|IN_ATTACK2)){
							// bot's firing
							if(FFEP->lFlags & FL_ES_DELAY_PRONE){
								FFEP->fTime2seeit = gpGlobals->time + f_ReactionTime;
								FFEP->lFlags &=~ FL_ES_DELAY_PRONE;
							}
							else if(FFEP->lFlags & FL_ES_DELAY_SG){
								FFEP->fTime2seeit -= (TI_ES_DELAY_SG - 1.f);
								FFEP->lFlags &=~ FL_ES_DELAY_SG;
							}
						}
						
						FFEP->bBehindSG = bBehindSG;
						
						//if(pEdictPlayer&&FFEP->bBehindSG)WaypointDrawBeam(pEdictPlayer,pEdict->v.origin,pPlayer->v.origin,10,0,200,200,200,200,0);
						
						if(!edict_old){
							// this is a new edict :=)
							bNotNew = TEq(gpGlobals->time,FFEP->fRecogn,_ERSTime)
								||TEq(gpGlobals->time,FFEP->fLastSeen,_ERSTime);
							
							FFEP->fRecogn = gpGlobals->time;
							if(bNotNew){
							}
							else{		// really new :D
							}
							// reset flags
							FFEP->lFlags = 0;
							// test on SGs
							//bBehindSG = IsBehindSG(pPlayer); // already some time above :)
							if(bBehindSG){		// is behind SG
								FFEP->lFlags|=FL_ES_DELAY_SG;
							}
							if(FFEP->pEdict->v.button & IN_DUCK){
								FFEP->lFlags|=FL_ES_DELAY_DUCK;
							}
							if(IsProne(FFEP->pEdict)){
								FFEP->lFlags|=FL_ES_DELAY_PRONE;
							}
							if(FFEP->pEdict->v.button & (IN_ATTACK|IN_ATTACK2)){
								FFEP->lFlags|=FL_ES_DELAY_FIRE;
							}
							else{
								FFEP->lFlags|=FL_ES_DELAY_NFIRE;
							}
							
							FFEP->fTime2seeit = 0.0;
							
							if(FFEP->fDistance < nearestdistance || bNotNew){
							}
							else{
								FFEP->fTime2seeit += (FFEP->fDistance - nearestdistance)/1000.0 * f_TPD / 1000.0;
							}
							if(FFEP->lFlags & FL_ES_DELAY_SG){
								FFEP->fTime2seeit += TI_ES_DELAY_SG;
							}
							if(FFEP->lFlags & FL_ES_DELAY_DUCK){
							}
							if(FFEP->lFlags & FL_ES_DELAY_PRONE){
								FFEP->fTime2seeit += (FFEP->fDistance - f_ProneInDistance)/1000.0 * f_ProneTPD / 1000.0;
							}
							
							if(bNotNew){
								FFEP->fTime2seeit /= 3.f;
							}
						}
						
						// check if it's in short term mem and evtl. add it
						STMem.Update(pPlayer);
					}
					else{		// not visible, so set the edict_t * pointer to NULL
						FFEP->pEdict = 0;
					}
				}
			}
			
			float fMin = 100000;
			
			for (i = 0; i < gpGlobals->maxClients; i++){
				FFEP = &FFE[i];
				if(FFEP->pEdict){
					if (FFEP->fRecogn + FFEP->fTime2seeit <= gpGlobals->time){
						FFEP->fLastSeen = gpGlobals->time;
						if(FFEP->fDistance < fMin){
							fMin = FFEP->fDistance;
							pNewEnemy = FFEP->pEdict;
						}
					}
				}
				else{
				}
			}
		}
		
		if (pNewEnemy)
		{
			// face the enemy
			//HeadToward(pNewEnemy->v.origin);
			STMem.UpdateP(pNewEnemy);
		}
		
		return (pNewEnemy);
	}
	else{
		if(pBotEnemy){
			if(IsAlive(pBotEnemy)){
				return pBotEnemy;
			}
			else
				pBotEnemy = 0;
		}
	}
	return 0;
}

bool CBotDOD :: IsProne(edict_t *pEdict){
	return pEdict->v.view_ofs.z < 0.0f;
}

void CBotDOD :: Init (void){
	fDODFlagU = 0;
	UpdateFlags();
	
	//f_5th = RANDOM_FLOAT(0.0,0.5);
	bBleeding = false;
	bSlowed = false;
	i1C = RANDOM_LONG(1,8);
	f_noWP = 0;
	f_SearchEnemy = 0;
	pNEn = 0;
	pNTeam = 0;
	f_ResetWP = 0;
	i_CurrWP = -1;
	iNearWP = -1;
	ResetWPlanning();
	iWantedDiv = 0;
	f_shootbox = 0;
	f_AimHead = 0.1;
	f_ducktill = 0;
	f_CheckSpEnt = 0;
	//f_Camp = 0;
	f_noCamp = 0;
	f_LTSFIP = RANDOM_FLOAT(0.0f,5.0f) + gpGlobals->time ;
	f_IOrigD = 200.0;
	f_LookTo = 0;
	//	f_GoBack = 0;
	f_Camp = 0;
	f_StrCamp = 0;
	f_DefCamp = 0;
	pLastGlobalKill = 0;
	
	v_Offset = Vector(0,0,0);
	fViewSpeedYaw = 0;
	fViewSpeedPitch = 0;
	fIdealAnglePitch = 0;
	fIdealAngleYaw = 0;
	fAngleSpeedPitch = 0;
	fAngleSpeedYaw = 0;
	pEdict->v.idealpitch = 0;
	pEdict->v.ideal_yaw = 0;
	pEdict->v.v_angle = pEdict->v.angles = Vector(90,90,0);
	
	f_PauseShoot = 0;	// start again to shot
	f_DenyWChange = 0;
	i_avoidstrafe = 1;
	iOnLadder = 0;
	f_end_use_ladder_time=0;
	f_start_use_ladder_time=0;
	f_dont_check_stuck = 0;
	f_bot_stuck = 0;
	lStuckC = 0;
	f_dont_avoid=0;
	//	f_avoid_strafe = 0;
	f_RWKnife = gpGlobals->time + 120.0;	// just some time
	f_blinded_time=0;
	f_gotohostage = 0;
	f_Bored = gpGlobals->time + RANDOM_LONG(long(-TIMETOBEBORED*2/3),long(TIMETOBEBORED/2));
	//	f_Hide = 0;
	//	pHidefrom = 0;
	//	f_GotoHidingPlace = 0;
	f_BoredTill = 0;
	//CChat.f_LastChat = 0;
	//l_ChatEvent = 0;
	f_SuspRem = 0;
	f_Aggressivity = 0;
	//	f_GotoHidingPlace = 0;	// don't start going to VHidingPlace
	f_HidingTime = 5.0;
	f_NNNUpdate = 0;			// begin next fight with nn update ...
	pEKilled = NULL;
	GOrder.lTypeoG = 0;
	//f_moveinacc = 1.0;
	
	bot_IRadioM.lMessage = 0;		// reset radio stuff
	f_UsedRadio = 0;
	
	Action.lAction &= (BA_BUYZONE|BA_DEFKIT);// no state def @ beginning of round except buyzone
	Action.pREdict = 0;
	
	Task.Reset();
	
	Ordered.lAction = 0;		// no ordered players @ start
	Ordered.pREdict = 0;
	
	//szChatText[0] = 0;
	
	pWPAMPlay = 0;
	bReplay = false;
	
	// delete short term memory
	STMem.f_NUpdate = 0;
	LTMem.f_NUpdate = 0;
	STMem.iCount = 0;
	f_LastFight = gpGlobals->time;
	
	fViewSpeedYaw=0;		// speed of changing view yaw
	fViewSpeedPitch=0;		// speed of changing view pitch
	
	// update skill
	UpdateSkill();
	
	// reset nnlog
	//BotResetNNLog(pBot);
	
	// handle pref weapon stuff
	if(lPWeapon<=0)
		lPWeapon = WeaponClass2ID(szPWeapon);
	
	// just sometimes more teamplay
	if(RANDOM_LONG(0,100) < 50){
		//Task.AddTask(BT_ROAMTEAM,gpGlobals->time + 60.0,0,0);
	}
	
	f_pronetill = 0;
	f_changedprone = 0;
	iStamina = 100;
}

bool CBotDOD :: HandleOrders(void){		// this fucntion is called every second
	// look if the related bot is still alive
	if(Task.current){		// bot got order
		CTaskItem * CurrentTask = Task.current;
		if(CurrentTask->lType & BT_GOTO){
			if(CurrentTask->lAdd == -1){
				Task.NextTask();
			}
		}
		else if((CurrentTask->lType & BT_FOLLOW)
			|| (CurrentTask->lType & BT_COVER)){
			if(CurrentTask->p){			// is an edict stored ?
				if(ENTINDEX((edict_t *)CurrentTask->p) == -1){
					Task.NextTask();
				}
				else if(!IsAlive((edict_t *)CurrentTask->p)){		// player who send order is dead
					Task.NextTask();
				}
			}
		}
		else if(CurrentTask->lType & BT_ROAMTEAM){
			if(CurrentTask->p){			// is an edict stored ?
				if(ENTINDEX((edict_t *)CurrentTask->p) == -1){
					Task.NextTask();
				}
				else if(!IsAlive((edict_t *)CurrentTask->p)){		// player who send order is dead
					Task.NextTask();
				}
				else{		// there is an edict and it's alive
					// test if other moved ... otherwise del this order
					float fDistance = (CurrentTask->VT - ((edict_t *)CurrentTask->p)->v.origin).Length();
					if(fDistance < 15.0){
						Task.NextTask();
					}
					else{
						CurrentTask->VT = ((edict_t *)CurrentTask->p)->v.origin;
					}
				}
			}
			else{
				// just get a new teammember
				Task.current->p = GetRandomPlayer(pEdict,bot_teamnm,1);
				CBotBase *pB = UTIL_GetBotPointer((edict_t*)Task.current->p);
				if(pB){
					if(pB->Task.SearchT( BT_ROAMTEAM|BT_COVER) != -1){	// just to make it easy ... although i don't like this
						Task.NextTask();
						return false;
					}
				}
				if(!Task.current->p){		// found no edict
					Task.NextTask();
					return false;
				}
			}
		}
		else if(CurrentTask->lType & BT_GUARD){
			if(CurrentTask->p){
				if(ENTINDEX((edict_t *)CurrentTask->p) == -1){
					Task.NextTask();
				}
				else if(!IsAlive((edict_t *)CurrentTask->p)){		// player who send order is dead
					Task.NextTask();
				}
			}
			if(CurrentTask->fLive2 < gpGlobals->time)
				Task.NextTask();
		}
		else if(CurrentTask->lType & BT_HOLDPOS){
			if(CurrentTask->fLive2 < gpGlobals->time)
				Task.NextTask();
			
			Task.AddTask(BT_CAMP, CurrentTask->fLive2,0,0,0);
		}
		else if(CurrentTask->lType & BT_WAIT4TM8){
			if(CurrentTask->fLive2 < gpGlobals->time){
				Task.NextTask();
				Task.RemoveT(BT_CAMP);
				f_ducktill = gpGlobals->time;
			}
			else{
				edict_t *pN;
				float fMin = 0;
				pN = GetNearestPlayer(pEdict,bot_teamnm,fMin,true,true);
				if((pN->v.origin-pEdict->v.origin).Length()<150.0){
					Task.NextTask();
					Task.RemoveT(BT_CAMP);
					f_ducktill = gpGlobals->time;
					return true;
				}
			}
			Task.AddTask(BT_CAMP,CurrentTask->fLive2,0,0,0);
		}
		/*		else if(CurrentTask->lType & BT_PAUSE){
		if(CurrentTask->lAdd == 0){
		CurrentTask->lAdd = 1;
		CurrentTask->fLive2 += gpGlobals->time;
		}
		else if(CurrentTask->lAdd == 1){
		f_Pause = CurrentTask->fLive2;
		if(CurrentTask->lType & BT_CROUCH){
		f_ducktill = CurrentTask->fLive2;
		}
		if(CurrentTask->fLive2 < gpGlobals->time){
		Task.NextTask();
		
		  f_Pause = gpGlobals->time;
		  }
		  }
		  else
		  Task.NextTask();
		}*/
		else if(CurrentTask->lType & BT_RELOAD){				// reload weapon
			if(CurrentTask->lAdd == 0){
				CurrentTask->lAdd = 1;
				CurrentTask->fLive2 = gpGlobals->time + 3.0;
				f_DenyWChange = CurrentTask->fLive2;
				
				lButton |= IN_RELOAD;
				
				//Task.AddTask(BT_PAUSE,gpGlobals->time + 3.0,0,0,0);					// pause for some time	
				f_Pause = gpGlobals->time + 2.0;
			}
			else if(CurrentTask->lAdd == 1){
				if(CurrentTask->fLive2 < gpGlobals->time){
					Task.NextTask();
				}
			}
			else
				Task.NextTask();
		}
	}
	return true;
}

void CBotDOD :: PreprocessTasks(void){
	if(Task.current){
		if(false){
		}
		else
			CBotBase::PreprocessTasks();
	}
}

void CBotDOD :: Think5th(void){
	TraceResult tr2;

	// see how far bot has moved since the previous position...
	v_diff = v_prev_origin - pEdict->v.origin;
	moved_distance = v_diff.Length();
	v_diff_angles = UTIL_VecToAngles(v_diff);
	
	// save current position as previous
	v_prev_origin = pEdict->v.origin;
	i1C--;
	
	// look if there are tasks in the task system to be deleted
	Task.CheckTLife();
	
	// process faked sound system
	ReactOnSound();
	
	// intercepted radio command - handling
	ReactOnRadio();
	
	// search for interesting locations
	//if(f_LastFight<gpGlobals->time - 3.0){
	if(!(Action.lAction&BA_FIGHT)){
		SearchForIL();
	}
	
	// do we want backup ???
	if((HasSniper() )							// has sniper, but no aug or sg552
		&&!(Ordered.lAction & BO_COVER)
		&&!Task.Important()
		&& f_UsedRadio < gpGlobals->time - 5.0f	// don't do that too often
		&& f_LastRadio<gpGlobals->time - 4.0				// don't enerve others
		&& f_noCamp < gpGlobals->time			// don't do that just after capming
		&& g_fRoundTime > 3.0
		&& RANDOM_LONG(0,100) < 50){						// not every time hackhack for double radio mess per frame
		if(HasPrimary()){
			if(!IsCWeaponPrimary()){
				if(f_RWKnife>gpGlobals->time)
					f_RWKnife = gpGlobals->time-.1f;
				Change2Weapon(HasPrimary());
			}
			else{
				float fMin;
				GetNearestPlayer(pEdict,bot_teamnm,fMin);
				if(fMin < 500 && CVAR_BOOL(jb_msgradio)){
					SendRadioCommand(RADIO_COVER_ME);
					Ordered.lAction |= BO_COVER;
					
					if(f_RWKnife>gpGlobals->time)
						f_RWKnife = gpGlobals->time-.1f;
				}
				else{
					f_noCamp = gpGlobals-> time + 1;	// to avoid to many tries
				}
			}
		}
	}
	
	// is susp location (where bot heard sth) visible ?
	if(Action.lAction & BA_SUSPLOC){				
		//Vector VTo = pEdict->v.origin+pEdict->v.view_ofs+1.0*pEdict->v.velocity;
		//UTIL_TraceLine(VTo,VSuspEn,ignore_monsters,pEdict->v.pContainingEntity,&tr);
		UTIL_TraceLine(pEdict->v.origin,VSuspEn,ignore_monsters,pEdict->v.pContainingEntity,&tr2);
		
		if(/*tr.flFraction == 1.0
			&&*/ tr2.flFraction == 1.0){				// i.e. visible
			VLookTo = VSuspEn;
		f_LookTo = gpGlobals->time +1.0;
		
		if(FInViewCone(&VSuspEn,pEdict))
			Action.lAction &= ~BA_SUSPLOC;			// del susploc flag
		}
	}
	
	// look for planted bomb message
	if(long(g_bBombPlanted<<5) != (Action.lAction & BA_BOMBPL) && RANDOM_LONG(0,100) < 20){ // bomb planted changed
		//b_BombPlanted = g_bBombPlanted;
		g_bBombPlanted?Action.lAction|=BA_BOMBPL:Action.lAction&=~BA_BOMBPL;
		
		if(g_bBombPlanted){
			//DEBUG_CLIENTCOMMAND(pEdict,"say noticed it");
			Chat.l_ChatEvent |= E_BombPl;
			Task.RemoveT(BT_CAMP);
			ResetWPlanning();
			Task.Reset();
		}
	}
	
	if(pLastGlobalKill){
		if(UTIL_ClientIndex(pLastGlobalKill) != -1
			&&UTIL_GetTeam(pLastGlobalKill) != bot_teamnm
			&& FVisible(pLastGlobalKill->v.origin,pEdict)){
			if(f_LastFight < gpGlobals->time - 1.0){
				HeadToward(pLastGlobalKill->v.origin);
				if(d_Manner < .1){
					Task.AddTask(BT_CAMP,gpGlobals->time + RANDOM_FLOAT(1.5,4.5),0,0,0);
					f_Aggressivity += 2.0;
					sLookToChange = 1;
					InitCamp();
					f_CaLooktoWP = gpGlobals->time + 2.5f;
					GoProne(true,10.0);
				}
			}
		}
		pLastGlobalKill = 0;
	}
	
	// make enemy down message
	if(pEKilled){
		if(UTIL_GetTeam(pEKilled) != bot_teamnm){		// no tk
			Chat.l_ChatEvent |= E_Kill;
			
			pELVictim = pEKilled;
			//				f_GotoHidingPlace = 0;	// don't try to hide ...
			Task.RemoveT(BT_HIDE);
			ResetWPlanning();
			f_Aggressivity += 1.0;		// be more agressive
			
			if(CVAR_BOOL(jb_msgenemydown)){	// flag's default is true
				if(f_UsedRadio < gpGlobals->time - _RADIO_FREQ
					&& CVAR_BOOL(jb_msgradio)){
					SendRadioCommand(RADIO_ENEMY_DOWN);
				}
			}
		}
		else{														// tk
			if(f_TKPunish < gpGlobals->time - 2.0){
				strcpy(Chat.szChatText,"sorry");						// just say sorry
				Chat.f_SayTime = gpGlobals->time + RANDOM_FLOAT(0,2.0);
				Chat.f_LastChat = gpGlobals->time + RANDOM_FLOAT(15,15);
			}
		}
		pEKilled = 0;
	}
	
	// sometimes try to reload
	// at least 5 sec after last fight
	if(RANDOM_LONG(0,100) < 7
		&&!(Action.lAction & BA_PLANT)
		&& current_weapon.iId>=0&&current_weapon.iId<32
		&& (IsCWeaponPrimary()|| IsCWeaponSecondary())
		&& Task.SearchT(BT_RELOAD) == -1
		&& f_LastFight + 2 < gpGlobals->time
		&& current_weapon.iId>=0&&current_weapon.iId<32
		&& float(current_weapon.iClip) / float(WeaponDefs.ipClipSize[DOD_DLL][current_weapon.iId]) < .3){
		Task.AddTask(BT_RELOAD,gpGlobals->time + 3.0,0,0,0);
		//cout << "reload" << endl;
	}
	
	/*if(i_FOV != 90
	&&f_LastFight + 1.f < gpGlobals->time){
	lButton|=IN_ATTACK2;
		}*/
	
	// lower aggresivity value
	f_Aggressivity *= .985f;
	
	// lower offset
	//v_Offset = v_Offset*.73;
	v_Offset = pEdict->v.punchangle * (float(bot_skill)/100.0) * .3;
	
	if(!(Action.lAction & BA_FIGHT))
		f_AimHead *= .8;
	
	if(Task.SearchT(BT_HIDE) != -1
		&&!TEq(gpGlobals->time,f_StrCamp,10)
		&&!(Task.current && Task.current->lType & BT_HOLDPOS)){
		edict_t *pNearest;
		int iTm8;
		iTm8 = UTIL_FightingAgainst(pEdict,bot_teamnm==CS_TEAM_CT?CS_TEAM_TE:CS_TEAM_CT,&pNearest,true);		// only duckin bots
		if(iTm8>1){
			if(f_UsedRadio < gpGlobals->time - _RADIO_FREQ
				&&CVAR_BOOL(jb_msgradio)
				&&Task.SearchT(BT_HIDE) != -1){
				SendRadioCommand(RADIO_GOGOGO);
				//f_Hide = gpGlobals->time - .01f;
			}
		}
	}
	
	HandleOrders();
	
	if(!i1C){				// every second one time	---------------------------------------------------------------------------------
		Think1();
	}
	
}

void CBotDOD :: Think1(void){
	edict_t *pent;


	i1C = 5;
	
	UpdateFlags();
	
	AdaptAvoid();
	
	//			DistanceSight();
	
	if(bSlowed){
		//DEBUG_CLIENTCOMMAND(pEdict,"say stamina prone");
		GoProne(true,5);
		Task.AddTask(BT_CAMP, gpGlobals->time + 2.0,0,0,0);
		InitCamp();
		bSlowed = false;
	}
	
	if(bBleeding){
		FakeClientCommand(pEdict,"stopbleed");
		bBleeding = false;
	}
	
	// is there something in the LTM ?
	float fDistance;
	for(long lschl = 0;lschl < _MAX_LTMEM;lschl++){
		if(LTMem.FMItem[lschl].bUsed){
			//DEBUG_CLIENTCOMMAND(pEdict,"say ltmprone");
			if(LTMem.FMItem[lschl].lType == LTM_KILLED){
				fDistance = (LTMem.FMItem[lschl].VAddI2 - pEdict->v.origin).Length();
				if ( fDistance < 200 ){
					GoProne(true,10);
				}
			}
			else if(LTMem.FMItem[lschl].lType == LTM_KILL){
				fDistance = (LTMem.FMItem[lschl].VAddI2 - pEdict->v.origin).Length();
				if ( fDistance < 100 ){
					GoProne(true,10);
				}
			}
		}
	}
	
	if(RANDOM_LONG(0,100) < 20){
		edict_t *pToUse;
		TestOnButtonWay(&pToUse);
		
		if(pToUse){
			int iDest = WaypointFindNearest(VecBModelOrigin(VARS(pToUse)),pEdict,400,bot_teamnm);
			if(iDest != -1&&Task.SearchT(BT_GOBUTTON) == -1){
				Task.AddTask(BT_GOTO|BT_GOBUTTON,-1,iDest,pToUse,0);
			}
			//DEBUG_CLIENTCOMMAND(pEdict,"say wanting to go where pressing a button is needed");
		}
	}
	
	// check pronetill variable
	if(f_pronetill > 0 && f_changedprone < gpGlobals->time - 2.0){
		if( IsProne(pEdict) && f_pronetill < gpGlobals->time){
			FakeClientCommand(pEdict,"prone");
			f_changedprone = gpGlobals->time;
			//DEBUG_CLIENTCOMMAND(pEdict,"say a prone +");
		}
		else if( !IsProne(pEdict) && f_pronetill > gpGlobals->time){
			FakeClientCommand(pEdict,"prone");
			f_changedprone = gpGlobals->time;
			//DEBUG_CLIENTCOMMAND(pEdict,"say a prone -");
		}
	}
	
	// chat
	if(CVAR_BOOL(jb_chat))
		Chat.Talk(this);
	
	// handle ordered stuff
	if(Ordered.lAction){		// bot got ordered
		if(Ordered.pREdict){			// is an edict stored ?
			if(!IsAlive(Ordered.pREdict)){		// player who got order is dead
				Ordered.lAction = 0;				// erase order
				Ordered.pREdict = 0;				// reset pointer
			}
		}
	}
	
	// change the weapon not directly after en is away
	if(f_LastFight + .3f < gpGlobals->time&&!(Action.lAction&BA_PLANT)){
		if(IsCWeaponKnife()
			&& HasSecondary()
			&& m_rgAmmo[weapon_defs[HasSecondary()].iAmmo1]>1){
			Change2Weapon(HasSecondary());
		}
		else if(IsCWeaponSecondary()
			&& HasPrimary()
			&&!GOrder.lTypeoG
			&& m_rgAmmo[weapon_defs[HasPrimary()].iAmmo1]>1){
			Change2Weapon(HasPrimary());
		}
	}
	
	// slthough it's never used, it should be updated some time
	if(bot_team == 5){
		bot_team = UTIL_GetTeam(pEdict)+1;
	}
	
	// fix nonshooting after map change
	if(f_PauseShoot > gpGlobals->time + 3.0f)
		f_PauseShoot = 0.0;
	
	// is this place normally dangerous ?
	int iNearWP = i_CurrWP;
	
	if(iNearWP != -1){
		if(WPStat[iNearWP].lKill > WPStat.lKillMax/2){
			if(f_RWKnife>gpGlobals->time){
				f_RWKnife = gpGlobals->time - .01;
			}
			if(!Task.Important()
				&& f_noCamp < gpGlobals->time - 5.0
				&&((g_fRoundTime < 30.0&&g_fRoundTime > 10.0) || RANDOM_LONG(0,100) < 20)
				&&d_Manner<0){
				float fMin = 200;
				pent = GetNearestPlayer(pEdict,bot_teamnm,fMin,false,false);
				if(pent
					&&(pent->v.origin-pEdict->v.origin).Length() < 600.0
					&& pent->v.velocity.Length() > 100.0){
					Task.AddTask(BT_WAIT4TM8,gpGlobals->time + 10.0,0,0,0);
					//DEBUG_CLIENTCOMMAND(pEdict,"say dangerous area");
				}
			}
		}
	}
	// avoid to get stuck trying to reach a waypoint
	if((f_lastWP + MAXTIMEPERWP*((pEdict->v.button&IN_DUCK)?1:2)) < gpGlobals->time){
		if(Task.SearchT(BT_CAMP) == -1
			&&f_ducktill < gpGlobals->time
			&&f_Pause<gpGlobals->time){
			iGoal = -1;
			iFarGoal = -1;
			f_lastWP = gpGlobals -> time;
			i_CurrWP = -1;
			
			Jump();		// if the bot is longer time blocked it should jump to get of fuckin ladders at least sometimes - although it may hurt :)
			
			//DEBUG_CLIENTCOMMAND(pEdict,"say resetting wpstuff");
		}
	}
	
	if(bNNInitError){
		FakeClientCommand(pEdict,"say NNs couldn't be loaded correctly, i.e. nn.br2 and/or nnc.br2");
	}
}

void CBotDOD :: Think(void){
	//f_max_speed = 200;
	//cout << pEdict->v.view_ofs.Length()<<endl;
	//cout << STRING(pEdict->v.netname) << endl;
	edict_t *pent=0;
	static char item_name[200];
	int index = 0;
	Vector v_diff;             // vector from previous to current location
	TraceResult tr;
	TraceResult tr2;
	bool found_waypoint;
	
	pEdict->v.flags |= FL_FAKECLIENT;
	
	/*if (name[0] == 0)  // name filled in yet?
	strcpy(name, STRING(pEdict->v.netname));*/
	
	//pEdict->v.button = 0;
	lButton = 0;
	f_strafe = 0.0f;		// reset strafing to 0.0
	
	// if the bot hasn't selected stuff to start the game yet, go do that...
	if (not_started)
	{
		HandleMenu();
		
		RUN_PLAYER_MOVE( pEdict, pEdict->v.v_angle, 0.0,
			0, 0, pEdict->v.button, 0, byte(g_msecval));
		
		FixIdealYaw();				// this fixes a bug, which does onl occurr in de_dust, cs_gamma_assault etc.
		
		return;
	}
	
	HandleMenu();
	
	bot_teamnm = UTIL_GetTeam(pEdict);
	
	// if the bot is dead, randomly press fire to respawn...
	if ((pEdict->v.health < 1) || (pEdict->v.deadflag != DEAD_NO))
	{
		if(CVAR_BOOL(jb_chat))
			Chat.Talk(this);
		
		if (need_to_initialize)
			SpawnInit();
		
		RUN_PLAYER_MOVE( pEdict, pEdict->v.v_angle, f_move_speed,
			0, 0, lButton, 0, byte(g_msecval));
		
		return;
	}
	
	if(bRSInit){		// delete ways at and other vars at start of round
		Init();
		
		g_bBombPlanted = false;
		g_fBombExplode = -1;
		
		ResetWPlanning();
		
		//DEBUG_CLIENTCOMMAND(pEdict,"say inited");
		bRSInit = false;		// don't process this a second time this round
		
		for(int i = 0;i < 32;i++){
			f_ES[i] = gpGlobals->time;
		}
	}
	
	// set this for the next time the bot dies so it will initialize stuff
	need_to_initialize = TRUE;
	
	f_move_speed = pEdict->v.maxspeed;
	
	if (g_b5th){								// this is every .2 s the case
		Think5th();
	}
	else{
		//b5th=false;
		moved_distance = 2.0;
	}
	
	// save current position as previous
	v_prev_origin = pEdict->v.origin;
	
	// see, if bot hears the steps of a running player, and react on it
	/*if(RANDOM_LONG(0,100) < 20)			// don't hear everything or let's say save cpu - time
	BotReactOnSound(pBot);*/
	
	if ((pEdict->v.waterlevel == 2) ||
		(pEdict->v.waterlevel == 3))
	{
		// turn towards ideal_pitch by pitch_speed degrees
		lButton|=IN_FORWARD;
	}
	
	// turn towards ideal_pitch by pitch_speed degrees
	old_pitch_degrees = pitch_degrees;
	pitch_degrees = ChangePitch();
	ChangeBodyPitch();
	// turn towards ideal_yaw by yaw_speed degrees
	old_yaw_degrees = yaw_degrees;
	yaw_degrees = ChangeYaw();
	ChangeBodyYaw();
	
	PreprocessTasks();
	
	/*if(Task.current){
	pBotOldEnemy = pBotEnemy;
	pBotEnemy = FindEnemy();
	
	  if (pBotEnemy != NULL){			// does an en exist ?
	  if(current_weapon.iId == CS_WEAPON_C4){		// dont try to attack with this
	  if(HasPrimary())			// change to primary
	  Change2Weapon(HasPrimary());
	  else							// or to secondary
	  Change2Weapon(HasSecondary());
	  }
	  LastEnemyOrigin = pBotEnemy->v.origin;	// store location of last en
	  Fight();										// ... and fight against it
	  }
	  
		int iFarGoal=-1,iWaytedDiv=0;
		if(Task.current->lType & BT_CAMP){
		Camp();
		}
		else if(Task.current->lType & BT_GOTO){
		found_waypoint = HeadTowardWaypoint();
		}
		}
		else{
	}*/
	
	if (false){
	}
	else	// else handle movement related actions...
	{		// see if bot can find an enemy...
		if(pBotEnemy) pBotOldEnemy = pBotEnemy;
		pBotEnemy = FindEnemy();
		
		if (pBotEnemy != NULL ){			// does an en exist ?
			
			if(Task.current
				&& (Task.current->lType&BT_IGNOREENEMY)){
				if(Task.current->p != pBotEnemy){
					Task.NextTask();
				}
				LastEnemyOrigin = pBotEnemy->v.origin;	// store location of last en
				Fight();										// ... and fight against it
				
				
				
				if(!iOnLadder){
					DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin + Vector(0,50,50),10,0,00,200,000,200,10);
					VLookTo = pBotEnemy -> v.origin;
					f_LookTo = gpGlobals->time + 2.0;
					//f_GoBack =
				}
				
				lButton &=~ (IN_DUCK|IN_JUMP);
				f_move_speed = pEdict->v.maxspeed;
				f_ducktill = 0;
				f_strafe = 0;
				if(i_CurrWP != -1)
					VRunningTo = waypoints[i_CurrWP].origin;
				//Action.lAction |= BA_FIGHT;
			}
			else{
				LastEnemyOrigin = pBotEnemy->v.origin;	// store location of last en
				Fight();										// ... and fight against it
			}
		}
		
		if (pBotEnemy == NULL || (Task.current&&(Task.current->lType&BT_IGNOREENEMY)))
		{	// no enemy or blinded ...
			// if blinded slow down
			if(f_blinded_time > gpGlobals->time){
				f_move_speed /= 3.0f;		// slow down
				f_shootbox = f_blinded_time;
				
				if(RANDOM_LONG(0,100) > 99){
					Jump();
				}
			}
			// no enemy, let's just wander around...
			
			// (defensive) bot went out of view of enemy and succeeded
			if(Task.current){
			/*if(Task.current -> lType & BT_HIDE){
			//float fThisHide;
			//f_GotoHidingPlace = 0;		// reset time to go to hiding place
			Task.NextTask();
			InitCamp();					// init camping
			
			  if(f_Hide < gpGlobals->time - 5.0f){
			  Task.AddTask(BT_CAMP,gpGlobals->time + f_HidingTime,0,0,0);	// camp for _HIDECAMPTIME s
			  f_Hide = gpGlobals->time + f_HidingTime;	// duck all that time ...
			  }
			  if(RANDOM_LONG(0,100) < 6
			  && !(Action.lAction & BA_TKAVOID)){
			  if(f_UsedRadio < gpGlobals->time - _RADIO_FREQ
			  &&CVAR_BOOL(jb_msgradio)){
			  SendRadioCommand(NEED_ASSISTANCE);
			  }
			  }
			  Action.lAction &=~ BA_TKAVOID;			// in case it was a TKAVOID, delete the flag
			  // search for a wp out of sight of the fuckin enemy
			  
				int index=-1,i;
				float distance;
				long lEnt = -1; 
				if(Task.current)
				lEnt = UTIL_ClientIndex(Task.current->p);
				
				  for (i=0; i < num_waypoints; i++)
				  {
				  if ((waypoints[i].flags & W_FL_DELETED) == W_FL_DELETED)
				  continue;  // skip any deleted waypoints
				  
					if (waypoints[i].flags & W_FL_AIMING)
					continue;  // skip any aiming waypoints
					
					  if(!FVisible(waypoints[i].origin,pEdict))		// skip if not visible ...
					  continue;
					  
						if(lEnt!=-1
						&&Task.current->p){		// is always BT_HIDE
						if(FVisible(waypoints[i].origin,Task.current->p)){
						continue;
						}
						}
						
						  // skip this waypoint if it's team specific and teams don't match...
						  if ((waypoints[i].flags & W_FL_TEAM_SPECIFIC) &&
						  ((waypoints[i].flags & W_FL_TEAM) != bot_teamnm))
						  continue;
						  
							distance = (waypoints[i].origin - pEdict->v.origin).Length();
							if(distance > 150)
							continue;
							
							  index = i;
							  break;
							  }
							  if(index != -1){		// that is found waypoint which is in view of pEdict but not in view of enemy
							  i_CurrWP = index;
							  f_Hide = gpGlobals->time;
							  Task.RemoveT(BT_CAMP);
							  }
			}*/
			}
			if(STMem.iCount
				&& dCombatNNIn[IAmmo] > 0.0
				&& f_shootbox < gpGlobals->time - 5.0f
				&&(IsCWeaponMachineGun()
				|| IsCWeaponRifle()
				|| IsCWeaponSniper()) ){
				if(IsCWeaponMachineGun()){
					f_shootbox = gpGlobals->time + 4.0;
				}
				else
					f_shootbox = gpGlobals->time + 1.5;
			}
			
			if(Action.lAction & BA_FIGHT
				&& (Task.current&&!(Task.current->lType&BT_IGNOREENEMY))){			// was last time a fight ??
				Action.lAction &=~ BA_FIGHT;			// del flag
				
				//f_noWP = gpGlobals->time + RANDOM_FLOAT(.5,1);
				
				if(((d_Manner >= 0
					&& pBotOldEnemy
					&& IsAlive(pBotOldEnemy)
					&& RANDOM_LONG(0,100) < 70)
					|| RANDOM_LONG(0,100) < 20)
					&& Task.SearchT(BT_HUNT) == -1){			// hunt the enemy down :D
					Task.AddTask(BT_HUNT,gpGlobals->time + 40.0f,0,pBotOldEnemy,0);
					ResetWPlanning();
				}
				// don't use WP for some time
				//f_noWP = gpGlobals->time + 1.0;
				i_CurrWP = -1;
				Action.lAction &=~ BA_SUSPLOC;	// forget susp loc
			}
			
			if(STMem.iCount
				&&!(Action.lAction&BA_SUSPLOC)){
				if((STMem[0].VCalcedOrigin-pEdict->v.origin).Length()<500.0f){
					VSuspEn = STMem[0].VCalcedOrigin;
					f_SuspRem = gpGlobals->time;
					Action.lAction|=BA_SUSPLOC;
				}
			}
			
			if(STMem.iCount
				&& (Task.current && !(Task.current->lType&BT_IGNOREENEMY))
				&& f_shootbox > gpGlobals->time 
				&& !STMem[0].bSolid){
				DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin + Vector(0,50,0),10,0,0,000,200,200,10);
				VATurnTo(STMem[0].VCalcedOrigin - pEdict->v.origin);
				
				UTIL_TraceLine(pEdict->v.origin+pEdict->v.view_ofs,STMem[0].VCalcedOrigin,dont_ignore_monsters,pEdict,&tr);
				if(// tr.flFraction < .9
					//UTIL_ClientIndex(tr.p9Hit) != -1
					UTIL_ClientIndex(tr.pHit) != -1			// hits a teamm8
					&& tr.pHit != pBotEnemy){// line is probably fract by enemy
					f_strafe = 50;
				}
				else{
					FireWeapon(STMem[0].VCalcedOrigin);
					f_noWP = gpGlobals->time + .2;
					//DEBUG_CLIENTCOMMAND(pEdict,"say shooting throught");
				}
			}
			
			if(!GOrder.lTypeoG){
				// has grenade as current weapon ???
				if(IsCWeaponGrenade()
					&& !GOrder.lTypeoG
					&& RANDOM_LONG(0,100) < 2){
					if(HasSecondary())
						Change2Weapon(HasSecondary());
				}
			}
			
			// reset angles
			if ((pEdict->v.waterlevel != 2) &&  // is bot NOT under water?
				(pEdict->v.waterlevel != 3))
			{
				// reset pitch to 0 (level horizontally)
				pEdict->v.idealpitch = 1;
				//pEdict->v.v_angle.x = 1;
				fIdealAnglePitch = 1;
				fIdealAnglePitch = -pEdict->v.v_angle.x/3;
				fIdealAngleYaw = pEdict->v.v_angle.y;
			}
			else{
				fIdealAnglePitch = -pEdict->v.v_angle.x;
				fIdealAngleYaw = pEdict->v.v_angle.y;
				lButton |= IN_FORWARD;
			}
			
			pEdict->v.v_angle.z = 0;  // reset roll to 0 (straight up and down)
			pEdict->v.angles.z = 0;
			
			// search for important ents (c4, hostages ...)
			if(!GoToSpEnt()){		// if there are no special ents
				if(!Bored()){		// if bots not bored ...
					found_waypoint = HeadTowardWaypoint();
					
					// check if the bot is on a ladder ...
					//(pEdict->vmovetype != FLY) && ((pEdict->vflags & FL_ONGROUND) != FL_ONGROUND)
					//if (pEdict->v.movetype == MOVETYPE_FLY)
					if ((pEdict->v.movetype == MOVETYPE_FLY)
						/*|| !(pEdict->v.flags & FL_ONGROUND)*/)
					{
						lButton |= IN_FORWARD;
						//f_move_speed /= 2.0;
						// check if bot JUST got on the ladder...
						if ((f_end_use_ladder_time + 1.0) < gpGlobals->time)
							f_start_use_ladder_time = gpGlobals->time;
						
						// go handle the ladder movement
						OnLadder();
						
						f_LookTo = 0 ;
						
						f_dont_avoid = gpGlobals->time + 1.0;
						f_dont_check_stuck = gpGlobals->time + 1.0;
						f_IgnoreFall = gpGlobals->time + 1.0;
						f_end_use_ladder_time = gpGlobals->time;
					}
					else
					{
						// check if the bot JUST got off the ladder...
						pELadder = 0;
						if ((f_end_use_ladder_time + 1.0) > gpGlobals->time)
						{
							iOnLadder = 0;
						}
					}
					
					// if the bot isn't headed toward a waypoint...
					if (found_waypoint == FALSE)
					{	
						Wander();
					}
					else{	// found waypoint and stuck ?? - This should be handled elsewhere
					}
				}// end bored
			}// end no special ent found
			
			AvoidStuck();
			
			AvoidCollision();
			
			// should the bot pause for a while here? ( looks kind of intelligent )
			if (!Task.Important()
				&& RANDOM_LONG(1, 200) <= 2)
			{
				// set the time that the bot will stop "pausing"
				f_Pause = gpGlobals->time + 0.5;
			}
		}
	}
	
	HandleGOrder();
	
	if(!bReplay && 
		f_LookTo + .3 > gpGlobals->time){		// f_LookTo code
		float f_newspeed;
		float f_newstrafe;
		float f_Dot;
		float f_DotMin = -.5;
		Vector VLDiff,VRDiff;
		
		//if(f_GoBack>gpGlobals->time){
		f_DotMin = -1.1f;
		//}
		
		//VLDiff = Vector(cos(pEdict->v.v_angle.y * M_PI / 180.0),sin(pEdict->v.v_angle.y * M_PI / 180.0),0);
		UTIL_MakeVectors(pEdict->v.v_angle);
		VLDiff = gpGlobals->v_forward;
		VLDiff.z = 0;
		VLDiff = VLDiff.Normalize();
		
		VRDiff = VRunningTo - pEdict->v.origin;
		VRDiff.z = 0.0;
		VRDiff = VRDiff.Normalize();
		
		f_Dot = DotProduct(VRDiff,VLDiff);
		if(f_Dot > f_DotMin){
			Vector VCross = CrossProduct(VLDiff,VRDiff);
			
			f_newspeed = f_Dot * f_move_speed;
			float fTemp = f_move_speed*f_move_speed-f_newspeed*f_newspeed;
			if(fTemp > 0)
				f_newstrafe = sqrt(fTemp);
			else
				f_newstrafe = 0;
			
			if(VCross.z>0.0)
				f_newstrafe = -f_newstrafe;
			
			f_strafe = f_newstrafe;
			f_move_speed = f_newspeed;
			
			if(f_LookTo > gpGlobals->time){
				HeadToward(VLookTo);
			}
			else{
				HeadToward(VRunningTo);
			}
		}
		else{
			f_LookTo = gpGlobals->time;
		}
	}
	/* ---------------------------------- 'anti fall code' ---------------------------------- */
	// just add the movement vector to the bot's origin and look down, how deep it is
	// this first stuff is always calculated, even if the bots hasn't to check falling
	// because it is also used for the avoidance code
	
	UTIL_MakeVectors(pEdict->v.v_angle);
	Vector v_view = gpGlobals->v_forward;
	//Vector v_view = Vector(cos(pEdict->v.v_angle.y * M_PI / 180.0),sin(pEdict->v.v_angle.y * M_PI / 180.0),0);
	//v_view.Normalize();
	Vector v_perp = gpGlobals->v_right;
	
	v_perp.z = 0;		/// why  ??? z isn't apparently 0 sometimes
	
	v_perp = v_perp.Normalize();		// vector to the right
	float fspeed;
	Vector v_comp = (v_view * f_move_speed + v_perp * f_strafe);
	fspeed = v_comp.Length();
	if(fspeed)
		v_comp = v_comp * (1.f/fspeed);
	
	/* code for avoidance of obstables */
	v_comp = v_comp + v_avoid * .7f;
	v_comp = v_comp.Normalize();
	
	f_strafe = CrossProduct(v_comp,v_view).z * fspeed;
	f_move_speed =(DotProduct(v_comp,v_view)>0?1:-1)*sqrt(fspeed*fspeed-f_strafe*f_strafe);
	/* end code for avoidance of obstables */
	
	if(f_IgnoreFall < gpGlobals->time
		&& !bReplay
		&& iOnLadder == 0){			// don't have fear in front of ladders
		
		float fDistTC = _DSTDOWN;
		
		if(lButton & IN_JUMP){			// check longer dist when jumping
			fDistTC = 130.0;
		}	
		Vector v_check = pEdict->v.origin + v_comp * fDistTC;
		/*Vector v_check = pEdict->v.origin + pEdict->v.velocity.Normalize() * fDistTC;*/
		Vector v_down = v_check - Vector( 0,0, 300);		// straight down 1000 units
		
		UTIL_TraceLine(v_check,v_down, ignore_monsters,pEdict->v.pContainingEntity, &tr);		// is that too deep ?
		if(tr.flFraction == 1.f){
			UTIL_TraceLine(pEdict->v.origin,v_check, ignore_monsters,pEdict->v.pContainingEntity, &tr);	// to check if bot is in front of a crate e.g.
			if(tr.flFraction == 1.f){
				f_move_speed = -f_move_speed;
				f_strafe = - f_strafe;
				lButton &=~ IN_JUMP;	// don't jump
			}
			else{
				if(RANDOM_LONG(0,100) < 25)
					Jump();	// jump
			}
		}
	}
	else{
	}
	/* end anti fall code*/
	
	if(f_ducktill > gpGlobals->time){	// ducktill -> useful, cause the bots need a time to duck
		lButton |= IN_DUCK;
	}
	
	if (f_Pause > gpGlobals->time)  // is the bot "paused"? (faking intelligence and thinking time)
		f_move_speed = 0;  // don't move while pausing
	
	HandleReplay();
	
	// save the previous speed (for checking if stuck)
	prev_speed = f_move_speed;
	
	// copying lButton from bot_t to pEdict->v.button
	pEdict->v.button = lButton;			// copy the new pressed buttons to the right location
	
	RUN_PLAYER_MOVE( pEdict, pEdict->v.v_angle, f_move_speed,
		f_strafe, 0, pEdict->v.button, 0, byte(g_msecval));
	
	//BotFixIdealYaw(pEdict);
	
	return;
}


/*void CBotDOD :: Think(void)
{
edict_t *pent=0;
static char item_name[200];
int index = 0;
Vector v_diff;             // vector from previous to current location
//Vector v_diff_angles;
//float yaw_degrees,pitch_degrees;
TraceResult tr;
TraceResult tr2;
bool found_waypoint;

pEdict->v.flags |= FL_FAKECLIENT;

if(bNNInitError){
FakeClientCommand(pEdict,"say NNs couldn't be loaded correctly, i.e. nn.br2 and/or nnc.br2");
}

// TheFatal - START from Advanced Bot Framework (Thanks Rich!)

// adjust the millisecond delay based on the frame rate interval...
if (msecdel <= gpGlobals->time)
{
msecdel = gpGlobals->time + 0.5;
if (msecnum > 0)
msecval = 450.0/msecnum;
msecnum = 0;
}
else
msecnum++;

if (msecval < 5)    // don't allow msec to be less than 5...
msecval = 5;

if (msecval > 100)  // ...or greater than 100
msecval = 100;

// TheFatal - END

//pEdict->v.button = 0;
lButton = 0;
f_strafe = 0.0f;		// reset strafing to 0.0
//f_move_speed = 0.0f;
bot_teamnm = UTIL_GetTeam(pEdict);

// if the bot hasn't selected stuff to start the game yet, go do that...
if (not_started)
{
HandleMenu();

RUN_PLAYER_MOVE( pEdict, pEdict->v.v_angle, 0.0,
0, 0, pEdict->v.button, 0, msecval);

FixIdealYaw();				// this fixes a bug, which does onl occurr in de_dust, cs_gamma_assault etc.

return;
}

HandleMenu();

// if the bot is dead, randomly press fire to respawn...
if ((pEdict->v.health < 1) || (pEdict->v.deadflag != DEAD_NO))
{
if(CVAR_BOOL(jb_chat))
Chat.Talk(this);

if (need_to_initialize)
SpawnInit();

if (RANDOM_LONG(1, 100) > 50)
lButton = IN_ATTACK;

RUN_PLAYER_MOVE( pEdict, pEdict->v.v_angle, f_move_speed,
0, 0, lButton, 0, msecval);

return;
}
// search ents

#ifdef _DEBUG
if(HasSecondary() && !HasPrimary() &&RANDOM_LONG(0,100) < 20)
LOG_MONEY(UTIL_VarArgs("%s\t%i\n\0",STRING(pEdict->v.netname),m_iBotMoney));
#endif

if(bRSInit){		// delete ways at and other vars at start of round
Init();

iGoal = -1;
iFarGoal = -1;
// don't forget the freezetime !! -> freezetime is done by setmaxspeed
//f_pause_time  = gpGlobals->time + g_mp_freezetime->value;

//DEBUG_CLIENTCOMMAND(pEdict,"say inited");
bRSInit = false;		// don't process this a second time this round

for(int i = 0;i < 32;i++){
f_ES[i] = gpGlobals->time;
}
}

// set this for the next time the bot dies so it will initialize stuff
need_to_initialize = TRUE;

f_move_speed = pEdict->v.maxspeed;

if (g_b5th){								// this is every .2 s the case


// see how far bot has moved since the previous position...
v_diff = v_prev_origin - pEdict->v.origin;
moved_distance = v_diff.Length();
if(moved_distance>300){
Init();
}
v_diff_angles = UTIL_VecToAngles(v_diff);

// save current position as previous
v_prev_origin = pEdict->v.origin;
i1C--;

// process faked sound system
ReactOnSound();

// intercepted radio command - handling
ReactOnRadio();

// search for interesting locations
if(f_LastFight<gpGlobals->time - 3.0){
SearchForIL();
}

if(pLastGlobalKill){
if(UTIL_GetTeam(pLastGlobalKill) == bot_teamnm
&& FVisible(pLastGlobalKill->v.origin,pEdict)){
if(f_LastFight < gpGlobals->time - 1.0){
HeadToward(pLastGlobalKill->v.origin);
if(d_Manner < .1){
Task.AddTask(BT_CAMP, gpGlobals->time + RANDOM_FLOAT(1.5,4.5),0,0,0);
f_Aggressivity += 2.0;
sLookToChange = 1;
InitCamp();
f_CaLooktoWP = gpGlobals->time + 2.5f;
GoProne(true,10.0);
}
}
}
pLastGlobalKill = 0;
}

// check pronetill variable
if(f_pronetill > 0 && f_changedprone < gpGlobals->time - 2.0){
if( IsProne(pEdict) && f_pronetill < gpGlobals->time){
FakeClientCommand(pEdict,"prone");
f_changedprone = gpGlobals->time;
//DEBUG_CLIENTCOMMAND(pEdict,"say a prone +");
}
else if( !IsProne(pEdict) && f_pronetill > gpGlobals->time){
FakeClientCommand(pEdict,"prone");
f_changedprone = gpGlobals->time;
//DEBUG_CLIENTCOMMAND(pEdict,"say a prone -");
}
}

// change the weapon not directly after en is away
if(f_LastFight + 1.0f < gpGlobals->time){
if(current_weapon.iId == DOD_WEAPON_KNIFE
&& HasSecondary()
&& m_rgAmmo[weapon_defs[HasSecondary()].iAmmo1]>0){
Change2Weapon(HasSecondary());
}
if(IsCWeaponSecondary()
&& HasPrimary()
&&!GOrder.lTypeoG
&& m_rgAmmo[weapon_defs[HasPrimary()].iAmmo1]>0){
Change2Weapon(HasPrimary());
}
}
// sometimes try to reload
// at least 5 sec after last fight
if(RANDOM_LONG(0,100) < 7
&&!(Action.lAction & BA_PLANT)
&& f_LastFight + 4 < gpGlobals->time
&& float(current_weapon.iClip) / float(weapon_defs[current_weapon.iId].iAmmoClip) < .3)
lButton |= IN_RELOAD;

// lower aggresivity value
f_Aggressivity *= .98;

// lower offset
//v_Offset = v_Offset*.73;
v_Offset = pEdict->v.punchangle * float(bot_skill)/100.0;

if(!(Action.lAction & BA_FIGHT))
f_AimHead *= .8;

if(f_Hide>gpGlobals->time
&&!TEq(gpGlobals->time,f_StrCamp,10)
&&!Task.Important()){
edict_t *pNearest;
int iTm8;
iTm8 = UTIL_FightingAgainst(pEdict,bot_teamnm==CT?TE:CT,&pNearest,true);		// only duckin bots
if(iTm8>1){
if(f_UsedRadio < gpGlobals->time - _RADIO_FREQ
&&CVAR_BOOL(jb_msgradio)
&&Task.SearchT(BT_HIDE) != -1){

}
}
}

if(!i1C){				// every second one time	---------------------------------------------------------------------------------
i1C = 5;

UpdateFlags();

HandleOrders();

AdaptAvoid();

// chat
if(CVAR_BOOL(jb_chat))
Chat.Talk(this);

// handle ordered stuff
if(Ordered.lAction){		// bot got ordered
if(Ordered.pREdict){			// is an edict stored ?
if(!IsAlive(Ordered.pREdict)){		// player who got order is dead
Ordered.lAction = 0;				// erase order
Ordered.pREdict = 0;				// reset pointer
}
}
}

if(bSlowed){
//DEBUG_CLIENTCOMMAND(pEdict,"say stamina prone");
GoProne(true,5);
Task.AddTask(BT_CAMP, gpGlobals->time + 2.0,0,0,0);
InitCamp();
bSlowed = false;
}

if(bBleeding){
FakeClientCommand(pEdict,"stopbleed");
bBleeding = false;
}

// slthough it's never used, it should be updated some time
if(bot_team == 5){
bot_team = UTIL_GetTeam(pEdict)+1;
}
// fix nonshooting after map change
if(f_PauseShoot > gpGlobals->time + 2.0f)
f_PauseShoot = 0.0;

// is there something in the LTM ?
float fDistance;
for(long lschl = 0;lschl < _MAX_LTMEM;lschl++){
if(LTMem.FMItem[lschl].bUsed){
//DEBUG_CLIENTCOMMAND(pEdict,"say ltmprone");
if(LTMem.FMItem[lschl].lType == LTM_KILLED){
fDistance = (LTMem.FMItem[lschl].VAddI2 - pEdict->v.origin).Length();
if ( fDistance < 200 ){
GoProne(true,10);
}
}
else if(LTMem.FMItem[lschl].lType == LTM_KILL){
fDistance = (LTMem.FMItem[lschl].VAddI2 - pEdict->v.origin).Length();
if ( fDistance < 100 ){
GoProne(true,10);
}
}
}
}

// is this place normally dangerous ?
int iNearWP = WaypointFindNearest(pEdict,1000,bot_teamnm);
if(iNearWP != -1){
if(WPStat[iNearWP].lKill > WPStat.lKillMax*3/2){
if(f_pronetill<gpGlobals->time){
GoProne(true,10.0);
}
}
}

// avoid to get stuck trying to reach a waypoint
if(f_lastWP + MAXTIMEPERWP < gpGlobals->time){
iGoal = -1;
//iFarGoal = -1;
f_lastWP = gpGlobals -> time;
}
}

}
else{
//b5th=false;
moved_distance = 2.0;
}

// save current position as previous
v_prev_origin = pEdict->v.origin;

// see, if bot hears the steps of a running player, and react on it


if ((pEdict->v.waterlevel == 2) ||
(pEdict->v.waterlevel == 3))
{
// turn towards ideal_pitch by pitch_speed degrees
lButton|=IN_FORWARD;
}

// turn towards ideal_pitch by pitch_speed degrees
old_pitch_degrees = pitch_degrees;
pitch_degrees = ChangePitch();
ChangeBodyPitch();
// turn towards ideal_yaw by yaw_speed degrees
old_yaw_degrees = yaw_degrees;
yaw_degrees = ChangeYaw(  );
ChangeBodyYaw();


if (0){
}
else	// else handle movement related actions...
{		// see if bot can find an enemy...
pBotOldEnemy = pBotEnemy;
pBotEnemy = FindEnemy();

if (pBotEnemy != NULL){			// does an en exist ?
LastEnemyOrigin = pBotEnemy->v.origin;	// store location of last en
Fight();										// ... and fight against it
}
else{	// no enemy or blinded ...
// if blinded slow down
if(f_blinded_time > gpGlobals->time){
f_move_speed /= 3.0f;		// slow down
f_shootbox = f_blinded_time;
if(RANDOM_LONG(0,100) > 95){		// 'spack ab'
if(IsCWeaponGrenade()){
lButton |= IN_ATTACK;
}
}
if(RANDOM_LONG(0,100) > 99){
lButton |= IN_JUMP;
}
}
// no enemy, let's just wander around...

// (defensive) bot went out of view of enemy and succeeded
if(Task.current&&Task.current->lType & BT_HIDE){
//float fThisHide;
//f_GotoHidingPlace = 0;		// reset time to go to hiding place
Task.RemoveT(BT_HIDE);
InitCamp();					// init camping

if(f_Hide < gpGlobals->time - 5.0f){
Task.AddTask(BT_CAMP,  gpGlobals->time + f_HidingTime,0,0,0);	// camp for _HIDECAMPTIME s
f_Hide = gpGlobals->time + f_HidingTime;	// duck all that time ...
}
if(RANDOM_LONG(0,100) < 6
&& !(Action.lAction & BA_TKAVOID)){
if(f_UsedRadio < gpGlobals->time - _RADIO_FREQ
&&CVAR_BOOL(jb_msgradio)){

}
}
Action.lAction &=~ BA_TKAVOID;			// in case it was a TKAVOID, delete the flag
// search for a wp out of sight of the fuckin enemy
if(Task.current&&Task.current->lType & BT_HIDE){
int index=-1,i;
float distance;
for (i=0; i < num_waypoints; i++)
{
if ((waypoints[i].flags & W_FL_DELETED) == W_FL_DELETED)
continue;  // skip any deleted waypoints

if (waypoints[i].flags & W_FL_AIMING)
continue;  // skip any aiming waypoints

if(!FVisible(waypoints[i].origin,pEdict))		// skip if not visible ...
continue;

if(Task.current->p)			// all the time bt_hide
if(FVisible(waypoints[i].origin,Task.current->p))
continue;

// skip this waypoint if it's team specific and teams don't match...
if ((waypoints[i].flags & W_FL_TEAM_SPECIFIC) &&
((waypoints[i].flags & W_FL_TEAM) != bot_teamnm))
continue;

distance = (waypoints[i].origin - pEdict->v.origin).Length();
if(distance > 150)
continue;

index = i;
break;
}
if(index != -1){		// that is found waypoint which is in view of pEdict but not in view of enemy
i_CurrWP = index;
f_Hide = gpGlobals->time;
Task.RemoveT(BT_CAMP);
}
}
}

if(STMem.iCount
&& dCombatNNIn[IAmmo] > 0.0
&& f_shootbox < gpGlobals->time - 5.0f
&&((IsCWeaponPrimary() == CS_WEAPON_M249) || 
IsCWeaponRifle() || 
IsCWeaponSniper()) ){
if(IsCWeaponPrimary() == CS_WEAPON_M249){
f_shootbox = gpGlobals->time + 4.0;
}
else
f_shootbox = gpGlobals->time + 1.5;
}

if(Action.lAction & BA_FIGHT){			// was last time a fight ??
Action.lAction &=~ BA_FIGHT;			// del flag

// don't use WP for some time
f_noWP = gpGlobals->time + 1.0;
i_CurrWP = -1;
Action.lAction &=~ BA_SUSPLOC;	// forget susp loc

}

if(STMem.iCount&&!(Action.lAction&BA_SUSPLOC)){
if((STMem[0].VCalcedOrigin-pEdict->v.origin).Length()<500.0f){
VSuspEn = STMem[0].VCalcedOrigin;
f_SuspRem = gpGlobals->time;
Action.lAction|=BA_SUSPLOC;
}
}

if(STMem.iCount
&& f_shootbox > gpGlobals->time 
&& !STMem[0].bSolid){
VATurnTo(STMem[0].VCalcedOrigin - pEdict->v.origin);

UTIL_TraceLine(pEdict->v.origin+pEdict->v.view_ofs,STMem[0].VCalcedOrigin,dont_ignore_monsters,pEdict,&tr);
if(// tr.flFraction < .9
//UTIL_ClientIndex(tr.p9Hit) != -1
ENTINDEX(tr.pHit) != 0			// hits a teamm8
&& tr.pHit != pBotEnemy){// line is probably fract by enemy
f_strafe = 50;
}
else{
FireWeapon(STMem[0].VCalcedOrigin);
f_noWP = gpGlobals->time + .2;
//DEBUG_CLIENTCOMMAND(pEdict,"say shooting throught");
}
}


// reset angles
if ((pEdict->v.waterlevel != 2) &&  // is bot NOT under water?
(pEdict->v.waterlevel != 3))
{
// reset pitch to 0 (level horizontally)
pEdict->v.idealpitch = 1;
//pEdict->v.v_angle.x = 1;
fIdealAnglePitch = 1;
fIdealAnglePitch = -pEdict->v.v_angle.x/3;
fIdealAngleYaw = pEdict->v.v_angle.y;
}
else{
fIdealAnglePitch = -pEdict->v.v_angle.x;
fIdealAngleYaw = pEdict->v.v_angle.y;
lButton |= IN_FORWARD;
}

pEdict->v.v_angle.z = 0;  // reset roll to 0 (straight up and down)
pEdict->v.angles.z = 0;

//fIdealAnglePitch = pEdict->v.v_angle.x;



// is susp location (where bot heard sth) visible ?
if(Action.lAction & BA_SUSPLOC){				
Vector VTo = pEdict->v.origin+pEdict->v.view_ofs+1.0*pEdict->v.velocity;
UTIL_TraceLine(VTo,VSuspEn,ignore_monsters,pEdict->v.pContainingEntity,&tr);
UTIL_TraceLine(pEdict->v.origin,VTo,ignore_monsters,pEdict->v.pContainingEntity,&tr2);
//HeadToward(pEdict,VSuspEn);
if(tr.flFraction == 1.0
&& tr2.flFraction == 1.0){				// i.e. visible
VLookTo = VSuspEn;
f_LookTo = gpGlobals->time +1.0;
//DEBUG_CLIENTCOMMAND(pEdict,"say susp");
//DEBUG_CLIENTCOMMAND(pEdict,"say looking to susp look");
if(FInViewCone(&VSuspEn,pEdict))
//bSawSuspLoc = true;
Action.lAction &= ~BA_SUSPLOC;			// del susploc flag
}
}
// search for important ents (c4, hostages ...)
if(!GoToSpEnt()){		// if there are no special ents
if(!Bored()){		// if bots not bored ...
found_waypoint = HeadTowardWaypoint();

// check if the bot is on a ladder ...
//(pEdict->vmovetype != FLY) && ((pEdict->vflags & FL_ONGROUND) != FL_ONGROUND)
//if (pEdict->v.movetype == MOVETYPE_FLY)
if ((pEdict->v.movetype == MOVETYPE_FLY)
)
{
lButton |= IN_FORWARD;
//f_move_speed /= 2.0;
// check if bot JUST got on the ladder...
if ((f_end_use_ladder_time + 1.0) < gpGlobals->time)
f_start_use_ladder_time = gpGlobals->time;

// go handle the ladder movement
OnLadder();

f_dont_avoid = gpGlobals->time + 1.0;
f_dont_check_stuck = gpGlobals->time + 1.0;
f_IgnoreFall = gpGlobals->time + 1.0;
f_end_use_ladder_time = gpGlobals->time;
}
else
{
// check if the bot JUST got off the ladder...
pELadder = 0;
if ((f_end_use_ladder_time + 1.0) > gpGlobals->time)
{
iOnLadder = 0;
}
}

// if the bot isn't headed toward a waypoint...
if (found_waypoint == FALSE)
{	
// the next stuff just test if the bot runs against a wall
Vector v_diff_norm = v_diff.Normalize();		// normalized vector of the bot's last move
v_diff_norm.z=0;								// don't look at pitch - just make it 2D
Vector v_view_norm;

double dAngle = pEdict->v.angles.y * M_PI / 180.0;

v_view_norm.x = cos(dAngle);	// convert the view from angles to a normalized vector
v_view_norm.y = sin(dAngle);
v_view_norm.z = 0;

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
lButton |= IN_JUMP;
lButton |= IN_DUCK;
}
}
else if(l_cState == 1){			// ... or turn later

pEdict->v.ideal_yaw = v_diff_angles.y;

l_cState ++;
}
else{
l_cState = 0;
}
FixIdealYaw();
}
else
l_cState =0;
}
else{	// found waypoint and stuck ?? - This should be handled elsewhere
}
}// end bored
}// end no special ent found

AvoidStuck();

AvoidCollision();

// should the bot pause for a while here? ( looks kind of intelligent )
if (!Task.Important()
&& RANDOM_LONG(1, 200) <= 2
&& f_Pause < gpGlobals->time-2.0)
{
// set the time that the bot will stop "pausing"
f_Pause = gpGlobals->time + 0.5;
}
}
}
if(!bReplay && f_LookTo + .3 > gpGlobals->time){
float f_newspeed;
float f_newstrafe;
float f_Dot;
float f_DotMin = -.5;
Vector VLDiff,VRDiff;

if(f_GoBack>gpGlobals->time){
f_DotMin = -1.0f;
}

//VLDiff = VLookTo - pEdict->v.origin;
VLDiff = Vector(cos(pEdict->v.v_angle.y * M_PI / 180.0)
,sin(pEdict->v.v_angle.y * M_PI / 180.0),0);
//VLDiff.z = 0.0;
//VLDiff = VLDiff.Normalize();

VRDiff = VRunningTo - pEdict->v.origin;
VRDiff.z = 0.0;
VRDiff = VRDiff.Normalize();

f_Dot = DotProduct(VRDiff,VLDiff);
if(f_Dot > f_DotMin){
Vector VCross = CrossProduct(VLDiff,VRDiff);

//snprintf(szBuffer,sizeof(szBuffer),"-| %f ; %f",f_move_speed,f_Dot);

f_newspeed = f_Dot * f_move_speed;
f_newstrafe = sqrt(f_move_speed*f_move_speed-f_newspeed*f_newspeed);
if(VCross.z>0.0)
f_newstrafe = - f_newstrafe;

f_strafe = f_newstrafe;
f_move_speed = f_newspeed;

if(f_LookTo > gpGlobals->time){
HeadToward(VLookTo);
}
else{
HeadToward(VRunningTo);
}

//DEBUG_CLIENTCOMMAND(pEdict,"say -| %f ; %f",f_newspeed,f_newstrafe);
}
else{
f_LookTo = gpGlobals->time;
}
}
if(f_IgnoreFall < gpGlobals->time
&& !bReplay
&& iOnLadder == 0){			// don't have fear in front of ladders
//float flast_height;
float fDistTC = _DSTDOWN;
bool bflag;

if(lButton & IN_JUMP){			// check longer dist when jumping
fDistTC = 130.0;
}

pent=0;			// fix de_aztec bug
bflag=false;
while (pent = UTIL_FindEntityInSphere( pent, pEdict->v.origin,80)){
if(FStrEq(STRING(pent->v.classname),"func_illusionary")){
bflag=true;
break;
}
}
if(!bflag){
// just add the movement vector to the bot's origin and look down, how deep it is

Vector v_view = Vector(cos(pEdict->v.v_angle.y * M_PI / 180.0)
,sin(pEdict->v.v_angle.y * M_PI / 180.0),0);
v_view.Normalize();
Vector v_perp = CrossProduct(v_view,Vector(0,0,1));

v_perp.z = 0;		/// why  ??? z isn't apparently 0 sometimes

v_perp = v_perp.Normalize();		// vector to the right
Vector v_comp = (v_view * f_move_speed + v_perp * f_strafe).Normalize();
Vector v_check = pEdict->v.origin + v_comp * fDistTC;

Vector v_down = v_check - Vector( 0,0, 300);		// straight down 1000 units

UTIL_TraceLine(pEdict->v.origin,v_check, ignore_monsters,pEdict->v.pContainingEntity, &tr);
}
}

if(f_ducktill > gpGlobals->time){	// ducktill -> useful, cause the bots need a time to duck
lButton |= IN_DUCK;
}

if(f_Hide > gpGlobals->time){	// Hide -> do everything to hide ...
lButton |= IN_DUCK;
f_move_speed = 0;
}

if (f_Pause > gpGlobals->time)  // is the bot "paused"? (faking intelligence and thinking time)
f_move_speed = 0;  // don't move while pausing

if(bReplay){
float fOffset = gpGlobals->time - fStartADVM;
int i = 0;
FILE *fhds;fhds=fopen("rec_move_a.txt","a");if(fhds){fprintf(fhds,"%f - %i\n",fOffset,pWPAMPlay->iNum);fclose(fhds);}

while(i < pWPAMPlay->iNum){
if(pWPAMPlay->Rec[i].fTime > fOffset){		// i.e. that's the last recorded item
// copy stuff from recorded data to bot
if(pWPAMPlay->Rec[i].lButton&IN_DUCK){
DEBUG_CLIENTCOMMAND(pEdict,"say duckin from advm");
}
//DEBUG_CLIENTCOMMAND(pEdict,"say -");
lButton = pWPAMPlay->Rec[i].lButton;
pEdict->v.origin = pWPAMPlay->Rec[i].v_origin;
pEdict->v.angles = pWPAMPlay->Rec[i].v_angles;
pEdict->v.v_angle = pWPAMPlay->Rec[i].v_v_angle;
pEdict->v.velocity = pWPAMPlay->Rec[i].v_velocity;
f_move_speed = pWPAMPlay->Rec[i].v_velocity.Length();

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
f_strafe = -150;
}
else if(lButton & IN_MOVERIGHT){
f_strafe = 150;
}

FILE *fhds;fhds=fopen("rec_move.txt","a");if(fhds){fprintf(fhds,"%i - %f - %f\n",\
pWPAMPlay->iNum,fOffset,pWPAMPlay->Rec[i].fTime);fclose(fhds);}
break;
}
i++;
}
if(!(i < pWPAMPlay->iNum)){
DEBUG_CLIENTCOMMAND(pEdict,"say stoppin advm");
bReplay = false;
}
}

// save the previous speed (for checking if stuck)
prev_speed = f_move_speed;

// copying lButton from bot_t to pEdict->v.button
pEdict->v.button = lButton;			// copy the new pressed buttons to the right location

RUN_PLAYER_MOVE( pEdict, pEdict->v.v_angle, f_move_speed,
f_strafe, 0, pEdict->v.button, 0, msecval);

//BotFixIdealYaw(pEdict);

return;
}*/

bool CBotDOD :: GoProne(bool bProne, float fTime){
	if(f_changedprone < gpGlobals->time - 2.0 && IsProne(pEdict) != bProne){
		f_changedprone = gpGlobals->time;
		FakeClientCommand(pEdict,"prone");
		if(fTime > 0){
			f_pronetill = gpGlobals->time + fTime;
		}
		else 
			f_pronetill = -1;
		return true;
	}
	else
		return false;
}

void CBotDOD :: ReactOnRadio(void){
	if(bot_IRadioM.lMessage != 0){		
		bot_IRadioM.lMessage = 0;		// since now there won't be captured radio messages
	}
}

#define _MAXSTEPDISTANCE	600.0f
#define _MAXSHOOTDISTANCE	1000.0f
#define _CAMPADVANTAGE		2.0f

#define _DELAY_SURPRISED .5f

bool CBotDOD :: ReactOnSound(void){
	float fDistance,
		fVelocity;
	edict_t *pEnt=0,
		*pSSoundSource=0,			// bot can only hear enemy
		*pVSoundSource=0;			// visible sound source
	
	float fMaxStepDistance = _MAXSTEPDISTANCE,
		fMaxShootDistance = _MAXSHOOTDISTANCE,fMaxShootDistanceThis;
	
	if(Task.SearchT(BT_CAMP) != -1){		// bot's camping - so he can hear more ...
		fMaxStepDistance *= _CAMPADVANTAGE;
		fMaxShootDistance *= _CAMPADVANTAGE;
	}
	
	int i;
	float fVDist=100000,fSDist=100000;
	TraceResult tr;
	Vector v_bot_view = pEdict->v.origin + pEdict->v.view_ofs;
	
	for (i = 0; i < gpGlobals->maxClients; i++){
		pEnt = INDEXENT(i + 1);
		
		// skip invalid players and skip self (i.e. this bot)
		if ((pEnt) && (!pEnt->free) && (pEnt != pEdict))
		{
			// skip this player if not alive (i.e. dead or dying)
			if (!IsAlive(pEnt))
				continue;
			
			if(bot_teamnm != UTIL_GetTeam(pEnt)){		// is in other team - ya can do this, cause the (human) players can look at their radar
				fVelocity = Vector(pEnt->v.velocity).Length2D();
				fDistance = Vector(pEdict->v.origin - pEnt->v.origin).Length();
				if(FVisible( v_bot_view, pEnt )){				// do ya see him
					if(fDistance < fMaxStepDistance&&fDistance < fVDist){				// do you hear steps
						if(fVelocity > 220.0f){
							pVSoundSource = pEnt;
							fVDist = fDistance;
							break;
						}
					}
					fMaxShootDistanceThis = fMaxShootDistance;
					
					if(fDistance < fMaxShootDistanceThis){				// do you hear sb shooting ??
						if(pEnt->v.oldbuttons & (IN_ATTACK|IN_ATTACK2)&&fDistance < fVDist){		// attack2 to hear the zoom of awp, p. ex.
							pVSoundSource = pEnt;
							fVDist = fDistance;
							break;
						}
					}
				}
				else{		// -------------------------------------     invisible
					
					if(fDistance < fMaxStepDistance){				// do you hear steps
						if(fVelocity > 220.0f&&fDistance < fVDist){
							pSSoundSource = pEnt;
							fSDist = fDistance;
							// no break - continue searching for visible edict_t's
						}
					}
					fMaxShootDistanceThis = fMaxShootDistance;
					
					if(fDistance < fMaxShootDistanceThis){				// do you hear sb shooting ??
						if(pEnt->v.oldbuttons & (IN_ATTACK|IN_ATTACK2)&&fDistance < fSDist){		// attack2 to hear the zoom of awp, p. ex.
							pSSoundSource = pEnt;
							fSDist = fDistance;
						}
					}
				}
			}
		}
	}
	
	if(pVSoundSource){
		Vector v_Sound = pVSoundSource->v.origin - pEdict->v.origin;
		Vector bot_angles = UTIL_VecToAngles( v_Sound );
		
		pEdict->v.ideal_yaw = bot_angles.y;
		
		FixIdealYaw();
		
		VLookTo = VSuspEn = pVSoundSource->v.origin;		// set, too, for making bot to stop looking @ evtl. old susp loc, if strafing around ( f_LookTo )
		f_SuspRem = gpGlobals->time;
		Action.lAction |= BA_SUSPLOC;
		
		if(!(Action.lAction & BA_FIGHT)){// if bot's already fighting, set no additional delay
			Action.lAction |= BA_FIGHT;			// set to true, to fake fighting and avoid reseting of delay
			//f_Delay = gpGlobals->time + _DELAY_SURPRISED;		// bot's for a moment surprised - or let's say, needs time to detect en
		}
		return true;
	}
	else if(pSSoundSource){
		if(f_SuspRem < gpGlobals->time - 2.0f){		// more than 2 secs ago ...
			// copy current susp edict's origin to pBot's structure
			VSuspEn = pSSoundSource->v.origin;
			f_SuspRem = gpGlobals->time;
			//bSawSuspLoc = false;
			Action.lAction |= BA_SUSPLOC;		// there is a susp loc, bot has't seen
		}
		else{	// bot has already heard sth. - do nthn
		}
		return true;
	}
	else
		return false;
}

void CBotDOD :: SendRadioCommand(int iType){
	switch(iType){
	case RADIO_NEED_ASSISTANCE:
	case RADIO_NEED_BACKUP:
		FakeClientCommand(pEdict,"voice_backup");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_LEFT:
		FakeClientCommand(pEdict,"voice_left");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_RIGHT:
		FakeClientCommand(pEdict,"voice_right");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_GRENADE:
		FakeClientCommand(pEdict,"voice_grenade");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_TAKE_COVER:
		FakeClientCommand(pEdict,"voice_takecover");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_SNIPER:
		FakeClientCommand(pEdict,"voice_sniper");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_MEDIC:
		FakeClientCommand(pEdict,"voice_medic");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_FALL_BACK:
		FakeClientCommand(pEdict,"voice_fallback");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_HOLD_THIS_POSITION:
		FakeClientCommand(pEdict,"voice_hold");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_AFFIRMATIVE:
		FakeClientCommand(pEdict,"voice_yessir");
		f_UsedRadio = gpGlobals->time;
		break;
	default:
		break;
	}
}
