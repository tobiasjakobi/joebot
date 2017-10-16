// BaseWeightFF.cpp: implementation of the CBaseWeightFF class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseWeightFF.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseWeightFF::CBaseWeightFF()
{

}

CBaseWeightFF::~CBaseWeightFF()
{

}

const CBaseWeightFF &CBaseWeightFF ::  operator = (const CBaseWeightFF &){
	return ((const CBaseWeightFF &)(*this));
}

void CBaseWeightFF::Load(FILE *fhd)
{
	CBaseWeight::Load(fhd);
}

void CBaseWeightFF::Save(FILE *fhd)
{
	CBaseWeight::Save(fhd);
}

void CBaseWeightFF::SetWeight(netvar_t &d){
	m_dWeight = d;
}

netvar_t CBaseWeightFF::GetWeight(void){
	return m_dWeight;
}
