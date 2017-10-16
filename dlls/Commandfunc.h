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
#ifndef __COMMANDFUNC_H
#define __COMMANDFUNC_H

#include "extdll.h"

extern cvar_t *jb_cstrike15;
extern cvar_t *jb_mixnames;
extern cvar_t *jb_msgwelcome;
extern cvar_t *jb_prefixaggression;
extern cvar_t *jb_suffixskill;
extern cvar_t *jb_botsmin;
extern cvar_t *jb_botsmax;
extern cvar_t *jb_chat1337;
extern cvar_t *jb_chat;
extern cvar_t *jb_chati;
extern cvar_t *jb_chatfreq;
extern cvar_t *jb_msgradio;
extern cvar_t *jb_aimmomentum;
extern cvar_t *jb_aimspeed;
extern cvar_t *jb_nnupdaterate;
extern cvar_t *jb_campprobability;
extern cvar_t *jb_msgenemydown;
extern cvar_t *jb_shoot;
extern cvar_t *jb_tkpunish;
extern cvar_t *jb_language;
extern cvar_t *jb_wp;
extern cvar_t *jb_wpfilename;
extern cvar_t *jb_wpsound;
extern cvar_t *jb_wppath;
extern cvar_t *jb_wpauto;
extern cvar_t *jb_wpautopath;
extern cvar_t *jb_wpautobots;
extern cvar_t *jb_wpautojump;
extern cvar_t *jb_wpautojumptest;
extern cvar_t *jb_wpautoadvanced;
extern cvar_t *jb_wpstats;
extern cvar_t *jb_wprecalc;
extern cvar_t *jb_wploadrandom;
extern cvar_t *jb_prefixdefensive;
extern cvar_t *jb_prefixnormal;
extern cvar_t *jb_prefixaggressive;
extern cvar_t *jb_pistolonly;
extern cvar_t *jb_skillmin;
extern cvar_t *jb_skillmax;
extern cvar_t *jb_entergame;
extern cvar_t *jb_jointeam;
extern cvar_t *jb_spraypaint;
extern cvar_t *jb_buyprobfile;
extern cvar_t *jb_showen;
extern cvar_t *jb_debugengine;

void RegisterCvars(void);
void UpdateLanguage(void);

bool bc_addbot(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_mix_names(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_welcome(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_endround(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_extjoe(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_extskill(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_min_bots(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_max_bots(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_kickbots(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_kickbot(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_leetposs(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_botchat(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_botichat(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_botuseradio(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_botmomentum(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_botvspeed(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_botskill(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_resetstat(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_nnupdate(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_campposs(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_wps(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_nnstat(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_edown(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_setme(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_search(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_savesom(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_savenn(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_trainnn(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_botshoot(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_bottkpunish(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_showen(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_debug_engine(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_setp(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_getp(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_botmenu(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_menuselect(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_language(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_debuggoal(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_waypoint(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_autowaypoint(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_autowaypointaddjump(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_advancedmovements(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_autopath(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_pathwaypoint(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_prefix(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_pistolonly(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_botskill_max(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_botskill_min(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_pause(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);

bool bc_startgnome(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_stopgnome(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_loadbuyprob(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_fillserver(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_bot_spraying(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);

bool bc_joinwhumanmax(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_joinwhumanres(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);

bool bc_randomwpload(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);

bool bc_test(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);
bool bc_botglow(edict_t *pEntity,int iType,const char *arg1,const char *arg2,const char *arg3,const char *arg4);

#endif //__COMMANDFUNC_H
