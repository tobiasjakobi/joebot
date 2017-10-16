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
#include <iostream.h>

#include "NeuralNet.h"
#include "BotNNDefs.h"
#include "nVec.h"

#define _MAXERROR	.2
#define _MAXCERROR	.2
#define _HIDDEN		7
#define _HIDDEN1	7
#define MAX_TRIES 750
#define _CHIDDEN	3
#define MAX_CTRIES 1000

void traincombat(void){
	double dInit = .5,dError=1000;
	double dIn[_NNBInput];
	double dOut[_NNBOutput];
	long lEpoch=0,lschl;
	bool bflag;
	CNeuralNetBProp NN;
	CPatternBProp Pattern;
//	FILE *fhd;
	long lloop;
	
	cout << "Program to train the NN for bot" << endl;
	
	NN.SetLRate(.1);
	NN.SetLayerNum(4);
	NN.m_plNPR[1] = 3;
	NN.m_plNPR[2] = 3;
	NN.SetNNeuronsOnLayer(0,IEND);
	NN.SetNNeuronsOnLayer(1,_HIDDEN);
	NN.SetNNeuronsOnLayer(2,_HIDDEN1);
	NN.SetNNeuronsOnLayer(3,OEND);
	NN.AllocNeurons();
	NN.ConnectLayer(0,1);
	NN.ConnectLayer(1,2);
	NN.ConnectLayer(2,3);
	//NN.ConnectFullLayer(0,3);
	NN.InitWeights(-.3,.3);
	//NN.SetMomentum(.7f);
	NN.SaveFile("untrained.br3");
	Pattern.SetNN(&NN);
	
	//	defs of pattern
	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Mid;
	dIn[IAmmo]			= A_High;
	dIn[ISituation]		= S_Normal;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);
	
	// long range weapons
	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_Duck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_Duck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_Low;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_Duck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_Duck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	//Pattern.AddPattern(dIn,dOut); /////

	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_Low;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_Duck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	//-near
	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Near;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Near;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_Jump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Near;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_Low;
	
	dOut[OJump]			= J_Jump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	// short range weapons										--------------
	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_Low;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_Low;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	//-near
	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Near;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_Jump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= S_Right;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Near;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_Jump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= S_Right;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Near;
	dIn[IEWeapon]		= EW_Low;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_Low;
	
	dOut[OJump]			= J_Jump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= S_Right;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

// ----------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------

	// long range weapons
	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_Duck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_Duck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_Low;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_Duck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_Low;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_Duck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	//-near
	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Near;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Near;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_Jump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Near;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Long;
	dIn[IAmmo]			= A_Low;
	
	dOut[OJump]			= J_Jump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	// short range weapons										--------------
	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_Low;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Far;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_Low;
	
	dOut[OJump]			= J_NoJump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= 0;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	//-near
	dIn[IHealth]		= H_High;
	dIn[IDistance]		= D_Near;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_Jump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= S_Right;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Near;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_High;
	
	dOut[OJump]			= J_Jump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_None;
	dOut[OStrafe]		= S_Right;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);

	dIn[IHealth]		= H_Low;
	dIn[IDistance]		= D_Near;
	dIn[IEWeapon]		= EW_Long;
	dIn[IWeapon]		= W_Low;
	dIn[IAmmo]			= A_Low;
	
	dOut[OJump]			= J_Jump;
	dOut[ODuck]			= D_NoDuck;
	dOut[OHide]			= H_Hide;
	dOut[OStrafe]		= S_Right;
	dOut[OMoveType]		= T_Run;
	
	Pattern.AddPattern(dIn,dOut);
	
	for(lschl=Pattern.GetPatternNum()-1;lschl>-1;lschl--){
		if(((CBasePatternElemFF*)Pattern[lschl])->m_pdIn[IHealth] == H_High){
			memcpy(dIn,((CBasePatternElemFF*)Pattern[lschl])->m_pdIn,sizeof(double)*_NNBInput);
			memcpy(dOut,((CBasePatternElemFF*)Pattern[lschl])->m_pdOut,sizeof(double)*_NNBOutput);
			
			dIn[IHealth] = H_Mid;

			Pattern.AddPattern(dIn,dOut);
		}
	}

	for(lschl=Pattern.GetPatternNum()-1;lschl>-1;lschl--){
		if(((CBasePatternElemFF*)Pattern[lschl])->m_pdIn[ISituation] == S_Normal){
			memcpy(dIn,((CBasePatternElemFF*)Pattern[lschl])->m_pdIn,sizeof(double)*_NNBInput);
			memcpy(dOut,((CBasePatternElemFF*)Pattern[lschl])->m_pdOut,sizeof(double)*_NNBOutput);
			
			if(dOut[OHide] == H_None && !(dIn[IDistance]==D_Far&&dIn[IWeapon]==W_Long)&&dIn[IWeapon]!=W_Long){
				dIn[ISituation] = S_Bad;
				dOut[OHide] = H_Hide;
				
				Pattern.AddPattern(dIn,dOut);
			}

			memcpy(dIn,((CBasePatternElemFF*)Pattern[lschl])->m_pdIn,sizeof(double)*_NNBInput);
			memcpy(dOut,((CBasePatternElemFF*)Pattern[lschl])->m_pdOut,sizeof(double)*_NNBOutput);
			
			if(dOut[OHide] ==H_Hide){
				dIn[ISituation] = S_Good;
				dOut[OHide] = H_None;
				
				Pattern.AddPattern(dIn,dOut);
			}
		}
	}
	
	Pattern.SaveFile("NNTrain.ptt");
	//Pattern.SaveText("NNTrain.pta");
	cout << "number of pattern :"<<Pattern.GetPatternNum() << endl;
	//Pattern.LoadText("NNTrain.pta");
	FILE *fhd;
	fhd = fopen("learnlog.html","w");
	//Pattern.StartLog(fhd);
	//Pattern.EnableHTMLLog(true);
	Pattern.LogErrors(true,_LODL_FINE);
	Pattern.Log2File(fhd);
//	fprintf(fhd,"<html><body><table>");
	
	bflag=true;
	lloop=0;
	while(bflag){
		lloop++;
		for(lschl=0;lschl < MAX_TRIES;lschl++){
			//fprintf(fhd,"<td>%li",lschl);
			Pattern.TeachEpoch();
			lEpoch++;
			cout << lEpoch << " ";cout.flush();
			dError = Pattern.m_dMaxError;
			if(dError < _MAXERROR)
				break;
			//cout << dError;
		}
		if(lschl >= MAX_TRIES){
			lEpoch = 0;
			NN.InitWeights(-.3,.3);
		}
		else
			bflag = false;
	}
	
	//Pattern.EndLog();
	fclose(fhd);

	Pattern.GetErrors();
	cout <<endl<<"max error :"<< Pattern.m_dMaxError << endl;
	NN.SaveFile("trained.br3");
	NN.LoadFile("trained.br3");
	Pattern.GetErrors();
	cout <<endl<<"max error :"<< Pattern.m_dMaxError << endl;

	//cout << lEpoch << endl;
	cout << "Saving local copy"<<endl;
	NN.SaveFile("trained.br3");
	NN.LoadFile("trained.br3");
	//cout << "Saving copy to c:\\spiele\\half-life\\cstrike\\joebot" << endl;
	NN.SaveFile("c:\\spiele\\half-life\\cstrike\\joebot\\nn.br3");
	//cout << "Saving copy to f:\\CJoeBot\\cstrike\\joebot\\" << endl;
	//NN.Save("f:\\CJoeBot\\cstrike\\joebot\\nn.br2");

	CGenom Gen;
	Gen.GetFFNet(&NN);
	Gen.SaveDescription("combatnetdescription.txt");
	Gen.Save("nncombat.br3");
}

void traincoll(void){
	double dInit = .3,dError=1000;
	double dIn[3];
	double dOut[1];
	long lEpoch=0,lschl;
	bool bflag;
	long lloop;
	CNeuralNetBProp NN;
	CPatternBProp Pattern;
	FILE *fhd;
	nVec nVIn(3);
	nVIn.SetMemVec(dIn);
	
	cout << "Program to train the NN for bot" << endl;
	
	NN.SetLRate(.1);
	
	NN.SetLayerNum(3);
	NN.SetNNeuronsOnLayer(0,3);
	NN.SetNNeuronsOnLayer(1,_CHIDDEN);
	NN.SetNNeuronsOnLayer(2,1);
	NN.AllocNeurons();
	NN.ConnectLayer(0,1);
	NN.ConnectLayer(1,2);
	//NN.ConnectFullLayer(2,3);
	//NN.ConnectFullLayer(0,2);
	NN.InitWeights(-.3,.3);
	NN.SaveFile("colluntrained.br3");
	Pattern.SetNN(&NN);

	// defs of pattern

	dIn[ICI_Left] = 1;
	dIn[ICI_Middle] = 1;
	dIn[ICI_Right] = 1;
	dOut[0] = OC_Middle;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);
	dIn[ICI_Left] = -1;
	dIn[ICI_Middle] = -1;
	dIn[ICI_Right] = -1;
	dOut[0] = OC_Middle;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);
	dIn[ICI_Left] = -0;
	dIn[ICI_Middle] = -0;
	dIn[ICI_Right] = -0;
	dOut[0] = OC_Middle;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);
	dIn[ICI_Left] = 0;
	dIn[ICI_Middle] = -1;
	dIn[ICI_Right] = 0;
	dOut[0] = OC_Right;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);
	/*dIn[ICI_Left] = 1;
	dIn[ICI_Middle] = -1;
	dIn[ICI_Right] = 1;
	dOut[0] = OC_Right;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);*/

	dIn[ICI_Left] = -1;
	dIn[ICI_Middle] = 1;
	dIn[ICI_Right] = -1;
	dOut[0] = OC_Middle;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);

	/*dIn[ICI_Left] = 1;
	dIn[ICI_Middle] = 0;
	dIn[ICI_Right] = 1;
	dOut[0] = OC_Right;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);
	dIn[ICI_Left] = 1;
	dIn[ICI_Middle] = -1;
	dIn[ICI_Right] = 1;
	dOut[0] = OC_Right;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);*/
	dIn[ICI_Left] = 0;
	dIn[ICI_Middle] = 1;
	dIn[ICI_Right] = 1;
	dOut[0] = OC_RIGHT;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);
	dIn[ICI_Left] = -1;
	dIn[ICI_Middle] = 1;
	dIn[ICI_Right] = 1;
	dOut[0] = OC_RIGHT;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);
	dIn[ICI_Left] = 0;
	dIn[ICI_Middle] = 0;
	dIn[ICI_Right] = 1;
	dOut[0] = OC_RIGHT;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);
	dIn[ICI_Left] = -1;
	dIn[ICI_Middle] = -1;
	dIn[ICI_Right] = 1;
	dOut[0] = OC_RIGHT;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);
	dIn[ICI_Left] = 1;
	dIn[ICI_Middle] = 1;
	dIn[ICI_Right] = 0;
	dOut[0] = OC_LEFT;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);
	dIn[ICI_Left] = 1;
	dIn[ICI_Middle] = 1;
	dIn[ICI_Right] = -1;
	dOut[0] = OC_LEFT;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);
	dIn[ICI_Left] = 1;
	dIn[ICI_Middle] = 0;
	dIn[ICI_Right] = 0;
	dOut[0] = OC_LEFT;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);
	dIn[ICI_Left] = 1;
	dIn[ICI_Middle] =-1;
	dIn[ICI_Right] = -1;
	dOut[0] = OC_LEFT;
	nVIn.Normalize();
	Pattern.AddPattern(dIn,dOut);

	//Pattern.Save("CollNNTrain.ptt");
	//Pattern.SaveText("CollNNTrain.pta");
	//Pattern.LoadText("NNTrain.pta");
	fhd = fopen("colllearnlog.html","w");
	//Pattern.StartLog(fhd);
	//Pattern.EnableHTMLLog(true);
	Pattern.LogErrors(true,_LODL_FINE);
	Pattern.Log2File(fhd);
	
	bflag=true;
	lloop=0;
	while(bflag){
		lloop++;
		for(lschl=0;lschl < MAX_CTRIES;lschl++){
			//fprintf(fhd,"<td>%li",lschl);
			Pattern.TeachEpoch();
			lEpoch++;
			cout << lEpoch << " ";cout.flush();
			dError = Pattern.m_dMaxError;
			if(dError < _MAXCERROR)
				break;
			//cout << dError;
		}
		if(lschl >= MAX_CTRIES){
			lEpoch = 0;
			NN.InitWeights(-.3,.3);
		}
		else
			bflag = false;
	}
	
//	Pattern.EndLog();
	fclose(fhd);
	cout << lEpoch << endl;
	cout << "Saving local copy"<<endl;
	NN.SaveFile("colltrained.br3");
	NN.SaveFile("c:\\spiele\\half-life\\cstrike\\joebot\\nnc.br3");
	NN.LoadFile("colltrained.br3");
	

	/*NN.Load("colltrained.br2");
	NN.Save("colltrained-.br2");*/


	CGenom Gen;
	Gen.GetFFNet(&NN);
	Gen.SaveDescription("collnetdescription.txt");
	Gen.Save("nncoll.br3");

	dIn[0] = 1;
	dIn[1] = -1;
	dIn[2] = -1;
	nVIn.Normalize();
	cout << dIn[0] << "." << dIn[1] << "." << dIn[2] << "." <<endl;
	NN.SetInput(dIn);
	NN.Propagate();
	double d[1];
	NN.GetOutput(d);
	cout << d[0] << endl;
}

void main(void){
	traincoll();
	traincombat();
	/*NeuralNetBProp temp;
	temp.Load("nn.br2");
	cout << temp.LWeights[6]->dWeight<<endl;*/
	//Pattern test;
	/*test.LoadText("nntrain.pta");
	test.SaveText("-nntrain.pta");*/
}
