# 🚀 AWS Deployment Guide for cultivio.io

Complete step-by-step guide to deploy the Cultivio website to AWS with custom domain and HTTPS.

---

## 📋 Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                         cultivio.io                              │
│                              │                                   │
│                              ▼                                   │
│                    ┌─────────────────┐                          │
│                    │   CloudFront    │  ← HTTPS + CDN           │
│                    │   (CDN + SSL)   │                          │
│                    └────────┬────────┘                          │
│                              │                                   │
│                              ▼                                   │
│                    ┌─────────────────┐                          │
│                    │    S3 Bucket    │  ← Static Files          │
│                    │  (cultivio.io)  │                          │
│                    └─────────────────┘                          │
│                                                                  │
│  DNS: Route 53 or External DNS → CloudFront                     │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🛠️ Step 1: Create S3 Bucket

### Via AWS Console:

1. Go to **S3** → **Create bucket**
2. **Bucket name**: `cultivio.io` (must match domain!)
3. **Region**: `ap-south-1` (Mumbai)
4. **Uncheck** "Block all public access"
5. Check the acknowledgment box
6. Click **Create bucket**

### Via AWS CLI:

```bash
# Create bucket
aws s3 mb s3://cultivio.io --region ap-south-1

# Remove block public access
aws s3api put-public-access-block \
  --bucket cultivio.io \
  --public-access-block-configuration \
  "BlockPublicAcls=false,IgnorePublicAcls=false,BlockPublicPolicy=false,RestrictPublicBuckets=false"
```

---

## 🛠️ Step 2: Enable Static Website Hosting

### Via AWS Console:

1. Go to **S3** → **cultivio.io** bucket
2. Click **Properties** tab
3. Scroll to **Static website hosting** → **Edit**
4. Select **Enable**
5. **Index document**: `index.html`
6. **Error document**: `index.html`
7. Click **Save changes**
8. Note the **Bucket website endpoint** (you'll need this)

### Via AWS CLI:

```bash
aws s3 website s3://cultivio.io \
  --index-document index.html \
  --error-document index.html
```

---

## 🛠️ Step 3: Set Bucket Policy (Public Read)

### Via AWS Console:

1. Go to **S3** → **cultivio.io** → **Permissions** tab
2. Click **Bucket policy** → **Edit**
3. Paste this policy:

```json
{
    "Version": "2012-10-17",
    "Statement": [
        {
            "Sid": "PublicReadGetObject",
            "Effect": "Allow",
            "Principal": "*",
            "Action": "s3:GetObject",
            "Resource": "arn:aws:s3:::cultivio.io/*"
        }
    ]
}
```

4. Click **Save changes**

### Via AWS CLI:

```bash
aws s3api put-bucket-policy --bucket cultivio.io --policy '{
    "Version": "2012-10-17",
    "Statement": [
        {
            "Sid": "PublicReadGetObject",
            "Effect": "Allow",
            "Principal": "*",
            "Action": "s3:GetObject",
            "Resource": "arn:aws:s3:::cultivio.io/*"
        }
    ]
}'
```

---

## 🛠️ Step 4: Upload Website Files (Manual Test)

```bash
# From project root
cd website

# Sync all files to S3
aws s3 sync . s3://cultivio.io/ --delete

# Test the S3 website URL
# http://cultivio.io.s3-website.ap-south-1.amazonaws.com
```

---

## 🛠️ Step 5: Request SSL Certificate (ACM)

⚠️ **IMPORTANT**: Certificate MUST be in **us-east-1** region for CloudFront!

### Via AWS Console:

1. Switch region to **US East (N. Virginia) - us-east-1**
2. Go to **Certificate Manager (ACM)**
3. Click **Request certificate**
4. Select **Request a public certificate**
5. **Domain names**: 
   - `cultivio.io`
   - `*.cultivio.io` (for subdomains)
6. **Validation method**: DNS validation (recommended)
7. Click **Request**
8. Click on the certificate → **Create records in Route 53** (if using Route 53)
   
   OR add CNAME records manually to your DNS:
   ```
   _abc123.cultivio.io → _xyz789.acm-validations.aws.
   ```
9. Wait for status to change to **Issued** (5-30 minutes)

---

## 🛠️ Step 6: Create CloudFront Distribution

### Via AWS Console:

1. Go to **CloudFront** → **Create distribution**
2. **Origin Settings**:
   - **Origin domain**: Select your S3 bucket website endpoint
     - ⚠️ Don't select from dropdown! Manually enter:
     - `cultivio.io.s3-website.ap-south-1.amazonaws.com`
   - **Origin path**: Leave empty
   - **Protocol**: HTTP only (S3 website doesn't support HTTPS)

3. **Default Cache Behavior**:
   - **Viewer protocol policy**: Redirect HTTP to HTTPS
   - **Allowed HTTP methods**: GET, HEAD
   - **Cache policy**: CachingOptimized

4. **Settings**:
   - **Alternate domain name (CNAME)**: 
     - `cultivio.io`
     - `www.cultivio.io`
   - **Custom SSL certificate**: Select your ACM certificate
   - **Default root object**: `index.html`

5. Click **Create distribution**
6. Note the **Distribution domain name** (e.g., `d1234abcd.cloudfront.net`)
7. Note the **Distribution ID** (e.g., `E1ABCDEF123456`)

---

## 🛠️ Step 7: Configure DNS

### Option A: Using Route 53 (Recommended)

1. Go to **Route 53** → **Hosted zones** → **cultivio.io**
2. Create **A Record** (Alias):
   - **Record name**: Leave empty (for root domain)
   - **Record type**: A
   - **Alias**: Yes
   - **Route traffic to**: CloudFront distribution
   - Select your distribution
3. Create another **A Record** for `www`:
   - **Record name**: `www`
   - **Record type**: A
   - **Alias**: Yes
   - **Route traffic to**: CloudFront distribution

### Option B: External DNS Provider

Add these records:

| Type  | Name | Value |
|-------|------|-------|
| CNAME | www  | d1234abcd.cloudfront.net |
| ALIAS/ANAME | @ | d1234abcd.cloudfront.net |

> Note: If your DNS doesn't support ALIAS for root domain, use a redirect service.

---

## 🛠️ Step 8: Setup GitHub Actions

### 8.1 Create IAM User for GitHub Actions

1. Go to **IAM** → **Users** → **Create user**
2. **User name**: `github-actions-cultivio`
3. Click **Next**
4. **Attach policies directly**:
   - Create custom policy:

```json
{
    "Version": "2012-10-17",
    "Statement": [
        {
            "Effect": "Allow",
            "Action": [
                "s3:PutObject",
                "s3:GetObject",
                "s3:DeleteObject",
                "s3:ListBucket"
            ],
            "Resource": [
                "arn:aws:s3:::cultivio.io",
                "arn:aws:s3:::cultivio.io/*"
            ]
        },
        {
            "Effect": "Allow",
            "Action": [
                "cloudfront:CreateInvalidation"
            ],
            "Resource": "*"
        }
    ]
}
```

5. Click **Create user**
6. Go to **Security credentials** → **Create access key**
7. Select **Application running outside AWS**
8. Download the CSV or copy the keys

### 8.2 Add GitHub Secrets

1. Go to your GitHub repo → **Settings** → **Secrets and variables** → **Actions**
2. Add these secrets:

| Secret Name | Value |
|-------------|-------|
| `AWS_ACCESS_KEY_ID` | Your access key ID |
| `AWS_SECRET_ACCESS_KEY` | Your secret access key |

### 8.3 Move CI File to Correct Location

```bash
# Create workflows directory
mkdir -p .github/workflows

# Move the CI file
mv ci-frontend.yml .github/workflows/deploy-frontend.yml
```

### 8.4 Update CloudFront Distribution ID

Edit `.github/workflows/deploy-frontend.yml`:

```yaml
env:
  CLOUDFRONT_DIST_ID: 'E1ABCDEF123456'  # Your actual distribution ID
```

---

## 🛠️ Step 9: Test Deployment

### Push to main branch:

```bash
git add .
git commit -m "Setup AWS deployment"
git push origin main
```

### Check GitHub Actions:

1. Go to your repo → **Actions** tab
2. Watch the deployment workflow
3. Once complete, visit:
   - 🌐 https://cultivio.io
   - 📱 https://cultivio.io/app

---

## 📊 Cost Estimate (Monthly)

| Service | Free Tier | After Free Tier |
|---------|-----------|-----------------|
| S3 | 5GB storage, 20K requests | ~₹10-50/month |
| CloudFront | 1TB transfer, 10M requests | ~₹0-100/month |
| Route 53 | - | ₹50/month (hosted zone) |
| ACM | **FREE** | **FREE** |

**Estimated Total**: ₹50-200/month for low-medium traffic

---

## 🔧 Troubleshooting

### Issue: Certificate not showing in CloudFront

✅ **Solution**: Certificate must be in **us-east-1** region

### Issue: Access Denied on website

✅ **Solution**: Check bucket policy allows public read

### Issue: Old content after deployment

✅ **Solution**: 
```bash
aws cloudfront create-invalidation \
  --distribution-id YOUR_DIST_ID \
  --paths "/*"
```

### Issue: SSL certificate pending validation

✅ **Solution**: Add CNAME records to DNS, wait 5-30 mins

### Issue: 403 error on /app or subpaths

✅ **Solution**: Ensure `index.html` exists in each folder, or configure CloudFront custom error pages to redirect to `/index.html`

---

## 📁 Final Directory Structure

```
.github/
└── workflows/
    └── deploy-frontend.yml    # CI/CD pipeline

website/
├── index.html                 # Main landing page
├── css/
│   ├── variables.css
│   ├── base.css
│   ├── components.css
│   ├── layout.css
│   └── animations.css
├── js/
│   └── main.js
├── images/
│   └── favicon.svg
└── app/
    ├── index.html             # Mobile app
    ├── manifest.json
    └── icon.svg
```

---

## ✅ Quick Checklist

- [ ] S3 bucket created with name `cultivio.io`
- [ ] Static website hosting enabled
- [ ] Bucket policy set for public read
- [ ] SSL certificate issued in us-east-1
- [ ] CloudFront distribution created
- [ ] DNS records configured
- [ ] GitHub secrets added
- [ ] CI file moved to `.github/workflows/`
- [ ] CloudFront Distribution ID added to CI file
- [ ] First deployment successful
- [ ] Website accessible at https://cultivio.io

---

## 🎉 You're Done!

Your website is now live at:
- 🌐 **https://cultivio.io** - Main website
- 📱 **https://cultivio.io/app** - Mobile app (PWA)

The CI/CD pipeline will automatically deploy changes when you push to the `main` branch!

