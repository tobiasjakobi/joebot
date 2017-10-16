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
#include <windows.h>
#include <stdio.h>
#include <time.h>

//#define __TRAIN

#include "SOM/som.h"
#include "NeuralNet.h"

#include "CDataVisu.h"

CDataVisu DATA;

#include "BotNNDefs.h"

#include "Size.h"

#define _BS (_XS>_YS?_XS:_YS)

#define _R 1

#define _LOGFILE "log.html"

#define _LINE
#define _REDRAW true

long _STEP = 20;

int _SHOW = IHealth;

class Brushes
{
public:
	inline Brushes();
	inline ~Brushes();
	
	HBRUSH brush[256];
}colors;

inline Brushes :: Brushes()
{
	for(register int a=0;a<256;a++)
	{
		brush[a]= CreateSolidBrush(RGB(a,a*0,255-a));
	}
}
inline Brushes :: ~Brushes()
{
	for(register int a=0;a<256;a++)
	{
		DeleteObject(brush[a]);
	}
}

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int LOG(const char *szText){
	FILE *fhd;
	if(fhd = fopen(_LOGFILE,"a")){
		fprintf(fhd,"%s",szText);
		fclose(fhd);
		return true;
	}
	else
		return false;
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("SOMA") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;
	
	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (hInstance, "CROSSHAIR") ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;
	
	if (!RegisterClass (&wndclass))
	{    // UNICODE-Compilierung ist die einzige realistische Fehlermöglichkeit 
		MessageBox (NULL, TEXT ("Programm arbeitet mit Unicode und setzt Windows NT voraus!"), 
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}
	
	hwnd = CreateWindow (szAppName,                 // Name der Fensterklasse
		TEXT ("Cat von Daten aus dem Spiel"),  // Fenstertitel
		WS_OVERLAPPEDWINDOW|WS_MAXIMIZE,                // Fensterstil
		CW_USEDEFAULT,                      // X-Position des Fensters
		CW_USEDEFAULT,                      // Y-Position des Fensters
		CW_USEDEFAULT,                      // Fensterbreite
		CW_USEDEFAULT,                      // Fensterhöhe
		NULL,                               // übergeordnetes Fenster
		NULL,                               // Menü
		hInstance,                          // Programm-Kopiezähler (Programm-ID)
		NULL) ;                             // zusätzliche Parameter
	
	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;
	
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
	PAINTSTRUCT ps ;
	//RECT        rect ;
	
	static qSOM2d		SOM;
	static SOMPattern	SOMP(5);
	static CPatternBProp	NNP;

	static long lp = RGB(80,80,80),lx,ly;
	static double _XSC,_YSC,_ASC;
	static char szBuffer[200];
	static long lxw,lyw,lCCount;
	//POINT pt;
	static clock_t start,end;
	static FILE *fhd;
	static long lschl,lWx,lWy,lxPos,lyPos,lMy,lMx,lCyc=0,lCyM;
	//time_t aclock;
	static HPEN blue = CreatePen(PS_SOLID,2,RGB(0,0,0xff));
	static HPEN red = CreatePen(PS_SOLID,2,RGB(0xff,0,0));
	static HPEN white = CreatePen(PS_SOLID,2,RGB(0xff,0xff,0xff));
	static char chCharCode;
	//HGDIOBJ htemp;
	static double dDiff[_XS][_YS];
	static double dDiff2[_XS][_YS];
	static double /*dFTemp[5],*/dNearest,dMaxDiff,dTemp,dAll;
	static nVec VTemp(5),VPattern(5),VAll(5);
	static HDC hdcMem;
	static HBITMAP hBit;

	static float f;

	static bool recalcbitmap = true;
	
	switch (message)
	{
	case WM_CREATE:
		hBit = 0;
		DATA.Init(_XS,_YS);
		SOM.SetSize(IEND,_XS,_YS);
#ifndef __TRAIN
		//SOM.Load("cat.som");
		SOM.Load("../SOM_ANALYSIS_LIN/cat.som");
#endif
		try{
			SOMP.Load("combat.spt");
		}
		catch(...){
			MessageBox(0,"Problems with combat.spt","",MB_OK);
		}
		SOMP.SetSOM(&SOM);
		try{
			NNP.LoadFile("../NN_Train/NNTrain.ptt");
		}
		catch(...){
			MessageBox(0,"Problems with NNtrain.ptt","",MB_OK);
		}
#ifdef __TRAIN
		sprintf(szBuffer,"Pattern from combat.spt : %li",SOMP.GetNumP());
		MessageBox(hwnd,szBuffer,"Hinweis",MB_OK);
#endif
		SOM.SetDistP(_XS * .9);
		dTemp = SOM.GetDistP();
		for(lschl=0;dTemp>1.0;lschl++)
			dTemp*= .99;
		sprintf(szBuffer,"Vermutliche Zyklen : %li",lschl);
		lCyM = lschl;
#ifdef _TRAIN
		MessageBox(hwnd,szBuffer,"Hinweis",MB_OK);
#endif
		SOM.SetLRate(.1);
		SOM.SetNKFunction(fNKgauss1);
#ifndef _TRAIN
		// calc differences to nearest pattern
		for(lx = 0;lx < _XS;lx++){
			for(ly = 0;ly < _YS;ly++){
				dNearest = 10000;
				//memcpy(dFTemp,SOM.GetNWeights(lx,ly),sizeof(double)*5);
				for(lschl=0;lschl<NNP.GetPatternNum();lschl++){
					VTemp.GetVec(SOM.GetNWeights(lx,ly));
					VPattern.GetVec(((CBasePatternElemFF *)(NNP[lschl]))->m_pdIn);
					VTemp -= VPattern;

					dTemp = sqrt(VTemp.SLength());
					if (dTemp < dNearest){
						dNearest = dTemp;
					}
				}
				dDiff[lx][ly] = dNearest;
			}
		}
		// searching for max
		dMaxDiff = 0;
		for(lx = 0;lx < _XS;lx++){
			for(ly = 0;ly < _YS;ly++){
				if(dDiff[lx][ly] > dMaxDiff){
					dMaxDiff = dDiff[lx][ly];
				}
			}
		}

		sprintf(szBuffer,"Max diff : %lf",dMaxDiff);
		MessageBox(hwnd,szBuffer,"Information",MB_ICONINFORMATION|MB_OK);

		// ddiff2
		memset(dDiff2,0,sizeof(double)*_XS*_YS);
		for(lx = 1;lx < _XS-1;lx++){
			for(ly = 1;ly < _YS-1;ly++){
				dAll = 0;
				
				VTemp.GetVec(SOM.GetNWeights(lx,ly));
				VPattern.GetVec(SOM.GetNWeights(lx-1,ly));
				VTemp -= VPattern;
				dAll += VTemp.Length();

				VTemp.GetVec(SOM.GetNWeights(lx,ly));
				VPattern.GetVec(SOM.GetNWeights(lx+1,ly));
				VTemp -= VPattern;
				dAll += VTemp.Length();

				VTemp.GetVec(SOM.GetNWeights(lx,ly));
				VPattern.GetVec(SOM.GetNWeights(lx,ly-1));
				VTemp -= VPattern;
				dAll += VTemp.Length();

				VTemp.GetVec(SOM.GetNWeights(lx,ly));
				VPattern.GetVec(SOM.GetNWeights(lx,ly+1));
				VTemp -= VPattern;
				dAll += VTemp.Length();


				VTemp.GetVec(SOM.GetNWeights(lx,ly));
				VPattern.GetVec(SOM.GetNWeights(lx-1,ly-1));
				VTemp -= VPattern;
				dAll += VTemp.Length();

				VTemp.GetVec(SOM.GetNWeights(lx,ly));
				VPattern.GetVec(SOM.GetNWeights(lx+1,ly+1));
				VTemp -= VPattern;
				dAll += VTemp.Length();

				VTemp.GetVec(SOM.GetNWeights(lx,ly));
				VPattern.GetVec(SOM.GetNWeights(lx+1,ly-1));
				VTemp -= VPattern;
				dAll += VTemp.Length();

				VTemp.GetVec(SOM.GetNWeights(lx,ly));
				VPattern.GetVec(SOM.GetNWeights(lx-1,ly+1));
				VTemp -= VPattern;
				dAll += VTemp.Length();
				
				dDiff2 [lx][ly] = dAll;
			}
		}
		/*// searching for max
		dMaxDiff = 0;
		for(lx = 0;lx < _XS;lx++){
			for(ly = 0;ly < _YS;ly++){
				if(dDiff2[lx][ly] > dMaxDiff){
					dMaxDiff = dDiff2[lx][ly];
				}
			}
		}*/
#endif
		return 0 ;
		
	case WM_SIZE :
		recalcbitmap = true;
		_XSC = LOWORD (lParam);
		_YSC = HIWORD (lParam);
		_XSC<_YSC?_ASC=_XSC:_ASC=_YSC;
		//_ASC -= long(1.0/_XSC * _ASC);
		if(hBit)
			DeleteObject(hBit);
		hdc = GetDC(hwnd);
		hBit = CreateCompatibleBitmap(hdc,_XSC,_YSC);
		ReleaseDC(hwnd,hdc);
		return 0;
		
	case WM_CHAR :
		chCharCode = (TCHAR) wParam;
		switch(chCharCode){
		case 'q':
			_STEP = 2;
			InvalidateRect(hwnd,0,false);
			recalcbitmap = true;
			break;

		case 's':
			_STEP /= 2;
			InvalidateRect(hwnd,0,false);
			recalcbitmap = true;
			break;

		case 'd':
			_STEP *=2;
			InvalidateRect(hwnd,0,false);
			recalcbitmap = true;
			break;

		case 'f':
			_STEP =20;
			InvalidateRect(hwnd,0,false);
			recalcbitmap = true;
			break;

		case 'a':
			_SHOW ++;
			recalcbitmap = true;
			if(_SHOW>=IEND)
				_SHOW=-2;
			InvalidateRect(hwnd,0,false);
			break;
		case 'y':
			_SHOW --;
			if(_SHOW<-2)
				_SHOW=IEND-2;
			InvalidateRect(hwnd,0,false);
			recalcbitmap = true;
			break;
		case ' ':
			InvalidateRect(hwnd,0,true);
			recalcbitmap = true;
			break;
		}
		return 0;
		
#ifndef __TRAIN
		case WM_LBUTTONUP:
			lxPos = LOWORD(lParam);  // horizontal position of cursor 
			lyPos = HIWORD(lParam);  // vertical position of cursor 

			f=(_XSC>_YSC?_XSC/(_XS>_YS?_XS:_YS):_YSC/(_XS>_YS?_XS:_YS));
			
			lMx = float(lxPos) / (f);
			lMy = float(lyPos) / (f);
			
			try{
			sprintf(szBuffer,"(%li,%li)\n%f\nIHealth\t\t:\t%f\n\
IDistance\t\t:\t%f\n\
IEWeapon\t:\t%f\n\
IWeapon\t\t:\t%f\n\
IAmmo\t\t:\t%f\n-------------------------------------------------\n\
Diff\t\t:\t%f\n",
lMx,
lMy,
f,
SOM.GetNWeights(lMx,lMy)[IHealth],
SOM.GetNWeights(lMx,lMy)[IDistance],
SOM.GetNWeights(lMx,lMy)[IEWeapon],
SOM.GetNWeights(lMx,lMy)[IWeapon],
SOM.GetNWeights(lMx,lMy)[IAmmo],
dDiff[lMx][lMy]);
			MessageBox(hwnd,szBuffer,"",MB_OK);
			LOG(szBuffer);
			}
			catch(...){
			}
			return 0;
			
		case WM_MOUSEMOVE:
			lxPos = LOWORD(lParam);  // horizontal position of cursor 
			lyPos = HIWORD(lParam);  // vertical position of cursor 
			/*if(_SHOW>=0){
				hdc = GetDC(hwnd);
				//SetBkMode(hdc,TRANSPARENT);
				if(lMx < _XS){
					if(lMx < _XS){
						SelectObject(hdc,GetStockObject(NULL_BRUSH));
						dTemp = SOM.GetNWeights(lMx,lMy)[_SHOW];
						SelectObject(hdc,colors.brush[int(dTemp*128.0+127.0)]);
						SelectObject(hdc,GetStockObject(NULL_PEN));
						Rectangle(hdc,
							_ASC / _XS * lMx -1 ,_ASC / _YS * lMy -1,
							_ASC / _XS * lMx + _ASC / _XS ,_ASC / _YS * lMy + _ASC / _YS );
					}
				}
				
				lMx = lxPos / (_ASC / _XS);
				lMy = lyPos / (_ASC / _YS);
				
				if(lMx >= _XS)
					return 0;
				if(lMy >= _YS)
					return 0;
				
				SelectObject(hdc,blue);
				Ellipse(hdc,
					_ASC / _XS * lMx +2*_R ,_ASC / _YS * lMy+2*_R ,
					_ASC / _XS * lMx + _ASC / _XS -2*_R,_ASC / _YS * lMy + _ASC / _YS -2*_R);
				ReleaseDC(hwnd,hdc);
			}*/
			return 0;
#endif
		case WM_PAINT:
			hdc = BeginPaint (hwnd, &ps) ;		
			hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem,hBit);
			if(recalcbitmap){
				recalcbitmap = false;
				SelectObject(hdc,GetStockObject(BLACK_PEN));
				SelectObject(hdc,GetStockObject(BLACK_BRUSH));
				Rectangle(hdcMem,0,0,_XSC,_YSC);
				start = clock();
#ifdef __TRAIN
				SOM.SetDistP(SOM.GetDistP() * .99 );
				//SOM.SetLRate(SOM.GetLRate() * .98 );
				if(SOM.GetDistP() > 1.0){
					SOMP.TeachEpoch(SOM.GetLRate(),SOM.GetDistP());
					lCyc ++;
				}
#endif
				end = clock();
				
				// updating title bar
				sprintf(szBuffer,"DI %.4f LR %.2f TE %.2f SP %.2fk - %li.CY - showing %i",SOM.GetDistP(),SOM.GetLRate(),double(end-start)/double(CLOCKS_PER_SEC),double(_XS*_YS*IEND * SOMP.GetNumP()) / (double(end-start)/double(CLOCKS_PER_SEC)) / 1000.0,lCyM-lCyc,_SHOW);
				SetWindowText(hwnd,szBuffer);
				if(_SHOW>=0){
					// displaying examples on SOM
					// show grid
					for(lx = 0;lx < _XS;lx++){
						for(ly = 0;ly < _YS;ly++){
							dTemp = SOM.GetNWeights(lx,ly)[_SHOW];
							DATA.Set(lx,ly,dTemp);
							/*dTemp = SOM.GetNWeights(lx,ly)[_SHOW];
							SelectObject(hdc,colors.brush[int(dTemp*128.0+127.0)]);
							SelectObject(hdc,GetStockObject(NULL_PEN));
							Rectangle(hdc,
							_ASC / _XS * lx -1 ,_ASC / _YS * ly -1,
							_ASC / _XS * lx + _ASC / _XS ,_ASC / _YS * ly + _ASC / _YS );*/
						}
					}
					// calc where the patterns are
					//SelectObject(hdc,red);
					DATA.Draw(hdcMem,hdc,(_XSC>_YSC?_XSC:_YSC)/_BS*_XS,(_XSC>_YSC?_XSC:_YSC)/_BS*_YS,(_XSC>_YSC?_XSC:_YSC)/_BS,0,0,1,_STEP);
					//DATA.Draw(hdc,10*_XS,10*_YS,600.0,0,0,2,10);
					SelectObject(hdc,GetStockObject(BLACK_PEN));
					SelectObject(hdc,GetStockObject(BLACK_BRUSH));
					/*for(lschl=NNP.GetAnz();lschl > 0;lschl--){
					SOM.SetInput(NNP[lschl]->Input());
					SOM.GetWinner(lWx,lWy);		// get place of winner neuron, when having this pattern as input
					Ellipse(hdc,
					_ASC / _XS * lWx+_R,_ASC / _YS * lWy+_R,
					_ASC / _XS * lWx + _ASC / _XS -_R,_ASC / _YS * lWy + _ASC / _YS -_R);
				}*/
				}else{
					if (_SHOW == -1){
						for(lx = 0;lx < _XS;lx++){
							for(ly = 0;ly < _YS;ly++){
								dTemp = dDiff[lx][ly];
								DATA.Set(lx,ly,dTemp);
								/*SelectObject(hdc,colors.brush[int(dTemp/dMaxDiff*255.0)]);
								SelectObject(hdc,GetStockObject(NULL_PEN));
								Rectangle(hdc,
								_ASC / _XS * lx -1 ,_ASC / _YS * ly -1,
								_ASC / _XS * lx + _ASC / _XS ,_ASC / _YS * ly + _ASC / _YS );*/
							}
						}
						DATA.Draw(hdcMem,hdc,(_XSC>_YSC?_XSC:_YSC)/_BS*_XS,(_XSC>_YSC?_XSC:_YSC)/_BS*_YS,(_XSC>_YSC?_XSC:_YSC)/_BS,0,0,2,_STEP);
						// calc where the patterns are
						/*SelectObject(hdc,GetStockObject(BLACK_PEN));
						SelectObject(hdc,GetStockObject(BLACK_BRUSH));
						for(lschl=NNP.GetAnz();lschl > 0;lschl--){
						SOM.SetInput(NNP[lschl]->Input());
						SOM.GetWinner(lWx,lWy);		// get place of winner neuron, when having this pattern as input
						Ellipse(hdc,
						_ASC / _XS * lWx+_R,_ASC / _YS * lWy+_R,
						_ASC / _XS * lWx + _ASC / _XS -_R,_ASC / _YS * lWy + _ASC / _YS -_R);
					}*/
					}
					else if (_SHOW == -2){
						for(lx = 0;lx < _XS;lx++){
							for(ly = 0;ly < _YS;ly++){
								dTemp = dDiff2[lx][ly];
								DATA.Set(lx,ly,dTemp);
								/*SelectObject(hdc,colors.brush[int(dTemp/dMaxDiff*255.0)]);
								SelectObject(hdc,GetStockObject(NULL_PEN));
								Rectangle(hdc,
								_ASC / _XS * lx -1 ,_ASC / _YS * ly -1,
								_ASC / _XS * lx + _ASC / _XS ,_ASC / _YS * ly + _ASC / _YS );*/
							}
						}
						DATA.Draw(hdcMem,hdc,(_XSC>_YSC?_XSC:_YSC)/_BS*_XS,(_XSC>_YSC?_XSC:_YSC)/_BS*_YS,(_XSC>_YSC?_XSC:_YSC)/_BS,0,0,2,_STEP);
						// calc where the patterns are
						/*SelectObject(hdc,GetStockObject(BLACK_PEN));
						SelectObject(hdc,GetStockObject(BLACK_BRUSH));
						for(lschl=NNP.GetAnz();lschl > 0;lschl--){
						SOM.SetInput(NNP[lschl]->Input());
						SOM.GetWinner(lWx,lWy);		// get place of winner neuron, when having this pattern as input
						Ellipse(hdc,
						_ASC / _XS * lWx+_R,_ASC / _YS * lWy+_R,
						_ASC / _XS * lWx + _ASC / _XS -_R,_ASC / _YS * lWy + _ASC / _YS -_R);
					}*/
					}
				}
			}
			BitBlt(hdc,0,0,_XSC,_YSC,hdcMem,0,0,SRCCOPY);
			
			DeleteDC(hdcMem);
			EndPaint (hwnd, &ps) ;
#ifdef __TRAIN
			if(SOM.GetDistP() > 1.0)
				InvalidateRect(hwnd,0,false);
#endif
			return 0 ;
			
		case WM_DESTROY:
			//fclose(fhd);
#ifdef __TRAIN
			SOM.Save("cat.som");
#endif
			PostQuitMessage (0) ;
			break;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
