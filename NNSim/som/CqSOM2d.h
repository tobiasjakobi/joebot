#ifndef __CQSOM2D_H
#define __CQSOM2D_H

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 ***************************************************************************/

// class for 2d SOMs
// by Johannes Lampel / LampelSoft
// (c) Johannes Lampel / LampelSoft
// written Sept 13rd 2000

#include "GlobalCqSOMDefs.h"
#include "nVec.h"
#include "nSOMErr.h"
#include "fNBorKernel.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <iostream.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <float.h>

#define _SOM_ID 0x20000927					// ID for som - files

class DllExport qSOM2d{
public:
	qSOM2d();									// ctor
	~qSOM2d();									// dtor

	int SetSize(long,long,long);				// set size of map (Dim of Input,x,y)	-> x=y isn't too useful
	int InitWeights(InitType);					// ...
	int SetInput(double *);						// copies input into local class storage
	double Categorize(void);						// cats input
	double Categorize(double*);					// like the two above
	double CategorizeFast(void);					// cats faster cause it neglects some (!) neurons
	double CategorizeFast(double*);				// ...
	void GetWinner(long &,long &);				// finds 'nearest' neuron
	double *GetNWeights(long,long);				// return the weight matrix of one neuron
	double GetDistP(void);						// return stiffness param
	void SetDistP(double);						// ..
	double GetLRate(void);						// return lRate
	void SetLRate(double);						// ..
	void SetNKFunction(double (*name)(const double&,const double&));	// sets the pointer to the NK functions, which is set to fNKgauss1 by default
	void *GetNKFunction(void);					// ..
	void Save(const char *);					// savin' data to file
	void Load(const char *);					// loadin' data from file to mem
	long lXSize,lYSize,lNNeurons;
protected:
	long lRow;									// lXSize * lNNeurons	(just performance)
	double *dp3Weights;							// field of weights
	double *dp1Input;							// input to be categorized or tested
	double dLearnrate;							// ...
	double dDistanceP;							// stiffness param
	char *szVersion;							// Just some information about this simulator
	double (*fNK)(const double&,const double&); // pointer to NK function
	nVec	*pVDiff;							// diff (updated by calling cat) between Input and Winner
private:
};

#endif  // __CQSOM2D_H
