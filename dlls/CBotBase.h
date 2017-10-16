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
#ifndef __CBOTBASE_H
#define __CBOTBASE_H

#include "extdll.h"
#include "studio.h"

#include "bot_globaldefs.h"
#include "bot_weapons.h"
#include "waypoint.h"

#include "ChatBot.h"
#include "CLTMem.h"
#include "CPersonality.h"
#include "CRadio.h"
#include "CState.h"
#include "CSTMem.h"
#include "CTask.h"

#include "BotNNDefs.h"
#include "NeuralNetDefs.h"

// stuff for Win32 vs. Linux builds

#ifdef _WIN32

#ifndef USE_METAMOD
typedef int (FAR *GETENTITYAPI)(DLL_FUNCTIONS *, int);
typedef int (FAR *GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
typedef void (DLLEXPORT *GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
// Typedef for Server_GetBlendingInterface() from Eric Smith on the hlcoders mailing list.
typedef int (FAR *SERVER_GETBLENDINGINTERFACE)(int version, struct sv_blending_interface_s **ppinterface,struct engine_studio_api_s *pstudio,float (*rotationmatrix)[3][4], float (*bonetransform)[MAXSTUDIOBONES][3][4]);
#endif /* not USE_METAMOD */
typedef void (FAR *LINK_ENTITY_FUNC)(entvars_t *);

#else

#include <dlfcn.h>
#define GetProcAddress dlsym

typedef int BOOL;

#ifndef USE_METAMOD
typedef int (*GETENTITYAPI)(DLL_FUNCTIONS *, int);
typedef int (*GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
typedef void (*GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
typedef int (*SERVER_GETBLENDINGINTERFACE)(int version, struct sv_blending_interface_s **ppinterface,struct engine_studio_api_s *pstudio,float (*rotationmatrix)[3][4], float (*bonetransform)[MAXSTUDIOBONES][3][4]);
#endif /* not USE_METAMOD */
typedef void (*LINK_ENTITY_FUNC)(entvars_t *);

#endif

#define _PAUSE_TIME 1.0

typedef struct{
	edict_t *p;
	Vector VOrigin;
	bool bUsed;
} GrenadeLog_t;

typedef struct{
	edict_t *p;
	Vector VOrigin;
	float fVelocity;
	bool bUsed;
} HostageLog_t;

extern GrenadeLog_t gSmoke[_MAXGRENADEREC];
extern GrenadeLog_t gFlash[_MAXGRENADEREC];
extern HostageLog_t gHossi[_MAXHOSTAGEREC];

class FEItem{
public:
	FEItem();
	~FEItem();

	edict_t *pEdict;

	float fLastSeen;		// last seen time -> time when bot reacted on enemy
	float fRecogn;			// time when bot was visible, but that doesn't mean that bot recognizes it as an enemy
	float fTime2seeit;		// time it takes to recognize the enemy
	float fDistance;
	float fDotView;			// ?
	bool bBehindSG;
	bool bProne;
	bool bSooting;
	long lButton;
	long lFlags;
};

class GrenadeOrder{
public :
	GrenadeOrder();
	~GrenadeOrder();
	void Attack(Vector &, long);

	float fStart;
	long lState;
	long lTypeoG;
	Vector VAim;
	edict_t *pAim;
};

enum grenadeorderstateline{
	GO_START=1,
	GO_SELECT,
	GO_WAITFS,
	GO_PREAIM,
	GO_AIM,
	GO_RELEASE1,
	GO_RELEASE2,
	GO_FINDGREN,
	GO_QUIT
};

class CBotBase{
public:
	// public functions
	CBotBase();
	virtual ~CBotBase();

	virtual void Think(void);
	virtual void Think5th(void);			// think 5th
	virtual void Think1(void);				// think every 1s
	virtual void Fight(void);
	virtual bool PauseShoot(void);
	virtual void VATurnTo(const Vector &);
	virtual void InstantTurn(void);
	virtual bool FireWeapon( Vector &v_enemy);

	virtual void HandleMenu(void) = 0;
	virtual bool HandleOrders(void);
	virtual void AdaptAvoid(void);
	virtual void UpdateSkill(void);
	static void MakeName(char *,const char*,int,float);
	virtual void SpawnInit(void) = 0;
	virtual void Init(void) = 0;
	virtual long WeaponModel2ID(const char *);

	virtual bool AvoidStuck(void);
	virtual bool AvoidCollision(void);
	virtual bool CanJumpUp(void);
	virtual void FixIdealYaw(void);
	virtual void FixIdealPitch(void);
	virtual void TestOnButtonWay(edict_t **);
	virtual bool BoolTestOnButtonWay(int,int);

	virtual void TestOnButtonWay(CWay &,edict_t **);
	virtual bool BoolTestOnButtonWay(CWay &,int,int);

	virtual float ChangePitch( void );
	virtual float ChangeBodyPitch( void );
	virtual float ChangeBodyYaw( void );
	virtual float ChangeYaw( void );

	virtual bool Jump(void);
	virtual bool Wander(void);
	virtual bool HeadToward(Vector &);
	virtual bool InitCamp(void);
	virtual bool SearchForIL(void);
	virtual bool GoToSpEnt( void );
	virtual bool Bored(void);
	virtual bool HeadTowardWaypoint( void );
	virtual int GoNextWPOnPath(void);
	virtual int GetNextWPOnPath(void);
	virtual bool HandleNearWP(int iNearest, bool &bReturn);
	virtual void OnLadder( void );
	virtual void AdjustLadder( void );
	virtual bool DecideOnWay(void);

	virtual void PreprocessTasks(void);

	virtual long RoamteamFindWP(edict_t *);

	virtual edict_t *FindEnemy(void);
	virtual netvar_t ConvertDistance(const netvar_t&);
	virtual bool GotoHidingPlace(void);
	virtual bool SearchHidingPlace(edict_t *);
	virtual int SearchHidingWP(edict_t *,int iL=-2);
	virtual long Change2Weapon(const long);
	virtual long ChangeToLWeapon(void);
	virtual void ShootAtEnemy( void );
	virtual Vector BodyTarget( edict_t *pBotEnemy);

	virtual void KilledBy(edict_t *,long);
	virtual void KilledSO(edict_t *,long);
	
	static long WeaponClass2ID(const char *);
	virtual long IsPrimaryWeapon(long lbit)=0;
	virtual long IsSecondaryWeapon(long lbit)=0;
	virtual long IsShotgunWeapon(long lbit)=0;
	virtual long IsSubMWeapon(long lbit)=0;
	virtual long IsKnifeWeapon(long lbit)=0;
	virtual long IsRifleWeapon(long lbit)=0;
	virtual long IsSniperWeapon(long lbit)=0;
	virtual long IsMachineGunWeapon(long lbit)=0;
	virtual long IsGrenadeWeapon(long lbit)=0;
	virtual long IsCWeaponPrimary(void);
	virtual long IsCWeaponSecondary(void);
	virtual long IsCWeaponShotgun(void);
	virtual long IsCWeaponSubM(void);
	virtual long IsCWeaponKnife(void);
	virtual long IsCWeaponRifle(void);
	virtual long IsCWeaponSniper(void);
	virtual long IsCWeaponMachineGun(void);
	virtual long IsCWeaponGrenade(void);
	virtual long HasPrimary(void);
	virtual long HasSecondary(void);
	virtual long HasShotgun(void);
	virtual long HasSubM(void);
	virtual long HasRifle(void);
	virtual long HasSniper(void);
	virtual long HasMachineGun(void);
	virtual long HasGrenade(void);
	virtual long HasKnife(void);
	bool HasWeapon(long);

	virtual bool IsWeaponBetterCurrent(long);

	void ResetWPlanning(void);
	virtual void SendRadioCommand(int)=0;
	virtual bool Camp(void);
	virtual bool CheckGrenadeThrowing(void);
	virtual void HandleGOrder(void);
	virtual void GrenadeThrown(void);			// anything to do after the grenade has been thrown ?
	virtual void HandleReplay(void);

	// chat stuff
	int LoadChat(void);

	// public data
	edict_t *pEdict;

	Vector v_avoid;

	FEItem FFE[32];

	int iEIndex;

	bool need_to_initialize;
	char name[BOT_NAME_LEN+1];
	char skin[BOT_SKIN_LEN+1];	// useless in cs-> defined by model
	bool not_started;
	int start_action;
	//float kick_time;
	
	int i_ITP;
	float f_Delay;			// delay before first shoot
	float f_PauseShoot;		// time to pause shooting - Used by PauseShoot based on Data from NNWeapon.cpp
	float f_MaxRecoil;		// max recoil
	float f_DenyWChange;	// set it to a certain time when u don't want any weapon changes
	float f_TKPunish;
	float f_Camp;			// last time of camping
	float f_StrCamp;		// last time of strategic camping
	float f_DefCamp;		// last time of defensive camping
	//bool bFight;			// flag, which is set to true, while fighting
	int iOnLadder;			// flag for ladder handling -> 1 means up, -1 down and 0 no ladder
	edict_t *pELadder;		// point to entity of current ladder - no ladder -> 0
	
	long lButton;			// temp storage for pressed buttons
	// things from pev in CBasePlayer...
	int bot_team;
	int bot_teamnm;		// coherent with UTIL_GETTEAM ...
	int bot_class;
	int bot_health;
	int bot_armor;
	int bot_weapons;  // bit map of weapons the bot is carrying
	int bot_skill;

	bool bFlashlight;
	
	float fViewSpeedYaw;		// speed of changing view yaw
	float fViewSpeedPitch;		// speed of changing view pitch
	float fIdealAnglePitch;
	float fIdealAngleYaw;
	float fAngleSpeedPitch;
	float fAngleSpeedYaw;

	float f_ReactionTime;
	
	float yaw_degrees,pitch_degrees;
	float old_yaw_degrees,old_pitch_degrees;
	CRadio bot_IRadioM;
	//bot_nn_log sbNNLog;

	long lTypeOWW;				// type of wanted way -> safe, 'kill-extensive', no special stuff etc .... WW_*
	int iGoal;					// goal running to
	int iFarGoal;				// goal where bot should arrive in some time
	int iWantedDiv;				// wanted division of route

	edict_t *pNEn;				// nearest enemy
	edict_t *pNTeam;			// nearest teammember to enemy
	
	edict_t *pBotEnemy;			// point to en edict
	edict_t *pLastGlobalKill;
	edict_t *pLastGlobalKilled;
	float f_SearchEnemy;
	edict_t *pBotOldEnemy;		// point to old en edict
	float f_Situation;			// diff of bot teams fighting against + manner + aggressivity
	Vector v_Offset;				// 'conscious' offset to locations bot's shooting @ - angles
	float f_AMomentum;			// Aiming speed ( 1.0-value of momentum ) 
	float f_ASpeed;				// aiming speed (c)
	float f_AimHead;			// 0-1 ... to make better and defined HEADSHOTS
	float f_InDistance;			// max distance at which an enemy can be seen instantly
	float f_TPD;				// time in milliseconds to view 1000 units further
	float f_ProneInDistance;	// max distance at which prone enemies are seen instantly
	float f_ProneTPD;			// time in milliseconds to view 1000 units further
	long lEnWeapon;				// weapon of enemy
	CSTMem STMem;				// short term memory
	CLTMem LTMem;				// long term memory

	CState Action;				// current action of bot
	//edict_t *pButton2Use;		// only with BA_GOBUTTON
	//int iButtonAGoal;			// the goal after the button is pressed
	//CState Order;				// current order to bot, i.e. fulfilling radio command etc.
	CTask Task;
	CState Ordered;				// what the bot ordered

	float f_RWKnife;
	float f_NJumpTill;			// No Jump till
	float f_TimeBJumps;			// Time between jumps

	float f_IOrigD;				// distance to last most intersting origin of an wp
	float f_LTSFIP;				// Last time searching for an interesting place
	float f_LookTo;				// time to look a a location specified by VLookTo, while running to VRunningTo
	//float f_GoBack;				// normally the bot wouldn't look really back , but u can force it to to this with this var.
	GrenadeOrder GOrder;
	Vector VRunningTo;			// ..
	Vector VLookTo;				// ..

	Vector VSuspEn;				// bot hears so there
	float f_SuspRem;			// time where bot remarked en ..
	float f_CheckSpEnt;
	//bool bSawSuspLoc;			// has susp loc been seen ?
	//bool bPlantDefBomb;		// bot's planting or defusing bomb
	edict_t *killer_edict;		// the guy who killed u ...
	edict_t *pEKilled;			// the guy u killed ...
	edict_t *pELVictim;			// the last guy u killed
	Vector LastEnemyOrigin;		// ...
	Vector VHidingPlace;		// where bot has decided to hide from current enemy
	float f_UsedRadio;			// last time using radio
	float f_shootbox;			// shoot through everything @ enemy
	bool bSeeThru;				// when shootbox, does the bot know the exact position of enemy ?
	//float f_GotoHidingPlace;	// set to last time, where bot should try to reach VHidingPlace
	int iJumpOC;				// the frames till a bot jumps over sth are stored here
	//float f_Hide;				// Bot hides this time ...
	float f_HidingTime;			// bot was last time searching hide place
	//edict_t *pHidefrom;			// this var contains a pointer to the enemy the bot is hiding from, when f_Hide is > gpGlobals->time
	
	float f_max_speed;			// max speed
	float f_strafe;				// strafing speed
	float prev_speed;			// speed at last iteration	1/5s
	Vector v_prev_origin;		// ...						1/5s
	float prev_speed1;			// speed at last iteration	1s
	Vector v_prev_origin1;		// ...						1s
	Vector v_diff;				// vector from previous to current location	1/5s
	Vector v_diff1;				// vector from previous to current location	1s
	float f_ducktill;			// ducktill this time
	float f_lastWP;				// time ya visited the last WP
	float f_ResetWP;			// last time the i_CurrWP was resetted
	float f_noWP;				// to to visit no WP
	float f_dont_check_stuck;
	float f_bot_stuck;			// last time bot was stuck
	long lStuckC;				// counter of getting stuck the last time
	float f_noStuck;			// time of last 'free' running
	//float f_Camp;				// time, till bot is snipering
	float f_noCamp;				// don't stay a sniper points ( to get there away )
	float f_CaLooktoWP;			// time of looking to one wp
	CWay  WLookTo;				// wp to look to;					// snipering/camping
	short sLookToChange;		// direction of change				// snipering/camping
	float f_Pause;			// pausing bot
	float f_blinded_time;		// flashbang
	float f_round_time;			// length of one round
	//float f_end_freezetime;		// end of freezetime
	float f_end_use_ladder_time;		// end using ladder
	float f_start_use_ladder_time;		// start using ladder
	float f_use_button_time;

	bool b_neardoor;
	float f_dont_avoid;
	//float f_avoid_strafe;
	int i_avoidstrafe;			// value to strafe while avoiding obstacle
	float f_gotohostage;		// time to go to hostage after recognizing it
	float f_Bored;				// time, when bot was bored last time
	int i_BorA;					// action of this bored sequence
	float f_BoredTill;			// duration of 'bored period'
	float fEnemyDist;
	float f_NNNUpdate;			// time after which an nn update is possible ( NextNN )
	double dCombatNNIn	[_NNBInput];	// last input of combat nn
	double dCombatNNOut	[_NNBOutput];	// last output of combat nn
	double dCollNNIn	[3];
	double dCollNNOut	[1];
	float f_Aggressivity;		// current aggressivity of bot. this value is added to the assessement of the situation
	double d_Manner;			// is the bot aggressive of defensive in general ?
	long lPWeapon;				// Preferred weapon
	char szPWeapon[20];			// string of pweapon

	// stuff for replaying movement data
	ADV_WPRecMove *pWPAMPlay;
	float fStartADVM;
	bool bReplay;

	float f_move_speed;			// current wanted move speed
	float moved_distance;		// length of v_diff vector (distance bot moved) 1/5s
	float moved_distance1;		// length of v_diff vector (distance bot moved) 1s

	int i1C;					// every sec ( counting down every 5th and if 0, set up to 5 ) 
	Vector v_diff_angles;		// ...
	float f_LastFight;			// time of last fight
	float f_IgnoreFall;			// ignore falling down -> at ladder p.ex.
	int i_FOV;					// current fov ... that obv not the same like pEdict->v.fov

	float fRightDist,fMiddleDist,fLeftDist;	// feeler of the bot

	// 'not frontal' collision stuff
	//float f_old_direction;
	long l_cState;			// first (0) jump to come over obstacle - ifthis wasn't sucessful and next time ya still stuckin', change to the direction of the obstacle (wall, ...)

	bool bRSInit;			// is set to 1 at beginning of a round
	
	// waypoint stuff
	int i_CurrWP;			// current wp bot is aiming to
	int iNearWP;
	float f_D2C1;
	float f_D2C2;
	int i_LastWP;			// last wp

	bot_current_weapon_t current_weapon;  // one current weapon for each bot
	int m_rgAmmo[MAX_AMMO_SLOTS];  // total ammo amounts (1 array for each bot)

	enum{
		_MAXLKW = 4
	};
	int iCLKW;
	int FLKW[_MAXLKW];

	CPersonality Personality;
	CChatBot Chat;

	char m_szDebugMsg[1024];
protected:
private:
};

enum RadioCommands{
	RADIO_IM_IN_POSITION = 0,
	RADIO_COVER_ME,
	RADIO_FOLLOW_ME,
	RADIO_NEED_ASSISTANCE,
	RADIO_NEED_BACKUP,
	RADIO_ENEMY_DOWN,
	RADIO_ENEMY_SPOTTED,
	RADIO_AFFIRMATIVE,
	RADIO_NEGATIVE,
	RADIO_GOGOGO,
	RADIO_SECTOR_CLEAR,
	RADIO_YOU_TAKE_THE_POINT,
	RADIO_FALL_BACK,
	RADIO_HOLD_THIS_POSITION,
	/*mainly DOD stuff*/
	RADIO_LEFT,
	RADIO_RIGHT,
	RADIO_GRENADE,
	RADIO_TAKE_COVER,
	RADIO_SNIPER,
	RADIO_MEDIC,

	RADIO_MAX_ITEM
};

#endif // __CBOTBASE_H
