// BaseNeuralNetFF.h: interface for the CBaseNeuralNetFF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASENEURALNETFF_H__E9D90CC8_2CE2_4656_8566_78FB6089E245__INCLUDED_)
#define AFX_BASENEURALNETFF_H__E9D90CC8_2CE2_4656_8566_78FB6089E245__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseNeuralNet.h"
#include "BaseNeuron.h"
#include "BaseNeuronFF.h"
#include "BaseWeightFF.h"

class CBaseNeuralNetFF : public CBaseNeuralNet  
{
	friend void InitObjects(void);
public:
	virtual void GetOutputErrors(netvar_t &,netvar_t &,netvar_t &);
	void SetLRate(const netvar_t &);
	netvar_t GetLRate(void);
	virtual void InitWeights(netvar_t, netvar_t);
	virtual long AddWeight(void);
	virtual void CopyTI2N(void);
	virtual bool SetTInput(netvar_t *);
	virtual void GetOutput(netvar_t *);
	virtual void SetInput(netvar_t *);
	virtual long GetConnectionNum(void);
	virtual long GetWeightNum(void);
	virtual long GetOutputNum(void);
	virtual long GetNeuronNum(void);
	virtual long GetInputNum(void);
	virtual bool ConnectNeurons(long,long,long,long,long);
	virtual bool ConnectLayer(long,long);
	virtual bool SetNNeuronsOnLayer(long, long);
	long lLayerNum;
	virtual bool SetLayerNum(long);
	virtual bool AllocNeurons(void);
	virtual bool Propagate(void) = 0;
	CBaseNeuralNetFF();
	virtual ~CBaseNeuralNetFF();
	
	void GetNPosition(CBaseNeuron *p, long &lL,long &lN);
	CBaseNeuronFF *GetNeuron(long,long);
	CBaseNeuronFF *GetNeuron(long);
	void RecalcPositions(void);

	virtual CBaseNeuron* NewNeuron(void) = 0;
	virtual CBaseWeight* NewWeight(void) = 0;

	long m_lNumProp;
	float *m_pfNDist;
	long *m_plNPR;
//protected :
	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);

	long *plLNNum;
	//long lMaxNpLayer;
	CBaseNeuron ***pppNeurons;
	netvar_t *m_pTI;			// teaching input
	netvar_t m_dLRate;
};
#define NT_BASENEURALNETFF 2

#endif // !defined(AFX_BASENEURALNETFF_H__E9D90CC8_2CE2_4656_8566_78FB6089E245__INCLUDED_)
