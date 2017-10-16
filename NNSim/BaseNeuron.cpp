// BaseNeuron.cpp: implementation of the CBaseNeuron class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseNeuron.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseNeuron::CBaseNeuron()
{
	m_fX = 0;
	m_fY = 0;
}

CBaseNeuron::~CBaseNeuron()
{
	
}

void CBaseNeuron::Save(FILE *fhd)
{
	fwrite(&m_fX,sizeof(float),1,fhd);
	fwrite(&m_fY,sizeof(float),1,fhd);
}

void CBaseNeuron::Load(FILE *fhd)
{
	fread(&m_fX,sizeof(float),1,fhd);
	fread(&m_fY,sizeof(float),1,fhd);
}
