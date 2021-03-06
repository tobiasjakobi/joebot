# Microsoft Developer Studio Project File - Name="SOM_Analysis_LIN" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=SOM_Analysis_LIN - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SOM_Analysis_LIN.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SOM_Analysis_LIN.mak" CFG="SOM_Analysis_LIN - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SOM_Analysis_LIN - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "SOM_Analysis_LIN - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SOM_Analysis_LIN - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /GX /O2 /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /I "../NNSim/" /I "../NNSim/SOM/" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FAs /FR /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "SOM_Analysis_LIN - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\engine" /I "..\common" /I "..\pm_shared" /I "..\\" /I "../NNSim/" /I "../NNSim/SOM/" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FAs /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SOM_Analysis_LIN - Win32 Release"
# Name "SOM_Analysis_LIN - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\NNSim\SOM\CqSOM2d.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\SOM\fNBorKernel.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=..\NN_Train\NN.CPP
# End Source File
# Begin Source File

SOURCE=..\NNSim\SOM\nVec.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\SOM\SOMPattern.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\SOM\SOMPatternElem.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\SOM_Analysis_Win\Size.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "nnsim"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\NNSim\Activations.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseLink.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseLinkFF.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseNeuralNet.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseNeuralNetFF.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseNeuron.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseNeuronFF.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\BasePattern.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\BasePatternElem.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\BasePatternElemFF.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\BasePatternFF.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseWeight.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseWeightFF.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\LinkBProp.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\LinkBPropM.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\NeuralNetBProp.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\NeuralNetBPropM.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\NeuronBProp.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\NeuronBPropM.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\NNSimService.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\PatternBProp.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\PatternElemBProp.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\PatternElemBProp.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\WeightBProp.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\WeightBPropM.cpp
# End Source File
# End Group
# End Target
# End Project
