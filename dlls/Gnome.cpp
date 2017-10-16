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
// Gnome.cpp: implementation of the CGnome class.
//
//////////////////////////////////////////////////////////////////////

#include "extdll.h"
#include "util.h"

#include "Gnome.h"

#include "globalvars.h"
#include "waypoint.h"

#define _GNOMESPEED 20
#define _MAXGNOMES 1000
#define _AVOIDLENGTH 50
#define _NAVOIDLENGTH 80
#define _CANGLEDIFF 45
long lGnomes = 0;
long lGnomesTemp = 0;

void *FG[100];
long lGN = 0;

void Garbage(void *p){
	if(lGN > 98){
		for(long lschl = 0;lschl < lGN;lschl++){
			//delete (CGnome*)FG[lschl];
		}
		lGN = 0;
	}
	FG[lGN] = p;
	lGN++;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGnome::CGnome()
{
	lGnomes ++;
	lFrame = 0;
	iLWP = -1;
	lEnergy = 3;
}

CGnome::~CGnome()
{
//	CLListElem::~CLListElem();
	lGnomes --;
}

CLListElem *CGnome ::  AllocNewItem(void){
	if(lGnomes < _MAXGNOMES)
		return new CGnome;
	else
		return 0;
}

void CGnome :: Think(void){
	lFrame++;
	lGnomesTemp ++;

	Move();
	Autowaypoint();
	AvoidWall();
	CheckLife();
	CheckInteresting();

	if(next)
		((CGnome*)(next))->Think();
}

void CGnome :: AvoidWall(void){
	Vector /*v_middle,*/v_left,v_right,v_lookav;
	TraceResult trm,trl,trr;

	v_lookav = v_look;
	/*UTIL_MakeVectors(v_lookav);
	v_middle = gpGlobals->v_forward;*/
	v_lookav.y -= _CANGLEDIFF;
	UTIL_MakeVectors(v_lookav);
	v_left = gpGlobals->v_forward;
	v_lookav = v_look;
	v_lookav.y += _CANGLEDIFF;
	UTIL_MakeVectors(v_lookav);
	v_right = gpGlobals->v_forward;

	//v_middle = v_origin + v_middle * _AVOIDLENGTH;
	v_right = v_origin + v_right * _AVOIDLENGTH;
	v_left = v_origin + v_left * _AVOIDLENGTH;

	//UTIL_TraceLine(v_origin,v_middle,ignore_monsters,ignore_glass,0,&trm);
	UTIL_TraceLine(v_origin,v_right,ignore_monsters,ignore_glass,0,&trr);
	UTIL_TraceLine(v_origin,v_left,ignore_monsters,ignore_glass,0,&trl);

	if(trl.flFraction > trr.flFraction){
		v_look.y -= 5;
	}
	else if(trl.flFraction < trr.flFraction){
		v_look.y += 5;
	}
	/////////////////////////////////////////////////////////////////////////
	if(trl.flFraction == 1.0f
		&&trr.flFraction == 1.0f){ // turn to wall
		v_lookav = v_look;
		/*UTIL_MakeVectors(v_lookav);
		v_middle = gpGlobals->v_forward;*/
		v_lookav.y -= _CANGLEDIFF;
		UTIL_MakeVectors(v_lookav);
		v_left = gpGlobals->v_forward;
		v_lookav = v_look;
		v_lookav.y += _CANGLEDIFF;
		UTIL_MakeVectors(v_lookav);
		v_right = gpGlobals->v_forward;
		
		//v_middle = v_origin + v_middle * _NAVOIDLENGTH;
		v_right = v_origin + v_right * _NAVOIDLENGTH;
		v_left = v_origin + v_left * _NAVOIDLENGTH;
		
		//UTIL_TraceLine(v_origin,v_middle,ignore_monsters,ignore_glass,0,&trm);
		UTIL_TraceLine(v_origin,v_right,ignore_monsters,ignore_glass,0,&trr);
		UTIL_TraceLine(v_origin,v_left,ignore_monsters,ignore_glass,0,&trl);
		
		if(trl.flFraction > trr.flFraction){
			v_look.y += 4;
		}
		else if(trl.flFraction < trr.flFraction){
			v_look.y -= 4;
		}
	}
}

void CGnome :: CheckLife(void){
	if(float(lFrame / 10) == float(lFrame) / 10.0 ){
		if((v_historicorigin-v_origin).Length() < 50){
			Unlink();
			Garbage(this);
			return;
		}

		v_historicorigin = v_origin;
	}
	if(1){
		CGnome *p = this;
		// up ...
		while(p = (CGnome*)p->next){
			if((v_origin - p->v_origin).Length() < 100.f){
				if(lFrame > 3)
					lEnergy -=1;
			}
		}
		// ... and down
		p = this;
		while(p = (CGnome*)p->prev){
			if(p->prev)
			if((v_origin - p->v_origin).Length() < 100.f){
				if(lFrame > 3)
					lEnergy -=1;
			}
		}

		int iwp = WaypointFindNearest(v_origin,0,20,-1,false);
		if(iwp != -1 && iwp != iLWP){
			if(lFrame > 100/_GNOMESPEED)
			{
				lEnergy -=1;
			}
			return;
		}
		if(lEnergy < 0
			|| v_move.Length2D() < .1){
			Unlink();
			Garbage(this);
			return;
		}
	}
}

void CGnome :: CheckInteresting(void){
}

void CGnome :: Autowaypoint(void){
	if(WaypointFindNearest(v_origin,0,150,-1,false) == -1){
		iLWP = WaypointAdd(v_origin,0,true,true);
		lEnergy +=5;
	}
}

void CGnome :: Move(void){
	//WaypointDrawBeam(pEdictPlayer,v_origin,pEdictPlayer->v.origin,1,1,250,250,250,200,1);

	Vector v_neworigin;
	TraceResult tr;
	bool bIgnored = false;

	UTIL_MakeVectors(v_look);

	v_move = gpGlobals->v_forward;

	v_move.Normalize();			// normalize direction

	v_neworigin = v_origin + v_move * _GNOMESPEED;

	UTIL_TraceLine(v_origin,v_neworigin,ignore_monsters,ignore_glass,pEdictPlayer,&tr);

	if(tr.flFraction == 1.0){
		//WaypointDrawBeam(pEdictPlayer,v_origin,v_neworigin,10,10,250,250,250,200,10);
		v_origin = v_neworigin;
	}
	else{
		if(tr.pHit){
			// check it it's a door
			char szDoor[] = "func_door";
			
			if(!strncmp(STRING(tr.pHit->v.classname),szDoor,strlen(szDoor)*sizeof(char))){
				// it's a door
				v_origin = tr.vecEndPos+v_move*.01;
				Move();
				bIgnored = true;
			}
		}
		if(bIgnored){
		}
		else{
			CGnome *p;
			
			p = (CGnome*)NewItem();
			if(p){
				Vector VTemp;

				p->v_origin = v_origin;
				p->v_look = v_look;
				p->v_look.y += 90;

				UTIL_MakeVectors(p->v_look);
				p->v_move = gpGlobals->v_forward;
				VTemp = p->v_origin + p->v_move * _GNOMESPEED;
				UTIL_TraceLine(p->v_origin,VTemp,ignore_monsters,ignore_glass,0,&tr);

				if(tr.flFraction != 1.0)
					delete p;

				p = (CGnome*)NewItem();
				if(p){
					p->v_origin = v_origin;
					p->v_look = v_look;
					p->v_look.y -= 90;
					
					UTIL_MakeVectors(p->v_look);
					p->v_move = gpGlobals->v_forward;
					VTemp = p->v_origin + p->v_move * _GNOMESPEED;
					UTIL_TraceLine(p->v_origin,VTemp,ignore_monsters,ignore_glass,0,&tr);
					
					if(tr.flFraction != 1.0)
						delete p;
				}
			}
			if(p){
				Unlink();
				Garbage(this);
				return;
			}
			else{
				v_look.y += 30;
			}

			return;
		}
	}

	// check for ground

	UTIL_TraceLine(v_origin,v_origin-Vector(0,0,300),ignore_monsters,ignore_glass,pEdictPlayer,&tr);

	if(tr.flFraction == 1.0f){
		Unlink();
		Garbage(this);
		return;
	}
	v_origin = tr.vecEndPos;
	UTIL_TraceLine(v_origin,v_origin+Vector(0,0,75),ignore_monsters,ignore_glass,pEdictPlayer,&tr);
	if(tr.flFraction == 1.0f){
		bCrouched = false;
	}
	else
		bCrouched = true;

	if(bCrouched)
		v_origin = v_origin + Vector(0,0,17);
	else
		v_origin = v_origin + Vector(0,0,35);

	WaypointDrawSpark(pEdictPlayer,v_origin);
}
