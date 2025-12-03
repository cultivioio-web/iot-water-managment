/**
 * Contact Form API Handler
 * 
 * This is a serverless function template that can be deployed to:
 * - Vercel (as-is)
 * - Netlify Functions (minor modifications)
 * - AWS Lambda (with API Gateway)
 * 
 * SETUP REQUIRED:
 * 1. Set environment variables:
 *    - EMAIL_SERVICE: 'sendgrid' | 'mailgun' | 'ses'
 *    - EMAIL_API_KEY: Your API key
 *    - EMAIL_FROM: sender@yourdomain.com
 *    - EMAIL_TO: hello@cultivio.in
 * 
 * 2. Deploy to your preferred platform
 * 3. Update the form action URL in the website
 */

// Rate limiting (simple in-memory, use Redis in production)
const rateLimitMap = new Map();
const RATE_LIMIT_WINDOW = 60 * 1000; // 1 minute
const RATE_LIMIT_MAX = 5; // 5 requests per minute

function checkRateLimit(ip) {
    const now = Date.now();
    const windowStart = now - RATE_LIMIT_WINDOW;
    
    if (!rateLimitMap.has(ip)) {
        rateLimitMap.set(ip, []);
    }
    
    const requests = rateLimitMap.get(ip).filter(time => time > windowStart);
    rateLimitMap.set(ip, requests);
    
    if (requests.length >= RATE_LIMIT_MAX) {
        return false;
    }
    
    requests.push(now);
    return true;
}

// Input validation
function validateInput(data) {
    const errors = [];
    
    // Name validation
    if (!data.name || data.name.trim().length < 2) {
        errors.push('Name must be at least 2 characters');
    }
    if (data.name && data.name.length > 100) {
        errors.push('Name must be less than 100 characters');
    }
    
    // Email validation
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    if (!data.email || !emailRegex.test(data.email)) {
        errors.push('Valid email address is required');
    }
    
    // Phone validation (optional, but if provided must be valid)
    if (data.phone) {
        const phoneRegex = /^[\d\s\-\+\(\)]{10,15}$/;
        if (!phoneRegex.test(data.phone)) {
            errors.push('Invalid phone number format');
        }
    }
    
    // Subject validation
    if (!data.subject || data.subject.trim().length < 3) {
        errors.push('Subject must be at least 3 characters');
    }
    if (data.subject && data.subject.length > 200) {
        errors.push('Subject must be less than 200 characters');
    }
    
    // Message validation
    if (!data.message || data.message.trim().length < 10) {
        errors.push('Message must be at least 10 characters');
    }
    if (data.message && data.message.length > 5000) {
        errors.push('Message must be less than 5000 characters');
    }
    
    return errors;
}

// Sanitize input to prevent XSS
function sanitizeInput(str) {
    if (!str) return '';
    return str
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;')
        .replace(/'/g, '&#039;');
}

// Send email (implement based on your email service)
async function sendEmail(data) {
    const emailService = process.env.EMAIL_SERVICE || 'console';
    
    const emailData = {
        from: process.env.EMAIL_FROM || 'noreply@cultivio.in',
        to: process.env.EMAIL_TO || 'hello@cultivio.in',
        subject: `[Cultivio Contact] ${data.subject}`,
        text: `
New contact form submission:

Name: ${data.name}
Email: ${data.email}
Phone: ${data.phone || 'Not provided'}
Subject: ${data.subject}

Message:
${data.message}

---
Submitted at: ${new Date().toISOString()}
        `,
        html: `
<h2>New Contact Form Submission</h2>
<table style="border-collapse: collapse; width: 100%;">
    <tr>
        <td style="padding: 8px; border: 1px solid #ddd;"><strong>Name</strong></td>
        <td style="padding: 8px; border: 1px solid #ddd;">${sanitizeInput(data.name)}</td>
    </tr>
    <tr>
        <td style="padding: 8px; border: 1px solid #ddd;"><strong>Email</strong></td>
        <td style="padding: 8px; border: 1px solid #ddd;"><a href="mailto:${sanitizeInput(data.email)}">${sanitizeInput(data.email)}</a></td>
    </tr>
    <tr>
        <td style="padding: 8px; border: 1px solid #ddd;"><strong>Phone</strong></td>
        <td style="padding: 8px; border: 1px solid #ddd;">${sanitizeInput(data.phone) || 'Not provided'}</td>
    </tr>
    <tr>
        <td style="padding: 8px; border: 1px solid #ddd;"><strong>Subject</strong></td>
        <td style="padding: 8px; border: 1px solid #ddd;">${sanitizeInput(data.subject)}</td>
    </tr>
</table>
<h3>Message:</h3>
<p style="white-space: pre-wrap; background: #f5f5f5; padding: 15px; border-radius: 5px;">${sanitizeInput(data.message)}</p>
<hr>
<p style="color: #666; font-size: 12px;">Submitted at: ${new Date().toISOString()}</p>
        `
    };
    
    switch (emailService) {
        case 'sendgrid':
            // SendGrid implementation
            const sgMail = require('@sendgrid/mail');
            sgMail.setApiKey(process.env.EMAIL_API_KEY);
            await sgMail.send(emailData);
            break;
            
        case 'mailgun':
            // Mailgun implementation
            const mailgun = require('mailgun-js');
            const mg = mailgun({
                apiKey: process.env.EMAIL_API_KEY,
                domain: process.env.MAILGUN_DOMAIN
            });
            await mg.messages().send(emailData);
            break;
            
        case 'ses':
            // AWS SES implementation
            const AWS = require('aws-sdk');
            const ses = new AWS.SES({ region: process.env.AWS_REGION || 'us-east-1' });
            await ses.sendEmail({
                Source: emailData.from,
                Destination: { ToAddresses: [emailData.to] },
                Message: {
                    Subject: { Data: emailData.subject },
                    Body: {
                        Text: { Data: emailData.text },
                        Html: { Data: emailData.html }
                    }
                }
            }).promise();
            break;
            
        case 'console':
        default:
            // Development: Just log to console
            console.log('=== EMAIL (Development Mode) ===');
            console.log('To:', emailData.to);
            console.log('Subject:', emailData.subject);
            console.log('Body:', emailData.text);
            console.log('================================');
            break;
    }
    
    return true;
}

// Main handler (Vercel/Netlify compatible)
module.exports = async function handler(req, res) {
    // CORS headers
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'POST, OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
    
    // Handle preflight
    if (req.method === 'OPTIONS') {
        return res.status(200).end();
    }
    
    // Only allow POST
    if (req.method !== 'POST') {
        return res.status(405).json({ 
            success: false, 
            error: 'Method not allowed' 
        });
    }
    
    try {
        // Get client IP for rate limiting
        const clientIp = req.headers['x-forwarded-for'] || 
                         req.connection?.remoteAddress || 
                         'unknown';
        
        // Check rate limit
        if (!checkRateLimit(clientIp)) {
            return res.status(429).json({
                success: false,
                error: 'Too many requests. Please try again later.'
            });
        }
        
        // Parse body
        const data = typeof req.body === 'string' ? JSON.parse(req.body) : req.body;
        
        // Validate input
        const errors = validateInput(data);
        if (errors.length > 0) {
            return res.status(400).json({
                success: false,
                errors: errors
            });
        }
        
        // Send email
        await sendEmail({
            name: data.name.trim(),
            email: data.email.trim().toLowerCase(),
            phone: data.phone?.trim() || '',
            subject: data.subject.trim(),
            message: data.message.trim()
        });
        
        // Success response
        return res.status(200).json({
            success: true,
            message: 'Thank you! Your message has been sent. We\'ll get back to you soon.'
        });
        
    } catch (error) {
        console.error('Contact form error:', error);
        return res.status(500).json({
            success: false,
            error: 'An error occurred. Please try again or email us directly at hello@cultivio.in'
        });
    }
};

