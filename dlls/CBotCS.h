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
#ifndef __CBOTCS_H
#define __CBOTCS_H

#include "CBotBase.h"

#define _BUY_HE 50
#define _BUY_FL 59
#define _BUY_SG 59

class CBotCS : public CBotBase{
public:
	bool DistanceSight(void);

	CBotCS();
	~CBotCS();

	virtual void Think(void);
	virtual void Think5th(void);			// think 5th
	virtual void Think1(void);				// think every 1s
	virtual void Fight(void);

	virtual void HandleMenu(void);
	virtual bool HandleOrders(void);
	virtual void SpawnInit(void);
	virtual void Init(void);
	virtual bool BuyWeapon(void);

	virtual void PreprocessTasks(void);

	virtual bool ReactOnSound(void);
	virtual void ReactOnRadio(void);
	virtual bool CheckGrenadeThrowing(void);

	virtual long IsPrimaryWeapon(long lbit);
	virtual long IsSecondaryWeapon(long lbit);
	virtual long IsShotgunWeapon(long lbit);
	virtual long IsSubMWeapon(long lbit);
	virtual long IsRifleWeapon(long lbit);
	virtual long IsSniperWeapon(long lbit);
	virtual long IsKnifeWeapon(long lbit);
	virtual long IsMachineGunWeapon(long lbit);
	virtual long IsGrenadeWeapon(long lbit);
	virtual long IsShieldWeapon(long lbit);
	virtual bool HasShield(void);
	virtual bool HasShieldDrawn(void);
	virtual long WeaponModel2ID(const char *);

	virtual bool IsWeaponBetterCurrent(long);

	virtual Vector BodyTarget( edict_t *pentBotEnemy);

	virtual edict_t *FindEnemy(void);
	bool IsBehindSG(edict_t *pentPlayer);
	virtual bool FireWeapon( Vector &vEnemy );

	virtual bool HasAlreadyHostage(const edict_t *pentHostage);

	virtual bool GoToSpEnt( void );
	virtual bool HeadTowardSpEnt(void);
	virtual void Goto(Vector &VGoto);
	virtual bool Bored(void);
	virtual bool HeadTowardWaypoint( void );
	virtual int GoNextWPOnPath(void);
	virtual int GetNextWPOnPath(void);
	virtual bool HandleNearWP(int iNearest, bool &bReturn);
	virtual void FindRoute(int iNearWP);
	virtual bool DecideOnWay(void);
	void CheckTasksOnReachability(int);

	virtual void SendRadioCommand(int);

	virtual void GrenadeThrown(void);

	// data
	float m_flBuyTime;			// last time of buying
	int m_iBotMoney;
	bool m_bIsVIP;				// is bot VIP ?
	bool m_bGlockBurst;
	
	edict_t *m_rgpentHostages[_MAXHOSTAGES];		// pointer to entities of used hostages
	int m_iHostagesUsed;			// number of touched hostages
protected:
private:
};

void BotBuy_CS_WEAPON_P228(CBotBase *pBot);
void BotBuy_CS_WEAPON_SHIELD(CBotBase *pBot);
void BotBuy_CS_WEAPON_SCOUT(CBotBase *pBot);
void BotBuy_CS_WEAPON_HEGRENADE(CBotBase *pBot);
void BotBuy_CS_WEAPON_XM1014(CBotBase *pBot);
void BotBuy_CS_WEAPON_MAC10(CBotBase *pBot);
void BotBuy_CS_WEAPON_AUG(CBotBase *pBot);
void BotBuy_CS_WEAPON_SMOKEGRENADE(CBotBase *pBot);
void BotBuy_CS_WEAPON_ELITE(CBotBase *pBot);
void BotBuy_CS_WEAPON_FIVESEVEN(CBotBase *pBot);
void BotBuy_CS_WEAPON_UMP45(CBotBase *pBot);
void BotBuy_CS_WEAPON_SG550(CBotBase *pBot);
void BotBuy_CS_WEAPON_GALIL(CBotBase *pBot);
void BotBuy_CS_WEAPON_FAMAS(CBotBase *pBot);
void BotBuy_CS_WEAPON_USP(CBotBase *pBot);
void BotBuy_CS_WEAPON_GLOCK18(CBotBase *pBot);
void BotBuy_CS_WEAPON_AWP(CBotBase *pBot);
void BotBuy_CS_WEAPON_MP5NAVY(CBotBase *pBot);
void BotBuy_CS_WEAPON_M249(CBotBase *pBot);
void BotBuy_CS_WEAPON_M3(CBotBase *pBot);
void BotBuy_CS_WEAPON_M4A1(CBotBase *pBot);
void BotBuy_CS_WEAPON_TMP(CBotBase *pBot);
void BotBuy_CS_WEAPON_G3SG1(CBotBase *pBot);
void BotBuy_CS_WEAPON_FLASHBANG(CBotBase *pBot);
void BotBuy_CS_WEAPON_DEAGLE(CBotBase *pBot);
void BotBuy_CS_WEAPON_SG552(CBotBase *pBot);
void BotBuy_CS_WEAPON_AK47(CBotBase *pBot);
void BotBuy_CS_WEAPON_P90(CBotBase *pBot);

void BotBuy_PAmmo(CBotBase *pBot);
void BotBuy_SAmmo(CBotBase *pBot);
void BotBuy_Kevlar(CBotBase *pBot);
void BotBuy_KevlarHelmet(CBotBase *pBot);
void BotBuy_Defuse(CBotBase *pBot);
void BotBuy_NightVision(CBotBase *pBot);

#endif //__CBOTCS_H
