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
    setupAccessibility();
    updateTreeStyling();

    // Setup filter debounce timer
    m_filterDebounceTimer.setSingleShot(true);
    connect(&m_filterDebounceTimer, &QTimer::timeout, this,
            [this]() { this->filterItems(m_currentFilter); });

    // Setup hover effect timer
    m_hoverEffectTimer.setSingleShot(true);
    m_hoverEffectTimer.setInterval(16);  // ~60fps for smooth effects
    connect(&m_hoverEffectTimer, &QTimer::timeout, this, [this]() {
        update();  // Trigger repaint for hover effects
    });

    // Setup advanced virtualization timer (simplified)
    m_virtualizationUpdateTimer.setSingleShot(true);
    m_virtualizationUpdateTimer.setInterval(16);  // ~60fps
    connect(&m_virtualizationUpdateTimer, &QTimer::timeout, this, [this]() {
        updateVirtualizationWindow();
        updateVirtualizationCache();
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

    // Connect to theme changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::themeChanged, this,
            &FluentTreeView::updateTreeStyling);

    // Connect to theme accessibility changes
    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::highContrastModeChanged, this,
            [this](bool enabled) {
                if (!m_highContrastModeOverride) {
                    updateTreeStyling();
                }
            });

    connect(&Styling::FluentTheme::instance(),
            &Styling::FluentTheme::reducedMotionModeChanged, this,
            [this](bool enabled) { setAnimationsEnabled(!enabled); });

    // Apply initial theme variant
    applyThemeVariant();
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

    // Use semantic colors with high contrast support
    const bool highContrast =
        m_highContrastModeOverride || theme.isHighContrastMode();
    const QColor backgroundColor =
        highContrast ? palette.highContrastBackground : palette.surface;
    const QColor borderColor =
        highContrast ? palette.borderFocus : palette.border;

    // Paint background
    painter.fillRect(rect(), backgroundColor);

    // Paint reveal effect if enabled and mouse is over the component
    if (m_revealEffectEnabled && rect().contains(m_lastMousePos)) {
        paintRevealEffect(&painter, rect());
    }

    // Paint focus indicator with proper accessibility support
    if (hasFocus()) {
        paintFocusIndicator(&painter, rect());
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

void FluentTreeView::keyPressEvent(QKeyEvent* event) {
    // Handle keyboard navigation first
    handleKeyboardNavigation(event);

    // If not handled by keyboard navigation, pass to base class
    if (!event->isAccepted()) {
        Core::FluentComponent::keyPressEvent(event);
    }
}

void FluentTreeView::focusInEvent(QFocusEvent* event) {
    Core::FluentComponent::focusInEvent(event);

    // Set initial focus item if none is set
    if (!m_focusItem && m_treeWidget->topLevelItemCount() > 0) {
        if (auto* item =
                dynamic_cast<FluentTreeItem*>(m_treeWidget->topLevelItem(0))) {
            setFocusItem(item);
        }
    }

    // Announce focus to screen readers
    if (m_focusItem) {
        announceToScreenReader(QString("Focused on tree view, current item: %1")
                                   .arg(m_focusItem->text(0)));
    }

    update();  // Trigger repaint for focus indicator
}

void FluentTreeView::focusOutEvent(QFocusEvent* event) {
    Core::FluentComponent::focusOutEvent(event);
    update();  // Trigger repaint to remove focus indicator
}

void FluentTreeView::mouseMoveEvent(QMouseEvent* event) {
    Core::FluentComponent::mouseMoveEvent(event);

    // Update mouse position for reveal effects
    m_lastMousePos = event->pos();

    // Update hover effects if enabled
    if (m_revealEffectEnabled) {
        updateHoverEffects(event->pos());
        m_hoverEffectTimer.start();  // Trigger smooth hover effect updates
    }
}

void FluentTreeView::leaveEvent(QEvent* event) {
    Core::FluentComponent::leaveEvent(event);

    // Clear mouse position and hover effects
    m_lastMousePos = QPoint(-1, -1);

    if (m_revealEffectEnabled) {
        update();  // Trigger repaint to clear hover effects
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

    // Check for high contrast mode
    const bool highContrast =
        m_highContrastModeOverride || theme.isHighContrastMode();
    const bool compactMode = m_compactMode;
    const bool touchMode = m_touchMode;

    // Build a cache key including theme variant information
    const QString key = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11|%12")
                            .arg(palette.neutralLightest.name())
                            .arg(palette.neutralQuaternary.name())
                            .arg(QString::number(cornerRadius()))
                            .arg(palette.accent.name())
                            .arg(palette.neutralLightest.name())
                            .arg(palette.neutralLight.name())
                            .arg(palette.neutralLight.name())
                            .arg(palette.accentDark1.name())
                            .arg(palette.neutralLighter.name())
                            .arg(highContrast ? "hc" : "normal")
                            .arg(compactMode ? "compact" : "normal")
                            .arg(touchMode ? "touch" : "normal");

    if (key == m_cachedStyleKey && !m_cachedTreeStyle.isEmpty() &&
        !m_cachedFilterStyle.isEmpty()) {
        m_treeWidget->setStyleSheet(m_cachedTreeStyle);
        m_filterEdit->setStyleSheet(m_cachedFilterStyle);
        return;
    }

    // Use design tokens for spacing and sizing
    const int itemPadding = compactMode ? theme.paddingValue("xs")
                            : touchMode ? theme.paddingValue("lg")
                                        : theme.paddingValue("sm");
    const int borderRadius = theme.borderRadius("medium");
    const int headerPadding = compactMode ? theme.paddingValue("xs")
                              : touchMode ? theme.paddingValue("md")
                                          : theme.paddingValue("sm");

    // Use semantic colors with high contrast support
    const QColor backgroundColor =
        highContrast ? palette.highContrastBackground : palette.surface;
    const QColor textColor =
        highContrast ? palette.highContrastText : palette.neutralPrimary;
    const QColor borderColor =
        highContrast ? palette.borderFocus : palette.border;
    const QColor hoverColor =
        highContrast ? palette.hover : palette.neutralLight;
    const QColor selectionColor =
        highContrast ? palette.focusIndicator : palette.accent;
    const QColor selectionTextColor = palette.neutralLightest;

    // Recompute and cache styles using design tokens
    QString styleSheet =
        QString(R"(
        QTreeWidget {
            background-color: %1;
            border: 1px solid %2;
            border-radius: %3px;
            outline: none;
            selection-background-color: %4;
            selection-color: %5;
            font-family: %6;
            font-size: %7px;
        }
        QTreeWidget::item {
            padding: %8px;
            border-bottom: 1px solid %9;
            min-height: %10px;
        }
        QTreeWidget::item:hover {
            background-color: %11;
        }
        QTreeWidget::item:selected {
            background-color: %4;
            color: %5;
        }
        QTreeWidget::item:selected:active {
            background-color: %12;
        }
        QTreeWidget::item:focus {
            outline: 2px solid %13;
            outline-offset: -2px;
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
            background-color: %14;
            padding: %15px;
            border: none;
            border-bottom: 2px solid %2;
            font-weight: 600;
            font-family: %16;
        }
    )")
            .arg(backgroundColor.name())         // 1: background
            .arg(borderColor.name())             // 2: border
            .arg(QString::number(borderRadius))  // 3: border radius
            .arg(selectionColor.name())          // 4: selection background
            .arg(selectionTextColor.name())      // 5: selection text
            .arg(theme.bodyFont().family())      // 6: font family
            .arg(QString::number(theme.bodyFont().pointSize()))  // 7: font size
            .arg(QString::number(itemPadding))         // 8: item padding
            .arg(palette.neutralQuaternaryAlt.name())  // 9: item border
            .arg(QString::number(touchMode     ? 44
                                 : compactMode ? 28
                                               : 32))  // 10: min height
            .arg(hoverColor.name())                    // 11: hover background
            .arg(palette.accentDark1.name())           // 12: active selection
            .arg(palette.focusIndicator.name())        // 13: focus outline
            .arg(palette.surfaceSecondary.name())      // 14: header background
            .arg(QString::number(headerPadding))       // 15: header padding
            .arg(theme.subtitleFont().family())        // 16: header font family
        ;

    QString filterStyle =
        QString(R"(
        QLineEdit {
            background-color: %1;
            border: 2px solid %2;
            border-radius: %3px;
            padding: %4px %5px;
            font-size: %6px;
            font-family: %7;
            color: %8;
            min-height: %9px;
        }
        QLineEdit:focus {
            border-color: %10;
            outline: none;
        }
        QLineEdit:hover {
            border-color: %11;
        }
        QLineEdit::placeholder {
            color: %12;
        }
    )")
            .arg(backgroundColor.name())            // 1: background
            .arg(borderColor.name())                // 2: border
            .arg(QString::number(borderRadius))     // 3: border radius
            .arg(QString::number(itemPadding))      // 4: vertical padding
            .arg(QString::number(itemPadding + 4))  // 5: horizontal padding
            .arg(QString::number(theme.bodyFont().pointSize()))  // 6: font size
            .arg(theme.bodyFont().family())  // 7: font family
            .arg(textColor.name())           // 8: text color
            .arg(QString::number(touchMode     ? 44
                                 : compactMode ? 32
                                               : 36))  // 9: min height
            .arg(selectionColor.name())                // 10: focus border
            .arg(palette.borderHover.name())           // 11: hover border
            .arg(palette.neutralTertiary.name())       // 12: placeholder color
        ;

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

// Helper Methods Implementation

void FluentTreeView::setupAccessibility() {
    // Set up basic accessibility attributes
    setAccessibleName("Tree View");
    setAccessibleDescription("Hierarchical tree view component");

    // Set ARIA role for the tree
    m_treeWidget->setAccessibleName("tree");
    m_treeWidget->setAccessibleDescription(
        "Tree widget with hierarchical data");

    // Enable focus policy for keyboard navigation
    m_treeWidget->setFocusPolicy(Qt::StrongFocus);
    setFocusPolicy(Qt::StrongFocus);

    // Set up initial accessibility attributes
    updateAccessibilityAttributes();
}

void FluentTreeView::updateAccessibilityAttributes() {
    if (!m_accessibleName.isEmpty()) {
        QWidget::setAccessibleName(m_accessibleName);
        m_treeWidget->setAccessibleName(m_accessibleName);
    }

    if (!m_accessibleDescription.isEmpty()) {
        QWidget::setAccessibleDescription(m_accessibleDescription);
        m_treeWidget->setAccessibleDescription(m_accessibleDescription);
    }
}

void FluentTreeView::announceToScreenReader(const QString& message) {
    // Use QAccessible to announce changes to screen readers
    if (QAccessible::isActive()) {
        QAccessibleEvent event(this, QAccessible::Alert);
        event.setText(message);
        QAccessible::updateAccessibility(&event);
    }
}

void FluentTreeView::handleKeyboardNavigation(QKeyEvent* event) {
    if (!m_keyboardNavigationEnabled) {
        return;
    }

    switch (event->key()) {
        case Qt::Key_Up:
            moveFocusUp();
            event->accept();
            break;
        case Qt::Key_Down:
            moveFocusDown();
            event->accept();
            break;
        case Qt::Key_Left:
            if (m_focusItem && m_focusItem->isExpanded()) {
                collapseItem(m_focusItem);
            } else {
                moveFocusToParent();
            }
            event->accept();
            break;
        case Qt::Key_Right:
            if (m_focusItem && !m_focusItem->isExpanded() &&
                m_focusItem->childCount() > 0) {
                expandItem(m_focusItem);
            } else {
                moveFocusToFirstChild();
            }
            event->accept();
            break;
        case Qt::Key_Home:
            if (m_treeWidget->topLevelItemCount() > 0) {
                if (auto* item = dynamic_cast<FluentTreeItem*>(
                        m_treeWidget->topLevelItem(0))) {
                    setFocusItem(item);
                }
            }
            event->accept();
            break;
        case Qt::Key_End:
            // Find the last visible item
            if (m_treeWidget->topLevelItemCount() > 0) {
                QTreeWidgetItem* lastItem = nullptr;
                QTreeWidgetItemIterator it(m_treeWidget);
                while (*it) {
                    if (!(*it)->isHidden()) {
                        lastItem = *it;
                    }
                    ++it;
                }
                if (auto* item = dynamic_cast<FluentTreeItem*>(lastItem)) {
                    setFocusItem(item);
                }
            }
            event->accept();
            break;
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (m_focusItem) {
                emit itemClicked(m_focusItem, 0);
                if (m_focusItem->isCheckable()) {
                    Qt::CheckState newState =
                        m_focusItem->checkState(0) == Qt::Checked
                            ? Qt::Unchecked
                            : Qt::Checked;
                    m_focusItem->setCheckState(0, newState);
                }
            }
            event->accept();
            break;
        default:
            // Let the base class handle other keys
            break;
    }
}

void FluentTreeView::selectItemWithKeyboard(FluentTreeItem* item) {
    if (item && m_keyboardNavigationEnabled) {
        setCurrentItem(item);
        setFocusItem(item);

        // Announce selection to screen readers
        announceToScreenReader(QString("Selected: %1").arg(item->text(0)));
    }
}

void FluentTreeView::applyThemeVariant() {
    // Apply spacing adjustments based on theme variant
    updateSpacingForVariant();

    // Update mouse tracking for touch mode
    if (m_touchMode) {
        setMouseTracking(true);
        m_treeWidget->setMouseTracking(true);
    }
}

void FluentTreeView::updateSpacingForVariant() {
    const auto& theme = Styling::FluentTheme::instance();

    // Adjust layout margins based on variant
    int margin = m_compactMode ? theme.marginsValue("xs")
                 : m_touchMode ? theme.marginsValue("lg")
                               : theme.marginsValue("sm");

    m_layout->setContentsMargins(margin, margin, margin, margin);

    // Adjust spacing between filter and tree
    int spacing = m_compactMode ? theme.spacing("xs")
                  : m_touchMode ? theme.spacing("lg")
                                : theme.spacing("sm");

    m_layout->setSpacing(spacing);
}

void FluentTreeView::paintRevealEffect(QPainter* painter, const QRect& rect) {
    if (!m_revealEffectEnabled || m_lastMousePos.x() < 0) {
        return;
    }

    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Create a subtle radial gradient for the reveal effect
    QRadialGradient gradient(m_lastMousePos, 100);
    QColor revealColor = palette.accent;
    revealColor.setAlpha(20);  // Very subtle effect

    gradient.setColorAt(0, revealColor);
    gradient.setColorAt(1, Qt::transparent);

    painter->save();
    painter->setBrush(QBrush(gradient));
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect);
    painter->restore();
}

void FluentTreeView::paintFocusIndicator(QPainter* painter, const QRect& rect) {
    const auto& theme = Styling::FluentTheme::instance();
    const auto& palette = theme.currentPalette();

    // Use high contrast focus indicator if needed
    const bool highContrast =
        m_highContrastModeOverride || theme.isHighContrastMode();
    const QColor focusColor =
        highContrast ? palette.focusIndicator : palette.accent;

    painter->save();
    QPen focusPen(focusColor, 2);
    focusPen.setStyle(Qt::SolidLine);
    painter->setPen(focusPen);
    painter->setBrush(Qt::NoBrush);

    // Draw focus rectangle with rounded corners
    const int radius = cornerRadius();
    painter->drawRoundedRect(rect.adjusted(1, 1, -1, -1), radius, radius);
    painter->restore();
}

void FluentTreeView::updateHoverEffects(const QPoint& mousePos) {
    // This method can be used to update hover effects on specific items
    // For now, we just store the mouse position for the reveal effect
    Q_UNUSED(mousePos);

    // Future enhancement: could highlight specific tree items under the mouse
}

// Accessibility Methods
QString FluentTreeView::accessibleName() const {
    return m_accessibleName.isEmpty() ? QWidget::accessibleName()
                                      : m_accessibleName;
}

void FluentTreeView::setAccessibleName(const QString& name) {
    if (m_accessibleName != name) {
        m_accessibleName = name;
        updateAccessibilityAttributes();
        emit accessibleNameChanged(name);
    }
}

QString FluentTreeView::accessibleDescription() const {
    return m_accessibleDescription;
}

void FluentTreeView::setAccessibleDescription(const QString& description) {
    if (m_accessibleDescription != description) {
        m_accessibleDescription = description;
        updateAccessibilityAttributes();
        emit accessibleDescriptionChanged(description);
    }
}

bool FluentTreeView::isKeyboardNavigationEnabled() const {
    return m_keyboardNavigationEnabled;
}

void FluentTreeView::setKeyboardNavigationEnabled(bool enabled) {
    if (m_keyboardNavigationEnabled != enabled) {
        m_keyboardNavigationEnabled = enabled;
        emit keyboardNavigationEnabledChanged(enabled);
    }
}

// Theme Variant Methods
bool FluentTreeView::isCompactMode() const { return m_compactMode; }

void FluentTreeView::setCompactMode(bool enabled) {
    if (m_compactMode != enabled) {
        m_compactMode = enabled;
        applyThemeVariant();
        updateTreeStyling();
        emit compactModeChanged(enabled);
    }
}

bool FluentTreeView::isTouchMode() const { return m_touchMode; }

void FluentTreeView::setTouchMode(bool enabled) {
    if (m_touchMode != enabled) {
        m_touchMode = enabled;
        applyThemeVariant();
        updateTreeStyling();
        emit touchModeChanged(enabled);
    }
}

bool FluentTreeView::isHighContrastMode() const {
    return m_highContrastModeOverride;
}

void FluentTreeView::setHighContrastMode(bool enabled) {
    if (m_highContrastModeOverride != enabled) {
        m_highContrastModeOverride = enabled;
        updateTreeStyling();
        emit highContrastModeChanged(enabled);
    }
}

// Visual Effect Methods
bool FluentTreeView::isRevealEffectEnabled() const {
    return m_revealEffectEnabled;
}

void FluentTreeView::setRevealEffectEnabled(bool enabled) {
    if (m_revealEffectEnabled != enabled) {
        m_revealEffectEnabled = enabled;
        emit revealEffectEnabledChanged(enabled);
    }
}

bool FluentTreeView::areAnimationsEnabled() const {
    return m_animationsEnabled;
}

void FluentTreeView::setAnimationsEnabled(bool enabled) {
    if (m_animationsEnabled != enabled) {
        m_animationsEnabled = enabled;
        setAnimated(enabled);
        emit animationsEnabledChanged(enabled);
    }
}

// Focus Management Methods
void FluentTreeView::setFocusItem(FluentTreeItem* item) {
    if (m_focusItem != item) {
        m_focusItem = item;
        if (item) {
            m_treeWidget->setCurrentItem(item);
            m_treeWidget->scrollToItem(item);
        }
        emit focusItemChanged(item);
    }
}

FluentTreeItem* FluentTreeView::focusItem() const { return m_focusItem; }

void FluentTreeView::moveFocusUp() {
    if (!m_keyboardNavigationEnabled || !m_focusItem)
        return;

    auto* item = m_treeWidget->itemAbove(m_focusItem);
    if (item) {
        if (auto* fluentItem = dynamic_cast<FluentTreeItem*>(item)) {
            setFocusItem(fluentItem);
        }
    }
}

void FluentTreeView::moveFocusDown() {
    if (!m_keyboardNavigationEnabled || !m_focusItem)
        return;

    auto* item = m_treeWidget->itemBelow(m_focusItem);
    if (item) {
        if (auto* fluentItem = dynamic_cast<FluentTreeItem*>(item)) {
            setFocusItem(fluentItem);
        }
    }
}

void FluentTreeView::moveFocusToParent() {
    if (!m_keyboardNavigationEnabled || !m_focusItem)
        return;

    auto* parent = m_focusItem->parent();
    if (parent) {
        if (auto* fluentParent = dynamic_cast<FluentTreeItem*>(parent)) {
            setFocusItem(fluentParent);
        }
    }
}

void FluentTreeView::moveFocusToFirstChild() {
    if (!m_keyboardNavigationEnabled || !m_focusItem)
        return;

    if (m_focusItem->childCount() > 0) {
        auto* child = m_focusItem->child(0);
        if (auto* fluentChild = dynamic_cast<FluentTreeItem*>(child)) {
            setFocusItem(fluentChild);
        }
    }
}

}  // namespace FluentQt::Components
