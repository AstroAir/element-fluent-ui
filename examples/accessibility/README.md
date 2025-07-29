# Accessibility and Responsive Design Examples

This directory contains comprehensive examples demonstrating accessibility features, keyboard navigation, screen reader support, and responsive behavior across different screen sizes for Element Fluent UI components.

## Overview

These examples showcase how to create inclusive, accessible applications that work for all users, including those with disabilities, and adapt seamlessly to different devices and screen sizes.

## Accessibility Features Demonstrated

### 🎯 Core Accessibility Principles

#### WCAG 2.1 Compliance
- **Level AA compliance** for color contrast ratios
- **Keyboard navigation** for all interactive elements
- **Screen reader support** with proper ARIA attributes
- **Focus management** with visible focus indicators
- **Alternative text** for images and icons

#### Inclusive Design Patterns
- **High contrast mode** support
- **Reduced motion** preferences
- **Large text** scaling support
- **Voice control** compatibility
- **Touch accessibility** for mobile devices

### 🔧 Technical Implementation

#### ARIA Attributes
- Proper role assignments
- State and property announcements
- Landmark navigation
- Live region updates
- Descriptive labels

#### Keyboard Navigation
- Tab order management
- Arrow key navigation
- Escape key handling
- Enter/Space activation
- Custom keyboard shortcuts

#### Screen Reader Support
- Semantic HTML structure
- Descriptive announcements
- Context information
- Progress updates
- Error notifications

## Responsive Design Features

### 📱 Device Adaptation

#### Breakpoint System
- **Mobile**: < 768px
- **Tablet**: 768px - 1024px
- **Desktop**: > 1024px
- **Large Desktop**: > 1440px

#### Adaptive Layouts
- Flexible grid systems
- Collapsible navigation
- Responsive typography
- Touch-friendly sizing
- Orientation handling

#### Performance Optimization
- Lazy loading for large screens
- Optimized images for different densities
- Efficient layout calculations
- Smooth animations across devices

## Example Categories

### 🎮 Interactive Accessibility Demos

#### [Accessible Form Builder](forms/accessible-form-builder/)
Demonstrates comprehensive form accessibility:
- **FluentTextInput** with proper labeling and validation
- **FluentCheckBox** with group relationships
- **FluentRadioButton** with keyboard navigation
- **FluentComboBox** with screen reader announcements
- **FluentButton** with loading states and feedback

#### [Navigation Accessibility](navigation/accessible-navigation/)
Shows accessible navigation patterns:
- **FluentNavigationView** with landmark roles
- **FluentTabView** with proper tab management
- **FluentBreadcrumb** with navigation context
- **FluentTreeView** with hierarchical navigation

#### [Data Accessibility](data/accessible-data/)
Demonstrates accessible data presentation:
- **FluentTreeView** with sortable, filterable data
- **FluentProgressBar** with progress announcements
- **FluentCard** with structured content
- **FluentToast** with non-intrusive notifications

### 📱 Responsive Design Demos

#### [Responsive Dashboard](responsive/dashboard/)
A complete dashboard that adapts to all screen sizes:
- **Adaptive layouts** using FluentPanel
- **Collapsible sidebar** with FluentNavigationView
- **Responsive cards** with FluentCard
- **Touch-optimized controls** for mobile

#### [Mobile-First Form](responsive/mobile-form/)
A form designed mobile-first:
- **Touch-friendly inputs** with FluentTextInput
- **Adaptive validation** with contextual feedback
- **Progressive enhancement** for larger screens
- **Gesture support** for mobile interactions

#### [Responsive Data Table](responsive/data-table/)
A data table that works on all devices:
- **Horizontal scrolling** on mobile
- **Column hiding** based on screen size
- **Touch gestures** for sorting and filtering
- **Accessible table navigation**

### 🎨 Theme and Contrast Examples

#### [High Contrast Mode](themes/high-contrast/)
Demonstrates high contrast accessibility:
- **Automatic contrast detection**
- **Enhanced focus indicators**
- **Simplified visual design**
- **Improved text readability**

#### [Dark Mode Accessibility](themes/dark-mode/)
Shows accessible dark mode implementation:
- **Proper contrast ratios** in dark theme
- **Reduced eye strain** considerations
- **Consistent focus indicators**
- **Accessible color schemes**

## Getting Started

### Prerequisites

- Qt 6.5 or later with accessibility support
- Screen reader software (NVDA, JAWS, VoiceOver)
- Browser developer tools for testing
- Mobile devices or emulators for responsive testing

### Building Examples

```bash
# Navigate to accessibility examples
cd examples/accessibility

# Create build directory
mkdir build && cd build

# Configure with accessibility features enabled
cmake .. -DENABLE_ACCESSIBILITY=ON

# Build all examples
cmake --build .
```

### Testing Accessibility

#### Screen Reader Testing
1. **Install screen reader software**:
   - Windows: NVDA (free) or JAWS
   - macOS: VoiceOver (built-in)
   - Linux: Orca

2. **Test with screen reader**:
   ```bash
   # Run example with screen reader active
   ./forms/accessible-form-builder/accessible-form-builder
   ```

3. **Verify announcements**:
   - Navigate with Tab key
   - Use arrow keys within components
   - Listen for state changes
   - Check error announcements

#### Keyboard Navigation Testing
1. **Test tab order**:
   - Use Tab to move forward
   - Use Shift+Tab to move backward
   - Verify logical order

2. **Test component navigation**:
   - Arrow keys in lists and trees
   - Enter/Space for activation
   - Escape for cancellation

3. **Test custom shortcuts**:
   - Application-specific shortcuts
   - Context menu activation
   - Quick navigation keys

#### Responsive Testing
1. **Test different screen sizes**:
   ```bash
   # Resize window to test breakpoints
   # Use browser developer tools
   # Test on actual devices
   ```

2. **Test orientation changes**:
   - Portrait to landscape
   - Layout adaptation
   - Content reflow

3. **Test touch interactions**:
   - Tap targets (minimum 44px)
   - Gesture support
   - Touch feedback

## Code Examples

### Accessible Form Component

```cpp
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Accessibility/FluentAccessibility.h"

class AccessibleForm : public QWidget {
public:
    AccessibleForm(QWidget* parent = nullptr) : QWidget(parent) {
        setupAccessibleForm();
    }

private:
    void setupAccessibleForm() {
        auto* layout = new QVBoxLayout(this);
        
        // Accessible text input
        auto* nameInput = new FluentTextInput();
        nameInput->setLabelText("Full Name");
        nameInput->setAccessibleName("Full Name Input");
        nameInput->setAccessibleDescription("Enter your complete name");
        nameInput->setRequired(true);
        
        // Associate label with input
        auto* nameLabel = new QLabel("Full Name *");
        nameLabel->setBuddy(nameInput);
        
        // Add validation feedback
        connect(nameInput, &FluentTextInput::textChanged, [=](const QString& text) {
            if (text.isEmpty()) {
                nameInput->setInputState(FluentTextInputState::Error);
                nameInput->setHelperText("Name is required");
                // Announce error to screen readers
                FluentAccessibility::announceText("Error: Name is required");
            } else {
                nameInput->setInputState(FluentTextInputState::Success);
                nameInput->setHelperText("Valid name entered");
            }
        });
        
        layout->addWidget(nameLabel);
        layout->addWidget(nameInput);
    }
};
```

### Responsive Layout Component

```cpp
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Layout/FluentResponsiveLayout.h"

class ResponsiveLayout : public QWidget {
public:
    ResponsiveLayout(QWidget* parent = nullptr) : QWidget(parent) {
        setupResponsiveLayout();
        connectResizeSignals();
    }

protected:
    void resizeEvent(QResizeEvent* event) override {
        QWidget::resizeEvent(event);
        updateLayoutForSize(event->size());
    }

private:
    void setupResponsiveLayout() {
        m_layout = new FluentResponsiveLayout(this);
        
        // Create panels that adapt to screen size
        m_sidebarPanel = new FluentPanel("Navigation");
        m_contentPanel = new FluentPanel("Content");
        m_detailsPanel = new FluentPanel("Details");
        
        // Set responsive behavior
        m_layout->addWidget(m_sidebarPanel, 0, 0, 1, 1);
        m_layout->addWidget(m_contentPanel, 0, 1, 1, 2);
        m_layout->addWidget(m_detailsPanel, 0, 3, 1, 1);
        
        // Configure breakpoints
        m_layout->setBreakpoint(FluentBreakpoint::Mobile, 768);
        m_layout->setBreakpoint(FluentBreakpoint::Tablet, 1024);
        m_layout->setBreakpoint(FluentBreakpoint::Desktop, 1440);
    }
    
    void updateLayoutForSize(const QSize& size) {
        if (size.width() < 768) {
            // Mobile layout: stack vertically
            m_layout->setColumnStretch(0, 0);
            m_sidebarPanel->setVisible(false);
            m_detailsPanel->setVisible(false);
        } else if (size.width() < 1024) {
            // Tablet layout: show sidebar and content
            m_layout->setColumnStretch(0, 1);
            m_sidebarPanel->setVisible(true);
            m_detailsPanel->setVisible(false);
        } else {
            // Desktop layout: show all panels
            m_layout->setColumnStretch(0, 1);
            m_sidebarPanel->setVisible(true);
            m_detailsPanel->setVisible(true);
        }
    }
    
    FluentResponsiveLayout* m_layout;
    FluentPanel* m_sidebarPanel;
    FluentPanel* m_contentPanel;
    FluentPanel* m_detailsPanel;
};
```

## Best Practices

### Accessibility Guidelines

1. **Always provide text alternatives** for non-text content
2. **Ensure keyboard accessibility** for all interactive elements
3. **Use sufficient color contrast** (4.5:1 for normal text, 3:1 for large text)
4. **Provide clear focus indicators** that are visible and distinct
5. **Use semantic HTML** and proper ARIA attributes
6. **Test with actual assistive technologies**
7. **Consider cognitive accessibility** with clear language and navigation

### Responsive Design Guidelines

1. **Design mobile-first** then enhance for larger screens
2. **Use flexible layouts** that adapt to content
3. **Optimize touch targets** (minimum 44px for mobile)
4. **Test on real devices** not just emulators
5. **Consider performance** on lower-end devices
6. **Handle orientation changes** gracefully
7. **Provide appropriate content** for each screen size

## Testing Checklist

### Accessibility Testing
- [ ] Screen reader navigation works correctly
- [ ] All interactive elements are keyboard accessible
- [ ] Focus indicators are visible and clear
- [ ] Color contrast meets WCAG guidelines
- [ ] Error messages are announced properly
- [ ] Form labels are properly associated
- [ ] Landmark navigation is available

### Responsive Testing
- [ ] Layout adapts to different screen sizes
- [ ] Touch targets are appropriately sized
- [ ] Content is readable at all sizes
- [ ] Navigation works on mobile devices
- [ ] Performance is acceptable on all devices
- [ ] Orientation changes are handled properly
- [ ] Images scale appropriately

## Resources

### Accessibility Tools
- **WAVE**: Web accessibility evaluation tool
- **axe**: Accessibility testing engine
- **Lighthouse**: Accessibility auditing
- **Color Contrast Analyzers**: For testing contrast ratios

### Responsive Design Tools
- **Browser DevTools**: For testing different screen sizes
- **Responsive Design Mode**: Built into modern browsers
- **Device Emulators**: For testing mobile interactions
- **Performance Profilers**: For testing on different devices

These examples provide a comprehensive foundation for creating accessible and responsive applications with Element Fluent UI.
