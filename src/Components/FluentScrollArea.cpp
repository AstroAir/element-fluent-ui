// src/Components/FluentScrollArea.cpp
#include "FluentQt/Components/FluentScrollArea.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAccessible>
#include <QDebug>
#include <QtMath>

namespace FluentQt::Components {

// Custom scroll bar implementation
class FluentScrollBar : public QScrollBar {
    Q_OBJECT

public:
    explicit FluentScrollBar(Qt::Orientation orientation, QWidget* parent = nullptr)
        : QScrollBar(orientation, parent) {
        setStyleSheet(getFluentScrollBarStyle());
        setAttribute(Qt::WA_Hover, true);
    }

    void setFluentOpacity(qreal opacity) {
        m_opacity = opacity;
        update();
    }

    qreal fluentOpacity() const { return m_opacity; }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(m_opacity);

        const QRect grooveRect = getGrooveRect();
        const QRect handleRect = getHandleRect();

        // Paint groove
        const QColor grooveColor = QColor(200, 200, 200, 50);
        painter.fillRect(grooveRect, grooveColor);

        // Paint handle
        QColor handleColor = QColor(100, 100, 100);
        if (underMouse()) {
            handleColor = QColor(0, 120, 215);
        }
        
        painter.setBrush(handleColor);
        painter.setPen(Qt::NoPen);
        
        const int radius = qMin(handleRect.width(), handleRect.height()) / 2;
        painter.drawRoundedRect(handleRect, radius, radius);
    }

private:
    QString getFluentScrollBarStyle() const {
        return R"(
            QScrollBar {
                background: transparent;
                border: none;
            }
            QScrollBar::handle {
                background: transparent;
                border: none;
            }
            QScrollBar::add-line, QScrollBar::sub-line {
                background: transparent;
                border: none;
            }
        )";
    }

    QRect getGrooveRect() const {
        const QRect rect = this->rect();
        if (orientation() == Qt::Horizontal) {
            return rect.adjusted(0, 2, 0, -2);
        } else {
            return rect.adjusted(2, 0, -2, 0);
        }
    }

    QRect getHandleRect() const {
        const QRect rect = this->rect();
        const int handleSize = qRound(static_cast<qreal>(maximum() - minimum() + pageStep()) / 
                                     qMax(1, maximum() - minimum() + pageStep()) * 
                                     (orientation() == Qt::Horizontal ? rect.width() : rect.height()));
        
        const int handlePos = qRound(static_cast<qreal>(value() - minimum()) / 
                                    qMax(1, maximum() - minimum()) * 
                                    ((orientation() == Qt::Horizontal ? rect.width() : rect.height()) - handleSize));

        if (orientation() == Qt::Horizontal) {
            return QRect(handlePos, 2, handleSize, rect.height() - 4);
        } else {
            return QRect(2, handlePos, rect.width() - 4, handleSize);
        }
    }

private:
    qreal m_opacity{1.0};
};

// Scroll position indicator
class FluentScrollIndicator : public QWidget {
public:
    explicit FluentScrollIndicator(QWidget* parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setVisible(false);
    }

    void showIndicator(const QString& text, const QPoint& position) {
        m_text = text;
        move(position);
        setVisible(true);
        
        if (!m_hideTimer) {
            m_hideTimer = new QTimer(this);
            m_hideTimer->setSingleShot(true);
            connect(m_hideTimer, &QTimer::timeout, this, &QWidget::hide);
        }
        
        m_hideTimer->start(1000);
        update();
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)
        
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QRect rect = this->rect();
        const QColor backgroundColor = QColor(0, 0, 0, 180);
        const QColor textColor = Qt::white;

        painter.setBrush(backgroundColor);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect, 4, 4);

        painter.setPen(textColor);
        painter.drawText(rect, Qt::AlignCenter, m_text);
    }

    QSize sizeHint() const override {
        const QFontMetrics fm(font());
        return fm.boundingRect(m_text).size() + QSize(16, 8);
    }

private:
    QString m_text;
    QTimer* m_hideTimer{nullptr};
};

FluentScrollArea::FluentScrollArea(QWidget* parent)
    : Core::FluentComponent(parent)
    , m_horizontalScrollBar(std::make_unique<FluentScrollBar>(Qt::Horizontal, this))
    , m_verticalScrollBar(std::make_unique<FluentScrollBar>(Qt::Vertical, this))
    , m_scrollIndicator(std::make_unique<FluentScrollIndicator>(this))
    , m_scrollAnimation(std::make_unique<QPropertyAnimation>(this, "scrollPosition"))
    , m_autoHideTimer(std::make_unique<QTimer>(this))
{
    setupScrollArea();
    setupScrollBars();
    setupAnimations();
    setupAccessibility();

    // Connect to theme changes
    connect(&FluentQt::Styling::FluentTheme::instance(), &FluentQt::Styling::FluentTheme::themeChanged,
            this, &FluentScrollArea::onThemeChanged);
}

FluentScrollArea::FluentScrollArea(QWidget* widget, QWidget* parent)
    : Core::FluentComponent(parent)
    , m_horizontalScrollBar(std::make_unique<FluentScrollBar>(Qt::Horizontal, this))
    , m_verticalScrollBar(std::make_unique<FluentScrollBar>(Qt::Vertical, this))
    , m_scrollIndicator(std::make_unique<FluentScrollIndicator>(this))
    , m_scrollAnimation(std::make_unique<QPropertyAnimation>(this, "scrollPosition"))
    , m_autoHideTimer(std::make_unique<QTimer>(this))
{
    setupScrollArea();
    setupScrollBars();
    setupAnimations();
    setupAccessibility();

    // Connect to theme changes
    connect(&FluentQt::Styling::FluentTheme::instance(), &FluentQt::Styling::FluentTheme::themeChanged,
            this, &FluentScrollArea::onThemeChanged);

    setWidget(widget);
}

FluentScrollArea::~FluentScrollArea() = default;

void FluentScrollArea::setWidget(QWidget* widget) {
    if (m_contentWidget == widget) {
        return;
    }

    if (m_contentWidget) {
        m_contentWidget->setParent(nullptr);
    }

    m_contentWidget = widget;
    if (m_contentWidget) {
        m_contentWidget->setParent(this);
        updateContentGeometry();
        updateScrollRanges();
    }

    m_sizeHintValid = false;
    updateGeometry();
}

QWidget* FluentScrollArea::widget() const {
    return m_contentWidget;
}

void FluentScrollArea::setWidgetResizable(bool resizable) {
    if (m_widgetResizable != resizable) {
        m_widgetResizable = resizable;
        updateContentGeometry();
    }
}

bool FluentScrollArea::widgetResizable() const {
    return m_widgetResizable;
}

void FluentScrollArea::setHorizontalScrollBarPolicy(FluentScrollBarPolicy policy) {
    if (m_horizontalPolicy != policy) {
        m_horizontalPolicy = policy;
        updateScrollBarVisibility();
        emit horizontalScrollBarPolicyChanged(policy);
    }
}

void FluentScrollArea::setVerticalScrollBarPolicy(FluentScrollBarPolicy policy) {
    if (m_verticalPolicy != policy) {
        m_verticalPolicy = policy;
        updateScrollBarVisibility();
        emit verticalScrollBarPolicyChanged(policy);
    }
}

void FluentScrollArea::setScrollBehavior(FluentScrollBehavior behavior) {
    if (m_scrollBehavior != behavior) {
        m_scrollBehavior = behavior;
        emit scrollBehaviorChanged(behavior);
    }
}

void FluentScrollArea::setSmoothScrolling(bool smooth) {
    m_smoothScrolling = smooth;
}

void FluentScrollArea::setScrollSpeed(int speed) {
    m_scrollSpeed = qMax(1, speed);
}

void FluentScrollArea::setElasticScrolling(bool elastic) {
    m_elasticScrolling = elastic;
}

void FluentScrollArea::setShowScrollIndicators(bool show) {
    m_showIndicators = show;
}

void FluentScrollArea::setAutoHideScrollBars(bool autoHide) {
    if (m_autoHideScrollBars != autoHide) {
        m_autoHideScrollBars = autoHide;
        updateScrollBarVisibility();
    }
}

void FluentScrollArea::setScrollBarWidth(int width) {
    if (m_scrollBarWidth != width && width > 0) {
        m_scrollBarWidth = width;
        updateScrollBarGeometry();
    }
}

void FluentScrollArea::setScrollBarOpacity(qreal opacity) {
    if (m_scrollBarOpacity != opacity) {
        m_scrollBarOpacity = qBound(0.0, opacity, 1.0);
        m_horizontalScrollBar->setFluentOpacity(m_scrollBarOpacity);
        m_verticalScrollBar->setFluentOpacity(m_scrollBarOpacity);
    }
}

QPoint FluentScrollArea::scrollPosition() const {
    return QPoint(m_horizontalScrollBar->value(), m_verticalScrollBar->value());
}

void FluentScrollArea::setScrollPosition(const QPoint& position, bool animated) {
    const QPoint constrainedPos = constrainScrollPosition(position);
    
    if (animated && m_smoothScrolling) {
        animateScrollTo(constrainedPos);
    } else {
        m_horizontalScrollBar->setValue(constrainedPos.x());
        m_verticalScrollBar->setValue(constrainedPos.y());
    }
}

int FluentScrollArea::horizontalScrollValue() const {
    return m_horizontalScrollBar->value();
}

void FluentScrollArea::setHorizontalScrollValue(int value, bool animated) {
    setScrollPosition(QPoint(value, verticalScrollValue()), animated);
}

int FluentScrollArea::verticalScrollValue() const {
    return m_verticalScrollBar->value();
}

void FluentScrollArea::setVerticalScrollValue(int value, bool animated) {
    setScrollPosition(QPoint(horizontalScrollValue(), value), animated);
}

QSize FluentScrollArea::scrollRange() const {
    return QSize(m_horizontalScrollBar->maximum() - m_horizontalScrollBar->minimum(),
                 m_verticalScrollBar->maximum() - m_verticalScrollBar->minimum());
}

QRect FluentScrollArea::visibleRect() const {
    const QPoint pos = scrollPosition();
    return QRect(pos, size());
}

QSize FluentScrollArea::contentSize() const {
    if (m_contentWidget) {
        return m_contentWidget->size();
    }
    return QSize();
}

void FluentScrollArea::scrollTo(const QPoint& position, bool animated) {
    setScrollPosition(position, animated);
}

void FluentScrollArea::scrollToWidget(QWidget* widget, bool animated) {
    if (!widget || !m_contentWidget) {
        return;
    }

    const QPoint widgetPos = widget->mapTo(m_contentWidget, QPoint(0, 0));
    scrollTo(widgetPos, animated);
}

void FluentScrollArea::scrollToRect(const QRect& rect, bool animated) {
    const QRect visibleArea = visibleRect();
    QPoint targetPos = scrollPosition();

    // Calculate target position to make rect visible
    if (rect.left() < visibleArea.left()) {
        targetPos.setX(rect.left());
    } else if (rect.right() > visibleArea.right()) {
        targetPos.setX(rect.right() - visibleArea.width());
    }

    if (rect.top() < visibleArea.top()) {
        targetPos.setY(rect.top());
    } else if (rect.bottom() > visibleArea.bottom()) {
        targetPos.setY(rect.bottom() - visibleArea.height());
    }

    scrollTo(targetPos, animated);
}

void FluentScrollArea::scrollBy(const QPoint& delta, bool animated) {
    const QPoint currentPos = scrollPosition();
    scrollTo(currentPos + delta, animated);
}

void FluentScrollArea::scrollByPixels(int dx, int dy, bool animated) {
    scrollBy(QPoint(dx, dy), animated);
}

void FluentScrollArea::scrollByPages(int dx, int dy, bool animated) {
    const int pageWidth = m_horizontalScrollBar->pageStep();
    const int pageHeight = m_verticalScrollBar->pageStep();
    scrollBy(QPoint(dx * pageWidth, dy * pageHeight), animated);
}

void FluentScrollArea::scrollToTop(bool animated) {
    setVerticalScrollValue(m_verticalScrollBar->minimum(), animated);
}

void FluentScrollArea::scrollToBottom(bool animated) {
    setVerticalScrollValue(m_verticalScrollBar->maximum(), animated);
}

void FluentScrollArea::scrollToLeft(bool animated) {
    setHorizontalScrollValue(m_horizontalScrollBar->minimum(), animated);
}

void FluentScrollArea::scrollToRight(bool animated) {
    setHorizontalScrollValue(m_horizontalScrollBar->maximum(), animated);
}

void FluentScrollArea::scrollToCenter(bool animated) {
    const QPoint centerPos(
        (m_horizontalScrollBar->minimum() + m_horizontalScrollBar->maximum()) / 2,
        (m_verticalScrollBar->minimum() + m_verticalScrollBar->maximum()) / 2
    );
    scrollTo(centerPos, animated);
}

QSize FluentScrollArea::sizeHint() const {
    if (!m_sizeHintValid) {
        if (m_contentWidget) {
            m_cachedSizeHint = m_contentWidget->sizeHint() + QSize(m_scrollBarWidth * 2, m_scrollBarWidth * 2);
        } else {
            m_cachedSizeHint = QSize(300, 200);
        }
        m_sizeHintValid = true;
    }
    return m_cachedSizeHint;
}

QSize FluentScrollArea::minimumSizeHint() const {
    return QSize(100, 100);
}

void FluentScrollArea::ensureVisible(int x, int y, int xmargin, int ymargin) {
    const QRect targetRect(x - xmargin, y - ymargin, 2 * xmargin, 2 * ymargin);
    scrollToRect(targetRect, true);
}

void FluentScrollArea::ensureWidgetVisible(QWidget* childWidget, int xmargin, int ymargin) {
    if (!childWidget || !m_contentWidget) {
        return;
    }

    const QRect widgetRect = childWidget->geometry();
    const QRect targetRect = widgetRect.adjusted(-xmargin, -ymargin, xmargin, ymargin);
    scrollToRect(targetRect, true);
}

void FluentScrollArea::centerOn(const QPoint& position) {
    const QSize viewportSize = size();
    const QPoint targetPos(
        position.x() - viewportSize.width() / 2,
        position.y() - viewportSize.height() / 2
    );
    scrollTo(targetPos, true);
}

void FluentScrollArea::centerOnWidget(QWidget* widget) {
    if (!widget || !m_contentWidget) {
        return;
    }

    const QRect widgetRect = widget->geometry();
    centerOn(widgetRect.center());
}

void FluentScrollArea::setupScrollArea() {
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::StrongFocus);
}

void FluentScrollArea::setupScrollBars() {
    // Configure horizontal scroll bar
    m_horizontalScrollBar->setOrientation(Qt::Horizontal);
    m_horizontalScrollBar->setParent(this);
    connect(m_horizontalScrollBar.get(), QOverload<int>::of(&QScrollBar::valueChanged),
            this, &FluentScrollArea::onScrollBarValueChanged);
    connect(m_horizontalScrollBar.get(), &QScrollBar::rangeChanged,
            this, &FluentScrollArea::onScrollBarRangeChanged);

    // Configure vertical scroll bar
    m_verticalScrollBar->setOrientation(Qt::Vertical);
    m_verticalScrollBar->setParent(this);
    connect(m_verticalScrollBar.get(), QOverload<int>::of(&QScrollBar::valueChanged),
            this, &FluentScrollArea::onScrollBarValueChanged);
    connect(m_verticalScrollBar.get(), &QScrollBar::rangeChanged,
            this, &FluentScrollArea::onScrollBarRangeChanged);

    updateScrollBarGeometry();
    updateScrollBarVisibility();
}

void FluentScrollArea::setupAnimations() {
    m_scrollAnimation->setDuration(300);
    m_scrollAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_scrollAnimation.get(), &QPropertyAnimation::valueChanged,
            this, &FluentScrollArea::onScrollAnimationValueChanged);
    connect(m_scrollAnimation.get(), &QPropertyAnimation::finished,
            this, &FluentScrollArea::onScrollAnimationFinished);

    // Setup auto-hide timer
    m_autoHideTimer->setSingleShot(true);
    m_autoHideTimer->setInterval(2000);
    connect(m_autoHideTimer.get(), &QTimer::timeout,
            this, &FluentScrollArea::onAutoHideTimer);
}

void FluentScrollArea::setupAccessibility() {
    setAccessibleName("Scroll Area");
    setAccessibleDescription("A scrollable area that can contain content larger than the visible area");
}

// Event handling
void FluentScrollArea::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect rect = this->rect();

    // Paint background
    paintBackground(painter, rect);

    // Paint scroll indicators if enabled
    if (m_showIndicators && (m_horizontalScrollBar->isVisible() || m_verticalScrollBar->isVisible())) {
        paintScrollIndicators(painter);
    }
}

void FluentScrollArea::resizeEvent(QResizeEvent* event) {
    updateContentGeometry();
    updateScrollBarGeometry();
    updateScrollRanges();
    m_sizeHintValid = false;
    Core::FluentComponent::resizeEvent(event);
}

void FluentScrollArea::wheelEvent(QWheelEvent* event) {
    if (!m_contentWidget) {
        event->ignore();
        return;
    }

    const QPoint delta = calculateScrollDelta(event);

    if (m_scrollBehavior == FluentScrollBehavior::Kinetic) {
        applyKineticScrolling(delta);
    } else {
        scrollBy(delta, m_smoothScrolling);
    }

    // Show scroll bars temporarily if auto-hide is enabled
    if (m_autoHideScrollBars) {
        showScrollBars();
        m_autoHideTimer->start();
    }

    event->accept();
}

void FluentScrollArea::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPos = event->pos();
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }

    Core::FluentComponent::mousePressEvent(event);
}

void FluentScrollArea::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        const QPoint delta = m_lastMousePos - event->pos();
        scrollBy(delta, false);
        m_lastMousePos = event->pos();
    }

    Core::FluentComponent::mouseMoveEvent(event);
}

void FluentScrollArea::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_dragging) {
        m_dragging = false;
        setCursor(Qt::ArrowCursor);
    }

    Core::FluentComponent::mouseReleaseEvent(event);
}

void FluentScrollArea::enterEvent(QEnterEvent* event) {
    if (m_autoHideScrollBars) {
        showScrollBars();
    }
    Core::FluentComponent::enterEvent(event);
}

void FluentScrollArea::leaveEvent(QEvent* event) {
    if (m_autoHideScrollBars && !m_dragging) {
        m_autoHideTimer->start();
    }
    Core::FluentComponent::leaveEvent(event);
}

void FluentScrollArea::changeEvent(QEvent* event) {
    if (event->type() == QEvent::EnabledChange) {
        updateScrollBarVisibility();
    }
    Core::FluentComponent::changeEvent(event);
}

void FluentScrollArea::keyPressEvent(QKeyEvent* event) {
    if (!hasFocus()) {
        Core::FluentComponent::keyPressEvent(event);
        return;
    }

    const int step = event->modifiers() & Qt::ShiftModifier ? m_scrollSpeed * 5 : m_scrollSpeed;
    QPoint delta;

    switch (event->key()) {
    case Qt::Key_Left:
        delta = QPoint(-step, 0);
        break;
    case Qt::Key_Right:
        delta = QPoint(step, 0);
        break;
    case Qt::Key_Up:
        delta = QPoint(0, -step);
        break;
    case Qt::Key_Down:
        delta = QPoint(0, step);
        break;
    case Qt::Key_PageUp:
        delta = QPoint(0, -m_verticalScrollBar->pageStep());
        break;
    case Qt::Key_PageDown:
        delta = QPoint(0, m_verticalScrollBar->pageStep());
        break;
    case Qt::Key_Home:
        scrollToTop(true);
        event->accept();
        return;
    case Qt::Key_End:
        scrollToBottom(true);
        event->accept();
        return;
    default:
        Core::FluentComponent::keyPressEvent(event);
        return;
    }

    if (!delta.isNull()) {
        scrollBy(delta, m_smoothScrolling);
        event->accept();
    }
}

void FluentScrollArea::updateStateStyle() {
    updateScrollBarStyles();
    update();
}

void FluentScrollArea::performStateTransition(Core::FluentState from, Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    updateStateStyle();
}

// Slot implementations
void FluentScrollArea::onScrollAnimationValueChanged(const QVariant& value) {
    const QPoint pos = value.toPoint();
    m_horizontalScrollBar->setValue(pos.x());
    m_verticalScrollBar->setValue(pos.y());
    emit scrollPositionChanged(pos);
}

void FluentScrollArea::onScrollAnimationFinished() {
    emit scrollFinished();
}

void FluentScrollArea::onScrollBarValueChanged(int value) {
    Q_UNUSED(value)
    updateContentGeometry();
    emit scrollPositionChanged(scrollPosition());
}

void FluentScrollArea::onScrollBarRangeChanged(int min, int max) {
    Q_UNUSED(min)
    Q_UNUSED(max)
    updateScrollBarVisibility();
    emit scrollRangeChanged(scrollRange());
}

void FluentScrollArea::onAutoHideTimer() {
    if (m_autoHideScrollBars && !underMouse() && !m_dragging) {
        hideScrollBars();
    }
}

void FluentScrollArea::onThemeChanged() {
    updateScrollBarStyles();
    updateStateStyle();
}

// Utility methods
void FluentScrollArea::updateScrollBarVisibility() {
    const bool hNeedsScrollBar = needsHorizontalScrollBar();
    const bool vNeedsScrollBar = needsVerticalScrollBar();

    bool hVisible = false;
    bool vVisible = false;

    switch (m_horizontalPolicy) {
    case FluentScrollBarPolicy::AlwaysOff:
        hVisible = false;
        break;
    case FluentScrollBarPolicy::AlwaysOn:
        hVisible = true;
        break;
    case FluentScrollBarPolicy::AsNeeded:
    case FluentScrollBarPolicy::AutoHide:
        hVisible = hNeedsScrollBar;
        break;
    }

    switch (m_verticalPolicy) {
    case FluentScrollBarPolicy::AlwaysOff:
        vVisible = false;
        break;
    case FluentScrollBarPolicy::AlwaysOn:
        vVisible = true;
        break;
    case FluentScrollBarPolicy::AsNeeded:
    case FluentScrollBarPolicy::AutoHide:
        vVisible = vNeedsScrollBar;
        break;
    }

    if (m_autoHideScrollBars && !m_scrollBarsVisible) {
        hVisible = false;
        vVisible = false;
    }

    m_horizontalScrollBar->setVisible(hVisible);
    m_verticalScrollBar->setVisible(vVisible);

    emit scrollBarVisibilityChanged(Qt::Horizontal, hVisible);
    emit scrollBarVisibilityChanged(Qt::Vertical, vVisible);
}

void FluentScrollArea::updateScrollBarGeometry() {
    const QRect rect = this->rect();
    const int scrollBarWidth = m_scrollBarWidth;

    // Position horizontal scroll bar
    const QRect hScrollRect(0, rect.height() - scrollBarWidth,
                           rect.width() - (m_verticalScrollBar->isVisible() ? scrollBarWidth : 0),
                           scrollBarWidth);
    m_horizontalScrollBar->setGeometry(hScrollRect);

    // Position vertical scroll bar
    const QRect vScrollRect(rect.width() - scrollBarWidth, 0,
                           scrollBarWidth,
                           rect.height() - (m_horizontalScrollBar->isVisible() ? scrollBarWidth : 0));
    m_verticalScrollBar->setGeometry(vScrollRect);
}

void FluentScrollArea::updateScrollBarStyles() {
    // Update scroll bar colors based on current theme
    m_horizontalScrollBar->setFluentOpacity(m_scrollBarOpacity);
    m_verticalScrollBar->setFluentOpacity(m_scrollBarOpacity);
}

void FluentScrollArea::showScrollBars() {
    if (!m_scrollBarsVisible) {
        m_scrollBarsVisible = true;
        updateScrollBarVisibility();
    }
}

void FluentScrollArea::hideScrollBars() {
    if (m_scrollBarsVisible) {
        m_scrollBarsVisible = false;
        updateScrollBarVisibility();
    }
}

void FluentScrollArea::animateScrollTo(const QPoint& targetPosition) {
    if (m_scrollAnimation->state() == QPropertyAnimation::Running) {
        m_scrollAnimation->stop();
    }

    m_animationStartPos = scrollPosition();
    m_animationTargetPos = constrainScrollPosition(targetPosition);

    m_scrollAnimation->setStartValue(m_animationStartPos);
    m_scrollAnimation->setEndValue(m_animationTargetPos);

    emit scrollStarted();
    m_scrollAnimation->start();
}

void FluentScrollArea::stopScrollAnimation() {
    if (m_scrollAnimation->state() == QPropertyAnimation::Running) {
        m_scrollAnimation->stop();
    }
}

bool FluentScrollArea::isScrollAnimationRunning() const {
    return m_scrollAnimation->state() == QPropertyAnimation::Running;
}

QPoint FluentScrollArea::constrainScrollPosition(const QPoint& position) const {
    const int x = qBound(m_horizontalScrollBar->minimum(), position.x(), m_horizontalScrollBar->maximum());
    const int y = qBound(m_verticalScrollBar->minimum(), position.y(), m_verticalScrollBar->maximum());
    return QPoint(x, y);
}

QPoint FluentScrollArea::calculateScrollDelta(QWheelEvent* event) const {
    const QPoint angleDelta = event->angleDelta();
    const int deltaX = angleDelta.x() / 8 / 15 * m_scrollSpeed;
    const int deltaY = angleDelta.y() / 8 / 15 * m_scrollSpeed;

    return QPoint(-deltaX, -deltaY);
}

void FluentScrollArea::applyKineticScrolling(const QPoint& velocity) {
    // Simple kinetic scrolling implementation
    m_lastScrollVelocity = velocity;

    if (!m_kineticTimer) {
        m_kineticTimer = new QTimer(this);
        connect(m_kineticTimer, &QTimer::timeout, [this]() {
            if (m_lastScrollVelocity.manhattanLength() < 1) {
                m_kineticTimer->stop();
                return;
            }

            scrollBy(m_lastScrollVelocity, false);
            m_lastScrollVelocity *= 0.95; // Friction
        });
    }

    m_kineticTimer->start(16); // ~60 FPS
}

void FluentScrollArea::paintBackground(QPainter& painter, const QRect& rect) {
    const QColor backgroundColor = getBackgroundColor();
    painter.fillRect(rect, backgroundColor);
}

void FluentScrollArea::paintScrollIndicators(QPainter& painter) {
    // Paint scroll position indicators if needed
    Q_UNUSED(painter)
    // Implementation can be added for visual scroll position indicators
}

QColor FluentScrollArea::getBackgroundColor() const {
    const auto& theme = FluentQt::Styling::FluentTheme::instance();
    return theme.color("layerFillColorDefault");
}

void FluentScrollArea::updateContentGeometry() {
    if (!m_contentWidget) {
        return;
    }

    const QRect rect = this->rect();
    const int hScrollBarHeight = m_horizontalScrollBar->isVisible() ? m_scrollBarWidth : 0;
    const int vScrollBarWidth = m_verticalScrollBar->isVisible() ? m_scrollBarWidth : 0;

    QRect contentRect = rect.adjusted(0, 0, -vScrollBarWidth, -hScrollBarHeight);

    if (m_widgetResizable) {
        m_contentWidget->resize(contentRect.size());
    }

    const QPoint scrollPos = scrollPosition();
    m_contentWidget->move(-scrollPos);
}

void FluentScrollArea::updateScrollRanges() {
    if (!m_contentWidget) {
        return;
    }

    const QSize contentSize = m_contentWidget->size();
    const QSize viewportSize = size();
    const int hScrollBarHeight = m_horizontalScrollBar->isVisible() ? m_scrollBarWidth : 0;
    const int vScrollBarWidth = m_verticalScrollBar->isVisible() ? m_scrollBarWidth : 0;

    const QSize effectiveViewportSize = viewportSize - QSize(vScrollBarWidth, hScrollBarHeight);

    // Update horizontal scroll bar
    const int hMax = qMax(0, contentSize.width() - effectiveViewportSize.width());
    m_horizontalScrollBar->setRange(0, hMax);
    m_horizontalScrollBar->setPageStep(effectiveViewportSize.width());

    // Update vertical scroll bar
    const int vMax = qMax(0, contentSize.height() - effectiveViewportSize.height());
    m_verticalScrollBar->setRange(0, vMax);
    m_verticalScrollBar->setPageStep(effectiveViewportSize.height());
}

bool FluentScrollArea::needsHorizontalScrollBar() const {
    if (!m_contentWidget) {
        return false;
    }

    const QSize contentSize = m_contentWidget->size();
    const QSize viewportSize = size();

    return contentSize.width() > viewportSize.width();
}

bool FluentScrollArea::needsVerticalScrollBar() const {
    if (!m_contentWidget) {
        return false;
    }

    const QSize contentSize = m_contentWidget->size();
    const QSize viewportSize = size();

    return contentSize.height() > viewportSize.height();
}

} // namespace FluentQt::Components

#include "FluentScrollArea.moc"
