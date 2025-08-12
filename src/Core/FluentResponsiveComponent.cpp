// src/Core/FluentResponsiveComponent.cpp
#include "FluentQt/Core/FluentResponsiveComponent.h"
#include <QApplication>
#include <QDebug>
#include <QMutexLocker>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QStyle>
#include <QTimer>
#include <algorithm>
#include "FluentQt/Core/FluentResponsive.h"

namespace FluentQt::Core {

FluentResponsiveComponentManager& FluentResponsiveComponentManager::instance() {
    static FluentResponsiveComponentManager instance;
    return instance;
}

FluentResponsiveComponentManager::FluentResponsiveComponentManager() {
    // Initialize update timers
    m_globalUpdateTimer = new QTimer(this);
    m_globalUpdateTimer->setSingleShot(true);
    connect(m_globalUpdateTimer, &QTimer::timeout, this,
            &FluentResponsiveComponentManager::onUpdateTimer);

    m_performanceTimer = new QTimer(this);
    m_performanceTimer->setInterval(5000);  // Check performance every 5 seconds
    connect(m_performanceTimer, &QTimer::timeout, this,
            &FluentResponsiveComponentManager::onPerformanceTimer);
    m_performanceTimer->start();

    // Connect to responsive manager
    auto& responsiveManager = FluentResponsiveManager::instance();
    connect(&responsiveManager, &FluentResponsiveManager::breakpointChanged,
            this, &FluentResponsiveComponentManager::onBreakpointChanged);

    qDebug() << "FluentResponsiveComponentManager initialized";
}

void FluentResponsiveComponentManager::registerComponent(
    QWidget* component, const FluentResponsiveComponentConfig& config) {
    if (!component)
        return;

    QMutexLocker locker(&m_componentsMutex);

    m_components[component] = config;

    // Initialize component state
    FluentComponentState initialState;
    initialState.breakpoint = m_currentBreakpoint;
    initialState.variant = config.defaultBehavior.variant;
    initialState.size = component->size();
    initialState.visible = component->isVisible();
    initialState.opacity = 1.0;
    m_currentStates[component] = initialState;

    // Initialize metrics
    FluentComponentMetrics metrics;
    metrics.renderSize = component->size();
    metrics.isVisible = component->isVisible();
    metrics.lastUpdate = std::chrono::steady_clock::now();
    m_metrics[component] = metrics;

    // Connect to component destruction
    connect(component, &QObject::destroyed, this,
            &FluentResponsiveComponentManager::onComponentDestroyed);

    // Apply initial state
    locker.unlock();
    updateComponentState(component, m_currentBreakpoint);

    emit componentRegistered(component);
}

void FluentResponsiveComponentManager::unregisterComponent(QWidget* component) {
    if (!component)
        return;

    QMutexLocker locker(&m_componentsMutex);

    // Clean up timers
    auto timerIt = m_updateTimers.find(component);
    if (timerIt != m_updateTimers.end()) {
        timerIt.value()->deleteLater();
        m_updateTimers.erase(timerIt);
    }

    // Clean up animations
    auto animIt = m_activeAnimations.find(component);
    if (animIt != m_activeAnimations.end()) {
        animIt.value()->deleteLater();
        m_activeAnimations.erase(animIt);
    }

    // Remove from all maps
    m_components.remove(component);
    m_currentStates.remove(component);
    m_metrics.remove(component);
    m_containerBreakpoints.remove(component);
    m_containerQueryClasses.remove(component);

    emit componentUnregistered(component);
}

bool FluentResponsiveComponentManager::isComponentRegistered(
    QWidget* component) const {
    QMutexLocker locker(&m_componentsMutex);
    return m_components.find(component) != m_components.end();
}

void FluentResponsiveComponentManager::setComponentConfig(
    QWidget* component, const FluentResponsiveComponentConfig& config) {
    if (!component)
        return;

    QMutexLocker locker(&m_componentsMutex);
    auto it = m_components.find(component);
    if (it != m_components.end()) {
        it.value() = config;
        locker.unlock();
        updateComponentState(component, m_currentBreakpoint);
    }
}

FluentResponsiveComponentConfig
FluentResponsiveComponentManager::getComponentConfig(QWidget* component) const {
    QMutexLocker locker(&m_componentsMutex);
    auto it = m_components.find(component);
    return (it != m_components.end()) ? it.value()
                                      : FluentResponsiveComponentConfig{};
}

void FluentResponsiveComponentManager::updateComponentState(
    QWidget* component, FluentBreakpoint breakpoint) {
    if (!component)
        return;

    QMutexLocker locker(&m_componentsMutex);

    auto configIt = m_components.find(component);
    if (configIt == m_components.end())
        return;

    const FluentResponsiveComponentConfig& config = configIt.value();

    // Find state for this breakpoint
    FluentComponentState targetState;
    auto stateIt = config.states.find(breakpoint);
    if (stateIt != config.states.end()) {
        targetState = stateIt.value();
    } else {
        // Use default state
        targetState.breakpoint = breakpoint;
        targetState.variant = config.defaultBehavior.variant;
        targetState.size = component->size();
        targetState.visible = true;
        targetState.opacity = 1.0;
    }

    // Get current state
    auto currentStateIt = m_currentStates.find(component);
    FluentComponentState currentState;
    if (currentStateIt != m_currentStates.end()) {
        currentState = currentStateIt.value();
    }

    // Update current state
    m_currentStates[component] = targetState;

    locker.unlock();

    // Apply the state
    if (config.defaultBehavior.enableTransitions &&
        !config.defaultBehavior.respectReducedMotion) {
        transitionToState(component, currentState, targetState);
    } else {
        applyComponentState(component, targetState);
    }

    emit componentStateChanged(component, breakpoint);
}

void FluentResponsiveComponentManager::setComponentVariant(
    QWidget* component, FluentComponentVariant variant) {
    if (!component)
        return;

    QMutexLocker locker(&m_componentsMutex);

    auto configIt = m_components.find(component);
    if (configIt != m_components.end()) {
        configIt.value().defaultBehavior.variant = variant;

        auto stateIt = m_currentStates.find(component);
        if (stateIt != m_currentStates.end()) {
            stateIt.value().variant = variant;
        }

        locker.unlock();

        // Apply variant
        switch (variant) {
            case FluentComponentVariant::Compact:
                applyCompactVariant(component);
                break;
            case FluentComponentVariant::Comfortable:
                applyComfortableVariant(component);
                break;
            case FluentComponentVariant::Spacious:
                applySpaciousVariant(component);
                break;
            case FluentComponentVariant::Touch:
                applyTouchVariant(component);
                break;
            case FluentComponentVariant::Dense:
                applyDenseVariant(component);
                break;
            case FluentComponentVariant::Accessible:
                applyAccessibleVariant(component);
                break;
            case FluentComponentVariant::Custom:
                applyCustomVariant(
                    component,
                    configIt.value().defaultBehavior.customProperties);
                break;
        }

        emit componentVariantChanged(component, variant);
    }
}

FluentComponentVariant FluentResponsiveComponentManager::getComponentVariant(
    QWidget* component) const {
    QMutexLocker locker(&m_componentsMutex);

    auto stateIt = m_currentStates.find(component);
    if (stateIt != m_currentStates.end()) {
        return stateIt.value().variant;
    }

    return FluentComponentVariant::Comfortable;
}

void FluentResponsiveComponentManager::setAdaptationStrategy(
    QWidget* component, FluentAdaptationStrategy strategy) {
    if (!component)
        return;

    QMutexLocker locker(&m_componentsMutex);

    auto configIt = m_components.find(component);
    if (configIt != m_components.end()) {
        configIt.value().defaultBehavior.strategy = strategy;

        auto stateIt = m_currentStates.find(component);
        if (stateIt != m_currentStates.end()) {
            stateIt.value().adaptationStrategy = strategy;
        }
    }
}

FluentAdaptationStrategy
FluentResponsiveComponentManager::getAdaptationStrategy(
    QWidget* component) const {
    QMutexLocker locker(&m_componentsMutex);

    auto stateIt = m_currentStates.find(component);
    if (stateIt != m_currentStates.end()) {
        return stateIt.value().adaptationStrategy;
    }

    return FluentAdaptationStrategy::Wrap;
}

void FluentResponsiveComponentManager::applyAdaptationStrategy(
    QWidget* component, FluentBreakpoint breakpoint) {
    if (!component)
        return;

    FluentAdaptationStrategy strategy = getAdaptationStrategy(component);

    // Adjust strategy based on breakpoint
    if (breakpoint == FluentBreakpoint::XSmall ||
        breakpoint == FluentBreakpoint::Small) {
        // For smaller screens, prefer more aggressive strategies
        if (strategy == FluentAdaptationStrategy::Wrap) {
            strategy = FluentAdaptationStrategy::Stack;
        }
    }

    switch (strategy) {
        case FluentAdaptationStrategy::Hide:
            applyHideStrategy(component);
            break;
        case FluentAdaptationStrategy::Collapse:
            applyCollapseStrategy(component);
            break;
        case FluentAdaptationStrategy::Stack:
            applyStackStrategy(component);
            break;
        case FluentAdaptationStrategy::Wrap:
            applyWrapStrategy(component);
            break;
        case FluentAdaptationStrategy::Scroll:
            applyScrollStrategy(component);
            break;
        case FluentAdaptationStrategy::Paginate:
            applyPaginateStrategy(component);
            break;
        case FluentAdaptationStrategy::Summarize:
            applySummarizeStrategy(component);
            break;
        case FluentAdaptationStrategy::Transform:
            applyTransformStrategy(component);
            break;
    }

    emit adaptationApplied(component, strategy);
}

void FluentResponsiveComponentManager::enablePerformanceOptimization(
    QWidget* component, bool enabled) {
    if (!component)
        return;

    QMutexLocker locker(&m_componentsMutex);

    auto configIt = m_components.find(component);
    if (configIt != m_components.end()) {
        configIt.value().enablePerformanceOptimization = enabled;

        if (enabled) {
            // Start performance monitoring for this component
            updateComponentMetrics(component);
        }
    }
}

FluentComponentMetrics FluentResponsiveComponentManager::getComponentMetrics(
    QWidget* component) const {
    QMutexLocker locker(&m_metricsMutex);

    auto it = m_metrics.find(component);
    return (it != m_metrics.end()) ? it.value() : FluentComponentMetrics{};
}

void FluentResponsiveComponentManager::optimizeComponent(QWidget* component) {
    if (!component)
        return;

    auto metrics = getComponentMetrics(component);
    double initialScore = metrics.performanceScore;

    // Apply optimizations based on metrics
    if (metrics.renderTime > 16) {  // > 60fps
        // Reduce update frequency
        setUpdateThrottle(component, 32);  // 30fps
    }

    if (metrics.memoryUsage > 50.0) {  // > 50MB
        // Enable lazy loading or reduce memory usage
        enableContainerQueries(component, false);
    }

    if (!metrics.isInViewport) {
        // Reduce updates for off-screen components
        setUpdateThrottle(component, 100);  // 10fps
    }

    // Update metrics after optimization
    updateComponentMetrics(component);
    auto newMetrics = getComponentMetrics(component);

    double improvement = newMetrics.performanceScore - initialScore;
    if (improvement > 0) {
        emit performanceOptimized(component, improvement);
    }
}

QSize FluentResponsiveComponentManager::calculateOptimalSize(
    QWidget* component, FluentBreakpoint breakpoint) const {
    if (!component)
        return QSize();

    QMutexLocker locker(&m_componentsMutex);

    auto configIt = m_components.find(component);
    if (configIt == m_components.end()) {
        return component->sizeHint();
    }

    const FluentResponsiveComponentConfig& config = configIt.value();

    // Check if there's a specific size for this breakpoint
    auto stateIt = config.states.find(breakpoint);
    if (stateIt != config.states.end() && !stateIt.value().size.isEmpty()) {
        return stateIt.value().size;
    }

    // Calculate based on variant
    QSize baseSize = component->sizeHint();
    FluentComponentVariant variant = config.defaultBehavior.variant;

    switch (variant) {
        case FluentComponentVariant::Compact:
            return QSize(baseSize.width() * 0.8, baseSize.height() * 0.8);
        case FluentComponentVariant::Comfortable:
            return baseSize;
        case FluentComponentVariant::Spacious:
            return QSize(baseSize.width() * 1.2, baseSize.height() * 1.2);
        case FluentComponentVariant::Touch:
            return QSize(std::max(baseSize.width(), 44),
                         std::max(baseSize.height(), 44));
        case FluentComponentVariant::Dense:
            return QSize(baseSize.width() * 0.7, baseSize.height() * 0.7);
        case FluentComponentVariant::Accessible:
            return QSize(baseSize.width() * 1.1, baseSize.height() * 1.1);
        default:
            return baseSize;
    }
}

void FluentResponsiveComponentManager::updateAllComponents() {
    QMutexLocker locker(&m_componentsMutex);

    for (auto it = m_components.begin(); it != m_components.end(); ++it) {
        QWidget* component = it.key();
        if (component && component->isVisible()) {
            locker.unlock();
            updateComponentState(component, m_currentBreakpoint);
            locker.relock();
        }
    }
}

// Private implementation methods
void FluentResponsiveComponentManager::applyComponentState(
    QWidget* component, const FluentComponentState& state) {
    if (!component)
        return;

    // Apply size
    if (!state.size.isEmpty()) {
        component->resize(state.size);
    }

    // Apply visibility
    component->setVisible(state.visible);

    // Apply opacity (if supported)
    if (component->graphicsEffect()) {
        // Would set opacity through graphics effect
    }

    // Apply style class
    if (!state.styleClass.isEmpty()) {
        component->setProperty("class", state.styleClass);
        component->style()->polish(component);
    }

    // Apply margins (if parent layout supports it)
    if (component->parentWidget() && component->parentWidget()->layout()) {
        // Would set margins through layout
    }
}

void FluentResponsiveComponentManager::transitionToState(
    QWidget* component, const FluentComponentState& fromState,
    const FluentComponentState& toState) {
    if (!component)
        return;

    // Create animation group
    auto* animationGroup = new QParallelAnimationGroup(this);

    // Animate size change
    if (fromState.size != toState.size && !toState.size.isEmpty()) {
        auto* sizeAnimation = new QPropertyAnimation(component, "size");
        sizeAnimation->setDuration(200);
        sizeAnimation->setStartValue(fromState.size);
        sizeAnimation->setEndValue(toState.size);
        sizeAnimation->setEasingCurve(QEasingCurve::OutCubic);
        animationGroup->addAnimation(sizeAnimation);
    }

    // Animate opacity change
    if (std::abs(fromState.opacity - toState.opacity) > 0.01) {
        // Would animate opacity if supported
    }

    // Clean up previous animation
    auto animIt = m_activeAnimations.find(component);
    if (animIt != m_activeAnimations.end()) {
        animIt.value()->stop();
        animIt.value()->deleteLater();
    }

    m_activeAnimations[component] = animationGroup;

    connect(animationGroup, &QParallelAnimationGroup::finished, this,
            [this, component, toState]() {
                applyComponentState(component, toState);
                m_activeAnimations.remove(component);
            });

    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void FluentResponsiveComponentManager::updateComponentMetrics(
    QWidget* component) {
    if (!component)
        return;

    QMutexLocker locker(&m_metricsMutex);

    auto& metrics = m_metrics[component];
    metrics.renderSize = component->size();
    metrics.isVisible = component->isVisible();
    metrics.lastUpdate = std::chrono::steady_clock::now();
    metrics.updateCount++;

    // Calculate performance score based on various factors
    double score = 100.0;

    if (metrics.renderTime > 16) {
        score -= (metrics.renderTime - 16) * 2.0;  // Penalize slow rendering
    }

    if (metrics.memoryUsage > 10.0) {
        score -= metrics.memoryUsage;  // Penalize high memory usage
    }

    if (metrics.updateCount > 60) {
        score -= 10.0;  // Penalize frequent updates
    }

    metrics.performanceScore = std::max(0.0, std::min(100.0, score));
}

// Variant implementation methods
void FluentResponsiveComponentManager::applyCompactVariant(QWidget* component) {
    if (!component)
        return;

    // Reduce spacing and padding
    component->setContentsMargins(4, 4, 4, 4);

    // Apply compact styling
    component->setProperty("variant", "compact");
    component->style()->polish(component);
}

void FluentResponsiveComponentManager::applyComfortableVariant(
    QWidget* component) {
    if (!component)
        return;

    // Standard spacing and padding
    component->setContentsMargins(8, 8, 8, 8);

    // Apply comfortable styling
    component->setProperty("variant", "comfortable");
    component->style()->polish(component);
}

void FluentResponsiveComponentManager::applySpaciousVariant(
    QWidget* component) {
    if (!component)
        return;

    // Increased spacing and padding
    component->setContentsMargins(16, 16, 16, 16);

    // Apply spacious styling
    component->setProperty("variant", "spacious");
    component->style()->polish(component);
}

void FluentResponsiveComponentManager::applyTouchVariant(QWidget* component) {
    if (!component)
        return;

    // Ensure minimum touch target size
    QSize currentSize = component->size();
    QSize minTouchSize(44, 44);

    if (currentSize.width() < minTouchSize.width() ||
        currentSize.height() < minTouchSize.height()) {
        component->resize(minTouchSize.expandedTo(currentSize));
    }

    // Apply touch-friendly styling
    component->setProperty("variant", "touch");
    component->style()->polish(component);
}

void FluentResponsiveComponentManager::applyDenseVariant(QWidget* component) {
    if (!component)
        return;

    // Minimal spacing and padding
    component->setContentsMargins(2, 2, 2, 2);

    // Apply dense styling
    component->setProperty("variant", "dense");
    component->style()->polish(component);
}

void FluentResponsiveComponentManager::applyAccessibleVariant(
    QWidget* component) {
    if (!component)
        return;

    // Enhanced accessibility features
    component->setContentsMargins(12, 12, 12, 12);

    // Ensure proper focus indicators
    component->setFocusPolicy(Qt::TabFocus);

    // Apply accessible styling
    component->setProperty("variant", "accessible");
    component->style()->polish(component);
}

void FluentResponsiveComponentManager::applyCustomVariant(
    QWidget* component, const QMap<QString, QVariant>& properties) {
    if (!component)
        return;

    // Apply custom properties
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        component->setProperty(it.key().toUtf8().constData(), it.value());
    }

    component->style()->polish(component);
}

// Adaptation strategy implementations
void FluentResponsiveComponentManager::applyHideStrategy(QWidget* component) {
    if (component) {
        component->hide();
    }
}

void FluentResponsiveComponentManager::applyCollapseStrategy(
    QWidget* component) {
    if (component) {
        component->resize(0, 0);
    }
}

void FluentResponsiveComponentManager::applyStackStrategy(QWidget* component) {
    // This would typically involve changing the layout of the parent container
    // to stack items vertically instead of horizontally
    Q_UNUSED(component)
}

void FluentResponsiveComponentManager::applyWrapStrategy(QWidget* component) {
    // This would typically involve enabling text wrapping or layout wrapping
    Q_UNUSED(component)
}

void FluentResponsiveComponentManager::applyScrollStrategy(QWidget* component) {
    // This would typically involve adding scroll bars or enabling scrolling
    Q_UNUSED(component)
}

void FluentResponsiveComponentManager::applyPaginateStrategy(
    QWidget* component) {
    // This would typically involve splitting content into pages
    Q_UNUSED(component)
}

void FluentResponsiveComponentManager::applySummarizeStrategy(
    QWidget* component) {
    // This would typically involve showing a summary or preview of content
    Q_UNUSED(component)
}

void FluentResponsiveComponentManager::applyTransformStrategy(
    QWidget* component) {
    // This would typically involve transforming the component to a different
    // type
    Q_UNUSED(component)
}

// Slot implementations
void FluentResponsiveComponentManager::onBreakpointChanged(
    FluentBreakpoint breakpoint) {
    m_currentBreakpoint = breakpoint;
    updateComponentsForBreakpoint(breakpoint);
}

void FluentResponsiveComponentManager::onComponentDestroyed(
    QObject* component) {
    QWidget* widget = qobject_cast<QWidget*>(component);
    if (widget) {
        unregisterComponent(widget);
    }
}

void FluentResponsiveComponentManager::onUpdateTimer() {
    // Process any pending updates
    updateAllComponents();
}

void FluentResponsiveComponentManager::onPerformanceTimer() {
    if (m_performanceOptimizationEnabled) {
        // Update metrics for all components
        QMutexLocker locker(&m_componentsMutex);
        for (auto it = m_components.begin(); it != m_components.end(); ++it) {
            QWidget* component = it.key();
            const FluentResponsiveComponentConfig& config = it.value();
            if (component && config.enablePerformanceOptimization) {
                locker.unlock();
                updateComponentMetrics(component);
                locker.relock();
            }
        }
    }
}

void FluentResponsiveComponentManager::updateComponentsForBreakpoint(
    FluentBreakpoint breakpoint) {
    QMutexLocker locker(&m_componentsMutex);

    for (auto it = m_components.begin(); it != m_components.end(); ++it) {
        QWidget* component = it.key();
        if (component && component->isVisible()) {
            locker.unlock();
            updateComponentState(component, breakpoint);
            locker.relock();
        }
    }
}

void FluentResponsiveComponentManager::setUpdateThrottle(QWidget* component,
                                                         int milliseconds) {
    if (!component)
        return;

    auto timerIt = m_updateTimers.find(component);
    if (timerIt != m_updateTimers.end()) {
        timerIt.value()->setInterval(milliseconds);
    } else {
        auto* timer = new QTimer(this);
        timer->setInterval(milliseconds);
        timer->setSingleShot(true);
        m_updateTimers[component] = timer;

        connect(timer, &QTimer::timeout, this, [this, component]() {
            updateComponentState(component, m_currentBreakpoint);
        });
    }
}

void FluentResponsiveComponentManager::enableContainerQueries(
    QWidget* component, bool enabled) {
    if (!component)
        return;

    QMutexLocker locker(&m_componentsMutex);

    auto configIt = m_components.find(component);
    if (configIt != m_components.end()) {
        configIt.value().enableContainerQueries = enabled;

        if (enabled) {
            // Initialize container query tracking
            m_containerBreakpoints[component] = QMap<QString, QSize>();
            m_containerQueryClasses[component] = QString();
        } else {
            // Clean up container query data
            m_containerBreakpoints.remove(component);
            m_containerQueryClasses.remove(component);
        }
    }
}

// FluentResponsiveWidget implementation
FluentResponsiveWidget::FluentResponsiveWidget(QWidget* parent)
    : QWidget(parent) {
    initializeResponsive();
}

FluentResponsiveWidget::~FluentResponsiveWidget() {
    // Unregister from responsive component manager
    auto& manager = FluentResponsiveComponentManager::instance();
    manager.unregisterComponent(this);
}

void FluentResponsiveWidget::onResponsiveUpdate() { updateResponsiveState(); }

void FluentResponsiveWidget::initializeResponsive() {
    // Register with responsive component manager
    auto& manager = FluentResponsiveComponentManager::instance();
    FluentResponsiveComponentConfig config;
    config.defaultBehavior.variant = FluentComponentVariant::Comfortable;
    config.defaultBehavior.strategy = FluentAdaptationStrategy::Wrap;
    config.enablePerformanceOptimization = true;
    manager.registerComponent(this, config);
}

void FluentResponsiveWidget::updateResponsiveState() {
    // Update component state based on current conditions
    auto& manager = FluentResponsiveComponentManager::instance();
    auto& responsiveManager = FluentResponsiveManager::instance();
    FluentBreakpoint currentBreakpoint =
        responsiveManager.getCurrentBreakpoint();
    manager.updateComponentState(this, currentBreakpoint);
}

// Virtual method implementations
void FluentResponsiveWidget::onBreakpointChanged(FluentBreakpoint breakpoint) {
    // Default implementation - subclasses can override
    Q_UNUSED(breakpoint);
    updateResponsiveState();
}

void FluentResponsiveWidget::onVariantChanged(FluentComponentVariant variant) {
    // Default implementation - subclasses can override
    Q_UNUSED(variant);
}

void FluentResponsiveWidget::onAdaptationApplied(
    FluentAdaptationStrategy strategy) {
    // Default implementation - subclasses can override
    Q_UNUSED(strategy);
}

QSize FluentResponsiveWidget::calculateOptimalSize(
    FluentBreakpoint breakpoint) const {
    // Default implementation - calculate based on breakpoint
    QSize baseSize = sizeHint();

    switch (breakpoint) {
        case FluentBreakpoint::XSmall:
            return QSize(baseSize.width() * 0.7, baseSize.height() * 0.9);
        case FluentBreakpoint::Small:
            return QSize(baseSize.width() * 0.8, baseSize.height() * 0.95);
        case FluentBreakpoint::Medium:
            return baseSize;
        case FluentBreakpoint::Large:
            return QSize(baseSize.width() * 1.1, baseSize.height() * 1.05);
        case FluentBreakpoint::XLarge:
            return QSize(baseSize.width() * 1.2, baseSize.height() * 1.1);
        case FluentBreakpoint::XXLarge:
            return QSize(baseSize.width() * 1.3, baseSize.height() * 1.15);
        default:
            return baseSize;
    }
}

// Event handling implementations
void FluentResponsiveWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateResponsiveState();
}

void FluentResponsiveWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    updateResponsiveState();
}

void FluentResponsiveWidget::hideEvent(QHideEvent* event) {
    QWidget::hideEvent(event);
    updateResponsiveState();
}

}  // namespace FluentQt::Core
