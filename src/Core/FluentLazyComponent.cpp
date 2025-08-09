// src/Core/FluentLazyComponent.cpp
#include "FluentQt/Core/FluentLazyComponent.h"
#include <QApplication>
#include <QDebug>
#include <QMutexLocker>
#include <QThread>
#include <algorithm>

namespace FluentQt::Core {

FluentLazyComponentManager& FluentLazyComponentManager::instance() {
    static FluentLazyComponentManager instance;
    return instance;
}

FluentLazyComponentManager::FluentLazyComponentManager() {
    // Initialize timers
    m_loadTimer = new QTimer(this);
    m_memoryTimer = new QTimer(this);
    m_statsTimer = new QTimer(this);
    m_cleanupTimer = new QTimer(this);
    
    // Connect signals
    connect(m_loadTimer, &QTimer::timeout, this, &FluentLazyComponentManager::processLoadQueue);
    connect(m_memoryTimer, &QTimer::timeout, this, &FluentLazyComponentManager::checkMemoryUsage);
    connect(m_statsTimer, &QTimer::timeout, this, &FluentLazyComponentManager::updateStatistics);
    connect(m_cleanupTimer, &QTimer::timeout, this, &FluentLazyComponentManager::cleanupUnusedComponents);
    
    // Start timers
    m_loadTimer->start(100);    // Process load queue every 100ms
    m_memoryTimer->start(5000); // Check memory every 5 seconds
    m_statsTimer->start(1000);  // Update stats every second
    m_cleanupTimer->start(30000); // Cleanup every 30 seconds
}

void FluentLazyComponentManager::registerLazyComponent(const QString& componentId,
                                                      std::function<QWidget*()> factory,
                                                      const FluentLazyLoadConfig& config) {
    FluentLazyComponentMetadata metadata;
    metadata.componentId = componentId;
    metadata.componentType = "Custom";
    metadata.config = config;
    metadata.creationTime = std::chrono::steady_clock::now();
    metadata.factory = factory;
    
    QMutexLocker locker(&m_mutex);
    m_components[componentId] = metadata;
    
    qDebug() << "Registered lazy component:" << componentId;
}

void FluentLazyComponentManager::unregisterLazyComponent(const QString& componentId) {
    QMutexLocker locker(&m_mutex);
    
    // Unload component if loaded
    auto it = m_components.find(componentId);
    if (it != m_components.end() && it->second.state == FluentLazyComponentState::Loaded) {
        if (auto widget = it->second.widget) {
            widget->deleteLater();
        }
    }
    
    m_components.erase(componentId);
    m_loadQueue.removeAll(componentId);
    
    qDebug() << "Unregistered lazy component:" << componentId;
}

QWidget* FluentLazyComponentManager::loadComponent(const QString& componentId) {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_components.find(componentId);
    if (it == m_components.end()) {
        qWarning() << "Component not registered:" << componentId;
        return nullptr;
    }
    
    auto& metadata = it->second;
    
    // If already loaded, return existing widget
    if (metadata.state == FluentLazyComponentState::Loaded) {
        if (auto widget = metadata.widget) {
            metadata.accessCount++;
            metadata.lastAccessTime = std::chrono::steady_clock::now();
            return widget;
        } else {
            // Widget was destroyed, reset state
            metadata.state = FluentLazyComponentState::NotLoaded;
        }
    }
    
    // Check if we should load the component
    if (!shouldLoadComponent(componentId)) {
        return nullptr;
    }
    
    // Load the component
    metadata.state = FluentLazyComponentState::Loading;
    emit componentLoadStarted(componentId);
    
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        QWidget* widget = metadata.factory();
        if (widget) {
            auto endTime = std::chrono::steady_clock::now();
            metadata.loadTime = endTime;
            metadata.loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            metadata.state = FluentLazyComponentState::Loaded;
            metadata.widget = widget;
            metadata.accessCount++;
            metadata.lastAccessTime = endTime;
            
            updateComponentMetadata(componentId, widget);
            
            emit componentLoaded(componentId, widget);
            qDebug() << "Loaded component" << componentId << "in" << metadata.loadDuration.count() << "ms";
            
            return widget;
        } else {
            metadata.state = FluentLazyComponentState::Failed;
            emit componentLoadFailed(componentId, "Factory returned null widget");
            return nullptr;
        }
    } catch (const std::exception& e) {
        metadata.state = FluentLazyComponentState::Failed;
        QString error = QString("Exception during component loading: %1").arg(e.what());
        emit componentLoadFailed(componentId, error);
        qWarning() << "Failed to load component" << componentId << ":" << error;
        return nullptr;
    }
}

void FluentLazyComponentManager::unloadComponent(const QString& componentId) {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_components.find(componentId);
    if (it == m_components.end()) {
        return;
    }
    
    auto& metadata = it->second;
    if (metadata.state == FluentLazyComponentState::Loaded) {
        if (auto widget = metadata.widget) {
            widget->deleteLater();
        }
        metadata.state = FluentLazyComponentState::Unloaded;
        metadata.widget.clear();
        
        emit componentUnloaded(componentId);
        qDebug() << "Unloaded component:" << componentId;
    }
}

bool FluentLazyComponentManager::isComponentLoaded(const QString& componentId) const {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_components.find(componentId);
    if (it == m_components.end()) {
        return false;
    }
    
    return it->second.state == FluentLazyComponentState::Loaded && 
           !it->second.widget.isNull();
}

FluentLazyComponentState FluentLazyComponentManager::getComponentState(const QString& componentId) const {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_components.find(componentId);
    if (it == m_components.end()) {
        return FluentLazyComponentState::NotLoaded;
    }
    
    return it->second.state;
}

void FluentLazyComponentManager::loadComponentAsync(const QString& componentId) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_loadQueue.contains(componentId)) {
        m_loadQueue.append(componentId);
    }
}

void FluentLazyComponentManager::preloadComponents(const QStringList& componentIds) {
    for (const QString& id : componentIds) {
        loadComponentAsync(id);
    }
}

void FluentLazyComponentManager::loadComponentsInBackground(const QStringList& componentIds) {
    if (!m_backgroundLoadingEnabled) {
        return;
    }
    
    // Add to load queue with background priority
    QMutexLocker locker(&m_mutex);
    for (const QString& id : componentIds) {
        if (!m_loadQueue.contains(id)) {
            m_loadQueue.append(id);
        }
    }
}

void FluentLazyComponentManager::setComponentVisible(const QString& componentId, bool visible) {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_components.find(componentId);
    if (it != m_components.end()) {
        it->second.isVisible = visible;
        
        // Auto-load if visible and strategy is OnVisible
        if (visible && it->second.config.strategy == FluentLazyLoadStrategy::OnVisible) {
            m_loadQueue.append(componentId);
        }
    }
}

void FluentLazyComponentManager::setComponentInViewport(const QString& componentId, bool inViewport) {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_components.find(componentId);
    if (it != m_components.end()) {
        it->second.isInViewport = inViewport;
        
        // Auto-load if in viewport and strategy is Proximity
        if (inViewport && it->second.config.strategy == FluentLazyLoadStrategy::Proximity) {
            m_loadQueue.append(componentId);
        }
    }
}

void FluentLazyComponentManager::updateViewportComponents(const QRect& viewport) {
    Q_UNUSED(viewport)
    // This would update which components are in the viewport
    // Implementation depends on how components report their positions
}

void FluentLazyComponentManager::unloadUnusedComponents() {
    QMutexLocker locker(&m_mutex);
    
    auto now = std::chrono::steady_clock::now();
    QStringList toUnload;
    
    for (auto& [id, metadata] : m_components) {
        if (metadata.state == FluentLazyComponentState::Loaded && 
            metadata.config.enableUnloading) {
            
            auto timeSinceAccess = now - metadata.lastAccessTime;
            if (timeSinceAccess > metadata.config.unloadDelay) {
                toUnload.append(id);
            }
        }
    }
    
    locker.unlock();
    
    for (const QString& id : toUnload) {
        unloadComponent(id);
    }
}

size_t FluentLazyComponentManager::getCurrentMemoryUsage() const {
    QMutexLocker locker(&m_mutex);
    
    size_t total = 0;
    for (const auto& [id, metadata] : m_components) {
        total += metadata.memoryUsage;
    }
    
    return total;
}

void FluentLazyComponentManager::setGlobalConfig(const FluentLazyLoadConfig& config) {
    m_globalConfig = config;
}

void FluentLazyComponentManager::setComponentConfig(const QString& componentId, const FluentLazyLoadConfig& config) {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_components.find(componentId);
    if (it != m_components.end()) {
        it->second.config = config;
    }
}

FluentLazyComponentManager::LazyLoadingStats FluentLazyComponentManager::getStatistics() const {
    QMutexLocker locker(&m_mutex);
    return m_stats;
}

void FluentLazyComponentManager::resetStatistics() {
    QMutexLocker locker(&m_mutex);
    m_stats = LazyLoadingStats{};
}

FluentLazyComponentMetadata FluentLazyComponentManager::getComponentMetadata(const QString& componentId) const {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_components.find(componentId);
    if (it != m_components.end()) {
        return it->second;
    }
    
    return FluentLazyComponentMetadata{};
}

QStringList FluentLazyComponentManager::getRegisteredComponents() const {
    QMutexLocker locker(&m_mutex);
    
    QStringList result;
    for (const auto& [id, metadata] : m_components) {
        result.append(id);
    }
    
    return result;
}

QStringList FluentLazyComponentManager::getLoadedComponents() const {
    QMutexLocker locker(&m_mutex);
    
    QStringList result;
    for (const auto& [id, metadata] : m_components) {
        if (metadata.state == FluentLazyComponentState::Loaded) {
            result.append(id);
        }
    }
    
    return result;
}

QStringList FluentLazyComponentManager::getUnloadedComponents() const {
    QMutexLocker locker(&m_mutex);
    
    QStringList result;
    for (const auto& [id, metadata] : m_components) {
        if (metadata.state == FluentLazyComponentState::NotLoaded || 
            metadata.state == FluentLazyComponentState::Unloaded) {
            result.append(id);
        }
    }
    
    return result;
}

// Private slot implementations
void FluentLazyComponentManager::processLoadQueue() {
    if (m_loadQueue.isEmpty() || m_currentLoads >= m_maxConcurrentLoads) {
        return;
    }

    QMutexLocker locker(&m_mutex);

    while (!m_loadQueue.isEmpty() && m_currentLoads < m_maxConcurrentLoads) {
        QString componentId = m_loadQueue.takeFirst();

        auto it = m_components.find(componentId);
        if (it == m_components.end() || it->second.state == FluentLazyComponentState::Loaded) {
            continue;
        }

        m_currentLoads++;
        locker.unlock();

        // Load component in background if enabled
        if (m_backgroundLoadingEnabled) {
            QTimer::singleShot(0, this, [this, componentId]() {
                loadComponentInternal(componentId);
                m_currentLoads--;
            });
        } else {
            loadComponentInternal(componentId);
            m_currentLoads--;
        }

        locker.relock();
    }
}

void FluentLazyComponentManager::checkMemoryUsage() {
    size_t currentUsage = getCurrentMemoryUsage();

    if (currentUsage > m_memoryLimit) {
        emit memoryLimitExceeded(currentUsage, m_memoryLimit);

        // Unload least recently used components
        QMutexLocker locker(&m_mutex);

        QList<std::pair<QString, std::chrono::steady_clock::time_point>> candidates;
        for (const auto& [id, metadata] : m_components) {
            if (metadata.state == FluentLazyComponentState::Loaded &&
                metadata.config.enableUnloading) {
                candidates.append({id, metadata.lastAccessTime});
            }
        }

        // Sort by last access time (oldest first)
        std::sort(candidates.begin(), candidates.end(),
                 [](const auto& a, const auto& b) {
                     return a.second < b.second;
                 });

        locker.unlock();

        // Unload components until under memory limit
        for (const auto& [id, time] : candidates) {
            unloadComponent(id);
            if (getCurrentMemoryUsage() <= m_memoryLimit) {
                break;
            }
        }
    }
}

void FluentLazyComponentManager::updateStatistics() {
    QMutexLocker locker(&m_mutex);

    m_stats.totalComponents = static_cast<int>(m_components.size());
    m_stats.loadedComponents = 0;
    m_stats.failedComponents = 0;
    m_stats.preloadedComponents = 0;
    m_stats.unloadedComponents = 0;
    m_stats.totalMemoryUsage = 0;

    std::chrono::milliseconds totalLoadTime{0};
    int loadedCount = 0;

    for (const auto& [id, metadata] : m_components) {
        switch (metadata.state) {
            case FluentLazyComponentState::Loaded:
                m_stats.loadedComponents++;
                totalLoadTime += metadata.loadDuration;
                loadedCount++;
                break;
            case FluentLazyComponentState::Failed:
                m_stats.failedComponents++;
                break;
            case FluentLazyComponentState::Unloaded:
                m_stats.unloadedComponents++;
                break;
            default:
                break;
        }

        m_stats.totalMemoryUsage += metadata.memoryUsage;

        if (metadata.config.enablePreloading) {
            m_stats.preloadedComponents++;
        }
    }

    if (loadedCount > 0) {
        m_stats.totalLoadTime = totalLoadTime;
        m_stats.averageLoadTime = totalLoadTime / loadedCount;
    }

    // Calculate cache hit rate (simplified)
    if (m_stats.totalComponents > 0) {
        m_stats.cacheHitRate = static_cast<double>(m_stats.loadedComponents) / m_stats.totalComponents * 100.0;
    }

    emit statisticsUpdated(m_stats);
}

void FluentLazyComponentManager::cleanupUnusedComponents() {
    unloadUnusedComponents();

    // Clean up failed components after some time
    QMutexLocker locker(&m_mutex);
    auto now = std::chrono::steady_clock::now();

    for (auto it = m_components.begin(); it != m_components.end();) {
        if (it->second.state == FluentLazyComponentState::Failed) {
            auto timeSinceFailure = now - it->second.loadTime;
            if (timeSinceFailure > std::chrono::minutes(5)) {
                it = m_components.erase(it);
                continue;
            }
        }
        ++it;
    }
}

void FluentLazyComponentManager::loadComponentInternal(const QString& componentId) {
    // This is called from processLoadQueue for async loading
    loadComponent(componentId);
}

bool FluentLazyComponentManager::shouldLoadComponent(const QString& componentId) const {
    auto it = m_components.find(componentId);
    if (it == m_components.end()) {
        return false;
    }

    const auto& metadata = it->second;
    const auto& config = metadata.config;

    // Check load condition if specified
    if (config.loadCondition && !config.loadCondition()) {
        return false;
    }

    // Check strategy-specific conditions
    switch (config.strategy) {
        case FluentLazyLoadStrategy::OnVisible:
            return metadata.isVisible;
        case FluentLazyLoadStrategy::OnInteraction:
            // Would need interaction tracking
            return true;
        case FluentLazyLoadStrategy::Proximity:
            return metadata.isInViewport;
        case FluentLazyLoadStrategy::OnDemand:
            return false; // Only load when explicitly requested
        default:
            return true;
    }
}

void FluentLazyComponentManager::updateComponentMetadata(const QString& componentId, QWidget* widget) {
    // Estimate memory usage (simplified)
    auto it = m_components.find(componentId);
    if (it != m_components.end()) {
        // Basic memory estimation - would need more sophisticated calculation
        it->second.memoryUsage = sizeof(*widget) + 1024; // Base estimate

        // Call load complete callback if specified
        if (it->second.config.onLoadComplete) {
            it->second.config.onLoadComplete();
        }
    }
}

} // namespace FluentQt::Core
