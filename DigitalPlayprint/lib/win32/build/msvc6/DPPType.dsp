# Microsoft Developer Studio Project File - Name="DPPType" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DPPType - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DPPType.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DPPType.mak" CFG="DPPType - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DPPType - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DPPType - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DPPType - Win32 Release"

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
PostBuild_Cmds=mkdir $(MLE_HOME)\lib\tools	copy Release\DPPType.lib $(MLE_HOME)\lib\tools\DPPType.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DPPType - Win32 Debug"

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
# ADD BSC32 /nologo /o"Debug/DPPTyped.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\DPPTyped.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Install Library
PostBuild_Cmds=mkdir $(MLE_HOME)\lib\tools	copy Debug\DPPTyped.lib $(MLE_HOME)\lib\tools\DPPTyped.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "DPPType - Win32 Release"
# Name "DPPType - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppArray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppEnum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppFloat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppFloat3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppFloat4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppFloatArray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppInt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppIntArray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppMatrix34.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppRotation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppScalar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppScalarArray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppTransform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppType.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppVector2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppVector3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppVector3Array.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppVector4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\DPPType\DppVoidStarType.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppArray.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppEnum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppFloat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppFloat3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppFloat4.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppFloatArray.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppInt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppIntArray.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppMatrix34.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppRotation.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppScalar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppScalarArray.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppTransform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppType.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppVector2.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppVector3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppVector3Array.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppVector4.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\DppVoidStarType.h
# End Source File
# End Group
# End Target
# End Project
