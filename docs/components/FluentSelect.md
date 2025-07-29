# FluentSelect Component

The `FluentSelect` component provides a modern, feature-rich dropdown selection control that implements Microsoft's Fluent Design principles. It supports single and multiple selection modes, search functionality, custom items, grouping, and extensive customization options.

## Overview

FluentSelect offers:
- **Single and multiple selection modes**
- **Search and filtering capabilities** with customizable search modes
- **Rich item support** with icons, descriptions, and custom content
- **Item grouping and separators**
- **Editable mode** for custom input
- **Keyboard navigation** with full accessibility support
- **Animated dropdown** with smooth transitions
- **Validation support** with custom validators
- **Flexible styling** and theming integration

## Basic Usage

### Simple Dropdown

```cpp
#include "FluentQt/Components/FluentSelect.h"

// Create a basic select
auto* select = new FluentSelect();
select->setPlaceholderText("Choose an option...");

// Add items
select->addItem("Option 1");
select->addItem("Option 2");
select->addItem("Option 3");

// Handle selection changes
connect(select, &FluentSelect::currentTextChanged, [=](const QString& text) {
    qDebug() << "Selected:" << text;
});
```

### Select with Icons and Data

```cpp
auto* select = new FluentSelect();
select->setPlaceholderText("Select a file type...");

// Add items with icons and associated data
select->addItem(QIcon(":/icons/document.png"), "Document", "doc");
select->addItem(QIcon(":/icons/image.png"), "Image", "img");
select->addItem(QIcon(":/icons/video.png"), "Video", "vid");
select->addItem(QIcon(":/icons/audio.png"), "Audio", "aud");

// Get selected data
connect(select, &FluentSelect::currentDataChanged, [=](const QVariant& data) {
    QString fileType = data.toString();
    qDebug() << "Selected file type:" << fileType;
});
```

## Selection Modes

### Single Selection (Default)

```cpp
auto* singleSelect = FluentSelect::createSingleSelect();
singleSelect->addItems({"Red", "Green", "Blue", "Yellow"});

connect(singleSelect, &FluentSelect::currentIndexChanged, [=](int index, int previous) {
    qDebug() << "Selection changed from" << previous << "to" << index;
});
```

### Multiple Selection

```cpp
auto* multiSelect = FluentSelect::createMultiSelect();
multiSelect->addItems({"Apple", "Banana", "Cherry", "Date", "Elderberry"});

connect(multiSelect, &FluentSelect::selectionChanged, [=]() {
    QStringList selected = multiSelect->selectedTexts();
    qDebug() << "Selected items:" << selected;
});

// Select multiple items programmatically
multiSelect->setSelectedIndexes({0, 2, 4}); // Select Apple, Cherry, Elderberry
```

## Search and Filtering

### Searchable Select

```cpp
auto* searchableSelect = FluentSelect::createSearchableSelect();
searchableSelect->setPlaceholderText("Search countries...");

// Add many items for searching
QStringList countries = {"United States", "United Kingdom", "Germany", "France", "Japan", "Australia"};
searchableSelect->addItems(countries);

// Configure search behavior
searchableSelect->setSearchMode(FluentSelectSearchMode::Contains); // Default
// searchableSelect->setSearchMode(FluentSelectSearchMode::StartsWith);

connect(searchableSelect, &FluentSelect::searchFilterChanged, [=](const QString& filter) {
    qDebug() << "Search filter:" << filter;
});
```

### Editable Select

```cpp
auto* editableSelect = FluentSelect::createEditableSelect();
editableSelect->setPlaceholderText("Type or select...");
editableSelect->addItems({"Preset 1", "Preset 2", "Preset 3"});

// Allow custom input
connect(editableSelect, &FluentSelect::currentTextChanged, [=](const QString& text) {
    if (!editableSelect->findText(text)) {
        qDebug() << "Custom input:" << text;
    }
});
```

### Custom Search Function

```cpp
auto* customSearchSelect = new FluentSelect();
customSearchSelect->addItems({"John Doe", "Jane Smith", "Bob Johnson", "Alice Brown"});

// Set custom search function
customSearchSelect->setCustomSearchFunction([](const FluentSelectItem& item, const QString& searchText) {
    // Custom search logic - search in both text and description
    return item.text().contains(searchText, Qt::CaseInsensitive) ||
           item.description().contains(searchText, Qt::CaseInsensitive);
});
```

## Rich Items and Grouping

### Items with Descriptions

```cpp
auto* select = new FluentSelect();

// Create items with descriptions
FluentSelectItem item1("Premium Plan");
item1.setDescription("Full access to all features");
item1.setIcon(QIcon(":/icons/premium.png"));
select->addItem(item1);

FluentSelectItem item2("Basic Plan");
item2.setDescription("Limited feature access");
item2.setIcon(QIcon(":/icons/basic.png"));
select->addItem(item2);

FluentSelectItem item3("Free Plan");
item3.setDescription("Basic functionality only");
item3.setIcon(QIcon(":/icons/free.png"));
item3.setEnabled(false); // Disabled item
select->addItem(item3);
```

### Grouped Items

```cpp
auto* groupedSelect = new FluentSelect();

// Add groups with items
groupedSelect->addGroup("Fruits");
groupedSelect->addItem(QIcon(":/icons/apple.png"), "Apple", "fruit_apple");
groupedSelect->addItem(QIcon(":/icons/banana.png"), "Banana", "fruit_banana");
groupedSelect->addItem(QIcon(":/icons/orange.png"), "Orange", "fruit_orange");

groupedSelect->addSeparator();

groupedSelect->addGroup("Vegetables");
groupedSelect->addItem(QIcon(":/icons/carrot.png"), "Carrot", "veg_carrot");
groupedSelect->addItem(QIcon(":/icons/broccoli.png"), "Broccoli", "veg_broccoli");
groupedSelect->addItem(QIcon(":/icons/spinach.png"), "Spinach", "veg_spinach");
```

### Checkable Items (Multiple Selection)

```cpp
auto* checkableSelect = new FluentSelect(FluentSelectMode::Multiple);

// Add checkable items
FluentSelectItem feature1 = FluentSelectItem::createCheckableItem("Dark Mode", false);
FluentSelectItem feature2 = FluentSelectItem::createCheckableItem("Notifications", true);
FluentSelectItem feature3 = FluentSelectItem::createCheckableItem("Auto-save", true);

checkableSelect->addItem(feature1);
checkableSelect->addItem(feature2);
checkableSelect->addItem(feature3);

// Handle checkbox changes
connect(checkableSelect, &FluentSelect::selectionChanged, [=]() {
    QStringList enabled = checkableSelect->selectedTexts();
    qDebug() << "Enabled features:" << enabled;
});
```

## Configuration and Styling

### Size Variants

```cpp
// Small select
auto* smallSelect = new FluentSelect();
smallSelect->setSelectSize(FluentSelectSize::Small);
smallSelect->addItems({"Small 1", "Small 2", "Small 3"});

// Medium select (default)
auto* mediumSelect = new FluentSelect();
mediumSelect->setSelectSize(FluentSelectSize::Medium);
mediumSelect->addItems({"Medium 1", "Medium 2", "Medium 3"});

// Large select
auto* largeSelect = new FluentSelect();
largeSelect->setSelectSize(FluentSelectSize::Large);
largeSelect->addItems({"Large 1", "Large 2", "Large 3"});
```

### Custom Configuration

```cpp
FluentSelectConfig config;
config.mode = FluentSelectMode::Multiple;
config.searchMode = FluentSelectSearchMode::Contains;
config.size = FluentSelectSize::Medium;
config.editable = true;
config.clearable = true;
config.searchable = true;
config.maxVisibleItems = 8;
config.maxDropdownHeight = 300;
config.placeholderText = "Select multiple options...";

auto* customSelect = new FluentSelect(config);
customSelect->addItems({"Option A", "Option B", "Option C", "Option D", "Option E"});
```

### Custom Colors

```cpp
FluentSelectConfig config;
config.autoCalculateColors = false;
config.customBackgroundColor = QColor("#2D3748");
config.customTextColor = QColor("#E2E8F0");
config.customBorderColor = QColor("#4A5568");

auto* themedSelect = new FluentSelect(config);
themedSelect->addItems({"Dark Theme", "Light Theme", "Auto Theme"});
```

## Validation

### Built-in Validation

```cpp
auto* validatedSelect = new FluentSelect();
validatedSelect->addItems({"Valid Option 1", "Valid Option 2", "Invalid Option"});

// Set validator
validatedSelect->setValidator([](const QVariant& data) {
    QString text = data.toString();
    return !text.contains("Invalid");
});

validatedSelect->setValidationErrorMessage("Please select a valid option.");

// Check validation
connect(validatedSelect, &FluentSelect::currentDataChanged, [=](const QVariant& data) {
    if (validatedSelect->isValid()) {
        qDebug() << "Valid selection:" << data.toString();
    } else {
        qDebug() << "Invalid selection:" << validatedSelect->validationError();
    }
});
```

## Keyboard Navigation

FluentSelect supports comprehensive keyboard navigation:

- **Arrow Keys**: Navigate through items
- **Enter/Space**: Select current item
- **Escape**: Close dropdown
- **Home/End**: Jump to first/last item
- **Page Up/Down**: Navigate by page
- **Type to search**: Quick item finding

```cpp
auto* keyboardSelect = new FluentSelect();
keyboardSelect->addItems({"Alpha", "Beta", "Gamma", "Delta", "Epsilon"});

// Enable keyboard navigation (enabled by default)
keyboardSelect->setFocusPolicy(Qt::StrongFocus);

// Handle keyboard events
connect(keyboardSelect, &FluentSelect::dropdownShown, [=]() {
    qDebug() << "Dropdown opened - keyboard navigation active";
});
```

## Advanced Features

### Dynamic Item Management

```cpp
auto* dynamicSelect = new FluentSelect();

// Add initial items
dynamicSelect->addItems({"Item 1", "Item 2", "Item 3"});

// Add items dynamically
QPushButton* addButton = new QPushButton("Add Item");
connect(addButton, &QPushButton::clicked, [=]() {
    static int counter = 4;
    dynamicSelect->addItem(QString("Item %1").arg(counter++));
});

// Remove items dynamically
QPushButton* removeButton = new QPushButton("Remove Selected");
connect(removeButton, &QPushButton::clicked, [=]() {
    int currentIndex = dynamicSelect->currentIndex();
    if (currentIndex >= 0) {
        dynamicSelect->removeItem(currentIndex);
    }
});

// Clear all items
QPushButton* clearButton = new QPushButton("Clear All");
connect(clearButton, &QPushButton::clicked, [=]() {
    dynamicSelect->clear();
});
```

### Dropdown Control

```cpp
auto* controlledSelect = new FluentSelect();
controlledSelect->addItems({"Option 1", "Option 2", "Option 3"});

// Programmatic dropdown control
QPushButton* showButton = new QPushButton("Show Dropdown");
connect(showButton, &QPushButton::clicked, [=]() {
    controlledSelect->showDropdown();
});

QPushButton* hideButton = new QPushButton("Hide Dropdown");
connect(hideButton, &QPushButton::clicked, [=]() {
    controlledSelect->hideDropdown();
});

// Handle dropdown events
connect(controlledSelect, &FluentSelect::dropdownAboutToShow, [=]() {
    qDebug() << "Dropdown about to show";
});

connect(controlledSelect, &FluentSelect::dropdownShown, [=]() {
    qDebug() << "Dropdown shown";
});

connect(controlledSelect, &FluentSelect::dropdownAboutToHide, [=]() {
    qDebug() << "Dropdown about to hide";
});

connect(controlledSelect, &FluentSelect::dropdownHidden, [=]() {
    qDebug() << "Dropdown hidden";
});
```

### Selection Management

```cpp
auto* managedSelect = new FluentSelect(FluentSelectMode::Multiple);
managedSelect->addItems({"A", "B", "C", "D", "E", "F", "G", "H"});

// Selection operations
QPushButton* selectAllButton = new QPushButton("Select All");
connect(selectAllButton, &QPushButton::clicked, [=]() {
    managedSelect->selectAll();
});

QPushButton* clearSelectionButton = new QPushButton("Clear Selection");
connect(clearSelectionButton, &QPushButton::clicked, [=]() {
    managedSelect->clearSelection();
});

QPushButton* invertSelectionButton = new QPushButton("Invert Selection");
connect(invertSelectionButton, &QPushButton::clicked, [=]() {
    managedSelect->invertSelection();
});

// Get selection information
connect(managedSelect, &FluentSelect::selectionChanged, [=]() {
    QList<int> selected = managedSelect->selectedIndexes();
    QStringList texts = managedSelect->selectedTexts();
    qDebug() << "Selected indexes:" << selected;
    qDebug() << "Selected texts:" << texts;
});
```

## Integration Examples

### Form Integration

```cpp
class UserProfileForm : public QWidget {
public:
    UserProfileForm(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
    }

private:
    void setupUI() {
        auto* layout = new QFormLayout(this);
        
        // Country selection
        m_countrySelect = new FluentSelect();
        m_countrySelect->setPlaceholderText("Select your country...");
        m_countrySelect->addItems({"United States", "Canada", "United Kingdom", "Germany", "France"});
        layout->addRow("Country:", m_countrySelect);
        
        // Language selection (multiple)
        m_languageSelect = new FluentSelect(FluentSelectMode::Multiple);
        m_languageSelect->setPlaceholderText("Select languages...");
        m_languageSelect->addItems({"English", "Spanish", "French", "German", "Chinese", "Japanese"});
        layout->addRow("Languages:", m_languageSelect);
        
        // Skills selection with search
        m_skillsSelect = FluentSelect::createSearchableSelect();
        m_skillsSelect->setPlaceholderText("Search and select skills...");
        m_skillsSelect->addItems({"C++", "Python", "JavaScript", "Java", "C#", "Go", "Rust", "Swift"});
        layout->addRow("Skills:", m_skillsSelect);
        
        // Experience level
        m_experienceSelect = new FluentSelect();
        m_experienceSelect->addItem(QIcon(":/icons/beginner.png"), "Beginner", 1);
        m_experienceSelect->addItem(QIcon(":/icons/intermediate.png"), "Intermediate", 2);
        m_experienceSelect->addItem(QIcon(":/icons/advanced.png"), "Advanced", 3);
        m_experienceSelect->addItem(QIcon(":/icons/expert.png"), "Expert", 4);
        layout->addRow("Experience:", m_experienceSelect);
        
        // Save button
        auto* saveButton = new QPushButton("Save Profile");
        connect(saveButton, &QPushButton::clicked, this, &UserProfileForm::saveProfile);
        layout->addRow(saveButton);
    }
    
    void saveProfile() {
        QString country = m_countrySelect->currentText();
        QStringList languages = m_languageSelect->selectedTexts();
        QStringList skills = m_skillsSelect->selectedTexts();
        int experienceLevel = m_experienceSelect->currentData().toInt();
        
        qDebug() << "Country:" << country;
        qDebug() << "Languages:" << languages;
        qDebug() << "Skills:" << skills;
        qDebug() << "Experience Level:" << experienceLevel;
        
        // Save to database or send to server
    }
    
private:
    FluentSelect* m_countrySelect;
    FluentSelect* m_languageSelect;
    FluentSelect* m_skillsSelect;
    FluentSelect* m_experienceSelect;
};
```

### Data Binding

```cpp
class DataBoundSelect : public QWidget {
public:
    DataBoundSelect(QWidget* parent = nullptr) : QWidget(parent) {
        m_select = new FluentSelect();
        
        // Bind to data model
        bindToModel();
        
        auto* layout = new QVBoxLayout(this);
        layout->addWidget(m_select);
    }
    
private:
    void bindToModel() {
        // Create custom model
        auto* model = new FluentSelectModel(this);
        
        // Add items from data source
        QList<User> users = getUsersFromDatabase();
        for (const User& user : users) {
            FluentSelectItem item(user.name, user.id);
            item.setDescription(user.email);
            item.setIcon(loadUserAvatar(user.id));
            model->addItem(item);
        }
        
        m_select->setModel(model);
        
        // Handle selection
        connect(m_select, &FluentSelect::currentDataChanged, [=](const QVariant& data) {
            int userId = data.toInt();
            User selectedUser = getUserById(userId);
            emit userSelected(selectedUser);
        });
    }
    
signals:
    void userSelected(const User& user);
    
private:
    FluentSelect* m_select;
    
    struct User {
        int id;
        QString name;
        QString email;
    };
    
    QList<User> getUsersFromDatabase() { /* Implementation */ return {}; }
    User getUserById(int id) { /* Implementation */ return {}; }
    QIcon loadUserAvatar(int userId) { /* Implementation */ return {}; }
};
```

## Accessibility

FluentSelect provides comprehensive accessibility support:

```cpp
auto* accessibleSelect = new FluentSelect();
accessibleSelect->setPlaceholderText("Accessible dropdown");
accessibleSelect->addItems({"Option 1", "Option 2", "Option 3"});

// Accessibility is automatically configured
// Screen readers will announce:
// - The current selection
// - Available options when dropdown is opened
// - Selection changes
// - Keyboard shortcuts

// Custom accessibility labels
accessibleSelect->setAccessibleName("File type selector");
accessibleSelect->setAccessibleDescription("Choose the type of file to create");
```

## Best Practices

1. **Use clear placeholder text** that describes what users should select
2. **Group related items** to improve organization
3. **Provide search functionality** for lists with many items
4. **Use icons consistently** to improve visual recognition
5. **Validate selections** when appropriate
6. **Handle empty states** gracefully
7. **Consider keyboard users** - ensure full keyboard accessibility
8. **Use appropriate selection modes** (single vs. multiple)
9. **Provide clear feedback** for validation errors
10. **Test with screen readers** to ensure accessibility

## API Reference

For complete API documentation, see the header files:
- `FluentQt/Components/FluentSelect.h`
- `FluentQt/Components/FluentSelectItem.h`
- `FluentQt/Components/FluentSelectDropdown.h`

## See Also

- [FluentComboBox](FluentComboBox.md) - For simpler dropdown needs
- [FluentListView](FluentListView.md) - For list-based selection
- [FluentTextInput](FluentTextInput.md) - For text input with validation
- [FluentButton](FluentButton.md) - For action buttons
