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
#include "dialog.h"

#include "NeuralNet.h"
CPatternBProp Pattern;
CNeuralNetBProp NNCombat;


#define UM_LISTINIT 1
#define _HIDDEN 10
#define _HIDDEN1 10

static HWND glhDlg;

double dMaxError = .2,dError,dInit=.3;
long lMaxLoops=10,lMaxTries=3000,lloop,lEpoch;
long start,end;
bool bflag,bCalc=true;
float ftime;

#define __PATTERNLOC "../NN_Train/NNTrain.ptt"
//#define __PATTERNLOC "NNTrain.ptt"

void EditPrintf (HWND hwndEdit, TCHAR * szFormat, ...)
{
	TCHAR   szBuffer [1024] ;
	va_list pArgList ;
	
	va_start (pArgList, szFormat) ;
	wvsprintf (szBuffer, szFormat, pArgList) ;
	va_end (pArgList) ;
	SendMessage (hwndEdit, EM_SETSEL, (WPARAM) -1, (LPARAM) -1) ;
	SendMessage (hwndEdit, EM_REPLACESEL, FALSE, (LPARAM) szBuffer) ;
	SendMessage (hwndEdit, EM_SCROLLCARET, 0, 0) ;
}

VOID TrainNN (PVOID pvoid)
{
	char szBuffer[200];
	long lschl;
	FILE *fhd;

	switch(1){
	case 1:
		// disable buttons ...
		EnableWindow(GetDlgItem(glhDlg,IDC_RESETNN),FALSE);
		EnableWindow(GetDlgItem(glhDlg,IDC_LOADNN),FALSE);
		EnableWindow(GetDlgItem(glhDlg,IDC_TRAINNN),FALSE);
		EnableWindow(GetDlgItem(glhDlg,IDC_ADD),FALSE);
		EnableWindow(GetDlgItem(glhDlg,IDC_DELETE),FALSE);
		EnableWindow(GetDlgItem(glhDlg,IDC_LOAD),FALSE);
		EnableWindow(GetDlgItem(glhDlg,IDC_MAXTRIES),FALSE);
		EnableWindow(GetDlgItem(glhDlg,IDC_RESETS),FALSE);
		EnableWindow(GetDlgItem(glhDlg,IDC_MAXERROR),FALSE);

		EnableWindow(GetDlgItem(glhDlg,IDC_CANCEL),TRUE);
		// collect inputs ...
		EditPrintf(GetDlgItem(glhDlg,IDC_LOG),"Starting training of NN ... please wait\r\n");
		GetDlgItemText(glhDlg,IDC_MAXTRIES,szBuffer,100);
		lMaxTries = atol(szBuffer);
		if(lMaxTries == 0){
			if(IDCANCEL==MessageBox(glhDlg,"MAXTRIES shouldn't be NULL ... Now set to 3000","Information",MB_OKCANCEL|MB_ICONINFORMATION))
				break;
			lMaxTries=3000;
		}
		GetDlgItemText(glhDlg,IDC_RESETS,szBuffer,100);
		lMaxLoops = atol(szBuffer);
		if(lMaxLoops == 0){
			if(IDCANCEL==MessageBox(glhDlg,"Resets shouldn't be NULL ... Now set to 5","Information",MB_OKCANCEL|MB_ICONINFORMATION))
				break;
			lMaxLoops=5;
		}
		GetDlgItemText(glhDlg,IDC_MAXERROR,szBuffer,100);
		dMaxError = atof(szBuffer);
		if(dMaxError == 0.0f){
			if(IDCANCEL==MessageBox(glhDlg,"MaxError shouldn't be NULL ... Now set to .2","Information",MB_OKCANCEL|MB_ICONINFORMATION))
				break;
		}
		
		//Pattern.LoadText("NNTrain.pta");
		fhd = fopen("learnlog.html","w");
		Pattern.LogErrors(true,2);
		Pattern.Log2File(fhd);
		//Pattern.StartLog(fhd);
		//Pattern.EnableHTMLLog(true);
		//fprintf(fhd,"<html><body><table>");
		
		lloop=0;
		bflag=true;
		lEpoch=0;
		
		while(bflag){
			lloop++;
			start = clock();
			Pattern.LogErrors(true);
			for(lschl=0;lschl < lMaxTries;lschl++){
				if(!bCalc)
					break;
				//fprintf(fhd,"<td>%li",lschl);
				Pattern.TeachEpoch();
				lEpoch++;
				sprintf(szBuffer,"%li",lEpoch);
				SetDlgItemText(glhDlg,IDC_TPROGRESS,szBuffer);
				dError = Pattern.m_dMaxError;
				if(dError < dMaxError)
					break;
				//cout << dError;
			}
			if(bCalc){
			end = clock();
			ftime=float(end-start) / CLOCKS_PER_SEC;
			sprintf(szBuffer,"%.2f",(NNCombat.GetWeightNum())*float(lschl)*float(Pattern.GetPatternNum())/ftime/1000000.0f);
			EditPrintf(GetDlgItem(glhDlg,IDC_LOG),"Speed : %s MCUPS\r\n",szBuffer);
			if(lschl >= lMaxTries && lloop < lMaxLoops){
				lEpoch = 0;
				NNCombat.InitWeights(-.3,.3);
				EditPrintf(GetDlgItem(glhDlg,IDC_LOG),"Error was too big - reseting nn and trying again\r\n");
			}
			else
				bflag = false;
			}
			else{
				bflag = false;
				EditPrintf(GetDlgItem(glhDlg,IDC_LOG),"Operation aborded by user\r\n");
			}
		}
		fclose(fhd);
		if(bCalc){
			if(lloop ==lMaxLoops){
				MessageBox(glhDlg,"NN couldn't be trained to the given maximum error","Information",MB_OK|MB_ICONINFORMATION);
			}
			EditPrintf(GetDlgItem(glhDlg,IDC_LOG),"Saving NN ... \r\n");
			NNCombat.SaveFile("nn.br3");
			EditPrintf(GetDlgItem(glhDlg,IDC_LOG),"Finished training\r\n");
		}
		break;
	}
	// reenable buttons ...
	EnableWindow(GetDlgItem(glhDlg,IDC_LOADNN),TRUE);
	EnableWindow(GetDlgItem(glhDlg,IDC_RESETNN),TRUE);
	EnableWindow(GetDlgItem(glhDlg,IDC_TRAINNN),TRUE);
	EnableWindow(GetDlgItem(glhDlg,IDC_ADD),TRUE);
	EnableWindow(GetDlgItem(glhDlg,IDC_DELETE),TRUE);
	EnableWindow(GetDlgItem(glhDlg,IDC_LOAD),TRUE);
	EnableWindow(GetDlgItem(glhDlg,IDC_MAXTRIES),TRUE);
	EnableWindow(GetDlgItem(glhDlg,IDC_RESETS),TRUE);
	EnableWindow(GetDlgItem(glhDlg,IDC_MAXERROR),TRUE);

	EnableWindow(GetDlgItem(glhDlg,IDC_CANCEL),FALSE);
	// return
	_endthread();
}



BOOL CALLBACK InfoDlgprog (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{	
	switch (iMsg)
	{
	case WM_INITDIALOG :
		return false;
		
	case WM_CLOSE:
		EndDialog(hDlg,0);
		return 0;
		
	case WM_COMMAND :
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg,0);
			break;
		}
		break ;
		
	}
	return FALSE ;
}

BOOL CALLBACK Dlgprog (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//static HINSTANCE	hInstance;
	char szBuffer[200];
	long lschl,lCSel;
	static long lSelNum;
	double dTemp;
	static double defaultin[10];
	static double dNNIn[10],dNNOut[10];
	
	switch (iMsg)
	{
	case WM_INITDIALOG :
		//hInstance=((LPCREATESTRUCT)lParam)->hInstance;
		memset(defaultin,0,sizeof(double) * 10);
		
		SendDlgItemMessage(hDlg,IDC_CDISTANCE,CB_ADDSTRING,0,(LPARAM)"near");
		SendDlgItemMessage(hDlg,IDC_CDISTANCE,CB_ADDSTRING,0,(LPARAM)"middle");
		SendDlgItemMessage(hDlg,IDC_CDISTANCE,CB_ADDSTRING,0,(LPARAM)"far");
		
		SendDlgItemMessage(hDlg,IDC_CHEALTH,CB_ADDSTRING,0,(LPARAM)"low");
		SendDlgItemMessage(hDlg,IDC_CHEALTH,CB_ADDSTRING,0,(LPARAM)"medium");
		SendDlgItemMessage(hDlg,IDC_CHEALTH,CB_ADDSTRING,0,(LPARAM)"high");
		
		SendDlgItemMessage(hDlg,IDC_CEWEAPON,CB_ADDSTRING,0,(LPARAM)"short");
		SendDlgItemMessage(hDlg,IDC_CEWEAPON,CB_ADDSTRING,0,(LPARAM)"medium");
		SendDlgItemMessage(hDlg,IDC_CEWEAPON,CB_ADDSTRING,0,(LPARAM)"long");
		
		SendDlgItemMessage(hDlg,IDC_CAMMO,CB_ADDSTRING,0,(LPARAM)"low");
		SendDlgItemMessage(hDlg,IDC_CAMMO,CB_ADDSTRING,0,(LPARAM)"medium");
		SendDlgItemMessage(hDlg,IDC_CAMMO,CB_ADDSTRING,0,(LPARAM)"high");

		SendDlgItemMessage(hDlg,IDC_CSITUATION,CB_ADDSTRING,0,(LPARAM)"bad");
		SendDlgItemMessage(hDlg,IDC_CSITUATION,CB_ADDSTRING,0,(LPARAM)"normal");
		SendDlgItemMessage(hDlg,IDC_CSITUATION,CB_ADDSTRING,0,(LPARAM)"good");
		
		SendDlgItemMessage(hDlg,IDC_CWEAPON,CB_ADDSTRING,0,(LPARAM)"short");
		SendDlgItemMessage(hDlg,IDC_CWEAPON,CB_ADDSTRING,0,(LPARAM)"medium");
		SendDlgItemMessage(hDlg,IDC_CWEAPON,CB_ADDSTRING,0,(LPARAM)"long");
		
		SendDlgItemMessage(hDlg,IDC_CSTRAFE,CB_ADDSTRING,0,(LPARAM)"left");
		SendDlgItemMessage(hDlg,IDC_CSTRAFE,CB_ADDSTRING,0,(LPARAM)"none");
		SendDlgItemMessage(hDlg,IDC_CSTRAFE,CB_ADDSTRING,0,(LPARAM)"right");
		
		SendDlgItemMessage(hDlg,IDC_CTACTICS,CB_ADDSTRING,0,(LPARAM)"+none");
		SendDlgItemMessage(hDlg,IDC_CTACTICS,CB_ADDSTRING,0,(LPARAM)"none");
		SendDlgItemMessage(hDlg,IDC_CTACTICS,CB_ADDSTRING,0,(LPARAM)"hide");
		
		SendDlgItemMessage(hDlg,IDC_CMOVETYPE,CB_ADDSTRING,0,(LPARAM)"ignored");
		SendDlgItemMessage(hDlg,IDC_CMOVETYPE,CB_ADDSTRING,0,(LPARAM)"walk");
		SendDlgItemMessage(hDlg,IDC_CMOVETYPE,CB_ADDSTRING,0,(LPARAM)"run");
		
		SendDlgItemMessage(hDlg,IDC_CCROUCH,CB_ADDSTRING,0,(LPARAM)"+none");
		SendDlgItemMessage(hDlg,IDC_CCROUCH,CB_ADDSTRING,0,(LPARAM)"none");
		SendDlgItemMessage(hDlg,IDC_CCROUCH,CB_ADDSTRING,0,(LPARAM)"crouch");
		
		SendDlgItemMessage(hDlg,IDC_CJUMP,CB_ADDSTRING,0,(LPARAM)"+none");
		SendDlgItemMessage(hDlg,IDC_CJUMP,CB_ADDSTRING,0,(LPARAM)"none");
		SendDlgItemMessage(hDlg,IDC_CJUMP,CB_ADDSTRING,0,(LPARAM)"jump");
		
		sprintf(szBuffer,"%li",lMaxLoops);
		SetDlgItemText(hDlg,IDC_RESETS,szBuffer);
		
		sprintf(szBuffer,"%li",lMaxTries);
		SetDlgItemText(hDlg,IDC_MAXTRIES,szBuffer);
		
		sprintf(szBuffer,"%.2lf",dMaxError);
		SetDlgItemText(hDlg,IDC_MAXERROR,szBuffer);
		
		EditPrintf(GetDlgItem(hDlg,IDC_LOG),"NNSIM : %s\r\n",NNCombat.Version());
		
		NNCombat.SetLayerNum(4);

		NNCombat.m_plNPR[1] = 3;
		NNCombat.m_plNPR[2] = 3;

		NNCombat.SetNNeuronsOnLayer(0,IEND);
		NNCombat.SetNNeuronsOnLayer(1,_HIDDEN);
		NNCombat.SetNNeuronsOnLayer(2,_HIDDEN1);
		NNCombat.SetNNeuronsOnLayer(3,OEND);
		NNCombat.AllocNeurons();
		NNCombat.ConnectLayer(0,1);
		NNCombat.ConnectLayer(1,2);
		NNCombat.ConnectLayer(2,3);
		
		NNCombat.InitWeights(-.3,.3);
		
		NNCombat.SaveFile("untrained.br3");

		Pattern.SetNN(&NNCombat);
		
		EditPrintf(GetDlgItem(hDlg,IDC_LOG),"... Initialized\r\n");
		SendMessage(hDlg,UM_LISTINIT,0,0);
		return false;
		
	case UM_LISTINIT:
		SendDlgItemMessage(hDlg,IDC_PATTERNLIST,LB_RESETCONTENT,0,0);
		for(lschl=0;lschl < Pattern.GetPatternNum();lschl++){
			sprintf(szBuffer,"%li",lschl);
			SendDlgItemMessage(hDlg,IDC_PATTERNLIST,LB_ADDSTRING,0,(LPARAM)szBuffer);
		}
		return true ;
		
	case WM_CLOSE:
		EndDialog(hDlg,0);
		return 0;
		
	case WM_COMMAND :
		switch(LOWORD(wParam))
		{
		case IDC_PATTERNLIST:
			switch((int)HIWORD(wParam))
			{
			case LB_SETCURSEL:
			case LBN_SELCHANGE:
				lSelNum=SendDlgItemMessage(hDlg,IDC_PATTERNLIST,LB_GETCURSEL,0,0L);
				
				dTemp = ((CBasePatternElemFF*)Pattern[lSelNum])->m_pdIn[IHealth];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				/*sprintf(szBuffer,"%lf",dTemp);
				MessageBox(hDlg,szBuffer,szBuffer,MB_OK);*/
				SendDlgItemMessage(hDlg,IDC_CHEALTH,CB_SETCURSEL,lCSel,true);
				
				dTemp = ((CBasePatternElemFF*)Pattern[lSelNum])->m_pdIn[IDistance];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CDISTANCE,CB_SETCURSEL,lCSel,true);
				
				dTemp = ((CBasePatternElemFF*)Pattern[lSelNum])->m_pdIn[IEWeapon];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CEWEAPON,CB_SETCURSEL,lCSel,true);
				
				dTemp = ((CBasePatternElemFF*)Pattern[lSelNum])->m_pdIn[IWeapon];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CWEAPON,CB_SETCURSEL,lCSel,true);
				
				dTemp = ((CBasePatternElemFF*)Pattern[lSelNum])->m_pdIn[IAmmo];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CAMMO,CB_SETCURSEL,lCSel,true);

				dTemp = ((CBasePatternElemFF*)Pattern[lSelNum])->m_pdIn[ISituation];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CSITUATION,CB_SETCURSEL,lCSel,true);
				
				// outputs ------------------------------------------------
				
				dTemp = ((CBasePatternElemFF*)Pattern[lSelNum])->m_pdOut[ODuck];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CCROUCH,CB_SETCURSEL,lCSel,true);
				
				dTemp = ((CBasePatternElemFF*)Pattern[lSelNum])->m_pdOut[OJump];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CJUMP,CB_SETCURSEL,lCSel,true);
				
				dTemp = ((CBasePatternElemFF*)Pattern[lSelNum])->m_pdOut[ODuck];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CCROUCH,CB_SETCURSEL,lCSel,true);
				
				dTemp = ((CBasePatternElemFF*)Pattern[lSelNum])->m_pdOut[OHide];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CTACTICS,CB_SETCURSEL,lCSel,true);
				
				dTemp = ((CBasePatternElemFF*)Pattern[lSelNum])->m_pdOut[OMoveType];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CMOVETYPE,CB_SETCURSEL,lCSel,true);
				
				dTemp = ((CBasePatternElemFF*)Pattern[lSelNum])->m_pdOut[OStrafe];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CSTRAFE,CB_SETCURSEL,lCSel,true);
			}
			break;
			
			case IDC_APPLY:		
				((CBasePatternElemFF*)Pattern[lSelNum])->m_pdIn[IHealth]		= SendDlgItemMessage(hDlg,IDC_CHEALTH,CB_GETCURSEL,0,0L)-1.0;
				((CBasePatternElemFF*)Pattern[lSelNum])->m_pdIn[IDistance]	= SendDlgItemMessage(hDlg,IDC_CDISTANCE,CB_GETCURSEL,0,0L)-1.0;
				((CBasePatternElemFF*)Pattern[lSelNum])->m_pdIn[IEWeapon]	= SendDlgItemMessage(hDlg,IDC_CEWEAPON,CB_GETCURSEL,0,0L)-1.0;
				((CBasePatternElemFF*)Pattern[lSelNum])->m_pdIn[IWeapon]		= SendDlgItemMessage(hDlg,IDC_CWEAPON,CB_GETCURSEL,0,0L)-1.0;
				((CBasePatternElemFF*)Pattern[lSelNum])->m_pdIn[IAmmo]		= SendDlgItemMessage(hDlg,IDC_CAMMO,CB_GETCURSEL,0,0L)-1.0;
				((CBasePatternElemFF*)Pattern[lSelNum])->m_pdIn[ISituation]	= SendDlgItemMessage(hDlg,IDC_CSITUATION,CB_GETCURSEL,0,0L)-1.0;

				((CBasePatternElemFF*)Pattern[lSelNum])->m_pdOut[ODuck]		= SendDlgItemMessage(hDlg,IDC_CCROUCH,CB_GETCURSEL,0,0L)-1.0;
				((CBasePatternElemFF*)Pattern[lSelNum])->m_pdOut[OJump]		= SendDlgItemMessage(hDlg,IDC_CJUMP,CB_GETCURSEL,0,0L)-1.0;
				((CBasePatternElemFF*)Pattern[lSelNum])->m_pdOut[OHide]		= SendDlgItemMessage(hDlg,IDC_CTACTICS,CB_GETCURSEL,0,0L)-1.0;
				((CBasePatternElemFF*)Pattern[lSelNum])->m_pdOut[OMoveType]	= SendDlgItemMessage(hDlg,IDC_CMOVETYPE,CB_GETCURSEL,0,0L)-1.0;
				((CBasePatternElemFF*)Pattern[lSelNum])->m_pdOut[OStrafe]	= SendDlgItemMessage(hDlg,IDC_CSTRAFE,CB_GETCURSEL,0,0L)-1.0;
				break;

			case IDC_PROPAGATE:
				dNNIn[IHealth]		= SendDlgItemMessage(hDlg,IDC_CHEALTH,CB_GETCURSEL,0,0L)-1.0;
				dNNIn[IDistance]	= SendDlgItemMessage(hDlg,IDC_CDISTANCE,CB_GETCURSEL,0,0L)-1.0;
				dNNIn[IEWeapon]		= SendDlgItemMessage(hDlg,IDC_CEWEAPON,CB_GETCURSEL,0,0L)-1.0;
				dNNIn[IWeapon]		= SendDlgItemMessage(hDlg,IDC_CWEAPON,CB_GETCURSEL,0,0L)-1.0;
				dNNIn[IAmmo]		= SendDlgItemMessage(hDlg,IDC_CAMMO,CB_GETCURSEL,0,0L)-1.0;
				dNNIn[ISituation]	= SendDlgItemMessage(hDlg,IDC_CSITUATION,CB_GETCURSEL,0,0L)-1.0;

				NNCombat.SetInput(dNNIn);
				NNCombat.Propagate();
				NNCombat.GetOutput(dNNOut);

				dTemp = dNNOut[ODuck];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CCROUCH,CB_SETCURSEL,lCSel,true);
				
				dTemp = dNNOut[OJump];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CJUMP,CB_SETCURSEL,lCSel,true);
				
				dTemp = dNNOut[ODuck];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CCROUCH,CB_SETCURSEL,lCSel,true);
				
				dTemp = dNNOut[OHide];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CTACTICS,CB_SETCURSEL,lCSel,true);
				
				dTemp = dNNOut[OMoveType];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CMOVETYPE,CB_SETCURSEL,lCSel,true);
				
				dTemp = dNNOut[OStrafe];
				if(dTemp < -.5) lCSel = 0;
				else if(dTemp > .5) lCSel = 2;
				else lCSel = 1;
				SendDlgItemMessage(hDlg,IDC_CSTRAFE,CB_SETCURSEL,lCSel,true);
				break;
				
			case IDC_SAVE:
				Pattern.SaveFile("NNTrain.pt3");
				EditPrintf(GetDlgItem(hDlg,IDC_LOG),"Saved Pattern to NNTrain.ptt\r\n");
				break;
				
			case IDC_SAVETEXT:
				//Pattern.SaveText("NNTrain.pta");
				EditPrintf(GetDlgItem(hDlg,IDC_LOG),"Saved Pattern to NNTrain.pta as text\r\n");
				break;
				
			case IDC_LOAD:
				
					Pattern.LoadFile(__PATTERNLOC);
				
				EditPrintf(GetDlgItem(hDlg,IDC_LOG),"Loaded Pattern from NNTrain.ptt\r\n");
				SendMessage(hDlg,UM_LISTINIT,0,0);
				break;

			case IDC_LOADNN:
				try{
					NNCombat.LoadFile("nn.br3");
					EditPrintf(GetDlgItem(hDlg,IDC_LOG),"Loaded NN from nn.br3\r\n");
				}catch(...){
					EditPrintf(GetDlgItem(hDlg,IDC_LOG),"Error loading nn.br3\r\n");
				}
				break;
				
			case IDC_SAVENN:
				try{
					NNCombat.SaveFile("nn.br3");
					EditPrintf(GetDlgItem(hDlg,IDC_LOG),"Saved NN to nn.br3\r\n");
				}catch(...){
					EditPrintf(GetDlgItem(hDlg,IDC_LOG),"Error saving nn.br3\r\n");
				}
				break;
				
			case IDC_RESETNN:
				NNCombat.InitWeights(-.3,.3);
				EditPrintf(GetDlgItem(hDlg,IDC_LOG),"Reset NN\r\n");
				break;
				
			case IDC_TRAINNN:
				glhDlg = hDlg;
				bCalc=true;
				_beginthread( TrainNN, 0, NULL );
				break;

			case IDC_CANCEL:
				bCalc=false;
				break;
				
			case IDCANCEL:
				EndDialog(hDlg,0);
				break;
				
			case IDC_INFO:
				DialogBox (0,"INFO",0,DLGPROC(InfoDlgprog));
				break;
				
			case IDC_ADD:
				Pattern.AddPattern(defaultin,defaultin);
				SendMessage(hDlg,UM_LISTINIT,0,0);
				SendDlgItemMessage(hDlg,IDC_PATTERNLIST,LB_SETCURSEL,Pattern.GetPatternNum(),true);
				break;
				
			case IDC_DELETE:
				//Pattern.DelElem(lSelNum - 1);
				SendMessage(hDlg,UM_LISTINIT,0,0);
				break;
		}
		break ;
		
	}
	return FALSE ;
}
