/**
 * ArduRoomba Website JavaScript
 * Handles navigation, animations, and interactive elements
 */

(function() {
    'use strict';

    // ========================================================================
    // UTILITY FUNCTIONS
    // ========================================================================

    /**
     * Debounce function to limit function calls
     */
    function debounce(func, wait) {
        let timeout;
        return function executedFunction(...args) {
            const later = () => {
                clearTimeout(timeout);
                func(...args);
            };
            clearTimeout(timeout);
            timeout = setTimeout(later, wait);
        };
    }

    /**
     * Throttle function to limit function calls
     */
    function throttle(func, limit) {
        let inThrottle;
        return function() {
            const args = arguments;
            const context = this;
            if (!inThrottle) {
                func.apply(context, args);
                inThrottle = true;
                setTimeout(() => inThrottle = false, limit);
            }
        };
    }

    /**
     * Check if element is in viewport
     */
    function isInViewport(element) {
        const rect = element.getBoundingClientRect();
        return (
            rect.top >= 0 &&
            rect.left >= 0 &&
            rect.bottom <= (window.innerHeight || document.documentElement.clientHeight) &&
            rect.right <= (window.innerWidth || document.documentElement.clientWidth)
        );
    }

    /**
     * Smooth scroll to element
     */
    function smoothScrollTo(element) {
        const targetPosition = element.offsetTop - 80; // Account for navbar height
        window.scrollTo({
            top: targetPosition,
            behavior: 'smooth'
        });
    }

    /**
     * Animate number counting
     */
    function animateNumber(element, target, duration = 2000) {
        const start = 0;
        const increment = target / (duration / 16); // 60fps
        let current = start;

        const timer = setInterval(() => {
            current += increment;
            if (current >= target) {
                current = target;
                clearInterval(timer);
            }
            element.textContent = Math.floor(current).toLocaleString();
        }, 16);
    }

    // ========================================================================
    // NAVIGATION
    // ========================================================================

    class Navigation {
        constructor() {
            this.navbar = document.querySelector('.navbar');
            this.navToggle = document.getElementById('navToggle');
            this.navMenu = document.getElementById('navMenu');
            this.navLinks = document.querySelectorAll('.nav-link');
            
            this.init();
        }

        init() {
            this.setupScrollEffect();
            this.setupMobileMenu();
            this.setupSmoothScrolling();
            this.setupActiveLink();
        }

        setupScrollEffect() {
            let lastScrollY = window.scrollY;
            
            const handleScroll = throttle(() => {
                const currentScrollY = window.scrollY;
                
                // Add/remove scrolled class
                if (currentScrollY > 50) {
                    this.navbar.classList.add('scrolled');
                } else {
                    this.navbar.classList.remove('scrolled');
                }
                
                // Hide/show navbar on scroll
                if (currentScrollY > lastScrollY && currentScrollY > 100) {
                    this.navbar.style.transform = 'translateY(-100%)';
                } else {
                    this.navbar.style.transform = 'translateY(0)';
                }
                
                lastScrollY = currentScrollY;
            }, 10);

            window.addEventListener('scroll', handleScroll);
        }

        setupMobileMenu() {
            if (!this.navToggle || !this.navMenu) return;

            this.navToggle.addEventListener('click', () => {
                this.navMenu.classList.toggle('active');
                this.navToggle.classList.toggle('active');
                document.body.classList.toggle('menu-open');
            });

            // Close menu when clicking outside
            document.addEventListener('click', (e) => {
                if (!this.navToggle.contains(e.target) && !this.navMenu.contains(e.target)) {
                    this.navMenu.classList.remove('active');
                    this.navToggle.classList.remove('active');
                    document.body.classList.remove('menu-open');
                }
            });

            // Close menu when clicking on a link
            this.navLinks.forEach(link => {
                link.addEventListener('click', () => {
                    this.navMenu.classList.remove('active');
                    this.navToggle.classList.remove('active');
                    document.body.classList.remove('menu-open');
                });
            });
        }

        setupSmoothScrolling() {
            this.navLinks.forEach(link => {
                if (link.getAttribute('href').startsWith('#')) {
                    link.addEventListener('click', (e) => {
                        e.preventDefault();
                        const targetId = link.getAttribute('href');
                        const targetElement = document.querySelector(targetId);
                        
                        if (targetElement) {
                            smoothScrollTo(targetElement);
                        }
                    });
                }
            });
        }

        setupActiveLink() {
            const sections = document.querySelectorAll('section[id]');
            
            const handleScroll = throttle(() => {
                let current = '';
                
                sections.forEach(section => {
                    const sectionTop = section.offsetTop - 100;
                    const sectionHeight = section.clientHeight;
                    
                    if (window.scrollY >= sectionTop && window.scrollY < sectionTop + sectionHeight) {
                        current = section.getAttribute('id');
                    }
                });

                this.navLinks.forEach(link => {
                    link.classList.remove('active');
                    if (link.getAttribute('href') === `#${current}`) {
                        link.classList.add('active');
                    }
                });
            }, 100);

            window.addEventListener('scroll', handleScroll);
        }
    }

    // ========================================================================
    // SCROLL ANIMATIONS
    // ========================================================================

    class ScrollAnimations {
        constructor() {
            this.animatedElements = document.querySelectorAll('.animate-on-scroll');
            this.statsAnimated = false;
            this.init();
        }

        init() {
            this.setupIntersectionObserver();
            this.setupStatsAnimation();
        }

        setupIntersectionObserver() {
            const options = {
                threshold: 0.1,
                rootMargin: '0px 0px -50px 0px'
            };

            const observer = new IntersectionObserver((entries) => {
                entries.forEach(entry => {
                    if (entry.isIntersecting) {
                        entry.target.classList.add('animated');
                        observer.unobserve(entry.target);
                    }
                });
            }, options);

            this.animatedElements.forEach(element => {
                observer.observe(element);
            });
        }

        setupStatsAnimation() {
            const statsSection = document.querySelector('.hero-stats');
            if (!statsSection) return;

            const observer = new IntersectionObserver((entries) => {
                entries.forEach(entry => {
                    if (entry.isIntersecting && !this.statsAnimated) {
                        this.animateStats();
                        this.statsAnimated = true;
                        observer.unobserve(entry.target);
                    }
                });
            }, { threshold: 0.5 });

            observer.observe(statsSection);
        }

        animateStats() {
            const statNumbers = document.querySelectorAll('.stat-number[data-target]');
            
            statNumbers.forEach(stat => {
                const target = parseInt(stat.getAttribute('data-target'));
                animateNumber(stat, target, 2000);
            });
        }
    }

    // ========================================================================
    // INTERACTIVE ELEMENTS
    // ========================================================================

    class InteractiveElements {
        constructor() {
            this.init();
        }

        init() {
            this.setupHeroDevice();
            this.setupFeatureCards();
            this.setupPlatformCards();
            this.setupCopyButtons();
        }

        setupHeroDevice() {
            const heroDevice = document.querySelector('.hero-device');
            if (!heroDevice) return;

            // Add mouse move effect for visual appeal
            heroDevice.addEventListener('mousemove', (e) => {
                const rect = heroDevice.getBoundingClientRect();
                const x = e.clientX - rect.left;
                const y = e.clientY - rect.top;
                const centerX = rect.width / 2;
                const centerY = rect.height / 2;
                
                const rotateX = (y - centerY) / 10;
                const rotateY = (centerX - x) / 10;
                
                heroDevice.style.transform = `perspective(1000px) rotateX(${rotateX}deg) rotateY(${rotateY}deg)`;
            });

            heroDevice.addEventListener('mouseleave', () => {
                heroDevice.style.transform = 'perspective(1000px) rotateY(-15deg) rotateX(10deg)';
            });

            // Add visual feedback to demo buttons (non-functional, just for show)
            const dpadButtons = document.querySelectorAll('.d-pad-btn');
            dpadButtons.forEach(button => {
                button.addEventListener('click', (e) => {
                    e.preventDefault();
                    button.style.transform = 'scale(0.95)';
                    setTimeout(() => {
                        button.style.transform = 'scale(1.05)';
                        setTimeout(() => {
                            button.style.transform = 'scale(1)';
                        }, 100);
                    }, 100);
                });
            });

            // Add visual feedback to action buttons
            const actionButtons = document.querySelectorAll('.action-btn');
            actionButtons.forEach(button => {
                button.addEventListener('click', (e) => {
                    e.preventDefault();
                    button.style.transform = 'scale(0.95)';
                    setTimeout(() => {
                        button.style.transform = 'scale(1)';
                    }, 150);
                });
            });
        }

        setupFeatureCards() {
            const featureCards = document.querySelectorAll('.feature-card');
            
            featureCards.forEach(card => {
                card.addEventListener('mouseenter', () => {
                    card.style.transform = 'translateY(-8px) scale(1.02)';
                });
                
                card.addEventListener('mouseleave', () => {
                    card.style.transform = 'translateY(0) scale(1)';
                });
            });
        }

        setupPlatformCards() {
            const platformCards = document.querySelectorAll('.platform-card');
            
            platformCards.forEach(card => {
                card.addEventListener('click', () => {
                    // Add click animation
                    card.style.transform = 'scale(0.98)';
                    setTimeout(() => {
                        card.style.transform = 'scale(1)';
                    }, 150);
                });
            });
        }

        setupCopyButtons() {
            // Add copy functionality to code blocks
            const codeBlocks = document.querySelectorAll('.code-block');
            
            codeBlocks.forEach(block => {
                const copyButton = document.createElement('button');
                copyButton.className = 'copy-button';
                copyButton.innerHTML = '📋 Copy';
                copyButton.style.cssText = `
                    position: absolute;
                    top: 8px;
                    right: 8px;
                    background: rgba(255, 255, 255, 0.1);
                    color: white;
                    border: none;
                    padding: 4px 8px;
                    border-radius: 4px;
                    font-size: 12px;
                    cursor: pointer;
                    transition: background 0.2s;
                `;
                
                block.style.position = 'relative';
                block.appendChild(copyButton);
                
                copyButton.addEventListener('click', () => {
                    const code = block.querySelector('code');
                    if (code) {
                        navigator.clipboard.writeText(code.textContent).then(() => {
                            copyButton.innerHTML = '✅ Copied!';
                            setTimeout(() => {
                                copyButton.innerHTML = '📋 Copy';
                            }, 2000);
                        });
                    }
                });
                
                copyButton.addEventListener('mouseenter', () => {
                    copyButton.style.background = 'rgba(255, 255, 255, 0.2)';
                });
                
                copyButton.addEventListener('mouseleave', () => {
                    copyButton.style.background = 'rgba(255, 255, 255, 0.1)';
                });
            });
        }
    }

    // ========================================================================
    // PERFORMANCE OPTIMIZATIONS
    // ========================================================================

    class PerformanceOptimizer {
        constructor() {
            this.init();
        }

        init() {
            this.setupLazyLoading();
            this.setupImageOptimization();
            this.setupPreloading();
        }

        setupLazyLoading() {
            // Lazy load images
            const images = document.querySelectorAll('img[data-src]');
            
            if ('IntersectionObserver' in window) {
                const imageObserver = new IntersectionObserver((entries) => {
                    entries.forEach(entry => {
                        if (entry.isIntersecting) {
                            const img = entry.target;
                            img.src = img.dataset.src;
                            img.classList.remove('lazy');
                            imageObserver.unobserve(img);
                        }
                    });
                });

                images.forEach(img => imageObserver.observe(img));
            } else {
                // Fallback for older browsers
                images.forEach(img => {
                    img.src = img.dataset.src;
                });
            }
        }

        setupImageOptimization() {
            // Add loading="lazy" to images below the fold
            const images = document.querySelectorAll('img');
            images.forEach((img, index) => {
                if (index > 2) { // Skip first few images
                    img.loading = 'lazy';
                }
            });
        }

        setupPreloading() {
            // Preload critical resources
            const criticalResources = [
                'assets/style.css',
                'https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap'
            ];

            criticalResources.forEach(resource => {
                const link = document.createElement('link');
                link.rel = 'preload';
                link.href = resource;
                link.as = resource.endsWith('.css') ? 'style' : 'font';
                if (link.as === 'font') {
                    link.crossOrigin = 'anonymous';
                }
                document.head.appendChild(link);
            });
        }
    }

    // ========================================================================
    // ACCESSIBILITY ENHANCEMENTS
    // ========================================================================

    class AccessibilityEnhancer {
        constructor() {
            this.init();
        }

        init() {
            this.setupKeyboardNavigation();
            this.setupFocusManagement();
            this.setupARIALabels();
            this.setupReducedMotion();
        }

        setupKeyboardNavigation() {
            // Enable keyboard navigation for interactive elements
            const interactiveElements = document.querySelectorAll('.btn, .nav-link, .doc-link, .example-link');
            
            interactiveElements.forEach(element => {
                element.addEventListener('keydown', (e) => {
                    if (e.key === 'Enter' || e.key === ' ') {
                        e.preventDefault();
                        element.click();
                    }
                });
            });
        }

        setupFocusManagement() {
            // Improve focus visibility
            const focusableElements = document.querySelectorAll('a, button, input, textarea, select, [tabindex]');
            
            focusableElements.forEach(element => {
                element.addEventListener('focus', () => {
                    element.classList.add('focused');
                });
                
                element.addEventListener('blur', () => {
                    element.classList.remove('focused');
                });
            });
        }

        setupARIALabels() {
            // Add ARIA labels to elements that need them
            const socialLinks = document.querySelectorAll('.social-link');
            socialLinks.forEach(link => {
                if (!link.getAttribute('aria-label')) {
                    const platform = link.href.includes('github') ? 'GitHub' : 
                                   link.href.includes('twitter') ? 'Twitter' : 
                                   link.href.includes('discord') ? 'Discord' : 'Social Media';
                    link.setAttribute('aria-label', `Follow us on ${platform}`);
                }
            });

            // Add ARIA labels to navigation toggle
            const navToggle = document.getElementById('navToggle');
            if (navToggle) {
                navToggle.setAttribute('aria-label', 'Toggle navigation menu');
                navToggle.setAttribute('aria-expanded', 'false');
                
                navToggle.addEventListener('click', () => {
                    const expanded = navToggle.getAttribute('aria-expanded') === 'true';
                    navToggle.setAttribute('aria-expanded', !expanded);
                });
            }
        }

        setupReducedMotion() {
            // Respect user's motion preferences
            if (window.matchMedia('(prefers-reduced-motion: reduce)').matches) {
                document.documentElement.style.setProperty('--transition-fast', '0ms');
                document.documentElement.style.setProperty('--transition-normal', '0ms');
                document.documentElement.style.setProperty('--transition-slow', '0ms');
                
                // Disable floating animations
                const floatingElements = document.querySelectorAll('.floating-element');
                floatingElements.forEach(element => {
                    element.style.animation = 'none';
                });
            }
        }
    }

    // ========================================================================
    // ANALYTICS & TRACKING
    // ========================================================================

    class Analytics {
        constructor() {
            this.init();
        }

        init() {
            this.setupEventTracking();
            this.setupScrollTracking();
            this.setupPerformanceTracking();
        }

        setupEventTracking() {
            // Track button clicks
            const buttons = document.querySelectorAll('.btn, .doc-link, .example-link');
            buttons.forEach(button => {
                button.addEventListener('click', () => {
                    this.trackEvent('click', {
                        element: button.textContent.trim(),
                        location: window.location.pathname
                    });
                });
            });

            // Track external links
            const externalLinks = document.querySelectorAll('a[href^="http"]');
            externalLinks.forEach(link => {
                link.addEventListener('click', () => {
                    this.trackEvent('external_link', {
                        url: link.href,
                        text: link.textContent.trim()
                    });
                });
            });
        }

        setupScrollTracking() {
            const sections = document.querySelectorAll('section[id]');
            const sectionViews = new Set();
            
            const observer = new IntersectionObserver((entries) => {
                entries.forEach(entry => {
                    if (entry.isIntersecting && !sectionViews.has(entry.target.id)) {
                        sectionViews.add(entry.target.id);
                        this.trackEvent('section_view', {
                            section: entry.target.id
                        });
                    }
                });
            }, { threshold: 0.5 });

            sections.forEach(section => observer.observe(section));
        }

        setupPerformanceTracking() {
            // Track page load performance
            window.addEventListener('load', () => {
                setTimeout(() => {
                    const perfData = performance.getEntriesByType('navigation')[0];
                    this.trackEvent('performance', {
                        loadTime: Math.round(perfData.loadEventEnd - perfData.fetchStart),
                        domContentLoaded: Math.round(perfData.domContentLoadedEventEnd - perfData.fetchStart),
                        firstPaint: Math.round(performance.getEntriesByType('paint')[0]?.startTime || 0)
                    });
                }, 1000);
            });
        }

        trackEvent(eventName, data = {}) {
            // Send to analytics service (Google Analytics, etc.)
            if (typeof gtag !== 'undefined') {
                gtag('event', eventName, data);
            }
            
            // Console log for development
            if (window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1') {
                console.log('Analytics Event:', eventName, data);
            }
        }
    }

    // ========================================================================
    // ERROR HANDLING
    // ========================================================================

    class ErrorHandler {
        constructor() {
            this.init();
        }

        init() {
            this.setupGlobalErrorHandling();
            this.setupUnhandledRejectionHandling();
        }

        setupGlobalErrorHandling() {
            window.addEventListener('error', (event) => {
                console.error('Global error:', event.error);
                this.reportError('javascript_error', {
                    message: event.message,
                    filename: event.filename,
                    lineno: event.lineno,
                    colno: event.colno
                });
            });
        }

        setupUnhandledRejectionHandling() {
            window.addEventListener('unhandledrejection', (event) => {
                console.error('Unhandled promise rejection:', event.reason);
                this.reportError('promise_rejection', {
                    reason: event.reason?.toString() || 'Unknown'
                });
            });
        }

        reportError(type, data) {
            // Report to error tracking service
            if (typeof gtag !== 'undefined') {
                gtag('event', 'exception', {
                    description: `${type}: ${JSON.stringify(data)}`,
                    fatal: false
                });
            }
        }
    }

    // ========================================================================
    // INITIALIZATION
    // ========================================================================

    function initializeWebsite() {
        // Check if DOM is ready
        if (document.readyState === 'loading') {
            document.addEventListener('DOMContentLoaded', initializeWebsite);
            return;
        }

        try {
            // Initialize all components
            new Navigation();
            new ScrollAnimations();
            new InteractiveElements();
            new PerformanceOptimizer();
            new AccessibilityEnhancer();
            new Analytics();
            new ErrorHandler();

            console.log('ArduRoomba website initialized successfully');
        } catch (error) {
            console.error('Error initializing website:', error);
        }
    }

    // Start initialization
    initializeWebsite();

    // ========================================================================
    // UTILITY EXPORTS (for external use)
    // ========================================================================

    window.ArduRoombaWebsite = {
        smoothScrollTo,
        animateNumber,
        debounce,
        throttle,
        isInViewport
    };

})();