// examples/FluentVisualFeedbackManager.cpp
#include "FluentVisualFeedbackManager.h"
#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsColorizeEffect>
#include <QTimer>
#include <QApplication>
#include <QDebug>

namespace FluentQt::Examples {

FluentVisualFeedbackManager::FluentVisualFeedbackManager(QObject* parent)
    : QObject(parent)
    , m_cleanupTimer(new QTimer(this))
    , m_effectTimer(new QTimer(this))
{
    // Setup cleanup timer to remove finished animations
    m_cleanupTimer->setInterval(1000);
    m_cleanupTimer->setSingleShot(false);
    connect(m_cleanupTimer, &QTimer::timeout, this, [this]() {
        // Clean up finished animations
        auto it = m_activeAnimations.begin();
        while (it != m_activeAnimations.end()) {
            if ((*it)->state() == QAbstractAnimation::Stopped) {
                (*it)->deleteLater();
                it = m_activeAnimations.erase(it);
            } else {
                ++it;
            }
        }
    });
    m_cleanupTimer->start();
}

FluentVisualFeedbackManager::~FluentVisualFeedbackManager() {
    stopAllAnimations();
    clearAllEffects();
}

void FluentVisualFeedbackManager::animateThemeTransition(QWidget* widget, int duration) {
    if (!widget) return;

    // Create fade out and fade in sequence
    auto* fadeOut = createPropertyAnimation(widget, "windowOpacity");
    fadeOut->setDuration(duration / 2);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.3);
    fadeOut->setEasingCurve(QEasingCurve::InQuad);

    auto* fadeIn = createPropertyAnimation(widget, "windowOpacity");
    fadeIn->setDuration(duration / 2);
    fadeIn->setStartValue(0.3);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::OutQuad);

    // Connect animations
    connect(fadeOut, &QPropertyAnimation::finished, [this, fadeIn]() {
        fadeIn->start();
    });

    connect(fadeIn, &QPropertyAnimation::finished, [this, widget]() {
        emit animationFinished(widget, AnimationType::FadeIn);
    });

    fadeOut->start();
    m_activeAnimations.append(fadeOut);
    m_activeAnimations.append(fadeIn);

    emit animationStarted(widget, AnimationType::FadeIn);
}

void FluentVisualFeedbackManager::showSuccessFeedback(QWidget* widget, const QString& message) {
    if (!widget) return;

    // Create green glow effect
    auto* colorEffect = createColorizeEffect(widget, QColor(0, 255, 0, 100));
    widget->setGraphicsEffect(colorEffect);

    // Animate the effect
    auto* animation = createPropertyAnimation(colorEffect, "strength");
    animation->setDuration(500);
    animation->setStartValue(0.0);
    animation->setEndValue(0.5);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    // Auto-remove effect after animation
    connect(animation, &QPropertyAnimation::finished, [this, widget, colorEffect]() {
        QTimer::singleShot(1000, [widget, colorEffect]() {
            if (widget && colorEffect) {
                widget->setGraphicsEffect(nullptr);
                colorEffect->deleteLater();
            }
        });
        emit animationFinished(widget, AnimationType::Glow);
    });

    animation->start();
    m_activeAnimations.append(animation);
    m_activeEffects.append(colorEffect);

    emit feedbackShown(widget, FeedbackType::Success, message);
    emit animationStarted(widget, AnimationType::Glow);
}

void FluentVisualFeedbackManager::showErrorFeedback(QWidget* widget, const QString& message) {
    if (!widget) return;

    // Create shake animation
    animateShake(widget, 400);

    // Add red glow
    auto* colorEffect = createColorizeEffect(widget, QColor(255, 0, 0, 100));
    widget->setGraphicsEffect(colorEffect);

    // Auto-remove effect
    QTimer::singleShot(1500, [widget, colorEffect]() {
        if (widget && colorEffect) {
            widget->setGraphicsEffect(nullptr);
            colorEffect->deleteLater();
        }
    });

    m_activeEffects.append(colorEffect);
    emit feedbackShown(widget, FeedbackType::Error, message);
}

void FluentVisualFeedbackManager::animateButtonPress(QWidget* button) {
    if (!button) return;

    // Scale down and back up
    auto* scaleDown = createPropertyAnimation(button, "geometry");
    QRect originalGeometry = button->geometry();
    QRect scaledGeometry = originalGeometry.adjusted(2, 2, -2, -2);

    scaleDown->setDuration(100);
    scaleDown->setStartValue(originalGeometry);
    scaleDown->setEndValue(scaledGeometry);
    scaleDown->setEasingCurve(QEasingCurve::InQuad);

    auto* scaleUp = createPropertyAnimation(button, "geometry");
    scaleUp->setDuration(100);
    scaleUp->setStartValue(scaledGeometry);
    scaleUp->setEndValue(originalGeometry);
    scaleUp->setEasingCurve(QEasingCurve::OutQuad);

    connect(scaleDown, &QPropertyAnimation::finished, [scaleUp]() {
        scaleUp->start();
    });

    connect(scaleUp, &QPropertyAnimation::finished, [this, button]() {
        emit animationFinished(button, AnimationType::Scale);
    });

    scaleDown->start();
    m_activeAnimations.append(scaleDown);
    m_activeAnimations.append(scaleUp);

    emit animationStarted(button, AnimationType::Scale);
}

void FluentVisualFeedbackManager::animateShake(QWidget* widget, int duration) {
    if (!widget) return;

    QRect originalGeometry = widget->geometry();
    auto* animation = createPropertyAnimation(widget, "geometry");
    animation->setDuration(duration);
    animation->setEasingCurve(QEasingCurve::InOutSine);

    // Create shake keyframes
    animation->setKeyValueAt(0.0, originalGeometry);
    animation->setKeyValueAt(0.1, originalGeometry.translated(-5, 0));
    animation->setKeyValueAt(0.3, originalGeometry.translated(5, 0));
    animation->setKeyValueAt(0.5, originalGeometry.translated(-3, 0));
    animation->setKeyValueAt(0.7, originalGeometry.translated(3, 0));
    animation->setKeyValueAt(0.9, originalGeometry.translated(-1, 0));
    animation->setKeyValueAt(1.0, originalGeometry);

    connect(animation, &QPropertyAnimation::finished, [this, widget]() {
        emit animationFinished(widget, AnimationType::Shake);
    });

    animation->start();
    m_activeAnimations.append(animation);

    emit animationStarted(widget, AnimationType::Shake);
}

void FluentVisualFeedbackManager::animatePulse(QWidget* widget, int duration) {
    if (!widget) return;

    auto* opacityEffect = createOpacityEffect(widget);
    widget->setGraphicsEffect(opacityEffect);

    auto* animation = createPropertyAnimation(opacityEffect, "opacity");
    animation->setDuration(duration);
    animation->setStartValue(1.0);
    animation->setEndValue(0.3);
    animation->setEasingCurve(QEasingCurve::InOutSine);
    animation->setLoopCount(3);
    animation->setDirection(QAbstractAnimation::Alternate);

    connect(animation, &QPropertyAnimation::finished, [this, widget, opacityEffect]() {
        widget->setGraphicsEffect(nullptr);
        opacityEffect->deleteLater();
        emit animationFinished(widget, AnimationType::Pulse);
    });

    animation->start();
    m_activeAnimations.append(animation);
    m_activeEffects.append(opacityEffect);

    emit animationStarted(widget, AnimationType::Pulse);
}

void FluentVisualFeedbackManager::stopAllAnimations() {
    for (auto* animation : m_activeAnimations) {
        if (animation) {
            animation->stop();
            animation->deleteLater();
        }
    }
    m_activeAnimations.clear();
}

void FluentVisualFeedbackManager::clearAllEffects() {
    for (auto* effect : m_activeEffects) {
        if (effect) {
            effect->deleteLater();
        }
    }
    m_activeEffects.clear();
}

QPropertyAnimation* FluentVisualFeedbackManager::createPropertyAnimation(QWidget* widget, const QByteArray& property) {
    auto* animation = new QPropertyAnimation(widget, property, this);
    animation->setEasingCurve(m_defaultEasing);
    animation->setDuration(m_defaultDuration);
    return animation;
}

QGraphicsOpacityEffect* FluentVisualFeedbackManager::createOpacityEffect(QWidget* widget) {
    Q_UNUSED(widget)
    auto* effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(1.0);
    return effect;
}

QGraphicsColorizeEffect* FluentVisualFeedbackManager::createColorizeEffect(QWidget* widget, const QColor& color) {
    Q_UNUSED(widget)
    auto* effect = new QGraphicsColorizeEffect(this);
    effect->setColor(color);
    effect->setStrength(0.0);
    return effect;
}

void FluentVisualFeedbackManager::onThemeChanged() {
    // Update any theme-dependent visual effects
    qDebug() << "Visual feedback manager: Theme changed";
}

void FluentVisualFeedbackManager::onComponentStateChanged(QWidget* widget, const QString& state) {
    if (!widget) return;

    if (state == "success") {
        showSuccessFeedback(widget);
    } else if (state == "error") {
        showErrorFeedback(widget);
    } else if (state == "pressed") {
        animateButtonPress(widget);
    }
}

} // namespace FluentQt::Examples
