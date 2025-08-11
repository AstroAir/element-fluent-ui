// src/Animation/FluentAnimator.cpp
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Animation/FluentTransformEffect.h"
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
#include <QOpenGLContext>

namespace FluentQt::Animation {

// Advanced animation utilities




FluentAnimator::FluentAnimator(QObject* parent)
    : QObject(parent)
{
    // Connect to performance monitor for adaptive animations
    connect(&Core::FluentPerformanceMonitor::instance(),
            &Core::FluentPerformanceMonitor::frameRateChanged,
            this, [](double fps) {
                // Adapt animation quality based on performance
                Q_UNUSED(fps)
                // This could trigger animation quality adjustments
            });
}

using namespace FluentQt::Animation;

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

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);
    target->show();

    auto animation = std::make_unique<QPropertyAnimation>(effect, "scale");

    animation->setStartValue(0.1);
    animation->setEndValue(1.0);
    setupAnimation(animation.get(), config);

    // Use Fluent Design easing for scale animation
    FluentAnimationConfig scaleConfig = config;
    if (scaleConfig.easing == FluentEasing::EaseOut) {
        scaleConfig.easing = FluentEasing::BackOut; // More appropriate for scale-in
    }
    animation->setEasingCurve(toQtEasing(scaleConfig.easing));

    // Cleanup effect when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished, [target, effect]() {
        target->setGraphicsEffect(nullptr);
        effect->deleteLater();
    });

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

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);

    auto animation = std::make_unique<QPropertyAnimation>(effect, "scale");

    animation->setStartValue(1.0);
    animation->setEndValue(0.1);
    setupAnimation(animation.get(), config);

    // Use Fluent Design easing for scale-out
    FluentAnimationConfig scaleConfig = config;
    if (scaleConfig.easing == FluentEasing::EaseOut) {
        scaleConfig.easing = FluentEasing::BackIn; // More appropriate for scale-out
    }
    animation->setEasingCurve(toQtEasing(scaleConfig.easing));

    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished, [target, effect]() {
        target->hide();
        target->setGraphicsEffect(nullptr);
        effect->deleteLater();
    });

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

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);
    target->show();

    auto animation = std::make_unique<QPropertyAnimation>(effect, "translation");

    animation->setStartValue(from);
    animation->setEndValue(QPointF(0, 0));
    setupAnimation(animation.get(), config);

    // Cleanup effect when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished, [target, effect]() {
        target->setGraphicsEffect(nullptr);
        effect->deleteLater();
    });

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

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);

    auto animation = std::make_unique<QPropertyAnimation>(effect, "translation");

    animation->setStartValue(QPointF(0, 0));
    animation->setEndValue(to);
    setupAnimation(animation.get(), config);

    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished, [target, effect]() {
        target->hide();
        target->setGraphicsEffect(nullptr);
        effect->deleteLater();
    });

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

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);
    target->show();

    auto animation = std::make_unique<QPropertyAnimation>(effect, "rotation");

    animation->setStartValue(fromAngle);
    animation->setEndValue(0.0);
    setupAnimation(animation.get(), config);

    // Cleanup effect when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished, [target, effect]() {
        target->setGraphicsEffect(nullptr);
        effect->deleteLater();
    });

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

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);

    auto animation = std::make_unique<QPropertyAnimation>(effect, "rotation");

    animation->setStartValue(0.0);
    animation->setEndValue(toAngle);
    setupAnimation(animation.get(), config);

    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished, [target, effect]() {
        target->hide();
        target->setGraphicsEffect(nullptr);
        effect->deleteLater();
    });

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

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);
    target->show();

    auto animation = std::make_unique<QPropertyAnimation>(effect, "translation");

    animation->setStartValue(QPointF(0, distance));
    animation->setEndValue(QPointF(0, 0));
    setupAnimation(animation.get(), config);

    // Use appropriate easing for slide up (entrance)
    FluentAnimationConfig slideConfig = config;
    if (slideConfig.easing == FluentEasing::EaseOut) {
        slideConfig.easing = FluentEasing::EaseOutCubic; // Fluent standard for entrances
    }
    animation->setEasingCurve(toQtEasing(slideConfig.easing));

    // Cleanup effect when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished, [target, effect]() {
        target->setGraphicsEffect(nullptr);
        effect->deleteLater();
    });

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

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);

    auto animation = std::make_unique<QPropertyAnimation>(effect, "translation");

    animation->setStartValue(QPointF(0, 0));
    animation->setEndValue(QPointF(0, distance));
    setupAnimation(animation.get(), config);

    // Use appropriate easing for slide down (exit)
    FluentAnimationConfig slideConfig = config;
    if (slideConfig.easing == FluentEasing::EaseOut) {
        slideConfig.easing = FluentEasing::EaseInCubic; // Fluent standard for exits
    }
    animation->setEasingCurve(toQtEasing(slideConfig.easing));

    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished, [target, effect]() {
        target->hide();
        target->setGraphicsEffect(nullptr);
        effect->deleteLater();
    });

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

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);

    auto animation = std::make_unique<QPropertyAnimation>(effect, "translation");

    animation->setStartValue(QPointF(0, 0));
    animation->setEndValue(QPointF(-distance, 0));
    setupAnimation(animation.get(), config);

    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished, [target, effect]() {
        target->hide();
        target->setGraphicsEffect(nullptr);
        effect->deleteLater();
    });

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

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);

    auto animation = std::make_unique<QPropertyAnimation>(effect, "translation");

    animation->setStartValue(QPointF(0, 0));
    animation->setEndValue(QPointF(distance, 0));
    setupAnimation(animation.get(), config);

    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished, [target, effect]() {
        target->hide();
        target->setGraphicsEffect(nullptr);
        effect->deleteLater();
    });

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

    // Check if we should skip animations for performance or accessibility
    if (Core::FluentPerformanceMonitor::instance().shouldSkipAnimation() ||
        (config.respectReducedMotion && shouldRespectReducedMotion())) {
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

    // Enable hardware acceleration hints if requested
    if (config.useHardwareAcceleration) {
        // Set hints for the graphics system to use GPU acceleration
        animation->setProperty("hardwareAccelerated", true);

        // Use optimized update intervals for GPU acceleration
        if (QOpenGLContext::currentContext()) {
            animation->setProperty("updateInterval", 16); // 60 FPS
        } else {
            animation->setProperty("updateInterval", 33); // 30 FPS fallback
        }
    }

    // Register animation with performance monitor
    FluentAnimationManager::instance().registerAnimation(animation);
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::hoverEffect(
    QWidget* target,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::hoverEffect");

    // Use opacity effect for better performance and visual consistency
    QGraphicsOpacityEffect* opacityEffect =
        qobject_cast<QGraphicsOpacityEffect*>(target->graphicsEffect());

    if (!opacityEffect) {
        opacityEffect = new QGraphicsOpacityEffect(target);
        opacityEffect->setOpacity(1.0);
        target->setGraphicsEffect(opacityEffect);
    }

    auto animation = std::make_unique<QPropertyAnimation>(opacityEffect, "opacity");

    // Fluent Design hover: subtle opacity change
    animation->setStartValue(opacityEffect->opacity());
    animation->setEndValue(0.9); // More subtle than 0.8

    // Use faster duration for micro-interactions
    FluentAnimationConfig hoverConfig = config;
    hoverConfig.duration = std::chrono::milliseconds(150); // Fluent standard for hover
    hoverConfig.easing = FluentEasing::EaseOutQuad; // Smooth but quick

    setupAnimation(animation.get(), hoverConfig);

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

    // Create a subtle scale-down effect for press feedback
    const QRect currentGeometry = target->geometry();
    const QPoint center = currentGeometry.center();

    // Scale down to 98% (subtle press feedback)
    const qreal scaleFactor = 0.98;
    const QSize scaledSize(currentGeometry.width() * scaleFactor,
                          currentGeometry.height() * scaleFactor);
    const QRect pressedGeometry(center.x() - scaledSize.width() / 2,
                               center.y() - scaledSize.height() / 2,
                               scaledSize.width(), scaledSize.height());

    auto animation = std::make_unique<QPropertyAnimation>(target, "geometry");
    animation->setStartValue(currentGeometry);
    animation->setEndValue(pressedGeometry);

    // Use very fast duration for press feedback
    FluentAnimationConfig pressConfig = config;
    pressConfig.duration = std::chrono::milliseconds(100); // Fluent standard for press
    pressConfig.easing = FluentEasing::EaseOutQuart; // Sharp response

    setupAnimation(animation.get(), pressConfig);

    // Auto-reverse the animation for press feedback
    animation->setLoopCount(2);
    animation->setProperty("direction", QAbstractAnimation::Backward);

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

    // Use opacity effect for better performance
    QGraphicsOpacityEffect* opacityEffect =
        qobject_cast<QGraphicsOpacityEffect*>(target->graphicsEffect());

    if (!opacityEffect) {
        opacityEffect = new QGraphicsOpacityEffect(target);
        opacityEffect->setOpacity(1.0);
        target->setGraphicsEffect(opacityEffect);
    }

    auto animation = std::make_unique<QPropertyAnimation>(opacityEffect, "opacity");
    animation->setStartValue(1.0);
    animation->setEndValue(0.6); // More subtle pulse
    animation->setLoopCount(-1); // Infinite

    // Use appropriate timing for pulse
    FluentAnimationConfig pulseConfig = config;
    pulseConfig.duration = std::chrono::milliseconds(1000); // Slower pulse
    pulseConfig.easing = FluentEasing::EaseInOutSine; // Smooth pulse

    setupAnimation(animation.get(), pulseConfig);

    return animation;
}

std::unique_ptr<QSequentialAnimationGroup> FluentAnimator::rippleEffect(
    QWidget* target,
    const QPoint& center,
    const FluentAnimationConfig& config
) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::rippleEffect");
    Q_UNUSED(config); // Future use for customization

    auto group = std::make_unique<QSequentialAnimationGroup>();

    // Create a temporary widget for the ripple effect
    QWidget* rippleWidget = new QWidget(target);
    rippleWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    rippleWidget->setStyleSheet("background-color: rgba(255, 255, 255, 100); border-radius: 50%;");

    // Position the ripple at the click center
    QPoint actualCenter = center.isNull() ? target->rect().center() : center;
    const int startSize = 10;
    const int endSize = qMax(target->width(), target->height()) * 2;

    rippleWidget->setGeometry(actualCenter.x() - startSize/2, actualCenter.y() - startSize/2,
                             startSize, startSize);
    rippleWidget->show();

    // Phase 1: Expand the ripple
    auto expandAnim = std::make_unique<QPropertyAnimation>(rippleWidget, "geometry");
    QRect startRect(actualCenter.x() - startSize/2, actualCenter.y() - startSize/2,
                   startSize, startSize);
    QRect endRect(actualCenter.x() - endSize/2, actualCenter.y() - endSize/2,
                 endSize, endSize);

    expandAnim->setStartValue(startRect);
    expandAnim->setEndValue(endRect);
    expandAnim->setDuration(400); // Fluent ripple duration
    expandAnim->setEasingCurve(QEasingCurve::OutCubic);

    // Phase 2: Fade out the ripple
    QGraphicsOpacityEffect* rippleOpacity = new QGraphicsOpacityEffect(rippleWidget);
    rippleWidget->setGraphicsEffect(rippleOpacity);

    auto fadeAnim = std::make_unique<QPropertyAnimation>(rippleOpacity, "opacity");
    fadeAnim->setStartValue(1.0);
    fadeAnim->setEndValue(0.0);
    fadeAnim->setDuration(200);
    fadeAnim->setEasingCurve(QEasingCurve::OutQuad);

    // Add animations to group
    group->addAnimation(expandAnim.release());
    group->addAnimation(fadeAnim.release());

    // Cleanup when finished
    QObject::connect(group.get(), &QSequentialAnimationGroup::finished, [rippleWidget]() {
        rippleWidget->deleteLater();
    });

    return group;
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



// FluentAnimationManager implementation
FluentAnimationManager& FluentAnimationManager::instance() {
    static FluentAnimationManager instance;
    return instance;
}

void FluentAnimationManager::registerAnimation(QAbstractAnimation* animation) {
    m_activeAnimations.insert(animation);

    connect(animation, &QAbstractAnimation::finished, [this, animation]() {
        m_activeAnimations.remove(animation);
    });

    connect(animation, &QAbstractAnimation::destroyed, [this, animation]() {
        m_activeAnimations.remove(animation);
    });
}

void FluentAnimationManager::pauseAllAnimations() {
    for (auto* animation : m_activeAnimations) {
        if (animation->state() == QAbstractAnimation::Running) {
            animation->pause();
            m_pausedAnimations.insert(animation);
        }
    }
}

void FluentAnimationManager::resumeAllAnimations() {
    for (auto* animation : m_pausedAnimations) {
        animation->resume();
    }
    m_pausedAnimations.clear();
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::fadeIn(
    QWidget* target,
    int durationMs,
    FluentEasing easing
) {
    FluentAnimationConfig cfg;
    cfg.duration = std::chrono::milliseconds(durationMs);
    cfg.easing = easing;
    return FluentAnimator::fadeIn(target, cfg);
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::fadeOut(
    QWidget* target,
    int durationMs,
    FluentEasing easing
) {
    FluentAnimationConfig cfg;
    cfg.duration = std::chrono::milliseconds(durationMs);
    cfg.easing = easing;
    return FluentAnimator::fadeOut(target, cfg);
}


void FluentAnimationManager::stopAllAnimations() {
    for (auto* animation : m_activeAnimations) {
        animation->stop();
    }
    m_activeAnimations.clear();
    m_pausedAnimations.clear();
}

int FluentAnimationManager::activeAnimationCount() const {
    return m_activeAnimations.size();
}

} // namespace FluentQt::Animation
