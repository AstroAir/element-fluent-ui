#include "FluentQt/Components/FluentSidebarItemWidget.h"
#include "FluentQt/Styling/FluentDesignTokenUtils.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QEnterEvent>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

// Design token macros for easier access
#define FLUENT_COLOR(name) \
    FluentQt::Styling::FluentDesignTokenUtils::instance().getColor(name)
#define FLUENT_FONT(name) \
    FluentQt::Styling::FluentDesignTokenUtils::instance().getFont(name)
#define FLUENT_SPACING(name) \
    FluentQt::Styling::FluentDesignTokenUtils::instance().getSpacing(name)

namespace FluentQt::Components {

FluentSidebarItemWidget::FluentSidebarItemWidget(const FluentSidebarItem& item,
                                                 QWidget* parent)
    : QToolButton(parent), m_item(item) {
    setupWidget();
}

void FluentSidebarItemWidget::setupWidget() {
    setText(m_item.text);
    setIcon(m_item.icon);
    setToolTip(m_item.tooltip.isEmpty() ? m_item.text : m_item.tooltip);
    setEnabled(m_item.enabled);
    setCheckable(m_item.selectable);
    setAutoRaise(true);
    setFocusPolicy(Qt::StrongFocus);

    // Set object name for styling
    setObjectName("FluentSidebarItemWidget");

    updateAccessibility();
    updateLayout();
    refreshDesignTokens();
}

void FluentSidebarItemWidget::updateItem(const FluentSidebarItem& item) {
    m_item = item;
    setupWidget();
    update();
}

void FluentSidebarItemWidget::setCompactMode(bool compact) {
    if (m_compactMode != compact) {
        m_compactMode = compact;
        updateLayout();
        update();
    }
}

void FluentSidebarItemWidget::setState(FluentSidebarItemState state) {
    if (m_currentState != state) {
        FluentSidebarItemState oldState = m_currentState;
        m_currentState = state;
        transitionToState(state);
        emit itemStateChanged(m_item.id, state);
        update();
    }
}

void FluentSidebarItemWidget::setBadge(FluentSidebarBadgeType type, int count,
                                       const QString& text) {
    m_badgeType = type;
    m_badgeCount = count;
    m_badgeText = text;
    update();
}

void FluentSidebarItemWidget::refreshDesignTokens() {
    if (!m_useFluentDesignTokens)
        return;

    m_cacheValid = false;
    m_colorCache.clear();
    m_fontCache.clear();
    m_spacingCache.clear();

    // Pre-cache commonly used tokens
    m_colorCache["background.rest"] = getStateColor("color.background.primary");
    m_colorCache["background.hover"] = getStateColor("color.background.hover");
    m_colorCache["background.pressed"] =
        getStateColor("color.background.pressed");
    m_colorCache["background.selected"] = getStateColor("brand.100");
    m_colorCache["text.primary"] = getStateColor("color.text.primary");
    m_colorCache["text.secondary"] = getStateColor("color.text.secondary");

    m_fontCache["body"] = getStateFont("typography.body1");
    m_fontCache["caption"] = getStateFont("typography.caption1");

    m_spacingCache["xs"] = getStateSpacing("spacing.xs");
    m_spacingCache["s"] = getStateSpacing("spacing.s");
    m_spacingCache["m"] = getStateSpacing("spacing.m");

    m_cacheValid = true;
    updateLayout();
}

void FluentSidebarItemWidget::updateLayout() {
    if (m_compactMode) {
        // Compact mode: icon only
        const int size = m_useFluentDesignTokens
                             ? getStateSpacing("control.size.compact")
                             : 48;
        const int iconSize =
            m_useFluentDesignTokens ? getStateSpacing("icon.size.medium") : 20;

        setFixedSize(size, size);
        setIconSize(QSize(iconSize, iconSize));
        setToolButtonStyle(Qt::ToolButtonIconOnly);
    } else {
        // Expanded mode: icon and text
        const int height = m_useFluentDesignTokens
                               ? getStateSpacing("control.height.medium")
                               : 32;
        const int iconSize =
            m_useFluentDesignTokens ? getStateSpacing("icon.size.small") : 16;
        const int minWidth = m_useFluentDesignTokens
                                 ? getStateSpacing("navigation.item.min.width")
                                 : 200;

        setFixedHeight(height);
        setMinimumWidth(minWidth);
        setIconSize(QSize(iconSize, iconSize));
        setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }

    // Set font
    if (m_useFluentDesignTokens) {
        setFont(getStateFont("typography.body1"));
    }
}

void FluentSidebarItemWidget::updateAccessibility() {
    setAccessibleName(m_item.accessibleName.isEmpty() ? m_item.text
                                                      : m_item.accessibleName);
    setAccessibleDescription(m_item.accessibleDescription);

    // Set ARIA role
    if (m_item.ariaRole == "menuitem") {
        // This is a menu item
    } else if (m_item.ariaRole == "button") {
        // This is a button
    }
}

QSize FluentSidebarItemWidget::sizeHint() const {
    if (m_compactMode) {
        const int size = m_useFluentDesignTokens
                             ? getStateSpacing("control.size.compact")
                             : 48;
        return QSize(size, size);
    } else {
        const int height = m_useFluentDesignTokens
                               ? getStateSpacing("control.height.medium")
                               : 32;
        const int minWidth = m_useFluentDesignTokens
                                 ? getStateSpacing("navigation.item.min.width")
                                 : 200;
        return QSize(minWidth, height);
    }
}

QSize FluentSidebarItemWidget::minimumSizeHint() const { return sizeHint(); }

// Event handling
void FluentSidebarItemWidget::mousePressEvent(QMouseEvent* event) {
    QToolButton::mousePressEvent(event);
    if (isEnabled()) {
        m_isPressed = true;
        setState(FluentSidebarItemState::Pressed);
        emit itemClicked(m_item.id);
    }
}

void FluentSidebarItemWidget::mouseReleaseEvent(QMouseEvent* event) {
    QToolButton::mouseReleaseEvent(event);
    m_isPressed = false;
    if (underMouse()) {
        setState(FluentSidebarItemState::Hover);
    } else {
        setState(isChecked() ? FluentSidebarItemState::Selected
                             : FluentSidebarItemState::Normal);
    }
}

void FluentSidebarItemWidget::enterEvent(QEnterEvent* event) {
    QToolButton::enterEvent(event);
    if (isEnabled() && !m_isPressed) {
        m_isHovered = true;
        setState(FluentSidebarItemState::Hover);
        emit itemHovered(m_item.id);
    }
}

void FluentSidebarItemWidget::leaveEvent(QEvent* event) {
    QToolButton::leaveEvent(event);
    m_isHovered = false;
    if (!m_isPressed) {
        setState(isChecked() ? FluentSidebarItemState::Selected
                             : FluentSidebarItemState::Normal);
    }
}

void FluentSidebarItemWidget::focusInEvent(QFocusEvent* event) {
    QToolButton::focusInEvent(event);
    m_hasFocus = true;
    emit itemFocused(m_item.id);
    update();
}

void FluentSidebarItemWidget::focusOutEvent(QFocusEvent* event) {
    QToolButton::focusOutEvent(event);
    m_hasFocus = false;
    update();
}

void FluentSidebarItemWidget::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Space:
            if (isEnabled()) {
                emit itemClicked(m_item.id);
                event->accept();
                return;
            }
            break;
        default:
            break;
    }
    QToolButton::keyPressEvent(event);
}

// Helper methods
QColor FluentSidebarItemWidget::getStateColor(const QString& colorName) const {
    if (!m_useFluentDesignTokens) {
        // Fallback colors
        static const QHash<QString, QColor> fallbackColors = {
            {"color.background.primary", QColor("#ffffff")},
            {"color.background.hover", QColor("#f3f2f1")},
            {"color.background.pressed", QColor("#edebe9")},
            {"brand.100", QColor("#0078d4")},
            {"color.text.primary", QColor("#323130")},
            {"color.text.secondary", QColor("#605e5c")}};
        return fallbackColors.value(colorName, QColor("#000000"));
    }

    if (m_cacheValid && m_colorCache.contains(colorName)) {
        return m_colorCache[colorName];
    }

    return FLUENT_COLOR(colorName);
}

QFont FluentSidebarItemWidget::getStateFont(const QString& fontName) const {
    if (!m_useFluentDesignTokens) {
        return QApplication::font();
    }

    if (m_cacheValid && m_fontCache.contains(fontName)) {
        return m_fontCache[fontName];
    }

    return FLUENT_FONT(fontName);
}

int FluentSidebarItemWidget::getStateSpacing(const QString& spacingName) const {
    if (!m_useFluentDesignTokens) {
        // Fallback spacing values
        static const QHash<QString, int> fallbackSpacing = {
            {"spacing.xs", 4},
            {"spacing.s", 8},
            {"spacing.m", 12},
            {"control.size.compact", 48},
            {"control.height.medium", 32},
            {"icon.size.small", 16},
            {"icon.size.medium", 20},
            {"navigation.item.min.width", 200},
            {"border.radius.small", 4}};
        return fallbackSpacing.value(spacingName, 8);
    }

    if (m_cacheValid && m_spacingCache.contains(spacingName)) {
        return m_spacingCache[spacingName];
    }

    return FLUENT_SPACING(spacingName);
}

void FluentSidebarItemWidget::transitionToState(
    FluentSidebarItemState newState) {
    // Update visual state
    updateStateColors();

    // Future: Add smooth transitions between states
    // This could include color animations, scale effects, etc.
}

void FluentSidebarItemWidget::updateStateColors() {
    // Colors will be applied in paintEvent
    // This method can be extended for more complex state management
}

void FluentSidebarItemWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const QRect rect = this->rect();

    // Paint background and selection indicator
    paintBackground(&painter, rect);

    // Paint content (icon, text, badge)
    paintContent(&painter, rect);

    // Paint focus indicator if needed
    if (m_hasFocus && m_focusIndicatorVisible) {
        paintFocusIndicator(&painter, rect);
    }
}

void FluentSidebarItemWidget::paintBackground(QPainter* painter,
                                              const QRect& rect) {
    QColor backgroundColor;

    // Determine background color based on state
    switch (m_currentState) {
        case FluentSidebarItemState::Selected:
            backgroundColor = getStateColor("brand.100").lighter(180);
            break;
        case FluentSidebarItemState::Hover:
            backgroundColor = getStateColor("color.background.hover");
            break;
        case FluentSidebarItemState::Pressed:
            backgroundColor = getStateColor("color.background.pressed");
            break;
        case FluentSidebarItemState::Disabled:
            backgroundColor = QColor(0, 0, 0, 0);  // Transparent
            break;
        default:
            backgroundColor = QColor(0, 0, 0, 0);  // Transparent
            break;
    }

    // Paint background with rounded corners
    if (backgroundColor.alpha() > 0) {
        const int borderRadius = getBorderRadius();
        QPainterPath backgroundPath;
        backgroundPath.addRoundedRect(rect.adjusted(2, 1, -2, -1), borderRadius,
                                      borderRadius);
        painter->fillPath(backgroundPath, backgroundColor);
    }

    // Paint selection indicator for selected items
    if (m_currentState == FluentSidebarItemState::Selected || isChecked()) {
        const QColor accentColor = getStateColor("brand.100");
        const int indicatorWidth = 3;
        const int indicatorHeight = rect.height() - 8;
        const int indicatorY = (rect.height() - indicatorHeight) / 2;

        QRect indicatorRect(0, indicatorY, indicatorWidth, indicatorHeight);
        QPainterPath indicatorPath;
        indicatorPath.addRoundedRect(indicatorRect, 2, 2);
        painter->fillPath(indicatorPath, accentColor);
    }
}

void FluentSidebarItemWidget::paintContent(QPainter* painter,
                                           const QRect& rect) {
    if (m_compactMode) {
        // Compact mode: center icon only
        paintIcon(painter, getIconRect(rect));
    } else {
        // Expanded mode: icon, text, and badge
        paintIcon(painter, getIconRect(rect));
        paintText(painter, getTextRect(rect));

        if (m_badgeType != FluentSidebarBadgeType::None) {
            paintBadge(painter, getBadgeRect(rect));
        }
    }
}

void FluentSidebarItemWidget::paintIcon(QPainter* painter,
                                        const QRect& iconRect) {
    if (icon().isNull())
        return;

    QIcon::Mode mode = QIcon::Normal;
    if (!isEnabled()) {
        mode = QIcon::Disabled;
    } else if (m_currentState == FluentSidebarItemState::Selected) {
        mode = QIcon::Selected;
    }

    QIcon::State state = isChecked() ? QIcon::On : QIcon::Off;
    icon().paint(painter, iconRect, Qt::AlignCenter, mode, state);
}

void FluentSidebarItemWidget::paintText(QPainter* painter,
                                        const QRect& textRect) {
    if (text().isEmpty())
        return;

    // Set text color based on state
    QColor textColor = getStateColor("color.text.primary");
    if (!isEnabled()) {
        textColor = getStateColor("color.text.secondary");
    }

    painter->setPen(textColor);
    painter->setFont(getStateFont("typography.body1"));

    // Draw primary text
    QRect primaryTextRect = textRect;
    if (!m_item.description.isEmpty()) {
        primaryTextRect.setHeight(textRect.height() / 2);
    }

    painter->drawText(primaryTextRect, Qt::AlignLeft | Qt::AlignVCenter,
                      text());

    // Draw description text if available
    if (!m_item.description.isEmpty()) {
        QRect descriptionRect = textRect;
        descriptionRect.setTop(primaryTextRect.bottom());

        painter->setPen(getStateColor("color.text.secondary"));
        painter->setFont(getStateFont("typography.caption1"));
        painter->drawText(descriptionRect, Qt::AlignLeft | Qt::AlignVCenter,
                          m_item.description);
    }
}

void FluentSidebarItemWidget::paintBadge(QPainter* painter,
                                         const QRect& badgeRect) {
    if (m_badgeType == FluentSidebarBadgeType::None)
        return;

    QColor badgeColor =
        m_badgeColor.isValid() ? m_badgeColor : getStateColor("brand.100");
    QString badgeText;

    switch (m_badgeType) {
        case FluentSidebarBadgeType::Dot:
            // Paint a small dot
            painter->setBrush(badgeColor);
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(badgeRect.center(), 4, 4);
            return;
        case FluentSidebarBadgeType::Count:
            badgeText = QString::number(m_badgeCount);
            break;
        case FluentSidebarBadgeType::Custom:
            badgeText = m_badgeText;
            break;
        default:
            return;
    }

    // Paint badge background
    painter->setBrush(badgeColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(badgeRect, 8, 8);

    // Paint badge text
    painter->setPen(Qt::white);
    painter->setFont(getStateFont("typography.caption1"));
    painter->drawText(badgeRect, Qt::AlignCenter, badgeText);
}

void FluentSidebarItemWidget::paintFocusIndicator(QPainter* painter,
                                                  const QRect& rect) {
    const QColor focusColor = getStateColor("brand.100");
    const int borderRadius = getBorderRadius();

    painter->setPen(QPen(focusColor, 2));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(rect.adjusted(1, 1, -1, -1), borderRadius,
                             borderRadius);
}

// Layout calculation methods
QRect FluentSidebarItemWidget::getIconRect(const QRect& contentRect) const {
    if (m_compactMode) {
        // Center icon in compact mode
        const int iconSize = getStateSpacing("icon.size.medium");
        const int x = (contentRect.width() - iconSize) / 2;
        const int y = (contentRect.height() - iconSize) / 2;
        return QRect(x, y, iconSize, iconSize);
    } else {
        // Left-aligned icon in expanded mode
        const int iconSize = getStateSpacing("icon.size.small");
        const int margin = getStateSpacing("spacing.m");
        const int y = (contentRect.height() - iconSize) / 2;
        return QRect(margin, y, iconSize, iconSize);
    }
}

QRect FluentSidebarItemWidget::getTextRect(const QRect& contentRect) const {
    if (m_compactMode)
        return QRect();  // No text in compact mode

    const int iconSize = getStateSpacing("icon.size.small");
    const int margin = getStateSpacing("spacing.m");
    const int spacing = getStateSpacing("spacing.s");

    const int x = margin + iconSize + spacing;
    const int width =
        contentRect.width() - x - margin - 20;  // Reserve space for badge

    return QRect(x, 0, width, contentRect.height());
}

QRect FluentSidebarItemWidget::getBadgeRect(const QRect& contentRect) const {
    if (m_compactMode) {
        // Top-right corner in compact mode
        return QRect(contentRect.width() - 16, 4, 12, 12);
    } else {
        // Right side in expanded mode
        const int margin = getStateSpacing("spacing.s");
        const int badgeWidth = 16;
        const int badgeHeight = 16;
        const int x = contentRect.width() - margin - badgeWidth;
        const int y = (contentRect.height() - badgeHeight) / 2;
        return QRect(x, y, badgeWidth, badgeHeight);
    }
}

int FluentSidebarItemWidget::getBorderRadius() const {
    return getStateSpacing("border.radius.small");
}

}  // namespace FluentQt::Components

#include "FluentSidebarItemWidget.moc"
