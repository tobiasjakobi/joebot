// Genom.h: interface for the CGenom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENOM_H__FEF79371_59D6_4D54_97A9_C3C3DDB6961E__INCLUDED_)
#define AFX_GENOM_H__FEF79371_59D6_4D54_97A9_C3C3DDB6961E__INCLUDED_

#include "NeuralNetBProp.h"
#include "NeuralNetBPropM.h"
#include "PatternBProp.h"
#include "memory.h"
#include "string.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGenom  
{
public:
	CGenom();
	virtual ~CGenom();
	void Init(void);
	bool CreateNet(CBaseNeuralNet **p);			// create a net based on the genetic information stored
	bool SaveDescription(const char *szName);
	bool GetFFNet(CBaseNeuralNetFF *p);		// create genetic information from the net
	bool Save(const char *szFileName, bool bRemoveExt = false);
	bool SaveTopData(const char *szFileNameP);
	bool SaveBiases(const char *szFileNameP);
	bool SaveWeights(const char *szFileNameP);
	bool Load(const char *szFileName, bool bRemoveExt = false);
	bool LoadTopData(const char *szFileNameP);
	bool LoadBiases(const char *szFileNameP);
	bool LoadWeights(const char *szFileNameP);
	void WannaAddToken(int iSize=1);
	void WannaAddWeight(int iSize=1);
	void WannaAddBias(int iSize=1);
	void Mutation(float fPropability, float fRange);
	bool Crossing(float fCut, CGenom &GOtherGenom);
//protected:
	long lNumTopData;
	long lNumTopDataAlloc;
	short *psTopData;
	long lNumWeights;
	long lNumWeightsAlloc;
	netvar_t *pWeights;
	long lNumBiases;
	long lNumBiasesAlloc;
	netvar_t *pBiases;
};

struct GenomFileHeader_s{
	char szDescription[32];
	int iType;
	int iVersion;
};

#define __CGENOMVERSION 1
#define _FILEEXT_TOPDATA "tpd"
#define _TOP_DATA 1
#define _FILEEXT_BIASES "bia"
#define _BIAS_DATA 2
#define _FILEEXT_WEIGHTS "wgs"
#define _WEIGHT_DATA 3

#endif // !defined(AFX_GENOM_H__FEF79371_59D6_4D54_97A9_C3C3DDB6961E__INCLUDED_)
