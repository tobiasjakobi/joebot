// BaseNeuronFF.h: interface for the CBaseNeuronFF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASENEURONFF_H__F2218CE5_6870_431B_BE26_CBD27ABA4993__INCLUDED_)
#define AFX_BASENEURONFF_H__F2218CE5_6870_431B_BE26_CBD27ABA4993__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseNeuron.h"
#include "BaseLinkFF.h"
class CGenom;

class CBaseNeuronFF : public CBaseNeuron  
{
	friend class CBaseNeuralNetFF;
	friend class CNeuralNetBProp;
	friend class CNeuralNetBPropM;
	friend class CGenom;
	friend void InitObjects(void);
public:
	virtual void Save(FILE *);
	virtual void Load(FILE *);
	CBaseNeuronFF();
	virtual ~CBaseNeuronFF();

	virtual CBaseLink *AddLinkFrom(long lW, CBaseWeight *pW,CBaseNeuron *);
	virtual CBaseLink *AddLinkTo(long lW, CBaseWeight *pW,CBaseNeuron *);

	virtual CBaseLink *NewLink(void) = 0;

	virtual void Calc(void) = 0;
	virtual void CalcDelta(void) = 0;
	virtual void InitWeights(netvar_t,netvar_t);
	void SetBias(netvar_t &);

	CBaseLink *getLinkTo(CBaseNeuron *);
protected:
	long lLToNum;
	long lLToMax;
	CBaseLink **ppLinksTo;
	long lLFromNum;
	long lLFromMax;
	CBaseLink **ppLinksFrom;

	netvar_t	m_dBias;		// bias
	netvar_t	m_dNet;			// net
	netvar_t	m_dAct;			// activation
	netvar_t	m_dOut;			// output
	netvar_t	m_dDelta;		// delta
	netvar_t	m_dTI;
	bool		m_bHidden;

	int m_iTypeAF;				// type of activation function
	netvar_t (*m_pfAct)	(const netvar_t &);		// activation function
	netvar_t (*m_pfActS)	(const netvar_t &);		// first derivative of activation function
};

#endif // !defined(AFX_BASENEURONFF_H__F2218CE5_6870_431B_BE26_CBD27ABA4993__INCLUDED_)
