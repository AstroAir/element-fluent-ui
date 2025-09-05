// include/FluentQt/Styling/FluentDesignTokenUtils.h
#pragma once

#include <QColor>
#include <QFont>
#include <QHash>
#include <QMutex>
#include <QObject>
#include <QSize>
#include <QString>
#include <QVariant>
#include "FluentAdvancedTheme.h"

namespace FluentQt::Styling {

// Design token utility class for easy access and manipulation
class FluentDesignTokenUtils : public QObject {
    // Q_OBJECT  // Temporarily commented out to fix linking issues

public:
    static FluentDesignTokenUtils& instance();

    // Color token utilities
    QColor getColor(const QString& tokenName,
                    const QColor& fallback = QColor()) const;
    QColor getBrandColor(int shade = 100) const;
    QColor getNeutralColor(int shade = 0) const;
    QColor getSemanticColor(const QString& semantic) const;

    // Typography token utilities
    QFont getFont(const QString& tokenName,
                  const QFont& fallback = QFont()) const;
    QFont getTitleFont(int level = 1) const;
    QFont getBodyFont(bool strong = false) const;
    QFont getCaptionFont(int level = 1) const;
    double getLineHeight(const QString& tokenName) const;

    // Spacing token utilities
    int getSpacing(const QString& size) const;
    int getComponentSpacing(const QString& component,
                            const QString& property) const;
    QMargins getComponentMargins(const QString& component,
                                 const QString& size = "medium") const;

    // Size token utilities
    int getSize(const QString& tokenName, int fallback = 0) const;
    QSize getComponentSize(const QString& component,
                           const QString& size = "medium") const;
    int getBorderRadius(const QString& size = "medium") const;
    int getBorderWidth(const QString& weight = "thin") const;

    // Shadow token utilities
    QString getShadow(const QString& depth) const;
    QString getComponentShadow(const QString& component,
                               const QString& state = "rest") const;

    // Animation token utilities
    int getDuration(const QString& speed = "normal") const;
    QString getEasing(const QString& type = "easyEase") const;

    // Component token utilities
    QHash<QString, QVariant> getComponentTokens(const QString& component) const;
    QVariant getComponentToken(const QString& component,
                               const QString& property,
                               const QString& state = "",
                               const QString& size = "") const;

    // Token validation and debugging
    bool isTokenValid(const QString& tokenName) const;
    QStringList getAvailableTokens(
        FluentTokenType type = FluentTokenType::Custom) const;
    QStringList getComponentTokenNames(const QString& component) const;
    QString getTokenDescription(const QString& tokenName) const;

    // Token manipulation
    void setCustomToken(const QString& tokenName, const QVariant& value,
                        FluentTokenType type = FluentTokenType::Custom);
    void removeCustomToken(const QString& tokenName);
    void resetToDefaults();

    // Responsive token utilities
    QVariant getResponsiveToken(const QString& tokenName,
                                const QSize& viewportSize) const;
    int getBreakpoint(const QString& size) const;
    bool isBreakpointActive(const QString& breakpoint,
                            const QSize& viewportSize) const;

    // Theme-aware token utilities
    QVariant getThemeAwareToken(const QString& tokenName,
                                FluentThemeMode mode) const;
    void invalidateTokenCache();

    // Accessibility-aware token utilities
    QColor getAccessibleColor(const QString& tokenName,
                              const QString& backgroundToken,
                              double minimumContrast = 4.5) const;
    bool validateColorContrast(const QString& foregroundToken,
                               const QString& backgroundToken,
                               double minimumRatio = 4.5) const;

    // signals:  // Temporarily commented out to fix linking issues
    //     void tokenChanged(const QString& tokenName, const QVariant&
    //     newValue); void tokenCacheInvalidated(); void customTokenAdded(const
    //     QString& tokenName); void customTokenRemoved(const QString&
    //     tokenName);

    // private slots:  // Temporarily commented out to fix linking issues
    //     void onThemeChanged(const QString& themeName);
    //     void onTokenUpdated(const QString& tokenName, const QVariant& value);

private:
    FluentDesignTokenUtils(QObject* parent = nullptr);
    ~FluentDesignTokenUtils() = default;

    // Internal utilities
    QString buildTokenName(const QString& category, const QString& property,
                           const QString& modifier = "") const;
    QVariant resolveTokenWithFallback(const QString& tokenName,
                                      const QVariant& fallback) const;
    void initializeTokenCache();
    void initializeDefaultTokens();
    void updateTokenCache(const QString& tokenName,
                          const QVariant& value) const;

    // Color contrast calculation utility
    double calculateSimpleContrast(const QColor& color1,
                                   const QColor& color2) const;

    // Token caching for performance
    mutable QMutex m_cacheMutex;
    mutable QHash<QString, QVariant> m_tokenCache;
    mutable bool m_cacheValid{false};

    // Custom tokens
    QHash<QString, QVariant> m_customTokens;

    // Theme manager reference
    FluentAdvancedThemeManager* m_themeManager;
};

// Convenience functions for common token operations
namespace TokenUtils {
// Quick color access
inline QColor color(const QString& token) {
    return FluentDesignTokenUtils::instance().getColor(token);
}

inline QColor brandColor(int shade = 100) {
    return FluentDesignTokenUtils::instance().getBrandColor(shade);
}

inline QColor neutralColor(int shade = 0) {
    return FluentDesignTokenUtils::instance().getNeutralColor(shade);
}

// Quick typography access
inline QFont font(const QString& token) {
    return FluentDesignTokenUtils::instance().getFont(token);
}

inline QFont titleFont(int level = 1) {
    return FluentDesignTokenUtils::instance().getTitleFont(level);
}

inline QFont bodyFont(bool strong = false) {
    return FluentDesignTokenUtils::instance().getBodyFont(strong);
}

// Quick spacing access
inline int spacing(const QString& size) {
    return FluentDesignTokenUtils::instance().getSpacing(size);
}

inline QMargins margins(const QString& component,
                        const QString& size = "medium") {
    return FluentDesignTokenUtils::instance().getComponentMargins(component,
                                                                  size);
}

// Quick size access
inline int size(const QString& token) {
    return FluentDesignTokenUtils::instance().getSize(token);
}

inline QSize componentSize(const QString& component,
                           const QString& size = "medium") {
    return FluentDesignTokenUtils::instance().getComponentSize(component, size);
}

// Quick animation access
inline int duration(const QString& speed = "normal") {
    return FluentDesignTokenUtils::instance().getDuration(speed);
}

inline QString easing(const QString& type = "easyEase") {
    return FluentDesignTokenUtils::instance().getEasing(type);
}
}  // namespace TokenUtils

// Enhanced convenience macros with better error handling
#define FLUENT_COLOR(token) FluentQt::Styling::TokenUtils::color(token)

#define FLUENT_BRAND_COLOR(shade) \
    FluentQt::Styling::TokenUtils::brandColor(shade)

#define FLUENT_NEUTRAL_COLOR(shade) \
    FluentQt::Styling::TokenUtils::neutralColor(shade)

#define FLUENT_FONT(token) FluentQt::Styling::TokenUtils::font(token)

#define FLUENT_TITLE_FONT(level) FluentQt::Styling::TokenUtils::titleFont(level)

#define FLUENT_BODY_FONT(strong) FluentQt::Styling::TokenUtils::bodyFont(strong)

#define FLUENT_SPACING(size) FluentQt::Styling::TokenUtils::spacing(size)

#define FLUENT_MARGINS(component, size) \
    FluentQt::Styling::TokenUtils::margins(component, size)

#define FLUENT_SIZE(token) FluentQt::Styling::TokenUtils::size(token)

#define FLUENT_COMPONENT_SIZE(component, size) \
    FluentQt::Styling::TokenUtils::componentSize(component, size)

#define FLUENT_DURATION(speed) FluentQt::Styling::TokenUtils::duration(speed)

#define FLUENT_EASING(type) FluentQt::Styling::TokenUtils::easing(type)

// Component-specific convenience macros
#define FLUENT_BUTTON_COLOR(style, state) \
    FLUENT_COLOR(QString("button.%1.background.%2").arg(style).arg(state))

#define FLUENT_BUTTON_SIZE(size, property) \
    FLUENT_SIZE(QString("button.size.%1.%2").arg(size).arg(property))

#define FLUENT_CARD_PADDING(size) \
    FLUENT_SPACING(QString("card.padding.%1").arg(size))

#define FLUENT_INPUT_HEIGHT(size) \
    FLUENT_SIZE(QString("input.height.%1").arg(size))

// Validation macros for development
#ifdef QT_DEBUG
#define FLUENT_VALIDATE_TOKEN(token)                               \
    do {                                                           \
        if (!FluentQt::Styling::FluentDesignTokenUtils::instance() \
                 .isTokenValid(token)) {                           \
            qWarning() << "Invalid design token:" << token;        \
        }                                                          \
    } while (0)
#else
#define FLUENT_VALIDATE_TOKEN(token) \
    do {                             \
    } while (0)
#endif

}  // namespace FluentQt::Styling
