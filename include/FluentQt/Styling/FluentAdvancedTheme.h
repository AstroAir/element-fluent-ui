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

// Theme modes
enum class FluentThemeMode { Light, Dark, HighContrast, Custom };

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

// Advanced theme manager with design tokens
class FluentAdvancedThemeManager : public QObject {
    Q_OBJECT

public:
    static FluentAdvancedThemeManager& instance();

    // Theme management
    void registerTheme(const FluentThemeConfig& themeConfig);
    void unregisterTheme(const QString& themeName);
    bool setCurrentTheme(const QString& themeName);
    QString getCurrentTheme() const { return m_currentThemeName; }
    QStringList getAvailableThemes() const;
    FluentThemeConfig getThemeConfig(const QString& themeName) const;

    // Design tokens
    void registerToken(const FluentDesignToken& token);
    void unregisterToken(const QString& tokenName);
    QVariant getToken(const QString& tokenName) const;
    void setToken(const QString& tokenName, const QVariant& value);
    QStringList getTokenNames(
        FluentTokenType type = FluentTokenType::Custom) const;

    // Token resolution with fallbacks
    QVariant resolveToken(const QString& tokenName,
                          const QVariant& fallback = QVariant()) const;
    QString resolveTokenExpression(const QString& expression) const;

    // Component theming
    void setComponentTheme(const QString& componentName,
                           const FluentComponentTheme& theme);
    FluentComponentTheme getComponentTheme(const QString& componentName) const;
    void removeComponentTheme(const QString& componentName);

    // Dynamic theming
    void enableDynamicTheming(bool enabled = true);
    bool isDynamicThemingEnabled() const { return m_dynamicThemingEnabled; }
    void setThemeTransitionDuration(int milliseconds) {
        m_transitionDuration = milliseconds;
    }

    // CSS-in-JS support
    QString generateCSS(const QString& componentName) const;
    QString generateGlobalCSS() const;
    void injectCSS(const QString& css);
    void clearInjectedCSS();

    // Runtime theme switching
    void switchTheme(const QString& themeName, bool animated = true);
    void toggleThemeMode();  // Light/Dark toggle
    void setAnimationPreference(FluentAnimationPreference preference);
    FluentAnimationPreference getAnimationPreference() const {
        return m_animationPreference;
    }

    // Theme inheritance
    void setThemeInheritance(const QString& childTheme,
                             const QString& parentTheme);
    QStringList getThemeHierarchy(const QString& themeName) const;

    // Custom properties
    void setCustomProperty(const QString& name, const QVariant& value);
    QVariant getCustomProperty(const QString& name,
                               const QVariant& defaultValue = QVariant()) const;
    void removeCustomProperty(const QString& name);

    // Theme validation
    bool validateTheme(const QString& themeName) const;
    QStringList getThemeErrors(const QString& themeName) const;

    // Import/Export
    bool loadThemeFromFile(const QString& filename);
    bool saveThemeToFile(const QString& themeName,
                         const QString& filename) const;
    bool loadThemeFromJson(const QJsonObject& json);
    QJsonObject exportThemeToJson(const QString& themeName) const;

    // Hot reloading
    void enableHotReload(bool enabled = true);
    void watchThemeFile(const QString& filename);
    void unwatchThemeFile(const QString& filename);

signals:
    void themeChanged(const QString& oldTheme, const QString& newTheme);
    void tokenChanged(const QString& tokenName, const QVariant& oldValue,
                      const QVariant& newValue);
    void componentThemeChanged(const QString& componentName);
    void themeTransitionStarted(const QString& fromTheme,
                                const QString& toTheme);
    void themeTransitionCompleted(const QString& themeName);
    void themeFileChanged(const QString& filename);
    void themeValidationFailed(const QString& themeName,
                               const QStringList& errors);

private slots:
    void onThemeFileChanged(const QString& path);
    void onThemeTransitionTimer();

private:
    FluentAdvancedThemeManager();
    void initializeDefaultTokens();
    void initializeDefaultThemes();
    QVariant computeTokenValue(const FluentDesignToken& token) const;
    void applyThemeTransition(const QString& fromTheme, const QString& toTheme);
    void validateTokenDependencies() const;

private:
    QString m_currentThemeName{"FluentLight"};
    std::unordered_map<QString, FluentThemeConfig> m_themes;
    std::unordered_map<QString, FluentDesignToken> m_tokens;
    std::unordered_map<QString, FluentComponentTheme> m_componentThemes;
    std::unordered_map<QString, QVariant> m_customProperties;

    // Dynamic theming
    bool m_dynamicThemingEnabled{true};
    int m_transitionDuration{300};
    FluentAnimationPreference m_animationPreference{
        FluentAnimationPreference::Full};

    // Hot reloading
    bool m_hotReloadEnabled{false};
    std::unique_ptr<QFileSystemWatcher> m_fileWatcher;

    // Theme transition
    QTimer* m_transitionTimer;
    QString m_transitionFromTheme;
    QString m_transitionToTheme;

    // CSS injection
    QStringList m_injectedCSS;

    // Thread safety
    mutable QMutex m_themesMutex;
    mutable QMutex m_tokensMutex;
    mutable QMutex m_componentsMutex;
};

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
