@echo off
REM Cultivio Business Docs to PDF Converter (Batch)

echo ============================================
echo   Cultivio Business Docs to PDF Converter
echo ============================================
echo.

cd /d "%~dp0"

REM Check if pandoc is installed
where pandoc >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [+] Pandoc detected!
    echo.
    
    REM Create output directory
    if not exist "..\docs\business\pdf" mkdir "..\docs\business\pdf"
    
    REM Convert each file
    for %%f in (..\docs\business\*.md) do (
        echo Converting: %%~nf.md
        pandoc "%%f" -o "..\docs\business\pdf\%%~nf.pdf" --pdf-engine=xelatex -V geometry:margin=1in --toc -s
        if %ERRORLEVEL% EQU 0 (
            echo   [OK] %%~nf.pdf created
        ) else (
            echo   [ERROR] Failed to convert %%~nf.md
        )
    )
    
    echo.
    echo [+] Conversion complete!
    echo Opening output folder...
    start "" "..\docs\business\pdf"
    
) else (
    echo [!] Pandoc not found!
    echo.
    echo Please install Pandoc to convert MD to PDF:
    echo.
    echo Option 1: Download from https://pandoc.org/installing.html
    echo Option 2: Use Chocolatey: choco install pandoc miktex -y
    echo.
    echo After installation, run this script again.
    echo.
)

pause

