# FluentQt Testing Guide

This comprehensive guide covers the testing framework, methodologies, and best practices for the FluentQt component library.

## Overview

FluentQt includes a comprehensive test suite with 45+ component tests covering all major UI components, utilities, and system integrations. The testing framework is built on Qt Test Framework and provides extensive coverage for functionality, user interactions, accessibility, and performance.

## Test Architecture

### Test Categories

The test suite is organized into logical categories:

#### Core System Tests (3 tests)
- **FluentAnimatorTest** - Animation system functionality
- **FluentThemeTest** - Theme management and switching
- **FluentComponentTest** - Base component functionality

#### Basic UI Components (6 tests)
- **FluentButtonTest** - Button interactions, states, styling
- **FluentCheckBoxTest** - Check states, tri-state support
- **FluentRadioButtonTest** - Selection groups, exclusivity
- **FluentToggleSwitchTest** - Toggle states, animations
- **FluentSliderTest** - Value ranges, orientation, ticks
- **FluentSpinBoxTest** - Numeric input, bounds, step values

#### Input Components (7 tests)
- **FluentTextBoxTest** - Text input, validation, formatting
- **FluentPasswordBoxTest** - Secure input, strength validation
- **FluentComboBoxTest** - Dropdown selection, editable mode
- **FluentSearchBoxTest** - Search functionality, suggestions
- **FluentDatePickerTest** - Date selection, calendar integration
- **FluentColorPickerTest** - Color selection, formats
- **FluentFilePickerTest** - File/folder selection, filters

#### Layout & Navigation (6 tests)
- **FluentNavigationViewTest** - Menu navigation, panes
- **FluentTabViewTest** - Tab management, closable tabs
- **FluentPivotTest** - Pivot navigation, content switching
- **FluentSplitViewTest** - Resizable panes, splitters
- **FluentAccordionTest** - Expandable sections
- **FluentBreadcrumbTest** - Navigation breadcrumbs

#### Data Display (6 tests)
- **FluentDataTableTest** - Tabular data, sorting, filtering
- **FluentTreeViewPerfTest** - Hierarchical data display
- **FluentGridTest** - Grid layout, item management
- **FluentCardTest** - Content cards, styling
- **FluentBadgeTest** - Status indicators, counts
- **FluentAvatarTest** - User avatars, initials

#### Feedback & Progress (5 tests)
- **FluentProgressBarTest** - Linear progress indication
- **FluentProgressRingTest** - Circular progress indication
- **FluentLoadingIndicatorTest** - Loading states, animations
- **FluentNotificationTest** - Toast notifications, alerts
- **FluentTooltipTest** - Hover information display

#### Media & Visualization (6 tests)
- **FluentImageViewTest** - Image display, zoom, pan
- **FluentChartViewTest** - Data visualization, charts
- **FluentCarouselTest** - Image/content carousels
- **FluentAutoCarouselTest** - Automatic sliding
- **FluentBasicCarouselTest** - Basic carousel functionality
- **FluentIndicatorCarouselTest** - Carousel with indicators

#### Dialog & Overlay (4 tests)
- **FluentContentDialogTest** - Modal dialogs
- **FluentContextMenuTest** - Right-click menus
- **FluentDropdownTest** - Dropdown overlays
- **FluentSheetTest** - Bottom sheets, slide-up panels

#### Utility Components (5 tests)
- **FluentScrollAreaTest** - Custom scrolling
- **FluentEnhancedScrollBarTest** - Enhanced scroll bars
- **FluentResizableTest** - Resizable containers
- **FluentRatingControlTest** - Star ratings, feedback
- **FluentCalendarTest** - Calendar widget

#### Integration & Accessibility (2 tests)
- **FluentIntegrationTest** - Cross-component integration
- **FluentAccessibilityTest** - Accessibility compliance

## Running Tests

### Prerequisites

```bash
# Ensure Qt Test is available
find_package(Qt6 REQUIRED COMPONENTS Test)

# Build the project with tests enabled
cmake -DBUILD_TESTING=ON ..
make
```

### Basic Test Execution

```bash
# Run all tests
python tests/run_tests.py

# Run specific test
python tests/run_tests.py --test FluentButtonTest

# Run tests by category
python tests/run_tests.py --category basic_ui

# List available categories
python tests/run_tests.py --list-categories
```

### Advanced Test Options

```bash
# Generate coverage report
python tests/run_tests.py --coverage

# Run with memory checking
python tests/run_tests.py --memcheck FluentButtonTest

# Run performance benchmarks
python tests/run_tests.py --benchmark

# Verbose output for debugging
python tests/run_tests.py --verbose --test FluentButtonTest
```

### CMake Test Integration

```bash
# Run tests via CMake
cmake --build . --target run_tests

# Run specific test category
ctest -R "Fluent.*Test" --output-on-failure

# Parallel test execution
ctest -j4 --output-on-failure
```

## Test Coverage

Each test file provides comprehensive coverage including:

### Functional Testing
- **Constructor Tests** - Default initialization, parameter validation
- **Property Tests** - Getters/setters, bounds checking, validation
- **State Management** - State changes, persistence, consistency
- **API Coverage** - All public methods and properties

### Interaction Testing
- **Mouse Events** - Clicks, drags, hover effects
- **Keyboard Input** - Key presses, shortcuts, navigation
- **Touch Events** - Touch gestures (where applicable)
- **Focus Management** - Tab order, focus indicators

### Visual Testing
- **Rendering** - Correct visual output
- **Styling** - Theme application, custom styles
- **Animations** - Smooth transitions, timing
- **Responsive Design** - Different sizes, orientations

### Signal/Slot Testing
- **Event Emission** - Correct signal emission
- **Parameter Validation** - Signal parameters
- **Connection Testing** - Slot connections
- **Timing** - Signal emission timing

### Edge Case Testing
- **Boundary Conditions** - Min/max values, limits
- **Invalid Input** - Error handling, validation
- **Null/Empty States** - Handling of empty data
- **Memory Management** - Leak detection, cleanup

## Writing New Tests

### Test File Structure

```cpp
// tests/Components/FluentNewComponentTest.cpp
#include <QSignalSpy>
#include <QtTest/QtTest>
#include "FluentQt/Components/FluentNewComponent.h"

class FluentNewComponentTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    // Test methods
    void testDefaultConstructor();
    void testPropertySetters();
    void testSignalEmission();
    void testUserInteraction();

private:
    FluentNewComponent* m_component{nullptr};
    QWidget* m_testWidget{nullptr};
};

// Implementation...
QTEST_MAIN(FluentNewComponentTest)
#include "FluentNewComponentTest.moc"
```

### Test Method Guidelines

1. **Naming Convention**: Use descriptive names starting with `test`
2. **Setup/Teardown**: Use `init()`/`cleanup()` for per-test setup
3. **Assertions**: Use appropriate `QCOMPARE`, `QVERIFY`, etc.
4. **Signal Testing**: Use `QSignalSpy` for signal verification
5. **User Simulation**: Use `QTest::mouseClick`, `QTest::keyClick`

### Adding Tests to Build System

1. Add test file to `tests/Components/`
2. Update `tests/CMakeLists.txt`:
   ```cmake
   add_fluent_test(FluentNewComponentTest Components/FluentNewComponentTest.cpp)
   ```
3. Add to appropriate category in test runner
4. Update documentation

## Continuous Integration

### GitHub Actions Integration

Tests run automatically on:
- Pull requests
- Main branch commits
- Release tags

### Test Reporting

- **JUnit XML** - For CI integration
- **Coverage Reports** - Code coverage metrics
- **Performance Metrics** - Benchmark results
- **Memory Reports** - Leak detection results

## Best Practices

### Test Design
- **Isolation** - Each test should be independent
- **Repeatability** - Tests should produce consistent results
- **Clarity** - Test intent should be obvious
- **Completeness** - Cover all code paths

### Performance
- **Fast Execution** - Keep tests quick
- **Parallel Safe** - Tests should not interfere
- **Resource Cleanup** - Proper cleanup of resources
- **Mock Usage** - Use mocks for external dependencies

### Maintenance
- **Regular Updates** - Keep tests current with code changes
- **Refactoring** - Improve test quality over time
- **Documentation** - Document complex test scenarios
- **Review Process** - Include tests in code reviews

## Troubleshooting

### Common Issues

1. **Test Timeouts** - Increase timeout for slow operations
2. **Display Issues** - Use `QTest::qWaitForWindowExposed()`
3. **Signal Timing** - Add `QTest::qWait()` for async operations
4. **Memory Leaks** - Ensure proper cleanup in `cleanup()`

### Debugging Tests

```bash
# Run single test with debugging
gdb --args ./FluentButtonTest

# Enable Qt logging
export QT_LOGGING_RULES="*.debug=true"

# Memory debugging with Valgrind
valgrind --tool=memcheck ./FluentButtonTest
```

## Contributing

When contributing new components or features:

1. **Write Tests First** - TDD approach recommended
2. **Comprehensive Coverage** - Aim for >90% code coverage
3. **Follow Patterns** - Use existing tests as templates
4. **Update Documentation** - Keep this guide current
5. **Run Full Suite** - Ensure no regressions

## Resources

- [Qt Test Framework Documentation](https://doc.qt.io/qt-6/qtest-overview.html)
- [QSignalSpy Documentation](https://doc.qt.io/qt-6/qsignalspy.html)
- [CMake Testing](https://cmake.org/cmake/help/latest/manual/ctest.1.html)
- [FluentQt Component Guide](COMPONENT_GUIDE.md)
