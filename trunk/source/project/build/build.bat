@ECHO OFF
SET OLDDIR=%CD%
SET BUILDDIR=%~dp0
CD %BUILDDIR%
if not exist .\MinGW mkdir MinGW
CD MinGW
cmake -G "MinGW Makefiles" ../../src

rem I have not figure out how to generate a pretty VS solution.
rem if not exist .\VS mkdir VS
rem @CD ../VS
rem cmake -G "Visual Studio 10" ../../src

CD %OLDDIR%
pause
