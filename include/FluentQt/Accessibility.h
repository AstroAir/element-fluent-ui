/**
 * @file Accessibility.h
 * @brief Comprehensive header for FluentQt accessibility features
 *
 * This header includes all accessibility-related classes and utilities
 * for creating inclusive, WCAG 2.1 AA compliant applications. It provides
 * screen reader support, keyboard navigation, high contrast themes,
 * and accessibility validation tools.
 *
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#pragma once

// Core Accessibility System
#include "FluentQt/Accessibility/FluentAccessibilityManager.h"
#include "FluentQt/Accessibility/FluentAccessible.h"

// Screen Reader Support
#include "FluentQt/Accessibility/FluentScreenReaderManager.h"

// Keyboard Navigation
#include "FluentQt/Accessibility/FluentKeyboardNavigationManager.h"

// Accessibility Validation
#include "FluentQt/Accessibility/FluentWcagChecker.h"

// Animation Accessibility
#include "FluentQt/Accessibility/FluentAccessibleAnimator.h"

/**
 * @namespace FluentQt::Accessibility
 * @brief Namespace containing all accessibility functionality
 *
 * The Accessibility namespace provides comprehensive accessibility
 * features to ensure FluentQt applications are usable by everyone,
 * including users with disabilities. It follows WCAG 2.1 AA guidelines
 * and integrates with platform accessibility APIs.
 *
 * **Core Features:**
 * - Screen reader support (NVDA, JAWS, VoiceOver, Orca)
 * - Keyboard navigation management
 * - High contrast theme support
 * - Reduced motion preferences
 * - Focus management and indication
 * - Accessible rich internet applications (ARIA) support
 *
 * **WCAG 2.1 AA Compliance:**
 * - Color contrast validation (4.5:1 ratio)
 * - Keyboard accessibility
 * - Focus indicators
 * - Alternative text for images
 * - Proper heading structure
 * - Form labels and descriptions
 *
 * **Screen Reader Features:**
 * - Live regions for dynamic content
 * - Proper role and state announcements
 * - Descriptive text for complex UI elements
 * - Navigation landmarks
 * - Table headers and captions
 *
 * **Keyboard Navigation:**
 * - Tab order management
 * - Arrow key navigation for complex widgets
 * - Escape key handling
 * - Shortcut key support
 * - Focus trapping in modals
 *
 * **Visual Accessibility:**
 * - High contrast mode detection
 * - Automatic color adjustment
 * - Scalable UI elements
 * - Clear focus indicators
 * - Sufficient color contrast
 *
 * @code
 * // Basic accessibility setup
 * auto& accessibility =
 * FluentQt::Accessibility::FluentAccessibilityManager::instance();
 * accessibility.initialize();
 *
 * // Make a widget accessible
 * auto* button = new FluentButton("Save");
 * FluentQt::Accessibility::makeAccessible(button, "Save document", "Saves the
 * current document to disk");
 *
 * // Check color contrast
 * bool isAccessible = FluentQt::Accessibility::validateContrast(textColor,
 * backgroundColor);
 *
 * // Enable high contrast mode
 * FluentQt::Accessibility::setHighContrastMode(true);
 * @endcode
 */
namespace FluentQt::Accessibility {

/**
 * @enum AccessibilityRole
 * @brief Accessibility roles for UI elements
 */
enum class AccessibilityRole {
    Button,
    CheckBox,
    RadioButton,
    ComboBox,
    Slider,
    ProgressBar,
    TextInput,
    Label,
    Heading,
    Link,
    List,
    ListItem,
    Table,
    TableCell,
    TableHeader,
    TabList,
    Tab,
    TabPanel,
    Dialog,
    AlertDialog,
    Menu,
    MenuItem,
    Toolbar,
    Tooltip,
    Status,
    Alert,
    Log,
    Marquee,
    Timer,
    Group,
    Region,
    Article,
    Banner,
    Complementary,
    ContentInfo,
    Form,
    Main,
    Navigation,
    Search
};

/**
 * @enum AccessibilityState
 * @brief Accessibility states for UI elements
 */
enum class AccessibilityState {
    Normal,
    Disabled,
    Focused,
    Pressed,
    Checked,
    Unchecked,
    Indeterminate,
    Expanded,
    Collapsed,
    Selected,
    Unselected,
    Busy,
    Invalid,
    Required,
    ReadOnly
};

/**
 * @enum WcagLevel
 * @brief WCAG compliance levels
 */
enum class WcagLevel {
    A,   ///< WCAG Level A (minimum)
    AA,  ///< WCAG Level AA (standard)
    AAA  ///< WCAG Level AAA (enhanced)
};

/**
 * @struct AccessibilityInfo
 * @brief Accessibility information for a UI element
 */
struct AccessibilityInfo {
    QString name;               ///< Accessible name
    QString description;        ///< Detailed description
    AccessibilityRole role;     ///< Element role
    AccessibilityState state;   ///< Current state
    QString value;              ///< Current value (for inputs)
    QString help;               ///< Help text
    QStringList labels;         ///< Associated labels
    QString shortcut;           ///< Keyboard shortcut
    bool isLiveRegion = false;  ///< Whether content changes dynamically
    QString liveRegionType =
        "polite";  ///< Live region politeness (polite/assertive)
};

/**
 * @brief Initialize the accessibility system
 *
 * This function initializes the FluentQt accessibility system,
 * sets up platform integration, and configures default settings.
 *
 * @return true if initialization was successful
 */
bool initializeAccessibility();

/**
 * @brief Initialize accessibility system safely for any platform
 *
 * This function provides a safe way to initialize accessibility that
 * automatically detects the platform and skips problematic operations
 * in headless/offscreen environments.
 *
 * @param forceFullInit If true, forces full initialization even in offscreen
 * mode
 * @return true if initialization was successful
 */
bool initializeAccessibilitySafe(bool forceFullInit = false);

/**
 * @brief Make a widget accessible
 *
 * @param widget The widget to make accessible
 * @param name Accessible name
 * @param description Detailed description
 * @param role Accessibility role
 */
void makeAccessible(QWidget* widget, const QString& name,
                    const QString& description = QString(),
                    AccessibilityRole role = AccessibilityRole::Button);

/**
 * @brief Set accessibility information for a widget
 *
 * @param widget The widget to configure
 * @param info Accessibility information
 */
void setAccessibilityInfo(QWidget* widget, const AccessibilityInfo& info);

/**
 * @brief Get accessibility information for a widget
 *
 * @param widget The widget to query
 * @return Current accessibility information
 */
AccessibilityInfo getAccessibilityInfo(QWidget* widget);

/**
 * @brief Check if screen reader is active
 * @return true if a screen reader is detected
 */
bool isScreenReaderActive();

/**
 * @brief Check if high contrast mode is enabled
 * @return true if high contrast mode is active
 */
bool isHighContrastMode();

/**
 * @brief Set high contrast mode
 * @param enabled Whether to enable high contrast mode
 */
void setHighContrastMode(bool enabled);

/**
 * @brief Check if reduced motion is enabled
 * @return true if reduced motion is preferred
 */
bool isReducedMotionEnabled();

/**
 * @brief Validate color contrast ratio
 *
 * @param foreground Foreground color
 * @param background Background color
 * @param level WCAG compliance level to check
 * @return true if contrast meets the specified level
 */
bool validateContrast(const QColor& foreground, const QColor& background,
                      WcagLevel level = WcagLevel::AA);

/**
 * @brief Calculate color contrast ratio
 *
 * @param color1 First color
 * @param color2 Second color
 * @return Contrast ratio (1.0 to 21.0)
 */
double calculateContrastRatio(const QColor& color1, const QColor& color2);

/**
 * @brief Get minimum contrast ratio for WCAG level
 *
 * @param level WCAG compliance level
 * @param isLargeText Whether the text is considered large (18pt+ or 14pt+ bold)
 * @return Minimum required contrast ratio
 */
double getMinimumContrastRatio(WcagLevel level, bool isLargeText = false);

/**
 * @brief Adjust color for better contrast
 *
 * @param color Color to adjust
 * @param background Background color
 * @param targetRatio Target contrast ratio
 * @return Adjusted color with improved contrast
 */
QColor adjustColorContrast(const QColor& color, const QColor& background,
                           double targetRatio = 4.5);

/**
 * @brief Announce text to screen readers
 *
 * @param text Text to announce
 * @param priority Announcement priority (polite/assertive)
 */
void announceToScreenReader(const QString& text,
                            const QString& priority = "polite");

/**
 * @brief Set focus to a widget with proper announcement
 *
 * @param widget Widget to focus
 * @param reason Focus reason for screen readers
 */
void setAccessibleFocus(QWidget* widget, const QString& reason = QString());

/**
 * @brief Create a live region for dynamic content
 *
 * @param widget Widget containing dynamic content
 * @param politeness Live region politeness (polite/assertive)
 */
void createLiveRegion(QWidget* widget, const QString& politeness = "polite");

/**
 * @brief Update live region content
 *
 * @param widget Live region widget
 * @param content New content to announce
 */
void updateLiveRegion(QWidget* widget, const QString& content);

/**
 * @brief Validate widget accessibility
 *
 * @param widget Widget to validate
 * @param level WCAG compliance level to check
 * @return List of accessibility issues found
 */
QStringList validateWidgetAccessibility(QWidget* widget,
                                        WcagLevel level = WcagLevel::AA);

/**
 * @brief Generate accessibility report for an application
 *
 * @param rootWidget Root widget to analyze (usually main window)
 * @param level WCAG compliance level to check
 * @return Detailed accessibility report
 */
QString generateAccessibilityReport(QWidget* rootWidget,
                                    WcagLevel level = WcagLevel::AA);

}  // namespace FluentQt::Accessibility
