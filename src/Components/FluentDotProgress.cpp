// src/Components/FluentDotProgress.cpp
#include "FluentQt/Components/FluentDotProgress.h"
#include <QEasingCurve>
#include <QPaintEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QTimer>
#include <QtMath>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentDotProgress::FluentDotProgress(QWidget* parent)
    : QWidget(parent),
      m_progressAnimation(std::make_unique<QPropertyAnimation>(this)),
      m_indeterminateTimer(std::make_unique<QTimer>(this)) {
    initializeComponent();
}

void FluentDotProgress::initializeComponent() {
    // Set default size
    setFixedHeight(20);
    setMinimumWidth(60);

    // Initialize colors from theme
    updateColors();

    // Setup progress animation
    m_progressAnimation->setTargetObject(this);
    m_progressAnimation->setPropertyName("displayProgress");
    connect(m_progressAnimation.get(), &QPropertyAnimation::valueChanged, this,
            &FluentDotProgress::onProgressAnimationValueChanged);

    // Setup indeterminate animation
    m_indeterminateTimer->setInterval(50);  // 20 FPS for smooth animation
    connect(m_indeterminateTimer.get(), &QTimer::timeout, this,
            &FluentDotProgress::onIndeterminateTimer);

    // Update animation duration from theme
    auto& theme = Styling::FluentTheme::instance();
    if (m_animationDuration <= 0) {
        m_animationDuration = theme.duration("normal");
        if (m_animationDuration <= 0) {
            m_animationDuration = 200;  // Fallback
        }
    }
    m_progressAnimation->setDuration(m_animationDuration);
    m_progressAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Set accessibility properties
    updateAccessibilityInfo();

    // Enable high DPI support
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_NoSystemBackground, true);

    // Mark layout as dirty
    m_layoutDirty = true;
}

void FluentDotProgress::updateColors() {
    auto& theme = Styling::FluentTheme::instance();

    // Use theme colors if not explicitly set
    if (!m_activeColor.isValid()) {
        m_activeColor = theme.color("accent");
    }
    if (!m_inactiveColor.isValid()) {
        m_inactiveColor = theme.color("neutralQuaternary");
    }
}

void FluentDotProgress::setProgress(qreal progress, bool animated) {
    progress = qBound(0.0, progress, 1.0);

    if (qAbs(m_progress - progress) < 0.001) {
        return;  // No significant change
    }

    m_progress = progress;

    if (animated && m_animated && !m_indeterminate) {
        m_progressAnimation->stop();
        m_progressAnimation->setStartValue(m_displayProgress);
        m_progressAnimation->setEndValue(m_progress);
        m_progressAnimation->start();
    } else {
        m_displayProgress = m_progress;
        update();
    }

    updateAccessibilityInfo();
    emit progressChanged(m_progress);
}

void FluentDotProgress::setActiveColor(const QColor& color) {
    if (m_activeColor != color) {
        m_activeColor = color;
        update();
        emit activeColorChanged(color);
    }
}

void FluentDotProgress::setInactiveColor(const QColor& color) {
    if (m_inactiveColor != color) {
        m_inactiveColor = color;
        update();
        emit inactiveColorChanged(color);
    }
}

void FluentDotProgress::setDotCount(int count) {
    count = qBound(3, count, 20);  // Reasonable limits
    if (m_dotCount != count) {
        m_dotCount = count;
        m_layoutDirty = true;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit dotCountChanged(count);
    }
}

void FluentDotProgress::setDotSize(int size) {
    size = qMax(2, size);
    if (m_dotSize != size) {
        m_dotSize = size;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit dotSizeChanged(size);
    }
}

void FluentDotProgress::setDotSpacing(int spacing) {
    spacing = qMax(2, spacing);
    if (m_dotSpacing != spacing) {
        m_dotSpacing = spacing;
        m_layoutDirty = true;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit dotSpacingChanged(spacing);
    }
}

void FluentDotProgress::setStyle(ProgressStyle style) {
    if (m_style != style) {
        m_style = style;
        m_layoutDirty = true;
        m_sizeHintValid = false;
        updateGeometry();
        update();
        emit styleChanged(style);
    }
}

void FluentDotProgress::setAnimated(bool animated) {
    if (m_animated != animated) {
        m_animated = animated;
        emit animatedChanged(animated);
    }
}

void FluentDotProgress::setAnimationDuration(int duration) {
    duration = qMax(50, duration);
    if (m_animationDuration != duration) {
        m_animationDuration = duration;
        m_progressAnimation->setDuration(duration);
        emit animationDurationChanged(duration);
    }
}

QSize FluentDotProgress::sizeHint() const {
    if (!m_sizeHintValid) {
        int width, height;

        switch (m_style) {
            case ProgressStyle::Linear:
                width = (m_dotCount * m_dotSize) +
                        ((m_dotCount - 1) * m_dotSpacing);
                height = m_dotSize + 4;  // Add some padding
                break;

            case ProgressStyle::Circular: {
                int diameter = qMax(40, (m_dotCount * m_dotSize) / 2);
                width = height = diameter + m_dotSize + 4;
            } break;

            case ProgressStyle::Wave:
            case ProgressStyle::Pulse:
                width = (m_dotCount * m_dotSize) +
                        ((m_dotCount - 1) * m_dotSpacing);
                height = m_dotSize * 2 + 4;  // Extra height for wave effect
                break;
        }

        m_cachedSizeHint = QSize(width, height);
        m_sizeHintValid = true;
    }

    return m_cachedSizeHint;
}

QSize FluentDotProgress::minimumSizeHint() const {
    int minWidth = (3 * m_dotSize) + (2 * m_dotSpacing);  // Minimum 3 dots
    int minHeight = m_dotSize + 4;
    return QSize(minWidth, minHeight);
}

void FluentDotProgress::startIndeterminate() {
    if (!m_indeterminate) {
        m_indeterminate = true;
        m_indeterminatePhase = 0.0;
        m_progressAnimation->stop();
        m_indeterminateTimer->start();
        emit indeterminateStarted();
    }
}

void FluentDotProgress::stopIndeterminate() {
    if (m_indeterminate) {
        m_indeterminate = false;
        m_indeterminateTimer->stop();
        update();
        emit indeterminateStopped();
    }
}

void FluentDotProgress::reset() { setProgress(0.0, m_animated); }

void FluentDotProgress::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Calculate dot positions if needed
    if (m_layoutDirty) {
        calculateDotPositions();
        m_layoutDirty = false;
    }

    // Paint based on style
    switch (m_style) {
        case ProgressStyle::Linear:
            paintLinearStyle(painter);
            break;
        case ProgressStyle::Circular:
            paintCircularStyle(painter);
            break;
        case ProgressStyle::Wave:
            paintWaveStyle(painter);
            break;
        case ProgressStyle::Pulse:
            paintPulseStyle(painter);
            break;
    }
}

void FluentDotProgress::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_layoutDirty = true;
    m_sizeHintValid = false;
}

void FluentDotProgress::changeEvent(QEvent* event) {
    QWidget::changeEvent(event);

    if (event->type() == QEvent::StyleChange ||
        event->type() == QEvent::PaletteChange) {
        updateColors();
        update();
    }
}

void FluentDotProgress::onProgressAnimationValueChanged(const QVariant& value) {
    m_displayProgress = value.toReal();
    update();
}

void FluentDotProgress::onIndeterminateTimer() {
    m_indeterminatePhase += 0.1;  // Increment phase
    if (m_indeterminatePhase >= 1.0) {
        m_indeterminatePhase = 0.0;
    }
    update();
}

void FluentDotProgress::updateAccessibilityInfo() {
    setAccessibleName(tr("Dot progress indicator"));
    setAccessibleDescription(
        tr("Progress: %1%").arg(static_cast<int>(m_progress * 100)));
    setProperty("aria-valuenow", static_cast<int>(m_progress * 100));
    setProperty("aria-valuemin", 0);
    setProperty("aria-valuemax", 100);
    setProperty("role", "progressbar");
}

void FluentDotProgress::calculateDotPositions() {
    m_dotPositions.clear();

    QRectF contentRect = rect().adjusted(2, 2, -2, -2);  // Add some margin

    switch (m_style) {
        case ProgressStyle::Linear:
        case ProgressStyle::Wave:
        case ProgressStyle::Pulse: {
            qreal totalWidth =
                (m_dotCount * m_dotSize) + ((m_dotCount - 1) * m_dotSpacing);
            qreal startX = (contentRect.width() - totalWidth) / 2.0;
            qreal y = contentRect.center().y();

            for (int i = 0; i < m_dotCount; ++i) {
                qreal x = startX + (i * (m_dotSize + m_dotSpacing)) +
                          (m_dotSize / 2.0);
                m_dotPositions.append(QPointF(x, y));
            }
        } break;

        case ProgressStyle::Circular: {
            QPointF center = contentRect.center();
            qreal radius =
                qMin(contentRect.width(), contentRect.height()) / 2.0 -
                m_dotSize;

            for (int i = 0; i < m_dotCount; ++i) {
                qreal angle = (2.0 * M_PI * i) / m_dotCount -
                              (M_PI / 2.0);  // Start from top
                qreal x = center.x() + radius * qCos(angle);
                qreal y = center.y() + radius * qSin(angle);
                m_dotPositions.append(QPointF(x, y));
            }
        } break;
    }
}

// Factory methods
FluentDotProgress* FluentDotProgress::createLinear(int dotCount,
                                                   QWidget* parent) {
    auto* progress = new FluentDotProgress(parent);
    progress->setDotCount(dotCount);
    progress->setStyle(ProgressStyle::Linear);
    return progress;
}

FluentDotProgress* FluentDotProgress::createCircular(int dotCount,
                                                     QWidget* parent) {
    auto* progress = new FluentDotProgress(parent);
    progress->setDotCount(dotCount);
    progress->setStyle(ProgressStyle::Circular);
    return progress;
}

FluentDotProgress* FluentDotProgress::createWave(int dotCount,
                                                 QWidget* parent) {
    auto* progress = new FluentDotProgress(parent);
    progress->setDotCount(dotCount);
    progress->setStyle(ProgressStyle::Wave);
    return progress;
}

FluentDotProgress* FluentDotProgress::createAccessible(int dotCount,
                                                       QWidget* parent) {
    auto* progress = new FluentDotProgress(parent);
    progress->setDotCount(dotCount);
    progress->setStyle(ProgressStyle::Linear);
    progress->setDotSize(8);      // Larger dots for better visibility
    progress->setDotSpacing(12);  // More spacing for clarity
    return progress;
}

void FluentDotProgress::paintLinearStyle(QPainter& painter) {
    for (int i = 0; i < m_dotPositions.size(); ++i) {
        qreal dotProgress =
            qBound(0.0, (m_displayProgress * m_dotCount) - i, 1.0);

        if (m_indeterminate) {
            // Create wave effect for indeterminate
            qreal phase = m_indeterminatePhase * m_dotCount;
            qreal distance = qAbs(i - phase);
            qreal opacity = qMax(0.2, 1.0 - (distance / 2.0));
            paintDot(painter, m_dotPositions[i], opacity);
        } else {
            QColor dotColor =
                interpolateColor(m_inactiveColor, m_activeColor, dotProgress);
            painter.setBrush(dotColor);
            painter.setPen(Qt::NoPen);
            paintDot(painter, m_dotPositions[i], 1.0);
        }
    }
}

void FluentDotProgress::paintCircularStyle(QPainter& painter) {
    for (int i = 0; i < m_dotPositions.size(); ++i) {
        if (m_indeterminate) {
            // Rotating highlight effect
            qreal phase = m_indeterminatePhase * m_dotCount;
            int activeIndex = static_cast<int>(phase) % m_dotCount;
            qreal opacity = (i == activeIndex) ? 1.0 : 0.3;
            paintDot(painter, m_dotPositions[i], opacity);
        } else {
            qreal dotProgress =
                qBound(0.0, (m_displayProgress * m_dotCount) - i, 1.0);
            QColor dotColor =
                interpolateColor(m_inactiveColor, m_activeColor, dotProgress);
            painter.setBrush(dotColor);
            painter.setPen(Qt::NoPen);
            paintDot(painter, m_dotPositions[i], 1.0);
        }
    }
}

void FluentDotProgress::paintWaveStyle(QPainter& painter) {
    for (int i = 0; i < m_dotPositions.size(); ++i) {
        QPointF position = m_dotPositions[i];

        if (m_indeterminate) {
            // Sine wave animation
            qreal phase = m_indeterminatePhase * 2.0 * M_PI;
            qreal waveOffset = qSin(phase + (i * 0.5)) * (m_dotSize / 2.0);
            position.setY(position.y() + waveOffset);
            qreal opacity = 0.5 + 0.5 * qSin(phase + (i * 0.3));
            paintDot(painter, position, opacity);
        } else {
            qreal dotProgress =
                qBound(0.0, (m_displayProgress * m_dotCount) - i, 1.0);
            qreal waveOffset = qSin(dotProgress * M_PI) * (m_dotSize / 4.0);
            position.setY(position.y() + waveOffset);
            QColor dotColor =
                interpolateColor(m_inactiveColor, m_activeColor, dotProgress);
            painter.setBrush(dotColor);
            painter.setPen(Qt::NoPen);
            paintDot(painter, position, 1.0);
        }
    }
}

void FluentDotProgress::paintPulseStyle(QPainter& painter) {
    for (int i = 0; i < m_dotPositions.size(); ++i) {
        if (m_indeterminate) {
            // Pulsing effect
            qreal phase = m_indeterminatePhase * 2.0 * M_PI + (i * 0.4);
            qreal scale = 0.7 + 0.3 * qSin(phase);
            qreal opacity = 0.4 + 0.6 * qSin(phase);
            paintDot(painter, m_dotPositions[i], opacity, scale);
        } else {
            qreal dotProgress =
                qBound(0.0, (m_displayProgress * m_dotCount) - i, 1.0);
            qreal scale = 0.7 + 0.3 * dotProgress;
            QColor dotColor =
                interpolateColor(m_inactiveColor, m_activeColor, dotProgress);
            painter.setBrush(dotColor);
            painter.setPen(Qt::NoPen);
            paintDot(painter, m_dotPositions[i], 1.0, scale);
        }
    }
}

void FluentDotProgress::paintDot(QPainter& painter, const QPointF& position,
                                 qreal opacity, qreal scale) {
    painter.save();

    // Apply opacity
    if (opacity < 1.0) {
        QColor color = painter.brush().color();
        if (!color.isValid()) {
            color = m_activeColor;
        }
        color.setAlphaF(opacity);
        painter.setBrush(color);
    }

    // Calculate dot size with scale
    qreal dotRadius = (m_dotSize / 2.0) * scale;
    QRectF dotRect(position.x() - dotRadius, position.y() - dotRadius,
                   dotRadius * 2, dotRadius * 2);

    painter.drawEllipse(dotRect);
    painter.restore();
}

QColor FluentDotProgress::interpolateColor(const QColor& from, const QColor& to,
                                           qreal factor) const {
    factor = qBound(0.0, factor, 1.0);

    int r = static_cast<int>(from.red() + factor * (to.red() - from.red()));
    int g =
        static_cast<int>(from.green() + factor * (to.green() - from.green()));
    int b = static_cast<int>(from.blue() + factor * (to.blue() - from.blue()));
    int a =
        static_cast<int>(from.alpha() + factor * (to.alpha() - from.alpha()));

    return QColor(r, g, b, a);
}

}  // namespace FluentQt::Components
