#ifndef __ACTIVATIONS_H
#define __ACTIVATIONS_H

#include "NeuralNetDefs.h"

bool SetAct(int iType, int iTypeoC,netvar_t (*&p)(const netvar_t &), netvar_t (*&pS)(const netvar_t &));

#define TOAC_DEFAULT	0

#define TOAC_HASH		0
#define TOAC_DIRECT		1

#define NEURON_ACT_TANH 1
netvar_t fNActtanh(const netvar_t &dParam);
netvar_t fNActtanhS(const netvar_t &dParam);

netvar_t foNActtanh(const netvar_t &dParam);
netvar_t foNActtanhS(const netvar_t &dParam);

#define NEURON_ACT_LOG 2
netvar_t fNActlog(const netvar_t &dParam);
netvar_t fNActlogS(const netvar_t &dParam);

netvar_t foNActlog(const netvar_t &dParam);
netvar_t foNActlogS(const netvar_t &dParam);

#endif  // __ACTIVATIONS_H
