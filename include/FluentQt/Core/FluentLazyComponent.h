// include/FluentQt/Core/FluentLazyComponent.h
#pragma once

#include <QMutex>
#include <QObject>
#include <QPointer>
#include <QSharedPointer>
#include <QTimer>
#include <QWeakPointer>
#include <QWidget>
#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>

namespace FluentQt::Core {

// Lazy loading strategies
enum class FluentLazyLoadStrategy {
    OnFirstAccess,  // Load when first accessed
    OnVisible,      // Load when widget becomes visible
    OnInteraction,  // Load when user interacts
    OnHover,        // Load when mouse hovers
    OnFocus,        // Load when widget receives focus
    OnDemand,       // Load only when explicitly requested
    Preload,        // Load during idle time
    Background,     // Load in background thread
    Proximity,      // Load when near viewport
    Predictive      // Load based on usage patterns
};

// Lazy loading triggers
enum class FluentLazyLoadTrigger {
    Immediate,
    Delayed,
    Conditional,
    UserDriven,
    SystemDriven
};

// Lazy component state
enum class FluentLazyComponentState {
    NotLoaded,
    Loading,
    Loaded,
    Failed,
    Unloaded
};

// Lazy loading configuration
struct FluentLazyLoadConfig {
    FluentLazyLoadStrategy strategy{FluentLazyLoadStrategy::OnFirstAccess};
    FluentLazyLoadTrigger trigger{FluentLazyLoadTrigger::Immediate};
    std::chrono::milliseconds loadDelay{0};
    std::chrono::milliseconds timeout{5000};
    int priority{0};
    bool enableCaching{true};
    bool enablePreloading{false};
    bool enableUnloading{false};
    std::chrono::milliseconds unloadDelay{30000};  // 30 seconds
    size_t maxMemoryUsage{50 * 1024 * 1024};       // 50MB
    std::function<bool()> loadCondition;
    std::function<void()> onLoadStart;
    std::function<void()> onLoadComplete;
    std::function<void(const QString&)> onLoadError;
};

// Lazy component metadata
struct FluentLazyComponentMetadata {
    QString componentId;
    QString componentType;
    QString moduleName;
    FluentLazyComponentState state{FluentLazyComponentState::NotLoaded};
    FluentLazyLoadConfig config;
    std::chrono::steady_clock::time_point creationTime;
    std::chrono::steady_clock::time_point loadTime;
    std::chrono::steady_clock::time_point lastAccessTime;
    std::chrono::milliseconds loadDuration{0};
    size_t memoryUsage{0};
    int accessCount{0};
    bool isVisible{false};
    bool isInViewport{false};
    QPointer<QWidget> widget;
    std::function<QWidget*()> factory;
};

// Lazy component manager
class FluentLazyComponentManager : public QObject {
    Q_OBJECT

public:
    static FluentLazyComponentManager& instance();

    // Component registration
    template <typename T>
    void registerLazyComponent(const QString& componentId,
                               const FluentLazyLoadConfig& config = {}) {
        FluentLazyComponentMetadata metadata;
        metadata.componentId = componentId;
        metadata.componentType = T::staticMetaObject.className();
        metadata.config = config;
        metadata.creationTime = std::chrono::steady_clock::now();
        metadata.factory = []() -> QWidget* { return new T(); };

        QMutexLocker locker(&m_mutex);
        m_components[componentId] = metadata;
    }

    void registerLazyComponent(const QString& componentId,
                               std::function<QWidget*()> factory,
                               const FluentLazyLoadConfig& config = {});

    void unregisterLazyComponent(const QString& componentId);

    // Component loading
    QWidget* loadComponent(const QString& componentId);
    void unloadComponent(const QString& componentId);
    bool isComponentLoaded(const QString& componentId) const;
    FluentLazyComponentState getComponentState(
        const QString& componentId) const;

    // Async loading
    void loadComponentAsync(const QString& componentId);
    void preloadComponents(const QStringList& componentIds);
    void loadComponentsInBackground(const QStringList& componentIds);

    // Visibility tracking
    void setComponentVisible(const QString& componentId, bool visible);
    void setComponentInViewport(const QString& componentId, bool inViewport);
    void updateViewportComponents(const QRect& viewport);

    // Memory management
    void unloadUnusedComponents();
    void setMemoryLimit(size_t limit) { m_memoryLimit = limit; }
    size_t getCurrentMemoryUsage() const;
    size_t getMemoryLimit() const { return m_memoryLimit; }

    // Configuration
    void setGlobalConfig(const FluentLazyLoadConfig& config);
    FluentLazyLoadConfig getGlobalConfig() const { return m_globalConfig; }
    void setComponentConfig(const QString& componentId,
                            const FluentLazyLoadConfig& config);

    // Statistics
    struct LazyLoadingStats {
        int totalComponents{0};
        int loadedComponents{0};
        int failedComponents{0};
        std::chrono::milliseconds totalLoadTime{0};
        std::chrono::milliseconds averageLoadTime{0};
        size_t totalMemoryUsage{0};
        double cacheHitRate{0.0};
        int preloadedComponents{0};
        int unloadedComponents{0};
    };

    LazyLoadingStats getStatistics() const;
    void resetStatistics();

    // Component metadata
    FluentLazyComponentMetadata getComponentMetadata(
        const QString& componentId) const;
    QStringList getRegisteredComponents() const;
    QStringList getLoadedComponents() const;
    QStringList getUnloadedComponents() const;

signals:
    void componentLoadStarted(const QString& componentId);
    void componentLoaded(const QString& componentId, QWidget* widget);
    void componentLoadFailed(const QString& componentId, const QString& error);
    void componentUnloaded(const QString& componentId);
    void memoryLimitExceeded(size_t currentUsage, size_t limit);
    void statisticsUpdated(const LazyLoadingStats& stats);

private slots:
    void processLoadQueue();
    void checkMemoryUsage();
    void updateStatistics();
    void cleanupUnusedComponents();

private:
    FluentLazyComponentManager();
    void loadComponentInternal(const QString& componentId);
    bool shouldLoadComponent(const QString& componentId) const;
    void updateComponentMetadata(const QString& componentId, QWidget* widget);

private:
    mutable QMutex m_mutex;
    std::unordered_map<QString, FluentLazyComponentMetadata> m_components;
    FluentLazyLoadConfig m_globalConfig;

    // Loading queue and timers
    QStringList m_loadQueue;
    QTimer* m_loadTimer;
    QTimer* m_memoryTimer;
    QTimer* m_statsTimer;
    QTimer* m_cleanupTimer;

    // Memory management
    size_t m_memoryLimit{100 * 1024 * 1024};  // 100MB default

    // Statistics
    LazyLoadingStats m_stats;

    // Background loading
    bool m_backgroundLoadingEnabled{true};
    int m_maxConcurrentLoads{3};
    int m_currentLoads{0};
};

// Lazy component wrapper
template <typename T>
class FluentLazyComponent {
public:
    explicit FluentLazyComponent(const QString& componentId,
                                 const FluentLazyLoadConfig& config = {})
        : m_componentId(componentId), m_config(config) {
        auto& manager = FluentLazyComponentManager::instance();
        manager.registerLazyComponent<T>(componentId, config);

        // Store callback for when component is loaded
        QObject::connect(&manager, &FluentLazyComponentManager::componentLoaded,
                         [this](const QString& id, QWidget* widget) {
                             if (id == m_componentId) {
                                 m_widget = qobject_cast<T*>(widget);
                                 if (m_readyCallback) {
                                     m_readyCallback(m_widget);
                                 }
                             }
                         });
    }

    T* get() {
        if (!m_widget) {
            auto& manager = FluentLazyComponentManager::instance();
            auto* widget = manager.loadComponent(m_componentId);
            m_widget = qobject_cast<T*>(widget);
        }
        return m_widget;
    }

    T* operator->() { return get(); }
    T& operator*() { return *get(); }

    bool isLoaded() const {
        auto& manager = FluentLazyComponentManager::instance();
        return manager.isComponentLoaded(m_componentId);
    }

    void loadAsync() {
        auto& manager = FluentLazyComponentManager::instance();
        manager.loadComponentAsync(m_componentId);
    }

    void unload() {
        auto& manager = FluentLazyComponentManager::instance();
        manager.unloadComponent(m_componentId);
        m_widget = nullptr;
    }

    // Set callback for when component is ready
    void setReadyCallback(std::function<void(T*)> callback) {
        m_readyCallback = callback;
    }

private:
    QString m_componentId;
    FluentLazyLoadConfig m_config;
    T* m_widget{nullptr};
    std::function<void(T*)> m_readyCallback;
};

// Convenience macros
#define FLUENT_LAZY_COMPONENT(ComponentType, componentId) \
    FluentQt::Core::FluentLazyComponent<ComponentType>(componentId)

#define FLUENT_LAZY_COMPONENT_WITH_CONFIG(ComponentType, componentId, config) \
    FluentQt::Core::FluentLazyComponent<ComponentType>(componentId, config)

#define FLUENT_REGISTER_LAZY_COMPONENT(ComponentType, componentId) \
    FluentQt::Core::FluentLazyComponentManager::instance()         \
        .registerLazyComponent<ComponentType>(componentId)

}  // namespace FluentQt::Core
