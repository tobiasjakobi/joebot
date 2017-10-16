// WeightBProp.h: interface for the CWeightBProp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEIGHTBPROP_H__9BD16BA8_7FF8_445B_8359_86BFEE7BE86C__INCLUDED_)
#define AFX_WEIGHTBPROP_H__9BD16BA8_7FF8_445B_8359_86BFEE7BE86C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseWeightFF.h"

class CWeightBProp : public CBaseWeightFF  
{
	friend class CNeuralNetBProp;
	friend class CNeuralNetBPropM;
	friend class CNeuronBProp;
	friend void InitObjects(void);
public:
	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);
	CWeightBProp();
	virtual ~CWeightBProp();

	virtual void Init(netvar_t,netvar_t);

	virtual const CWeightBProp & operator = (const CWeightBProp &);
protected :
};

#endif // !defined(AFX_WEIGHTBPROP_H__9BD16BA8_7FF8_445B_8359_86BFEE7BE86C__INCLUDED_)
