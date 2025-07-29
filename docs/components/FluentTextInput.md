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

## Standalone Examples Collection

### Example 1: Complete Registration Form with Real-time Validation

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegularExpression>
#include <QTimer>
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"

class RegistrationForm : public QWidget {
    Q_OBJECT
public:
    RegistrationForm(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupValidation();
        connectSignals();
    }

private slots:
    void validateUsername() {
        QString username = m_usernameInput->text();

        if (username.length() < 3) {
            m_usernameInput->setInputState(FluentTextInputState::Error);
            m_usernameInput->setHelperText("Username must be at least 3 characters");
            return;
        }

        if (!QRegularExpression("^[a-zA-Z0-9_]+$").match(username).hasMatch()) {
            m_usernameInput->setInputState(FluentTextInputState::Error);
            m_usernameInput->setHelperText("Username can only contain letters, numbers, and underscores");
            return;
        }

        // Simulate checking username availability
        m_usernameInput->setInputState(FluentTextInputState::Warning);
        m_usernameInput->setHelperText("Checking availability...");

        QTimer::singleShot(1000, [this]() {
            // Simulate API response
            bool isAvailable = (QRandomGenerator::global()->bounded(2) == 0);
            if (isAvailable) {
                m_usernameInput->setInputState(FluentTextInputState::Success);
                m_usernameInput->setHelperText("Username is available!");
            } else {
                m_usernameInput->setInputState(FluentTextInputState::Error);
                m_usernameInput->setHelperText("Username is already taken");
            }
            validateForm();
        });
    }

    void validateEmail() {
        QString email = m_emailInput->text();
        QRegularExpression emailRegex("^[\\w\\.-]+@[\\w\\.-]+\\.[a-zA-Z]{2,}$");

        if (email.isEmpty()) {
            m_emailInput->setInputState(FluentTextInputState::Normal);
            m_emailInput->setHelperText("Enter your email address");
        } else if (emailRegex.match(email).hasMatch()) {
            m_emailInput->setInputState(FluentTextInputState::Success);
            m_emailInput->setHelperText("Valid email address");
        } else {
            m_emailInput->setInputState(FluentTextInputState::Error);
            m_emailInput->setHelperText("Please enter a valid email address");
        }
        validateForm();
    }

    void validatePassword() {
        QString password = m_passwordInput->text();

        if (password.length() < 8) {
            m_passwordInput->setInputState(FluentTextInputState::Error);
            m_passwordInput->setHelperText("Password must be at least 8 characters");
        } else if (!QRegularExpression("(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)").match(password).hasMatch()) {
            m_passwordInput->setInputState(FluentTextInputState::Warning);
            m_passwordInput->setHelperText("Password should contain uppercase, lowercase, and numbers");
        } else {
            m_passwordInput->setInputState(FluentTextInputState::Success);
            m_passwordInput->setHelperText("Strong password");
        }
        validateConfirmPassword();
        validateForm();
    }

    void validateConfirmPassword() {
        QString password = m_passwordInput->text();
        QString confirm = m_confirmPasswordInput->text();

        if (confirm.isEmpty()) {
            m_confirmPasswordInput->setInputState(FluentTextInputState::Normal);
            m_confirmPasswordInput->setHelperText("Confirm your password");
        } else if (password == confirm) {
            m_confirmPasswordInput->setInputState(FluentTextInputState::Success);
            m_confirmPasswordInput->setHelperText("Passwords match");
        } else {
            m_confirmPasswordInput->setInputState(FluentTextInputState::Error);
            m_confirmPasswordInput->setHelperText("Passwords do not match");
        }
        validateForm();
    }

    void validateForm() {
        bool isValid =
            m_usernameInput->inputState() == FluentTextInputState::Success &&
            m_emailInput->inputState() == FluentTextInputState::Success &&
            m_passwordInput->inputState() == FluentTextInputState::Success &&
            m_confirmPasswordInput->inputState() == FluentTextInputState::Success;

        m_submitButton->setEnabled(isValid);
        if (isValid) {
            m_submitButton->setButtonStyle(FluentButtonStyle::Primary);
        } else {
            m_submitButton->setButtonStyle(FluentButtonStyle::Subtle);
        }
    }

    void submitForm() {
        m_submitButton->setLoading(true);
        m_submitButton->setText("Creating Account...");

        // Simulate account creation
        QTimer::singleShot(2000, [this]() {
            m_submitButton->setLoading(false);
            m_submitButton->setText("✓ Account Created!");
            m_submitButton->setButtonStyle(FluentButtonStyle::Accent);

            // Reset form after success
            QTimer::singleShot(2000, [this]() {
                resetForm();
            });
        });
    }

    void resetForm() {
        m_usernameInput->clear();
        m_emailInput->clear();
        m_passwordInput->clear();
        m_confirmPasswordInput->clear();

        m_submitButton->setText("Create Account");
        m_submitButton->setButtonStyle(FluentButtonStyle::Subtle);
        m_submitButton->setEnabled(false);
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // Form card
        auto* card = new FluentCard;
        auto* cardLayout = new QVBoxLayout(card);

        // Title
        auto* title = new QLabel("Create Account");
        title->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 20px;");

        // Username input
        m_usernameInput = new FluentTextInput();
        m_usernameInput->setLabelText("Username");
        m_usernameInput->setPlaceholderText("Choose a unique username");
        m_usernameInput->setHelperText("3+ characters, letters, numbers, and underscores only");

        // Email input
        m_emailInput = new FluentTextInput();
        m_emailInput->setLabelText("Email Address");
        m_emailInput->setPlaceholderText("Enter your email");
        m_emailInput->setInputType(FluentTextInputType::Email);
        m_emailInput->setHelperText("We'll use this to verify your account");

        // Password input
        m_passwordInput = new FluentTextInput();
        m_passwordInput->setLabelText("Password");
        m_passwordInput->setPlaceholderText("Create a strong password");
        m_passwordInput->setInputType(FluentTextInputType::Password);
        m_passwordInput->setHelperText("8+ characters with uppercase, lowercase, and numbers");

        // Confirm password input
        m_confirmPasswordInput = new FluentTextInput();
        m_confirmPasswordInput->setLabelText("Confirm Password");
        m_confirmPasswordInput->setPlaceholderText("Re-enter your password");
        m_confirmPasswordInput->setInputType(FluentTextInputType::Password);
        m_confirmPasswordInput->setHelperText("Must match the password above");

        // Submit button
        m_submitButton = new FluentButton("Create Account");
        m_submitButton->setButtonStyle(FluentButtonStyle::Subtle);
        m_submitButton->setEnabled(false);

        cardLayout->addWidget(title);
        cardLayout->addWidget(m_usernameInput);
        cardLayout->addWidget(m_emailInput);
        cardLayout->addWidget(m_passwordInput);
        cardLayout->addWidget(m_confirmPasswordInput);
        cardLayout->addWidget(m_submitButton);

        layout->addWidget(card);
    }

    void setupValidation() {
        // Set up debounced validation timers
        m_usernameTimer = new QTimer(this);
        m_usernameTimer->setSingleShot(true);
        m_usernameTimer->setInterval(500); // 500ms debounce
        connect(m_usernameTimer, &QTimer::timeout, this, &RegistrationForm::validateUsername);

        m_emailTimer = new QTimer(this);
        m_emailTimer->setSingleShot(true);
        m_emailTimer->setInterval(300);
        connect(m_emailTimer, &QTimer::timeout, this, &RegistrationForm::validateEmail);
    }

    void connectSignals() {
        // Debounced validation
        connect(m_usernameInput, &FluentTextInput::textChanged, [this]() {
            m_usernameTimer->start();
        });

        connect(m_emailInput, &FluentTextInput::textChanged, [this]() {
            m_emailTimer->start();
        });

        // Immediate validation for passwords
        connect(m_passwordInput, &FluentTextInput::textChanged,
                this, &RegistrationForm::validatePassword);
        connect(m_confirmPasswordInput, &FluentTextInput::textChanged,
                this, &RegistrationForm::validateConfirmPassword);

        // Form submission
        connect(m_submitButton, &FluentButton::clicked,
                this, &RegistrationForm::submitForm);
    }

    FluentTextInput* m_usernameInput;
    FluentTextInput* m_emailInput;
    FluentTextInput* m_passwordInput;
    FluentTextInput* m_confirmPasswordInput;
    FluentButton* m_submitButton;
    QTimer* m_usernameTimer;
    QTimer* m_emailTimer;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    RegistrationForm form;
    form.show();

    return app.exec();
}

#include "registration_form.moc"
```

### Example 2: Search Interface with Auto-complete

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTimer>
#include <QStringList>
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentCard.h"

class SearchInterface : public QWidget {
    Q_OBJECT
public:
    SearchInterface(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupSearchData();
        connectSignals();
    }

private slots:
    void performSearch() {
        QString query = m_searchInput->text().toLower();

        if (query.isEmpty()) {
            m_resultsWidget->clear();
            m_searchInput->setHelperText("Enter search terms");
            return;
        }

        if (query.length() < 2) {
            m_searchInput->setHelperText("Enter at least 2 characters");
            return;
        }

        // Show loading state
        m_searchInput->setInputState(FluentTextInputState::Warning);
        m_searchInput->setHelperText("Searching...");

        // Simulate search delay
        QTimer::singleShot(300, [this, query]() {
            QStringList results;
            for (const QString& item : m_searchData) {
                if (item.toLower().contains(query)) {
                    results.append(item);
                }
            }

            displayResults(results, query);
        });
    }

    void displayResults(const QStringList& results, const QString& query) {
        m_resultsWidget->clear();

        if (results.isEmpty()) {
            m_searchInput->setInputState(FluentTextInputState::Warning);
            m_searchInput->setHelperText(QString("No results found for '%1'").arg(query));

            auto* noResultsItem = new QListWidgetItem("No results found");
            noResultsItem->setFlags(Qt::NoItemFlags);
            m_resultsWidget->addItem(noResultsItem);
        } else {
            m_searchInput->setInputState(FluentTextInputState::Success);
            m_searchInput->setHelperText(QString("%1 results found").arg(results.size()));

            for (const QString& result : results) {
                auto* item = new QListWidgetItem(result);
                m_resultsWidget->addItem(item);
            }
        }
    }

    void selectResult(QListWidgetItem* item) {
        if (item && item->flags() != Qt::NoItemFlags) {
            m_searchInput->setText(item->text());
            m_searchInput->setInputState(FluentTextInputState::Success);
            m_searchInput->setHelperText("Selected: " + item->text());
        }
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // Search input
        m_searchInput = new FluentTextInput();
        m_searchInput->setLabelText("Search");
        m_searchInput->setPlaceholderText("Type to search...");
        m_searchInput->setInputType(FluentTextInputType::Search);
        m_searchInput->setHelperText("Enter at least 2 characters to search");

        // Results list
        m_resultsWidget = new QListWidget;
        m_resultsWidget->setMaximumHeight(200);

        layout->addWidget(m_searchInput);
        layout->addWidget(m_resultsWidget);
    }

    void setupSearchData() {
        m_searchData = {
            "Apple iPhone 14", "Samsung Galaxy S23", "Google Pixel 7",
            "MacBook Pro", "Dell XPS 13", "HP Spectre x360",
            "iPad Air", "Surface Pro 9", "Lenovo ThinkPad",
            "AirPods Pro", "Sony WH-1000XM4", "Bose QuietComfort",
            "Apple Watch", "Garmin Forerunner", "Fitbit Versa"
        };
    }

    void connectSignals() {
        // Debounced search
        m_searchTimer = new QTimer(this);
        m_searchTimer->setSingleShot(true);
        m_searchTimer->setInterval(300);
        connect(m_searchTimer, &QTimer::timeout, this, &SearchInterface::performSearch);

        connect(m_searchInput, &FluentTextInput::textChanged, [this]() {
            m_searchTimer->start();
        });

        connect(m_resultsWidget, &QListWidget::itemClicked,
                this, &SearchInterface::selectResult);
    }

    FluentTextInput* m_searchInput;
    QListWidget* m_resultsWidget;
    QStringList m_searchData;
    QTimer* m_searchTimer;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    SearchInterface search;
    search.show();

    return app.exec();
}

#include "search_interface.moc"
```

### Example 3: Multi-language Input with Character Counter

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentCard.h"

class MultiLanguageInput : public QWidget {
    Q_OBJECT
public:
    MultiLanguageInput(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        connectSignals();
    }

private slots:
    void updateCharacterCount() {
        QString text = m_textInput->text();
        int length = text.length();
        int maxLength = m_maxCharacters;

        m_characterLabel->setText(QString("%1 / %2 characters").arg(length).arg(maxLength));

        if (length > maxLength) {
            m_textInput->setInputState(FluentTextInputState::Error);
            m_textInput->setHelperText("Text exceeds maximum length");
            m_characterLabel->setStyleSheet("color: red;");
        } else if (length > maxLength * 0.9) {
            m_textInput->setInputState(FluentTextInputState::Warning);
            m_textInput->setHelperText("Approaching character limit");
            m_characterLabel->setStyleSheet("color: orange;");
        } else {
            m_textInput->setInputState(FluentTextInputState::Normal);
            m_textInput->setHelperText("Enter your message in the selected language");
            m_characterLabel->setStyleSheet("color: gray;");
        }
    }

    void changeLanguage(const QString& language) {
        if (language == "English") {
            m_textInput->setPlaceholderText("Enter your message in English...");
            m_maxCharacters = 280;
        } else if (language == "中文") {
            m_textInput->setPlaceholderText("请输入您的中文消息...");
            m_maxCharacters = 140;
        } else if (language == "العربية") {
            m_textInput->setPlaceholderText("أدخل رسالتك باللغة العربية...");
            m_maxCharacters = 140;
        } else if (language == "Español") {
            m_textInput->setPlaceholderText("Ingresa tu mensaje en español...");
            m_maxCharacters = 280;
        }

        updateCharacterCount();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        auto* card = new FluentCard;
        auto* cardLayout = new QVBoxLayout(card);

        // Language selector
        auto* langLayout = new QHBoxLayout;
        auto* langLabel = new QLabel("Language:");
        m_languageCombo = new QComboBox;
        m_languageCombo->addItems({"English", "中文", "العربية", "Español"});

        langLayout->addWidget(langLabel);
        langLayout->addWidget(m_languageCombo);
        langLayout->addStretch();

        // Text input
        m_textInput = new FluentTextInput();
        m_textInput->setLabelText("Message");
        m_textInput->setInputType(FluentTextInputType::Multiline);
        m_textInput->setPlaceholderText("Enter your message in English...");
        m_textInput->setHelperText("Enter your message in the selected language");

        // Character counter
        m_characterLabel = new QLabel("0 / 280 characters");
        m_characterLabel->setAlignment(Qt::AlignRight);
        m_characterLabel->setStyleSheet("color: gray; font-size: 12px;");

        cardLayout->addLayout(langLayout);
        cardLayout->addWidget(m_textInput);
        cardLayout->addWidget(m_characterLabel);

        layout->addWidget(card);
    }

    void connectSignals() {
        connect(m_textInput, &FluentTextInput::textChanged,
                this, &MultiLanguageInput::updateCharacterCount);
        connect(m_languageCombo, &QComboBox::currentTextChanged,
                this, &MultiLanguageInput::changeLanguage);
    }

    FluentTextInput* m_textInput;
    QComboBox* m_languageCombo;
    QLabel* m_characterLabel;
    int m_maxCharacters = 280;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MultiLanguageInput input;
    input.show();

    return app.exec();
}

#include "multilanguage_input.moc"
```

## See Also

- [FluentButton](FluentButton.md) - For form submission buttons
- [FluentCheckBox](FluentCheckBox.md) - For boolean form inputs
- [FluentComboBox](FluentComboBox.md) - For selection inputs
- [FluentPanel](FluentPanel.md) - For form containers
- [Form Validation Guide](../guides/form-validation.md) - Best practices for form validation
