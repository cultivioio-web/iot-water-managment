# 🔌 Cultivio Website API

Serverless API endpoints for the Cultivio website.

## Endpoints

### `POST /api/contact`

Contact form submission handler.

#### Request Body
```json
{
  "name": "John Doe",
  "email": "john@example.com",
  "phone": "+91 9876543210",  // optional
  "subject": "Product Inquiry",
  "message": "I want to know more about AquaSense..."
}
```

#### Response
```json
{
  "success": true,
  "message": "Thank you! Your message has been sent."
}
```

#### Errors
- `400` - Validation errors
- `405` - Method not allowed
- `429` - Rate limited
- `500` - Server error

---

## Deployment Options

### Option 1: Vercel (Recommended)

1. Install Vercel CLI: `npm i -g vercel`
2. Set environment variables in Vercel dashboard:
   ```
   EMAIL_SERVICE=sendgrid
   EMAIL_API_KEY=SG.xxxxx
   EMAIL_FROM=noreply@cultivio.in
   EMAIL_TO=hello@cultivio.in
   ```
3. Deploy: `vercel --prod`

### Option 2: Netlify Functions

1. Move `contact.js` to `netlify/functions/contact.js`
2. Modify exports:
   ```js
   exports.handler = async (event, context) => {
     // ... same logic but return { statusCode, body }
   };
   ```
3. Set env vars in Netlify dashboard
4. Deploy via Netlify

### Option 3: AWS Lambda + API Gateway

1. Create Lambda function with `contact.js`
2. Add API Gateway trigger
3. Configure environment variables
4. Set up CORS in API Gateway

---

## Email Service Setup

### SendGrid
```bash
npm install @sendgrid/mail
```
```
EMAIL_SERVICE=sendgrid
EMAIL_API_KEY=SG.your-api-key
```

### Mailgun
```bash
npm install mailgun-js
```
```
EMAIL_SERVICE=mailgun
EMAIL_API_KEY=your-api-key
MAILGUN_DOMAIN=mg.yourdomain.com
```

### AWS SES
```bash
npm install aws-sdk
```
```
EMAIL_SERVICE=ses
AWS_REGION=ap-south-1
AWS_ACCESS_KEY_ID=xxx
AWS_SECRET_ACCESS_KEY=xxx
```

---

## Local Development

1. Set environment variables:
   ```bash
   export EMAIL_SERVICE=console
   ```

2. Run with Vercel dev:
   ```bash
   vercel dev
   ```

3. Test with curl:
   ```bash
   curl -X POST http://localhost:3000/api/contact \
     -H "Content-Type: application/json" \
     -d '{"name":"Test","email":"test@test.com","subject":"Test","message":"Hello World!"}'
   ```

---

## Rate Limiting

- 5 requests per minute per IP
- Returns `429 Too Many Requests` when exceeded

---

## Security Features

- ✅ Input validation
- ✅ HTML sanitization (XSS prevention)
- ✅ Rate limiting
- ✅ CORS headers
- ✅ Method restriction (POST only)

---

*Last Updated: December 3, 2025*

