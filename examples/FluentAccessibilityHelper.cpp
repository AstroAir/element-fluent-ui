// examples/FluentAccessibilityHelper.cpp
#include "FluentAccessibilityHelper.h"
#include <QAccessible>
#include <QApplication>
#include <QDebug>
#include <QFocusFrame>
#include <QHash>
#include <QKeyEvent>
#include <QScrollArea>
#include <QSettings>
#include <QStyleHints>
#include <QTimer>
#include <QWidget>
#include <algorithm>

namespace FluentQt::Examples {

// Static member definitions
QHash<QString, QWidget*> FluentAccessibilityHelper::s_liveRegions;
QTimer* FluentAccessibilityHelper::s_announcementTimer = nullptr;
QString FluentAccessibilityHelper::s_pendingAnnouncement;
bool FluentAccessibilityHelper::s_highContrastMode = false;

FluentAccessibilityHelper::FluentAccessibilityHelper(QObject* parent)
    : QObject(parent), m_focusTimer(new QTimer(this)) {
    // Initialize announcement timer
    if (!s_announcementTimer) {
        s_announcementTimer = new QTimer();
        s_announcementTimer->setSingleShot(true);
        s_announcementTimer->setInterval(100);  // Debounce announcements
    }

    // Setup focus tracking
    m_focusTimer->setSingleShot(true);
    m_focusTimer->setInterval(50);
    connect(m_focusTimer, &QTimer::timeout, this, [this]() {
        if (m_lastFocusedWidget) {
            announceFocusChange(m_lastFocusedWidget);
        }
    });

    // Connect to application focus changes
    connect(qApp, &QApplication::focusChanged, this,
            &FluentAccessibilityHelper::onFocusChanged);

    // Detect high contrast mode
    s_highContrastMode = isHighContrastMode();
}

FluentAccessibilityHelper::~FluentAccessibilityHelper() = default;

void FluentAccessibilityHelper::announceToScreenReader(
    const QString& message, QAccessible::Event event) {
    if (message.isEmpty())
        return;

    // Debounce announcements to avoid spam
    s_pendingAnnouncement = message;

    if (!s_announcementTimer->isActive()) {
        s_announcementTimer->start();
        QTimer::singleShot(100, []() {
            if (!s_pendingAnnouncement.isEmpty()) {
                // Create a temporary widget for the announcement
                auto* tempWidget = new QWidget();
                tempWidget->setAccessibleName(s_pendingAnnouncement);
                tempWidget->setAccessibleDescription(s_pendingAnnouncement);

                QAccessibleEvent accessibleEvent(tempWidget,
                                                 QAccessible::Alert);
                QAccessible::updateAccessibility(&accessibleEvent);

                // Clean up
                tempWidget->deleteLater();
                s_pendingAnnouncement.clear();

                qDebug() << "Accessibility announcement:"
                         << s_pendingAnnouncement;
            }
        });
    }
}

void FluentAccessibilityHelper::announceThemeChange(const QString& newTheme) {
    QString message = QString("Theme changed to %1 mode").arg(newTheme);
    announceToScreenReader(message, QAccessible::Alert);
}

void FluentAccessibilityHelper::announceComponentStateChange(
    QWidget* widget, const QString& state) {
    if (!widget)
        return;

    QString widgetName = widget->accessibleName();
    if (widgetName.isEmpty()) {
        widgetName = widget->objectName();
    }
    if (widgetName.isEmpty()) {
        widgetName = "Component";
    }

    QString message = QString("%1 %2").arg(widgetName, state);
    announceToScreenReader(message, QAccessible::StateChanged);
}

void FluentAccessibilityHelper::enhanceWidgetAccessibility(
    QWidget* widget, const QString& role) {
    if (!widget)
        return;

    // Set basic accessible properties
    if (!role.isEmpty()) {
        widget->setProperty("accessibleRole", role);
    }

    // Install event filter for keyboard navigation
    installEventFilter(widget);

    // Update accessible properties
    updateAccessibleProperties(widget);

    // Apply high contrast styles if needed
    if (s_highContrastMode) {
        applyHighContrastStyles(widget);
    }
}

void FluentAccessibilityHelper::setAccessibleName(QWidget* widget,
                                                  const QString& name) {
    if (widget) {
        widget->setAccessibleName(name);
    }
}

void FluentAccessibilityHelper::setAccessibleDescription(
    QWidget* widget, const QString& description) {
    if (widget) {
        widget->setAccessibleDescription(description);
    }
}

void FluentAccessibilityHelper::setAccessibleRole(QWidget* widget,
                                                  QAccessible::Role role) {
    if (widget) {
        widget->setProperty("accessibleRole", static_cast<int>(role));
    }
}

bool FluentAccessibilityHelper::handleKeyboardNavigation(QWidget* widget,
                                                         QKeyEvent* event) {
    if (!widget || !event)
        return false;

    switch (event->key()) {
        case Qt::Key_Tab:
            // Handle tab navigation - let Qt handle this naturally
            return false;  // Let Qt handle tab navigation

        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Space:
            // Activate focused element
            if (widget->hasFocus()) {
                QAccessibleEvent activateEvent(widget,
                                               QAccessible::StateChanged);
                QAccessible::updateAccessibility(&activateEvent);
                return true;
            }
            break;

        case Qt::Key_Escape:
            // Close dialogs or cancel operations
            announceToScreenReader("Cancelled", QAccessible::Alert);
            return false;  // Let parent handle

        default:
            break;
    }

    return false;
}

void FluentAccessibilityHelper::setupKeyboardNavigation(QWidget* container) {
    if (!container)
        return;

    // Create keyboard navigator
    auto* navigator = new FluentKeyboardNavigator(container, container);

    // Find all focusable widgets
    auto focusableWidgets = container->findChildren<QWidget*>();
    for (auto* widget : focusableWidgets) {
        if (widget->focusPolicy() != Qt::NoFocus) {
            navigator->addNavigableWidget(widget);
        }
    }
}

bool FluentAccessibilityHelper::isHighContrastMode() {
    // Check system high contrast mode
#ifdef Q_OS_WIN
    // Windows high contrast detection
    QSettings registry(
        "HKEY_CURRENT_USER\\Control Panel\\Accessibility\\HighContrast",
        QSettings::NativeFormat);
    return registry.value("Flags", 0).toInt() & 1;
#elif defined(Q_OS_MAC)
    // macOS high contrast detection
    return QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
    // Linux/Unix - check environment variables
    QString theme = qgetenv("GTK_THEME");
    return theme.contains("HighContrast", Qt::CaseInsensitive);
#endif
}

void FluentAccessibilityHelper::applyHighContrastStyles(QWidget* widget) {
    if (!widget)
        return;

    QString highContrastStyle = getHighContrastStyleSheet(widget->styleSheet());
    widget->setStyleSheet(highContrastStyle);
}

QString FluentAccessibilityHelper::getHighContrastStyleSheet(
    const QString& baseStyle) {
    QString highContrastAdditions = R"(
        QWidget {
            border: 2px solid black;
            background-color: white;
            color: black;
        }
        QWidget:focus {
            border: 3px solid blue;
            outline: 2px solid yellow;
        }
        QPushButton {
            border: 2px solid black;
            background-color: white;
            color: black;
            padding: 4px;
        }
        QPushButton:hover {
            background-color: yellow;
            color: black;
        }
        QPushButton:pressed {
            background-color: blue;
            color: white;
        }
        QLineEdit, QTextEdit {
            border: 2px solid black;
            background-color: white;
            color: black;
        }
        QLineEdit:focus, QTextEdit:focus {
            border: 3px solid blue;
        }
    )";

    return baseStyle + "\n" + highContrastAdditions;
}

void FluentAccessibilityHelper::setFocusIndicator(QWidget* widget,
                                                  bool visible) {
    if (!widget)
        return;

    if (visible) {
        widget->setStyleSheet(widget->styleSheet() +
                              "\nQWidget:focus { border: 2px solid blue; "
                              "outline: 1px solid yellow; }");
    }
}

void FluentAccessibilityHelper::ensureVisibleFocus(QWidget* widget) {
    if (!widget)
        return;

    // Ensure the focused widget is visible
    widget->ensurePolished();

    // Scroll to make it visible if in a scroll area
    QWidget* parent = widget->parentWidget();
    while (parent) {
        if (auto* scrollArea = qobject_cast<QScrollArea*>(parent)) {
            scrollArea->ensureWidgetVisible(widget);
            break;
        }
        parent = parent->parentWidget();
    }
}

void FluentAccessibilityHelper::announceFocusChange(QWidget* widget) {
    if (!widget)
        return;

    QString widgetName = widget->accessibleName();
    if (widgetName.isEmpty()) {
        widgetName = widget->objectName();
    }
    if (widgetName.isEmpty()) {
        widgetName = getWidgetDescription(widget);
    }

    QString message = QString("Focused on %1").arg(widgetName);
    announceToScreenReader(message, QAccessible::Focus);
}

void FluentAccessibilityHelper::setupThemeAccessibility(
    QWidget* themeControls) {
    if (!themeControls)
        return;

    enhanceWidgetAccessibility(themeControls, "toolbar");
    setAccessibleName(themeControls, "Theme Controls");
    setAccessibleDescription(
        themeControls,
        "Controls for changing application theme and appearance");

    // Setup accessibility for child controls
    auto children = themeControls->findChildren<QWidget*>();
    for (auto* child : children) {
        if (child->objectName().contains("theme", Qt::CaseInsensitive)) {
            setupButtonAccessibility(child, "Change theme mode");
        } else if (child->objectName().contains("accent",
                                                Qt::CaseInsensitive)) {
            setupButtonAccessibility(child, "Change accent color");
        } else if (child->objectName().contains("contrast",
                                                Qt::CaseInsensitive)) {
            setupButtonAccessibility(child, "Toggle high contrast mode");
        }
    }
}

void FluentAccessibilityHelper::announceAccentColorChange(
    const QString& colorName) {
    QString message = QString("Accent color changed to %1").arg(colorName);
    announceToScreenReader(message, QAccessible::Alert);
}

void FluentAccessibilityHelper::announceContrastModeChange(bool highContrast) {
    QString message = highContrast ? "High contrast mode enabled"
                                   : "High contrast mode disabled";
    announceToScreenReader(message, QAccessible::Alert);
}

void FluentAccessibilityHelper::onFocusChanged(QWidget* old, QWidget* now) {
    Q_UNUSED(old)

    if (now && now != m_lastFocusedWidget) {
        m_lastFocusedWidget = now;
        m_focusTimer->start();
        ensureVisibleFocus(now);
    }
}

void FluentAccessibilityHelper::installEventFilter(QWidget* widget) {
    // Note: This would need an instance to work properly
    // For now, just enhance the widget's accessibility
    enhanceWidgetAccessibility(widget);
}

QString FluentAccessibilityHelper::getWidgetDescription(QWidget* widget) {
    if (!widget)
        return "Unknown widget";

    QString className = widget->metaObject()->className();

    // Remove namespace prefixes
    if (className.contains("::")) {
        className = className.split("::").last();
    }

    // Convert CamelCase to readable text
    QString result;
    for (int i = 0; i < className.length(); ++i) {
        if (i > 0 && className[i].isUpper()) {
            result += " ";
        }
        result += className[i].toLower();
    }

    return result;
}

void FluentAccessibilityHelper::updateAccessibleProperties(QWidget* widget) {
    if (!widget)
        return;

    // Ensure widget has accessible name
    if (widget->accessibleName().isEmpty()) {
        QString name = widget->objectName();
        if (name.isEmpty()) {
            name = getWidgetDescription(widget);
        }
        widget->setAccessibleName(name);
    }

    // Update accessible interface
    QAccessibleEvent updateEvent(widget, QAccessible::NameChanged);
    QAccessible::updateAccessibility(&updateEvent);
}

// FluentKeyboardNavigator implementation
FluentKeyboardNavigator::FluentKeyboardNavigator(QWidget* container,
                                                 QObject* parent)
    : QObject(parent), m_container(container) {
    if (m_container) {
        m_container->installEventFilter(this);
    }
}

void FluentKeyboardNavigator::addNavigableWidget(QWidget* widget,
                                                 int priority) {
    if (!widget || m_navigableWidgets.contains(widget))
        return;

    m_navigableWidgets.append(widget);
    m_priorities[widget] = priority;

    // Connect to widget destruction
    connect(widget, &QObject::destroyed, this,
            &FluentKeyboardNavigator::onWidgetDestroyed);

    updateTabOrder();
}

void FluentKeyboardNavigator::removeNavigableWidget(QWidget* widget) {
    if (!widget)
        return;

    m_navigableWidgets.removeAll(widget);
    m_priorities.remove(widget);
    updateTabOrder();
}

void FluentKeyboardNavigator::setNavigationOrder(const QList<QWidget*>& order) {
    m_navigableWidgets = order;
    updateTabOrder();
}

bool FluentKeyboardNavigator::handleKeyPress(QKeyEvent* event) {
    if (!event)
        return false;

    switch (event->key()) {
        case Qt::Key_Tab:
            if (event->modifiers() & Qt::ShiftModifier) {
                focusPrevious();
            } else {
                focusNext();
            }
            return true;

        case Qt::Key_Home:
            focusFirst();
            return true;

        case Qt::Key_End:
            focusLast();
            return true;

        default:
            return false;
    }
}

void FluentKeyboardNavigator::focusNext() {
    if (m_navigableWidgets.isEmpty())
        return;

    QWidget* current = qApp->focusWidget();
    QWidget* next = findNextWidget(current, true);

    if (next) {
        next->setFocus();
        emit navigationChanged(next, current);
    }
}

void FluentKeyboardNavigator::focusPrevious() {
    if (m_navigableWidgets.isEmpty())
        return;

    QWidget* current = qApp->focusWidget();
    QWidget* previous = findNextWidget(current, false);

    if (previous) {
        previous->setFocus();
        emit navigationChanged(previous, current);
    }
}

void FluentKeyboardNavigator::focusFirst() {
    if (!m_navigableWidgets.isEmpty()) {
        QWidget* first = m_navigableWidgets.first();
        if (isWidgetNavigable(first)) {
            first->setFocus();
            emit navigationChanged(first, nullptr);
        }
    }
}

void FluentKeyboardNavigator::focusLast() {
    if (!m_navigableWidgets.isEmpty()) {
        QWidget* last = m_navigableWidgets.last();
        if (isWidgetNavigable(last)) {
            last->setFocus();
            emit navigationChanged(last, nullptr);
        }
    }
}

void FluentKeyboardNavigator::onWidgetDestroyed(QObject* widget) {
    auto* w = static_cast<QWidget*>(widget);
    removeNavigableWidget(w);
}

void FluentKeyboardNavigator::updateTabOrder() {
    // Sort widgets by priority
    std::sort(m_navigableWidgets.begin(), m_navigableWidgets.end(),
              [this](QWidget* a, QWidget* b) {
                  return m_priorities.value(a, 0) < m_priorities.value(b, 0);
              });

    // Set tab order
    for (int i = 0; i < m_navigableWidgets.size() - 1; ++i) {
        QWidget::setTabOrder(m_navigableWidgets[i], m_navigableWidgets[i + 1]);
    }
}

QWidget* FluentKeyboardNavigator::findNextWidget(QWidget* current,
                                                 bool forward) {
    if (m_navigableWidgets.isEmpty())
        return nullptr;

    int currentIndex = m_navigableWidgets.indexOf(current);

    if (currentIndex == -1) {
        // Current widget not in list, start from beginning or end
        return forward ? m_navigableWidgets.first() : m_navigableWidgets.last();
    }

    int nextIndex = currentIndex;
    int step = forward ? 1 : -1;

    do {
        nextIndex += step;

        // Wrap around
        if (nextIndex >= m_navigableWidgets.size()) {
            nextIndex = 0;
            emit navigationWrapped(true);
        } else if (nextIndex < 0) {
            nextIndex = m_navigableWidgets.size() - 1;
            emit navigationWrapped(false);
        }

        QWidget* candidate = m_navigableWidgets[nextIndex];
        if (isWidgetNavigable(candidate)) {
            return candidate;
        }

    } while (nextIndex != currentIndex);

    return nullptr;  // No navigable widget found
}

bool FluentKeyboardNavigator::isWidgetNavigable(QWidget* widget) {
    return widget && widget->isVisible() && widget->isEnabled() &&
           widget->focusPolicy() != Qt::NoFocus;
}

// FluentAccessibleWidget implementation
FluentAccessibleWidget::FluentAccessibleWidget(QWidget* widget,
                                               QAccessible::Role role)
    : QAccessibleWidget(widget), m_role(role) {}

QString FluentAccessibleWidget::text(QAccessible::Text t) const {
    switch (t) {
        case QAccessible::Name:
            if (!m_customName.isEmpty()) {
                return m_customName;
            }
            break;
        case QAccessible::Description:
            if (!m_customDescription.isEmpty()) {
                return m_customDescription;
            }
            break;
        default:
            break;
    }

    return QAccessibleWidget::text(t);
}

void FluentAccessibleWidget::setText(QAccessible::Text t, const QString& text) {
    switch (t) {
        case QAccessible::Name:
            m_customName = text;
            break;
        case QAccessible::Description:
            m_customDescription = text;
            break;
        default:
            QAccessibleWidget::setText(t, text);
            break;
    }
}

QAccessible::State FluentAccessibleWidget::state() const {
    QAccessible::State state = QAccessibleWidget::state();

    if (m_isLiveRegion) {
        state.active = true;
    }

    return state;
}

QAccessible::Role FluentAccessibleWidget::role() const {
    if (m_role != QAccessible::NoRole) {
        return m_role;
    }
    return QAccessibleWidget::role();
}

void FluentAccessibleWidget::announceStateChange(const QString& newState) {
    FluentAccessibilityHelper::announceComponentStateChange(widget(), newState);
}

void FluentAccessibleWidget::setLiveRegion(bool isLive) {
    m_isLiveRegion = isLive;
}

}  // namespace FluentQt::Examples
