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
#include "Chat.h"
#include "Commandfunc.h"
#include "globalvars.h"
#include "NNWeapon.h"

float g_fGravityAdj = .67f;

CBotCS :: CBotCS(){
	memset(&m_rgpentHostages,0,sizeof(m_rgpentHostages));		// pointer to entities of used hostages
	m_iHostagesUsed = 0;
	m_flBuyTime = 0;
	m_iBotMoney = 0;
}

CBotCS :: ~CBotCS(){
}

void CBotCS :: HandleMenu( void )
{
	// handle Counter-Strike stuff here...

	switch (start_action)
	{
		case MSG_CS_IDLE:
			if (bot_team == 6 && UTIL_PlayerCount(COUNT_HUMAN))
				FakeClientCommand(pEdict, "chooseteam");
			break;

		case MSG_CS_TEAM_SELECT:
			if ((bot_team != 1) && (bot_team != 2) && (bot_team != 5))
			{
				if (!CVAR_BOOL(jb_jointeam))
				{
					if (UTIL_PlayerCount(COUNT_HUMAN))
						bot_team = 5;
					else if (CVAR_GET_FLOAT("allow_spectators") > 0.f)
						bot_team = 6;
					else
						break; // stay unassigned
				}
				else
					bot_team = 5; // auto-assign if invalid
			}
			start_action = MSG_CS_IDLE; // switch back to idle
			if (bot_team != 6) not_started = false;
			FakeClientCommand(pEdict, "menuselect %d", bot_team);
			break;

		case MSG_CS_CT_SELECT: // counter terrorist
		case MSG_CS_T_SELECT:  // terrorist
			if ((bot_class < 1) || (bot_class > 4)) bot_class = RANDOM_LONG(1, 4);  // random if invalid
			start_action = MSG_CS_IDLE;  // switch back to idle
			not_started = false; // bot has now joined the game (doesn't need to be started)
			FakeClientCommand(pEdict, "menuselect %d", bot_class);
			break;

		default:
			;
	}
}

void CBotCS :: SpawnInit(void)
{
	v_prev_origin = Vector(9999.0, 9999.0, 9999.0);
	
	// init some global vars, they are nvtl recaluclated before next think	
	g_msecnum = 0;
	g_msecdel = 0.0;
	g_msecval = 0.0;
	
	bot_health = 0;
	bot_armor = 0;
	bot_weapons = 0;
	m_iBotMoney = 0;
	
	f_max_speed = pEdict->v.maxspeed;
	f_Pause  = gpGlobals->time + g_mp_freezetime->value;
	
	prev_speed = 0.0;  // fake "paused" since bot is NOT stuck
	
	pBotEnemy = NULL;  // no enemy yet
	
	//InitBotVars(pBot);
	
	//bFight = false;
	
	iGoal = iFarGoal = i_CurrWP = -1;
	
	//WeaponDefs.Init();
	
	memset(&(current_weapon), 0, sizeof(current_weapon));
	memset(&(m_rgAmmo), 0, sizeof(m_rgAmmo));
	
	need_to_initialize = FALSE;
}

void CBotCS :: Init(void){
	float ffreeze = g_mp_freezetime->value;
	float fRSS = gpGlobals-> time + ffreeze;
	bot_teamnm = UTIL_GetTeam(pEdict);
	//	f_5th = RANDOM_FLOAT(0.0,0.5);
	i1C = RANDOM_LONG(2,10);
	f_noWP = fRSS + RANDOM_FLOAT(-2,1);
	f_SearchEnemy = fRSS;
	f_NJumpTill=0;
	f_TimeBJumps=2;
	pNEn = 0;
	pNTeam = 0;
	f_ResetWP = 0;
	i_CurrWP = -1;
	ResetWPlanning();
	iWantedDiv = 0;
	iNearWP = -1;
	f_shootbox = 0;
	f_AimHead = 0.1;
	f_ducktill = 0;
	f_CheckSpEnt = 0;
	//f_Camp = 0;
	f_noCamp = 0;
	f_LTSFIP = RANDOM_FLOAT(0.0f,5.0f) + fRSS;
	f_IOrigD = 200.0;
	f_LookTo = 0;
	//	f_GoBack = 0;
	f_Camp = 0;
	f_StrCamp = 0;
	f_DefCamp = 0;
	pLastGlobalKill = 0;
	pLastGlobalKilled = 0;
	m_bGlockBurst = false;
	
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
	m_iHostagesUsed = 0;
	i_avoidstrafe = 1;
	iOnLadder = 0;
	f_end_use_ladder_time=0;
	f_start_use_ladder_time=0;
	f_dont_check_stuck = fRSS;
	f_bot_stuck = 0;
	lStuckC = 0;
	f_dont_avoid = fRSS;
	//f_avoid_strafe = 0;
	if(m_iBotMoney < 2200 && RANDOM_LONG(0,100) < 30){			// if less money then the bots should save some money
		m_flBuyTime = gpGlobals->time + 90.0;
	}
	else
		m_flBuyTime = gpGlobals->time + RANDOM_FLOAT(0,ffreeze/2);
	f_RWKnife = gpGlobals->time + 120.0;	// just some time
	f_blinded_time=0;
	f_gotohostage = 0;
	f_Bored = gpGlobals->time + RANDOM_LONG(long(-TIMETOBEBORED*2/3),long(TIMETOBEBORED/2));
	//f_Hide = 0;
	//	pHidefrom = 0;
	//	f_GotoHidingPlace = 0;
	f_BoredTill = 0;
	Chat.f_LastChat = gpGlobals->time;
	Chat.l_ChatEvent = 0;
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
	
	*Chat.szChatText = 0;
	
	pWPAMPlay = 0;
	bReplay = false;
	
	// delete short term memory
	STMem.f_NUpdate = 0;
	LTMem.f_NUpdate = 0;
	STMem.iCount = 0;
	f_LastFight = gpGlobals->time;
	
	fViewSpeedYaw=0;		// speed of changing view yaw
	fViewSpeedPitch=0;		// speed of changing view pitch
	
	memset(FFE,0,sizeof(FEItem)*32);
	
	// update skill
	UpdateSkill();
	
	// reset nnlog
	//BotResetNNLog(pBot);
	
	// handle pref weapon stuff
	if(lPWeapon<=0)
		lPWeapon = WeaponClass2ID(szPWeapon);
	
	// just sometimes more teamplay
	/*if(RANDOM_LONG(0,100) < 65){
		Task.AddTask(BT_ROAMTEAM,gpGlobals->time + 40.0,0,0,0);
	}*/
	
	//m_bIsVIP = false;
	
	if(g_iMapType == MT_AS){
		if(!TEq(fLastVIPScan,gpGlobals->time,2)){
			fLastVIPScan = gpGlobals->time;
			//cout << "searching for vip" << endl;
			g_pVIP = 0;
			for (int i = 0; i < gpGlobals->maxClients; i++){
				edict_t *pEnt = INDEXENT(i + 1);
				// skip invalid players
				if ((pEnt) && (!pEnt->free)){
					if(UTIL_IsVIP(pEnt)){
						m_bIsVIP = true;
						g_pVIP = pEnt;
					}
					else{
						m_bIsVIP = false;
					}
				}
			}
		}
	}
	else
		m_bIsVIP = false;
}

bool CBotCS :: BuyWeapon(void){	
	if(m_flBuyTime > gpGlobals->time-1.0f)
		return false;
	
	m_flBuyTime = gpGlobals->time;
	
	FakeClientCommand(pEdict,"menuselect 0");
	
	//BotHandleMenu(pBot);
	if(m_iBotMoney < 300)
		return false;
	
	if(Task.SearchT(BT_PICKUP) != -1)
		return false;
	
	if(HasPrimary())
		BotBuy_PAmmo(this);
	
	if(m_iBotMoney < 1600
		|| CVAR_BOOL(jb_pistolonly)
		|| (HasPrimary() && m_iBotMoney > 4000))
	{
		if(HasSecondary() != CS_WEAPON_DEAGLE &&			// ya don't need 2 deagles etc.
			HasSecondary() != CS_WEAPON_ELITE &&
			HasSecondary() != CS_WEAPON_P228 &&
			HasSecondary() != CS_WEAPON_FIVESEVEN &&
			!HasPrimary())
		{
			FakeClientCommand(pEdict,"menuselect 0");	//quit fuckin menues
			if(RANDOM_LONG(0,100) > 90){
				if(bot_teamnm == CS_TEAM_TE){
					if(m_iBotMoney > 900){
						Buy[CS_WEAPON_ELITE](this);
					}
					else
						if(RANDOM_LONG(0,100)>30)
							Buy[CS_WEAPON_DEAGLE](this);
						else
							Buy[CS_WEAPON_P228](this);
				}
				else{		// ct
					if(RANDOM_LONG(0,100)>50)
						Buy[CS_WEAPON_DEAGLE](this);
					else
						if(RANDOM_LONG(0,100) < 50)
							Buy[CS_WEAPON_P228](this);
						else
							Buy[CS_WEAPON_FIVESEVEN](this);

					if (!HasShield() && m_iBotMoney > 2200)
						Buy[CS_WEAPON_SHIELD](this);
				}
			}
			else{
				m_flBuyTime = gpGlobals->time + 2.0f;
			}
			
			BotBuy_SAmmo(this);
		}
		return true;
	}
	else{
		if(HasPrimary())
			return true;
		
		if(!bLoadedLPB){				// evtl. load probabilities
			bLoadedLPB = true;
			WeaponDefs.Load(jb_buyprobfile->string);
		}
		
		int ischl;
		int im_iBotMoney = m_iBotMoney-200;
		int iWeaponDecision=-1;
		float fChoice;
		float fBuyLProb[32];
		
		long lKSniper=0,
			lKSub = 0,
			lKShot = 0;
		bool bKSniper = false,
			bKSub = false,
			bKShot = false;
		
		// process log of weapon with which the bot has been killed
		for(ischl = 0;ischl < _MAXLKW;ischl++){
			if(FLKW[ischl] != -1){
				if(IsSniperWeapon(1<<FLKW[ischl])){
					lKSniper ++;
				}
				if(IsShotgunWeapon(1<<FLKW[ischl])){
					lKShot ++;
				}
				if(IsSubMWeapon(1<<FLKW[ischl])){
					lKSub ++;
				}
			}
		}
		
		lKSniper>lKSub&&lKSniper>lKShot?
			bKSniper=true:lKSub>lKSniper&&lKSub>lKShot?
				bKSub=true:lKShot>lKSniper&&lKShot>lKSub?
					bKShot=true:0==0;
		
		if(im_iBotMoney > 3500)
			im_iBotMoney -= 1000;
		
		if(!HasPrimary()){
			LOG_BUYWEAPON(UTIL_VarArgs("--------- %s ---------\n",STRING(pEdict->v.netname)));
			if(bot_teamnm == CS_TEAM_CT){		// check team dep weapon stuff and evtl. change favourite weapon
				if(lPWeapon == CS_WEAPON_FAMAS)
					lPWeapon = CS_WEAPON_M4A1;
				else if(lPWeapon == CS_WEAPON_AK47)
					lPWeapon = CS_WEAPON_M4A1;
				else if(lPWeapon == CS_WEAPON_MAC10)
					lPWeapon = CS_WEAPON_TMP;
				else if(lPWeapon == CS_WEAPON_G3SG1)
					lPWeapon = CS_WEAPON_SG550;
				else if(lPWeapon == CS_WEAPON_SG552)
					lPWeapon = CS_WEAPON_AUG;
			}
			else{	// te
				if(lPWeapon == CS_WEAPON_GALIL)
					lPWeapon = CS_WEAPON_AK47;
				else if(lPWeapon == CS_WEAPON_M4A1)
					lPWeapon = CS_WEAPON_AK47;
				else if(lPWeapon == CS_WEAPON_TMP)
					lPWeapon = CS_WEAPON_MAC10;
				else if(lPWeapon == CS_WEAPON_SG550)
					lPWeapon = CS_WEAPON_G3SG1;
				else if(lPWeapon == CS_WEAPON_AUG)
					lPWeapon = CS_WEAPON_SG552;
			}
			fChoice = RANDOM_FLOAT(0.0,100.0);
			
			//////////////////////////////////
			float fTemp[32];
			memcpy(fTemp,WeaponDefs.fPBuyProb[mod_id],sizeof(float) * 32);
			float fSum=0,fLast=0;
			long lschl;
			for(lschl=0;lschl<32;lschl++){
				if((IsSniperWeapon(1<<lschl)&&bKSniper)			// bit higher priority for weapons with which the bot has been klled often
					||(IsSubMWeapon(1<<lschl)&&bKSub)
					||(IsShotgunWeapon(1<<lschl)&&bKShot)){
					fTemp[lschl]*=1.5;
				}
				if(lPWeapon == lschl)							// pref weapon
					fTemp[lschl] *=2.0;
				if(WeaponDefs.iCost[mod_id][lschl] > im_iBotMoney){	// no prob for too expenive weapons
					fTemp[lschl]=0;
				}
				if(WeaponDefs.iTeam[mod_id][lschl] != -1){
					if(WeaponDefs.iTeam[mod_id][lschl] != bot_teamnm){
						fTemp[lschl]=0;
					}
				}
			}
			for(lschl=0;lschl<32;lschl++)
				fSum+=fTemp[lschl];
			
			for(lschl=0;lschl<32;lschl++){
				fBuyLProb[lschl] = fLast + fTemp[lschl] / fSum * 100.0f;
				fLast = fBuyLProb[lschl];
				if(fTemp[lschl] == .0)
					fBuyLProb[lschl] = 0;
			}
			//////////////////////////////////
			
			for(ischl=0;ischl<32;ischl++){
				if(fChoice < fBuyLProb[ischl]){
					iWeaponDecision = ischl;
					break;
				}
			}
			if(iWeaponDecision!=-1){
				LOG_BUYWEAPON(UTIL_VarArgs("%s",weapon_defs[iWeaponDecision].szClassname));
				
				if(WeaponDefs.iCost[mod_id][iWeaponDecision] > im_iBotMoney){
					LOG_BUYWEAPON(UTIL_VarArgs(" is too expensive - %i\n",im_iBotMoney));
					iWeaponDecision = -1;
				}
				else if(WeaponDefs.iTeam[mod_id][iWeaponDecision] != -1
					&&WeaponDefs.iTeam[mod_id][iWeaponDecision] != bot_teamnm){
					iWeaponDecision = -1;
					LOG_BUYWEAPON(UTIL_VarArgs(" is wrong team \n"));
				}
				else if(iWeaponDecision !=-1 ){
					LOG_BUYWEAPON(UTIL_VarArgs(" is ok\n"));
				}
				
				//FakeClientCommand(pEdict,"menuselect 0");	//quit fuckin menues
				Buy[iWeaponDecision](this);
				
				if(iWeaponDecision == CS_WEAPON_SCOUT || iWeaponDecision == CS_WEAPON_AWP){
					Task.RemoveT(BT_ROAMTEAM);
				}
			}
		}
		
		BotBuy_PAmmo(this);
		/*Buy[CS_WEAPON_HEGRENADE](this);
		Buy[CS_WEAPON_FLASHBANG](this);
		Buy[CS_WEAPON_FLASHBANG](this);*/
		
		if(im_iBotMoney > 900 &&
			HasPrimary()){
			if(RANDOM_LONG(0,100) > _BUY_HE){
				Buy[CS_WEAPON_HEGRENADE](this);
			}
			if(RANDOM_LONG(0,100) > _BUY_FL){
				Buy[CS_WEAPON_FLASHBANG](this);
			}
			if(RANDOM_LONG(0,100) > _BUY_SG){
				Buy[CS_WEAPON_SMOKEGRENADE](this);
			}
		}
		
		if((HasSniper() == CS_WEAPON_AWP
			||HasSniper() == CS_WEAPON_SCOUT
			||HasSniper() == CS_WEAPON_G3SG1
			||HasSniper() == CS_WEAPON_SG550)
			&& im_iBotMoney > 5000
			&& HasSecondary() != CS_WEAPON_DEAGLE
			&& HasSecondary() != CS_WEAPON_ELITE
			&& HasSecondary() != CS_WEAPON_FIVESEVEN
			&& HasSecondary() != CS_WEAPON_P228){
			if(bot_teamnm== CS_TEAM_CT){
				if(RANDOM_LONG(0,100) < 33){
					Buy[CS_WEAPON_DEAGLE](this);
				}
				if(RANDOM_LONG(0,100) < 50){
					Buy[CS_WEAPON_FIVESEVEN](this);
				}
				else{
					Buy[CS_WEAPON_P228](this);
				}
				BotBuy_SAmmo(this);
			}
			else{
				if(RANDOM_LONG(0,100) < 33){
					Buy[CS_WEAPON_DEAGLE](this);
				}
				if(RANDOM_LONG(0,100) < 50){
					Buy[CS_WEAPON_ELITE](this);
				}
				else{
					Buy[CS_WEAPON_P228](this);
				}
				BotBuy_SAmmo(this);
			}
		}
		
		if(g_iMapType == MT_DE		// def kit
			&&bot_teamnm == CS_TEAM_CT
			&&RANDOM_LONG(0,100) < 30){
			
			BotBuy_Defuse(this);
		}
		
		if(bot_armor<50
			&& RANDOM_LONG(0,100) < 50){
			if (im_iBotMoney > 1000)
				BotBuy_KevlarHelmet(this);
			else
				BotBuy_Kevlar(this);
		}
		return true;
	}
	return false;
}

void CBotCS :: GrenadeThrown(void){
	if(GOrder.lTypeoG == CS_WEAPON_FLASHBANG){
		// find a near wp which is out of sight of this grenade
		int iENWp = WaypointFindNearest(GOrder.VAim,0,1000,-1,false);
		
		float fMin = 0,fTempD;
		
		int iNIVWP = WaypointFindNearest(pEdict,1000,bot_teamnm,fMin,false,false,false);
		
		float fDist = 1000000;
		int iNTemp = -1;
		for(int index = 0; index < num_waypoints ; index ++){
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
			}
			else{
				continue;
			}
			
			/*if(WPStat.GetVisible(iNIVWP,index))
				continue;*/
			
			fTempD = WaypointDistanceFromTo(iNIVWP,index,bot_teamnm);
			if(fTempD < fDist
				&& fTempD < 800){
				fDist = fTempD;
				iNTemp = index;
			}
		}
		if(iNTemp != -1){
			if(Task.current&&!(Task.current->lType & BT_HIDE)){
				Task.AddTask(BT_GOTO|BT_CAMPATGOAL|BT_HIDE|BT_LOCKED, gpGlobals->time + 4.0, iNTemp, pBotEnemy,2);
				ResetWPlanning();
			}
		}
	}
	else{
	}
}

void CBotCS :: ReactOnRadio(void){
	if(bot_IRadioM.lMessage != 0){		
		// callers weapon
		long lCaWeapon = 0;
		int iNearWP;
		int iNum=0;
		int i;
		edict_t *pEnt,*pEFound;
		float fDistance,fMin;
		if(bot_IRadioM.pECalling)
			lCaWeapon = WeaponModel2ID(STRING(bot_IRadioM.pECalling->v.weaponmodel));
		
		switch(bot_IRadioM.lMessage){
		case RC_FOLLOW_ME:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			if(bot_IRadioM.f_Time < gpGlobals->time
				&& (pEdict->v.origin - bot_IRadioM.pECalling->v.origin).Length() < 500.0f
				&&!m_bIsVIP){
				if(RANDOM_LONG(0,100) < 50){
					if((IsSniperWeapon(1<<lCaWeapon)&&(HasSubM()||HasRifle())
						|| RANDOM_LONG(0,100) < 33
						||UTIL_IsVIP(bot_IRadioM.pECalling))
						&& CVAR_BOOL(jb_msgradio)
						&& !Task.Important() ){		// no current order
						Task.AddTask(BT_FOLLOW,gpGlobals->time + 600.0,0,bot_IRadioM.pECalling,0);
						
						ResetWPlanning();					// delete current way to decide on a new one
						
						SendRadioCommand(RADIO_AFFIRMATIVE);
					}
					else{
						if(RANDOM_LONG(0,100) < 50 && CVAR_BOOL(jb_msgradio)){
							SendRadioCommand(RADIO_NEGATIVE);
						}
					}
				}
				bot_IRadioM.lMessage = 0;
			}
			break;
		case RC_COVER_ME:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			if(bot_IRadioM.f_Time < gpGlobals->time
				&& (pEdict->v.origin - bot_IRadioM.pECalling->v.origin).Length() < 500.0f
				&&!m_bIsVIP){
				if(RANDOM_LONG(0,100) < 50){
					if((IsSniperWeapon(1<<lCaWeapon)&&(HasSubM()||HasRifle())
						||(HasSniper()&&(IsSubMWeapon(1<<lCaWeapon)||IsRifleWeapon(1<<lCaWeapon) )))	// and vice versa
						|| RANDOM_LONG(0,100) < 33
						|| UTIL_IsVIP(bot_IRadioM.pECalling)
						&& CVAR_BOOL(jb_msgradio)
						&& !Task.Important()
						&& Task.SearchT(BT_COVER) == -1){		// no current order
						Task.AddTask(BT_COVER,gpGlobals->time + 60.0,0,bot_IRadioM.pECalling,0);
						
						ResetWPlanning();
						
						SendRadioCommand(RADIO_AFFIRMATIVE);
						//DEBUG_CLIENTCOMMAND(pEdict,"say responding to backup call");
					}
					else{
						if(RANDOM_LONG(0,100) < 50 && CVAR_BOOL(jb_msgradio)){
							SendRadioCommand(RADIO_NEGATIVE);
						}
					}
				}
				bot_IRadioM.lMessage = 0;
			}
			break;
		case RC_YOU_TAKE_THE_POINT:				// i.e. stop following my commands
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			if(Task.SearchP(bot_IRadioM.pECalling) != -1){		// calling bot is the one, the order came from
				Task.RemoveP(bot_IRadioM.pECalling);
				ResetWPlanning();
				//DEBUG_CLIENTCOMMAND(pEdict,"say got it");
			}
			bot_IRadioM.lMessage = 0;
			break;
		case RC_HOLD_THIS_POSITION:			// just camp were u are ... but not as long as regulary
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			if(!Task.Important()){
				Task.AddTask(BT_HOLDPOS,gpGlobals->time + 30.0,0,0,0);
				
				//f_Hide = gpGlobals->time + 30 * RANDOM_FLOAT(.8,1.5);		// stop there for a minute;
				
				//pHidefrom = 0;
				//InitCamp();
				bot_IRadioM.lMessage = 0;
				f_Aggressivity = -2;		// be defensive
			}
			break;
		case RC_REGROUP_TEAM:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			bot_IRadioM.lMessage = 0;
			break;
		case RC_NEED_BACKUP:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			if(bot_IRadioM.f_Time < gpGlobals->time){	// kind of reaction time
				if(RANDOM_LONG(0,100) < 50		// todo look at callers health etc
					&& !Task.Important()
					&& CVAR_BOOL(jb_msgradio)
					&&!m_bIsVIP){		// no current order
					Task.AddTask(BT_HELP,gpGlobals->time + 60.0,0,bot_IRadioM.pECalling,0);
					
					SendRadioCommand(RADIO_AFFIRMATIVE);
					ResetWPlanning();
					//DEBUG_CLIENTCOMMAND(pEdict,"say responding to backup call");
				}
				bot_IRadioM.lMessage = 0;
			}
			break;
		case RC_TAKING_FIRE:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			bot_IRadioM.lMessage = RC_NEED_BACKUP;
			break;
		case RC_GO_GO_GO:						// go and fight against enemy
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			fDistance = (bot_IRadioM.pECalling->v.origin - pEdict->v.origin).Length();
			if(fDistance > 500.0)
				break;
			f_Aggressivity += 3.0;		// be aggressive
			fMin = 1000;
			pEFound = 0;
			for (i = 0; i < gpGlobals->maxClients; i++){
				pEnt = INDEXENT(i + 1);
				if ((pEnt) && (!pEnt->free) && (pEnt != pEdict)){
					// skip this player if not alive (i.e. dead or dying)
					if (!IsAlive(pEnt))
						continue;
					if(!FInViewCone(&pEnt->v.origin,bot_IRadioM.pECalling))
						continue;
					fDistance = (bot_IRadioM.pECalling->v.origin - pEnt->v.origin).Length();
					if(fDistance < fMin){
						pEFound = pEnt;
						fMin = fDistance;
					}
				}
			}
			Task.RemoveT(BT_CAMP);	// stop camping etc.
			Task.RemoveT(BT_HIDE);
			
			f_noCamp = gpGlobals->time + 20.0;
			bot_IRadioM.lMessage = 0;
			
			if(pEFound){
				iGoal = WaypointFindNearest(pEFound,300,bot_teamnm);
			}
			break;
			
		case RC_TEAM_FALL_BACK:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			bot_IRadioM.lMessage = 0;
			break;
		case RC_STICK_TOGETHER_TEAM:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			bot_IRadioM.lMessage = 0;
			break;
		case RC_GET_IN_POSITION_AND_WAIT:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			bot_IRadioM.lMessage = RC_HOLD_THIS_POSITION;		// just do that
			break;
		case RC_STORM_THE_FRONT:				// stop camping
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			Task.RemoveT(BT_CAMP);
			Task.RemoveT(BT_HIDE);
			
			f_noCamp = gpGlobals->time + 20.0;
			bot_IRadioM.lMessage = 0;
			f_Aggressivity += 3;		// be very agressive
			break;
			
		case RC_REPORT_IN_TEAM:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			bot_IRadioM.lMessage = 0;
			break;
			
		case RC_AFFIRMATIVE:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... somebody said ok, so store its edict and set flag to affirmed
				if(Ordered.lAction){			// bot ordered sth., therefore there is sth to aff
					Ordered.lAction |= BO_AFF;
					Ordered.pREdict = bot_IRadioM.pECalling;
					Ordered.VOrigin = bot_IRadioM.pECalling->v.origin;	// just store it, but do we need it ?
				}
			}
			bot_IRadioM.lMessage = 0;
			break;
		case RC_ENEMY_SPOTTED:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			if(1){
				if(f_LastFight + 20.0 < gpGlobals->time
					&& RANDOM_LONG(0,gpGlobals->maxClients) < 3){
					//FindWayVector(pEdict,bot_IRadioM.pECalling->v.origin,FW_Cur,WayDecideShortest);
					int iWP = WaypointFindNearestGoal(pEdict->v.origin,pEdict,5000,bot_teamnm,0);
					Task.AddTask(BT_GOTO,gpGlobals->time + 30.0,iWP,(void*)0,0);
					ResetWPlanning();
				}	
			}
			bot_IRadioM.lMessage = 0;
			break;
		case RC_SECTOR_CLEAR:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			iNearWP = WaypointFindNearest(bot_IRadioM.pECalling,300,bot_teamnm);
			
			if (g_iMapType == MT_DE){
				if(iNearWP != -1
					&&iFarGoal != -1){
					if(WaypointDistanceFromTo(iNearWP,iFarGoal,bot_teamnm) < 900){		// i.e. bot is running the same goal as where there isn't anything
						iGoal = -1;
						iFarGoal = WaypointFindNearestGoal(pEdict,iNearWP,bot_teamnm,W_FL_FLAG,300,&iNearWP,1);
						Task.AddTask(BT_GOTO,gpGlobals->time + 60.0,iFarGoal,(void*)1,0);
						ResetWPlanning();
					}
				}
			}
			bot_IRadioM.lMessage = 0;
			break;
		case RC_IN_POSITION:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			bot_IRadioM.lMessage = 0;
			break;
		case RC_REPORTING_IN:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			bot_IRadioM.lMessage = 0;
			break;
		case RC_GET_OUT_OF_THERE:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			bot_IRadioM.lMessage = 0;
			break;
		case RC_NEGATIVE:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete, cause negative answer
				Ordered.lAction = 0;
			}
			bot_IRadioM.lMessage = 0;
			break;
		case RC_ENEMY_DOWN:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			bot_IRadioM.lMessage = 0;
			break;
		case RC_TEAMATTACK:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			if(RANDOM_LONG(0,100) < 30)			// otherwise this would be too much
				Chat.l_ChatEvent |= E_TK;
			
			bot_IRadioM.lMessage = 0;
			break;
		case RC_HOSTAGE_DOWN:
			if(Ordered.lAction & BO_AFF){		// bot has already a affirmed radio mess
			}
			else{		// bot has no affirmed order ... delete all
				Ordered.lAction = 0;
			}
			Chat.l_ChatEvent |= E_HDown;
			bot_IRadioM.lMessage = 0;
			break;
		default:
			bot_IRadioM.lMessage = 0;
			break;
		}
	}
}

bool CBotCS :: HandleOrders(void){		// this fucntion is called every second
	/*if(Task.current
		&& Task.current->lType & BT_CAMP
		&& Task.current->fLive2 > gpGlobals->time + 10.f
		&& !HasSniper())
	{
		if(!Task.Important()){
			Task.AddTask(BT_WAIT4TM8,gpGlobals->time + 9.0,0,0,0);

			//DEBUG_CLIENTCOMMAND(pEdict,"say -------------------");
		}
	}*/

	if(Task.current){		// bot got order
		CTaskItem * CurrentTask = Task.current;
		
		if(CurrentTask->lType & BT_GOTO){
			if(CurrentTask->lAdd == -1){
				Task.NextTask();
				DEBUG_CLIENTCOMMAND(pEdict,"say nothing in it");
			}
		}
		else if(CurrentTask->lType & BT_CAMPATGOAL){
		}
		else if(CurrentTask->lType & BT_CAMP){
			if(Task.SearchT(BT_ROAMTEAM) == -1
				&& !HasSniper()){
				
				edict_t *pN;
				float fMin = 0;
				pN = GetNearestPlayer(pEdict,bot_teamnm,fMin,true,true);
				if(pN
					&&(pN->v.origin-pEdict->v.origin).Length()<250.0){
					
					Task.RemoveT(BT_CAMP);
					f_ducktill = gpGlobals->time;
					if(Task.Important()){
						CBotBase *pB = UTIL_GetBotPointer(pN);
						if(pB){
							if(pB->Task.SearchT(BT_COVER|BT_ROAMTEAM)==-1){		// don't cover another bot whic covers another player himself
								Task.AddTask(BT_ROAMTEAM,gpGlobals->time + 260.0,0,pN,0);
								DEBUG_CLIENTCOMMAND(pEdict,"say wait4tm8 roamteam");
							}
						}
					}
				}
			}
		}
		/*else if((CurrentTask->lType & BT_FOLLOW)
			|| (CurrentTask->lType & BT_COVER))
		{
			if(CurrentTask->p){			// is an edict stored ?
				if(UTIL_ClientIndex((edict_t *)CurrentTask->p) == -1){
					Task.NextTask();
				}
				// look if the related bot is still alive
				else if(!IsAlive((edict_t *)CurrentTask->p)){		// player who send order is dead
					Task.NextTask();
				}
			}
		}*/// taken over by preprocess tasks
		/*else if(CurrentTask->lType & BT_ROAMTEAM){
			if(CurrentTask->p){			// is an edict stored ?
				if(UTIL_ClientIndex((edict_t *)CurrentTask->p) == -1){
					Task.NextTask();
				}
				else if(!IsAlive((edict_t *)CurrentTask->p)){		// player who send order is dead
					Task.NextTask();
				}
				else{		// there is an edict and it's alive
					if(g_fRoundTime > 10.0){
						// test if other moved ... otherwise del this order
						float fDistance = (CurrentTask->VT - ((edict_t *)CurrentTask->p)->v.origin).Length();
						bool bIsCamping = Task.SearchT(BT_CAMP)!=-1 && f_ducktill > gpGlobals->time;
						if(fDistance < 10.0
							&& !bIsCamping){
							if(RANDOM_LONG(0,100) < 80){
								Task.AddTask(BT_GOTO|BT_CAMPATGOAL|BT_CROUCH,gpGlobals->time + 3,WaypointFindNearest(Task.current->p,1000,bot_teamnm,50,true,false),0,0);
							}
							else{
								Task.NextTask();
							}
						}
						else{
							CurrentTask->VT = ((edict_t *)CurrentTask->p)->v.origin;
							if(bIsCamping){
								Task.RemoveT(BT_CAMP);
								f_ducktill = 0;
								f_Pause = 0;
							}
						}
					}
				}
#ifdef _ROAMBEAM
				if(pEdictPlayer && Task.current->p){
					Vector VMid = (pEdict->v.origin+Task.current->p->v.origin)/2;
					//WaypointDrawBeam(pEdictPlayer,pEdict->v.origin,VMid,20,5,200,200,200,200,100);
					//WaypointDrawBeam(pEdictPlayer,VMid,Task.current->p->v.origin,20,5,200,000,000,200,100);
				}
#endif
			}
			else{
				// just get a new teammember
				Task.current->p = GetRandomPlayerWO(pEdict,bot_teamnm,1,BT_COVER|BT_ROAMTEAM,pEdict);
				
				if(!Task.current->p){		// found no edict
					Task.NextTask();
					return false;
				}
			}
			if(m_bIsVIP)				// vip shouldn't
			Task.NextTask();
		}*/// taken over by preprocess tasks
		else if(CurrentTask->lType & BT_GUARD){
			if(CurrentTask->p){
				if(!IsAlive((edict_t *)CurrentTask->p)){
					Task.NextTask();
				}
				else if(UTIL_ClientIndex((edict_t *)CurrentTask->p) == -1){
					Task.NextTask();
				}
			}
		}
		else if(CurrentTask->lType & BT_HOLDPOS){
			Task.AddTask(BT_CAMP|BT_TMP,CurrentTask->fLive2,0,0,0);
		}
		else if(CurrentTask->lType & BT_WAIT4TM8){
			Task.AddTask(BT_CAMP,CurrentTask->fLive2,0,0,0);
			if(CurrentTask->fLive2 < gpGlobals->time-.5){
				Task.NextTask();
				Task.RemoveT(BT_CAMP);
				f_ducktill = gpGlobals->time;
			}
			else{
				Task.AddTask(BT_CAMP|BT_CROUCH,CurrentTask->fLive2,0,0,0);
				//f_ducktill = CurrentTask->fLive2;
				
				edict_t *pN;
				float fMin = 0;
				pN = GetNearestPlayer(pEdict,bot_teamnm,fMin,true,true);
				if(pN&&(pN->v.origin-pEdict->v.origin).Length()<200.0){
					Task.NextTask();
					Task.RemoveT(BT_CAMP);
					f_ducktill = gpGlobals->time;
					if(Task.Important()){
						CBotBase *pB = UTIL_GetBotPointer(pN);
						if(pB){
							if(!pB->Task.SearchT(BT_COVER|BT_ROAMTEAM) ){		// don't cover another bot whic covers another player himself
								Task.AddTask(BT_COVER,gpGlobals->time + 60.0,0,pN,0);
							}
						}
					}
				}
			}
		}
		/*else if(CurrentTask->lType & BT_PAUSE){
			if(CurrentTask->lAdd == 0){					// initializing
				f_Pause = CurrentTask->fLive2;
				CurrentTask->lAdd = 1;
			}
			else if(CurrentTask->lAdd == 1){
				if(CurrentTask->lType & BT_CROUCH){		// should the bot pause crouch ?
					f_ducktill = CurrentTask->fLive2;
				}
			}
			else
				Task.NextTask();
		}*/
		else if(CurrentTask->lType & BT_RELOAD){				// reload weapon
			CurrentTask->lAdd ++;
			if(CurrentTask->lAdd < 2)
				CurrentTask->fLive2 = gpGlobals->time + 1.0;
			f_DenyWChange = CurrentTask->fLive2;
			
			if(HasPrimary()){
				if(IsCWeaponPrimary()){
					lButton |= IN_RELOAD;
					Task.NextTask();
				}
				else{
					f_DenyWChange = 0;
					Change2Weapon(HasPrimary());
				}
			}
			else{
				lButton |= IN_RELOAD;
				Task.NextTask();
			}
			f_Pause = gpGlobals->time + 1.5f;
			f_DenyWChange = gpGlobals->time + 2.5f;
			DEBUG_CLIENTCOMMAND(pEdict,"say reload due to BT_RELOAD");
			//Task.AddTask(BT_PAUSE|BT_TMP,gpGlobals->time + 2,0,0,0);					// pause for some time	
		}
	}
	return true;
}

bool CBotCS :: IsBehindSG(edict_t *pentPlayer){
	long lschl;
	Vector VB,VVG,VVP, VVB;
#ifdef USE_SGOPTIMIZED
	float fGDistance;
	float fPDistance;
	float fDotBorder;
	
	VVP = pentPlayer->v.origin - pEdict->v.origin;
	fPDistance = VVP.Length();
	VVP = VVP * (1.f / fPDistance);		// normalize
#else
 	VVP = (pentPlayer->v.origin - pEdict->v.origin).Normalize();
#endif /* USE_SGOPTIMIZED */
	
	for(lschl=0;lschl < _MAXGRENADEREC;lschl++){
		if(gSmoke[lschl].bUsed){
#ifdef USE_SGOPTIMIZED
			VVG = gSmoke[lschl].VOrigin - pEdict->v.origin;
			fGDistance = VVG.Length();
			if(fGDistance > fPDistance + _SMOKERADIUS/2)		// if that player is a lot in front of the grenade ...
				continue;

			VVG = VVG * (1.f/fGDistance);		// normalize
			//VB = Vector(VVG.y,VVG.x,0)/*CrossProduct(VVG,Vector(0,0,1))).Normalize()*/ * _SMOKERADIUS + gSmoke[lschl].VOrigin;
			// ( what's anyway this replacement of the crossproduct ? did I do that ? holy shit !
			// 
			fDotBorder = fGDistance / sqrt(fGDistance * fGDistance + _SMOKERADIUS*_SMOKERADIUS);
		
			//VVB = (VB - pEdict->v.origin).Normalize();
			
			// if the dotproduct of ( connection from here to smokegrenade and connection from here to origin to see )
			// is bigger than dotproduct of ( connection from here to smokegrenade and connection from here to border of
			// possible smoke area )

			if(DotProduct(VVG,VVP) > fDotBorder /*DotProduct(VVG,VVB)*/){
				return true;
			}
#else /* not USE_SGOPTIMIZED */
 			VVG = (gSmoke[lschl].VOrigin - pEdict->v.origin).Normalize();
 			VB = Vector(VVG.y,VVG.x,0)/*CrossProduct(VVG,Vector(0,0,1))).Normalize()*/ * _SMOKERADIUS + gSmoke[lschl].VOrigin;
 			VVB = (VB - pEdict->v.origin).Normalize();
 			
 			if(DotProduct(VVG,VVP) > DotProduct(VVG,VVB)){
				return true;
			}
#endif /* USE_SGOPTIMIZED */
		}
	}
	return false;
}

edict_t *CBotCS :: FindEnemy(){
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
		bool bNotNew,bBehindSG;
		float _ERSTime = 3.0;
		
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
						
						//FFEP->fDistance= (pPlayer->v.origin - pEdict->v.origin).Length();
						FFEP->fDistance = pfPlayerDistance[iEIndex][i];

						bBehindSG = IsBehindSG(pPlayer);
						if(bBehindSG == false
							&& FFEP->bBehindSG == true){		// enemy is't anymore covered by SG
							FFEP->fTime2seeit -= TI_ES_DELAY_SG;
						}
						
						if(bBehindSG == true
							&& FFEP->bBehindSG == false){
							FFEP->fTime2seeit += 1.0;
						}
						
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
							
							if(bNotNew){
							}
							else{		// really new :D
								FFEP->fRecogn = gpGlobals->time;
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
							/*if(IsProne(FFEP->pEdict)){
								FFEP->lFlags|=FL_ES_DELAY_PRONE;
							}*/
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
								FFEP->fTime2seeit += (FFEP->fDistance - nearestdistance)*.001 * f_TPD *.001;
							}
							if(FFEP->lFlags & FL_ES_DELAY_SG){
								FFEP->fTime2seeit += TI_ES_DELAY_SG;
							}
							if(FFEP->lFlags & FL_ES_DELAY_DUCK){
							}
							if(FFEP->lFlags & FL_ES_DELAY_PRONE){
								FFEP->fTime2seeit += (FFEP->fDistance - f_ProneInDistance)*.001 * f_ProneTPD *.001;
							}
							
							if(bNotNew){
								FFEP->fTime2seeit *= .333333333f;
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
		
		if (pNewEnemy){
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

void CBotCS :: PreprocessTasks(void){
	if(Task.current){
		if(false){
		}
		else
			CBotBase::PreprocessTasks();
	}
}

void CBotCS :: Think5th(void){
	TraceResult tr;
	// see how far bot has moved since the previous position...
	v_diff = v_prev_origin - pEdict->v.origin;
	moved_distance = v_diff.Length();
	
	// save current position as previous
	v_prev_origin = pEdict->v.origin;
	i1C--;
	
	// look if there are tasks in the task system to be deleted
	Task.CheckTLife();
	
	// process faked sound system
	ReactOnSound();
	
	// is susp location (where bot heard sth) visible ?
	if(Action.lAction & BA_SUSPLOC){
		if(f_LastFight < gpGlobals->time-1
			&&f_LookTo < gpGlobals->time-1){
			if(d_Manner + f_Aggressivity <.1f){
				if((VSuspEn-pEdict->v.origin).Length() < 400.f){
					f_ducktill = gpGlobals->time + 1.25f;
				}
			}
			
			//Vector VTo = pEdict->v.origin+pEdict->v.view_ofs+1.0*pEdict->v.velocity;
			UTIL_TraceLine(pEdict->v.origin,VSuspEn,ignore_monsters,pEdict->v.pContainingEntity,&tr);
			//UTIL_TraceLine(pEdict->v.origin,VTo,ignore_monsters,pEdict->v.pContainingEntity,&tr2);
			
			if(tr.flFraction == 1.0
				/*&& tr2.flFraction == 1.0*/){				// i.e. visible
				VLookTo = VSuspEn;
			f_LookTo = gpGlobals->time +1.5;
			//f_GoBack = 
			
			DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,VSuspEn,4,10,200,200,200,200,10);
			
			if(FInViewCone(&VSuspEn,pEdict)){
				Action.lAction &= ~BA_SUSPLOC;			// del susploc flag
			}
			}
		}
	}
	
	if(pLastGlobalKilled){
		if(UTIL_ClientIndex(pLastGlobalKilled) != -1
			&& UTIL_GetTeam(pLastGlobalKilled) == bot_teamnm
			&& FVisible(pLastGlobalKilled->v.origin,pEdict)){
			if(f_LastFight < gpGlobals->time - 1.0){
				HeadToward(pLastGlobalKilled->v.origin);
				if(d_Manner < .1){
					// don#t run into the enemy
					Task.AddTask(BT_CAMP,gpGlobals->time + RANDOM_FLOAT(1.5,4.5),1,0,0);
					f_Aggressivity += 2.0;
					sLookToChange = 1;
					//InitCamp();
					f_CaLooktoWP = gpGlobals->time + 2.5f;
				}
			}
		}
		pLastGlobalKilled = 0;
	}
	
	if(pLastGlobalKill){
		if(UTIL_ClientIndex(pLastGlobalKilled) != -1
			&& UTIL_GetTeam(pLastGlobalKilled) == bot_teamnm
			&& FVisible(pLastGlobalKilled->v.origin,pEdict)){
			if(f_LastFight < gpGlobals->time - 1.0){
				if(d_Manner > -.1){	// just be more aggressive
					f_Aggressivity += 2.0;
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
			Action.lAction&=~BA_SUSPLOC;
			GOrder.lState = GO_QUIT;
			f_Aggressivity += 1.0;		// be more agressive
			if(Task.current
				&& Task.current->lType & BT_CAMP){
				Task.current->fLive2 += float(RANDOM_LONG(2,HasSniper()?10:5));
			}
			
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
				Chat.f_LastChat = gpGlobals->time + RANDOM_FLOAT(0,15);
			}
		}
		pEKilled = 0;
	}
	
	/*// sometimes try to reload
	// at least 5 sec after last fight
	if(RANDOM_LONG(0,100) < 17
		&&!(Action.lAction & BA_PLANT)
		&& (IsCWeaponPrimary()|| IsCWeaponSecondary())
		//&& Task.SearchT(BT_RELOAD) == -1
		&& f_LastFight + 1.5 < gpGlobals->time
		&& float(current_weapon.iClip) / float(WeaponDefs.ipClipSize[mod_id][current_weapon.iId]) < .3)
	{
		//Task.AddTask(BT_RELOAD,gpGlobals->time + 3.0,0,0,0);
		//cout << "reload" << endl;
		lButton|=IN_RELOAD;
	}*/
	
	if(i_FOV != 90
		&&f_LastFight + 1.f < gpGlobals->time){
		lButton|=IN_ATTACK2;
	}
	
	// lower offset
	//v_Offset = v_Offset*.73;
	v_Offset = -pEdict->v.punchangle * (float(bot_skill)*.01);
	
	if(!(Action.lAction & BA_FIGHT))
		f_AimHead *= .8f;
	
	/*if(f_Hide>gpGlobals->time
		&&!TEq(gpGlobals->time,f_StrCamp,10)
		&&!(Task.current && Task.current->lType & BT_HOLDPOS))
	{
		edict_t *pNearest;
		int iTm8;
		iTm8 = UTIL_FightingAgainst(pEdict,bot_teamnm==CT?TE:CT,&pNearest,true);		// only duckin bots

		if(iTm8>1){
			if(f_UsedRadio < gpGlobals->time - _RADIO_FREQ
				&&CVAR_BOOL(jb_msgradio)
				&&Task.SearchT(BT_HIDE) != -1){
				SendRadioCommand(RADIO_GOGOGO);
				f_Hide = gpGlobals->time - .01f;
			}
		}
	}*/
	
	HandleOrders();
	
	if(!i1C){				// every second one time	---------------------------------------------------------------------------------
		i1C = 5;
		Think1();
		
	}
}

void CBotCS :: Think1(void){
	bot_teamnm = UTIL_GetTeam(pEdict);
		
		/*if(m_bIsVIP)
			DEBUG_CLIENTCOMMAND(pEdict,"say I#m the vip");*/
		
		// see how far bot has moved since the previous position...
		v_diff1 = v_prev_origin1 - pEdict->v.origin;
		moved_distance1 = v_diff1.Length();
		
		// save current position as previous
		v_prev_origin1 = pEdict->v.origin;
		
		prev_speed1 = prev_speed;
		
		SearchForIL();
		
		AdaptAvoid();
		
		DistanceSight();
		
		// intercepted radio command - handling
		ReactOnRadio();
		
		// lower aggresivity value
		f_Aggressivity *= .92f;
		
		/*if(RANDOM_LONG(0,100) < 20){
			edict_t *pToUse;
			TestOnButtonWay(&pToUse);
			
			if(pToUse){
				int iDest = WaypointFindNearest(VecBModelOrigin(VARS(pToUse)),pEdict,400,bot_teamnm);
				if(iDest != -1&&Task.SearchT(BT_GOBUTTON) == -1){
					Task.AddTask(BT_GOTO|BT_GOBUTTON,-1,iDest,pToUse,0);
				}
				//DEBUG_CLIENTCOMMAND(pEdict,"say wanting to go where pressing a button is needed");
			}
		}*/
		
		// should bot buy sth ?
		if( Action.lAction & BA_BUYZONE ){
			BuyWeapon();
		}
		
		// do we want backup ???
		if((HasSniper() && !(HasSniper() == CS_WEAPON_AUG || HasSniper() == CS_WEAPON_SG552))							// has sniper, but no aug or sg552
			&&!(Ordered.lAction & BO_COVER)
			&&!Task.Important()
			&& f_UsedRadio < gpGlobals->time - 5.0f	// don't do that too often
			&& f_LastRadio<gpGlobals->time - 4.0	// don't enerve others
			&& f_noCamp < gpGlobals->time			// don't do that just after capming
			&& g_fRoundTime > 6.0){	// not every time hackhack for double radio mess per frame
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
		
		// sometimes try to reload
		// at least 2.5 sec after last fight
		if(!(Action.lAction & BA_PLANT)
			&& (IsCWeaponPrimary()|| IsCWeaponSecondary())
			&& f_LastFight + 2.5 < gpGlobals->time
			&& float(current_weapon.iClip) / float(WeaponDefs.ipClipSize[mod_id][current_weapon.iId]) < .3){
			//Task.AddTask(BT_RELOAD,gpGlobals->time + 3.0,0,0,0);
			//cout << "reload" << endl;
			lButton|=IN_RELOAD;
			f_DenyWChange = gpGlobals->time + 2.5f;
		}
		
		// fix de_aztec bridge 'bug'
		edict_t *pent;
		pent=0;			// fix de_aztec bug
		bool bflag=false;
		while (pent = UTIL_FindEntityInSphere( pent, pEdict->v.origin,85)){
			if(FStrEq(STRING(pent->v.classname),"func_illusionary")){
				bflag=true;
				break;
			}
		}
		if(bflag){
			f_IgnoreFall = gpGlobals->time + 1.1f;
		}
		
		// chat
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
		
		STMem.Process(pEdict,bSeeThru);
		
		CheckGrenadeThrowing();
		
		// change the weapon not directly after en is away
		if(f_LastFight + .3f < gpGlobals->time
			&&!(Action.lAction&BA_PLANT)){
			if(f_RWKnife > gpGlobals->time){
				if(current_weapon.iId != CS_WEAPON_KNIFE){
					Change2Weapon(CS_WEAPON_KNIFE);
				}
			}
			else{
				if(HasPrimary()
					&&!GOrder.lTypeoG
					&& m_rgAmmo[weapon_defs[HasPrimary()].iAmmo1]>1
					&& !IsCWeaponPrimary()){
					Change2Weapon(HasPrimary());
				}
				else if(current_weapon.iId == CS_WEAPON_KNIFE
					&& HasSecondary()
					&& m_rgAmmo[weapon_defs[HasSecondary()].iAmmo1]>1){
					Change2Weapon(HasSecondary());
				}
			}
		}
		
		if(g_iMapType == MT_DE){
			// look for planted bomb message
			if(long(g_bBombPlanted<<5) != (Action.lAction & BA_BOMBPL)
				&& RANDOM_LONG(0,100) < 40){ // bomb planted changed
				//b_BombPlanted = g_bBombPlanted;
				g_bBombPlanted?Action.lAction|=BA_BOMBPL:Action.lAction&=~BA_BOMBPL;
				
				if(g_bBombPlanted){
					//DEBUG_CLIENTCOMMAND(pEdict,"say noticed it");
					if(RANDOM_LONG(0,100) < 50)
						Chat.l_ChatEvent |= E_BombPl;
					Task.RemoveT(BT_CAMP);
					ResetWPlanning();
					Task.Reset();
				}
			}
			
			// handle flee from bomb stuff
			if(Action.lAction & BA_BOMBPL
				&& g_fBombExplode > 0
				&& RANDOM_LONG(0,100) < 20
				&& TEq(g_fBombExplode,gpGlobals->time , 10)){
				if( Task.SearchT(BT_FLEE) == -1){
					long lFleeto;
					float fMin = 1250 + RANDOM_FLOAT(0,500);
					edict_t *pEdictBomb = 0;
					
					while(pEdictBomb = UTIL_FindEntityByClassname(pEdictBomb,"grenade")){	// let's cheat .. search the bomb
						if(FStrEq(STRING(pEdictBomb->v.model),"models/w_c4.mdl")){
							break;
						}
					}
					if(pEdictBomb){
						if((pEdict->v.origin - pEdictBomb->v.origin).Length()> fMin){
							Task.AddTask(BT_CAMP,gpGlobals->time + 1000.0f,0,0,0);		// just stay there if u are save
						}
						else{
							lFleeto = WaypointFindNearest(pEdictBomb,10000,bot_teamnm,fMin,false,false);
							if(lFleeto != -1){
								Task.AddTaskI(BT_FLEE,-1,lFleeto,0,0);
								ResetWPlanning();
							}
						}
					}
				}
			}
		}
		
		// although it's never used, it should be updated some time
		if(bot_team == 5){
			bot_team = UTIL_GetTeam(pEdict)+1;
		}
		// fix nonshooting after map change
		if(f_PauseShoot > gpGlobals->time + 3.0f)
			f_PauseShoot = 0.0;
		
		// is this place normally dangerous ?
		int iDangerCheckWP = i_CurrWP;
		
		if(iDangerCheckWP != -1){
			if(WPStat[iDangerCheckWP].lKill > WPStat.lKillMax/2){
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
		if((f_lastWP + MAXTIMEPERWP*((pEdict->v.button&IN_DUCK)?2:1)) < gpGlobals->time){
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

void CBotCS :: Think(void){
	//cout << pEdict->v.view_ofs.Length()<<endl;
	//cout << STRING(pEdict->v.netname) << endl;
	edict_t *pent=0;
	static char item_name[200];
	int index = 0;
	//
	TraceResult tr;
	//TraceResult tr2;
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
			
		RUN_PLAYER_MOVE( pEdict, pEdict->v.v_angle, 0,
			0, 0, pEdict->v.button, 0, byte(g_msecval));
		
		FixIdealYaw();				// this fixes a bug, which does onl occurr in de_dust, cs_gamma_assault etc.
		
		return;
	}
	
	HandleMenu();
	
	//bot_teamnm = UTIL_GetTeam(pEdict);
	
	// if the bot is dead, randomly press fire to respawn...
	if ((pEdict->v.health < 1) || (pEdict->v.deadflag != DEAD_NO))
	{
		if(g_b5th){
			i1C--;
			if(!i1C){
				i1C = 5;
				if(CVAR_BOOL(jb_chat))
					Chat.Talk(this);
			}
		}
		
		if (need_to_initialize)
			SpawnInit();
		
			/*if (RANDOM_LONG(1, 100) > 50)
		lButton = IN_ATTACK;*/
		
		if (g_fFreezeTimeEnd > gpGlobals->time)
			f_move_speed = 0;

		RUN_PLAYER_MOVE( pEdict, pEdict->v.v_angle, f_move_speed,
			0, 0, lButton, 0, byte(g_msecval));
		
		//cout << "deadthinkend" << endl;
		return;
	}
#ifdef _DEBUG
	if(HasSecondary() && !HasPrimary() &&RANDOM_LONG(0,100) < 20)
		LOG_MONEY(UTIL_VarArgs("%s\t%i\n\0",STRING(pEdict->v.netname),m_iBotMoney));
#endif
	if(bRSInit){		// delete ways at and other vars at start of round
		Init();
		
		g_bBombPlanted = false;
		g_fBombExplode = -1;
		
		ResetWPlanning();
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
		Think5th();
	}
	else{
		//b5th=false;
		moved_distance = 2.0;
	}
	//cout << "thinke5th"<<endl;
	
	// save current position as previous
	v_prev_origin = pEdict->v.origin;
	
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
	
	/*if (false){
	}
	else*/
	{		// see if bot can find an enemy...
		if(pBotEnemy) pBotOldEnemy = pBotEnemy;
		pBotEnemy = FindEnemy();
		
		if (pBotEnemy != NULL ){			// does an en exist ?
			if(current_weapon.iId == CS_WEAPON_C4){		// dont try to attack with this
				if(HasPrimary())			// change to primary
					Change2Weapon(HasPrimary());
				else							// or to secondary
					Change2Weapon(HasSecondary());
			}
			if(Task.current
				&& (Task.current->lType&BT_IGNOREENEMY)){
				if(Task.current->p != pBotEnemy){
					Task.NextTask();
				}
				LastEnemyOrigin = pBotEnemy->v.origin;	// store location of last en
				Fight();								// ... and fight against it
				
				if((current_weapon.iId == CS_WEAPON_AWP
					||current_weapon.iId == CS_WEAPON_SCOUT
					||current_weapon.iId == CS_WEAPON_SG550
					||current_weapon.iId == CS_WEAPON_G3SG1)
					&&HasSecondary()){
					Change2Weapon(HasSecondary());
				}
				
				if(!iOnLadder){
					DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin + Vector(0,50,50),10,0,00,200,000,200,10);
					VLookTo = pBotEnemy -> v.origin;
					f_LookTo = gpGlobals->time + 2.0;
					//f_GoBack = 
				}
				else{
					f_LookTo = 0;
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
				Fight();								// ... and fight against it
				VLookTo = pBotEnemy -> v.origin;
				if(!GOrder.lTypeoG)
					f_LookTo = 0;
			}
		}
		
		if (pBotEnemy == NULL
			|| (Task.current&&(Task.current->lType&BT_IGNOREENEMY)))
		{	// no enemy or blinded ...
			if(Task.current&&(Task.current->lType&BT_IGNOREENEMY)){
				if(pBotOldEnemy
					&& !pBotEnemy
					&& !(Action.lAction & BA_SUSPLOC && FVisible(VSuspEn,pEdict))
					){				// there was an enemy
					if(f_LookTo < gpGlobals->time+.3f){
						VLookTo = pBotOldEnemy -> v.origin;
						f_LookTo = gpGlobals->time + 2.0;
						//f_GoBack =
					}
				}
			}
			if(f_blinded_time > gpGlobals->time){
				f_move_speed /= 1.5f;		// slow down
				f_shootbox = f_blinded_time;
				
				if(RANDOM_LONG(0,100) > 96){
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
						&& !(Action.lAction & BA_TKAVOID))
					{
						if(f_UsedRadio < gpGlobals->time - _RADIO_FREQ
							&&CVAR_BOOL(jb_msgradio))
						{
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
						  
						if(lEnt!=-1 &&Task.current->p)		// is always BT_HIDE
						{
							if(FVisible(waypoints[i].origin,Task.current->p)){
								continue;
							}
						}
							
						// skip this waypoint if it's team specific and teams don't match...
						if ((waypoints[i].flags & W_FL_TEAM_SPECIFIC) &&
							((waypoints[i].flags & W_FL_TEAM) != bot_teamnm))
						{
							continue;
						}
							  
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
			if(g_b5th){
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
				
				if(STMem.iCount
					&&!(Action.lAction&BA_SUSPLOC)){
					if((STMem[0].VCalcedOrigin-pEdict->v.origin).Length()<500.0f){
						VSuspEn = STMem[0].VCalcedOrigin;
						f_SuspRem = gpGlobals->time;
						Action.lAction|=BA_SUSPLOC;
					}
				}
				
				if(!GOrder.lTypeoG){
					// has grenade as current weapon ???
					if(IsCWeaponGrenade()
						&& GOrder.fStart < gpGlobals->time - 2){
						if(HasPrimary())
							Change2Weapon(HasPrimary());
						else if(HasSecondary())
							Change2Weapon(HasSecondary());
					}
				}
				
				// vip shouldnt run alone
				if(g_iMapType == MT_AS
					&&m_bIsVIP){
					edict_t *pNearest;
					float fNearest,fNearestVC;
					pNearest = GetNearestPlayer(pEdict,bot_teamnm,fNearest);
					if(pNearest&&fNearest > 500){
						//Task.AddTask ( BT_PAUSE | BT_CROUCH,gpGlobals->time + 3.0,0,0,0);
						f_Pause = f_ducktill = gpGlobals->time + 3.0;
					}
					if(pNearest&&!GetNearestPlayer(pEdict,bot_teamnm,fNearestVC,false,true) && fNearest > 300){
						f_ducktill = gpGlobals->time + 1.0;
					}
					if(!Ordered.pREdict
						&& f_UsedRadio < gpGlobals->time - 5.0
						&& CVAR_BOOL(jb_msgradio)
						&&g_fRoundTime > 6.0){
						SendRadioCommand(RADIO_COVER_ME);
						Ordered.lAction |= BO_COVER;
					}
				}
			}
			
			if(Action.lAction & BA_FIGHT
				&& (Task.current&&!(Task.current->lType&BT_IGNOREENEMY))){			// was last time a fight ??
				Action.lAction &=~ BA_FIGHT;			// del flag
				
				if(pBotOldEnemy){
					VLookTo = pBotOldEnemy -> v.origin;
					f_LookTo = gpGlobals->time + 2.3;
					//f_GoBack =
				}
				
				//f_noWP = gpGlobals->time + RANDOM_FLOAT(.5,1);
				
				if((d_Manner >= 0
					&& pBotOldEnemy
					&& m_iHostagesUsed == 0
					&& IsAlive(pBotOldEnemy)
					&& RANDOM_LONG(0,100) < 70)){			// hunt the enemy down :D
					if(Task.SearchT(BT_HUNT) == -1){
						Task.AddTask(BT_HUNT,gpGlobals->time + 40.0f,0,pBotOldEnemy,0);
						ResetWPlanning();
					}
					else{
						if(RANDOM_LONG(0,100) < 30){
							if(Task.current 
								&& !(Task.current->lType & BT_CAMP)){
								Task.AddTask(BT_CAMP,gpGlobals->time + RANDOM_LONG(1,8),0,0,0);
							}
						}
					}
					if(IsCWeaponSniper()
						&& RANDOM_LONG(0,100) < 60){
						Task.AddTask(BT_CAMP,gpGlobals->time + RANDOM_LONG(2,8),0,0,0);
					}
				}
				// don't use WP for some time
				//f_noWP = gpGlobals->time + 1.0;
				i_CurrWP = -1;
				Action.lAction &=~ BA_SUSPLOC;	// forget susp loc
			}
			
			if(STMem.iCount
				&& f_LastFight < gpGlobals->time - 1.0
				&& f_shootbox > gpGlobals->time 
				&& !STMem[0].bSolid){
				DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin,pEdict->v.origin + Vector(0,50,0),10,0,0,000,200,200,10);
				VATurnTo(STMem[0].VCalcedOrigin - pEdict->v.origin);
				VLookTo = STMem[0].VCalcedOrigin;
				f_LookTo = gpGlobals->time + .5f;
				
				FireWeapon(STMem[0].VCalcedOrigin);
				f_noWP = gpGlobals->time + .2;
			}
			
			// reset angles
			if ((pEdict->v.waterlevel != 2) &&  // is bot NOT under water?
				(pEdict->v.waterlevel != 3))
			{
				// reset pitch to 0 (level horizontally)
				pEdict->v.idealpitch = 1;
				//pEdict->v.v_angle.x = 1;
				fIdealAnglePitch = 1;
				fIdealAnglePitch = -pEdict->v.v_angle.x/3.f;
				fIdealAngleYaw = pEdict->v.v_angle.y;
			}
			else{
				fIdealAnglePitch = -pEdict->v.v_angle.x;
				fIdealAngleYaw = pEdict->v.v_angle.y;
				lButton |= IN_FORWARD;
			}
			
			pEdict->v.angles.z = pEdict->v.v_angle.z = 0;  // reset roll to 0 (straight up and down)
			
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
			if (g_b5th
				&& !Task.Important()
				&& RANDOM_LONG(1, 100) <= 2)
			{
				// set the time that the bot will stop "pausing"
				f_Pause = gpGlobals->time + 0.5;
			}

			// withdraw shield
			if (HasShieldDrawn())
				lButton |= IN_ATTACK2;
		}
	}
	
	HandleGOrder();
	
	if(!bReplay && 
		f_LookTo > gpGlobals->time){		// f_LookTo code
		float f_newspeed;
		float f_newstrafe;
		float f_Dot;
		float f_DotMin = -.5;
		Vector VLDiff,VRDiff;
		
		//if(1||f_GoBack>gpGlobals->time){
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
			
			if(f_LookTo-.3 > gpGlobals->time){
				HeadToward(VLookTo);
				
				if(g_b5th){
					DEBUG_DRAWBEAM(listenserver_edict,pEdict->v.origin+Vector(0,0,15),VLookTo+Vector(0,0,15),4,1,0,255,200,200,100);
				}
			}
			else{
				if(Action.lAction & BA_FIGHT){
				}
				else{
					HeadToward(VRunningTo);
				}
			}
		}
		else{
			f_LookTo = gpGlobals->time - .1;
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
	
	/*if(f_Hide > gpGlobals->time){	// Hide -> do everything to hide ...
		lButton |= IN_DUCK;
		f_move_speed = 0;
	}*/
	
	if (f_Pause > gpGlobals->time || g_fFreezeTimeEnd > gpGlobals->time)  // is the bot "paused"? (faking intelligence and thinking time)
		f_move_speed = 0;  // don't move while pausing
	
	HandleReplay();
	
	// save the previous speed (for checking if stuck)
	prev_speed = f_move_speed;
	
	// copying lButton from bot_t to pEdict->v.button
	pEdict->v.button = lButton; // copy the new pressed buttons to the right location
	
	RUN_PLAYER_MOVE( pEdict, pEdict->v.v_angle, f_move_speed,
		f_strafe, 0, pEdict->v.button, 0, byte(g_msecval));
	
	//BotFixIdealYaw(pEdict);
	
	//cout << "thinkend" << endl;
	
	return;
}

long CBotCS :: IsSecondaryWeapon(long lbit){
	if(lbit & 1<<CS_WEAPON_USP){
		return CS_WEAPON_USP;
	}
	else if(lbit & 1<<CS_WEAPON_GLOCK18){
		return CS_WEAPON_GLOCK18;
	}
	else if(lbit & 1<<CS_WEAPON_DEAGLE){
		return CS_WEAPON_DEAGLE;
	}
	else if(lbit & 1<<CS_WEAPON_ELITE){
		return CS_WEAPON_ELITE;
	}
	else if(lbit & 1<<CS_WEAPON_P228){
		return CS_WEAPON_P228;
	}
	else if(lbit & 1<<CS_WEAPON_FIVESEVEN){		// cs10
		return CS_WEAPON_FIVESEVEN;
	}
	
	return 0;
}

long CBotCS :: IsPrimaryWeapon(long lbit){
	if(lbit & 1<<CS_WEAPON_AK47){
		return CS_WEAPON_AK47;
	}
	if(lbit & 1<<CS_WEAPON_AWP){
		return CS_WEAPON_AWP;
	}
	else if(lbit & 1<<CS_WEAPON_AUG){
		return CS_WEAPON_AUG;
	}
	else if(lbit & 1<<CS_WEAPON_G3SG1){
		return CS_WEAPON_G3SG1;
	}
	else if(lbit & 1<<CS_WEAPON_M249){
		return CS_WEAPON_M249;
	}
	else if(lbit & 1<<CS_WEAPON_M3){
		return CS_WEAPON_M3;
	}
	else if(lbit & 1<<CS_WEAPON_M4A1){
		return CS_WEAPON_M4A1;
	}
	else if(lbit & 1<<CS_WEAPON_MAC10){
		return CS_WEAPON_MAC10;
	}
	else if(lbit & 1<<CS_WEAPON_MP5NAVY){
		return CS_WEAPON_MP5NAVY;
	}
	else if(lbit & 1<<CS_WEAPON_P90){
		return CS_WEAPON_P90;
	}
	else if(lbit & 1<<CS_WEAPON_TMP){
		return CS_WEAPON_TMP;
	}
	else if(lbit & 1<<CS_WEAPON_SCOUT){
		return CS_WEAPON_SCOUT;
	}
	else if(lbit & 1<<CS_WEAPON_SG552){
		return CS_WEAPON_SG552;
	}
	else if(lbit & 1<<CS_WEAPON_XM1014){
		return CS_WEAPON_XM1014;
	}
	else if(lbit & 1<<CS_WEAPON_UMP45){	// cs10
		return CS_WEAPON_UMP45;
	}
	else if(lbit & 1<<CS_WEAPON_SG550){
		return CS_WEAPON_SG550;
	}
	else if(lbit & 1<<CS_WEAPON_GALIL){
		return CS_WEAPON_GALIL;
	}
	else if(lbit & 1<<CS_WEAPON_FAMAS){
		return CS_WEAPON_FAMAS;
	}
	return 0;
}

long CBotCS :: IsShotgunWeapon(long lbit){
	if(lbit & 1<<CS_WEAPON_XM1014){
		return CS_WEAPON_XM1014;
	}
	else if(lbit & 1<<CS_WEAPON_M3){
		return CS_WEAPON_M3;
	}
	return 0;
}

long CBotCS :: IsSubMWeapon(long lbit){
	if(lbit & 1<<CS_WEAPON_UMP45){
		return CS_WEAPON_UMP45;
	}
	else if(lbit & 1<<CS_WEAPON_MAC10){
		return CS_WEAPON_MAC10;
	}
	else if(lbit & 1<<CS_WEAPON_MP5NAVY){
		return CS_WEAPON_MP5NAVY;
	}
	else if(lbit & 1<<CS_WEAPON_P90){
		return CS_WEAPON_P90;
	}
	else if(lbit & 1<<CS_WEAPON_TMP){
		return CS_WEAPON_TMP;
	}
	
	return 0;
}

long CBotCS :: IsMachineGunWeapon(long lbit){
	if(lbit & 1<<CS_WEAPON_M249){
		return CS_WEAPON_M249;
	}
	else
		return 0;
}

long CBotCS :: IsRifleWeapon(long lbit){
	
	if(lbit & 1<<CS_WEAPON_AK47){
		return CS_WEAPON_AK47;
	}
	else if(lbit & 1<<CS_WEAPON_AUG){
		return CS_WEAPON_AUG;
	}
	else if(lbit & 1<<CS_WEAPON_M4A1){
		return CS_WEAPON_M4A1;
	}
	else if(lbit & 1<<CS_WEAPON_GALIL){
		return CS_WEAPON_GALIL;
	}
	else if(lbit & 1<<CS_WEAPON_FAMAS){
		return CS_WEAPON_FAMAS;
	}
	return 0;
}

long CBotCS :: IsSniperWeapon(long lbit){				// return weapons, who allow you to zoom
	if(lbit & 1<<CS_WEAPON_AUG){
		return CS_WEAPON_AUG;
	}
	else if(lbit & 1<<CS_WEAPON_AWP){
		return CS_WEAPON_AWP;
	}
	else if(lbit & 1<<CS_WEAPON_G3SG1){
		return CS_WEAPON_G3SG1;
	}
	else if(lbit & 1<<CS_WEAPON_SCOUT){
		return CS_WEAPON_SCOUT;
	}
	else if(lbit & 1<<CS_WEAPON_SG552){
		return CS_WEAPON_SG552;
	}
	else if(lbit & 1<<CS_WEAPON_SG550){	// cs 10
		return CS_WEAPON_SG550;
	}
	return 0;
}

long CBotCS :: IsGrenadeWeapon(long lbit){
	if(lbit & 1<<CS_WEAPON_HEGRENADE){
		return CS_WEAPON_HEGRENADE;
	}
	else if(lbit & 1<<CS_WEAPON_FLASHBANG){
		return CS_WEAPON_FLASHBANG;
	}
	else if(lbit & 1<<CS_WEAPON_SMOKEGRENADE){
		return CS_WEAPON_SMOKEGRENADE;
	}
	return 0;
}

long CBotCS :: IsKnifeWeapon(long lbit){
	if(lbit & 1<<CS_WEAPON_KNIFE){
		return CS_WEAPON_KNIFE;
	}
	else return 0;
}

long CBotCS :: IsShieldWeapon(long lbit){
	if (lbit & 1<<CS_WEAPON_SHIELD){
		return CS_WEAPON_SHIELD;
	}
	else return 0;
}

bool CBotCS :: HasShield(void){
	// Adapted from Wei Mingzhi's YAPB
	return (strncmp(STRING(pEdict->v.viewmodel), "models/shield/v_shield_", 23) == 0);
}

bool CBotCS :: HasShieldDrawn(void){
	// Adapted from Wei Mingzhi's YAPB
	if (!HasShield())
		return FALSE;

	return (pEdict->v.weaponanim == 6 || pEdict->v.weaponanim == 7);
}

long CBotCS :: WeaponModel2ID(const char *szModel){
	if(!szModel||!*szModel)
		return -1;

	if (strlen(szModel) > 9 && (
		strncmp(szModel, "models/w_", 9) == 0 ||
		strncmp(szModel, "models/p_", 9) == 0 ||
		strncmp(szModel, "models/v_", 9) == 0))
	{
		szModel += 9;
	}
	else if (strlen(szModel) > 23 && (
		strncmp(szModel, "models/shield/p_shield_", 23) == 0 ||
		strncmp(szModel, "models/shield/v_shield_", 23) == 0))
	{
		szModel += 23;
	}

	if(FStrEq(szModel,"mp5.mdl"))
		return CS_WEAPON_MP5NAVY;
	if(FStrEq(szModel,"m4a1.mdl"))
		return CS_WEAPON_M4A1;
	if(FStrEq(szModel,"ak47.mdl"))
		return CS_WEAPON_AK47;
	
	if(FStrEq(szModel,"usp.mdl"))
		return CS_WEAPON_USP;
	if(FStrEq(szModel,"glock18.mdl"))
		return CS_WEAPON_GLOCK18;
	if(FStrEq(szModel,"deagle.mdl"))
		return CS_WEAPON_DEAGLE;
	if(FStrEq(szModel,"elite.mdl"))
		return CS_WEAPON_ELITE;
	if(FStrEq(szModel,"p228.mdl"))
		return CS_WEAPON_P228;
	if(FStrEq(szModel,"fiveseven.mdl"))
		return CS_WEAPON_FIVESEVEN;
	
	if(FStrEq(szModel,"hegrenade.mdl"))
		return CS_WEAPON_HEGRENADE;
	if(FStrEq(szModel,"flashbang.mdl"))
		return CS_WEAPON_FLASHBANG;
	if(FStrEq(szModel,"smokegrenade.mdl"))
		return CS_WEAPON_SMOKEGRENADE;
	
	if(FStrEq(szModel,"awp.mdl"))
		return CS_WEAPON_AWP;
	if(FStrEq(szModel,"aug.mdl"))
		return CS_WEAPON_AUG;
	if(FStrEq(szModel,"g3sg1.mdl"))
		return CS_WEAPON_G3SG1;
	if(FStrEq(szModel,"m249.mdl"))
		return CS_WEAPON_M249;
	if(FStrEq(szModel,"m3.mdl"))
		return CS_WEAPON_M3;
	if(FStrEq(szModel,"mac10.mdl"))
		return CS_WEAPON_MAC10;
	if(FStrEq(szModel,"p90.mdl"))
		return CS_WEAPON_P90;
	if(FStrEq(szModel,"tmp.mdl"))
		return CS_WEAPON_TMP;
	if(FStrEq(szModel,"scout.mdl"))
		return CS_WEAPON_SCOUT;
	if(FStrEq(szModel,"sg552.mdl"))
		return CS_WEAPON_SG552;
	if(FStrEq(szModel,"xm1014.mdl"))
		return CS_WEAPON_XM1014;
	if(FStrEq(szModel,"ump45.mdl"))
		return CS_WEAPON_UMP45;
	if(FStrEq(szModel,"sg550.mdl"))
		return CS_WEAPON_SG550;
	if(FStrEq(szModel,"knife.mdl"))
		return CS_WEAPON_KNIFE;
	if(FStrEq(szModel,"c4.mdl"))
		return CS_WEAPON_C4;
	if(FStrEq(szModel,"galil.mdl"))
		return CS_WEAPON_GALIL;
	if(FStrEq(szModel,"famas.mdl"))
		return CS_WEAPON_FAMAS;
	if(FStrEq(szModel,"shield.mdl"))
		return CS_WEAPON_SHIELD;

	return -1;
}

#define _MAXSTEPDISTANCE	600.0f
#define _MAXSHOOTDISTANCE	1000.0f
#define _CAMPADVANTAGE		2.0f

#define _DELAY_SURPRISED .5f

bool CBotCS :: ReactOnSound(void){
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
	
	int i,iWeapon;
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
			
			if(bot_teamnm != UTIL_GetTeam(pEnt)){		// is in other team - ya can do this, cause the (human) players can look at the radar
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
					iWeapon = WeaponModel2ID(STRING(pEnt->v.weaponmodel));
					if(iWeapon == CS_WEAPON_KNIFE
						|| iWeapon == CS_WEAPON_TMP
						|| (iWeapon == CS_WEAPON_M4A1&&pEnt->v.weaponanim==0 )	// silenced m4a1
						|| (iWeapon == CS_WEAPON_USP&&pEnt->v.weaponanim==0)){	// silenced usp
						fMaxShootDistanceThis /= 2.0;
					}
					else if(iWeapon == CS_WEAPON_HEGRENADE
						|| iWeapon == CS_WEAPON_SMOKEGRENADE
						|| iWeapon == CS_WEAPON_FLASHBANG){
						fMaxShootDistanceThis = 0;
					}
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
					iWeapon = WeaponModel2ID(STRING(pEnt->v.weaponmodel));
					if(iWeapon == CS_WEAPON_KNIFE
						|| iWeapon == CS_WEAPON_TMP
						|| (iWeapon == CS_WEAPON_M4A1&&pEnt->v.weaponanim==0 )	// silenced m4a1
						|| (iWeapon == CS_WEAPON_USP&&pEnt->v.weaponanim==0)){	// silenced usp
						fMaxShootDistanceThis /= 2.0;
					}
					else if(iWeapon == CS_WEAPON_HEGRENADE
						|| iWeapon == CS_WEAPON_SMOKEGRENADE
						|| iWeapon == CS_WEAPON_FLASHBANG){
						fMaxShootDistanceThis = 0;
					}
					if(fDistance < fMaxShootDistanceThis){				// do you hear sb shooting ??
						if(pEnt->v.oldbuttons & (IN_ATTACK|IN_ATTACK2)
							&&fDistance < fSDist){		// attack2 to hear the zoom of awp, p. ex.
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
		f_RWKnife = gpGlobals->time;		// stop running with knife
		
		if(!(Action.lAction & BA_FIGHT)){// if bot's already fighting, set no additional delay
			Action.lAction |= BA_FIGHT;			// set to true, to fake fighting and avoid reseting of delay
			//f_Delay = gpGlobals->time + _DELAY_SURPRISED;		// bot's for a moment surprised - or let's say, needs time to detect en
		}
		return true;
	}
	else if(pSSoundSource){
		if(f_SuspRem < gpGlobals->time - 1.0f){		// more than 1 secs ago ...
			// copy current susp edict's origin to pBot's structure
			VSuspEn = pSSoundSource->v.origin;
			f_SuspRem = gpGlobals->time;
			//bSawSuspLoc = false;
			Action.lAction |= BA_SUSPLOC;		// there is a susp loc, bot has't seen
			if(f_RWKnife > gpGlobals->time)
				f_RWKnife = gpGlobals->time;		// stop running with knife
		}
		else{	// bot has already heard sth. - do nthn
		}
		return true;
	}
	else
		return false;
}

bool CBotCS :: CheckGrenadeThrowing(void){
	if(f_LastFight > gpGlobals->time - 1.f)
		return false;
	TraceResult tr,tr2;
	// is there the possibility of using a HE ?
	if(Action.lAction&BA_SUSPLOC
		&& GOrder.lTypeoG == 0
		&& GOrder.fStart < gpGlobals->time - 10.f
		&& RANDOM_LONG(0,100) < 30
		&& bot_weapons & (1<<CS_WEAPON_HEGRENADE)){
		float fMin = 0;
		float fSuspDist = (VSuspEn - pEdict->v.origin).Length();
		if(fSuspDist < 1200 && fSuspDist > 400){
			if(GetNearestPlayer(pEdict,bot_teamnm,fMin,true,true) == 0){
				if(RANDOM_LONG(0,100) < 70){
					Vector VGTo = Vector(cos(pEdict->v.angles.y * M_PI / 180.0)			// 300 units away in view direction ... probably the flashbang will land there
						,sin(pEdict->v.angles.y * M_PI / 180.0),0)*300 + pEdict->v.origin;
					UTIL_TraceLine(VSuspEn,VGTo,ignore_monsters,ignore_glass,pEdict,&tr);
					UTIL_TraceLine(pEdict->v.origin,VGTo,ignore_monsters,ignore_glass,pEdict,&tr2);
					
					if(tr.flFraction == 1.0
						&&tr2.flFraction == 1.0){
						if(VecCheckToss(&pEdict->v,pEdict->v.origin,VGTo,g_fGravityAdj).Length() > 10){
							GOrder.Attack(VGTo,CS_WEAPON_HEGRENADE);
							return true;
						}
					}
					else{
						Vector VAim = VSuspEn;
						TraceResult tr;
						
						VAim = VSuspEn;
						float fMin = 0;
						GetNearestPlayer(VAim,bot_teamnm,fMin,1000,0);
						if(fMin < 300)
							return false;
						
						if(VecCheckToss(&pEdict->v,pEdict->v.origin,VAim,g_fGravityAdj).Length() > 10){
							GOrder.Attack(VAim,CS_WEAPON_HEGRENADE);
							return true;
						}
					}
				}
			}
		}
	}
	//if(false){}
	else{
		// is there the possibility of using a flashbang ?
		if(Action.lAction&BA_SUSPLOC
			&& GOrder.lTypeoG == 0
			&& GOrder.fStart < gpGlobals->time - 10.f
			&& bot_weapons & (1<<CS_WEAPON_FLASHBANG)){
			float fMin = 0;
			if((VSuspEn - pEdict->v.origin).Length() < 1500){
				if(GetNearestPlayer(pEdict,bot_teamnm,fMin,true,true) == 0){
					if(RANDOM_LONG(0,100) < 70){
						Vector VGTo = Vector(cos(pEdict->v.angles.y * M_PI / 180.0)			// 300 units away in view direction ... probably the flashbang will land there
							,sin(pEdict->v.angles.y * M_PI / 180.0),0)*300 + pEdict->v.origin;
						UTIL_TraceLine(VSuspEn,VGTo,ignore_monsters,ignore_glass,pEdict,&tr);
						UTIL_TraceLine(pEdict->v.origin,VGTo,ignore_monsters,ignore_glass,pEdict,&tr2);
						
						if(tr.flFraction == 1.0
							&&!tr2.flFraction == 1.0){
							if(VecCheckToss(&pEdict->v,pEdict->v.origin,VGTo,g_fGravityAdj).Length() > 10){
								GOrder.Attack(VGTo,CS_WEAPON_FLASHBANG);
								return true;
							}
						}
						else{
							if(iNearWP != -1){
								// check some waypoints which should be visible to the enemy, but not
								// to the throwing bot
								Vector VAim = VSuspEn;
								int iNearSE = WaypointFindNearest(VSuspEn,pEdict,100000,-1,false),i;
								float distance;
								TraceResult tr;
								int iRMR = RANDOM_LONG(0,100)<50?0:1;
								// find a waypoint which is visible from VSusp and NOT from iNearWP
								for (i=0; i < num_waypoints; i++)
								{
									if ((waypoints[i].flags & W_FL_DELETED) == W_FL_DELETED)
										continue;  // skip any deleted waypoints
									
									if((i+iRMR)&1)
										continue;
									
									if(!WPStat.GetVisible(i,iNearSE))	// skip if not visible ...
										continue;
									
									if(WPStat.GetVisible(i,iNearWP))	// skip if visible ...
										continue;
									
									distance = WaypointDistanceFromTo(iNearWP,i,-1);
									
									if(distance > 1500
										|| distance < 400)
										continue;
									
									distance = (pEdict->v.origin - waypoints[i].origin).Length();
									
									if(distance < 400)
										continue;
									
									break;
								}
								if(i < num_waypoints){
									VAim = waypoints[i].origin;
								}
								else
									VAim = VSuspEn;
								if(VecCheckToss(&pEdict->v,pEdict->v.origin,VAim,g_fGravityAdj).Length() > 10){
									GOrder.Attack(VAim,CS_WEAPON_FLASHBANG);
									f_Pause  =gpGlobals->time + 1.0;
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	if(STMem.iCount
		&& GOrder.lTypeoG == 0
		&& RANDOM_LONG(0,100) < 50
		&& bot_weapons&((1<<CS_WEAPON_FLASHBANG)|(1<<CS_WEAPON_HEGRENADE))){
		int ischl;
		float fDistance;
		// check da short term memory
		//
		// loop thru the items
		for(ischl = STMem.iCount;ischl >= 0;ischl --){
			CSTMemItem *p = &(STMem[ischl]);
			if(p->bUsed){
				fDistance = (p->VCalcedOrigin - pEdict->v.origin).Length();
				if(fDistance > 1500 || fDistance < 400)
					continue;
				/*if(FVisible(p->VCalcedOrigin,pEdict))
					continue;*/
				if(VecCheckToss(&pEdict->v,pEdict->v.origin,p->VCalcedOrigin,g_fGravityAdj).Length() > 10){
					if(bot_weapons & (1<<CS_WEAPON_FLASHBANG|1<<CS_WEAPON_HEGRENADE)){
						if(bot_weapons & (1<<CS_WEAPON_FLASHBANG)&&RANDOM_LONG(0,100)<50){
							GOrder.Attack(p->VCalcedOrigin,CS_WEAPON_FLASHBANG);
							return true;
						}
						else if(bot_weapons & (1<<CS_WEAPON_HEGRENADE)){
							float fMin = 0;
							GetNearestPlayer(p->VCalcedOrigin,bot_teamnm,fMin,1000,0);
							if(fMin > 300){
								GOrder.Attack(p->VCalcedOrigin,CS_WEAPON_HEGRENADE);
							}
							return true;
						}
					}else{
						if(bot_weapons & (1<<CS_WEAPON_FLASHBANG)){
							GOrder.Attack(p->VCalcedOrigin,CS_WEAPON_FLASHBANG);
							return true;
						}
						else if(bot_weapons & (1<<CS_WEAPON_HEGRENADE)){
							float fMin = 0;
							GetNearestPlayer(p->VCalcedOrigin,bot_teamnm,fMin,1000,0);
							if(fMin > 300){
								GOrder.Attack(p->VCalcedOrigin,CS_WEAPON_HEGRENADE);
							}
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

Vector CBotCS :: BodyTarget( edict_t *pentBotEnemy){
	Vector target;
	float f_distance;
	
	f_distance = (pentBotEnemy->v.origin - pEdict->v.origin).Length();
	
	// HACKHACK : TODO : Fav weapon handling, sniper weapon handling. This has to be a bit more sophisticated, because the siming is a process, cannot be done a simply changing one shitty variable in one function
	
	/*if(pBot->lPWeapon==pBot->current_weapon.iId){	// favourite weapons will be more accurate
		fAccuracy /= 1.1f;
	}*/
	
	if(current_weapon.iId == CS_WEAPON_AWP||
		current_weapon.iId == CS_WEAPON_G3SG1||
		current_weapon.iId == CS_WEAPON_SG550){
		// reset VOFFSET
		v_Offset.x = v_Offset.y = v_Offset.z = 0.0;
		target = pentBotEnemy->v.origin;
	}
	else{
		target = pentBotEnemy->v.origin + pentBotEnemy->v.view_ofs * (.5+f_AimHead/2.0);
	}
	
	return target;
}

void CBotCS :: SendRadioCommand(int iType){
	switch(iType){
	case RADIO_IM_IN_POSITION:
		FakeClientCommand(pEdict,"radio3;menuselect 5");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_COVER_ME:
		FakeClientCommand(pEdict,"radio1;menuselect 1");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_FOLLOW_ME:
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_NEED_ASSISTANCE:
	case RADIO_NEED_BACKUP:
		FakeClientCommand(pEdict,"radio3;menuselect 3");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_ENEMY_DOWN:
		FakeClientCommand(pEdict,"radio3;menuselect 9");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_ENEMY_SPOTTED:
		FakeClientCommand(pEdict,"radio3;menuselect 2");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_AFFIRMATIVE:
		FakeClientCommand(pEdict,"radio3;menuselect 1");
		FakeClientCommand(pEdict,"menuselect 1");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_NEGATIVE:
		FakeClientCommand(pEdict,"radio3;menuselect 8");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_GOGOGO:
		FakeClientCommand(pEdict,"radio2;menuselect 1");
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_YOU_TAKE_THE_POINT:
		if(!m_bIsVIP){
			FakeClientCommand(pEdict,"radio1;menuselect 2");
		}
		f_UsedRadio = gpGlobals->time;
		break;
	case RADIO_SECTOR_CLEAR:
		FakeClientCommand(pEdict,"radio3;menuselect 4");			// # sector clear
		f_UsedRadio = gpGlobals->time;
		break;
	default:
		break;
	}
}

/* old jump over code */
/*if (tr.flFraction == 1.0)
{
	UTIL_TraceLine(v_check, v_down, ignore_monsters,pEdict->v.pContainingEntity, &tr);
	//if (pEdictPlayer)
	//	WaypointDrawBeam(pEdictPlayer, v_check, v_down, 30, 0, 0, f_strafe, 255, 250, 5);

	if (!tr.fInWater)
	{
		// height from ground (300 'cause ya went 300 lower)
		flast_height = tr.flFraction * 300.0;

		// check for jumping
		Vector v_veln = v_comp;
		v_veln.Normalize();
		Vector VCheckJF = pEdict->v.origin + (v_veln * 100.0) + Vector(0,0,50);
		Vector VCheckJS = pEdict->v.origin + (v_veln * 150.0) - Vector(0,0,150);	// n bisschen schraeg
  
		UTIL_TraceLine(VCheckJF, VCheckJS, ignore_monsters,pEdict->v.pContainingEntity, &tr);
		//if (pEdictPlayer)
		//	WaypointDrawBeam(pEdictPlayer,VCheckJF,VCheckJS,10,10,255,255,255,255,50);
				
		float fJumpTo = tr.flFraction * 206.0 - 50.0;
				  
		if (flast_height > 100.0f					// would fall at least a bit.
			&& fJumpTo < flast_height - 30.0f		// jumping would be less dangerous
			&& fJumpTo < 150.0)				// but there where the bot should jump has to be something, too
		{
			bJumpOver = true;
		}
		else
		{
			bJumpOver = false;
		}

		if (bJumpOver)
		{
			iJumpOC++;
			InstantTurn (pBot);
		
			if (!(pEdict->v.button & IN_JUMP) && iJumpOC > 2)
			{
				lButton |= IN_JUMP;
				f_LookTo = 0.0;
				HeadToward(pEdict,VRunningTo);
				f_ducktill = gpGlobals->time + 1.0;
				f_dont_avoid = gpGlobals->time + .5;
				f_pause_time = gpGlobals->time;
				//DEBUG_CLIENTCOMMAND(pEdict,"say jumping over");
				// dont strafe
				f_strafe = 0.0;
				f_move_speed = pEdict->v.maxspeed;
			}
			else
			{
				f_dont_avoid = gpGlobals->time + .5;
				//f_move_speed /=2.0;
			}
		}
		else if (flast_height > 220)
		{
			iJumpOC=0;		// reset jumping delay var
			  
			f_move_speed = -f_move_speed;	// negate ya speed
			f_strafe = - f_strafe;
			lButton &=~ IN_JUMP;						// don't jump
		}
	}
}
*/
