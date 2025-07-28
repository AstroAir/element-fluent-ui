# FluentTreeView Component

The `FluentTreeView` component is an enhanced tree widget that implements Microsoft's Fluent Design principles. It provides hierarchical data display with smooth animations, drag-and-drop support, and comprehensive customization options.

## Overview

FluentTreeView offers:
- **Hierarchical data display** with expandable nodes
- **Multiple selection modes** (Single, Multiple, Extended)
- **Drag and drop** support for reordering
- **Custom item rendering** with icons and rich content
- **Search and filtering** capabilities
- **Smooth animations** for expand/collapse
- **Theme integration** with automatic styling
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Tree View

```cpp
#include "FluentQt/Components/FluentTreeView.h"

// Create a basic tree view
auto* treeView = new FluentTreeView;

// Create model and populate data
auto* model = new QStandardItemModel;
model->setHorizontalHeaderLabels({"Name", "Type", "Size"});

// Add root items
auto* documentsItem = new QStandardItem("Documents");
auto* picturesItem = new QStandardItem("Pictures");
auto* musicItem = new QStandardItem("Music");

model->appendRow({documentsItem, new QStandardItem("Folder"), new QStandardItem("")});
model->appendRow({picturesItem, new QStandardItem("Folder"), new QStandardItem("")});
model->appendRow({musicItem, new QStandardItem("Folder"), new QStandardItem("")});

// Add child items
auto* docFile1 = new QStandardItem("Report.docx");
auto* docFile2 = new QStandardItem("Presentation.pptx");
documentsItem->appendRow({docFile1, new QStandardItem("Document"), new QStandardItem("2.5 MB")});
documentsItem->appendRow({docFile2, new QStandardItem("Presentation"), new QStandardItem("5.1 MB")});

// Set model
treeView->setModel(model);

// Handle selection changes
connect(treeView->selectionModel(), &QItemSelectionModel::currentChanged,
        [=](const QModelIndex& current, const QModelIndex& previous) {
    if (current.isValid()) {
        QString itemName = model->itemFromIndex(current)->text();
        qDebug() << "Selected:" << itemName;
    }
});
```

### Tree View with Icons

```cpp
auto* treeView = new FluentTreeView;
auto* model = new QStandardItemModel;

// Create items with icons
auto* folderItem = new QStandardItem(QIcon(":/icons/folder"), "Projects");
auto* fileItem1 = new QStandardItem(QIcon(":/icons/cpp"), "main.cpp");
auto* fileItem2 = new QStandardItem(QIcon(":/icons/header"), "header.h");

folderItem->appendRow(fileItem1);
folderItem->appendRow(fileItem2);
model->appendRow(folderItem);

treeView->setModel(model);

// Expand all items by default
treeView->expandAll();
```

## Selection Modes

### Different Selection Types

```cpp
auto* treeView = new FluentTreeView;

// Single selection (default)
treeView->setSelectionMode(QAbstractItemView::SingleSelection);

// Multiple selection with Ctrl+Click
treeView->setSelectionMode(QAbstractItemView::MultiSelection);

// Extended selection with Shift+Click for ranges
treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

// No selection
treeView->setSelectionMode(QAbstractItemView::NoSelection);

// Handle multiple selections
connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
        [=](const QItemSelection& selected, const QItemSelection& deselected) {
    QModelIndexList selectedIndexes = treeView->selectionModel()->selectedIndexes();
    qDebug() << "Selected" << selectedIndexes.size() << "items";
});
```

### Custom Selection Behavior

```cpp
auto* treeView = new FluentTreeView;

// Enable selection of entire rows
treeView->setSelectionBehavior(QAbstractItemView::SelectRows);

// Enable selection of individual items
treeView->setSelectionBehavior(QAbstractItemView::SelectItems);

// Custom selection logic
connect(treeView, &FluentTreeView::itemSelectionChanged, [=]() {
    QList<QStandardItem*> selectedItems = treeView->selectedItems();
    
    // Custom logic for selection
    for (auto* item : selectedItems) {
        if (item->hasChildren()) {
            // Automatically select all children when parent is selected
            selectAllChildren(item);
        }
    }
});
```

## Drag and Drop

### Enable Drag and Drop

```cpp
auto* treeView = new FluentTreeView;

// Enable drag and drop
treeView->setDragDropMode(QAbstractItemView::DragDrop);
treeView->setDefaultDropAction(Qt::MoveAction);
treeView->setDragEnabled(true);
treeView->setAcceptDrops(true);

// Enable internal moves
treeView->setDragDropOverwriteMode(false);

// Handle drop events
connect(treeView, &FluentTreeView::itemDropped, [=](const QModelIndex& index, const QMimeData* data) {
    qDebug() << "Item dropped at:" << index.data().toString();
});
```

### Custom Drag and Drop

```cpp
class CustomTreeModel : public QStandardItemModel
{
public:
    Qt::ItemFlags flags(const QModelIndex& index) const override {
        Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);
        
        if (index.isValid()) {
            // Allow dragging for all items
            defaultFlags |= Qt::ItemIsDragEnabled;
            
            // Only allow dropping on folder items
            QStandardItem* item = itemFromIndex(index);
            if (item && item->data(Qt::UserRole).toString() == "folder") {
                defaultFlags |= Qt::ItemIsDropEnabled;
            }
        } else {
            // Allow dropping on empty space (root)
            defaultFlags |= Qt::ItemIsDropEnabled;
        }
        
        return defaultFlags;
    }
    
    QMimeData* mimeData(const QModelIndexList& indexes) const override {
        auto* mimeData = new QMimeData;
        QByteArray encodedData;
        QDataStream stream(&encodedData, QIODevice::WriteOnly);
        
        for (const QModelIndex& index : indexes) {
            if (index.isValid()) {
                QString text = data(index, Qt::DisplayRole).toString();
                stream << text;
            }
        }
        
        mimeData->setData("application/x-treeviewitem", encodedData);
        return mimeData;
    }
    
    bool dropMimeData(const QMimeData* data, Qt::DropAction action,
                      int row, int column, const QModelIndex& parent) override {
        if (!data->hasFormat("application/x-treeviewitem")) {
            return false;
        }
        
        // Custom drop handling logic
        QByteArray encodedData = data->data("application/x-treeviewitem");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        
        while (!stream.atEnd()) {
            QString text;
            stream >> text;
            
            // Create new item at drop location
            auto* newItem = new QStandardItem(text);
            
            if (parent.isValid()) {
                itemFromIndex(parent)->appendRow(newItem);
            } else {
                appendRow(newItem);
            }
        }
        
        return true;
    }
};

auto* treeView = new FluentTreeView;
auto* customModel = new CustomTreeModel;
treeView->setModel(customModel);
```

## Animations

### Expand/Collapse Animations

```cpp
auto* treeView = new FluentTreeView;

// Enable smooth expand/collapse animations
treeView->setAnimated(true);
treeView->setExpandAnimationDuration(250);
treeView->setCollapseAnimationDuration(200);
treeView->setAnimationEasing(FluentEasing::CubicOut);

// Custom expand animation
connect(treeView, &FluentTreeView::itemExpanded, [=](const QModelIndex& index) {
    auto* animator = new FluentAnimator(this);
    QWidget* itemWidget = treeView->indexWidget(index);
    if (itemWidget) {
        animator->fadeIn(itemWidget, 200);
    }
});

// Custom collapse animation
connect(treeView, &FluentTreeView::itemCollapsed, [=](const QModelIndex& index) {
    auto* animator = new FluentAnimator(this);
    QWidget* itemWidget = treeView->indexWidget(index);
    if (itemWidget) {
        animator->fadeOut(itemWidget, 150);
    }
});
```

### Hover Effects

```cpp
auto* treeView = new FluentTreeView;

// Enable hover animations
treeView->setHoverAnimationEnabled(true);
treeView->setHoverAnimationDuration(150);

// Custom hover effects
connect(treeView, &FluentTreeView::itemHovered, [=](const QModelIndex& index, bool isHovered) {
    auto* animator = new FluentAnimator(this);
    QRect itemRect = treeView->visualRect(index);
    
    if (isHovered) {
        // Highlight effect
        animator->colorTransition(treeView->viewport(), "background-color",
                                QColor(240, 240, 240), QColor(230, 240, 250), 150);
    } else {
        // Remove highlight
        animator->colorTransition(treeView->viewport(), "background-color",
                                QColor(230, 240, 250), QColor(240, 240, 240), 150);
    }
});
```

## Search and Filtering

### Built-in Search

```cpp
auto* treeView = new FluentTreeView;

// Enable search functionality
treeView->setSearchEnabled(true);
treeView->setSearchPlaceholder("Search items...");
treeView->setSearchMode(FluentTreeSearchMode::Incremental);

// Configure search behavior
treeView->setSearchCaseSensitive(false);
treeView->setSearchColumns({0, 1});  // Search in first two columns
treeView->setHighlightSearchResults(true);

// Handle search results
connect(treeView, &FluentTreeView::searchResultsChanged, [=](int resultCount) {
    qDebug() << "Found" << resultCount << "search results";
});
```

### Custom Filtering

```cpp
class CustomFilterProxyModel : public QSortFilterProxyModel
{
public:
    void setFilterText(const QString& text) {
        m_filterText = text.toLower();
        invalidateFilter();
    }
    
    void setFilterColumn(int column) {
        m_filterColumn = column;
        invalidateFilter();
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override {
        if (m_filterText.isEmpty()) {
            return true;
        }
        
        QModelIndex index = sourceModel()->index(sourceRow, m_filterColumn, sourceParent);
        QString itemText = sourceModel()->data(index, Qt::DisplayRole).toString().toLower();
        
        // Check if current item matches
        if (itemText.contains(m_filterText)) {
            return true;
        }
        
        // Check if any child matches (show parent if child matches)
        return hasMatchingChild(index);
    }

private:
    bool hasMatchingChild(const QModelIndex& parent) const {
        int childCount = sourceModel()->rowCount(parent);
        for (int i = 0; i < childCount; ++i) {
            QModelIndex child = sourceModel()->index(i, m_filterColumn, parent);
            QString childText = sourceModel()->data(child, Qt::DisplayRole).toString().toLower();
            
            if (childText.contains(m_filterText) || hasMatchingChild(child)) {
                return true;
            }
        }
        return false;
    }

private:
    QString m_filterText;
    int m_filterColumn{0};
};

auto* treeView = new FluentTreeView;
auto* sourceModel = new QStandardItemModel;
auto* filterModel = new CustomFilterProxyModel;

filterModel->setSourceModel(sourceModel);
treeView->setModel(filterModel);

// Connect search input to filter
auto* searchInput = new FluentTextInput;
searchInput->setPlaceholderText("Filter tree items...");
connect(searchInput, &FluentTextInput::textChanged, [=](const QString& text) {
    filterModel->setFilterText(text);
    if (!text.isEmpty()) {
        treeView->expandAll();  // Expand all to show search results
    }
});
```

## Custom Item Rendering

### Custom Item Delegate

```cpp
class CustomTreeItemDelegate : public QStyledItemDelegate
{
public:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        
        painter->save();
        
        // Custom background
        if (opt.state & QStyle::State_Selected) {
            painter->fillRect(opt.rect, QColor(0, 120, 215, 100));
        } else if (opt.state & QStyle::State_MouseOver) {
            painter->fillRect(opt.rect, QColor(0, 120, 215, 50));
        }
        
        // Draw icon
        QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
        if (!icon.isNull()) {
            QRect iconRect(opt.rect.left() + 4, opt.rect.top() + 2, 20, 20);
            icon.paint(painter, iconRect);
        }
        
        // Draw text
        QString text = index.data(Qt::DisplayRole).toString();
        QRect textRect(opt.rect.left() + 28, opt.rect.top(), opt.rect.width() - 32, opt.rect.height());
        
        QFont font = opt.font;
        if (index.data(Qt::UserRole + 1).toBool()) {  // Bold for special items
            font.setBold(true);
        }
        painter->setFont(font);
        
        painter->setPen(opt.state & QStyle::State_Selected ? Qt::white : Qt::black);
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
        
        // Draw additional info
        QString additionalInfo = index.data(Qt::UserRole + 2).toString();
        if (!additionalInfo.isEmpty()) {
            QFont smallFont = font;
            smallFont.setPointSize(font.pointSize() - 1);
            painter->setFont(smallFont);
            painter->setPen(QColor(128, 128, 128));
            
            QRect infoRect(textRect.left(), textRect.bottom() - 15, textRect.width(), 12);
            painter->drawText(infoRect, Qt::AlignLeft | Qt::AlignVCenter, additionalInfo);
        }
        
        painter->restore();
    }
    
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        Q_UNUSED(option)
        
        QString additionalInfo = index.data(Qt::UserRole + 2).toString();
        int height = additionalInfo.isEmpty() ? 24 : 36;
        
        return QSize(200, height);
    }
};

auto* treeView = new FluentTreeView;
treeView->setItemDelegate(new CustomTreeItemDelegate);
```

### Rich Content Items

```cpp
auto* treeView = new FluentTreeView;

// Create custom widget for tree item
auto createCustomItemWidget = [](const QString& title, const QString& subtitle, const QIcon& icon, int progress = -1) {
    auto* widget = new QWidget;
    auto* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(4, 2, 4, 2);
    
    // Icon
    auto* iconLabel = new QLabel;
    iconLabel->setPixmap(icon.pixmap(24, 24));
    
    // Text content
    auto* textWidget = new QWidget;
    auto* textLayout = new QVBoxLayout(textWidget);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(1);
    
    auto* titleLabel = new QLabel(title);
    titleLabel->setFont(QFont("Segoe UI", 9, QFont::Bold));
    
    auto* subtitleLabel = new QLabel(subtitle);
    subtitleLabel->setFont(QFont("Segoe UI", 8));
    subtitleLabel->setStyleSheet("color: #666;");
    
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(subtitleLabel);
    
    layout->addWidget(iconLabel);
    layout->addWidget(textWidget);
    
    // Progress bar (optional)
    if (progress >= 0) {
        auto* progressBar = new FluentProgressBar;
        progressBar->setProgressStyle(FluentProgressStyle::Linear);
        progressBar->setHeight(4);
        progressBar->setValue(progress);
        progressBar->setMaximumWidth(60);
        
        layout->addWidget(progressBar);
    }
    
    layout->addStretch();
    
    return widget;
};

// Add items with custom widgets
auto* model = new QStandardItemModel;
auto* projectItem = new QStandardItem("Project Alpha");
model->appendRow(projectItem);

QModelIndex projectIndex = model->indexFromItem(projectItem);
auto* projectWidget = createCustomItemWidget("Project Alpha", "In Progress", QIcon(":/icons/project"), 75);
treeView->setIndexWidget(projectIndex, projectWidget);
```

## File System Browser Example

### Complete File Browser

```cpp
class FileBrowserWidget : public QWidget
{
public:
    FileBrowserWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupModel();
        setupConnections();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Toolbar
        auto* toolbar = new QHBoxLayout;
        
        m_backButton = new FluentButton("Back");
        m_backButton->setIcon(QIcon(":/icons/back"));
        m_backButton->setEnabled(false);
        
        m_forwardButton = new FluentButton("Forward");
        m_forwardButton->setIcon(QIcon(":/icons/forward"));
        m_forwardButton->setEnabled(false);
        
        m_upButton = new FluentButton("Up");
        m_upButton->setIcon(QIcon(":/icons/up"));
        
        m_pathEdit = new FluentTextInput;
        m_pathEdit->setPlaceholderText("Enter path...");
        
        toolbar->addWidget(m_backButton);
        toolbar->addWidget(m_forwardButton);
        toolbar->addWidget(m_upButton);
        toolbar->addWidget(m_pathEdit);
        
        // Tree view
        m_treeView = new FluentTreeView;
        m_treeView->setAnimated(true);
        m_treeView->setDragDropMode(QAbstractItemView::DragDrop);
        m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        
        layout->addLayout(toolbar);
        layout->addWidget(m_treeView);
    }
    
    void setupModel() {
        m_fileSystemModel = new QFileSystemModel;
        m_fileSystemModel->setRootPath(QDir::homePath());
        
        // Set custom icons
        m_fileSystemModel->setIconProvider(new CustomFileIconProvider);
        
        m_treeView->setModel(m_fileSystemModel);
        m_treeView->setRootIndex(m_fileSystemModel->index(QDir::homePath()));
        
        // Hide size and date columns for cleaner look
        m_treeView->hideColumn(1);
        m_treeView->hideColumn(2);
        m_treeView->hideColumn(3);
    }
    
    void setupConnections() {
        // Navigation
        connect(m_backButton, &FluentButton::clicked, this, &FileBrowserWidget::navigateBack);
        connect(m_forwardButton, &FluentButton::clicked, this, &FileBrowserWidget::navigateForward);
        connect(m_upButton, &FluentButton::clicked, this, &FileBrowserWidget::navigateUp);
        
        // Path editing
        connect(m_pathEdit, &FluentTextInput::returnPressed, [=]() {
            QString path = m_pathEdit->text();
            if (QDir(path).exists()) {
                navigateToPath(path);
            }
        });
        
        // Double-click to navigate
        connect(m_treeView, &FluentTreeView::doubleClicked, [=](const QModelIndex& index) {
            if (m_fileSystemModel->isDir(index)) {
                QString path = m_fileSystemModel->filePath(index);
                navigateToPath(path);
            } else {
                // Open file
                QString filePath = m_fileSystemModel->filePath(index);
                QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
            }
        });
        
        // Context menu
        m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_treeView, &FluentTreeView::customContextMenuRequested, 
                this, &FileBrowserWidget::showContextMenu);
    }
    
    void navigateToPath(const QString& path) {
        if (QDir(path).exists()) {
            m_navigationHistory.append(m_currentPath);
            m_currentPath = path;
            m_pathEdit->setText(path);
            m_treeView->setRootIndex(m_fileSystemModel->index(path));
            updateNavigationButtons();
        }
    }
    
    void showContextMenu(const QPoint& position) {
        QModelIndex index = m_treeView->indexAt(position);
        
        auto* menu = new QMenu(this);
        
        if (index.isValid()) {
            QString filePath = m_fileSystemModel->filePath(index);
            bool isDir = m_fileSystemModel->isDir(index);
            
            if (isDir) {
                menu->addAction("Open", [=]() {
                    navigateToPath(filePath);
                });
            } else {
                menu->addAction("Open", [=]() {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
                });
            }
            
            menu->addSeparator();
            menu->addAction("Copy", [=]() {
                // Copy file path to clipboard
                QApplication::clipboard()->setText(filePath);
            });
            
            menu->addAction("Delete", [=]() {
                // Delete file/folder with confirmation
                auto reply = QMessageBox::question(this, "Delete", 
                    QString("Are you sure you want to delete '%1'?").arg(QFileInfo(filePath).fileName()));
                if (reply == QMessageBox::Yes) {
                    if (isDir) {
                        QDir().rmdir(filePath);
                    } else {
                        QFile::remove(filePath);
                    }
                }
            });
        }
        
        menu->addSeparator();
        menu->addAction("Refresh", [=]() {
            m_fileSystemModel->refresh();
        });
        
        menu->exec(m_treeView->mapToGlobal(position));
    }

private:
    FluentTreeView* m_treeView{nullptr};
    QFileSystemModel* m_fileSystemModel{nullptr};
    FluentButton* m_backButton{nullptr};
    FluentButton* m_forwardButton{nullptr};
    FluentButton* m_upButton{nullptr};
    FluentTextInput* m_pathEdit{nullptr};
    
    QString m_currentPath;
    QStringList m_navigationHistory;
    int m_historyIndex{-1};
};
```

## Accessibility

FluentTreeView provides comprehensive accessibility support:

```cpp
auto* treeView = new FluentTreeView;

// Accessible properties (automatically set)
treeView->setAccessibleName("File tree");
treeView->setAccessibleDescription("Navigate through files and folders");

// Keyboard navigation (automatic)
// - Arrow keys to navigate items
// - Enter to expand/collapse or activate
// - Space to select items
// - Ctrl+A to select all
// - Delete to remove items (if enabled)

// Screen reader support
treeView->setAccessibleRole(QAccessible::Tree);

// Custom accessibility for items
connect(treeView, &FluentTreeView::currentItemChanged, [=](QStandardItem* current, QStandardItem* previous) {
    if (current) {
        QString announcement = QString("Selected: %1").arg(current->text());
        if (current->hasChildren()) {
            announcement += QString(", %1 children").arg(current->rowCount());
        }
        treeView->announceToScreenReader(announcement);
    }
});
```

## Best Practices

### Tree Design Guidelines
1. **Use clear hierarchies** - Make parent-child relationships obvious
2. **Provide meaningful icons** - Help users identify item types
3. **Enable search/filtering** - Help users find items in large trees
4. **Support keyboard navigation** - Ensure accessibility
5. **Show loading states** - For dynamically loaded content

### Performance Considerations
- Use lazy loading for large hierarchies
- Implement virtual scrolling for very large trees
- Cache expanded states
- Optimize custom delegates for smooth scrolling

### Accessibility Guidelines
- Support keyboard navigation
- Provide screen reader announcements
- Use sufficient color contrast
- Include meaningful item labels
- Test with assistive technologies

## Signals Reference

```cpp
// Selection signals
void currentItemChanged(QStandardItem* current, QStandardItem* previous);
void itemSelectionChanged();
void itemClicked(QStandardItem* item);
void itemDoubleClicked(QStandardItem* item);

// Expansion signals
void itemExpanded(const QModelIndex& index);
void itemCollapsed(const QModelIndex& index);

// Drag and drop signals
void itemDropped(const QModelIndex& index, const QMimeData* data);
void itemMoved(const QModelIndex& from, const QModelIndex& to);
```

## See Also

- [FluentNavigationView](FluentNavigationView.md) - For navigation trees
- [FluentComboBox](FluentComboBox.md) - For dropdown selection
- [FluentTextInput](FluentTextInput.md) - For search functionality
- [API Reference - FluentTreeView](../API_REFERENCE.md#fluenttreeview) - Complete API documentation
