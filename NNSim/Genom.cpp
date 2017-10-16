// Genom.cpp: implementation of the CGenom class.
//
//////////////////////////////////////////////////////////////////////

#include "Genom.h"
#include "GenomToken.h"

#include <iostream.h>

//#define __GENOM_COUT_LOG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGenom::CGenom()
{
	psTopData = 0;
	pWeights = 0;
	pBiases = 0;
	Init();
}

CGenom::~CGenom()
{
	if(psTopData){
		delete [] psTopData;
		psTopData = 0;
	}
	if(pWeights){
		delete [] pWeights;
		pWeights = 0;
	}
	if(pBiases){
		delete [] pBiases;
		pBiases = 0;
	}
}

void CGenom :: Init(void){
	lNumTopData = 0;
	lNumTopDataAlloc = 1024;
	if(psTopData)
		delete [] psTopData;
	psTopData = new short[1024];
	lNumWeights = 0;
	lNumWeightsAlloc = 1024;
	if(pWeights)
		delete [] pWeights;
	pWeights = new netvar_t[1024];
	lNumBiases = 0;
	lNumBiasesAlloc = 1024;
	if(pBiases)
		delete [] pBiases;
	pBiases = new netvar_t[1024];
}

bool CGenom :: Save(const char *szFileNameP, bool bRemoveExt){
	char szFileName[250],
		szFileNameTopData[250],
		szFileNameBiases[250],
		szFileNameWeights[250],
		*szTemp;
	
	strcpy(szFileName,szFileNameP);
	
	if(bRemoveExt){
		szTemp = strchr(szFileName,'.');
		if(szTemp)
			*szTemp = 0;
	}
	
	strcpy(szFileNameTopData,szFileName);
	strcat(szFileNameTopData,".");
	strcat(szFileNameTopData,_FILEEXT_TOPDATA);
	strcpy(szFileNameBiases,szFileName);
	strcat(szFileNameBiases,".");
	strcat(szFileNameBiases,_FILEEXT_BIASES);
	strcpy(szFileNameWeights,szFileName);
	strcat(szFileNameWeights,".");
	strcat(szFileNameWeights,_FILEEXT_WEIGHTS);
	
	if(SaveTopData(szFileNameTopData)){
		if(SaveBiases(szFileNameBiases)){
			if(SaveWeights(szFileNameWeights)){
				return true;
	}}}
	
	return false;
}

bool CGenom :: SaveTopData(const char *szFileNameP){
	GenomFileHeader_s header;
	FILE *fhd;
	
	if(fhd = fopen(szFileNameP,"wb")){
		strcpy(header.szDescription,"LampelSoft GALib TData");
		header.iType = _TOP_DATA;
		header.iVersion = __CGENOMVERSION;

		fwrite(&header,1,sizeof(header),fhd);

		fwrite(&lNumTopData,1,sizeof(long),fhd);
		fwrite(psTopData,lNumTopData,sizeof(short),fhd);

		fclose(fhd);
		return true;
	}
#ifdef __GENOM_COUT_LOG
	cout << szFileNameP << " : File not found" << endl;
#endif
	return false;
}

bool CGenom :: SaveWeights(const char *szFileNameP){
	GenomFileHeader_s header;
	FILE *fhd;
	
	if(fhd = fopen(szFileNameP,"wb")){
		strcpy(header.szDescription,"LampelSoft GALib Weights");
		header.iType = _WEIGHT_DATA;
		header.iVersion = __CGENOMVERSION;

		fwrite(&header,1,sizeof(header),fhd);

		fwrite(&lNumWeights,1,sizeof(long),fhd);
		fwrite(pWeights,lNumWeights,sizeof(netvar_t),fhd);

		fclose(fhd);
		return true;
	}
#ifdef __GENOM_COUT_LOG
	cout << szFileNameP << " : File not found" << endl;
#endif
	return false;
}

bool CGenom :: SaveBiases(const char *szFileNameP){
	GenomFileHeader_s header;
	FILE *fhd;
	
	if(fhd = fopen(szFileNameP,"wb")){
		strcpy(header.szDescription,"LampelSoft GALib Biases");
		header.iType = _BIAS_DATA;
		header.iVersion = __CGENOMVERSION;

		fwrite(&header,1,sizeof(header),fhd);

		fwrite(&lNumBiases,1,sizeof(long),fhd);
		fwrite(pBiases,lNumBiases,sizeof(netvar_t),fhd);

		fclose(fhd);
		return true;
	}
#ifdef __GENOM_COUT_LOG
	cout << szFileNameP << " : File not found" << endl;
#endif
	return false;
}

bool CGenom :: Load(const char *szFileNameP, bool bRemoveExt){
	char szFileName[250],
		szFileNameTopData[250],
		szFileNameBiases[250],
		szFileNameWeights[250],
		*szTemp;
	
	strcpy(szFileName,szFileNameP);
	
	if(bRemoveExt){
		szTemp = strchr(szFileName,'.');
		if(szTemp)
			*szTemp = 0;
	}
	
	strcpy(szFileNameTopData,szFileName);
	strcat(szFileNameTopData,".");
	strcat(szFileNameTopData,_FILEEXT_TOPDATA);
	strcpy(szFileNameBiases,szFileName);
	strcat(szFileNameBiases,".");
	strcat(szFileNameBiases,_FILEEXT_BIASES);
	strcpy(szFileNameWeights,szFileName);
	strcat(szFileNameWeights,".");
	strcat(szFileNameWeights,_FILEEXT_WEIGHTS);

	if(LoadTopData(szFileNameTopData)){
		if(LoadBiases(szFileNameBiases)){
			if(LoadWeights(szFileNameWeights)){
				return true;
	}}}

	return false;
}

bool CGenom :: LoadTopData(const char *szFileNameP){
	GenomFileHeader_s header;
	FILE *fhd;
	
	if(fhd = fopen(szFileNameP,"rb")){
		fread(&header,1,sizeof(header),fhd);
		if(header.iType != _TOP_DATA){
#ifdef __GENOM_COUT_LOG
	cout << szFileNameP << " : wrong data in file" << endl;
#endif
			return false;
		}
		if(header.iVersion != __CGENOMVERSION){
#ifdef __GENOM_COUT_LOG
	cout << szFileNameP << " : wrong fileversion" << endl;
#endif
			return false;
		}

		fread(&lNumTopData,1,sizeof(long),fhd);
		lNumTopDataAlloc = lNumTopData;
		psTopData = new short[lNumTopDataAlloc];
		fread(psTopData,lNumTopData,sizeof(short),fhd);

		fclose(fhd);
		return true;
	}
#ifdef __GENOM_COUT_LOG
	cout << szFileNameP << " : File not found" << endl;
#endif
	return false;
}

bool CGenom :: LoadWeights(const char *szFileNameP){
	GenomFileHeader_s header;
	FILE *fhd;
	
	if(fhd = fopen(szFileNameP,"rb")){
		fread(&header,1,sizeof(header),fhd);

		if(header.iType != _WEIGHT_DATA){
#ifdef __GENOM_COUT_LOG
	cout << szFileNameP << " : wrong data in file" << endl;
#endif
			return false;
		}
		if(header.iVersion != __CGENOMVERSION){
#ifdef __GENOM_COUT_LOG
	cout << szFileNameP << " : wrong fileversion" << endl;
#endif
			return false;
		}

		fread(&lNumWeights,1,sizeof(long),fhd);
		lNumWeightsAlloc = lNumWeights;
		pWeights = new netvar_t[lNumWeightsAlloc];
		fread(pWeights,lNumWeights,sizeof(netvar_t),fhd);

		fclose(fhd);
		return true;
	}
#ifdef __GENOM_COUT_LOG
	cout << szFileNameP << " : File not found" << endl;
#endif
	return false;
}

bool CGenom :: LoadBiases(const char *szFileNameP){
	GenomFileHeader_s header;
	FILE *fhd;
	
	if(fhd = fopen(szFileNameP,"rb")){
		fread(&header,1,sizeof(header),fhd);

		if(header.iType != _BIAS_DATA){
#ifdef __GENOM_COUT_LOG
	cout << szFileNameP << " : wrong data in file" << endl;
#endif
			return false;
		}
		if(header.iVersion != __CGENOMVERSION){
#ifdef __GENOM_COUT_LOG
	cout << szFileNameP << " : wrong fileversion" << endl;
#endif
			return false;
		}

		fread(&lNumBiases,1,sizeof(long),fhd);
		lNumBiasesAlloc = lNumBiases;
		pBiases = new netvar_t[lNumBiasesAlloc];
		fread(pBiases,lNumBiases,sizeof(netvar_t),fhd);

		fclose(fhd);
		return true;
	}
#ifdef __GENOM_COUT_LOG
	cout << szFileNameP << " : File not found" << endl;
#endif
	return false;
}

bool CGenom :: CreateNet(CBaseNeuralNet **p){			// create a net based on the genetic information stored
	if(*p){
		delete *p;
		*p=0;
	}
	short sType=0;
	short sNumLayer;
	short sLayer;
	int iNumoN;
	short sL1,sL2;
	int iLN1,iLN2;
	int iWeight,*pI,iNetWeight;
	short *pToken,*pData,*pTempData;
	
	pToken = psTopData;
	while(pToken){
		if(pToken[0] < 0
			|| pToken[0] > GT_NUMBEROFTOKEN){
			pToken = 0;
			cerr << "cgenom :: createnet : unknown token" << endl;
			break;
		}
		
		if(psTokenDataSize[pToken[0]]/sizeof(short) + pToken - psTopData > lNumTopData){
			pToken = 0;
			cerr << "cgenom :: createnet : out of field size" << endl;
			break;
		}
		
		pTempData = pData = &(pToken[1]);
		switch(pToken[0]){
		case GT_NetType:
#ifdef __GENOM_COUT_LOG
			cout << "GT_NetType" << endl;
#endif
			sType = pData[0];
			/*switch(sType){
			case NT_BASENEURALNETBP
		}*/
			*p = new CNeuralNetBProp;
			break;
		case GT_NumLayer:
#ifdef __GENOM_COUT_LOG
			cout << "GT_NumLayer" << endl;
#endif
			sNumLayer = pData[0];
			((CBaseNeuralNetFF*)(*p))->SetLayerNum(sNumLayer);
			break;
		case GT_NumNeuronsOL:
#ifdef __GENOM_COUT_LOG
			cout << "GT_NumNeuronsOL" << endl;
#endif
			sLayer = pData[0];
			pTempData++;
			iNumoN = *((int*)(pTempData));
			((CBaseNeuralNetFF*)(*p))->SetNNeuronsOnLayer(sLayer,iNumoN);
			break;
		case GT_Alloc:
#ifdef __GENOM_COUT_LOG
			cout << "GT_Alloc" << endl;
#endif
			((CBaseNeuralNetFF*)(*p))->AllocNeurons();
			break;
		case GT_ConnectNeurons:
#ifdef __GENOM_COUT_LOG
			cout << "GT_ConnectNeurons" << endl;
#endif
			sL1 = pTempData[0];
			pTempData ++;
			
			iLN1 = *((int*)(pTempData));
			pTempData+=sizeof(int)/sizeof(short);
			
			sL2 = pTempData[0];
			pTempData ++;
			
			iLN2 = *((int*)(pTempData));
			pTempData+=sizeof(int)/sizeof(short);
			
			iWeight = *((int*)(pTempData));
			pTempData+=sizeof(int)/sizeof(short);
			
			iNetWeight = ((CBaseNeuralNetFF*)(*p))->AddWeight();
			((CBaseWeightFF*)(((CBaseNeuralNetFF*)(*p))->GetWeight(iNetWeight)))->SetWeight(pWeights[iWeight]);
			((CBaseNeuralNetFF*)(*p))->ConnectNeurons(sL1,iLN1,sL2,iLN2,iNetWeight);
			break;
		case GT_SetNeuronBias:
#ifdef __GENOM_COUT_LOG
			cout << "GT_SetNeuronBias" << endl;
#endif
			sL1 = pTempData[0];
			pTempData ++;
			iLN1 = *((int*)(pTempData));
			pI=(int*)pTempData;pI++;pTempData=(short*)pI;
			iWeight = *((int*)(pTempData));
			pI=(int*)pTempData;pI++;pTempData=(short*)pI;
			((CBaseNeuralNetFF*)(*p))->GetNeuron(sL1,iLN1)->SetBias(pBiases[iWeight]);
			break;
		case GT_End:
#ifdef __GENOM_COUT_LOG
			cout << "GT_End" << endl;
#endif
			pToken = 0;
			break;
		default:
			cerr << "cgenom :: createnet : unknown token, but in range" << endl;
			break;
		}
		if(pToken)
			pToken += psTokenDataSize[pToken[0]]/sizeof(short);
	}
	return false;
}

bool CGenom :: SaveDescription(const char *szName){
	FILE *fhd;
	short sType=0;
	short sNumLayer;
	short sLayer;
	int iNumoN;
	short sL1,sL2;
	int iLN1,iLN2;
	int iWeight,*pI;
	short *pToken,*pData,*pTempData;
	
	fhd = fopen(szName,"w");
	if(!fhd)
		return false;
	
	pToken = psTopData;
	while(pToken){
		if(pToken[0] < 0
			|| pToken[0] > GT_NUMBEROFTOKEN){
			pToken = 0;
			cerr << "cgenom :: createnet : unknown token" << endl;
			break;
		}
		
		if(psTokenDataSize[pToken[0]]/sizeof(short) + pToken - psTopData > lNumTopData){
			pToken = 0;
			cerr << "cgenom :: createnet : out of field size" << endl;
			break;
		}
		
		pTempData = pData = &(pToken[1]);
		switch(pToken[0]){
		case GT_NetType:
#ifdef __GENOM_COUT_LOG
			cout << "GT_NetType" << endl;
#endif
			sType = pData[0];
			fprintf(fhd,"Create a Feedforward NeuralNet\n");
			break;
		case GT_NumLayer:
#ifdef __GENOM_COUT_LOG
			cout << "GT_NumLayer" << endl;
#endif
			sNumLayer = pData[0];
			fprintf(fhd,"with %i layers\n",sNumLayer);
			break;
		case GT_NumNeuronsOL:
#ifdef __GENOM_COUT_LOG
			cout << "GT_NumNeuronsOL" << endl;
#endif
			sLayer = pData[0];
			pTempData++;
			iNumoN = *((int*)(pTempData));
			fprintf(fhd,"%i neurons on layer #%i\n",iNumoN,sLayer);
			break;
		case GT_Alloc:
#ifdef __GENOM_COUT_LOG
			cout << "GT_Alloc" << endl;
#endif
			fprintf(fhd,"and that's all about the basic topology\n");
			break;
		case GT_ConnectNeurons:
#ifdef __GENOM_COUT_LOG
			cout << "GT_ConnectNeurons" << endl;
#endif
			sL1 = pTempData[0];
			pTempData ++;
			
			iLN1 = *((int*)(pTempData));
			pTempData+=sizeof(int)/sizeof(short);
			
			sL2 = pTempData[0];
			pTempData ++;
			
			iLN2 = *((int*)(pTempData));
			pTempData+=sizeof(int)/sizeof(short);
			
			iWeight = *((int*)(pTempData));
			pTempData+=sizeof(int)/sizeof(short);
			
			fprintf(fhd,"Connect neuron %i on layer %i with neuron %i on layer %i with %f\n",iLN1,sL1,iLN2,sL2,pWeights[iWeight]);
			break;
		case GT_SetNeuronBias:
#ifdef __GENOM_COUT_LOG
			cout << "GT_SetNeuronBias" << endl;
#endif
			sL1 = pTempData[0];
			pTempData ++;
			iLN1 = *((int*)(pTempData));
			pI=(int*)pTempData;pI++;pTempData=(short*)pI;
			iWeight = *((int*)(pTempData));
			pI=(int*)pTempData;pI++;pTempData=(short*)pI;
			fprintf(fhd,"Set the bias of neuron %i on layer %i to %f\n",iLN1,sL1,pBiases[iWeight]);
			break;
		case GT_End:
#ifdef __GENOM_COUT_LOG
			cout << "GT_End" << endl;
#endif
			fprintf(fhd,"and that was the whole net :D\n");
			pToken = 0;
			break;
		default:
			cerr << "cgenom :: createnet : unknown token, but in range" << endl;
			break;
		}
		if(pToken)
			pToken += psTokenDataSize[pToken[0]]/sizeof(short);
	}
	fclose(fhd);
	return false;
}

bool CGenom :: GetFFNet(CBaseNeuralNetFF *p){		// create genetic information from the net
	Init();
	short *pToken;
	short *pData;
	long lschl,lL,lN,lT,lL2,lN2,lW;
	
	pToken = psTopData;
	pData = &(pToken[1]);
	
	WannaAddToken(psTokenDataSize[GT_NetType]);
	pToken[0] = GT_NetType;
	pData[0] = short(p->GetITONet());
	lNumTopData += psTokenDataSize[GT_NetType]/sizeof(short);
	//cout << psTokenDataSize[pToken[0]] << endl;
	
	WannaAddToken(psTokenDataSize[GT_NumLayer]);
	pToken =  &(psTopData[lNumTopData]);
	pData = &(pToken[1]);
	pToken[0] = GT_NumLayer;
	pData[0] = short(p->lLayerNum);
	lNumTopData += psTokenDataSize[GT_NumLayer]/sizeof(short);
	//cout << psTokenDataSize[pToken[0]] << endl;
	
	for(lschl = 0;lschl < p->lLayerNum;lschl ++){		
		WannaAddToken(psTokenDataSize[GT_NumNeuronsOL]);
		pToken =  &(psTopData[lNumTopData]);
		pData = &(pToken[1]);
		pToken[0] = GT_NumNeuronsOL;
		pData[0] = short(lschl);
		pData ++;
		*((int *)(pData)) = p->plLNNum[lschl];
		lNumTopData += psTokenDataSize[GT_NumNeuronsOL]/sizeof(short);
		//cout << psTokenDataSize[pToken[0]] << endl;
	}
	
	WannaAddToken(psTokenDataSize[GT_Alloc]);
	pToken =  &(psTopData[lNumTopData]);
	pData = &(pToken[1]);
	pToken[0] = GT_Alloc;
	lNumTopData += psTokenDataSize[GT_Alloc]/sizeof(short);
	//cout << psTokenDataSize[pToken[0]] << endl;
	
	CBaseNeuronFF *pN,*pNT;
	for(lL = 0;lL < p->lLayerNum;lL++){
		for(lN = 0;lN < p->plLNNum[lL];lN++){
			pN = (CBaseNeuronFF*)(p->pppNeurons[lL][lN]);
			
			WannaAddToken(psTokenDataSize[GT_SetNeuronBias]);
			pToken =  &(psTopData[lNumTopData]);
			pData = &(pToken[1]);
			pToken[0] = GT_SetNeuronBias;
			pData[0] = short(lL);
			pData ++;
			*((int *)(pData)) = int(lN);
			pData += sizeof(int)/sizeof(short);
			
			WannaAddBias();
			lW = lNumBiases;
			pBiases[lW] = pN->m_dBias;
			lNumBiases ++;
			*((int *)(pData)) = lW;
			
			lNumTopData += psTokenDataSize[GT_SetNeuronBias]/sizeof(short);
			
			for(lT = pN->lLToNum-1;lT >= 0;lT--){
				pNT = ((CBaseNeuronFF *)(((CBaseLinkFF*)(pN->ppLinksTo[lT]))->p));
				
				WannaAddToken(psTokenDataSize[GT_ConnectNeurons]);
				pToken =  &(psTopData[lNumTopData]);
				pData = &(pToken[1]);
				pToken[0] = GT_ConnectNeurons;
				pData[0] = short(lL);
				pData ++;
				*((int *)(pData)) = int(lN);
				pData += sizeof(int)/sizeof(short);
				
				p->GetNPosition(pNT,lL2,lN2);
				
				pData[0] = short(lL2);
				pData ++;
				*((int *)(pData)) = int(lN2);
				pData += sizeof(int)/sizeof(short);
				
				WannaAddWeight();
				lW = lNumWeights;
				pWeights[lW] = ((CBaseWeightFF*)( ((CBaseLinkFF*)(pN->ppLinksTo[lT]))->pW) )->GetWeight();
				//pWeights[lW] = ((CBaseWeightFF *)(p->GetWeight( ((CBaseLinkFF*)(pN->ppLinksTo[lT]))->lWIndex)))->GetWeight();
				//cout << ((CBaseLinkFF*)(pN->ppLinksTo))->pW << endl;
				//cout << pWeights[lW] << endl;
				lNumWeights ++;
				*((int *)(pData)) = lW;
				//cout << lW << endl;
				//pData += sizeof(int)/sizeof(short);
				
				lNumTopData += psTokenDataSize[GT_ConnectNeurons]/sizeof(short);
			}
		}
	}
	
	/*for(lschl=0;lschl < lNumTopData;lschl++){
	cout << psTopData[lschl] << endl;
	}*/
	
	WannaAddToken(psTokenDataSize[GT_End]);
	pToken =  &(psTopData[lNumTopData]);
	pData = &(pToken[1]);
	pToken[0] = GT_End;
	lNumTopData += psTokenDataSize[GT_End]/sizeof(short);
	
	
	return false;
}

void CGenom :: Mutation(float fPropability, float fRange){
	long lschl;
	
	for(lschl = 0;lschl < lNumTopData;lschl ++){
	}
	for(lschl = 0;lschl < lNumBiases;lschl ++){
	}
}

bool CGenom :: Crossing(float fCut, CGenom &GOtherGenom){
	// fcut is [0;1]
	
	if(lNumWeights != GOtherGenom.lNumWeights
		|| lNumBiases != GOtherGenom.lNumBiases)
		return false;
	
	long lWBorder,lBBorder,lschl;
	netvar_t dTemp;
	
	lWBorder = long(float(lNumWeights) * fCut);
	lBBorder = long(float(lNumBiases) * fCut);
	
	// exchange weights
	for(lschl = 0;lschl < lWBorder;lschl ++){
		dTemp = pWeights[lschl];
		pWeights[lschl] = GOtherGenom.pWeights[lschl];
		GOtherGenom.pWeights[lschl] = dTemp;
	}
	for(lschl = 0;lschl < lBBorder;lschl ++){
		dTemp = pBiases[lschl];
		pBiases[lschl] = GOtherGenom.pBiases[lschl];
		GOtherGenom.pBiases[lschl] = dTemp;
	}
	
	return true;
}

void CGenom::WannaAddToken(int iSize){ // isize is number of wanted short shit
	if(lNumTopData + iSize > lNumTopDataAlloc){
		short *psTopDataNew = new short [ lNumTopDataAlloc + 1024 ];
		memcpy(psTopDataNew,psTopData,sizeof(short) * lNumTopData);
		if(psTopData)
			delete [] psTopData;
		psTopData = psTopDataNew;
		lNumTopDataAlloc += 1024;
	}
	else
		return;
}

void CGenom::WannaAddWeight(int iSize){ // isize is number of wanted short shit
	if(lNumWeights + iSize > lNumWeightsAlloc){
		netvar_t *pWeightsNew = new netvar_t [ lNumWeightsAlloc + 1024 ];
		memcpy(pWeightsNew,pWeights,sizeof(netvar_t) * lNumWeights);
		if(pWeights)
			delete [] pWeights;
		pWeights = pWeightsNew;
		lNumWeightsAlloc += 1024;
	}
	else
		return;
}

void CGenom::WannaAddBias(int iSize){ // isize is number of wanted short shit
	if(lNumBiases + iSize > lNumBiasesAlloc){
		netvar_t *pBiasesNew = new netvar_t [ lNumBiasesAlloc + 1024 ];
		memcpy(pBiasesNew,pBiases,sizeof(netvar_t) * lNumBiases);
		if(pBiases)
			delete [] pBiases;
		pBiases = pBiasesNew;
		lNumBiasesAlloc += 1024;
	}
	else
		return;
}
