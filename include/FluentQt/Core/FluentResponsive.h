// include/FluentQt/Core/FluentResponsive.h
#pragma once

#include <QObject>
#include <QWidget>
#include <QLayout>
#include <QSize>
#include <QRect>
#include <QScreen>
#include <QTimer>
#include <QMutex>
#include <functional>
#include <unordered_map>
#include <memory>

namespace FluentQt::Core {

// Responsive breakpoints following modern design systems
enum class FluentBreakpoint {
    XSmall,     // < 576px (mobile portrait)
    Small,      // 576px - 768px (mobile landscape, small tablets)
    Medium,     // 768px - 992px (tablets)
    Large,      // 992px - 1200px (small desktops)
    XLarge,     // 1200px - 1400px (large desktops)
    XXLarge     // > 1400px (extra large screens)
};

// Device types for responsive behavior
enum class FluentDeviceType {
    Mobile,
    Tablet,
    Desktop,
    TV,
    Unknown
};

// Orientation states
enum class FluentOrientation {
    Portrait,
    Landscape,
    Square
};

// Responsive configuration
struct ResponsiveConfig {
    QSize minSize{320, 240};        // Minimum supported size
    QSize maxSize{3840, 2160};      // Maximum supported size
    bool enableTouchOptimization{true};
    bool enableMobileFirst{true};
    bool enableFluidLayouts{true};
    bool enableAdaptiveSpacing{true};
    bool enableScalableText{true};
    double minScaleFactor{0.8};
    double maxScaleFactor{2.0};
};

// Responsive properties for widgets
struct ResponsiveProperties {
    std::unordered_map<FluentBreakpoint, QSize> sizes;
    std::unordered_map<FluentBreakpoint, QMargins> margins;
    std::unordered_map<FluentBreakpoint, int> spacing;
    std::unordered_map<FluentBreakpoint, bool> visibility;
    std::unordered_map<FluentBreakpoint, QString> styleSheet;
    std::unordered_map<FluentBreakpoint, QFont> fonts;
    bool isFluid{true};
    double aspectRatio{0.0}; // 0 = no constraint
};

// Responsive layout manager
class FluentResponsiveLayout : public QLayout {
    Q_OBJECT

public:
    explicit FluentResponsiveLayout(QWidget* parent = nullptr);
    ~FluentResponsiveLayout() override;
    
    // QLayout interface
    void addItem(QLayoutItem* item) override;
    QLayoutItem* itemAt(int index) const override;
    QLayoutItem* takeAt(int index) override;
    int count() const override;
    QSize sizeHint() const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect& rect) override;
    
    // Responsive layout configuration
    void setBreakpointBehavior(FluentBreakpoint breakpoint, 
                              std::function<void(const QRect&)> layoutFunction);
    void setFluidMode(bool enabled) { m_fluidMode = enabled; }
    bool isFluidMode() const { return m_fluidMode; }
    
    // Grid system
    void setColumns(FluentBreakpoint breakpoint, int columns);
    void setGutterSize(FluentBreakpoint breakpoint, int size);
    void addWidget(QWidget* widget, int column, int row, int columnSpan = 1, int rowSpan = 1);
    void addWidget(QWidget* widget, FluentBreakpoint breakpoint, 
                   int column, int row, int columnSpan = 1, int rowSpan = 1);
    
    // Responsive utilities
    void setItemResponsiveProperties(QWidget* widget, const ResponsiveProperties& properties);
    ResponsiveProperties getItemResponsiveProperties(QWidget* widget) const;

signals:
    void breakpointChanged(FluentBreakpoint oldBreakpoint, FluentBreakpoint newBreakpoint);
    void layoutRecalculated(const QRect& geometry);

private:
    void updateLayout(const QRect& rect);
    void applyBreakpointLayout(FluentBreakpoint breakpoint, const QRect& rect);
    
private:
    struct LayoutItem {
        QLayoutItem* item;
        ResponsiveProperties properties;
        std::unordered_map<FluentBreakpoint, QRect> geometries;
    };
    
    QList<LayoutItem> m_items;
    FluentBreakpoint m_currentBreakpoint{FluentBreakpoint::Medium};
    bool m_fluidMode{true};
    
    // Grid system
    std::unordered_map<FluentBreakpoint, int> m_columns;
    std::unordered_map<FluentBreakpoint, int> m_gutterSizes;
    std::unordered_map<FluentBreakpoint, std::function<void(const QRect&)>> m_breakpointLayouts;
};

// Responsive manager for global responsive behavior
class FluentResponsiveManager : public QObject {
    Q_OBJECT

public:
    static FluentResponsiveManager& instance();
    
    // Configuration
    void setResponsiveConfig(const ResponsiveConfig& config);
    ResponsiveConfig getResponsiveConfig() const { return m_config; }
    
    // Breakpoint management
    FluentBreakpoint getCurrentBreakpoint() const { return m_currentBreakpoint; }
    FluentBreakpoint getBreakpointForSize(const QSize& size) const;
    QSize getBreakpointMinSize(FluentBreakpoint breakpoint) const;
    QSize getBreakpointMaxSize(FluentBreakpoint breakpoint) const;
    
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
    void setWidgetResponsiveProperties(QWidget* widget, const ResponsiveProperties& properties);
    
    // Manual updates
    void updateAllWidgets();
    void updateWidget(QWidget* widget);
    
    // Touch optimization
    void enableTouchOptimization(bool enabled);
    bool isTouchOptimizationEnabled() const { return m_touchOptimization; }
    int getTouchTargetMinSize() const { return m_touchTargetMinSize; }
    void setTouchTargetMinSize(int size) { m_touchTargetMinSize = size; }

signals:
    void breakpointChanged(FluentBreakpoint oldBreakpoint, FluentBreakpoint newBreakpoint);
    void deviceTypeChanged(FluentDeviceType oldType, FluentDeviceType newType);
    void orientationChanged(FluentOrientation oldOrientation, FluentOrientation newOrientation);
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
    void applyResponsiveProperties(QWidget* widget, const ResponsiveProperties& properties);

private:
    ResponsiveConfig m_config;
    FluentBreakpoint m_currentBreakpoint{FluentBreakpoint::Medium};
    FluentDeviceType m_deviceType{FluentDeviceType::Desktop};
    FluentOrientation m_orientation{FluentOrientation::Landscape};
    
    bool m_isTouchDevice{false};
    bool m_isHighDPI{false};
    bool m_touchOptimization{true};
    int m_touchTargetMinSize{44}; // 44px minimum touch target
    
    double m_scaleFactor{1.0};
    QScreen* m_currentScreen{nullptr};
    
    // Widget registry
    std::unordered_map<QWidget*, ResponsiveProperties> m_responsiveWidgets;
    mutable QMutex m_widgetsMutex;
    
    // Update timer to debounce rapid changes
    QTimer* m_updateTimer;
    
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
    static QSize calculateFluidSize(const QSize& baseSize, const QSize& containerSize, 
                                   double minScale = 0.5, double maxScale = 2.0);
    
    static QMargins calculateFluidMargins(const QMargins& baseMargins, 
                                         FluentBreakpoint breakpoint);
    
    static int calculateFluidSpacing(int baseSpacing, FluentBreakpoint breakpoint);
    
    // Layout utilities
    static int calculateColumns(int containerWidth, int itemWidth, int gutter);
    static QSize calculateItemSize(int containerWidth, int columns, int gutter);
    static QRect calculateItemGeometry(int index, int columns, const QSize& itemSize, 
                                      int gutter, const QMargins& margins);
    
    // Touch optimization
    static QSize ensureTouchTarget(const QSize& size, int minSize = 44);
    static QMargins ensureTouchSpacing(const QMargins& margins, int minSpacing = 8);
    
    // Accessibility scaling
    static QFont scaleForAccessibility(const QFont& font, double scaleFactor);
    static int scaleForAccessibility(int value, double scaleFactor);
};

// Convenience macros
#define FLUENT_RESPONSIVE_WIDGET(widget) \
    FluentQt::Core::FluentResponsiveManager::instance().registerResponsiveWidget(widget)

#define FLUENT_BREAKPOINT_CASE(breakpoint, code) \
    case FluentQt::Core::FluentBreakpoint::breakpoint: { code; } break

} // namespace FluentQt::Core
