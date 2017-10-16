// WeightBPropM.h: interface for the CWeightBPropM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEIGHTBPROPM_H__C5BF4C2A_EAE9_41F5_BE6F_C0C5B61017A2__INCLUDED_)
#define AFX_WEIGHTBPROPM_H__C5BF4C2A_EAE9_41F5_BE6F_C0C5B61017A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WeightBProp.h"

class CWeightBPropM : public CWeightBProp  
{
	friend class CNeuralNetBPropM;
public:
	CWeightBPropM();
	virtual ~CWeightBPropM();

	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);

	virtual void Init(netvar_t,netvar_t);

	virtual const CWeightBPropM & operator = (const CWeightBPropM &);
protected:
	netvar_t m_dMomentum;
};

#endif // !defined(AFX_WEIGHTBPROPM_H__C5BF4C2A_EAE9_41F5_BE6F_C0C5B61017A2__INCLUDED_)
