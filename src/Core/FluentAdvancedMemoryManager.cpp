// src/Core/FluentAdvancedMemoryManager.cpp
#include "FluentQt/Core/FluentAdvancedMemoryManager.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QApplication>
#include <QDebug>
#include <QMutexLocker>
#include <algorithm>

namespace FluentQt::Core {

FluentAdvancedMemoryManager& FluentAdvancedMemoryManager::instance() {
    static FluentAdvancedMemoryManager instance;
    return instance;
}

FluentAdvancedMemoryManager::FluentAdvancedMemoryManager(QObject* parent)
    : QObject(parent),
      m_cleanupTimer(new QTimer(this)),
      m_statsTimer(new QTimer(this)),
      m_memoryCheckTimer(new QTimer(this)) {
    // Setup timers
    m_cleanupTimer->setInterval(30000);  // 30 seconds
    m_cleanupTimer->setSingleShot(false);
    connect(m_cleanupTimer, &QTimer::timeout, this,
            &FluentAdvancedMemoryManager::performCleanup);

    m_statsTimer->setInterval(1000);  // 1 second
    m_statsTimer->setSingleShot(false);
    connect(m_statsTimer, &QTimer::timeout, this,
            &FluentAdvancedMemoryManager::updateStatistics);

    m_memoryCheckTimer->setInterval(5000);  // 5 seconds
    m_memoryCheckTimer->setSingleShot(false);
    connect(m_memoryCheckTimer, &QTimer::timeout, this,
            &FluentAdvancedMemoryManager::checkMemoryLimit);

    // Start timers
    m_cleanupTimer->start();
    m_statsTimer->start();
    m_memoryCheckTimer->start();

    // Register common component types with optimized configurations
    registerCommonComponents();

    m_performanceTimer.start();
}

void FluentAdvancedMemoryManager::registerComponentType(
    const QString& typeName, const FluentComponentMemoryConfig& config) {
    QMutexLocker locker(&m_mutex);
    m_componentConfigs[typeName] = config;

    // Pre-allocate if requested
    if (config.enablePreallocation && config.preallocationCount > 0) {
        // This would be implemented based on the specific component type
        qDebug() << "Pre-allocation requested for" << typeName
                 << "count:" << config.preallocationCount;
    }
}

void FluentAdvancedMemoryManager::unregisterComponentType(
    const QString& typeName) {
    QMutexLocker locker(&m_mutex);

    // Clean up any cached components
    if (m_componentCaches.contains(typeName)) {
        auto& cache = m_componentCaches[typeName];
        for (const auto& cached : cache) {
            // Component-specific cleanup would go here
            delete static_cast<QObject*>(cached.component);
        }
        cache.clear();
    }

    m_componentConfigs.remove(typeName);
    m_componentCaches.remove(typeName);
    m_componentStats.remove(typeName);
    m_cacheHitCounts.remove(typeName);
    m_cacheMissCounts.remove(typeName);
}

void FluentAdvancedMemoryManager::optimizeMemoryUsage() {
    FLUENT_PROFILE("FluentAdvancedMemoryManager::optimizeMemoryUsage");

    QMutexLocker locker(&m_mutex);

    size_t freedBytes = 0;

    // Clean up expired cached components
    auto now = std::chrono::steady_clock::now();

    for (auto it = m_componentCaches.begin(); it != m_componentCaches.end();
         ++it) {
        const QString& typeName = it.key();
        auto& cache = it.value();

        if (!m_componentConfigs.contains(typeName))
            continue;

        const auto& config = m_componentConfigs[typeName];

        // Remove expired components
        cache.erase(
            std::remove_if(
                cache.begin(), cache.end(),
                [&](const CachedComponent& cached) {
                    auto age =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            now - cached.cacheTime);

                    if (age > config.cacheTimeout) {
                        delete static_cast<QObject*>(cached.component);
                        freedBytes += sizeof(QObject);  // Approximate
                        return true;
                    }
                    return false;
                }),
            cache.end());

        // Limit cache size
        if (cache.size() > static_cast<int>(config.maxCachedObjects)) {
            // Sort by access count (LRU)
            std::sort(cache.begin(), cache.end(),
                      [](const CachedComponent& a, const CachedComponent& b) {
                          return a.accessCount < b.accessCount;
                      });

            // Remove least recently used
            int toRemove = cache.size() - config.maxCachedObjects;
            for (int i = 0; i < toRemove; ++i) {
                delete static_cast<QObject*>(cache[i].component);
                freedBytes += sizeof(QObject);  // Approximate
            }

            cache.erase(cache.begin(), cache.begin() + toRemove);
        }
    }

    // Optimize memory pools
    for (const QString& typeName : m_componentConfigs.keys()) {
        optimizePool(typeName);
    }

    if (freedBytes > 0) {
        emit memoryOptimized(freedBytes);
    }
}

void FluentAdvancedMemoryManager::cleanupUnusedMemory() {
    QMutexLocker locker(&m_mutex);

    // Clean up empty caches
    for (auto it = m_componentCaches.begin(); it != m_componentCaches.end();) {
        if (it->isEmpty()) {
            it = m_componentCaches.erase(it);
        } else {
            ++it;
        }
    }

    // Reset statistics for unused types
    for (auto it = m_componentStats.begin(); it != m_componentStats.end();) {
        if (it->currentUsage == 0 &&
            it->totalAllocated == it->totalDeallocated) {
            it = m_componentStats.erase(it);
        } else {
            ++it;
        }
    }
}

FluentMemoryStats FluentAdvancedMemoryManager::getMemoryStats() const {
    QMutexLocker locker(&m_mutex);
    return m_globalStats;
}

FluentMemoryStats FluentAdvancedMemoryManager::getComponentStats(
    const QString& typeName) const {
    QMutexLocker locker(&m_mutex);
    return m_componentStats.value(typeName);
}

QStringList FluentAdvancedMemoryManager::getRegisteredTypes() const {
    QMutexLocker locker(&m_mutex);
    return m_componentConfigs.keys();
}

void FluentAdvancedMemoryManager::setGlobalStrategy(
    FluentMemoryStrategy strategy) {
    QMutexLocker locker(&m_mutex);
    m_globalStrategy = strategy;
}

void FluentAdvancedMemoryManager::setMemoryLimit(size_t limitBytes) {
    QMutexLocker locker(&m_mutex);
    m_memoryLimit = limitBytes;
}

void FluentAdvancedMemoryManager::setCleanupInterval(
    std::chrono::milliseconds interval) {
    m_cleanupTimer->setInterval(static_cast<int>(interval.count()));
}

void FluentAdvancedMemoryManager::enablePerformanceMonitoring(bool enabled) {
    m_performanceMonitoringEnabled = enabled;

    if (enabled) {
        m_statsTimer->start();
    } else {
        m_statsTimer->stop();
    }
}

void FluentAdvancedMemoryManager::performCleanup() {
    optimizeMemoryUsage();
    cleanupUnusedMemory();
}

void FluentAdvancedMemoryManager::updateStatistics() {
    if (!m_performanceMonitoringEnabled)
        return;

    QMutexLocker locker(&m_mutex);

    auto now = std::chrono::steady_clock::now();
    m_globalStats.lastUpdate = now;

    // Update allocation rates
    updateAllocationRate();

    // Calculate peak usage
    if (m_globalStats.currentUsage > m_globalStats.peakUsage) {
        m_globalStats.peakUsage = m_globalStats.currentUsage;
    }

    // Update component-specific statistics
    for (auto& stats : m_componentStats) {
        stats.lastUpdate = now;

        if (stats.currentUsage > stats.peakUsage) {
            stats.peakUsage = stats.currentUsage;
        }
    }
}

void FluentAdvancedMemoryManager::checkMemoryLimit() {
    QMutexLocker locker(&m_mutex);

    if (m_globalStats.currentUsage > m_memoryLimit) {
        emit memoryLimitExceeded(m_globalStats.currentUsage, m_memoryLimit);

        // Trigger aggressive cleanup
        optimizeMemoryUsage();
    }
}

void FluentAdvancedMemoryManager::trackAllocation(const QString& typeName,
                                                  size_t size) {
    m_globalStats.totalAllocated += size;
    m_globalStats.currentUsage += size;

    auto& componentStats = m_componentStats[typeName];
    componentStats.totalAllocated += size;
    componentStats.currentUsage += size;

    // Update allocation counts for rate calculation
    m_allocationCounts[typeName]++;
    m_lastAllocationTime[typeName] = std::chrono::steady_clock::now();
}

void FluentAdvancedMemoryManager::trackDeallocation(const QString& typeName,
                                                    size_t size) {
    m_globalStats.totalDeallocated += size;
    m_globalStats.currentUsage = (m_globalStats.currentUsage > size)
                                     ? m_globalStats.currentUsage - size
                                     : 0;

    auto& componentStats = m_componentStats[typeName];
    componentStats.totalDeallocated += size;
    componentStats.currentUsage = (componentStats.currentUsage > size)
                                      ? componentStats.currentUsage - size
                                      : 0;
}

void FluentAdvancedMemoryManager::updateAllocationRate() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                       now - m_performanceTimer.elapsed())
                       .count();

    if (elapsed > 0) {
        for (auto it = m_allocationCounts.begin();
             it != m_allocationCounts.end(); ++it) {
            const QString& typeName = it.key();
            size_t count = it.value();

            auto& stats = m_componentStats[typeName];
            stats.allocationRate = static_cast<double>(count) / elapsed;

            // Reset counter
            it.value() = 0;
        }
    }
}

FluentMemoryStrategy FluentAdvancedMemoryManager::determineOptimalStrategy(
    const QString& typeName) const {
    const auto& stats = m_componentStats.value(typeName);

    // High allocation rate -> use pooling
    if (stats.allocationRate > 10.0) {
        return FluentMemoryStrategy::Pooled;
    }

    // Frequent allocations of same size -> use caching
    if (stats.totalAllocated > 1000 && stats.averageAllocationSize > 0) {
        return FluentMemoryStrategy::Cached;
    }

    // Low usage -> use standard allocation
    if (stats.allocationRate < 1.0) {
        return FluentMemoryStrategy::Standard;
    }

    return FluentMemoryStrategy::Pooled;  // Default for moderate usage
}

void FluentAdvancedMemoryManager::registerCommonComponents() {
    // FluentButton - high frequency, small size
    FluentComponentMemoryConfig buttonConfig;
    buttonConfig.strategy = FluentMemoryStrategy::Pooled;
    buttonConfig.poolBlockSize = 8192;
    buttonConfig.maxCachedObjects = 50;
    buttonConfig.enablePreallocation = true;
    buttonConfig.preallocationCount = 20;
    registerComponentType("FluentButton", buttonConfig);

    // FluentCard - medium frequency, medium size
    FluentComponentMemoryConfig cardConfig;
    cardConfig.strategy = FluentMemoryStrategy::Cached;
    cardConfig.maxCachedObjects = 20;
    cardConfig.cacheTimeout = std::chrono::milliseconds(60000);  // 1 minute
    registerComponentType("FluentCard", cardConfig);

    // FluentTreeView - low frequency, large size
    FluentComponentMemoryConfig treeConfig;
    treeConfig.strategy = FluentMemoryStrategy::Standard;
    treeConfig.maxCachedObjects = 5;
    registerComponentType("FluentTreeView", treeConfig);
}

}  // namespace FluentQt::Core
