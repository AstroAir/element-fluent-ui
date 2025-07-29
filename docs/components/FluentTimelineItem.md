# FluentTimelineItem Component

The `FluentTimelineItem` component represents individual items within a FluentTimeline. It provides rich content display, state management, and interactive features following Microsoft's Fluent Design principles.

## Overview

FluentTimelineItem offers:
- **Rich content display** (title, description, subtitle, date/time)
- **Multiple item states** (Pending, Current, Completed, Failed, Cancelled, Warning)
- **Various item types** (Default, Milestone, Event, Task, Note, Custom)
- **Flexible indicator styles** (Circle, Square, Diamond, Icon, Avatar)
- **Expandable content** with smooth animations
- **Custom content widgets** for complex layouts
- **Theme integration** with automatic styling
- **Accessibility support** with proper ARIA attributes

## Basic Usage

### Simple Timeline Item

```cpp
#include "FluentQt/Components/FluentTimelineItem.h"

// Create a basic timeline item
auto* item = new FluentTimelineItem("Task Title", this);
item->setDescription("Detailed description of the task");
item->setDateTime(QDateTime::currentDateTime());

// Set item state
item->setItemState(FluentTimelineItemState::Current);

// Handle interactions
connect(item, &FluentTimelineItem::clicked, [=]() {
    qDebug() << "Item clicked:" << item->title();
});
```

### Item with Icon

```cpp
// Create item with icon
QIcon taskIcon(":/icons/task.png");
auto* iconItem = FluentTimelineItem::createIconItem(
    taskIcon, 
    "Important Task", 
    "This task requires immediate attention"
);
iconItem->setItemState(FluentTimelineItemState::Warning);
```

## Item Types and Factory Methods

### Milestone Items

```cpp
// Create milestone item
auto* milestone = FluentTimelineItem::createMilestone(
    "Project Milestone",
    QDateTime::fromString("2024-03-15", "yyyy-MM-dd")
);
milestone->setItemState(FluentTimelineItemState::Completed);
milestone->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Diamond);
```

### Event Items

```cpp
// Create event item
auto* event = FluentTimelineItem::createEvent(
    "Team Meeting",
    "Weekly standup meeting to discuss project progress",
    QDateTime::currentDateTime().addDays(1)
);
event->setItemType(FluentTimelineItemType::Event);
```

### Task Items

```cpp
// Create task item
auto* task = FluentTimelineItem::createTask(
    "Implement Feature X",
    FluentTimelineItemState::Current
);
task->setDescription("Add new functionality to the application");
task->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Square);
```

### Note Items

```cpp
// Create note item
auto* note = FluentTimelineItem::createNote(
    "Important Note",
    "Remember to update the documentation after implementation"
);
note->setItemState(FluentTimelineItemState::Current);
```

### Avatar Items

```cpp
// Create item with avatar
QPixmap userAvatar(":/images/user_avatar.png");
auto* avatarItem = FluentTimelineItem::createAvatarItem(
    userAvatar,
    "John Doe commented",
    "Great work on the latest update!"
);
avatarItem->setDateTime(QDateTime::currentDateTime());
```

## Item States

### State Management

```cpp
auto* item = new FluentTimelineItem("Task", this);

// Set different states with visual feedback
item->setItemState(FluentTimelineItemState::Pending);     // Gray, waiting
item->setItemState(FluentTimelineItemState::Current);     // Blue, active
item->setItemState(FluentTimelineItemState::Completed);   // Green, done
item->setItemState(FluentTimelineItemState::Failed);      // Red, error
item->setItemState(FluentTimelineItemState::Cancelled);   // Gray, cancelled
item->setItemState(FluentTimelineItemState::Warning);     // Orange, attention needed

// Convenience methods
item->markCompleted();
item->markFailed();
item->markCancelled();
item->markPending();

// Handle state changes
connect(item, &FluentTimelineItem::stateChanged, 
        [=](FluentTimelineItemState state) {
    qDebug() << "Item state changed to:" << static_cast<int>(state);
});
```

## Visual Customization

### Indicator Styles

```cpp
auto* item = new FluentTimelineItem("Custom Item", this);

// Different indicator shapes
item->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Circle);   // Default
item->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Square);   // For tasks
item->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Diamond);  // For milestones
item->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Icon);     // Uses item icon
item->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Avatar);   // Uses item avatar
item->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Custom);   // Custom rendering
```

### Item Sizes

```cpp
// Different sizes for different importance levels
item->setItemSize(FluentTimelineItemSize::Small);    // Compact display
item->setItemSize(FluentTimelineItemSize::Medium);   // Default size
item->setItemSize(FluentTimelineItemSize::Large);    // Prominent display
```

### Custom Colors

```cpp
// Override default state colors
item->setCustomColor(QColor("#FF6B6B"));  // Custom red
item->setCustomColor(QColor("#4ECDC4"));  // Custom teal
item->setCustomColor(QColor("#45B7D1"));  // Custom blue

// Custom text and icons
item->setCustomText("URGENT");
item->setCustomIcon(QIcon(":/icons/priority.png"));
```

## Expandable Content

### Basic Expansion

```cpp
auto* expandableItem = new FluentTimelineItem("Expandable Item", this);
expandableItem->setExpandable(true);
expandableItem->setDescription("Click to see more details");

// Toggle expansion
expandableItem->toggle();
expandableItem->expand();
expandableItem->collapse();

// Check expansion state
bool isExpanded = expandableItem->isExpanded();

// Handle expansion events
connect(expandableItem, &FluentTimelineItem::expanded, [=]() {
    qDebug() << "Item expanded";
});

connect(expandableItem, &FluentTimelineItem::collapsed, [=]() {
    qDebug() << "Item collapsed";
});
```

### Custom Expanded Content

```cpp
auto* item = new FluentTimelineItem("Project Details", this);
item->setExpandable(true);

// Create expanded content widget
auto* expandedWidget = new QWidget();
auto* layout = new QVBoxLayout(expandedWidget);

// Add detailed information
auto* detailsLabel = new QLabel("Detailed project information:");
detailsLabel->setFont(QFont("Arial", 10, QFont::Bold));
layout->addWidget(detailsLabel);

auto* progressBar = new QProgressBar();
progressBar->setValue(75);
layout->addWidget(progressBar);

auto* statusLabel = new QLabel("Status: 75% complete");
layout->addWidget(statusLabel);

// Add action buttons
auto* buttonLayout = new QHBoxLayout();
auto* editButton = new QPushButton("Edit");
auto* deleteButton = new QPushButton("Delete");
buttonLayout->addWidget(editButton);
buttonLayout->addWidget(deleteButton);
layout->addLayout(buttonLayout);

item->setExpandedWidget(expandedWidget);
```

## Custom Content Widgets

### Rich Content Display

```cpp
auto* richItem = new FluentTimelineItem("Rich Content Item", this);

// Create custom content widget
auto* contentWidget = new QWidget();
auto* contentLayout = new QHBoxLayout(contentWidget);

// Add image
auto* imageLabel = new QLabel();
QPixmap image(":/images/content_preview.png");
imageLabel->setPixmap(image.scaled(80, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
contentLayout->addWidget(imageLabel);

// Add text content
auto* textWidget = new QWidget();
auto* textLayout = new QVBoxLayout(textWidget);

auto* titleLabel = new QLabel("Custom Content Title");
titleLabel->setFont(QFont("Arial", 12, QFont::Bold));
textLayout->addWidget(titleLabel);

auto* descLabel = new QLabel("This is a custom content widget with rich formatting and multiple elements.");
descLabel->setWordWrap(true);
textLayout->addWidget(descLabel);

auto* metaLabel = new QLabel("Created: March 15, 2024 | Author: John Doe");
metaLabel->setStyleSheet("color: #666; font-size: 10px;");
textLayout->addWidget(metaLabel);

contentLayout->addWidget(textWidget);
richItem->setContentWidget(contentWidget);
```

## Data Management

### Using FluentTimelineItemData

```cpp
// Create item data structure
FluentTimelineItemData data;
data.title = "Complex Item";
data.description = "This item has comprehensive data";
data.subtitle = "Subtitle information";
data.dateTime = QDateTime::currentDateTime();
data.icon = QIcon(":/icons/item.png");
data.state = FluentTimelineItemState::Current;
data.type = FluentTimelineItemType::Task;
data.size = FluentTimelineItemSize::Large;
data.indicatorStyle = FluentTimelineItemIndicatorStyle::Icon;
data.expandable = true;
data.expanded = false;
data.showDateTime = true;
data.showIndicator = true;
data.customColor = QColor("#2196F3");
data.userData = QVariant::fromValue(customDataObject);

// Create item from data
auto* item = new FluentTimelineItem(data, this);

// Update data
data.state = FluentTimelineItemState::Completed;
item->setData(data);

// Get current data
FluentTimelineItemData currentData = item->data();
```

### User Data Storage

```cpp
// Store custom data with items
struct ProjectInfo {
    QString projectId;
    int priority;
    QStringList tags;
};

ProjectInfo info{"PROJ-123", 5, {"urgent", "frontend"}};
item->setUserData(QVariant::fromValue(info));

// Retrieve custom data
QVariant userData = item->userData();
if (userData.canConvert<ProjectInfo>()) {
    ProjectInfo retrievedInfo = userData.value<ProjectInfo>();
    qDebug() << "Project ID:" << retrievedInfo.projectId;
}
```

## Animations

### State Transitions

```cpp
auto* item = new FluentTimelineItem("Animated Item", this);

// State changes are automatically animated
item->setItemState(FluentTimelineItemState::Pending);
QTimer::singleShot(2000, [=]() {
    item->setItemState(FluentTimelineItemState::Current);
});
QTimer::singleShot(4000, [=]() {
    item->setItemState(FluentTimelineItemState::Completed);
});
```

### Expansion Animations

```cpp
auto* item = new FluentTimelineItem("Smooth Expansion", this);
item->setExpandable(true);

// Expansion is automatically animated
item->animateExpansion(true);   // Expand with animation
item->animateExpansion(false);  // Collapse with animation
```

## Event Handling

### Interaction Events

```cpp
auto* item = new FluentTimelineItem("Interactive Item", this);

// Click events
connect(item, &FluentTimelineItem::clicked, [=]() {
    qDebug() << "Item clicked";
});

connect(item, &FluentTimelineItem::doubleClicked, [=]() {
    qDebug() << "Item double-clicked";
    item->toggle(); // Toggle expansion on double-click
});

// State change events
connect(item, &FluentTimelineItem::stateChanged, 
        [=](FluentTimelineItemState state) {
    updateItemAppearance(state);
});

// Expansion events
connect(item, &FluentTimelineItem::expanded, [=]() {
    logItemExpansion(item->title());
});

connect(item, &FluentTimelineItem::collapsed, [=]() {
    logItemCollapse(item->title());
});
```

### Property Change Events

```cpp
// Monitor property changes
connect(item, &FluentTimelineItem::titleChanged, 
        [=](const QString& title) {
    updateItemTitle(title);
});

connect(item, &FluentTimelineItem::descriptionChanged,
        [=](const QString& description) {
    updateItemDescription(description);
});

connect(item, &FluentTimelineItem::dateTimeChanged,
        [=](const QDateTime& dateTime) {
    updateItemTimestamp(dateTime);
});
```

## Accessibility

### Screen Reader Support

```cpp
auto* item = new FluentTimelineItem("Accessible Item", this);

// Accessibility is automatically configured, but can be customized
item->setAccessibleName("Project milestone: Version 1.0 release");
item->setAccessibleDescription("Completed milestone representing the first major release");

// State information is automatically included in accessibility data
item->setItemState(FluentTimelineItemState::Completed);
// Screen reader will announce: "Completed: Project milestone: Version 1.0 release"
```

### Keyboard Navigation

```cpp
// Items automatically support keyboard navigation when in a timeline
// Focus management is handled by the parent timeline component

// Custom keyboard handling (if needed)
item->setFocusPolicy(Qt::StrongFocus);
```

## Styling and Theming

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* item = new FluentTimelineItem("Themed Item", this);

// Items automatically adapt to theme changes
auto& theme = FluentTheme::instance();

// Custom theme integration (if needed)
connect(&theme, &FluentTheme::themeChanged, [=]() {
    // Update custom styling based on theme
    updateItemStyling(item);
});
```

### Custom Styling

```cpp
// Items can be styled using Qt stylesheets
item->setStyleSheet(R"(
    FluentTimelineItem {
        background-color: #f0f0f0;
        border: 1px solid #ddd;
        border-radius: 4px;
        padding: 8px;
    }
    
    FluentTimelineItem:hover {
        background-color: #e8e8e8;
    }
    
    FluentTimelineItem:focus {
        border-color: #0078d4;
    }
)");
```

## Performance Considerations

### Efficient Updates

```cpp
// Batch property updates for better performance
item->setUpdatesEnabled(false);
item->setTitle("New Title");
item->setDescription("New Description");
item->setItemState(FluentTimelineItemState::Completed);
item->setUpdatesEnabled(true);
```

### Memory Management

```cpp
// Items are automatically managed by their parent timeline
// Manual cleanup is only needed for standalone items
delete item; // Only if not added to a timeline

// For items in a timeline, removal is handled automatically
timeline->removeItem(item); // Item will be deleted
```

## Best Practices

1. **Use appropriate item types** for different kinds of content (Milestone for important events, Task for actionable items, Note for information)

2. **Provide meaningful state information** to help users understand progress and status

3. **Use consistent indicator styles** throughout your application

4. **Include timestamps** for chronological context

5. **Make items expandable** when they contain detailed information

6. **Use custom content widgets** for rich, interactive content

7. **Provide accessible names and descriptions** for screen reader users

8. **Handle state changes** to update related UI elements

9. **Use animations judiciously** - they enhance UX but can impact performance with many items

10. **Test with different themes** to ensure proper appearance

## Integration Examples

### With Data Models

```cpp
class TimelineItemModel : public QAbstractListModel {
    // Model implementation for timeline items
    // Allows data binding and automatic updates
};

// Bind model to timeline items
connect(model, &TimelineItemModel::dataChanged, [=](const QModelIndex& index) {
    auto* item = timeline->item(index.row());
    updateItemFromModel(item, model, index);
});
```

### With Databases

```cpp
class DatabaseTimelineItem : public FluentTimelineItem {
public:
    DatabaseTimelineItem(int recordId, QWidget* parent = nullptr) 
        : FluentTimelineItem(parent), m_recordId(recordId) {
        loadFromDatabase();
    }
    
    void saveToDatabase() {
        // Save item state to database
        DatabaseManager::updateRecord(m_recordId, data());
    }
    
private:
    void loadFromDatabase() {
        // Load item data from database
        auto data = DatabaseManager::getRecord(m_recordId);
        setData(data);
    }
    
    int m_recordId;
};
```

## API Reference

For complete API documentation, see:
- `FluentQt/Components/FluentTimelineItem.h`

## See Also

- [FluentTimeline](FluentTimeline.md) - The container component for timeline items
- [FluentCard](FluentCard.md) - For containing individual timeline items
- [FluentButton](FluentButton.md) - For interactive elements within items
- [FluentProgressBar](FluentProgressBar.md) - For progress indication within items
