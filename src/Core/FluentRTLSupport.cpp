// src/Core/FluentRTLSupport.cpp
#include "FluentQt/Core/FluentRTLSupport.h"
#include <QApplication>
#include <QDebug>
#include <QMutexLocker>
#include <QLayoutItem>
#include <QBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <algorithm>

namespace FluentQt::Core {

FluentRTLSupportManager& FluentRTLSupportManager::instance() {
    static FluentRTLSupportManager instance;
    return instance;
}

FluentRTLSupportManager::FluentRTLSupportManager() {
    // Initialize RTL character categories
    m_rtlCategories.insert(QChar::Letter_Other);
    m_rtlCategories.insert(QChar::Mark_NonSpacing);
    
    m_ltrCategories.insert(QChar::Letter_Lowercase);
    m_ltrCategories.insert(QChar::Letter_Uppercase);
    m_ltrCategories.insert(QChar::Letter_Titlecase);
    m_ltrCategories.insert(QChar::Letter_Other);
    
    m_neutralCategories.insert(QChar::Number_DecimalDigit);
    m_neutralCategories.insert(QChar::Number_Letter);
    m_neutralCategories.insert(QChar::Number_Other);
    m_neutralCategories.insert(QChar::Punctuation_Dash);
    m_neutralCategories.insert(QChar::Punctuation_Open);
    m_neutralCategories.insert(QChar::Punctuation_Close);
    
    // Initialize locale-specific RTL features
    m_localeRTLFeatures[QLocale(QLocale::Arabic)] = {"contextual-forms", "ligatures", "diacritics"};
    m_localeRTLFeatures[QLocale(QLocale::Hebrew)] = {"contextual-forms", "diacritics"};
    m_localeRTLFeatures[QLocale(QLocale::Persian)] = {"contextual-forms", "ligatures"};
    m_localeRTLFeatures[QLocale(QLocale::Urdu)] = {"contextual-forms", "ligatures"};
    
    qDebug() << "FluentRTLSupportManager initialized";
}

void FluentRTLSupportManager::setRTLConfig(const FluentRTLConfig& config) {
    QMutexLocker locker(&m_configMutex);
    m_config = config;
    emit rtlModeChanged(config.mode);
}

void FluentRTLSupportManager::setRTLMode(FluentRTLMode mode) {
    QMutexLocker locker(&m_configMutex);
    m_config.mode = mode;
    emit rtlModeChanged(mode);
}

bool FluentRTLSupportManager::isRTLLocale(const QLocale& locale) const {
    return locale.language() == QLocale::Arabic ||
           locale.language() == QLocale::Hebrew ||
           locale.language() == QLocale::Persian ||
           locale.language() == QLocale::Urdu ||
           locale.language() == QLocale::Pashto ||
           locale.language() == QLocale::Sindhi;
}

FluentTextDirection FluentRTLSupportManager::detectTextDirection(const QString& text) const {
    if (text.isEmpty()) {
        return FluentTextDirection::Auto;
    }
    
    // Check cache first
    auto it = m_textDirectionCache.find(text);
    if (it != m_textDirectionCache.end()) {
        return it.value();
    }
    
    FluentTextDirection direction = detectTextDirectionInternal(text);
    
    // Cache the result
    if (m_cachingEnabled && m_textDirectionCache.size() < 1000) {
        m_textDirectionCache[text] = direction;
    }

    // Note: Cannot emit signal from const method
    return direction;
}

FluentBidiTextType FluentRTLSupportManager::analyzeBidiText(const QString& text) const {
    if (text.isEmpty()) {
        return FluentBidiTextType::Neutral;
    }
    
    bool hasRTL = false;
    bool hasLTR = false;
    
    for (const QChar& ch : text) {
        if (isRTLCharacter(ch)) {
            hasRTL = true;
        } else if (isLTRCharacter(ch)) {
            hasLTR = true;
        }
        
        if (hasRTL && hasLTR) {
            return FluentBidiTextType::Mixed;
        }
    }
    
    if (hasRTL) {
        return FluentBidiTextType::RTL;
    } else if (hasLTR) {
        return FluentBidiTextType::LTR;
    } else {
        return FluentBidiTextType::Neutral;
    }
}

bool FluentRTLSupportManager::containsRTLCharacters(const QString& text) const {
    for (const QChar& ch : text) {
        if (isRTLCharacter(ch)) {
            return true;
        }
    }
    return false;
}

bool FluentRTLSupportManager::containsLTRCharacters(const QString& text) const {
    for (const QChar& ch : text) {
        if (isLTRCharacter(ch)) {
            return true;
        }
    }
    return false;
}

FluentRTLAdaptationResult FluentRTLSupportManager::adaptWidget(QWidget* widget, bool recursive) {
    if (!widget) {
        return FluentRTLAdaptationResult{};
    }
    
    FluentRTLAdaptationResult result;
    result.isAdapted = true;
    
    QMutexLocker locker(&m_configMutex);
    
    if (m_config.mode == FluentRTLMode::Disabled) {
        result.isAdapted = false;
        return result;
    }
    
    // Detect or force RTL mode
    bool shouldApplyRTL = false;
    if (m_config.mode == FluentRTLMode::Forced) {
        shouldApplyRTL = true;
    } else if (m_config.mode == FluentRTLMode::Automatic) {
        // Check widget text for RTL content
        QString widgetText = widget->property("text").toString();
        if (widgetText.isEmpty()) {
            // Check for QLabel, QPushButton, etc.
            if (auto* label = qobject_cast<QLabel*>(widget)) {
                widgetText = label->text();
            } else if (auto* button = qobject_cast<QPushButton*>(widget)) {
                widgetText = button->text();
            }
        }
        
        if (!widgetText.isEmpty()) {
            result.detectedDirection = detectTextDirection(widgetText);
            shouldApplyRTL = (result.detectedDirection == FluentTextDirection::RightToLeft);
        }
    }
    
    if (shouldApplyRTL) {
        // Apply RTL adaptations
        adaptWidgetGeometry(widget);
        adaptWidgetAlignment(widget);
        adaptWidgetMargins(widget);
        adaptWidgetText(widget);
        
        result.appliedAdaptations << "geometry" << "alignment" << "margins" << "text";
        
        // Set layout direction
        widget->setLayoutDirection(Qt::RightToLeft);
        result.appliedAdaptations << "layout-direction";
    }
    
    // Recursively adapt child widgets
    if (recursive) {
        for (QWidget* child : widget->findChildren<QWidget*>()) {
            auto childResult = adaptWidget(child, false);
            result.appliedAdaptations.append(childResult.appliedAdaptations);
        }
    }
    
    emit rtlAdaptationApplied(widget);
    return result;
}

FluentRTLAdaptationResult FluentRTLSupportManager::adaptLayout(QLayout* layout, bool recursive) {
    if (!layout) {
        return FluentRTLAdaptationResult{};
    }
    
    FluentRTLAdaptationResult result;
    result.isAdapted = true;
    
    QMutexLocker locker(&m_configMutex);
    
    if (m_config.mode == FluentRTLMode::Disabled) {
        result.isAdapted = false;
        return result;
    }
    
    // Adapt layout direction
    adaptLayoutDirection(layout);
    adaptLayoutSpacing(layout);
    adaptLayoutAlignment(layout);
    adaptLayoutItems(layout);
    
    result.appliedAdaptations << "direction" << "spacing" << "alignment" << "items";
    
    // Recursively adapt child layouts
    if (recursive) {
        for (int i = 0; i < layout->count(); ++i) {
            QLayoutItem* item = layout->itemAt(i);
            if (item && item->layout()) {
                auto childResult = adaptLayout(item->layout(), true);
                result.appliedAdaptations.append(childResult.appliedAdaptations);
            }
        }
    }
    
    return result;
}

QRect FluentRTLSupportManager::mirrorRect(const QRect& rect, const QRect& container) const {
    int newX = container.width() - rect.x() - rect.width();
    return QRect(newX, rect.y(), rect.width(), rect.height());
}

QPoint FluentRTLSupportManager::mirrorPoint(const QPoint& point, const QRect& container) const {
    int newX = container.width() - point.x();
    return QPoint(newX, point.y());
}

QMargins FluentRTLSupportManager::mirrorMargins(const QMargins& margins) const {
    return QMargins(margins.right(), margins.top(), margins.left(), margins.bottom());
}

Qt::Alignment FluentRTLSupportManager::mirrorAlignment(Qt::Alignment alignment) const {
    Qt::Alignment result = alignment;
    
    // Mirror horizontal alignment
    if (alignment & Qt::AlignLeft) {
        result &= ~Qt::AlignLeft;
        result |= Qt::AlignRight;
    } else if (alignment & Qt::AlignRight) {
        result &= ~Qt::AlignRight;
        result |= Qt::AlignLeft;
    }
    
    return result;
}

QString FluentRTLSupportManager::processRTLText(const QString& text, FluentRTLTextAlignment alignment) const {
    if (text.isEmpty()) {
        return text;
    }
    
    QString processedText = text;
    
    // Add bidi markers if needed
    if (m_config.enableBidiSupport) {
        FluentTextDirection direction = detectTextDirection(text);
        if (direction == FluentTextDirection::RightToLeft) {
            processedText = wrapWithBidiIsolation(processedText, direction);
        }
    }
    
    return processedText;
}

QString FluentRTLSupportManager::wrapWithBidiIsolation(const QString& text, FluentTextDirection direction) const {
    if (direction == FluentTextDirection::RightToLeft) {
        return QString("\u2067") + text + QString("\u2069"); // RLI + text + PDI
    } else if (direction == FluentTextDirection::LeftToRight) {
        return QString("\u2066") + text + QString("\u2069"); // LRI + text + PDI
    }
    return text;
}

QString FluentRTLSupportManager::insertBidiMarkers(const QString& text) const {
    QString result = text;
    
    // Insert RLM (Right-to-Left Mark) after RTL text
    for (int i = 0; i < result.length(); ++i) {
        if (isRTLCharacter(result[i])) {
            // Find end of RTL run
            int j = i;
            while (j < result.length() && (isRTLCharacter(result[j]) || isNeutralCharacter(result[j]))) {
                ++j;
            }
            
            // Insert RLM after RTL run
            if (j < result.length()) {
                result.insert(j, QChar(0x200F)); // RLM
            }
            
            i = j;
        }
    }
    
    return result;
}

// Private helper methods
FluentTextDirection FluentRTLSupportManager::detectTextDirectionInternal(const QString& text) const {
    int rtlCount = 0;
    int ltrCount = 0;
    
    for (const QChar& ch : text) {
        if (isRTLCharacter(ch)) {
            rtlCount++;
        } else if (isLTRCharacter(ch)) {
            ltrCount++;
        }
    }
    
    if (rtlCount > ltrCount) {
        return FluentTextDirection::RightToLeft;
    } else if (ltrCount > rtlCount) {
        return FluentTextDirection::LeftToRight;
    } else {
        return FluentTextDirection::Auto;
    }
}

bool FluentRTLSupportManager::isRTLCharacter(const QChar& character) const {
    return m_rtlCategories.contains(character.category()) ||
           character.direction() == QChar::DirR ||
           character.direction() == QChar::DirAL;
}

bool FluentRTLSupportManager::isLTRCharacter(const QChar& character) const {
    return m_ltrCategories.contains(character.category()) ||
           character.direction() == QChar::DirL;
}

bool FluentRTLSupportManager::isNeutralCharacter(const QChar& character) const {
    return m_neutralCategories.contains(character.category()) ||
           character.direction() == QChar::DirEN ||
           character.direction() == QChar::DirES ||
           character.direction() == QChar::DirET ||
           character.direction() == QChar::DirAN ||
           character.direction() == QChar::DirCS ||
           character.direction() == QChar::DirNSM ||
           character.direction() == QChar::DirBN;
}

void FluentRTLSupportManager::adaptWidgetGeometry(QWidget* widget) const {
    // This would involve more complex geometry adaptation
    // For now, we'll just ensure the widget respects RTL layout direction
    widget->setLayoutDirection(Qt::RightToLeft);
}

void FluentRTLSupportManager::adaptWidgetAlignment(QWidget* widget) const {
    // Adapt text alignment for labels and other text widgets
    if (auto* label = qobject_cast<QLabel*>(widget)) {
        Qt::Alignment alignment = label->alignment();
        label->setAlignment(mirrorAlignment(alignment));
    }
}

void FluentRTLSupportManager::adaptWidgetMargins(QWidget* widget) const {
    if (m_config.mirrorMargins) {
        QMargins margins = widget->contentsMargins();
        widget->setContentsMargins(mirrorMargins(margins));
    }
}

void FluentRTLSupportManager::adaptWidgetText(QWidget* widget) const {
    // Process text content for RTL
    if (auto* label = qobject_cast<QLabel*>(widget)) {
        QString text = label->text();
        if (!text.isEmpty()) {
            QString processedText = processRTLText(text);
            label->setText(processedText);
        }
    } else if (auto* button = qobject_cast<QPushButton*>(widget)) {
        QString text = button->text();
        if (!text.isEmpty()) {
            QString processedText = processRTLText(text);
            button->setText(processedText);
        }
    }
}

void FluentRTLSupportManager::adaptLayoutDirection(QLayout* layout) const {
    if (auto* boxLayout = qobject_cast<QBoxLayout*>(layout)) {
        if (boxLayout->direction() == QBoxLayout::LeftToRight) {
            boxLayout->setDirection(QBoxLayout::RightToLeft);
        }
    }
}

void FluentRTLSupportManager::adaptLayoutSpacing(QLayout* layout) const {
    // Layout spacing adaptation would go here
    Q_UNUSED(layout)
}

void FluentRTLSupportManager::adaptLayoutAlignment(QLayout* layout) const {
    // Layout alignment adaptation would go here
    Q_UNUSED(layout)
}

void FluentRTLSupportManager::adaptLayoutItems(QLayout* layout) const {
    // Layout item adaptation would go here
    Q_UNUSED(layout)
}

void FluentRTLSupportManager::onLocaleChanged(const QLocale& locale) {
    // Respond to locale changes by updating RTL configuration
    QMutexLocker locker(&m_configMutex);

    // Update RTL mode based on locale
    if (m_config.mode == FluentRTLMode::Automatic) {
        bool isRTL = isRTLLocale(locale);
        if (isRTL != (m_config.mode == FluentRTLMode::Forced)) {
            // Update locale-specific RTL features
            auto features = m_localeRTLFeatures.value(locale, QStringList());
            qDebug() << "Locale changed to:" << locale.name() << "RTL features:" << features;
        }
    }

    emit rtlModeChanged(m_config.mode);
}

void FluentRTLSupportManager::onLayoutDirectionChanged(Qt::LayoutDirection direction) {
    // Respond to layout direction changes
    QMutexLocker locker(&m_configMutex);

    if (direction == Qt::RightToLeft) {
        qDebug() << "Layout direction changed to RTL";
    } else {
        qDebug() << "Layout direction changed to LTR";
    }

    emit rtlModeChanged(m_config.mode);
}

// Utility function implementations
namespace FluentRTLUtilities {

bool isRTLText(const QString& text) {
    return FluentRTLSupportManager::instance().detectTextDirection(text) == FluentTextDirection::RightToLeft;
}

bool isLTRText(const QString& text) {
    return FluentRTLSupportManager::instance().detectTextDirection(text) == FluentTextDirection::LeftToRight;
}

bool isMixedDirectionText(const QString& text) {
    return FluentRTLSupportManager::instance().analyzeBidiText(text) == FluentBidiTextType::Mixed;
}

QRect mirrorRectInContainer(const QRect& rect, const QRect& container) {
    return FluentRTLSupportManager::instance().mirrorRect(rect, container);
}

QPoint mirrorPointInContainer(const QPoint& point, const QRect& container) {
    return FluentRTLSupportManager::instance().mirrorPoint(point, container);
}

Qt::Alignment mirrorHorizontalAlignment(Qt::Alignment alignment) {
    return FluentRTLSupportManager::instance().mirrorAlignment(alignment);
}

bool isRTLCharacter(QChar character) {
    return FluentRTLSupportManager::instance().isRTLCharacter(character);
}

bool isLTRCharacter(QChar character) {
    return FluentRTLSupportManager::instance().isLTRCharacter(character);
}

QString addRTLMark(const QString& text) {
    return text + QChar(0x200F); // RLM
}

QString addLTRMark(const QString& text) {
    return text + QChar(0x200E); // LRM
}

QString isolateRTLText(const QString& text) {
    return QString("\u2067") + text + QString("\u2069"); // RLI + text + PDI
}

QString isolateLTRText(const QString& text) {
    return QString("\u2066") + text + QString("\u2069"); // LRI + text + PDI
}

} // namespace FluentRTLUtilities

} // namespace FluentQt::Core
