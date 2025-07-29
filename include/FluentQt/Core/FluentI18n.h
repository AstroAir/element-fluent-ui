// include/FluentQt/Core/FluentI18n.h
#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QLocale>
#include <QTranslator>
#include <QDateTime>
#include <QDir>
#include <QMutex>
#include <QTimer>
#include <functional>
#include <unordered_map>
#include <memory>

namespace FluentQt::Core {

// Text direction for RTL support
enum class FluentTextDirection {
    LeftToRight,
    RightToLeft,
    Auto  // Determined by locale
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
    Short,      // 12/31/99, 3:30 PM
    Medium,     // Dec 31, 1999, 3:30:00 PM
    Long,       // December 31, 1999, 3:30:00 PM PST
    Full,       // Friday, December 31, 1999, 3:30:00 PM PST
    Relative,   // 2 hours ago, tomorrow
    Custom      // User-defined format
};

// Pluralization rules
enum class FluentPluralRule {
    Zero,       // 0 items
    One,        // 1 item
    Two,        // 2 items (for languages with dual)
    Few,        // few items (2-4 in some languages)
    Many,       // many items (5+ in some languages)
    Other       // default/other
};

// Translation context for better organization
struct TranslationContext {
    QString domain;         // Translation domain (e.g., "buttons", "messages")
    QString component;      // Component name
    QString feature;        // Feature name
    QStringList tags;       // Additional tags for categorization
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

// Locale information with extended properties
struct FluentLocaleInfo {
    QLocale locale;
    QString displayName;
    QString nativeName;
    FluentTextDirection textDirection;
    QString dateFormat;
    QString timeFormat;
    QString numberFormat;
    QString currencySymbol;
    QStringList supportedFeatures;
    bool isRTL{false};
    bool isComplete{false};  // Translation completeness
    double completionPercentage{0.0};
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
    
    QString translatePlural(const QString& key, int count, const QString& defaultSingular,
                           const QString& defaultPlural = "", const TranslationContext& context = {});
    
    QString translateWithArgs(const QString& key, const QStringList& args,
                             const QString& defaultText = "", const TranslationContext& context = {});
    
    // Context-aware translation
    QString translateInContext(const QString& context, const QString& key,
                              const QString& defaultText = "");
    
    // Dynamic translation registration
    void registerTranslation(const QString& key, const QString& text,
                            const TranslationContext& context = {});
    void registerPluralTranslation(const QString& key,
                                  const std::unordered_map<FluentPluralRule, QString>& forms,
                                  const TranslationContext& context = {});
    
    // Translation validation and management
    QStringList getMissingTranslations(const QLocale& locale) const;
    double getTranslationCompleteness(const QLocale& locale) const;
    void exportTranslations(const QLocale& locale, const QString& filename) const;
    void importTranslations(const QLocale& locale, const QString& filename);
    
    // RTL support
    FluentTextDirection getTextDirection() const;
    bool isRightToLeft() const;
    void setLayoutDirection(Qt::LayoutDirection direction);
    Qt::LayoutDirection getLayoutDirection() const;
    
    // Formatting utilities
    QString formatNumber(double number, FluentNumberFormat format = FluentNumberFormat::Decimal) const;
    QString formatCurrency(double amount, const QString& currencyCode = "") const;
    QString formatPercent(double value) const;
    QString formatDateTime(const QDateTime& dateTime, FluentDateTimeFormat format = FluentDateTimeFormat::Medium) const;
    QString formatDate(const QDate& date, FluentDateTimeFormat format = FluentDateTimeFormat::Medium) const;
    QString formatTime(const QTime& time, FluentDateTimeFormat format = FluentDateTimeFormat::Medium) const;
    QString formatRelativeTime(const QDateTime& dateTime) const;
    
    // Pluralization
    FluentPluralRule getPluralRule(int count) const;
    QString formatPlural(int count, const QString& singular, const QString& plural = "") const;
    
    // Locale-specific utilities
    QString getLocalizedString(const QString& key) const;
    QStringList getLocalizedStringList(const QString& key) const;
    QString getDecimalSeparator() const;
    QString getThousandsSeparator() const;
    QString getCurrencySymbol() const;
    
    // Translation caching
    void enableTranslationCache(bool enabled = true) { m_cacheEnabled = enabled; }
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
    FluentPluralRule calculatePluralRule(int count, const QLocale& locale) const;

private:
    QLocale m_currentLocale;
    QList<FluentLocaleInfo> m_supportedLocales;
    
    // Translation storage
    std::unordered_map<QString, TranslationEntry> m_translations;
    std::unordered_map<QLocale, std::unique_ptr<QTranslator>> m_translators;
    
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
    QString formatCurrency(double amount, const QString& currencyCode = "") const;
    QString formatPercent(double value, int precision = 2) const;
    QString formatFileSize(qint64 bytes) const;
    
    // Date/time formatting
    QString formatDateTime(const QDateTime& dateTime, const QString& format = "") const;
    QString formatDate(const QDate& date, const QString& format = "") const;
    QString formatTime(const QTime& time, const QString& format = "") const;
    QString formatDuration(qint64 milliseconds) const;
    QString formatRelativeTime(const QDateTime& dateTime) const;
    
    // List formatting
    QString formatList(const QStringList& items, const QString& separator = ", ",
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
#define FLUENT_TR(key, defaultText) \
    FluentQt::Core::FluentTranslationManager::instance().translate(key, defaultText)

#define FLUENT_TR_CTX(context, key, defaultText) \
    FluentQt::Core::FluentTranslationManager::instance().translateInContext(context, key, defaultText)

#define FLUENT_TR_PLURAL(key, count, singular, plural) \
    FluentQt::Core::FluentTranslationManager::instance().translatePlural(key, count, singular, plural)

#define FLUENT_TR_ARGS(key, args, defaultText) \
    FluentQt::Core::FluentTranslationManager::instance().translateWithArgs(key, args, defaultText)

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
    static void applyRTLStyleSheet(QWidget* widget, const QString& ltrStyleSheet);
};

} // namespace FluentQt::Core
