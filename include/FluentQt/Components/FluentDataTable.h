#pragma once

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLabel>
#include <QMenu>
#include <QParallelAnimationGroup>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSequentialAnimationGroup>
#include <QSortFilterProxyModel>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>
#include <memory>

namespace FluentQt::Components {

// Forward declarations
class FluentTheme;

/**
 * @brief Table state enumeration for different UI states
 */
enum class FluentTableState {
    Loading,  // Data is being loaded
    Ready,    // Data is ready and displayed
    Empty,    // No data to display
    Error     // Error occurred while loading data
};

/**
 * @brief Animation types for table operations
 */
enum class FluentTableAnimation {
    None,         // No animation
    Fade,         // Fade in/out
    Slide,        // Slide in/out
    Scale,        // Scale in/out
    FluentMotion  // Fluent Design motion
};

/**
 * @brief Accessibility configuration for table columns
 */
struct FluentTableAccessibility {
    QString ariaLabel;              // ARIA label for screen readers
    QString ariaDescription;        // ARIA description
    QString role = "columnheader";  // ARIA role
    bool focusable = true;          // Whether column header is focusable
    int tabIndex = 0;               // Tab order index
    QString accessKey;              // Keyboard access key
};

/**
 * @brief Enhanced column configuration with Fluent UI compliance
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

    // Enhanced Fluent UI properties
    FluentTableAccessibility accessibility;  // Accessibility configuration
    QString iconName;      // Fluent icon name for column header
    bool pinned = false;   // Whether column is pinned
    QString tooltip;       // Column tooltip text
    QVariantMap metadata;  // Additional column metadata
};

/**
 * @brief Enhanced row data with state management and accessibility
 */
struct FluentTableRow {
    QVariantMap data;        // Row data
    bool selected = false;   // Selection state
    bool enabled = true;     // Whether row is enabled
    QVariantMap metadata;    // Additional metadata
    QColor backgroundColor;  // Custom background color
    QColor textColor;        // Custom text color

    // Enhanced Fluent UI properties
    FluentTableState state = FluentTableState::Ready;  // Row state
    QString ariaLabel;                                 // ARIA label for row
    QString ariaDescription;                           // ARIA description
    bool focusable = true;          // Whether row is focusable
    QString tooltip;                // Row tooltip
    QVariantMap accessibilityData;  // Additional accessibility data
    bool isNew = false;             // Whether row is newly added
    bool isModified = false;        // Whether row is modified
    QDateTime lastModified;         // Last modification time
};

/**
 * @brief Enhanced pagination with accessibility and theming
 */
struct FluentTablePagination {
    int currentPage = 1;                             // Current page (1-based)
    int pageSize = 25;                               // Items per page
    int totalItems = 0;                              // Total number of items
    QList<int> pageSizeOptions = {10, 25, 50, 100};  // Page size options
    bool showPageInfo = true;                        // Show page information
    bool showPageSizeSelector = true;                // Show page size selector

    // Enhanced Fluent UI properties
    bool showFirstLastButtons = true;        // Show first/last page buttons
    bool showPageNumbers = false;            // Show page number buttons
    int maxVisiblePages = 5;                 // Max visible page numbers
    QString ariaLabel = "Table pagination";  // ARIA label for pagination
    bool compactMode = false;                // Compact pagination layout
};

/**
 * @brief Table configuration for theming and behavior
 */
struct FluentTableConfig {
    // Theme integration
    bool useFluentTheme = true;      // Use Fluent theme colors/fonts
    bool adaptToSystemTheme = true;  // Adapt to system dark/light mode
    bool highContrastMode = false;   // High contrast accessibility mode

    // Animation settings
    FluentTableAnimation defaultAnimation = FluentTableAnimation::FluentMotion;
    int animationDuration = 200;  // Animation duration in ms
    bool reduceMotion = false;    // Reduce motion for accessibility

    // Performance settings
    bool virtualScrolling = false;   // Enable virtual scrolling
    int virtualBufferSize = 50;      // Virtual scroll buffer size
    bool incrementalUpdates = true;  // Use incremental table updates
    bool debounceFiltering = true;   // Debounce filter operations
    int debounceDelay = 300;         // Debounce delay in ms

    // Accessibility settings
    bool screenReaderSupport = true;  // Enable screen reader support
    bool keyboardNavigation = true;   // Enable keyboard navigation
    bool focusManagement = true;      // Enable focus management
    QString tableRole = "table";      // ARIA role for table
    QString tableLabel;               // ARIA label for table
    QString tableDescription;         // ARIA description for table
};

/**
 * @brief Modern data table component with enhanced Fluent UI compliance
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
    Q_PROPERTY(
        FluentTableState state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(bool useFluentTheme READ useFluentTheme WRITE setUseFluentTheme)
    Q_PROPERTY(
        bool highContrastMode READ highContrastMode WRITE setHighContrastMode)
    Q_PROPERTY(bool reduceMotion READ reduceMotion WRITE setReduceMotion)
    Q_PROPERTY(QString ariaLabel READ ariaLabel WRITE setAriaLabel)
    Q_PROPERTY(
        QString ariaDescription READ ariaDescription WRITE setAriaDescription)

public:
    enum class SelectionMode {
        None,     // No selection
        Single,   // Single row selection
        Multiple  // Multiple row selection
    };

    explicit FluentDataTable(QWidget* parent = nullptr);
    ~FluentDataTable() override;

    // Configuration
    void setConfig(const FluentTableConfig& config);
    FluentTableConfig config() const { return m_config; }

    // State management
    FluentTableState state() const { return m_currentState; }
    void setState(FluentTableState state);
    void setLoading(bool loading, const QString& message = QString());
    void setError(const QString& errorMessage,
                  const QString& details = QString());
    void setEmpty(const QString& emptyMessage = QString());

    // Theme integration
    bool useFluentTheme() const { return m_config.useFluentTheme; }
    void setUseFluentTheme(bool use);
    bool highContrastMode() const { return m_config.highContrastMode; }
    void setHighContrastMode(bool enabled);
    bool reduceMotion() const { return m_config.reduceMotion; }
    void setReduceMotion(bool reduce);

    // Accessibility
    QString ariaLabel() const { return m_config.tableLabel; }
    void setAriaLabel(const QString& label);
    QString ariaDescription() const { return m_config.tableDescription; }
    void setAriaDescription(const QString& description);
    void setAccessibilityEnabled(bool enabled);
    void announceToScreenReader(const QString& message);

    // Column management
    void addColumn(const FluentTableColumn& column);
    void insertColumn(int index, const FluentTableColumn& column);
    void removeColumn(const QString& id);
    void removeColumn(int index);
    void setColumns(const QList<FluentTableColumn>& columns);
    QList<FluentTableColumn> columns() const { return m_columns; }
    void setColumnVisible(const QString& id, bool visible);
    void setColumnWidth(const QString& id, int width);
    void setColumnPinned(const QString& id, bool pinned);

    // Data management with enhanced features
    void setData(const QList<FluentTableRow>& rows);
    void setDataAsync(
        const QList<FluentTableRow>& rows,
        FluentTableAnimation animation = FluentTableAnimation::FluentMotion);
    void addRow(const FluentTableRow& row);
    void addRowAsync(
        const FluentTableRow& row,
        FluentTableAnimation animation = FluentTableAnimation::FluentMotion);
    void insertRow(int index, const FluentTableRow& row);
    void removeRow(int index);
    void removeRowAsync(int index, FluentTableAnimation animation =
                                       FluentTableAnimation::FluentMotion);
    void updateRow(int index, const FluentTableRow& row);
    void clearData();
    QList<FluentTableRow> data() const { return m_rows; }
    QList<FluentTableRow> filteredData() const { return m_filteredRows; }
    QList<FluentTableRow> visibleData() const;

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
    // Enhanced data signals
    void rowClicked(int row);
    void rowDoubleClicked(int row);
    void rowRightClicked(int row, const QPoint& globalPos);
    void cellClicked(int row, int column);
    void cellDoubleClicked(int row, int column);
    void cellRightClicked(int row, int column, const QPoint& globalPos);
    void selectionChanged(const QList<int>& selectedRows);
    void dataChanged();
    void dataLoaded();
    void dataLoadFailed(const QString& error);

    // Enhanced interaction signals
    void sortingChanged(const QString& columnId, Qt::SortOrder order);
    void filterChanged(const QString& columnId, const QString& filter);
    void globalFilterChanged(const QString& filter);
    void pageChanged(int page);
    void pageSizeChanged(int size);
    void columnResized(const QString& columnId, int width);
    void columnMoved(const QString& columnId, int fromIndex, int toIndex);
    void columnVisibilityChanged(const QString& columnId, bool visible);

    // State and theme signals
    void stateChanged(FluentTableState state);
    void themeChanged();
    void accessibilityChanged();
    void configChanged();

    // Animation signals
    void animationStarted(FluentTableAnimation animation);
    void animationFinished(FluentTableAnimation animation);

    // Keyboard and focus signals
    void focusChanged(int row, int column);
    void keyPressed(int key, Qt::KeyboardModifiers modifiers);
    void contextMenuRequested(const QPoint& position);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    // Protected members for inheritance
    QTableWidget* m_table = nullptr;
    QList<FluentTableRow> m_filteredRows;

    // Protected methods for inheritance
    void updateTable();

private slots:
    void onCellClicked(int row, int column);
    void onCellDoubleClicked(int row, int column);
    void onHeaderClicked(int logicalIndex);
    void onSelectionChanged();
    void onPageSizeChanged();
    void onFilterTextChanged();

private:
    // Enhanced setup methods
    void setupUI();
    void setupTable();
    void setupToolbar();
    void setupPagination();
    void setupStateWidgets();
    void setupAccessibility();
    void setupTheme();
    void setupAnimations();
    void setupKeyboardNavigation();

    // Enhanced update methods
    void updatePagination();
    void updateToolbar();
    void updateTheme();
    void updateAccessibility();
    void updateStateWidget();
    void applyFilters();
    void applySorting();
    void updateRowVisibility();

    // Enhanced animation methods
    void animateRowInsertion(int row, FluentTableAnimation animation =
                                          FluentTableAnimation::FluentMotion);
    void animateRowRemoval(int row, FluentTableAnimation animation =
                                        FluentTableAnimation::FluentMotion);
    void animateStateChange(FluentTableState fromState,
                            FluentTableState toState);
    void animateDataUpdate(FluentTableAnimation animation);

    // Performance optimization methods
    void performIncrementalUpdate();
    void optimizeMemoryUsage();
    void debounceOperation(std::function<void()> operation, int delay);

    // Accessibility helper methods
    void updateAriaAttributes();
    void manageFocus();
    void handleKeyboardNavigation(QKeyEvent* event);
    void announceStateChange(const QString& message);

    // Theme integration methods
    void applyFluentTheme();
    void updateColors();
    void updateFonts();
    void updateSpacing();
    void updateBorderRadius();
    void updateElevation();

    // UI components
    QVBoxLayout* m_mainLayout = nullptr;
    QStackedWidget* m_stackedWidget = nullptr;  // For different states
    QToolBar* m_toolbar = nullptr;
    QWidget* m_paginationWidget = nullptr;
    QHBoxLayout* m_paginationLayout = nullptr;

    // State widgets
    QWidget* m_loadingWidget = nullptr;
    QWidget* m_errorWidget = nullptr;
    QWidget* m_emptyWidget = nullptr;
    QProgressBar* m_progressBar = nullptr;
    QLabel* m_stateLabel = nullptr;
    QPushButton* m_retryButton = nullptr;

    // Pagination controls
    QPushButton* m_firstPageButton = nullptr;
    QPushButton* m_previousPageButton = nullptr;
    QPushButton* m_nextPageButton = nullptr;
    QPushButton* m_lastPageButton = nullptr;
    QLabel* m_pageInfoLabel = nullptr;
    QComboBox* m_pageSizeCombo = nullptr;

    // Animation components
    QPropertyAnimation* m_fadeAnimation = nullptr;
    QParallelAnimationGroup* m_animationGroup = nullptr;
    QSequentialAnimationGroup* m_sequentialAnimationGroup = nullptr;
    QGraphicsOpacityEffect* m_opacityEffect = nullptr;

    // Theme integration
    std::shared_ptr<FluentTheme> m_theme;
    QTimer* m_themeUpdateTimer = nullptr;
    QTimer* m_debounceTimer = nullptr;

    // Enhanced configuration
    FluentTableConfig m_config;
    FluentTableState m_currentState = FluentTableState::Ready;
    QString m_currentStateMessage;
    QString m_currentErrorDetails;

    // Data
    QList<FluentTableColumn> m_columns;
    QList<FluentTableRow> m_rows;

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

    // Performance tracking
    QHash<int, QTableWidgetItem*> m_itemCache;
    QSet<int> m_dirtyRows;
    bool m_updatePending = false;
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
