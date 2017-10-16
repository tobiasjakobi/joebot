// NeuralNetBProp.h: interface for the CNeuralNetBProp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEURALNETBPROP_H__7E181A08_4519_4118_B019_AFD19B448BFA__INCLUDED_)
#define AFX_NEURALNETBPROP_H__7E181A08_4519_4118_B019_AFD19B448BFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseNeuralNetFF.h"
#include "NeuronBProp.h"
#include "WeightBProp.h"

class CNeuralNetBProp : public CBaseNeuralNetFF  
{
public:
	virtual bool BackPropagate(void);
	CNeuralNetBProp();
	virtual ~CNeuralNetBProp();
	virtual bool Propagate(void);
	virtual CBaseNeuron* NewNeuron(void);
	virtual CBaseWeight* NewWeight(void);

	long m_lNumBProp;
protected:
	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);
};
#define NT_NEURALNETBPROP 3

#endif // !defined(AFX_NEURALNETBPROP_H__7E181A08_4519_4118_B019_AFD19B448BFA__INCLUDED_)
