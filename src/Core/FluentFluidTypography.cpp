// src/Core/FluentFluidTypography.cpp
#include "FluentQt/Core/FluentFluidTypography.h"
#include <QApplication>
#include <QDebug>
#include <QMutexLocker>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QTimer>
#include <algorithm>
#include <cmath>

namespace FluentQt::Core {

FluentFluidTypographyManager& FluentFluidTypographyManager::instance() {
    static FluentFluidTypographyManager instance;
    return instance;
}

FluentFluidTypographyManager::FluentFluidTypographyManager() {
    // Initialize default configuration
    m_config = FluentFluidTypographyConfig{};
    
    // Initialize cache cleanup timer
    m_cacheCleanupTimer = new QTimer(this);
    m_cacheCleanupTimer->setInterval(300000); // 5 minutes
    connect(m_cacheCleanupTimer, &QTimer::timeout, this, &FluentFluidTypographyManager::cleanupCache);
    m_cacheCleanupTimer->start();
    
    // Register default typography styles
    registerDefaultStyles();
    
    // Connect to system changes
    connect(qApp, &QApplication::fontDatabaseChanged, this, &FluentFluidTypographyManager::onSystemFontChanged);
    
    qDebug() << "FluentFluidTypographyManager initialized";
}

void FluentFluidTypographyManager::setConfig(const FluentFluidTypographyConfig& config) {
    QMutexLocker locker(&m_configMutex);
    
    if (validateConfigInternal(config)) {
        m_config = config;
        clearCache();
        emit configurationChanged();
    } else {
        qWarning() << "Invalid typography configuration provided";
    }
}

void FluentFluidTypographyManager::setTypographyScale(FluentTypographyScale scale) {
    QMutexLocker locker(&m_configMutex);
    m_config.scale = scale;
    clearCache();
    emit configurationChanged();
}

void FluentFluidTypographyManager::setCustomScaleRatio(double ratio) {
    if (isValidRatio(ratio)) {
        QMutexLocker locker(&m_configMutex);
        m_config.customScaleRatio = ratio;
        if (m_config.scale == FluentTypographyScale::Custom) {
            clearCache();
            emit configurationChanged();
        }
    }
}

double FluentFluidTypographyManager::getScaleRatio() const {
    QMutexLocker locker(&m_configMutex);
    return getScaleRatio(m_config.scale);
}

double FluentFluidTypographyManager::getScaleRatio(FluentTypographyScale scale) const {
    switch (scale) {
        case FluentTypographyScale::MinorSecond: return 1.067;
        case FluentTypographyScale::MajorSecond: return 1.125;
        case FluentTypographyScale::MinorThird: return 1.200;
        case FluentTypographyScale::MajorThird: return 1.250;
        case FluentTypographyScale::PerfectFourth: return 1.333;
        case FluentTypographyScale::AugmentedFourth: return 1.414;
        case FluentTypographyScale::PerfectFifth: return 1.500;
        case FluentTypographyScale::GoldenRatio: return 1.618;
        case FluentTypographyScale::Custom: return m_config.customScaleRatio;
    }
    return 1.250; // Default to major third
}

void FluentFluidTypographyManager::registerStyle(const FluentTypographyStyleDefinition& definition) {
    QMutexLocker locker(&m_configMutex);
    m_styleDefinitions[definition.style] = definition;
    clearCache();
    emit styleRegistered(definition.style);
}

void FluentFluidTypographyManager::unregisterStyle(FluentTypographyStyle style) {
    QMutexLocker locker(&m_configMutex);
    if (m_styleDefinitions.remove(style) > 0) {
        clearCache();
        emit styleUnregistered(style);
    }
}

FluentTypographyStyleDefinition FluentFluidTypographyManager::getStyleDefinition(FluentTypographyStyle style) const {
    QMutexLocker locker(&m_configMutex);
    return m_styleDefinitions.value(style, FluentTypographyStyleDefinition{});
}

FluentCalculatedTypography FluentFluidTypographyManager::calculateFluidTypography(FluentTypographyStyle style, 
                                                                                 const QSize& viewportSize) const {
    // Check cache first
    QString cacheKey = generateCacheKey(style, viewportSize);
    FluentCalculatedTypography cached;
    if (m_config.enableCaching && getFromCache(cacheKey, cached)) {
        return cached;
    }
    
    FluentCalculatedTypography result;
    
    QMutexLocker locker(&m_configMutex);
    auto styleIt = m_styleDefinitions.find(style);
    if (styleIt == m_styleDefinitions.end()) {
        // Return default typography
        result.fontSize = m_config.baseSize;
        result.lineHeight = m_config.baseLineHeight * result.fontSize;
        result.letterSpacing = 0.0;
        result.font = QFont();
        result.isFluid = false;
        result.scaleFactor = 1.0;
        return result;
    }
    
    const FluentTypographyStyleDefinition& definition = styleIt.value();
    
    // Calculate base font size for this style
    double baseFontSize = m_config.baseSize * definition.sizeRatio;
    
    // Calculate fluid font size
    if (m_config.enableFluidScaling) {
        result.fontSize = calculateFluidFontSize(baseFontSize, viewportSize);
        result.isFluid = true;
    } else {
        result.fontSize = baseFontSize;
        result.isFluid = false;
    }
    
    // Apply accessibility scaling
    result.fontSize = applyAccessibilityScaling(result.fontSize);
    
    // Calculate line height
    if (m_config.enableResponsiveLineHeight) {
        result.lineHeight = calculateFluidLineHeight(result.fontSize, viewportSize);
    } else {
        result.lineHeight = result.fontSize * definition.lineHeightRatio;
    }
    
    // Calculate letter spacing
    if (m_config.enableResponsiveLetterSpacing) {
        result.letterSpacing = calculateFluidLetterSpacing(result.fontSize, viewportSize);
    } else {
        result.letterSpacing = definition.letterSpacingEm * result.fontSize;
    }
    
    // Create font
    result.font = createFontFromDefinition(definition, result.fontSize);
    
    // Calculate scale factor
    result.scaleFactor = result.fontSize / baseFontSize;
    
    // Generate CSS class and properties
    result.cssClass = QString("fluent-typography-%1").arg(static_cast<int>(style));
    result.cssProperties = generateCSSProperties(result, definition);
    
    locker.unlock();
    
    // Cache the result
    if (m_config.enableCaching) {
        addToCache(cacheKey, result);
    }
    
    return result;
}

FluentCalculatedTypography FluentFluidTypographyManager::calculateFluidTypography(double baseSize, 
                                                                                 const QSize& viewportSize) const {
    FluentCalculatedTypography result;
    
    QMutexLocker locker(&m_configMutex);
    
    // Calculate fluid font size
    if (m_config.enableFluidScaling) {
        result.fontSize = calculateFluidFontSize(baseSize, viewportSize);
        result.isFluid = true;
    } else {
        result.fontSize = baseSize;
        result.isFluid = false;
    }
    
    // Apply accessibility scaling
    result.fontSize = applyAccessibilityScaling(result.fontSize);
    
    // Calculate line height
    if (m_config.enableResponsiveLineHeight) {
        result.lineHeight = calculateFluidLineHeight(result.fontSize, viewportSize);
    } else {
        result.lineHeight = result.fontSize * m_config.baseLineHeight;
    }
    
    // Calculate letter spacing
    if (m_config.enableResponsiveLetterSpacing) {
        result.letterSpacing = calculateFluidLetterSpacing(result.fontSize, viewportSize);
    } else {
        result.letterSpacing = 0.0;
    }
    
    // Create font
    result.font = QFont();
    result.font.setPointSizeF(result.fontSize);
    
    // Calculate scale factor
    result.scaleFactor = result.fontSize / baseSize;
    
    result.cssClass = "fluent-typography-custom";
    
    return result;
}

QFont FluentFluidTypographyManager::calculateFluidFont(FluentTypographyStyle style, const QSize& viewportSize) const {
    auto typography = calculateFluidTypography(style, viewportSize);
    return typography.font;
}

QFont FluentFluidTypographyManager::calculateFluidFont(const QFont& baseFont, const QSize& viewportSize) const {
    double baseFontSize = baseFont.pointSizeF();
    if (baseFontSize <= 0) {
        baseFontSize = m_config.baseSize;
    }
    
    auto typography = calculateFluidTypography(baseFontSize, viewportSize);
    
    QFont fluidFont = baseFont;
    fluidFont.setPointSizeF(typography.fontSize);
    
    return fluidFont;
}

double FluentFluidTypographyManager::calculateFluidFontSize(double baseSize, const QSize& viewportSize) const {
    QMutexLocker locker(&m_configMutex);
    
    if (!m_config.enableFluidScaling) {
        return baseSize;
    }
    
    double viewportWidth = viewportSize.width();
    
    // Clamp viewport width to configured range
    viewportWidth = clamp(viewportWidth, m_config.minViewport, m_config.maxViewport);
    
    // Calculate fluid size using linear interpolation
    double minSize = std::max(baseSize * 0.8, m_config.minSize);
    double maxSize = std::min(baseSize * 1.5, m_config.maxSize);
    
    double fluidSize = interpolate(minSize, maxSize, m_config.minViewport, m_config.maxViewport, viewportWidth);
    
    // Apply accessibility scaling
    fluidSize = applyAccessibilityScaling(fluidSize);
    
    // Ensure size is within bounds
    return clamp(fluidSize, m_config.minSize, m_config.maxSize);
}

double FluentFluidTypographyManager::calculateFluidLineHeight(double fontSize, const QSize& viewportSize) const {
    Q_UNUSED(viewportSize)
    
    QMutexLocker locker(&m_configMutex);
    
    if (!m_config.enableResponsiveLineHeight) {
        return fontSize * m_config.baseLineHeight;
    }
    
    // Calculate optimal line height based on font size
    // Smaller fonts need relatively larger line heights for readability
    double ratio = m_config.baseLineHeight;
    
    if (fontSize < 14.0) {
        ratio += 0.2; // Increase line height for small text
    } else if (fontSize > 24.0) {
        ratio -= 0.1; // Decrease line height for large text
    }
    
    ratio = clamp(ratio, m_config.minLineHeight, m_config.maxLineHeight);
    
    return fontSize * ratio;
}

double FluentFluidTypographyManager::calculateFluidLetterSpacing(double fontSize, const QSize& viewportSize) const {
    Q_UNUSED(viewportSize)
    
    QMutexLocker locker(&m_configMutex);
    
    if (!m_config.enableResponsiveLetterSpacing) {
        return 0.0;
    }
    
    // Calculate optimal letter spacing based on font size
    // Larger fonts typically need tighter letter spacing
    double spacing = 0.0;
    
    if (fontSize > 32.0) {
        spacing = -0.02 * fontSize; // Tighter spacing for large text
    } else if (fontSize < 12.0) {
        spacing = 0.01 * fontSize;  // Looser spacing for small text
    }
    
    return clamp(spacing, m_config.minLetterSpacing * fontSize, m_config.maxLetterSpacing * fontSize);
}

void FluentFluidTypographyManager::setAccessibilityScaleFactor(double factor) {
    factor = clamp(factor, m_config.minAccessibilityScale, m_config.maxAccessibilityScale);
    
    if (std::abs(m_accessibilityScale - factor) > 0.01) {
        m_accessibilityScale = factor;
        clearCache();
        emit accessibilityScaleChanged(factor);
    }
}

void FluentFluidTypographyManager::clearCache() {
    QMutexLocker locker(&m_cacheMutex);
    m_cache.clear();
    emit cacheCleared();
}

int FluentFluidTypographyManager::getCacheSize() const {
    QMutexLocker locker(&m_cacheMutex);
    return m_cache.size();
}

bool FluentFluidTypographyManager::validateConfiguration() const {
    QMutexLocker locker(&m_configMutex);
    return validateConfigInternal(m_config);
}

QStringList FluentFluidTypographyManager::getValidationErrors() const {
    QStringList errors;
    
    QMutexLocker locker(&m_configMutex);
    
    if (!isValidSize(m_config.baseSize)) {
        errors << "Invalid base size";
    }
    if (!isValidSize(m_config.minSize)) {
        errors << "Invalid minimum size";
    }
    if (!isValidSize(m_config.maxSize)) {
        errors << "Invalid maximum size";
    }
    if (m_config.minSize >= m_config.maxSize) {
        errors << "Minimum size must be less than maximum size";
    }
    if (!isValidViewport(m_config.minViewport)) {
        errors << "Invalid minimum viewport";
    }
    if (!isValidViewport(m_config.maxViewport)) {
        errors << "Invalid maximum viewport";
    }
    if (m_config.minViewport >= m_config.maxViewport) {
        errors << "Minimum viewport must be less than maximum viewport";
    }
    if (!isValidRatio(m_config.customScaleRatio)) {
        errors << "Invalid custom scale ratio";
    }
    
    return errors;
}

// Private helper methods
double FluentFluidTypographyManager::interpolate(double min, double max, double minViewport, 
                                                double maxViewport, double currentViewport) const {
    if (maxViewport <= minViewport) {
        return min;
    }
    
    double ratio = (currentViewport - minViewport) / (maxViewport - minViewport);
    ratio = clamp(ratio, 0.0, 1.0);
    
    return min + (max - min) * ratio;
}

double FluentFluidTypographyManager::clamp(double value, double min, double max) const {
    return std::max(min, std::min(max, value));
}

double FluentFluidTypographyManager::applyAccessibilityScaling(double value) const {
    if (m_config.enableAccessibilityScaling) {
        return value * m_accessibilityScale;
    }
    return value;
}

QFont FluentFluidTypographyManager::createFontFromDefinition(const FluentTypographyStyleDefinition& definition, 
                                                           double fontSize) const {
    QFont font;
    
    // Set font family
    if (!definition.fontFamily.isEmpty()) {
        font.setFamily(definition.fontFamily);
    }
    
    // Set font size
    font.setPointSizeF(fontSize);
    
    // Set font weight
    font.setWeight(static_cast<QFont::Weight>(definition.weight));
    
    // Set italic
    font.setItalic(definition.isItalic);
    
    return font;
}

QString FluentFluidTypographyManager::generateCacheKey(FluentTypographyStyle style, const QSize& viewportSize) const {
    return QString("%1_%2x%3_%4")
           .arg(static_cast<int>(style))
           .arg(viewportSize.width())
           .arg(viewportSize.height())
           .arg(m_accessibilityScale, 0, 'f', 2);
}

void FluentFluidTypographyManager::addToCache(const QString& key, const FluentCalculatedTypography& typography) const {
    QMutexLocker locker(&m_cacheMutex);
    
    // Remove oldest entries if cache is full
    if (m_cache.size() >= m_config.cacheSize) {
        auto it = m_cache.begin();
        m_cache.erase(it);
    }
    
    m_cache[key] = typography;
}

bool FluentFluidTypographyManager::getFromCache(const QString& key, FluentCalculatedTypography& typography) const {
    QMutexLocker locker(&m_cacheMutex);
    
    auto it = m_cache.find(key);
    if (it != m_cache.end()) {
        typography = it.value();
        return true;
    }
    
    return false;
}

void FluentFluidTypographyManager::cleanupCache() {
    QMutexLocker locker(&m_cacheMutex);
    
    // Remove excess entries if cache has grown too large
    while (m_cache.size() > m_config.cacheSize) {
        auto it = m_cache.begin();
        m_cache.erase(it);
    }
}

bool FluentFluidTypographyManager::validateConfigInternal(const FluentFluidTypographyConfig& config) const {
    return isValidSize(config.baseSize) &&
           isValidSize(config.minSize) &&
           isValidSize(config.maxSize) &&
           config.minSize < config.maxSize &&
           isValidViewport(config.minViewport) &&
           isValidViewport(config.maxViewport) &&
           config.minViewport < config.maxViewport &&
           isValidRatio(config.customScaleRatio);
}

bool FluentFluidTypographyManager::isValidSize(double size) const {
    return size > 0.0 && size <= 200.0; // Reasonable font size range
}

bool FluentFluidTypographyManager::isValidViewport(double viewport) const {
    return viewport > 0.0 && viewport <= 10000.0; // Reasonable viewport range
}

bool FluentFluidTypographyManager::isValidRatio(double ratio) const {
    return ratio >= 1.0 && ratio <= 3.0; // Reasonable scale ratio range
}

void FluentFluidTypographyManager::registerDefaultStyles() {
    // Register common typography styles
    FluentTypographyStyleDefinition display;
    display.style = FluentTypographyStyle::Display;
    display.name = "Display";
    display.sizeRatio = 3.0;
    display.weight = FluentTypographyWeight::Bold;
    display.lineHeightRatio = 1.2;
    registerStyle(display);
    
    FluentTypographyStyleDefinition headline;
    headline.style = FluentTypographyStyle::Headline;
    headline.name = "Headline";
    headline.sizeRatio = 2.0;
    headline.weight = FluentTypographyWeight::SemiBold;
    headline.lineHeightRatio = 1.3;
    registerStyle(headline);
    
    FluentTypographyStyleDefinition title;
    title.style = FluentTypographyStyle::Title;
    title.name = "Title";
    title.sizeRatio = 1.5;
    title.weight = FluentTypographyWeight::Medium;
    title.lineHeightRatio = 1.4;
    registerStyle(title);
    
    FluentTypographyStyleDefinition body;
    body.style = FluentTypographyStyle::Body;
    body.name = "Body";
    body.sizeRatio = 1.0;
    body.weight = FluentTypographyWeight::Regular;
    body.lineHeightRatio = 1.5;
    registerStyle(body);
    
    FluentTypographyStyleDefinition caption;
    caption.style = FluentTypographyStyle::Caption;
    caption.name = "Caption";
    caption.sizeRatio = 0.8;
    caption.weight = FluentTypographyWeight::Regular;
    caption.lineHeightRatio = 1.4;
    registerStyle(caption);
}

QMap<QString, QString> FluentFluidTypographyManager::generateCSSProperties(const FluentCalculatedTypography& typography,
                                                                          const FluentTypographyStyleDefinition& definition) const {
    QMap<QString, QString> properties;
    
    properties["font-size"] = QString("%1px").arg(typography.fontSize);
    properties["line-height"] = QString("%1px").arg(typography.lineHeight);
    
    if (std::abs(typography.letterSpacing) > 0.01) {
        properties["letter-spacing"] = QString("%1px").arg(typography.letterSpacing);
    }
    
    properties["font-weight"] = QString::number(static_cast<int>(definition.weight));
    
    if (definition.isItalic) {
        properties["font-style"] = "italic";
    }
    
    if (definition.isUppercase) {
        properties["text-transform"] = "uppercase";
    }
    
    if (!definition.fontFamily.isEmpty()) {
        properties["font-family"] = definition.fontFamily;
    }
    
    return properties;
}

void FluentFluidTypographyManager::onSystemFontChanged() {
    m_systemFontChanged = true;
    clearCache();
    updateFromSystemSettings();
}

void FluentFluidTypographyManager::updateFromSystemSettings() {
    if (m_config.respectUserPreferences) {
        // Update accessibility scale based on system settings
        // This would typically read from system accessibility settings
        // For now, we'll use a placeholder implementation

        clearCache();
        emit configurationChanged();
    }
}

void FluentFluidTypographyManager::onAccessibilitySettingsChanged() {
    // Respond to accessibility settings changes
    updateFromSystemSettings();

    // Update accessibility scale factor if needed
    // This would typically read from system accessibility settings
    double newScale = 1.0; // Placeholder - would read from system
    setAccessibilityScaleFactor(newScale);

    qDebug() << "Accessibility settings changed, updating typography";
}

} // namespace FluentQt::Core
