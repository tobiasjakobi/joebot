// LinkBProp.h: interface for the CLinkBProp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINKBPROP_H__F6C6CCFF_E343_4768_A894_97166D8269B5__INCLUDED_)
#define AFX_LINKBPROP_H__F6C6CCFF_E343_4768_A894_97166D8269B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLinkFF.h"

class CLinkBProp  : public CBaseLinkFF
{
public:
	virtual void Load(FILE *fhd);
	virtual void Save(FILE *fhd);
	CLinkBProp();
	virtual ~CLinkBProp();

};

#endif // !defined(AFX_LINKBPROP_H__F6C6CCFF_E343_4768_A894_97166D8269B5__INCLUDED_)
