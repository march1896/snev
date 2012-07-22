@ECHO OFF
SET OLDDIR=%CD%
SET BUILDDIR=%~dp0
if not exist %BUILDDIR%\MinGW mkdir %BUILDDIR%\MinGW
CD %BUILDDIR%\MinGW
cmake -G "MinGW Makefiles" ..\..\src

rem I have not figure out how to generate a pretty VS solution.
if not exist %BUILDDIR%\VS mkdir %BUILDDIR%\VS
@CD %BUILDDIR%\VS
cmake -G "Visual Studio 10" ..\..\src

CD %OLDDIR%
pause
