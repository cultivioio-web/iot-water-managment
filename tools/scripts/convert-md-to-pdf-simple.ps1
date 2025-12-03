# Simple Markdown to PDF Converter for Business Docs

Write-Host "============================================" -ForegroundColor Cyan
Write-Host "  Business Docs to PDF Converter" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

# Setup paths
$docsPath = "..\docs\business"
$outputPath = "..\docs\business\pdf"

# Create output directory
if (-not (Test-Path $outputPath)) {
    New-Item -ItemType Directory -Path $outputPath -Force | Out-Null
    Write-Host "[+] Created: $outputPath" -ForegroundColor Green
}

# Get markdown files
$files = Get-ChildItem -Path $docsPath -Filter "*.md" | Where-Object { $_.Name -ne "CONVERT_TO_PDF.md" }

Write-Host "[i] Found $($files.Count) files to convert" -ForegroundColor Yellow
Write-Host ""

# Check for pandoc
$pandoc = Get-Command pandoc -ErrorAction SilentlyContinue

if ($pandoc) {
    Write-Host "[OK] Pandoc found" -ForegroundColor Green
    Write-Host ""
    
    foreach ($file in $files) {
        $input = $file.FullName
        $output = Join-Path $outputPath ($file.BaseName + ".pdf")
        
        Write-Host "Converting: $($file.Name)" -ForegroundColor Cyan
        
        & pandoc $input -o $output --pdf-engine=xelatex -V geometry:margin=1in --toc -s 2>&1 | Out-Null
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "  Success: $($file.BaseName).pdf" -ForegroundColor Green
        } else {
            Write-Host "  Failed: $($file.Name)" -ForegroundColor Red
        }
    }
    
    Write-Host ""
    Write-Host "[DONE] Conversion complete!" -ForegroundColor Green
    Write-Host "Output: $outputPath" -ForegroundColor Cyan
    
    # Open folder
    Start-Process explorer.exe $outputPath
    
} else {
    Write-Host "[ERROR] Pandoc not found!" -ForegroundColor Red
    Write-Host ""
    Write-Host "Install Pandoc:" -ForegroundColor Yellow
    Write-Host "  choco install pandoc miktex -y" -ForegroundColor White
    Write-Host ""
    Write-Host "Or download from:" -ForegroundColor Yellow
    Write-Host "  https://pandoc.org/installing.html" -ForegroundColor White
    Write-Host ""
}

