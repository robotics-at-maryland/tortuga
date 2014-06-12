@REM Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007 The SCons Foundation
@REM /home/scons/scons/branch.0/branch.96/baseline/src/script/scons.bat 0.96.96.D001 2007/04/12 14:12:18 knight
@echo off
if "%OS%" == "Windows_NT" goto WinNT
REM for 9x/Me you better not have more than 9 args
python -c "from os.path import join; import sys; sys.path = [ join(sys.prefix, 'Lib', 'site-packages', 'scons-0.96.96'), join(sys.prefix, 'Lib', 'site-packages', 'scons'), join(sys.prefix, 'scons-0.96.96'), join(sys.prefix, 'scons')] + sys.path; import SCons.Script; SCons.Script.main()" %1 %2 %3 %4 %5 %6 %7 %8 %9
@REM no way to set exit status of this script for 9x/Me
goto endscons
:WinNT
@REM set path=%path%;%~dp0
python -c "from os.path import join; import sys; sys.path = [ join(sys.prefix, 'Lib', 'site-packages', 'scons-0.96.96'), join(sys.prefix, 'Lib', 'site-packages', 'scons'), join(sys.prefix, 'scons-0.96.96'), join(sys.prefix, 'scons')] + sys.path; import SCons.Script; SCons.Script.main()" %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endscons
if errorlevel 9009 echo you do not have python in your PATH
@REM color 00 causes this script to exit with non-zero exit status
if errorlevel 1 color 00
:endscons
