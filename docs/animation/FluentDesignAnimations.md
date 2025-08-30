# Fluent Design Animation System

The FluentQt animation system has been enhanced to fully align with Microsoft's Fluent Design System specifications, providing smooth, purposeful, and accessible animations that feel natural and responsive.

## Overview

The enhanced animation system includes:

- **Fluent Design Easing Curves**: Authentic Microsoft Fluent Design timing functions
- **Motion Hierarchy**: Primary, secondary, and utility motion levels
- **Component-Specific Animations**: Optimized animations for buttons, cards, navigation, etc.
- **Performance Optimizations**: Hardware acceleration, 60fps targeting, and accessibility support
- **Connected Animations**: Seamless transitions between related elements
- **Entrance/Exit Animations**: Proper element lifecycle animations

## Fluent Design Easing Curves

### Core Fluent Curves

```cpp
#include "FluentQt/Animation/FluentAnimator.h"

using namespace FluentQt::Animation;

// Fluent Design standard curve (0.8, 0.0, 0.2, 1.0)
FluentAnimationConfig config;
config.easing = FluentEasing::FluentStandard;

// Fluent Design emphasized curve (0.3, 0.0, 0.8, 0.15)
config.easing = FluentEasing::FluentEmphasized;

// Fluent Design decelerate curve (0.1, 0.9, 0.2, 1.0)
config.easing = FluentEasing::FluentDecelerate;
```

### Component-Specific Curves

```cpp
// Button interactions (0.1, 0.9, 0.2, 1.0)
config.easing = FluentEasing::FluentButton;

// Card animations (0.25, 0.46, 0.45, 0.94)
config.easing = FluentEasing::FluentCard;

// Navigation transitions (0.4, 0.0, 0.2, 1.0)
config.easing = FluentEasing::FluentNavigation;

// Dialog transitions (0.0, 0.0, 0.2, 1.0)
config.easing = FluentEasing::FluentDialog;

// Reveal animations (0.0, 0.0, 0.2, 1.0)
config.easing = FluentEasing::FluentReveal;
```

## Motion Hierarchy

Fluent Design defines three levels of motion hierarchy:

### Primary Motion (250ms)
For main content transitions and important UI changes:

```cpp
auto animation = FluentAnimator::primaryMotion(widget);
animation->start();

// Or using config
FluentAnimationConfig config = FluentAnimationConfig::contentTransition();
auto fadeAnimation = FluentAnimator::fadeIn(widget, config);
```

### Secondary Motion (150ms)
For supporting element transitions:

```cpp
auto animation = FluentAnimator::secondaryMotion(widget);
animation->start();

// Or using config
FluentAnimationConfig config = FluentAnimationConfig::uiTransition();
auto slideAnimation = FluentAnimator::slideIn(widget, QPoint(0, 20), config);
```

### Utility Motion (100ms)
For micro-interactions and immediate feedback:

```cpp
auto animation = FluentAnimator::utilityMotion(widget);
animation->start();

// Or using config
FluentAnimationConfig config = FluentAnimationConfig::microInteraction();
auto pressAnimation = FluentAnimator::pressEffect(button, config);
```

## Animation Presets

### Button Interactions

```cpp
// Quick button press feedback
FluentAnimationConfig buttonConfig = FluentAnimationConfig::buttonInteraction();
auto pressAnim = FluentAnimator::pressEffect(button, buttonConfig);
pressAnim->start();

// Button hover effect
auto hoverAnim = FluentAnimator::hoverEffect(button, buttonConfig);
connect(button, &QPushButton::entered, [=]() { hoverAnim->start(); });
```

### Card Animations

```cpp
// Card hover elevation change
FluentAnimationConfig cardConfig = FluentAnimationConfig::cardHover();
auto elevationAnim = FluentAnimator::scaleIn(card, cardConfig);

// Card entrance animation
auto entranceAnim = FluentAnimator::entranceAnimation(card, cardConfig);
entranceAnim->start();
```

### Navigation Transitions

```cpp
// Page transition
FluentAnimationConfig navConfig = FluentAnimationConfig::navigationTransition();
auto transition = FluentAnimator::navigationTransition(fromPage, toPage, navConfig);
transition->start();

// Connected element animation
auto connectedAnim = FluentAnimator::connectedElementAnimation(
    fromElement, toElement, fromContainer, toContainer, navConfig);
connectedAnim->start();
```

## Performance Optimizations

### Hardware Acceleration

```cpp
FluentAnimationConfig config;
config.useHardwareAcceleration = true;  // Enable GPU acceleration
config.hierarchy = FluentAnimationConfig::MotionHierarchy::Utility;  // High priority

auto animation = FluentAnimator::fadeIn(widget, config);
```

### Accessibility Support

```cpp
FluentAnimationConfig config;
config.respectReducedMotion = true;  // Automatically respects system settings

// The animation will be disabled or shortened based on:
// - System accessibility settings
// - User preferences
// - Device performance
auto animation = FluentAnimator::slideIn(widget, QPoint(0, 20), config);
```

### 60fps Optimization

The animation system automatically optimizes for 60fps performance:

- Micro-interactions run at 120fps for maximum responsiveness
- Primary and secondary animations target 60fps
- Hardware acceleration is used when available
- CPU fallback maintains 30fps minimum

## Connected Animations

Connected animations create seamless transitions between related elements:

```cpp
// Simple connected animation
auto connectedAnim = FluentAnimator::connectedAnimation(fromWidget, toWidget);
connectedAnim->start();

// Advanced connected element animation with shared element transition
auto elementAnim = FluentAnimator::connectedElementAnimation(
    sourceButton,      // Element in source view
    targetButton,      // Element in target view  
    sourceContainer,   // Source view container
    targetContainer,   // Target view container
    FluentAnimationConfig::navigationTransition()
);
elementAnim->start();
```

## Entrance and Exit Animations

### Entrance Animations

```cpp
// Fluent Design entrance: fade + slide up + scale
auto entrance = FluentAnimator::entranceAnimation(widget);
entrance->start();

// Custom entrance with specific config
FluentAnimationConfig config = FluentAnimationConfig::dialogTransition();
auto customEntrance = FluentAnimator::entranceAnimation(dialog, config);
```

### Exit Animations

```cpp
// Fluent Design exit: fade + slide down + scale
auto exit = FluentAnimator::exitAnimation(widget);
exit->start();

// Faster exit animation
FluentAnimationConfig config;
config.duration = 150ms;
config.easing = FluentEasing::FluentAccelerate;
auto fastExit = FluentAnimator::exitAnimation(widget, config);
```

## Theme Transitions

Enhanced theme switching with coordinated animations:

```cpp
auto& theme = FluentQt::Styling::FluentTheme::instance();

// Theme changes now automatically trigger smooth transitions
theme.setMode(FluentThemeMode::Dark);  // Animated transition

// The system automatically:
// - Staggers widget animations for visual hierarchy
// - Uses Fluent emphasized curves
// - Maintains 60fps performance
// - Respects reduced motion preferences
```

## Best Practices

### 1. Use Appropriate Motion Hierarchy

```cpp
// ✅ Good: Use utility motion for micro-interactions
auto buttonPress = FluentAnimator::utilityMotion(button);

// ✅ Good: Use primary motion for main content
auto pageTransition = FluentAnimator::primaryMotion(mainContent);

// ❌ Avoid: Using primary motion for micro-interactions
// auto buttonPress = FluentAnimator::primaryMotion(button);  // Too slow
```

### 2. Respect Accessibility

```cpp
// ✅ Always enable reduced motion support
FluentAnimationConfig config;
config.respectReducedMotion = true;

// ✅ Use appropriate durations
config.duration = 150ms;  // Good for UI transitions

// ❌ Avoid very long animations
// config.duration = 2000ms;  // Too long for UI
```

### 3. Use Hardware Acceleration Wisely

```cpp
// ✅ Enable for smooth animations
config.useHardwareAcceleration = true;

// ✅ Use motion hierarchy for performance optimization
config.hierarchy = FluentAnimationConfig::MotionHierarchy::Utility;
```

### 4. Clean Up Animations

```cpp
// ✅ Animations are automatically cleaned up when widgets are destroyed
// ✅ Use the animation manager for automatic lifecycle management
FluentAnimationManager::instance().registerAnimation(animation.get());

// ✅ Or use RAII with smart pointers
auto animation = FluentAnimator::fadeIn(widget);
// Automatically cleaned up when animation goes out of scope
```

## Migration Guide

### From Basic Qt Animations

```cpp
// Old Qt animation
QPropertyAnimation* oldAnim = new QPropertyAnimation(widget, "opacity");
oldAnim->setDuration(200);
oldAnim->setEasingCurve(QEasingCurve::OutCubic);
oldAnim->setStartValue(0.0);
oldAnim->setEndValue(1.0);
oldAnim->start();

// New Fluent Design animation
auto newAnim = FluentAnimator::fadeIn(widget, 
    FluentAnimationConfig::uiTransition());
newAnim->start();
```

### Backward Compatibility

All existing FluentQt animation APIs remain functional. New Fluent Design features are additive and don't break existing code.

## Examples

See the `examples/` directory for complete working examples:

- `FluentAnimationShowcase.cpp` - Comprehensive animation demonstrations
- `FluentButtonAnimations.cpp` - Button-specific animation examples  
- `FluentNavigationAnimations.cpp` - Navigation transition examples
- `FluentThemeAnimations.cpp` - Theme switching animations
