/**
 * Cultivio Website - Component Loader
 * Loads reusable header and footer components
 * 
 * Security: Sanitizes HTML to prevent XSS attacks
 */

class ComponentLoader {
    constructor() {
        this.componentsPath = '/components';
        this.cache = {};
        // Allowed tags for sanitization
        this.allowedTags = [
            'a', 'abbr', 'b', 'blockquote', 'br', 'button', 'cite', 'code',
            'div', 'em', 'footer', 'form', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6',
            'header', 'hr', 'i', 'img', 'input', 'label', 'li', 'main', 'nav',
            'ol', 'p', 'path', 'picture', 'pre', 'section', 'small', 'source',
            'span', 'strong', 'sub', 'sup', 'svg', 'table', 'tbody', 'td',
            'textarea', 'tfoot', 'th', 'thead', 'time', 'tr', 'u', 'ul',
            'video', 'article', 'aside', 'figure', 'figcaption', 'address',
            'circle', 'rect', 'line', 'polyline', 'polygon', 'g', 'defs',
            'linearGradient', 'stop', 'use', 'symbol', 'title', 'desc'
        ];
        // Dangerous attributes to remove
        this.dangerousAttrs = [
            'onclick', 'onerror', 'onload', 'onmouseover', 'onmouseout',
            'onkeydown', 'onkeyup', 'onkeypress', 'onfocus', 'onblur',
            'onchange', 'onsubmit', 'onreset', 'onselect', 'oninput',
            'ondblclick', 'oncontextmenu', 'ondrag', 'ondragend', 'ondragenter',
            'ondragleave', 'ondragover', 'ondragstart', 'ondrop', 'onscroll'
        ];
    }

    /**
     * Sanitize HTML to prevent XSS attacks
     * @param {string} html - Raw HTML string
     * @returns {string} - Sanitized HTML string
     */
    sanitizeHTML(html) {
        // Create a temporary container
        const temp = document.createElement('div');
        temp.innerHTML = html;

        // Remove all script tags
        const scripts = temp.querySelectorAll('script');
        scripts.forEach(script => script.remove());

        // Remove dangerous event handlers from all elements
        const allElements = temp.querySelectorAll('*');
        allElements.forEach(el => {
            // Remove dangerous attributes
            this.dangerousAttrs.forEach(attr => {
                el.removeAttribute(attr);
            });

            // Remove javascript: URLs
            ['href', 'src', 'action', 'formaction', 'xlink:href'].forEach(attr => {
                const value = el.getAttribute(attr);
                if (value && value.toLowerCase().trim().startsWith('javascript:')) {
                    el.removeAttribute(attr);
                }
            });

            // Remove data: URLs from src (except for images)
            if (el.tagName !== 'IMG') {
                const src = el.getAttribute('src');
                if (src && src.toLowerCase().trim().startsWith('data:')) {
                    el.removeAttribute('src');
                }
            }
        });

        // Remove style tags (optional - they could contain expressions)
        const styles = temp.querySelectorAll('style');
        styles.forEach(style => {
            // Check for CSS expressions (IE) or behavior URLs
            const content = style.textContent.toLowerCase();
            if (content.includes('expression(') || 
                content.includes('behavior:') || 
                content.includes('javascript:')) {
                style.remove();
            }
        });

        return temp.innerHTML;
    }

    /**
     * Fetch and cache a component
     * @param {string} name - Component name
     * @returns {Promise<string>} - Sanitized HTML content
     */
    async fetchComponent(name) {
        if (this.cache[name]) {
            return this.cache[name];
        }

        try {
            const response = await fetch(`${this.componentsPath}/${name}.html`);
            if (!response.ok) {
                throw new Error(`Failed to load component: ${name}`);
            }
            const html = await response.text();
            
            // SECURITY: Sanitize HTML before caching
            const sanitizedHtml = this.sanitizeHTML(html);
            this.cache[name] = sanitizedHtml;
            
            return sanitizedHtml;
        } catch (error) {
            console.error(`Error loading component ${name}:`, error);
            return '';
        }
    }

    /**
     * Load component into a placeholder element
     * @param {string} name - Component name
     * @param {string} placeholderId - Target element ID
     */
    async loadComponent(name, placeholderId) {
        const placeholder = document.getElementById(placeholderId);
        if (!placeholder) {
            console.warn(`Placeholder #${placeholderId} not found`);
            return;
        }

        const html = await this.fetchComponent(name);
        
        // SECURITY: HTML is already sanitized in fetchComponent
        placeholder.innerHTML = html;
        placeholder.classList.add('component-loaded');

        // Dispatch event for post-load initialization
        document.dispatchEvent(new CustomEvent('componentLoaded', { 
            detail: { name, placeholderId } 
        }));
    }

    /**
     * Load all standard components (header, footer)
     */
    async loadAllComponents() {
        const components = [
            { name: 'header', placeholder: 'header-placeholder' },
            { name: 'footer', placeholder: 'footer-placeholder' }
        ];

        try {
            await Promise.all(
                components.map(c => this.loadComponent(c.name, c.placeholder))
            );

            // Initialize navigation after header loads
            this.initNavigation();
            
            // Mark active nav link
            this.markActiveNavLink();
        } catch (error) {
            console.error('Error loading components:', error);
        }
    }

    /**
     * Initialize mobile navigation toggle
     */
    initNavigation() {
        const navToggle = document.getElementById('navToggle');
        const navMenu = document.getElementById('navMenu');
        const navbar = document.getElementById('navbar');

        if (navToggle && navMenu) {
            // Toggle menu
            navToggle.addEventListener('click', () => {
                const isActive = navMenu.classList.toggle('active');
                navToggle.classList.toggle('active');
                
                // Accessibility: Update aria-expanded
                navToggle.setAttribute('aria-expanded', isActive);
                
                // Prevent body scroll when menu is open
                document.body.style.overflow = isActive ? 'hidden' : '';
            });

            // Close menu when clicking a link
            navMenu.querySelectorAll('.nav-link').forEach(link => {
                link.addEventListener('click', () => {
                    navMenu.classList.remove('active');
                    navToggle.classList.remove('active');
                    navToggle.setAttribute('aria-expanded', 'false');
                    document.body.style.overflow = '';
                });
            });

            // Close menu on Escape key
            document.addEventListener('keydown', (e) => {
                if (e.key === 'Escape' && navMenu.classList.contains('active')) {
                    navMenu.classList.remove('active');
                    navToggle.classList.remove('active');
                    navToggle.setAttribute('aria-expanded', 'false');
                    document.body.style.overflow = '';
                    navToggle.focus();
                }
            });
        }

        // Navbar scroll effect
        if (navbar) {
            let lastScroll = 0;
            window.addEventListener('scroll', () => {
                const currentScroll = window.scrollY;
                
                if (currentScroll > 50) {
                    navbar.classList.add('scrolled');
                } else {
                    navbar.classList.remove('scrolled');
                }
                
                lastScroll = currentScroll;
            }, { passive: true });
        }
    }

    /**
     * Mark the current page's nav link as active
     */
    markActiveNavLink() {
        const currentPath = window.location.pathname;
        const navLinks = document.querySelectorAll('.nav-link');

        navLinks.forEach(link => {
            const href = link.getAttribute('href');
            
            // Check for exact match or starts with (for subpages)
            if (href === currentPath || 
                (currentPath !== '/' && href && currentPath.includes(href.replace(/^\//, ''))) ||
                (currentPath === '/' && href === '/#hero') ||
                (currentPath.endsWith('index.html') && href === '/#hero')) {
                link.classList.add('active');
                link.setAttribute('aria-current', 'page');
            }
        });
    }
}

// Initialize component loader when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    const loader = new ComponentLoader();
    loader.loadAllComponents();
});

// Export for module usage
if (typeof module !== 'undefined' && module.exports) {
    module.exports = ComponentLoader;
}
