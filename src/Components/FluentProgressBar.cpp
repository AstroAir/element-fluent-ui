// src/Components/FluentProgressBar.cpp
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QtMath>

namespace FluentQt::Components {

FluentProgressBar::FluentProgressBar(QWidget* parent)
    : FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setObjectName("FluentProgressBar");
    setupAnimations();
    setupTimer();
    setupGlowEffect();
    updateColors();

    // Set default accent color from theme
    const auto& theme = Styling::FluentTheme::instance();
    m_accentColor = theme.color("accent");

    // Enable mouse tracking for reveal effects
    setMouseTracking(true);
}

FluentProgressBar::FluentProgressBar(FluentProgressBarType type,
                                     QWidget* parent)
    : FluentProgressBar(parent) {
    setProgressType(type);
}

FluentProgressBar::~FluentProgressBar() = default;

void FluentProgressBar::setupAnimations() {
    FLUENT_PROFILE("FluentProgressBar::setupAnimations");

    // Value animation for smooth progress changes using Fluent curves
    m_valueAnimation = new QPropertyAnimation(this, "value", this);
    m_valueAnimation->setDuration(300);
    m_valueAnimation->setEasingCurve(m_animator->createFluentBezierCurve(
        Animation::FluentEasing::FluentStandard));

    // Indeterminate animation with Fluent emphasized curve
    m_indeterminateAnimation =
        new QPropertyAnimation(this, "animationProgress", this);
    m_indeterminateAnimation->setDuration(2000);
    m_indeterminateAnimation->setEasingCurve(
        m_animator->createFluentBezierCurve(
            Animation::FluentEasing::FluentEmphasized));
    m_indeterminateAnimation->setLoopCount(-1);  // Infinite loop

    // Glow animation for visual effects
    m_glowAnimation = new QPropertyAnimation(this, "glowIntensity", this);
    m_glowAnimation->setDuration(600);
    m_glowAnimation->setEasingCurve(m_animator->createFluentBezierCurve(
        Animation::FluentEasing::FluentDecelerate));

    // Reveal animation for mouse interactions
    m_revealAnimation = new QPropertyAnimation(this, "revealProgress", this);
    m_revealAnimation->setDuration(400);
    m_revealAnimation->setEasingCurve(m_animator->createFluentBezierCurve(
        Animation::FluentEasing::FluentReveal));

    // Connect animation finished signals
    connect(m_glowAnimation, &QPropertyAnimation::finished, this,
            &FluentProgressBar::onGlowAnimationFinished);
    connect(m_revealAnimation, &QPropertyAnimation::finished, this,
            &FluentProgressBar::onRevealAnimationFinished);

    // Connect theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentProgressBar::updateColors);
}

void FluentProgressBar::setupTimer() {
    m_animationTimer = new QTimer(this);
    m_animationTimer->setInterval(16);  // ~60 FPS
    connect(m_animationTimer, &QTimer::timeout, this,
            &FluentProgressBar::onAnimationStep);
}

void FluentProgressBar::setupGlowEffect() {
    m_glowEffect = new QGraphicsDropShadowEffect(this);
    m_glowEffect->setBlurRadius(12);
    m_glowEffect->setColor(QColor(0, 120, 215, 0));  // Start with transparent
    m_glowEffect->setOffset(0, 0);
    m_glowEffect->setEnabled(false);  // Disabled by default
}

int FluentProgressBar::value() const { return m_value; }

void FluentProgressBar::setValue(int value) {
    const int clampedValue = qBound(m_minimum, value, m_maximum);
    if (m_value != clampedValue) {
        const int oldValue = m_value;
        m_value = clampedValue;

        // Animate value change if enabled
        if (m_animated && isVisible()) {
            m_valueAnimation->stop();
            m_valueAnimation->setStartValue(oldValue);
            m_valueAnimation->setEndValue(clampedValue);
            m_valueAnimation->start();
        }

        updateTextContent();
        update();
        emit valueChanged(clampedValue);

        if (isComplete()) {
            emit finished();
        }
    }
}

int FluentProgressBar::minimum() const { return m_minimum; }

void FluentProgressBar::setMinimum(int minimum) {
    if (m_minimum != minimum) {
        m_minimum = minimum;
        if (m_value < minimum) {
            setValue(minimum);
        }
        emit minimumChanged(minimum);
    }
}

int FluentProgressBar::maximum() const { return m_maximum; }

void FluentProgressBar::setMaximum(int maximum) {
    if (m_maximum != maximum) {
        m_maximum = maximum;
        if (m_value > maximum) {
            setValue(maximum);
        }
        emit maximumChanged(maximum);
    }
}

void FluentProgressBar::setRange(int minimum, int maximum) {
    setMinimum(minimum);
    setMaximum(maximum);
}

QString FluentProgressBar::text() const { return m_text; }

void FluentProgressBar::setText(const QString& text) {
    if (m_text != text) {
        m_text = text;
        updateTextContent();
        update();
        emit textChanged(text);
    }
}

bool FluentProgressBar::isTextVisible() const { return m_textVisible; }

void FluentProgressBar::setTextVisible(bool visible) {
    if (m_textVisible != visible) {
        m_textVisible = visible;
        update();
        emit textVisibleChanged(visible);
    }
}

FluentProgressBarType FluentProgressBar::progressType() const {
    return m_progressType;
}

void FluentProgressBar::setProgressType(FluentProgressBarType type) {
    if (m_progressType != type) {
        m_progressType = type;

        // Stop current animations
        stop();

        // Start appropriate animation for new type
        if (type == FluentProgressBarType::Indeterminate ||
            type == FluentProgressBarType::Ring ||
            type == FluentProgressBarType::Dots) {
            start();
        }

        updateGeometry();
        update();
        emit progressTypeChanged(type);
    }
}

FluentProgressBarSize FluentProgressBar::progressSize() const {
    return m_progressSize;
}

void FluentProgressBar::setProgressSize(FluentProgressBarSize size) {
    if (m_progressSize != size) {
        m_progressSize = size;
        updateGeometry();
        update();
        emit progressSizeChanged(size);
    }
}

FluentProgressBarState FluentProgressBar::progressState() const {
    return m_progressState;
}

void FluentProgressBar::setProgressState(FluentProgressBarState state) {
    if (m_progressState != state) {
        m_progressState = state;
        updateColors();
        update();
        emit progressStateChanged(state);
    }
}

FluentProgressBarVisualStyle FluentProgressBar::visualStyle() const {
    return m_visualStyle;
}

void FluentProgressBar::setVisualStyle(FluentProgressBarVisualStyle style) {
    if (m_visualStyle != style) {
        m_visualStyle = style;
        updateVisualEffects();
        update();
        emit visualStyleChanged(style);
    }
}

bool FluentProgressBar::isAnimated() const { return m_animated; }

void FluentProgressBar::setAnimated(bool animated) {
    if (m_animated != animated) {
        m_animated = animated;
        emit animatedChanged(animated);
    }
}

qreal FluentProgressBar::animationProgress() const {
    return m_animationProgress;
}

void FluentProgressBar::setAnimationProgress(qreal progress) {
    progress = qBound(0.0, progress, 1.0);
    if (!qFuzzyCompare(m_animationProgress, progress)) {
        m_animationProgress = progress;
        update();  // Trigger repaint for animation
        emit animationProgressChanged(progress);
    }
}

QColor FluentProgressBar::accentColor() const { return m_accentColor; }

void FluentProgressBar::setAccentColor(const QColor& color) {
    if (m_accentColor != color) {
        m_accentColor = color;
        updateVisualEffects();
        update();
        emit accentColorChanged(color);
    }
}

bool FluentProgressBar::isGlowEnabled() const { return m_glowEnabled; }

void FluentProgressBar::setGlowEnabled(bool enabled) {
    if (m_glowEnabled != enabled) {
        m_glowEnabled = enabled;
        if (m_glowEffect) {
            m_glowEffect->setEnabled(enabled);
        }
        if (enabled && m_visualStyle == FluentProgressBarVisualStyle::Glow) {
            setGraphicsEffect(m_glowEffect);
        } else if (!enabled) {
            setGraphicsEffect(nullptr);
        }
        emit glowEnabledChanged(enabled);
    }
}

qreal FluentProgressBar::glowIntensity() const { return m_glowIntensity; }

void FluentProgressBar::setGlowIntensity(qreal intensity) {
    intensity = qBound(0.0, intensity, 1.0);
    if (!qFuzzyCompare(m_glowIntensity, intensity)) {
        m_glowIntensity = intensity;
        updateVisualEffects();
        emit glowIntensityChanged(intensity);
    }
}

bool FluentProgressBar::isRevealEnabled() const { return m_revealEnabled; }

void FluentProgressBar::setRevealEnabled(bool enabled) {
    if (m_revealEnabled != enabled) {
        m_revealEnabled = enabled;
        setMouseTracking(enabled);  // Enable mouse tracking for reveal effect
        emit revealEnabledChanged(enabled);
    }
}

qreal FluentProgressBar::revealProgress() const { return m_revealProgress; }

void FluentProgressBar::setRevealProgress(qreal progress) {
    progress = qBound(0.0, progress, 1.0);
    if (!qFuzzyCompare(m_revealProgress, progress)) {
        m_revealProgress = progress;
        update();
        emit revealProgressChanged(progress);
    }
}

qreal FluentProgressBar::percentage() const {
    if (m_maximum == m_minimum) {
        return 0.0;
    }
    return static_cast<qreal>(m_value - m_minimum) / (m_maximum - m_minimum);
}

bool FluentProgressBar::isComplete() const { return m_value >= m_maximum; }

bool FluentProgressBar::isRunning() const { return m_isRunning; }

bool FluentProgressBar::isPaused() const { return m_isPaused; }

bool FluentProgressBar::isIndeterminate() const {
    return m_progressType == FluentProgressBarType::Indeterminate ||
           m_progressType == FluentProgressBarType::Ring ||
           m_progressType == FluentProgressBarType::Dots;
}

QColor FluentProgressBar::getStateColor() const {
    const auto& theme = Styling::FluentTheme::instance();

    switch (m_progressState) {
        case FluentProgressBarState::Success:
            return theme.color("systemFillColorSuccess");
        case FluentProgressBarState::Warning:
            return theme.color("systemFillColorCaution");
        case FluentProgressBarState::Error:
            return theme.color("systemFillColorCritical");
        case FluentProgressBarState::Paused:
            return theme.color("controlFillColorDisabled");
        case FluentProgressBarState::Normal:
        default:
            return getProgressColor();
    }
}

void FluentProgressBar::reset() {
    setValue(m_minimum);
    setProgressState(FluentProgressBarState::Normal);
    stop();
}

QSize FluentProgressBar::sizeHint() const {
    const int height = getBarHeight();
    int width = 200;  // Default width

    if (m_progressType == FluentProgressBarType::Ring) {
        const int ringSize = getRingSize();
        width = ringSize;
        return QSize(ringSize, ringSize);
    }

    // Add space for text if visible
    if (m_textVisible && !m_cachedText.isEmpty()) {
        const QFontMetrics fm(font());
        width = qMax(width, fm.horizontalAdvance(m_cachedText) + 20);
    }

    return QSize(width, height);
}

QSize FluentProgressBar::minimumSizeHint() const {
    const int height = getBarHeight();
    int width = 100;  // Minimum width

    if (m_progressType == FluentProgressBarType::Ring) {
        const int ringSize = getRingSize();
        return QSize(ringSize, ringSize);
    }

    return QSize(width, height);
}

void FluentProgressBar::start() {
    if (!m_isRunning) {
        m_isRunning = true;
        m_isPaused = false;

        if (m_progressType == FluentProgressBarType::Indeterminate ||
            m_progressType == FluentProgressBarType::Ring ||
            m_progressType == FluentProgressBarType::Dots) {
            m_animationTimer->start();
            if (m_animated) {
                m_indeterminateAnimation->start();
            }
        }

        emit started();
    }
}

void FluentProgressBar::stop() {
    if (m_isRunning) {
        m_isRunning = false;
        m_isPaused = false;
        m_animationTimer->stop();
        m_indeterminateAnimation->stop();
        m_animationProgress = 0.0;
        update();
    }
}

void FluentProgressBar::pause() {
    if (m_isRunning && !m_isPaused) {
        m_isPaused = true;
        m_animationTimer->stop();
        m_indeterminateAnimation->pause();
        emit paused();
    }
}

void FluentProgressBar::resume() {
    if (m_isRunning && m_isPaused) {
        m_isPaused = false;
        m_animationTimer->start();
        m_indeterminateAnimation->resume();
        emit resumed();
    }
}

void FluentProgressBar::setProgress(int value, bool animated) {
    if (animated && m_animated) {
        setValue(value);  // This will trigger animation
    } else {
        // Set value directly without animation
        const bool wasAnimated = m_animated;
        m_animated = false;
        setValue(value);
        m_animated = wasAnimated;
    }
}

void FluentProgressBar::incrementProgress(int delta) {
    setValue(m_value + delta);
}

void FluentProgressBar::setComplete() {
    setValue(m_maximum);
    setProgressState(FluentProgressBarState::Success);

    // Trigger glow effect for completion
    if (m_glowEnabled && m_visualStyle == FluentProgressBarVisualStyle::Glow) {
        m_glowAnimation->stop();
        m_glowAnimation->setStartValue(0.0);
        m_glowAnimation->setEndValue(1.0);
        m_glowAnimation->start();
    }
}

void FluentProgressBar::setError(const QString& errorMessage) {
    setProgressState(FluentProgressBarState::Error);
    stop();

    if (!errorMessage.isEmpty()) {
        emit errorOccurred(errorMessage);
    }
}

void FluentProgressBar::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const QRect rect = this->rect();

    // Draw reveal effect first (background layer)
    if (m_revealEnabled && m_revealProgress > 0.0) {
        drawRevealEffect(painter, rect);
    }

    switch (m_progressType) {
        case FluentProgressBarType::Determinate:
            drawDeterminateBar(painter, rect);
            break;
        case FluentProgressBarType::Indeterminate:
            drawIndeterminateBar(painter, rect);
            break;
        case FluentProgressBarType::Ring:
            drawRingProgress(painter, rect);
            break;
        case FluentProgressBarType::Dots:
            drawDotsProgress(painter, rect);
            break;
    }

    // Draw glow effect if enabled
    if (m_glowEnabled && m_visualStyle == FluentProgressBarVisualStyle::Glow &&
        m_glowIntensity > 0.0) {
        drawGlowEffect(painter, rect);
    }

    // Draw text if visible
    if (m_textVisible && !m_cachedText.isEmpty()) {
        drawProgressText(painter, rect);
    }
}

void FluentProgressBar::resizeEvent(QResizeEvent* event) {
    FluentComponent::resizeEvent(event);
    updateGeometry();
}

void FluentProgressBar::changeEvent(QEvent* event) {
    FluentComponent::changeEvent(event);

    if (event->type() == QEvent::FontChange) {
        updateTextContent();
        updateGeometry();
    }
}

void FluentProgressBar::mousePressEvent(QMouseEvent* event) {
    FluentComponent::mousePressEvent(event);

    if (m_revealEnabled) {
        m_revealCenter = event->position();
        m_revealAnimation->stop();
        m_revealAnimation->setStartValue(0.0);
        m_revealAnimation->setEndValue(1.0);
        m_revealAnimation->start();
    }
}

void FluentProgressBar::mouseMoveEvent(QMouseEvent* event) {
    FluentComponent::mouseMoveEvent(event);

    if (m_revealEnabled) {
        m_revealCenter = event->position();
        update();
    }
}

void FluentProgressBar::leaveEvent(QEvent* event) {
    FluentComponent::leaveEvent(event);

    if (m_revealEnabled && m_revealProgress > 0.0) {
        m_revealAnimation->stop();
        m_revealAnimation->setStartValue(m_revealProgress);
        m_revealAnimation->setEndValue(0.0);
        m_revealAnimation->start();
    }
}

void FluentProgressBar::updateAnimation() { update(); }

void FluentProgressBar::onAnimationStep() {
    // Update animation progress for custom animations
    const qreal step = 0.02;  // 2% per step
    m_animationProgress += step * m_animationDirection;

    if (m_animationProgress >= 1.0) {
        m_animationProgress = 1.0;
        m_animationDirection = -1;
    } else if (m_animationProgress <= 0.0) {
        m_animationProgress = 0.0;
        m_animationDirection = 1;
    }

    update();
}

void FluentProgressBar::onGlowAnimationFinished() {
    // Reset glow intensity after animation completes
    if (m_glowIntensity >= 1.0) {
        m_glowAnimation->setStartValue(1.0);
        m_glowAnimation->setEndValue(0.0);
        m_glowAnimation->start();
    }
}

void FluentProgressBar::onRevealAnimationFinished() {
    // Reset reveal progress after animation completes
    setRevealProgress(0.0);
}

void FluentProgressBar::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();
    if (m_accentColor == QColor()) {
        m_accentColor = theme.color("accent");
    }
    updateVisualEffects();
    update();
}

void FluentProgressBar::updateVisualEffects() {
    if (m_glowEffect) {
        QColor glowColor = getStateColor();
        glowColor.setAlphaF(m_glowIntensity * 0.8);
        m_glowEffect->setColor(glowColor);
        m_glowEffect->setBlurRadius(12 + (m_glowIntensity * 8));
    }
}

void FluentProgressBar::updateGeometry() {
    m_cachedProgressRect = progressRect();
    m_cachedTextRect = textRect();
}

void FluentProgressBar::updateTextContent() {
    if (m_text.isEmpty()) {
        // Generate default text showing percentage
        m_cachedText = QString("%1%").arg(qRound(percentage() * 100));
    } else {
        m_cachedText = m_text;
    }
}

// Drawing methods
void FluentProgressBar::drawDeterminateBar(QPainter& painter,
                                           const QRect& rect) {
    Q_UNUSED(rect)
    const auto& theme = Styling::FluentTheme::instance();
    const QRect progressRect = this->progressRect();
    const int radius = theme.borderRadius("small");

    // Draw background
    painter.fillRect(progressRect, getBackgroundColor());

    // Draw progress fill
    if (percentage() > 0.0) {
        const int fillWidth =
            static_cast<int>(progressRect.width() * percentage());
        const QRect fillRect(progressRect.x(), progressRect.y(), fillWidth,
                             progressRect.height());

        QPainterPath fillPath;
        fillPath.addRoundedRect(fillRect, radius, radius);

        // Apply visual style
        if (m_visualStyle == FluentProgressBarVisualStyle::Gradient) {
            drawGradientFill(painter, fillRect, percentage());
        } else {
            painter.fillPath(fillPath, getStateColor());
        }
    }

    // Draw border
    QPainterPath borderPath;
    borderPath.addRoundedRect(progressRect, radius, radius);
    painter.setPen(QPen(theme.color("controlStrokeDefault"), 1));
    painter.drawPath(borderPath);
}

void FluentProgressBar::drawIndeterminateBar(QPainter& painter,
                                             const QRect& rect) {
    Q_UNUSED(rect)
    const auto& theme = Styling::FluentTheme::instance();
    const QRect progressRect = this->progressRect();
    const int radius = theme.borderRadius("small");

    // Draw background
    painter.fillRect(progressRect, getBackgroundColor());

    // Draw moving indicator
    if (m_isRunning && !m_isPaused) {
        const int indicatorWidth = progressRect.width() / 3;
        const int maxX = progressRect.width() - indicatorWidth;
        const int currentX = static_cast<int>(maxX * m_animationProgress);

        const QRect indicatorRect(progressRect.x() + currentX, progressRect.y(),
                                  indicatorWidth, progressRect.height());

        QPainterPath indicatorPath;
        indicatorPath.addRoundedRect(indicatorRect, radius, radius);

        // Apply visual style
        if (m_visualStyle == FluentProgressBarVisualStyle::Gradient) {
            drawGradientFill(painter, indicatorRect, 1.0);
        } else if (m_visualStyle == FluentProgressBarVisualStyle::Pulse) {
            // Pulsing effect
            QColor pulseColor = getStateColor();
            qreal pulseIntensity =
                0.5 + 0.5 * qSin(m_animationProgress * 2 * M_PI);
            pulseColor.setAlphaF(pulseIntensity);
            painter.fillPath(indicatorPath, pulseColor);
        } else {
            painter.fillPath(indicatorPath, getStateColor());
        }
    }

    // Draw border
    QPainterPath borderPath;
    borderPath.addRoundedRect(progressRect, radius, radius);
    painter.setPen(QPen(theme.color("controlStrokeDefault"), 1));
    painter.drawPath(borderPath);
}

void FluentProgressBar::drawRingProgress(QPainter& painter, const QRect& rect) {
    const QPointF center = rect.center();
    const int ringSize = getRingSize();
    const int strokeWidth = ringSize / 10;
    const int radius = (ringSize - strokeWidth) / 2;

    // Draw background ring
    painter.setPen(QPen(getBackgroundColor(), strokeWidth));
    painter.drawEllipse(center, radius, radius);

    // Draw progress arc
    if (m_progressType == FluentProgressBarType::Ring && percentage() > 0.0) {
        const int spanAngle = static_cast<int>(
            360 * 16 * percentage());  // Qt uses 16ths of degrees

        QColor ringColor = getStateColor();

        // Apply visual style
        if (m_visualStyle == FluentProgressBarVisualStyle::Glow) {
            // Add glow effect to ring
            QPen glowPen(ringColor, strokeWidth + 4, Qt::SolidLine,
                         Qt::RoundCap);
            glowPen.setColor(QColor(ringColor.red(), ringColor.green(),
                                    ringColor.blue(), 100));
            painter.setPen(glowPen);
            painter.drawArc(QRect(center.x() - radius, center.y() - radius,
                                  radius * 2, radius * 2),
                            90 * 16, -spanAngle);
        }

        painter.setPen(
            QPen(ringColor, strokeWidth, Qt::SolidLine, Qt::RoundCap));
        painter.drawArc(QRect(center.x() - radius, center.y() - radius,
                              radius * 2, radius * 2),
                        90 * 16, -spanAngle);  // Start from top, go clockwise
    } else if (m_isRunning && !m_isPaused) {
        // Indeterminate ring animation
        const int spanAngle = 90 * 16;  // 90 degrees
        const int startAngle = static_cast<int>(360 * 16 * m_animationProgress);

        QColor ringColor = getStateColor();
        if (m_visualStyle == FluentProgressBarVisualStyle::Pulse) {
            qreal pulseIntensity =
                0.5 + 0.5 * qSin(m_animationProgress * 4 * M_PI);
            ringColor.setAlphaF(pulseIntensity);
        }

        painter.setPen(
            QPen(ringColor, strokeWidth, Qt::SolidLine, Qt::RoundCap));
        painter.drawArc(QRect(center.x() - radius, center.y() - radius,
                              radius * 2, radius * 2),
                        startAngle, spanAngle);
    }
}

void FluentProgressBar::drawDotsProgress(QPainter& painter, const QRect& rect) {
    const int dotCount = 5;
    const int dotSize = getBarHeight() / 3;
    const int spacing = dotSize;
    const int totalWidth = dotCount * dotSize + (dotCount - 1) * spacing;

    const int startX = rect.center().x() - totalWidth / 2;
    const int y = rect.center().y();

    QColor dotColor = getStateColor();
    painter.setBrush(dotColor);
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < dotCount; ++i) {
        const int x = startX + i * (dotSize + spacing);

        // Animate dots with wave effect
        qreal opacity = 0.3;
        qreal scale = 1.0;

        if (m_isRunning && !m_isPaused) {
            const qreal phase = m_animationProgress * 2 * M_PI + i * M_PI / 3;
            opacity = 0.3 + 0.7 * (qSin(phase) + 1.0) / 2.0;

            // Add scaling effect for pulse style
            if (m_visualStyle == FluentProgressBarVisualStyle::Pulse) {
                scale = 0.8 + 0.4 * (qSin(phase) + 1.0) / 2.0;
            }
        }

        painter.setOpacity(opacity);

        // Apply scaling
        const int scaledSize = static_cast<int>(dotSize * scale);
        const int offsetX = (dotSize - scaledSize) / 2;
        const int offsetY = (dotSize - scaledSize) / 2;

        painter.drawEllipse(x + offsetX, y - dotSize / 2 + offsetY, scaledSize,
                            scaledSize);
    }

    painter.setOpacity(1.0);
}

void FluentProgressBar::drawProgressText(QPainter& painter, const QRect& rect) {
    Q_UNUSED(rect)
    const auto& theme = Styling::FluentTheme::instance();

    // Use state-appropriate text color
    QColor textColor = theme.color("textPrimary");
    if (m_progressState == FluentProgressBarState::Error) {
        textColor = theme.color("systemFillColorCritical");
    } else if (m_progressState == FluentProgressBarState::Success) {
        textColor = theme.color("systemFillColorSuccess");
    }

    painter.setPen(textColor);
    painter.setFont(font());

    const QRect textRect = this->textRect();
    painter.drawText(textRect, Qt::AlignCenter, m_cachedText);
}

void FluentProgressBar::drawGlowEffect(QPainter& painter, const QRect& rect) {
    if (m_glowIntensity <= 0.0)
        return;

    painter.save();

    const QRect progressRect = this->progressRect();
    QColor glowColor = getStateColor();
    glowColor.setAlphaF(m_glowIntensity * 0.3);

    // Create glow gradient
    QRadialGradient gradient(
        progressRect.center(),
        qMax(progressRect.width(), progressRect.height()) / 2.0);
    gradient.setColorAt(0.0, glowColor);
    gradient.setColorAt(1.0, Qt::transparent);

    painter.fillRect(rect, gradient);
    painter.restore();
}

void FluentProgressBar::drawRevealEffect(QPainter& painter, const QRect& rect) {
    if (m_revealProgress <= 0.0)
        return;

    painter.save();

    QColor revealColor = getStateColor();
    revealColor.setAlphaF(0.1 * m_revealProgress);

    const qreal maxRadius =
        qSqrt(rect.width() * rect.width() + rect.height() * rect.height());
    const qreal currentRadius = maxRadius * m_revealProgress;

    QRadialGradient gradient(m_revealCenter, currentRadius);
    gradient.setColorAt(0.0, revealColor);
    gradient.setColorAt(1.0, Qt::transparent);

    painter.fillRect(rect, gradient);
    painter.restore();
}

void FluentProgressBar::drawGradientFill(QPainter& painter, const QRect& rect,
                                         qreal progress) {
    Q_UNUSED(progress)

    QColor baseColor = getStateColor();
    QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());

    // Create a subtle gradient effect
    gradient.setColorAt(0.0, baseColor.lighter(110));
    gradient.setColorAt(0.5, baseColor);
    gradient.setColorAt(1.0, baseColor.darker(110));

    painter.fillRect(rect, gradient);
}

// Helper methods
QRect FluentProgressBar::progressRect() const {
    const int height = getBarHeight();
    const int y = (this->height() - height) / 2;

    if (m_progressType == FluentProgressBarType::Ring) {
        const int size = getRingSize();
        const int x = (width() - size) / 2;
        return QRect(x, (this->height() - size) / 2, size, size);
    }

    return QRect(0, y, width(), height);
}

QRect FluentProgressBar::textRect() const {
    if (m_progressType == FluentProgressBarType::Ring) {
        const QRect ringRect = progressRect();
        return QRect(ringRect.x(), ringRect.bottom() + 8, ringRect.width(), 20);
    }

    return QRect(0, 0, width(), height());
}

int FluentProgressBar::getBarHeight() const {
    switch (m_progressSize) {
        case FluentProgressBarSize::Small:
            return 4;
        case FluentProgressBarSize::Medium:
            return 8;
        case FluentProgressBarSize::Large:
            return 12;
    }
    return 8;
}

int FluentProgressBar::getRingSize() const {
    switch (m_progressSize) {
        case FluentProgressBarSize::Small:
            return 24;
        case FluentProgressBarSize::Medium:
            return 32;
        case FluentProgressBarSize::Large:
            return 48;
    }
    return 32;
}

QColor FluentProgressBar::getProgressColor() const {
    return m_accentColor.isValid()
               ? m_accentColor
               : Styling::FluentTheme::instance().color("accent");
}

QColor FluentProgressBar::getBackgroundColor() const {
    const auto& theme = Styling::FluentTheme::instance();
    return theme.color("controlFillSecondary");
}

}  // namespace FluentQt::Components
