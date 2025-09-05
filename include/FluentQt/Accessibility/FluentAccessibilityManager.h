// include/FluentQt/Accessibility/FluentAccessibilityManager.h
#pragma once

#include <QAccessible>
#include <QAccessibleWidget>
#include <QColor>
#include <QKeySequence>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QWidget>
#include <chrono>
#include <memory>
#include <unordered_map>

namespace FluentQt::Accessibility {

// WCAG 2.1 compliance levels
enum class WcagLevel {
    A,   // Level A compliance
    AA,  // Level AA compliance (recommended)
    AAA  // Level AAA compliance (highest)
};

// Live region types for dynamic content
enum class LiveRegionType {
    Off,       // No announcements
    Polite,    // Announce when user is idle
    Assertive  // Announce immediately
};

// Accessibility roles extending Qt's basic roles
enum class FluentAccessibleRole {
    // Standard roles
    Button = QAccessible::Button,
    CheckBox = QAccessible::CheckBox,
    RadioButton = QAccessible::RadioButton,
    ComboBox = QAccessible::ComboBox,
    EditableText = QAccessible::EditableText,
    StaticText = QAccessible::StaticText,

    // Extended roles for modern UI
    Card = 0x1000,
    Toast,
    Notification,
    Timeline,
    Carousel,
    NavigationView,
    TabPanel,
    Accordion,
    Badge,
    Avatar,
    Rating,
    ColorPicker,
    DatePicker,
    TimePicker
};

// Enhanced accessibility properties
struct AccessibilityProperties {
    QString name;
    QString description;
    QString help;
    QString value;
    QString placeholder;
    QKeySequence shortcut;
    FluentAccessibleRole role{FluentAccessibleRole::Button};
    LiveRegionType liveRegion{LiveRegionType::Off};
    QString ariaLabel;
    QString ariaDescribedBy;
    QString ariaLabelledBy;
    int tabIndex{0};
    bool focusable{true};
    bool visible{true};
    bool required{false};
    bool invalid{false};
    QStringList controls;     // IDs of controlled elements
    QStringList describedBy;  // IDs of describing elements
    double contrastRatio{0.0};
};

// Enhanced accessibility issue types for comprehensive WCAG 2.1 validation
enum class AccessibilityIssueType {
    // Perceivable issues (WCAG 2.1 Principle 1)
    MissingLabel,
    LowContrast,
    MissingAltText,
    NoTextAlternatives,
    InsufficientColorContrast,
    ColorOnlyInformation,
    AudioWithoutCaptions,
    VideoWithoutDescriptions,
    FlashingContent,
    AutoPlayingMedia,
    NonTextContentMissing,

    // Operable issues (WCAG 2.1 Principle 2)
    NoKeyboardAccess,
    KeyboardTrap,
    InvalidTabOrder,
    NoFocusIndicator,
    TimingTooShort,
    NoSkipLinks,
    UnpredictableNavigation,
    ContextChangeOnFocus,
    MotionActivation,
    TargetSizeTooSmall,

    // Understandable issues (WCAG 2.1 Principle 3)
    MissingHeading,
    EmptyLink,
    FormValidationError,
    NoErrorIdentification,
    NoErrorSuggestion,
    InconsistentNavigation,
    InconsistentIdentification,
    UnexpectedContextChange,
    LabelInNameMismatch,

    // Robust issues (WCAG 2.1 Principle 4)
    InvalidMarkup,
    IncompatibleAssistiveTech,
    MissingLandmarks,
    ImproperHeadingStructure,
    MissingRequiredAttributes,
    StatusMessagesMissing,

    // Additional WCAG 2.1 AA specific issues
    OrientationLocked,
    ConcurrentInputMechanisms,
    CharacterKeyShortcuts,
    PointerCancellation,
    PointerGestures
};

// Enhanced accessibility issue for compliance checking
struct AccessibilityIssue {
    QString widgetName;
    AccessibilityIssueType issueType;
    QString description;
    QString suggestion;
    WcagLevel requiredLevel;
    QString wcagCriterion;
    QString wcagTechnique;
    int severity{1};  // 1-5, 5 being most severe
    bool isBlocking{false};
    bool isAutomaticallyFixable{false};
    QString fixAction;
    QStringList affectedUsers;  // Types of users affected
    double impactScore{0.0};    // 0-100 impact score
};

// Enhanced accessibility interface
class FluentEnhancedAccessible : public QAccessibleWidget {
public:
    explicit FluentEnhancedAccessible(QWidget* widget);
    virtual ~FluentEnhancedAccessible() = default;

    // Basic accessibility interface
    QString text(QAccessible::Text t) const override;
    void setText(QAccessible::Text t, const QString& text) override;
    QAccessible::Role role() const override;
    QAccessible::State state() const override;

    // Enhanced accessibility features
    void setAccessibilityProperties(const AccessibilityProperties& properties);
    AccessibilityProperties getAccessibilityProperties() const;

    void setAccessibleName(const QString& name);
    void setAccessibleDescription(const QString& description);
    void setAccessibleRole(FluentAccessibleRole role);

    // ARIA support
    void setAriaLabel(const QString& label);
    void setAriaDescribedBy(const QString& elementId);
    void setAriaLabelledBy(const QString& elementId);
    void setAriaControls(const QStringList& controlIds);

    // Keyboard navigation support
    void setKeyboardShortcut(const QKeySequence& shortcut);
    QKeySequence keyboardShortcut() const;
    void setTabIndex(int index);
    int tabIndex() const;

    // Screen reader support
    void announceToScreenReader(
        const QString& message,
        LiveRegionType urgency = LiveRegionType::Polite);
    void setLiveRegion(LiveRegionType type);
    LiveRegionType liveRegion() const;

    // Focus management
    void setFocusable(bool focusable);
    bool isFocusable() const;
    void requestFocus();

    // Validation and error handling
    void setValidationState(bool isValid, const QString& errorMessage = "");
    bool isValid() const;
    QString validationError() const;

    // Contrast checking
    void setContrastRatio(double ratio);
    double contrastRatio() const;
    bool meetsContrastRequirements(WcagLevel level) const;

protected:
    virtual void updateAccessibilityInfo();
    virtual QString generateAccessibleDescription() const;
    virtual QAccessible::State calculateState() const;

private:
    AccessibilityProperties m_properties;
    mutable QMutex m_propertiesMutex;
    bool m_isValid{true};
    QString m_validationError;
};

// Accessibility manager for global settings and coordination
class FluentAccessibilityManager : public QObject {
    Q_OBJECT

public:
    static FluentAccessibilityManager& instance();

    // Global accessibility settings
    void setWcagComplianceLevel(WcagLevel level);
    WcagLevel wcagComplianceLevel() const { return m_wcagLevel; }

    void setHighContrastMode(bool enabled);
    bool isHighContrastMode() const { return m_highContrastMode; }

    void setReducedMotionMode(bool enabled);
    bool isReducedMotionMode() const { return m_reducedMotionMode; }

    void setScreenReaderMode(bool enabled);
    bool isScreenReaderMode() const { return m_screenReaderMode; }

    // Keyboard navigation
    void setKeyboardNavigationEnabled(bool enabled);
    bool isKeyboardNavigationEnabled() const { return m_keyboardNavigation; }

    void setFocusIndicatorVisible(bool visible);
    bool isFocusIndicatorVisible() const { return m_focusIndicatorVisible; }

    // Color and contrast
    void setMinimumContrastRatio(double ratio);
    double minimumContrastRatio() const { return m_minimumContrastRatio; }

    double calculateContrastRatio(const QColor& foreground,
                                  const QColor& background) const;
    QColor ensureContrast(const QColor& foreground, const QColor& background,
                          double minRatio) const;

    // Announcements
    void announceGlobally(const QString& message,
                          LiveRegionType urgency = LiveRegionType::Polite);
    void setAnnouncementDelay(int milliseconds) {
        m_announcementDelay = milliseconds;
    }

    // Enhanced accessibility testing and validation
    QList<AccessibilityIssue> validateAccessibility(QWidget* rootWidget);
    QList<AccessibilityIssue> validateWcag21Compliance(QWidget* rootWidget);
    void generateAccessibilityReport(QWidget* rootWidget,
                                     const QString& filename);
    void generateWcag21Report(QWidget* rootWidget, const QString& filename);
    bool checkWcagCompliance(QWidget* widget, WcagLevel level);

    // Specific WCAG 2.1 validation methods
    QList<AccessibilityIssue> validatePerceivable(QWidget* widget);
    QList<AccessibilityIssue> validateOperable(QWidget* widget);
    QList<AccessibilityIssue> validateUnderstandable(QWidget* widget);
    QList<AccessibilityIssue> validateRobust(QWidget* widget);

    // Automated accessibility fixes
    void autoFixAccessibilityIssues(QWidget* rootWidget,
                                    const QList<AccessibilityIssue>& issues);
    bool canAutoFixIssue(const AccessibilityIssue& issue);
    void applyAccessibilityFix(QWidget* widget,
                               const AccessibilityIssue& issue);

    // Advanced contrast and color checking
    double calculateContrastRatio(const QColor& foreground,
                                  const QColor& background);
    bool meetsContrastRequirements(const QColor& fg, const QColor& bg,
                                   WcagLevel level, bool isLargeText = false);
    QColor suggestBetterContrast(const QColor& foreground,
                                 const QColor& background, WcagLevel level);
    QList<AccessibilityIssue> validateColorAccessibility(QWidget* widget);

    // Keyboard navigation validation
    QList<AccessibilityIssue> validateKeyboardNavigation(QWidget* rootWidget);
    bool isKeyboardAccessible(QWidget* widget);
    QList<QWidget*> findKeyboardTraps(QWidget* rootWidget);
    void validateTabOrder(QWidget* rootWidget,
                          QList<AccessibilityIssue>& issues);

    // Screen reader compatibility
    QList<AccessibilityIssue> validateScreenReaderCompatibility(
        QWidget* widget);
    bool hasProperAriaLabels(QWidget* widget);
    bool hasProperHeadingStructure(QWidget* rootWidget);
    void validateLandmarks(QWidget* rootWidget,
                           QList<AccessibilityIssue>& issues);

    // Touch and pointer accessibility (WCAG 2.1 AA)
    QList<AccessibilityIssue> validateTouchAccessibility(QWidget* widget);
    bool meetsMinimumTargetSize(QWidget* widget);
    void validatePointerGestures(QWidget* widget,
                                 QList<AccessibilityIssue>& issues);
    void validateMotionActivation(QWidget* widget,
                                  QList<AccessibilityIssue>& issues);

    // Widget registration
    void registerAccessibleWidget(QWidget* widget,
                                  FluentEnhancedAccessible* accessible);
    void unregisterAccessibleWidget(QWidget* widget);
    FluentEnhancedAccessible* getAccessible(QWidget* widget) const;

    // System integration
    void detectSystemAccessibilitySettings();
    void applySystemSettings();

signals:
    void highContrastModeChanged(bool enabled);
    void reducedMotionModeChanged(bool enabled);
    void screenReaderModeChanged(bool enabled);
    void keyboardNavigationChanged(bool enabled);
    void accessibilityIssueFound(const AccessibilityIssue& issue);
    void wcagComplianceLevelChanged(WcagLevel level);

    // Enhanced accessibility signals
    void accessibilityValidationCompleted(
        const QList<FluentQt::Accessibility::AccessibilityIssue>& issues);
    void wcag21ValidationCompleted(
        const QList<FluentQt::Accessibility::AccessibilityIssue>& issues);
    void accessibilityIssueFixed(
        const FluentQt::Accessibility::AccessibilityIssue& issue);
    void contrastIssueDetected(QWidget* widget, double ratio, double required);
    void keyboardTrapDetected(QWidget* widget);
    void focusManagementIssue(QWidget* widget, const QString& issue);
    void screenReaderIssueDetected(QWidget* widget, const QString& issue);
    void touchAccessibilityIssue(QWidget* widget, const QString& issue);
    void motionActivationIssue(QWidget* widget, const QString& issue);
    void accessibilityReportGenerated(const QString& filename);
    void autoFixApplied(QWidget* widget, const AccessibilityIssue& issue);

private slots:
    void processAnnouncementQueue();
    void onSystemAccessibilityChanged();
    void detectSystemAccessibilitySettingsAsync();

private:
    FluentAccessibilityManager();
    void validateWidget(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkColorContrast(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkKeyboardAccess(QWidget* widget,
                             QList<AccessibilityIssue>& issues);
    void checkFocusManagement(QWidget* widget,
                              QList<AccessibilityIssue>& issues);
    void checkTextAlternatives(QWidget* widget,
                               QList<AccessibilityIssue>& issues);

    // Enhanced WCAG 2.1 validation methods
    void validatePerceivableInternal(QWidget* widget,
                                     QList<AccessibilityIssue>& issues);
    void validateOperableInternal(QWidget* widget,
                                  QList<AccessibilityIssue>& issues);
    void validateUnderstandableInternal(QWidget* widget,
                                        QList<AccessibilityIssue>& issues);
    void validateRobustInternal(QWidget* widget,
                                QList<AccessibilityIssue>& issues);

    // Specific validation helpers
    void checkImageAlternatives(QWidget* widget,
                                QList<AccessibilityIssue>& issues);
    void checkMediaAlternatives(QWidget* widget,
                                QList<AccessibilityIssue>& issues);
    void checkColorDependency(QWidget* widget,
                              QList<AccessibilityIssue>& issues);
    void checkFlashingContent(QWidget* widget,
                              QList<AccessibilityIssue>& issues);
    void checkTimingConstraints(QWidget* widget,
                                QList<AccessibilityIssue>& issues);
    void checkSkipLinks(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkPageTitles(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkFormLabels(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkErrorHandling(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkConsistentNavigation(QWidget* widget,
                                   QList<AccessibilityIssue>& issues);
    void checkStatusMessages(QWidget* widget,
                             QList<AccessibilityIssue>& issues);
    void checkTargetSizes(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkPointerInputs(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkMotionInputs(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkCharacterKeyShortcuts(QWidget* widget,
                                    QList<AccessibilityIssue>& issues);

    // Auto-fix helpers
    void autoFixContrast(QWidget* widget, const AccessibilityIssue& issue);
    void autoFixLabels(QWidget* widget, const AccessibilityIssue& issue);
    void autoFixTabOrder(QWidget* widget, const AccessibilityIssue& issue);
    void autoFixFocusIndicators(QWidget* widget,
                                const AccessibilityIssue& issue);
    void autoFixTargetSizes(QWidget* widget, const AccessibilityIssue& issue);

    // Utility methods
    QString generateIssueDescription(AccessibilityIssueType type,
                                     QWidget* widget);
    QString generateFixSuggestion(AccessibilityIssueType type, QWidget* widget);
    QString getWcagCriterion(AccessibilityIssueType type);
    QString getWcagTechnique(AccessibilityIssueType type);
    int calculateSeverity(AccessibilityIssueType type, WcagLevel level);
    double calculateImpactScore(AccessibilityIssueType type, QWidget* widget);
    QStringList getAffectedUserTypes(AccessibilityIssueType type);

private slots:
    void initializeTimer();

private:
    WcagLevel m_wcagLevel{WcagLevel::AA};
    bool m_highContrastMode{false};
    bool m_reducedMotionMode{false};
    bool m_screenReaderMode{false};
    bool m_keyboardNavigation{true};
    bool m_focusIndicatorVisible{true};
    double m_minimumContrastRatio{4.5};  // WCAG AA standard

    // Announcement system
    struct PendingAnnouncement {
        QString message;
        LiveRegionType urgency;
        std::chrono::steady_clock::time_point timestamp;
    };

    QList<PendingAnnouncement> m_announcementQueue;
    QTimer* m_announcementTimer;
    int m_announcementDelay{100};

    // Widget registry
    std::unordered_map<QWidget*, FluentEnhancedAccessible*> m_accessibleWidgets;
    mutable QMutex m_registryMutex;
};

// Accessibility utilities
class FluentAccessibilityUtils {
public:
    // Color and contrast utilities
    static double calculateLuminance(const QColor& color);
    static double calculateContrastRatio(const QColor& color1,
                                         const QColor& color2);
    static bool meetsWcagContrast(const QColor& foreground,
                                  const QColor& background, WcagLevel level);

    // Keyboard navigation utilities
    static bool isKeyboardNavigable(QWidget* widget);
    static QWidget* findNextFocusWidget(QWidget* current, bool forward = true);
    static void setFocusChain(const QList<QWidget*>& widgets);

    // Screen reader utilities
    static QString generateAccessibleText(QWidget* widget);
    static QString formatForScreenReader(const QString& text);
    static void announceWidgetChange(QWidget* widget, const QString& change);

    // Validation utilities
    static bool hasAccessibleName(QWidget* widget);
    static bool hasAccessibleDescription(QWidget* widget);
    static bool isProperlyLabeled(QWidget* widget);
    static QStringList findAccessibilityIssues(QWidget* widget);
};

}  // namespace FluentQt::Accessibility
