# FluentScrollArea Component

The `FluentScrollArea` component provides a custom scrollable area with styled scrollbars that match the Fluent UI design system. It offers smooth scrolling behavior, scroll position indicators, programmatic scroll control methods, and full accessibility compliance.

## Features

- **Custom Styled Scrollbars**: Fluent UI design system compliant scrollbars
- **Smooth Scrolling**: Multiple scrolling behaviors including kinetic scrolling
- **Auto-Hide Scrollbars**: Optional auto-hiding scrollbars on mouse leave
- **Scroll Indicators**: Visual indicators for scroll position
- **Programmatic Control**: Comprehensive API for scroll control
- **Touch Support**: Full touch device compatibility with kinetic scrolling
- **Accessibility**: Complete keyboard navigation and screen reader support
- **Performance Optimized**: Efficient scrolling with minimal repaints

## Basic Usage

### Simple Scroll Area

```cpp
#include "FluentQt/Components/FluentScrollArea.h"

// Create a scroll area
auto scrollArea = new FluentScrollArea(this);

// Add content
auto contentWidget = new QWidget();
contentWidget->setMinimumSize(800, 1200); // Larger than viewport
scrollArea->setWidget(contentWidget);

// Configure basic properties
scrollArea->setWidgetResizable(true);
```

### Scroll Area with Custom Content

```cpp
auto scrollArea = new FluentScrollArea(this);

// Create content layout
auto contentWidget = new QWidget();
auto layout = new QVBoxLayout(contentWidget);

for (int i = 0; i < 50; ++i) {
    layout->addWidget(new QLabel(QString("Item %1").arg(i)));
}

scrollArea->setWidget(contentWidget);
scrollArea->setWidgetResizable(true);
```

## Scroll Bar Policies

### Always Show Scrollbars

```cpp
auto scrollArea = new FluentScrollArea(this);
scrollArea->setHorizontalScrollBarPolicy(FluentScrollBarPolicy::AlwaysOn);
scrollArea->setVerticalScrollBarPolicy(FluentScrollBarPolicy::AlwaysOn);
```

### Auto-Hide Scrollbars

```cpp
auto scrollArea = new FluentScrollArea(this);
scrollArea->setAutoHideScrollBars(true);
scrollArea->setHorizontalScrollBarPolicy(FluentScrollBarPolicy::AutoHide);
scrollArea->setVerticalScrollBarPolicy(FluentScrollBarPolicy::AutoHide);
```

### Hide Scrollbars

```cpp
auto scrollArea = new FluentScrollArea(this);
scrollArea->setHorizontalScrollBarPolicy(FluentScrollBarPolicy::AlwaysOff);
scrollArea->setVerticalScrollBarPolicy(FluentScrollBarPolicy::AlwaysOff);
```

## Scroll Behavior Configuration

### Smooth Scrolling

```cpp
auto scrollArea = new FluentScrollArea(this);
scrollArea->setScrollBehavior(FluentScrollBehavior::Smooth);
scrollArea->setSmoothScrolling(true);
scrollArea->setScrollSpeed(120); // Pixels per scroll step
```

### Kinetic Scrolling

```cpp
auto scrollArea = new FluentScrollArea(this);
scrollArea->setScrollBehavior(FluentScrollBehavior::Kinetic);
scrollArea->setElasticScrolling(true); // Bounce effect at boundaries
```

### Instant Scrolling

```cpp
auto scrollArea = new FluentScrollArea(this);
scrollArea->setScrollBehavior(FluentScrollBehavior::Instant);
scrollArea->setSmoothScrolling(false);
```

## Programmatic Scrolling

### Scroll to Position

```cpp
auto scrollArea = new FluentScrollArea(this);

// Scroll to specific position
scrollArea->scrollTo(QPoint(100, 200), true); // With animation

// Scroll to specific coordinates
scrollArea->setHorizontalScrollValue(150, true);
scrollArea->setVerticalScrollValue(300, true);
```

### Scroll by Amount

```cpp
auto scrollArea = new FluentScrollArea(this);

// Scroll by pixels
scrollArea->scrollByPixels(50, 100, true);

// Scroll by pages
scrollArea->scrollByPages(1, 2, true);

// Scroll by delta
scrollArea->scrollBy(QPoint(25, 75), true);
```

### Convenience Scrolling

```cpp
auto scrollArea = new FluentScrollArea(this);

// Scroll to edges
scrollArea->scrollToTop(true);
scrollArea->scrollToBottom(true);
scrollArea->scrollToLeft(true);
scrollArea->scrollToRight(true);

// Scroll to center
scrollArea->scrollToCenter(true);
```

## Properties

### Scroll Bar Policies

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `horizontalScrollBarPolicy` | `FluentScrollBarPolicy` | `AsNeeded` | Horizontal scrollbar visibility policy |
| `verticalScrollBarPolicy` | `FluentScrollBarPolicy` | `AsNeeded` | Vertical scrollbar visibility policy |

### Scroll Behavior

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `scrollBehavior` | `FluentScrollBehavior` | `Smooth` | Scrolling behavior type |
| `smoothScrolling` | `bool` | `true` | Enable smooth scrolling animations |
| `scrollSpeed` | `int` | `100` | Scroll speed in pixels per step |
| `elasticScrolling` | `bool` | `false` | Enable elastic/bounce scrolling |

### Appearance

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `showScrollIndicators` | `bool` | `true` | Show scroll position indicators |
| `autoHideScrollBars` | `bool` | `false` | Auto-hide scrollbars when not needed |
| `scrollBarWidth` | `int` | `12` | Width of scrollbars in pixels |
| `scrollBarOpacity` | `qreal` | `1.0` | Opacity of scrollbars (0.0-1.0) |

## Enums

### FluentScrollBarPolicy

```cpp
enum class FluentScrollBarPolicy {
    AsNeeded,   // Show when content exceeds viewport
    AlwaysOff,  // Never show scrollbars
    AlwaysOn,   // Always show scrollbars
    AutoHide    // Hide when not needed, show on hover
};
```

### FluentScrollBehavior

```cpp
enum class FluentScrollBehavior {
    Instant,    // Immediate scrolling without animation
    Smooth,     // Smooth animated scrolling
    Kinetic     // Physics-based scrolling with momentum
};
```

## Signals

### Scroll Events

```cpp
// Emitted when scroll position changes
void scrollPositionChanged(const QPoint& position);

// Emitted when scroll range changes
void scrollRangeChanged(const QSize& range);

// Emitted when scrolling starts/stops
void scrollStarted();
void scrollFinished();

// Emitted when scrollbar visibility changes
void scrollBarVisibilityChanged(Qt::Orientation orientation, bool visible);
```

### Policy Changes

```cpp
void horizontalScrollBarPolicyChanged(FluentScrollBarPolicy policy);
void verticalScrollBarPolicyChanged(FluentScrollBarPolicy policy);
void scrollBehaviorChanged(FluentScrollBehavior behavior);
```

## Public Slots

### Content Management

```cpp
// Ensure specific area is visible
void ensureVisible(int x, int y, int xmargin = 50, int ymargin = 50);
void ensureWidgetVisible(QWidget* childWidget, int xmargin = 50, int ymargin = 50);

// Center on specific position or widget
void centerOn(const QPoint& position);
void centerOnWidget(QWidget* widget);
```

## Scroll Position Information

### Getting Scroll Information

```cpp
auto scrollArea = new FluentScrollArea(this);

// Get current scroll position
QPoint position = scrollArea->scrollPosition();
int hValue = scrollArea->horizontalScrollValue();
int vValue = scrollArea->verticalScrollValue();

// Get scroll range
QSize range = scrollArea->scrollRange();

// Get visible area
QRect visibleRect = scrollArea->visibleRect();

// Get content size
QSize contentSize = scrollArea->contentSize();
```

## Keyboard Navigation

The FluentScrollArea component supports comprehensive keyboard navigation:

- **Arrow Keys**: Scroll by scroll speed amount
- **Shift + Arrow Keys**: Scroll by 5x scroll speed amount
- **Page Up/Down**: Scroll by page
- **Home/End**: Scroll to top/bottom
- **Ctrl + Home/End**: Scroll to left/right edges

## Touch and Mouse Support

### Mouse Interaction

- **Wheel Scrolling**: Standard mouse wheel scrolling
- **Drag Scrolling**: Click and drag to scroll content
- **Scrollbar Interaction**: Click and drag scrollbar handles

### Touch Interaction

- **Touch Scrolling**: Finger drag to scroll
- **Kinetic Scrolling**: Momentum-based scrolling with physics
- **Elastic Scrolling**: Bounce effect at content boundaries

## Accessibility

The component provides comprehensive accessibility support:

- **Screen Reader**: Proper ARIA labels and scroll position announcements
- **Keyboard Navigation**: Full keyboard control
- **Focus Management**: Proper focus handling for scrollable content
- **High Contrast**: Supports high contrast themes

## Performance Considerations

- **Efficient Rendering**: Optimized paint events and scrollbar rendering
- **Smooth Animations**: Hardware-accelerated scrolling animations
- **Memory Management**: Efficient memory usage for large content
- **Viewport Culling**: Only renders visible content areas

## Examples

### Complete Example with Large Content

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "FluentQt/Components/FluentScrollArea.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QMainWindow window;
    auto centralWidget = new QWidget();
    window.setCentralWidget(centralWidget);
    
    auto layout = new QVBoxLayout(centralWidget);
    
    // Create scroll area
    auto scrollArea = new FluentScrollArea();
    scrollArea->setAutoHideScrollBars(true);
    scrollArea->setScrollBehavior(FluentScrollBehavior::Smooth);
    scrollArea->setScrollSpeed(150);
    
    // Create large content
    auto contentWidget = new QWidget();
    auto contentLayout = new QVBoxLayout(contentWidget);
    
    for (int i = 0; i < 100; ++i) {
        auto label = new QLabel(QString("Item %1 - This is a long text item that demonstrates scrolling").arg(i));
        label->setStyleSheet("padding: 10px; border: 1px solid gray; margin: 2px;");
        contentLayout->addWidget(label);
    }
    
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    
    // Add control buttons
    auto buttonLayout = new QHBoxLayout();
    
    auto topButton = new QPushButton("Scroll to Top");
    QObject::connect(topButton, &QPushButton::clicked, scrollArea, &FluentScrollArea::scrollToTop);
    
    auto bottomButton = new QPushButton("Scroll to Bottom");
    QObject::connect(bottomButton, &QPushButton::clicked, scrollArea, &FluentScrollArea::scrollToBottom);
    
    auto centerButton = new QPushButton("Scroll to Center");
    QObject::connect(centerButton, &QPushButton::clicked, scrollArea, &FluentScrollArea::scrollToCenter);
    
    buttonLayout->addWidget(topButton);
    buttonLayout->addWidget(centerButton);
    buttonLayout->addWidget(bottomButton);
    
    layout->addLayout(buttonLayout);
    layout->addWidget(scrollArea);
    
    window.resize(600, 400);
    window.show();
    return app.exec();
}
```

## Best Practices

1. **Set Widget Resizable**: Use `setWidgetResizable(true)` for responsive content
2. **Choose Appropriate Behavior**: Select scroll behavior based on content type
3. **Configure Auto-Hide**: Use auto-hide scrollbars for cleaner interfaces
4. **Optimize Large Content**: Consider virtual scrolling for very large datasets
5. **Test Touch Interaction**: Verify touch scrolling on mobile devices
6. **Provide Keyboard Access**: Ensure all content is keyboard accessible

## See Also

- [FluentResizable](FluentResizable.md) - For resizable container components
- [FluentSheet](FluentSheet.md) - For sliding panel components
- [FluentCard](FluentCard.md) - For container components with elevation
