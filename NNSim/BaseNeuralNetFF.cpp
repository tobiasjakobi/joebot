// BaseNeuralNetFF.cpp: implementation of the CBaseNeuralNetFF class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseNeuralNetFF.h"
#include <memory.h>
#include <iostream.h>
#include <string.h>
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseNeuralNetFF::CBaseNeuralNetFF()
{
	plLNNum = 0;
	pppNeurons = 0;
	lLayerNum = 0;
	m_pTI = 0;
	m_dLRate = .1;
	m_lNumProp = 0;
	
	memset(m_szType,0,sizeof(char)*_LTYPEI);
	strcpy(m_szType,"CBaseNeuralNetFF");
	m_iType = NT_BASENEURALNETFF;

	m_pfNDist = 0;
	m_plNPR = 0;
}

CBaseNeuralNetFF::~CBaseNeuralNetFF()
{
	long lschl;
	
	if(plLNNum){
		for(lschl = 0;lschl < lLayerNum;lschl++){		// search for maximum
			if(plLNNum[lschl]){
				for(long lschl1 = plLNNum[lschl]-1;lschl1>-1;lschl1--){
					delete pppNeurons[lschl][lschl1];
				}
			}
			if(pppNeurons[lschl])
				delete [] pppNeurons[lschl];
		}
	}
	
	if(pppNeurons)
		delete [] pppNeurons;

	if(m_pfNDist)
		delete [] m_pfNDist;
	if(m_plNPR)
		delete [] m_plNPR;
	
	if(m_pTI)
		delete [] m_pTI;
	if(plLNNum)
		delete [] plLNNum;
}

bool CBaseNeuralNetFF::SetLayerNum(long lNum)
{
	if(m_pfNDist){
		delete [] m_pfNDist;
		m_pfNDist = 0;
	}
	if(m_plNPR){
		delete [] m_plNPR;
		m_plNPR=0;
	}
	lLayerNum = lNum;
	plLNNum = new long[lNum];				// alloc mem fr storing the number of neurons on each layer
	memset(plLNNum,0,sizeof(long)*lNum);	// set all to zero
	m_pfNDist = new float[lLayerNum];
	m_plNPR = new long[lLayerNum];
	for(long lschl=0;lschl < lNum;lschl ++){
		m_pfNDist[lschl] = 1;
		m_plNPR[lschl] = 100000;
	}
	return true;
}

bool CBaseNeuralNetFF::SetNNeuronsOnLayer(long lLayer, long lNNum)
{
	if(lLayer < lLayerNum){
		plLNNum[lLayer] = lNNum;
		return true;
	}
	else
		return false;
}

bool CBaseNeuralNetFF :: ConnectNeurons(long lFNL, long lFNN, long lSNL, long lSNN, long lIW)
{
	if(lFNL > lLayerNum || lSNL > lLayerNum)			// check ranges
		return false;
	if(lFNL < 0 || lSNL < 0)							// check ranges
		return false;
	if(plLNNum[lFNL] < lFNN || plLNNum[lSNL] < lSNN)
		return false;
	if(0> lFNN || 0> lSNN)
		return false;
	
	//cout << lFNL << " : "<<lFNN << " : "<<lSNL << " : "<<lSNN << " : "<<lIW<<endl;
	
	if(lIW == -1)										// if there isn't already a weight defined for this connection, get one
		lIW = AddWeight();
	
	CBaseNeuron *pF,*pS;
	pF = pppNeurons[lFNL][lFNN];
	pS = pppNeurons[lSNL][lSNN];
	
	((CBaseNeuronFF*)(pS))->AddLinkFrom(lIW,ppWeights[lIW],pF);
	((CBaseNeuronFF*)(pF))->AddLinkTo(lIW,ppWeights[lIW],pS);
	
	return true;
}

bool CBaseNeuralNetFF :: ConnectLayer(long lFL,long lSL){
	long lF,lS;
	
	if(lFL >= lLayerNum || lSL >= lLayerNum)			// check ranges
		return false;
	
	for(lF = plLNNum[lFL]-1;lF>-1;lF--){
		for(lS = plLNNum[lSL]-1;lS>-1;lS--){
			ConnectNeurons(lFL,lF,lSL,lS,-1);
		}
	}
	return true;
}

void CBaseNeuralNetFF::SetInput(netvar_t *p){
	long lschl;
	CBaseNeuron **ppNL = pppNeurons[0];
	
	for(lschl = plLNNum[0]-1;lschl>-1;lschl --){
		((CBaseNeuronFF*)(ppNL[lschl]))->m_dOut = p[lschl];
	}
}

void CBaseNeuralNetFF::GetOutput(netvar_t *p){
	long lschl;
	CBaseNeuron **ppNL = pppNeurons[lLayerNum-1];
	
	for(lschl = plLNNum[lLayerNum-1]-1;lschl>-1;lschl --){
		p[lschl] = ((CBaseNeuronFF*)(ppNL[lschl]))->m_dOut;
	}
}

bool CBaseNeuralNetFF::SetTInput(netvar_t *p)
{
	if(!m_pTI)
		return false;
	
	memcpy(m_pTI,p,sizeof(netvar_t)*GetOutputNum());
	return true;
}

void CBaseNeuralNetFF::CopyTI2N(){
	long lschl;
	CBaseNeuron **ppNL = pppNeurons[lLayerNum-1];

	for(lschl = GetOutputNum()-1;lschl>-1;lschl --){
		((CBaseNeuronFF*)(ppNL[lschl]))->m_dTI = m_pTI[lschl];
	}
}

void CBaseNeuralNetFF::InitWeights(netvar_t l,netvar_t h)
{
	CBaseNeuralNet::InitWeights(l,h);
	
	if(pppNeurons){
		long lN,lL;
		CBaseNeuron **ppNL;
		for(lL=lLayerNum-1;lL>-1;lL--){
			ppNL = pppNeurons[lL];
			for(lN = plLNNum[lL]-1;lN>-1;lN--){
				ppNL[lN]->InitWeights(l,h);
			}
		}
		ppNL = pppNeurons[0];
		for(lN = plLNNum[0]-1;lN>-1;lN--){
			((CBaseNeuronFF*)ppNL[lN])->m_dBias = 0;
		}
	}
}

void CBaseNeuralNetFF::SetLRate(const netvar_t &d){
	m_dLRate = d;
}

netvar_t CBaseNeuralNetFF :: GetLRate(void){
	return m_dLRate;
}

void CBaseNeuralNetFF::GetNPosition(CBaseNeuron *p, long &lL,long &lN){
	if(pppNeurons){
		CBaseNeuron **ppNL;
		for(lL=lLayerNum-1;lL>-1;lL--){
			ppNL = pppNeurons[lL];
			for(lN = plLNNum[lL]-1;lN>-1;lN--){
				if(p==ppNL[lN])
					return;
			}
		}
	}
	lL = lN = -1;
}

long CBaseNeuralNetFF :: GetConnectionNum(void){
	long lN,lL,lNumoCon=0;
	CBaseNeuron **ppNL;

	if(pppNeurons)
	for(lL=0;lL<lLayerNum;lL++){
		ppNL = pppNeurons[lL];
		for(lN = 0;lN<plLNNum[lL];lN++){
			lNumoCon += ((CBaseNeuronFF*)ppNL[lN])->lLToNum;
		}
	}
	return lNumoCon;
}

long CBaseNeuralNetFF :: GetWeightNum(void){
	long lN,lL,lNumoW=0;
	CBaseNeuron **ppNL;
	
	if(pppNeurons){
		for(lL=0;lL<lLayerNum;lL++){
			ppNL = pppNeurons[lL];
			for(lN = 0;lN<plLNNum[lL];lN++){
				lNumoW += ((CBaseNeuronFF*)ppNL[lN])->lLToNum;
				lNumoW ++;	// add bias weight
			}
		}
	}
	return lNumoW;
}

void CBaseNeuralNetFF::Save(FILE *fhd){
	CBaseNeuralNet::Save(fhd);
	
	if(!pppNeurons)
		return;
	
	long lNumoCon = 0;
	long lLayerThis,lNeuronThis;
	long lLayerOther,lNeuronOther;
	CBaseNeuron *pA;
	long lN,lL;
	CBaseNeuron **ppNL;
	
	fwrite(&m_dLRate,sizeof(netvar_t),1,fhd);
	fwrite(&lLayerNum,sizeof(long),1,fhd);
	fwrite(plLNNum,sizeof(long),lLayerNum,fhd);
	fwrite(m_pfNDist,sizeof(float),lLayerNum,fhd);
	fwrite(m_plNPR,sizeof(long),lLayerNum,fhd);

	for(lL=0;lL<lLayerNum;lL++){
		ppNL = pppNeurons[lL];
		for(lN = 0;lN<plLNNum[lL];lN++){
			ppNL[lN]->Save(fhd);
		}
	}

	lNumoCon = GetConnectionNum();
	
	fwrite (&lNumoCon,sizeof(long),1,fhd);
	//cout <<lNumoCon << endl;
	
	if(lNumoCon>0){
		long lN,lL,lLTo;
		CBaseNeuron **ppNL;
		for(lL=lLayerNum-1;lL>-1;lL--){
			ppNL = pppNeurons[lL];
			for(lN = plLNNum[lL]-1;lN>-1;lN--){
				GetNPosition(ppNL[lN],lLayerThis,lNeuronThis);
				//cout << lLayerThis << " : " << lNeuronThis << endl;
				for(lLTo=0;lLTo<((CBaseNeuronFF*)ppNL[lN])->lLToNum;lLTo++){
					pA = ((CBaseLinkFF*)((CBaseNeuronFF*)ppNL[lN])->ppLinksTo[lLTo])->p;
					GetNPosition(pA,lLayerOther,lNeuronOther);
					
					fwrite(&lLayerThis,sizeof(long),1,fhd);
					fwrite(&lNeuronThis,sizeof(long),1,fhd);
					fwrite(&lLayerOther,sizeof(long),1,fhd);
					fwrite(&lNeuronOther,sizeof(long),1,fhd);
					fwrite(&((CBaseNeuronFF*)ppNL[lN])->ppLinksTo[lLTo]->lWIndex,sizeof(long),1,fhd);
					//((CBaseNeuronFF*)ppNL[lN])->ppLinksTo[lLTo]->Save(fhd);
				}
			}
		}
	}
	fwrite(&m_lNumProp,sizeof(long),1,fhd);
}

void CBaseNeuralNetFF::Load(FILE *fhd){
	long lschl,
		lL,
		lN,
		lNumoCon;
	long lLayerThis,lNeuronThis;
	long lLayerOther,lNeuronOther;
	long lWIndex;
	CBaseNeuron **ppNL;
	
	if(pppNeurons){
		// delete evtl. allocated data before loading
		for(lschl = 0;lschl < lLayerNum;lschl++){		// search for maximum
			if(plLNNum[lschl]){
				for(long lschl1 = plLNNum[lschl]-1;lschl1>-1;lschl1--){
					delete pppNeurons[lschl][lschl1];
				}
			}
			if(pppNeurons[lschl])
				delete [] pppNeurons[lschl];
		}
		
		delete [] pppNeurons;
		pppNeurons = 0;
	}
	
	if(m_pTI)
		delete [] m_pTI;
	m_pTI = 0;
	
	if(plLNNum)
		delete [] plLNNum;
	plLNNum = 0;

	CBaseNeuralNet::Load(fhd);

	// load data from file

	fread(&m_dLRate,sizeof(netvar_t),1,fhd);
	fread(&lLayerNum,sizeof(long),1,fhd);
	plLNNum = new long[lLayerNum];
	m_plNPR = new long[lLayerNum];
	m_pfNDist = new float[lLayerNum];
	fread(plLNNum,sizeof(long),lLayerNum,fhd);
	fread(m_pfNDist,sizeof(float),lLayerNum,fhd);
	fread(m_plNPR,sizeof(long),lLayerNum,fhd);
	AllocNeurons();
	for(lL=0;lL<lLayerNum;lL++){
		ppNL = pppNeurons[lL];
		for(lN = 0;lN<plLNNum[lL];lN++){
			ppNL[lN]->Load(fhd);
		}
	}
	fread (&lNumoCon,sizeof(long),1,fhd);			// number of connections 
	//cout << lNumoCon << endl;
	for(lschl=0;lschl < lNumoCon;lschl++){
		fread(&lLayerThis,sizeof(long),1,fhd);
		fread(&lNeuronThis,sizeof(long),1,fhd);
		fread(&lLayerOther,sizeof(long),1,fhd);
		fread(&lNeuronOther,sizeof(long),1,fhd);
		fread(&lWIndex,sizeof(long),1,fhd);

		if(!ConnectNeurons(lLayerThis,lNeuronThis,lLayerOther,lNeuronOther,lWIndex)){
			cout << "got something strange, canceling operation"<<endl;
			return;
		}
	}
	fread(&m_lNumProp,sizeof(long),1,fhd);
}

long CBaseNeuralNetFF :: GetInputNum(void){
	if(plLNNum){
		return plLNNum[0];
	}
	else
		return 0;
}

long CBaseNeuralNetFF :: GetOutputNum(void){
	if(plLNNum){
		return plLNNum[lLayerNum-1];
	}
	else
		return 0;
}

long CBaseNeuralNetFF :: GetNeuronNum(void){
	long lNumoN=0;
	CBaseNeuron **ppNL;
	if(pppNeurons){
		for(long lL=0;lL<lLayerNum;lL++){
			ppNL = pppNeurons[lL];
			for(long lN = 0;lN<plLNNum[lL];lN++){
				lNumoN ++;
			}
		}
	}
	return lNumoN;
}

void CBaseNeuralNetFF::GetOutputErrors(netvar_t &dMin,netvar_t &dMax,netvar_t &dAv){
	if( pppNeurons ){
		CBaseNeuron **ppNL = pppNeurons[lLayerNum-1];
		netvar_t dDiff;
		long lschl;

		dMin = 10000;
		dMax = -10000;
		dAv = 0;

		for(lschl = GetOutputNum()-1;lschl>-1;lschl --){
			//dDiff = fabs(((CBaseNeuronFF*)(ppNL[lschl]))->m_dTI-((CBaseNeuronFF*)(ppNL[lschl]))->m_dOut);
			dDiff = fabs(m_pTI[lschl]-((CBaseNeuronFF*)(ppNL[lschl]))->m_dOut);
			
			if(dDiff < dMin)
				dMin = dDiff;

			if(dDiff > dMax)
				dMax = dDiff;

			dAv += dDiff;
		}
		dAv /= netvar_t(GetOutputNum());
	}
}

void CBaseNeuralNetFF :: RecalcPositions(void){
	long lschl,lschl1;

	float fX,fY;

	for(lschl = 0;lschl < lLayerNum;lschl++){
		if(plLNNum[lschl]){
			fX = fY = 0;
			for(lschl1 = plLNNum[lschl]-1;lschl1>-1;lschl1--){
				pppNeurons[lschl][lschl1]->m_fX = (lschl1 % m_plNPR[lschl])*m_pfNDist[lschl];
				pppNeurons[lschl][lschl1]->m_fY = (lschl1 / m_plNPR[lschl])*m_pfNDist[lschl];

				fX += pppNeurons[lschl][lschl1]->m_fX;
				fY += pppNeurons[lschl][lschl1]->m_fY;
			}
			fX /= float(plLNNum[lschl]);
			fY /= float(plLNNum[lschl]);
			for(lschl1 = plLNNum[lschl]-1;lschl1>-1;lschl1--){			// set the middle of each layer to one line
				pppNeurons[lschl][lschl1]->m_fX -= fX;
				pppNeurons[lschl][lschl1]->m_fY -= fY;
			}
		}
	}
}

bool CBaseNeuralNetFF :: AllocNeurons(void){
	long lschl,lLLayer = lLayerNum-1,lschl1;

	pppNeurons = new CBaseNeuron**[lLayerNum];		// allocate pointers for pointing to the neurons on the layers

	for(lschl = 0;lschl < lLayerNum;lschl++){
		if(plLNNum[lschl]){
			pppNeurons[lschl] = new CBaseNeuron*[plLNNum[lschl]];	// allocating pointer to neurons
			for(lschl1 = plLNNum[lschl]-1;lschl1>-1;lschl1--){
				pppNeurons[lschl][lschl1] = NewNeuron();		// allocating neurons on each layer
				if(lschl == lLLayer)
					((CBaseNeuronFF *)pppNeurons[lschl][lschl1])->m_bHidden = false;
				else
					((CBaseNeuronFF *)pppNeurons[lschl][lschl1])->m_bHidden = true;
			}
		}
		else
		{
			pppNeurons[lschl] = 0;
			cout << "what the fuck is this ?"<<endl;
		}
	}
	// allocate memory for storing teaching input
	if(m_pTI)
		delete [] m_pTI;
	m_pTI = new netvar_t [plLNNum[lLLayer]];

	RecalcPositions();
	return true;
}

CBaseNeuronFF *CBaseNeuralNetFF :: GetNeuron(long lL,long lN){
	return ((CBaseNeuronFF*)(pppNeurons[lL][lN]));
}

CBaseNeuronFF *CBaseNeuralNetFF :: GetNeuron(long lONum){
	if(pppNeurons){
		for(long lL=0;lL<lLayerNum;lL++){
			if(lONum>=plLNNum[lL]){
				lONum -= plLNNum[lL];
			}
			else{
				return ((CBaseNeuronFF*)pppNeurons[lL][lONum]);
			}
		}
	}
	return 0;
}

long CBaseNeuralNetFF :: AddWeight(void){
	if(lNumWeights >= lMaxWeights){
		// at end of field -> alloc new 
		lMaxWeights += _ARRAY_SEG_SIZE_NNWEIGHTS;
		CBaseWeight **pNew = new CBaseWeight*[lMaxWeights];
		if(ppWeights){
			//memset(pNew,0,lMaxWeights*sizeof(CBaseWeight*));
			memcpy(pNew,ppWeights,sizeof(CBaseWeight*)*lNumWeights);// copy from old to new
			delete [] ppWeights;									// and delete it
		}

		for(long lschl = lNumWeights;lschl < lMaxWeights;lschl++){		// alloc new weights
			pNew[lschl] = NewWeight();
		}
		ppWeights = pNew;											// copy pointer to new data to var
	}
	lNumWeights ++;
	return lNumWeights - 1;
}
