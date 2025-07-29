# Theming and Customization Examples

This directory contains comprehensive examples demonstrating component appearance with different theme configurations, custom styling, and theme switching scenarios for Element Fluent UI components.

## Overview

These examples showcase the powerful theming system of Element Fluent UI, demonstrating how to create consistent, beautiful, and customizable user interfaces that adapt to different themes, user preferences, and branding requirements.

## Theming Features Demonstrated

### 🎨 Core Theming Concepts

#### Theme Modes
- **Light Mode**: Optimized for well-lit environments
- **Dark Mode**: Reduced eye strain in low-light conditions
- **High Contrast**: Enhanced accessibility for users with visual impairments
- **Auto Mode**: Automatically switches based on system preferences

#### Color Systems
- **Accent Colors**: Primary brand colors with automatic variants
- **Semantic Colors**: Success, warning, error, and info colors
- **Neutral Colors**: Background, surface, and text colors
- **Adaptive Colors**: Colors that change based on theme mode

#### Typography System
- **Font Families**: Primary and secondary font stacks
- **Font Sizes**: Responsive typography scale
- **Font Weights**: Consistent weight hierarchy
- **Line Heights**: Optimized for readability

### 🔧 Customization Features

#### Design Tokens
- **Color Tokens**: Semantic color definitions
- **Spacing Tokens**: Consistent spacing scale
- **Border Radius Tokens**: Rounded corner variations
- **Shadow Tokens**: Elevation and depth effects

#### Component Styling
- **Style Variants**: Different visual styles for components
- **Size Variants**: Multiple size options
- **State Styling**: Hover, focus, active, and disabled states
- **Custom Properties**: CSS custom properties for easy customization

#### Brand Integration
- **Logo Integration**: Seamless brand logo placement
- **Color Palette**: Custom brand color integration
- **Typography**: Custom font integration
- **Iconography**: Custom icon set integration

## Example Categories

### 🎮 Interactive Theme Demos

#### [Theme Designer Studio](designer/theme-designer-studio/)
A comprehensive theme creation and testing environment:
- **Real-time theme editing** with live preview
- **Color palette generator** with accessibility checking
- **Typography customization** with font preview
- **Component gallery** showing all themed components
- **Export functionality** for theme configurations

#### [Brand Showcase](branding/brand-showcase/)
Demonstrates brand integration capabilities:
- **Multiple brand themes** with different color schemes
- **Logo integration** in various layouts
- **Custom typography** with brand fonts
- **Consistent styling** across all components

#### [Theme Switcher Demo](switching/theme-switcher/)
Shows dynamic theme switching:
- **Smooth transitions** between themes
- **Preference persistence** across sessions
- **System theme detection** and automatic switching
- **Custom theme selection** interface

### 🎨 Visual Design Examples

#### [Color System Explorer](colors/color-system/)
Interactive color system demonstration:
- **Color palette visualization** with all variants
- **Accessibility testing** for contrast ratios
- **Color harmony** and relationship visualization
- **Usage guidelines** for each color

#### [Typography Showcase](typography/typography-showcase/)
Comprehensive typography demonstration:
- **Font family combinations** and pairings
- **Responsive typography** scaling
- **Readability optimization** across themes
- **Multilingual support** examples

#### [Component Variants Gallery](variants/component-variants/)
Shows all component style variations:
- **Size variants** for different use cases
- **Style variants** for different contexts
- **State demonstrations** for all interactive states
- **Combination examples** showing components together

### 🏢 Real-World Applications

#### [Corporate Dashboard](applications/corporate-dashboard/)
A professional dashboard with corporate theming:
- **Corporate color scheme** with brand integration
- **Professional typography** and layout
- **Data visualization** with themed charts
- **Responsive design** for all devices

#### [Creative Portfolio](applications/creative-portfolio/)
A creative portfolio with artistic theming:
- **Vibrant color schemes** for creative expression
- **Custom typography** with artistic fonts
- **Image-focused layouts** with themed overlays
- **Interactive animations** with theme-aware effects

#### [E-commerce Platform](applications/ecommerce-platform/)
An e-commerce interface with commercial theming:
- **Trust-building colors** and professional appearance
- **Product-focused layouts** with themed components
- **Shopping cart integration** with consistent styling
- **Mobile-optimized** responsive design

## Getting Started

### Prerequisites

- Qt 6.5 or later
- Element Fluent UI library with theming support
- CMake 3.20 or later
- C++20 compatible compiler

### Building Examples

```bash
# Navigate to theming examples
cd examples/theming

# Create build directory
mkdir build && cd build

# Configure with theming features enabled
cmake .. -DENABLE_THEMING=ON

# Build all examples
cmake --build .
```

### Running Examples

```bash
# Run theme designer studio
./designer/theme-designer-studio/theme-designer-studio

# Run brand showcase
./branding/brand-showcase/brand-showcase

# Run theme switcher demo
./switching/theme-switcher/theme-switcher
```

## Theme Configuration

### Basic Theme Setup

```cpp
#include "FluentQt/Styling/FluentTheme.h"

// Get theme instance
auto& theme = FluentTheme::instance();

// Set theme mode
theme.setMode(FluentThemeMode::Dark);

// Set accent color
theme.setAccentColor(FluentAccentColor::Blue);

// Enable high contrast mode
theme.setHighContrastMode(true);
```

### Custom Theme Creation

```cpp
#include "FluentQt/Styling/FluentThemeConfig.h"

// Create custom theme configuration
FluentThemeConfig customTheme;

// Set custom colors
customTheme.colors.accent = QColor("#FF6B35");
customTheme.colors.background = QColor("#1E1E1E");
customTheme.colors.surface = QColor("#2D2D2D");
customTheme.colors.onSurface = QColor("#FFFFFF");

// Set custom typography
customTheme.typography.fontFamily = "Segoe UI";
customTheme.typography.baseFontSize = 14;

// Set custom spacing
customTheme.spacing.small = 8;
customTheme.spacing.medium = 16;
customTheme.spacing.large = 24;

// Apply custom theme
theme.applyCustomTheme(customTheme);
```

### Dynamic Theme Switching

```cpp
class ThemeSwitcher : public QWidget {
public:
    ThemeSwitcher() {
        setupUI();
        connectThemeSignals();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Theme mode buttons
        auto* lightButton = new FluentButton("Light");
        auto* darkButton = new FluentButton("Dark");
        auto* autoButton = new FluentButton("Auto");
        
        connect(lightButton, &FluentButton::clicked, []() {
            FluentTheme::instance().setMode(FluentThemeMode::Light);
        });
        
        connect(darkButton, &FluentButton::clicked, []() {
            FluentTheme::instance().setMode(FluentThemeMode::Dark);
        });
        
        connect(autoButton, &FluentButton::clicked, []() {
            FluentTheme::instance().setMode(FluentThemeMode::Auto);
        });
        
        layout->addWidget(lightButton);
        layout->addWidget(darkButton);
        layout->addWidget(autoButton);
    }
    
    void connectThemeSignals() {
        auto& theme = FluentTheme::instance();
        
        connect(&theme, &FluentTheme::themeChanged, [this]() {
            updateComponentAppearance();
        });
        
        connect(&theme, &FluentTheme::modeChanged, [this](FluentThemeMode mode) {
            handleModeChange(mode);
        });
    }
    
    void updateComponentAppearance() {
        // Update custom styling based on new theme
        updateCustomStyles();
        
        // Refresh component states
        refreshComponents();
    }
    
    void handleModeChange(FluentThemeMode mode) {
        QString modeText;
        switch (mode) {
        case FluentThemeMode::Light:
            modeText = "Light";
            break;
        case FluentThemeMode::Dark:
            modeText = "Dark";
            break;
        case FluentThemeMode::Auto:
            modeText = "Auto";
            break;
        }
        
        qDebug() << "Theme mode changed to:" << modeText;
    }
};
```

## Best Practices

### Theme Design Guidelines

1. **Maintain Consistency**
   - Use consistent color relationships across themes
   - Maintain visual hierarchy in all theme modes
   - Ensure consistent spacing and typography

2. **Accessibility First**
   - Test contrast ratios in all theme modes
   - Provide high contrast alternatives
   - Support system accessibility preferences

3. **Performance Optimization**
   - Cache theme resources for quick switching
   - Use efficient color calculations
   - Minimize theme switching overhead

4. **User Experience**
   - Provide smooth transitions between themes
   - Remember user theme preferences
   - Respect system theme settings

### Custom Styling Guidelines

1. **Use Design Tokens**
   - Leverage semantic color tokens
   - Use consistent spacing tokens
   - Follow typography scale

2. **Component Integration**
   - Extend existing component styles
   - Maintain component behavior
   - Preserve accessibility features

3. **Brand Integration**
   - Integrate brand colors thoughtfully
   - Maintain readability and usability
   - Test with brand guidelines

## Advanced Features

### Theme Animation

```cpp
// Smooth theme transitions
theme.setTransitionDuration(300); // milliseconds
theme.setTransitionEasing(QEasingCurve::OutCubic);

// Animate specific properties
theme.animateColorChange(FluentColorToken::Accent, newColor);
```

### Conditional Styling

```cpp
// Apply styles based on theme mode
auto* button = new FluentButton("Themed Button");

connect(&theme, &FluentTheme::modeChanged, [button](FluentThemeMode mode) {
    if (mode == FluentThemeMode::Dark) {
        button->setCustomStyle("dark-mode-button");
    } else {
        button->setCustomStyle("light-mode-button");
    }
});
```

### Theme Persistence

```cpp
// Save theme preferences
QSettings settings;
settings.setValue("theme/mode", static_cast<int>(theme.mode()));
settings.setValue("theme/accentColor", theme.accentColor().name());

// Load theme preferences
auto savedMode = static_cast<FluentThemeMode>(settings.value("theme/mode", 0).toInt());
auto savedAccent = QColor(settings.value("theme/accentColor", "#0078D4").toString());

theme.setMode(savedMode);
theme.setAccentColor(savedAccent);
```

## Testing Themes

### Visual Testing
- Test all components in each theme mode
- Verify color contrast ratios
- Check typography readability
- Validate brand integration

### Accessibility Testing
- Test with high contrast mode
- Verify screen reader compatibility
- Check keyboard navigation visibility
- Validate color-blind accessibility

### Performance Testing
- Measure theme switching performance
- Test memory usage with multiple themes
- Verify smooth animations
- Check resource loading efficiency

## Resources

### Design Tools
- **Color Palette Generators**: For creating harmonious color schemes
- **Contrast Checkers**: For accessibility validation
- **Typography Tools**: For font pairing and sizing
- **Brand Guidelines**: For consistent brand integration

### Development Tools
- **Theme Inspector**: For debugging theme values
- **Performance Profiler**: For optimization
- **Accessibility Auditor**: For compliance checking
- **Visual Regression Testing**: For consistency validation

## Example Highlights

### Theme Designer Studio
The most comprehensive theming example featuring:
- **Real-time theme editing** with instant visual feedback
- **Color accessibility checking** with WCAG compliance validation
- **Component gallery** showing all components in current theme
- **Export/import functionality** for sharing theme configurations
- **Brand integration tools** for corporate identity

### Brand Showcase
Demonstrates professional brand integration:
- **Multiple brand examples** (Corporate, Creative, E-commerce)
- **Logo placement strategies** across different layouts
- **Color psychology** application in UI design
- **Consistent brand experience** across all components

### Theme Switcher Demo
Shows advanced theme switching capabilities:
- **Smooth animated transitions** between theme modes
- **System preference detection** and automatic adaptation
- **User preference persistence** across application sessions
- **Custom theme creation** and management

## Learning Path

1. **Start with Theme Switcher** to understand basic theme concepts
2. **Explore Brand Showcase** to see real-world applications
3. **Use Theme Designer Studio** to create custom themes
4. **Study the code** to understand implementation patterns
5. **Create your own themes** using these examples as templates

These theming examples provide a comprehensive foundation for creating beautiful, accessible, and customizable user interfaces with Element Fluent UI.
