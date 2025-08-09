// include/FluentQt/Styling/FluentTheme.h
#pragma once

#include <QObject>
#include <QColor>
#include <QFont>
#include <QBrush>
#include <QPalette>
#include <QSettings>
#include <QTimer>
#include <unordered_map>
#include <string_view>

namespace FluentQt::Styling {

// Enhanced theme modes
enum class FluentThemeMode {
    Light,
    Dark,
    System,         // Follow system preference
    HighContrast,   // High contrast mode
    HighContrastLight, // High contrast light
    HighContrastDark,  // High contrast dark
    Custom,         // Custom theme mode
    Auto            // Automatic based on time/location
};

// Enhanced accent colors with more options
enum class FluentAccentColor {
    Blue,
    Purple,
    Pink,
    Red,
    Orange,
    Yellow,
    Green,
    Teal,
    Cyan,
    Indigo,
    Lime,
    Amber,
    DeepOrange,
    Brown,
    BlueGrey,
    Custom,         // Custom color
    System,         // System accent color
    Dynamic         // Dynamic color based on content
};

// Theme variants for different use cases
enum class FluentThemeVariant {
    Default,        // Standard theme
    Compact,        // Compact spacing and sizing
    Touch,          // Touch-optimized
    Dense,          // Dense information display
    Accessible,     // Enhanced accessibility
    Gaming,         // Gaming-optimized
    Productivity,   // Productivity-focused
    Creative,       // Creative work optimized
    Presentation,   // Presentation mode
    Custom          // Custom variant
};

// Color scheme types
enum class FluentColorScheme {
    Monochromatic,  // Single hue variations
    Analogous,      // Adjacent hues
    Complementary,  // Opposite hues
    Triadic,        // Three evenly spaced hues
    Tetradic,       // Four hues (rectangle)
    SplitComplementary, // Base + two adjacent to complement
    Custom,         // Custom color scheme
    Material,       // Material Design inspired
    Fluent,         // Fluent Design inspired
    Accessible      // Accessibility-focused
};

// Dynamic theming modes
enum class FluentDynamicMode {
    Static,         // No dynamic changes
    TimeOfDay,      // Changes based on time
    Location,       // Changes based on location/weather
    Content,        // Changes based on content
    Usage,          // Changes based on usage patterns
    Ambient,        // Changes based on ambient light
    Custom          // Custom dynamic logic
};

struct FluentColorPalette {
    // Primary colors
    QColor accent;
    QColor accentLight1;
    QColor accentLight2; 
    QColor accentLight3;
    QColor accentDark1;
    QColor accentDark2;
    QColor accentDark3;

    // Neutral colors
    QColor neutralLightest;
    QColor neutralLighter;
    QColor neutralLight;
    QColor neutralQuaternaryAlt;
    QColor neutralQuaternary;
    QColor neutralTertiaryAlt;
    QColor neutralTertiary;
    QColor neutralSecondaryAlt;
    QColor neutralSecondary;
    QColor neutralPrimaryAlt;
    QColor neutralPrimary;
    QColor neutralDark;
    QColor neutralDarker;
    QColor neutralDarkest;

    // Semantic colors
    QColor error;
    QColor errorLight;
    QColor errorDark;
    QColor warning;
    QColor warningLight;
    QColor warningDark;
    QColor success;
    QColor successLight;
    QColor successDark;
    QColor info;
    QColor infoLight;
    QColor infoDark;

    // Interactive states
    QColor hover;
    QColor pressed;
    QColor selected;
    QColor focus;
    QColor disabled;
    QColor loading;

    // Surface colors
    QColor surface;
    QColor surfaceSecondary;
    QColor surfaceTertiary;
    QColor overlay;
    QColor modal;
    QColor tooltip;

    // Shadow colors
    QColor shadowLight;
    QColor shadowMedium;
    QColor shadowDark;
    QColor shadowKey;
    QColor shadowAmbient;

    // Border colors
    QColor border;
    QColor borderSecondary;
    QColor borderFocus;
    QColor borderHover;
    QColor borderActive;
    QColor borderDisabled;

    // Accessibility colors
    QColor highContrastText;
    QColor highContrastBackground;
    QColor focusIndicator;

    // Chart/visualization colors
    QList<QColor> chartColors;
    QList<QColor> dataColors;

    // Custom extensible colors
    QMap<QString, QColor> customColors;
};

class FluentTheme : public QObject {
    Q_OBJECT
    Q_PROPERTY(FluentThemeMode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(FluentAccentColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged)
    Q_PROPERTY(FluentThemeVariant variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(FluentColorScheme colorScheme READ colorScheme WRITE setColorScheme NOTIFY colorSchemeChanged)
    Q_PROPERTY(FluentDynamicMode dynamicMode READ dynamicMode WRITE setDynamicMode NOTIFY dynamicModeChanged)
    Q_PROPERTY(bool highContrastMode READ isHighContrastMode WRITE setHighContrastMode NOTIFY highContrastModeChanged)
    Q_PROPERTY(bool reducedMotionMode READ isReducedMotionMode WRITE setReducedMotionMode NOTIFY reducedMotionModeChanged)
    Q_PROPERTY(double contrastRatio READ contrastRatio WRITE setContrastRatio NOTIFY contrastRatioChanged)

public:
    static FluentTheme& instance();

    // Theme mode
    FluentThemeMode mode() const noexcept { return m_mode; }
    void setMode(FluentThemeMode mode);

    // Convenience methods for dark mode detection
    bool isDarkMode() const noexcept;
    bool isLightMode() const noexcept;
    void setDarkMode(bool dark);
    void toggleThemeMode();

    // Accent color
    FluentAccentColor accentColor() const noexcept { return m_accentColor; }
    void setAccentColor(FluentAccentColor color);
    void setCustomAccentColor(const QColor& color);

    // Theme variant
    FluentThemeVariant variant() const noexcept { return m_variant; }
    void setVariant(FluentThemeVariant variant);

    // Color scheme
    FluentColorScheme colorScheme() const noexcept { return m_colorScheme; }
    void setColorScheme(FluentColorScheme scheme);

    // Dynamic theming
    FluentDynamicMode dynamicMode() const noexcept { return m_dynamicMode; }
    void setDynamicMode(FluentDynamicMode mode);
    void enableDynamicTheming(bool enabled = true);
    bool isDynamicThemingEnabled() const noexcept { return m_dynamicThemingEnabled; }

    // Accessibility features
    bool isHighContrastMode() const noexcept { return m_highContrastMode; }
    void setHighContrastMode(bool enabled);
    bool isReducedMotionMode() const noexcept { return m_reducedMotionMode; }
    void setReducedMotionMode(bool enabled);
    double contrastRatio() const noexcept { return m_contrastRatio; }
    void setContrastRatio(double ratio);

    // Advanced color management
    void generateColorPalette(const QColor& baseColor, FluentColorScheme scheme = FluentColorScheme::Fluent);
    void generateAccessiblePalette(double targetContrastRatio = 4.5);
    QColor generateContrastColor(const QColor& baseColor, double targetRatio = 4.5);
    bool meetsAccessibilityStandards(const QColor& foreground, const QColor& background) const;
    double calculateContrastRatio(const QColor& foreground, const QColor& background) const;

    // Color palette access
    const FluentColorPalette& lightPalette() const noexcept { return m_lightPalette; }
    const FluentColorPalette& darkPalette() const noexcept { return m_darkPalette; }
    const FluentColorPalette& currentPalette() const noexcept;

    // Dynamic color access
    QColor color(std::string_view colorName) const;
    QBrush brush(std::string_view brushName) const;
    
    // Typography
    QFont displayFont() const;
    QFont titleLargeFont() const;
    QFont titleFont() const;
    QFont titleSmallFont() const;
    QFont subtitleFont() const;
    QFont bodyLargeFont() const;
    QFont bodyFont() const;
    QFont bodySmallFont() const;
    QFont captionFont() const;
    QFont overlineFont() const;

    // Typography with semantic meaning
    QFont headingFont(int level = 1) const; // H1-H6
    QFont buttonFont() const;
    QFont linkFont() const;
    QFont codeFont() const;

    // Spacing and sizing
    int spacing(std::string_view size) const;
    QSize iconSize(std::string_view size) const;

    // Enhanced spacing system
    int borderRadius(std::string_view size) const;
    int elevation(std::string_view level) const;
    QMargins margins(std::string_view size) const;
    QMargins padding(std::string_view size) const;

    // Layout dimensions
    int componentHeight(std::string_view size) const;
    int componentWidth(std::string_view size) const;
    int strokeWidth(std::string_view weight) const;

    // Custom colors
    void setCustomColor(const QString& name, const QColor& color);
    QColor customColor(const QString& name) const;

    // Accessibility features (duplicate methods removed - already declared above)
    qreal contrastRatio(const QColor& foreground, const QColor& background) const;
    QColor ensureContrast(const QColor& foreground, const QColor& background, qreal minRatio = 4.5) const;

    // Advanced color utilities
    QColor lighten(const QColor& color, qreal amount = 0.1) const;
    QColor darken(const QColor& color, qreal amount = 0.1) const;
    QColor adjustAlpha(const QColor& color, qreal alpha) const;
    QColor blend(const QColor& color1, const QColor& color2, qreal ratio = 0.5) const;

    // Theme management
    void resetToDefaults();
    QString exportTheme() const;
    bool importTheme(const QString& themeData);

signals:
    void modeChanged(FluentThemeMode mode);
    void accentColorChanged(FluentAccentColor color);
    void variantChanged(FluentThemeVariant variant);
    void colorSchemeChanged(FluentColorScheme scheme);
    void dynamicModeChanged(FluentDynamicMode mode);
    void highContrastModeChanged(bool enabled);
    void reducedMotionModeChanged(bool enabled);
    void contrastRatioChanged(double ratio);
    void paletteGenerated(FluentColorScheme scheme);
    void accessibilityFeaturesChanged();
    void themeChanged();
    void dynamicThemeUpdated();
    void customColorAdded(const QString& name, const QColor& color);
    void customColorRemoved(const QString& name);

private slots:
    void onSystemThemeChanged(Qt::ColorScheme colorScheme);

private:
    FluentTheme();
    void initializePalettes();
    void updateAccentColors();
    void loadSettings();
    void saveSettings();
    void connectToSystemTheme();
    void initializeSpacingAndSizing();
    void initializeTypography();
    void initializeBorderRadius();
    void initializeElevation();
    void updateEffectiveMode();
    void loadCustomColors(QSettings& settings);
    void saveCustomColors(QSettings& settings);
    bool isSystemDarkMode() const;
    void updateSystemAccentColor(const QColor& systemColor);
    FluentAccentColor findClosestAccentColor(const QColor& targetColor) const;

    // Brush creation helpers
    QBrush createAcrylicBrush(const QColor& baseColor) const;
    QBrush createMicaBrush() const;
    QBrush createGradientBrush(const QColor& baseColor) const;

#ifdef Q_OS_WIN
    bool isWindowsDarkMode() const;
    void checkWindowsThemeChanges();
    QColor getWindowsAccentColor() const;
#endif

#ifdef Q_OS_MAC
    bool isMacOSDarkMode() const;
#endif

private:
    FluentThemeMode m_mode{FluentThemeMode::System};
    FluentAccentColor m_accentColor{FluentAccentColor::Blue};
    FluentThemeVariant m_variant{FluentThemeVariant::Default};
    FluentColorScheme m_colorScheme{FluentColorScheme::Fluent};
    FluentDynamicMode m_dynamicMode{FluentDynamicMode::Static};
    FluentColorPalette m_lightPalette;
    FluentColorPalette m_darkPalette;
    FluentColorPalette m_highContrastLightPalette;
    FluentColorPalette m_highContrastDarkPalette;

    // Enhanced theme properties
    bool m_highContrastMode{false};
    bool m_reducedMotionMode{false};
    bool m_dynamicThemingEnabled{false};
    double m_contrastRatio{4.5};
    QColor m_customAccentColor;

    // Enhanced design tokens
    std::unordered_map<std::string, int> m_spacingMap;
    std::unordered_map<std::string, QSize> m_iconSizeMap;
    std::unordered_map<std::string, int> m_borderRadiusMap;
    std::unordered_map<std::string, int> m_elevationMap;
    std::unordered_map<std::string, QMargins> m_marginsMap;
    std::unordered_map<std::string, QMargins> m_paddingMap;
    std::unordered_map<std::string, int> m_componentHeightMap;
    std::unordered_map<std::string, int> m_componentWidthMap;
    std::unordered_map<std::string, int> m_strokeWidthMap;

    // Typography maps
    std::unordered_map<std::string, QFont> m_fontMap;

    // Custom colors
    std::unordered_map<std::string, QColor> m_customColors;

    // System integration
    QTimer* m_systemThemeTimer{nullptr};
    bool m_useSystemAccentColor{false};
};

} // namespace FluentQt::Styling