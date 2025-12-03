/**
 * Cultivio Website - Analytics Integration
 * Google Analytics 4 + Event Tracking
 * 
 * SETUP: Replace 'G-XXXXXXXXXX' with your actual GA4 Measurement ID
 */

// Google Analytics 4 Configuration
const GA_MEASUREMENT_ID = 'G-XXXXXXXXXX'; // TODO: Replace with actual ID

// Initialize Google Analytics
(function() {
    // Don't load in development
    if (window.location.hostname === 'localhost' || 
        window.location.hostname === '127.0.0.1') {
        console.log('[Analytics] Development mode - GA disabled');
        window.gtag = function() { 
            console.log('[Analytics Mock]', arguments); 
        };
        return;
    }

    // Load gtag.js
    const script = document.createElement('script');
    script.async = true;
    script.src = `https://www.googletagmanager.com/gtag/js?id=${GA_MEASUREMENT_ID}`;
    document.head.appendChild(script);

    // Initialize dataLayer and gtag
    window.dataLayer = window.dataLayer || [];
    window.gtag = function() { dataLayer.push(arguments); };
    
    gtag('js', new Date());
    gtag('config', GA_MEASUREMENT_ID, {
        'anonymize_ip': true,
        'cookie_flags': 'SameSite=None;Secure'
    });

    console.log('[Analytics] GA4 initialized');
})();

/**
 * Track page views (automatic for SPA navigation)
 */
function trackPageView(pagePath, pageTitle) {
    if (typeof gtag === 'function') {
        gtag('event', 'page_view', {
            page_path: pagePath || window.location.pathname,
            page_title: pageTitle || document.title
        });
    }
}

/**
 * Track custom events
 */
function trackEvent(eventName, parameters = {}) {
    if (typeof gtag === 'function') {
        gtag('event', eventName, parameters);
        console.log(`[Analytics] Event: ${eventName}`, parameters);
    }
}

/**
 * Track button clicks
 */
function trackButtonClick(buttonName, buttonLocation) {
    trackEvent('button_click', {
        button_name: buttonName,
        button_location: buttonLocation
    });
}

/**
 * Track form submissions
 */
function trackFormSubmission(formName, success = true) {
    trackEvent('form_submit', {
        form_name: formName,
        success: success
    });
}

/**
 * Track product interactions
 */
function trackProductView(productName, productId) {
    trackEvent('view_item', {
        item_name: productName,
        item_id: productId
    });
}

/**
 * Track outbound links
 */
function trackOutboundLink(url, linkText) {
    trackEvent('click', {
        event_category: 'outbound',
        event_label: url,
        link_text: linkText
    });
}

/**
 * Track scroll depth
 */
let scrollDepthTracked = {};
function trackScrollDepth() {
    const scrollPercent = Math.round(
        (window.scrollY / (document.body.scrollHeight - window.innerHeight)) * 100
    );
    
    const milestones = [25, 50, 75, 100];
    milestones.forEach(milestone => {
        if (scrollPercent >= milestone && !scrollDepthTracked[milestone]) {
            scrollDepthTracked[milestone] = true;
            trackEvent('scroll_depth', {
                percent_scrolled: milestone,
                page_path: window.location.pathname
            });
        }
    });
}

// Initialize scroll tracking
let scrollTimeout;
window.addEventListener('scroll', () => {
    clearTimeout(scrollTimeout);
    scrollTimeout = setTimeout(trackScrollDepth, 100);
}, { passive: true });

/**
 * Auto-track CTA buttons
 */
document.addEventListener('DOMContentLoaded', () => {
    // Track all CTA button clicks
    document.querySelectorAll('.btn-primary, .cta-button, [data-track]').forEach(btn => {
        btn.addEventListener('click', () => {
            const buttonText = btn.textContent.trim();
            const section = btn.closest('section')?.id || 'unknown';
            trackButtonClick(buttonText, section);
        });
    });

    // Track outbound links
    document.querySelectorAll('a[href^="http"]').forEach(link => {
        if (!link.href.includes(window.location.hostname)) {
            link.addEventListener('click', () => {
                trackOutboundLink(link.href, link.textContent.trim());
            });
        }
    });

    // Track contact form
    const contactForm = document.querySelector('#contactForm, .contact-form');
    if (contactForm) {
        contactForm.addEventListener('submit', () => {
            trackFormSubmission('contact_form');
        });
    }
});

// Export for module usage
if (typeof module !== 'undefined' && module.exports) {
    module.exports = {
        trackPageView,
        trackEvent,
        trackButtonClick,
        trackFormSubmission,
        trackProductView,
        trackOutboundLink
    };
}

