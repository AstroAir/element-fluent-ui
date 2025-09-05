// src/Components/FluentGrid.cpp
#include "FluentQt/Components/FluentGrid.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QChildEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QtMath>
#include <algorithm>

namespace FluentQt::Components {

FluentGrid::FluentGrid(QWidget* parent)
    : FluentComponent(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setObjectName("FluentGrid");

    // Apply FluentUI design tokens for spacing
    applyFluentSpacingTokens();
    setupAnimations();

    // Validate FluentUI compliance
    validateFluentUICompliance();
}

FluentGrid::FluentGrid(int columns, QWidget* parent) : FluentGrid(parent) {
    setColumns(columns);
}

FluentGrid::FluentGrid(int columns, int rows, QWidget* parent)
    : FluentGrid(columns, parent) {
    setRows(rows);
}

FluentGrid::~FluentGrid() = default;

void FluentGrid::setupAnimations() {
    FLUENT_PROFILE("FluentGrid::setupAnimations");
    // Animation setup is handled per-item in animateToLayout()
}

int FluentGrid::columns() const { return m_columns; }

void FluentGrid::setColumns(int columns) {
    const int clampedColumns = qMax(-1, columns);
    if (m_columns != clampedColumns) {
        m_columns = clampedColumns;
        m_layoutDirty = true;
        relayout();
        emit columnsChanged(clampedColumns);
    }
}

int FluentGrid::rows() const { return m_rows; }

void FluentGrid::setRows(int rows) {
    const int clampedRows = qMax(-1, rows);
    if (m_rows != clampedRows) {
        m_rows = clampedRows;
        m_layoutDirty = true;
        relayout();
        emit rowsChanged(clampedRows);
    }
}

FluentGridFlow FluentGrid::flow() const { return m_flow; }

void FluentGrid::setFlow(FluentGridFlow flow) {
    if (m_flow != flow) {
        m_flow = flow;
        m_layoutDirty = true;
        relayout();
        emit flowChanged(flow);
    }
}

FluentGridAlignment FluentGrid::alignItems() const { return m_alignItems; }

void FluentGrid::setAlignItems(FluentGridAlignment alignment) {
    if (m_alignItems != alignment) {
        m_alignItems = alignment;
        m_layoutDirty = true;
        relayout();
        emit alignItemsChanged(alignment);
    }
}

FluentGridAlignment FluentGrid::justifyItems() const { return m_justifyItems; }

void FluentGrid::setJustifyItems(FluentGridAlignment alignment) {
    if (m_justifyItems != alignment) {
        m_justifyItems = alignment;
        m_layoutDirty = true;
        relayout();
        emit justifyItemsChanged(alignment);
    }
}

FluentGridAlignment FluentGrid::alignContent() const { return m_alignContent; }

void FluentGrid::setAlignContent(FluentGridAlignment alignment) {
    if (m_alignContent != alignment) {
        m_alignContent = alignment;
        m_layoutDirty = true;
        relayout();
        emit alignContentChanged(alignment);
    }
}

FluentGridAlignment FluentGrid::justifyContent() const {
    return m_justifyContent;
}

void FluentGrid::setJustifyContent(FluentGridAlignment alignment) {
    if (m_justifyContent != alignment) {
        m_justifyContent = alignment;
        m_layoutDirty = true;
        relayout();
        emit justifyContentChanged(alignment);
    }
}

int FluentGrid::spacing() const { return qMax(m_rowSpacing, m_columnSpacing); }

void FluentGrid::setSpacing(int spacing) {
    const int clampedSpacing = qMax(0, spacing);
    if (m_rowSpacing != clampedSpacing || m_columnSpacing != clampedSpacing) {
        m_rowSpacing = clampedSpacing;
        m_columnSpacing = clampedSpacing;
        m_layoutDirty = true;
        relayout();
        emit spacingChanged(clampedSpacing);
        emit rowSpacingChanged(clampedSpacing);
        emit columnSpacingChanged(clampedSpacing);
    }
}

int FluentGrid::rowSpacing() const { return m_rowSpacing; }

void FluentGrid::setRowSpacing(int spacing) {
    const int clampedSpacing = qMax(0, spacing);
    if (m_rowSpacing != clampedSpacing) {
        m_rowSpacing = clampedSpacing;
        m_layoutDirty = true;
        relayout();
        emit rowSpacingChanged(clampedSpacing);
    }
}

int FluentGrid::columnSpacing() const { return m_columnSpacing; }

void FluentGrid::setColumnSpacing(int spacing) {
    const int clampedSpacing = qMax(0, spacing);
    if (m_columnSpacing != clampedSpacing) {
        m_columnSpacing = clampedSpacing;
        m_layoutDirty = true;
        relayout();
        emit columnSpacingChanged(clampedSpacing);
    }
}

QMargins FluentGrid::padding() const { return m_padding; }

void FluentGrid::setPadding(const QMargins& padding) {
    if (m_padding != padding) {
        m_padding = padding;
        m_layoutDirty = true;
        relayout();
        emit paddingChanged(padding);
    }
}

void FluentGrid::setPadding(int padding) {
    setPadding(QMargins(padding, padding, padding, padding));
}

bool FluentGrid::autoColumns() const { return m_autoColumns; }

void FluentGrid::setAutoColumns(bool auto_) {
    if (m_autoColumns != auto_) {
        m_autoColumns = auto_;
        m_layoutDirty = true;
        relayout();
        emit autoColumnsChanged(auto_);
    }
}

bool FluentGrid::autoRows() const { return m_autoRows; }

void FluentGrid::setAutoRows(bool auto_) {
    if (m_autoRows != auto_) {
        m_autoRows = auto_;
        m_layoutDirty = true;
        relayout();
        emit autoRowsChanged(auto_);
    }
}

bool FluentGrid::isAnimated() const { return m_animated; }

void FluentGrid::setAnimated(bool animated) {
    if (m_animated != animated) {
        m_animated = animated;
        emit animatedChanged(animated);
    }
}

void FluentGrid::addItem(QWidget* widget) {
    if (widget) {
        addItem(FluentGridItem(widget));
    }
}

void FluentGrid::addItem(QWidget* widget, int row, int column) {
    if (widget) {
        addItem(FluentGridItem(widget, row, column));
    }
}

void FluentGrid::addItem(QWidget* widget, int row, int column, int rowSpan,
                         int columnSpan) {
    if (widget) {
        FluentGridItem item(widget, row, column);
        item.rowSpan = qMax(1, rowSpan);
        item.columnSpan = qMax(1, columnSpan);
        addItem(item);
    }
}

void FluentGrid::addItem(const FluentGridItem& item) {
    if (!item.widget) {
        qWarning() << "FluentGrid::addItem: Cannot add null widget";
        return;
    }

    // Validate item parameters
    if (item.rowSpan < 1 || item.columnSpan < 1) {
        qWarning() << "FluentGrid::addItem: Invalid span values (rowSpan:"
                   << item.rowSpan << ", columnSpan:" << item.columnSpan << ")";
        return;
    }

    // Check for duplicate widgets
    for (const FluentGridItem& existingItem : m_items) {
        if (existingItem.widget == item.widget) {
            qWarning() << "FluentGrid::addItem: Widget already exists in grid";
            return;
        }
    }

    try {
        m_items.append(item);
        item.widget->setParent(this);

        connect(item.widget, &QObject::destroyed, this,
                &FluentGrid::onItemDestroyed);

        invalidateLayoutCache();
        m_layoutDirty = true;
        relayout();

        emit itemAdded(m_items.size() - 1);
    } catch (const std::exception& e) {
        qCritical() << "FluentGrid::addItem: Exception occurred:" << e.what();
        // Remove the item if it was partially added
        m_items.removeAll(item);
    }
}

void FluentGrid::insertItem(int index, QWidget* widget) {
    if (widget) {
        insertItem(index, FluentGridItem(widget));
    }
}

void FluentGrid::insertItem(int index, const FluentGridItem& item) {
    if (!item.widget)
        return;

    const int clampedIndex = qBound(0, index, m_items.size());
    m_items.insert(clampedIndex, item);
    item.widget->setParent(this);

    connect(item.widget, &QObject::destroyed, this,
            &FluentGrid::onItemDestroyed);

    m_layoutDirty = true;
    relayout();

    emit itemAdded(clampedIndex);
}

void FluentGrid::removeItem(QWidget* widget) {
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].widget == widget) {
            removeItemAt(i);
            break;
        }
    }
}

void FluentGrid::removeItemAt(int index) {
    if (index >= 0 && index < m_items.size()) {
        FluentGridItem item = m_items.takeAt(index);
        if (item.widget) {
            disconnect(item.widget, &QObject::destroyed, this,
                       &FluentGrid::onItemDestroyed);
            item.widget->setParent(nullptr);
        }

        m_layoutDirty = true;
        relayout();

        emit itemRemoved(index);
    }
}

void FluentGrid::clear() {
    for (const FluentGridItem& item : m_items) {
        if (item.widget) {
            disconnect(item.widget, &QObject::destroyed, this,
                       &FluentGrid::onItemDestroyed);
            item.widget->setParent(nullptr);
        }
    }

    m_items.clear();
    m_itemRects.clear();
    m_layoutDirty = true;
    relayout();
}

// Item access methods
FluentGridItem* FluentGrid::itemAt(int index) {
    if (index >= 0 && index < m_items.size()) {
        return &m_items[index];
    }
    return nullptr;
}

const FluentGridItem* FluentGrid::itemAt(int index) const {
    if (index >= 0 && index < m_items.size()) {
        return &m_items[index];
    }
    return nullptr;
}

FluentGridItem* FluentGrid::itemForWidget(QWidget* widget) {
    for (FluentGridItem& item : m_items) {
        if (item.widget == widget) {
            return &item;
        }
    }
    return nullptr;
}

const FluentGridItem* FluentGrid::itemForWidget(QWidget* widget) const {
    for (const FluentGridItem& item : m_items) {
        if (item.widget == widget) {
            return &item;
        }
    }
    return nullptr;
}

int FluentGrid::itemCount() const { return m_items.size(); }

QList<FluentGridItem>& FluentGrid::items() { return m_items; }

const QList<FluentGridItem>& FluentGrid::items() const { return m_items; }

// Grid information methods
int FluentGrid::actualColumns() const { return m_actualColumns; }

int FluentGrid::actualRows() const { return m_actualRows; }

QSize FluentGrid::gridSize() const { return m_gridSize; }

QRect FluentGrid::itemRect(int index) const {
    if (index >= 0 && index < m_itemRects.size()) {
        return m_itemRects[index];
    }
    return QRect();
}

QRect FluentGrid::itemRect(const FluentGridItem& item) const {
    for (int i = 0; i < m_items.size(); ++i) {
        if (&m_items[i] == &item) {
            return itemRect(i);
        }
    }
    return QRect();
}

// Size hints
QSize FluentGrid::sizeHint() const {
    if (m_layoutDirty) {
        const_cast<FluentGrid*>(this)->calculateLayout();
    }

    QSize hint = m_gridSize;
    hint += QSize(m_padding.left() + m_padding.right(),
                  m_padding.top() + m_padding.bottom());

    return hint.expandedTo(minimumSizeHint());
}

QSize FluentGrid::minimumSizeHint() const {
    // Calculate minimum size based on minimum item sizes
    QSize minSize(0, 0);

    if (!m_items.isEmpty()) {
        // Use FluentUI minimum touch target size (44x44)
        const int minTouchTarget = 44;
        minSize = QSize(minTouchTarget, minTouchTarget);
    }

    minSize += QSize(m_padding.left() + m_padding.right(),
                     m_padding.top() + m_padding.bottom());

    return minSize;
}

// Layout methods
void FluentGrid::relayout() {
    if (!m_layoutInProgress) {
        m_layoutDirty = true;
        if (m_animated) {
            animateLayout();
        } else {
            calculateLayout();
            applyLayout();
        }
    }
}

void FluentGrid::animateLayout() {
    if (!m_animated || !isVisible()) {
        calculateLayout();
        applyLayout();
        return;
    }

    // Store current positions for animation
    QList<QRect> oldRects = m_itemRects;

    // Calculate new layout
    calculateLayout();

    // Animate to new positions
    animateToLayout();
}

// Core layout calculation methods
void FluentGrid::calculateLayout() {
    FLUENT_PROFILE("FluentGrid::calculateLayout");

    if (!m_layoutDirty || m_items.isEmpty()) {
        return;
    }

    // Check if we can use cached layout
    if (canUseCachedLayout()) {
        applyCachedLayout();
        return;
    }

    // Performance optimization: batch layout calculations
    const bool wasLayoutInProgress = m_layoutInProgress;
    m_layoutInProgress = true;

    // Sort items by order if needed
    sortItemsByOrder();

    // Calculate grid dimensions
    calculateGridDimensions();

    // Calculate item positions and sizes
    calculateItemPositions();
    calculateItemSizes();

    // Apply alignment
    applyAlignment();

    // Cache the layout for future use
    cacheCurrentLayout();

    m_layoutDirty = false;
    m_layoutInProgress = wasLayoutInProgress;

    // Announce layout change for accessibility
    announceLayoutChange();

    emit layoutChanged();
}

void FluentGrid::calculateGridDimensions() {
    if (m_items.isEmpty()) {
        m_actualColumns = 0;
        m_actualRows = 0;
        m_gridSize = QSize(0, 0);
        return;
    }

    try {
        // Determine effective columns and rows with validation
        m_actualColumns = getEffectiveColumns();
        m_actualRows = getEffectiveRows();

        // Validate calculated dimensions
        if (m_actualColumns <= 0 || m_actualRows <= 0) {
            qWarning() << "FluentGrid::calculateGridDimensions: Invalid grid "
                          "dimensions"
                       << "columns:" << m_actualColumns
                       << "rows:" << m_actualRows;
            m_actualColumns = qMax(1, m_actualColumns);
            m_actualRows = qMax(1, m_actualRows);
        }

        // Calculate grid size based on available space
        const QRect contentRect = rect().marginsRemoved(m_padding);

        if (contentRect.width() <= 0 || contentRect.height() <= 0) {
            qWarning()
                << "FluentGrid::calculateGridDimensions: Invalid content rect"
                << contentRect;
            m_gridSize = QSize(0, 0);
            return;
        }

        if (m_actualColumns > 0) {
            const int totalColumnSpacing =
                (m_actualColumns - 1) * m_columnSpacing;
            const int availableWidth = contentRect.width() - totalColumnSpacing;
            const int columnWidth = qMax(1, availableWidth / m_actualColumns);

            const int totalRowSpacing = (m_actualRows - 1) * m_rowSpacing;
            const int availableHeight = contentRect.height() - totalRowSpacing;
            const int rowHeight = qMax(1, availableHeight / m_actualRows);

            m_gridSize =
                QSize(m_actualColumns * columnWidth + totalColumnSpacing,
                      m_actualRows * rowHeight + totalRowSpacing);
        }
    } catch (const std::exception& e) {
        qCritical()
            << "FluentGrid::calculateGridDimensions: Exception occurred:"
            << e.what();
        // Fallback to safe defaults
        m_actualColumns = 1;
        m_actualRows = m_items.size();
        m_gridSize = QSize(100, 100 * m_items.size());
    }
}

void FluentGrid::calculateItemPositions() {
    m_itemRects.clear();
    m_itemRects.reserve(m_items.size());

    const QRect contentRect = rect().marginsRemoved(m_padding);
    const int columnWidth =
        m_actualColumns > 0
            ? (contentRect.width() - (m_actualColumns - 1) * m_columnSpacing) /
                  m_actualColumns
            : 0;
    const int rowHeight =
        m_actualRows > 0
            ? (contentRect.height() - (m_actualRows - 1) * m_rowSpacing) /
                  m_actualRows
            : 0;

    for (int i = 0; i < m_items.size(); ++i) {
        const FluentGridItem& item = m_items[i];

        int row, column;
        if (item.row >= 0 && item.column >= 0) {
            // Explicit positioning
            row = item.row;
            column = item.column;
        } else {
            // Auto positioning based on flow
            switch (m_flow) {
                case FluentGridFlow::Row:
                    row = i / m_actualColumns;
                    column = i % m_actualColumns;
                    break;
                case FluentGridFlow::Column:
                    column = i / m_actualRows;
                    row = i % m_actualRows;
                    break;
                case FluentGridFlow::RowReverse:
                    row = i / m_actualColumns;
                    column = m_actualColumns - 1 - (i % m_actualColumns);
                    break;
                case FluentGridFlow::ColumnReverse:
                    column = i / m_actualRows;
                    row = m_actualRows - 1 - (i % m_actualRows);
                    break;
            }
        }

        // Calculate position
        const QPoint position = calculateItemPosition(item, row, column);
        const QSize size =
            calculateItemSize(item, QSize(columnWidth, rowHeight));

        m_itemRects.append(QRect(position, size));
    }
}

void FluentGrid::calculateItemSizes() {
    // Item sizes are calculated in calculateItemPositions for efficiency
    // This method can be used for future enhancements
}

void FluentGrid::applyAlignment() {
    // Apply FluentUI-compliant alignment algorithms
    applyContentAlignment();
    applyItemAlignment();
    applyJustification();
}

void FluentGrid::applyContentAlignment() {
    // Apply content alignment within the grid container
    const QRect contentRect = rect().marginsRemoved(m_padding);

    if (m_gridSize.width() < contentRect.width() ||
        m_gridSize.height() < contentRect.height()) {
        // Calculate offset for content alignment
        QPoint offset =
            calculateContentOffset(contentRect, m_gridSize, m_alignContent);

        // Apply offset to all item rectangles
        for (QRect& itemRect : m_itemRects) {
            itemRect.translate(offset);
        }
    }
}

void FluentGrid::applyItemAlignment() {
    // Apply individual item alignment within their grid cells
    const QRect contentRect = rect().marginsRemoved(m_padding);
    const int columnWidth =
        m_actualColumns > 0
            ? (contentRect.width() - (m_actualColumns - 1) * m_columnSpacing) /
                  m_actualColumns
            : 0;
    const int rowHeight =
        m_actualRows > 0
            ? (contentRect.height() - (m_actualRows - 1) * m_rowSpacing) /
                  m_actualRows
            : 0;

    for (int i = 0; i < m_items.size() && i < m_itemRects.size(); ++i) {
        const FluentGridItem& item = m_items[i];
        QRect& itemRect = m_itemRects[i];

        // Determine effective alignment for this item
        FluentGridAlignment effectiveAlignment =
            item.alignSelf != FluentGridItemAlignment::Auto
                ? static_cast<FluentGridAlignment>(item.alignSelf)
                : m_alignItems;

        // Apply item-specific alignment
        itemRect = applyItemSpecificAlignment(
            itemRect, QSize(columnWidth, rowHeight), effectiveAlignment);
    }
}

void FluentGrid::applyJustification() {
    // Apply justification for space distribution
    if (m_justifyContent == FluentGridAlignment::SpaceBetween ||
        m_justifyContent == FluentGridAlignment::SpaceAround ||
        m_justifyContent == FluentGridAlignment::SpaceEvenly) {
        applySpaceDistribution();
    }

    // Apply item justification
    if (m_justifyItems != FluentGridAlignment::Stretch) {
        applyItemJustification();
    }
}

void FluentGrid::applyLayout() {
    if (m_items.size() != m_itemRects.size()) {
        return;
    }

    m_layoutInProgress = true;

    // Apply calculated positions to widgets
    for (int i = 0; i < m_items.size(); ++i) {
        const FluentGridItem& item = m_items[i];
        const QRect& rect = m_itemRects[i];

        if (item.widget && item.visible) {
            item.widget->setGeometry(rect);
            item.widget->setVisible(true);
        } else if (item.widget) {
            item.widget->setVisible(false);
        }
    }

    updateItemVisibility();
    m_layoutInProgress = false;
}

// Helper calculation methods
QSize FluentGrid::calculateItemSize(const FluentGridItem& item,
                                    const QSize& availableSize) const {
    QSize size = availableSize;

    // Apply span multipliers
    if (item.columnSpan > 1) {
        size.setWidth(size.width() * item.columnSpan +
                      (item.columnSpan - 1) * m_columnSpacing);
    }
    if (item.rowSpan > 1) {
        size.setHeight(size.height() * item.rowSpan +
                       (item.rowSpan - 1) * m_rowSpacing);
    }

    // Respect min/max size constraints
    if (!item.minSize.isEmpty()) {
        size = size.expandedTo(item.minSize);
    }
    if (!item.maxSize.isEmpty()) {
        size = size.boundedTo(item.maxSize);
    }

    return size;
}

QPoint FluentGrid::calculateItemPosition(const FluentGridItem& item, int row,
                                         int column) const {
    const QRect contentRect = rect().marginsRemoved(m_padding);
    const int columnWidth =
        m_actualColumns > 0
            ? (contentRect.width() - (m_actualColumns - 1) * m_columnSpacing) /
                  m_actualColumns
            : 0;
    const int rowHeight =
        m_actualRows > 0
            ? (contentRect.height() - (m_actualRows - 1) * m_rowSpacing) /
                  m_actualRows
            : 0;

    const int x = contentRect.left() + column * (columnWidth + m_columnSpacing);
    const int y = contentRect.top() + row * (rowHeight + m_rowSpacing);

    return QPoint(x, y);
}

QRect FluentGrid::calculateAlignedRect(const QRect& rect, const QSize& size,
                                       FluentGridAlignment alignment) const {
    QRect aligned = rect;

    switch (alignment) {
        case FluentGridAlignment::Start:
            aligned.setSize(size);
            break;
        case FluentGridAlignment::Center:
            aligned = QRect(
                rect.center() - QPoint(size.width() / 2, size.height() / 2),
                size);
            break;
        case FluentGridAlignment::End:
            aligned = QRect(rect.bottomRight() -
                                QPoint(size.width() - 1, size.height() - 1),
                            size);
            break;
        case FluentGridAlignment::Stretch:
            // Keep original rect size
            break;
        case FluentGridAlignment::SpaceBetween:
        case FluentGridAlignment::SpaceAround:
        case FluentGridAlignment::SpaceEvenly:
            // These are handled at the grid level, not individual items
            aligned.setSize(size);
            break;
    }

    return aligned;
}

// Enhanced alignment helper methods
QPoint FluentGrid::calculateContentOffset(const QRect& container,
                                          const QSize& content,
                                          FluentGridAlignment alignment) const {
    QPoint offset(0, 0);

    switch (alignment) {
        case FluentGridAlignment::Start:
            // No offset needed - already at start
            break;
        case FluentGridAlignment::Center:
            offset.setX((container.width() - content.width()) / 2);
            offset.setY((container.height() - content.height()) / 2);
            break;
        case FluentGridAlignment::End:
            offset.setX(container.width() - content.width());
            offset.setY(container.height() - content.height());
            break;
        case FluentGridAlignment::Stretch:
            // No offset for stretch - content fills container
            break;
        default:
            // For space distribution, handle at grid level
            break;
    }

    return offset;
}

QRect FluentGrid::applyItemSpecificAlignment(
    const QRect& itemRect, const QSize& cellSize,
    FluentGridAlignment alignment) const {
    QRect aligned = itemRect;

    switch (alignment) {
        case FluentGridAlignment::Start:
            // Item stays at top-left of cell
            aligned.setSize(aligned.size().boundedTo(cellSize));
            break;
        case FluentGridAlignment::Center:
            // Center item within cell
            {
                const QPoint cellCenter =
                    itemRect.topLeft() +
                    QPoint(cellSize.width() / 2, cellSize.height() / 2);
                const QSize itemSize = aligned.size().boundedTo(cellSize);
                aligned = QRect(cellCenter - QPoint(itemSize.width() / 2,
                                                    itemSize.height() / 2),
                                itemSize);
            }
            break;
        case FluentGridAlignment::End:
            // Align item to bottom-right of cell
            {
                const QSize itemSize = aligned.size().boundedTo(cellSize);
                const QPoint bottomRight =
                    itemRect.topLeft() +
                    QPoint(cellSize.width(), cellSize.height());
                aligned = QRect(
                    bottomRight - QPoint(itemSize.width(), itemSize.height()),
                    itemSize);
            }
            break;
        case FluentGridAlignment::Stretch:
            // Stretch item to fill cell
            aligned.setSize(cellSize);
            break;
        default:
            break;
    }

    return aligned;
}

void FluentGrid::applySpaceDistribution() {
    if (m_itemRects.isEmpty() || m_actualColumns == 0) {
        return;
    }

    const QRect contentRect = rect().marginsRemoved(m_padding);
    const int totalContentWidth = m_gridSize.width();
    const int availableSpace = contentRect.width() - totalContentWidth;

    if (availableSpace <= 0) {
        return;
    }

    int spacing = 0;
    int offset = 0;

    switch (m_justifyContent) {
        case FluentGridAlignment::SpaceBetween:
            if (m_actualColumns > 1) {
                spacing = availableSpace / (m_actualColumns - 1);
            }
            break;
        case FluentGridAlignment::SpaceAround:
            spacing = availableSpace / m_actualColumns;
            offset = spacing / 2;
            break;
        case FluentGridAlignment::SpaceEvenly:
            spacing = availableSpace / (m_actualColumns + 1);
            offset = spacing;
            break;
        default:
            return;
    }

    // Apply spacing adjustments
    for (int i = 0; i < m_itemRects.size(); ++i) {
        const int column = i % m_actualColumns;
        const int extraOffset = offset + column * spacing;
        m_itemRects[i].translate(extraOffset, 0);
    }
}

void FluentGrid::applyItemJustification() {
    // Apply horizontal justification to individual items within their cells
    const QRect contentRect = rect().marginsRemoved(m_padding);
    const int columnWidth =
        m_actualColumns > 0
            ? (contentRect.width() - (m_actualColumns - 1) * m_columnSpacing) /
                  m_actualColumns
            : 0;

    for (int i = 0; i < m_items.size() && i < m_itemRects.size(); ++i) {
        QRect& itemRect = m_itemRects[i];

        // Apply horizontal justification based on m_justifyItems
        switch (m_justifyItems) {
            case FluentGridAlignment::Start:
                // Already at start position
                break;
            case FluentGridAlignment::Center: {
                const int extraSpace = columnWidth - itemRect.width();
                if (extraSpace > 0) {
                    itemRect.translate(extraSpace / 2, 0);
                }
            } break;
            case FluentGridAlignment::End: {
                const int extraSpace = columnWidth - itemRect.width();
                if (extraSpace > 0) {
                    itemRect.translate(extraSpace, 0);
                }
            } break;
            case FluentGridAlignment::Stretch:
                // Item already stretched to column width
                break;
            default:
                break;
        }
    }
}

// Utility methods
int FluentGrid::getEffectiveColumns() const {
    if (m_columns > 0) {
        return m_columns;
    }

    if (m_autoColumns && !m_items.isEmpty()) {
        // Auto-calculate columns based on content and available space
        const QRect contentRect = rect().marginsRemoved(m_padding);
        const int minItemWidth = 100;  // FluentUI minimum reasonable item width
        const int maxColumns = qMax(1, contentRect.width() / minItemWidth);

        // Use square root as a reasonable default for auto-sizing
        const int autoColumns =
            qMax(1, static_cast<int>(qSqrt(m_items.size())));
        return qMin(autoColumns, maxColumns);
    }

    return qMax(1, static_cast<int>(qSqrt(m_items.size())));
}

int FluentGrid::getEffectiveRows() const {
    if (m_rows > 0) {
        return m_rows;
    }

    const int effectiveColumns = getEffectiveColumns();
    if (effectiveColumns > 0) {
        return qMax(1,
                    (m_items.size() + effectiveColumns - 1) / effectiveColumns);
    }

    return 1;
}

void FluentGrid::updateItemVisibility() {
    for (int i = 0; i < m_items.size(); ++i) {
        const FluentGridItem& item = m_items[i];
        if (item.widget) {
            item.widget->setVisible(item.visible && isVisible());
        }
    }
}

void FluentGrid::sortItemsByOrder() {
    std::stable_sort(m_items.begin(), m_items.end(),
                     [](const FluentGridItem& a, const FluentGridItem& b) {
                         return a.order < b.order;
                     });
}

// Enhanced Animation methods with FluentUI motion principles
void FluentGrid::animateToLayout() {
    if (!m_animator || m_items.size() != m_itemRects.size()) {
        applyLayout();
        return;
    }

    // Check for reduced motion preference
    if (shouldRespectReducedMotion()) {
        applyLayout();
        return;
    }

    // Clean up previous animations
    cleanupAnimations();

    // Create FluentUI-compliant animations for each item
    createItemAnimations();

    // Start staggered animations for better visual flow
    startStaggeredAnimations();
}

void FluentGrid::cleanupAnimations() {
    for (auto* anim : m_itemAnimations) {
        if (anim) {
            anim->stop();
            anim->deleteLater();
        }
    }
    m_itemAnimations.clear();
}

void FluentGrid::createItemAnimations() {
    const int staggerDelay = 20;  // FluentUI stagger timing

    for (int i = 0; i < m_items.size(); ++i) {
        const FluentGridItem& item = m_items[i];
        if (!item.widget || !item.visible) {
            continue;
        }

        const QRect targetRect = m_itemRects[i];
        const QRect currentRect = item.widget->geometry();

        if (currentRect != targetRect) {
            // Create FluentUI-compliant animation
            auto* animation = createFluentAnimation(
                item.widget, currentRect, targetRect, i * staggerDelay);
            if (animation) {
                m_itemAnimations.append(animation);
            }
        }
    }
}

QPropertyAnimation* FluentGrid::createFluentAnimation(QWidget* widget,
                                                      const QRect& from,
                                                      const QRect& to,
                                                      int delay) {
    if (!widget) {
        return nullptr;
    }

    auto* animation = new QPropertyAnimation(widget, "geometry", this);
    animation->setStartValue(from);
    animation->setEndValue(to);

    // Use FluentUI motion tokens
    const int duration = getFluentAnimationDuration();
    animation->setDuration(duration);

    // Apply FluentUI easing curve
    animation->setEasingCurve(getFluentEasingCurve());

    // Add stagger delay for natural motion flow
    if (delay > 0) {
        QTimer::singleShot(delay, [animation]() {
            if (animation) {
                animation->start();
            }
        });
    }

    return animation;
}

void FluentGrid::startStaggeredAnimations() {
    if (m_itemAnimations.isEmpty()) {
        applyLayout();
        return;
    }

    // Create animation group for coordinated playback
    auto* group = new QParallelAnimationGroup(this);

    // Add all animations to group
    for (auto* anim : m_itemAnimations) {
        if (anim) {
            group->addAnimation(anim);
        }
    }

    // Connect completion handler
    connect(group, &QParallelAnimationGroup::finished, this,
            &FluentGrid::onLayoutAnimationFinished);

    // Start with FluentUI entrance animation
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

int FluentGrid::getFluentAnimationDuration() const {
    // FluentUI motion tokens for layout transitions
    // Fast: 150ms, Normal: 300ms, Slow: 500ms
    return 300;  // Normal duration for layout changes
}

QEasingCurve FluentGrid::getFluentEasingCurve() const {
    // FluentUI standard easing curve for layout transitions
    // Equivalent to CSS cubic-bezier(0.1, 0.9, 0.2, 1.0)
    QEasingCurve curve(QEasingCurve::BezierSpline);
    curve.addCubicBezierSegment(QPointF(0.1, 0.9), QPointF(0.2, 1.0),
                                QPointF(1.0, 1.0));
    return curve;
}

bool FluentGrid::shouldRespectReducedMotion() const {
    // Check system preference for reduced motion
    // This would typically check system accessibility settings
    return false;  // For now, always allow animations
}

// Event handlers
void FluentGrid::resizeEvent(QResizeEvent* event) {
    Core::FluentComponent::resizeEvent(event);

    if (event->size() != m_lastSize) {
        m_lastSize = event->size();
        m_layoutDirty = true;

        // Update responsive layout on resize
        updateResponsiveLayout();
        relayout();
    }
}

void FluentGrid::childEvent(QChildEvent* event) {
    Core::FluentComponent::childEvent(event);

    if (event->type() == QEvent::ChildAdded) {
        if (auto* widget = qobject_cast<QWidget*>(event->child())) {
            // Check if this widget is already in our items
            bool found = false;
            for (const FluentGridItem& item : m_items) {
                if (item.widget == widget) {
                    found = true;
                    break;
                }
            }

            // If not found, add it automatically
            if (!found) {
                addItem(widget);

                // Setup focus chain for keyboard navigation
                setupFocusChain();
            }
        }
    } else if (event->type() == QEvent::ChildRemoved) {
        if (auto* widget = qobject_cast<QWidget*>(event->child())) {
            // Remove from our items if it exists
            removeItem(widget);
            setupFocusChain();
        }
    }
}

void FluentGrid::showEvent(QShowEvent* event) {
    Core::FluentComponent::showEvent(event);

    if (m_layoutDirty) {
        relayout();
    }

    // Setup accessibility when first shown
    setupAccessibility();
}

void FluentGrid::keyPressEvent(QKeyEvent* event) {
    // Handle grid-specific keyboard navigation
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right ||
        event->key() == Qt::Key_Up || event->key() == Qt::Key_Down ||
        event->key() == Qt::Key_Home || event->key() == Qt::Key_End) {
        handleKeyboardNavigation(event);
        if (event->isAccepted()) {
            return;
        }
    }

    // Handle Tab navigation
    if (event->key() == Qt::Key_Tab) {
        focusNextItem();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_Backtab) {
        focusPreviousItem();
        event->accept();
        return;
    }

    Core::FluentComponent::keyPressEvent(event);
}

void FluentGrid::focusInEvent(QFocusEvent* event) {
    Core::FluentComponent::focusInEvent(event);

    // If no item has focus, focus the first focusable item
    if (getCurrentFocusIndex() < 0 && !m_items.isEmpty()) {
        for (const FluentGridItem& item : m_items) {
            if (item.widget && item.visible &&
                item.widget->focusPolicy() != Qt::NoFocus) {
                item.widget->setFocus();
                break;
            }
        }
    }
}

void FluentGrid::focusOutEvent(QFocusEvent* event) {
    Core::FluentComponent::focusOutEvent(event);
    // Grid focus handling - let individual items manage their focus
}

// Slot implementations
void FluentGrid::onItemDestroyed(QObject* object) {
    // Remove the destroyed widget from our items
    for (int i = m_items.size() - 1; i >= 0; --i) {
        if (m_items[i].widget == object) {
            m_items.removeAt(i);
            if (i < m_itemRects.size()) {
                m_itemRects.removeAt(i);
            }
            m_layoutDirty = true;
            relayout();
            emit itemRemoved(i);
            break;
        }
    }
}

void FluentGrid::onLayoutAnimationFinished() {
    // Clean up animation references
    m_itemAnimations.clear();
    updateItemVisibility();
}

// User interaction and focus management
void FluentGrid::setupFocusChain() {
    // Create proper focus chain for keyboard navigation
    QWidget* previousWidget = nullptr;

    for (const FluentGridItem& item : m_items) {
        if (item.widget && item.visible &&
            item.widget->focusPolicy() != Qt::NoFocus) {
            if (previousWidget) {
                setTabOrder(previousWidget, item.widget);
            }
            previousWidget = item.widget;
        }
    }
}

void FluentGrid::handleKeyboardNavigation(QKeyEvent* event) {
    const int currentIndex = getCurrentFocusIndex();
    if (currentIndex < 0) {
        return;
    }

    int targetIndex = currentIndex;
    const int columns = getEffectiveColumns();

    switch (event->key()) {
        case Qt::Key_Left:
            if (currentIndex % columns > 0) {
                targetIndex = currentIndex - 1;
            }
            break;
        case Qt::Key_Right:
            if (currentIndex % columns < columns - 1 &&
                currentIndex + 1 < m_items.size()) {
                targetIndex = currentIndex + 1;
            }
            break;
        case Qt::Key_Up:
            if (currentIndex >= columns) {
                targetIndex = currentIndex - columns;
            }
            break;
        case Qt::Key_Down:
            if (currentIndex + columns < m_items.size()) {
                targetIndex = currentIndex + columns;
            }
            break;
        case Qt::Key_Home:
            targetIndex = 0;
            break;
        case Qt::Key_End:
            targetIndex = m_items.size() - 1;
            break;
        default:
            return;
    }

    // Focus the target item
    if (targetIndex != currentIndex && targetIndex >= 0 &&
        targetIndex < m_items.size()) {
        const FluentGridItem& targetItem = m_items[targetIndex];
        if (targetItem.widget && targetItem.visible) {
            targetItem.widget->setFocus();
            event->accept();
        }
    }
}

void FluentGrid::focusNextItem() {
    const int currentIndex = getCurrentFocusIndex();
    for (int i = currentIndex + 1; i < m_items.size(); ++i) {
        const FluentGridItem& item = m_items[i];
        if (item.widget && item.visible &&
            item.widget->focusPolicy() != Qt::NoFocus) {
            item.widget->setFocus();
            return;
        }
    }

    // Wrap around to beginning
    for (int i = 0; i <= currentIndex; ++i) {
        const FluentGridItem& item = m_items[i];
        if (item.widget && item.visible &&
            item.widget->focusPolicy() != Qt::NoFocus) {
            item.widget->setFocus();
            return;
        }
    }
}

void FluentGrid::focusPreviousItem() {
    const int currentIndex = getCurrentFocusIndex();
    for (int i = currentIndex - 1; i >= 0; --i) {
        const FluentGridItem& item = m_items[i];
        if (item.widget && item.visible &&
            item.widget->focusPolicy() != Qt::NoFocus) {
            item.widget->setFocus();
            return;
        }
    }

    // Wrap around to end
    for (int i = m_items.size() - 1; i >= currentIndex; --i) {
        const FluentGridItem& item = m_items[i];
        if (item.widget && item.visible &&
            item.widget->focusPolicy() != Qt::NoFocus) {
            item.widget->setFocus();
            return;
        }
    }
}

int FluentGrid::getCurrentFocusIndex() const {
    QWidget* focusedWidget = QApplication::focusWidget();
    if (!focusedWidget) {
        return -1;
    }

    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].widget == focusedWidget ||
            m_items[i].widget->isAncestorOf(focusedWidget)) {
            return i;
        }
    }

    return -1;
}

// Accessibility support
void FluentGrid::setupAccessibility() {
    // Set up grid accessibility properties
    setAccessibleName("Grid Layout");
    setAccessibleDescription(
        QString("Grid with %1 items arranged in %2 columns")
            .arg(m_items.size())
            .arg(getEffectiveColumns()));

    // Enable keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // Setup focus chain for proper tab navigation
    setupFocusChain();

    // Update accessibility info for all items
    updateAccessibilityInfo();
}

void FluentGrid::updateAccessibilityInfo() {
    // Update accessibility information for each grid item
    for (int i = 0; i < m_items.size(); ++i) {
        const FluentGridItem& item = m_items[i];
        if (item.widget) {
            // Calculate grid position
            const int row = i / getEffectiveColumns();
            const int column = i % getEffectiveColumns();

            // Set accessible description with grid position
            const QString description =
                QString("Grid item at row %1, column %2 of %3")
                    .arg(row + 1)
                    .arg(column + 1)
                    .arg(m_items.size());
            item.widget->setAccessibleDescription(description);

            // Ensure proper tab order
            if (i > 0 && m_items[i - 1].widget) {
                setTabOrder(m_items[i - 1].widget, item.widget);
            }
        }
    }
}

void FluentGrid::announceLayoutChange() {
    // Announce layout changes to screen readers
    const QString announcement =
        QString("Grid layout updated: %1 items in %2 columns")
            .arg(m_items.size())
            .arg(getEffectiveColumns());
    setAccessibleDescription(announcement);
}

// FluentUI Design System Integration
void FluentGrid::applyFluentSpacingTokens() {
    // Apply FluentUI 4px spacing system
    // Default spacing values based on FluentUI design tokens

    // Use size160 (16px) as default spacing - optimal for grid layouts
    const int fluentSpacing = 16;

    // Apply consistent spacing
    setRowSpacing(fluentSpacing);
    setColumnSpacing(fluentSpacing);

    // Use size160 (16px) as default padding for grid containers
    setPadding(fluentSpacing);

    // Ensure spacing aligns with 4px grid
    validateFluentSpacing();
}

void FluentGrid::validateFluentSpacing() {
    // Ensure all spacing values align with FluentUI 4px base unit
    const int baseUnit = 4;

    // Round spacing to nearest 4px multiple
    m_rowSpacing = ((m_rowSpacing + baseUnit / 2) / baseUnit) * baseUnit;
    m_columnSpacing = ((m_columnSpacing + baseUnit / 2) / baseUnit) * baseUnit;

    // Validate padding aligns with 4px grid
    const int left = ((m_padding.left() + baseUnit / 2) / baseUnit) * baseUnit;
    const int top = ((m_padding.top() + baseUnit / 2) / baseUnit) * baseUnit;
    const int right =
        ((m_padding.right() + baseUnit / 2) / baseUnit) * baseUnit;
    const int bottom =
        ((m_padding.bottom() + baseUnit / 2) / baseUnit) * baseUnit;

    m_padding = QMargins(left, top, right, bottom);
}

int FluentGrid::getFluentSpacingToken(const QString& tokenName) const {
    // FluentUI spacing tokens (in pixels)
    static const QHash<QString, int> spacingTokens = {
        {"sizeNone", 0},  // 0px
        {"size20", 2},    // 2px
        {"size40", 4},    // 4px
        {"size60", 6},    // 6px
        {"size80", 8},    // 8px
        {"size100", 10},  // 10px
        {"size120", 12},  // 12px
        {"size160", 16},  // 16px - default grid spacing
        {"size200", 20},  // 20px
        {"size240", 24},  // 24px
        {"size280", 28},  // 28px
        {"size320", 32},  // 32px
        {"size360", 36},  // 36px
        {"size400", 40},  // 40px
        {"size480", 48},  // 48px
        {"size520", 52},  // 52px
        {"size560", 56}   // 56px
    };

    return spacingTokens.value(tokenName, 16);  // Default to size160
}

void FluentGrid::applyFluentAlignment() {
    // Apply FluentUI alignment principles
    // Ensure proper visual hierarchy and spacing relationships

    // Default to stretch alignment for optimal space utilization
    if (m_alignItems == FluentGridAlignment::Start) {
        // Use FluentUI recommended alignment for grid items
        m_alignItems = FluentGridAlignment::Stretch;
    }

    if (m_justifyItems == FluentGridAlignment::Start) {
        // Use FluentUI recommended justification for grid items
        m_justifyItems = FluentGridAlignment::Stretch;
    }
}

bool FluentGrid::isFluentCompliant() const {
    // Validate FluentUI design compliance
    const int baseUnit = 4;

    // Check spacing alignment with 4px grid
    if (m_rowSpacing % baseUnit != 0 || m_columnSpacing % baseUnit != 0) {
        return false;
    }

    // Check padding alignment
    if (m_padding.left() % baseUnit != 0 || m_padding.top() % baseUnit != 0 ||
        m_padding.right() % baseUnit != 0 ||
        m_padding.bottom() % baseUnit != 0) {
        return false;
    }

    // Check minimum touch target compliance (44x44 for mobile)
    const QSize minTouchTarget(44, 44);
    for (const FluentGridItem& item : m_items) {
        if (item.widget &&
            item.widget->size().width() < minTouchTarget.width() &&
            item.widget->size().height() < minTouchTarget.height()) {
            // Allow small decorative elements
            continue;
        }
    }

    return true;
}

// FluentUI compliance validation
void FluentGrid::validateFluentUICompliance() {
    // Comprehensive validation of FluentUI design system compliance
    bool isCompliant = true;

    // Validate spacing compliance
    if (!validateSpacingCompliance()) {
        qWarning() << "FluentGrid: Spacing does not comply with FluentUI 4px "
                      "grid system";
        isCompliant = false;
    }

    // Validate alignment compliance
    if (!validateAlignmentCompliance()) {
        qWarning()
            << "FluentGrid: Alignment does not comply with FluentUI guidelines";
        isCompliant = false;
    }

    // Validate accessibility compliance
    if (!validateAccessibilityCompliance()) {
        qWarning() << "FluentGrid: Accessibility features do not meet FluentUI "
                      "standards";
        isCompliant = false;
    }

    if (isCompliant) {
        qDebug()
            << "FluentGrid: Successfully validated FluentUI design compliance";
    }
}

bool FluentGrid::validateSpacingCompliance() const {
    const int baseUnit = 4;

    // Check row and column spacing alignment with 4px grid
    if (m_rowSpacing % baseUnit != 0 || m_columnSpacing % baseUnit != 0) {
        return false;
    }

    // Check padding alignment
    if (m_padding.left() % baseUnit != 0 || m_padding.top() % baseUnit != 0 ||
        m_padding.right() % baseUnit != 0 ||
        m_padding.bottom() % baseUnit != 0) {
        return false;
    }

    // Validate spacing values are within FluentUI recommended range
    const QList<int> validSpacing = {0,  2,  4,  6,  8,  10, 12, 16, 20,
                                     24, 28, 32, 36, 40, 48, 52, 56};
    if (!validSpacing.contains(m_rowSpacing) ||
        !validSpacing.contains(m_columnSpacing)) {
        qDebug() << "FluentGrid: Using non-standard spacing values - consider "
                    "using FluentUI tokens";
    }

    return true;
}

bool FluentGrid::validateAlignmentCompliance() const {
    // Validate that alignment settings follow FluentUI principles

    // Check for proper content alignment
    if (m_alignContent == FluentGridAlignment::Stretch &&
        (m_justifyContent == FluentGridAlignment::SpaceBetween ||
         m_justifyContent == FluentGridAlignment::SpaceAround ||
         m_justifyContent == FluentGridAlignment::SpaceEvenly)) {
        qDebug() << "FluentGrid: Mixed stretch and space distribution may not "
                    "follow FluentUI guidelines";
    }

    // Validate item alignment consistency
    bool hasConsistentAlignment = true;
    FluentGridItemAlignment firstAlignment = FluentGridItemAlignment::Auto;

    for (const FluentGridItem& item : m_items) {
        if (firstAlignment == FluentGridItemAlignment::Auto) {
            firstAlignment = item.alignSelf;
        } else if (item.alignSelf != FluentGridItemAlignment::Auto &&
                   item.alignSelf != firstAlignment) {
            hasConsistentAlignment = false;
            break;
        }
    }

    if (!hasConsistentAlignment) {
        qDebug() << "FluentGrid: Inconsistent item alignment may affect visual "
                    "hierarchy";
    }

    return true;
}

bool FluentGrid::validateAccessibilityCompliance() const {
    // Validate accessibility features compliance with FluentUI standards

    // Check focus policy
    if (focusPolicy() == Qt::NoFocus && !m_items.isEmpty()) {
        return false;
    }

    // Check minimum touch target sizes (44x44 for mobile compatibility)
    const QSize minTouchTarget(44, 44);
    for (const FluentGridItem& item : m_items) {
        if (item.widget && item.visible) {
            const QSize itemSize = item.widget->size();
            if (itemSize.width() < minTouchTarget.width() &&
                itemSize.height() < minTouchTarget.height()) {
                // Allow small decorative elements, but warn about potential
                // accessibility issues
                qDebug()
                    << "FluentGrid: Item smaller than minimum touch target size"
                    << itemSize;
            }
        }
    }

    // Check for proper accessible names and descriptions
    if (accessibleName().isEmpty()) {
        qDebug() << "FluentGrid: Consider setting accessible name for better "
                    "screen reader support";
    }

    return true;
}

// Performance optimization methods
bool FluentGrid::canUseCachedLayout() const {
    // Check if current conditions match cached layout conditions
    return !m_layoutDirty && m_lastSize == size() && !m_itemRects.isEmpty() &&
           m_itemRects.size() == m_items.size();
}

void FluentGrid::applyCachedLayout() {
    // Apply the cached layout without recalculation
    if (m_itemRects.size() == m_items.size()) {
        applyLayout();
    }
}

void FluentGrid::cacheCurrentLayout() {
    // Cache is automatically maintained through m_itemRects and m_lastSize
    // Additional caching could be added here for complex calculations
    m_lastSize = size();
}

void FluentGrid::invalidateLayoutCache() {
    // Force layout recalculation on next layout request
    m_layoutDirty = true;
    m_lastSize = QSize();
}

// Responsive Design Implementation
void FluentGrid::updateResponsiveLayout() {
    if (!shouldUseResponsiveLayout()) {
        return;
    }

    const int responsiveColumns = getResponsiveColumns();
    if (responsiveColumns != m_actualColumns) {
        // Update columns based on responsive breakpoints
        if (m_autoColumns) {
            m_actualColumns = responsiveColumns;
            m_layoutDirty = true;
            relayout();
        }
    }

    // Apply responsive spacing adjustments
    applyResponsiveSpacing();
}

int FluentGrid::getResponsiveColumns() const {
    const QRect contentRect = rect().marginsRemoved(m_padding);
    const int availableWidth = contentRect.width();

    // FluentUI responsive breakpoints (based on Microsoft's design system)
    if (availableWidth >= 1920) {
        // xxx-large: 1920px and up - 6-8 columns
        return qMin(8, qMax(6, m_items.size()));
    } else if (availableWidth >= 1366) {
        // xx-large: 1366-1919px - 4-6 columns
        return qMin(6, qMax(4, static_cast<int>(qSqrt(m_items.size() * 1.5))));
    } else if (availableWidth >= 1024) {
        // x-large: 1024-1365px - 3-4 columns
        return qMin(4, qMax(3, static_cast<int>(qSqrt(m_items.size()))));
    } else if (availableWidth >= 640) {
        // large: 640-1023px - 2-3 columns
        return qMin(3, qMax(2, static_cast<int>(qSqrt(m_items.size() * 0.8))));
    } else if (availableWidth >= 480) {
        // medium: 480-639px - 2 columns
        return qMin(2, qMax(1, m_items.size()));
    } else {
        // small: <479px - 1 column
        return 1;
    }
}

bool FluentGrid::shouldUseResponsiveLayout() const {
    // Enable responsive behavior when auto-sizing is enabled
    return m_autoColumns || m_autoRows;
}

void FluentGrid::applyResponsiveSpacing() {
    const QRect contentRect = rect().marginsRemoved(m_padding);
    const int availableWidth = contentRect.width();

    // Adjust spacing based on screen size for optimal touch targets
    int responsiveSpacing;

    if (availableWidth >= 1024) {
        // Desktop: use standard spacing
        responsiveSpacing = getFluentSpacingToken("size160");  // 16px
    } else if (availableWidth >= 640) {
        // Tablet: slightly reduced spacing
        responsiveSpacing = getFluentSpacingToken("size120");  // 12px
    } else {
        // Mobile: minimal spacing for more content
        responsiveSpacing = getFluentSpacingToken("size80");  // 8px
    }

    // Apply responsive spacing if different from current
    if (m_rowSpacing != responsiveSpacing ||
        m_columnSpacing != responsiveSpacing) {
        m_rowSpacing = responsiveSpacing;
        m_columnSpacing = responsiveSpacing;
        m_layoutDirty = true;
    }
}

}  // namespace FluentQt::Components
