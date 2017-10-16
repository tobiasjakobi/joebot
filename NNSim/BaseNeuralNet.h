// BaseNeuralNet.h: interface for the CBaseNeuralNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASENEURALNET_H__F35839F1_1A0E_49CC_B8B1_559F0886A887__INCLUDED_)
#define AFX_BASENEURALNET_H__F35839F1_1A0E_49CC_B8B1_559F0886A887__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NeuralNetDefs.h"
#include "BaseWeight.h"
#include "BaseNeuron.h"

#define _LTYPEI 32

class CBaseNeuralNet  
{
public:
	netvar_t GetLDist(void);
	void SetLDist(const netvar_t &dParam);
	virtual bool LoadFile(const char *szFileName);
	virtual bool SaveFile(const char *szFileName);
	virtual void InitWeights(netvar_t,netvar_t);
	virtual char * Version(void);
	virtual long AddWeight(void) = 0;
	virtual long GetWeightNum(void)=0;
	CBaseNeuralNet();
	virtual ~CBaseNeuralNet();
	char *GetTONet(void);
	int GetITONet(void);
	virtual CBaseNeuron* NewNeuron(void) = 0;
	virtual CBaseWeight* NewWeight(void) = 0;
	CBaseWeight* GetWeight(long lW);
protected:
	virtual void Save(FILE *fhd);
	virtual void Load(FILE *fhd);

	char * szVersion;

	long lMaxWeights;			// number of allocated weigths
	long lNumWeights;			// Number of weigths
	CBaseWeight **ppWeights;		// the weigths themselves

	char m_szType[_LTYPEI];
	short m_iType;
	netvar_t m_fNLDist;			// distances of layers
};

#define NT_BASENEURALNET 1

#endif // !defined(AFX_BASENEURALNET_H__F35839F1_1A0E_49CC_B8B1_559F0886A887__INCLUDED_)
