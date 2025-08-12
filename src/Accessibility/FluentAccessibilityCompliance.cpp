// src/Accessibility/FluentAccessibilityCompliance.cpp
#include "FluentQt/Accessibility/FluentAccessibilityCompliance.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentAdvancedTheme.h"

#include <QAccessible>
#include <QAccessibleWidget>
#include <QApplication>
#include <QDebug>
#include <QMutexLocker>
#include <QScreen>
#include <QSettings>
#include <QStyleOption>
#include <QWidget>
#include <cmath>

namespace FluentQt::Accessibility {

FluentAccessibilityComplianceManager&
FluentAccessibilityComplianceManager::instance() {
    static FluentAccessibilityComplianceManager instance;
    return instance;
}

FluentAccessibilityComplianceManager::FluentAccessibilityComplianceManager(
    QObject* parent)
    : QObject(parent), m_validationTimer(new QTimer(this)) {
    // Setup real-time validation timer
    m_validationTimer->setInterval(1000);  // Check every second
    m_validationTimer->setSingleShot(false);
    connect(m_validationTimer, &QTimer::timeout, this,
            &FluentAccessibilityComplianceManager::performRealTimeValidation);

    // Connect to application focus changes
    connect(qApp, &QApplication::focusChanged, this,
            &FluentAccessibilityComplianceManager::handleFocusChanged);

    // Detect system accessibility settings
    detectSystemAccessibilitySettings();

    // Start validation timer if real-time validation is enabled
    if (m_config.enableRealTimeValidation) {
        m_validationTimer->start();
    }
}

void FluentAccessibilityComplianceManager::setConfiguration(
    const FluentAccessibilityConfig& config) {
    QMutexLocker locker(&m_mutex);
    m_config = config;

    // Update validation timer
    if (config.enableRealTimeValidation && !m_validationTimer->isActive()) {
        m_validationTimer->start();
    } else if (!config.enableRealTimeValidation &&
               m_validationTimer->isActive()) {
        m_validationTimer->stop();
    }

    // Apply system preferences
    applySystemPreferences();
}

FluentAccessibilityConfig
FluentAccessibilityComplianceManager::getConfiguration() const {
    QMutexLocker locker(&m_mutex);
    return m_config;
}

void FluentAccessibilityComplianceManager::registerComponent(
    QWidget* component, const FluentComponentAccessibility& metadata) {
    if (!component)
        return;

    QMutexLocker locker(&m_mutex);
    m_componentMetadata[component] = metadata;

    // Apply accessibility attributes immediately
    applyAutomaticFixes(component);

    // Schedule validation
    if (m_config.enableRealTimeValidation &&
        !m_pendingValidation.contains(component)) {
        m_pendingValidation.append(component);
    }
}

double FluentAccessibilityComplianceManager::calculateContrastRatio(
    const QColor& foreground, const QColor& background) {
    return FluentAccessibilityUtils::calculateContrastRatio(foreground,
                                                            background);
}

bool FluentAccessibilityComplianceManager::validateContrastRatio(
    const QColor& foreground, const QColor& background,
    const FluentContrastRequirement& requirement) {
    double ratio = calculateContrastRatio(foreground, background);
    double requiredRatio = requirement.requiresEnhanced
                               ? requirement.enhancedRatio
                               : requirement.minimumRatio;
    return ratio >= requiredRatio;
}

QColor FluentAccessibilityComplianceManager::suggestAccessibleColor(
    const QColor& original, const QColor& background,
    const FluentContrastRequirement& requirement) {
    double targetRatio = requirement.requiresEnhanced
                             ? requirement.enhancedRatio
                             : requirement.minimumRatio;

    // Try adjusting lightness first
    QColor adjusted =
        adjustColorForContrast(original, background, targetRatio, true);
    if (calculateContrastRatio(adjusted, background) >= targetRatio) {
        return adjusted;
    }

    // Try darkening
    adjusted = adjustColorForContrast(original, background, targetRatio, false);
    if (calculateContrastRatio(adjusted, background) >= targetRatio) {
        return adjusted;
    }

    // Fallback to high contrast colors
    double backgroundLuminance = getLuminance(background);
    return backgroundLuminance > 0.5 ? QColor(0, 0, 0) : QColor(255, 255, 255);
}

void FluentAccessibilityComplianceManager::applyAutomaticFixes(
    QWidget* component) {
    if (!component || !m_config.enableAutomaticFixes)
        return;

    FLUENT_PROFILE("FluentAccessibilityComplianceManager::applyAutomaticFixes");

    // Fix color contrast
    fixColorContrast(component);

    // Fix keyboard navigation
    if (m_config.enableKeyboardNavigation) {
        fixKeyboardNavigation(component);
    }

    // Fix focus indicators
    fixFocusIndicators(component);

    // Fix touch targets
    fixTouchTargets(component);

    // Apply ARIA attributes
    QMutexLocker locker(&m_mutex);
    if (m_componentMetadata.contains(component)) {
        const auto& metadata = m_componentMetadata[component];

        if (!metadata.role.isEmpty()) {
            setAriaRole(component, metadata.role);
        }

        if (!metadata.label.isEmpty()) {
            setAriaLabel(component, metadata.label);
        }

        if (!metadata.description.isEmpty()) {
            setAriaDescription(component, metadata.description);
        }

        // Apply states and properties
        for (const QString& state : metadata.states) {
            QStringList parts = state.split('=');
            if (parts.size() == 2) {
                setAriaState(component, parts[0], parts[1]);
            }
        }

        for (const QString& property : metadata.properties) {
            QStringList parts = property.split('=');
            if (parts.size() == 2) {
                setAriaProperty(component, parts[0], parts[1]);
            }
        }
    }
}

void FluentAccessibilityComplianceManager::fixColorContrast(
    QWidget* component) {
    if (!component)
        return;

    // Get component colors
    QPalette palette = component->palette();
    QColor background = palette.color(QPalette::Window);
    QColor text = palette.color(QPalette::WindowText);

    // Check if contrast is sufficient
    FluentContrastRequirement requirement;
    requirement.minimumRatio = 4.5;   // WCAG AA
    requirement.enhancedRatio = 7.0;  // WCAG AAA
    requirement.requiresEnhanced =
        (m_config.targetLevel == FluentWCAGLevel::AAA);

    if (!validateContrastRatio(text, background, requirement)) {
        // Suggest accessible color
        QColor accessibleText =
            suggestAccessibleColor(text, background, requirement);

        // Apply the fix
        palette.setColor(QPalette::WindowText, accessibleText);
        component->setPalette(palette);

        emit accessibilityFixed(component, "Color contrast improved");
    }
}

void FluentAccessibilityComplianceManager::fixKeyboardNavigation(
    QWidget* component) {
    if (!component)
        return;

    // Ensure component is focusable if it should be
    if (FluentAccessibilityUtils::isKeyboardFocusable(component)) {
        component->setFocusPolicy(Qt::TabFocus);
    }

    // Set tab order for child widgets
    QList<QWidget*> focusableChildren =
        FluentAccessibilityUtils::findFocusableChildren(component);
    if (focusableChildren.size() > 1) {
        for (int i = 0; i < focusableChildren.size() - 1; ++i) {
            QWidget::setTabOrder(focusableChildren[i],
                                 focusableChildren[i + 1]);
        }
    }
}

void FluentAccessibilityComplianceManager::fixFocusIndicators(
    QWidget* component) {
    if (!component)
        return;

    // Apply focus indicator styling
    QString focusStyle = QString(
                             "QWidget:focus { "
                             "outline: %1px solid %2; "
                             "outline-offset: 2px; "
                             "}")
                             .arg(m_config.focusIndicatorWidth)
                             .arg(m_config.focusIndicatorColor.name());

    QString existingStyle = component->styleSheet();
    if (!existingStyle.contains("QWidget:focus")) {
        component->setStyleSheet(existingStyle + focusStyle);
    }
}

void FluentAccessibilityComplianceManager::fixTouchTargets(QWidget* component) {
    if (!component)
        return;

    QSize currentSize = component->size();
    QSize minimumSize =
        QSize(m_config.minimumTouchTargetSize, m_config.minimumTouchTargetSize);

    if (currentSize.width() < minimumSize.width() ||
        currentSize.height() < minimumSize.height()) {
        // Expand the component to meet minimum touch target size
        QSize newSize = currentSize.expandedTo(minimumSize);
        component->setMinimumSize(newSize);

        emit accessibilityFixed(component, "Touch target size increased");
    }
}

void FluentAccessibilityComplianceManager::setAriaRole(QWidget* component,
                                                       const QString& role) {
    if (!component)
        return;

    component->setProperty("aria-role", role);

    // Update accessible interface if available
    if (QAccessibleInterface* iface =
            QAccessible::queryAccessibleInterface(component)) {
        // Set role through accessible interface
        // This would require custom accessible interface implementation
        delete iface;
    }
}

void FluentAccessibilityComplianceManager::setAriaLabel(QWidget* component,
                                                        const QString& label) {
    if (!component)
        return;

    component->setAccessibleName(label);
    component->setProperty("aria-label", label);
}

void FluentAccessibilityComplianceManager::setAriaDescription(
    QWidget* component, const QString& description) {
    if (!component)
        return;

    component->setAccessibleDescription(description);
    component->setProperty("aria-description", description);
}

void FluentAccessibilityComplianceManager::enableHighContrastMode(
    bool enabled) {
    if (m_config.enableHighContrastMode != enabled) {
        m_config.enableHighContrastMode = enabled;

        if (enabled) {
            applyHighContrastTheme();
        }

        emit highContrastModeChanged(enabled);
    }
}

void FluentAccessibilityComplianceManager::applyHighContrastTheme() {
    // Apply high contrast theme through the theme manager
    auto& themeManager = Styling::FluentAdvancedThemeManager::instance();

    if (m_config.enableHighContrastMode) {
        // Create high contrast color tokens
        Styling::FluentDesignToken backgroundToken;
        backgroundToken.name = "color.background.highContrast";
        backgroundToken.value = QColor(0, 0, 0);
        backgroundToken.type = Styling::FluentTokenType::Color;
        themeManager.registerToken(backgroundToken);

        Styling::FluentDesignToken textToken;
        textToken.name = "color.text.highContrast";
        textToken.value = QColor(255, 255, 255);
        textToken.type = Styling::FluentTokenType::Color;
        themeManager.registerToken(textToken);

        // Apply to all registered components
        QMutexLocker locker(&m_mutex);
        for (auto it = m_componentMetadata.begin();
             it != m_componentMetadata.end(); ++it) {
            QWidget* component = it.key();
            if (component) {
                QPalette palette = component->palette();
                palette.setColor(QPalette::Window, QColor(0, 0, 0));
                palette.setColor(QPalette::WindowText, QColor(255, 255, 255));
                component->setPalette(palette);
            }
        }
    }
}

void FluentAccessibilityComplianceManager::enableReducedMotion(bool enabled) {
    if (m_config.enableReducedMotion != enabled) {
        m_config.enableReducedMotion = enabled;
        emit reducedMotionChanged(enabled);
    }
}

void FluentAccessibilityComplianceManager::performRealTimeValidation() {
    if (!m_config.enableRealTimeValidation)
        return;

    QMutexLocker locker(&m_mutex);

    // Process pending validations
    for (QWidget* component : m_pendingValidation) {
        if (component) {
            auto findings = auditComponent(component);
            m_componentFindings[component] = findings;

            // Emit violations
            for (const auto& finding : findings) {
                if (finding.result == FluentAccessibilityResult::Fail) {
                    emit accessibilityViolationFound(finding);
                }
            }
        }
    }

    m_pendingValidation.clear();
}

void FluentAccessibilityComplianceManager::detectSystemAccessibilitySettings() {
    // Detect system accessibility preferences
    QSettings settings(
        "HKEY_CURRENT_"
        "USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personaliz"
        "e",
        QSettings::NativeFormat);

    // High contrast mode
    m_systemHighContrast =
        settings.value("AppsUseHighContrast", false).toBool();

    // Reduced motion (Windows 10+)
    QSettings animationSettings(
        "HKEY_CURRENT_USER\\Control Panel\\Desktop\\WindowMetrics",
        QSettings::NativeFormat);
    m_systemReducedMotion =
        animationSettings.value("MinAnimate", "1").toString() == "0";

    // Screen reader detection (basic)
    m_systemScreenReader = !qEnvironmentVariable("NVDA_PID").isEmpty() ||
                           !qEnvironmentVariable("JAWS_PID").isEmpty();
}

void FluentAccessibilityComplianceManager::applySystemPreferences() {
    if (m_systemHighContrast) {
        enableHighContrastMode(true);
    }

    if (m_systemReducedMotion) {
        enableReducedMotion(true);
    }
}

double FluentAccessibilityComplianceManager::getLuminance(const QColor& color) {
    return FluentAccessibilityUtils::calculateLuminance(color);
}

QColor FluentAccessibilityComplianceManager::adjustColorForContrast(
    const QColor& color, const QColor& background, double targetRatio,
    bool lighten) {
    QColor adjusted = color;
    double currentRatio = calculateContrastRatio(color, background);

    if (currentRatio >= targetRatio) {
        return color;
    }

    // Adjust lightness iteratively
    int step = lighten ? 10 : -10;
    int lightness = adjusted.lightness();

    while ((lighten ? lightness < 255 : lightness > 0) &&
           calculateContrastRatio(adjusted, background) < targetRatio) {
        lightness += step;
        lightness = qBound(0, lightness, 255);
        adjusted.setHsl(adjusted.hue(), adjusted.saturation(), lightness);
    }

    return adjusted;
}

// FluentAccessibilityUtils implementation
double FluentAccessibilityUtils::calculateLuminance(const QColor& color) {
    // Convert to linear RGB
    auto toLinear = [](double c) {
        c = c / 255.0;
        return c <= 0.03928 ? c / 12.92 : std::pow((c + 0.055) / 1.055, 2.4);
    };

    double r = toLinear(color.red());
    double g = toLinear(color.green());
    double b = toLinear(color.blue());

    // Calculate relative luminance
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

double FluentAccessibilityUtils::calculateContrastRatio(const QColor& color1,
                                                        const QColor& color2) {
    double lum1 = calculateLuminance(color1);
    double lum2 = calculateLuminance(color2);

    double lighter = qMax(lum1, lum2);
    double darker = qMin(lum1, lum2);

    return (lighter + 0.05) / (darker + 0.05);
}

bool FluentAccessibilityUtils::meetsWCAGContrast(const QColor& foreground,
                                                 const QColor& background,
                                                 FluentWCAGLevel level,
                                                 bool isLargeText) {
    double ratio = calculateContrastRatio(foreground, background);

    switch (level) {
        case FluentWCAGLevel::A:
            return true;  // Level A has no contrast requirements
        case FluentWCAGLevel::AA:
            return ratio >= (isLargeText ? 3.0 : 4.5);
        case FluentWCAGLevel::AAA:
            return ratio >= (isLargeText ? 4.5 : 7.0);
    }

    return false;
}

void initializeFluentAccessibility() {
    // Initialize the accessibility compliance manager
    auto& manager = FluentAccessibilityComplianceManager::instance();

    // Set default configuration
    FluentAccessibilityConfig config;
    config.targetLevel = FluentWCAGLevel::AA;
    config.enableAutomaticFixes = true;
    config.enableRealTimeValidation = true;
    manager.setConfiguration(config);

    qDebug() << "FluentUI Accessibility system initialized";
}

}  // namespace FluentQt::Accessibility
