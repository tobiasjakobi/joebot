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
#ifndef __CBOTDOD_H
#define __CBOTDOD_H

#include "CBotBase.h"

#define _MAXFLAGS 10

typedef struct{
	int iFlag;
	Vector vOrigin;
	edict_t *pEdict;
}DOD_Flag;

extern DOD_Flag g_DODFlags[_MAXFLAGS];
extern float fDODFlagU;

#define _DOD_TIMETOUPDATEFLAGS 1.0

class CBotDOD : public CBotBase{
public:
	CBotDOD();
	~CBotDOD();

	virtual void Think(void);
	virtual void Think5th(void);			// think 5th
	virtual void Think1(void);				// think every 1s
	virtual void Fight(void);
	virtual bool HeadTowardWaypoint(void);
	virtual bool HandleNearWP(int iNearest, bool &bReturn);

	virtual void HandleMenu(void);
	virtual bool HandleOrders(void);
	virtual void ReactOnRadio(void);
	virtual bool ReactOnSound(void);
	virtual void SpawnInit(void);
	virtual void Init (void);
	virtual void UpdateSkill(void);
	virtual void PreprocessTasks(void);

	virtual long IsPrimaryWeapon(long lbit);
	virtual long IsSecondaryWeapon(long lbit);
	virtual long IsShotgunWeapon(long lbit);
	virtual long IsSubMWeapon(long lbit);
	virtual long IsRifleWeapon(long lbit);
	virtual long IsSniperWeapon(long lbit);
	virtual long IsKnifeWeapon(long lbit);
	virtual long IsMachineGunWeapon(long lbit);
	virtual long IsGrenadeWeapon(long lbit);
	virtual long WeaponModel2ID(const char *);

	virtual bool IsWeaponBetterCurrent(long);

	virtual bool IsProne(edict_t *pEdict);
	virtual bool GoProne(bool bProne, float fTime);

	virtual bool GoToSpEnt(void);
	virtual bool HeadTowardSpEnt(void);
	virtual void FindRoute(int iNearWP);
	virtual bool DecideOnWay(void);
	virtual bool Bored(void);
	virtual void UpdateFlags(void);

	virtual edict_t *FindEnemy(void);
	virtual bool FireWeapon( Vector &v_enemy );

	virtual void SendRadioCommand(int iType);

	//data
	//float f_ProneAdv;			// advantage of prone enemies when recognizing them
	float f_pronetill;
	float f_changedprone;
	int iStamina;
	bool bSlowed;
	bool bBleeding;
	bool bTNT;
protected:
private:
};

#endif //__CBOTDOD_H
