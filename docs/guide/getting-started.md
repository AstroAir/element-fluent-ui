# Getting Started

Welcome to FluentQt! This guide will help you set up and start using FluentQt in your Qt6 applications.

## Prerequisites

Before you begin, ensure you have the following installed:

- **Qt6** (6.2.0 or later) with the following modules:
  - Qt6Core
  - Qt6Widgets
  - Qt6Gui
  - Qt6PrintSupport
  - Qt6Charts (optional, for chart components)
- **CMake** 3.20 or higher
- **C++20 compatible compiler**:
  - GCC 10+ on Linux
  - Clang 10+ on macOS/Linux
  - MSVC 2019+ on Windows

## Installation

### Option 1: Using vcpkg (Recommended)

vcpkg is the easiest way to install FluentQt and its dependencies:

```bash
# Install vcpkg if you haven't already
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh  # or bootstrap-vcpkg.bat on Windows

# Install FluentQt
./vcpkg install fluentqt
```

### Option 2: Using Conan

If you prefer Conan for package management:

```bash
# Install Conan if you haven't already
pip install conan

# Create a conanfile.txt
echo "[requires]
fluentqt/1.0.0

[generators]
CMakeDeps
CMakeToolchain" > conanfile.txt

# Install dependencies
conan install . --build=missing
```

### Option 3: Building from Source

For the latest features or custom builds:

```bash
# Clone the repository
git clone https://github.com/ElementAstro/element-fluent-ui.git
cd element-fluent-ui

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DFLUENTQT_BUILD_EXAMPLES=ON \
         -DFLUENTQT_BUILD_TESTS=ON

# Build
cmake --build . --parallel

# Install (optional)
cmake --install . --prefix /usr/local
```

## Your First FluentQt Application

Let's create a simple "Hello World" application to verify your installation:

### 1. Create Project Structure

```
my-fluent-app/
├── CMakeLists.txt
├── main.cpp
└── README.md
```

### 2. CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyFluentApp VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard
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

### 3. main.cpp

```cpp
#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>

// Include FluentQt headers
#include "FluentQt/FluentQt.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Initialize FluentQt
    if (!FluentQt::initialize(&app)) {
        qCritical() << "Failed to initialize FluentQt";
        return -1;
    }

    // Set up theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);
    theme.setAccentColor(Styling::FluentAccentColor::Blue);

    // Create main window
    auto* window = new QWidget;
    window->setWindowTitle("My First FluentQt App");
    window->resize(400, 300);

    // Create layout
    auto* layout = new QVBoxLayout(window);

    // Create a welcome card
    auto* welcomeCard = new Components::FluentCard("Welcome to FluentQt!");
    welcomeCard->setSubtitle("Your first Fluent Design application");

    // Create card content layout
    auto* cardLayout = welcomeCard->createVerticalLayout();

    // Add a text input
    auto* nameInput = new Components::FluentTextInput("Enter your name...");
    cardLayout->addWidget(nameInput);

    // Add buttons
    auto* primaryButton = Components::FluentButton::createPrimaryButton("Say Hello");
    auto* secondaryButton = Components::FluentButton::createSecondaryButton("Clear");

    cardLayout->addWidget(primaryButton);
    cardLayout->addWidget(secondaryButton);

    // Add card to main layout
    layout->addWidget(welcomeCard);

    // Connect button signals
    QObject::connect(primaryButton, &Components::FluentButton::clicked, [=]() {
        QString name = nameInput->text();
        if (name.isEmpty()) {
            name = "World";
        }
        nameInput->setText(QString("Hello, %1!").arg(name));
    });

    QObject::connect(secondaryButton, &Components::FluentButton::clicked, [=]() {
        nameInput->clear();
    });

    // Show the window
    window->show();

    return app.exec();
}
```

### 4. Build and Run

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./MyFluentApp  # or MyFluentApp.exe on Windows
```

## Key Concepts

### FluentQt Initialization

Always call `FluentQt::initialize()` at the start of your application:

```cpp
if (!FluentQt::initialize(&app)) {
    qCritical() << "Failed to initialize FluentQt";
    return -1;
}
```

This sets up:
- Theme system
- Animation engine
- Accessibility features
- Platform integration

### Theme Management

FluentQt provides a comprehensive theming system:

```cpp
auto& theme = FluentQt::Styling::FluentTheme::instance();

// Set theme mode
theme.setMode(FluentQt::Styling::FluentThemeMode::Dark);

// Set accent color
theme.setAccentColor(FluentQt::Styling::FluentAccentColor::Purple);

// Get theme colors
QColor background = theme.color(FluentQt::Styling::FluentColor::Background);
QColor accent = theme.accentColor();
```

### Component Usage

All FluentQt components inherit from `FluentComponent` and follow consistent patterns:

```cpp
// Create components
auto* button = new FluentQt::Components::FluentButton("Click me");
auto* card = new FluentQt::Components::FluentCard("Title", "Subtitle");

// Configure properties
button->setEnabled(true);
card->setElevation(2);

// Connect signals
connect(button, &FluentQt::Components::FluentButton::clicked, 
        this, &MyClass::handleButtonClick);
```

## Next Steps

Now that you have FluentQt set up and running:

1. **Explore Components** - Check out the [Component Guide](/components/) to learn about all available UI components
2. **Learn Theming** - Read the [Theming Guide](/guide/theming) to customize your app's appearance
3. **Add Animations** - Discover the [Animation System](/guide/animation) for smooth transitions
4. **Ensure Accessibility** - Follow the [Accessibility Guide](/guide/accessibility) for inclusive design
5. **See Examples** - Browse [Example Applications](/examples/) for real-world usage patterns

## Troubleshooting

### Common Issues

**Qt6 not found:**
```bash
# Make sure Qt6 is in your PATH or set Qt6_DIR
export Qt6_DIR=/path/to/qt6/lib/cmake/Qt6
```

**FluentQt not found:**
```bash
# If building from source, set FluentQt_DIR
export FluentQt_DIR=/path/to/fluentqt/lib/cmake/FluentQt
```

**Compilation errors:**
- Ensure you're using C++20 compatible compiler
- Check that all required Qt6 modules are installed
- Verify CMake version is 3.20 or higher

### Getting Help

If you encounter issues:

1. Check the [FAQ](/guide/faq)
2. Search [GitHub Issues](https://github.com/ElementAstro/element-fluent-ui/issues)
3. Ask on [Discord](https://discord.gg/elementastro)
4. Create a [new issue](https://github.com/ElementAstro/element-fluent-ui/issues/new) with a minimal reproduction case

## What's Next?

Continue your FluentQt journey:

- [Component System](/guide/component-system) - Understanding the architecture
- [Theming Deep Dive](/guide/theming) - Advanced customization
- [Animation Guide](/guide/animation) - Creating smooth interactions
- [Performance Tips](/guide/performance) - Optimizing your application
