// src/Core/FluentResponsiveLayout.cpp
#include "FluentQt/Core/FluentResponsiveLayout.h"
#include <QApplication>
#include <QDebug>
#include <QMutexLocker>
#include <QTimer>
#include <QWidget>
#include <algorithm>
#include "FluentQt/Core/FluentResponsive.h"

namespace FluentQt::Core {

FluentResponsiveLayout::FluentResponsiveLayout(QWidget* parent)
    : QLayout(parent) {
    // Connect to responsive manager
    auto& responsiveManager = FluentResponsiveManager::instance();
    connect(&responsiveManager, &FluentResponsiveManager::breakpointChanged,
            this, &FluentResponsiveLayout::onBreakpointChanged);

    qDebug() << "FluentResponsiveLayout initialized";
}

FluentResponsiveLayout::FluentResponsiveLayout(
    const FluentResponsiveLayoutConfig& config, QWidget* parent)
    : QLayout(parent), m_config(config) {
    // Connect to responsive manager
    auto& responsiveManager = FluentResponsiveManager::instance();
    connect(&responsiveManager, &FluentResponsiveManager::breakpointChanged,
            this, &FluentResponsiveLayout::onBreakpointChanged);

    qDebug() << "FluentResponsiveLayout initialized";
}

FluentResponsiveLayout::~FluentResponsiveLayout() {
    // Clean up all items
    while (!m_items.isEmpty()) {
        delete takeAt(0);
    }
}

// QLayout interface implementations
void FluentResponsiveLayout::addItem(QLayoutItem* item) {
    if (!item)
        return;

    m_items.append(item);
    m_itemConfigs.append(FluentLayoutItemConfig{});
    invalidate();
}

QLayoutItem* FluentResponsiveLayout::itemAt(int index) const {
    if (index < 0 || index >= m_items.size()) {
        return nullptr;
    }
    return m_items.at(index);
}

QLayoutItem* FluentResponsiveLayout::takeAt(int index) {
    if (index < 0 || index >= m_items.size()) {
        return nullptr;
    }

    QLayoutItem* item = m_items.takeAt(index);
    if (index < m_itemConfigs.size()) {
        m_itemConfigs.removeAt(index);
    }
    invalidate();
    return item;
}

int FluentResponsiveLayout::count() const { return m_items.size(); }

QSize FluentResponsiveLayout::sizeHint() const {
    if (m_items.isEmpty()) {
        return QSize(100, 100);
    }

    QSize totalSize;
    auto& responsiveManager = FluentResponsiveManager::instance();
    FluentBreakpoint currentBreakpoint =
        responsiveManager.getCurrentBreakpoint();

    // Calculate size based on current layout type and breakpoint
    switch (m_config.type) {
        case FluentLayoutType::Grid:
            totalSize = calculateGridSizeHint();
            // Adjust for smaller breakpoints
            if (currentBreakpoint == FluentBreakpoint::XSmall ||
                currentBreakpoint == FluentBreakpoint::Small) {
                totalSize = totalSize * 0.8;  // Reduce size for smaller screens
            }
            break;
        case FluentLayoutType::Flexbox:
            totalSize = calculateFlexboxSizeHint();
            break;
        case FluentLayoutType::Masonry:
            totalSize = calculateMasonrySizeHint();
            break;
        case FluentLayoutType::Flow:
            totalSize = QSize(200, 200);  // Default size
            break;
        default:
            totalSize = QSize(200, 200);  // Default size
            break;
    }

    return totalSize;
}

QSize FluentResponsiveLayout::minimumSize() const {
    QSize minSize;

    for (QLayoutItem* item : m_items) {
        if (item) {
            QSize itemMinSize = item->minimumSize();
            minSize = minSize.expandedTo(itemMinSize);
        }
    }

    return minSize.isEmpty() ? QSize(50, 50) : minSize;
}

QSize FluentResponsiveLayout::maximumSize() const {
    return QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

void FluentResponsiveLayout::setGeometry(const QRect& rect) {
    QLayout::setGeometry(rect);

    if (m_items.isEmpty()) {
        return;
    }

    // Get current breakpoint for future use if needed
    // auto& responsiveManager = FluentResponsiveManager::instance();
    // FluentBreakpoint currentBreakpoint =
    // responsiveManager.getCurrentBreakpoint();

    // Apply layout based on current type
    switch (m_config.type) {
        case FluentLayoutType::Grid:
            calculateGridLayout(rect);
            break;
        case FluentLayoutType::Flexbox:
            calculateFlexboxLayout(rect);
            break;
        case FluentLayoutType::Masonry:
            calculateMasonryLayout(rect);
            break;
        case FluentLayoutType::Flow:
            calculateFlowLayout(rect);
            break;
        default:
            // Default vertical layout
            int y = rect.top();
            for (QLayoutItem* item : m_items) {
                if (item) {
                    QSize itemSize = item->sizeHint();
                    item->setGeometry(
                        QRect(rect.left(), y, rect.width(), itemSize.height()));
                    y += itemSize.height();
                }
            }
            break;
    }
}

bool FluentResponsiveLayout::hasHeightForWidth() const {
    return m_config.type == FluentLayoutType::Flow ||
           m_config.type == FluentLayoutType::Masonry;
}

int FluentResponsiveLayout::heightForWidth(int width) const {
    if (!hasHeightForWidth()) {
        return -1;
    }

    // Calculate height based on width for flow/masonry layouts
    int totalHeight = 0;
    int currentRowHeight = 0;
    int currentRowWidth = 0;

    for (QLayoutItem* item : m_items) {
        if (item) {
            QSize itemSize = item->sizeHint();

            if (currentRowWidth + itemSize.width() > width) {
                // Start new row
                totalHeight += currentRowHeight;
                currentRowHeight = itemSize.height();
                currentRowWidth = itemSize.width();
            } else {
                // Add to current row
                currentRowWidth += itemSize.width();
                currentRowHeight =
                    std::max(currentRowHeight, itemSize.height());
            }
        }
    }

    totalHeight += currentRowHeight;
    return totalHeight;
}

Qt::Orientations FluentResponsiveLayout::expandingDirections() const {
    return Qt::Horizontal | Qt::Vertical;
}

void FluentResponsiveLayout::invalidate() {
    QLayout::invalidate();
    m_cachedSizeHint = QSize();
    m_sizeHintValid = false;
}

// Configuration methods
void FluentResponsiveLayout::setConfig(
    const FluentResponsiveLayoutConfig& config) {
    m_config = config;
    invalidate();
}

// Widget management methods
void FluentResponsiveLayout::addWidget(QWidget* widget,
                                       const FluentLayoutItemConfig& config) {
    if (!widget)
        return;

    addItem(new QWidgetItem(widget));

    // Store item config in the parallel list
    if (m_itemConfigs.size() < m_items.size()) {
        m_itemConfigs.append(config);
    }
}

void FluentResponsiveLayout::removeWidget(QWidget* widget) {
    if (!widget)
        return;

    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i] && m_items[i]->widget() == widget) {
            delete takeAt(i);
            break;
        }
    }
}

// Private slot implementations
void FluentResponsiveLayout::onBreakpointChanged(FluentBreakpoint breakpoint) {
    m_currentBreakpoint = breakpoint;
    invalidate();
    emit breakpointChanged(breakpoint);
}

void FluentResponsiveLayout::onContainerSizeChanged() {
    // Handle container size changes
    invalidate();
}

void FluentResponsiveLayout::onAnimationFinished() {
    // Handle animation completion
    emit animationFinished();
}

// Private helper methods - only implement methods that are declared in the
// header

// Implement the methods that are actually declared in the header
void FluentResponsiveLayout::calculateFlexboxLayout(const QRect& rect) {
    // Simplified flexbox layout calculation - vertical layout
    int y = rect.top();
    for (QLayoutItem* item : m_items) {
        if (item) {
            QSize itemSize = item->sizeHint();
            item->setGeometry(
                QRect(rect.left(), y, rect.width(), itemSize.height()));
            y += itemSize.height();
        }
    }
}

void FluentResponsiveLayout::calculateGridLayout(const QRect& rect) {
    // Simplified grid layout calculation using gridColumns size
    int columns = m_config.gridColumns.size();
    if (columns <= 0)
        columns = 3;  // Default to 3 columns

    int itemWidth = rect.width() / columns;
    int itemHeight = rect.height() / ((m_items.size() + columns - 1) / columns);

    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i]) {
            int row = i / columns;
            int col = i % columns;

            QRect itemRect(rect.left() + col * itemWidth,
                           rect.top() + row * itemHeight, itemWidth,
                           itemHeight);
            m_items[i]->setGeometry(itemRect);
        }
    }
}

void FluentResponsiveLayout::calculateMasonryLayout(const QRect& rect) {
    // Simplified masonry layout calculation - use columns from config
    int columns = m_config.masonryColumns;
    if (columns <= 0)
        columns = 3;

    QList<int> columnHeights(columns, rect.top());
    int itemWidth = rect.width() / columns;

    for (QLayoutItem* item : m_items) {
        if (item) {
            // Find shortest column
            int shortestCol = 0;
            for (int i = 1; i < columns; ++i) {
                if (columnHeights[i] < columnHeights[shortestCol]) {
                    shortestCol = i;
                }
            }

            QSize itemSize = item->sizeHint();
            int itemHeight = itemSize.height();

            QRect itemRect(rect.left() + shortestCol * itemWidth,
                           columnHeights[shortestCol], itemWidth, itemHeight);
            item->setGeometry(itemRect);

            columnHeights[shortestCol] +=
                itemHeight + m_config.containerSpacing;
        }
    }
}

void FluentResponsiveLayout::calculateStackLayout(const QRect& rect) {
    // Stack layout - items on top of each other
    for (QLayoutItem* item : m_items) {
        if (item) {
            item->setGeometry(rect);
        }
    }
}

void FluentResponsiveLayout::calculateFlowLayout(const QRect& rect) {
    // Flow layout - items flow horizontally then wrap
    int x = rect.left();
    int y = rect.top();
    int maxHeight = 0;

    for (QLayoutItem* item : m_items) {
        if (item) {
            QSize itemSize = item->sizeHint();

            if (x + itemSize.width() > rect.right()) {
                // Wrap to next line
                x = rect.left();
                y += maxHeight;
                maxHeight = 0;
            }

            item->setGeometry(QRect(x, y, itemSize.width(), itemSize.height()));
            x += itemSize.width();
            maxHeight = std::max(maxHeight, itemSize.height());
        }
    }
}

void FluentResponsiveLayout::calculateAdaptiveLayout(const QRect& rect) {
    // Adaptive layout - choose best layout based on container size
    if (rect.width() > rect.height()) {
        calculateFlexboxLayout(rect);
    } else {
        calculateStackLayout(rect);
    }
}

// Additional method implementations
void FluentResponsiveLayout::addWidget(QWidget* widget, int stretch,
                                       Qt::Alignment alignment) {
    Q_UNUSED(stretch);
    Q_UNUSED(alignment);
    addWidget(widget, FluentLayoutItemConfig{});
}

void FluentResponsiveLayout::insertWidget(
    int index, QWidget* widget, const FluentLayoutItemConfig& config) {
    Q_UNUSED(index);
    addWidget(widget, config);  // Simplified implementation
}

void FluentResponsiveLayout::setItemConfig(
    QWidget* widget, const FluentLayoutItemConfig& config) {
    Q_UNUSED(widget);
    Q_UNUSED(config);
    // Simplified implementation
}

void FluentResponsiveLayout::setItemConfig(
    int index, const FluentLayoutItemConfig& config) {
    Q_UNUSED(index);
    Q_UNUSED(config);
    // Simplified implementation
}

FluentLayoutItemConfig FluentResponsiveLayout::getItemConfig(
    QWidget* widget) const {
    Q_UNUSED(widget);
    return FluentLayoutItemConfig{};
}

FluentLayoutItemConfig FluentResponsiveLayout::getItemConfig(int index) const {
    Q_UNUSED(index);
    return FluentLayoutItemConfig{};
}

void FluentResponsiveLayout::setConfigForBreakpoint(
    FluentBreakpoint breakpoint, const FluentResponsiveLayoutConfig& config) {
    Q_UNUSED(breakpoint);
    Q_UNUSED(config);
    // Simplified implementation
}

void FluentResponsiveLayout::updateForBreakpoint(FluentBreakpoint breakpoint) {
    Q_UNUSED(breakpoint);
    invalidate();
}

void FluentResponsiveLayout::updateForContainerSize(const QSize& size) {
    Q_UNUSED(size);
    invalidate();
}

void FluentResponsiveLayout::enableTransitions(bool enabled) {
    Q_UNUSED(enabled);
    // Simplified implementation
}

void FluentResponsiveLayout::setTransitionDuration(int milliseconds) {
    Q_UNUSED(milliseconds);
    // Simplified implementation
}

void FluentResponsiveLayout::animateToLayout() {
    // Simplified implementation
    emit animationStarted();
    // Would perform actual animation here
    emit animationFinished();
}

void FluentResponsiveLayout::animateItemToPosition(
    QWidget* widget, const QRect& targetGeometry) {
    Q_UNUSED(widget);
    Q_UNUSED(targetGeometry);
    // Simplified implementation
}

QRect FluentResponsiveLayout::calculateItemGeometry(
    int index, const QRect& containerRect) const {
    Q_UNUSED(index);
    return containerRect;  // Simplified implementation
}

QSize FluentResponsiveLayout::calculateLayoutSize(
    const QSize& containerSize) const {
    return containerSize;  // Simplified implementation
}

int FluentResponsiveLayout::getOptimalColumnCount(int containerWidth) const {
    return std::max(1, containerWidth / 200);  // Simplified implementation
}

QList<QRect> FluentResponsiveLayout::calculateMasonryLayout(
    const QRect& containerRect) const {
    Q_UNUSED(containerRect);
    return QList<QRect>{};  // Simplified implementation
}

void FluentResponsiveLayout::enableVirtualization(bool enabled) {
    Q_UNUSED(enabled);
    // Simplified implementation
}

void FluentResponsiveLayout::setVirtualizationThreshold(int threshold) {
    Q_UNUSED(threshold);
    // Simplified implementation
}

void FluentResponsiveLayout::enableLazyLoading(bool enabled) {
    Q_UNUSED(enabled);
    // Simplified implementation
}

void FluentResponsiveLayout::optimizeLayout() {
    // Simplified implementation
    invalidate();
}

// Helper method implementations that are declared in the header
QSize FluentResponsiveLayout::calculateFlexboxSizeHint() const {
    // Simple flexbox size hint calculation
    QSize totalSize(200, 200);  // Default size

    if (!m_items.isEmpty()) {
        int totalHeight = 0;
        int maxWidth = 0;
        for (QLayoutItem* item : m_items) {
            if (item) {
                QSize itemSize = item->sizeHint();
                totalHeight += itemSize.height();
                maxWidth = std::max(maxWidth, itemSize.width());
            }
        }
        totalSize = QSize(maxWidth, totalHeight);
    }

    return totalSize;
}

void FluentResponsiveLayout::distributeFlexSpace(const QRect& containerRect) {
    Q_UNUSED(containerRect);
    // Simplified implementation
}

void FluentResponsiveLayout::alignFlexItems(const QRect& containerRect) {
    Q_UNUSED(containerRect);
    // Simplified implementation
}

QSize FluentResponsiveLayout::calculateGridSizeHint() const {
    int columns = m_config.gridColumns.size();
    if (columns <= 0)
        columns = 3;  // Default to 3 columns
    int rows = (m_items.size() + columns - 1) / columns;

    QSize itemSize(100, 100);  // Default item size
    if (!m_items.isEmpty() && m_items.first()) {
        itemSize = m_items.first()->sizeHint();
    }

    return QSize(columns * itemSize.width(), rows * itemSize.height());
}

QList<int> FluentResponsiveLayout::resolveGridTracks(
    const QList<FluentGridTrack>& tracks, int availableSpace) const {
    Q_UNUSED(tracks);
    Q_UNUSED(availableSpace);
    return QList<int>{};  // Simplified implementation
}

int FluentResponsiveLayout::parseGridSize(const QString& size,
                                          int availableSpace) const {
    Q_UNUSED(size);
    Q_UNUSED(availableSpace);
    return 100;  // Simplified implementation
}

QSize FluentResponsiveLayout::calculateMasonrySizeHint() const {
    // Simple masonry size hint calculation
    QSize totalSize(300, 200);  // Default size

    if (!m_items.isEmpty()) {
        int totalHeight = 0;
        for (QLayoutItem* item : m_items) {
            if (item) {
                totalHeight += item->sizeHint().height();
            }
        }
        totalSize.setHeight(totalHeight / m_config.masonryColumns);
    }

    return totalSize;
}

QList<int> FluentResponsiveLayout::calculateMasonryColumnHeights(
    const QRect& containerRect) const {
    Q_UNUSED(containerRect);
    return QList<int>{};  // Simplified implementation
}

int FluentResponsiveLayout::findShortestColumn(
    const QList<int>& heights) const {
    if (heights.isEmpty())
        return 0;
    return std::distance(heights.begin(),
                         std::min_element(heights.begin(), heights.end()));
}

void FluentResponsiveLayout::setupLayoutAnimation() {
    // Simplified implementation
}

void FluentResponsiveLayout::animateItemsToPositions(
    const QMap<QWidget*, QRect>& targetPositions) {
    Q_UNUSED(targetPositions);
    // Simplified implementation
}

FluentResponsiveLayoutConfig FluentResponsiveLayout::getConfigForBreakpoint(
    FluentBreakpoint breakpoint) const {
    Q_UNUSED(breakpoint);
    return m_config;  // Simplified implementation
}

FluentLayoutItemConfig FluentResponsiveLayout::getItemConfigForBreakpoint(
    int index, FluentBreakpoint breakpoint) const {
    Q_UNUSED(index);
    Q_UNUSED(breakpoint);
    return FluentLayoutItemConfig{};  // Simplified implementation
}

void FluentResponsiveLayout::updateResponsiveConfiguration() {
    // Simplified implementation
    invalidate();
}

QRect FluentResponsiveLayout::applyMargins(const QRect& rect,
                                           const QMargins& margins) const {
    return rect.marginsRemoved(margins);
}

QSize FluentResponsiveLayout::applyConstraints(const QSize& size,
                                               const QSize& minSize,
                                               const QSize& maxSize) const {
    return size.boundedTo(maxSize).expandedTo(minSize);
}

bool FluentResponsiveLayout::shouldShowItem(int index) const {
    Q_UNUSED(index);
    return true;  // Simplified implementation
}

// FluentFlexLayout implementation
FluentFlexLayout::FluentFlexLayout(QWidget* parent)
    : FluentResponsiveLayout(parent) {
    FluentResponsiveLayoutConfig config;
    config.type = FluentLayoutType::Flexbox;
    config.flexDirection = FluentFlexDirection::Column;  // Default to column
    setConfig(config);
}

FluentFlexLayout::FluentFlexLayout(FluentFlexDirection direction,
                                   QWidget* parent)
    : FluentResponsiveLayout(parent) {
    FluentResponsiveLayoutConfig config;
    config.type = FluentLayoutType::Flexbox;
    config.flexDirection = direction;
    setConfig(config);
}

// FluentGridLayout implementation
FluentGridLayout::FluentGridLayout(QWidget* parent)
    : FluentResponsiveLayout(parent) {
    FluentResponsiveLayoutConfig config;
    config.type = FluentLayoutType::Grid;
    config.gridColumns =
        QList<FluentGridTrack>{FluentGridTrack{}, FluentGridTrack{},
                               FluentGridTrack{}};  // 3 columns default
    setConfig(config);
}

FluentGridLayout::FluentGridLayout(int rows, int columns, QWidget* parent)
    : FluentResponsiveLayout(parent) {
    Q_UNUSED(rows);  // Rows are calculated automatically based on items

    FluentResponsiveLayoutConfig config;
    config.type = FluentLayoutType::Grid;

    // Create the specified number of columns
    for (int i = 0; i < columns; ++i) {
        config.gridColumns.append(FluentGridTrack{});
    }

    setConfig(config);
}

// FluentMasonryLayout implementation
FluentMasonryLayout::FluentMasonryLayout(QWidget* parent)
    : FluentResponsiveLayout(parent) {
    FluentResponsiveLayoutConfig config;
    config.type = FluentLayoutType::Masonry;
    config.masonryColumns = 3;  // Default to 3 columns
    setConfig(config);
}

FluentMasonryLayout::FluentMasonryLayout(int columns, QWidget* parent)
    : FluentResponsiveLayout(parent) {
    FluentResponsiveLayoutConfig config;
    config.type = FluentLayoutType::Masonry;
    config.masonryColumns = columns;
    setConfig(config);
}

}  // namespace FluentQt::Core
