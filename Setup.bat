@echo off

set "ROOT_PATH=%~dp0"
set "SCRIPT_PATH=%ROOT_PATH%\Script"
echo Script path: %SCRIPT_PATH%
echo.

rem call Script/premake5.lua to build assignment frame
cd %SCRIPT_PATH%
"premake5.exe" vs2022

pause
