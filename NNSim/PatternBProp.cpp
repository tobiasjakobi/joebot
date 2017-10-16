// PatternBProp.cpp: implementation of the CPatternBProp class.
//
//////////////////////////////////////////////////////////////////////

#include "PatternBProp.h"
#include "PatternElemBProp.h"
#include "NeuralNetBProp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPatternBProp::CPatternBProp(){
}

CPatternBProp::~CPatternBProp(){
}

CBasePatternElem *CPatternBProp :: NewPattern(void){
	return new CPatternElemBProp;
}

void CPatternBProp :: TeachEpoch(void){
	long lP;
	CPatternElemBProp *pP;

	m_dMinError = 10000;
	m_dMaxError = -1;
	m_dAvError = 0;

	if(fhdLog && m_iLODL == _LODL_1){		// write log file
		fprintf(fhdLog,"<tr>");
	}

	for(lP = 0;lP < lNumPattern;lP++){
		pP = (CPatternElemBProp *)ppPattern[lP];

		((CNeuralNetBProp *)m_pNN)->SetInput(pP->m_pdIn);
		((CNeuralNetBProp *)m_pNN)->Propagate();
		((CNeuralNetBProp *)m_pNN)->SetTInput(pP->m_pdOut);
		((CNeuralNetBProp *)m_pNN)->BackPropagate();

		if(m_bLogErrors){
			((CNeuralNetBProp *)m_pNN)->GetOutputErrors(pP->m_dMinError,pP->m_dMaxError,pP->m_dAvError);
			if(pP->m_dMinError < m_dMinError)
				m_dMinError = pP->m_dMinError;
			if(pP->m_dMaxError > m_dMaxError)
				m_dMaxError = pP->m_dMaxError;

			if(fhdLog && m_iLODL == _LODL_1){		// write log file
				fprintf(fhdLog,"<td>%lf",pP->m_dMaxError);
			}
		}
	}
	if(fhdLog && m_iLODL != _LODL_1){		// write log file
		fprintf(fhdLog,"<tr><td><b>%li</b><td>%lf<td>%lf<td>%lf</tr>",lP,m_dMinError,m_dMaxError,-1);
	}
}

void CPatternBProp :: GetErrors(void){
	long lP;
	CPatternElemBProp *pP;

	m_dMinError = 10000;
	m_dMaxError = -1;
	m_dAvError = 0;

	for(lP = 0;lP < lNumPattern;lP++){
		pP = (CPatternElemBProp *)ppPattern[lP];
		((CNeuralNetBProp *)m_pNN)->SetInput(pP->m_pdIn);
		((CNeuralNetBProp *)m_pNN)->Propagate();
		((CNeuralNetBProp *)m_pNN)->SetTInput(pP->m_pdOut);

		if(m_bLogErrors){
			((CNeuralNetBProp *)m_pNN)->GetOutputErrors(pP->m_dMinError,pP->m_dMaxError,pP->m_dAvError);
			if(pP->m_dMinError < m_dMinError)
				m_dMinError = pP->m_dMinError;
			if(pP->m_dMaxError > m_dMaxError)
				m_dMaxError = pP->m_dMaxError;
		}
	}
}

void CPatternBProp :: TeachPartoEpoch(long lNumoP){
	long lS;
	CPatternElemBProp *pP;

	m_dMinError = 10000;
	m_dMaxError = -1;
	m_dAvError = 0;

	for(lS = 0;lS < lNumoP;m_lPTrain++,lS++){
		if(m_lPTrain > lNumPattern)
			m_lPTrain = 0;
		pP = (CPatternElemBProp *)ppPattern[m_lPTrain];
		((CNeuralNetBProp *)m_pNN)->SetInput(pP->m_pdIn);
		((CNeuralNetBProp *)m_pNN)->Propagate();
		((CNeuralNetBProp *)m_pNN)->SetTInput(pP->m_pdOut);
		((CNeuralNetBProp *)m_pNN)->BackPropagate();

		if(m_bLogErrors){
			((CNeuralNetBProp *)m_pNN)->GetOutputErrors(pP->m_dMinError,pP->m_dMaxError,pP->m_dAvError);
			if(pP->m_dMinError < m_dMinError)
				m_dMinError = pP->m_dMinError;
			if(pP->m_dMaxError > m_dMaxError)
				m_dMaxError = pP->m_dMaxError;
		}
	}
}

void CPatternBProp :: Load(FILE *fhd){
	CBasePatternFF::Load(fhd);
}

void CPatternBProp :: Save(FILE *fhd){
	CBasePatternFF::Save(fhd);
}
