@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "ROOT_DIR=%%~fI"
set "CONFIG=Debug"
if not "%~1"=="" set "CONFIG=%~1"

set "TEST_EXE=%ROOT_DIR%\build\bin\%CONFIG%\PasstoreLibTest.exe"

echo [test.bat] Root: %ROOT_DIR%
echo [test.bat] Config: %CONFIG%

call "%SCRIPT_DIR%build.bat" "%CONFIG%"
if errorlevel 1 (
    echo [test.bat] Build failed.
    exit /b 1
)

if not exist "%TEST_EXE%" (
    echo [test.bat] Error: test executable not found at %TEST_EXE%
    exit /b 1
)

echo [test.bat] Running %TEST_EXE%
"%TEST_EXE%"
if errorlevel 1 (
    echo [test.bat] Tests failed.
    exit /b 1
)

echo [test.bat] Tests finished successfully.
exit /b 0
