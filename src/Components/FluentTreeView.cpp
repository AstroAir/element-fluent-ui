// src/Components/FluentTreeView.cpp
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScrollBar>
#include <QApplication>
#include <QTimer>

namespace FluentQt::Components {

// FluentTreeItem Implementation
FluentTreeItem::FluentTreeItem(QTreeWidget* parent) : QTreeWidgetItem(parent) {
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

FluentTreeItem::FluentTreeItem(QTreeWidgetItem* parent) : QTreeWidgetItem(parent) {
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

bool FluentTreeItem::isExpandable() const {
    return m_expandable;
}

void FluentTreeItem::setCheckable(bool checkable) {
    m_checkable = checkable;
    if (checkable) {
        setFlags(flags() | Qt::ItemIsUserCheckable);
        setCheckState(0, Qt::Unchecked);
    } else {
        setFlags(flags() & ~Qt::ItemIsUserCheckable);
    }
}

bool FluentTreeItem::isCheckable() const {
    return m_checkable;
}

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

bool FluentTreeItem::isLazyLoading() const {
    return m_lazyLoading;
}

// FluentTreeView Implementation
FluentTreeView::FluentTreeView(QWidget* parent) : Core::FluentComponent(parent) {
    setupTreeWidget();
    setupFilterBar();
    updateTreeStyling();
    
    connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::themeChanged,
            this, &FluentTreeView::updateTreeStyling);
}

void FluentTreeView::setSelectionMode(FluentTreeSelectionMode mode) {
    if (m_selectionMode != mode) {
        m_selectionMode = mode;

        QAbstractItemView::SelectionMode qtMode;
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

bool FluentTreeView::filteringEnabled() const {
    return m_filteringEnabled;
}

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
        
        header->setSectionResizeMode(i, column.resizable ? 
            QHeaderView::Interactive : QHeaderView::Fixed);
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

FluentTreeItem* FluentTreeView::addChildItem(FluentTreeItem* parent, const QString& text) {
    if (!parent) return nullptr;
    
    auto* item = new FluentTreeItem(parent);
    item->setText(0, text);
    parent->addChild(item);
    return item;
}

void FluentTreeView::removeItem(FluentTreeItem* item) {
    if (!item) return;
    
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

void FluentTreeView::clear() {
    m_treeWidget->clear();
}

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

void FluentTreeView::expandAll() {
    m_treeWidget->expandAll();
}

void FluentTreeView::collapseAll() {
    m_treeWidget->collapseAll();
}

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
        painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), cornerRadius(), cornerRadius());
    }
    
    Core::FluentComponent::paintEvent(event);
}

void FluentTreeView::resizeEvent(QResizeEvent* event) {
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
            int autoWidth = (totalWidth - fixedWidth) / autoColumns;
            for (int i = 0; i < static_cast<int>(m_columns.size()); ++i) {
                if (m_columns[i].width <= 0) {
                    header->resizeSection(i, autoWidth);
                }
            }
        }
    }
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
    m_currentFilter = text;
    filterItems(text);
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
    
    connect(m_treeWidget, &QTreeWidget::itemClicked,
            this, &FluentTreeView::onItemClicked);
    connect(m_treeWidget, &QTreeWidget::itemDoubleClicked,
            [this](QTreeWidgetItem* item, int column) {
                if (auto* fluentItem = dynamic_cast<FluentTreeItem*>(item)) {
                    emit itemDoubleClicked(fluentItem, column);
                }
            });
    connect(m_treeWidget, &QTreeWidget::itemExpanded,
            this, &FluentTreeView::onItemExpanded);
    connect(m_treeWidget, &QTreeWidget::itemCollapsed,
            [this](QTreeWidgetItem* item) {
                if (auto* fluentItem = dynamic_cast<FluentTreeItem*>(item)) {
                    emit itemCollapsed(fluentItem);
                }
            });
    connect(m_treeWidget, &QTreeWidget::itemSelectionChanged,
            this, &FluentTreeView::itemSelectionChanged);
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
    
    connect(m_filterEdit, &QLineEdit::textChanged,
            this, &FluentTreeView::onFilterTextChanged);
    
    m_layout->addWidget(m_filterEdit);
    m_layout->addWidget(m_treeWidget);
}

void FluentTreeView::updateTreeStyling() {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();
    
    QString styleSheet = QString(R"(
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
    )").arg(
        palette.neutralLightest.name(),          // background
        palette.neutralQuaternary.name(),        // border
        QString::number(cornerRadius()),         // border radius
        palette.accent.name(),                   // selection background
        palette.neutralLightest.name(),          // selection text
        palette.neutralLight.name(),             // item border
        palette.neutralLight.name(),             // hover background
        palette.accentDark1.name(),              // active selection
        palette.neutralLighter.name()            // header background
    );
    
    m_treeWidget->setStyleSheet(styleSheet);
    
    // Style filter edit
    QString filterStyle = QString(R"(
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
    )").arg(
        palette.neutralLightest.name(),
        palette.neutralQuaternary.name(),
        QString::number(cornerRadius()),
        palette.accent.name()
    );
    
    m_filterEdit->setStyleSheet(filterStyle);
}

void FluentTreeView::filterItems(const QString& filter) {
    if (filter.isEmpty()) {
        clearFilter();
        return;
    }
    
    QTreeWidgetItemIterator it(m_treeWidget);
    while (*it) {
        bool matches = itemMatchesFilter(*it, filter);
        (*it)->setHidden(!matches);
        
        // Show parent items if any child matches
        if (matches) {
            auto* parent = (*it)->parent();
            while (parent) {
                parent->setHidden(false);
                parent = parent->parent();
            }
        }
        
        ++it;
    }
}

bool FluentTreeView::itemMatchesFilter(QTreeWidgetItem* item, const QString& filter) const {
    if (!item) return false;
    
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

} // namespace FluentQt::Components