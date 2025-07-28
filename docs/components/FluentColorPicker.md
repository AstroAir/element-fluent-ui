# FluentColorPicker Component

The `FluentColorPicker` component is a comprehensive color selection widget that implements Microsoft's Fluent Design principles. It provides multiple color selection interfaces with smooth animations and extensive customization options.

## Overview

FluentColorPicker offers:
- **Multiple picker modes** (Palette, Wheel, Sliders, Compact, Full)
- **Color format support** (RGB, HSV, HSL, Hex, Named)
- **Custom color palettes** with save/load functionality
- **Recent colors** tracking
- **Smooth animations** for mode transitions
- **Theme integration** with automatic styling
- **Accessibility support** with keyboard navigation

## Basic Usage

### Simple Color Picker

```cpp
#include "FluentQt/Components/FluentColorPicker.h"

// Create a basic color picker
auto* colorPicker = new FluentColorPicker;
colorPicker->setCurrentColor(QColor(255, 0, 0));  // Start with red

// Handle color changes
connect(colorPicker, &FluentColorPicker::colorChanged, [=](const QColor& color) {
    qDebug() << "Selected color:" << color.name();
});

// Add to layout
layout->addWidget(colorPicker);
```

### Color Picker with Label

```cpp
auto* colorPicker = new FluentColorPicker;
colorPicker->setLabelText("Background Color");
colorPicker->setCurrentColor(QColor(240, 240, 240));
colorPicker->setShowColorName(true);  // Show color name/hex value

// Show color preview
colorPicker->setShowPreview(true);
colorPicker->setPreviewSize(QSize(40, 40));
```

## Picker Modes

### Palette Mode

```cpp
auto* colorPicker = new FluentColorPicker;
colorPicker->setPickerMode(FluentColorPickerMode::Palette);

// Use predefined palette
colorPicker->setPalette(FluentColorPalette::Material);
colorPicker->setPalette(FluentColorPalette::Windows);
colorPicker->setPalette(FluentColorPalette::Web);

// Custom palette
QList<QColor> customColors = {
    QColor(255, 0, 0),    // Red
    QColor(0, 255, 0),    // Green
    QColor(0, 0, 255),    // Blue
    QColor(255, 255, 0),  // Yellow
    QColor(255, 0, 255),  // Magenta
    QColor(0, 255, 255)   // Cyan
};
colorPicker->setCustomPalette(customColors);

// Configure palette layout
colorPicker->setPaletteColumns(8);
colorPicker->setPaletteColorSize(QSize(24, 24));
colorPicker->setPaletteSpacing(2);
```

### Color Wheel Mode

```cpp
auto* colorPicker = new FluentColorPicker;
colorPicker->setPickerMode(FluentColorPickerMode::Wheel);

// Configure wheel appearance
colorPicker->setWheelSize(200);
colorPicker->setWheelThickness(20);
colorPicker->setShowWheelCenter(true);  // Show brightness/saturation square

// Wheel interaction
colorPicker->setWheelContinuous(true);  // Continuous color updates while dragging
```

### Slider Mode

```cpp
auto* colorPicker = new FluentColorPicker;
colorPicker->setPickerMode(FluentColorPickerMode::Sliders);

// Choose color model for sliders
colorPicker->setSliderColorModel(FluentColorModel::RGB);
colorPicker->setSliderColorModel(FluentColorModel::HSV);
colorPicker->setSliderColorModel(FluentColorModel::HSL);

// Configure slider appearance
colorPicker->setSliderHeight(20);
colorPicker->setSliderSpacing(8);
colorPicker->setShowSliderLabels(true);
colorPicker->setShowSliderValues(true);
```

### Compact Mode

```cpp
auto* colorPicker = new FluentColorPicker;
colorPicker->setPickerMode(FluentColorPickerMode::Compact);

// Compact mode shows a button that opens a popup
colorPicker->setCompactButtonSize(QSize(32, 32));
colorPicker->setCompactShowText(true);  // Show color name on button

// Configure popup content
colorPicker->setCompactPopupMode(FluentColorPickerMode::Wheel);
colorPicker->setCompactPopupSize(QSize(300, 250));
```

### Full Mode

```cpp
auto* colorPicker = new FluentColorPicker;
colorPicker->setPickerMode(FluentColorPickerMode::Full);

// Full mode combines multiple interfaces
colorPicker->setFullModeLayout(FluentFullModeLayout::Tabbed);
colorPicker->setFullModeLayout(FluentFullModeLayout::Stacked);

// Configure which modes to include
colorPicker->setEnabledModes({
    FluentColorPickerMode::Palette,
    FluentColorPickerMode::Wheel,
    FluentColorPickerMode::Sliders
});
```

## Color Formats

### Different Color Models

```cpp
auto* colorPicker = new FluentColorPicker;

// Set color using different formats
colorPicker->setCurrentColor(QColor(255, 128, 64));           // RGB
colorPicker->setCurrentColorHsv(30, 75, 100);                // HSV
colorPicker->setCurrentColorHsl(30, 100, 62);                // HSL
colorPicker->setCurrentColorHex("#FF8040");                   // Hex
colorPicker->setCurrentColorName("orange");                   // Named color

// Get color in different formats
QColor rgbColor = colorPicker->currentColor();
QString hexColor = colorPicker->currentColorHex();
QString namedColor = colorPicker->currentColorName();
QVector3D hsvColor = colorPicker->currentColorHsv();
QVector3D hslColor = colorPicker->currentColorHsl();
```

### Color Format Display

```cpp
auto* colorPicker = new FluentColorPicker;

// Show multiple format displays
colorPicker->setShowHexInput(true);
colorPicker->setShowRgbInputs(true);
colorPicker->setShowHsvInputs(true);
colorPicker->setShowHslInputs(true);

// Format input validation
colorPicker->setHexInputValidation(true);
colorPicker->setRgbInputRange(0, 255);
colorPicker->setHsvInputRange(0, 360, 0, 100, 0, 100);  // H, S, V ranges

// Handle format changes
connect(colorPicker, &FluentColorPicker::hexChanged, [=](const QString& hex) {
    qDebug() << "Hex color:" << hex;
});
```

## Recent Colors and History

### Recent Colors Tracking

```cpp
auto* colorPicker = new FluentColorPicker;

// Enable recent colors
colorPicker->setRecentColorsEnabled(true);
colorPicker->setMaxRecentColors(12);
colorPicker->setRecentColorsColumns(6);

// Add colors to recent list
colorPicker->addRecentColor(QColor(255, 0, 0));
colorPicker->addRecentColor(QColor(0, 255, 0));

// Get recent colors
QList<QColor> recentColors = colorPicker->recentColors();

// Clear recent colors
colorPicker->clearRecentColors();

// Handle recent color selection
connect(colorPicker, &FluentColorPicker::recentColorSelected, [=](const QColor& color) {
    qDebug() << "Selected recent color:" << color.name();
});
```

### Color History

```cpp
auto* colorPicker = new FluentColorPicker;

// Enable color history with undo/redo
colorPicker->setHistoryEnabled(true);
colorPicker->setMaxHistorySize(50);

// Navigation
colorPicker->undo();  // Go back to previous color
colorPicker->redo();  // Go forward in history

// Check history state
bool canUndo = colorPicker->canUndo();
bool canRedo = colorPicker->canRedo();

// Get history
QList<QColor> colorHistory = colorPicker->colorHistory();
int currentHistoryIndex = colorPicker->historyIndex();
```

## Custom Palettes

### Creating Custom Palettes

```cpp
auto* colorPicker = new FluentColorPicker;

// Create theme-based palette
QList<QColor> themePalette = {
    QColor("#0078D4"),  // Primary blue
    QColor("#106EBE"),  // Dark blue
    QColor("#005A9E"),  // Darker blue
    QColor("#004578"),  // Darkest blue
    QColor("#40E0D0"),  // Turquoise
    QColor("#00BCF2"),  // Light blue
    QColor("#00B7C3"),  // Teal
    QColor("#018574")   // Dark teal
};

colorPicker->addCustomPalette("Theme Colors", themePalette);

// Material Design palette
QList<QColor> materialPalette = {
    QColor("#F44336"), QColor("#E91E63"), QColor("#9C27B0"), QColor("#673AB7"),
    QColor("#3F51B5"), QColor("#2196F3"), QColor("#03A9F4"), QColor("#00BCD4"),
    QColor("#009688"), QColor("#4CAF50"), QColor("#8BC34A"), QColor("#CDDC39"),
    QColor("#FFEB3B"), QColor("#FFC107"), QColor("#FF9800"), QColor("#FF5722")
};

colorPicker->addCustomPalette("Material Design", materialPalette);

// Save/load palettes
colorPicker->savePalette("MyPalette", themePalette);
QList<QColor> loadedPalette = colorPicker->loadPalette("MyPalette");
```

### Palette Management

```cpp
auto* colorPicker = new FluentColorPicker;

// Enable palette editing
colorPicker->setPaletteEditable(true);

// Handle palette modifications
connect(colorPicker, &FluentColorPicker::paletteColorAdded, [=](const QColor& color) {
    qDebug() << "Color added to palette:" << color.name();
});

connect(colorPicker, &FluentColorPicker::paletteColorRemoved, [=](const QColor& color) {
    qDebug() << "Color removed from palette:" << color.name();
});

// Palette context menu
connect(colorPicker, &FluentColorPicker::paletteContextMenuRequested, [=](const QPoint& position, const QColor& color) {
    auto* menu = new QMenu(this);
    
    menu->addAction("Add to Favorites", [=]() {
        colorPicker->addFavoriteColor(color);
    });
    
    menu->addAction("Remove from Palette", [=]() {
        colorPicker->removePaletteColor(color);
    });
    
    menu->addSeparator();
    menu->addAction("Copy Hex", [=]() {
        QApplication::clipboard()->setText(color.name());
    });
    
    menu->exec(colorPicker->mapToGlobal(position));
});
```

## Animations

### Color Transition Animations

```cpp
auto* colorPicker = new FluentColorPicker;

// Enable smooth color transitions
colorPicker->setAnimated(true);
colorPicker->setColorTransitionDuration(200);
colorPicker->setColorTransitionEasing(FluentEasing::CubicOut);

// Animate to specific color
colorPicker->animateToColor(QColor(255, 0, 0));

// Custom transition effects
connect(colorPicker, &FluentColorPicker::colorChanging, [=](const QColor& from, const QColor& to) {
    auto* animator = new FluentAnimator(this);
    
    // Animate preview background
    QWidget* preview = colorPicker->previewWidget();
    if (preview) {
        animator->colorTransition(preview, "background-color", from, to, 300);
    }
});
```

### Mode Transition Animations

```cpp
auto* colorPicker = new FluentColorPicker;

// Enable mode change animations
colorPicker->setModeTransitionAnimated(true);
colorPicker->setModeTransitionDuration(300);

// Custom mode transitions
connect(colorPicker, &FluentColorPicker::modeChanging, [=](FluentColorPickerMode from, FluentColorPickerMode to) {
    auto* animator = new FluentAnimator(this);
    
    QWidget* fromWidget = colorPicker->modeWidget(from);
    QWidget* toWidget = colorPicker->modeWidget(to);
    
    if (fromWidget && toWidget) {
        // Slide transition
        animator->slideOutToLeft(fromWidget, 200);
        animator->slideInFromRight(toWidget, 200);
    }
});
```

## Integration Examples

### Theme Color Selector

```cpp
class ThemeColorSelector : public QWidget
{
public:
    ThemeColorSelector(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupConnections();
        loadThemeColors();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Title
        auto* titleLabel = new QLabel("Theme Colors");
        titleLabel->setFont(QFont("Segoe UI", 12, QFont::Bold));
        
        // Primary color picker
        m_primaryColorPicker = new FluentColorPicker;
        m_primaryColorPicker->setLabelText("Primary Color");
        m_primaryColorPicker->setPickerMode(FluentColorPickerMode::Compact);
        m_primaryColorPicker->setCompactPopupMode(FluentColorPickerMode::Wheel);
        
        // Secondary color picker
        m_secondaryColorPicker = new FluentColorPicker;
        m_secondaryColorPicker->setLabelText("Secondary Color");
        m_secondaryColorPicker->setPickerMode(FluentColorPickerMode::Compact);
        
        // Accent color picker
        m_accentColorPicker = new FluentColorPicker;
        m_accentColorPicker->setLabelText("Accent Color");
        m_accentColorPicker->setPickerMode(FluentColorPickerMode::Palette);
        
        // Create accent color palette
        QList<QColor> accentColors = {
            QColor("#0078D4"), QColor("#005A9E"), QColor("#004578"),
            QColor("#40E0D0"), QColor("#00BCF2"), QColor("#00B7C3"),
            QColor("#F44336"), QColor("#E91E63"), QColor("#9C27B0"),
            QColor("#4CAF50"), QColor("#8BC34A"), QColor("#FFC107")
        };
        m_accentColorPicker->setCustomPalette(accentColors);
        
        // Preview panel
        m_previewPanel = new FluentPanel("Theme Preview");
        m_previewPanel->setFixedHeight(100);
        
        // Buttons
        auto* buttonLayout = new QHBoxLayout;
        auto* applyButton = FluentButton::createPrimaryButton("Apply Theme");
        auto* resetButton = new FluentButton("Reset to Default");
        
        buttonLayout->addStretch();
        buttonLayout->addWidget(resetButton);
        buttonLayout->addWidget(applyButton);
        
        layout->addWidget(titleLabel);
        layout->addWidget(m_primaryColorPicker);
        layout->addWidget(m_secondaryColorPicker);
        layout->addWidget(m_accentColorPicker);
        layout->addWidget(m_previewPanel);
        layout->addLayout(buttonLayout);
        
        connect(applyButton, &FluentButton::clicked, this, &ThemeColorSelector::applyTheme);
        connect(resetButton, &FluentButton::clicked, this, &ThemeColorSelector::resetTheme);
    }
    
    void setupConnections() {
        // Update preview when colors change
        connect(m_primaryColorPicker, &FluentColorPicker::colorChanged, this, &ThemeColorSelector::updatePreview);
        connect(m_secondaryColorPicker, &FluentColorPicker::colorChanged, this, &ThemeColorSelector::updatePreview);
        connect(m_accentColorPicker, &FluentColorPicker::colorChanged, this, &ThemeColorSelector::updatePreview);
    }
    
    void updatePreview() {
        QColor primary = m_primaryColorPicker->currentColor();
        QColor secondary = m_secondaryColorPicker->currentColor();
        QColor accent = m_accentColorPicker->currentColor();
        
        // Update preview panel styling
        QString style = QString(R"(
            FluentPanel {
                background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                    stop: 0 %1, stop: 0.5 %2, stop: 1 %3);
                border: 2px solid %3;
                border-radius: 8px;
            }
        )").arg(primary.name(), secondary.name(), accent.name());
        
        m_previewPanel->setStyleSheet(style);
    }
    
    void applyTheme() {
        QColor primary = m_primaryColorPicker->currentColor();
        QColor secondary = m_secondaryColorPicker->currentColor();
        QColor accent = m_accentColorPicker->currentColor();
        
        // Apply to theme system
        auto& theme = FluentTheme::instance();
        theme.setColor("primary", primary);
        theme.setColor("secondary", secondary);
        theme.setColor("accent", accent);
        
        emit themeChanged(primary, secondary, accent);
    }

signals:
    void themeChanged(const QColor& primary, const QColor& secondary, const QColor& accent);

private:
    FluentColorPicker* m_primaryColorPicker{nullptr};
    FluentColorPicker* m_secondaryColorPicker{nullptr};
    FluentColorPicker* m_accentColorPicker{nullptr};
    FluentPanel* m_previewPanel{nullptr};
};
```

### Drawing Application Color Palette

```cpp
class DrawingColorPalette : public QWidget
{
public:
    DrawingColorPalette(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupConnections();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Current colors display
        auto* currentColorsLayout = new QHBoxLayout;
        
        m_primaryColorDisplay = new QLabel;
        m_primaryColorDisplay->setFixedSize(40, 40);
        m_primaryColorDisplay->setStyleSheet("border: 2px solid #ccc; background: black;");
        m_primaryColorDisplay->setToolTip("Primary Color (Left Click)");
        
        m_secondaryColorDisplay = new QLabel;
        m_secondaryColorDisplay->setFixedSize(30, 30);
        m_secondaryColorDisplay->setStyleSheet("border: 2px solid #ccc; background: white;");
        m_secondaryColorDisplay->setToolTip("Secondary Color (Right Click)");
        
        currentColorsLayout->addWidget(m_primaryColorDisplay);
        currentColorsLayout->addWidget(m_secondaryColorDisplay);
        currentColorsLayout->addStretch();
        
        // Color picker
        m_colorPicker = new FluentColorPicker;
        m_colorPicker->setPickerMode(FluentColorPickerMode::Full);
        m_colorPicker->setEnabledModes({
            FluentColorPickerMode::Palette,
            FluentColorPickerMode::Wheel,
            FluentColorPickerMode::Sliders
        });
        
        // Quick palette
        m_quickPalette = new FluentColorPicker;
        m_quickPalette->setPickerMode(FluentColorPickerMode::Palette);
        m_quickPalette->setPaletteColumns(8);
        m_quickPalette->setPaletteColorSize(QSize(20, 20));
        
        // Create drawing palette
        QList<QColor> drawingColors = {
            Qt::black, Qt::white, Qt::red, Qt::green, Qt::blue, Qt::yellow, Qt::magenta, Qt::cyan,
            QColor(128, 0, 0), QColor(0, 128, 0), QColor(0, 0, 128), QColor(128, 128, 0),
            QColor(128, 0, 128), QColor(0, 128, 128), QColor(128, 128, 128), QColor(64, 64, 64)
        };
        m_quickPalette->setCustomPalette(drawingColors);
        
        layout->addLayout(currentColorsLayout);
        layout->addWidget(new QLabel("Quick Colors:"));
        layout->addWidget(m_quickPalette);
        layout->addWidget(new QLabel("Color Picker:"));
        layout->addWidget(m_colorPicker);
    }
    
    void setupConnections() {
        // Update primary color from main picker
        connect(m_colorPicker, &FluentColorPicker::colorChanged, [=](const QColor& color) {
            setPrimaryColor(color);
        });
        
        // Update primary color from quick palette
        connect(m_quickPalette, &FluentColorPicker::colorChanged, [=](const QColor& color) {
            setPrimaryColor(color);
            m_colorPicker->setCurrentColor(color);
        });
        
        // Handle mouse clicks on color displays for swapping
        m_primaryColorDisplay->installEventFilter(this);
        m_secondaryColorDisplay->installEventFilter(this);
    }
    
    void setPrimaryColor(const QColor& color) {
        m_primaryColor = color;
        m_primaryColorDisplay->setStyleSheet(QString("border: 2px solid #ccc; background: %1;").arg(color.name()));
        emit primaryColorChanged(color);
    }
    
    void setSecondaryColor(const QColor& color) {
        m_secondaryColor = color;
        m_secondaryColorDisplay->setStyleSheet(QString("border: 2px solid #ccc; background: %1;").arg(color.name()));
        emit secondaryColorChanged(color);
    }
    
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (event->type() == QEvent::MouseButtonPress) {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            
            if (obj == m_primaryColorDisplay || obj == m_secondaryColorDisplay) {
                if (mouseEvent->button() == Qt::LeftButton) {
                    // Swap primary and secondary colors
                    QColor temp = m_primaryColor;
                    setPrimaryColor(m_secondaryColor);
                    setSecondaryColor(temp);
                    m_colorPicker->setCurrentColor(m_primaryColor);
                    return true;
                }
            }
        }
        
        return QWidget::eventFilter(obj, event);
    }

signals:
    void primaryColorChanged(const QColor& color);
    void secondaryColorChanged(const QColor& color);

private:
    FluentColorPicker* m_colorPicker{nullptr};
    FluentColorPicker* m_quickPalette{nullptr};
    QLabel* m_primaryColorDisplay{nullptr};
    QLabel* m_secondaryColorDisplay{nullptr};
    QColor m_primaryColor{Qt::black};
    QColor m_secondaryColor{Qt::white};
};
```

## Accessibility

FluentColorPicker provides comprehensive accessibility support:

```cpp
auto* colorPicker = new FluentColorPicker;

// Accessible properties (automatically set)
colorPicker->setAccessibleName("Color picker");
colorPicker->setAccessibleDescription("Select a color using various input methods");

// Keyboard navigation (automatic)
// - Tab to navigate between elements
// - Arrow keys for palette navigation
// - Enter/Space to select colors
// - Escape to close popups

// Screen reader support
colorPicker->setAccessibleRole(QAccessible::ColorChooser);

// Color announcements
connect(colorPicker, &FluentColorPicker::colorChanged, [=](const QColor& color) {
    QString colorName = getColorName(color);  // Convert to readable name
    QString announcement = QString("Selected color: %1").arg(colorName);
    colorPicker->announceToScreenReader(announcement);
});
```

## Best Practices

### Color Picker Design Guidelines
1. **Provide multiple input methods** - Accommodate different user preferences
2. **Show color previews** - Help users visualize their selection
3. **Include recent colors** - Speed up common selections
4. **Support keyboard navigation** - Ensure accessibility
5. **Use meaningful color names** - Help with color identification

### Performance Considerations
- Cache color calculations for smooth interactions
- Use efficient color space conversions
- Optimize palette rendering for large color sets
- Implement lazy loading for custom palettes

### Accessibility Guidelines
- Support keyboard navigation
- Provide color name announcements
- Use sufficient contrast for UI elements
- Include alternative selection methods
- Test with color vision deficiency simulators

## Signals Reference

```cpp
// Color change signals
void colorChanged(const QColor& color);
void colorSelected(const QColor& color);
void hexChanged(const QString& hex);

// Palette signals
void paletteColorSelected(const QColor& color);
void recentColorSelected(const QColor& color);
void paletteColorAdded(const QColor& color);
void paletteColorRemoved(const QColor& color);

// Mode change signals
void modeChanged(FluentColorPickerMode mode);
void modeChanging(FluentColorPickerMode from, FluentColorPickerMode to);

// Interaction signals
void colorHovered(const QColor& color);
void paletteContextMenuRequested(const QPoint& position, const QColor& color);
```

## See Also

- [FluentSlider](FluentSlider.md) - For RGB/HSV sliders
- [FluentButton](FluentButton.md) - For color selection buttons
- [FluentPanel](FluentPanel.md) - For organizing color picker layouts
- [API Reference - FluentColorPicker](../API_REFERENCE.md#fluentcolorpicker) - Complete API documentation
