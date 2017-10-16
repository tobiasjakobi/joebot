// Neighborhood kernel functions for SOMs
// by Johannes Lampel / LampelSoft
// (c) Johannes Lampel / LampelSoft
// written Sept 13rd 2000

// z -> Distance of neurons
// d -> size of neighborhood kernel
#include <math.h>
#include "GlobalCqSOMDefs.h"

#define pi 3.14159265358979

double DllExport fNKcos(const double &z,const double &d){	
	if(z < d)
		return (cos( z/d * pi/2.0));
	else
		return 0;
}

double DllExport fNKgauss1(const double &z,const double &d){
	return exp(-((z*z)/(d*d)));									// don't use pow(?,2),'cause it's to time expensive
}

double DllExport fNKgauss2(const double &z,const double &d){
	return (1-((z*z)/(d*d)))*fNKgauss1(z,d);
}

double DllExport fNKcyl(const double &z,const double &d){
	if(z < d)
		return 1.0;
	else
		return 0.0;
}

double DllExport fNKcone(const double &z,const double &d){
	if(z < d)
		return 1.0-(z/d);
	else
		return 0.0;
}
