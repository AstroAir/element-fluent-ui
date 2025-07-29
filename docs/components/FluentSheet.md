# FluentSheet Component

The `FluentSheet` component provides a sheet/drawer that slides in from screen edges. It supports all four directions (top, bottom, left, right), configurable overlay backdrop with click-to-close functionality, smooth slide animations, responsive behavior, and comprehensive focus management.

## Features

- **Multi-directional Sliding**: Support for top, bottom, left, and right edge sliding
- **Configurable Sizes**: Predefined sizes (small, medium, large, full) and custom sizing
- **Modal and Modeless**: Support for modal and modeless behavior
- **Overlay Backdrop**: Configurable overlay with click-to-close functionality
- **Smooth Animations**: Customizable slide animations with easing curves
- **Drag to Close**: Optional drag-to-close gesture support
- **Focus Management**: Automatic focus capture and restoration
- **Responsive Design**: Adapts to different screen sizes and orientations
- **Accessibility**: Full keyboard navigation and screen reader support

## Basic Usage

### Simple Bottom Sheet

```cpp
#include "FluentQt/Components/FluentSheet.h"

// Create a bottom sheet
auto sheet = new FluentSheet(FluentSheetDirection::Bottom, this);

// Add content
auto contentWidget = new QLabel("Sheet Content");
sheet->setContentWidget(contentWidget);

// Configure basic properties
sheet->setTitle("My Sheet");
sheet->setSheetSize(FluentSheetSize::Medium);

// Open the sheet
sheet->open();
```

### Factory Methods

```cpp
// Create sheets using factory methods
auto bottomSheet = FluentSheet::createBottomSheet(this);
auto topSheet = FluentSheet::createTopSheet(this);
auto leftSheet = FluentSheet::createLeftSheet(this);
auto rightSheet = FluentSheet::createRightSheet(this);
```

## Direction and Size Configuration

### Direction Settings

```cpp
auto sheet = new FluentSheet(this);

// Set slide direction
sheet->setDirection(FluentSheetDirection::Top);
sheet->setDirection(FluentSheetDirection::Bottom);
sheet->setDirection(FluentSheetDirection::Left);
sheet->setDirection(FluentSheetDirection::Right);
```

### Size Configuration

```cpp
auto sheet = new FluentSheet(this);

// Predefined sizes
sheet->setSheetSize(FluentSheetSize::Small);   // 25% of screen
sheet->setSheetSize(FluentSheetSize::Medium);  // 50% of screen
sheet->setSheetSize(FluentSheetSize::Large);   // 75% of screen
sheet->setSheetSize(FluentSheetSize::Full);    // 100% of screen

// Custom size
sheet->setSheetSize(FluentSheetSize::Custom);
sheet->setCustomSize(QSize(400, 300));
```

## Behavior Configuration

### Modal vs Modeless

```cpp
auto sheet = new FluentSheet(this);

// Modal behavior (blocks parent interaction)
sheet->setBehavior(FluentSheetBehavior::Modal);

// Modeless behavior (allows parent interaction)
sheet->setBehavior(FluentSheetBehavior::Modeless);

// Persistent behavior (stays open until explicitly closed)
sheet->setBehavior(FluentSheetBehavior::Persistent);
```

### Overlay Configuration

```cpp
auto sheet = new FluentSheet(this);

// Configure overlay
sheet->setOverlayVisible(true);
sheet->setOverlayOpacity(0.6);
sheet->setCloseOnOverlayClick(true);
```

### Interaction Settings

```cpp
auto sheet = new FluentSheet(this);

// Configure interaction behavior
sheet->setCloseOnEscape(true);
sheet->setDragToClose(true);
sheet->setAutoResize(true);
```

## Animation Configuration

### Animation Settings

```cpp
auto sheet = new FluentSheet(this);

// Configure animations
sheet->setAnimationDuration(400); // milliseconds
sheet->setEasingCurve(QEasingCurve::OutCubic);
```

### Custom Animation Curves

```cpp
auto sheet = new FluentSheet(this);

// Different easing curves
sheet->setEasingCurve(QEasingCurve::InOutQuad);
sheet->setEasingCurve(QEasingCurve::OutBounce);
sheet->setEasingCurve(QEasingCurve::InOutElastic);
```

## Properties

### Core Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `direction` | `FluentSheetDirection` | `Bottom` | Slide direction |
| `sheetSize` | `FluentSheetSize` | `Medium` | Predefined size |
| `customSize` | `QSize` | `(400, 300)` | Custom size when using Custom size mode |
| `behavior` | `FluentSheetBehavior` | `Modal` | Interaction behavior |

### Overlay Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `overlayVisible` | `bool` | `true` | Whether to show overlay backdrop |
| `overlayOpacity` | `qreal` | `0.5` | Opacity of overlay backdrop (0.0-1.0) |

### Animation Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `animationDuration` | `int` | `300` | Animation duration in milliseconds |
| `easingCurve` | `QEasingCurve::Type` | `OutCubic` | Animation easing curve |

### Interaction Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `closeOnOverlayClick` | `bool` | `true` | Close when overlay is clicked |
| `closeOnEscape` | `bool` | `true` | Close when Escape key is pressed |
| `dragToClose` | `bool` | `true` | Enable drag-to-close gesture |
| `autoResize` | `bool` | `true` | Auto-resize based on content |

## Enums

### FluentSheetDirection

```cpp
enum class FluentSheetDirection {
    Top,     // Slide from top edge
    Bottom,  // Slide from bottom edge
    Left,    // Slide from left edge
    Right    // Slide from right edge
};
```

### FluentSheetSize

```cpp
enum class FluentSheetSize {
    Small,   // 25% of screen
    Medium,  // 50% of screen
    Large,   // 75% of screen
    Full,    // 100% of screen
    Custom   // User-defined size
};
```

### FluentSheetBehavior

```cpp
enum class FluentSheetBehavior {
    Modal,      // Blocks interaction with parent
    Modeless,   // Allows interaction with parent
    Persistent  // Stays open until explicitly closed
};
```

## Signals

### Lifecycle Events

```cpp
// Emitted before sheet opens
void aboutToOpen();

// Emitted after sheet has opened
void opened();

// Emitted before sheet closes
void aboutToClose();

// Emitted after sheet has closed
void closed();
```

### Interaction Events

```cpp
// Emitted when overlay is clicked
void overlayClicked();

// Emitted when drag operation starts/ends
void dragStarted();
void dragFinished();
```

### Property Changes

```cpp
void directionChanged(FluentSheetDirection direction);
void sheetSizeChanged(FluentSheetSize size);
void behaviorChanged(FluentSheetBehavior behavior);
void visibilityChanged(bool visible);
```

## Public Slots

### Open/Close Operations

```cpp
// Open/close the sheet
void open();
void close();
void toggle();

// Open/close with explicit animation
void openWithAnimation();
void closeWithAnimation();
```

## Content Management

### Setting Content

```cpp
auto sheet = new FluentSheet(this);

// Set content widget
auto contentWidget = new QWidget();
auto layout = new QVBoxLayout(contentWidget);
layout->addWidget(new QLabel("Sheet content goes here"));
sheet->setContentWidget(contentWidget);

// Set title and subtitle
sheet->setTitle("Sheet Title");
sheet->setSubtitle("Optional subtitle");
```

### Getting Content

```cpp
auto sheet = new FluentSheet(this);

// Get content information
QWidget* content = sheet->contentWidget();
QString title = sheet->title();
QString subtitle = sheet->subtitle();

// Check state
bool isOpen = sheet->isOpen();
bool isAnimating = sheet->isAnimating();
```

## Keyboard Navigation

The FluentSheet component supports comprehensive keyboard navigation:

- **Escape**: Close the sheet (when `closeOnEscape` is enabled)
- **Tab/Shift+Tab**: Navigate between focusable elements within the sheet
- **Enter/Space**: Activate focused elements
- **Arrow Keys**: Navigate within content (if supported by content widgets)

## Focus Management

The sheet automatically manages focus:

- **Focus Capture**: Automatically captures focus when opened
- **Focus Restoration**: Restores focus to previous element when closed
- **Focus Containment**: Keeps focus within the sheet in modal mode
- **Focus Chain**: Maintains proper tab order within sheet content

## Accessibility

The component provides comprehensive accessibility support:

- **Screen Reader**: Proper ARIA labels and role announcements
- **Keyboard Navigation**: Full keyboard control
- **Focus Management**: Proper focus handling and containment
- **High Contrast**: Supports high contrast themes
- **Announcements**: Announces open/close state changes

## Examples

### Complete Example with Form Content

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include "FluentQt/Components/FluentSheet.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QMainWindow window;
    auto centralWidget = new QWidget();
    window.setCentralWidget(centralWidget);
    
    auto layout = new QVBoxLayout(centralWidget);
    
    // Create trigger button
    auto openButton = new QPushButton("Open Settings Sheet");
    layout->addWidget(openButton);
    
    // Create sheet
    auto sheet = new FluentSheet(FluentSheetDirection::Right, &window);
    sheet->setTitle("Settings");
    sheet->setSubtitle("Configure application settings");
    sheet->setSheetSize(FluentSheetSize::Medium);
    
    // Create form content
    auto contentWidget = new QWidget();
    auto formLayout = new QFormLayout(contentWidget);
    
    auto nameEdit = new QLineEdit();
    auto emailEdit = new QLineEdit();
    auto phoneEdit = new QLineEdit();
    
    formLayout->addRow("Name:", nameEdit);
    formLayout->addRow("Email:", emailEdit);
    formLayout->addRow("Phone:", phoneEdit);
    
    // Add action buttons
    auto buttonLayout = new QHBoxLayout();
    auto saveButton = new QPushButton("Save");
    auto cancelButton = new QPushButton("Cancel");
    
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    formLayout->addRow(buttonLayout);
    
    sheet->setContentWidget(contentWidget);
    
    // Connect signals
    QObject::connect(openButton, &QPushButton::clicked, sheet, &FluentSheet::open);
    QObject::connect(cancelButton, &QPushButton::clicked, sheet, &FluentSheet::close);
    QObject::connect(saveButton, &QPushButton::clicked, [sheet]() {
        // Save logic here
        sheet->close();
    });
    
    window.resize(800, 600);
    window.show();
    return app.exec();
}
```

### Multi-Direction Sheet Example

```cpp
// Create sheets for different directions
auto topSheet = FluentSheet::createTopSheet(this);
topSheet->setTitle("Top Sheet");
topSheet->setSheetSize(FluentSheetSize::Small);

auto bottomSheet = FluentSheet::createBottomSheet(this);
bottomSheet->setTitle("Bottom Sheet");
bottomSheet->setSheetSize(FluentSheetSize::Medium);

auto leftSheet = FluentSheet::createLeftSheet(this);
leftSheet->setTitle("Left Sheet");
leftSheet->setSheetSize(FluentSheetSize::Large);

auto rightSheet = FluentSheet::createRightSheet(this);
rightSheet->setTitle("Right Sheet");
rightSheet->setSheetSize(FluentSheetSize::Custom);
rightSheet->setCustomSize(QSize(350, 500));
```

## Best Practices

1. **Choose Appropriate Direction**: Select direction based on content and context
2. **Set Reasonable Sizes**: Use appropriate sizes for content and screen size
3. **Configure Overlay**: Use overlay for modal sheets, consider disabling for modeless
4. **Handle Focus**: Ensure proper focus management for accessibility
5. **Test Animations**: Verify animations work well on different devices
6. **Provide Escape Routes**: Always provide ways to close the sheet
7. **Consider Mobile**: Test drag-to-close on touch devices

## See Also

- [FluentResizable](FluentResizable.md) - For resizable container components
- [FluentScrollArea](FluentScrollArea.md) - For scrollable content areas
- [FluentCard](FluentCard.md) - For container components with elevation
