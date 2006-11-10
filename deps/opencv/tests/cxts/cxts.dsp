# Microsoft Developer Studio Project File - Name="cxts" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=cxts - Win32 Debug64 Itanium
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cxts.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cxts.mak" CFG="cxts - Win32 Debug64 Itanium"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cxts - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cxts - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cxts - Win32 Release64" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cxts - Win32 Debug64" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cxts - Win32 Release64 Itanium" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cxts - Win32 Debug64 Itanium" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cxts - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\_temp\cxts_Rls"
# PROP Intermediate_Dir "..\..\_temp\cxts_Rls"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zi /O2 /I "..\..\cxcore\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "CVAPI_EXPORTS" /Yu"_cxts.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cxcore.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libmmd.lib" /out:"..\..\bin\cxts001.dll" /implib:"..\..\lib\cxts.lib" /libpath:"..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cxts - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\_temp\cxts_Dbg"
# PROP Intermediate_Dir "..\..\_temp\cxts_Dbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\cxcore\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "CVAPI_EXPORTS" /Yu"_cxts.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cxcored.lib /nologo /subsystem:windows /dll /pdb:"..\..\_temp\cxts_Dbg/cxts.pdb" /debug /machine:I386 /nodefaultlib:"libmmdd.lib" /out:"..\..\bin\cxts001d.dll" /implib:"..\..\lib\cxtsd.lib" /pdbtype:sept /libpath:"..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cxts - Win32 Release64"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "cxts___Win32_Release64"
# PROP BASE Intermediate_Dir "cxts___Win32_Release64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\_temp\cxts_Rls64"
# PROP Intermediate_Dir "..\..\_temp\cxts_Rls64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MD /W4 /GX /Zi /O2 /I "..\..\cxcore\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "CVAPI_EXPORTS" /Yu"_cxts.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zi /O2 /I "..\..\cxcore\include" /D "_WINDOWS" /D "CVAPI_EXPORTS" /D "NDEBUG" /D "WIN32" /D "WIN64" /D "EM64T" /Yu"_cxts.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cxcore.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /nodefaultlib:"libmmd.lib" /out:"..\..\bin\cxts001.dll" /implib:"..\..\lib\cxts.lib" /libpath:"..\..\lib" /machine:AMD64
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cxcore_64.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /nodefaultlib:"libmmd.lib" /out:"..\..\bin\cxts001_64.dll" /implib:"..\..\lib\cxts_64.lib" /libpath:"..\..\lib" /machine:AMD64
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cxts - Win32 Debug64"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cxts___Win32_Debug64"
# PROP BASE Intermediate_Dir "cxts___Win32_Debug64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\_temp\cxts_Dbg64"
# PROP Intermediate_Dir "..\..\_temp\cxts_Dbg64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\cxcore\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "CVAPI_EXPORTS" /Yu"_cxts.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\cxcore\include" /D "_WINDOWS" /D "CVAPI_EXPORTS" /D "_DEBUG" /D "WIN32" /D "WIN64" /D "EM64T" /Yu"_cxts.h" /FD /Wp64 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cxcored.lib /nologo /subsystem:windows /dll /pdb:"..\..\_temp\cxts_Dbg/cxts.pdb" /debug /machine:IX86 /nodefaultlib:"libmmdd.lib" /out:"..\..\bin\cxts001d.dll" /implib:"..\..\lib\cxtsd.lib" /libpath:"..\..\lib" /machine:AMD64
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cxcored_64.lib /nologo /subsystem:windows /dll /pdb:"..\..\bin/cxts001d_64.pdb" /debug /machine:IX86 /nodefaultlib:"libmmdd.lib" /out:"..\..\bin\cxts001d_64.dll" /implib:"..\..\lib\cxtsd_64.lib" /libpath:"..\..\lib" /machine:AMD64
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cxts - Win32 Release64 Itanium"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "cxts___Win32_Release64_Itanium"
# PROP BASE Intermediate_Dir "cxts___Win32_Release64_Itanium"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\_temp\cxts_RlsI7"
# PROP Intermediate_Dir "..\..\_temp\cxts_RlsI7"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MD /W4 /GX /Zi /O2 /I "..\..\cxcore\include" /D "_WINDOWS" /D "CVAPI_EXPORTS" /D "NDEBUG" /D "WIN32" /D "WIN64" /D "EM64T" /Yu"_cxts.h" /FD /c
# ADD CPP /nologo /MD /w /W0 /GX /Zi /O2 /I "..\..\cxcore\include" /D "_WINDOWS" /D "CVAPI_EXPORTS" /D "NDEBUG" /D "WIN32" /D "WIN64" /Yu"_cxts.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cxcore_i7.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /nodefaultlib:"libmmd.lib" /out:"..\..\bin\cxts001_i7.dll" /implib:"..\..\lib\cxts_i7.lib" /libpath:"..\..\lib" /machine:IA64
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cxcore_i7.lib /nologo /subsystem:windows /dll /debug /machine:IX86 /out:"..\..\bin\cxts001_i7.dll" /implib:"..\..\lib\cxts_i7.lib" /libpath:"..\..\lib" /machine:IA64
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cxts - Win32 Debug64 Itanium"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cxts___Win32_Debug64_Itanium"
# PROP BASE Intermediate_Dir "cxts___Win32_Debug64_Itanium"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\_temp\cxts_DbgI7"
# PROP Intermediate_Dir "..\..\_temp\cxts_DbgI7"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\..\cxcore\include" /D "_WINDOWS" /D "CVAPI_EXPORTS" /D "_DEBUG" /D "WIN32" /D "WIN64" /D "EM64T" /Yu"_cxts.h" /FD /Wp64 /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\..\cxcore\include" /D "_WINDOWS" /D "CVAPI_EXPORTS" /D "_DEBUG" /D "WIN32" /D "WIN64" /Yu"_cxts.h" /FD /Qwd167 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cxcored_i7.lib /nologo /subsystem:windows /dll /pdb:"..\..\bin/cxts001d_i7.pdb" /debug /machine:IX86 /nodefaultlib:"libmmdd.lib" /out:"..\..\bin\cxts001d_i7.dll" /implib:"..\..\lib\cxtsd_i7.lib" /libpath:"..\..\lib" /machine:IA64
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cxcored_i7.lib /nologo /subsystem:windows /dll /pdb:"..\..\bin/cxts001d_i7.pdb" /debug /machine:IX86 /out:"..\..\bin\cxts001d_i7.dll" /implib:"..\..\lib\cxtsd_i7.lib" /libpath:"..\..\lib" /machine:IA64
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "cxts - Win32 Release"
# Name "cxts - Win32 Debug"
# Name "cxts - Win32 Release64"
# Name "cxts - Win32 Debug64"
# Name "cxts - Win32 Release64 Itanium"
# Name "cxts - Win32 Debug64 Itanium"
# Begin Group "Src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\_cxts.h
# End Source File
# Begin Source File

SOURCE=.\cxts.cpp
# End Source File
# Begin Source File

SOURCE=.\cxts_arrtest.cpp
# End Source File
# Begin Source File

SOURCE=.\cxts_math.cpp
# End Source File
# Begin Source File

SOURCE=.\precomp.cpp
# ADD CPP /Yc"_cxts.h"
# End Source File
# End Group
# Begin Group "Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cxts.h
# End Source File
# End Group
# End Target
# End Project
