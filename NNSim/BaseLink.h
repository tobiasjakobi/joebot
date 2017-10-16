// BaseLink.h: interface for the CBaseLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASELINK_H__84232728_A1D0_4F08_B4AF_F2E9E4A48891__INCLUDED_)
#define AFX_BASELINK_H__84232728_A1D0_4F08_B4AF_F2E9E4A48891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseWeight.h"

class CBaseLink  
{
public:
	virtual void Load(FILE *fhd);
	virtual void Save(FILE *fhd);
	CBaseLink();
	virtual ~CBaseLink();

	long lWIndex;
	CBaseWeight *pW;			// just for speeding up
};

#endif // !defined(AFX_BASELINK_H__84232728_A1D0_4F08_B4AF_F2E9E4A48891__INCLUDED_)
