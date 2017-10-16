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

//#include <iostream.h>
#include <stdio.h>
#include <string.h>

#include "extdll.h"
#include "util.h"

#ifdef USE_METAMOD
#define SDK_UTIL_H  // util.h already included
#include "meta_api.h"
#endif /* USE_METAMOD */

#include "NNWeapon.h"

#include "bot_weapons.h"
#include "Commandfunc.h"
#include "globalvars.h"

#include "BotNNDefs.h"

bool bLoadedLPB = false;

#define _DELAY_NONE .3

NNWeapon :: NNWeapon(){
//	Init();
}

void NNWeapon :: Init(void){
	memset(dpWeaV,0,sizeof(double)*MAX_MOD*32);
	memset(dpWeaponPause,0,sizeof(double)*MAX_MOD*32);
	memset(dpWeaponPauseTime,0,sizeof(double)*MAX_MOD*32);
	memset(ipClipSize,0,sizeof(int)*MAX_MOD*32);
	memset(iCost,0,sizeof(int)*MAX_MOD*32);
	memset(iTeam,0,sizeof(int)*MAX_MOD*32);
	memset(fPBuyProb,0,sizeof(float)*MAX_MOD*32);

	dpWeaV[CSTRIKE_DLL][CS_WEAPON_P228]			= dr_weapon_p228;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_SHIELD]		= dr_weapon_shield;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_SCOUT]		= dr_weapon_scout;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_HEGRENADE]	= dr_weapon_hegrenade;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_XM1014]		= dr_weapon_xm1014;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_C4]			= dr_weapon_c4;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_MAC10]		= dr_weapon_mac10;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_AUG]			= dr_weapon_aug;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_SMOKEGRENADE]	= dr_weapon_smokegrenade;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_ELITE]		= dr_weapon_elite;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_FIVESEVEN]	= dr_weapon_fiveseven;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_UMP45]		= dr_weapon_ump45;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_SG550]		= dr_weapon_sg550;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_GALIL]		= dr_weapon_galil;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_FAMAS]		= dr_weapon_famas;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_USP]			= dr_weapon_usp;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_GLOCK18]		= dr_weapon_glock18;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_AWP]			= dr_weapon_awp;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_MP5NAVY]		= dr_weapon_mp5navy;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_M249]			= dr_weapon_m249;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_M3]			= dr_weapon_m3;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_M4A1]			= dr_weapon_m4a1;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_TMP]			= dr_weapon_tmp;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_G3SG1]		= dr_weapon_g3sg1;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_FLASHBANG]	= dr_weapon_flashbang;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_DEAGLE]		= dr_weapon_deagle;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_SG552]		= dr_weapon_sg552;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_AK47]			= dr_weapon_ak47;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_KNIFE]		= dr_weapon_knife;
	dpWeaV[CSTRIKE_DLL][CS_WEAPON_P90]			= dr_weapon_p90;
	
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_P228]			= 0;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_SHIELD]		= 0;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_SCOUT]			= 0;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_HEGRENADE]		= 50;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_XM1014]		= 0;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_C4]			= 0;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_MAC10]			= 8;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_AUG]			= 8;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_SMOKEGRENADE]	= 50;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_ELITE]			= 0;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_FIVESEVEN]		= 5;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_UMP45]			= 1;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_SG550]			= 5;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_GALIL]			= 10;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_FAMAS]			= 10;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_USP]			= 10;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_GLOCK18]		= 10;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_AWP]			= 0;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_MP5NAVY]		= 10;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_M249]			= 25;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_M3]			= 0;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_M4A1]			= 10;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_TMP]			= 10;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_G3SG1]			= 0;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_FLASHBANG]		= 50;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_DEAGLE]		= 6;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_SG552]			= 10;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_AK47]			= 10;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_KNIFE]			= 0;
	dpWeaponPause[CSTRIKE_DLL][CS_WEAPON_P90]			= 0;

	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_P228]			= 0;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_SHIELD]		= 0;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_SCOUT]			= 0;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_HEGRENADE]		= .1;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_XM1014]		= 0;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_C4]			= 0;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_MAC10]			= .7;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_AUG]			= .7;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_SMOKEGRENADE]	= .1;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_ELITE]			= .3;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_FIVESEVEN]		= .5;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_UMP45]			= .6;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_SG550]			= .5;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_GALIL]			= .7;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_FAMAS]			= .7;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_USP]			= .2;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_GLOCK18]		= .2;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_AWP]			= 0;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_MP5NAVY]		= .7;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_M249]			= .8;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_M3]			= 0;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_M4A1]			= .7;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_TMP]			= .7;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_G3SG1]			= 0;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_FLASHBANG]		= .1;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_DEAGLE]		= .3;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_SG552]			= .7;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_AK47]			= .7;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_KNIFE]			= 0;
	dpWeaponPauseTime[CSTRIKE_DLL][CS_WEAPON_P90]			= .6;

	/*dpWeaponDelay[CS_WEAPON_P228]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_SHIELD]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_SCOUT]			= .4;
	dpWeaponDelay[CS_WEAPON_HEGRENADE]		= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_XM1014]			= .3;
	dpWeaponDelay[CS_WEAPON_C4]				= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_MAC10]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_AUG]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_SMOKEGRENADE]	= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_ELITE]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_FIVESEVEN]		= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_UMP45]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_SG550]			= .4;
	dpWeaponDelay[CS_WEAPON_GALIL]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_FAMAS]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_USP]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_GLOCK18]		= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_AWP]			= .5;
	dpWeaponDelay[CS_WEAPON_MP5NAVY]		= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_M249]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_M3]				= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_M4A1]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_TMP]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_G3SG1]			= .3;
	dpWeaponDelay[CS_WEAPON_FLASHBANG]		= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_DEAGLE]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_SG552]			= .3;
	dpWeaponDelay[CS_WEAPON_AK47]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_KNIFE]			= _DELAY_NONE;
	dpWeaponDelay[CS_WEAPON_P90]			= _DELAY_NONE;*/

	ipClipSize[CSTRIKE_DLL][CS_WEAPON_P228]				= 13;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_SHIELD]			= 0;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_SCOUT]			= 10;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_HEGRENADE]		= 1;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_XM1014]			= 7;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_C4]				= 1000;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_MAC10]			= 30;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_AUG]				= 30;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_SMOKEGRENADE]		= 1;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_ELITE]			= 30;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_FIVESEVEN]		= 20;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_UMP45]			= 30;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_SG550]			= 30;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_GALIL]			= 35;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_FAMAS]			= 25;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_USP]				= 12;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_GLOCK18]			= 20;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_AWP]				= 10;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_MP5NAVY]			= 30;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_M249]				= 100;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_M3]				= 8;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_M4A1]				= 30;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_TMP]				= 30;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_G3SG1]			= 20;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_FLASHBANG]		= 2;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_DEAGLE]			= 7;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_SG552]			= 30;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_AK47]				= 30;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_KNIFE]			= 1;
	ipClipSize[CSTRIKE_DLL][CS_WEAPON_P90]				= 50;

	for(int ischl=0;ischl < 32;ischl++){
		iCost[CSTRIKE_DLL][ischl] = 60000;				// just set evrything high enough to prevent bot from buy non def stuff
		iTeam[CSTRIKE_DLL][ischl] = -1;							// standard is no team
		fPBuyProb[CSTRIKE_DLL][ischl] = 0;
	}

	iCost[CSTRIKE_DLL][CS_WEAPON_P228]				= 600;
	iCost[CSTRIKE_DLL][CS_WEAPON_SHIELD]			= 2200;
	iCost[CSTRIKE_DLL][CS_WEAPON_SCOUT]				= 2750;
	iCost[CSTRIKE_DLL][CS_WEAPON_HEGRENADE]			= 300;
	iCost[CSTRIKE_DLL][CS_WEAPON_XM1014]			= 3000;
	iCost[CSTRIKE_DLL][CS_WEAPON_MAC10]				= 1400;
	iCost[CSTRIKE_DLL][CS_WEAPON_AUG]				= 3500;
	iCost[CSTRIKE_DLL][CS_WEAPON_SMOKEGRENADE]		= 300;
	iCost[CSTRIKE_DLL][CS_WEAPON_ELITE]				= 800;
	iCost[CSTRIKE_DLL][CS_WEAPON_FIVESEVEN]			= 750;
	iCost[CSTRIKE_DLL][CS_WEAPON_UMP45]				= 1700;
	iCost[CSTRIKE_DLL][CS_WEAPON_SG550]				= 4200;
	iCost[CSTRIKE_DLL][CS_WEAPON_GALIL]				= 1700;
	iCost[CSTRIKE_DLL][CS_WEAPON_FAMAS]				= 2250;
	iCost[CSTRIKE_DLL][CS_WEAPON_USP]				= 500;
	iCost[CSTRIKE_DLL][CS_WEAPON_GLOCK18]			= 400;
	iCost[CSTRIKE_DLL][CS_WEAPON_AWP]				= 4750;
	iCost[CSTRIKE_DLL][CS_WEAPON_MP5NAVY]			= 1500;
	iCost[CSTRIKE_DLL][CS_WEAPON_M249]				= 5750;
	iCost[CSTRIKE_DLL][CS_WEAPON_M3]				= 1700;
	iCost[CSTRIKE_DLL][CS_WEAPON_M4A1]				= 3100;
	iCost[CSTRIKE_DLL][CS_WEAPON_TMP]				= 1250;
	iCost[CSTRIKE_DLL][CS_WEAPON_G3SG1]				= 5000;
	iCost[CSTRIKE_DLL][CS_WEAPON_FLASHBANG]			= 300;
	iCost[CSTRIKE_DLL][CS_WEAPON_DEAGLE]			= 650;
	iCost[CSTRIKE_DLL][CS_WEAPON_SG552]				= 3500;
	iCost[CSTRIKE_DLL][CS_WEAPON_AK47]				= 2500;
	iCost[CSTRIKE_DLL][CS_WEAPON_P90]				= 2350;

	iTeam[CSTRIKE_DLL][CS_WEAPON_SHIELD]			= 1;		// ct=>1;te=>0
	iTeam[CSTRIKE_DLL][CS_WEAPON_AUG]				= 1;
	iTeam[CSTRIKE_DLL][CS_WEAPON_MAC10]				= 0;
	iTeam[CSTRIKE_DLL][CS_WEAPON_SG550]				= 1;
	iTeam[CSTRIKE_DLL][CS_WEAPON_GALIL]				= 0;
	iTeam[CSTRIKE_DLL][CS_WEAPON_FAMAS]				= 1;
	iTeam[CSTRIKE_DLL][CS_WEAPON_M4A1]				= 1;
	iTeam[CSTRIKE_DLL][CS_WEAPON_TMP]				= 1;
	iTeam[CSTRIKE_DLL][CS_WEAPON_SG552]				= 0;
	iTeam[CSTRIKE_DLL][CS_WEAPON_AK47]				= 0;

	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_SHIELD]		= 5;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_SCOUT]			= 5;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_XM1014]		= 5;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_MAC10]			= 5;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_AUG]			= 20;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_UMP45]			= 15;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_SG550]			= 5;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_GALIL]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_FAMAS]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_AWP]			= 8;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_MP5NAVY]		= 20;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_M249]			= 2;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_M3]			= 3;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_M4A1]			= 20;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_TMP]			= 5;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_G3SG1]			= 5;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_SG552]			= 20;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_AK47]			= 20;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_P90]			= 5;

	/*fPBuyProb[CSTRIKE_DLL][CS_WEAPON_SHIELD]		= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_SCOUT]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_XM1014]		= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_MAC10]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_AUG]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_UMP45]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_SG550]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_GALIL]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_FAMAS]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_AWP]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_MP5NAVY]		= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_M249]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_M3]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_M4A1]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_TMP]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_G3SG1]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_SG552]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_AK47]			= 10;
	fPBuyProb[CSTRIKE_DLL][CS_WEAPON_P90]			= 10;*/

	float fTemp[32];
	memcpy(fTemp,fPBuyProb[CSTRIKE_DLL],sizeof(float) * 32);
	float fSum=0,fLast=0;
	long lschl;
	for(lschl=0;lschl<32;lschl++)
		fSum+=fPBuyProb[CSTRIKE_DLL][lschl];
	for(lschl=0;lschl<32;lschl++){
		/*if(fTemp[lschl]<0)
			continue;*/
		fPBuyProb[CSTRIKE_DLL][lschl] = fLast + fTemp[lschl] / fSum * 100.0f;
		fLast = fPBuyProb[CSTRIKE_DLL][lschl];
		//cout << fLast << endl;
		if(fTemp[lschl] == .0)
			fPBuyProb[CSTRIKE_DLL][lschl] = 0;
		//cout << fPBuyProb[CSTRIKE_DLL][lschl] << endl;
	}
	
	// do DoD stuff		-------------									DoD--------------
	dpWeaV[DOD_DLL][DOD_WEAPON_GARAND]			= .6;
	dpWeaV[DOD_DLL][DOD_WEAPON_THOMPSON]		= .4;
	dpWeaV[DOD_DLL][DOD_WEAPON_SPRING]			= 1.0;
	dpWeaV[DOD_DLL][DOD_WEAPON_BAR]				= .5;
	dpWeaV[DOD_DLL][DOD_WEAPON_COLT]			= -.3;
	dpWeaV[DOD_DLL][DOD_WEAPON_GERKNIFE]		= -1.0;
	dpWeaV[DOD_DLL][DOD_WEAPON_AMERKNIFE]		= -1.0;
	dpWeaV[DOD_DLL][DOD_WEAPON_HANDGRENADE]		= -.8;
	dpWeaV[DOD_DLL][DOD_WEAPON_HANDGRENADE_EX]	= -.8;
	
	dpWeaV[DOD_DLL][DOD_WEAPON_KAR]				= .6;
	dpWeaV[DOD_DLL][DOD_WEAPON_MP40]			= .4;
	dpWeaV[DOD_DLL][DOD_WEAPON_GEWEHR]			= 1.0;
	dpWeaV[DOD_DLL][DOD_WEAPON_MP44]			= .5;
	dpWeaV[DOD_DLL][DOD_WEAPON_LUGER]			= -.3;
	dpWeaV[DOD_DLL][DOD_WEAPON_STICKGRENADE]	= -.8;
	dpWeaV[DOD_DLL][DOD_WEAPON_STICKGRENADE_EX]	= -.8;

	dpWeaV[DOD_DLL][DOD_WEAPON_30CAL]			= -1.0;
	dpWeaV[DOD_DLL][DOD_WEAPON_MG34]			= -1.0;
	dpWeaV[DOD_DLL][DOD_WEAPON_MG42]			= -1.0;

	/* copy stuff for csclassic */
	memcpy(&(dpWeaV[CSCLASSIC_DLL]),&(dpWeaV[CSTRIKE_DLL]),sizeof(double)*32);
	memcpy(&(dpWeaponPause[CSCLASSIC_DLL]),&(dpWeaponPause[CSTRIKE_DLL]),sizeof(double)*32);
	memcpy(&(dpWeaponPauseTime[CSCLASSIC_DLL]),&(dpWeaponPauseTime[CSTRIKE_DLL]),sizeof(double)*32);

	memcpy(&(ipClipSize[CSCLASSIC_DLL]),&(ipClipSize[CSTRIKE_DLL]),sizeof(int)*32);
	memcpy(&(iCost[CSCLASSIC_DLL]),&(iCost[CSTRIKE_DLL]),sizeof(int)*32);
	memcpy(&(iTeam[CSCLASSIC_DLL]),&(iTeam[CSTRIKE_DLL]),sizeof(int)*32);

	memcpy(&(fPBuyProb[CSCLASSIC_DLL]),&(fPBuyProb[CSTRIKE_DLL]),sizeof(float)*32);
	/* ------------------------ */
}

NNWeapon :: ~NNWeapon(){}

void NNWeapon :: Load(const char *szFileName){
	char szfilename[80];
	UTIL_BuildFileName(szfilename, sizeof(szfilename), "joebot/%s", szFileName);
	LoadFile(szfilename);
}

void NNWeapon :: LoadFile(const char *szFileName){
	char *szFileContent,*szAct,szName[200];
	long lFileSize,lschl;
	FILE *fhd;
	//char szType[200],szWeapon[200];
	
	if(lFileSize = GetFileSize(szFileName))
	{
		szFileContent = new char[lFileSize+1];
		memset(szFileContent,0,sizeof(char)*(lFileSize+1));
		
		if(fhd = fopen(szFileName,"r")){
			LOG_MESSAGE(PLID, "Loading weapon buy probabilities file: %s", szFileName);

			//fread(szFileContent,sizeof(char),lFileSize,fhd);
			char *p = szFileContent;
			while (!feof(fhd)){
				*p = fgetc(fhd);
				p ++;
			}
			
			fclose(fhd);
			
			Preprocess(szFileContent);		// remove uninteresting stuff
			
			szAct = szFileContent;

			for(lschl=0;lschl < 32;lschl ++){
				if(strlen(weapon_defs[lschl].szClassname)){
					if(szAct = strstr(szFileContent,weapon_defs[lschl].szClassname)){
						snprintf(szName,sizeof(szName),"%s %%f",weapon_defs[lschl].szClassname);
						//cout << szName << endl;
						sscanf(szAct,szName,&fPBuyProb[mod_id][lschl]);
						//cout << fPBuyProb[mod_id][lschl] << endl;
					}
				}
			}
		}
		else{
			LOG_MESSAGE(PLID, "Problems loading weapon buy probabilities file: %s", szFileName);
		}
		delete [] szFileContent;
	}
	else{
		LOG_MESSAGE(PLID, "Problems loading weapon buy probabilities file: %s", szFileName);
	}
}
