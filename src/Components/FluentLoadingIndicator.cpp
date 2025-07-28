// src/Components/FluentLoadingIndicator.cpp
#include "FluentQt/Components/FluentLoadingIndicator.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QShowEvent>
#include <QHideEvent>
#include <QtMath>

namespace FluentQt::Components {

FluentLoadingIndicator::FluentLoadingIndicator(QWidget* parent)
    : FluentComponent(parent)
    , m_animator(std::make_unique<Animation::FluentAnimator>(this))
{
    setObjectName("FluentLoadingIndicator");
    setupAnimations();
    updateColors();
    
    // Initialize dot phases for dots animation
    m_dotPhases = {0.0, 0.2, 0.4, 0.6, 0.8};
    
    // Initialize bar heights for bars animation
    m_barHeights = {0.3, 0.6, 1.0, 0.6, 0.3};
}

FluentLoadingIndicator::FluentLoadingIndicator(FluentLoadingType type, QWidget* parent)
    : FluentLoadingIndicator(parent)
{
    setLoadingType(type);
}

FluentLoadingIndicator::FluentLoadingIndicator(FluentLoadingType type, FluentLoadingSize size, QWidget* parent)
    : FluentLoadingIndicator(type, parent)
{
    setLoadingSize(size);
}

FluentLoadingIndicator::~FluentLoadingIndicator() = default;

void FluentLoadingIndicator::setupAnimations() {
    FLUENT_PROFILE("FluentLoadingIndicator::setupAnimations");
    
    // Main animation timer
    m_animationTimer = new QTimer(this);
    m_animationTimer->setInterval(16); // ~60 FPS
    connect(m_animationTimer, &QTimer::timeout, this, &FluentLoadingIndicator::onAnimationStep);
    
    // Rotation animation for spinner
    m_rotationAnimation = new QPropertyAnimation(this, "rotationAngle", this);
    m_rotationAnimation->setDuration(getAnimationDuration());
    m_rotationAnimation->setStartValue(0.0);
    m_rotationAnimation->setEndValue(360.0);
    m_rotationAnimation->setLoopCount(-1); // Infinite
    m_rotationAnimation->setEasingCurve(QEasingCurve::Linear);
    
    // Pulse animation
    m_pulseAnimation = new QPropertyAnimation(this, "animationProgress", this);
    m_pulseAnimation->setDuration(getAnimationDuration());
    m_pulseAnimation->setStartValue(0.0);
    m_pulseAnimation->setEndValue(1.0);
    m_pulseAnimation->setLoopCount(-1); // Infinite
    m_pulseAnimation->setEasingCurve(QEasingCurve::InOutSine);
    
    // Connect theme changes
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, &FluentLoadingIndicator::updateColors);
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

QColor FluentLoadingIndicator::color() const {
    return m_color;
}

void FluentLoadingIndicator::setColor(const QColor& color) {
    if (m_color != color) {
        m_color = color;
        update();
        emit colorChanged(color);
    }
}

bool FluentLoadingIndicator::isRunning() const {
    return m_running;
}

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

int FluentLoadingIndicator::speed() const {
    return m_speed;
}

void FluentLoadingIndicator::setSpeed(int speed) {
    const int clampedSpeed = qBound(1, speed, 10);
    if (m_speed != clampedSpeed) {
        m_speed = clampedSpeed;
        
        // Update animation durations
        const int duration = getAnimationDuration();
        m_rotationAnimation->setDuration(duration);
        m_pulseAnimation->setDuration(duration);
        m_animationTimer->setInterval(16 / m_speed); // Adjust frame rate
        
        emit speedChanged(clampedSpeed);
    }
}

QString FluentLoadingIndicator::text() const {
    return m_text;
}

void FluentLoadingIndicator::setText(const QString& text) {
    if (m_text != text) {
        m_text = text;
        updateGeometry();
        update();
        emit textChanged(text);
    }
}

bool FluentLoadingIndicator::isTextVisible() const {
    return m_textVisible;
}

void FluentLoadingIndicator::setTextVisible(bool visible) {
    if (m_textVisible != visible) {
        m_textVisible = visible;
        updateGeometry();
        update();
        emit textVisibleChanged(visible);
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
        height += textHeight + 8; // 8px spacing
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
        
        switch (m_loadingType) {
            case FluentLoadingType::Spinner:
            case FluentLoadingType::Ring:
                m_rotationAnimation->start();
                break;
            case FluentLoadingType::Pulse:
                m_pulseAnimation->start();
                break;
            case FluentLoadingType::Dots:
            case FluentLoadingType::Bars:
            case FluentLoadingType::Wave:
                m_animationTimer->start();
                break;
        }
        
        emit started();
    }
}

void FluentLoadingIndicator::stop() {
    if (m_running) {
        m_running = false;
        
        m_rotationAnimation->stop();
        m_pulseAnimation->stop();
        m_animationTimer->stop();
        
        // Reset animation state
        m_animationProgress = 0.0;
        m_rotationAngle = 0.0;
        
        update();
        emit stopped();
    }
}

void FluentLoadingIndicator::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    const QRect indicatorRect = this->indicatorRect();
    
    // Draw loading indicator
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
    }
    
    // Draw text if visible
    if (m_textVisible && !m_text.isEmpty()) {
        drawText(painter, textRect());
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

void FluentLoadingIndicator::updateAnimation() {
    update();
}

void FluentLoadingIndicator::onAnimationStep() {
    // Update animation progress
    m_animationProgress += 0.05 * m_speed; // Adjust speed
    if (m_animationProgress > 1.0) {
        m_animationProgress = 0.0;
    }
    
    // Update dot phases for dots animation
    if (m_loadingType == FluentLoadingType::Dots) {
        for (int i = 0; i < m_dotPhases.size(); ++i) {
            m_dotPhases[i] = fmod(m_animationProgress + i * 0.2, 1.0);
        }
    }
    
    // Update bar heights for bars animation
    if (m_loadingType == FluentLoadingType::Bars) {
        for (int i = 0; i < m_barHeights.size(); ++i) {
            const qreal phase = fmod(m_animationProgress + i * 0.15, 1.0);
            m_barHeights[i] = 0.3 + 0.7 * qAbs(qSin(phase * M_PI));
        }
    }
    
    update();
}

void FluentLoadingIndicator::updateColors() {
    const auto& theme = Styling::FluentTheme::instance();
    if (!m_color.isValid()) {
        m_color = theme.color("accent");
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
    const int strokeWidth = qMax(1, radius / 8);

    painter.setPen(QPen(getIndicatorColor(), strokeWidth, Qt::SolidLine, Qt::RoundCap));

    // Draw partial circle that rotates
    const int spanAngle = 270 * 16; // 270 degrees in 16ths
    const int startAngle = static_cast<int>(m_rotationAngle * 16);

    painter.drawArc(QRect(center.x() - radius, center.y() - radius,
                         radius * 2, radius * 2),
                   startAngle, spanAngle);
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

        painter.drawEllipse(x - currentSize/2, y - currentSize/2, currentSize, currentSize);
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

    const int startX = rect.center().x() - (barCount * barWidth + (barCount - 1) * spacing) / 2;
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
    const int strokeWidth = qMax(2, outerRadius / 6);

    // Draw background ring
    QColor bgColor = getIndicatorColor();
    bgColor.setAlphaF(0.2);
    painter.setPen(QPen(bgColor, strokeWidth));
    painter.drawEllipse(center, outerRadius, outerRadius);

    // Draw rotating segment
    painter.setPen(QPen(getIndicatorColor(), strokeWidth, Qt::SolidLine, Qt::RoundCap));
    const int spanAngle = 90 * 16; // 90 degrees
    const int startAngle = static_cast<int>(m_rotationAngle * 16);

    painter.drawArc(QRect(center.x() - outerRadius, center.y() - outerRadius,
                         outerRadius * 2, outerRadius * 2),
                   startAngle, spanAngle);
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

void FluentLoadingIndicator::drawText(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    painter.setPen(theme.color("textSecondary"));
    painter.setFont(font());
    painter.drawText(rect, Qt::AlignCenter, m_text);
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
        case FluentLoadingSize::Small: return 16;
        case FluentLoadingSize::Medium: return 24;
        case FluentLoadingSize::Large: return 32;
        case FluentLoadingSize::ExtraLarge: return 48;
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
    // Base duration is 2000ms, adjusted by speed (1-10)
    const int baseDuration = 2000;
    return baseDuration / m_speed;
}

} // namespace FluentQt::Components
