// src/Components/FluentProgressBar.cpp
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QResizeEvent>
#include <QtMath>

namespace FluentQt::Components {

FluentProgressBar::FluentProgressBar(QWidget* parent)
    : FluentComponent(parent)
    , m_animator(std::make_unique<Animation::FluentAnimator>(this))
{
    setObjectName("FluentProgressBar");
    setupAnimations();
    setupTimer();
    updateColors();
    
    // Set default accent color from theme
    const auto& theme = Styling::FluentTheme::instance();
    m_accentColor = theme.color("accent");
}

FluentProgressBar::FluentProgressBar(FluentProgressBarType type, QWidget* parent)
    : FluentProgressBar(parent)
{
    setProgressType(type);
}

FluentProgressBar::~FluentProgressBar() = default;

void FluentProgressBar::setupAnimations() {
    FLUENT_PROFILE("FluentProgressBar::setupAnimations");
    
    // Value animation for smooth progress changes
    m_valueAnimation = new QPropertyAnimation(this, "value", this);
    m_valueAnimation->setDuration(300);
    m_valueAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // Indeterminate animation
    m_indeterminateAnimation = new QPropertyAnimation(this, "animationProgress", this);
    m_indeterminateAnimation->setDuration(2000);
    m_indeterminateAnimation->setEasingCurve(QEasingCurve::InOutSine);
    m_indeterminateAnimation->setLoopCount(-1); // Infinite loop
    
    // Connect theme changes
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, &FluentProgressBar::updateColors);
}

void FluentProgressBar::setupTimer() {
    m_animationTimer = new QTimer(this);
    m_animationTimer->setInterval(16); // ~60 FPS
    connect(m_animationTimer, &QTimer::timeout, this, &FluentProgressBar::onAnimationStep);
}

int FluentProgressBar::value() const {
    return m_value;
}

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

int FluentProgressBar::minimum() const {
    return m_minimum;
}

void FluentProgressBar::setMinimum(int minimum) {
    if (m_minimum != minimum) {
        m_minimum = minimum;
        if (m_value < minimum) {
            setValue(minimum);
        }
        emit minimumChanged(minimum);
    }
}

int FluentProgressBar::maximum() const {
    return m_maximum;
}

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

QString FluentProgressBar::text() const {
    return m_text;
}

void FluentProgressBar::setText(const QString& text) {
    if (m_text != text) {
        m_text = text;
        updateTextContent();
        update();
        emit textChanged(text);
    }
}

bool FluentProgressBar::isTextVisible() const {
    return m_textVisible;
}

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

bool FluentProgressBar::isAnimated() const {
    return m_animated;
}

void FluentProgressBar::setAnimated(bool animated) {
    if (m_animated != animated) {
        m_animated = animated;
        emit animatedChanged(animated);
    }
}

QColor FluentProgressBar::accentColor() const {
    return m_accentColor;
}

void FluentProgressBar::setAccentColor(const QColor& color) {
    if (m_accentColor != color) {
        m_accentColor = color;
        update();
        emit accentColorChanged(color);
    }
}

qreal FluentProgressBar::percentage() const {
    if (m_maximum == m_minimum) {
        return 0.0;
    }
    return static_cast<qreal>(m_value - m_minimum) / (m_maximum - m_minimum);
}

bool FluentProgressBar::isComplete() const {
    return m_value >= m_maximum;
}

void FluentProgressBar::reset() {
    setValue(m_minimum);
    stop();
}

QSize FluentProgressBar::sizeHint() const {
    const int height = getBarHeight();
    int width = 200; // Default width
    
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
    int width = 100; // Minimum width
    
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

void FluentProgressBar::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    const QRect rect = this->rect();
    
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

void FluentProgressBar::updateAnimation() {
    update();
}

void FluentProgressBar::onAnimationStep() {
    // Update animation progress for custom animations
    const qreal step = 0.02; // 2% per step
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

void FluentProgressBar::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();
    if (m_accentColor == QColor()) {
        m_accentColor = theme.color("accent");
    }
    update();
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
void FluentProgressBar::drawDeterminateBar(QPainter& painter, const QRect& rect) {
    Q_UNUSED(rect)
    const auto& theme = Styling::FluentTheme::instance();
    const QRect progressRect = this->progressRect();
    const int radius = theme.borderRadius("small");

    // Draw background
    painter.fillRect(progressRect, getBackgroundColor());

    // Draw progress fill
    if (percentage() > 0.0) {
        const int fillWidth = static_cast<int>(progressRect.width() * percentage());
        const QRect fillRect(progressRect.x(), progressRect.y(), fillWidth, progressRect.height());

        QPainterPath fillPath;
        fillPath.addRoundedRect(fillRect, radius, radius);
        painter.fillPath(fillPath, getProgressColor());
    }

    // Draw border
    QPainterPath borderPath;
    borderPath.addRoundedRect(progressRect, radius, radius);
    painter.setPen(QPen(theme.color("controlStrokeDefault"), 1));
    painter.drawPath(borderPath);
}

void FluentProgressBar::drawIndeterminateBar(QPainter& painter, const QRect& rect) {
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
        painter.fillPath(indicatorPath, getProgressColor());
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
        const int spanAngle = static_cast<int>(360 * 16 * percentage()); // Qt uses 16ths of degrees

        painter.setPen(QPen(getProgressColor(), strokeWidth, Qt::SolidLine, Qt::RoundCap));
        painter.drawArc(QRect(center.x() - radius, center.y() - radius,
                            radius * 2, radius * 2),
                       90 * 16, -spanAngle); // Start from top, go clockwise
    } else if (m_isRunning && !m_isPaused) {
        // Indeterminate ring animation
        const int spanAngle = 90 * 16; // 90 degrees
        const int startAngle = static_cast<int>(360 * 16 * m_animationProgress);

        painter.setPen(QPen(getProgressColor(), strokeWidth, Qt::SolidLine, Qt::RoundCap));
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

    painter.setBrush(getProgressColor());
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < dotCount; ++i) {
        const int x = startX + i * (dotSize + spacing);

        // Animate dots with wave effect
        qreal opacity = 0.3;
        if (m_isRunning && !m_isPaused) {
            const qreal phase = m_animationProgress * 2 * M_PI + i * M_PI / 3;
            opacity = 0.3 + 0.7 * (qSin(phase) + 1.0) / 2.0;
        }

        painter.setOpacity(opacity);
        painter.drawEllipse(x, y - dotSize/2, dotSize, dotSize);
    }

    painter.setOpacity(1.0);
}

void FluentProgressBar::drawProgressText(QPainter& painter, const QRect& rect) {
    Q_UNUSED(rect)
    const auto& theme = Styling::FluentTheme::instance();
    painter.setPen(theme.color("textPrimary"));
    painter.setFont(font());

    const QRect textRect = this->textRect();
    painter.drawText(textRect, Qt::AlignCenter, m_cachedText);
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
        case FluentProgressBarSize::Small: return 4;
        case FluentProgressBarSize::Medium: return 8;
        case FluentProgressBarSize::Large: return 12;
    }
    return 8;
}

int FluentProgressBar::getRingSize() const {
    switch (m_progressSize) {
        case FluentProgressBarSize::Small: return 24;
        case FluentProgressBarSize::Medium: return 32;
        case FluentProgressBarSize::Large: return 48;
    }
    return 32;
}

QColor FluentProgressBar::getProgressColor() const {
    return m_accentColor.isValid() ? m_accentColor :
           Styling::FluentTheme::instance().color("accent");
}

QColor FluentProgressBar::getBackgroundColor() const {
    const auto& theme = Styling::FluentTheme::instance();
    return theme.color("controlFillSecondary");
}

} // namespace FluentQt::Components
