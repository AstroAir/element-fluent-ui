// src/Animation/FluentPhysicsAnimator.cpp
#include "FluentQt/Animation/FluentPhysicsAnimator.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QApplication>
#include <QMouseEvent>
#include <QTouchEvent>
#include <QWheelEvent>
#include <QtMath>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPauseAnimation>

namespace FluentQt::Animation {

FluentPhysicsAnimator::FluentPhysicsAnimator(QObject* parent)
    : QObject(parent)
    , m_physicsTimer(new QTimer(this))
{
    // Setup physics update timer
    m_physicsTimer->setInterval(PHYSICS_UPDATE_INTERVAL);
    connect(m_physicsTimer, &QTimer::timeout, this, &FluentPhysicsAnimator::updateSpringAnimation);
    
    // Connect to performance monitor for adaptive quality
    connect(&Core::FluentPerformanceMonitor::instance(),
            &Core::FluentPerformanceMonitor::frameRateChanged,
            this, [this](double fps) {
                // Adjust update interval based on performance
                if (fps < 30) {
                    m_physicsTimer->setInterval(33); // 30 FPS
                } else if (fps > 90) {
                    m_physicsTimer->setInterval(HIGH_PERF_UPDATE_INTERVAL); // 120 FPS
                } else {
                    m_physicsTimer->setInterval(PHYSICS_UPDATE_INTERVAL); // 60 FPS
                }
            });
}

std::unique_ptr<QPropertyAnimation> FluentPhysicsAnimator::springAnimation(
    QObject* target,
    const QByteArray& property,
    const QVariant& from,
    const QVariant& to,
    const FluentSpringConfig& config
) {
    if (!target) {
        return nullptr;
    }
    
    FLUENT_PROFILE("FluentPhysicsAnimator::springAnimation");
    
    auto animation = std::make_unique<FluentSpringAnimation>(target, property, config);
    animation->setStartValue(from);
    animation->setEndValue(to);
    animation->setDuration(config.maxDuration);
    
    // Create custom easing curve for spring physics
    animation->setEasingCurve(createSpringEasing(config));
    
    return std::unique_ptr<QPropertyAnimation>(animation.release());
}

std::unique_ptr<QPropertyAnimation> FluentPhysicsAnimator::springScale(
    QWidget* target,
    double fromScale,
    double toScale,
    const FluentSpringConfig& config
) {
    if (!target) {
        return nullptr;
    }
    
    FLUENT_PROFILE("FluentPhysicsAnimator::springScale");
    
    // Store original geometry for scaling calculations
    const QRect originalGeometry = target->geometry();
    target->setProperty("originalGeometry", originalGeometry);
    target->setProperty("targetScale", toScale);
    
    // Calculate scaled geometries
    const QPoint center = originalGeometry.center();
    const QSize fromSize(originalGeometry.width() * fromScale, 
                        originalGeometry.height() * fromScale);
    const QSize toSize(originalGeometry.width() * toScale, 
                      originalGeometry.height() * toScale);
    
    const QRect fromGeometry(center.x() - fromSize.width() / 2,
                            center.y() - fromSize.height() / 2,
                            fromSize.width(), fromSize.height());
    const QRect toGeometry(center.x() - toSize.width() / 2,
                          center.y() - toSize.height() / 2,
                          toSize.width(), toSize.height());
    
    auto animation = std::make_unique<FluentSpringAnimation>(target, "geometry", config);
    animation->setStartValue(fromGeometry);
    animation->setEndValue(toGeometry);
    animation->setDuration(config.maxDuration);
    
    return std::unique_ptr<QPropertyAnimation>(animation.release());
}

std::unique_ptr<QPropertyAnimation> FluentPhysicsAnimator::springPosition(
    QWidget* target,
    const QPointF& from,
    const QPointF& to,
    const FluentSpringConfig& config
) {
    if (!target) {
        return nullptr;
    }
    
    FLUENT_PROFILE("FluentPhysicsAnimator::springPosition");
    
    auto animation = std::make_unique<FluentSpringAnimation>(target, "pos", config);
    animation->setStartValue(from.toPoint());
    animation->setEndValue(to.toPoint());
    animation->setDuration(config.maxDuration);
    
    return std::unique_ptr<QPropertyAnimation>(animation.release());
}

std::unique_ptr<QSequentialAnimationGroup> FluentPhysicsAnimator::sharedElementTransition(
    QWidget* fromElement,
    QWidget* toElement,
    const FluentAnimationConfig& config
) {
    if (!fromElement || !toElement) {
        return nullptr;
    }
    
    FLUENT_PROFILE("FluentPhysicsAnimator::sharedElementTransition");
    
    auto group = std::make_unique<QSequentialAnimationGroup>();
    
    // Get global positions and sizes
    const QRect fromGlobalRect = QRect(fromElement->mapToGlobal(QPoint(0, 0)), fromElement->size());
    const QRect toGlobalRect = QRect(toElement->mapToGlobal(QPoint(0, 0)), toElement->size());
    
    // Create a temporary transition widget
    QWidget* transitionWidget = new QWidget(fromElement->window());
    transitionWidget->setStyleSheet(fromElement->styleSheet());
    transitionWidget->setGeometry(fromGlobalRect);
    transitionWidget->show();
    
    // Phase 1: Fade out source element
    QGraphicsOpacityEffect* fromOpacity = new QGraphicsOpacityEffect(fromElement);
    fromElement->setGraphicsEffect(fromOpacity);
    
    auto fadeOutAnim = new QPropertyAnimation(fromOpacity, "opacity");
    fadeOutAnim->setStartValue(1.0);
    fadeOutAnim->setEndValue(0.0);
    fadeOutAnim->setDuration(config.duration.count() / 3);
    fadeOutAnim->setEasingCurve(QEasingCurve::OutCubic);
    
    // Phase 2: Morph transition widget with spring physics
    FluentSpringConfig springConfig = FluentSpringConfig::gentle();
    auto morphAnim = new FluentSpringAnimation(transitionWidget, "geometry", springConfig);
    morphAnim->setStartValue(fromGlobalRect);
    morphAnim->setEndValue(toGlobalRect);
    morphAnim->setDuration(config.duration.count() * 2 / 3);
    
    // Phase 3: Fade in destination element
    QGraphicsOpacityEffect* toOpacity = new QGraphicsOpacityEffect(toElement);
    toElement->setGraphicsEffect(toOpacity);
    toOpacity->setOpacity(0.0);
    toElement->show();
    
    auto fadeInAnim = new QPropertyAnimation(toOpacity, "opacity");
    fadeInAnim->setStartValue(0.0);
    fadeInAnim->setEndValue(1.0);
    fadeInAnim->setDuration(config.duration.count() / 3);
    fadeInAnim->setEasingCurve(QEasingCurve::OutCubic);
    
    // Create parallel group for morph and fade in
    auto parallelGroup = new QParallelAnimationGroup();
    parallelGroup->addAnimation(morphAnim);
    parallelGroup->addAnimation(fadeInAnim);
    
    // Add phases to sequential group
    group->addAnimation(fadeOutAnim);
    group->addAnimation(parallelGroup);
    
    // Cleanup when finished
    connect(group.get(), &QSequentialAnimationGroup::finished, [fromElement, toElement, transitionWidget]() {
        // Remove effects
        fromElement->setGraphicsEffect(nullptr);
        toElement->setGraphicsEffect(nullptr);
        
        // Show source element again
        fromElement->show();
        
        // Delete transition widget
        transitionWidget->deleteLater();
    });
    
    return group;
}

std::unique_ptr<QParallelAnimationGroup> FluentPhysicsAnimator::morphTransition(
    QWidget* target,
    const QRect& fromGeometry,
    const QRect& toGeometry,
    const QColor& fromColor,
    const QColor& toColor,
    const FluentSpringConfig& springConfig
) {
    if (!target) {
        return nullptr;
    }
    
    FLUENT_PROFILE("FluentPhysicsAnimator::morphTransition");
    
    auto group = std::make_unique<QParallelAnimationGroup>();
    
    // Geometry animation with spring physics
    auto geometryAnim = new FluentSpringAnimation(target, "geometry", springConfig);
    geometryAnim->setStartValue(fromGeometry);
    geometryAnim->setEndValue(toGeometry);
    geometryAnim->setDuration(springConfig.maxDuration);
    
    // Color animation
    auto colorAnim = new QPropertyAnimation(target, "styleSheet");
    QString fromStyle = QString("background-color: %1;").arg(fromColor.name());
    QString toStyle = QString("background-color: %1;").arg(toColor.name());
    colorAnim->setStartValue(fromStyle);
    colorAnim->setEndValue(toStyle);
    colorAnim->setDuration(springConfig.maxDuration);
    colorAnim->setEasingCurve(QEasingCurve::OutCubic);
    
    group->addAnimation(geometryAnim);
    group->addAnimation(colorAnim);
    
    return group;
}

QEasingCurve FluentPhysicsAnimator::createSpringEasing(const FluentSpringConfig& config) {
    // Create a custom easing curve that simulates spring physics
    QEasingCurve curve(QEasingCurve::Custom);
    
    curve.setCustomType([config](qreal progress) -> qreal {
        if (progress <= 0.0) return 0.0;
        if (progress >= 1.0) return 1.0;
        
        // Spring physics calculation
        const double omega = qSqrt(config.stiffness / config.mass);
        const double zeta = config.damping / (2.0 * qSqrt(config.stiffness * config.mass));
        
        double result;
        if (zeta < 1.0) {
            // Underdamped (oscillating)
            const double omegaD = omega * qSqrt(1.0 - zeta * zeta);
            const double t = progress * (config.maxDuration / 1000.0);
            result = 1.0 - qExp(-zeta * omega * t) * qCos(omegaD * t);
        } else {
            // Overdamped or critically damped
            const double t = progress * (config.maxDuration / 1000.0);
            result = 1.0 - qExp(-omega * t);
        }
        
        return qBound(0.0, result, 1.0);
    });
    
    return curve;
}

QEasingCurve FluentPhysicsAnimator::createDampingEasing(const FluentDampingConfig& config) {
    QEasingCurve curve(QEasingCurve::Custom);
    
    curve.setCustomType([config](qreal progress) -> qreal {
        if (progress <= 0.0) return 0.0;
        if (progress >= 1.0) return 1.0;
        
        // Damping calculation with friction
        const double t = progress;
        const double damped = 1.0 - qPow(config.friction, t * 10.0);
        
        return qBound(0.0, damped, 1.0);
    });
    
    return curve;
}

void FluentPhysicsAnimator::updateSpringAnimation() {
    if (!m_springState.active) {
        return;
    }
    
    // Calculate elapsed time
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_springState.startTime);
    
    if (elapsed.count() > m_springState.config.maxDuration) {
        m_springState.active = false;
        m_physicsTimer->stop();
        emit springAnimationFinished();
        return;
    }
    
    // Calculate spring forces
    QPointF force = calculateSpringForce(m_springState.position, m_springState.target, m_springState.config);
    
    // Update velocity and position
    const double dt = PHYSICS_UPDATE_INTERVAL / 1000.0; // Convert to seconds
    m_springState.velocity += QVector2D(force) * dt;
    m_springState.position += m_springState.velocity.toPointF() * dt;
    
    // Check if settled
    QVector2D displacement = QVector2D(m_springState.target - m_springState.position);
    if (displacement.length() < m_springState.config.precision && 
        m_springState.velocity.length() < m_springState.config.precision) {
        m_springState.active = false;
        m_physicsTimer->stop();
        emit springAnimationFinished();
    }
}

QPointF FluentPhysicsAnimator::calculateSpringForce(const QPointF& position, const QPointF& target, 
                                                   const FluentSpringConfig& config) const {
    // Hooke's law: F = -k * x - c * v
    QVector2D displacement = QVector2D(target - position);
    QVector2D springForce = displacement * config.stiffness;
    QVector2D dampingForce = m_springState.velocity * config.damping;
    
    return (springForce - dampingForce).toPointF() / config.mass;
}

// FluentSpringAnimation implementation
FluentSpringAnimation::FluentSpringAnimation(QObject* target, const QByteArray& property,
                                           const FluentSpringConfig& config, QObject* parent)
    : QPropertyAnimation(target, property, parent)
    , m_config(config)
    , m_lastTime(std::chrono::steady_clock::now())
{
    setEasingCurve(FluentPhysicsAnimator::createSpringEasing(config));
}

void FluentSpringAnimation::setSpringConfig(const FluentSpringConfig& config) {
    m_config = config;
    setEasingCurve(FluentPhysicsAnimator::createSpringEasing(config));
    setDuration(config.maxDuration);
}

QVariant FluentSpringAnimation::interpolated(const QVariant& from, const QVariant& to, qreal progress) const {
    // For most properties, use the default interpolation with spring easing
    if (from.metaType().id() != QMetaType::QPointF && from.metaType().id() != QMetaType::QPoint) {
        return QPropertyAnimation::interpolated(from, to, progress);
    }

    // Custom interpolation for position-based properties with velocity consideration
    auto now = std::chrono::steady_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastTime).count() / 1000.0;
    m_lastTime = now;

    // Apply spring physics calculation
    QPointF fromPoint = from.toPointF();
    QPointF toPoint = to.toPointF();

    // Calculate spring-based progress
    const double omega = qSqrt(m_config.stiffness / m_config.mass);
    const double zeta = m_config.damping / (2.0 * qSqrt(m_config.stiffness * m_config.mass));

    double springProgress;
    if (zeta < 1.0) {
        // Underdamped (oscillating)
        const double omegaD = omega * qSqrt(1.0 - zeta * zeta);
        const double t = progress * (m_config.maxDuration / 1000.0);
        springProgress = 1.0 - qExp(-zeta * omega * t) * qCos(omegaD * t);
    } else {
        // Overdamped or critically damped
        const double t = progress * (m_config.maxDuration / 1000.0);
        springProgress = 1.0 - qExp(-omega * t);
    }

    springProgress = qBound(0.0, springProgress, 1.0);

    QPointF result = fromPoint + (toPoint - fromPoint) * springProgress;

    if (from.metaType().id() == QMetaType::QPoint) {
        return result.toPoint();
    }
    return result;
}

// FluentGestureRecognizer implementation
FluentGestureRecognizer::FluentGestureRecognizer(QWidget* target, QObject* parent)
    : QObject(parent)
    , m_target(target)
{
    if (m_target) {
        m_target->installEventFilter(this);
        m_target->setAttribute(Qt::WA_AcceptTouchEvents, true);
    }

    // Initialize default sensitivities
    m_sensitivities[Pan] = 1.0;
    m_sensitivities[Pinch] = 1.0;
    m_sensitivities[Rotate] = 1.0;
    m_sensitivities[Swipe] = 1.0;
    m_sensitivities[Fling] = 1.0;

    // Enable common gestures by default
    m_enabledGestures[Pan] = true;
    m_enabledGestures[Swipe] = true;
    m_enabledGestures[Fling] = true;
}

void FluentGestureRecognizer::setGestureEnabled(GestureType type, bool enabled) {
    m_enabledGestures[type] = enabled;
}

bool FluentGestureRecognizer::isGestureEnabled(GestureType type) const {
    return m_enabledGestures.value(type, false);
}

void FluentGestureRecognizer::setSensitivity(GestureType type, double sensitivity) {
    m_sensitivities[type] = qBound(0.1, sensitivity, 5.0);
}

double FluentGestureRecognizer::sensitivity(GestureType type) const {
    return m_sensitivities.value(type, 1.0);
}

bool FluentGestureRecognizer::eventFilter(QObject* obj, QEvent* event) {
    if (obj != m_target) {
        return QObject::eventFilter(obj, event);
    }

    switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseMove:
        case QEvent::MouseButtonRelease:
            processMouseEvent(static_cast<QMouseEvent*>(event));
            break;

        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
            processTouchEvent(static_cast<QTouchEvent*>(event));
            break;

        case QEvent::Wheel:
            processWheelEvent(static_cast<QWheelEvent*>(event));
            break;

        default:
            break;
    }

    return QObject::eventFilter(obj, event);
}

void FluentGestureRecognizer::processMouseEvent(QMouseEvent* event) {
    const QPointF position = event->position();
    auto now = std::chrono::steady_clock::now();

    switch (event->type()) {
        case QEvent::MouseButtonPress:
            if (isGestureEnabled(Pan)) {
                m_gestureStates[Pan].active = true;
                m_gestureStates[Pan].startPosition = position;
                m_gestureStates[Pan].lastPosition = position;
                m_gestureStates[Pan].startTime = now;
                m_gestureStates[Pan].lastTime = now;
                emit gestureStarted(Pan, position);
            }
            break;

        case QEvent::MouseMove:
            if (m_gestureStates[Pan].active) {
                auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - m_gestureStates[Pan].lastTime).count();

                if (deltaTime > 0) {
                    QVector2D velocity = QVector2D(position - m_gestureStates[Pan].lastPosition) / deltaTime * 1000.0;
                    velocity *= sensitivity(Pan);

                    m_gestureStates[Pan].lastPosition = position;
                    m_gestureStates[Pan].lastTime = now;

                    emit gestureUpdated(Pan, position, QVariant::fromValue(velocity));
                }
            }
            break;

        case QEvent::MouseButtonRelease:
            if (m_gestureStates[Pan].active) {
                auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - m_gestureStates[Pan].startTime).count();

                QVector2D totalDisplacement = QVector2D(position - m_gestureStates[Pan].startPosition);
                QVector2D velocity = totalDisplacement / totalTime * 1000.0;

                // Detect swipe or fling
                if (velocity.length() > 500.0 && isGestureEnabled(Fling)) {
                    emit gestureStarted(Fling, position);
                    emit gestureFinished(Fling, position, QVariant::fromValue(velocity));
                } else if (totalDisplacement.length() > 50.0 && isGestureEnabled(Swipe)) {
                    emit gestureStarted(Swipe, position);
                    emit gestureFinished(Swipe, position, QVariant::fromValue(totalDisplacement));
                }

                m_gestureStates[Pan].active = false;
                emit gestureFinished(Pan, position, QVariant::fromValue(velocity));
            }
            break;

        default:
            break;
    }
}

void FluentGestureRecognizer::processTouchEvent(QTouchEvent* event) {
    // Handle multi-touch gestures (pinch, rotate)
    const auto& touchPoints = event->touchPoints();

    if (touchPoints.size() == 2 && (isGestureEnabled(Pinch) || isGestureEnabled(Rotate))) {
        const auto& point1 = touchPoints[0];
        const auto& point2 = touchPoints[1];

        QPointF center = (point1.position() + point2.position()) / 2.0;

        if (event->type() == QEvent::TouchBegin) {
            if (isGestureEnabled(Pinch)) {
                double distance = QVector2D(point2.position() - point1.position()).length();
                m_gestureStates[Pinch].customData = distance;
                emit gestureStarted(Pinch, center);
            }

            if (isGestureEnabled(Rotate)) {
                QVector2D vector = QVector2D(point2.position() - point1.position());
                double angle = qAtan2(vector.y(), vector.x());
                m_gestureStates[Rotate].customData = angle;
                emit gestureStarted(Rotate, center);
            }
        } else if (event->type() == QEvent::TouchUpdate) {
            if (isGestureEnabled(Pinch)) {
                double distance = QVector2D(point2.position() - point1.position()).length();
                double scale = distance / m_gestureStates[Pinch].customData.toDouble();
                emit gestureUpdated(Pinch, center, scale);
            }

            if (isGestureEnabled(Rotate)) {
                QVector2D vector = QVector2D(point2.position() - point1.position());
                double angle = qAtan2(vector.y(), vector.x());
                double rotation = angle - m_gestureStates[Rotate].customData.toDouble();
                emit gestureUpdated(Rotate, center, rotation);
            }
        } else if (event->type() == QEvent::TouchEnd) {
            if (isGestureEnabled(Pinch)) {
                emit gestureFinished(Pinch, center, QVariant());
            }

            if (isGestureEnabled(Rotate)) {
                emit gestureFinished(Rotate, center, QVariant());
            }
        }
    }
}

void FluentGestureRecognizer::processWheelEvent(QWheelEvent* event) {
    // Handle wheel-based gestures (zoom, scroll)
    if (isGestureEnabled(Pan)) {
        QPointF position = event->position();
        QVector2D delta = QVector2D(event->angleDelta().x(), event->angleDelta().y()) / 120.0;
        delta *= sensitivity(Pan);

        emit gestureStarted(Pan, position);
        emit gestureUpdated(Pan, position, QVariant::fromValue(delta));
        emit gestureFinished(Pan, position, QVariant::fromValue(delta));
    }
}

} // namespace FluentQt::Animation

#include "FluentPhysicsAnimator.moc"
