// include/FluentQt/Core/FluentModuleLoader.h
#pragma once

#include <QLibrary>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <functional>
#include <memory>
#include <unordered_map>

namespace FluentQt::Core {

// Module loading strategies
enum class FluentLoadingStrategy {
    Immediate,  // Load immediately
    Lazy,       // Load when first accessed
    OnDemand,   // Load only when explicitly requested
    Preload,    // Load during application startup
    Background  // Load in background thread
};

// Module information
struct FluentModuleInfo {
    QString name;
    QString version;
    QString description;
    QStringList dependencies;
    QStringList optionalDependencies;
    QString libraryPath;
    FluentLoadingStrategy loadingStrategy{FluentLoadingStrategy::Lazy};
    bool isLoaded{false};
    bool isRequired{false};
    size_t estimatedSize{0};
    QStringList providedComponents;
};

// Bundle configuration for optimization
struct FluentBundleConfig {
    bool enableLazyLoading{true};
    bool enableCodeSplitting{true};
    bool enableTreeShaking{true};
    bool enableMinification{true};
    bool enableCompression{true};
    bool enableCaching{true};
    int maxBundleSize{1024 * 1024};  // 1MB default
    QStringList excludeFromBundle;
    QStringList alwaysInclude;
};

// Module dependency resolver
class FluentDependencyResolver {
public:
    void addModule(const FluentModuleInfo& moduleInfo);
    void removeModule(const QString& moduleName);

    QStringList resolveDependencies(const QString& moduleName) const;
    QStringList getLoadOrder(const QStringList& moduleNames) const;
    bool hasCircularDependencies(const QString& moduleName) const;
    QStringList findCircularDependencies() const;

    QStringList getRequiredModules() const;
    QStringList getOptionalModules() const;
    size_t calculateBundleSize(const QStringList& moduleNames) const;

private:
    void topologicalSort(const QString& moduleName,
                         std::unordered_map<QString, bool>& visited,
                         std::unordered_map<QString, bool>& recursionStack,
                         QStringList& result) const;

private:
    std::unordered_map<QString, FluentModuleInfo> m_modules;
};

// Dynamic module loader with lazy loading support
class FluentModuleLoader : public QObject {
    Q_OBJECT

public:
    static FluentModuleLoader& instance();

    // Configuration
    void setBundleConfig(const FluentBundleConfig& config);
    FluentBundleConfig getBundleConfig() const { return m_config; }

    // Module registration
    void registerModule(const FluentModuleInfo& moduleInfo);
    void unregisterModule(const QString& moduleName);
    FluentModuleInfo getModuleInfo(const QString& moduleName) const;
    QStringList getRegisteredModules() const;

    // Module loading
    bool loadModule(const QString& moduleName);
    bool unloadModule(const QString& moduleName);
    bool isModuleLoaded(const QString& moduleName) const;
    void preloadModules(const QStringList& moduleNames);
    void loadModulesInBackground(const QStringList& moduleNames);

    // Lazy loading
    void enableLazyLoading(const QString& moduleName, bool enabled = true);
    bool isLazyLoadingEnabled(const QString& moduleName) const;
    void setLazyLoadTrigger(const QString& moduleName,
                            std::function<void()> trigger);

    // Bundle optimization
    void optimizeBundle();
    void enableTreeShaking(bool enabled = true);
    void enableCodeSplitting(bool enabled = true);
    QStringList getUnusedModules() const;
    void removeUnusedModules();

    // Dependency management
    QStringList getModuleDependencies(const QString& moduleName) const;
    bool loadModuleWithDependencies(const QString& moduleName);
    void validateDependencies();

    // Performance monitoring
    struct LoadingMetrics {
        QString moduleName;
        std::chrono::milliseconds loadTime;
        size_t moduleSize;
        bool loadedFromCache;
        QStringList loadedDependencies;
    };

    QList<LoadingMetrics> getLoadingMetrics() const;
    void clearLoadingMetrics();

    // Cache management
    void enableModuleCache(bool enabled = true);
    bool isModuleCacheEnabled() const { return m_cacheEnabled; }
    void clearModuleCache();
    void precompileModules(const QStringList& moduleNames);

signals:
    void moduleLoaded(const QString& moduleName);
    void moduleUnloaded(const QString& moduleName);
    void moduleLoadFailed(const QString& moduleName, const QString& error);
    void dependencyResolved(const QString& moduleName,
                            const QStringList& dependencies);
    void bundleOptimized(const QStringList& removedModules, size_t savedSize);

private slots:
    void onModuleLoadTimeout();
    void processBackgroundLoading();

private:
    FluentModuleLoader();
    bool loadModuleInternal(const QString& moduleName);
    void updateLoadingMetrics(const QString& moduleName,
                              std::chrono::milliseconds loadTime,
                              size_t moduleSize, bool fromCache);

private:
    FluentBundleConfig m_config;
    FluentDependencyResolver m_dependencyResolver;

    // Module management
    std::unordered_map<QString, std::unique_ptr<QLibrary>> m_loadedModules;
    std::unordered_map<QString, std::function<void()>> m_lazyLoadTriggers;

    // Performance tracking
    QList<LoadingMetrics> m_loadingMetrics;
    mutable QMutex m_metricsMutex;

    // Background loading
    QTimer* m_backgroundLoadTimer;
    QStringList m_backgroundLoadQueue;

    // Caching
    bool m_cacheEnabled{true};
    QString m_cacheDirectory;

    // Thread safety
    mutable QMutex m_modulesMutex;
    mutable QMutex m_configMutex;
};

// Code splitting utilities
class FluentCodeSplitter {
public:
    struct SplitPoint {
        QString name;
        QStringList modules;
        FluentLoadingStrategy strategy;
        std::function<bool()> condition;
    };

    void addSplitPoint(const SplitPoint& splitPoint);
    void removeSplitPoint(const QString& name);

    QStringList getSplitPoints() const;
    QStringList getModulesForSplitPoint(const QString& splitPointName) const;

    void evaluateSplitPoints();
    void loadSplitPoint(const QString& splitPointName);

private:
    std::unordered_map<QString, SplitPoint> m_splitPoints;
};

// Tree shaking analyzer
class FluentTreeShaker {
public:
    struct UsageInfo {
        QString symbol;
        QString module;
        bool isUsed{false};
        QStringList usedBy;
        QStringList dependencies;
    };

    void analyzeUsage(const QString& rootModule);
    void markSymbolAsUsed(const QString& symbol, const QString& usedBy);

    QStringList getUnusedSymbols() const;
    QStringList getUnusedModules() const;
    void removeUnusedCode();

    void generateUsageReport(const QString& filename) const;

private:
    void traverseDependencies(const QString& symbol);

private:
    std::unordered_map<QString, UsageInfo> m_usageMap;
    QStringList m_rootSymbols;
};

// Bundle optimizer
class FluentBundleOptimizer {
public:
    explicit FluentBundleOptimizer(const FluentBundleConfig& config);

    // Optimization strategies
    void optimizeForSize();
    void optimizeForSpeed();
    void optimizeForMemory();

    // Analysis
    struct BundleAnalysis {
        size_t totalSize{0};
        size_t compressedSize{0};
        int moduleCount{0};
        QStringList largestModules;
        QStringList unusedModules;
        QStringList duplicatedCode;
        double compressionRatio{0.0};
    };

    BundleAnalysis analyzeBundleSize() const;
    QStringList findDuplicatedCode() const;
    QStringList suggestOptimizations() const;

    // Compression
    void enableCompression(bool enabled = true);
    void setCompressionLevel(int level);  // 1-9

    // Minification
    void enableMinification(bool enabled = true);
    void minifyBundle();

private:
    FluentBundleConfig m_config;
    FluentTreeShaker m_treeShaker;
    FluentCodeSplitter m_codeSplitter;
};

// Lazy loading utilities
class FluentLazyLoader {
public:
    template <typename T>
    static std::function<T*()> createLazyFactory(const QString& moduleName) {
        return [moduleName]() -> T* {
            auto& loader = FluentModuleLoader::instance();
            if (!loader.isModuleLoaded(moduleName)) {
                loader.loadModule(moduleName);
            }
            return new T();
        };
    }

    template <typename T>
    static std::shared_ptr<T> createLazyShared(const QString& moduleName) {
        static std::weak_ptr<T> instance;

        if (auto shared = instance.lock()) {
            return shared;
        }

        auto& loader = FluentModuleLoader::instance();
        if (!loader.isModuleLoaded(moduleName)) {
            loader.loadModule(moduleName);
        }

        auto newInstance = std::make_shared<T>();
        instance = newInstance;
        return newInstance;
    }
};

// Convenience macros
#define FLUENT_LAZY_MODULE(moduleName) \
    FluentQt::Core::FluentModuleLoader::instance().enableLazyLoading(moduleName)

#define FLUENT_LOAD_MODULE(moduleName) \
    FluentQt::Core::FluentModuleLoader::instance().loadModule(moduleName)

#define FLUENT_LAZY_COMPONENT(ComponentType, moduleName)                \
    FluentQt::Core::FluentLazyLoader::createLazyFactory<ComponentType>( \
        moduleName)

#define FLUENT_SPLIT_POINT(name, modules, strategy)           \
    do {                                                      \
        FluentQt::Core::FluentCodeSplitter::SplitPoint point; \
        point.name = name;                                    \
        point.modules = modules;                              \
        point.strategy = strategy;                            \
        /* Add to splitter */                                 \
    } while (0)

}  // namespace FluentQt::Core
