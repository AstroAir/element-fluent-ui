// src/Components/FluentTooltip.cpp
#include "FluentQt/Components/FluentTooltip.h"
#include <QAccessible>
#include <QApplication>
#include <QEnterEvent>
#include <QGraphicsOpacityEffect>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>
#include "FluentQt/Styling/FluentDesignTokenUtils.h"

namespace FluentQt::Components {

// Constants for tooltip appearance
static constexpr int ARROW_SIZE = 8;
static constexpr int BORDER_RADIUS = 4;

// Static instance for convenience methods
FluentTooltip* FluentTooltip::s_instance = nullptr;

FluentTooltip::FluentTooltip(QWidget* parent)
    : QWidget(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setupUI();
    setupAnimations();
    setupTimers();
    setupAccessibility();
    updateTheme();

    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFocusPolicy(Qt::StrongFocus);  // Enable keyboard focus for accessibility

    // Enable keyboard events
    setFocusPolicy(Qt::StrongFocus);
    installEventFilter(this);
}

FluentTooltip::FluentTooltip(const QString& text, QWidget* parent)
    : FluentTooltip(parent) {
    setText(text);
}

FluentTooltip::FluentTooltip(const QString& title, const QString& text,
                             QWidget* parent)
    : FluentTooltip(parent) {
    setTitle(title);
    setText(text);
}

FluentTooltip::~FluentTooltip() = default;

QString FluentTooltip::text() const { return m_text; }

void FluentTooltip::setText(const QString& text) {
    if (m_text != text) {
        m_text = text;
        updateContent();
        emit textChanged(text);
    }
}

QString FluentTooltip::title() const { return m_title; }

void FluentTooltip::setTitle(const QString& title) {
    if (m_title != title) {
        m_title = title;
        updateContent();
        emit titleChanged(title);
    }
}

FluentTooltipPosition FluentTooltip::position() const { return m_position; }

void FluentTooltip::setPosition(FluentTooltipPosition position) {
    if (m_position != position) {
        m_position = position;
        emit positionChanged(position);
    }
}

FluentTooltipTheme FluentTooltip::theme() const { return m_tooltipTheme; }

void FluentTooltip::setTheme(FluentTooltipTheme theme) {
    if (m_tooltipTheme != theme) {
        m_tooltipTheme = theme;
        updateTheme();
        emit themeChanged(theme);
    }
}

int FluentTooltip::showDelay() const { return m_showDelay; }

void FluentTooltip::setShowDelay(int delay) {
    if (m_showDelay != delay) {
        m_showDelay = delay;
        emit showDelayChanged(delay);
    }
}

int FluentTooltip::hideDelay() const { return m_hideDelay; }

void FluentTooltip::setHideDelay(int delay) {
    if (m_hideDelay != delay) {
        m_hideDelay = delay;
        emit hideDelayChanged(delay);
    }
}

bool FluentTooltip::isPersistent() const { return m_persistent; }

void FluentTooltip::setPersistent(bool persistent) {
    if (m_persistent != persistent) {
        m_persistent = persistent;
        emit persistentChanged(persistent);
    }
}

QSize FluentTooltip::maxSize() const { return m_maxSize; }

void FluentTooltip::setMaxSize(const QSize& size) {
    if (m_maxSize != size) {
        m_maxSize = size;
        emit maxSizeChanged(size);
    }
}

void FluentTooltip::showTooltip(const QString& text, const QPoint& position,
                                QWidget* parent) {
    if (!s_instance) {
        s_instance = new FluentTooltip(parent);
    }
    s_instance->setText(text);
    s_instance->showAt(position);
}

void FluentTooltip::showTooltip(const QString& title, const QString& text,
                                const QPoint& position, QWidget* parent) {
    if (!s_instance) {
        s_instance = new FluentTooltip(parent);
    }
    s_instance->setTitle(title);
    s_instance->setText(text);
    s_instance->showAt(position);
}

void FluentTooltip::hideTooltip() {
    if (s_instance) {
        s_instance->hide();
    }
}

void FluentTooltip::showAt(const QPoint& position) {
    m_targetPosition = position;
    updatePosition();
    show();
}

void FluentTooltip::showFor(QWidget* widget, FluentTooltipPosition pos) {
    if (!widget)
        return;

    m_targetWidget = widget;
    FluentTooltipPosition usePos =
        (pos == FluentTooltipPosition::Auto) ? m_position : pos;

    // Calculate position relative to the widget
    QPoint globalPos = widget->mapToGlobal(QPoint(0, 0));
    QSize widgetSize = widget->size();

    // Adjust position based on widget center
    QPoint centerPos =
        globalPos + QPoint(widgetSize.width() / 2, widgetSize.height() / 2);

    m_targetPosition = calculatePosition(centerPos, widgetSize, usePos);
    move(m_targetPosition);
    show();
}

void FluentTooltip::show() {
    if (!m_isShowing && !isVisible()) {
        emit aboutToShow();
        m_isShowing = true;
        startShowAnimation();
        QWidget::show();
    }
}

void FluentTooltip::hide() {
    if (isVisible() && !m_isHiding) {
        emit aboutToHide();
        m_isHiding = true;
        startHideAnimation();
    }
}

QSize FluentTooltip::sizeHint() const {
    QSize hint = QWidget::sizeHint();
    if (hint.width() > m_maxSize.width()) {
        hint.setWidth(m_maxSize.width());
    }
    if (hint.height() > m_maxSize.height()) {
        hint.setHeight(m_maxSize.height());
    }
    return hint;
}

void FluentTooltip::setupUI() {
    using namespace FluentQt::Styling;

    // Use Fluent design tokens for spacing
    int padding = TokenUtils::spacing("m");  // 8px standard padding
    int spacing = TokenUtils::spacing("s");  // 4px internal spacing

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(padding, padding, padding, padding);
    m_layout->setSpacing(spacing);

    // Title label with Fluent typography
    m_titleLabel = new QLabel(this);
    m_titleLabel->setWordWrap(true);
    m_titleLabel->setFont(
        TokenUtils::bodyFont(true));  // Strong body font for title
    m_titleLabel->hide();
    m_layout->addWidget(m_titleLabel);

    // Text label with Fluent typography
    m_textLabel = new QLabel(this);
    m_textLabel->setWordWrap(true);
    m_textLabel->setFont(
        TokenUtils::bodyFont(false));  // Regular body font for text
    m_layout->addWidget(m_textLabel);
}

void FluentTooltip::setupAnimations() {
    using namespace FluentQt::Animation;

    m_opacityEffect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(m_opacityEffect);

    // Use FluentAnimator for proper Fluent Design animations
    // Tooltips use utility motion (100ms, subtle easing)
    FluentAnimationConfig showConfig;
    showConfig.duration =
        std::chrono::milliseconds(100);  // Utility motion duration
    showConfig.easing =
        FluentEasing::FluentSubtle;          // Subtle easing for tooltips
    showConfig.respectReducedMotion = true;  // Accessibility support

    FluentAnimationConfig hideConfig;
    hideConfig.duration =
        std::chrono::milliseconds(75);  // Faster hide for responsiveness
    hideConfig.easing = FluentEasing::FluentSubtle;
    hideConfig.respectReducedMotion = true;

    // Use FluentAnimator's public fadeIn/fadeOut methods
    auto showAnim = FluentAnimator::fadeIn(this, showConfig);
    auto hideAnim = FluentAnimator::fadeOut(this, hideConfig);

    // Take ownership of the animations
    m_showAnimation = showAnim.release();
    m_hideAnimation = hideAnim.release();

    // Set parent to manage memory
    m_showAnimation->setParent(this);
    m_hideAnimation->setParent(this);

    connect(m_showAnimation, &QPropertyAnimation::finished, this,
            &FluentTooltip::onAnimationFinished);
    connect(m_hideAnimation, &QPropertyAnimation::finished, this,
            &FluentTooltip::onAnimationFinished);
}

void FluentTooltip::setupTimers() {
    m_showTimer = new QTimer(this);
    m_showTimer->setSingleShot(true);
    connect(m_showTimer, &QTimer::timeout, this, &FluentTooltip::onShowTimer);

    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    connect(m_hideTimer, &QTimer::timeout, this, &FluentTooltip::onHideTimer);
}

void FluentTooltip::setupAccessibility() {
    // Set accessibility properties
    setAccessibleName("Tooltip");
    setAccessibleDescription("Contextual information tooltip");

    // Note: Qt6 handles tooltip accessibility role automatically for
    // Qt::ToolTip windows

    // Enable keyboard navigation
    setFocusPolicy(Qt::StrongFocus);
    setTabOrder(this, nullptr);  // Remove from tab order by default
}

void FluentTooltip::updateContent() {
    if (m_titleLabel) {
        m_titleLabel->setText(m_title);
        m_titleLabel->setVisible(!m_title.isEmpty());
    }

    if (m_textLabel) {
        m_textLabel->setText(m_text);
    }

    adjustSize();
}

void FluentTooltip::updatePosition() {
    if (m_targetPosition.isNull())
        return;

    QPoint pos = calculatePosition(m_targetPosition, QSize(), m_position);
    move(pos);
}

void FluentTooltip::updateTheme() {
    using namespace FluentQt::Styling;

    // Get theme-appropriate colors using Fluent design tokens
    QColor textColor;
    QColor backgroundColor;
    QColor borderColor;

    if (m_tooltipTheme == FluentTooltipTheme::Auto) {
        // Use semantic colors that adapt to system theme
        textColor = TokenUtils::color("color.text.primary");
        backgroundColor = TokenUtils::color("color.background.primary");
        borderColor = TokenUtils::color("color.border.primary");
    } else if (m_tooltipTheme == FluentTooltipTheme::Dark) {
        textColor = TokenUtils::neutralColor(0);         // White
        backgroundColor = TokenUtils::neutralColor(48);  // Dark gray
        borderColor = TokenUtils::neutralColor(40);      // Lighter dark gray
    } else {
        textColor = TokenUtils::neutralColor(54);       // Black
        backgroundColor = TokenUtils::neutralColor(0);  // White
        borderColor = TokenUtils::neutralColor(20);     // Light gray
    }

    // Apply colors to labels
    if (m_titleLabel) {
        m_titleLabel->setStyleSheet(
            QString("QLabel { color: %1; }").arg(textColor.name()));
    }
    if (m_textLabel) {
        m_textLabel->setStyleSheet(
            QString("QLabel { color: %1; }").arg(textColor.name()));
    }

    // Store colors for painting
    m_currentBackgroundColor = backgroundColor;
    m_currentBorderColor = borderColor;
    m_currentTextColor = textColor;

    update();
}

QPoint FluentTooltip::calculatePosition(const QPoint& targetPos,
                                        const QSize& targetSize,
                                        FluentTooltipPosition pos) {
    using namespace FluentQt::Styling;

    QSize tooltipSize = sizeHint();
    QPoint result = targetPos;

    // Use design token for arrow size
    int arrowSize = TokenUtils::spacing("m");  // 8px arrow size

    // Position relative to target size with complete implementation
    switch (pos) {
        case FluentTooltipPosition::Top:
            result.ry() -= tooltipSize.height() + arrowSize;
            result.rx() += (targetSize.width() - tooltipSize.width()) / 2;
            break;
        case FluentTooltipPosition::Bottom:
            result.ry() += targetSize.height() + arrowSize;
            result.rx() += (targetSize.width() - tooltipSize.width()) / 2;
            break;
        case FluentTooltipPosition::Left:
            result.rx() -= tooltipSize.width() + arrowSize;
            result.ry() += (targetSize.height() - tooltipSize.height()) / 2;
            break;
        case FluentTooltipPosition::Right:
            result.rx() += targetSize.width() + arrowSize;
            result.ry() += (targetSize.height() - tooltipSize.height()) / 2;
            break;
        case FluentTooltipPosition::TopLeft:
            result.ry() -= tooltipSize.height() + arrowSize;
            result.rx() -= tooltipSize.width() / 4;
            break;
        case FluentTooltipPosition::TopRight:
            result.ry() -= tooltipSize.height() + arrowSize;
            result.rx() += targetSize.width() - (tooltipSize.width() * 3 / 4);
            break;
        case FluentTooltipPosition::BottomLeft:
            result.ry() += targetSize.height() + arrowSize;
            result.rx() -= tooltipSize.width() / 4;
            break;
        case FluentTooltipPosition::BottomRight:
            result.ry() += targetSize.height() + arrowSize;
            result.rx() += targetSize.width() - (tooltipSize.width() * 3 / 4);
            break;
        default:
            // Auto positioning - use optimal position calculation
            calculateOptimalPosition(targetPos, targetSize);
            return calculatePosition(targetPos, targetSize, m_actualPosition);
    }

    return result;
}

void FluentTooltip::startShowAnimation() {
    if (m_showAnimation) {
        m_showAnimation->start();
    }
}

void FluentTooltip::startHideAnimation() {
    if (m_hideAnimation) {
        m_hideAnimation->start();
    }
}

void FluentTooltip::onShowTimer() { show(); }

void FluentTooltip::onHideTimer() { hide(); }

void FluentTooltip::updateColors() { updateTheme(); }

void FluentTooltip::onAnimationFinished() {
    if (sender() == m_hideAnimation) {
        QWidget::hide();
        m_isHiding = false;
    } else if (sender() == m_showAnimation) {
        m_isShowing = false;
    }
}

void FluentTooltip::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawTooltip(painter);
    QWidget::paintEvent(event);
}

void FluentTooltip::drawTooltip(QPainter& painter) {
    using namespace FluentQt::Styling;

    QRect rect = this->rect().adjusted(1, 1, -1, -1);

    // Use design tokens for border radius
    int borderRadius = TokenUtils::spacing("s");  // 4px border radius

    // Use the colors set by updateTheme()
    painter.setBrush(m_currentBackgroundColor);
    painter.setPen(QPen(m_currentBorderColor, 1));
    painter.drawRoundedRect(rect, borderRadius, borderRadius);

    // Draw arrow if needed
    drawArrow(painter, rect);
}

void FluentTooltip::drawArrow(QPainter& painter, const QRect& rect) {
    using namespace FluentQt::Styling;

    // Use design token for arrow size
    int arrowSize = TokenUtils::spacing("m");  // 8px arrow size

    painter.setBrush(m_currentBackgroundColor);
    painter.setPen(Qt::NoPen);

    // Draw arrow based on position with complete implementation
    QPolygon arrow;
    switch (m_actualPosition) {
        case FluentTooltipPosition::Top:
        case FluentTooltipPosition::TopLeft:
        case FluentTooltipPosition::TopRight:
            arrow << QPoint(rect.center().x(), rect.bottom())
                  << QPoint(rect.center().x() - arrowSize,
                            rect.bottom() + arrowSize)
                  << QPoint(rect.center().x() + arrowSize,
                            rect.bottom() + arrowSize);
            break;
        case FluentTooltipPosition::Bottom:
        case FluentTooltipPosition::BottomLeft:
        case FluentTooltipPosition::BottomRight:
            arrow << QPoint(rect.center().x(), rect.top())
                  << QPoint(rect.center().x() - arrowSize,
                            rect.top() - arrowSize)
                  << QPoint(rect.center().x() + arrowSize,
                            rect.top() - arrowSize);
            break;
        case FluentTooltipPosition::Left:
            arrow << QPoint(rect.right(), rect.center().y())
                  << QPoint(rect.right() + arrowSize,
                            rect.center().y() - arrowSize)
                  << QPoint(rect.right() + arrowSize,
                            rect.center().y() + arrowSize);
            break;
        case FluentTooltipPosition::Right:
            arrow << QPoint(rect.left(), rect.center().y())
                  << QPoint(rect.left() - arrowSize,
                            rect.center().y() - arrowSize)
                  << QPoint(rect.left() - arrowSize,
                            rect.center().y() + arrowSize);
            break;
        default:
            break;
    }

    if (!arrow.isEmpty()) {
        painter.drawPolygon(arrow);
    }
}

void FluentTooltip::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updatePosition();
}

void FluentTooltip::enterEvent(QEnterEvent* event) {
    QWidget::enterEvent(event);
    if (m_hideTimer->isActive()) {
        m_hideTimer->stop();
    }
}

void FluentTooltip::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
    if (!m_persistent) {
        m_hideTimer->start(m_hideDelay);
    }
}

void FluentTooltip::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
    if (!m_persistent) {
        hide();
    }
}

bool FluentTooltip::eventFilter(QObject* object, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            // Hide tooltip on Escape key for accessibility
            hide();
            return true;
        }
    }
    return QWidget::eventFilter(object, event);
}

void FluentTooltip::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        hide();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void FluentTooltip::calculateOptimalPosition(const QPoint& targetPos,
                                             const QSize& targetSize) {
    // Auto-calculate the best position based on screen boundaries
    QScreen* screen = QApplication::primaryScreen();
    if (!screen)
        return;

    QRect screenGeometry = screen->availableGeometry();
    QSize tooltipSize = sizeHint();

    // Try different positions and pick the one that fits best
    FluentTooltipPosition bestPos = FluentTooltipPosition::Top;

    // Check if tooltip fits above the target
    if (targetPos.y() - tooltipSize.height() - ARROW_SIZE >=
        screenGeometry.top()) {
        bestPos = FluentTooltipPosition::Top;
    }
    // Check if tooltip fits below the target
    else if (targetPos.y() + targetSize.height() + tooltipSize.height() +
                 ARROW_SIZE <=
             screenGeometry.bottom()) {
        bestPos = FluentTooltipPosition::Bottom;
    }
    // Check if tooltip fits to the right
    else if (targetPos.x() + targetSize.width() + tooltipSize.width() +
                 ARROW_SIZE <=
             screenGeometry.right()) {
        bestPos = FluentTooltipPosition::Right;
    }
    // Default to left
    else {
        bestPos = FluentTooltipPosition::Left;
    }

    m_actualPosition = bestPos;
}

QRect FluentTooltip::getArrowRect() const {
    // Return the rectangle where the arrow should be drawn
    QRect rect = this->rect();
    switch (m_actualPosition) {
        case FluentTooltipPosition::Top:
            return QRect(rect.center().x() - ARROW_SIZE, rect.bottom(),
                         ARROW_SIZE * 2, ARROW_SIZE);
        case FluentTooltipPosition::Bottom:
            return QRect(rect.center().x() - ARROW_SIZE,
                         rect.top() - ARROW_SIZE, ARROW_SIZE * 2, ARROW_SIZE);
        case FluentTooltipPosition::Left:
            return QRect(rect.right(), rect.center().y() - ARROW_SIZE,
                         ARROW_SIZE, ARROW_SIZE * 2);
        case FluentTooltipPosition::Right:
            return QRect(rect.left() - ARROW_SIZE,
                         rect.center().y() - ARROW_SIZE, ARROW_SIZE,
                         ARROW_SIZE * 2);
        default:
            return QRect();
    }
}

QPainterPath FluentTooltip::createTooltipPath(const QRect& rect) const {
    QPainterPath path;
    path.addRoundedRect(rect, BORDER_RADIUS, BORDER_RADIUS);
    return path;
}

}  // namespace FluentQt::Components
