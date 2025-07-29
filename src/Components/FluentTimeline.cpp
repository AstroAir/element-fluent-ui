// src/Components/FluentTimeline.cpp
#include "FluentQt/Components/FluentTimeline.h"
#include "FluentQt/Components/FluentTimelineItem.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QScrollBar>
#include <QApplication>
#include <QDebug>

using namespace FluentQt::Components;
using namespace FluentQt::Core;
using namespace FluentQt::Styling;

FluentTimeline::FluentTimeline(QWidget* parent)
    : FluentComponent(parent)
{
    setupLayout();
    setupScrollArea();
    setupAnimations();
    
    // Connect to theme changes
    connect(&FluentTheme::instance(), &FluentTheme::themeChanged,
            this, &FluentTimeline::onThemeChanged);
    
    updateColors();
    updateAccessibility();
    
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover);
}

FluentTimeline::FluentTimeline(FluentTimelineOrientation orientation, QWidget* parent)
    : FluentTimeline(parent)
{
    m_config.orientation = orientation;
    updateLayout();
}

FluentTimeline::FluentTimeline(const FluentTimelineConfig& config, QWidget* parent)
    : FluentTimeline(parent)
{
    setConfiguration(config);
}

FluentTimeline::~FluentTimeline() = default;

void FluentTimeline::setOrientation(FluentTimelineOrientation orientation) {
    if (m_config.orientation != orientation) {
        m_config.orientation = orientation;
        m_layoutDirty = true;
        updateLayout();
        emit orientationChanged(orientation);
    }
}

void FluentTimeline::setAlignment(FluentTimelineAlignment alignment) {
    if (m_config.alignment != alignment) {
        m_config.alignment = alignment;
        m_layoutDirty = true;
        updateLayout();
        emit alignmentChanged(alignment);
    }
}

void FluentTimeline::setTimelineStyle(FluentTimelineStyle style) {
    if (m_config.style != style) {
        m_config.style = style;
        updateLayout();
        updateColors();
        emit styleChanged(style);
    }
}

void FluentTimeline::setConnectorStyle(FluentTimelineConnectorStyle style) {
    if (m_config.connectorStyle != style) {
        m_config.connectorStyle = style;
        update();
        emit connectorStyleChanged(style);
    }
}

void FluentTimeline::setItemSpacing(int spacing) {
    if (m_config.itemSpacing != spacing) {
        m_config.itemSpacing = spacing;
        m_layoutDirty = true;
        updateLayout();
        emit itemSpacingChanged(spacing);
    }
}

void FluentTimeline::setConnectorWidth(int width) {
    if (m_config.connectorWidth != width) {
        m_config.connectorWidth = width;
        update();
        emit connectorWidthChanged(width);
    }
}

void FluentTimeline::setIndicatorSize(int size) {
    if (m_config.indicatorSize != size) {
        m_config.indicatorSize = size;
        m_layoutDirty = true;
        updateLayout();
        emit indicatorSizeChanged(size);
    }
}

void FluentTimeline::setAnimated(bool animated) {
    if (m_config.animated != animated) {
        m_config.animated = animated;
        emit animatedChanged(animated);
    }
}

void FluentTimeline::setScrollable(bool scrollable) {
    if (m_config.scrollable != scrollable) {
        m_config.scrollable = scrollable;
        setupScrollArea();
        emit scrollableChanged(scrollable);
    }
}

void FluentTimeline::setInteractive(bool interactive) {
    if (m_config.interactive != interactive) {
        m_config.interactive = interactive;
        
        // Update item interactivity
        for (auto* item : m_items) {
            if (item) {
                item->setInteractive(interactive);
            }
        }
        
        emit interactiveChanged(interactive);
    }
}

void FluentTimeline::setShowConnectors(bool show) {
    if (m_config.showConnectors != show) {
        m_config.showConnectors = show;
        update();
        emit showConnectorsChanged(show);
    }
}

void FluentTimeline::setShowIndicators(bool show) {
    if (m_config.showIndicators != show) {
        m_config.showIndicators = show;
        
        // Update item indicator visibility
        for (auto* item : m_items) {
            if (item) {
                item->setShowIndicator(show);
            }
        }
        
        emit showIndicatorsChanged(show);
    }
}

void FluentTimeline::setConfiguration(const FluentTimelineConfig& config) {
    const bool orientationHasChanged = (m_config.orientation != config.orientation);
    const bool alignmentHasChanged = (m_config.alignment != config.alignment);
    const bool styleHasChanged = (m_config.style != config.style);
    const bool spacingHasChanged = (m_config.itemSpacing != config.itemSpacing);
    const bool indicatorSizeHasChanged = (m_config.indicatorSize != config.indicatorSize);

    m_config = config;

    if (orientationHasChanged || alignmentHasChanged || spacingHasChanged || indicatorSizeHasChanged) {
        m_layoutDirty = true;
        updateLayout();
    }

    if (styleHasChanged) {
        updateColors();
    }

    setupScrollArea();
    updateItemStates();

    // Emit change signals
    if (orientationHasChanged) emit orientationChanged(config.orientation);
    if (alignmentHasChanged) emit alignmentChanged(config.alignment);
    if (styleHasChanged) emit styleChanged(config.style);
    emit connectorStyleChanged(config.connectorStyle);
    if (spacingHasChanged) emit itemSpacingChanged(config.itemSpacing);
    emit connectorWidthChanged(config.connectorWidth);
    if (indicatorSizeHasChanged) emit indicatorSizeChanged(config.indicatorSize);
    emit animatedChanged(config.animated);
    emit scrollableChanged(config.scrollable);
    emit interactiveChanged(config.interactive);
    emit showConnectorsChanged(config.showConnectors);
    emit showIndicatorsChanged(config.showIndicators);
}

void FluentTimeline::addItem(FluentTimelineItem* item) {
    if (!item) return;
    
    insertItem(m_items.size(), item);
}

void FluentTimeline::insertItem(int index, FluentTimelineItem* item) {
    if (!item || index < 0 || index > m_items.size()) return;
    
    // Set parent and configure item
    item->setParent(m_contentWidget ? m_contentWidget : this);
    item->setInteractive(m_config.interactive);
    item->setShowIndicator(m_config.showIndicators);
    
    // Connect signals
    connect(item, &FluentTimelineItem::clicked,
            this, &FluentTimeline::onItemClicked);
    connect(item, &FluentTimelineItem::doubleClicked,
            this, &FluentTimeline::onItemDoubleClicked);
    connect(item, &FluentTimelineItem::itemStateChanged,
            this, &FluentTimeline::onItemStateChanged);
    
    // Insert item
    m_items.insert(index, item);
    
    // Update layout
    m_layoutDirty = true;
    updateLayout();
    
    // Animate appearance if enabled
    if (m_config.animated && isVisible()) {
        animateItemAppearance(item);
    }
    
    emit itemAdded(item, index);
}

void FluentTimeline::removeItem(FluentTimelineItem* item) {
    if (!item) return;
    
    const int index = m_items.indexOf(item);
    if (index >= 0) {
        removeItem(index);
    }
}

void FluentTimeline::removeItem(int index) {
    if (index < 0 || index >= m_items.size()) return;
    
    FluentTimelineItem* item = m_items.at(index);
    
    // Animate removal if enabled
    if (m_config.animated && isVisible()) {
        animateItemRemoval(item);
    }
    
    // Remove from list
    m_items.removeAt(index);
    
    // Update current item if necessary
    if (m_currentItem == item) {
        m_currentItem = nullptr;
        if (!m_items.isEmpty()) {
            setCurrentIndex(qMin(index, m_items.size() - 1));
        }
    }
    
    // Update layout
    m_layoutDirty = true;
    updateLayout();
    
    emit itemRemoved(item, index);
    
    // Delete item if it's not being animated
    if (!m_config.animated || !isVisible()) {
        item->deleteLater();
    }
}

void FluentTimeline::clearItems() {
    while (!m_items.isEmpty()) {
        removeItem(0);
    }
}

FluentTimelineItem* FluentTimeline::item(int index) const {
    if (index >= 0 && index < m_items.size()) {
        return m_items.at(index);
    }
    return nullptr;
}

int FluentTimeline::itemCount() const {
    return m_items.size();
}

QList<FluentTimelineItem*> FluentTimeline::items() const {
    return m_items;
}

int FluentTimeline::indexOf(FluentTimelineItem* item) const {
    return m_items.indexOf(item);
}

int FluentTimeline::currentIndex() const {
    return indexOf(m_currentItem);
}

void FluentTimeline::setCurrentItem(FluentTimelineItem* item) {
    if (m_currentItem != item) {
        FluentTimelineItem* previous = m_currentItem;
        m_currentItem = item;
        
        updateItemStates();
        
        emit currentItemChanged(item, previous);
        emit currentIndexChanged(currentIndex());
    }
}

void FluentTimeline::setCurrentIndex(int index) {
    setCurrentItem(item(index));
}

FluentTimelineItem* FluentTimeline::addTextItem(const QString& title, const QString& description) {
    auto* item = new FluentTimelineItem(title, this);
    if (!description.isEmpty()) {
        item->setDescription(description);
    }
    addItem(item);
    return item;
}

FluentTimelineItem* FluentTimeline::addIconItem(const QIcon& icon, const QString& title, const QString& description) {
    auto* item = new FluentTimelineItem(title, this);
    item->setIcon(icon);
    if (!description.isEmpty()) {
        item->setDescription(description);
    }
    addItem(item);
    return item;
}

FluentTimelineItem* FluentTimeline::addDateTimeItem(const QDateTime& dateTime, const QString& title, const QString& description) {
    auto* item = new FluentTimelineItem(title, this);
    item->setDateTime(dateTime);
    if (!description.isEmpty()) {
        item->setDescription(description);
    }
    addItem(item);
    return item;
}

void FluentTimeline::animateToItem(FluentTimelineItem* item) {
    if (!item || !m_config.animated) {
        scrollToItem(item);
        return;
    }
    
    const int index = indexOf(item);
    if (index >= 0) {
        animateToIndex(index);
    }
}

void FluentTimeline::animateToIndex(int index) {
    if (!m_config.animated || !m_scrollArea) {
        scrollToIndex(index);
        return;
    }
    
    FluentTimelineItem* targetItem = item(index);
    if (!targetItem) return;
    
    const QRect itemRect = getItemRect(index);
    const QRect visibleRect = m_scrollArea->viewport()->rect();
    
    int targetValue = 0;
    if (m_config.orientation == FluentTimelineOrientation::Vertical) {
        targetValue = itemRect.center().y() - visibleRect.height() / 2;
        targetValue = qBound(0, targetValue, m_scrollArea->verticalScrollBar()->maximum());
    } else {
        targetValue = itemRect.center().x() - visibleRect.width() / 2;
        targetValue = qBound(0, targetValue, m_scrollArea->horizontalScrollBar()->maximum());
    }
    
    if (m_scrollAnimation) {
        m_scrollAnimation->stop();
        
        if (m_config.orientation == FluentTimelineOrientation::Vertical) {
            m_scrollAnimation->setTargetObject(m_scrollArea->verticalScrollBar());
            m_scrollAnimation->setStartValue(m_scrollArea->verticalScrollBar()->value());
        } else {
            m_scrollAnimation->setTargetObject(m_scrollArea->horizontalScrollBar());
            m_scrollAnimation->setStartValue(m_scrollArea->horizontalScrollBar()->value());
        }
        
        m_scrollAnimation->setPropertyName("value");
        m_scrollAnimation->setEndValue(targetValue);
        m_scrollAnimation->setDuration(300);
        m_scrollAnimation->setEasingCurve(QEasingCurve::OutCubic);
        m_scrollAnimation->start();
    }
}

void FluentTimeline::scrollToItem(FluentTimelineItem* item) {
    const int index = indexOf(item);
    if (index >= 0) {
        scrollToIndex(index);
    }
}

void FluentTimeline::scrollToIndex(int index) {
    if (!m_scrollArea) return;
    
    const QRect itemRect = getItemRect(index);
    if (m_config.orientation == FluentTimelineOrientation::Vertical) {
        m_scrollArea->ensureVisible(itemRect.center().x(), itemRect.center().y(), 
                                   itemRect.width() / 2, itemRect.height() / 2);
    } else {
        m_scrollArea->ensureVisible(itemRect.center().x(), itemRect.center().y(),
                                   itemRect.width() / 2, itemRect.height() / 2);
    }
}

// Static factory methods
FluentTimeline* FluentTimeline::createVerticalTimeline(QWidget* parent) {
    return new FluentTimeline(FluentTimelineOrientation::Vertical, parent);
}

FluentTimeline* FluentTimeline::createHorizontalTimeline(QWidget* parent) {
    return new FluentTimeline(FluentTimelineOrientation::Horizontal, parent);
}

FluentTimeline* FluentTimeline::createCompactTimeline(QWidget* parent) {
    FluentTimelineConfig config;
    config.style = FluentTimelineStyle::Compact;
    config.itemSpacing = 8;
    config.indicatorSize = 8;
    return new FluentTimeline(config, parent);
}

FluentTimeline* FluentTimeline::createDetailedTimeline(QWidget* parent) {
    FluentTimelineConfig config;
    config.style = FluentTimelineStyle::Detailed;
    config.itemSpacing = 24;
    config.indicatorSize = 16;
    return new FluentTimeline(config, parent);
}

// Public slots
void FluentTimeline::expandAll() {
    for (auto* item : m_items) {
        if (item && item->isExpandable()) {
            item->setExpanded(true);
        }
    }
}

void FluentTimeline::collapseAll() {
    for (auto* item : m_items) {
        if (item && item->isExpandable()) {
            item->setExpanded(false);
        }
    }
}

void FluentTimeline::refresh() {
    m_layoutDirty = true;
    updateLayout();
    updateColors();
    update();
}

// Protected event handlers
void FluentTimeline::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    FLUENT_PROFILE("FluentTimeline::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    // Paint background
    paintBackground(painter);

    // Paint connectors if enabled
    if (m_config.showConnectors && m_items.size() > 1) {
        paintConnectors(painter);
    }

    // Paint indicators if enabled and not handled by items
    if (m_config.showIndicators) {
        paintIndicators(painter);
    }
}

void FluentTimeline::resizeEvent(QResizeEvent* event) {
    FluentComponent::resizeEvent(event);

    if (event->size() != m_lastSize) {
        m_lastSize = event->size();
        m_layoutDirty = true;
        updateLayout();
    }
}

void FluentTimeline::wheelEvent(QWheelEvent* event) {
    if (m_scrollArea) {
        // Forward wheel events to scroll area
        QApplication::sendEvent(m_scrollArea, event);
    } else {
        FluentComponent::wheelEvent(event);
    }
}

void FluentTimeline::keyPressEvent(QKeyEvent* event) {
    if (!m_config.interactive) {
        FluentComponent::keyPressEvent(event);
        return;
    }

    const int currentIdx = currentIndex();
    int newIndex = currentIdx;

    switch (event->key()) {
        case Qt::Key_Up:
            if (m_config.orientation == FluentTimelineOrientation::Vertical) {
                newIndex = qMax(0, currentIdx - 1);
            }
            break;
        case Qt::Key_Down:
            if (m_config.orientation == FluentTimelineOrientation::Vertical) {
                newIndex = qMin(m_items.size() - 1, currentIdx + 1);
            }
            break;
        case Qt::Key_Left:
            if (m_config.orientation == FluentTimelineOrientation::Horizontal) {
                newIndex = qMax(0, currentIdx - 1);
            }
            break;
        case Qt::Key_Right:
            if (m_config.orientation == FluentTimelineOrientation::Horizontal) {
                newIndex = qMin(m_items.size() - 1, currentIdx + 1);
            }
            break;
        case Qt::Key_Home:
            newIndex = 0;
            break;
        case Qt::Key_End:
            newIndex = m_items.size() - 1;
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Space:
            if (m_currentItem) {
                emit itemClicked(m_currentItem, currentIdx);
            }
            break;
        default:
            FluentComponent::keyPressEvent(event);
            return;
    }

    if (newIndex != currentIdx && newIndex >= 0 && newIndex < m_items.size()) {
        setCurrentIndex(newIndex);
        animateToIndex(newIndex);
        event->accept();
    } else {
        FluentComponent::keyPressEvent(event);
    }
}

void FluentTimeline::focusInEvent(QFocusEvent* event) {
    FluentComponent::focusInEvent(event);

    // Set current item if none is selected
    if (!m_currentItem && !m_items.isEmpty()) {
        setCurrentIndex(0);
    }

    update();
}

void FluentTimeline::focusOutEvent(QFocusEvent* event) {
    FluentComponent::focusOutEvent(event);
    update();
}

void FluentTimeline::updateStateStyle() {
    FluentComponent::updateStateStyle();
    updateColors();
}

void FluentTimeline::performStateTransition(FluentState from, FluentState to) {
    FluentComponent::performStateTransition(from, to);

    if (m_config.animated) {
        // Add any state transition animations here
    }
}

// Private slots
void FluentTimeline::onItemClicked() {
    auto* item = qobject_cast<FluentTimelineItem*>(sender());
    if (item) {
        const int index = indexOf(item);
        setCurrentItem(item);
        emit itemClicked(item, index);
    }
}

void FluentTimeline::onItemDoubleClicked() {
    auto* item = qobject_cast<FluentTimelineItem*>(sender());
    if (item) {
        const int index = indexOf(item);
        emit itemDoubleClicked(item, index);
    }
}

void FluentTimeline::onItemStateChanged(FluentTimelineItemState state) {
    Q_UNUSED(state)
    // Update connectors and indicators if needed
    update();
}

void FluentTimeline::onThemeChanged() {
    updateColors();
    update();
}

void FluentTimeline::onScrollValueChanged(int value) {
    Q_UNUSED(value)
    // Handle scroll position changes if needed
}

// Private implementation methods
void FluentTimeline::setupLayout() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Create content widget
    m_contentWidget = new QWidget(this);
    m_contentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    updateLayout();
}

void FluentTimeline::setupScrollArea() {
    if (m_config.scrollable) {
        if (!m_scrollArea) {
            m_scrollArea = new QScrollArea(this);
            m_scrollArea->setWidgetResizable(true);
            m_scrollArea->setFrameShape(QFrame::NoFrame);
            m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

            // Connect scroll signals
            connect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
                    this, &FluentTimeline::onScrollValueChanged);
            connect(m_scrollArea->horizontalScrollBar(), &QScrollBar::valueChanged,
                    this, &FluentTimeline::onScrollValueChanged);

            m_mainLayout->addWidget(m_scrollArea);
        }

        m_scrollArea->setWidget(m_contentWidget);
        m_scrollArea->show();
    } else {
        if (m_scrollArea) {
            m_scrollArea->hide();
            m_scrollArea->setWidget(nullptr);
        }

        m_mainLayout->addWidget(m_contentWidget);
    }
}

void FluentTimeline::setupAnimations() {
    // Scroll animation
    m_scrollAnimation = std::make_unique<QPropertyAnimation>(this);
    m_scrollAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Layout animation group
    m_layoutAnimation = std::make_unique<QParallelAnimationGroup>(this);

    // Item animation group
    m_itemAnimation = std::make_unique<QSequentialAnimationGroup>(this);
}

void FluentTimeline::updateLayout() {
    if (!m_layoutDirty) return;

    // Clear existing layout
    if (m_contentLayout) {
        QLayoutItem* item;
        while ((item = m_contentLayout->takeAt(0)) != nullptr) {
            delete item;
        }
        delete m_contentLayout;
        m_contentLayout = nullptr;
    }

    // Create new layout based on orientation
    if (m_config.orientation == FluentTimelineOrientation::Vertical) {
        layoutVertical();
    } else {
        layoutHorizontal();
    }

    calculateItemPositions();
    m_layoutDirty = false;

    if (m_config.animated && isVisible()) {
        animateLayoutChange();
    }
}

void FluentTimeline::layoutVertical() {
    auto* layout = new QVBoxLayout(m_contentWidget);
    layout->setSpacing(m_config.itemSpacing);

    // Set margins based on style
    int margin = 8;
    switch (m_config.style) {
        case FluentTimelineStyle::Compact:
            margin = 4;
            break;
        case FluentTimelineStyle::Detailed:
            margin = 16;
            break;
        case FluentTimelineStyle::Minimal:
            margin = 2;
            break;
        default:
            margin = 8;
            break;
    }

    layout->setContentsMargins(margin, margin, margin, margin);

    // Add items to layout
    for (auto* item : m_items) {
        if (item) {
            // FluentTimelineItem inherits from FluentComponent which inherits from QWidget
            QWidget* widget = static_cast<QWidget*>(item);
            layout->addWidget(widget);

            // Set alignment based on configuration
            switch (m_config.alignment) {
                case FluentTimelineAlignment::Left:
                    layout->setAlignment(widget, Qt::AlignLeft);
                    break;
                case FluentTimelineAlignment::Right:
                    layout->setAlignment(widget, Qt::AlignRight);
                    break;
                case FluentTimelineAlignment::Center:
                    layout->setAlignment(widget, Qt::AlignCenter);
                    break;
                case FluentTimelineAlignment::Alternate:
                    // Alternate alignment will be handled in item positioning
                    break;
            }
        }
    }

    layout->addStretch();
    m_contentLayout = layout;
}

void FluentTimeline::layoutHorizontal() {
    auto* layout = new QHBoxLayout(m_contentWidget);
    layout->setSpacing(m_config.itemSpacing);

    // Set margins based on style
    int margin = 8;
    switch (m_config.style) {
        case FluentTimelineStyle::Compact:
            margin = 4;
            break;
        case FluentTimelineStyle::Detailed:
            margin = 16;
            break;
        case FluentTimelineStyle::Minimal:
            margin = 2;
            break;
        default:
            margin = 8;
            break;
    }

    layout->setContentsMargins(margin, margin, margin, margin);

    // Add items to layout
    for (auto* item : m_items) {
        if (item) {
            // Cast to QWidget since FluentTimelineItem inherits from FluentComponent which inherits from QWidget
            QWidget* widget = qobject_cast<QWidget*>(item);
            if (widget) {
                layout->addWidget(widget);

                // Set alignment based on configuration
                switch (m_config.alignment) {
                    case FluentTimelineAlignment::Left:
                        layout->setAlignment(widget, Qt::AlignTop);
                        break;
                    case FluentTimelineAlignment::Right:
                        layout->setAlignment(widget, Qt::AlignBottom);
                        break;
                    case FluentTimelineAlignment::Center:
                        layout->setAlignment(widget, Qt::AlignVCenter);
                        break;
                    case FluentTimelineAlignment::Alternate:
                        // Alternate alignment will be handled in item positioning
                        break;
                }
            }
        }
    }

    layout->addStretch();
    m_contentLayout = layout;
}

void FluentTimeline::calculateItemPositions() {
    m_itemPositions.clear();
    m_itemRects.clear();

    for (int i = 0; i < m_items.size(); ++i) {
        auto* item = m_items.at(i);
        if (item) {
            const QPoint pos = getItemPosition(i);
            const QRect rect = getItemRect(i);

            m_itemPositions.append(pos);
            m_itemRects.append(rect);
        } else {
            m_itemPositions.append(QPoint());
            m_itemRects.append(QRect());
        }
    }
}

void FluentTimeline::updateItemPositions() {
    calculateItemPositions();
    update();
}

void FluentTimeline::updateConnectors() {
    update();
}

void FluentTimeline::updateColors() {
    const auto& theme = FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    if (m_config.autoCalculateColors) {
        // Calculate colors based on theme and style
        switch (m_config.style) {
            case FluentTimelineStyle::Compact:
                m_connectorColor = palette.neutralTertiary;
                m_indicatorColor = palette.accent;
                m_backgroundColor = palette.neutralLightest;
                break;
            case FluentTimelineStyle::Detailed:
                m_connectorColor = palette.neutralSecondary;
                m_indicatorColor = palette.accent;
                m_backgroundColor = palette.neutralLightest;
                break;
            case FluentTimelineStyle::Minimal:
                m_connectorColor = palette.neutralQuaternary;
                m_indicatorColor = palette.neutralPrimary;
                m_backgroundColor = Qt::transparent;
                break;
            default:
                m_connectorColor = palette.neutralTertiary;
                m_indicatorColor = palette.accent;
                m_backgroundColor = palette.neutralLightest;
                break;
        }
    } else {
        // Use custom colors if provided
        if (m_config.connectorColor.isValid()) {
            m_connectorColor = m_config.connectorColor;
        }
        if (m_config.indicatorColor.isValid()) {
            m_indicatorColor = m_config.indicatorColor;
        }
        if (m_config.backgroundColor.isValid()) {
            m_backgroundColor = m_config.backgroundColor;
        }
    }

    m_focusColor = palette.accent;
}

void FluentTimeline::updateAccessibility() {
    Accessibility::setAccessibleName(this, "Timeline");
    Accessibility::setAccessibleRole(this, QAccessible::List);
    Accessibility::setAccessibleDescription(this,
        QString("Timeline with %1 items").arg(m_items.size()));
}

void FluentTimeline::paintBackground(QPainter& painter) {
    if (m_backgroundColor.alpha() > 0) {
        painter.fillRect(rect(), m_backgroundColor);
    }
}

void FluentTimeline::paintConnectors(QPainter& painter) {
    if (m_items.size() < 2) return;

    QPen connectorPen(m_connectorColor, m_config.connectorWidth);

    switch (m_config.connectorStyle) {
        case FluentTimelineConnectorStyle::Dashed:
            connectorPen.setStyle(Qt::DashLine);
            break;
        case FluentTimelineConnectorStyle::Dotted:
            connectorPen.setStyle(Qt::DotLine);
            break;
        case FluentTimelineConnectorStyle::None:
            return;
        default:
            connectorPen.setStyle(Qt::SolidLine);
            break;
    }

    painter.setPen(connectorPen);

    for (int i = 0; i < m_items.size() - 1; ++i) {
        const QPoint start = getConnectorStart(i);
        const QPoint end = getConnectorEnd(i + 1);

        if (m_config.orientation == FluentTimelineOrientation::Vertical) {
            painter.drawLine(start, end);
        } else {
            painter.drawLine(start, end);
        }
    }
}

void FluentTimeline::paintIndicators(QPainter& painter) {
    // Indicators are typically painted by individual items
    // This method can be used for custom indicator rendering
    Q_UNUSED(painter)
}

QRect FluentTimeline::getItemRect(int index) const {
    if (index >= 0 && index < m_itemRects.size()) {
        return m_itemRects.at(index);
    }

    // Fallback calculation
    auto* item = this->item(index);
    if (item) {
        return item->geometry();
    }

    return QRect();
}

QPoint FluentTimeline::getItemPosition(int index) const {
    if (index >= 0 && index < m_itemPositions.size()) {
        return m_itemPositions.at(index);
    }

    // Fallback calculation
    auto* item = this->item(index);
    if (item) {
        return item->pos();
    }

    return QPoint();
}

QPoint FluentTimeline::getConnectorStart(int index) const {
    const QRect itemRect = getItemRect(index);

    if (m_config.orientation == FluentTimelineOrientation::Vertical) {
        return QPoint(itemRect.left() + m_config.indicatorSize / 2,
                     itemRect.bottom());
    } else {
        return QPoint(itemRect.right(),
                     itemRect.top() + m_config.indicatorSize / 2);
    }
}

QPoint FluentTimeline::getConnectorEnd(int index) const {
    const QRect itemRect = getItemRect(index);

    if (m_config.orientation == FluentTimelineOrientation::Vertical) {
        return QPoint(itemRect.left() + m_config.indicatorSize / 2,
                     itemRect.top());
    } else {
        return QPoint(itemRect.left(),
                     itemRect.top() + m_config.indicatorSize / 2);
    }
}

void FluentTimeline::ensureItemVisible(FluentTimelineItem* item) {
    if (m_scrollArea && item) {
        const int index = indexOf(item);
        if (index >= 0) {
            scrollToIndex(index);
        }
    }
}

void FluentTimeline::updateItemStates() {
    for (int i = 0; i < m_items.size(); ++i) {
        auto* item = m_items.at(i);
        if (item) {
            // Update item visual state based on selection
            if (item == m_currentItem) {
                item->setState(FluentState::Focused);
            } else if (i == m_hoveredIndex) {
                item->setState(FluentState::Hovered);
            } else {
                item->setState(FluentState::Normal);
            }
        }
    }
}

void FluentTimeline::animateItemAppearance(FluentTimelineItem* item) {
    if (!item || !m_config.animated) return;

    // Create fade-in animation
    auto* opacityEffect = new QGraphicsOpacityEffect(item);
    item->setGraphicsEffect(opacityEffect);

    auto* animation = new QPropertyAnimation(opacityEffect, "opacity", this);
    animation->setDuration(300);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    connect(animation, &QPropertyAnimation::finished, [=]() {
        item->setGraphicsEffect(nullptr);
        opacityEffect->deleteLater();
        animation->deleteLater();
    });

    animation->start();
}

void FluentTimeline::animateItemRemoval(FluentTimelineItem* item) {
    if (!item || !m_config.animated) return;

    // Create fade-out animation
    auto* opacityEffect = new QGraphicsOpacityEffect(item);
    item->setGraphicsEffect(opacityEffect);

    auto* animation = new QPropertyAnimation(opacityEffect, "opacity", this);
    animation->setDuration(200);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->setEasingCurve(QEasingCurve::InCubic);

    connect(animation, &QPropertyAnimation::finished, [=]() {
        item->deleteLater();
        animation->deleteLater();
    });

    animation->start();
}

void FluentTimeline::animateLayoutChange() {
    if (!m_config.animated) return;

    // Animate layout changes if needed
    // This can be expanded to include more sophisticated layout animations
}
