// src/Components/FluentDataTable.cpp
#include "FluentQt/Components/FluentDataTable.h"
#include <QApplication>
#include <QFileDialog>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTextStream>
#include <QTimer>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentDataTable::FluentDataTable(QWidget* parent) : QWidget(parent) {
    setupUI();
    setupTable();
    setupToolbar();
    setupPagination();

    // Set default pagination
    m_pagination.currentPage = 1;
    m_pagination.pageSize = 25;
    m_pagination.totalItems = 0;

    updateTable();
    updatePagination();
    updateToolbar();
}

FluentDataTable::~FluentDataTable() = default;

void FluentDataTable::addColumn(const FluentTableColumn& column) {
    m_columns.append(column);
    updateTable();
}

void FluentDataTable::insertColumn(int index, const FluentTableColumn& column) {
    if (index >= 0 && index <= m_columns.size()) {
        m_columns.insert(index, column);
        updateTable();
    }
}

void FluentDataTable::removeColumn(const QString& id) {
    for (int i = 0; i < m_columns.size(); ++i) {
        if (m_columns[i].id == id) {
            removeColumn(i);
            break;
        }
    }
}

void FluentDataTable::removeColumn(int index) {
    if (index >= 0 && index < m_columns.size()) {
        m_columns.removeAt(index);
        updateTable();
    }
}

void FluentDataTable::setColumns(const QList<FluentTableColumn>& columns) {
    m_columns = columns;
    updateTable();
}

void FluentDataTable::setData(const QList<FluentTableRow>& rows) {
    m_rows = rows;
    m_pagination.totalItems = rows.size();
    applyFilters();
    updateTable();
    updatePagination();
    emit dataChanged();
}

void FluentDataTable::addRow(const FluentTableRow& row) {
    m_rows.append(row);
    m_pagination.totalItems = m_rows.size();
    applyFilters();
    animateRowInsertion(m_rows.size() - 1);
    updateTable();
    updatePagination();
    emit dataChanged();
}

void FluentDataTable::insertRow(int index, const FluentTableRow& row) {
    if (index >= 0 && index <= m_rows.size()) {
        m_rows.insert(index, row);
        m_pagination.totalItems = m_rows.size();
        applyFilters();
        animateRowInsertion(index);
        updateTable();
        updatePagination();
        emit dataChanged();
    }
}

void FluentDataTable::removeRow(int index) {
    if (index >= 0 && index < m_rows.size()) {
        animateRowRemoval(index);
        m_rows.removeAt(index);
        m_pagination.totalItems = m_rows.size();
        applyFilters();
        updateTable();
        updatePagination();
        emit dataChanged();
    }
}

void FluentDataTable::clearData() {
    m_rows.clear();
    m_filteredRows.clear();
    m_selectedRows.clear();
    m_pagination.totalItems = 0;
    m_pagination.currentPage = 1;
    updateTable();
    updatePagination();
    emit dataChanged();
    emit selectionChanged(m_selectedRows);
}

void FluentDataTable::setSelectionMode(SelectionMode mode) {
    if (m_selectionMode != mode) {
        m_selectionMode = mode;
        clearSelection();

        switch (mode) {
            case SelectionMode::None:
                m_table->setSelectionMode(QAbstractItemView::NoSelection);
                break;
            case SelectionMode::Single:
                m_table->setSelectionMode(QAbstractItemView::SingleSelection);
                break;
            case SelectionMode::Multiple:
                m_table->setSelectionMode(QAbstractItemView::MultiSelection);
                break;
        }
    }
}

QList<int> FluentDataTable::selectedRows() const { return m_selectedRows; }

void FluentDataTable::selectRow(int index, bool selected) {
    if (index >= 0 && index < m_filteredRows.size()) {
        if (selected) {
            if (m_selectionMode == SelectionMode::Single) {
                m_selectedRows.clear();
            }
            if (!m_selectedRows.contains(index)) {
                m_selectedRows.append(index);
            }
        } else {
            m_selectedRows.removeAll(index);
        }

        m_table->selectRow(index);
        emit selectionChanged(m_selectedRows);
    }
}

void FluentDataTable::selectAll() {
    if (m_selectionMode == SelectionMode::Multiple) {
        m_selectedRows.clear();
        for (int i = 0; i < m_filteredRows.size(); ++i) {
            m_selectedRows.append(i);
        }
        m_table->selectAll();
        emit selectionChanged(m_selectedRows);
    }
}

void FluentDataTable::clearSelection() {
    m_selectedRows.clear();
    m_table->clearSelection();
    emit selectionChanged(m_selectedRows);
}

void FluentDataTable::setSortingEnabled(bool enabled) {
    if (m_sortingEnabled != enabled) {
        m_sortingEnabled = enabled;
        m_table->setSortingEnabled(enabled);
        if (!enabled) {
            clearSorting();
        }
    }
}

void FluentDataTable::sortByColumn(const QString& columnId,
                                   Qt::SortOrder order) {
    if (!m_sortingEnabled)
        return;

    // Find column index
    int columnIndex = -1;
    for (int i = 0; i < m_columns.size(); ++i) {
        if (m_columns[i].id == columnId) {
            columnIndex = i;
            break;
        }
    }

    if (columnIndex >= 0) {
        m_currentSortColumn = columnId;
        m_currentSortOrder = order;
        applySorting();
        updateTable();
        emit sortingChanged(columnId, order);
    }
}

void FluentDataTable::clearSorting() {
    m_currentSortColumn.clear();
    m_currentSortOrder = Qt::AscendingOrder;
    applySorting();
    updateTable();
}

void FluentDataTable::setFilteringEnabled(bool enabled) {
    if (m_filteringEnabled != enabled) {
        m_filteringEnabled = enabled;
        if (!enabled) {
            clearFilters();
        }
        updateToolbar();
    }
}

void FluentDataTable::setFilter(const QString& columnId,
                                const QString& filter) {
    if (!m_filteringEnabled)
        return;

    if (filter.isEmpty()) {
        m_columnFilters.remove(columnId);
    } else {
        m_columnFilters[columnId] = filter;
    }

    applyFilters();
    updateTable();
    updatePagination();
    emit filterChanged(columnId, filter);
}

void FluentDataTable::setGlobalFilter(const QString& filter) {
    if (!m_filteringEnabled)
        return;

    m_globalFilter = filter;
    applyFilters();
    updateTable();
    updatePagination();
}

void FluentDataTable::clearFilters() {
    m_columnFilters.clear();
    m_globalFilter.clear();
    applyFilters();
    updateTable();
    updatePagination();
}

void FluentDataTable::setPaginationEnabled(bool enabled) {
    if (m_paginationEnabled != enabled) {
        m_paginationEnabled = enabled;
        m_paginationWidget->setVisible(enabled);
        updateTable();
        updatePagination();
    }
}

void FluentDataTable::setPagination(const FluentTablePagination& pagination) {
    m_pagination = pagination;
    updateTable();
    updatePagination();
}

void FluentDataTable::setAlternatingRowColors(bool enable) {
    m_table->setAlternatingRowColors(enable);
}

void FluentDataTable::setGridVisible(bool visible) {
    m_table->setShowGrid(visible);
}

void FluentDataTable::setHeaderVisible(bool visible) {
    m_table->horizontalHeader()->setVisible(visible);
}

void FluentDataTable::exportToCsv(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);

    // Write headers
    QStringList headers;
    for (const auto& column : m_columns) {
        if (column.visible) {
            headers.append(column.title);
        }
    }
    stream << headers.join(",") << "\n";

    // Write data
    for (const auto& row : m_filteredRows) {
        QStringList values;
        for (const auto& column : m_columns) {
            if (column.visible) {
                QString value = row.data.value(column.field).toString();
                // Escape commas and quotes
                if (value.contains(",") || value.contains("\"")) {
                    value = "\"" + value.replace("\"", "\"\"") + "\"";
                }
                values.append(value);
            }
        }
        stream << values.join(",") << "\n";
    }
}

void FluentDataTable::exportToJson(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }

    QJsonArray jsonArray;
    for (const auto& row : m_filteredRows) {
        QJsonObject jsonRow;
        for (const auto& column : m_columns) {
            if (column.visible) {
                jsonRow[column.field] =
                    QJsonValue::fromVariant(row.data.value(column.field));
            }
        }
        jsonArray.append(jsonRow);
    }

    QJsonDocument doc(jsonArray);
    file.write(doc.toJson());
}

void FluentDataTable::setSelectionEnabled(bool enabled) {
    if (m_selectionEnabled != enabled) {
        m_selectionEnabled = enabled;
        if (!enabled) {
            setSelectionMode(SelectionMode::None);
        } else {
            setSelectionMode(SelectionMode::Single);
        }
    }
}

void FluentDataTable::refresh() {
    applyFilters();
    applySorting();
    updateTable();
    updatePagination();
}

void FluentDataTable::goToPage(int page) {
    if (page >= 1 &&
        page <= (m_filteredRows.size() + m_pagination.pageSize - 1) /
                    m_pagination.pageSize) {
        m_pagination.currentPage = page;
        updateTable();
        updatePagination();
        emit pageChanged(page);
    }
}

void FluentDataTable::setPageSize(int size) {
    if (size > 0 && m_pagination.pageSizeOptions.contains(size)) {
        m_pagination.pageSize = size;
        m_pagination.currentPage = 1;  // Reset to first page
        updateTable();
        updatePagination();
        emit pageSizeChanged(size);
    }
}

void FluentDataTable::previousPage() {
    if (m_pagination.currentPage > 1) {
        goToPage(m_pagination.currentPage - 1);
    }
}

void FluentDataTable::nextPage() {
    int maxPage = (m_filteredRows.size() + m_pagination.pageSize - 1) /
                  m_pagination.pageSize;
    if (m_pagination.currentPage < maxPage) {
        goToPage(m_pagination.currentPage + 1);
    }
}

void FluentDataTable::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateTable();
}

void FluentDataTable::keyPressEvent(QKeyEvent* event) {
    if (m_selectionEnabled) {
        switch (event->key()) {
            case Qt::Key_A:
                if (event->modifiers() & Qt::ControlModifier) {
                    selectAll();
                    event->accept();
                    return;
                }
                break;
            case Qt::Key_Delete:
                // Could emit a signal for row deletion
                event->accept();
                return;
            default:
                break;
        }
    }
    QWidget::keyPressEvent(event);
}

void FluentDataTable::onCellClicked(int row, int column) {
    Q_UNUSED(column)
    if (m_selectionEnabled && row >= 0 && row < m_filteredRows.size()) {
        selectRow(row, true);
        emit rowClicked(row);
    }
}

void FluentDataTable::onCellDoubleClicked(int row, int column) {
    Q_UNUSED(column)
    if (row >= 0 && row < m_filteredRows.size()) {
        emit rowDoubleClicked(row);
    }
}

void FluentDataTable::onHeaderClicked(int logicalIndex) {
    if (m_sortingEnabled && logicalIndex >= 0 &&
        logicalIndex < m_columns.size()) {
        const QString& columnId = m_columns[logicalIndex].id;
        Qt::SortOrder newOrder = Qt::AscendingOrder;

        if (m_currentSortColumn == columnId &&
            m_currentSortOrder == Qt::AscendingOrder) {
            newOrder = Qt::DescendingOrder;
        }

        sortByColumn(columnId, newOrder);
    }
}

void FluentDataTable::onSelectionChanged() {
    m_selectedRows.clear();
    QList<QTableWidgetItem*> selectedItems = m_table->selectedItems();
    QSet<int> selectedRowSet;

    for (QTableWidgetItem* item : selectedItems) {
        selectedRowSet.insert(item->row());
    }

    m_selectedRows = selectedRowSet.values();
    emit selectionChanged(m_selectedRows);
}

void FluentDataTable::onPageSizeChanged() {
    if (m_pageSizeCombo) {
        int newSize = m_pageSizeCombo->currentData().toInt();
        setPageSize(newSize);
    }
}

void FluentDataTable::onFilterTextChanged() {
    // This would be connected to filter input widgets
    // Implementation depends on specific filter UI
}

void FluentDataTable::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
}

void FluentDataTable::setupTable() {
    m_table = new QTableWidget(this);
    m_table->setObjectName("dataTable");

    // Set item delegate for custom rendering
    auto* delegate = new FluentTableItemDelegate(this);
    m_table->setItemDelegate(delegate);

    // Configure table
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setAlternatingRowColors(true);
    m_table->setSortingEnabled(true);
    m_table->setShowGrid(true);

    // Configure headers
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);

    // Connect signals
    connect(m_table, &QTableWidget::cellClicked, this,
            &FluentDataTable::onCellClicked);
    connect(m_table, &QTableWidget::cellDoubleClicked, this,
            &FluentDataTable::onCellDoubleClicked);
    connect(m_table->horizontalHeader(), &QHeaderView::sectionClicked, this,
            &FluentDataTable::onHeaderClicked);
    connect(m_table, &QTableWidget::itemSelectionChanged, this,
            &FluentDataTable::onSelectionChanged);

    m_mainLayout->addWidget(m_table);
}

void FluentDataTable::setupToolbar() {
    m_toolbar = new QToolBar(this);
    m_toolbar->setObjectName("dataTableToolbar");

    // Add filter controls when filtering is enabled
    // This would be expanded based on specific requirements

    m_mainLayout->insertWidget(0, m_toolbar);
}

void FluentDataTable::setupPagination() {
    m_paginationWidget = new QWidget(this);
    m_paginationWidget->setObjectName("paginationWidget");
    m_paginationLayout = new QHBoxLayout(m_paginationWidget);

    // First page button
    m_firstPageButton = new QPushButton("<<", m_paginationWidget);
    m_firstPageButton->setToolTip("First Page");
    connect(m_firstPageButton, &QPushButton::clicked, this,
            [this]() { goToPage(1); });
    m_paginationLayout->addWidget(m_firstPageButton);

    // Previous page button
    m_previousPageButton = new QPushButton("<", m_paginationWidget);
    m_previousPageButton->setToolTip("Previous Page");
    connect(m_previousPageButton, &QPushButton::clicked, this,
            &FluentDataTable::previousPage);
    m_paginationLayout->addWidget(m_previousPageButton);

    // Page info label
    m_pageInfoLabel = new QLabel(m_paginationWidget);
    m_paginationLayout->addWidget(m_pageInfoLabel);

    // Next page button
    m_nextPageButton = new QPushButton(">", m_paginationWidget);
    m_nextPageButton->setToolTip("Next Page");
    connect(m_nextPageButton, &QPushButton::clicked, this,
            &FluentDataTable::nextPage);
    m_paginationLayout->addWidget(m_nextPageButton);

    // Last page button
    m_lastPageButton = new QPushButton(">>", m_paginationWidget);
    m_lastPageButton->setToolTip("Last Page");
    connect(m_lastPageButton, &QPushButton::clicked, this, [this]() {
        int maxPage = (m_filteredRows.size() + m_pagination.pageSize - 1) /
                      m_pagination.pageSize;
        goToPage(maxPage);
    });
    m_paginationLayout->addWidget(m_lastPageButton);

    m_paginationLayout->addStretch();

    // Page size selector
    m_paginationLayout->addWidget(
        new QLabel("Items per page:", m_paginationWidget));
    m_pageSizeCombo = new QComboBox(m_paginationWidget);
    for (int size : m_pagination.pageSizeOptions) {
        m_pageSizeCombo->addItem(QString::number(size), size);
    }
    connect(m_pageSizeCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &FluentDataTable::onPageSizeChanged);
    m_paginationLayout->addWidget(m_pageSizeCombo);

    m_mainLayout->addWidget(m_paginationWidget);
}

void FluentDataTable::updateTable() {
    if (!m_table)
        return;

    // Set column count
    m_table->setColumnCount(m_columns.size());

    // Set headers
    QStringList headers;
    for (const auto& column : m_columns) {
        headers.append(column.title);
    }
    m_table->setHorizontalHeaderLabels(headers);

    // Calculate visible rows for current page
    QList<FluentTableRow> visibleRows;
    if (m_paginationEnabled) {
        int startIndex = (m_pagination.currentPage - 1) * m_pagination.pageSize;
        int endIndex =
            qMin(startIndex + m_pagination.pageSize, m_filteredRows.size());

        for (int i = startIndex; i < endIndex; ++i) {
            visibleRows.append(m_filteredRows[i]);
        }
    } else {
        visibleRows = m_filteredRows;
    }

    // Set row count
    m_table->setRowCount(visibleRows.size());

    // Populate data
    for (int row = 0; row < visibleRows.size(); ++row) {
        const FluentTableRow& rowData = visibleRows[row];

        for (int col = 0; col < m_columns.size(); ++col) {
            const FluentTableColumn& column = m_columns[col];
            QVariant cellData = rowData.data.value(column.field);

            QString displayText;
            if (column.formatter) {
                displayText = column.formatter(cellData);
            } else if (!column.format.isEmpty()) {
                // Apply format string (simplified)
                displayText = cellData.toString();
            } else {
                displayText = cellData.toString();
            }

            auto* item = new QTableWidgetItem(displayText);
            item->setTextAlignment(column.alignment);

            // Apply custom colors if set
            if (rowData.backgroundColor.isValid()) {
                item->setBackground(rowData.backgroundColor);
            }
            if (rowData.textColor.isValid()) {
                item->setForeground(rowData.textColor);
            }

            // Set item as read-only by default
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);

            m_table->setItem(row, col, item);
        }
    }

    // Update column widths
    for (int col = 0; col < m_columns.size(); ++col) {
        const FluentTableColumn& column = m_columns[col];
        if (column.width > 0) {
            m_table->setColumnWidth(col, column.width);
        }
        m_table->setColumnHidden(col, !column.visible);
    }
}

void FluentDataTable::updatePagination() {
    if (!m_paginationWidget || !m_paginationEnabled)
        return;

    int totalPages = (m_filteredRows.size() + m_pagination.pageSize - 1) /
                     m_pagination.pageSize;
    totalPages = qMax(1, totalPages);

    // Update buttons
    m_firstPageButton->setEnabled(m_pagination.currentPage > 1);
    m_previousPageButton->setEnabled(m_pagination.currentPage > 1);
    m_nextPageButton->setEnabled(m_pagination.currentPage < totalPages);
    m_lastPageButton->setEnabled(m_pagination.currentPage < totalPages);

    // Update page info
    int startItem = (m_pagination.currentPage - 1) * m_pagination.pageSize + 1;
    int endItem = qMin(m_pagination.currentPage * m_pagination.pageSize,
                       m_filteredRows.size());

    m_pageInfoLabel->setText(QString("Page %1 of %2 (%3-%4 of %5 items)")
                                 .arg(m_pagination.currentPage)
                                 .arg(totalPages)
                                 .arg(startItem)
                                 .arg(endItem)
                                 .arg(m_filteredRows.size()));

    // Update page size combo
    for (int i = 0; i < m_pageSizeCombo->count(); ++i) {
        if (m_pageSizeCombo->itemData(i).toInt() == m_pagination.pageSize) {
            m_pageSizeCombo->setCurrentIndex(i);
            break;
        }
    }
}

void FluentDataTable::updateToolbar() {
    // Update toolbar based on current settings
    // This would be expanded based on specific filter UI requirements
}

void FluentDataTable::applyFilters() {
    m_filteredRows.clear();

    for (const FluentTableRow& row : m_rows) {
        bool passesFilter = true;

        // Apply column-specific filters
        for (auto it = m_columnFilters.begin(); it != m_columnFilters.end();
             ++it) {
            const QString& columnId = it.key();
            const QString& filterText = it.value();

            // Find column
            QString fieldName;
            for (const auto& column : m_columns) {
                if (column.id == columnId) {
                    fieldName = column.field;
                    break;
                }
            }

            if (!fieldName.isEmpty()) {
                QString cellValue = row.data.value(fieldName).toString();
                if (!cellValue.contains(filterText, Qt::CaseInsensitive)) {
                    passesFilter = false;
                    break;
                }
            }
        }

        // Apply global filter
        if (passesFilter && !m_globalFilter.isEmpty()) {
            bool foundMatch = false;
            for (const auto& column : m_columns) {
                QString cellValue = row.data.value(column.field).toString();
                if (cellValue.contains(m_globalFilter, Qt::CaseInsensitive)) {
                    foundMatch = true;
                    break;
                }
            }
            passesFilter = foundMatch;
        }

        if (passesFilter) {
            m_filteredRows.append(row);
        }
    }
}

void FluentDataTable::applySorting() {
    if (m_currentSortColumn.isEmpty() || !m_sortingEnabled) {
        return;
    }

    // Find column field name
    QString fieldName;
    for (const auto& column : m_columns) {
        if (column.id == m_currentSortColumn) {
            fieldName = column.field;
            break;
        }
    }

    if (fieldName.isEmpty())
        return;

    // Sort filtered rows
    std::sort(
        m_filteredRows.begin(), m_filteredRows.end(),
        [fieldName, this](const FluentTableRow& a, const FluentTableRow& b) {
            QVariant valueA = a.data.value(fieldName);
            QVariant valueB = b.data.value(fieldName);

            if (m_currentSortOrder == Qt::AscendingOrder) {
                return valueA.toString() < valueB.toString();
            } else {
                return valueA.toString() > valueB.toString();
            }
        });
}

void FluentDataTable::updateRowVisibility() {
    // Update row visibility based on current filters and pagination
    updateTable();
}

void FluentDataTable::animateRowInsertion(int row) {
    Q_UNUSED(row)
    // Simple animation placeholder - could be enhanced with actual animations
}

void FluentDataTable::animateRowRemoval(int row) {
    Q_UNUSED(row)
    // Simple animation placeholder - could be enhanced with actual animations
}

// FluentTableItemDelegate Implementation
FluentTableItemDelegate::FluentTableItemDelegate(QObject* parent)
    : QItemDelegate(parent) {}

void FluentTableItemDelegate::paint(QPainter* painter,
                                    const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const {
    painter->save();

    paintBackground(painter, option, index);
    paintSelection(painter, option);
    paintText(painter, option, index);

    painter->restore();
}

QSize FluentTableItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const {
    return QItemDelegate::sizeHint(option, index);
}

QWidget* FluentTableItemDelegate::createEditor(
    QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index) const {
    return QItemDelegate::createEditor(parent, option, index);
}

void FluentTableItemDelegate::paintBackground(
    QPainter* painter, const QStyleOptionViewItem& option,
    const QModelIndex& index) const {
    Q_UNUSED(index)

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else if (option.features & QStyleOptionViewItem::Alternate) {
        painter->fillRect(option.rect, option.palette.alternateBase());
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }
}

void FluentTableItemDelegate::paintText(QPainter* painter,
                                        const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const {
    QString text = index.data(Qt::DisplayRole).toString();
    QRect textRect = option.rect.adjusted(4, 2, -4, -2);

    QPen textPen;
    if (option.state & QStyle::State_Selected) {
        textPen = QPen(option.palette.highlightedText().color());
    } else {
        textPen = QPen(option.palette.text().color());
    }

    painter->setPen(textPen);
    painter->drawText(textRect, option.displayAlignment, text);
}

void FluentTableItemDelegate::paintSelection(
    QPainter* painter, const QStyleOptionViewItem& option) const {
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }
}

// FluentVirtualTable Implementation
FluentVirtualTable::FluentVirtualTable(QWidget* parent)
    : FluentDataTable(parent) {
    setupVirtualScrolling();
}

void FluentVirtualTable::setDataSource(
    std::function<FluentTableRow(int)> dataSource) {
    m_dataSource = dataSource;
    m_cachedRows.clear();
    updateVisibleRows();
}

void FluentVirtualTable::setRowCount(int count) {
    m_totalRowCount = count;
    updateVisibleRows();
}

void FluentVirtualTable::setVisibleRowCount(int count) {
    m_visibleRowCount = qMax(1, count);
    updateVisibleRows();
}

void FluentVirtualTable::setBufferSize(int size) {
    m_bufferSize = qMax(0, size);
}

void FluentVirtualTable::onScrollValueChanged(int value) {
    Q_UNUSED(value)
    updateVisibleRows();
}

void FluentVirtualTable::updateVisibleRows() {
    if (!m_dataSource)
        return;

    loadVisibleData();
    updateTable();
}

void FluentVirtualTable::setupVirtualScrolling() {
    // Connect to scroll bar for virtual scrolling
    if (m_table && m_table->verticalScrollBar()) {
        connect(m_table->verticalScrollBar(), &QScrollBar::valueChanged, this,
                &FluentVirtualTable::onScrollValueChanged);
    }
}

void FluentVirtualTable::loadVisibleData() {
    if (!m_dataSource)
        return;

    // Calculate visible range
    int startRow = m_firstVisibleRow;
    int endRow =
        qMin(startRow + m_visibleRowCount + m_bufferSize, m_totalRowCount);

    // Load data for visible range
    QList<FluentTableRow> visibleData;
    for (int i = startRow; i < endRow; ++i) {
        if (m_cachedRows.contains(i)) {
            visibleData.append(m_cachedRows[i]);
        } else {
            FluentTableRow row = m_dataSource(i);
            m_cachedRows[i] = row;
            visibleData.append(row);
        }
    }

    // Update filtered rows with visible data
    m_filteredRows = visibleData;

    emit dataRequested(startRow, endRow - startRow);
}

}  // namespace FluentQt::Components
