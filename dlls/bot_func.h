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
// bot_func.h
//

#ifndef BOT_FUNC_H
#define BOT_FUNC_H

void UpdateLanguage(void);
void TrainNN(edict_t *pEntity);
void KickBots(edict_t *pEntity,int iTeam,int iAll);
void FillServer(int iType, int iTypeAdd);
void BotCreate( edict_t *pPlayer, const char *, const char *,const char *,const char *);
void Endround(void);
void BotServerCommand(void);

#endif //BOT_FUNC_H
