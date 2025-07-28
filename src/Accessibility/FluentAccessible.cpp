// src/Accessibility/FluentAccessible.cpp
#include "FluentQt/Accessibility/FluentAccessible.h"
#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"

#include <QWidget>
#include <QApplication>
#include <QAccessible>
#include <QAccessibleWidget>
#include <QAccessibleInterface>
#include <QSettings>
#include <QTimer>
#include <QDebug>
#include <memory>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#endif

namespace FluentQt::Accessibility {

// FluentAccessibleInterface Implementation
FluentAccessibleInterface::FluentAccessibleInterface(QWidget* widget)
    : QAccessibleWidget(widget, getFluentRole()) 
{
    // Additional initialization if needed
}

QString FluentAccessibleInterface::text(QAccessible::Text type) const {
    switch (type) {
        case QAccessible::Name:
            if (!object()->objectName().isEmpty()) {
                return object()->objectName();
            }
            return getFluentDescription();
            
        case QAccessible::Description:
            return getFluentDescription();
            
        case QAccessible::Help:
            return help();
            
        case QAccessible::Accelerator:
            if (auto* widget = qobject_cast<QWidget*>(object())) {
                // Extract accelerator from text (e.g., "&Save" -> "Alt+S")
                const QString text = widget->property("text").toString();
                const int ampPos = text.indexOf('&');
                if (ampPos >= 0 && ampPos + 1 < text.length()) {
                    const QChar accel = text.at(ampPos + 1);
                    return QString("Alt+%1").arg(accel.toUpper());
                }
            }
            return QString();
            
        case QAccessible::Value:
            if (auto* widget = qobject_cast<QWidget*>(object())) {
                // For checkable buttons, return checked state
                if (widget->property("checkable").toBool()) {
                    return widget->property("checked").toBool() ? "checked" : "unchecked";
                }
                
                // For progress indicators, return current value
                if (widget->inherits("QProgressBar")) {
                    return widget->property("value").toString();
                }
            }
            return QString();
            
        default:
            return QAccessibleWidget::text(type);
    }
}

void FluentAccessibleInterface::setText(QAccessible::Text type, const QString& text) {
    switch (type) {
        case QAccessible::Name:
            object()->setObjectName(text);
            break;
            
        case QAccessible::Description:
            object()->setProperty("accessibleDescription", text);
            break;
            
        default:
            QAccessibleWidget::setText(type, text);
            break;
    }
}

QAccessible::Role FluentAccessibleInterface::role() const {
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
    const QString helpText = object()->property("accessibleHelp").toString();
    if (!helpText.isEmpty()) {
        return helpText;
    }
    
    // Generate contextual help based on widget type
    if (auto* widget = qobject_cast<QWidget*>(object())) {
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
    
    if (auto* widget = qobject_cast<QWidget*>(object())) {
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
    if (auto* widget = qobject_cast<QWidget*>(object())) {
        if (widget->inherits("FluentButton")) {
            const bool checkable = widget->property("checkable").toBool();
            return checkable ? QAccessible::CheckBox : QAccessible::Button;
        } else if (widget->inherits("FluentCard")) {
            return QAccessible::Grouping;
        } else if (widget->inherits("FluentNavigationView")) {
            return QAccessible::PageTabList;
        } else if (widget->inherits("FluentListView")) {
            return QAccessible::List;
        } else if (widget->inherits("FluentTextBox")) {
            return QAccessible::EditableText;
        } else if (widget->inherits("FluentSlider")) {
            return QAccessible::Slider;
        } else if (widget->inherits("FluentProgressBar")) {
            return QAccessible::ProgressBar;
        }
    }
    
    return QAccessible::Client; // Default fallback
}

QAccessible::State FluentAccessibleInterface::getFluentState() const {
    QAccessible::State state;
    
    if (auto* widget = qobject_cast<QWidget*>(object())) {
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
        if (widget->inherits("QTextEdit") || widget->inherits("QPlainTextEdit")) {
            state.multiLine = true;
        }
        
        // Read-only state
        state.readOnly = widget->property("readOnly").toBool();
        
        // Password field
        if (widget->inherits("QLineEdit")) {
            const auto echoMode = widget->property("echoMode").toInt();
            state.passwordEdit = (echoMode == 2); // QLineEdit::Password
        }
    }
    
    return state;
}

QString FluentAccessibleInterface::getFluentDescription() const {
    QString description;
    
    if (auto* widget = qobject_cast<QWidget*>(object())) {
        // Check for explicit description
        const QString explicitDesc = widget->property("accessibleDescription").toString();
        if (!explicitDesc.isEmpty()) {
            return explicitDesc;
        }
        
        // Generate description based on widget type and properties
        if (widget->inherits("FluentButton")) {
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
            
        } else if (widget->inherits("FluentCard")) {
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
    if (!widget) return;
    
    widget->setObjectName(name);
    widget->setAccessibleName(name);
    
    // Notify screen readers of name change
    if (QAccessibleInterface* iface = QAccessible::queryAccessibleInterface(widget)) {
        QAccessibleEvent event(widget, QAccessible::NameChanged);
        QAccessible::updateAccessibility(&event);
    }
}

void setAccessibleDescription(QWidget* widget, const QString& description) {
    if (!widget) return;
    
    widget->setProperty("accessibleDescription", description);
    widget->setAccessibleDescription(description);
    
    // Notify screen readers
    if (QAccessibleInterface* iface = QAccessible::queryAccessibleInterface(widget)) {
        QAccessibleEvent event(widget, QAccessible::DescriptionChanged);
        QAccessible::updateAccessibility(&event);
    }
}

void setAccessibleRole(QWidget* widget, QAccessible::Role role) {
    if (!widget) return;
    
    widget->setProperty("accessibleRole", static_cast<int>(role));
    
    // Install custom accessible interface if needed
    if (!widget->accessibleName().isEmpty() || !widget->accessibleDescription().isEmpty()) {
        QAccessible::installFactory([](const QString& classname, QObject* object) -> QAccessibleInterface* {
            if (classname.startsWith("FluentQt::") && qobject_cast<QWidget*>(object)) {
                return new FluentAccessibleInterface(qobject_cast<QWidget*>(object));
            }
            return nullptr;
        });
    }
}

void announceToScreenReader(const QString& message) {
    if (message.isEmpty()) return;
    
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
        if (SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0)) {
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
            const double bgLuminance = 0.299 * bg.redF() + 0.587 * bg.greenF() + 0.114 * bg.blueF();
            const double fgLuminance = 0.299 * fg.redF() + 0.587 * fg.greenF() + 0.114 * fg.blueF();
            
            const double contrast = (std::max(bgLuminance, fgLuminance) + 0.05) / 
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
    static QAccessibleInterface* create(const QString& classname, QObject* object) {
        if (!object || !object->isWidgetType()) {
            return nullptr;
        }
        
        QWidget* widget = qobject_cast<QWidget*>(object);
        if (!widget) {
            return nullptr;
        }
        
        // Handle FluentQt components
        if (classname.startsWith("FluentQt::")) {
            return new FluentAccessibleInterface(widget);
        }
        
        return nullptr;
    }
};

// Initialize accessibility support
void initializeAccessibility() {
    static bool initialized = false;
    if (initialized) return;
    
    initialized = true;
    
    // Install the factory
    QAccessible::installFactory(FluentAccessibleFactory::create);
    
    // Enable accessibility if needed
    if (!QAccessible::isActive()) {
        // Check if assistive technology is running
        const bool hasAT = []() {
#ifdef Q_OS_WIN
            // Check for common Windows screen readers
            return FindWindow(L"JAWS", nullptr) != nullptr ||
                   FindWindow(L"NVDAHelperWindow", nullptr) != nullptr;
#endif
#ifdef Q_OS_MAC
            // Check for VoiceOver
            Boolean keyExists = false;
            Boolean voiceOverEnabled = CFPreferencesGetAppBooleanValue(
                CFSTR("voiceOverOnOffKey"),
                CFSTR("com.apple.universalaccess"),
                &keyExists
            );
            return keyExists && voiceOverEnabled;
#endif
            return false;
        }();
        
        if (hasAT) {
            QAccessible::setActive(true);
        }
    }
    
    qDebug() << "FluentQt accessibility initialized - Active:" << QAccessible::isActive();
}

// Auto-initialize accessibility when the module is loaded
Q_CONSTRUCTOR_FUNCTION(initializeAccessibility)

} // namespace FluentQt::Accessibility
