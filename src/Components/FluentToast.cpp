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

    // Use theme tokens for spacing and sizing
    const int contentPadding = theme.paddingValue("medium");
    const int contentSpacing = theme.spacing("small");
    const int iconSize = theme.iconSize("small").width();
    const int buttonSize = theme.componentHeight("small");
    const int progressHeight = theme.spacing("xsmall");

    int width = m_config.minWidth;
    int height = contentPadding;  // Top padding

    // Icon space calculation
    const int iconSpace = iconSize + contentSpacing;

    // Calculate available text width
    int textWidth =
        width - iconSpace - (contentPadding * 2);  // Account for margins
    if (m_config.closable) {
        textWidth -= buttonSize + contentSpacing;  // Close button space
    }

    // Title height calculation
    if (!m_title.isEmpty()) {
        const QRect titleRect = titleMetrics.boundingRect(
            QRect(0, 0, textWidth, 0), Qt::TextWordWrap, m_title);
        height += titleRect.height() + theme.spacing("xsmall");
        width = qMax(width,
                     titleRect.width() + iconSpace + (contentPadding * 2) +
                         (m_config.closable ? buttonSize + contentSpacing : 0));
    }

    // Message height calculation
    if (!m_message.isEmpty()) {
        const QRect messageRect = messageMetrics.boundingRect(
            QRect(0, 0, textWidth, 0), Qt::TextWordWrap, m_message);
        height += messageRect.height() + theme.spacing("xsmall");
        width = qMax(width,
                     messageRect.width() + iconSpace + (contentPadding * 2) +
                         (m_config.closable ? buttonSize + contentSpacing : 0));
    }

    // Progress bar height
    if (m_config.showProgress) {
        height += progressHeight + contentSpacing;
    }

    // Actions height
    if (!m_actions.isEmpty()) {
        height += buttonSize + contentSpacing;
    }

    height += contentPadding;  // Bottom padding

    // Constrain to max width with theme considerations
    const int maxWidth = m_config.maxWidth > 0 ? m_config.maxWidth
                                               : theme.componentWidth("dialog");
    width = qMin(width, maxWidth);

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

void FluentToast::keyPressEvent(QKeyEvent* event) {
    // Enhanced keyboard navigation for accessibility
    switch (event->key()) {
        case Qt::Key_Escape:
            if (m_config.closable) {
                dismiss();
                event->accept();
                return;
            }
            break;

        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            // Activate the primary action or close if no actions
            if (!m_actions.isEmpty()) {
                // Find and trigger the primary action
                for (int i = 0; i < m_actions.size(); ++i) {
                    if (m_actions[i].primary && i < m_actionButtons.size()) {
                        emit m_actionButtons[i]->clicked();
                        event->accept();
                        return;
                    }
                }
                // If no primary action, trigger the first action
                if (!m_actionButtons.isEmpty()) {
                    emit m_actionButtons[0]->clicked();
                    event->accept();
                    return;
                }
            } else if (m_config.closable) {
                dismiss();
                event->accept();
                return;
            }
            break;

        case Qt::Key_Tab:
            // Handle tab navigation between toast elements
            if (m_config.closable && m_closeButton) {
                m_closeButton->setFocus();
                event->accept();
                return;
            }
            break;

        default:
            break;
    }

    FluentComponent::keyPressEvent(event);
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
    const auto& theme = FluentTheme::instance();

    // Use theme tokens for layout spacing
    const int contentPadding = theme.paddingValue("medium");
    const int contentSpacing = theme.spacing("small");
    const int iconSize = theme.iconSize("small").width();
    const int buttonSize = theme.componentHeight("small");

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(contentPadding, contentPadding,
                                     contentPadding, contentPadding);
    m_mainLayout->setSpacing(contentSpacing);

    // Content layout (icon + text + close button)
    m_contentLayout = new QHBoxLayout();
    m_contentLayout->setSpacing(contentSpacing);

    // Icon label with theme-based sizing
    m_iconLabel = new QLabel();
    m_iconLabel->setFixedSize(iconSize, iconSize);
    m_iconLabel->setScaledContents(true);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_contentLayout->addWidget(m_iconLabel);

    // Text layout with theme spacing
    auto* textLayout = new QVBoxLayout();
    textLayout->setSpacing(theme.spacing("xsmall"));

    m_titleLabel = new QLabel();
    m_titleLabel->setWordWrap(true);
    m_titleLabel->setFont(theme.titleSmallFont());
    textLayout->addWidget(m_titleLabel);

    m_messageLabel = new QLabel();
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setFont(theme.bodyFont());
    textLayout->addWidget(m_messageLabel);

    m_contentLayout->addLayout(textLayout, 1);

    // Close button with proper accessibility
    m_closeButton = new FluentButton();
    m_closeButton->setIcon(QIcon(":/icons/close"));
    m_closeButton->setFixedSize(buttonSize, buttonSize);
    m_closeButton->setVisible(m_config.closable);
    m_closeButton->setAccessibleName(tr("Close notification"));
    m_closeButton->setAccessibleDescription(
        tr("Close this toast notification"));
    m_closeButton->setToolTip(tr("Close"));
    connect(m_closeButton, &FluentButton::clicked, this,
            &FluentToast::onCloseButtonClicked);
    m_contentLayout->addWidget(m_closeButton);

    m_mainLayout->addLayout(m_contentLayout);

    // Progress bar with theme styling
    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 100);
    m_progressBar->setVisible(m_config.showProgress);
    m_progressBar->setFixedHeight(theme.spacing("xsmall"));
    m_mainLayout->addWidget(m_progressBar);

    // Actions layout with theme spacing
    m_actionsLayout = new QHBoxLayout();
    m_actionsLayout->setSpacing(contentSpacing);
    m_actionsLayout->addStretch();
    m_mainLayout->addLayout(m_actionsLayout);

    // Apply Fluent styling and setup
    setupFluentStyling();
    updateIcon();
    updateLayout();
}

void FluentToast::setupAnimations() {
    const auto& theme = FluentTheme::instance();

    // Opacity effect for animations
    m_opacityEffect = std::make_unique<QGraphicsOpacityEffect>(this);
    setGraphicsEffect(m_opacityEffect.get());

    // Shadow effect with theme-based elevation
    m_shadowEffect = std::make_unique<QGraphicsDropShadowEffect>(this);
    updateElevation();  // Apply theme-based shadow settings

    // Enhanced show animation with Fluent motion principles
    m_showAnimation =
        std::make_unique<QPropertyAnimation>(m_opacityEffect.get(), "opacity");

    // Use theme-based animation duration or config override
    const int duration =
        m_config.animationDuration > 0
            ? m_config.animationDuration
            : theme.spacing("medium") * 10;  // Convert to reasonable ms value

    m_showAnimation->setDuration(duration);

    // Use Fluent Design easing curves
    QEasingCurve showEasing = m_config.easingCurve;
    if (showEasing.type() == QEasingCurve::Linear) {
        // Default to Fluent-recommended easing
        showEasing = QEasingCurve::OutCubic;
    }
    m_showAnimation->setEasingCurve(showEasing);
    m_showAnimation->setStartValue(0.0);
    m_showAnimation->setEndValue(1.0);
    connect(m_showAnimation.get(), &QPropertyAnimation::finished, this,
            &FluentToast::onShowAnimationFinished);

    // Enhanced hide animation
    m_hideAnimation =
        std::make_unique<QPropertyAnimation>(m_opacityEffect.get(), "opacity");
    m_hideAnimation->setDuration(duration * 0.8);  // Slightly faster hide

    QEasingCurve hideEasing = QEasingCurve::InCubic;  // Fluent hide pattern
    m_hideAnimation->setEasingCurve(hideEasing);
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
        // Use semantic color tokens based on toast type
        switch (m_config.type) {
            case FluentToastType::Success:
                m_backgroundColor = theme.color("successBackground");
                m_textColor = theme.color("successForeground");
                m_borderColor = theme.color("successBorder");
                break;
            case FluentToastType::Warning:
                m_backgroundColor = theme.color("warningBackground");
                m_textColor = theme.color("warningForeground");
                m_borderColor = theme.color("warningBorder");
                break;
            case FluentToastType::Error:
                m_backgroundColor = theme.color("errorBackground");
                m_textColor = theme.color("errorForeground");
                m_borderColor = theme.color("errorBorder");
                break;
            case FluentToastType::Info:
                m_backgroundColor = theme.color("infoBackground");
                m_textColor = theme.color("infoForeground");
                m_borderColor = theme.color("infoBorder");
                break;
            case FluentToastType::Custom:
            default:
                m_backgroundColor = theme.color("cardBackground");
                m_textColor = theme.color("textPrimary");
                m_borderColor = theme.color("strokeSecondary");
                break;
        }

        // Ensure accessibility compliance
        if (!theme.meetsAccessibilityStandards(m_textColor,
                                               m_backgroundColor)) {
            m_textColor =
                const_cast<Styling::FluentTheme&>(theme).generateContrastColor(
                    m_backgroundColor);
        }
    } else {
        // Use custom colors if provided, with fallbacks
        m_backgroundColor = m_config.customBackgroundColor.isValid()
                                ? m_config.customBackgroundColor
                                : theme.color("cardBackground");
        m_textColor = m_config.customTextColor.isValid()
                          ? m_config.customTextColor
                          : theme.color("textPrimary");
        m_borderColor = m_config.customBorderColor.isValid()
                            ? m_config.customBorderColor
                            : theme.color("strokeSecondary");
    }

    // Apply colors to UI elements with proper styling
    applyColorsToElements();
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
    // Enhanced accessibility with comprehensive ARIA attributes
    QString accessibleName = m_title;
    if (!m_message.isEmpty()) {
        accessibleName += " - " + m_message;
    }

    // Set basic accessibility properties
    setAccessibleName(accessibleName);
    setAccessibleDescription(QString("Toast notification: %1").arg(m_title));

    // Set ARIA-like properties for better screen reader support
    setProperty("role", "alert");
    setProperty("aria-live", "polite");
    setProperty("aria-atomic", "true");

    // Set toast type for screen readers
    QString toastTypeText;
    switch (m_config.type) {
        case FluentToastType::Success:
            toastTypeText = tr("Success");
            setProperty("aria-live", "polite");
            break;
        case FluentToastType::Warning:
            toastTypeText = tr("Warning");
            setProperty("aria-live", "assertive");
            break;
        case FluentToastType::Error:
            toastTypeText = tr("Error");
            setProperty("aria-live", "assertive");
            break;
        case FluentToastType::Info:
            toastTypeText = tr("Information");
            setProperty("aria-live", "polite");
            break;
        case FluentToastType::Custom:
        default:
            toastTypeText = tr("Notification");
            break;
    }

    setProperty("aria-label",
                QString("%1: %2").arg(toastTypeText, accessibleName));

    // Set dismissible state
    if (m_config.closable) {
        setProperty("aria-describedby", "toast-close-instruction");
        setProperty("aria-keyshortcuts", "Escape");
    }

    // Set progress information if applicable
    if (m_config.showProgress && m_progressBar) {
        setProperty("aria-describedby", "toast-progress");
    }
}

void FluentToast::paintBackground(QPainter& painter, const QRect& rect) {
    const auto& theme = FluentTheme::instance();
    const int radius = theme.borderRadius("medium");

    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);

    painter.fillPath(path, m_backgroundColor);
}

void FluentToast::paintBorder(QPainter& painter, const QRect& rect) {
    const auto& theme = FluentTheme::instance();
    const int radius = theme.borderRadius("medium");
    const int strokeWidth = theme.strokeWidth("thin");

    QPen pen(m_borderColor, strokeWidth);
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
    // Elevation is managed through updateElevation()
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
    const auto& theme = FluentTheme::instance();
    const int progressHeight = theme.spacing("xsmall");
    return QRect(contentRect.left(), contentRect.bottom() - progressHeight,
                 contentRect.width(), progressHeight);
}

// New helper methods for enhanced Fluent UI compliance

void FluentToast::setupFluentStyling() {
    const auto& theme = FluentTheme::instance();

    // Apply elevation/shadow effect if enabled
    if (m_config.autoElevation) {
        updateElevation();
    }

    // Set up keyboard navigation if enabled
    if (m_config.enableKeyboardNavigation) {
        setFocusPolicy(Qt::StrongFocus);
        if (m_closeButton) {
            setTabOrder(this, m_closeButton);
        }
    }

    // Apply theme-based corner radius
    setCornerRadius(theme.borderRadius("medium"));

    // Enable smooth transitions based on config and system settings
    bool enableTransitions = m_config.useFluentMotion;
    if (m_config.respectReducedMotion) {
        // Check system reduced motion preference (simplified check)
        // In a real implementation, this would check system accessibility
        // settings
        enableTransitions = enableTransitions && !theme.isReducedMotionMode();
    }

    setSmoothTransitions(enableTransitions);
    if (enableTransitions) {
        const int duration = m_config.animationDuration > 0
                                 ? m_config.animationDuration
                                 : theme.spacing("medium") * 10;
        setTransitionDuration(duration);
    }
}

void FluentToast::updateElevation() {
    const auto& theme = FluentTheme::instance();

    if (m_shadowEffect) {
        // Use theme elevation system
        const int elevation = theme.elevation("card");
        m_shadowEffect->setBlurRadius(elevation);
        m_shadowEffect->setOffset(0, elevation / 4);

        // Adjust shadow color based on theme
        QColor shadowColor = theme.color("shadow");
        shadowColor.setAlpha(60);
        m_shadowEffect->setColor(shadowColor);
    }
}

void FluentToast::applyColorsToElements() {
    const auto& theme = FluentTheme::instance();

    // Apply colors to text elements with proper contrast
    if (m_titleLabel) {
        m_titleLabel->setStyleSheet(
            QString("QLabel { color: %1; font-weight: 600; }")
                .arg(m_textColor.name()));
    }

    if (m_messageLabel) {
        m_messageLabel->setStyleSheet(
            QString("QLabel { color: %1; }").arg(m_textColor.name()));
    }

    // Style progress bar with theme tokens
    if (m_progressBar) {
        const int borderRadius = theme.borderRadius("small");
        const QString progressStyle =
            QString(
                "QProgressBar {"
                "    background-color: %1;"
                "    border: %2px solid %3;"
                "    border-radius: %4px;"
                "    text-align: center;"
                "}"
                "QProgressBar::chunk {"
                "    background-color: %5;"
                "    border-radius: %6px;"
                "}")
                .arg(m_backgroundColor.lighter(110).name())
                .arg(theme.strokeWidth("hairline"))
                .arg(m_borderColor.name())
                .arg(borderRadius)
                .arg(m_textColor.name())
                .arg(borderRadius - 1);

        m_progressBar->setStyleSheet(progressStyle);
    }
}
