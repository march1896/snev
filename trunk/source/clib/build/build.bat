@ECHO OFF
SET OLDDIR=%CD%
SET BUILDDIR=%~dp0
if not exist %BUILDDIR%\MinGW mkdir %BUILDDIR%\MinGW
CD %BUILDDIR%\MinGW
cmake -G "MinGW Makefiles" ..\..\src

if not exist %BUILDDIR%\VS mkdir %BUILDDIR%\VS
@CD %BUILDDIR%\VS
cmake -G "Visual Studio 10" ..\..\src

if not exist %BUILDDIR%\CDT4 mkdir %BUILDDIR%\CDT4
@CD %BUILDDIR%\CDT4
cmake -G "Eclipse CDT4 - MinGW Makefiles" ..\..\src


CD %OLDDIR%
pause
