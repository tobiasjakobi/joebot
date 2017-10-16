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
#include "CDataVisu.h"

#define _DIS 5

inline double q(const double f){
	return f*f;
}

CDataVisu :: CDataVisu(){
	lRGBMin = RGB(0,0,0);
	lRGBMax = RGB(255,255,255);
	fBorderMin = 0;
	fBorderMax = 10;
}

CDataVisu :: ~CDataVisu(){
}

void CDataVisu :: Draw(HDC hdc,HDC hdclog,int iBMXS,int iBMYS,double fPScale,double fPXOffset,double fPYOffset, double fPGGKS,int iStep){
	iXBitmap = iBMXS;
	iYBitmap = iBMYS;
	fScale = fPScale;
	fXOffset = fPXOffset;
	fYOffset = fPYOffset;
	fGGKS = fPGGKS*fScale;

	int ixschl,iyschl;
	double ifxschl,ifyschl;
	int ixdschl,iydschl;
	double ifxdschl,ifydschl;
	double fDistance;
	double fThis = 0;
// 	int ipx,ipy;
	double *fPThis;
	double dd;
	double fGGKSQ,rfGGKSQ;
	double dDISmfScale = _DIS * fScale;
	fPThis = new double[iXBitmap*iYBitmap];
	fGGKSQ = fGGKS*fGGKS;
	rfGGKSQ = 1.0/fGGKSQ;
	
	for(ixschl=0;ixschl < iXBitmap;ixschl+=iStep){
		ifxschl = double(ixschl);
		SetPixel(hdclog,ixschl,10,0x0000ff00);
		for(iyschl=0;iyschl < iYBitmap;iyschl+=iStep){
			ifyschl = double(iyschl);
			fThis = 0.0;
			
			for(ixdschl=0;ixdschl < iSX;ixdschl++){
				ifxdschl = double(ixdschl)*fScale;
				
				if(fabs(ifxdschl-ifxschl) < dDISmfScale)
					for(iydschl=0;iydschl < iSY;iydschl++){
						ifydschl = double(iydschl)*fScale;
						if(fabs(ifydschl-ifyschl) < dDISmfScale){
							fDistance = 
								q(ifxdschl-ifxschl)
								+q(ifydschl-ifyschl);
							
							dd = fDistance*rfGGKSQ;
							fThis += Get(ixdschl,iydschl) *exp(- dd);
						}
						/*if(fabs(ifydschl-ifyschl) < _DIS * fScale){
							fDistance = 
								sqrt(q(ifxdschl-ifxschl)
								+q(ifydschl-ifyschl));
							
							dd = fDistance/fGGKS;
							fThis += Get(ixdschl,iydschl) *exp(- q(dd));
						}*/
					}
			}
			fPThis[iYBitmap*ixschl+iyschl] = fThis;
		}
	}
	double fMax = -100000;
	double fMin = 100000;
	double fTemp;
	for(ixschl=0;ixschl < iXBitmap;ixschl+=iStep){
		SetPixel(hdclog,ixschl,20,RGB(0xff,0xff,0xff));
		for(iyschl=0;iyschl < iYBitmap;iyschl+=iStep){
			fTemp = fPThis[iYBitmap*ixschl+iyschl];
			if(fTemp > fMax){
				fMax = fTemp;
			}
			if(fTemp < fMin){
				fMin = fTemp;
			}
		}
	}
	HBRUSH temp;
	HPEN tempp;
	double r,g,b;
	if(iStep != 1){
		for(ixschl=0;ixschl < iXBitmap;ixschl+=iStep){
			SetPixel(hdclog,ixschl,30,RGB(0xff,0xff,0xff));
			for(iyschl=0;iyschl < iYBitmap;iyschl+=iStep){
				r = ((fPThis[iYBitmap*ixschl+iyschl]-fMin)/(fMax-fMin))*511.0;
				g = 0;
				b = 255.0-((fPThis[iYBitmap*ixschl+iyschl]-fMin)/(fMax-fMin))*511.0;
				r-= 256.0;
				b<0.0?b=0.0:0==0;
				r<0.0?r=0.0:0==0;
				long lrgb = RGB(
					r,
					g,
					b);
				//SetPixel(hdc,ipx,ipy,lrgb);
				temp = CreateSolidBrush(lrgb);
				tempp = CreatePen(0,0,lrgb);
				SelectObject(hdc,temp);
				SelectObject(hdc,tempp);
				Rectangle(hdc,ixschl,iyschl,ixschl+iStep,iyschl+iStep);
				DeleteObject(temp);
				DeleteObject(tempp);
			}
		}
	}
	else{
		for(ixschl=0;ixschl < iXBitmap;ixschl+=iStep){
			SetPixel(hdclog,ixschl,30,RGB(0xff,0xff,0xff));
			for(iyschl=0;iyschl < iYBitmap;iyschl+=iStep){
				r = ((fPThis[iYBitmap*ixschl+iyschl]-fMin)/(fMax-fMin))*511.0;
				g = 0;
				b = 255.0-((fPThis[iYBitmap*ixschl+iyschl]-fMin)/(fMax-fMin))*511.0;
				r-= 256.0;
				b<0.0?b=0.0:0==0;
				r<0.0?r=0.0:0==0;
				long lrgb = RGB(
					r,
					g,
					b);
				
				SetPixel(hdc,ixschl,iyschl,lrgb);
				//temp = CreateSolidBrush(lrgb);
				//tempp = CreatePen(0,0,lrgb);
				//SelectObject(hdc,temp);
				//SelectObject(hdc,tempp);
				//Rectangle(hdc,ixschl,iyschl,ixschl+iStep,iyschl+iStep);
				//DeleteObject(temp);
				//DeleteObject(tempp);
			}
		}
	}
	delete [] fPThis;
}