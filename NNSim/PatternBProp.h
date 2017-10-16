// PatternBProp.h: interface for the CPatternBProp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATTERNBPROP_H__19754D43_C996_4DEC_AF89_EB0A61FEBB7A__INCLUDED_)
#define AFX_PATTERNBPROP_H__19754D43_C996_4DEC_AF89_EB0A61FEBB7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BasePatternFF.h"

class CPatternBProp : public CBasePatternFF
{
public:
	CPatternBProp();
	virtual ~CPatternBProp();

	virtual CBasePatternElem *NewPattern(void);

	virtual void TeachEpoch(void);
	virtual void GetErrors(void);
	virtual void TeachPartoEpoch(long);

	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);
};

#endif // !defined(AFX_PATTERNBPROP_H__19754D43_C996_4DEC_AF89_EB0A61FEBB7A__INCLUDED_)
