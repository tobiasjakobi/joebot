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
#include <string.h>

#include "extdll.h"
#include "util.h"

#include "CCommand.h"
#include "Commandfunc.h"

#include "bot_globaldefs.h"

CCommand :: CCommand(){
	*szName = 0;
	*sz2Name = 0;
	iType = 0;
	pFunc = 0;
}

CCommand :: ~CCommand(){
}

// commands

CCommands ::CCommands(){
	AddCommand("addbot",		"ab",		(void*)bc_addbot,CM_ALL);
	AddCommand("mix_names",		"mn",		(void*)bc_mix_names,CM_ALL);
	AddCommand("welcome",		"welcome_message",(void*)bc_welcome,CM_ALL);
	AddCommand("endround",		"er",		(void*)bc_endround,CM_ALL);
	AddCommand("extjoe",		"ej",		(void*)bc_extjoe,CM_ALL);
	AddCommand("extskill",		"es",		(void*)bc_extskill,CM_ALL);
	AddCommand("min_bots",		"mib",		(void*)bc_min_bots,CM_ALL);
	AddCommand("max_bots",		"mab",		(void*)bc_max_bots,CM_ALL);
	AddCommand("kickbots",		"kbs",		(void*)bc_kickbots,CM_ALL);
	AddCommand("kickbot",		"kb",		(void*)bc_kickbot,CM_ALL);
	AddCommand("leetposs",		"lp",		(void*)bc_leetposs,CM_ALL);
	AddCommand("botchat",		"bot_chat",	(void*)bc_botchat,CM_ALL);
	AddCommand("botichat",		"bot_ichat",(void*)bc_botichat,CM_ALL);
	AddCommand("botuseradio",	"bur",		(void*)bc_botuseradio,CM_ALL);
	AddCommand("botmomentum",	"botmom",	(void*)bc_botmomentum,CM_ALL);
	AddCommand("botvspeed",		"botvs",	(void*)bc_botvspeed,CM_ALL);
	AddCommand("botskill",		"bs",		(void*)bc_botskill,CM_ALL);
	AddCommand("resetstat",		"rs",		(void*)bc_resetstat,CM_ALL);
	AddCommand("nn_update",		"nnupdate",	(void*)bc_nnupdate,CM_ALL);
	AddCommand("campposs",		"cp",		(void*)bc_campposs,CM_ALL);
	AddCommand("wps",			"",			(void*)bc_wps,CM_ALL);
	AddCommand("nnstat",		"",			(void*)bc_nnstat,CM_ALL);
	AddCommand("edown",			"enemy_down",(void*)bc_edown,CM_ALL);
	AddCommand("setme",			"sm",		(void*)bc_setme,CM_ALL);
	AddCommand("search",		"",			(void*)bc_search,CM_CONSOLE);
	AddCommand("savesom",		"",			(void*)bc_savesom,CM_ALL);
	AddCommand("savenn",		"",			(void*)bc_savenn,CM_ALL);
	AddCommand("trainnn",		"",			(void*)bc_trainnn,CM_ALL);
	AddCommand("botshoot",		"bot_shoot",(void*)bc_botshoot,CM_ALL);
	AddCommand("bottkpunish",	"",			(void*)bc_bottkpunish,CM_ALL);
#ifdef _DEBUG
	AddCommand("showen",		"",			(void*)bc_showen,CM_ALL);
	AddCommand("debug_engine",	"de",		(void*)bc_debug_engine,CM_ALL);
#endif
	AddCommand("setp",			"",			(void*)bc_setp,CM_ALL);
	AddCommand("getp",			"",			(void*)bc_getp,CM_ALL);
	AddCommand("botmenu",		"bm",		(void*)bc_botmenu,CM_CONSOLE);
	AddCommand("menuselect",	"",			(void*)bc_menuselect,CM_ALL);
	AddCommand("language",		"",			(void*)bc_language,CM_ALL);
	AddCommand("debuggoal",		"",			(void*)bc_debuggoal,CM_ALL);
	AddCommand("waypoint",		"wp",		(void*)bc_waypoint,CM_ALL);
	AddCommand("autowaypoint",	"autowp",		(void*)bc_autowaypoint,CM_ALL);
	AddCommand("autowaypointaddjump","autowpaj",(void*)bc_autowaypointaddjump,CM_ALL);
	AddCommand("advancedmovements","advm",	(void*)bc_advancedmovements,CM_ALL);
	AddCommand("autopath",		"ap",		(void*)bc_autopath,CM_ALL);
	AddCommand("pathwaypoint",	"pw",		(void*)bc_pathwaypoint,CM_CONSOLE);
	AddCommand("prefix",		"",			(void*)bc_prefix,CM_ALL);
	AddCommand("pistolonly",	"po",		(void*)bc_pistolonly,CM_ALL);
	AddCommand("botskill_min",	"",			(void*)bc_botskill_min,CM_ALL);
	AddCommand("botskill_max",	"",			(void*)bc_botskill_max,CM_ALL);
	AddCommand("pause",			"p",		(void*)bc_pause,CM_ALL);
	AddCommand("fillserver",	"fs",		(void*)bc_fillserver,CM_ALL);
	AddCommand("joinwhuman_max","jwhm",		(void*)bc_joinwhumanmax,CM_ALL);
	AddCommand("joinwhuman_res","jwhr",		(void*)bc_joinwhumanres,CM_ALL);
	AddCommand("randomwpload",	"rwpl",		(void*)bc_randomwpload,CM_ALL);

	AddCommand("bot_spraying","botspraying",(void*)bc_bot_spraying,CM_ALL);
	AddCommand("bot_spray","botspray",(void*)bc_bot_spraying,CM_ALL);
	AddCommand("botusespray","botsusespray",(void*)bc_bot_spraying,CM_ALL);
#ifdef USE_GNOME
	AddCommand("startgnome","stag",(void*)bc_startgnome,CM_CONSOLE);
	AddCommand("stopgnome","stog",(void*)bc_stopgnome,CM_CONSOLE);
#endif /* USE_GNOME */
	AddCommand("loadbuyprob","lbp",(void*)bc_loadbuyprob,CM_ALL);

	AddCommand("botglow","bg",(void*)bc_botglow,CM_ALL);
}

CCommands ::~CCommands(){
/*#ifndef __BORLANDC__
	CCommand *p,*pp;
	p=LCom;
	while(p){
		pp = p;
		p = (CCommand*)p->next;
	}
	while (pp){
		p = pp;
		pp = (CCommand*)pp->prev;
		delete p;
	}
#endif*/ /* __BORLANDC__ */
}

bool CCommands :: AddCommand(const char *n,const char *n2,void *p,int iT){
	CCommand temp;
	
	temp.iType = iT;
	temp.pFunc = ((bool (*)(edict_t *p,int iType,const char *arg1,const char *arg2,const char *arg3, const char *arg4))(p));
	strcpy(temp.szName,n);
	strcpy(temp.sz2Name,n2);

	m_LCommands.push_back(temp);
	return true;
}

bool CCommands ::Exec(edict_t *pEntity,int iType,const char *cmd,const char *arg1,const char *arg2,const char *arg3,const char *arg4){
	std::list<CCommand>::iterator iter;

	if(!arg1)arg1="";
	if(!arg2)arg2="";
	if(!arg3)arg3="";
	if(!arg4)arg4="";

	//cout << cmd <<" --- "<<arg1<<" --- "<<arg2<<" --- "<<arg3<<" --- "<<arg4<<endl;

	iter = m_LCommands.begin();
	while(iter != m_LCommands.end()){
		//cout << (*iter).szName << endl;
		if((strlen((*iter).szName)
				&&FStrEq((*iter).szName,cmd))
			||(strlen((*iter).sz2Name)
				&&FStrEq((*iter).sz2Name,cmd))){
			if((*iter).iType & iType){
				return (*iter).pFunc(pEntity,iType,arg1,arg2,arg3,arg4);
			}
		}
		iter ++;
	}
	return false;
}
