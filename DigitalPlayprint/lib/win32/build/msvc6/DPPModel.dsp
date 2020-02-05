# Microsoft Developer Studio Project File - Name="DPPModel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DPPModel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DPPModel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DPPModel.mak" CFG="DPPModel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DPPModel - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DPPModel - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DPPModel - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../common/include" /I "../../include" /I "$(MLE_HOME)/Core/util/common/include" /I "$(MLE_HOME)/Core/util/win32/include" /I "$(MLE_HOME)/Core/math/common/include" /I "$(MLE_HOME)/CORE/math/win32/include" /I "$(MLE_HOME)/DigitalWorkprint/lib/common/include" /I "$(MLE_HOME)/DigitalWorkprint/lib/win32/include" /D "ML_CDECL" /D "MLE_NOT_DLL" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Install Library
PostBuild_Cmds=mkdir $(MLE_HOME)\lib\tools	copy Release\DPPModel.lib $(MLE_HOME)\lib\tools\DPPModel.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DPPModel - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../common/include" /I "../../include" /I "$(MLE_HOME)/Core/util/common/include" /I "$(MLE_HOME)/Core/util/win32/include" /I "$(MLE_HOME)/Core/math/common/include" /I "$(MLE_HOME)/CORE/math/win32/include" /I "$(MLE_HOME)/DigitalWorkprint/lib/common/include" /I "$(MLE_HOME)/DigitalWorkprint/lib/win32/include" /D "MLE_DEBUG" /D "ML_CDECL" /D "MLE_NOT_DLL" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/DPPModeld.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\DPPModeld.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Install Library
PostBuild_Cmds=mkdir $(MLE_HOME)\lib\tools	copy Debug\DPPModeld.lib $(MLE_HOME)\lib\tools\DPPModeld.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "DPPModel - Win32 Release"
# Name "DPPModel - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\common\src\DPPModel\DppActor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPModel\DppActorSet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPModel\DppGroup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPModel\DppGroupRef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPModel\DppProperty.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPModel\DppRoleAttachment.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPModel\DppRoleBinding.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPModel\DppScene.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPModel\DppSet.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppActor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppActorSet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppGroup.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppGroupRef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppProperty.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppRoleAttachment.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppRoleBinding.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppScene.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppSet.h
# End Source File
# End Group
# End Target
# End Project
