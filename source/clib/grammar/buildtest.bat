@ECHO OFF
SET OLDDIR=%CD%
SET BUILDDIR=%~dp0
if not exist %BUILDDIR%\MinGW mkdir %BUILDDIR%\MinGW
CD %BUILDDIR%\MinGW
cmake -G "MinGW Makefiles" ..\

if not exist %BUILDDIR%\VS mkdir %BUILDDIR%\VS
@CD %BUILDDIR%\VS
cmake -G "Visual Studio 10" ..\

CD %OLDDIR%
pause
