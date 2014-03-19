rem use one of the below options to generate the make system.
rem cmake_build.bat "MinGW Makefiles"
rem cmake_build.bat "Eclipse CDT4 - MinGW Makefiles"
rem cmake_build.bat "Visual Studio 10"
rem cmake_build.bat "Visual Studio 11"

@ECHO OFF
if exist set_env.bat call set_env.bat

IF %1=="" GOTO PLATFORM_NOT_DEFINED
SET PLATFORM=%1 

IF x%VCMAKE% == "x" GOTO VCMAKE_NOT_DEFINED
SET PATH=%PATH%;%VCMAKE%\bin

IF %PLATFORM%=="MinGW Makefiles" SET OUT_DIR=MinGW
IF %PLATFORM%=="Visual Studio 10" SET OUT_DIR=VS10
IF %PLATFORM%=="Visual Studio 11" SET OUT_DIR=VS11
IF %PLATFORM%=="Eclipse CDT4 - MinGW Makefiles" SET OUT_DIR=ECLIPSE

SET OLDDIR=%CD%

SET BUILDDIR=%~dp0

IF NOT EXIST %BUILDDIR%%OUT_DIR% mkdir %BUILDDIR%%OUT_DIR%
CD %BUILDDIR%%OUT_DIR%

cmake -G %PLATFORM% ..\..\src
CD %OLDDIR%
pause
goto END

:PLATFORM_NOT_DEFINED
echo Platform not defined!
echo must one of:
echo   MinGW Makefiles
echo   Visual Studio 10
echo   Visual Studio 11
echo   Eclipse CDT4 - MinGW Makefiles
goto END

:VCMAKE_NOT_DEFINED
echo VCMAKE not defined!
goto END

:END
