// src/Accessibility/FluentKeyboardNavigationManager.cpp
#include "FluentQt/Accessibility/FluentKeyboardNavigationManager.h"
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QMutexLocker>
#include <QThread>
#include <QWidget>
#include <algorithm>

namespace FluentQt::Accessibility {

FluentKeyboardNavigationManager& FluentKeyboardNavigationManager::instance() {
    static FluentKeyboardNavigationManager instance;
    return instance;
}

FluentKeyboardNavigationManager::FluentKeyboardNavigationManager() {
    // Initialize focus update timer
    m_focusUpdateTimer = new QTimer(this);
    connect(m_focusUpdateTimer, &QTimer::timeout, this,
            &FluentKeyboardNavigationManager::updateFocusIndicators);

    // Only start timer if we have a proper event loop
    if (QApplication::instance() &&
        QThread::currentThread() == QApplication::instance()->thread()) {
        m_focusUpdateTimer->start(100);  // Update every 100ms
    }

    // Connect to application focus changes (only if QApplication exists)
    if (auto* app = qobject_cast<QApplication*>(QApplication::instance())) {
        connect(app, &QApplication::focusChanged, this,
                &FluentKeyboardNavigationManager::onApplicationFocusChanged);
    } else {
        // In test/headless environment, QApplication might not be available yet
        // This is normal and expected - suppress debug messages to reduce noise
        // The connection will be established later if needed
    }

    qDebug() << "FluentKeyboardNavigationManager initialized";
}

void FluentKeyboardNavigationManager::setNavigationConfig(
    const FluentKeyboardConfig& config) {
    QMutexLocker locker(&m_mutex);
    m_config = config;
    emit navigationModeChanged(config.mode);
}

void FluentKeyboardNavigationManager::registerNavigableWidget(QWidget* widget,
                                                              int tabOrder) {
    if (!widget)
        return;

    QMutexLocker locker(&m_mutex);

    if (!m_navigableWidgets.contains(widget)) {
        m_navigableWidgets.append(widget);

        // Connect to widget destruction
        connect(widget, &QObject::destroyed, this,
                &FluentKeyboardNavigationManager::onWidgetDestroyed);
    }

    if (tabOrder >= 0) {
        m_tabOrders[widget] = tabOrder;
    }

    // Ensure widget can receive focus
    if (widget->focusPolicy() == Qt::NoFocus) {
        widget->setFocusPolicy(Qt::TabFocus);
    }
}

void FluentKeyboardNavigationManager::unregisterNavigableWidget(
    QWidget* widget) {
    if (!widget)
        return;

    QMutexLocker locker(&m_mutex);

    m_navigableWidgets.removeAll(widget);
    m_tabOrders.erase(widget);
    m_widgetModes.erase(widget);

    // Remove from focus traps
    for (auto it = m_focusTraps.begin(); it != m_focusTraps.end();) {
        if (it->first == widget) {
            it = m_focusTraps.erase(it);
        } else {
            ++it;
        }
    }

    // Remove from spatial groups
    m_spatialGroups.erase(widget);
    m_spatialGrids.erase(widget);

    // Remove from roving groups
    m_rovingGroups.erase(widget);
    m_rovingActive.erase(widget);

    // Remove focus indicator
    auto indicatorIt = m_focusIndicators.find(widget);
    if (indicatorIt != m_focusIndicators.end()) {
        if (indicatorIt->second) {
            indicatorIt->second->deleteLater();
        }
        m_focusIndicators.erase(indicatorIt);
    }
}

void FluentKeyboardNavigationManager::setWidgetNavigationMode(
    QWidget* widget, FluentNavigationMode mode) {
    if (!widget)
        return;

    QMutexLocker locker(&m_mutex);
    m_widgetModes[widget] = mode;
}

void FluentKeyboardNavigationManager::setWidgetTabOrder(QWidget* widget,
                                                        int order) {
    if (!widget)
        return;

    QMutexLocker locker(&m_mutex);
    m_tabOrders[widget] = order;
}

bool FluentKeyboardNavigationManager::moveFocus(
    FluentFocusDirection direction) {
    QWidget* currentWidget = getCurrentFocus();
    if (!currentWidget) {
        // Find first focusable widget
        currentWidget = findFirstFocusableWidget(nullptr);
        if (currentWidget) {
            setFocus(currentWidget);
            return true;
        }
        return false;
    }

    QWidget* nextWidget = getNextFocusWidget(currentWidget, direction);
    if (nextWidget && nextWidget != currentWidget) {
        setFocus(nextWidget);
        return true;
    }

    return false;
}

void FluentKeyboardNavigationManager::setFocus(QWidget* widget, bool announce) {
    if (!widget || !isWidgetFocusable(widget)) {
        return;
    }

    QWidget* oldFocus = m_currentFocus;
    m_currentFocus = widget;

    // Set actual focus
    widget->setFocus();

    // Update navigation history
    if (m_navigationHistoryEnabled) {
        m_navigationHistory.removeAll(widget);  // Remove if already in history
        m_navigationHistory.append(widget);

        // Limit history size
        while (m_navigationHistory.size() > m_maxHistorySize) {
            m_navigationHistory.removeFirst();
        }
    }

    // Update focus indicator
    updateFocusIndicator(widget);

    // Announce to screen reader if enabled
    if (announce && m_config.announceNavigation) {
        announceNavigation(widget);
    }

    emit focusChanged(widget, oldFocus);
}

QWidget* FluentKeyboardNavigationManager::getCurrentFocus() const {
    return QApplication::focusWidget();
}

QWidget* FluentKeyboardNavigationManager::getNextFocusWidget(
    QWidget* current, FluentFocusDirection direction) {
    if (!current)
        return nullptr;

    QMutexLocker locker(&m_mutex);

    // Check widget-specific navigation mode
    auto modeIt = m_widgetModes.find(current);
    FluentNavigationMode mode =
        (modeIt != m_widgetModes.end()) ? modeIt->second : m_config.mode;

    switch (mode) {
        case FluentNavigationMode::Tab:
            return findNextTabWidget(current,
                                     direction == FluentFocusDirection::Next);

        case FluentNavigationMode::Spatial:
            return findSpatialNeighbor(current, direction);

        case FluentNavigationMode::Arrow:
            // Arrow navigation within the same container
            return findNextTabWidget(current,
                                     direction == FluentFocusDirection::Next);

        default:
            return findNextTabWidget(current,
                                     direction == FluentFocusDirection::Next);
    }
}

void FluentKeyboardNavigationManager::installFocusTrap(QWidget* container,
                                                       QWidget* initialFocus) {
    if (!container)
        return;

    QMutexLocker locker(&m_mutex);

    FluentFocusTrap trap;
    trap.container = container;
    trap.initialFocus = initialFocus;
    trap.firstFocusable = findFirstFocusableWidget(container);
    trap.lastFocusable = findLastFocusableWidget(container);
    trap.previousFocus = getCurrentFocus();

    m_focusTraps[container] = trap;
}

void FluentKeyboardNavigationManager::removeFocusTrap(QWidget* container) {
    if (!container)
        return;

    QMutexLocker locker(&m_mutex);

    auto it = m_focusTraps.find(container);
    if (it != m_focusTraps.end()) {
        FluentFocusTrap& trap = it->second;

        // Restore previous focus if needed
        if (trap.restoreFocus && trap.previousFocus &&
            isWidgetFocusable(trap.previousFocus)) {
            trap.previousFocus->setFocus();
        }

        m_focusTraps.erase(it);
        emit focusTrapDeactivated(container);
    }
}

void FluentKeyboardNavigationManager::activateFocusTrap(QWidget* container) {
    if (!container)
        return;

    QMutexLocker locker(&m_mutex);

    auto it = m_focusTraps.find(container);
    if (it != m_focusTraps.end()) {
        FluentFocusTrap& trap = it->second;
        trap.isActive = true;
        m_activeTrap = container;

        // Set initial focus
        QWidget* focusTarget =
            trap.initialFocus ? trap.initialFocus : trap.firstFocusable;
        if (focusTarget) {
            locker.unlock();
            setFocus(focusTarget);
            locker.relock();
        }

        emit focusTrapActivated(container);
    }
}

void FluentKeyboardNavigationManager::deactivateFocusTrap(QWidget* container) {
    if (!container)
        return;

    QMutexLocker locker(&m_mutex);

    auto it = m_focusTraps.find(container);
    if (it != m_focusTraps.end()) {
        it->second.isActive = false;
        if (m_activeTrap == container) {
            m_activeTrap = nullptr;
        }
        emit focusTrapDeactivated(container);
    }
}

bool FluentKeyboardNavigationManager::isFocusTrapped(QWidget* widget) const {
    if (!widget || !m_activeTrap)
        return false;

    QMutexLocker locker(&m_mutex);

    auto it = m_focusTraps.find(m_activeTrap);
    if (it != m_focusTraps.end() && it->second.isActive) {
        // Check if widget is within the trapped container
        QWidget* parent = widget;
        while (parent) {
            if (parent == m_activeTrap) {
                return true;
            }
            parent = parent->parentWidget();
        }
    }

    return false;
}

void FluentKeyboardNavigationManager::registerShortcut(
    const QKeySequence& sequence, std::function<void()> action,
    const QString& description, FluentShortcutType type) {
    FluentKeyboardShortcut shortcut;
    shortcut.sequence = sequence;
    shortcut.action = action;
    shortcut.description = description;
    shortcut.type = type;
    shortcut.enabled = true;

    QMutexLocker locker(&m_mutex);
    m_shortcuts.append(shortcut);
}

void FluentKeyboardNavigationManager::registerShortcut(
    const FluentKeyboardShortcut& shortcut) {
    QMutexLocker locker(&m_mutex);
    m_shortcuts.append(shortcut);
}

void FluentKeyboardNavigationManager::unregisterShortcut(
    const QKeySequence& sequence) {
    QMutexLocker locker(&m_mutex);

    m_shortcuts.erase(
        std::remove_if(m_shortcuts.begin(), m_shortcuts.end(),
                       [&sequence](const FluentKeyboardShortcut& shortcut) {
                           return shortcut.sequence == sequence;
                       }),
        m_shortcuts.end());
}

void FluentKeyboardNavigationManager::addSkipLink(
    const QString& text, QWidget* target, const QKeySequence& shortcut) {
    if (!target)
        return;

    FluentSkipLink skipLink;
    skipLink.text = text;
    skipLink.target = target;
    skipLink.shortcut = shortcut;
    skipLink.description = QString("Skip to %1").arg(text);

    QMutexLocker locker(&m_mutex);
    m_skipLinks.append(skipLink);
}

void FluentKeyboardNavigationManager::removeSkipLink(QWidget* target) {
    if (!target)
        return;

    QMutexLocker locker(&m_mutex);

    m_skipLinks.erase(std::remove_if(m_skipLinks.begin(), m_skipLinks.end(),
                                     [target](const FluentSkipLink& link) {
                                         return link.target == target;
                                     }),
                      m_skipLinks.end());
}

QList<FluentSkipLink> FluentKeyboardNavigationManager::getSkipLinks() const {
    QMutexLocker locker(&m_mutex);
    return m_skipLinks;
}

void FluentKeyboardNavigationManager::enableSpatialNavigation(
    QWidget* container) {
    if (!container)
        return;

    QMutexLocker locker(&m_mutex);

    // Find all focusable children
    QList<QWidget*> focusableChildren;
    for (QWidget* child : container->findChildren<QWidget*>()) {
        if (isWidgetFocusable(child) && isWidgetVisible(child)) {
            focusableChildren.append(child);
        }
    }

    m_spatialGroups[container] = focusableChildren;
}

void FluentKeyboardNavigationManager::setSpatialNavigationGrid(
    QWidget* container, int rows, int columns) {
    if (!container)
        return;

    QMutexLocker locker(&m_mutex);
    m_spatialGrids[container] = std::make_pair(rows, columns);
}

QWidget* FluentKeyboardNavigationManager::findSpatialNeighbor(
    QWidget* current, FluentFocusDirection direction) {
    if (!current)
        return nullptr;

    QMutexLocker locker(&m_mutex);

    // Find the spatial group containing this widget
    QList<QWidget*> candidates;
    for (const auto& [container, widgets] : m_spatialGroups) {
        if (widgets.contains(current)) {
            candidates = widgets;
            break;
        }
    }

    if (candidates.isEmpty()) {
        return nullptr;
    }

    return findBestSpatialMatch(current, candidates, direction);
}

bool FluentKeyboardNavigationManager::handleKeyEvent(QKeyEvent* event) {
    if (!event)
        return false;

    // Handle shortcuts first
    if (handleShortcut(event)) {
        return true;
    }

    // Handle navigation
    switch (event->key()) {
        case Qt::Key_Tab:
            return handleTabNavigation(event);
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
            return handleArrowNavigation(event);
        default:
            return false;
    }
}

bool FluentKeyboardNavigationManager::handleTabNavigation(QKeyEvent* event) {
    if (!m_config.enableArrowNavigation) {
        return false;
    }

    bool forward = !(event->modifiers() & Qt::ShiftModifier);
    FluentFocusDirection direction =
        forward ? FluentFocusDirection::Next : FluentFocusDirection::Previous;

    return moveFocus(direction);
}

bool FluentKeyboardNavigationManager::handleArrowNavigation(QKeyEvent* event) {
    if (!m_config.enableArrowNavigation) {
        return false;
    }

    FluentFocusDirection direction;
    switch (event->key()) {
        case Qt::Key_Up:
            direction = FluentFocusDirection::Up;
            break;
        case Qt::Key_Down:
            direction = FluentFocusDirection::Down;
            break;
        case Qt::Key_Left:
            direction = FluentFocusDirection::Left;
            break;
        case Qt::Key_Right:
            direction = FluentFocusDirection::Right;
            break;
        default:
            return false;
    }

    return moveFocus(direction);
}

bool FluentKeyboardNavigationManager::handleShortcut(QKeyEvent* event) {
    QKeySequence sequence(event->key() | event->modifiers());

    QMutexLocker locker(&m_mutex);

    for (const FluentKeyboardShortcut& shortcut : m_shortcuts) {
        if (shortcut.enabled && matchesShortcut(event, shortcut.sequence)) {
            locker.unlock();
            executeShortcut(shortcut);
            return true;
        }
    }

    return false;
}

void FluentKeyboardNavigationManager::announceNavigation(
    QWidget* widget, const QString& action) {
    if (!widget || !m_config.announceNavigation) {
        return;
    }

    QString announcement =
        action.isEmpty() ? QString("Focused %1")
                               .arg(widget->accessibleName().isEmpty()
                                        ? widget->metaObject()->className()
                                        : widget->accessibleName())
                         : action;

    // This would integrate with the screen reader manager
    qDebug() << "Navigation announcement:" << announcement;
}

QStringList FluentKeyboardNavigationManager::validateKeyboardNavigation(
    QWidget* rootWidget) {
    QStringList issues;

    if (!rootWidget) {
        return issues;
    }

    // Find all widgets that should be focusable
    QList<QWidget*> widgets = rootWidget->findChildren<QWidget*>();

    for (QWidget* widget : widgets) {
        if (!isWidgetVisible(widget)) {
            continue;
        }

        // Check if interactive widgets are keyboard accessible
        if (widget->inherits("QPushButton") || widget->inherits("QLineEdit") ||
            widget->inherits("QComboBox") || widget->inherits("QCheckBox") ||
            widget->inherits("QRadioButton")) {
            if (!isKeyboardAccessible(widget)) {
                issues.append(QString("Widget %1 is not keyboard accessible")
                                  .arg(widget->objectName().isEmpty()
                                           ? widget->metaObject()->className()
                                           : widget->objectName()));
            }
        }
    }

    return issues;
}

bool FluentKeyboardNavigationManager::isKeyboardAccessible(QWidget* widget) {
    return widget && widget->focusPolicy() != Qt::NoFocus &&
           isWidgetVisible(widget) && widget->isEnabled();
}

// Private helper methods
QWidget* FluentKeyboardNavigationManager::findNextTabWidget(QWidget* current,
                                                            bool forward) {
    if (!current)
        return nullptr;

    QWidget* next = current;
    do {
        next =
            forward ? next->nextInFocusChain() : next->previousInFocusChain();

        if (next && isWidgetFocusable(next) && isWidgetVisible(next)) {
            return next;
        }

    } while (next && next != current);

    return nullptr;
}

QWidget* FluentKeyboardNavigationManager::findFirstFocusableWidget(
    QWidget* container) {
    QWidget* root = container ? container : qApp->activeWindow();
    if (!root)
        return nullptr;

    QList<QWidget*> widgets = root->findChildren<QWidget*>();

    for (QWidget* widget : widgets) {
        if (isWidgetFocusable(widget) && isWidgetVisible(widget)) {
            return widget;
        }
    }

    return nullptr;
}

QWidget* FluentKeyboardNavigationManager::findLastFocusableWidget(
    QWidget* container) {
    QWidget* root = container ? container : qApp->activeWindow();
    if (!root)
        return nullptr;

    QList<QWidget*> widgets = root->findChildren<QWidget*>();

    for (int i = widgets.size() - 1; i >= 0; --i) {
        QWidget* widget = widgets[i];
        if (isWidgetFocusable(widget) && isWidgetVisible(widget)) {
            return widget;
        }
    }

    return nullptr;
}

bool FluentKeyboardNavigationManager::isWidgetFocusable(QWidget* widget) {
    return widget && widget->focusPolicy() != Qt::NoFocus &&
           widget->isEnabled();
}

bool FluentKeyboardNavigationManager::isWidgetVisible(QWidget* widget) {
    return widget && widget->isVisible();
}

QWidget* FluentKeyboardNavigationManager::findBestSpatialMatch(
    QWidget* current, const QList<QWidget*>& candidates,
    FluentFocusDirection direction) {
    if (!current || candidates.isEmpty()) {
        return nullptr;
    }

    QRect currentRect = getWidgetRect(current);
    QWidget* bestMatch = nullptr;
    double bestDistance = std::numeric_limits<double>::max();

    for (QWidget* candidate : candidates) {
        if (candidate == current || !isWidgetFocusable(candidate)) {
            continue;
        }

        QRect candidateRect = getWidgetRect(candidate);
        double distance =
            calculateDistance(currentRect, candidateRect, direction);

        if (distance < bestDistance) {
            bestDistance = distance;
            bestMatch = candidate;
        }
    }

    return bestMatch;
}

QRect FluentKeyboardNavigationManager::getWidgetRect(QWidget* widget) {
    if (!widget)
        return QRect();

    return widget->geometry();
}

double FluentKeyboardNavigationManager::calculateDistance(
    const QRect& from, const QRect& to, FluentFocusDirection direction) {
    QPoint fromCenter = from.center();
    QPoint toCenter = to.center();

    // Calculate Euclidean distance
    double dx = toCenter.x() - fromCenter.x();
    double dy = toCenter.y() - fromCenter.y();
    double distance = std::sqrt(dx * dx + dy * dy);

    // Apply directional bias
    switch (direction) {
        case FluentFocusDirection::Up:
            if (dy >= 0)
                distance *= 10;  // Penalize downward movement
            break;
        case FluentFocusDirection::Down:
            if (dy <= 0)
                distance *= 10;  // Penalize upward movement
            break;
        case FluentFocusDirection::Left:
            if (dx >= 0)
                distance *= 10;  // Penalize rightward movement
            break;
        case FluentFocusDirection::Right:
            if (dx <= 0)
                distance *= 10;  // Penalize leftward movement
            break;
        default:
            break;
    }

    return distance;
}

bool FluentKeyboardNavigationManager::matchesShortcut(
    QKeyEvent* event, const QKeySequence& sequence) {
    if (sequence.isEmpty())
        return false;

    QKeySequence eventSequence(event->key() | event->modifiers());
    return eventSequence == sequence;
}

void FluentKeyboardNavigationManager::executeShortcut(
    const FluentKeyboardShortcut& shortcut) {
    if (shortcut.action) {
        shortcut.action();
        emit shortcutActivated(shortcut.sequence, shortcut.description);
    }
}

void FluentKeyboardNavigationManager::updateFocusIndicator(QWidget* widget) {
    if (!widget)
        return;

    // This would update visual focus indicators
    // Implementation depends on the specific styling system
    emit focusIndicatorChanged(widget, m_config.focusStyle);
}

void FluentKeyboardNavigationManager::onWidgetDestroyed(QObject* widget) {
    QWidget* w = qobject_cast<QWidget*>(widget);
    if (w) {
        unregisterNavigableWidget(w);
    }
}

void FluentKeyboardNavigationManager::onApplicationFocusChanged(QWidget* old,
                                                                QWidget* now) {
    Q_UNUSED(old)
    m_currentFocus = now;

    if (now) {
        updateFocusIndicator(now);
    }
}

void FluentKeyboardNavigationManager::updateFocusIndicators() {
    // Update all focus indicators
    // This is called periodically to ensure indicators are properly positioned
}

}  // namespace FluentQt::Accessibility
