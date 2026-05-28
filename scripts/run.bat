@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "ROOT_DIR=%%~fI"
set "CONFIG=Debug"
if not "%~1"=="" set "CONFIG=%~1"

set "APP_EXE=%ROOT_DIR%\build\bin\%CONFIG%\Passtore.exe"

echo [run.bat] Root: %ROOT_DIR%
echo [run.bat] Config: %CONFIG%

if not exist "%APP_EXE%" (
    echo [run.bat] Executable not found at %APP_EXE%
    echo [run.bat] Run scripts\build.bat %CONFIG% first.
    exit /b 1
)

echo [run.bat] Running %APP_EXE%
"%APP_EXE%"
set "APP_EXIT=%ERRORLEVEL%"
echo [run.bat] Application exited with code %APP_EXIT%
exit /b %APP_EXIT%
