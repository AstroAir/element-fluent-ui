// src/Components/FluentLoadingIndicator.cpp
#include "FluentQt/Components/FluentLoadingIndicator.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QFontMetrics>
#include <QHideEvent>
#include <QPainter>
#include <QPainterPath>
#include <QShowEvent>
#include <QtMath>

namespace FluentQt::Components {

FluentLoadingIndicator::FluentLoadingIndicator(QWidget* parent)
    : FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setObjectName("FluentLoadingIndicator");
    setupAnimations();
    updateColors();

    // Initialize dot phases for dots animation (ElaWidgetTools-inspired timing)
    m_dotPhases = {0.0, 0.15, 0.3, 0.45, 0.6};

    // Initialize bar heights for bars animation with smoother progression
    m_barHeights = {0.2, 0.5, 0.8, 1.0, 0.8, 0.5, 0.2};

    // Apply ElaWidgetTools-style optimizations
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_NoSystemBackground, true);

    // Enable smooth animations
    setProperty("smoothAnimations", true);

    // Initialize accessibility
    updateAccessibility();

    // Set default accessible properties
    setAccessibleName("Loading indicator");
    setAccessibleDescription("Shows loading progress");
}

FluentLoadingIndicator::FluentLoadingIndicator(FluentLoadingType type,
                                               QWidget* parent)
    : FluentLoadingIndicator(parent) {
    setLoadingType(type);
}

FluentLoadingIndicator::FluentLoadingIndicator(FluentLoadingType type,
                                               FluentLoadingSize size,
                                               QWidget* parent)
    : FluentLoadingIndicator(type, parent) {
    setLoadingSize(size);
}

FluentLoadingIndicator::~FluentLoadingIndicator() = default;

void FluentLoadingIndicator::setupAnimations() {
    FLUENT_PROFILE("FluentLoadingIndicator::setupAnimations");

    // Main animation timer with FluentUI-optimized timing
    m_animationTimer = new QTimer(this);

    // Adjust frame rate based on motion preference
    int frameInterval = isReducedMotionEnabled() ? 33 : 16;  // 30fps vs 60fps
    m_animationTimer->setInterval(frameInterval);
    m_animationTimer->setTimerType(Qt::PreciseTimer);
    connect(m_animationTimer, &QTimer::timeout, this,
            &FluentLoadingIndicator::onAnimationStep);

    // Rotation animation with FluentUI-compliant duration
    m_rotationAnimation = new QPropertyAnimation(this, "rotationAngle", this);
    m_rotationAnimation->setDuration(getAnimationDuration());
    m_rotationAnimation->setStartValue(0.0);
    m_rotationAnimation->setEndValue(360.0);
    m_rotationAnimation->setLoopCount(-1);  // Infinite

    // Use FluentUI-compliant easing
    if (isReducedMotionEnabled()) {
        m_rotationAnimation->setEasingCurve(QEasingCurve::Linear);
    } else {
        m_rotationAnimation->setEasingCurve(QEasingCurve::OutCubic);
    }

    // Pulse animation with FluentUI timing
    m_pulseAnimation = new QPropertyAnimation(this, "animationProgress", this);
    m_pulseAnimation->setDuration(getAnimationDuration());
    m_pulseAnimation->setStartValue(0.0);
    m_pulseAnimation->setEndValue(1.0);
    m_pulseAnimation->setLoopCount(-1);  // Infinite

    // Use FluentUI-compliant easing for organic motion
    if (isReducedMotionEnabled()) {
        m_pulseAnimation->setEasingCurve(QEasingCurve::Linear);
    } else {
        m_pulseAnimation->setEasingCurve(QEasingCurve::InOutSine);
    }

    // Connect theme changes with smooth transitions
    connect(
        &Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
        this, [this]() {
            // Smooth theme transition
            QPropertyAnimation* colorTransition =
                new QPropertyAnimation(this, "color", this);
            colorTransition->setDuration(200);  // FluentUI transition duration
            colorTransition->setStartValue(m_color);

            // Update to new theme color
            updateColors();

            colorTransition->setEndValue(m_color);
            colorTransition->setEasingCurve(QEasingCurve::OutCubic);
            colorTransition->start(QAbstractAnimation::DeleteWhenStopped);
        });
}

FluentLoadingType FluentLoadingIndicator::loadingType() const {
    return m_loadingType;
}

void FluentLoadingIndicator::setLoadingType(FluentLoadingType type) {
    if (m_loadingType != type) {
        const bool wasRunning = m_running;
        if (wasRunning) {
            stop();
        }

        m_loadingType = type;
        updateGeometry();

        if (wasRunning) {
            start();
        }

        emit loadingTypeChanged(type);
    }
}

FluentLoadingSize FluentLoadingIndicator::loadingSize() const {
    return m_loadingSize;
}

void FluentLoadingIndicator::setLoadingSize(FluentLoadingSize size) {
    if (m_loadingSize != size) {
        m_loadingSize = size;
        updateGeometry();
        update();
        emit loadingSizeChanged(size);
    }
}

FluentLoadingComplexity FluentLoadingIndicator::complexity() const {
    return m_complexity;
}

void FluentLoadingIndicator::setComplexity(FluentLoadingComplexity complexity) {
    if (m_complexity != complexity) {
        const bool wasRunning = m_running;
        if (wasRunning) {
            stop();
        }

        m_complexity = complexity;

        // Adjust default text visibility based on complexity mode
        if (complexity == FluentLoadingComplexity::Simple) {
            setTextVisible(false);  // Simple mode defaults to no text
        }

        updateGeometry();

        if (wasRunning) {
            start();
        }

        emit complexityChanged(complexity);
    }
}

FluentLoadingMode FluentLoadingIndicator::loadingMode() const {
    return m_loadingMode;
}

void FluentLoadingIndicator::setLoadingMode(FluentLoadingMode mode) {
    if (m_loadingMode != mode) {
        m_loadingMode = mode;

        // Reset progress value when switching modes
        if (mode == FluentLoadingMode::Indeterminate) {
            m_progressValue = 0.0;
        }

        updateGeometry();
        update();
        updateAccessibility();
        emit loadingModeChanged(mode);
    }
}

FluentMotionPreference FluentLoadingIndicator::motionPreference() const {
    return m_motionPreference;
}

void FluentLoadingIndicator::setMotionPreference(
    FluentMotionPreference preference) {
    if (m_motionPreference != preference) {
        const bool wasRunning = m_running;
        if (wasRunning) {
            stop();
        }

        m_motionPreference = preference;

        // Update animation settings based on preference
        setupAnimations();

        if (wasRunning) {
            start();
        }

        emit motionPreferenceChanged(preference);
    }
}

QColor FluentLoadingIndicator::color() const { return m_color; }

void FluentLoadingIndicator::setColor(const QColor& color) {
    if (m_color != color) {
        m_color = color;
        update();
        emit colorChanged(color);
    }
}

bool FluentLoadingIndicator::isRunning() const { return m_running; }

void FluentLoadingIndicator::setRunning(bool running) {
    if (m_running != running) {
        m_running = running;

        if (running) {
            start();
        } else {
            stop();
        }

        emit runningChanged(running);
    }
}

int FluentLoadingIndicator::speed() const { return m_speed; }

void FluentLoadingIndicator::setSpeed(int speed) {
    const int clampedSpeed = qBound(1, speed, 10);
    if (m_speed != clampedSpeed) {
        m_speed = clampedSpeed;

        // Update animation durations with FluentUI-compliant timing
        const int duration = getAnimationDuration();
        m_rotationAnimation->setDuration(duration);
        m_pulseAnimation->setDuration(duration);

        // Adjust frame rate based on motion preference and speed
        int baseInterval =
            isReducedMotionEnabled() ? 33 : 16;  // 30fps vs 60fps
        int adjustedInterval =
            baseInterval * (11 - m_speed) / 10;  // Slower for lower speeds
        m_animationTimer->setInterval(adjustedInterval);

        emit speedChanged(clampedSpeed);
    }
}

QString FluentLoadingIndicator::text() const { return m_text; }

void FluentLoadingIndicator::setText(const QString& text) {
    if (m_text != text) {
        m_text = text;
        updateGeometry();
        update();
        emit textChanged(text);
    }
}

bool FluentLoadingIndicator::isTextVisible() const { return m_textVisible; }

void FluentLoadingIndicator::setTextVisible(bool visible) {
    if (m_textVisible != visible) {
        m_textVisible = visible;
        updateGeometry();
        update();
        emit textVisibleChanged(visible);
    }
}

qreal FluentLoadingIndicator::animationProgress() const {
    return m_animationProgress;
}

void FluentLoadingIndicator::setAnimationProgress(qreal progress) {
    progress = qBound(0.0, progress, 1.0);
    if (!qFuzzyCompare(m_animationProgress, progress)) {
        m_animationProgress = progress;
        update();  // Trigger repaint for animation
        emit animationProgressChanged(progress);
    }
}

qreal FluentLoadingIndicator::progressValue() const { return m_progressValue; }

void FluentLoadingIndicator::setProgressValue(qreal value) {
    value = qBound(0.0, value, 1.0);
    if (!qFuzzyCompare(m_progressValue, value)) {
        m_progressValue = value;

        // Auto-switch to determinate mode if not already
        if (m_loadingMode == FluentLoadingMode::Indeterminate) {
            setLoadingMode(FluentLoadingMode::Determinate);
        }

        update();
        updateAccessibility();

        // Announce progress to screen readers
        if (m_loadingMode == FluentLoadingMode::Determinate) {
            const int percentage = static_cast<int>(value * 100);
            announceToScreenReader(QString("Loading %1%").arg(percentage));
        }

        emit progressValueChanged(value);
    }
}

QString FluentLoadingIndicator::accessibleName() const {
    return m_accessibleName;
}

void FluentLoadingIndicator::setAccessibleName(const QString& name) {
    if (m_accessibleName != name) {
        m_accessibleName = name;
        updateAccessibility();
        emit accessibleNameChanged(name);
    }
}

QString FluentLoadingIndicator::accessibleDescription() const {
    return m_accessibleDescription;
}

void FluentLoadingIndicator::setAccessibleDescription(
    const QString& description) {
    if (m_accessibleDescription != description) {
        m_accessibleDescription = description;
        updateAccessibility();
        emit accessibleDescriptionChanged(description);
    }
}

QSize FluentLoadingIndicator::sizeHint() const {
    const int indicatorSize = getIndicatorSize();
    int width = indicatorSize;
    int height = indicatorSize;

    // Add space for text if visible
    if (m_textVisible && !m_text.isEmpty()) {
        const QFontMetrics fm(font());
        const int textWidth = fm.horizontalAdvance(m_text);
        const int textHeight = fm.height();

        width = qMax(width, textWidth);
        height += textHeight + 8;  // 8px spacing
    }

    return QSize(width, height);
}

QSize FluentLoadingIndicator::minimumSizeHint() const {
    const int indicatorSize = getIndicatorSize();
    return QSize(indicatorSize, indicatorSize);
}

void FluentLoadingIndicator::start() {
    if (!m_running) {
        m_running = true;

        // Start loading timeout if error boundary is set (Full mode only)
        if (m_errorBoundary && m_complexity == FluentLoadingComplexity::Full) {
            m_errorBoundary->clearError();
            m_errorBoundary->setLoadingTimeout(m_loadingTimeoutMs);
            m_errorBoundary->startLoadingTimeout();
        }

        switch (m_loadingType) {
            case FluentLoadingType::Spinner:
            case FluentLoadingType::Ring:
                if (m_complexity == FluentLoadingComplexity::Full) {
                    m_rotationAnimation->start();
                } else {
                    // Simple mode uses basic timer animation
                    m_animationTimer->start();
                }
                break;
            case FluentLoadingType::Pulse:
                if (m_complexity == FluentLoadingComplexity::Full) {
                    m_pulseAnimation->start();
                } else {
                    // Simple mode uses basic timer animation
                    m_animationTimer->start();
                }
                break;
            case FluentLoadingType::Dots:
            case FluentLoadingType::Bars:
            case FluentLoadingType::Wave:
            case FluentLoadingType::Skeleton:
                m_animationTimer->start();
                break;
        }

        emit started();
    }
}

void FluentLoadingIndicator::stop() {
    if (m_running) {
        m_running = false;

        // Stop loading timeout if error boundary is set (Full mode only)
        if (m_errorBoundary && m_complexity == FluentLoadingComplexity::Full) {
            m_errorBoundary->stopLoadingTimeout();
        }

        // Stop all animations
        if (m_complexity == FluentLoadingComplexity::Full) {
            m_rotationAnimation->stop();
            m_pulseAnimation->stop();
        }
        m_animationTimer->stop();

        // Reset animation state
        m_animationProgress = 0.0;
        m_rotationAngle = 0.0;

        update();
        emit stopped();
    }
}

void FluentLoadingIndicator::paintEvent(QPaintEvent* event) {
    // Performance optimization: only paint the dirty region
    QPainter painter(this);

    // Enable optimized rendering hints
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Clip to the update region for better performance
    if (event && !event->region().isEmpty()) {
        painter.setClipRegion(event->region());
    }

    const QRect indicatorRect = this->indicatorRect();

    // Early exit if indicator is not visible in update region
    if (event && !event->region().intersects(indicatorRect)) {
        return;
    }

    // Draw loading indicator with optimized rendering
    switch (m_loadingType) {
        case FluentLoadingType::Spinner:
            drawSpinner(painter, indicatorRect);
            break;
        case FluentLoadingType::Dots:
            drawDots(painter, indicatorRect);
            break;
        case FluentLoadingType::Pulse:
            drawPulse(painter, indicatorRect);
            break;
        case FluentLoadingType::Bars:
            drawBars(painter, indicatorRect);
            break;
        case FluentLoadingType::Ring:
            drawRing(painter, indicatorRect);
            break;
        case FluentLoadingType::Wave:
            drawWave(painter, indicatorRect);
            break;
        case FluentLoadingType::Skeleton:
            drawSkeleton(painter, indicatorRect);
            break;
    }

    // Draw text if visible and in update region
    const QRect textRectArea = textRect();
    if (m_textVisible && !m_text.isEmpty() &&
        (!event || event->region().intersects(textRectArea))) {
        drawText(painter, textRectArea);
    }

    // Draw progress text for determinate mode
    if (m_loadingMode == FluentLoadingMode::Determinate &&
        (!event || event->region().intersects(textRectArea))) {
        drawProgressText(painter, textRectArea);
    }
}

void FluentLoadingIndicator::resizeEvent(QResizeEvent* event) {
    FluentComponent::resizeEvent(event);
    updateGeometry();
}

void FluentLoadingIndicator::showEvent(QShowEvent* event) {
    FluentComponent::showEvent(event);
    if (m_autoStart) {
        start();
    }
}

void FluentLoadingIndicator::hideEvent(QHideEvent* event) {
    FluentComponent::hideEvent(event);
    stop();
}

void FluentLoadingIndicator::changeEvent(QEvent* event) {
    FluentComponent::changeEvent(event);

    if (event->type() == QEvent::FontChange) {
        updateGeometry();
    }
}

void FluentLoadingIndicator::updateAnimation() { update(); }

void FluentLoadingIndicator::onAnimationStep() {
    // Performance optimization: skip updates if not visible
    if (!isVisible() || !m_running) {
        return;
    }

    // Calculate optimized step size based on motion preference and speed
    qreal stepSize = isReducedMotionEnabled() ? 0.02 : 0.05;
    stepSize *= m_speed / 5.0;  // Normalize speed

    if (m_complexity == FluentLoadingComplexity::Simple) {
        // Simple mode animation with optimized calculations
        m_animationProgress += stepSize;
        if (m_animationProgress >= 1.0) {
            m_animationProgress = 0.0;
        }

        // Update rotation angle for spinner/ring
        m_rotationAngle = m_animationProgress * 360.0;

        // Optimized updates only for active loading types
        switch (m_loadingType) {
            case FluentLoadingType::Dots:
                // Update dot phases with staggered timing
                for (int i = 0; i < m_dotPhases.size(); ++i) {
                    qreal phase = m_animationProgress + (i * 0.2);
                    if (phase >= 1.0)
                        phase -= 1.0;
                    m_dotPhases[i] = phase;
                }
                break;
            case FluentLoadingType::Bars:
                // Update bar heights with wave pattern
                for (int i = 0; i < m_barHeights.size(); ++i) {
                    qreal phase = m_animationProgress + (i * 0.15);
                    if (phase >= 1.0)
                        phase -= 1.0;
                    m_barHeights[i] = 0.3 + 0.7 * qAbs(qSin(phase * M_PI));
                }
                break;
            default:
                // No additional calculations needed for other types
                break;
        }

        emit animationProgressChanged(m_animationProgress);
    } else {
        // Full mode animation with performance optimizations
        m_animationProgress += stepSize;
        if (m_animationProgress > 1.0) {
            m_animationProgress = 0.0;
        }

        // Optimized updates only for active loading types
        switch (m_loadingType) {
            case FluentLoadingType::Dots:
                for (int i = 0; i < m_dotPhases.size(); ++i) {
                    m_dotPhases[i] = fmod(m_animationProgress + i * 0.2, 1.0);
                }
                break;
            case FluentLoadingType::Bars:
                for (int i = 0; i < m_barHeights.size(); ++i) {
                    const qreal phase =
                        fmod(m_animationProgress + i * 0.15, 1.0);
                    m_barHeights[i] = 0.3 + 0.7 * qAbs(qSin(phase * M_PI));
                }
                break;
            default:
                break;
        }
    }

    // Optimized repaint: only update if values actually changed
    update();
}

void FluentLoadingIndicator::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();
    if (!m_color.isValid()) {
        // Use FluentUI-compliant accent color with proper contrast
        m_color = theme.color("accent");

        // Ensure sufficient contrast for accessibility
        if (theme.mode() == Styling::FluentThemeMode::Dark) {
            m_color = m_color.lighter(120);
        }
    }
    update();
}

void FluentLoadingIndicator::updateGeometry() {
    // This method updates cached geometry values
    // For now, geometry is calculated on-demand in helper methods
}

// Drawing methods
void FluentLoadingIndicator::drawSpinner(QPainter& painter, const QRect& rect) {
    const QPointF center = rect.center();
    const int radius = rect.width() / 2 - 2;
    const int strokeWidth =
        qMax(2, radius / 6);  // Slightly thicker for better visibility

    // Enhanced FluentUI styling with gradient effect
    QColor baseColor = getIndicatorColor();

    if (m_loadingMode == FluentLoadingMode::Determinate) {
        // Determinate mode: draw progress arc
        painter.setPen(QPen(baseColor.lighter(130), strokeWidth, Qt::SolidLine,
                            Qt::RoundCap));

        // Background circle
        painter.drawEllipse(center, radius, radius);

        // Progress arc
        painter.setPen(
            QPen(baseColor, strokeWidth, Qt::SolidLine, Qt::RoundCap));
        const int spanAngle = static_cast<int>(
            m_progressValue * 360 * 16);  // Full circle based on progress
        const int startAngle = -90 * 16;  // Start from top

        painter.drawArc(QRect(center.x() - radius, center.y() - radius,
                              radius * 2, radius * 2),
                        startAngle, spanAngle);
    } else {
        // Indeterminate mode: rotating partial arc
        painter.setPen(
            QPen(baseColor, strokeWidth, Qt::SolidLine, Qt::RoundCap));

        const int spanAngle = 270 * 16;  // 270 degrees in 16ths
        const int startAngle = static_cast<int>(m_rotationAngle * 16);

        painter.drawArc(QRect(center.x() - radius, center.y() - radius,
                              radius * 2, radius * 2),
                        startAngle, spanAngle);
    }
}

void FluentLoadingIndicator::drawDots(QPainter& painter, const QRect& rect) {
    const int dotCount = 5;
    const int dotSize = rect.width() / (dotCount * 2);
    const int spacing = dotSize;
    const int totalWidth = dotCount * dotSize + (dotCount - 1) * spacing;

    const int startX = rect.center().x() - totalWidth / 2;
    const int y = rect.center().y();

    painter.setBrush(getIndicatorColor());
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < dotCount && i < m_dotPhases.size(); ++i) {
        const int x = startX + i * (dotSize + spacing);

        // Animate dot with bounce effect
        const qreal phase = m_dotPhases[i];
        const qreal scale = 0.5 + 0.5 * qAbs(qSin(phase * 2 * M_PI));
        const int currentSize = static_cast<int>(dotSize * scale);

        painter.drawEllipse(x - currentSize / 2, y - currentSize / 2,
                            currentSize, currentSize);
    }
}

void FluentLoadingIndicator::drawPulse(QPainter& painter, const QRect& rect) {
    const QPointF center = rect.center();
    const int maxRadius = rect.width() / 2 - 2;

    // Draw multiple concentric circles with different opacities
    const int circleCount = 3;
    for (int i = 0; i < circleCount; ++i) {
        const qreal phase = fmod(m_animationProgress + i * 0.3, 1.0);
        const int radius = static_cast<int>(maxRadius * phase);
        const qreal opacity = 1.0 - phase;

        QColor color = getIndicatorColor();
        color.setAlphaF(opacity * 0.6);

        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(center, radius, radius);
    }
}

void FluentLoadingIndicator::drawBars(QPainter& painter, const QRect& rect) {
    const int barCount = 5;
    const int barWidth = rect.width() / (barCount * 2);
    const int spacing = barWidth;
    const int maxHeight = rect.height() - 4;

    const int startX = rect.center().x() -
                       (barCount * barWidth + (barCount - 1) * spacing) / 2;
    const int baseY = rect.bottom() - 2;

    painter.setBrush(getIndicatorColor());
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < barCount && i < m_barHeights.size(); ++i) {
        const int x = startX + i * (barWidth + spacing);
        const int height = static_cast<int>(maxHeight * m_barHeights[i]);
        const int y = baseY - height;

        painter.drawRect(x, y, barWidth, height);
    }
}

void FluentLoadingIndicator::drawRing(QPainter& painter, const QRect& rect) {
    const QPointF center = rect.center();
    const int outerRadius = rect.width() / 2 - 2;
    const int strokeWidth =
        qMax(3, outerRadius / 5);  // Thicker for FluentUI style

    QColor baseColor = getIndicatorColor();

    if (m_loadingMode == FluentLoadingMode::Determinate) {
        // Determinate mode: progress ring
        // Draw background ring
        QColor bgColor = baseColor;
        bgColor.setAlphaF(0.15);
        painter.setPen(QPen(bgColor, strokeWidth));
        painter.drawEllipse(center, outerRadius, outerRadius);

        // Draw progress ring
        painter.setPen(
            QPen(baseColor, strokeWidth, Qt::SolidLine, Qt::RoundCap));
        const int spanAngle = static_cast<int>(m_progressValue * 360 * 16);
        const int startAngle = -90 * 16;  // Start from top

        painter.drawArc(
            QRect(center.x() - outerRadius, center.y() - outerRadius,
                  outerRadius * 2, outerRadius * 2),
            startAngle, spanAngle);
    } else {
        // Indeterminate mode: rotating segment
        // Draw background ring with subtle appearance
        QColor bgColor = baseColor;
        bgColor.setAlphaF(0.12);
        painter.setPen(QPen(bgColor, strokeWidth));
        painter.drawEllipse(center, outerRadius, outerRadius);

        // Draw rotating segment with gradient effect
        painter.setPen(
            QPen(baseColor, strokeWidth, Qt::SolidLine, Qt::RoundCap));
        const int spanAngle =
            120 * 16;  // Slightly larger arc for better visibility
        const int startAngle = static_cast<int>(m_rotationAngle * 16);

        painter.drawArc(
            QRect(center.x() - outerRadius, center.y() - outerRadius,
                  outerRadius * 2, outerRadius * 2),
            startAngle, spanAngle);
    }
}

void FluentLoadingIndicator::drawWave(QPainter& painter, const QRect& rect) {
    const int waveCount = 20;
    const qreal waveWidth = static_cast<qreal>(rect.width()) / waveCount;
    const int amplitude = rect.height() / 4;
    const int centerY = rect.center().y();

    painter.setPen(QPen(getIndicatorColor(), 2, Qt::SolidLine, Qt::RoundCap));

    QPainterPath wavePath;
    bool firstPoint = true;

    for (int i = 0; i <= waveCount; ++i) {
        const qreal x = rect.x() + i * waveWidth;
        const qreal phase = m_animationProgress * 2 * M_PI + i * 0.3;
        const qreal y = centerY + amplitude * qSin(phase);

        if (firstPoint) {
            wavePath.moveTo(x, y);
            firstPoint = false;
        } else {
            wavePath.lineTo(x, y);
        }
    }

    painter.drawPath(wavePath);
}

void FluentLoadingIndicator::drawSkeleton(QPainter& painter,
                                          const QRect& rect) {
    // FluentUI 2.0 skeleton/shimmer loading pattern
    const QColor baseColor = getIndicatorColor();
    const QColor shimmerColor = baseColor.lighter(150);

    // Create gradient for shimmer effect
    QLinearGradient gradient(rect.topLeft(), rect.topRight());
    const qreal shimmerPos = m_animationProgress;

    gradient.setColorAt(0.0, baseColor);
    gradient.setColorAt(qMax(0.0, shimmerPos - 0.3), baseColor);
    gradient.setColorAt(shimmerPos, shimmerColor);
    gradient.setColorAt(qMin(1.0, shimmerPos + 0.3), baseColor);
    gradient.setColorAt(1.0, baseColor);

    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);

    // Draw skeleton bars with rounded corners
    const int barHeight = rect.height() / 6;
    const int spacing = barHeight / 2;
    const int cornerRadius = barHeight / 4;

    for (int i = 0; i < 3; ++i) {
        const int y = rect.y() + i * (barHeight + spacing);
        const int width =
            rect.width() - (i * rect.width() / 6);  // Varying widths
        const QRect barRect(rect.x(), y, width, barHeight);

        painter.drawRoundedRect(barRect, cornerRadius, cornerRadius);
    }
}

void FluentLoadingIndicator::drawText(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    painter.setPen(theme.color("textSecondary"));
    painter.setFont(font());
    painter.drawText(rect, Qt::AlignCenter, m_text);
}

void FluentLoadingIndicator::drawProgressText(QPainter& painter,
                                              const QRect& rect) {
    if (m_loadingMode != FluentLoadingMode::Determinate) {
        return;
    }

    const auto& theme = Styling::FluentTheme::instance();
    painter.setPen(theme.color("textPrimary"));
    painter.setFont(font());

    const int percentage = static_cast<int>(m_progressValue * 100);
    const QString progressText = QString("%1%").arg(percentage);

    // Draw progress text below the main text
    QRect progressRect = rect;
    if (!m_text.isEmpty() && m_textVisible) {
        progressRect.moveTop(rect.bottom() + 4);
    }

    painter.drawText(progressRect, Qt::AlignCenter, progressText);
}

// Helper methods
QRect FluentLoadingIndicator::indicatorRect() const {
    const int size = getIndicatorSize();
    const int x = (width() - size) / 2;
    int y = (height() - size) / 2;

    // Adjust for text if visible
    if (m_textVisible && !m_text.isEmpty()) {
        const QFontMetrics fm(font());
        const int textHeight = fm.height();
        y = (height() - size - textHeight - 8) / 2;
    }

    return QRect(x, y, size, size);
}

QRect FluentLoadingIndicator::textRect() const {
    if (!m_textVisible || m_text.isEmpty()) {
        return QRect();
    }

    const QRect indicatorRect = this->indicatorRect();
    const int y = indicatorRect.bottom() + 8;
    const int height = 20;

    return QRect(0, y, width(), height);
}

int FluentLoadingIndicator::getIndicatorSize() const {
    switch (m_loadingSize) {
        case FluentLoadingSize::Small:
            return 16;
        case FluentLoadingSize::Medium:
            return 24;
        case FluentLoadingSize::Large:
            return 32;
        case FluentLoadingSize::ExtraLarge:
            return 48;
    }
    return 24;
}

QColor FluentLoadingIndicator::getIndicatorColor() const {
    if (m_color.isValid()) {
        return m_color;
    }
    return Styling::FluentTheme::instance().color("accent");
}

int FluentLoadingIndicator::getAnimationDuration() const {
    // FluentUI-compliant durations based on motion hierarchy
    int baseDuration;

    if (isReducedMotionEnabled()) {
        // Reduced motion: use minimal animation
        baseDuration = 100;
    } else {
        // Follow FluentUI motion hierarchy
        switch (m_loadingType) {
            case FluentLoadingType::Spinner:
            case FluentLoadingType::Ring:
                baseDuration = 1000;  // Primary motion for main indicators
                break;
            case FluentLoadingType::Dots:
            case FluentLoadingType::Bars:
                baseDuration = 800;  // Secondary motion for multi-element
                break;
            case FluentLoadingType::Pulse:
            case FluentLoadingType::Wave:
                baseDuration = 1200;  // Slower for organic motion
                break;
            case FluentLoadingType::Skeleton:
                baseDuration = 1500;  // Slower shimmer effect
                break;
        }
    }

    // Adjust by speed setting (1-10 scale)
    return baseDuration / m_speed;
}

bool FluentLoadingIndicator::isReducedMotionEnabled() const {
    switch (m_motionPreference) {
        case FluentMotionPreference::Reduced:
            return true;
        case FluentMotionPreference::Full:
            return false;
        case FluentMotionPreference::Auto:
        default:
            // Check theme setting for reduced motion
            // For now, return false - this can be enhanced when theme supports
            // it
            return false;
    }
}

void FluentLoadingIndicator::updateAccessibility() {
    // Set accessible properties
    QString accessibleText = m_accessibleName;
    if (accessibleText.isEmpty()) {
        accessibleText = "Loading indicator";
    }

    if (!m_accessibleDescription.isEmpty()) {
        accessibleText += ". " + m_accessibleDescription;
    }

    // Add progress information for determinate mode
    if (m_loadingMode == FluentLoadingMode::Determinate) {
        const int percentage = static_cast<int>(m_progressValue * 100);
        accessibleText += QString(". Progress: %1%").arg(percentage);
    }

    // Add loading state
    if (m_running) {
        accessibleText += ". Loading";
    } else {
        accessibleText += ". Stopped";
    }

    // Set accessible name using Qt's accessibility system
    setAccessibleName(accessibleText);
    setAccessibleDescription(accessibleText);
}

void FluentLoadingIndicator::announceToScreenReader(const QString& message) {
    // Simple implementation for screen reader announcements
    if (!message.isEmpty()) {
        // Store the message for accessibility queries
        setProperty("accessibleStatusText", message);

        // Update accessible name to trigger screen reader notification
        QString currentName = accessibleName();
        setAccessibleName(message);
        // Restore original name after brief announcement
        QTimer::singleShot(100, this, [this, currentName]() {
            setAccessibleName(currentName);
        });
    }
}

// Error boundary integration methods
void FluentLoadingIndicator::setErrorBoundary(
    Core::FluentErrorBoundary* boundary) {
    m_errorBoundary = boundary;

    if (m_errorBoundary) {
        // Enhanced error boundary integration
        // Set up retry callback to restart loading
        m_errorBoundary->setRetryCallback([this]() {
            // Reset progress and restart
            if (m_loadingMode == FluentLoadingMode::Determinate) {
                setProgressValue(0.0);
            }
            start();
        });

        // Connect error signals for better integration
        connect(m_errorBoundary, &Core::FluentErrorBoundary::errorOccurred,
                this,
                [this](const QString& message,
                       Core::FluentErrorBoundary::ErrorType type) {
                    // Stop loading on error
                    stop();

                    // Announce error to screen readers
                    announceToScreenReader(
                        QString("Loading failed: %1").arg(message));

                    // Update accessibility information
                    updateAccessibility();
                });

        connect(m_errorBoundary, &Core::FluentErrorBoundary::errorCleared, this,
                [this]() {
                    // Update accessibility when error is cleared
                    updateAccessibility();
                });
    }
}

void FluentLoadingIndicator::setLoadingTimeout(int timeoutMs) {
    m_loadingTimeoutMs = timeoutMs;

    if (m_errorBoundary) {
        m_errorBoundary->setLoadingTimeout(timeoutMs);
    }
}

// Factory methods (from Simple variant)
FluentLoadingIndicator* FluentLoadingIndicator::createSpinner(
    FluentLoadingSize size, QWidget* parent) {
    auto* indicator =
        new FluentLoadingIndicator(FluentLoadingType::Spinner, size, parent);
    indicator->setComplexity(FluentLoadingComplexity::Simple);
    return indicator;
}

FluentLoadingIndicator* FluentLoadingIndicator::createDots(
    FluentLoadingSize size, QWidget* parent) {
    auto* indicator =
        new FluentLoadingIndicator(FluentLoadingType::Dots, size, parent);
    indicator->setComplexity(FluentLoadingComplexity::Simple);
    return indicator;
}

FluentLoadingIndicator* FluentLoadingIndicator::createPulse(
    FluentLoadingSize size, QWidget* parent) {
    auto* indicator =
        new FluentLoadingIndicator(FluentLoadingType::Pulse, size, parent);
    indicator->setComplexity(FluentLoadingComplexity::Simple);
    return indicator;
}

FluentLoadingIndicator* FluentLoadingIndicator::createSkeleton(
    FluentLoadingSize size, QWidget* parent) {
    auto* indicator =
        new FluentLoadingIndicator(FluentLoadingType::Skeleton, size, parent);
    indicator->setComplexity(FluentLoadingComplexity::Simple);
    indicator->setAccessibleName("Content loading");
    indicator->setAccessibleDescription(
        "Skeleton placeholder while content loads");
    return indicator;
}

FluentLoadingIndicator* FluentLoadingIndicator::createDeterminate(
    FluentLoadingType type, FluentLoadingSize size, QWidget* parent) {
    auto* indicator = new FluentLoadingIndicator(type, size, parent);
    indicator->setLoadingMode(FluentLoadingMode::Determinate);
    indicator->setAccessibleName("Progress indicator");
    indicator->setAccessibleDescription(
        "Shows loading progress with percentage");
    return indicator;
}

FluentLoadingIndicator* FluentLoadingIndicator::createSimple(
    FluentLoadingType type, FluentLoadingSize size, QWidget* parent) {
    auto* indicator = new FluentLoadingIndicator(type, size, parent);
    indicator->setComplexity(FluentLoadingComplexity::Simple);
    return indicator;
}

}  // namespace FluentQt::Components
