// src/Components/FluentCircularProgress.cpp
#include "FluentQt/Components/FluentCircularProgress.h"
#include <QEasingCurve>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QTimer>
#include <QtMath>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentCircularProgress::FluentCircularProgress(QWidget* parent)
    : QWidget(parent),
      m_progressAnimation(std::make_unique<QPropertyAnimation>(this)),
      m_indeterminateTimer(std::make_unique<QTimer>(this)) {
    initializeComponent();
}

void FluentCircularProgress::initializeComponent() {
    // Set default size
    setFixedSize(24, 24);

    // Initialize colors from theme
    updateColors();

    // Setup progress animation
    m_progressAnimation->setTargetObject(this);
    m_progressAnimation->setPropertyName("displayProgress");
    connect(m_progressAnimation.get(), &QPropertyAnimation::valueChanged, this,
            &FluentCircularProgress::onProgressAnimationValueChanged);

    // Setup indeterminate animation
    m_indeterminateTimer->setInterval(16);  // ~60 FPS
    connect(m_indeterminateTimer.get(), &QTimer::timeout, this,
            &FluentCircularProgress::onIndeterminateTimer);

    // Set default animation duration
    if (m_animationDuration <= 0) {
        m_animationDuration = 200;  // Default Fluent UI animation duration
    }
    m_progressAnimation->setDuration(m_animationDuration);
    m_progressAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Set accessibility properties
    updateAccessibilityInfo();

    // Enable high DPI support
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_NoSystemBackground, true);
}

void FluentCircularProgress::updateColors() {
    auto& theme = Styling::FluentTheme::instance();

    // Use theme colors if not explicitly set
    if (!m_activeColor.isValid()) {
        m_activeColor = theme.color("accent");
    }
    if (!m_inactiveColor.isValid()) {
        m_inactiveColor = theme.color("neutralQuaternary");
    }
}

void FluentCircularProgress::setProgress(qreal progress, bool animated) {
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

void FluentCircularProgress::setActiveColor(const QColor& color) {
    if (m_activeColor != color) {
        m_activeColor = color;
        update();
        emit activeColorChanged(color);
    }
}

void FluentCircularProgress::setInactiveColor(const QColor& color) {
    if (m_inactiveColor != color) {
        m_inactiveColor = color;
        update();
        emit inactiveColorChanged(color);
    }
}

void FluentCircularProgress::setStrokeWidth(int width) {
    width = qMax(1, width);
    if (m_strokeWidth != width) {
        m_strokeWidth = width;
        update();
        emit strokeWidthChanged(width);
    }
}

void FluentCircularProgress::setStyle(ProgressStyle style) {
    if (m_style != style) {
        m_style = style;
        update();
        emit styleChanged(style);
    }
}

void FluentCircularProgress::setAnimated(bool animated) {
    if (m_animated != animated) {
        m_animated = animated;
        emit animatedChanged(animated);
    }
}

void FluentCircularProgress::setAnimationDuration(int duration) {
    duration = qMax(50, duration);
    if (m_animationDuration != duration) {
        m_animationDuration = duration;
        m_progressAnimation->setDuration(duration);
        emit animationDurationChanged(duration);
    }
}

QSize FluentCircularProgress::sizeHint() const {
    if (!m_sizeHintValid) {
        // Calculate size based on stroke width and style
        int baseSize = 24;
        switch (m_style) {
            case ProgressStyle::Ring:
                baseSize = 24;
                break;
            case ProgressStyle::Filled:
                baseSize = 32;
                break;
            case ProgressStyle::Segmented:
                baseSize = 28;
                break;
            case ProgressStyle::Gradient:
                baseSize = 28;
                break;
        }

        // Add stroke width padding
        int totalSize = baseSize + (m_strokeWidth * 2);
        m_cachedSizeHint = QSize(totalSize, totalSize);
        m_sizeHintValid = true;
    }

    return m_cachedSizeHint;
}

QSize FluentCircularProgress::minimumSizeHint() const {
    int minSize = 16 + (m_strokeWidth * 2);
    return QSize(minSize, minSize);
}

void FluentCircularProgress::startIndeterminate() {
    if (!m_indeterminate) {
        m_indeterminate = true;
        m_indeterminateAngle = 0.0;
        m_progressAnimation->stop();
        m_indeterminateTimer->start();
        emit indeterminateStarted();
    }
}

void FluentCircularProgress::stopIndeterminate() {
    if (m_indeterminate) {
        m_indeterminate = false;
        m_indeterminateTimer->stop();
        update();
        emit indeterminateStopped();
    }
}

void FluentCircularProgress::reset() { setProgress(0.0, m_animated); }

void FluentCircularProgress::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Calculate progress rectangle
    QRectF progressRect = calculateProgressRect();

    // Paint based on style
    switch (m_style) {
        case ProgressStyle::Ring:
            paintRingStyle(painter, progressRect);
            break;
        case ProgressStyle::Filled:
            paintFilledStyle(painter, progressRect);
            break;
        case ProgressStyle::Segmented:
            paintSegmentedStyle(painter, progressRect);
            break;
        case ProgressStyle::Gradient:
            paintGradientStyle(painter, progressRect);
            break;
    }
}

void FluentCircularProgress::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_sizeHintValid = false;
}

void FluentCircularProgress::changeEvent(QEvent* event) {
    QWidget::changeEvent(event);

    if (event->type() == QEvent::StyleChange ||
        event->type() == QEvent::PaletteChange) {
        updateColors();
        update();
    }
}

void FluentCircularProgress::onProgressAnimationValueChanged(
    const QVariant& value) {
    m_displayProgress = value.toReal();
    update();
}

void FluentCircularProgress::onIndeterminateTimer() {
    m_indeterminateAngle += 6.0;  // Rotate 6 degrees per frame
    if (m_indeterminateAngle >= 360.0) {
        m_indeterminateAngle = 0.0;
    }
    update();
}

void FluentCircularProgress::updateAccessibilityInfo() {
    setAccessibleName(tr("Progress indicator"));
    setAccessibleDescription(
        tr("Progress: %1%").arg(static_cast<int>(m_progress * 100)));
    setProperty("aria-valuenow", static_cast<int>(m_progress * 100));
    setProperty("aria-valuemin", 0);
    setProperty("aria-valuemax", 100);
    setProperty("role", "progressbar");
}

QRectF FluentCircularProgress::calculateProgressRect() const {
    QRectF rect = this->rect();
    qreal margin = m_strokeWidth / 2.0;
    return rect.adjusted(margin, margin, -margin, -margin);
}

QPen FluentCircularProgress::createProgressPen(const QColor& color) const {
    QPen pen(color);
    pen.setWidth(m_strokeWidth);
    pen.setCapStyle(Qt::RoundCap);
    return pen;
}

void FluentCircularProgress::paintRingStyle(QPainter& painter,
                                            const QRectF& rect) {
    // Paint background ring
    painter.setPen(createProgressPen(m_inactiveColor));
    painter.drawEllipse(rect);

    if (m_indeterminate) {
        // Paint indeterminate arc
        painter.setPen(createProgressPen(m_activeColor));
        painter.drawArc(rect, static_cast<int>(m_indeterminateAngle * 16),
                        90 * 16);
    } else if (m_displayProgress > 0.0) {
        // Paint progress arc
        painter.setPen(createProgressPen(m_activeColor));
        int spanAngle = static_cast<int>(m_displayProgress * 360 * 16);
        painter.drawArc(rect, 90 * 16,
                        -spanAngle);  // Start from top, go clockwise
    }
}

void FluentCircularProgress::paintFilledStyle(QPainter& painter,
                                              const QRectF& rect) {
    // Paint background circle
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_inactiveColor);
    painter.drawEllipse(rect);

    if (m_displayProgress > 0.0 || m_indeterminate) {
        // Create clipping path for progress
        QPainterPath clipPath;
        if (m_indeterminate) {
            qreal angle = m_indeterminateAngle;
            qreal spanAngle = 90.0;
            clipPath.moveTo(rect.center());
            clipPath.arcTo(rect, angle, spanAngle);
            clipPath.closeSubpath();
        } else {
            qreal angle = -90.0;  // Start from top
            qreal spanAngle = m_displayProgress * 360.0;
            clipPath.moveTo(rect.center());
            clipPath.arcTo(rect, angle, spanAngle);
            clipPath.closeSubpath();
        }

        painter.setClipPath(clipPath);
        painter.setBrush(m_activeColor);
        painter.drawEllipse(rect);
    }
}

void FluentCircularProgress::paintSegmentedStyle(QPainter& painter,
                                                 const QRectF& rect) {
    const int segmentCount = 12;
    const qreal segmentAngle = 360.0 / segmentCount;
    const qreal gapAngle = 2.0;  // Gap between segments

    // Paint background segments
    painter.setPen(createProgressPen(m_inactiveColor));
    for (int i = 0; i < segmentCount; ++i) {
        qreal startAngle = (i * segmentAngle) - 90.0;  // Start from top
        painter.drawArc(rect, static_cast<int>(startAngle * 16),
                        static_cast<int>((segmentAngle - gapAngle) * 16));
    }

    // Paint active segments
    if (m_displayProgress > 0.0 || m_indeterminate) {
        painter.setPen(createProgressPen(m_activeColor));

        if (m_indeterminate) {
            // Paint 3 segments for indeterminate
            for (int i = 0; i < 3; ++i) {
                int segmentIndex =
                    (static_cast<int>(m_indeterminateAngle / segmentAngle) +
                     i) %
                    segmentCount;
                qreal startAngle = (segmentIndex * segmentAngle) - 90.0;
                painter.drawArc(
                    rect, static_cast<int>(startAngle * 16),
                    static_cast<int>((segmentAngle - gapAngle) * 16));
            }
        } else {
            // Paint progress segments
            int activeSegments =
                static_cast<int>(m_displayProgress * segmentCount);
            for (int i = 0; i < activeSegments; ++i) {
                qreal startAngle = (i * segmentAngle) - 90.0;
                painter.drawArc(
                    rect, static_cast<int>(startAngle * 16),
                    static_cast<int>((segmentAngle - gapAngle) * 16));
            }
        }
    }
}

void FluentCircularProgress::paintGradientStyle(QPainter& painter,
                                                const QRectF& rect) {
    // Paint background ring
    painter.setPen(createProgressPen(m_inactiveColor));
    painter.drawEllipse(rect);

    if (m_displayProgress > 0.0 || m_indeterminate) {
        // Create gradient
        QConicalGradient gradient = createProgressGradient(rect);
        QPen gradientPen(QBrush(gradient), m_strokeWidth);
        gradientPen.setCapStyle(Qt::RoundCap);

        painter.setPen(gradientPen);

        if (m_indeterminate) {
            painter.drawArc(rect, static_cast<int>(m_indeterminateAngle * 16),
                            90 * 16);
        } else {
            int spanAngle = static_cast<int>(m_displayProgress * 360 * 16);
            painter.drawArc(rect, 90 * 16, -spanAngle);
        }
    }
}

QConicalGradient FluentCircularProgress::createProgressGradient(
    const QRectF& rect) const {
    QConicalGradient gradient(rect.center(), -90.0);

    // Create gradient from active color to lighter version
    QColor lightColor = m_activeColor.lighter(150);
    gradient.setColorAt(0.0, m_activeColor);
    gradient.setColorAt(0.5, lightColor);
    gradient.setColorAt(1.0, m_activeColor);

    return gradient;
}

// Factory methods
FluentCircularProgress* FluentCircularProgress::createRing(int size,
                                                           QWidget* parent) {
    auto* progress = new FluentCircularProgress(parent);
    progress->setFixedSize(size, size);
    progress->setStyle(ProgressStyle::Ring);
    progress->setStrokeWidth(qMax(2, size / 12));
    return progress;
}

FluentCircularProgress* FluentCircularProgress::createFilled(int size,
                                                             QWidget* parent) {
    auto* progress = new FluentCircularProgress(parent);
    progress->setFixedSize(size, size);
    progress->setStyle(ProgressStyle::Filled);
    return progress;
}

FluentCircularProgress* FluentCircularProgress::createSegmented(
    int size, QWidget* parent) {
    auto* progress = new FluentCircularProgress(parent);
    progress->setFixedSize(size, size);
    progress->setStyle(ProgressStyle::Segmented);
    progress->setStrokeWidth(qMax(3, size / 10));
    return progress;
}

FluentCircularProgress* FluentCircularProgress::createAccessible(
    int size, QWidget* parent) {
    auto* progress = new FluentCircularProgress(parent);
    progress->setFixedSize(size, size);
    progress->setStyle(ProgressStyle::Segmented);
    progress->setStrokeWidth(
        qMax(4, size / 8));  // Thicker for better visibility
    return progress;
}

}  // namespace FluentQt::Components
