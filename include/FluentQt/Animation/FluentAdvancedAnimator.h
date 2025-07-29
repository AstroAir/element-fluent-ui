// include/FluentQt/Animation/FluentAdvancedAnimator.h
#pragma once

#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QPauseAnimation>
#include <QGraphicsEffect>
#include <QTimer>
#include <QEasingCurve>
#include <QPoint>
#include <QRect>
#include <QColor>
#include <functional>
#include <memory>
#include <unordered_map>

namespace FluentQt::Animation {

// Advanced animation types
enum class FluentAdvancedAnimationType {
    // Micro-interactions
    Hover,
    Press,
    Focus,
    Ripple,
    Pulse,
    Shake,
    Bounce,
    Glow,
    
    // Page transitions
    SlideLeft,
    SlideRight,
    SlideUp,
    SlideDown,
    FadeThrough,
    SharedElement,
    Morph,
    
    // Gesture animations
    Swipe,
    Pinch,
    Rotate,
    Drag,
    Fling,
    
    // Advanced effects
    Parallax,
    Reveal,
    Dissolve,
    Flip,
    Fold,
    Unfold,
    Elastic,
    Magnetic
};

// Animation performance levels
enum class FluentAnimationPerformance {
    High,       // Full quality, all effects
    Medium,     // Reduced quality for better performance
    Low,        // Minimal effects, maximum performance
    Adaptive    // Automatically adjust based on system performance
};

// Gesture types for gesture animations
enum class FluentGestureType {
    Tap,
    DoubleTap,
    LongPress,
    Swipe,
    Pan,
    Pinch,
    Rotate,
    Hover,
    Leave
};

// Animation configuration
struct FluentAdvancedAnimationConfig {
    int duration{300};
    QEasingCurve::Type easing{QEasingCurve::OutCubic};
    int delay{0};
    bool respectReducedMotion{true};
    FluentAnimationPerformance performance{FluentAnimationPerformance::Adaptive};
    bool enableHardwareAcceleration{true};
    double intensity{1.0}; // 0.0 to 2.0
    QVariantMap customProperties;
};

// Micro-interaction configuration
struct FluentMicroInteractionConfig {
    bool enableHover{true};
    bool enablePress{true};
    bool enableFocus{true};
    bool enableRipple{true};
    int hoverDuration{150};
    int pressDuration{100};
    int focusDuration{200};
    int rippleDuration{400};
    double hoverScale{1.05};
    double pressScale{0.95};
    QColor rippleColor{255, 255, 255, 100};
};

// Page transition configuration
struct FluentPageTransitionConfig {
    FluentAdvancedAnimationType type{FluentAdvancedAnimationType::FadeThrough};
    int duration{400};
    QEasingCurve::Type easing{QEasingCurve::OutCubic};
    bool enableSharedElements{true};
    bool enableParallax{false};
    double parallaxIntensity{0.3};
    QStringList sharedElementIds;
};

// Advanced animator with micro-interactions and page transitions
class FluentAdvancedAnimator : public QObject {
    Q_OBJECT

public:
    explicit FluentAdvancedAnimator(QObject* parent = nullptr);
    ~FluentAdvancedAnimator() override;
    
    // Configuration
    void setDefaultConfig(const FluentAdvancedAnimationConfig& config);
    FluentAdvancedAnimationConfig getDefaultConfig() const { return m_defaultConfig; }
    
    void setPerformanceLevel(FluentAnimationPerformance level);
    FluentAnimationPerformance getPerformanceLevel() const { return m_performanceLevel; }
    
    // Micro-interactions
    void enableMicroInteractions(QWidget* widget, const FluentMicroInteractionConfig& config = {});
    void disableMicroInteractions(QWidget* widget);
    
    std::unique_ptr<QPropertyAnimation> createHoverAnimation(QWidget* widget, const FluentAdvancedAnimationConfig& config = {});
    std::unique_ptr<QPropertyAnimation> createPressAnimation(QWidget* widget, const FluentAdvancedAnimationConfig& config = {});
    std::unique_ptr<QPropertyAnimation> createFocusAnimation(QWidget* widget, const FluentAdvancedAnimationConfig& config = {});
    std::unique_ptr<QPropertyAnimation> createRippleAnimation(QWidget* widget, const QPoint& center, const FluentAdvancedAnimationConfig& config = {});
    
    // Advanced effects
    std::unique_ptr<QPropertyAnimation> createPulseAnimation(QWidget* widget, const FluentAdvancedAnimationConfig& config = {});
    std::unique_ptr<QPropertyAnimation> createShakeAnimation(QWidget* widget, const FluentAdvancedAnimationConfig& config = {});
    std::unique_ptr<QPropertyAnimation> createBounceAnimation(QWidget* widget, const FluentAdvancedAnimationConfig& config = {});
    std::unique_ptr<QPropertyAnimation> createGlowAnimation(QWidget* widget, const FluentAdvancedAnimationConfig& config = {});
    
    // Page transitions
    std::unique_ptr<QSequentialAnimationGroup> createPageTransition(QWidget* fromPage, QWidget* toPage, 
                                                                    const FluentPageTransitionConfig& config = {});
    
    std::unique_ptr<QParallelAnimationGroup> createSharedElementTransition(const QList<QPair<QWidget*, QWidget*>>& sharedElements,
                                                                           const FluentAdvancedAnimationConfig& config = {});
    
    // Gesture animations
    void enableGestureAnimations(QWidget* widget);
    void disableGestureAnimations(QWidget* widget);
    
    std::unique_ptr<QPropertyAnimation> createSwipeAnimation(QWidget* widget, const QPoint& direction, const FluentAdvancedAnimationConfig& config = {});
    std::unique_ptr<QPropertyAnimation> createPinchAnimation(QWidget* widget, double scaleFactor, const FluentAdvancedAnimationConfig& config = {});
    std::unique_ptr<QPropertyAnimation> createRotateAnimation(QWidget* widget, double angle, const FluentAdvancedAnimationConfig& config = {});
    
    // Advanced visual effects
    std::unique_ptr<QPropertyAnimation> createParallaxAnimation(QWidget* widget, const QPoint& offset, const FluentAdvancedAnimationConfig& config = {});
    std::unique_ptr<QSequentialAnimationGroup> createRevealAnimation(QWidget* widget, const QPoint& center, const FluentAdvancedAnimationConfig& config = {});
    std::unique_ptr<QPropertyAnimation> createDissolveAnimation(QWidget* widget, const FluentAdvancedAnimationConfig& config = {});
    std::unique_ptr<QSequentialAnimationGroup> createFlipAnimation(QWidget* widget, Qt::Axis axis, const FluentAdvancedAnimationConfig& config = {});
    
    // Performance optimization
    void optimizeForPerformance();
    void enableHardwareAcceleration(QWidget* widget, bool enabled = true);
    bool shouldSkipAnimation(QWidget* widget) const;
    
    // Animation management
    void pauseAllAnimations();
    void resumeAllAnimations();
    void stopAllAnimations();
    void clearAnimations(QWidget* widget);
    
    // Animation groups and sequences
    std::unique_ptr<QSequentialAnimationGroup> createSequence(const QList<QAbstractAnimation*>& animations);
    std::unique_ptr<QParallelAnimationGroup> createParallel(const QList<QAbstractAnimation*>& animations);
    std::unique_ptr<QSequentialAnimationGroup> createStaggered(const QList<QWidget*>& widgets, 
                                                               FluentAdvancedAnimationType type,
                                                               int staggerDelay = 50);

signals:
    void animationStarted(QWidget* widget, FluentAdvancedAnimationType type);
    void animationFinished(QWidget* widget, FluentAdvancedAnimationType type);
    void microInteractionTriggered(QWidget* widget, FluentGestureType gesture);
    void pageTransitionStarted(QWidget* fromPage, QWidget* toPage);
    void pageTransitionFinished(QWidget* fromPage, QWidget* toPage);

private slots:
    void onAnimationFinished();
    void onMicroInteractionTriggered();

private:
    void setupMicroInteractionHandlers(QWidget* widget, const FluentMicroInteractionConfig& config);
    void setupGestureHandlers(QWidget* widget);
    QPropertyAnimation* createBaseAnimation(QWidget* widget, const QByteArray& property, 
                                          const QVariant& startValue, const QVariant& endValue,
                                          const FluentAdvancedAnimationConfig& config);
    void applyPerformanceOptimizations(QPropertyAnimation* animation);
    bool isReducedMotionEnabled() const;

private:
    FluentAdvancedAnimationConfig m_defaultConfig;
    FluentAnimationPerformance m_performanceLevel{FluentAnimationPerformance::Adaptive};
    
    // Widget tracking
    std::unordered_map<QWidget*, FluentMicroInteractionConfig> m_microInteractionWidgets;
    std::unordered_map<QWidget*, QList<QPropertyAnimation*>> m_activeAnimations;
    
    // Performance monitoring
    QTimer* m_performanceTimer;
    int m_activeAnimationCount{0};
    bool m_hardwareAccelerationEnabled{true};
};

// Micro-interaction manager
class FluentMicroInteractionManager : public QObject {
    Q_OBJECT

public:
    static FluentMicroInteractionManager& instance();
    
    // Global configuration
    void setGlobalMicroInteractionConfig(const FluentMicroInteractionConfig& config);
    FluentMicroInteractionConfig getGlobalMicroInteractionConfig() const { return m_globalConfig; }
    
    // Widget registration
    void registerWidget(QWidget* widget, const FluentMicroInteractionConfig& config = {});
    void unregisterWidget(QWidget* widget);
    
    // Enable/disable globally
    void setMicroInteractionsEnabled(bool enabled);
    bool areMicroInteractionsEnabled() const { return m_enabled; }
    
    // Performance adaptation
    void adaptToPerformance(double frameRate);

signals:
    void microInteractionConfigChanged(const FluentMicroInteractionConfig& config);
    void microInteractionsEnabledChanged(bool enabled);

private:
    FluentMicroInteractionManager() = default;

private:
    FluentMicroInteractionConfig m_globalConfig;
    bool m_enabled{true};
    std::unordered_map<QWidget*, FluentMicroInteractionConfig> m_widgetConfigs;
};

// Page transition manager
class FluentPageTransitionManager : public QObject {
    Q_OBJECT

public:
    explicit FluentPageTransitionManager(QObject* parent = nullptr);
    
    // Transition execution
    void transitionToPage(QWidget* fromPage, QWidget* toPage, const FluentPageTransitionConfig& config = {});
    void transitionWithSharedElements(QWidget* fromPage, QWidget* toPage, 
                                     const QList<QPair<QWidget*, QWidget*>>& sharedElements,
                                     const FluentPageTransitionConfig& config = {});
    
    // Transition history
    void pushPage(QWidget* page);
    QWidget* popPage();
    QWidget* currentPage() const;
    bool canGoBack() const;
    
    // Configuration
    void setDefaultTransitionConfig(const FluentPageTransitionConfig& config);
    FluentPageTransitionConfig getDefaultTransitionConfig() const { return m_defaultConfig; }

signals:
    void transitionStarted(QWidget* fromPage, QWidget* toPage);
    void transitionFinished(QWidget* fromPage, QWidget* toPage);
    void pageStackChanged();

private:
    FluentPageTransitionConfig m_defaultConfig;
    QList<QWidget*> m_pageStack;
    QWidget* m_currentPage{nullptr};
    FluentAdvancedAnimator* m_animator;
};

// Gesture animation controller
class FluentGestureAnimationController : public QObject {
    Q_OBJECT

public:
    explicit FluentGestureAnimationController(QWidget* widget, QObject* parent = nullptr);
    
    // Gesture configuration
    void enableGesture(FluentGestureType gesture, bool enabled = true);
    bool isGestureEnabled(FluentGestureType gesture) const;
    
    void setGestureAnimation(FluentGestureType gesture, FluentAdvancedAnimationType animation);
    FluentAdvancedAnimationType getGestureAnimation(FluentGestureType gesture) const;
    
    // Gesture sensitivity
    void setSwipeSensitivity(double sensitivity) { m_swipeSensitivity = sensitivity; }
    void setPinchSensitivity(double sensitivity) { m_pinchSensitivity = sensitivity; }
    void setRotationSensitivity(double sensitivity) { m_rotationSensitivity = sensitivity; }

signals:
    void gestureDetected(FluentGestureType gesture, const QVariantMap& data);
    void gestureAnimationTriggered(FluentGestureType gesture, FluentAdvancedAnimationType animation);

protected:
    bool eventFilter(QObject* object, QEvent* event) override;

private:
    void handleTouchEvent(QTouchEvent* event);
    void handleMouseEvent(QMouseEvent* event);
    void detectGesture(const QList<QTouchEvent::TouchPoint>& touchPoints);

private:
    QWidget* m_widget;
    std::unordered_map<FluentGestureType, bool> m_enabledGestures;
    std::unordered_map<FluentGestureType, FluentAdvancedAnimationType> m_gestureAnimations;
    
    double m_swipeSensitivity{1.0};
    double m_pinchSensitivity{1.0};
    double m_rotationSensitivity{1.0};
    
    // Gesture state
    QPoint m_lastTouchPoint;
    QPoint m_gestureStartPoint;
    double m_initialPinchDistance{0.0};
    double m_initialRotationAngle{0.0};
};

// Animation performance monitor
class FluentAnimationPerformanceMonitor : public QObject {
    Q_OBJECT

public:
    static FluentAnimationPerformanceMonitor& instance();
    
    // Performance monitoring
    void startMonitoring();
    void stopMonitoring();
    
    void recordAnimationStart(QWidget* widget, FluentAdvancedAnimationType type);
    void recordAnimationEnd(QWidget* widget, FluentAdvancedAnimationType type);
    
    // Performance metrics
    double getCurrentFrameRate() const { return m_currentFrameRate; }
    int getActiveAnimationCount() const { return m_activeAnimationCount; }
    double getAverageAnimationDuration() const;
    
    // Performance recommendations
    FluentAnimationPerformance getRecommendedPerformanceLevel() const;
    bool shouldReduceAnimations() const;
    bool shouldDisableHardwareAcceleration() const;

signals:
    void performanceLevelChanged(FluentAnimationPerformance level);
    void frameRateChanged(double frameRate);

private slots:
    void updatePerformanceMetrics();

private:
    FluentAnimationPerformanceMonitor() = default;

private:
    QTimer* m_monitorTimer;
    double m_currentFrameRate{60.0};
    int m_activeAnimationCount{0};
    QList<std::chrono::milliseconds> m_animationDurations;
};

// Convenience macros
#define FLUENT_MICRO_INTERACTIONS(widget) \
    FluentQt::Animation::FluentMicroInteractionManager::instance().registerWidget(widget)

#define FLUENT_GESTURE_ANIMATIONS(widget) \
    new FluentQt::Animation::FluentGestureAnimationController(widget, widget)

#define FLUENT_PAGE_TRANSITION(from, to, type) \
    do { \
        FluentQt::Animation::FluentPageTransitionConfig config; \
        config.type = type; \
        /* Apply transition */ \
    } while(0)

} // namespace FluentQt::Animation
