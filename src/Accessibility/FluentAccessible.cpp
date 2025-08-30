// src/Accessibility/FluentAccessible.cpp
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Accessibility/FluentAccessibilityManager.h"
#include "FluentQt/Accessibility/FluentKeyboardNavigationManager.h"
#include "FluentQt/Accessibility/FluentScreenReaderManager.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Core/FluentComponent.h"

#include <QAccessible>
#include <QAccessibleInterface>
#include <QAccessibleWidget>
#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <QWidget>
#include <memory>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#ifdef Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace FluentQt::Accessibility {

// FluentAccessibleInterface Implementation
FluentAccessibleInterface::FluentAccessibleInterface(QWidget* widget)
    : QAccessibleWidget(widget,
                        QAccessible::Client)  // Use safe default role initially
{
    // Validate widget parameter
    if (!widget) {
        qWarning()
            << "FluentAccessibleInterface: Null widget passed to constructor";
        return;
    }

    // Additional initialization if needed
    qDebug() << "FluentAccessibleInterface created for widget:"
             << widget->objectName();
}

// Safe object access helper methods
QObject* FluentAccessibleInterface::safeObject() const {
    QObject* obj = object();
    if (!obj) {
        qWarning()
            << "FluentAccessibleInterface: object() returned null pointer";
        return nullptr;
    }
    return obj;
}

QWidget* FluentAccessibleInterface::safeWidget() const {
    QObject* obj = safeObject();
    if (!obj) {
        return nullptr;
    }

    QWidget* widget = qobject_cast<QWidget*>(obj);
    if (!widget) {
        qWarning() << "FluentAccessibleInterface: object is not a QWidget";
        return nullptr;
    }

    return widget;
}

bool FluentAccessibleInterface::isObjectValid() const {
    return safeObject() != nullptr;
}

QString FluentAccessibleInterface::text(QAccessible::Text type) const {
    // Early return if object is invalid
    if (!isObjectValid()) {
        qWarning() << "FluentAccessibleInterface::text: Invalid object, "
                      "returning empty string";
        return QString();
    }

    switch (type) {
        case QAccessible::Name: {
            QObject* obj = safeObject();
            if (obj && !obj->objectName().isEmpty()) {
                return obj->objectName();
            }

            // For components without explicit names, try to get meaningful text
            QWidget* widget = safeWidget();
            if (widget) {
                const QString className = widget->metaObject()->className();

                // For buttons, use their text property
                if (className.contains("FluentButton")) {
                    // Try direct cast first
                    if (auto* button =
                            qobject_cast<FluentQt::Components::FluentButton*>(
                                widget)) {
                        const QString text = button->text();
                        if (!text.isEmpty()) {
                            return text;
                        }
                    }
                    // Fallback to property system
                    const QString text = widget->property("text").toString();
                    if (!text.isEmpty()) {
                        return text;
                    }
                }

                // For checkboxes, use their text property
                if (className.contains("FluentCheckBox")) {
                    // Try direct cast first
                    if (auto* checkBox =
                            qobject_cast<FluentQt::Components::FluentCheckBox*>(
                                widget)) {
                        const QString text = checkBox->text();
                        if (!text.isEmpty()) {
                            return text;
                        }
                    }
                    // Fallback to property system
                    const QString text = widget->property("text").toString();
                    if (!text.isEmpty()) {
                        return text;
                    }
                }

                // For text inputs, return the placeholder or current text
                if (className.contains("FluentTextInput")) {
                    const QString placeholder =
                        widget->property("placeholderText").toString();
                    if (!placeholder.isEmpty()) {
                        return placeholder;
                    }
                    const QString text = widget->property("text").toString();
                    if (!text.isEmpty()) {
                        return text;
                    }
                }
            }

            return getFluentDescription();
        }

        case QAccessible::Description:
            return getFluentDescription();

        case QAccessible::Help:
            return help();

        case QAccessible::Accelerator: {
            QWidget* widget = safeWidget();
            if (widget) {
                // Extract accelerator from text (e.g., "&Save" -> "Alt+S")
                const QString text = widget->property("text").toString();
                const int ampPos = text.indexOf('&');
                if (ampPos >= 0 && ampPos + 1 < text.length()) {
                    const QChar accel = text.at(ampPos + 1);
                    return QString("Alt+%1").arg(accel.toUpper());
                }
            }
            return QString();
        }

        case QAccessible::Value: {
            QWidget* widget = safeWidget();
            if (widget) {
                // For checkable buttons, return checked state
                if (widget->property("checkable").toBool()) {
                    return widget->property("checked").toBool() ? "checked"
                                                                : "unchecked";
                }

                // For progress indicators, return current value
                if (widget->inherits("QProgressBar")) {
                    return widget->property("value").toString();
                }
            }
            return QString();
        }

        default:
            return QAccessibleWidget::text(type);
    }
}

void FluentAccessibleInterface::setText(QAccessible::Text type,
                                        const QString& text) {
    // Early return if object is invalid
    if (!isObjectValid()) {
        qWarning() << "FluentAccessibleInterface::setText: Invalid object, "
                      "cannot set text";
        return;
    }

    QObject* obj = safeObject();
    if (!obj) {
        return;
    }

    switch (type) {
        case QAccessible::Name:
            obj->setObjectName(text);
            break;

        case QAccessible::Description:
            obj->setProperty("accessibleDescription", text);
            break;

        default:
            QAccessibleWidget::setText(type, text);
            break;
    }
}

QAccessible::Role FluentAccessibleInterface::role() const {
    if (!isObjectValid()) {
        qWarning() << "FluentAccessibleInterface::role: Invalid object, "
                      "returning default role";
        return QAccessible::Client;
    }
    return getFluentRole();
}

QAccessible::State FluentAccessibleInterface::state() const {
    QAccessible::State state = QAccessibleWidget::state();

    // Add Fluent-specific states
    const QAccessible::State fluentState = getFluentState();
    state.checkable = fluentState.checkable;
    state.checked = fluentState.checked;
    state.expanded = fluentState.expanded;
    state.collapsed = fluentState.collapsed;
    state.busy = fluentState.busy;

    return state;
}

QString FluentAccessibleInterface::description() const {
    return getFluentDescription();
}

QString FluentAccessibleInterface::help() const {
    if (!isObjectValid()) {
        qWarning() << "FluentAccessibleInterface::help: Invalid object, "
                      "returning empty help";
        return QString();
    }

    QObject* obj = safeObject();
    if (obj) {
        const QString helpText = obj->property("accessibleHelp").toString();
        if (!helpText.isEmpty()) {
            return helpText;
        }
    }

    // Generate contextual help based on widget type
    QWidget* widget = safeWidget();
    if (widget) {
        if (widget->inherits("FluentButton")) {
            return "Press Space or Enter to activate this button";
        } else if (widget->inherits("FluentCard")) {
            const bool selectable = widget->property("selectable").toBool();
            if (selectable) {
                return "Press Space to select or deselect this card";
            }
            return "Card containing information and controls";
        }
    }

    return QString();
}

QString FluentAccessibleInterface::keyBindingsDescription() const {
    QStringList bindings;

    if (!isObjectValid()) {
        qWarning() << "FluentAccessibleInterface::keyBindingsDescription: "
                      "Invalid object, returning empty bindings";
        return QString();
    }

    QWidget* widget = safeWidget();
    if (widget) {
        // Common key bindings
        if (widget->focusPolicy() & Qt::TabFocus) {
            bindings << "Tab: Move to next element";
            bindings << "Shift+Tab: Move to previous element";
        }

        if (widget->inherits("FluentButton")) {
            bindings << "Space/Enter: Activate button";

            if (widget->property("checkable").toBool()) {
                bindings << "Space: Toggle button state";
            }
        } else if (widget->inherits("FluentCard")) {
            const bool selectable = widget->property("selectable").toBool();
            const bool expandable = widget->property("expandable").toBool();

            if (selectable) {
                bindings << "Space: Select/deselect card";
            }

            if (expandable) {
                bindings << "Enter: Expand/collapse card";
            }
        }

        // Extract accelerator keys
        const QString text = widget->property("text").toString();
        const int ampPos = text.indexOf('&');
        if (ampPos >= 0 && ampPos + 1 < text.length()) {
            const QChar accel = text.at(ampPos + 1);
            bindings << QString("Alt+%1: Activate").arg(accel.toUpper());
        }
    }

    return bindings.join("; ");
}

QAccessible::Role FluentAccessibleInterface::getFluentRole() const {
    if (!isObjectValid()) {
        qWarning() << "FluentAccessibleInterface::getFluentRole: Invalid "
                      "object, returning default role";
        return QAccessible::Client;
    }

    QWidget* widget = safeWidget();
    if (widget) {
        const QString className = widget->metaObject()->className();
        qDebug() << "FluentAccessibleInterface::getFluentRole: className ="
                 << className;

        if (className.contains("FluentButton")) {
            const bool checkable = widget->property("checkable").toBool();
            return checkable ? QAccessible::CheckBox : QAccessible::PushButton;
        } else if (className.contains("FluentCard")) {
            return QAccessible::Grouping;
        } else if (className.contains("FluentNavigationView")) {
            return QAccessible::PageTabList;
        } else if (className.contains("FluentListView")) {
            return QAccessible::List;
        } else if (className.contains("FluentTextInput") ||
                   className.contains("FluentTextBox")) {
            return QAccessible::EditableText;
        } else if (className.contains("FluentSlider")) {
            return QAccessible::Slider;
        } else if (className.contains("FluentProgressBar")) {
            return QAccessible::ProgressBar;
        } else if (className.contains("FluentCheckBox")) {
            return QAccessible::CheckBox;
        } else if (className.contains("FluentRadioButton")) {
            return QAccessible::RadioButton;
        } else if (className.contains("FluentComboBox")) {
            return QAccessible::ComboBox;
        }
    }

    return QAccessible::Client;  // Default fallback
}

QAccessible::State FluentAccessibleInterface::getFluentState() const {
    QAccessible::State state;

    if (!isObjectValid()) {
        qWarning() << "FluentAccessibleInterface::getFluentState: Invalid "
                      "object, returning default state";
        return state;
    }

    QWidget* widget = safeWidget();
    if (widget) {
        // Checkable state
        state.checkable = widget->property("checkable").toBool();
        state.checked = widget->property("checked").toBool();

        // Loading state
        state.busy = widget->property("loading").toBool();

        // Expansion state
        if (widget->property("expandable").toBool()) {
            const bool expanded = widget->property("expanded").toBool();
            state.expandable = true;
            state.expanded = expanded;
            state.collapsed = !expanded;
        }

        // Selection state
        if (widget->property("selectable").toBool()) {
            state.selectable = true;
            state.selected = widget->property("selected").toBool();
        }

        // Multi-line text
        if (widget->inherits("QTextEdit") ||
            widget->inherits("QPlainTextEdit")) {
            state.multiLine = true;
        }

        // Read-only state
        state.readOnly = widget->property("readOnly").toBool();

        // Password field
        if (widget->inherits("QLineEdit")) {
            const auto echoMode = widget->property("echoMode").toInt();
            state.passwordEdit = (echoMode == 2);  // QLineEdit::Password
        }
    }

    return state;
}

QString FluentAccessibleInterface::getFluentDescription() const {
    QString description;

    if (!isObjectValid()) {
        qWarning() << "FluentAccessibleInterface::getFluentDescription: "
                      "Invalid object, returning default description";
        return "UI Element";
    }

    QWidget* widget = safeWidget();
    if (widget) {
        // Check for explicit description
        const QString explicitDesc =
            widget->property("accessibleDescription").toString();
        if (!explicitDesc.isEmpty()) {
            return explicitDesc;
        }

        // Generate description based on widget type and properties
        const QString className = widget->metaObject()->className();

        if (className.contains("FluentButton")) {
            const QString text = widget->property("text").toString();
            const bool loading = widget->property("loading").toBool();
            const bool checkable = widget->property("checkable").toBool();
            const bool checked = widget->property("checked").toBool();

            if (!text.isEmpty()) {
                description = text;
            } else {
                description = "Button";
            }

            if (loading) {
                description += " (loading)";
            } else if (checkable) {
                description += checked ? " (checked)" : " (unchecked)";
            }

        } else if (className.contains("FluentCard")) {
            const QString title = widget->property("title").toString();
            const QString subtitle = widget->property("subtitle").toString();
            const bool selected = widget->property("selected").toBool();
            const bool expanded = widget->property("expanded").toBool();

            if (!title.isEmpty()) {
                description = title;
                if (!subtitle.isEmpty()) {
                    description += " - " + subtitle;
                }
            } else {
                description = "Card";
            }

            if (selected) {
                description += " (selected)";
            }

            if (widget->property("expandable").toBool()) {
                description += expanded ? " (expanded)" : " (collapsed)";
            }
        }
    }

    return description.isEmpty() ? "UI Element" : description;
}

// Global accessibility helper functions
void setAccessibleName(QWidget* widget, const QString& name) {
    if (!widget)
        return;

    widget->setObjectName(name);
    widget->setAccessibleName(name);

    // Notify screen readers of name change
    if (QAccessible::queryAccessibleInterface(widget)) {
        QAccessibleEvent event(widget, QAccessible::NameChanged);
        QAccessible::updateAccessibility(&event);
    }
}

void setAccessibleDescription(QWidget* widget, const QString& description) {
    if (!widget)
        return;

    widget->setProperty("accessibleDescription", description);
    widget->setAccessibleDescription(description);

    // Notify screen readers
    if (QAccessible::queryAccessibleInterface(widget)) {
        QAccessibleEvent event(widget, QAccessible::DescriptionChanged);
        QAccessible::updateAccessibility(&event);
    }
}

void setAccessibleRole(QWidget* widget, QAccessible::Role role) {
    if (!widget)
        return;

    widget->setProperty("accessibleRole", static_cast<int>(role));

    // Install custom accessible interface if needed
    if (!widget->accessibleName().isEmpty() ||
        !widget->accessibleDescription().isEmpty()) {
        QAccessible::installFactory(
            [](const QString& classname,
               QObject* object) -> QAccessibleInterface* {
                if ((classname.startsWith("FluentQt::") ||
                     classname.contains("Fluent")) &&
                    qobject_cast<QWidget*>(object)) {
                    return new FluentAccessibleInterface(
                        qobject_cast<QWidget*>(object));
                }
                return nullptr;
            });
    }
}

void announceToScreenReader(const QString& message) {
    if (message.isEmpty())
        return;

    // Create a temporary widget for the announcement
    static QWidget* announceWidget = nullptr;
    if (!announceWidget) {
        announceWidget = new QWidget();
        announceWidget->setObjectName("FluentAnnouncer");
        announceWidget->hide();
    }

    announceWidget->setAccessibleName(message);

    // Fire accessibility event
    QAccessibleEvent event(announceWidget, QAccessible::Alert);
    QAccessible::updateAccessibility(&event);

    // Also use platform-specific APIs if available
#ifdef Q_OS_WIN
    // Use Windows SAPI for immediate announcement
    QTimer::singleShot(0, [message]() {
        // This would integrate with Windows Speech API
        // Implementation would require linking to SAPI
        qDebug() << "Screen reader announcement:" << message;
    });
#endif

#ifdef Q_OS_MAC
    // Use macOS VoiceOver APIs
    QTimer::singleShot(0, [message]() {
        // This would use NSAccessibility APIs
        qDebug() << "VoiceOver announcement:" << message;
    });
#endif

    // Fallback: Log for debugging
    qDebug() << "Accessibility announcement:" << message;
}

bool isHighContrastMode() {
    static bool checked = false;
    static bool highContrast = false;

    if (!checked) {
        checked = true;

#ifdef Q_OS_WIN
        // Check Windows high contrast mode
        HIGHCONTRAST hc = {};
        hc.cbSize = sizeof(HIGHCONTRAST);
        if (SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc,
                                 0)) {
            highContrast = (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
        }
#endif

#ifdef Q_OS_MAC
        // Check macOS accessibility settings
        // This would require proper macOS accessibility API integration
        // For now, we'll use a simple heuristic
        const QString theme = qgetenv("APPEARANCE_THEME");
        highContrast = theme.contains("contrast", Qt::CaseInsensitive);
#endif

        // Cross-platform fallback: check application palette
        if (!highContrast) {
            const QPalette palette = QApplication::palette();
            const QColor bg = palette.color(QPalette::Window);
            const QColor fg = palette.color(QPalette::WindowText);

            // Calculate contrast ratio
            const double bgLuminance =
                0.299 * bg.redF() + 0.587 * bg.greenF() + 0.114 * bg.blueF();
            const double fgLuminance =
                0.299 * fg.redF() + 0.587 * fg.greenF() + 0.114 * fg.blueF();

            const double contrast =
                (std::max(bgLuminance, fgLuminance) + 0.05) /
                (std::min(bgLuminance, fgLuminance) + 0.05);

            // WCAG AA standard requires 4.5:1 for normal text
            // High contrast mode typically exceeds 7:1
            highContrast = contrast > 7.0;
        }
    }

    return highContrast;
}

void updateForHighContrast(QWidget* widget) {
    if (!widget || !isHighContrastMode()) {
        return;
    }

    // Apply high contrast styling
    const QString highContrastStyle = R"(
        QWidget {
            background-color: window;
            color: windowtext;
            border: 1px solid windowtext;
        }

        QPushButton {
            background-color: button;
            color: buttontext;
            border: 2px solid buttontext;
            padding: 4px 8px;
        }

        QPushButton:hover {
            background-color: highlight;
            color: highlightedtext;
        }

        QPushButton:pressed {
            background-color: buttontext;
            color: button;
        }

        QPushButton:disabled {
            background-color: button;
            color: graytext;
            border-color: graytext;
        }

        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: base;
            color: text;
            border: 2px solid windowtext;
            selection-background-color: highlight;
            selection-color: highlightedtext;
        }

        QLabel {
            color: windowtext;
            background-color: transparent;
        }

        QGroupBox {
            border: 2px solid windowtext;
            color: windowtext;
            font-weight: bold;
        }

        QScrollBar {
            background-color: button;
            border: 1px solid windowtext;
        }

        QScrollBar::handle {
            background-color: buttontext;
            border: 1px solid windowtext;
        }

        QScrollBar::handle:hover {
            background-color: highlight;
        }
    )";

    widget->setStyleSheet(widget->styleSheet() + highContrastStyle);

    // Also update child widgets
    const auto children = widget->findChildren<QWidget*>();
    for (QWidget* child : children) {
        child->setStyleSheet(child->styleSheet() + highContrastStyle);
    }

    // Announce the change
    announceToScreenReader("High contrast mode applied");
}

// Accessibility factory for FluentQt components
class FluentAccessibleFactory {
public:
    static QAccessibleInterface* create(const QString& classname,
                                        QObject* object) {
        // Enhanced validation
        if (!object) {
            return nullptr;  // Don't log for null objects as this is common
        }

        // Skip non-widget objects silently (like QApplication)
        if (!object->isWidgetType()) {
            return nullptr;
        }

        QWidget* widget = qobject_cast<QWidget*>(object);
        if (!widget) {
            return nullptr;
        }

        // Additional safety check - ensure widget is not being destroyed
        if (widget->testAttribute(Qt::WA_DeleteOnClose) &&
            !widget->isVisible()) {
            qDebug() << "FluentAccessibleFactory: Widget appears to be in "
                        "destruction process";
            return nullptr;
        }

        // Handle FluentQt components only
        if (classname.startsWith("FluentQt::")) {
            try {
                return new FluentAccessibleInterface(widget);
            } catch (const std::exception& e) {
                qCritical() << "FluentAccessibleFactory: Exception creating "
                               "accessibility interface:"
                            << e.what();
                return nullptr;
            } catch (...) {
                qCritical() << "FluentAccessibleFactory: Unknown exception "
                               "creating accessibility interface";
                return nullptr;
            }
        }

        return nullptr;
    }
};

// Initialize enhanced accessibility support
void initializeAccessibility() {
    static bool initialized = false;
    if (initialized)
        return;

    initialized = true;

    // Check if we're running in offscreen mode or other headless environments
    // In these cases, skip full accessibility initialization to avoid deadlocks
    if (QApplication::instance()) {
        // Check for offscreen platform via environment variable or command line
        QString platformName = qgetenv("QT_QPA_PLATFORM");
        if (platformName.isEmpty()) {
            // Try to get from application arguments
            QStringList args = QApplication::arguments();
            for (int i = 0; i < args.size() - 1; ++i) {
                if (args[i] == "-platform") {
                    platformName = args[i + 1];
                    break;
                }
            }
        }

        if (platformName == "offscreen" || platformName == "minimal" ||
            qEnvironmentVariableIsSet("FLUENTQT_TEST_MODE") ||
            qEnvironmentVariableIsSet("FLUENTQT_SKIP_PROCESS_DETECTION")) {
            qDebug()
                << "Skipping full accessibility initialization for platform:"
                << platformName;
            // Only install the factory for basic accessibility support
            QAccessible::installFactory(FluentAccessibleFactory::create);
            QAccessible::setActive(true);
            qDebug() << "Basic FluentQt accessibility initialized for"
                     << platformName << "platform";
            return;
        }
    }

    // Install the factory
    QAccessible::installFactory(FluentAccessibleFactory::create);

    // Always enable accessibility for FluentQt components
    // This ensures accessibility works even without assistive technology
    QAccessible::setActive(true);

    // Initialize enhanced accessibility managers
    auto& accessibilityManager =
        FluentQt::Accessibility::FluentAccessibilityManager::instance();
    // Note: detectSystemAccessibilitySettings() is already called in
    // constructor and applySystemSettings() calls it again, so no need to call
    // it here explicitly
    Q_UNUSED(accessibilityManager);

    qDebug() << "FluentQt accessibility initialized";

    // Initialize screen reader support asynchronously to avoid blocking startup
    auto& screenReaderManager =
        FluentQt::Accessibility::FluentScreenReaderManager::instance();
    // Defer screen reader detection to avoid blocking UI thread
    QTimer::singleShot(200, [&screenReaderManager]() {
        screenReaderManager.detectScreenReader();
    });

    // Initialize keyboard navigation
    auto& keyboardManager =
        FluentQt::Accessibility::FluentKeyboardNavigationManager::instance();
    FluentQt::Accessibility::FluentKeyboardConfig config;
    config.enableArrowNavigation = true;
    config.enableSkipLinks = true;
    config.enableFocusTrapping = true;
    keyboardManager.setNavigationConfig(config);

    qDebug() << "Enhanced FluentQt accessibility initialized";

    // Additional check for assistive technology (deferred to avoid blocking)
    QTimer::singleShot(500, []() {
        const bool hasAT = []() {
#ifdef Q_OS_WIN
            // Check for common Windows screen readers
            return FindWindow(L"JAWS", nullptr) != nullptr ||
                   FindWindow(L"NVDAHelperWindow", nullptr) != nullptr ||
                   FindWindow(L"NVDA_controllerClient32", nullptr) != nullptr;
#endif
#ifdef Q_OS_MAC
            // Check for VoiceOver
            Boolean keyExists = false;
            Boolean voiceOverEnabled = CFPreferencesGetAppBooleanValue(
                CFSTR("voiceOverOnOffKey"), CFSTR("com.apple.universalaccess"),
                &keyExists);
            return keyExists && voiceOverEnabled;
#endif
            return false;
        }();

        if (hasAT) {
            qDebug() << "Assistive technology detected";
        }
    });

    qDebug() << "FluentQt accessibility initialized - Active:"
             << QAccessible::isActive();
}

// Safe initialization function that can be called explicitly
bool initializeAccessibilitySafe(bool forceFullInit) {
    static bool initialized = false;
    if (initialized)
        return true;

    try {
        // Check platform first
        bool isOffscreenMode = false;
        if (QApplication::instance() && !forceFullInit) {
            QString platformName = qgetenv("QT_QPA_PLATFORM");
            if (platformName.isEmpty()) {
                QStringList args = QApplication::arguments();
                for (int i = 0; i < args.size() - 1; ++i) {
                    if (args[i] == "-platform") {
                        platformName = args[i + 1];
                        break;
                    }
                }
            }
            isOffscreenMode =
                (platformName == "offscreen" || platformName == "minimal");
        }

        if (isOffscreenMode) {
            qDebug() << "Safe accessibility initialization: Basic mode for "
                        "offscreen platform";
            QAccessible::installFactory(FluentAccessibleFactory::create);
            QAccessible::setActive(true);
            initialized = true;
            return true;
        }

        // Full initialization for normal platforms
        initializeAccessibility();
        initialized = true;
        return true;
    } catch (...) {
        qWarning() << "Failed to initialize accessibility system safely";
        return false;
    }
}

// Auto-initialize accessibility when the module is loaded
Q_CONSTRUCTOR_FUNCTION(initializeAccessibility)

}  // namespace FluentQt::Accessibility
