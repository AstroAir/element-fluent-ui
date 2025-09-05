// include/FluentQt/Components/FluentTreeView.h
#pragma once

#include <QAccessible>
#include <QAccessibleWidget>
#include <QCheckBox>
#include <QFocusEvent>
#include <QHeaderView>
#include <QIcon>
#include <QKeyEvent>
#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QMouseEvent>
#include <QString>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QVariant>
#include <functional>
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentTreeSelectionMode {
    NoSelection,
    SingleSelection,
    MultiSelection,
    ExtendedSelection
};

enum class FluentTreeExpandMode {
    Manual,
    SingleClick,
    DoubleClick,
    AutoExpand
};

struct FluentTreeColumn {
    QString header;
    int width{-1};
    bool sortable{true};
    bool resizable{true};
    Qt::Alignment alignment{Qt::AlignLeft | Qt::AlignVCenter};
    std::function<QVariant(QTreeWidgetItem*, int)> dataProvider;
};

class FluentTreeItem : public QTreeWidgetItem {
public:
    FluentTreeItem(QTreeWidget* parent = nullptr);
    FluentTreeItem(QTreeWidgetItem* parent = nullptr);

    // Enhanced data handling
    void setItemData(const QString& key, const QVariant& value);
    QVariant itemData(const QString& key) const;

    // Visual states
    void setExpandable(bool expandable);
    bool isExpandable() const;

    void setCheckable(bool checkable);
    bool isCheckable() const;

    void setIcon(const QIcon& icon);
    void setToolTip(const QString& tooltip);

    // Lazy loading support
    void setLazyLoading(bool lazy);
    bool isLazyLoading() const;

private:
    QVariantMap m_userData;
    bool m_expandable{true};
    bool m_checkable{false};
    bool m_lazyLoading{false};
};

class FluentTreeView : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentTreeSelectionMode selectionMode READ selectionMode WRITE
                   setSelectionMode NOTIFY selectionModeChanged)
    Q_PROPERTY(FluentTreeExpandMode expandMode READ expandMode WRITE
                   setExpandMode NOTIFY expandModeChanged)
    Q_PROPERTY(bool showHeader READ showHeader WRITE setShowHeader NOTIFY
                   showHeaderChanged)
    Q_PROPERTY(bool showRootDecoration READ showRootDecoration WRITE
                   setShowRootDecoration NOTIFY showRootDecorationChanged)
    Q_PROPERTY(bool alternatingRowColors READ alternatingRowColors WRITE
                   setAlternatingRowColors NOTIFY alternatingRowColorsChanged)
    Q_PROPERTY(bool sortingEnabled READ sortingEnabled WRITE setSortingEnabled
                   NOTIFY sortingEnabledChanged)
    Q_PROPERTY(bool filteringEnabled READ filteringEnabled WRITE
                   setFilteringEnabled NOTIFY filteringEnabledChanged)
    Q_PROPERTY(bool dragDropEnabled READ dragDropEnabled WRITE
                   setDragDropEnabled NOTIFY dragDropEnabledChanged)

    // Accessibility properties
    Q_PROPERTY(QString accessibleName READ accessibleName WRITE
                   setAccessibleName NOTIFY accessibleNameChanged)
    Q_PROPERTY(QString accessibleDescription READ accessibleDescription WRITE
                   setAccessibleDescription NOTIFY accessibleDescriptionChanged)
    Q_PROPERTY(bool keyboardNavigationEnabled READ isKeyboardNavigationEnabled
                   WRITE setKeyboardNavigationEnabled NOTIFY
                       keyboardNavigationEnabledChanged)

    // Theme variant support
    Q_PROPERTY(bool compactMode READ isCompactMode WRITE setCompactMode NOTIFY
                   compactModeChanged)
    Q_PROPERTY(bool touchMode READ isTouchMode WRITE setTouchMode NOTIFY
                   touchModeChanged)
    Q_PROPERTY(bool highContrastMode READ isHighContrastMode WRITE
                   setHighContrastMode NOTIFY highContrastModeChanged)

    // Enhanced visual properties
    Q_PROPERTY(bool revealEffectEnabled READ isRevealEffectEnabled WRITE
                   setRevealEffectEnabled NOTIFY revealEffectEnabledChanged)
    Q_PROPERTY(bool animationsEnabled READ areAnimationsEnabled WRITE
                   setAnimationsEnabled NOTIFY animationsEnabledChanged)

public:
    explicit FluentTreeView(QWidget* parent = nullptr);

    // Selection
    FluentTreeSelectionMode selectionMode() const noexcept {
        return m_selectionMode;
    }
    void setSelectionMode(FluentTreeSelectionMode mode);

    // Expansion
    FluentTreeExpandMode expandMode() const noexcept { return m_expandMode; }
    void setExpandMode(FluentTreeExpandMode mode);

    // Appearance
    bool showHeader() const;
    void setShowHeader(bool show);

    bool showRootDecoration() const;
    void setShowRootDecoration(bool show);

    bool alternatingRowColors() const;
    void setAlternatingRowColors(bool enabled);

    // Functionality
    bool sortingEnabled() const;
    void setSortingEnabled(bool enabled);

    bool filteringEnabled() const;
    void setFilteringEnabled(bool enabled);

    bool dragDropEnabled() const;
    void setDragDropEnabled(bool enabled);

    // Column management
    void setColumns(const std::vector<FluentTreeColumn>& columns);
    void addColumn(const FluentTreeColumn& column);
    void removeColumn(int index);
    int columnCount() const;

    // Item management
    FluentTreeItem* addTopLevelItem(const QString& text);
    FluentTreeItem* addChildItem(FluentTreeItem* parent, const QString& text);
    void removeItem(FluentTreeItem* item);
    void clear();

    // Selection
    QList<FluentTreeItem*> selectedItems() const;
    FluentTreeItem* currentItem() const;
    void setCurrentItem(FluentTreeItem* item);

    // Expansion
    void expandAll();
    void collapseAll();
    void expandItem(FluentTreeItem* item);
    void collapseItem(FluentTreeItem* item);

    // Filtering
    void setFilter(const QString& filter);
    void clearFilter();

    // Lazy loading
    void setLazyLoadingEnabled(bool enabled);
    bool isLazyLoadingEnabled() const;

    // Accessibility support
    QString accessibleName() const;
    void setAccessibleName(const QString& name);
    QString accessibleDescription() const;
    void setAccessibleDescription(const QString& description);
    bool isKeyboardNavigationEnabled() const;
    void setKeyboardNavigationEnabled(bool enabled);

    // Theme variant support
    bool isCompactMode() const;
    void setCompactMode(bool enabled);
    bool isTouchMode() const;
    void setTouchMode(bool enabled);
    bool isHighContrastMode() const;
    void setHighContrastMode(bool enabled);

    // Enhanced visual effects
    bool isRevealEffectEnabled() const;
    void setRevealEffectEnabled(bool enabled);
    bool areAnimationsEnabled() const;
    void setAnimationsEnabled(bool enabled);

    // Focus management
    void setFocusItem(FluentTreeItem* item);
    FluentTreeItem* focusItem() const;
    void moveFocusUp();
    void moveFocusDown();
    void moveFocusToParent();
    void moveFocusToFirstChild();

    // Advanced Virtualization System
    void setVirtualizationEnabled(bool enabled);
    bool isVirtualizationEnabled() const;

    // Virtualization configuration
    void setVirtualizationOverscan(int overscan);
    int virtualizationOverscan() const { return m_virtualizationOverscan; }

    void setVirtualizationChunkSize(int chunkSize);
    int virtualizationChunkSize() const { return m_virtualizationChunkSize; }

    // Column virtualization
    void setColumnVirtualizationEnabled(bool enabled);
    bool isColumnVirtualizationEnabled() const {
        return m_columnVirtualizationEnabled;
    }

    // Performance monitoring
    struct VirtualizationMetrics {
        int totalItems{0};
        int visibleItems{0};
        int renderedItems{0};
        int cachedItems{0};
        double renderTime{0.0};
        size_t memoryUsage{0};
    };
    VirtualizationMetrics getVirtualizationMetrics() const;

signals:
    void itemClicked(FluentTreeItem* item, int column);
    void itemDoubleClicked(FluentTreeItem* item, int column);
    void itemExpanded(FluentTreeItem* item);
    void itemCollapsed(FluentTreeItem* item);
    void itemSelectionChanged();
    void itemChanged(FluentTreeItem* item, int column);
    void lazyLoadRequested(FluentTreeItem* parent);

    // Property change signals
    void selectionModeChanged(FluentTreeSelectionMode mode);
    void expandModeChanged(FluentTreeExpandMode mode);
    void showHeaderChanged(bool show);
    void showRootDecorationChanged(bool show);
    void alternatingRowColorsChanged(bool enabled);
    void sortingEnabledChanged(bool enabled);
    void filteringEnabledChanged(bool enabled);
    void dragDropEnabledChanged(bool enabled);

    // Accessibility signals
    void accessibleNameChanged(const QString& name);
    void accessibleDescriptionChanged(const QString& description);
    void keyboardNavigationEnabledChanged(bool enabled);

    // Theme variant signals
    void compactModeChanged(bool enabled);
    void touchModeChanged(bool enabled);
    void highContrastModeChanged(bool enabled);

    // Visual effect signals
    void revealEffectEnabledChanged(bool enabled);
    void animationsEnabledChanged(bool enabled);

    // Focus signals
    void focusItemChanged(FluentTreeItem* item);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private slots:
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onItemExpanded(QTreeWidgetItem* item);
    void onFilterTextChanged(const QString& text);

private:
    void setupTreeWidget();
    void setupFilterBar();
    void updateTreeStyling();
    void filterItems(const QString& filter);
    bool itemMatchesFilter(QTreeWidgetItem* item, const QString& filter) const;

    // Accessibility helpers
    void setupAccessibility();
    void updateAccessibilityAttributes();
    void announceToScreenReader(const QString& message);

    // Keyboard navigation helpers
    void handleKeyboardNavigation(QKeyEvent* event);
    void selectItemWithKeyboard(FluentTreeItem* item);

    // Theme variant helpers
    void applyThemeVariant();
    void updateSpacingForVariant();

    // Visual effect helpers
    void paintRevealEffect(QPainter* painter, const QRect& rect);
    void paintFocusIndicator(QPainter* painter, const QRect& rect);
    void updateHoverEffects(const QPoint& mousePos);

    // Advanced virtualization helpers
    void updateVirtualizationWindow();
    void updateColumnVirtualization();
    void updateVirtualizationCache();
    void renderVirtualizedItems();
    void optimizeVirtualizationPerformance();

    // Virtualization data structures
    struct VirtualizedItem {
        QTreeWidgetItem* item{nullptr};
        QRect boundingRect;
        bool isVisible{false};
        bool isCached{false};
        QPixmap cachedPixmap;
        qint64 lastAccessTime{0};
    };

    struct VirtualizationWindow {
        int firstVisibleRow{0};
        int lastVisibleRow{0};
        int firstVisibleColumn{0};
        int lastVisibleColumn{0};
        QRect viewportRect;
    };

    // Debounce support
    void scheduleFilterApply();

private:
    QVBoxLayout* m_layout;
    QLineEdit* m_filterEdit;
    QTreeWidget* m_treeWidget;

    FluentTreeSelectionMode m_selectionMode{
        FluentTreeSelectionMode::SingleSelection};
    FluentTreeExpandMode m_expandMode{FluentTreeExpandMode::DoubleClick};
    std::vector<FluentTreeColumn> m_columns;

    bool m_filteringEnabled{true};
    bool m_lazyLoadingEnabled{false};
    QString m_currentFilter;

    // Advanced virtualization state
    bool m_virtualizationEnabled{false};
    bool m_columnVirtualizationEnabled{false};
    int m_virtualizationOverscan{10};    // rows before/after viewport
    int m_virtualizationChunkSize{100};  // items to process per chunk

    // Virtualization data
    mutable VirtualizationWindow m_virtualizationWindow;
    mutable QHash<QTreeWidgetItem*, VirtualizedItem> m_virtualizedItems;
    mutable QList<QTreeWidgetItem*> m_visibleItems;
    mutable QTimer m_virtualizationUpdateTimer;

    // Performance tracking
    mutable VirtualizationMetrics m_virtualizationMetrics;

    // Style caching and debounce timer
    QString m_cachedStyleKey;
    QString m_cachedTreeStyle;
    QString m_cachedFilterStyle;
    QTimer m_filterDebounceTimer;

    // Accessibility properties
    QString m_accessibleName;
    QString m_accessibleDescription;
    bool m_keyboardNavigationEnabled{true};
    FluentTreeItem* m_focusItem{nullptr};

    // Theme variant properties
    bool m_compactMode{false};
    bool m_touchMode{false};
    bool m_highContrastModeOverride{false};

    // Visual effect properties
    bool m_revealEffectEnabled{true};
    bool m_animationsEnabled{true};
    QPoint m_lastMousePos;
    QTimer m_hoverEffectTimer;
};

}  // namespace FluentQt::Components
