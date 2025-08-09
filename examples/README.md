# FluentQt Unified Component Showcase

A comprehensive demonstration of all FluentQt components with complete theme switching functionality, responsive design, and accessibility features.

## Overview

The Unified Component Showcase serves as both a demonstration of FluentQt's capabilities and a reference implementation for developers integrating the library into their projects. It showcases:

- **Complete Component Library**: All available FluentQt components organized in logical sections
- **Advanced Theme Switching**: Seamless transitions between light, dark, and system themes
- **Responsive Design**: Adaptive layout that works across different screen sizes
- **Accessibility Features**: Full keyboard navigation and screen reader support
- **Interactive Demonstrations**: Real-world usage examples and component interactions
- **Theme Persistence**: Automatic saving and restoration of user preferences

## Features

### 🎨 Theme Management
- **Theme Modes**: Light, Dark, and System (follows OS preference)
- **Accent Colors**: Six built-in accent colors with smooth transitions
- **High Contrast Mode**: Enhanced accessibility support
- **Custom Opacity**: Adjustable window transparency
- **Theme Persistence**: Settings automatically saved and restored
- **Visual Feedback**: Smooth animations during theme changes

### 🧩 Component Categories

#### Basic Controls
- **FluentButton**: Primary, secondary, accent, and subtle button styles
- **FluentTextInput**: Text inputs with labels, validation, and helper text
- **FluentCheckBox**: Checkboxes with custom styling and animations
- **FluentRadioButton**: Radio buttons with fluent design
- **FluentToggleSwitch**: Modern toggle switches
- **FluentSlider**: Horizontal and vertical sliders with custom styling
- **FluentSpinBox**: Numeric input controls
- **FluentComboBox**: Dropdown selection controls

#### Layout & Navigation
- **FluentCard**: Elevated cards with headers, content, and actions
- **FluentPanel**: Organized content panels with titles
- **FluentBreadcrumb**: Navigation breadcrumbs
- **FluentSeparator**: Visual content separators
- **FluentNavigationView**: Sidebar navigation component

#### Data Display
- **FluentProgressBar**: Progress indicators with text and animations
- **FluentLoadingIndicator**: Animated loading spinners
- **FluentRating**: Star rating controls
- **FluentBadge**: Status badges and counters
- **FluentAvatar**: User avatar components
- **FluentTreeView**: Hierarchical data display

#### Feedback Components
- **FluentToast**: Toast notifications with multiple types
- **FluentNotification**: Rich notification components
- **FluentContentDialog**: Modal dialogs with custom content
- **FluentTooltip**: Smart tooltips with positioning

#### Advanced Components
- **FluentCalendar**: Date selection calendar
- **FluentDatePicker**: Date input controls
- **FluentTimePicker**: Time selection controls
- **FluentColorPicker**: Color selection interface
- **FluentCarousel**: Image and content carousels
- **FluentRichTextEditor**: Rich text editing capabilities

### 🔧 Interactive Demonstrations
- **Form Validation**: Real-time form validation examples
- **Component Interactions**: How components work together
- **Theme Integration**: Live theme switching demonstrations
- **Responsive Behavior**: Layout adaptation examples
- **Accessibility Features**: Keyboard navigation and screen reader support

## Building and Running

### Prerequisites
- Qt 6.5 or later
- CMake 3.20 or later
- C++20 compatible compiler
- FluentQt library

### Build Instructions

```bash
# Navigate to examples directory
cd examples

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the unified showcase
cmake --build . --target UnifiedComponentShowcase

# Run the application
./UnifiedComponentShowcase  # Linux/macOS
# or
UnifiedComponentShowcase.exe  # Windows
```

### Alternative Build (Qt Creator)
1. Open `examples/CMakeLists.txt` in Qt Creator
2. Configure the project with your Qt kit
3. Build and run the `UnifiedComponentShowcase` target

## Usage Guide

### Theme Controls
The theme control panel at the top provides:
- **Toggle Theme**: Switch between light and dark modes
- **Change Accent**: Cycle through available accent colors
- **High Contrast**: Enable/disable high contrast mode
- **Opacity Slider**: Adjust window transparency

### Navigation
- Use the tab bar to explore different component categories
- Each tab contains organized sections of related components
- Interactive elements demonstrate real-world usage patterns

### Keyboard Navigation
- **Tab**: Navigate between interactive elements
- **Enter/Space**: Activate buttons and controls
- **Arrow Keys**: Navigate within components
- **Escape**: Close dialogs and popups

### Settings Persistence
The application automatically saves:
- Theme mode preference
- Selected accent color
- High contrast setting
- Window size and position
- UI opacity setting

Settings are restored when the application restarts.

## Integration Examples

### Basic Theme Integration
```cpp
#include "FluentQt/Styling/FluentTheme.h"

// Get theme instance
auto& theme = FluentTheme::instance();

// Set theme mode
theme.setMode(FluentThemeMode::Dark);

// Listen for theme changes
connect(&theme, &FluentTheme::themeChanged, [=]() {
    updateApplicationColors();
});
```

### Component Usage
```cpp
#include "FluentQt/Components/FluentButton.h"

// Create a primary button
auto* button = new FluentButton("Click Me");
button->setButtonStyle(FluentButtonStyle::Primary);

// Connect to action
connect(button, &FluentButton::clicked, [=]() {
    // Handle button click
});
```

### Toast Notifications
```cpp
#include "FluentQt/Components/FluentToastManager.h"

// Show success toast
FluentToastManager::instance().showSuccess(
    "Success", "Operation completed successfully!");

// Show error toast
FluentToastManager::instance().showError(
    "Error", "Something went wrong.");
```

## Customization

### Custom Themes
The showcase demonstrates how to:
- Create custom color schemes
- Implement theme inheritance
- Add custom accent colors
- Integrate with system themes

### Responsive Design
Examples show how to:
- Adapt layouts for different screen sizes
- Hide/show components based on available space
- Implement mobile-friendly interactions
- Handle orientation changes

### Accessibility
The showcase includes:
- Proper ARIA labels and roles
- Keyboard navigation support
- High contrast mode compatibility
- Screen reader announcements

## Troubleshooting

### Common Issues
1. **Build Errors**: Ensure Qt 6.5+ and C++20 compiler are installed
2. **Missing Components**: Verify FluentQt library is properly linked
3. **Theme Not Loading**: Check that theme files are in the correct location
4. **Performance Issues**: Try disabling animations in system settings

### Debug Mode
Build in debug mode for additional logging:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

## Contributing

This showcase serves as a reference implementation. When contributing:
1. Follow the existing code style and patterns
2. Ensure all components are properly themed
3. Add accessibility features for new components
4. Update documentation for new features
5. Test across different themes and screen sizes

## License

This example is part of the FluentQt library and follows the same license terms.
