# FluentButton Component

The `FluentButton` component is a versatile, animated button that implements Microsoft's Fluent Design principles. It supports multiple styles, sizes, states, and includes built-in animations and accessibility features.

## Overview

FluentButton provides a modern button experience with:
- **Multiple visual styles** (Default, Primary, Accent, Subtle, Outline, Text, Icon, Toggle, Split)
- **Three sizes** (Small, Medium, Large) 
- **Icon support** with flexible positioning
- **Loading states** with built-in indicators
- **Toggle functionality** for checkable buttons
- **Smooth animations** for all interactions
- **Full accessibility** support

## Basic Usage

### Simple Button

```cpp
#include "FluentQt/Components/FluentButton.h"

// Create a basic button
auto* button = new FluentButton("Click Me");

// Or use convenience methods
auto* primaryButton = FluentButton::createPrimaryButton("Primary Action");
auto* accentButton = FluentButton::createAccentButton("Accent Action");
auto* iconButton = FluentButton::createIconButton(QIcon(":/icons/settings"));
```

### Button with Icon

```cpp
auto* button = new FluentButton(QIcon(":/icons/save"), "Save Document");

// Customize icon position
button->setIconPosition(FluentIconPosition::Left);   // Icon on left (default)
button->setIconPosition(FluentIconPosition::Right);  // Icon on right
button->setIconPosition(FluentIconPosition::Top);    // Icon above text
button->setIconPosition(FluentIconPosition::Bottom); // Icon below text
```

## Styles

### Primary Button
Used for the main action in a UI. Typically one per screen.

```cpp
auto* button = new FluentButton("Get Started");
button->setButtonStyle(FluentButtonStyle::Primary);
// or
auto* button = FluentButton::createPrimaryButton("Get Started");
```

### Accent Button
Uses the system accent color. For important secondary actions.

```cpp
auto* button = new FluentButton("Learn More");
button->setButtonStyle(FluentButtonStyle::Accent);
// or
auto* button = FluentButton::createAccentButton("Learn More");
```

### Subtle Button
Low-emphasis button for secondary actions.

```cpp
auto* button = new FluentButton("Cancel");
button->setButtonStyle(FluentButtonStyle::Subtle);
```

### Outline Button
Button with border but transparent background.

```cpp
auto* button = new FluentButton("More Options");
button->setButtonStyle(FluentButtonStyle::Outline);
```

### Text Button
Minimal button with just text, no background.

```cpp
auto* button = new FluentButton("Skip");
button->setButtonStyle(FluentButtonStyle::Text);
```

### Icon Button
Button with only an icon, no text.

```cpp
auto* button = new FluentButton();
button->setIcon(QIcon(":/icons/settings"));
button->setButtonStyle(FluentButtonStyle::Icon);
// or
auto* button = FluentButton::createIconButton(QIcon(":/icons/settings"));
```

### Toggle Button
Button that can be toggled on/off.

```cpp
auto* button = new FluentButton("Toggle Feature");
button->setButtonStyle(FluentButtonStyle::Toggle);
button->setCheckable(true);

connect(button, &FluentButton::toggled, [](bool checked) {
    qDebug() << "Toggle state:" << checked;
});
```

## Sizes

```cpp
auto* smallButton = new FluentButton("Small");
smallButton->setButtonSize(FluentButtonSize::Small);    // 24px height

auto* mediumButton = new FluentButton("Medium");
mediumButton->setButtonSize(FluentButtonSize::Medium);  // 32px height (default)

auto* largeButton = new FluentButton("Large");
largeButton->setButtonSize(FluentButtonSize::Large);    // 40px height
```

## States and Interactions

### Loading State

```cpp
auto* button = new FluentButton("Submit");

// Show loading state
button->setLoading(true);  // Shows spinner, disables interaction

// Handle async operation
connect(button, &FluentButton::clicked, [=]() {
    button->setLoading(true);
    
    // Simulate async operation
    QTimer::singleShot(2000, [=]() {
        button->setLoading(false);
        // Show success feedback
    });
});
```

### Disabled State

```cpp
auto* button = new FluentButton("Submit");
button->setEnabled(false);  // Standard Qt disabled state
```

### Checkable/Toggle State

```cpp
auto* toggleButton = new FluentButton("Dark Mode");
toggleButton->setCheckable(true);
toggleButton->setChecked(false);

connect(toggleButton, &FluentButton::toggled, [](bool checked) {
    if (checked) {
        // Enable dark mode
    } else {
        // Enable light mode
    }
});
```

## Animations

FluentButton includes built-in animations that can be customized:

```cpp
#include "FluentQt/Animation/FluentAnimator.h"

auto* button = new FluentButton("Animated");
auto* animator = new FluentAnimator(this);

// Manual animation triggers
connect(button, &FluentButton::pressed, [=]() {
    animator->pressEffect(button);
});

connect(button, &FluentButton::clicked, [=]() {
    animator->pulseEffect(button);
});

// Disable animations if needed
button->setAnimated(false);
```

## Accessibility

FluentButton provides comprehensive accessibility support:

```cpp
auto* button = new FluentButton("Save Document");

// Accessible description
button->setAccessibleName("Save Document");
button->setAccessibleDescription("Saves the current document to disk");

// Keyboard shortcuts
button->setShortcut(QKeySequence::Save);  // Ctrl+S

// Screen reader support (automatic)
// - Button role is automatically set
// - State changes are announced
// - Keyboard navigation works out of the box
```

## Styling and Theming

FluentButton automatically adapts to the current theme:

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto& theme = FluentTheme::instance();

// Buttons automatically use theme colors
auto* button = new FluentButton("Themed Button");

// Custom styling (if needed)
button->setStyleSheet(QString(
    "FluentButton { "
    "    background-color: %1; "
    "    color: %2; "
    "}"
).arg(theme.color("accent").name(), theme.color("textOnAccent").name()));

// Respond to theme changes
connect(&theme, &FluentTheme::themeChanged, [=]() {
    // Update custom styling if needed
});
```

## Advanced Usage

### Split Button

```cpp
auto* splitButton = new FluentButton("Save");
splitButton->setButtonStyle(FluentButtonStyle::Split);

// Add dropdown menu
auto* menu = new QMenu(splitButton);
menu->addAction("Save");
menu->addAction("Save As...");
menu->addAction("Save All");

splitButton->setMenu(menu);
```

### Button Groups

```cpp
// Create a group of related buttons
auto* layout = new QHBoxLayout;

auto* button1 = new FluentButton("Option 1");
auto* button2 = new FluentButton("Option 2");
auto* button3 = new FluentButton("Option 3");

// Make them toggleable and exclusive
button1->setCheckable(true);
button2->setCheckable(true);
button3->setCheckable(true);

auto* buttonGroup = new QButtonGroup(this);
buttonGroup->addButton(button1, 0);
buttonGroup->addButton(button2, 1);
buttonGroup->addButton(button3, 2);
buttonGroup->setExclusive(true);

layout->addWidget(button1);
layout->addWidget(button2);
layout->addWidget(button3);
```

### Custom Validation

```cpp
auto* submitButton = new FluentButton("Submit Form");

// Enable/disable based on form validation
auto validateForm = [=]() {
    bool isValid = /* check form validity */;
    submitButton->setEnabled(isValid);
    
    if (isValid) {
        submitButton->setButtonStyle(FluentButtonStyle::Primary);
    } else {
        submitButton->setButtonStyle(FluentButtonStyle::Subtle);
    }
};

// Connect to form field changes
connect(emailInput, &FluentTextInput::textChanged, validateForm);
connect(passwordInput, &FluentTextInput::textChanged, validateForm);
```

## Best Practices

### Button Hierarchy
1. **Primary Button** - One per screen for main action
2. **Accent Button** - Important secondary actions
3. **Default Button** - Standard actions
4. **Subtle Button** - Less important actions
5. **Text Button** - Minimal actions like "Cancel" or "Skip"

### Sizing Guidelines
- **Small** - Compact UIs, toolbars, dense layouts
- **Medium** - Standard UI elements (default)
- **Large** - Touch interfaces, prominent actions

### Icon Usage
- Use recognizable, universal icons
- Ensure icons are accessible (provide text alternatives)
- Keep icon and text semantically aligned
- Consider icon-only buttons for space-constrained UIs

### Loading States
- Always provide feedback for async operations
- Use loading state for operations > 500ms
- Disable interaction during loading
- Provide clear success/error feedback

### Accessibility
- Always provide meaningful accessible names
- Use keyboard shortcuts for important actions
- Ensure sufficient color contrast
- Test with screen readers

## Common Patterns

### Form Submission

```cpp
auto* form = new QWidget;
auto* layout = new QVBoxLayout(form);

auto* emailInput = new FluentTextInput("Email");
auto* passwordInput = new FluentTextInput("Password");
passwordInput->setInputType(FluentTextInputType::Password);

auto* submitButton = FluentButton::createPrimaryButton("Sign In");
auto* cancelButton = new FluentButton("Cancel");
cancelButton->setButtonStyle(FluentButtonStyle::Subtle);

layout->addWidget(emailInput);
layout->addWidget(passwordInput);

auto* buttonLayout = new QHBoxLayout;
buttonLayout->addWidget(cancelButton);
buttonLayout->addStretch();
buttonLayout->addWidget(submitButton);
layout->addLayout(buttonLayout);

// Handle submission
connect(submitButton, &FluentButton::clicked, [=]() {
    submitButton->setLoading(true);
    // Perform authentication...
});
```

### Toolbar Actions

```cpp
auto* toolbar = new QToolBar;

auto* newButton = FluentButton::createIconButton(QIcon(":/icons/new"));
newButton->setButtonSize(FluentButtonSize::Small);
newButton->setToolTip("New Document (Ctrl+N)");
newButton->setShortcut(QKeySequence::New);

auto* openButton = FluentButton::createIconButton(QIcon(":/icons/open"));
openButton->setButtonSize(FluentButtonSize::Small);
openButton->setToolTip("Open Document (Ctrl+O)");
openButton->setShortcut(QKeySequence::Open);

toolbar->addWidget(newButton);
toolbar->addWidget(openButton);
```

## Signals Reference

```cpp
// Text and appearance changes
void textChanged(const QString& text);
void iconChanged(const QIcon& icon);
void styleChanged(FluentButtonStyle style);
void sizeChanged(FluentButtonSize size);
void iconPositionChanged(FluentIconPosition position);

// State changes
void loadingChanged(bool loading);
void checkedChanged(bool checked);

// Interaction signals
void clicked(bool checked = false);
void pressed();
void released();
void toggled(bool checked);
```

## See Also

- [FluentTextInput](FluentTextInput.md) - For form input components
- [FluentPanel](FluentPanel.md) - For container components
- [FluentAnimator](../animation/FluentAnimator.md) - For custom animations
- [FluentTheme](../styling/FluentTheme.md) - For theming and styling
