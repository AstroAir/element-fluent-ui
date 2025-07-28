# FluentCheckBox Component

The `FluentCheckBox` component is an enhanced checkbox that implements Microsoft's Fluent Design principles. It provides smooth animations, tri-state support, and comprehensive accessibility features.

## Overview

FluentCheckBox offers:
- **Standard and tri-state** checkbox functionality
- **Smooth animations** for state transitions
- **Custom styling** with theme integration
- **Accessibility support** with screen reader compatibility
- **Keyboard navigation** support
- **Flexible sizing** options

## Basic Usage

### Simple Checkbox

```cpp
#include "FluentQt/Components/FluentCheckBox.h"

// Create a basic checkbox
auto* checkbox = new FluentCheckBox("Accept terms and conditions");

// Handle state changes
connect(checkbox, &FluentCheckBox::toggled, [](bool checked) {
    qDebug() << "Checkbox is now" << (checked ? "checked" : "unchecked");
});
```

### Checkbox without Text

```cpp
auto* checkbox = new FluentCheckBox();
checkbox->setChecked(true);

// Add to layout with separate label
auto* layout = new QHBoxLayout;
layout->addWidget(checkbox);
layout->addWidget(new QLabel("Custom label text"));
```

## States and Interactions

### Check States

```cpp
auto* checkbox = new FluentCheckBox("Multi-state checkbox");

// Standard checked/unchecked
checkbox->setChecked(true);        // Checked
checkbox->setChecked(false);       // Unchecked

// Using Qt::CheckState for more control
checkbox->setCheckState(Qt::Checked);      // Fully checked
checkbox->setCheckState(Qt::Unchecked);    // Not checked
checkbox->setCheckState(Qt::PartiallyChecked); // Partially checked (tri-state)

// Get current state
bool isChecked = checkbox->isChecked();
Qt::CheckState state = checkbox->checkState();
```

### Tri-State Support

```cpp
auto* parentCheckbox = new FluentCheckBox("Select All");
parentCheckbox->setTristate(true);  // Enable tri-state

auto* child1 = new FluentCheckBox("Option 1");
auto* child2 = new FluentCheckBox("Option 2");
auto* child3 = new FluentCheckBox("Option 3");

// Update parent based on children
auto updateParent = [=]() {
    int checkedCount = 0;
    int totalCount = 3;
    
    if (child1->isChecked()) checkedCount++;
    if (child2->isChecked()) checkedCount++;
    if (child3->isChecked()) checkedCount++;
    
    if (checkedCount == 0) {
        parentCheckbox->setCheckState(Qt::Unchecked);
    } else if (checkedCount == totalCount) {
        parentCheckbox->setCheckState(Qt::Checked);
    } else {
        parentCheckbox->setCheckState(Qt::PartiallyChecked);
    }
};

connect(child1, &FluentCheckBox::toggled, updateParent);
connect(child2, &FluentCheckBox::toggled, updateParent);
connect(child3, &FluentCheckBox::toggled, updateParent);

// Handle parent checkbox clicks
connect(parentCheckbox, &FluentCheckBox::clicked, [=](bool checked) {
    // When parent is clicked, set all children to the same state
    child1->setChecked(checked);
    child2->setChecked(checked);
    child3->setChecked(checked);
});
```

### Disabled State

```cpp
auto* checkbox = new FluentCheckBox("Disabled option");
checkbox->setEnabled(false);
checkbox->setChecked(true);  // Can be checked but not interactive
```

## Animations

FluentCheckBox includes built-in animations that can be customized:

```cpp
auto* checkbox = new FluentCheckBox("Animated checkbox");

// Enable/disable animations
checkbox->setAnimated(true);   // Default: enabled
checkbox->setAnimated(false);  // Disable for performance

// Custom animation on state change
connect(checkbox, &FluentCheckBox::stateChanged, [=](Qt::CheckState state) {
    if (state == Qt::Checked) {
        // Custom success animation
        auto* animator = new FluentAnimator(this);
        animator->pulseEffect(checkbox);
    }
});
```

## Styling and Theming

FluentCheckBox automatically adapts to the current theme:

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* checkbox = new FluentCheckBox("Themed checkbox");

// Checkbox automatically uses theme colors
auto& theme = FluentTheme::instance();

// Custom styling (if needed)
checkbox->setStyleSheet(QString(
    "FluentCheckBox::indicator:checked { "
    "    background-color: %1; "
    "    border-color: %1; "
    "}"
).arg(theme.color("accent").name()));

// Respond to theme changes
connect(&theme, &FluentTheme::themeChanged, [=]() {
    // Update custom styling if needed
});
```

## Accessibility

FluentCheckBox provides comprehensive accessibility support:

```cpp
auto* checkbox = new FluentCheckBox("Save preferences");

// Accessible properties (automatically set)
checkbox->setAccessibleName("Save preferences");
checkbox->setAccessibleDescription("Save your current preferences for future sessions");

// Custom accessibility
checkbox->setAccessibleDescription("Check this box to save your settings");

// Keyboard shortcuts
checkbox->setShortcut(QKeySequence("Ctrl+S"));

// Screen reader support (automatic)
// - Checkbox role is automatically set
// - State changes are announced
// - Keyboard navigation works out of the box
```

## Form Integration

### Settings Form Example

```cpp
auto* settingsPanel = new FluentPanel("Application Settings");
auto* layout = settingsPanel->createVerticalLayout();

// Notification settings
auto* notificationsGroup = new QGroupBox("Notifications");
auto* notifLayout = new QVBoxLayout(notificationsGroup);

auto* emailNotifications = new FluentCheckBox("Email notifications");
auto* pushNotifications = new FluentCheckBox("Push notifications");
auto* soundNotifications = new FluentCheckBox("Sound notifications");

emailNotifications->setChecked(true);  // Default enabled
pushNotifications->setChecked(false);
soundNotifications->setChecked(true);

notifLayout->addWidget(emailNotifications);
notifLayout->addWidget(pushNotifications);
notifLayout->addWidget(soundNotifications);

// Privacy settings
auto* privacyGroup = new QGroupBox("Privacy");
auto* privacyLayout = new QVBoxLayout(privacyGroup);

auto* analytics = new FluentCheckBox("Share usage analytics");
auto* crashReports = new FluentCheckBox("Send crash reports");

analytics->setChecked(false);  // Default disabled for privacy
crashReports->setChecked(true);

privacyLayout->addWidget(analytics);
privacyLayout->addWidget(crashReports);

layout->addWidget(notificationsGroup);
layout->addWidget(privacyGroup);

// Save settings when changed
auto saveSettings = [=]() {
    QSettings settings;
    settings.setValue("notifications/email", emailNotifications->isChecked());
    settings.setValue("notifications/push", pushNotifications->isChecked());
    settings.setValue("notifications/sound", soundNotifications->isChecked());
    settings.setValue("privacy/analytics", analytics->isChecked());
    settings.setValue("privacy/crashReports", crashReports->isChecked());
};

connect(emailNotifications, &FluentCheckBox::toggled, saveSettings);
connect(pushNotifications, &FluentCheckBox::toggled, saveSettings);
connect(soundNotifications, &FluentCheckBox::toggled, saveSettings);
connect(analytics, &FluentCheckBox::toggled, saveSettings);
connect(crashReports, &FluentCheckBox::toggled, saveSettings);
```

### Form Validation with Checkboxes

```cpp
auto* registrationForm = new FluentPanel("User Registration");
auto* layout = registrationForm->createVerticalLayout();

// Form fields
auto* nameInput = new FluentTextInput();
nameInput->setLabelText("Full Name");
nameInput->setValidationType(FluentTextInputValidation::Required);

auto* emailInput = new FluentTextInput();
emailInput->setLabelText("Email Address");
emailInput->setInputType(FluentTextInputType::Email);
emailInput->setValidationType(FluentTextInputValidation::Email);

// Required checkboxes
auto* termsCheckbox = new FluentCheckBox("I agree to the Terms of Service");
auto* privacyCheckbox = new FluentCheckBox("I agree to the Privacy Policy");

// Optional checkbox
auto* newsletterCheckbox = new FluentCheckBox("Subscribe to newsletter");
newsletterCheckbox->setChecked(true);  // Default opt-in

// Submit button
auto* submitButton = FluentButton::createPrimaryButton("Create Account");
submitButton->setEnabled(false);  // Initially disabled

layout->addWidget(nameInput);
layout->addWidget(emailInput);
layout->addWidget(termsCheckbox);
layout->addWidget(privacyCheckbox);
layout->addWidget(newsletterCheckbox);
layout->addWidget(submitButton);

// Form validation
auto validateForm = [=]() {
    bool isValid = nameInput->isValid() && 
                   emailInput->isValid() && 
                   termsCheckbox->isChecked() && 
                   privacyCheckbox->isChecked();
    
    submitButton->setEnabled(isValid);
    
    // Visual feedback for required checkboxes
    if (!termsCheckbox->isChecked()) {
        termsCheckbox->setStyleSheet("color: red;");
    } else {
        termsCheckbox->setStyleSheet("");
    }
    
    if (!privacyCheckbox->isChecked()) {
        privacyCheckbox->setStyleSheet("color: red;");
    } else {
        privacyCheckbox->setStyleSheet("");
    }
};

connect(nameInput, &FluentTextInput::validationStateChanged, validateForm);
connect(emailInput, &FluentTextInput::validationStateChanged, validateForm);
connect(termsCheckbox, &FluentCheckBox::toggled, validateForm);
connect(privacyCheckbox, &FluentCheckBox::toggled, validateForm);

// Initial validation
validateForm();
```

## Advanced Usage

### Checkbox Groups

```cpp
class FluentCheckBoxGroup : public QWidget
{
    Q_OBJECT

public:
    FluentCheckBoxGroup(const QString& title, QWidget* parent = nullptr)
        : QWidget(parent)
    {
        auto* layout = new QVBoxLayout(this);
        
        m_titleLabel = new QLabel(title);
        m_titleLabel->setFont(QFont("Segoe UI", 12, QFont::Bold));
        layout->addWidget(m_titleLabel);
        
        m_checkboxLayout = new QVBoxLayout;
        layout->addLayout(m_checkboxLayout);
    }
    
    void addCheckBox(const QString& text, bool checked = false) {
        auto* checkbox = new FluentCheckBox(text);
        checkbox->setChecked(checked);
        
        connect(checkbox, &FluentCheckBox::toggled, this, &FluentCheckBoxGroup::selectionChanged);
        
        m_checkboxes.append(checkbox);
        m_checkboxLayout->addWidget(checkbox);
    }
    
    QStringList selectedItems() const {
        QStringList selected;
        for (auto* checkbox : m_checkboxes) {
            if (checkbox->isChecked()) {
                selected.append(checkbox->text());
            }
        }
        return selected;
    }
    
    void setSelectedItems(const QStringList& items) {
        for (auto* checkbox : m_checkboxes) {
            checkbox->setChecked(items.contains(checkbox->text()));
        }
    }
    
    void selectAll() {
        for (auto* checkbox : m_checkboxes) {
            checkbox->setChecked(true);
        }
    }
    
    void selectNone() {
        for (auto* checkbox : m_checkboxes) {
            checkbox->setChecked(false);
        }
    }

signals:
    void selectionChanged();

private:
    QLabel* m_titleLabel;
    QVBoxLayout* m_checkboxLayout;
    QList<FluentCheckBox*> m_checkboxes;
};

// Usage
auto* skillsGroup = new FluentCheckBoxGroup("Programming Skills");
skillsGroup->addCheckBox("C++", true);
skillsGroup->addCheckBox("Python", true);
skillsGroup->addCheckBox("JavaScript", false);
skillsGroup->addCheckBox("Java", false);
skillsGroup->addCheckBox("C#", true);

connect(skillsGroup, &FluentCheckBoxGroup::selectionChanged, [=]() {
    QStringList selected = skillsGroup->selectedItems();
    qDebug() << "Selected skills:" << selected;
});
```

### Data Binding

```cpp
class UserPreferences : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool emailNotifications READ emailNotifications WRITE setEmailNotifications NOTIFY emailNotificationsChanged)
    Q_PROPERTY(bool darkMode READ darkMode WRITE setDarkMode NOTIFY darkModeChanged)

public:
    bool emailNotifications() const { return m_emailNotifications; }
    void setEmailNotifications(bool enabled) {
        if (m_emailNotifications != enabled) {
            m_emailNotifications = enabled;
            emit emailNotificationsChanged(enabled);
        }
    }
    
    bool darkMode() const { return m_darkMode; }
    void setDarkMode(bool enabled) {
        if (m_darkMode != enabled) {
            m_darkMode = enabled;
            emit darkModeChanged(enabled);
        }
    }

signals:
    void emailNotificationsChanged(bool enabled);
    void darkModeChanged(bool enabled);

private:
    bool m_emailNotifications{true};
    bool m_darkMode{false};
};

// Bind checkboxes to data model
auto* preferences = new UserPreferences(this);

auto* emailCheckbox = new FluentCheckBox("Email notifications");
auto* darkModeCheckbox = new FluentCheckBox("Dark mode");

// Bind checkbox to model
emailCheckbox->setChecked(preferences->emailNotifications());
connect(emailCheckbox, &FluentCheckBox::toggled, preferences, &UserPreferences::setEmailNotifications);
connect(preferences, &UserPreferences::emailNotificationsChanged, emailCheckbox, &FluentCheckBox::setChecked);

darkModeCheckbox->setChecked(preferences->darkMode());
connect(darkModeCheckbox, &FluentCheckBox::toggled, preferences, &UserPreferences::setDarkMode);
connect(preferences, &UserPreferences::darkModeChanged, darkModeCheckbox, &FluentCheckBox::setChecked);
```

## Best Practices

### Checkbox Guidelines
1. **Use clear, concise labels** that describe what will happen when checked
2. **Group related checkboxes** using group boxes or panels
3. **Use tri-state checkboxes** for hierarchical selections
4. **Provide immediate feedback** for important state changes
5. **Consider default states** carefully (opt-in vs opt-out)

### Accessibility Best Practices
- Always provide meaningful labels
- Use accessible descriptions for complex checkboxes
- Ensure sufficient color contrast
- Test with keyboard navigation
- Test with screen readers

### Performance Considerations
- Disable animations for large lists of checkboxes
- Use data binding for complex state management
- Batch updates when changing multiple checkboxes

## Common Patterns

### Filter Interface

```cpp
auto* filterPanel = new FluentPanel("Filter Options");
auto* layout = filterPanel->createVerticalLayout();

// Category filters
auto* categories = new FluentCheckBoxGroup("Categories");
categories->addCheckBox("Electronics", true);
categories->addCheckBox("Books", true);
categories->addCheckBox("Clothing", false);
categories->addCheckBox("Home & Garden", false);

// Price range filters
auto* priceGroup = new FluentCheckBoxGroup("Price Range");
priceGroup->addCheckBox("Under $25", false);
priceGroup->addCheckBox("$25 - $50", true);
priceGroup->addCheckBox("$50 - $100", true);
priceGroup->addCheckBox("Over $100", false);

layout->addWidget(categories);
layout->addWidget(priceGroup);

// Apply filters when selection changes
auto applyFilters = [=]() {
    QStringList selectedCategories = categories->selectedItems();
    QStringList selectedPriceRanges = priceGroup->selectedItems();
    
    // Apply filters to data model
    filterData(selectedCategories, selectedPriceRanges);
};

connect(categories, &FluentCheckBoxGroup::selectionChanged, applyFilters);
connect(priceGroup, &FluentCheckBoxGroup::selectionChanged, applyFilters);
```

## Signals Reference

```cpp
// State change signals
void stateChanged(Qt::CheckState state);
void toggled(bool checked);
void clicked(bool checked);

// Text change signal
void textChanged(const QString& text);
```

## See Also

- [FluentRadioButton](FluentRadioButton.md) - For exclusive selection options
- [FluentButton](FluentButton.md) - For action buttons
- [FluentTextInput](FluentTextInput.md) - For text input components
- [FluentPanel](FluentPanel.md) - For grouping checkboxes
- [Form Validation Guide](../guides/form-validation.md) - Best practices for form validation
