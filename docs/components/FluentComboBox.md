# FluentComboBox Component

The `FluentComboBox` component is an enhanced dropdown selection control that implements Microsoft's Fluent Design principles. It provides smooth animations, search functionality, and comprehensive customization options.

## Overview

FluentComboBox offers:
- **Enhanced dropdown** with smooth animations
- **Search and filtering** capabilities
- **Custom item rendering** with icons and rich content
- **Multiple selection modes** (single, multiple)
- **Validation support** with error states
- **Theme integration** with automatic styling
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple ComboBox

```cpp
#include "FluentQt/Components/FluentComboBox.h"

// Create a basic combobox
auto* comboBox = new FluentComboBox;
comboBox->setLabelText("Select Country");

// Add items
comboBox->addItem("United States");
comboBox->addItem("Canada");
comboBox->addItem("United Kingdom");
comboBox->addItem("Germany");
comboBox->addItem("France");

// Handle selection changes
connect(comboBox, &FluentComboBox::currentTextChanged, [=](const QString& text) {
    qDebug() << "Selected:" << text;
});
```

### ComboBox with Icons

```cpp
auto* comboBox = new FluentComboBox;
comboBox->setLabelText("Select Application");

// Add items with icons
comboBox->addItem(QIcon(":/icons/browser"), "Web Browser");
comboBox->addItem(QIcon(":/icons/editor"), "Text Editor");
comboBox->addItem(QIcon(":/icons/terminal"), "Terminal");
comboBox->addItem(QIcon(":/icons/calculator"), "Calculator");

// Set placeholder text
comboBox->setPlaceholderText("Choose an application...");
```

## Advanced Features

### Searchable ComboBox

```cpp
auto* comboBox = new FluentComboBox;
comboBox->setLabelText("Select City");
comboBox->setSearchable(true);  // Enable search functionality
comboBox->setSearchPlaceholder("Type to search cities...");

// Add many items
QStringList cities = {
    "New York", "Los Angeles", "Chicago", "Houston", "Phoenix",
    "Philadelphia", "San Antonio", "San Diego", "Dallas", "San Jose",
    "Austin", "Jacksonville", "Fort Worth", "Columbus", "Charlotte"
};

for (const QString& city : cities) {
    comboBox->addItem(city);
}

// Search is automatically handled - items are filtered as user types
```

### Multiple Selection

```cpp
auto* comboBox = new FluentComboBox;
comboBox->setLabelText("Select Skills");
comboBox->setSelectionMode(FluentComboBoxSelectionMode::Multiple);
comboBox->setPlaceholderText("Select your skills...");

// Add skill options
QStringList skills = {
    "C++", "Python", "JavaScript", "Java", "C#",
    "React", "Vue.js", "Angular", "Node.js", "Django"
};

for (const QString& skill : skills) {
    comboBox->addItem(skill);
}

// Handle multiple selections
connect(comboBox, &FluentComboBox::selectionChanged, [=](const QStringList& selected) {
    qDebug() << "Selected skills:" << selected;
});

// Get selected items
QStringList selectedSkills = comboBox->selectedItems();
```

### Custom Item Data

```cpp
struct UserData {
    QString name;
    QString email;
    QIcon avatar;
    QString role;
};

auto* comboBox = new FluentComboBox;
comboBox->setLabelText("Assign to User");

// Add items with custom data
QList<UserData> users = {
    {"John Doe", "john@example.com", QIcon(":/avatars/john"), "Developer"},
    {"Jane Smith", "jane@example.com", QIcon(":/avatars/jane"), "Designer"},
    {"Bob Johnson", "bob@example.com", QIcon(":/avatars/bob"), "Manager"}
};

for (const UserData& user : users) {
    comboBox->addItem(user.avatar, user.name, QVariant::fromValue(user));
}

// Handle selection with custom data
connect(comboBox, &FluentComboBox::currentDataChanged, [=](const QVariant& data) {
    if (data.canConvert<UserData>()) {
        UserData user = data.value<UserData>();
        qDebug() << "Selected user:" << user.name << "(" << user.role << ")";
    }
});
```

## Validation and States

### Input Validation

```cpp
auto* comboBox = new FluentComboBox;
comboBox->setLabelText("Priority Level");
comboBox->setValidationType(FluentComboBoxValidation::Required);

comboBox->addItem("Low");
comboBox->addItem("Medium");
comboBox->addItem("High");
comboBox->addItem("Critical");

// Validation states
connect(comboBox, &FluentComboBox::validationStateChanged, [=](bool isValid) {
    if (isValid) {
        comboBox->setInputState(FluentComboBoxState::Success);
        comboBox->setHelperText("Priority level selected");
    } else {
        comboBox->setInputState(FluentComboBoxState::Error);
        comboBox->setErrorText("Please select a priority level");
    }
});

// Manual validation
bool isValid = comboBox->isValid();
```

### Visual States

```cpp
auto* comboBox = new FluentComboBox;

// Set different visual states
comboBox->setInputState(FluentComboBoxState::Normal);   // Default state
comboBox->setInputState(FluentComboBoxState::Focused);  // Focused state
comboBox->setInputState(FluentComboBoxState::Error);    // Error state
comboBox->setInputState(FluentComboBoxState::Warning);  // Warning state
comboBox->setInputState(FluentComboBoxState::Success);  // Success state
comboBox->setInputState(FluentComboBoxState::Disabled); // Disabled state

// Helper and error text
comboBox->setHelperText("Choose the most appropriate option");
comboBox->setErrorText("This field is required");
```

## Custom Rendering

### Custom Item Delegate

```cpp
class CustomComboBoxDelegate : public QStyledItemDelegate
{
public:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        
        painter->save();
        
        // Custom painting logic
        QRect rect = opt.rect;
        
        // Draw background
        if (opt.state & QStyle::State_Selected) {
            painter->fillRect(rect, QColor(0, 120, 215, 50));
        } else if (opt.state & QStyle::State_MouseOver) {
            painter->fillRect(rect, QColor(0, 120, 215, 25));
        }
        
        // Draw icon
        QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
        if (!icon.isNull()) {
            QRect iconRect(rect.left() + 8, rect.top() + 4, 24, 24);
            icon.paint(painter, iconRect);
        }
        
        // Draw text
        QString text = index.data(Qt::DisplayRole).toString();
        QRect textRect(rect.left() + 40, rect.top(), rect.width() - 48, rect.height());
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
        
        painter->restore();
    }
    
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        Q_UNUSED(option)
        Q_UNUSED(index)
        return QSize(200, 32);
    }
};

// Apply custom delegate
auto* comboBox = new FluentComboBox;
comboBox->setItemDelegate(new CustomComboBoxDelegate);
```

### Rich Content Items

```cpp
auto* comboBox = new FluentComboBox;
comboBox->setLabelText("Select Template");

// Create custom item widgets
auto createTemplateItem = [](const QString& name, const QString& description, const QIcon& icon) {
    auto* widget = new QWidget;
    auto* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(8, 4, 8, 4);
    
    auto* iconLabel = new QLabel;
    iconLabel->setPixmap(icon.pixmap(32, 32));
    
    auto* textWidget = new QWidget;
    auto* textLayout = new QVBoxLayout(textWidget);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(2);
    
    auto* nameLabel = new QLabel(name);
    nameLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
    
    auto* descLabel = new QLabel(description);
    descLabel->setFont(QFont("Segoe UI", 8));
    descLabel->setStyleSheet("color: #666;");
    
    textLayout->addWidget(nameLabel);
    textLayout->addWidget(descLabel);
    
    layout->addWidget(iconLabel);
    layout->addWidget(textWidget);
    layout->addStretch();
    
    return widget;
};

// Add rich content items
comboBox->addCustomItem("basic", createTemplateItem("Basic Template", "Simple layout with header and content", QIcon(":/templates/basic")));
comboBox->addCustomItem("advanced", createTemplateItem("Advanced Template", "Complex layout with sidebar and multiple sections", QIcon(":/templates/advanced")));
comboBox->addCustomItem("minimal", createTemplateItem("Minimal Template", "Clean and minimal design", QIcon(":/templates/minimal")));
```

## Animations

FluentComboBox includes smooth animations for interactions:

```cpp
auto* comboBox = new FluentComboBox;

// Enable/disable animations
comboBox->setAnimated(true);   // Default: enabled

// Configure animation settings
comboBox->setDropdownAnimationDuration(250);
comboBox->setDropdownAnimationEasing(FluentEasing::CubicOut);

// Custom animations for state changes
connect(comboBox, &FluentComboBox::dropdownOpened, [=]() {
    auto* animator = new FluentAnimator(this);
    animator->fadeIn(comboBox->dropdownWidget(), 200);
});

connect(comboBox, &FluentComboBox::dropdownClosed, [=]() {
    auto* animator = new FluentAnimator(this);
    animator->fadeOut(comboBox->dropdownWidget(), 150);
});
```

## Form Integration

### Complete Form Example

```cpp
auto* form = new FluentPanel("User Registration");
auto* layout = form->createVerticalLayout();

// Country selection
auto* countryCombo = new FluentComboBox;
countryCombo->setLabelText("Country");
countryCombo->setValidationType(FluentComboBoxValidation::Required);
countryCombo->addItems({"United States", "Canada", "United Kingdom", "Germany", "France"});

// Language preferences (multiple selection)
auto* languageCombo = new FluentComboBox;
languageCombo->setLabelText("Languages");
languageCombo->setSelectionMode(FluentComboBoxSelectionMode::Multiple);
languageCombo->addItems({"English", "Spanish", "French", "German", "Chinese", "Japanese"});

// Experience level
auto* experienceCombo = new FluentComboBox;
experienceCombo->setLabelText("Experience Level");
experienceCombo->setValidationType(FluentComboBoxValidation::Required);
experienceCombo->addItems({"Beginner", "Intermediate", "Advanced", "Expert"});

// Submit button
auto* submitButton = FluentButton::createPrimaryButton("Register");
submitButton->setEnabled(false);

layout->addWidget(countryCombo);
layout->addWidget(languageCombo);
layout->addWidget(experienceCombo);
layout->addWidget(submitButton);

// Form validation
auto validateForm = [=]() {
    bool isValid = countryCombo->isValid() && experienceCombo->isValid();
    submitButton->setEnabled(isValid);
};

connect(countryCombo, &FluentComboBox::validationStateChanged, validateForm);
connect(experienceCombo, &FluentComboBox::validationStateChanged, validateForm);

// Handle form submission
connect(submitButton, &FluentButton::clicked, [=]() {
    QString country = countryCombo->currentText();
    QStringList languages = languageCombo->selectedItems();
    QString experience = experienceCombo->currentText();
    
    // Process registration
    processRegistration(country, languages, experience);
});
```

## Accessibility

FluentComboBox provides comprehensive accessibility support:

```cpp
auto* comboBox = new FluentComboBox;
comboBox->setLabelText("Select Option");

// Accessible properties (automatically set)
comboBox->setAccessibleName("Option selector");
comboBox->setAccessibleDescription("Choose an option from the dropdown list");

// Keyboard navigation (automatic)
// - Tab to focus the combobox
// - Space/Enter to open dropdown
// - Arrow keys to navigate items
// - Escape to close dropdown
// - Type to search (if searchable)

// Screen reader support
comboBox->setAccessibleRole(QAccessible::ComboBox);

// Custom accessibility for items
for (int i = 0; i < comboBox->count(); ++i) {
    comboBox->setItemAccessibleText(i, QString("Option %1: %2").arg(i + 1).arg(comboBox->itemText(i)));
}
```

## Data Binding

### Model Integration

```cpp
// Create a custom model
auto* model = new QStandardItemModel;

// Populate model
QStringList countries = {"United States", "Canada", "United Kingdom"};
for (const QString& country : countries) {
    auto* item = new QStandardItem(country);
    item->setData(country, Qt::UserRole);
    model->appendRow(item);
}

// Set model to combobox
auto* comboBox = new FluentComboBox;
comboBox->setModel(model);

// Handle model changes
connect(model, &QStandardItemModel::dataChanged, [=]() {
    // Model data changed - combobox automatically updates
});
```

### Dynamic Content Updates

```cpp
auto* comboBox = new FluentComboBox;
comboBox->setLabelText("Select City");

// Function to update cities based on country
auto updateCities = [=](const QString& country) {
    comboBox->clear();
    
    QStringList cities;
    if (country == "United States") {
        cities = {"New York", "Los Angeles", "Chicago", "Houston"};
    } else if (country == "Canada") {
        cities = {"Toronto", "Vancouver", "Montreal", "Calgary"};
    } else if (country == "United Kingdom") {
        cities = {"London", "Manchester", "Birmingham", "Glasgow"};
    }
    
    // Animate the update
    auto* animator = new FluentAnimator(this);
    auto* fadeOut = animator->fadeOut(comboBox, 150);
    
    connect(fadeOut, &QPropertyAnimation::finished, [=]() {
        comboBox->addItems(cities);
        animator->fadeIn(comboBox, 150);
    });
    
    fadeOut->start();
};

// Connect to country selection change
connect(countryComboBox, &FluentComboBox::currentTextChanged, updateCities);
```

## Best Practices

### ComboBox Design Guidelines
1. **Use clear labels** - Describe what the user is selecting
2. **Provide appropriate defaults** - Pre-select common or recommended options
3. **Group related options** - Use separators or categories for long lists
4. **Enable search for long lists** - Make it easy to find options
5. **Validate selections** - Provide immediate feedback for required fields

### Performance Considerations
- Use virtual scrolling for very large item lists
- Implement lazy loading for dynamic content
- Cache frequently used item data
- Optimize custom delegates for smooth scrolling

### Accessibility Guidelines
- Provide meaningful labels and descriptions
- Support keyboard navigation
- Ensure sufficient color contrast
- Test with screen readers
- Use semantic markup for custom items

## Common Patterns

### Cascading ComboBoxes

```cpp
// Country -> State/Province -> City cascade
auto* countryCombo = new FluentComboBox;
auto* stateCombo = new FluentComboBox;
auto* cityCombo = new FluentComboBox;

countryCombo->setLabelText("Country");
stateCombo->setLabelText("State/Province");
cityCombo->setLabelText("City");

// Initially disable dependent comboboxes
stateCombo->setEnabled(false);
cityCombo->setEnabled(false);

// Update states when country changes
connect(countryCombo, &FluentComboBox::currentTextChanged, [=](const QString& country) {
    stateCombo->clear();
    cityCombo->clear();
    cityCombo->setEnabled(false);
    
    if (!country.isEmpty()) {
        QStringList states = getStatesForCountry(country);
        stateCombo->addItems(states);
        stateCombo->setEnabled(true);
    } else {
        stateCombo->setEnabled(false);
    }
});

// Update cities when state changes
connect(stateCombo, &FluentComboBox::currentTextChanged, [=](const QString& state) {
    cityCombo->clear();
    
    if (!state.isEmpty()) {
        QStringList cities = getCitiesForState(countryCombo->currentText(), state);
        cityCombo->addItems(cities);
        cityCombo->setEnabled(true);
    } else {
        cityCombo->setEnabled(false);
    }
});
```

## Signals Reference

```cpp
// Selection signals
void currentTextChanged(const QString& text);
void currentIndexChanged(int index);
void currentDataChanged(const QVariant& data);
void selectionChanged(const QStringList& selected); // Multiple selection mode

// Interaction signals
void dropdownOpened();
void dropdownClosed();
void searchTextChanged(const QString& text);

// Validation signals
void validationStateChanged(bool isValid);
```

## See Also

- [FluentTextInput](FluentTextInput.md) - For text input components
- [FluentCheckBox](FluentCheckBox.md) - For multiple selection options
- [FluentButton](FluentButton.md) - For action buttons
- [API Reference - FluentComboBox](../API_REFERENCE.md#fluentcombobox) - Complete API documentation
