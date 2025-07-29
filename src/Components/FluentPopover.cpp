// src/Components/FluentPopover.cpp
#include "FluentQt/Components/FluentPopover.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QAccessible>
#include <QDesktopWidget>
#include <QScreen>
#include <QtMath>
#include <algorithm>

namespace FluentQt::Components {

FluentPopover::FluentPopover(QWidget* parent)
    : Core::FluentComponent(parent)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);
    setObjectName("FluentPopover");
    
    setupUI();
    setupAnimations();
    updateColors();
    updateFonts();
    
    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, &FluentPopover::onThemeChanged);
}

FluentPopover::FluentPopover(const QString& content, QWidget* parent)
    : FluentPopover(parent)
{
    setContent(content);
}

FluentPopover::FluentPopover(const QString& title, const QString& content, QWidget* parent)
    : FluentPopover(parent)
{
    setTitle(title);
    setContent(content);
}

FluentPopover::~FluentPopover() {
    removeTargetEventFilter();
}

void FluentPopover::setupUI() {
    FLUENT_PROFILE("FluentPopover::setupUI");
    
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(12, 8, 12, 8);
    m_mainLayout->setSpacing(8);
    
    // Create header layout
    m_headerLayout = new QHBoxLayout();
    m_headerLayout->setContentsMargins(0, 0, 0, 0);
    m_headerLayout->setSpacing(8);
    
    // Create icon label
    m_iconLabel = new QLabel(this);
    m_iconLabel->setObjectName("FluentPopover_Icon");
    m_iconLabel->setFixedSize(16, 16);
    m_iconLabel->hide(); // Initially hidden
    m_headerLayout->addWidget(m_iconLabel);
    
    // Create title label
    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("FluentPopover_Title");
    m_titleLabel->setWordWrap(true);
    m_titleLabel->hide(); // Initially hidden
    m_headerLayout->addWidget(m_titleLabel, 1);
    
    // Create close button
    m_closeButton = new QPushButton("×", this);
    m_closeButton->setObjectName("FluentPopover_CloseButton");
    m_closeButton->setFixedSize(20, 20);
    m_closeButton->setFlat(true);
    m_closeButton->hide(); // Initially hidden
    m_headerLayout->addWidget(m_closeButton);
    
    m_mainLayout->addLayout(m_headerLayout);
    
    // Create content label
    m_contentLabel = new QLabel(this);
    m_contentLabel->setObjectName("FluentPopover_Content");
    m_contentLabel->setWordWrap(true);
    m_contentLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_mainLayout->addWidget(m_contentLabel);
    
    // Connect signals
    connect(m_closeButton, &QPushButton::clicked, this, &FluentPopover::onCloseButtonClicked);
    
    // Set maximum width
    setMaximumWidth(m_maxWidth);
    
    // Initially hidden
    QWidget::hide();
}

void FluentPopover::setupAnimations() {
    FLUENT_PROFILE("FluentPopover::setupAnimations");
    
    // Create opacity effect
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityEffect->setOpacity(0.0);
    setGraphicsEffect(m_opacityEffect);
    
    // Create shadow effect
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    m_shadowEffect->setBlurRadius(20);
    m_shadowEffect->setOffset(0, 4);
    m_shadowEffect->setColor(QColor(0, 0, 0, 60));
    
    // Create show animation
    m_showAnimation = std::make_unique<QPropertyAnimation>(m_opacityEffect, "opacity");
    m_showAnimation->setDuration(200);
    m_showAnimation->setEasingCurve(QEasingCurve::OutQuad);
    m_showAnimation->setStartValue(0.0);
    m_showAnimation->setEndValue(1.0);
    
    // Create hide animation
    m_hideAnimation = std::make_unique<QPropertyAnimation>(m_opacityEffect, "opacity");
    m_hideAnimation->setDuration(150);
    m_hideAnimation->setEasingCurve(QEasingCurve::InQuad);
    m_hideAnimation->setStartValue(1.0);
    m_hideAnimation->setEndValue(0.0);
    
    // Create auto-hide timer
    m_autoHideTimer = new QTimer(this);
    m_autoHideTimer->setSingleShot(true);
    
    // Connect animations
    connect(m_showAnimation.get(), &QPropertyAnimation::finished,
            this, &FluentPopover::onShowAnimationFinished);
    connect(m_hideAnimation.get(), &QPropertyAnimation::finished,
            this, &FluentPopover::onHideAnimationFinished);
    connect(m_autoHideTimer, &QTimer::timeout,
            this, &FluentPopover::onAutoHideTimer);
}

QString FluentPopover::title() const {
    return m_title;
}

void FluentPopover::setTitle(const QString& title) {
    if (m_title != title) {
        m_title = title;
        m_titleLabel->setText(title);
        m_titleLabel->setVisible(!title.isEmpty());
        m_sizeHintValid = false;
        updateGeometry();
        emit titleChanged(title);
    }
}

QString FluentPopover::content() const {
    return m_content;
}

void FluentPopover::setContent(const QString& content) {
    if (m_content != content) {
        m_content = content;
        m_contentLabel->setText(content);
        m_sizeHintValid = false;
        updateGeometry();
        emit contentChanged(content);
    }
}

QIcon FluentPopover::icon() const {
    return m_icon;
}

void FluentPopover::setIcon(const QIcon& icon) {
    if (m_icon.cacheKey() != icon.cacheKey()) {
        m_icon = icon;
        
        if (!icon.isNull()) {
            m_iconLabel->setPixmap(icon.pixmap(16, 16));
            m_iconLabel->show();
        } else {
            m_iconLabel->hide();
        }
        
        m_sizeHintValid = false;
        updateGeometry();
        emit iconChanged(icon);
    }
}

FluentPopoverPlacement FluentPopover::placement() const {
    return m_placement;
}

void FluentPopover::setPlacement(FluentPopoverPlacement placement) {
    if (m_placement != placement) {
        m_placement = placement;
        if (m_isVisible) {
            updatePosition();
        }
        emit placementChanged(placement);
    }
}

FluentPopoverTrigger FluentPopover::trigger() const {
    return m_trigger;
}

void FluentPopover::setTrigger(FluentPopoverTrigger trigger) {
    if (m_trigger != trigger) {
        m_trigger = trigger;
        
        // Update event filter based on trigger
        if (m_target) {
            removeTargetEventFilter();
            installTargetEventFilter();
        }
        
        emit triggerChanged(trigger);
    }
}

bool FluentPopover::showArrow() const {
    return m_showArrow;
}

void FluentPopover::setShowArrow(bool show) {
    if (m_showArrow != show) {
        m_showArrow = show;
        update();
        emit showArrowChanged(show);
    }
}

bool FluentPopover::showCloseButton() const {
    return m_showCloseButton;
}

void FluentPopover::setShowCloseButton(bool show) {
    if (m_showCloseButton != show) {
        m_showCloseButton = show;
        m_closeButton->setVisible(show);
        m_sizeHintValid = false;
        updateGeometry();
        emit showCloseButtonChanged(show);
    }
}

int FluentPopover::autoHideDelay() const {
    return m_autoHideDelay;
}

void FluentPopover::setAutoHideDelay(int delay) {
    if (m_autoHideDelay != delay) {
        m_autoHideDelay = delay;
        emit autoHideDelayChanged(delay);
    }
}

int FluentPopover::maxWidth() const {
    return m_maxWidth;
}

void FluentPopover::setMaxWidth(int width) {
    if (m_maxWidth != width && width > 0) {
        m_maxWidth = width;
        setMaximumWidth(width);
        m_sizeHintValid = false;
        updateGeometry();
        emit maxWidthChanged(width);
    }
}

void FluentPopover::setContentWidget(QWidget* widget) {
    if (m_contentWidget) {
        m_mainLayout->removeWidget(m_contentWidget);
        m_contentWidget->setParent(nullptr);
    }
    
    m_contentWidget = widget;
    if (widget) {
        m_mainLayout->addWidget(widget);
        m_contentLabel->hide();
        m_sizeHintValid = false;
        updateGeometry();
    } else {
        m_contentLabel->show();
    }
}

QWidget* FluentPopover::contentWidget() const {
    return m_contentWidget;
}

void FluentPopover::setTarget(QWidget* target) {
    if (m_target != target) {
        removeTargetEventFilter();
        m_target = target;
        installTargetEventFilter();
    }
}

QWidget* FluentPopover::target() const {
    return m_target;
}

QSize FluentPopover::sizeHint() const {
    FLUENT_PROFILE("FluentPopover::sizeHint");
    
    if (m_sizeHintValid) {
        return m_cachedSizeHint;
    }
    
    // Calculate content size
    QSize contentSize;
    
    if (m_contentWidget) {
        contentSize = m_contentWidget->sizeHint();
    } else {
        const QFontMetrics fm(m_contentLabel->font());
        const int textWidth = std::min(m_maxWidth - 24, fm.horizontalAdvance(m_content)); // 24 for margins
        contentSize = fm.boundingRect(0, 0, textWidth, 0, Qt::TextWordWrap, m_content).size();
    }
    
    // Add header size if needed
    if (!m_title.isEmpty() || !m_icon.isNull() || m_showCloseButton) {
        contentSize.setHeight(contentSize.height() + 24); // Header height
    }
    
    // Add margins and arrow space
    contentSize += QSize(24, 16); // Margins
    if (m_showArrow) {
        contentSize += QSize(0, 8); // Arrow space
    }
    
    // Limit to maximum width
    contentSize.setWidth(std::min(contentSize.width(), m_maxWidth));
    
    m_cachedSizeHint = contentSize;
    m_sizeHintValid = true;
    
    return contentSize;
}

QSize FluentPopover::minimumSizeHint() const {
    return QSize(100, 50);
}

void FluentPopover::show() {
    if (!m_isVisible) {
        m_isVisible = true;
        emit aboutToShow();

        updatePosition();
        QWidget::show();
        startShowAnimation();

        // Start auto-hide timer if configured
        if (m_autoHideDelay > 0) {
            m_autoHideTimer->start(m_autoHideDelay);
        }

        emit visibilityChanged(true);
    }
}

void FluentPopover::hide() {
    if (m_isVisible) {
        m_isVisible = false;
        emit aboutToHide();

        m_autoHideTimer->stop();
        startHideAnimation();

        emit visibilityChanged(false);
    }
}

void FluentPopover::toggle() {
    if (m_isVisible) {
        hide();
    } else {
        show();
    }
}

void FluentPopover::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    FLUENT_PROFILE("FluentPopover::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const QRect rect = this->rect();

    // Paint background
    paintBackground(painter, rect);

    // Paint border
    paintBorder(painter, rect);

    // Paint arrow
    if (m_showArrow) {
        paintArrow(painter);
    }
}

void FluentPopover::paintBackground(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Create path with rounded corners
    QPainterPath path;
    const int radius = cornerRadius();
    path.addRoundedRect(rect, radius, radius);

    // Fill background
    painter.fillPath(path, palette.neutralLighter);
}

void FluentPopover::paintBorder(QPainter& painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    QPen borderPen(palette.neutralQuaternary, 1);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);

    const int radius = cornerRadius();
    const QRectF borderRect = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);

    painter.drawRoundedRect(borderRect, radius, radius);

    painter.restore();
}

void FluentPopover::paintArrow(QPainter& painter) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    painter.save();

    const int arrowSize = 8;
    QPainterPath arrowPath;

    // Calculate arrow position and direction based on placement
    switch (m_actualPlacement) {
        case FluentPopoverPlacement::Top:
        case FluentPopoverPlacement::TopStart:
        case FluentPopoverPlacement::TopEnd:
            // Arrow pointing down
            arrowPath.moveTo(m_arrowPosition.x() - arrowSize, height());
            arrowPath.lineTo(m_arrowPosition.x(), height() + arrowSize);
            arrowPath.lineTo(m_arrowPosition.x() + arrowSize, height());
            break;

        case FluentPopoverPlacement::Bottom:
        case FluentPopoverPlacement::BottomStart:
        case FluentPopoverPlacement::BottomEnd:
            // Arrow pointing up
            arrowPath.moveTo(m_arrowPosition.x() - arrowSize, 0);
            arrowPath.lineTo(m_arrowPosition.x(), -arrowSize);
            arrowPath.lineTo(m_arrowPosition.x() + arrowSize, 0);
            break;

        case FluentPopoverPlacement::Left:
        case FluentPopoverPlacement::LeftStart:
        case FluentPopoverPlacement::LeftEnd:
            // Arrow pointing right
            arrowPath.moveTo(width(), m_arrowPosition.y() - arrowSize);
            arrowPath.lineTo(width() + arrowSize, m_arrowPosition.y());
            arrowPath.lineTo(width(), m_arrowPosition.y() + arrowSize);
            break;

        case FluentPopoverPlacement::Right:
        case FluentPopoverPlacement::RightStart:
        case FluentPopoverPlacement::RightEnd:
            // Arrow pointing left
            arrowPath.moveTo(0, m_arrowPosition.y() - arrowSize);
            arrowPath.lineTo(-arrowSize, m_arrowPosition.y());
            arrowPath.lineTo(0, m_arrowPosition.y() + arrowSize);
            break;

        default:
            break;
    }

    // Fill arrow
    painter.fillPath(arrowPath, palette.neutralLighter);

    // Draw arrow border
    painter.setPen(QPen(palette.neutralQuaternary, 1));
    painter.drawPath(arrowPath);

    painter.restore();
}

void FluentPopover::mousePressEvent(QMouseEvent* event) {
    // Don't hide on click inside popover
    event->accept();
}

void FluentPopover::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        hide();
        event->accept();
        return;
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentPopover::focusOutEvent(QFocusEvent* event) {
    // Hide popover when losing focus (unless it's due to clicking inside)
    if (event->reason() != Qt::PopupFocusReason) {
        hide();
    }

    Core::FluentComponent::focusOutEvent(event);
}

void FluentPopover::showEvent(QShowEvent* event) {
    Core::FluentComponent::showEvent(event);
    setFocus();
}

void FluentPopover::hideEvent(QHideEvent* event) {
    Core::FluentComponent::hideEvent(event);
    m_isVisible = false;
}

bool FluentPopover::eventFilter(QObject* object, QEvent* event) {
    if (object == m_target && m_target) {
        switch (event->type()) {
            case QEvent::MouseButtonPress:
                if (m_trigger == FluentPopoverTrigger::Click) {
                    toggle();
                    return true;
                }
                break;

            case QEvent::Enter:
                if (m_trigger == FluentPopoverTrigger::Hover) {
                    show();
                }
                break;

            case QEvent::Leave:
                if (m_trigger == FluentPopoverTrigger::Hover) {
                    hide();
                }
                break;

            case QEvent::FocusIn:
                if (m_trigger == FluentPopoverTrigger::Focus) {
                    show();
                }
                break;

            case QEvent::FocusOut:
                if (m_trigger == FluentPopoverTrigger::Focus) {
                    hide();
                }
                break;

            default:
                break;
        }
    }

    return Core::FluentComponent::eventFilter(object, event);
}

void FluentPopover::updateStateStyle() {
    update();
}

void FluentPopover::performStateTransition(Core::FluentState from, Core::FluentState to) {
    Q_UNUSED(from)
    Q_UNUSED(to)
    update();
}

void FluentPopover::onCloseButtonClicked() {
    hide();
}

void FluentPopover::onAutoHideTimer() {
    hide();
}

void FluentPopover::onThemeChanged() {
    updateColors();
    updateFonts();
    update();
}

void FluentPopover::onShowAnimationFinished() {
    emit shown();
}

void FluentPopover::onHideAnimationFinished() {
    QWidget::hide();
    emit hidden();
}

void FluentPopover::updateColors() {
    // Colors are handled in paintEvent
}

void FluentPopover::updateFonts() {
    const auto& theme = Styling::FluentTheme::instance();
    m_titleLabel->setFont(theme.subtitleFont());
    m_contentLabel->setFont(theme.bodyFont());
}

void FluentPopover::startShowAnimation() {
    if (!isAnimated()) {
        m_opacityEffect->setOpacity(1.0);
        emit shown();
        return;
    }

    m_showAnimation->start();
}

void FluentPopover::startHideAnimation() {
    if (!isAnimated()) {
        m_opacityEffect->setOpacity(0.0);
        QWidget::hide();
        emit hidden();
        return;
    }

    m_hideAnimation->start();
}

void FluentPopover::updatePosition() {
    if (!m_target) return;

    // Determine optimal placement
    m_actualPlacement = (m_placement == FluentPopoverPlacement::Auto) ?
                       getOptimalPlacement() : m_placement;

    // Calculate position
    const QPoint position = calculatePosition(m_actualPlacement);
    move(position);

    // Update arrow position
    updateArrowPosition();
}

void FluentPopover::updateArrowPosition() {
    if (!m_target || !m_showArrow) return;

    const QRect targetRect = m_target->geometry();
    const QPoint targetCenter = m_target->mapToGlobal(targetRect.center());
    const QPoint popoverPos = pos();

    // Calculate arrow position relative to popover
    switch (m_actualPlacement) {
        case FluentPopoverPlacement::Top:
        case FluentPopoverPlacement::Bottom:
            m_arrowPosition.setX(targetCenter.x() - popoverPos.x());
            m_arrowPosition.setX(qBound(16, m_arrowPosition.x(), width() - 16));
            break;

        case FluentPopoverPlacement::Left:
        case FluentPopoverPlacement::Right:
            m_arrowPosition.setY(targetCenter.y() - popoverPos.y());
            m_arrowPosition.setY(qBound(16, m_arrowPosition.y(), height() - 16));
            break;

        default:
            break;
    }
}

QPoint FluentPopover::calculatePosition(FluentPopoverPlacement placement) const {
    if (!m_target) return QPoint();

    const QRect targetRect = m_target->geometry();
    const QPoint targetGlobal = m_target->mapToGlobal(QPoint(0, 0));
    const QSize popoverSize = sizeHint();
    const int arrowOffset = m_showArrow ? 8 : 0;

    QPoint position;

    switch (placement) {
        case FluentPopoverPlacement::Top:
            position = QPoint(
                targetGlobal.x() + (targetRect.width() - popoverSize.width()) / 2,
                targetGlobal.y() - popoverSize.height() - arrowOffset
            );
            break;

        case FluentPopoverPlacement::Bottom:
            position = QPoint(
                targetGlobal.x() + (targetRect.width() - popoverSize.width()) / 2,
                targetGlobal.y() + targetRect.height() + arrowOffset
            );
            break;

        case FluentPopoverPlacement::Left:
            position = QPoint(
                targetGlobal.x() - popoverSize.width() - arrowOffset,
                targetGlobal.y() + (targetRect.height() - popoverSize.height()) / 2
            );
            break;

        case FluentPopoverPlacement::Right:
            position = QPoint(
                targetGlobal.x() + targetRect.width() + arrowOffset,
                targetGlobal.y() + (targetRect.height() - popoverSize.height()) / 2
            );
            break;

        default:
            position = targetGlobal;
            break;
    }

    return position;
}

FluentPopoverPlacement FluentPopover::getOptimalPlacement() const {
    if (!m_target) return FluentPopoverPlacement::Top;

    const QRect targetRect = m_target->geometry();
    const QPoint targetGlobal = m_target->mapToGlobal(QPoint(0, 0));
    const QSize popoverSize = sizeHint();
    const QRect screenRect = QApplication::primaryScreen()->availableGeometry();

    // Check available space in each direction
    const int spaceTop = targetGlobal.y() - screenRect.top();
    const int spaceBottom = screenRect.bottom() - (targetGlobal.y() + targetRect.height());
    const int spaceLeft = targetGlobal.x() - screenRect.left();
    const int spaceRight = screenRect.right() - (targetGlobal.x() + targetRect.width());

    // Choose placement with most available space
    if (spaceBottom >= popoverSize.height()) {
        return FluentPopoverPlacement::Bottom;
    } else if (spaceTop >= popoverSize.height()) {
        return FluentPopoverPlacement::Top;
    } else if (spaceRight >= popoverSize.width()) {
        return FluentPopoverPlacement::Right;
    } else if (spaceLeft >= popoverSize.width()) {
        return FluentPopoverPlacement::Left;
    } else {
        return FluentPopoverPlacement::Bottom; // Default fallback
    }
}

void FluentPopover::installTargetEventFilter() {
    if (m_target && m_trigger != FluentPopoverTrigger::Manual) {
        m_target->installEventFilter(this);
    }
}

void FluentPopover::removeTargetEventFilter() {
    if (m_target) {
        m_target->removeEventFilter(this);
    }
}

} // namespace FluentQt::Components
