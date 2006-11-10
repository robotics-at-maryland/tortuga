# Microsoft Developer Studio Project File - Name="ml" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ml - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ml.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ml.mak" CFG="ml - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ml - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ml - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ml - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\_temp\ml_Rls"
# PROP Intermediate_Dir "..\..\_temp\ml_Rls"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ml_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /Zi /O2 /Ob2 /I "../include" /I "../../cxcore/include" /D "NDEBUG" /D "CVAPI_EXPORTS" /D "WIN32" /D "_WINDOWS" /Yu"_ml.h" /FD /Zm200 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 cxcore.lib kernel32.lib user32.lib gdi32.lib /nologo /dll /pdb:"..\..\bin/ml100.pdb" /debug /machine:I386 /nodefaultlib:"libmmd.lib" /out:"..\..\bin/ml100.dll" /implib:"..\..\lib/ml.lib" /libpath:"..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ml - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\_temp\ml_Dbg"
# PROP Intermediate_Dir "..\..\_temp\ml_Dbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ml_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /Zi /Od /I "../include" /I "../../cxcore/include" /D "_DEBUG" /D "CVAPI_EXPORTS" /D "WIN32" /D "_WINDOWS" /FR /Yu"_ml.h" /FD /GZ /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cxcored.lib kernel32.lib user32.lib gdi32.lib /nologo /dll /pdb:"..\..\bin/ml100d.pdb" /debug /machine:I386 /nodefaultlib:"libmmdd.lib" /out:"..\..\bin/ml100d.dll" /implib:"..\..\lib/mld.lib" /pdbtype:sept /libpath:"..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ml - Win32 Release"
# Name "ml - Win32 Debug"
# Begin Group "Src"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\ml.cpp

!IF  "$(CFG)" == "ml - Win32 Release"

# ADD CPP /Yc"_ml.h"

!ELSEIF  "$(CFG)" == "ml - Win32 Debug"

# ADD CPP /FR /Yc"_ml.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ml.rc
# End Source File
# Begin Source File

SOURCE=..\src\ml_inner_functions.cpp

!IF  "$(CFG)" == "ml - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ml - Win32 Debug"

# ADD CPP /FR /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mlann_mlp.cpp
# End Source File
# Begin Source File

SOURCE=.\mlboost.cpp
# End Source File
# Begin Source File

SOURCE=..\src\mlcnn.cpp
# End Source File
# Begin Source File

SOURCE=..\src\mlem.cpp

!IF  "$(CFG)" == "ml - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ml - Win32 Debug"

# ADD CPP /FR /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mlestimate.cpp
# End Source File
# Begin Source File

SOURCE=..\src\mlknearest.cpp

!IF  "$(CFG)" == "ml - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ml - Win32 Debug"

# ADD CPP /FR /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mlnbayes.cpp

!IF  "$(CFG)" == "ml - Win32 Release"

!ELSEIF  "$(CFG)" == "ml - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mlrtrees.cpp
# End Source File
# Begin Source File

SOURCE=..\src\mlsvm.cpp

!IF  "$(CFG)" == "ml - Win32 Release"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "ml - Win32 Debug"

# ADD CPP /FR /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\src\mltestset.cpp

!IF  "$(CFG)" == "ml - Win32 Release"

!ELSEIF  "$(CFG)" == "ml - Win32 Debug"

# ADD CPP /FR

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mltree.cpp
# End Source File
# End Group
# Begin Group "Include"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Internal"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\_ml.h
# End Source File
# End Group
# Begin Group "External"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\ml.h
# End Source File
# End Group
# End Group
# End Target
# End Project
