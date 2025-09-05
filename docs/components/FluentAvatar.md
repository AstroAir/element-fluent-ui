# FluentAvatar Component

The `FluentAvatar` component is a versatile user avatar display widget that implements Microsoft's Fluent Design principles. It supports multiple avatar types including images, initials, icons, and placeholders with comprehensive customization options.

## Overview

FluentAvatar offers:
- **Multiple avatar types** (Image, Initials, Icon, Placeholder)
- **Flexible sizing** with predefined size options
- **Shape variations** (Circle, Square, Rounded Square)
- **Status indicators** with customizable positions
- **Smooth animations** for state transitions
- **Theme integration** with automatic color generation
- **Accessibility support** with proper ARIA attributes
- **Click interactions** with customizable behavior

## Basic Usage

### Image Avatar

```cpp
#include "FluentQt/Components/FluentAvatar.h"

// Create avatar with image
auto* avatar = new FluentAvatar();
avatar->setImage(QPixmap(":/images/user-photo.jpg"));
avatar->setAvatarSize(FluentAvatarSize::Large);

// Or create directly with image
auto* imageAvatar = FluentAvatar::createImageAvatar(
    QPixmap(":/images/profile.jpg"), 
    FluentAvatarSize::Medium
);
```

### Initials Avatar

```cpp
// Create avatar with user name (auto-generates initials)
auto* avatar = new FluentAvatar("John Doe");

// Or set initials manually
auto* initialsAvatar = new FluentAvatar();
initialsAvatar->setInitials("JD");
initialsAvatar->setName("John Doe");  // For accessibility

// Create using convenience method
auto* namedAvatar = FluentAvatar::createInitialsAvatar("Jane Smith");
```

### Icon Avatar

```cpp
// Create avatar with icon
auto* iconAvatar = new FluentAvatar();
iconAvatar->setIcon(QIcon(":/icons/user.svg"));
iconAvatar->setAvatarType(FluentAvatarType::Icon);

// Or use convenience method
auto* userIcon = FluentAvatar::createIconAvatar(
    QIcon(":/icons/profile.svg"),
    FluentAvatarSize::Small
);
```

## Configuration Examples

### Size and Shape Options

```cpp
auto* avatar = new FluentAvatar("Alice Johnson");

// Configure size (XSmall, Small, Medium, Large, XLarge, XXLarge)
avatar->setAvatarSize(FluentAvatarSize::Large);

// Configure shape (Circle, Square, RoundedSquare)
avatar->setShape(FluentAvatarShape::Circle);

// Get size information
int diameter = avatar->avatarDiameter();
QSize size = avatar->sizeHint();
```

### Status Indicators

```cpp
auto* avatar = new FluentAvatar("Bob Wilson");

// Enable and configure status indicator
avatar->setShowStatus(true);
avatar->setStatus(FluentAvatarStatus::Online);

// Status options: None, Online, Offline, Away, Busy, DoNotDisturb
avatar->setStatus(FluentAvatarStatus::Away);

// Get status indicator size and position
QSize statusSize = avatar->statusSize();
QRect statusRect = avatar->statusRect();
```

### Appearance Customization

```cpp
auto* avatar = new FluentAvatar();

// Set custom background color (for initials/icon avatars)
avatar->setCustomBackgroundColor(QColor("#FF6B6B"));

// Configure animations
avatar->setAnimated(true);

// Set tooltip
avatar->setTooltip("User Profile: John Doe");

// Make clickable
avatar->setClickable(true);
```

## State Examples

### Avatar Types

```cpp
auto* avatar = new FluentAvatar();

// Switch between different avatar types
avatar->setAvatarType(FluentAvatarType::Image);
avatar->setImage(QPixmap(":/images/photo.jpg"));

avatar->setAvatarType(FluentAvatarType::Initials);
avatar->setName("Sarah Connor");

avatar->setAvatarType(FluentAvatarType::Icon);
avatar->setIcon(QIcon(":/icons/user.svg"));

avatar->setAvatarType(FluentAvatarType::Placeholder);
// Shows default placeholder
```

### Dynamic Content Updates

```cpp
auto* avatar = new FluentAvatar();

// Update content dynamically
avatar->setName("New User Name");  // Auto-updates initials
avatar->setImage(":/images/new-photo.jpg");  // Load from path

// Refresh avatar display
avatar->refresh();

// Check content validity
if (avatar->hasValidImage()) {
    qDebug() << "Avatar has valid image";
}
if (avatar->hasValidIcon()) {
    qDebug() << "Avatar has valid icon";
}
```

## Event Handling Examples

### Click Interactions

```cpp
auto* avatar = new FluentAvatar("Interactive User");
avatar->setClickable(true);

// Handle click events
connect(avatar, &FluentAvatar::clicked, [=]() {
    qDebug() << "Avatar clicked for:" << avatar->name();
    showUserProfile(avatar->name());
});

connect(avatar, &FluentAvatar::doubleClicked, [=]() {
    qDebug() << "Avatar double-clicked";
    editUserProfile();
});
```

### Status Changes

```cpp
connect(avatar, &FluentAvatar::statusChanged, [=](FluentAvatarStatus status) {
    QString statusText;
    switch (status) {
        case FluentAvatarStatus::Online: statusText = "Online"; break;
        case FluentAvatarStatus::Away: statusText = "Away"; break;
        case FluentAvatarStatus::Busy: statusText = "Busy"; break;
        default: statusText = "Unknown"; break;
    }
    qDebug() << "Status changed to:" << statusText;
});
```

## Advanced Usage

### Dynamic Image Loading

```cpp
auto* avatar = new FluentAvatar("Remote User");

// Load image from URL (if network support is available)
avatar->loadImageFromUrl("https://example.com/avatar.jpg");

// Handle loading states
connect(avatar, &FluentAvatar::imageChanged, [=]() {
    if (avatar->hasValidImage()) {
        qDebug() << "Image loaded successfully";
    } else {
        qDebug() << "Failed to load image, showing initials";
    }
});
```

### Custom Initials Generation

```cpp
auto* avatar = new FluentAvatar();

// Custom initials logic
QString customInitials = avatar->generateInitials("Dr. Jane Smith-Wilson");
avatar->setInitials(customInitials);  // Would be "DJ" or "JS"

// Custom background color generation
QColor bgColor = avatar->generateBackgroundColor("John Doe");
avatar->setCustomBackgroundColor(bgColor);
```

### Animation Effects

```cpp
auto* avatar = new FluentAvatar("Animated User");
avatar->setAnimated(true);

// Trigger animations
avatar->animateIn();    // Fade in animation
avatar->animateOut();   // Fade out animation
avatar->pulse();        // Pulse effect
avatar->bounce();       // Bounce effect

// Use in loading states
avatar->pulse();  // Indicate loading
// ... load content ...
avatar->refresh();  // Update display
```

## Accessibility Examples

### Screen Reader Support

```cpp
auto* avatar = new FluentAvatar("Accessible User");

// Set accessible properties
avatar->setAccessibleName("User avatar for John Doe");
avatar->setAccessibleDescription("Profile picture showing user status as online");

// Update accessibility when status changes
connect(avatar, &FluentAvatar::statusChanged, [=](FluentAvatarStatus status) {
    QString description = QString("Profile picture, status: %1")
                         .arg(getStatusText(status));
    avatar->setAccessibleDescription(description);
});
```

### Keyboard Navigation

```cpp
auto* avatar = new FluentAvatar("Keyboard Accessible");
avatar->setClickable(true);
avatar->setFocusPolicy(Qt::TabFocus);

// Handle keyboard activation
avatar->installEventFilter(this);

bool eventFilter(QObject* obj, QEvent* event) override {
    if (obj == avatar && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Space) {
            emit avatar->clicked();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
```

## Theming Examples

### Automatic Color Generation

```cpp
auto* avatar = new FluentAvatar("Theme User");

// Colors are automatically generated based on name/initials
// Uses consistent color mapping for same names
QColor autoColor = avatar->generateBackgroundColor("John Doe");

// Theme integration
#include "FluentQt/Styling/FluentTheme.h"
auto& theme = FluentTheme::instance();

// Avatar automatically adapts to theme changes
connect(&theme, &FluentTheme::themeChanged, avatar, &FluentAvatar::refresh);
```

### Custom Styling

```cpp
auto* avatar = new FluentAvatar();

// Apply custom colors
avatar->setCustomBackgroundColor(QColor("#4A90E2"));

// Custom stylesheet for additional styling
avatar->setStyleSheet(R"(
    FluentAvatar {
        border: 2px solid #FFFFFF;
    }
    FluentAvatar:hover {
        border-color: #4A90E2;
    }
)");
```

## Real-World Scenarios

### User List with Avatars

```cpp
void createUserList(const QList<User>& users) {
    auto* layout = new QVBoxLayout;
    
    for (const auto& user : users) {
        auto* userWidget = new QWidget;
        auto* userLayout = new QHBoxLayout(userWidget);
        
        // Create avatar
        auto* avatar = new FluentAvatar(user.name);
        if (!user.photoPath.isEmpty()) {
            avatar->setImage(user.photoPath);
        }
        avatar->setStatus(user.status);
        avatar->setClickable(true);
        
        // Connect to user profile
        connect(avatar, &FluentAvatar::clicked, [=]() {
            showUserProfile(user.id);
        });
        
        userLayout->addWidget(avatar);
        userLayout->addWidget(new QLabel(user.name));
        layout->addWidget(userWidget);
    }
}
```

### Chat Interface

```cpp
void addChatMessage(const QString& sender, const QString& message) {
    auto* messageWidget = new QWidget;
    auto* layout = new QHBoxLayout(messageWidget);
    
    // Sender avatar
    auto* avatar = FluentAvatar::createInitialsAvatar(sender, FluentAvatarSize::Small);
    avatar->setTooltip(sender);
    
    // Message content
    auto* messageLabel = new QLabel(message);
    messageLabel->setWordWrap(true);
    
    layout->addWidget(avatar);
    layout->addWidget(messageLabel);
    layout->addStretch();
    
    chatLayout->addWidget(messageWidget);
}
```

## API Reference

### Enums

```cpp
enum class FluentAvatarType { Image, Initials, Icon, Placeholder };
enum class FluentAvatarSize { XSmall, Small, Medium, Large, XLarge, XXLarge };
enum class FluentAvatarShape { Circle, Square, RoundedSquare };
enum class FluentAvatarStatus { None, Online, Offline, Away, Busy, DoNotDisturb };
```

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `name` | `QString` | User name (used for initials generation) |
| `initials` | `QString` | Custom initials text |
| `image` | `QPixmap` | Avatar image |
| `icon` | `QIcon` | Avatar icon |
| `avatarType` | `FluentAvatarType` | Type of avatar display |
| `avatarSize` | `FluentAvatarSize` | Size of the avatar |
| `shape` | `FluentAvatarShape` | Shape of the avatar |
| `status` | `FluentAvatarStatus` | Status indicator |
| `showStatus` | `bool` | Whether to show status indicator |
| `clickable` | `bool` | Whether avatar responds to clicks |
| `animated` | `bool` | Whether to use animations |

### Static Methods

| Method | Description |
|--------|-------------|
| `createImageAvatar()` | Create avatar with image |
| `createInitialsAvatar()` | Create avatar with name/initials |
| `createIconAvatar()` | Create avatar with icon |
| `createPlaceholderAvatar()` | Create placeholder avatar |

### Signals

| Signal | Description |
|--------|-------------|
| `clicked()` | Emitted when avatar is clicked |
| `doubleClicked()` | Emitted when avatar is double-clicked |
| `imageChanged()` | Emitted when image is updated |
| `statusChanged(FluentAvatarStatus)` | Emitted when status changes |

## Best Practices

### Performance Guidelines

- Use appropriate sizes for your use case
- Cache generated initials and colors
- Use placeholder avatars for unknown users
- Implement lazy loading for image avatars

### Accessibility Guidelines

- Always provide meaningful names and descriptions
- Use status indicators consistently
- Ensure sufficient color contrast
- Support keyboard navigation for interactive avatars

### Design Guidelines

- Use consistent sizing across your application
- Implement status indicators meaningfully
- Provide fallbacks (initials) for missing images
- Use appropriate shapes for your design system

## Related Components

- **[FluentBadge](FluentBadge.md)** - For status indicators and notifications
- **[FluentButton](FluentButton.md)** - For interactive avatar buttons
- **[FluentCard](FluentCard.md)** - For user profile cards
- **[FluentTooltip](FluentTooltip.md)** - For avatar hover information
