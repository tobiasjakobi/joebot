#include "nVec.h"

nVec :: nVec(){
	lCDim = 0;
	iType = -1;
	ppdDim = 0;
}

nVec :: nVec(const dim_c dcDim){
	ppdDim = new double[dcDim];
	lCDim = dcDim;
	iType = nVT_copy;							// copy data by default
}

nVec :: nVec(const dim_c dcDim,const double * const dptr){
	ppdDim = new double[dcDim];
	lCDim = dcDim;
	memcpy(ppdDim,dptr,sizeof(double) * lCDim);
	iType = nVT_copy;							// copy data by default
}

nVec :: nVec(const nVec &npVec){
	ppdDim = 0;
	this->operator = (npVec);
	iType = nVT_copy;							// copy data by default
}

nVec :: ~nVec(){
	if(!iType && ppdDim) delete [] ppdDim;
	lCDim = 0;
	ppdDim = 0;
}

double nVec :: Length(void){
	//return CBLength();
	double dQSum = 0,dTemp;									// sum of quadr
	long lschl;
	for(lschl = 0;lschl < lCDim;lschl ++){
		dTemp = ppdDim[lschl];
		dQSum += dTemp * dTemp;
	}
	return sqrt(dQSum);
}

int nVec :: Normalize(void){
	double dLength = Length();
	if(dLength==0.0){
		dLength=1.0;
		return 1;
	}
	dLength = 1.0 / dLength;

	long lschl;
	for(lschl=0;lschl < lCDim;lschl++){
		ppdDim[lschl] *= dLength;
	}
	return 1;
}

const nVec & nVec :: operator = (const nVec & NParam){
	try{
		if(!iType){
			if(ppdDim) delete []ppdDim;
			ppdDim = new double[NParam.lCDim];
			lCDim = NParam.lCDim;
			memcpy(ppdDim,NParam.ppdDim,sizeof(double)*lCDim);
		}
		else{
			if(lCDim != NParam.lCDim){
				throw IncompatibleVectors();
			}
			memcpy(ppdDim,NParam.ppdDim,sizeof(double)*lCDim);
		}
		return ((const nVec &)(*this));
	}
	catch(IncompatibleVectors IV){
		cerr << " 'IncompatibleVectors' thrown by = " << lCDim << " + " << NParam.lCDim <<" : "<< &NParam <<endl;
		throw (IV);
	}
}

const nVec & nVec :: operator +=(const nVec & NParam){
	try{
		if(lCDim != NParam.lCDim){
			throw IncompatibleVectors();
		}
		long lschl;
		for(lschl=0;lschl<lCDim;lschl++){
			ppdDim[lschl] += NParam.ppdDim[lschl];
		}
	}
	catch(IncompatibleVectors IV){
		cerr << " 'IncompatibleVectors' thrown by += " << lCDim << " + " << NParam.lCDim <<" : "<< &NParam <<endl;
		throw(IV);
	}
	return ((const nVec &)(*this));
}

const nVec & nVec :: operator -=(const nVec & NParam){
	try{
		if(lCDim != NParam.lCDim){
			throw IncompatibleVectors();
		}
		long lschl;
		for(lschl=0;lschl < lCDim;lschl++){
			ppdDim[lschl] -= NParam.ppdDim[lschl];
		}
	}
	catch(IncompatibleVectors IV){
		cerr << " 'IncompatibleVectors' thrown by -= " << lCDim << " + " << NParam.lCDim <<" : "<< &NParam <<endl;
		throw(IV);
	}
	return ((const nVec &)(*this));
}

const nVec & nVec :: operator *=(const double dParam){
	long lschl;
	for(lschl=0;lschl<lCDim;lschl++){
		ppdDim[lschl] *= dParam;
	}
	return ((const nVec &)(*this));
}

const nVec & nVec :: operator /=(const double dParam){
	long lschl;
	for(lschl=0;lschl<lCDim;lschl++){
		ppdDim[lschl] /= dParam;
	}
	return ((const nVec &)(*this));
}

nVec nVec :: operator +(const nVec &NParam){
	try{
		if(lCDim != NParam.lCDim){
			throw IncompatibleVectors();
		}
			nVec TmpVec(lCDim);
			TmpVec = *this;
			TmpVec+=NParam;
			return TmpVec;
	}
	catch(IncompatibleVectors IV){
		cerr << " 'IncompatibleVectors' thrown by + " << lCDim << " + " << NParam.lCDim <<" : "<< &NParam <<endl;
		throw(IV);
	}
	return 0;			// error if this is executed
}

nVec nVec :: operator -(const nVec &NParam){
	try{
		if(lCDim != NParam.lCDim){
			throw IncompatibleVectors();
		}
		nVec TmpVec(lCDim);
		TmpVec = *this;
		TmpVec-=NParam;
		return TmpVec;
	}
	catch(IncompatibleVectors IV){
		cerr << " 'IncompatibleVectors' thrown by - " << lCDim << " + " << NParam.lCDim << " : "<< &NParam <<endl;
		throw(IV);
	}
	return 0;			// error if this is executed
}

nVec nVec :: operator *(const double dParam){
	nVec TmpVec(lCDim);
	TmpVec = *this;
	TmpVec*=dParam;
	return TmpVec;
}

nVec nVec :: operator /(const double dParam){
	nVec TmpVec(lCDim);
	TmpVec = *this;
	TmpVec/=dParam;
	return TmpVec;
}

double & nVec :: operator [] (long lIndex){
	return ((double &)(ppdDim[lIndex]));
}

double nVec :: SLength(void){
	double dtemp=0;
	double dx;
	long lschl;
	for(lschl=0;lschl < lCDim;lschl++){
		dx = ppdDim[lschl];
		dtemp += (dx * dx);
	}
	return dtemp;
}

double nVec :: CBLength(void){
	double dtemp=0;
	long lschl;
	for(lschl=0;lschl < lCDim;lschl++){
		dtemp += ppdDim[lschl];
	}
	return dtemp;
}

int nVec :: GetVec(double *dptr){
	/*long lschl;
	for(lschl=0;lschl < lCDim;lschl ++){
		ppdDim[lschl] = dptr[lschl];
	}*/
	memcpy(ppdDim,dptr,sizeof(double) * lCDim);
	return 1;
}

int nVec :: SetVec(double *dpDest){
	/*long lschl;
	for(lschl=0;lschl < lCDim;lschl++){
		dpDest[lschl] = ppdDim[lschl];
	}*/
	memcpy(dpDest,ppdDim,sizeof(double) * lCDim);
	return 1;
}

int nVec :: SetMemVec(double *dpParam){
	// setting ppdDim to mempos and iType to nVT_mem (case del of old alloc mem)
	if(!iType){
		if(ppdDim) delete [] ppdDim;
	}
	ppdDim = dpParam;
	iType = nVT_mem;
	return 1;
}

int nVec :: ConvMemtoCopy(void){
	double *dpTmp;
	dpTmp = new double[lCDim];
	memcpy(dpTmp,ppdDim,sizeof(double) * lCDim);
	ppdDim = dpTmp;
	iType = nVT_copy;
	return 1;
}

int nVec :: SetNumDim(const dim_c dcDim){
	if(!iType && !ppdDim) delete [] ppdDim;
	lCDim = dcDim;
	ppdDim = new double[dcDim];
	return 1;
}

int nVec :: Save(FILE *fhd){
	return fwrite(ppdDim,sizeof(double),lCDim,fhd);
}

int nVec :: Load(FILE *fhd){
	if(ppdDim) return fread(ppdDim,sizeof(double),lCDim,fhd);
	return false;
}

void nVec :: Zero(void){
	long lschl;
	for(lschl=0;lschl < lCDim;lschl++){
		ppdDim[lschl] = 0;
	}
}

// global functions

nVec operator *(double dParam,nVec &nParam){
	return (nParam.operator *(dParam));
}
