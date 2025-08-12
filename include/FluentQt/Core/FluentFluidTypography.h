// include/FluentQt/Core/FluentFluidTypography.h
#pragma once

#include <QFont>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QSize>
#include <QString>
#include <QTimer>
#include <functional>
#include <memory>

namespace FluentQt::Core {

// Typography scale types
enum class FluentTypographyScale {
    MinorSecond,      // 1.067
    MajorSecond,      // 1.125
    MinorThird,       // 1.200
    MajorThird,       // 1.250
    PerfectFourth,    // 1.333
    AugmentedFourth,  // 1.414
    PerfectFifth,     // 1.500
    GoldenRatio,      // 1.618
    Custom            // User-defined ratio
};

// Typography weight mapping
enum class FluentTypographyWeight {
    Thin = 100,
    ExtraLight = 200,
    Light = 300,
    Regular = 400,
    Medium = 500,
    SemiBold = 600,
    Bold = 700,
    ExtraBold = 800,
    Black = 900
};

// Typography style variants
enum class FluentTypographyStyle {
    Display,     // Large display text
    Headline,    // Page/section headlines
    Title,       // Card/component titles
    Subheading,  // Section subheadings
    Body,        // Regular body text
    Caption,     // Small descriptive text
    Overline,    // Small uppercase labels
    Button,      // Button text
    Link,        // Link text
    Code,        // Monospace code text
    Custom       // Custom style
};

// Fluid typography configuration
struct FluentFluidTypographyConfig {
    // Base configuration
    double baseSize{16.0};       // Base font size in px
    double minSize{12.0};        // Minimum font size in px
    double maxSize{72.0};        // Maximum font size in px
    double minViewport{320.0};   // Minimum viewport width in px
    double maxViewport{1920.0};  // Maximum viewport width in px

    // Scaling configuration
    FluentTypographyScale scale{FluentTypographyScale::MinorThird};
    double customScaleRatio{1.25};  // Used when scale is Custom
    bool enableFluidScaling{true};
    bool enableResponsiveLineHeight{true};
    bool enableResponsiveLetterSpacing{true};

    // Line height configuration
    double baseLineHeight{1.5};
    double minLineHeight{1.2};
    double maxLineHeight{1.8};
    double lineHeightScalingFactor{0.1};

    // Letter spacing configuration
    double baseLetterSpacing{0.0};   // em units
    double minLetterSpacing{-0.05};  // em units
    double maxLetterSpacing{0.1};    // em units
    bool enableOpticalSizing{true};

    // Accessibility configuration
    bool respectUserPreferences{true};
    bool enableAccessibilityScaling{true};
    double minAccessibilityScale{1.0};
    double maxAccessibilityScale{2.0};
    bool respectReducedMotion{true};

    // Performance configuration
    bool enableCaching{true};
    int cacheSize{100};
    bool enableDebugMode{false};
};

// Typography style definition
struct FluentTypographyStyleDefinition {
    FluentTypographyStyle style;
    QString name;
    double sizeRatio{1.0};  // Relative to base size
    FluentTypographyWeight weight{FluentTypographyWeight::Regular};
    double lineHeightRatio{1.5};
    double letterSpacingEm{0.0};
    bool isUppercase{false};
    bool isItalic{false};
    QString fontFamily;  // Empty = use default
    QMap<QString, QString> customProperties;
};

// Calculated typography values
struct FluentCalculatedTypography {
    double fontSize;
    double lineHeight;
    double letterSpacing;
    QFont font;
    QString cssClass;
    QMap<QString, QString> cssProperties;
    bool isFluid{true};
    double scaleFactor{1.0};
};

// Fluid typography manager
class FluentFluidTypographyManager : public QObject {
    Q_OBJECT

public:
    static FluentFluidTypographyManager& instance();

    // Configuration
    void setConfig(const FluentFluidTypographyConfig& config);
    FluentFluidTypographyConfig getConfig() const { return m_config; }

    // Typography scale management
    void setTypographyScale(FluentTypographyScale scale);
    void setCustomScaleRatio(double ratio);
    double getScaleRatio() const;
    double getScaleRatio(FluentTypographyScale scale) const;

    // Style definitions
    void registerStyle(const FluentTypographyStyleDefinition& definition);
    void unregisterStyle(FluentTypographyStyle style);
    void registerDefaultStyles();
    FluentTypographyStyleDefinition getStyleDefinition(
        FluentTypographyStyle style) const;
    QList<FluentTypographyStyle> getRegisteredStyles() const;

    // Fluid calculations
    FluentCalculatedTypography calculateFluidTypography(
        FluentTypographyStyle style, const QSize& viewportSize) const;
    FluentCalculatedTypography calculateFluidTypography(
        double baseSize, const QSize& viewportSize) const;

    // Font calculations
    QFont calculateFluidFont(FluentTypographyStyle style,
                             const QSize& viewportSize) const;
    QFont calculateFluidFont(const QFont& baseFont,
                             const QSize& viewportSize) const;
    double calculateFluidFontSize(double baseSize,
                                  const QSize& viewportSize) const;
    double calculateFluidLineHeight(double fontSize,
                                    const QSize& viewportSize) const;
    double calculateFluidLetterSpacing(double fontSize,
                                       const QSize& viewportSize) const;

    // Responsive utilities
    QString generateFluidCSS(FluentTypographyStyle style) const;
    QString generateFluidCSS(double minSize, double maxSize, double minViewport,
                             double maxViewport) const;
    QMap<QString, QString> getFluidCSSProperties(
        FluentTypographyStyle style, const QSize& viewportSize) const;
    QMap<QString, QString> generateCSSProperties(
        const FluentCalculatedTypography& typography,
        const FluentTypographyStyleDefinition& definition) const;

    // Accessibility integration
    void setAccessibilityScaleFactor(double factor);
    double getAccessibilityScaleFactor() const { return m_accessibilityScale; }
    bool respectsUserPreferences() const {
        return m_config.respectUserPreferences;
    }
    void updateFromSystemSettings();

    // Caching
    void clearCache();
    void setCacheEnabled(bool enabled) { m_config.enableCaching = enabled; }
    int getCacheSize() const;

    // Debugging and validation
    void enableDebugMode(bool enabled) { m_config.enableDebugMode = enabled; }
    bool validateConfiguration() const;
    QStringList getValidationErrors() const;
    void dumpTypographyInfo() const;

signals:
    void configurationChanged();
    void styleRegistered(FluentTypographyStyle style);
    void styleUnregistered(FluentTypographyStyle style);
    void accessibilityScaleChanged(double factor);
    void cacheCleared();

private slots:
    void onSystemFontChanged();
    void onAccessibilitySettingsChanged();

private:
    FluentFluidTypographyManager();

    // Calculation helpers
    double interpolate(double min, double max, double minViewport,
                       double maxViewport, double currentViewport) const;
    double clamp(double value, double min, double max) const;
    double applyAccessibilityScaling(double value) const;
    QFont createFontFromDefinition(
        const FluentTypographyStyleDefinition& definition,
        double fontSize) const;

    // Cache management
    QString generateCacheKey(FluentTypographyStyle style,
                             const QSize& viewportSize) const;
    void addToCache(const QString& key,
                    const FluentCalculatedTypography& typography) const;
    bool getFromCache(const QString& key,
                      FluentCalculatedTypography& typography) const;
    void cleanupCache();

    // Validation helpers
    bool isValidSize(double size) const;
    bool isValidViewport(double viewport) const;
    bool isValidRatio(double ratio) const;
    bool validateConfigInternal(
        const FluentFluidTypographyConfig& config) const;

private:
    FluentFluidTypographyConfig m_config;
    QMap<FluentTypographyStyle, FluentTypographyStyleDefinition>
        m_styleDefinitions;

    // Caching
    mutable QMap<QString, FluentCalculatedTypography> m_cache;
    mutable QMutex m_cacheMutex;

    // Accessibility
    double m_accessibilityScale{1.0};
    bool m_systemFontChanged{false};

    // Performance
    QTimer* m_cacheCleanupTimer;

    // Thread safety
    mutable QMutex m_configMutex;
};

// Utility functions
namespace FluentTypographyUtils {
// Convert between units
double pxToEm(double px, double baseFontSize);
double emToPx(double em, double baseFontSize);
double pxToRem(double px, double rootFontSize = 16.0);
double remToPx(double rem, double rootFontSize = 16.0);

// Typography calculations
double calculateOptimalLineHeight(double fontSize);
double calculateOptimalLetterSpacing(double fontSize,
                                     FluentTypographyWeight weight);
double calculateReadingMeasure(double fontSize, double lineHeight);

// Accessibility helpers
bool meetsAccessibilityContrast(const QColor& text, const QColor& background,
                                double fontSize);
double getMinimumTouchTarget(double fontSize);
bool isReadableSize(double fontSize, double viewingDistance = 60.0);  // cm

// CSS generation
QString generateClampCSS(double minValue, double maxValue, double minViewport,
                         double maxViewport, const QString& unit = "px");
QString generateMediaQuery(double minWidth, double maxWidth = -1);
QString generateContainerQuery(double minWidth, double maxWidth = -1);
}  // namespace FluentTypographyUtils

// Convenience macros
#define FLUENT_FLUID_FONT(style, viewport)                   \
    FluentQt::Core::FluentFluidTypographyManager::instance() \
        .calculateFluidFont(style, viewport)

#define FLUENT_FLUID_SIZE(baseSize, viewport)                \
    FluentQt::Core::FluentFluidTypographyManager::instance() \
        .calculateFluidFontSize(baseSize, viewport)

#define FLUENT_REGISTER_TYPOGRAPHY_STYLE(definition)                        \
    FluentQt::Core::FluentFluidTypographyManager::instance().registerStyle( \
        definition)

}  // namespace FluentQt::Core
