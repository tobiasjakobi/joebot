#ifndef __SOMPATTERNELEM_H
#define __SOMPATTERNELEM_H

// SOMPatternElem.h by Johannes Lampel / LampelSoft
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

#include "nVec.h"

class DllExport SOMPatternElem:protected nVec{
	friend class SOMPattern;
public:
	SOMPatternElem();			// ctor
	~SOMPatternElem();			// dtor
protected:
	double dLastError;			// just the last error, which has been calced last m::cat
private:
};

#endif  // __SOMPATTERNELEM_H
