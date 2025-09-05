# FluentBadge Component

The `FluentBadge` component is a versatile status indicator widget that implements Microsoft's Fluent Design principles. It provides visual indicators for notifications, status, counts, and other contextual information with flexible positioning and styling options.

## Overview

FluentBadge offers:
- **Multiple badge types** (Count, Text, Icon, Dot, Status)
- **Flexible positioning** (TopRight, TopLeft, BottomRight, BottomLeft, Center)
- **Attachment system** for positioning relative to other widgets
- **Status indicators** with semantic colors
- **Smooth animations** for show/hide and pulse effects
- **Theme integration** with automatic color adaptation
- **Accessibility support** with proper ARIA attributes
- **Customizable styling** with size and color options

## Basic Usage

### Count Badge

```cpp
#include "FluentQt/Components/FluentBadge.h"

// Create a count badge
auto* badge = new FluentBadge();
badge->setBadgeType(FluentBadgeType::Count);
badge->setCount(5);

// Or create directly with count
auto* countBadge = new FluentBadge(12);
```

### Text Badge

```cpp
// Create a text badge
auto* textBadge = new FluentBadge("NEW");
textBadge->setBadgeType(FluentBadgeType::Text);

// Configure appearance
textBadge->setBadgeSize(FluentBadgeSize::Small);
textBadge->setBadgeStyle(FluentBadgeStyle::Filled);
```

### Icon Badge

```cpp
// Create an icon badge
auto* iconBadge = new FluentBadge();
iconBadge->setBadgeType(FluentBadgeType::Icon);
iconBadge->setIcon(QIcon(":/icons/notification.svg"));
```

## Configuration Examples

### Badge Attachment

```cpp
// Create a button with attached badge
auto* button = new QPushButton("Messages");
auto* badge = new FluentBadge(3);

// Attach badge to button
badge->attachToWidget(button);
badge->setPosition(FluentBadgePosition::TopRight);

// Badge will automatically position itself relative to the button
parentLayout->addWidget(button);  // Badge moves with button
```

### Size and Style Options

```cpp
auto* badge = new FluentBadge("URGENT");

// Configure size (XSmall, Small, Medium, Large)
badge->setBadgeSize(FluentBadgeSize::Large);

// Configure style (Filled, Outline, Ghost)
badge->setBadgeStyle(FluentBadgeStyle::Outline);

// Configure position
badge->setPosition(FluentBadgePosition::TopLeft);
```

### Count Configuration

```cpp
auto* badge = new FluentBadge();
badge->setBadgeType(FluentBadgeType::Count);

// Set count with maximum display limit
badge->setCount(150);
badge->setMaxCount(99);  // Will display "99+"

// Control zero display
badge->setShowZero(false);  // Hide when count is 0
badge->setCount(0);  // Badge will be hidden
```

## State Examples

### Status Badges

```cpp
auto* statusBadge = new FluentBadge();
statusBadge->setBadgeType(FluentBadgeType::Status);

// Set different status types
statusBadge->setStatus(FluentBadgeStatus::Success);  // Green
statusBadge->setStatus(FluentBadgeStatus::Warning);  // Yellow
statusBadge->setStatus(FluentBadgeStatus::Error);    // Red
statusBadge->setStatus(FluentBadgeStatus::Info);     // Blue
```

### Dot Badges

```cpp
// Simple dot indicator
auto* dotBadge = new FluentBadge();
dotBadge->setBadgeType(FluentBadgeType::Dot);
dotBadge->setStatus(FluentBadgeStatus::Success);

// Pulsing dot for active states
dotBadge->setPulsing(true);
```

### Dynamic Updates

```cpp
auto* badge = new FluentBadge(0);

// Update count dynamically
badge->setCount(badge->count() + 1);  // Increment
badge->setCount(qMax(0, badge->count() - 1));  // Decrement

// Update text dynamically
badge->setText("UPDATED");

// Animate changes
badge->setAnimated(true);
```

## Event Handling Examples

### Click Interactions

```cpp
auto* badge = new FluentBadge("Clickable");
badge->setClickable(true);

// Handle click events
connect(badge, &FluentBadge::clicked, [=]() {
    qDebug() << "Badge clicked!";
    // Handle badge click (e.g., show notifications)
    showNotificationPanel();
});

connect(badge, &FluentBadge::doubleClicked, [=]() {
    qDebug() << "Badge double-clicked!";
    clearNotifications();
});
```

### Animation Events

```cpp
auto* badge = new FluentBadge();

// Handle animation completion
connect(badge, &FluentBadge::showAnimationFinished, [=]() {
    qDebug() << "Badge show animation completed";
});

connect(badge, &FluentBadge::hideAnimationFinished, [=]() {
    qDebug() << "Badge hide animation completed";
    // Clean up or perform post-hide actions
});
```

## Advanced Usage

### Notification System Integration

```cpp
class NotificationBadge : public QObject {
    Q_OBJECT
    
public:
    NotificationBadge(QWidget* parent) {
        m_badge = new FluentBadge();
        m_badge->setBadgeType(FluentBadgeType::Count);
        m_badge->attachToWidget(parent);
        m_badge->setPosition(FluentBadgePosition::TopRight);
        m_badge->setAnimated(true);
    }
    
    void addNotification() {
        m_notificationCount++;
        m_badge->setCount(m_notificationCount);
        m_badge->pulse();  // Draw attention
    }
    
    void clearNotifications() {
        m_notificationCount = 0;
        m_badge->setCount(0);  // Will hide if showZero is false
    }
    
private:
    FluentBadge* m_badge;
    int m_notificationCount = 0;
};
```

### Multi-State Badge

```cpp
class StatusBadge : public QObject {
    Q_OBJECT
    
public:
    enum State { Idle, Processing, Success, Error };
    
    StatusBadge(QWidget* parent) {
        m_badge = new FluentBadge();
        m_badge->attachToWidget(parent);
        setState(Idle);
    }
    
    void setState(State state) {
        switch (state) {
            case Idle:
                m_badge->setBadgeType(FluentBadgeType::Dot);
                m_badge->setStatus(FluentBadgeStatus::None);
                m_badge->setPulsing(false);
                break;
            case Processing:
                m_badge->setBadgeType(FluentBadgeType::Text);
                m_badge->setText("...");
                m_badge->setPulsing(true);
                break;
            case Success:
                m_badge->setBadgeType(FluentBadgeType::Icon);
                m_badge->setIcon(QIcon(":/icons/check.svg"));
                m_badge->setStatus(FluentBadgeStatus::Success);
                m_badge->setPulsing(false);
                break;
            case Error:
                m_badge->setBadgeType(FluentBadgeType::Icon);
                m_badge->setIcon(QIcon(":/icons/error.svg"));
                m_badge->setStatus(FluentBadgeStatus::Error);
                m_badge->setPulsing(false);
                break;
        }
    }
    
private:
    FluentBadge* m_badge;
};
```

### Custom Positioning

```cpp
auto* badge = new FluentBadge("Custom");

// Manual positioning (not attached to widget)
badge->move(100, 50);
badge->show();

// Custom offset from attached widget
badge->attachToWidget(targetWidget);
badge->setCustomOffset(QPoint(10, -5));  // 10px right, 5px up from default position
```

## Accessibility Examples

### Screen Reader Support

```cpp
auto* badge = new FluentBadge(5);

// Set accessible properties
badge->setAccessibleName("Notification count");
badge->setAccessibleDescription("5 unread notifications");

// Update accessibility when count changes
connect(badge, &FluentBadge::countChanged, [=](int count) {
    QString description = QString("%1 unread notification%2")
                         .arg(count)
                         .arg(count == 1 ? "" : "s");
    badge->setAccessibleDescription(description);
});
```

### High Contrast Support

```cpp
auto* badge = new FluentBadge();

// Badge automatically adapts to high contrast themes
// Custom high contrast handling if needed
if (QApplication::palette().color(QPalette::Window).lightness() > 128) {
    // Light theme
    badge->setCustomColor(QColor("#0078D4"));
} else {
    // Dark theme
    badge->setCustomColor(QColor("#60CDFF"));
}
```

## Theming Examples

### Status Colors

```cpp
auto* badge = new FluentBadge();

// Use semantic status colors
badge->setStatus(FluentBadgeStatus::Success);  // Green
badge->setStatus(FluentBadgeStatus::Warning);  // Amber
badge->setStatus(FluentBadgeStatus::Error);    // Red
badge->setStatus(FluentBadgeStatus::Info);     // Blue

// Custom colors
badge->setCustomColor(QColor("#FF6B6B"));  // Custom red
```

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* badge = new FluentBadge();

// Badge automatically uses theme colors
auto& theme = FluentTheme::instance();

// Access theme colors for custom styling
QColor accentColor = theme.color("accent.primary");
badge->setCustomColor(accentColor);
```

## Real-World Scenarios

### Shopping Cart Badge

```cpp
void setupShoppingCart() {
    auto* cartButton = new QPushButton("Cart");
    auto* cartBadge = new FluentBadge(0);
    
    cartBadge->attachToWidget(cartButton);
    cartBadge->setShowZero(false);  // Hide when empty
    cartBadge->setBadgeStyle(FluentBadgeStyle::Filled);
    
    // Update when items added/removed
    connect(shoppingCart, &ShoppingCart::itemCountChanged, 
            cartBadge, &FluentBadge::setCount);
}
```

### Message Status Indicators

```cpp
void createMessageItem(const Message& message) {
    auto* messageWidget = new QWidget;
    auto* layout = new QHBoxLayout(messageWidget);
    
    auto* messageLabel = new QLabel(message.text);
    layout->addWidget(messageLabel);
    
    // Status badge
    auto* statusBadge = new FluentBadge();
    statusBadge->setBadgeType(FluentBadgeType::Dot);
    
    if (message.isUnread) {
        statusBadge->setStatus(FluentBadgeStatus::Info);
        statusBadge->setPulsing(true);
    } else if (message.hasError) {
        statusBadge->setStatus(FluentBadgeStatus::Error);
    } else {
        statusBadge->setStatus(FluentBadgeStatus::Success);
    }
    
    statusBadge->attachToWidget(messageWidget);
    statusBadge->setPosition(FluentBadgePosition::TopRight);
}
```

## API Reference

### Enums

```cpp
enum class FluentBadgeType { Count, Text, Icon, Dot, Status };
enum class FluentBadgeSize { XSmall, Small, Medium, Large };
enum class FluentBadgeStyle { Filled, Outline, Ghost };
enum class FluentBadgePosition { TopRight, TopLeft, BottomRight, BottomLeft, Center };
enum class FluentBadgeStatus { None, Success, Warning, Error, Info };
```

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `text` | `QString` | Badge text content |
| `count` | `int` | Numeric count value |
| `maxCount` | `int` | Maximum count before showing "+" |
| `icon` | `QIcon` | Badge icon |
| `badgeType` | `FluentBadgeType` | Type of badge display |
| `badgeSize` | `FluentBadgeSize` | Size of the badge |
| `badgeStyle` | `FluentBadgeStyle` | Visual style |
| `position` | `FluentBadgePosition` | Position relative to attached widget |
| `status` | `FluentBadgeStatus` | Semantic status |
| `animated` | `bool` | Whether to use animations |
| `showZero` | `bool` | Whether to show when count is 0 |
| `pulsing` | `bool` | Whether to pulse continuously |

### Methods

| Method | Description |
|--------|-------------|
| `attachToWidget(QWidget*)` | Attach badge to a widget |
| `detachFromWidget()` | Detach from current widget |
| `show()` | Show badge with animation |
| `hide()` | Hide badge with animation |
| `pulse()` | Trigger pulse animation |
| `setCustomOffset(QPoint)` | Set custom position offset |

### Signals

| Signal | Description |
|--------|-------------|
| `clicked()` | Emitted when badge is clicked |
| `doubleClicked()` | Emitted when badge is double-clicked |
| `countChanged(int)` | Emitted when count changes |
| `textChanged(QString)` | Emitted when text changes |
| `statusChanged(FluentBadgeStatus)` | Emitted when status changes |

## Best Practices

### Performance Guidelines

- Use appropriate badge types for your content
- Avoid excessive pulsing animations
- Cache badge instances for frequently updated content
- Use `setShowZero(false)` to hide empty badges

### Accessibility Guidelines

- Provide meaningful accessible names and descriptions
- Update accessibility info when content changes
- Use semantic status colors consistently
- Ensure sufficient color contrast

### Design Guidelines

- Use consistent badge sizes across your application
- Position badges consistently (typically top-right)
- Use semantic status colors appropriately
- Avoid overwhelming users with too many badges

## Related Components

- **[FluentAvatar](FluentAvatar.md)** - For user status indicators
- **[FluentButton](FluentButton.md)** - For interactive elements with badges
- **[FluentNotification](FluentNotification.md)** - For detailed notifications
- **[FluentToast](FluentToast.md)** - For temporary status messages
