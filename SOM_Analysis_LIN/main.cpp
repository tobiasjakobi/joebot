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
//#include <windows.h>
#include <stdio.h>
#include <time.h>

#define __TRAIN

#define _RED .99

#include "../SOM_Analysis_Win/size.h"

#include "SOM/SOM.h"
#include "NeuralNet.h"

#include "BotNNDefs.h"

#define _R 2

#define _LOGFILE "log.html"

void main(){
	
	cout << "Initialisiere LSSOMSim und LSNNSI"<<endl;
	qSOM2d		SOM;
	SOMPattern	SOMP(1);
	CPatternBProp NNP;
	
	//int _XSC,_YSC,_ASC;
	char szBuffer[200];
	//POINT pt;
	clock_t start,end,ovstart,ctime;
	long lschl,lCyc=0,lCyM;
	//time_t aclock;
	double dTemp;
	//HGDIOBJ htemp;

	ovstart = clock();
	
	SOM.SetSize(IEND,_XS,_YS);
	
	cout << "Loading combat.spt " << endl;
	SOMP.Load("combat.spt");
	//cout<<IEND<<endl;
	cout << "combat.spt   : " <<SOMP.GetNumP() << " Pattern"<<endl;
	SOMP.SetSOM(&SOM);
	cout << "Loading NNTrain.ptt " <<endl;
	NNP.LoadFile("../NN_Train/nntrain.ptt");
	cout << "NNTrain.ptt  : " << NNP.GetPatternNum() << " Pattern" << endl;
	
	SOM.SetDistP(_XS * .9);
	//SOM.SetNKFunction(fNKcone);
	
	dTemp = SOM.GetDistP();
	for(lschl=0;dTemp>1.0;lschl++)
		dTemp*= _RED;
	printf("Vermutliche Zyklen : %li\n\n",lschl);
	lCyM = lschl;
	
	dTemp = SOM.GetDistP();
	SOM.SetLRate(.1);
	SOM.SetNKFunction(fNKgauss1);
	
	while(1){
		SOM.SetDistP(SOM.GetDistP() * _RED );
		
		start = clock();
		
		if(SOM.GetDistP() > 1.0){
			SOMP.TeachEpoch(SOM.GetLRate(),SOM.GetDistP());
			lCyc ++;
		}
		else
			break;
		
		end = clock();
		
		ctime = clock();
		double dpCyc = double(ctime-ovstart) / double(CLOCKS_PER_SEC)/double(lCyc);
		float EAsec = long(dpCyc * double(lCyM) - dpCyc * double(lCyc));
		long lHour = long(EAsec / 3600.0);
		EAsec -= lHour * 3600;
		long lMin =  long(EAsec / 60.0);
		EAsec -= lMin * 60;
		long lSec = (long)EAsec;
		sprintf(szBuffer,"DI %2.4f\tLR %1.2f\tTE %2.2f\tSP %4.2fk\t%3.li.CY\tET %2li:%2li:%2li",SOM.GetDistP(),SOM.GetLRate(),double(end-start)/double(CLOCKS_PER_SEC),double(_XS*_YS*IEND * SOMP.GetNumP()) / (double(end-start)/double(CLOCKS_PER_SEC)) / 1000.0,lCyM-lCyc,lHour,lMin,lSec);
		cout << szBuffer << endl;
		FILE *fhd;
		if(fhd=fopen("catlog.txt","a")){
			tm *timetm;
			time_t timet = time(NULL);
			timetm = localtime(&timet);
			fprintf(fhd,"%s\t%s",szBuffer,asctime(timetm));
			fclose(fhd);
		}
	}
//	cout << "time elapsed : "<<double(ctime-ovstart) / CLOCKS_PER_SEC<<" secs"<<endl<<SOM.lCat<<endl;
	cout << endl << "saving som to file \"cat.som\" ... ";cout.flush();
	SOM.Save("cat.som");
	cout << " finished" << endl;
}