// WeightBProp.cpp: implementation of the CWeightBProp class.
//
//////////////////////////////////////////////////////////////////////

#include "WeightBProp.h"
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeightBProp::CWeightBProp()
{

}

CWeightBProp::~CWeightBProp()
{

}

const CWeightBProp &CWeightBProp ::  operator = (const CWeightBProp &p){
	m_dWeight = p.m_dWeight;

	return ((const CWeightBProp &)(*this));
}

void CWeightBProp :: Init(netvar_t l,netvar_t h){
	m_dWeight = (netvar_t)rand() / (netvar_t)RAND_MAX * (h-l) + l;
}

void CWeightBProp::Load(FILE *fhd){
	CBaseWeightFF::Load(fhd);

	fread(&m_dWeight,sizeof(netvar_t),1,fhd);
}

void CWeightBProp::Save(FILE *fhd){
	CBaseWeightFF::Save(fhd);

	fwrite(&m_dWeight,sizeof(netvar_t),1,fhd);
}
