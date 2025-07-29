// src/Components/FluentGrid.cpp
#include "FluentQt/Components/FluentGrid.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QPainter>
#include <QResizeEvent>
#include <QChildEvent>
#include <QShowEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QtMath>
#include <algorithm>

namespace FluentQt::Components {

FluentGrid::FluentGrid(QWidget* parent)
    : FluentComponent(parent)
    , m_animator(std::make_unique<Animation::FluentAnimator>(this))
{
    setObjectName("FluentGrid");
    setupAnimations();
}

FluentGrid::FluentGrid(int columns, QWidget* parent)
    : FluentGrid(parent)
{
    setColumns(columns);
}

FluentGrid::FluentGrid(int columns, int rows, QWidget* parent)
    : FluentGrid(columns, parent)
{
    setRows(rows);
}

FluentGrid::~FluentGrid() = default;

void FluentGrid::setupAnimations() {
    FLUENT_PROFILE("FluentGrid::setupAnimations");
    // Animation setup is handled per-item in animateToLayout()
}

int FluentGrid::columns() const {
    return m_columns;
}

void FluentGrid::setColumns(int columns) {
    const int clampedColumns = qMax(-1, columns);
    if (m_columns != clampedColumns) {
        m_columns = clampedColumns;
        m_layoutDirty = true;
        relayout();
        emit columnsChanged(clampedColumns);
    }
}

int FluentGrid::rows() const {
    return m_rows;
}

void FluentGrid::setRows(int rows) {
    const int clampedRows = qMax(-1, rows);
    if (m_rows != clampedRows) {
        m_rows = clampedRows;
        m_layoutDirty = true;
        relayout();
        emit rowsChanged(clampedRows);
    }
}

FluentGridFlow FluentGrid::flow() const {
    return m_flow;
}

void FluentGrid::setFlow(FluentGridFlow flow) {
    if (m_flow != flow) {
        m_flow = flow;
        m_layoutDirty = true;
        relayout();
        emit flowChanged(flow);
    }
}

FluentGridAlignment FluentGrid::alignItems() const {
    return m_alignItems;
}

void FluentGrid::setAlignItems(FluentGridAlignment alignment) {
    if (m_alignItems != alignment) {
        m_alignItems = alignment;
        m_layoutDirty = true;
        relayout();
        emit alignItemsChanged(alignment);
    }
}

FluentGridAlignment FluentGrid::justifyItems() const {
    return m_justifyItems;
}

void FluentGrid::setJustifyItems(FluentGridAlignment alignment) {
    if (m_justifyItems != alignment) {
        m_justifyItems = alignment;
        m_layoutDirty = true;
        relayout();
        emit justifyItemsChanged(alignment);
    }
}

FluentGridAlignment FluentGrid::alignContent() const {
    return m_alignContent;
}

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

int FluentGrid::spacing() const {
    return qMax(m_rowSpacing, m_columnSpacing);
}

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

int FluentGrid::rowSpacing() const {
    return m_rowSpacing;
}

void FluentGrid::setRowSpacing(int spacing) {
    const int clampedSpacing = qMax(0, spacing);
    if (m_rowSpacing != clampedSpacing) {
        m_rowSpacing = clampedSpacing;
        m_layoutDirty = true;
        relayout();
        emit rowSpacingChanged(clampedSpacing);
    }
}

int FluentGrid::columnSpacing() const {
    return m_columnSpacing;
}

void FluentGrid::setColumnSpacing(int spacing) {
    const int clampedSpacing = qMax(0, spacing);
    if (m_columnSpacing != clampedSpacing) {
        m_columnSpacing = clampedSpacing;
        m_layoutDirty = true;
        relayout();
        emit columnSpacingChanged(clampedSpacing);
    }
}

QMargins FluentGrid::padding() const {
    return m_padding;
}

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

bool FluentGrid::autoColumns() const {
    return m_autoColumns;
}

void FluentGrid::setAutoColumns(bool auto_) {
    if (m_autoColumns != auto_) {
        m_autoColumns = auto_;
        m_layoutDirty = true;
        relayout();
        emit autoColumnsChanged(auto_);
    }
}

bool FluentGrid::autoRows() const {
    return m_autoRows;
}

void FluentGrid::setAutoRows(bool auto_) {
    if (m_autoRows != auto_) {
        m_autoRows = auto_;
        m_layoutDirty = true;
        relayout();
        emit autoRowsChanged(auto_);
    }
}

bool FluentGrid::isAnimated() const {
    return m_animated;
}

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

void FluentGrid::addItem(QWidget* widget, int row, int column, int rowSpan, int columnSpan) {
    if (widget) {
        FluentGridItem item(widget, row, column);
        item.rowSpan = qMax(1, rowSpan);
        item.columnSpan = qMax(1, columnSpan);
        addItem(item);
    }
}

void FluentGrid::addItem(const FluentGridItem& item) {
    if (!item.widget) return;
    
    m_items.append(item);
    item.widget->setParent(this);
    
    connect(item.widget, &QObject::destroyed, this, &FluentGrid::onItemDestroyed);
    
    m_layoutDirty = true;
    relayout();
    
    emit itemAdded(m_items.size() - 1);
}

void FluentGrid::insertItem(int index, QWidget* widget) {
    if (widget) {
        insertItem(index, FluentGridItem(widget));
    }
}

void FluentGrid::insertItem(int index, const FluentGridItem& item) {
    if (!item.widget) return;
    
    const int clampedIndex = qBound(0, index, m_items.size());
    m_items.insert(clampedIndex, item);
    item.widget->setParent(this);
    
    connect(item.widget, &QObject::destroyed, this, &FluentGrid::onItemDestroyed);
    
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
            disconnect(item.widget, &QObject::destroyed, this, &FluentGrid::onItemDestroyed);
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
            disconnect(item.widget, &QObject::destroyed, this, &FluentGrid::onItemDestroyed);
            item.widget->setParent(nullptr);
        }
    }
    
    m_items.clear();
    m_itemRects.clear();
    m_layoutDirty = true;
    relayout();
}

} // namespace FluentQt::Components
