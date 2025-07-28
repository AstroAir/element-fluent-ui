# Element Fluent UI - Migration Guide

This guide helps developers migrate from other Qt UI libraries or previous versions to Element Fluent UI. It provides step-by-step instructions, code examples, and best practices for a smooth transition.

## Table of Contents

1. [Migration from Standard Qt Widgets](#migration-from-standard-qt-widgets)
2. [Migration from Qt Quick/QML](#migration-from-qt-quickqml)
3. [Migration from Other UI Libraries](#migration-from-other-ui-libraries)
4. [Version Migration](#version-migration)
5. [Common Migration Patterns](#common-migration-patterns)
6. [Troubleshooting](#troubleshooting)

## Migration from Standard Qt Widgets

### Overview

Element Fluent UI is designed to be a drop-in replacement for many standard Qt widgets while providing enhanced functionality and modern styling.

### Button Migration

**Before (QPushButton):**
```cpp
#include <QtWidgets/QPushButton>

auto* button = new QPushButton("Click Me", this);
button->setIcon(QIcon(":/icons/action"));
button->setEnabled(true);

connect(button, &QPushButton::clicked, this, &MyClass::onButtonClicked);
```

**After (FluentButton):**
```cpp
#include "FluentQt/Components/FluentButton.h"

auto* button = new FluentButton("Click Me", this);
button->setIcon(QIcon(":/icons/action"));
button->setButtonStyle(FluentButtonStyle::Primary);  // Enhanced styling
button->setEnabled(true);

connect(button, &FluentButton::clicked, this, &MyClass::onButtonClicked);
```

**Key Differences:**
- Enhanced visual styles (Primary, Accent, Subtle, etc.)
- Built-in animations and hover effects
- Better accessibility support
- Consistent theming integration

### Text Input Migration

**Before (QLineEdit):**
```cpp
#include <QtWidgets/QLineEdit>

auto* lineEdit = new QLineEdit(this);
lineEdit->setPlaceholderText("Enter text...");
lineEdit->setMaxLength(100);

// Manual validation
connect(lineEdit, &QLineEdit::textChanged, [=](const QString& text) {
    if (text.isEmpty()) {
        lineEdit->setStyleSheet("border: 2px solid red;");
    } else {
        lineEdit->setStyleSheet("");
    }
});
```

**After (FluentTextInput):**
```cpp
#include "FluentQt/Components/FluentTextInput.h"

auto* textInput = new FluentTextInput(this);
textInput->setPlaceholderText("Enter text...");
textInput->setMaxLength(100);
textInput->setLabelText("Name");  // Built-in label support
textInput->setValidationType(FluentTextInputValidation::Required);  // Built-in validation

// Automatic validation with visual feedback
connect(textInput, &FluentTextInput::validationStateChanged, [=](bool isValid) {
    if (isValid) {
        textInput->setHelperText("Looks good!");
    }
});
```

**Key Differences:**
- Built-in validation system
- Label and helper text support
- Multiple input types (Email, Password, etc.)
- Automatic error state styling

### Layout Migration

**Before (Standard Layouts):**
```cpp
auto* widget = new QWidget;
auto* layout = new QVBoxLayout(widget);

auto* button1 = new QPushButton("Button 1");
auto* button2 = new QPushButton("Button 2");

layout->addWidget(button1);
layout->addWidget(button2);
```

**After (FluentPanel with Layout Helpers):**
```cpp
auto* panel = new FluentPanel("Button Group");
panel->setPanelType(FluentPanelType::Card);

auto* layout = panel->createVerticalLayout();  // Built-in layout helper

auto* button1 = new FluentButton("Button 1");
auto* button2 = new FluentButton("Button 2");

layout->addWidget(button1);
layout->addWidget(button2);
```

**Key Differences:**
- Automatic theming and styling
- Built-in elevation and shadows
- Layout helpers for common patterns
- Consistent spacing and margins

### Complete Form Migration Example

**Before (Standard Qt):**
```cpp
class LoginForm : public QWidget
{
public:
    LoginForm(QWidget* parent = nullptr) : QWidget(parent) {
        auto* layout = new QVBoxLayout(this);
        
        // Username
        auto* usernameLabel = new QLabel("Username:");
        m_usernameEdit = new QLineEdit;
        m_usernameEdit->setPlaceholderText("Enter username");
        
        // Password
        auto* passwordLabel = new QLabel("Password:");
        m_passwordEdit = new QLineEdit;
        m_passwordEdit->setEchoMode(QLineEdit::Password);
        m_passwordEdit->setPlaceholderText("Enter password");
        
        // Buttons
        auto* buttonLayout = new QHBoxLayout;
        m_loginButton = new QPushButton("Login");
        m_cancelButton = new QPushButton("Cancel");
        
        buttonLayout->addWidget(m_cancelButton);
        buttonLayout->addStretch();
        buttonLayout->addWidget(m_loginButton);
        
        layout->addWidget(usernameLabel);
        layout->addWidget(m_usernameEdit);
        layout->addWidget(passwordLabel);
        layout->addWidget(m_passwordEdit);
        layout->addLayout(buttonLayout);
        
        // Manual styling
        setStyleSheet(R"(
            QWidget { background-color: white; }
            QLineEdit { 
                border: 1px solid #ccc; 
                padding: 8px; 
                border-radius: 4px; 
            }
            QPushButton { 
                padding: 8px 16px; 
                border-radius: 4px; 
            }
        )");
    }
    
private:
    QLineEdit* m_usernameEdit;
    QLineEdit* m_passwordEdit;
    QPushButton* m_loginButton;
    QPushButton* m_cancelButton;
};
```

**After (Element Fluent UI):**
```cpp
class LoginForm : public FluentPanel
{
public:
    LoginForm(QWidget* parent = nullptr) : FluentPanel("Login", parent) {
        setPanelType(FluentPanelType::Card);
        setElevation(FluentPanelElevation::Medium);
        
        auto* layout = createVerticalLayout();  // Built-in helper
        
        // Username with built-in label and validation
        m_usernameInput = new FluentTextInput;
        m_usernameInput->setLabelText("Username");
        m_usernameInput->setPlaceholderText("Enter username");
        m_usernameInput->setValidationType(FluentTextInputValidation::Required);
        
        // Password with built-in type and validation
        m_passwordInput = new FluentTextInput;
        m_passwordInput->setLabelText("Password");
        m_passwordInput->setPlaceholderText("Enter password");
        m_passwordInput->setInputType(FluentTextInputType::Password);
        m_passwordInput->setValidationType(FluentTextInputValidation::Required);
        
        // Buttons with enhanced styling
        auto* buttonLayout = createHorizontalLayout();
        m_loginButton = FluentButton::createPrimaryButton("Login");
        m_cancelButton = new FluentButton("Cancel");
        m_cancelButton->setButtonStyle(FluentButtonStyle::Subtle);
        
        buttonLayout->addWidget(m_cancelButton);
        buttonLayout->addStretch();
        buttonLayout->addWidget(m_loginButton);
        
        layout->addWidget(m_usernameInput);
        layout->addWidget(m_passwordInput);
        layout->addLayout(buttonLayout);
        
        // Automatic theming - no manual styling needed!
        // Form validation
        setupValidation();
    }
    
private:
    void setupValidation() {
        auto validateForm = [this]() {
            bool isValid = m_usernameInput->isValid() && m_passwordInput->isValid();
            m_loginButton->setEnabled(isValid);
        };
        
        connect(m_usernameInput, &FluentTextInput::validationStateChanged, validateForm);
        connect(m_passwordInput, &FluentTextInput::validationStateChanged, validateForm);
    }
    
    FluentTextInput* m_usernameInput;
    FluentTextInput* m_passwordInput;
    FluentButton* m_loginButton;
    FluentButton* m_cancelButton;
};
```

## Migration from Qt Quick/QML

### Architecture Differences

**QML Approach:**
- Declarative UI definition
- Property bindings
- State-based animations
- Component composition

**Element Fluent UI Approach:**
- Imperative C++ API
- Signal-slot connections
- Animation system integration
- Object-oriented composition

### Component Mapping

| QML Component | Element Fluent UI Equivalent |
|---------------|------------------------------|
| `Button` | `FluentButton` |
| `TextField` | `FluentTextInput` |
| `CheckBox` | `FluentCheckBox` |
| `RadioButton` | `FluentRadioButton` |
| `ComboBox` | `FluentComboBox` |
| `Slider` | `FluentSlider` |
| `ProgressBar` | `FluentProgressBar` |
| `Rectangle` | `FluentPanel` |
| `ScrollView` | `QScrollArea` with Fluent styling |

### Migration Example

**Before (QML):**
```qml
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    width: 400
    height: 300
    
    Column {
        anchors.centerIn: parent
        spacing: 20
        
        TextField {
            id: nameField
            placeholderText: "Enter name"
            width: 200
        }
        
        Button {
            text: "Submit"
            enabled: nameField.text.length > 0
            onClicked: console.log("Submitted:", nameField.text)
        }
    }
}
```

**After (C++ with Element Fluent UI):**
```cpp
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentPanel.h"

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        auto* centralWidget = new FluentPanel;
        centralWidget->setPanelType(FluentPanelType::Content);
        setCentralWidget(centralWidget);
        
        auto* layout = centralWidget->createVerticalLayout();
        layout->setAlignment(Qt::AlignCenter);
        layout->setSpacing(20);
        
        // Text input
        m_nameField = new FluentTextInput;
        m_nameField->setPlaceholderText("Enter name");
        m_nameField->setFixedWidth(200);
        
        // Submit button
        m_submitButton = FluentButton::createPrimaryButton("Submit");
        m_submitButton->setEnabled(false);
        
        layout->addWidget(m_nameField);
        layout->addWidget(m_submitButton);
        
        // Enable button when text is entered
        connect(m_nameField, &FluentTextInput::textChanged, [this](const QString& text) {
            m_submitButton->setEnabled(!text.isEmpty());
        });
        
        connect(m_submitButton, &FluentButton::clicked, [this]() {
            qDebug() << "Submitted:" << m_nameField->text();
        });
        
        resize(400, 300);
    }
    
private:
    FluentTextInput* m_nameField;
    FluentButton* m_submitButton;
};
```

## Migration from Other UI Libraries

### From Material Design Libraries

**Key Differences:**
- Fluent Design vs Material Design principles
- Different color schemes and typography
- Animation timing and easing differences

**Migration Steps:**
1. Replace Material components with Fluent equivalents
2. Update color schemes using `FluentTheme`
3. Adjust animation timings
4. Update spacing and sizing

### From Bootstrap/Web UI

**Conceptual Mapping:**
- Bootstrap cards → `FluentPanel` with Card type
- Bootstrap buttons → `FluentButton` with appropriate styles
- Bootstrap forms → `FluentTextInput` with validation
- Bootstrap navigation → `FluentNavigationView`

## Version Migration

### From Version 1.0 to 1.1

**Breaking Changes:**
- `FluentTheme::setAccentColor()` now takes enum instead of QColor
- `FluentAnimator` static methods require config parameter

**Migration Steps:**

**Before (v1.0):**
```cpp
FluentTheme::instance().setAccentColor(QColor(0, 120, 215));
auto* animation = FluentAnimator::fadeIn(widget);
```

**After (v1.1):**
```cpp
FluentTheme::instance().setAccentColor(FluentAccentColor::Blue);
// or for custom colors:
FluentTheme::instance().setCustomAccentColor(QColor(0, 120, 215));

FluentAnimationConfig config;
config.duration = 300ms;
auto* animation = FluentAnimator::fadeIn(widget, config);
```

### From Version 1.1 to 1.2

**New Features:**
- Enhanced accessibility support
- New component states
- Improved theming system

**Recommended Updates:**
```cpp
// Add accessibility descriptions
button->setAccessibleName("Save Document");
button->setAccessibleDescription("Saves the current document to disk");

// Use new validation states
textInput->setInputState(FluentTextInputState::Success);
textInput->setHelperText("Email format is valid");
```

## Common Migration Patterns

### Theme Integration

**Before (Manual Styling):**
```cpp
button->setStyleSheet("background-color: #0078d4; color: white;");
```

**After (Theme Integration):**
```cpp
// Automatic theme integration
button->setButtonStyle(FluentButtonStyle::Primary);

// Or manual theme usage
auto& theme = FluentTheme::instance();
button->setStyleSheet(QString("background-color: %1; color: %2;")
    .arg(theme.color("accent").name())
    .arg(theme.color("textOnAccent").name()));
```

### Animation Migration

**Before (Manual QPropertyAnimation):**
```cpp
auto* animation = new QPropertyAnimation(widget, "opacity");
animation->setDuration(300);
animation->setStartValue(0.0);
animation->setEndValue(1.0);
animation->setEasingCurve(QEasingCurve::OutCubic);
animation->start();
```

**After (FluentAnimator):**
```cpp
auto* animation = FluentAnimator::fadeIn(widget);
animation->start();
```

### Validation Migration

**Before (Manual Validation):**
```cpp
connect(lineEdit, &QLineEdit::textChanged, [=](const QString& text) {
    QRegExp emailRegex("\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Z|a-z]{2,}\\b");
    if (emailRegex.exactMatch(text)) {
        lineEdit->setStyleSheet("border: 2px solid green;");
    } else {
        lineEdit->setStyleSheet("border: 2px solid red;");
    }
});
```

**After (Built-in Validation):**
```cpp
textInput->setInputType(FluentTextInputType::Email);
textInput->setValidationType(FluentTextInputValidation::Email);

connect(textInput, &FluentTextInput::validationStateChanged, [=](bool isValid) {
    if (isValid) {
        textInput->setInputState(FluentTextInputState::Success);
        textInput->setHelperText("Email format is valid");
    } else {
        textInput->setInputState(FluentTextInputState::Error);
        textInput->setErrorText("Please enter a valid email address");
    }
});
```

## Troubleshooting

### Common Issues

**Issue: Components don't appear styled**
```cpp
// Solution: Ensure theme is initialized
#include "FluentQt/Styling/FluentTheme.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Initialize theme before creating components
    FluentTheme::instance().setMode(FluentThemeMode::System);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
```

**Issue: Animations not working**
```cpp
// Solution: Check if animations are enabled
component->setAnimated(true);

// Or check system animation settings
FluentAnimationConfig config;
config.respectReducedMotion = true;  // Respects system preferences
```

**Issue: Layout spacing issues**
```cpp
// Solution: Use FluentPanel layout helpers
auto* panel = new FluentPanel;
auto* layout = panel->createVerticalLayout();  // Automatic spacing

// Instead of:
auto* layout = new QVBoxLayout;
layout->setSpacing(16);  // Manual spacing
```

### Performance Considerations

**Memory Usage:**
- Element Fluent UI components may use more memory due to enhanced features
- Use object pools for frequently created/destroyed components
- Consider lazy loading for complex UIs

**Rendering Performance:**
- Enable hardware acceleration where available
- Use appropriate elevation levels (higher elevation = more rendering cost)
- Cache complex backgrounds and effects

### Migration Checklist

- [ ] Replace standard Qt widgets with Fluent equivalents
- [ ] Update styling to use FluentTheme instead of manual stylesheets
- [ ] Replace manual animations with FluentAnimator
- [ ] Add proper accessibility attributes
- [ ] Update validation logic to use built-in validation
- [ ] Test with both light and dark themes
- [ ] Verify keyboard navigation works correctly
- [ ] Test with screen readers
- [ ] Performance test with large datasets
- [ ] Update documentation and examples

## Getting Help

### Resources

- **Documentation**: Check component-specific documentation
- **Examples**: Review example applications in the `examples/` directory
- **API Reference**: Consult the complete API documentation
- **Community**: Join discussions on GitHub Issues

### Migration Support

For complex migrations or specific questions:

1. **Search existing issues** on GitHub
2. **Create a new issue** with migration questions
3. **Provide code examples** of current and desired implementations
4. **Include system information** (Qt version, platform, etc.)

---

This migration guide covers the most common scenarios. For specific use cases not covered here, please refer to the component documentation or create an issue for additional guidance.
