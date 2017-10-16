#ifndef __SOMPATTERN_H
#define __SOMPATTERN_H

// SOMPattern.h by Johannes Lampel /LampelSoft
// (c) Johannes Lampel / LampelSoft
// written Sep 27th 2000

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/

#include "GlobalCqSOMDefs.h"
#include "nVec.h"
#include "SOMPatternElem.h"
#include "CqSOM2d.h"
#include "SOMPatternErr.h"

class DllExport SOMPattern{
public:
	SOMPattern(const dim_c);							// ctor
	~SOMPattern();										// dtor
	int SetMaxPatternNum(const long);					// max Number of Pattern - not yet able to copy data from old array to new and no dynamic alloc
	long AddPattern(const double *);					// ...
	long AddPattern(const nVec &);						// ... (-> calling AddPattern(const double *)
	int SetSOM(qSOM2d *);								// setting som to be trained
	int TeachEpoch(const double&,const double&);		// Teaching a full number of pattern and storing the errors ( return values of qSOM2d :: Categorize ) in a pattern related variable
	int TeachEpochFast(const double &,const double &);	// same as above, only using qSOM2d::CategorizeFast
	long GetNumP(void){return lPNum;};

	int Load(const char *);
	int Load(FILE *fhd);
	int Save(const char *);
	int Save(FILE *fhd);
protected:
	SOMPatternElem *pFPattern;							// Field of PatternElem instances with static size
	long lPDimNum;										// Number of dimensions of every Pattern
	long lPNum;											// Number of pattern
	long lActPPos;										// Actual position of last elem ( lActPPos - 1 )

	qSOM2d * SActSOM;									// som to be trained
private:
};

#endif  // __SOMPATTERN_H
