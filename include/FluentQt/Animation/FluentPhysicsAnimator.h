// include/FluentQt/Animation/FluentPhysicsAnimator.h
#ifndef FLUENTPHYSICSANIMATOR_H
#define FLUENTPHYSICSANIMATOR_H

#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QTimer>
#include <QPointF>
#include <QVector2D>
#include <QEasingCurve>
#include <memory>
#include <chrono>

#include "FluentAnimator.h"

namespace FluentQt::Animation {

// Physics-based animation parameters
struct FluentSpringConfig {
    double stiffness{300.0};        // Spring stiffness (higher = more rigid)
    double damping{30.0};           // Damping coefficient (higher = less oscillation)
    double mass{1.0};               // Mass of the animated object
    double precision{0.01};         // Precision threshold for settling
    double initialVelocity{0.0};    // Initial velocity
    int maxDuration{2000};          // Maximum animation duration (ms)
    
    // Presets for common use cases
    static FluentSpringConfig gentle() {
        return {200.0, 25.0, 1.0, 0.01, 0.0, 1500};
    }
    
    static FluentSpringConfig bouncy() {
        return {400.0, 20.0, 1.0, 0.01, 0.0, 2000};
    }
    
    static FluentSpringConfig stiff() {
        return {500.0, 40.0, 1.0, 0.01, 0.0, 1000};
    }
    
    static FluentSpringConfig wobbly() {
        return {180.0, 12.0, 1.0, 0.01, 0.0, 2500};
    }
};

struct FluentDampingConfig {
    double friction{0.8};           // Friction coefficient (0-1)
    double restitution{0.3};        // Bounce factor (0-1)
    double threshold{0.1};          // Velocity threshold for stopping
    int maxDuration{3000};          // Maximum animation duration (ms)
};

// Gesture-driven animation parameters
struct FluentGestureConfig {
    double sensitivity{1.0};        // Gesture sensitivity multiplier
    double momentum{0.9};           // Momentum preservation (0-1)
    double snapThreshold{50.0};     // Distance threshold for snapping
    bool enableInertia{true};       // Enable inertial scrolling
    bool enableBounce{true};        // Enable boundary bouncing
    QRectF bounds;                  // Gesture boundaries
};

// Advanced physics animator class
class FluentPhysicsAnimator : public QObject {
    Q_OBJECT
    
public:
    explicit FluentPhysicsAnimator(QObject* parent = nullptr);
    ~FluentPhysicsAnimator() = default;
    
    // Spring-based animations
    std::unique_ptr<QPropertyAnimation> springAnimation(
        QObject* target,
        const QByteArray& property,
        const QVariant& from,
        const QVariant& to,
        const FluentSpringConfig& config = FluentSpringConfig::gentle()
    );
    
    std::unique_ptr<QPropertyAnimation> springScale(
        QWidget* target,
        double fromScale,
        double toScale,
        const FluentSpringConfig& config = FluentSpringConfig::bouncy()
    );
    
    std::unique_ptr<QPropertyAnimation> springPosition(
        QWidget* target,
        const QPointF& from,
        const QPointF& to,
        const FluentSpringConfig& config = FluentSpringConfig::gentle()
    );
    
    // Damping-based animations (for drag and drop, momentum scrolling)
    std::unique_ptr<QPropertyAnimation> dampingAnimation(
        QObject* target,
        const QByteArray& property,
        const QVariant& startValue,
        const QVariant& initialVelocity,
        const FluentDampingConfig& config = {}
    );
    
    // Gesture-driven animations
    void startGestureTracking(QWidget* target, const FluentGestureConfig& config = {});
    void updateGesturePosition(const QPointF& position, const QVector2D& velocity = {});
    void endGestureTracking();
    
    // Complex transition sequences
    std::unique_ptr<QSequentialAnimationGroup> sharedElementTransition(
        QWidget* fromElement,
        QWidget* toElement,
        const FluentAnimationConfig& config = {}
    );
    
    std::unique_ptr<QParallelAnimationGroup> morphTransition(
        QWidget* target,
        const QRect& fromGeometry,
        const QRect& toGeometry,
        const QColor& fromColor,
        const QColor& toColor,
        const FluentSpringConfig& springConfig = FluentSpringConfig::gentle()
    );
    
    // Page transition effects
    std::unique_ptr<QSequentialAnimationGroup> pageSlideTransition(
        QWidget* fromPage,
        QWidget* toPage,
        Qt::Orientation direction = Qt::Horizontal,
        const FluentSpringConfig& config = FluentSpringConfig::stiff()
    );
    
    std::unique_ptr<QParallelAnimationGroup> pageFadeTransition(
        QWidget* fromPage,
        QWidget* toPage,
        const FluentAnimationConfig& config = {}
    );
    
    // Utility methods
    static QEasingCurve createSpringEasing(const FluentSpringConfig& config);
    static QEasingCurve createDampingEasing(const FluentDampingConfig& config);
    
    // Performance monitoring
    void setPerformanceMode(bool highPerformance) { m_highPerformanceMode = highPerformance; }
    bool isHighPerformanceMode() const { return m_highPerformanceMode; }
    
signals:
    void gestureStarted(const QPointF& position);
    void gestureUpdated(const QPointF& position, const QVector2D& velocity);
    void gestureEnded(const QPointF& position, const QVector2D& velocity);
    void springAnimationFinished();
    void transitionCompleted();

private slots:
    void updateSpringAnimation();
    void updateDampingAnimation();
    void updateGestureAnimation();

private:
    // Physics calculation methods
    QPointF calculateSpringForce(const QPointF& position, const QPointF& target, 
                                const FluentSpringConfig& config) const;
    QVector2D calculateDamping(const QVector2D& velocity, 
                              const FluentDampingConfig& config) const;
    
    // Animation state management
    struct SpringState {
        QPointF position;
        QVector2D velocity;
        QPointF target;
        FluentSpringConfig config;
        std::chrono::steady_clock::time_point startTime;
        bool active{false};
    };
    
    struct GestureState {
        QWidget* target{nullptr};
        QPointF startPosition;
        QPointF currentPosition;
        QVector2D velocity;
        FluentGestureConfig config;
        bool active{false};
        std::chrono::steady_clock::time_point lastUpdate;
    };
    
    SpringState m_springState;
    GestureState m_gestureState;
    
    QTimer* m_physicsTimer;
    bool m_highPerformanceMode{false};
    
    // Performance optimization
    static constexpr int PHYSICS_UPDATE_INTERVAL = 16; // ~60 FPS
    static constexpr int HIGH_PERF_UPDATE_INTERVAL = 8; // ~120 FPS
};

// Specialized spring animation class for complex behaviors
class FluentSpringAnimation : public QPropertyAnimation {
    Q_OBJECT
    
public:
    explicit FluentSpringAnimation(QObject* target, const QByteArray& property, 
                                  const FluentSpringConfig& config, QObject* parent = nullptr);
    
    void setSpringConfig(const FluentSpringConfig& config);
    FluentSpringConfig springConfig() const { return m_config; }
    
    // Override to implement custom spring physics
    QVariant interpolated(const QVariant& from, const QVariant& to, qreal progress) const override;
    
private:
    FluentSpringConfig m_config;
    mutable double m_currentVelocity{0.0};
    mutable std::chrono::steady_clock::time_point m_lastTime;
};

// Gesture recognizer for advanced touch interactions
class FluentGestureRecognizer : public QObject {
    Q_OBJECT
    
public:
    enum GestureType {
        Pan,
        Pinch,
        Rotate,
        Swipe,
        Fling
    };
    
    explicit FluentGestureRecognizer(QWidget* target, QObject* parent = nullptr);
    
    void setGestureEnabled(GestureType type, bool enabled);
    bool isGestureEnabled(GestureType type) const;
    
    void setSensitivity(GestureType type, double sensitivity);
    double sensitivity(GestureType type) const;

signals:
    void gestureStarted(GestureType type, const QPointF& position);
    void gestureUpdated(GestureType type, const QPointF& position, const QVariant& data);
    void gestureFinished(GestureType type, const QPointF& position, const QVariant& data);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    QWidget* m_target;
    QMap<GestureType, bool> m_enabledGestures;
    QMap<GestureType, double> m_sensitivities;
    
    // Gesture state tracking
    struct GestureData {
        bool active{false};
        QPointF startPosition;
        QPointF lastPosition;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point lastTime;
        QVariant customData;
    };
    
    QMap<GestureType, GestureData> m_gestureStates;
    
    void processMouseEvent(QMouseEvent* event);
    void processTouchEvent(QTouchEvent* event);
    void processWheelEvent(QWheelEvent* event);
    
    GestureType detectGestureType(const QPointF& start, const QPointF& current, 
                                 const QVector2D& velocity) const;
};

} // namespace FluentQt::Animation

#endif // FLUENTPHYSICSANIMATOR_H
