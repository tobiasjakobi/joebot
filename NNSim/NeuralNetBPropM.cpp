// NeuralNetBPropM.cpp: implementation of the CNeuralNetBPropM class.
//
//////////////////////////////////////////////////////////////////////

#include "NeuralNetBPropM.h"
#include "memory.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNeuralNetBPropM::CNeuralNetBPropM()
{
	memset(m_szType,0,sizeof(char)*_LTYPEI);
	strcpy(m_szType,"CNeuralNetBPropM");
	m_iType = NT_NEURALNETBPROPM;

	m_dMomentum = .0;
}

CNeuralNetBPropM::~CNeuralNetBPropM()
{

}

void CNeuralNetBPropM :: SetMomentum(const netvar_t &dP){
	m_dMomentum = dP;
}

netvar_t CNeuralNetBPropM :: GetMomentum(void){
	return m_dMomentum;
}

void CNeuralNetBPropM::Save(FILE *fhd){
	CNeuralNetBProp::Save(fhd);

	fwrite(&m_dMomentum,sizeof(netvar_t),1,fhd);
}

void CNeuralNetBPropM::Load(FILE *fhd){
	CNeuralNetBProp::Load(fhd);
	fread(&m_dMomentum,sizeof(netvar_t),1,fhd);
}

CBaseNeuron* CNeuralNetBPropM :: NewNeuron(void){
	return new CNeuronBPropM;
}

CBaseWeight* CNeuralNetBPropM :: NewWeight(void){
	return new CWeightBPropM;
}

bool CNeuralNetBPropM::BackPropagate(){
	if(!m_pTI || !pppNeurons)
		return false;

	m_lNumBProp ++;
	CopyTI2N();

	long lL,lN,lIN;
	CBaseNeuron **ppNL;
	CNeuronBPropM *pNBP;
	CLinkBPropM *p;
	netvar_t dDelta,dChange;

	for(lL = lLayerNum-1;lL >0;lL--){
		ppNL = pppNeurons[lL];
		for(lN = plLNNum[lL]-1;lN>-1;lN--){
			((CBaseNeuronFF*)ppNL[lN])->CalcDelta();
		}
	}

	for(lL = lLayerNum-1;lL >0;lL--){
		ppNL = pppNeurons[lL];
		for(lN = plLNNum[lL]-1;lN>-1;lN--){
			pNBP = (CNeuronBPropM*)ppNL[lN];

			dDelta = pNBP->m_dDelta;

			dChange = (dDelta * m_dLRate) + pNBP->m_dBiasMomentum * m_dMomentum;

			pNBP->m_dBias -= dChange;
			
			for(lIN = pNBP->lLFromNum-1;lIN>-1;lIN--){
				p = (CLinkBPropM*)(pNBP->ppLinksFrom[lIN]);
				dChange = (m_dLRate * ((CNeuronBPropM *)(p->p))->m_dOut * dDelta) + ((CWeightBPropM*)(p->pW))->m_dMomentum * m_dMomentum;
				((CWeightBPropM*)(p->pW))->m_dWeight += dChange;
			}
		}
	}

	return true;
}
