#ifndef __NVEC_H
#define __NVEC_H

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/

// n-dim vectorclass
// by Johannes Lampel / LampelSoft
// (c)  copyright Johannes Lampel
// written Sept 13rd 2000

#include "GlobalCqSOMDefs.h"
#include "nVec.h"
#include "nVecErr.h"
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <iostream.h>

enum nVecTypes{
	nVT_copy=0,
	nVT_mem
};

class nVec;

nVec operator *(double,nVec&);

class DllExport nVec{
	friend nVec operator *(double,nVec&);
	friend class SOMPattern;
public:
	nVec();
	nVec(const dim_c);
	nVec(const dim_c,const double * const);
	nVec(const nVec &);
	virtual ~nVec();

	double	Length(void);							// ...
	int		Normalize(void);						// ...
	int		GetVec(double *);						// copy data from field into this class
	int		SetMemVec(double *);					// set ddpDim to Param and iType to nVT_mem - be careful, you havn't a copy of this data, your changing the original data
	int		ConvMemtoCopy(void);						// convert a Mem - typed instance of this class to a instance, which contains a copy of this data items - alloc mem and copy
	int		SetVec(double *);						// copy data from class to field
	double	SLength(void);							// return sum of all quadrs of comps
	double	CBLength(void);							// return CBDist
	void	Zero(void);
	int		SetNumDim(const dim_c);

	int		Load(FILE *);
	int		Save(FILE *);

	virtual const nVec & operator = (const nVec &);	// ...
	virtual const nVec & operator +=(const nVec &);	// ...
	virtual const nVec & operator -=(const nVec &);	// ...
	virtual const nVec & operator *=(const double);	// ...
	virtual const nVec & operator /=(const double);	// ...

	virtual nVec		operator +(const nVec &);	// ...
	virtual nVec		operator -(const nVec &);	// ...
	virtual nVec		operator *(const double);	// ...
	virtual nVec		operator /(const double);	// ...

	virtual double &	operator [](long);			// return value of [dimension] component
protected:
	double *ppdDim;									// data items
	long lCDim;										// number of dimensions
	int iType;										// copy data or set pointer to data
private:
};

#endif  // __NVEC_H
