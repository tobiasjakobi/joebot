// BasePatternElemFF.h: interface for the CBasePatternElemFF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEPATTERNELEMFF_H__EE0104D4_9ED8_4F1E_AAD2_928310BE7AD5__INCLUDED_)
#define AFX_BASEPATTERNELEMFF_H__EE0104D4_9ED8_4F1E_AAD2_928310BE7AD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BasePatternElem.h"

class CBasePatternElemFF : public CBasePatternElem  
{
public:
	CBasePatternElemFF();
	virtual ~CBasePatternElemFF();
	void SetDim(long,long);

	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);

	virtual void SaveText(FILE *fhd);
	virtual void LoadText(FILE *fhd);

	virtual void SetInput(netvar_t *p);
	virtual void SetOutput(netvar_t *p);

	netvar_t *m_pdIn;
	netvar_t *m_pdOut;

	long m_lInDim;
	long m_lOutDim;

	netvar_t m_dMaxError;
	netvar_t m_dMinError;
	netvar_t m_dAvError;
};

#endif // !defined(AFX_BASEPATTERNELEMFF_H__EE0104D4_9ED8_4F1E_AAD2_928310BE7AD5__INCLUDED_)
