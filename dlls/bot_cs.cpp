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
// bot.cpp
//

#include <sys/types.h>
#include <sys/stat.h>

#include "extdll.h"
#include "util.h"

#include "bot.h"
#include "CBotCS.h"
#include "Commandfunc.h"

void BotBuy_CS_WEAPON_P228(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 1;menuselect 4");
	else
		FakeClientCommand(pBot->pEdict,"buy;menuselect 1;menuselect 3");
}

void BotBuy_CS_WEAPON_SHIELD(CBotBase *pBot){
	if (!CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buyequip;menuselect 8");
}

void BotBuy_CS_WEAPON_SCOUT(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 5");
	else
	{
		switch (pBot->bot_teamnm)
		{
			case CS_TEAM_TE:
				FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 3");
				break;
			case CS_TEAM_CT:
				FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 2");
				break;
		}
	}
}

void BotBuy_CS_WEAPON_HEGRENADE(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buyequip;menuselect 4");
}

void BotBuy_CS_WEAPON_XM1014(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buy;menuselect 2;menuselect 2");
}

void BotBuy_CS_WEAPON_MAC10(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 3;menuselect 4");
	else
		FakeClientCommand(pBot->pEdict,"buy;menuselect 3;menuselect 1");
}

void BotBuy_CS_WEAPON_AUG(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 4");
}

void BotBuy_CS_WEAPON_SMOKEGRENADE(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buyequip;menuselect 5");
}

void BotBuy_CS_WEAPON_ELITE(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buy;menuselect 1;menuselect 5");
}

void BotBuy_CS_WEAPON_FIVESEVEN(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 1;menuselect 6");
	else
		FakeClientCommand(pBot->pEdict,"buy;menuselect 1;menuselect 5");
}

void BotBuy_CS_WEAPON_UMP45(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 3;menuselect 5");
	else
		FakeClientCommand(pBot->pEdict,"buy;menuselect 3;menuselect 3");
}

void BotBuy_CS_WEAPON_SG550(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 8");
	else
		FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 5");
}

void BotBuy_CS_WEAPON_GALIL(CBotBase *pBot){
	if (!CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 1");
}

void BotBuy_CS_WEAPON_FAMAS(CBotBase *pBot){
	if (!CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 1");
}

void BotBuy_CS_WEAPON_USP(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buy;menuselect 1;menuselect 2");
}

void BotBuy_CS_WEAPON_GLOCK18(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buy;menuselect 1;menuselect 1");
}

void BotBuy_CS_WEAPON_AWP(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 6");
	else
	{
		switch (pBot->bot_teamnm)
		{
			case CS_TEAM_TE:
				FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 5");
				break;
			case CS_TEAM_CT:
				FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 6");
				break;
		}
	}
}

void BotBuy_CS_WEAPON_MP5NAVY(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 3;menuselect 1");
	else
		FakeClientCommand(pBot->pEdict,"buy;menuselect 3;menuselect 2");
}

void BotBuy_CS_WEAPON_M249(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buy;menuselect 5;menuselect 1");
}

void BotBuy_CS_WEAPON_M3(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buy;menuselect 2;menuselect 1");
}

void BotBuy_CS_WEAPON_M4A1(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 3");
}

void BotBuy_CS_WEAPON_TMP(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 3;menuselect 2");
	else
		FakeClientCommand(pBot->pEdict,"buy;menuselect 3;menuselect 1");
}

void BotBuy_CS_WEAPON_G3SG1(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 7");
	else
		FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 6");
}

void BotBuy_CS_WEAPON_FLASHBANG(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buyequip;menuselect 3");
}

void BotBuy_CS_WEAPON_DEAGLE(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 1;menuselect 3");
	else
		FakeClientCommand(pBot->pEdict,"buy;menuselect 1;menuselect 4");
}

void BotBuy_CS_WEAPON_SG552(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 2");
	else
		FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 4");
}

void BotBuy_CS_WEAPON_AK47(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 1");
	else
		FakeClientCommand(pBot->pEdict,"buy;menuselect 4;menuselect 2");
}

void BotBuy_CS_WEAPON_P90(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buy;menuselect 3;menuselect 3");
	else
		FakeClientCommand(pBot->pEdict,"buy;menuselect 3;menuselect 4");
}

void BotBuy_PAmmo(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buy;menuselect 6");
}

void BotBuy_SAmmo(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buy;menuselect 7");
}

void BotBuy_Kevlar(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buyequip;menuselect 1");
}

void BotBuy_KevlarHelmet(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buyequip;menuselect 2");
}

void BotBuy_Defuse(CBotBase *pBot){
	FakeClientCommand(pBot->pEdict,"buyequip;menuselect 6");
}

void BotBuy_NightVision(CBotBase *pBot){
	if (CVAR_BOOL(jb_cstrike15))
		FakeClientCommand(pBot->pEdict,"buyequip;menuselect 7");
	else
	{
		switch (pBot->bot_teamnm)
		{
			case CS_TEAM_TE:
				FakeClientCommand(pBot->pEdict,"buyequip;menuselect 6");
				break;
			case CS_TEAM_CT:
				FakeClientCommand(pBot->pEdict,"buyequip;menuselect 7");
				break;
		}
	}
}
