# FluentCarousel Component Documentation

## Overview

The FluentCarousel component is a versatile, accessible carousel implementation for the Element Fluent UI library. It provides a foundation for displaying multiple items in a rotating fashion with smooth transitions, keyboard navigation, and comprehensive accessibility support.

## Features

- **Multiple Transition Effects**: Slide, Fade, Scale, Flip, Cube, Coverflow, and more
- **Flexible Orientation**: Horizontal and vertical layouts
- **Auto-play Support**: Configurable automatic progression with pause/resume
- **Keyboard Navigation**: Full keyboard accessibility with arrow keys, Home/End, Page Up/Down
- **Touch/Gesture Support**: Swipe gestures and momentum scrolling
- **Indicator Systems**: Dots, lines, numbers, thumbnails, and progress indicators
- **Responsive Design**: Adapts to different screen sizes and orientations
- **Theme Integration**: Seamless integration with Fluent Design System
- **Accessibility**: ARIA labels, screen reader support, high contrast mode

## Basic Usage

### Simple Carousel

```cpp
#include "FluentQt/Components/FluentCarousel.h"

// Create a basic carousel
auto* carousel = new FluentCarousel(this);

// Add items
auto* item1 = new QLabel("Slide 1");
auto* item2 = new QLabel("Slide 2");
auto* item3 = new QLabel("Slide 3");

carousel->addItem(item1);
carousel->addItem(item2);
carousel->addItem(item3);

// Configure basic properties
carousel->setTransition(FluentCarouselTransition::Slide);
carousel->setTransitionDuration(300);
carousel->setWrapAround(true);

// Show the carousel
carousel->show();
```

### Configuration-based Setup

```cpp
#include "FluentQt/Components/FluentCarousel.h"

// Create configuration
FluentCarouselConfig config;
config.orientation = FluentCarouselOrientation::Horizontal;
config.transition = FluentCarouselTransition::Fade;
config.transitionDuration = 500;
config.autoPlay = FluentCarouselAutoPlay::Enabled;
config.autoPlayInterval = 3000;
config.wrapAround = true;
config.showNavigationButtons = true;
config.keyboardNavigation = true;

// Create carousel with configuration
auto* carousel = new FluentCarousel(config, this);

// Add content
for (int i = 0; i < 5; ++i) {
    auto* widget = new QWidget();
    widget->setStyleSheet(QString("background-color: hsl(%1, 70%, 50%);").arg(i * 72));
    
    auto* label = new QLabel(QString("Item %1").arg(i + 1), widget);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    
    carousel->addItem(widget);
}
```

## API Reference

### Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `itemCount` | `int` | `0` | Number of items in the carousel (read-only) |
| `currentIndex` | `int` | `-1` | Index of the currently displayed item |
| `orientation` | `FluentCarouselOrientation` | `Horizontal` | Layout orientation |
| `transition` | `FluentCarouselTransition` | `Slide` | Transition effect between items |
| `transitionDuration` | `int` | `300` | Duration of transitions in milliseconds |
| `autoPlayEnabled` | `bool` | `false` | Whether auto-play is enabled |
| `autoPlayInterval` | `int` | `3000` | Auto-play interval in milliseconds |
| `autoPlayActive` | `bool` | `false` | Whether auto-play is currently active (read-only) |
| `wrapAround` | `bool` | `true` | Whether navigation wraps around at edges |
| `showNavigationButtons` | `bool` | `true` | Whether to show previous/next buttons |
| `keyboardNavigation` | `bool` | `true` | Whether keyboard navigation is enabled |

### Methods

#### Item Management

```cpp
// Add items
void addItem(QWidget* item);
void insertItem(int index, QWidget* item);

// Remove items
void removeItem(QWidget* item);
void removeItemAt(int index);
void clear();

// Access items
QWidget* itemAt(int index) const;
int indexOf(QWidget* item) const;
QList<QWidget*> items() const;
```

#### Navigation

```cpp
// Basic navigation
void next();
void previous();
void goToIndex(int index);
void goToFirst();
void goToLast();

// Auto-play control
void startAutoPlay();
void stopAutoPlay();
void pauseAutoPlay();
void resumeAutoPlay();
```

#### Configuration

```cpp
// Set configuration
void setConfiguration(const FluentCarouselConfig& config);
FluentCarouselConfig configuration() const;

// Individual property setters
void setOrientation(FluentCarouselOrientation orientation);
void setTransition(FluentCarouselTransition transition);
void setTransitionDuration(int duration);
void setAutoPlayEnabled(bool enabled);
void setAutoPlayInterval(int interval);
void setWrapAround(bool wrap);
void setShowNavigationButtons(bool show);
void setKeyboardNavigation(bool enabled);
```

### Signals

```cpp
// Item changes
void currentIndexChanged(int index);
void itemChanged(QWidget* current, QWidget* previous);
void itemAdded(QWidget* item, int index);
void itemRemoved(QWidget* item, int index);

// Transitions
void transitionStarted(FluentCarouselTransition transition);
void transitionFinished();

// Auto-play
void autoPlayStarted();
void autoPlayStopped();
void autoPlayPaused();
void autoPlayResumed();
void autoPlayProgressChanged(qreal progress);

// Configuration
void configurationChanged(const FluentCarouselConfig& config);

// Accessibility
void accessibilityUpdateRequested(const QString& announcement);
```

## Enumerations

### FluentCarouselOrientation

```cpp
enum class FluentCarouselOrientation {
    Horizontal,  // Items arranged horizontally (default)
    Vertical     // Items arranged vertically
};
```

### FluentCarouselTransition

```cpp
enum class FluentCarouselTransition {
    None,        // No transition
    Slide,       // Slide transition (default)
    Fade,        // Fade in/out
    Scale,       // Scale up/down
    Flip,        // 3D flip effect
    Cube,        // 3D cube rotation
    Coverflow,   // Cover flow effect
    Push,        // Push effect
    Reveal,      // Reveal effect
    Custom       // Custom transition
};
```

### FluentCarouselAutoPlay

```cpp
enum class FluentCarouselAutoPlay {
    Disabled,    // Auto-play disabled
    Enabled,     // Auto-play enabled
    PauseOnHover // Auto-play with pause on hover
};
```

## Configuration Structure

```cpp
struct FluentCarouselConfig {
    // Layout and appearance
    FluentCarouselOrientation orientation = FluentCarouselOrientation::Horizontal;
    FluentCarouselAlignment alignment = FluentCarouselAlignment::Center;
    
    // Transitions
    FluentCarouselTransition transition = FluentCarouselTransition::Slide;
    int transitionDuration = 300;
    QString customTransitionEasing = "cubic-bezier(0.25, 0.46, 0.45, 0.94)";
    
    // Auto-play
    FluentCarouselAutoPlay autoPlay = FluentCarouselAutoPlay::Disabled;
    int autoPlayInterval = 3000;
    bool pauseOnHover = true;
    bool pauseOnFocus = true;
    
    // Navigation
    bool wrapAround = true;
    bool showNavigationButtons = true;
    FluentCarouselNavigationStyle navigationStyle = FluentCarouselNavigationStyle::Buttons;
    
    // Interaction
    bool keyboardNavigation = true;
    bool mouseNavigation = true;
    bool touchNavigation = true;
    FluentCarouselGestureSensitivity gestureSensitivity = FluentCarouselGestureSensitivity::Medium;
    
    // Accessibility
    bool announceChanges = true;
    QString ariaLabel = "Carousel";
    QString ariaDescription = "Use arrow keys to navigate";
    
    // Performance
    bool lazyLoading = false;
    int preloadDistance = 1;
    bool hardwareAcceleration = true;
};
```

## Examples

### Image Gallery Carousel

```cpp
#include "FluentQt/Components/FluentCarousel.h"
#include <QPixmap>
#include <QLabel>

class ImageGalleryCarousel : public FluentCarousel {
    Q_OBJECT

public:
    explicit ImageGalleryCarousel(QWidget* parent = nullptr)
        : FluentCarousel(parent) {
        
        // Configure for image display
        FluentCarouselConfig config;
        config.transition = FluentCarouselTransition::Fade;
        config.transitionDuration = 500;
        config.autoPlay = FluentCarouselAutoPlay::Enabled;
        config.autoPlayInterval = 4000;
        config.pauseOnHover = true;
        
        setConfiguration(config);
        setFixedSize(600, 400);
    }
    
    void addImage(const QString& imagePath, const QString& caption = QString()) {
        auto* container = new QWidget();
        container->setFixedSize(600, 400);
        
        auto* imageLabel = new QLabel(container);
        QPixmap pixmap(imagePath);
        imageLabel->setPixmap(pixmap.scaled(600, 350, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setGeometry(0, 0, 600, 350);
        
        if (!caption.isEmpty()) {
            auto* captionLabel = new QLabel(caption, container);
            captionLabel->setAlignment(Qt::AlignCenter);
            captionLabel->setGeometry(0, 350, 600, 50);
            captionLabel->setStyleSheet("background-color: rgba(0,0,0,0.7); color: white; padding: 10px;");
        }
        
        addItem(container);
    }
};

// Usage
auto* gallery = new ImageGalleryCarousel(this);
gallery->addImage(":/images/photo1.jpg", "Beautiful sunset");
gallery->addImage(":/images/photo2.jpg", "Mountain landscape");
gallery->addImage(":/images/photo3.jpg", "Ocean waves");
```

### Product Showcase Carousel

```cpp
#include "FluentQt/Components/FluentCarousel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

class ProductShowcaseCarousel : public FluentCarousel {
    Q_OBJECT

public:
    explicit ProductShowcaseCarousel(QWidget* parent = nullptr)
        : FluentCarousel(parent) {
        
        FluentCarouselConfig config;
        config.transition = FluentCarouselTransition::Scale;
        config.transitionDuration = 400;
        config.wrapAround = false;
        config.showNavigationButtons = true;
        
        setConfiguration(config);
        setFixedSize(800, 500);
    }
    
    void addProduct(const QString& name, const QString& description, 
                   const QString& price, const QString& imagePath) {
        auto* productWidget = new QWidget();
        productWidget->setFixedSize(800, 500);
        
        auto* layout = new QVBoxLayout(productWidget);
        
        // Product image
        auto* imageLabel = new QLabel();
        QPixmap productImage(imagePath);
        imageLabel->setPixmap(productImage.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->setAlignment(Qt::AlignCenter);
        
        // Product info
        auto* nameLabel = new QLabel(name);
        nameLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #323130;");
        nameLabel->setAlignment(Qt::AlignCenter);
        
        auto* descLabel = new QLabel(description);
        descLabel->setStyleSheet("font-size: 14px; color: #605e5c; margin: 10px;");
        descLabel->setAlignment(Qt::AlignCenter);
        descLabel->setWordWrap(true);
        
        auto* priceLabel = new QLabel(price);
        priceLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #0078d4;");
        priceLabel->setAlignment(Qt::AlignCenter);
        
        // Action buttons
        auto* buttonLayout = new QHBoxLayout();
        auto* buyButton = new QPushButton("Buy Now");
        auto* detailsButton = new QPushButton("View Details");
        
        buyButton->setStyleSheet("QPushButton { background-color: #0078d4; color: white; padding: 10px 20px; border: none; border-radius: 4px; }");
        detailsButton->setStyleSheet("QPushButton { background-color: #f3f2f1; color: #323130; padding: 10px 20px; border: 1px solid #c8c6c4; border-radius: 4px; }");
        
        buttonLayout->addWidget(buyButton);
        buttonLayout->addWidget(detailsButton);
        buttonLayout->setAlignment(Qt::AlignCenter);
        
        // Add to layout
        layout->addWidget(imageLabel);
        layout->addWidget(nameLabel);
        layout->addWidget(descLabel);
        layout->addWidget(priceLabel);
        layout->addLayout(buttonLayout);
        layout->setAlignment(Qt::AlignCenter);
        
        addItem(productWidget);
    }
};

// Usage
auto* showcase = new ProductShowcaseCarousel(this);
showcase->addProduct("Wireless Headphones", 
                    "Premium noise-cancelling headphones with 30-hour battery life",
                    "$299.99", ":/products/headphones.jpg");
showcase->addProduct("Smart Watch", 
                    "Advanced fitness tracking with heart rate monitoring",
                    "$399.99", ":/products/smartwatch.jpg");
```

## Accessibility

The FluentCarousel component provides comprehensive accessibility support:

### ARIA Attributes

- `role="tablist"` - Identifies the carousel as a tab list
- `aria-label` - Provides accessible name
- `aria-describedby` - Links to description
- `aria-live="polite"` - Announces changes to screen readers
- `aria-current="true"` - Identifies current item

### Keyboard Navigation

| Key | Action |
|-----|--------|
| `Left Arrow` / `Up Arrow` | Previous item |
| `Right Arrow` / `Down Arrow` | Next item |
| `Home` | First item |
| `End` | Last item |
| `Page Up` | Previous item (alternative) |
| `Page Down` | Next item (alternative) |
| `Space` / `Enter` | Activate current item |
| `Escape` | Stop auto-play |

### Screen Reader Support

```cpp
// Configure accessibility
carousel->setAccessibleName("Product Gallery");
carousel->setAccessibleDescription("Use arrow keys to browse products");

// Custom announcements
connect(carousel, &FluentCarousel::itemChanged, [](QWidget* current, QWidget* previous) {
    QString announcement = QString("Showing item %1 of %2").arg(index + 1).arg(total);
    // Announce to screen readers
});
```

## Styling and Theming

The carousel integrates seamlessly with the Fluent Design System:

```cpp
// Apply custom styling
carousel->setProperty("fluentStyle", "accent");
carousel->setProperty("fluentElevation", 2);

// Respond to theme changes
connect(&FluentTheme::instance(), &FluentTheme::themeChanged, [carousel]() {
    carousel->updateTheme();
});
```

## Performance Considerations

### Lazy Loading

```cpp
FluentCarouselConfig config;
config.lazyLoading = true;
config.preloadDistance = 2; // Preload 2 items in each direction
carousel->setConfiguration(config);
```

### Hardware Acceleration

```cpp
// Enable hardware acceleration for smooth transitions
config.hardwareAcceleration = true;
carousel->setConfiguration(config);
```

### Memory Management

```cpp
// Automatic cleanup when items are removed
carousel->removeItemAt(0); // Item is automatically deleted

// Manual cleanup
carousel->clear(); // All items are deleted
```

## Best Practices

1. **Item Sizing**: Ensure all carousel items have consistent dimensions
2. **Content Loading**: Use lazy loading for large datasets
3. **Accessibility**: Always provide meaningful labels and descriptions
4. **Performance**: Limit the number of items for optimal performance
5. **User Experience**: Provide clear navigation indicators
6. **Responsive Design**: Test on different screen sizes and orientations

## Troubleshooting

### Common Issues

**Items not displaying correctly**
- Ensure items have proper size constraints
- Check parent widget layout

**Transitions not smooth**
- Verify hardware acceleration is enabled
- Reduce transition duration for better performance

**Keyboard navigation not working**
- Ensure the carousel has focus
- Check that keyboard navigation is enabled

**Auto-play not functioning**
- Verify auto-play is enabled in configuration
- Check that there are multiple items in the carousel

## See Also

- [FluentBasicCarousel](FluentBasicCarousel.md) - Basic carousel with navigation buttons
- [FluentAutoCarousel](FluentAutoCarousel.md) - Auto-playing carousel with controls
- [FluentIndicatorCarousel](FluentIndicatorCarousel.md) - Carousel with indicator dots
- [FluentTouchCarousel](FluentTouchCarousel.md) - Touch-enabled carousel with gestures
- [Fluent Design System](../design-system/README.md) - Design principles and guidelines
