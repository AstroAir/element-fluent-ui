# FluentQt Enhanced API Documentation

## Overview

This document provides comprehensive API documentation for all FluentQt components, including performance optimizations, accessibility features, responsive design capabilities, and internationalization support.

## Core Systems

### FluentPerformanceMonitor

Enhanced performance monitoring system with comprehensive metrics and optimization hints.

#### Class: `FluentPerformanceMonitor`

**Header:** `FluentQt/Core/FluentPerformance.h`

##### Public Methods

```cpp
// Singleton access
static FluentPerformanceMonitor& instance();

// Frame rate monitoring
void startFrameRateMonitoring();
void stopFrameRateMonitoring();
double currentFrameRate() const;

// Comprehensive metrics
const PerformanceMetrics& currentMetrics() const;

// Component-specific monitoring
void registerComponent(const QString& componentName, QObject* component);
void unregisterComponent(const QString& componentName);
void recordComponentRender(const QString& componentName, std::chrono::milliseconds renderTime);
ComponentPerformanceData getComponentData(const QString& componentName) const;
QList<ComponentPerformanceData> getAllComponentData() const;

// Memory tracking
void trackMemoryUsage();
size_t currentMemoryUsage() const;
size_t peakMemoryUsage() const;
void resetPeakMemoryUsage();

// Benchmarking system
void runBenchmark(const QString& benchmarkName, std::function<void()> benchmark);
QList<BenchmarkResult> getBenchmarkResults() const;
void clearBenchmarkResults();

// Performance optimization hints
bool shouldSkipAnimation() const;
bool shouldReduceEffects() const;
int recommendedAnimationDuration() const;
bool shouldUseLowQualityRendering() const;
int recommendedMaxAnimations() const;

// Performance mode management
void enableLowPerformanceMode(bool enable = true);
bool isLowPerformanceModeEnabled() const;
```

##### Signals

```cpp
void frameRateChanged(double fps);
void performanceWarning(const QString& message);
void performanceCritical(const QString& message);
void metricsUpdated(const PerformanceMetrics& metrics);
void componentPerformanceIssue(const QString& componentName, const QString& issue);
void benchmarkCompleted(const BenchmarkResult& result);
```

##### Usage Example

```cpp
#include "FluentQt/Core/FluentPerformance.h"

// Monitor performance
auto& monitor = FluentPerformanceMonitor::instance();
monitor.startFrameRateMonitoring();

// Register component for monitoring
monitor.registerComponent("MyButton", myButton);

// Profile operations
{
    FLUENT_PROFILE("ButtonClick");
    // ... expensive operation
}

// Check performance hints
if (monitor.shouldSkipAnimation()) {
    // Skip animations for better performance
}

// Run benchmarks
monitor.runBenchmark("WidgetCreation", []() {
    auto* widget = new FluentButton("Test");
    delete widget;
});
```

### FluentAccessibilityManager

Comprehensive accessibility system with WCAG 2.1 compliance.

#### Class: `FluentAccessibilityManager`

**Header:** `FluentQt/Accessibility/FluentAccessibilityManager.h`

##### Public Methods

```cpp
// Singleton access
static FluentAccessibilityManager& instance();

// Global accessibility settings
void setWcagComplianceLevel(WcagLevel level);
WcagLevel wcagComplianceLevel() const;

void setHighContrastMode(bool enabled);
bool isHighContrastMode() const;

void setReducedMotionMode(bool enabled);
bool isReducedMotionMode() const;

void setScreenReaderMode(bool enabled);
bool isScreenReaderMode() const;

// Color and contrast
double calculateContrastRatio(const QColor& foreground, const QColor& background) const;
QColor ensureContrast(const QColor& foreground, const QColor& background, double minRatio) const;

// Accessibility validation
QList<AccessibilityIssue> validateAccessibility(QWidget* rootWidget);
void generateAccessibilityReport(QWidget* rootWidget, const QString& filename);
bool checkWcagCompliance(QWidget* widget, WcagLevel level);

// Announcements
void announceGlobally(const QString& message, LiveRegionType urgency = LiveRegionType::Polite);
```

##### Usage Example

```cpp
#include "FluentQt/Accessibility/FluentAccessibilityManager.h"

auto& accessibility = FluentAccessibilityManager::instance();

// Set compliance level
accessibility.setWcagComplianceLevel(WcagLevel::AA);

// Enable high contrast mode
accessibility.setHighContrastMode(true);

// Validate accessibility
auto issues = accessibility.validateAccessibility(mainWindow);
for (const auto& issue : issues) {
    qDebug() << "Accessibility issue:" << issue.description;
}

// Ensure proper contrast
QColor textColor = accessibility.ensureContrast(Qt::black, backgroundColor, 4.5);

// Make announcements
accessibility.announceGlobally("Form submitted successfully", LiveRegionType::Polite);
```

### FluentResponsiveManager

Responsive design system with mobile-first approach.

#### Class: `FluentResponsiveManager`

**Header:** `FluentQt/Core/FluentResponsive.h`

##### Public Methods

```cpp
// Singleton access
static FluentResponsiveManager& instance();

// Breakpoint management
FluentBreakpoint getCurrentBreakpoint() const;
FluentBreakpoint getBreakpointForSize(const QSize& size) const;

// Device detection
FluentDeviceType getCurrentDeviceType() const;
FluentOrientation getCurrentOrientation() const;
bool isTouchDevice() const;
bool isHighDPI() const;

// Responsive utilities
double getScaleFactor() const;
void setScaleFactor(double factor);
QSize scaleSize(const QSize& size) const;
QFont scaleFont(const QFont& font) const;

// Widget registration
void registerResponsiveWidget(QWidget* widget);
void setWidgetResponsiveProperties(QWidget* widget, const ResponsiveProperties& properties);

// Touch optimization
void enableTouchOptimization(bool enabled);
int getTouchTargetMinSize() const;
```

##### Usage Example

```cpp
#include "FluentQt/Core/FluentResponsive.h"

auto& responsive = FluentResponsiveManager::instance();

// Register widget for responsive behavior
responsive.registerResponsiveWidget(myWidget);

// Set responsive properties
ResponsiveProperties props;
props.sizes[FluentBreakpoint::Small] = QSize(200, 100);
props.sizes[FluentBreakpoint::Large] = QSize(400, 200);
responsive.setWidgetResponsiveProperties(myWidget, props);

// Check device type
if (responsive.getCurrentDeviceType() == FluentDeviceType::Mobile) {
    // Apply mobile-specific styling
}

// Scale for current device
QFont scaledFont = responsive.scaleFont(baseFont);
```

### FluentTranslationManager

Comprehensive internationalization system.

#### Class: `FluentTranslationManager`

**Header:** `FluentQt/Core/FluentI18n.h`

##### Public Methods

```cpp
// Singleton access
static FluentTranslationManager& instance();

// Locale management
void setCurrentLocale(const QLocale& locale);
QLocale getCurrentLocale() const;

// Translation functions
QString translate(const QString& key, const QString& defaultText = "");
QString translatePlural(const QString& key, int count, const QString& defaultSingular, const QString& defaultPlural = "");

// RTL support
FluentTextDirection getTextDirection() const;
bool isRightToLeft() const;

// Formatting utilities
QString formatNumber(double number, FluentNumberFormat format = FluentNumberFormat::Decimal) const;
QString formatCurrency(double amount, const QString& currencyCode = "") const;
QString formatDateTime(const QDateTime& dateTime, FluentDateTimeFormat format = FluentDateTimeFormat::Medium) const;

// Translation management
bool loadTranslations(const QString& directory);
double getTranslationCompleteness(const QLocale& locale) const;
```

##### Usage Example

```cpp
#include "FluentQt/Core/FluentI18n.h"

auto& i18n = FluentTranslationManager::instance();

// Set locale
i18n.setCurrentLocale(QLocale("es_ES"));

// Translate text
QString text = FLUENT_TR("button.save", "Save");
QString pluralText = FLUENT_TR_PLURAL("items.count", count, "%1 item", "%1 items");

// Format numbers and dates
QString price = i18n.formatCurrency(29.99, "USD");
QString date = i18n.formatDateTime(QDateTime::currentDateTime());

// Check RTL
if (i18n.isRightToLeft()) {
    // Apply RTL layout
}
```

## Enhanced Component APIs

### FluentButton (Enhanced)

Extended button component with performance optimizations and accessibility.

#### Additional Properties

```cpp
// Performance properties
Q_PROPERTY(bool enableRenderCache READ isRenderCacheEnabled WRITE setRenderCacheEnabled)
Q_PROPERTY(FluentRenderQuality renderQuality READ renderQuality WRITE setRenderQuality)

// Accessibility properties
Q_PROPERTY(QString ariaLabel READ ariaLabel WRITE setAriaLabel)
Q_PROPERTY(QString ariaDescribedBy READ ariaDescribedBy WRITE setAriaDescribedBy)
Q_PROPERTY(bool highContrastMode READ isHighContrastMode WRITE setHighContrastMode)

// Responsive properties
Q_PROPERTY(bool touchOptimized READ isTouchOptimized WRITE setTouchOptimized)
Q_PROPERTY(FluentBreakpoint currentBreakpoint READ currentBreakpoint NOTIFY breakpointChanged)
```

#### Enhanced Methods

```cpp
// Performance optimization
void setRenderCacheEnabled(bool enabled);
bool isRenderCacheEnabled() const;
void setRenderQuality(FluentRenderQuality quality);
FluentRenderQuality renderQuality() const;

// Accessibility enhancement
void setAriaLabel(const QString& label);
QString ariaLabel() const;
void setAriaDescribedBy(const QString& elementId);
QString ariaDescribedBy() const;
void announceStateChange(const QString& message);

// Responsive behavior
void setTouchOptimized(bool optimized);
bool isTouchOptimized() const;
void setResponsiveProperties(const ResponsiveProperties& properties);

// Internationalization
void setTranslationKey(const QString& key);
QString translationKey() const;
void updateTranslation();
```

## Performance Optimization Guidelines

### Memory Management

1. **Use Memory Pools**: Utilize `FluentMemoryPool` for frequently allocated objects
2. **Smart Pointers**: Use `FluentPoolPtr` for automatic memory management
3. **Cache Management**: Enable render caching for static content

### Rendering Optimization

1. **Dirty Region Tracking**: Use `FluentDirtyRegionTracker` to minimize redraws
2. **Render Quality**: Adjust quality based on performance metrics
3. **Animation Optimization**: Use performance hints to skip animations when needed

### Component Registration

```cpp
// Register components for monitoring
FluentPerformanceMonitor::instance().registerComponent("MyComponent", this);

// Use profiling macros
FLUENT_PROFILE_COMPONENT("MyComponent");

// Track memory usage
FLUENT_TRACK_MEMORY("ComponentCreation");
```

## Accessibility Best Practices

### WCAG Compliance

1. **Color Contrast**: Ensure 4.5:1 ratio for AA compliance
2. **Keyboard Navigation**: Provide keyboard shortcuts and tab order
3. **Screen Reader Support**: Use proper ARIA labels and descriptions
4. **Focus Management**: Implement visible focus indicators

### Implementation

```cpp
// Set accessibility properties
auto* accessible = new FluentEnhancedAccessible(widget);
accessible->setAriaLabel("Save document");
accessible->setAriaDescribedBy("save-help-text");

// Ensure contrast
QColor textColor = FluentAccessibilityManager::instance()
    .ensureContrast(Qt::black, backgroundColor, 4.5);

// Announce changes
accessible->announceToScreenReader("Document saved", LiveRegionType::Polite);
```

## Responsive Design Implementation

### Breakpoint System

- **XSmall**: < 576px (mobile portrait)
- **Small**: 576px - 768px (mobile landscape)
- **Medium**: 768px - 992px (tablets)
- **Large**: 992px - 1200px (desktops)
- **XLarge**: 1200px - 1400px (large desktops)
- **XXLarge**: > 1400px (extra large screens)

### Usage

```cpp
// Register for responsive behavior
FLUENT_RESPONSIVE_WIDGET(myWidget);

// Set breakpoint-specific properties
ResponsiveProperties props;
props.sizes[FluentBreakpoint::Small] = QSize(300, 200);
props.sizes[FluentBreakpoint::Large] = QSize(600, 400);
props.visibility[FluentBreakpoint::XSmall] = false;

FluentResponsiveManager::instance().setWidgetResponsiveProperties(myWidget, props);
```

## Internationalization Implementation

### Translation Keys

Use hierarchical keys for better organization:

```cpp
// Component.feature.element
FLUENT_TR("button.save.text", "Save")
FLUENT_TR("dialog.confirmation.title", "Confirm Action")
FLUENT_TR("form.validation.required", "This field is required")
```

### RTL Support

```cpp
// Check text direction
if (FluentTranslationManager::instance().isRightToLeft()) {
    widget->setLayoutDirection(Qt::RightToLeft);
}

// Mirror layouts
QRect mirroredRect = FluentRTLUtils::mirrorRect(originalRect, containerRect);
```

## Testing and Validation

### Performance Testing

```cpp
// Run performance benchmarks
auto& monitor = FluentPerformanceMonitor::instance();
monitor.runBenchmark("ComponentCreation", []() {
    auto* component = new FluentButton("Test");
    delete component;
});

// Check results
auto results = monitor.getBenchmarkResults();
for (const auto& result : results) {
    qDebug() << result.testName << ":" << result.operationsPerSecond << "ops/sec";
}
```

### Accessibility Testing

```cpp
// Validate accessibility
auto issues = FluentAccessibilityManager::instance()
    .validateAccessibility(rootWidget);

// Generate report
FluentAccessibilityManager::instance()
    .generateAccessibilityReport(rootWidget, "accessibility_report.html");
```

This enhanced API documentation provides comprehensive coverage of all new performance, accessibility, responsive design, and internationalization features added to the FluentQt library.
