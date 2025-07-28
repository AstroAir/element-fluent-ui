# FluentCarousel Variants Documentation

## Overview

Element Fluent UI provides four specialized carousel variants, each optimized for specific use cases while maintaining the core functionality of the base FluentCarousel component.

## FluentBasicCarousel

### Description
A simple carousel with navigation buttons, perfect for basic content browsing with clear previous/next controls.

### Features
- Previous/Next navigation buttons
- Keyboard navigation support
- Button state management (enabled/disabled at edges)
- Customizable button styles and positioning

### Usage

```cpp
#include "FluentQt/Components/FluentBasicCarousel.h"

// Create basic carousel
auto* basicCarousel = new FluentBasicCarousel(this);

// Add content
for (int i = 0; i < 5; ++i) {
    auto* item = new QLabel(QString("Slide %1").arg(i + 1));
    item->setAlignment(Qt::AlignCenter);
    item->setStyleSheet("background-color: lightblue; padding: 20px;");
    basicCarousel->addItem(item);
}

// Configure navigation
basicCarousel->setShowNavigationButtons(true);
basicCarousel->setWrapAround(false); // Disable buttons at edges

// Connect to navigation signals
connect(basicCarousel, &FluentBasicCarousel::nextButtonClicked, []() {
    qDebug() << "Next button clicked";
});

connect(basicCarousel, &FluentBasicCarousel::previousButtonClicked, []() {
    qDebug() << "Previous button clicked";
});
```

### API Reference

#### Additional Properties
- `showNavigationButtons` (bool) - Show/hide navigation buttons
- `navigationButtonSize` (QSize) - Size of navigation buttons
- `navigationButtonPosition` (FluentCarouselNavigationPosition) - Button positioning

#### Additional Signals
```cpp
void nextButtonClicked();
void previousButtonClicked();
void navigationButtonHovered(bool next);
```

---

## FluentAutoCarousel

### Description
An auto-playing carousel with play/pause controls and progress indicators, ideal for showcasing content that should automatically advance.

### Features
- Automatic content progression
- Play/pause controls
- Progress indicators (linear and circular)
- Pause on hover/focus
- Configurable timing and behavior

### Usage

```cpp
#include "FluentQt/Components/FluentAutoCarousel.h"

// Create auto carousel
auto* autoCarousel = new FluentAutoCarousel(this);

// Configure auto-play
autoCarousel->setAutoPlayInterval(3000); // 3 seconds
autoCarousel->setPauseOnHover(true);
autoCarousel->setShowPlayPauseButton(true);
autoCarousel->setShowProgressIndicator(true);

// Add content
QStringList images = {":/images/slide1.jpg", ":/images/slide2.jpg", ":/images/slide3.jpg"};
for (const QString& imagePath : images) {
    auto* imageLabel = new QLabel();
    QPixmap pixmap(imagePath);
    imageLabel->setPixmap(pixmap.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setAlignment(Qt::AlignCenter);
    autoCarousel->addItem(imageLabel);
}

// Start auto-play
autoCarousel->startAutoPlay();

// Connect to auto-play signals
connect(autoCarousel, &FluentAutoCarousel::autoPlayProgressChanged, [](qreal progress) {
    qDebug() << "Auto-play progress:" << progress;
});

connect(autoCarousel, &FluentAutoCarousel::playButtonClicked, []() {
    qDebug() << "Play button clicked";
});
```

### API Reference

#### Additional Properties
- `showPlayPauseButton` (bool) - Show/hide play/pause control
- `showProgressIndicator` (bool) - Show/hide progress indicator
- `pauseOnHover` (bool) - Pause auto-play on hover
- `pauseOnFocus` (bool) - Pause auto-play on focus
- `progressIndicatorStyle` (FluentCarouselProgressStyle) - Progress indicator style

#### Additional Signals
```cpp
void playButtonClicked();
void pauseButtonClicked();
void autoPlayProgressChanged(qreal progress);
```

---

## FluentIndicatorCarousel

### Description
A carousel with visual indicators showing the current position and allowing direct navigation to any item.

### Features
- Multiple indicator styles (dots, lines, numbers, thumbnails)
- Clickable indicators for direct navigation
- Flexible indicator positioning
- Custom indicator styling and animations

### Usage

```cpp
#include "FluentQt/Components/FluentIndicatorCarousel.h"

// Create indicator carousel
auto* indicatorCarousel = new FluentIndicatorCarousel(this);

// Configure indicators
indicatorCarousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Dots);
indicatorCarousel->setIndicatorPosition(FluentCarouselIndicatorPosition::Bottom);
indicatorCarousel->setShowIndicators(true);

// Add content with thumbnails for thumbnail indicators
QStringList products = {"Product A", "Product B", "Product C", "Product D"};
for (int i = 0; i < products.size(); ++i) {
    auto* productWidget = new QWidget();
    auto* layout = new QVBoxLayout(productWidget);
    
    auto* titleLabel = new QLabel(products[i]);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    
    auto* descLabel = new QLabel(QString("Description for %1").arg(products[i]));
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    
    layout->addWidget(titleLabel);
    layout->addWidget(descLabel);
    
    indicatorCarousel->addItem(productWidget);
    
    // Add thumbnail for this item
    QPixmap thumbnail = createThumbnail(products[i]); // Your thumbnail creation function
    indicatorCarousel->setItemThumbnail(i, thumbnail);
}

// Connect to indicator signals
connect(indicatorCarousel, &FluentIndicatorCarousel::indicatorClicked, [](int index) {
    qDebug() << "Indicator clicked for item:" << index;
});

connect(indicatorCarousel, &FluentIndicatorCarousel::indicatorHovered, [](int index, bool hovered) {
    qDebug() << "Indicator" << index << (hovered ? "hovered" : "unhovered");
});
```

### Indicator Styles

#### Dots
```cpp
indicatorCarousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Dots);
// Small circular indicators
```

#### Lines
```cpp
indicatorCarousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Lines);
// Horizontal line indicators that expand when active
```

#### Numbers
```cpp
indicatorCarousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Numbers);
// Numbered indicators showing item position
```

#### Thumbnails
```cpp
indicatorCarousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Thumbnails);
// Small preview images of each item
```

#### Progress
```cpp
indicatorCarousel->setIndicatorStyle(FluentCarouselIndicatorStyle::Progress);
// Progress bars showing completion for each item
```

### API Reference

#### Additional Properties
- `indicatorStyle` (FluentCarouselIndicatorStyle) - Style of indicators
- `indicatorPosition` (FluentCarouselIndicatorPosition) - Position of indicators
- `showIndicators` (bool) - Show/hide indicators
- `indicatorSpacing` (int) - Spacing between indicators
- `indicatorSize` (QSize) - Size of indicators

#### Additional Methods
```cpp
void setItemThumbnail(int index, const QPixmap& thumbnail);
QPixmap itemThumbnail(int index) const;
void setIndicatorText(int index, const QString& text);
QString indicatorText(int index) const;
```

#### Additional Signals
```cpp
void indicatorClicked(int index);
void indicatorHovered(int index, bool hovered);
void indicatorChanged(int index);
```

---

## FluentTouchCarousel

### Description
A touch-enabled carousel optimized for mobile and touch devices with gesture support and momentum scrolling.

### Features
- Swipe gesture recognition
- Pan gesture support
- Momentum scrolling with physics
- Edge resistance and bounce effects
- Multi-touch support
- Configurable gesture sensitivity

### Usage

```cpp
#include "FluentQt/Components/FluentTouchCarousel.h"

// Create touch carousel
auto* touchCarousel = new FluentTouchCarousel(this);

// Configure touch behavior
FluentCarouselTouchConfig touchConfig;
touchConfig.swipeThreshold = 50;
touchConfig.gestureSensitivity = FluentCarouselGestureSensitivity::High;
touchConfig.momentumScrolling = true;
touchConfig.edgeBehavior = FluentCarouselEdgeBehavior::Resistance;
touchConfig.hapticFeedback = true;

touchCarousel->setTouchConfiguration(touchConfig);

// Add content optimized for touch
for (int i = 0; i < 6; ++i) {
    auto* touchItem = new QWidget();
    touchItem->setFixedSize(300, 200);
    touchItem->setStyleSheet(QString("background-color: hsl(%1, 70%, 60%); border-radius: 8px;").arg(i * 60));
    
    auto* label = new QLabel(QString("Touch Item %1\nSwipe to navigate").arg(i + 1), touchItem);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    label->setGeometry(touchItem->rect());
    
    touchCarousel->addItem(touchItem);
}

// Connect to touch signals
connect(touchCarousel, &FluentTouchCarousel::swipeDetected, [](QSwipeGesture::SwipeDirection direction) {
    qDebug() << "Swipe detected:" << direction;
});

connect(touchCarousel, &FluentTouchCarousel::panGestureChanged, [](qreal delta, qreal velocity) {
    qDebug() << "Pan gesture - Delta:" << delta << "Velocity:" << velocity;
});

connect(touchCarousel, &FluentTouchCarousel::momentumScrollStarted, []() {
    qDebug() << "Momentum scrolling started";
});
```

### Touch Configuration

```cpp
struct FluentCarouselTouchConfig {
    // Gesture thresholds
    int swipeThreshold = 100;           // Minimum distance for swipe
    int panThreshold = 10;              // Minimum distance for pan
    qreal velocityThreshold = 500.0;    // Minimum velocity for momentum
    
    // Sensitivity
    FluentCarouselGestureSensitivity gestureSensitivity = FluentCarouselGestureSensitivity::Medium;
    
    // Behavior
    bool momentumScrolling = true;      // Enable momentum scrolling
    FluentCarouselEdgeBehavior edgeBehavior = FluentCarouselEdgeBehavior::Stop;
    bool multiTouchEnabled = true;      // Enable multi-touch gestures
    
    // Feedback
    bool hapticFeedback = false;        // Enable haptic feedback
    bool visualFeedback = true;         // Enable visual feedback
    
    // Physics
    qreal friction = 0.95;              // Momentum friction
    qreal elasticity = 0.3;             // Edge elasticity
    int maxMomentumDuration = 2000;     // Maximum momentum duration
};
```

### API Reference

#### Additional Properties
- `touchConfiguration` (FluentCarouselTouchConfig) - Touch behavior configuration
- `gestureEnabled` (bool) - Enable/disable gesture recognition
- `momentumScrolling` (bool) - Enable momentum scrolling
- `edgeBehavior` (FluentCarouselEdgeBehavior) - Behavior at edges

#### Additional Methods
```cpp
void setTouchConfiguration(const FluentCarouselTouchConfig& config);
FluentCarouselTouchConfig touchConfiguration() const;
void enableGesture(Qt::GestureType type, bool enabled = true);
bool isGestureEnabled(Qt::GestureType type) const;
```

#### Additional Signals
```cpp
void swipeDetected(QSwipeGesture::SwipeDirection direction);
void panGestureStarted(const QPointF& startPos);
void panGestureChanged(qreal delta, qreal velocity);
void panGestureFinished(qreal finalVelocity);
void momentumScrollStarted();
void momentumScrollFinished();
void edgeReached(bool leftEdge, bool rightEdge);
```

---

## Combining Variants

You can combine features from different variants by using multiple inheritance or composition:

### Auto-playing Carousel with Indicators

```cpp
class AutoIndicatorCarousel : public FluentAutoCarousel {
    Q_OBJECT

public:
    explicit AutoIndicatorCarousel(QWidget* parent = nullptr)
        : FluentAutoCarousel(parent) {
        
        // Enable both auto-play and indicators
        setAutoPlayInterval(4000);
        setPauseOnHover(true);
        
        // Add indicator functionality
        m_indicators = new FluentCarouselIndicators(this);
        m_indicators->setStyle(FluentCarouselIndicatorStyle::Dots);
        
        // Connect signals
        connect(this, &FluentCarousel::currentIndexChanged,
                m_indicators, &FluentCarouselIndicators::setCurrentIndex);
        connect(m_indicators, &FluentCarouselIndicators::indicatorClicked,
                this, &FluentCarousel::goToIndex);
    }

private:
    FluentCarouselIndicators* m_indicators;
};
```

### Touch Carousel with Navigation Buttons

```cpp
class TouchNavigationCarousel : public FluentTouchCarousel {
    Q_OBJECT

public:
    explicit TouchNavigationCarousel(QWidget* parent = nullptr)
        : FluentTouchCarousel(parent) {
        
        // Enable touch gestures
        FluentCarouselTouchConfig touchConfig;
        touchConfig.gestureSensitivity = FluentCarouselGestureSensitivity::High;
        touchConfig.momentumScrolling = true;
        setTouchConfiguration(touchConfig);
        
        // Add navigation buttons for non-touch users
        setShowNavigationButtons(true);
    }
};
```

## Best Practices

### Choosing the Right Variant

1. **FluentBasicCarousel**: Use for simple content browsing where users control navigation
2. **FluentAutoCarousel**: Use for promotional content, slideshows, or when automatic progression is desired
3. **FluentIndicatorCarousel**: Use when users need to see their position and jump to specific items
4. **FluentTouchCarousel**: Use for mobile applications or touch-first interfaces

### Performance Optimization

- Use lazy loading for large datasets
- Optimize images and content for the target platform
- Consider using thumbnails for indicator carousels
- Enable hardware acceleration for smooth touch interactions

### Accessibility Considerations

- Provide alternative navigation methods for touch-only carousels
- Ensure auto-playing carousels can be paused
- Use appropriate ARIA labels for indicators
- Test with screen readers and keyboard navigation

### Responsive Design

```cpp
// Adapt carousel behavior based on screen size
void adaptToScreenSize(FluentCarousel* carousel, const QSize& screenSize) {
    if (screenSize.width() < 768) {
        // Mobile: Enable touch, hide buttons
        if (auto* touchCarousel = qobject_cast<FluentTouchCarousel*>(carousel)) {
            touchCarousel->setGestureEnabled(Qt::SwipeGesture, true);
        }
        carousel->setShowNavigationButtons(false);
    } else {
        // Desktop: Show buttons, reduce touch sensitivity
        carousel->setShowNavigationButtons(true);
        if (auto* touchCarousel = qobject_cast<FluentTouchCarousel*>(carousel)) {
            FluentCarouselTouchConfig config = touchCarousel->touchConfiguration();
            config.gestureSensitivity = FluentCarouselGestureSensitivity::Low;
            touchCarousel->setTouchConfiguration(config);
        }
    }
}
```

## See Also

- [FluentCarousel Base Documentation](FluentCarousel.md)
- [Carousel Styling Guide](../styling/CarouselStyling.md)
- [Accessibility Guidelines](../accessibility/README.md)
- [Touch and Gesture Guidelines](../interaction/TouchGestures.md)
