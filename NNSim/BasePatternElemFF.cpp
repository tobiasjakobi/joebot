// BasePatternElemFF.cpp: implementation of the CBasePatternElemFF class.
//
//////////////////////////////////////////////////////////////////////

#include "BasePatternElemFF.h"
#include <memory.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBasePatternElemFF::CBasePatternElemFF()
{
	m_pdIn = 0;
	m_pdOut = 0;
}

void CBasePatternElemFF :: SetDim(long lDimIn,long lDimOut){
	if(m_pdIn)
		delete [] m_pdIn;
	if(m_pdOut)
		delete [] m_pdOut;

	m_pdIn = new netvar_t[lDimIn];
	m_pdOut = new netvar_t[lDimOut];

	m_lInDim = lDimIn;
	m_lOutDim = lDimOut;
}

CBasePatternElemFF::~CBasePatternElemFF()
{
	if(m_pdIn)
		delete [] m_pdIn;
	if(m_pdOut)
		delete [] m_pdOut;
}

void CBasePatternElemFF :: SetInput(netvar_t *p){
	memcpy(m_pdIn,p,sizeof(netvar_t)*m_lInDim);
}

void CBasePatternElemFF :: SetOutput(netvar_t *p){
	memcpy(m_pdOut,p,sizeof(netvar_t)*m_lOutDim);
}

void CBasePatternElemFF :: Load( FILE *fhd){
	CBasePatternElem::Load(fhd);

	fread(&m_lInDim,sizeof(long),1,fhd);
	fread(&m_lOutDim,sizeof(long),1,fhd);

	SetDim(m_lInDim,m_lOutDim);	//alloc mem

	fread(m_pdIn,sizeof(netvar_t),m_lInDim,fhd);
	fread(m_pdOut,sizeof(netvar_t),m_lOutDim,fhd);
}

void CBasePatternElemFF :: Save( FILE *fhd){
	CBasePatternElem::Save(fhd);

	fwrite(&m_lInDim,sizeof(long),1,fhd);
	fwrite(&m_lOutDim,sizeof(long),1,fhd);

	fwrite(m_pdIn,sizeof(netvar_t),m_lInDim,fhd);
	fwrite(m_pdOut,sizeof(netvar_t),m_lOutDim,fhd);
}

void CBasePatternElemFF :: LoadText( FILE *fhd){
	CBasePatternElem::LoadText(fhd);

	fread(&m_lInDim,sizeof(long),1,fhd);
	fread(&m_lOutDim,sizeof(long),1,fhd);

	SetDim(m_lInDim,m_lOutDim);	//alloc mem

	fread(m_pdIn,sizeof(netvar_t),m_lInDim,fhd);
	fread(m_pdOut,sizeof(netvar_t),m_lOutDim,fhd);
}

void CBasePatternElemFF :: SaveText( FILE *fhd){
	CBasePatternElem::SaveText(fhd);

	long lschl;
	fprintf(fhd,"I%li\nO%li\n{",m_lInDim,m_lOutDim);

	for(lschl = 0;lschl < m_lInDim;lschl++){
		fprintf(fhd,"%lf, ",m_pdIn[lschl]);
	}
	fprintf(fhd,"\n");
	for(lschl = 0;lschl < m_lOutDim;lschl++){
		fprintf(fhd,"%lf, ",m_pdOut[lschl]);
	}
	fprintf(fhd,"\n}\n\n");
}
