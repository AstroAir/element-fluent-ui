// include/FluentQt/Accessibility/FluentAccessibilityCompliance.h
#pragma once

#include <QColor>
#include <QFont>
#include <QHash>
#include <QList>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QWidget>
#include <memory>

namespace FluentQt::Accessibility {

// WCAG 2.1 compliance levels
enum class FluentWCAGLevel {
    A,   // Level A (minimum)
    AA,  // Level AA (standard)
    AAA  // Level AAA (enhanced)
};

// Accessibility audit result
enum class FluentAccessibilityResult { Pass, Fail, Warning, NotApplicable };

// Color contrast requirements
struct FluentContrastRequirement {
    double minimumRatio{4.5};   // WCAG AA standard
    double enhancedRatio{7.0};  // WCAG AAA standard
    bool requiresEnhanced{false};
    QString context;  // "normal text", "large text", "UI component"
};

// Accessibility audit finding
struct FluentAccessibilityFinding {
    QString id;
    QString component;
    QString rule;
    FluentAccessibilityResult result;
    QString description;
    QString recommendation;
    FluentWCAGLevel level;
    int severity{1};  // 1-5 scale
    QStringList affectedElements;
};

// Accessibility configuration
struct FluentAccessibilityConfig {
    FluentWCAGLevel targetLevel{FluentWCAGLevel::AA};
    bool enableAutomaticFixes{true};
    bool enableRealTimeValidation{true};
    bool enableKeyboardNavigation{true};
    bool enableScreenReaderSupport{true};
    bool enableHighContrastMode{false};
    bool enableReducedMotion{false};
    double minimumTouchTargetSize{44.0};      // 44x44 pixels minimum
    int focusIndicatorWidth{2};               // Focus ring width in pixels
    QColor focusIndicatorColor{0, 120, 215};  // Default focus color
};

// Component accessibility metadata
struct FluentComponentAccessibility {
    QString role;              // ARIA role
    QString label;             // Accessible name
    QString description;       // Accessible description
    QStringList states;        // ARIA states (expanded, checked, etc.)
    QStringList properties;    // ARIA properties (readonly, required, etc.)
    QString keyboardShortcut;  // Keyboard shortcut
    bool isFocusable{true};
    bool isLandmark{false};
    int tabIndex{0};
    QString liveRegion;  // ARIA live region type
};

// Accessibility compliance manager
class FluentAccessibilityComplianceManager : public QObject {
    // Q_OBJECT  // Temporarily commented out to fix linking issues

public:
    static FluentAccessibilityComplianceManager& instance();

    // Configuration
    void setConfiguration(const FluentAccessibilityConfig& config);
    FluentAccessibilityConfig getConfiguration() const;

    // Component registration and validation
    void registerComponent(QWidget* component,
                           const FluentComponentAccessibility& metadata);
    void unregisterComponent(QWidget* component);
    void updateComponentMetadata(QWidget* component,
                                 const FluentComponentAccessibility& metadata);

    // Accessibility auditing
    QList<FluentAccessibilityFinding> auditComponent(QWidget* component);
    QList<FluentAccessibilityFinding> auditApplication();
    QList<FluentAccessibilityFinding> auditColorContrast();
    QList<FluentAccessibilityFinding> auditKeyboardNavigation();
    QList<FluentAccessibilityFinding> auditScreenReaderSupport();

    // Color contrast validation
    double calculateContrastRatio(const QColor& foreground,
                                  const QColor& background);
    bool validateContrastRatio(const QColor& foreground,
                               const QColor& background,
                               const FluentContrastRequirement& requirement);
    QColor suggestAccessibleColor(const QColor& original,
                                  const QColor& background,
                                  const FluentContrastRequirement& requirement);

    // Automatic accessibility fixes
    void applyAutomaticFixes(QWidget* component);
    void fixColorContrast(QWidget* component);
    void fixKeyboardNavigation(QWidget* component);
    void fixFocusIndicators(QWidget* component);
    void fixTouchTargets(QWidget* component);

    // ARIA support
    void setAriaRole(QWidget* component, const QString& role);
    void setAriaLabel(QWidget* component, const QString& label);
    void setAriaDescription(QWidget* component, const QString& description);
    void setAriaState(QWidget* component, const QString& state,
                      const QString& value);
    void setAriaProperty(QWidget* component, const QString& property,
                         const QString& value);

    // Keyboard navigation
    void enableKeyboardNavigation(QWidget* component);
    void setTabOrder(const QList<QWidget*>& widgets);
    void setKeyboardShortcut(QWidget* component, const QString& shortcut);

    // Screen reader support
    void announceToScreenReader(const QString& message,
                                const QString& priority = "polite");
    void setLiveRegion(QWidget* component, const QString& type);

    // High contrast mode
    void enableHighContrastMode(bool enabled = true);
    bool isHighContrastModeEnabled() const;
    void applyHighContrastTheme();

    // Reduced motion
    void enableReducedMotion(bool enabled = true);
    bool isReducedMotionEnabled() const;

    // Focus management
    void setFocusIndicatorStyle(const QColor& color, int width);
    void ensureVisibleFocus(QWidget* component);
    QWidget* findNextFocusableWidget(QWidget* current, bool forward = true);

    // Touch accessibility
    void ensureMinimumTouchTarget(QWidget* component);
    QSize getMinimumTouchTargetSize() const;

    // Validation and reporting
    bool validateComponent(QWidget* component,
                           FluentWCAGLevel level = FluentWCAGLevel::AA);
    QString generateAccessibilityReport();
    void exportAuditResults(const QString& filePath);

    // signals:  // Temporarily commented out to fix linking issues
    //     void accessibilityViolationFound(const FluentAccessibilityFinding&
    //     finding); void accessibilityFixed(QWidget* component, const QString&
    //     fix); void highContrastModeChanged(bool enabled); void
    //     reducedMotionChanged(bool enabled); void focusChanged(QWidget*
    //     oldFocus, QWidget* newFocus);

    // private slots:  // Temporarily commented out to fix linking issues
    //     void performRealTimeValidation();
    //     void updateSystemAccessibilitySettings();
    // void handleFocusChanged(QWidget* old, QWidget* now);  // Temporarily
    // commented out void detectSystemAccessibilitySettingsAsync();  //
    // Temporarily commented out

private:
    FluentAccessibilityComplianceManager(QObject* parent = nullptr);
    ~FluentAccessibilityComplianceManager() = default;

    // Internal validation methods
    FluentAccessibilityFinding validateColorContrast(QWidget* component);
    FluentAccessibilityFinding validateKeyboardAccess(QWidget* component);
    FluentAccessibilityFinding validateAriaAttributes(QWidget* component);
    FluentAccessibilityFinding validateFocusManagement(QWidget* component);
    FluentAccessibilityFinding validateTouchTargets(QWidget* component);
    FluentAccessibilityFinding validateSemanticStructure(QWidget* component);

    // Color utilities
    double getLuminance(const QColor& color);
    QColor adjustColorForContrast(const QColor& color, const QColor& background,
                                  double targetRatio, bool lighten);

    // System integration
    void detectSystemAccessibilitySettings();
    void applySystemPreferences();

    // Component tracking
    void trackComponentChanges(QWidget* component);
    void validateComponentHierarchy(QWidget* root);

private:
    mutable QMutex m_mutex;
    FluentAccessibilityConfig m_config;

    // Component registry
    QHash<QWidget*, FluentComponentAccessibility> m_componentMetadata;
    QHash<QWidget*, QList<FluentAccessibilityFinding>> m_componentFindings;

    // Validation state
    QTimer* m_validationTimer;
    QList<QWidget*> m_pendingValidation;

    // System state
    bool m_systemHighContrast{false};
    bool m_systemReducedMotion{false};
    bool m_systemScreenReader{false};

    // Focus tracking
    QWidget* m_lastFocusedWidget{nullptr};
    QList<QWidget*> m_focusHistory;

    // Audit results
    QList<FluentAccessibilityFinding> m_auditResults;
    QString m_lastAuditReport;
};

// Forward declaration - FluentAccessibilityUtils is defined in
// FluentAccessibilityManager.h
class FluentAccessibilityUtils;

// Initialize accessibility system
void initializeFluentAccessibility();

// Convenience macros for accessibility
#define FLUENT_SET_ACCESSIBLE_NAME(widget, name) \
    FluentAccessibilityComplianceManager::instance().setAriaLabel(widget, name)

#define FLUENT_SET_ACCESSIBLE_DESCRIPTION(widget, desc)                  \
    FluentAccessibilityComplianceManager::instance().setAriaDescription( \
        widget, desc)

#define FLUENT_SET_ACCESSIBLE_ROLE(widget, role) \
    FluentAccessibilityComplianceManager::instance().setAriaRole(widget, role)

#define FLUENT_VALIDATE_ACCESSIBILITY(widget) \
    FluentAccessibilityComplianceManager::instance().validateComponent(widget)

#define FLUENT_ANNOUNCE_TO_SCREEN_READER(message)                            \
    FluentAccessibilityComplianceManager::instance().announceToScreenReader( \
        message)

}  // namespace FluentQt::Accessibility
