// NeuralNetBProp.cpp: implementation of the CNeuralNetBProp class.
//
//////////////////////////////////////////////////////////////////////

#include "NeuralNetBProp.h"
#include <memory.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNeuralNetBProp::CNeuralNetBProp(){
	memset(m_szType,0,sizeof(char)*_LTYPEI);
	strcpy(m_szType,"CNeuralNetBProp");
	m_iType = NT_NEURALNETBPROP;

	m_lNumBProp = 0;
}

CNeuralNetBProp::~CNeuralNetBProp()
{

}

bool CNeuralNetBProp :: Propagate(void){
	m_lNumProp ++;

	long lL,lN;
	CBaseNeuron **ppNL;
	for(lL = 1;lL < lLayerNum;lL++){
		ppNL = pppNeurons[lL];
		for(lN = plLNNum[lL]-1;lN>-1;lN--){
			ppNL[lN]->Calc();
		}
	}
	return true;
}

bool CNeuralNetBProp::BackPropagate(){
	if(!m_pTI || !pppNeurons)
		return false;

	long lL,lN,lIN;
	CBaseNeuron **ppNL;
	CNeuronBProp *pNBP;
	CLinkBProp *p;
	netvar_t dDelta;

	m_lNumBProp ++;

	CopyTI2N();

	for(lL = lLayerNum-1;lL >0;lL--){
		ppNL = pppNeurons[lL];
		for(lN = plLNNum[lL]-1;lN>-1;lN--){
			((CBaseNeuronFF*)(ppNL[lN]))->CalcDelta();
		}
	}

	for(lL = lLayerNum-1;lL >0;lL--){
		ppNL = pppNeurons[lL];
		for(lN = plLNNum[lL]-1;lN>-1;lN--){
			pNBP = (CNeuronBProp*)ppNL[lN];

			dDelta = pNBP->m_dDelta;

			pNBP->m_dBias -= (dDelta * m_dLRate);

			for(lIN = pNBP->lLFromNum-1;lIN>-1;lIN--){
				p = (CLinkBProp*)(pNBP->ppLinksFrom[lIN]);
				//cout << " : " << ((CWeightBProp*)(p->pW))->m_dWeight << " -:=:- " <<(m_dLRate * ((CNeuronBProp *)(p->p))->m_dOut * dDelta) << endl;
				((CWeightBProp*)(p->pW))->m_dWeight += (m_dLRate * ((CNeuronBProp *)(p->p))->m_dOut * dDelta);
			}
		}
	}

	return true;
}

void CNeuralNetBProp::Save(FILE *fhd){
	CBaseNeuralNetFF::Save(fhd);

	fwrite(&m_lNumBProp,sizeof(long),1,fhd);
}

void CNeuralNetBProp::Load(FILE *fhd){
	CBaseNeuralNetFF::Load(fhd);
	fread(&m_lNumBProp,sizeof(long),1,fhd);
}

CBaseNeuron* CNeuralNetBProp :: NewNeuron(void){
	return new CNeuronBProp;
}

CBaseWeight* CNeuralNetBProp :: NewWeight(void){
	return new CWeightBProp;
}
