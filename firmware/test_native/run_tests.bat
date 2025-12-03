@echo off
REM Cultivio AquaSense - Native Unit Test Runner
REM Compiles and runs unit tests on Windows

echo.
echo ========================================
echo Cultivio AquaSense - Unit Test Runner
echo ========================================
echo.

REM Check for GCC
where gcc >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: GCC not found in PATH
    echo.
    echo Please install MinGW-w64:
    echo   1. winget install -e --id MSYS2.MSYS2
    echo   2. Open MSYS2 and run: pacman -S mingw-w64-x86_64-gcc
    echo   3. Add C:\msys64\mingw64\bin to PATH
    echo.
    exit /b 1
)

echo [1/3] Compiling tests...
gcc -o test_all.exe test_all.c -I./mocks -Wall -Wextra
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo COMPILE ERROR: Check the output above
    exit /b 1
)

echo [2/3] Running tests...
echo.
test_all.exe
set TEST_RESULT=%ERRORLEVEL%

echo.
echo [3/3] Cleanup...
del test_all.exe 2>nul

if %TEST_RESULT% EQU 0 (
    echo.
    echo SUCCESS: All tests passed!
    exit /b 0
) else (
    echo.
    echo FAILURE: Some tests failed!
    exit /b 1
)

