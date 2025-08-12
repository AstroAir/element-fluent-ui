// src/Accessibility/FluentAccessibilityManagerEnhanced.cpp
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGroupBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMutexLocker>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSlider>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QWidget>
#include <algorithm>
#include <cmath>
#include "FluentQt/Accessibility/FluentAccessibilityManager.h"
#include "FluentQt/Accessibility/FluentScreenReaderManager.h"
#include "FluentQt/Accessibility/FluentWcagChecker.h"

namespace FluentQt::Accessibility {

// Enhanced WCAG 2.1 validation implementation
QList<AccessibilityIssue> FluentAccessibilityManager::validateWcag21Compliance(
    QWidget* rootWidget) {
    QList<AccessibilityIssue> issues;

    if (!rootWidget) {
        return issues;
    }

    // Validate all four WCAG 2.1 principles
    auto perceivableIssues = validatePerceivable(rootWidget);
    auto operableIssues = validateOperable(rootWidget);
    auto understandableIssues = validateUnderstandable(rootWidget);
    auto robustIssues = validateRobust(rootWidget);

    issues.append(perceivableIssues);
    issues.append(operableIssues);
    issues.append(understandableIssues);
    issues.append(robustIssues);

    // Sort by severity and impact
    std::sort(issues.begin(), issues.end(),
              [](const AccessibilityIssue& a, const AccessibilityIssue& b) {
                  if (a.severity != b.severity) {
                      return a.severity > b.severity;  // Higher severity first
                  }
                  return a.impactScore > b.impactScore;  // Higher impact first
              });

    emit wcag21ValidationCompleted(issues);
    return issues;
}

QList<AccessibilityIssue> FluentAccessibilityManager::validatePerceivable(
    QWidget* widget) {
    QList<AccessibilityIssue> issues;

    validatePerceivableInternal(widget, issues);

    // Recursively check child widgets
    for (QWidget* child : widget->findChildren<QWidget*>()) {
        if (child->isVisible()) {
            validatePerceivableInternal(child, issues);
        }
    }

    return issues;
}

QList<AccessibilityIssue> FluentAccessibilityManager::validateOperable(
    QWidget* widget) {
    QList<AccessibilityIssue> issues;

    validateOperableInternal(widget, issues);

    // Recursively check child widgets
    for (QWidget* child : widget->findChildren<QWidget*>()) {
        if (child->isVisible()) {
            validateOperableInternal(child, issues);
        }
    }

    return issues;
}

QList<AccessibilityIssue> FluentAccessibilityManager::validateUnderstandable(
    QWidget* widget) {
    QList<AccessibilityIssue> issues;

    validateUnderstandableInternal(widget, issues);

    // Recursively check child widgets
    for (QWidget* child : widget->findChildren<QWidget*>()) {
        if (child->isVisible()) {
            validateUnderstandableInternal(child, issues);
        }
    }

    return issues;
}

QList<AccessibilityIssue> FluentAccessibilityManager::validateRobust(
    QWidget* widget) {
    QList<AccessibilityIssue> issues;

    validateRobustInternal(widget, issues);

    // Recursively check child widgets
    for (QWidget* child : widget->findChildren<QWidget*>()) {
        if (child->isVisible()) {
            validateRobustInternal(child, issues);
        }
    }

    return issues;
}

// Advanced contrast and color checking
double FluentAccessibilityManager::calculateContrastRatio(
    const QColor& foreground, const QColor& background) {
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

bool FluentAccessibilityManager::meetsContrastRequirements(const QColor& fg,
                                                           const QColor& bg,
                                                           WcagLevel level,
                                                           bool isLargeText) {
    double ratio = calculateContrastRatio(fg, bg);

    switch (level) {
        case WcagLevel::A:
            return true;  // No contrast requirements for Level A
        case WcagLevel::AA:
            return isLargeText ? ratio >= 3.0 : ratio >= 4.5;
        case WcagLevel::AAA:
            return isLargeText ? ratio >= 4.5 : ratio >= 7.0;
    }

    return false;
}

QColor FluentAccessibilityManager::suggestBetterContrast(
    const QColor& foreground, const QColor& background, WcagLevel level) {
    double targetRatio = (level == WcagLevel::AAA) ? 7.0 : 4.5;

    // Try darkening or lightening the foreground color
    QColor suggested = foreground;
    double currentRatio = calculateContrastRatio(foreground, background);

    if (currentRatio < targetRatio) {
        // Try making foreground darker
        for (int factor = 10; factor <= 100; factor += 10) {
            QColor darker = foreground.darker(100 + factor);
            if (calculateContrastRatio(darker, background) >= targetRatio) {
                suggested = darker;
                break;
            }
        }

        // If darkening didn't work, try lightening
        if (calculateContrastRatio(suggested, background) < targetRatio) {
            for (int factor = 10; factor <= 100; factor += 10) {
                QColor lighter = foreground.lighter(100 + factor);
                if (calculateContrastRatio(lighter, background) >=
                    targetRatio) {
                    suggested = lighter;
                    break;
                }
            }
        }
    }

    return suggested;
}

QList<AccessibilityIssue>
FluentAccessibilityManager::validateColorAccessibility(QWidget* widget) {
    QList<AccessibilityIssue> issues;

    if (!widget || !widget->isVisible()) {
        return issues;
    }

    // Get widget colors
    QPalette palette = widget->palette();
    QColor foreground = palette.color(QPalette::WindowText);
    QColor background = palette.color(QPalette::Window);

    // Check contrast ratio
    double ratio = calculateContrastRatio(foreground, background);
    bool meetsAA =
        meetsContrastRequirements(foreground, background, WcagLevel::AA);

    if (!meetsAA) {
        AccessibilityIssue issue;
        issue.widgetName = widget->objectName().isEmpty()
                               ? widget->metaObject()->className()
                               : widget->objectName();
        issue.issueType = AccessibilityIssueType::InsufficientColorContrast;
        issue.description = QString(
                                "Color contrast ratio %1:1 does not meet WCAG "
                                "AA requirements (4.5:1)")
                                .arg(ratio, 0, 'f', 2);
        issue.suggestion = QString(
                               "Increase contrast ratio to at least 4.5:1. "
                               "Suggested color: %1")
                               .arg(suggestBetterContrast(
                                        foreground, background, WcagLevel::AA)
                                        .name());
        issue.requiredLevel = WcagLevel::AA;
        issue.wcagCriterion = "1.4.3";
        issue.wcagTechnique = "G18";
        issue.severity = 4;
        issue.isBlocking = true;
        issue.isAutomaticallyFixable = true;
        issue.fixAction = "Adjust foreground or background color";
        issue.affectedUsers = {"Users with low vision",
                               "Users with color blindness"};
        issue.impactScore = 85.0;

        issues.append(issue);
        emit contrastIssueDetected(widget, ratio, 4.5);
    }

    return issues;
}

// Keyboard navigation validation
QList<AccessibilityIssue>
FluentAccessibilityManager::validateKeyboardNavigation(QWidget* rootWidget) {
    QList<AccessibilityIssue> issues;

    if (!rootWidget) {
        return issues;
    }

    // Find all focusable widgets
    QList<QWidget*> focusableWidgets;
    std::function<void(QWidget*)> findFocusable = [&](QWidget* widget) {
        if (widget->focusPolicy() != Qt::NoFocus && widget->isVisible() &&
            widget->isEnabled()) {
            focusableWidgets.append(widget);
        }
        for (QWidget* child : widget->findChildren<QWidget*>(
                 QString(), Qt::FindDirectChildrenOnly)) {
            findFocusable(child);
        }
    };

    findFocusable(rootWidget);

    // Check if widgets are keyboard accessible
    for (QWidget* widget : focusableWidgets) {
        if (!isKeyboardAccessible(widget)) {
            AccessibilityIssue issue;
            issue.widgetName = widget->objectName().isEmpty()
                                   ? widget->metaObject()->className()
                                   : widget->objectName();
            issue.issueType = AccessibilityIssueType::NoKeyboardAccess;
            issue.description =
                "Widget is not accessible via keyboard navigation";
            issue.suggestion =
                "Ensure widget can receive focus and respond to keyboard input";
            issue.requiredLevel = WcagLevel::A;
            issue.wcagCriterion = "2.1.1";
            issue.wcagTechnique = "G202";
            issue.severity = 5;
            issue.isBlocking = true;
            issue.isAutomaticallyFixable = true;
            issue.fixAction = "Set appropriate focus policy and tab order";
            issue.affectedUsers = {"Keyboard users", "Screen reader users",
                                   "Users with motor disabilities"};
            issue.impactScore = 90.0;

            issues.append(issue);
        }
    }

    // Check for keyboard traps
    auto traps = findKeyboardTraps(rootWidget);
    for (QWidget* trap : traps) {
        AccessibilityIssue issue;
        issue.widgetName = trap->objectName().isEmpty()
                               ? trap->metaObject()->className()
                               : trap->objectName();
        issue.issueType = AccessibilityIssueType::KeyboardTrap;
        issue.description = "Keyboard focus is trapped in this widget";
        issue.suggestion =
            "Ensure users can navigate away from this widget using standard "
            "keyboard commands";
        issue.requiredLevel = WcagLevel::A;
        issue.wcagCriterion = "2.1.2";
        issue.wcagTechnique = "G21";
        issue.severity = 5;
        issue.isBlocking = true;
        issue.isAutomaticallyFixable = false;
        issue.fixAction = "Review focus management and escape mechanisms";
        issue.affectedUsers = {"Keyboard users", "Screen reader users"};
        issue.impactScore = 95.0;

        issues.append(issue);
        emit keyboardTrapDetected(trap);
    }

    // Validate tab order
    validateTabOrder(rootWidget, issues);

    return issues;
}

bool FluentAccessibilityManager::isKeyboardAccessible(QWidget* widget) {
    if (!widget) {
        return false;
    }

    // Check if widget can receive focus
    if (widget->focusPolicy() == Qt::NoFocus) {
        return false;
    }

    // Check if widget is visible and enabled
    if (!widget->isVisible() || !widget->isEnabled()) {
        return false;
    }

    // Check if widget responds to keyboard events
    // This is a simplified check - in practice, you'd want to test actual key
    // events
    return widget->focusPolicy() != Qt::NoFocus;
}

QList<QWidget*> FluentAccessibilityManager::findKeyboardTraps(
    QWidget* rootWidget) {
    QList<QWidget*> traps;

    if (!rootWidget) {
        return traps;
    }

    // This is a simplified implementation
    // In practice, you'd need to simulate keyboard navigation to detect traps

    // Check for widgets that might trap focus (like modal dialogs without
    // proper escape)
    auto modalWidgets = rootWidget->findChildren<QWidget*>();
    for (QWidget* widget : modalWidgets) {
        if (widget->isModal() && widget->isVisible()) {
            // Check if there's a way to escape (Escape key, close button, etc.)
            bool hasEscape = false;

            // Look for close buttons or escape mechanisms
            auto buttons = widget->findChildren<QPushButton*>();
            for (QPushButton* button : buttons) {
                if (button->text().contains("Close", Qt::CaseInsensitive) ||
                    button->text().contains("Cancel", Qt::CaseInsensitive) ||
                    button->text().contains("OK", Qt::CaseInsensitive)) {
                    hasEscape = true;
                    break;
                }
            }

            if (!hasEscape) {
                traps.append(widget);
            }
        }
    }

    return traps;
}

void FluentAccessibilityManager::validateTabOrder(
    QWidget* rootWidget, QList<AccessibilityIssue>& issues) {
    if (!rootWidget) {
        return;
    }

    // Get all focusable widgets in tab order
    QList<QWidget*> tabOrder;
    QWidget* current = rootWidget;

    while (current) {
        if (current->focusPolicy() != Qt::NoFocus && current->isVisible() &&
            current->isEnabled()) {
            tabOrder.append(current);
        }
        current = current->nextInFocusChain();

        // Prevent infinite loops
        if (current == rootWidget) {
            break;
        }
    }

    // Check for logical tab order issues
    for (int i = 0; i < tabOrder.size() - 1; ++i) {
        QWidget* current = tabOrder[i];
        QWidget* next = tabOrder[i + 1];

        // Check if the spatial order makes sense
        QRect currentRect = current->geometry();
        QRect nextRect = next->geometry();

        // Simple heuristic: next widget should be to the right or below current
        // widget
        bool logicalOrder = (nextRect.x() >= currentRect.x()) ||
                            (nextRect.y() > currentRect.bottom());

        if (!logicalOrder) {
            AccessibilityIssue issue;
            issue.widgetName = current->objectName().isEmpty()
                                   ? current->metaObject()->className()
                                   : current->objectName();
            issue.issueType = AccessibilityIssueType::InvalidTabOrder;
            issue.description =
                "Tab order does not follow logical reading sequence";
            issue.suggestion =
                "Adjust tab order to follow visual layout (left-to-right, "
                "top-to-bottom)";
            issue.requiredLevel = WcagLevel::A;
            issue.wcagCriterion = "2.4.3";
            issue.wcagTechnique = "G59";
            issue.severity = 3;
            issue.isBlocking = false;
            issue.isAutomaticallyFixable = true;
            issue.fixAction = "Reorder widgets or set explicit tab order";
            issue.affectedUsers = {"Keyboard users", "Screen reader users"};
            issue.impactScore = 60.0;

            issues.append(issue);
        }
    }
}

// Screen reader compatibility validation
QList<AccessibilityIssue>
FluentAccessibilityManager::validateScreenReaderCompatibility(QWidget* widget) {
    QList<AccessibilityIssue> issues;

    if (!widget || !widget->isVisible()) {
        return issues;
    }

    // Check for proper ARIA labels
    if (!hasProperAriaLabels(widget)) {
        AccessibilityIssue issue;
        issue.widgetName = widget->objectName().isEmpty()
                               ? widget->metaObject()->className()
                               : widget->objectName();
        issue.issueType = AccessibilityIssueType::MissingLabel;
        issue.description =
            "Widget lacks proper ARIA labels for screen readers";
        issue.suggestion =
            "Add aria-label, aria-labelledby, or aria-describedby attributes";
        issue.requiredLevel = WcagLevel::A;
        issue.wcagCriterion = "4.1.2";
        issue.wcagTechnique = "ARIA6";
        issue.severity = 4;
        issue.isBlocking = true;
        issue.isAutomaticallyFixable = true;
        issue.fixAction = "Add appropriate ARIA labels";
        issue.affectedUsers = {"Screen reader users",
                               "Users with cognitive disabilities"};
        issue.impactScore = 80.0;

        issues.append(issue);
        emit screenReaderIssueDetected(widget, "Missing ARIA labels");
    }

    return issues;
}

bool FluentAccessibilityManager::hasProperAriaLabels(QWidget* widget) {
    if (!widget) {
        return false;
    }

    // Check if widget has accessible name
    QString accessibleName = widget->accessibleName();
    if (!accessibleName.isEmpty()) {
        return true;
    }

    // Check if widget has accessible description
    QString accessibleDescription = widget->accessibleDescription();
    if (!accessibleDescription.isEmpty()) {
        return true;
    }

    // For certain widget types, check if they have associated labels
    if (qobject_cast<QLineEdit*>(widget) || qobject_cast<QTextEdit*>(widget) ||
        qobject_cast<QComboBox*>(widget) || qobject_cast<QSlider*>(widget)) {
        // Look for associated label
        QWidget* parent = widget->parentWidget();
        if (parent) {
            auto labels = parent->findChildren<QLabel*>();
            for (QLabel* label : labels) {
                if (label->buddy() == widget) {
                    return true;
                }
            }
        }

        return false;  // Form controls should have labels
    }

    // For buttons, check if they have text or icons with alt text
    if (auto* button = qobject_cast<QPushButton*>(widget)) {
        return !button->text().isEmpty() || !button->accessibleName().isEmpty();
    }

    return true;  // Other widgets may not need explicit labels
}

}  // namespace FluentQt::Accessibility
