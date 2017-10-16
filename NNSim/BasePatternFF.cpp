// BasePatternFF.cpp: implementation of the CBasePatternFF class.
//
//////////////////////////////////////////////////////////////////////

#include "BasePatternFF.h"
#include "BasePatternElemFF.h"
#include <memory.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBasePatternFF::CBasePatternFF()
{
	m_lInDim = 0;
	m_lOutDim = 0;
}

CBasePatternFF::~CBasePatternFF()
{

}

CBasePatternElem *CBasePatternFF :: AddPattern(netvar_t *pIn,netvar_t*pOut){
	CBasePatternElemFF *p= (CBasePatternElemFF*)CBasePattern::AddPattern();

	p->SetDim(m_lInDim,m_lOutDim);

	p->SetInput(pIn);
	p->SetOutput(pOut);
	return p;
}

CBasePatternElem *CBasePatternFF :: NewPattern(void){
	return new CBasePatternElemFF;
}

void CBasePatternFF :: SetNN(CBaseNeuralNetFF *pNN){
	m_lInDim = pNN->GetInputNum();
	m_lOutDim = pNN->GetOutputNum();

	m_pNN = pNN;
}

void CBasePatternFF :: Load(FILE *fhd){
	CBasePattern::Load(fhd);
}

void CBasePatternFF :: Save(FILE *fhd){
	CBasePattern::Save(fhd);
}
