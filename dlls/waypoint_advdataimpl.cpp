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
#ifdef _WIN32
#include <io.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>

#include "extdll.h"
#include "util.h"

#include "bot.h"
#include "bot_wpstat.h"
#include "waypoint.h"

adv_wprecmove::adv_wprecmove(){
	iIndexEndWP = -1;
	iIndexStartWP = -1;
	iNum = 0;
}

adv_wprecmove::~adv_wprecmove(){
	iIndexEndWP = -1;
	iIndexStartWP = -1;
}

adv_recitem::adv_recitem(){
	fTime = -1;
	lButton = 0;
	v_velocity = Vector(0,0,0);
	v_angles = Vector(0,0,0);
	v_v_angle = Vector(0,0,0);
	v_origin = Vector(0,0,0);
}

adv_recitem::~adv_recitem(){
}
