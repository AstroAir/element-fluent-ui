// src/Styling/FluentDesignTokenUtils.cpp
#include "FluentQt/Styling/FluentDesignTokenUtils.h"
#include <QApplication>
#include <QDebug>
#include <QMutexLocker>
#include <QScreen>
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentAdvancedThemeManager.h"

namespace FluentQt::Styling {

FluentDesignTokenUtils& FluentDesignTokenUtils::instance() {
    static FluentDesignTokenUtils instance;
    return instance;
}

FluentDesignTokenUtils::FluentDesignTokenUtils(QObject* parent)
    : QObject(parent), m_themeManager(nullptr) {
    // Initialize token cache
    initializeTokenCache();

    // Initialize default tokens
    initializeDefaultTokens();
}

QColor FluentDesignTokenUtils::getColor(const QString& tokenName,
                                        const QColor& fallback) const {
    QVariant value = resolveTokenWithFallback(tokenName, fallback);
    if (value.canConvert<QColor>()) {
        return value.value<QColor>();
    }
    return fallback;
}

QColor FluentDesignTokenUtils::getBrandColor(int shade) const {
    QString tokenName = QString("brand.%1").arg(shade);
    return getColor(tokenName, QColor("#0078d4"));  // Default Microsoft blue
}

QColor FluentDesignTokenUtils::getNeutralColor(int shade) const {
    QString tokenName = QString("neutral.%1").arg(shade);
    return getColor(tokenName,
                    shade == 0 ? QColor("#ffffff") : QColor("#000000"));
}

QColor FluentDesignTokenUtils::getSemanticColor(const QString& semantic) const {
    QString tokenName = QString("color.%1").arg(semantic);
    return getColor(tokenName);
}

QFont FluentDesignTokenUtils::getFont(const QString& tokenName,
                                      const QFont& fallback) const {
    QVariant value = resolveTokenWithFallback(tokenName, fallback);
    if (value.canConvert<QFont>()) {
        return value.value<QFont>();
    }
    return fallback;
}

QFont FluentDesignTokenUtils::getTitleFont(int level) const {
    QString tokenName;
    switch (level) {
        case 1:
            tokenName = "typography.title1";
            break;
        case 2:
            tokenName = "typography.title2";
            break;
        case 3:
            tokenName = "typography.title3";
            break;
        default:
            tokenName = "typography.title1";
            break;
    }
    return getFont(tokenName, QFont("Segoe UI", 20, QFont::DemiBold));
}

QFont FluentDesignTokenUtils::getBodyFont(bool strong) const {
    QString tokenName = strong ? "typography.body1Strong" : "typography.body1";
    return getFont(tokenName, QFont("Segoe UI", 14,
                                    strong ? QFont::DemiBold : QFont::Normal));
}

QFont FluentDesignTokenUtils::getCaptionFont(int level) const {
    QString tokenName =
        level == 1 ? "typography.caption1" : "typography.caption2";
    return getFont(tokenName, QFont("Segoe UI", 12, QFont::Normal));
}

double FluentDesignTokenUtils::getLineHeight(const QString& tokenName) const {
    QVariant value = resolveTokenWithFallback(tokenName, 1.5);
    return value.toDouble();
}

int FluentDesignTokenUtils::getSpacing(const QString& size) const {
    QString tokenName = QString("spacing.%1").arg(size);
    return getSize(tokenName, 8);  // Default to 8px
}

int FluentDesignTokenUtils::getComponentSpacing(const QString& component,
                                                const QString& property) const {
    QString tokenName = QString("spacing.%1.%2").arg(component).arg(property);
    return getSize(tokenName, 8);
}

QMargins FluentDesignTokenUtils::getComponentMargins(
    const QString& component, const QString& size) const {
    QString paddingToken = QString("%1.padding.%2").arg(component).arg(size);
    int padding = getSize(paddingToken, 8);
    return QMargins(padding, padding, padding, padding);
}

int FluentDesignTokenUtils::getSize(const QString& tokenName,
                                    int fallback) const {
    QVariant value = resolveTokenWithFallback(tokenName, fallback);
    return value.toInt();
}

QSize FluentDesignTokenUtils::getComponentSize(const QString& component,
                                               const QString& size) const {
    QString widthToken = QString("%1.size.%2.width").arg(component).arg(size);
    QString heightToken = QString("%1.size.%2.height").arg(component).arg(size);

    int width = getSize(widthToken, 100);
    int height = getSize(heightToken, 32);

    return QSize(width, height);
}

int FluentDesignTokenUtils::getBorderRadius(const QString& size) const {
    QString tokenName = QString("borderRadius.%1").arg(size);
    return getSize(tokenName, 4);
}

int FluentDesignTokenUtils::getBorderWidth(const QString& weight) const {
    QString tokenName = QString("borderWidth.%1").arg(weight);
    return getSize(tokenName, 1);
}

QString FluentDesignTokenUtils::getShadow(const QString& depth) const {
    QString tokenName = QString("shadow.%1").arg(depth);
    QVariant value = resolveTokenWithFallback(tokenName, QString());
    return value.toString();
}

QString FluentDesignTokenUtils::getComponentShadow(const QString& component,
                                                   const QString& state) const {
    QString tokenName = QString("%1.shadow.%2").arg(component).arg(state);
    return resolveTokenWithFallback(tokenName, QString()).toString();
}

int FluentDesignTokenUtils::getDuration(const QString& speed) const {
    QString tokenName = QString("duration.%1").arg(speed);
    return getSize(tokenName, 200);
}

QString FluentDesignTokenUtils::getEasing(const QString& type) const {
    QString tokenName = QString("easing.%1").arg(type);
    QVariant value = resolveTokenWithFallback(tokenName, "ease-out");
    return value.toString();
}

QHash<QString, QVariant> FluentDesignTokenUtils::getComponentTokens(
    const QString& component) const {
    QHash<QString, QVariant> tokens;

    QString prefix = component + ".";
    for (auto it = m_customTokens.begin(); it != m_customTokens.end(); ++it) {
        if (it.key().startsWith(prefix)) {
            tokens[it.key()] = it.value();
        }
    }

    return tokens;
}

QVariant FluentDesignTokenUtils::getComponentToken(const QString& component,
                                                   const QString& property,
                                                   const QString& state,
                                                   const QString& size) const {
    QString tokenName = component + "." + property;

    if (!state.isEmpty()) {
        tokenName += "." + state;
    }

    if (!size.isEmpty()) {
        tokenName += "." + size;
    }

    return resolveTokenWithFallback(tokenName, QVariant());
}

bool FluentDesignTokenUtils::isTokenValid(const QString& tokenName) const {
    return m_customTokens.contains(tokenName);
}

QStringList FluentDesignTokenUtils::getAvailableTokens(
    FluentTokenType type) const {
    Q_UNUSED(type)
    return m_customTokens.keys();
}

QStringList FluentDesignTokenUtils::getComponentTokenNames(
    const QString& component) const {
    QStringList componentTokens;
    QStringList allTokens = getAvailableTokens();

    QString prefix = component + ".";
    for (const QString& token : allTokens) {
        if (token.startsWith(prefix)) {
            componentTokens.append(token);
        }
    }

    return componentTokens;
}

QString FluentDesignTokenUtils::getTokenDescription(
    const QString& tokenName) const {
    // This would require extending the theme manager to store descriptions
    return QString("Design token: %1").arg(tokenName);
}

void FluentDesignTokenUtils::setCustomToken(const QString& tokenName,
                                            const QVariant& value,
                                            FluentTokenType type) {
    QMutexLocker locker(&m_cacheMutex);

    m_customTokens[tokenName] = value;

    // Token is stored in m_customTokens above

    // Update cache
    updateTokenCache(tokenName, value);

    // emit customTokenAdded(tokenName);  // Temporarily commented out
    // emit tokenChanged(tokenName, value);  // Temporarily commented out
}

void FluentDesignTokenUtils::removeCustomToken(const QString& tokenName) {
    QMutexLocker locker(&m_cacheMutex);

    if (m_customTokens.remove(tokenName) > 0) {
        m_tokenCache.remove(tokenName);

        // emit customTokenRemoved(tokenName);  // Temporarily commented out
    }
}

void FluentDesignTokenUtils::resetToDefaults() {
    QMutexLocker locker(&m_cacheMutex);

    // Remove all custom tokens
    m_customTokens.clear();
    invalidateTokenCache();

    // Reinitialize default tokens
    initializeDefaultTokens();
}

QVariant FluentDesignTokenUtils::getResponsiveToken(
    const QString& tokenName, const QSize& viewportSize) const {
    // Determine active breakpoint
    QString activeBreakpoint = "medium";  // default

    if (viewportSize.width() < getBreakpoint("small")) {
        activeBreakpoint = "small";
    } else if (viewportSize.width() >= getBreakpoint("large")) {
        activeBreakpoint = "large";
    }

    // Try responsive token first
    QString responsiveToken =
        QString("%1.%2").arg(tokenName).arg(activeBreakpoint);
    if (m_customTokens.contains(responsiveToken)) {
        return m_customTokens[responsiveToken];
    }

    // Fallback to base token
    return m_customTokens.value(tokenName, QVariant());
}

int FluentDesignTokenUtils::getBreakpoint(const QString& size) const {
    QString tokenName = QString("breakpoint.%1").arg(size);
    return getSize(tokenName, 768);  // Default medium breakpoint
}

bool FluentDesignTokenUtils::isBreakpointActive(
    const QString& breakpoint, const QSize& viewportSize) const {
    int breakpointValue = getBreakpoint(breakpoint);
    return viewportSize.width() >= breakpointValue;
}

QVariant FluentDesignTokenUtils::getThemeAwareToken(
    const QString& tokenName, FluentThemeMode mode) const {
    // This would require theme-specific token variants
    Q_UNUSED(mode)
    return m_customTokens.value(tokenName, QVariant());
}

void FluentDesignTokenUtils::invalidateTokenCache() {
    QMutexLocker locker(&m_cacheMutex);
    m_tokenCache.clear();
    m_cacheValid = false;
    // emit tokenCacheInvalidated();  // Temporarily commented out
}

QColor FluentDesignTokenUtils::getAccessibleColor(
    const QString& tokenName, const QString& backgroundToken,
    double minimumContrast) const {
    QColor color = getColor(tokenName);
    QColor background = getColor(backgroundToken);

    if (!color.isValid() || !background.isValid()) {
        return color;
    }

    // Simple contrast check - in production, use proper accessibility utilities
    double contrast = calculateSimpleContrast(color, background);
    if (contrast >= minimumContrast) {
        return color;
    }

    // Return high contrast alternative
    return background.lightness() > 128 ? QColor(0, 0, 0)
                                        : QColor(255, 255, 255);
}

bool FluentDesignTokenUtils::validateColorContrast(
    const QString& foregroundToken, const QString& backgroundToken,
    double minimumRatio) const {
    QColor foreground = getColor(foregroundToken);
    QColor background = getColor(backgroundToken);

    if (!foreground.isValid() || !background.isValid()) {
        return false;
    }

    double contrast = calculateSimpleContrast(foreground, background);
    return contrast >= minimumRatio;
}

// Temporarily commented out slot implementations
// void FluentDesignTokenUtils::onThemeChanged(const QString& themeName) {
//     Q_UNUSED(themeName)
//     invalidateTokenCache();
// }

// void FluentDesignTokenUtils::onTokenUpdated(const QString& tokenName,
//                                             const QVariant& value) {
//     updateTokenCache(tokenName, value);
//     emit tokenChanged(tokenName, value);
// }

QString FluentDesignTokenUtils::buildTokenName(const QString& category,
                                               const QString& property,
                                               const QString& modifier) const {
    QString tokenName = category + "." + property;
    if (!modifier.isEmpty()) {
        tokenName += "." + modifier;
    }
    return tokenName;
}

QVariant FluentDesignTokenUtils::resolveTokenWithFallback(
    const QString& tokenName, const QVariant& fallback) const {
    FLUENT_PROFILE("FluentDesignTokenUtils::resolveTokenWithFallback");

    // Check cache first
    QMutexLocker locker(&m_cacheMutex);
    if (m_cacheValid && m_tokenCache.contains(tokenName)) {
        return m_tokenCache[tokenName];
    }
    locker.unlock();

    // Resolve from custom tokens
    QVariant value = m_customTokens.value(tokenName, fallback);

    // Update cache
    updateTokenCache(tokenName, value);

    return value;
}

void FluentDesignTokenUtils::initializeTokenCache() {
    QMutexLocker locker(&m_cacheMutex);
    m_tokenCache.clear();
    m_cacheValid = true;
}

void FluentDesignTokenUtils::initializeDefaultTokens() {
    // Initialize basic design tokens with default values
    m_customTokens["brand.100"] = QColor("#0078d4");
    m_customTokens["neutral.0"] = QColor("#ffffff");
    m_customTokens["neutral.54"] = QColor("#000000");

    // Typography tokens
    m_customTokens["typography.body1.fontSize"] = 14;
    m_customTokens["typography.title1.fontSize"] = 28;
    m_customTokens["typography.caption1.fontSize"] = 12;

    // Spacing tokens
    m_customTokens["spacing.xs"] = 4;
    m_customTokens["spacing.s"] = 8;
    m_customTokens["spacing.m"] = 12;
    m_customTokens["spacing.l"] = 16;
    m_customTokens["spacing.xl"] = 20;

    // Border radius tokens
    m_customTokens["borderRadius.small"] = 2;
    m_customTokens["borderRadius.medium"] = 4;
    m_customTokens["borderRadius.large"] = 8;

    // Animation duration tokens
    m_customTokens["duration.fast"] = 100;
    m_customTokens["duration.normal"] = 200;
    m_customTokens["duration.slow"] = 300;
}

void FluentDesignTokenUtils::updateTokenCache(const QString& tokenName,
                                              const QVariant& value) const {
    QMutexLocker locker(&m_cacheMutex);
    m_tokenCache[tokenName] = value;
}

double FluentDesignTokenUtils::calculateSimpleContrast(
    const QColor& color1, const QColor& color2) const {
    // Simplified contrast calculation
    double lum1 = (color1.red() * 0.299 + color1.green() * 0.587 +
                   color1.blue() * 0.114) /
                  255.0;
    double lum2 = (color2.red() * 0.299 + color2.green() * 0.587 +
                   color2.blue() * 0.114) /
                  255.0;

    double lighter = qMax(lum1, lum2);
    double darker = qMin(lum1, lum2);

    return (lighter + 0.05) / (darker + 0.05);
}

}  // namespace FluentQt::Styling
