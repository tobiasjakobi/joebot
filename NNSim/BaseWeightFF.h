// BaseWeightFF.h: interface for the CBaseWeightFF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEWEIGHTFF_H__6CA598BC_802A_44A5_80BB_ECCBE9DDEC51__INCLUDED_)
#define AFX_BASEWEIGHTFF_H__6CA598BC_802A_44A5_80BB_ECCBE9DDEC51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseWeight.h"

class CBaseWeightFF : public CBaseWeight
{
public:
	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);
	CBaseWeightFF();
	virtual ~CBaseWeightFF();

	virtual void Init(netvar_t,netvar_t) = 0;

	void SetWeight(netvar_t &d);
	netvar_t GetWeight(void);

	virtual const CBaseWeightFF & operator = (const CBaseWeightFF &);
protected:
	netvar_t m_dWeight;
};

#endif // !defined(AFX_BASEWEIGHTFF_H__6CA598BC_802A_44A5_80BB_ECCBE9DDEC51__INCLUDED_)
