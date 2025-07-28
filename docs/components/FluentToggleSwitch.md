# FluentToggleSwitch Component

The `FluentToggleSwitch` component is an enhanced toggle switch widget that implements Microsoft's Fluent Design principles. It provides smooth animations, customizable appearance, and comprehensive state management for binary choices.

## Overview

FluentToggleSwitch offers:
- **Smooth toggle animations** with customizable easing
- **Rich content support** with labels and descriptions
- **Custom styling** with theme integration
- **State validation** with visual feedback
- **Icon support** for on/off states
- **Theme integration** with accent colors
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Toggle Switch

```cpp
#include "FluentQt/Components/FluentToggleSwitch.h"

// Create a basic toggle switch
auto* toggleSwitch = new FluentToggleSwitch("Enable notifications");
toggleSwitch->setChecked(true);

// Handle state changes
connect(toggleSwitch, &FluentToggleSwitch::toggled, [=](bool checked) {
    if (checked) {
        qDebug() << "Notifications enabled";
    } else {
        qDebug() << "Notifications disabled";
    }
});

// Add to layout
layout->addWidget(toggleSwitch);
```

### Toggle Switch with Description

```cpp
auto* toggleSwitch = new FluentToggleSwitch("Dark Mode");
toggleSwitch->setDescription("Switch between light and dark themes");
toggleSwitch->setChecked(false);

// Handle theme changes
connect(toggleSwitch, &FluentToggleSwitch::toggled, [=](bool checked) {
    if (checked) {
        applyDarkTheme();
    } else {
        applyLightTheme();
    }
});
```

## Content and Labels

### Custom On/Off Labels

```cpp
auto* toggleSwitch = new FluentToggleSwitch("Auto-save");

// Set custom on/off text
toggleSwitch->setOnText("ON");
toggleSwitch->setOffText("OFF");

// Show text on the switch itself
toggleSwitch->setShowOnOffText(true);
toggleSwitch->setOnOffTextPosition(FluentToggleTextPosition::Inside);

// Alternative: show text beside the switch
toggleSwitch->setOnOffTextPosition(FluentToggleTextPosition::Beside);
```

### Toggle Switch with Icons

```cpp
auto* toggleSwitch = new FluentToggleSwitch("Wi-Fi");

// Set icons for on/off states
toggleSwitch->setOnIcon(QIcon(":/icons/wifi-on"));
toggleSwitch->setOffIcon(QIcon(":/icons/wifi-off"));
toggleSwitch->setShowIcons(true);
toggleSwitch->setIconSize(QSize(16, 16));

// Icon position
toggleSwitch->setIconPosition(FluentIconPosition::Left);
toggleSwitch->setIconPosition(FluentIconPosition::Inside);  // Inside the switch
```

### Rich Content Toggle

```cpp
auto* toggleSwitch = new FluentToggleSwitch("Location Services");
toggleSwitch->setDescription("Allow apps to access your location");
toggleSwitch->setDetailText("This helps provide location-based features and services");

// Configure appearance
toggleSwitch->setDescriptionFont(QFont("Segoe UI", 9));
toggleSwitch->setDetailFont(QFont("Segoe UI", 8));
toggleSwitch->setDescriptionColor(QColor(96, 96, 96));
toggleSwitch->setDetailColor(QColor(128, 128, 128));

// Multi-line layout
toggleSwitch->setContentLayout(FluentToggleContentLayout::Vertical);
```

## Styling and Theming

### Color Customization

```cpp
auto* toggleSwitch = new FluentToggleSwitch("Custom Colors");

// Use theme accent color (default)
toggleSwitch->setUseAccentColor(true);

// Custom colors
toggleSwitch->setOnColor(QColor(0, 120, 215));
toggleSwitch->setOffColor(QColor(200, 200, 200));
toggleSwitch->setThumbColor(QColor(255, 255, 255));
toggleSwitch->setDisabledColor(QColor(128, 128, 128));

// Border colors
toggleSwitch->setBorderColor(QColor(128, 128, 128));
toggleSwitch->setOnBorderColor(QColor(0, 120, 215));

// Hover and pressed states
toggleSwitch->setHoverColor(QColor(0, 100, 195));
toggleSwitch->setPressedColor(QColor(0, 80, 175));
```

### Size and Shape

```cpp
auto* toggleSwitch = new FluentToggleSwitch("Size Options");

// Size presets
toggleSwitch->setSize(FluentToggleSize::Small);
toggleSwitch->setSize(FluentToggleSize::Medium);   // Default
toggleSwitch->setSize(FluentToggleSize::Large);

// Custom dimensions
toggleSwitch->setCustomSize(QSize(60, 30));
toggleSwitch->setThumbSize(QSize(24, 24));

// Shape options
toggleSwitch->setCornerRadius(15);  // Rounded corners
toggleSwitch->setThumbCornerRadius(12);
```

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* toggleSwitch = new FluentToggleSwitch("Themed Toggle");

// Automatic theme integration
auto& theme = FluentTheme::instance();
toggleSwitch->setOnColor(theme.color("accent"));
toggleSwitch->setHoverColor(theme.color("accentLight"));

// Respond to theme changes
connect(&theme, &FluentTheme::themeChanged, [=]() {
    toggleSwitch->setOnColor(theme.color("accent"));
    toggleSwitch->setHoverColor(theme.color("accentLight"));
});
```

## Animations

### Toggle Animations

```cpp
auto* toggleSwitch = new FluentToggleSwitch("Animated Toggle");

// Enable smooth animations
toggleSwitch->setAnimated(true);
toggleSwitch->setAnimationDuration(200);
toggleSwitch->setAnimationEasing(FluentEasing::CubicOut);

// Different animation types
toggleSwitch->setToggleAnimation(FluentToggleAnimation::Slide);
toggleSwitch->setToggleAnimation(FluentToggleAnimation::Fade);
toggleSwitch->setToggleAnimation(FluentToggleAnimation::Scale);

// Custom animations
connect(toggleSwitch, &FluentToggleSwitch::animationStarted, [=](FluentAnimationType type) {
    if (type == FluentAnimationType::Toggle) {
        auto* animator = new FluentAnimator(this);
        animator->pulseEffect(toggleSwitch, 300);
    }
});
```

### Hover and Focus Effects

```cpp
auto* toggleSwitch = new FluentToggleSwitch("Interactive Effects");

// Enable hover animations
toggleSwitch->setHoverAnimationEnabled(true);
toggleSwitch->setHoverAnimationDuration(150);

// Focus ring
toggleSwitch->setFocusRingEnabled(true);
toggleSwitch->setFocusRingColor(QColor(0, 120, 215));
toggleSwitch->setFocusRingWidth(2);

// Custom hover effects
connect(toggleSwitch, &FluentToggleSwitch::hovered, [=](bool isHovered) {
    auto* animator = new FluentAnimator(this);
    
    if (isHovered) {
        // Glow effect on hover
        animator->glowEffect(toggleSwitch, QColor(0, 120, 215), 150);
    } else {
        // Remove glow
        animator->removeGlowEffect(toggleSwitch, 150);
    }
});
```

## Validation and States

### Validation States

```cpp
auto* toggleSwitch = new FluentToggleSwitch("Validation Example");

// Set validation state
toggleSwitch->setValidationState(FluentValidationState::Valid);
toggleSwitch->setValidationState(FluentValidationState::Invalid);
toggleSwitch->setValidationState(FluentValidationState::Warning);

// Custom validation styling
toggleSwitch->setValidColor(QColor(16, 124, 16));
toggleSwitch->setInvalidColor(QColor(196, 43, 28));
toggleSwitch->setWarningColor(QColor(255, 185, 0));

// Validation messages
toggleSwitch->setValidationMessage("This setting is required");
toggleSwitch->showValidationMessage(true);
```

### Conditional Enabling

```cpp
auto* parentToggle = new FluentToggleSwitch("Enable Feature");
auto* childToggle = new FluentToggleSwitch("Advanced Options");

// Child toggle depends on parent
childToggle->setEnabled(parentToggle->isChecked());

connect(parentToggle, &FluentToggleSwitch::toggled, [=](bool checked) {
    childToggle->setEnabled(checked);
    
    if (!checked) {
        childToggle->setChecked(false);  // Reset child when parent disabled
    }
});
```

## Integration Examples

### Settings Panel

```cpp
class SettingsPanel : public QWidget
{
public:
    SettingsPanel(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        loadSettings();
        setupConnections();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // General settings group
        auto* generalGroup = new QGroupBox("General");
        auto* generalLayout = new QVBoxLayout(generalGroup);
        
        m_autoStartToggle = new FluentToggleSwitch("Start with Windows");
        m_autoStartToggle->setDescription("Automatically start the application when Windows starts");
        
        m_minimizeToTrayToggle = new FluentToggleSwitch("Minimize to system tray");
        m_minimizeToTrayToggle->setDescription("Hide the application in the system tray when minimized");
        
        m_checkUpdatesToggle = new FluentToggleSwitch("Check for updates");
        m_checkUpdatesToggle->setDescription("Automatically check for application updates");
        
        generalLayout->addWidget(m_autoStartToggle);
        generalLayout->addWidget(m_minimizeToTrayToggle);
        generalLayout->addWidget(m_checkUpdatesToggle);
        
        // Privacy settings group
        auto* privacyGroup = new QGroupBox("Privacy");
        auto* privacyLayout = new QVBoxLayout(privacyGroup);
        
        m_telemetryToggle = new FluentToggleSwitch("Send usage data");
        m_telemetryToggle->setDescription("Help improve the application by sending anonymous usage statistics");
        m_telemetryToggle->setDetailText("No personal information is collected");
        
        m_crashReportsToggle = new FluentToggleSwitch("Send crash reports");
        m_crashReportsToggle->setDescription("Automatically send crash reports to help fix issues");
        
        privacyLayout->addWidget(m_telemetryToggle);
        privacyLayout->addWidget(m_crashReportsToggle);
        
        // Appearance settings group
        auto* appearanceGroup = new QGroupBox("Appearance");
        auto* appearanceLayout = new QVBoxLayout(appearanceGroup);
        
        m_darkModeToggle = new FluentToggleSwitch("Dark mode");
        m_darkModeToggle->setDescription("Use dark theme for the interface");
        m_darkModeToggle->setOnIcon(QIcon(":/icons/moon"));
        m_darkModeToggle->setOffIcon(QIcon(":/icons/sun"));
        m_darkModeToggle->setShowIcons(true);
        
        m_animationsToggle = new FluentToggleSwitch("Enable animations");
        m_animationsToggle->setDescription("Show smooth animations and transitions");
        
        m_soundsToggle = new FluentToggleSwitch("System sounds");
        m_soundsToggle->setDescription("Play sounds for notifications and events");
        
        appearanceLayout->addWidget(m_darkModeToggle);
        appearanceLayout->addWidget(m_animationsToggle);
        appearanceLayout->addWidget(m_soundsToggle);
        
        // Notifications settings group
        auto* notificationsGroup = new QGroupBox("Notifications");
        auto* notificationsLayout = new QVBoxLayout(notificationsGroup);
        
        m_notificationsToggle = new FluentToggleSwitch("Enable notifications");
        m_notificationsToggle->setDescription("Show desktop notifications");
        
        m_soundNotificationsToggle = new FluentToggleSwitch("Notification sounds");
        m_soundNotificationsToggle->setDescription("Play sound when notifications appear");
        
        m_badgeToggle = new FluentToggleSwitch("Show badge count");
        m_badgeToggle->setDescription("Display notification count on taskbar icon");
        
        // Notification sound depends on notifications being enabled
        m_soundNotificationsToggle->setEnabled(m_notificationsToggle->isChecked());
        m_badgeToggle->setEnabled(m_notificationsToggle->isChecked());
        
        notificationsLayout->addWidget(m_notificationsToggle);
        notificationsLayout->addWidget(m_soundNotificationsToggle);
        notificationsLayout->addWidget(m_badgeToggle);
        
        layout->addWidget(generalGroup);
        layout->addWidget(privacyGroup);
        layout->addWidget(appearanceGroup);
        layout->addWidget(notificationsGroup);
        layout->addStretch();
    }
    
    void setupConnections() {
        // Auto-save settings when changed
        connect(m_autoStartToggle, &FluentToggleSwitch::toggled, this, &SettingsPanel::saveSettings);
        connect(m_minimizeToTrayToggle, &FluentToggleSwitch::toggled, this, &SettingsPanel::saveSettings);
        connect(m_checkUpdatesToggle, &FluentToggleSwitch::toggled, this, &SettingsPanel::saveSettings);
        connect(m_telemetryToggle, &FluentToggleSwitch::toggled, this, &SettingsPanel::saveSettings);
        connect(m_crashReportsToggle, &FluentToggleSwitch::toggled, this, &SettingsPanel::saveSettings);
        connect(m_darkModeToggle, &FluentToggleSwitch::toggled, this, &SettingsPanel::saveSettings);
        connect(m_animationsToggle, &FluentToggleSwitch::toggled, this, &SettingsPanel::saveSettings);
        connect(m_soundsToggle, &FluentToggleSwitch::toggled, this, &SettingsPanel::saveSettings);
        connect(m_notificationsToggle, &FluentToggleSwitch::toggled, this, &SettingsPanel::saveSettings);
        connect(m_soundNotificationsToggle, &FluentToggleSwitch::toggled, this, &SettingsPanel::saveSettings);
        connect(m_badgeToggle, &FluentToggleSwitch::toggled, this, &SettingsPanel::saveSettings);
        
        // Handle dependent settings
        connect(m_notificationsToggle, &FluentToggleSwitch::toggled, [=](bool checked) {
            m_soundNotificationsToggle->setEnabled(checked);
            m_badgeToggle->setEnabled(checked);
            
            if (!checked) {
                m_soundNotificationsToggle->setChecked(false);
                m_badgeToggle->setChecked(false);
            }
        });
        
        // Apply theme changes immediately
        connect(m_darkModeToggle, &FluentToggleSwitch::toggled, [=](bool checked) {
            auto& theme = FluentTheme::instance();
            theme.setTheme(checked ? FluentTheme::Dark : FluentTheme::Light);
        });
        
        // Apply animation settings immediately
        connect(m_animationsToggle, &FluentToggleSwitch::toggled, [=](bool checked) {
            FluentAnimator::setGlobalAnimationsEnabled(checked);
        });
    }
    
    void loadSettings() {
        QSettings settings;
        
        m_autoStartToggle->setChecked(settings.value("general/autoStart", false).toBool());
        m_minimizeToTrayToggle->setChecked(settings.value("general/minimizeToTray", true).toBool());
        m_checkUpdatesToggle->setChecked(settings.value("general/checkUpdates", true).toBool());
        
        m_telemetryToggle->setChecked(settings.value("privacy/telemetry", false).toBool());
        m_crashReportsToggle->setChecked(settings.value("privacy/crashReports", true).toBool());
        
        m_darkModeToggle->setChecked(settings.value("appearance/darkMode", false).toBool());
        m_animationsToggle->setChecked(settings.value("appearance/animations", true).toBool());
        m_soundsToggle->setChecked(settings.value("appearance/sounds", true).toBool());
        
        m_notificationsToggle->setChecked(settings.value("notifications/enabled", true).toBool());
        m_soundNotificationsToggle->setChecked(settings.value("notifications/sounds", true).toBool());
        m_badgeToggle->setChecked(settings.value("notifications/badge", true).toBool());
    }
    
    void saveSettings() {
        QSettings settings;
        
        settings.setValue("general/autoStart", m_autoStartToggle->isChecked());
        settings.setValue("general/minimizeToTray", m_minimizeToTrayToggle->isChecked());
        settings.setValue("general/checkUpdates", m_checkUpdatesToggle->isChecked());
        
        settings.setValue("privacy/telemetry", m_telemetryToggle->isChecked());
        settings.setValue("privacy/crashReports", m_crashReportsToggle->isChecked());
        
        settings.setValue("appearance/darkMode", m_darkModeToggle->isChecked());
        settings.setValue("appearance/animations", m_animationsToggle->isChecked());
        settings.setValue("appearance/sounds", m_soundsToggle->isChecked());
        
        settings.setValue("notifications/enabled", m_notificationsToggle->isChecked());
        settings.setValue("notifications/sounds", m_soundNotificationsToggle->isChecked());
        settings.setValue("notifications/badge", m_badgeToggle->isChecked());
        
        emit settingsChanged();
    }

signals:
    void settingsChanged();

private:
    FluentToggleSwitch* m_autoStartToggle{nullptr};
    FluentToggleSwitch* m_minimizeToTrayToggle{nullptr};
    FluentToggleSwitch* m_checkUpdatesToggle{nullptr};
    FluentToggleSwitch* m_telemetryToggle{nullptr};
    FluentToggleSwitch* m_crashReportsToggle{nullptr};
    FluentToggleSwitch* m_darkModeToggle{nullptr};
    FluentToggleSwitch* m_animationsToggle{nullptr};
    FluentToggleSwitch* m_soundsToggle{nullptr};
    FluentToggleSwitch* m_notificationsToggle{nullptr};
    FluentToggleSwitch* m_soundNotificationsToggle{nullptr};
    FluentToggleSwitch* m_badgeToggle{nullptr};
};
```

### Feature Toggle Dashboard

```cpp
class FeatureToggleDashboard : public QWidget
{
public:
    FeatureToggleDashboard(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        loadFeatureStates();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Header
        auto* headerLabel = new QLabel("Feature Toggles");
        headerLabel->setFont(QFont("Segoe UI", 16, QFont::Bold));
        layout->addWidget(headerLabel);
        
        auto* descLabel = new QLabel("Enable or disable experimental features");
        descLabel->setStyleSheet("color: #666;");
        layout->addWidget(descLabel);
        
        layout->addSpacing(20);
        
        // Feature toggles
        createFeatureToggle("New UI Design", "Enable the redesigned user interface", 
                          "This is an experimental feature that may have issues", false);
        
        createFeatureToggle("Advanced Search", "Enable advanced search capabilities",
                          "Includes filters, sorting, and saved searches", true);
        
        createFeatureToggle("Real-time Collaboration", "Enable real-time collaborative editing",
                          "Allows multiple users to edit documents simultaneously", false);
        
        createFeatureToggle("AI Assistant", "Enable AI-powered assistance features",
                          "Provides smart suggestions and automated tasks", false);
        
        createFeatureToggle("Performance Mode", "Enable performance optimizations",
                          "May reduce visual effects to improve performance", true);
        
        layout->addStretch();
        
        // Reset button
        auto* resetButton = new FluentButton("Reset to Defaults");
        connect(resetButton, &FluentButton::clicked, this, &FeatureToggleDashboard::resetToDefaults);
        layout->addWidget(resetButton);
    }
    
    void createFeatureToggle(const QString& title, const QString& description, 
                           const QString& detail, bool defaultValue) {
        auto* toggle = new FluentToggleSwitch(title);
        toggle->setDescription(description);
        toggle->setDetailText(detail);
        toggle->setContentLayout(FluentToggleContentLayout::Vertical);
        toggle->setChecked(defaultValue);
        
        // Add warning for experimental features
        if (title.contains("experimental") || title.contains("New UI") || title.contains("AI")) {
            toggle->setValidationState(FluentValidationState::Warning);
            toggle->setValidationMessage("Experimental feature - use with caution");
        }
        
        connect(toggle, &FluentToggleSwitch::toggled, [=](bool checked) {
            saveFeatureState(title, checked);
            emit featureToggled(title, checked);
        });
        
        layout()->addWidget(toggle);
        m_featureToggles[title] = toggle;
    }
    
    void loadFeatureStates() {
        QSettings settings;
        settings.beginGroup("features");
        
        for (auto it = m_featureToggles.begin(); it != m_featureToggles.end(); ++it) {
            bool state = settings.value(it.key(), it.value()->isChecked()).toBool();
            it.value()->setChecked(state);
        }
        
        settings.endGroup();
    }
    
    void saveFeatureState(const QString& feature, bool enabled) {
        QSettings settings;
        settings.setValue(QString("features/%1").arg(feature), enabled);
    }
    
    void resetToDefaults() {
        QSettings settings;
        settings.remove("features");
        
        // Reset to default values (defined in createFeatureToggle)
        m_featureToggles["New UI Design"]->setChecked(false);
        m_featureToggles["Advanced Search"]->setChecked(true);
        m_featureToggles["Real-time Collaboration"]->setChecked(false);
        m_featureToggles["AI Assistant"]->setChecked(false);
        m_featureToggles["Performance Mode"]->setChecked(true);
    }

signals:
    void featureToggled(const QString& feature, bool enabled);

private:
    QMap<QString, FluentToggleSwitch*> m_featureToggles;
};
```

## Accessibility

FluentToggleSwitch provides comprehensive accessibility support:

```cpp
auto* toggleSwitch = new FluentToggleSwitch("Accessible Toggle");

// Accessible properties (automatically set)
toggleSwitch->setAccessibleName("Enable feature");
toggleSwitch->setAccessibleDescription("Toggle to enable or disable this feature");

// Keyboard navigation (automatic)
// - Tab to focus toggle switch
// - Space/Enter to toggle state
// - Arrow keys for navigation

// Screen reader support
toggleSwitch->setAccessibleRole(QAccessible::CheckBox);

// State announcements
connect(toggleSwitch, &FluentToggleSwitch::toggled, [=](bool checked) {
    QString announcement = checked ? "Enabled" : "Disabled";
    toggleSwitch->announceToScreenReader(announcement);
});
```

## Best Practices

### Toggle Switch Design Guidelines
1. **Use for binary choices** - On/off, enable/disable states
2. **Provide clear labels** - Make the purpose obvious
3. **Show immediate effect** - Apply changes instantly when possible
4. **Use consistent styling** - Maintain visual consistency
5. **Support keyboard navigation** - Ensure accessibility

### Performance Considerations
- Use efficient animation algorithms
- Cache styled elements when possible
- Optimize state change handling
- Implement smooth toggle transitions

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

## See Also

- [FluentCheckBox](FluentCheckBox.md) - For multiple selection options
- [FluentRadioButton](FluentRadioButton.md) - For exclusive selection
- [FluentButton](FluentButton.md) - For action buttons
- [API Reference - FluentToggleSwitch](../API_REFERENCE.md#fluenttoggleswitch) - Complete API documentation
