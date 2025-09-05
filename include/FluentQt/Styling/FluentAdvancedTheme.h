// include/FluentQt/Styling/FluentAdvancedTheme.h
#pragma once

#include <QColor>
#include <QFileSystemWatcher>
#include <QFont>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVariant>
#include <functional>
#include <memory>
#include <unordered_map>

// Include FluentTheme.h for FluentThemeMode enum
#include "FluentTheme.h"

namespace FluentQt::Styling {

// Design token types
enum class FluentTokenType {
    Color,
    Size,
    Spacing,
    Typography,
    Shadow,
    Border,
    Animation,
    Breakpoint,
    ZIndex,
    Opacity,
    Custom
};

// Theme modes - using definition from FluentTheme.h to avoid conflicts

// Animation preferences
enum class FluentAnimationPreference {
    Full,     // All animations enabled
    Reduced,  // Reduced animations for accessibility
    None      // No animations
};

// Design token definition
struct FluentDesignToken {
    QString name;
    QVariant value;
    FluentTokenType type;
    QString category;
    QString description;
    QVariant fallback;
    std::function<QVariant(const QVariant&)> transformer;
    QStringList aliases;
    bool isComputed{false};
    QString computeExpression;
};

// Theme configuration
struct FluentThemeConfig {
    QString name;
    QString version;
    QString description;
    FluentThemeMode mode{FluentThemeMode::Light};
    QString baseTheme;  // Inherit from another theme
    QJsonObject tokens;
    QJsonObject components;
    QJsonObject customProperties;
    bool isDynamic{false};
    QString sourceFile;
};

// Component theme definition
struct FluentComponentTheme {
    QString componentName;
    std::unordered_map<QString, QVariant> properties;
    std::unordered_map<QString, QString> stateStyles;
    QJsonObject customCSS;
    bool inheritFromBase{true};
};

// Forward declaration - FluentAdvancedThemeManager is defined in
// FluentAdvancedThemeManager.h
class FluentAdvancedThemeManager;

// Design token utilities
class FluentDesignTokens {
public:
    // Color tokens
    static FluentDesignToken colorToken(const QString& name,
                                        const QColor& color,
                                        const QString& description = "");
    static FluentDesignToken colorPalette(const QString& baseName,
                                          const QColor& baseColor,
                                          const QList<int>& shades = {
                                              50, 100, 200, 300, 400, 500, 600,
                                              700, 800, 900});

    // Size tokens
    static FluentDesignToken sizeToken(const QString& name, int size,
                                       const QString& unit = "px");
    static FluentDesignToken spacingScale(const QString& baseName,
                                          int baseSize = 8);

    // Typography tokens
    static FluentDesignToken fontToken(const QString& name, const QFont& font);
    static FluentDesignToken typographyScale(const QString& baseName,
                                             const QFont& baseFont);

    // Shadow tokens
    static FluentDesignToken shadowToken(const QString& name,
                                         const QString& shadowCSS);

    // Animation tokens
    static FluentDesignToken animationToken(const QString& name, int duration,
                                            const QString& easing = "ease-out");

    // Computed tokens
    static FluentDesignToken computedToken(
        const QString& name, const QString& expression,
        FluentTokenType type = FluentTokenType::Custom);
};

// CSS-in-JS generator
class FluentCSSGenerator {
public:
    explicit FluentCSSGenerator(FluentAdvancedThemeManager* themeManager);

    // CSS generation
    QString generateComponentCSS(const QString& componentName) const;
    QString generateUtilityCSS() const;
    QString generateAnimationCSS() const;
    QString generateResponsiveCSS() const;

    // CSS utilities
    QString colorToCSS(const QColor& color) const;
    QString fontToCSS(const QFont& font) const;
    QString spacingToCSS(const QVariant& spacing) const;

    // CSS optimization
    void enableMinification(bool enabled = true) { m_minifyCSS = enabled; }
    void enableAutoprefixer(bool enabled = true) { m_autoprefix = enabled; }

    // CSS variables
    QString generateCSSVariables() const;
    QString generateCSSCustomProperties() const;

private:
    QString minifyCSS(const QString& css) const;
    QString addVendorPrefixes(const QString& css) const;

private:
    FluentAdvancedThemeManager* m_themeManager;
    bool m_minifyCSS{false};
    bool m_autoprefix{true};
};

// Theme builder for creating themes programmatically
class FluentThemeBuilder {
public:
    FluentThemeBuilder(const QString& themeName);

    // Basic configuration
    FluentThemeBuilder& setMode(FluentThemeMode mode);
    FluentThemeBuilder& setDescription(const QString& description);
    FluentThemeBuilder& setVersion(const QString& version);
    FluentThemeBuilder& inheritFrom(const QString& baseTheme);

    // Token configuration
    FluentThemeBuilder& addToken(const FluentDesignToken& token);
    FluentThemeBuilder& addColorToken(const QString& name, const QColor& color);
    FluentThemeBuilder& addSizeToken(const QString& name, int size);
    FluentThemeBuilder& addFontToken(const QString& name, const QFont& font);

    // Component configuration
    FluentThemeBuilder& setComponentProperty(const QString& component,
                                             const QString& property,
                                             const QVariant& value);
    FluentThemeBuilder& setComponentCSS(const QString& component,
                                        const QString& css);

    // Custom properties
    FluentThemeBuilder& setCustomProperty(const QString& name,
                                          const QVariant& value);

    // Build theme
    FluentThemeConfig build() const;
    void registerTheme() const;

private:
    FluentThemeConfig m_config;
    std::unordered_map<QString, FluentDesignToken> m_tokens;
    std::unordered_map<QString, FluentComponentTheme> m_componentThemes;
};

// Theme animation controller
class FluentThemeAnimator : public QObject {
    Q_OBJECT

public:
    explicit FluentThemeAnimator(QObject* parent = nullptr);

    void animateThemeTransition(const QString& fromTheme,
                                const QString& toTheme, int duration = 300);
    void animateTokenChange(const QString& tokenName, const QVariant& fromValue,
                            const QVariant& toValue, int duration = 200);

    void setEasingCurve(const QString& easing) { m_easingCurve = easing; }
    QString getEasingCurve() const { return m_easingCurve; }

signals:
    void animationStarted();
    void animationFinished();
    void animationProgress(double progress);

private slots:
    void updateAnimation();

private:
    QTimer* m_animationTimer;
    QString m_easingCurve{"ease-out"};
    int m_animationDuration{300};
    double m_animationProgress{0.0};
};

// Convenience macros
#define FLUENT_TOKEN(name) \
    FluentQt::Styling::FluentAdvancedThemeManager::instance().getToken(name)

#define FLUENT_COLOR_TOKEN(name)                              \
    FluentQt::Styling::FluentAdvancedThemeManager::instance() \
        .getToken(name)                                       \
        .value<QColor>()

#define FLUENT_SIZE_TOKEN(name)                               \
    FluentQt::Styling::FluentAdvancedThemeManager::instance() \
        .getToken(name)                                       \
        .toInt()

#define FLUENT_FONT_TOKEN(name)                               \
    FluentQt::Styling::FluentAdvancedThemeManager::instance() \
        .getToken(name)                                       \
        .value<QFont>()

}  // namespace FluentQt::Styling
