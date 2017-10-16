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
#ifndef __BOT_GLOBALDEFS_H
#define __BOT_GLOBALDEFS_H

// fillserver #defs
#define FILL_0		1
#define FILL_1		2
#define FILL_ALL	3
#define FILL_HALF	4
#define FILL_FULL	5

#define FL_ES_DELAY_SG		(1<<0)
#define FL_ES_DELAY_PRONE	(1<<1)
#define FL_ES_DELAY_DUCK	(1<<2)
#define FL_ES_DELAY_FIRE	(1<<3)
#define FL_ES_DELAY_NFIRE	(1<<4)
#define FL_ES_DELAY_BLINDED	(1<<4)

#define TI_ES_DELAY_SG	4		// fixed 4 seconds

#define _MAXC4DIST			300
#define _MAXBOMBTARGETDIST	50
#define _MAXC4DEFDIST		150
#define _SMOKERADIUS		200

#define UPLOOK -89
#define DOWNLOOK 89
#define LADDER_UNKNOWN	0
#define LADDER_DOWN		-1
#define LADDER_UP		1

#define _TIMETOBEBORED 45.0f
#define _TIMEBEINGBORED 4.0f

#define _ADDSTDWPDIST 100

#define _NEAR 100
#define SNIPER_ROTATE_TIME 5
#define _CAMPTIME 25.0f
#define _ANGLEDIFFCAMP 20.0f

#define _MAXSTRAFE (f_max_speed / 1.5f)

#define LANG_EN	0
#define LANG_DE	1
#define LANG_FR	2

#define _SEARCHEN 10
#define _ESTIME 40

#define _LERITRACE 400.0f		// distance to trace left and right
#define _HIDESTEP 50.0f			// accuracy to search

#define BOT_PITCH_SPEED  20
#define BOT_YAW_SPEED    20

#define DOD_FLAG_NONE	0
#define DOD_FLAG_ALLIES	1
#define DOD_FLAG_AXIS	2

#define MAX_WP_DIR 20

#define RESPAWN_IDLE             1
#define RESPAWN_NEED_TO_RESPAWN  2
#define RESPAWN_IS_RESPAWNING    3

// game start messages for TFC...
#define MSG_TFC_IDLE          1
#define MSG_TFC_TEAM_SELECT   2
#define MSG_TFC_CLASS_SELECT  3

// game start messages for CS...
#define MSG_CS_IDLE         1
#define MSG_CS_TEAM_SELECT  2
#define MSG_CS_CT_SELECT    3
#define MSG_CS_T_SELECT     4
#define MSG_RADIO1			5
#define MSG_RADIO2			6
#define MSG_RADIO3			7

// define player classes used in TFC
#define TFC_CLASS_CIVILIAN  0
#define TFC_CLASS_SCOUT     1
#define TFC_CLASS_SNIPER    2
#define TFC_CLASS_SOLDIER   3
#define TFC_CLASS_DEMOMAN   4
#define TFC_CLASS_MEDIC     5
#define TFC_CLASS_HWGUY     6
#define TFC_CLASS_PYRO      7
#define TFC_CLASS_SPY       8
#define TFC_CLASS_ENGINEER  9

// game start messages for DOD...
#define MSG_DOD_IDLE					1
#define MSG_DOD_TEAM_SELECT				2
#define MSG_DOD_CLASS_SELECT_AX			3
#define MSG_DOD_CLASS_SELECT_AL			4
#define MSG_DOD_CLASS_SELECT_AX_PARA	5
#define MSG_DOD_CLASS_SELECT_AL_PARA	6
#define MSG_DOD_WPN_SELECT_SERGEANT		7
#define MSG_DOD_WPN_SELECT_MACHINE		8
#define MSG_DOD_WPN_SELECT_GRENADIER	9
#define MSG_DOD_WPN_SELECT_RIFLEMAN		10
#define MSG_DOD_WPN_SELECT_FG42			11

#define DOD_CLASS_ASSAULTINF		1
#define DOD_CLASS_LIGHTINF  		2
#define DOD_CLASS_SNIPER    		3
#define DOD_CLASS_HEAVYWEAPONS  	4
#define DOD_CLASS_LEICHTINF			5
#define DOD_CLASS_STURMTRUP			6
#define DOD_CLASS_GERMANSNIPER		7
#define DOD_CLASS_HEAVYWEAPONSGER	8

#define BOT_SKIN_LEN 32
#define BOT_NAME_LEN 32

#define _MAXHOSTAGES 4		// max number of hostages to take with'em
#define _MAXGRENADEREC 5
#define _MAXHOSTAGEREC 6

#define _DEFAULTNNUPDATE	5
#define _DEFAULTNNLRATE		.1f
#define _DEFAULTAMOMENTUM	.7f
#define _DEFAULTASPEED		1.0f
#define _DEFAULTSKILL		90

#define PLAYER_SEARCH_RADIUS 40.0

// max time to wait for a WP, otherwise create new way
#define MAXTIMEPERWP 5.0

// distance to check on falling
#define _DSTDOWN 20

// time to hide when bot is defensive (this value is multiplied by a random value from .7 to 1.3 to make it less predictable)
#define _HIDECAMPTIME 5.0f

// max length of logging nn in and outputs
#define _MAXNNLOG 20

// defs of bot's states
#define BA_DEFUSE		(1<<0)		// bot's defusing bomb
#define BA_PLANT		(1<<0)		// bot's planting bomb
#define BA_FIGHT		(1<<1)		// bot's fighting
#define BA_SUSPLOC		(1<<2)		// bot has remarked a possible position of an en
#define BA_BUYZONE		(1<<3)		// bot's in buyzone
#define BA_BOMBZONE		(1<<4)		// bot's in bombzone ( NA )
#define BA_BOMBPL		(1<<5)		// bomb has been planted
#define BA_PICKUP		(1<<6)		// bot's trying to pick up a weapon
#define BA_DEFKIT		(1<<7)		// bot's carrying a defusion kit
#define BA_TKAVOID		(1<<8)		// bot is avoiding tk
//#define BA_GOBUTTON		(1<<9)		// bot want to go to a button to open a door in his way

// defs of bot's orders
#define BO_COVER		(1<<0)		// bot should cover specified entity
//
// pREdict			edict to cover
//
#define BO_HELP			(1<<1)		// bot should run and help specified entity
#define BO_FOLLOW		(1<<2)		// bot should follow specified entity
//
// pREdict			edict to follow
//
#define BO_ROAMTEAM		(1<<3)		// bot should go along with others. Note that this 'order' is given to the bot by himself, therefore it can be canceled every time. The time the bot made this decision is stored
//
// pREdict			 edict to follow
// 
#define BO_GUARD		(1<<4)		// bot should guard sth. This can be a teamm8 planting, defusing the bomb etc - the time the bot can stop guarding is stored
//
// pREdict				edict to guard
// fTime				till fTime
// 
#define BO_CAMPATGOAL	(1<<5)		// bot should go normal to far_goal and camp there
//
// Camp at far goal (iFarGoal)
//
#define BO_HOLDPOS		(1<<5)		// bot should wait
//
// fTime				till fTime
// 
#define BO_WAIT4TM8		(1<<6)		// wait for teamm8
//
// fTime				max time to wait
//

#define BO_AFF			(1<<31)		// an order has been affirmed

#define BO2I			(BO_ROAMTEAM|BO_CAMPATGOAL)		// bot oders which can be ignored

// max radio send freq of bots
#define _RADIO_FREQ 5.0f	// in sec

// radio commands
#define RC_COVER_ME					1
#define RC_YOU_TAKE_THE_POINT		2
#define RC_HOLD_THIS_POSITION		3
#define RC_REGROUP_TEAM				4
#define RC_FOLLOW_ME				5
#define RC_TAKING_FIRE				6
#define RC_GO_GO_GO					7
#define RC_TEAM_FALL_BACK			8
#define RC_STICK_TOGETHER_TEAM		9
#define RC_GET_IN_POSITION_AND_WAIT 10
#define RC_STORM_THE_FRONT			11
#define RC_REPORT_IN_TEAM			12
#define RC_AFFIRMATIVE				13
#define RC_ENEMY_SPOTTED			14
#define RC_NEED_BACKUP				15
#define RC_SECTOR_CLEAR				16
#define RC_IN_POSITION				17
#define RC_REPORTING_IN				18
#define RC_GET_OUT_OF_THERE			19
#define RC_NEGATIVE					20
#define RC_ENEMY_DOWN				21
#define RC_TEAMATTACK				22
#define RC_HOSTAGE_DOWN				23

// #define map types
#define MT_CS 1 
#define MT_DE 2
#define MT_AS 3
#define MT_ES 4

// #define type of wanted way
#define WW_NONE		0		// no special way, just totally random
#define WW_VAGG		1		// very agressive
#define WW_AGG		2		// aggressive
#define WW_MED		3		// nothing of that all, just the middle
#define WW_DEF		4		// defensive, i.e. low killed rate
#define WW_VDEF		5		// very defensive, i.e. very low killed rate

//#defs for NNLOG -> SOMANALYSIS
#define _LOGNETINPUT 0
#define _LOGNETOUTPUT 1

// AvoidCollision stuff

#define _CAA 35		// angles of each trace to middle
#define _CAL 75		// length of traces

#define _WPFILEDESC "Joe_bot"

#endif //__BOT_GLOBALDEFS_H
