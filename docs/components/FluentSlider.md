# FluentSlider Component

The `FluentSlider` component is an enhanced slider widget that implements Microsoft's Fluent Design principles. It provides smooth value selection with customizable appearance, animations, and comprehensive interaction features.

## Overview

FluentSlider offers:
- **Horizontal and vertical** orientations
- **Single and range** selection modes
- **Custom tick marks** and labels
- **Smooth animations** for value changes
- **Tooltip display** for current values
- **Theme integration** with accent colors
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Slider

```cpp
#include "FluentQt/Components/FluentSlider.h"

// Create a basic slider
auto* slider = new FluentSlider;
slider->setRange(0, 100);
slider->setValue(50);
slider->setOrientation(Qt::Horizontal);

// Handle value changes
connect(slider, &FluentSlider::valueChanged, [=](int value) {
    qDebug() << "Slider value:" << value;
});

// Add to layout
layout->addWidget(slider);
```

### Slider with Label

```cpp
auto* slider = new FluentSlider;
slider->setLabelText("Volume");
slider->setRange(0, 100);
slider->setValue(75);
slider->setShowValue(true);  // Show current value next to label

// Custom value format
slider->setValueFormat("%1%");  // Shows "75%"
slider->setValueFormat("%1 dB"); // Shows "75 dB"
```

## Orientations

### Horizontal Slider

```cpp
auto* horizontalSlider = new FluentSlider;
horizontalSlider->setOrientation(Qt::Horizontal);
horizontalSlider->setRange(0, 100);
horizontalSlider->setValue(25);

// Configure appearance
horizontalSlider->setTrackHeight(6);
horizontalSlider->setHandleSize(20);
horizontalSlider->setMinimumWidth(200);
```

### Vertical Slider

```cpp
auto* verticalSlider = new FluentSlider;
verticalSlider->setOrientation(Qt::Vertical);
verticalSlider->setRange(0, 100);
verticalSlider->setValue(75);

// Configure appearance
verticalSlider->setTrackWidth(6);
verticalSlider->setHandleSize(20);
verticalSlider->setMinimumHeight(200);

// Invert direction (higher values at bottom)
verticalSlider->setInvertedAppearance(true);
```

## Range Slider

### Dual Handle Slider

```cpp
auto* rangeSlider = new FluentSlider;
rangeSlider->setSliderMode(FluentSliderMode::Range);
rangeSlider->setRange(0, 100);

// Set range values
rangeSlider->setLowerValue(25);
rangeSlider->setUpperValue(75);

// Handle range changes
connect(rangeSlider, &FluentSlider::rangeChanged, [=](int lower, int upper) {
    qDebug() << "Range:" << lower << "to" << upper;
});

// Get range values
QPair<int, int> range = rangeSlider->getRange();
int lowerValue = rangeSlider->lowerValue();
int upperValue = rangeSlider->upperValue();
```

### Range Constraints

```cpp
auto* rangeSlider = new FluentSlider;
rangeSlider->setSliderMode(FluentSliderMode::Range);
rangeSlider->setRange(0, 100);

// Set minimum range span
rangeSlider->setMinimumRange(10);  // Handles must be at least 10 units apart

// Set maximum range span
rangeSlider->setMaximumRange(50);  // Handles can't be more than 50 units apart

// Prevent handle crossing
rangeSlider->setHandleCrossingEnabled(false);
```

## Tick Marks and Labels

### Tick Marks

```cpp
auto* slider = new FluentSlider;
slider->setRange(0, 100);

// Enable tick marks
slider->setTickPosition(FluentSliderTicks::Both);  // Above and below
slider->setTickPosition(FluentSliderTicks::Above); // Above only
slider->setTickPosition(FluentSliderTicks::Below); // Below only

// Configure tick spacing
slider->setTickInterval(10);  // Tick every 10 units
slider->setMinorTickInterval(5);  // Minor ticks every 5 units

// Custom tick marks
slider->addTickMark(25, FluentTickType::Major);
slider->addTickMark(50, FluentTickType::Major);
slider->addTickMark(75, FluentTickType::Major);
```

### Custom Labels

```cpp
auto* slider = new FluentSlider;
slider->setRange(0, 100);

// Enable labels
slider->setShowLabels(true);
slider->setLabelPosition(FluentSliderLabels::Below);

// Custom label text
slider->setLabelText(0, "Min");
slider->setLabelText(50, "Medium");
slider->setLabelText(100, "Max");

// Custom label format function
slider->setLabelFormatter([](int value) -> QString {
    if (value == 0) return "Off";
    if (value <= 25) return "Low";
    if (value <= 50) return "Medium";
    if (value <= 75) return "High";
    return "Maximum";
});
```

## Styling and Theming

### Color Customization

```cpp
auto* slider = new FluentSlider;

// Use theme accent color (default)
slider->setUseAccentColor(true);

// Custom colors
slider->setTrackColor(QColor(200, 200, 200));           // Inactive track
slider->setActiveTrackColor(QColor(0, 120, 215));       // Active track
slider->setHandleColor(QColor(0, 120, 215));            // Handle color
slider->setHandleBorderColor(QColor(255, 255, 255));    // Handle border

// Hover and pressed states
slider->setHandleHoverColor(QColor(0, 100, 195));
slider->setHandlePressedColor(QColor(0, 80, 175));
```

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* slider = new FluentSlider;

// Automatic theme integration
auto& theme = FluentTheme::instance();
slider->setActiveTrackColor(theme.color("accent"));
slider->setHandleColor(theme.color("accent"));

// Respond to theme changes
connect(&theme, &FluentTheme::themeChanged, [=]() {
    slider->setActiveTrackColor(theme.color("accent"));
    slider->setHandleColor(theme.color("accent"));
});
```

## Animations

### Smooth Value Changes

```cpp
auto* slider = new FluentSlider;

// Enable smooth animations
slider->setAnimated(true);
slider->setAnimationDuration(200);
slider->setAnimationEasing(FluentEasing::CubicOut);

// Animate to specific value
slider->animateToValue(75);

// Animate range values
slider->animateToRange(25, 75);

// Handle animation completion
connect(slider, &FluentSlider::animationFinished, [=]() {
    qDebug() << "Animation completed";
});
```

### Hover Effects

```cpp
auto* slider = new FluentSlider;

// Enable hover animations
slider->setHoverAnimationEnabled(true);
slider->setHoverAnimationDuration(150);

// Custom hover effects
connect(slider, &FluentSlider::hovered, [=](bool isHovered) {
    auto* animator = new FluentAnimator(this);
    
    if (isHovered) {
        // Scale up handle on hover
        animator->scaleEffect(slider->handle(), 1.2, 150);
    } else {
        // Scale back to normal
        animator->scaleEffect(slider->handle(), 1.0, 150);
    }
});
```

## Tooltips and Value Display

### Value Tooltips

```cpp
auto* slider = new FluentSlider;

// Enable value tooltip
slider->setShowTooltip(true);
slider->setTooltipPosition(FluentTooltipPosition::Above);

// Custom tooltip format
slider->setTooltipFormat("Value: %1");
slider->setTooltipFormat("%1%");
slider->setTooltipFormat("%1 / 100");

// Custom tooltip content
slider->setTooltipFormatter([](int value) -> QString {
    return QString("Level %1").arg(value);
});

// Tooltip appearance
slider->setTooltipDelay(500);  // Show after 500ms hover
slider->setTooltipDuration(3000);  // Hide after 3 seconds
```

### Value Display

```cpp
auto* slider = new FluentSlider;

// Show value in different positions
slider->setShowValue(true);
slider->setValuePosition(FluentValuePosition::Right);
slider->setValuePosition(FluentValuePosition::Left);
slider->setValuePosition(FluentValuePosition::Above);
slider->setValuePosition(FluentValuePosition::Below);

// Custom value display
auto* valueLabel = new QLabel;
connect(slider, &FluentSlider::valueChanged, [=](int value) {
    valueLabel->setText(QString("Current: %1").arg(value));
});
```

## Specialized Sliders

### Volume Slider

```cpp
auto* volumeSlider = new FluentSlider;
volumeSlider->setLabelText("Volume");
volumeSlider->setRange(0, 100);
volumeSlider->setValue(50);
volumeSlider->setShowValue(true);
volumeSlider->setValueFormat("%1%");

// Add volume icon
auto* volumeIcon = new QLabel;
volumeIcon->setPixmap(QIcon(":/icons/volume").pixmap(16, 16));

auto* layout = new QHBoxLayout;
layout->addWidget(volumeIcon);
layout->addWidget(volumeSlider);

// Update icon based on volume level
connect(volumeSlider, &FluentSlider::valueChanged, [=](int value) {
    QIcon icon;
    if (value == 0) {
        icon = QIcon(":/icons/volume-mute");
    } else if (value < 30) {
        icon = QIcon(":/icons/volume-low");
    } else if (value < 70) {
        icon = QIcon(":/icons/volume-medium");
    } else {
        icon = QIcon(":/icons/volume-high");
    }
    volumeIcon->setPixmap(icon.pixmap(16, 16));
});
```

### Color Picker Sliders

```cpp
class ColorPickerWidget : public QWidget
{
public:
    ColorPickerWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupConnections();
        updateColor();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Red slider
        m_redSlider = new FluentSlider;
        m_redSlider->setLabelText("Red");
        m_redSlider->setRange(0, 255);
        m_redSlider->setValue(255);
        m_redSlider->setActiveTrackColor(QColor(255, 0, 0));
        
        // Green slider
        m_greenSlider = new FluentSlider;
        m_greenSlider->setLabelText("Green");
        m_greenSlider->setRange(0, 255);
        m_greenSlider->setValue(0);
        m_greenSlider->setActiveTrackColor(QColor(0, 255, 0));
        
        // Blue slider
        m_blueSlider = new FluentSlider;
        m_blueSlider->setLabelText("Blue");
        m_blueSlider->setRange(0, 255);
        m_blueSlider->setValue(0);
        m_blueSlider->setActiveTrackColor(QColor(0, 0, 255));
        
        // Color preview
        m_colorPreview = new QLabel;
        m_colorPreview->setFixedSize(100, 50);
        m_colorPreview->setStyleSheet("border: 1px solid #ccc;");
        
        layout->addWidget(m_redSlider);
        layout->addWidget(m_greenSlider);
        layout->addWidget(m_blueSlider);
        layout->addWidget(m_colorPreview);
    }
    
    void setupConnections() {
        connect(m_redSlider, &FluentSlider::valueChanged, this, &ColorPickerWidget::updateColor);
        connect(m_greenSlider, &FluentSlider::valueChanged, this, &ColorPickerWidget::updateColor);
        connect(m_blueSlider, &FluentSlider::valueChanged, this, &ColorPickerWidget::updateColor);
    }
    
    void updateColor() {
        int red = m_redSlider->value();
        int green = m_greenSlider->value();
        int blue = m_blueSlider->value();
        
        QColor color(red, green, blue);
        m_colorPreview->setStyleSheet(QString("background-color: %1; border: 1px solid #ccc;").arg(color.name()));
        
        emit colorChanged(color);
    }

signals:
    void colorChanged(const QColor& color);

private:
    FluentSlider* m_redSlider{nullptr};
    FluentSlider* m_greenSlider{nullptr};
    FluentSlider* m_blueSlider{nullptr};
    QLabel* m_colorPreview{nullptr};
};
```

### Progress Slider

```cpp
auto* progressSlider = new FluentSlider;
progressSlider->setLabelText("Progress");
progressSlider->setRange(0, 100);
progressSlider->setValue(0);
progressSlider->setEnabled(false);  // Read-only progress display

// Simulate progress
auto* timer = new QTimer(this);
connect(timer, &QTimer::timeout, [=]() {
    int currentValue = progressSlider->value();
    if (currentValue < 100) {
        progressSlider->animateToValue(currentValue + 1);
    } else {
        timer->stop();
        // Progress complete
        progressSlider->setActiveTrackColor(QColor(16, 124, 16));  // Green
    }
});
timer->start(100);  // Update every 100ms
```

## Form Integration

### Settings Panel with Sliders

```cpp
class SettingsPanel : public FluentPanel
{
public:
    SettingsPanel(QWidget* parent = nullptr) : FluentPanel("Audio Settings", parent) {
        setupUI();
        loadSettings();
        setupConnections();
    }

private:
    void setupUI() {
        auto* layout = createVerticalLayout();
        
        // Master volume
        m_masterVolumeSlider = new FluentSlider;
        m_masterVolumeSlider->setLabelText("Master Volume");
        m_masterVolumeSlider->setRange(0, 100);
        m_masterVolumeSlider->setShowValue(true);
        m_masterVolumeSlider->setValueFormat("%1%");
        
        // Music volume
        m_musicVolumeSlider = new FluentSlider;
        m_musicVolumeSlider->setLabelText("Music Volume");
        m_musicVolumeSlider->setRange(0, 100);
        m_musicVolumeSlider->setShowValue(true);
        m_musicVolumeSlider->setValueFormat("%1%");
        
        // Effects volume
        m_effectsVolumeSlider = new FluentSlider;
        m_effectsVolumeSlider->setLabelText("Effects Volume");
        m_effectsVolumeSlider->setRange(0, 100);
        m_effectsVolumeSlider->setShowValue(true);
        m_effectsVolumeSlider->setValueFormat("%1%");
        
        // Bass/Treble range slider
        m_eqSlider = new FluentSlider;
        m_eqSlider->setLabelText("EQ Range");
        m_eqSlider->setSliderMode(FluentSliderMode::Range);
        m_eqSlider->setRange(-10, 10);
        m_eqSlider->setLowerValue(-3);
        m_eqSlider->setUpperValue(3);
        m_eqSlider->setTickPosition(FluentSliderTicks::Below);
        m_eqSlider->setTickInterval(5);
        m_eqSlider->setShowLabels(true);
        
        layout->addWidget(m_masterVolumeSlider);
        layout->addWidget(m_musicVolumeSlider);
        layout->addWidget(m_effectsVolumeSlider);
        layout->addWidget(m_eqSlider);
        
        // Reset button
        auto* resetButton = new FluentButton("Reset to Defaults");
        connect(resetButton, &FluentButton::clicked, this, &SettingsPanel::resetToDefaults);
        layout->addWidget(resetButton);
    }
    
    void setupConnections() {
        // Auto-save settings when values change
        connect(m_masterVolumeSlider, &FluentSlider::valueChanged, this, &SettingsPanel::saveSettings);
        connect(m_musicVolumeSlider, &FluentSlider::valueChanged, this, &SettingsPanel::saveSettings);
        connect(m_effectsVolumeSlider, &FluentSlider::valueChanged, this, &SettingsPanel::saveSettings);
        connect(m_eqSlider, &FluentSlider::rangeChanged, this, &SettingsPanel::saveSettings);
        
        // Apply settings to audio system
        connect(m_masterVolumeSlider, &FluentSlider::valueChanged, [=](int value) {
            AudioSystem::setMasterVolume(value / 100.0);
        });
        
        connect(m_musicVolumeSlider, &FluentSlider::valueChanged, [=](int value) {
            AudioSystem::setMusicVolume(value / 100.0);
        });
    }
    
    void loadSettings() {
        QSettings settings;
        m_masterVolumeSlider->setValue(settings.value("audio/masterVolume", 75).toInt());
        m_musicVolumeSlider->setValue(settings.value("audio/musicVolume", 50).toInt());
        m_effectsVolumeSlider->setValue(settings.value("audio/effectsVolume", 75).toInt());
        
        int eqLower = settings.value("audio/eqLower", -3).toInt();
        int eqUpper = settings.value("audio/eqUpper", 3).toInt();
        m_eqSlider->setRange(eqLower, eqUpper);
    }
    
    void saveSettings() {
        QSettings settings;
        settings.setValue("audio/masterVolume", m_masterVolumeSlider->value());
        settings.setValue("audio/musicVolume", m_musicVolumeSlider->value());
        settings.setValue("audio/effectsVolume", m_effectsVolumeSlider->value());
        settings.setValue("audio/eqLower", m_eqSlider->lowerValue());
        settings.setValue("audio/eqUpper", m_eqSlider->upperValue());
    }
    
    void resetToDefaults() {
        m_masterVolumeSlider->animateToValue(75);
        m_musicVolumeSlider->animateToValue(50);
        m_effectsVolumeSlider->animateToValue(75);
        m_eqSlider->animateToRange(-3, 3);
    }

private:
    FluentSlider* m_masterVolumeSlider{nullptr};
    FluentSlider* m_musicVolumeSlider{nullptr};
    FluentSlider* m_effectsVolumeSlider{nullptr};
    FluentSlider* m_eqSlider{nullptr};
};
```

## Accessibility

FluentSlider provides comprehensive accessibility support:

```cpp
auto* slider = new FluentSlider;

// Accessible properties (automatically set)
slider->setAccessibleName("Volume control");
slider->setAccessibleDescription("Adjust the volume level from 0 to 100");

// Keyboard navigation (automatic)
// - Arrow keys to change value
// - Page Up/Down for larger steps
// - Home/End for min/max values
// - Tab to focus slider

// Screen reader support
slider->setAccessibleRole(QAccessible::Slider);

// Value announcements
connect(slider, &FluentSlider::valueChanged, [=](int value) {
    QString announcement = QString("Volume %1 percent").arg(value);
    slider->announceToScreenReader(announcement);
});
```

## Best Practices

### Slider Design Guidelines
1. **Provide clear labels** - Describe what the slider controls
2. **Show current values** - Help users understand the setting
3. **Use appropriate ranges** - Choose meaningful min/max values
4. **Add tick marks** - For important reference points
5. **Enable keyboard navigation** - Ensure accessibility

### Performance Considerations
- Throttle rapid value changes for expensive operations
- Use appropriate animation durations
- Cache styled elements when possible
- Optimize custom drawing for smooth interaction

### Accessibility Guidelines
- Support keyboard navigation
- Provide value announcements
- Use sufficient color contrast
- Include meaningful labels and descriptions
- Test with assistive technologies

## Signals Reference

```cpp
// Value change signals
void valueChanged(int value);
void sliderPressed();
void sliderReleased();
void sliderMoved(int value);

// Range slider signals (FluentSliderMode::Range)
void rangeChanged(int lower, int upper);
void lowerValueChanged(int value);
void upperValueChanged(int value);

// Interaction signals
void hovered(bool isHovered);
void animationFinished();
```

## See Also

- [FluentProgressBar](FluentProgressBar.md) - For progress indication
- [FluentTextInput](FluentTextInput.md) - For numeric input
- [FluentButton](FluentButton.md) - For discrete value selection
- [API Reference - FluentSlider](../API_REFERENCE.md#fluentslider) - Complete API documentation
