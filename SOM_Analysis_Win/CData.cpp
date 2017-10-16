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
#include "CData.h"

CData :: CData(){
	iDim = 0;
	iSX=0;
	iSY=0;
	fData = 0;
}

CData :: ~CData(){
	if(fData){
		delete [] fData;
		fData = 0;
	}
}

void CData :: Init(int ixS,int iyS){
	iSX = ixS;
	iSY = iyS;

	fData = new double [iSX * iSY];
}

bool CData :: Set(int iX, int iY, double fSet){
	fData[iX*iSY+iY] = fSet;
	return true;
}

double CData :: Get(int iX,int iY){
	if(iX>=iSX)
		return 0.0;
	if(iY>=iSY)
		return 0.0;
	return fData[iX*iSY+iY];
}