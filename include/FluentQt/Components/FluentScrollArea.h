// include/FluentQt/Components/FluentScrollArea.h
#pragma once

#include <QEasingCurve>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <memory>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentScrollBarPolicy {
    AsNeeded,
    AlwaysOff,
    AlwaysOn,
    AutoHide  // Hide when not needed, show on hover
};

enum class FluentScrollDirection { Vertical, Horizontal, Both };

enum class FluentScrollBehavior {
    Instant,
    Smooth,
    Kinetic  // Physics-based scrolling with momentum
};

class FluentScrollBar;
class FluentScrollIndicator;

class FluentScrollArea : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentScrollBarPolicy horizontalScrollBarPolicy READ
                   horizontalScrollBarPolicy WRITE setHorizontalScrollBarPolicy
                       NOTIFY horizontalScrollBarPolicyChanged)
    Q_PROPERTY(FluentScrollBarPolicy verticalScrollBarPolicy READ
                   verticalScrollBarPolicy WRITE setVerticalScrollBarPolicy
                       NOTIFY verticalScrollBarPolicyChanged)
    Q_PROPERTY(FluentScrollBehavior scrollBehavior READ scrollBehavior WRITE
                   setScrollBehavior NOTIFY scrollBehaviorChanged)
    Q_PROPERTY(
        bool smoothScrolling READ smoothScrolling WRITE setSmoothScrolling)
    Q_PROPERTY(int scrollSpeed READ scrollSpeed WRITE setScrollSpeed)
    Q_PROPERTY(bool showScrollIndicators READ showScrollIndicators WRITE
                   setShowScrollIndicators)
    Q_PROPERTY(bool autoHideScrollBars READ autoHideScrollBars WRITE
                   setAutoHideScrollBars)
    Q_PROPERTY(int scrollBarWidth READ scrollBarWidth WRITE setScrollBarWidth)
    Q_PROPERTY(
        bool elasticScrolling READ elasticScrolling WRITE setElasticScrolling)
    Q_PROPERTY(
        qreal scrollBarOpacity READ scrollBarOpacity WRITE setScrollBarOpacity)
    Q_PROPERTY(bool highContrastMode READ highContrastMode WRITE
                   setHighContrastMode NOTIFY accessibilityModeChanged)
    Q_PROPERTY(bool reducedMotionMode READ reducedMotionMode WRITE
                   setReducedMotionMode NOTIFY accessibilityModeChanged)

public:
    explicit FluentScrollArea(QWidget* parent = nullptr);
    explicit FluentScrollArea(QWidget* widget, QWidget* parent = nullptr);
    ~FluentScrollArea() override;

    // Content management
    void setWidget(QWidget* widget);
    QWidget* widget() const;
    void setWidgetResizable(bool resizable);
    bool widgetResizable() const;

    // Scroll bar policies
    FluentScrollBarPolicy horizontalScrollBarPolicy() const noexcept {
        return m_horizontalPolicy;
    }
    void setHorizontalScrollBarPolicy(FluentScrollBarPolicy policy);

    FluentScrollBarPolicy verticalScrollBarPolicy() const noexcept {
        return m_verticalPolicy;
    }
    void setVerticalScrollBarPolicy(FluentScrollBarPolicy policy);

    // Scroll behavior
    FluentScrollBehavior scrollBehavior() const noexcept {
        return m_scrollBehavior;
    }
    void setScrollBehavior(FluentScrollBehavior behavior);

    bool smoothScrolling() const noexcept { return m_smoothScrolling; }
    void setSmoothScrolling(bool smooth);

    int scrollSpeed() const noexcept { return m_scrollSpeed; }
    void setScrollSpeed(int speed);

    bool elasticScrolling() const noexcept { return m_elasticScrolling; }
    void setElasticScrolling(bool elastic);

    // Scroll bar appearance
    bool showScrollIndicators() const noexcept { return m_showIndicators; }
    void setShowScrollIndicators(bool show);

    bool autoHideScrollBars() const noexcept { return m_autoHideScrollBars; }
    void setAutoHideScrollBars(bool autoHide);

    int scrollBarWidth() const noexcept { return m_scrollBarWidth; }
    void setScrollBarWidth(int width);

    qreal scrollBarOpacity() const noexcept { return m_scrollBarOpacity; }
    void setScrollBarOpacity(qreal opacity);

    // Accessibility support
    bool highContrastMode() const noexcept { return m_highContrastMode; }
    void setHighContrastMode(bool enabled);

    bool reducedMotionMode() const noexcept { return m_reducedMotionMode; }
    void setReducedMotionMode(bool enabled);

    void announceScrollPosition();

    // Scroll position
    QPoint scrollPosition() const;
    void setScrollPosition(const QPoint& position, bool animated = true);

    int horizontalScrollValue() const;
    void setHorizontalScrollValue(int value, bool animated = true);

    int verticalScrollValue() const;
    void setVerticalScrollValue(int value, bool animated = true);

    // Scroll ranges
    QSize scrollRange() const;
    QRect visibleRect() const;
    QSize contentSize() const;

    // Programmatic scrolling
    void scrollTo(const QPoint& position, bool animated = true);
    void scrollToWidget(QWidget* widget, bool animated = true);
    void scrollToRect(const QRect& rect, bool animated = true);

    // Scroll by amount
    void scrollBy(const QPoint& delta, bool animated = true);
    void scrollByPixels(int dx, int dy, bool animated = true);
    void scrollByPages(int dx, int dy, bool animated = true);

    // Convenience methods
    void scrollToTop(bool animated = true);
    void scrollToBottom(bool animated = true);
    void scrollToLeft(bool animated = true);
    void scrollToRight(bool animated = true);
    void scrollToCenter(bool animated = true);

    // Size management
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Scroll bar access
    FluentScrollBar* horizontalScrollBar() const {
        return m_horizontalScrollBar.get();
    }
    FluentScrollBar* verticalScrollBar() const {
        return m_verticalScrollBar.get();
    }

public slots:
    void ensureVisible(int x, int y, int xmargin = 50, int ymargin = 50);
    void ensureWidgetVisible(QWidget* childWidget, int xmargin = 50,
                             int ymargin = 50);
    void centerOn(const QPoint& position);
    void centerOnWidget(QWidget* widget);

signals:
    void horizontalScrollBarPolicyChanged(FluentScrollBarPolicy policy);
    void verticalScrollBarPolicyChanged(FluentScrollBarPolicy policy);
    void scrollBehaviorChanged(FluentScrollBehavior behavior);
    void scrollPositionChanged(const QPoint& position);
    void scrollRangeChanged(const QSize& range);
    void scrollStarted();
    void scrollFinished();
    void scrollBarVisibilityChanged(Qt::Orientation orientation, bool visible);
    void accessibilityModeChanged();
    void themeColorsChanged();

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void changeEvent(QEvent* event) override;

    // Keyboard events
    void keyPressEvent(QKeyEvent* event) override;

    // State management
    void updateStateStyle() override;
    void performStateTransition(Core::FluentState from,
                                Core::FluentState to) override;

private slots:
    void onScrollAnimationValueChanged(const QVariant& value);
    void onScrollAnimationFinished();
    void onScrollBarValueChanged(int value);
    void onScrollBarRangeChanged(int min, int max);
    void onAutoHideTimer();
    void onThemeChanged();
    void onAccessibilityModeChanged();

private:
    // Setup methods
    void setupScrollArea();
    void setupScrollBars();
    void setupAnimations();
    void setupAccessibility();
    void initializeThemeProperties();

    // Scroll bar management
    void updateScrollBarVisibility();
    void updateScrollBarGeometry();
    void updateScrollBarStyles();
    void showScrollBars();
    void hideScrollBars();

    // Animation methods
    void animateScrollTo(const QPoint& targetPosition);
    void stopScrollAnimation();
    bool isScrollAnimationRunning() const;

    // Scroll calculations
    QPoint constrainScrollPosition(const QPoint& position) const;
    QPoint calculateScrollDelta(QWheelEvent* event) const;
    void applyKineticScrolling(const QPoint& velocity);

    // Painting methods
    void paintBackground(QPainter& painter, const QRect& rect);
    void paintScrollIndicators(QPainter& painter);

    // Style methods
    QColor getBackgroundColor() const;
    QColor getScrollBarColor() const;
    QColor getScrollBarHoverColor() const;
    QColor getScrollBarPressedColor() const;

    // Utility methods
    void updateContentGeometry();
    void updateScrollRanges();
    bool needsHorizontalScrollBar() const;
    bool needsVerticalScrollBar() const;

private:
    // Content
    QWidget* m_contentWidget{nullptr};
    bool m_widgetResizable{false};

    // Scroll policies
    FluentScrollBarPolicy m_horizontalPolicy{FluentScrollBarPolicy::AsNeeded};
    FluentScrollBarPolicy m_verticalPolicy{FluentScrollBarPolicy::AsNeeded};

    // Scroll behavior
    FluentScrollBehavior m_scrollBehavior{FluentScrollBehavior::Smooth};
    bool m_smoothScrolling{true};
    int m_scrollSpeed{100};
    bool m_elasticScrolling{false};

    // Scroll bar appearance
    bool m_showIndicators{true};
    bool m_autoHideScrollBars{false};
    int m_scrollBarWidth{12};
    qreal m_scrollBarOpacity{1.0};

    // Scroll bars
    std::unique_ptr<FluentScrollBar> m_horizontalScrollBar;
    std::unique_ptr<FluentScrollBar> m_verticalScrollBar;
    std::unique_ptr<FluentScrollIndicator> m_scrollIndicator;

    // Animation
    std::unique_ptr<QPropertyAnimation> m_scrollAnimation;
    QPoint m_animationStartPos;
    QPoint m_animationTargetPos;

    // Auto-hide functionality
    std::unique_ptr<QTimer> m_autoHideTimer;
    bool m_scrollBarsVisible{true};

    // Kinetic scrolling
    QPoint m_lastScrollVelocity;
    QTimer* m_kineticTimer{nullptr};

    // Mouse interaction
    bool m_dragging{false};
    QPoint m_dragStartPos;
    QPoint m_lastMousePos;

    // Performance optimization
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};

    // Enhanced theme integration and accessibility
    bool m_highContrastMode{false};
    bool m_reducedMotionMode{false};
    bool m_showBorder{false};

    // Cached theme colors for performance
    QColor m_cachedBackgroundColor;
    QColor m_cachedBorderColor;
};

}  // namespace FluentQt::Components
