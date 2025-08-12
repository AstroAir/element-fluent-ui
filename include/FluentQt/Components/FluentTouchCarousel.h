// include/FluentQt/Components/FluentTouchCarousel.h
#pragma once

#include <QEasingCurve>
#include <QGestureEvent>
#include <QPanGesture>
#include <QPropertyAnimation>
#include <QScroller>
#include <QScrollerProperties>
#include <QSwipeGesture>
#include <QTouchEvent>
#include <chrono>
#include "FluentQt/Components/FluentCarousel.h"

namespace FluentQt::Components {

/**
 * @brief Touch gesture types for carousel interaction
 */
enum class FluentCarouselTouchGesture {
    None,       // No gesture detected
    Pan,        // Pan/drag gesture
    Swipe,      // Quick swipe gesture
    Pinch,      // Pinch gesture (for zoom)
    Tap,        // Single tap
    DoubleTap,  // Double tap
    LongPress   // Long press gesture
};

/**
 * @brief Touch feedback types
 */
enum class FluentCarouselTouchFeedback {
    None,     // No feedback
    Haptic,   // Haptic feedback (if supported)
    Visual,   // Visual feedback (highlight, ripple)
    Audio,    // Audio feedback
    Combined  // Multiple feedback types
};

/**
 * @brief Edge behavior when swiping beyond bounds
 */
enum class FluentCarouselEdgeBehavior {
    Bounce,  // Bounce back to bounds
    Resist,  // Resistance when approaching edge
    Stop,    // Hard stop at edges
    Wrap     // Wrap around to other end
};

/**
 * @brief Touch configuration structure
 */
struct FluentCarouselTouchConfig {
    // Gesture sensitivity
    qreal swipeVelocityThreshold{500.0};  // Pixels per second
    qreal swipeDistanceThreshold{50.0};   // Minimum distance in pixels
    qreal panThreshold{10.0};             // Minimum pan distance

    // Timing thresholds
    std::chrono::milliseconds tapTimeout{200};
    std::chrono::milliseconds doubleTapTimeout{300};
    std::chrono::milliseconds longPressTimeout{500};

    // Physics simulation
    qreal friction{0.85};           // Deceleration friction
    qreal elasticity{0.3};          // Bounce elasticity
    qreal maximumVelocity{2000.0};  // Maximum scroll velocity

    // Edge behavior
    FluentCarouselEdgeBehavior edgeBehavior{FluentCarouselEdgeBehavior::Bounce};
    qreal edgeResistance{0.5};  // Resistance factor at edges

    // Feedback
    FluentCarouselTouchFeedback feedback{FluentCarouselTouchFeedback::Visual};
    bool enableHapticFeedback{true};

    // Accessibility
    bool enableTouchAccessibility{true};
    qreal accessibilityScaleFactor{1.2};  // Scale factor for accessibility

    FluentCarouselTouchConfig() = default;
};

/**
 * @brief Carousel with touch/swipe gesture support for mobile devices
 *
 * FluentTouchCarousel provides comprehensive touch interaction with:
 * - Multi-touch gesture recognition (pan, swipe, pinch)
 * - Momentum scrolling with physics simulation
 * - Edge resistance and bounce effects
 * - Haptic and visual feedback
 * - Accessibility support for touch devices
 * - Responsive touch handling across screen sizes
 * - Performance optimizations for smooth scrolling
 */
class FluentTouchCarousel : public FluentCarousel {
    Q_OBJECT
    Q_PROPERTY(bool touchEnabled READ isTouchEnabled WRITE setTouchEnabled
                   NOTIFY touchEnabledChanged)
    Q_PROPERTY(bool gestureEnabled READ isGestureEnabled WRITE setGestureEnabled
                   NOTIFY gestureEnabledChanged)
    Q_PROPERTY(
        qreal swipeVelocityThreshold READ swipeVelocityThreshold WRITE
            setSwipeVelocityThreshold NOTIFY swipeVelocityThresholdChanged)
    Q_PROPERTY(
        qreal swipeDistanceThreshold READ swipeDistanceThreshold WRITE
            setSwipeDistanceThreshold NOTIFY swipeDistanceThresholdChanged)
    Q_PROPERTY(FluentCarouselEdgeBehavior edgeBehavior READ edgeBehavior WRITE
                   setEdgeBehavior NOTIFY edgeBehaviorChanged)
    Q_PROPERTY(FluentCarouselTouchFeedback touchFeedback READ touchFeedback
                   WRITE setTouchFeedback NOTIFY touchFeedbackChanged)
    Q_PROPERTY(bool momentumScrolling READ momentumScrolling WRITE
                   setMomentumScrolling NOTIFY momentumScrollingChanged)
    Q_PROPERTY(
        qreal currentOffset READ currentOffset NOTIFY currentOffsetChanged)

public:
    explicit FluentTouchCarousel(QWidget* parent = nullptr);
    explicit FluentTouchCarousel(const FluentCarouselConfig& config,
                                 QWidget* parent = nullptr);
    explicit FluentTouchCarousel(const FluentCarouselTouchConfig& touchConfig,
                                 QWidget* parent = nullptr);
    ~FluentTouchCarousel() override = default;

    // Touch configuration
    const FluentCarouselTouchConfig& touchConfig() const noexcept {
        return m_touchConfig;
    }
    void setTouchConfig(const FluentCarouselTouchConfig& config);

    // Touch properties
    bool isTouchEnabled() const noexcept { return m_touchEnabled; }
    void setTouchEnabled(bool enabled);

    bool isGestureEnabled() const noexcept { return m_gestureEnabled; }
    void setGestureEnabled(bool enabled);

    // Gesture thresholds
    qreal swipeVelocityThreshold() const noexcept {
        return m_touchConfig.swipeVelocityThreshold;
    }
    void setSwipeVelocityThreshold(qreal threshold);

    qreal swipeDistanceThreshold() const noexcept {
        return m_touchConfig.swipeDistanceThreshold;
    }
    void setSwipeDistanceThreshold(qreal threshold);

    // Edge behavior
    FluentCarouselEdgeBehavior edgeBehavior() const noexcept {
        return m_touchConfig.edgeBehavior;
    }
    void setEdgeBehavior(FluentCarouselEdgeBehavior behavior);

    // Feedback
    FluentCarouselTouchFeedback touchFeedback() const noexcept {
        return m_touchConfig.feedback;
    }
    void setTouchFeedback(FluentCarouselTouchFeedback feedback);

    // Momentum scrolling
    bool momentumScrolling() const noexcept { return m_momentumScrolling; }
    void setMomentumScrolling(bool enabled);

    // Current state
    qreal currentOffset() const noexcept { return m_currentOffset; }
    bool isTouchActive() const noexcept { return m_touchActive; }
    bool isMomentumActive() const noexcept { return m_momentumActive; }

    // Convenience factory methods
    static FluentTouchCarousel* createMobile(QWidget* parent = nullptr);
    static FluentTouchCarousel* createTablet(QWidget* parent = nullptr);
    static FluentTouchCarousel* createDesktop(QWidget* parent = nullptr);

public slots:
    void resetTouch();
    void stopMomentum();
    void enableTouchFeedback(bool enabled);
    void calibrateTouchSensitivity();

signals:
    void touchEnabledChanged(bool enabled);
    void gestureEnabledChanged(bool enabled);
    void swipeVelocityThresholdChanged(qreal threshold);
    void swipeDistanceThresholdChanged(qreal threshold);
    void edgeBehaviorChanged(FluentCarouselEdgeBehavior behavior);
    void touchFeedbackChanged(FluentCarouselTouchFeedback feedback);
    void momentumScrollingChanged(bool enabled);
    void currentOffsetChanged(qreal offset);

    // Touch interaction signals
    void touchStarted(const QPointF& position);
    void touchMoved(const QPointF& position, qreal velocity);
    void touchEnded(const QPointF& position, qreal velocity);
    void gestureDetected(FluentCarouselTouchGesture gesture, qreal velocity);
    void swipePerformed(Qt::GestureType direction, qreal velocity);
    void edgeReached(bool atStart, bool atEnd);
    void momentumStarted(qreal velocity);
    void momentumFinished();

protected:
    // Event handling
    bool event(QEvent* event) override;
    void touchEvent(QTouchEvent* event);
    bool gestureEvent(QGestureEvent* event);
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onMomentumAnimationValueChanged(const QVariant& value);
    void onMomentumAnimationFinished();
    void onScrollerStateChanged(QScroller::State state);
    void onTouchFeedbackTimer();

private:
    void initializeTouchCarousel();
    void setupGestureRecognition();
    void setupMomentumScrolling();
    void setupTouchFeedback();

    // Touch processing
    void processTouchBegin(QTouchEvent* event);
    void processTouchUpdate(QTouchEvent* event);
    void processTouchEnd(QTouchEvent* event);
    void calculateTouchVelocity();
    void updateTouchOffset(qreal offset);

    // Gesture handling
    void handlePanGesture(QPanGesture* gesture);
    void handleSwipeGesture(QSwipeGesture* gesture);
    void handleTapGesture(const QPointF& position);
    void handleDoubleTapGesture(const QPointF& position);
    void handleLongPressGesture(const QPointF& position);

    // Physics simulation
    void startMomentumAnimation(qreal velocity);
    void applyEdgeResistance(qreal& offset);
    void handleEdgeBounce();
    qreal calculateDeceleration(qreal velocity);

    // Feedback
    void provideTouchFeedback(FluentCarouselTouchGesture gesture);
    void provideHapticFeedback();
    void provideVisualFeedback(const QPointF& position);
    void provideAudioFeedback();

    // Utility methods
    qreal normalizeOffset(qreal offset) const;
    int offsetToIndex(qreal offset) const;
    qreal indexToOffset(int index) const;
    bool isAtEdge() const;
    void updateAccessibilityInfo();

private:
    // Configuration
    FluentCarouselTouchConfig m_touchConfig;
    bool m_touchEnabled{true};
    bool m_gestureEnabled{true};
    bool m_momentumScrolling{true};

    // Touch state
    bool m_touchActive{false};
    bool m_momentumActive{false};
    QPointF m_touchStartPos;
    QPointF m_touchCurrentPos;
    QPointF m_touchLastPos;
    qreal m_currentOffset{0.0};
    qreal m_touchVelocity{0.0};
    std::chrono::steady_clock::time_point m_touchStartTime;
    std::chrono::steady_clock::time_point m_touchLastTime;

    // Gesture recognition
    FluentCarouselTouchGesture m_currentGesture{
        FluentCarouselTouchGesture::None};
    bool m_gestureRecognized{false};
    QPointF m_gestureStartPos;
    std::chrono::steady_clock::time_point m_gestureStartTime;

    // Physics simulation
    std::unique_ptr<QPropertyAnimation> m_momentumAnimation;
    QScroller* m_scroller{nullptr};
    qreal m_targetOffset{0.0};

    // Feedback
    std::unique_ptr<QTimer> m_feedbackTimer;
    QPointF m_feedbackPosition;
    bool m_feedbackActive{false};

    // Performance optimization
    bool m_updateScheduled{false};
    qreal m_lastRenderedOffset{0.0};
};

}  // namespace FluentQt::Components
