// src/Components/FluentToastManager.cpp
#include "FluentQt/Components/FluentToastManager.h"
#include "FluentQt/Components/FluentToast.h"
#include "FluentQt/Styling/FluentTheme.h"

#include <QApplication>
#include <QScreen>
#include <QWidget>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QDebug>

using namespace FluentQt::Components;
using namespace FluentQt::Styling;

// Static instance
FluentToastManager* FluentToastManager::s_globalInstance = nullptr;

FluentToastManager::FluentToastManager(QObject* parent)
    : QObject(parent)
{
    setupConnections();
    updateScreenGeometry();
    
    // Queue processing timer
    m_queueTimer = new QTimer(this);
    m_queueTimer->setSingleShot(true);
    m_queueTimer->setInterval(100);
    connect(m_queueTimer, &QTimer::timeout, this, &FluentToastManager::processQueue);
    
    // Animation group for repositioning
    m_repositionAnimation = std::make_unique<QParallelAnimationGroup>(this);
}

FluentToastManager::FluentToastManager(const FluentToastManagerConfig& config, QObject* parent)
    : FluentToastManager(parent)
{
    setConfiguration(config);
}

FluentToastManager::~FluentToastManager() {
    // Clean up all visible toasts
    for (auto& toastList : m_visibleToasts) {
        for (auto* toast : toastList) {
            if (toast) {
                disconnectToast(toast);
                toast->deleteLater();
            }
        }
    }
    
    // Clean up queued toasts
    while (!m_queuedToasts.isEmpty()) {
        auto* toast = m_queuedToasts.dequeue();
        if (toast) {
            toast->deleteLater();
        }
    }
}

FluentToastManager& FluentToastManager::instance() {
    if (!s_globalInstance) {
        s_globalInstance = new FluentToastManager(qApp);
    }
    return *s_globalInstance;
}

void FluentToastManager::setGlobalInstance(FluentToastManager* manager) {
    if (s_globalInstance && s_globalInstance != manager) {
        delete s_globalInstance;
    }
    s_globalInstance = manager;
}

void FluentToastManager::setDefaultPosition(FluentToastPosition position) {
    if (m_config.defaultPosition != position) {
        m_config.defaultPosition = position;
        emit defaultPositionChanged(position);
    }
}

void FluentToastManager::setMaxVisible(int max) {
    if (m_config.maxVisible != max) {
        m_config.maxVisible = qMax(1, max);
        
        // Hide excess toasts if needed
        for (auto& toastList : m_visibleToasts) {
            while (toastList.size() > m_config.maxVisible) {
                auto* toast = toastList.takeLast();
                if (toast) {
                    toast->hideAnimated();
                }
            }
        }
        
        emit maxVisibleChanged(m_config.maxVisible);
    }
}

void FluentToastManager::setMaxQueued(int max) {
    if (m_config.maxQueued != max) {
        m_config.maxQueued = qMax(0, max);
        
        // Remove excess queued toasts if needed
        while (m_queuedToasts.size() > m_config.maxQueued) {
            auto* toast = m_queuedToasts.dequeue();
            if (toast) {
                toast->deleteLater();
            }
        }
        
        emit maxQueuedChanged(m_config.maxQueued);
    }
}

void FluentToastManager::setStackSpacing(int spacing) {
    if (m_config.stackSpacing != spacing) {
        m_config.stackSpacing = qMax(0, spacing);
        repositionToasts();
        emit stackSpacingChanged(m_config.stackSpacing);
    }
}

void FluentToastManager::setScreenMargin(int margin) {
    if (m_config.screenMargin != margin) {
        m_config.screenMargin = qMax(0, margin);
        repositionToasts();
        emit screenMarginChanged(m_config.screenMargin);
    }
}

void FluentToastManager::setAllowDuplicates(bool allow) {
    if (m_config.allowDuplicates != allow) {
        m_config.allowDuplicates = allow;
        emit allowDuplicatesChanged(allow);
    }
}

void FluentToastManager::setStackToasts(bool stack) {
    if (m_config.stackToasts != stack) {
        m_config.stackToasts = stack;
        
        if (!stack) {
            // Hide all but the most recent toast in each position
            for (auto& toastList : m_visibleToasts) {
                while (toastList.size() > 1) {
                    auto* toast = toastList.takeFirst();
                    if (toast) {
                        toast->hideAnimated();
                    }
                }
            }
        }
        
        emit stackToastsChanged(stack);
    }
}

void FluentToastManager::setConfiguration(const FluentToastManagerConfig& config) {
    const bool hasPositionChanged = (m_config.defaultPosition != config.defaultPosition);
    const bool hasMaxVisibleChanged = (m_config.maxVisible != config.maxVisible);
    const bool hasMaxQueuedChanged = (m_config.maxQueued != config.maxQueued);
    const bool hasSpacingChanged = (m_config.stackSpacing != config.stackSpacing);
    const bool hasMarginChanged = (m_config.screenMargin != config.screenMargin);
    const bool hasDuplicatesChanged = (m_config.allowDuplicates != config.allowDuplicates);
    const bool hasStackChanged = (m_config.stackToasts != config.stackToasts);
    
    m_config = config;
    
    // Update parent widget if changed
    if (m_config.parentWidget) {
        connect(m_config.parentWidget, &QObject::destroyed,
                this, &FluentToastManager::onParentWidgetDestroyed);
    }
    
    // Apply constraints
    if (hasMaxVisibleChanged) {
        setMaxVisible(m_config.maxVisible);
    }
    if (hasMaxQueuedChanged) {
        setMaxQueued(m_config.maxQueued);
    }
    if (hasSpacingChanged || hasMarginChanged) {
        repositionToasts();
    }
    if (hasStackChanged && !m_config.stackToasts) {
        // Hide excess toasts
        for (auto& toastList : m_visibleToasts) {
            while (toastList.size() > 1) {
                auto* toast = toastList.takeFirst();
                if (toast) {
                    toast->hideAnimated();
                }
            }
        }
    }
    
    // Emit change signals
    if (hasPositionChanged) emit defaultPositionChanged(config.defaultPosition);
    if (hasMaxVisibleChanged) emit maxVisibleChanged(config.maxVisible);
    if (hasMaxQueuedChanged) emit maxQueuedChanged(config.maxQueued);
    if (hasSpacingChanged) emit stackSpacingChanged(config.stackSpacing);
    if (hasMarginChanged) emit screenMarginChanged(config.screenMargin);
    if (hasDuplicatesChanged) emit allowDuplicatesChanged(config.allowDuplicates);
    if (hasStackChanged) emit stackToastsChanged(config.stackToasts);
}

void FluentToastManager::setParentWidget(QWidget* parent) {
    if (m_config.parentWidget != parent) {
        if (m_config.parentWidget) {
            disconnect(m_config.parentWidget, &QObject::destroyed,
                      this, &FluentToastManager::onParentWidgetDestroyed);
        }
        
        m_config.parentWidget = parent;
        
        if (parent) {
            connect(parent, &QObject::destroyed,
                    this, &FluentToastManager::onParentWidgetDestroyed);
        }
        
        updateScreenGeometry();
        repositionToasts();
    }
}

FluentToast* FluentToastManager::show(FluentToastType type, const QString& title, const QString& message) {
    return show(type, title, message, m_config.defaultPosition);
}

FluentToast* FluentToastManager::show(FluentToastType type, const QString& title, const QString& message, FluentToastPosition position) {
    auto* toast = new FluentToast(type, title, message);
    return show(toast, position);
}

FluentToast* FluentToastManager::show(FluentToastType type, const QString& title, const QString& message, const FluentToastConfig& config) {
    // Create toast with the specified type and config
    auto* toast = new FluentToast(type, title, message);

    // Apply config settings
    toast->setConfiguration(config);

    return show(toast, config.position);
}

FluentToast* FluentToastManager::show(FluentToast* toast) {
    return show(toast, m_config.defaultPosition);
}

FluentToast* FluentToastManager::show(FluentToast* toast, FluentToastPosition position) {
    if (!toast) return nullptr;
    
    // Check for duplicates if not allowed
    if (!m_config.allowDuplicates && isDuplicate(toast)) {
        toast->deleteLater();
        return nullptr;
    }
    
    // Check if we can show immediately
    if (canShowToast(toast)) {
        addToVisible(toast, position);
        connectToast(toast);
        
        // Position and show the toast
        const QRect toastRect = getToastRect(position, toast->sizeHint(), 
                                           visibleCountAt(position) - 1);
        toast->setGeometry(toastRect);
        toast->showAnimated();
        
        emit toastShown(toast);
        return toast;
    } else {
        // Add to queue
        addToQueue(toast);
        return toast;
    }
}

// Convenience methods
FluentToast* FluentToastManager::showInfo(const QString& title, const QString& message) {
    return show(FluentToastType::Info, title, message);
}

FluentToast* FluentToastManager::showSuccess(const QString& title, const QString& message) {
    return show(FluentToastType::Success, title, message);
}

FluentToast* FluentToastManager::showWarning(const QString& title, const QString& message) {
    return show(FluentToastType::Warning, title, message);
}

FluentToast* FluentToastManager::showError(const QString& title, const QString& message) {
    return show(FluentToastType::Error, title, message);
}

FluentToast* FluentToastManager::showCustom(const QIcon& icon, const QString& title, const QString& message) {
    auto* toast = FluentToast::createCustom(icon, title, message);
    return show(toast);
}

// Position-specific convenience methods
FluentToast* FluentToastManager::showInfoAt(FluentToastPosition position, const QString& title, const QString& message) {
    return show(FluentToastType::Info, title, message, position);
}

FluentToast* FluentToastManager::showSuccessAt(FluentToastPosition position, const QString& title, const QString& message) {
    return show(FluentToastType::Success, title, message, position);
}

FluentToast* FluentToastManager::showWarningAt(FluentToastPosition position, const QString& title, const QString& message) {
    return show(FluentToastType::Warning, title, message, position);
}

FluentToast* FluentToastManager::showErrorAt(FluentToastPosition position, const QString& title, const QString& message) {
    return show(FluentToastType::Error, title, message, position);
}

void FluentToastManager::hide(FluentToast* toast) {
    if (!toast) return;

    removeFromVisible(toast);
    toast->hideAnimated();
}

void FluentToastManager::hideAll() {
    for (auto& toastList : m_visibleToasts) {
        for (auto* toast : toastList) {
            if (toast) {
                toast->hideAnimated();
            }
        }
    }

    emit allToastsHidden();
}

void FluentToastManager::hideAllAt(FluentToastPosition position) {
    if (m_visibleToasts.contains(position)) {
        for (auto* toast : m_visibleToasts[position]) {
            if (toast) {
                toast->hideAnimated();
            }
        }
    }
}

void FluentToastManager::clear() {
    hideAll();
    clearQueue();
}

void FluentToastManager::clearAt(FluentToastPosition position) {
    hideAllAt(position);
}

void FluentToastManager::pauseQueue() {
    m_queuePaused = true;
    if (m_queueTimer) {
        m_queueTimer->stop();
    }
}

void FluentToastManager::resumeQueue() {
    m_queuePaused = false;
    if (!m_queuedToasts.isEmpty()) {
        m_queueTimer->start();
    }
}

void FluentToastManager::clearQueue() {
    while (!m_queuedToasts.isEmpty()) {
        auto* toast = m_queuedToasts.dequeue();
        if (toast) {
            toast->deleteLater();
        }
    }
}

int FluentToastManager::visibleCount() const {
    int count = 0;
    for (const auto& toastList : m_visibleToasts) {
        count += toastList.size();
    }
    return count;
}

int FluentToastManager::visibleCountAt(FluentToastPosition position) const {
    return m_visibleToasts.value(position).size();
}

QList<FluentToast*> FluentToastManager::visibleToasts() const {
    QList<FluentToast*> allToasts;
    for (const auto& toastList : m_visibleToasts) {
        allToasts.append(toastList);
    }
    return allToasts;
}

QList<FluentToast*> FluentToastManager::visibleToastsAt(FluentToastPosition position) const {
    return m_visibleToasts.value(position);
}

QRect FluentToastManager::getToastRect(FluentToastPosition position, const QSize& toastSize, int index) const {
    const QPoint pos = getToastPosition(position, toastSize, index);
    return QRect(pos, toastSize);
}

QPoint FluentToastManager::getToastPosition(FluentToastPosition position, const QSize& toastSize, int index) const {
    return calculateToastPosition(position, toastSize, index);
}

QRect FluentToastManager::getAvailableArea(FluentToastPosition position) const {
    return getPositionArea(position);
}

QRect FluentToastManager::getScreenRect() const {
    if (m_config.parentWidget) {
        return m_config.parentWidget->geometry();
    } else {
        return QApplication::primaryScreen()->availableGeometry();
    }
}

// Public slots
void FluentToastManager::processQueue() {
    if (m_queuePaused || m_queuedToasts.isEmpty()) {
        return;
    }

    // Try to show queued toasts
    while (!m_queuedToasts.isEmpty() && visibleCount() < m_config.maxVisible) {
        auto* toast = m_queuedToasts.dequeue();
        if (toast) {
            show(toast);
        }
    }

    emit queueProcessed();

    // Schedule next processing if there are still queued toasts
    if (!m_queuedToasts.isEmpty()) {
        m_queueTimer->start();
    }
}

void FluentToastManager::repositionToasts() {
    for (auto it = m_visibleToasts.begin(); it != m_visibleToasts.end(); ++it) {
        repositionToastsAt(it.key());
    }
}

void FluentToastManager::repositionToastsAt(FluentToastPosition position) {
    if (!m_visibleToasts.contains(position)) {
        return;
    }

    const auto& toastList = m_visibleToasts[position];

    for (int i = 0; i < toastList.size(); ++i) {
        auto* toast = toastList[i];
        if (toast) {
            const QRect newRect = getToastRect(position, toast->size(), i);
            animateToastToPosition(toast, newRect.topLeft());
        }
    }
}

// Private slots
void FluentToastManager::onToastShown() {
    auto* toast = qobject_cast<FluentToast*>(sender());
    if (toast) {
        emit toastShown(toast);
    }
}

void FluentToastManager::onToastHidden() {
    auto* toast = qobject_cast<FluentToast*>(sender());
    if (toast) {
        removeFromVisible(toast);
        cleanupToast(toast);
        emit toastHidden(toast);

        // Process queue to show next toast
        if (!m_queuedToasts.isEmpty()) {
            m_queueTimer->start();
        }
    }
}

void FluentToastManager::onToastDismissed() {
    auto* toast = qobject_cast<FluentToast*>(sender());
    if (toast) {
        emit toastDismissed(toast);
    }
}

void FluentToastManager::onScreenChanged() {
    updateScreenGeometry();
    repositionToasts();
}

void FluentToastManager::onParentWidgetDestroyed() {
    m_config.parentWidget = nullptr;
    updateScreenGeometry();
    repositionToasts();
}

// Private implementation methods
void FluentToastManager::setupConnections() {
    // Connect to screen changes
    if (auto* screen = QApplication::primaryScreen()) {
        connect(screen, &QScreen::geometryChanged, this, &FluentToastManager::onScreenChanged);
        connect(screen, &QScreen::availableGeometryChanged, this, &FluentToastManager::onScreenChanged);
    }
}

void FluentToastManager::connectToast(FluentToast* toast) {
    if (!toast) return;

    connect(toast, &FluentToast::shown, this, &FluentToastManager::onToastShown);
    connect(toast, &FluentToast::hidden, this, &FluentToastManager::onToastHidden);
    connect(toast, &FluentToast::dismissed, this, &FluentToastManager::onToastDismissed);
}

void FluentToastManager::disconnectToast(FluentToast* toast) {
    if (!toast) return;

    disconnect(toast, &FluentToast::shown, this, &FluentToastManager::onToastShown);
    disconnect(toast, &FluentToast::hidden, this, &FluentToastManager::onToastHidden);
    disconnect(toast, &FluentToast::dismissed, this, &FluentToastManager::onToastDismissed);
}

bool FluentToastManager::canShowToast(FluentToast* toast) const {
    Q_UNUSED(toast)
    return visibleCount() < m_config.maxVisible;
}

bool FluentToastManager::isDuplicate(FluentToast* toast) const {
    if (!toast || m_config.allowDuplicates) {
        return false;
    }

    // Check for duplicate title and message
    for (const auto& toastList : m_visibleToasts) {
        for (const auto* existingToast : toastList) {
            if (existingToast &&
                existingToast->title() == toast->title() &&
                existingToast->message() == toast->message()) {
                return true;
            }
        }
    }

    return false;
}

void FluentToastManager::addToQueue(FluentToast* toast) {
    if (!toast) return;

    // Remove oldest if queue is full
    while (m_queuedToasts.size() >= m_config.maxQueued && !m_queuedToasts.isEmpty()) {
        auto* oldToast = m_queuedToasts.dequeue();
        if (oldToast) {
            oldToast->deleteLater();
        }
    }

    m_queuedToasts.enqueue(toast);

    // Start processing if not paused
    if (!m_queuePaused) {
        m_queueTimer->start();
    }
}

void FluentToastManager::removeFromQueue(FluentToast* toast) {
    // Note: QQueue doesn't have a remove method, so we need to rebuild the queue
    QQueue<FluentToast*> newQueue;
    while (!m_queuedToasts.isEmpty()) {
        auto* queuedToast = m_queuedToasts.dequeue();
        if (queuedToast != toast) {
            newQueue.enqueue(queuedToast);
        }
    }
    m_queuedToasts = newQueue;
}

void FluentToastManager::showNextFromQueue() {
    if (!m_queuedToasts.isEmpty() && visibleCount() < m_config.maxVisible) {
        auto* toast = m_queuedToasts.dequeue();
        if (toast) {
            show(toast);
        }
    }
}

void FluentToastManager::addToVisible(FluentToast* toast, FluentToastPosition position) {
    if (!toast) return;

    if (!m_config.stackToasts) {
        // Hide existing toasts at this position
        hideAllAt(position);
    }

    // Add to visible list
    m_visibleToasts[position].append(toast);

    // Remove excess toasts if needed
    while (m_visibleToasts[position].size() > m_config.maxVisible) {
        auto* oldToast = m_visibleToasts[position].takeFirst();
        if (oldToast) {
            oldToast->hideAnimated();
        }
    }
}

void FluentToastManager::removeFromVisible(FluentToast* toast) {
    if (!toast) return;

    for (auto& toastList : m_visibleToasts) {
        toastList.removeAll(toast);
    }
}

void FluentToastManager::updateToastPositions(FluentToastPosition position) {
    repositionToastsAt(position);
}

void FluentToastManager::animateToastToPosition(FluentToast* toast, const QPoint& position) {
    if (!toast) {
        return;
    }

    // Always animate for now (could add a config option later)
    auto* animation = new QPropertyAnimation(toast, "pos");
    animation->setDuration(m_config.animationDuration);
    animation->setEasingCurve(m_config.easingCurve);
    animation->setStartValue(toast->pos());
    animation->setEndValue(position);

    connect(animation, &QPropertyAnimation::finished, animation, &QPropertyAnimation::deleteLater);
    animation->start();
}

QRect FluentToastManager::calculateToastRect(FluentToastPosition position, const QSize& toastSize, int stackIndex) const {
    const QPoint pos = calculateToastPosition(position, toastSize, stackIndex);
    return QRect(pos, toastSize);
}

QPoint FluentToastManager::calculateToastPosition(FluentToastPosition position, const QSize& toastSize, int stackIndex) const {
    const QRect area = getPositionArea(position);
    const int spacing = m_config.stackSpacing;

    QPoint basePos;

    switch (position) {
        case FluentToastPosition::TopLeft:
            basePos = area.topLeft();
            break;
        case FluentToastPosition::TopCenter:
            basePos = QPoint(area.center().x() - toastSize.width() / 2, area.top());
            break;
        case FluentToastPosition::TopRight:
            basePos = QPoint(area.right() - toastSize.width(), area.top());
            break;
        case FluentToastPosition::BottomLeft:
            basePos = QPoint(area.left(), area.bottom() - toastSize.height());
            break;
        case FluentToastPosition::BottomCenter:
            basePos = QPoint(area.center().x() - toastSize.width() / 2, area.bottom() - toastSize.height());
            break;
        case FluentToastPosition::BottomRight:
            basePos = QPoint(area.right() - toastSize.width(), area.bottom() - toastSize.height());
            break;
        case FluentToastPosition::Center:
            basePos = QPoint(area.center().x() - toastSize.width() / 2, area.center().y() - toastSize.height() / 2);
            break;
    }

    // Apply stack offset
    if (stackIndex > 0) {
        switch (position) {
            case FluentToastPosition::TopLeft:
            case FluentToastPosition::TopCenter:
            case FluentToastPosition::TopRight:
                basePos.ry() += stackIndex * (toastSize.height() + spacing);
                break;
            case FluentToastPosition::BottomLeft:
            case FluentToastPosition::BottomCenter:
            case FluentToastPosition::BottomRight:
                basePos.ry() -= stackIndex * (toastSize.height() + spacing);
                break;
            case FluentToastPosition::Center:
                basePos.ry() += stackIndex * (toastSize.height() + spacing);
                break;
        }
    }

    return basePos;
}

QRect FluentToastManager::getPositionArea(FluentToastPosition position) const {
    Q_UNUSED(position)

    QRect area = getScreenRect();
    area.adjust(m_config.screenMargin, m_config.screenMargin,
                -m_config.screenMargin, -m_config.screenMargin);
    return area;
}

void FluentToastManager::cleanupToast(FluentToast* toast) {
    if (!toast) return;

    disconnectToast(toast);
    removeFromQueue(toast);
    toast->deleteLater();
}

void FluentToastManager::updateScreenGeometry() {
    m_screenGeometry = getScreenRect();
    m_screenMargins = QMargins(m_config.screenMargin, m_config.screenMargin,
                              m_config.screenMargin, m_config.screenMargin);
}

// Global convenience functions
namespace FluentQt::Components::FluentToastGlobal {
    FluentToast* showInfo(const QString& title, const QString& message) {
        return FluentToastManager::instance().showInfo(title, message);
    }

    FluentToast* showSuccess(const QString& title, const QString& message) {
        return FluentToastManager::instance().showSuccess(title, message);
    }

    FluentToast* showWarning(const QString& title, const QString& message) {
        return FluentToastManager::instance().showWarning(title, message);
    }

    FluentToast* showError(const QString& title, const QString& message) {
        return FluentToastManager::instance().showError(title, message);
    }

    FluentToast* showCustom(const QIcon& icon, const QString& title, const QString& message) {
        return FluentToastManager::instance().showCustom(icon, title, message);
    }

    void hideAll() {
        FluentToastManager::instance().hideAll();
    }

    void clear() {
        FluentToastManager::instance().clear();
    }

    FluentToastManager& manager() {
        return FluentToastManager::instance();
    }
}
