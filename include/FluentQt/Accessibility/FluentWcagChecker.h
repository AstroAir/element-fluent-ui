// include/FluentQt/Accessibility/FluentWcagChecker.h
#pragma once

#include <QColor>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QWidget>
#include <functional>
#include <memory>
#include <unordered_map>

#include "FluentAccessibilityManager.h"

namespace FluentQt::Accessibility {

// WCAG 2.1 Success Criteria
enum class WcagCriterion {
    // Perceivable
    NonTextContent,             // 1.1.1
    AudioVideo,                 // 1.2.1
    Captions,                   // 1.2.2
    AudioDescription,           // 1.2.3
    Captions_Live,              // 1.2.4
    AudioDescription_Extended,  // 1.2.5
    SignLanguage,               // 1.2.6
    ExtendedAudioDescription,   // 1.2.7
    MediaAlternative,           // 1.2.8
    AudioOnly,                  // 1.2.9
    InfoAndRelationships,       // 1.3.1
    MeaningfulSequence,         // 1.3.2
    SensoryCharacteristics,     // 1.3.3
    Orientation,                // 1.3.4
    IdentifyInputPurpose,       // 1.3.5
    IdentifyPurpose,            // 1.3.6
    UseOfColor,                 // 1.4.1
    AudioControl,               // 1.4.2
    ContrastMinimum,            // 1.4.3
    ResizeText,                 // 1.4.4
    ImagesOfText,               // 1.4.5
    ContrastEnhanced,           // 1.4.6
    LowOrNoBackgroundAudio,     // 1.4.7
    VisualPresentation,         // 1.4.8
    ImagesOfTextNoException,    // 1.4.9
    Reflow,                     // 1.4.10
    NonTextContrast,            // 1.4.11
    TextSpacing,                // 1.4.12
    ContentOnHoverOrFocus,      // 1.4.13

    // Operable
    KeyboardAccess,             // 2.1.1
    NoKeyboardTrap,             // 2.1.2
    KeyboardNoException,        // 2.1.3
    CharacterKeyShortcuts,      // 2.1.4
    TimingAdjustable,           // 2.2.1
    PauseStopHide,              // 2.2.2
    NoTiming,                   // 2.2.3
    Interruptions,              // 2.2.4
    Reauthentication,           // 2.2.5
    TimeoutWarning,             // 2.2.6
    ThreeFlashes,               // 2.3.1
    ThreeFlashesBelow,          // 2.3.2
    AnimationFromInteractions,  // 2.3.3
    BypassBlocks,               // 2.4.1
    PageTitled,                 // 2.4.2
    FocusOrder,                 // 2.4.3
    LinkPurpose,                // 2.4.4
    MultipleWays,               // 2.4.5
    HeadingsAndLabels,          // 2.4.6
    FocusVisible,               // 2.4.7
    Location,                   // 2.4.8
    LinkPurposeOnly,            // 2.4.9
    SectionHeadings,            // 2.4.10
    PointerGestures,            // 2.5.1
    PointerCancellation,        // 2.5.2
    LabelInName,                // 2.5.3
    MotionActuation,            // 2.5.4
    TargetSize,                 // 2.5.5
    ConcurrentInputMechanisms,  // 2.5.6

    // Understandable
    LanguageOfPage,            // 3.1.1
    LanguageOfParts,           // 3.1.2
    UnusualWords,              // 3.1.3
    Abbreviations,             // 3.1.4
    ReadingLevel,              // 3.1.5
    Pronunciation,             // 3.1.6
    OnFocus,                   // 3.2.1
    OnInput,                   // 3.2.2
    ConsistentNavigation,      // 3.2.3
    ConsistentIdentification,  // 3.2.4
    ChangeOnRequest,           // 3.2.5
    ErrorIdentification,       // 3.3.1
    LabelsOrInstructions,      // 3.3.2
    ErrorSuggestion,           // 3.3.3
    ErrorPrevention,           // 3.3.4
    Help,                      // 3.3.5
    ErrorPrevention_All,       // 3.3.6

    // Robust
    Parsing,        // 4.1.1
    NameRoleValue,  // 4.1.2
    StatusMessages  // 4.1.3
};

// WCAG 2.1 Principle
enum class WcagPrinciple {
    Perceivable,     // 1.x
    Operable,        // 2.x
    Understandable,  // 3.x
    Robust           // 4.x
};

// WCAG 2.1 Guideline
enum class WcagGuideline {
    TextAlternatives,    // 1.1
    TimeBasedMedia,      // 1.2
    Adaptable,           // 1.3
    Distinguishable,     // 1.4
    KeyboardAccessible,  // 2.1
    EnoughTime,          // 2.2
    Seizures,            // 2.3
    Navigable,           // 2.4
    InputModalities,     // 2.5
    Readable,            // 3.1
    Predictable,         // 3.2
    InputAssistance,     // 3.3
    Compatible           // 4.1
};

// WCAG 2.1 Technique
enum class WcagTechnique {
    // General techniques
    G1,
    G4,
    G5,
    G8,
    G9,
    G10,
    G11,
    G13,
    G14,
    G17,
    G18,
    G19,
    G21,
    G53,
    G54,
    G55,
    G56,
    G57,
    G58,
    G59,
    G60,
    G61,
    G62,
    G63,
    G64,
    G65,
    G68,
    G69,
    G70,
    G71,
    G73,
    G74,
    G75,
    G76,
    G78,
    G79,
    G80,
    G81,
    G82,
    G83,
    G84,
    G85,
    G86,
    G87,
    G88,
    G89,
    G90,
    G91,
    G92,
    G93,
    G94,
    G95,
    G96,
    G97,
    G98,
    G99,
    G100,
    G101,
    G102,
    G103,
    G105,
    G107,
    G108,
    G110,
    G111,
    G112,
    G115,
    G117,
    G120,
    G121,
    G123,
    G124,
    G125,
    G126,
    G127,
    G128,
    G130,
    G131,
    G133,
    G134,
    G135,
    G136,
    G138,
    G139,
    G140,
    G141,
    G142,
    G143,
    G144,
    G145,
    G146,
    G148,
    G149,
    G150,
    G151,
    G152,
    G153,
    G155,
    G156,
    G157,
    G158,
    G159,
    G160,
    G161,
    G162,
    G163,
    G164,
    G165,
    G166,
    G167,
    G168,
    G169,
    G170,
    G171,
    G172,
    G173,
    G174,
    G175,
    G176,
    G177,
    G178,
    G179,
    G180,
    G181,
    G182,
    G183,
    G184,
    G185,
    G186,
    G187,
    G188,
    G189,
    G190,
    G191,
    G192,
    G193,
    G194,
    G195,
    G196,
    G197,
    G198,
    G199,
    G200,
    G201,
    G202,
    G203,
    G204,
    G205,
    G206,
    G207,
    G208,
    G209,
    G210,
    G211,
    G212,
    G213,
    G214,
    G215,
    G216,
    G217,
    G218,
    G219,
    G220,
    G221,
    G222,
    G223,
    G224,
    G225,

    // HTML techniques
    H2,
    H4,
    H24,
    H25,
    H28,
    H30,
    H32,
    H33,
    H34,
    H36,
    H37,
    H39,
    H40,
    H42,
    H43,
    H44,
    H45,
    H46,
    H48,
    H49,
    H51,
    H53,
    H54,
    H56,
    H57,
    H58,
    H59,
    H60,
    H62,
    H63,
    H64,
    H65,
    H67,
    H69,
    H70,
    H71,
    H73,
    H74,
    H75,
    H76,
    H77,
    H78,
    H79,
    H80,
    H81,
    H83,
    H84,
    H85,
    H86,
    H87,
    H88,
    H89,
    H90,
    H91,
    H93,
    H94,
    H95,
    H96,
    H97,
    H98,
    H99,

    // ARIA techniques
    ARIA1,
    ARIA2,
    ARIA4,
    ARIA5,
    ARIA6,
    ARIA7,
    ARIA8,
    ARIA9,
    ARIA10,
    ARIA11,
    ARIA12,
    ARIA13,
    ARIA14,
    ARIA15,
    ARIA16,
    ARIA17,
    ARIA18,
    ARIA19,
    ARIA20,
    ARIA21,
    ARIA22,
    ARIA23,
    ARIA24,
    ARIA25,
    ARIA26,
    ARIA27,
    ARIA28,
    ARIA29,
    ARIA30,
    ARIA31,
    ARIA32,
    ARIA33,
    ARIA34,
    ARIA35,
    ARIA36,
    ARIA37,
    ARIA38,
    ARIA39,
    ARIA40,
    ARIA41,
    ARIA42,
    ARIA43,
    ARIA44,
    ARIA45,
    ARIA46,
    ARIA47,
    ARIA48,
    ARIA49,
    ARIA50,
    ARIA51,
    ARIA52,
    ARIA53,
    ARIA54,
    ARIA55,
    ARIA56,
    ARIA57,
    ARIA58,
    ARIA59,
    ARIA60,
    ARIA61,
    ARIA62,
    ARIA63,
    ARIA64,
    ARIA65,
    ARIA66,
    ARIA67,
    ARIA68,
    ARIA69,
    ARIA70,
    ARIA71,
    ARIA72,
    ARIA73,
    ARIA74,
    ARIA75,
    ARIA76,
    ARIA77,
    ARIA78,
    ARIA79,
    ARIA80,
    ARIA81,
    ARIA82,
    ARIA83,
    ARIA84,
    ARIA85,
    ARIA86,
    ARIA87,
    ARIA88,
    ARIA89,
    ARIA90,
    ARIA91,
    ARIA92,
    ARIA93,
    ARIA94,
    ARIA95,
    ARIA96,
    ARIA97,
    ARIA98,
    ARIA99,
    ARIA100
};

// WCAG 2.1 Compliance Report
struct WcagComplianceReport {
    QString applicationName;
    QString reportDate;
    QString reportVersion;
    WcagLevel targetLevel{WcagLevel::AA};
    int totalIssues{0};
    int criticalIssues{0};
    int majorIssues{0};
    int minorIssues{0};
    double complianceScore{0.0};  // 0-100
    QList<AccessibilityIssue> issues;
    QMap<WcagCriterion, bool> criteriaResults;
    QMap<WcagPrinciple, double> principleScores;
    QStringList passedTests;
    QStringList failedTests;
    QStringList manualCheckNeeded;
    QStringList recommendations;
};

// WCAG 2.1 Test Result
struct WcagTestResult {
    WcagCriterion criterion;
    bool passed{false};
    QList<AccessibilityIssue> issues;
    QString details;
    bool requiresManualCheck{false};
    QStringList affectedWidgets;
    QStringList recommendations;
    WcagTechnique technique;
};

// WCAG 2.1 Compliance Checker
class FluentWcagChecker : public QObject {
    Q_OBJECT

public:
    static FluentWcagChecker& instance();

    // Comprehensive WCAG 2.1 compliance checking
    WcagComplianceReport checkWcagCompliance(QWidget* rootWidget,
                                             WcagLevel level = WcagLevel::AA);

    // Individual criterion checks
    WcagTestResult checkCriterion(QWidget* rootWidget, WcagCriterion criterion);
    QList<WcagTestResult> checkPrinciple(QWidget* rootWidget,
                                         WcagPrinciple principle,
                                         WcagLevel level = WcagLevel::AA);
    QList<WcagTestResult> checkGuideline(QWidget* rootWidget,
                                         WcagGuideline guideline,
                                         WcagLevel level = WcagLevel::AA);

    // Specific WCAG 2.1 tests
    bool checkTextAlternatives(QWidget* widget);
    bool checkKeyboardAccessibility(QWidget* widget);
    bool checkColorContrast(QWidget* widget, WcagLevel level = WcagLevel::AA);
    bool checkFocusVisibility(QWidget* widget);
    bool checkFormLabels(QWidget* widget);
    bool checkErrorIdentification(QWidget* widget);
    bool checkHeadingStructure(QWidget* rootWidget);
    bool checkLandmarkStructure(QWidget* rootWidget);
    bool checkTargetSize(QWidget* widget);
    bool checkMotionActuation(QWidget* widget);
    bool checkOrientationLock(QWidget* rootWidget);
    bool checkReflow(QWidget* rootWidget);
    bool checkStatusMessages(QWidget* widget);
    bool checkPointerGestures(QWidget* widget);

    // Contrast ratio calculation
    double calculateContrastRatio(const QColor& foreground,
                                  const QColor& background);
    bool meetsContrastRequirements(double ratio, WcagLevel level,
                                   bool isLargeText = false);

    // Report generation
    void generateHtmlReport(const WcagComplianceReport& report,
                            const QString& filename);
    void generateJsonReport(const WcagComplianceReport& report,
                            const QString& filename);
    void generatePdfReport(const WcagComplianceReport& report,
                           const QString& filename);

    // Automated fixes
    void applyAutomatedFixes(QWidget* rootWidget,
                             const WcagComplianceReport& report);
    bool canAutoFix(const AccessibilityIssue& issue);
    void applyFix(QWidget* widget, const AccessibilityIssue& issue);

    // WCAG 2.1 metadata
    QString getCriterionDescription(WcagCriterion criterion);
    QString getCriterionReference(WcagCriterion criterion);
    WcagLevel getCriterionLevel(WcagCriterion criterion);
    QList<WcagTechnique> getTechniquesForCriterion(WcagCriterion criterion);
    QString getTechniqueDescription(WcagTechnique technique);

    // Configuration
    void setTestTimeout(int milliseconds) { m_testTimeout = milliseconds; }
    void enableDetailedReporting(bool enabled) {
        m_detailedReporting = enabled;
    }
    void setMaxIssuesPerCriterion(int max) { m_maxIssuesPerCriterion = max; }
    void setAutomaticFixesEnabled(bool enabled) {
        m_automaticFixesEnabled = enabled;
    }
    void setManualCheckPrompts(bool enabled) { m_manualCheckPrompts = enabled; }

signals:
    void complianceCheckStarted(QWidget* rootWidget, WcagLevel level);
    void complianceCheckCompleted(const WcagComplianceReport& report);
    void criterionCheckStarted(WcagCriterion criterion);
    void criterionCheckCompleted(const WcagTestResult& result);
    void accessibilityIssueFound(const AccessibilityIssue& issue);
    void manualCheckNeeded(WcagCriterion criterion,
                           const QString& instructions);
    void fixApplied(QWidget* widget, const AccessibilityIssue& issue);
    void reportGenerated(const QString& filename);

private:
    FluentWcagChecker();
    void registerTestFunctions();
    void initializeCriterionMetadata();

    // Test implementation helpers
    WcagTestResult runTest(
        QWidget* rootWidget, WcagCriterion criterion,
        std::function<bool(QWidget*, QList<AccessibilityIssue>&)> testFunc);
    void checkWidgetRecursively(
        QWidget* widget, WcagCriterion criterion,
        std::function<bool(QWidget*, QList<AccessibilityIssue>&)> testFunc,
        QList<AccessibilityIssue>& issues);

    // Specific test implementations
    bool testNonTextContent(QWidget* widget, QList<AccessibilityIssue>& issues);
    bool testKeyboardAccess(QWidget* widget, QList<AccessibilityIssue>& issues);
    bool testNoKeyboardTrap(QWidget* widget, QList<AccessibilityIssue>& issues);
    bool testContrastMinimum(QWidget* widget,
                             QList<AccessibilityIssue>& issues);
    bool testResizeText(QWidget* widget, QList<AccessibilityIssue>& issues);
    bool testFocusVisible(QWidget* widget, QList<AccessibilityIssue>& issues);
    bool testErrorIdentification(QWidget* widget,
                                 QList<AccessibilityIssue>& issues);
    bool testLabelsOrInstructions(QWidget* widget,
                                  QList<AccessibilityIssue>& issues);
    bool testNameRoleValue(QWidget* widget, QList<AccessibilityIssue>& issues);
    bool testTargetSize(QWidget* widget, QList<AccessibilityIssue>& issues);
    bool testMotionActuation(QWidget* widget,
                             QList<AccessibilityIssue>& issues);
    bool testStatusMessages(QWidget* widget, QList<AccessibilityIssue>& issues);

    // Report generation helpers
    QString generateCriterionSummary(const WcagTestResult& result);
    QString generateIssueSummary(const AccessibilityIssue& issue);
    double calculateComplianceScore(const QMap<WcagCriterion, bool>& results,
                                    WcagLevel level);

    // Fix implementation helpers
    void applyContrastFix(QWidget* widget, const AccessibilityIssue& issue);
    void applyLabelFix(QWidget* widget, const AccessibilityIssue& issue);
    void applyFocusIndicatorFix(QWidget* widget,
                                const AccessibilityIssue& issue);
    void applyTargetSizeFix(QWidget* widget, const AccessibilityIssue& issue);
    void applyKeyboardAccessFix(QWidget* widget,
                                const AccessibilityIssue& issue);

private:
    // Configuration
    int m_testTimeout{5000};
    bool m_detailedReporting{true};
    int m_maxIssuesPerCriterion{10};
    bool m_automaticFixesEnabled{false};
    bool m_manualCheckPrompts{true};

    // Test registry
    std::unordered_map<
        WcagCriterion,
        std::function<bool(QWidget*, QList<AccessibilityIssue>&)>>
        m_tests;

    // WCAG metadata
    QMap<WcagCriterion, QString> m_criterionDescriptions;
    QMap<WcagCriterion, WcagLevel> m_criterionLevels;
    QMap<WcagCriterion, QList<WcagTechnique>> m_criterionTechniques;
    QMap<WcagTechnique, QString> m_techniqueDescriptions;

    // Thread safety
    mutable QMutex m_mutex;
};

}  // namespace FluentQt::Accessibility
