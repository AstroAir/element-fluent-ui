// include/FluentQt/Core/FluentResponsive.h
#pragma once

#include <QLayout>
#include <QMutex>
#include <QObject>
#include <QRect>
#include <QScreen>
#include <QSize>
#include <QTimer>
#include <QWidget>
#include <functional>
#include <memory>
#include <unordered_map>

namespace FluentQt::Core {

// Enhanced responsive breakpoints following modern design systems
enum class FluentBreakpoint {
    XSmall,     // < 576px (mobile portrait)
    Small,      // 576px - 768px (mobile landscape, small tablets)
    Medium,     // 768px - 992px (tablets)
    Large,      // 992px - 1200px (small desktops)
    XLarge,     // 1200px - 1400px (large desktops)
    XXLarge,    // 1400px - 1920px (extra large screens)
    UltraWide,  // 1920px - 2560px (ultrawide monitors)
    FourK       // > 2560px (4K and beyond)
};

// Responsive behavior modes
enum class FluentResponsiveMode {
    Adaptive,   // Discrete breakpoint changes
    Fluid,      // Continuous scaling
    Hybrid,     // Combination of adaptive and fluid
    Container,  // Based on container size
    Content     // Based on content requirements
};

// Device types for responsive behavior
enum class FluentDeviceType { Mobile, Tablet, Desktop, TV, Unknown };

// Orientation states
enum class FluentOrientation { Portrait, Landscape, Square };

// Enhanced responsive configuration
struct ResponsiveConfig {
    QSize minSize{320, 240};    // Minimum supported size
    QSize maxSize{7680, 4320};  // Maximum supported size (8K)
    bool enableTouchOptimization{true};
    bool enableMobileFirst{true};
    bool enableFluidLayouts{true};
    bool enableAdaptiveSpacing{true};
    bool enableScalableText{true};
    bool enableFluidTypography{true};
    bool enableContainerQueries{true};
    bool enableAccessibilityScaling{true};
    bool enableReducedMotionRespect{true};
    bool enableHighContrastAdaptation{true};
    FluentResponsiveMode mode{FluentResponsiveMode::Hybrid};
    double minScaleFactor{0.8};
    double maxScaleFactor{3.0};
    double fluidScalingFactor{1.0};
    int animationDuration{200};  // ms for responsive transitions
    QString customBreakpoints;   // JSON string for custom breakpoints
};

// Fluid typography configuration
struct FluidTypographyConfig {
    double minFontSize{12.0};         // Minimum font size in px
    double maxFontSize{72.0};         // Maximum font size in px
    double minViewportWidth{320.0};   // Minimum viewport width in px
    double maxViewportWidth{1920.0};  // Maximum viewport width in px
    double scalingFactor{1.2};        // Typography scaling ratio
    bool enableLineHeightScaling{true};
    bool enableLetterSpacingScaling{true};
    bool enableAccessibilityScaling{true};
    double minLineHeight{1.2};
    double maxLineHeight{1.8};
    double baseLineHeight{1.5};
};

// Container query configuration
struct ContainerQueryConfig {
    bool enabled{true};
    QSize minContainerSize{200, 100};
    QSize maxContainerSize{2000, 1500};
    bool enableNestedQueries{true};
    bool enableAspectRatioQueries{true};
    bool enableOrientationQueries{true};
};

// Enhanced responsive properties for widgets
struct ResponsiveProperties {
    // Basic responsive properties
    std::unordered_map<FluentBreakpoint, QSize> sizes;
    std::unordered_map<FluentBreakpoint, QMargins> margins;
    std::unordered_map<FluentBreakpoint, int> spacing;
    std::unordered_map<FluentBreakpoint, bool> visibility;
    std::unordered_map<FluentBreakpoint, QString> styleSheet;
    std::unordered_map<FluentBreakpoint, QFont> fonts;

    // Advanced responsive properties
    std::unordered_map<FluentBreakpoint, QSize> minSizes;
    std::unordered_map<FluentBreakpoint, QSize> maxSizes;
    std::unordered_map<FluentBreakpoint, Qt::Alignment> alignment;
    std::unordered_map<FluentBreakpoint, int> zIndex;
    std::unordered_map<FluentBreakpoint, double> opacity;
    std::unordered_map<FluentBreakpoint, QString> className;

    // Fluid properties
    bool isFluid{true};
    bool enableFluidTypography{true};
    bool enableFluidSpacing{true};
    bool enableFluidSizing{true};
    double aspectRatio{0.0};  // 0 = no constraint
    double fluidScalingFactor{1.0};

    // Container query properties
    bool enableContainerQueries{false};
    ContainerQueryConfig containerConfig;

    // Accessibility properties
    bool respectReducedMotion{true};
    bool respectHighContrast{true};
    bool respectForcedColors{true};
    bool enableAccessibilityScaling{true};
    double accessibilityScaleFactor{1.0};

    // Animation properties
    int transitionDuration{200};  // ms
    QString transitionEasing{"ease-in-out"};
    bool enableTransitions{true};

    // Grid and flexbox properties
    std::unordered_map<FluentBreakpoint, int> gridColumns;
    std::unordered_map<FluentBreakpoint, int> gridRows;
    std::unordered_map<FluentBreakpoint, QString> gridArea;
    std::unordered_map<FluentBreakpoint, QString> flexDirection;
    std::unordered_map<FluentBreakpoint, QString> flexWrap;
    std::unordered_map<FluentBreakpoint, QString> justifyContent;
    std::unordered_map<FluentBreakpoint, QString> alignItems;
};

// Responsive manager for global responsive behavior
class FluentResponsiveManager : public QObject {
    Q_OBJECT

public:
    static FluentResponsiveManager& instance();

    // Configuration
    void setResponsiveConfig(const ResponsiveConfig& config);
    ResponsiveConfig getResponsiveConfig() const { return m_config; }
    void setFluidTypographyConfig(const FluidTypographyConfig& config);
    FluidTypographyConfig getFluidTypographyConfig() const {
        return m_typographyConfig;
    }
    void setContainerQueryConfig(const ContainerQueryConfig& config);
    ContainerQueryConfig getContainerQueryConfig() const {
        return m_containerConfig;
    }

    // Breakpoint management
    FluentBreakpoint getCurrentBreakpoint() const {
        return m_currentBreakpoint;
    }
    FluentBreakpoint getBreakpointForSize(const QSize& size) const;
    FluentBreakpoint getBreakpointForContainer(QWidget* container) const;
    QSize getBreakpointMinSize(FluentBreakpoint breakpoint) const;
    QSize getBreakpointMaxSize(FluentBreakpoint breakpoint) const;
    QList<FluentBreakpoint> getAllBreakpoints() const;
    void setCustomBreakpoint(const QString& name, int minWidth,
                             int maxWidth = -1);

    // Fluid typography
    QFont calculateFluidFont(const QFont& baseFont,
                             const QSize& viewportSize) const;
    double calculateFluidFontSize(double baseFontSize,
                                  const QSize& viewportSize) const;
    double calculateFluidLineHeight(double baseFontSize,
                                    const QSize& viewportSize) const;
    double calculateFluidLetterSpacing(double baseFontSize,
                                       const QSize& viewportSize) const;

    // Container queries
    void enableContainerQueries(QWidget* container, bool enabled = true);
    void setContainerQueryBreakpoints(QWidget* container,
                                      const QMap<QString, QSize>& breakpoints);
    QString getContainerQueryClass(QWidget* container) const;
    void updateContainerQueries(QWidget* container = nullptr);

    // Device detection
    FluentDeviceType getCurrentDeviceType() const { return m_deviceType; }
    FluentOrientation getCurrentOrientation() const { return m_orientation; }
    bool isTouchDevice() const { return m_isTouchDevice; }
    bool isHighDPI() const { return m_isHighDPI; }

    // Screen information
    QScreen* getCurrentScreen() const;
    QSize getScreenSize() const;
    qreal getDevicePixelRatio() const;
    int getScreenDPI() const;

    // Responsive utilities
    double getScaleFactor() const { return m_scaleFactor; }
    void setScaleFactor(double factor);

    QSize scaleSize(const QSize& size) const;
    QMargins scaleMargins(const QMargins& margins) const;
    QFont scaleFont(const QFont& font) const;
    int scaleValue(int value) const;

    // Widget registration for responsive behavior
    void registerResponsiveWidget(QWidget* widget);
    void unregisterResponsiveWidget(QWidget* widget);
    void setWidgetResponsiveProperties(QWidget* widget,
                                       const ResponsiveProperties& properties);

    // Manual updates
    void updateAllWidgets();
    void updateWidget(QWidget* widget);

    // Touch optimization
    void enableTouchOptimization(bool enabled);
    bool isTouchOptimizationEnabled() const { return m_touchOptimization; }
    int getTouchTargetMinSize() const { return m_touchTargetMinSize; }
    void setTouchTargetMinSize(int size) { m_touchTargetMinSize = size; }

signals:
    void breakpointChanged(FluentBreakpoint oldBreakpoint,
                           FluentBreakpoint newBreakpoint);
    void deviceTypeChanged(FluentDeviceType oldType, FluentDeviceType newType);
    void orientationChanged(FluentOrientation oldOrientation,
                            FluentOrientation newOrientation);
    void scaleFactorChanged(double oldFactor, double newFactor);
    void screenChanged(QScreen* screen);

private slots:
    void onScreenChanged();
    void onScreenResized();
    void onOrientationChanged();
    void updateResponsiveState();

private:
    FluentResponsiveManager();
    void detectDeviceType();
    void detectOrientation();
    void updateBreakpoint();
    void applyResponsiveProperties(QWidget* widget,
                                   const ResponsiveProperties& properties);

private:
    ResponsiveConfig m_config;
    FluidTypographyConfig m_typographyConfig;
    ContainerQueryConfig m_containerConfig;
    FluentBreakpoint m_currentBreakpoint{FluentBreakpoint::Medium};
    FluentDeviceType m_deviceType{FluentDeviceType::Desktop};
    FluentOrientation m_orientation{FluentOrientation::Landscape};

    bool m_isTouchDevice{false};
    bool m_isHighDPI{false};
    bool m_touchOptimization{true};
    int m_touchTargetMinSize{44};  // 44px minimum touch target

    double m_scaleFactor{1.0};
    QScreen* m_currentScreen{nullptr};

    // Widget registry
    std::unordered_map<QWidget*, ResponsiveProperties> m_responsiveWidgets;
    mutable QMutex m_widgetsMutex;

    // Container queries
    std::unordered_map<QWidget*, ContainerQueryConfig> m_containerQueries;
    std::unordered_map<QWidget*, QMap<QString, QSize>> m_containerBreakpoints;
    std::unordered_map<QWidget*, QString> m_containerClasses;

    // Custom breakpoints
    QMap<QString, QPair<int, int>> m_customBreakpoints;

    // Update timer to debounce rapid changes
    QTimer* m_updateTimer;
    QTimer* m_containerUpdateTimer;

    // Breakpoint definitions (in pixels)
    static constexpr int BREAKPOINT_XS = 0;
    static constexpr int BREAKPOINT_SM = 576;
    static constexpr int BREAKPOINT_MD = 768;
    static constexpr int BREAKPOINT_LG = 992;
    static constexpr int BREAKPOINT_XL = 1200;
    static constexpr int BREAKPOINT_XXL = 1400;
};

// Responsive utilities
class FluentResponsiveUtils {
public:
    // Size calculations
    static QSize calculateFluidSize(const QSize& baseSize,
                                    const QSize& containerSize,
                                    double minScale = 0.5,
                                    double maxScale = 2.0);

    static QMargins calculateFluidMargins(const QMargins& baseMargins,
                                          FluentBreakpoint breakpoint);

    static int calculateFluidSpacing(int baseSpacing,
                                     FluentBreakpoint breakpoint);

    // Layout utilities
    static int calculateColumns(int containerWidth, int itemWidth, int gutter);
    static QSize calculateItemSize(int containerWidth, int columns, int gutter);
    static QRect calculateItemGeometry(int index, int columns,
                                       const QSize& itemSize, int gutter,
                                       const QMargins& margins);

    // Touch optimization
    static QSize ensureTouchTarget(const QSize& size, int minSize = 44);
    static QMargins ensureTouchSpacing(const QMargins& margins,
                                       int minSpacing = 8);

    // Accessibility scaling
    static QFont scaleForAccessibility(const QFont& font, double scaleFactor);
    static int scaleForAccessibility(int value, double scaleFactor);
};

// Convenience macros
#define FLUENT_RESPONSIVE_WIDGET(widget)                \
    FluentQt::Core::FluentResponsiveManager::instance() \
        .registerResponsiveWidget(widget)

#define FLUENT_BREAKPOINT_CASE(breakpoint, code)         \
    case FluentQt::Core::FluentBreakpoint::breakpoint: { \
        code;                                            \
    } break

}  // namespace FluentQt::Core
