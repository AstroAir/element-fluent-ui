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

## Standalone Examples Collection

### Example 1: Complete Login Form with Validation

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentTextInput.h"

class LoginForm : public QWidget {
    Q_OBJECT
public:
    LoginForm(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        connectSignals();
    }

private slots:
    void validateAndSubmit() {
        bool isValid = !m_emailInput->text().isEmpty() &&
                      !m_passwordInput->text().isEmpty();

        if (isValid) {
            m_submitButton->setLoading(true);
            m_submitButton->setText("Signing In...");

            // Simulate authentication
            QTimer::singleShot(2000, [this]() {
                m_submitButton->setLoading(false);
                m_submitButton->setText("Sign In");
                // Show success or error
            });
        }
    }

    void validateForm() {
        bool isValid = !m_emailInput->text().isEmpty() &&
                      !m_passwordInput->text().isEmpty();
        m_submitButton->setEnabled(isValid);

        if (isValid) {
            m_submitButton->setButtonStyle(FluentButtonStyle::Primary);
        } else {
            m_submitButton->setButtonStyle(FluentButtonStyle::Subtle);
        }
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // Title
        auto* title = new QLabel("Sign In");
        title->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 20px;");

        // Email input
        m_emailInput = new FluentTextInput("Enter your email");
        m_emailInput->setInputType(FluentTextInputType::Email);

        // Password input
        m_passwordInput = new FluentTextInput("Enter your password");
        m_passwordInput->setInputType(FluentTextInputType::Password);

        // Button layout
        auto* buttonLayout = new QHBoxLayout;

        // Cancel button
        m_cancelButton = new FluentButton("Cancel");
        m_cancelButton->setButtonStyle(FluentButtonStyle::Subtle);

        // Submit button
        m_submitButton = new FluentButton("Sign In");
        m_submitButton->setButtonStyle(FluentButtonStyle::Subtle);
        m_submitButton->setEnabled(false);

        buttonLayout->addWidget(m_cancelButton);
        buttonLayout->addStretch();
        buttonLayout->addWidget(m_submitButton);

        layout->addWidget(title);
        layout->addWidget(m_emailInput);
        layout->addWidget(m_passwordInput);
        layout->addLayout(buttonLayout);
    }

    void connectSignals() {
        connect(m_emailInput, &FluentTextInput::textChanged,
                this, &LoginForm::validateForm);
        connect(m_passwordInput, &FluentTextInput::textChanged,
                this, &LoginForm::validateForm);
        connect(m_submitButton, &FluentButton::clicked,
                this, &LoginForm::validateAndSubmit);
    }

    FluentTextInput* m_emailInput;
    FluentTextInput* m_passwordInput;
    FluentButton* m_submitButton;
    FluentButton* m_cancelButton;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    LoginForm form;
    form.show();

    return app.exec();
}

#include "login_example.moc"
```

### Example 2: Interactive Button States Showcase

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include "FluentQt/Components/FluentButton.h"

class ButtonStatesDemo : public QWidget {
    Q_OBJECT
public:
    ButtonStatesDemo(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupDemoControls();
    }

private slots:
    void toggleButtonState() {
        static int state = 0;
        state = (state + 1) % 4;

        switch (state) {
        case 0: // Normal
            m_demoButton->setEnabled(true);
            m_demoButton->setLoading(false);
            m_demoButton->setText("Normal State");
            m_stateLabel->setText("State: Normal");
            break;
        case 1: // Loading
            m_demoButton->setLoading(true);
            m_demoButton->setText("Loading...");
            m_stateLabel->setText("State: Loading");
            break;
        case 2: // Disabled
            m_demoButton->setLoading(false);
            m_demoButton->setEnabled(false);
            m_demoButton->setText("Disabled State");
            m_stateLabel->setText("State: Disabled");
            break;
        case 3: // Success feedback
            m_demoButton->setEnabled(true);
            m_demoButton->setText("✓ Success!");
            m_stateLabel->setText("State: Success");

            // Reset after 2 seconds
            QTimer::singleShot(2000, [this]() {
                m_demoButton->setText("Click to Cycle");
                m_stateLabel->setText("State: Normal");
            });
            break;
        }
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // Demo button
        m_demoButton = new FluentButton("Click to Cycle States");
        m_demoButton->setButtonStyle(FluentButtonStyle::Primary);
        m_demoButton->setButtonSize(FluentButtonSize::Large);

        // State label
        m_stateLabel = new QLabel("State: Normal");
        m_stateLabel->setAlignment(Qt::AlignCenter);

        layout->addWidget(m_demoButton);
        layout->addWidget(m_stateLabel);

        connect(m_demoButton, &FluentButton::clicked,
                this, &ButtonStatesDemo::toggleButtonState);
    }

    void setupDemoControls() {
        // Add style switching controls
        auto* styleGroup = new QGroupBox("Button Styles");
        auto* styleLayout = new QHBoxLayout(styleGroup);

        auto styles = {
            std::make_pair("Default", FluentButtonStyle::Default),
            std::make_pair("Primary", FluentButtonStyle::Primary),
            std::make_pair("Accent", FluentButtonStyle::Accent),
            std::make_pair("Subtle", FluentButtonStyle::Subtle),
            std::make_pair("Outline", FluentButtonStyle::Outline)
        };

        for (const auto& [name, style] : styles) {
            auto* button = new FluentButton(name);
            button->setButtonSize(FluentButtonSize::Small);
            connect(button, &FluentButton::clicked, [this, style]() {
                m_demoButton->setButtonStyle(style);
            });
            styleLayout->addWidget(button);
        }

        layout()->addWidget(styleGroup);
    }

    FluentButton* m_demoButton;
    QLabel* m_stateLabel;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ButtonStatesDemo demo;
    demo.show();

    return app.exec();
}

#include "button_states_demo.moc"
```

### Example 3: Responsive Button Toolbar

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QResizeEvent>
#include "FluentQt/Components/FluentButton.h"

class ResponsiveToolbar : public QMainWindow {
    Q_OBJECT
public:
    ResponsiveToolbar(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupResponsiveBehavior();
    }

protected:
    void resizeEvent(QResizeEvent* event) override {
        QMainWindow::resizeEvent(event);
        updateButtonLayout();
    }

private slots:
    void updateButtonLayout() {
        int width = this->width();

        // Switch between full toolbar and compact mode
        if (width < 600) {
            // Compact mode: smaller buttons, icons only
            for (auto* button : m_toolbarButtons) {
                button->setButtonSize(FluentButtonSize::Small);
                if (!button->icon().isNull()) {
                    button->setText(""); // Hide text, show only icon
                }
            }
            m_moreButton->setVisible(true);
        } else {
            // Full mode: normal buttons with text and icons
            for (auto* button : m_toolbarButtons) {
                button->setButtonSize(FluentButtonSize::Medium);
                // Restore original text
                if (button == m_newButton) button->setText("New");
                else if (button == m_openButton) button->setText("Open");
                else if (button == m_saveButton) button->setText("Save");
                else if (button == m_printButton) button->setText("Print");
            }
            m_moreButton->setVisible(false);
        }
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        // Create toolbar
        auto* toolbar = new QToolBar;
        addToolBar(toolbar);

        // Create buttons with icons and text
        m_newButton = new FluentButton(QIcon(":/icons/new"), "New");
        m_newButton->setShortcut(QKeySequence::New);
        m_newButton->setToolTip("Create new document (Ctrl+N)");

        m_openButton = new FluentButton(QIcon(":/icons/open"), "Open");
        m_openButton->setShortcut(QKeySequence::Open);
        m_openButton->setToolTip("Open document (Ctrl+O)");

        m_saveButton = new FluentButton(QIcon(":/icons/save"), "Save");
        m_saveButton->setShortcut(QKeySequence::Save);
        m_saveButton->setToolTip("Save document (Ctrl+S)");

        m_printButton = new FluentButton(QIcon(":/icons/print"), "Print");
        m_printButton->setShortcut(QKeySequence::Print);
        m_printButton->setToolTip("Print document (Ctrl+P)");

        // More button for overflow menu
        m_moreButton = new FluentButton(QIcon(":/icons/more"), "");
        m_moreButton->setButtonStyle(FluentButtonStyle::Subtle);
        m_moreButton->setVisible(false);

        // Add buttons to toolbar
        m_toolbarButtons = {m_newButton, m_openButton, m_saveButton, m_printButton};
        for (auto* button : m_toolbarButtons) {
            toolbar->addWidget(button);
        }
        toolbar->addWidget(m_moreButton);

        // Content area
        auto* layout = new QVBoxLayout(centralWidget);
        auto* label = new QLabel("Resize the window to see responsive toolbar behavior");
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
    }

    void setupResponsiveBehavior() {
        // Connect button actions
        connect(m_newButton, &FluentButton::clicked, []() {
            qDebug() << "New document";
        });
        connect(m_openButton, &FluentButton::clicked, []() {
            qDebug() << "Open document";
        });
        connect(m_saveButton, &FluentButton::clicked, []() {
            qDebug() << "Save document";
        });
        connect(m_printButton, &FluentButton::clicked, []() {
            qDebug() << "Print document";
        });
    }

    QList<FluentButton*> m_toolbarButtons;
    FluentButton* m_newButton;
    FluentButton* m_openButton;
    FluentButton* m_saveButton;
    FluentButton* m_printButton;
    FluentButton* m_moreButton;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ResponsiveToolbar window;
    window.resize(800, 600);
    window.show();

    return app.exec();
}

#include "responsive_toolbar.moc"
```

### Example 4: Accessibility-First Button Implementation

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Styling/FluentTheme.h"

class AccessibleButtonDemo : public QWidget {
    Q_OBJECT
public:
    AccessibleButtonDemo(QWidget* parent = nullptr) : QWidget(parent) {
        setupAccessibleUI();
        setupKeyboardNavigation();
        setupScreenReaderSupport();
        setupHighContrastSupport();
    }

protected:
    void keyPressEvent(QKeyEvent* event) override {
        // Custom keyboard navigation
        if (event->key() == Qt::Key_Tab) {
            focusNextButton();
            event->accept();
        } else if (event->key() == Qt::Key_Backtab) {
            focusPreviousButton();
            event->accept();
        } else {
            QWidget::keyPressEvent(event);
        }
    }

private slots:
    void focusNextButton() {
        m_currentFocus = (m_currentFocus + 1) % m_buttons.size();
        m_buttons[m_currentFocus]->setFocus();
        announceButtonFocus();
    }

    void focusPreviousButton() {
        m_currentFocus = (m_currentFocus - 1 + m_buttons.size()) % m_buttons.size();
        m_buttons[m_currentFocus]->setFocus();
        announceButtonFocus();
    }

    void announceButtonFocus() {
        auto* button = m_buttons[m_currentFocus];
        QString announcement = QString("Focused on %1 button. %2")
            .arg(button->text())
            .arg(button->accessibleDescription());

        // This would integrate with screen reader APIs
        qDebug() << "Screen reader announcement:" << announcement;
    }

    void toggleHighContrast() {
        auto& theme = FluentTheme::instance();
        bool isHighContrast = theme.isHighContrastMode();
        theme.setHighContrastMode(!isHighContrast);

        // Update button styles for high contrast
        updateButtonContrast(!isHighContrast);
    }

private:
    void setupAccessibleUI() {
        auto* layout = new QVBoxLayout(this);

        // Title with proper heading role
        auto* title = new QLabel("Accessible Button Demo");
        title->setAccessibleName("Main heading");
        title->setStyleSheet("font-size: 18px; font-weight: bold;");

        // Primary action button
        auto* primaryButton = new FluentButton("Save Document");
        primaryButton->setButtonStyle(FluentButtonStyle::Primary);
        primaryButton->setAccessibleName("Save Document");
        primaryButton->setAccessibleDescription("Saves the current document to disk. Keyboard shortcut: Ctrl+S");
        primaryButton->setShortcut(QKeySequence::Save);

        // Secondary action button
        auto* secondaryButton = new FluentButton("Export PDF");
        secondaryButton->setButtonStyle(FluentButtonStyle::Accent);
        secondaryButton->setAccessibleName("Export PDF");
        secondaryButton->setAccessibleDescription("Exports the document as a PDF file");

        // Destructive action button
        auto* deleteButton = new FluentButton("Delete");
        deleteButton->setButtonStyle(FluentButtonStyle::Outline);
        deleteButton->setAccessibleName("Delete Document");
        deleteButton->setAccessibleDescription("Permanently deletes the current document. This action cannot be undone");

        // Toggle button for accessibility features
        auto* contrastButton = new FluentButton("Toggle High Contrast");
        contrastButton->setButtonStyle(FluentButtonStyle::Subtle);
        contrastButton->setAccessibleName("Toggle High Contrast Mode");
        contrastButton->setAccessibleDescription("Switches between normal and high contrast display modes");

        m_buttons = {primaryButton, secondaryButton, deleteButton, contrastButton};

        layout->addWidget(title);
        for (auto* button : m_buttons) {
            layout->addWidget(button);
        }

        // Connect signals
        connect(contrastButton, &FluentButton::clicked,
                this, &AccessibleButtonDemo::toggleHighContrast);
    }

    void setupKeyboardNavigation() {
        // Ensure proper tab order
        for (int i = 0; i < m_buttons.size() - 1; ++i) {
            setTabOrder(m_buttons[i], m_buttons[i + 1]);
        }

        // Set initial focus
        m_currentFocus = 0;
        m_buttons[0]->setFocus();
    }

    void setupScreenReaderSupport() {
        // Set window role for screen readers
        setAccessibleName("Button Accessibility Demo");
        setAccessibleDescription("Demonstration of accessible button implementations with keyboard navigation and screen reader support");

        // Ensure all buttons have proper ARIA roles (handled automatically by FluentButton)
        for (auto* button : m_buttons) {
            // FluentButton automatically sets button role and handles state announcements
            connect(button, &FluentButton::clicked, [button]() {
                qDebug() << "Screen reader: Button activated -" << button->accessibleName();
            });
        }
    }

    void setupHighContrastSupport() {
        auto& theme = FluentTheme::instance();
        connect(&theme, &FluentTheme::themeChanged, [this]() {
            updateButtonContrast(theme.isHighContrastMode());
        });
    }

    void updateButtonContrast(bool highContrast) {
        for (auto* button : m_buttons) {
            if (highContrast) {
                // Ensure sufficient contrast ratios
                button->setStyleSheet(
                    "FluentButton { "
                    "    border: 2px solid; "
                    "    font-weight: bold; "
                    "}"
                );
            } else {
                button->setStyleSheet("");
            }
        }
    }

    QList<FluentButton*> m_buttons;
    int m_currentFocus = 0;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    AccessibleButtonDemo demo;
    demo.show();

    return app.exec();
}

#include "accessible_button_demo.moc"
```

### Example 5: Dynamic Theming and Custom Styling

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Styling/FluentTheme.h"

class ThemingDemo : public QWidget {
    Q_OBJECT
public:
    ThemingDemo(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupThemeControls();
        connectThemeSignals();
    }

private slots:
    void switchToLightTheme() {
        auto& theme = FluentTheme::instance();
        theme.setMode(FluentThemeMode::Light);
        updateThemeDisplay();
    }

    void switchToDarkTheme() {
        auto& theme = FluentTheme::instance();
        theme.setMode(FluentThemeMode::Dark);
        updateThemeDisplay();
    }

    void changeAccentColor(FluentAccentColor color) {
        auto& theme = FluentTheme::instance();
        theme.setAccentColor(color);
        updateAccentButtons();
    }

    void updateCornerRadius(int radius) {
        QString style = QString(
            "FluentButton { border-radius: %1px; }"
        ).arg(radius);

        for (auto* button : m_demoButtons) {
            button->setStyleSheet(style);
        }
        m_radiusLabel->setText(QString("Corner Radius: %1px").arg(radius));
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // Demo buttons section
        auto* demoGroup = new QGroupBox("Button Styles Preview");
        auto* demoLayout = new QHBoxLayout(demoGroup);

        // Create buttons with different styles
        auto* primaryBtn = new FluentButton("Primary");
        primaryBtn->setButtonStyle(FluentButtonStyle::Primary);

        auto* accentBtn = new FluentButton("Accent");
        accentBtn->setButtonStyle(FluentButtonStyle::Accent);

        auto* defaultBtn = new FluentButton("Default");
        defaultBtn->setButtonStyle(FluentButtonStyle::Default);

        auto* subtleBtn = new FluentButton("Subtle");
        subtleBtn->setButtonStyle(FluentButtonStyle::Subtle);

        auto* outlineBtn = new FluentButton("Outline");
        outlineBtn->setButtonStyle(FluentButtonStyle::Outline);

        m_demoButtons = {primaryBtn, accentBtn, defaultBtn, subtleBtn, outlineBtn};

        for (auto* button : m_demoButtons) {
            demoLayout->addWidget(button);
        }

        layout->addWidget(demoGroup);
    }

    void setupThemeControls() {
        // Theme mode controls
        auto* themeGroup = new QGroupBox("Theme Mode");
        auto* themeLayout = new QHBoxLayout(themeGroup);

        auto* lightBtn = new FluentButton("Light Theme");
        lightBtn->setButtonStyle(FluentButtonStyle::Subtle);
        connect(lightBtn, &FluentButton::clicked, this, &ThemingDemo::switchToLightTheme);

        auto* darkBtn = new FluentButton("Dark Theme");
        darkBtn->setButtonStyle(FluentButtonStyle::Subtle);
        connect(darkBtn, &FluentButton::clicked, this, &ThemingDemo::switchToDarkTheme);

        themeLayout->addWidget(lightBtn);
        themeLayout->addWidget(darkBtn);

        // Accent color controls
        auto* accentGroup = new QGroupBox("Accent Colors");
        auto* accentLayout = new QHBoxLayout(accentGroup);

        auto accentColors = {
            std::make_pair("Blue", FluentAccentColor::Blue),
            std::make_pair("Purple", FluentAccentColor::Purple),
            std::make_pair("Green", FluentAccentColor::Green),
            std::make_pair("Orange", FluentAccentColor::Orange),
            std::make_pair("Red", FluentAccentColor::Red)
        };

        for (const auto& [name, color] : accentColors) {
            auto* btn = new FluentButton(name);
            btn->setButtonSize(FluentButtonSize::Small);
            connect(btn, &FluentButton::clicked, [this, color]() {
                changeAccentColor(color);
            });
            accentLayout->addWidget(btn);
        }

        // Corner radius control
        auto* radiusGroup = new QGroupBox("Corner Radius");
        auto* radiusLayout = new QVBoxLayout(radiusGroup);

        m_radiusLabel = new QLabel("Corner Radius: 8px");
        auto* radiusSlider = new QSlider(Qt::Horizontal);
        radiusSlider->setRange(0, 20);
        radiusSlider->setValue(8);

        connect(radiusSlider, &QSlider::valueChanged,
                this, &ThemingDemo::updateCornerRadius);

        radiusLayout->addWidget(m_radiusLabel);
        radiusLayout->addWidget(radiusSlider);

        layout()->addWidget(themeGroup);
        layout()->addWidget(accentGroup);
        layout()->addWidget(radiusGroup);
    }

    void connectThemeSignals() {
        auto& theme = FluentTheme::instance();
        connect(&theme, &FluentTheme::themeChanged,
                this, &ThemingDemo::updateThemeDisplay);
    }

    void updateThemeDisplay() {
        auto& theme = FluentTheme::instance();
        QString mode = (theme.mode() == FluentThemeMode::Light) ? "Light" : "Dark";
        setWindowTitle(QString("Theming Demo - %1 Mode").arg(mode));
    }

    void updateAccentButtons() {
        // Accent and primary buttons will automatically update with theme changes
        // This is handled by the FluentButton implementation
    }

    QList<FluentButton*> m_demoButtons;
    QLabel* m_radiusLabel;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ThemingDemo demo;
    demo.show();

    return app.exec();
}

#include "theming_demo.moc"
```

### Example 6: Real-World E-commerce Cart Interface

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QScrollArea>
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"

struct CartItem {
    QString name;
    QString description;
    double price;
    int quantity;
    QString imageUrl;
};

class ShoppingCartWidget : public QWidget {
    Q_OBJECT
public:
    ShoppingCartWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        populateCart();
        updateTotals();
    }

private slots:
    void removeItem(int index) {
        if (index >= 0 && index < m_cartItems.size()) {
            m_cartItems.removeAt(index);
            refreshCartDisplay();
            updateTotals();
        }
    }

    void updateQuantity(int index, int newQuantity) {
        if (index >= 0 && index < m_cartItems.size()) {
            m_cartItems[index].quantity = newQuantity;
            updateTotals();
        }
    }

    void proceedToCheckout() {
        if (m_cartItems.isEmpty()) {
            m_checkoutButton->setText("Cart is Empty");
            m_checkoutButton->setEnabled(false);
            return;
        }

        m_checkoutButton->setLoading(true);
        m_checkoutButton->setText("Processing...");

        // Simulate checkout process
        QTimer::singleShot(3000, [this]() {
            m_checkoutButton->setLoading(false);
            m_checkoutButton->setText("✓ Order Placed!");
            m_checkoutButton->setButtonStyle(FluentButtonStyle::Accent);

            // Reset after showing success
            QTimer::singleShot(2000, [this]() {
                m_cartItems.clear();
                refreshCartDisplay();
                updateTotals();
                m_checkoutButton->setText("Proceed to Checkout");
                m_checkoutButton->setButtonStyle(FluentButtonStyle::Primary);
                m_checkoutButton->setEnabled(false);
            });
        });
    }

    void continueShopping() {
        // This would navigate back to the product catalog
        qDebug() << "Continue shopping clicked";
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // Header
        auto* header = new QLabel("Shopping Cart");
        header->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 20px;");

        // Cart items scroll area
        m_scrollArea = new QScrollArea;
        m_cartContainer = new QWidget;
        m_cartLayout = new QVBoxLayout(m_cartContainer);
        m_scrollArea->setWidget(m_cartContainer);
        m_scrollArea->setWidgetResizable(true);
        m_scrollArea->setMinimumHeight(300);

        // Totals section
        auto* totalsCard = new FluentCard;
        auto* totalsLayout = new QVBoxLayout(totalsCard);

        m_subtotalLabel = new QLabel("Subtotal: $0.00");
        m_taxLabel = new QLabel("Tax: $0.00");
        m_totalLabel = new QLabel("Total: $0.00");
        m_totalLabel->setStyleSheet("font-weight: bold; font-size: 16px;");

        totalsLayout->addWidget(m_subtotalLabel);
        totalsLayout->addWidget(m_taxLabel);
        totalsLayout->addWidget(m_totalLabel);

        // Action buttons
        auto* buttonLayout = new QHBoxLayout;

        m_continueButton = new FluentButton("Continue Shopping");
        m_continueButton->setButtonStyle(FluentButtonStyle::Subtle);
        connect(m_continueButton, &FluentButton::clicked,
                this, &ShoppingCartWidget::continueShopping);

        m_checkoutButton = new FluentButton("Proceed to Checkout");
        m_checkoutButton->setButtonStyle(FluentButtonStyle::Primary);
        m_checkoutButton->setEnabled(false);
        connect(m_checkoutButton, &FluentButton::clicked,
                this, &ShoppingCartWidget::proceedToCheckout);

        buttonLayout->addWidget(m_continueButton);
        buttonLayout->addStretch();
        buttonLayout->addWidget(m_checkoutButton);

        layout->addWidget(header);
        layout->addWidget(m_scrollArea);
        layout->addWidget(totalsCard);
        layout->addLayout(buttonLayout);
    }

    void populateCart() {
        m_cartItems = {
            {"Wireless Headphones", "Premium noise-canceling headphones", 199.99, 1, ""},
            {"Smartphone Case", "Protective case with wireless charging", 29.99, 2, ""},
            {"USB-C Cable", "High-speed charging and data cable", 19.99, 1, ""}
        };
    }

    void refreshCartDisplay() {
        // Clear existing items
        QLayoutItem* item;
        while ((item = m_cartLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }

        // Add current items
        for (int i = 0; i < m_cartItems.size(); ++i) {
            auto* itemWidget = createCartItemWidget(m_cartItems[i], i);
            m_cartLayout->addWidget(itemWidget);
        }

        if (m_cartItems.isEmpty()) {
            auto* emptyLabel = new QLabel("Your cart is empty");
            emptyLabel->setAlignment(Qt::AlignCenter);
            emptyLabel->setStyleSheet("color: gray; font-style: italic; padding: 40px;");
            m_cartLayout->addWidget(emptyLabel);
        }
    }

    QWidget* createCartItemWidget(const CartItem& item, int index) {
        auto* card = new FluentCard;
        auto* layout = new QHBoxLayout(card);

        // Item info
        auto* infoLayout = new QVBoxLayout;
        auto* nameLabel = new QLabel(item.name);
        nameLabel->setStyleSheet("font-weight: bold;");
        auto* descLabel = new QLabel(item.description);
        descLabel->setStyleSheet("color: gray;");
        auto* priceLabel = new QLabel(QString("$%1").arg(item.price, 0, 'f', 2));

        infoLayout->addWidget(nameLabel);
        infoLayout->addWidget(descLabel);
        infoLayout->addWidget(priceLabel);

        // Quantity controls
        auto* quantityLayout = new QHBoxLayout;
        auto* quantityLabel = new QLabel("Qty:");
        auto* quantitySpinBox = new QSpinBox;
        quantitySpinBox->setRange(1, 99);
        quantitySpinBox->setValue(item.quantity);

        connect(quantitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                [this, index](int value) {
                    updateQuantity(index, value);
                });

        quantityLayout->addWidget(quantityLabel);
        quantityLayout->addWidget(quantitySpinBox);

        // Remove button
        auto* removeButton = new FluentButton("Remove");
        removeButton->setButtonStyle(FluentButtonStyle::Subtle);
        removeButton->setButtonSize(FluentButtonSize::Small);
        connect(removeButton, &FluentButton::clicked, [this, index]() {
            removeItem(index);
        });

        layout->addLayout(infoLayout);
        layout->addStretch();
        layout->addLayout(quantityLayout);
        layout->addWidget(removeButton);

        return card;
    }

    void updateTotals() {
        double subtotal = 0.0;
        for (const auto& item : m_cartItems) {
            subtotal += item.price * item.quantity;
        }

        double tax = subtotal * 0.08; // 8% tax
        double total = subtotal + tax;

        m_subtotalLabel->setText(QString("Subtotal: $%1").arg(subtotal, 0, 'f', 2));
        m_taxLabel->setText(QString("Tax: $%1").arg(tax, 0, 'f', 2));
        m_totalLabel->setText(QString("Total: $%1").arg(total, 0, 'f', 2));

        m_checkoutButton->setEnabled(!m_cartItems.isEmpty());
    }

    QList<CartItem> m_cartItems;
    QScrollArea* m_scrollArea;
    QWidget* m_cartContainer;
    QVBoxLayout* m_cartLayout;
    QLabel* m_subtotalLabel;
    QLabel* m_taxLabel;
    QLabel* m_totalLabel;
    FluentButton* m_continueButton;
    FluentButton* m_checkoutButton;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ShoppingCartWidget cart;
    cart.resize(600, 500);
    cart.show();

    return app.exec();
}

#include "shopping_cart.moc"
```

## Performance Considerations

### Memory Management
- FluentButton uses smart pointers internally for automatic memory management
- Animation objects are automatically cleaned up when buttons are destroyed
- Icon caching reduces memory usage for repeated icons

### Rendering Optimization
- Buttons use cached style sheets to avoid repeated style calculations
- Animation states are optimized to minimize repaints
- Icon rendering is optimized for different DPI settings

### Best Practices for Performance
```cpp
// Good: Reuse button instances
auto* button = new FluentButton("Action");
button->setText("New Action"); // Update existing button

// Avoid: Creating new buttons frequently
// for (int i = 0; i < 1000; ++i) {
//     auto* btn = new FluentButton(QString::number(i));
// }

// Good: Use button pools for dynamic content
QList<FluentButton*> buttonPool;
for (int i = 0; i < 10; ++i) {
    buttonPool.append(new FluentButton);
}
```

## See Also

- [FluentTextInput](FluentTextInput.md) - For form input components
- [FluentPanel](FluentPanel.md) - For container components
- [FluentAnimator](../animation/FluentAnimator.md) - For custom animations
- [FluentTheme](../styling/FluentTheme.md) - For theming and styling
