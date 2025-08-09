// src/Accessibility/FluentWcagChecker.cpp
#include "FluentQt/Accessibility/FluentWcagChecker.h"
#include "FluentQt/Accessibility/FluentAccessibilityManager.h"
#include <QApplication>
#include <QDebug>
#include <QMutexLocker>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QSlider>
#include <QProgressBar>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <algorithm>
#include <cmath>

namespace FluentQt::Accessibility {

FluentWcagChecker& FluentWcagChecker::instance() {
    static FluentWcagChecker instance;
    return instance;
}

FluentWcagChecker::FluentWcagChecker() {
    // Initialize WCAG metadata
    initializeCriterionMetadata();
    
    // Register test functions
    registerTestFunctions();
    
    qDebug() << "FluentWcagChecker initialized";
}

WcagComplianceReport FluentWcagChecker::checkWcagCompliance(QWidget* rootWidget, WcagLevel level) {
    WcagComplianceReport report;
    
    if (!rootWidget) {
        return report;
    }
    
    emit complianceCheckStarted(rootWidget, level);
    
    report.applicationName = QApplication::applicationName();
    report.reportDate = QDateTime::currentDateTime().toString(Qt::ISODate);
    report.reportVersion = "1.0";
    report.targetLevel = level;
    
    QMutexLocker locker(&m_mutex);
    
    // Check all relevant criteria for the target level
    QList<WcagCriterion> criteriaToCheck;
    for (auto it = m_criterionLevels.begin(); it != m_criterionLevels.end(); ++it) {
        if (static_cast<int>(it.value()) <= static_cast<int>(level)) {
            criteriaToCheck.append(it.key());
        }
    }
    
    locker.unlock();
    
    // Run tests for each criterion
    for (WcagCriterion criterion : criteriaToCheck) {
        WcagTestResult result = checkCriterion(rootWidget, criterion);
        
        report.criteriaResults[criterion] = result.passed;
        if (!result.passed) {
            report.issues.append(result.issues);
            report.failedTests.append(getCriterionReference(criterion));
        } else {
            report.passedTests.append(getCriterionReference(criterion));
        }
        
        if (result.requiresManualCheck) {
            report.manualCheckNeeded.append(getCriterionReference(criterion));
        }
    }
    
    // Calculate statistics
    report.totalIssues = report.issues.size();
    report.criticalIssues = 0;
    report.majorIssues = 0;
    report.minorIssues = 0;
    
    for (const AccessibilityIssue& issue : report.issues) {
        switch (issue.severity) {
            case 5:
                report.criticalIssues++;
                break;
            case 4:
                report.majorIssues++;
                break;
            case 3:
                report.minorIssues++;
                break;
            default:
                break;
        }
    }
    
    // Calculate compliance score
    report.complianceScore = calculateComplianceScore(report.criteriaResults, level);
    
    // Calculate principle scores
    report.principleScores[WcagPrinciple::Perceivable] = 0.0;
    report.principleScores[WcagPrinciple::Operable] = 0.0;
    report.principleScores[WcagPrinciple::Understandable] = 0.0;
    report.principleScores[WcagPrinciple::Robust] = 0.0;
    
    // Generate recommendations
    if (report.criticalIssues > 0) {
        report.recommendations.append("Address critical accessibility issues immediately");
    }
    if (report.majorIssues > 0) {
        report.recommendations.append("Fix major accessibility barriers");
    }
    if (report.complianceScore < 80.0) {
        report.recommendations.append("Comprehensive accessibility review recommended");
    }
    
    emit complianceCheckCompleted(report);
    return report;
}

WcagTestResult FluentWcagChecker::checkCriterion(QWidget* rootWidget, WcagCriterion criterion) {
    WcagTestResult result;
    result.criterion = criterion;
    
    emit criterionCheckStarted(criterion);
    
    QMutexLocker locker(&m_mutex);
    auto testIt = m_tests.find(criterion);
    locker.unlock();
    
    if (testIt != m_tests.end()) {
        result = runTest(rootWidget, criterion, testIt->second);
    } else {
        // No test implementation available
        result.passed = true;
        result.requiresManualCheck = true;
        result.details = "Manual check required - no automated test available";
    }
    
    emit criterionCheckCompleted(result);
    return result;
}

bool FluentWcagChecker::checkTextAlternatives(QWidget* widget) {
    if (!widget) return true;
    
    // Check images, icons, and other non-text content
    if (widget->inherits("QLabel")) {
        QLabel* label = qobject_cast<QLabel*>(widget);
        if (label && !label->pixmap().isNull()) {
            // Has image but check for alt text
            return !label->accessibleName().isEmpty() || !label->accessibleDescription().isEmpty();
        }
    }
    
    // Check buttons with icons
    if (widget->inherits("QPushButton")) {
        QPushButton* button = qobject_cast<QPushButton*>(widget);
        if (button && !button->icon().isNull()) {
            // Has icon, check for text or accessible name
            return !button->text().isEmpty() || !button->accessibleName().isEmpty();
        }
    }
    
    return true;
}

bool FluentWcagChecker::checkKeyboardAccessibility(QWidget* widget) {
    if (!widget) return true;
    
    // Check if interactive widgets are keyboard accessible
    if (widget->inherits("QPushButton") || 
        widget->inherits("QLineEdit") ||
        widget->inherits("QComboBox") ||
        widget->inherits("QCheckBox") ||
        widget->inherits("QRadioButton") ||
        widget->inherits("QSlider")) {
        
        return widget->focusPolicy() != Qt::NoFocus && 
               widget->isEnabled() && 
               widget->isVisible();
    }
    
    return true;
}

bool FluentWcagChecker::checkColorContrast(QWidget* widget, WcagLevel level) {
    if (!widget) return true;
    
    // Get widget colors
    QPalette palette = widget->palette();
    QColor foreground = palette.color(QPalette::WindowText);
    QColor background = palette.color(QPalette::Window);
    
    // Calculate contrast ratio
    double ratio = calculateContrastRatio(foreground, background);
    
    // Check against WCAG requirements
    return meetsContrastRequirements(ratio, level, false);
}

bool FluentWcagChecker::checkFocusVisibility(QWidget* widget) {
    if (!widget) return true;
    
    // Check if widget has visible focus indicator
    if (widget->focusPolicy() != Qt::NoFocus) {
        // This is a simplified check - in practice, you'd need to verify
        // that the focus indicator is actually visible and meets contrast requirements
        return true;
    }
    
    return true;
}

bool FluentWcagChecker::checkFormLabels(QWidget* widget) {
    if (!widget) return true;
    
    // Check if form controls have proper labels
    if (widget->inherits("QLineEdit") || 
        widget->inherits("QTextEdit") ||
        widget->inherits("QComboBox") ||
        widget->inherits("QSlider")) {
        
        // Check for accessible name
        if (!widget->accessibleName().isEmpty()) {
            return true;
        }
        
        // Check for associated label
        QWidget* parent = widget->parentWidget();
        if (parent) {
            auto labels = parent->findChildren<QLabel*>();
            for (QLabel* label : labels) {
                if (label->buddy() == widget) {
                    return true;
                }
            }
        }
        
        return false; // Form control without label
    }
    
    return true;
}

bool FluentWcagChecker::checkTargetSize(QWidget* widget) {
    if (!widget) return true;
    
    // Check if interactive elements meet minimum target size (44x44 CSS pixels)
    if (widget->inherits("QPushButton") || 
        widget->inherits("QCheckBox") ||
        widget->inherits("QRadioButton")) {
        
        QSize size = widget->size();
        const int minSize = 44; // 44 CSS pixels
        
        return size.width() >= minSize && size.height() >= minSize;
    }
    
    return true;
}

double FluentWcagChecker::calculateContrastRatio(const QColor& foreground, const QColor& background) {
    // Convert to relative luminance
    auto relativeLuminance = [](const QColor& color) -> double {
        auto sRGB = [](double value) -> double {
            value /= 255.0;
            if (value <= 0.03928) {
                return value / 12.92;
            } else {
                return std::pow((value + 0.055) / 1.055, 2.4);
            }
        };
        
        double r = sRGB(color.red());
        double g = sRGB(color.green());
        double b = sRGB(color.blue());
        
        return 0.2126 * r + 0.7152 * g + 0.0722 * b;
    };
    
    double l1 = relativeLuminance(foreground);
    double l2 = relativeLuminance(background);
    
    // Ensure l1 is the lighter color
    if (l1 < l2) {
        std::swap(l1, l2);
    }
    
    return (l1 + 0.05) / (l2 + 0.05);
}

bool FluentWcagChecker::meetsContrastRequirements(double ratio, WcagLevel level, bool isLargeText) {
    switch (level) {
        case WcagLevel::A:
            return true; // No contrast requirements for Level A
        case WcagLevel::AA:
            return isLargeText ? ratio >= 3.0 : ratio >= 4.5;
        case WcagLevel::AAA:
            return isLargeText ? ratio >= 4.5 : ratio >= 7.0;
    }
    
    return false;
}

QString FluentWcagChecker::getCriterionDescription(WcagCriterion criterion) {
    QMutexLocker locker(&m_mutex);
    return m_criterionDescriptions.value(criterion, "No description available");
}

QString FluentWcagChecker::getCriterionReference(WcagCriterion criterion) {
    // Convert criterion enum to WCAG reference string
    switch (criterion) {
        case WcagCriterion::NonTextContent: return "1.1.1";
        case WcagCriterion::KeyboardAccess: return "2.1.1";
        case WcagCriterion::NoKeyboardTrap: return "2.1.2";
        case WcagCriterion::ContrastMinimum: return "1.4.3";
        case WcagCriterion::FocusVisible: return "2.4.7";
        case WcagCriterion::LabelsOrInstructions: return "3.3.2";
        case WcagCriterion::NameRoleValue: return "4.1.2";
        case WcagCriterion::TargetSize: return "2.5.5";
        default: return "Unknown";
    }
}

WcagLevel FluentWcagChecker::getCriterionLevel(WcagCriterion criterion) {
    QMutexLocker locker(&m_mutex);
    return m_criterionLevels.value(criterion, WcagLevel::AA);
}

// Private helper methods
WcagTestResult FluentWcagChecker::runTest(QWidget* rootWidget, WcagCriterion criterion, 
                                         std::function<bool(QWidget*, QList<AccessibilityIssue>&)> testFunc) {
    WcagTestResult result;
    result.criterion = criterion;
    result.passed = true;
    
    QList<AccessibilityIssue> issues;
    
    // Test the root widget and all children
    checkWidgetRecursively(rootWidget, criterion, testFunc, issues);
    
    result.issues = issues;
    result.passed = issues.isEmpty();
    result.details = QString("Found %1 issues").arg(issues.size());
    
    // Collect affected widgets
    for (const AccessibilityIssue& issue : issues) {
        if (!result.affectedWidgets.contains(issue.widgetName)) {
            result.affectedWidgets.append(issue.widgetName);
        }
    }
    
    return result;
}

void FluentWcagChecker::checkWidgetRecursively(QWidget* widget, WcagCriterion criterion, 
                                              std::function<bool(QWidget*, QList<AccessibilityIssue>&)> testFunc,
                                              QList<AccessibilityIssue>& issues) {
    if (!widget || !widget->isVisible()) {
        return;
    }
    
    // Test current widget
    testFunc(widget, issues);
    
    // Test child widgets
    for (QWidget* child : widget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly)) {
        checkWidgetRecursively(child, criterion, testFunc, issues);
    }
}

double FluentWcagChecker::calculateComplianceScore(const QMap<WcagCriterion, bool>& results, WcagLevel level) {
    Q_UNUSED(level)
    
    if (results.isEmpty()) {
        return 100.0;
    }
    
    int passed = 0;
    int total = results.size();
    
    for (auto it = results.begin(); it != results.end(); ++it) {
        if (it.value()) {
            passed++;
        }
    }
    
    return (static_cast<double>(passed) / total) * 100.0;
}

void FluentWcagChecker::initializeCriterionMetadata() {
    // Initialize criterion descriptions
    m_criterionDescriptions[WcagCriterion::NonTextContent] = "All non-text content has text alternatives";
    m_criterionDescriptions[WcagCriterion::KeyboardAccess] = "All functionality is available from a keyboard";
    m_criterionDescriptions[WcagCriterion::NoKeyboardTrap] = "Keyboard focus is not trapped";
    m_criterionDescriptions[WcagCriterion::ContrastMinimum] = "Text has sufficient contrast ratio";
    m_criterionDescriptions[WcagCriterion::FocusVisible] = "Keyboard focus indicator is visible";
    m_criterionDescriptions[WcagCriterion::LabelsOrInstructions] = "Form controls have labels or instructions";
    m_criterionDescriptions[WcagCriterion::NameRoleValue] = "UI components have accessible name and role";
    m_criterionDescriptions[WcagCriterion::TargetSize] = "Touch targets are at least 44x44 CSS pixels";
    
    // Initialize criterion levels
    m_criterionLevels[WcagCriterion::NonTextContent] = WcagLevel::A;
    m_criterionLevels[WcagCriterion::KeyboardAccess] = WcagLevel::A;
    m_criterionLevels[WcagCriterion::NoKeyboardTrap] = WcagLevel::A;
    m_criterionLevels[WcagCriterion::ContrastMinimum] = WcagLevel::AA;
    m_criterionLevels[WcagCriterion::FocusVisible] = WcagLevel::AA;
    m_criterionLevels[WcagCriterion::LabelsOrInstructions] = WcagLevel::A;
    m_criterionLevels[WcagCriterion::NameRoleValue] = WcagLevel::A;
    m_criterionLevels[WcagCriterion::TargetSize] = WcagLevel::AA;
}

void FluentWcagChecker::registerTestFunctions() {
    // Register test functions for each criterion
    m_tests[WcagCriterion::NonTextContent] = [this](QWidget* widget, QList<AccessibilityIssue>& issues) {
        return testNonTextContent(widget, issues);
    };
    
    m_tests[WcagCriterion::KeyboardAccess] = [this](QWidget* widget, QList<AccessibilityIssue>& issues) {
        return testKeyboardAccess(widget, issues);
    };
    
    m_tests[WcagCriterion::ContrastMinimum] = [this](QWidget* widget, QList<AccessibilityIssue>& issues) {
        return testContrastMinimum(widget, issues);
    };
    
    m_tests[WcagCriterion::FocusVisible] = [this](QWidget* widget, QList<AccessibilityIssue>& issues) {
        return testFocusVisible(widget, issues);
    };
    
    m_tests[WcagCriterion::LabelsOrInstructions] = [this](QWidget* widget, QList<AccessibilityIssue>& issues) {
        return testLabelsOrInstructions(widget, issues);
    };
    
    m_tests[WcagCriterion::TargetSize] = [this](QWidget* widget, QList<AccessibilityIssue>& issues) {
        return testTargetSize(widget, issues);
    };
}

bool FluentWcagChecker::testNonTextContent(QWidget* widget, QList<AccessibilityIssue>& issues) {
    if (!checkTextAlternatives(widget)) {
        AccessibilityIssue issue;
        issue.widgetName = widget->objectName().isEmpty() ? widget->metaObject()->className() : widget->objectName();
        issue.issueType = AccessibilityIssueType::MissingAltText;
        issue.description = "Non-text content lacks text alternative";
        issue.suggestion = "Add accessible name or description";
        issue.requiredLevel = WcagLevel::A;
        issue.wcagCriterion = "1.1.1";
        issue.severity = 4;
        issue.isBlocking = true;
        issue.isAutomaticallyFixable = true;
        issue.impactScore = 80.0;
        
        issues.append(issue);
        emit accessibilityIssueFound(issue);
        return false;
    }
    
    return true;
}

bool FluentWcagChecker::testKeyboardAccess(QWidget* widget, QList<AccessibilityIssue>& issues) {
    if (!checkKeyboardAccessibility(widget)) {
        AccessibilityIssue issue;
        issue.widgetName = widget->objectName().isEmpty() ? widget->metaObject()->className() : widget->objectName();
        issue.issueType = AccessibilityIssueType::NoKeyboardAccess;
        issue.description = "Interactive element is not keyboard accessible";
        issue.suggestion = "Set appropriate focus policy";
        issue.requiredLevel = WcagLevel::A;
        issue.wcagCriterion = "2.1.1";
        issue.severity = 5;
        issue.isBlocking = true;
        issue.isAutomaticallyFixable = true;
        issue.impactScore = 90.0;
        
        issues.append(issue);
        emit accessibilityIssueFound(issue);
        return false;
    }
    
    return true;
}

bool FluentWcagChecker::testContrastMinimum(QWidget* widget, QList<AccessibilityIssue>& issues) {
    if (!checkColorContrast(widget, WcagLevel::AA)) {
        AccessibilityIssue issue;
        issue.widgetName = widget->objectName().isEmpty() ? widget->metaObject()->className() : widget->objectName();
        issue.issueType = AccessibilityIssueType::InsufficientColorContrast;
        issue.description = "Text does not meet minimum contrast ratio";
        issue.suggestion = "Increase contrast ratio to at least 4.5:1";
        issue.requiredLevel = WcagLevel::AA;
        issue.wcagCriterion = "1.4.3";
        issue.severity = 4;
        issue.isBlocking = true;
        issue.isAutomaticallyFixable = true;
        issue.impactScore = 85.0;
        
        issues.append(issue);
        emit accessibilityIssueFound(issue);
        return false;
    }
    
    return true;
}

bool FluentWcagChecker::testFocusVisible(QWidget* widget, QList<AccessibilityIssue>& issues) {
    if (!checkFocusVisibility(widget)) {
        AccessibilityIssue issue;
        issue.widgetName = widget->objectName().isEmpty() ? widget->metaObject()->className() : widget->objectName();
        issue.issueType = AccessibilityIssueType::NoFocusIndicator;
        issue.description = "Focus indicator is not visible";
        issue.suggestion = "Ensure focus indicator is clearly visible";
        issue.requiredLevel = WcagLevel::AA;
        issue.wcagCriterion = "2.4.7";
        issue.severity = 3;
        issue.isBlocking = false;
        issue.isAutomaticallyFixable = true;
        issue.impactScore = 70.0;
        
        issues.append(issue);
        emit accessibilityIssueFound(issue);
        return false;
    }
    
    return true;
}

bool FluentWcagChecker::testLabelsOrInstructions(QWidget* widget, QList<AccessibilityIssue>& issues) {
    if (!checkFormLabels(widget)) {
        AccessibilityIssue issue;
        issue.widgetName = widget->objectName().isEmpty() ? widget->metaObject()->className() : widget->objectName();
        issue.issueType = AccessibilityIssueType::MissingLabel;
        issue.description = "Form control lacks label or instructions";
        issue.suggestion = "Add label or accessible name";
        issue.requiredLevel = WcagLevel::A;
        issue.wcagCriterion = "3.3.2";
        issue.severity = 4;
        issue.isBlocking = true;
        issue.isAutomaticallyFixable = true;
        issue.impactScore = 85.0;
        
        issues.append(issue);
        emit accessibilityIssueFound(issue);
        return false;
    }
    
    return true;
}

bool FluentWcagChecker::testTargetSize(QWidget* widget, QList<AccessibilityIssue>& issues) {
    if (!checkTargetSize(widget)) {
        AccessibilityIssue issue;
        issue.widgetName = widget->objectName().isEmpty() ? widget->metaObject()->className() : widget->objectName();
        issue.issueType = AccessibilityIssueType::TargetSizeTooSmall;
        issue.description = "Touch target is smaller than 44x44 CSS pixels";
        issue.suggestion = "Increase target size to at least 44x44 pixels";
        issue.requiredLevel = WcagLevel::AA;
        issue.wcagCriterion = "2.5.5";
        issue.severity = 3;
        issue.isBlocking = false;
        issue.isAutomaticallyFixable = true;
        issue.impactScore = 60.0;
        
        issues.append(issue);
        emit accessibilityIssueFound(issue);
        return false;
    }
    
    return true;
}

} // namespace FluentQt::Accessibility
