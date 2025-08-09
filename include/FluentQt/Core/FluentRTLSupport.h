// include/FluentQt/Core/FluentRTLSupport.h
#pragma once

#include <QObject>
#include <QWidget>
#include <QLayout>
#include <QLayoutItem>
#include <QString>
#include <QStringList>
#include <QLocale>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QMargins>
#include <QFont>
#include <QPropertyAnimation>
#include <QHash>
#include <QTextOption>
#include <QTextDocument>
#include <QMutex>
#include <QMap>
#include <QVariant>

#include "FluentI18n.h"

namespace FluentQt::Core {

// RTL adaptation modes
enum class FluentRTLMode {
    Disabled,              // No RTL support
    Automatic,             // Automatic based on locale
    Forced,                // Force RTL regardless of locale
    Mixed,                 // Mixed LTR/RTL content support
    Contextual,            // Context-aware RTL adaptation
    Custom                 // Custom RTL logic
};

// RTL layout mirroring types
enum class FluentRTLMirroring {
    None,                  // No mirroring
    Horizontal,            // Horizontal mirroring only
    Complete,              // Complete layout mirroring
    Selective,             // Selective element mirroring
    Content,               // Content-aware mirroring
    Adaptive               // Adaptive mirroring based on content
};

// RTL text alignment strategies
enum class FluentRTLTextAlignment {
    Auto,                  // Automatic based on text direction
    Start,                 // Align to start (right for RTL)
    End,                   // Align to end (left for RTL)
    Center,                // Center alignment
    Justify,               // Justify alignment
    Natural,               // Natural text direction alignment
    Forced                 // Force specific alignment
};

// RTL icon and image handling
enum class FluentRTLIconHandling {
    None,                  // No special handling
    Mirror,                // Mirror horizontally
    Replace,               // Replace with RTL-specific version
    Rotate,                // Rotate if appropriate
    Contextual,            // Context-aware handling
    Custom                 // Custom handling logic
};

// Bidirectional text types
enum class FluentBidiTextType {
    LTR,                   // Left-to-right text
    RTL,                   // Right-to-left text
    Mixed,                 // Mixed direction text
    Neutral,               // Direction-neutral text
    Auto                   // Automatic detection
};

// RTL configuration
struct FluentRTLConfig {
    FluentRTLMode mode{FluentRTLMode::Automatic};
    FluentRTLMirroring mirroring{FluentRTLMirroring::Complete};
    FluentRTLTextAlignment textAlignment{FluentRTLTextAlignment::Auto};
    FluentRTLIconHandling iconHandling{FluentRTLIconHandling::Contextual};
    
    // Layout mirroring preferences
    bool mirrorLayouts{true};
    bool mirrorMargins{true};
    bool mirrorPadding{true};
    bool mirrorBorders{true};
    bool mirrorScrollbars{true};
    bool mirrorToolbars{true};
    bool mirrorMenus{true};
    bool mirrorTabs{true};
    
    // Text handling preferences
    bool enableBidiSupport{true};
    bool preserveTextDirection{true};
    bool autoDetectTextDirection{true};
    bool handleMixedContent{true};
    
    // Icon and image preferences
    QStringList mirrorableIcons;
    QStringList nonMirrorableIcons;
    QMap<QString, QString> rtlIconReplacements;
    
    // Animation preferences
    bool mirrorAnimations{true};
    bool reverseTransitions{true};
    
    // Custom properties
    QMap<QString, QVariant> customProperties;
};

// RTL adaptation result
struct FluentRTLAdaptationResult {
    bool isAdapted{false};
    FluentTextDirection detectedDirection{FluentTextDirection::Auto};
    QStringList appliedAdaptations;
    QStringList skippedAdaptations;
    QStringList warnings;
    QMap<QString, QVariant> adaptedProperties;
};

// Advanced RTL support manager
class FluentRTLSupportManager : public QObject {
    Q_OBJECT

public:
    static FluentRTLSupportManager& instance();
    
    // Configuration
    void setRTLConfig(const FluentRTLConfig& config);
    FluentRTLConfig getRTLConfig() const { return m_config; }
    void setRTLMode(FluentRTLMode mode);
    FluentRTLMode getRTLMode() const { return m_config.mode; }
    
    // RTL detection and analysis
    bool isRTLLocale(const QLocale& locale) const;
    FluentTextDirection detectTextDirection(const QString& text) const;
    FluentBidiTextType analyzeBidiText(const QString& text) const;
    bool containsRTLCharacters(const QString& text) const;
    bool containsLTRCharacters(const QString& text) const;
    bool isRTLCharacter(const QChar& character) const;
    bool isLTRCharacter(const QChar& character) const;
    bool isNeutralCharacter(const QChar& character) const;
    
    // Widget and layout adaptation
    FluentRTLAdaptationResult adaptWidget(QWidget* widget, bool recursive = true);
    FluentRTLAdaptationResult adaptLayout(QLayout* layout, bool recursive = true);
    void adaptWidgetForRTL(QWidget* widget, const QLocale& locale);
    void adaptLayoutForRTL(QLayout* layout, const QLocale& locale);
    
    // Layout mirroring
    QRect mirrorRect(const QRect& rect, const QRect& container) const;
    QPoint mirrorPoint(const QPoint& point, const QRect& container) const;
    QMargins mirrorMargins(const QMargins& margins) const;
    Qt::Alignment mirrorAlignment(Qt::Alignment alignment) const;
    
    // Text handling
    QString processRTLText(const QString& text, FluentRTLTextAlignment alignment = FluentRTLTextAlignment::Auto) const;
    QTextOption createRTLTextOption(FluentRTLTextAlignment alignment = FluentRTLTextAlignment::Auto);
    void configureTextDocumentForRTL(QTextDocument* document, const QLocale& locale);
    QString insertBidiMarkers(const QString& text) const;
    QString removeBidiMarkers(const QString& text) const;
    
    // Icon and image handling
    QString getRTLIconPath(const QString& iconPath) const;
    bool shouldMirrorIcon(const QString& iconName) const;
    QPixmap mirrorPixmap(const QPixmap& pixmap) const;
    QIcon createRTLIcon(const QIcon& icon) const;
    
    // Font and typography adaptation
    QFont adaptFontForRTL(const QFont& font, const QLocale& locale) const;
    double getOptimalLetterSpacing(const QFont& font, const QLocale& locale) const;
    double getOptimalWordSpacing(const QFont& font, const QLocale& locale) const;
    
    // Animation and transition adaptation
    QString adaptAnimationForRTL(const QString& animationName) const;
    QPropertyAnimation* createRTLAnimation(QObject* target, const QByteArray& property) const;
    void reverseAnimationDirection(QPropertyAnimation* animation) const;
    
    // Bidirectional text utilities
    QString wrapWithBidiIsolation(const QString& text, FluentTextDirection direction) const;
    QString wrapWithBidiOverride(const QString& text, FluentTextDirection direction) const;
    QStringList splitBidiText(const QString& text) const;
    QString normalizeBidiText(const QString& text) const;
    
    // RTL-aware measurements
    int getTextWidth(const QString& text, const QFont& font, FluentTextDirection direction) const;
    QSize getTextSize(const QString& text, const QFont& font, FluentTextDirection direction) const;
    QRect getTextBoundingRect(const QString& text, const QFont& font, const QRect& rect, FluentTextDirection direction) const;
    
    // Locale-specific RTL features
    bool requiresSpecialRTLHandling(const QLocale& locale) const;
    QStringList getRTLSpecificFeatures(const QLocale& locale) const;
    QString getLocaleSpecificBidiClass(const QLocale& locale) const;
    
    // Validation and testing
    bool validateRTLAdaptation(QWidget* widget) const;
    QStringList getRTLAdaptationIssues(QWidget* widget) const;
    double calculateRTLCompatibilityScore(QWidget* widget) const;
    
    // Performance optimization
    void enableRTLCaching(bool enabled = true);
    void clearRTLCache();
    void precomputeRTLAdaptations(const QList<QWidget*>& widgets);

signals:
    void rtlModeChanged(FluentRTLMode mode);
    void rtlAdaptationApplied(QWidget* widget);
    void textDirectionDetected(const QString& text, FluentTextDirection direction);
    void bidiTextAnalyzed(const QString& text, FluentBidiTextType type);

private slots:
    void onLocaleChanged(const QLocale& locale);
    void onLayoutDirectionChanged(Qt::LayoutDirection direction);

private:
    FluentRTLSupportManager();
    
    // RTL detection algorithms
    FluentTextDirection detectTextDirectionInternal(const QString& text) const;
    
    // Widget adaptation algorithms
    void adaptWidgetGeometry(QWidget* widget) const;
    void adaptWidgetAlignment(QWidget* widget) const;
    void adaptWidgetMargins(QWidget* widget) const;
    void adaptWidgetText(QWidget* widget) const;
    
    // Layout adaptation algorithms
    void adaptLayoutDirection(QLayout* layout) const;
    void adaptLayoutSpacing(QLayout* layout) const;
    void adaptLayoutAlignment(QLayout* layout) const;
    void adaptLayoutItems(QLayout* layout) const;
    
    // Caching
    QString generateCacheKey(const QString& operation, const QVariant& input) const;
    void addToCache(const QString& key, const QVariant& value);
    bool getFromCache(const QString& key, QVariant& value) const;
    
    // Unicode bidirectional algorithm helpers
    QString applyUnicodeBidiAlgorithm(const QString& text) const;
    QList<QPair<int, FluentTextDirection>> analyzeBidiRuns(const QString& text) const;
    QString resolveBidiLevels(const QString& text) const;

private:
    FluentRTLConfig m_config;
    
    // Caching
    bool m_cachingEnabled{true};
    QMap<QString, QVariant> m_adaptationCache;
    mutable QMutex m_cacheMutex;
    
    // RTL character classification
    QSet<QChar::Category> m_rtlCategories;
    QSet<QChar::Category> m_ltrCategories;
    QSet<QChar::Category> m_neutralCategories;
    
    // Locale-specific RTL data
    QHash<QLocale, QStringList> m_localeRTLFeatures;
    QHash<QLocale, QString> m_localeBidiClasses;
    
    // Performance optimization
    mutable QMap<QString, FluentTextDirection> m_textDirectionCache;
    mutable QMap<QString, FluentBidiTextType> m_bidiTypeCache;
    
    // Thread safety
    mutable QMutex m_configMutex;
};

// RTL utility functions
namespace FluentRTLUtilities {
    // Text direction utilities
    bool isRTLText(const QString& text);
    bool isLTRText(const QString& text);
    bool isMixedDirectionText(const QString& text);
    FluentTextDirection getStrongDirectionality(const QString& text);
    
    // Layout utilities
    QRect mirrorRectInContainer(const QRect& rect, const QRect& container);
    QPoint mirrorPointInContainer(const QPoint& point, const QRect& container);
    Qt::Alignment mirrorHorizontalAlignment(Qt::Alignment alignment);
    
    // Unicode utilities
    bool isRTLCharacter(QChar character);
    bool isLTRCharacter(QChar character);
    bool isNeutralCharacter(QChar character);
    QChar::Direction getCharacterDirection(QChar character);
    
    // Bidi utilities
    QString addLTRMark(const QString& text);
    QString addRTLMark(const QString& text);
    QString addPopDirectionalFormatting(const QString& text);
    QString isolateRTLText(const QString& text);
    QString isolateLTRText(const QString& text);
    
    // Icon utilities
    bool shouldMirrorIcon(const QString& iconName);
    QString getRTLIconVariant(const QString& iconName);
    QPixmap mirrorPixmapHorizontally(const QPixmap& pixmap);
    
    // Font utilities
    bool isFontRTLCompatible(const QFont& font);
    QFont getOptimalRTLFont(const QLocale& locale);
    double getOptimalRTLLetterSpacing(const QFont& font);
} // namespace FluentRTLUtilities

// Convenience macros for RTL support
#define FLUENT_RTL_ADAPT_WIDGET(widget) \
    FluentQt::Core::FluentRTLSupportManager::instance().adaptWidget(widget)

#define FLUENT_RTL_ADAPT_LAYOUT(layout) \
    FluentQt::Core::FluentRTLSupportManager::instance().adaptLayout(layout)

#define FLUENT_RTL_MIRROR_RECT(rect, container) \
    FluentQt::Core::FluentRTLSupportManager::instance().mirrorRect(rect, container)

#define FLUENT_RTL_DETECT_DIRECTION(text) \
    FluentQt::Core::FluentRTLSupportManager::instance().detectTextDirection(text)

} // namespace FluentQt::Core
