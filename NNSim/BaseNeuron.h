// BaseNeuron.h: interface for the CBaseNeuron class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASENEURON_H__FE91BF63_C4AB_47B6_9525_C563A6E3D2C3__INCLUDED_)
#define AFX_BASENEURON_H__FE91BF63_C4AB_47B6_9525_C563A6E3D2C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NeuralNetDefs.h"
#include "BaseLink.h"
#include "Activations.h"
#include "stdio.h"

class CBaseNeuron  
{
	friend class CBaseNeuralNet;
public:
	virtual void Load(FILE *);
	virtual void Save(FILE *);
	CBaseNeuron();
	virtual ~CBaseNeuron();
	virtual void Calc(void) = 0;
	virtual void InitWeights(netvar_t,netvar_t) = 0;

	virtual CBaseLink *NewLink(void) = 0;

	float m_fX,m_fY;			// position on layer ( layers are on z )
};

#endif // !defined(AFX_BASENEURON_H__FE91BF63_C4AB_47B6_9525_C563A6E3D2C3__INCLUDED_)
