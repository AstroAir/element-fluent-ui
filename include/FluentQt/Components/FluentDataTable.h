#pragma once

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLabel>
#include <QMenu>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>

namespace FluentQt::Components {

/**
 * @brief Column configuration for data table
 */
struct FluentTableColumn {
    QString id;                               // Unique identifier
    QString title;                            // Display title
    QString field;                            // Data field name
    int width = -1;                           // Column width (-1 for auto)
    bool sortable = true;                     // Whether column is sortable
    bool filterable = true;                   // Whether column is filterable
    bool resizable = true;                    // Whether column is resizable
    bool visible = true;                      // Whether column is visible
    Qt::Alignment alignment = Qt::AlignLeft;  // Text alignment
    QString format;                           // Format string for data
    std::function<QString(const QVariant&)> formatter;  // Custom formatter
    std::function<QWidget*(const QVariant&, QWidget*)>
        cellRenderer;  // Custom cell renderer
};

/**
 * @brief Row data for data table
 */
struct FluentTableRow {
    QVariantMap data;        // Row data
    bool selected = false;   // Selection state
    bool enabled = true;     // Whether row is enabled
    QVariantMap metadata;    // Additional metadata
    QColor backgroundColor;  // Custom background color
    QColor textColor;        // Custom text color
};

/**
 * @brief Pagination configuration
 */
struct FluentTablePagination {
    int currentPage = 1;                             // Current page (1-based)
    int pageSize = 25;                               // Items per page
    int totalItems = 0;                              // Total number of items
    QList<int> pageSizeOptions = {10, 25, 50, 100};  // Page size options
    bool showPageInfo = true;                        // Show page information
    bool showPageSizeSelector = true;                // Show page size selector
};

/**
 * @brief Modern data table component with advanced features
 */
class FluentDataTable : public QWidget {
    Q_OBJECT
    Q_PROPERTY(
        bool sortingEnabled READ isSortingEnabled WRITE setSortingEnabled)
    Q_PROPERTY(
        bool filteringEnabled READ isFilteringEnabled WRITE setFilteringEnabled)
    Q_PROPERTY(bool paginationEnabled READ isPaginationEnabled WRITE
                   setPaginationEnabled)
    Q_PROPERTY(
        bool selectionEnabled READ isSelectionEnabled WRITE setSelectionEnabled)

public:
    enum class SelectionMode {
        None,     // No selection
        Single,   // Single row selection
        Multiple  // Multiple row selection
    };

    explicit FluentDataTable(QWidget* parent = nullptr);
    ~FluentDataTable() override;

    // Column management
    void addColumn(const FluentTableColumn& column);
    void insertColumn(int index, const FluentTableColumn& column);
    void removeColumn(const QString& id);
    void removeColumn(int index);
    void setColumns(const QList<FluentTableColumn>& columns);
    QList<FluentTableColumn> columns() const { return m_columns; }

    // Data management
    void setData(const QList<FluentTableRow>& rows);
    void addRow(const FluentTableRow& row);
    void insertRow(int index, const FluentTableRow& row);
    void removeRow(int index);
    void clearData();
    QList<FluentTableRow> data() const { return m_rows; }

    // Selection
    SelectionMode selectionMode() const { return m_selectionMode; }
    void setSelectionMode(SelectionMode mode);

    QList<int> selectedRows() const;
    void selectRow(int index, bool selected = true);
    void selectAll();
    void clearSelection();

    // Sorting
    bool isSortingEnabled() const { return m_sortingEnabled; }
    void setSortingEnabled(bool enabled);

    void sortByColumn(const QString& columnId,
                      Qt::SortOrder order = Qt::AscendingOrder);
    void clearSorting();

    // Filtering
    bool isFilteringEnabled() const { return m_filteringEnabled; }
    void setFilteringEnabled(bool enabled);

    void setFilter(const QString& columnId, const QString& filter);
    void setGlobalFilter(const QString& filter);
    void clearFilters();

    // Pagination
    bool isPaginationEnabled() const { return m_paginationEnabled; }
    void setPaginationEnabled(bool enabled);

    FluentTablePagination pagination() const { return m_pagination; }
    void setPagination(const FluentTablePagination& pagination);

    // Appearance
    void setAlternatingRowColors(bool enable);
    void setGridVisible(bool visible);
    void setHeaderVisible(bool visible);

    // Export
    void exportToCsv(const QString& filename);
    void exportToJson(const QString& filename);

    // Accessibility
    bool isSelectionEnabled() const { return m_selectionEnabled; }
    void setSelectionEnabled(bool enabled);

public slots:
    void refresh();
    void goToPage(int page);
    void setPageSize(int size);
    void previousPage();
    void nextPage();

signals:
    void rowClicked(int row);
    void rowDoubleClicked(int row);
    void selectionChanged(const QList<int>& selectedRows);
    void sortingChanged(const QString& columnId, Qt::SortOrder order);
    void filterChanged(const QString& columnId, const QString& filter);
    void pageChanged(int page);
    void pageSizeChanged(int size);
    void dataChanged();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onCellClicked(int row, int column);
    void onCellDoubleClicked(int row, int column);
    void onHeaderClicked(int logicalIndex);
    void onSelectionChanged();
    void onPageSizeChanged();
    void onFilterTextChanged();

private:
    void setupUI();
    void setupTable();
    void setupToolbar();
    void setupPagination();
    void updateTable();
    void updatePagination();
    void updateToolbar();
    void applyFilters();
    void applySorting();
    void updateRowVisibility();
    void animateRowInsertion(int row);
    void animateRowRemoval(int row);

    // UI components
    QVBoxLayout* m_mainLayout = nullptr;
    QToolBar* m_toolbar = nullptr;
    QTableWidget* m_table = nullptr;
    QWidget* m_paginationWidget = nullptr;
    QHBoxLayout* m_paginationLayout = nullptr;

    // Pagination controls
    QPushButton* m_firstPageButton = nullptr;
    QPushButton* m_previousPageButton = nullptr;
    QPushButton* m_nextPageButton = nullptr;
    QPushButton* m_lastPageButton = nullptr;
    QLabel* m_pageInfoLabel = nullptr;
    QComboBox* m_pageSizeCombo = nullptr;

    // Data
    QList<FluentTableColumn> m_columns;
    QList<FluentTableRow> m_rows;
    QList<FluentTableRow> m_filteredRows;

    // Configuration
    SelectionMode m_selectionMode = SelectionMode::Single;
    bool m_sortingEnabled = true;
    bool m_filteringEnabled = true;
    bool m_paginationEnabled = true;
    bool m_selectionEnabled = true;
    FluentTablePagination m_pagination;

    // State
    QString m_currentSortColumn;
    Qt::SortOrder m_currentSortOrder = Qt::AscendingOrder;
    QHash<QString, QString> m_columnFilters;
    QString m_globalFilter;
    QList<int> m_selectedRows;
};

/**
 * @brief Custom item delegate for enhanced cell rendering
 */
class FluentTableItemDelegate : public QItemDelegate {
    Q_OBJECT

public:
    explicit FluentTableItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;

private:
    void paintBackground(QPainter* painter, const QStyleOptionViewItem& option,
                         const QModelIndex& index) const;
    void paintText(QPainter* painter, const QStyleOptionViewItem& option,
                   const QModelIndex& index) const;
    void paintSelection(QPainter* painter,
                        const QStyleOptionViewItem& option) const;
};

/**
 * @brief Virtual table for large datasets
 */
class FluentVirtualTable : public FluentDataTable {
    Q_OBJECT

public:
    explicit FluentVirtualTable(QWidget* parent = nullptr);

    // Virtual data source
    void setDataSource(std::function<FluentTableRow(int)> dataSource);
    void setRowCount(int count);

    // Performance
    void setVisibleRowCount(int count);
    void setBufferSize(int size);

signals:
    void dataRequested(int startRow, int count);

private slots:
    void onScrollValueChanged(int value);
    void updateVisibleRows();

private:
    void setupVirtualScrolling();
    void loadVisibleData();

    std::function<FluentTableRow(int)> m_dataSource;
    int m_totalRowCount = 0;
    int m_visibleRowCount = 50;
    int m_bufferSize = 10;
    int m_firstVisibleRow = 0;
    QHash<int, FluentTableRow> m_cachedRows;
};

}  // namespace FluentQt::Components
