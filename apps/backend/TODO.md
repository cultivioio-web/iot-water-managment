# ☁️ Backend Services - TODO

**Status:** 🟡 In Progress  
**Priority:** Medium  
**Target:** Q1 2026

---

## 📋 Tasks

### ✅ Completed
- [x] Contact form API handler (`contact.js`)
- [x] Input validation & sanitization
- [x] Rate limiting (5 req/min/IP)
- [x] Email service integration templates (SendGrid, Mailgun, SES)
- [x] API documentation (`README.md`)

### ⏳ Pending - Phase 1 (This Month)

#### Contact Form Deployment (High Priority)
- [ ] Choose deployment platform (Vercel/Netlify/AWS)
- [ ] Set up email service (SendGrid recommended)
- [ ] Configure environment variables
- [ ] Deploy and test
- [ ] Update website form action URL

#### User Authentication (Medium Priority)
- [ ] Choose auth provider (Firebase Auth / Cognito)
- [ ] Implement user registration
- [ ] Implement login/logout
- [ ] Password reset flow
- [ ] JWT token handling

### ⏳ Pending - Phase 2 (Next Month)

#### Device Management API
- [ ] Device registration endpoint
- [ ] Device status endpoint
- [ ] Device configuration endpoint
- [ ] Device history endpoint

#### Real-time Data
- [ ] WebSocket or MQTT setup
- [ ] Live device status updates
- [ ] Push notifications

### ⏳ Pending - Phase 3 (Q1 2026)

#### Subscription System
- [ ] Stripe/Razorpay integration
- [ ] Subscription plans API
- [ ] Payment webhooks
- [ ] Usage tracking

#### Cloud Dashboard
- [ ] Admin panel API
- [ ] Analytics endpoints
- [ ] User management
- [ ] Bulk operations

---

## 📁 Current Files

| File | Purpose | Status |
|------|---------|--------|
| `contact.js` | Contact form handler | ✅ Ready |
| `README.md` | API documentation | ✅ Ready |

---

## 🔧 Tech Stack

| Component | Choice | Status |
|-----------|--------|--------|
| Runtime | Node.js | ✅ |
| Contact API | Serverless | ✅ Ready |
| Email | SendGrid/Mailgun/SES | ⏳ Choose |
| Auth | Firebase/Cognito | ⏳ Decide |
| Database | Firestore/DynamoDB | ⏳ Decide |
| Realtime | MQTT/WebSocket | ⏳ Decide |
| Payments | Razorpay | ⏳ Phase 3 |

---

## 📝 Notes

- Contact API is serverless-ready (Vercel/Netlify/Lambda)
- Start with Firebase for faster MVP
- See `README.md` for deployment instructions
- Related: `docs/business/SUBSCRIPTION_TODO.md`

---

*Last Updated: December 3, 2025*
