// NeuronBPropM.h: interface for the CNeuronBPropM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEURONBPROPM_H__396EAD46_E392_43F4_9A11_85B3799E2CB6__INCLUDED_)
#define AFX_NEURONBPROPM_H__396EAD46_E392_43F4_9A11_85B3799E2CB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NeuronBProp.h"
#include "LinkBPropM.h"

class CNeuronBPropM : public CNeuronBProp  
{
public:
	CNeuronBPropM();
	virtual ~CNeuronBPropM();

	virtual void Load(FILE *);
	virtual void Save(FILE *);

	virtual CBaseLink *NewLink(void);

	netvar_t m_dBiasMomentum;
};

#endif // !defined(AFX_NEURONBPROPM_H__396EAD46_E392_43F4_9A11_85B3799E2CB6__INCLUDED_)
