# Cultivio AquaSense - Native Unit Test Runner (PowerShell)
# Compiles and runs unit tests on Windows

Write-Host ""
Write-Host "========================================"
Write-Host "Cultivio AquaSense - Unit Test Runner"
Write-Host "========================================"
Write-Host ""

# Check for GCC
$gccPath = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gccPath) {
    Write-Host "ERROR: GCC not found in PATH" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please install MinGW-w64:"
    Write-Host "  1. winget install -e --id MSYS2.MSYS2"
    Write-Host "  2. Open MSYS2 and run: pacman -S mingw-w64-x86_64-gcc"
    Write-Host "  3. Add C:\msys64\mingw64\bin to PATH"
    Write-Host ""
    exit 1
}

Write-Host "[1/3] Compiling tests..." -ForegroundColor Cyan

$compileResult = & gcc -o test_all.exe test_all.c -I./mocks -Wall -Wextra 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "COMPILE ERROR:" -ForegroundColor Red
    Write-Host $compileResult
    exit 1
}

Write-Host "[2/3] Running tests..." -ForegroundColor Cyan
Write-Host ""

& .\test_all.exe
$testResult = $LASTEXITCODE

Write-Host ""
Write-Host "[3/3] Cleanup..." -ForegroundColor Cyan
Remove-Item test_all.exe -ErrorAction SilentlyContinue

if ($testResult -eq 0) {
    Write-Host ""
    Write-Host "SUCCESS: All tests passed!" -ForegroundColor Green
    exit 0
} else {
    Write-Host ""
    Write-Host "FAILURE: Some tests failed!" -ForegroundColor Red
    exit 1
}

