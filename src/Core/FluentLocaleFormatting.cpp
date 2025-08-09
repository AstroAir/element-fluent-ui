// src/Core/FluentLocaleFormatting.cpp
#include "FluentQt/Core/FluentLocaleFormatting.h"
#include <QApplication>
#include <QDebug>
#include <QMutexLocker>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <algorithm>
#include <cmath>

namespace FluentQt::Core {

FluentLocaleFormattingManager& FluentLocaleFormattingManager::instance() {
    static FluentLocaleFormattingManager instance;
    return instance;
}

FluentLocaleFormattingManager::FluentLocaleFormattingManager() {
    // Initialize default formatting configurations
    qDebug() << "FluentLocaleFormattingManager initialized";
}

void FluentLocaleFormattingManager::setFormattingConfig(const QLocale& locale, const FluentFormattingConfig& config) {
    QMutexLocker locker(&m_configMutex);
    m_formattingConfigs[locale] = config;
    emit formattingConfigChanged(locale);
}

FluentFormattingConfig FluentLocaleFormattingManager::getFormattingConfig(const QLocale& locale) const {
    QMutexLocker locker(&m_configMutex);
    
    auto it = m_formattingConfigs.find(locale);
    if (it != m_formattingConfigs.end()) {
        return it.value();
    }
    
    return createDefaultConfig(locale);
}

FluentFormattingResult FluentLocaleFormattingManager::formatNumber(double number, const QLocale& locale, FluentNumberStyle style) {
    FluentFormattingResult result;
    result.type = FluentFormattingType::Number;
    result.originalText = QString::number(number);
    
    auto config = getFormattingConfig(locale);
    
    switch (style) {
        case FluentNumberStyle::Decimal:
            result.formattedText = locale.toString(number, 'f', config.decimalPlaces >= 0 ? config.decimalPlaces : 2);
            break;
        case FluentNumberStyle::Scientific:
            result.formattedText = locale.toString(number, 'e', 2);
            break;
        case FluentNumberStyle::Compact:
            result.formattedText = formatCompactNumberInternal(number, locale);
            break;
        case FluentNumberStyle::Spelled:
            result.formattedText = formatSpelledNumberInternal(static_cast<int>(number), locale);
            break;
        default:
            result.formattedText = locale.toString(number);
            break;
    }
    
    return result;
}

FluentFormattingResult FluentLocaleFormattingManager::formatInteger(qint64 number, const QLocale& locale, FluentNumberStyle style) {
    FluentFormattingResult result;
    result.type = FluentFormattingType::Number;
    result.originalText = QString::number(number);
    
    switch (style) {
        case FluentNumberStyle::Decimal:
            result.formattedText = locale.toString(number);
            break;
        case FluentNumberStyle::Compact:
            result.formattedText = formatCompactNumberInternal(static_cast<double>(number), locale);
            break;
        case FluentNumberStyle::Spelled:
            result.formattedText = formatSpelledNumberInternal(static_cast<int>(number), locale);
            break;
        case FluentNumberStyle::Ordinal:
            result.formattedText = formatOrdinalInternal(static_cast<int>(number), locale);
            break;
        case FluentNumberStyle::Roman:
            result.formattedText = formatRomanNumeralInternal(static_cast<int>(number));
            break;
        default:
            result.formattedText = locale.toString(number);
            break;
    }
    
    return result;
}

FluentFormattingResult FluentLocaleFormattingManager::formatCurrency(double amount, const QString& currencyCode, const QLocale& locale) {
    FluentFormattingResult result;
    result.type = FluentFormattingType::Currency;
    result.originalText = QString::number(amount);
    
    // Use QLocale's currency formatting
    result.formattedText = locale.toCurrencyString(amount, currencyCode);
    
    // If QLocale doesn't support the currency, format manually
    if (result.formattedText.isEmpty() || result.formattedText == QString::number(amount)) {
        QString symbol = getCurrencySymbol(currencyCode, locale);
        QString numberPart = locale.toString(amount, 'f', 2);
        
        // Different locales have different currency formatting conventions
        if (locale.language() == QLocale::English && locale.country() == QLocale::UnitedStates) {
            result.formattedText = symbol + numberPart;
        } else {
            result.formattedText = numberPart + " " + symbol;
        }
    }
    
    return result;
}

FluentFormattingResult FluentLocaleFormattingManager::formatPercentage(double percentage, const QLocale& locale, int decimalPlaces) {
    FluentFormattingResult result;
    result.type = FluentFormattingType::Percentage;
    result.originalText = QString::number(percentage);
    
    double percentValue = percentage * 100.0;
    result.formattedText = locale.toString(percentValue, 'f', decimalPlaces) + "%";
    
    return result;
}

FluentFormattingResult FluentLocaleFormattingManager::formatDateTime(const QDateTime& dateTime, const QLocale& locale, FluentDateTimeStyle style) {
    FluentFormattingResult result;
    result.type = FluentFormattingType::DateTime;
    result.originalText = dateTime.toString(Qt::ISODate);
    
    QLocale::FormatType formatType;
    switch (style) {
        case FluentDateTimeStyle::Short:
            formatType = QLocale::ShortFormat;
            break;
        case FluentDateTimeStyle::Long:
            formatType = QLocale::LongFormat;
            break;
        case FluentDateTimeStyle::Full:
            formatType = QLocale::LongFormat; // QLocale doesn't have Full format
            break;
        case FluentDateTimeStyle::Relative:
            result.formattedText = formatRelativeDateTimeInternal(dateTime, locale);
            return result;
        case FluentDateTimeStyle::ISO:
            result.formattedText = dateTime.toString(Qt::ISODate);
            return result;
        default:
            formatType = QLocale::ShortFormat;
            break;
    }
    
    result.formattedText = locale.toString(dateTime, formatType);
    return result;
}

FluentFormattingResult FluentLocaleFormattingManager::formatDate(const QDate& date, const QLocale& locale, FluentDateTimeStyle style) {
    FluentFormattingResult result;
    result.type = FluentFormattingType::DateTime;
    result.originalText = date.toString(Qt::ISODate);
    
    QLocale::FormatType formatType;
    switch (style) {
        case FluentDateTimeStyle::Short:
            formatType = QLocale::ShortFormat;
            break;
        case FluentDateTimeStyle::Long:
            formatType = QLocale::LongFormat;
            break;
        default:
            formatType = QLocale::ShortFormat;
            break;
    }
    
    result.formattedText = locale.toString(date, formatType);
    return result;
}

FluentFormattingResult FluentLocaleFormattingManager::formatTime(const QTime& time, const QLocale& locale, FluentDateTimeStyle style) {
    FluentFormattingResult result;
    result.type = FluentFormattingType::DateTime;
    result.originalText = time.toString(Qt::ISODate);
    
    QLocale::FormatType formatType;
    switch (style) {
        case FluentDateTimeStyle::Short:
            formatType = QLocale::ShortFormat;
            break;
        case FluentDateTimeStyle::Long:
            formatType = QLocale::LongFormat;
            break;
        default:
            formatType = QLocale::ShortFormat;
            break;
    }
    
    result.formattedText = locale.toString(time, formatType);
    return result;
}

FluentFormattingResult FluentLocaleFormattingManager::formatRelativeDateTime(const QDateTime& dateTime, const QLocale& locale) {
    FluentFormattingResult result;
    result.type = FluentFormattingType::DateTime;
    result.originalText = dateTime.toString(Qt::ISODate);
    result.formattedText = formatRelativeDateTimeInternal(dateTime, locale);
    return result;
}

FluentFormattingResult FluentLocaleFormattingManager::formatDuration(qint64 milliseconds, const QLocale& locale) {
    FluentFormattingResult result;
    result.type = FluentFormattingType::Duration;
    result.originalText = QString::number(milliseconds);
    
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    qint64 hours = minutes / 60;
    qint64 days = hours / 24;
    
    seconds %= 60;
    minutes %= 60;
    hours %= 24;
    
    QStringList parts;
    
    if (days > 0) {
        parts << QString("%1 %2").arg(days).arg(days == 1 ? "day" : "days");
    }
    if (hours > 0) {
        parts << QString("%1 %2").arg(hours).arg(hours == 1 ? "hour" : "hours");
    }
    if (minutes > 0) {
        parts << QString("%1 %2").arg(minutes).arg(minutes == 1 ? "minute" : "minutes");
    }
    if (seconds > 0 || parts.isEmpty()) {
        parts << QString("%1 %2").arg(seconds).arg(seconds == 1 ? "second" : "seconds");
    }
    
    // Join with locale-appropriate separators
    if (parts.size() == 1) {
        result.formattedText = parts.first();
    } else if (parts.size() == 2) {
        result.formattedText = parts.join(" and ");
    } else {
        QString last = parts.takeLast();
        result.formattedText = parts.join(", ") + ", and " + last;
    }
    
    return result;
}

FluentFormattingResult FluentLocaleFormattingManager::formatList(const QStringList& items, const QLocale& locale) {
    FluentFormattingResult result;
    result.type = FluentFormattingType::List;
    result.originalText = items.join("|");
    
    auto config = getFormattingConfig(locale);
    
    if (items.isEmpty()) {
        result.formattedText = "";
    } else if (items.size() == 1) {
        result.formattedText = items.first();
    } else if (items.size() == 2) {
        result.formattedText = items.first() + config.listPairSeparator + items.last();
    } else {
        QStringList allButLast = items.mid(0, items.size() - 1);
        result.formattedText = allButLast.join(config.listSeparator) + config.listLastSeparator + items.last();
    }
    
    return result;
}

FluentFormattingResult FluentLocaleFormattingManager::formatName(const QString& given, const QString& family, const QLocale& locale, FluentNameStyle style) {
    FluentFormattingResult result;
    result.type = FluentFormattingType::Name;
    result.originalText = given + "|" + family;
    
    switch (style) {
        case FluentNameStyle::Western:
            result.formattedText = given + " " + family;
            break;
        case FluentNameStyle::Eastern:
            result.formattedText = family + " " + given;
            break;
        case FluentNameStyle::Informal:
            result.formattedText = given;
            break;
        case FluentNameStyle::Initials:
            result.formattedText = QString("%1. %2.").arg(given.isEmpty() ? "" : given.left(1)).arg(family.isEmpty() ? "" : family.left(1));
            break;
        default:
            // Use locale-appropriate default
            if (locale.language() == QLocale::Chinese || locale.language() == QLocale::Japanese || locale.language() == QLocale::Korean) {
                result.formattedText = family + " " + given;
            } else {
                result.formattedText = given + " " + family;
            }
            break;
    }
    
    return result;
}

// Private helper methods
QString FluentLocaleFormattingManager::formatCompactNumberInternal(double number, const QLocale& locale) {
    if (std::abs(number) < 1000) {
        return locale.toString(number, 'f', 0);
    }
    
    const QStringList suffixes = {"", "K", "M", "B", "T"};
    int suffixIndex = 0;
    double compactNumber = number;
    
    while (std::abs(compactNumber) >= 1000 && suffixIndex < suffixes.size() - 1) {
        compactNumber /= 1000.0;
        suffixIndex++;
    }
    
    QString numberPart = locale.toString(compactNumber, 'f', compactNumber == static_cast<int>(compactNumber) ? 0 : 1);
    return numberPart + suffixes[suffixIndex];
}

QString FluentLocaleFormattingManager::formatSpelledNumberInternal(int number, const QLocale& locale) {
    // This is a simplified implementation for English
    // A full implementation would support multiple languages
    
    if (locale.language() != QLocale::English) {
        return QString::number(number); // Fallback to numeric
    }
    
    const QStringList ones = {"", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
                             "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", 
                             "seventeen", "eighteen", "nineteen"};
    const QStringList tens = {"", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"};
    
    if (number == 0) return "zero";
    if (number < 0) return "negative " + formatSpelledNumberInternal(-number, locale);
    if (number < 20) return ones[number];
    if (number < 100) {
        return tens[number / 10] + (number % 10 ? "-" + ones[number % 10] : "");
    }
    if (number < 1000) {
        return ones[number / 100] + " hundred" + (number % 100 ? " " + formatSpelledNumberInternal(number % 100, locale) : "");
    }
    
    return QString::number(number); // Fallback for larger numbers
}

QString FluentLocaleFormattingManager::formatOrdinalInternal(int number, const QLocale& locale) {
    if (locale.language() != QLocale::English) {
        return QString::number(number) + "."; // Fallback
    }
    
    QString suffix;
    int lastDigit = number % 10;
    int lastTwoDigits = number % 100;
    
    if (lastTwoDigits >= 11 && lastTwoDigits <= 13) {
        suffix = "th";
    } else {
        switch (lastDigit) {
            case 1: suffix = "st"; break;
            case 2: suffix = "nd"; break;
            case 3: suffix = "rd"; break;
            default: suffix = "th"; break;
        }
    }
    
    return QString::number(number) + suffix;
}

QString FluentLocaleFormattingManager::formatRomanNumeralInternal(int number) {
    if (number <= 0 || number > 3999) {
        return QString::number(number); // Fallback
    }
    
    const QStringList thousands = {"", "M", "MM", "MMM"};
    const QStringList hundreds = {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
    const QStringList tens = {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
    const QStringList ones = {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};
    
    return thousands[number / 1000] +
           hundreds[(number % 1000) / 100] +
           tens[(number % 100) / 10] +
           ones[number % 10];
}

QString FluentLocaleFormattingManager::formatRelativeDateTimeInternal(const QDateTime& dateTime, const QLocale& locale) {
    QDateTime now = QDateTime::currentDateTime();
    qint64 secondsDiff = dateTime.secsTo(now);
    
    if (secondsDiff < 0) {
        // Future time
        secondsDiff = -secondsDiff;
        if (secondsDiff < 60) {
            return "in a few seconds";
        } else if (secondsDiff < 3600) {
            int minutes = secondsDiff / 60;
            return QString("in %1 %2").arg(minutes).arg(minutes == 1 ? "minute" : "minutes");
        } else if (secondsDiff < 86400) {
            int hours = secondsDiff / 3600;
            return QString("in %1 %2").arg(hours).arg(hours == 1 ? "hour" : "hours");
        } else {
            int days = secondsDiff / 86400;
            return QString("in %1 %2").arg(days).arg(days == 1 ? "day" : "days");
        }
    } else {
        // Past time
        if (secondsDiff < 60) {
            return "a few seconds ago";
        } else if (secondsDiff < 3600) {
            int minutes = secondsDiff / 60;
            return QString("%1 %2 ago").arg(minutes).arg(minutes == 1 ? "minute" : "minutes");
        } else if (secondsDiff < 86400) {
            int hours = secondsDiff / 3600;
            return QString("%1 %2 ago").arg(hours).arg(hours == 1 ? "hour" : "hours");
        } else {
            int days = secondsDiff / 86400;
            return QString("%1 %2 ago").arg(days).arg(days == 1 ? "day" : "days");
        }
    }
}

FluentFormattingConfig FluentLocaleFormattingManager::createDefaultConfig(const QLocale& locale) const {
    FluentFormattingConfig config;
    config.locale = locale;
    
    // Set locale-specific defaults
    if (locale.language() == QLocale::English) {
        config.listSeparator = ", ";
        config.listLastSeparator = ", and ";
        config.listPairSeparator = " and ";
    } else if (locale.language() == QLocale::German) {
        config.listSeparator = ", ";
        config.listLastSeparator = " und ";
        config.listPairSeparator = " und ";
    } else if (locale.language() == QLocale::French) {
        config.listSeparator = ", ";
        config.listLastSeparator = " et ";
        config.listPairSeparator = " et ";
    } else {
        config.listSeparator = ", ";
        config.listLastSeparator = " and ";
        config.listPairSeparator = " and ";
    }
    
    return config;
}

QString FluentLocaleFormattingManager::getCurrencySymbol(const QString& currencyCode, const QLocale& locale) const {
    // Common currency symbols
    static const QMap<QString, QString> currencySymbols = {
        {"USD", "$"}, {"EUR", "€"}, {"GBP", "£"}, {"JPY", "¥"},
        {"CNY", "¥"}, {"KRW", "₩"}, {"INR", "₹"}, {"RUB", "₽"},
        {"CAD", "C$"}, {"AUD", "A$"}, {"CHF", "CHF"}, {"SEK", "kr"},
        {"NOK", "kr"}, {"DKK", "kr"}, {"PLN", "zł"}, {"CZK", "Kč"},
        {"HUF", "Ft"}, {"BGN", "лв"}, {"RON", "lei"}, {"HRK", "kn"}
    };
    
    auto it = currencySymbols.find(currencyCode);
    if (it != currencySymbols.end()) {
        return it.value();
    }
    
    return currencyCode; // Fallback to currency code
}

void FluentLocaleFormattingManager::onLocaleChanged(const QLocale& locale) {
    // Respond to locale changes by updating formatting configurations
    auto config = createDefaultConfig(locale);
    setFormattingConfig(locale, config);

    qDebug() << "Locale changed to:" << locale.name() << "- updating formatting configuration";
}

// Utility function implementations
namespace FluentFormattingUtils {

QString formatCompactNumber(double number, const QLocale& locale, int precision) {
    return FluentLocaleFormattingManager::instance().formatCompactNumberInternal(number, locale);
}

QString formatOrdinalNumber(int number, const QLocale& locale) {
    return FluentLocaleFormattingManager::instance().formatOrdinalInternal(number, locale);
}

QString formatRomanNumeral(int number) {
    return FluentLocaleFormattingManager::instance().formatRomanNumeralInternal(number);
}

QString formatRelativeTime(const QDateTime& dateTime, const QLocale& locale) {
    return FluentLocaleFormattingManager::instance().formatRelativeDateTimeInternal(dateTime, locale);
}

QString formatBytes(qint64 bytes, const QLocale& locale, bool binary) {
    const double base = binary ? 1024.0 : 1000.0;
    const QStringList suffixes = binary ? 
        QStringList{"B", "KiB", "MiB", "GiB", "TiB", "PiB"} :
        QStringList{"B", "KB", "MB", "GB", "TB", "PB"};
    
    if (bytes < base) {
        return locale.toString(bytes) + " " + suffixes[0];
    }
    
    int suffixIndex = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= base && suffixIndex < suffixes.size() - 1) {
        size /= base;
        suffixIndex++;
    }
    
    return locale.toString(size, 'f', 1) + " " + suffixes[suffixIndex];
}

} // namespace FluentFormattingUtils

} // namespace FluentQt::Core
