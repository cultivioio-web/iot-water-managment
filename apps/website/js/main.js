/* ========================================
   Cultivio Website - Main JavaScript
   ======================================== */

// ===== Global Error Handling =====
window.addEventListener('error', (e) => {
    console.error('Global error:', e.error);
    // In production, you might want to send this to an error tracking service
});

window.addEventListener('unhandledrejection', (e) => {
    console.error('Unhandled promise rejection:', e.reason);
    // In production, you might want to send this to an error tracking service
});

// ===== DOM Ready =====
document.addEventListener('DOMContentLoaded', () => {
    try {
        initNavbar();
        initMobileMenu();
        initSmoothScroll();
        initScrollReveal();
        initContactForm();
        initWaterLevelAnimation();
        initFAQAccordion();
    } catch (error) {
        console.error('Initialization error:', error);
        // Show user-friendly error message if needed
    }
});

// ===== Reinitialize after components load =====
document.addEventListener('componentLoaded', (e) => {
    // Reinitialize nav functionality when header loads
    if (e.detail.name === 'header') {
        initNavbar();
        initMobileMenu();
        initSmoothScroll();
    }
});

// ===== Navbar Scroll Effect =====
function initNavbar() {
    const navbar = document.getElementById('navbar');
    const sections = document.querySelectorAll('section[id]');
    const navLinks = document.querySelectorAll('.nav-link');
    
    // Constants
    const SCROLL_THRESHOLD = 50;
    const SECTION_OFFSET = 100;
    
    // Single scroll handler for both navbar and active link highlighting
    let lastScroll = 0;
    window.addEventListener('scroll', () => {
        const currentScroll = window.pageYOffset;
        
        // Add/remove scrolled class
        if (currentScroll > SCROLL_THRESHOLD) {
            navbar.classList.add('scrolled');
        } else {
            navbar.classList.remove('scrolled');
        }
        
        // Update active nav link based on scroll position
        let current = '';
        sections.forEach(section => {
            const sectionTop = section.offsetTop - SECTION_OFFSET;
            const sectionHeight = section.offsetHeight;
            
            if (currentScroll >= sectionTop && currentScroll < sectionTop + sectionHeight) {
                current = section.getAttribute('id');
            }
        });
        
        navLinks.forEach(link => {
            link.classList.remove('active');
            if (link.getAttribute('href') === `#${current}`) {
                link.classList.add('active');
            }
        });
        
        lastScroll = currentScroll;
    });
}

// ===== Mobile Menu Toggle =====
function initMobileMenu() {
    const navToggle = document.getElementById('navToggle');
    const navMenu = document.getElementById('navMenu');
    const navLinks = document.querySelectorAll('.nav-link');
    
    // Toggle menu
    navToggle.addEventListener('click', () => {
        const isActive = navMenu.classList.contains('active');
        
        navToggle.classList.toggle('active');
        navMenu.classList.toggle('active');
        navToggle.setAttribute('aria-expanded', !isActive);
        
        // Prevent body scroll when menu is open
        if (!isActive) {
            document.body.style.overflow = 'hidden';
            document.body.style.position = 'fixed';
            document.body.style.width = '100%';
        } else {
            document.body.style.overflow = '';
            document.body.style.position = '';
            document.body.style.width = '';
        }
    });
    
    // Close menu function
    const closeMenu = () => {
        navToggle.classList.remove('active');
        navMenu.classList.remove('active');
        navToggle.setAttribute('aria-expanded', 'false');
        document.body.style.overflow = '';
        document.body.style.position = '';
        document.body.style.width = '';
    };
    
    // Close menu when clicking a link
    navLinks.forEach(link => {
        link.addEventListener('click', closeMenu);
    });
    
    // Close menu when clicking outside
    document.addEventListener('click', (e) => {
        if (!navMenu.contains(e.target) && !navToggle.contains(e.target)) {
            closeMenu();
        }
    });
    
    // Close menu on Escape key
    document.addEventListener('keydown', (e) => {
        if (e.key === 'Escape' && navMenu.classList.contains('active')) {
            closeMenu();
            navToggle.focus();
        }
    });
}

// ===== Smooth Scroll =====
function initSmoothScroll() {
    document.querySelectorAll('a[href^="#"]').forEach(anchor => {
        anchor.addEventListener('click', function(e) {
            e.preventDefault();
            const target = document.querySelector(this.getAttribute('href'));
            
            if (target) {
                const headerOffset = 80;
                const elementPosition = target.getBoundingClientRect().top;
                const offsetPosition = elementPosition + window.pageYOffset - headerOffset;
                
                window.scrollTo({
                    top: offsetPosition,
                    behavior: 'smooth'
                });
            }
        });
    });
}

// ===== Scroll Reveal Animation =====
function initScrollReveal() {
    const reveals = document.querySelectorAll('.reveal, .reveal-left, .reveal-right, .reveal-scale, .stagger-children');
    
    const revealOnScroll = () => {
        reveals.forEach(element => {
            const elementTop = element.getBoundingClientRect().top;
            const elementVisible = 150;
            
            if (elementTop < window.innerHeight - elementVisible) {
                element.classList.add('active');
            }
        });
    };
    
    // Initial check
    revealOnScroll();
    
    // Check on scroll
    window.addEventListener('scroll', revealOnScroll);
}

// ===== Contact Form =====
function initContactForm() {
    const form = document.getElementById('contactForm');
    
    if (!form) return;
    
    // Real-time validation
    const inputs = form.querySelectorAll('input, textarea, select');
    inputs.forEach(input => {
        input.addEventListener('blur', () => validateField(input));
        input.addEventListener('input', () => {
            if (input.classList.contains('error')) {
                validateField(input);
            }
        });
    });
    
    form.addEventListener('submit', async (e) => {
        e.preventDefault();
        
        // Validate all fields
        let isValid = true;
        inputs.forEach(input => {
            if (!validateField(input)) {
                isValid = false;
            }
        });
        
        if (!isValid) {
            showNotification('Please fill in all required fields correctly.', 'error');
            return;
        }
        
        const submitBtn = form.querySelector('button[type="submit"]');
        const originalText = submitBtn.innerHTML;
        
        // Show loading state
        submitBtn.innerHTML = '<span class="animate-spin">⏳</span> Sending...';
        submitBtn.disabled = true;
        
        // Collect form data
        const formData = new FormData(form);
        const data = Object.fromEntries(formData.entries());
        
        // Simulate form submission (replace with actual API call)
        try {
            // TODO: Replace with actual API endpoint
            // await fetch('/api/contact', {
            //     method: 'POST',
            //     headers: { 'Content-Type': 'application/json' },
            //     body: JSON.stringify(data)
            // });
            
            await new Promise(resolve => setTimeout(resolve, 1500));
            
            // Show success message
            showNotification('Thank you! We\'ll get back to you soon.', 'success');
            form.reset();
            inputs.forEach(input => input.classList.remove('error', 'success'));
            
        } catch (error) {
            console.error('Form submission error:', error);
            showNotification('Oops! Something went wrong. Please try again.', 'error');
        } finally {
            submitBtn.innerHTML = originalText;
            submitBtn.disabled = false;
        }
    });
}

// Validate individual field
function validateField(field) {
    const value = field.value.trim();
    let isValid = true;
    
    // Required field check
    if (field.hasAttribute('required') && !value) {
        isValid = false;
    }
    
    // Email validation
    if (field.type === 'email' && value) {
        const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
        isValid = emailRegex.test(value);
    }
    
    // Phone validation (Indian format)
    if (field.type === 'tel' && value) {
        const phoneRegex = /^[6-9]\d{9}$/;
        isValid = phoneRegex.test(value.replace(/\D/g, '').slice(-10));
    }
    
    // Select validation
    if (field.tagName === 'SELECT' && (!value || value === '')) {
        isValid = false;
    }
    
    // Update field styling
    if (isValid) {
        field.classList.remove('error');
        field.classList.add('success');
    } else {
        field.classList.remove('success');
        field.classList.add('error');
    }
    
    return isValid;
}

// ===== Notification Toast =====
function showNotification(message, type = 'success') {
    // Remove existing notifications
    const existing = document.querySelector('.notification');
    if (existing) existing.remove();
    
    // Create notification element
    const notification = document.createElement('div');
    notification.className = `notification notification-${type}`;
    notification.innerHTML = `
        <span class="notification-icon">${type === 'success' ? '✓' : '!'}</span>
        <span class="notification-message">${message}</span>
        <button class="notification-close" onclick="this.parentElement.remove()">×</button>
    `;
    
    // Add styles
    notification.style.cssText = `
        position: fixed;
        bottom: 24px;
        right: 24px;
        display: flex;
        align-items: center;
        gap: 12px;
        padding: 16px 24px;
        background: ${type === 'success' ? '#10B981' : '#EF4444'};
        color: white;
        border-radius: 12px;
        box-shadow: 0 10px 40px rgba(0,0,0,0.2);
        z-index: 9999;
        animation: slide-up 0.3s ease;
    `;
    
    document.body.appendChild(notification);
    
    // Auto remove after 5 seconds
    setTimeout(() => {
        notification.style.animation = 'fade-out 0.3s ease forwards';
        setTimeout(() => notification.remove(), 300);
    }, 5000);
}

// ===== Water Level Animation =====
function initWaterLevelAnimation() {
    const waterLevel = document.querySelector('.water-level');
    
    if (!waterLevel) return;
    
    // Animate water level randomly
    setInterval(() => {
        const level = 60 + Math.random() * 30; // 60-90%
        waterLevel.style.setProperty('--level', `${level}%`);
    }, 3000);
}

// ===== Counter Animation =====
function animateCounter(element, target, duration = 2000) {
    let start = 0;
    const increment = target / (duration / 16);
    
    const updateCounter = () => {
        start += increment;
        if (start < target) {
            element.textContent = Math.floor(start);
            requestAnimationFrame(updateCounter);
        } else {
            element.textContent = target;
        }
    };
    
    updateCounter();
}

// ===== Intersection Observer for lazy loading =====
const lazyLoad = (entries, observer) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            const img = entry.target;
            img.src = img.dataset.src;
            img.classList.add('loaded');
            observer.unobserve(img);
        }
    });
};

const imageObserver = new IntersectionObserver(lazyLoad, {
    rootMargin: '50px 0px',
    threshold: 0.01
});

document.querySelectorAll('img[data-src]').forEach(img => {
    imageObserver.observe(img);
});

// ===== Pricing Card Hover Effect =====
document.querySelectorAll('.pricing-card').forEach(card => {
    card.addEventListener('mouseenter', function() {
        document.querySelectorAll('.pricing-card').forEach(c => {
            if (c !== this && !c.classList.contains('featured')) {
                c.style.opacity = '0.7';
            }
        });
    });
    
    card.addEventListener('mouseleave', function() {
        document.querySelectorAll('.pricing-card').forEach(c => {
            c.style.opacity = '1';
        });
    });
});

// ===== Parallax Effect =====
window.addEventListener('scroll', () => {
    const scrolled = window.pageYOffset;
    
    // Hero parallax
    const heroGradient = document.querySelector('.hero-gradient');
    if (heroGradient) {
        heroGradient.style.transform = `translateY(${scrolled * 0.3}px)`;
    }
    
    // Floating drops parallax
    document.querySelectorAll('.drop').forEach((drop, index) => {
        const speed = 0.1 + (index * 0.05);
        drop.style.transform = `translateY(${scrolled * speed}px)`;
    });
});

// ===== FAQ Accordion =====
function initFAQAccordion() {
    const accordionHeaders = document.querySelectorAll('.accordion-header');
    
    accordionHeaders.forEach(header => {
        header.addEventListener('click', () => {
            const content = header.nextElementSibling;
            const isActive = header.classList.contains('active');
            
            // Close all other accordions
            accordionHeaders.forEach(otherHeader => {
                if (otherHeader !== header) {
                    otherHeader.classList.remove('active');
                    otherHeader.nextElementSibling.classList.remove('active');
                }
            });
            
            // Toggle current accordion
            header.classList.toggle('active');
            content.classList.toggle('active');
        });
    });
}

// ===== Add CSS for notification animation =====
const style = document.createElement('style');
style.textContent = `
    @keyframes fade-out {
        from { opacity: 1; transform: translateY(0); }
        to { opacity: 0; transform: translateY(20px); }
    }
    
    .notification-close {
        background: none;
        border: none;
        color: white;
        font-size: 20px;
        cursor: pointer;
        opacity: 0.7;
        transition: opacity 0.2s;
    }
    
    .notification-close:hover {
        opacity: 1;
    }
`;
document.head.appendChild(style);

