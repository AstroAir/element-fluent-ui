// include/FluentQt/Components/FluentTreeView.h
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <functional>

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
    Q_PROPERTY(FluentTreeSelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(FluentTreeExpandMode expandMode READ expandMode WRITE setExpandMode)
    Q_PROPERTY(bool showHeader READ showHeader WRITE setShowHeader)
    Q_PROPERTY(bool showRootDecoration READ showRootDecoration WRITE setShowRootDecoration)
    Q_PROPERTY(bool alternatingRowColors READ alternatingRowColors WRITE setAlternatingRowColors)
    Q_PROPERTY(bool sortingEnabled READ sortingEnabled WRITE setSortingEnabled)
    Q_PROPERTY(bool filteringEnabled READ filteringEnabled WRITE setFilteringEnabled)
    Q_PROPERTY(bool dragDropEnabled READ dragDropEnabled WRITE setDragDropEnabled)

public:
    explicit FluentTreeView(QWidget* parent = nullptr);

    // Selection
    FluentTreeSelectionMode selectionMode() const noexcept { return m_selectionMode; }
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

signals:
    void itemClicked(FluentTreeItem* item, int column);
    void itemDoubleClicked(FluentTreeItem* item, int column);
    void itemExpanded(FluentTreeItem* item);
    void itemCollapsed(FluentTreeItem* item);
    void itemSelectionChanged();
    void itemChanged(FluentTreeItem* item, int column);
    void lazyLoadRequested(FluentTreeItem* parent);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

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

private:
    QVBoxLayout* m_layout;
    QLineEdit* m_filterEdit;
    QTreeWidget* m_treeWidget;

    FluentTreeSelectionMode m_selectionMode{FluentTreeSelectionMode::SingleSelection};
    FluentTreeExpandMode m_expandMode{FluentTreeExpandMode::DoubleClick};
    std::vector<FluentTreeColumn> m_columns;
    
    bool m_filteringEnabled{true};
    bool m_lazyLoadingEnabled{false};
    QString m_currentFilter;
};

} // namespace FluentQt::Components