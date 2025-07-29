// include/FluentQt/Accessibility/FluentAccessibilityManager.h
#pragma once

#include <QObject>
#include <QAccessible>
#include <QAccessibleWidget>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QKeySequence>
#include <QTimer>
#include <QMutex>
#include <QColor>
#include <memory>
#include <unordered_map>
#include <chrono>

namespace FluentQt::Accessibility {

// WCAG 2.1 compliance levels
enum class WcagLevel {
    A,      // Level A compliance
    AA,     // Level AA compliance (recommended)
    AAA     // Level AAA compliance (highest)
};

// Live region types for dynamic content
enum class LiveRegionType {
    Off,        // No announcements
    Polite,     // Announce when user is idle
    Assertive   // Announce immediately
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
    QStringList controls;  // IDs of controlled elements
    QStringList describedBy;  // IDs of describing elements
    double contrastRatio{0.0};
};

// Accessibility issue for compliance checking
struct AccessibilityIssue {
    QString widgetName;
    QString issueType;
    QString description;
    QString suggestion;
    WcagLevel requiredLevel;
    QString wcagCriterion;
    bool isBlocking{false};
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
    void announceToScreenReader(const QString& message, LiveRegionType urgency = LiveRegionType::Polite);
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
    
    double calculateContrastRatio(const QColor& foreground, const QColor& background) const;
    QColor ensureContrast(const QColor& foreground, const QColor& background, double minRatio) const;
    
    // Announcements
    void announceGlobally(const QString& message, LiveRegionType urgency = LiveRegionType::Polite);
    void setAnnouncementDelay(int milliseconds) { m_announcementDelay = milliseconds; }
    
    // Accessibility testing and validation
    QList<AccessibilityIssue> validateAccessibility(QWidget* rootWidget);
    void generateAccessibilityReport(QWidget* rootWidget, const QString& filename);
    bool checkWcagCompliance(QWidget* widget, WcagLevel level);
    
    // Widget registration
    void registerAccessibleWidget(QWidget* widget, FluentEnhancedAccessible* accessible);
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

private slots:
    void processAnnouncementQueue();
    void onSystemAccessibilityChanged();

private:
    FluentAccessibilityManager();
    void validateWidget(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkColorContrast(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkKeyboardAccess(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkFocusManagement(QWidget* widget, QList<AccessibilityIssue>& issues);
    void checkTextAlternatives(QWidget* widget, QList<AccessibilityIssue>& issues);

private:
    WcagLevel m_wcagLevel{WcagLevel::AA};
    bool m_highContrastMode{false};
    bool m_reducedMotionMode{false};
    bool m_screenReaderMode{false};
    bool m_keyboardNavigation{true};
    bool m_focusIndicatorVisible{true};
    double m_minimumContrastRatio{4.5}; // WCAG AA standard
    
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
    static double calculateContrastRatio(const QColor& color1, const QColor& color2);
    static bool meetsWcagContrast(const QColor& foreground, const QColor& background, WcagLevel level);
    
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

} // namespace FluentQt::Accessibility
