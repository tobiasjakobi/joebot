// NeuronBPropM.cpp: implementation of the CNeuronBPropM class.
//
//////////////////////////////////////////////////////////////////////

#include "NeuronBPropM.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNeuronBPropM::CNeuronBPropM()
{
	m_dBiasMomentum = 0;
}

CNeuronBPropM::~CNeuronBPropM()
{

}

void CNeuronBPropM::Save(FILE *fhd)
{
	CBaseNeuronFF::Save(fhd);
	fwrite(&m_dBiasMomentum,sizeof(netvar_t),1,fhd);
}

void CNeuronBPropM::Load(FILE *fhd)
{
	CBaseNeuronFF::Load(fhd);
	fread(&m_dBiasMomentum,sizeof(netvar_t),1,fhd);
}

CBaseLink * CNeuronBPropM :: NewLink(void){
	return new CLinkBPropM;
}
