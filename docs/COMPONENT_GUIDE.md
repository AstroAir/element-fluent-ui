# Element Fluent UI - Component Guide

This guide provides comprehensive documentation for all Element Fluent UI components, including the enhanced animation system, visual design improvements, and expanded component library implementing Microsoft's Fluent Design System.

> **📚 Complete Documentation Suite:**
> - **[API Reference](API_REFERENCE.md)** - Complete API documentation for all classes and methods
> - **[Developer Guide](DEVELOPER_GUIDE.md)** - Setup, build process, and contribution guidelines
> - **[Migration Guide](MIGRATION_GUIDE.md)** - Migration from other UI libraries
> - **[Individual Component Docs](components/)** - Detailed documentation for each component

## Table of Contents

1. [Animation System](#animation-system)
2. [Visual Design System](#visual-design-system)
3. [Form Components](#form-components)
4. [Feedback Components](#feedback-components)
5. [Layout and Navigation Components](#layout-and-navigation-components)
6. [Integration Guide](#integration-guide)
7. [Best Practices](#best-practices)
8. [See Also](#see-also)

## Animation System

### FluentAnimator

The enhanced `FluentAnimator` class provides comprehensive animation capabilities with 36 easing functions and 28 animation types.

> **📖 For complete FluentAnimator API documentation, see [API Reference - FluentAnimator](API_REFERENCE.md#fluentanimator)**

#### Basic Usage

```cpp
#include "FluentQt/Animation/FluentAnimator.h"

auto* animator = new FluentAnimator(this);

// Simple fade in animation
auto* fadeAnimation = animator->fadeIn(widget, 300, FluentEasing::CubicOut);
fadeAnimation->start();

// Slide animation with callback
auto* slideAnimation = animator->slideUp(widget, 250);
connect(slideAnimation, &QPropertyAnimation::finished, [=]() {
    qDebug() << "Animation completed!";
});
slideAnimation->start();
```

#### Available Easing Functions

**Basic Easing:**
- `Linear` - Constant speed
- `QuadIn/Out/InOut` - Quadratic acceleration/deceleration
- `CubicIn/Out/InOut` - Cubic curves
- `QuartIn/Out/InOut` - Quartic curves
- `QuintIn/Out/InOut` - Quintic curves

**Advanced Easing:**
- `SineIn/Out/InOut` - Sinusoidal curves
- `ExpoIn/Out/InOut` - Exponential curves
- `CircIn/Out/InOut` - Circular curves
- `BounceIn/Out/InOut` - Bouncing effects
- `ElasticIn/Out/InOut` - Elastic spring effects
- `BackIn/Out/InOut` - Overshoot effects

#### Animation Types

**Basic Animations:**
```cpp
// Opacity animations
animator->fadeIn(widget);
animator->fadeOut(widget);

// Movement animations
animator->slideUp(widget);
animator->slideDown(widget);
animator->slideLeft(widget);
animator->slideRight(widget);

// Scale animations
animator->scaleIn(widget);
animator->scaleOut(widget);

// Rotation animations (fallback to opacity on Qt limitations)
animator->rotateIn(widget);
animator->rotateOut(widget);
```

**Micro-interactions:**
```cpp
// Hover effects
animator->hoverEffect(widget);

// Press feedback
animator->pressEffect(widget);

// Focus indication
animator->focusEffect(widget);

// Attention-grabbing effects
animator->pulseEffect(widget);
animator->shakeEffect(widget);
```

#### Performance Configuration

```cpp
FluentAnimationConfig config;
config.enableMicroInteractions = true;
config.respectReducedMotion = true;
config.enableHardwareAcceleration = true;
config.debugMode = false;

// Apply configuration (implementation-specific)
```

## Visual Design System

### FluentTheme

The enhanced `FluentTheme` class provides comprehensive design tokens and utilities.

#### Typography System

```cpp
auto& theme = FluentTheme::instance();

// 14 font methods available
QFont caption = theme.captionFont();      // 12px
QFont body = theme.bodyFont();            // 14px
QFont subtitle = theme.subtitleFont();    // 16px
QFont title = theme.titleFont();          // 20px
QFont display = theme.displayFont();      // 28px

// Specialized fonts
QFont button = theme.buttonFont();        // Optimized for buttons
QFont link = theme.linkFont();           // Underlined for links
QFont code = theme.codeFont();           // Monospace for code
```

#### Color System

```cpp
// Basic colors
QColor background = theme.color("backgroundPrimary");
QColor text = theme.color("textPrimary");
QColor accent = theme.color("accent");

// Semantic colors
QColor success = theme.color("success");
QColor warning = theme.color("warning");
QColor error = theme.color("error");

// Surface colors
QColor surface = theme.color("surface");
QColor surfaceVariant = theme.color("surfaceVariant");
```

#### Spacing System

```cpp
// Standard spacing scale
int xs = theme.spacing("xs");        // 4px
int small = theme.spacing("small");  // 8px
int medium = theme.spacing("medium"); // 16px
int large = theme.spacing("large");  // 24px
int xl = theme.spacing("xl");        // 32px

// Component-specific spacing
int buttonHeight = theme.componentHeight("medium");  // 32px
int inputPadding = theme.padding("medium");          // 12px
int cardMargin = theme.margins("large");             // 24px
```

#### Accessibility Features

```cpp
// High contrast mode
theme.setHighContrastMode(true);

// Contrast ratio checking
qreal ratio = theme.contrastRatio(textColor, backgroundColor);
bool accessible = ratio >= 4.5; // WCAG AA standard

// Automatic contrast adjustment
QColor adjustedText = theme.ensureContrast(textColor, backgroundColor, 4.5);
```

#### Color Utilities

```cpp
// Color manipulation
QColor lighter = theme.lighten(baseColor, 0.2);    // 20% lighter
QColor darker = theme.darken(baseColor, 0.3);      // 30% darker
QColor transparent = theme.adjustAlpha(color, 0.5); // 50% opacity

// Color blending
QColor blended = theme.blend(color1, color2, 0.5); // 50/50 blend
```

## Form Components

> **📖 For detailed component documentation:**
> - **[FluentButton](components/FluentButton.md)** - Complete button component guide
> - **[FluentTextInput](components/FluentTextInput.md)** - Advanced text input documentation
> - **[API Reference - Form Components](API_REFERENCE.md#form-components)** - Complete API documentation

### FluentTextInput

Enhanced text input with validation, states, and animations.

```cpp
#include "FluentQt/Components/FluentTextInput.h"

auto* emailInput = new FluentTextInput("Enter your email...");

// Validation
emailInput->setValidationType(FluentTextInputValidation::Email);
connect(emailInput, &FluentTextInput::validationStateChanged, [](bool isValid) {
    qDebug() << "Email is" << (isValid ? "valid" : "invalid");
});

// Password mode
auto* passwordInput = new FluentTextInput("Password");
passwordInput->setPasswordMode(true);
passwordInput->setPasswordVisible(false); // Hidden by default

// Multiline support
auto* textArea = new FluentTextInput();
textArea->setMultiline(true);
textArea->setMaxLength(500);

// Clear button
emailInput->setClearButtonEnabled(true);
```

#### Validation Types

- `None` - No validation
- `Email` - Email format validation
- `URL` - URL format validation
- `Number` - Numeric validation
- `Phone` - Phone number validation
- `Custom` - Custom validation function

```cpp
// Custom validation
input->setValidationType(FluentTextInputValidation::Custom);
input->setCustomValidator([](const QString& text) {
    return text.length() >= 8 && text.contains(QRegExp("[0-9]"));
});
```

### FluentCheckBox

Enhanced checkbox with animations and tri-state support.

```cpp
#include "FluentQt/Components/FluentCheckBox.h"

auto* checkBox = new FluentCheckBox("I agree to the terms");

// Tri-state support
checkBox->setTristate(true);
checkBox->setCheckState(Qt::PartiallyChecked);

// Animation control
checkBox->setAnimated(true);

// State monitoring
connect(checkBox, &FluentCheckBox::stateChanged, [](Qt::CheckState state) {
    switch (state) {
        case Qt::Unchecked:
            qDebug() << "Unchecked";
            break;
        case Qt::PartiallyChecked:
            qDebug() << "Partially checked";
            break;
        case Qt::Checked:
            qDebug() << "Checked";
            break;
    }
});
```

## Feedback Components

### FluentProgressBar

Multi-type progress indicator with animations.

```cpp
#include "FluentQt/Components/FluentProgressBar.h"

// Determinate progress
auto* progressBar = new FluentProgressBar();
progressBar->setType(FluentProgressType::Determinate);
progressBar->setValue(75); // 75%

// Indeterminate progress
auto* loadingBar = new FluentProgressBar();
loadingBar->setType(FluentProgressType::Indeterminate);
loadingBar->start();

// Ring progress
auto* ringProgress = new FluentProgressBar();
ringProgress->setType(FluentProgressType::Ring);
ringProgress->setSize(FluentProgressSize::Large);

// Custom styling
progressBar->setAccentColor(QColor(255, 0, 0)); // Red progress
progressBar->setShowText(true);
```

### FluentLoadingIndicator

Animated loading indicators with multiple styles.

```cpp
#include "FluentQt/Components/FluentLoadingIndicator.h"

// Spinner indicator
auto* spinner = new FluentLoadingIndicator();
spinner->setType(FluentLoadingType::Spinner);
spinner->setSize(FluentLoadingSize::Medium);
spinner->start();

// Dots indicator
auto* dots = new FluentLoadingIndicator();
dots->setType(FluentLoadingType::Dots);
dots->setText("Loading...");
dots->setSpeed(1.5); // 1.5x speed

// Available types: Spinner, Dots, Pulse, Bars, Ring, Wave
```

### FluentTooltip

Rich tooltips with smart positioning.

```cpp
#include "FluentQt/Components/FluentTooltip.h"

// Simple tooltip
FluentTooltip::showTooltip(widget, "This is a helpful tooltip");

// Rich tooltip
auto* tooltip = new FluentTooltip();
tooltip->setTitle("Advanced Feature");
tooltip->setBody("This feature provides advanced functionality with multiple options.");
tooltip->setPosition(FluentTooltipPosition::TopCenter);
tooltip->showFor(widget);

// Auto-positioning
tooltip->setPosition(FluentTooltipPosition::Auto); // Smart positioning
```

## Layout and Navigation Components

> **📖 For detailed component documentation:**
> - **[FluentPanel](components/FluentPanel.md)** - Complete panel component guide
> - **[API Reference - Layout Components](API_REFERENCE.md#layout-components)** - Complete API documentation

### FluentPanel

Versatile panel component with multiple types and elevation.

```cpp
#include "FluentQt/Components/FluentPanel.h"

// Card panel
auto* card = new FluentPanel("Card Title");
card->setPanelType(FluentPanelType::Card);
card->setElevation(FluentPanelElevation::Medium);
card->setCornerRadius(FluentPanelCornerRadius::Medium);

// Add content
auto* layout = card->createVerticalLayout();
layout->addWidget(new QLabel("Card content"));
layout->addWidget(new QPushButton("Action"));

// Collapsible panel
auto* collapsiblePanel = new FluentPanel("Settings");
collapsiblePanel->setCollapsible(true);
collapsiblePanel->setExpanded(false); // Start collapsed

// Acrylic panel (translucent)
auto* acrylicPanel = new FluentPanel();
acrylicPanel->setPanelType(FluentPanelType::Acrylic);
acrylicPanel->setElevation(FluentPanelElevation::High);
```

#### Panel Types

- `Card` - Elevated card with shadow
- `Surface` - Flat surface with border
- `Acrylic` - Translucent acrylic effect
- `Mica` - Mica material effect
- `Sidebar` - Navigation sidebar panel
- `Content` - Main content area
- `Dialog` - Modal dialog panel
- `Flyout` - Popup flyout panel

### FluentBreadcrumb

Navigation breadcrumb with multiple styles.

```cpp
#include "FluentQt/Components/FluentBreadcrumb.h"

auto* breadcrumb = new FluentBreadcrumb();

// Add items
breadcrumb->addItem(QIcon(":/home"), "Home");
breadcrumb->addItem("Documents");
breadcrumb->addItem("Projects");
breadcrumb->addItem("MyApp");

// Set current item
breadcrumb->setCurrentIndex(3);

// Style options
breadcrumb->setStyle(FluentBreadcrumbStyle::Pills);
breadcrumb->setSeparator(FluentBreadcrumbSeparator::Arrow);

// Path operations
breadcrumb->setPath("Home/Documents/Projects/MyApp", "/");
QString currentPath = breadcrumb->path("/");

// Navigation handling
connect(breadcrumb, &FluentBreadcrumb::itemClicked, [](int index) {
    qDebug() << "Navigate to index:" << index;
});
```

## Integration Guide

> **📖 For comprehensive integration information:**
> - **[Developer Guide](DEVELOPER_GUIDE.md)** - Complete setup and integration instructions
> - **[Migration Guide](MIGRATION_GUIDE.md)** - Migration from other UI libraries
> - **[API Reference - FluentTheme](API_REFERENCE.md#fluenttheme)** - Complete theming API

### Theme Integration

```cpp
// Apply theme to your application
auto& theme = FluentTheme::instance();
theme.setAccentColor(QColor(0, 120, 215)); // Windows blue

// Listen for theme changes
connect(&theme, &FluentTheme::themeChanged, [=]() {
    updateApplicationColors();
});

// Dark mode support
theme.setDarkMode(true);
```

### Animation Integration

```cpp
// Create animator for your widget
auto* animator = new FluentAnimator(this);

// Animate state changes
connect(button, &QPushButton::clicked, [=]() {
    animator->pressEffect(button);
    // Perform action after animation
});

// Animate widget visibility
auto* fadeIn = animator->fadeIn(widget, 300, FluentEasing::CubicOut);
connect(fadeIn, &QPropertyAnimation::finished, [=]() {
    widget->setFocus();
});
fadeIn->start();
```

### Performance Optimization

```cpp
// Enable hardware acceleration
FluentAnimationConfig config;
config.enableHardwareAcceleration = true;

// Respect system preferences
config.respectReducedMotion = true;

// Monitor performance
auto* monitor = FluentPerformanceMonitor::instance();
monitor->startProfiling("widget_animation");
// ... perform animation
monitor->endProfiling("widget_animation");
```

## Best Practices

### Animation Guidelines

1. **Use appropriate durations:**
   - Micro-interactions: 100-200ms
   - State transitions: 200-300ms
   - Page transitions: 300-500ms

2. **Choose suitable easing:**
   - UI feedback: `CubicOut`
   - Attention-grabbing: `ElasticOut`
   - Smooth transitions: `CubicInOut`

3. **Respect accessibility:**
   - Always check `respectReducedMotion`
   - Provide non-animated alternatives
   - Ensure sufficient contrast ratios

### Component Usage

1. **Form validation:**
   - Validate on input change, not just on submit
   - Provide clear error messages
   - Use appropriate validation types

2. **Layout organization:**
   - Use panels to group related content
   - Apply consistent elevation hierarchy
   - Maintain proper spacing relationships

3. **Navigation patterns:**
   - Use breadcrumbs for hierarchical navigation
   - Provide clear current state indication
   - Support keyboard navigation

### Performance Considerations

1. **Animation performance:**
   - Limit concurrent animations
   - Use hardware acceleration when available
   - Profile animation performance regularly

2. **Memory management:**
   - Clean up animations when widgets are destroyed
   - Avoid memory leaks in custom validators
   - Use smart pointers where appropriate

3. **Responsive design:**
   - Test components at different sizes
   - Ensure proper scaling on high-DPI displays
   - Adapt layouts for different screen sizes

## Quick Reference

### Common Patterns

```cpp
// Standard form setup
auto* form = new FluentPanel("User Information");
auto* layout = form->createVerticalLayout();

auto* nameInput = new FluentTextInput("Full Name");
auto* emailInput = new FluentTextInput("Email Address");
emailInput->setValidationType(FluentTextInputValidation::Email);

auto* agreeCheckBox = new FluentCheckBox("I agree to the terms");

layout->addWidget(nameInput);
layout->addWidget(emailInput);
layout->addWidget(agreeCheckBox);

// Animated feedback
auto* animator = new FluentAnimator(this);
connect(submitButton, &QPushButton::clicked, [=]() {
    if (emailInput->isValid() && agreeCheckBox->isChecked()) {
        animator->pulseEffect(submitButton);
        // Process form
    } else {
        animator->shakeEffect(form);
        // Show validation errors
    }
});
```

## See Also

### Complete Documentation

- **[API Reference](API_REFERENCE.md)** - Comprehensive API documentation with all classes, methods, properties, and signals
- **[Developer Guide](DEVELOPER_GUIDE.md)** - Development environment setup, coding standards, testing, and contribution guidelines
- **[Migration Guide](MIGRATION_GUIDE.md)** - Step-by-step migration from other UI libraries and previous versions

### Individual Component Documentation

- **[FluentButton](components/FluentButton.md)** - Enhanced button component with multiple styles and animations
- **[FluentTextInput](components/FluentTextInput.md)** - Advanced text input with validation and states
- **[FluentPanel](components/FluentPanel.md)** - Versatile container with multiple types and elevation effects

### Guides and Tutorials

- **[Quick Start Guide](../README.md#quick-start)** - Get started with Element Fluent UI in minutes
- **[Build Instructions](../BUILD.md)** - Detailed build and installation instructions
- **[Examples](../examples/)** - Complete example applications demonstrating various features

### External Resources

- **[Microsoft Fluent Design System](https://fluent2.microsoft.design/)** - Official Fluent Design guidelines
- **[Qt6 Documentation](https://doc.qt.io/qt-6/)** - Qt6 framework documentation
- **[C++20 Reference](https://en.cppreference.com/w/cpp/20)** - Modern C++ features used in the library

---

*For the most up-to-date API documentation, see the individual header files and the [complete API reference](API_REFERENCE.md).*
