// include/FluentQt/Core/FluentResponsiveLayout.h
#pragma once

#include <QLayout>
#include <QLayoutItem>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QRect>
#include <QSize>
#include <QWidget>

#include "FluentResponsive.h"
#include "FluentResponsiveComponent.h"

namespace FluentQt::Core {

// Layout types
enum class FluentLayoutType {
    Grid,      // CSS Grid-like layout
    Flexbox,   // CSS Flexbox-like layout
    Masonry,   // Pinterest-style masonry layout
    Flow,      // Flowing text-like layout
    Stack,     // Vertical/horizontal stack
    Adaptive,  // Adapts based on content
    Custom     // Custom layout algorithm
};

// Flexbox properties
enum class FluentFlexDirection {
    Row,           // Left to right
    RowReverse,    // Right to left
    Column,        // Top to bottom
    ColumnReverse  // Bottom to top
};

enum class FluentFlexWrap {
    NoWrap,      // Single line
    Wrap,        // Multi-line
    WrapReverse  // Multi-line, reversed
};

enum class FluentJustifyContent {
    FlexStart,     // Start of container
    FlexEnd,       // End of container
    Center,        // Center of container
    SpaceBetween,  // Space between items
    SpaceAround,   // Space around items
    SpaceEvenly    // Even space distribution
};

enum class FluentAlignItems {
    FlexStart,  // Start of cross axis
    FlexEnd,    // End of cross axis
    Center,     // Center of cross axis
    Baseline,   // Baseline alignment
    Stretch     // Stretch to fill
};

// Grid properties
struct FluentGridArea {
    int rowStart{1};
    int rowEnd{-1};  // -1 = auto
    int columnStart{1};
    int columnEnd{-1};  // -1 = auto
    QString namedArea;  // Optional named grid area
};

struct FluentGridTrack {
    QString size{
        "1fr"};  // CSS-like sizing (px, %, fr, auto, min-content, max-content)
    QString minSize{"auto"};
    QString maxSize{"auto"};
    QString name;  // Optional track name
};

// Layout item configuration
struct FluentLayoutItemConfig {
    // Common properties
    QSize minSize{0, 0};
    QSize maxSize{QWIDGETSIZE_MAX, QWIDGETSIZE_MAX};
    QSize preferredSize{-1, -1};  // -1 = use widget's preferred size
    Qt::Alignment alignment{Qt::AlignCenter};
    QMargins margins{0, 0, 0, 0};
    int stretch{0};
    bool visible{true};

    // Flexbox properties
    int flexGrow{0};
    int flexShrink{1};
    QString flexBasis{"auto"};
    Qt::Alignment alignSelf{Qt::AlignCenter};
    int order{0};

    // Grid properties
    FluentGridArea gridArea;
    Qt::Alignment gridAlignment{Qt::AlignCenter};

    // Responsive properties
    QMap<FluentBreakpoint, FluentLayoutItemConfig> responsiveConfigs;
    bool enableResponsive{true};

    // Animation properties
    bool enableTransitions{true};
    int transitionDuration{200};
    QString transitionEasing{"ease-in-out"};
};

// Layout configuration
struct FluentResponsiveLayoutConfig {
    FluentLayoutType type{FluentLayoutType::Flexbox};

    // Container properties
    QMargins containerMargins{0, 0, 0, 0};
    int containerSpacing{8};
    QSize minContainerSize{0, 0};
    QSize maxContainerSize{QWIDGETSIZE_MAX, QWIDGETSIZE_MAX};

    // Flexbox configuration
    FluentFlexDirection flexDirection{FluentFlexDirection::Row};
    FluentFlexWrap flexWrap{FluentFlexWrap::Wrap};
    FluentJustifyContent justifyContent{FluentJustifyContent::FlexStart};
    FluentAlignItems alignItems{FluentAlignItems::Stretch};
    Qt::Alignment alignContent{Qt::AlignTop};
    int gap{8};
    int rowGap{8};
    int columnGap{8};

    // Grid configuration
    QList<FluentGridTrack> gridRows;
    QList<FluentGridTrack> gridColumns;
    QMap<QString, QRect> namedAreas;
    bool autoFlow{true};
    QString autoFlowDirection{"row"};
    bool dense{false};

    // Masonry configuration
    int masonryColumns{3};
    bool masonryAdaptive{true};
    int masonryMinColumnWidth{200};

    // Responsive configuration
    QMap<FluentBreakpoint, FluentResponsiveLayoutConfig> responsiveConfigs;
    bool enableResponsive{true};
    bool enableContainerQueries{false};

    // Animation configuration
    bool enableTransitions{true};
    int transitionDuration{200};
    QString transitionEasing{"ease-in-out"};
    bool respectReducedMotion{true};

    // Performance configuration
    bool enableVirtualization{false};
    int virtualizationThreshold{100};
    bool enableLazyLoading{false};
    bool enableCaching{true};
};

// Responsive layout implementation
class FluentResponsiveLayout : public QLayout {
    Q_OBJECT

public:
    explicit FluentResponsiveLayout(QWidget* parent = nullptr);
    explicit FluentResponsiveLayout(const FluentResponsiveLayoutConfig& config,
                                    QWidget* parent = nullptr);
    ~FluentResponsiveLayout() override;

    // QLayout interface
    void addItem(QLayoutItem* item) override;
    QLayoutItem* itemAt(int index) const override;
    QLayoutItem* takeAt(int index) override;
    int count() const override;
    QSize sizeHint() const override;
    QSize minimumSize() const override;
    QSize maximumSize() const override;
    void setGeometry(const QRect& rect) override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;
    Qt::Orientations expandingDirections() const override;
    void invalidate() override;

    // Configuration
    void setConfig(const FluentResponsiveLayoutConfig& config);
    FluentResponsiveLayoutConfig getConfig() const { return m_config; }
    void setConfigForBreakpoint(FluentBreakpoint breakpoint,
                                const FluentResponsiveLayoutConfig& config);

    // Item management
    void addWidget(QWidget* widget, const FluentLayoutItemConfig& config = {});
    void addWidget(QWidget* widget, int stretch,
                   Qt::Alignment alignment = Qt::Alignment());
    void insertWidget(int index, QWidget* widget,
                      const FluentLayoutItemConfig& config = {});
    void removeWidget(QWidget* widget);

    // Item configuration
    void setItemConfig(QWidget* widget, const FluentLayoutItemConfig& config);
    void setItemConfig(int index, const FluentLayoutItemConfig& config);
    FluentLayoutItemConfig getItemConfig(QWidget* widget) const;
    FluentLayoutItemConfig getItemConfig(int index) const;

    // Flexbox methods
    void setFlexDirection(FluentFlexDirection direction);
    void setFlexWrap(FluentFlexWrap wrap);
    void setJustifyContent(FluentJustifyContent justify);
    void setAlignItems(FluentAlignItems align);
    void setGap(int gap);
    void setRowGap(int gap);
    void setColumnGap(int gap);

    // Grid methods
    void setGridTemplate(const QList<FluentGridTrack>& rows,
                         const QList<FluentGridTrack>& columns);
    void setGridRows(const QList<FluentGridTrack>& rows);
    void setGridColumns(const QList<FluentGridTrack>& columns);
    void setNamedArea(const QString& name, const QRect& area);
    void setItemGridArea(QWidget* widget, const FluentGridArea& area);
    void setItemGridArea(QWidget* widget, int rowStart, int columnStart,
                         int rowSpan = 1, int columnSpan = 1);

    // Masonry methods
    void setMasonryColumns(int columns);
    void setMasonryMinColumnWidth(int width);
    void setMasonryAdaptive(bool adaptive);

    // Responsive methods
    void enableResponsive(bool enabled = true);
    void enableContainerQueries(bool enabled = true);
    void updateForBreakpoint(FluentBreakpoint breakpoint);
    void updateForContainerSize(const QSize& size);

    // Animation methods
    void enableTransitions(bool enabled = true);
    void setTransitionDuration(int milliseconds);
    void animateToLayout();
    void animateItemToPosition(QWidget* widget, const QRect& targetGeometry);

    // Utility methods
    QRect calculateItemGeometry(int index, const QRect& containerRect) const;
    QSize calculateLayoutSize(const QSize& containerSize) const;
    int getOptimalColumnCount(int containerWidth) const;
    QList<QRect> calculateMasonryLayout(const QRect& containerRect) const;

    // Performance methods
    void enableVirtualization(bool enabled = true);
    void setVirtualizationThreshold(int threshold);
    void enableLazyLoading(bool enabled = true);
    void optimizeLayout();

signals:
    void layoutChanged();
    void breakpointChanged(FluentBreakpoint breakpoint);
    void itemAdded(QWidget* widget);
    void itemRemoved(QWidget* widget);
    void animationStarted();
    void animationFinished();

private slots:
    void onBreakpointChanged(FluentBreakpoint breakpoint);
    void onContainerSizeChanged();
    void onAnimationFinished();

private:
    // Layout calculation methods
    void calculateFlexboxLayout(const QRect& rect);
    void calculateGridLayout(const QRect& rect);
    void calculateMasonryLayout(const QRect& rect);
    void calculateStackLayout(const QRect& rect);
    void calculateFlowLayout(const QRect& rect);
    void calculateAdaptiveLayout(const QRect& rect);

    // Flexbox helpers
    QSize calculateFlexboxSizeHint() const;
    void distributeFlexSpace(const QRect& containerRect);
    void alignFlexItems(const QRect& containerRect);

    // Grid helpers
    QSize calculateGridSizeHint() const;
    QList<int> resolveGridTracks(const QList<FluentGridTrack>& tracks,
                                 int availableSpace) const;
    int parseGridSize(const QString& size, int availableSpace) const;

    // Masonry helpers
    QSize calculateMasonrySizeHint() const;
    QList<int> calculateMasonryColumnHeights(const QRect& containerRect) const;
    int findShortestColumn(const QList<int>& heights) const;

    // Animation helpers
    void setupLayoutAnimation();
    void animateItemsToPositions(const QMap<QWidget*, QRect>& targetPositions);

    // Responsive helpers
    FluentResponsiveLayoutConfig getConfigForBreakpoint(
        FluentBreakpoint breakpoint) const;
    FluentLayoutItemConfig getItemConfigForBreakpoint(
        int index, FluentBreakpoint breakpoint) const;
    void updateResponsiveConfiguration();

    // Utility helpers
    QRect applyMargins(const QRect& rect, const QMargins& margins) const;
    QSize applyConstraints(const QSize& size, const QSize& minSize,
                           const QSize& maxSize) const;
    bool shouldShowItem(int index) const;

private:
    FluentResponsiveLayoutConfig m_config;
    QList<QLayoutItem*> m_items;
    QList<FluentLayoutItemConfig> m_itemConfigs;

    // Layout state
    QRect m_lastGeometry;
    FluentBreakpoint m_currentBreakpoint{FluentBreakpoint::Medium};
    bool m_layoutValid{false};

    // Animation
    QParallelAnimationGroup* m_animationGroup;
    QMap<QWidget*, QPropertyAnimation*> m_itemAnimations;
    bool m_animating{false};

    // Performance
    bool m_virtualizationEnabled{false};
    int m_virtualizationThreshold{100};
    QList<bool> m_itemVisibility;

    // Caching
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};

    // Thread safety
    mutable QMutex m_layoutMutex;
};

// Convenience layout classes
class FluentFlexLayout : public FluentResponsiveLayout {
    Q_OBJECT
public:
    explicit FluentFlexLayout(QWidget* parent = nullptr);
    explicit FluentFlexLayout(FluentFlexDirection direction,
                              QWidget* parent = nullptr);
};

class FluentGridLayout : public FluentResponsiveLayout {
    Q_OBJECT
public:
    explicit FluentGridLayout(QWidget* parent = nullptr);
    explicit FluentGridLayout(int rows, int columns, QWidget* parent = nullptr);
};

class FluentMasonryLayout : public FluentResponsiveLayout {
    Q_OBJECT
public:
    explicit FluentMasonryLayout(QWidget* parent = nullptr);
    explicit FluentMasonryLayout(int columns, QWidget* parent = nullptr);
};

}  // namespace FluentQt::Core
