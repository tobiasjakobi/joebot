#ifndef __BOTNNDEFS_H
#define __BOTNNDEFS_H

#include "../dlls/bot_weapons.h"

// def values for every weapon

// low range  -> -1 (-> knife);
// high range ->  1 (-> awp);

const double dr_weapon_ak47			=   0;		// 4 1
const double dr_weapon_aug			=  .6;		// 4 4
const double dr_weapon_awp			= 1.0;		// 4 6
const double dr_weapon_c4			=- .9;		//
const double dr_weapon_deagle		=  .0;		// 1 3
const double dr_weapon_elite		=  .1;		// 1 5 t
const double dr_weapon_flashbang	=- .5;		//
const double dr_weapon_g3sg1		=  .9;		// 4 7
const double dr_weapon_glock18		=- .5;		// 1 2 t
const double dr_weapon_hegrenade	=- .5;		//
const double dr_weapon_knife		=-1.0;
const double dr_weapon_m249			=- .3;		// 5 1
const double dr_weapon_m3			=- .2;		// 2 1
const double dr_weapon_m4a1			=  .6;		// 4 3
const double dr_weapon_mac10		=- .1;		// 3 4 t
const double dr_weapon_mp5navy		=  .2;		// 3 1
const double dr_weapon_p228			=- .2;		// 1 4
const double dr_weapon_p90			=  .1;		// 3 3
const double dr_weapon_tmp			=  .3;		// 3 2
const double dr_weapon_usp			=- .6;		// 1 1 ct
const double dr_weapon_scout		=  .9;		// 4 5
const double dr_weapon_sg552		=  .4;		// 4 2
const double dr_weapon_smokegrenade	=- .5;		//
const double dr_weapon_xm1014		=- .4;		// 2 2
const double dr_weapon_fiveseven	=- .2;		// 1 5 ct
const double dr_weapon_sg550		=  .9;		// 4 8
const double dr_weapon_ump45		=  .4;		// 3 5
const double dr_weapon_galil		=  .0;
const double dr_weapon_famas		=  .1;
const double dr_weapon_shield		=-1.0;

// indices for layers of nn

enum Inputs{
	IHealth = 0,		// health of bot
	IDistance,			// distance to en
	//IEnMove,			// Movements of En
	//IPositionH,			// pos of en - left _ right
	IEWeapon,			// pos of en - top _ bottom
	IWeapon,			// special value for each weapon - range
	IAmmo,				// amount of ammo in used clip
	ISituation,			// situation, i.e. how many enemies are fighting against how many allies ?
	IEND
};

const long _NNBInput = IEND;

enum Health{
	H_Low		= -1,
	H_Mid		= 0,
	H_High		= 1
};

enum Distance{
	D_Near		= -1,
	D_Mid		= 0,
	D_Far		= 1
};

/*enum EMove{
	EM_Stand	= -1,
	EM_Walk		= 0,
	EM_Run		= 1
};*/

/*enum HPositions{
	HP_Left		= -1,
	HP_Center	= 0,
	HP_Right	= 1
};*/

/*enum VPositions{
	VP_Under	= -1,
	VP_Center	= 0,
	VP_Up		= 1
};*/

enum EWeapon{
	EW_Low		= -1,
	EW_Mid		= 0,
	EW_Long		= 1
};

enum Weapon{
	W_Low		= -1,
	W_Mid		= 0,
	W_Long		= 1
};

enum AmmoClip{
	A_Low		= -1,
	A_Mid		= 0,
	A_High		= 1
};

enum Situations{
	S_Bad = -1,
	S_Normal =0,
	S_Good = 1
};

enum Outputs{
	ODuck = 0,			// Duck
	OJump,				// Jump
	OHide,				// run backward - forward
	OMoveType,			// Run - Walk
	OStrafe,			// strafe left - right
	OEND
};

const long _NNBOutput = OEND;

enum Duck{
	D_NoDuck	= -1,
	D_Duck		= 1
};

enum Jump{
	J_NoJump	= -1,
	J_Jump		= 1
};

enum Moves{
	M_Back		= -1,
	M_None		= 0,
	M_Forward	= 1
};

enum MoveType{
	T_None = -1,
	T_Walk = 0,
	T_Run  = 1
};

enum Strafe{		// strafe
	S_Left=-1,
	S_None=0,
	S_Right=1
};

enum Hide{
	H_Hide = 1,
	H_None = 0
};

// defs for collision NN

enum ICollII{
	ICI_Left = 0,
	ICI_Middle,
	ICI_Right
};

enum OCollLoc{
	OC_Left = -1,
	OC_Middle = 0,
	OC_Right = 1,
	OC_LEFT = -1,
	OC_MIDDLE = 0,
	OC_RIGHT = 1
};

#endif  // __BOTNNDEFS_H
