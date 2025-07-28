# FluentSplitter Component

The `FluentSplitter` component is an enhanced splitter widget that implements Microsoft's Fluent Design principles. It provides smooth resizing, customizable handles, and comprehensive layout management for resizable panes.

## Overview

FluentSplitter offers:
- **Horizontal and vertical** orientations
- **Custom splitter handles** with hover effects
- **Smooth animations** for resize operations
- **Collapsible panes** with expand/collapse buttons
- **Size constraints** and proportional sizing
- **Theme integration** with automatic styling
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Splitter

```cpp
#include "FluentQt/Components/FluentSplitter.h"

// Create a horizontal splitter
auto* splitter = new FluentSplitter(Qt::Horizontal);

// Add widgets to splitter
auto* leftWidget = new QWidget;
auto* rightWidget = new QWidget;

splitter->addWidget(leftWidget);
splitter->addWidget(rightWidget);

// Set initial sizes
splitter->setSizes({200, 400});

// Add to layout
layout->addWidget(splitter);
```

### Vertical Splitter

```cpp
auto* splitter = new FluentSplitter(Qt::Vertical);

// Add widgets
auto* topWidget = new QWidget;
auto* bottomWidget = new QWidget;

splitter->addWidget(topWidget);
splitter->addWidget(bottomWidget);

// Set proportional sizes (1:2 ratio)
splitter->setStretchFactor(0, 1);
splitter->setStretchFactor(1, 2);
```

## Splitter Handle Customization

### Custom Handle Appearance

```cpp
auto* splitter = new FluentSplitter(Qt::Horizontal);

// Configure handle appearance
splitter->setHandleWidth(8);
splitter->setHandleColor(QColor(200, 200, 200));
splitter->setHandleHoverColor(QColor(0, 120, 215));
splitter->setHandlePressedColor(QColor(0, 100, 195));

// Handle style
splitter->setHandleStyle(FluentSplitterHandleStyle::Line);
splitter->setHandleStyle(FluentSplitterHandleStyle::Dots);
splitter->setHandleStyle(FluentSplitterHandleStyle::Grip);

// Show handle icons
splitter->setShowHandleIcons(true);
splitter->setHandleIconSize(QSize(12, 12));
```

### Interactive Handle Features

```cpp
auto* splitter = new FluentSplitter(Qt::Horizontal);

// Enable hover effects
splitter->setHandleHoverEnabled(true);
splitter->setHandleHoverAnimationDuration(150);

// Double-click to reset sizes
splitter->setDoubleClickToReset(true);
splitter->setResetSizes({300, 300});

// Handle tooltips
splitter->setHandleTooltipEnabled(true);
splitter->setHandleTooltip("Drag to resize panes");

// Custom handle context menu
splitter->setHandleContextMenuEnabled(true);
connect(splitter, &FluentSplitter::handleContextMenuRequested, [=](int index, const QPoint& pos) {
    auto* menu = new QMenu(this);
    menu->addAction("Reset to Default", [=]() {
        splitter->resetSizes();
    });
    menu->addAction("Collapse Left", [=]() {
        splitter->collapseWidget(0);
    });
    menu->addAction("Collapse Right", [=]() {
        splitter->collapseWidget(1);
    });
    menu->exec(pos);
});
```

## Collapsible Panes

### Enable Collapsing

```cpp
auto* splitter = new FluentSplitter(Qt::Horizontal);

// Enable collapsible panes
splitter->setCollapsible(0, true);  // Left pane collapsible
splitter->setCollapsible(1, true);  // Right pane collapsible

// Show collapse buttons on handles
splitter->setShowCollapseButtons(true);
splitter->setCollapseButtonSize(QSize(16, 16));

// Handle collapse/expand events
connect(splitter, &FluentSplitter::widgetCollapsed, [=](int index) {
    qDebug() << "Widget" << index << "collapsed";
});

connect(splitter, &FluentSplitter::widgetExpanded, [=](int index) {
    qDebug() << "Widget" << index << "expanded";
});

// Programmatic collapse/expand
splitter->collapseWidget(0);  // Collapse first widget
splitter->expandWidget(0);    // Expand first widget
splitter->toggleWidget(0);    // Toggle first widget
```

### Animated Collapse/Expand

```cpp
auto* splitter = new FluentSplitter(Qt::Horizontal);

// Enable smooth collapse/expand animations
splitter->setAnimated(true);
splitter->setCollapseAnimationDuration(300);
splitter->setExpandAnimationDuration(250);
splitter->setAnimationEasing(FluentEasing::CubicOut);

// Custom animation curves
splitter->setCollapseEasing(FluentEasing::BackIn);
splitter->setExpandEasing(FluentEasing::BackOut);

// Handle animation events
connect(splitter, &FluentSplitter::collapseAnimationStarted, [=](int index) {
    qDebug() << "Collapse animation started for widget" << index;
});

connect(splitter, &FluentSplitter::expandAnimationFinished, [=](int index) {
    qDebug() << "Expand animation finished for widget" << index;
});
```

## Size Constraints

### Widget Size Limits

```cpp
auto* splitter = new FluentSplitter(Qt::Horizontal);

// Set minimum and maximum sizes for widgets
splitter->setWidgetMinimumSize(0, 150);  // Left widget min 150px
splitter->setWidgetMaximumSize(0, 400);  // Left widget max 400px

splitter->setWidgetMinimumSize(1, 200);  // Right widget min 200px
// No maximum size for right widget (unlimited)

// Proportional constraints
splitter->setProportionalResize(true);
splitter->setMinimumProportion(0, 0.2);  // Left widget at least 20%
splitter->setMaximumProportion(0, 0.7);  // Left widget at most 70%
```

### Resize Policies

```cpp
auto* splitter = new FluentSplitter(Qt::Horizontal);

// Set resize policies for widgets
splitter->setResizePolicy(0, FluentSplitterResizePolicy::Fixed);      // Fixed size
splitter->setResizePolicy(1, FluentSplitterResizePolicy::Expanding);  // Takes remaining space
splitter->setResizePolicy(2, FluentSplitterResizePolicy::Preferred);  // Preferred size

// Handle resize events
connect(splitter, &FluentSplitter::splitterMoved, [=](int pos, int index) {
    qDebug() << "Splitter" << index << "moved to position" << pos;
});

// Save and restore splitter state
QByteArray state = splitter->saveState();
// ... later ...
splitter->restoreState(state);
```

## Advanced Features

### Nested Splitters

```cpp
// Create main horizontal splitter
auto* mainSplitter = new FluentSplitter(Qt::Horizontal);

// Left pane
auto* leftWidget = new QWidget;

// Right side with vertical splitter
auto* rightSplitter = new FluentSplitter(Qt::Vertical);
auto* topRightWidget = new QWidget;
auto* bottomRightWidget = new QWidget;

rightSplitter->addWidget(topRightWidget);
rightSplitter->addWidget(bottomRightWidget);
rightSplitter->setSizes({200, 200});

// Add to main splitter
mainSplitter->addWidget(leftWidget);
mainSplitter->addWidget(rightSplitter);
mainSplitter->setSizes({250, 400});

// Configure nested splitter
rightSplitter->setHandleWidth(6);
rightSplitter->setCollapsible(0, true);
rightSplitter->setCollapsible(1, true);
```

### Dynamic Widget Management

```cpp
auto* splitter = new FluentSplitter(Qt::Horizontal);

// Add widgets dynamically
void addPane(QWidget* widget, const QString& title) {
    int index = splitter->count();
    splitter->addWidget(widget);
    
    // Set default size
    QList<int> sizes = splitter->sizes();
    if (sizes.size() > 1) {
        int totalSize = std::accumulate(sizes.begin(), sizes.end(), 0);
        int newSize = totalSize / sizes.size();
        sizes.append(newSize);
        
        // Adjust existing sizes
        for (int& size : sizes) {
            size = totalSize / sizes.size();
        }
        
        splitter->setSizes(sizes);
    }
    
    // Make new pane collapsible
    splitter->setCollapsible(index, true);
}

// Remove widgets dynamically
void removePane(int index) {
    if (index >= 0 && index < splitter->count()) {
        QWidget* widget = splitter->widget(index);
        splitter->removeWidget(widget);
        widget->deleteLater();
        
        // Redistribute sizes
        redistributeSizes();
    }
}

void redistributeSizes() {
    int count = splitter->count();
    if (count > 0) {
        int totalSize = splitter->size().width();  // or height() for vertical
        int sizePerWidget = totalSize / count;
        
        QList<int> sizes;
        for (int i = 0; i < count; ++i) {
            sizes.append(sizePerWidget);
        }
        splitter->setSizes(sizes);
    }
}
```

## Integration Examples

### IDE Layout with Splitters

```cpp
class IDEMainWindow : public QMainWindow
{
public:
    IDEMainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupSplitters();
        loadLayout();
    }

private:
    void setupUI() {
        // Create main horizontal splitter
        m_mainSplitter = new FluentSplitter(Qt::Horizontal);
        setCentralWidget(m_mainSplitter);
        
        // Left panel (project explorer)
        m_projectExplorer = new ProjectExplorerWidget;
        m_projectExplorer->setMinimumWidth(200);
        
        // Center area with vertical splitter
        m_centerSplitter = new FluentSplitter(Qt::Vertical);
        
        // Editor area
        m_editorArea = new EditorTabWidget;
        
        // Bottom panel (output, console, etc.)
        m_bottomSplitter = new FluentSplitter(Qt::Horizontal);
        
        m_outputPanel = new OutputWidget;
        m_consolePanel = new ConsoleWidget;
        m_problemsPanel = new ProblemsWidget;
        
        m_bottomSplitter->addWidget(m_outputPanel);
        m_bottomSplitter->addWidget(m_consolePanel);
        m_bottomSplitter->addWidget(m_problemsPanel);
        
        // Right panel (properties, outline)
        m_rightSplitter = new FluentSplitter(Qt::Vertical);
        m_propertiesPanel = new PropertiesWidget;
        m_outlinePanel = new OutlineWidget;
        
        m_rightSplitter->addWidget(m_propertiesPanel);
        m_rightSplitter->addWidget(m_outlinePanel);
        
        // Assemble layout
        m_centerSplitter->addWidget(m_editorArea);
        m_centerSplitter->addWidget(m_bottomSplitter);
        
        m_mainSplitter->addWidget(m_projectExplorer);
        m_mainSplitter->addWidget(m_centerSplitter);
        m_mainSplitter->addWidget(m_rightSplitter);
    }
    
    void setupSplitters() {
        // Configure main splitter
        m_mainSplitter->setHandleWidth(4);
        m_mainSplitter->setCollapsible(0, true);  // Project explorer
        m_mainSplitter->setCollapsible(2, true);  // Right panel
        m_mainSplitter->setShowCollapseButtons(true);
        
        // Configure center splitter
        m_centerSplitter->setHandleWidth(4);
        m_centerSplitter->setCollapsible(1, true);  // Bottom panel
        m_centerSplitter->setShowCollapseButtons(true);
        
        // Configure bottom splitter
        m_bottomSplitter->setHandleWidth(3);
        m_bottomSplitter->setCollapsible(0, true);
        m_bottomSplitter->setCollapsible(1, true);
        m_bottomSplitter->setCollapsible(2, true);
        
        // Configure right splitter
        m_rightSplitter->setHandleWidth(3);
        m_rightSplitter->setCollapsible(0, true);
        m_rightSplitter->setCollapsible(1, true);
        
        // Set initial sizes
        m_mainSplitter->setSizes({250, 800, 200});
        m_centerSplitter->setSizes({600, 200});
        m_bottomSplitter->setSizes({300, 300, 200});
        m_rightSplitter->setSizes({150, 150});
        
        // Connect signals
        connect(m_mainSplitter, &FluentSplitter::splitterMoved, this, &IDEMainWindow::saveLayout);
        connect(m_centerSplitter, &FluentSplitter::splitterMoved, this, &IDEMainWindow::saveLayout);
    }
    
    void saveLayout() {
        QSettings settings;
        settings.setValue("layout/mainSplitter", m_mainSplitter->saveState());
        settings.setValue("layout/centerSplitter", m_centerSplitter->saveState());
        settings.setValue("layout/bottomSplitter", m_bottomSplitter->saveState());
        settings.setValue("layout/rightSplitter", m_rightSplitter->saveState());
    }
    
    void loadLayout() {
        QSettings settings;
        
        QByteArray mainState = settings.value("layout/mainSplitter").toByteArray();
        if (!mainState.isEmpty()) {
            m_mainSplitter->restoreState(mainState);
        }
        
        QByteArray centerState = settings.value("layout/centerSplitter").toByteArray();
        if (!centerState.isEmpty()) {
            m_centerSplitter->restoreState(centerState);
        }
        
        QByteArray bottomState = settings.value("layout/bottomSplitter").toByteArray();
        if (!bottomState.isEmpty()) {
            m_bottomSplitter->restoreState(bottomState);
        }
        
        QByteArray rightState = settings.value("layout/rightSplitter").toByteArray();
        if (!rightState.isEmpty()) {
            m_rightSplitter->restoreState(rightState);
        }
    }

private:
    FluentSplitter* m_mainSplitter{nullptr};
    FluentSplitter* m_centerSplitter{nullptr};
    FluentSplitter* m_bottomSplitter{nullptr};
    FluentSplitter* m_rightSplitter{nullptr};
    
    ProjectExplorerWidget* m_projectExplorer{nullptr};
    EditorTabWidget* m_editorArea{nullptr};
    OutputWidget* m_outputPanel{nullptr};
    ConsoleWidget* m_consolePanel{nullptr};
    ProblemsWidget* m_problemsPanel{nullptr};
    PropertiesWidget* m_propertiesPanel{nullptr};
    OutlineWidget* m_outlinePanel{nullptr};
};
```

### Image Viewer with Splitter

```cpp
class ImageViewerWidget : public QWidget
{
public:
    ImageViewerWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupConnections();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Main splitter (horizontal)
        m_mainSplitter = new FluentSplitter(Qt::Horizontal);
        
        // Left panel: thumbnail list
        m_thumbnailList = new ThumbnailListWidget;
        m_thumbnailList->setMaximumWidth(200);
        m_thumbnailList->setMinimumWidth(150);
        
        // Right panel: image viewer with metadata
        m_rightSplitter = new FluentSplitter(Qt::Vertical);
        
        // Image viewer
        m_imageViewer = new ImageViewerWidget;
        
        // Metadata panel
        m_metadataPanel = new ImageMetadataWidget;
        m_metadataPanel->setMaximumHeight(200);
        m_metadataPanel->setMinimumHeight(100);
        
        // Assemble layout
        m_rightSplitter->addWidget(m_imageViewer);
        m_rightSplitter->addWidget(m_metadataPanel);
        
        m_mainSplitter->addWidget(m_thumbnailList);
        m_mainSplitter->addWidget(m_rightSplitter);
        
        layout->addWidget(m_mainSplitter);
        
        // Configure splitters
        m_mainSplitter->setHandleWidth(4);
        m_mainSplitter->setCollapsible(0, true);  // Thumbnail list
        m_mainSplitter->setShowCollapseButtons(true);
        m_mainSplitter->setSizes({180, 620});
        
        m_rightSplitter->setHandleWidth(4);
        m_rightSplitter->setCollapsible(1, true);  // Metadata panel
        m_rightSplitter->setShowCollapseButtons(true);
        m_rightSplitter->setSizes({500, 150});
        
        // Toolbar for splitter controls
        auto* toolbar = new QHBoxLayout;
        
        auto* toggleThumbnailsBtn = new FluentButton("Thumbnails");
        toggleThumbnailsBtn->setCheckable(true);
        toggleThumbnailsBtn->setChecked(true);
        
        auto* toggleMetadataBtn = new FluentButton("Metadata");
        toggleMetadataBtn->setCheckable(true);
        toggleMetadataBtn->setChecked(true);
        
        auto* resetLayoutBtn = new FluentButton("Reset Layout");
        
        toolbar->addWidget(toggleThumbnailsBtn);
        toolbar->addWidget(toggleMetadataBtn);
        toolbar->addStretch();
        toolbar->addWidget(resetLayoutBtn);
        
        layout->addLayout(toolbar);
        
        // Connect toolbar buttons
        connect(toggleThumbnailsBtn, &FluentButton::toggled, [=](bool checked) {
            if (checked) {
                m_mainSplitter->expandWidget(0);
            } else {
                m_mainSplitter->collapseWidget(0);
            }
        });
        
        connect(toggleMetadataBtn, &FluentButton::toggled, [=](bool checked) {
            if (checked) {
                m_rightSplitter->expandWidget(1);
            } else {
                m_rightSplitter->collapseWidget(1);
            }
        });
        
        connect(resetLayoutBtn, &FluentButton::clicked, [=]() {
            m_mainSplitter->setSizes({180, 620});
            m_rightSplitter->setSizes({500, 150});
            m_mainSplitter->expandWidget(0);
            m_rightSplitter->expandWidget(1);
            toggleThumbnailsBtn->setChecked(true);
            toggleMetadataBtn->setChecked(true);
        });
    }
    
    void setupConnections() {
        // Update toolbar buttons when panels are collapsed/expanded
        connect(m_mainSplitter, &FluentSplitter::widgetCollapsed, [=](int index) {
            if (index == 0) {  // Thumbnails collapsed
                // Update button state without triggering signal
                QSignalBlocker blocker(findChild<FluentButton*>("Thumbnails"));
                findChild<FluentButton*>("Thumbnails")->setChecked(false);
            }
        });
        
        connect(m_mainSplitter, &FluentSplitter::widgetExpanded, [=](int index) {
            if (index == 0) {  // Thumbnails expanded
                QSignalBlocker blocker(findChild<FluentButton*>("Thumbnails"));
                findChild<FluentButton*>("Thumbnails")->setChecked(true);
            }
        });
        
        connect(m_rightSplitter, &FluentSplitter::widgetCollapsed, [=](int index) {
            if (index == 1) {  // Metadata collapsed
                QSignalBlocker blocker(findChild<FluentButton*>("Metadata"));
                findChild<FluentButton*>("Metadata")->setChecked(false);
            }
        });
        
        connect(m_rightSplitter, &FluentSplitter::widgetExpanded, [=](int index) {
            if (index == 1) {  // Metadata expanded
                QSignalBlocker blocker(findChild<FluentButton*>("Metadata"));
                findChild<FluentButton*>("Metadata")->setChecked(true);
            }
        });
    }

private:
    FluentSplitter* m_mainSplitter{nullptr};
    FluentSplitter* m_rightSplitter{nullptr};
    ThumbnailListWidget* m_thumbnailList{nullptr};
    ImageViewerWidget* m_imageViewer{nullptr};
    ImageMetadataWidget* m_metadataPanel{nullptr};
};
```

## Accessibility

FluentSplitter provides comprehensive accessibility support:

```cpp
auto* splitter = new FluentSplitter(Qt::Horizontal);

// Accessible properties (automatically set)
splitter->setAccessibleName("Resizable pane splitter");
splitter->setAccessibleDescription("Drag handles to resize panes");

// Keyboard navigation (automatic)
// - Tab to focus splitter handles
// - Arrow keys to move splitter
// - Enter/Space to activate collapse buttons
// - Escape to cancel resize operation

// Screen reader support
splitter->setAccessibleRole(QAccessible::Splitter);

// Handle announcements
connect(splitter, &FluentSplitter::splitterMoved, [=](int pos, int index) {
    QString announcement = QString("Splitter %1 moved to position %2").arg(index).arg(pos);
    splitter->announceToScreenReader(announcement);
});
```

## Best Practices

### Splitter Design Guidelines
1. **Provide visual feedback** - Show hover states on handles
2. **Set reasonable constraints** - Prevent panes from becoming too small
3. **Enable collapsing** - Allow users to hide panes when needed
4. **Save layout state** - Remember user preferences
5. **Support keyboard navigation** - Ensure accessibility

### Performance Considerations
- Use efficient resize algorithms
- Cache splitter states for quick restoration
- Optimize animations for smooth interaction
- Implement lazy loading for collapsed panes

### Accessibility Guidelines
- Support keyboard navigation
- Provide resize announcements
- Use sufficient handle contrast
- Include meaningful accessible names
- Test with assistive technologies

## Signals Reference

```cpp
// Resize signals
void splitterMoved(int pos, int index);
void sizeChanged(const QList<int>& sizes);

// Collapse/expand signals
void widgetCollapsed(int index);
void widgetExpanded(int index);
void collapseAnimationStarted(int index);
void collapseAnimationFinished(int index);
void expandAnimationStarted(int index);
void expandAnimationFinished(int index);

// Handle interaction signals
void handlePressed(int index);
void handleReleased(int index);
void handleHovered(int index, bool isHovered);
void handleContextMenuRequested(int index, const QPoint& pos);
```

## See Also

- [FluentPanel](FluentPanel.md) - For organizing splitter content
- [FluentTabView](FluentTabView.md) - For tabbed layouts
- [FluentNavigationView](FluentNavigationView.md) - For navigation layouts
- [API Reference - FluentSplitter](../API_REFERENCE.md#fluentsplitter) - Complete API documentation
