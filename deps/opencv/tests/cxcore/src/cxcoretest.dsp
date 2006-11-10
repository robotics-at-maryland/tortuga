# Microsoft Developer Studio Project File - Name="cxcoretest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=cxcoretest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cxcoretest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cxcoretest.mak" CFG="cxcoretest - Win32 Debug64 Itanium"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cxcoretest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "cxcoretest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "cxcoretest - Win32 Release64" (based on "Win32 (x86) Console Application")
!MESSAGE "cxcoretest - Win32 Debug64" (based on "Win32 (x86) Console Application")
!MESSAGE "cxcoretest - Win32 Release64 Itanium" (based on "Win32 (x86) Console Application")
!MESSAGE "cxcoretest - Win32 Debug64 Itanium" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "cxcoretest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\_temp\cxcoretest_Rls"
# PROP Intermediate_Dir "..\..\..\_temp\cxcoretest_Rls"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zi /O2 /I "." /I "..\..\cxts" /I "..\..\..\cxcore\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"cxcoretest.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 cxcore.lib cxts.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libmmd.lib" /out:"..\..\..\bin\cxcoretest.exe" /libpath:"..\..\..\lib"

!ELSEIF  "$(CFG)" == "cxcoretest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\_temp\cxcoretest_Dbg"
# PROP Intermediate_Dir "..\..\..\_temp\cxcoretest_Dbg"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "." /I "..\..\cxts" /I "..\..\..\cxcore\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"cxcoretest.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cxcored.lib cxtsd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libmmdd.lib" /out:"..\..\..\bin\cxcoretestd.exe" /pdbtype:sept /libpath:"..\..\..\lib"

!ELSEIF  "$(CFG)" == "cxcoretest - Win32 Release64"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "cxcoretest___Win32_Release64"
# PROP BASE Intermediate_Dir "cxcoretest___Win32_Release64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\_temp\cxcoretest_Rls64"
# PROP Intermediate_Dir "..\..\..\_temp\cxcoretest_Rls64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MD /W4 /GX /Zi /O2 /I "." /I "..\..\cxts" /I "..\..\..\cxcore\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"cxcoretest.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /Zi /O2 /I "." /I "..\..\cxts" /I "..\..\..\cxcore\include" /D "_CONSOLE" /D "_MBCS" /D "NDEBUG" /D "WIN32" /D "WIN64" /D "EM64T" /Yu"cxcoretest.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 cxcore.lib cxts.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:console /debug /machine:IX86 /nodefaultlib:"libmmd.lib" /out:"..\..\..\bin\cxcoretest.exe" /libpath:"..\..\..\lib" /machine:AMD64
# ADD LINK32 cxcore_64.lib cxts_64.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:console /debug /machine:IX86 /nodefaultlib:"libmmd.lib" /out:"..\..\..\bin\cxcoretest_64.exe" /libpath:"..\..\..\lib" /machine:AMD64
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cxcoretest - Win32 Debug64"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cxcoretest___Win32_Debug64"
# PROP BASE Intermediate_Dir "cxcoretest___Win32_Debug64"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\_temp\cxcoretest_Dbg64"
# PROP Intermediate_Dir "..\..\..\_temp\cxcoretest_Dbg64"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "." /I "..\..\cxts" /I "..\..\..\cxcore\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"cxcoretest.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "." /I "..\..\cxts" /I "..\..\..\cxcore\include" /D "_CONSOLE" /D "_MBCS" /D "_DEBUG" /D "WIN32" /D "WIN64" /D "EM64T" /Yu"cxcoretest.h" /FD /Wp64 /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 cxcored.lib cxtsd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:console /debug /machine:IX86 /nodefaultlib:"libmmdd.lib" /out:"..\..\..\bin\cxcoretestd.exe" /libpath:"..\..\..\lib" /machine:AMD64
# ADD LINK32 cxcored_64.lib cxtsd_64.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:console /pdb:"..\..\..\bin/cxcoretestd_64.pdb" /debug /machine:IX86 /nodefaultlib:"libmmdd.lib" /out:"..\..\..\bin\cxcoretestd_64.exe" /libpath:"..\..\..\lib" /machine:AMD64
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cxcoretest - Win32 Release64 Itanium"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "cxcoretest___Win32_Release64_Itanium"
# PROP BASE Intermediate_Dir "cxcoretest___Win32_Release64_Itanium"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\_temp\cxcoretest_RlsI7"
# PROP Intermediate_Dir "..\..\..\_temp\cxcoretest_RlsI7"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MD /W4 /GX /Zi /O2 /I "." /I "..\..\cxts" /I "..\..\..\cxcore\include" /D "_CONSOLE" /D "_MBCS" /D "NDEBUG" /D "WIN32" /D "WIN64" /D "EM64T" /Yu"cxcoretest.h" /FD /c
# ADD CPP /nologo /MD /w /W0 /GX /Zi /O2 /I "." /I "..\..\cxts" /I "..\..\..\cxcore\include" /D "_CONSOLE" /D "_MBCS" /D "NDEBUG" /D "WIN32" /D "WIN64" /Yu"cxcoretest.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 cxcore_i7.lib cxts_i7.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:console /debug /machine:IX86 /nodefaultlib:"libmmd.lib" /out:"..\..\..\bin\cxcoretest_i7.exe" /libpath:"..\..\..\lib" /machine:IA64
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 cxcore_i7.lib cxts_i7.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:console /debug /machine:IX86 /out:"..\..\..\bin\cxcoretest_i7.exe" /libpath:"..\..\..\lib" /machine:IA64
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cxcoretest - Win32 Debug64 Itanium"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cxcoretest___Win32_Debug64_Itanium"
# PROP BASE Intermediate_Dir "cxcoretest___Win32_Debug64_Itanium"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\_temp\cxcoretest_DbgI7"
# PROP Intermediate_Dir "..\..\..\_temp\cxcoretest_DbgI7"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "." /I "..\..\cxts" /I "..\..\..\cxcore\include" /D "_CONSOLE" /D "_MBCS" /D "_DEBUG" /D "WIN32" /D "WIN64" /D "EM64T" /Yu"cxcoretest.h" /FD /Wp64 /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I "..\..\cxts" /I "..\..\..\cxcore\include" /D "_CONSOLE" /D "_MBCS" /D "_DEBUG" /D "WIN32" /D "WIN64" /Yu"cxcoretest.h" /FD /Qwd167 /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 cxcored_i7.lib cxtsd_i7.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:console /pdb:"..\..\..\bin/cxcoretestd_i7.pdb" /debug /machine:IX86 /nodefaultlib:"libmmdd.lib" /out:"..\..\..\bin\cxcoretestd_i7.exe" /libpath:"..\..\..\lib" /machine:IA64
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 cxcored_i7.lib cxtsd_i7.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:console /pdb:"..\..\..\bin/cxcoretestd_i7.pdb" /debug /machine:IX86 /out:"..\..\..\bin\cxcoretestd_i7.exe" /libpath:"..\..\..\lib" /machine:IA64
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "cxcoretest - Win32 Release"
# Name "cxcoretest - Win32 Debug"
# Name "cxcoretest - Win32 Release64"
# Name "cxcoretest - Win32 Debug64"
# Name "cxcoretest - Win32 Release64 Itanium"
# Name "cxcoretest - Win32 Debug64 Itanium"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\aarithm.cpp
# End Source File
# Begin Source File

SOURCE=.\adatastruct.cpp
# End Source File
# Begin Source File

SOURCE=.\adxt.cpp
# End Source File
# Begin Source File

SOURCE=.\amath.cpp
# End Source File
# Begin Source File

SOURCE=.\cxcoretest_main.cpp
# End Source File
# Begin Source File

SOURCE=.\precomp.cpp
# ADD CPP /Yc"cxcoretest.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cxcoretest.h
# End Source File
# End Group
# End Target
# End Project
