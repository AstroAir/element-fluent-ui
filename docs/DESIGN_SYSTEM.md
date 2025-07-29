# FluentQt Visual Design System

## 🎨 Design Philosophy

FluentQt follows Microsoft's Fluent Design principles while adapting them for Qt applications. Our design system emphasizes:

- **Light**: Use of light, shadow, and depth to create hierarchy
- **Depth**: Layered interfaces that feel natural and intuitive  
- **Motion**: Purposeful animations that guide user attention
- **Material**: Surfaces that feel tactile and responsive
- **Scale**: Adaptive design that works across all devices

## 🎯 Design Tokens

### Color System

#### Primary Colors
```cpp
// Brand Colors
FluentColor::Primary50   = #E3F2FD    // Lightest
FluentColor::Primary100  = #BBDEFB
FluentColor::Primary200  = #90CAF9
FluentColor::Primary300  = #64B5F6
FluentColor::Primary400  = #42A5F5
FluentColor::Primary500  = #2196F3    // Base Primary
FluentColor::Primary600  = #1E88E5
FluentColor::Primary700  = #1976D2
FluentColor::Primary800  = #1565C0
FluentColor::Primary900  = #0D47A1    // Darkest
```

#### Semantic Colors
```cpp
// Status Colors
FluentColor::Success     = #4CAF50    // Green
FluentColor::Warning     = #FF9800    // Orange  
FluentColor::Error       = #F44336    // Red
FluentColor::Info        = #2196F3    // Blue

// Neutral Colors
FluentColor::Gray50      = #FAFAFA
FluentColor::Gray100     = #F5F5F5
FluentColor::Gray200     = #EEEEEE
FluentColor::Gray300     = #E0E0E0
FluentColor::Gray400     = #BDBDBD
FluentColor::Gray500     = #9E9E9E    // Base Gray
FluentColor::Gray600     = #757575
FluentColor::Gray700     = #616161
FluentColor::Gray800     = #424242
FluentColor::Gray900     = #212121
```

#### Theme-Specific Colors
```cpp
// Light Theme
FluentColor::Light::Background       = #FFFFFF
FluentColor::Light::Surface          = #F8F9FA
FluentColor::Light::OnBackground     = #212121
FluentColor::Light::OnSurface        = #424242

// Dark Theme  
FluentColor::Dark::Background        = #121212
FluentColor::Dark::Surface           = #1E1E1E
FluentColor::Dark::OnBackground      = #FFFFFF
FluentColor::Dark::OnSurface         = #E0E0E0

// High Contrast Theme
FluentColor::HighContrast::Background = #000000
FluentColor::HighContrast::Surface    = #000000
FluentColor::HighContrast::OnBackground = #FFFFFF
FluentColor::HighContrast::OnSurface  = #FFFFFF
```

### Typography Scale

#### Font Families
```cpp
// Primary Font Stack
FluentFont::Primary = "Segoe UI, system-ui, -apple-system, sans-serif"

// Monospace Font Stack  
FluentFont::Monospace = "Consolas, 'SF Mono', Monaco, monospace"

// Display Font Stack
FluentFont::Display = "Segoe UI Light, system-ui, sans-serif"
```

#### Type Scale
```cpp
// Heading Styles
FluentTypography::Display1    = 57px / 64px (3.5rem)
FluentTypography::Display2    = 45px / 52px (2.8rem)  
FluentTypography::Display3    = 36px / 44px (2.25rem)

FluentTypography::Headline1   = 32px / 40px (2rem)
FluentTypography::Headline2   = 28px / 36px (1.75rem)
FluentTypography::Headline3   = 24px / 32px (1.5rem)
FluentTypography::Headline4   = 20px / 28px (1.25rem)
FluentTypography::Headline5   = 18px / 24px (1.125rem)
FluentTypography::Headline6   = 16px / 24px (1rem)

// Body Styles
FluentTypography::Body1       = 16px / 24px (1rem)
FluentTypography::Body2       = 14px / 20px (0.875rem)

// Utility Styles
FluentTypography::Caption     = 12px / 16px (0.75rem)
FluentTypography::Overline    = 10px / 16px (0.625rem)
```

#### Font Weights
```cpp
FluentFontWeight::Light       = 300
FluentFontWeight::Regular     = 400
FluentFontWeight::Medium      = 500
FluentFontWeight::SemiBold    = 600
FluentFontWeight::Bold        = 700
```

### Spacing System

#### Base Unit: 8px
```cpp
// Spacing Scale (8px base unit)
FluentSpacing::XS    = 4px   (0.25rem)
FluentSpacing::SM    = 8px   (0.5rem)
FluentSpacing::MD    = 16px  (1rem)
FluentSpacing::LG    = 24px  (1.5rem)
FluentSpacing::XL    = 32px  (2rem)
FluentSpacing::XXL   = 48px  (3rem)
FluentSpacing::XXXL  = 64px  (4rem)

// Component-Specific Spacing
FluentSpacing::Button::Padding     = 12px 24px
FluentSpacing::Card::Padding       = 24px
FluentSpacing::Dialog::Padding     = 32px
FluentSpacing::Form::FieldSpacing  = 16px
```

### Elevation & Shadows

#### Shadow Levels
```cpp
// Elevation Shadows
FluentShadow::Level0 = "none"
FluentShadow::Level1 = "0 1px 3px rgba(0,0,0,0.12), 0 1px 2px rgba(0,0,0,0.24)"
FluentShadow::Level2 = "0 3px 6px rgba(0,0,0,0.16), 0 3px 6px rgba(0,0,0,0.23)"
FluentShadow::Level3 = "0 10px 20px rgba(0,0,0,0.19), 0 6px 6px rgba(0,0,0,0.23)"
FluentShadow::Level4 = "0 14px 28px rgba(0,0,0,0.25), 0 10px 10px rgba(0,0,0,0.22)"
FluentShadow::Level5 = "0 19px 38px rgba(0,0,0,0.30), 0 15px 12px rgba(0,0,0,0.22)"

// Component Shadow Mapping
FluentButton::Shadow      = FluentShadow::Level1
FluentCard::Shadow        = FluentShadow::Level2  
FluentDialog::Shadow      = FluentShadow::Level4
FluentTooltip::Shadow     = FluentShadow::Level3
```

### Border Radius

#### Radius Scale
```cpp
FluentRadius::None    = 0px
FluentRadius::XS      = 2px
FluentRadius::SM      = 4px
FluentRadius::MD      = 6px    // Default
FluentRadius::LG      = 8px
FluentRadius::XL      = 12px
FluentRadius::XXL     = 16px
FluentRadius::Round   = 50%    // Circular
```

## 🧩 Component Specifications

### Buttons

#### Primary Button
```cpp
FluentButton::Primary {
    background: FluentColor::Primary500
    color: #FFFFFF
    padding: 12px 24px
    border-radius: FluentRadius::MD
    font-weight: FluentFontWeight::Medium
    box-shadow: FluentShadow::Level1
    
    // States
    hover: {
        background: FluentColor::Primary600
        box-shadow: FluentShadow::Level2
        transform: translateY(-1px)
    }
    
    active: {
        background: FluentColor::Primary700
        box-shadow: FluentShadow::Level1
        transform: translateY(0)
    }
    
    disabled: {
        background: FluentColor::Gray300
        color: FluentColor::Gray500
        box-shadow: none
    }
}
```

#### Secondary Button
```cpp
FluentButton::Secondary {
    background: transparent
    color: FluentColor::Primary500
    border: 1px solid FluentColor::Primary500
    padding: 12px 24px
    border-radius: FluentRadius::MD
    
    hover: {
        background: FluentColor::Primary50
        border-color: FluentColor::Primary600
    }
}
```

### Cards

#### Standard Card
```cpp
FluentCard {
    background: FluentColor::Light::Surface
    border-radius: FluentRadius::LG
    box-shadow: FluentShadow::Level2
    padding: FluentSpacing::Card::Padding
    border: 1px solid FluentColor::Gray200
    
    hover: {
        box-shadow: FluentShadow::Level3
        transform: translateY(-2px)
    }
}
```

### Form Controls

#### Text Input
```cpp
FluentTextBox {
    background: FluentColor::Light::Surface
    border: 2px solid FluentColor::Gray300
    border-radius: FluentRadius::MD
    padding: 12px 16px
    font-size: FluentTypography::Body1
    
    focus: {
        border-color: FluentColor::Primary500
        box-shadow: 0 0 0 3px rgba(33, 150, 243, 0.1)
    }
    
    error: {
        border-color: FluentColor::Error
        box-shadow: 0 0 0 3px rgba(244, 67, 54, 0.1)
    }
}
```

## 📐 Layout Principles

### Grid System

#### Breakpoints
```cpp
FluentBreakpoint::XS     = 0px      // Mobile portrait
FluentBreakpoint::SM     = 576px    // Mobile landscape  
FluentBreakpoint::MD     = 768px    // Tablet
FluentBreakpoint::LG     = 992px    // Desktop
FluentBreakpoint::XL     = 1200px   // Large desktop
FluentBreakpoint::XXL    = 1400px   // Extra large
```

#### Container Widths
```cpp
FluentContainer::XS      = 100%
FluentContainer::SM      = 540px
FluentContainer::MD      = 720px
FluentContainer::LG      = 960px
FluentContainer::XL      = 1140px
FluentContainer::XXL     = 1320px
```

#### Grid Columns
```cpp
// 12-column grid system
FluentGrid::Columns      = 12
FluentGrid::GutterWidth  = 24px
FluentGrid::MarginWidth  = 16px
```

### Spacing Guidelines

#### Component Spacing
```cpp
// Vertical rhythm
FluentLayout::VerticalRhythm     = 24px

// Section spacing
FluentLayout::SectionSpacing     = 48px

// Component margins
FluentLayout::ComponentMargin    = 16px

// Form field spacing
FluentLayout::FieldSpacing       = 16px
```

## 🎭 Animation System

### Easing Functions
```cpp
// Microsoft Fluent Design easing curves
FluentEasing::Standard    = cubic-bezier(0.4, 0.0, 0.2, 1)    // General purpose
FluentEasing::Decelerate  = cubic-bezier(0.0, 0.0, 0.2, 1)    // Entrances
FluentEasing::Accelerate  = cubic-bezier(0.4, 0.0, 1, 1)      // Exits
FluentEasing::Sharp       = cubic-bezier(0.4, 0.0, 0.6, 1)    // Quick interactions

// Fluent-specific curves (optimized for perceived performance)
FluentEasing::Entrance    = cubic-bezier(0.1, 0.9, 0.2, 1)    // Smooth entrances
FluentEasing::Exit        = cubic-bezier(0.7, 0.0, 0.84, 0.0) // Quick exits
FluentEasing::Bounce      = cubic-bezier(0.68, -0.55, 0.265, 1.55) // Playful feedback
FluentEasing::Elastic     = cubic-bezier(0.175, 0.885, 0.32, 1.275) // Spring-like

// Micro-interaction curves
FluentEasing::Hover       = cubic-bezier(0.25, 0.46, 0.45, 0.94)    // Subtle hover
FluentEasing::Press       = cubic-bezier(0.55, 0.055, 0.675, 0.19)  // Immediate press
FluentEasing::Focus       = cubic-bezier(0.23, 1, 0.32, 1)          // Smooth focus
```

### Duration Scale
```cpp
// Animation durations (Microsoft Fluent Design standards)
FluentDuration::Fast      = 150ms   // Micro-interactions (hover, focus)
FluentDuration::Medium    = 300ms   // Standard transitions (state changes)
FluentDuration::Slow      = 500ms   // Complex animations (page transitions)
FluentDuration::Slower    = 700ms   // Large content transitions

// Micro-interaction durations (optimized for performance)
FluentButton::HoverDuration    = 150ms  // Quick hover feedback
FluentButton::PressDuration    = 100ms  // Immediate press response
FluentButton::RippleDuration   = 400ms  // Ripple effect expansion
FluentCard::HoverDuration      = 200ms  // Subtle card elevation
FluentDialog::EnterDuration    = 300ms  // Modal appearance
FluentDialog::ExitDuration     = 200ms  // Modal dismissal
FluentTooltip::ShowDuration    = 150ms  // Tooltip fade-in
FluentTooltip::HideDuration    = 100ms  // Tooltip fade-out
```

### Motion Principles
```cpp
// Micro-interactions
FluentMotion::Hover {
    scale: 1.02
    duration: FluentDuration::Fast
    easing: FluentEasing::Standard
}

FluentMotion::Press {
    scale: 0.98
    duration: 100ms
    easing: FluentEasing::Sharp
}

// Page transitions
FluentMotion::SlideIn {
    transform: translateX(100%)
    duration: FluentDuration::Medium
    easing: FluentEasing::Decelerate
}
```

## 🌓 Theme Variations

### Light Theme
```cpp
FluentTheme::Light {
    primary: FluentColor::Primary500
    background: #FFFFFF
    surface: #F8F9FA
    onBackground: #212121
    onSurface: #424242
    shadow: rgba(0, 0, 0, 0.1)
}
```

### Dark Theme
```cpp
FluentTheme::Dark {
    primary: FluentColor::Primary300
    background: #121212
    surface: #1E1E1E
    onBackground: #FFFFFF
    onSurface: #E0E0E0
    shadow: rgba(0, 0, 0, 0.3)
}
```

### High Contrast Theme
```cpp
FluentTheme::HighContrast {
    primary: #FFFF00
    background: #000000
    surface: #000000
    onBackground: #FFFFFF
    onSurface: #FFFFFF
    border: #FFFFFF
    shadow: none
}
```

## 📱 Responsive Design

### Touch Targets
```cpp
// Minimum touch target sizes
FluentTouch::MinimumSize     = 44px
FluentTouch::RecommendedSize = 48px
FluentTouch::ComfortableSize = 56px

// Touch spacing
FluentTouch::MinimumSpacing  = 8px
FluentTouch::RecommendedSpacing = 12px
```

### Adaptive Sizing
```cpp
// Component scaling factors
FluentScale::Mobile    = 0.9
FluentScale::Tablet    = 1.0
FluentScale::Desktop   = 1.0
FluentScale::Large     = 1.1
```

## ♿ Accessibility Standards

### Color Contrast
```cpp
// WCAG 2.1 contrast ratios
FluentContrast::AA_Normal    = 4.5:1
FluentContrast::AA_Large     = 3:1
FluentContrast::AAA_Normal   = 7:1
FluentContrast::AAA_Large    = 4.5:1
```

### Focus Indicators
```cpp
FluentFocus::Outline {
    color: FluentColor::Primary500
    width: 2px
    style: solid
    offset: 2px
    border-radius: FluentRadius::SM
}
```

## 🛠️ Implementation

### Using Design Tokens
```cpp
// In C++
auto button = new FluentButton("Save", this);
button->setBackgroundColor(FLUENT_COLOR_TOKEN("primary.500"));
button->setFont(FLUENT_FONT_TOKEN("body.medium"));
button->setPadding(FLUENT_SIZE_TOKEN("spacing.md"));

// In QML
FluentButton {
    text: "Save"
    backgroundColor: FluentTokens.color.primary[500]
    font: FluentTokens.typography.body.medium
    padding: FluentTokens.spacing.md
}
```

### Custom Themes
```cpp
// Create custom theme
FluentThemeBuilder builder("MyCustomTheme");
builder.setMode(FluentThemeMode::Light)
       .addColorToken("primary", QColor("#FF6B35"))
       .addSizeToken("button.padding", 16)
       .addFontToken("heading", QFont("Arial", 18, QFont::Bold));

// Register and apply
builder.registerTheme();
FluentAdvancedThemeManager::instance().setCurrentTheme("MyCustomTheme");
```

This design system ensures consistency, accessibility, and scalability across all FluentQt components while maintaining the distinctive Fluent Design aesthetic.
