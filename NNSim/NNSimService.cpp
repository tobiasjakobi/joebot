#include "NNSimService.h"

#include "NeuralNet.h"
#include "string.h"

CBaseNeuralNet *LoadNet(CBaseNeuralNet *&pO,const char *szName){
	if( pO ){
		delete pO;
	}
	pO = LoadNet(szName);
	return pO;
}

CBaseNeuralNet *LoadNet(const char *szName){
	CBaseNeuralNet *p = 0;
	FILE *fhd;
	char szPreI[50];

	if(fhd = fopen(szName,"r")){
		fread(szPreI,sizeof(char),40,fhd);

		if(!strcmp(szPreI,"CNeuralNetBPropM")){
			p = new CNeuralNetBPropM;
		}
		else if(!strcmp(szPreI,"CNeuralNetBProp")){
			p = new CNeuralNetBProp;
		}

		if(p)
			p->LoadFile(szName);
	}

	return p;
}
