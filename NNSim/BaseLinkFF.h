// BaseLinkFF.h: interface for the CBaseLinkFF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASELINKFF_H__E5D674CC_F100_42B4_A339_0205DA519BF4__INCLUDED_)
#define AFX_BASELINKFF_H__E5D674CC_F100_42B4_A339_0205DA519BF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLink.h"
#include "BaseNeuron.h"

class CBaseLinkFF : public CBaseLink  
{
public:
	virtual void Load(FILE *fhd);
	virtual void Save(FILE *fhd);
	CBaseLinkFF();
	virtual ~CBaseLinkFF();

	CBaseNeuron *p;
};

#endif // !defined(AFX_BASELINKFF_H__E5D674CC_F100_42B4_A339_0205DA519BF4__INCLUDED_)
