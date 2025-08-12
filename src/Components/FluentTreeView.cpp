// src/Components/FluentTreeView.cpp
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QDateTime>
#include <QElapsedTimer>
#include <QHash>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <QSet>
#include <QTimer>
#include <QTreeWidgetItemIterator>
#include <algorithm>
#include <chrono>

namespace FluentQt::Components {

// FluentTreeItem Implementation
FluentTreeItem::FluentTreeItem(QTreeWidget* parent) : QTreeWidgetItem(parent) {
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

FluentTreeItem::FluentTreeItem(QTreeWidgetItem* parent)
    : QTreeWidgetItem(parent) {
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

void FluentTreeItem::setItemData(const QString& key, const QVariant& value) {
    m_userData[key] = value;
}

QVariant FluentTreeItem::itemData(const QString& key) const {
    return m_userData.value(key);
}

void FluentTreeItem::setExpandable(bool expandable) {
    m_expandable = expandable;
    if (!expandable && childCount() == 0) {
        setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
    } else {
        setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
    }
}

bool FluentTreeItem::isExpandable() const { return m_expandable; }

void FluentTreeItem::setCheckable(bool checkable) {
    m_checkable = checkable;
    if (checkable) {
        setFlags(flags() | Qt::ItemIsUserCheckable);
        setCheckState(0, Qt::Unchecked);
    } else {
        setFlags(flags() & ~Qt::ItemIsUserCheckable);
    }
}

bool FluentTreeItem::isCheckable() const { return m_checkable; }

void FluentTreeItem::setIcon(const QIcon& icon) {
    QTreeWidgetItem::setIcon(0, icon);
}

void FluentTreeItem::setToolTip(const QString& tooltip) {
    QTreeWidgetItem::setToolTip(0, tooltip);
}

void FluentTreeItem::setLazyLoading(bool lazy) {
    m_lazyLoading = lazy;
    if (lazy && childCount() == 0) {
        // Add a dummy child to show expand indicator
        auto* dummy = new FluentTreeItem(this);
        dummy->setText(0, "Loading...");
        dummy->setFlags(Qt::NoItemFlags);
    }
}

bool FluentTreeItem::isLazyLoading() const { return m_lazyLoading; }

// FluentTreeView Implementation
FluentTreeView::FluentTreeView(QWidget* parent)
    : Core::FluentComponent(parent) {
    setupTreeWidget();
    setupFilterBar();
    updateTreeStyling();

    // Setup filter debounce timer
    m_filterDebounceTimer.setSingleShot(true);
    connect(&m_filterDebounceTimer, &QTimer::timeout, this,
            [this]() { this->filterItems(m_currentFilter); });

    // Setup advanced virtualization timer
    m_virtualizationUpdateTimer.setSingleShot(true);
    m_virtualizationUpdateTimer.setInterval(16);  // ~60fps
    connect(&m_virtualizationUpdateTimer, &QTimer::timeout, this, [this]() {
        updateVirtualizationWindow();
        updateVirtualizationCache();
        optimizeVirtualizationPerformance();
    });

    // Hook scroll events for virtualization
    connect(m_treeWidget->verticalScrollBar(), &QScrollBar::valueChanged, this,
            [this](int) {
                if (m_virtualizationEnabled) {
                    m_virtualizationUpdateTimer.start();
                }
            });

    connect(m_treeWidget->horizontalScrollBar(), &QScrollBar::valueChanged,
            this, [this](int) {
                if (m_virtualizationEnabled && m_columnVirtualizationEnabled) {
                    m_virtualizationUpdateTimer.start();
                }
            });

    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentTreeView::updateTreeStyling);
}

void FluentTreeView::setSelectionMode(FluentTreeSelectionMode mode) {
    if (m_selectionMode != mode) {
        m_selectionMode = mode;

        QAbstractItemView::SelectionMode qtMode =
            QAbstractItemView::SingleSelection;
        switch (mode) {
            case FluentTreeSelectionMode::NoSelection:
                qtMode = QAbstractItemView::NoSelection;
                break;
            case FluentTreeSelectionMode::SingleSelection:
                qtMode = QAbstractItemView::SingleSelection;
                break;
            case FluentTreeSelectionMode::MultiSelection:
                qtMode = QAbstractItemView::MultiSelection;
                break;
            case FluentTreeSelectionMode::ExtendedSelection:
                qtMode = QAbstractItemView::ExtendedSelection;
                break;
        }

        m_treeWidget->setSelectionMode(qtMode);
    }
}

void FluentTreeView::setExpandMode(FluentTreeExpandMode mode) {
    if (m_expandMode != mode) {
        m_expandMode = mode;

        switch (mode) {
            case FluentTreeExpandMode::Manual:
                m_treeWidget->setExpandsOnDoubleClick(false);
                // Disconnect any click handlers for expansion
                break;
            case FluentTreeExpandMode::SingleClick:
                m_treeWidget->setExpandsOnDoubleClick(false);
                // Connect single click to expand/collapse
                break;
            case FluentTreeExpandMode::DoubleClick:
                m_treeWidget->setExpandsOnDoubleClick(true);
                break;
            case FluentTreeExpandMode::AutoExpand:
                m_treeWidget->setExpandsOnDoubleClick(false);
                // Auto-expand items when they are added
                m_treeWidget->expandAll();
                break;
        }
    }
}

bool FluentTreeView::showHeader() const {
    return !m_treeWidget->header()->isHidden();
}

void FluentTreeView::setShowHeader(bool show) {
    m_treeWidget->header()->setVisible(show);
}

bool FluentTreeView::showRootDecoration() const {
    return m_treeWidget->rootIsDecorated();
}

void FluentTreeView::setShowRootDecoration(bool show) {
    m_treeWidget->setRootIsDecorated(show);
}

bool FluentTreeView::alternatingRowColors() const {
    return m_treeWidget->alternatingRowColors();
}

void FluentTreeView::setAlternatingRowColors(bool enabled) {
    m_treeWidget->setAlternatingRowColors(enabled);
}

bool FluentTreeView::sortingEnabled() const {
    return m_treeWidget->isSortingEnabled();
}

void FluentTreeView::setSortingEnabled(bool enabled) {
    m_treeWidget->setSortingEnabled(enabled);
}

bool FluentTreeView::filteringEnabled() const { return m_filteringEnabled; }

void FluentTreeView::setFilteringEnabled(bool enabled) {
    if (m_filteringEnabled != enabled) {
        m_filteringEnabled = enabled;
        m_filterEdit->setVisible(enabled);
        if (!enabled) {
            clearFilter();
        }
    }
}

bool FluentTreeView::dragDropEnabled() const {
    return m_treeWidget->dragDropMode() != QAbstractItemView::NoDragDrop;
}

void FluentTreeView::setDragDropEnabled(bool enabled) {
    if (enabled) {
        m_treeWidget->setDragDropMode(QAbstractItemView::InternalMove);
        m_treeWidget->setDefaultDropAction(Qt::MoveAction);
    } else {
        m_treeWidget->setDragDropMode(QAbstractItemView::NoDragDrop);
    }
}

void FluentTreeView::setColumns(const std::vector<FluentTreeColumn>& columns) {
    m_columns = columns;

    m_treeWidget->setColumnCount(static_cast<int>(columns.size()));

    QStringList headers;
    for (const auto& column : columns) {
        headers << column.header;
    }
    m_treeWidget->setHeaderLabels(headers);

    // Configure column properties
    auto* header = m_treeWidget->header();
    for (int i = 0; i < static_cast<int>(columns.size()); ++i) {
        const auto& column = columns[i];

        if (column.width > 0) {
            header->resizeSection(i, column.width);
        }

        header->setSectionResizeMode(i, column.resizable
                                            ? QHeaderView::Interactive
                                            : QHeaderView::Fixed);
    }
}

void FluentTreeView::addColumn(const FluentTreeColumn& column) {
    m_columns.push_back(column);
    setColumns(m_columns);
}

void FluentTreeView::removeColumn(int index) {
    if (index >= 0 && index < static_cast<int>(m_columns.size())) {
        m_columns.erase(m_columns.begin() + index);
        setColumns(m_columns);
    }
}

int FluentTreeView::columnCount() const {
    return static_cast<int>(m_columns.size());
}

FluentTreeItem* FluentTreeView::addTopLevelItem(const QString& text) {
    auto* item = new FluentTreeItem(m_treeWidget);
    item->setText(0, text);
    m_treeWidget->addTopLevelItem(item);
    return item;
}

FluentTreeItem* FluentTreeView::addChildItem(FluentTreeItem* parent,
                                             const QString& text) {
    if (!parent)
        return nullptr;

    auto* item = new FluentTreeItem(parent);
    item->setText(0, text);
    parent->addChild(item);
    return item;
}

void FluentTreeView::removeItem(FluentTreeItem* item) {
    if (!item)
        return;

    auto* parent = item->parent();
    if (parent) {
        parent->removeChild(item);
    } else {
        int index = m_treeWidget->indexOfTopLevelItem(item);
        if (index >= 0) {
            m_treeWidget->takeTopLevelItem(index);
        }
    }
    delete item;
}

void FluentTreeView::clear() { m_treeWidget->clear(); }

QList<FluentTreeItem*> FluentTreeView::selectedItems() const {
    QList<FluentTreeItem*> items;
    for (auto* qtItem : m_treeWidget->selectedItems()) {
        if (auto* fluentItem = dynamic_cast<FluentTreeItem*>(qtItem)) {
            items.append(fluentItem);
        }
    }
    return items;
}

FluentTreeItem* FluentTreeView::currentItem() const {
    return dynamic_cast<FluentTreeItem*>(m_treeWidget->currentItem());
}

void FluentTreeView::setCurrentItem(FluentTreeItem* item) {
    m_treeWidget->setCurrentItem(item);
}

void FluentTreeView::expandAll() { m_treeWidget->expandAll(); }

void FluentTreeView::collapseAll() { m_treeWidget->collapseAll(); }

void FluentTreeView::expandItem(FluentTreeItem* item) {
    if (item) {
        m_treeWidget->expandItem(item);
    }
}

void FluentTreeView::collapseItem(FluentTreeItem* item) {
    if (item) {
        m_treeWidget->collapseItem(item);
    }
}

void FluentTreeView::setFilter(const QString& filter) {
    m_currentFilter = filter;
    m_filterEdit->setText(filter);
    filterItems(filter);
}

void FluentTreeView::clearFilter() {
    m_currentFilter.clear();
    m_filterEdit->clear();

    // Show all items
    QTreeWidgetItemIterator it(m_treeWidget);
    while (*it) {
        (*it)->setHidden(false);
        ++it;
    }
}

void FluentTreeView::setLazyLoadingEnabled(bool enabled) {
    m_lazyLoadingEnabled = enabled;
}

bool FluentTreeView::isLazyLoadingEnabled() const {
    return m_lazyLoadingEnabled;
}

void FluentTreeView::paintEvent(QPaintEvent* event) {
    FLUENT_PROFILE("FluentTreeView::paintEvent");

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Paint background
    painter.fillRect(rect(), palette.neutralLightest);

    // Paint border
    if (hasFocus()) {
        QPen pen(palette.accent, 2);
        painter.setPen(pen);
        painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), cornerRadius(),
                                cornerRadius());
    }

    Core::FluentComponent::paintEvent(event);
}

void FluentTreeView::resizeEvent(QResizeEvent* event) {
    FLUENT_PROFILE("FluentTreeView::resizeEvent");
    Core::FluentComponent::resizeEvent(event);

    // Update column widths for auto-sized columns
    if (!m_columns.empty()) {
        auto* header = m_treeWidget->header();

        int totalWidth = width();
        int fixedWidth = 0;
        int autoColumns = 0;

        for (int i = 0; i < static_cast<int>(m_columns.size()); ++i) {
            if (m_columns[i].width > 0) {
                fixedWidth += m_columns[i].width;
            } else {
                autoColumns++;
            }
        }

        if (autoColumns > 0) {
            int autoWidth = autoColumns > 0
                                ? (totalWidth - fixedWidth) / autoColumns
                                : totalWidth;
            for (int i = 0; i < static_cast<int>(m_columns.size()); ++i) {
                if (m_columns[i].width <= 0) {
                    header->resizeSection(i, autoWidth);
                }
            }
        }
    }
}

void FluentTreeView::updateVirtualizationWindow() {
    if (!m_virtualizationEnabled || !m_treeWidget)
        return;

    FLUENT_PROFILE("FluentTreeView::updateVirtualizationWindow");
    QElapsedTimer timer;
    timer.start();

    const auto viewport = m_treeWidget->viewport()->rect();
    const int rowHeight = m_treeWidget->uniformRowHeights()
                              ? m_treeWidget->sizeHintForRow(0)
                              : 24;
    const int scrollY = m_treeWidget->verticalScrollBar()->value();
    const int scrollX = m_treeWidget->horizontalScrollBar()->value();

    // Calculate visible window with overscan
    const int firstVisibleRow = std::max(
        0, (scrollY / std::max(1, rowHeight)) - m_virtualizationOverscan);
    const int visibleRowCount =
        (viewport.height() / std::max(1, rowHeight)) + 1;
    const int lastVisibleRow =
        firstVisibleRow + visibleRowCount + (2 * m_virtualizationOverscan);

    // Update virtualization window
    m_virtualizationWindow.firstVisibleRow = firstVisibleRow;
    m_virtualizationWindow.lastVisibleRow = lastVisibleRow;
    m_virtualizationWindow.viewportRect = viewport;

    // Use scrollX for column virtualization if enabled
    if (m_columnVirtualizationEnabled) {
        Q_UNUSED(scrollX);  // scrollX is used in updateColumnVirtualization()
    }
    m_virtualizationWindow.viewportRect = viewport;

    // Column virtualization
    if (m_columnVirtualizationEnabled) {
        updateColumnVirtualization();
    }

    // Update visible items efficiently
    renderVirtualizedItems();

    // Update performance metrics
    m_virtualizationMetrics.renderTime = timer.elapsed();
    m_virtualizationMetrics.visibleItems = m_visibleItems.size();
}

void FluentTreeView::onItemClicked(QTreeWidgetItem* item, int column) {
    if (auto* fluentItem = dynamic_cast<FluentTreeItem*>(item)) {
        emit itemClicked(fluentItem, column);
    }
}

void FluentTreeView::onItemExpanded(QTreeWidgetItem* item) {
    if (auto* fluentItem = dynamic_cast<FluentTreeItem*>(item)) {
        if (fluentItem->isLazyLoading() && fluentItem->childCount() == 1) {
            // Remove dummy child and request lazy loading
            auto* dummy = fluentItem->child(0);
            if (dummy && dummy->text(0) == "Loading...") {
                fluentItem->removeChild(dummy);
                delete dummy;
                emit lazyLoadRequested(fluentItem);
            }
        }
        emit itemExpanded(fluentItem);
    }
}

void FluentTreeView::onFilterTextChanged(const QString& text) {
    FLUENT_PROFILE("FluentTreeView::onFilterTextChanged");
    m_currentFilter = text;
    // Debounce to avoid repeated filtering on every keystroke
    m_filterDebounceTimer.start(120);
}

void FluentTreeView::setupTreeWidget() {
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(4);

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setFrameShape(QFrame::NoFrame);
    m_treeWidget->setAlternatingRowColors(false);
    m_treeWidget->setUniformRowHeights(true);
    m_treeWidget->setAnimated(true);
    m_treeWidget->setExpandsOnDoubleClick(true);
    m_treeWidget->setHeaderHidden(false);

    connect(m_treeWidget, &QTreeWidget::itemClicked, this,
            &FluentTreeView::onItemClicked);
    connect(m_treeWidget, &QTreeWidget::itemDoubleClicked,
            [this](QTreeWidgetItem* item, int column) {
                if (auto* fluentItem = dynamic_cast<FluentTreeItem*>(item)) {
                    emit itemDoubleClicked(fluentItem, column);
                }
            });
    connect(m_treeWidget, &QTreeWidget::itemExpanded, this,
            &FluentTreeView::onItemExpanded);
    connect(m_treeWidget, &QTreeWidget::itemCollapsed,
            [this](QTreeWidgetItem* item) {
                if (auto* fluentItem = dynamic_cast<FluentTreeItem*>(item)) {
                    emit itemCollapsed(fluentItem);
                }
            });
    connect(m_treeWidget, &QTreeWidget::itemSelectionChanged, this,
            &FluentTreeView::itemSelectionChanged);
    connect(m_treeWidget, &QTreeWidget::itemChanged,
            [this](QTreeWidgetItem* item, int column) {
                if (auto* fluentItem = dynamic_cast<FluentTreeItem*>(item)) {
                    emit itemChanged(fluentItem, column);
                }
            });
}

void FluentTreeView::setupFilterBar() {
    m_filterEdit = new QLineEdit(this);
    m_filterEdit->setPlaceholderText("Filter items...");
    m_filterEdit->setClearButtonEnabled(true);

    connect(m_filterEdit, &QLineEdit::textChanged, this,
            &FluentTreeView::onFilterTextChanged);

    m_layout->addWidget(m_filterEdit);
    m_layout->addWidget(m_treeWidget);
}

void FluentTreeView::updateTreeStyling() {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Build a cache key from palette colors and corner radius
    const QString key = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9")
                            .arg(palette.neutralLightest.name())
                            .arg(palette.neutralQuaternary.name())
                            .arg(QString::number(cornerRadius()))
                            .arg(palette.accent.name())
                            .arg(palette.neutralLightest.name())
                            .arg(palette.neutralLight.name())
                            .arg(palette.neutralLight.name())
                            .arg(palette.accentDark1.name())
                            .arg(palette.neutralLighter.name());

    if (key == m_cachedStyleKey && !m_cachedTreeStyle.isEmpty() &&
        !m_cachedFilterStyle.isEmpty()) {
        m_treeWidget->setStyleSheet(m_cachedTreeStyle);
        m_filterEdit->setStyleSheet(m_cachedFilterStyle);
        return;
    }

    // Recompute and cache styles
    QString styleSheet =
        QString(R"(
        QTreeWidget {
            background-color: %1;
            border: 1px solid %2;
            border-radius: %3px;
            outline: none;
            selection-background-color: %4;
            selection-color: %5;
        }
        QTreeWidget::item {
            padding: 8px;
            border-bottom: 1px solid %6;
        }
        QTreeWidget::item:hover {
            background-color: %7;
        }
        QTreeWidget::item:selected {
            background-color: %4;
            color: %5;
        }
        QTreeWidget::item:selected:active {
            background-color: %8;
        }
        QTreeWidget::branch {
            background: transparent;
        }
        QTreeWidget::branch:has-siblings:!adjoins-item {
            border-image: none;
        }
        QTreeWidget::branch:has-siblings:adjoins-item {
            border-image: none;
        }
        QTreeWidget::branch:!has-children:!has-siblings:adjoins-item {
            border-image: none;
        }
        QTreeWidget::branch:has-children:!has-siblings:closed,
        QTreeWidget::branch:closed:has-children:has-siblings {
            image: url(:/icons/chevron-right.png);
        }
        QTreeWidget::branch:open:has-children:!has-siblings,
        QTreeWidget::branch:open:has-children:has-siblings {
            image: url(:/icons/chevron-down.png);
        }
        QHeaderView::section {
            background-color: %9;
            padding: 8px;
            border: none;
            border-bottom: 2px solid %2;
            font-weight: 600;
        }
    )")
            .arg(palette.neutralLightest.name(),    // background
                 palette.neutralQuaternary.name(),  // border
                 QString::number(cornerRadius()),   // border radius
                 palette.accent.name(),             // selection background
                 palette.neutralLightest.name(),    // selection text
                 palette.neutralLight.name(),       // item border
                 palette.neutralLight.name(),       // hover background
                 palette.accentDark1.name(),        // active selection
                 palette.neutralLighter.name()      // header background
            );

    QString filterStyle =
        QString(R"(
        QLineEdit {
            background-color: %1;
            border: 2px solid %2;
            border-radius: %3px;
            padding: 8px 12px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: %4;
        }
    )")
            .arg(palette.neutralLightest.name(),
                 palette.neutralQuaternary.name(),
                 QString::number(cornerRadius()), palette.accent.name());

    m_treeWidget->setStyleSheet(styleSheet);
    m_filterEdit->setStyleSheet(filterStyle);

    m_cachedStyleKey = key;
    m_cachedTreeStyle = styleSheet;
    m_cachedFilterStyle = filterStyle;
}

void FluentTreeView::filterItems(const QString& filter) {
    FLUENT_PROFILE("FluentTreeView::filterItems");
    if (filter.isEmpty()) {
        clearFilter();
        return;
    }

    QElapsedTimer timer;
    timer.start();

    QTreeWidgetItemIterator it(m_treeWidget);
    while (*it) {
        bool matches = itemMatchesFilter(*it, filter);
        (*it)->setHidden(!matches);

        if (matches) {
            auto* parent = (*it)->parent();
            while (parent) {
                parent->setHidden(false);
                parent = parent->parent();
            }
        }
        ++it;
    }

    auto elapsed = std::chrono::milliseconds(timer.elapsed());
    FluentQt::Core::FluentPerformanceMonitor::instance().recordComponentUpdate(
        "FluentTreeView", elapsed);
}

void FluentTreeView::setVirtualizationEnabled(bool enabled) {
    m_virtualizationEnabled = enabled;
    if (enabled) {
        updateVirtualizationWindow();
    } else {
        // Unhide all items when disabling
        for (QTreeWidgetItemIterator it(m_treeWidget); *it; ++it) {
            (*it)->setHidden(false);
        }
    }
}

bool FluentTreeView::isVirtualizationEnabled() const {
    return m_virtualizationEnabled;
}

bool FluentTreeView::itemMatchesFilter(QTreeWidgetItem* item,
                                       const QString& filter) const {
    if (!item)
        return false;

    // Check item text
    for (int i = 0; i < item->columnCount(); ++i) {
        if (item->text(i).contains(filter, Qt::CaseInsensitive)) {
            return true;
        }
    }

    // Check children
    for (int i = 0; i < item->childCount(); ++i) {
        if (itemMatchesFilter(item->child(i), filter)) {
            return true;
        }
    }

    return false;
}

// Advanced Virtualization Implementation

void FluentTreeView::setVirtualizationChunkSize(int chunkSize) {
    m_virtualizationChunkSize = qMax(10, chunkSize);
}

void FluentTreeView::setVirtualizationOverscan(int overscan) {
    m_virtualizationOverscan = std::max(0, overscan);
    if (m_virtualizationEnabled) {
        updateVirtualizationWindow();
    }
}

void FluentTreeView::setColumnVirtualizationEnabled(bool enabled) {
    if (m_columnVirtualizationEnabled != enabled) {
        m_columnVirtualizationEnabled = enabled;
        if (m_virtualizationEnabled) {
            updateVirtualizationWindow();
        }
    }
}

void FluentTreeView::updateColumnVirtualization() {
    if (!m_columnVirtualizationEnabled || !m_treeWidget)
        return;

    const auto header = m_treeWidget->header();
    const int scrollX = m_treeWidget->horizontalScrollBar()->value();
    const int viewportWidth = m_treeWidget->viewport()->width();

    int firstVisibleColumn = 0;
    int lastVisibleColumn = header->count() - 1;
    int currentX = 0;

    // Find first visible column
    for (int i = 0; i < header->count(); ++i) {
        const int columnWidth = header->sectionSize(i);
        if (currentX + columnWidth > scrollX) {
            firstVisibleColumn = i;
            break;
        }
        currentX += columnWidth;
    }

    // Find last visible column
    currentX = 0;
    for (int i = 0; i < header->count(); ++i) {
        const int columnWidth = header->sectionSize(i);
        currentX += columnWidth;
        if (currentX >= scrollX + viewportWidth) {
            lastVisibleColumn = i;
            break;
        }
    }

    m_virtualizationWindow.firstVisibleColumn = firstVisibleColumn;
    m_virtualizationWindow.lastVisibleColumn = lastVisibleColumn;
}

void FluentTreeView::renderVirtualizedItems() {
    if (!m_virtualizationEnabled || !m_treeWidget)
        return;

    FLUENT_PROFILE("FluentTreeView::renderVirtualizedItems");

    m_visibleItems.clear();
    int totalItems = 0;
    int renderedItems = 0;

    // Process items in chunks to avoid UI blocking
    QTreeWidgetItemIterator it(m_treeWidget);
    int currentIndex = 0;

    while (*it) {
        totalItems++;

        const bool shouldBeVisible =
            (currentIndex >= m_virtualizationWindow.firstVisibleRow &&
             currentIndex <= m_virtualizationWindow.lastVisibleRow);

        if (shouldBeVisible) {
            (*it)->setHidden(false);
            m_visibleItems.append(*it);
            renderedItems++;

            // Update virtualized item data
            VirtualizedItem& vItem = m_virtualizedItems[*it];
            vItem.item = *it;
            vItem.isVisible = true;
            vItem.lastAccessTime = QDateTime::currentMSecsSinceEpoch();

        } else {
            (*it)->setHidden(true);
        }

        ++it;
        currentIndex++;

        // Process in chunks to maintain responsiveness
        if (currentIndex % m_virtualizationChunkSize == 0) {
            QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }

    // Update metrics
    m_virtualizationMetrics.totalItems = totalItems;
    m_virtualizationMetrics.renderedItems = renderedItems;
    m_virtualizationMetrics.cachedItems = m_virtualizedItems.size();
}

FluentTreeView::VirtualizationMetrics FluentTreeView::getVirtualizationMetrics()
    const {
    return m_virtualizationMetrics;
}

void FluentTreeView::updateVirtualizationCache() {
    if (!m_virtualizationEnabled)
        return;

    // Clean up old cached items (LRU eviction)
    const qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    const qint64 maxAge = 30000;  // 30 seconds

    auto it = m_virtualizedItems.begin();
    while (it != m_virtualizedItems.end()) {
        if (currentTime - it->lastAccessTime > maxAge) {
            it = m_virtualizedItems.erase(it);
        } else {
            ++it;
        }
    }
}

void FluentTreeView::optimizeVirtualizationPerformance() {
    if (!m_virtualizationEnabled)
        return;

    // Adjust chunk size based on performance
    if (m_virtualizationMetrics.renderTime > 16.0) {  // > 60fps
        m_virtualizationChunkSize = qMax(10, m_virtualizationChunkSize - 10);
    } else if (m_virtualizationMetrics.renderTime < 8.0) {  // < 120fps
        m_virtualizationChunkSize = qMin(500, m_virtualizationChunkSize + 20);
    }

    // Adjust overscan based on scroll speed
    // This could be enhanced with scroll velocity detection
}

}  // namespace FluentQt::Components
