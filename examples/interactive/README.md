# Interactive Examples Collection

This directory contains comprehensive, standalone interactive examples that demonstrate real-world usage scenarios and component combinations in the Element Fluent UI library.

## Overview

Each example is designed to be:
- **Self-contained**: Can be compiled and run independently
- **Interactive**: Demonstrates real user interactions and workflows
- **Educational**: Shows best practices and common patterns
- **Production-ready**: Uses proper error handling and performance optimizations

## Example Categories

### 🏢 Business Applications

#### [Customer Management System](business/customer-management/)
A complete customer relationship management interface demonstrating:
- FluentTreeView for hierarchical customer data
- FluentCard for customer details display
- FluentTextInput with validation for data entry
- FluentButton for actions and navigation
- FluentProgressBar for data loading states

#### [Project Dashboard](business/project-dashboard/)
A comprehensive project management dashboard featuring:
- FluentPanel layouts for organized content
- FluentCalendar for project timelines
- FluentProgressBar for project completion tracking
- FluentToast for status notifications
- FluentNavigationView for section navigation

#### [Invoice Generator](business/invoice-generator/)
A professional invoice creation tool showcasing:
- FluentTextInput for invoice details
- FluentComboBox for customer selection
- FluentCheckBox for invoice options
- FluentButton for actions and calculations
- FluentCard for invoice preview

### 🎮 Interactive Demos

#### [Component Playground](interactive/component-playground/)
An interactive testing environment for all components:
- Live property editing
- Real-time preview updates
- Code generation
- Theme switching
- Accessibility testing

#### [Theme Designer](interactive/theme-designer/)
A visual theme customization tool featuring:
- Color picker integration
- Live theme preview
- Export/import functionality
- Component gallery with theme variations

#### [Layout Builder](interactive/layout-builder/)
A drag-and-drop interface builder demonstrating:
- Dynamic component creation
- Layout management
- Property panels
- Code export functionality

### 📱 Mobile-Responsive Examples

#### [Responsive Dashboard](responsive/dashboard/)
A dashboard that adapts to different screen sizes:
- Responsive FluentPanel layouts
- Adaptive FluentNavigationView
- Mobile-optimized FluentButton sizing
- Touch-friendly interactions

#### [Mobile Form Designer](responsive/form-designer/)
A form builder optimized for mobile devices:
- Touch-optimized FluentTextInput
- Mobile-friendly FluentComboBox
- Responsive FluentCheckBox layouts
- Adaptive validation displays

### 🎨 Design System Examples

#### [Design Token Explorer](design-system/token-explorer/)
An interactive design token browser:
- Live token editing
- Visual token preview
- Export capabilities
- Documentation generation

#### [Component Library Browser](design-system/component-browser/)
A comprehensive component showcase:
- Interactive component gallery
- Usage examples
- Code snippets
- Best practices guide

### 🔧 Developer Tools

#### [Debug Console](developer/debug-console/)
A debugging interface for component development:
- Component state inspection
- Event logging
- Performance monitoring
- Accessibility auditing

#### [Performance Profiler](developer/performance-profiler/)
A tool for analyzing component performance:
- Render time measurement
- Memory usage tracking
- Animation performance analysis
- Optimization suggestions

## Getting Started

### Prerequisites

- Qt 6.5 or later
- CMake 3.20 or later
- C++20 compatible compiler
- Element Fluent UI library

### Building Examples

1. **Clone the repository:**
   ```bash
   git clone https://github.com/your-org/element-fluent-ui.git
   cd element-fluent-ui/examples/interactive
   ```

2. **Create build directory:**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake:**
   ```bash
   cmake ..
   ```

4. **Build all examples:**
   ```bash
   cmake --build .
   ```

5. **Build specific example:**
   ```bash
   cmake --build . --target customer-management
   ```

### Running Examples

Each example can be run independently:

```bash
# Run customer management system
./business/customer-management/customer-management

# Run component playground
./interactive/component-playground/component-playground

# Run responsive dashboard
./responsive/dashboard/responsive-dashboard
```

## Example Structure

Each example follows a consistent structure:

```
example-name/
├── CMakeLists.txt          # Build configuration
├── README.md               # Example-specific documentation
├── main.cpp                # Application entry point
├── src/                    # Source files
│   ├── MainWindow.cpp
│   ├── MainWindow.h
│   └── ...
├── resources/              # Resources (icons, images, etc.)
│   ├── icons/
│   └── images/
└── docs/                   # Additional documentation
    ├── screenshots/
    └── usage-guide.md
```

## Code Quality Standards

All examples follow these standards:

### Modern C++ Practices
- C++20 features where appropriate
- RAII for resource management
- Smart pointers for memory management
- Range-based for loops
- Auto type deduction

### Qt Best Practices
- Proper signal/slot connections
- Efficient layout management
- Resource management with QResource
- Internationalization support
- Accessibility features

### Element Fluent UI Guidelines
- Consistent component usage
- Proper theme integration
- Accessibility compliance
- Performance optimization
- Error handling

## Contributing

When adding new examples:

1. **Follow the established structure**
2. **Include comprehensive documentation**
3. **Add screenshots and usage guides**
4. **Ensure cross-platform compatibility**
5. **Test with different themes and screen sizes**
6. **Include accessibility features**
7. **Add performance considerations**

## Documentation

Each example includes:

- **README.md**: Overview and setup instructions
- **Usage Guide**: Step-by-step usage instructions
- **Screenshots**: Visual documentation
- **Code Comments**: Inline documentation
- **API Usage**: Demonstration of component APIs

## Testing

Examples are tested for:

- **Compilation**: All platforms and configurations
- **Functionality**: Core features work as expected
- **Performance**: Acceptable performance characteristics
- **Accessibility**: Screen reader and keyboard navigation
- **Responsive Design**: Different screen sizes and orientations

## Support

For questions or issues with examples:

1. Check the example's README.md
2. Review the main documentation
3. Search existing issues
4. Create a new issue with:
   - Example name
   - Platform and Qt version
   - Detailed description
   - Steps to reproduce

## License

All examples are provided under the same license as the Element Fluent UI library.

## Next Steps

After exploring these examples:

1. **Study the source code** to understand implementation patterns
2. **Modify examples** to experiment with different configurations
3. **Create your own examples** based on these templates
4. **Contribute improvements** back to the community
5. **Share your experiences** with other developers

## Example Highlights

### Customer Management System
A complete business application demonstrating:
- **Real-time search and filtering** with debounced input
- **CRUD operations** with proper validation and error handling
- **Data export functionality** with progress indication
- **Responsive layout** that adapts to different screen sizes
- **Professional UI patterns** commonly used in business applications

### Component Playground
An interactive testing environment featuring:
- **Live property editing** with immediate visual feedback
- **Code generation** for tested configurations
- **Theme switching** to test appearance variations
- **Accessibility testing** tools and guidelines
- **Performance monitoring** for optimization insights

### Responsive Dashboard
A modern dashboard showcasing:
- **Adaptive layouts** that work on desktop, tablet, and mobile
- **Touch-friendly interactions** for mobile devices
- **Progressive disclosure** of information based on screen size
- **Optimized performance** for different device capabilities

## Learning Path

1. **Start with Customer Management** to understand basic component usage
2. **Explore Component Playground** to experiment with properties and themes
3. **Study Responsive Dashboard** to learn adaptive design patterns
4. **Build your own examples** using these as templates

These interactive examples provide a comprehensive foundation for understanding and using the Element Fluent UI library in real-world applications.
