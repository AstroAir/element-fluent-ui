// include/FluentQt/Components/FluentGrid.h
#pragma once

#include <QList>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QWidget>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentGridFlow {
    Row,           // Items flow in rows (left to right, then wrap)
    Column,        // Items flow in columns (top to bottom, then wrap)
    RowReverse,    // Items flow in rows (right to left, then wrap)
    ColumnReverse  // Items flow in columns (bottom to top, then wrap)
};

enum class FluentGridAlignment {
    Start,         // Align to start of container
    Center,        // Center in container
    End,           // Align to end of container
    Stretch,       // Stretch to fill container
    SpaceBetween,  // Space evenly with no space at edges
    SpaceAround,   // Space evenly with half space at edges
    SpaceEvenly    // Space evenly with equal space everywhere
};

enum class FluentGridItemAlignment {
    Auto,    // Use grid's alignment
    Start,   // Align to start
    Center,  // Center
    End,     // Align to end
    Stretch  // Stretch to fill
};

struct FluentGridItem {
    QWidget* widget{nullptr};
    int row{-1};        // Explicit row (-1 for auto)
    int column{-1};     // Explicit column (-1 for auto)
    int rowSpan{1};     // Number of rows to span
    int columnSpan{1};  // Number of columns to span
    FluentGridItemAlignment alignSelf{FluentGridItemAlignment::Auto};
    QSize minSize;       // Minimum size
    QSize maxSize;       // Maximum size
    int order{0};        // Display order (for sorting)
    bool visible{true};  // Visibility

    FluentGridItem() = default;
    FluentGridItem(QWidget* w) : widget(w) {}
    FluentGridItem(QWidget* w, int r, int c) : widget(w), row(r), column(c) {}
};

class FluentGrid : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
    Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
    Q_PROPERTY(FluentGridFlow flow READ flow WRITE setFlow NOTIFY flowChanged)
    Q_PROPERTY(FluentGridAlignment alignItems READ alignItems WRITE
                   setAlignItems NOTIFY alignItemsChanged)
    Q_PROPERTY(FluentGridAlignment justifyItems READ justifyItems WRITE
                   setJustifyItems NOTIFY justifyItemsChanged)
    Q_PROPERTY(FluentGridAlignment alignContent READ alignContent WRITE
                   setAlignContent NOTIFY alignContentChanged)
    Q_PROPERTY(FluentGridAlignment justifyContent READ justifyContent WRITE
                   setJustifyContent NOTIFY justifyContentChanged)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(int rowSpacing READ rowSpacing WRITE setRowSpacing NOTIFY
                   rowSpacingChanged)
    Q_PROPERTY(int columnSpacing READ columnSpacing WRITE setColumnSpacing
                   NOTIFY columnSpacingChanged)
    Q_PROPERTY(
        QMargins padding READ padding WRITE setPadding NOTIFY paddingChanged)
    Q_PROPERTY(bool autoColumns READ autoColumns WRITE setAutoColumns NOTIFY
                   autoColumnsChanged)
    Q_PROPERTY(
        bool autoRows READ autoRows WRITE setAutoRows NOTIFY autoRowsChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)

public:
    explicit FluentGrid(QWidget* parent = nullptr);
    explicit FluentGrid(int columns, QWidget* parent = nullptr);
    explicit FluentGrid(int columns, int rows, QWidget* parent = nullptr);
    ~FluentGrid() override;

    // Grid configuration
    int columns() const;
    void setColumns(int columns);

    int rows() const;
    void setRows(int rows);

    FluentGridFlow flow() const;
    void setFlow(FluentGridFlow flow);

    // Alignment
    FluentGridAlignment alignItems() const;
    void setAlignItems(FluentGridAlignment alignment);

    FluentGridAlignment justifyItems() const;
    void setJustifyItems(FluentGridAlignment alignment);

    FluentGridAlignment alignContent() const;
    void setAlignContent(FluentGridAlignment alignment);

    FluentGridAlignment justifyContent() const;
    void setJustifyContent(FluentGridAlignment alignment);

    // Spacing
    int spacing() const;
    void setSpacing(int spacing);

    int rowSpacing() const;
    void setRowSpacing(int spacing);

    int columnSpacing() const;
    void setColumnSpacing(int spacing);

    QMargins padding() const;
    void setPadding(const QMargins& padding);
    void setPadding(int padding);

    // Auto sizing
    bool autoColumns() const;
    void setAutoColumns(bool auto_);

    bool autoRows() const;
    void setAutoRows(bool auto_);

    // Animation
    bool isAnimated() const;
    void setAnimated(bool animated);

    // Item management
    void addItem(QWidget* widget);
    void addItem(QWidget* widget, int row, int column);
    void addItem(QWidget* widget, int row, int column, int rowSpan,
                 int columnSpan);
    void addItem(const FluentGridItem& item);

    void insertItem(int index, QWidget* widget);
    void insertItem(int index, const FluentGridItem& item);

    void removeItem(QWidget* widget);
    void removeItemAt(int index);
    void clear();

    // Item access
    FluentGridItem* itemAt(int index);
    const FluentGridItem* itemAt(int index) const;
    FluentGridItem* itemForWidget(QWidget* widget);
    const FluentGridItem* itemForWidget(QWidget* widget) const;

    int itemCount() const;
    QList<FluentGridItem>& items();
    const QList<FluentGridItem>& items() const;

    // Grid information
    int actualColumns() const;
    int actualRows() const;
    QSize gridSize() const;
    QRect itemRect(int index) const;
    QRect itemRect(const FluentGridItem& item) const;

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void relayout();
    void animateLayout();

signals:
    void columnsChanged(int columns);
    void rowsChanged(int rows);
    void flowChanged(FluentGridFlow flow);
    void alignItemsChanged(FluentGridAlignment alignment);
    void justifyItemsChanged(FluentGridAlignment alignment);
    void alignContentChanged(FluentGridAlignment alignment);
    void justifyContentChanged(FluentGridAlignment alignment);
    void spacingChanged(int spacing);
    void rowSpacingChanged(int spacing);
    void columnSpacingChanged(int spacing);
    void paddingChanged(const QMargins& padding);
    void autoColumnsChanged(bool auto_);
    void autoRowsChanged(bool auto_);
    void animatedChanged(bool animated);
    void itemAdded(int index);
    void itemRemoved(int index);
    void layoutChanged();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void childEvent(QChildEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private slots:
    void onItemDestroyed(QObject* object);
    void onLayoutAnimationFinished();

private:
    void setupAnimations();
    void calculateLayout();
    void applyLayout();
    void animateToLayout();

    void calculateGridDimensions();
    void calculateItemPositions();
    void calculateItemSizes();
    void applyAlignment();

    QSize calculateItemSize(const FluentGridItem& item,
                            const QSize& availableSize) const;
    QPoint calculateItemPosition(const FluentGridItem& item, int row,
                                 int column) const;
    QRect calculateAlignedRect(const QRect& rect, const QSize& size,
                               FluentGridAlignment alignment) const;

    // Enhanced alignment methods
    void applyContentAlignment();
    void applyItemAlignment();
    void applyJustification();
    QPoint calculateContentOffset(const QRect& container, const QSize& content,
                                  FluentGridAlignment alignment) const;
    QRect applyItemSpecificAlignment(const QRect& itemRect,
                                     const QSize& cellSize,
                                     FluentGridAlignment alignment) const;
    void applySpaceDistribution();
    void applyItemJustification();

    // Enhanced animation methods
    void cleanupAnimations();
    void createItemAnimations();
    void startStaggeredAnimations();
    QPropertyAnimation* createFluentAnimation(QWidget* widget,
                                              const QRect& from,
                                              const QRect& to, int delay);
    int getFluentAnimationDuration() const;
    QEasingCurve getFluentEasingCurve() const;
    bool shouldRespectReducedMotion() const;

    int getEffectiveColumns() const;
    int getEffectiveRows() const;

    void updateItemVisibility();
    void sortItemsByOrder();

    // FluentUI Design System Integration
    void applyFluentSpacingTokens();
    void validateFluentSpacing();
    int getFluentSpacingToken(const QString& tokenName) const;
    void applyFluentAlignment();
    bool isFluentCompliant() const;

    // Responsive design methods
    void updateResponsiveLayout();
    int getResponsiveColumns() const;
    bool shouldUseResponsiveLayout() const;
    void applyResponsiveSpacing();

    // User interaction and focus management
    void setupFocusChain();
    void handleKeyboardNavigation(QKeyEvent* event);
    void focusNextItem();
    void focusPreviousItem();
    int getCurrentFocusIndex() const;

    // Accessibility support
    void setupAccessibility();
    void updateAccessibilityInfo();
    void announceLayoutChange();

    // Performance optimization methods
    bool canUseCachedLayout() const;
    void applyCachedLayout();
    void cacheCurrentLayout();
    void invalidateLayoutCache();

    // FluentUI compliance validation
    void validateFluentUICompliance();
    bool validateSpacingCompliance() const;
    bool validateAlignmentCompliance() const;
    bool validateAccessibilityCompliance() const;

private:
    // Grid configuration
    int m_columns{-1};  // -1 for auto
    int m_rows{-1};     // -1 for auto
    FluentGridFlow m_flow{FluentGridFlow::Row};

    // Alignment
    FluentGridAlignment m_alignItems{FluentGridAlignment::Stretch};
    FluentGridAlignment m_justifyItems{FluentGridAlignment::Stretch};
    FluentGridAlignment m_alignContent{FluentGridAlignment::Start};
    FluentGridAlignment m_justifyContent{FluentGridAlignment::Start};

    // Spacing
    int m_rowSpacing{8};
    int m_columnSpacing{8};
    QMargins m_padding{0, 0, 0, 0};

    // Auto sizing
    bool m_autoColumns{true};
    bool m_autoRows{true};
    bool m_animated{true};

    // Items and layout
    QList<FluentGridItem> m_items;
    QList<QRect> m_itemRects;  // Calculated positions
    QSize m_gridSize;          // Actual grid dimensions
    int m_actualColumns{0};
    int m_actualRows{0};

    // Animation
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QList<QPropertyAnimation*> m_itemAnimations;
    bool m_layoutInProgress{false};

    // Layout cache
    bool m_layoutDirty{true};
    QSize m_lastSize;
};

}  // namespace FluentQt::Components
