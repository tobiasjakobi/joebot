#include "Activations.h"

#include <iostream.h>
#include <math.h>

#define _HASH_BORDER 3
#define _HASH_NUMBER 100

class CActivation{
public:
	CActivation();
	~CActivation();
	
	long max;
	
	netvar_t *ptanh;
	netvar_t *ptanhS;
	
	netvar_t *plog;
	netvar_t *plogS;
}Activation_lookup;

CActivation :: CActivation(){
	netvar_t f;
	long lschl;
	long lP = 0;
#ifdef __NNSIM_TEXT
	cout << "NNSIM : Intializing Activation lookup table ";cout.flush();
#endif
	
	max = _HASH_BORDER * _HASH_NUMBER * 2;
	
	ptanh = new netvar_t[max];
	ptanhS = new netvar_t[max];
	
	plog = new netvar_t[max];
	plogS = new netvar_t[max];
	
	for(lschl = 0;lschl < max;lschl ++){
#ifdef __NNSIM_TEXT
		while(lP<lschl){
			lP += max/10;
			cout <<".";cout.flush();
		}
#endif

		f = ((netvar_t)lschl) / _HASH_NUMBER - _HASH_BORDER;
		
		ptanh[lschl] = foNActtanh(f);
		ptanhS[lschl]= foNActtanhS(f);
		
		plog[lschl] = foNActlog(f);
		plogS[lschl]= foNActlogS(f);
	}
#ifdef __NNSIM_TEXT
	cout << " finished" << endl;
#endif
}

CActivation :: ~CActivation(){
	if(ptanh) delete [] ptanh;
	if(ptanhS)delete [] ptanhS;
	
	if(plog) delete [] plog;
	if(plogS)delete [] plogS;
}

bool SetAct(int iType, int iTypeoC, netvar_t (*&p)(const netvar_t &), netvar_t (*&pS)(const netvar_t &)){
	switch(iTypeoC){
	case TOAC_HASH:
		switch(iType){
		case NEURON_ACT_TANH:
			p	= fNActtanh;
			pS	= fNActtanhS;
			return true;
		case NEURON_ACT_LOG:
			p	= fNActlog;
			pS	= fNActlogS;
			return true;
		default:
			p	= 0;
			pS	= 0;
			return false;
		}
		
	case TOAC_DIRECT:
		switch(iType){
		case NEURON_ACT_TANH:
			p	= foNActtanh;
			pS	= foNActtanhS;
			return true;
		case NEURON_ACT_LOG:
			p	= foNActlog;
			pS	= foNActlogS;
			return true;
		default:
			p	= 0;
			pS	= 0;
			return false;
		}
	}
	return false;
}
long lI;
// Act Functions
netvar_t fNActtanh(const netvar_t &dParam)
{
	//return (tanh(dParam));
	
	lI = long((dParam + _HASH_BORDER) * _HASH_NUMBER);
	if(lI<0){
		lI = 0;
	}
	else if(lI >= Activation_lookup.max){
		lI = Activation_lookup.max-1;
	}
	
	return Activation_lookup.ptanh[lI];
}

netvar_t fNActtanhS(const netvar_t &dParam)
{
	//return (1.1-(tanh(dParam)*tanh(dParam)));
	
	lI = long((dParam + _HASH_BORDER) * _HASH_NUMBER);
	if(lI<0){
		lI = 0;
	}
	else if(lI >= Activation_lookup.max){
		lI = Activation_lookup.max-1;
	}
	
	return Activation_lookup.ptanhS[lI];
}

netvar_t fNActlog(const netvar_t &dParam)
{
	//return (1/(1+exp(dParam)));
	
	lI = long((dParam + _HASH_BORDER) * _HASH_NUMBER);
	if(lI<0){
		lI = 0;
	}
	else if(lI >= Activation_lookup.max){
		lI = Activation_lookup.max-1;
	}
	
	return Activation_lookup.plog[lI];
}

netvar_t fNActlogS(const netvar_t &dParam)
{
	//return (fNActlog(dParam)*(1-fNActlog(dParam)));
	lI = long((dParam + _HASH_BORDER) * _HASH_NUMBER);
	if(lI<0){
		lI = 0;
	}
	else if(lI >= Activation_lookup.max){
		lI = Activation_lookup.max-1;
	}
	
	return Activation_lookup.plogS[lI];
}


// original functions

netvar_t foNActtanh(const netvar_t &dParam)
{
	return (tanh(dParam));
}

netvar_t foNActtanhS(const netvar_t &dParam)
{
	return (netvar_t(1.0)-(tanh(dParam)*tanh(dParam)));
}

netvar_t foNActlog(const netvar_t &dParam)
{
	return (netvar_t(1.0)/(netvar_t(1.0)+exp(dParam)));
}

netvar_t foNActlogS(const netvar_t &dParam)
{
	return (fNActlog(dParam)*(netvar_t(1)-fNActlog(dParam)));
}
