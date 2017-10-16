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
// bot_client.cpp
//

#include "extdll.h"
#include "util.h"
#include "engine.h"
#include "cbase.h"

#ifdef USE_METAMOD
#define SDK_UTIL_H  // util.h already included
#include "meta_api.h"
#endif /* USE_METAMOD */

#include "bot_client.h"

#include "bot.h"
#include "bot_modid.h"
#include "CBotCS.h"
#include "CBotDOD.h"
#include "globalvars.h"

// types of damage to ignore...
#define IGNORE_DAMAGE (DMG_CRUSH | DMG_FREEZE | DMG_SHOCK | DMG_DROWN | \
	DMG_NERVEGAS | DMG_RADIATION | DMG_DROWNRECOVER | \
	DMG_ACID | DMG_SLOWBURN | DMG_SLOWFREEZE)

float f_LastRadio;
char szLastSayText[80];				// if somebody answered this, set szLastSayText[0] = 0;
int iLastSayClient;

char szDeadLastSayText[80];				// if somebody answered this, set szLastSayText[0] = 0;
int iDeadLastSayClient;

bot_weapon_t weapon_defs[MAX_WEAPONS]; // array of weapon definitions

botmsg_t botmsgs[MAX_REG_MSGS];
#ifndef USE_METAMOD
usermsg_t usermsgs[MAX_REG_MSGS];
int usermsgs_count = 0;
#endif /* USE_METAMOD */
int msg_state = 0;

// This message is sent when the Counter-Strike VGUI menu is displayed.
void BotClient_CS_VGUIMenu(void *p, int bot_index){
	if (msg_state == 0){
		if ((*(int *)p) == 2)  // is it a team select menu?
			bots[bot_index]->start_action = MSG_CS_TEAM_SELECT;
		else if ((*(int *)p) == 26)  // is it a terrorist model select menu?
			bots[bot_index]->start_action = MSG_CS_T_SELECT;
		else if ((*(int *)p) == 27)  // is it a counter-terrorist model select menu?	
			bots[bot_index]->start_action = MSG_CS_CT_SELECT;
	}
}

void BotClient_DOD_VGUIMenu(void *p, int bot_index)
{
	if (msg_state == 0){
		if ((*(int *)p) == 2)  // is it a team select menu?
			bots[bot_index]->start_action = MSG_DOD_TEAM_SELECT;
		else if ((*(int *)p) == 10)  // is is a class selection axis menu?
			bots[bot_index]->start_action = MSG_DOD_CLASS_SELECT_AX;
		else if ((*(int *)p) == 9)  // is is a class selection allies menu?
			bots[bot_index]->start_action = MSG_DOD_CLASS_SELECT_AL;
		else if ((*(int *)p) == 21)  // is is a class selection allies menu?	// para
			bots[bot_index]->start_action = MSG_DOD_CLASS_SELECT_AL_PARA;	
		else if ((*(int *)p) == 22)  // is is a class selection axis menu?		// para
			bots[bot_index]->start_action = MSG_DOD_CLASS_SELECT_AX_PARA;
		else if ((*(int *)p) == 19)  // is is a class selection allies menu of the sergeant?
			bots[bot_index]->start_action = MSG_DOD_WPN_SELECT_SERGEANT;
		else if ((*(int *)p) == 20)  // is is a class selection axis menu for machinegunners ?
			bots[bot_index]->start_action = MSG_DOD_WPN_SELECT_MACHINE;
		else if ((*(int *)p) == 23)  // is is a class selection axis menu for fg42 ?
			bots[bot_index]->start_action = MSG_DOD_WPN_SELECT_FG42;
		else if ((*(int *)p) == 24)  // is is a class selection axis menu for riflemen ?
			bots[bot_index]->start_action = MSG_DOD_WPN_SELECT_RIFLEMAN;
		else if ((*(int *)p) == 25)  // is is a class selection axis menu for grenadiers ?
			bots[bot_index]->start_action = MSG_DOD_WPN_SELECT_GRENADIER;
	}
}

// This message is sent when a menu is being displayed in Counter-Strike.
void BotClient_CS_ShowMenu(void *p, int bot_index){
	if (msg_state < 3)
		return;  // ignore first 3 fields of message
	
	if (FStrEq((char *)p, "#Team_Select"))  // team select menu?
		bots[bot_index]->start_action = MSG_CS_TEAM_SELECT;
	else if (FStrEq((char *)p, "#Terrorist_Select"))  // T model select?
		bots[bot_index]->start_action = MSG_CS_T_SELECT;
	else if (FStrEq((char *)p, "#CT_Select"))  // CT model select menu?
		bots[bot_index]->start_action = MSG_CS_CT_SELECT;
}


// This message is sent when a client joins the game.  All of the weapons
// are sent with the weapon ID and information about what ammo is used.
void BotClient_Valve_WeaponList(void *p, int bot_index){
	static bot_weapon_t bot_weapon;
	
	if (msg_state == 0){
		bot_weapon.iAmmoClip = -1;		// just init
		strcpy(bot_weapon.szClassname, (char *)p);
	}
	else if (msg_state == 1)
		bot_weapon.iAmmo1 = *(int *)p;  // ammo index 1
	else if (msg_state == 2)
		bot_weapon.iAmmo1Max = *(int *)p;  // max ammo1
	else if (msg_state == 3)
		bot_weapon.iAmmo2 = *(int *)p;  // ammo index 2
	else if (msg_state == 4)
		bot_weapon.iAmmo2Max = *(int *)p;  // max ammo2
	else if (msg_state == 5)
		bot_weapon.iSlot = *(int *)p;  // slot for this weapon
	else if (msg_state == 6)
		bot_weapon.iPosition = *(int *)p;  // position in slot
	else if (msg_state == 7)
		bot_weapon.iId = *(int *)p;  // weapon ID
	else if (msg_state == 8){
		bot_weapon.iFlags = *(int *)p;  // flags for weapon (WTF???)
		
		// store away this weapon with it's ammo information...
		weapon_defs[bot_weapon.iId] = bot_weapon;
	}
}

void BotClient_DOD_WeaponList(void *p, int bot_index)
{
	static bot_weapon_t bot_weapon;
	
	if (msg_state == 0)
		strcpy(bot_weapon.szClassname, (char *)p);
	else if (msg_state == 1)
		bot_weapon.iAmmo1 = *(int *)p;  // ammo index 1
	else if (msg_state == 2)
		bot_weapon.iAmmo1Max = *(int *)p;  // max ammo1
	else if (msg_state == 3)
		bot_weapon.iAmmo2 = *(int *)p;  // ammo index 2
	else if (msg_state == 4)
		bot_weapon.iAmmo2Max = *(int *)p;  // max ammo2
	else if (msg_state == 5)
		bot_weapon.iSlot = *(int *)p;  // slot for this weapon
	else if (msg_state == 6)
		bot_weapon.iPosition = *(int *)p;  // position in slot
	else if (msg_state == 7)
		bot_weapon.iId = *(int *)p;  // weapon ID
	else if (msg_state == 8)
		bot_weapon.iFlags = *(int *)p;  // flags for weapon (WTF???)
	else if (msg_state == 9){
		bot_weapon.iAmmoClip = *(int *)p;  // clip size
		
		// store away this weapon with it's ammo information...
		weapon_defs[bot_weapon.iId] = bot_weapon;
		LOG_WEAPONS(UTIL_VarArgs("%i %s\n",bot_weapon.iId,bot_weapon.szClassname));
	}
}

void BotClient_CS_WeaponList(void *p, int bot_index){
	// this is just like the Valve Weapon List message
	BotClient_Valve_WeaponList(p, bot_index);
}

void BotClient_Gearbox_WeaponList(void *p, int bot_index){
	// this is just like the Valve Weapon List message
	BotClient_Valve_WeaponList(p, bot_index);
}


// This message is sent when a weapon is selected (either by the bot chosing
// a weapon or by the server auto assigning the bot a weapon).
void BotClient_Valve_CurWeapon(void *p, int bot_index){
	static int iState;
	static int iId;
	static int iClip;
	
	if (msg_state == 0)
		iState = *(int *)p;  // state of the current weapon (WTF???)
	else if (msg_state == 1)
		iId = *(int *)p;  // weapon ID of current weapon
	else if (msg_state == 2){
		if (iId <= 31 && iState){			// &&iState added by @$3.1415rin
			iClip = *(int *)p;  // ammo currently in the clip for this weapon
			
			bots[bot_index]->bot_weapons |= (1<<iId);  // set this weapon bit
			
			bots[bot_index]->current_weapon.iId = iId;
			bots[bot_index]->current_weapon.iClip = iClip;
			
			// update the ammo counts for this weapon...
			bots[bot_index]->current_weapon.iAmmo1 =
				bots[bot_index]->m_rgAmmo[weapon_defs[iId].iAmmo1];
			bots[bot_index]->current_weapon.iAmmo2 =
				bots[bot_index]->m_rgAmmo[weapon_defs[iId].iAmmo2];
		}
	}
}

void BotClient_CS_CurWeapon(void *p, int bot_index){
	// this is just like the Valve Current Weapon message
	BotClient_Valve_CurWeapon(p, bot_index);
}

void BotClient_Gearbox_CurWeapon(void *p, int bot_index){
	// this is just like the Valve Current Weapon message
	BotClient_Valve_CurWeapon(p, bot_index);
}

void BotClient_DOD_CurWeapon(void *p, int bot_index)
{
	// this is just like the Valve Current Weapon message
	BotClient_Valve_CurWeapon(p, bot_index);
}

// This message is sent whenever ammo ammounts are adjusted (up or down).
void BotClient_Valve_AmmoX(void *p, int bot_index){
	static int index;
	static int ammount;
	int ammo_index;
	
	if (msg_state == 0)
		index = *(int *)p;  // ammo index (for type of ammo)
	else if (msg_state == 1){
		ammount = *(int *)p;  // the ammount of ammo currently available
		
		bots[bot_index]->m_rgAmmo[index] = ammount;  // store it away
		
		ammo_index = bots[bot_index]->current_weapon.iId;
		
		// update the ammo counts for this weapon...
		bots[bot_index]->current_weapon.iAmmo1 =
			bots[bot_index]->m_rgAmmo[weapon_defs[ammo_index].iAmmo1];
		bots[bot_index]->current_weapon.iAmmo2 =
			bots[bot_index]->m_rgAmmo[weapon_defs[ammo_index].iAmmo2];
	}
}

void BotClient_CS_AmmoX(void *p, int bot_index){
	// this is just like the Valve AmmoX message
	BotClient_Valve_AmmoX(p, bot_index);
}

void BotClient_Gearbox_AmmoX(void *p, int bot_index){
	// this is just like the Valve AmmoX message
	BotClient_Valve_AmmoX(p, bot_index);
}

void BotClient_DOD_AmmoX(void *p, int bot_index)
{
	// this is just like the Valve AmmoX message
	BotClient_Valve_AmmoX(p, bot_index);
}

void BotClient_Valve_SayText(void *p, int bot_index){
	static short int index;
	static bool bAllMsg;
	
	if (g_bIsSteam) {
		if (msg_state == 0){
			index = *(short *)p;
			bAllMsg = false;
			LOG_DEBUG(UTIL_VarArgs("index = %d", index));
		}
		else if (msg_state == 1) {
			char *szFormat = (char *)p;
			LOG_DEBUG(UTIL_VarArgs("szFormat = %s", szFormat));

			// #Cstrike_
			if (szFormat && (szFormat[1] == 'C')) {
				// ... Chat_All[Dead,Spec]
				if (!strncmp(szFormat + 9, "Chat_All", 8)) {
					bAllMsg = true;
				}
				// ... Name_Change
				else if (!strncmp(szFormat + 9, "Name_Change", 11)) {
					return; // ignore message
				}
				// ... Chat_[CT,T,CT_Dead,T_Dead,Spec]
				else {
					return; // ignore messages
				}
			}
			// #Game_radio
			else if (szFormat && (szFormat[1] == 'G')) {
					return; // ignore message
			}
		}
		else if (msg_state == 2) {
			char *szName = (char *)p;
			LOG_DEBUG(UTIL_VarArgs("szName = %s", szName));
		}
		else if (msg_state == 3) {
			char *szText = (char *)p;
			LOG_DEBUG(UTIL_VarArgs("szText = %s", szText));

			char *szSayText;
			edict_t *pSayEdict = INDEXENT(index);

			if (pSayEdict && IsAlive(pSayEdict)) {
				szSayText = szLastSayText;
				iLastSayClient = index;
			}
			else {
				szSayText = szDeadLastSayText;
				iDeadLastSayClient = index;
			}

			if (bAllMsg) {
				UTIL_strlwr(szSayText);
			}
			else {
				return; // ignore team messages
			}
		}
	}
	else {
		if (msg_state == 0)
			index = *(short *)p;
		else if (msg_state == 1) {
			char *szText = (char *)p;

			char *szSayText;
			edict_t *pSayEdict = INDEXENT(index);

			if(pSayEdict && IsAlive(pSayEdict)) {
				szSayText = szLastSayText;
				iLastSayClient = index;
			}
			else {
				szSayText = szDeadLastSayText;
				iDeadLastSayClient = index;
			}

			if(strstr(szText,"(Counter-Terrorist)")		// just ignore this message if it's team specific
				||strstr(szText,"(Terrorist)")			// NOTE : This neednt be mod independant. This is just for cs/hldm
				||strstr(szText,"(TEAM)")){
				return;
			}

			strcpy(szSayText,szText);

			char *szDP = strchr(szSayText,':');

			if(szDP  < szSayText + sizeof(char) * 80 )
				memset(szSayText,' ',szDP - szSayText);

			// make it lowercase;
			UTIL_strlwr(szSayText);

			//cout << szSayText;
		}
	}
}

void BotClient_CS_SayText(void *p, int bot_index){
	// this is just like the Valve SayText message
	BotClient_Valve_SayText(p, bot_index);
}

void BotClient_Gearbox_SayText(void *p, int bot_index){
	// this is just like the Valve SayText message
	BotClient_Valve_SayText(p, bot_index);
}

void BotClient_DOD_SayText(void *p, int bot_index)
{
	// this is just like the Valve SayText message
	BotClient_Valve_SayText(p, bot_index);
}

// This message is sent when the bot picks up some ammo (AmmoX messages are
// also sent so this message is probably not really necessary except it
// allows the HUD to draw pictures of ammo that have been picked up.  The
// bots don't really need pictures since they don't have any eyes anyway.
void BotClient_Valve_AmmoPickup(void *p, int bot_index){
	static int index;
	static int ammount;
	int ammo_index;
	
	if (msg_state == 0)
		index = *(int *)p;
	else if (msg_state == 1){
		ammount = *(int *)p;
		
		bots[bot_index]->m_rgAmmo[index] = ammount;
		
		ammo_index = bots[bot_index]->current_weapon.iId;
		
		// update the ammo counts for this weapon...
		bots[bot_index]->current_weapon.iAmmo1 =
			bots[bot_index]->m_rgAmmo[weapon_defs[ammo_index].iAmmo1];
		bots[bot_index]->current_weapon.iAmmo2 =
			bots[bot_index]->m_rgAmmo[weapon_defs[ammo_index].iAmmo2];
	}
}

void BotClient_CS_AmmoPickup(void *p, int bot_index){
	// this is just like the Valve Ammo Pickup message
	BotClient_Valve_AmmoPickup(p, bot_index);
}

void BotClient_Gearbox_AmmoPickup(void *p, int bot_index){
	// this is just like the Valve Ammo Pickup message
	BotClient_Valve_AmmoPickup(p, bot_index);
}

void BotClient_DOD_AmmoPickup(void *p, int bot_index)
{
	// this is just like the Valve Ammo Pickup message
	BotClient_Valve_AmmoPickup(p, bot_index);
}

// This message gets sent when the bot picks up a weapon.
void BotClient_Valve_WeapPickup(void *p, int bot_index){
	int index;
	
	if (msg_state == 0){
		index = *(int *)p;
		
		// set this weapon bit to indicate that we are carrying this weapon
		bots[bot_index]->bot_weapons |= (1<<index);
	}
}

void BotClient_CS_WeapPickup(void *p, int bot_index){
	// this is just like the Valve Weapon Pickup message
	BotClient_Valve_WeapPickup(p, bot_index);
}

void BotClient_Gearbox_WeapPickup(void *p, int bot_index){
	// this is just like the Valve Weapon Pickup message
	BotClient_Valve_WeapPickup(p, bot_index);
}

void BotClient_DOD_WeapPickup(void *p, int bot_index)
{
	// this is just like the Valve Weapon Pickup message
	BotClient_Valve_WeapPickup(p, bot_index);
}

// This message gets sent when the bot picks up an item (like a battery
// or a healthkit)
void BotClient_Valve_ItemPickup(void *p, int bot_index){
	if (msg_state == 0){
	}
}

void BotClient_CS_ItemPickup(void *p, int bot_index){
	// this is just like the Valve Item Pickup message
	BotClient_Valve_ItemPickup(p, bot_index);
}

void BotClient_Gearbox_ItemPickup(void *p, int bot_index){
	// this is just like the Valve Item Pickup message
	BotClient_Valve_ItemPickup(p, bot_index);
}

void BotClient_DOD_ItemPickup(void *p, int bot_index)
{
	// this is just like the Valve Item Pickup message
	BotClient_Valve_ItemPickup(p, bot_index);
}

// This message gets sent when the bots health changes.
void BotClient_Valve_Health(void *p, int bot_index){
	if (msg_state == 0)
		bots[bot_index]->bot_health = *(int *)p;  // health ammount
}

void BotClient_CS_Health(void *p, int bot_index){
	// this is just like the Valve Health message
	BotClient_Valve_Health(p, bot_index);
}

void BotClient_Gearbox_Health(void *p, int bot_index){
	// this is just like the Valve Health message
	BotClient_Valve_Health(p, bot_index);
}

void BotClient_DOD_Health(void *p, int bot_index)
{
	// this is just like the Valve Health message
	BotClient_Valve_Health(p, bot_index);
}

// This message gets sent when the bots armor changes.
void BotClient_Valve_Battery(void *p, int bot_index){
	if (msg_state == 0)
		bots[bot_index]->bot_armor = *(int *)p;  // armor ammount
}

void BotClient_CS_Battery(void *p, int bot_index){
	// this is just like the Valve Battery message
	BotClient_Valve_Battery(p, bot_index);
}

void BotClient_Gearbox_Battery(void *p, int bot_index){
	// this is just like the Valve Battery message
	BotClient_Valve_Battery(p, bot_index);
}

void BotClient_DOD_Battery(void *p, int bot_index)
{
	// this is just like the Valve Battery message
	BotClient_Valve_Battery(p, bot_index);
}

// This message gets sent when the bots are getting damaged.
void BotClient_Valve_Damage(void *p, int bot_index){
	static int damage_armor;
	static int damage_taken;
	static int damage_bits;  // type of damage being done
	static Vector damage_origin;
	
	if (msg_state == 0)
		damage_armor = *(int *)p;
	else if (msg_state == 1)
		damage_taken = *(int *)p;
	else if (msg_state == 2)
		damage_bits = *(int *)p;
	else if (msg_state == 3)
		damage_origin.x = *(float *)p;
	else if (msg_state == 4)
		damage_origin.y = *(float *)p;
	else if (msg_state == 5){
		damage_origin.z = *(float *)p;
		
		if ((damage_armor > 0) || (damage_taken > 0))
		{
			// ignore certain types of damage...
			if (damage_bits & IGNORE_DAMAGE)
				return;
			
			// if the bot doesn't have an enemy and someone is shooting at it then
			// turn in the attacker's direction...
			if (bots[bot_index]->pBotEnemy == NULL)
			{
				bots[bot_index]->VSuspEn = damage_origin;
				bots[bot_index]->Action.lAction|=BA_SUSPLOC;
				/*// face the attacker...
				Vector v_enemy = damage_origin - bots[bot_index]->pEdict->v.origin;
				Vector bot_angles = UTIL_VecToAngles( v_enemy );
				
				  bots[bot_index]->pEdict->v.ideal_yaw = bot_angles.y;
				  
				BotFixIdealYaw(bots[bot_index]->pEdict);*/
			}
		}
	}
}

void BotClient_CS_Damage(void *p, int bot_index)
{
	// this is just like the Valve Battery message
	BotClient_Valve_Damage(p, bot_index);
}

void BotClient_Gearbox_Damage(void *p, int bot_index)
{
	// this is just like the Valve Battery message
	BotClient_Valve_Damage(p, bot_index);
}

void BotClient_DOD_Damage(void *p, int bot_index)
{
	// this is just like the Valve Battery message
	BotClient_Valve_Damage(p, bot_index);
}

// This message gets sent when the bots money ammount changes (for CS)
void BotClient_CS_Money(void *p, int bot_index)
{
	if (msg_state == 0)
		((CBotCS *)(bots[bot_index]))->m_iBotMoney = *(int *)p;  // amount of money
}

void BotClient_CS_StatusIcon(void *p, int bot_index)
{
	static bool btemp;
	
	if(msg_state == 0)
		btemp = *(bool*)p;
	else if (msg_state == 1){
		if(FStrEq((char *)p,"buyzone")){
			btemp?bots[bot_index]->Action.lAction|=BA_BUYZONE:bots[bot_index]->Action.lAction&=~BA_BUYZONE;
		}
		if(FStrEq((char *)p,"defuser")){
			btemp?bots[bot_index]->Action.lAction|=BA_DEFKIT:bots[bot_index]->Action.lAction&=~BA_DEFKIT;
		}
	}
}

void BotClient_CS_RoundTime(void *p, int bot_index)
{
	if (msg_state == 0){
		float fRoundTime = *(short *)p;  // roundtime in seconds

		CBotCS* pBot = (CBotCS*)bots[bot_index];
		pBot->f_round_time = fRoundTime;

		if (fRoundTime <= g_mp_freezetime->value) // we're in freezetime, init for new round
		{
			pBot->m_bIsVIP = false;
			pBot->bRSInit = true;
			AWP_ED[bot_index].iLastWP = -1;
			bRec[bot_index] = false;
			bStopRec[bot_index] = false;
		}
	}
}

void BotClient_CS_StatusValue(void *p, int bot_index)						// not yet impl
{
	if (msg_state == 0){
	}
	else if (msg_state == 1){
	}
}

void BotClient_CS_SetFOV(void *p, int bot_index)
{
	if (msg_state == 0){
		bots[bot_index]->i_FOV = *(int*)(p);
		//cout << bots[bot_index]->i_FOV << endl;
	}
}

void BotClient_CS_TextMsg(void *p, int bot_index){
	// assuming 10 is max sentences
	static char szTextMsg[10][32]={"","","","","","","","","",""};

	if (msg_state == 0)
		return;

	if ((msg_state - 1) < 10)
		strcpy(szTextMsg[msg_state-1],(char*)p);		// copy string

	LOG_DEBUG(UTIL_VarArgs("%d, %s",msg_state,szTextMsg[msg_state-1]));

	if (msg_state == 1){
		LOG_RADIO(UTIL_VarArgs("--------------------"));
		if(FStrEq(szTextMsg[msg_state-1],"#Switch_To_BurstFire"))
			((CBotCS*)(bots[bot_index]))->m_bGlockBurst = true;
		else if(FStrEq(szTextMsg[msg_state-1],"#Switch_To_SemiAuto"))
			((CBotCS*)(bots[bot_index]))->m_bGlockBurst = false;
		else if(FStrEq(szTextMsg[msg_state-1],"#Bomb_Planted")){
			g_bBombPlanted = true;
			g_fBombExplode = gpGlobals->time + CVAR_GET_FLOAT("mp_c4timer");
		}
		else if(FStrEq(szTextMsg[msg_state-1],"#Game_bomb_drop"))
			g_bBombDropped = true;
		else if(FStrEq(szTextMsg[msg_state-1],"#Got_bomb"))
			g_bBombDropped = false;
		else if(FStrEq(szTextMsg[msg_state-1],"#Terrorists_Win"))
			g_bBombPlanted = false;
		else if(FStrEq(szTextMsg[msg_state-1],"#Bomb_Defused"))
			g_bBombPlanted = false;
		else if(FStrEq(szTextMsg[msg_state-1],"#CTs_Win"))
			g_bBombPlanted = false;
		else if(FStrEq(szTextMsg[msg_state-1],"#Game_Commencing"))
			g_fGameCommenced = gpGlobals->time;
		else if(FStrEq(szTextMsg[msg_state-1],"#Cant_buy"))
			((CBotCS*)(bots[bot_index]))->m_flBuyTime = gpGlobals->time + 60.0;			// just to avoid too many buy trials
		else
			strcpy(szTextMsg[msg_state-1],(char*)p);		// copy string
	}
	else if (msg_state == 2){
		if(FStrEq(szTextMsg[msg_state-2], "#Game_teammate_attack")) {
			// get edict of radio using player
			edict_t *pEnt = 0;
			CBotCS *pBot = ((CBotCS*)(bots[bot_index]));
			edict_t *pEdict = pBot->pEdict;
			int i;
			
			pBot->bot_IRadioM.pECalling = 0;		// resetting calling edict
			pBot->bot_IRadioM.lMessage = 0;
			
			for (i = 0; i < gpGlobals->maxClients; i++){
				pEnt = INDEXENT(i + 1);
				
				// skip invalid players and skip self (i.e. this bot)
				if ((pEnt) && (!pEnt->free) && (pEnt != pEdict)) {
					if(!IsAlive(pEnt))
						continue;

					if(FStrEq(szTextMsg[msg_state-1],STRING(pEnt->v.netname))){// if found ent with that name
						pBot->bot_IRadioM.pECalling = pEnt;					// copy pointer to ent
						pBot->bot_IRadioM.f_Time = gpGlobals->time + RANDOM_FLOAT(1.0,2.5);		// this random stuff for faking kind of reaction time to radio commands
						pBot->bot_IRadioM.lMessage = RC_TEAMATTACK;
					}
				}
			}
		}
		else
			strcpy(szTextMsg[msg_state-1],(char*)p);		// copy string
	}
	else if (msg_state == 3 || msg_state == 4){
		if(FStrEq(szTextMsg[msg_state-3],"#Game_radio")){
			// get edict of radio using player
			edict_t *pEnt = 0;
			CBotCS *pBot = ((CBotCS*)(bots[bot_index]));
			edict_t *pEdict = pBot->pEdict;
			int i;
			
			f_LastRadio = gpGlobals->time;			// set last time a radio command was used
			
			pBot->bot_IRadioM.pECalling = 0;		// resetting calling edict
			pBot->bot_IRadioM.lMessage = 0;
			
			for (i = 0; i < gpGlobals->maxClients; i++){
				pEnt = INDEXENT(i + 1);
				
				// skip invalid players and skip self (i.e. this bot)
				if ((pEnt) && (!pEnt->free) && (pEnt != pEdict))
				{
					if(!IsAlive(pEnt))
						continue;
					if(FStrEq(szTextMsg[msg_state - 2],STRING(pEnt->v.netname))){// if found ent with that name
						pBot->bot_IRadioM.pECalling = pEnt;					// copy pointer to ent
						
						pBot->bot_IRadioM.f_Time = gpGlobals->time + RANDOM_FLOAT(.75,2.0);		// this random stuff for faking kind of reaction time to radio commands
						
						if(FStrEq(szTextMsg[msg_state-1],"#Cover_me"))
							pBot->bot_IRadioM.lMessage = RC_COVER_ME;
						else if(FStrEq(szTextMsg[msg_state-1],"#You_take_the_point"))
							pBot->bot_IRadioM.lMessage = RC_YOU_TAKE_THE_POINT;
						else if(FStrEq(szTextMsg[msg_state-1],"#Hold_this_position"))
							pBot->bot_IRadioM.lMessage = RC_HOLD_THIS_POSITION;
						else if(FStrEq(szTextMsg[msg_state-1],"#Regroup_team"))
							pBot->bot_IRadioM.lMessage = RC_REGROUP_TEAM;
						else if(FStrEq(szTextMsg[msg_state-1],"#Follow_me"))
							pBot->bot_IRadioM.lMessage = RC_FOLLOW_ME;
						else if(FStrEq(szTextMsg[msg_state-1],"#Taking_fire"))
							pBot->bot_IRadioM.lMessage = RC_TAKING_FIRE;
						else if(FStrEq(szTextMsg[msg_state-1],"#Go_go_go"))
							pBot->bot_IRadioM.lMessage = RC_GO_GO_GO;
						else if(FStrEq(szTextMsg[msg_state-1],"#Team_fall_back"))
							pBot->bot_IRadioM.lMessage = RC_TEAM_FALL_BACK;
						else if(FStrEq(szTextMsg[msg_state-1],"#Stick_together_team"))
							pBot->bot_IRadioM.lMessage = RC_STICK_TOGETHER_TEAM;
						else if(FStrEq(szTextMsg[msg_state-1],"#Get_in_position_and_wait"))
							pBot->bot_IRadioM.lMessage = RC_GET_IN_POSITION_AND_WAIT;
						else if(FStrEq(szTextMsg[msg_state-1],"#Storm_the_front"))
							pBot->bot_IRadioM.lMessage = RC_STORM_THE_FRONT;
						else if(FStrEq(szTextMsg[msg_state-1],"#Report_in_team"))
							pBot->bot_IRadioM.lMessage = RC_REPORT_IN_TEAM;
						else if(FStrEq(szTextMsg[msg_state-1],"#Affirmative")||FStrEq(szTextMsg[msg_state-1],"#Roger_that"))
							pBot->bot_IRadioM.lMessage = RC_AFFIRMATIVE;
						else if(FStrEq(szTextMsg[msg_state-1],"#Enemy_spotted"))
							pBot->bot_IRadioM.lMessage = RC_ENEMY_SPOTTED;
						else if(FStrEq(szTextMsg[msg_state-1],"#Need_backup"))
							pBot->bot_IRadioM.lMessage = RC_NEED_BACKUP;
						else if(FStrEq(szTextMsg[msg_state-1],"#Sector_clear"))
							pBot->bot_IRadioM.lMessage = RC_SECTOR_CLEAR;
						else if(FStrEq(szTextMsg[msg_state-1],"#In_position"))
							pBot->bot_IRadioM.lMessage = RC_IN_POSITION;
						else if(FStrEq(szTextMsg[msg_state-1],"#Reporting_in"))
							pBot->bot_IRadioM.lMessage = RC_REPORTING_IN;
						else if(FStrEq(szTextMsg[msg_state-1],"#Get_out_of_there"))
							pBot->bot_IRadioM.lMessage = RC_GET_OUT_OF_THERE;
						else if(FStrEq(szTextMsg[msg_state-1],"#Negative"))
							pBot->bot_IRadioM.lMessage = RC_NEGATIVE;
						else if(FStrEq(szTextMsg[msg_state-1],"#Enemy_down"))
							pBot->bot_IRadioM.lMessage = RC_ENEMY_DOWN;
						else if(FStrEq(szTextMsg[msg_state-1],"#Fire_in_the_hole")){
							// do nothing, this isn't important since it's handled by other funcs
						}
						else if(FStrEq(szTextMsg[msg_state-1],"#Hostage_down"))
							pBot->bot_IRadioM.lMessage = RC_HOSTAGE_DOWN;
						else{
							pBot->bot_IRadioM.lMessage = 0;
							LOG_RADIO(UTIL_VarArgs("Unknown: %s",szTextMsg[msg_state-1]));
						}
						break;
					}
				}
			}
		}
	}
}

void BotClient_Valve_ScreenFade(void *p, int bot_index)
{
	static int duration;
	static int hold_time;
	static int fade_flags;
	int length;
	
	if (msg_state == 0)
		duration = *(int *)p;
	else if (msg_state == 1)
		hold_time = *(int *)p;
	else if (msg_state == 2)
		fade_flags = *(int *)p;
	else if (msg_state == 6) {
		length = (duration + hold_time) / 4096;
		bots[bot_index]->f_blinded_time = gpGlobals->time + length*2/3 - 1;
	}
}

void BotClient_CS_ScreenFade(void *p, int bot_index)
{
	// this is just like the Valve ScreenFade message
	BotClient_Valve_ScreenFade(p, bot_index);
}

void BotClient_DOD_ScreenFade(void *p, int bot_index)
{
	// this is just like the Valve ScreenFade message
	BotClient_Valve_ScreenFade(p, bot_index);
}

// This message gets sent when the bots get killed
void BotClient_Valve_DeathMsg(void *p, int bot_index)
{
	static int killer_index;
	static int victim_index;
	static int iHeadShot;
	static edict_t *victim_edict=0,
		*killer_edict=0;
	static int killer_bot_index,victim_bot_index;
	
	if (msg_state == 0)
		killer_index = *(int *)p;  // ENTINDEX() of killer
	else if (msg_state == 1)
		victim_index = *(int *)p;  // ENTINDEX() of victim
	else if (msg_state == 2)
		iHeadShot = *(int*)p;		// true when headshot
	else if(msg_state == 3){		// (char*)p would be name of weapon
		//cout << (char*)p << endl;
		char szWeaponclass[32];
		long lKWeapon;
		strcpy(szWeaponclass,"weapon_");
		strcat(szWeaponclass,(char*)p);
		lKWeapon = CBotBase::WeaponClass2ID(szWeaponclass);

		if(victim_index >= 0 && victim_index < 32)victim_edict = INDEXENT(victim_index);
		else victim_edict = 0;
		if(victim_index >= 0 && victim_index < 32)killer_edict = INDEXENT(killer_index);
		else killer_edict = 0;

		victim_bot_index = UTIL_GetBotIndex(victim_edict);
		killer_bot_index = UTIL_GetBotIndex(killer_edict);
		
		//index = UTIL_GetBotIndex(victim_edict);

		// is this message about a bot being killed?
		
		if ((killer_index == 0) || (killer_index == victim_index))
		{
			// bot killed by world (worldspawn) or bot killed self...
			if (victim_bot_index != -1){
				bots[victim_bot_index]->killer_edict = NULL;
			}
		}
		else
		{
			// notice bot that has been killed
			if (victim_bot_index != -1){
				bots[victim_bot_index]->KilledBy(killer_edict,lKWeapon);
			}
			// store information to killer
			
			//index = killer_bot_index;
			
			if(killer_bot_index != -1
				&&!FNullEnt(victim_index)){
				bots[killer_bot_index]->KilledSO(victim_edict,lKWeapon);
				//bots[killer_bot_index]->pEKilled = victim_edict;
			}
		}
		
		if(killer_edict != victim_edict){
			int i;
			for (i=0; i < 32; i++){
				if (bots[i]){
					bots[i]->pLastGlobalKill = killer_edict;
					bots[i]->pLastGlobalKilled = victim_edict;
				}
			}
			
			// do some stuff for statistics
			WPStat.AddKill(killer_edict,victim_edict);
		}
	}
}

void BotClient_CS_DeathMsg(void *p, int bot_index)
{
	// this is just like the Valve DeathMsg message
	BotClient_Valve_DeathMsg(p, bot_index);
}

void BotClient_DOD_DeathMsg(void *p, int bot_index)
{
	//cout << "nak" << endl;
	static int killer_index;
	static int victim_index;
	static edict_t *victim_edict,*killer_edict;
	static int killer_bot_index,victim_bot_index,killer_team,victim_team;
	
	if (msg_state == 0)
		killer_index = *(int *)p;  // ENTINDEX() of killer
	else if (msg_state == 1)
		victim_index = *(int *)p;  // ENTINDEX() of victim
	else if(msg_state == 2){		// (char*)p would be name of weapon
		//cout << (char*)p << endl;
		char szWeaponclass[32];
		long lKWeapon;
		strcpy(szWeaponclass,"weapon_");
		strcat(szWeaponclass,(char*)p);
		lKWeapon = CBotBase::WeaponClass2ID(szWeaponclass);

		victim_edict = INDEXENT(victim_index);
		killer_edict = INDEXENT(killer_index);
		victim_bot_index = UTIL_GetBotIndex(victim_edict);
		killer_bot_index = UTIL_GetBotIndex(killer_edict);
		
		//index = UTIL_GetBotIndex(victim_edict);
		
		// is this message about a bot being killed?
		
		if ((killer_index == 0) || (killer_index == victim_index))
		{
			// bot killed by world (worldspawn) or bot killed self...
			if (victim_bot_index != -1){
				bots[victim_bot_index]->killer_edict = NULL;
			}
		}
		else
		{
			// store edict of player that killed this bot...
			if (victim_bot_index != -1){
				bots[victim_bot_index]->killer_edict = killer_edict;

				// store information with which weapon the bot has been killed
				bots[victim_bot_index]->FLKW[bots[victim_bot_index]->iCLKW] = lKWeapon;
				bots[victim_bot_index]->iCLKW ++;
				if(bots[victim_bot_index]->iCLKW >= CBotBase::_MAXLKW){
					bots[victim_bot_index]->iCLKW = 0;
				}
			}
			// store information to killer
			
			//index = killer_bot_index;
			
			if(killer_bot_index != -1){
				bots[killer_bot_index]->pEKilled = victim_edict;
			}
		}
		
		if(killer_edict != victim_edict){
			if(victim_bot_index != -1 && killer_bot_index != -1){
				// update LTM of bot
				if(victim_edict&&killer_edict){
					victim_team = UTIL_GetTeam(victim_edict); 
					killer_team = UTIL_GetTeam(killer_edict);
					if(killer_team != victim_team){		// no tk
						if(victim_bot_index != -1)
							bots[victim_bot_index]->LTMem.Add(LTM_KILLED,victim_edict,killer_edict->v.origin,victim_edict->v.origin);
						
						if(killer_bot_index != -1)
							bots[killer_bot_index]->LTMem.Add(LTM_KILL,killer_edict,victim_edict->v.origin,killer_edict->v.origin);
					}
					else{
						if(victim_bot_index!=-1){
							Vector VTemp = Vector(0,0,0);
							bots[victim_bot_index]->LTMem.Add(LTM_TKILL,killer_edict,killer_edict->v.origin,VTemp);
						}
					}
				}
			}
			
			int i;
			for (i=0; i < 32; i++){
				if (bots[i]){
					bots[i]->pLastGlobalKill = killer_edict;
				}
			}
			
			// do some stuff for statistics
			//WPStat.AddKill(killer_edict,victim_edict);
		}
	}
}

void BotClient_DOD_Stamina(void *p, int bot_index)
{
	if (msg_state == 0){
		((CBotDOD * )(bots[bot_index]))->iStamina = *(int *)p;
	}
}

void BotClient_DOD_Speed(void *p, int bot_index)
{
	if (msg_state == 0){
		//bots[bot_index]->f_max_speed = *(int *)p;
	}
}

void BotClient_DOD_Slowed(void *p, int bot_index)
{
	if (msg_state == 0){
		((CBotDOD * )(bots[bot_index]))->bSlowed = *(bool *)p;
	}
}

void BotClient_DOD_Bleeding(void *p, int bot_index)
{
	if (msg_state == 0){
		((CBotDOD * )(bots[bot_index]))->bBleeding = *(bool *)p;
	}
}

void BotClient_DOD_Object(void *p, int bot_index)
{
	/*static char szString[80];

	if (msg_state == 0){
		strcpy(szString,(char *)p);
		if(FStrEq(szString,"sprites/obj_tnt.spr")){
			((CBotDOD * )(bots[bot_index]))->bTNT = true;
		}
	}*/
}

void BotClient_CS_Flashlight(void *p, int bot_index)
{
	static bool bFlashlight;
	static int iStrength;

	if (msg_state == 0)
		bFlashlight = *(bool*)p;
	if (msg_state == 1){
		iStrength = *(int*)p;
		
		bots[bot_index]->bFlashlight = bFlashlight;
	}
}

void RegBotMsgs(void)
{
	static bool bInitDone = FALSE;

	if (bInitDone) return;

	// initialize bot messages array
	for (int i = 0; i < MAX_REG_MSGS; i++)
		botmsgs[i] = NULL;

	switch(mod_id)
	{
		case VALVE_DLL:
			botmsgs[GET_USER_MSG_ID(PLID, "WeaponList", NULL)] = BotClient_Valve_WeaponList;
			botmsgs[GET_USER_MSG_ID(PLID, "CurWeapon", NULL)] = BotClient_Valve_CurWeapon;
			botmsgs[GET_USER_MSG_ID(PLID, "AmmoX", NULL)] = BotClient_Valve_AmmoX;
			botmsgs[GET_USER_MSG_ID(PLID, "AmmoPickup", NULL)] = BotClient_Valve_AmmoPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "WeapPickup", NULL)] = BotClient_Valve_WeapPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "ItemPickup", NULL)] = BotClient_Valve_ItemPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "Health", NULL)] = BotClient_Valve_Health;
			botmsgs[GET_USER_MSG_ID(PLID, "Battery", NULL)] = BotClient_Valve_Battery;
			botmsgs[GET_USER_MSG_ID(PLID, "Damage", NULL)] = BotClient_Valve_Damage;
			break;

		/*case TFC_DLL:
			botmsgs[GET_USER_MSG_ID(PLID, "WeaponList", NULL)] = BotClient_TFC_WeaponList;
			botmsgs[GET_USER_MSG_ID(PLID, "VGUIMenu", NULL)] = BotClient_TFC_VGUIMenu;
			botmsgs[GET_USER_MSG_ID(PLID, "CurWeapon", NULL)] = BotClient_TFC_CurWeapon;
			botmsgs[GET_USER_MSG_ID(PLID, "AmmoX", NULL)] = BotClient_TFC_AmmoX;
			botmsgs[GET_USER_MSG_ID(PLID, "AmmoPickup", NULL)] = BotClient_TFC_AmmoPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "WeapPickup", NULL)] = BotClient_TFC_WeapPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "ItemPickup", NULL)] = BotClient_TFC_ItemPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "Health", NULL)] = BotClient_TFC_Health;
			botmsgs[GET_USER_MSG_ID(PLID, "Battery", NULL)] = BotClient_TFC_Battery;
			botmsgs[GET_USER_MSG_ID(PLID, "Damage", NULL)] = BotClient_TFC_Damage;
			break;*/

		case CSTRIKE_DLL:
			botmsgs[GET_USER_MSG_ID(PLID, "WeaponList", NULL)] = BotClient_CS_WeaponList;
			botmsgs[GET_USER_MSG_ID(PLID, "VGUIMenu", NULL)] = BotClient_CS_VGUIMenu;
			botmsgs[GET_USER_MSG_ID(PLID, "CurWeapon", NULL)] = BotClient_CS_CurWeapon;
			botmsgs[GET_USER_MSG_ID(PLID, "AmmoX", NULL)] = BotClient_CS_AmmoX;
			botmsgs[GET_USER_MSG_ID(PLID, "AmmoPickup", NULL)] = BotClient_CS_AmmoPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "WeapPickup", NULL)] = BotClient_CS_WeapPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "ItemPickup", NULL)] = BotClient_CS_ItemPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "Health", NULL)] = BotClient_CS_Health;
			botmsgs[GET_USER_MSG_ID(PLID, "Battery", NULL)] = BotClient_CS_Battery;
			botmsgs[GET_USER_MSG_ID(PLID, "Damage", NULL)] = BotClient_CS_Damage;
			botmsgs[GET_USER_MSG_ID(PLID, "ShowMenu", NULL)] = BotClient_CS_ShowMenu;
			botmsgs[GET_USER_MSG_ID(PLID, "Money", NULL)] = BotClient_CS_Money;
			botmsgs[GET_USER_MSG_ID(PLID, "StatusIcon", NULL)] = BotClient_CS_StatusIcon;
			botmsgs[GET_USER_MSG_ID(PLID, "RoundTime", NULL)] = BotClient_CS_RoundTime;
			botmsgs[GET_USER_MSG_ID(PLID, "SetFOV", NULL)] = BotClient_CS_SetFOV;
			botmsgs[GET_USER_MSG_ID(PLID, "ScreenFade", NULL)] = BotClient_CS_ScreenFade;
			botmsgs[GET_USER_MSG_ID(PLID, "TextMsg", NULL)] = BotClient_CS_TextMsg;
			botmsgs[GET_USER_MSG_ID(PLID, "Flashlight", NULL)] = BotClient_CS_Flashlight;
			botmsgs[GET_USER_MSG_ID(PLID, "SayText", NULL)] = BotClient_CS_SayText;
			/*botmsgs[GET_USER_MSG_ID(PLID, "StatusValue", NULL)] = BotClient_CS_StatusValue;*/
			break;

		case DOD_DLL:
			botmsgs[GET_USER_MSG_ID(PLID, "WeaponList", NULL)] = BotClient_DOD_WeaponList;
			botmsgs[GET_USER_MSG_ID(PLID, "VGUIMenu", NULL)] = BotClient_DOD_VGUIMenu;
			botmsgs[GET_USER_MSG_ID(PLID, "CurWeapon", NULL)] = BotClient_DOD_CurWeapon;
			botmsgs[GET_USER_MSG_ID(PLID, "AmmoX", NULL)] = BotClient_DOD_AmmoX;
			botmsgs[GET_USER_MSG_ID(PLID, "AmmoPickup", NULL)] = BotClient_DOD_AmmoPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "WeapPickup", NULL)] = BotClient_DOD_WeapPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "ItemPickup", NULL)] = BotClient_DOD_ItemPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "Health", NULL)] = BotClient_DOD_Health;
			botmsgs[GET_USER_MSG_ID(PLID, "Battery", NULL)] = BotClient_DOD_Battery;
			botmsgs[GET_USER_MSG_ID(PLID, "Damage", NULL)] = BotClient_DOD_Damage;
			botmsgs[GET_USER_MSG_ID(PLID, "ScreenFade", NULL)] = BotClient_DOD_ScreenFade;
			botmsgs[GET_USER_MSG_ID(PLID, "Stamina", NULL)] = BotClient_DOD_Stamina;
			botmsgs[GET_USER_MSG_ID(PLID, "Slowed", NULL)] = BotClient_DOD_Slowed;
			botmsgs[GET_USER_MSG_ID(PLID, "Speed", NULL)] = BotClient_DOD_Speed;
			botmsgs[GET_USER_MSG_ID(PLID, "Bleeding", NULL)] = BotClient_DOD_Bleeding;
			botmsgs[GET_USER_MSG_ID(PLID, "Object", NULL)] = BotClient_DOD_Object;
			break;

		/*case GEARBOX_DLL:
			botmsgs[GET_USER_MSG_ID(PLID, "WeaponList", NULL)] = BotClient_Gearbox_WeaponList;
			botmsgs[GET_USER_MSG_ID(PLID, "CurWeapon", NULL)] = BotClient_Gearbox_CurWeapon;
			botmsgs[GET_USER_MSG_ID(PLID, "AmmoX", NULL)] = BotClient_Gearbox_AmmoX;
			botmsgs[GET_USER_MSG_ID(PLID, "AmmoPickup", NULL)] = BotClient_Gearbox_AmmoPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "WeapPickup", NULL)] = BotClient_Gearbox_WeapPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "ItemPickup", NULL)] = BotClient_Gearbox_ItemPickup;
			botmsgs[GET_USER_MSG_ID(PLID, "Health", NULL)] = BotClient_Gearbox_Health;
			botmsgs[GET_USER_MSG_ID(PLID, "Battery", NULL)] = BotClient_Gearbox_Battery;
			botmsgs[GET_USER_MSG_ID(PLID, "Damage", NULL)] = BotClient_Gearbox_Damage;
			break;*/
	}

	bInitDone = TRUE;
}

#ifndef USE_METAMOD
void AddUserMsg(const char *msgname, int msgid, int size)
{
	// Adapted from Pierre-Marie Baty's RACC 

	int i;

	// is this message NOT already registered ?
	for (i = 0; i < usermsgs_count; i++)
		if (FStrEq(usermsgs[i].name, msgname))
			break; // cycle through usermsgs types array and break when a similar record is found

	// now keep track (or update if necessary) this user message in our own array...
	usermsgs[i].name = msgname; // record this message's name
	usermsgs[i].id = msgid; // record this message's index
	usermsgs[i].size = size; // record this message's size

	// are we certain this message has not been registered in the past ?
	if (i == usermsgs_count)
		usermsgs_count++; // we know now one user message more
}

int GetUserMsgId(const char *msgname, int *size)
{
	// Adapted from Pierre-Marie Baty's RACC 

	// this function returns the user message id of the recorded message named msgname. Local
	// variables have been made static to speedup recurrent calls of this function.

	static int i;

	// is it a standard engine message (i.e, NOT a user message, already registered by engine) ?
	if (FStrEq("TempEntity", msgname)) {
		if (size) *size = strlen("TempEntity");
		return (SVC_TEMPENTITY); // return the correct message ID
	}
	else if (FStrEq("Intermission", msgname)) {
		if (size) *size = strlen("Intermission");
		return (SVC_INTERMISSION); // return the correct message ID
	}
	else if (FStrEq("CDTrack", msgname)) {
		if (size) *size = strlen("CDTrack");
		return (SVC_CDTRACK); // return the correct message ID
	}
	else if (FStrEq("WeaponAnim", msgname)) {
		if (size) *size = strlen("WeaponAnim");
		return (SVC_WEAPONANIM); // return the correct message ID
	}
	else if (FStrEq("RoomType", msgname)) {
		if (size) *size = strlen("RoomType");
		return (SVC_ROOMTYPE); // return the correct message ID
	}
	else if (FStrEq("Director", msgname)) {
		if (size) *size = strlen("Director");
		return (SVC_DIRECTOR); // return the correct message ID
	}

	// cycle through our known user message types array
	for (i = 0; i < usermsgs_count; i++)
		if (FStrEq(usermsgs[i].name, msgname)) {
			if (size) *size = usermsgs[i].size;
			return (usermsgs[i].id); // return the id of the user message named msgname
		}

	// unregistered user message, have the engine register it
	return (REG_USER_MSG (msgname, -1)); // ask the engine to register this new message
}

const char *GetUserMsgName(int msgid, int *size)
{
	// Adapted from Pierre-Marie Baty's RACC 

	// this function returns the user message name of the recorded message index msgid. Local
	// variables have been made static to speedup recurrent calls of this function.

	static int i;

	// is it a standard engine message (i.e, NOT a user message, already registered by engine) ?
	switch(msgid) {
		case SVC_TEMPENTITY:
			if (size) *size = -1;
			return ("TempEntity"); // return the correct message name
		case SVC_INTERMISSION:
			if (size) *size = -1;
			return ("Intermission"); // return the correct message name
		case SVC_CDTRACK:
			if (size) *size = -1;
			return ("CDTrack"); // return the correct message name
		case SVC_WEAPONANIM:
			if (size) *size = -1;
			return ("WeaponAnim"); // return the correct message name
		case SVC_ROOMTYPE:
			if (size) *size = -1;
			return ("RoomType"); // return the correct message name
		case SVC_DIRECTOR:
			if (size) *size = -1;
			return ("Director"); // return the correct message name
		default:
			// cycle through our known user message types array
			for (i = 0; i < usermsgs_count; i++)
				if (usermsgs[i].id == msgid) {
					if (size) *size = usermsgs[i].size;
					return (usermsgs[i].name); // return the name of the user message having the msgid id
				}
	}

	return (NULL); // unregistered user message
}
#endif /* USE_METAMOD */
