# FluentPanel Component

The `FluentPanel` component is a versatile container that implements Microsoft's Fluent Design principles. It provides multiple visual styles, elevation effects, and layout management capabilities for organizing content in modern applications.

## Overview

FluentPanel offers:
- **8 panel types** (Card, Surface, Acrylic, Mica, Sidebar, Content, Dialog, Flyout)
- **4 elevation levels** (None, Low, Medium, High)
- **Flexible corner radius** options
- **Collapsible functionality** with smooth animations
- **Built-in layout helpers** for common patterns
- **Theme integration** with automatic color adaptation
- **Accessibility support** for screen readers

## Basic Usage

### Simple Panel

```cpp
#include "FluentQt/Components/FluentPanel.h"

// Create a basic card panel
auto* panel = new FluentPanel();
panel->setPanelType(FluentPanelType::Card);
panel->setElevation(FluentPanelElevation::Medium);

// Add content using layout helpers
auto* layout = panel->createVerticalLayout();
layout->addWidget(new QLabel("Panel content"));
layout->addWidget(new QPushButton("Action"));
```

### Panel with Title

```cpp
auto* panel = new FluentPanel("Settings");
panel->setPanelType(FluentPanelType::Card);
panel->setCornerRadius(FluentPanelCornerRadius::Medium);

auto* layout = panel->createVerticalLayout();
layout->addWidget(new QLabel("Configure your preferences"));
```

## Panel Types

### Card Panel
Elevated container with shadow, perfect for content cards.

```cpp
auto* card = new FluentPanel("User Profile");
card->setPanelType(FluentPanelType::Card);
card->setElevation(FluentPanelElevation::Medium);
card->setCornerRadius(FluentPanelCornerRadius::Medium);

auto* layout = card->createVerticalLayout();
layout->addWidget(new QLabel("John Doe"));
layout->addWidget(new QLabel("Software Developer"));
```

### Surface Panel
Flat panel with subtle border, ideal for form sections.

```cpp
auto* surface = new FluentPanel("Contact Information");
surface->setPanelType(FluentPanelType::Surface);
surface->setBorderWidth(1);

auto* layout = surface->createVerticalLayout();
layout->addWidget(emailInput);
layout->addWidget(phoneInput);
```

### Acrylic Panel
Translucent panel with blur effect (where supported).

```cpp
auto* acrylic = new FluentPanel();
acrylic->setPanelType(FluentPanelType::Acrylic);
acrylic->setElevation(FluentPanelElevation::High);

// Acrylic panels work best over complex backgrounds
auto* layout = acrylic->createVerticalLayout();
layout->addWidget(new QLabel("Overlay Content"));
```

### Mica Panel
Modern material effect panel.

```cpp
auto* mica = new FluentPanel("Navigation");
mica->setPanelType(FluentPanelType::Mica);
mica->setCornerRadius(FluentPanelCornerRadius::Large);
```

### Sidebar Panel
Optimized for navigation sidebars.

```cpp
auto* sidebar = new FluentPanel();
sidebar->setPanelType(FluentPanelType::Sidebar);
sidebar->setElevation(FluentPanelElevation::Low);

auto* layout = sidebar->createVerticalLayout();
layout->addWidget(new QLabel("Navigation"));
// Add navigation items...
```

### Content Panel
Main content area panel.

```cpp
auto* content = new FluentPanel();
content->setPanelType(FluentPanelType::Content);
content->setPadding(QMargins(24, 24, 24, 24));

auto* layout = content->createVerticalLayout();
// Add main content...
```

### Dialog Panel
Modal dialog container.

```cpp
auto* dialog = new FluentPanel("Confirm Action");
dialog->setPanelType(FluentPanelType::Dialog);
dialog->setElevation(FluentPanelElevation::High);
dialog->setCornerRadius(FluentPanelCornerRadius::Medium);

auto* layout = dialog->createVerticalLayout();
layout->addWidget(new QLabel("Are you sure you want to delete this item?"));

auto* buttonLayout = dialog->createHorizontalLayout();
buttonLayout->addWidget(cancelButton);
buttonLayout->addWidget(deleteButton);
layout->addLayout(buttonLayout);
```

### Flyout Panel
Popup/dropdown container.

```cpp
auto* flyout = new FluentPanel();
flyout->setPanelType(FluentPanelType::Flyout);
flyout->setElevation(FluentPanelElevation::High);
flyout->setCornerRadius(FluentPanelCornerRadius::Small);

auto* layout = flyout->createVerticalLayout();
layout->addWidget(new QLabel("Quick Actions"));
// Add flyout content...
```

## Elevation and Shadows

### Elevation Levels

```cpp
auto* panel = new FluentPanel("Elevated Panel");

// No elevation (flat)
panel->setElevation(FluentPanelElevation::None);

// Low elevation (subtle shadow)
panel->setElevation(FluentPanelElevation::Low);

// Medium elevation (standard shadow)
panel->setElevation(FluentPanelElevation::Medium);

// High elevation (prominent shadow)
panel->setElevation(FluentPanelElevation::High);
```

### Dynamic Elevation

```cpp
auto* interactivePanel = new FluentPanel("Interactive Card");
interactivePanel->setElevation(FluentPanelElevation::Low);

// Increase elevation on hover
connect(interactivePanel, &FluentPanel::hovered, [=]() {
    interactivePanel->setElevation(FluentPanelElevation::Medium);
});

connect(interactivePanel, &FluentPanel::left, [=]() {
    interactivePanel->setElevation(FluentPanelElevation::Low);
});
```

## Corner Radius and Styling

### Corner Radius Options

```cpp
auto* panel = new FluentPanel("Styled Panel");

// No rounded corners
panel->setCornerRadius(FluentPanelCornerRadius::None);

// Small radius (4px)
panel->setCornerRadius(FluentPanelCornerRadius::Small);

// Medium radius (8px) - default
panel->setCornerRadius(FluentPanelCornerRadius::Medium);

// Large radius (16px)
panel->setCornerRadius(FluentPanelCornerRadius::Large);
```

### Custom Colors

```cpp
auto* panel = new FluentPanel("Custom Panel");

// Custom background color
panel->setBackgroundColor(QColor(240, 248, 255));  // Light blue

// Custom border
panel->setBorderColor(QColor(70, 130, 180));       // Steel blue
panel->setBorderWidth(2);

// Custom padding
panel->setPadding(QMargins(20, 16, 20, 16));
```

## Collapsible Panels

### Basic Collapsible Panel

```cpp
auto* collapsiblePanel = new FluentPanel("Advanced Settings");
collapsiblePanel->setCollapsible(true);
collapsiblePanel->setCollapsed(true);  // Start collapsed

auto* layout = collapsiblePanel->createVerticalLayout();
layout->addWidget(new QLabel("These are advanced options"));
layout->addWidget(advancedSettingsWidget);

// Handle collapse state changes
connect(collapsiblePanel, &FluentPanel::collapsedChanged, [](bool collapsed) {
    qDebug() << "Panel is now" << (collapsed ? "collapsed" : "expanded");
});
```

### Programmatic Control

```cpp
auto* panel = new FluentPanel("Expandable Content");
panel->setCollapsible(true);

// Control collapse state
auto* toggleButton = new FluentButton("Toggle Panel");
connect(toggleButton, &FluentButton::clicked, [=]() {
    panel->setCollapsed(!panel->isCollapsed());
});

// Animate expansion/collapse
panel->setAnimated(true);
```

## Layout Helpers

### Vertical Layout

```cpp
auto* panel = new FluentPanel("Vertical Layout");
auto* layout = panel->createVerticalLayout();

layout->addWidget(new QLabel("Item 1"));
layout->addWidget(new QLabel("Item 2"));
layout->addWidget(new QLabel("Item 3"));
layout->addStretch();  // Push content to top
```

### Horizontal Layout

```cpp
auto* panel = new FluentPanel("Horizontal Layout");
auto* layout = panel->createHorizontalLayout();

layout->addWidget(new QLabel("Left"));
layout->addStretch();
layout->addWidget(new QLabel("Right"));
```

### Grid Layout

```cpp
auto* panel = new FluentPanel("Grid Layout");
auto* layout = panel->createGridLayout();

layout->addWidget(new QLabel("Name:"), 0, 0);
layout->addWidget(nameInput, 0, 1);
layout->addWidget(new QLabel("Email:"), 1, 0);
layout->addWidget(emailInput, 1, 1);
```

### Stacked Layout

```cpp
auto* panel = new FluentPanel("Stacked Content");
auto* layout = panel->createStackedLayout();

layout->addWidget(page1Widget);
layout->addWidget(page2Widget);
layout->addWidget(page3Widget);

// Switch between pages
layout->setCurrentIndex(0);
```

## Advanced Usage

### Nested Panels

```cpp
auto* outerPanel = new FluentPanel("Main Container");
outerPanel->setPanelType(FluentPanelType::Content);

auto* outerLayout = outerPanel->createVerticalLayout();

// Nested card panels
auto* card1 = new FluentPanel("Section 1");
card1->setPanelType(FluentPanelType::Card);
card1->setElevation(FluentPanelElevation::Low);

auto* card2 = new FluentPanel("Section 2");
card2->setPanelType(FluentPanelType::Card);
card2->setElevation(FluentPanelElevation::Low);

outerLayout->addWidget(card1);
outerLayout->addWidget(card2);
```

### Responsive Panels

```cpp
auto* responsivePanel = new FluentPanel("Responsive Content");

// Adjust panel based on size
auto updatePanelStyle = [=]() {
    QSize size = responsivePanel->size();
    
    if (size.width() < 400) {
        // Mobile-like layout
        responsivePanel->setPadding(QMargins(12, 12, 12, 12));
        responsivePanel->setCornerRadius(FluentPanelCornerRadius::Small);
    } else {
        // Desktop layout
        responsivePanel->setPadding(QMargins(24, 24, 24, 24));
        responsivePanel->setCornerRadius(FluentPanelCornerRadius::Medium);
    }
};

connect(responsivePanel, &FluentPanel::resized, updatePanelStyle);
```

### Panel with Actions

```cpp
auto* actionPanel = new FluentPanel("Document");
actionPanel->setPanelType(FluentPanelType::Card);

auto* layout = actionPanel->createVerticalLayout();

// Content
layout->addWidget(new QLabel("Document Title"));
layout->addWidget(new QLabel("Document description..."));

// Action buttons
auto* actionLayout = actionPanel->createHorizontalLayout();
actionLayout->addStretch();
actionLayout->addWidget(new FluentButton("Edit"));
actionLayout->addWidget(new FluentButton("Delete"));

layout->addLayout(actionLayout);
```

## Theming and Styling

### Automatic Theme Adaptation

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* panel = new FluentPanel("Themed Panel");

// Panel automatically adapts to theme changes
auto& theme = FluentTheme::instance();

// Manual theme integration (if needed)
connect(&theme, &FluentTheme::themeChanged, [=]() {
    // Update custom styling if needed
    panel->setBackgroundColor(theme.color("surface"));
    panel->setBorderColor(theme.color("border"));
});
```

### Custom Styling

```cpp
auto* styledPanel = new FluentPanel("Custom Styled");

// Apply custom stylesheet
styledPanel->setStyleSheet(R"(
    FluentPanel {
        background-color: qlineargradient(
            x1: 0, y1: 0, x2: 0, y2: 1,
            stop: 0 #f0f8ff,
            stop: 1 #e6f3ff
        );
        border: 2px solid #4682b4;
        border-radius: 12px;
    }
)");
```

## Accessibility

FluentPanel provides accessibility support:

```cpp
auto* panel = new FluentPanel("Accessible Panel");

// Accessible properties
panel->setAccessibleName("User Settings Panel");
panel->setAccessibleDescription("Contains user preference controls");

// Collapsible panel accessibility
if (panel->isCollapsible()) {
    panel->setAccessibleDescription(
        panel->isCollapsed() ? "Collapsed settings panel" : "Expanded settings panel"
    );
}

// Keyboard navigation (automatic)
// - Tab navigation works within panel content
// - Collapsible panels respond to Enter/Space
```

## Best Practices

### Panel Hierarchy
1. **Content Panel** - Main application content area
2. **Card Panels** - Individual content sections
3. **Surface Panels** - Form sections and grouped controls
4. **Sidebar Panels** - Navigation and secondary content

### Elevation Guidelines
- **None** - Flat surfaces, backgrounds
- **Low** - Subtle separation, hover states
- **Medium** - Standard cards, important content
- **High** - Modals, flyouts, critical content

### Layout Recommendations
- Use appropriate padding for content density
- Maintain consistent spacing between panels
- Consider responsive behavior for different screen sizes
- Group related content logically

### Performance Tips
- Avoid excessive nesting of panels
- Use appropriate panel types for content
- Consider lazy loading for collapsible content
- Optimize animations for smooth performance

## Common Patterns

### Settings Panel

```cpp
auto* settingsPanel = new FluentPanel("Application Settings");
settingsPanel->setPanelType(FluentPanelType::Card);
settingsPanel->setCollapsible(true);

auto* layout = settingsPanel->createVerticalLayout();

// Settings sections
auto* generalSection = new FluentPanel("General");
generalSection->setPanelType(FluentPanelType::Surface);
// Add general settings...

auto* advancedSection = new FluentPanel("Advanced");
advancedSection->setPanelType(FluentPanelType::Surface);
advancedSection->setCollapsible(true);
advancedSection->setCollapsed(true);
// Add advanced settings...

layout->addWidget(generalSection);
layout->addWidget(advancedSection);
```

### Dashboard Layout

```cpp
auto* dashboard = new FluentPanel();
dashboard->setPanelType(FluentPanelType::Content);

auto* layout = dashboard->createGridLayout();

// Dashboard cards
auto* statsCard = new FluentPanel("Statistics");
statsCard->setPanelType(FluentPanelType::Card);
statsCard->setElevation(FluentPanelElevation::Low);

auto* chartCard = new FluentPanel("Performance");
chartCard->setPanelType(FluentPanelType::Card);
chartCard->setElevation(FluentPanelElevation::Low);

auto* activityCard = new FluentPanel("Recent Activity");
activityCard->setPanelType(FluentPanelType::Card);
activityCard->setElevation(FluentPanelElevation::Low);

layout->addWidget(statsCard, 0, 0);
layout->addWidget(chartCard, 0, 1);
layout->addWidget(activityCard, 1, 0, 1, 2);  // Span 2 columns
```

## Signals Reference

```cpp
// Title and state changes
void titleChanged(const QString& title);
void collapsedChanged(bool collapsed);

// Interaction signals
void clicked();
void hovered();
void left();
void resized();
```

## See Also

- [FluentCard](FluentCard.md) - For specialized card containers
- [FluentNavigationView](FluentNavigationView.md) - For navigation panels
- [FluentTheme](../styling/FluentTheme.md) - For theming and styling
- [Layout Guide](../guides/layout-patterns.md) - Best practices for layouts
