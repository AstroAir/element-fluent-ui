#pragma once

#include <QPropertyAnimation>
#include <QScrollBar>
#include <QTimer>
#include <memory>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

/**
 * @brief Enhanced scroll bar with ElaWidgetTools-inspired smooth scrolling
 *
 * This component provides smooth, animated scrolling with proper easing curves
 * and performance optimizations based on ElaWidgetTools patterns.
 */
class FluentEnhancedScrollBar : public QScrollBar {
    Q_OBJECT
    Q_PROPERTY(bool smoothScrolling READ isSmoothScrolling WRITE
                   setSmoothScrolling NOTIFY smoothScrollingChanged)
    Q_PROPERTY(qreal speedLimit READ speedLimit WRITE setSpeedLimit NOTIFY
                   speedLimitChanged)
    Q_PROPERTY(bool expandOnHover READ expandOnHover WRITE setExpandOnHover
                   NOTIFY expandOnHoverChanged)

public:
    explicit FluentEnhancedScrollBar(QWidget* parent = nullptr);
    explicit FluentEnhancedScrollBar(Qt::Orientation orientation,
                                     QWidget* parent = nullptr);
    ~FluentEnhancedScrollBar() override;

    // Smooth scrolling properties
    bool isSmoothScrolling() const { return m_smoothScrolling; }
    void setSmoothScrolling(bool enabled);

    qreal speedLimit() const { return m_speedLimit; }
    void setSpeedLimit(qreal limit);

    bool expandOnHover() const { return m_expandOnHover; }
    void setExpandOnHover(bool expand);

    // ElaWidgetTools-inspired smooth scroll methods
    void smoothScrollTo(int value, int duration = 300);
    void smoothScrollBy(int delta, int duration = 300);

signals:
    void smoothScrollingChanged(bool enabled);
    void speedLimitChanged(qreal limit);
    void expandOnHoverChanged(bool expand);
    void smoothScrollFinished();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onSmoothScrollStep();
    void onExpandTimer();

private:
    void setupAnimations();
    void startSmoothScroll(int targetValue, int duration);
    void stopSmoothScroll();
    void updateExpandState();

    // Animation properties
    bool m_smoothScrolling{true};
    qreal m_speedLimit{20.0};
    bool m_expandOnHover{true};

    // Animation objects
    std::unique_ptr<QPropertyAnimation> m_smoothScrollAnimation;
    std::unique_ptr<QPropertyAnimation> m_expandAnimation;
    QTimer* m_expandTimer{nullptr};

    // State tracking
    int m_targetScrollValue{0};
    int m_currentScrollValue{0};
    bool m_isExpanded{false};
    bool m_isDragging{false};

    // Performance optimization
    QTimer* m_updateTimer{nullptr};
    qint64 m_lastWheelTime{0};
};

}  // namespace FluentQt::Components
