# FluentQt Test Suite

This directory contains comprehensive tests for all FluentQt components and systems. The test suite is built using Qt Test framework and provides thorough coverage of functionality, performance, and accessibility.

## Test Structure

### Core System Tests

- **FluentAnimatorTest** - Animation system functionality
- **FluentThemeTest** - Theme and styling system
- **FluentPerformanceTest** - Performance benchmarks and monitoring

### Component Tests

- **FluentFormComponentsTest** - Form controls (TextInput, CheckBox, etc.)
- **FluentFeedbackComponentsTest** - Feedback components (ProgressBar, LoadingIndicator, Tooltip)
- **FluentLayoutComponentsTest** - Layout components (Panel, Grid, Breadcrumb)

### Integration Tests

- **FluentIntegrationTest** - Cross-component integration
- **FluentAccessibilityTest** - Accessibility compliance

## Running Tests

### Prerequisites

- Qt 6.0 or later
- CMake 3.16 or later
- C++17 compatible compiler

### Build and Run

```bash
# Build the test suite
mkdir build && cd build
cmake ..
make

# Run all tests
make run_tests

# Run specific test
ctest -R FluentAnimatorTest

# Run tests with verbose output
ctest --output-on-failure

# Run tests in parallel
ctest --parallel 4
```

### Test Targets

```bash
# Run all tests
make run_tests

# Generate coverage report (requires gcov/lcov)
make coverage

# Run performance benchmarks
make benchmark

# Memory leak detection (Linux with Valgrind)
make memcheck
```

## Test Categories

### Unit Tests

Each component has comprehensive unit tests covering:

- **Constructor behavior** - Proper initialization and parameter handling
- **Basic functionality** - Core features and API methods
- **State management** - State transitions and property changes
- **Signal emission** - Proper signal/slot behavior
- **Edge cases** - Error handling and boundary conditions
- **Memory management** - Proper cleanup and leak prevention

### Integration Tests

Integration tests verify:

- **Component interaction** - How components work together
- **Theme integration** - Proper theme application and updates
- **Animation coordination** - Multiple animations working together
- **Event handling** - Proper event propagation and handling

### Performance Tests

Performance tests measure:

- **Animation performance** - Frame rates and smoothness
- **Memory usage** - Memory allocation and cleanup
- **Rendering performance** - Paint event efficiency
- **Startup time** - Component initialization speed

### Accessibility Tests

Accessibility tests verify:

- **Keyboard navigation** - Proper tab order and key handling
- **Screen reader support** - Accessible names and descriptions
- **High contrast support** - Proper contrast ratios
- **Reduced motion** - Animation alternatives for accessibility

## Test Examples

### Basic Component Test

```cpp
void FluentComponentTest::testBasicFunctionality() {
    auto* component = new FluentComponent();
    
    // Test initial state
    QCOMPARE(component->state(), FluentState::Normal);
    
    // Test state changes
    QSignalSpy stateSpy(component, &FluentComponent::stateChanged);
    component->setState(FluentState::Hovered);
    
    QCOMPARE(component->state(), FluentState::Hovered);
    QCOMPARE(stateSpy.count(), 1);
    
    delete component;
}
```

### Animation Test

```cpp
void FluentAnimatorTest::testFadeAnimation() {
    auto* widget = new QWidget();
    auto* animator = new FluentAnimator();
    
    auto* animation = animator->fadeIn(widget, 300);
    QVERIFY(animation != nullptr);
    
    QSignalSpy finishedSpy(animation, &QPropertyAnimation::finished);
    animation->start();
    
    QVERIFY(finishedSpy.wait(1000));
    QCOMPARE(finishedSpy.count(), 1);
    
    delete animation;
    delete animator;
    delete widget;
}
```

### Theme Test

```cpp
void FluentThemeTest::testColorSystem() {
    auto& theme = FluentTheme::instance();
    
    // Test color retrieval
    QColor primary = theme.color("backgroundPrimary");
    QVERIFY(primary.isValid());
    
    // Test theme changes
    QSignalSpy themeSpy(&theme, &FluentTheme::themeChanged);
    theme.setDarkMode(true);
    
    QCOMPARE(themeSpy.count(), 1);
    
    QColor darkPrimary = theme.color("backgroundPrimary");
    QVERIFY(darkPrimary != primary);
}
```

## Test Configuration

### Environment Variables

- `QT_QPA_PLATFORM=offscreen` - Run tests headless
- `FLUENT_TEST_TIMEOUT=30` - Set test timeout in seconds
- `FLUENT_TEST_VERBOSE=1` - Enable verbose test output

### Test Data

Test data files are located in the `tests/data/` directory and include:

- Sample images for icon tests
- Test configuration files
- Mock data for validation tests

### Continuous Integration

The test suite is designed to run in CI environments:

```yaml
# Example GitHub Actions configuration
- name: Run Tests
  run: |
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    make
    xvfb-run -a make run_tests
```

## Writing New Tests

### Test File Structure

```cpp
#include <QtTest/QtTest>
#include "FluentQt/Components/YourComponent.h"

class YourComponentTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();    // Run once before all tests
    void init();           // Run before each test
    void cleanup();        // Run after each test
    void cleanupTestCase(); // Run once after all tests
    
    // Your test methods
    void testBasicFunctionality();
    void testEdgeCases();

private:
    YourComponent* m_component{nullptr};
};

void YourComponentTest::init() {
    m_component = new YourComponent();
}

void YourComponentTest::cleanup() {
    delete m_component;
    m_component = nullptr;
}

// Test implementations...

QTEST_MAIN(YourComponentTest)
#include "YourComponentTest.moc"
```

### Test Guidelines

1. **Test naming** - Use descriptive test method names
2. **Setup/teardown** - Always clean up resources
3. **Assertions** - Use appropriate QVERIFY/QCOMPARE macros
4. **Signal testing** - Use QSignalSpy for signal verification
5. **Async testing** - Use QSignalSpy::wait() for async operations
6. **Error handling** - Test both success and failure cases

### Adding Tests to CMake

```cmake
# Add your test to tests/CMakeLists.txt
add_fluent_test(YourComponentTest YourComponentTest.cpp)
```

## Debugging Tests

### Running Single Tests

```bash
# Run specific test with debugging
./YourComponentTest

# Run with Qt logging
QT_LOGGING_RULES="*.debug=true" ./YourComponentTest

# Run with GDB
gdb ./YourComponentTest
```

### Common Issues

1. **Widget not shown** - Ensure `QTest::qWaitForWindowExposed(widget)`
2. **Signal not emitted** - Check signal/slot connections
3. **Animation not finishing** - Verify animation duration and wait time
4. **Memory leaks** - Ensure proper cleanup in test teardown

## Coverage Reports

Coverage reports are generated using gcov/lcov:

```bash
make coverage
# Open coverage/html/index.html in browser
```

The coverage report shows:

- Line coverage percentage
- Function coverage
- Branch coverage
- Uncovered code sections

## Performance Benchmarks

Performance benchmarks measure:

- Animation frame rates
- Component creation time
- Memory allocation patterns
- Paint event performance

```bash
make benchmark
# Results saved to benchmark_results.txt
```

## Contributing

When adding new components or features:

1. Write comprehensive unit tests
2. Add integration tests if needed
3. Update performance benchmarks
4. Verify accessibility compliance
5. Update documentation

For more information, see the main project documentation.
