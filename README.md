# Element Fluent UI

A modern, comprehensive C++ Qt6 library implementing Microsoft's Fluent Design System. Element Fluent UI provides a complete set of beautiful, accessible, and performant UI components for building native desktop applications.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Qt Version](https://img.shields.io/badge/Qt-6.0%2B-green.svg)](https://www.qt.io/)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](#build-instructions)

## ✨ Features

### 🎨 **Complete Design System**
- **30+ UI Components** - Buttons, inputs, panels, navigation, data visualization, and more
- **Fluent Design Language** - Authentic Microsoft Fluent Design implementation
- **Light & Dark Themes** - Automatic system theme detection with custom theme support
- **Accessibility First** - WCAG 2.1 AA compliant with screen reader support

### 🚀 **Advanced Animation System**
- **36 Easing Functions** - From basic linear to advanced elastic and bounce effects
- **28 Animation Types** - Fade, slide, scale, rotate, morph, reveal, and micro-interactions
- **Performance Optimized** - Hardware acceleration with reduced motion support
- **Fluent Transitions** - Signature connected animations and reveal effects

### 🎯 **Developer Experience**
- **Modern C++20** - Leverages latest language features for clean, efficient code
- **Qt6 Integration** - Seamless integration with Qt's widget system
- **Type Safety** - Strong typing with comprehensive enums and validation
- **Memory Safe** - Smart pointers and RAII patterns throughout

### 📱 **Responsive & Adaptive**
- **High DPI Support** - Crisp rendering on all display densities
- **Flexible Layouts** - Responsive components that adapt to different screen sizes
- **Touch Friendly** - Optimized for both mouse and touch interactions
- **Cross Platform** - Windows, macOS, and Linux support

## 🚀 Quick Start

### Installation

#### Prerequisites
- **Qt6** (Core, Widgets, Gui, PrintSupport modules)
- **CMake** 3.16 or higher
- **C++20 compatible compiler** (GCC 10+, Clang 10+, MSVC 2019+)

#### Build from Source

**Quick Build:**
```bash
# Clone the repository
git clone https://github.com/ElementAstro/element-fluent-ui.git
cd element-fluent-ui

# One-command build
./scripts/build.sh --preset release    # Linux/macOS
.\scripts\build.ps1 -Preset release    # Windows
```

**Manual Build:**
```bash
# Configure and build
cmake --preset default
cmake --build --preset default

# Or with specific options
cmake -B build -DFLUENTQT_BUILD_EXAMPLES=ON
cmake --build build
```

📖 **For detailed build instructions, see [BUILD_GUIDE.md](docs/BUILD_GUIDE.md)**

#### Using with CMake

```cmake
# Find the package
find_package(FluentQt REQUIRED)

# Link to your target
target_link_libraries(your_target FluentQt::FluentQt)
```

### Basic Usage

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Animation/FluentAnimator.h"

using namespace FluentQt;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Initialize Fluent theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setAccentColor(Styling::FluentAccentColor::Blue);

    // Create main window
    QMainWindow window;
    auto* centralWidget = new QWidget;
    auto* layout = new QVBoxLayout(centralWidget);

    // Create a card container
    auto* card = new Components::FluentCard("Welcome to Fluent UI");
    card->setSubtitle("Modern UI components for Qt applications");

    // Add form components
    auto* nameInput = new Components::FluentTextInput("Enter your name...");
    auto* emailInput = new Components::FluentTextInput("Enter your email...");
    emailInput->setInputType(Components::FluentTextInputType::Email);

    // Create animated button
    auto* submitButton = Components::FluentButton::createPrimaryButton("Get Started");

    // Add components to card
    auto* cardLayout = card->createVerticalLayout();
    cardLayout->addWidget(nameInput);
    cardLayout->addWidget(emailInput);
    cardLayout->addWidget(submitButton);

    // Add card to main layout
    layout->addWidget(card);

    // Setup animations
    auto* animator = new Animation::FluentAnimator(&window);
    QObject::connect(submitButton, &Components::FluentButton::clicked, [=]() {
        animator->pulseEffect(submitButton);
        // Handle form submission
    });

    window.setCentralWidget(centralWidget);
    window.resize(400, 300);
    window.show();

    return app.exec();
}
```

## 📚 Documentation

### Core Documentation
- **[Component Guide](docs/COMPONENT_GUIDE.md)** - Comprehensive guide to all UI components
- **[API Reference](docs/API_REFERENCE.md)** - Complete API documentation
- **[Developer Guide](docs/DEVELOPER_GUIDE.md)** - Setup, building, and contributing
- **[Migration Guide](docs/MIGRATION_GUIDE.md)** - Migrating from other UI libraries

### Component Categories

#### 🎛️ **Form Components**
- **[FluentButton](docs/components/FluentButton.md)** - Buttons with multiple styles and states
- **[FluentTextInput](docs/components/FluentTextInput.md)** - Text inputs with validation
- **[FluentCheckBox](docs/components/FluentCheckBox.md)** - Checkboxes with tri-state support
- **[FluentRadioButton](docs/components/FluentRadioButton.md)** - Radio buttons with grouping
- **[FluentComboBox](docs/components/FluentComboBox.md)** - Dropdown selections
- **[FluentSlider](docs/components/FluentSlider.md)** - Range sliders and value pickers
- **[FluentToggleSwitch](docs/components/FluentToggleSwitch.md)** - Toggle switches

#### 📊 **Data & Feedback**
- **[FluentProgressBar](docs/components/FluentProgressBar.md)** - Progress indicators
- **[FluentLoadingIndicator](docs/components/FluentLoadingIndicator.md)** - Loading animations
- **[FluentTooltip](docs/components/FluentTooltip.md)** - Rich tooltips
- **[FluentNotification](docs/components/FluentNotification.md)** - Toast notifications
- **[FluentBadge](docs/components/FluentBadge.md)** - Status badges
- **[FluentRating](docs/components/FluentRating.md)** - Star ratings

#### 🏗️ **Layout & Navigation**
- **[FluentPanel](docs/components/FluentPanel.md)** - Versatile container panels
- **[FluentCard](docs/components/FluentCard.md)** - Content cards
- **[FluentNavigationView](docs/components/FluentNavigationView.md)** - Navigation sidebars
- **[FluentTabView](docs/components/FluentTabView.md)** - Tab containers
- **[FluentBreadcrumb](docs/components/FluentBreadcrumb.md)** - Navigation breadcrumbs
- **[FluentSplitter](docs/components/FluentSplitter.md)** - Resizable panes

#### 📅 **Specialized Components**
- **[FluentCalendar](docs/components/FluentCalendar.md)** - Date picker and calendar
- **[FluentTimePicker](docs/components/FluentTimePicker.md)** - Time selection
- **[FluentColorPicker](docs/components/FluentColorPicker.md)** - Color selection
- **[FluentTreeView](docs/components/FluentTreeView.md)** - Hierarchical data display
- **[FluentRichTextEditor](docs/components/FluentRichTextEditor.md)** - Rich text editing

## 🎨 Theming & Customization

### Theme System

Element Fluent UI provides a comprehensive theming system that automatically adapts to system preferences while allowing full customization.

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto& theme = FluentQt::Styling::FluentTheme::instance();

// Theme modes
theme.setMode(FluentThemeMode::Light);    // Light theme
theme.setMode(FluentThemeMode::Dark);     // Dark theme
theme.setMode(FluentThemeMode::System);   // Follow system preference

// Accent colors
theme.setAccentColor(FluentAccentColor::Blue);      // Windows blue
theme.setAccentColor(FluentAccentColor::Purple);    // Purple accent
theme.setAccentColor(FluentAccentColor::Green);     // Green accent

// Custom colors
theme.setCustomAccentColor(QColor(255, 100, 50));   // Custom orange

// Typography
QFont titleFont = theme.titleFont();        // 20px title font
QFont bodyFont = theme.bodyFont();          // 14px body font
QFont captionFont = theme.captionFont();    // 12px caption font

// Spacing and sizing
int mediumSpacing = theme.spacing("medium");     // 16px
int largeSpacing = theme.spacing("large");       // 24px
QSize iconSize = theme.iconSize("medium");       // 16x16px
```

### Accessibility Features

```cpp
// High contrast mode
theme.setHighContrastMode(true);

// Contrast ratio checking (WCAG compliance)
qreal ratio = theme.contrastRatio(textColor, backgroundColor);
bool isAccessible = ratio >= 4.5;  // WCAG AA standard

// Automatic contrast adjustment
QColor accessibleText = theme.ensureContrast(textColor, backgroundColor, 4.5);

// Reduced motion support
FluentAnimationConfig config;
config.respectReducedMotion = true;  // Honors system accessibility settings
```

## 🎬 Animation System

The animation system provides smooth, performant transitions that bring your UI to life while respecting accessibility preferences.

### Basic Animations

```cpp
#include "FluentQt/Animation/FluentAnimator.h"

auto* animator = new FluentAnimator(this);

// Fade animations
auto* fadeIn = animator->fadeIn(widget, 300, FluentEasing::CubicOut);
auto* fadeOut = animator->fadeOut(widget, 200, FluentEasing::CubicIn);

// Movement animations
auto* slideUp = animator->slideUp(widget, 250);
auto* slideDown = animator->slideDown(widget, 250);

// Scale animations
auto* scaleIn = animator->scaleIn(widget, 200, FluentEasing::BackOut);
auto* scaleOut = animator->scaleOut(widget, 150, FluentEasing::CubicIn);

// Start animations
fadeIn->start();
```

### Micro-interactions

```cpp
// Hover effects
animator->hoverEffect(button);

// Press feedback
animator->pressEffect(button);

// Focus indication
animator->focusEffect(input);

// Attention effects
animator->pulseEffect(notification);
animator->shakeEffect(errorField);
```

### Advanced Animations

```cpp
// Reveal animation (Fluent signature effect)
auto* reveal = animator->revealAnimation(panel, clickPoint);

// Connected animations (for navigation)
auto* connected = animator->connectedAnimation(fromWidget, toWidget);

// Stagger animations (for lists)
auto* stagger = animator->staggerAnimation(widgetList, FluentAnimationType::FadeIn);
```

## 🏗️ Architecture

### Component Hierarchy

```
FluentComponent (Base)
├── Form Components
│   ├── FluentButton
│   ├── FluentTextInput
│   ├── FluentCheckBox
│   └── FluentComboBox
├── Layout Components
│   ├── FluentPanel
│   ├── FluentCard
│   └── FluentNavigationView
└── Feedback Components
    ├── FluentProgressBar
    ├── FluentTooltip
    └── FluentNotification
```

### Core Systems

- **FluentComponent** - Base class providing state management, animations, and theming
- **FluentTheme** - Centralized theme and design token management
- **FluentAnimator** - Animation system with easing functions and transitions
- **FluentAccessible** - Accessibility support and screen reader integration

## 🧪 Examples

### Complete Application Example

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "FluentQt/Components/FluentNavigationView.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Styling/FluentTheme.h"

class FluentApp : public QMainWindow {
public:
    FluentApp() {
        setupTheme();
        setupUI();
        setupConnections();
    }

private:
    void setupTheme() {
        auto& theme = FluentQt::Styling::FluentTheme::instance();
        theme.setMode(FluentThemeMode::System);
        theme.setAccentColor(FluentAccentColor::Blue);
    }

    void setupUI() {
        // Create navigation
        auto* nav = new FluentNavigationView;
        nav->addItem(QIcon(":/home"), "Home");
        nav->addItem(QIcon(":/settings"), "Settings");
        nav->addItem(QIcon(":/profile"), "Profile");

        // Create main content
        auto* mainCard = new FluentCard("Dashboard");
        auto* layout = mainCard->createVerticalLayout();

        // Add form
        auto* nameInput = new FluentTextInput("Full Name");
        auto* emailInput = new FluentTextInput("Email Address");
        emailInput->setInputType(FluentTextInputType::Email);

        auto* submitBtn = FluentButton::createPrimaryButton("Submit");
        auto* progress = new FluentProgressBar;
        progress->setVisible(false);

        layout->addWidget(nameInput);
        layout->addWidget(emailInput);
        layout->addWidget(submitBtn);
        layout->addWidget(progress);

        // Setup main layout
        auto* centralWidget = new QWidget;
        auto* mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->addWidget(nav, 0);
        mainLayout->addWidget(mainCard, 1);

        setCentralWidget(centralWidget);
    }

    void setupConnections() {
        // Handle form submission with animations
        connect(submitBtn, &FluentButton::clicked, [=]() {
            progress->setVisible(true);
            progress->setType(FluentProgressType::Indeterminate);
            progress->start();

            // Simulate async operation
            QTimer::singleShot(2000, [=]() {
                progress->setVisible(false);
                // Show success notification
            });
        });
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    FluentApp window;
    window.show();

    return app.exec();
}
```

## 🔧 Build Configuration

### CMake Integration

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyFluentApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Find FluentQt
find_package(FluentQt REQUIRED)

# Create executable
add_executable(MyFluentApp main.cpp)

# Link libraries
target_link_libraries(MyFluentApp
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    FluentQt::FluentQt
)

# Enable Qt MOC
set_target_properties(MyFluentApp PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)
```

### Package Managers

#### vcpkg
```bash
vcpkg install element-fluent-ui
```

#### Conan
```bash
conan install element-fluent-ui/1.0.0@
```

## 🧪 Testing

### Running Tests

```bash
# Build with tests enabled
cmake -DBUILD_TESTING=ON ..
cmake --build .

# Run all tests
ctest

# Run specific test suite
./tests/FluentComponentsTest
./tests/FluentAnimationTest
./tests/FluentThemeTest
```

### Test Coverage

The library includes comprehensive tests covering:

- **Unit Tests** - Individual component functionality
- **Integration Tests** - Component interactions and theming
- **Accessibility Tests** - Screen reader and keyboard navigation
- **Performance Tests** - Animation performance and memory usage
- **Visual Tests** - Rendering accuracy across platforms

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

### Development Setup

```bash
# Clone with submodules
git clone --recursive https://github.com/your-username/element-fluent-ui.git

# Install development dependencies
# Qt6 development packages
# CMake 3.16+
# C++20 compiler

# Build in debug mode
mkdir build-debug && cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .

# Run tests
ctest
```

### Code Style

- **C++20** modern features encouraged
- **Qt conventions** for naming and patterns
- **RAII** and smart pointers for memory management
- **const correctness** throughout
- **Comprehensive documentation** for public APIs

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Microsoft** - For the Fluent Design System specification
- **Qt Project** - For the excellent Qt framework
- **Contributors** - Everyone who has contributed to this project

## 📞 Support

- **Documentation**: [docs/](docs/)
- **Examples**: [examples/](examples/)
- **Issues**: [GitHub Issues](https://github.com/your-username/element-fluent-ui/issues)
- **Discussions**: [GitHub Discussions](https://github.com/your-username/element-fluent-ui/discussions)

## 🗺️ Roadmap

### Current Version (1.0)
- ✅ Core component library
- ✅ Animation system
- ✅ Theme system
- ✅ Accessibility support

### Upcoming Features
- 🔄 **Data Grid Component** - Advanced table with sorting, filtering, virtualization
- 🔄 **Chart Components** - Line, bar, pie charts with animations
- 🔄 **Command Bar** - Ribbon-style command interface
- 🔄 **Fluent Icons** - Complete icon library integration
- 🔄 **Layout Animations** - Automatic layout transition animations
- 🔄 **Mobile Support** - Touch-optimized components for mobile Qt

### Future Versions
- **2.0** - Advanced data visualization components
- **3.0** - 3D effects and advanced materials
- **4.0** - AI-powered adaptive interfaces

---

**Made with ❤️ for the Qt community**

*Element Fluent UI - Bringing Microsoft's Fluent Design to Qt applications*
