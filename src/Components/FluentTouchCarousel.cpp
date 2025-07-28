// src/Components/FluentTouchCarousel.cpp
#include "FluentQt/Components/FluentTouchCarousel.h"
#include "FluentQt/Styling/FluentTheme.h"
#include <QTouchEvent>
#include <QGestureEvent>
#include <QPanGesture>
#include <QSwipeGesture>
#include <QScroller>
#include <QScrollerProperties>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTimer>
#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include <QDebug>
#include <cmath>
#include <algorithm>

namespace FluentQt::Components {

FluentTouchCarousel::FluentTouchCarousel(QWidget* parent)
    : FluentCarousel(parent)
    , m_momentumAnimation(std::make_unique<QPropertyAnimation>(this))
    , m_feedbackTimer(std::make_unique<QTimer>(this))
{
    initializeTouchCarousel();
}

FluentTouchCarousel::FluentTouchCarousel(const FluentCarouselConfig& config, QWidget* parent)
    : FluentCarousel(config, parent)
    , m_momentumAnimation(std::make_unique<QPropertyAnimation>(this))
    , m_feedbackTimer(std::make_unique<QTimer>(this))
{
    initializeTouchCarousel();
}

FluentTouchCarousel::FluentTouchCarousel(const FluentCarouselTouchConfig& touchConfig, QWidget* parent)
    : FluentCarousel(parent)
    , m_touchConfig(touchConfig)
    , m_momentumAnimation(std::make_unique<QPropertyAnimation>(this))
    , m_feedbackTimer(std::make_unique<QTimer>(this))
{
    initializeTouchCarousel();
}

void FluentTouchCarousel::initializeTouchCarousel() {
    // Set default configuration for touch carousel
    FluentCarouselConfig config = this->config();
    config.enableTouch = true;
    config.enableKeyboard = true;
    config.enableWheel = true;
    config.pauseOnHover = false; // Don't pause on hover for touch devices
    setConfig(config);
    
    setupGestureRecognition();
    setupMomentumScrolling();
    setupTouchFeedback();
    
    // Connect signals
    connect(this, &FluentCarousel::currentIndexChanged,
            this, &FluentTouchCarousel::currentOffsetChanged);
    
    updateAccessibilityInfo();
}

void FluentTouchCarousel::setupGestureRecognition() {
    if (m_gestureEnabled) {
        grabGesture(Qt::PanGesture);
        grabGesture(Qt::SwipeGesture);
        grabGesture(Qt::TapGesture);
        grabGesture(Qt::TapAndHoldGesture);
    }
    
    setAttribute(Qt::WA_AcceptTouchEvents, m_touchEnabled);
}

void FluentTouchCarousel::setupMomentumScrolling() {
    // Setup momentum animation
    m_momentumAnimation->setTargetObject(this);
    m_momentumAnimation->setPropertyName("currentOffset");
    m_momentumAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    connect(m_momentumAnimation.get(), &QPropertyAnimation::valueChanged,
            this, &FluentTouchCarousel::onMomentumAnimationValueChanged);
    connect(m_momentumAnimation.get(), &QPropertyAnimation::finished,
            this, &FluentTouchCarousel::onMomentumAnimationFinished);
    
    // Setup QScroller for advanced momentum
    if (m_momentumScrolling) {
        m_scroller = QScroller::scroller(this);
        if (m_scroller) {
            QScrollerProperties properties = m_scroller->scrollerProperties();
            
            // Configure physics
            properties.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor, m_touchConfig.friction);
            properties.setScrollMetric(QScrollerProperties::MinimumVelocity, 0.1);
            properties.setScrollMetric(QScrollerProperties::MaximumVelocity, m_touchConfig.maximumVelocity);
            properties.setScrollMetric(QScrollerProperties::AcceleratingFlickMaximumTime, 0.5);
            properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
            properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
            
            m_scroller->setScrollerProperties(properties);
            
            connect(m_scroller, &QScroller::stateChanged,
                    this, &FluentTouchCarousel::onScrollerStateChanged);
        }
    }
}

void FluentTouchCarousel::setupTouchFeedback() {
    // Setup feedback timer
    m_feedbackTimer->setSingleShot(true);
    m_feedbackTimer->setInterval(50);
    connect(m_feedbackTimer.get(), &QTimer::timeout,
            this, &FluentTouchCarousel::onTouchFeedbackTimer);
}

void FluentTouchCarousel::setTouchConfig(const FluentCarouselTouchConfig& config) {
    m_touchConfig = config;
    
    // Update gesture recognition
    setupGestureRecognization();
    
    // Update momentum scrolling
    setupMomentumScrolling();
    
    // Update accessibility
    updateAccessibilityInfo();
}

void FluentTouchCarousel::setTouchEnabled(bool enabled) {
    if (m_touchEnabled != enabled) {
        m_touchEnabled = enabled;
        setAttribute(Qt::WA_AcceptTouchEvents, enabled);
        emit touchEnabledChanged(enabled);
    }
}

void FluentTouchCarousel::setGestureEnabled(bool enabled) {
    if (m_gestureEnabled != enabled) {
        m_gestureEnabled = enabled;
        setupGestureRecognition();
        emit gestureEnabledChanged(enabled);
    }
}

void FluentTouchCarousel::setSwipeVelocityThreshold(qreal threshold) {
    if (m_touchConfig.swipeVelocityThreshold != threshold) {
        m_touchConfig.swipeVelocityThreshold = threshold;
        emit swipeVelocityThresholdChanged(threshold);
    }
}

void FluentTouchCarousel::setSwipeDistanceThreshold(qreal threshold) {
    if (m_touchConfig.swipeDistanceThreshold != threshold) {
        m_touchConfig.swipeDistanceThreshold = threshold;
        emit swipeDistanceThresholdChanged(threshold);
    }
}

void FluentTouchCarousel::setEdgeBehavior(FluentCarouselEdgeBehavior behavior) {
    if (m_touchConfig.edgeBehavior != behavior) {
        m_touchConfig.edgeBehavior = behavior;
        emit edgeBehaviorChanged(behavior);
    }
}

void FluentTouchCarousel::setTouchFeedback(FluentCarouselTouchFeedback feedback) {
    if (m_touchConfig.feedback != feedback) {
        m_touchConfig.feedback = feedback;
        emit touchFeedbackChanged(feedback);
    }
}

void FluentTouchCarousel::setMomentumScrolling(bool enabled) {
    if (m_momentumScrolling != enabled) {
        m_momentumScrolling = enabled;
        setupMomentumScrolling();
        emit momentumScrollingChanged(enabled);
    }
}

// Factory methods
FluentTouchCarousel* FluentTouchCarousel::createMobile(QWidget* parent) {
    FluentCarouselTouchConfig touchConfig;
    touchConfig.swipeVelocityThreshold = 300.0;
    touchConfig.swipeDistanceThreshold = 30.0;
    touchConfig.panThreshold = 5.0;
    touchConfig.feedback = FluentCarouselTouchFeedback::Combined;
    touchConfig.enableHapticFeedback = true;
    
    return new FluentTouchCarousel(touchConfig, parent);
}

FluentTouchCarousel* FluentTouchCarousel::createTablet(QWidget* parent) {
    FluentCarouselTouchConfig touchConfig;
    touchConfig.swipeVelocityThreshold = 400.0;
    touchConfig.swipeDistanceThreshold = 40.0;
    touchConfig.panThreshold = 8.0;
    touchConfig.feedback = FluentCarouselTouchFeedback::Visual;
    touchConfig.enableHapticFeedback = false;
    
    return new FluentTouchCarousel(touchConfig, parent);
}

FluentTouchCarousel* FluentTouchCarousel::createDesktop(QWidget* parent) {
    FluentCarouselTouchConfig touchConfig;
    touchConfig.swipeVelocityThreshold = 500.0;
    touchConfig.swipeDistanceThreshold = 50.0;
    touchConfig.panThreshold = 10.0;
    touchConfig.feedback = FluentCarouselTouchFeedback::Visual;
    touchConfig.enableHapticFeedback = false;
    
    return new FluentTouchCarousel(touchConfig, parent);
}

// Public slots
void FluentTouchCarousel::resetTouch() {
    m_touchActive = false;
    m_momentumActive = false;
    m_currentOffset = 0.0;
    m_touchVelocity = 0.0;
    m_currentGesture = FluentCarouselTouchGesture::None;
    m_gestureRecognized = false;
    
    if (m_momentumAnimation->state() == QPropertyAnimation::Running) {
        m_momentumAnimation->stop();
    }
    
    update();
}

void FluentTouchCarousel::stopMomentum() {
    if (m_momentumActive) {
        m_momentumActive = false;
        
        if (m_momentumAnimation->state() == QPropertyAnimation::Running) {
            m_momentumAnimation->stop();
        }
        
        if (m_scroller && m_scroller->state() != QScroller::Inactive) {
            m_scroller->stop();
        }
        
        emit momentumFinished();
    }
}

void FluentTouchCarousel::enableTouchFeedback(bool enabled) {
    m_touchConfig.enableHapticFeedback = enabled;
}

void FluentTouchCarousel::calibrateTouchSensitivity() {
    // Auto-calibrate touch sensitivity based on device characteristics
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) return;
    
    qreal dpi = screen->physicalDotsPerInch();
    qreal scaleFactor = dpi / 96.0; // 96 DPI is standard
    
    // Adjust thresholds based on DPI
    m_touchConfig.swipeDistanceThreshold *= scaleFactor;
    m_touchConfig.panThreshold *= scaleFactor;
    
    // Adjust for accessibility if needed
    if (m_touchConfig.enableTouchAccessibility) {
        m_touchConfig.swipeDistanceThreshold *= m_touchConfig.accessibilityScaleFactor;
        m_touchConfig.panThreshold *= m_touchConfig.accessibilityScaleFactor;
    }
}

// Event handling
bool FluentTouchCarousel::event(QEvent* event) {
    switch (event->type()) {
    case QEvent::Gesture:
        return gestureEvent(static_cast<QGestureEvent*>(event));
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    case QEvent::TouchCancel:
        touchEvent(static_cast<QTouchEvent*>(event));
        return true;
    default:
        return FluentCarousel::event(event);
    }
}

void FluentTouchCarousel::touchEvent(QTouchEvent* event) {
    if (!m_touchEnabled) {
        FluentCarousel::touchEvent(event);
        return;
    }
    
    const QList<QTouchEvent::TouchPoint>& touchPoints = event->points();
    if (touchPoints.isEmpty()) {
        return;
    }
    
    switch (event->type()) {
    case QEvent::TouchBegin:
        processTouchBegin(event);
        break;
    case QEvent::TouchUpdate:
        processTouchUpdate(event);
        break;
    case QEvent::TouchEnd:
    case QEvent::TouchCancel:
        processTouchEnd(event);
        break;
    default:
        break;
    }
    
    event->accept();
}

bool FluentTouchCarousel::gestureEvent(QGestureEvent* event) {
    if (!m_gestureEnabled) {
        return false;
    }
    
    bool handled = false;
    
    if (QGesture* pan = event->gesture(Qt::PanGesture)) {
        handlePanGesture(static_cast<QPanGesture*>(pan));
        handled = true;
    }
    
    if (QGesture* swipe = event->gesture(Qt::SwipeGesture)) {
        handleSwipeGesture(static_cast<QSwipeGesture*>(swipe));
        handled = true;
    }
    
    if (QGesture* tap = event->gesture(Qt::TapGesture)) {
        handleTapGesture(tap->position());
        handled = true;
    }
    
    if (QGesture* longPress = event->gesture(Qt::TapAndHoldGesture)) {
        handleLongPressGesture(longPress->position());
        handled = true;
    }
    
    return handled;
}

void FluentTouchCarousel::mousePressEvent(QMouseEvent* event) {
    if (m_touchEnabled && event->button() == Qt::LeftButton) {
        // Treat mouse as touch for desktop compatibility
        m_touchStartPos = event->position();
        m_touchCurrentPos = event->position();
        m_touchLastPos = event->position();
        m_touchActive = true;
        m_touchStartTime = std::chrono::steady_clock::now();
        m_touchLastTime = m_touchStartTime;
        
        emit touchStarted(m_touchStartPos);
        
        // Stop any ongoing momentum
        stopMomentum();
        
        // Provide touch feedback
        provideTouchFeedback(FluentCarouselTouchGesture::Tap);
    }
    
    FluentCarousel::mousePressEvent(event);
}

void FluentTouchCarousel::mouseMoveEvent(QMouseEvent* event) {
    if (m_touchActive) {
        m_touchCurrentPos = event->position();
        
        // Calculate velocity
        calculateTouchVelocity();
        
        // Update offset
        QPointF delta = m_touchCurrentPos - m_touchStartPos;
        qreal distance = (config().orientation == FluentCarouselOrientation::Horizontal) 
                        ? delta.x() : delta.y();
        
        updateTouchOffset(distance);
        
        // Detect gesture
        if (!m_gestureRecognized && std::abs(distance) > m_touchConfig.panThreshold) {
            m_currentGesture = FluentCarouselTouchGesture::Pan;
            m_gestureRecognized = true;
            emit gestureDetected(m_currentGesture, m_touchVelocity);
        }
        
        emit touchMoved(m_touchCurrentPos, m_touchVelocity);
        
        m_touchLastPos = m_touchCurrentPos;
        m_touchLastTime = std::chrono::steady_clock::now();
    }
    
    FluentCarousel::mouseMoveEvent(event);
}

void FluentTouchCarousel::mouseReleaseEvent(QMouseEvent* event) {
    if (m_touchActive && event->button() == Qt::LeftButton) {
        m_touchActive = false;

        // Calculate final velocity
        calculateTouchVelocity();

        // Determine if this was a swipe
        QPointF delta = m_touchCurrentPos - m_touchStartPos;
        qreal distance = (config().orientation == FluentCarouselOrientation::Horizontal)
                        ? delta.x() : delta.y();

        bool isSwipe = (std::abs(distance) > m_touchConfig.swipeDistanceThreshold &&
                       std::abs(m_touchVelocity) > m_touchConfig.swipeVelocityThreshold);

        if (isSwipe) {
            // Handle swipe
            if (distance > 0) {
                goToPrevious();
            } else {
                goToNext();
            }

            emit swipePerformed(distance > 0 ? Qt::SwipeGesture : Qt::SwipeGesture, m_touchVelocity);
            provideTouchFeedback(FluentCarouselTouchGesture::Swipe);
        } else if (m_momentumScrolling && std::abs(m_touchVelocity) > 100) {
            // Start momentum scrolling
            startMomentumAnimation(m_touchVelocity);
        }

        emit touchEnded(m_touchCurrentPos, m_touchVelocity);

        // Reset gesture state
        m_currentGesture = FluentCarouselTouchGesture::None;
        m_gestureRecognized = false;
        m_currentOffset = 0.0;

        update();
    }

    FluentCarousel::mouseReleaseEvent(event);
}

void FluentTouchCarousel::wheelEvent(QWheelEvent* event) {
    if (m_touchEnabled) {
        // Handle wheel as touch input
        QPoint delta = event->angleDelta();
        qreal distance = (config().orientation == FluentCarouselOrientation::Horizontal)
                        ? delta.x() : delta.y();

        if (std::abs(distance) >= 120) { // Standard wheel step
            if (distance > 0) {
                goToPrevious();
            } else {
                goToNext();
            }

            provideTouchFeedback(FluentCarouselTouchGesture::Swipe);
        }

        event->accept();
    } else {
        FluentCarousel::wheelEvent(event);
    }
}

void FluentTouchCarousel::resizeEvent(QResizeEvent* event) {
    FluentCarousel::resizeEvent(event);

    // Recalibrate touch sensitivity for new size
    calibrateTouchSensitivity();
}

void FluentTouchCarousel::paintEvent(QPaintEvent* event) {
    FluentCarousel::paintEvent(event);

    // Paint touch feedback if active
    if (m_feedbackActive && m_touchConfig.feedback == FluentCarouselTouchFeedback::Visual) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draw ripple effect at feedback position
        QColor rippleColor = Styling::FluentTheme::instance().color("accent");
        rippleColor.setAlpha(100);

        painter.setBrush(rippleColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(m_feedbackPosition, 20, 20);
    }

    // Paint current offset visualization during drag
    if (m_touchActive && std::abs(m_currentOffset) > 0) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draw offset indicator
        QColor offsetColor = Styling::FluentTheme::instance().color("accent");
        offsetColor.setAlpha(50);

        QRect offsetRect = rect();
        if (config().orientation == FluentCarouselOrientation::Horizontal) {
            offsetRect.translate(static_cast<int>(m_currentOffset), 0);
        } else {
            offsetRect.translate(0, static_cast<int>(m_currentOffset));
        }

        painter.fillRect(offsetRect, offsetColor);
    }
}

// Private slot implementations
void FluentTouchCarousel::onMomentumAnimationValueChanged(const QVariant& value) {
    m_currentOffset = value.toReal();
    emit currentOffsetChanged(m_currentOffset);
    update();
}

void FluentTouchCarousel::onMomentumAnimationFinished() {
    m_momentumActive = false;
    m_currentOffset = 0.0;
    emit momentumFinished();
    emit currentOffsetChanged(0.0);
}

void FluentTouchCarousel::onScrollerStateChanged(QScroller::State state) {
    switch (state) {
    case QScroller::Inactive:
        m_momentumActive = false;
        emit momentumFinished();
        break;
    case QScroller::Pressed:
        // Touch started
        break;
    case QScroller::Dragging:
        m_momentumActive = true;
        break;
    case QScroller::Scrolling:
        if (!m_momentumActive) {
            m_momentumActive = true;
            emit momentumStarted(m_touchVelocity);
        }
        break;
    }
}

void FluentTouchCarousel::onTouchFeedbackTimer() {
    m_feedbackActive = false;
    update();
}

// Private helper methods
void FluentTouchCarousel::processTouchBegin(QTouchEvent* event) {
    const QList<QTouchEvent::TouchPoint>& touchPoints = event->points();
    if (touchPoints.isEmpty()) return;

    const QTouchEvent::TouchPoint& point = touchPoints.first();
    m_touchStartPos = point.position();
    m_touchCurrentPos = point.position();
    m_touchLastPos = point.position();
    m_touchActive = true;
    m_touchStartTime = std::chrono::steady_clock::now();
    m_touchLastTime = m_touchStartTime;
    m_gestureStartPos = point.position();
    m_gestureStartTime = m_touchStartTime;

    emit touchStarted(m_touchStartPos);

    // Stop any ongoing momentum
    stopMomentum();

    // Provide touch feedback
    provideTouchFeedback(FluentCarouselTouchGesture::Tap);
}

void FluentTouchCarousel::processTouchUpdate(QTouchEvent* event) {
    const QList<QTouchEvent::TouchPoint>& touchPoints = event->points();
    if (touchPoints.isEmpty() || !m_touchActive) return;

    const QTouchEvent::TouchPoint& point = touchPoints.first();
    m_touchCurrentPos = point.position();

    // Calculate velocity
    calculateTouchVelocity();

    // Update offset
    QPointF delta = m_touchCurrentPos - m_touchStartPos;
    qreal distance = (config().orientation == FluentCarouselOrientation::Horizontal)
                    ? delta.x() : delta.y();

    updateTouchOffset(distance);

    // Detect gesture
    if (!m_gestureRecognized) {
        QPointF gestureDelta = m_touchCurrentPos - m_gestureStartPos;
        qreal gestureDistance = std::sqrt(gestureDelta.x() * gestureDelta.x() + gestureDelta.y() * gestureDelta.y());

        if (gestureDistance > m_touchConfig.panThreshold) {
            m_currentGesture = FluentCarouselTouchGesture::Pan;
            m_gestureRecognized = true;
            emit gestureDetected(m_currentGesture, m_touchVelocity);
        }
    }

    emit touchMoved(m_touchCurrentPos, m_touchVelocity);

    m_touchLastPos = m_touchCurrentPos;
    m_touchLastTime = std::chrono::steady_clock::now();
}

void FluentTouchCarousel::processTouchEnd(QTouchEvent* event) {
    if (!m_touchActive) return;

    const QList<QTouchEvent::TouchPoint>& touchPoints = event->points();
    if (!touchPoints.isEmpty()) {
        const QTouchEvent::TouchPoint& point = touchPoints.first();
        m_touchCurrentPos = point.position();
    }

    m_touchActive = false;

    // Calculate final velocity
    calculateTouchVelocity();

    // Determine gesture type and handle accordingly
    QPointF delta = m_touchCurrentPos - m_touchStartPos;
    qreal distance = (config().orientation == FluentCarouselOrientation::Horizontal)
                    ? delta.x() : delta.y();

    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_touchStartTime);

    // Check for different gesture types
    if (duration < m_touchConfig.tapTimeout && std::abs(distance) < m_touchConfig.panThreshold) {
        // Tap gesture
        handleTapGesture(m_touchCurrentPos);
    } else if (std::abs(distance) > m_touchConfig.swipeDistanceThreshold &&
               std::abs(m_touchVelocity) > m_touchConfig.swipeVelocityThreshold) {
        // Swipe gesture
        if (distance > 0) {
            goToPrevious();
        } else {
            goToNext();
        }

        emit swipePerformed(distance > 0 ? Qt::SwipeGesture : Qt::SwipeGesture, m_touchVelocity);
        provideTouchFeedback(FluentCarouselTouchGesture::Swipe);
    } else if (m_momentumScrolling && std::abs(m_touchVelocity) > 100) {
        // Start momentum scrolling
        startMomentumAnimation(m_touchVelocity);
    }

    emit touchEnded(m_touchCurrentPos, m_touchVelocity);

    // Reset gesture state
    m_currentGesture = FluentCarouselTouchGesture::None;
    m_gestureRecognized = false;
    m_currentOffset = 0.0;

    update();
}

void FluentTouchCarousel::calculateTouchVelocity() {
    auto now = std::chrono::steady_clock::now();
    auto timeDelta = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_touchLastTime);

    if (timeDelta.count() > 0) {
        QPointF posDelta = m_touchCurrentPos - m_touchLastPos;
        qreal distance = (config().orientation == FluentCarouselOrientation::Horizontal)
                        ? posDelta.x() : posDelta.y();

        m_touchVelocity = distance / timeDelta.count() * 1000.0; // pixels per second
    }
}

void FluentTouchCarousel::updateTouchOffset(qreal offset) {
    // Apply edge resistance
    applyEdgeResistance(offset);

    m_currentOffset = offset;
    emit currentOffsetChanged(offset);

    // Check for edge conditions
    if (isAtEdge()) {
        emit edgeReached(currentIndex() == 0, currentIndex() == itemCount() - 1);
    }

    update();
}

// Gesture handling methods
void FluentTouchCarousel::handlePanGesture(QPanGesture* gesture) {
    QPointF delta = gesture->delta();
    qreal distance = (config().orientation == FluentCarouselOrientation::Horizontal)
                    ? delta.x() : delta.y();

    switch (gesture->state()) {
    case Qt::NoGesture:
        break;

    case Qt::GestureStarted:
        m_currentGesture = FluentCarouselTouchGesture::Pan;
        stopMomentum();
        break;

    case Qt::GestureUpdated:
        updateTouchOffset(distance);
        break;

    case Qt::GestureFinished:
        if (std::abs(distance) > m_touchConfig.swipeDistanceThreshold) {
            if (distance > 0) {
                goToPrevious();
            } else {
                goToNext();
            }
        }
        m_currentGesture = FluentCarouselTouchGesture::None;
        break;

    case Qt::GestureCanceled:
        m_currentGesture = FluentCarouselTouchGesture::None;
        m_currentOffset = 0.0;
        update();
        break;
    }
}

void FluentTouchCarousel::handleSwipeGesture(QSwipeGesture* gesture) {
    if (gesture->state() != Qt::GestureFinished) {
        return;
    }

    QSwipeGesture::SwipeDirection direction = gesture->horizontalDirection();
    if (config().orientation == FluentCarouselOrientation::Vertical) {
        direction = gesture->verticalDirection();
    }

    switch (direction) {
    case QSwipeGesture::Left:
    case QSwipeGesture::Up:
        goToNext();
        break;
    case QSwipeGesture::Right:
    case QSwipeGesture::Down:
        goToPrevious();
        break;
    default:
        return;
    }

    emit swipePerformed(Qt::SwipeGesture, 0.0);
    provideTouchFeedback(FluentCarouselTouchGesture::Swipe);
}

void FluentTouchCarousel::handleTapGesture(const QPointF& position) {
    Q_UNUSED(position)
    m_currentGesture = FluentCarouselTouchGesture::Tap;

    // Simple tap - could be used for play/pause or other actions
    emit gestureDetected(m_currentGesture, 0.0);
    provideTouchFeedback(FluentCarouselTouchGesture::Tap);

    m_currentGesture = FluentCarouselTouchGesture::None;
}

void FluentTouchCarousel::handleDoubleTapGesture(const QPointF& position) {
    Q_UNUSED(position)
    m_currentGesture = FluentCarouselTouchGesture::DoubleTap;

    // Double tap - could be used for zoom or other actions
    emit gestureDetected(m_currentGesture, 0.0);
    provideTouchFeedback(FluentCarouselTouchGesture::DoubleTap);

    m_currentGesture = FluentCarouselTouchGesture::None;
}

void FluentTouchCarousel::handleLongPressGesture(const QPointF& position) {
    Q_UNUSED(position)
    m_currentGesture = FluentCarouselTouchGesture::LongPress;

    // Long press - could be used for context menu or other actions
    emit gestureDetected(m_currentGesture, 0.0);
    provideTouchFeedback(FluentCarouselTouchGesture::LongPress);

    m_currentGesture = FluentCarouselTouchGesture::None;
}

// Physics simulation methods
void FluentTouchCarousel::startMomentumAnimation(qreal velocity) {
    if (!m_momentumScrolling || std::abs(velocity) < 100) {
        return;
    }

    m_momentumActive = true;
    m_targetOffset = calculateDeceleration(velocity);

    // Determine target index based on momentum
    int targetIndex = currentIndex();
    if (std::abs(m_targetOffset) > width() * 0.3) { // 30% threshold
        if (m_targetOffset > 0) {
            targetIndex = std::max(0, currentIndex() - 1);
        } else {
            targetIndex = std::min(itemCount() - 1, currentIndex() + 1);
        }
    }

    // Start animation to target index
    if (targetIndex != currentIndex()) {
        setCurrentIndex(targetIndex, true);
    }

    emit momentumStarted(velocity);
}

void FluentTouchCarousel::applyEdgeResistance(qreal& offset) {
    if (m_touchConfig.edgeBehavior == FluentCarouselEdgeBehavior::Stop) {
        // Hard stop at edges
        if ((currentIndex() == 0 && offset > 0) ||
            (currentIndex() == itemCount() - 1 && offset < 0)) {
            offset = 0;
        }
    } else if (m_touchConfig.edgeBehavior == FluentCarouselEdgeBehavior::Resist) {
        // Apply resistance at edges
        if ((currentIndex() == 0 && offset > 0) ||
            (currentIndex() == itemCount() - 1 && offset < 0)) {
            offset *= m_touchConfig.edgeResistance;
        }
    }
    // Bounce and Wrap behaviors would be handled in the animation
}

void FluentTouchCarousel::handleEdgeBounce() {
    if (m_touchConfig.edgeBehavior != FluentCarouselEdgeBehavior::Bounce) {
        return;
    }

    // Implement bounce animation
    if (m_currentOffset != 0) {
        m_momentumAnimation->setStartValue(m_currentOffset);
        m_momentumAnimation->setEndValue(0.0);
        m_momentumAnimation->setDuration(300);
        m_momentumAnimation->setEasingCurve(QEasingCurve::OutBounce);
        m_momentumAnimation->start();
    }
}

qreal FluentTouchCarousel::calculateDeceleration(qreal velocity) {
    // Simple deceleration calculation
    qreal friction = m_touchConfig.friction;
    qreal time = std::abs(velocity) / (friction * 1000.0); // Time to stop
    return velocity * time * 0.5; // Distance traveled
}

// Feedback methods
void FluentTouchCarousel::provideTouchFeedback(FluentCarouselTouchGesture gesture) {
    Q_UNUSED(gesture)
    switch (m_touchConfig.feedback) {
    case FluentCarouselTouchFeedback::None:
        break;

    case FluentCarouselTouchFeedback::Haptic:
        provideHapticFeedback();
        break;

    case FluentCarouselTouchFeedback::Visual:
        provideVisualFeedback(m_touchCurrentPos);
        break;

    case FluentCarouselTouchFeedback::Audio:
        provideAudioFeedback();
        break;

    case FluentCarouselTouchFeedback::Combined:
        if (m_touchConfig.enableHapticFeedback) {
            provideHapticFeedback();
        }
        provideVisualFeedback(m_touchCurrentPos);
        break;
    }
}

void FluentTouchCarousel::provideHapticFeedback() {
    // Platform-specific haptic feedback would go here
    // For now, this is a placeholder
#ifdef Q_OS_ANDROID
    // Android haptic feedback
#elif defined(Q_OS_IOS)
    // iOS haptic feedback
#endif
}

void FluentTouchCarousel::provideVisualFeedback(const QPointF& position) {
    m_feedbackPosition = position;
    m_feedbackActive = true;
    m_feedbackTimer->start();
    update();
}

void FluentTouchCarousel::provideAudioFeedback() {
    // Audio feedback would go here
    // Could play system sounds or custom audio
}

// Utility methods
qreal FluentTouchCarousel::normalizeOffset(qreal offset) const {
    // Normalize offset to valid range
    qreal maxOffset = width(); // or height for vertical
    return std::clamp(offset, -maxOffset, maxOffset);
}

int FluentTouchCarousel::offsetToIndex(qreal offset) const {
    // Convert offset to carousel index
    qreal itemWidth = static_cast<qreal>(width()) / std::max(1, itemCount());
    return static_cast<int>(std::round(offset / itemWidth));
}

qreal FluentTouchCarousel::indexToOffset(int index) const {
    // Convert carousel index to offset
    qreal itemWidth = static_cast<qreal>(width()) / std::max(1, itemCount());
    return index * itemWidth;
}

bool FluentTouchCarousel::isAtEdge() const {
    return (currentIndex() == 0 && m_currentOffset > 0) ||
           (currentIndex() == itemCount() - 1 && m_currentOffset < 0);
}

void FluentTouchCarousel::updateAccessibilityInfo() {
    // Call parent's protected method through public interface
    if (m_touchConfig.enableTouchAccessibility) {
        setAccessibleDescription(accessibleDescription() + " - Touch enabled");
    }
}

} // namespace FluentQt::Components
