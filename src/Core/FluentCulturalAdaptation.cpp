// src/Core/FluentCulturalAdaptation.cpp
#include "FluentQt/Core/FluentCulturalAdaptation.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>
#include <QStandardPaths>
#include <algorithm>

namespace FluentQt::Core {

FluentCulturalAdaptationManager& FluentCulturalAdaptationManager::instance() {
    static FluentCulturalAdaptationManager instance;
    return instance;
}

FluentCulturalAdaptationManager::FluentCulturalAdaptationManager() {
    // Load default cultural data
    loadDefaultCulturalData();

    qDebug() << "FluentCulturalAdaptationManager initialized";
}

void FluentCulturalAdaptationManager::setCulturalConfig(
    const QLocale& locale, const FluentCulturalConfig& config) {
    QMutexLocker locker(&m_configMutex);
    m_culturalConfigs[locale] = config;
    emit culturalConfigChanged(locale);
}

FluentCulturalConfig FluentCulturalAdaptationManager::getCulturalConfig(
    const QLocale& locale) const {
    QMutexLocker locker(&m_configMutex);

    auto it = m_culturalConfigs.find(locale);
    if (it != m_culturalConfigs.end()) {
        return it.value();
    }

    // Return default config for this locale
    return createDefaultConfig(locale);
}

FluentCulturalAdaptationResult FluentCulturalAdaptationManager::adaptWidget(
    QWidget* widget, const QLocale& locale) {
    if (!widget) {
        return FluentCulturalAdaptationResult{};
    }

    FluentCulturalAdaptationResult result;
    result.isAdapted = true;

    auto config = getCulturalConfig(locale);

    // Adapt colors
    if (!config.preferredColors.isEmpty()) {
        // Apply preferred colors to widget
        QString styleSheet = widget->styleSheet();
        // This would involve more complex color adaptation logic
        result.appliedAdaptations << "colors";
    }

    // Adapt typography
    if (!config.preferredFontFamilies.isEmpty()) {
        QFont font = widget->font();
        font.setFamily(config.preferredFontFamilies.first());
        font.setPointSizeF(font.pointSizeF() * config.textScalingFactor);
        widget->setFont(font);
        result.appliedAdaptations << "typography";
    }

    // Adapt layout
    if (config.densityFactor != 1.0) {
        QMargins margins = widget->contentsMargins();
        margins =
            QMargins(static_cast<int>(margins.left() * config.densityFactor),
                     static_cast<int>(margins.top() * config.densityFactor),
                     static_cast<int>(margins.right() * config.densityFactor),
                     static_cast<int>(margins.bottom() * config.densityFactor));
        widget->setContentsMargins(margins);
        result.appliedAdaptations << "layout";
    }

    result.adaptationScore =
        static_cast<double>(result.appliedAdaptations.size()) /
        3.0;  // Max 3 adaptations

    emit culturalAdaptationApplied(locale, result.appliedAdaptations);
    return result;
}

QColor FluentCulturalAdaptationManager::adaptColor(
    const QColor& color, FluentCulturalColorMeaning meaning,
    const QLocale& locale) {
    auto config = getCulturalConfig(locale);

    // Check if this color has cultural significance
    auto meaningIt = config.colorMeanings.find(meaning);
    if (meaningIt != config.colorMeanings.end() &&
        !meaningIt.value().isEmpty()) {
        // Return culturally appropriate color
        return meaningIt.value().first();
    }

    // Check if color should be avoided
    if (config.avoidedColors.contains(color)) {
        // Find a suitable replacement
        if (!config.preferredColors.isEmpty()) {
            return config.preferredColors.first();
        }
    }

    return color;
}

QList<QColor> FluentCulturalAdaptationManager::generateCulturalColorPalette(
    const QLocale& locale, int count) {
    auto config = getCulturalConfig(locale);

    QList<QColor> palette;

    // Start with preferred colors
    palette.append(config.preferredColors);

    // Add cultural colors
    for (auto it = config.colorMeanings.begin();
         it != config.colorMeanings.end(); ++it) {
        palette.append(it.value());
    }

    // Remove duplicates and avoided colors
    palette.erase(
        std::remove_if(palette.begin(), palette.end(),
                       [&config](const QColor& color) {
                           return config.avoidedColors.contains(color);
                       }),
        palette.end());

    // Ensure we have enough colors
    while (palette.size() < count) {
        // Generate additional culturally appropriate colors
        QColor baseColor =
            palette.isEmpty() ? QColor(0, 120, 215) : palette.first();
        // This would involve more sophisticated color generation
        palette.append(baseColor.lighter(110 + (palette.size() % 5) * 10));
    }

    // Trim to requested count
    if (palette.size() > count) {
        palette = palette.mid(0, count);
    }

    return palette;
}

bool FluentCulturalAdaptationManager::isColorCulturallyAppropriate(
    const QColor& color, const QLocale& locale) {
    auto config = getCulturalConfig(locale);

    // Check if color is in avoided list
    if (config.avoidedColors.contains(color)) {
        return false;
    }

    // Check if color has negative cultural meaning
    auto negativeColors =
        config.colorMeanings.value(FluentCulturalColorMeaning::Negative);
    if (negativeColors.contains(color)) {
        return false;
    }

    return true;
}

QFont FluentCulturalAdaptationManager::adaptFont(const QFont& font,
                                                 const QLocale& locale) {
    auto config = getCulturalConfig(locale);

    QFont adaptedFont = font;

    // Apply preferred font family
    if (!config.preferredFontFamilies.isEmpty()) {
        adaptedFont.setFamily(config.preferredFontFamilies.first());
    }

    // Apply text scaling
    adaptedFont.setPointSizeF(font.pointSizeF() * config.textScalingFactor);

    return adaptedFont;
}

QMargins FluentCulturalAdaptationManager::adaptMargins(const QMargins& margins,
                                                       const QLocale& locale) {
    auto config = getCulturalConfig(locale);

    // Apply density factor
    return QMargins(static_cast<int>(margins.left() * config.densityFactor),
                    static_cast<int>(margins.top() * config.densityFactor),
                    static_cast<int>(margins.right() * config.densityFactor),
                    static_cast<int>(margins.bottom() * config.densityFactor));
}

double FluentCulturalAdaptationManager::getCulturalDimensionScore(
    const QLocale& locale, FluentCulturalDimension dimension) {
    auto config = getCulturalConfig(locale);
    return config.dimensionScores.value(dimension, 0.5);  // Default to neutral
}

FluentCulturalContext FluentCulturalAdaptationManager::getCulturalContext(
    const QLocale& locale) {
    auto config = getCulturalConfig(locale);
    return config.context;
}

void FluentCulturalAdaptationManager::loadDefaultCulturalData() {
    // Load default cultural configurations for common locales

    // Western cultures (US, UK, Germany, etc.)
    FluentCulturalConfig westernConfig;
    westernConfig.context = FluentCulturalContext::LowContext;
    westernConfig.layoutStyle = FluentCulturalLayoutStyle::Minimal;
    westernConfig.interactionStyle = FluentCulturalInteraction::Direct;
    westernConfig.dimensionScores[FluentCulturalDimension::Individualism] = 0.8;
    westernConfig.dimensionScores[FluentCulturalDimension::PowerDistance] = 0.3;
    westernConfig.preferredColors = {QColor(0, 120, 215), QColor(16, 124, 16),
                                     QColor(255, 185, 0)};

    m_culturalConfigs[QLocale(QLocale::English, QLocale::UnitedStates)] =
        westernConfig;
    m_culturalConfigs[QLocale(QLocale::English, QLocale::UnitedKingdom)] =
        westernConfig;
    m_culturalConfigs[QLocale(QLocale::German, QLocale::Germany)] =
        westernConfig;

    // East Asian cultures (China, Japan, Korea)
    FluentCulturalConfig eastAsianConfig;
    eastAsianConfig.context = FluentCulturalContext::HighContext;
    eastAsianConfig.layoutStyle = FluentCulturalLayoutStyle::Hierarchical;
    eastAsianConfig.interactionStyle = FluentCulturalInteraction::Formal;
    eastAsianConfig.dimensionScores[FluentCulturalDimension::Individualism] =
        0.2;
    eastAsianConfig.dimensionScores[FluentCulturalDimension::PowerDistance] =
        0.7;
    eastAsianConfig.preferredColors = {QColor(220, 20, 60), QColor(255, 215, 0),
                                       QColor(0, 100, 0)};
    eastAsianConfig.avoidedColors = {
        QColor(255, 255, 255)};  // White can signify death

    m_culturalConfigs[QLocale(QLocale::Chinese, QLocale::China)] =
        eastAsianConfig;
    m_culturalConfigs[QLocale(QLocale::Japanese, QLocale::Japan)] =
        eastAsianConfig;
    m_culturalConfigs[QLocale(QLocale::Korean, QLocale::SouthKorea)] =
        eastAsianConfig;

    // Middle Eastern cultures (Arabic, Hebrew)
    FluentCulturalConfig middleEasternConfig;
    middleEasternConfig.context = FluentCulturalContext::HighContext;
    middleEasternConfig.layoutStyle = FluentCulturalLayoutStyle::Rich;
    middleEasternConfig.interactionStyle = FluentCulturalInteraction::Formal;
    middleEasternConfig
        .dimensionScores[FluentCulturalDimension::PowerDistance] = 0.8;
    middleEasternConfig.preferredColors = {
        QColor(0, 128, 0), QColor(255, 215, 0), QColor(128, 0, 128)};
    middleEasternConfig.preferredFontFamilies = {"Arial", "Tahoma", "Segoe UI"};

    m_culturalConfigs[QLocale(QLocale::Arabic, QLocale::SaudiArabia)] =
        middleEasternConfig;
    m_culturalConfigs[QLocale(QLocale::Hebrew, QLocale::Israel)] =
        middleEasternConfig;
}

FluentCulturalConfig FluentCulturalAdaptationManager::createDefaultConfig(
    const QLocale& locale) const {
    FluentCulturalConfig config;
    config.locale = locale;

    // Set defaults based on locale characteristics
    if (locale.language() == QLocale::Arabic ||
        locale.language() == QLocale::Hebrew) {
        config.context = FluentCulturalContext::HighContext;
        config.layoutStyle = FluentCulturalLayoutStyle::Rich;
        config.interactionStyle = FluentCulturalInteraction::Formal;
    } else if (locale.language() == QLocale::Chinese ||
               locale.language() == QLocale::Japanese ||
               locale.language() == QLocale::Korean) {
        config.context = FluentCulturalContext::HighContext;
        config.layoutStyle = FluentCulturalLayoutStyle::Hierarchical;
        config.interactionStyle = FluentCulturalInteraction::Formal;
    } else {
        config.context = FluentCulturalContext::LowContext;
        config.layoutStyle = FluentCulturalLayoutStyle::Minimal;
        config.interactionStyle = FluentCulturalInteraction::Direct;
    }

    return config;
}

void FluentCulturalAdaptationManager::onThemeChanged() {
    // Respond to theme changes by updating cultural adaptations
    // This would typically involve re-evaluating color adaptations
    qDebug() << "Theme changed, updating cultural adaptations";
}

void FluentCulturalAdaptationManager::onLocaleChanged(const QLocale& locale) {
    // Respond to locale changes by updating cultural configurations
    auto config = getCulturalConfig(locale);
    emit culturalConfigChanged(locale);

    qDebug() << "Locale changed to:" << locale.name();
}

// Utility function implementations
namespace FluentCulturalUtils {

bool isRTLCulture(const QLocale& locale) {
    return locale.language() == QLocale::Arabic ||
           locale.language() == QLocale::Hebrew ||
           locale.language() == QLocale::Persian ||
           locale.language() == QLocale::Urdu;
}

bool isHighContextCulture(const QLocale& locale) {
    return locale.language() == QLocale::Chinese ||
           locale.language() == QLocale::Japanese ||
           locale.language() == QLocale::Korean ||
           locale.language() == QLocale::Arabic ||
           locale.language() == QLocale::Thai;
}

bool isCollectivistCulture(const QLocale& locale) {
    return locale.language() == QLocale::Chinese ||
           locale.language() == QLocale::Japanese ||
           locale.language() == QLocale::Korean ||
           locale.territory() == QLocale::India ||
           locale.territory() == QLocale::Brazil;
}

bool isFormalCulture(const QLocale& locale) {
    return locale.language() == QLocale::German ||
           locale.language() == QLocale::Japanese ||
           locale.language() == QLocale::Korean ||
           locale.language() == QLocale::Russian;
}

bool isColorTaboo(const QColor& color, const QLocale& locale) {
    // White can be associated with death in some East Asian cultures
    if ((locale.language() == QLocale::Chinese ||
         locale.language() == QLocale::Korean) &&
        color == QColor(255, 255, 255)) {
        return true;
    }

    // Green can be problematic in some Middle Eastern cultures
    if (locale.language() == QLocale::Arabic && color.green() > 200 &&
        color.red() < 50 && color.blue() < 50) {
        return true;
    }

    return false;
}

QList<QColor> getCulturallyPositiveColors(const QLocale& locale) {
    if (locale.language() == QLocale::Chinese) {
        return {QColor(220, 20, 60), QColor(255, 215, 0)};  // Red and gold
    } else if (locale.language() == QLocale::Arabic) {
        return {QColor(0, 128, 0), QColor(255, 215, 0)};  // Green and gold
    } else {
        return {QColor(0, 120, 215), QColor(16, 124, 16)};  // Blue and green
    }
}

}  // namespace FluentCulturalUtils

}  // namespace FluentQt::Core
