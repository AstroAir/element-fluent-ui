# FluentTimeline Component

The `FluentTimeline` component is a comprehensive timeline widget that implements Microsoft's Fluent Design principles. It provides a flexible way to display chronological information with smooth animations, multiple orientations, and extensive customization options.

## Overview

FluentTimeline offers:
- **Multiple orientations** (Vertical, Horizontal)
- **Flexible alignment** (Left, Right, Center, Alternate)
- **Multiple styles** (Default, Compact, Detailed, Minimal)
- **Interactive timeline items** with state management
- **Smooth animations** for all interactions
- **Scrollable content** with optional scroll areas
- **Theme integration** with automatic color adaptation
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Vertical Timeline

```cpp
#include "FluentQt/Components/FluentTimeline.h"
#include "FluentQt/Components/FluentTimelineItem.h"

// Create a basic vertical timeline
auto* timeline = new FluentTimeline(this);

// Add timeline items
timeline->addTextItem("Project Started", "Initial project setup and planning");
timeline->addTextItem("Development Phase", "Core functionality implementation");
timeline->addTextItem("Testing Phase", "Quality assurance and bug fixes");
timeline->addTextItem("Release", "Product launch and deployment");

// Handle item interactions
connect(timeline, &FluentTimeline::itemClicked, [=](FluentTimelineItem* item, int index) {
    qDebug() << "Clicked item:" << item->title() << "at index:" << index;
});
```

### Horizontal Timeline

```cpp
// Create a horizontal timeline
auto* timeline = FluentTimeline::createHorizontalTimeline(this);
timeline->setAlignment(FluentTimelineAlignment::Center);

// Add items with icons
QIcon startIcon(":/icons/start.png");
QIcon progressIcon(":/icons/progress.png");
QIcon completeIcon(":/icons/complete.png");

timeline->addIconItem(startIcon, "Start", "Project initialization");
timeline->addIconItem(progressIcon, "In Progress", "Development ongoing");
timeline->addIconItem(completeIcon, "Complete", "Project finished");
```

## Timeline Configuration

### Using FluentTimelineConfig

```cpp
FluentTimelineConfig config;
config.orientation = FluentTimelineOrientation::Vertical;
config.alignment = FluentTimelineAlignment::Left;
config.style = FluentTimelineStyle::Detailed;
config.connectorStyle = FluentTimelineConnectorStyle::Dashed;
config.itemSpacing = 20;
config.indicatorSize = 14;
config.animated = true;
config.scrollable = true;
config.showConnectors = true;
config.showIndicators = true;

auto* timeline = new FluentTimeline(config, this);
```

### Timeline Styles

```cpp
// Compact timeline for space-constrained layouts
auto* compactTimeline = FluentTimeline::createCompactTimeline(this);

// Detailed timeline with more spacing and larger indicators
auto* detailedTimeline = FluentTimeline::createDetailedTimeline(this);

// Minimal timeline with reduced visual elements
FluentTimelineConfig minimalConfig;
minimalConfig.style = FluentTimelineStyle::Minimal;
minimalConfig.connectorStyle = FluentTimelineConnectorStyle::Dotted;
auto* minimalTimeline = new FluentTimeline(minimalConfig, this);
```

## Timeline Items

### Creating Different Item Types

```cpp
// Milestone item
auto* milestone = FluentTimelineItem::createMilestone(
    "Version 1.0 Released", 
    QDateTime::currentDateTime()
);
milestone->setItemState(FluentTimelineItemState::Completed);

// Event item
auto* event = FluentTimelineItem::createEvent(
    "Team Meeting",
    "Weekly standup meeting to discuss progress",
    QDateTime::currentDateTime().addDays(1)
);

// Task item
auto* task = FluentTimelineItem::createTask(
    "Implement Feature X",
    FluentTimelineItemState::Current
);

// Note item
auto* note = FluentTimelineItem::createNote(
    "Important Note",
    "Remember to update documentation"
);

timeline->addItem(milestone);
timeline->addItem(event);
timeline->addItem(task);
timeline->addItem(note);
```

### Item States and Visual Indicators

```cpp
auto* item = new FluentTimelineItem("Task Title", this);

// Set different states
item->setItemState(FluentTimelineItemState::Pending);     // Gray indicator
item->setItemState(FluentTimelineItemState::Current);     // Blue indicator
item->setItemState(FluentTimelineItemState::Completed);   // Green indicator
item->setItemState(FluentTimelineItemState::Failed);      // Red indicator
item->setItemState(FluentTimelineItemState::Cancelled);   // Gray indicator
item->setItemState(FluentTimelineItemState::Warning);     // Orange indicator

// Set different indicator styles
item->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Circle);
item->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Square);
item->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Diamond);
item->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Icon);
item->setIndicatorStyle(FluentTimelineItemIndicatorStyle::Avatar);

// Custom colors
item->setCustomColor(QColor("#FF6B6B"));
```

### Expandable Items

```cpp
auto* expandableItem = new FluentTimelineItem("Expandable Item", this);
expandableItem->setExpandable(true);
expandableItem->setDescription("Click to expand for more details");

// Create expanded content widget
auto* expandedWidget = new QWidget();
auto* layout = new QVBoxLayout(expandedWidget);
layout->addWidget(new QLabel("Additional details here"));
layout->addWidget(new QLabel("More information"));

expandableItem->setExpandedWidget(expandedWidget);

// Handle expansion
connect(expandableItem, &FluentTimelineItem::expanded, [=]() {
    qDebug() << "Item expanded";
});

connect(expandableItem, &FluentTimelineItem::collapsed, [=]() {
    qDebug() << "Item collapsed";
});
```

## Advanced Features

### Custom Item Content

```cpp
auto* customItem = new FluentTimelineItem("Custom Content", this);

// Create custom content widget
auto* contentWidget = new QWidget();
auto* contentLayout = new QHBoxLayout(contentWidget);

auto* imageLabel = new QLabel();
imageLabel->setPixmap(QPixmap(":/images/preview.png").scaled(64, 64, Qt::KeepAspectRatio));
contentLayout->addWidget(imageLabel);

auto* textWidget = new QWidget();
auto* textLayout = new QVBoxLayout(textWidget);
textLayout->addWidget(new QLabel("Custom Title"));
textLayout->addWidget(new QLabel("Custom description with rich content"));
contentLayout->addWidget(textWidget);

customItem->setContentWidget(contentWidget);
timeline->addItem(customItem);
```

### Animation Control

```cpp
// Animate to specific item
timeline->animateToItem(targetItem);

// Animate to index
timeline->animateToIndex(2);

// Control animation settings
timeline->setAnimated(true);

// Expand/collapse all items
timeline->expandAll();
timeline->collapseAll();
```

### Navigation and Selection

```cpp
// Set current item
timeline->setCurrentItem(item);
timeline->setCurrentIndex(0);

// Get current selection
FluentTimelineItem* current = timeline->currentItem();
int currentIndex = timeline->currentIndex();

// Handle selection changes
connect(timeline, &FluentTimeline::currentItemChanged, 
        [=](FluentTimelineItem* current, FluentTimelineItem* previous) {
    qDebug() << "Selection changed from" << (previous ? previous->title() : "none")
             << "to" << (current ? current->title() : "none");
});
```

## Keyboard Navigation

The timeline supports comprehensive keyboard navigation:

- **Arrow Keys**: Navigate between items (Up/Down for vertical, Left/Right for horizontal)
- **Home/End**: Jump to first/last item
- **Enter/Space**: Activate current item
- **Tab**: Focus navigation

```cpp
// Enable keyboard navigation
timeline->setFocusPolicy(Qt::StrongFocus);
timeline->setInteractive(true);
```

## Styling and Theming

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* timeline = new FluentTimeline(this);

// Timeline automatically adapts to theme changes
auto& theme = FluentTheme::instance();

// Custom styling (if needed)
connect(&theme, &FluentTheme::themeChanged, [=]() {
    // Update custom styling based on theme
    timeline->refresh();
});
```

### Custom Colors

```cpp
FluentTimelineConfig config;
config.autoCalculateColors = false;
config.connectorColor = QColor("#E0E0E0");
config.indicatorColor = QColor("#2196F3");
config.backgroundColor = QColor("#FAFAFA");

auto* timeline = new FluentTimeline(config, this);
```

## Performance Considerations

### Large Datasets

```cpp
// For large numbers of items, consider using scrollable mode
timeline->setScrollable(true);

// Disable animations for better performance with many items
timeline->setAnimated(false);

// Use compact style for better performance
timeline->setTimelineStyle(FluentTimelineStyle::Compact);
```

### Memory Management

```cpp
// Items are automatically managed by the timeline
// Manual cleanup is only needed if removing items before timeline destruction
timeline->removeItem(item); // Item will be deleted automatically

// Clear all items
timeline->clearItems(); // All items will be deleted
```

## Accessibility

The timeline components include comprehensive accessibility support:

- **Screen Reader Support**: Proper ARIA roles and labels
- **Keyboard Navigation**: Full keyboard accessibility
- **High Contrast**: Automatic adaptation to high contrast themes
- **Focus Management**: Clear focus indicators

```cpp
// Accessibility is enabled by default
// Additional customization:
item->setAccessibleName("Custom accessible name");
item->setAccessibleDescription("Detailed description for screen readers");
```

## Integration Examples

### With Other FluentQt Components

```cpp
// Timeline in a card
auto* card = new FluentCard("Project Timeline", this);
auto* timeline = new FluentTimeline(card);
card->setContentWidget(timeline);

// Timeline in a tab
auto* tabView = new FluentTabView(this);
auto* timelineTab = tabView->addTab("Timeline");
auto* timeline = new FluentTimeline(timelineTab);
```

### Real-world Example: Project Management

```cpp
class ProjectTimelineWidget : public QWidget {
public:
    ProjectTimelineWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupTimeline();
        loadProjectData();
    }

private:
    void setupTimeline() {
        m_timeline = FluentTimeline::createDetailedTimeline(this);
        m_timeline->setAlignment(FluentTimelineAlignment::Left);
        
        auto* layout = new QVBoxLayout(this);
        layout->addWidget(m_timeline);
        
        connect(m_timeline, &FluentTimeline::itemClicked,
                this, &ProjectTimelineWidget::onItemClicked);
    }
    
    void loadProjectData() {
        // Add project milestones
        auto* kickoff = FluentTimelineItem::createMilestone(
            "Project Kickoff", 
            QDateTime::fromString("2024-01-15", "yyyy-MM-dd")
        );
        kickoff->setItemState(FluentTimelineItemState::Completed);
        m_timeline->addItem(kickoff);
        
        // Add development phases
        auto* phase1 = FluentTimelineItem::createTask(
            "Phase 1: Core Development",
            FluentTimelineItemState::Completed
        );
        phase1->setDescription("Implement core functionality and basic UI");
        m_timeline->addItem(phase1);
        
        auto* phase2 = FluentTimelineItem::createTask(
            "Phase 2: Advanced Features",
            FluentTimelineItemState::Current
        );
        phase2->setDescription("Add advanced features and integrations");
        m_timeline->addItem(phase2);
        
        auto* phase3 = FluentTimelineItem::createTask(
            "Phase 3: Testing & Polish",
            FluentTimelineItemState::Pending
        );
        phase3->setDescription("Quality assurance and final polish");
        m_timeline->addItem(phase3);
        
        // Add release milestone
        auto* release = FluentTimelineItem::createMilestone(
            "Release v1.0",
            QDateTime::fromString("2024-06-01", "yyyy-MM-dd")
        );
        release->setItemState(FluentTimelineItemState::Pending);
        m_timeline->addItem(release);
    }
    
    void onItemClicked(FluentTimelineItem* item, int index) {
        // Handle item selection
        showItemDetails(item);
    }
    
    void showItemDetails(FluentTimelineItem* item) {
        // Show detailed information about the selected item
        // This could open a dialog, update a side panel, etc.
    }

private:
    FluentTimeline* m_timeline;
};
```

## Best Practices

1. **Use appropriate timeline styles** for your use case (Compact for space-constrained layouts, Detailed for rich information display)

2. **Provide meaningful item states** to help users understand progress and status

3. **Use consistent indicator styles** throughout your application

4. **Enable animations** for better user experience, but disable for performance-critical scenarios

5. **Provide keyboard navigation** for accessibility

6. **Use expandable items** for detailed information without cluttering the main view

7. **Consider scrollable mode** for timelines with many items

8. **Test with different themes** to ensure proper appearance in light and dark modes

## API Reference

For complete API documentation, see the header files:
- `FluentQt/Components/FluentTimeline.h`
- `FluentQt/Components/FluentTimelineItem.h`

## See Also

- [FluentCard](FluentCard.md) - For containing timeline components
- [FluentNavigationView](FluentNavigationView.md) - For timeline-based navigation
- [FluentProgressBar](FluentProgressBar.md) - For progress indication
- [FluentBreadcrumb](FluentBreadcrumb.md) - For hierarchical navigation
