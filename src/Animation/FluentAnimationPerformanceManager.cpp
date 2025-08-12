// src/Animation/FluentAnimationPerformanceManager.cpp
#include "FluentQt/Animation/FluentAnimationPerformanceManager.h"
#include <QApplication>
#include <QDebug>
#include <QMutexLocker>
#include <QScreen>
#include <QSettings>
#include <QThread>
#include <algorithm>
#include <chrono>
#include "FluentQt/Core/FluentPerformance.h"  // added for FLUENT_PROFILE macros

namespace FluentQt::Animation {

FluentAnimationPerformanceManager&
FluentAnimationPerformanceManager::instance() {
    static FluentAnimationPerformanceManager instance;
    return instance;
}

FluentAnimationPerformanceManager::FluentAnimationPerformanceManager() {
    // Initialize timers
    m_performanceTimer = new QTimer(this);
    m_performanceTimer->setSingleShot(false);
    connect(m_performanceTimer, &QTimer::timeout, this,
            &FluentAnimationPerformanceManager::onPerformanceTimer);

    m_optimizationTimer = new QTimer(this);
    m_optimizationTimer->setSingleShot(false);
    connect(m_optimizationTimer, &QTimer::timeout, this,
            &FluentAnimationPerformanceManager::onOptimizationTimer);

    // Initialize default configuration
    m_config.targetPerformance = FluentAnimationPerformance::Adaptive;
    m_config.targetFPS = 60.0;
    m_config.minFPS = 30.0;
    m_config.maxCPUUsage = 70.0;
    m_config.maxMemoryUsage = 100.0;
    m_config.maxConcurrentAnimations = 20;
    m_config.enableHardwareAcceleration = true;
    m_config.enablePerformanceMonitoring = true;
    m_config.enableAdaptiveOptimization = true;

    // Initialize default optimization strategies
    m_enabledStrategies.insert(FluentOptimizationStrategy::BatchAnimations);
    m_enabledStrategies.insert(FluentOptimizationStrategy::PrioritizeVisible);
    m_enabledStrategies.insert(FluentOptimizationStrategy::AdaptiveFrameRate);

    // Detect hardware capabilities
    detectHardwareCapabilities();

    // Connect to system signals
    connectToSystemSignals();

    qDebug() << "FluentAnimationPerformanceManager initialized";
}

void FluentAnimationPerformanceManager::setPerformanceConfig(
    const FluentPerformanceConfig& config) {
    QMutexLocker locker(&m_configMutex);
    m_config = config;

    if (config.enablePerformanceMonitoring && !m_performanceMonitoringEnabled) {
        startPerformanceMonitoring();
    } else if (!config.enablePerformanceMonitoring &&
               m_performanceMonitoringEnabled) {
        stopPerformanceMonitoring();
    }
}

void FluentAnimationPerformanceManager::setTargetPerformance(
    FluentAnimationPerformance performance) {
    QMutexLocker locker(&m_configMutex);
    m_config.targetPerformance = performance;

    // Apply immediate optimizations based on performance level
    switch (performance) {
        case FluentAnimationPerformance::High:
            m_config.targetFPS = 60.0;
            m_config.maxConcurrentAnimations = 30;
            break;
        case FluentAnimationPerformance::Medium:
            m_config.targetFPS = 45.0;
            m_config.maxConcurrentAnimations = 20;
            break;
        case FluentAnimationPerformance::Low:
            m_config.targetFPS = 30.0;
            m_config.maxConcurrentAnimations = 10;
            break;
        case FluentAnimationPerformance::PowerSaver:
            m_config.targetFPS = 24.0;
            m_config.maxConcurrentAnimations = 5;
            break;
        case FluentAnimationPerformance::Accessibility:
            m_config.targetFPS = 30.0;
            m_config.maxConcurrentAnimations = 15;
            break;
        default:
            break;
    }

    optimizePerformance();
}

void FluentAnimationPerformanceManager::startPerformanceMonitoring() {
    if (m_performanceMonitoringEnabled) {
        return;
    }

    m_performanceMonitoringEnabled = true;
    m_frameTimer.start();
    m_performanceTimer->start(100);    // Update every 100ms
    m_optimizationTimer->start(1000);  // Optimize every second

    qDebug() << "Performance monitoring started";
}

void FluentAnimationPerformanceManager::stopPerformanceMonitoring() {
    if (!m_performanceMonitoringEnabled) {
        return;
    }

    m_performanceMonitoringEnabled = false;
    m_performanceTimer->stop();
    m_optimizationTimer->stop();

    qDebug() << "Performance monitoring stopped";
}

FluentAnimationMetrics FluentAnimationPerformanceManager::getCurrentMetrics()
    const {
    QMutexLocker locker(&m_metricsMutex);
    return m_currentMetrics;
}

FluentAnimationMetrics FluentAnimationPerformanceManager::getAverageMetrics(
    std::chrono::seconds duration) const {
    QMutexLocker locker(&m_metricsMutex);

    FluentAnimationMetrics averageMetrics;
    if (m_metricsHistory.isEmpty()) {
        return averageMetrics;
    }

    auto cutoffTime = std::chrono::steady_clock::now() - duration;
    double totalFPS = 0.0;
    double totalFrameTime = 0.0;
    int totalDroppedFrames = 0;
    int totalFrames = 0;
    int count = 0;

    for (const auto& metrics : m_metricsHistory) {
        if (metrics.lastUpdate >= cutoffTime) {
            totalFPS += metrics.averageFPS;
            totalFrameTime += metrics.frameTime;
            totalDroppedFrames += metrics.droppedFrames;
            totalFrames += metrics.totalFrames;
            count++;
        }
    }

    if (count > 0) {
        averageMetrics.averageFPS = totalFPS / count;
        averageMetrics.frameTime = totalFrameTime / count;
        averageMetrics.droppedFrames = totalDroppedFrames;
        averageMetrics.totalFrames = totalFrames;
        averageMetrics.performanceScore = calculatePerformanceScore();
    }

    return averageMetrics;
}

int FluentAnimationPerformanceManager::requestAnimation(
    const FluentAnimationRequest& request) {
    QMutexLocker locker(&m_queueMutex);

    int requestId = m_nextRequestId++;
    m_animationRequests[requestId] = request;

    // Check if we should skip this animation
    if (shouldSkipAnimation(request)) {
        emit animationSkipped(requestId, "Performance optimization");
        if (request.onSkipped) {
            request.onSkipped();
        }
        return requestId;
    }

    // Add to queue or execute immediately
    if (m_activeAnimations.size() < m_config.maxConcurrentAnimations) {
        executeAnimation(request);
        m_activeAnimations.insert(requestId);
    } else {
        m_animationQueue.enqueue(requestId);
    }

    return requestId;
}

void FluentAnimationPerformanceManager::optimizePerformance() {
    if (!m_config.enableAdaptiveOptimization) {
        return;
    }

    auto metrics = getCurrentMetrics();

    // Only optimize if performance is below threshold
    if (metrics.frameTime > 16.67) {  // 60 FPS threshold
        // Apply enabled optimization strategies
        for (auto strategy : m_enabledStrategies) {
            switch (strategy) {
                case FluentOptimizationStrategy::ReduceQuality:
                    applyReduceQualityStrategy();
                    break;
                case FluentOptimizationStrategy::SkipFrames:
                    applySkipFramesStrategy();
                    break;
                case FluentOptimizationStrategy::SimplifyEffects:
                    applySimplifyEffectsStrategy();
                    break;
                case FluentOptimizationStrategy::BatchAnimations:
                    applyBatchAnimationsStrategy();
                    break;
                case FluentOptimizationStrategy::ReduceDuration:
                    applyReduceDurationStrategy();
                    break;
                case FluentOptimizationStrategy::PrioritizeVisible:
                    applyPrioritizeVisibleStrategy();
                    break;
                case FluentOptimizationStrategy::AdaptiveFrameRate:
                    applyAdaptiveFrameRateStrategy();
                    break;
                default:
                    break;
            }
        }
    }

    emit performanceOptimized(FluentOptimizationStrategy::None);
}

void FluentAnimationPerformanceManager::setCulturalAnimationStyle(
    FluentCulturalAnimationStyle style) {
    m_culturalStyle = style;
    applyCulturalAnimationStyle();
    emit culturalStyleChanged(style);
}

void FluentAnimationPerformanceManager::setAccessibilityAnimationMode(
    FluentAccessibilityAnimationMode mode) {
    m_accessibilityMode = mode;
    applyAccessibilityAnimationMode();
    emit accessibilityModeChanged(mode);
}

void FluentAnimationPerformanceManager::adaptAnimationsForCulture(
    const QLocale& locale) {
    // Adapt animation style based on cultural preferences
    if (locale.language() == QLocale::Chinese ||
        locale.language() == QLocale::Japanese) {
        setCulturalAnimationStyle(FluentCulturalAnimationStyle::Eastern);
    } else if (locale.language() == QLocale::German ||
               locale.language() == QLocale::English) {
        setCulturalAnimationStyle(FluentCulturalAnimationStyle::Western);
    } else {
        setCulturalAnimationStyle(FluentCulturalAnimationStyle::Adaptive);
    }
}

void FluentAnimationPerformanceManager::adaptAnimationsForAccessibility() {
    // Check system accessibility preferences
    if (isReducedMotionPreferred()) {
        setAccessibilityAnimationMode(
            FluentAccessibilityAnimationMode::Reduced);
    } else {
        setAccessibilityAnimationMode(FluentAccessibilityAnimationMode::Full);
    }
}

double FluentAnimationPerformanceManager::calculatePerformanceScore() const {
    auto metrics = getCurrentMetrics();

    // Calculate score based on FPS, frame time, and dropped frames
    double fpsScore =
        std::min(100.0, (metrics.averageFPS / m_config.targetFPS) * 100.0);
    double frameTimeScore =
        std::max(0.0, 100.0 - (metrics.frameTime - 16.67) * 2.0);
    double droppedFrameScore =
        std::max(0.0, 100.0 - (metrics.droppedFrames * 5.0));

    return (fpsScore + frameTimeScore + droppedFrameScore) / 3.0;
}

QStringList FluentAnimationPerformanceManager::getPerformanceRecommendations()
    const {
    QStringList recommendations;
    auto metrics = getCurrentMetrics();

    if (metrics.averageFPS < m_config.minFPS) {
        recommendations << "Consider reducing animation quality or duration";
    }

    if (metrics.droppedFrames > 5) {
        recommendations << "Enable frame skipping optimization";
    }

    if (metrics.activeAnimations > m_config.maxConcurrentAnimations) {
        recommendations << "Reduce concurrent animations";
    }

    if (m_systemPowerSaving) {
        recommendations << "Enable power saving mode";
    }

    if (recommendations.isEmpty()) {
        recommendations << "Performance is optimal";
    }

    return recommendations;
}

// Private slot implementations
void FluentAnimationPerformanceManager::onPerformanceTimer() {
    updateMetrics();

    if (m_config.enableAdaptiveOptimization) {
        updatePerformanceBasedOnMetrics();
    }
}

void FluentAnimationPerformanceManager::onOptimizationTimer() {
    optimizePerformance();
    processAnimationQueue();
}

// Private helper methods
void FluentAnimationPerformanceManager::updateMetrics() {
    QMutexLocker locker(&m_metricsMutex);

    // Calculate frame rate
    calculateFrameRate();

    // Monitor system resources
    monitorSystemResources();

    // Update metrics
    m_currentMetrics.activeAnimations = m_activeAnimations.size();
    m_currentMetrics.queuedAnimations = m_animationQueue.size();
    m_currentMetrics.lastUpdate = std::chrono::steady_clock::now();
    m_currentMetrics.performanceScore = calculatePerformanceScore();

    // Add to history
    m_metricsHistory.enqueue(m_currentMetrics);
    if (m_metricsHistory.size() > 100) {  // Keep last 100 entries
        m_metricsHistory.dequeue();
    }

    emit performanceChanged(m_currentMetrics);
}

void FluentAnimationPerformanceManager::calculateFrameRate() {
    m_frameCount++;

    qint64 elapsed = m_frameTimer.elapsed();
    if (elapsed >= 1000) {  // Calculate FPS every second
        m_currentFPS = (m_frameCount * 1000.0) / elapsed;
        m_currentMetrics.averageFPS = m_currentFPS;
        m_currentMetrics.frameTime =
            elapsed / static_cast<double>(m_frameCount);
        m_currentMetrics.totalFrames += m_frameCount;

        m_frameCount = 0;
        m_frameTimer.restart();
    }
}

void FluentAnimationPerformanceManager::monitorSystemResources() {
    // This would involve platform-specific code to monitor CPU and memory usage
    // For now, we'll use placeholder values
    m_currentMetrics.cpuUsage = 50.0;     // Placeholder
    m_currentMetrics.memoryUsage = 75.0;  // Placeholder
}

bool FluentAnimationPerformanceManager::shouldSkipAnimation(
    const FluentAnimationRequest& request) const {
    // Skip if performance is too low and animation can be skipped
    if (request.canBeSkipped && m_currentMetrics.performanceScore < 50.0) {
        return true;
    }

    // Skip if in power saving mode and animation is not critical
    if (m_powerSavingEnabled &&
        request.priority < FluentAnimationPriority::High) {
        return true;
    }

    // Skip if accessibility mode requires it
    if (m_accessibilityMode == FluentAccessibilityAnimationMode::None) {
        return true;
    }

    return false;
}

void FluentAnimationPerformanceManager::executeAnimation(
    const FluentAnimationRequest& request) {
    // This would create and start the actual animation
    // For now, we'll just simulate execution
    if (request.onComplete) {
        QTimer::singleShot(request.duration.count(), request.onComplete);
    }
}

void FluentAnimationPerformanceManager::processAnimationQueue() {
    QMutexLocker locker(&m_queueMutex);

    while (!m_animationQueue.isEmpty() &&
           m_activeAnimations.size() < m_config.maxConcurrentAnimations) {
        int requestId = m_animationQueue.dequeue();
        auto it = m_animationRequests.find(requestId);
        if (it != m_animationRequests.end()) {
            executeAnimation(it.value());
            m_activeAnimations.insert(requestId);
        }
    }
}

void FluentAnimationPerformanceManager::applyCulturalAnimationStyle() {
    // Apply cultural animation preferences
    // This would modify animation parameters based on cultural style
}

void FluentAnimationPerformanceManager::applyAccessibilityAnimationMode() {
    // Apply accessibility animation preferences
    // This would modify animation parameters based on accessibility mode
}

void FluentAnimationPerformanceManager::detectHardwareCapabilities() {
    // Detect hardware acceleration support
    m_hardwareAccelerationSupported = true;  // Placeholder
}

void FluentAnimationPerformanceManager::connectToSystemSignals() {
    // Connect to system signals for power management, accessibility, etc.
    // This would be platform-specific
}

bool FluentAnimationPerformanceManager::isReducedMotionPreferred() const {
    // Check system accessibility preferences
    return m_systemReducedMotion;
}

// Optimization strategy implementations
void FluentAnimationPerformanceManager::applyReduceQualityStrategy() {
    // Reduce animation quality
}

void FluentAnimationPerformanceManager::applySkipFramesStrategy() {
    // Skip intermediate frames
}

void FluentAnimationPerformanceManager::applySimplifyEffectsStrategy() {
    // Use simpler effects
}

void FluentAnimationPerformanceManager::applyBatchAnimationsStrategy() {
    FLUENT_PROFILE(
        "FluentAnimationPerformanceManager::applyBatchAnimationsStrategy");

    QMutexLocker locker(&m_queueMutex);

    // Group animations by type and target for batching
    QMap<QString, QList<int>> animationGroups;

    for (auto it = m_animationRequests.begin(); it != m_animationRequests.end();
         ++it) {
        const auto& request = it.value();
        QString groupKey =
            QString("%1_%2_%3")
                .arg(request.animationType)  // fixed field name
                .arg(reinterpret_cast<quintptr>(request.target))
                .arg(static_cast<int>(request.duration.count() /
                                      100));  // Group by 100ms intervals

        animationGroups[groupKey].append(it.key());
    }

    // Process batched animations
    for (auto groupIt = animationGroups.begin();
         groupIt != animationGroups.end(); ++groupIt) {
        const auto& requestIds = groupIt.value();

        if (requestIds.size() > 1) {
            // Create batched animation group
            auto batchedGroup = createBatchedAnimationGroup(requestIds);
            if (batchedGroup) {
                // Execute batched group instead of individual animations
                executeBatchedAnimation(
                    std::move(batchedGroup));  // ensure move

                // Remove individual requests from queue
                for (int requestId : requestIds) {
                    m_animationRequests.remove(requestId);
                    m_animationQueue.removeAll(requestId);
                }
            }
        }
    }
}

void FluentAnimationPerformanceManager::applyReduceDurationStrategy() {
    // Reduce animation durations based on performance
    QMutexLocker locker(&m_queueMutex);

    const double performanceRatio =
        m_currentMetrics.performanceScore / 100.0;  // fixed member name
    const double durationMultiplier =
        qMax(0.3, performanceRatio);  // Minimum 30% of original duration

    for (auto& request : m_animationRequests) {
        auto originalDuration = request.duration;
        request.duration = std::chrono::milliseconds(
            static_cast<int>(originalDuration.count() * durationMultiplier));
    }
}

std::unique_ptr<FluentAnimationPerformanceManager::BatchedAnimationGroup>
FluentAnimationPerformanceManager::createBatchedAnimationGroup(
    const QList<int>& requestIds) {
    if (requestIds.isEmpty())
        return nullptr;

    auto batchedGroup = std::make_unique<BatchedAnimationGroup>();
    batchedGroup->requestIds = requestIds;
    batchedGroup->animationGroup = std::make_unique<QParallelAnimationGroup>();

    // Find common properties
    QWidget* commonTarget = nullptr;
    QString commonType;
    std::chrono::milliseconds maxDuration{0};

    for (int requestId : requestIds) {
        auto it = m_animationRequests.find(requestId);
        if (it != m_animationRequests.end()) {
            const auto& request = it.value();

            if (!commonTarget) {
                commonTarget = request.target;
                commonType = request.animationType;
            } else if (commonTarget != request.target) {
                // Different targets, can't batch effectively
                return nullptr;
            }

            maxDuration = std::max(maxDuration, request.duration);
        }
    }

    batchedGroup->commonTarget = commonTarget;
    batchedGroup->groupType = commonType;
    batchedGroup->totalDuration = maxDuration;

    return batchedGroup;
}

void FluentAnimationPerformanceManager::executeBatchedAnimation(
    std::unique_ptr<BatchedAnimationGroup> batchedGroup) {
    if (!batchedGroup || !batchedGroup->animationGroup)
        return;

    FLUENT_PROFILE(
        "FluentAnimationPerformanceManager::executeBatchedAnimation");

    // Execute the batched animation group
    batchedGroup->animationGroup->start();

    // Track batched animation performance
    m_currentMetrics.activeAnimations +=
        batchedGroup->requestIds.size();  // fixed member name

    // Connect completion signal
    connect(batchedGroup->animationGroup.get(),
            &QParallelAnimationGroup::finished,
            [this, requestIds = batchedGroup->requestIds]() {
                // Update metrics when batch completes
                m_currentMetrics.activeAnimations -=
                    requestIds.size();  // fixed member name

                // Execute completion callbacks
                for (int requestId : requestIds) {
                    auto it = m_animationRequests.find(requestId);
                    if (it != m_animationRequests.end() && it->onComplete) {
                        it->onComplete();
                    }
                }

                // Clean up completed requests
                for (int requestId : requestIds) {
                    m_animationRequests.remove(requestId);
                    m_activeAnimations.remove(requestId);
                }
            });

    // Release ownership - the animation group will manage its own lifetime
    batchedGroup->animationGroup.release();
}

void FluentAnimationPerformanceManager::applyPrioritizeVisibleStrategy() {
    // Prioritize visible animations
}

void FluentAnimationPerformanceManager::applyAdaptiveFrameRateStrategy() {
    // Adapt frame rate based on performance
}

void FluentAnimationPerformanceManager::onAccessibilitySettingsChanged() {
    // Respond to accessibility settings changes
    adaptAnimationsForAccessibility();

    // Update performance configuration based on accessibility needs
    if (isReducedMotionPreferred()) {
        setTargetPerformance(FluentAnimationPerformance::Accessibility);
    }
}

void FluentAnimationPerformanceManager::onSystemPerformanceChanged() {
    // Respond to system performance changes
    detectHardwareCapabilities();

    // Update performance monitoring
    if (m_performanceMonitoringEnabled) {
        updateMetrics();
        optimizePerformance();
    }
}

void FluentAnimationPerformanceManager::updatePerformanceBasedOnMetrics() {
    auto metrics = getCurrentMetrics();

    // Adjust performance settings based on current metrics
    if (metrics.averageFPS < m_config.minFPS) {
        // Performance is too low, reduce quality
        if (m_config.targetPerformance != FluentAnimationPerformance::Low) {
            qDebug() << "Performance degraded, reducing animation quality";
            setTargetPerformance(FluentAnimationPerformance::Low);
        }
    } else if (metrics.averageFPS > m_config.targetFPS * 0.9) {
        // Performance is good, can potentially increase quality
        if (m_config.targetPerformance == FluentAnimationPerformance::Low) {
            qDebug() << "Performance improved, increasing animation quality";
            setTargetPerformance(FluentAnimationPerformance::Medium);
        }
    }

    // Adjust based on dropped frames
    if (metrics.droppedFrames > 10) {
        m_enabledStrategies.insert(FluentOptimizationStrategy::SkipFrames);
    }

    // Adjust based on system resources
    if (metrics.cpuUsage > m_config.maxCPUUsage) {
        m_enabledStrategies.insert(FluentOptimizationStrategy::ReduceQuality);
    }
}

}  // namespace FluentQt::Animation
