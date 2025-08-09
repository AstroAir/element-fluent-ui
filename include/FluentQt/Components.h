/**
 * @file Components.h
 * @brief Comprehensive header for all FluentQt UI components
 * 
 * This header includes all available UI components in the FluentQt library.
 * Use this when you need access to the complete component set, or include
 * individual component headers for better compile times.
 * 
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#pragma once

// Form Components
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
#include "FluentQt/Components/FluentGrid.h"
#include "FluentQt/Components/FluentSheet.h"

// Feedback & Data Components
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentLoadingIndicator.h"
#include "FluentQt/Components/FluentNotification.h"
#include "FluentQt/Components/FluentToast.h"
#include "FluentQt/Components/FluentToastManager.h"
#include "FluentQt/Components/FluentTooltip.h"
#include "FluentQt/Components/FluentBadge.h"
#include "FluentQt/Components/FluentRating.h"
#include "FluentQt/Components/FluentSeparator.h"

// Selection Components
#include "FluentQt/Components/FluentSelect.h"
#include "FluentQt/Components/FluentSelectItem.h"
#include "FluentQt/Components/FluentSelectDropdown.h"
#include "FluentQt/Components/FluentDropdown.h"

// Date & Time Components
#include "FluentQt/Components/FluentCalendar.h"
#include "FluentQt/Components/FluentDatePicker.h"
#include "FluentQt/Components/FluentTimePicker.h"

// Media & Display Components
#include "FluentQt/Components/FluentColorPicker.h"
#include "FluentQt/Components/FluentImageView.h"
#include "FluentQt/Components/FluentAvatar.h"

// Data Display Components
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Components/FluentRichTextEditor.h"

// Advanced Interactive Components
#include "FluentQt/Components/FluentCarousel.h"
#include "FluentQt/Components/FluentBasicCarousel.h"
#include "FluentQt/Components/FluentAutoCarousel.h"
#include "FluentQt/Components/FluentIndicatorCarousel.h"
#include "FluentQt/Components/FluentTouchCarousel.h"
#include "FluentQt/Components/FluentTimeline.h"
#include "FluentQt/Components/FluentTimelineItem.h"
#include "FluentQt/Components/FluentAccordion.h"

// Navigation Components
#include "FluentQt/Components/FluentContextMenu.h"
#include "FluentQt/Components/FluentBreadcrumb.h"

// Dialog & Overlay Components
#include "FluentQt/Components/FluentContentDialog.h"
#include "FluentQt/Components/FluentPopover.h"

// Utility Components
#include "FluentQt/Components/FluentResizable.h"

// Chart Components (if available)
#ifdef FLUENTQT_CHARTS_AVAILABLE
#include "FluentQt/Components/FluentChartView.h"
#endif

/**
 * @namespace FluentQt::Components
 * @brief Namespace containing all UI components
 * 
 * This namespace contains all the UI components provided by FluentQt.
 * Components are organized into several categories:
 * 
 * **Form Components:**
 * - FluentButton - Various button styles and states
 * - FluentTextInput - Text input with validation
 * - FluentCheckBox - Checkbox with tri-state support
 * - FluentRadioButton - Radio button with grouping
 * - FluentComboBox - Dropdown selection
 * - FluentSlider - Range slider and value picker
 * - FluentToggleSwitch - Toggle switch control
 * - FluentSpinBox - Numeric input with spin controls
 * 
 * **Layout & Container Components:**
 * - FluentCard - Content card container
 * - FluentPanel - Versatile panel container
 * - FluentNavigationView - Navigation sidebar
 * - FluentTabView - Tab container
 * - FluentSplitter - Resizable pane splitter
 * - FluentScrollArea - Scrollable content area
 * - FluentGrid - Grid layout container
 * - FluentSheet - Modal sheet overlay
 * 
 * **Feedback & Data Components:**
 * - FluentProgressBar - Progress indicator
 * - FluentLoadingIndicator - Loading animation
 * - FluentNotification - Toast notification
 * - FluentToast - Simple toast message
 * - FluentTooltip - Rich tooltip
 * - FluentBadge - Status badge
 * - FluentRating - Star rating control
 * - FluentSeparator - Visual separator
 * 
 * **Advanced Components:**
 * - FluentCarousel - Image/content carousel
 * - FluentTimeline - Timeline display
 * - FluentAccordion - Collapsible content sections
 * - FluentTreeView - Hierarchical data display
 * - FluentRichTextEditor - Rich text editing
 * - FluentContextMenu - Context menu
 * - FluentBreadcrumb - Navigation breadcrumb
 * 
 * **Date & Time Components:**
 * - FluentCalendar - Calendar picker
 * - FluentDatePicker - Date selection
 * - FluentTimePicker - Time selection
 * 
 * **Media Components:**
 * - FluentColorPicker - Color selection
 * - FluentImageView - Image display
 * - FluentAvatar - User avatar display
 * 
 * All components inherit from FluentComponent and support:
 * - Automatic theming
 * - Accessibility features
 * - Animation support
 * - Responsive design
 * - High DPI scaling
 */
namespace FluentQt::Components {

/**
 * @brief Register all component types with Qt's meta-object system
 * 
 * This function registers all FluentQt component types so they can be
 * used in QML, with QVariant, and in other Qt reflection systems.
 * It's automatically called by FluentQt::initialize().
 */
void registerTypes();

/**
 * @brief Get a list of all available component types
 * @return QStringList containing the names of all component classes
 */
QStringList availableComponents();

/**
 * @brief Create a component by name
 * @param componentName The name of the component class
 * @param parent Optional parent widget
 * @return Pointer to the created component, or nullptr if the name is invalid
 */
QWidget* createComponent(const QString& componentName, QWidget* parent = nullptr);

} // namespace FluentQt::Components
