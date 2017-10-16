#include "CqSOM2d.h"

qSOM2d :: qSOM2d(){
	char szTemp[] = "LampelSoft SOM Simulator 0.2 by Johannes Lampel\n";
	szVersion = new char[strlen(szTemp)+1];
	strcpy(szVersion,szTemp);
	
	dp3Weights	= 0;
	dp1Input	= 0;
	pVDiff		= 0;
	dLearnrate	= 0.0;
	dDistanceP	= 0.0;

	// init pointer to Neighborhoodkernel function
	fNK = fNKgauss1;

	// INIT RAND
	srand(time(0));
}

qSOM2d :: ~qSOM2d(){
	if(dp3Weights)	delete [] dp3Weights;
	if(dp1Input)	delete [] dp1Input;
	if(szVersion)	delete [] szVersion;
	if(pVDiff)		delete pVDiff;

	dp3Weights	= 0;
	dp1Input	= 0;
	szVersion	= 0;
	pVDiff		= 0;
}

int qSOM2d :: SetSize(long lINeurons,long lpXSize,long lpYSize){
	lNNeurons	= lINeurons;
	lXSize		= lpXSize;
	lYSize		= lpYSize;
	lRow		= lXSize * lNNeurons;
	
	// alloc mem for weights - 3dim
	// usage : weigth := weights[ly * lXSize * lNNeurons + lx * lNNeurons + lN]
	// lXSize * lNNeurons := lRow
	
	if(dp3Weights) delete [] dp3Weights;
	dp3Weights = new double[lNNeurons * lXSize * lYSize];
	
	// alloc mem for input neurons
	
	if(dp1Input) delete [] dp1Input;
	dp1Input = new double [lNNeurons];

	// Alloc space for VDiff - Diff between input and winner vector

	if(pVDiff) delete pVDiff;
	pVDiff = new nVec(lNNeurons);
	
	return 1;
}

int qSOM2d :: InitWeights(InitType IType){
	long lschl;
	switch (IType){
	case Init_Random:
		for(lschl=0;lschl < lNNeurons * lXSize * lYSize;lschl++){
			dp3Weights[lschl] = double(rand()) / RAND_MAX;
		}
		return 0;
	default:
		cerr << "\"What's that\" - InitWeights"<<endl;
		return 1;
	}
}

int qSOM2d :: SetInput(double *dptr){
	memcpy(dp1Input,dptr,sizeof(double) * lNNeurons);
	return 0;
}

void qSOM2d :: GetWinner(long &lxMin,long &lyMin){			// GetNearest Neuron to Input
	try{
		if(dp1Input){
			// set vars to 0
			// this has to be done, 'cause sometimes ( or only with optimized compilin' with MS C++6.0 he cannot find the nearest Neuron !!!??) this function isn't able to find minima and would otherwise cause a page fault
			lyMin = lxMin = 0;
			
			long lxschl,lyschl;
			double dMin = DBL_MAX;	// dist of best neuron
			double dLength;
			long lsP;
			
			nVec VInput (lNNeurons);
			nVec VComp	(lNNeurons);
			nVec VDiff	(lNNeurons);
			
			VInput.SetMemVec(dp1Input);			// don't copy data - only set pointer
			
			for(lxschl = 0;lxschl < lXSize;lxschl++){
				lsP = lxschl * lNNeurons;
				for(lyschl = 0;lyschl < lYSize;lyschl++){
					VComp.GetVec(&dp3Weights[lyschl * lRow + lsP]);
					VComp -= VInput;
					dLength = VComp.SLength();
					
					if(dLength < dMin){
						dMin = dLength;
						lxMin = lxschl;
						lyMin = lyschl;
					}
				}
			}
		}
		else{
			throw NoInput();
		}
	}
	catch(NoInput NI){
		cerr << "No Input specified" << endl;
		throw NI;
	}
}

double qSOM2d :: Categorize(double *dpParam){
	SetInput(dpParam);
	return Categorize();
}

double qSOM2d :: Categorize(void){		// learning function
	long lxWinner=0,lyWinner=0;
	long lxschl,lyschl;
	double ddistance;
	long lDx,lDy;
	nVec	VIn(lNNeurons);			// input vector
	nVec	VNeuron(lNNeurons);		// vector of current neuron
	nVec	VTemp(lNNeurons);		// temp
	double	*dpCVec;
	
	GetWinner(lxWinner,lyWinner);							// getting winner neuron
	VIn.GetVec(dp1Input);									// get input to vector

	pVDiff->GetVec(GetNWeights(lxWinner,lyWinner));
	*pVDiff -= VIn;
	
	for(lxschl=0;lxschl < lXSize;lxschl++){
		for(lyschl=0;lyschl < lYSize;lyschl++){
			// get current weights
			dpCVec = &dp3Weights[lyschl * lRow + lxschl * lNNeurons];
			// get distance to winner
			lDx = lxschl - lxWinner;
			lDy = lyschl - lyWinner;
			ddistance = sqrt(lDx * lDx + lDy * lDy);
			// get weights from current neuron
			VNeuron.SetMemVec(dpCVec);
			// update weights
			
			VTemp = VIn;
			VTemp -= VNeuron;
			VTemp *= (dLearnrate * fNK(ddistance,dDistanceP));
			VNeuron += VTemp;
			// don't have to write updated vector back to mem cause it was calced in mem (VNeuron.SetMemVec)
		}
	}
	return pVDiff->Length();
}

double qSOM2d :: CategorizeFast(double *dpParam){
	SetInput(dpParam);
	return CategorizeFast();
}

double qSOM2d :: CategorizeFast(void){
/* fast learning function -> not suitable for gauss1 + 2  -> only for cone,cyl and cos because for the gauss' the result of them isn't 0 when z > d
This function neglects the neurons outside a square defined by the winner neuron as middle and the min radius by the 'dDistanceP' The distance or stiffness param
*/

	long lxWinner,lyWinner;
	long lxschl,lyschl;
	double ddistance;
	long lDx,lDy;
	nVec	VIn(lNNeurons);			// input vector
	nVec	VNeuron(lNNeurons);		// vector of current neuron
	nVec	VUpdated(lNNeurons);	// updated vector
	nVec	VTemp(lNNeurons);		// temp
	double	*dpCVec;
	long lxstart,lxend,lystart,lyend;
	
	GetWinner(lxWinner,lyWinner);							// getting winner neuron
	VIn.GetVec(dp1Input);									// get input to vector

	pVDiff->GetVec(GetNWeights(lxWinner,lyWinner));
	*pVDiff -= VIn;

	// calc vars for only changing weights in neurons which are in the radius of dDistanceP
	if(dDistanceP < 1.0){					// no other neurons are affected
		lxstart = lxWinner;
		lxend = lxstart + 1;
		lystart = lyWinner;
		lyend = lystart + 1;
	}
	else{									// otherwise test ...
		lxstart = long(lxWinner - dDistanceP - 1);
		lxend	= long(lxWinner + dDistanceP + 1);
		lystart = long(lyWinner - dDistanceP - 1);
		lyend	= long(lyWinner + dDistanceP + 1);
		if(lxstart < 1){					// if outside of the borders
			lxstart = 0;
		}
		if(lystart < 1){
			lystart = 0;
		}
		if(lxend > lXSize){
			lxend = lXSize;
		}
		if(lyend > lYSize){
			lyend = lYSize;
		}
	}
	
	for(lxschl=lxstart;lxschl < lxend;lxschl++){
		for(lyschl=lystart;lyschl < lyend;lyschl++){
			// get current weights
			dpCVec = &dp3Weights[lyschl * lRow + lxschl * lNNeurons];
			// get distance to winner
			lDx = lxschl - lxWinner;
			lDy = lyschl - lyWinner;
			ddistance = sqrt(lDx * lDx + lDy * lDy);
			// get weights from current neuron
			VNeuron.SetMemVec(dpCVec);
			// update weights
			VTemp = VIn;
			VTemp -= VNeuron;
			VTemp *= (dLearnrate * fNK(ddistance,dDistanceP));
			VNeuron += VTemp;
			// don't have to write updated vector back to mem cause it was calced in mem (VNeuron.SetMemVec)
		}
	}
	return pVDiff->Length();
}

double *qSOM2d :: GetNWeights(long lx,long ly){
	try{
		if(lx < 0 ||ly < 0)
			throw FuckinParam();
		return &dp3Weights[ly * lRow + lx * lNNeurons];
	}
	catch(FuckinParam FP){
		cerr << "Got some fuckin' params in qSOM2d :: GetNWeights"<<endl;
		throw FP;
	}
}

double qSOM2d :: GetDistP(void){
	return dDistanceP;
}

void qSOM2d :: SetDistP(double dParam){
	dDistanceP = dParam;
}

double qSOM2d :: GetLRate(void){
	return dLearnrate;
}

void qSOM2d :: SetLRate(double dParam){
	dLearnrate = dParam;
}

void qSOM2d :: Save(const char *szFileName){
	FILE *fhd;
	long lCoding = _SOM_ID;
	try{
		if((fhd = fopen(szFileName,"wb"))){
			fwrite(&lCoding,sizeof(long),1,fhd);			// writing id for somfiles
			fwrite(&lXSize,sizeof(long),1,fhd);
			fwrite(&lYSize,sizeof(long),1,fhd);
			fwrite(&lNNeurons,sizeof(long),1,fhd);
			fwrite(&lRow,sizeof(long),1,fhd);
			fwrite(&dLearnrate,sizeof(double),1,fhd);
			fwrite(&dDistanceP,sizeof(double),1,fhd);
			fwrite(dp3Weights,sizeof(double),lXSize * lYSize * lNNeurons,fhd);
			fclose(fhd);
		}
		else
		{
			throw FileSaveError();
		}
	}
	catch(FileSaveError FSE){
		cerr << "Error opening file : " << szFileName << endl;
		throw FSE;
	}
}

void qSOM2d :: Load (const char *szFileName){
	FILE *fhd;
	long lCoding;
	try{
		if((fhd=fopen(szFileName,"rb"))){
			if(dp3Weights)	delete [] dp3Weights;
			if(dp1Input)	delete [] dp1Input;
			
			fread(&lCoding,sizeof(long),1,fhd);					// read coding and verify
			if(lCoding != _SOM_ID)
				throw UnknownFileFormat();
			fread(&lXSize,sizeof(long),1,fhd);					// just as save - vice versa
			fread(&lYSize,sizeof(long),1,fhd);
			fread(&lNNeurons,sizeof(long),1,fhd);
			fread(&lRow,sizeof(long),1,fhd);
			
			fread(&dLearnrate,sizeof(double),1,fhd);
			fread(&dDistanceP,sizeof(double),1,fhd);
			
			dp3Weights	= new double [lXSize * lYSize * lNNeurons];
			dp1Input	= new double [lNNeurons];
			
			fread(dp3Weights,sizeof(double),lXSize * lYSize * lNNeurons,fhd);
			memset(dp1Input,0,sizeof(double) * lNNeurons);
			fclose(fhd);
		}
		else
		{
			throw FileLoadError();
		}
	}
	catch(FileLoadError FLE){
		cerr << "Error opening file : " << szFileName << endl;
		throw FLE;
	}
	catch(UnknownFileFormat UFF){
		cerr << "Unknown file format" << endl;
		throw UFF;
	}
}

void qSOM2d :: SetNKFunction(double (*fNKParam)(const double&,const double&)){
	fNK = fNKParam;
}

void *qSOM2d :: GetNKFunction(void){
	return ((void *)(fNK));
}
