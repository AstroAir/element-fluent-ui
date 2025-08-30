// src/Animation/FluentAnimator.cpp
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Animation/FluentTransformEffect.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QEasingCurve>
#include <QGraphicsEffect>
#include <QGraphicsOpacityEffect>
#include <QOpenGLContext>
#include <QParallelAnimationGroup>
#include <QPauseAnimation>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QSettings>
#include <QTimer>
#include <QTransform>
#include <QWidget>
#include <QtMath>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace FluentQt::Animation {

// Advanced animation utilities

FluentAnimator::FluentAnimator(QObject* parent) : QObject(parent) {
    // Connect to performance monitor for adaptive animations
    connect(&Core::FluentPerformanceMonitor::instance(),
            &Core::FluentPerformanceMonitor::frameRateChanged, this,
            [](double fps) {
                // Adapt animation quality based on performance
                Q_UNUSED(fps)
                // This could trigger animation quality adjustments
            });
}

using namespace FluentQt::Animation;

std::unique_ptr<QPropertyAnimation> FluentAnimator::fadeIn(
    QWidget* target, const FluentAnimationConfig& config) {
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

    auto animation =
        std::make_unique<QPropertyAnimation>(opacityEffect, "opacity");

    // Set initial state
    opacityEffect->setOpacity(0.0);
    target->show();

    // Configure animation
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    setupAnimation(animation.get(), config);

    // Cleanup effect when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished,
                     [target]() {
                         if (auto* effect = target->graphicsEffect()) {
                             target->setGraphicsEffect(nullptr);
                             effect->deleteLater();
                         }
                     });

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::fadeOut(
    QWidget* target, const FluentAnimationConfig& config) {
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

    auto animation =
        std::make_unique<QPropertyAnimation>(opacityEffect, "opacity");

    // Configure animation
    animation->setStartValue(opacityEffect->opacity());
    animation->setEndValue(0.0);
    setupAnimation(animation.get(), config);

    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished,
                     [target]() {
                         target->hide();
                         if (auto* effect = target->graphicsEffect()) {
                             target->setGraphicsEffect(nullptr);
                             effect->deleteLater();
                         }
                     });

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::scaleIn(
    QWidget* target, const FluentAnimationConfig& config) {
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
        scaleConfig.easing =
            FluentEasing::BackOut;  // More appropriate for scale-in
    }
    animation->setEasingCurve(toQtEasing(scaleConfig.easing));

    // Cleanup effect when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished,
                     [target, effect]() {
                         target->setGraphicsEffect(nullptr);
                         effect->deleteLater();
                     });

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::scaleOut(
    QWidget* target, const FluentAnimationConfig& config) {
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
        scaleConfig.easing =
            FluentEasing::BackIn;  // More appropriate for scale-out
    }
    animation->setEasingCurve(toQtEasing(scaleConfig.easing));

    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished,
                     [target, effect]() {
                         target->hide();
                         target->setGraphicsEffect(nullptr);
                         effect->deleteLater();
                     });

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::slideIn(
    QWidget* target, const QPoint& from, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::slideIn");

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);
    target->show();

    auto animation =
        std::make_unique<QPropertyAnimation>(effect, "translation");

    animation->setStartValue(from);
    animation->setEndValue(QPointF(0, 0));
    setupAnimation(animation.get(), config);

    // Cleanup effect when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished,
                     [target, effect]() {
                         target->setGraphicsEffect(nullptr);
                         effect->deleteLater();
                     });

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::slideOut(
    QWidget* target, const QPoint& to, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::slideOut");

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);

    auto animation =
        std::make_unique<QPropertyAnimation>(effect, "translation");

    animation->setStartValue(QPointF(0, 0));
    animation->setEndValue(to);
    setupAnimation(animation.get(), config);

    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished,
                     [target, effect]() {
                         target->hide();
                         target->setGraphicsEffect(nullptr);
                         effect->deleteLater();
                     });

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::rotateIn(
    QWidget* target, qreal fromAngle, const FluentAnimationConfig& config) {
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
    QObject::connect(animation.get(), &QPropertyAnimation::finished,
                     [target, effect]() {
                         target->setGraphicsEffect(nullptr);
                         effect->deleteLater();
                     });

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::rotateOut(
    QWidget* target, qreal toAngle, const FluentAnimationConfig& config) {
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
    QObject::connect(animation.get(), &QPropertyAnimation::finished,
                     [target, effect]() {
                         target->hide();
                         target->setGraphicsEffect(nullptr);
                         effect->deleteLater();
                     });

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::colorTransition(
    QObject* target, const QByteArray& property, const QColor& from,
    const QColor& to, const FluentAnimationConfig& config) {
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
    QWidget* target, int distance, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::slideUp");

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);
    target->show();

    auto animation =
        std::make_unique<QPropertyAnimation>(effect, "translation");

    animation->setStartValue(QPointF(0, distance));
    animation->setEndValue(QPointF(0, 0));
    setupAnimation(animation.get(), config);

    // Use appropriate easing for slide up (entrance)
    FluentAnimationConfig slideConfig = config;
    if (slideConfig.easing == FluentEasing::EaseOut) {
        slideConfig.easing =
            FluentEasing::EaseOutCubic;  // Fluent standard for entrances
    }
    animation->setEasingCurve(toQtEasing(slideConfig.easing));

    // Cleanup effect when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished,
                     [target, effect]() {
                         target->setGraphicsEffect(nullptr);
                         effect->deleteLater();
                     });

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::slideDown(
    QWidget* target, int distance, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::slideDown");

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);

    auto animation =
        std::make_unique<QPropertyAnimation>(effect, "translation");

    animation->setStartValue(QPointF(0, 0));
    animation->setEndValue(QPointF(0, distance));
    setupAnimation(animation.get(), config);

    // Use appropriate easing for slide down (exit)
    FluentAnimationConfig slideConfig = config;
    if (slideConfig.easing == FluentEasing::EaseOut) {
        slideConfig.easing =
            FluentEasing::EaseInCubic;  // Fluent standard for exits
    }
    animation->setEasingCurve(toQtEasing(slideConfig.easing));

    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished,
                     [target, effect]() {
                         target->hide();
                         target->setGraphicsEffect(nullptr);
                         effect->deleteLater();
                     });

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::slideLeft(
    QWidget* target, int distance, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::slideLeft");

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);

    auto animation =
        std::make_unique<QPropertyAnimation>(effect, "translation");

    animation->setStartValue(QPointF(0, 0));
    animation->setEndValue(QPointF(-distance, 0));
    setupAnimation(animation.get(), config);

    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished,
                     [target, effect]() {
                         target->hide();
                         target->setGraphicsEffect(nullptr);
                         effect->deleteLater();
                     });

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::slideRight(
    QWidget* target, int distance, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::slideRight");

    auto* effect = new FluentTransformEffect(target);
    target->setGraphicsEffect(effect);

    auto animation =
        std::make_unique<QPropertyAnimation>(effect, "translation");

    animation->setStartValue(QPointF(0, 0));
    animation->setEndValue(QPointF(distance, 0));
    setupAnimation(animation.get(), config);

    // Hide widget and cleanup when animation finishes
    QObject::connect(animation.get(), &QPropertyAnimation::finished,
                     [target, effect]() {
                         target->hide();
                         target->setGraphicsEffect(nullptr);
                         effect->deleteLater();
                     });

    return animation;
}

std::unique_ptr<QSequentialAnimationGroup> FluentAnimator::revealAnimation(
    QWidget* target, const QPoint& center,
    const FluentAnimationConfig& config) {
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
    scalePhase->setDuration(config.duration.count() *
                            0.6);  // 60% of total duration
    scalePhase->setEasingCurve(toQtEasing(FluentEasing::EaseOut));

    // Phase 2: Fade in effect
    auto fadePhase =
        std::make_unique<QPropertyAnimation>(opacityEffect, "opacity");
    fadePhase->setStartValue(0.0);
    fadePhase->setEndValue(1.0);
    fadePhase->setDuration(config.duration.count() *
                           0.4);  // 40% of total duration
    fadePhase->setEasingCurve(toQtEasing(FluentEasing::EaseOut));

    // Add phases to group
    group->addAnimation(scalePhase.release());
    group->addAnimation(fadePhase.release());

    // Cleanup when finished
    QObject::connect(group.get(), &QSequentialAnimationGroup::finished,
                     [target, opacityEffect]() {
                         target->setGraphicsEffect(nullptr);
                         opacityEffect->deleteLater();
                     });

    return group;
}

std::unique_ptr<QParallelAnimationGroup> FluentAnimator::connectedAnimation(
    QWidget* from, QWidget* to, const FluentAnimationConfig& config) {
    if (!from || !to) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::connectedAnimation");

    auto group = std::make_unique<QParallelAnimationGroup>();

    // Get geometries in global coordinates
    const QRect fromGeometry =
        QRect(from->mapToGlobal(QPoint(0, 0)), from->size());
    const QRect toGeometry = QRect(to->mapToGlobal(QPoint(0, 0)), to->size());

    // Create temporary widget for the connected animation
    QWidget* connector = new QWidget(from->window());
    connector->setStyleSheet(from->styleSheet());  // Copy appearance
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
    fadeInConfig.duration =
        std::chrono::milliseconds(config.duration.count() / 2);
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
    QObject::connect(group.get(), &QParallelAnimationGroup::finished,
                     [connector]() { connector->deleteLater(); });

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

        // Microsoft Fluent Design System specific curves
        case FluentEasing::FluentAccelerate:
        case FluentEasing::FluentDecelerate:
        case FluentEasing::FluentStandard:
        case FluentEasing::FluentEmphasized:
        case FluentEasing::FluentSubtle:
        case FluentEasing::FluentNormal:
        case FluentEasing::FluentExpressive:
        case FluentEasing::FluentButton:
        case FluentEasing::FluentCard:
        case FluentEasing::FluentDialog:
        case FluentEasing::FluentNavigation:
        case FluentEasing::FluentReveal:
            // These will be handled by custom Bezier curves
            return QEasingCurve::BezierSpline;

        default:
            return QEasingCurve::OutCubic;
    }
}

bool FluentAnimator::isFluentDesignEasing(FluentEasing easing) {
    switch (easing) {
        case FluentEasing::FluentAccelerate:
        case FluentEasing::FluentDecelerate:
        case FluentEasing::FluentStandard:
        case FluentEasing::FluentEmphasized:
        case FluentEasing::FluentSubtle:
        case FluentEasing::FluentNormal:
        case FluentEasing::FluentExpressive:
        case FluentEasing::FluentButton:
        case FluentEasing::FluentCard:
        case FluentEasing::FluentDialog:
        case FluentEasing::FluentNavigation:
        case FluentEasing::FluentReveal:
            return true;
        default:
            return false;
    }
}

QEasingCurve FluentAnimator::createFluentBezierCurve(FluentEasing easing) {
    QEasingCurve curve(QEasingCurve::BezierSpline);

    switch (easing) {
        case FluentEasing::FluentAccelerate:
            // Fluent accelerate curve (0.7, 0.0, 1.0, 1.0)
            curve.addCubicBezierSegment(QPointF(0.7, 0.0), QPointF(1.0, 1.0),
                                        QPointF(1.0, 1.0));
            break;
        case FluentEasing::FluentDecelerate:
            // Fluent decelerate curve (0.1, 0.9, 0.2, 1.0)
            curve.addCubicBezierSegment(QPointF(0.1, 0.9), QPointF(0.2, 1.0),
                                        QPointF(1.0, 1.0));
            break;
        case FluentEasing::FluentStandard:
            // Fluent standard curve (0.8, 0.0, 0.2, 1.0)
            curve.addCubicBezierSegment(QPointF(0.8, 0.0), QPointF(0.2, 1.0),
                                        QPointF(1.0, 1.0));
            break;
        case FluentEasing::FluentEmphasized:
            // Fluent emphasized curve (0.3, 0.0, 0.8, 0.15)
            curve.addCubicBezierSegment(QPointF(0.3, 0.0), QPointF(0.8, 0.15),
                                        QPointF(1.0, 1.0));
            break;
        case FluentEasing::FluentSubtle:
            // Subtle motion (0.33, 0.0, 0.67, 1.0)
            curve.addCubicBezierSegment(QPointF(0.33, 0.0), QPointF(0.67, 1.0),
                                        QPointF(1.0, 1.0));
            break;
        case FluentEasing::FluentNormal:
            // Normal motion (0.5, 0.0, 0.5, 1.0)
            curve.addCubicBezierSegment(QPointF(0.5, 0.0), QPointF(0.5, 1.0),
                                        QPointF(1.0, 1.0));
            break;
        case FluentEasing::FluentExpressive:
            // Expressive motion (0.68, -0.55, 0.265, 1.55)
            curve.addCubicBezierSegment(
                QPointF(0.68, -0.55), QPointF(0.265, 1.55), QPointF(1.0, 1.0));
            break;
        case FluentEasing::FluentButton:
            // Button interactions (0.1, 0.9, 0.2, 1.0)
            curve.addCubicBezierSegment(QPointF(0.1, 0.9), QPointF(0.2, 1.0),
                                        QPointF(1.0, 1.0));
            break;
        case FluentEasing::FluentCard:
            // Card animations (0.25, 0.46, 0.45, 0.94)
            curve.addCubicBezierSegment(QPointF(0.25, 0.46),
                                        QPointF(0.45, 0.94), QPointF(1.0, 1.0));
            break;
        case FluentEasing::FluentDialog:
            // Dialog transitions (0.0, 0.0, 0.2, 1.0)
            curve.addCubicBezierSegment(QPointF(0.0, 0.0), QPointF(0.2, 1.0),
                                        QPointF(1.0, 1.0));
            break;
        case FluentEasing::FluentNavigation:
            // Navigation transitions (0.4, 0.0, 0.2, 1.0)
            curve.addCubicBezierSegment(QPointF(0.4, 0.0), QPointF(0.2, 1.0),
                                        QPointF(1.0, 1.0));
            break;
        case FluentEasing::FluentReveal:
            // Reveal animations (0.0, 0.0, 0.2, 1.0)
            curve.addCubicBezierSegment(QPointF(0.0, 0.0), QPointF(0.2, 1.0),
                                        QPointF(1.0, 1.0));
            break;
        default:
            // Fallback to standard curve
            curve.addCubicBezierSegment(QPointF(0.8, 0.0), QPointF(0.2, 1.0),
                                        QPointF(1.0, 1.0));
            break;
    }

    return curve;
}

void FluentAnimator::setupAnimation(QPropertyAnimation* animation,
                                    const FluentAnimationConfig& config) {
    if (!animation)
        return;

    // Check if we should skip animations for performance or accessibility
    if (Core::FluentPerformanceMonitor::instance().shouldSkipAnimation() ||
        (config.respectReducedMotion && shouldRespectReducedMotion())) {
        animation->setDuration(0);
        return;
    }

    // Apply configuration
    animation->setDuration(config.duration.count());

    // Use custom Bezier curves for Fluent Design easing curves
    if (isFluentDesignEasing(config.easing)) {
        animation->setEasingCurve(createFluentBezierCurve(config.easing));
    } else {
        animation->setEasingCurve(toQtEasing(config.easing));
    }

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
            animation->setProperty("updateInterval", 16);  // 60 FPS
        } else {
            animation->setProperty("updateInterval", 33);  // 30 FPS fallback
        }

        // Enable additional performance optimizations
        enableHardwareAcceleration(animation);
    }

    // Apply motion hierarchy-based performance optimizations
    switch (config.hierarchy) {
        case FluentAnimationConfig::MotionHierarchy::Utility:
            // Micro-interactions: prioritize responsiveness
            animation->setProperty("priority", "high");
            animation->setProperty("updateInterval",
                                   8);  // 120 FPS for micro-interactions
            break;
        case FluentAnimationConfig::MotionHierarchy::Secondary:
            // Supporting elements: balanced performance
            animation->setProperty("priority", "normal");
            animation->setProperty("updateInterval", 16);  // 60 FPS
            break;
        case FluentAnimationConfig::MotionHierarchy::Primary:
            // Main content: smooth but efficient
            animation->setProperty("priority", "normal");
            animation->setProperty("updateInterval", 16);  // 60 FPS
            break;
    }

    // Register animation with performance monitor
    FluentAnimationManager::instance().registerAnimation(animation);
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::hoverEffect(
    QWidget* target, const FluentAnimationConfig& config) {
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

    auto animation =
        std::make_unique<QPropertyAnimation>(opacityEffect, "opacity");

    // Fluent Design hover: subtle opacity change
    animation->setStartValue(opacityEffect->opacity());
    animation->setEndValue(0.9);  // More subtle than 0.8

    // Use faster duration for micro-interactions
    FluentAnimationConfig hoverConfig = config;
    hoverConfig.duration =
        std::chrono::milliseconds(150);  // Fluent standard for hover
    hoverConfig.easing = FluentEasing::EaseOutQuad;  // Smooth but quick

    setupAnimation(animation.get(), hoverConfig);

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::pressEffect(
    QWidget* target, const FluentAnimationConfig& config) {
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
    pressConfig.duration =
        std::chrono::milliseconds(100);  // Fluent standard for press
    pressConfig.easing = FluentEasing::EaseOutQuart;  // Sharp response

    setupAnimation(animation.get(), pressConfig);

    // Auto-reverse the animation for press feedback
    animation->setLoopCount(2);
    animation->setProperty("direction", QAbstractAnimation::Backward);

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::focusEffect(
    QWidget* target, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::focusEffect");

    auto animation =
        std::make_unique<QPropertyAnimation>(target, "windowOpacity");
    animation->setStartValue(0.8);
    animation->setEndValue(1.0);
    setupAnimation(animation.get(), config);

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::pulseEffect(
    QWidget* target, const FluentAnimationConfig& config) {
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

    auto animation =
        std::make_unique<QPropertyAnimation>(opacityEffect, "opacity");
    animation->setStartValue(1.0);
    animation->setEndValue(0.6);  // More subtle pulse
    animation->setLoopCount(-1);  // Infinite

    // Use appropriate timing for pulse
    FluentAnimationConfig pulseConfig = config;
    pulseConfig.duration = std::chrono::milliseconds(1000);  // Slower pulse
    pulseConfig.easing = FluentEasing::EaseInOutSine;        // Smooth pulse

    setupAnimation(animation.get(), pulseConfig);

    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::shakeEffect(
    QWidget* target, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::shakeEffect");

    // Create a shake animation by animating the widget's position
    const QPoint originalPos = target->pos();
    const int shakeDistance = 10;  // pixels to shake

    auto animation = std::make_unique<QPropertyAnimation>(target, "pos");

    // Create shake keyframes
    animation->setKeyValueAt(0.0, originalPos);
    animation->setKeyValueAt(0.1, originalPos + QPoint(shakeDistance, 0));
    animation->setKeyValueAt(0.2, originalPos + QPoint(-shakeDistance, 0));
    animation->setKeyValueAt(0.3, originalPos + QPoint(shakeDistance, 0));
    animation->setKeyValueAt(0.4, originalPos + QPoint(-shakeDistance, 0));
    animation->setKeyValueAt(0.5, originalPos + QPoint(shakeDistance, 0));
    animation->setKeyValueAt(0.6, originalPos + QPoint(-shakeDistance, 0));
    animation->setKeyValueAt(0.7, originalPos + QPoint(shakeDistance, 0));
    animation->setKeyValueAt(0.8, originalPos + QPoint(-shakeDistance, 0));
    animation->setKeyValueAt(0.9, originalPos + QPoint(shakeDistance, 0));
    animation->setKeyValueAt(1.0, originalPos);

    // Configure shake animation
    FluentAnimationConfig shakeConfig = config;
    shakeConfig.duration = std::chrono::milliseconds(500);  // Quick shake
    shakeConfig.easing = FluentEasing::EaseInOutQuad;  // Sharp shake motion

    setupAnimation(animation.get(), shakeConfig);

    return animation;
}

std::unique_ptr<QSequentialAnimationGroup> FluentAnimator::rippleEffect(
    QWidget* target, const QPoint& center,
    const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::rippleEffect");
    Q_UNUSED(config);  // Future use for customization

    auto group = std::make_unique<QSequentialAnimationGroup>();

    // Create a temporary widget for the ripple effect
    QWidget* rippleWidget = new QWidget(target);
    rippleWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    rippleWidget->setStyleSheet(
        "background-color: rgba(255, 255, 255, 100); border-radius: 50%;");

    // Position the ripple at the click center
    QPoint actualCenter = center.isNull() ? target->rect().center() : center;
    const int startSize = 10;
    const int endSize = qMax(target->width(), target->height()) * 2;

    rippleWidget->setGeometry(actualCenter.x() - startSize / 2,
                              actualCenter.y() - startSize / 2, startSize,
                              startSize);
    rippleWidget->show();

    // Phase 1: Expand the ripple
    auto expandAnim =
        std::make_unique<QPropertyAnimation>(rippleWidget, "geometry");
    QRect startRect(actualCenter.x() - startSize / 2,
                    actualCenter.y() - startSize / 2, startSize, startSize);
    QRect endRect(actualCenter.x() - endSize / 2,
                  actualCenter.y() - endSize / 2, endSize, endSize);

    expandAnim->setStartValue(startRect);
    expandAnim->setEndValue(endRect);
    expandAnim->setDuration(400);  // Fluent ripple duration
    expandAnim->setEasingCurve(QEasingCurve::OutCubic);

    // Phase 2: Fade out the ripple
    QGraphicsOpacityEffect* rippleOpacity =
        new QGraphicsOpacityEffect(rippleWidget);
    rippleWidget->setGraphicsEffect(rippleOpacity);

    auto fadeAnim =
        std::make_unique<QPropertyAnimation>(rippleOpacity, "opacity");
    fadeAnim->setStartValue(1.0);
    fadeAnim->setEndValue(0.0);
    fadeAnim->setDuration(200);
    fadeAnim->setEasingCurve(QEasingCurve::OutQuad);

    // Add animations to group
    group->addAnimation(expandAnim.release());
    group->addAnimation(fadeAnim.release());

    // Cleanup when finished
    QObject::connect(group.get(), &QSequentialAnimationGroup::finished,
                     [rippleWidget]() { rippleWidget->deleteLater(); });

    return group;
}

void FluentAnimator::applyMicroInteractionSettings(
    QPropertyAnimation* animation, const FluentAnimationConfig& config) {
    if (!animation)
        return;

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
        QObject::connect(animation, &QPropertyAnimation::finished, animation,
                         &QPropertyAnimation::deleteLater);
    }
}

void FluentAnimator::enableHardwareAcceleration(QPropertyAnimation* animation) {
    if (!animation) {
        return;
    }

    // Enable hardware acceleration hints for better performance
    animation->setProperty("hardwareAccelerated", true);

    // Set optimal update intervals based on hardware capabilities
    if (QOpenGLContext::currentContext()) {
        // GPU acceleration available
        animation->setProperty("updateInterval", 16);  // 60 FPS
        animation->setProperty("gpuAccelerated", true);
    } else {
        // Fallback to CPU optimization
        animation->setProperty("updateInterval", 33);  // 30 FPS
        animation->setProperty("cpuOptimized", true);
    }

    // Enable compositing hints for smoother animations
    if (QWidget* widget = qobject_cast<QWidget*>(animation->targetObject())) {
        widget->setAttribute(Qt::WA_OpaquePaintEvent, true);
        widget->setAttribute(Qt::WA_NoSystemBackground, true);
    }
}

bool FluentAnimator::shouldRespectReducedMotion() {
    // Enhanced accessibility support for reduced motion

    // Check if accessibility is active
    if (QAccessible::isActive()) {
        return true;
    }

    // Check system-specific reduced motion settings
#ifdef Q_OS_WIN
    // Windows: Check for reduced motion preference
    BOOL enabled = FALSE;
    if (SystemParametersInfo(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0)) {
        return !enabled;  // If client area animation is disabled, respect
                          // reduced motion
    }
#endif

#ifdef Q_OS_MAC
    // macOS: Check for reduced motion preference
    // This would require Objective-C code to check
    // NSWorkspace.accessibilityDisplayShouldReduceMotion For now, we'll use a
    // simplified check
    QSettings settings("com.apple.universalaccess",
                       "com.apple.universalaccess");
    if (settings.value("reduceMotion", false).toBool()) {
        return true;
    }
#endif

#ifdef Q_OS_LINUX
    // Linux: Check for reduced motion preference in accessibility settings
    QSettings settings("org.gnome.desktop.interface",
                       "org.gnome.desktop.interface");
    if (settings.value("enable-animations", true).toBool() == false) {
        return true;
    }
#endif

    // Check FluentQt-specific reduced motion setting
    auto& theme = FluentQt::Styling::FluentTheme::instance();
    if (theme.isReducedMotionMode()) {
        return true;
    }

    // Check for low-performance devices
    if (Core::FluentPerformanceMonitor::instance().shouldSkipAnimation()) {
        return true;
    }

    return false;
}

// FluentAnimationManager implementation
FluentAnimationManager& FluentAnimationManager::instance() {
    static FluentAnimationManager instance;
    return instance;
}

void FluentAnimationManager::registerAnimation(QAbstractAnimation* animation) {
    if (!animation) {
        return;
    }

    m_activeAnimations.insert(animation);

    // Enhanced cleanup for finished animations
    connect(animation, &QAbstractAnimation::finished, [this, animation]() {
        m_activeAnimations.remove(animation);

        // Clean up any associated graphics effects
        if (auto* propAnim = qobject_cast<QPropertyAnimation*>(animation)) {
            if (auto* widget =
                    qobject_cast<QWidget*>(propAnim->targetObject())) {
                cleanupWidgetAnimations(widget);
            }
        }
    });

    // Enhanced cleanup for destroyed animations
    connect(animation, &QAbstractAnimation::destroyed, [this, animation]() {
        m_activeAnimations.remove(animation);
        m_pausedAnimations.remove(animation);
    });

    // Monitor target object destruction for proper cleanup
    if (auto* propAnim = qobject_cast<QPropertyAnimation*>(animation)) {
        if (QObject* target = propAnim->targetObject()) {
            connect(target, &QObject::destroyed, [this, animation]() {
                // Stop and remove animation when target is destroyed
                if (m_activeAnimations.contains(animation)) {
                    animation->stop();
                    m_activeAnimations.remove(animation);
                    m_pausedAnimations.remove(animation);
                    animation->deleteLater();
                }
            });
        }
    }
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
    QWidget* target, int durationMs, FluentEasing easing) {
    FluentAnimationConfig cfg;
    cfg.duration = std::chrono::milliseconds(durationMs);
    cfg.easing = easing;
    return FluentAnimator::fadeIn(target, cfg);
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::fadeOut(
    QWidget* target, int durationMs, FluentEasing easing) {
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

void FluentAnimationManager::cleanupWidgetAnimations(QWidget* widget) {
    if (!widget) {
        return;
    }

    // Clean up any graphics effects that might be left over
    if (auto* effect = widget->graphicsEffect()) {
        if (auto* opacityEffect =
                qobject_cast<QGraphicsOpacityEffect*>(effect)) {
            // Reset opacity effect to default state
            opacityEffect->setOpacity(1.0);
        }
    }

    // Reset any animation-related widget attributes
    widget->setAttribute(Qt::WA_OpaquePaintEvent, false);
    widget->setAttribute(Qt::WA_NoSystemBackground, false);

    // Ensure widget is in a clean state
    widget->setWindowOpacity(1.0);
    widget->update();
}

int FluentAnimationManager::activeAnimationCount() const {
    return m_activeAnimations.size();
}

// ElaWidgetTools-inspired smooth scroll animation
std::unique_ptr<QPropertyAnimation> FluentAnimator::smoothScroll(
    QWidget* target, int fromValue, int toValue, int durationMs,
    FluentEasing easing) {
    if (!target)
        return nullptr;

    auto animation = std::make_unique<QPropertyAnimation>(target, "value");
    animation->setStartValue(fromValue);
    animation->setEndValue(toValue);
    animation->setDuration(durationMs);
    animation->setEasingCurve(toQtEasing(easing));

    // Apply ElaWidgetTools-style smooth scrolling optimizations
    animation->setProperty("smoothScrolling", true);

    // Enable hardware acceleration hints for smooth scrolling
    if (QOpenGLContext::currentContext()) {
        animation->setProperty("hardwareAccelerated", true);
        animation->setProperty("updateInterval", 16);  // 60 FPS
    }

    return animation;
}

// Enhanced expand/collapse animation with proper timing
std::unique_ptr<QPropertyAnimation> FluentAnimator::expandAnimation(
    QWidget* target, bool expand, int durationMs, FluentEasing easing) {
    if (!target)
        return nullptr;

    auto animation =
        std::make_unique<QPropertyAnimation>(target, "maximumHeight");

    if (expand) {
        // Expanding: from 0 to sizeHint height
        animation->setStartValue(0);
        animation->setEndValue(target->sizeHint().height());
    } else {
        // Collapsing: from current height to 0
        animation->setStartValue(target->height());
        animation->setEndValue(0);
    }

    animation->setDuration(durationMs);
    animation->setEasingCurve(toQtEasing(easing));

    // Apply ElaWidgetTools-style expand timing
    // Use a slightly longer duration for expand to feel more natural
    if (expand) {
        animation->setDuration(durationMs + 50);
    }

    // Connect to handle widget visibility
    QObject::connect(
        animation.get(), &QPropertyAnimation::finished, [target, expand]() {
            if (!expand) {
                target->hide();
            }
            target->setMaximumHeight(QWIDGETSIZE_MAX);  // Reset max height
        });

    if (expand) {
        target->show();
    }

    return animation;
}

std::unique_ptr<QParallelAnimationGroup> FluentAnimator::navigationTransition(
    QWidget* fromWidget, QWidget* toWidget,
    const FluentAnimationConfig& config) {
    if (!fromWidget || !toWidget) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::navigationTransition");

    auto group = std::make_unique<QParallelAnimationGroup>();

    // Exit animation for current widget
    auto exitAnimation =
        std::make_unique<QPropertyAnimation>(fromWidget, "geometry");
    const QRect fromGeometry = fromWidget->geometry();
    const QRect exitGeometry(fromGeometry.x() - fromGeometry.width() / 4,
                             fromGeometry.y(), fromGeometry.width(),
                             fromGeometry.height());

    exitAnimation->setStartValue(fromGeometry);
    exitAnimation->setEndValue(exitGeometry);

    // Fade out current widget
    auto fadeOutAnimation =
        std::make_unique<QPropertyAnimation>(fromWidget, "windowOpacity");
    fadeOutAnimation->setStartValue(1.0);
    fadeOutAnimation->setEndValue(0.0);

    // Enter animation for new widget
    auto enterAnimation =
        std::make_unique<QPropertyAnimation>(toWidget, "geometry");
    const QRect toGeometry = toWidget->geometry();
    const QRect enterStartGeometry(toGeometry.x() + toGeometry.width() / 4,
                                   toGeometry.y(), toGeometry.width(),
                                   toGeometry.height());

    toWidget->setGeometry(enterStartGeometry);
    toWidget->show();

    enterAnimation->setStartValue(enterStartGeometry);
    enterAnimation->setEndValue(toGeometry);

    // Fade in new widget
    auto fadeInAnimation =
        std::make_unique<QPropertyAnimation>(toWidget, "windowOpacity");
    toWidget->setWindowOpacity(0.0);
    fadeInAnimation->setStartValue(0.0);
    fadeInAnimation->setEndValue(1.0);

    // Use Fluent navigation curves and timing
    FluentAnimationConfig navConfig = config;
    navConfig.duration =
        std::chrono::milliseconds(300);  // Fluent navigation duration
    navConfig.easing =
        FluentEasing::FluentNavigation;  // Fluent navigation curve

    setupAnimation(exitAnimation.get(), navConfig);
    setupAnimation(fadeOutAnimation.get(), navConfig);
    setupAnimation(enterAnimation.get(), navConfig);
    setupAnimation(fadeInAnimation.get(), navConfig);

    group->addAnimation(exitAnimation.release());
    group->addAnimation(fadeOutAnimation.release());
    group->addAnimation(enterAnimation.release());
    group->addAnimation(fadeInAnimation.release());

    return group;
}

std::unique_ptr<QParallelAnimationGroup>
FluentAnimator::connectedElementAnimation(QWidget* fromElement,
                                          QWidget* toElement,
                                          QWidget* fromContainer,
                                          QWidget* toContainer,
                                          const FluentAnimationConfig& config) {
    if (!fromElement || !toElement || !fromContainer || !toContainer) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::connectedElementAnimation");

    auto group = std::make_unique<QParallelAnimationGroup>();

    // Calculate the transformation between the two elements
    QRect fromRect = fromElement->geometry();
    QRect toRect = toElement->geometry();

    // Convert to global coordinates for accurate positioning
    QPoint fromGlobal = fromContainer->mapToGlobal(fromRect.topLeft());
    QPoint toGlobal = toContainer->mapToGlobal(toRect.topLeft());

    // Create a temporary widget for the shared element transition
    auto* sharedElement = new QWidget(fromContainer->window());
    sharedElement->setGeometry(fromGlobal.x(), fromGlobal.y(), fromRect.width(),
                               fromRect.height());
    sharedElement->setStyleSheet(fromElement->styleSheet());
    sharedElement->show();

    // Hide the original elements during transition
    fromElement->hide();
    toElement->hide();

    // Animate the shared element from source to destination
    auto* moveAnimation = new QPropertyAnimation(sharedElement, "geometry");
    moveAnimation->setStartValue(QRect(fromGlobal, fromRect.size()));
    moveAnimation->setEndValue(QRect(toGlobal, toRect.size()));

    // Use Fluent emphasized curve for connected animations
    FluentAnimationConfig connectedConfig = config;
    connectedConfig.duration =
        std::chrono::milliseconds(400);  // Longer for connected animations
    connectedConfig.easing = FluentEasing::FluentEmphasized;

    setupAnimation(moveAnimation, connectedConfig);

    // Fade out source container
    auto* fadeOutAnimation =
        new QPropertyAnimation(fromContainer, "windowOpacity");
    fadeOutAnimation->setStartValue(1.0);
    fadeOutAnimation->setEndValue(0.3);
    setupAnimation(fadeOutAnimation, connectedConfig);

    // Fade in destination container
    auto* fadeInAnimation =
        new QPropertyAnimation(toContainer, "windowOpacity");
    fadeInAnimation->setStartValue(0.3);
    fadeInAnimation->setEndValue(1.0);
    setupAnimation(fadeInAnimation, connectedConfig);

    group->addAnimation(moveAnimation);
    group->addAnimation(fadeOutAnimation);
    group->addAnimation(fadeInAnimation);

    // Clean up when animation finishes
    connect(group.get(), &QParallelAnimationGroup::finished,
            [sharedElement, fromElement, toElement]() {
                sharedElement->deleteLater();
                fromElement->show();
                toElement->show();
            });

    return group;
}

std::unique_ptr<QParallelAnimationGroup> FluentAnimator::entranceAnimation(
    QWidget* target, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::entranceAnimation");

    auto group = std::make_unique<QParallelAnimationGroup>();

    // Fluent Design entrance: fade + slide up + scale
    auto* fadeAnimation = new QPropertyAnimation(target, "windowOpacity");
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);

    auto* slideAnimation = new QPropertyAnimation(target, "geometry");
    QRect currentGeometry = target->geometry();
    QRect startGeometry =
        currentGeometry.translated(0, 20);  // Start 20px below
    slideAnimation->setStartValue(startGeometry);
    slideAnimation->setEndValue(currentGeometry);

    auto* scaleAnimation = new QPropertyAnimation(target, "geometry");
    QRect scaledGeometry =
        QRect(currentGeometry.x() + currentGeometry.width() * 0.05,
              currentGeometry.y() + currentGeometry.height() * 0.05,
              currentGeometry.width() * 0.9, currentGeometry.height() * 0.9);
    scaleAnimation->setStartValue(scaledGeometry);
    scaleAnimation->setEndValue(currentGeometry);

    // Use Fluent entrance timing and curves
    FluentAnimationConfig entranceConfig = config;
    entranceConfig.duration = std::chrono::milliseconds(300);
    entranceConfig.easing = FluentEasing::FluentStandard;

    setupAnimation(fadeAnimation, entranceConfig);
    setupAnimation(slideAnimation, entranceConfig);
    setupAnimation(scaleAnimation, entranceConfig);

    group->addAnimation(fadeAnimation);
    group->addAnimation(slideAnimation);
    group->addAnimation(scaleAnimation);

    return group;
}

std::unique_ptr<QParallelAnimationGroup> FluentAnimator::exitAnimation(
    QWidget* target, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::exitAnimation");

    auto group = std::make_unique<QParallelAnimationGroup>();

    // Fluent Design exit: fade + slide down + scale
    auto* fadeAnimation = new QPropertyAnimation(target, "windowOpacity");
    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);

    auto* slideAnimation = new QPropertyAnimation(target, "geometry");
    QRect currentGeometry = target->geometry();
    QRect endGeometry = currentGeometry.translated(0, 20);  // End 20px below
    slideAnimation->setStartValue(currentGeometry);
    slideAnimation->setEndValue(endGeometry);

    auto* scaleAnimation = new QPropertyAnimation(target, "geometry");
    QRect scaledGeometry =
        QRect(currentGeometry.x() + currentGeometry.width() * 0.05,
              currentGeometry.y() + currentGeometry.height() * 0.05,
              currentGeometry.width() * 0.9, currentGeometry.height() * 0.9);
    scaleAnimation->setStartValue(currentGeometry);
    scaleAnimation->setEndValue(scaledGeometry);

    // Use Fluent exit timing and curves
    FluentAnimationConfig exitConfig = config;
    exitConfig.duration = std::chrono::milliseconds(200);  // Faster exit
    exitConfig.easing = FluentEasing::FluentAccelerate;    // Accelerate out

    setupAnimation(fadeAnimation, exitConfig);
    setupAnimation(slideAnimation, exitConfig);
    setupAnimation(scaleAnimation, exitConfig);

    group->addAnimation(fadeAnimation);
    group->addAnimation(slideAnimation);
    group->addAnimation(scaleAnimation);

    return group;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::primaryMotion(
    QWidget* target, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::primaryMotion");

    auto animation =
        std::make_unique<QPropertyAnimation>(target, "windowOpacity");
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);

    // Primary motion: 250ms with Fluent standard curve
    FluentAnimationConfig primaryConfig = config;
    primaryConfig.duration = std::chrono::milliseconds(250);
    primaryConfig.easing = FluentEasing::FluentStandard;
    primaryConfig.hierarchy = FluentAnimationConfig::MotionHierarchy::Primary;

    setupAnimation(animation.get(), primaryConfig);
    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::secondaryMotion(
    QWidget* target, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::secondaryMotion");

    auto animation =
        std::make_unique<QPropertyAnimation>(target, "windowOpacity");
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);

    // Secondary motion: 150ms with Fluent subtle curve
    FluentAnimationConfig secondaryConfig = config;
    secondaryConfig.duration = std::chrono::milliseconds(150);
    secondaryConfig.easing = FluentEasing::FluentSubtle;
    secondaryConfig.hierarchy =
        FluentAnimationConfig::MotionHierarchy::Secondary;

    setupAnimation(animation.get(), secondaryConfig);
    return animation;
}

std::unique_ptr<QPropertyAnimation> FluentAnimator::utilityMotion(
    QWidget* target, const FluentAnimationConfig& config) {
    if (!target) {
        return nullptr;
    }

    FLUENT_PROFILE("FluentAnimator::utilityMotion");

    auto animation =
        std::make_unique<QPropertyAnimation>(target, "windowOpacity");
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);

    // Utility motion: 100ms with Fluent button curve for micro-interactions
    FluentAnimationConfig utilityConfig = config;
    utilityConfig.duration = std::chrono::milliseconds(100);
    utilityConfig.easing = FluentEasing::FluentButton;
    utilityConfig.hierarchy = FluentAnimationConfig::MotionHierarchy::Utility;

    setupAnimation(animation.get(), utilityConfig);
    return animation;
}

}  // namespace FluentQt::Animation
