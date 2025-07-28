// src/Animation/FluentAnimator.cpp
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QWidget>
#include <QGraphicsOpacityEffect>
#include <QGraphicsEffect>
#include <QTransform>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QPauseAnimation>
#include <QEasingCurve>
#include <QApplication>
#include <QTimer>
#include <QtMath>
#include <QAccessible>

namespace FluentQt::Animation {

FluentAnimator::FluentAnimator(QObject* parent)
    : QObject(parent)
{
    // Connect to performance monitor for adaptive animations
    connect(&Core::FluentPerformanceMonitor::instance(),
            &Core::FluentPerformanceMonitor::frameRateChanged,
            this, [this](double fps) {
                // Adapt animation quality based on performance
                Q_UNUSED(fps)
                // This could trigger animation quality adjustments
            });
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::fadeIn(
    QWidget* target,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }
    
    FLUENT_PROFILE("FluentAnimator::fadeIn");
    
    // Create or get opacity effect
    QGraphicsOpacityEffect* opacityEffect = 
        qobject_cast<QGraphicsOpacityEffect*>(target->graphicsEffect());
    
    if (!opacityEffect) {
        opacityEffect = new QGraphicsOpacityEffect(target);
        target->setGraphicsEffect(opacityEffect);
    }
    
    auto animation = std::make_unique<QPropertyAnimation>(opacityEffect, "opacity");
    
    // Set initial state
    opacityEffect->setOpacity(0.0);
    target->show();
    
    // Configure animation
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    setupAnimation(animation.get(), config);
    
    // Cleanup effect when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished, [target]() {
        if (auto* effect = target->graphicsEffect()) {
            target->setGraphicsEffect(nullptr);
            effect->deleteLater();
        }
    });
    
    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::fadeOut(
    QWidget* target,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }
    
    FLUENT_PROFILE("FluentAnimator::fadeOut");
    
    // Create or get opacity effect
    QGraphicsOpacityEffect* opacityEffect = 
        qobject_cast<QGraphicsOpacityEffect*>(target->graphicsEffect());
    
    if (!opacityEffect) {
        opacityEffect = new QGraphicsOpacityEffect(target);
        opacityEffect->setOpacity(1.0);
        target->setGraphicsEffect(opacityEffect);
    }
    
    auto animation = std::make_unique<QPropertyAnimation>(opacityEffect, "opacity");
    
    // Configure animation
    animation->setStartValue(opacityEffect->opacity());
    animation->setEndValue(0.0);
    setupAnimation(animation.get(), config);
    
    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished, [target]() {
        target->hide();
        if (auto* effect = target->graphicsEffect()) {
            target->setGraphicsEffect(nullptr);
            effect->deleteLater();
        }
    });
    
    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::scaleIn(
    QWidget* target,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }
    
    FLUENT_PROFILE("FluentAnimator::scaleIn");
    
    // Store original size
    const QSize originalSize = target->size();
    target->setProperty("originalSize", originalSize);
    
    auto animation = std::make_unique<QPropertyAnimation>(target, "size");
    
    // Set initial state (very small)
    const QSize startSize(originalSize.width() / 10, originalSize.height() / 10);
    target->resize(startSize);
    target->show();
    
    // Configure animation
    animation->setStartValue(startSize);
    animation->setEndValue(originalSize);
    setupAnimation(animation.get(), config);
    
    // Use custom easing for scale animation
    FluentAnimationConfig scaleConfig = config;
    if (scaleConfig.easing == FluentEasing::EaseOut) {
        scaleConfig.easing = FluentEasing::Back;
    }
    
    animation->setEasingCurve(toQtEasing(scaleConfig.easing));
    
    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::scaleOut(
    QWidget* target,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::scaleOut");

    // Get current size
    const QSize currentSize = target->size();
    const QSize endSize(currentSize.width() / 10, currentSize.height() / 10);

    auto animation = std::make_unique<QPropertyAnimation>(target, "size");

    // Configure animation
    animation->setStartValue(currentSize);
    animation->setEndValue(endSize);
    setupAnimation(animation.get(), config);

    // Use custom easing for scale animation
    FluentAnimationConfig scaleConfig = config;
    if (scaleConfig.easing == FluentEasing::EaseOut) {
        scaleConfig.easing = FluentEasing::EaseIn;
    }

    animation->setEasingCurve(toQtEasing(scaleConfig.easing));

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::slideIn(
    QWidget* target,
    const QPoint& from,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }
    
    FLUENT_PROFILE("FluentAnimator::slideIn");
    
    // Store original position
    const QPoint originalPos = target->pos();
    target->setProperty("originalPos", originalPos);
    
    auto animation = std::make_unique<QPropertyAnimation>(target, "pos");
    
    // Calculate start position
    QPoint startPos = originalPos + from;
    target->move(startPos);
    target->show();
    
    // Configure animation
    animation->setStartValue(startPos);
    animation->setEndValue(originalPos);
    setupAnimation(animation.get(), config);
    
    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::slideOut(
    QWidget* target,
    const QPoint& to,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::slideOut");

    // Get current position
    const QPoint currentPos = target->pos();

    auto animation = std::make_unique<QPropertyAnimation>(target, "pos");

    // Configure animation
    animation->setStartValue(currentPos);
    animation->setEndValue(to);
    setupAnimation(animation.get(), config);

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::rotateIn(
    QWidget* target,
    qreal fromAngle,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::rotateIn");

    // For now, use a simple opacity animation as a placeholder
    // In a full implementation, this would use a custom graphics effect
    auto animation = std::make_unique<QPropertyAnimation>(target, "windowOpacity");
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    setupAnimation(animation.get(), config);

    // TODO: Implement proper rotation using QGraphicsView or custom painting
    Q_UNUSED(fromAngle)

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::rotateOut(
    QWidget* target,
    qreal toAngle,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::rotateOut");

    // For now, use a simple opacity animation as a placeholder
    // In a full implementation, this would use a custom graphics effect
    auto animation = std::make_unique<QPropertyAnimation>(target, "windowOpacity");
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    setupAnimation(animation.get(), config);

    // TODO: Implement proper rotation using QGraphicsView or custom painting
    Q_UNUSED(toAngle)

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::colorTransition(
    QObject* target,
    const QByteArray& property,
    const QColor& from,
    const QColor& to,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::colorTransition");

    auto animation = std::make_unique<QPropertyAnimation>(target, property);

    // Configure animation
    animation->setStartValue(from);
    animation->setEndValue(to);
    setupAnimation(animation.get(), config);

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::slideUp(
    QWidget* target,
    int distance,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::slideUp");

    QRect startGeometry = target->geometry();
    QRect endGeometry = startGeometry;
    endGeometry.moveTop(startGeometry.top() - distance);

    auto animation = std::make_unique<QPropertyAnimation>(target, "geometry");
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    setupAnimation(animation.get(), config);

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::slideDown(
    QWidget* target,
    int distance,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::slideDown");

    QRect startGeometry = target->geometry();
    QRect endGeometry = startGeometry;
    endGeometry.moveTop(startGeometry.top() + distance);

    auto animation = std::make_unique<QPropertyAnimation>(target, "geometry");
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    setupAnimation(animation.get(), config);

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::slideLeft(
    QWidget* target,
    int distance,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::slideLeft");

    QRect startGeometry = target->geometry();
    QRect endGeometry = startGeometry;
    endGeometry.moveLeft(startGeometry.left() - distance);

    auto animation = std::make_unique<QPropertyAnimation>(target, "geometry");
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    setupAnimation(animation.get(), config);

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::slideRight(
    QWidget* target,
    int distance,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::slideRight");

    QRect startGeometry = target->geometry();
    QRect endGeometry = startGeometry;
    endGeometry.moveLeft(startGeometry.left() + distance);

    auto animation = std::make_unique<QPropertyAnimation>(target, "geometry");
    animation->setStartValue(startGeometry);
    animation->setEndValue(endGeometry);
    setupAnimation(animation.get(), config);

    return animation;
}

std::unique_ptr<QSequentialAnimationGroup> FluentAnimator::revealAnimation(
    QWidget* target,
    const QPoint& center,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }
    
    FLUENT_PROFILE("FluentAnimator::revealAnimation");
    
    auto group = std::make_unique<QSequentialAnimationGroup>();
    
    // Phase 1: Quick scale animation from center
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(target);
    target->setGraphicsEffect(opacityEffect);
    
    // Create reveal mask animation (simplified version)
    // In a full implementation, this would use custom rendering
    auto scalePhase = std::make_unique<QPropertyAnimation>(target, "geometry");
    
    QRect startGeometry = target->geometry();
    QPoint actualCenter = center.isNull() ? target->rect().center() : center;
    
    // Start from a small rect at the center point
    QRect startRect(actualCenter.x() - 5, actualCenter.y() - 5, 10, 10);
    
    scalePhase->setStartValue(startRect);
    scalePhase->setEndValue(target->geometry());
    scalePhase->setDuration(config.duration.count() * 0.6); // 60% of total duration
    scalePhase->setEasingCurve(toQtEasing(FluentEasing::EaseOut));
    
    // Phase 2: Fade in effect
    auto fadePhase = std::make_unique<QPropertyAnimation>(opacityEffect, "opacity");
    fadePhase->setStartValue(0.0);
    fadePhase->setEndValue(1.0);
    fadePhase->setDuration(config.duration.count() * 0.4); // 40% of total duration
    fadePhase->setEasingCurve(toQtEasing(FluentEasing::EaseOut));
    
    // Add phases to group
    group->addAnimation(scalePhase.release());
    group->addAnimation(fadePhase.release());
    
    // Cleanup when finished
    QObject::connect(group.get(), &QSequentialAnimationGroup::finished, [target, opacityEffect]() {
        target->setGraphicsEffect(nullptr);
        opacityEffect->deleteLater();
    });
    
    return group;
}

std::unique_ptr<QParallelAnimationGroup> FluentAnimator::connectedAnimation(
    QWidget* from,
    QWidget* to,
    const FluentAnimationConfig& config
) {
    if (!from || !to) {
        return nullptr;
    }
    
    FLUENT_PROFILE("FluentAnimator::connectedAnimation");
    
    auto group = std::make_unique<QParallelAnimationGroup>();
    
    // Get geometries in global coordinates
    const QRect fromGeometry = QRect(from->mapToGlobal(QPoint(0, 0)), from->size());
    const QRect toGeometry = QRect(to->mapToGlobal(QPoint(0, 0)), to->size());
    
    // Create temporary widget for the connected animation
    QWidget* connector = new QWidget(from->window());
    connector->setStyleSheet(from->styleSheet()); // Copy appearance
    connector->setGeometry(fromGeometry);
    connector->show();
    
    // Fade out the source
    FluentAnimationConfig fadeOutConfig;
    fadeOutConfig.duration = config.duration;
    fadeOutConfig.easing = FluentEasing::EaseIn;
    auto fadeOutAnim = fadeOut(from, fadeOutConfig);
    
    // Move and resize the connector
    auto moveAnim = std::make_unique<QPropertyAnimation>(connector, "geometry");
    moveAnim->setStartValue(fromGeometry);
    moveAnim->setEndValue(toGeometry);
    moveAnim->setDuration(config.duration.count());
    moveAnim->setEasingCurve(toQtEasing(config.easing));
    
    // Fade in the destination
    FluentAnimationConfig fadeInConfig;
    fadeInConfig.duration = std::chrono::milliseconds(config.duration.count() / 2);
    fadeInConfig.easing = FluentEasing::EaseOut;
    fadeInConfig.delay = std::chrono::milliseconds(config.duration.count() / 2);
    auto fadeInAnim = fadeIn(to, fadeInConfig);
    
    // Add delay for destination fade-in
    auto pauseAnim = new QPauseAnimation(config.duration.count() / 2);
    group->addAnimation(pauseAnim);
    
    group->addAnimation(fadeOutAnim.release());
    group->addAnimation(moveAnim.release());
    group->addAnimation(fadeInAnim.release());
    
    // Cleanup connector when finished
    QObject::connect(group.get(), &QParallelAnimationGroup::finished, [connector]() {
        connector->deleteLater();
    });
    
    return group;
}

QEasingCurve::Type FluentAnimator::toQtEasing(FluentEasing easing) {
    switch (easing) {
        case FluentEasing::Linear:
            return QEasingCurve::Linear;
        case FluentEasing::EaseOut:
            return QEasingCurve::OutCubic;
        case FluentEasing::EaseIn:
            return QEasingCurve::InCubic;
        case FluentEasing::EaseInOut:
            return QEasingCurve::InOutCubic;
        case FluentEasing::EaseOutQuad:
            return QEasingCurve::OutQuad;
        case FluentEasing::EaseInQuad:
            return QEasingCurve::InQuad;
        case FluentEasing::EaseInOutQuad:
            return QEasingCurve::InOutQuad;
        case FluentEasing::EaseOutCubic:
            return QEasingCurve::OutCubic;
        case FluentEasing::EaseInCubic:
            return QEasingCurve::InCubic;
        case FluentEasing::EaseInOutCubic:
            return QEasingCurve::InOutCubic;
        case FluentEasing::EaseOutQuart:
            return QEasingCurve::OutQuart;
        case FluentEasing::EaseInQuart:
            return QEasingCurve::InQuart;
        case FluentEasing::EaseInOutQuart:
            return QEasingCurve::InOutQuart;
        case FluentEasing::EaseOutQuint:
            return QEasingCurve::OutQuint;
        case FluentEasing::EaseInQuint:
            return QEasingCurve::InQuint;
        case FluentEasing::EaseInOutQuint:
            return QEasingCurve::InOutQuint;
        case FluentEasing::EaseOutSine:
            return QEasingCurve::OutSine;
        case FluentEasing::EaseInSine:
            return QEasingCurve::InSine;
        case FluentEasing::EaseInOutSine:
            return QEasingCurve::InOutSine;
        case FluentEasing::EaseOutExpo:
            return QEasingCurve::OutExpo;
        case FluentEasing::EaseInExpo:
            return QEasingCurve::InExpo;
        case FluentEasing::EaseInOutExpo:
            return QEasingCurve::InOutExpo;
        case FluentEasing::EaseOutCirc:
            return QEasingCurve::OutCirc;
        case FluentEasing::EaseInCirc:
            return QEasingCurve::InCirc;
        case FluentEasing::EaseInOutCirc:
            return QEasingCurve::InOutCirc;
        case FluentEasing::Bounce:
        case FluentEasing::BounceOut:
            return QEasingCurve::OutBounce;
        case FluentEasing::BounceIn:
            return QEasingCurve::InBounce;
        case FluentEasing::BounceInOut:
            return QEasingCurve::InOutBounce;
        case FluentEasing::Elastic:
        case FluentEasing::ElasticOut:
            return QEasingCurve::OutElastic;
        case FluentEasing::ElasticIn:
            return QEasingCurve::InElastic;
        case FluentEasing::ElasticInOut:
            return QEasingCurve::InOutElastic;
        case FluentEasing::Back:
        case FluentEasing::BackOut:
            return QEasingCurve::OutBack;
        case FluentEasing::BackIn:
            return QEasingCurve::InBack;
        case FluentEasing::BackInOut:
            return QEasingCurve::InOutBack;
        default:
            return QEasingCurve::OutCubic;
    }
}

void FluentAnimator::setupAnimation(
    QPropertyAnimation* animation,
    const FluentAnimationConfig& config
) {
    if (!animation) return;
    
    // Check if we should skip animations for performance
    if (Core::FluentPerformanceMonitor::instance().shouldSkipAnimation()) {
        animation->setDuration(0);
        return;
    }
    
    // Apply configuration
    animation->setDuration(config.duration.count());
    animation->setEasingCurve(toQtEasing(config.easing));
    animation->setLoopCount(config.loops);
    
    // Apply delay if specified
    if (config.delay.count() > 0) {
        auto* group = new QSequentialAnimationGroup();
        auto* pause = new QPauseAnimation(config.delay.count());
        group->addAnimation(pause);
        group->addAnimation(animation);
    }
    
    // Apply reverse on complete
    if (config.reverseOnComplete && config.loops == 1) {
        animation->setLoopCount(2);
        animation->setProperty("direction", QAbstractAnimation::Backward);
    }
    
    // Note: Performance monitoring would need to be handled by the caller
    // since this is a static method and cannot emit signals
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::hoverEffect(
    QWidget* target,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::hoverEffect");

    auto animation = std::make_unique<QPropertyAnimation>(target, "windowOpacity");
    animation->setStartValue(1.0);
    animation->setEndValue(0.8);
    setupAnimation(animation.get(), config);

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::pressEffect(
    QWidget* target,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::pressEffect");

    auto animation = scaleOut(target, config);
    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::focusEffect(
    QWidget* target,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::focusEffect");

    auto animation = std::make_unique<QPropertyAnimation>(target, "windowOpacity");
    animation->setStartValue(0.8);
    animation->setEndValue(1.0);
    setupAnimation(animation.get(), config);

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::pulseEffect(
    QWidget* target,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::pulseEffect");

    auto animation = std::make_unique<QPropertyAnimation>(target, "windowOpacity");
    animation->setStartValue(1.0);
    animation->setEndValue(0.5);
    animation->setLoopCount(-1); // Infinite
    setupAnimation(animation.get(), config);

    return animation;
}

void FluentAnimator::applyMicroInteractionSettings(
    QPropertyAnimation* animation,
    const FluentAnimationConfig& config
) {
    if (!animation) return;

    // Apply hardware acceleration if available
    if (config.useHardwareAcceleration) {
        // This would be platform-specific implementation
        // For now, just a placeholder
    }

    // Auto-start if configured
    if (config.autoStart) {
        QTimer::singleShot(0, [animation]() { animation->start(); });
    }

    // Auto-delete if configured
    if (config.deleteOnFinish) {
        QObject::connect(animation, &QPropertyAnimation::finished,
                         animation, &QPropertyAnimation::deleteLater);
    }
}

bool FluentAnimator::shouldRespectReducedMotion() {
    // Check system accessibility settings
    // This is a simplified implementation
    return QAccessible::isActive();
}

// Specialized animation effects
namespace Effects {

class FluentSpringEffect : public QObject {
    Q_OBJECT

public:
    FluentSpringEffect(QWidget* target, QObject* parent = nullptr)
        : QObject(parent), m_target(target) {}

    void start(const QVariant& from, const QVariant& to) {
        if (!m_target) return;
        
        m_timer = new QTimer(this);
        m_timer->setInterval(16); // ~60 FPS
        
        m_startValue = from;
        m_endValue = to;
        m_currentValue = from;
        m_velocity = 0.0;
        m_time = 0.0;
        
        connect(m_timer, &QTimer::timeout, this, &FluentSpringEffect::updateSpring);
        m_timer->start();
    }

private slots:
    void updateSpring() {
        const double dt = 0.016; // 16ms
        const double springConstant = 200.0;
        const double damping = 20.0;
        
        m_time += dt;
        
        // Simple spring physics
        const double displacement = m_currentValue.toDouble() - m_endValue.toDouble();
        const double force = -springConstant * displacement - damping * m_velocity;
        
        m_velocity += force * dt;
        double newValue = m_currentValue.toDouble() + m_velocity * dt;
        
        m_currentValue = newValue;
        
        // Apply the value to the target
        if (m_target) {
            // This would need to be implemented for specific properties
            // For now, just demonstrate the concept
        }
        
        // Check if spring has settled
        if (std::abs(displacement) < 0.1 && std::abs(m_velocity) < 0.1) {
            m_timer->stop();
            m_timer->deleteLater();
            m_timer = nullptr;
            
            // Ensure final value is exact
            m_currentValue = m_endValue;
            emit finished();
        }
    }

signals:
    void finished();

private:
    QWidget* m_target;
    QTimer* m_timer = nullptr;
    QVariant m_startValue;
    QVariant m_endValue;
    QVariant m_currentValue;
    double m_velocity = 0.0;
    double m_time = 0.0;
};

} // namespace Effects

// Advanced animation utilities
class FluentAnimationManager : public QObject {
    Q_OBJECT

public:
    static FluentAnimationManager& instance() {
        static FluentAnimationManager instance;
        return instance;
    }

    void registerAnimation(QAbstractAnimation* animation) {
        m_activeAnimations.insert(animation);
        
        connect(animation, &QAbstractAnimation::finished, [this, animation]() {
            m_activeAnimations.remove(animation);
        });
        
        connect(animation, &QAbstractAnimation::destroyed, [this, animation]() {
            m_activeAnimations.remove(animation);
        });
    }

    void pauseAllAnimations() {
        for (auto* animation : m_activeAnimations) {
            if (animation->state() == QAbstractAnimation::Running) {
                animation->pause();
                m_pausedAnimations.insert(animation);
            }
        }
    }

    void resumeAllAnimations() {
        for (auto* animation : m_pausedAnimations) {
            animation->resume();
        }
        m_pausedAnimations.clear();
    }

    void stopAllAnimations() {
        for (auto* animation : m_activeAnimations) {
            animation->stop();
        }
        m_activeAnimations.clear();
        m_pausedAnimations.clear();
    }

    int activeAnimationCount() const {
        return m_activeAnimations.size();
    }

private:
    FluentAnimationManager() = default;
    
    QSet<QAbstractAnimation*> m_activeAnimations;
    QSet<QAbstractAnimation*> m_pausedAnimations;
};

} // namespace FluentQt::Animation

#include "FluentAnimator.moc"