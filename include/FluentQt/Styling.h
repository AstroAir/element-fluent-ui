/**
 * @file Styling.h
 * @brief Comprehensive header for FluentQt styling and theming system
 *
 * This header includes all styling-related classes and utilities for
 * customizing the appearance of FluentQt components. It provides access
 * to the theme system, color management, typography, and styling utilities.
 *
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#pragma once

// Core Theme System
#include "FluentQt/Styling/FluentAdvancedThemeManager.h"
#include "FluentQt/Styling/FluentTheme.h"

// Component-Specific Styling
#include "FluentQt/Styling/FluentCarouselStyles.h"

// Advanced Theming (if available)
#ifdef FLUENTQT_ADVANCED_THEMING
#include "FluentQt/Styling/FluentAdvancedTheme.h"
#endif

// Core styling support
#include "FluentQt/Core/FluentCulturalAdaptation.h"
#include "FluentQt/Core/FluentFluidTypography.h"
#include "FluentQt/Core/FluentRTLSupport.h"
#include "FluentQt/Core/FluentResponsive.h"

/**
 * @namespace FluentQt::Styling
 * @brief Namespace containing all styling and theming functionality
 *
 * The Styling namespace provides comprehensive theming and styling
 * capabilities for FluentQt components. It includes:
 *
 * **Core Theme System:**
 * - FluentTheme - Main theme management class
 * - FluentAdvancedThemeManager - Advanced theme features
 * - Color palette management
 * - Typography system
 * - Spacing and sizing tokens
 *
 * **Theme Features:**
 * - Light and dark mode support
 * - System theme detection
 * - Custom accent colors
 * - High contrast mode
 * - Accessibility compliance (WCAG 2.1 AA)
 * - RTL (Right-to-Left) language support
 * - Cultural adaptations
 *
 * **Responsive Design:**
 * - Breakpoint system
 * - Adaptive layouts
 * - High DPI scaling
 * - Touch-friendly sizing
 *
 * **Typography:**
 * - Fluent Design typography scale
 * - Font weight and size management
 * - Line height optimization
 * - Localized font preferences
 *
 * **Color System:**
 * - Semantic color tokens
 * - Automatic contrast adjustment
 * - Color accessibility validation
 * - Brand color integration
 *
 * @code
 * // Basic theme usage
 * auto& theme = FluentQt::Styling::FluentTheme::instance();
 * theme.setMode(FluentThemeMode::Dark);
 * theme.setAccentColor(FluentAccentColor::Blue);
 *
 * // Get theme colors
 * QColor background = theme.color(FluentColor::Background);
 * QColor accent = theme.accentColor();
 *
 * // Typography
 * QFont titleFont = theme.titleFont();
 * QFont bodyFont = theme.bodyFont();
 *
 * // Spacing
 * int spacing = theme.spacing(FluentSpacing::Medium);
 * @endcode
 */
namespace FluentQt::Styling {

/**
 * @enum FluentThemeMode
 * @brief Available theme modes
 */
enum class FluentThemeMode {
    Light,        ///< Light theme mode
    Dark,         ///< Dark theme mode
    System,       ///< Follow system preference
    HighContrast  ///< High contrast mode for accessibility
};

/**
 * @enum FluentAccentColor
 * @brief Predefined accent colors following Fluent Design
 */
enum class FluentAccentColor {
    Blue,    ///< Default Windows blue
    Purple,  ///< Purple accent
    Pink,    ///< Pink accent
    Red,     ///< Red accent
    Orange,  ///< Orange accent
    Yellow,  ///< Yellow accent
    Green,   ///< Green accent
    Teal,    ///< Teal accent
    Custom   ///< Custom color (use setCustomAccentColor)
};

/**
 * @enum FluentColor
 * @brief Semantic color tokens
 */
enum class FluentColor {
    // Background colors
    Background,
    BackgroundSecondary,
    BackgroundTertiary,

    // Surface colors
    Surface,
    SurfaceSecondary,
    SurfaceTertiary,

    // Text colors
    TextPrimary,
    TextSecondary,
    TextTertiary,
    TextDisabled,

    // Border colors
    Border,
    BorderSecondary,
    BorderFocus,

    // State colors
    Success,
    Warning,
    Error,
    Info,

    // Interactive colors
    Accent,
    AccentSecondary,
    AccentTertiary,

    // Control colors
    ControlBackground,
    ControlBorder,
    ControlText
};

/**
 * @enum FluentSpacing
 * @brief Spacing scale following Fluent Design
 */
enum class FluentSpacing {
    None = 0,         ///< 0px
    ExtraSmall = 2,   ///< 2px
    Small = 4,        ///< 4px
    Medium = 8,       ///< 8px
    Large = 12,       ///< 12px
    ExtraLarge = 16,  ///< 16px
    XXLarge = 20,     ///< 20px
    XXXLarge = 24     ///< 24px
};

/**
 * @enum FluentCornerRadius
 * @brief Corner radius scale
 */
enum class FluentCornerRadius {
    None = 0,        ///< 0px - Sharp corners
    Small = 2,       ///< 2px - Subtle rounding
    Medium = 4,      ///< 4px - Standard rounding
    Large = 8,       ///< 8px - Prominent rounding
    ExtraLarge = 12  ///< 12px - Maximum rounding
};

/**
 * @brief Initialize the styling system
 *
 * This function initializes the FluentQt styling system, sets up
 * default themes, and configures system integration.
 *
 * @return true if initialization was successful
 */
bool initializeStyling();

/**
 * @brief Apply a theme to the entire application
 *
 * @param mode The theme mode to apply
 * @param accentColor The accent color to use
 */
void applyTheme(FluentThemeMode mode,
                FluentAccentColor accentColor = FluentAccentColor::Blue);

/**
 * @brief Get the current theme mode
 * @return Current FluentThemeMode
 */
FluentThemeMode currentThemeMode();

/**
 * @brief Check if dark mode is currently active
 * @return true if dark mode is active
 */
bool isDarkMode();

/**
 * @brief Check if high contrast mode is currently active
 * @return true if high contrast mode is active
 */
bool isHighContrastMode();

/**
 * @brief Get a color value for the current theme
 * @param color The semantic color to retrieve
 * @return QColor value for the current theme
 */
QColor getColor(FluentColor color);

/**
 * @brief Get a spacing value
 * @param spacing The spacing size to retrieve
 * @return Pixel value for the spacing
 */
int getSpacing(FluentSpacing spacing);

/**
 * @brief Get a corner radius value
 * @param radius The corner radius size to retrieve
 * @return Pixel value for the corner radius
 */
int getCornerRadius(FluentCornerRadius radius);

/**
 * @brief Validate color contrast for accessibility
 *
 * @param foreground Foreground color
 * @param background Background color
 * @param level WCAG compliance level (AA = 4.5, AAA = 7.0)
 * @return true if the contrast ratio meets the specified level
 */
bool validateContrast(const QColor& foreground, const QColor& background,
                      double level = 4.5);

/**
 * @brief Automatically adjust color for better contrast
 *
 * @param color Color to adjust
 * @param background Background color to contrast against
 * @param targetRatio Target contrast ratio
 * @return Adjusted color with improved contrast
 */
QColor adjustContrast(const QColor& color, const QColor& background,
                      double targetRatio = 4.5);

}  // namespace FluentQt::Styling
