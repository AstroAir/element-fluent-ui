// include/FluentQt/Core/FluentI18n.h
#pragma once

#include <QColor>
#include <QDateTime>
#include <QDir>
#include <QLocale>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QTranslator>
#include <functional>
#include <memory>
#include <unordered_map>

namespace FluentQt::Core {

// Enhanced text direction for comprehensive RTL support
enum class FluentTextDirection {
    LeftToRight,
    RightToLeft,
    Auto,       // Determined by locale
    Inherit,    // Inherit from parent
    Mixed,      // Mixed content direction
    Contextual  // Based on content analysis
};

// Number formatting styles
enum class FluentNumberFormat {
    Decimal,
    Currency,
    Percent,
    Scientific,
    Compact
};

// Date/time formatting styles
enum class FluentDateTimeFormat {
    Short,     // 12/31/99, 3:30 PM
    Medium,    // Dec 31, 1999, 3:30:00 PM
    Long,      // December 31, 1999, 3:30:00 PM PST
    Full,      // Friday, December 31, 1999, 3:30:00 PM PST
    Relative,  // 2 hours ago, tomorrow
    Custom     // User-defined format
};

// Enhanced pluralization rules
enum class FluentPluralRule {
    Zero,        // 0 items
    One,         // 1 item
    Two,         // 2 items (for languages with dual)
    Few,         // few items (2-4 in some languages)
    Many,        // many items (5+ in some languages)
    Other,       // default/other
    Fractional,  // Fractional numbers (1.5, 2.3, etc.)
    Ordinal      // Ordinal numbers (1st, 2nd, 3rd, etc.)
};

// Cultural adaptation types
enum class FluentCulturalAdaptation {
    None,         // No cultural adaptation
    Colors,       // Color preferences and meanings
    Icons,        // Icon styles and meanings
    Layout,       // Layout preferences (spacing, density)
    Typography,   // Font preferences and sizing
    Imagery,      // Image and illustration styles
    Interaction,  // Interaction patterns and gestures
    Content,      // Content presentation preferences
    All           // All cultural adaptations
};

// Locale-specific formatting preferences
enum class FluentFormattingStyle {
    Default,       // Default system formatting
    Formal,        // Formal/business formatting
    Casual,        // Casual/friendly formatting
    Technical,     // Technical/precise formatting
    Localized,     // Fully localized formatting
    International  // International/neutral formatting
};

// Text case transformation types
enum class FluentTextCase {
    None,      // No transformation
    Lower,     // lowercase
    Upper,     // UPPERCASE
    Title,     // Title Case
    Sentence,  // Sentence case
    Camel,     // camelCase
    Pascal,    // PascalCase
    Snake,     // snake_case
    Kebab,     // kebab-case
    Localized  // Locale-specific casing
};

// Translation context for better organization
struct TranslationContext {
    QString domain;     // Translation domain (e.g., "buttons", "messages")
    QString component;  // Component name
    QString feature;    // Feature name
    QStringList tags;   // Additional tags for categorization
};

// Translation entry with metadata
struct TranslationEntry {
    QString key;
    QString sourceText;
    QString translatedText;
    QString context;
    QString comment;
    TranslationContext translationContext;
    QDateTime lastModified;
    bool isPlural{false};
    std::unordered_map<FluentPluralRule, QString> pluralForms;
};

// Enhanced locale information with cultural adaptation
struct FluentLocaleInfo {
    QLocale locale;
    QString displayName;
    QString nativeName;
    QString englishName;
    FluentTextDirection textDirection;
    QString dateFormat;
    QString timeFormat;
    QString numberFormat;
    QString currencySymbol;
    QString currencyCode;
    QStringList supportedFeatures;
    bool isRTL{false};
    bool isComplete{false};  // Translation completeness
    double completionPercentage{0.0};

    // Cultural adaptation properties
    FluentFormattingStyle preferredFormattingStyle{
        FluentFormattingStyle::Default};
    QMap<FluentCulturalAdaptation, QVariant> culturalPreferences;
    QStringList preferredFonts;
    QStringList fallbackFonts;
    double textScalingFactor{1.0};
    double lineHeightFactor{1.0};

    // Regional preferences
    QString region;
    QString script;
    QString variant;
    QStringList territories;
    QStringList languages;

    // Formatting preferences
    QString listSeparator{", "};
    QString listLastSeparator{" and "};
    QString decimalSeparator{"."};
    QString thousandsSeparator{","};
    QString quotationMarks{"\"\"''"};  // Primary and secondary quotes

    // Calendar and time preferences
    Qt::DayOfWeek firstDayOfWeek{Qt::Monday};
    QStringList monthNames;
    QStringList monthNamesShort;
    QStringList dayNames;
    QStringList dayNamesShort;
    bool use24HourFormat{true};
    QString amPmIndicator{"AM/PM"};

    // Cultural color preferences
    QColor primaryCulturalColor;
    QColor secondaryCulturalColor;
    QList<QColor> culturalColorPalette;

    // Accessibility preferences
    bool preferHighContrast{false};
    bool preferReducedMotion{false};
    double preferredFontSize{14.0};

    // Custom properties for extensibility
    QMap<QString, QVariant> customProperties;
};

// Translation manager for comprehensive i18n support
class FluentTranslationManager : public QObject {
    Q_OBJECT

public:
    static FluentTranslationManager& instance();

    // Locale management
    void setCurrentLocale(const QLocale& locale);
    QLocale getCurrentLocale() const { return m_currentLocale; }

    void addSupportedLocale(const FluentLocaleInfo& localeInfo);
    QList<FluentLocaleInfo> getSupportedLocales() const;
    FluentLocaleInfo getLocaleInfo(const QLocale& locale) const;

    // Translation loading
    bool loadTranslations(const QString& directory);
    bool loadTranslation(const QLocale& locale, const QString& domain = "");
    void unloadTranslations();
    void reloadTranslations();

    // Translation functions
    QString translate(const QString& key, const QString& defaultText = "",
                      const TranslationContext& context = {});

    QString translatePlural(const QString& key, int count,
                            const QString& defaultSingular,
                            const QString& defaultPlural = "",
                            const TranslationContext& context = {});

    QString translateWithArgs(const QString& key, const QStringList& args,
                              const QString& defaultText = "",
                              const TranslationContext& context = {});

    // Context-aware translation
    QString translateInContext(const QString& context, const QString& key,
                               const QString& defaultText = "");

    // Dynamic translation registration
    void registerTranslation(const QString& key, const QString& text,
                             const TranslationContext& context = {});
    void registerPluralTranslation(
        const QString& key,
        const std::unordered_map<FluentPluralRule, QString>& forms,
        const TranslationContext& context = {});

    // Translation validation and management
    QStringList getMissingTranslations(const QLocale& locale) const;
    double getTranslationCompleteness(const QLocale& locale) const;
    void exportTranslations(const QLocale& locale,
                            const QString& filename) const;
    void importTranslations(const QLocale& locale, const QString& filename);

    // RTL support
    FluentTextDirection getTextDirection() const;
    bool isRightToLeft() const;
    void setLayoutDirection(Qt::LayoutDirection direction);
    Qt::LayoutDirection getLayoutDirection() const;

    // Formatting utilities
    QString formatNumber(double number, FluentNumberFormat format =
                                            FluentNumberFormat::Decimal) const;
    QString formatCurrency(double amount,
                           const QString& currencyCode = "") const;
    QString formatPercent(double value) const;
    QString formatDateTime(
        const QDateTime& dateTime,
        FluentDateTimeFormat format = FluentDateTimeFormat::Medium) const;
    QString formatDate(
        const QDate& date,
        FluentDateTimeFormat format = FluentDateTimeFormat::Medium) const;
    QString formatTime(
        const QTime& time,
        FluentDateTimeFormat format = FluentDateTimeFormat::Medium) const;
    QString formatRelativeTime(const QDateTime& dateTime) const;

    // Pluralization
    FluentPluralRule getPluralRule(int count) const;
    QString formatPlural(int count, const QString& singular,
                         const QString& plural = "") const;

    // Enhanced locale-specific utilities
    QString getLocalizedString(const QString& key) const;
    QStringList getLocalizedStringList(const QString& key) const;
    QString getDecimalSeparator() const;
    QString getThousandsSeparator() const;
    QString getCurrencySymbol() const;
    QString getCurrencyCode() const;
    QString getListSeparator() const;
    QString getListLastSeparator() const;
    QString getQuotationMarks(bool primary = true) const;

    // Cultural adaptation
    void enableCulturalAdaptation(FluentCulturalAdaptation adaptation,
                                  bool enabled = true);
    bool isCulturalAdaptationEnabled(FluentCulturalAdaptation adaptation) const;
    QVariant getCulturalPreference(FluentCulturalAdaptation adaptation,
                                   const QString& key) const;
    void setCulturalPreference(FluentCulturalAdaptation adaptation,
                               const QString& key, const QVariant& value);

    // Text transformation
    QString transformText(const QString& text, FluentTextCase textCase) const;
    QString formatList(const QStringList& items) const;
    QString formatOrdinal(int number) const;
    QString formatCardinal(int number) const;

    // Advanced formatting
    QString formatAddress(
        const QMap<QString, QString>& addressComponents) const;
    QString formatPhoneNumber(const QString& phoneNumber) const;
    QString formatName(const QString& firstName, const QString& lastName,
                       const QString& title = "") const;

    // Locale detection and suggestion
    QLocale detectLocaleFromText(const QString& text) const;
    QList<QLocale> suggestLocales(const QString& userInput) const;
    bool isLocaleSupported(const QLocale& locale) const;

    // Translation caching
    void enableTranslationCache(bool enabled = true) {
        m_cacheEnabled = enabled;
    }
    bool isTranslationCacheEnabled() const { return m_cacheEnabled; }
    void clearTranslationCache();

    // Hot reloading for development
    void enableHotReload(bool enabled = true);
    bool isHotReloadEnabled() const { return m_hotReloadEnabled; }
    void watchTranslationFiles(const QString& directory);

signals:
    void localeChanged(const QLocale& oldLocale, const QLocale& newLocale);
    void textDirectionChanged(FluentTextDirection direction);
    void translationsLoaded(const QLocale& locale);
    void translationMissing(const QString& key, const QLocale& locale);
    void hotReloadTriggered(const QString& filename);

private slots:
    void onTranslationFileChanged(const QString& filename);
    void processTranslationQueue();

private:
    FluentTranslationManager();
    void initializeDefaultLocales();
    void setupPluralRules();
    QString getCachedTranslation(const QString& key) const;
    void setCachedTranslation(const QString& key, const QString& translation);
    FluentPluralRule calculatePluralRule(int count,
                                         const QLocale& locale) const;

private:
    QLocale m_currentLocale;
    QList<FluentLocaleInfo> m_supportedLocales;

    // Translation storage
    std::unordered_map<QString, TranslationEntry> m_translations;
    QMap<QLocale, std::unique_ptr<QTranslator>> m_translators;

    // Caching
    bool m_cacheEnabled{true};
    mutable std::unordered_map<QString, QString> m_translationCache;
    mutable QMutex m_cacheMutex;

    // Hot reload
    bool m_hotReloadEnabled{false};
    QStringList m_watchedDirectories;
    QTimer* m_reloadTimer;

    // Thread safety
    mutable QMutex m_translationMutex;
    mutable QMutex m_localeMutex;
};

// Locale-aware formatting utilities
class FluentFormatter {
public:
    explicit FluentFormatter(const QLocale& locale = QLocale());

    // Number formatting
    QString formatNumber(double number, int precision = -1) const;
    QString formatInteger(qint64 number) const;
    QString formatCurrency(double amount,
                           const QString& currencyCode = "") const;
    QString formatPercent(double value, int precision = 2) const;
    QString formatFileSize(qint64 bytes) const;

    // Date/time formatting
    QString formatDateTime(const QDateTime& dateTime,
                           const QString& format = "") const;
    QString formatDate(const QDate& date, const QString& format = "") const;
    QString formatTime(const QTime& time, const QString& format = "") const;
    QString formatDuration(qint64 milliseconds) const;
    QString formatRelativeTime(const QDateTime& dateTime) const;

    // List formatting
    QString formatList(const QStringList& items,
                       const QString& separator = ", ",
                       const QString& lastSeparator = " and ") const;

    // Text formatting
    QString formatTitle(const QString& text) const;
    QString formatSentence(const QString& text) const;

    // Locale information
    QLocale locale() const { return m_locale; }
    void setLocale(const QLocale& locale) { m_locale = locale; }

private:
    QLocale m_locale;
};

// Translation macros for convenience
#define FLUENT_TR(key, defaultText)                                 \
    FluentQt::Core::FluentTranslationManager::instance().translate( \
        key, defaultText)

#define FLUENT_TR_CTX(context, key, defaultText)                             \
    FluentQt::Core::FluentTranslationManager::instance().translateInContext( \
        context, key, defaultText)

#define FLUENT_TR_PLURAL(key, count, singular, plural)                    \
    FluentQt::Core::FluentTranslationManager::instance().translatePlural( \
        key, count, singular, plural)

#define FLUENT_TR_ARGS(key, args, defaultText)                              \
    FluentQt::Core::FluentTranslationManager::instance().translateWithArgs( \
        key, args, defaultText)

// RTL layout utilities
class FluentRTLUtils {
public:
    static bool isRTLLocale(const QLocale& locale);
    static Qt::LayoutDirection getLayoutDirection(const QLocale& locale);
    static QString mirrorText(const QString& text);
    static QRect mirrorRect(const QRect& rect, const QRect& container);
    static QPoint mirrorPoint(const QPoint& point, const QRect& container);
    static Qt::Alignment mirrorAlignment(Qt::Alignment alignment);
    static void applyRTLLayout(QWidget* widget);
    static void applyRTLStyleSheet(QWidget* widget,
                                   const QString& ltrStyleSheet);
};

}  // namespace FluentQt::Core
