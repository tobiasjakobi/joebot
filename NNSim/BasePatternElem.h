// BasePatternElem.h: interface for the CBasePatternElem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEPATTERNELEM_H__CF5CC414_4CDD_4C1B_B55F_D83A44D03C23__INCLUDED_)
#define AFX_BASEPATTERNELEM_H__CF5CC414_4CDD_4C1B_B55F_D83A44D03C23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NeuralNetDefs.h"
#include <stdio.h>

class CBasePatternElem  
{
public:
	CBasePatternElem();
	virtual ~CBasePatternElem();

	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);

	virtual void SaveText(FILE *fhd);
	virtual void LoadText(FILE *fhd);
};

#endif // !defined(AFX_BASEPATTERNELEM_H__CF5CC414_4CDD_4C1B_B55F_D83A44D03C23__INCLUDED_)
