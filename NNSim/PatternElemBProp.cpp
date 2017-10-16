// PatternElemBProp.cpp: implementation of the CPatternElemBProp class.
//
//////////////////////////////////////////////////////////////////////

#include "PatternElemBProp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPatternElemBProp::CPatternElemBProp()
{

}

CPatternElemBProp::~CPatternElemBProp()
{

}

void CPatternElemBProp :: Load( FILE *fhd){
	CBasePatternElemFF::Load(fhd);
}

void CPatternElemBProp :: Save( FILE *fhd){
	CBasePatternElemFF::Save(fhd);
}

void CPatternElemBProp :: LoadText( FILE *fhd){
	CBasePatternElemFF::LoadText(fhd);
}

void CPatternElemBProp :: SaveText( FILE *fhd){
	CBasePatternElemFF::SaveText(fhd);
}
