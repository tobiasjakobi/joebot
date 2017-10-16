// PatternElemBProp.h: interface for the CPatternElemBProp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATTERNELEMBPROP_H__6FA86838_116C_4341_95F8_33D4F7C9366B__INCLUDED_)
#define AFX_PATTERNELEMBPROP_H__6FA86838_116C_4341_95F8_33D4F7C9366B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BasePatternElemFF.h"

class CPatternElemBProp : public CBasePatternElemFF  
{
public:
	CPatternElemBProp();
	virtual ~CPatternElemBProp();

	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);

	virtual void SaveText(FILE *fhd);
	virtual void LoadText(FILE *fhd);
};

#endif // !defined(AFX_PATTERNELEMBPROP_H__6FA86838_116C_4341_95F8_33D4F7C9366B__INCLUDED_)
