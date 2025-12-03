# Comprehensive TODO - Cultivio AquaSense Project
## Master Task List (Firmware + Website + Business/Subscription)

**Last Updated:** December 3, 2025  
**Overall Progress:** 75% (Firmware 100%, Website 89%, Business 0%)  
**Project Phases:**  
- **Phase 1: Core Fixes & Testing** (Complete for firmware; ongoing for website)  
- **Phase 2: Optimization & Features**  
- **Phase 3: Business/Subscription Implementation**  
- **Phase 4: Deployment & Monitoring**

### Progress Tracking Table
| Category | Tasks | Completed | Status | Assignee | Deadline | Notes |
|----------|-------|-----------|--------|----------|----------|-------|
| **Firmware** | 63 | 63 | ✅ 100% | AI Assistant | Completed | All bugs fixed; ready for production after testing |
| **Website** | 25 | 22 | 🟡 88% | Dev Team | Week 2 | Critical bugs fixed; performance/SEO pending |
| **Business/Subscription** | 15 | 0 | 🔴 0% | Product Team | Month 2 | App monetization model to define |
| **Total** | **103** | **85** | **83%** | - | - | On track; focus on testing & business |

---

## 🔴 Phase 1: Immediate Tasks (This Week - Testing & Remaining Fixes)
Focus: Validate firmware/website, fix outstanding issues. Estimated: 20-30 hours.

### Firmware Testing & Validation (From firmware/TODO.md)
- **Code Review** (High Priority)  
  - [ ] Create git branch: `feature/firmware-complete`  
  - [ ] Commit all fixes with messages (e.g., "fix: atomic Zigbee attributes (BUG #9)")  
  - [ ] PR on GitHub: Include FEATURE_LIST.md, FIXES_IMPLEMENTED.md  
  - [ ] Reviewers: @lead-dev, @firmware-team  
  - [ ] Address feedback; merge after approval  
  - Assignee: Lead Developer | Deadline: Day 2 | Success: All approvals, CI passes  

- **Unit Testing** (High Priority)  
  - [ ] Water level calculation (tank_height=0, invalid samples)  
  - [ ] Pump logic (thresholds, timeouts, manual override)  
  - [ ] BLE parsing (invalid inputs, password change)  
  - [ ] Zigbee attribute updates (race condition simulation)  
  - [ ] NVS recovery (corruption simulation)  
  - Assignee: QA Team | Deadline: Day 4 | Success: 100% pass rate; report generated  

- **Integration Testing** (High Priority)  
  - [ ] Sensor → Controller Zigbee flow (reports, status updates)  
  - [ ] BLE provisioning end-to-end (role selection, config save/load)  
  - [ ] Manual pump override (10-60 min timers, expiry)  
  - [ ] Power cycle during operations (NVS recovery)  
  - Assignee: QA + Dev | Deadline: Day 5 | Success: No crashes in 2-hour test  

- **Stress Testing** (High Priority)  
  - [ ] 7-day continuous run (3 systems: sensor+controller+router)  
  - [ ] Rapid disconnects (sensor every 5 min)  
  - [ ] BLE config changes hourly  
  - [ ] Simulate 49+ day uptime (manual override expiry)  
  - [ ] Metrics: 0 crashes, <1% packet loss, <30% CPU  
  - Assignee: QA | Deadline: Week 2 (7 days) | Success: Full uptime, detailed report  

- **Security Audit** (High Priority)  
  - [ ] BLE pairing (iOS/Android, unauthorized attempts)  
  - [ ] Password fuzzing (weak/overflow)  
  - [ ] Input validation (tank params, thresholds)  
  - [ ] Buffer overflow tests (names, commands)  
  - Assignee: Security Team | Deadline: Week 2 | Success: No vulnerabilities; report  

### Website Fixes (From QUICK_FIX_GUIDE.md & ANALYSIS_REPORT.md)
- **Error Handling in Components** (High Priority - XSS Risk)  
  - [ ] Sanitize HTML in `js/components.js` (lines 44-45): Use DOMPurify or regex to strip <script> tags  
  - [ ] Add try-catch around fetch/insert  
  - Assignee: Dev | Deadline: Day 1 | Success: No XSS via Lighthouse audit  

- **Contact Form Backend** (High Priority)  
  - [ ] Implement API in `api/` (e.g., Node.js/EmailJS for submissions)  
  - [ ] Remove simulation in `pages/support.html`; add server-side validation/CSRF  
  - [ ] Rate limiting (e.g., 5/min per IP)  
  - Assignee: Dev | Deadline: Day 2 | Success: Real emails sent; test form  

- **Mobile Menu Scroll Lock** (Medium Priority)  
  - [ ] In `js/main.js`: Set `body.style.overflow = 'hidden'` on menu open  
  - [ ] Add focus trap for accessibility  
  - Assignee: Dev | Deadline: Day 3 | Success: No background scroll on mobile  

- **Image Optimization** (Medium Priority)  
  - [ ] Add `width/height` to all `<img>` to fix CLS (0.15 → <0.1)  
  - [ ] Convert to WebP; consistent `loading="lazy"`  
  - Assignee: Dev | Deadline: Day 3 | Success: Lighthouse CLS <0.1  

- **SEO Meta Tags** (Medium Priority)  
  - [ ] Add Open Graph/Twitter Cards in `<head>` (title, desc, image)  
  - [ ] Schema.org for products in `index.html`  
  - [ ] Create `sitemap.xml` & optimize robots.txt  
  - Assignee: Dev | Deadline: Day 4 | Success: Validated via Google Structured Data Tool  

---

## 🟡 Phase 2: Short-Term Tasks (This Month - Optimizations & Features)
Focus: Polish, add value. Estimated: 40-60 hours.

### Firmware Optimizations
- **Power Management** (Medium)  
  - [ ] Light sleep in sensor idle (wake on timer/button)  
  - [ ] PM config for H2 (max 96MHz, min 32MHz)  
  - Assignee: Dev | Deadline: Week 3 | Success: Battery life >7 days  

- **OTA Support** (Medium)  
  - [ ] BLE/Zigbee OTA partition; rollback on failure  
  - [ ] App-triggered updates  
  - Assignee: Dev | Deadline: Week 4 | Success: Successful OTA test  

- **Data Logging** (Medium)  
  - [ ] NVS for 24h history (levels, pump runs)  
  - [ ] BLE export to app  
  - Assignee: Dev | Deadline: Week 4 | Success: Retrieve 100 entries  

### Website Optimizations
- **PWA Implementation** (High)  
  - [ ] Service Worker for offline/caching  
  - [ ] Install prompt; background sync for forms  
  - [ ] Offline indicator  
  - Assignee: Dev | Deadline: Week 3 | Success: Installable as app (Chrome audit)  

- **Accessibility Full Compliance** (Medium)  
  - [ ] ARIA labels everywhere; keyboard nav test  
  - [ ] Contrast fixes (WCAG AA)  
  - [ ] Screen reader testing (NVDA/VoiceOver)  
  - Assignee: Dev | Deadline: Week 3 | Success: WAVE score 100%  

- **Performance Tuning** (Medium)  
  - [ ] Critical CSS extraction; defer JS  
  - [ ] Font preloading; minify assets  
  - [ ] Lighthouse >90 all categories  
  - Assignee: Dev | Deadline: Week 4 | Success: LCP <2.5s, TTI <3.8s  

- **Analytics Integration** (Medium)  
  - [ ] Google Analytics: Page views, events (form submits, app connects)  
  - [ ] Heatmaps (Hotjar) for UX insights  
  - Assignee: Product | Deadline: Week 4 | Success: Tracking live  

### Business/Subscription Setup (New Section)
- **Subscription Model Definition** (High)  
  - [ ] Basic (Free): Local monitoring, basic alerts  
  - [ ] Premium ($4.99/mo): Cloud history, multi-device, advanced alerts (low water SMS)  
  - [ ] Enterprise ($19.99/mo): API access, custom integrations  
  - [ ] Define tiers in docs/business/subscription_todo.md  
  - Assignee: Product Manager | Deadline: Week 1 | Success: Document approved  

- **App Integration for Subscriptions** (Medium)  
  - [ ] Stripe/PayPal SDK in mobile app for payments  
  - [ ] User auth (Firebase); subscription status check  
  - [ ] Gate premium features (e.g., history export) behind paywall  
  - Assignee: Mobile Dev | Deadline: Week 3 | Success: Test purchase flow  

- **Backend for Subscriptions** (Medium)  
  - [ ] Cloud DB (Firebase/Firestore) for user data/sub status  
  - [ ] Webhook for payment events; sync to app  
  - [ ] Compliance: GDPR for data, PCI for payments  
  - Assignee: Backend Dev | Deadline: Week 4 | Success: Sandbox payments work  

---

## 🟢 Phase 3: Medium-Term Tasks (Next Quarter - Advanced Features)
Focus: Expand ecosystem. Estimated: 80-120 hours.

### Firmware
- **Health Monitoring** (Low)  
  - [ ] Sensor reliability %; RSSI trends/alerts  
  - [ ] Auto-rejoin on signal loss  
  - Assignee: Dev | Deadline: Month 2  

- **Calibration Mode** (Low)  
  - [ ] App-triggered: Measure empty/full tank for offset  
  - Assignee: Dev | Deadline: Month 2  

### Website/App
- **Dark Mode** (Low)  
  - [ ] Toggle using CSS variables  
  - Assignee: Dev | Deadline: Month 1  

- **Video Demos/FAQs** (Low)  
  - [ ] Embed install videos; expand support page  
  - Assignee: Content | Deadline: Month 1  

- **Blog Section** (Medium)  
  - [ ] SEO-optimized posts (water saving tips)  
  - Assignee: Marketing | Deadline: Month 2  

### Business/Subscription
- **A/B Testing** (Medium)  
  - [ ] Test pricing tiers; conversion funnels  
  - Assignee: Product | Deadline: Month 2 | Success: 20% uplift  

- **Customer Onboarding** (Medium)  
  - [ ] Email sequences for new subscribers  
  - [ ] In-app tutorials for premium features  
  - Assignee: Marketing | Deadline: Month 2  

- **Partner Integrations** (Low)  
  - [ ] API for dealers (bulk provisioning)  
  - Assignee: Business Dev | Deadline: Month 3  

---

## 🔵 Phase 4: Long-Term Tasks (Ongoing - Scaling)
Focus: Growth & maintenance. Estimated: 100+ hours.

- **Multi-Language Support** (Low): App/website localization (Hindi/English).  
- **Cloud Dashboard** (High): Web portal for enterprise (subscription-gated).  
- **AI Insights** (Medium): Predictive low-water alerts (premium).  
- **Hardware V2** (High): Add flow sensors, solar charging.  
- **Monitoring** (Ongoing): Sentry for errors; user feedback loop.  
- **Compliance** (Medium): CE/FCC cert; privacy policy updates.

## Blockers & Risks
- **Firmware:** Hardware availability for stress tests. Mitigation: Use emulators.  
- **Website:** Backend hosting for forms. Mitigation: Netlify free tier.  
- **Business:** Legal review for subscriptions. Mitigation: Consult lawyer early.  

## Success Criteria
- **Firmware:** 0 crashes in 7-day test; full feature coverage.  
- **Website:** Lighthouse 90+; 100% accessibility.  
- **Business:** 10% conversion to premium in beta; $X MRR target.  

**Contacts:**  
- Dev Lead: TBD (firmware/website)  
- Product: TBD (subscriptions)  
- QA: TBD (testing)  

For updates, edit this file or use issue tracker.
