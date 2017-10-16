# Microsoft Developer Studio Project File - Name="joebot" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=joebot - Win32 Debug_mm
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "joebot.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "joebot.mak" CFG="joebot - Win32 Debug_mm"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "joebot - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "joebot - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "joebot - Win32 Release_mm" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "joebot - Win32 Debug_mm" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "joebot - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release/win32/std"
# PROP Intermediate_Dir "release/win32/std"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "JOEBOT_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\dlls" /I "..\..\..\..\server\metamod\src\1.17\metamod" /I "..\common" /I "..\engine" /I "..\pm_shared" /I "..\NNSim" /I "..\NNSim\som" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /def:"joebot.def"
# ADD LINK32 /nologo /dll /map /machine:I386 /def:"joebot.def"
# Begin Custom Build
TargetPath=.\release\win32\std\joebot.dll
TargetName=joebot
InputPath=.\release\win32\std\joebot.dll
SOURCE="$(InputPath)"

BuildCmds= \
	copy $(TargetPath) "C:\Program Files\Steam\SteamApps\cruft@hotmail.com\counter-strike\cstrike\joebot" \
	copy $(TargetPath) "C:\Program Files\Half-Life\cstrike\joebot" \
	copy $(TargetPath) "C:\HLServer\cstrike\joebot" \
	

"C:\Program Files\Steam\SteamApps\cruft@hotmail.com\counter-strike\cstrike\joebot\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"C:\Program Files\Half-Life\cstrike\joebot\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"C:\HLServer\cstrike\joebot\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "joebot - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug/win32/std"
# PROP Intermediate_Dir "debug/win32/std"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "JOEBOT_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\..\..\..\server\metamod\src\1.17\metamod" /I "..\common" /I "..\engine" /I "..\pm_shared" /I "..\NNSim" /I "..\NNSim\som" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /def:"joebot.def" /pdbtype:sept
# ADD LINK32 /nologo /dll /map /debug /machine:I386 /def:"joebot.def" /pdbtype:sept
# SUBTRACT LINK32 /profile /nodefaultlib
# Begin Custom Build
TargetPath=.\debug\win32\std\joebot.dll
TargetName=joebot
InputPath=.\debug\win32\std\joebot.dll
SOURCE="$(InputPath)"

BuildCmds= \
	copy $(TargetPath) "C:\Program Files\Steam\SteamApps\cruft@hotmail.com\counter-strike\cstrike\joebot" \
	copy $(TargetPath) "C:\Program Files\Half-Life\cstrike\joebot" \
	copy $(TargetPath) "C:\HLServer\cstrike\joebot" \
	

"C:\Program Files\Steam\SteamApps\cruft@hotmail.com\counter-strike\cstrike\joebot\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"C:\Program Files\Half-Life\cstrike\joebot\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"C:\HLServer\cstrike\joebot\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "joebot - Win32 Release_mm"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "joebot___Win32_Release_MM"
# PROP BASE Intermediate_Dir "joebot___Win32_Release_MM"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release/win32/mm"
# PROP Intermediate_Dir "release/win32/mm"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "..\..\..\..\server\metamod\src\1.17\metamod" /I "..\common" /I "..\engine" /I "..\pm_shared" /I "..\NNSim" /I "..\NNSim\som" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\dlls" /I "..\..\..\..\server\metamod\src\1.17\metamod" /I "..\common" /I "..\engine" /I "..\pm_shared" /I "..\NNSim" /I "..\NNSim\som" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "USE_METAMOD" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "USE_METAMOD"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /def:"joebot.def"
# ADD LINK32 /nologo /dll /map /machine:I386 /def:"joebot.def" /out:"release/win32/mm/joebot_mm.dll"
# Begin Custom Build
TargetPath=.\release\win32\mm\joebot_mm.dll
TargetName=joebot_mm
InputPath=.\release\win32\mm\joebot_mm.dll
SOURCE="$(InputPath)"

BuildCmds= \
	copy $(TargetPath) "C:\Program Files\Steam\SteamApps\cruft@hotmail.com\counter-strike\cstrike\joebot" \
	copy $(TargetPath) "C:\Program Files\Half-Life\cstrike\joebot" \
	copy $(TargetPath) "C:\HLServer\cstrike\joebot" \
	

"C:\Program Files\Steam\SteamApps\cruft@hotmail.com\counter-strike\cstrike\joebot\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"C:\Program Files\Half-Life\cstrike\joebot\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"C:\HLServer\cstrike\joebot\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "joebot - Win32 Debug_mm"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "joebot___Win32_Debug_MM"
# PROP BASE Intermediate_Dir "joebot___Win32_Debug_MM"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug/win32/mm"
# PROP Intermediate_Dir "debug/win32/mm"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\server\metamod\src\1.17\metamod" /I "..\common" /I "..\engine" /I "..\pm_shared" /I "..\NNSim" /I "..\NNSim\som" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR"debug/" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\dlls" /I "..\..\..\..\server\metamod\src\1.17\metamod" /I "..\common" /I "..\engine" /I "..\pm_shared" /I "..\NNSim" /I "..\NNSim\som" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "USE_METAMOD" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "USE_METAMOD"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /def:"joebot.def" /pdbtype:sept
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 /nologo /dll /map /debug /machine:I386 /def:"joebot.def" /out:"debug/win32/mm/joebot_mm.dll" /pdbtype:sept
# SUBTRACT LINK32 /profile
# Begin Custom Build
TargetPath=.\debug\win32\mm\joebot_mm.dll
TargetName=joebot_mm
InputPath=.\debug\win32\mm\joebot_mm.dll
SOURCE="$(InputPath)"

BuildCmds= \
	copy $(TargetPath) "C:\Program Files\Steam\SteamApps\cruft@hotmail.com\counter-strike\cstrike\joebot" \
	copy $(TargetPath) "C:\Program Files\Half-Life\cstrike\joebot" \
	copy $(TargetPath) "C:\HLServer\cstrike\joebot" \
	

"C:\Program Files\Steam\SteamApps\cruft@hotmail.com\counter-strike\cstrike\joebot\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"C:\Program Files\Half-Life\cstrike\joebot\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"C:\HLServer\cstrike\joebot\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# Begin Target

# Name "joebot - Win32 Release"
# Name "joebot - Win32 Debug"
# Name "joebot - Win32 Release_mm"
# Name "joebot - Win32 Debug_mm"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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

SOURCE=.\basemem.cpp
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

SOURCE=.\bot.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_client.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_cs.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_names.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_navigate.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_wpdir.cpp
# End Source File
# Begin Source File

SOURCE=.\bot_wpstat.cpp
# End Source File
# Begin Source File

SOURCE=.\CBotBase.cpp
# End Source File
# Begin Source File

SOURCE=.\CBotCS.cpp
# End Source File
# Begin Source File

SOURCE=.\CBotCS_combat.cpp
# End Source File
# Begin Source File

SOURCE=.\CBotCS_navigate.cpp
# End Source File
# Begin Source File

SOURCE=.\CBotDOD.cpp
# End Source File
# Begin Source File

SOURCE=.\CBotDOD_combat.cpp
# End Source File
# Begin Source File

SOURCE=.\CBotDOD_navigate.cpp
# End Source File
# Begin Source File

SOURCE=.\CCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\Chat.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatBot.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatHost.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatHostData.cpp
# End Source File
# Begin Source File

SOURCE=.\CLTMem.cpp
# End Source File
# Begin Source File

SOURCE=.\Commandfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\CParser.cpp
# End Source File
# Begin Source File

SOURCE=.\CPersonality.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\CqSOM2d.cpp
# End Source File
# Begin Source File

SOURCE=.\CRadio.cpp
# End Source File
# Begin Source File

SOURCE=.\CSDecals.cpp
# End Source File
# Begin Source File

SOURCE=.\CSenses.cpp
# End Source File
# Begin Source File

SOURCE=.\CSkill.cpp
# End Source File
# Begin Source File

SOURCE=.\CState.cpp
# End Source File
# Begin Source File

SOURCE=.\CSTMem.cpp
# End Source File
# Begin Source File

SOURCE=.\CTask.cpp
# End Source File
# Begin Source File

SOURCE=.\dll.cpp
# End Source File
# Begin Source File

SOURCE=.\engine.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\fNBorKernel.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\Genom.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\GenomToken.cpp
# End Source File
# Begin Source File

SOURCE=.\Gnome.cpp
# End Source File
# Begin Source File

SOURCE=.\h_export.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\LinkBProp.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\LinkBPropM.cpp
# End Source File
# Begin Source File

SOURCE=.\linkfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\LListElem.cpp
# End Source File
# Begin Source File

SOURCE=.\namefunc.cpp
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

SOURCE=.\NNWeapon.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\nVec.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\PatternBProp.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\PatternElemBProp.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\SOMPattern.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\SOMPatternElem.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\waypoint.cpp
# End Source File
# Begin Source File

SOURCE=.\waypoint_advdataimpl.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\WeightBProp.cpp
# End Source File
# Begin Source File

SOURCE=..\NNSim\WeightBPropM.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldGnome.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\NNSim\Activations.h
# End Source File
# Begin Source File

SOURCE=.\activity.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseLink.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseLinkFF.h
# End Source File
# Begin Source File

SOURCE=.\basemem.h
# End Source File
# Begin Source File

SOURCE=.\basemonster.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseNeuralNet.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseNeuralNetFF.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseNeuron.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseNeuronFF.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BasePattern.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BasePatternElem.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BasePatternElemFF.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BasePatternFF.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseWeight.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BaseWeightFF.h
# End Source File
# Begin Source File

SOURCE=.\bot.h
# End Source File
# Begin Source File

SOURCE=.\bot_client.h
# End Source File
# Begin Source File

SOURCE=.\bot_func.h
# End Source File
# Begin Source File

SOURCE=.\bot_globaldefs.h
# End Source File
# Begin Source File

SOURCE=.\bot_modid.h
# End Source File
# Begin Source File

SOURCE=.\bot_names.h
# End Source File
# Begin Source File

SOURCE=.\bot_navigate.h
# End Source File
# Begin Source File

SOURCE=.\bot_weapons.h
# End Source File
# Begin Source File

SOURCE=.\bot_wpdir.h
# End Source File
# Begin Source File

SOURCE=.\bot_wpstat.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\BotNNDefs.h
# End Source File
# Begin Source File

SOURCE=.\cbase.h
# End Source File
# Begin Source File

SOURCE=.\CBotBase.h
# End Source File
# Begin Source File

SOURCE=.\CBotCS.h
# End Source File
# Begin Source File

SOURCE=.\CBotDOD.h
# End Source File
# Begin Source File

SOURCE=.\CCommand.h
# End Source File
# Begin Source File

SOURCE=.\cdll_dll.h
# End Source File
# Begin Source File

SOURCE=.\Chat.h
# End Source File
# Begin Source File

SOURCE=.\ChatBot.h
# End Source File
# Begin Source File

SOURCE=.\ChatHost.h
# End Source File
# Begin Source File

SOURCE=.\ChatHostData.h
# End Source File
# Begin Source File

SOURCE=.\CLTMem.h
# End Source File
# Begin Source File

SOURCE=.\Commandfunc.h
# End Source File
# Begin Source File

SOURCE=.\CParser.h
# End Source File
# Begin Source File

SOURCE=.\CPersonality.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\CqSOM2d.h
# End Source File
# Begin Source File

SOURCE=.\CRadio.h
# End Source File
# Begin Source File

SOURCE=.\CSDecals.h
# End Source File
# Begin Source File

SOURCE=.\CSenses.h
# End Source File
# Begin Source File

SOURCE=.\CSkill.h
# End Source File
# Begin Source File

SOURCE=.\CState.h
# End Source File
# Begin Source File

SOURCE=.\CSTMem.h
# End Source File
# Begin Source File

SOURCE=.\CTask.h
# End Source File
# Begin Source File

SOURCE=.\engine.h
# End Source File
# Begin Source File

SOURCE=.\enginecallback.h
# End Source File
# Begin Source File

SOURCE=.\extdll.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\fNBorKernel.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\GA.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\Genom.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\GenomToken.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\GlobalCqSOMDefs.h
# End Source File
# Begin Source File

SOURCE=.\globalvars.h
# End Source File
# Begin Source File

SOURCE=.\Gnome.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\LinkBProp.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\LinkBPropM.h
# End Source File
# Begin Source File

SOURCE=.\LListElem.h
# End Source File
# Begin Source File

SOURCE=.\monsterevent.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\NeuralNet.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\NeuralNetBProp.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\NeuralNetBPropM.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\NeuralNetDefs.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\NeuronBProp.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\NeuronBPropM.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\NNSimService.h
# End Source File
# Begin Source File

SOURCE=.\NNWeapon.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\nSOMErr.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\nVec.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\nVecErr.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\PatternBProp.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\PatternElemBProp.h
# End Source File
# Begin Source File

SOURCE=.\saverestore.h
# End Source File
# Begin Source File

SOURCE=.\schedule.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\som.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\SOMPattern.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\SOMPatternElem.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\som\SOMPatternErr.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# Begin Source File

SOURCE=.\vers_info.h
# End Source File
# Begin Source File

SOURCE=.\waypoint.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\WeightBProp.h
# End Source File
# Begin Source File

SOURCE=..\NNSim\WeightBPropM.h
# End Source File
# Begin Source File

SOURCE=.\WorldGnome.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\joebot.rc
# End Source File
# End Group
# End Target
# End Project
