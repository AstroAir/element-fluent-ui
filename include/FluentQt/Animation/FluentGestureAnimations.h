// include/FluentQt/Animation/FluentGestureAnimations.h
#pragma once

#include <QObject>
#include <QWidget>
#include <QGestureEvent>
#include <QTouchEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QTimer>
#include <QElapsedTimer>
#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QRect>
#include <QTransform>
#include <QMutex>
#include <QMap>
#include <QQueue>
#include <functional>
#include <memory>
#include <chrono>

#include "FluentAdvancedAnimator.h"

namespace FluentQt::Animation {

// Gesture types for animations
enum class FluentGestureType {
    Tap,                    // Single tap
    DoubleTap,              // Double tap
    LongPress,              // Long press/hold
    Swipe,                  // Swipe gesture
    Pan,                    // Pan/drag gesture
    Pinch,                  // Pinch to zoom
    Rotate,                 // Rotation gesture
    Flick,                  // Flick gesture
    Hover,                  // Mouse hover
    Scroll,                 // Scroll wheel
    Custom                  // Custom gesture
};

// Gesture animation phases
enum class FluentGesturePhase {
    Begin,                  // Gesture started
    Update,                 // Gesture in progress
    End,                    // Gesture completed
    Cancel                  // Gesture cancelled
};

// Gesture animation styles
enum class FluentGestureAnimationStyle {
    Immediate,              // Instant response
    Smooth,                 // Smooth following
    Elastic,                // Elastic response
    Magnetic,               // Magnetic attraction
    Physics,                // Physics-based
    Spring,                 // Spring dynamics
    Momentum,               // Momentum-based
    Custom                  // Custom style
};

// Gesture feedback types
enum class FluentGestureFeedback {
    None,                   // No feedback
    Visual,                 // Visual feedback only
    Haptic,                 // Haptic feedback (if supported)
    Audio,                  // Audio feedback
    Combined                // Multiple feedback types
};

// Gesture configuration
struct FluentGestureConfig {
    FluentGestureType type{FluentGestureType::Tap};
    FluentGestureAnimationStyle style{FluentGestureAnimationStyle::Smooth};
    FluentGestureFeedback feedback{FluentGestureFeedback::Visual};
    
    // Timing configuration
    std::chrono::milliseconds responseDuration{150};
    std::chrono::milliseconds feedbackDuration{100};
    std::chrono::milliseconds resetDuration{300};
    
    // Sensitivity and thresholds
    double sensitivity{1.0};
    double threshold{10.0};
    double maxDistance{1000.0};
    double dampingFactor{0.8};
    
    // Visual properties
    double scaleMultiplier{1.1};
    double rotationMultiplier{1.0};
    double opacityMultiplier{0.9};
    QColor highlightColor{255, 255, 255, 50};
    
    // Physics properties
    double mass{1.0};
    double stiffness{300.0};
    double damping{30.0};
    double friction{0.7};
    
    // Constraints
    QRect constraintRect;
    bool constrainToParent{true};
    bool allowOvershoot{true};
    bool snapToGrid{false};
    QSize gridSize{10, 10};
    
    // Custom properties
    QMap<QString, QVariant> customProperties;
};

// Gesture animation state
struct FluentGestureState {
    FluentGesturePhase phase{FluentGesturePhase::Begin};
    QPointF startPosition;
    QPointF currentPosition;
    QPointF velocity;
    QPointF acceleration;
    double scale{1.0};
    double rotation{0.0};
    double pressure{1.0};
    std::chrono::steady_clock::time_point timestamp;
    QMap<QString, QVariant> customData;
};

// Gesture animation result
struct FluentGestureAnimationResult {
    bool wasHandled{false};
    bool shouldContinue{true};
    QPointF finalPosition;
    double finalScale{1.0};
    double finalRotation{0.0};
    std::chrono::milliseconds duration{0};
    QString animationType;
    QStringList appliedEffects;
};

// Advanced gesture animation controller
class FluentGestureAnimationController : public QObject {
    Q_OBJECT

public:
    explicit FluentGestureAnimationController(QWidget* widget, QObject* parent = nullptr);
    ~FluentGestureAnimationController();
    
    // Gesture configuration
    void enableGesture(FluentGestureType gesture, bool enabled = true);
    bool isGestureEnabled(FluentGestureType gesture) const;
    void setGestureConfig(FluentGestureType gesture, const FluentGestureConfig& config);
    FluentGestureConfig getGestureConfig(FluentGestureType gesture) const;
    
    // Animation style configuration
    void setGestureAnimationStyle(FluentGestureType gesture, FluentGestureAnimationStyle style);
    FluentGestureAnimationStyle getGestureAnimationStyle(FluentGestureType gesture) const;
    void setGlobalAnimationStyle(FluentGestureAnimationStyle style);
    
    // Feedback configuration
    void setGestureFeedback(FluentGestureType gesture, FluentGestureFeedback feedback);
    FluentGestureFeedback getGestureFeedback(FluentGestureType gesture) const;
    void enableHapticFeedback(bool enabled = true);
    void enableAudioFeedback(bool enabled = true);
    
    // Gesture handling
    bool handleGestureEvent(QGestureEvent* event);
    bool handleTouchEvent(QTouchEvent* event);
    bool handleMouseEvent(QMouseEvent* event);
    bool handleWheelEvent(QWheelEvent* event);
    
    // Animation control
    void startGestureAnimation(FluentGestureType gesture, const FluentGestureState& state);
    void updateGestureAnimation(FluentGestureType gesture, const FluentGestureState& state);
    void endGestureAnimation(FluentGestureType gesture, const FluentGestureState& state);
    void cancelGestureAnimation(FluentGestureType gesture);
    void cancelAllGestureAnimations();
    
    // Physics simulation
    void enablePhysicsSimulation(bool enabled = true);
    bool isPhysicsSimulationEnabled() const { return m_physicsEnabled; }
    void setPhysicsProperties(double mass, double stiffness, double damping);
    void simulatePhysics(const FluentGestureState& state);
    
    // Constraint management
    void setConstraintRect(const QRect& rect);
    QRect getConstraintRect() const { return m_constraintRect; }
    void enableParentConstraint(bool enabled = true);
    void enableGridSnapping(bool enabled = true, const QSize& gridSize = QSize(10, 10));
    
    // Custom gesture handlers
    void registerCustomGestureHandler(FluentGestureType gesture, 
                                    std::function<FluentGestureAnimationResult(const FluentGestureState&)> handler);
    void unregisterCustomGestureHandler(FluentGestureType gesture);
    
    // Performance optimization
    void enablePerformanceOptimization(bool enabled = true);
    void setMaxConcurrentGestures(int maxGestures);
    void setUpdateThrottle(std::chrono::milliseconds throttle);
    
    // Cultural and accessibility adaptation
    void adaptForCulture(const QLocale& locale);
    void adaptForAccessibility(FluentAccessibilityAnimationMode mode);
    void setReducedMotionMode(bool enabled);

signals:
    void gestureStarted(FluentGestureType gesture, const FluentGestureState& state);
    void gestureUpdated(FluentGestureType gesture, const FluentGestureState& state);
    void gestureEnded(FluentGestureType gesture, const FluentGestureState& state);
    void gestureCancelled(FluentGestureType gesture);
    void animationCompleted(FluentGestureType gesture, const FluentGestureAnimationResult& result);
    void constraintViolated(FluentGestureType gesture, const QPointF& attemptedPosition);
    void physicsSimulationUpdated(const FluentGestureState& state);

private slots:
    void onAnimationFinished();
    void onPhysicsTimer();
    void onThrottleTimer();

private:
    // Gesture detection and processing
    FluentGestureType detectGestureType(QGestureEvent* event);
    FluentGestureType detectGestureType(QTouchEvent* event);
    FluentGestureType detectGestureType(QMouseEvent* event);
    FluentGestureState createGestureState(QGestureEvent* event);
    FluentGestureState createGestureState(QTouchEvent* event);
    FluentGestureState createGestureState(QMouseEvent* event);
    
    // Animation creation and management
    QPropertyAnimation* createGestureAnimation(FluentGestureType gesture, const FluentGestureState& state);
    void configureAnimationForStyle(QPropertyAnimation* animation, FluentGestureAnimationStyle style);
    void applyConstraints(QPointF& position);
    QPointF snapToGrid(const QPointF& position);
    
    // Physics simulation
    void updatePhysicsState(FluentGestureState& state, std::chrono::milliseconds deltaTime);
    QPointF calculateSpringForce(const QPointF& position, const QPointF& target);
    QPointF calculateDampingForce(const QPointF& velocity);
    QPointF calculateFrictionForce(const QPointF& velocity);
    
    // Feedback generation
    void generateVisualFeedback(FluentGestureType gesture, const FluentGestureState& state);
    void generateHapticFeedback(FluentGestureType gesture, const FluentGestureState& state);
    void generateAudioFeedback(FluentGestureType gesture, const FluentGestureState& state);
    
    // Cultural and accessibility adaptation
    FluentGestureConfig adaptConfigForCulture(const FluentGestureConfig& config, const QLocale& locale);
    FluentGestureConfig adaptConfigForAccessibility(const FluentGestureConfig& config, FluentAccessibilityAnimationMode mode);
    
    // Performance optimization
    bool shouldThrottleUpdate() const;
    void optimizeForPerformance();
    void cleanupFinishedAnimations();

private:
    QWidget* m_widget;
    
    // Configuration
    QMap<FluentGestureType, FluentGestureConfig> m_gestureConfigs;
    QMap<FluentGestureType, bool> m_enabledGestures;
    FluentGestureAnimationStyle m_globalStyle{FluentGestureAnimationStyle::Smooth};
    
    // Animation management
    QMap<FluentGestureType, QPropertyAnimation*> m_activeAnimations;
    QMap<FluentGestureType, FluentGestureState> m_gestureStates;
    QMap<FluentGestureType, std::function<FluentGestureAnimationResult(const FluentGestureState&)>> m_customHandlers;
    
    // Physics simulation
    bool m_physicsEnabled{false};
    QTimer* m_physicsTimer;
    std::chrono::steady_clock::time_point m_lastPhysicsUpdate;
    
    // Constraints
    QRect m_constraintRect;
    bool m_parentConstraintEnabled{true};
    bool m_gridSnappingEnabled{false};
    QSize m_gridSize{10, 10};
    
    // Performance optimization
    bool m_performanceOptimizationEnabled{true};
    int m_maxConcurrentGestures{5};
    std::chrono::milliseconds m_updateThrottle{16}; // ~60fps
    QTimer* m_throttleTimer;
    QQueue<std::function<void()>> m_pendingUpdates;
    
    // Feedback
    bool m_hapticFeedbackEnabled{false};
    bool m_audioFeedbackEnabled{false};
    
    // Cultural and accessibility
    QLocale m_currentLocale;
    FluentAccessibilityAnimationMode m_accessibilityMode{FluentAccessibilityAnimationMode::Full};
    bool m_reducedMotionMode{false};
    
    // Thread safety
    mutable QMutex m_configMutex;
    mutable QMutex m_animationMutex;
};

// Gesture animation factory
class FluentGestureAnimationFactory {
public:
    // Standard gesture animations
    static QPropertyAnimation* createTapAnimation(QWidget* widget, const FluentGestureConfig& config);
    static QPropertyAnimation* createSwipeAnimation(QWidget* widget, const QPointF& direction, const FluentGestureConfig& config);
    static QPropertyAnimation* createPinchAnimation(QWidget* widget, double scaleFactor, const FluentGestureConfig& config);
    static QPropertyAnimation* createRotateAnimation(QWidget* widget, double angle, const FluentGestureConfig& config);
    static QPropertyAnimation* createPanAnimation(QWidget* widget, const QPointF& delta, const FluentGestureConfig& config);
    
    // Physics-based animations
    static QPropertyAnimation* createSpringAnimation(QWidget* widget, const QPointF& target, const FluentGestureConfig& config);
    static QPropertyAnimation* createMomentumAnimation(QWidget* widget, const QPointF& velocity, const FluentGestureConfig& config);
    static QPropertyAnimation* createElasticAnimation(QWidget* widget, const QPointF& displacement, const FluentGestureConfig& config);
    
    // Feedback animations
    static QPropertyAnimation* createRippleAnimation(QWidget* widget, const QPointF& center, const FluentGestureConfig& config);
    static QPropertyAnimation* createHighlightAnimation(QWidget* widget, const FluentGestureConfig& config);
    static QPropertyAnimation* createPulseAnimation(QWidget* widget, const FluentGestureConfig& config);
    
    // Utility methods
    static QEasingCurve getEasingForStyle(FluentGestureAnimationStyle style);
    static std::chrono::milliseconds getDurationForStyle(FluentGestureAnimationStyle style, std::chrono::milliseconds baseDuration);
    static QPropertyAnimation* combineAnimations(const QList<QPropertyAnimation*>& animations);
};

// Gesture utility functions
namespace FluentGestureUtils {
    // Gesture detection
    bool isGestureSupported(FluentGestureType gesture);
    QPointF calculateGestureVelocity(const QList<QPointF>& positions, const QList<std::chrono::steady_clock::time_point>& timestamps);
    double calculateGestureDistance(const QPointF& start, const QPointF& end);
    double calculateGestureAngle(const QPointF& start, const QPointF& end);
    
    // Physics calculations
    QPointF calculateSpringForce(const QPointF& position, const QPointF& target, double stiffness);
    QPointF calculateDampingForce(const QPointF& velocity, double damping);
    QPointF integrateMotion(const QPointF& position, const QPointF& velocity, const QPointF& acceleration, std::chrono::milliseconds deltaTime);
    
    // Constraint utilities
    QPointF constrainToRect(const QPointF& position, const QRect& rect);
    QPointF snapToGrid(const QPointF& position, const QSize& gridSize);
    bool isWithinBounds(const QPointF& position, const QRect& bounds);
    
    // Cultural adaptation
    FluentGestureConfig adaptConfigForCulture(const FluentGestureConfig& config, const QLocale& locale);
    std::chrono::milliseconds getCulturalResponseTime(std::chrono::milliseconds baseTime, const QLocale& locale);
    double getCulturalSensitivity(double baseSensitivity, const QLocale& locale);
}

} // namespace FluentQt::Animation
