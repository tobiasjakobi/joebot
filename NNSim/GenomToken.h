#ifndef __GENOMTOKEN_H
#define __GENOMTOKEN_H

enum e_genomtoken{
	GT_NetType = 1,
	/*
	GL_NetType : Setting the Type of Net.

	Data :	1 short ( type )
	*/
	GT_NumLayer,
	/*
	GT_NumLayer : Setting the number of layers

	Data :	1 short ( number of layers )
	*/
	GT_NumNeuronsOL,
	/*
	GT_NumNeuronsOL : Setting the number of Neurons on layer

	Data :	1 short ( layer )
			1 int ( number of neurons on this layer )
	*/
	GT_Alloc,
	/*
	GT_Alloc : Allocation memory for the neurons.

	No Data
	*/
	GT_ConnectNeurons,
	/*
	GT_ConnectNeurons : Connect two neurons with a defined weight

	Data :	1 short ( layer neuron1 )
			1 int	( number of neuron1 )
			1 short ( layer neuron2 )
			1 int	( number of neuron2 )
			1 int	( number of corresponding weight. -1 if a random weight should be used here )
	*/
	GT_SetNeuronBias,
	/*
	GT_SetNeuronBias : Setting the bias of a neuron

	Data :	1 short ( layer neuron )
			1 int	( number of neuron )
			1 int	( number of corresponding bias stored )
	*/
	GT_End,
	/*
	GT_END : Setting end of data

	Data : none
	*/
	GT_NUMBEROFTOKEN
};

extern short psTokenDataSize[GT_NUMBEROFTOKEN]; // a field containing the data sizes for each token

#endif
