// src/Components/FluentEnhancedScrollBar.cpp
#include "FluentQt/Components/FluentEnhancedScrollBar.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QElapsedTimer>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>
#include <QtMath>

namespace FluentQt::Components {

FluentEnhancedScrollBar::FluentEnhancedScrollBar(QWidget* parent)
    : QScrollBar(parent) {
    setObjectName("FluentEnhancedScrollBar");
    setupAnimations();

    // Apply ElaWidgetTools-inspired optimizations
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_Hover, true);

    // Set default properties
    setSingleStep(1);
    setPageStep(10);
}

FluentEnhancedScrollBar::FluentEnhancedScrollBar(Qt::Orientation orientation,
                                                 QWidget* parent)
    : FluentEnhancedScrollBar(parent) {
    setOrientation(orientation);
}

FluentEnhancedScrollBar::~FluentEnhancedScrollBar() = default;

void FluentEnhancedScrollBar::setupAnimations() {
    // Smooth scroll animation with ElaWidgetTools-inspired easing
    m_smoothScrollAnimation =
        std::make_unique<QPropertyAnimation>(this, "value");
    m_smoothScrollAnimation->setEasingCurve(QEasingCurve::OutSine);
    m_smoothScrollAnimation->setDuration(300);

    connect(m_smoothScrollAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentEnhancedScrollBar::smoothScrollFinished);

    connect(m_smoothScrollAnimation.get(), &QPropertyAnimation::finished,
            [this]() { m_currentScrollValue = value(); });

    // Expand animation for hover effect
    m_expandAnimation = std::make_unique<QPropertyAnimation>(this, "geometry");
    m_expandAnimation->setDuration(200);
    m_expandAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Expand timer for delayed hover response
    m_expandTimer = new QTimer(this);
    m_expandTimer->setSingleShot(true);
    m_expandTimer->setInterval(350);  // ElaWidgetTools-inspired delay
    connect(m_expandTimer, &QTimer::timeout, this,
            &FluentEnhancedScrollBar::onExpandTimer);

    // Update timer for performance optimization
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(16);  // 60 FPS
    m_updateTimer->setSingleShot(true);
}

void FluentEnhancedScrollBar::setSmoothScrolling(bool enabled) {
    if (m_smoothScrolling != enabled) {
        m_smoothScrolling = enabled;
        emit smoothScrollingChanged(enabled);
    }
}

void FluentEnhancedScrollBar::setSpeedLimit(qreal limit) {
    if (!qFuzzyCompare(m_speedLimit, limit)) {
        m_speedLimit = limit;
        emit speedLimitChanged(limit);
    }
}

void FluentEnhancedScrollBar::setExpandOnHover(bool expand) {
    if (m_expandOnHover != expand) {
        m_expandOnHover = expand;
        emit expandOnHoverChanged(expand);
    }
}

void FluentEnhancedScrollBar::smoothScrollTo(int value, int duration) {
    if (!m_smoothScrolling) {
        setValue(value);
        return;
    }

    startSmoothScroll(value, duration);
}

void FluentEnhancedScrollBar::smoothScrollBy(int delta, int duration) {
    smoothScrollTo(value() + delta, duration);
}

void FluentEnhancedScrollBar::wheelEvent(QWheelEvent* event) {
    if (!m_smoothScrolling) {
        QScrollBar::wheelEvent(event);
        return;
    }

    // ElaWidgetTools-inspired smooth wheel scrolling
    int delta = event->angleDelta().y();
    if (delta == 0) {
        delta = event->angleDelta().x();
    }

    if (delta == 0) {
        event->ignore();
        return;
    }

    // Apply speed limiting
    qint64 currentTime = QElapsedTimer().elapsed();
    if (currentTime - m_lastWheelTime < 16) {  // Limit to 60 FPS
        event->accept();
        return;
    }
    m_lastWheelTime = currentTime;

    // Calculate scroll amount with speed limiting
    int scrollAmount =
        qBound(-static_cast<int>(m_speedLimit), delta / 120 * singleStep(),
               static_cast<int>(m_speedLimit));

    // Check bounds
    int newValue = value() + scrollAmount;
    if ((newValue <= minimum() && scrollAmount < 0) ||
        (newValue >= maximum() && scrollAmount > 0)) {
        QScrollBar::wheelEvent(event);
        return;
    }

    // Start smooth scroll
    if (m_smoothScrollAnimation->state() == QAbstractAnimation::Stopped) {
        m_currentScrollValue = value();
    }

    smoothScrollBy(scrollAmount, 200);  // Shorter duration for wheel events
    event->accept();
}

void FluentEnhancedScrollBar::startSmoothScroll(int targetValue, int duration) {
    m_targetScrollValue = qBound(minimum(), targetValue, maximum());

    if (m_smoothScrollAnimation->state() != QAbstractAnimation::Stopped) {
        m_smoothScrollAnimation->stop();
    }

    m_smoothScrollAnimation->setStartValue(value());
    m_smoothScrollAnimation->setEndValue(m_targetScrollValue);
    m_smoothScrollAnimation->setDuration(duration);
    m_smoothScrollAnimation->start();
}

void FluentEnhancedScrollBar::stopSmoothScroll() {
    if (m_smoothScrollAnimation->state() != QAbstractAnimation::Stopped) {
        m_smoothScrollAnimation->stop();
        m_currentScrollValue = value();
    }
}

void FluentEnhancedScrollBar::mousePressEvent(QMouseEvent* event) {
    stopSmoothScroll();
    m_isDragging = true;
    QScrollBar::mousePressEvent(event);
    m_currentScrollValue = value();
}

void FluentEnhancedScrollBar::mouseReleaseEvent(QMouseEvent* event) {
    m_isDragging = false;
    QScrollBar::mouseReleaseEvent(event);
    m_currentScrollValue = value();
}

void FluentEnhancedScrollBar::mouseMoveEvent(QMouseEvent* event) {
    if (m_isDragging) {
        stopSmoothScroll();
    }
    QScrollBar::mouseMoveEvent(event);
    m_currentScrollValue = value();
}

void FluentEnhancedScrollBar::enterEvent(QEnterEvent* event) {
    QScrollBar::enterEvent(event);

    if (m_expandOnHover) {
        m_expandTimer->stop();
        if (!m_isExpanded) {
            m_expandTimer->start();
        }
    }
}

void FluentEnhancedScrollBar::leaveEvent(QEvent* event) {
    QScrollBar::leaveEvent(event);

    if (m_expandOnHover) {
        m_expandTimer->stop();
        if (m_isExpanded) {
            m_expandTimer->start();
        }
    }
}

void FluentEnhancedScrollBar::onExpandTimer() { updateExpandState(); }

void FluentEnhancedScrollBar::updateExpandState() {
    bool shouldExpand = underMouse() && m_expandOnHover;

    if (m_isExpanded != shouldExpand) {
        m_isExpanded = shouldExpand;

        // Trigger expand/collapse animation
        QRect currentGeometry = geometry();
        QRect targetGeometry = currentGeometry;

        if (orientation() == Qt::Vertical) {
            int newWidth = m_isExpanded ? 16 : 12;
            targetGeometry.setWidth(newWidth);
        } else {
            int newHeight = m_isExpanded ? 16 : 12;
            targetGeometry.setHeight(newHeight);
        }

        m_expandAnimation->setStartValue(currentGeometry);
        m_expandAnimation->setEndValue(targetGeometry);
        m_expandAnimation->start();
    }
}

void FluentEnhancedScrollBar::paintEvent(QPaintEvent* event) {
    // Use default painting for now - can be enhanced later
    QScrollBar::paintEvent(event);
}

void FluentEnhancedScrollBar::onSmoothScrollStep() {
    // This method can be used for custom smooth scrolling logic if needed
    update();
}

}  // namespace FluentQt::Components
