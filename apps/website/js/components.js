/**
 * Cultivio Website - Component Loader
 * Loads reusable header and footer components
 */

class ComponentLoader {
    constructor() {
        this.componentsPath = '/components';
        this.cache = {};
    }

    /**
     * Fetch and cache a component
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
            this.cache[name] = html;
            return html;
        } catch (error) {
            console.error(`Error loading component ${name}:`, error);
            return '';
        }
    }

    /**
     * Load component into a placeholder element
     */
    async loadComponent(name, placeholderId) {
        const placeholder = document.getElementById(placeholderId);
        if (!placeholder) {
            console.warn(`Placeholder #${placeholderId} not found`);
            return;
        }

        const html = await this.fetchComponent(name);
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

        await Promise.all(
            components.map(c => this.loadComponent(c.name, c.placeholder))
        );

        // Initialize navigation after header loads
        this.initNavigation();
        
        // Mark active nav link
        this.markActiveNavLink();
    }

    /**
     * Initialize mobile navigation toggle
     */
    initNavigation() {
        const navToggle = document.getElementById('navToggle');
        const navMenu = document.getElementById('navMenu');
        const navbar = document.getElementById('navbar');

        if (navToggle && navMenu) {
            navToggle.addEventListener('click', () => {
                navMenu.classList.toggle('active');
                navToggle.classList.toggle('active');
            });

            // Close menu when clicking a link
            navMenu.querySelectorAll('.nav-link').forEach(link => {
                link.addEventListener('click', () => {
                    navMenu.classList.remove('active');
                    navToggle.classList.remove('active');
                });
            });
        }

        // Navbar scroll effect
        if (navbar) {
            window.addEventListener('scroll', () => {
                if (window.scrollY > 50) {
                    navbar.classList.add('scrolled');
                } else {
                    navbar.classList.remove('scrolled');
                }
            });
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
                (currentPath !== '/' && href.startsWith(currentPath)) ||
                (currentPath === '/' && href === '/#hero')) {
                link.classList.add('active');
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

