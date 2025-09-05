// src/Components/FluentDataTable.cpp
#include "FluentQt/Components/FluentDataTable.h"
#include <QAccessible>
#include <QAccessibleWidget>
#include <QApplication>
#include <QDebug>
#include <QEasingCurve>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QScrollBar>
#include <QSequentialAnimationGroup>
#include <QStackedWidget>
#include <QStyleOption>
#include <QTextStream>
#include <QTimer>
#include "FluentQt/Styling/FluentTheme.h"

namespace FluentQt::Components {

FluentDataTable::FluentDataTable(QWidget* parent) : QWidget(parent) {
    // Initialize theme integration first
    m_theme = std::make_shared<FluentTheme>();

    // Setup enhanced UI components
    setupUI();
    setupTable();
    setupToolbar();
    setupPagination();
    setupStateWidgets();
    setupAccessibility();
    setupTheme();
    setupAnimations();
    setupKeyboardNavigation();

    // Set default configuration
    m_config = FluentTableConfig{};
    m_pagination.currentPage = 1;
    m_pagination.pageSize = 25;
    m_pagination.totalItems = 0;
    m_currentState = FluentTableState::Ready;

    // Initialize timers
    m_themeUpdateTimer = new QTimer(this);
    m_themeUpdateTimer->setSingleShot(true);
    m_themeUpdateTimer->setInterval(100);
    connect(m_themeUpdateTimer, &QTimer::timeout, this,
            &FluentDataTable::updateTheme);

    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);

    // Apply initial theme and updates
    applyFluentTheme();
    updateTable();
    updatePagination();
    updateToolbar();
    updateAccessibility();

    // Set initial focus policy for keyboard navigation
    setFocusPolicy(Qt::StrongFocus);

    qDebug()
        << "FluentDataTable initialized with enhanced Fluent UI compliance";
}

FluentDataTable::~FluentDataTable() = default;

// Enhanced configuration methods
void FluentDataTable::setConfig(const FluentTableConfig& config) {
    if (m_config.useFluentTheme != config.useFluentTheme ||
        m_config.highContrastMode != config.highContrastMode ||
        m_config.adaptToSystemTheme != config.adaptToSystemTheme) {
        m_config = config;
        applyFluentTheme();
        emit configChanged();
    } else {
        m_config = config;
    }

    // Update accessibility settings
    if (m_config.screenReaderSupport || m_config.keyboardNavigation) {
        updateAccessibility();
    }

    // Update animation settings
    if (m_config.reduceMotion) {
        // Disable animations for accessibility
        if (m_fadeAnimation) {
            m_fadeAnimation->setDuration(0);
        }
    }
}

// Enhanced state management methods
void FluentDataTable::setState(FluentTableState state) {
    if (m_currentState == state)
        return;

    FluentTableState previousState = m_currentState;
    m_currentState = state;

    // Animate state change if animations are enabled
    if (!m_config.reduceMotion &&
        m_config.defaultAnimation != FluentTableAnimation::None) {
        animateStateChange(previousState, state);
    } else {
        updateStateWidget();
    }

    emit stateChanged(state);

    // Announce state change to screen readers
    if (m_config.screenReaderSupport) {
        QString announcement;
        switch (state) {
            case FluentTableState::Loading:
                announcement = "Loading data...";
                break;
            case FluentTableState::Ready:
                announcement = QString("Data loaded. %1 items available.")
                                   .arg(m_filteredRows.size());
                break;
            case FluentTableState::Empty:
                announcement = "No data available.";
                break;
            case FluentTableState::Error:
                announcement = "Error loading data.";
                break;
        }
        announceToScreenReader(announcement);
    }
}

void FluentDataTable::setLoading(bool loading, const QString& message) {
    if (loading) {
        m_currentStateMessage = message.isEmpty() ? "Loading data..." : message;
        setState(FluentTableState::Loading);
    } else {
        setState(FluentTableState::Ready);
    }
}

void FluentDataTable::setError(const QString& errorMessage,
                               const QString& details) {
    m_currentStateMessage = errorMessage;
    m_currentErrorDetails = details;
    setState(FluentTableState::Error);
}

void FluentDataTable::setEmpty(const QString& emptyMessage) {
    m_currentStateMessage =
        emptyMessage.isEmpty() ? "No data available" : emptyMessage;
    setState(FluentTableState::Empty);
}

// Enhanced theme integration methods
void FluentDataTable::setUseFluentTheme(bool use) {
    if (m_config.useFluentTheme != use) {
        m_config.useFluentTheme = use;
        applyFluentTheme();
        emit themeChanged();
    }
}

void FluentDataTable::setHighContrastMode(bool enabled) {
    if (m_config.highContrastMode != enabled) {
        m_config.highContrastMode = enabled;
        applyFluentTheme();
        emit themeChanged();
    }
}

void FluentDataTable::setReduceMotion(bool reduce) {
    if (m_config.reduceMotion != reduce) {
        m_config.reduceMotion = reduce;

        // Update animation durations
        if (reduce) {
            if (m_fadeAnimation)
                m_fadeAnimation->setDuration(0);
            if (m_animationGroup) {
                for (int i = 0; i < m_animationGroup->animationCount(); ++i) {
                    if (auto* anim = qobject_cast<QPropertyAnimation*>(
                            m_animationGroup->animationAt(i))) {
                        anim->setDuration(0);
                    }
                }
            }
        } else {
            if (m_fadeAnimation)
                m_fadeAnimation->setDuration(m_config.animationDuration);
        }

        emit configChanged();
    }
}

// Enhanced accessibility methods
void FluentDataTable::setAriaLabel(const QString& label) {
    m_config.tableLabel = label;
    updateAccessibility();
    emit accessibilityChanged();
}

void FluentDataTable::setAriaDescription(const QString& description) {
    m_config.tableDescription = description;
    updateAccessibility();
    emit accessibilityChanged();
}

void FluentDataTable::setAccessibilityEnabled(bool enabled) {
    m_config.screenReaderSupport = enabled;
    m_config.keyboardNavigation = enabled;
    m_config.focusManagement = enabled;
    updateAccessibility();
    emit accessibilityChanged();
}

void FluentDataTable::announceToScreenReader(const QString& message) {
    if (!m_config.screenReaderSupport || message.isEmpty())
        return;

    // Use QAccessible to announce to screen readers
    QAccessibleEvent event(this, QAccessible::Alert);
    // Note: setText is not available in Qt6, using alternative approach
    QAccessible::updateAccessibility(&event);
    QAccessible::updateAccessibility(&event);
}

void FluentDataTable::addColumn(const FluentTableColumn& column) {
    m_columns.append(column);
    if (m_config.incrementalUpdates) {
        performIncrementalUpdate();
    } else {
        updateTable();
    }
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

    // Use Fluent spacing if theme is enabled
    if (m_config.useFluentTheme && m_theme) {
        int spacing = m_theme->spacing("m");
        int padding = m_theme->paddingValue("m");
        m_mainLayout->setContentsMargins(padding, padding, padding, padding);
        m_mainLayout->setSpacing(spacing);
    } else {
        m_mainLayout->setContentsMargins(0, 0, 0, 0);
        m_mainLayout->setSpacing(0);
    }

    // Create stacked widget for different states
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setObjectName("tableStackedWidget");

    // Set accessibility properties
    if (m_config.screenReaderSupport) {
        m_stackedWidget->setAccessibleName("Data table container");
        m_stackedWidget->setAccessibleDescription(
            "Container for table data and state widgets");
    }
}

void FluentDataTable::setupStateWidgets() {
    // Loading widget
    m_loadingWidget = new QWidget();
    m_loadingWidget->setObjectName("loadingWidget");

    auto* loadingLayout = new QVBoxLayout(m_loadingWidget);
    loadingLayout->setAlignment(Qt::AlignCenter);

    m_progressBar = new QProgressBar();
    m_progressBar->setObjectName("loadingProgressBar");
    m_progressBar->setRange(0, 0);  // Indeterminate progress
    m_progressBar->setTextVisible(false);

    m_stateLabel = new QLabel("Loading data...");
    m_stateLabel->setObjectName("stateLabel");
    m_stateLabel->setAlignment(Qt::AlignCenter);

    loadingLayout->addWidget(m_progressBar);
    loadingLayout->addWidget(m_stateLabel);

    // Error widget
    m_errorWidget = new QWidget();
    m_errorWidget->setObjectName("errorWidget");

    auto* errorLayout = new QVBoxLayout(m_errorWidget);
    errorLayout->setAlignment(Qt::AlignCenter);

    auto* errorLabel = new QLabel("Error loading data");
    errorLabel->setObjectName("errorLabel");
    errorLabel->setAlignment(Qt::AlignCenter);

    m_retryButton = new QPushButton("Retry");
    m_retryButton->setObjectName("retryButton");
    connect(m_retryButton, &QPushButton::clicked, this, [this]() {
        setState(FluentTableState::Loading);
        // Emit signal for parent to handle retry
        emit dataLoadFailed("Retry requested");
    });

    errorLayout->addWidget(errorLabel);
    errorLayout->addWidget(m_retryButton);

    // Empty widget
    m_emptyWidget = new QWidget();
    m_emptyWidget->setObjectName("emptyWidget");

    auto* emptyLayout = new QVBoxLayout(m_emptyWidget);
    emptyLayout->setAlignment(Qt::AlignCenter);

    auto* emptyLabel = new QLabel("No data available");
    emptyLabel->setObjectName("emptyLabel");
    emptyLabel->setAlignment(Qt::AlignCenter);

    emptyLayout->addWidget(emptyLabel);

    // Add state widgets to stacked widget
    m_stackedWidget->addWidget(m_table);          // Index 0: Ready state
    m_stackedWidget->addWidget(m_loadingWidget);  // Index 1: Loading state
    m_stackedWidget->addWidget(m_errorWidget);    // Index 2: Error state
    m_stackedWidget->addWidget(m_emptyWidget);    // Index 3: Empty state

    m_mainLayout->insertWidget(1, m_stackedWidget);  // Insert after toolbar
}

void FluentDataTable::setupAccessibility() {
    if (!m_config.screenReaderSupport)
        return;

    // Set main widget accessibility properties
    setAccessibleName(m_config.tableLabel.isEmpty() ? "Data Table"
                                                    : m_config.tableLabel);
    setAccessibleDescription(m_config.tableDescription);

    // Set table accessibility properties
    if (m_table) {
        m_table->setAccessibleName("Table data");
        m_table->setAccessibleDescription(
            "Data table with sortable columns and pagination");

        // Set ARIA role
        m_table->setProperty("role", m_config.tableRole);

        // Enable keyboard navigation
        m_table->setTabKeyNavigation(true);
        m_table->setFocusPolicy(Qt::StrongFocus);
    }

    // Set pagination accessibility
    if (m_paginationWidget) {
        m_paginationWidget->setAccessibleName(m_pagination.ariaLabel);
        m_paginationWidget->setAccessibleDescription(
            "Table pagination controls");
    }

    // Set button accessibility
    if (m_firstPageButton) {
        m_firstPageButton->setAccessibleName("Go to first page");
        m_firstPageButton->setAccessibleDescription(
            "Navigate to the first page of results");
    }
    if (m_previousPageButton) {
        m_previousPageButton->setAccessibleName("Go to previous page");
        m_previousPageButton->setAccessibleDescription(
            "Navigate to the previous page of results");
    }
    if (m_nextPageButton) {
        m_nextPageButton->setAccessibleName("Go to next page");
        m_nextPageButton->setAccessibleDescription(
            "Navigate to the next page of results");
    }
    if (m_lastPageButton) {
        m_lastPageButton->setAccessibleName("Go to last page");
        m_lastPageButton->setAccessibleDescription(
            "Navigate to the last page of results");
    }
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

// Enhanced theme integration methods
void FluentDataTable::setupTheme() {
    if (!m_config.useFluentTheme || !m_theme)
        return;

    // Connect to theme changes
    connect(m_theme.get(), &FluentTheme::themeChanged, this, [this]() {
        if (m_config.adaptToSystemTheme) {
            m_themeUpdateTimer->start();
        }
    });

    applyFluentTheme();
}

void FluentDataTable::applyFluentTheme() {
    if (!m_config.useFluentTheme || !m_theme)
        return;

    updateColors();
    updateFonts();
    updateSpacing();
    updateBorderRadius();
    updateElevation();

    // Apply theme to child widgets
    if (m_table) {
        m_table->setStyleSheet(generateTableStyleSheet());
    }

    if (m_paginationWidget) {
        m_paginationWidget->setStyleSheet(generatePaginationStyleSheet());
    }

    if (m_toolbar) {
        m_toolbar->setStyleSheet(generateToolbarStyleSheet());
    }

    // Apply theme to state widgets
    if (m_loadingWidget) {
        m_loadingWidget->setStyleSheet(generateStateWidgetStyleSheet());
    }
    if (m_errorWidget) {
        m_errorWidget->setStyleSheet(generateStateWidgetStyleSheet());
    }
    if (m_emptyWidget) {
        m_emptyWidget->setStyleSheet(generateStateWidgetStyleSheet());
    }
}

void FluentDataTable::updateColors() {
    if (!m_config.useFluentTheme || !m_theme)
        return;

    // Get current palette based on theme mode and high contrast
    const auto& palette = m_theme->currentPalette();

    // Set main widget colors
    QPalette widgetPalette = this->palette();
    widgetPalette.setColor(QPalette::Window, palette.surface);
    widgetPalette.setColor(QPalette::WindowText, palette.neutralPrimary);
    widgetPalette.setColor(QPalette::Base, palette.surface);
    widgetPalette.setColor(QPalette::Text, palette.neutralPrimary);
    widgetPalette.setColor(QPalette::Button, palette.neutralLighter);
    widgetPalette.setColor(QPalette::ButtonText, palette.neutralPrimary);
    widgetPalette.setColor(QPalette::Highlight, palette.accent);
    widgetPalette.setColor(QPalette::HighlightedText, palette.neutralLightest);

    // Apply high contrast adjustments if enabled
    if (m_config.highContrastMode) {
        widgetPalette.setColor(QPalette::Window, QColor("#000000"));
        widgetPalette.setColor(QPalette::WindowText, QColor("#ffffff"));
        widgetPalette.setColor(QPalette::Base, QColor("#000000"));
        widgetPalette.setColor(QPalette::Text, QColor("#ffffff"));
        widgetPalette.setColor(QPalette::Highlight, QColor("#ffff00"));
        widgetPalette.setColor(QPalette::HighlightedText, QColor("#000000"));
    }

    setPalette(widgetPalette);
}

void FluentDataTable::updateFonts() {
    if (!m_config.useFluentTheme || !m_theme)
        return;

    // Apply Fluent typography
    QFont bodyFont = m_theme->bodyFont();
    setFont(bodyFont);

    // Set specific fonts for different elements
    if (m_table) {
        m_table->setFont(bodyFont);

        // Set header font
        QFont headerFont = m_theme->subtitleFont();
        m_table->horizontalHeader()->setFont(headerFont);
    }

    if (m_stateLabel) {
        QFont titleFont = m_theme->titleSmallFont();
        m_stateLabel->setFont(titleFont);
    }

    if (m_pageInfoLabel) {
        QFont captionFont = m_theme->captionFont();
        m_pageInfoLabel->setFont(captionFont);
    }
}

void FluentDataTable::updateSpacing() {
    if (!m_config.useFluentTheme || !m_theme)
        return;

    // Update layout spacing using Fluent tokens
    if (m_mainLayout) {
        int spacing = m_theme->spacing("m");
        int padding = m_theme->paddingValue("m");
        m_mainLayout->setSpacing(spacing);
        m_mainLayout->setContentsMargins(padding, padding, padding, padding);
    }

    if (m_paginationLayout) {
        int spacing = m_theme->spacing("s");
        m_paginationLayout->setSpacing(spacing);
    }
}

void FluentDataTable::updateBorderRadius() {
    if (!m_config.useFluentTheme || !m_theme)
        return;

    // Border radius will be applied via stylesheets in applyFluentTheme()
    // This method can be extended for specific border radius updates
}

void FluentDataTable::updateElevation() {
    if (!m_config.useFluentTheme || !m_theme)
        return;

    // Apply elevation (shadow) effects
    int elevation = m_theme->elevation("medium");

    if (m_table && elevation > 0) {
        auto* shadowEffect = new QGraphicsDropShadowEffect();
        shadowEffect->setBlurRadius(elevation);
        shadowEffect->setColor(m_theme->currentPalette().shadowMedium);
        shadowEffect->setOffset(0, 2);
        m_table->setGraphicsEffect(shadowEffect);
    }
}

QString FluentDataTable::generateTableStyleSheet() {
    if (!m_config.useFluentTheme || !m_theme)
        return QString();

    const auto& palette = m_theme->currentPalette();
    int borderRadius = m_theme->borderRadius("medium");
    int borderWidth = m_theme->strokeWidth("border");

    return QString(R"(
        QTableWidget {
            background-color: %1;
            color: %2;
            border: %3px solid %4;
            border-radius: %5px;
            gridline-color: %6;
            selection-background-color: %7;
            selection-color: %8;
        }
        QTableWidget::item {
            padding: 8px;
            border: none;
        }
        QTableWidget::item:selected {
            background-color: %7;
            color: %8;
        }
        QTableWidget::item:hover {
            background-color: %9;
        }
        QHeaderView::section {
            background-color: %10;
            color: %2;
            padding: 8px;
            border: none;
            border-bottom: %3px solid %4;
            font-weight: 600;
        }
        QHeaderView::section:hover {
            background-color: %11;
        }
    )")
        .arg(palette.surface.name())
        .arg(palette.neutralPrimary.name())
        .arg(borderWidth)
        .arg(palette.border.name())
        .arg(borderRadius)
        .arg(palette.borderSecondary.name())
        .arg(palette.accent.name())
        .arg(palette.neutralLightest.name())
        .arg(palette.hover.name())
        .arg(palette.surfaceSecondary.name())
        .arg(palette.surfaceTertiary.name());
}

QString FluentDataTable::generatePaginationStyleSheet() {
    if (!m_config.useFluentTheme || !m_theme)
        return QString();

    const auto& palette = m_theme->currentPalette();
    int borderRadius = m_theme->borderRadius("small");

    return QString(R"(
        QPushButton {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: %4px;
            padding: 6px 12px;
            min-width: 32px;
        }
        QPushButton:hover {
            background-color: %5;
            border-color: %6;
        }
        QPushButton:pressed {
            background-color: %7;
        }
        QPushButton:disabled {
            background-color: %8;
            color: %9;
            border-color: %10;
        }
        QComboBox {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: %4px;
            padding: 4px 8px;
        }
        QComboBox:hover {
            border-color: %6;
        }
    )")
        .arg(palette.neutralLighter.name())
        .arg(palette.neutralPrimary.name())
        .arg(palette.border.name())
        .arg(borderRadius)
        .arg(palette.hover.name())
        .arg(palette.borderHover.name())
        .arg(palette.pressed.name())
        .arg(palette.disabled.name())
        .arg(palette.neutralTertiary.name())
        .arg(palette.borderDisabled.name());
}

QString FluentDataTable::generateToolbarStyleSheet() {
    if (!m_config.useFluentTheme || !m_theme)
        return QString();

    const auto& palette = m_theme->currentPalette();

    return QString(R"(
        QToolBar {
            background-color: %1;
            border: none;
            spacing: 8px;
        }
    )")
        .arg(palette.surfaceSecondary.name());
}

QString FluentDataTable::generateStateWidgetStyleSheet() {
    if (!m_config.useFluentTheme || !m_theme)
        return QString();

    const auto& palette = m_theme->currentPalette();

    return QString(R"(
        QWidget {
            background-color: %1;
            color: %2;
        }
        QLabel {
            color: %3;
        }
        QProgressBar {
            border: 2px solid %4;
            border-radius: 5px;
            background-color: %5;
        }
        QProgressBar::chunk {
            background-color: %6;
            border-radius: 3px;
        }
    )")
        .arg(palette.surface.name())
        .arg(palette.neutralPrimary.name())
        .arg(palette.neutralSecondary.name())
        .arg(palette.border.name())
        .arg(palette.neutralLighter.name())
        .arg(palette.accent.name());
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

// Enhanced animation methods
void FluentDataTable::setupAnimations() {
    // Create fade animation for state changes
    m_fadeAnimation = new QPropertyAnimation(this, "windowOpacity");
    m_fadeAnimation->setDuration(m_config.animationDuration);
    m_fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Create opacity effect for smooth transitions
    m_opacityEffect = new QGraphicsOpacityEffect();
    m_opacityEffect->setOpacity(1.0);

    // Create animation groups for complex animations
    m_animationGroup = new QParallelAnimationGroup(this);
    m_sequentialAnimationGroup = new QSequentialAnimationGroup(this);

    // Connect animation signals
    connect(m_fadeAnimation, &QPropertyAnimation::started, this,
            [this]() { emit animationStarted(m_config.defaultAnimation); });

    connect(m_fadeAnimation, &QPropertyAnimation::finished, this,
            [this]() { emit animationFinished(m_config.defaultAnimation); });
}

void FluentDataTable::setupKeyboardNavigation() {
    if (!m_config.keyboardNavigation)
        return;

    // Enable keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // Install event filter for advanced keyboard handling
    installEventFilter(this);

    // Set tab order for accessibility
    if (m_table && m_paginationWidget) {
        setTabOrder(m_table, m_firstPageButton);
        setTabOrder(m_firstPageButton, m_previousPageButton);
        setTabOrder(m_previousPageButton, m_nextPageButton);
        setTabOrder(m_nextPageButton, m_lastPageButton);
        setTabOrder(m_lastPageButton, m_pageSizeCombo);
    }
}

void FluentDataTable::animateRowInsertion(int row,
                                          FluentTableAnimation animation) {
    if (m_config.reduceMotion || animation == FluentTableAnimation::None) {
        return;
    }

    if (!m_table || row < 0 || row >= m_table->rowCount())
        return;

    switch (animation) {
        case FluentTableAnimation::Fade: {
            // Create fade-in effect for the new row
            for (int col = 0; col < m_table->columnCount(); ++col) {
                if (auto* item = m_table->item(row, col)) {
                    auto* effect = new QGraphicsOpacityEffect();
                    effect->setOpacity(0.0);
                    item->setData(Qt::UserRole, QVariant::fromValue(effect));

                    auto* fadeIn = new QPropertyAnimation(effect, "opacity");
                    fadeIn->setDuration(m_config.animationDuration);
                    fadeIn->setStartValue(0.0);
                    fadeIn->setEndValue(1.0);
                    fadeIn->setEasingCurve(QEasingCurve::OutCubic);
                    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
                }
            }
            break;
        }
        case FluentTableAnimation::FluentMotion: {
            // Implement Fluent Design motion principles
            animateDataUpdate(animation);
            break;
        }
        default:
            break;
    }
}

void FluentDataTable::animateRowRemoval(int row,
                                        FluentTableAnimation animation) {
    if (m_config.reduceMotion || animation == FluentTableAnimation::None) {
        return;
    }

    if (!m_table || row < 0 || row >= m_table->rowCount())
        return;

    switch (animation) {
        case FluentTableAnimation::Fade: {
            // Create fade-out effect for the row being removed
            for (int col = 0; col < m_table->columnCount(); ++col) {
                if (auto* item = m_table->item(row, col)) {
                    auto* effect = new QGraphicsOpacityEffect();
                    effect->setOpacity(1.0);
                    item->setData(Qt::UserRole, QVariant::fromValue(effect));

                    auto* fadeOut = new QPropertyAnimation(effect, "opacity");
                    fadeOut->setDuration(m_config.animationDuration);
                    fadeOut->setStartValue(1.0);
                    fadeOut->setEndValue(0.0);
                    fadeOut->setEasingCurve(QEasingCurve::InCubic);
                    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
                }
            }
            break;
        }
        case FluentTableAnimation::FluentMotion: {
            // Implement Fluent Design motion principles
            animateDataUpdate(animation);
            break;
        }
        default:
            break;
    }
}

void FluentDataTable::animateStateChange(FluentTableState fromState,
                                         FluentTableState toState) {
    if (m_config.reduceMotion) {
        updateStateWidget();
        return;
    }

    // Animate transition between states
    if (m_stackedWidget) {
        int targetIndex = static_cast<int>(toState);

        // Create smooth transition animation
        auto* fadeOut = new QPropertyAnimation(m_opacityEffect, "opacity");
        fadeOut->setDuration(m_config.animationDuration / 2);
        fadeOut->setStartValue(1.0);
        fadeOut->setEndValue(0.0);
        fadeOut->setEasingCurve(QEasingCurve::InCubic);

        auto* fadeIn = new QPropertyAnimation(m_opacityEffect, "opacity");
        fadeIn->setDuration(m_config.animationDuration / 2);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);
        fadeIn->setEasingCurve(QEasingCurve::OutCubic);

        // Connect animations
        connect(fadeOut, &QPropertyAnimation::finished, this,
                [this, targetIndex, fadeIn]() {
                    updateStateWidget();
                    m_stackedWidget->setCurrentIndex(targetIndex);
                    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
                });

        m_stackedWidget->setGraphicsEffect(m_opacityEffect);
        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void FluentDataTable::animateDataUpdate(FluentTableAnimation animation) {
    if (m_config.reduceMotion || animation == FluentTableAnimation::None) {
        return;
    }

    // Implement smooth data update animations
    switch (animation) {
        case FluentTableAnimation::FluentMotion: {
            // Use Fluent Design motion principles
            if (m_table) {
                auto* effect = new QGraphicsOpacityEffect();
                effect->setOpacity(1.0);
                m_table->setGraphicsEffect(effect);

                auto* animation = new QPropertyAnimation(effect, "opacity");
                animation->setDuration(m_config.animationDuration);
                animation->setStartValue(0.8);
                animation->setEndValue(1.0);
                animation->setEasingCurve(QEasingCurve::OutCubic);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
            }
            break;
        }
        default:
            break;
    }
}

// Performance optimization methods
void FluentDataTable::performIncrementalUpdate() {
    if (!m_config.incrementalUpdates || m_updatePending)
        return;

    m_updatePending = true;

    // Use timer to batch updates
    QTimer::singleShot(50, this, [this]() {
        // Only update dirty rows
        for (int row : m_dirtyRows) {
            if (row >= 0 && row < m_table->rowCount()) {
                updateTableRow(row);
            }
        }
        m_dirtyRows.clear();
        m_updatePending = false;
    });
}

void FluentDataTable::optimizeMemoryUsage() {
    // Clear unused cached items
    auto it = m_itemCache.begin();
    while (it != m_itemCache.end()) {
        if (!m_table->item(it.key() / m_table->columnCount(),
                           it.key() % m_table->columnCount())) {
            delete it.value();
            it = m_itemCache.erase(it);
        } else {
            ++it;
        }
    }
}

void FluentDataTable::debounceOperation(std::function<void()> operation,
                                        int delay) {
    if (!m_debounceTimer)
        return;

    // Store the operation
    m_debounceTimer->disconnect();
    connect(m_debounceTimer, &QTimer::timeout, this, operation);

    // Start or restart the timer
    m_debounceTimer->start(delay);
}

// Accessibility helper methods
void FluentDataTable::updateAriaAttributes() {
    if (!m_config.screenReaderSupport)
        return;

    // Update main widget ARIA attributes
    setAccessibleName(m_config.tableLabel);
    setAccessibleDescription(m_config.tableDescription);

    // Update table ARIA attributes
    if (m_table) {
        m_table->setProperty("aria-rowcount", m_filteredRows.size());
        m_table->setProperty("aria-colcount", m_columns.size());

        // Update column headers with ARIA attributes
        for (int col = 0; col < m_columns.size(); ++col) {
            const auto& column = m_columns[col];
            if (auto* headerItem = m_table->horizontalHeaderItem(col)) {
                headerItem->setData(Qt::AccessibleTextRole,
                                    column.accessibility.ariaLabel);
                headerItem->setData(Qt::AccessibleDescriptionRole,
                                    column.accessibility.ariaDescription);
            }
        }
    }
}

void FluentDataTable::manageFocus() {
    if (!m_config.focusManagement)
        return;

    // Ensure proper focus management
    if (m_currentState == FluentTableState::Ready && m_table) {
        if (!m_table->hasFocus() && isVisible()) {
            m_table->setFocus();
        }
    }
}

void FluentDataTable::handleKeyboardNavigation(QKeyEvent* event) {
    if (!m_config.keyboardNavigation)
        return;

    switch (event->key()) {
        case Qt::Key_Home:
            if (event->modifiers() & Qt::ControlModifier) {
                // Go to first page
                goToPage(1);
                event->accept();
            }
            break;

        case Qt::Key_End:
            if (event->modifiers() & Qt::ControlModifier) {
                // Go to last page
                int maxPage =
                    (m_filteredRows.size() + m_pagination.pageSize - 1) /
                    m_pagination.pageSize;
                goToPage(maxPage);
                event->accept();
            }
            break;

        case Qt::Key_PageUp:
            previousPage();
            event->accept();
            break;

        case Qt::Key_PageDown:
            nextPage();
            event->accept();
            break;

        case Qt::Key_F5:
            refresh();
            event->accept();
            break;

        case Qt::Key_Escape:
            clearSelection();
            event->accept();
            break;

        default:
            break;
    }
}

void FluentDataTable::announceStateChange(const QString& message) {
    if (m_config.screenReaderSupport && !message.isEmpty()) {
        announceToScreenReader(message);
    }
}

// Additional missing methods
void FluentDataTable::updateStateWidget() {
    if (!m_stackedWidget)
        return;

    int targetIndex = 0;  // Default to table view
    QString stateMessage = m_currentStateMessage;

    switch (m_currentState) {
        case FluentTableState::Loading:
            targetIndex = 1;
            if (m_stateLabel) {
                m_stateLabel->setText(stateMessage.isEmpty() ? "Loading data..."
                                                             : stateMessage);
            }
            if (m_progressBar) {
                m_progressBar->setVisible(true);
            }
            break;

        case FluentTableState::Error:
            targetIndex = 2;
            if (auto* errorLabel =
                    m_errorWidget->findChild<QLabel*>("errorLabel")) {
                errorLabel->setText(stateMessage.isEmpty()
                                        ? "Error loading data"
                                        : stateMessage);
                if (!m_currentErrorDetails.isEmpty()) {
                    errorLabel->setToolTip(m_currentErrorDetails);
                }
            }
            break;

        case FluentTableState::Empty:
            targetIndex = 3;
            if (auto* emptyLabel =
                    m_emptyWidget->findChild<QLabel*>("emptyLabel")) {
                emptyLabel->setText(stateMessage.isEmpty() ? "No data available"
                                                           : stateMessage);
            }
            break;

        case FluentTableState::Ready:
        default:
            targetIndex = 0;
            break;
    }

    m_stackedWidget->setCurrentIndex(targetIndex);

    // Update accessibility
    if (m_config.screenReaderSupport) {
        updateAriaAttributes();
    }
}

void FluentDataTable::updateTheme() {
    if (m_config.useFluentTheme && m_theme) {
        applyFluentTheme();
        emit themeChanged();
    }
}

void FluentDataTable::updateAccessibility() {
    if (m_config.screenReaderSupport) {
        updateAriaAttributes();
        manageFocus();
    }
}

void FluentDataTable::updateTableRow(int row) {
    if (!m_table || row < 0 || row >= m_filteredRows.size())
        return;

    const FluentTableRow& rowData = m_filteredRows[row];

    for (int col = 0; col < m_columns.size(); ++col) {
        const FluentTableColumn& column = m_columns[col];
        QVariant cellData = rowData.data.value(column.field);

        QString displayText;
        if (column.formatter) {
            displayText = column.formatter(cellData);
        } else {
            displayText = cellData.toString();
        }

        // Check if item exists in cache
        int itemKey = row * m_table->columnCount() + col;
        QTableWidgetItem* item = m_itemCache.value(itemKey, nullptr);

        if (!item) {
            item = new QTableWidgetItem(displayText);
            m_itemCache[itemKey] = item;
            m_table->setItem(row, col, item);
        } else {
            item->setText(displayText);
        }

        item->setTextAlignment(column.alignment);

        // Apply custom colors if set
        if (rowData.backgroundColor.isValid()) {
            item->setBackground(rowData.backgroundColor);
        }
        if (rowData.textColor.isValid()) {
            item->setForeground(rowData.textColor);
        }

        // Set accessibility properties
        if (m_config.screenReaderSupport) {
            if (!rowData.ariaLabel.isEmpty()) {
                item->setData(Qt::AccessibleTextRole, rowData.ariaLabel);
            }
            if (!rowData.ariaDescription.isEmpty()) {
                item->setData(Qt::AccessibleDescriptionRole,
                              rowData.ariaDescription);
            }
        }

        // Set item as read-only by default
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }
}

// Enhanced event handling
void FluentDataTable::keyPressEvent(QKeyEvent* event) {
    // Handle enhanced keyboard navigation first
    handleKeyboardNavigation(event);

    // If not handled by enhanced navigation, use original logic
    if (!event->isAccepted()) {
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
                    // Emit signal for row deletion
                    if (!m_selectedRows.isEmpty()) {
                        emit keyPressed(Qt::Key_Delete, event->modifiers());
                    }
                    event->accept();
                    return;
                default:
                    break;
            }
        }

        // Emit general key press signal
        emit keyPressed(event->key(), event->modifiers());

        QWidget::keyPressEvent(event);
    }
}

bool FluentDataTable::eventFilter(QObject* object, QEvent* event) {
    if (object == this && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        handleKeyboardNavigation(keyEvent);
        if (keyEvent->isAccepted()) {
            return true;
        }
    }

    return QWidget::eventFilter(object, event);
}

void FluentDataTable::contextMenuEvent(QContextMenuEvent* event) {
    emit contextMenuRequested(event->pos());
    QWidget::contextMenuEvent(event);
}

// Enhanced data management methods
QList<FluentTableRow> FluentDataTable::visibleData() const {
    if (!m_paginationEnabled) {
        return m_filteredRows;
    }

    QList<FluentTableRow> visibleRows;
    int startIndex = (m_pagination.currentPage - 1) * m_pagination.pageSize;
    int endIndex =
        qMin(startIndex + m_pagination.pageSize, m_filteredRows.size());

    for (int i = startIndex; i < endIndex; ++i) {
        visibleRows.append(m_filteredRows[i]);
    }

    return visibleRows;
}

void FluentDataTable::setDataAsync(const QList<FluentTableRow>& rows,
                                   FluentTableAnimation animation) {
    // Set loading state
    setState(FluentTableState::Loading);

    // Use timer to simulate async operation and allow UI to update
    QTimer::singleShot(100, this, [this, rows, animation]() {
        m_rows = rows;
        m_pagination.totalItems = rows.size();
        applyFilters();

        if (m_filteredRows.isEmpty() && !rows.isEmpty()) {
            setState(FluentTableState::Empty);
        } else {
            setState(FluentTableState::Ready);

            // Animate data update if requested
            if (animation != FluentTableAnimation::None &&
                !m_config.reduceMotion) {
                animateDataUpdate(animation);
            }
        }

        updateTable();
        updatePagination();
        emit dataChanged();
        emit dataLoaded();
    });
}

void FluentDataTable::addRowAsync(const FluentTableRow& row,
                                  FluentTableAnimation animation) {
    m_rows.append(row);
    m_pagination.totalItems = m_rows.size();
    applyFilters();

    if (animation != FluentTableAnimation::None && !m_config.reduceMotion) {
        animateRowInsertion(m_rows.size() - 1, animation);
    }

    updateTable();
    updatePagination();
    emit dataChanged();
}

void FluentDataTable::removeRowAsync(int index,
                                     FluentTableAnimation animation) {
    if (index >= 0 && index < m_rows.size()) {
        if (animation != FluentTableAnimation::None && !m_config.reduceMotion) {
            animateRowRemoval(index, animation);
        }

        m_rows.removeAt(index);
        m_pagination.totalItems = m_rows.size();
        applyFilters();
        updateTable();
        updatePagination();
        emit dataChanged();
    }
}

void FluentDataTable::updateRow(int index, const FluentTableRow& row) {
    if (index >= 0 && index < m_rows.size()) {
        m_rows[index] = row;

        // Mark row as dirty for incremental updates
        if (m_config.incrementalUpdates) {
            m_dirtyRows.insert(index);
            performIncrementalUpdate();
        } else {
            updateTable();
        }

        emit dataChanged();
    }
}

// Enhanced column management methods
void FluentDataTable::setColumnVisible(const QString& id, bool visible) {
    for (auto& column : m_columns) {
        if (column.id == id) {
            column.visible = visible;
            updateTable();
            emit columnVisibilityChanged(id, visible);
            break;
        }
    }
}

void FluentDataTable::setColumnWidth(const QString& id, int width) {
    for (auto& column : m_columns) {
        if (column.id == id) {
            column.width = width;
            updateTable();
            emit columnResized(id, width);
            break;
        }
    }
}

void FluentDataTable::setColumnPinned(const QString& id, bool pinned) {
    for (auto& column : m_columns) {
        if (column.id == id) {
            column.pinned = pinned;
            updateTable();
            break;
        }
    }
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
