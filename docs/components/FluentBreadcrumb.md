# FluentBreadcrumb Component

The `FluentBreadcrumb` component is an enhanced breadcrumb navigation widget that implements Microsoft's Fluent Design principles. It provides hierarchical navigation with smooth animations, customizable separators, and comprehensive interaction features.

## Overview

FluentBreadcrumb offers:
- **Hierarchical navigation** with clickable path segments
- **Custom separators** and styling options
- **Overflow handling** with ellipsis and dropdown
- **Interactive elements** with hover and click effects
- **Icon support** for path segments
- **Theme integration** with automatic styling
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Breadcrumb

```cpp
#include "FluentQt/Components/FluentBreadcrumb.h"

// Create a basic breadcrumb
auto* breadcrumb = new FluentBreadcrumb(this);

// Set the current path
breadcrumb->setPath({"Home", "Documents", "Projects", "MyProject"});

// Handle navigation
connect(breadcrumb, &FluentBreadcrumb::itemClicked, [=](int index, const QString& text) {
    qDebug() << "Navigating to:" << text << "at index:" << index;
    navigateToPath(index);
});

// Add to layout
layout->addWidget(breadcrumb);
```

### Breadcrumb with Icons

```cpp
auto* breadcrumb = new FluentBreadcrumb(this);

// Create breadcrumb items with icons
QList<FluentBreadcrumbItem> items;
items.append({"Home", QIcon(":/icons/home")});
items.append({"Documents", QIcon(":/icons/folder")});
items.append({"Projects", QIcon(":/icons/folder")});
items.append({"MyProject", QIcon(":/icons/project")});

breadcrumb->setItems(items);

// Configure icon display
breadcrumb->setShowIcons(true);
breadcrumb->setIconSize(QSize(16, 16));
breadcrumb->setIconPosition(FluentIconPosition::Left);
```

## Path Management

### Dynamic Path Updates

```cpp
auto* breadcrumb = new FluentBreadcrumb(this);

// Update path dynamically
void updateBreadcrumbPath(const QString& currentPath) {
    QStringList pathParts = currentPath.split('/', Qt::SkipEmptyParts);
    
    QList<FluentBreadcrumbItem> items;
    
    // Add root item
    items.append({"Computer", QIcon(":/icons/computer")});
    
    // Add path segments
    for (const QString& part : pathParts) {
        QIcon icon = getIconForPath(part);
        items.append({part, icon});
    }
    
    breadcrumb->setItems(items);
}

// Navigate to specific index
void navigateToPath(int index) {
    if (index >= 0 && index < breadcrumb->count()) {
        // Build path up to selected index
        QStringList pathParts;
        for (int i = 1; i <= index; ++i) {  // Skip root
            pathParts.append(breadcrumb->itemText(i));
        }
        
        QString newPath = pathParts.join('/');
        emit pathChanged(newPath);
    }
}
```

### Breadcrumb with Data

```cpp
struct PathData {
    QString displayName;
    QString fullPath;
    QIcon icon;
    QVariant userData;
};

auto* breadcrumb = new FluentBreadcrumb(this);

// Set items with associated data
void setBreadcrumbWithData(const QList<PathData>& pathData) {
    QList<FluentBreadcrumbItem> items;
    
    for (const PathData& data : pathData) {
        FluentBreadcrumbItem item;
        item.text = data.displayName;
        item.icon = data.icon;
        item.data = QVariant::fromValue(data);
        items.append(item);
    }
    
    breadcrumb->setItems(items);
}

// Handle navigation with data
connect(breadcrumb, &FluentBreadcrumb::itemClicked, [=](int index, const QString& text) {
    FluentBreadcrumbItem item = breadcrumb->item(index);
    PathData data = item.data.value<PathData>();
    
    // Navigate using full path
    navigateToFullPath(data.fullPath);
});
```

## Customization

### Separator Styles

```cpp
auto* breadcrumb = new FluentBreadcrumb(this);

// Built-in separator styles
breadcrumb->setSeparatorStyle(FluentBreadcrumbSeparator::Arrow);
breadcrumb->setSeparatorStyle(FluentBreadcrumbSeparator::Chevron);
breadcrumb->setSeparatorStyle(FluentBreadcrumbSeparator::Slash);
breadcrumb->setSeparatorStyle(FluentBreadcrumbSeparator::Dot);

// Custom separator
breadcrumb->setSeparatorText(" → ");
breadcrumb->setSeparatorIcon(QIcon(":/icons/arrow-right"));

// Separator styling
breadcrumb->setSeparatorColor(QColor(128, 128, 128));
breadcrumb->setSeparatorSize(QSize(12, 12));
breadcrumb->setSeparatorSpacing(8);
```

### Item Appearance

```cpp
auto* breadcrumb = new FluentBreadcrumb(this);

// Item styling
breadcrumb->setItemTextColor(QColor(0, 0, 0));
breadcrumb->setItemHoverColor(QColor(0, 120, 215));
breadcrumb->setItemPressedColor(QColor(0, 100, 195));
breadcrumb->setItemDisabledColor(QColor(128, 128, 128));

// Current item (last item) styling
breadcrumb->setCurrentItemColor(QColor(0, 120, 215));
breadcrumb->setCurrentItemBold(true);

// Item padding and spacing
breadcrumb->setItemPadding(QMargins(8, 4, 8, 4));
breadcrumb->setItemSpacing(4);

// Hover effects
breadcrumb->setItemHoverEnabled(true);
breadcrumb->setItemHoverAnimationDuration(150);
```

## Overflow Handling

### Ellipsis and Dropdown

```cpp
auto* breadcrumb = new FluentBreadcrumb(this);

// Enable overflow handling
breadcrumb->setOverflowMode(FluentBreadcrumbOverflow::Ellipsis);
breadcrumb->setMaxVisibleItems(5);

// Configure ellipsis
breadcrumb->setEllipsisText("...");
breadcrumb->setEllipsisIcon(QIcon(":/icons/more"));

// Dropdown for overflow items
breadcrumb->setOverflowDropdownEnabled(true);
breadcrumb->setOverflowDropdownIcon(QIcon(":/icons/dropdown"));

// Handle overflow dropdown
connect(breadcrumb, &FluentBreadcrumb::overflowDropdownRequested, [=](const QPoint& pos) {
    auto* menu = new QMenu(this);
    
    // Add hidden items to menu
    QList<FluentBreadcrumbItem> hiddenItems = breadcrumb->hiddenItems();
    for (const FluentBreadcrumbItem& item : hiddenItems) {
        auto* action = menu->addAction(item.icon, item.text);
        action->setData(item.data);
        
        connect(action, &QAction::triggered, [=]() {
            int index = breadcrumb->indexOfItem(item);
            emit breadcrumb->itemClicked(index, item.text);
        });
    }
    
    menu->exec(pos);
});
```

### Responsive Behavior

```cpp
auto* breadcrumb = new FluentBreadcrumb(this);

// Enable responsive behavior
breadcrumb->setResponsive(true);
breadcrumb->setMinimumVisibleItems(2);  // Always show at least 2 items

// Custom responsive logic
connect(breadcrumb, &FluentBreadcrumb::resized, [=](const QSize& size) {
    int availableWidth = size.width();
    int itemWidth = breadcrumb->averageItemWidth();
    int maxItems = availableWidth / itemWidth;
    
    breadcrumb->setMaxVisibleItems(qMax(2, maxItems));
});

// Adaptive separator
breadcrumb->setAdaptiveSeparator(true);  // Use shorter separators when space is limited
```

## Integration Examples

### File Explorer Breadcrumb

```cpp
class FileExplorerBreadcrumb : public FluentBreadcrumb
{
    Q_OBJECT

public:
    FileExplorerBreadcrumb(QWidget* parent = nullptr) : FluentBreadcrumb(parent) {
        setupBreadcrumb();
        setupConnections();
    }
    
    void setCurrentPath(const QString& path) {
        m_currentPath = path;
        updateBreadcrumb();
    }
    
    QString currentPath() const {
        return m_currentPath;
    }

private:
    void setupBreadcrumb() {
        // Configure appearance
        setSeparatorStyle(FluentBreadcrumbSeparator::Chevron);
        setShowIcons(true);
        setIconSize(QSize(16, 16));
        setOverflowMode(FluentBreadcrumbOverflow::Ellipsis);
        setMaxVisibleItems(6);
        
        // Enable context menu
        setContextMenuPolicy(Qt::CustomContextMenu);
    }
    
    void setupConnections() {
        // Handle navigation
        connect(this, &FluentBreadcrumb::itemClicked, [=](int index, const QString& text) {
            navigateToIndex(index);
        });
        
        // Handle context menu
        connect(this, &QWidget::customContextMenuRequested, [=](const QPoint& pos) {
            showContextMenu(pos);
        });
        
        // Handle overflow dropdown
        connect(this, &FluentBreadcrumb::overflowDropdownRequested, [=](const QPoint& pos) {
            showOverflowMenu(pos);
        });
    }
    
    void updateBreadcrumb() {
        QList<FluentBreadcrumbItem> items;
        
        // Add computer/root item
        items.append({"This PC", QIcon(":/icons/computer"), QVariant("")});
        
        if (!m_currentPath.isEmpty()) {
            QDir dir(m_currentPath);
            QStringList pathParts;
            
            // Build path hierarchy
            QString tempPath = m_currentPath;
            while (!tempPath.isEmpty() && tempPath != "/") {
                QDir tempDir(tempPath);
                pathParts.prepend(tempDir.dirName());
                tempPath = tempDir.absolutePath();
                tempDir.cdUp();
                tempPath = tempDir.absolutePath();
                
                if (tempPath == tempDir.absolutePath()) break;  // Reached root
            }
            
            // Add path segments
            QString buildPath = "";
            for (const QString& part : pathParts) {
                if (!buildPath.isEmpty()) buildPath += "/";
                buildPath += part;
                
                QIcon icon = getIconForPath(buildPath);
                items.append({part, icon, QVariant(buildPath)});
            }
        }
        
        setItems(items);
    }
    
    void navigateToIndex(int index) {
        if (index >= 0 && index < count()) {
            FluentBreadcrumbItem item = this->item(index);
            QString path = item.data.toString();
            
            if (path != m_currentPath) {
                m_currentPath = path;
                emit pathChanged(path);
            }
        }
    }
    
    void showContextMenu(const QPoint& pos) {
        int index = itemAt(pos);
        if (index < 0) return;
        
        FluentBreadcrumbItem item = this->item(index);
        QString path = item.data.toString();
        
        auto* menu = new QMenu(this);
        
        menu->addAction(QIcon(":/icons/open"), "Open", [=]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        });
        
        menu->addAction(QIcon(":/icons/copy"), "Copy Path", [=]() {
            QApplication::clipboard()->setText(path);
        });
        
        if (!path.isEmpty()) {
            menu->addSeparator();
            menu->addAction(QIcon(":/icons/properties"), "Properties", [=]() {
                showPathProperties(path);
            });
        }
        
        menu->exec(mapToGlobal(pos));
    }
    
    void showOverflowMenu(const QPoint& pos) {
        auto* menu = new QMenu(this);
        
        QList<FluentBreadcrumbItem> hiddenItems = this->hiddenItems();
        for (const FluentBreadcrumbItem& item : hiddenItems) {
            auto* action = menu->addAction(item.icon, item.text);
            action->setData(item.data);
            
            connect(action, &QAction::triggered, [=]() {
                QString path = action->data().toString();
                if (path != m_currentPath) {
                    m_currentPath = path;
                    emit pathChanged(path);
                }
            });
        }
        
        menu->exec(pos);
    }
    
    QIcon getIconForPath(const QString& path) {
        if (path.isEmpty()) return QIcon(":/icons/computer");
        
        QFileInfo info(path);
        if (info.isDir()) {
            // Special folder icons
            if (path.contains("Desktop")) return QIcon(":/icons/desktop");
            if (path.contains("Documents")) return QIcon(":/icons/documents");
            if (path.contains("Downloads")) return QIcon(":/icons/downloads");
            if (path.contains("Pictures")) return QIcon(":/icons/pictures");
            return QIcon(":/icons/folder");
        }
        
        return QIcon(":/icons/file");
    }

signals:
    void pathChanged(const QString& path);

private:
    QString m_currentPath;
};
```

### Website Navigation Breadcrumb

```cpp
class WebsiteBreadcrumb : public FluentBreadcrumb
{
    Q_OBJECT

public:
    WebsiteBreadcrumb(QWidget* parent = nullptr) : FluentBreadcrumb(parent) {
        setupBreadcrumb();
    }
    
    void setCurrentUrl(const QUrl& url) {
        m_currentUrl = url;
        updateBreadcrumb();
    }

private:
    void setupBreadcrumb() {
        setSeparatorStyle(FluentBreadcrumbSeparator::Chevron);
        setShowIcons(true);
        setIconSize(QSize(16, 16));
        setOverflowMode(FluentBreadcrumbOverflow::Ellipsis);
        setMaxVisibleItems(5);
        
        connect(this, &FluentBreadcrumb::itemClicked, [=](int index, const QString& text) {
            navigateToIndex(index);
        });
    }
    
    void updateBreadcrumb() {
        QList<FluentBreadcrumbItem> items;
        
        if (m_currentUrl.isValid()) {
            // Add domain
            QString domain = m_currentUrl.host();
            items.append({domain, QIcon(":/icons/web"), QVariant(QString("https://%1").arg(domain))});
            
            // Add path segments
            QString path = m_currentUrl.path();
            if (!path.isEmpty() && path != "/") {
                QStringList pathParts = path.split('/', Qt::SkipEmptyParts);
                QString buildPath = "";
                
                for (const QString& part : pathParts) {
                    buildPath += "/" + part;
                    QString fullUrl = QString("https://%1%2").arg(domain, buildPath);
                    
                    // Decode URL-encoded text for display
                    QString displayText = QUrl::fromPercentEncoding(part.toUtf8());
                    
                    items.append({displayText, QIcon(":/icons/page"), QVariant(fullUrl)});
                }
            }
        }
        
        setItems(items);
    }
    
    void navigateToIndex(int index) {
        if (index >= 0 && index < count()) {
            FluentBreadcrumbItem item = this->item(index);
            QString url = item.data.toString();
            emit urlRequested(QUrl(url));
        }
    }

signals:
    void urlRequested(const QUrl& url);

private:
    QUrl m_currentUrl;
};
```

### Application Settings Breadcrumb

```cpp
class SettingsBreadcrumb : public FluentBreadcrumb
{
    Q_OBJECT

public:
    SettingsBreadcrumb(QWidget* parent = nullptr) : FluentBreadcrumb(parent) {
        setupBreadcrumb();
        setupSettingsHierarchy();
    }
    
    void navigateToSetting(const QString& settingPath) {
        QStringList pathParts = settingPath.split('/', Qt::SkipEmptyParts);
        updateBreadcrumbForPath(pathParts);
    }

private:
    void setupBreadcrumb() {
        setSeparatorStyle(FluentBreadcrumbSeparator::Arrow);
        setShowIcons(true);
        setIconSize(QSize(16, 16));
        
        connect(this, &FluentBreadcrumb::itemClicked, [=](int index, const QString& text) {
            navigateToSettingIndex(index);
        });
    }
    
    void setupSettingsHierarchy() {
        // Define settings hierarchy with icons
        m_settingsHierarchy = {
            {"Settings", QIcon(":/icons/settings")},
            {"System", QIcon(":/icons/system")},
            {"Display", QIcon(":/icons/display")},
            {"Sound", QIcon(":/icons/sound")},
            {"Network", QIcon(":/icons/network")},
            {"Privacy", QIcon(":/icons/privacy")},
            {"Accounts", QIcon(":/icons/accounts")},
            {"Personalization", QIcon(":/icons/personalization")},
            {"Apps", QIcon(":/icons/apps")},
            {"Gaming", QIcon(":/icons/gaming")},
            {"Accessibility", QIcon(":/icons/accessibility")}
        };
    }
    
    void updateBreadcrumbForPath(const QStringList& pathParts) {
        QList<FluentBreadcrumbItem> items;
        
        // Always start with Settings root
        items.append({"Settings", QIcon(":/icons/settings"), QVariant("Settings")});
        
        // Add path segments
        QString buildPath = "Settings";
        for (const QString& part : pathParts) {
            buildPath += "/" + part;
            
            QIcon icon = getIconForSetting(part);
            items.append({part, icon, QVariant(buildPath)});
        }
        
        setItems(items);
    }
    
    QIcon getIconForSetting(const QString& setting) {
        for (const auto& pair : m_settingsHierarchy) {
            if (pair.first == setting) {
                return pair.second;
            }
        }
        return QIcon(":/icons/setting");  // Default icon
    }
    
    void navigateToSettingIndex(int index) {
        if (index >= 0 && index < count()) {
            FluentBreadcrumbItem item = this->item(index);
            QString settingPath = item.data.toString();
            emit settingRequested(settingPath);
        }
    }

signals:
    void settingRequested(const QString& settingPath);

private:
    QList<QPair<QString, QIcon>> m_settingsHierarchy;
};
```

## Accessibility

FluentBreadcrumb provides comprehensive accessibility support:

```cpp
auto* breadcrumb = new FluentBreadcrumb(this);

// Accessible properties (automatically set)
breadcrumb->setAccessibleName("Navigation breadcrumb");
breadcrumb->setAccessibleDescription("Shows current location in hierarchy");

// Keyboard navigation (automatic)
// - Tab to navigate between breadcrumb items
// - Enter/Space to activate items
// - Arrow keys for item navigation
// - Home/End for first/last item

// Screen reader support
breadcrumb->setAccessibleRole(QAccessible::List);

// Item announcements
connect(breadcrumb, &FluentBreadcrumb::itemClicked, [=](int index, const QString& text) {
    QString announcement = QString("Navigating to %1").arg(text);
    breadcrumb->announceToScreenReader(announcement);
});
```

## Best Practices

### Breadcrumb Design Guidelines
1. **Show clear hierarchy** - Make parent-child relationships obvious
2. **Use meaningful labels** - Avoid technical or cryptic names
3. **Handle overflow gracefully** - Provide access to hidden items
4. **Support keyboard navigation** - Ensure accessibility
5. **Provide visual feedback** - Show hover and active states

### Performance Considerations
- Use efficient path parsing algorithms
- Cache breadcrumb items when possible
- Optimize overflow calculations
- Implement lazy loading for large hierarchies

### Accessibility Guidelines
- Support keyboard navigation
- Provide navigation announcements
- Use sufficient color contrast
- Include meaningful item descriptions
- Test with assistive technologies

## Signals Reference

```cpp
// Navigation signals
void itemClicked(int index, const QString& text);
void itemHovered(int index, const QString& text, bool isHovered);
void pathChanged(const QStringList& path);

// Overflow signals
void overflowDropdownRequested(const QPoint& pos);
void overflowStateChanged(bool hasOverflow);

// Interaction signals
void itemPressed(int index);
void itemReleased(int index);
void contextMenuRequested(int index, const QPoint& pos);
```

## See Also

- [FluentNavigationView](FluentNavigationView.md) - For main navigation
- [FluentTabView](FluentTabView.md) - For tab navigation
- [FluentButton](FluentButton.md) - For navigation buttons
- [API Reference - FluentBreadcrumb](../API_REFERENCE.md#fluentbreadcrumb) - Complete API documentation
