// BasePatternFF.h: interface for the CBasePatternFF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEPATTERNFF_H__42875618_7B2D_404F_8F4C_09262CCCE717__INCLUDED_)
#define AFX_BASEPATTERNFF_H__42875618_7B2D_404F_8F4C_09262CCCE717__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BasePattern.h"
#include "BasePatternElemFF.h"
#include "BaseNeuralNetFF.h"

class CBasePatternFF : public CBasePattern  
{
public:
	CBasePatternFF();
	virtual ~CBasePatternFF();

	virtual CBasePatternElem *AddPattern(netvar_t *,netvar_t*);
	virtual CBasePatternElem *NewPattern(void);

	virtual void SetNN(CBaseNeuralNetFF *);
	virtual void TeachEpoch(void) = 0;
	virtual void TeachPartoEpoch(long) = 0;

	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);

	long m_lInDim,
		m_lOutDim;

	CBaseNeuralNetFF *m_pNN;

	netvar_t m_dMinError;
	netvar_t m_dMaxError;
	netvar_t m_dAvError;
};

#endif // !defined(AFX_BASEPATTERNFF_H__42875618_7B2D_404F_8F4C_09262CCCE717__INCLUDED_)
