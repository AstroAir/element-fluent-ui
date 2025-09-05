// src/Components/FluentNotification.cpp
#include "FluentQt/Components/FluentNotification.h"
#include <QAccessible>
#include <QApplication>
#include <QContextMenuEvent>
#include <QDebug>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QStyleOption>
#include <QVBoxLayout>
#include "FluentQt/Styling/FluentDesignTokenUtils.h"
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

    // Create progress bar with FluentUI styling
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_progressBar->setRange(0, 100);
    m_progressBar->setTextVisible(false);

    // Apply FluentUI styling to progress bar
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();
    QString progressStyle =
        QString(
            "QProgressBar {"
            "    background-color: %1;"
            "    border: none;"
            "    border-radius: %2px;"
            "    text-align: center;"
            "}"
            "QProgressBar::chunk {"
            "    background-color: %3;"
            "    border-radius: %2px;"
            "}")
            .arg(
                tokenUtils
                    .getColor("color.progress.background",
                              QColor(243, 243, 243))
                    .name(),
                QString::number(tokenUtils.getBorderRadius("small")),
                tokenUtils.getColor("color.accent.primary", QColor(0, 120, 212))
                    .name());
    m_progressBar->setStyleSheet(progressStyle);

    connect(m_progressBar, QOverload<int>::of(&QProgressBar::valueChanged),
            this, &FluentNotification::onProgressChanged);

    // Create quick reply components
    m_quickReplyInput = new QLineEdit(this);
    m_quickReplyInput->setVisible(false);
    m_quickReplyInput->setPlaceholderText("Type a reply...");

    m_quickReplyButton = new QPushButton("Send", this);
    m_quickReplyButton->setVisible(false);
    connect(m_quickReplyButton, &QPushButton::clicked, this,
            &FluentNotification::onQuickReplySubmitted);
    connect(m_quickReplyInput, &QLineEdit::returnPressed, this,
            &FluentNotification::onQuickReplySubmitted);

    // Create context menu
    m_contextMenu = new QMenu(this);

    // Use FluentUI design tokens for sizing
    int minHeight = tokenUtils.getSize("notification.height.minimum", 64);
    int maxHeight = tokenUtils.getSize("notification.height.maximum", 200);

    setMinimumHeight(minHeight);
    setMaximumHeight(maxHeight);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    // Enable focus for keyboard navigation
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);

    // Set up accessibility attributes
    setupAccessibility();
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

void FluentNotification::setupAccessibility() {
    // Set accessible properties for the notification
    updateAccessibleProperties();

    // Enable accessibility for child components
    if (m_progressBar) {
        m_progressBar->setAccessibleName("Notification progress");
        m_progressBar->setAccessibleDescription(
            "Shows the progress of the current operation");
    }

    if (m_quickReplyInput) {
        m_quickReplyInput->setAccessibleName("Quick reply input");
        m_quickReplyInput->setAccessibleDescription(
            "Type a quick reply to this notification");
    }

    if (m_quickReplyButton) {
        m_quickReplyButton->setAccessibleName("Send reply");
        m_quickReplyButton->setAccessibleDescription(
            "Send the quick reply message");
    }
}

// Type and content
FluentNotificationType FluentNotification::type() const { return m_type; }

void FluentNotification::setType(FluentNotificationType type) {
    if (m_type != type) {
        m_type = type;
        updateTypeIcon();
        updateAccessibleProperties();
        update();
        emit typeChanged(type);
    }
}

FluentNotificationComplexity FluentNotification::complexity() const {
    return m_complexity;
}

void FluentNotification::setComplexity(
    FluentNotificationComplexity complexity) {
    if (m_complexity != complexity) {
        m_complexity = complexity;

        // Adjust behavior based on complexity mode
        if (complexity == FluentNotificationComplexity::Simple) {
            // Simple mode: disable advanced features
            setClosable(true);     // Simple mode always allows closing
            setPersistent(false);  // Simple mode doesn't support persistence
        }

        updateLayout();
        emit complexityChanged(complexity);
    }
}

QString FluentNotification::title() const { return m_title; }

void FluentNotification::setTitle(const QString& title) {
    if (m_title != title) {
        m_title = title;
        m_layoutDirty = true;
        updateGeometry();
        update();
        updateAccessibleProperties();
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
        updateAccessibleProperties();
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

// Elevation
FluentNotificationElevation FluentNotification::elevation() const {
    return m_elevation;
}

void FluentNotification::setElevation(FluentNotificationElevation elevation) {
    if (m_elevation != elevation) {
        m_elevation = elevation;
        update();  // Trigger repaint for shadow changes
        emit elevationChanged(elevation);
    }
}

// Actions
void FluentNotification::addAction(const FluentNotificationAction& action) {
    m_actions.append(action);
    updateActionButtons();
    updateAccessibleProperties();
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

    updateAccessibleProperties();
    m_layoutDirty = true;
    updateGeometry();
    update();
}

// Progress support
void FluentNotification::setProgress(int value) {
    if (m_progressValue != value) {
        m_progressValue = value;
        if (m_progressBar) {
            m_progressBar->setValue(value);
        }
        updateAccessibleProperties();
        emit progressChanged(value);
    }
}

void FluentNotification::setProgressRange(int minimum, int maximum) {
    if (m_progressMin != minimum || m_progressMax != maximum) {
        m_progressMin = minimum;
        m_progressMax = maximum;
        if (m_progressBar) {
            m_progressBar->setRange(minimum, maximum);
        }
    }
}

void FluentNotification::setProgressVisible(bool visible) {
    if (m_progressVisible != visible) {
        m_progressVisible = visible;
        if (m_progressBar) {
            m_progressBar->setVisible(visible);
        }
        updateAccessibleProperties();
        m_layoutDirty = true;
        updateGeometry();
        update();
    }
}

int FluentNotification::progress() const { return m_progressValue; }

bool FluentNotification::isProgressVisible() const { return m_progressVisible; }

// Quick reply support
void FluentNotification::setQuickReplyEnabled(bool enabled) {
    if (m_quickReplyEnabled != enabled) {
        m_quickReplyEnabled = enabled;
        if (m_quickReplyInput) {
            m_quickReplyInput->setVisible(enabled);
        }
        if (m_quickReplyButton) {
            m_quickReplyButton->setVisible(enabled);
        }
        updateAccessibleProperties();
        m_layoutDirty = true;
        updateGeometry();
        update();
    }
}

void FluentNotification::setQuickReplyPlaceholder(const QString& placeholder) {
    if (m_quickReplyPlaceholder != placeholder) {
        m_quickReplyPlaceholder = placeholder;
        if (m_quickReplyInput) {
            m_quickReplyInput->setPlaceholderText(placeholder);
        }
    }
}

bool FluentNotification::isQuickReplyEnabled() const {
    return m_quickReplyEnabled;
}

QString FluentNotification::quickReplyText() const {
    if (m_quickReplyInput) {
        return m_quickReplyInput->text();
    }
    return QString();
}

// Context menu support
void FluentNotification::setContextMenuEnabled(bool enabled) {
    m_contextMenuEnabled = enabled;
}

void FluentNotification::addContextMenuAction(const QString& text,
                                              std::function<void()> callback) {
    m_contextMenuActions.append(qMakePair(text, callback));

    if (m_contextMenu) {
        QAction* action = m_contextMenu->addAction(text);
        connect(action, &QAction::triggered, this,
                &FluentNotification::onContextMenuActionTriggered);
    }
}

void FluentNotification::clearContextMenuActions() {
    m_contextMenuActions.clear();
    if (m_contextMenu) {
        m_contextMenu->clear();
    }
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
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

    // Use FluentUI design tokens for sizing
    int width = tokenUtils.getSize("notification.width.default", 360);
    int contentPadding = tokenUtils.getSpacing("l");  // 16px
    int elementSpacing = tokenUtils.getSpacing("s");  // 8px
    int iconSize = tokenUtils.getSize("icon.size.medium", 32);

    int height = contentPadding;  // Top padding

    // Icon space
    if (!getTypeIcon().isNull() || !m_customIcon.isNull()) {
        height = qMax(height, iconSize + contentPadding);
    }

    // Title height
    if (!m_title.isEmpty()) {
        QFont titleFont = tokenUtils.getTitleFont(2);
        QFontMetrics titleFm(titleFont);
        height +=
            titleFm.height() + tokenUtils.getSpacing("xs");  // 4px spacing
    }

    // Message height
    if (!m_message.isEmpty()) {
        QFont messageFont = tokenUtils.getBodyFont(false);
        QFontMetrics messageFm(messageFont);

        int textWidth = width - (2 * contentPadding);
        if (!getTypeIcon().isNull() || !m_customIcon.isNull()) {
            textWidth -=
                iconSize + tokenUtils.getSpacing("m");  // 12px icon spacing
        }

        QRect textRect(0, 0, textWidth, 0);
        QRect boundingRect =
            messageFm.boundingRect(textRect, Qt::TextWordWrap, m_message);
        height +=
            boundingRect.height() + tokenUtils.getSpacing("xs");  // 4px spacing
    }

    // Progress bar height
    if (m_progressVisible) {
        height += tokenUtils.getSize("progress.height", 4) + elementSpacing;
    }

    // Quick reply height
    if (m_quickReplyEnabled) {
        height += tokenUtils.getSize("input.height.small", 28) + elementSpacing;
    }

    // Actions height
    if (!m_actions.isEmpty()) {
        height +=
            tokenUtils.getSize("button.height.small", 32) + elementSpacing;
    }

    height += contentPadding;  // Bottom padding

    int minHeight = tokenUtils.getSize("notification.height.minimum", 64);
    return QSize(width, qMax(height, minHeight));
}

QSize FluentNotification::minimumSizeHint() const {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();
    int minWidth = tokenUtils.getSize("notification.width.minimum", 300);
    int minHeight = tokenUtils.getSize("notification.height.minimum", 64);
    return QSize(minWidth, minHeight);
}

// Public slots
void FluentNotification::showAnimated() {
    if (m_showing || isVisible())
        return;

    m_showing = true;
    m_hiding = false;

    setOpacity(0.0);
    show();

    if (m_complexity == FluentNotificationComplexity::Simple) {
        // Simple mode: basic fade-in without complex animations
        m_showAnimation->setDuration(200);  // Faster animation
        m_showAnimation->setEasingCurve(QEasingCurve::OutQuad);
    } else {
        // Full mode: use FluentAnimator with proper motion hierarchy
        if (m_animator) {
            // Use FluentAnimator for enhanced entrance animation
            auto config = Animation::FluentAnimationConfig();
            config.duration = std::chrono::milliseconds(300);
            config.easing = Animation::FluentEasing::FluentStandard;
            config.hierarchy =
                Animation::FluentAnimationConfig::MotionHierarchy::Primary;

            // Create slide-in animation from the right
            auto slideAnimation =
                m_animator->slideIn(this, QPoint(50, 0), config);
            if (slideAnimation) {
                connect(slideAnimation.get(), &QPropertyAnimation::finished,
                        this, &FluentNotification::onShowAnimationFinished);
                slideAnimation->start();
                return;
            }
        }

        // Fallback to standard animation
        m_showAnimation->setDuration(300);
        m_showAnimation->setEasingCurve(QEasingCurve::OutCubic);
    }

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

    if (m_complexity == FluentNotificationComplexity::Simple) {
        // Simple mode: basic fade-out without complex animations
        m_hideAnimation->setDuration(150);  // Faster animation
        m_hideAnimation->setEasingCurve(QEasingCurve::InQuad);
    } else {
        // Full mode: use FluentAnimator with proper motion hierarchy
        if (m_animator) {
            // Use FluentAnimator for enhanced exit animation
            auto config = Animation::FluentAnimationConfig();
            config.duration = std::chrono::milliseconds(250);
            config.easing = Animation::FluentEasing::FluentDecelerate;
            config.hierarchy =
                Animation::FluentAnimationConfig::MotionHierarchy::Secondary;

            // Create slide-out animation to the right
            auto slideAnimation =
                m_animator->slideOut(this, QPoint(50, 0), config);
            if (slideAnimation) {
                connect(slideAnimation.get(), &QPropertyAnimation::finished,
                        this, &FluentNotification::onHideAnimationFinished);
                slideAnimation->start();
                return;
            }
        }

        // Fallback to standard animation
        m_hideAnimation->setDuration(250);
        m_hideAnimation->setEasingCurve(QEasingCurve::InCubic);
    }

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
        const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

        // Use FluentUI design tokens for text color and typography
        QColor titleColor =
            tokenUtils.getColor("color.text.primary", QColor(0, 0, 0));
        painter.setPen(titleColor);

        // Use proper FluentUI typography scale for title
        QFont titleFont = tokenUtils.getFont("typography.subtitle1",
                                             tokenUtils.getTitleFont(2));
        painter.setFont(titleFont);

        QRect titleRect = getTextRect();
        titleRect.setHeight(painter.fontMetrics().height());
        painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignTop, m_title);
    }

    // Paint message
    if (!m_message.isEmpty()) {
        const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

        // Use secondary text color for message
        QColor messageColor =
            tokenUtils.getColor("color.text.secondary", QColor(96, 96, 96));
        painter.setPen(messageColor);

        // Use body font for message text
        QFont messageFont = tokenUtils.getFont("typography.body1",
                                               tokenUtils.getBodyFont(false));
        painter.setFont(messageFont);

        QRect messageRect = getTextRect();
        if (!m_title.isEmpty()) {
            // Use design token for spacing between title and message
            int titleSpacing = tokenUtils.getSpacing("xs");  // 4px
            messageRect.setTop(messageRect.top() +
                               painter.fontMetrics().height() + titleSpacing);
        }

        painter.drawText(messageRect,
                         Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop,
                         m_message);
    }

    // Paint close button
    if (m_closable) {
        QRect closeRect = getCloseButtonRect();
        const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

        // Use design tokens for close button colors
        QColor closeColor =
            tokenUtils.getColor("color.text.secondary", QColor(96, 96, 96));
        if (m_hovered || m_focused) {
            closeColor =
                tokenUtils.getColor("color.text.primary", QColor(0, 0, 0));
        }

        // Use design token for line width
        int lineWidth = tokenUtils.getBorderWidth("thin");
        painter.setPen(QPen(closeColor, lineWidth));
        painter.setBrush(Qt::NoBrush);

        // Draw X with proper spacing using design tokens
        int margin = tokenUtils.getSpacing("xs");  // 4px
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

void FluentNotification::mouseReleaseEvent(QMouseEvent* event) {
    Core::FluentComponent::mouseReleaseEvent(event);
}

void FluentNotification::contextMenuEvent(QContextMenuEvent* event) {
    if (m_contextMenuEnabled && m_contextMenu &&
        !m_contextMenuActions.isEmpty()) {
        emit contextMenuRequested(event->pos());
        m_contextMenu->exec(event->globalPos());
        event->accept();
    } else {
        Core::FluentComponent::contextMenuEvent(event);
    }
}

void FluentNotification::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Escape:
            if (m_closable) {
                close();
                event->accept();
                return;
            }
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (m_quickReplyEnabled && m_quickReplyInput &&
                m_quickReplyInput->hasFocus()) {
                onQuickReplySubmitted();
                event->accept();
                return;
            }
            break;
        case Qt::Key_Tab:
            // Handle tab navigation between interactive elements
            if (m_quickReplyEnabled && m_quickReplyInput) {
                if (!m_quickReplyInput->hasFocus()) {
                    m_quickReplyInput->setFocus();
                    event->accept();
                    return;
                }
            }
            break;
    }
    Core::FluentComponent::keyPressEvent(event);
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

void FluentNotification::focusInEvent(QFocusEvent* event) {
    m_focused = true;
    stopAutoHide();  // Pause auto-hide when focused
    update();
    Core::FluentComponent::focusInEvent(event);
}

void FluentNotification::focusOutEvent(QFocusEvent* event) {
    m_focused = false;
    if (!m_persistent && m_duration > 0 && !m_hovered) {
        startAutoHide();  // Resume auto-hide when not focused and not hovered
    }
    update();
    Core::FluentComponent::focusOutEvent(event);
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

void FluentNotification::onQuickReplySubmitted() {
    if (m_quickReplyInput) {
        QString text = m_quickReplyInput->text();
        if (!text.isEmpty()) {
            emit quickReplySubmitted(text);
            m_quickReplyInput->clear();
        }
    }
}

void FluentNotification::onProgressChanged(int value) {
    m_progressValue = value;
    emit progressChanged(value);
}

void FluentNotification::onContextMenuActionTriggered() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    QString actionText = action->text();
    for (const auto& pair : m_contextMenuActions) {
        if (pair.first == actionText && pair.second) {
            pair.second();
            break;
        }
    }
}

void FluentNotification::updateColors() { update(); }

// Private utility methods
void FluentNotification::updateLayout() {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

    // Use FluentUI 4px base unit spacing system
    int contentPadding = tokenUtils.getSpacing("l");  // 16px
    int iconSize = tokenUtils.getSize("icon.size.medium", 32);
    int iconSpacing = tokenUtils.getSpacing("m");  // 12px
    int closeButtonSize = tokenUtils.getSize("button.close.size", 16);
    int actionButtonHeight = tokenUtils.getSize("button.height.small", 32);
    int elementSpacing = tokenUtils.getSpacing("s");  // 8px

    QRect contentRect = rect().adjusted(contentPadding, contentPadding,
                                        -contentPadding, -contentPadding);

    // Icon area
    if (!getTypeIcon().isNull() || !m_customIcon.isNull()) {
        m_iconRect =
            QRect(contentRect.left(), contentRect.top(), iconSize, iconSize);
        m_textRect =
            QRect(m_iconRect.right() + iconSpacing, contentRect.top(),
                  contentRect.width() - iconSize - iconSpacing -
                      (m_closable ? closeButtonSize + elementSpacing : 0),
                  contentRect.height());
    } else {
        m_iconRect = QRect();
        m_textRect =
            QRect(contentRect.left(), contentRect.top(),
                  contentRect.width() -
                      (m_closable ? closeButtonSize + elementSpacing : 0),
                  contentRect.height());
    }

    // Close button
    if (m_closable) {
        m_closeButtonRect =
            QRect(contentRect.right() - closeButtonSize, contentRect.top(),
                  closeButtonSize, closeButtonSize);
    } else {
        m_closeButtonRect = QRect();
    }

    // Progress bar area
    if (m_progressVisible) {
        int progressHeight = tokenUtils.getSize("progress.height", 4);
        m_progressRect =
            QRect(m_textRect.left(), contentRect.bottom() - progressHeight,
                  m_textRect.width(), progressHeight);
        contentRect.setBottom(m_progressRect.top() - elementSpacing);
    } else {
        m_progressRect = QRect();
    }

    // Quick reply area
    if (m_quickReplyEnabled) {
        int quickReplyHeight = tokenUtils.getSize("input.height.small", 28);
        m_quickReplyRect =
            QRect(m_textRect.left(), contentRect.bottom() - quickReplyHeight,
                  m_textRect.width(), quickReplyHeight);
        contentRect.setBottom(m_quickReplyRect.top() - elementSpacing);
    } else {
        m_quickReplyRect = QRect();
    }

    // Actions area
    if (!m_actions.isEmpty()) {
        m_actionsRect =
            QRect(m_textRect.left(), contentRect.bottom() - actionButtonHeight,
                  m_textRect.width(), actionButtonHeight);
        m_textRect.setBottom(m_actionsRect.top() - elementSpacing);

        // Position action buttons
        int buttonSpacing = tokenUtils.getSpacing("s");  // 8px between buttons
        int currentX = m_actionsRect.left();
        int buttonWidth = tokenUtils.getSize("button.width.small", 80);

        for (int i = 0; i < m_actionButtons.size(); ++i) {
            QPushButton* button = m_actionButtons[i];
            if (button) {
                button->setGeometry(currentX, m_actionsRect.top(), buttonWidth,
                                    actionButtonHeight);
                currentX += buttonWidth + buttonSpacing;
            }
        }
    } else {
        m_actionsRect = QRect();
    }

    // Update component positions
    if (m_progressBar && m_progressVisible) {
        m_progressBar->setGeometry(m_progressRect);
    }

    if (m_quickReplyInput && m_quickReplyEnabled) {
        int buttonWidth = tokenUtils.getSize("button.width.small", 60);
        QRect inputRect = m_quickReplyRect;
        inputRect.setWidth(inputRect.width() - buttonWidth - elementSpacing);
        m_quickReplyInput->setGeometry(inputRect);

        if (m_quickReplyButton) {
            QRect buttonRect = QRect(inputRect.right() + elementSpacing,
                                     m_quickReplyRect.top(), buttonWidth,
                                     m_quickReplyRect.height());
            m_quickReplyButton->setGeometry(buttonRect);
        }
    }

    m_layoutDirty = false;
}

void FluentNotification::updateAccessibleProperties() {
    // Build accessible name from title and type
    QString accessibleName = m_title;
    if (accessibleName.isEmpty()) {
        accessibleName = "Notification";
    }

    // Add type information
    QString typeText;
    switch (m_type) {
        case FluentNotificationType::Success:
            typeText = "Success";
            break;
        case FluentNotificationType::Warning:
            typeText = "Warning";
            break;
        case FluentNotificationType::Error:
            typeText = "Error";
            break;
        case FluentNotificationType::Info:
        default:
            typeText = "Information";
            break;
    }

    setAccessibleName(QString("%1 %2").arg(typeText, accessibleName));

    // Build accessible description
    QString description = m_message;
    if (m_progressVisible) {
        description += QString(" Progress: %1%").arg(m_progressValue);
    }
    if (m_quickReplyEnabled) {
        description += " Quick reply available";
    }
    if (!m_actions.isEmpty()) {
        description += QString(" %1 action(s) available").arg(m_actions.size());
    }

    setAccessibleDescription(description);
}

void FluentNotification::updateTypeIcon() { update(); }

void FluentNotification::updateActionButtons() {
    // Clean up existing buttons
    for (QPushButton* button : m_actionButtons) {
        button->deleteLater();
    }
    m_actionButtons.clear();

    if (m_actions.isEmpty()) {
        return;
    }

    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

    // Create new buttons with FluentUI styling
    for (int i = 0; i < m_actions.size(); ++i) {
        const auto& action = m_actions[i];
        QPushButton* button = new QPushButton(action.text, this);

        if (!action.icon.isNull()) {
            button->setIcon(action.icon);
        }

        // Apply FluentUI button styling using design tokens
        int buttonHeight = tokenUtils.getSize("button.height.small", 28);
        button->setFixedHeight(buttonHeight);

        // Use FluentUI typography for button text
        QFont buttonFont = tokenUtils.getFont("typography.body1",
                                              tokenUtils.getBodyFont(false));
        button->setFont(buttonFont);

        // Apply FluentUI button colors and styling
        QString buttonStyle =
            QString(
                "QPushButton {"
                "    background-color: %1;"
                "    color: %2;"
                "    border: %3px solid %4;"
                "    border-radius: %5px;"
                "    padding: %6px %7px;"
                "    font-weight: %8;"
                "}"
                "QPushButton:hover {"
                "    background-color: %9;"
                "    border-color: %10;"
                "}"
                "QPushButton:pressed {"
                "    background-color: %11;"
                "    border-color: %12;"
                "}"
                "QPushButton:focus {"
                "    outline: 2px solid %13;"
                "    outline-offset: 1px;"
                "}")
                .arg(
                    // Primary button for first action, secondary for others
                    action.primary || i == 0
                        ? tokenUtils
                              .getColor("color.button.primary.background",
                                        QColor(0, 120, 212))
                              .name()
                        : tokenUtils
                              .getColor("color.button.secondary.background",
                                        QColor(255, 255, 255))
                              .name(),
                    action.primary || i == 0
                        ? tokenUtils
                              .getColor("color.button.primary.text",
                                        QColor(255, 255, 255))
                              .name()
                        : tokenUtils
                              .getColor("color.button.secondary.text",
                                        QColor(0, 0, 0))
                              .name(),
                    QString::number(tokenUtils.getBorderWidth("thin")),
                    action.primary || i == 0
                        ? tokenUtils
                              .getColor("color.button.primary.border",
                                        QColor(0, 120, 212))
                              .name()
                        : tokenUtils
                              .getColor("color.button.secondary.border",
                                        QColor(96, 96, 96))
                              .name(),
                    QString::number(tokenUtils.getBorderRadius("small")),
                    QString::number(
                        tokenUtils.getSpacing("xs")),  // vertical padding
                    QString::number(
                        tokenUtils.getSpacing("m")),  // horizontal padding
                    action.primary || i == 0 ? "600" : "400",  // font weight
                    // Hover states
                    action.primary || i == 0
                        ? tokenUtils
                              .getColor("color.button.primary.background.hover",
                                        QColor(16, 110, 190))
                              .name()
                        : tokenUtils
                              .getColor(
                                  "color.button.secondary.background.hover",
                                  QColor(243, 243, 243))
                              .name(),
                    action.primary || i == 0
                        ? tokenUtils
                              .getColor("color.button.primary.border.hover",
                                        QColor(16, 110, 190))
                              .name()
                        : tokenUtils
                              .getColor("color.button.secondary.border.hover",
                                        QColor(96, 96, 96))
                              .name(),
                    // Pressed states
                    action.primary || i == 0
                        ? tokenUtils
                              .getColor(
                                  "color.button.primary.background.pressed",
                                  QColor(14, 102, 180))
                              .name()
                        : tokenUtils
                              .getColor(
                                  "color.button.secondary.background.pressed",
                                  QColor(237, 237, 237))
                              .name(),
                    action.primary || i == 0
                        ? tokenUtils
                              .getColor("color.button.primary.border.pressed",
                                        QColor(14, 102, 180))
                              .name()
                        : tokenUtils
                              .getColor("color.button.secondary.border.pressed",
                                        QColor(96, 96, 96))
                              .name(),
                    // Focus outline
                    tokenUtils
                        .getColor("color.focus.outline", QColor(0, 120, 212))
                        .name());

        button->setStyleSheet(buttonStyle);

        connect(button, &QPushButton::clicked, this,
                &FluentNotification::onActionButtonClicked);

        m_actionButtons.append(button);
    }

    // Position buttons in the layout
    updateLayout();
}

// Painting methods
void FluentNotification::paintBackground(QPainter* painter) {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

    // Use FluentUI design tokens for background color
    QColor backgroundColor =
        tokenUtils.getColor("color.surface.primary", QColor(255, 255, 255));

    // Apply type-specific background tinting
    switch (m_type) {
        case FluentNotificationType::Success:
            backgroundColor =
                tokenUtils.getColor("color.surface.success", backgroundColor);
            break;
        case FluentNotificationType::Warning:
            backgroundColor =
                tokenUtils.getColor("color.surface.warning", backgroundColor);
            break;
        case FluentNotificationType::Error:
            backgroundColor =
                tokenUtils.getColor("color.surface.critical", backgroundColor);
            break;
        case FluentNotificationType::Info:
        default:
            backgroundColor =
                tokenUtils.getColor("color.surface.info", backgroundColor);
            break;
    }

    painter->setBrush(backgroundColor);
    painter->setPen(Qt::NoPen);

    // Use design token for border radius
    int borderRadius = tokenUtils.getBorderRadius("medium");
    painter->drawRoundedRect(rect(), borderRadius, borderRadius);
}

void FluentNotification::paintBorder(QPainter* painter) {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();
    QColor borderColor = getTypeColor();

    // Use design tokens for border styling
    int borderWidth = tokenUtils.getBorderWidth("thin");
    int borderRadius = tokenUtils.getBorderRadius("medium");

    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(borderColor, borderWidth));
    painter->drawRoundedRect(rect().adjusted(1, 1, -1, -1), borderRadius,
                             borderRadius);
}

void FluentNotification::paintShadow(QPainter* painter) {
    if (m_elevation == FluentNotificationElevation::None) {
        return;  // No shadow for None elevation
    }

    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

    // Use FluentUI elevation system for shadows
    QColor shadowColor =
        tokenUtils.getColor("color.shadow.primary", QColor(0, 0, 0));

    // FluentUI elevation specifications
    int offsetY = 0;
    int alpha = 0;
    // Note: blurRadius would be used for QGraphicsDropShadowEffect if needed

    switch (m_elevation) {
        case FluentNotificationElevation::Low:
            offsetY = 1;
            alpha = 28;
            break;
        case FluentNotificationElevation::Medium:
            offsetY = 2;
            alpha = 35;
            break;
        case FluentNotificationElevation::High:
            offsetY = 4;
            alpha = 42;
            break;
        case FluentNotificationElevation::VeryHigh:
            offsetY = 8;
            alpha = 50;
            break;
        default:
            return;
    }

    // Adjust shadow based on notification type for additional visual hierarchy
    switch (m_type) {
        case FluentNotificationType::Error:
            alpha = qMin(alpha + 10, 60);  // Slightly more prominent for errors
            break;
        case FluentNotificationType::Warning:
            alpha = qMin(alpha + 5, 55);
            break;
        default:
            break;
    }

    shadowColor.setAlpha(alpha);
    painter->setBrush(shadowColor);
    painter->setPen(Qt::NoPen);

    int borderRadius = tokenUtils.getBorderRadius("medium");

    // Create shadow effect with proper offset
    QRect shadowRect = rect().adjusted(0, offsetY, 0, offsetY);
    painter->drawRoundedRect(shadowRect, borderRadius, borderRadius);
}

// Utility methods
QColor FluentNotification::getTypeColor() const {
    const auto& tokenUtils = Styling::FluentDesignTokenUtils::instance();

    switch (m_type) {
        case FluentNotificationType::Success:
            return tokenUtils.getColor("color.border.success",
                                       QColor(16, 124, 16));
        case FluentNotificationType::Warning:
            return tokenUtils.getColor("color.border.warning",
                                       QColor(255, 140, 0));
        case FluentNotificationType::Error:
            return tokenUtils.getColor("color.border.critical",
                                       QColor(196, 43, 28));
        case FluentNotificationType::Info:
        default:
            return tokenUtils.getColor("color.border.info",
                                       QColor(0, 120, 212));
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

// Factory methods (from Simple variant)
FluentNotification* FluentNotification::createInfo(const QString& title,
                                                   const QString& message,
                                                   QWidget* parent) {
    auto* notification = new FluentNotification(FluentNotificationType::Info,
                                                title, message, parent);
    notification->setComplexity(FluentNotificationComplexity::Simple);
    return notification;
}

FluentNotification* FluentNotification::createSuccess(const QString& title,
                                                      const QString& message,
                                                      QWidget* parent) {
    auto* notification = new FluentNotification(FluentNotificationType::Success,
                                                title, message, parent);
    notification->setComplexity(FluentNotificationComplexity::Simple);
    return notification;
}

FluentNotification* FluentNotification::createWarning(const QString& title,
                                                      const QString& message,
                                                      QWidget* parent) {
    auto* notification = new FluentNotification(FluentNotificationType::Warning,
                                                title, message, parent);
    notification->setComplexity(FluentNotificationComplexity::Simple);
    return notification;
}

FluentNotification* FluentNotification::createError(const QString& title,
                                                    const QString& message,
                                                    QWidget* parent) {
    auto* notification = new FluentNotification(FluentNotificationType::Error,
                                                title, message, parent);
    notification->setComplexity(FluentNotificationComplexity::Simple);
    return notification;
}

FluentNotification* FluentNotification::createSimple(
    FluentNotificationType type, const QString& title, const QString& message,
    QWidget* parent) {
    auto* notification = new FluentNotification(type, title, message, parent);
    notification->setComplexity(FluentNotificationComplexity::Simple);
    return notification;
}

}  // namespace FluentQt::Components
