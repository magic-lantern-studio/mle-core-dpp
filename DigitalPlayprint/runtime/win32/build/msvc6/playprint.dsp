# Microsoft Developer Studio Project File - Name="playprint" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=playprint - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "playprint.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "playprint.mak" CFG="playprint - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "playprint - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "playprint - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "playprint - Win32 Release"

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
# ADD CPP /nologo /Zp8 /MT /W3 /GX /O2 /I "../../../common/include" /I "../../include" /I "$(MLE_HOME)/Core/util/common/include" /I "$(MLE_HOME)/Core/util/win32/include" /I "$(MLE_HOME)/Core/math/common/include" /I "$(MLE_HOME)/Core/math/win32/include" /I "$(MLE_HOME)/DigitalWorkprint/lib/common/include" /I "$(MLE_HOME)//DigitalWorkprint/lib/win32/include" /I "$(MLE_HOME)/DigitalPlayprint/lib/common/include" /I "$(MLE_HOME)/DigitalPlayprint/lib/win32/include" /D "MLE_NOT_DLL" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
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
PostBuild_Cmds=mkdir $(MLE_HOME)\lib\tools	copy Release\playprint.lib $(MLE_HOME)\lib\tools\playprint.lib	mkdir $(MLE_HOME)\include\mle	xcopy /y ..\..\..\common\include\mle\*.h $(MLE_HOME)\include\mle
# End Special Build Tool

!ELSEIF  "$(CFG)" == "playprint - Win32 Debug"

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
# ADD CPP /nologo /Zp8 /MTd /W3 /Gm /GX /ZI /Od /I "../../../common/include" /I "../../include" /I "$(MLE_HOME)/Core/util/common/include" /I "$(MLE_HOME)/Core/util/win32/include" /I "$(MLE_HOME)/Core/math/common/include" /I "$(MLE_HOME)/Core/math/win32/include" /I "$(MLE_HOME)/DigitalWorkprint/lib/common/include" /I "$(MLE_HOME)//DigitalWorkprint/lib/win32/include" /I "$(MLE_HOME)/DigitalPlayprint/lib/common/include" /I "$(MLE_HOME)/DigitalPlayprint/lib/win32/include" /D "MLE_DEBUG" /D "MLE_NOT_DLL" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/playprintd.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\playprintd.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Install Library
PostBuild_Cmds=mkdir $(MLE_HOME)\lib\tools	copy Debug\playprintd.lib $(MLE_HOME)\lib\tools\playprintd.lib	mkdir $(MLE_HOME)\include\mle	xcopy /y ..\..\..\common\include\mle\*.h $(MLE_HOME)\include\mle
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "playprint - Win32 Release"
# Name "playprint - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\common\src\agchk.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\chkfile.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\chktable.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\mrefchk.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\playprnt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\pp.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\ppinput.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\ppoutput.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\common\src\scenechk.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\common\include\mle\agchk.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\chkfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\chktable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\chunk.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\mrefchk.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\playprnt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\pp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\ppinput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\ppoutput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\common\include\mle\scenechk.h
# End Source File
# End Group
# End Target
# End Project
