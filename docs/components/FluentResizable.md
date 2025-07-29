# FluentResizable Component

The `FluentResizable` component provides a resizable container that allows users to adjust dimensions by dragging resize handles. It supports horizontal, vertical, and both-direction resize capabilities with configurable constraints, smooth animations, touch support, and accessibility features.

## Features

- **Multi-directional Resizing**: Support for horizontal, vertical, and both-direction resize
- **Configurable Handles**: Enable/disable specific resize handles
- **Size Constraints**: Set minimum and maximum size limits
- **Smooth Animations**: Optional animated resizing with customizable easing
- **Touch Support**: Full touch device compatibility
- **Grid Snapping**: Snap to grid functionality for precise sizing
- **Aspect Ratio**: Maintain aspect ratio during resize operations
- **Accessibility**: Full keyboard navigation and screen reader support
- **Performance Optimized**: Efficient rendering and interaction handling

## Basic Usage

### Simple Resizable Container

```cpp
#include "FluentQt/Components/FluentResizable.h"

// Create a resizable container
auto resizable = new FluentResizable(this);

// Add content
auto contentWidget = new QLabel("Resizable Content");
resizable->setContentWidget(contentWidget);

// Configure basic properties
resizable->setMinimumSize(QSize(100, 100));
resizable->setMaximumSize(QSize(800, 600));
```

### Horizontal-Only Resizing

```cpp
auto resizable = new FluentResizable(this);
resizable->setResizeDirection(FluentResizeDirection::Horizontal);
resizable->setEnabledHandles(FluentResizeHandle::Left | FluentResizeHandle::Right);
```

### Vertical-Only Resizing

```cpp
auto resizable = new FluentResizable(this);
resizable->setResizeDirection(FluentResizeDirection::Vertical);
resizable->setEnabledHandles(FluentResizeHandle::Top | FluentResizeHandle::Bottom);
```

## Advanced Configuration

### Grid Snapping

```cpp
auto resizable = new FluentResizable(this);
resizable->setSnapToGrid(true);
resizable->setGridSize(20); // Snap to 20-pixel grid
```

### Aspect Ratio Maintenance

```cpp
auto resizable = new FluentResizable(this);
resizable->setAspectRatio(16.0 / 9.0); // 16:9 aspect ratio
resizable->setMaintainAspectRatio(true);
```

### Custom Handle Configuration

```cpp
auto resizable = new FluentResizable(this);

// Enable only corner handles
resizable->setEnabledHandles(
    FluentResizeHandle::TopLeft | 
    FluentResizeHandle::TopRight | 
    FluentResizeHandle::BottomLeft | 
    FluentResizeHandle::BottomRight
);

// Customize handle appearance
resizable->setHandleSize(12);
resizable->setShowHandles(true);
```

### Animation Settings

```cpp
auto resizable = new FluentResizable(this);
resizable->setSmoothResize(true);

// Programmatic resize with animation
resizable->resizeToSize(QSize(400, 300), true);
```

## Properties

### Core Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `resizeDirection` | `FluentResizeDirection` | `Both` | Allowed resize directions |
| `enabledHandles` | `FluentResizeHandles` | `BottomRight` | Which resize handles are enabled |
| `resizeMode` | `FluentResizeMode` | `Immediate` | Resize behavior mode |
| `minimumSize` | `QSize` | `(50, 50)` | Minimum allowed size |
| `maximumSize` | `QSize` | `(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX)` | Maximum allowed size |

### Appearance Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `handleSize` | `int` | `8` | Size of resize handles in pixels |
| `showHandles` | `bool` | `true` | Whether to show resize handles |
| `smoothResize` | `bool` | `true` | Enable smooth resize animations |

### Behavior Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `touchEnabled` | `bool` | `true` | Enable touch interaction |
| `snapToGrid` | `bool` | `false` | Enable grid snapping |
| `gridSize` | `int` | `10` | Grid size for snapping |
| `aspectRatio` | `qreal` | `0.0` | Fixed aspect ratio (0 = disabled) |
| `maintainAspectRatio` | `bool` | `false` | Maintain aspect ratio during resize |

## Enums

### FluentResizeDirection

```cpp
enum class FluentResizeDirection {
    None = 0x00,
    Horizontal = 0x01,
    Vertical = 0x02,
    Both = Horizontal | Vertical
};
```

### FluentResizeHandle

```cpp
enum class FluentResizeHandle {
    None = 0x00,
    Top = 0x01,
    Bottom = 0x02,
    Left = 0x04,
    Right = 0x08,
    TopLeft = Top | Left,
    TopRight = Top | Right,
    BottomLeft = Bottom | Left,
    BottomRight = Bottom | Right
};
```

### FluentResizeMode

```cpp
enum class FluentResizeMode {
    Immediate,      // Resize immediately during drag
    Preview,        // Show preview during drag, apply on release
    Smooth          // Smooth animated resize
};
```

## Signals

### Resize Events

```cpp
// Emitted when resize operation starts
void resizeStarted(const QSize& startSize);

// Emitted during resize operation
void resizing(const QSize& currentSize);

// Emitted when resize operation completes
void resizeFinished(const QSize& finalSize);

// Emitted when resize operation is cancelled
void resizeCancelled();
```

### Property Changes

```cpp
void resizeDirectionChanged(FluentResizeDirection direction);
void enabledHandlesChanged(FluentResizeHandles handles);
void resizeModeChanged(FluentResizeMode mode);
void minimumSizeChanged(const QSize& size);
void maximumSizeChanged(const QSize& size);
void handleSizeChanged(int size);
void showHandlesChanged(bool show);
```

## Public Slots

### Programmatic Resizing

```cpp
// Resize to specific size
void resizeToSize(const QSize& size, bool animated = true);
void resizeToWidth(int width, bool animated = true);
void resizeToHeight(int height, bool animated = true);

// Reset to constraint sizes
void resetToMinimumSize();
void resetToMaximumSize();
void resetToSizeHint();

// Toggle handle visibility
void toggleHandleVisibility();
```

## Keyboard Navigation

The FluentResizable component supports full keyboard navigation:

- **Arrow Keys**: Resize by 1 pixel in the corresponding direction
- **Shift + Arrow Keys**: Resize by 10 pixels in the corresponding direction
- **Escape**: Cancel current resize operation
- **Tab**: Navigate between focusable elements

## Touch Support

Touch interaction is fully supported:

- **Single Touch**: Drag resize handles to resize
- **Multi-touch**: Pinch gestures for proportional resizing (when enabled)
- **Touch Feedback**: Visual feedback for touch interactions

## Accessibility

The component provides comprehensive accessibility support:

- **Screen Reader**: Proper ARIA labels and descriptions
- **Keyboard Navigation**: Full keyboard control
- **Focus Management**: Clear focus indicators
- **High Contrast**: Supports high contrast themes

## Performance Considerations

- **Efficient Rendering**: Optimized paint events and handle positioning
- **Cached Calculations**: Size hints and geometry calculations are cached
- **Smooth Animations**: Hardware-accelerated animations when available
- **Memory Management**: Efficient memory usage with smart pointers

## Examples

### Complete Example

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include "FluentQt/Components/FluentResizable.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QMainWindow window;
    auto centralWidget = new QWidget();
    window.setCentralWidget(centralWidget);
    
    auto layout = new QVBoxLayout(centralWidget);
    
    // Create resizable container
    auto resizable = new FluentResizable();
    
    // Configure properties
    resizable->setMinimumSize(QSize(200, 150));
    resizable->setMaximumSize(QSize(800, 600));
    resizable->setSnapToGrid(true);
    resizable->setGridSize(25);
    
    // Add content
    auto content = new QLabel("Drag the handles to resize this container!");
    content->setAlignment(Qt::AlignCenter);
    content->setStyleSheet("background-color: lightblue; border: 1px solid blue;");
    resizable->setContentWidget(content);
    
    layout->addWidget(resizable);
    
    window.show();
    return app.exec();
}
```

## Best Practices

1. **Set Appropriate Constraints**: Always set reasonable minimum and maximum sizes
2. **Consider Content**: Ensure content can adapt to size changes
3. **Use Grid Snapping**: For precise layouts, enable grid snapping
4. **Provide Feedback**: Use signals to update UI based on size changes
5. **Test Accessibility**: Verify keyboard navigation and screen reader support
6. **Optimize Performance**: Use smooth animations judiciously for better performance

## See Also

- [FluentScrollArea](FluentScrollArea.md) - For scrollable content areas
- [FluentSheet](FluentSheet.md) - For sliding panel components
- [FluentCard](FluentCard.md) - For container components with elevation
