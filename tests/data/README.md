# Test Data Directory

This directory contains test data files used by the FluentQt test suite.

## Directory Structure

- `icons/` - Test icon files for icon-related tests
- `themes/` - Test theme files for theme system tests
- `fonts/` - Test font files for typography tests
- `images/` - Test image files for image-related components
- `configs/` - Test configuration files
- `samples/` - Sample data files for various tests

## Usage

Test data files are automatically copied to the build directory during the build process and can be accessed in tests using the `FLUENT_QT_TEST_DATA_DIR` macro or the `FluentQtTest::getTestResource()` helper function.

Example:
```cpp
QString iconPath = FluentQtTest::getTestResource("icons/test_icon.png");
QIcon testIcon(iconPath);
```

## Adding New Test Data

1. Add your test data files to the appropriate subdirectory
2. Update the test that uses the data to reference the correct path
3. Ensure the file is small and necessary for testing
4. Document any special requirements or formats

## File Size Guidelines

- Keep test data files as small as possible
- Use compressed formats when appropriate
- Avoid large binary files unless absolutely necessary
- Consider generating test data programmatically instead of using files
