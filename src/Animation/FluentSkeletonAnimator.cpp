// src/Animation/FluentSkeletonAnimator.cpp
#include "FluentQt/Animation/FluentSkeletonAnimator.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QEasingCurve>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QParallelAnimationGroup>
#include <QVBoxLayout>
#include <QtMath>

namespace FluentQt::Animation {

// FluentSkeletonWidget implementation
FluentSkeletonWidget::FluentSkeletonWidget(const FluentSkeletonConfig& config,
                                           QWidget* parent)
    : QWidget(parent), m_config(config) {
    setFixedSize(config.customSize);
    setupAnimation();

    // Set background to transparent
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_NoSystemBackground, true);
}

void FluentSkeletonWidget::setConfig(const FluentSkeletonConfig& config) {
    m_config = config;
    setFixedSize(config.customSize);
    setupAnimation();
    update();
}

void FluentSkeletonWidget::startAnimation() {
    if (m_animating)
        return;

    m_animating = true;

    if (m_config.enableShimmer && m_shimmerAnimation) {
        m_shimmerAnimation->start();
    }

    if (m_config.pattern == FluentLoadingPattern::Pulse && m_pulseAnimation) {
        m_pulseAnimation->start();
    }
}

void FluentSkeletonWidget::stopAnimation() {
    if (!m_animating)
        return;

    m_animating = false;

    if (m_shimmerAnimation) {
        m_shimmerAnimation->stop();
    }

    if (m_pulseAnimation) {
        m_pulseAnimation->stop();
    }
}

void FluentSkeletonWidget::setShimmerPosition(qreal position) {
    m_shimmerPosition = position;
    update();
}

void FluentSkeletonWidget::setPulseOpacity(qreal opacity) {
    m_pulseOpacity = opacity;
    update();
}

void FluentSkeletonWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    paintSkeleton(painter);

    if (m_animating && m_config.enableShimmer) {
        paintShimmer(painter);
    }
}

void FluentSkeletonWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    setupAnimation();
}

void FluentSkeletonWidget::setupAnimation() {
    // Setup shimmer animation
    if (m_config.enableShimmer) {
        m_shimmerAnimation =
            std::make_unique<QPropertyAnimation>(this, "shimmerPosition");
        m_shimmerAnimation->setStartValue(-1.0);
        m_shimmerAnimation->setEndValue(2.0);
        m_shimmerAnimation->setDuration(m_config.animationDuration);
        m_shimmerAnimation->setEasingCurve(QEasingCurve::InOutSine);
        m_shimmerAnimation->setLoopCount(-1);  // Infinite
    }

    // Setup pulse animation
    if (m_config.pattern == FluentLoadingPattern::Pulse) {
        m_pulseAnimation =
            std::make_unique<QPropertyAnimation>(this, "pulseOpacity");
        m_pulseAnimation->setStartValue(0.3);
        m_pulseAnimation->setEndValue(1.0);
        m_pulseAnimation->setDuration(m_config.animationDuration / 2);
        m_pulseAnimation->setEasingCurve(QEasingCurve::InOutSine);
        m_pulseAnimation->setLoopCount(-1);
        m_pulseAnimation->setProperty("direction",
                                      QAbstractAnimation::Backward);
    }
}

void FluentSkeletonWidget::paintSkeleton(QPainter& painter) {
    QColor baseColor = m_config.baseColor;

    if (m_config.pattern == FluentLoadingPattern::Pulse) {
        baseColor.setAlphaF(m_pulseOpacity);
    }

    painter.setBrush(QBrush(baseColor));
    painter.setPen(Qt::NoPen);

    QRect rect = this->rect();

    switch (m_config.type) {
        case FluentSkeletonType::Text:
            painter.drawRoundedRect(rect, m_config.cornerRadius,
                                    m_config.cornerRadius);
            break;

        case FluentSkeletonType::Avatar:
            painter.drawEllipse(rect);
            break;

        case FluentSkeletonType::Image:
        case FluentSkeletonType::Card:
        case FluentSkeletonType::Custom:
            painter.drawRoundedRect(rect, m_config.cornerRadius,
                                    m_config.cornerRadius);
            break;

        case FluentSkeletonType::List:
            // Draw list item with icon and text areas
            painter.drawEllipse(QRect(8, 8, 24, 24));  // Icon area
            painter.drawRoundedRect(QRect(40, 12, rect.width() - 48, 16), 2,
                                    2);  // Text area
            break;

        case FluentSkeletonType::Table:
            // Draw table row with columns
            int columnWidth = rect.width() / 3;
            for (int i = 0; i < 3; ++i) {
                QRect columnRect(i * columnWidth + 4, 8, columnWidth - 8,
                                 rect.height() - 16);
                painter.drawRoundedRect(columnRect, 2, 2);
            }
            break;

        case FluentSkeletonType::Chart:
            // Draw chart skeleton with bars
            int barCount = 5;
            int barWidth = (rect.width() - 40) / barCount;
            for (int i = 0; i < barCount; ++i) {
                int barHeight = 20 + (i * 15) % 60;
                QRect barRect(20 + i * barWidth, rect.height() - barHeight - 10,
                              barWidth - 4, barHeight);
                painter.drawRoundedRect(barRect, 2, 2);
            }
            break;
    }
}

void FluentSkeletonWidget::paintShimmer(QPainter& painter) {
    if (!m_config.enableShimmer)
        return;

    QRect rect = this->rect();

    // Create shimmer gradient
    QLinearGradient shimmerGradient;
    shimmerGradient.setStart(rect.left() + m_shimmerPosition * rect.width(),
                             rect.top());
    shimmerGradient.setFinalStop(
        rect.left() + (m_shimmerPosition + 0.3) * rect.width(), rect.bottom());

    shimmerGradient.setColorAt(0.0, QColor(255, 255, 255, 0));
    shimmerGradient.setColorAt(0.5, QColor(255, 255, 255, 80));
    shimmerGradient.setColorAt(1.0, QColor(255, 255, 255, 0));

    painter.setCompositionMode(QPainter::CompositionMode_Overlay);
    painter.setBrush(QBrush(shimmerGradient));
    painter.setPen(Qt::NoPen);

    // Apply shimmer based on skeleton type
    switch (m_config.type) {
        case FluentSkeletonType::Avatar:
            painter.drawEllipse(rect);
            break;
        default:
            painter.drawRoundedRect(rect, m_config.cornerRadius,
                                    m_config.cornerRadius);
            break;
    }

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
}

// FluentLoadingIndicator implementation
FluentLoadingIndicator::FluentLoadingIndicator(FluentLoadingPattern pattern,
                                               QWidget* parent)
    : QWidget(parent), m_pattern(pattern) {
    setFixedSize(m_indicatorSize);
    setupAnimation();

    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_NoSystemBackground, true);
}

void FluentLoadingIndicator::setPattern(FluentLoadingPattern pattern) {
    if (m_pattern == pattern)
        return;

    bool wasAnimating = m_animating;
    if (wasAnimating) {
        stopAnimation();
    }

    m_pattern = pattern;
    setupAnimation();

    if (wasAnimating) {
        startAnimation();
    }

    update();
}

void FluentLoadingIndicator::setSize(const QSize& size) {
    m_indicatorSize = size;
    setFixedSize(size);
    update();
}

void FluentLoadingIndicator::setColors(const QColor& primary,
                                       const QColor& secondary) {
    m_primaryColor = primary;
    if (secondary.isValid()) {
        m_secondaryColor = secondary;
    }
    update();
}

void FluentLoadingIndicator::startAnimation() {
    if (m_animating)
        return;

    m_animating = true;

    switch (m_pattern) {
        case FluentLoadingPattern::Rotate:
            if (m_rotationAnimation) {
                m_rotationAnimation->start();
            }
            break;

        case FluentLoadingPattern::Progress:
            if (m_progressAnimation) {
                m_progressAnimation->start();
            }
            break;

        case FluentLoadingPattern::Dots:
            if (m_dotsAnimation) {
                m_dotsAnimation->start();
            }
            break;

        default:
            break;
    }
}

void FluentLoadingIndicator::stopAnimation() {
    if (!m_animating)
        return;

    m_animating = false;

    if (m_rotationAnimation) {
        m_rotationAnimation->stop();
    }
    if (m_progressAnimation) {
        m_progressAnimation->stop();
    }
    if (m_dotsAnimation) {
        m_dotsAnimation->stop();
    }
}

void FluentLoadingIndicator::setRotationAngle(qreal angle) {
    m_rotationAngle = angle;
    update();
}

void FluentLoadingIndicator::setProgress(qreal progress) {
    m_progress = progress;
    update();
}

void FluentLoadingIndicator::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    switch (m_pattern) {
        case FluentLoadingPattern::Rotate:
            paintSpinner(painter);
            break;
        case FluentLoadingPattern::Dots:
            paintDots(painter);
            break;
        case FluentLoadingPattern::Progress:
            paintProgress(painter);
            break;
        case FluentLoadingPattern::Wave:
            paintWave(painter);
            break;
        default:
            paintSpinner(painter);
            break;
    }
}

void FluentLoadingIndicator::setupAnimation() {
    switch (m_pattern) {
        case FluentLoadingPattern::Rotate:
            m_rotationAnimation =
                std::make_unique<QPropertyAnimation>(this, "rotationAngle");
            m_rotationAnimation->setStartValue(0.0);
            m_rotationAnimation->setEndValue(360.0);
            m_rotationAnimation->setDuration(1000);
            m_rotationAnimation->setEasingCurve(QEasingCurve::Linear);
            m_rotationAnimation->setLoopCount(-1);
            break;

        case FluentLoadingPattern::Progress:
            m_progressAnimation =
                std::make_unique<QPropertyAnimation>(this, "progress");
            m_progressAnimation->setStartValue(0.0);
            m_progressAnimation->setEndValue(1.0);
            m_progressAnimation->setDuration(2000);
            m_progressAnimation->setEasingCurve(QEasingCurve::InOutCubic);
            m_progressAnimation->setLoopCount(-1);
            break;

        default:
            break;
    }
}

void FluentLoadingIndicator::paintSpinner(QPainter& painter) {
    QRect rect = this->rect();
    QPoint center = rect.center();
    int radius = qMin(rect.width(), rect.height()) / 2 - 4;

    painter.translate(center);
    painter.rotate(m_rotationAngle);

    // Draw spinner arc
    QPen pen(m_primaryColor, 3, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    QRect arcRect(-radius, -radius, radius * 2, radius * 2);
    painter.drawArc(arcRect, 0, 270 * 16);  // 270 degree arc
}

void FluentLoadingIndicator::paintDots(QPainter& painter) {
    QRect rect = this->rect();
    QPoint center = rect.center();
    int dotRadius = 3;
    int spacing = 12;

    painter.setBrush(QBrush(m_primaryColor));
    painter.setPen(Qt::NoPen);

    // Draw three dots with animated opacity
    for (int i = 0; i < 3; ++i) {
        QColor dotColor = m_primaryColor;
        qreal phase = (m_rotationAngle + i * 120.0) / 360.0;
        qreal opacity = 0.3 + 0.7 * (0.5 + 0.5 * qSin(phase * 2 * M_PI));
        dotColor.setAlphaF(opacity);

        painter.setBrush(QBrush(dotColor));

        QPoint dotPos(center.x() - spacing + i * spacing, center.y());
        painter.drawEllipse(dotPos, dotRadius, dotRadius);
    }
}

void FluentLoadingIndicator::paintProgress(QPainter& painter) {
    QRect rect = this->rect().adjusted(4, 4, -4, -4);

    // Background
    painter.setBrush(QBrush(m_secondaryColor));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, 2, 2);

    // Progress
    QRect progressRect = rect;
    progressRect.setWidth(rect.width() * m_progress);

    painter.setBrush(QBrush(m_primaryColor));
    painter.drawRoundedRect(progressRect, 2, 2);
}

void FluentLoadingIndicator::paintWave(QPainter& painter) {
    QRect rect = this->rect();

    painter.setPen(QPen(m_primaryColor, 2));
    painter.setBrush(Qt::NoBrush);

    // Draw animated wave
    QPainterPath wavePath;
    wavePath.moveTo(rect.left(), rect.center().y());

    for (int x = rect.left(); x <= rect.right(); x += 2) {
        qreal phase = (x - rect.left()) / 20.0 + m_rotationAngle / 60.0;
        qreal y = rect.center().y() + 10 * qSin(phase);
        wavePath.lineTo(x, y);
    }

    painter.drawPath(wavePath);
}

}  // namespace FluentQt::Animation
