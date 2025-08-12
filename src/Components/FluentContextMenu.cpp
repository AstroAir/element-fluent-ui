// src/Components/FluentContextMenu.cpp
#include "FluentQt/Components/FluentContextMenu.h"
#include <QApplication>
#include <QEasingCurve>
#include <QEnterEvent>
#include <QFocusEvent>
#include <QGraphicsDropShadowEffect>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScreen>
#include <QStyleOption>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentContextMenu::FluentContextMenu(QWidget* parent)
    : QMenu(parent),
      m_animationDuration(200),
      m_shadowEnabled(true),
      m_roundedCorners(true),
      m_fadeAnimation(new QPropertyAnimation(this, "windowOpacity", this)) {
    setupUI();
    setupAnimation();
    setupShadow();
}

void FluentContextMenu::setupUI() {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint |
                   Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // Apply Fluent styling
    updateTheme();

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentContextMenu::updateTheme);
}

void FluentContextMenu::setupAnimation() {
    m_fadeAnimation->setDuration(m_animationDuration);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_fadeAnimation, &QPropertyAnimation::finished, this,
            &FluentContextMenu::onAnimationFinished);
}

void FluentContextMenu::setupShadow() {
    if (m_shadowEnabled) {
        auto* shadow = new QGraphicsDropShadowEffect(this);

        // Enhanced shadow with Fluent Design elevation
        const auto& theme = Styling::FluentTheme::instance();
        bool isDarkMode = (theme.mode() == Styling::FluentThemeMode::Dark);

        shadow->setBlurRadius(isDarkMode ? 32 : 24);
        shadow->setColor(isDarkMode ? QColor(0, 0, 0, 120)
                                    : QColor(0, 0, 0, 40));
        shadow->setOffset(0, isDarkMode ? 8 : 6);

        setGraphicsEffect(shadow);
    }
}

void FluentContextMenu::setAnimationDuration(int duration) {
    if (m_animationDuration == duration)
        return;

    m_animationDuration = duration;
    m_fadeAnimation->setDuration(duration);
}

void FluentContextMenu::setShadowEnabled(bool enabled) {
    if (m_shadowEnabled == enabled)
        return;

    m_shadowEnabled = enabled;
    setupShadow();
}

void FluentContextMenu::setRoundedCorners(bool rounded) {
    if (m_roundedCorners == rounded)
        return;

    m_roundedCorners = rounded;
    update();
}

FluentMenuItem* FluentContextMenu::addFluentAction(const QString& text,
                                                   const QIcon& icon) {
    auto* action = new QAction(icon, text, this);
    QMenu::addAction(action);
    auto* item = new FluentMenuItem(action, this);
    return item;
}

FluentMenuItem* FluentContextMenu::addFluentAction(
    const QString& text, const QIcon& icon,
    const std::function<void()>& callback) {
    auto* item = addFluentAction(text, icon);
    if (callback && item->action()) {
        connect(item->action(), &QAction::triggered, callback);
    }
    return item;
}

FluentMenuItem* FluentContextMenu::addToggleAction(const QString& text,
                                                   const QIcon& icon,
                                                   bool checked) {
    auto* action = new QAction(icon, text, this);
    action->setCheckable(true);
    action->setChecked(checked);
    QMenu::addAction(action);
    auto* item = new FluentMenuItem(action, this);
    item->setItemType(FluentMenuItemType::Toggle);
    return item;
}

FluentMenuItem* FluentContextMenu::addRadioAction(const QString& text,
                                                  const QString& group,
                                                  const QIcon& icon,
                                                  bool checked) {
    auto* action = new QAction(icon, text, this);
    action->setCheckable(true);
    action->setChecked(checked);
    QMenu::addAction(action);
    auto* item = new FluentMenuItem(action, this);
    item->setItemType(FluentMenuItemType::Radio);
    item->setRadioGroup(group);

    // Handle radio group logic
    if (checked) {
        // Uncheck other items in the same group
        for (auto* otherItem : m_menuItems.values()) {
            if (otherItem != item &&
                otherItem->itemType() == FluentMenuItemType::Radio &&
                otherItem->radioGroup() == group) {
                otherItem->setChecked(false);
            }
        }
    }

    return item;
}

FluentMenuItem* FluentContextMenu::addCheckableAction(const QString& text,
                                                      const QIcon& icon,
                                                      bool checked) {
    auto* action = new QAction(icon, text, this);
    action->setCheckable(true);
    action->setChecked(checked);
    QMenu::addAction(action);
    auto* item = new FluentMenuItem(action, this);
    item->setItemType(FluentMenuItemType::Action);
    return item;
}

QAction* FluentContextMenu::addSeparator() { return QMenu::addSeparator(); }

FluentContextMenu* FluentContextMenu::addFluentSubmenu(const QString& title,
                                                       const QIcon& icon) {
    auto* submenu = new FluentContextMenu(this);
    submenu->setTitle(title);
    submenu->setIcon(icon);

    // Set up submenu with same styling as parent
    submenu->setAnimationDuration(m_animationDuration);
    submenu->setShadowEnabled(m_shadowEnabled);
    submenu->setRoundedCorners(m_roundedCorners);
    submenu->setEntranceAnimation(m_entranceAnimationType);
    submenu->setExitAnimation(m_exitAnimationType);

    QMenu::addMenu(submenu);

    // Connect submenu signals for cascading behavior
    connect(submenu, &FluentContextMenu::aboutToShow, this,
            [this, submenu]() { m_activeSubmenus.append(submenu); });

    connect(submenu, &FluentContextMenu::aboutToHide, this,
            [this, submenu]() { m_activeSubmenus.removeAll(submenu); });

    return submenu;
}

void FluentContextMenu::showEvent(QShowEvent* event) {
    QMenu::showEvent(event);

    if (m_animationDuration > 0 && m_animationEnabled) {
        animateShow();
    }
}

void FluentContextMenu::animateShow() {
    using namespace Animation;

    switch (m_entranceAnimationType) {
        case FluentAnimationType::Fade:
            setWindowOpacity(0.0);
            m_fadeAnimation->setStartValue(0.0);
            m_fadeAnimation->setEndValue(1.0);
            m_fadeAnimation->start();
            break;

        case FluentAnimationType::Scale: {
            setWindowOpacity(1.0);
            FluentAnimationConfig config;
            config.duration = std::chrono::milliseconds(m_animationDuration);
            auto scaleAnimation = FluentAnimator::scaleIn(this, config);
            scaleAnimation->start();
        } break;

        case FluentAnimationType::Slide: {
            setWindowOpacity(1.0);
            QPoint slideFrom = pos() + QPoint(0, -20);
            FluentAnimationConfig config;
            config.duration = std::chrono::milliseconds(m_animationDuration);
            auto slideAnimation =
                FluentAnimator::slideIn(this, slideFrom, config);
            slideAnimation->start();
        } break;

        default:
            setWindowOpacity(0.0);
            m_fadeAnimation->setStartValue(0.0);
            m_fadeAnimation->setEndValue(1.0);
            m_fadeAnimation->start();
            break;
    }
}

void FluentContextMenu::hideEvent(QHideEvent* event) {
    if (m_animationDuration > 0 && m_animationEnabled) {
        animateHide();
    }
    QMenu::hideEvent(event);
}

void FluentContextMenu::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // Get current theme colors
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Draw background with subtle gradient and elevation
    QRect bgRect = rect().adjusted(1, 1, -1, -1);  // Account for border

    if (m_roundedCorners) {
        // Create subtle gradient background
        QLinearGradient gradient(bgRect.topLeft(), bgRect.bottomLeft());
        gradient.setColorAt(0.0, palette.neutralLightest);
        gradient.setColorAt(1.0, palette.neutralLightest.darker(102));

        painter.setBrush(QBrush(gradient));
        painter.setPen(QPen(palette.neutralQuaternary, 1));
        painter.drawRoundedRect(bgRect, FluentSpacing::Small,
                                FluentSpacing::Small);

        // Add inner highlight for depth
        QRect highlightRect = bgRect.adjusted(1, 1, -1, -1);
        QPen highlightPen(palette.neutralLightest.lighter(110), 1);
        highlightPen.setStyle(Qt::SolidLine);
        painter.setPen(highlightPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(highlightRect, FluentSpacing::Small - 1,
                                FluentSpacing::Small - 1);
    } else {
        painter.fillRect(bgRect, palette.neutralLightest);
        painter.setPen(QPen(palette.neutralQuaternary, 1));
        painter.drawRect(bgRect);
    }

    // Let QMenu handle the rest
    QMenu::paintEvent(event);
}

void FluentContextMenu::updateTheme() {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Enhanced Fluent Design System colors with proper spacing
    QString styleSheet =
        QString(R"(
        QMenu {
            background-color: %1;
            border: 1px solid %2;
            border-radius: %9px;
            padding: %10px;
            color: %3;
            font-family: "Segoe UI Variable", "Segoe UI", "Helvetica Neue", Arial, sans-serif;
            font-size: %11px;
            font-weight: 400;
            selection-background-color: %4;
            min-width: 120px;
        }
        QMenu::item {
            background-color: transparent;
            padding: %12px %13px %12px %14px;
            margin: %15px %16px;
            border-radius: %17px;
            color: %3;
            min-height: %18px;
            border: none;
            font-size: %11px;
            line-height: 1.4;
        }
        QMenu::item:selected {
            background-color: %4;
            color: %5;
        }
        QMenu::item:pressed {
            background-color: %6;
            color: %5;
        }
        QMenu::item:disabled {
            color: %7;
            background-color: transparent;
        }
        QMenu::separator {
            height: 1px;
            background-color: %8;
            margin: 4px 8px;
            border: none;
        }
        QMenu::indicator {
            width: 16px;
            height: 16px;
            left: 8px;
        }
        QMenu::indicator:checked {
            image: url(:/icons/checkmark.png);
        }
        QMenu::right-arrow {
            image: url(:/icons/arrow-right.png);
            width: 12px;
            height: 12px;
        }
    )")
            .arg(palette.neutralLightest.name())      // %1 - Background
            .arg(palette.neutralQuaternary.name())    // %2 - Border
            .arg(palette.neutralPrimary.name())       // %3 - Text color
            .arg(palette.accent.lighter(180).name())  // %4 - Hover background
            .arg(palette.neutralLightest.name())      // %5 - Hover text
            .arg(palette.accent.darker(110).name())   // %6 - Pressed background
            .arg(palette.neutralTertiary.name())      // %7 - Disabled text
            .arg(palette.neutralQuaternaryAlt.name())  // %8 - Separator
            .arg(FluentSpacing::Small)                 // %9 - Border radius
            .arg(FluentSpacing::XSmall)                // %10 - Menu padding
            .arg(FluentTypography::BodyFontSize)       // %11 - Font size
            .arg(FluentSpacing::Small)   // %12 - Item vertical padding
            .arg(FluentSpacing::Medium)  // %13 - Item horizontal padding
            .arg(FluentSpacing::XXLarge +
                 FluentSpacing::Small)  // %14 - Item left padding (for icons)
            .arg(1)                     // %15 - Item vertical margin
            .arg(FluentSpacing::XSmall / 2)  // %16 - Item horizontal margin
            .arg(FluentSpacing::XSmall)      // %17 - Item border radius
            .arg(FluentSpacing::Large +
                 FluentSpacing::XSmall);  // %18 - Item min height

    setStyleSheet(styleSheet);

    // Update shadow color based on theme
    if (m_shadowEnabled && graphicsEffect()) {
        auto* shadow =
            qobject_cast<QGraphicsDropShadowEffect*>(graphicsEffect());
        if (shadow) {
            QColor shadowColor =
                (theme.mode() == Styling::FluentThemeMode::Dark)
                    ? QColor(0, 0, 0, 80)
                    : QColor(0, 0, 0, 40);
            shadow->setColor(shadowColor);
        }
    }
}

void FluentContextMenu::onAnimationFinished() {
    // Animation completed
}

void FluentContextMenu::setEntranceAnimation(
    Animation::FluentAnimationType type) {
    m_entranceAnimationType = type;
}

void FluentContextMenu::setExitAnimation(Animation::FluentAnimationType type) {
    m_exitAnimationType = type;
}

void FluentContextMenu::setHoverAnimationEnabled(bool enabled) {
    m_hoverAnimationEnabled = enabled;
    // Update all menu items
    for (auto* item : m_menuItems.values()) {
        item->setHoverAnimationEnabled(enabled);
    }
}

void FluentContextMenu::animateHide() {
    using namespace Animation;

    switch (m_exitAnimationType) {
        case FluentAnimationType::Fade:
            m_fadeAnimation->setStartValue(1.0);
            m_fadeAnimation->setEndValue(0.0);
            connect(m_fadeAnimation, &QPropertyAnimation::finished, this,
                    [this]() { QMenu::hide(); });
            m_fadeAnimation->start();
            break;

        case FluentAnimationType::Scale: {
            FluentAnimationConfig config;
            config.duration = std::chrono::milliseconds(m_animationDuration /
                                                        2);  // Faster exit
            auto scaleAnimation = FluentAnimator::scaleOut(this, config);
            connect(scaleAnimation.get(), &QPropertyAnimation::finished, this,
                    [this]() { QMenu::hide(); });
            scaleAnimation.release()->start();  // Release ownership and start
        } break;

        case FluentAnimationType::Slide: {
            QPoint slideTo = pos() + QPoint(0, -10);
            FluentAnimationConfig config;
            config.duration =
                std::chrono::milliseconds(m_animationDuration / 2);
            auto slideAnimation =
                FluentAnimator::slideOut(this, slideTo, config);
            connect(slideAnimation.get(), &QPropertyAnimation::finished, this,
                    [this]() { QMenu::hide(); });
            slideAnimation.release()->start();  // Release ownership and start
        } break;

        default:
            QMenu::hide();
            break;
    }
}

// FluentMenuItem implementation
FluentMenuItem::FluentMenuItem(QAction* action, QWidget* parent)
    : QWidget(parent),
      m_action(action),
      m_hoverAnimation(
          std::make_unique<QPropertyAnimation>(this, "hoverOpacity")),
      m_backgroundAnimation(
          std::make_unique<QPropertyAnimation>(this, "currentBackgroundColor")),
      m_focusAnimation(
          std::make_unique<QPropertyAnimation>(this, "focusOpacity")) {
    if (m_action) {
        connect(m_action, &QAction::changed, this,
                &FluentMenuItem::updateFromAction);
        updateFromAction();
    }

    // Set up hover tracking and focus policy
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    // Initialize animations
    setupHoverAnimations();
}

FluentMenuItem::FluentMenuItem(FluentMenuItemType type, QWidget* parent)
    : QWidget(parent), m_itemType(type) {
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
}

void FluentMenuItem::setAction(QAction* action) {
    if (m_action == action)
        return;

    if (m_action) {
        disconnect(m_action, nullptr, this, nullptr);
    }

    m_action = action;

    if (m_action) {
        connect(m_action, &QAction::changed, this,
                &FluentMenuItem::updateFromAction);
        updateFromAction();
    }
}

void FluentMenuItem::updateFromAction() {
    if (!m_action)
        return;

    setEnabled(m_action->isEnabled());
    setVisible(m_action->isVisible());
    update();
}

void FluentMenuItem::setItemType(FluentMenuItemType type) {
    if (m_itemType == type)
        return;

    m_itemType = type;
    update();
}

void FluentMenuItem::setItemState(FluentMenuItemState state) {
    if (m_itemState == state)
        return;

    FluentMenuItemState oldState = m_itemState;
    m_itemState = state;

    // Trigger hover animation if enabled
    if (m_hoverAnimationEnabled && (state == FluentMenuItemState::Hovered ||
                                    oldState == FluentMenuItemState::Hovered)) {
        startHoverAnimation(state == FluentMenuItemState::Hovered);
    }

    update();
}

void FluentMenuItem::setRadioGroup(const QString& groupName) {
    m_radioGroup = groupName;
}

void FluentMenuItem::setHoverAnimationEnabled(bool enabled) {
    m_hoverAnimationEnabled = enabled;
}

void FluentMenuItem::setupHoverAnimations() {
    // Configure hover opacity animation
    m_hoverAnimation->setDuration(150);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Configure background color animation
    m_backgroundAnimation->setDuration(200);
    m_backgroundAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Configure focus animation
    m_focusAnimation->setDuration(200);
    m_focusAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Initialize colors from theme
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();
    m_currentBackgroundColor = palette.neutralLightest;
}

void FluentMenuItem::startHoverAnimation(bool hovered) {
    if (!m_hoverAnimationEnabled)
        return;

    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    if (hovered) {
        // Animate to hover state
        m_hoverAnimation->setStartValue(m_hoverOpacity);
        m_hoverAnimation->setEndValue(1.0);

        m_backgroundAnimation->setStartValue(m_currentBackgroundColor);
        m_backgroundAnimation->setEndValue(palette.accent.lighter(150));
    } else {
        // Animate to normal state
        m_hoverAnimation->setStartValue(m_hoverOpacity);
        m_hoverAnimation->setEndValue(0.0);

        m_backgroundAnimation->setStartValue(m_currentBackgroundColor);
        m_backgroundAnimation->setEndValue(palette.neutralLightest);
    }

    m_hoverAnimation->start();
    m_backgroundAnimation->start();
}

void FluentMenuItem::setHoverOpacity(qreal opacity) {
    if (qFuzzyCompare(m_hoverOpacity, opacity))
        return;

    m_hoverOpacity = opacity;
    update();
}

void FluentMenuItem::setCurrentBackgroundColor(const QColor& color) {
    if (m_currentBackgroundColor == color)
        return;

    m_currentBackgroundColor = color;
    update();
}

void FluentMenuItem::setFocusOpacity(qreal opacity) {
    if (qFuzzyCompare(m_focusOpacity, opacity))
        return;

    m_focusOpacity = opacity;
    update();
}

void FluentMenuItem::setShowFocusIndicator(bool show) {
    if (m_showFocusIndicator == show)
        return;

    m_showFocusIndicator = show;
    update();
}

void FluentMenuItem::startFocusAnimation(bool focused) {
    if (!m_showFocusIndicator)
        return;

    if (focused) {
        m_focusAnimation->setStartValue(m_focusOpacity);
        m_focusAnimation->setEndValue(1.0);
    } else {
        m_focusAnimation->setStartValue(m_focusOpacity);
        m_focusAnimation->setEndValue(0.0);
    }

    m_focusAnimation->start();
}

// Event handlers for FluentMenuItem
void FluentMenuItem::enterEvent(QEnterEvent* event) {
    QWidget::enterEvent(event);
    m_hovered = true;
    setItemState(FluentMenuItemState::Hovered);
    emit hovered();
}

void FluentMenuItem::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
    m_hovered = false;
    setItemState(hasFocus() ? FluentMenuItemState::Focused
                            : FluentMenuItemState::Normal);
}

void FluentMenuItem::focusInEvent(QFocusEvent* event) {
    QWidget::focusInEvent(event);
    setItemState(FluentMenuItemState::Focused);
    startFocusAnimation(true);
}

void FluentMenuItem::focusOutEvent(QFocusEvent* event) {
    QWidget::focusOutEvent(event);
    setItemState(m_hovered ? FluentMenuItemState::Hovered
                           : FluentMenuItemState::Normal);
    startFocusAnimation(false);
}

void FluentMenuItem::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter ||
        event->key() == Qt::Key_Space) {
        if (m_action && m_action->isEnabled()) {
            m_action->trigger();
            emit triggered();
        }
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

void FluentMenuItem::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        setItemState(FluentMenuItemState::Pressed);
    }
}

void FluentMenuItem::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;

        if (rect().contains(event->pos()) && m_action &&
            m_action->isEnabled()) {
            m_action->trigger();
            emit triggered();
        }

        setItemState(m_hovered ? FluentMenuItemState::Hovered
                               : FluentMenuItemState::Normal);
    }
}

// Enhanced submenu methods for FluentContextMenu
void FluentContextMenu::setSubmenuDelay(int milliseconds) {
    m_submenuDelay = milliseconds;
}

void FluentContextMenu::setSubmenuOverlap(int pixels) {
    m_submenuOverlap = pixels;
}

void FluentContextMenu::showSubmenuAt(FluentContextMenu* submenu,
                                      const QPoint& position) {
    if (!submenu)
        return;

    // Calculate optimal position for submenu
    QPoint submenuPos = calculateSubmenuPosition(submenu, position);

    // Show submenu with animation
    submenu->popup(submenuPos);

    // Track active submenu
    if (!m_activeSubmenus.contains(submenu)) {
        m_activeSubmenus.append(submenu);
    }
}

void FluentContextMenu::hideAllSubmenus() {
    for (auto* submenu : m_activeSubmenus) {
        if (submenu && submenu->isVisible()) {
            submenu->hide();
        }
    }
    m_activeSubmenus.clear();
}

QList<FluentContextMenu*> FluentContextMenu::activeSubmenus() const {
    return m_activeSubmenus;
}

QPoint FluentContextMenu::calculateSubmenuPosition(FluentContextMenu* submenu,
                                                   const QPoint& triggerPos) {
    if (!submenu)
        return triggerPos;

    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    QSize submenuSize = submenu->sizeHint();
    QPoint position = triggerPos;

    // Try to position submenu to the right of the trigger point
    position.setX(triggerPos.x() + width() - m_submenuOverlap);

    // Check if submenu would go off-screen horizontally
    if (position.x() + submenuSize.width() > screenGeometry.right()) {
        // Position to the left instead
        position.setX(triggerPos.x() - submenuSize.width() + m_submenuOverlap);
    }

    // Ensure submenu doesn't go off-screen vertically
    if (position.y() + submenuSize.height() > screenGeometry.bottom()) {
        position.setY(screenGeometry.bottom() - submenuSize.height());
    }

    if (position.y() < screenGeometry.top()) {
        position.setY(screenGeometry.top());
    }

    return position;
}

// Missing FluentContextMenu methods
void FluentContextMenu::onSubMenuAboutToShow() {
    // Handle submenu about to show
}

void FluentContextMenu::onActionTriggered() {
    // Handle action triggered
}

void FluentContextMenu::onActionHovered() {
    // Handle action hovered
}

bool FluentContextMenu::eventFilter(QObject* obj, QEvent* event) {
    Q_UNUSED(obj)
    Q_UNUSED(event)
    return false;
}

void FluentContextMenu::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
}

void FluentContextMenu::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
}

void FluentContextMenu::mouseMoveEvent(QMouseEvent* event) {
    QWidget::mouseMoveEvent(event);
}

void FluentContextMenu::keyPressEvent(QKeyEvent* event) {
    QWidget::keyPressEvent(event);
}

void FluentContextMenu::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
}

void FluentContextMenu::setTitle(const QString& title) {
    Q_UNUSED(title)
    // Set menu title
}

void FluentContextMenu::hide() { QWidget::hide(); }

void FluentContextMenu::popup(const QPoint& pos) {
    move(pos);
    show();
}

// Missing FluentMenuItem methods
void FluentMenuItem::setEnabled(bool enabled) { QWidget::setEnabled(enabled); }

void FluentMenuItem::setChecked(bool checked) {
    Q_UNUSED(checked)
    // Set checked state
}

QSize FluentMenuItem::sizeHint() const { return QSize(200, 32); }

void FluentMenuItem::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    // Paint menu item
}

// Missing FluentMenuSeparator methods
FluentMenuSeparator::FluentMenuSeparator(FluentMenuSeparatorType type,
                                         QWidget* parent)
    : QWidget(parent), m_type(type) {
    setFixedHeight(type == FluentMenuSeparatorType::Line ? 8 : 24);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize FluentMenuSeparator::sizeHint() const {
    return QSize(200, m_type == FluentMenuSeparatorType::Line ? 8 : 24);
}

void FluentMenuSeparator::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    if (m_type == FluentMenuSeparatorType::Line) {
        // Draw separator line
        painter.setPen(QPen(palette.neutralQuaternary, 1));
        int y = height() / 2;
        painter.drawLine(8, y, width() - 8, y);
    } else {
        // Draw header text
        painter.setPen(palette.neutralSecondary);
        painter.setFont(theme.captionFont());
        painter.drawText(rect().adjusted(8, 0, -8, 0),
                         Qt::AlignLeft | Qt::AlignVCenter, m_text);
    }
}

void FluentMenuSeparator::setText(const QString& text) {
    m_text = text;
    update();
}

void FluentMenuSeparator::setSeparatorType(FluentMenuSeparatorType type) {
    if (m_type != type) {
        m_type = type;
        setFixedHeight(type == FluentMenuSeparatorType::Line ? 8 : 24);
        update();
    }
}

}  // namespace FluentQt::Components
