# FluentUI Design Token System

## Overview

The FluentUI Design Token System provides a comprehensive implementation of Microsoft's FluentUI design tokens, ensuring consistent styling across all components while maintaining full compatibility with the official FluentUI design system.

## 🎨 Design Token Categories

### Color Tokens

#### Neutral Palette
Complete 28-color neutral palette from white to black:
```cpp
QColor white = FLUENT_NEUTRAL_COLOR(0);      // #ffffff
QColor gray10 = FLUENT_NEUTRAL_COLOR(10);    // #e1e1e1
QColor gray30 = FLUENT_NEUTRAL_COLOR(30);    // #7a7a7a
QColor black = FLUENT_NEUTRAL_COLOR(54);     // #000000
```

#### Brand Palette
16-color brand palette based on Microsoft Blue:
```cpp
QColor brandDark = FLUENT_BRAND_COLOR(10);   // #061724
QColor brandPrimary = FLUENT_BRAND_COLOR(100); // #0d8cf0
QColor brandLight = FLUENT_BRAND_COLOR(160); // #b4d6fa
```

#### Semantic Colors
Context-aware color references:
```cpp
QColor background = FLUENT_COLOR("color.background.primary");
QColor text = FLUENT_COLOR("color.text.primary");
QColor accent = FLUENT_COLOR("color.text.accent");
QColor border = FLUENT_COLOR("color.border.primary");
```

### Typography Tokens

#### Typography Scale
Complete Microsoft FluentUI typography hierarchy:
```cpp
QFont caption = FLUENT_FONT("typography.caption1");     // 12px
QFont body = FLUENT_FONT("typography.body1");           // 14px
QFont subtitle = FLUENT_FONT("typography.subtitle1");   // 16px DemiBold
QFont title = FLUENT_FONT("typography.title1");         // 28px DemiBold
QFont display = FLUENT_FONT("typography.display");      // 68px DemiBold
```

#### Convenience Functions
```cpp
QFont titleFont = FLUENT_TITLE_FONT(1);    // typography.title1
QFont bodyFont = FLUENT_BODY_FONT(false);  // typography.body1
QFont strongFont = FLUENT_BODY_FONT(true); // typography.body1Strong
QFont captionFont = TokenUtils::captionFont(1);
```

### Spacing Tokens

#### 4px Base Unit System
All spacing follows FluentUI's 4px base unit:
```cpp
int xs = FLUENT_SPACING("xs");     // 4px
int s = FLUENT_SPACING("s");       // 8px
int m = FLUENT_SPACING("m");       // 12px
int l = FLUENT_SPACING("l");       // 16px
int xl = FLUENT_SPACING("xl");     // 20px
int xxl = FLUENT_SPACING("xxl");   // 24px
```

#### Component Spacing
```cpp
int buttonPadding = FLUENT_SIZE("spacing.button.padding.horizontal");
int cardPadding = FLUENT_CARD_PADDING("medium");
QMargins margins = FLUENT_MARGINS("card", "large");
```

### Component Tokens

#### Button Tokens
Complete button styling system:
```cpp
// Button sizes
int smallHeight = FLUENT_BUTTON_SIZE("small", "height");     // 24px
int mediumHeight = FLUENT_BUTTON_SIZE("medium", "height");   // 32px
int largeHeight = FLUENT_BUTTON_SIZE("large", "height");     // 40px

// Button colors
QColor primaryBg = FLUENT_BUTTON_COLOR("primary", "rest");
QColor primaryHover = FLUENT_BUTTON_COLOR("primary", "hover");
QColor secondaryBg = FLUENT_BUTTON_COLOR("secondary", "rest");
```

#### Card Tokens
```cpp
int cardPadding = FLUENT_SIZE("card.padding.medium");
QColor cardBackground = FLUENT_COLOR("card.background");
QString cardShadow = TokenUtils::getShadow("card.shadow.rest");
```

#### Input Tokens
```cpp
int inputHeight = FLUENT_INPUT_HEIGHT("medium");
QColor inputBorder = FLUENT_COLOR("input.border.rest");
QColor inputFocus = FLUENT_COLOR("input.border.focus");
```

## 🛠️ Usage Guide

### Basic Token Access

#### Direct Token Access
```cpp
// Using the token utilities
auto& tokenUtils = FluentDesignTokenUtils::instance();
QColor color = tokenUtils.getColor("brand.100");
QFont font = tokenUtils.getFont("typography.body1");
int spacing = tokenUtils.getSpacing("m");
```

#### Convenience Functions
```cpp
// Using namespace functions
using namespace FluentQt::Styling::TokenUtils;
QColor brandColor = brandColor(100);
QFont bodyFont = bodyFont(false);
int mediumSpacing = spacing("m");
```

#### Convenience Macros
```cpp
// Using macros (recommended for most cases)
QColor accent = FLUENT_BRAND_COLOR(100);
QFont title = FLUENT_TITLE_FONT(1);
int padding = FLUENT_SPACING("m");
```

### Component Integration

#### Button Implementation
```cpp
class FluentButton : public QPushButton {
public:
    FluentButton(const QString& text, QWidget* parent = nullptr) 
        : QPushButton(text, parent) {
        
        // Apply design tokens
        setFont(FLUENT_BODY_FONT(false));
        setMinimumHeight(FLUENT_BUTTON_SIZE("medium", "height"));
        
        // Set colors using tokens
        QPalette palette = this->palette();
        palette.setColor(QPalette::Button, FLUENT_BUTTON_COLOR("primary", "rest"));
        palette.setColor(QPalette::ButtonText, FLUENT_COLOR("button.primary.text"));
        setPalette(palette);
        
        // Apply spacing
        int hPadding = FLUENT_BUTTON_SIZE("medium", "padding.horizontal");
        int vPadding = FLUENT_BUTTON_SIZE("medium", "padding.vertical");
        setContentsMargins(hPadding, vPadding, hPadding, vPadding);
    }
};
```

#### Card Implementation
```cpp
class FluentCard : public QFrame {
public:
    FluentCard(QWidget* parent = nullptr) : QFrame(parent) {
        // Apply card tokens
        setFrameStyle(QFrame::Box);
        setLineWidth(FLUENT_SIZE("card.borderWidth"));
        
        // Set background and border colors
        QPalette palette = this->palette();
        palette.setColor(QPalette::Window, FLUENT_COLOR("card.background"));
        setPalette(palette);
        
        // Apply padding
        int padding = FLUENT_SIZE("card.padding.medium");
        setContentsMargins(padding, padding, padding, padding);
        
        // Apply shadow (would require custom painting)
        QString shadow = TokenUtils::getShadow("card.shadow.rest");
    }
};
```

### Custom Tokens

#### Creating Custom Tokens
```cpp
auto& tokenUtils = FluentDesignTokenUtils::instance();

// Add custom color
tokenUtils.setCustomToken("myapp.brand.primary", QColor("#ff5722"), FluentTokenType::Color);

// Add custom spacing
tokenUtils.setCustomToken("myapp.spacing.custom", 18, FluentTokenType::Spacing);

// Use custom tokens
QColor customBrand = FLUENT_COLOR("myapp.brand.primary");
int customSpacing = FLUENT_SIZE("myapp.spacing.custom");
```

#### Custom Component Tokens
```cpp
// Register custom component tokens
tokenUtils.setCustomToken("mywidget.background.rest", "neutral.2", FluentTokenType::Color);
tokenUtils.setCustomToken("mywidget.background.hover", "neutral.4", FluentTokenType::Color);
tokenUtils.setCustomToken("mywidget.padding", "spacing.m", FluentTokenType::Spacing);

// Use in component
class MyWidget : public QWidget {
public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        QPalette palette = this->palette();
        palette.setColor(QPalette::Window, FLUENT_COLOR("mywidget.background.rest"));
        setPalette(palette);
        
        int padding = FLUENT_SIZE("mywidget.padding");
        setContentsMargins(padding, padding, padding, padding);
    }
};
```

### Responsive Design

#### Breakpoint-Aware Tokens
```cpp
// Get responsive token based on viewport size
QSize viewportSize = QApplication::primaryScreen()->size();
QVariant responsiveValue = tokenUtils.getResponsiveToken("spacing.container", viewportSize);

// Check breakpoint
bool isLargeScreen = tokenUtils.isBreakpointActive("large", viewportSize);
if (isLargeScreen) {
    // Use large screen layout
}
```

#### Responsive Component Sizing
```cpp
class ResponsiveCard : public FluentCard {
public:
    ResponsiveCard(QWidget* parent = nullptr) : FluentCard(parent) {
        updateForViewport();
    }
    
protected:
    void resizeEvent(QResizeEvent* event) override {
        FluentCard::resizeEvent(event);
        updateForViewport();
    }
    
private:
    void updateForViewport() {
        QSize viewportSize = size();
        auto& tokenUtils = FluentDesignTokenUtils::instance();
        
        // Adjust padding based on screen size
        QString paddingSize = "medium";
        if (tokenUtils.isBreakpointActive("small", viewportSize)) {
            paddingSize = "small";
        } else if (tokenUtils.isBreakpointActive("large", viewportSize)) {
            paddingSize = "large";
        }
        
        int padding = FLUENT_SIZE(QString("card.padding.%1").arg(paddingSize));
        setContentsMargins(padding, padding, padding, padding);
    }
};
```

## 🎯 Best Practices

### Token Usage Guidelines

1. **Always Use Tokens**: Never hardcode colors, sizes, or fonts
```cpp
// ❌ Bad
setStyleSheet("color: #0078d4; font-size: 14px; padding: 8px;");

// ✅ Good
setFont(FLUENT_BODY_FONT(false));
QPalette palette = this->palette();
palette.setColor(QPalette::WindowText, FLUENT_BRAND_COLOR(100));
setPalette(palette);
setContentsMargins(FLUENT_SPACING("s"), FLUENT_SPACING("s"), 
                   FLUENT_SPACING("s"), FLUENT_SPACING("s"));
```

2. **Use Semantic Tokens**: Prefer semantic over direct tokens
```cpp
// ❌ Less flexible
QColor textColor = FLUENT_NEUTRAL_COLOR(54);

// ✅ More flexible and theme-aware
QColor textColor = FLUENT_COLOR("color.text.primary");
```

3. **Component-Specific Tokens**: Use component tokens for consistency
```cpp
// ❌ Generic
int padding = FLUENT_SPACING("m");

// ✅ Component-specific
int padding = FLUENT_SIZE("button.size.medium.padding.horizontal");
```

### Performance Optimization

1. **Cache Token Values**: For frequently accessed tokens
```cpp
class OptimizedComponent : public QWidget {
private:
    // Cache frequently used tokens
    static QColor s_primaryColor;
    static QFont s_bodyFont;
    static int s_standardPadding;
    
public:
    static void initializeTokens() {
        s_primaryColor = FLUENT_COLOR("color.text.primary");
        s_bodyFont = FLUENT_BODY_FONT(false);
        s_standardPadding = FLUENT_SPACING("m");
    }
};
```

2. **Batch Token Access**: When accessing multiple tokens
```cpp
// Get all component tokens at once
QHash<QString, QVariant> buttonTokens = tokenUtils.getComponentTokens("button");
```

### Accessibility Compliance

1. **Validate Color Contrast**: Ensure accessible color combinations
```cpp
bool isAccessible = tokenUtils.validateColorContrast(
    "color.text.primary", 
    "color.background.primary", 
    4.5  // WCAG AA requirement
);

if (!isAccessible) {
    QColor accessibleColor = tokenUtils.getAccessibleColor(
        "color.text.primary", 
        "color.background.primary"
    );
}
```

2. **Use High Contrast Tokens**: For accessibility modes
```cpp
// Automatically use high contrast colors when system setting is enabled
QColor textColor = tokenUtils.getThemeAwareToken("color.text.primary", currentThemeMode);
```

## 🧪 Testing and Validation

### Token Validation
```cpp
// Validate token exists
FLUENT_VALIDATE_TOKEN("brand.100");

// Check token type
bool isColorToken = tokenUtils.isTokenValid("brand.100");

// Get token description
QString description = tokenUtils.getTokenDescription("typography.body1");
```

### Performance Testing
```cpp
// Measure token resolution performance
QElapsedTimer timer;
timer.start();

for (int i = 0; i < 1000; ++i) {
    QColor color = FLUENT_BRAND_COLOR(100);
}

qint64 elapsed = timer.elapsed();
qDebug() << "1000 token resolutions took" << elapsed << "ms";
```

## 🚀 Advanced Features

### Theme Integration
The design token system automatically integrates with the theme system:
- Tokens update automatically when themes change
- Support for light/dark/high-contrast themes
- Custom theme token overrides

### Performance Optimization
- Intelligent token caching for frequently accessed tokens
- Batch token resolution for component initialization
- Minimal memory footprint with lazy loading

### Extensibility
- Custom token registration system
- Component-specific token namespaces
- Plugin architecture for token extensions

This comprehensive design token system ensures consistent, accessible, and maintainable styling across all FluentUI components while providing the flexibility needed for custom implementations.
