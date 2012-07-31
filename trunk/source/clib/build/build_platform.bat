@ECHO OFF
IF %1=="" GOTO END
SET PLATFORM=%1 
rem here we may pass other parameters

IF %PLATFORM%=="MinGW Makefiles" SET OUT_DIR=MinGW
IF %PLATFORM%=="Visual Studio 10" SET OUT_DIR=VS10
IF %PLATFORM%=="Eclipse CDT4 - MinGW Makefiles" SET OUT_DIR=ECLIPSE

SET OLDDIR=%CD%
SET BUILDDIR=%~dp0
IF NOT EXIST %BUILDDIR%%OUT_DIR% mkdir %BUILDDIR%%OUT_DIR%

CD %BUILDDIR%%OUT_DIR%

cmake -G %PLATFORM% ..\..\src

CD %OLDDIR%
pause
:END
