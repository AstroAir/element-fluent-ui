// include/FluentQt/Styling/FluentAdvancedThemeManager.h
#pragma once

#include <QObject>
#include <QColor>
#include <QTimer>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSettings>
#include <QMutex>
#include <QFileSystemWatcher>
#include <QVector3D>
#include <functional>
#include <memory>

#include "FluentTheme.h"

namespace FluentQt::Styling {

// Theme generation algorithms
enum class FluentThemeGenerationAlgorithm {
    MaterialDesign,     // Material Design color generation
    FluentDesign,       // Fluent Design color generation
    Accessible,         // Accessibility-focused generation
    Monochromatic,      // Single hue variations
    Analogous,          // Adjacent hues
    Complementary,      // Opposite hues
    Triadic,            // Three evenly spaced hues
    Custom              // Custom algorithm
};

// Color harmony types
enum class FluentColorHarmony {
    Monochromatic,      // Variations of single hue
    Analogous,          // Adjacent colors on color wheel
    Complementary,      // Opposite colors
    SplitComplementary, // Base + two adjacent to complement
    Triadic,            // Three evenly spaced colors
    Tetradic,           // Four colors forming rectangle
    Square,             // Four evenly spaced colors
    Custom              // Custom harmony
};

// Theme adaptation modes
enum class FluentThemeAdaptation {
    None,               // No adaptation
    TimeOfDay,          // Adapt based on time
    AmbientLight,       // Adapt based on ambient light
    UserActivity,       // Adapt based on user activity
    ContentType,        // Adapt based on content
    Performance,        // Adapt based on performance
    Accessibility,      // Adapt based on accessibility needs
    Custom              // Custom adaptation logic
};

// Color space for calculations
enum class FluentColorSpace {
    RGB,                // RGB color space
    HSV,                // HSV color space
    HSL,                // HSL color space
    LAB,                // LAB color space
    LCH,                // LCH color space
    OKLCH               // OKLCH color space
};

// Theme configuration for generation
struct FluentThemeGenerationConfig {
    FluentThemeGenerationAlgorithm algorithm{FluentThemeGenerationAlgorithm::FluentDesign};
    FluentColorHarmony harmony{FluentColorHarmony::Analogous};
    FluentColorSpace colorSpace{FluentColorSpace::HSL};
    QColor baseColor{0, 120, 215}; // Default blue
    double saturationRange{0.3};   // Saturation variation range
    double lightnessRange{0.4};    // Lightness variation range
    double contrastRatio{4.5};     // Target contrast ratio
    bool generateAccessibleVariants{true};
    bool generateSemanticColors{true};
    bool generateChartColors{true};
    int chartColorCount{12};
    bool respectUserPreferences{true};
    QMap<QString, QVariant> customParameters;
};

// Dynamic theme configuration
struct FluentDynamicThemeConfig {
    FluentThemeAdaptation adaptationMode{FluentThemeAdaptation::None};
    int updateInterval{300000};    // 5 minutes in milliseconds
    bool enableSmoothing{true};
    int smoothingDuration{1000};   // 1 second
    double adaptationSensitivity{0.5}; // 0.0 to 1.0
    bool enablePredictiveAdaptation{false};
    QMap<QString, QVariant> adaptationParameters;
};

// Theme validation result
struct FluentThemeValidationResult {
    bool isValid{true};
    QStringList errors;
    QStringList warnings;
    QStringList suggestions;
    double accessibilityScore{100.0}; // 0-100
    double performanceScore{100.0};   // 0-100
    QMap<QString, double> contrastRatios;
};

// Advanced theme manager
class FluentAdvancedThemeManager : public QObject {
    Q_OBJECT

public:
    static FluentAdvancedThemeManager& instance();
    
    // Theme generation
    FluentColorPalette generateColorPalette(const FluentThemeGenerationConfig& config);
    FluentColorPalette generateColorPalette(const QColor& baseColor, 
                                          FluentColorHarmony harmony = FluentColorHarmony::Analogous);
    QList<QColor> generateColorHarmony(const QColor& baseColor, FluentColorHarmony harmony, int count = 5);
    
    // Semantic color generation
    QColor generateSemanticColor(const QString& semantic, const QColor& baseColor);
    QMap<QString, QColor> generateSemanticColors(const QColor& baseColor);
    QList<QColor> generateChartColors(const QColor& baseColor, int count = 12);
    QList<QColor> generateDataVisualizationColors(const QColor& baseColor, int count = 8);
    
    // Color manipulation and analysis
    QColor adjustColorForContrast(const QColor& color, const QColor& background, double targetRatio = 4.5);
    double calculateColorContrast(const QColor& foreground, const QColor& background);
    QColor findOptimalTextColor(const QColor& background);
    QColor blendColors(const QColor& color1, const QColor& color2, double ratio = 0.5);
    QColor adjustSaturation(const QColor& color, double factor);
    QColor adjustLightness(const QColor& color, double factor);
    QColor adjustHue(const QColor& color, double degrees);
    
    // Color space conversions
    QVector3D rgbToHsl(const QColor& color);
    QColor hslToRgb(const QVector3D& hsl);
    QVector3D rgbToHsv(const QColor& color);
    QColor hsvToRgb(const QVector3D& hsv);
    QVector3D rgbToLab(const QColor& color);
    QColor labToRgb(const QVector3D& lab);
    
    // Theme validation
    FluentThemeValidationResult validateTheme(const FluentColorPalette& palette);
    bool validateAccessibility(const FluentColorPalette& palette, double minContrast = 4.5);
    QStringList checkColorContrasts(const FluentColorPalette& palette);
    double calculateAccessibilityScore(const FluentColorPalette& palette);
    
    // Dynamic theming
    void enableDynamicTheming(const FluentDynamicThemeConfig& config);
    void disableDynamicTheming();
    bool isDynamicThemingEnabled() const { return m_dynamicThemingEnabled; }
    void updateDynamicTheme();
    void setAdaptationSensitivity(double sensitivity);
    
    // Theme presets
    void registerThemePreset(const QString& name, const FluentThemeGenerationConfig& config);
    void unregisterThemePreset(const QString& name);
    void registerDefaultPresets();
    QStringList getThemePresets() const;
    FluentThemeGenerationConfig getThemePreset(const QString& name) const;
    void applyThemePreset(const QString& name);
    
    // Theme import/export
    QString exportTheme(const FluentColorPalette& palette, const QString& name = "Custom");
    FluentColorPalette importTheme(const QString& themeData);
    bool saveThemeToFile(const FluentColorPalette& palette, const QString& filePath);
    FluentColorPalette loadThemeFromFile(const QString& filePath);
    
    // Theme interpolation and animation
    FluentColorPalette interpolateThemes(const FluentColorPalette& from, 
                                       const FluentColorPalette& to, double progress);
    void animateThemeTransition(const FluentColorPalette& targetPalette, int duration = 1000);
    void setThemeTransitionEasing(const QString& easing);
    
    // Advanced color utilities
    QColor getComplementaryColor(const QColor& color);
    QList<QColor> getAnalogousColors(const QColor& color, int count = 3);
    QList<QColor> getTriadicColors(const QColor& color);
    QList<QColor> getTetradicColors(const QColor& color);
    QColor getContrastingColor(const QColor& color, double minContrast = 4.5);
    
    // Performance optimization
    void enableColorCaching(bool enabled = true);
    void clearColorCache();
    void setMaxCacheSize(int size);
    int getCacheSize() const;
    
    // Accessibility features
    void enableAccessibilityMode(bool enabled = true);
    void setMinimumContrastRatio(double ratio);
    void enableColorBlindnessSimulation(bool enabled = true);
    QColor simulateColorBlindness(const QColor& color, const QString& type);
    
    // System integration
    void enableSystemThemeTracking(bool enabled = true);
    void enableSystemAccentColorTracking(bool enabled = true);
    QColor getSystemAccentColor() const;
    bool isSystemDarkMode() const;

signals:
    void colorPaletteGenerated(const FluentColorPalette& palette);
    void themeValidated(const FluentThemeValidationResult& result);
    void dynamicThemeUpdated(const FluentColorPalette& palette);
    void themePresetApplied(const QString& name);
    void themeTransitionStarted();
    void themeTransitionFinished();
    void systemThemeChanged();
    void systemAccentColorChanged(const QColor& color);
    void accessibilityModeChanged(bool enabled);

private slots:
    void onDynamicThemeTimer();
    void onSystemThemeChanged();
    void onThemeTransitionStep();

private:
    FluentAdvancedThemeManager();
    
    // Color generation algorithms
    FluentColorPalette generateMaterialDesignPalette(const QColor& baseColor);
    FluentColorPalette generateFluentDesignPalette(const QColor& baseColor);
    FluentColorPalette generateAccessiblePalette(const QColor& baseColor, double targetContrast);
    FluentColorPalette generateMonochromaticPalette(const QColor& baseColor);
    FluentColorPalette generateAnalogousPalette(const QColor& baseColor);
    FluentColorPalette generateComplementaryPalette(const QColor& baseColor);
    FluentColorPalette generateTriadicPalette(const QColor& baseColor);
    
    // Color harmony calculations
    QList<QColor> calculateMonochromaticHarmony(const QColor& baseColor, int count);
    QList<QColor> calculateAnalogousHarmony(const QColor& baseColor, int count);
    QList<QColor> calculateComplementaryHarmony(const QColor& baseColor);
    QList<QColor> calculateTriadicHarmony(const QColor& baseColor);
    QList<QColor> calculateTetradicHarmony(const QColor& baseColor);
    
    // Dynamic theme adaptation
    void adaptToTimeOfDay();
    void adaptToAmbientLight();
    void adaptToUserActivity();
    void adaptToContentType();
    void adaptToPerformance();
    void adaptToAccessibility();
    
    // Validation helpers
    bool checkContrastRatio(const QColor& foreground, const QColor& background, double minRatio);
    double calculateLuminance(const QColor& color);
    bool isColorAccessible(const QColor& color, const QColor& background);
    
    // Caching
    QString generateCacheKey(const QColor& baseColor, FluentColorHarmony harmony, int count);
    void addToCache(const QString& key, const QList<QColor>& colors);
    bool getFromCache(const QString& key, QList<QColor>& colors);
    
    // System integration helpers
    void connectToSystemTheme();
    void updateFromSystemTheme();

private:
    // Configuration
    FluentDynamicThemeConfig m_dynamicConfig;
    bool m_dynamicThemingEnabled{false};
    bool m_accessibilityModeEnabled{false};
    bool m_colorCachingEnabled{true};
    double m_minimumContrastRatio{4.5};
    int m_maxCacheSize{1000};
    
    // Theme presets
    QMap<QString, FluentThemeGenerationConfig> m_themePresets;
    
    // Dynamic theming
    QTimer* m_dynamicThemeTimer;
    QTimer* m_transitionTimer;
    FluentColorPalette m_currentPalette;
    FluentColorPalette m_targetPalette;
    double m_transitionProgress{0.0};
    int m_transitionDuration{1000};
    QString m_transitionEasing{"ease-in-out"};
    
    // Caching
    QMap<QString, QList<QColor>> m_colorCache;
    mutable QMutex m_cacheMutex;
    
    // System integration
    QFileSystemWatcher* m_systemWatcher;
    bool m_systemThemeTrackingEnabled{false};
    bool m_systemAccentColorTrackingEnabled{false};
    
    // Performance monitoring
    QMap<QString, std::chrono::steady_clock::time_point> m_performanceMetrics;
    
    // Thread safety
    mutable QMutex m_themeMutex;
};

// Utility functions
namespace FluentThemeUtils {
    // Color analysis
    double getColorBrightness(const QColor& color);
    bool isColorDark(const QColor& color);
    bool isColorLight(const QColor& color);
    QColor getReadableTextColor(const QColor& backgroundColor);
    
    // Color generation
    QColor generateRandomColor();
    QColor generateRandomColor(double minSaturation, double maxSaturation, 
                              double minLightness, double maxLightness);
    QList<QColor> generateGradientColors(const QColor& start, const QColor& end, int steps);
    
    // Accessibility helpers
    bool meetsWCAGAA(const QColor& foreground, const QColor& background);
    bool meetsWCAGAAA(const QColor& foreground, const QColor& background);
    double getWCAGContrastRatio(const QColor& foreground, const QColor& background);
    
    // Color space utilities
    QColor convertColorSpace(const QColor& color, FluentColorSpace fromSpace, FluentColorSpace toSpace);
    double calculateColorDistance(const QColor& color1, const QColor& color2, FluentColorSpace space);
    QColor findClosestColor(const QColor& target, const QList<QColor>& palette);
}

} // namespace FluentQt::Styling
