/**
 * @file FluentQt.h
 * @brief Main header file for the Element Fluent UI library
 * 
 * This header provides convenient access to all core FluentQt components,
 * styling, and animation systems. Include this file to get started with
 * the complete FluentQt library.
 * 
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#pragma once

#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Core/FluentState.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Core/FluentResponsive.h"

// Core Styling System
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Styling/FluentAdvancedThemeManager.h"

// Animation System
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Animation/FluentAnimationPerformanceManager.h"

// Essential Form Components
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentRadioButton.h"
#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Components/FluentSlider.h"
#include "FluentQt/Components/FluentToggleSwitch.h"
#include "FluentQt/Components/FluentSpinBox.h"

// Layout & Container Components
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentNavigationView.h"
#include "FluentQt/Components/FluentTabView.h"
#include "FluentQt/Components/FluentSplitter.h"
#include "FluentQt/Components/FluentScrollArea.h"

// Feedback & Data Components
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentLoadingIndicator.h"
#include "FluentQt/Components/FluentNotification.h"
#include "FluentQt/Components/FluentToast.h"
#include "FluentQt/Components/FluentTooltip.h"
#include "FluentQt/Components/FluentBadge.h"
#include "FluentQt/Components/FluentRating.h"

// Specialized Components
#include "FluentQt/Components/FluentCalendar.h"
#include "FluentQt/Components/FluentDatePicker.h"
#include "FluentQt/Components/FluentTimePicker.h"
#include "FluentQt/Components/FluentColorPicker.h"
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Components/FluentRichTextEditor.h"

// Advanced Components
#include "FluentQt/Components/FluentCarousel.h"
#include "FluentQt/Components/FluentTimeline.h"
#include "FluentQt/Components/FluentContextMenu.h"
#include "FluentQt/Components/FluentBreadcrumb.h"
#include "FluentQt/Components/FluentAccordion.h"

// Accessibility Support
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Accessibility/FluentAccessibilityManager.h"

/**
 * @namespace FluentQt
 * @brief Main namespace for all FluentQt components and utilities
 * 
 * The FluentQt namespace contains all classes, enums, and functions
 * provided by the Element Fluent UI library. It is organized into
 * several sub-namespaces:
 * 
 * - FluentQt::Core - Core functionality and base classes
 * - FluentQt::Components - UI components
 * - FluentQt::Styling - Theme and styling system
 * - FluentQt::Animation - Animation and transition system
 * - FluentQt::Accessibility - Accessibility support
 */
namespace FluentQt {

/**
 * @brief Initialize the FluentQt library
 * 
 * This function should be called once at the beginning of your application
 * to initialize the FluentQt library. It sets up the theme system,
 * registers custom types, and configures default settings.
 * 
 * @param app Pointer to the QApplication instance
 * @return true if initialization was successful, false otherwise
 * 
 * @code
 * #include <QApplication>
 * #include "FluentQt/FluentQt.h"
 * 
 * int main(int argc, char *argv[]) {
 *     QApplication app(argc, argv);
 *     
 *     // Initialize FluentQt
 *     if (!FluentQt::initialize(&app)) {
 *         qCritical() << "Failed to initialize FluentQt";
 *         return -1;
 *     }
 *     
 *     // Your application code here
 *     
 *     return app.exec();
 * }
 * @endcode
 */
bool initialize(QApplication* app = nullptr);

/**
 * @brief Get the current version of FluentQt
 * @return Version string in format "major.minor.patch"
 */
QString version();

/**
 * @brief Get the build information
 * @return Build information including compiler, Qt version, and build date
 */
QString buildInfo();

/**
 * @brief Check if FluentQt is properly initialized
 * @return true if the library has been initialized, false otherwise
 */
bool isInitialized();

/**
 * @brief Cleanup FluentQt resources
 * 
 * This function should be called before your application exits to
 * properly cleanup FluentQt resources. It's automatically called
 * when the QApplication is destroyed if you used initialize().
 */
void cleanup();

} // namespace FluentQt

// Convenience macros for common operations
#define FLUENT_THEME() FluentQt::Styling::FluentTheme::instance()
#define FLUENT_ANIMATOR(parent) new FluentQt::Animation::FluentAnimator(parent)

// Version information
#define FLUENTQT_VERSION_MAJOR 1
#define FLUENTQT_VERSION_MINOR 0
#define FLUENTQT_VERSION_PATCH 0
#define FLUENTQT_VERSION_STRING "1.0.0"

// Feature detection macros
#ifdef QT_CHARTS_LIB
#define FLUENTQT_CHARTS_AVAILABLE
#endif

#ifdef QT_MULTIMEDIA_LIB
#define FLUENTQT_MULTIMEDIA_AVAILABLE
#endif

#ifdef QT_NETWORK_LIB
#define FLUENTQT_NETWORK_AVAILABLE
#endif
