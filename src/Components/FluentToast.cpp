// src/Components/FluentToast.cpp
#include "FluentQt/Components/FluentToast.h"
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QDebug>
#include <QFontMetrics>
#include <QHideEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QScreen>
#include <QShowEvent>

using namespace FluentQt::Components;
using namespace FluentQt::Core;
using namespace FluentQt::Styling;

FluentToast::FluentToast(QWidget* parent) : FluentComponent(parent) {
    setupUI();
    setupAnimations();
    setupTimer();

    // Connect to theme changes
    connect(&FluentTheme::instance(), &FluentTheme::themeChanged, this,
            &FluentToast::onThemeChanged);

    updateColors();
    updateAccessibility();

    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint);
}

FluentToast::FluentToast(FluentToastType type, const QString& title,
                         QWidget* parent)
    : FluentToast(parent) {
    m_config.type = type;
    setTitle(title);
}

FluentToast::FluentToast(FluentToastType type, const QString& title,
                         const QString& message, QWidget* parent)
    : FluentToast(parent) {
    m_config.type = type;
    setTitle(title);
    setMessage(message);
}

FluentToast::FluentToast(const FluentToastConfig& config, QWidget* parent)
    : FluentToast(parent) {
    setConfiguration(config);
}

FluentToast::~FluentToast() = default;

void FluentToast::setTitle(const QString& title) {
    if (m_title != title) {
        m_title = title;
        m_sizeHintValid = false;
        updateLayout();
        updateAccessibility();
        emit titleChanged(title);
    }
}

void FluentToast::setMessage(const QString& message) {
    if (m_message != message) {
        m_message = message;
        m_sizeHintValid = false;
        updateLayout();
        emit messageChanged(message);
    }
}

void FluentToast::setToastType(FluentToastType type) {
    if (m_config.type != type) {
        m_config.type = type;
        updateIcon();
        updateColors();
        update();
        emit typeChanged(type);
    }
}

void FluentToast::setIcon(const QIcon& icon) {
    if (m_icon.cacheKey() != icon.cacheKey()) {
        m_icon = icon;
        updateIcon();
        update();
        emit iconChanged(icon);
    }
}

void FluentToast::setDuration(int duration) {
    if (m_config.duration != duration) {
        m_config.duration = duration;
        if (m_autoHideTimer && m_autoHideTimer->isActive()) {
            if (duration > 0 && !m_config.persistent) {
                m_autoHideTimer->start(duration);
            } else {
                m_autoHideTimer->stop();
            }
        }
        emit durationChanged(duration);
    }
}

void FluentToast::setClosable(bool closable) {
    if (m_config.closable != closable) {
        m_config.closable = closable;
        updateLayout();
        emit closableChanged(closable);
    }
}

void FluentToast::setPauseOnHover(bool pause) {
    if (m_config.pauseOnHover != pause) {
        m_config.pauseOnHover = pause;
        emit pauseOnHoverChanged(pause);
    }
}

void FluentToast::setShowProgress(bool show) {
    if (m_config.showProgress != show) {
        m_config.showProgress = show;
        updateProgress();
        updateLayout();
        emit showProgressChanged(show);
    }
}

void FluentToast::setPersistent(bool persistent) {
    if (m_config.persistent != persistent) {
        m_config.persistent = persistent;
        if (persistent && m_autoHideTimer) {
            m_autoHideTimer->stop();
        } else if (!persistent && m_config.duration > 0 && m_autoHideTimer) {
            m_autoHideTimer->start(m_config.duration);
        }
        emit persistentChanged(persistent);
    }
}

void FluentToast::setConfiguration(const FluentToastConfig& config) {
    const bool hasTypeChanged = (m_config.type != config.type);
    const bool hasDurationChanged = (m_config.duration != config.duration);
    const bool hasClosableChanged = (m_config.closable != config.closable);
    const bool hasShowProgressChanged =
        (m_config.showProgress != config.showProgress);
    const bool hasPersistentChanged =
        (m_config.persistent != config.persistent);

    m_config = config;

    m_sizeHintValid = false;
    updateLayout();
    updateColors();
    updateIcon();
    updateProgress();
    updateAccessibility();

    // Update timer if needed
    if (hasDurationChanged || hasPersistentChanged) {
        if (m_autoHideTimer) {
            if (m_config.duration > 0 && !m_config.persistent) {
                m_autoHideTimer->start(m_config.duration);
            } else {
                m_autoHideTimer->stop();
            }
        }
    }

    // Emit change signals
    if (hasTypeChanged)
        emit typeChanged(config.type);
    if (hasDurationChanged)
        emit durationChanged(config.duration);
    if (hasClosableChanged)
        emit closableChanged(config.closable);
    if (hasShowProgressChanged)
        emit showProgressChanged(config.showProgress);
    if (hasPersistentChanged)
        emit persistentChanged(config.persistent);
    emit pauseOnHoverChanged(config.pauseOnHover);
}

void FluentToast::addAction(const FluentToastAction& action) {
    m_actions.append(action);
    updateActionButtons();
    updateLayout();
}

void FluentToast::addAction(const QString& text, std::function<void()> callback,
                            bool primary) {
    addAction(FluentToastAction(text, std::move(callback), primary));
}

void FluentToast::addAction(const QIcon& icon, const QString& text,
                            std::function<void()> callback, bool primary) {
    addAction(FluentToastAction(icon, text, std::move(callback), primary));
}

void FluentToast::clearActions() {
    m_actions.clear();
    updateActionButtons();
    updateLayout();
}

void FluentToast::setProgress(int value) {
    if (m_progressBar) {
        m_progressBar->setValue(value);
        emit progressChanged(value);
    }
}

void FluentToast::setProgressRange(int minimum, int maximum) {
    if (m_progressBar) {
        m_progressBar->setRange(minimum, maximum);
    }
}

int FluentToast::progress() const {
    return m_progressBar ? m_progressBar->value() : 0;
}

int FluentToast::progressMinimum() const {
    return m_progressBar ? m_progressBar->minimum() : 0;
}

int FluentToast::progressMaximum() const {
    return m_progressBar ? m_progressBar->maximum() : 100;
}

void FluentToast::setUserData(const QVariant& data) { m_userData = data; }

void FluentToast::setOpacity(qreal opacity) {
    if (m_opacity != opacity) {
        m_opacity = opacity;
        if (m_opacityEffect) {
            m_opacityEffect->setOpacity(opacity);
        }
        update();
    }
}

bool FluentToast::isVisible() const {
    return QWidget::isVisible() && !m_hiding;
}

QSize FluentToast::sizeHint() const {
    if (!m_sizeHintValid) {
        const_cast<FluentToast*>(this)->m_cachedSizeHint = calculateSizeHint();
        const_cast<FluentToast*>(this)->m_sizeHintValid = true;
    }
    return m_cachedSizeHint;
}

QSize FluentToast::minimumSizeHint() const {
    return QSize(m_config.minWidth, 60);
}

QSize FluentToast::calculateSizeHint() const {
    const auto& theme = FluentTheme::instance();
    const QFontMetrics titleMetrics(theme.titleSmallFont());
    const QFontMetrics messageMetrics(theme.bodyFont());

    int width = m_config.minWidth;
    int height = 16;  // Top padding

    // Icon space
    const int iconSize = 24;
    const int iconSpace = iconSize + 8;

    // Calculate text width
    int textWidth = width - iconSpace - 16;  // Margins
    if (m_config.closable) {
        textWidth -= 32;  // Close button space
    }

    // Title height
    if (!m_title.isEmpty()) {
        const QRect titleRect = titleMetrics.boundingRect(
            QRect(0, 0, textWidth, 0), Qt::TextWordWrap, m_title);
        height += titleRect.height() + 4;
        width = qMax(width, titleRect.width() + iconSpace + 32);
    }

    // Message height
    if (!m_message.isEmpty()) {
        const QRect messageRect = messageMetrics.boundingRect(
            QRect(0, 0, textWidth, 0), Qt::TextWordWrap, m_message);
        height += messageRect.height() + 4;
        width = qMax(width, messageRect.width() + iconSpace + 32);
    }

    // Progress bar height
    if (m_config.showProgress) {
        height += 8 + 4;  // Progress bar height + spacing
    }

    // Actions height
    if (!m_actions.isEmpty()) {
        height += 32 + 8;  // Button height + spacing
    }

    height += 16;  // Bottom padding

    // Constrain to max width
    if (m_config.maxWidth > 0) {
        width = qMin(width, m_config.maxWidth);
    }

    return QSize(width, height);
}

// Static factory methods
FluentToast* FluentToast::createInfo(const QString& title,
                                     const QString& message, QWidget* parent) {
    return new FluentToast(FluentToastType::Info, title, message, parent);
}

FluentToast* FluentToast::createSuccess(const QString& title,
                                        const QString& message,
                                        QWidget* parent) {
    return new FluentToast(FluentToastType::Success, title, message, parent);
}

FluentToast* FluentToast::createWarning(const QString& title,
                                        const QString& message,
                                        QWidget* parent) {
    return new FluentToast(FluentToastType::Warning, title, message, parent);
}

FluentToast* FluentToast::createError(const QString& title,
                                      const QString& message, QWidget* parent) {
    return new FluentToast(FluentToastType::Error, title, message, parent);
}

FluentToast* FluentToast::createCustom(const QIcon& icon, const QString& title,
                                       const QString& message,
                                       QWidget* parent) {
    auto* toast =
        new FluentToast(FluentToastType::Custom, title, message, parent);
    toast->setIcon(icon);
    return toast;
}

// Public slots
void FluentToast::show() {
    if (m_showing || isVisible())
        return;

    QWidget::show();
    emit shown();
}

void FluentToast::hide() {
    if (m_hiding || !isVisible())
        return;

    QWidget::hide();
    emit hidden();
}

void FluentToast::showAnimated() {
    if (m_showing || isVisible())
        return;

    emit aboutToShow();
    m_showing = true;
    m_hiding = false;

    startShowAnimation();
}

void FluentToast::hideAnimated() {
    if (m_hiding || !isVisible())
        return;

    emit aboutToHide();
    m_hiding = true;
    m_showing = false;

    pauseTimer();
    startHideAnimation();
}

void FluentToast::dismiss() {
    emit dismissed();
    hideAnimated();
}

void FluentToast::pauseTimer() {
    if (m_autoHideTimer && m_autoHideTimer->isActive() && !m_timerPaused) {
        m_remainingTime = m_autoHideTimer->remainingTime();
        m_pauseTime = QDateTime::currentDateTime();
        m_autoHideTimer->stop();
        m_timerPaused = true;
        emit timerPaused();
    }
}

void FluentToast::resumeTimer() {
    if (m_autoHideTimer && m_timerPaused && m_remainingTime > 0) {
        m_autoHideTimer->start(m_remainingTime);
        m_timerPaused = false;
        emit timerResumed();
    }
}

void FluentToast::resetTimer() {
    if (m_autoHideTimer && m_config.duration > 0 && !m_config.persistent) {
        m_autoHideTimer->start(m_config.duration);
        m_timerPaused = false;
        m_remainingTime = 0;
    }
}

// Protected event handlers
void FluentToast::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    FLUENT_PROFILE("FluentToast::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const QRect rect = this->rect();

    // Paint shadow
    paintShadow(painter, rect);

    // Paint background
    paintBackground(painter, rect);

    // Paint border
    paintBorder(painter, rect);
}

void FluentToast::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        setState(FluentState::Pressed);
        event->accept();
    } else {
        FluentComponent::mousePressEvent(event);
    }
}

void FluentToast::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;
        setState(m_hovered ? FluentState::Hovered : FluentState::Normal);

        if (rect().contains(event->pos())) {
            emit clicked();
        }

        event->accept();
    } else {
        FluentComponent::mouseReleaseEvent(event);
    }
}

void FluentToast::enterEvent(QEnterEvent* event) {
    FluentComponent::enterEvent(event);

    m_hovered = true;
    setState(FluentState::Hovered);

    if (m_config.pauseOnHover) {
        pauseTimer();
    }
}

void FluentToast::leaveEvent(QEvent* event) {
    FluentComponent::leaveEvent(event);

    m_hovered = false;
    m_pressed = false;
    setState(FluentState::Normal);

    if (m_config.pauseOnHover) {
        resumeTimer();
    }
}

void FluentToast::resizeEvent(QResizeEvent* event) {
    FluentComponent::resizeEvent(event);

    if (event->size() != m_lastRect.size()) {
        m_lastRect = rect();
        m_sizeHintValid = false;
    }
}

void FluentToast::showEvent(QShowEvent* event) {
    FluentComponent::showEvent(event);

    // Start auto-hide timer
    if (m_autoHideTimer && m_config.duration > 0 && !m_config.persistent) {
        m_autoHideTimer->start(m_config.duration);
    }

    emit shown();
}

void FluentToast::hideEvent(QHideEvent* event) {
    FluentComponent::hideEvent(event);

    // Stop timers
    if (m_autoHideTimer) {
        m_autoHideTimer->stop();
    }
    if (m_progressTimer) {
        m_progressTimer->stop();
    }

    emit hidden();
}

void FluentToast::updateStateStyle() {
    FluentComponent::updateStateStyle();
    updateColors();
}

void FluentToast::performStateTransition(FluentState from, FluentState to) {
    FluentComponent::performStateTransition(from, to);

    // Add any state transition animations here
}

// Private slots
void FluentToast::onAutoHideTimer() { hideAnimated(); }

void FluentToast::onProgressTimer() {
    if (m_progressBar && m_autoHideTimer) {
        const int elapsed =
            m_config.duration - m_autoHideTimer->remainingTime();
        const int progress = (elapsed * 100) / m_config.duration;
        m_progressBar->setValue(progress);
    }
}

void FluentToast::onShowAnimationFinished() {
    m_showing = false;
    emit shown();
}

void FluentToast::onHideAnimationFinished() {
    m_hiding = false;
    hide();
    emit hidden();
}

void FluentToast::onCloseButtonClicked() { dismiss(); }

void FluentToast::onActionButtonClicked() {
    auto* button = qobject_cast<FluentButton*>(sender());
    if (!button)
        return;

    // Find the action
    for (int i = 0; i < m_actionButtons.size(); ++i) {
        if (m_actionButtons[i] == button && i < m_actions.size()) {
            const auto& action = m_actions[i];

            emit actionTriggered(action.text);

            if (action.callback) {
                action.callback();
            }

            if (action.dismissOnClick) {
                dismiss();
            }

            break;
        }
    }
}

void FluentToast::onThemeChanged() {
    updateColors();
    m_sizeHintValid = false;
    updateGeometry();
    update();
}

// Private implementation methods
void FluentToast::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(16, 12, 16, 12);
    m_mainLayout->setSpacing(8);

    // Content layout (icon + text + close button)
    m_contentLayout = new QHBoxLayout();
    m_contentLayout->setSpacing(8);

    // Icon label
    m_iconLabel = new QLabel();
    m_iconLabel->setFixedSize(24, 24);
    m_iconLabel->setScaledContents(true);
    m_contentLayout->addWidget(m_iconLabel);

    // Text layout
    auto* textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);

    m_titleLabel = new QLabel();
    m_titleLabel->setWordWrap(true);
    textLayout->addWidget(m_titleLabel);

    m_messageLabel = new QLabel();
    m_messageLabel->setWordWrap(true);
    textLayout->addWidget(m_messageLabel);

    m_contentLayout->addLayout(textLayout, 1);

    // Close button
    m_closeButton = new FluentButton();
    m_closeButton->setIcon(QIcon(":/icons/close"));
    m_closeButton->setFixedSize(24, 24);
    m_closeButton->setVisible(m_config.closable);
    connect(m_closeButton, &FluentButton::clicked, this,
            &FluentToast::onCloseButtonClicked);
    m_contentLayout->addWidget(m_closeButton);

    m_mainLayout->addLayout(m_contentLayout);

    // Progress bar
    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 100);
    m_progressBar->setVisible(m_config.showProgress);
    m_mainLayout->addWidget(m_progressBar);

    // Actions layout
    m_actionsLayout = new QHBoxLayout();
    m_actionsLayout->setSpacing(8);
    m_actionsLayout->addStretch();
    m_mainLayout->addLayout(m_actionsLayout);

    updateIcon();
    updateLayout();
}

void FluentToast::setupAnimations() {
    // Opacity effect for animations
    m_opacityEffect = std::make_unique<QGraphicsOpacityEffect>(this);
    setGraphicsEffect(m_opacityEffect.get());

    // Shadow effect
    m_shadowEffect = std::make_unique<QGraphicsDropShadowEffect>(this);
    m_shadowEffect->setBlurRadius(16);
    m_shadowEffect->setOffset(0, 4);
    m_shadowEffect->setColor(QColor(0, 0, 0, 60));

    // Show animation
    m_showAnimation =
        std::make_unique<QPropertyAnimation>(m_opacityEffect.get(), "opacity");
    m_showAnimation->setDuration(m_config.animationDuration);
    m_showAnimation->setEasingCurve(m_config.easingCurve);
    m_showAnimation->setStartValue(0.0);
    m_showAnimation->setEndValue(1.0);
    connect(m_showAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentToast::onShowAnimationFinished);

    // Hide animation
    m_hideAnimation =
        std::make_unique<QPropertyAnimation>(m_opacityEffect.get(), "opacity");
    m_hideAnimation->setDuration(m_config.animationDuration);
    m_hideAnimation->setEasingCurve(m_config.easingCurve);
    m_hideAnimation->setStartValue(1.0);
    m_hideAnimation->setEndValue(0.0);
    connect(m_hideAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentToast::onHideAnimationFinished);
}

void FluentToast::setupTimer() {
    // Auto-hide timer
    m_autoHideTimer = new QTimer(this);
    m_autoHideTimer->setSingleShot(true);
    connect(m_autoHideTimer, &QTimer::timeout, this,
            &FluentToast::onAutoHideTimer);

    // Progress timer (for progress bar animation)
    m_progressTimer = new QTimer(this);
    m_progressTimer->setInterval(50);  // Update every 50ms
    connect(m_progressTimer, &QTimer::timeout, this,
            &FluentToast::onProgressTimer);
}

void FluentToast::updateLayout() {
    if (!m_titleLabel || !m_messageLabel)
        return;

    // Update title
    m_titleLabel->setText(m_title);
    m_titleLabel->setVisible(!m_title.isEmpty());

    // Update message
    m_messageLabel->setText(m_message);
    m_messageLabel->setVisible(!m_message.isEmpty());

    // Update close button
    if (m_closeButton) {
        m_closeButton->setVisible(m_config.closable);
    }

    // Update progress bar
    if (m_progressBar) {
        m_progressBar->setVisible(m_config.showProgress);
        if (m_config.showProgress && m_config.duration > 0) {
            m_progressTimer->start();
        } else {
            m_progressTimer->stop();
        }
    }

    // Update actions
    updateActionButtons();

    m_sizeHintValid = false;
    updateGeometry();
}

void FluentToast::updateColors() {
    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    if (m_config.autoCalculateColors) {
        // Calculate colors based on theme and type
        switch (m_config.type) {
            case FluentToastType::Success:
                m_backgroundColor = palette.success;
                m_textColor = palette.neutralLightest;
                m_borderColor = palette.success.darker(120);
                break;
            case FluentToastType::Warning:
                m_backgroundColor = palette.warning;
                m_textColor = palette.neutralDarkest;
                m_borderColor = palette.warning.darker(120);
                break;
            case FluentToastType::Error:
                m_backgroundColor = palette.error;
                m_textColor = palette.neutralLightest;
                m_borderColor = palette.error.darker(120);
                break;
            case FluentToastType::Info:
                m_backgroundColor = palette.accent;
                m_textColor = palette.neutralLightest;
                m_borderColor = palette.accent.darker(120);
                break;
            case FluentToastType::Custom:
            default:
                m_backgroundColor = palette.neutralLighter;
                m_textColor = palette.neutralPrimary;
                m_borderColor = palette.neutralTertiary;
                break;
        }
    } else {
        // Use custom colors if provided
        if (m_config.customBackgroundColor.isValid()) {
            m_backgroundColor = m_config.customBackgroundColor;
        }
        if (m_config.customTextColor.isValid()) {
            m_textColor = m_config.customTextColor;
        }
        if (m_config.customBorderColor.isValid()) {
            m_borderColor = m_config.customBorderColor;
        }
    }

    // Apply colors to UI elements
    if (m_titleLabel) {
        m_titleLabel->setStyleSheet(
            QString("color: %1; font-weight: bold;").arg(m_textColor.name()));
    }
    if (m_messageLabel) {
        m_messageLabel->setStyleSheet(
            QString("color: %1;").arg(m_textColor.name()));
    }
    if (m_progressBar) {
        m_progressBar->setStyleSheet(
            QString("QProgressBar { background-color: %1; border: 1px solid "
                    "%2; border-radius: 2px; }"
                    "QProgressBar::chunk { background-color: %3; }")
                .arg(m_backgroundColor.lighter(120).name())
                .arg(m_borderColor.name())
                .arg(m_textColor.name()));
    }
}

void FluentToast::updateIcon() {
    if (!m_iconLabel)
        return;

    QIcon displayIcon = m_icon;

    // Use type-specific icon if no custom icon is set
    if (displayIcon.isNull()) {
        displayIcon = getTypeIcon();
    }

    if (!displayIcon.isNull()) {
        m_iconLabel->setPixmap(displayIcon.pixmap(24, 24));
        m_iconLabel->setVisible(true);
    } else {
        m_iconLabel->setVisible(false);
    }
}

void FluentToast::updateProgress() {
    if (!m_progressBar)
        return;

    if (m_config.showProgress) {
        m_progressBar->setVisible(true);
        m_progressBar->setValue(0);

        if (m_config.duration > 0 && m_progressTimer) {
            m_progressTimer->start();
        }
    } else {
        m_progressBar->setVisible(false);
        if (m_progressTimer) {
            m_progressTimer->stop();
        }
    }
}

void FluentToast::updateAccessibility() {
    QString accessibleName = m_title;
    if (!m_message.isEmpty()) {
        accessibleName += " - " + m_message;
    }

    setAccessibleName(accessibleName);
    setAccessibleDescription(QString("Toast notification: %1").arg(m_title));
}

void FluentToast::paintBackground(QPainter& painter, const QRect& rect) {
    const int radius = 8;

    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);

    painter.fillPath(path, m_backgroundColor);
}

void FluentToast::paintBorder(QPainter& painter, const QRect& rect) {
    const int radius = 8;

    QPen pen(m_borderColor, 1);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    QPainterPath path;
    path.addRoundedRect(rect.adjusted(0, 0, -1, -1), radius, radius);
    painter.drawPath(path);
}

void FluentToast::paintShadow(QPainter& painter, const QRect& rect) {
    Q_UNUSED(painter)
    Q_UNUSED(rect)
    // Shadow is handled by QGraphicsDropShadowEffect
}

QColor FluentToast::getBackgroundColor() const { return m_backgroundColor; }

QColor FluentToast::getTextColor() const { return m_textColor; }

QColor FluentToast::getBorderColor() const { return m_borderColor; }

QColor FluentToast::getProgressColor() const { return m_textColor; }

QIcon FluentToast::getTypeIcon() const {
    switch (m_config.type) {
        case FluentToastType::Success:
            return QIcon(":/icons/checkmark");
        case FluentToastType::Warning:
            return QIcon(":/icons/warning");
        case FluentToastType::Error:
            return QIcon(":/icons/error");
        case FluentToastType::Info:
            return QIcon(":/icons/info");
        case FluentToastType::Custom:
        default:
            return m_config.customIcon;
    }
}

void FluentToast::createActionButtons() {
    // Clear existing buttons
    for (auto* button : m_actionButtons) {
        m_actionsLayout->removeWidget(button);
        button->deleteLater();
    }
    m_actionButtons.clear();

    // Create new buttons
    for (const auto& action : m_actions) {
        auto* button = new FluentButton(action.text);

        if (!action.icon.isNull()) {
            button->setIcon(action.icon);
        }

        if (action.primary) {
            button->setButtonStyle(FluentButtonStyle::Primary);
        } else {
            button->setButtonStyle(FluentButtonStyle::Default);
        }

        connect(button, &FluentButton::clicked, this,
                &FluentToast::onActionButtonClicked);

        m_actionButtons.append(button);
        m_actionsLayout->addWidget(button);
    }
}

void FluentToast::updateActionButtons() {
    createActionButtons();

    // Show/hide actions layout
    const bool hasActions = !m_actions.isEmpty();
    for (int i = 0; i < m_actionsLayout->count(); ++i) {
        if (auto* item = m_actionsLayout->itemAt(i)) {
            if (auto* widget = item->widget()) {
                widget->setVisible(hasActions);
            }
        }
    }
}

void FluentToast::startShowAnimation() {
    if (m_showAnimation) {
        QWidget::show();
        m_showAnimation->start();
    } else {
        show();
    }
}

void FluentToast::startHideAnimation() {
    if (m_hideAnimation) {
        m_hideAnimation->start();
    } else {
        hide();
    }
}

void FluentToast::startProgressAnimation() {
    if (m_config.showProgress && m_progressTimer && m_config.duration > 0) {
        m_progressTimer->start();
    }
}

QRect FluentToast::getContentRect() const {
    return rect().adjusted(16, 12, -16, -12);
}

QRect FluentToast::getIconRect() const {
    const QRect contentRect = getContentRect();
    return QRect(contentRect.left(), contentRect.top(), 24, 24);
}

QRect FluentToast::getTextRect() const {
    const QRect contentRect = getContentRect();
    const QRect iconRect = getIconRect();
    int left = iconRect.right() + 8;
    int right = contentRect.right();

    if (m_config.closable) {
        right -= 32;  // Close button space
    }

    return QRect(left, contentRect.top(), right - left, contentRect.height());
}

QRect FluentToast::getActionsRect() const {
    const QRect contentRect = getContentRect();
    return QRect(contentRect.left(), contentRect.bottom() - 32,
                 contentRect.width(), 32);
}

QRect FluentToast::getCloseButtonRect() const {
    const QRect contentRect = getContentRect();
    return QRect(contentRect.right() - 24, contentRect.top(), 24, 24);
}

QRect FluentToast::getProgressRect() const {
    const QRect contentRect = getContentRect();
    return QRect(contentRect.left(), contentRect.bottom() - 8,
                 contentRect.width(), 8);
}
