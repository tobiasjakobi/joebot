#include "GenomToken.h"

short psTokenDataSize[GT_NUMBEROFTOKEN]={
	0,					// because the flags begin with 1
	sizeof(short)
	+ sizeof(short),	//GT_NetType
	sizeof(short)
	+ sizeof(short),	//GT_NumLayer,
	sizeof(short)
	+ sizeof(short)
	+ sizeof(int),		//GT_NumNeuronsOL,
	sizeof(short),		//GT_Alloc,
	sizeof(short)
	+sizeof(short)
	+sizeof(int)
	+sizeof(short)
	+sizeof(int)
	+sizeof(int),		//GT_ConnectNeurons,
	sizeof(short)
	+sizeof(short)
	+sizeof(int)
	+sizeof(int),		//GT_SetNeuronBias,
	sizeof(short)		//GT_End
};
