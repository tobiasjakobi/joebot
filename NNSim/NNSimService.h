#ifndef __NNSIMSERVICE_H
#define __NNSIMSERVICE_H

#include "BaseNeuralNet.h"

CBaseNeuralNet *LoadNet(CBaseNeuralNet *&pO,const char *szName);
CBaseNeuralNet *LoadNet(const char *szName);

#endif  // __NNSIMSERVICE_H
