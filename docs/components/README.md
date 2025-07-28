# FluentCarousel Components

## Overview

The FluentCarousel component family provides a comprehensive set of carousel implementations for the Element Fluent UI library. These components follow Microsoft's Fluent Design System principles and offer smooth animations, accessibility features, and responsive design.

## Components

### 🎠 [FluentCarousel](FluentCarousel.md)
**Base carousel component with comprehensive functionality**

- Multiple transition effects (Slide, Fade, Scale, Flip, Cube, Coverflow)
- Keyboard navigation and accessibility support
- Auto-play with configurable timing
- Touch/gesture support
- Flexible orientation (horizontal/vertical)
- Theme integration

```cpp
#include "FluentQt/Components/FluentCarousel.h"

auto* carousel = new FluentCarousel(this);
carousel->setTransition(FluentCarouselTransition::Slide);
carousel->addItem(new QLabel("Item 1"));
carousel->addItem(new QLabel("Item 2"));
```

### 🔄 [FluentBasicCarousel](FluentCarouselVariants.md#fluentbasiccarousel)
**Simple carousel with navigation buttons**

- Previous/Next navigation buttons
- Button state management
- Clean, minimal interface
- Perfect for basic content browsing

```cpp
#include "FluentQt/Components/FluentBasicCarousel.h"

auto* basicCarousel = new FluentBasicCarousel(this);
basicCarousel->setShowNavigationButtons(true);
basicCarousel->setWrapAround(false);
```

### ⏯️ [FluentAutoCarousel](FluentCarouselVariants.md#fluentautocarousel)
**Auto-playing carousel with controls**

- Automatic content progression
- Play/pause controls
- Progress indicators (linear and circular)
- Pause on hover/focus
- Configurable timing

```cpp
#include "FluentQt/Components/FluentAutoCarousel.h"

auto* autoCarousel = new FluentAutoCarousel(this);
autoCarousel->setAutoPlayInterval(3000);
autoCarousel->setPauseOnHover(true);
autoCarousel->startAutoPlay();
```

### 🔘 [FluentIndicatorCarousel](FluentCarouselVariants.md#fluentindicatorcarousel)
**Carousel with visual position indicators**

- Multiple indicator styles (dots, lines, numbers, thumbnails)
- Clickable indicators for direct navigation
- Flexible positioning
- Custom styling support

```cpp
#include "FluentQt/Components/FluentIndicatorCarousel.h"

auto* indicatorCarousel = new FluentIndicatorCarousel(this);
indicatorCarousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Dots);
indicatorCarousel->setIndicatorPosition(FluentCarouselIndicatorPosition::Bottom);
```

### 👆 [FluentTouchCarousel](FluentCarouselVariants.md#fluenttouchcarousel)
**Touch-optimized carousel with gestures**

- Swipe gesture recognition
- Momentum scrolling with physics
- Edge resistance and bounce effects
- Multi-touch support
- Configurable sensitivity

```cpp
#include "FluentQt/Components/FluentTouchCarousel.h"

auto* touchCarousel = new FluentTouchCarousel(this);
FluentCarouselTouchConfig config;
config.gestureSensitivity = FluentCarouselGestureSensitivity::High;
config.momentumScrolling = true;
touchCarousel->setTouchConfiguration(config);
```

## Quick Start

### 1. Installation

Add to your CMakeLists.txt:
```cmake
find_package(FluentQt REQUIRED)
target_link_libraries(your_target FluentQt::FluentQt)
```

### 2. Basic Usage

```cpp
#include "FluentQt/Components/FluentCarousel.h"

// Create carousel
auto* carousel = new FluentCarousel(this);

// Add items
for (int i = 0; i < 5; ++i) {
    auto* item = new QLabel(QString("Slide %1").arg(i + 1));
    item->setAlignment(Qt::AlignCenter);
    carousel->addItem(item);
}

// Configure
carousel->setTransition(FluentCarouselTransition::Slide);
carousel->setAutoPlayEnabled(true);
carousel->setAutoPlayInterval(3000);

// Show
carousel->show();
```

### 3. Run the Showcase

Build and run the comprehensive example:
```bash
mkdir build && cd build
cmake ..
make CarouselShowcase
./CarouselShowcase
```

## Features

### 🎨 **Rich Transitions**
- **Slide**: Classic sliding transition
- **Fade**: Smooth fade in/out
- **Scale**: Scale up/down effect
- **Flip**: 3D flip animation
- **Cube**: 3D cube rotation
- **Coverflow**: Cover flow effect

### ♿ **Accessibility**
- Full keyboard navigation
- Screen reader support
- ARIA labels and descriptions
- High contrast mode support
- Focus management

### 📱 **Responsive Design**
- Adapts to different screen sizes
- Touch-friendly on mobile devices
- Configurable breakpoints
- Flexible layouts

### 🎯 **Performance**
- Hardware acceleration
- Lazy loading support
- Memory efficient
- Smooth 60fps animations

### 🎨 **Theming**
- Fluent Design System integration
- Dark/light mode support
- Custom styling support
- Design tokens

## Use Cases

### 📸 **Image Galleries**
```cpp
auto* gallery = new FluentAutoCarousel(this);
gallery->setTransition(FluentCarouselTransition::Fade);
gallery->setAutoPlayInterval(4000);

for (const QString& imagePath : imagePaths) {
    auto* imageLabel = new QLabel();
    imageLabel->setPixmap(QPixmap(imagePath));
    gallery->addItem(imageLabel);
}
```

### 🛍️ **Product Showcases**
```cpp
auto* showcase = new FluentIndicatorCarousel(this);
showcase->setIndicatorStyle(FluentCarouselIndicatorStyle::Thumbnails);
showcase->setTransition(FluentCarouselTransition::Scale);

for (const Product& product : products) {
    auto* productWidget = createProductWidget(product);
    showcase->addItem(productWidget);
    showcase->setItemThumbnail(showcase->itemCount() - 1, product.thumbnail);
}
```

### 📰 **Content Sliders**
```cpp
auto* slider = new FluentBasicCarousel(this);
slider->setTransition(FluentCarouselTransition::Slide);
slider->setWrapAround(true);

for (const Article& article : articles) {
    auto* articleCard = createArticleCard(article);
    slider->addItem(articleCard);
}
```

### 📱 **Mobile Interfaces**
```cpp
auto* mobileCarousel = new FluentTouchCarousel(this);
FluentCarouselTouchConfig config;
config.gestureSensitivity = FluentCarouselGestureSensitivity::High;
config.momentumScrolling = true;
config.edgeBehavior = FluentCarouselEdgeBehavior::Bounce;
mobileCarousel->setTouchConfiguration(config);
```

## Architecture

### Component Hierarchy
```
FluentComponent (base)
└── FluentCarousel (core functionality)
    ├── FluentBasicCarousel (navigation buttons)
    ├── FluentAutoCarousel (auto-play + progress)
    ├── FluentIndicatorCarousel (position indicators)
    └── FluentTouchCarousel (touch gestures)
```

### Key Classes
- **FluentCarousel**: Base carousel with core functionality
- **FluentCarouselConfig**: Configuration structure
- **FluentCarouselStyles**: Styling and theming system
- **FluentCarouselIndicators**: Indicator management
- **FluentCarouselTouchConfig**: Touch behavior configuration

## Configuration

### Basic Configuration
```cpp
FluentCarouselConfig config;
config.orientation = FluentCarouselOrientation::Horizontal;
config.transition = FluentCarouselTransition::Slide;
config.transitionDuration = 300;
config.wrapAround = true;
config.keyboardNavigation = true;

carousel->setConfiguration(config);
```

### Auto-Play Configuration
```cpp
config.autoPlay = FluentCarouselAutoPlay::Enabled;
config.autoPlayInterval = 3000;
config.pauseOnHover = true;
config.pauseOnFocus = true;
```

### Touch Configuration
```cpp
FluentCarouselTouchConfig touchConfig;
touchConfig.swipeThreshold = 100;
touchConfig.gestureSensitivity = FluentCarouselGestureSensitivity::Medium;
touchConfig.momentumScrolling = true;
touchConfig.edgeBehavior = FluentCarouselEdgeBehavior::Resistance;
```

## Testing

Run the comprehensive test suite:
```bash
cd build
make FluentCarouselTest FluentCarouselVariantsTest FluentCarouselStylingTest
./FluentCarouselTest
./FluentCarouselVariantsTest
./FluentCarouselStylingTest
```

## Documentation

- **[API Reference](FluentCarousel.md)** - Complete API documentation
- **[Variants Guide](FluentCarouselVariants.md)** - Specialized carousel types
- **[Integration Guide](../guides/CarouselIntegrationGuide.md)** - Integration patterns and best practices
- **[Styling Guide](../styling/CarouselStyling.md)** - Theming and customization

## Examples

- **[Basic Example](../../examples/CarouselShowcaseExample.cpp)** - Comprehensive showcase
- **[Integration Examples](../guides/CarouselIntegrationGuide.md#examples)** - Real-world usage patterns

## Contributing

1. Follow the [Element Fluent UI coding standards](../DEVELOPER_GUIDE.md)
2. Add comprehensive tests for new features
3. Update documentation for API changes
4. Ensure accessibility compliance
5. Test on multiple platforms and screen sizes

## License

Element Fluent UI is licensed under the MIT License. See [LICENSE](../../LICENSE) for details.

---

**Element Fluent UI** - Modern C++ Qt6 library implementing Microsoft's Fluent Design System
