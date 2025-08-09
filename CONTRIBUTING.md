# Contributing to FluentQt

Thank you for your interest in contributing to FluentQt! This document provides guidelines and information for contributors.

## 🌟 Ways to Contribute

There are many ways to contribute to FluentQt:

- 🐛 **Report bugs** - Help us identify and fix issues
- ✨ **Suggest features** - Propose new components or improvements
- 📝 **Improve documentation** - Help make our docs clearer and more comprehensive
- 🧪 **Write tests** - Increase test coverage and reliability
- 💻 **Submit code** - Fix bugs, implement features, or optimize performance
- 🎨 **Design feedback** - Help ensure components follow Fluent Design principles
- 🌍 **Translations** - Help make FluentQt accessible to more developers

## 🚀 Getting Started

### Prerequisites

Before contributing, ensure you have:

- **Qt6** (6.2.0 or later) with development packages
- **CMake** 3.20 or higher
- **C++20 compatible compiler** (GCC 10+, Clang 10+, MSVC 2019+)
- **Git** for version control
- **Node.js** 16+ (for documentation development)

### Development Setup

1. **Fork and clone the repository:**
   ```bash
   git clone https://github.com/YOUR_USERNAME/element-fluent-ui.git
   cd element-fluent-ui
   ```

2. **Install development dependencies:**
   ```bash
   # Install Node.js dependencies for documentation
   npm install
   
   # Install Qt6 and other system dependencies
   # See README.md for platform-specific instructions
   ```

3. **Build the project:**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Debug \
            -DFLUENTQT_BUILD_EXAMPLES=ON \
            -DFLUENTQT_BUILD_TESTS=ON \
            -DFLUENTQT_BUILD_DOCS=ON
   cmake --build . --parallel
   ```

4. **Run tests to verify setup:**
   ```bash
   ctest --output-on-failure
   ```

5. **Start documentation server (optional):**
   ```bash
   npm run docs:dev
   ```

## 📋 Development Workflow

### Branch Strategy

- `main` - Stable release branch
- `develop` - Development branch for next release
- `feature/feature-name` - Feature development branches
- `bugfix/issue-number` - Bug fix branches
- `docs/topic` - Documentation improvement branches

### Making Changes

1. **Create a feature branch:**
   ```bash
   git checkout develop
   git pull origin develop
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes following our coding standards**

3. **Write or update tests:**
   ```bash
   # Run tests frequently during development
   cd build && ctest
   ```

4. **Update documentation if needed:**
   ```bash
   # Test documentation locally
   npm run docs:dev
   ```

5. **Commit your changes:**
   ```bash
   git add .
   git commit -m "feat: add new component FluentAwesomeWidget
   
   - Implements basic FluentAwesomeWidget functionality
   - Adds comprehensive unit tests
   - Updates documentation with usage examples
   - Follows Fluent Design System guidelines
   
   Closes #123"
   ```

6. **Push and create a pull request:**
   ```bash
   git push origin feature/your-feature-name
   ```

## 📝 Coding Standards

### C++ Style Guidelines

We follow a modified version of the Qt coding style:

#### Naming Conventions
```cpp
// Classes: PascalCase with Fluent prefix
class FluentButton : public FluentComponent { };

// Methods and variables: camelCase
void setEnabled(bool enabled);
bool isEnabled() const;

// Constants: UPPER_SNAKE_CASE
static const int DEFAULT_ANIMATION_DURATION = 300;

// Private members: camelCase with m_ prefix
private:
    bool m_isEnabled = true;
    QString m_text;
```

#### Code Formatting
- **Indentation:** 4 spaces (no tabs)
- **Line length:** 120 characters maximum
- **Braces:** Opening brace on same line for functions, new line for classes
- **Spacing:** Space after keywords, around operators

```cpp
// Good
if (condition) {
    doSomething();
}

class FluentWidget : public QWidget
{
public:
    explicit FluentWidget(QWidget* parent = nullptr);
    
private:
    void setupUI();
};
```

#### Modern C++ Features
- Use C++20 features when appropriate
- Prefer `auto` for complex types
- Use smart pointers for memory management
- Leverage RAII patterns
- Use `constexpr` and `const` liberally

```cpp
// Good
auto* button = new FluentButton(this);
const auto theme = FluentTheme::instance();
constexpr int MARGIN = 8;

// Use smart pointers for ownership
std::unique_ptr<FluentAnimator> m_animator;
```

### Documentation Standards

#### Code Documentation
Use Doxygen-style comments for all public APIs:

```cpp
/**
 * @brief Creates a primary button with the specified text
 * 
 * Primary buttons are used for the main action in a UI. They have
 * the accent color background and are visually prominent.
 * 
 * @param text The button text to display
 * @param parent The parent widget (optional)
 * @return Pointer to the created FluentButton
 * 
 * @code
 * auto* saveButton = FluentButton::createPrimaryButton("Save");
 * connect(saveButton, &FluentButton::clicked, this, &MyClass::save);
 * @endcode
 * 
 * @see createSecondaryButton(), createTextButton()
 * @since 1.0.0
 */
static FluentButton* createPrimaryButton(const QString& text, QWidget* parent = nullptr);
```

#### Markdown Documentation
- Use clear, concise language
- Include code examples for all features
- Add screenshots for UI components
- Follow the existing documentation structure

### Testing Standards

#### Unit Tests
- Write tests for all new functionality
- Aim for >90% code coverage
- Use descriptive test names
- Follow AAA pattern (Arrange, Act, Assert)

```cpp
TEST_F(FluentButtonTest, ShouldEmitClickedSignalWhenPressed)
{
    // Arrange
    FluentButton button("Test");
    QSignalSpy clickSpy(&button, &FluentButton::clicked);
    
    // Act
    QTest::mouseClick(&button, Qt::LeftButton);
    
    // Assert
    EXPECT_EQ(clickSpy.count(), 1);
}
```

#### Integration Tests
- Test component interactions
- Verify theme system integration
- Test accessibility features
- Validate animation behavior

## 🎨 Design Guidelines

### Fluent Design Principles

All components must follow Microsoft's Fluent Design System:

1. **Light** - Use subtle shadows and depth
2. **Depth** - Create visual hierarchy with elevation
3. **Motion** - Smooth, purposeful animations
4. **Material** - Consistent visual language
5. **Scale** - Responsive across different screen sizes

### Component Design Checklist

- [ ] Follows Fluent Design visual guidelines
- [ ] Supports both light and dark themes
- [ ] Includes hover, focus, and pressed states
- [ ] Implements proper accessibility features
- [ ] Responsive to different screen sizes
- [ ] Includes smooth animations/transitions
- [ ] Consistent with existing components

### Accessibility Requirements

All components must be accessible:

- [ ] WCAG 2.1 AA compliant
- [ ] Proper ARIA labels and roles
- [ ] Keyboard navigation support
- [ ] Screen reader compatibility
- [ ] High contrast mode support
- [ ] Sufficient color contrast ratios
- [ ] Reduced motion support

## 🧪 Testing Your Changes

### Running Tests

```bash
# Run all tests
cd build && ctest

# Run specific test suite
./tests/FluentButtonTest

# Run with verbose output
ctest --verbose

# Run performance tests
ctest -R Performance
```

### Manual Testing

1. **Build and run examples:**
   ```bash
   ./examples/FluentQtDemo
   ./examples/CarouselShowcase
   ```

2. **Test on different platforms:**
   - Windows 10/11
   - macOS 12+
   - Ubuntu 20.04+

3. **Test accessibility:**
   - Use screen reader (NVDA, VoiceOver, Orca)
   - Test keyboard navigation
   - Verify high contrast mode
   - Check color contrast ratios

4. **Test theming:**
   - Switch between light/dark modes
   - Test different accent colors
   - Verify custom themes

## 📤 Submitting Changes

### Pull Request Process

1. **Ensure your branch is up to date:**
   ```bash
   git checkout develop
   git pull origin develop
   git checkout your-feature-branch
   git rebase develop
   ```

2. **Run the full test suite:**
   ```bash
   npm run test:all
   ```

3. **Create a pull request with:**
   - Clear title and description
   - Reference to related issues
   - Screenshots for UI changes
   - Breaking change notes (if any)

### Pull Request Template

Use our PR template to ensure all requirements are met:

- [ ] Tests added/updated and passing
- [ ] Documentation updated
- [ ] Code follows style guidelines
- [ ] Accessibility requirements met
- [ ] Breaking changes documented
- [ ] Examples updated (if needed)

### Review Process

1. **Automated checks** - CI/CD pipeline runs tests
2. **Code review** - Maintainers review code quality
3. **Design review** - UI/UX review for component changes
4. **Accessibility review** - Accessibility compliance check
5. **Final approval** - Maintainer approval for merge

## 🏷️ Release Process

### Versioning

We follow [Semantic Versioning](https://semver.org/):

- **MAJOR** - Breaking changes
- **MINOR** - New features (backward compatible)
- **PATCH** - Bug fixes (backward compatible)

### Changelog

All changes are documented in [CHANGELOG.md](CHANGELOG.md) following [Keep a Changelog](https://keepachangelog.com/) format.

## 🤝 Community Guidelines

### Code of Conduct

We are committed to providing a welcoming and inclusive environment. Please read and follow our [Code of Conduct](CODE_OF_CONDUCT.md).

### Communication

- **GitHub Issues** - Bug reports and feature requests
- **GitHub Discussions** - General questions and ideas
- **Discord** - Real-time chat and community support
- **Email** - Security issues and private matters

### Recognition

Contributors are recognized in:
- README.md contributors section
- Release notes
- Documentation credits
- Annual contributor highlights

## ❓ Getting Help

If you need help contributing:

1. Check existing [documentation](https://fluentqt.elementastro.org)
2. Search [GitHub Issues](https://github.com/ElementAstro/element-fluent-ui/issues)
3. Ask in [GitHub Discussions](https://github.com/ElementAstro/element-fluent-ui/discussions)
4. Join our [Discord server](https://discord.gg/elementastro)
5. Email maintainers for sensitive issues

## 📄 License

By contributing to FluentQt, you agree that your contributions will be licensed under the [MIT License](LICENSE).

---

Thank you for contributing to FluentQt! Your efforts help make Qt development more beautiful and accessible for everyone. 🎉
