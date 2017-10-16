#ifndef __NEURALNETDEFS_H
#define __NEURALNETDEFS_H

//#define __NNSIM_TEXT

// which type of variable should be used for the data ?
typedef double netvar_t;

// define the minimum array sizes for weigths in a nn
#define _ARRAY_SEG_SIZE_NNWEIGHTS	1024

// define the minimum array sizes for links from one neuron to another
#define _ARRAY_SEG_SIZE_NLINK		1024

// define the minimum array sizes for pattern
#define _ARRAY_SEG_SIZE_PATTERN		64

#endif  // __NEURALNETDEFS_H
