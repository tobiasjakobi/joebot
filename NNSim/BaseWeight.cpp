// BaseWeight.cpp: implementation of the CBaseWeight class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseWeight.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseWeight::CBaseWeight()
{
	lUsed = 0;
}

CBaseWeight::~CBaseWeight()
{

}

const CBaseWeight &CBaseWeight ::  operator = (const CBaseWeight &){
	return ((const CBaseWeight &)(*this));
}

void CBaseWeight::Load(FILE *fhd){
}

void CBaseWeight::Save(FILE *fhd){
}
