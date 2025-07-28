# FluentTextInput Component

The `FluentTextInput` component is a modern, feature-rich text input field that implements Microsoft's Fluent Design principles. It provides validation, multiple input types, states, animations, and comprehensive accessibility support.

## Overview

FluentTextInput offers:
- **Multiple input types** (Text, Password, Email, URL, Number, Phone, Search, Multiline)
- **Built-in validation** with visual feedback
- **Rich states** (Normal, Focused, Error, Warning, Success, Disabled)
- **Label and helper text** support
- **Clear button** functionality
- **Smooth animations** for state transitions
- **Full accessibility** compliance

## Basic Usage

### Simple Text Input

```cpp
#include "FluentQt/Components/FluentTextInput.h"

// Create a basic text input
auto* input = new FluentTextInput("Enter your name...");

// With label
auto* labeledInput = new FluentTextInput();
labeledInput->setLabelText("Full Name");
labeledInput->setPlaceholderText("Enter your full name");
```

### Input with Helper Text

```cpp
auto* input = new FluentTextInput();
input->setLabelText("Username");
input->setPlaceholderText("Enter username");
input->setHelperText("Must be 3-20 characters, letters and numbers only");
```

## Input Types

### Text Input (Default)

```cpp
auto* textInput = new FluentTextInput("Enter text...");
textInput->setInputType(FluentTextInputType::Text);
```

### Password Input

```cpp
auto* passwordInput = new FluentTextInput("Enter password...");
passwordInput->setInputType(FluentTextInputType::Password);
// Automatically includes show/hide password toggle
```

### Email Input

```cpp
auto* emailInput = new FluentTextInput("Enter email address...");
emailInput->setInputType(FluentTextInputType::Email);
emailInput->setValidationType(FluentTextInputValidation::Email);
```

### URL Input

```cpp
auto* urlInput = new FluentTextInput("Enter website URL...");
urlInput->setInputType(FluentTextInputType::Url);
urlInput->setValidationType(FluentTextInputValidation::Url);
```

### Number Input

```cpp
auto* numberInput = new FluentTextInput("Enter amount...");
numberInput->setInputType(FluentTextInputType::Number);
numberInput->setValidationType(FluentTextInputValidation::Number);
```

### Phone Input

```cpp
auto* phoneInput = new FluentTextInput("Enter phone number...");
phoneInput->setInputType(FluentTextInputType::Phone);
phoneInput->setValidationType(FluentTextInputValidation::Phone);
```

### Search Input

```cpp
auto* searchInput = new FluentTextInput("Search...");
searchInput->setInputType(FluentTextInputType::Search);
// Automatically includes search icon
```

### Multiline Input

```cpp
auto* textArea = new FluentTextInput();
textArea->setInputType(FluentTextInputType::Multiline);
textArea->setLabelText("Description");
textArea->setPlaceholderText("Enter detailed description...");
textArea->setMaxLength(500);
```

## Validation

### Built-in Validation Types

```cpp
auto* emailInput = new FluentTextInput("Email address...");

// Email validation
emailInput->setValidationType(FluentTextInputValidation::Email);

// Required field validation
emailInput->setValidationType(FluentTextInputValidation::Required);

// URL validation
auto* urlInput = new FluentTextInput("Website URL...");
urlInput->setValidationType(FluentTextInputValidation::Url);

// Number validation
auto* ageInput = new FluentTextInput("Age...");
ageInput->setValidationType(FluentTextInputValidation::Number);

// Phone validation
auto* phoneInput = new FluentTextInput("Phone...");
phoneInput->setValidationType(FluentTextInputValidation::Phone);
```

### Custom Validation

```cpp
auto* passwordInput = new FluentTextInput("Password...");
passwordInput->setValidationType(FluentTextInputValidation::Custom);

// Set custom validator function
passwordInput->setCustomValidator([](const QString& text) {
    // Password must be at least 8 characters with at least one number
    return text.length() >= 8 && text.contains(QRegExp("[0-9]"));
});

// Handle validation state changes
connect(passwordInput, &FluentTextInput::validationStateChanged, [=](bool isValid) {
    if (isValid) {
        passwordInput->setHelperText("Password meets requirements");
        passwordInput->setInputState(FluentTextInputState::Success);
    } else {
        passwordInput->setErrorText("Password must be at least 8 characters with a number");
        passwordInput->setInputState(FluentTextInputState::Error);
    }
});
```

### Manual Validation

```cpp
auto* input = new FluentTextInput("Enter value...");

// Trigger validation manually
bool isValid = input->isValid();

// Force validation
input->validate();

// Clear validation state
input->clearValidation();
```

## States and Visual Feedback

### Input States

```cpp
auto* input = new FluentTextInput("Enter text...");

// Normal state (default)
input->setInputState(FluentTextInputState::Normal);

// Focused state (automatic when input gains focus)
input->setInputState(FluentTextInputState::Focused);

// Error state
input->setInputState(FluentTextInputState::Error);
input->setErrorText("This field is required");

// Warning state
input->setInputState(FluentTextInputState::Warning);
input->setHelperText("Please double-check this value");

// Success state
input->setInputState(FluentTextInputState::Success);
input->setHelperText("Looks good!");

// Disabled state
input->setInputState(FluentTextInputState::Disabled);
input->setEnabled(false);
```

### Error Handling

```cpp
auto* emailInput = new FluentTextInput("Email...");
emailInput->setValidationType(FluentTextInputValidation::Email);

connect(emailInput, &FluentTextInput::validationStateChanged, [=](bool isValid) {
    if (!isValid) {
        emailInput->setInputState(FluentTextInputState::Error);
        emailInput->setErrorText("Please enter a valid email address");
    } else {
        emailInput->setInputState(FluentTextInputState::Success);
        emailInput->setErrorText("");  // Clear error
    }
});
```

## Features and Properties

### Clear Button

```cpp
auto* input = new FluentTextInput("Search...");
input->setClearButtonVisible(true);  // Show clear button when text is present

connect(input, &FluentTextInput::textChanged, [=](const QString& text) {
    // Clear button automatically shows/hides based on text content
});
```

### Character Limits

```cpp
auto* input = new FluentTextInput("Enter description...");
input->setMaxLength(280);  // Twitter-style character limit

// Show character count in helper text
connect(input, &FluentTextInput::textChanged, [=](const QString& text) {
    int remaining = 280 - text.length();
    input->setHelperText(QString("%1 characters remaining").arg(remaining));
    
    if (remaining < 20) {
        input->setInputState(FluentTextInputState::Warning);
    } else {
        input->setInputState(FluentTextInputState::Normal);
    }
});
```

### Read-Only Mode

```cpp
auto* displayInput = new FluentTextInput();
displayInput->setText("Read-only value");
displayInput->setReadOnly(true);
displayInput->setLabelText("Current Status");
```

## Text Operations

### Selection and Clipboard

```cpp
auto* input = new FluentTextInput("Sample text content");

// Select all text
input->selectAll();

// Select specific range
input->selectText(0, 6);  // Select "Sample"

// Get selected text
QString selected = input->selectedText();

// Clipboard operations
input->copy();
input->cut();
input->paste();

// Undo/Redo
input->undo();
input->redo();

// Clear content
input->clear();
```

### Focus Management

```cpp
auto* input = new FluentTextInput("Enter text...");

// Set focus
input->setFocus();

// Clear focus
input->clearFocus();

// Handle focus events
connect(input, &FluentTextInput::focusChanged, [](bool hasFocus) {
    if (hasFocus) {
        qDebug() << "Input gained focus";
    } else {
        qDebug() << "Input lost focus";
    }
});
```

## Animations

FluentTextInput includes smooth animations for state transitions:

```cpp
#include "FluentQt/Animation/FluentAnimator.h"

auto* input = new FluentTextInput("Animated input...");
auto* animator = new FluentAnimator(this);

// Custom animations on validation
connect(input, &FluentTextInput::validationStateChanged, [=](bool isValid) {
    if (!isValid) {
        // Shake animation for errors
        animator->shakeEffect(input);
    } else {
        // Pulse animation for success
        animator->pulseEffect(input);
    }
});

// Disable built-in animations if needed
input->setAnimated(false);
```

## Accessibility

FluentTextInput provides comprehensive accessibility support:

```cpp
auto* input = new FluentTextInput("Enter email...");
input->setLabelText("Email Address");

// Accessible properties (automatically set)
input->setAccessibleName("Email Address");
input->setAccessibleDescription("Enter your email address for account registration");

// Screen reader support
// - Label association is automatic
// - Error messages are announced
// - State changes are communicated
// - Keyboard navigation works out of the box

// Custom accessibility
input->setAccessibleDescription("Email format: user@domain.com");
```

## Form Integration

### Complete Form Example

```cpp
auto* form = new QWidget;
auto* layout = new QVBoxLayout(form);

// Name input
auto* nameInput = new FluentTextInput();
nameInput->setLabelText("Full Name *");
nameInput->setPlaceholderText("Enter your full name");
nameInput->setValidationType(FluentTextInputValidation::Required);

// Email input
auto* emailInput = new FluentTextInput();
emailInput->setLabelText("Email Address *");
emailInput->setPlaceholderText("Enter your email");
emailInput->setInputType(FluentTextInputType::Email);
emailInput->setValidationType(FluentTextInputValidation::Email);

// Phone input (optional)
auto* phoneInput = new FluentTextInput();
phoneInput->setLabelText("Phone Number");
phoneInput->setPlaceholderText("Enter phone number");
phoneInput->setInputType(FluentTextInputType::Phone);
phoneInput->setHelperText("Optional - for account recovery");

// Message input
auto* messageInput = new FluentTextInput();
messageInput->setLabelText("Message");
messageInput->setPlaceholderText("Enter your message...");
messageInput->setInputType(FluentTextInputType::Multiline);
messageInput->setMaxLength(500);

layout->addWidget(nameInput);
layout->addWidget(emailInput);
layout->addWidget(phoneInput);
layout->addWidget(messageInput);

// Form validation
auto validateForm = [=]() {
    bool isValid = nameInput->isValid() && emailInput->isValid();
    submitButton->setEnabled(isValid);
    return isValid;
};

connect(nameInput, &FluentTextInput::textChanged, validateForm);
connect(emailInput, &FluentTextInput::textChanged, validateForm);
```

### Real-time Validation

```cpp
auto* usernameInput = new FluentTextInput();
usernameInput->setLabelText("Username");
usernameInput->setPlaceholderText("Choose a username");

// Real-time validation with debouncing
QTimer* validationTimer = new QTimer(this);
validationTimer->setSingleShot(true);
validationTimer->setInterval(500);  // 500ms delay

connect(usernameInput, &FluentTextInput::textChanged, [=]() {
    validationTimer->start();
});

connect(validationTimer, &QTimer::timeout, [=]() {
    QString username = usernameInput->text();
    
    if (username.length() < 3) {
        usernameInput->setInputState(FluentTextInputState::Error);
        usernameInput->setErrorText("Username must be at least 3 characters");
    } else if (isUsernameTaken(username)) {
        usernameInput->setInputState(FluentTextInputState::Error);
        usernameInput->setErrorText("Username is already taken");
    } else {
        usernameInput->setInputState(FluentTextInputState::Success);
        usernameInput->setHelperText("Username is available");
    }
});
```

## Best Practices

### Validation Guidelines
- Validate on input change, not just on submit
- Provide clear, actionable error messages
- Use appropriate validation types for input types
- Show success states for important validations

### Label and Helper Text
- Always provide labels for accessibility
- Use helper text to clarify requirements
- Keep error messages concise and helpful
- Use placeholder text for examples, not instructions

### Input Types
- Choose appropriate input types for better UX
- Use email type for email inputs (mobile keyboards)
- Use number type for numeric inputs
- Use multiline for longer text content

### Performance
- Debounce real-time validation to avoid excessive API calls
- Use appropriate validation timing (on change vs on blur)
- Clear validation state when appropriate

## Signals Reference

```cpp
// Text changes
void textChanged(const QString& text);
void textEdited(const QString& text);
void returnPressed();
void editingFinished();

// Validation and state
void validationStateChanged(bool isValid);
void inputStateChanged(FluentTextInputState state);

// Focus events
void focusChanged(bool hasFocus);
```

## See Also

- [FluentButton](FluentButton.md) - For form submission buttons
- [FluentCheckBox](FluentCheckBox.md) - For boolean form inputs
- [FluentComboBox](FluentComboBox.md) - For selection inputs
- [FluentPanel](FluentPanel.md) - For form containers
- [Form Validation Guide](../guides/form-validation.md) - Best practices for form validation
