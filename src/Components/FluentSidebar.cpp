#include "FluentQt/Components/FluentSidebar.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QAccessible>
#include <QApplication>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QToolButton>

namespace FluentQt::Components {

/**
 * @brief Custom sidebar item widget
 */
class FluentSidebarItemWidget : public QToolButton {
    Q_OBJECT

public:
    explicit FluentSidebarItemWidget(const FluentSidebarItem& item,
                                     QWidget* parent = nullptr)
        : QToolButton(parent), m_item(item) {
        setupWidget();
    }

    void updateItem(const FluentSidebarItem& item) {
        m_item = item;
        setupWidget();
    }

    const FluentSidebarItem& item() const { return m_item; }

    void setCompactMode(bool compact) {
        if (m_compactMode != compact) {
            m_compactMode = compact;
            updateLayout();
        }
    }

    bool isCompactMode() const { return m_compactMode; }

signals:
    void itemClicked(const QString& id);

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QRect rect = this->rect();
        const auto& theme = Styling::FluentTheme::instance();
        const auto& palette = theme.currentPalette();

        // Paint background
        if (isChecked()) {
            painter.fillRect(rect, palette.accent.lighter(180));

            // Selection indicator
            QRect indicator(0, 0, 3, rect.height());
            painter.fillRect(indicator, palette.accent);
        } else if (underMouse() && isEnabled()) {
            painter.fillRect(rect, palette.neutralLight);
        }

        // Paint content
        paintContent(&painter, rect);
    }

    void mousePressEvent(QMouseEvent* event) override {
        QToolButton::mousePressEvent(event);
        if (isEnabled()) {
            emit itemClicked(m_item.id);
        }
    }

private:
    void setupWidget() {
        setText(m_item.text);
        setIcon(m_item.icon);
        setToolTip(m_item.tooltip.isEmpty() ? m_item.text : m_item.tooltip);
        setEnabled(m_item.enabled);
        setCheckable(true);
        setAutoRaise(true);

        // Accessibility
        setAccessibleName(m_item.accessibleName.isEmpty()
                              ? m_item.text
                              : m_item.accessibleName);
        setAccessibleDescription(m_item.accessibleDescription);

        updateLayout();
    }

    void updateLayout() {
        if (m_compactMode) {
            setFixedSize(48, 48);
            setIconSize(QSize(20, 20));
            setToolButtonStyle(Qt::ToolButtonIconOnly);
        } else {
            setFixedHeight(40);
            setMinimumWidth(200);
            setIconSize(QSize(16, 16));
            setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        }
    }

    void paintContent(QPainter* painter, const QRect& rect) {
        if (m_compactMode) {
            // Center icon in compact mode
            if (!icon().isNull()) {
                QIcon::Mode mode =
                    isEnabled() ? QIcon::Normal : QIcon::Disabled;
                QIcon::State state = isChecked() ? QIcon::On : QIcon::Off;
                QRect iconRect = rect.adjusted(14, 14, -14, -14);
                icon().paint(painter, iconRect, Qt::AlignCenter, mode, state);
            }
        } else {
            // Icon and text layout for expanded mode
            const int iconSize = 16;
            const int margin = 16;
            const int spacing = 12;

            QRect iconRect(margin, (rect.height() - iconSize) / 2, iconSize,
                           iconSize);
            QRect textRect(margin + iconSize + spacing, 0,
                           rect.width() - margin - iconSize - spacing - margin,
                           rect.height());

            // Draw icon
            if (!icon().isNull()) {
                QIcon::Mode mode =
                    isEnabled() ? QIcon::Normal : QIcon::Disabled;
                QIcon::State state = isChecked() ? QIcon::On : QIcon::Off;
                icon().paint(painter, iconRect, Qt::AlignCenter, mode, state);
            }

            // Draw text
            if (!text().isEmpty()) {
                const auto& theme = Styling::FluentTheme::instance();
                const auto& palette = theme.currentPalette();

                painter->setPen(isEnabled() ? palette.text
                                            : palette.textDisabled);
                painter->setFont(font());
                painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter,
                                  text());
            }
        }
    }

    FluentSidebarItem m_item;
    bool m_compactMode = false;
};

// FluentSidebar implementation
FluentSidebar::FluentSidebar(QWidget* parent) : QWidget(parent) {
    setupUI();
    setupAnimations();
    setupAccessibility();

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
    setFixedWidth(m_expandedWidth);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Create scroll area for content
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(2);
    m_contentLayout->addStretch();

    m_scrollArea->setWidget(m_contentWidget);
    m_mainLayout->addWidget(m_scrollArea);
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
    setAccessibleName(m_accessibleName.isEmpty() ? tr("Navigation Sidebar")
                                                 : m_accessibleName);
    setAccessibleDescription(m_accessibleDescription.isEmpty()
                                 ? tr("Main navigation sidebar")
                                 : m_accessibleDescription);
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

    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Paint background
    painter.fillRect(rect(), palette.background);

    // Paint border
    if (m_mode != FluentSidebarMode::Hidden) {
        painter.setPen(QPen(palette.border, 1));
        painter.drawLine(rect().topRight(), rect().bottomRight());
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

}  // namespace FluentQt::Components

#include "FluentSidebar.moc"
