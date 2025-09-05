#include "FluentQt/Components/FluentSidebar.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentSidebarItemWidget.h"
#include "FluentQt/Styling/FluentDesignTokenUtils.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QHash>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QToolButton>

// Design token macros for easier access
#define FLUENT_COLOR(name) \
    FluentQt::Styling::FluentDesignTokenUtils::instance().getColor(name)
#define FLUENT_FONT(name) \
    FluentQt::Styling::FluentDesignTokenUtils::instance().getFont(name)
#define FLUENT_SPACING(name) \
    FluentQt::Styling::FluentDesignTokenUtils::instance().getSpacing(name)
#define FLUENT_SIZE(name) \
    FluentQt::Styling::FluentDesignTokenUtils::instance().getSize(name)

namespace FluentQt::Components {

// FluentSidebar implementation
FluentSidebar::FluentSidebar(QWidget* parent) : QWidget(parent) {
    // Initialize with Fluent UI design tokens
    if (m_useFluentDesignTokens) {
        m_expandedWidth = FLUENT_SIZE("navigation.expanded.width");
        m_compactWidth = FLUENT_SIZE("navigation.compact.width");
        m_currentWidth = m_expandedWidth;
    }

    setupUI();
    setupAnimations();
    setupAccessibility();
    setupKeyboardNavigation();

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentSidebar::updateItemStyles);

    // Setup responsive timer
    m_responsiveTimer = new QTimer(this);
    m_responsiveTimer->setSingleShot(true);
    m_responsiveTimer->setInterval(100);
    connect(m_responsiveTimer, &QTimer::timeout, this,
            &FluentSidebar::updateResponsiveMode);

    // Setup typeahead timer
    m_typeaheadTimer = new QTimer(this);
    m_typeaheadTimer->setSingleShot(true);
    m_typeaheadTimer->setInterval(1000);
    connect(m_typeaheadTimer, &QTimer::timeout, this,
            &FluentSidebar::clearTypeaheadBuffer);

    // Apply Fluent UI design tokens
    if (m_useFluentDesignTokens) {
        applyFluentDesignTokens();
    }

    updateLayout();
}

FluentSidebar::~FluentSidebar() = default;

void FluentSidebar::setMode(FluentSidebarMode mode) {
    if (m_mode != mode && !m_animating) {
        m_previousMode = m_mode;
        m_mode = mode;
        animateToMode(mode);
        emit modeChanged(mode);
    }
}

void FluentSidebar::setExpandedWidth(int width) {
    if (m_expandedWidth != width) {
        m_expandedWidth = width;
        if (m_mode == FluentSidebarMode::Expanded) {
            updateLayout();
        }
    }
}

void FluentSidebar::setCompactWidth(int width) {
    if (m_compactWidth != width) {
        m_compactWidth = width;
        if (m_mode == FluentSidebarMode::Compact) {
            updateLayout();
        }
    }
}

void FluentSidebar::setCollapsible(bool collapsible) {
    m_collapsible = collapsible;
}

void FluentSidebar::setAutoHide(bool autoHide) {
    m_autoHide = autoHide;
    if (autoHide) {
        checkResponsiveBreakpoints();
    }
}

void FluentSidebar::addItem(const FluentSidebarItem& item) {
    insertItem(m_items.size(), item);
}

void FluentSidebar::insertItem(int index, const FluentSidebarItem& item) {
    if (index < 0 || index > m_items.size()) {
        return;
    }

    m_items.insert(index, item);
    createItemWidget(item, index);
    updateLayout();
}

void FluentSidebar::removeItem(const QString& id) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == id) {
            removeItem(i);
            break;
        }
    }
}

void FluentSidebar::removeItem(int index) {
    if (index < 0 || index >= m_items.size()) {
        return;
    }

    m_items.removeAt(index);
    removeItemWidget(index);
    updateLayout();
}

void FluentSidebar::clearItems() {
    m_items.clear();
    qDeleteAll(m_itemWidgets);
    m_itemWidgets.clear();
    m_selectedItemId.clear();
    updateLayout();
}

FluentSidebarItem* FluentSidebar::findItem(const QString& id) {
    for (auto& item : m_items) {
        if (item.id == id) {
            return &item;
        }
    }
    return nullptr;
}

int FluentSidebar::itemCount() const { return m_items.size(); }

void FluentSidebar::setSelectedItem(const QString& id) {
    if (m_selectedItemId != id) {
        // Deselect previous item
        for (int i = 0; i < m_itemWidgets.size(); ++i) {
            if (auto* itemWidget =
                    qobject_cast<FluentSidebarItemWidget*>(m_itemWidgets[i])) {
                itemWidget->setChecked(itemWidget->item().id == id);
            }
        }

        m_selectedItemId = id;
        emit itemSelected(id);

        // Announce selection change for accessibility
        if (auto* item = findItem(id)) {
            announceStateChange(tr("Selected %1").arg(item->text));
        }
    }
}

void FluentSidebar::clearSelection() { setSelectedItem(QString()); }

void FluentSidebar::setupUI() {
    // Set initial width using design tokens
    setFixedWidth(m_currentWidth);

    // Apply Fluent UI styling
    setObjectName("FluentSidebar");
    setAttribute(Qt::WA_StyledBackground, true);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Create scroll area for content with Fluent UI styling
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setObjectName("FluentSidebarScrollArea");

    // Apply Fluent UI scrollbar styling
    if (m_useFluentDesignTokens) {
        m_scrollArea->setStyleSheet(
            QString("QScrollArea#FluentSidebarScrollArea { "
                    "    background-color: %1; "
                    "    border: none; "
                    "} "
                    "QScrollBar:vertical { "
                    "    background-color: transparent; "
                    "    width: %2px; "
                    "    border-radius: %3px; "
                    "} "
                    "QScrollBar::handle:vertical { "
                    "    background-color: %4; "
                    "    border-radius: %3px; "
                    "    min-height: 20px; "
                    "} "
                    "QScrollBar::handle:vertical:hover { "
                    "    background-color: %5; "
                    "}")
                .arg(getSemanticColor("color.background.primary").name())
                .arg(getSemanticSpacing("scrollbar.width"))
                .arg(getSemanticSpacing("border.radius.small"))
                .arg(getSemanticColor("color.scrollbar.thumb").name())
                .arg(getSemanticColor("color.scrollbar.thumb.hover").name()));
    }

    m_contentWidget = new QWidget();
    m_contentWidget->setObjectName("FluentSidebarContent");
    m_contentLayout = new QVBoxLayout(m_contentWidget);

    // Use Fluent UI spacing tokens
    const int contentPadding =
        m_useFluentDesignTokens ? getSemanticSpacing("spacing.s") : 4;
    const int itemSpacing =
        m_useFluentDesignTokens ? getSemanticSpacing("spacing.xs") : 2;

    m_contentLayout->setContentsMargins(contentPadding, contentPadding,
                                        contentPadding, contentPadding);
    m_contentLayout->setSpacing(itemSpacing);
    m_contentLayout->addStretch();

    m_scrollArea->setWidget(m_contentWidget);
    m_mainLayout->addWidget(m_scrollArea);

    // Add drop shadow effect for Fluent UI elevation
    if (m_useFluentDesignTokens) {
        auto* shadowEffect = new QGraphicsDropShadowEffect(this);
        shadowEffect->setBlurRadius(8);
        shadowEffect->setColor(QColor(0, 0, 0, 32));
        shadowEffect->setOffset(2, 0);
        setGraphicsEffect(shadowEffect);
    }
}

void FluentSidebar::setupAnimations() {
    m_widthAnimation = new QPropertyAnimation(this, "minimumWidth", this);
    m_widthAnimation->setDuration(m_animationDuration);
    m_widthAnimation->setEasingCurve(m_animationEasing);

    connect(m_widthAnimation, &QPropertyAnimation::finished, this,
            &FluentSidebar::onAnimationFinished);

    m_modeAnimation = new QParallelAnimationGroup(this);
    m_modeAnimation->addAnimation(m_widthAnimation);
}

void FluentSidebar::setupAccessibility() {
    // Set ARIA role for navigation
    setAccessibleName(m_accessibleName.isEmpty() ? tr("Navigation Sidebar")
                                                 : m_accessibleName);
    setAccessibleDescription(
        m_accessibleDescription.isEmpty()
            ? tr("Main navigation sidebar with keyboard support")
            : m_accessibleDescription);

    // Set focus policy for keyboard navigation
    setFocusPolicy(Qt::StrongFocus);

    // Enable tab focus
    setTabOrder(this, nullptr);

    // Set up accessibility properties
    updateAccessibilityProperties();
}

void FluentSidebar::setupKeyboardNavigation() {
    // Enable keyboard navigation by default
    m_keyboardNavigationEnabled = true;
    m_typeaheadSearchEnabled = true;

    // Install event filter for global keyboard handling
    installEventFilter(this);
}

void FluentSidebar::applyFluentDesignTokens() {
    if (!m_useFluentDesignTokens)
        return;

    // Cache design tokens for performance
    m_colorTokenCache.clear();
    m_fontTokenCache.clear();
    m_spacingTokenCache.clear();

    // Pre-cache commonly used tokens
    m_colorTokenCache["background.primary"] =
        FLUENT_COLOR("color.background.primary");
    m_colorTokenCache["text.primary"] = FLUENT_COLOR("color.text.primary");
    m_colorTokenCache["accent.primary"] = FLUENT_COLOR("brand.100");
    m_colorTokenCache["border.primary"] = FLUENT_COLOR("color.border.primary");

    m_fontTokenCache["body"] = FLUENT_FONT("typography.body1");
    m_fontTokenCache["caption"] = FLUENT_FONT("typography.caption1");

    m_spacingTokenCache["xs"] = FLUENT_SPACING("spacing.xs");
    m_spacingTokenCache["s"] = FLUENT_SPACING("spacing.s");
    m_spacingTokenCache["m"] = FLUENT_SPACING("spacing.m");
    m_spacingTokenCache["l"] = FLUENT_SPACING("spacing.l");

    m_tokenCacheValid = true;

    // Apply background color
    setStyleSheet(
        QString("FluentSidebar { background-color: %1; }")
            .arg(getSemanticColor("color.background.primary").name()));
}

void FluentSidebar::updateLayout() {
    int targetWidth = 0;
    bool compactMode = false;

    switch (m_mode) {
        case FluentSidebarMode::Expanded:
            targetWidth = m_expandedWidth;
            compactMode = false;
            break;
        case FluentSidebarMode::Compact:
            targetWidth = m_compactWidth;
            compactMode = true;
            break;
        case FluentSidebarMode::Hidden:
            targetWidth = 0;
            compactMode = true;
            break;
        case FluentSidebarMode::Overlay:
            targetWidth = m_expandedWidth;
            compactMode = false;
            break;
        case FluentSidebarMode::Auto:
            checkResponsiveBreakpoints();
            return;
    }

    setFixedWidth(targetWidth);
    setVisible(targetWidth > 0);

    // Update item widgets
    for (auto* widget : m_itemWidgets) {
        if (auto* itemWidget = qobject_cast<FluentSidebarItemWidget*>(widget)) {
            itemWidget->setCompactMode(compactMode);
        }
    }
}

void FluentSidebar::animateToMode(FluentSidebarMode newMode) {
    if (m_animating) {
        return;
    }

    m_animating = true;

    int targetWidth = 0;
    switch (newMode) {
        case FluentSidebarMode::Expanded:
            targetWidth = m_expandedWidth;
            break;
        case FluentSidebarMode::Compact:
            targetWidth = m_compactWidth;
            break;
        case FluentSidebarMode::Hidden:
            targetWidth = 0;
            break;
        case FluentSidebarMode::Overlay:
            targetWidth = m_expandedWidth;
            break;
        case FluentSidebarMode::Auto:
            checkResponsiveBreakpoints();
            m_animating = false;
            return;
    }

    m_widthAnimation->setStartValue(width());
    m_widthAnimation->setEndValue(targetWidth);
    m_widthAnimation->start();
}

void FluentSidebar::createItemWidget(const FluentSidebarItem& item, int index) {
    if (item.separator) {
        // Create separator
        auto* separator = new QFrame();
        separator->setFrameShape(QFrame::HLine);
        separator->setFixedHeight(1);
        m_contentLayout->insertWidget(index, separator);
        m_itemWidgets.insert(index, separator);
    } else if (item.customWidget) {
        // Use custom widget
        m_contentLayout->insertWidget(index, item.customWidget);
        m_itemWidgets.insert(index, item.customWidget);
    } else {
        // Create standard item widget
        auto* itemWidget = new FluentSidebarItemWidget(item, this);
        connect(itemWidget, &FluentSidebarItemWidget::itemClicked, this,
                &FluentSidebar::onItemClicked);

        m_contentLayout->insertWidget(index, itemWidget);
        m_itemWidgets.insert(index, itemWidget);
    }
}

void FluentSidebar::removeItemWidget(int index) {
    if (index >= 0 && index < m_itemWidgets.size()) {
        auto* widget = m_itemWidgets.takeAt(index);
        m_contentLayout->removeWidget(widget);
        widget->deleteLater();
    }
}

void FluentSidebar::onItemClicked() {
    if (auto* itemWidget = qobject_cast<FluentSidebarItemWidget*>(sender())) {
        const QString& id = itemWidget->item().id;
        setSelectedItem(id);
        emit itemClicked(id);

        // Execute callback if provided
        if (itemWidget->item().onClicked) {
            itemWidget->item().onClicked();
        }
    }
}

void FluentSidebar::onAnimationFinished() {
    m_animating = false;
    updateLayout();

    switch (m_mode) {
        case FluentSidebarMode::Expanded:
            emit expanded();
            break;
        case FluentSidebarMode::Compact:
        case FluentSidebarMode::Hidden:
            emit collapsed();
            break;
        case FluentSidebarMode::Overlay:
            emit overlayShown();
            break;
        default:
            break;
    }
}

void FluentSidebar::checkResponsiveBreakpoints() {
    if (!m_responsiveEnabled || !parentWidget()) {
        return;
    }

    const int parentWidth = parentWidget()->width();
    FluentSidebarMode newMode = m_mode;

    if (parentWidth <= m_hiddenBreakpoint) {
        newMode = FluentSidebarMode::Hidden;
    } else if (parentWidth <= m_compactBreakpoint) {
        newMode = FluentSidebarMode::Compact;
    } else {
        newMode = FluentSidebarMode::Expanded;
    }

    if (newMode != m_mode) {
        setMode(newMode);
    }
}

void FluentSidebar::announceStateChange(const QString& message) {
    // Announce state change for screen readers
    if (QAccessible::isActive()) {
        QAccessibleEvent event(this, QAccessible::NameChanged);
        QAccessible::updateAccessibility(&event);
    }

    // Emit signal for external accessibility handling
    emit accessibilityAnnouncement(message);
}

// Design token helper methods
QColor FluentSidebar::getSemanticColor(const QString& colorName) const {
    if (!m_useFluentDesignTokens) {
        // Fallback colors when design tokens are disabled
        static const QHash<QString, QColor> fallbackColors = {
            {"color.background.primary", QColor("#ffffff")},
            {"color.text.primary", QColor("#323130")},
            {"brand.100", QColor("#0078d4")},
            {"color.border.primary", QColor("#e1dfdd")},
            {"color.scrollbar.thumb", QColor("#c8c6c4")},
            {"color.scrollbar.thumb.hover", QColor("#a19f9d")}};
        return fallbackColors.value(colorName, QColor("#000000"));
    }

    if (m_tokenCacheValid && m_colorTokenCache.contains(colorName)) {
        return m_colorTokenCache[colorName];
    }

    return FLUENT_COLOR(colorName);
}

QFont FluentSidebar::getSemanticFont(const QString& fontName) const {
    if (!m_useFluentDesignTokens) {
        return QApplication::font();
    }

    if (m_tokenCacheValid && m_fontTokenCache.contains(fontName)) {
        return m_fontTokenCache[fontName];
    }

    return FLUENT_FONT(fontName);
}

int FluentSidebar::getSemanticSpacing(const QString& spacingName) const {
    if (!m_useFluentDesignTokens) {
        // Fallback spacing values
        static const QHash<QString, int> fallbackSpacing = {
            {"spacing.xs", 4},       {"spacing.s", 8},
            {"spacing.m", 12},       {"spacing.l", 16},
            {"scrollbar.width", 12}, {"border.radius.small", 4}};
        return fallbackSpacing.value(spacingName, 8);
    }

    if (m_tokenCacheValid && m_spacingTokenCache.contains(spacingName)) {
        return m_spacingTokenCache[spacingName];
    }

    return FLUENT_SPACING(spacingName);
}

// Public slots implementation
void FluentSidebar::expand() { setMode(FluentSidebarMode::Expanded); }

void FluentSidebar::collapse() { setMode(FluentSidebarMode::Compact); }

void FluentSidebar::toggle() {
    if (m_mode == FluentSidebarMode::Expanded) {
        collapse();
    } else {
        expand();
    }
}

void FluentSidebar::showOverlay() { setMode(FluentSidebarMode::Overlay); }

void FluentSidebar::hideOverlay() { setMode(FluentSidebarMode::Hidden); }

// Event handlers
void FluentSidebar::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    if (m_responsiveEnabled && parentWidget()) {
        const QSize newParentSize = parentWidget()->size();
        if (newParentSize != m_lastParentSize) {
            m_lastParentSize = newParentSize;
            m_responsiveTimer->start();
        }
    }
}

void FluentSidebar::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_Down: {
            // Navigate between items
            int currentIndex = -1;
            for (int i = 0; i < m_itemWidgets.size(); ++i) {
                if (auto* itemWidget = qobject_cast<FluentSidebarItemWidget*>(
                        m_itemWidgets[i])) {
                    if (itemWidget->item().id == m_selectedItemId) {
                        currentIndex = i;
                        break;
                    }
                }
            }

            int direction = (event->key() == Qt::Key_Up) ? -1 : 1;
            int newIndex = currentIndex + direction;

            // Find next valid item
            while (newIndex >= 0 && newIndex < m_itemWidgets.size()) {
                if (auto* itemWidget = qobject_cast<FluentSidebarItemWidget*>(
                        m_itemWidgets[newIndex])) {
                    if (itemWidget->isEnabled()) {
                        setSelectedItem(itemWidget->item().id);
                        itemWidget->setFocus();
                        break;
                    }
                }
                newIndex += direction;
            }

            event->accept();
            break;
        }

        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Space: {
            // Activate selected item
            if (!m_selectedItemId.isEmpty()) {
                emit itemClicked(m_selectedItemId);
                if (auto* item = findItem(m_selectedItemId)) {
                    if (item->onClicked) {
                        item->onClicked();
                    }
                }
            }
            event->accept();
            break;
        }

        case Qt::Key_Escape: {
            // Close overlay mode
            if (m_mode == FluentSidebarMode::Overlay) {
                hideOverlay();
            }
            event->accept();
            break;
        }

        default:
            QWidget::keyPressEvent(event);
            break;
    }
}

void FluentSidebar::focusInEvent(QFocusEvent* event) {
    QWidget::focusInEvent(event);

    // If no item is selected, select the first enabled item
    if (m_selectedItemId.isEmpty() && !m_itemWidgets.isEmpty()) {
        for (auto* widget : m_itemWidgets) {
            if (auto* itemWidget =
                    qobject_cast<FluentSidebarItemWidget*>(widget)) {
                if (itemWidget->isEnabled()) {
                    setSelectedItem(itemWidget->item().id);
                    break;
                }
            }
        }
    }
}

void FluentSidebar::focusOutEvent(QFocusEvent* event) {
    QWidget::focusOutEvent(event);
}

void FluentSidebar::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const QRect rect = this->rect();

    if (m_useFluentDesignTokens) {
        // Use Fluent UI design tokens for styling
        const QColor backgroundColor =
            getSemanticColor("color.background.primary");
        const QColor borderColor = getSemanticColor("color.border.primary");
        const int borderRadius = getSemanticSpacing("border.radius.small");

        // Paint background with rounded corners
        QPainterPath backgroundPath;
        backgroundPath.addRoundedRect(rect, borderRadius, borderRadius);
        painter.fillPath(backgroundPath, backgroundColor);

        // Paint border (only on the right side for left sidebar)
        if (m_mode != FluentSidebarMode::Hidden) {
            painter.setPen(QPen(borderColor, 1));
            painter.drawLine(rect.topRight(), rect.bottomRight());
        }

        // Paint focus indicator if focused
        if (hasFocus() && m_focusIndicatorVisible) {
            const QColor focusColor = getSemanticColor("brand.100");
            painter.setPen(QPen(focusColor, 2));
            painter.drawRoundedRect(rect.adjusted(1, 1, -1, -1), borderRadius,
                                    borderRadius);
        }
    } else {
        // Fallback styling without design tokens
        const auto& theme = Styling::FluentTheme::instance();
        const auto& palette = theme.currentPalette();

        // Paint background
        painter.fillRect(rect, palette.background);

        // Paint border
        if (m_mode != FluentSidebarMode::Hidden) {
            painter.setPen(QPen(palette.border, 1));
            painter.drawLine(rect.topRight(), rect.bottomRight());
        }
    }

    QWidget::paintEvent(event);
}

bool FluentSidebar::eventFilter(QObject* object, QEvent* event) {
    // Handle overlay mode clicks outside
    if (m_mode == FluentSidebarMode::Overlay &&
        event->type() == QEvent::MouseButtonPress) {
        if (!rect().contains(mapFromGlobal(QCursor::pos()))) {
            hideOverlay();
            return true;
        }
    }

    return QWidget::eventFilter(object, event);
}

// Additional slots
void FluentSidebar::onResponsiveTimer() { checkResponsiveBreakpoints(); }

void FluentSidebar::updateResponsiveMode() {
    if (m_mode == FluentSidebarMode::Auto) {
        checkResponsiveBreakpoints();
    }
}

void FluentSidebar::updateItemVisibility() {
    // Update visibility based on current mode
    for (auto* widget : m_itemWidgets) {
        widget->setVisible(m_mode != FluentSidebarMode::Hidden);
    }
}

void FluentSidebar::updateItemStyles() {
    // Update all item widgets with new theme
    for (auto* widget : m_itemWidgets) {
        if (auto* itemWidget = qobject_cast<FluentSidebarItemWidget*>(widget)) {
            itemWidget->update();
        }
    }
    update();
}

// Configuration methods
void FluentSidebar::setHeader(QWidget* header) {
    if (m_header) {
        m_mainLayout->removeWidget(m_header);
        m_header->deleteLater();
    }

    m_header = header;
    if (header) {
        m_mainLayout->insertWidget(0, header);
    }
}

void FluentSidebar::setFooter(QWidget* footer) {
    if (m_footer) {
        m_mainLayout->removeWidget(m_footer);
        m_footer->deleteLater();
    }

    m_footer = footer;
    if (footer) {
        m_mainLayout->addWidget(footer);
    }
}

void FluentSidebar::setResponsiveBreakpoints(int compactBreakpoint,
                                             int hiddenBreakpoint) {
    m_compactBreakpoint = compactBreakpoint;
    m_hiddenBreakpoint = hiddenBreakpoint;

    if (m_responsiveEnabled) {
        checkResponsiveBreakpoints();
    }
}

void FluentSidebar::enableResponsiveBehavior(bool enable) {
    m_responsiveEnabled = enable;
    if (enable) {
        checkResponsiveBreakpoints();
    }
}

void FluentSidebar::setAnimationDuration(int duration) {
    m_animationDuration = duration;
    if (m_widthAnimation) {
        m_widthAnimation->setDuration(duration);
    }
}

void FluentSidebar::setAnimationEasing(QEasingCurve::Type easing) {
    m_animationEasing = easing;
    if (m_widthAnimation) {
        m_widthAnimation->setEasingCurve(easing);
    }
}

void FluentSidebar::setAccessibleName(const QString& name) {
    m_accessibleName = name;
    QWidget::setAccessibleName(name);
}

void FluentSidebar::setAccessibleDescription(const QString& description) {
    m_accessibleDescription = description;
    QWidget::setAccessibleDescription(description);
}

// Enhanced API methods implementation
void FluentSidebar::updateItem(const QString& id,
                               const FluentSidebarItem& item) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].id == id) {
            m_items[i] = item;
            if (auto* itemWidget =
                    qobject_cast<FluentSidebarItemWidget*>(m_itemWidgets[i])) {
                itemWidget->updateItem(item);
            }
            m_cacheValid = false;
            break;
        }
    }
}

void FluentSidebar::setItemEnabled(const QString& id, bool enabled) {
    if (auto* item = findItem(id)) {
        item->enabled = enabled;
        updateItem(id, *item);
    }
}

void FluentSidebar::setItemText(const QString& id, const QString& text) {
    if (auto* item = findItem(id)) {
        item->text = text;
        updateItem(id, *item);
    }
}

void FluentSidebar::setItemBadge(const QString& id, FluentSidebarBadgeType type,
                                 int count) {
    if (auto* item = findItem(id)) {
        item->badgeType = type;
        item->badgeCount = count;
        updateItem(id, *item);
        emit itemBadgeChanged(id, type, count);
    }
}

void FluentSidebar::setUseFluentDesignTokens(bool use) {
    if (m_useFluentDesignTokens != use) {
        m_useFluentDesignTokens = use;
        m_tokenCacheValid = false;
        if (use) {
            applyFluentDesignTokens();
        }
        update();
    }
}

void FluentSidebar::refreshDesignTokens() {
    m_tokenCacheValid = false;
    if (m_useFluentDesignTokens) {
        applyFluentDesignTokens();
    }
    update();
}

void FluentSidebar::announceToScreenReader(const QString& message) {
    announceStateChange(message);
}

void FluentSidebar::setFocusIndicatorVisible(bool visible) {
    if (m_focusIndicatorVisible != visible) {
        m_focusIndicatorVisible = visible;
        update();
    }
}

void FluentSidebar::setKeyboardNavigationEnabled(bool enabled) {
    m_keyboardNavigationEnabled = enabled;
}

void FluentSidebar::setTypeaheadSearchEnabled(bool enabled) {
    m_typeaheadSearchEnabled = enabled;
}

void FluentSidebar::updateAccessibilityProperties() {
    // Update accessibility properties for all items
    for (int i = 0; i < m_itemWidgets.size(); ++i) {
        updateItemAccessibility(i);
    }
}

void FluentSidebar::updateItemAccessibility(int index) {
    if (index < 0 || index >= m_itemWidgets.size())
        return;

    if (auto* itemWidget =
            qobject_cast<FluentSidebarItemWidget*>(m_itemWidgets[index])) {
        const auto& item = m_items[index];

        // Set ARIA properties
        itemWidget->setAccessibleName(
            item.accessibleName.isEmpty() ? item.text : item.accessibleName);
        itemWidget->setAccessibleDescription(item.accessibleDescription);

        // Set focus order
        if (index > 0) {
            if (auto* prevWidget = qobject_cast<FluentSidebarItemWidget*>(
                    m_itemWidgets[index - 1])) {
                setTabOrder(prevWidget, itemWidget);
            }
        }
    }
}

void FluentSidebar::clearTypeaheadBuffer() { m_typeaheadBuffer.clear(); }

}  // namespace FluentQt::Components

#include "FluentSidebar.moc"
