// include/FluentQt/Core/FluentResponsiveComponent.h
#pragma once

#include <QAbstractAnimation>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPropertyAnimation>
#include <QSize>
#include <QString>
#include <QTimer>
#include <QVariant>
#include <QWidget>
#include <functional>
#include <memory>

#include "FluentResponsive.h"

namespace FluentQt::Core {

// Component variant types
enum class FluentComponentVariant {
    Compact,      // Minimal space usage
    Comfortable,  // Standard spacing
    Spacious,     // Generous spacing
    Touch,        // Optimized for touch
    Dense,        // Maximum information density
    Accessible,   // Enhanced accessibility
    Custom        // User-defined variant
};

// Component adaptation strategies
enum class FluentAdaptationStrategy {
    Hide,       // Hide component
    Collapse,   // Collapse to minimal size
    Stack,      // Stack elements vertically
    Wrap,       // Wrap elements to new lines
    Scroll,     // Add scrolling
    Paginate,   // Split into pages
    Summarize,  // Show summary/preview
    Transform   // Transform to different component
};

// Responsive behavior configuration
struct FluentResponsiveBehavior {
    FluentComponentVariant variant{FluentComponentVariant::Comfortable};
    FluentAdaptationStrategy strategy{FluentAdaptationStrategy::Wrap};
    bool enableTransitions{true};
    int transitionDuration{200};
    QString transitionEasing{"ease-in-out"};
    bool respectReducedMotion{true};
    bool enableAccessibilityEnhancements{true};
    QMap<QString, QVariant> customProperties;
};

// Component state for different breakpoints
struct FluentComponentState {
    FluentBreakpoint breakpoint;
    FluentComponentVariant variant;
    QSize size;
    QMargins margins;
    int spacing{8};
    bool visible{true};
    double opacity{1.0};
    QString styleClass;
    QMap<QString, QVariant> properties;
    FluentAdaptationStrategy adaptationStrategy{FluentAdaptationStrategy::Wrap};
};

// Responsive component configuration
struct FluentResponsiveComponentConfig {
    QString componentType;
    QMap<FluentBreakpoint, FluentComponentState> states;
    FluentResponsiveBehavior defaultBehavior;
    bool enableContainerQueries{false};
    bool enableFluidSizing{true};
    bool enableAccessibilityAdaptation{true};
    bool enablePerformanceOptimization{true};
    int updateThrottleMs{16};  // ~60fps
};

// Component metrics for optimization
struct FluentComponentMetrics {
    QSize renderSize;
    int renderTime{0};  // milliseconds
    int updateCount{0};
    double memoryUsage{0.0};  // MB
    bool isVisible{true};
    bool isInViewport{false};
    std::chrono::steady_clock::time_point lastUpdate;
    double performanceScore{100.0};  // 0-100
};

// Responsive component manager
class FluentResponsiveComponentManager : public QObject {
    Q_OBJECT

public:
    static FluentResponsiveComponentManager& instance();

    // Component registration
    void registerComponent(QWidget* component,
                           const FluentResponsiveComponentConfig& config);
    void unregisterComponent(QWidget* component);
    bool isComponentRegistered(QWidget* component) const;

    // Configuration management
    void setComponentConfig(QWidget* component,
                            const FluentResponsiveComponentConfig& config);
    FluentResponsiveComponentConfig getComponentConfig(
        QWidget* component) const;
    void updateComponentState(QWidget* component, FluentBreakpoint breakpoint);

    // Variant management
    void setComponentVariant(QWidget* component,
                             FluentComponentVariant variant);
    FluentComponentVariant getComponentVariant(QWidget* component) const;
    void setVariantForBreakpoint(QWidget* component,
                                 FluentBreakpoint breakpoint,
                                 FluentComponentVariant variant);

    // Adaptation strategies
    void setAdaptationStrategy(QWidget* component,
                               FluentAdaptationStrategy strategy);
    FluentAdaptationStrategy getAdaptationStrategy(QWidget* component) const;
    void applyAdaptationStrategy(QWidget* component,
                                 FluentBreakpoint breakpoint);

    // Container queries
    void enableContainerQueries(QWidget* component, bool enabled = true);
    void setContainerBreakpoints(QWidget* component,
                                 const QMap<QString, QSize>& breakpoints);
    void updateContainerQuery(QWidget* component);

    // Performance optimization
    void enablePerformanceOptimization(QWidget* component, bool enabled = true);
    void setUpdateThrottle(QWidget* component, int milliseconds);
    FluentComponentMetrics getComponentMetrics(QWidget* component) const;
    void optimizeComponent(QWidget* component);

    // Accessibility integration
    void enableAccessibilityAdaptation(QWidget* component, bool enabled = true);
    void updateAccessibilityFeatures(QWidget* component,
                                     FluentBreakpoint breakpoint);
    void setAccessibilityVariant(QWidget* component,
                                 FluentComponentVariant variant);

    // Animation and transitions
    void enableTransitions(QWidget* component, bool enabled = true);
    void setTransitionDuration(QWidget* component, int milliseconds);
    void setTransitionEasing(QWidget* component, const QString& easing);
    void animateToState(QWidget* component,
                        const FluentComponentState& targetState);

    // Utility methods
    QSize calculateOptimalSize(QWidget* component,
                               FluentBreakpoint breakpoint) const;
    FluentComponentVariant suggestVariant(QWidget* component,
                                          const QSize& availableSpace) const;
    FluentAdaptationStrategy suggestAdaptation(
        QWidget* component, const QSize& availableSpace) const;

    // Batch operations
    void updateAllComponents();
    void updateComponentsForBreakpoint(FluentBreakpoint breakpoint);
    void optimizeAllComponents();
    void applyVariantToAll(FluentComponentVariant variant);

    // Debugging and monitoring
    void enableDebugMode(bool enabled) { m_debugMode = enabled; }
    QStringList getRegisteredComponents() const;
    QMap<QWidget*, FluentComponentMetrics> getAllMetrics() const;
    void dumpComponentInfo(QWidget* component) const;

signals:
    void componentRegistered(QWidget* component);
    void componentUnregistered(QWidget* component);
    void componentVariantChanged(QWidget* component,
                                 FluentComponentVariant variant);
    void componentStateChanged(QWidget* component, FluentBreakpoint breakpoint);
    void adaptationApplied(QWidget* component,
                           FluentAdaptationStrategy strategy);
    void performanceOptimized(QWidget* component, double improvementPercent);
    void containerQueryUpdated(QWidget* component, const QString& queryClass);

private slots:
    void onBreakpointChanged(FluentBreakpoint breakpoint);
    void onComponentDestroyed(QObject* component);
    void onUpdateTimer();
    void onPerformanceTimer();

private:
    FluentResponsiveComponentManager();

    // Component management
    void applyComponentState(QWidget* component,
                             const FluentComponentState& state);
    void transitionToState(QWidget* component,
                           const FluentComponentState& fromState,
                           const FluentComponentState& toState);

    // Adaptation implementations
    void applyHideStrategy(QWidget* component);
    void applyCollapseStrategy(QWidget* component);
    void applyStackStrategy(QWidget* component);
    void applyWrapStrategy(QWidget* component);
    void applyScrollStrategy(QWidget* component);
    void applyPaginateStrategy(QWidget* component);
    void applySummarizeStrategy(QWidget* component);
    void applyTransformStrategy(QWidget* component);

    // Variant implementations
    void applyCompactVariant(QWidget* component);
    void applyComfortableVariant(QWidget* component);
    void applySpaciousVariant(QWidget* component);
    void applyTouchVariant(QWidget* component);
    void applyDenseVariant(QWidget* component);
    void applyAccessibleVariant(QWidget* component);
    void applyCustomVariant(QWidget* component,
                            const QMap<QString, QVariant>& properties);

    // Performance helpers
    void updateComponentMetrics(QWidget* component);
    void throttleUpdates(QWidget* component);
    bool shouldUpdateComponent(QWidget* component) const;

    // Container query helpers
    QString generateContainerQueryClass(QWidget* component) const;
    void updateContainerQueryClasses();

    // Animation helpers
    QPropertyAnimation* createTransitionAnimation(QWidget* component,
                                                  const QString& property,
                                                  const QVariant& startValue,
                                                  const QVariant& endValue);
    void setupTransitionGroup(QWidget* component,
                              const FluentComponentState& targetState);

private:
    // Component registry
    QMap<QWidget*, FluentResponsiveComponentConfig> m_components;
    QMap<QWidget*, FluentComponentState> m_currentStates;
    QMap<QWidget*, FluentComponentMetrics> m_metrics;

    // Container queries
    QMap<QWidget*, QMap<QString, QSize>> m_containerBreakpoints;
    QMap<QWidget*, QString> m_containerQueryClasses;

    // Performance optimization
    QMap<QWidget*, QTimer*> m_updateTimers;
    QTimer* m_globalUpdateTimer;
    QTimer* m_performanceTimer;
    bool m_performanceOptimizationEnabled{true};

    // Animation management
    QMap<QWidget*, QAbstractAnimation*> m_activeAnimations;

    // Configuration
    bool m_debugMode{false};
    FluentBreakpoint m_currentBreakpoint{FluentBreakpoint::Medium};

    // Thread safety
    mutable QMutex m_componentsMutex;
    mutable QMutex m_metricsMutex;
};

// Responsive component base class
class FluentResponsiveWidget : public QWidget {
    Q_OBJECT

public:
    explicit FluentResponsiveWidget(QWidget* parent = nullptr);
    virtual ~FluentResponsiveWidget();

    // Responsive configuration
    void setResponsiveConfig(const FluentResponsiveComponentConfig& config);
    FluentResponsiveComponentConfig getResponsiveConfig() const;

    // Variant management
    void setVariant(FluentComponentVariant variant);
    FluentComponentVariant getVariant() const;

    // Adaptation
    void setAdaptationStrategy(FluentAdaptationStrategy strategy);
    FluentAdaptationStrategy getAdaptationStrategy() const;

    // Container queries
    void enableContainerQueries(bool enabled = true);
    void setContainerBreakpoints(const QMap<QString, QSize>& breakpoints);

    // Performance
    void enablePerformanceOptimization(bool enabled = true);
    FluentComponentMetrics getMetrics() const;

protected:
    // Virtual methods for subclasses to override
    virtual void onBreakpointChanged(FluentBreakpoint breakpoint);
    virtual void onVariantChanged(FluentComponentVariant variant);
    virtual void onAdaptationApplied(FluentAdaptationStrategy strategy);
    virtual QSize calculateOptimalSize(FluentBreakpoint breakpoint) const;

    // Event handling
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private slots:
    void onResponsiveUpdate();

private:
    void initializeResponsive();
    void updateResponsiveState();

private:
    FluentResponsiveComponentConfig m_config;
    FluentComponentVariant m_currentVariant{
        FluentComponentVariant::Comfortable};
    FluentAdaptationStrategy m_currentStrategy{FluentAdaptationStrategy::Wrap};
    bool m_responsiveInitialized{false};
};

// Utility macros
#define FLUENT_RESPONSIVE_COMPONENT(ComponentClass) \
    class ComponentClass : public FluentQt::Core::FluentResponsiveWidget

#define FLUENT_REGISTER_RESPONSIVE_COMPONENT(component, config)  \
    FluentQt::Core::FluentResponsiveComponentManager::instance() \
        .registerComponent(component, config)

#define FLUENT_SET_COMPONENT_VARIANT(component, variant)         \
    FluentQt::Core::FluentResponsiveComponentManager::instance() \
        .setComponentVariant(component, variant)

}  // namespace FluentQt::Core
