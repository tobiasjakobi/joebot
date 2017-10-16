// BaseWeight.h: interface for the CBaseWeight class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEWEIGHT_H__5D5E1E5D_AD6E_4803_8F86_2332BC7B10DE__INCLUDED_)
#define AFX_BASEWEIGHT_H__5D5E1E5D_AD6E_4803_8F86_2332BC7B10DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NeuralNetDefs.h"
#include <stdio.h>

class CBaseWeight  
{
public:
	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);
	CBaseWeight();
	virtual ~CBaseWeight();
	virtual void Init(netvar_t,netvar_t) = 0;

	virtual const CBaseWeight & operator = (const CBaseWeight &);
	long lUsed;
};

#endif // !defined(AFX_BASEWEIGHT_H__5D5E1E5D_AD6E_4803_8F86_2332BC7B10DE__INCLUDED_)
