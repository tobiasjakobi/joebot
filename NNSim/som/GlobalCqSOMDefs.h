#ifndef __GlobalCqSOMDefs_H
#define __GlobalCqSOMDefs_H

// written Sept 13rd 2000

typedef long InitType;
typedef long dim_c;

enum eInitTypes{
	Init_Random,
	Init_Regular
};

// def for easy use as dll

#ifdef __DLL
#define DllExport __declspec( dllexport )
#else
#define DllExport
#endif

#endif
