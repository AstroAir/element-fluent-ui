# FluentAccordion Component

The `FluentAccordion` component is a collapsible content container that implements Microsoft's Fluent Design principles. It provides smooth animations, elevation effects, and comprehensive accessibility features for organizing content in expandable sections.

## Overview

FluentAccordion offers:
- **Smooth expand/collapse animations** with Fluent Design motion tokens
- **Elevation and shadow effects** following Fluent Design principles
- **Icon and description support** with flexible content layout
- **Accessibility features** with ARIA attributes and keyboard navigation
- **Theme integration** with automatic color adaptation
- **Reduced motion support** for accessibility preferences
- **Customizable styling** with elevation levels

## Basic Usage

### Simple Accordion

```cpp
#include "FluentQt/Components/FluentAccordion.h"

// Create a basic accordion
auto* accordion = new FluentAccordion("Settings");
accordion->setDescription("Configure application preferences");

// Add content widget
auto* contentWidget = new QWidget;
auto* layout = new QVBoxLayout(contentWidget);
layout->addWidget(new QLabel("Accordion content goes here"));

accordion->setContentWidget(contentWidget);

// Add to parent layout
parentLayout->addWidget(accordion);
```

### Accordion with Icon

```cpp
auto* accordion = new FluentAccordion("User Profile");
accordion->setDescription("Manage your profile settings");
accordion->setIcon(QIcon(":/icons/user.svg"));
accordion->setShowIcon(true);

// Configure appearance
accordion->setElevation(2);
accordion->setShowDescription(true);
```

## Configuration Examples

### Elevation and Styling

```cpp
auto* accordion = new FluentAccordion("Advanced Settings");

// Configure elevation (0-4 levels)
accordion->setElevation(3);

// Control visibility of elements
accordion->setShowDescription(true);
accordion->setShowIcon(true);

// Set custom icon
accordion->setIcon(QIcon(":/icons/settings.svg"));
```

### Animation Control

```cpp
auto* accordion = new FluentAccordion("Animated Section");

// Control animation behavior
accordion->setReducedMotion(false);  // Enable full animations

// Respect system accessibility preferences
if (QAccessible::isActive()) {
    accordion->setReducedMotion(true);
}
```

## State Examples

### Programmatic Control

```cpp
auto* accordion = new FluentAccordion("Collapsible Content");

// Control expansion state
accordion->expand();    // Expand with animation
accordion->collapse();  // Collapse with animation
accordion->toggle();    // Toggle current state

// Check current state
if (accordion->isExpanded()) {
    qDebug() << "Accordion is expanded";
}
```

### Initial State Configuration

```cpp
auto* accordion = new FluentAccordion("Initially Expanded");
accordion->setExpanded(true);  // Start expanded

auto* accordion2 = new FluentAccordion("Initially Collapsed");
accordion2->setExpanded(false);  // Start collapsed (default)
```

## Event Handling Examples

### Expansion Events

```cpp
auto* accordion = new FluentAccordion("Interactive Section");

// Connect to expansion signals
connect(accordion, &FluentAccordion::expanded, [=]() {
    qDebug() << "Accordion expanded";
    // Load content dynamically when expanded
    loadAccordionContent();
});

connect(accordion, &FluentAccordion::collapsed, [=]() {
    qDebug() << "Accordion collapsed";
    // Clean up resources when collapsed
    cleanupAccordionContent();
});

connect(accordion, &FluentAccordion::toggled, [=](bool expanded) {
    qDebug() << "Accordion toggled:" << (expanded ? "expanded" : "collapsed");
});
```

### Click Handling

```cpp
connect(accordion, &FluentAccordion::clicked, [=]() {
    qDebug() << "Accordion header clicked";
    // Custom click handling if needed
});
```

## Advanced Usage

### Dynamic Content Loading

```cpp
auto* accordion = new FluentAccordion("Dynamic Content");

connect(accordion, &FluentAccordion::expanded, [=]() {
    if (!accordion->contentWidget()) {
        // Create content widget on first expansion
        auto* content = createDynamicContent();
        accordion->setContentWidget(content);
    }
});

QWidget* createDynamicContent() {
    auto* widget = new QWidget;
    auto* layout = new QVBoxLayout(widget);
    
    // Add dynamic content
    layout->addWidget(new QLabel("Dynamically loaded content"));
    layout->addWidget(new QPushButton("Action Button"));
    
    return widget;
}
```

### Accordion Groups

```cpp
// Create multiple accordions in a group
QList<FluentAccordion*> accordionGroup;

for (int i = 0; i < 3; ++i) {
    auto* accordion = new FluentAccordion(QString("Section %1").arg(i + 1));
    accordionGroup.append(accordion);
    
    // Implement exclusive expansion (only one open at a time)
    connect(accordion, &FluentAccordion::expanded, [=]() {
        for (auto* other : accordionGroup) {
            if (other != accordion && other->isExpanded()) {
                other->collapse();
            }
        }
    });
    
    parentLayout->addWidget(accordion);
}
```

## Accessibility Examples

### ARIA Attributes

```cpp
auto* accordion = new FluentAccordion("Accessible Section");

// Set accessible labels
accordion->setAriaLabel("Settings accordion");
accordion->setAccessibleName("Application Settings");
accordion->setAccessibleDescription("Expand to view and modify application settings");

// The component automatically handles:
// - role="button" for the header
// - aria-expanded state
// - aria-controls relationship
// - keyboard navigation (Space/Enter to toggle)
```

### Keyboard Navigation

```cpp
// Keyboard navigation is automatically supported:
// - Tab: Focus the accordion header
// - Space/Enter: Toggle expansion
// - Escape: Collapse if expanded

// Custom keyboard handling if needed
accordion->installEventFilter(this);

bool eventFilter(QObject* obj, QEvent* event) override {
    if (obj == accordion && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_F1) {
            showAccordionHelp();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
```

## Theming Examples

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* accordion = new FluentAccordion("Themed Section");

// Theme colors are automatically applied
// Access theme for custom styling if needed
auto& theme = FluentTheme::instance();
QColor headerColor = theme.color("surface.primary");
QColor textColor = theme.color("text.primary");
```

### Custom Styling

```cpp
auto* accordion = new FluentAccordion("Custom Styled");

// Apply custom stylesheet if needed
accordion->setStyleSheet(R"(
    FluentAccordion {
        border: 1px solid #E0E0E0;
        border-radius: 8px;
    }
    FluentAccordion:hover {
        background-color: #F5F5F5;
    }
)");
```

## API Reference

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `title` | `QString` | Accordion header title text |
| `description` | `QString` | Optional description text below title |
| `icon` | `QIcon` | Optional icon displayed in header |
| `expanded` | `bool` | Current expansion state |
| `elevation` | `int` | Shadow elevation level (0-4) |
| `showDescription` | `bool` | Whether to show description text |
| `showIcon` | `bool` | Whether to show icon |
| `ariaLabel` | `QString` | Accessible label for screen readers |
| `reducedMotion` | `bool` | Whether to use reduced motion animations |

### Methods

| Method | Description |
|--------|-------------|
| `setContentWidget(QWidget*)` | Set the content widget displayed when expanded |
| `contentWidget()` | Get the current content widget |
| `toggle()` | Toggle expansion state |
| `expand()` | Expand with animation |
| `collapse()` | Collapse with animation |
| `isExpanded()` | Check if currently expanded |

### Signals

| Signal | Description |
|--------|-------------|
| `expanded()` | Emitted when accordion expands |
| `collapsed()` | Emitted when accordion collapses |
| `toggled(bool)` | Emitted when expansion state changes |
| `clicked()` | Emitted when header is clicked |

## Best Practices

### Performance Optimization

- Use dynamic content loading for heavy content
- Implement lazy loading for accordion groups
- Consider using `setReducedMotion(true)` for better performance on low-end devices

### Accessibility Guidelines

- Always provide meaningful titles and descriptions
- Use `setAriaLabel()` for additional context
- Ensure content is keyboard accessible
- Test with screen readers

### Design Guidelines

- Use consistent elevation levels across your application
- Group related accordions together
- Provide clear visual hierarchy with titles and descriptions
- Consider exclusive expansion for related content sections

## Related Components

- **[FluentCard](FluentCard.md)** - For non-collapsible content containers
- **[FluentPanel](FluentPanel.md)** - For general content organization
- **[FluentTabView](FluentTabView.md)** - For tabbed content organization
- **[FluentSheet](FluentSheet.md)** - For modal content presentation
