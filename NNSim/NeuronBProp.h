// NeuronBProp.h: interface for the CNeuronBProp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEURONBPROP_H__F9CAD8EF_D1CB_4617_9F78_DDC25CCD3763__INCLUDED_)
#define AFX_NEURONBPROP_H__F9CAD8EF_D1CB_4617_9F78_DDC25CCD3763__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseNeuronFF.h"
#include "LinkBProp.h"
#include "WeightBProp.h"

class CNeuronBProp : public CBaseNeuronFF  
{
	friend class CNeuralNetBProp;
public:
	virtual void Load(FILE *);
	virtual void Save(FILE *);
	CNeuronBProp();
	virtual ~CNeuronBProp();

	virtual CBaseLink *NewLink(void);

	virtual void Calc(void);
	virtual void CalcDelta(void);
protected:
};

#endif // !defined(AFX_NEURONBPROP_H__F9CAD8EF_D1CB_4617_9F78_DDC25CCD3763__INCLUDED_)
