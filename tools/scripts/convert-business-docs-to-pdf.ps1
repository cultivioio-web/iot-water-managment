# Cultivio Business Docs - Markdown to PDF Converter
# Converts all business documentation from MD to PDF format

Write-Host "==============================================" -ForegroundColor Cyan
Write-Host "  Cultivio Business Docs to PDF Converter" -ForegroundColor Cyan
Write-Host "==============================================" -ForegroundColor Cyan
Write-Host ""

# Configuration
$docsPath = Join-Path $PSScriptRoot "..\docs\business"
$outputPath = Join-Path $PSScriptRoot "..\docs\business\pdf"

# Create output directory if it doesn't exist
if (-not (Test-Path $outputPath)) {
    New-Item -ItemType Directory -Path $outputPath -Force | Out-Null
    Write-Host "[+] Created output directory: $outputPath" -ForegroundColor Green
}

# Get all markdown files
$markdownFiles = Get-ChildItem -Path $docsPath -Filter "*.md"

Write-Host "[i] Found $($markdownFiles.Count) markdown files to convert" -ForegroundColor Yellow
Write-Host ""

# Check if pandoc is installed
$pandocInstalled = Get-Command pandoc -ErrorAction SilentlyContinue

if ($pandocInstalled) {
    Write-Host "[✓] Pandoc detected - using high-quality PDF conversion" -ForegroundColor Green
    Write-Host ""
    
    foreach ($file in $markdownFiles) {
        $inputFile = $file.FullName
        $outputFile = Join-Path $outputPath ($file.BaseName + ".pdf")
        
        Write-Host "[→] Converting: $($file.Name)" -ForegroundColor Cyan
        
        try {
            # Pandoc with nice formatting
            pandoc $inputFile `
                -o $outputFile `
                --pdf-engine=xelatex `
                -V geometry:margin=1in `
                -V fontsize=11pt `
                -V documentclass=article `
                -V colorlinks=true `
                --toc `
                --toc-depth=3 `
                -s `
                2>&1 | Out-Null
                
            if ($LASTEXITCODE -eq 0) {
                Write-Host "    [✓] Successfully created: $($file.BaseName).pdf" -ForegroundColor Green
            } else {
                Write-Host "    [✗] Failed to convert: $($file.Name)" -ForegroundColor Red
            }
        } catch {
            Write-Host "    [✗] Error: $_" -ForegroundColor Red
        }
    }
    
} else {
    Write-Host "[!] Pandoc not found. Installing via Chocolatey or providing alternatives..." -ForegroundColor Yellow
    Write-Host ""
    
    # Check if Chocolatey is available
    $chocoInstalled = Get-Command choco -ErrorAction SilentlyContinue
    
    if ($chocoInstalled) {
        Write-Host "[i] Chocolatey detected. Do you want to install Pandoc? (Y/N)" -ForegroundColor Yellow
        $response = Read-Host
        
        if ($response -eq 'Y' -or $response -eq 'y') {
            Write-Host "[→] Installing Pandoc and dependencies..." -ForegroundColor Cyan
            choco install pandoc miktex -y
            
            Write-Host "[✓] Pandoc installed! Please run this script again." -ForegroundColor Green
            exit
        }
    }
    
    # Alternative: Use markdown-pdf via npm
    $npmInstalled = Get-Command npm -ErrorAction SilentlyContinue
    
    if ($npmInstalled) {
        Write-Host "[i] NPM detected. Using markdown-pdf package..." -ForegroundColor Yellow
        
        # Check if markdown-pdf is installed
        $mdPdfInstalled = npm list -g markdown-pdf 2>&1 | Select-String "markdown-pdf"
        
        if (-not $mdPdfInstalled) {
            Write-Host "[→] Installing markdown-pdf..." -ForegroundColor Cyan
            npm install -g markdown-pdf
        }
        
        foreach ($file in $markdownFiles) {
            $inputFile = $file.FullName
            $outputFile = Join-Path $outputPath ($file.BaseName + ".pdf")
            
            Write-Host "[→] Converting: $($file.Name)" -ForegroundColor Cyan
            
            try {
                markdown-pdf $inputFile -o $outputFile
                Write-Host "    [✓] Successfully created: $($file.BaseName).pdf" -ForegroundColor Green
            } catch {
                Write-Host "    [✗] Error: $_" -ForegroundColor Red
            }
        }
        
    } else {
        # Manual conversion instructions
        Write-Host ""
        Write-Host "=============================================" -ForegroundColor Red
        Write-Host "  MANUAL CONVERSION REQUIRED" -ForegroundColor Red
        Write-Host "=============================================" -ForegroundColor Red
        Write-Host ""
        Write-Host "Neither Pandoc nor NPM is installed." -ForegroundColor Yellow
        Write-Host ""
        Write-Host "Please choose one of these options:" -ForegroundColor White
        Write-Host ""
        Write-Host "OPTION 1: Install Pandoc (Recommended)" -ForegroundColor Cyan
        Write-Host "  1. Download from: https://pandoc.org/installing.html" -ForegroundColor White
        Write-Host "  2. Install MiKTeX from: https://miktex.org/download" -ForegroundColor White
        Write-Host "  3. Run this script again" -ForegroundColor White
        Write-Host ""
        Write-Host "OPTION 2: Install via Chocolatey" -ForegroundColor Cyan
        Write-Host "  choco install pandoc miktex -y" -ForegroundColor White
        Write-Host ""
        Write-Host "OPTION 3: Online Conversion" -ForegroundColor Cyan
        Write-Host "  1. Visit: https://md2pdf.netlify.app/" -ForegroundColor White
        Write-Host "  2. Upload each .md file" -ForegroundColor White
        Write-Host "  3. Download the PDF" -ForegroundColor White
        Write-Host ""
        Write-Host "OPTION 4: VS Code Extension" -ForegroundColor Cyan
        Write-Host "  1. Install 'Markdown PDF' extension" -ForegroundColor White
        Write-Host "  2. Open each .md file" -ForegroundColor White
        Write-Host "  3. Right-click → 'Markdown PDF: Export (pdf)'" -ForegroundColor White
        Write-Host ""
        Write-Host "Files to convert:" -ForegroundColor Yellow
        foreach ($file in $markdownFiles) {
            Write-Host "  - $($file.Name)" -ForegroundColor White
        }
    }
}

Write-Host ""
Write-Host "==============================================" -ForegroundColor Cyan
Write-Host "  Conversion Complete!" -ForegroundColor Cyan
Write-Host "==============================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Output location: $outputPath" -ForegroundColor Green
Write-Host ""

# Open output folder
if (Test-Path $outputPath) {
    $pdfFiles = Get-ChildItem -Path $outputPath -Filter "*.pdf"
    if ($pdfFiles.Count -gt 0) {
        Write-Host "[✓] Generated $($pdfFiles.Count) PDF files:" -ForegroundColor Green
        foreach ($pdf in $pdfFiles) {
            Write-Host "    - $($pdf.Name)" -ForegroundColor White
        }
        Write-Host ""
        Write-Host "Opening output folder..." -ForegroundColor Cyan
        Start-Process explorer.exe $outputPath
    }
}

