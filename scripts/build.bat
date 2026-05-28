@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "ROOT_DIR=%%~fI"
set "BUILD_DIR=%ROOT_DIR%\build"
set "CONFIG=Debug"

if not "%~1"=="" set "CONFIG=%~1"

echo [build.bat] Root: %ROOT_DIR%
echo [build.bat] Config: %CONFIG%

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

cmake -S "%ROOT_DIR%" -B "%BUILD_DIR%"
if errorlevel 1 (
    echo [build.bat] Configure failed.
    exit /b 1
)

cmake --build "%BUILD_DIR%" --config "%CONFIG%"
if errorlevel 1 (
    echo [build.bat] Build failed.
    exit /b 1
)

echo [build.bat] Build finished successfully.
exit /b 0
