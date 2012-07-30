@ECHO OFF
SET OLDDIR=%CD%
SET BUILDDIR=%~dp0
if not exist %BUILDDIR%\MinGW_TEST mkdir %BUILDDIR%\MinGW_TEST
CD %BUILDDIR%\MinGW_TEST
cmake -G "MinGW Makefiles" ..\..\tests

if not exist %BUILDDIR%\VS mkdir %BUILDDIR%\VS
@CD %BUILDDIR%\VS
cmake -G "Visual Studio 10" ..\..\tests

CD %OLDDIR%
pause
