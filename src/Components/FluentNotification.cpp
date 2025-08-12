// src/Components/FluentNotification.cpp
#include "FluentQt/Components/FluentNotification.h"
#include <QApplication>
#include <QDebug>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QStyleOption>
#include <QVBoxLayout>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentNotification::FluentNotification(QWidget* parent)
    : Core::FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)),
      m_autoHideTimer(new QTimer(this)) {
    setupUI();
    setupAnimations();
    updateColors();

    setFixedWidth(360);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint);

    // Connect theme changes
    auto& theme = Styling::FluentTheme::instance();
    connect(&theme, &Styling::FluentTheme::themeChanged, this,
            &FluentNotification::updateColors);
}

FluentNotification::FluentNotification(FluentNotificationType type,
                                       const QString& title,
                                       const QString& message, QWidget* parent)
    : FluentNotification(parent) {
    setType(type);
    setTitle(title);
    setMessage(message);
}

FluentNotification::~FluentNotification() = default;

void FluentNotification::setupUI() {
    // Create opacity effect
    m_opacityEffect = new QGraphicsOpacityEffect(this);
    m_opacityEffect->setOpacity(1.0);
    setGraphicsEffect(m_opacityEffect);

    // Setup auto-hide timer
    m_autoHideTimer->setSingleShot(true);
    connect(m_autoHideTimer, &QTimer::timeout, this,
            &FluentNotification::onAutoHideTimeout);

    setMinimumHeight(64);
    setMaximumHeight(200);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
}

void FluentNotification::setupAnimations() {
    // Show animation
    m_showAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    m_showAnimation->setDuration(300);
    m_showAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_showAnimation, &QPropertyAnimation::finished, this,
            &FluentNotification::onShowAnimationFinished);

    // Hide animation
    m_hideAnimation = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    m_hideAnimation->setDuration(200);
    m_hideAnimation->setEasingCurve(QEasingCurve::InCubic);
    connect(m_hideAnimation, &QPropertyAnimation::finished, this,
            &FluentNotification::onHideAnimationFinished);
}

// Type and content
FluentNotificationType FluentNotification::type() const { return m_type; }

void FluentNotification::setType(FluentNotificationType type) {
    if (m_type != type) {
        m_type = type;
        updateTypeIcon();
        update();
        emit typeChanged(type);
    }
}

QString FluentNotification::title() const { return m_title; }

void FluentNotification::setTitle(const QString& title) {
    if (m_title != title) {
        m_title = title;
        m_layoutDirty = true;
        updateGeometry();
        update();
        emit titleChanged(title);
    }
}

QString FluentNotification::message() const { return m_message; }

void FluentNotification::setMessage(const QString& message) {
    if (m_message != message) {
        m_message = message;
        m_layoutDirty = true;
        updateGeometry();
        update();
        emit messageChanged(message);
    }
}

QIcon FluentNotification::icon() const { return m_customIcon; }

void FluentNotification::setIcon(const QIcon& icon) {
    if (m_customIcon.cacheKey() != icon.cacheKey()) {
        m_customIcon = icon;
        update();
        emit iconChanged(icon);
    }
}

// Behavior
int FluentNotification::duration() const { return m_duration; }

void FluentNotification::setDuration(int milliseconds) {
    if (m_duration != milliseconds) {
        m_duration = milliseconds;
        emit durationChanged(milliseconds);
    }
}

bool FluentNotification::isClosable() const { return m_closable; }

void FluentNotification::setClosable(bool closable) {
    if (m_closable != closable) {
        m_closable = closable;
        m_layoutDirty = true;
        update();
        emit closableChanged(closable);
    }
}

bool FluentNotification::isPersistent() const { return m_persistent; }

void FluentNotification::setPersistent(bool persistent) {
    if (m_persistent != persistent) {
        m_persistent = persistent;
        emit persistentChanged(persistent);
    }
}

// Actions
void FluentNotification::addAction(const FluentNotificationAction& action) {
    m_actions.append(action);
    updateActionButtons();
    m_layoutDirty = true;
    updateGeometry();
    update();
}

void FluentNotification::addAction(const QString& text,
                                   std::function<void()> callback) {
    addAction(FluentNotificationAction(text, callback));
}

void FluentNotification::addAction(const QString& text, const QIcon& icon,
                                   std::function<void()> callback) {
    addAction(FluentNotificationAction(text, icon, callback));
}

void FluentNotification::clearActions() {
    m_actions.clear();

    // Clean up action buttons
    for (QPushButton* button : m_actionButtons) {
        button->deleteLater();
    }
    m_actionButtons.clear();

    m_layoutDirty = true;
    updateGeometry();
    update();
}

// Animation and display
qreal FluentNotification::opacity() const { return m_currentOpacity; }

void FluentNotification::setOpacity(qreal opacity) {
    if (!qFuzzyCompare(m_currentOpacity, opacity)) {
        m_currentOpacity = opacity;
        if (m_opacityEffect) {
            m_opacityEffect->setOpacity(opacity);
        }
        emit opacityChanged(opacity);
    }
}

void FluentNotification::show() {
    QWidget::show();
    emit aboutToShow();

    if (!m_persistent && m_duration > 0) {
        startAutoHide();
    }

    emit shown();
}

void FluentNotification::hide() {
    emit aboutToHide();
    QWidget::hide();
    emit hidden();
}

void FluentNotification::close() {
    emit aboutToHide();
    stopAutoHide();
    QWidget::close();
    emit closed();
}

// Size hints
QSize FluentNotification::sizeHint() const {
    QFontMetrics titleFm(font());
    QFontMetrics messageFm(font());

    int width = 360;
    int height = 16;  // Top padding

    // Icon space
    if (!getTypeIcon().isNull() || !m_customIcon.isNull()) {
        height = qMax(height, 48 + 16);  // Icon height + padding
    }

    // Title height
    if (!m_title.isEmpty()) {
        height += titleFm.height() + 4;
    }

    // Message height
    if (!m_message.isEmpty()) {
        QRect textRect(0, 0, width - 80, 0);  // Account for icon and padding
        QRect boundingRect =
            messageFm.boundingRect(textRect, Qt::TextWordWrap, m_message);
        height += boundingRect.height() + 4;
    }

    // Actions height
    if (!m_actions.isEmpty()) {
        height += 32 + 8;  // Button height + spacing
    }

    height += 16;  // Bottom padding

    return QSize(width, qMax(height, 64));
}

QSize FluentNotification::minimumSizeHint() const { return QSize(300, 64); }

// Public slots
void FluentNotification::showAnimated() {
    if (m_showing || isVisible())
        return;

    m_showing = true;
    m_hiding = false;

    setOpacity(0.0);
    show();

    m_showAnimation->setStartValue(0.0);
    m_showAnimation->setEndValue(1.0);
    m_showAnimation->start();
}

void FluentNotification::hideAnimated() {
    if (m_hiding || !isVisible())
        return;

    m_hiding = true;
    m_showing = false;

    stopAutoHide();

    m_hideAnimation->setStartValue(opacity());
    m_hideAnimation->setEndValue(0.0);
    m_hideAnimation->start();
}

void FluentNotification::startAutoHide() {
    if (!m_persistent && m_duration > 0) {
        m_autoHideTimer->start(m_duration);
    }
}

void FluentNotification::stopAutoHide() { m_autoHideTimer->stop(); }

// Protected methods - Event handling
void FluentNotification::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_layoutDirty) {
        updateLayout();
    }

    paintShadow(&painter);
    paintBackground(&painter);
    paintBorder(&painter);

    // Paint icon
    QIcon currentIcon = m_customIcon.isNull() ? getTypeIcon() : m_customIcon;
    if (!currentIcon.isNull()) {
        QRect iconRect = getIconRect();
        currentIcon.paint(&painter, iconRect, Qt::AlignCenter);
    }

    // Paint title
    if (!m_title.isEmpty()) {
        auto& theme = Styling::FluentTheme::instance();
        painter.setPen(theme.color("textFillColorPrimary"));
        QFont titleFont = theme.subtitleFont();
        painter.setFont(titleFont);

        QRect titleRect = getTextRect();
        titleRect.setHeight(painter.fontMetrics().height());
        painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignTop, m_title);
    }

    // Paint message
    if (!m_message.isEmpty()) {
        auto& theme = Styling::FluentTheme::instance();
        painter.setPen(theme.color("textFillColorSecondary"));
        painter.setFont(theme.bodyFont());

        QRect messageRect = getTextRect();
        if (!m_title.isEmpty()) {
            messageRect.setTop(messageRect.top() +
                               painter.fontMetrics().height() + 4);
        }

        painter.drawText(messageRect,
                         Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop,
                         m_message);
    }

    // Paint close button
    if (m_closable) {
        QRect closeRect = getCloseButtonRect();
        auto& theme = Styling::FluentTheme::instance();

        QColor closeColor = theme.color("textFillColorSecondary");
        if (m_hovered) {
            closeColor = theme.color("textFillColorPrimary");
        }

        painter.setPen(QPen(closeColor, 2));
        painter.setBrush(Qt::NoBrush);

        // Draw X
        int margin = 4;
        painter.drawLine(closeRect.left() + margin, closeRect.top() + margin,
                         closeRect.right() - margin,
                         closeRect.bottom() - margin);
        painter.drawLine(closeRect.right() - margin, closeRect.top() + margin,
                         closeRect.left() + margin,
                         closeRect.bottom() - margin);
    }
}

void FluentNotification::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_closable && getCloseButtonRect().contains(event->pos())) {
            onCloseButtonClicked();
            event->accept();
            return;
        }
    }
    Core::FluentComponent::mousePressEvent(event);
}

void FluentNotification::enterEvent(QEnterEvent* event) {
    m_hovered = true;
    stopAutoHide();  // Pause auto-hide when hovered
    update();
    Core::FluentComponent::enterEvent(event);
}

void FluentNotification::leaveEvent(QEvent* event) {
    m_hovered = false;
    if (!m_persistent && m_duration > 0) {
        startAutoHide();  // Resume auto-hide when not hovered
    }
    update();
    Core::FluentComponent::leaveEvent(event);
}

void FluentNotification::resizeEvent(QResizeEvent* event) {
    m_layoutDirty = true;
    Core::FluentComponent::resizeEvent(event);
}

// Private slots
void FluentNotification::onCloseButtonClicked() { close(); }

void FluentNotification::onActionButtonClicked() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button)
        return;

    int index = m_actionButtons.indexOf(button);
    if (index >= 0 && index < m_actions.size()) {
        const auto& action = m_actions[index];
        if (action.callback) {
            action.callback();
        }
        emit actionTriggered(action.text);
    }
}

void FluentNotification::onAutoHideTimeout() { hideAnimated(); }

void FluentNotification::onShowAnimationFinished() {
    m_showing = false;
    setOpacity(1.0);
}

void FluentNotification::onHideAnimationFinished() {
    m_hiding = false;
    hide();
    close();
}

void FluentNotification::updateColors() { update(); }

// Private utility methods
void FluentNotification::updateLayout() {
    QRect contentRect = rect().adjusted(16, 16, -16, -16);

    // Icon area
    if (!getTypeIcon().isNull() || !m_customIcon.isNull()) {
        m_iconRect = QRect(contentRect.left(), contentRect.top(), 32, 32);
        m_textRect = QRect(m_iconRect.right() + 12, contentRect.top(),
                           contentRect.width() - 44 - (m_closable ? 24 : 0),
                           contentRect.height());
    } else {
        m_iconRect = QRect();
        m_textRect = QRect(contentRect.left(), contentRect.top(),
                           contentRect.width() - (m_closable ? 24 : 0),
                           contentRect.height());
    }

    // Close button
    if (m_closable) {
        m_closeButtonRect =
            QRect(contentRect.right() - 16, contentRect.top(), 16, 16);
    } else {
        m_closeButtonRect = QRect();
    }

    // Actions area
    if (!m_actions.isEmpty()) {
        int actionsHeight = 32;
        m_actionsRect =
            QRect(m_textRect.left(), contentRect.bottom() - actionsHeight,
                  m_textRect.width(), actionsHeight);
        m_textRect.setBottom(m_actionsRect.top() - 8);
    } else {
        m_actionsRect = QRect();
    }

    m_layoutDirty = false;
}

void FluentNotification::updateTypeIcon() { update(); }

void FluentNotification::updateActionButtons() {
    // Clean up existing buttons
    for (QPushButton* button : m_actionButtons) {
        button->deleteLater();
    }
    m_actionButtons.clear();

    // Create new buttons
    for (const auto& action : m_actions) {
        QPushButton* button = new QPushButton(action.text, this);
        if (!action.icon.isNull()) {
            button->setIcon(action.icon);
        }

        button->setFixedHeight(28);
        connect(button, &QPushButton::clicked, this,
                &FluentNotification::onActionButtonClicked);

        m_actionButtons.append(button);
    }
}

// Painting methods
void FluentNotification::paintBackground(QPainter* painter) {
    auto& theme = Styling::FluentTheme::instance();
    QColor backgroundColor = theme.color("cardBackgroundFillColorDefault");

    painter->setBrush(backgroundColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect(), 8, 8);
}

void FluentNotification::paintBorder(QPainter* painter) {
    QColor borderColor = getTypeColor();

    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(borderColor, 2));
    painter->drawRoundedRect(rect().adjusted(1, 1, -1, -1), 8, 8);
}

void FluentNotification::paintShadow(QPainter* painter) {
    auto& theme = Styling::FluentTheme::instance();
    QColor shadowColor = theme.color("shadow");
    shadowColor.setAlpha(60);

    painter->setBrush(shadowColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect().adjusted(2, 2, 2, 2), 8, 8);
}

// Utility methods
QColor FluentNotification::getTypeColor() const {
    auto& theme = Styling::FluentTheme::instance();

    switch (m_type) {
        case FluentNotificationType::Success:
            return theme.color("systemFillColorSuccess");
        case FluentNotificationType::Warning:
            return theme.color("systemFillColorCaution");
        case FluentNotificationType::Error:
            return theme.color("systemFillColorCritical");
        case FluentNotificationType::Info:
        default:
            return theme.color("accent");
    }
}

QIcon FluentNotification::getTypeIcon() const {
    // Return appropriate icon based on type
    // This would typically load from resources or use system icons
    switch (m_type) {
        case FluentNotificationType::Success:
            return QIcon(":/icons/check-circle.svg");
        case FluentNotificationType::Warning:
            return QIcon(":/icons/warning-triangle.svg");
        case FluentNotificationType::Error:
            return QIcon(":/icons/x-circle.svg");
        case FluentNotificationType::Info:
        default:
            return QIcon(":/icons/info-circle.svg");
    }
}

QRect FluentNotification::getContentRect() const {
    return rect().adjusted(16, 16, -16, -16);
}

QRect FluentNotification::getIconRect() const { return m_iconRect; }

QRect FluentNotification::getTextRect() const { return m_textRect; }

QRect FluentNotification::getActionsRect() const { return m_actionsRect; }

QRect FluentNotification::getCloseButtonRect() const {
    return m_closeButtonRect;
}

// FluentNotificationManager implementation
FluentNotificationManager& FluentNotificationManager::instance() {
    static FluentNotificationManager instance;
    return instance;
}

FluentNotificationManager::FluentNotificationManager(QObject* parent)
    : QObject(parent), m_layoutTimer(new QTimer(this)) {
    m_layoutTimer->setSingleShot(true);
    m_layoutTimer->setInterval(50);
    connect(m_layoutTimer, &QTimer::timeout, this,
            &FluentNotificationManager::updateLayout);
}

FluentNotificationManager::~FluentNotificationManager() { clear(); }

// Configuration
FluentNotificationPosition FluentNotificationManager::position() const {
    return m_position;
}

void FluentNotificationManager::setPosition(
    FluentNotificationPosition position) {
    if (m_position != position) {
        m_position = position;
        positionNotifications();
        emit positionChanged(position);
    }
}

FluentNotificationAnimation FluentNotificationManager::animation() const {
    return m_animation;
}

void FluentNotificationManager::setAnimation(
    FluentNotificationAnimation animation) {
    if (m_animation != animation) {
        m_animation = animation;
        emit animationChanged(animation);
    }
}

int FluentNotificationManager::maxVisible() const { return m_maxVisible; }

void FluentNotificationManager::setMaxVisible(int count) {
    if (m_maxVisible != count) {
        m_maxVisible = qMax(1, count);
        processQueue();
        emit maxVisibleChanged(m_maxVisible);
    }
}

int FluentNotificationManager::spacing() const { return m_spacing; }

void FluentNotificationManager::setSpacing(int spacing) {
    if (m_spacing != spacing) {
        m_spacing = spacing;
        positionNotifications();
        emit spacingChanged(spacing);
    }
}

QMargins FluentNotificationManager::margins() const { return m_margins; }

void FluentNotificationManager::setMargins(const QMargins& margins) {
    if (m_margins != margins) {
        m_margins = margins;
        positionNotifications();
        emit marginsChanged(margins);
    }
}

// Notification management
FluentNotification* FluentNotificationManager::show(FluentNotificationType type,
                                                    const QString& title,
                                                    const QString& message,
                                                    int duration) {
    auto* notification = new FluentNotification(type, title, message);
    notification->setDuration(duration);

    connect(notification, &FluentNotification::closed, this,
            &FluentNotificationManager::onNotificationClosed);

    if (m_visibleNotifications.size() < m_maxVisible) {
        m_visibleNotifications.append(notification);
        animateNotification(notification, true);
        positionNotifications();
        emit notificationShown(notification);
    } else {
        m_queuedNotifications.enqueue(notification);
    }

    return notification;
}

FluentNotification* FluentNotificationManager::showInfo(const QString& title,
                                                        const QString& message,
                                                        int duration) {
    return show(FluentNotificationType::Info, title, message, duration);
}

FluentNotification* FluentNotificationManager::showSuccess(
    const QString& title, const QString& message, int duration) {
    return show(FluentNotificationType::Success, title, message, duration);
}

FluentNotification* FluentNotificationManager::showWarning(
    const QString& title, const QString& message, int duration) {
    return show(FluentNotificationType::Warning, title, message, duration);
}

FluentNotification* FluentNotificationManager::showError(const QString& title,
                                                         const QString& message,
                                                         int duration) {
    return show(FluentNotificationType::Error, title, message, duration);
}

void FluentNotificationManager::hide(FluentNotification* notification) {
    if (!notification)
        return;

    notification->hideAnimated();
}

void FluentNotificationManager::hideAll() {
    for (auto* notification : m_visibleNotifications) {
        notification->hideAnimated();
    }
}

void FluentNotificationManager::clear() {
    hideAll();

    // Clear queued notifications
    while (!m_queuedNotifications.isEmpty()) {
        auto* notification = m_queuedNotifications.dequeue();
        notification->deleteLater();
    }
}

// Queue management
int FluentNotificationManager::visibleCount() const {
    return m_visibleNotifications.size();
}

int FluentNotificationManager::queuedCount() const {
    return m_queuedNotifications.size();
}

QList<FluentNotification*> FluentNotificationManager::visibleNotifications()
    const {
    return m_visibleNotifications;
}

// Public slots
void FluentNotificationManager::updateLayout() { positionNotifications(); }

// Private slots
void FluentNotificationManager::onNotificationClosed() {
    auto* notification = qobject_cast<FluentNotification*>(sender());
    if (!notification)
        return;

    m_visibleNotifications.removeAll(notification);
    emit notificationHidden(notification);

    // Process queue
    processQueue();
    positionNotifications();
}

void FluentNotificationManager::processQueue() {
    while (m_visibleNotifications.size() < m_maxVisible &&
           !m_queuedNotifications.isEmpty()) {
        auto* notification = m_queuedNotifications.dequeue();
        m_visibleNotifications.append(notification);
        animateNotification(notification, true);
        emit notificationShown(notification);
    }
}

// Private methods
void FluentNotificationManager::positionNotifications() {
    for (int i = 0; i < m_visibleNotifications.size(); ++i) {
        auto* notification = m_visibleNotifications[i];
        QPoint position = calculateNotificationPosition(i);
        notification->move(position);
    }
}

void FluentNotificationManager::animateNotification(
    FluentNotification* notification, bool show) {
    if (!notification)
        return;

    if (show) {
        notification->showAnimated();
    } else {
        notification->hideAnimated();
    }
}

QPoint FluentNotificationManager::calculateNotificationPosition(
    int index) const {
    QWidget* parentWidget = getParentWidget();
    if (!parentWidget) {
        return QPoint(100, 100 + index * 80);
    }

    QRect parentRect = parentWidget->geometry();
    QSize notificationSize(360, 80);  // Default notification size

    int x, y;

    // Calculate X position
    switch (m_position) {
        case FluentNotificationPosition::TopLeft:
        case FluentNotificationPosition::BottomLeft:
            x = parentRect.left() + m_margins.left();
            break;
        case FluentNotificationPosition::TopCenter:
        case FluentNotificationPosition::BottomCenter:
        case FluentNotificationPosition::Center:
            x = parentRect.center().x() - notificationSize.width() / 2;
            break;
        case FluentNotificationPosition::TopRight:
        case FluentNotificationPosition::BottomRight:
        default:
            x = parentRect.right() - notificationSize.width() -
                m_margins.right();
            break;
    }

    // Calculate Y position
    switch (m_position) {
        case FluentNotificationPosition::TopLeft:
        case FluentNotificationPosition::TopCenter:
        case FluentNotificationPosition::TopRight:
            y = parentRect.top() + m_margins.top() +
                index * (notificationSize.height() + m_spacing);
            break;
        case FluentNotificationPosition::Center:
            y = parentRect.center().y() -
                (m_visibleNotifications.size() *
                 (notificationSize.height() + m_spacing)) /
                    2 +
                index * (notificationSize.height() + m_spacing);
            break;
        case FluentNotificationPosition::BottomLeft:
        case FluentNotificationPosition::BottomCenter:
        case FluentNotificationPosition::BottomRight:
        default:
            y = parentRect.bottom() - m_margins.bottom() -
                notificationSize.height() -
                index * (notificationSize.height() + m_spacing);
            break;
    }

    return QPoint(x, y);
}

QWidget* FluentNotificationManager::getParentWidget() const {
    // Get the active window or main application window
    QWidget* activeWindow = QApplication::activeWindow();
    if (activeWindow) {
        return activeWindow;
    }

    // Fallback to primary screen
    QScreen* primaryScreen = QApplication::primaryScreen();
    if (primaryScreen) {
        // Return a virtual widget representing the screen
        static QWidget screenWidget;
        screenWidget.setGeometry(primaryScreen->availableGeometry());
        return &screenWidget;
    }

    return nullptr;
}

}  // namespace FluentQt::Components
