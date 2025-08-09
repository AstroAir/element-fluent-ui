// src/Core/FluentBundleAnalyzer.cpp
#include "FluentQt/Core/FluentBundleAnalyzer.h"
#include <QDebug>
#include <QMutexLocker>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QTextStream>
#include <algorithm>

namespace FluentQt::Core {

FluentBundleAnalyzer& FluentBundleAnalyzer::instance() {
    static FluentBundleAnalyzer instance;
    return instance;
}

FluentBundleAnalyzer::FluentBundleAnalyzer() {
    // Initialize monitoring timer
    m_monitoringTimer = new QTimer(this);
    connect(m_monitoringTimer, &QTimer::timeout, this, &FluentBundleAnalyzer::performRealTimeAnalysis);
    
    // Set default configuration
    m_config = AnalysisConfig{};
}

void FluentBundleAnalyzer::registerModule(const QString& name, const QString& path, size_t size) {
    QMutexLocker locker(&m_mutex);
    
    FluentModuleInfo info;
    info.name = name;
    info.path = path;
    info.size = size;
    info.lastModified = std::chrono::steady_clock::now();
    
    // Calculate checksum (simplified)
    info.checksum = QString::number(qHash(path + QString::number(size)));
    
    m_modules[name] = info;
    m_cacheValid = false;
    
    emit moduleRegistered(name);
    qDebug() << "Registered module:" << name << "Size:" << size << "bytes";
}

void FluentBundleAnalyzer::unregisterModule(const QString& name) {
    QMutexLocker locker(&m_mutex);
    
    m_modules.erase(name);
    m_cacheValid = false;
    
    emit moduleUnregistered(name);
    qDebug() << "Unregistered module:" << name;
}

void FluentBundleAnalyzer::updateModuleSize(const QString& name, size_t size) {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_modules.find(name);
    if (it != m_modules.end()) {
        size_t oldSize = it->second.size;
        it->second.size = size;
        it->second.lastModified = std::chrono::steady_clock::now();
        m_cacheValid = false;
        
        emit bundleSizeChanged(oldSize, size);
    }
}

void FluentBundleAnalyzer::setModuleDependencies(const QString& name, const QStringList& dependencies) {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_modules.find(name);
    if (it != m_modules.end()) {
        it->second.dependencies = dependencies;
        m_cacheValid = false;
    }
}

void FluentBundleAnalyzer::markModuleAsCore(const QString& name, bool isCore) {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_modules.find(name);
    if (it != m_modules.end()) {
        it->second.isCore = isCore;
        m_cacheValid = false;
    }
}

void FluentBundleAnalyzer::markModuleAsLazyLoaded(const QString& name, bool isLazy) {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_modules.find(name);
    if (it != m_modules.end()) {
        it->second.isLazyLoaded = isLazy;
        m_cacheValid = false;
    }
}

void FluentBundleAnalyzer::markModuleAsTreeShaken(const QString& name, bool isShaken) {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_modules.find(name);
    if (it != m_modules.end()) {
        it->second.isTreeShaken = isShaken;
        m_cacheValid = false;
    }
}

FluentBundleMetrics FluentBundleAnalyzer::analyzeBundleSize() {
    QMutexLocker locker(&m_mutex);
    
    FluentBundleMetrics metrics;
    metrics.lastAnalysis = std::chrono::steady_clock::now();
    
    for (const auto& [name, info] : m_modules) {
        metrics.totalSize += info.size;
        metrics.compressedSize += info.compressedSize;
        metrics.moduleCount++;
        
        if (info.isCore) {
            metrics.coreSize += info.size;
            metrics.coreModuleCount++;
        }
        
        if (info.isLazyLoaded) {
            metrics.lazyLoadedSize += info.size;
            metrics.lazyModuleCount++;
        }
        
        // Check if module appears unused (no dependents)
        bool hasDependent = false;
        for (const auto& [otherName, otherInfo] : m_modules) {
            if (otherInfo.dependencies.contains(name)) {
                hasDependent = true;
                break;
            }
        }
        
        if (!hasDependent && !info.isCore) {
            metrics.unusedSize += info.size;
            metrics.unusedModuleCount++;
        }
    }
    
    // Calculate ratios
    if (metrics.totalSize > 0) {
        metrics.compressionRatio = static_cast<double>(metrics.compressedSize) / metrics.totalSize;
        metrics.lazyLoadingCoverage = static_cast<double>(metrics.lazyLoadedSize) / metrics.totalSize;
    }
    
    // Calculate tree shaking efficiency
    size_t shakenSize = 0;
    for (const auto& [name, info] : m_modules) {
        if (info.isTreeShaken) {
            shakenSize += info.size;
        }
    }
    
    if (metrics.totalSize > 0) {
        metrics.treeShakingEfficiency = static_cast<double>(shakenSize) / metrics.totalSize;
    }
    
    m_currentMetrics = metrics;
    emit analysisCompleted(metrics);
    
    return metrics;
}

FluentDependencyGraph FluentBundleAnalyzer::analyzeDependencies() {
    QMutexLocker locker(&m_mutex);
    
    FluentDependencyGraph graph;
    
    // Build dependency and dependent maps
    for (const auto& [name, info] : m_modules) {
        graph.dependencies[name] = info.dependencies;
        
        for (const QString& dep : info.dependencies) {
            graph.dependents[dep].append(name);
        }
    }
    
    // Find circular dependencies (simplified detection)
    for (const auto& [name, deps] : graph.dependencies) {
        for (const QString& dep : deps) {
            auto depDeps = graph.dependencies.find(dep);
            if (depDeps != graph.dependencies.end() && depDeps->second.contains(name)) {
                if (!graph.circularDependencies.contains(name) && 
                    !graph.circularDependencies.contains(dep)) {
                    graph.circularDependencies.append(name);
                    emit circularDependencyDetected({name, dep});
                }
            }
        }
    }
    
    // Calculate complexity (average number of dependencies)
    int totalDeps = 0;
    for (const auto& [name, deps] : graph.dependencies) {
        totalDeps += deps.size();
    }
    
    if (!graph.dependencies.empty()) {
        graph.complexity = static_cast<double>(totalDeps) / graph.dependencies.size();
    }
    
    return graph;
}

QList<FluentOptimizationSuggestion> FluentBundleAnalyzer::generateOptimizationSuggestions() {
    if (m_cacheValid && !m_cachedSuggestions.isEmpty()) {
        return m_cachedSuggestions;
    }
    
    QList<FluentOptimizationSuggestion> suggestions;
    
    QMutexLocker locker(&m_mutex);
    
    // Find unused modules
    auto unusedModules = findUnusedModules();
    for (const QString& module : unusedModules) {
        FluentOptimizationSuggestion suggestion;
        suggestion.type = FluentOptimizationSuggestion::RemoveUnusedModule;
        suggestion.module = module;
        suggestion.description = QString("Remove unused module: %1").arg(module);
        suggestion.potentialSavings = m_modules[module].size;
        suggestion.priority = 8;
        suggestion.implementation = QString("Remove module %1 from bundle").arg(module);
        suggestions.append(suggestion);
    }
    
    // Find large modules that could be lazy loaded
    auto largeModules = findLargeModules();
    for (const QString& module : largeModules) {
        auto it = m_modules.find(module);
        if (it != m_modules.end() && !it->second.isLazyLoaded && !it->second.isCore) {
            FluentOptimizationSuggestion suggestion;
            suggestion.type = FluentOptimizationSuggestion::LazyLoadModule;
            suggestion.module = module;
            suggestion.description = QString("Enable lazy loading for large module: %1").arg(module);
            suggestion.potentialSavings = it->second.size;
            suggestion.priority = 6;
            suggestion.implementation = QString("Configure %1 for lazy loading").arg(module);
            suggestions.append(suggestion);
        }
    }
    
    // Find tiny modules that could be merged
    auto tinyModules = findTinyModules();
    if (tinyModules.size() > 1) {
        FluentOptimizationSuggestion suggestion;
        suggestion.type = FluentOptimizationSuggestion::MergeTinyModules;
        suggestion.module = tinyModules.first();
        suggestion.description = QString("Merge %1 tiny modules").arg(tinyModules.size());
        suggestion.affectedModules = tinyModules;
        suggestion.priority = 4;
        suggestion.implementation = "Combine small modules into larger bundles";
        suggestions.append(suggestion);
    }
    
    // Sort by priority
    std::sort(suggestions.begin(), suggestions.end(),
             [](const FluentOptimizationSuggestion& a, const FluentOptimizationSuggestion& b) {
                 return a.priority > b.priority;
             });
    
    m_cachedSuggestions = suggestions;
    m_cacheValid = true;
    
    for (const auto& suggestion : suggestions) {
        emit optimizationSuggestionGenerated(suggestion);
    }
    
    return suggestions;
}

QStringList FluentBundleAnalyzer::findUnusedModules() {
    QStringList unused;
    
    for (const auto& [name, info] : m_modules) {
        if (info.isCore) {
            continue; // Core modules are always considered used
        }
        
        // Check if any other module depends on this one
        bool isUsed = false;
        for (const auto& [otherName, otherInfo] : m_modules) {
            if (otherInfo.dependencies.contains(name)) {
                isUsed = true;
                break;
            }
        }
        
        if (!isUsed) {
            unused.append(name);
            emit unusedModuleDetected(name);
        }
    }
    
    return unused;
}

QStringList FluentBundleAnalyzer::findLargeModules(size_t threshold) {
    QStringList large;
    
    for (const auto& [name, info] : m_modules) {
        if (info.size > threshold) {
            large.append(name);
        }
    }
    
    return large;
}

QStringList FluentBundleAnalyzer::findTinyModules(size_t threshold) {
    QStringList tiny;
    
    for (const auto& [name, info] : m_modules) {
        if (info.size < threshold && !info.isCore) {
            tiny.append(name);
        }
    }
    
    return tiny;
}

QStringList FluentBundleAnalyzer::findCircularDependencies() {
    auto graph = analyzeDependencies();
    return graph.circularDependencies;
}

QStringList FluentBundleAnalyzer::findDuplicatedCode() {
    // Simplified implementation - would need more sophisticated analysis
    QStringList duplicated;
    
    // This would analyze actual code content for duplicates
    // For now, just return empty list
    
    return duplicated;
}

void FluentBundleAnalyzer::startRealTimeMonitoring() {
    m_realTimeMonitoring = true;
    m_monitoringTimer->start(5000); // Update every 5 seconds
}

void FluentBundleAnalyzer::stopRealTimeMonitoring() {
    m_realTimeMonitoring = false;
    m_monitoringTimer->stop();
}

FluentModuleInfo FluentBundleAnalyzer::getModuleInfo(const QString& name) const {
    QMutexLocker locker(&m_mutex);
    
    auto it = m_modules.find(name);
    if (it != m_modules.end()) {
        return it->second;
    }
    
    return FluentModuleInfo{};
}

QList<FluentModuleInfo> FluentBundleAnalyzer::getAllModules() const {
    QMutexLocker locker(&m_mutex);
    
    QList<FluentModuleInfo> result;
    for (const auto& [name, info] : m_modules) {
        result.append(info);
    }
    
    return result;
}

// Private slot implementations
void FluentBundleAnalyzer::performRealTimeAnalysis() {
    if (!m_realTimeMonitoring) {
        return;
    }
    
    auto metrics = analyzeBundleSize();
    emit realTimeMetricsUpdated(metrics);
    
    // Check for significant changes
    if (m_previousMetrics.totalSize > 0) {
        double sizeChange = std::abs(static_cast<double>(metrics.totalSize) - 
                                   static_cast<double>(m_previousMetrics.totalSize)) / 
                           m_previousMetrics.totalSize;
        
        if (sizeChange > 0.1) { // 10% change
            emit bundleSizeChanged(m_previousMetrics.totalSize, metrics.totalSize);
        }
    }
    
    m_previousMetrics = metrics;
}

void FluentBundleAnalyzer::cleanupOldSnapshots() {
    // Keep only recent snapshots (last 10)
    if (m_snapshots.size() > 10) {
        // Remove oldest snapshots
        // Implementation would depend on timestamp tracking
    }
}

// FluentBundleOptimizer implementation
FluentBundleOptimizer::FluentBundleOptimizer(QObject* parent)
    : QObject(parent), m_analyzer(&FluentBundleAnalyzer::instance()) {
    qDebug() << "FluentBundleOptimizer initialized";
}

void FluentBundleOptimizer::optimizeForSize() {
    emit optimizationStarted("Size Optimization");

    OptimizationResult result;
    result.optimization = "Size Optimization";
    result.sizeBefore = m_analyzer->getCurrentMetrics().totalSize;

    auto start = std::chrono::steady_clock::now();

    try {
        // Execute size optimizations
        removeUnusedModules();
        optimizeCompression();
        enableTreeShaking(QStringList{});

        result.success = true;
        result.sizeAfter = m_analyzer->getCurrentMetrics().totalSize;
        result.savings = result.sizeBefore - result.sizeAfter;
    } catch (const std::exception& e) {
        result.success = false;
        result.error = QString::fromStdString(e.what());
    }

    auto end = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    m_results.append(result);

    if (result.success) {
        emit optimizationCompleted(result);
    } else {
        emit optimizationFailed(result.optimization, result.error);
    }
}

void FluentBundleOptimizer::optimizeForSpeed() {
    emit optimizationStarted("Speed Optimization");

    OptimizationResult result;
    result.optimization = "Speed Optimization";
    result.sizeBefore = m_analyzer->getCurrentMetrics().totalSize;

    auto start = std::chrono::steady_clock::now();

    try {
        // Execute speed optimizations
        enableLazyLoading(QStringList{});
        splitLargeModules();

        result.success = true;
        result.sizeAfter = m_analyzer->getCurrentMetrics().totalSize;
        result.savings = result.sizeBefore - result.sizeAfter;
    } catch (const std::exception& e) {
        result.success = false;
        result.error = QString::fromStdString(e.what());
    }

    auto end = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    m_results.append(result);

    if (result.success) {
        emit optimizationCompleted(result);
    } else {
        emit optimizationFailed(result.optimization, result.error);
    }
}

void FluentBundleOptimizer::optimizeForMemory() {
    emit optimizationStarted("Memory Optimization");

    OptimizationResult result;
    result.optimization = "Memory Optimization";
    result.sizeBefore = m_analyzer->getCurrentMetrics().totalSize;

    auto start = std::chrono::steady_clock::now();

    try {
        // Execute memory optimizations
        removeUnusedModules();
        mergeTinyModules();
        enableLazyLoading(QStringList{});

        result.success = true;
        result.sizeAfter = m_analyzer->getCurrentMetrics().totalSize;
        result.savings = result.sizeBefore - result.sizeAfter;
    } catch (const std::exception& e) {
        result.success = false;
        result.error = QString::fromStdString(e.what());
    }

    auto end = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    m_results.append(result);

    if (result.success) {
        emit optimizationCompleted(result);
    } else {
        emit optimizationFailed(result.optimization, result.error);
    }
}

void FluentBundleOptimizer::optimizeForBandwidth() {
    emit optimizationStarted("Bandwidth Optimization");

    OptimizationResult result;
    result.optimization = "Bandwidth Optimization";
    result.sizeBefore = m_analyzer->getCurrentMetrics().totalSize;

    auto start = std::chrono::steady_clock::now();

    try {
        // Execute bandwidth optimizations
        optimizeCompression();
        enableLazyLoading(QStringList{});
        splitLargeModules();

        result.success = true;
        result.sizeAfter = m_analyzer->getCurrentMetrics().totalSize;
        result.savings = result.sizeBefore - result.sizeAfter;
    } catch (const std::exception& e) {
        result.success = false;
        result.error = QString::fromStdString(e.what());
    }

    auto end = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    m_results.append(result);

    if (result.success) {
        emit optimizationCompleted(result);
    } else {
        emit optimizationFailed(result.optimization, result.error);
    }
}

void FluentBundleOptimizer::removeUnusedModules() {
    // Simplified implementation
    qDebug() << "Removing unused modules";
}

void FluentBundleOptimizer::enableLazyLoading(const QStringList& modules) {
    Q_UNUSED(modules);
    // Simplified implementation
    qDebug() << "Enabling lazy loading";
}

void FluentBundleOptimizer::enableTreeShaking(const QStringList& modules) {
    Q_UNUSED(modules);
    // Simplified implementation
    qDebug() << "Enabling tree shaking";
}

void FluentBundleOptimizer::optimizeCompression() {
    // Simplified implementation
    qDebug() << "Optimizing compression";
}

void FluentBundleOptimizer::splitLargeModules(size_t threshold) {
    Q_UNUSED(threshold);
    // Simplified implementation
    qDebug() << "Splitting large modules";
}

void FluentBundleOptimizer::mergeTinyModules(size_t threshold) {
    Q_UNUSED(threshold);
    // Simplified implementation
    qDebug() << "Merging tiny modules";
}

void FluentBundleOptimizer::executeOptimizationPlan(const QList<FluentOptimizationSuggestion>& suggestions) {
    for (const auto& suggestion : suggestions) {
        emit optimizationStarted(suggestion.description);

        OptimizationResult result;
        result.optimization = suggestion.description;
        result.sizeBefore = m_analyzer->getCurrentMetrics().totalSize;

        auto start = std::chrono::steady_clock::now();

        try {
            // Execute the specific optimization
            switch (suggestion.type) {
                case FluentOptimizationSuggestion::RemoveUnusedModule:
                    removeUnusedModules();
                    break;
                case FluentOptimizationSuggestion::LazyLoadModule:
                    enableLazyLoading(QStringList{suggestion.module});
                    break;
                case FluentOptimizationSuggestion::SplitLargeModule:
                    splitLargeModules();
                    break;
                case FluentOptimizationSuggestion::MergeTinyModules:
                    mergeTinyModules();
                    break;
                case FluentOptimizationSuggestion::OptimizeCompression:
                    optimizeCompression();
                    break;
                case FluentOptimizationSuggestion::TreeShakeModule:
                    enableTreeShaking(QStringList{suggestion.module});
                    break;
                default:
                    qDebug() << "Unknown optimization type";
                    break;
            }

            result.success = true;
            result.sizeAfter = m_analyzer->getCurrentMetrics().totalSize;
            result.savings = result.sizeBefore - result.sizeAfter;
        } catch (const std::exception& e) {
            result.success = false;
            result.error = QString::fromStdString(e.what());
        }

        auto end = std::chrono::steady_clock::now();
        result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        m_results.append(result);

        if (result.success) {
            emit optimizationCompleted(result);
        } else {
            emit optimizationFailed(result.optimization, result.error);
        }
    }
}

void FluentBundleOptimizer::createOptimizationPlan() {
    // Generate optimization suggestions and execute them
    auto suggestions = m_analyzer->generateOptimizationSuggestions();
    executeOptimizationPlan(suggestions);
}

} // namespace FluentQt::Core
