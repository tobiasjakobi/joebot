// WeightBPropM.cpp: implementation of the CWeightBPropM class.
//
//////////////////////////////////////////////////////////////////////

#include "WeightBPropM.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeightBPropM::CWeightBPropM()
{
	m_dMomentum = 0;
}

CWeightBPropM::~CWeightBPropM()
{

}

const CWeightBPropM &CWeightBPropM ::  operator = (const CWeightBPropM &p){
	m_dWeight = p.m_dWeight;
	m_dMomentum = p.m_dMomentum;

	return ((const CWeightBPropM &)(*this));
}

void CWeightBPropM :: Init(netvar_t l,netvar_t h){
	m_dMomentum = 0;
	CWeightBProp::Init(l,h);
}

void CWeightBPropM::Load(FILE *fhd){
	CWeightBProp::Load(fhd);

	fread(&m_dMomentum,sizeof(netvar_t),1,fhd);
}

void CWeightBPropM::Save(FILE *fhd){
	CWeightBProp::Save(fhd);

	fwrite(&m_dMomentum,sizeof(netvar_t),1,fhd);
}
