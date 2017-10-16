// BasePattern.h: interface for the CBasePattern class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEPATTERN_H__B007C9EF_A9EC_4889_A3A8_198D8E60CBEB__INCLUDED_)
#define AFX_BASEPATTERN_H__B007C9EF_A9EC_4889_A3A8_198D8E60CBEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NeuralNetDefs.h"
#include "BasePatternElem.h"

#define _LODL_NORMAL	0
#define _LODL_FINE		1
#define _LODL_1			1

class CBasePattern  
{
public:
	void LogErrors(bool,int iLODL = 0);
	void Log2File(FILE *fhd);
	virtual long GetPatternNum(void);
	CBasePattern();
	virtual ~CBasePattern();
	virtual CBasePatternElem *AddPattern(void);
	virtual CBasePatternElem *NewPattern(void);

	virtual void Save(FILE *fhd);
	virtual void SaveText(FILE *fhd);
	virtual void Load(FILE *fhd);

	virtual bool SaveFile(const char *);
	virtual bool SaveFileText(const char *);
	virtual bool LoadFile(const char *);

	CBasePatternElem *operator [] (long);

	CBasePatternElem **ppPattern;
	long lNumPattern;
	long lMaxPattern;
	bool m_bLogErrors;

	long m_lPTrain;

	FILE *fhdLog;
	int m_iLODL;			// LOD of log
};

#endif // !defined(AFX_BASEPATTERN_H__B007C9EF_A9EC_4889_A3A8_198D8E60CBEB__INCLUDED_)
