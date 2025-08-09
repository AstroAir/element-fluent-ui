---
layout: home

hero:
  name: "FluentQt"
  text: "Modern Qt6 Fluent Design"
  tagline: "A comprehensive C++ library implementing Microsoft's Fluent Design System for Qt6 applications"
  image:
    src: /logo.svg
    alt: FluentQt Logo
  actions:
    - theme: brand
      text: Get Started
      link: /guide/getting-started
    - theme: alt
      text: View Components
      link: /components/
    - theme: alt
      text: GitHub
      link: https://github.com/ElementAstro/element-fluent-ui

features:
  - icon: 🎨
    title: Complete Design System
    details: 30+ UI components implementing authentic Microsoft Fluent Design with light & dark themes, accessibility support, and responsive layouts.

  - icon: 🚀
    title: Advanced Animation System
    details: 36 easing functions and 28+ animation types with hardware acceleration, performance optimization, and reduced motion support.

  - icon: 🛠️
    title: Modern C++20
    details: Leverages latest C++20 features with Qt6 integration, type safety, memory safety, and comprehensive error handling.

  - icon: ♿
    title: Accessibility First
    details: WCAG 2.1 AA compliant with screen reader support, keyboard navigation, high contrast themes, and accessibility validation tools.

  - icon: 📱
    title: Cross Platform
    details: Native support for Windows, macOS, and Linux with high DPI scaling, touch-friendly interfaces, and platform-specific optimizations.

  - icon: 📚
    title: Developer Experience
    details: Comprehensive documentation, interactive examples, CMake integration, and package manager support (vcpkg, Conan).
---

## Quick Example

Get started with FluentQt in just a few lines of code:

```cpp
#include <QApplication>
#include "FluentQt/FluentQt.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Initialize FluentQt
    FluentQt::initialize(&app);
    
    // Create a card with components
    auto* card = new FluentQt::Components::FluentCard("Welcome");
    card->setSubtitle("Modern UI components for Qt applications");
    
    auto* layout = card->createVerticalLayout();
    layout->addWidget(new FluentQt::Components::FluentTextInput("Enter your name..."));
    layout->addWidget(FluentQt::Components::FluentButton::createPrimaryButton("Get Started"));
    
    card->show();
    return app.exec();
}
```

## Why FluentQt?

### 🎯 **Production Ready**
Built for real-world applications with comprehensive testing, performance optimization, and enterprise-grade reliability.

### 🔧 **Easy Integration**
Simple CMake integration with support for vcpkg, Conan, and other package managers. Works with existing Qt projects.

### 🎨 **Authentic Fluent Design**
Faithful implementation of Microsoft's Fluent Design System with proper animations, spacing, typography, and color systems.

### 📈 **Performance Focused**
Optimized for smooth 60fps animations, minimal memory usage, and efficient rendering across all supported platforms.

## Component Showcase

<div class="component-grid">
  <div class="component-card">
    <h3>Form Components</h3>
    <p>Buttons, inputs, checkboxes, sliders, and more with built-in validation and accessibility.</p>
    <a href="/components/button">Explore →</a>
  </div>
  
  <div class="component-card">
    <h3>Layout Components</h3>
    <p>Cards, panels, navigation views, and responsive layouts for organizing content.</p>
    <a href="/components/card">Explore →</a>
  </div>
  
  <div class="component-card">
    <h3>Feedback Components</h3>
    <p>Progress bars, notifications, tooltips, and loading indicators for user feedback.</p>
    <a href="/components/progress-bar">Explore →</a>
  </div>
  
  <div class="component-card">
    <h3>Advanced Components</h3>
    <p>Carousels, timelines, rich text editors, and data visualization components.</p>
    <a href="/components/carousel">Explore →</a>
  </div>
</div>

## Installation

### Using vcpkg
```bash
vcpkg install fluentqt
```

### Using Conan
```bash
conan install fluentqt/1.0.0@
```

### Using CMake
```cmake
find_package(FluentQt REQUIRED)
target_link_libraries(your_target FluentQt::FluentQt)
```

## Community

Join our growing community of developers building beautiful Qt applications:

- 💬 [Discord Server](https://discord.gg/elementastro) - Get help and discuss features
- 🐛 [GitHub Issues](https://github.com/ElementAstro/element-fluent-ui/issues) - Report bugs and request features
- 📖 [Discussions](https://github.com/ElementAstro/element-fluent-ui/discussions) - Share your projects and ideas
- 📧 [Newsletter](https://elementastro.org/newsletter) - Stay updated with releases and tutorials

## Sponsors

FluentQt is an open source project. Consider sponsoring to support development:

<div class="sponsors">
  <a href="https://github.com/sponsors/ElementAstro" class="sponsor-button">
    ❤️ Sponsor FluentQt
  </a>
</div>

<style>
.component-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 1rem;
  margin: 2rem 0;
}

.component-card {
  border: 1px solid var(--vp-c-border);
  border-radius: 8px;
  padding: 1.5rem;
  background: var(--vp-c-bg-soft);
  transition: all 0.2s ease;
}

.component-card:hover {
  border-color: var(--vp-c-brand);
  transform: translateY(-2px);
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
}

.component-card h3 {
  margin: 0 0 0.5rem 0;
  color: var(--vp-c-brand);
}

.component-card p {
  margin: 0 0 1rem 0;
  color: var(--vp-c-text-2);
  font-size: 0.9rem;
  line-height: 1.5;
}

.component-card a {
  color: var(--vp-c-brand);
  text-decoration: none;
  font-weight: 500;
}

.sponsors {
  text-align: center;
  margin: 2rem 0;
}

.sponsor-button {
  display: inline-block;
  padding: 0.75rem 1.5rem;
  background: var(--vp-c-brand);
  color: white;
  text-decoration: none;
  border-radius: 6px;
  font-weight: 500;
  transition: background 0.2s ease;
}

.sponsor-button:hover {
  background: var(--vp-c-brand-dark);
}
</style>
