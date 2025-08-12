// include/FluentQt/Core/FluentAdvancedMemoryManager.h
#pragma once

#include <QElapsedTimer>
#include <QHash>
#include <QMutex>
#include <QObject>
#include <QTimer>
#include <chrono>
#include <memory>
#include <unordered_map>
#include "FluentMemoryPool.h"

namespace FluentQt::Core {

// Forward declarations
class FluentComponent;
class FluentButton;
class FluentCard;
class FluentTreeView;

// Memory allocation strategies
enum class FluentMemoryStrategy {
    Standard,  // Standard Qt allocation
    Pooled,    // Use memory pools
    Cached,    // Cache frequently used objects
    Lazy,      // Lazy allocation/deallocation
    Adaptive   // Adapt based on usage patterns
};

// Memory usage statistics
struct FluentMemoryStats {
    size_t totalAllocated{0};
    size_t totalDeallocated{0};
    size_t currentUsage{0};
    size_t peakUsage{0};
    size_t pooledAllocations{0};
    size_t standardAllocations{0};
    double averageAllocationSize{0.0};
    double allocationRate{0.0};  // allocations per second
    std::chrono::steady_clock::time_point lastUpdate;
};

// Component-specific memory configuration
struct FluentComponentMemoryConfig {
    FluentMemoryStrategy strategy{FluentMemoryStrategy::Adaptive};
    size_t poolBlockSize{4096};
    size_t maxCachedObjects{100};
    std::chrono::milliseconds cacheTimeout{30000};  // 30 seconds
    bool enablePreallocation{false};
    size_t preallocationCount{10};
};

// Advanced memory manager for FluentUI components
class FluentAdvancedMemoryManager : public QObject {
    Q_OBJECT

public:
    static FluentAdvancedMemoryManager& instance();

    // Component registration
    void registerComponentType(const QString& typeName,
                               const FluentComponentMemoryConfig& config);
    void unregisterComponentType(const QString& typeName);

    // Memory allocation
    template <typename T>
    T* allocateComponent(const QString& typeName = {});

    template <typename T>
    void deallocateComponent(T* component, const QString& typeName = {});

    // Bulk operations
    template <typename T>
    QList<T*> allocateComponents(int count, const QString& typeName = {});

    template <typename T>
    void deallocateComponents(const QList<T*>& components,
                              const QString& typeName = {});

    // Memory pools management
    template <typename T>
    FluentMemoryPool<T>& getPool();

    template <typename T>
    void configurePool(size_t blockSize, size_t maxObjects = 1000);

    // Caching system
    template <typename T>
    void enableCaching(const QString& typeName, size_t maxCached = 50);

    template <typename T>
    T* getCachedComponent(const QString& typeName);

    template <typename T>
    void returnToCache(T* component, const QString& typeName);

    // Memory optimization
    void optimizeMemoryUsage();
    void cleanupUnusedMemory();
    void preAllocateComponents();

    // Statistics and monitoring
    FluentMemoryStats getMemoryStats() const;
    FluentMemoryStats getComponentStats(const QString& typeName) const;
    QStringList getRegisteredTypes() const;

    // Configuration
    void setGlobalStrategy(FluentMemoryStrategy strategy);
    FluentMemoryStrategy globalStrategy() const { return m_globalStrategy; }

    void setMemoryLimit(size_t limitBytes);
    size_t memoryLimit() const { return m_memoryLimit; }

    void setCleanupInterval(std::chrono::milliseconds interval);

    // Performance monitoring
    void enablePerformanceMonitoring(bool enabled = true);
    bool isPerformanceMonitoringEnabled() const {
        return m_performanceMonitoringEnabled;
    }

signals:
    void memoryLimitExceeded(size_t currentUsage, size_t limit);
    void memoryOptimized(size_t freedBytes);
    void componentCached(const QString& typeName, int cachedCount);
    void componentUncached(const QString& typeName, int remainingCount);

private slots:
    void performCleanup();
    void updateStatistics();
    void checkMemoryLimit();

private:
    FluentAdvancedMemoryManager(QObject* parent = nullptr);
    ~FluentAdvancedMemoryManager() = default;

    // Memory pool management
    template <typename T>
    void initializePool();

    void cleanupPool(const QString& typeName);
    void optimizePool(const QString& typeName);

    // Caching implementation
    struct CachedComponent {
        void* component;
        std::chrono::steady_clock::time_point cacheTime;
        size_t accessCount;
    };

    template <typename T>
    void cleanupCache(const QString& typeName);

    // Statistics tracking
    void trackAllocation(const QString& typeName, size_t size);
    void trackDeallocation(const QString& typeName, size_t size);
    void updateAllocationRate();

    // Memory strategy implementation
    template <typename T>
    T* allocateWithStrategy(const QString& typeName,
                            FluentMemoryStrategy strategy);

    template <typename T>
    void deallocateWithStrategy(T* component, const QString& typeName,
                                FluentMemoryStrategy strategy);

    // Adaptive strategy logic
    FluentMemoryStrategy determineOptimalStrategy(
        const QString& typeName) const;
    void adaptStrategyBasedOnUsage(const QString& typeName);

private:
    mutable QMutex m_mutex;

    // Configuration
    FluentMemoryStrategy m_globalStrategy{FluentMemoryStrategy::Adaptive};
    size_t m_memoryLimit{100 * 1024 * 1024};  // 100MB default
    bool m_performanceMonitoringEnabled{true};

    // Component configurations
    QHash<QString, FluentComponentMemoryConfig> m_componentConfigs;

    // Memory pools (type-erased)
    QHash<QString, std::shared_ptr<void>> m_memoryPools;

    // Caching system
    QHash<QString, QList<CachedComponent>> m_componentCaches;
    QHash<QString, size_t> m_cacheHitCounts;
    QHash<QString, size_t> m_cacheMissCounts;

    // Statistics
    mutable FluentMemoryStats m_globalStats;
    mutable QHash<QString, FluentMemoryStats> m_componentStats;

    // Timers
    QTimer* m_cleanupTimer;
    QTimer* m_statsTimer;
    QTimer* m_memoryCheckTimer;

    // Performance tracking
    QElapsedTimer m_performanceTimer;
    QHash<QString, std::chrono::steady_clock::time_point> m_lastAllocationTime;
    QHash<QString, size_t> m_allocationCounts;
};

// Template implementations
template <typename T>
T* FluentAdvancedMemoryManager::allocateComponent(const QString& typeName) {
    QMutexLocker locker(&m_mutex);

    QString actualTypeName =
        typeName.isEmpty() ? T::staticMetaObject.className() : typeName;

    // Check if we have a cached component first
    if (T* cached = getCachedComponent<T>(actualTypeName)) {
        trackAllocation(actualTypeName, sizeof(T));
        return cached;
    }

    // Determine strategy
    FluentMemoryStrategy strategy = m_globalStrategy;
    if (m_componentConfigs.contains(actualTypeName)) {
        strategy = m_componentConfigs[actualTypeName].strategy;
    }

    if (strategy == FluentMemoryStrategy::Adaptive) {
        strategy = determineOptimalStrategy(actualTypeName);
    }

    T* component = allocateWithStrategy<T>(actualTypeName, strategy);
    trackAllocation(actualTypeName, sizeof(T));

    return component;
}

template <typename T>
void FluentAdvancedMemoryManager::deallocateComponent(T* component,
                                                      const QString& typeName) {
    if (!component)
        return;

    QMutexLocker locker(&m_mutex);

    QString actualTypeName =
        typeName.isEmpty() ? T::staticMetaObject.className() : typeName;

    // Try to return to cache first
    if (m_componentConfigs.contains(actualTypeName)) {
        const auto& config = m_componentConfigs[actualTypeName];
        if (config.maxCachedObjects > 0) {
            returnToCache(component, actualTypeName);
            trackDeallocation(actualTypeName, sizeof(T));
            return;
        }
    }

    // Determine strategy
    FluentMemoryStrategy strategy = m_globalStrategy;
    if (m_componentConfigs.contains(actualTypeName)) {
        strategy = m_componentConfigs[actualTypeName].strategy;
    }

    deallocateWithStrategy(component, actualTypeName, strategy);
    trackDeallocation(actualTypeName, sizeof(T));
}

template <typename T>
FluentMemoryPool<T>& FluentAdvancedMemoryManager::getPool() {
    QString typeName = T::staticMetaObject.className();

    auto it = m_memoryPools.find(typeName);
    if (it == m_memoryPools.end()) {
        initializePool<T>();
        it = m_memoryPools.find(typeName);
    }

    return *static_cast<FluentMemoryPool<T>*>(it->get());
}

// Convenience macros for memory management
#define FLUENT_ALLOCATE(Type) \
    FluentAdvancedMemoryManager::instance().allocateComponent<Type>()

#define FLUENT_DEALLOCATE(component) \
    FluentAdvancedMemoryManager::instance().deallocateComponent(component)

#define FLUENT_POOL_ALLOCATE(Type, ...)                                \
    FluentAdvancedMemoryManager::instance().getPool<Type>().construct( \
        __VA_ARGS__)

#define FLUENT_POOL_DEALLOCATE(component)                      \
    FluentAdvancedMemoryManager::instance()                    \
        .getPool<std::remove_pointer_t<decltype(component)>>() \
        .destroy(component)

}  // namespace FluentQt::Core
