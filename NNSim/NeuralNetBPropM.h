// NeuralNetBPropM.h: interface for the CNeuralNetBPropM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEURALNETBPROPM_H__CF74ACEE_B4F9_4240_8345_02294A4B122F__INCLUDED_)
#define AFX_NEURALNETBPROPM_H__CF74ACEE_B4F9_4240_8345_02294A4B122F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NeuralNetBProp.h"
#include "NeuronBPropM.h"
#include "WeightBPropM.h"
#include "LinkBPropM.h"

class CNeuralNetBPropM : public CNeuralNetBProp  
{
public:
	CNeuralNetBPropM();
	virtual ~CNeuralNetBPropM();
	void SetMomentum(const netvar_t &);
	netvar_t GetMomentum(void);

	virtual bool BackPropagate(void);

	virtual CBaseNeuron* NewNeuron(void);
	virtual CBaseWeight* NewWeight(void);
protected :
	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);

	netvar_t m_dMomentum;
};

#define NT_NEURALNETBPROPM 4

#endif // !defined(AFX_NEURALNETBPROPM_H__CF74ACEE_B4F9_4240_8345_02294A4B122F__INCLUDED_)
