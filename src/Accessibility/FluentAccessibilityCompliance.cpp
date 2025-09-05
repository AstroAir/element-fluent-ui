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
    // Temporarily commented out slot connections
    // connect(m_validationTimer, &QTimer::timeout, this,
    //         &FluentAccessibilityComplianceManager::performRealTimeValidation);

    // Connect to application focus changes
    // connect(qApp, &QApplication::focusChanged, this,
    //         &FluentAccessibilityComplianceManager::handleFocusChanged);

    // Defer system accessibility detection to avoid blocking UI thread
    // Check for skip environment variables
    if (qEnvironmentVariableIsSet("FLUENTQT_SKIP_PROCESS_DETECTION") ||
        qEnvironmentVariableIsSet("FLUENTQT_SKIP_ACCESSIBILITY_DETECTION")) {
        qDebug() << "Skipping accessibility compliance system detection (env "
                    "override)";
    } else {
        // Temporarily commented out async detection
        // QTimer::singleShot(100, this,
        //                    &FluentAccessibilityComplianceManager::
        //                        detectSystemAccessibilitySettingsAsync);
    }

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
    // Simplified contrast calculation - replace with proper implementation
    double l1 = foreground.lightness() / 255.0;
    double l2 = background.lightness() / 255.0;
    return (qMax(l1, l2) + 0.05) / (qMin(l1, l2) + 0.05);
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

        // emit accessibilityFixed(component, "Color contrast improved");  //
        // Temporarily commented out
    }
}

void FluentAccessibilityComplianceManager::fixKeyboardNavigation(
    QWidget* component) {
    if (!component)
        return;

    // Ensure component is focusable if it should be
    if (component->focusPolicy() == Qt::NoFocus) {
        component->setFocusPolicy(Qt::TabFocus);
    }

    // Set tab order for child widgets
    QList<QWidget*> focusableChildren = component->findChildren<QWidget*>();
    QList<QWidget*> actuallyFocusable;

    for (QWidget* child : focusableChildren) {
        if (child->focusPolicy() != Qt::NoFocus) {
            actuallyFocusable.append(child);
        }
    }
    if (actuallyFocusable.size() > 1) {
        for (int i = 0; i < actuallyFocusable.size() - 1; ++i) {
            QWidget::setTabOrder(actuallyFocusable[i],
                                 actuallyFocusable[i + 1]);
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

        // emit accessibilityFixed(component, "Touch target size increased"); //
        // Temporarily commented out
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
        // Don't delete QAccessibleInterface directly - Qt manages it
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

        // emit highContrastModeChanged(enabled);  // Temporarily commented out
    }
}

void FluentAccessibilityComplianceManager::applyHighContrastTheme() {
    // Simplified high contrast theme application
    // TODO: Implement proper theme management integration when
    // FluentAdvancedThemeManager is available

    if (m_config.enableHighContrastMode) {
        // For now, just apply basic high contrast styles
        // The actual theme integration will be implemented later

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
        // emit reducedMotionChanged(enabled);  // Temporarily commented out
    }
}

// Temporarily commented out slot implementation
// void FluentAccessibilityComplianceManager::performRealTimeValidation() {
//     if (!m_config.enableRealTimeValidation)
//         return;

//     QMutexLocker locker(&m_mutex);

//     // Process pending validations
//     for (QWidget* component : m_pendingValidation) {
//         if (component) {
//             auto findings = auditComponent(component);
//             m_componentFindings[component] = findings;

//             // Emit violations
//             for (const auto& finding : findings) {
//                 if (finding.result == FluentAccessibilityResult::Fail) {
//                     // emit accessibilityViolationFound(finding);  //
//                     // Temporarily commented out
//                 }
//             }
//         }
//     }

//     m_pendingValidation.clear();
// }

void FluentAccessibilityComplianceManager::detectSystemAccessibilitySettings() {
    // Check for skip environment variables
    if (qEnvironmentVariableIsSet("FLUENTQT_SKIP_PROCESS_DETECTION") ||
        qEnvironmentVariableIsSet("FLUENTQT_SKIP_ACCESSIBILITY_DETECTION")) {
        qDebug() << "Skipping system accessibility settings detection (env "
                    "override)";
        return;
    }

    try {
        // Detect system accessibility preferences with timeout protection
        qDebug() << "Detecting system accessibility settings";

        // High contrast mode detection with error handling
        try {
            QSettings settings(
                "HKEY_CURRENT_"
                "USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Pe"
                "rsonaliz"
                "e",
                QSettings::NativeFormat);
            m_systemHighContrast =
                settings.value("AppsUseHighContrast", false).toBool();
        } catch (...) {
            qWarning()
                << "Failed to read high contrast settings, using default";
            m_systemHighContrast = false;
        }

        // Reduced motion detection with error handling
        try {
            QSettings animationSettings(
                "HKEY_CURRENT_USER\\Control Panel\\Desktop\\WindowMetrics",
                QSettings::NativeFormat);
            m_systemReducedMotion =
                animationSettings.value("MinAnimate", "1").toString() == "0";
        } catch (...) {
            qWarning() << "Failed to read animation settings, using default";
            m_systemReducedMotion = false;
        }

        // Screen reader detection (basic, safe)
        m_systemScreenReader = !qEnvironmentVariable("NVDA_PID").isEmpty() ||
                               !qEnvironmentVariable("JAWS_PID").isEmpty();

        qDebug() << "System accessibility detection completed - High contrast:"
                 << m_systemHighContrast
                 << "Reduced motion:" << m_systemReducedMotion
                 << "Screen reader:" << m_systemScreenReader;
    } catch (const std::exception& e) {
        qWarning() << "Error during system accessibility detection:"
                   << e.what();
        // Set safe defaults
        m_systemHighContrast = false;
        m_systemReducedMotion = false;
        m_systemScreenReader = false;
    } catch (...) {
        qWarning() << "Unknown error during system accessibility detection";
        // Set safe defaults
        m_systemHighContrast = false;
        m_systemReducedMotion = false;
        m_systemScreenReader = false;
    }
}

// Temporarily commented out slot implementation
// void FluentAccessibilityComplianceManager::
//     detectSystemAccessibilitySettingsAsync() {
//     qDebug() << "Starting asynchronous accessibility compliance detection";
//     detectSystemAccessibilitySettings();
//     qDebug() << "Asynchronous accessibility compliance detection completed";
// }

void FluentAccessibilityComplianceManager::applySystemPreferences() {
    if (m_systemHighContrast) {
        enableHighContrastMode(true);
    }

    if (m_systemReducedMotion) {
        enableReducedMotion(true);
    }
}

double FluentAccessibilityComplianceManager::getLuminance(const QColor& color) {
    // Simplified luminance calculation
    double r = color.redF();
    double g = color.greenF();
    double b = color.blueF();

    // Apply gamma correction
    r = (r <= 0.03928) ? r / 12.92 : qPow((r + 0.055) / 1.055, 2.4);
    g = (g <= 0.03928) ? g / 12.92 : qPow((g + 0.055) / 1.055, 2.4);
    b = (b <= 0.03928) ? b / 12.92 : qPow((b + 0.055) / 1.055, 2.4);

    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
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

// FluentAccessibilityUtils implementations removed - they should be in
// FluentAccessibilityManager.cpp

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

// Stub implementations for missing methods
void FluentAccessibilityComplianceManager::setAriaState(QWidget* component,
                                                        const QString& state,
                                                        const QString& value) {
    if (!component)
        return;

    // Simplified implementation - set as dynamic property
    component->setProperty(("aria-" + state).toUtf8().constData(), value);
}

void FluentAccessibilityComplianceManager::setAriaProperty(
    QWidget* component, const QString& property, const QString& value) {
    if (!component)
        return;

    // Simplified implementation - set as dynamic property
    component->setProperty(("aria-" + property).toUtf8().constData(), value);
}

}  // namespace FluentQt::Accessibility
