# ============================================
# Cultivio Website - Deploy to S3
# ============================================

param(
    [string]$BucketName = "cultivio.io",
    [string]$Region = "ap-south-1",
    [switch]$CreateBucket,
    [switch]$DryRun
)

$ErrorActionPreference = "Stop"
$WebsiteDir = Join-Path $PSScriptRoot "..\website"

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   Cultivio Website S3 Deployment" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check AWS CLI
Write-Host "[1/6] Checking AWS CLI..." -ForegroundColor Yellow
try {
    $awsVersion = aws --version 2>&1
    Write-Host "      AWS CLI: $awsVersion" -ForegroundColor Green
} catch {
    Write-Host "      ERROR: AWS CLI not found. Install from https://aws.amazon.com/cli/" -ForegroundColor Red
    exit 1
}

# Check credentials
Write-Host "[2/6] Checking AWS credentials..." -ForegroundColor Yellow
try {
    $identity = aws sts get-caller-identity --output json 2>&1 | ConvertFrom-Json
    Write-Host "      Account: $($identity.Account)" -ForegroundColor Green
    Write-Host "      User: $($identity.Arn)" -ForegroundColor Green
} catch {
    Write-Host "      ERROR: AWS credentials not configured or invalid" -ForegroundColor Red
    Write-Host ""
    Write-Host "      Run: aws configure" -ForegroundColor Yellow
    Write-Host "      Enter your Access Key ID and Secret Access Key" -ForegroundColor Yellow
    exit 1
}

# Check website directory
Write-Host "[3/6] Checking website files..." -ForegroundColor Yellow
if (-not (Test-Path $WebsiteDir)) {
    Write-Host "      ERROR: Website directory not found: $WebsiteDir" -ForegroundColor Red
    exit 1
}
$fileCount = (Get-ChildItem -Path $WebsiteDir -Recurse -File).Count
Write-Host "      Found $fileCount files in website/" -ForegroundColor Green

# Create bucket if requested
if ($CreateBucket) {
    Write-Host "[4/6] Creating S3 bucket..." -ForegroundColor Yellow
    try {
        # Check if bucket exists
        $bucketExists = aws s3api head-bucket --bucket $BucketName 2>&1
        Write-Host "      Bucket already exists: $BucketName" -ForegroundColor Green
    } catch {
        Write-Host "      Creating bucket: $BucketName" -ForegroundColor Cyan
        
        # Create bucket (different syntax for us-east-1)
        if ($Region -eq "us-east-1") {
            aws s3api create-bucket --bucket $BucketName
        } else {
            aws s3api create-bucket --bucket $BucketName --region $Region --create-bucket-configuration LocationConstraint=$Region
        }
        
        # Disable block public access
        Write-Host "      Configuring public access..." -ForegroundColor Cyan
        aws s3api put-public-access-block --bucket $BucketName --public-access-block-configuration "BlockPublicAcls=false,IgnorePublicAcls=false,BlockPublicPolicy=false,RestrictPublicBuckets=false"
        
        # Enable static website hosting
        Write-Host "      Enabling static website hosting..." -ForegroundColor Cyan
        aws s3 website "s3://$BucketName" --index-document index.html --error-document index.html
        
        # Set bucket policy
        Write-Host "      Setting bucket policy..." -ForegroundColor Cyan
        $policy = @"
{
    "Version": "2012-10-17",
    "Statement": [
        {
            "Sid": "PublicReadGetObject",
            "Effect": "Allow",
            "Principal": "*",
            "Action": "s3:GetObject",
            "Resource": "arn:aws:s3:::$BucketName/*"
        }
    ]
}
"@
        $policy | aws s3api put-bucket-policy --bucket $BucketName --policy file:///dev/stdin
        
        Write-Host "      Bucket created and configured!" -ForegroundColor Green
    }
} else {
    Write-Host "[4/6] Skipping bucket creation (use -CreateBucket to create)" -ForegroundColor Gray
}

# Sync files
Write-Host "[5/6] Uploading files to S3..." -ForegroundColor Yellow

if ($DryRun) {
    Write-Host "      DRY RUN - showing what would be uploaded:" -ForegroundColor Cyan
    aws s3 sync $WebsiteDir "s3://$BucketName/" --delete --dryrun
} else {
    aws s3 sync $WebsiteDir "s3://$BucketName/" --delete --exclude ".git/*" --exclude "*.md"
    
    # Set content types
    Write-Host "      Setting content types..." -ForegroundColor Cyan
    
    # HTML files - no cache
    aws s3 cp "s3://$BucketName/" "s3://$BucketName/" --recursive --exclude "*" --include "*.html" --content-type "text/html; charset=utf-8" --metadata-directive REPLACE --cache-control "no-cache, no-store, must-revalidate" 2>$null
    
    # CSS files
    aws s3 cp "s3://$BucketName/" "s3://$BucketName/" --recursive --exclude "*" --include "*.css" --content-type "text/css; charset=utf-8" --metadata-directive REPLACE --cache-control "max-age=31536000" 2>$null
    
    # JS files
    aws s3 cp "s3://$BucketName/" "s3://$BucketName/" --recursive --exclude "*" --include "*.js" --content-type "application/javascript; charset=utf-8" --metadata-directive REPLACE --cache-control "max-age=31536000" 2>$null
    
    # JSON files
    aws s3 cp "s3://$BucketName/" "s3://$BucketName/" --recursive --exclude "*" --include "*.json" --content-type "application/json; charset=utf-8" --metadata-directive REPLACE --cache-control "no-cache" 2>$null
    
    # SVG files
    aws s3 cp "s3://$BucketName/" "s3://$BucketName/" --recursive --exclude "*" --include "*.svg" --content-type "image/svg+xml" --metadata-directive REPLACE --cache-control "max-age=31536000" 2>$null
    
    Write-Host "      Upload complete!" -ForegroundColor Green
}

# Summary
Write-Host "[6/6] Deployment Summary" -ForegroundColor Yellow
Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "   Deployment Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "   S3 Website URL:" -ForegroundColor Cyan
Write-Host "   http://$BucketName.s3-website.$Region.amazonaws.com" -ForegroundColor White
Write-Host ""
Write-Host "   Mobile App:" -ForegroundColor Cyan
Write-Host "   http://$BucketName.s3-website.$Region.amazonaws.com/app" -ForegroundColor White
Write-Host ""

if (-not $CreateBucket) {
    Write-Host "   Note: If bucket doesn't exist, run with -CreateBucket flag" -ForegroundColor Yellow
    Write-Host ""
}

Write-Host "========================================" -ForegroundColor Green

