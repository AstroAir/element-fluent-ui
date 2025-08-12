/**
 * @file index.h
 * @brief Complete index of all FluentQt headers and components
 *
 * This file provides a comprehensive overview of all available headers
 * in the FluentQt library. Use this as a reference for what's available
 * or include specific category headers for better compile times.
 *
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#pragma once

// ============================================================================
// MAIN LIBRARY HEADERS
// ============================================================================

/**
 * @brief Main FluentQt header - includes core functionality
 *
 * Include this for basic FluentQt usage with essential components.
 * This is the recommended header for most applications.
 */
#include "FluentQt/FluentQt.h"

// ============================================================================
// CATEGORY HEADERS
// ============================================================================

/**
 * @brief All UI components
 *
 * Includes all form controls, layout containers, feedback components,
 * and specialized widgets. Use this when you need access to the
 * complete component library.
 */
#include "FluentQt/Components.h"

/**
 * @brief Styling and theming system
 *
 * Includes theme management, color system, typography, spacing,
 * and responsive design utilities.
 */
#include "FluentQt/Styling.h"

/**
 * @brief Animation system
 *
 * Includes all animation types, easing functions, performance
 * optimization, and micro-interaction utilities.
 */
#include "FluentQt/Animation.h"

/**
 * @brief Accessibility features
 *
 * Includes screen reader support, keyboard navigation, WCAG
 * compliance tools, and accessibility validation.
 */
#include "FluentQt/Accessibility.h"

// ============================================================================
// CORE SYSTEM HEADERS
// ============================================================================

// Core Framework
#include "FluentQt/Core/FluentBundleAnalyzer.h"   ///< Bundle analysis
#include "FluentQt/Core/FluentComponent.h"        ///< Base component class
#include "FluentQt/Core/FluentFluidTypography.h"  ///< Typography system
#include "FluentQt/Core/FluentLazyComponent.h"    ///< Lazy loading
#include "FluentQt/Core/FluentPerformance.h"      ///< Performance monitoring
#include "FluentQt/Core/FluentResponsive.h"       ///< Responsive design
#include "FluentQt/Core/FluentResponsiveComponent.h"  ///< Responsive components
#include "FluentQt/Core/FluentResponsiveLayout.h"     ///< Responsive layouts
#include "FluentQt/Core/FluentState.h"                ///< State management

// Internationalization
#include "FluentQt/Core/FluentCulturalAdaptation.h"  ///< Cultural adaptations
#include "FluentQt/Core/FluentI18n.h"                ///< Internationalization
#include "FluentQt/Core/FluentLocaleFormatting.h"    ///< Locale formatting
#include "FluentQt/Core/FluentRTLSupport.h"          ///< Right-to-left support

// Performance & Optimization
#include "FluentQt/Core/FluentBenchmark.h"        ///< Benchmarking tools
#include "FluentQt/Core/FluentMemoryPool.h"       ///< Memory management
#include "FluentQt/Core/FluentModuleLoader.h"     ///< Dynamic module loading
#include "FluentQt/Core/FluentRenderOptimizer.h"  ///< Rendering optimization

// ============================================================================
// COMPONENT HEADERS (ORGANIZED BY CATEGORY)
// ============================================================================

// Form Components
#include "FluentQt/Components/FluentButton.h"        ///< Button component
#include "FluentQt/Components/FluentCheckBox.h"      ///< Checkbox
#include "FluentQt/Components/FluentComboBox.h"      ///< Combo box
#include "FluentQt/Components/FluentRadioButton.h"   ///< Radio button
#include "FluentQt/Components/FluentSlider.h"        ///< Slider
#include "FluentQt/Components/FluentSpinBox.h"       ///< Spin box
#include "FluentQt/Components/FluentTextInput.h"     ///< Text input
#include "FluentQt/Components/FluentToggleSwitch.h"  ///< Toggle switch

// Layout & Container Components
#include "FluentQt/Components/FluentCard.h"            ///< Card container
#include "FluentQt/Components/FluentGrid.h"            ///< Grid layout
#include "FluentQt/Components/FluentNavigationView.h"  ///< Navigation view
#include "FluentQt/Components/FluentPanel.h"           ///< Panel container
#include "FluentQt/Components/FluentScrollArea.h"      ///< Scroll area
#include "FluentQt/Components/FluentSheet.h"           ///< Modal sheet
#include "FluentQt/Components/FluentSplitter.h"        ///< Splitter
#include "FluentQt/Components/FluentTabView.h"         ///< Tab view

// Feedback & Data Components
#include "FluentQt/Components/FluentBadge.h"             ///< Badge
#include "FluentQt/Components/FluentLoadingIndicator.h"  ///< Loading indicator
#include "FluentQt/Components/FluentNotification.h"      ///< Notification
#include "FluentQt/Components/FluentProgressBar.h"       ///< Progress bar
#include "FluentQt/Components/FluentRating.h"            ///< Rating
#include "FluentQt/Components/FluentSeparator.h"         ///< Separator
#include "FluentQt/Components/FluentToast.h"             ///< Toast message
#include "FluentQt/Components/FluentToastManager.h"      ///< Toast manager
#include "FluentQt/Components/FluentTooltip.h"           ///< Tooltip

// Selection Components
#include "FluentQt/Components/FluentDropdown.h"        ///< Dropdown
#include "FluentQt/Components/FluentSelect.h"          ///< Select component
#include "FluentQt/Components/FluentSelectDropdown.h"  ///< Select dropdown
#include "FluentQt/Components/FluentSelectItem.h"      ///< Select item

// Date & Time Components
#include "FluentQt/Components/FluentCalendar.h"    ///< Calendar
#include "FluentQt/Components/FluentDatePicker.h"  ///< Date picker
#include "FluentQt/Components/FluentTimePicker.h"  ///< Time picker

// Media & Display Components
#include "FluentQt/Components/FluentAvatar.h"       ///< Avatar
#include "FluentQt/Components/FluentColorPicker.h"  ///< Color picker
#include "FluentQt/Components/FluentImageView.h"    ///< Image view

// Data Display Components
#include "FluentQt/Components/FluentRichTextEditor.h"  ///< Rich text editor
#include "FluentQt/Components/FluentTreeView.h"        ///< Tree view

// Advanced Interactive Components
#include "FluentQt/Components/FluentAccordion.h"          ///< Accordion
#include "FluentQt/Components/FluentAutoCarousel.h"       ///< Auto carousel
#include "FluentQt/Components/FluentBasicCarousel.h"      ///< Basic carousel
#include "FluentQt/Components/FluentCarousel.h"           ///< Carousel
#include "FluentQt/Components/FluentIndicatorCarousel.h"  ///< Indicator carousel
#include "FluentQt/Components/FluentTimeline.h"           ///< Timeline
#include "FluentQt/Components/FluentTimelineItem.h"       ///< Timeline item
#include "FluentQt/Components/FluentTouchCarousel.h"      ///< Touch carousel

// Navigation Components
#include "FluentQt/Components/FluentBreadcrumb.h"   ///< Breadcrumb
#include "FluentQt/Components/FluentContextMenu.h"  ///< Context menu

// Dialog & Overlay Components
#include "FluentQt/Components/FluentContentDialog.h"  ///< Content dialog
#include "FluentQt/Components/FluentPopover.h"        ///< Popover

// Utility Components
#include "FluentQt/Components/FluentResizable.h"  ///< Resizable

// Chart Components (optional)
#ifdef FLUENTQT_CHARTS_AVAILABLE
#include "FluentQt/Components/FluentChartView.h"  ///< Chart view
#endif

// ============================================================================
// STYLING SYSTEM HEADERS
// ============================================================================

#include "FluentQt/Styling/FluentAdvancedThemeManager.h"  ///< Advanced theming
#include "FluentQt/Styling/FluentCarouselStyles.h"        ///< Carousel styles
#include "FluentQt/Styling/FluentTheme.h"                 ///< Main theme system

#ifdef FLUENTQT_ADVANCED_THEMING
#include "FluentQt/Styling/FluentAdvancedTheme.h"  ///< Advanced theme features
#endif

// ============================================================================
// ANIMATION SYSTEM HEADERS
// ============================================================================

#include "FluentQt/Animation/FluentAdvancedAnimator.h"  ///< Advanced animations
#include "FluentQt/Animation/FluentAdvancedEasing.h"    ///< Advanced easing
#include "FluentQt/Animation/FluentAnimationBatch.h"    ///< Batch animations
#include "FluentQt/Animation/FluentAnimationPerformanceManager.h"  ///< Performance manager
#include "FluentQt/Animation/FluentAnimator.h"           ///< Main animator
#include "FluentQt/Animation/FluentGPUAnimator.h"        ///< GPU acceleration
#include "FluentQt/Animation/FluentGestureAnimations.h"  ///< Gesture animations
#include "FluentQt/Animation/FluentPhysicsAnimator.h"    ///< Physics animations
#include "FluentQt/Animation/FluentSkeletonAnimator.h"  ///< Skeleton animations

// ============================================================================
// ACCESSIBILITY SYSTEM HEADERS
// ============================================================================

#include "FluentQt/Accessibility/FluentAccessibilityManager.h"  ///< Accessibility manager
#include "FluentQt/Accessibility/FluentAccessible.h"  ///< Accessibility base
#include "FluentQt/Accessibility/FluentAccessibleAnimator.h"  ///< Accessible animations
#include "FluentQt/Accessibility/FluentKeyboardNavigationManager.h"  ///< Keyboard navigation
#include "FluentQt/Accessibility/FluentScreenReaderManager.h"  ///< Screen reader support
#include "FluentQt/Accessibility/FluentWcagChecker.h"  ///< WCAG compliance

// ============================================================================
// PERFORMANCE & TOOLS HEADERS
// ============================================================================

#include "FluentQt/Performance/FluentAdvancedPerformanceAnalytics.h"  ///< Performance analytics
#include "FluentQt/Performance/FluentPerformanceDashboard.h"  ///< Performance dashboard
#include "FluentQt/Performance/FluentSystemIntegration.h"  ///< System integration

#include "FluentQt/Tools/FluentAnimationDesigner.h"  ///< Animation designer tool

#include "FluentQt/Validation/FluentValidator.h"  ///< Input validation

// ============================================================================
// VERSION AND FEATURE INFORMATION
// ============================================================================

/**
 * @def FLUENTQT_VERSION_STRING
 * @brief Current version of FluentQt as a string
 */
#define FLUENTQT_VERSION_STRING "1.0.0"

/**
 * @def FLUENTQT_VERSION_MAJOR
 * @brief Major version number
 */
#define FLUENTQT_VERSION_MAJOR 1

/**
 * @def FLUENTQT_VERSION_MINOR
 * @brief Minor version number
 */
#define FLUENTQT_VERSION_MINOR 0

/**
 * @def FLUENTQT_VERSION_PATCH
 * @brief Patch version number
 */
#define FLUENTQT_VERSION_PATCH 0

// Feature availability macros
#ifdef QT_CHARTS_LIB
#define FLUENTQT_CHARTS_AVAILABLE
#endif

#ifdef QT_MULTIMEDIA_LIB
#define FLUENTQT_MULTIMEDIA_AVAILABLE
#endif

#ifdef QT_NETWORK_LIB
#define FLUENTQT_NETWORK_AVAILABLE
#endif
