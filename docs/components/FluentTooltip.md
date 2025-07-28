# FluentTooltip Component

The `FluentTooltip` component is an enhanced tooltip widget that implements Microsoft's Fluent Design principles. It provides rich content support, smooth animations, and comprehensive customization options for contextual information display.

## Overview

FluentTooltip offers:
- **Rich content support** with text, icons, and custom widgets
- **Smart positioning** with automatic placement adjustment
- **Smooth animations** for show/hide transitions
- **Multiple trigger modes** (hover, click, manual)
- **Custom styling** with theme integration
- **Delay and timing control** for optimal user experience
- **Accessibility support** with screen reader compatibility

## Basic Usage

### Simple Text Tooltip

```cpp
#include "FluentQt/Components/FluentTooltip.h"

// Create a widget with tooltip
auto* button = new FluentButton("Hover me");

// Set simple tooltip
FluentTooltip::setTooltip(button, "This is a helpful tooltip");

// Alternative: create tooltip manually
auto* tooltip = new FluentTooltip("This is a helpful tooltip", button);
tooltip->setTarget(button);
```

### Rich Content Tooltip

```cpp
auto* button = new FluentButton("Rich Tooltip");

// Create tooltip with icon and description
auto* tooltip = new FluentTooltip(button);
tooltip->setTitle("Save Document");
tooltip->setDescription("Save the current document to disk");
tooltip->setIcon(QIcon(":/icons/save"));
tooltip->setShortcut("Ctrl+S");

// Set as tooltip for the button
FluentTooltip::setTooltip(button, tooltip);
```

## Content Types

### Text-Only Tooltips

```cpp
auto* widget = new QWidget;

// Simple text
FluentTooltip::setTooltip(widget, "Simple tooltip text");

// Multi-line text
FluentTooltip::setTooltip(widget, "Line 1\nLine 2\nLine 3");

// Rich text with HTML
FluentTooltip::setTooltip(widget, "<b>Bold text</b><br><i>Italic text</i>");
```

### Tooltips with Icons

```cpp
auto* button = new FluentButton("Delete");

auto* tooltip = new FluentTooltip(button);
tooltip->setText("Delete selected items");
tooltip->setIcon(QIcon(":/icons/delete"));
tooltip->setIconSize(QSize(16, 16));
tooltip->setIconPosition(FluentIconPosition::Left);

FluentTooltip::setTooltip(button, tooltip);
```

### Tooltips with Shortcuts

```cpp
auto* action = new QAction("Copy", this);
action->setShortcut(QKeySequence::Copy);

auto* button = new FluentButton("Copy");

auto* tooltip = new FluentTooltip(button);
tooltip->setText("Copy selected text");
tooltip->setShortcut(action->shortcut());
tooltip->setShowShortcut(true);

FluentTooltip::setTooltip(button, tooltip);
```

### Custom Widget Tooltips

```cpp
auto* button = new FluentButton("Custom Content");

// Create custom tooltip content
auto* customWidget = new QWidget;
auto* layout = new QVBoxLayout(customWidget);

auto* titleLabel = new QLabel("Custom Tooltip");
titleLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));

auto* descLabel = new QLabel("This tooltip contains custom widgets");
descLabel->setWordWrap(true);

auto* progressBar = new QProgressBar;
progressBar->setValue(75);

layout->addWidget(titleLabel);
layout->addWidget(descLabel);
layout->addWidget(progressBar);

auto* tooltip = new FluentTooltip(button);
tooltip->setCustomWidget(customWidget);

FluentTooltip::setTooltip(button, tooltip);
```

## Positioning and Placement

### Automatic Positioning

```cpp
auto* tooltip = new FluentTooltip("Auto-positioned tooltip");

// Enable smart positioning (default)
tooltip->setAutoPosition(true);
tooltip->setPreferredPosition(FluentTooltipPosition::Top);

// Fallback positions if preferred position doesn't fit
tooltip->setFallbackPositions({
    FluentTooltipPosition::Bottom,
    FluentTooltipPosition::Left,
    FluentTooltipPosition::Right
});
```

### Manual Positioning

```cpp
auto* tooltip = new FluentTooltip("Manual position");

// Disable auto-positioning
tooltip->setAutoPosition(false);
tooltip->setPosition(FluentTooltipPosition::BottomRight);

// Custom offset
tooltip->setOffset(QPoint(10, 5));

// Show at specific global position
tooltip->showAt(QPoint(100, 200));
```

### Position Constraints

```cpp
auto* tooltip = new FluentTooltip("Constrained tooltip");

// Keep tooltip within screen bounds
tooltip->setConstrainToScreen(true);

// Keep tooltip within parent widget bounds
tooltip->setConstrainToParent(true);

// Custom constraint rectangle
tooltip->setConstraintRect(QRect(0, 0, 800, 600));
```

## Timing and Triggers

### Show/Hide Delays

```cpp
auto* tooltip = new FluentTooltip("Delayed tooltip");

// Show delay (hover time before showing)
tooltip->setShowDelay(500);  // 500ms

// Hide delay (time before hiding after mouse leaves)
tooltip->setHideDelay(100);  // 100ms

// Auto-hide timeout (hide after this time regardless)
tooltip->setAutoHideTimeout(5000);  // 5 seconds
```

### Trigger Modes

```cpp
auto* tooltip = new FluentTooltip("Trigger modes");

// Hover trigger (default)
tooltip->setTriggerMode(FluentTooltipTrigger::Hover);

// Click trigger
tooltip->setTriggerMode(FluentTooltipTrigger::Click);

// Manual trigger (show/hide programmatically)
tooltip->setTriggerMode(FluentTooltipTrigger::Manual);

// Focus trigger
tooltip->setTriggerMode(FluentTooltipTrigger::Focus);

// Combined triggers
tooltip->setTriggerMode(FluentTooltipTrigger::Hover | FluentTooltipTrigger::Focus);
```

### Manual Control

```cpp
auto* tooltip = new FluentTooltip("Manual control");
tooltip->setTriggerMode(FluentTooltipTrigger::Manual);

// Show/hide manually
tooltip->show();
tooltip->hide();

// Show with animation
tooltip->showAnimated();
tooltip->hideAnimated();

// Toggle visibility
tooltip->toggle();
```

## Styling and Theming

### Appearance Customization

```cpp
auto* tooltip = new FluentTooltip("Styled tooltip");

// Background and border
tooltip->setBackgroundColor(QColor(255, 255, 255));
tooltip->setBorderColor(QColor(200, 200, 200));
tooltip->setBorderWidth(1);
tooltip->setCornerRadius(4);

// Text styling
tooltip->setTextColor(QColor(0, 0, 0));
tooltip->setFont(QFont("Segoe UI", 9));

// Shadow effect
tooltip->setShadowEnabled(true);
tooltip->setShadowColor(QColor(0, 0, 0, 50));
tooltip->setShadowOffset(QPoint(2, 2));
tooltip->setShadowBlurRadius(8);
```

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* tooltip = new FluentTooltip("Themed tooltip");

// Use theme colors
auto& theme = FluentTheme::instance();
tooltip->setBackgroundColor(theme.color("tooltipBackground"));
tooltip->setTextColor(theme.color("tooltipText"));
tooltip->setBorderColor(theme.color("tooltipBorder"));

// Respond to theme changes
connect(&theme, &FluentTheme::themeChanged, [=]() {
    tooltip->setBackgroundColor(theme.color("tooltipBackground"));
    tooltip->setTextColor(theme.color("tooltipText"));
    tooltip->setBorderColor(theme.color("tooltipBorder"));
});
```

## Animations

### Show/Hide Animations

```cpp
auto* tooltip = new FluentTooltip("Animated tooltip");

// Enable animations
tooltip->setAnimated(true);
tooltip->setShowAnimationDuration(200);
tooltip->setHideAnimationDuration(150);

// Animation types
tooltip->setShowAnimation(FluentTooltipAnimation::Fade);
tooltip->setHideAnimation(FluentTooltipAnimation::Fade);

// Alternative animations
tooltip->setShowAnimation(FluentTooltipAnimation::Scale);
tooltip->setShowAnimation(FluentTooltipAnimation::Slide);

// Custom easing
tooltip->setAnimationEasing(FluentEasing::CubicOut);
```

### Custom Animation Effects

```cpp
auto* tooltip = new FluentTooltip("Custom animations");

// Custom show animation
connect(tooltip, &FluentTooltip::aboutToShow, [=]() {
    auto* animator = new FluentAnimator(this);
    animator->fadeIn(tooltip, 200);
    animator->scaleEffect(tooltip, 0.8, 1.0, 200);
});

// Custom hide animation
connect(tooltip, &FluentTooltip::aboutToHide, [=]() {
    auto* animator = new FluentAnimator(this);
    animator->fadeOut(tooltip, 150);
    animator->scaleEffect(tooltip, 1.0, 0.9, 150);
});
```

## Integration Examples

### Toolbar with Tooltips

```cpp
class ToolbarWithTooltips : public QToolBar
{
public:
    ToolbarWithTooltips(QWidget* parent = nullptr) : QToolBar(parent) {
        setupActions();
    }

private:
    void setupActions() {
        // New action
        auto* newAction = addAction(QIcon(":/icons/new"), "New");
        newAction->setShortcut(QKeySequence::New);
        
        auto* newTooltip = new FluentTooltip(this);
        newTooltip->setTitle("New Document");
        newTooltip->setDescription("Create a new document");
        newTooltip->setShortcut(newAction->shortcut());
        newTooltip->setIcon(QIcon(":/icons/new"));
        
        FluentTooltip::setTooltip(widgetForAction(newAction), newTooltip);
        
        // Open action
        auto* openAction = addAction(QIcon(":/icons/open"), "Open");
        openAction->setShortcut(QKeySequence::Open);
        
        auto* openTooltip = new FluentTooltip(this);
        openTooltip->setTitle("Open Document");
        openTooltip->setDescription("Open an existing document from disk");
        openTooltip->setShortcut(openAction->shortcut());
        openTooltip->setIcon(QIcon(":/icons/open"));
        
        FluentTooltip::setTooltip(widgetForAction(openAction), openTooltip);
        
        // Save action
        auto* saveAction = addAction(QIcon(":/icons/save"), "Save");
        saveAction->setShortcut(QKeySequence::Save);
        
        auto* saveTooltip = new FluentTooltip(this);
        saveTooltip->setTitle("Save Document");
        saveTooltip->setDescription("Save the current document");
        saveTooltip->setShortcut(saveAction->shortcut());
        saveTooltip->setIcon(QIcon(":/icons/save"));
        
        FluentTooltip::setTooltip(widgetForAction(saveAction), saveTooltip);
        
        addSeparator();
        
        // Cut action
        auto* cutAction = addAction(QIcon(":/icons/cut"), "Cut");
        cutAction->setShortcut(QKeySequence::Cut);
        
        FluentTooltip::setTooltip(widgetForAction(cutAction), 
            "Cut\nRemove selected content and copy to clipboard\nCtrl+X");
        
        // Copy action
        auto* copyAction = addAction(QIcon(":/icons/copy"), "Copy");
        copyAction->setShortcut(QKeySequence::Copy);
        
        FluentTooltip::setTooltip(widgetForAction(copyAction),
            "Copy\nCopy selected content to clipboard\nCtrl+C");
        
        // Paste action
        auto* pasteAction = addAction(QIcon(":/icons/paste"), "Paste");
        pasteAction->setShortcut(QKeySequence::Paste);
        
        FluentTooltip::setTooltip(widgetForAction(pasteAction),
            "Paste\nInsert clipboard content at cursor\nCtrl+V");
    }
};
```

### Form with Help Tooltips

```cpp
class FormWithTooltips : public QWidget
{
public:
    FormWithTooltips(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupTooltips();
    }

private:
    void setupUI() {
        auto* layout = new QFormLayout(this);
        
        // Name field
        m_nameEdit = new FluentTextInput;
        m_nameEdit->setPlaceholderText("Enter your full name");
        layout->addRow("Name:", m_nameEdit);
        
        // Email field
        m_emailEdit = new FluentTextInput;
        m_emailEdit->setPlaceholderText("Enter your email address");
        layout->addRow("Email:", m_emailEdit);
        
        // Password field
        m_passwordEdit = new FluentTextInput;
        m_passwordEdit->setEchoMode(QLineEdit::Password);
        m_passwordEdit->setPlaceholderText("Enter a secure password");
        layout->addRow("Password:", m_passwordEdit);
        
        // Age field
        m_ageSpinBox = new QSpinBox;
        m_ageSpinBox->setRange(13, 120);
        m_ageSpinBox->setValue(25);
        layout->addRow("Age:", m_ageSpinBox);
        
        // Country field
        m_countryComboBox = new FluentComboBox;
        m_countryComboBox->addItems({"United States", "Canada", "United Kingdom", "Germany", "France"});
        layout->addRow("Country:", m_countryComboBox);
        
        // Newsletter checkbox
        m_newsletterCheckBox = new FluentCheckBox("Subscribe to newsletter");
        layout->addRow("", m_newsletterCheckBox);
        
        // Submit button
        m_submitButton = FluentButton::createPrimaryButton("Submit");
        layout->addRow("", m_submitButton);
    }
    
    void setupTooltips() {
        // Name field tooltip
        auto* nameTooltip = new FluentTooltip(m_nameEdit);
        nameTooltip->setTitle("Full Name");
        nameTooltip->setDescription("Enter your first and last name as they appear on official documents");
        nameTooltip->setIcon(QIcon(":/icons/user"));
        FluentTooltip::setTooltip(m_nameEdit, nameTooltip);
        
        // Email field tooltip
        auto* emailTooltip = new FluentTooltip(m_emailEdit);
        emailTooltip->setTitle("Email Address");
        emailTooltip->setDescription("We'll use this to send you important updates and notifications");
        emailTooltip->setDetailText("We never share your email with third parties");
        emailTooltip->setIcon(QIcon(":/icons/email"));
        FluentTooltip::setTooltip(m_emailEdit, emailTooltip);
        
        // Password field tooltip
        auto* passwordWidget = new QWidget;
        auto* passwordLayout = new QVBoxLayout(passwordWidget);
        
        auto* titleLabel = new QLabel("Password Requirements");
        titleLabel->setFont(QFont("Segoe UI", 9, QFont::Bold));
        
        auto* requirementsList = new QLabel(
            "• At least 8 characters\n"
            "• Include uppercase and lowercase letters\n"
            "• Include at least one number\n"
            "• Include at least one special character"
        );
        requirementsList->setStyleSheet("color: #666;");
        
        passwordLayout->addWidget(titleLabel);
        passwordLayout->addWidget(requirementsList);
        
        auto* passwordTooltip = new FluentTooltip(m_passwordEdit);
        passwordTooltip->setCustomWidget(passwordWidget);
        passwordTooltip->setShowDelay(200);
        FluentTooltip::setTooltip(m_passwordEdit, passwordTooltip);
        
        // Age field tooltip
        FluentTooltip::setTooltip(m_ageSpinBox, 
            "Age\nYou must be at least 13 years old to create an account");
        
        // Country field tooltip
        FluentTooltip::setTooltip(m_countryComboBox,
            "Country\nSelect your country of residence for localized content");
        
        // Newsletter checkbox tooltip
        auto* newsletterTooltip = new FluentTooltip(m_newsletterCheckBox);
        newsletterTooltip->setTitle("Newsletter Subscription");
        newsletterTooltip->setDescription("Receive monthly updates about new features and tips");
        newsletterTooltip->setDetailText("You can unsubscribe at any time");
        FluentTooltip::setTooltip(m_newsletterCheckBox, newsletterTooltip);
        
        // Submit button tooltip
        FluentTooltip::setTooltip(m_submitButton, "Submit Form\nCreate your account with the provided information");
    }

private:
    FluentTextInput* m_nameEdit{nullptr};
    FluentTextInput* m_emailEdit{nullptr};
    FluentTextInput* m_passwordEdit{nullptr};
    QSpinBox* m_ageSpinBox{nullptr};
    FluentComboBox* m_countryComboBox{nullptr};
    FluentCheckBox* m_newsletterCheckBox{nullptr};
    FluentButton* m_submitButton{nullptr};
};
```

### Status Bar with Tooltips

```cpp
class StatusBarWithTooltips : public QStatusBar
{
public:
    StatusBarWithTooltips(QWidget* parent = nullptr) : QStatusBar(parent) {
        setupStatusWidgets();
    }
    
    void updateStatus(const QString& message, int timeout = 0) {
        showMessage(message, timeout);
    }
    
    void setConnectionStatus(bool connected) {
        if (connected) {
            m_connectionLabel->setPixmap(QIcon(":/icons/connected").pixmap(16, 16));
            FluentTooltip::setTooltip(m_connectionLabel, "Connected\nSuccessfully connected to server");
        } else {
            m_connectionLabel->setPixmap(QIcon(":/icons/disconnected").pixmap(16, 16));
            FluentTooltip::setTooltip(m_connectionLabel, "Disconnected\nNo connection to server");
        }
    }
    
    void setProgress(int value, int maximum = 100) {
        m_progressBar->setMaximum(maximum);
        m_progressBar->setValue(value);
        
        QString tooltipText = QString("Progress: %1%\n%2 of %3 items processed")
                             .arg(value * 100 / maximum)
                             .arg(value)
                             .arg(maximum);
        FluentTooltip::setTooltip(m_progressBar, tooltipText);
    }

private:
    void setupStatusWidgets() {
        // Connection status
        m_connectionLabel = new QLabel;
        m_connectionLabel->setPixmap(QIcon(":/icons/disconnected").pixmap(16, 16));
        addPermanentWidget(m_connectionLabel);
        
        // Progress bar
        m_progressBar = new QProgressBar;
        m_progressBar->setMaximumWidth(200);
        m_progressBar->setVisible(false);
        addPermanentWidget(m_progressBar);
        
        // Memory usage
        m_memoryLabel = new QLabel("Memory: 0 MB");
        addPermanentWidget(m_memoryLabel);
        
        // Setup tooltips
        FluentTooltip::setTooltip(m_connectionLabel, "Connection Status\nShows current server connection state");
        FluentTooltip::setTooltip(m_progressBar, "Operation Progress\nShows progress of current operation");
        
        // Memory usage tooltip with live updates
        auto* memoryTooltip = new FluentTooltip(m_memoryLabel);
        memoryTooltip->setTriggerMode(FluentTooltipTrigger::Hover);
        memoryTooltip->setShowDelay(500);
        
        // Update memory tooltip content when shown
        connect(memoryTooltip, &FluentTooltip::aboutToShow, [=]() {
            qint64 memoryUsage = getCurrentMemoryUsage();
            qint64 totalMemory = getTotalSystemMemory();
            
            QString tooltipText = QString(
                "Memory Usage\n"
                "Used: %1 MB\n"
                "Total: %2 MB\n"
                "Usage: %3%"
            ).arg(memoryUsage / 1024 / 1024)
             .arg(totalMemory / 1024 / 1024)
             .arg(memoryUsage * 100 / totalMemory);
            
            memoryTooltip->setText(tooltipText);
        });
        
        FluentTooltip::setTooltip(m_memoryLabel, memoryTooltip);
        
        // Update memory display periodically
        auto* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [=]() {
            qint64 memoryUsage = getCurrentMemoryUsage();
            m_memoryLabel->setText(QString("Memory: %1 MB").arg(memoryUsage / 1024 / 1024));
        });
        timer->start(1000);  // Update every second
    }
    
    qint64 getCurrentMemoryUsage() {
        // Platform-specific memory usage calculation
        return 128 * 1024 * 1024;  // Placeholder: 128 MB
    }
    
    qint64 getTotalSystemMemory() {
        // Platform-specific total memory calculation
        return 8 * 1024 * 1024 * 1024;  // Placeholder: 8 GB
    }

private:
    QLabel* m_connectionLabel{nullptr};
    QProgressBar* m_progressBar{nullptr};
    QLabel* m_memoryLabel{nullptr};
};
```

## Accessibility

FluentTooltip provides comprehensive accessibility support:

```cpp
auto* tooltip = new FluentTooltip("Accessible tooltip");

// Accessible properties (automatically set)
tooltip->setAccessibleName("Tooltip");
tooltip->setAccessibleDescription("Provides additional information about the element");

// Screen reader support
tooltip->setAccessibleRole(QAccessible::ToolTip);

// Keyboard navigation
tooltip->setKeyboardNavigationEnabled(true);

// Screen reader announcements
connect(tooltip, &FluentTooltip::shown, [=]() {
    tooltip->announceToScreenReader(tooltip->text());
});
```

## Best Practices

### Tooltip Design Guidelines
1. **Keep content concise** - Provide helpful but brief information
2. **Use consistent timing** - Maintain predictable show/hide delays
3. **Position intelligently** - Ensure tooltips don't obscure important content
4. **Provide keyboard access** - Support keyboard navigation
5. **Use appropriate triggers** - Choose hover, click, or focus based on context

### Performance Considerations
- Use efficient positioning algorithms
- Cache tooltip content when possible
- Optimize animations for smooth interaction
- Implement proper cleanup for dynamic tooltips

### Accessibility Guidelines
- Support keyboard navigation
- Provide screen reader announcements
- Use sufficient color contrast
- Include meaningful tooltip content
- Test with assistive technologies

## Signals Reference

```cpp
// Visibility signals
void aboutToShow();
void shown();
void aboutToHide();
void hidden();

// Animation signals
void showAnimationStarted();
void showAnimationFinished();
void hideAnimationStarted();
void hideAnimationFinished();

// Interaction signals
void clicked();
void targetChanged(QWidget* target);
```

## See Also

- [FluentButton](FluentButton.md) - For buttons with tooltips
- [FluentTextInput](FluentTextInput.md) - For input fields with help text
- [FluentNotification](FluentNotification.md) - For persistent notifications
- [API Reference - FluentTooltip](../API_REFERENCE.md#fluenttooltip) - Complete API documentation
