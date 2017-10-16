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
#ifndef __CDATAVISU_H
#define __CDATAVISU_H

#include "windows.h"
#include "CData.h"
#include "math.h"

class CDataVisu : public CData{
public:
	CDataVisu();
	~CDataVisu();

	void Draw(HDC,HDC,int,int,double,double,double,double,int);
protected:
private:
	double fBorderMin,fBorderMax;
	long lRGBMin,lRGBMax;
	int iXBitmap,iYBitmap;
	double fScale;
	double fXOffset,fYOffset;
	double fGGKS;
};

#endif  __CDATAVISU_H