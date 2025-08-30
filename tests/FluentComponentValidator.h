#ifndef FLUENTCOMPONENTVALIDATOR_H
#define FLUENTCOMPONENTVALIDATOR_H

#include <QAccessible>
#include <QApplication>
#include <QDebug>
#include <QGraphicsEffect>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QPropertyAnimation>
#include <QStyleOption>
#include <QTimer>
#include <QWidget>
#include <chrono>
#include <functional>
#include <memory>

#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/FluentQt.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;
using namespace std::chrono_literals;

/**
 * @brief Validation result for a single test
 */
struct ValidationResult {
    QString testName;
    QString componentName;
    QString category;
    bool passed;
    QString details;
    QString expectedValue;
    QString actualValue;
    std::chrono::milliseconds duration;

    ValidationResult(const QString& name, const QString& component,
                     const QString& cat, bool pass, const QString& detail = "",
                     const QString& expected = "", const QString& actual = "")
        : testName(name),
          componentName(component),
          category(cat),
          passed(pass),
          details(detail),
          expectedValue(expected),
          actualValue(actual),
          duration(0ms) {}
};

/**
 * @brief Component validation requirements
 */
struct ComponentValidationSpec {
    QString componentName;
    QStringList requiredProperties;
    QStringList requiredMethods;
    QStringList requiredSignals;
    QStringList expectedAnimations;
    QStringList fluentDesignRequirements;
    bool requiresAccessibility;
    bool requiresThemeSupport;
    std::chrono::milliseconds expectedAnimationDuration;
    Animation::FluentEasing expectedEasing;
};

/**
 * @brief Comprehensive FluentQt component validation system
 */
class FluentComponentValidator : public QObject {
    Q_OBJECT

public:
    explicit FluentComponentValidator(QObject* parent = nullptr);
    ~FluentComponentValidator() = default;

    // Main validation methods
    void validateAllComponents();
    void validateComponent(const QString& componentName);
    void validateComponent(QWidget* component, const QString& componentName);

    // Specific validation categories
    bool validateInterfaceCompleteness(QWidget* component,
                                       const ComponentValidationSpec& spec);
    bool validateAnimationSystem(QWidget* component,
                                 const ComponentValidationSpec& spec);
    bool validateFluentDesignCompliance(QWidget* component,
                                        const ComponentValidationSpec& spec);
    bool validateComponentSpecificBehavior(QWidget* component,
                                           const ComponentValidationSpec& spec);
    bool validateCrossPlatformCompatibility(
        QWidget* component, const ComponentValidationSpec& spec);

    // Animation-specific validation
    bool validateAnimationTiming(QPropertyAnimation* animation,
                                 std::chrono::milliseconds expected);
    bool validateEasingCurve(QPropertyAnimation* animation,
                             Animation::FluentEasing expected);
    bool validateHardwareAcceleration(QPropertyAnimation* animation);
    bool validateReducedMotionSupport(QWidget* component);

    // Fluent Design validation
    bool validateFluentColors(QWidget* component);
    bool validateFluentTypography(QWidget* component);
    bool validateFluentSpacing(QWidget* component);
    bool validateStateTransitions(QWidget* component);
    bool validateThemeSupport(QWidget* component);

    // Accessibility validation
    bool validateAccessibilityCompliance(QWidget* component);
    bool validateKeyboardNavigation(QWidget* component);
    bool validateScreenReaderSupport(QWidget* component);
    bool validateFocusManagement(QWidget* component);

    // Results and reporting
    QList<ValidationResult> getResults() const { return m_results; }
    void generateReport(const QString& outputPath = "");
    void printSummary() const;

    // Configuration
    void setStrictMode(bool strict) { m_strictMode = strict; }
    void setVerboseOutput(bool verbose) { m_verboseOutput = verbose; }
    void setPlatformSpecific(bool enabled) { m_platformSpecific = enabled; }

    // Public helper for specialized validators
    void addResult(const QString& testName, const QString& componentName,
                   const QString& category, bool passed,
                   const QString& details = "", const QString& expected = "",
                   const QString& actual = "");

signals:
    void validationStarted(const QString& componentName);
    void validationCompleted(const QString& componentName, bool passed);
    void testCompleted(const ValidationResult& result);
    void allValidationsCompleted(int totalTests, int passedTests);

private slots:
    void onAnimationFinished();
    void onThemeChanged();

private:
    // Helper methods
    QWidget* createComponent(const QString& componentName);
    ComponentValidationSpec getValidationSpec(const QString& componentName);
    void addResult(const ValidationResult& result);

    // Component creation helpers
    template <typename T>
    T* createTypedComponent() {
        return new T();
    }

    // Animation testing helpers
    std::unique_ptr<QPropertyAnimation> captureAnimation(
        QWidget* component, const QString& trigger);
    bool triggerHoverAnimation(QWidget* component);
    bool triggerPressAnimation(QWidget* component);
    bool triggerFocusAnimation(QWidget* component);

    // Timing helpers
    void startTimer() { m_testStartTime = std::chrono::steady_clock::now(); }
    std::chrono::milliseconds getElapsedTime() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            now - m_testStartTime);
    }

private:
    QList<ValidationResult> m_results;
    bool m_strictMode{false};
    bool m_verboseOutput{true};
    bool m_platformSpecific{true};
    std::chrono::steady_clock::time_point m_testStartTime;

    // Test state
    QWidget* m_currentTestWidget{nullptr};
    QPropertyAnimation* m_currentAnimation{nullptr};
    QTimer* m_animationTimeout{nullptr};

    // Component specifications
    QHash<QString, ComponentValidationSpec> m_componentSpecs;

    // Statistics
    int m_totalTests{0};
    int m_passedTests{0};
    int m_failedTests{0};

    void initializeComponentSpecs();
    void setupAnimationCapture();
    void cleanupTestWidget();
};

/**
 * @brief Specialized validators for different component categories
 */
class FormComponentValidator {
public:
    static bool validateButton(Components::FluentButton* button,
                               FluentComponentValidator* validator);
    static bool validateTextInput(Components::FluentTextInput* input,
                                  FluentComponentValidator* validator);
    static bool validateCheckBox(Components::FluentCheckBox* checkbox,
                                 FluentComponentValidator* validator);
    static bool validateComboBox(Components::FluentComboBox* combobox,
                                 FluentComponentValidator* validator);
    static bool validateSlider(Components::FluentSlider* slider,
                               FluentComponentValidator* validator);
};

class LayoutComponentValidator {
public:
    static bool validateCard(Components::FluentCard* card,
                             FluentComponentValidator* validator);
    static bool validatePanel(Components::FluentPanel* panel,
                              FluentComponentValidator* validator);
    static bool validateNavigationView(Components::FluentNavigationView* nav,
                                       FluentComponentValidator* validator);
    static bool validateTabView(Components::FluentTabView* tabs,
                                FluentComponentValidator* validator);
};

class FeedbackComponentValidator {
public:
    static bool validateProgressBar(Components::FluentProgressBar* progress,
                                    FluentComponentValidator* validator);
    static bool validateToast(Components::FluentToast* toast,
                              FluentComponentValidator* validator);
    static bool validateTooltip(Components::FluentTooltip* tooltip,
                                FluentComponentValidator* validator);
    static bool validateBadge(Components::FluentBadge* badge,
                              FluentComponentValidator* validator);
};

class DialogComponentValidator {
public:
    // Note: FluentContentDialog and FluentPopover don't exist yet
    // These validators are placeholders for future implementation
    static bool validateBasicDialog(QWidget* dialog,
                                    FluentComponentValidator* validator);
};

/**
 * @brief Platform-specific validation helpers
 */
class PlatformValidator {
public:
    static bool validateWindowsSpecific(QWidget* component);
    static bool validateMacOSSpecific(QWidget* component);
    static bool validateLinuxSpecific(QWidget* component);
    static bool validateHighDPISupport(QWidget* component);
    static bool validatePerformanceCharacteristics(QWidget* component);
};

/**
 * @brief Animation system validator
 */
class AnimationSystemValidator {
public:
    static bool validateFluentEasingCurves(QPropertyAnimation* animation);
    static bool validateMotionHierarchy(QWidget* component);
    static bool validateConnectedAnimations(QWidget* fromComponent,
                                            QWidget* toComponent);
    static bool validateEntranceExitAnimations(QWidget* component);
    static bool validatePerformanceOptimizations(QPropertyAnimation* animation);
};

/**
 * @brief Fluent Design System compliance validator
 */
class FluentDesignValidator {
public:
    static bool validateColorSystem(QWidget* component);
    static bool validateTypographySystem(QWidget* component);
    static bool validateSpacingSystem(QWidget* component);
    static bool validateElevationSystem(QWidget* component);
    static bool validateIconography(QWidget* component);
    static bool validateStateSystem(QWidget* component);
};

#endif  // FLUENTCOMPONENTVALIDATOR_H
