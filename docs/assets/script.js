/**
 * ArduRoomba Website JavaScript
 * Handles navigation, animations, and interactive elements
 * No fake functionality - visual effects only
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
        if (element) {
            element.scrollIntoView({
                behavior: 'smooth',
                block: 'start'
            });
        }
    }

    // ========================================================================
    // MAIN APPLICATION CLASS
    // ========================================================================

    class ArduRoombaWebsite {
        constructor() {
            this.isLoaded = false;
            this.scrollPosition = 0;
            this.isScrolling = false;
            
            this.init();
        }

        init() {
            // Wait for DOM to be ready
            if (document.readyState === 'loading') {
                document.addEventListener('DOMContentLoaded', () => this.onDOMReady());
            } else {
                this.onDOMReady();
            }
        }

        onDOMReady() {
            this.setupNavigation();
            this.setupScrollEffects();
            this.setupHeroDevice();
            this.setupFeatureCards();
            this.setupExampleCards();
            this.setupCopyButtons();
            this.setupAccessibility();
            this.setupPerformanceOptimizations();
            
            this.isLoaded = true;
            
            // Add loaded class for CSS animations
            document.body.classList.add('loaded');
        }

        // ====================================================================
        // NAVIGATION
        // ====================================================================

        setupNavigation() {
            const navToggle = document.getElementById('navToggle');
            const navMenu = document.getElementById('navMenu');
            const navLinks = document.querySelectorAll('.nav-link');

            // Mobile menu toggle
            if (navToggle && navMenu) {
                navToggle.addEventListener('click', () => {
                    navMenu.classList.toggle('active');
                    navToggle.classList.toggle('active');
                });
            }

            // Smooth scroll for navigation links
            navLinks.forEach(link => {
                link.addEventListener('click', (e) => {
                    const href = link.getAttribute('href');
                    if (href && href.startsWith('#')) {
                        e.preventDefault();
                        const target = document.querySelector(href);
                        if (target) {
                            smoothScrollTo(target);
                            
                            // Close mobile menu if open
                            if (navMenu) navMenu.classList.remove('active');
                            if (navToggle) navToggle.classList.remove('active');
                        }
                    }
                });
            });

            // Update active nav link on scroll
            this.updateActiveNavLink();
        }

        updateActiveNavLink() {
            const sections = document.querySelectorAll('section[id]');
            const navLinks = document.querySelectorAll('.nav-link');

            const updateActive = throttle(() => {
                let current = '';
                const scrollPosition = window.scrollY + 100;

                sections.forEach(section => {
                    const sectionTop = section.offsetTop;
                    const sectionHeight = section.offsetHeight;
                    
                    if (scrollPosition >= sectionTop && scrollPosition < sectionTop + sectionHeight) {
                        current = section.getAttribute('id');
                    }
                });

                navLinks.forEach(link => {
                    link.classList.remove('active');
                    if (link.getAttribute('href') === `#${current}`) {
                        link.classList.add('active');
                    }
                });
            }, 100);

            window.addEventListener('scroll', updateActive);
        }

        // ====================================================================
        // SCROLL EFFECTS
        // ====================================================================

        setupScrollEffects() {
            const navbar = document.querySelector('.navbar');
            
            const handleScroll = throttle(() => {
                const currentScrollY = window.scrollY;
                
                // Navbar background on scroll
                if (navbar) {
                    if (currentScrollY > 50) {
                        navbar.classList.add('scrolled');
                    } else {
                        navbar.classList.remove('scrolled');
                    }
                }

                this.scrollPosition = currentScrollY;
            }, 16);

            window.addEventListener('scroll', handleScroll);

            // Intersection Observer for fade-in animations
            this.setupIntersectionObserver();
        }

        setupIntersectionObserver() {
            const observerOptions = {
                threshold: 0.1,
                rootMargin: '0px 0px -50px 0px'
            };

            const observer = new IntersectionObserver((entries) => {
                entries.forEach(entry => {
                    if (entry.isIntersecting) {
                        entry.target.classList.add('animate-in');
                    }
                });
            }, observerOptions);

            // Observe elements for animation
            const animatedElements = document.querySelectorAll(
                '.feature-card, .platform-card, .example-card, .doc-card, .step'
            );
            
            animatedElements.forEach(el => {
                observer.observe(el);
            });
        }

        // ====================================================================
        // HERO DEVICE INTERACTION
        // ====================================================================

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

        // ====================================================================
        // FEATURE CARDS
        // ====================================================================

        setupFeatureCards() {
            const featureCards = document.querySelectorAll('.feature-card');
            
            featureCards.forEach(card => {
                card.addEventListener('mouseenter', () => {
                    card.style.transform = 'translateY(-8px)';
                });
                
                card.addEventListener('mouseleave', () => {
                    card.style.transform = 'translateY(0)';
                });
            });
        }

        // ====================================================================
        // EXAMPLE CARDS
        // ====================================================================

        setupExampleCards() {
            const exampleCards = document.querySelectorAll('.example-card');
            
            exampleCards.forEach(card => {
                const link = card.querySelector('.example-link');
                
                card.addEventListener('mouseenter', () => {
                    card.style.transform = 'translateY(-4px)';
                    if (link) link.style.color = 'var(--primary-color)';
                });
                
                card.addEventListener('mouseleave', () => {
                    card.style.transform = 'translateY(0)';
                    if (link) link.style.color = '';
                });
            });
        }

        // ====================================================================
        // CODE BLOCKS AND COPY FUNCTIONALITY
        // ====================================================================

        setupCopyButtons() {
            const codeBlocks = document.querySelectorAll('.code-block');
            
            codeBlocks.forEach(block => {
                // Create copy button
                const copyButton = document.createElement('button');
                copyButton.className = 'copy-button';
                copyButton.innerHTML = '📋 Copy';
                copyButton.setAttribute('aria-label', 'Copy code to clipboard');
                
                // Add copy functionality
                copyButton.addEventListener('click', async () => {
                    const code = block.querySelector('code');
                    if (code) {
                        try {
                            await navigator.clipboard.writeText(code.textContent);
                            copyButton.innerHTML = '✅ Copied!';
                            copyButton.classList.add('copied');
                            
                            setTimeout(() => {
                                copyButton.innerHTML = '📋 Copy';
                                copyButton.classList.remove('copied');
                            }, 2000);
                        } catch (err) {
                            console.error('Failed to copy code:', err);
                            copyButton.innerHTML = '❌ Failed';
                            setTimeout(() => {
                                copyButton.innerHTML = '📋 Copy';
                            }, 2000);
                        }
                    }
                });
                
                // Position the button
                block.style.position = 'relative';
                block.appendChild(copyButton);
            });
        }

        // ====================================================================
        // ACCESSIBILITY
        // ====================================================================

        setupAccessibility() {
            // Add focus indicators for keyboard navigation
            const focusableElements = document.querySelectorAll('a, button, input, textarea, select, [tabindex]');
            
            focusableElements.forEach(element => {
                element.addEventListener('focus', () => {
                    element.classList.add('keyboard-focus');
                });
                
                element.addEventListener('blur', () => {
                    element.classList.remove('keyboard-focus');
                });
            });

            // Skip to main content link
            const skipLink = document.createElement('a');
            skipLink.href = '#main';
            skipLink.className = 'skip-link';
            skipLink.textContent = 'Skip to main content';
            document.body.insertBefore(skipLink, document.body.firstChild);

            // Reduce motion for users who prefer it
            if (window.matchMedia('(prefers-reduced-motion: reduce)').matches) {
                document.body.classList.add('reduce-motion');
            }
        }

        // ====================================================================
        // PERFORMANCE OPTIMIZATIONS
        // ====================================================================

        setupPerformanceOptimizations() {
            // Lazy load images
            const images = document.querySelectorAll('img[data-src]');
            const imageObserver = new IntersectionObserver((entries) => {
                entries.forEach(entry => {
                    if (entry.isIntersecting) {
                        const img = entry.target;
                        img.src = img.dataset.src;
                        img.removeAttribute('data-src');
                        imageObserver.unobserve(img);
                    }
                });
            });

            images.forEach(img => imageObserver.observe(img));

            // Preload critical resources
            this.preloadCriticalResources();
        }

        preloadCriticalResources() {
            // Preload fonts
            const fontLink = document.createElement('link');
            fontLink.rel = 'preload';
            fontLink.as = 'font';
            fontLink.type = 'font/woff2';
            fontLink.crossOrigin = 'anonymous';
            fontLink.href = 'https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700&display=swap';
            document.head.appendChild(fontLink);
        }
    }

    // ========================================================================
    // INITIALIZATION
    // ========================================================================

    // Initialize the website when DOM is ready
    let website = null;
    
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', () => {
            website = new ArduRoombaWebsite();
        });
    } else {
        website = new ArduRoombaWebsite();
    }

    // Export for debugging (development only)
    if (typeof window !== 'undefined') {
        window.ArduRoombaWebsite = website;
    }

})();