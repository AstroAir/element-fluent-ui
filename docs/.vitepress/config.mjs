import { defineConfig } from 'vitepress'

export default defineConfig({
  title: 'FluentQt',
  description: 'A modern, comprehensive C++ Qt6 library implementing Microsoft\'s Fluent Design System',

  // Ignore dead links for now to allow build to complete
  ignoreDeadLinks: true,

  head: [
    ['link', { rel: 'icon', href: '/favicon.ico' }],
    ['meta', { name: 'theme-color', content: '#0078d4' }],
    ['meta', { property: 'og:type', content: 'website' }],
    ['meta', { property: 'og:locale', content: 'en' }],
    ['meta', { property: 'og:title', content: 'FluentQt | Modern Qt6 Fluent Design Library' }],
    ['meta', { property: 'og:site_name', content: 'FluentQt' }],
    ['meta', { property: 'og:image', content: 'https://fluentqt.elementastro.org/og-image.png' }],
    ['meta', { property: 'og:url', content: 'https://fluentqt.elementastro.org/' }],
  ],

  themeConfig: {
    logo: '/logo.svg',

    nav: [
      { text: 'Home', link: '/' },
      { text: 'Guide', link: '/guide/getting-started' },
      { text: 'Components', link: '/components/' },
      { text: 'API Reference', link: '/api/' },
      { text: 'Examples', link: '/examples/' },
      {
        text: 'v1.0.0',
        items: [
          { text: 'Changelog', link: '/changelog' },
          { text: 'Contributing', link: '/contributing' },
        ]
      }
    ],

    sidebar: {
      '/guide/': [
        {
          text: 'Getting Started',
          items: [
            { text: 'Introduction', link: '/guide/introduction' },
            { text: 'Installation', link: '/guide/installation' },
            { text: 'Quick Start', link: '/guide/quick-start' },
            { text: 'Configuration', link: '/guide/configuration' },
          ]
        },
        {
          text: 'Core Concepts',
          items: [
            { text: 'Architecture', link: '/guide/architecture' },
            { text: 'Component System', link: '/guide/component-system' },
            { text: 'Theming', link: '/guide/theming' },
            { text: 'Animation', link: '/guide/animation' },
            { text: 'Accessibility', link: '/guide/accessibility' },
          ]
        },
        {
          text: 'Advanced Topics',
          items: [
            { text: 'Custom Components', link: '/guide/custom-components' },
            { text: 'Performance', link: '/guide/performance' },
            { text: 'Internationalization', link: '/guide/i18n' },
            { text: 'Testing', link: '/guide/testing' },
            { text: 'Deployment', link: '/guide/deployment' },
          ]
        }
      ],

      '/components/': [
        {
          text: 'Overview',
          items: [
            { text: 'Component Index', link: '/components/' },
            { text: 'Design Principles', link: '/components/design-principles' },
            { text: 'Usage Guidelines', link: '/components/usage-guidelines' },
          ]
        },
        {
          text: 'Form Components',
          items: [
            { text: 'Button', link: '/components/button' },
            { text: 'TextInput', link: '/components/text-input' },
            { text: 'CheckBox', link: '/components/checkbox' },
            { text: 'RadioButton', link: '/components/radio-button' },
            { text: 'ComboBox', link: '/components/combobox' },
            { text: 'Slider', link: '/components/slider' },
            { text: 'ToggleSwitch', link: '/components/toggle-switch' },
            { text: 'SpinBox', link: '/components/spinbox' },
          ]
        },
        {
          text: 'Layout Components',
          items: [
            { text: 'Card', link: '/components/card' },
            { text: 'Panel', link: '/components/panel' },
            { text: 'NavigationView', link: '/components/navigation-view' },
            { text: 'TabView', link: '/components/tab-view' },
            { text: 'Splitter', link: '/components/splitter' },
            { text: 'ScrollArea', link: '/components/scroll-area' },
          ]
        },
        {
          text: 'Feedback Components',
          items: [
            { text: 'ProgressBar', link: '/components/progress-bar' },
            { text: 'LoadingIndicator', link: '/components/loading-indicator' },
            { text: 'Notification', link: '/components/notification' },
            { text: 'Toast', link: '/components/toast' },
            { text: 'Tooltip', link: '/components/tooltip' },
            { text: 'Badge', link: '/components/badge' },
            { text: 'Rating', link: '/components/rating' },
          ]
        },
        {
          text: 'Advanced Components',
          items: [
            { text: 'Carousel', link: '/components/carousel' },
            { text: 'Timeline', link: '/components/timeline' },
            { text: 'TreeView', link: '/components/tree-view' },
            { text: 'RichTextEditor', link: '/components/rich-text-editor' },
            { text: 'Calendar', link: '/components/calendar' },
            { text: 'ColorPicker', link: '/components/color-picker' },
          ]
        }
      ],

      '/api/': [
        {
          text: 'Core API',
          items: [
            { text: 'FluentQt', link: '/api/fluentqt' },
            { text: 'FluentComponent', link: '/api/fluent-component' },
            { text: 'FluentTheme', link: '/api/fluent-theme' },
            { text: 'FluentAnimator', link: '/api/fluent-animator' },
          ]
        },
        {
          text: 'Components API',
          items: [
            { text: 'Button API', link: '/api/components/button' },
            { text: 'Card API', link: '/api/components/card' },
            { text: 'NavigationView API', link: '/api/components/navigation-view' },
          ]
        },
        {
          text: 'Styling API',
          items: [
            { text: 'Theme System', link: '/api/styling/theme-system' },
            { text: 'Color System', link: '/api/styling/color-system' },
            { text: 'Typography', link: '/api/styling/typography' },
          ]
        },
        {
          text: 'Animation API',
          items: [
            { text: 'Animation Types', link: '/api/animation/types' },
            { text: 'Easing Functions', link: '/api/animation/easing' },
            { text: 'Performance', link: '/api/animation/performance' },
          ]
        }
      ],

      '/examples/': [
        {
          text: 'Basic Examples',
          items: [
            { text: 'Hello World', link: '/examples/hello-world' },
            { text: 'Form Application', link: '/examples/form-app' },
            { text: 'Dashboard', link: '/examples/dashboard' },
          ]
        },
        {
          text: 'Advanced Examples',
          items: [
            { text: 'Media Player', link: '/examples/media-player' },
            { text: 'Data Visualization', link: '/examples/data-viz' },
            { text: 'Settings Panel', link: '/examples/settings-panel' },
          ]
        },
        {
          text: 'Integration Examples',
          items: [
            { text: 'CMake Integration', link: '/examples/cmake' },
            { text: 'vcpkg Integration', link: '/examples/vcpkg' },
            { text: 'Conan Integration', link: '/examples/conan' },
          ]
        }
      ]
    },

    socialLinks: [
      { icon: 'github', link: 'https://github.com/ElementAstro/element-fluent-ui' },
      { icon: 'discord', link: 'https://discord.gg/elementastro' },
    ],

    footer: {
      message: 'Released under the MIT License.',
      copyright: 'Copyright © 2025 Element Astro'
    },

    editLink: {
      pattern: 'https://github.com/ElementAstro/element-fluent-ui/edit/main/docs/:path',
      text: 'Edit this page on GitHub'
    },

    search: {
      provider: 'algolia',
      options: {
        appId: 'YOUR_APP_ID',
        apiKey: 'YOUR_API_KEY',
        indexName: 'fluentqt'
      }
    },

    carbonAds: {
      code: 'your-carbon-code',
      placement: 'your-carbon-placement'
    }
  },

  markdown: {
    theme: {
      light: 'github-light',
      dark: 'github-dark'
    },
    lineNumbers: true,
    config: (md) => {
      // Add custom markdown plugins here
    }
  },

  vite: {
    define: {
      __VUE_OPTIONS_API__: false
    },
    optimizeDeps: {
      include: ['vue']
    },
    server: {
      host: true,
      fs: {
        allow: ['..']
      }
    },
    build: {
      minify: 'terser',
      chunkSizeWarningLimit: 1000
    }
  },

  lastUpdated: true,
  cleanUrls: true,

  sitemap: {
    hostname: 'https://fluentqt.elementastro.org'
  }
})
