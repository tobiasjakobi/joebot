// LinkBProp.cpp: implementation of the CLinkBProp class.
//
//////////////////////////////////////////////////////////////////////

#include "LinkBProp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLinkBProp::CLinkBProp()
{

}

CLinkBProp::~CLinkBProp()
{

}

void CLinkBProp::Save(FILE *fhd)
{
	CBaseLinkFF::Save(fhd);
}

void CLinkBProp::Load(FILE *fhd)
{
	CBaseLinkFF::Load(fhd);
}