@ECHO OFF
IF x%MINGW% == "x" GOTO MINGW_NOT_DEFINED
SET PATH=%PATH%;%MINGW%\bin

build_platform.bat "MinGW Makefiles"
goto END

:MINGW_NOT_DEFINED
echo MINGW not defined!
goto END

:END
pause
