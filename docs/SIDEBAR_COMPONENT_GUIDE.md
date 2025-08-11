# Enhanced Sidebar Component Guide

The FluentSidebar component provides a comprehensive navigation solution with responsive behavior, smooth animations, and full accessibility support.

## Key Features

### 🎯 Multiple Display Modes
- **Expanded**: Full sidebar with icons and text
- **Compact**: Icons-only mode for space efficiency
- **Overlay**: Modal overlay for mobile interfaces
- **Hidden**: Completely hidden for maximum content space
- **Auto**: Automatically adapts based on screen size

### 📱 Responsive Design
- **Automatic breakpoints**: Adapts to different screen sizes
- **Smooth transitions**: Animated mode changes
- **Touch-friendly**: Optimized for mobile interaction
- **Customizable breakpoints**: Configure responsive behavior

### ♿ Accessibility First
- **Keyboard navigation**: Full keyboard support with arrow keys
- **Screen reader support**: Proper ARIA labels and announcements
- **Focus management**: Logical focus flow and indicators
- **Reduced motion**: Respects accessibility preferences

## Quick Start

### Basic Sidebar Setup

```cpp
#include "FluentQt/Components/FluentSidebar.h"

// Create sidebar
auto* sidebar = new FluentSidebar();
sidebar->setCollapsible(true);
sidebar->setAutoHide(true);

// Add navigation items
FluentSidebarItem homeItem;
homeItem.id = "home";
homeItem.text = "Home";
homeItem.icon = QIcon(":/icons/home.png");
homeItem.tooltip = "Go to home page";
homeItem.accessibleName = "Home navigation item";
sidebar->addItem(homeItem);

FluentSidebarItem settingsItem;
settingsItem.id = "settings";
settingsItem.text = "Settings";
settingsItem.icon = QIcon(":/icons/settings.png");
sidebar->addItem(settingsItem);

// Handle navigation
connect(sidebar, &FluentSidebar::itemClicked, [](const QString& id) {
    qDebug() << "Navigated to:" << id;
});
```

### Responsive Configuration

```cpp
// Enable responsive behavior
sidebar->enableResponsiveBehavior(true);
sidebar->setResponsiveBreakpoints(768, 480); // Compact at 768px, hidden at 480px
sidebar->setMode(FluentSidebarMode::Auto);

// Handle mode changes
connect(sidebar, &FluentSidebar::modeChanged, [](FluentSidebarMode mode) {
    switch (mode) {
        case FluentSidebarMode::Expanded:
            qDebug() << "Sidebar expanded";
            break;
        case FluentSidebarMode::Compact:
            qDebug() << "Sidebar compact";
            break;
        case FluentSidebarMode::Hidden:
            qDebug() << "Sidebar hidden";
            break;
    }
});
```

## Advanced Features

### Custom Item Widgets

```cpp
// Create custom widget for sidebar item
auto* customWidget = new QWidget();
auto* layout = new QHBoxLayout(customWidget);
layout->addWidget(new QLabel("Custom"));
layout->addWidget(new QPushButton("Action"));

FluentSidebarItem customItem;
customItem.id = "custom";
customItem.customWidget = customWidget;
sidebar->addItem(customItem);
```

### Header and Footer

```cpp
// Add header
auto* header = new QLabel("Navigation");
header->setAlignment(Qt::AlignCenter);
header->setStyleSheet("font-weight: bold; padding: 16px;");
sidebar->setHeader(header);

// Add footer with controls
auto* footer = new QWidget();
auto* footerLayout = new QVBoxLayout(footer);
auto* toggleButton = new QPushButton("Toggle");
connect(toggleButton, &QPushButton::clicked, sidebar, &FluentSidebar::toggle);
footerLayout->addWidget(toggleButton);
sidebar->setFooter(footer);
```

### Item Callbacks

```cpp
FluentSidebarItem actionItem;
actionItem.id = "action";
actionItem.text = "Action Item";
actionItem.onClicked = []() {
    // Custom action when item is clicked
    QMessageBox::information(nullptr, "Action", "Custom action executed!");
};
sidebar->addItem(actionItem);
```

## Responsive Behavior

### Breakpoint Configuration

```cpp
// Configure responsive breakpoints
sidebar->setResponsiveBreakpoints(
    768,  // Compact breakpoint - switch to compact mode below this width
    480   // Hidden breakpoint - hide sidebar below this width
);

// Enable responsive behavior
sidebar->enableResponsiveBehavior(true);
sidebar->setMode(FluentSidebarMode::Auto);
```

### Manual Mode Control

```cpp
// Programmatically control sidebar mode
sidebar->expand();        // Switch to expanded mode
sidebar->collapse();      // Switch to compact mode
sidebar->toggle();        // Toggle between expanded and compact
sidebar->showOverlay();   // Show as overlay
sidebar->hideOverlay();   // Hide overlay
```

## Animation Configuration

### Custom Animation Settings

```cpp
// Configure animation timing
sidebar->setAnimationDuration(300);  // 300ms transitions
sidebar->setAnimationEasing(QEasingCurve::OutCubic);

// Handle animation events
connect(sidebar, &FluentSidebar::expanded, []() {
    qDebug() << "Sidebar expansion animation completed";
});

connect(sidebar, &FluentSidebar::collapsed, []() {
    qDebug() << "Sidebar collapse animation completed";
});
```

## Accessibility Features

### Keyboard Navigation

The sidebar supports full keyboard navigation:

- **Up/Down arrows**: Navigate between items
- **Enter/Space**: Activate selected item
- **Escape**: Close overlay mode
- **Tab**: Move focus to next focusable element

### Screen Reader Support

```cpp
// Configure accessibility properties
sidebar->setAccessibleName("Main Navigation");
sidebar->setAccessibleDescription("Primary navigation sidebar");

// Item accessibility
FluentSidebarItem accessibleItem;
accessibleItem.id = "accessible";
accessibleItem.text = "Accessible Item";
accessibleItem.accessibleName = "Accessible navigation item";
accessibleItem.accessibleDescription = "Navigate to accessible content";
accessibleItem.ariaRole = "button";
sidebar->addItem(accessibleItem);
```

### Reduced Motion Support

The sidebar automatically respects system reduced motion preferences and provides appropriate fallbacks.

## Styling and Theming

### Custom Appearance

```cpp
// Configure appearance
sidebar->setExpandedWidth(320);  // Wider expanded mode
sidebar->setCompactWidth(60);    // Wider compact mode

// The sidebar automatically adapts to the current Fluent theme
// Custom styling can be applied through CSS or by subclassing
```

## Best Practices

### 1. Responsive Design

```cpp
// Always enable responsive behavior for better UX
sidebar->enableResponsiveBehavior(true);
sidebar->setMode(FluentSidebarMode::Auto);

// Set appropriate breakpoints for your design
sidebar->setResponsiveBreakpoints(768, 480);
```

### 2. Accessibility

```cpp
// Always provide accessible names and descriptions
FluentSidebarItem item;
item.accessibleName = "Clear, descriptive name";
item.accessibleDescription = "Detailed description of the action";

// Use meaningful tooltips
item.tooltip = "Navigate to the home page";
```

### 3. Performance

```cpp
// Use callbacks for complex actions instead of connecting to signals
item.onClicked = [this]() {
    // Direct action - more efficient than signal/slot
    performAction();
};

// Limit the number of items for better performance
// Consider using categories or sub-menus for large navigation structures
```

### 4. User Experience

```cpp
// Provide visual feedback for state changes
connect(sidebar, &FluentSidebar::modeChanged, [this](FluentSidebarMode mode) {
    updateStatusBar(mode);
    announceStateChange(mode);
});

// Handle overlay mode properly
connect(sidebar, &FluentSidebar::overlayShown, [this]() {
    // Dim background content
    setBackgroundDimmed(true);
});

connect(sidebar, &FluentSidebar::overlayHidden, [this]() {
    // Restore background
    setBackgroundDimmed(false);
});
```

## Integration Examples

### With Main Window

```cpp
class MainWindow : public QMainWindow {
public:
    MainWindow() {
        setupSidebar();
        setupContent();
        setupLayout();
    }

private:
    void setupSidebar() {
        m_sidebar = new FluentSidebar();
        m_sidebar->enableResponsiveBehavior(true);
        
        // Add navigation items
        addNavigationItems();
        
        // Connect navigation
        connect(m_sidebar, &FluentSidebar::itemClicked,
                this, &MainWindow::navigateToPage);
    }
    
    void setupContent() {
        m_contentArea = new QStackedWidget();
        // Add pages to content area
    }
    
    void setupLayout() {
        auto* centralWidget = new QWidget();
        auto* layout = new QHBoxLayout(centralWidget);
        layout->addWidget(m_sidebar);
        layout->addWidget(m_contentArea, 1);
        setCentralWidget(centralWidget);
    }
    
    void navigateToPage(const QString& pageId) {
        // Switch to the appropriate page
        int pageIndex = getPageIndex(pageId);
        m_contentArea->setCurrentIndex(pageIndex);
    }
    
    FluentSidebar* m_sidebar;
    QStackedWidget* m_contentArea;
};
```

### Mobile-First Design

```cpp
// Configure for mobile-first approach
sidebar->setMode(FluentSidebarMode::Hidden);  // Start hidden on mobile
sidebar->setResponsiveBreakpoints(768, 0);    // Show only on larger screens

// Add hamburger menu button for mobile
auto* menuButton = new QPushButton("☰");
connect(menuButton, &QPushButton::clicked, [sidebar]() {
    if (sidebar->mode() == FluentSidebarMode::Hidden) {
        sidebar->showOverlay();
    } else {
        sidebar->hideOverlay();
    }
});
```

## Troubleshooting

### Common Issues

1. **Sidebar not responding to resize events**
   - Ensure responsive behavior is enabled
   - Check that breakpoints are set correctly
   - Verify parent widget size changes are detected

2. **Animations not smooth**
   - Reduce animation duration for better performance
   - Enable hardware acceleration in animation config
   - Check for conflicting animations

3. **Accessibility issues**
   - Verify accessible names are set for all items
   - Test with screen reader software
   - Ensure keyboard navigation works properly

### Performance Optimization

```cpp
// For large numbers of items, consider virtualization
// or lazy loading of sidebar content

// Optimize animations for lower-end devices
if (isLowEndDevice()) {
    sidebar->setAnimationDuration(150);  // Faster animations
    sidebar->setAnimationEasing(QEasingCurve::Linear);  // Simpler easing
}
```

For more examples and detailed API documentation, see the [API Reference](API_REFERENCE.md) and [Examples](../examples/) directory.
