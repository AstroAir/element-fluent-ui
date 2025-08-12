// include/FluentQt/Core/FluentCulturalAdaptation.h
#pragma once

#include <QColor>
#include <QFont>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLayout>
#include <QLocale>
#include <QMap>
#include <QMargins>
#include <QMutex>
#include <QObject>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QWidget>
#include <memory>

#include "FluentI18n.h"

namespace FluentQt::Core {

// Cultural dimension types based on Hofstede's cultural dimensions
enum class FluentCulturalDimension {
    PowerDistance,          // Hierarchy and authority acceptance
    Individualism,          // Individual vs collective orientation
    MasculinityFemininity,  // Competitive vs cooperative values
    UncertaintyAvoidance,   // Risk tolerance and ambiguity
    LongTermOrientation,    // Future vs present/past orientation
    Indulgence,             // Gratification vs restraint
    Custom                  // Custom cultural dimension
};

// Cultural context types
enum class FluentCulturalContext {
    HighContext,  // Implicit communication, relationship-focused
    LowContext,   // Explicit communication, task-focused
    Mixed,        // Combination of both
    Adaptive      // Adapts based on situation
};

// Cultural color meanings and associations
enum class FluentCulturalColorMeaning {
    Positive,     // Positive associations
    Negative,     // Negative associations
    Neutral,      // Neutral associations
    Sacred,       // Religious/spiritual significance
    Warning,      // Warning or caution
    Celebration,  // Festive or celebratory
    Mourning,     // Grief or mourning
    Prosperity,   // Wealth or success
    Nature,       // Natural or environmental
    Authority,    // Power or authority
    Custom        // Custom meaning
};

// Cultural layout preferences
enum class FluentCulturalLayoutStyle {
    Minimal,       // Clean, minimal design
    Rich,          // Rich, detailed design
    Hierarchical,  // Clear hierarchy and structure
    Organic,       // Natural, flowing layout
    Geometric,     // Structured, geometric layout
    Traditional,   // Traditional cultural patterns
    Modern,        // Contemporary design
    Adaptive       // Adapts to content and context
};

// Cultural interaction patterns
enum class FluentCulturalInteraction {
    Direct,        // Direct, immediate interaction
    Indirect,      // Subtle, contextual interaction
    Formal,        // Formal, structured interaction
    Casual,        // Casual, relaxed interaction
    Hierarchical,  // Respects hierarchy and roles
    Egalitarian,   // Equal, democratic interaction
    Adaptive       // Adapts to user and context
};

// Cultural adaptation configuration
struct FluentCulturalConfig {
    QLocale locale;
    FluentCulturalContext context{FluentCulturalContext::Mixed};
    FluentCulturalLayoutStyle layoutStyle{FluentCulturalLayoutStyle::Adaptive};
    FluentCulturalInteraction interactionStyle{
        FluentCulturalInteraction::Adaptive};

    // Cultural dimension scores (0.0 to 1.0)
    QMap<FluentCulturalDimension, double> dimensionScores;

    // Color preferences and meanings
    QMap<FluentCulturalColorMeaning, QList<QColor>> colorMeanings;
    QList<QColor> preferredColors;
    QList<QColor> avoidedColors;

    // Typography preferences
    QStringList preferredFontFamilies;
    QStringList avoidedFontFamilies;
    double textScalingFactor{1.0};
    double lineHeightFactor{1.0};
    double letterSpacingFactor{1.0};

    // Layout preferences
    QMargins preferredMargins{16, 16, 16, 16};
    int preferredSpacing{8};
    double densityFactor{1.0};
    bool preferVerticalLayout{false};

    // Interaction preferences
    int preferredTouchTargetSize{44};
    int preferredAnimationDuration{200};
    bool preferSubtleAnimations{false};
    bool preferImmediateFeedback{true};

    // Content preferences
    bool preferDetailedInformation{false};
    bool preferVisualCues{true};
    bool preferTextualInformation{true};
    bool preferSymbolicRepresentation{false};

    // Custom properties
    QMap<QString, QVariant> customProperties;
};

// Cultural adaptation result
struct FluentCulturalAdaptationResult {
    bool isAdapted{false};
    QStringList appliedAdaptations;
    QStringList skippedAdaptations;
    QStringList warnings;
    double adaptationScore{0.0};  // 0.0 to 1.0
    QMap<QString, QVariant> adaptedProperties;
};

// Cultural adaptation manager
class FluentCulturalAdaptationManager : public QObject {
    Q_OBJECT

public:
    static FluentCulturalAdaptationManager& instance();

    // Configuration management
    void setCulturalConfig(const QLocale& locale,
                           const FluentCulturalConfig& config);
    FluentCulturalConfig getCulturalConfig(const QLocale& locale) const;
    void loadCulturalConfigs(const QString& directory);
    void saveCulturalConfig(const QLocale& locale,
                            const QString& filePath) const;

    // Cultural adaptation
    FluentCulturalAdaptationResult adaptWidget(QWidget* widget,
                                               const QLocale& locale);
    FluentCulturalAdaptationResult adaptLayout(QLayout* layout,
                                               const QLocale& locale);
    FluentCulturalAdaptationResult adaptColors(const QList<QColor>& colors,
                                               const QLocale& locale);
    FluentCulturalAdaptationResult adaptTypography(const QFont& font,
                                                   const QLocale& locale);

    // Color cultural adaptation
    QColor adaptColor(const QColor& color, FluentCulturalColorMeaning meaning,
                      const QLocale& locale);
    QList<QColor> generateCulturalColorPalette(const QLocale& locale,
                                               int count = 8);
    bool isColorCulturallyAppropriate(const QColor& color,
                                      const QLocale& locale);
    FluentCulturalColorMeaning getColorMeaning(const QColor& color,
                                               const QLocale& locale);

    // Typography cultural adaptation
    QFont adaptFont(const QFont& font, const QLocale& locale);
    QStringList getCulturallyPreferredFonts(const QLocale& locale);
    double getCulturalTextScaling(const QLocale& locale);
    double getCulturalLineHeightFactor(const QLocale& locale);

    // Layout cultural adaptation
    QMargins adaptMargins(const QMargins& margins, const QLocale& locale);
    int adaptSpacing(int spacing, const QLocale& locale);
    Qt::Alignment adaptAlignment(Qt::Alignment alignment,
                                 const QLocale& locale);
    QSize adaptSize(const QSize& size, const QLocale& locale);

    // Interaction cultural adaptation
    int adaptTouchTargetSize(int size, const QLocale& locale);
    int adaptAnimationDuration(int duration, const QLocale& locale);
    QString adaptInteractionFeedback(const QString& feedback,
                                     const QLocale& locale);

    // Cultural dimension analysis
    double getCulturalDimensionScore(const QLocale& locale,
                                     FluentCulturalDimension dimension);
    void setCulturalDimensionScore(const QLocale& locale,
                                   FluentCulturalDimension dimension,
                                   double score);
    FluentCulturalContext getCulturalContext(const QLocale& locale);

    // Cultural validation
    bool validateCulturalAdaptation(const QLocale& locale);
    QStringList getCulturalAdaptationWarnings(const QLocale& locale);
    double calculateCulturalCompatibility(const QLocale& locale1,
                                          const QLocale& locale2);

    // Locale analysis
    QStringList analyzeCulturalRequirements(const QLocale& locale);
    QMap<QString, double> getCulturalMetrics(const QLocale& locale);
    QStringList getSimilarCultures(const QLocale& locale, int maxResults = 5);

    // Cultural learning and adaptation
    void enableCulturalLearning(bool enabled = true);
    void recordCulturalPreference(const QLocale& locale,
                                  const QString& preference,
                                  const QVariant& value);
    void updateCulturalModel(const QLocale& locale,
                             const QMap<QString, QVariant>& feedback);

    // Integration with theming system
    void enableThemeIntegration(bool enabled = true);
    void adaptThemeForCulture(const QLocale& locale);
    QColor getCulturalAccentColor(const QLocale& locale);

    // Performance and caching
    void enableCaching(bool enabled = true);
    void clearCache();
    void preloadCulturalData(const QList<QLocale>& locales);

signals:
    void culturalConfigChanged(const QLocale& locale);
    void culturalAdaptationApplied(const QLocale& locale,
                                   const QStringList& adaptations);
    void culturalLearningUpdated(const QLocale& locale);
    void culturalValidationFailed(const QLocale& locale,
                                  const QStringList& errors);

private slots:
    void onLocaleChanged(const QLocale& locale);
    void onThemeChanged();

private:
    FluentCulturalAdaptationManager();

    // Cultural data loading
    void loadDefaultCulturalData();
    void loadCulturalDataForLocale(const QLocale& locale);
    FluentCulturalConfig createDefaultConfig(const QLocale& locale) const;

    // Adaptation algorithms
    QColor adaptColorForCulture(const QColor& color,
                                const FluentCulturalConfig& config);
    QFont adaptFontForCulture(const QFont& font,
                              const FluentCulturalConfig& config);
    QMargins adaptMarginsForCulture(const QMargins& margins,
                                    const FluentCulturalConfig& config);

    // Cultural analysis
    FluentCulturalContext analyzeCulturalContext(const QLocale& locale);
    QMap<FluentCulturalDimension, double> calculateCulturalDimensions(
        const QLocale& locale);

    // Caching
    QString generateCacheKey(const QLocale& locale,
                             const QString& operation) const;
    void addToCache(const QString& key, const QVariant& value);
    bool getFromCache(const QString& key, QVariant& value) const;

    // Cultural learning
    void updateLearningModel(const QLocale& locale, const QString& adaptation,
                             bool successful);
    double getLearningWeight(const QLocale& locale,
                             const QString& adaptation) const;

private:
    // Cultural configurations
    QHash<QLocale, FluentCulturalConfig> m_culturalConfigs;
    QHash<QLocale, QMap<QString, QVariant>> m_culturalLearning;

    // Default cultural data
    QMap<QString, FluentCulturalConfig> m_defaultConfigs;  // By country/region
    QMap<FluentCulturalDimension, QMap<QString, double>> m_dimensionData;

    // Caching
    bool m_cachingEnabled{true};
    QMap<QString, QVariant> m_adaptationCache;
    mutable QMutex m_cacheMutex;

    // Learning system
    bool m_learningEnabled{false};
    QMap<QLocale, QMap<QString, double>> m_learningWeights;

    // Integration
    bool m_themeIntegrationEnabled{false};

    // Thread safety
    mutable QMutex m_configMutex;
    mutable QMutex m_learningMutex;
};

// Cultural utility functions
namespace FluentCulturalUtils {
// Cultural analysis
bool isRTLCulture(const QLocale& locale);
bool isHighContextCulture(const QLocale& locale);
bool isCollectivistCulture(const QLocale& locale);
bool isFormalCulture(const QLocale& locale);

// Color cultural analysis
bool isColorTaboo(const QColor& color, const QLocale& locale);
QList<QColor> getCulturallyPositiveColors(const QLocale& locale);
QList<QColor> getCulturallyNegativeColors(const QLocale& locale);

// Typography cultural analysis
bool isFontCulturallyAppropriate(const QString& fontFamily,
                                 const QLocale& locale);
double getOptimalTextScaling(const QLocale& locale);

// Layout cultural analysis
bool prefersDenseLayout(const QLocale& locale);
bool prefersVerticalLayout(const QLocale& locale);
Qt::Alignment getCulturalAlignment(const QLocale& locale);

// Interaction cultural analysis
bool prefersSubtleAnimations(const QLocale& locale);
bool prefersImmediateFeedback(const QLocale& locale);
int getOptimalTouchTargetSize(const QLocale& locale);
}  // namespace FluentCulturalUtils

}  // namespace FluentQt::Core
