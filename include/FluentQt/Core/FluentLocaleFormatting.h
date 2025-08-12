// include/FluentQt/Core/FluentLocaleFormatting.h
#pragma once

#include <QDate>
#include <QDateTime>
#include <QHash>
#include <QLocale>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QTimeZone>
#include <QUrl>
#include <QVariant>
#include <functional>

#include "FluentI18n.h"

namespace FluentQt::Core {

// Advanced formatting types
enum class FluentFormattingType {
    Number,       // Numeric formatting
    Currency,     // Currency formatting
    Percentage,   // Percentage formatting
    DateTime,     // Date and time formatting
    Duration,     // Duration/time span formatting
    Address,      // Address formatting
    PhoneNumber,  // Phone number formatting
    Name,         // Personal name formatting
    List,         // List formatting
    Measurement,  // Measurement unit formatting
    Custom        // Custom formatting
};

// Number formatting styles
enum class FluentNumberStyle {
    Decimal,      // Standard decimal notation
    Scientific,   // Scientific notation
    Engineering,  // Engineering notation
    Compact,      // Compact notation (1K, 1M, etc.)
    Spelled,      // Spelled out numbers
    Ordinal,      // Ordinal numbers (1st, 2nd, etc.)
    Roman,        // Roman numerals
    Traditional,  // Traditional number systems
    Custom        // Custom number formatting
};

// Date/time formatting styles
enum class FluentDateTimeStyle {
    Short,     // Short format
    Medium,    // Medium format
    Long,      // Long format
    Full,      // Full format
    Relative,  // Relative format (2 hours ago)
    Fuzzy,     // Fuzzy format (about 2 hours ago)
    ISO,       // ISO 8601 format
    RFC,       // RFC format
    Custom     // Custom format
};

// Address formatting styles
enum class FluentAddressStyle {
    Postal,         // Postal address format
    Display,        // Display format
    Compact,        // Compact format
    International,  // International format
    Local,          // Local format
    Custom          // Custom format
};

// Name formatting styles
enum class FluentNameStyle {
    Western,   // Given Family
    Eastern,   // Family Given
    Formal,    // Title Given Family
    Informal,  // Given
    Initials,  // G. F.
    Mononym,   // Single name
    Custom     // Custom format
};

// Measurement unit types
enum class FluentMeasurementUnit {
    Length,       // Distance/length units
    Weight,       // Weight/mass units
    Volume,       // Volume units
    Temperature,  // Temperature units
    Speed,        // Speed/velocity units
    Area,         // Area units
    Time,         // Time units
    Digital,      // Digital storage units
    Currency,     // Currency units
    Custom        // Custom units
};

// Formatting configuration
struct FluentFormattingConfig {
    QLocale locale;
    FluentFormattingStyle style{FluentFormattingStyle::Localized};

    // Number formatting
    FluentNumberStyle numberStyle{FluentNumberStyle::Decimal};
    int decimalPlaces{-1};  // -1 = automatic
    bool useGroupingSeparator{true};
    bool showPositiveSign{false};
    QString customNumberFormat;

    // Currency formatting
    bool showCurrencySymbol{true};
    bool showCurrencyCode{false};
    QString customCurrencyFormat;

    // Date/time formatting
    FluentDateTimeStyle dateStyle{FluentDateTimeStyle::Medium};
    FluentDateTimeStyle timeStyle{FluentDateTimeStyle::Medium};
    QTimeZone timeZone;
    bool use24HourFormat{true};
    bool showTimeZone{false};
    QString customDateTimeFormat;

    // Address formatting
    FluentAddressStyle addressStyle{FluentAddressStyle::Local};
    bool includeCountry{true};
    bool abbreviateRegions{false};
    QString customAddressFormat;

    // Name formatting
    FluentNameStyle nameStyle{FluentNameStyle::Western};
    bool includeTitle{false};
    bool includeSuffix{false};
    QString customNameFormat;

    // List formatting
    QString listSeparator{", "};
    QString listLastSeparator{" and "};
    QString listPairSeparator{" and "};

    // Custom properties
    QMap<QString, QVariant> customProperties;
};

// Formatting result
struct FluentFormattingResult {
    bool isValid{true};
    QString formattedText;
    QString originalText;
    FluentFormattingType type;
    QStringList warnings;
    QMap<QString, QVariant> metadata;
};

// Advanced locale-aware formatting manager
class FluentLocaleFormattingManager : public QObject {
    Q_OBJECT

public:
    static FluentLocaleFormattingManager& instance();

    // Configuration
    void setFormattingConfig(const QLocale& locale,
                             const FluentFormattingConfig& config);
    FluentFormattingConfig getFormattingConfig(const QLocale& locale) const;
    void loadFormattingConfigs(const QString& directory);

    // Number formatting
    FluentFormattingResult formatNumber(
        double number, const QLocale& locale,
        FluentNumberStyle style = FluentNumberStyle::Decimal);
    FluentFormattingResult formatInteger(
        qint64 number, const QLocale& locale,
        FluentNumberStyle style = FluentNumberStyle::Decimal);
    FluentFormattingResult formatPercentage(double percentage,
                                            const QLocale& locale,
                                            int decimalPlaces = 2);
    FluentFormattingResult formatCurrency(double amount,
                                          const QString& currencyCode,
                                          const QLocale& locale);
    FluentFormattingResult formatCompactNumber(double number,
                                               const QLocale& locale);
    FluentFormattingResult formatOrdinal(int number, const QLocale& locale);
    FluentFormattingResult formatSpelledNumber(int number,
                                               const QLocale& locale);

    // Date/time formatting
    FluentFormattingResult formatDateTime(
        const QDateTime& dateTime, const QLocale& locale,
        FluentDateTimeStyle style = FluentDateTimeStyle::Medium);
    FluentFormattingResult formatDate(
        const QDate& date, const QLocale& locale,
        FluentDateTimeStyle style = FluentDateTimeStyle::Medium);
    FluentFormattingResult formatTime(
        const QTime& time, const QLocale& locale,
        FluentDateTimeStyle style = FluentDateTimeStyle::Medium);
    FluentFormattingResult formatRelativeDateTime(const QDateTime& dateTime,
                                                  const QLocale& locale);
    FluentFormattingResult formatDuration(qint64 milliseconds,
                                          const QLocale& locale);
    FluentFormattingResult formatTimeZone(const QTimeZone& timeZone,
                                          const QLocale& locale);

    // Address formatting
    FluentFormattingResult formatAddress(
        const QMap<QString, QString>& addressComponents, const QLocale& locale,
        FluentAddressStyle style = FluentAddressStyle::Local);
    FluentFormattingResult formatPostalAddress(const QString& street,
                                               const QString& city,
                                               const QString& region,
                                               const QString& postalCode,
                                               const QString& country,
                                               const QLocale& locale);

    // Name formatting
    FluentFormattingResult formatName(
        const QString& given, const QString& family, const QLocale& locale,
        FluentNameStyle style = FluentNameStyle::Western);
    FluentFormattingResult formatFullName(
        const QString& title, const QString& given, const QString& middle,
        const QString& family, const QString& suffix, const QLocale& locale);
    FluentFormattingResult formatInitials(const QString& given,
                                          const QString& family,
                                          const QLocale& locale);

    // Phone number formatting
    FluentFormattingResult formatPhoneNumber(const QString& phoneNumber,
                                             const QLocale& locale);
    FluentFormattingResult formatInternationalPhoneNumber(
        const QString& phoneNumber, const QString& countryCode);

    // List formatting
    FluentFormattingResult formatList(const QStringList& items,
                                      const QLocale& locale);
    FluentFormattingResult formatConjunctionList(const QStringList& items,
                                                 const QLocale& locale);
    FluentFormattingResult formatDisjunctionList(const QStringList& items,
                                                 const QLocale& locale);

    // Measurement formatting
    FluentFormattingResult formatMeasurement(double value,
                                             FluentMeasurementUnit unit,
                                             const QString& unitName,
                                             const QLocale& locale);
    FluentFormattingResult formatLength(double meters, const QLocale& locale);
    FluentFormattingResult formatWeight(double kilograms,
                                        const QLocale& locale);
    FluentFormattingResult formatVolume(double liters, const QLocale& locale);
    FluentFormattingResult formatTemperature(double celsius,
                                             const QLocale& locale);
    FluentFormattingResult formatDigitalSize(qint64 bytes,
                                             const QLocale& locale);

    // URL and email formatting
    FluentFormattingResult formatUrl(const QUrl& url, const QLocale& locale);
    FluentFormattingResult formatEmail(const QString& email,
                                       const QLocale& locale);

    // Parsing (reverse formatting)
    double parseNumber(const QString& text, const QLocale& locale);
    QDateTime parseDateTime(const QString& text, const QLocale& locale);
    QMap<QString, QString> parseAddress(const QString& text,
                                        const QLocale& locale);
    QPair<QString, QString> parseName(const QString& text,
                                      const QLocale& locale);
    QString parsePhoneNumber(const QString& text, const QLocale& locale);

    // Utility methods for external access
    QString formatCompactNumberInternal(double number, const QLocale& locale);
    QString formatCompactNumberInternal(double number, const QLocale& locale,
                                        int precision);
    QString formatSpelledNumberInternal(int number, const QLocale& locale);
    QString formatRomanNumeralInternal(int number);
    QString formatRelativeDateTimeInternal(const QDateTime& dateTime,
                                           const QLocale& locale);
    QString formatOrdinalInternal(int number, const QLocale& locale);

    // Validation
    bool validateNumber(const QString& text, const QLocale& locale);
    bool validateDateTime(const QString& text, const QLocale& locale);
    bool validateAddress(const QString& text, const QLocale& locale);
    bool validatePhoneNumber(const QString& text, const QLocale& locale);
    bool validateEmail(const QString& text);

    // Locale-specific utilities
    QString getDecimalSeparator(const QLocale& locale) const;
    QString getThousandsSeparator(const QLocale& locale) const;
    QString getCurrencySymbol(const QString& currencyCode,
                              const QLocale& locale) const;
    QString getDateSeparator(const QLocale& locale) const;
    QString getTimeSeparator(const QLocale& locale) const;
    QStringList getMonthNames(const QLocale& locale,
                              bool abbreviated = false) const;
    QStringList getDayNames(const QLocale& locale,
                            bool abbreviated = false) const;

    // Custom formatting
    void registerCustomFormatter(
        FluentFormattingType type, const QString& name,
        std::function<QString(const QVariant&, const QLocale&)> formatter);
    void unregisterCustomFormatter(FluentFormattingType type,
                                   const QString& name);
    FluentFormattingResult formatCustom(const QVariant& value,
                                        const QString& formatterName,
                                        const QLocale& locale);

    // Pluralization integration
    QString formatWithPluralization(const QString& template_, int count,
                                    const QLocale& locale);
    QString getPluralizationRule(int count, const QLocale& locale);

    // Performance optimization
    void enableCaching(bool enabled = true);
    void clearCache();
    void preloadFormattingData(const QList<QLocale>& locales);

signals:
    void formattingConfigChanged(const QLocale& locale);
    void customFormatterRegistered(FluentFormattingType type,
                                   const QString& name);
    void formattingError(const QString& error, const QVariant& value,
                         const QLocale& locale);

private slots:
    void onLocaleChanged(const QLocale& locale);

private:
    FluentLocaleFormattingManager();

    // Formatting implementations
    QString formatNumberInternal(double number,
                                 const FluentFormattingConfig& config);
    QString formatDateTimeInternal(const QDateTime& dateTime,
                                   const FluentFormattingConfig& config);
    QString formatAddressInternal(const QMap<QString, QString>& components,
                                  const FluentFormattingConfig& config);
    QString formatNameInternal(const QMap<QString, QString>& nameComponents,
                               const FluentFormattingConfig& config);

    // Parsing implementations
    double parseNumberInternal(const QString& text,
                               const FluentFormattingConfig& config);
    QDateTime parseDateTimeInternal(const QString& text,
                                    const FluentFormattingConfig& config);

    // Locale-specific data loading
    void loadLocaleFormattingData(const QLocale& locale);
    FluentFormattingConfig createDefaultConfig(const QLocale& locale) const;

    // Validation helpers
    bool isValidNumberFormat(const QString& text, const QLocale& locale);
    bool isValidDateTimeFormat(const QString& text, const QLocale& locale);

    // Caching
    QString generateCacheKey(const QString& operation, const QVariant& input,
                             const QLocale& locale) const;
    void addToCache(const QString& key, const QString& value);
    bool getFromCache(const QString& key, QString& value) const;

    // Utility functions
    QString applyNumberGrouping(const QString& number, const QLocale& locale);
    QString removeNumberGrouping(const QString& number, const QLocale& locale);
    QString convertToLocalDigits(const QString& number, const QLocale& locale);
    QString convertFromLocalDigits(const QString& number,
                                   const QLocale& locale);

private:
    // Configuration storage
    QHash<QLocale, FluentFormattingConfig> m_formattingConfigs;

    // Custom formatters
    QMap<FluentFormattingType,
         QMap<QString, std::function<QString(const QVariant&, const QLocale&)>>>
        m_customFormatters;

    // Locale-specific data
    QHash<QLocale, QMap<QString, QVariant>> m_localeData;

    // Caching
    bool m_cachingEnabled{true};
    QMap<QString, QString> m_formattingCache;
    mutable QMutex m_cacheMutex;

    // Regular expressions for parsing
    QHash<QLocale, QMap<QString, QRegularExpression>> m_parsingRegexes;

    // Thread safety
    mutable QMutex m_configMutex;
};

// Formatting utility functions
namespace FluentFormattingUtils {
// Number utilities
QString formatCompactNumber(double number, const QLocale& locale,
                            int precision = 1);
QString formatOrdinalNumber(int number, const QLocale& locale);
QString formatRomanNumeral(int number);
bool isValidNumber(const QString& text, const QLocale& locale);

// Date/time utilities
QString formatRelativeTime(const QDateTime& dateTime, const QLocale& locale);
QString formatFuzzyTime(const QDateTime& dateTime, const QLocale& locale);
QString formatDuration(qint64 seconds, const QLocale& locale);
bool isValidDateTime(const QString& text, const QLocale& locale);

// Address utilities
QString formatPostalCode(const QString& postalCode, const QString& countryCode);
bool isValidPostalCode(const QString& postalCode, const QString& countryCode);
QString normalizeAddress(const QString& address);

// Phone number utilities
QString formatPhoneNumber(const QString& phoneNumber,
                          const QString& countryCode);
QString normalizePhoneNumber(const QString& phoneNumber);
bool isValidPhoneNumber(const QString& phoneNumber, const QString& countryCode);

// Name utilities
QString formatWesternName(const QString& given, const QString& family);
QString formatEasternName(const QString& family, const QString& given);
QPair<QString, QString> parseWesternName(const QString& fullName);

// List utilities
QString joinList(const QStringList& items, const QString& separator,
                 const QString& lastSeparator);
QString formatConjunctionList(const QStringList& items, const QLocale& locale);
QString formatDisjunctionList(const QStringList& items, const QLocale& locale);

// Measurement utilities
QString formatBytes(qint64 bytes, const QLocale& locale, bool binary = true);
QString formatDistance(double meters, const QLocale& locale);
QString formatWeight(double grams, const QLocale& locale);
QString formatTemperature(double celsius, const QLocale& locale,
                          bool includeDegreeSymbol = true);
}  // namespace FluentFormattingUtils

}  // namespace FluentQt::Core
