# Element Fluent UI - Developer Guide

This guide provides comprehensive information for developers working with or contributing to Element Fluent UI, a modern C++ Qt6 library implementing Microsoft's Fluent Design System.

## Table of Contents

1. [Development Environment Setup](#development-environment-setup)
2. [Project Structure](#project-structure)
3. [Build System](#build-system)
4. [Coding Standards](#coding-standards)
5. [Component Development](#component-development)
6. [Testing](#testing)
7. [Documentation](#documentation)
8. [Contributing](#contributing)
9. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
10. [Performance Guidelines](#performance-guidelines)

## Development Environment Setup

### Prerequisites

**Required Tools:**
- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Qt6**: Version 6.2 or higher with the following modules:
  - Qt6::Core
  - Qt6::Widgets
  - Qt6::Gui
  - Qt6::PrintSupport (optional)

**Recommended Tools:**
- **IDE**: Qt Creator, Visual Studio, CLion, or VS Code with C++ extensions
- **Git**: For version control
- **Ninja**: Fast build system (optional but recommended)
- **Doxygen**: For generating API documentation
- **Valgrind/AddressSanitizer**: For memory debugging

### Environment Setup

#### Windows (MSVC)

```powershell
# Install Qt6 via Qt Installer or vcpkg
vcpkg install qt6-base qt6-widgets

# Set environment variables
$env:CMAKE_PREFIX_PATH = "C:\Qt\6.5.0\msvc2019_64"
$env:PATH += ";C:\Qt\6.5.0\msvc2019_64\bin"
```

#### Linux (Ubuntu/Debian)

```bash
# Install Qt6 development packages
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential

# Install additional dependencies
sudo apt install libgl1-mesa-dev libxkbcommon-dev
```

#### macOS

```bash
# Install Qt6 via Homebrew
brew install qt6 cmake

# Set environment variables
export CMAKE_PREFIX_PATH="/opt/homebrew/opt/qt6"
export PATH="/opt/homebrew/opt/qt6/bin:$PATH"
```

### IDE Configuration

#### Qt Creator

1. Open Qt Creator
2. Go to **Tools > Options > Kits**
3. Ensure Qt6 kit is properly configured
4. Set CMake generator to Ninja (recommended)
5. Enable Clang Code Model for better IntelliSense

#### Visual Studio Code

Install recommended extensions:
```json
{
    "recommendations": [
        "ms-vscode.cpptools",
        "ms-vscode.cmake-tools",
        "twxs.cmake",
        "ms-vscode.cpptools-extension-pack"
    ]
}
```

Configure `.vscode/settings.json`:
```json
{
    "cmake.configureArgs": [
        "-DCMAKE_PREFIX_PATH=/path/to/qt6"
    ],
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
}
```

## Project Structure

```
element-fluent-ui/
├── include/                    # Public headers
│   └── FluentQt/
│       ├── Components/         # UI component headers
│       ├── Core/              # Core system headers
│       ├── Styling/           # Theme and styling headers
│       └── Animation/         # Animation system headers
├── src/                       # Implementation files
│   ├── Components/            # Component implementations
│   ├── Core/                  # Core system implementations
│   ├── Styling/               # Theme implementations
│   └── Animation/             # Animation implementations
├── examples/                  # Example applications
│   ├── basic-usage/           # Basic usage examples
│   ├── advanced-features/     # Advanced feature demos
│   └── showcase/              # Complete showcase app
├── tests/                     # Unit and integration tests
│   ├── unit/                  # Unit tests
│   ├── integration/           # Integration tests
│   └── benchmarks/            # Performance benchmarks
├── docs/                      # Documentation
│   ├── components/            # Component documentation
│   ├── guides/                # Developer guides
│   └── api/                   # Generated API docs
├── resources/                 # Resource files
│   ├── icons/                 # Icon assets
│   ├── fonts/                 # Font files
│   └── themes/                # Theme definitions
├── tools/                     # Development tools
│   ├── code-generator/        # Component generator
│   └── theme-builder/         # Theme building tools
├── CMakeLists.txt             # Root CMake file
├── BUILD.md                   # Build instructions
└── README.md                  # Project overview
```

### Header Organization

**Public Headers** (`include/FluentQt/`):
- Only include what's necessary for public API
- Use forward declarations when possible
- Follow consistent naming conventions
- Include proper documentation comments

**Private Headers** (`src/private/`):
- Implementation details
- Internal helper classes
- Platform-specific code

## Build System

### CMake Configuration

The project uses modern CMake (3.16+) with the following key features:

```cmake
# Key CMake options
option(FLUENT_BUILD_EXAMPLES "Build example applications" ON)
option(FLUENT_BUILD_TESTS "Build unit tests" ON)
option(FLUENT_BUILD_DOCS "Build documentation" OFF)
option(FLUENT_ENABLE_ANIMATIONS "Enable animation system" ON)
option(FLUENT_ENABLE_ACCESSIBILITY "Enable accessibility features" ON)
```

### Build Types

```bash
# Debug build (default)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Release build
cmake -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release

# Release with debug info
cmake -B build-relwithdebinfo -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build-relwithdebinfo
```

### Custom Build Options

```bash
# Build with all features
cmake -B build \
    -DFLUENT_BUILD_EXAMPLES=ON \
    -DFLUENT_BUILD_TESTS=ON \
    -DFLUENT_BUILD_DOCS=ON \
    -DFLUENT_ENABLE_ANIMATIONS=ON \
    -DFLUENT_ENABLE_ACCESSIBILITY=ON

# Minimal build
cmake -B build-minimal \
    -DFLUENT_BUILD_EXAMPLES=OFF \
    -DFLUENT_BUILD_TESTS=OFF \
    -DFLUENT_ENABLE_ANIMATIONS=OFF
```

## Coding Standards

### C++ Standards

**Language Standard**: C++20
- Use modern C++ features appropriately
- Prefer standard library over custom implementations
- Use smart pointers for memory management
- Leverage RAII principles

### Naming Conventions

```cpp
// Classes: PascalCase
class FluentButton : public FluentComponent { };

// Methods and functions: camelCase
void setText(const QString& text);
bool isEnabled() const;

// Member variables: camelCase with m_ prefix
private:
    QString m_text;
    bool m_isEnabled{true};

// Constants: UPPER_SNAKE_CASE
static constexpr int DEFAULT_ANIMATION_DURATION = 300;

// Enums: PascalCase with descriptive prefix
enum class FluentButtonStyle {
    Default,
    Primary,
    Accent
};
```

### Code Style

**Formatting**:
- Use 4 spaces for indentation (no tabs)
- Maximum line length: 120 characters
- Braces on new lines for classes and functions
- Braces on same line for control structures

```cpp
class FluentButton : public FluentComponent
{
    Q_OBJECT

public:
    explicit FluentButton(QWidget* parent = nullptr);
    
    void setText(const QString& text);
    QString text() const noexcept;

private slots:
    void onClicked();

private:
    void updateStyle();
    
    QString m_text;
    FluentButtonStyle m_style{FluentButtonStyle::Default};
};
```

**Documentation**:
```cpp
/**
 * @brief Sets the button text
 * @param text The text to display on the button
 * 
 * This method updates the button's display text and triggers
 * a repaint if necessary. The text is automatically elided
 * if it exceeds the button's width.
 * 
 * @see text(), setIcon()
 * @since 1.0.0
 */
void setText(const QString& text);
```

### Memory Management

```cpp
// Prefer smart pointers
std::unique_ptr<FluentAnimator> m_animator;
std::shared_ptr<FluentTheme> m_theme;

// Use Qt's parent-child system for widgets
auto* button = new FluentButton(this);  // 'this' manages lifetime

// RAII for resources
class FluentResource {
public:
    FluentResource() : m_handle(acquireResource()) {}
    ~FluentResource() { releaseResource(m_handle); }
    
    // Non-copyable, movable
    FluentResource(const FluentResource&) = delete;
    FluentResource& operator=(const FluentResource&) = delete;
    FluentResource(FluentResource&&) = default;
    FluentResource& operator=(FluentResource&&) = default;
    
private:
    ResourceHandle m_handle;
};
```

### Error Handling

```cpp
// Use exceptions for exceptional cases
class FluentException : public std::runtime_error {
public:
    explicit FluentException(const std::string& message)
        : std::runtime_error(message) {}
};

// Use optional for nullable returns
std::optional<QColor> findColor(const QString& name) const;

// Use expected/result types for operations that can fail
enum class FluentError {
    InvalidInput,
    ResourceNotFound,
    PermissionDenied
};

template<typename T>
using FluentResult = std::expected<T, FluentError>;

FluentResult<QPixmap> loadIcon(const QString& path);
```

## Component Development

### Creating a New Component

1. **Generate Component Skeleton**:
```bash
# Use the component generator tool
./tools/code-generator/generate-component.py FluentNewComponent
```

2. **Header File Structure** (`include/FluentQt/Components/FluentNewComponent.h`):
```cpp
#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include <QtWidgets/QWidget>

class FluentNewComponentPrivate;

class FLUENT_EXPORT FluentNewComponent : public FluentComponent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FluentNewComponent)
    
    // Properties
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit FluentNewComponent(QWidget* parent = nullptr);
    ~FluentNewComponent() override;

    // Property accessors
    QString text() const;
    void setText(const QString& text);
    
    bool isEnabled() const;
    void setEnabled(bool enabled);

public slots:
    void performAction();

signals:
    void textChanged(const QString& text);
    void enabledChanged(bool enabled);
    void actionPerformed();

protected:
    // Event handling
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    
    // State management
    void updateStateStyle() override;

private:
    Q_DISABLE_COPY(FluentNewComponent)
    std::unique_ptr<FluentNewComponentPrivate> d_ptr;
};
```

3. **Implementation File** (`src/Components/FluentNewComponent.cpp`):
```cpp
#include "FluentQt/Components/FluentNewComponent.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Animation/FluentAnimator.h"

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

class FluentNewComponentPrivate
{
    Q_DECLARE_PUBLIC(FluentNewComponent)
    
public:
    explicit FluentNewComponentPrivate(FluentNewComponent* q);
    
    void updateStyle();
    void performAnimation();
    
    FluentNewComponent* q_ptr;
    QString text;
    bool enabled{true};
    std::unique_ptr<FluentAnimator> animator;
};

FluentNewComponent::FluentNewComponent(QWidget* parent)
    : FluentComponent(parent)
    , d_ptr(std::make_unique<FluentNewComponentPrivate>(this))
{
    Q_D(FluentNewComponent);
    
    // Initialize component
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    
    // Setup animations
    d->animator = std::make_unique<FluentAnimator>(this);
    
    // Connect to theme changes
    connect(&FluentTheme::instance(), &FluentTheme::themeChanged,
            this, [d]() { d->updateStyle(); });
}

// Implementation continues...
```

### Component Guidelines

**State Management**:
- Inherit from `FluentComponent` for consistent behavior
- Use the state system (`FluentState`) for visual feedback
- Implement smooth state transitions

**Theming Integration**:
- Always use `FluentTheme` for colors and styling
- Respond to theme changes automatically
- Support both light and dark modes

**Animation Integration**:
- Use `FluentAnimator` for consistent animations
- Respect user's motion preferences
- Provide animation disable options

**Accessibility**:
- Set appropriate accessible names and descriptions
- Support keyboard navigation
- Implement proper focus handling
- Test with screen readers

## Testing

### Unit Tests

Create unit tests for each component in `tests/unit/`:

```cpp
#include <QtTest/QtTest>
#include "FluentQt/Components/FluentButton.h"

class FluentButtonTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Test cases
    void testTextProperty();
    void testButtonStyles();
    void testClickSignal();
    void testKeyboardNavigation();
    void testAccessibility();

private:
    FluentButton* m_button{nullptr};
};

void FluentButtonTest::testTextProperty()
{
    const QString testText = "Test Button";
    m_button->setText(testText);
    
    QCOMPARE(m_button->text(), testText);
    
    QSignalSpy spy(m_button, &FluentButton::textChanged);
    m_button->setText("New Text");
    
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().first().toString(), QString("New Text"));
}

QTEST_MAIN(FluentButtonTest)
#include "FluentButtonTest.moc"
```

### Integration Tests

Test component interactions in `tests/integration/`:

```cpp
class FluentFormTest : public QObject
{
    Q_OBJECT

private slots:
    void testFormValidation();
    void testFormSubmission();
    void testThemeIntegration();
};
```

### Running Tests

```bash
# Build and run all tests
cmake --build build --target test

# Run specific test
./build/tests/unit/FluentButtonTest

# Run with verbose output
ctest --verbose

# Generate coverage report (if configured)
cmake --build build --target coverage
```

## Documentation

### API Documentation

Use Doxygen-style comments:

```cpp
/**
 * @class FluentButton
 * @brief A modern button component implementing Fluent Design principles
 * 
 * FluentButton provides a rich button experience with multiple styles,
 * animations, and accessibility features. It supports various visual
 * styles including Primary, Accent, and Subtle variants.
 * 
 * @code
 * auto* button = new FluentButton("Click Me");
 * button->setButtonStyle(FluentButtonStyle::Primary);
 * connect(button, &FluentButton::clicked, this, &MyClass::onButtonClicked);
 * @endcode
 * 
 * @see FluentComponent, FluentTheme
 * @since 1.0.0
 */
```

### Component Documentation

Each component should have:
1. **Overview** - Purpose and key features
2. **Usage Examples** - Common use cases
3. **API Reference** - Properties, methods, signals
4. **Best Practices** - Recommended usage patterns
5. **Accessibility** - Accessibility considerations

### Building Documentation

```bash
# Configure with documentation enabled
cmake -B build -DFLUENT_BUILD_DOCS=ON

# Build documentation
cmake --build build --target docs

# Open generated documentation
open build/docs/html/index.html
```

## Contributing

### Development Workflow

1. **Fork and Clone**:
```bash
git clone https://github.com/yourusername/element-fluent-ui.git
cd element-fluent-ui
```

2. **Create Feature Branch**:
```bash
git checkout -b feature/new-component
```

3. **Make Changes**:
- Follow coding standards
- Add tests for new functionality
- Update documentation

4. **Test Changes**:
```bash
cmake --build build --target test
```

5. **Submit Pull Request**:
- Provide clear description
- Reference related issues
- Ensure CI passes

### Code Review Process

**Review Checklist**:
- [ ] Code follows style guidelines
- [ ] Tests are included and passing
- [ ] Documentation is updated
- [ ] API is consistent with existing components
- [ ] Accessibility is considered
- [ ] Performance impact is minimal

### Commit Guidelines

Use conventional commit format:
```
feat(button): add loading state support
fix(theme): resolve dark mode color issues
docs(api): update FluentButton documentation
test(input): add validation test cases
```

## Debugging and Troubleshooting

### Common Issues

**Build Issues**:
```bash
# Clear CMake cache
rm -rf build/CMakeCache.txt build/CMakeFiles

# Verbose build output
cmake --build build --verbose
```

**Qt Issues**:
```bash
# Check Qt installation
qmake --version
cmake --find-package Qt6

# Debug Qt module loading
export QT_DEBUG_PLUGINS=1
```

**Runtime Debugging**:
```cpp
// Enable Qt logging
QLoggingCategory::setFilterRules("fluent.*.debug=true");

// Use Qt's debugging macros
qDebug() << "Button clicked with text:" << text();
qWarning() << "Invalid state transition:" << oldState << "->" << newState;
```

### Performance Profiling

```bash
# Build with profiling enabled
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Profile with perf (Linux)
perf record ./build/examples/showcase/FluentShowcase
perf report

# Profile with Instruments (macOS)
instruments -t "Time Profiler" ./build/examples/showcase/FluentShowcase
```

## Performance Guidelines

### General Principles

1. **Minimize Repaints**: Cache expensive calculations
2. **Efficient Layouts**: Use appropriate layout managers
3. **Memory Management**: Avoid memory leaks and excessive allocations
4. **Animation Performance**: Use hardware acceleration when available

### Optimization Techniques

```cpp
// Cache expensive calculations
class FluentButton {
private:
    mutable QPixmap m_cachedBackground;
    mutable bool m_backgroundDirty{true};
    
    const QPixmap& backgroundPixmap() const {
        if (m_backgroundDirty) {
            m_cachedBackground = generateBackground();
            m_backgroundDirty = false;
        }
        return m_cachedBackground;
    }
};

// Use efficient painting
void FluentButton::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Use cached pixmaps for complex backgrounds
    painter.drawPixmap(rect(), backgroundPixmap());
    
    // Clip to update region
    painter.setClipRegion(event->region());
}
```

### Memory Optimization

```cpp
// Use object pools for frequently created objects
class FluentAnimationPool {
public:
    std::unique_ptr<QPropertyAnimation> acquire() {
        if (!m_pool.empty()) {
            auto animation = std::move(m_pool.back());
            m_pool.pop_back();
            return animation;
        }
        return std::make_unique<QPropertyAnimation>();
    }
    
    void release(std::unique_ptr<QPropertyAnimation> animation) {
        animation->stop();
        animation->setTargetObject(nullptr);
        m_pool.push_back(std::move(animation));
    }
    
private:
    std::vector<std::unique_ptr<QPropertyAnimation>> m_pool;
};
```

---

This developer guide provides the foundation for working with Element Fluent UI. For specific implementation details, refer to the source code and component documentation.
