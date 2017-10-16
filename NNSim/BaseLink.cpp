// BaseLink.cpp: implementation of the CBaseLink class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseLink.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseLink::CBaseLink()
{
	lWIndex = 0;
	pW = 0;
}

CBaseLink::~CBaseLink()
{

}

void CBaseLink::Save(FILE *fhd)
{
	fwrite(&lWIndex,sizeof(long),1,fhd);
}

void CBaseLink::Load(FILE *fhd)
{
	fread(&lWIndex,sizeof(long),1,fhd);
}
