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
//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// bot_client.h
//

void BotClient_Valve_WeaponList(void *p, int bot_index);
void BotClient_Valve_CurWeapon(void *p, int bot_index);
void BotClient_Valve_AmmoX(void *p, int bot_index);
void BotClient_Valve_SayText(void *p, int bot_index);
void BotClient_Valve_AmmoPickup(void *p, int bot_index);
void BotClient_Valve_WeapPickup(void *p, int bot_index);
void BotClient_Valve_ItemPickup(void *p, int bot_index);
void BotClient_Valve_Health(void *p, int bot_index);
void BotClient_Valve_Battery(void *p, int bot_index);
void BotClient_Valve_Damage(void *p, int bot_index);
void BotClient_Valve_ScreenFade(void *p, int bot_index);
void BotClient_Valve_DeathMsg(void *p, int bot_index);


/*void BotClient_TFC_VGUIMenu(void *p, int bot_index);
void BotClient_TFC_WeaponList(void *p, int bot_index);
void BotClient_TFC_CurWeapon(void *p, int bot_index);
void BotClient_TFC_AmmoX(void *p, int bot_index);
void BotClient_TFC_SayText(void *p, int bot_index);
void BotClient_TFC_AmmoPickup(void *p, int bot_index);
void BotClient_TFC_WeapPickup(void *p, int bot_index);
void BotClient_TFC_ItemPickup(void *p, int bot_index);
void BotClient_TFC_Health(void *p, int bot_index);
void BotClient_TFC_Battery(void *p, int bot_index);
void BotClient_TFC_Damage(void *p, int bot_index);
void BotClient_TFC_ScreenFade(void *p, int bot_index);
void BotClient_TFC_DeathMsg(void *p, int bot_index);*/


void BotClient_CS_VGUIMenu(void *p, int bot_index);
void BotClient_CS_WeaponList(void *p, int bot_index);
void BotClient_CS_CurWeapon(void *p, int bot_index);
void BotClient_CS_AmmoX(void *p, int bot_index);
void BotClient_CS_SayText(void *p, int bot_index);
void BotClient_CS_AmmoPickup(void *p, int bot_index);
void BotClient_CS_WeapPickup(void *p, int bot_index);
void BotClient_CS_ItemPickup(void *p, int bot_index);
void BotClient_CS_Health(void *p, int bot_index);
void BotClient_CS_Battery(void *p, int bot_index);
void BotClient_CS_Damage(void *p, int bot_index);
void BotClient_CS_ScreenFade(void *p, int bot_index);
void BotClient_CS_DeathMsg(void *p, int bot_index);
// CS specific
void BotClient_CS_Money(void *p, int bot_index);
void BotClient_CS_ShowMenu(void *p, int bot_index);
void BotClient_CS_StatusIcon(void *p, int bot_index);
void BotClient_CS_RoundTime(void *p, int bot_index);
void BotClient_CS_TextMsg(void *p, int bot_index);
void BotClient_CS_StatusValue(void *p, int bot_index);
void BotClient_CS_SetFOV(void *p, int bot_index);
void BotClient_CS_Flashlight(void *p, int bot_index);


/*void BotClient_Gearbox_WeaponList(void *p, int bot_index);
void BotClient_Gearbox_CurWeapon(void *p, int bot_index);
void BotClient_Gearbox_AmmoX(void *p, int bot_index);
void BotClient_Gearbox_SayText(void *p, int bot_index);
void BotClient_Gearbox_AmmoPickup(void *p, int bot_index);
void BotClient_Gearbox_WeapPickup(void *p, int bot_index);
void BotClient_Gearbox_ItemPickup(void *p, int bot_index);
void BotClient_Gearbox_Health(void *p, int bot_index);
void BotClient_Gearbox_Battery(void *p, int bot_index);
void BotClient_Gearbox_Damage(void *p, int bot_index);*/


void BotClient_DOD_VGUIMenu(void *p, int bot_index);
void BotClient_DOD_WeaponList(void *p, int bot_index);
void BotClient_DOD_CurWeapon(void *p, int bot_index);
void BotClient_DOD_AmmoX(void *p, int bot_index);
void BotClient_DOD_AmmoPickup(void *p, int bot_index);
void BotClient_DOD_WeapPickup(void *p, int bot_index);
void BotClient_DOD_ItemPickup(void *p, int bot_index);
void BotClient_DOD_Health(void *p, int bot_index);
void BotClient_DOD_Battery(void *p, int bot_index);
void BotClient_DOD_Damage(void *p, int bot_index);
void BotClient_DOD_DeathMsg(void *p, int bot_index);
void BotClient_DOD_ScreenFade(void *p, int bot_index);
// DOD specific
void BotClient_DOD_Stamina(void *p, int bot_index);
void BotClient_DOD_Speed(void *p, int bot_index);
void BotClient_DOD_Slowed(void *p, int bot_index);
void BotClient_DOD_Bleeding(void *p, int bot_index);
void BotClient_DOD_Object(void *p, int bot_index);


#ifndef USE_METAMOD
// maximum number of user messages the engine can register (hardcoded in the engine)
#define MAX_REG_MSGS 256

// user message record structure definition
typedef struct
{
	const char *name; // name of user message as called by the MOD DLL
	int id; // identification number the engine recorded this user message under
	int size; // size of this message in packets
} usermsg_t;

extern usermsg_t usermsgs[MAX_REG_MSGS];
extern int usermsgs_count;

void AddUserMsg(const char *msgname, int msgid, int size);
int GetUserMsgId(const char *msgname, int *size);
const char *GetUserMsgName(int msgid, int *size);

#define GET_USER_MSG_ID(plid, msgname, size) GetUserMsgId(msgname, size)
#define GET_USER_MSG_NAME(plid, msgid, size) GetUserMsgName(msgid, size)
#endif /* USE_METAMOD */

typedef void (*botmsg_t)(void *, int);
extern botmsg_t botmsgs[MAX_REG_MSGS];
void RegBotMsgs(void);
extern int msg_state;
