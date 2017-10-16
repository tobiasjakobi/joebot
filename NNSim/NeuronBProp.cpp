// NeuronBProp.cpp: implementation of the CNeuronBProp class.
//
//////////////////////////////////////////////////////////////////////

#include "NeuronBProp.h"
#include "memory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNeuronBProp::CNeuronBProp()
{
	m_dBias = 0;
	m_dDelta = 0;
}

CNeuronBProp::~CNeuronBProp()
{

}

void CNeuronBProp :: Calc(void){
	long lIN;
	CLinkBProp *p;

	m_dNet = 0;

	for(lIN = lLFromNum-1;lIN>-1;lIN--){
		p = (CLinkBProp*)(ppLinksFrom[lIN]);
		m_dNet += ((CWeightBProp*)(p->pW))->m_dWeight * ((CNeuronBProp *)(p->p))->m_dOut;
	}

	m_dAct = m_pfAct(m_dNet - m_dBias);
	m_dOut = m_dAct;				// identity as output function
}
#include <iostream.h>
void CNeuronBProp :: CalcDelta(void){
	if(m_bHidden){
		long lIN;
		CLinkBProp *p;

		m_dDelta = 0;

		for(lIN = lLToNum-1;lIN>-1;lIN--){
			p = (CLinkBProp*)(ppLinksTo[lIN]);
			m_dDelta += ((CWeightBProp*)(p->pW))->m_dWeight * ((CNeuronBProp *)(p->p))->m_dDelta;
		}

		m_dDelta *= m_pfActS(m_dNet);
	}
	else{		// isn't hidden -> is output neuron
		m_dDelta = m_pfActS(m_dNet) * (m_dTI - m_dOut);
	}
}

void CNeuronBProp::Save(FILE *fhd)
{
	CBaseNeuronFF::Save(fhd);
}

void CNeuronBProp::Load(FILE *fhd)
{
	CBaseNeuronFF::Load(fhd);
}

CBaseLink * CNeuronBProp :: NewLink(void){
	return new CLinkBProp;
}
