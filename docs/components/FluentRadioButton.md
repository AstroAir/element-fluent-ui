# FluentRadioButton Component

The `FluentRadioButton` component is an enhanced radio button widget that implements Microsoft's Fluent Design principles. It provides smooth animations, customizable appearance, and comprehensive group management features.

## Overview

FluentRadioButton offers:
- **Smooth animations** for state transitions
- **Custom styling** with theme integration
- **Group management** with automatic exclusivity
- **Rich content support** with icons and descriptions
- **Validation states** with visual feedback
- **Theme integration** with accent colors
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Radio Button

```cpp
#include "FluentQt/Components/FluentRadioButton.h"

// Create a basic radio button
auto* radioButton = new FluentRadioButton("Option 1");
radioButton->setChecked(true);

// Handle state changes
connect(radioButton, &FluentRadioButton::toggled, [=](bool checked) {
    if (checked) {
        qDebug() << "Option 1 selected";
    }
});

// Add to layout
layout->addWidget(radioButton);
```

### Radio Button Group

```cpp
// Create a group of radio buttons
auto* option1 = new FluentRadioButton("Small");
auto* option2 = new FluentRadioButton("Medium");
auto* option3 = new FluentRadioButton("Large");

// Set default selection
option2->setChecked(true);

// Add to layout
auto* groupLayout = new QVBoxLayout;
groupLayout->addWidget(option1);
groupLayout->addWidget(option2);
groupLayout->addWidget(option3);

// Handle group selection
auto handleSelection = [=]() {
    if (option1->isChecked()) qDebug() << "Small selected";
    else if (option2->isChecked()) qDebug() << "Medium selected";
    else if (option3->isChecked()) qDebug() << "Large selected";
};

connect(option1, &FluentRadioButton::toggled, handleSelection);
connect(option2, &FluentRadioButton::toggled, handleSelection);
connect(option3, &FluentRadioButton::toggled, handleSelection);
```

## Radio Button Groups

### Using QButtonGroup

```cpp
// Create button group for automatic exclusivity
auto* buttonGroup = new QButtonGroup(this);

auto* option1 = new FluentRadioButton("Option 1");
auto* option2 = new FluentRadioButton("Option 2");
auto* option3 = new FluentRadioButton("Option 3");

// Add to group
buttonGroup->addButton(option1, 0);
buttonGroup->addButton(option2, 1);
buttonGroup->addButton(option3, 2);

// Set default selection
option1->setChecked(true);

// Handle group selection changes
connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) {
    qDebug() << "Selected option:" << id;
});

// Get selected option
int selectedId = buttonGroup->checkedId();
FluentRadioButton* selectedButton = qobject_cast<FluentRadioButton*>(buttonGroup->checkedButton());
```

### Custom Group Management

```cpp
class FluentRadioButtonGroup : public QObject
{
    Q_OBJECT

public:
    FluentRadioButtonGroup(QObject* parent = nullptr) : QObject(parent) {}
    
    void addButton(FluentRadioButton* button, int id = -1) {
        if (id == -1) {
            id = m_buttons.size();
        }
        
        m_buttons[id] = button;
        connect(button, &FluentRadioButton::toggled, [=](bool checked) {
            if (checked) {
                setCheckedButton(id);
            }
        });
    }
    
    void setCheckedButton(int id) {
        if (m_checkedId == id) return;
        
        // Uncheck previous button
        if (m_checkedId != -1 && m_buttons.contains(m_checkedId)) {
            m_buttons[m_checkedId]->setChecked(false);
        }
        
        // Check new button
        if (m_buttons.contains(id)) {
            m_buttons[id]->setChecked(true);
            m_checkedId = id;
            emit selectionChanged(id);
        }
    }
    
    int checkedId() const { return m_checkedId; }
    FluentRadioButton* checkedButton() const {
        return m_checkedId != -1 ? m_buttons.value(m_checkedId) : nullptr;
    }

signals:
    void selectionChanged(int id);

private:
    QMap<int, FluentRadioButton*> m_buttons;
    int m_checkedId{-1};
};
```

## Rich Content

### Radio Buttons with Icons

```cpp
auto* option1 = new FluentRadioButton("Home");
option1->setIcon(QIcon(":/icons/home"));
option1->setIconSize(QSize(20, 20));

auto* option2 = new FluentRadioButton("Work");
option2->setIcon(QIcon(":/icons/work"));
option2->setIconSize(QSize(20, 20));

auto* option3 = new FluentRadioButton("Travel");
option3->setIcon(QIcon(":/icons/travel"));
option3->setIconSize(QSize(20, 20));

// Configure icon position
option1->setIconPosition(FluentIconPosition::Left);   // Default
option2->setIconPosition(FluentIconPosition::Right);
option3->setIconPosition(FluentIconPosition::Top);
```

### Radio Buttons with Descriptions

```cpp
auto* option1 = new FluentRadioButton("Basic Plan");
option1->setDescription("Essential features for personal use");
option1->setDescriptionFont(QFont("Segoe UI", 8));
option1->setDescriptionColor(QColor(128, 128, 128));

auto* option2 = new FluentRadioButton("Pro Plan");
option2->setDescription("Advanced features for professionals");
option2->setPrice("$9.99/month");

auto* option3 = new FluentRadioButton("Enterprise Plan");
option3->setDescription("Full feature set for large organizations");
option3->setPrice("$29.99/month");
option3->setBadgeText("Popular");
option3->setBadgeColor(QColor(0, 120, 215));
```

### Custom Content Layout

```cpp
class CustomRadioButton : public FluentRadioButton
{
public:
    CustomRadioButton(const QString& title, const QString& subtitle, 
                     const QIcon& icon, QWidget* parent = nullptr)
        : FluentRadioButton(parent) {
        setupCustomLayout(title, subtitle, icon);
    }

private:
    void setupCustomLayout(const QString& title, const QString& subtitle, const QIcon& icon) {
        // Create custom content widget
        auto* contentWidget = new QWidget;
        auto* layout = new QHBoxLayout(contentWidget);
        layout->setContentsMargins(8, 4, 8, 4);
        
        // Icon
        auto* iconLabel = new QLabel;
        iconLabel->setPixmap(icon.pixmap(32, 32));
        
        // Text content
        auto* textWidget = new QWidget;
        auto* textLayout = new QVBoxLayout(textWidget);
        textLayout->setContentsMargins(0, 0, 0, 0);
        textLayout->setSpacing(2);
        
        auto* titleLabel = new QLabel(title);
        titleLabel->setFont(QFont("Segoe UI", 10, QFont::Bold));
        
        auto* subtitleLabel = new QLabel(subtitle);
        subtitleLabel->setFont(QFont("Segoe UI", 8));
        subtitleLabel->setStyleSheet("color: #666;");
        
        textLayout->addWidget(titleLabel);
        textLayout->addWidget(subtitleLabel);
        
        layout->addWidget(iconLabel);
        layout->addWidget(textWidget);
        layout->addStretch();
        
        // Set as custom content
        setCustomContent(contentWidget);
    }
};

// Usage
auto* customOption1 = new CustomRadioButton("High Performance", 
    "Optimized for speed and responsiveness", QIcon(":/icons/performance"));
auto* customOption2 = new CustomRadioButton("Balanced", 
    "Good balance of performance and battery life", QIcon(":/icons/balanced"));
auto* customOption3 = new CustomRadioButton("Power Saver", 
    "Maximizes battery life", QIcon(":/icons/battery"));
```

## Styling and Theming

### Color Customization

```cpp
auto* radioButton = new FluentRadioButton("Custom Style");

// Use theme accent color (default)
radioButton->setUseAccentColor(true);

// Custom colors
radioButton->setCheckedColor(QColor(0, 120, 215));
radioButton->setUncheckedColor(QColor(200, 200, 200));
radioButton->setHoverColor(QColor(0, 100, 195));
radioButton->setPressedColor(QColor(0, 80, 175));

// Border colors
radioButton->setBorderColor(QColor(128, 128, 128));
radioButton->setCheckedBorderColor(QColor(0, 120, 215));

// Text colors
radioButton->setTextColor(QColor(0, 0, 0));
radioButton->setDisabledTextColor(QColor(128, 128, 128));
```

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* radioButton = new FluentRadioButton("Themed Radio Button");

// Automatic theme integration
auto& theme = FluentTheme::instance();
radioButton->setCheckedColor(theme.color("accent"));
radioButton->setHoverColor(theme.color("accentLight"));

// Respond to theme changes
connect(&theme, &FluentTheme::themeChanged, [=]() {
    radioButton->setCheckedColor(theme.color("accent"));
    radioButton->setHoverColor(theme.color("accentLight"));
});
```

## Animations

### State Transition Animations

```cpp
auto* radioButton = new FluentRadioButton("Animated Radio Button");

// Enable smooth animations
radioButton->setAnimated(true);
radioButton->setAnimationDuration(200);
radioButton->setAnimationEasing(FluentEasing::CubicOut);

// Different animation types
radioButton->setCheckAnimation(FluentCheckAnimation::Scale);
radioButton->setCheckAnimation(FluentCheckAnimation::Fade);
radioButton->setCheckAnimation(FluentCheckAnimation::Slide);

// Custom animations
connect(radioButton, &FluentRadioButton::animationStarted, [=](FluentAnimationType type) {
    if (type == FluentAnimationType::Check) {
        auto* animator = new FluentAnimator(this);
        animator->pulseEffect(radioButton, 300);
    }
});
```

### Hover Effects

```cpp
auto* radioButton = new FluentRadioButton("Hover Effects");

// Enable hover animations
radioButton->setHoverAnimationEnabled(true);
radioButton->setHoverAnimationDuration(150);

// Custom hover effects
connect(radioButton, &FluentRadioButton::hovered, [=](bool isHovered) {
    auto* animator = new FluentAnimator(this);
    
    if (isHovered) {
        // Scale up slightly on hover
        animator->scaleEffect(radioButton, 1.05, 150);
    } else {
        // Scale back to normal
        animator->scaleEffect(radioButton, 1.0, 150);
    }
});
```

## Validation States

### Visual Feedback

```cpp
auto* radioButton = new FluentRadioButton("Validation Example");

// Set validation state
radioButton->setValidationState(FluentValidationState::Valid);
radioButton->setValidationState(FluentValidationState::Invalid);
radioButton->setValidationState(FluentValidationState::Warning);

// Custom validation styling
radioButton->setValidColor(QColor(16, 124, 16));
radioButton->setInvalidColor(QColor(196, 43, 28));
radioButton->setWarningColor(QColor(255, 185, 0));

// Validation messages
radioButton->setValidationMessage("Selection is required");
radioButton->showValidationMessage(true);
```

### Form Validation

```cpp
class FormWithRadioButtons : public QWidget
{
public:
    FormWithRadioButtons(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupValidation();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Question 1
        auto* question1Label = new QLabel("What is your experience level?");
        question1Label->setFont(QFont("Segoe UI", 10, QFont::Bold));
        
        m_beginnerRadio = new FluentRadioButton("Beginner");
        m_intermediateRadio = new FluentRadioButton("Intermediate");
        m_advancedRadio = new FluentRadioButton("Advanced");
        
        m_experienceGroup = new QButtonGroup(this);
        m_experienceGroup->addButton(m_beginnerRadio, 0);
        m_experienceGroup->addButton(m_intermediateRadio, 1);
        m_experienceGroup->addButton(m_advancedRadio, 2);
        
        // Question 2
        auto* question2Label = new QLabel("How did you hear about us?");
        question2Label->setFont(QFont("Segoe UI", 10, QFont::Bold));
        
        m_searchRadio = new FluentRadioButton("Search Engine");
        m_socialRadio = new FluentRadioButton("Social Media");
        m_friendRadio = new FluentRadioButton("Friend/Colleague");
        m_otherRadio = new FluentRadioButton("Other");
        
        m_sourceGroup = new QButtonGroup(this);
        m_sourceGroup->addButton(m_searchRadio, 0);
        m_sourceGroup->addButton(m_socialRadio, 1);
        m_sourceGroup->addButton(m_friendRadio, 2);
        m_sourceGroup->addButton(m_otherRadio, 3);
        
        // Submit button
        m_submitButton = FluentButton::createPrimaryButton("Submit");
        m_submitButton->setEnabled(false);
        
        layout->addWidget(question1Label);
        layout->addWidget(m_beginnerRadio);
        layout->addWidget(m_intermediateRadio);
        layout->addWidget(m_advancedRadio);
        layout->addSpacing(20);
        layout->addWidget(question2Label);
        layout->addWidget(m_searchRadio);
        layout->addWidget(m_socialRadio);
        layout->addWidget(m_friendRadio);
        layout->addWidget(m_otherRadio);
        layout->addSpacing(20);
        layout->addWidget(m_submitButton);
        
        connect(m_submitButton, &FluentButton::clicked, this, &FormWithRadioButtons::submitForm);
    }
    
    void setupValidation() {
        // Validate form when selections change
        auto validateForm = [=]() {
            bool experienceSelected = m_experienceGroup->checkedButton() != nullptr;
            bool sourceSelected = m_sourceGroup->checkedButton() != nullptr;
            
            // Update validation states
            updateValidationState(m_beginnerRadio, experienceSelected);
            updateValidationState(m_intermediateRadio, experienceSelected);
            updateValidationState(m_advancedRadio, experienceSelected);
            
            updateValidationState(m_searchRadio, sourceSelected);
            updateValidationState(m_socialRadio, sourceSelected);
            updateValidationState(m_friendRadio, sourceSelected);
            updateValidationState(m_otherRadio, sourceSelected);
            
            // Enable submit button if all required fields are filled
            m_submitButton->setEnabled(experienceSelected && sourceSelected);
        };
        
        connect(m_experienceGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), validateForm);
        connect(m_sourceGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), validateForm);
    }
    
    void updateValidationState(FluentRadioButton* radioButton, bool isValid) {
        if (isValid) {
            radioButton->setValidationState(FluentValidationState::Valid);
            radioButton->showValidationMessage(false);
        } else {
            radioButton->setValidationState(FluentValidationState::Invalid);
            radioButton->setValidationMessage("Please make a selection");
            radioButton->showValidationMessage(true);
        }
    }
    
    void submitForm() {
        int experience = m_experienceGroup->checkedId();
        int source = m_sourceGroup->checkedId();
        
        qDebug() << "Experience level:" << experience;
        qDebug() << "Source:" << source;
        
        emit formSubmitted(experience, source);
    }

signals:
    void formSubmitted(int experience, int source);

private:
    QButtonGroup* m_experienceGroup{nullptr};
    QButtonGroup* m_sourceGroup{nullptr};
    
    FluentRadioButton* m_beginnerRadio{nullptr};
    FluentRadioButton* m_intermediateRadio{nullptr};
    FluentRadioButton* m_advancedRadio{nullptr};
    
    FluentRadioButton* m_searchRadio{nullptr};
    FluentRadioButton* m_socialRadio{nullptr};
    FluentRadioButton* m_friendRadio{nullptr};
    FluentRadioButton* m_otherRadio{nullptr};
    
    FluentButton* m_submitButton{nullptr};
};
```

## Settings Panel Example

### Preferences Dialog

```cpp
class PreferencesDialog : public QDialog
{
public:
    PreferencesDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setupUI();
        loadSettings();
        setupConnections();
    }

private:
    void setupUI() {
        setWindowTitle("Preferences");
        setFixedSize(400, 500);
        
        auto* layout = new QVBoxLayout(this);
        
        // Theme selection
        auto* themeGroup = new QGroupBox("Theme");
        auto* themeLayout = new QVBoxLayout(themeGroup);
        
        m_lightThemeRadio = new FluentRadioButton("Light Theme");
        m_lightThemeRadio->setDescription("Clean and bright interface");
        m_lightThemeRadio->setIcon(QIcon(":/icons/light-theme"));
        
        m_darkThemeRadio = new FluentRadioButton("Dark Theme");
        m_darkThemeRadio->setDescription("Easy on the eyes in low light");
        m_darkThemeRadio->setIcon(QIcon(":/icons/dark-theme"));
        
        m_autoThemeRadio = new FluentRadioButton("Auto Theme");
        m_autoThemeRadio->setDescription("Follows system theme");
        m_autoThemeRadio->setIcon(QIcon(":/icons/auto-theme"));
        
        m_themeGroup = new QButtonGroup(this);
        m_themeGroup->addButton(m_lightThemeRadio, 0);
        m_themeGroup->addButton(m_darkThemeRadio, 1);
        m_themeGroup->addButton(m_autoThemeRadio, 2);
        
        themeLayout->addWidget(m_lightThemeRadio);
        themeLayout->addWidget(m_darkThemeRadio);
        themeLayout->addWidget(m_autoThemeRadio);
        
        // Language selection
        auto* languageGroup = new QGroupBox("Language");
        auto* languageLayout = new QVBoxLayout(languageGroup);
        
        m_englishRadio = new FluentRadioButton("English");
        m_spanishRadio = new FluentRadioButton("Español");
        m_frenchRadio = new FluentRadioButton("Français");
        m_germanRadio = new FluentRadioButton("Deutsch");
        
        m_languageGroup = new QButtonGroup(this);
        m_languageGroup->addButton(m_englishRadio, 0);
        m_languageGroup->addButton(m_spanishRadio, 1);
        m_languageGroup->addButton(m_frenchRadio, 2);
        m_languageGroup->addButton(m_germanRadio, 3);
        
        languageLayout->addWidget(m_englishRadio);
        languageLayout->addWidget(m_spanishRadio);
        languageLayout->addWidget(m_frenchRadio);
        languageLayout->addWidget(m_germanRadio);
        
        // Startup behavior
        auto* startupGroup = new QGroupBox("Startup Behavior");
        auto* startupLayout = new QVBoxLayout(startupGroup);
        
        m_startMinimizedRadio = new FluentRadioButton("Start Minimized");
        m_startNormalRadio = new FluentRadioButton("Start Normally");
        m_restoreSessionRadio = new FluentRadioButton("Restore Last Session");
        
        m_startupGroup = new QButtonGroup(this);
        m_startupGroup->addButton(m_startMinimizedRadio, 0);
        m_startupGroup->addButton(m_startNormalRadio, 1);
        m_startupGroup->addButton(m_restoreSessionRadio, 2);
        
        startupLayout->addWidget(m_startMinimizedRadio);
        startupLayout->addWidget(m_startNormalRadio);
        startupLayout->addWidget(m_restoreSessionRadio);
        
        // Buttons
        auto* buttonLayout = new QHBoxLayout;
        auto* okButton = FluentButton::createPrimaryButton("OK");
        auto* cancelButton = new FluentButton("Cancel");
        auto* applyButton = new FluentButton("Apply");
        
        buttonLayout->addStretch();
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(applyButton);
        
        layout->addWidget(themeGroup);
        layout->addWidget(languageGroup);
        layout->addWidget(startupGroup);
        layout->addLayout(buttonLayout);
        
        connect(okButton, &FluentButton::clicked, this, &PreferencesDialog::accept);
        connect(cancelButton, &FluentButton::clicked, this, &PreferencesDialog::reject);
        connect(applyButton, &FluentButton::clicked, this, &PreferencesDialog::applySettings);
    }
    
    void setupConnections() {
        // Auto-apply theme changes for preview
        connect(m_themeGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) {
            applyTheme(id);
        });
    }
    
    void loadSettings() {
        QSettings settings;
        
        int theme = settings.value("theme", 0).toInt();
        m_themeGroup->button(theme)->setChecked(true);
        
        int language = settings.value("language", 0).toInt();
        m_languageGroup->button(language)->setChecked(true);
        
        int startup = settings.value("startup", 1).toInt();
        m_startupGroup->button(startup)->setChecked(true);
    }
    
    void applySettings() {
        QSettings settings;
        
        settings.setValue("theme", m_themeGroup->checkedId());
        settings.setValue("language", m_languageGroup->checkedId());
        settings.setValue("startup", m_startupGroup->checkedId());
        
        emit settingsChanged();
    }
    
    void applyTheme(int themeId) {
        auto& theme = FluentTheme::instance();
        switch (themeId) {
            case 0: theme.setTheme(FluentTheme::Light); break;
            case 1: theme.setTheme(FluentTheme::Dark); break;
            case 2: theme.setTheme(FluentTheme::Auto); break;
        }
    }

signals:
    void settingsChanged();

private:
    QButtonGroup* m_themeGroup{nullptr};
    QButtonGroup* m_languageGroup{nullptr};
    QButtonGroup* m_startupGroup{nullptr};
    
    FluentRadioButton* m_lightThemeRadio{nullptr};
    FluentRadioButton* m_darkThemeRadio{nullptr};
    FluentRadioButton* m_autoThemeRadio{nullptr};
    
    FluentRadioButton* m_englishRadio{nullptr};
    FluentRadioButton* m_spanishRadio{nullptr};
    FluentRadioButton* m_frenchRadio{nullptr};
    FluentRadioButton* m_germanRadio{nullptr};
    
    FluentRadioButton* m_startMinimizedRadio{nullptr};
    FluentRadioButton* m_startNormalRadio{nullptr};
    FluentRadioButton* m_restoreSessionRadio{nullptr};
};
```

## Accessibility

FluentRadioButton provides comprehensive accessibility support:

```cpp
auto* radioButton = new FluentRadioButton("Accessible Radio Button");

// Accessible properties (automatically set)
radioButton->setAccessibleName("Option selection");
radioButton->setAccessibleDescription("Select this option from the available choices");

// Keyboard navigation (automatic)
// - Tab to navigate between radio buttons
// - Arrow keys to navigate within groups
// - Space to select radio button
// - Enter to activate

// Screen reader support
radioButton->setAccessibleRole(QAccessible::RadioButton);

// State announcements
connect(radioButton, &FluentRadioButton::toggled, [=](bool checked) {
    if (checked) {
        QString announcement = QString("%1 selected").arg(radioButton->text());
        radioButton->announceToScreenReader(announcement);
    }
});
```

## Best Practices

### Radio Button Design Guidelines
1. **Use for mutually exclusive options** - Only one selection allowed
2. **Provide clear labels** - Make options easy to understand
3. **Group related options** - Use visual grouping for clarity
4. **Set sensible defaults** - Pre-select the most common option
5. **Support keyboard navigation** - Ensure accessibility

### Performance Considerations
- Use efficient group management for large sets
- Cache styled elements when possible
- Optimize animations for smooth interaction
- Implement lazy loading for dynamic options

### Accessibility Guidelines
- Support keyboard navigation
- Provide state announcements
- Use sufficient color contrast
- Include meaningful labels and descriptions
- Test with assistive technologies

## Signals Reference

```cpp
// State change signals
void toggled(bool checked);
void clicked();
void pressed();
void released();

// Animation signals
void animationStarted(FluentAnimationType type);
void animationFinished(FluentAnimationType type);

// Interaction signals
void hovered(bool isHovered);
void focusChanged(bool hasFocus);
```

## Standalone Examples Collection

### Example 1: Product Configuration Interface

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QButtonGroup>
#include <QSpinBox>
#include "FluentQt/Components/FluentRadioButton.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"

class ProductConfigurator : public QWidget {
    Q_OBJECT
public:
    ProductConfigurator(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        connectSignals();
        updatePricing();
    }

private slots:
    void updatePricing() {
        double basePrice = 999.0;
        double sizeMultiplier = 1.0;
        double colorMultiplier = 1.0;
        double storageMultiplier = 1.0;

        // Size pricing
        if (m_sizeGroup->checkedId() == 1) sizeMultiplier = 1.2; // Medium
        else if (m_sizeGroup->checkedId() == 2) sizeMultiplier = 1.5; // Large

        // Color pricing
        if (m_colorGroup->checkedId() == 1) colorMultiplier = 1.1; // Premium color
        else if (m_colorGroup->checkedId() == 2) colorMultiplier = 1.15; // Special edition

        // Storage pricing
        if (m_storageGroup->checkedId() == 1) storageMultiplier = 1.3; // 256GB
        else if (m_storageGroup->checkedId() == 2) storageMultiplier = 1.6; // 512GB
        else if (m_storageGroup->checkedId() == 3) storageMultiplier = 2.0; // 1TB

        double totalPrice = basePrice * sizeMultiplier * colorMultiplier * storageMultiplier;
        int quantity = m_quantitySpinBox->value();

        m_priceLabel->setText(QString("Price: $%1 each").arg(totalPrice, 0, 'f', 2));
        m_totalLabel->setText(QString("Total: $%1").arg(totalPrice * quantity, 0, 'f', 2));

        updateConfigurationSummary();
    }

    void updateConfigurationSummary() {
        QStringList config;

        // Size
        auto* sizeButton = qobject_cast<FluentRadioButton*>(m_sizeGroup->checkedButton());
        if (sizeButton) config << "Size: " + sizeButton->text();

        // Color
        auto* colorButton = qobject_cast<FluentRadioButton*>(m_colorGroup->checkedButton());
        if (colorButton) config << "Color: " + colorButton->text();

        // Storage
        auto* storageButton = qobject_cast<FluentRadioButton*>(m_storageGroup->checkedButton());
        if (storageButton) config << "Storage: " + storageButton->text();

        m_configLabel->setText(config.join(" • "));
    }

    void addToCart() {
        m_addToCartButton->setLoading(true);
        m_addToCartButton->setText("Adding to Cart...");

        // Simulate adding to cart
        QTimer::singleShot(1500, [this]() {
            m_addToCartButton->setLoading(false);
            m_addToCartButton->setText("✓ Added to Cart");
            m_addToCartButton->setButtonStyle(FluentButtonStyle::Accent);

            QTimer::singleShot(2000, [this]() {
                m_addToCartButton->setText("Add to Cart");
                m_addToCartButton->setButtonStyle(FluentButtonStyle::Primary);
            });
        });
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // Product title
        auto* title = new QLabel("Customize Your Product");
        title->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 20px;");

        // Size selection
        auto* sizeCard = new FluentCard;
        auto* sizeLayout = new QVBoxLayout(sizeCard);
        auto* sizeTitle = new QLabel("Size");
        sizeTitle->setStyleSheet("font-weight: bold; margin-bottom: 10px;");

        m_sizeGroup = new QButtonGroup(this);

        auto* smallSize = new FluentRadioButton("Small (13\")");
        smallSize->setDescription("Perfect for portability");
        auto* mediumSize = new FluentRadioButton("Medium (15\")");
        mediumSize->setDescription("Balanced performance and portability (+$200)");
        auto* largeSize = new FluentRadioButton("Large (17\")");
        largeSize->setDescription("Maximum screen real estate (+$500)");

        m_sizeGroup->addButton(smallSize, 0);
        m_sizeGroup->addButton(mediumSize, 1);
        m_sizeGroup->addButton(largeSize, 2);

        smallSize->setChecked(true); // Default selection

        sizeLayout->addWidget(sizeTitle);
        sizeLayout->addWidget(smallSize);
        sizeLayout->addWidget(mediumSize);
        sizeLayout->addWidget(largeSize);

        // Color selection
        auto* colorCard = new FluentCard;
        auto* colorLayout = new QVBoxLayout(colorCard);
        auto* colorTitle = new QLabel("Color");
        colorTitle->setStyleSheet("font-weight: bold; margin-bottom: 10px;");

        m_colorGroup = new QButtonGroup(this);

        auto* silverColor = new FluentRadioButton("Silver");
        silverColor->setDescription("Classic silver finish");
        auto* spaceGray = new FluentRadioButton("Space Gray");
        spaceGray->setDescription("Premium space gray (+$100)");
        auto* goldColor = new FluentRadioButton("Gold");
        goldColor->setDescription("Limited edition gold (+$150)");

        m_colorGroup->addButton(silverColor, 0);
        m_colorGroup->addButton(spaceGray, 1);
        m_colorGroup->addButton(goldColor, 2);

        silverColor->setChecked(true);

        colorLayout->addWidget(colorTitle);
        colorLayout->addWidget(silverColor);
        colorLayout->addWidget(spaceGray);
        colorLayout->addWidget(goldColor);

        // Storage selection
        auto* storageCard = new FluentCard;
        auto* storageLayout = new QVBoxLayout(storageCard);
        auto* storageTitle = new QLabel("Storage");
        storageTitle->setStyleSheet("font-weight: bold; margin-bottom: 10px;");

        m_storageGroup = new QButtonGroup(this);

        auto* storage128 = new FluentRadioButton("128GB");
        storage128->setDescription("Basic storage");
        auto* storage256 = new FluentRadioButton("256GB");
        storage256->setDescription("Recommended for most users (+$300)");
        auto* storage512 = new FluentRadioButton("512GB");
        storage512->setDescription("For power users (+$600)");
        auto* storage1TB = new FluentRadioButton("1TB");
        storage1TB->setDescription("Maximum storage (+$1000)");

        m_storageGroup->addButton(storage128, 0);
        m_storageGroup->addButton(storage256, 1);
        m_storageGroup->addButton(storage512, 2);
        m_storageGroup->addButton(storage1TB, 3);

        storage128->setChecked(true);

        storageLayout->addWidget(storageTitle);
        storageLayout->addWidget(storage128);
        storageLayout->addWidget(storage256);
        storageLayout->addWidget(storage512);
        storageLayout->addWidget(storage1TB);

        // Quantity and pricing
        auto* orderCard = new FluentCard;
        auto* orderLayout = new QVBoxLayout(orderCard);

        auto* quantityLayout = new QHBoxLayout;
        auto* quantityLabel = new QLabel("Quantity:");
        m_quantitySpinBox = new QSpinBox;
        m_quantitySpinBox->setRange(1, 10);
        m_quantitySpinBox->setValue(1);

        quantityLayout->addWidget(quantityLabel);
        quantityLayout->addWidget(m_quantitySpinBox);
        quantityLayout->addStretch();

        m_configLabel = new QLabel;
        m_configLabel->setStyleSheet("color: gray; margin: 10px 0;");

        m_priceLabel = new QLabel("Price: $999.00 each");
        m_priceLabel->setStyleSheet("font-size: 16px; font-weight: bold;");

        m_totalLabel = new QLabel("Total: $999.00");
        m_totalLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #0078d4;");

        m_addToCartButton = new FluentButton("Add to Cart");
        m_addToCartButton->setButtonStyle(FluentButtonStyle::Primary);

        orderLayout->addLayout(quantityLayout);
        orderLayout->addWidget(m_configLabel);
        orderLayout->addWidget(m_priceLabel);
        orderLayout->addWidget(m_totalLabel);
        orderLayout->addWidget(m_addToCartButton);

        layout->addWidget(title);
        layout->addWidget(sizeCard);
        layout->addWidget(colorCard);
        layout->addWidget(storageCard);
        layout->addWidget(orderCard);
    }

    void connectSignals() {
        // Connect all radio button groups to update pricing
        connect(m_sizeGroup, QOverload<int>::of(&QButtonGroup::idClicked),
                this, &ProductConfigurator::updatePricing);
        connect(m_colorGroup, QOverload<int>::of(&QButtonGroup::idClicked),
                this, &ProductConfigurator::updatePricing);
        connect(m_storageGroup, QOverload<int>::of(&QButtonGroup::idClicked),
                this, &ProductConfigurator::updatePricing);

        // Connect quantity changes
        connect(m_quantitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &ProductConfigurator::updatePricing);

        // Connect add to cart button
        connect(m_addToCartButton, &FluentButton::clicked,
                this, &ProductConfigurator::addToCart);
    }

    QButtonGroup* m_sizeGroup;
    QButtonGroup* m_colorGroup;
    QButtonGroup* m_storageGroup;
    QSpinBox* m_quantitySpinBox;
    QLabel* m_configLabel;
    QLabel* m_priceLabel;
    QLabel* m_totalLabel;
    FluentButton* m_addToCartButton;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ProductConfigurator configurator;
    configurator.show();

    return app.exec();
}

#include "product_configurator.moc"
```

## See Also

- [FluentCheckBox](FluentCheckBox.md) - For multiple selection options
- [FluentButton](FluentButton.md) - For action buttons
- [FluentComboBox](FluentComboBox.md) - For dropdown selection
- [API Reference - FluentRadioButton](../API_REFERENCE.md#fluentradiobutton) - Complete API documentation
