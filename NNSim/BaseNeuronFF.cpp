// BaseNeuronFF.cpp: implementation of the CBaseNeuronFF class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseNeuronFF.h"
#include "BaseWeightFF.h"
#include <stdlib.h>
#include <memory.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseNeuronFF::CBaseNeuronFF()
{
	ppLinksFrom = 0;
	ppLinksTo = 0;

	lLToNum	= 0;
	lLToMax = 0;
	lLFromNum = 0;
	lLFromMax = 0;

	m_dBias = 0;
	m_dNet = 0;
	m_dAct = 0;
	m_dOut = 0;
	m_dDelta = 0;
	m_dTI = 0;

	m_iTypeAF = NEURON_ACT_TANH;			// set activation functions

	SetAct(m_iTypeAF,TOAC_DEFAULT,m_pfAct,m_pfActS);
}

CBaseNeuronFF::~CBaseNeuronFF()
{
	long lschl;

	for(lschl = 0;lschl < lLToMax;lschl++){		// alloc new weights
		delete ppLinksTo[lschl];
	}

	for(lschl = 0;lschl < lLFromMax;lschl++){		// alloc new weights
		delete ppLinksFrom[lschl];
	}
}

void CBaseNeuronFF :: InitWeights(netvar_t l,netvar_t h){
	m_dBias = (netvar_t)rand() / (netvar_t)RAND_MAX * (h-l) + l;
}

void CBaseNeuronFF :: SetBias(netvar_t &d){
	m_dBias = d;
}

void CBaseNeuronFF::Load(FILE *fhd)
{
	CBaseNeuron::Load(fhd);

	fread(&m_bHidden,sizeof(bool),1,fhd);
	fread(&m_dAct,sizeof(netvar_t),1,fhd);
	fread(&m_dBias,sizeof(netvar_t),1,fhd);
	fread(&m_dDelta,sizeof(netvar_t),1,fhd);
	fread(&m_dNet,sizeof(netvar_t),1,fhd);
	fread(&m_dOut,sizeof(netvar_t),1,fhd);
	fread(&m_dTI,sizeof(netvar_t),1,fhd);
	fread(&m_iTypeAF,sizeof(int),1,fhd);
}

void CBaseNeuronFF::Save(FILE *fhd)
{
	CBaseNeuron::Save(fhd);

	fwrite(&m_bHidden,sizeof(bool),1,fhd);
	fwrite(&m_dAct,sizeof(netvar_t),1,fhd);
	fwrite(&m_dBias,sizeof(netvar_t),1,fhd);
	fwrite(&m_dDelta,sizeof(netvar_t),1,fhd);
	fwrite(&m_dNet,sizeof(netvar_t),1,fhd);
	fwrite(&m_dOut,sizeof(netvar_t),1,fhd);
	fwrite(&m_dTI,sizeof(netvar_t),1,fhd);
	fwrite(&m_iTypeAF,sizeof(int),1,fhd);
}

CBaseLink *CBaseNeuronFF :: AddLinkFrom(long lW, CBaseWeight *pW,CBaseNeuron *p){
	if(lLFromNum >= lLFromMax){
		// at end of field -> alloc new 
		lLFromMax += _ARRAY_SEG_SIZE_NLINK;
		CBaseLink **pNew = new CBaseLink*[lLFromMax];
		if(ppLinksFrom){
			//memset(pNew,0,lLFromMax);
			memcpy(pNew,ppLinksFrom,sizeof(CBaseLink*)*lLFromNum);// copy from old to new
			delete [] ppLinksFrom;									// and delete it
		}

		for(long lschl = lLFromNum;lschl < lLFromMax;lschl++){		// alloc new weights
			pNew[lschl] = NewLink();
		}
		ppLinksFrom = pNew;											// copy pointer to new data to var
	}

	((CBaseLinkFF*)(ppLinksFrom[lLFromNum]))->p = p;
	((CBaseLinkFF*)(ppLinksFrom[lLFromNum]))->lWIndex = lW;
	((CBaseLinkFF*)(ppLinksFrom[lLFromNum]))->pW = pW;
	pW->lUsed++;

	lLFromNum ++;

	return ppLinksFrom[lLFromNum - 1];
}

CBaseLink *CBaseNeuronFF :: AddLinkTo(long lW, CBaseWeight *pW, CBaseNeuron *p){
	if(lLToNum >= lLToMax){
		// at end of field -> alloc new 
		lLToMax += _ARRAY_SEG_SIZE_NLINK;
		CBaseLink **pNew = new CBaseLink*[lLToMax];
		if(ppLinksTo){
			//memset(pNew,0,lLToMax);
			memcpy(pNew,ppLinksTo,sizeof(CBaseLink*)*lLToNum);// copy To old to new
			delete [] ppLinksTo;									// and delete it
		}

		for(long lschl = lLToNum;lschl < lLToMax;lschl++){		// alloc new weights
			pNew[lschl] = NewLink();
		}
		ppLinksTo = pNew;											// copy pointer to new data to var
	}

	((CBaseLinkFF*)(ppLinksTo[lLToNum]))->p = p;
	((CBaseLinkFF*)(ppLinksTo[lLToNum]))->lWIndex = lW;
	((CBaseLinkFF*)(ppLinksTo[lLToNum]))->pW = pW;
	pW->lUsed++;
	lLToNum ++;

	return ppLinksTo[lLToNum - 1];
}

CBaseLink *CBaseNeuronFF::getLinkTo(CBaseNeuron *p){
	for(long lC=0;lC < lLToNum; lC++){
		if( ((CBaseLinkFF *)(ppLinksTo[lC]))->p == p){
			return ppLinksTo[lC];
		}
	}
	return 0;
}
