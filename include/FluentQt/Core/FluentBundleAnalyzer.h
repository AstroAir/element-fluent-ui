// include/FluentQt/Core/FluentBundleAnalyzer.h
#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QMutex>
#include <chrono>
#include <unordered_map>
#include <memory>

namespace FluentQt::Core {

// Bundle analysis data structures
struct FluentModuleInfo {
    QString name;
    QString path;
    size_t size{0};
    size_t compressedSize{0};
    QStringList dependencies;
    QStringList dependents;
    bool isCore{false};
    bool isLazyLoaded{false};
    bool isTreeShaken{false};
    double compressionRatio{0.0};
    std::chrono::steady_clock::time_point lastModified;
    QString checksum;
};

struct FluentBundleMetrics {
    size_t totalSize{0};
    size_t compressedSize{0};
    size_t coreSize{0};
    size_t lazyLoadedSize{0};
    size_t unusedSize{0};
    int moduleCount{0};
    int coreModuleCount{0};
    int lazyModuleCount{0};
    int unusedModuleCount{0};
    double compressionRatio{0.0};
    double treeShakingEfficiency{0.0};
    double lazyLoadingCoverage{0.0};
    std::chrono::milliseconds buildTime{0};
    std::chrono::steady_clock::time_point lastAnalysis;
};

struct FluentDependencyGraph {
    std::unordered_map<QString, QStringList> dependencies;
    std::unordered_map<QString, QStringList> dependents;
    QStringList circularDependencies;
    int maxDepth{0};
    double complexity{0.0};
};

struct FluentOptimizationSuggestion {
    enum Type {
        RemoveUnusedModule,
        LazyLoadModule,
        SplitLargeModule,
        MergeTinyModules,
        OptimizeCompression,
        TreeShakeModule,
        CacheModule,
        PreloadModule
    };
    
    Type type;
    QString module;
    QString description;
    size_t potentialSavings{0};
    int priority{0}; // 1-10, 10 being highest
    QStringList affectedModules;
    QString implementation;
};

// Bundle analyzer class
class FluentBundleAnalyzer : public QObject {
    Q_OBJECT

public:
    static FluentBundleAnalyzer& instance();
    
    // Analysis configuration
    struct AnalysisConfig {
        bool enableDeepAnalysis{true};
        bool enableDependencyAnalysis{true};
        bool enableCompressionAnalysis{true};
        bool enableTreeShakingAnalysis{true};
        bool enableLazyLoadingAnalysis{true};
        bool enablePerformanceAnalysis{true};
        bool generateOptimizationSuggestions{true};
        QString outputDirectory{"./bundle_analysis"};
        bool saveDetailedReport{true};
        bool saveJsonReport{true};
        bool saveHtmlReport{true};
    };
    
    void setAnalysisConfig(const AnalysisConfig& config) { m_config = config; }
    AnalysisConfig getAnalysisConfig() const { return m_config; }
    
    // Module registration and tracking
    void registerModule(const QString& name, const QString& path, size_t size);
    void unregisterModule(const QString& name);
    void updateModuleSize(const QString& name, size_t size);
    void setModuleDependencies(const QString& name, const QStringList& dependencies);
    void markModuleAsCore(const QString& name, bool isCore = true);
    void markModuleAsLazyLoaded(const QString& name, bool isLazy = true);
    void markModuleAsTreeShaken(const QString& name, bool isShaken = true);
    
    // Analysis methods
    FluentBundleMetrics analyzeBundleSize();
    FluentDependencyGraph analyzeDependencies();
    QList<FluentOptimizationSuggestion> generateOptimizationSuggestions();
    
    // Specific analyses
    QStringList findUnusedModules();
    QStringList findLargeModules(size_t threshold = 1024 * 1024); // 1MB default
    QStringList findTinyModules(size_t threshold = 10 * 1024); // 10KB default
    QStringList findCircularDependencies();
    QStringList findDuplicatedCode();
    
    // Compression analysis
    struct CompressionAnalysis {
        QString algorithm;
        double ratio{0.0};
        size_t originalSize{0};
        size_t compressedSize{0};
        std::chrono::milliseconds compressionTime{0};
        std::chrono::milliseconds decompressionTime{0};
    };
    
    QList<CompressionAnalysis> analyzeCompressionOptions();
    CompressionAnalysis getBestCompressionStrategy();
    
    // Tree shaking analysis
    struct TreeShakingAnalysis {
        QString module;
        size_t originalSize{0};
        size_t shakenSize{0};
        size_t removedSize{0};
        double efficiency{0.0};
        QStringList removedSymbols;
        QStringList keptSymbols;
    };
    
    QList<TreeShakingAnalysis> analyzeTreeShakingOpportunities();
    size_t estimateTreeShakingSavings();
    
    // Lazy loading analysis
    struct LazyLoadingAnalysis {
        QString module;
        size_t size{0};
        bool isCandidate{false};
        QString reason;
        size_t potentialSavings{0};
        QStringList dependencies;
        double loadFrequency{0.0};
    };
    
    QList<LazyLoadingAnalysis> analyzeLazyLoadingOpportunities();
    QStringList suggestLazyLoadingCandidates();
    
    // Performance impact analysis
    struct PerformanceImpact {
        QString optimization;
        std::chrono::milliseconds loadTimeImprovement{0};
        size_t memorySavings{0};
        double performanceScore{0.0};
        QString description;
    };
    
    QList<PerformanceImpact> analyzePerformanceImpact();
    
    // Reporting
    void generateDetailedReport(const QString& filename = "");
    void generateJsonReport(const QString& filename = "");
    void generateHtmlReport(const QString& filename = "");
    QJsonObject exportAnalysisData();
    void importAnalysisData(const QJsonObject& data);
    
    // Real-time monitoring
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoringEnabled() const { return m_realTimeMonitoring; }
    
    // Historical analysis
    void saveAnalysisSnapshot(const QString& label = "");
    QStringList getAnalysisSnapshots() const;
    FluentBundleMetrics getSnapshotMetrics(const QString& snapshot) const;
    void compareSnapshots(const QString& snapshot1, const QString& snapshot2);
    
    // Module information
    FluentModuleInfo getModuleInfo(const QString& name) const;
    QList<FluentModuleInfo> getAllModules() const;
    FluentBundleMetrics getCurrentMetrics() const { return m_currentMetrics; }
    
    // Optimization execution
    void executeOptimization(const FluentOptimizationSuggestion& suggestion);
    void executeAllOptimizations(int minPriority = 5);
    void undoLastOptimization();

signals:
    void analysisCompleted(const FluentBundleMetrics& metrics);
    void optimizationSuggestionGenerated(const FluentOptimizationSuggestion& suggestion);
    void moduleRegistered(const QString& name);
    void moduleUnregistered(const QString& name);
    void bundleSizeChanged(size_t oldSize, size_t newSize);
    void unusedModuleDetected(const QString& module);
    void circularDependencyDetected(const QStringList& cycle);
    void optimizationExecuted(const FluentOptimizationSuggestion& suggestion, bool success);
    void realTimeMetricsUpdated(const FluentBundleMetrics& metrics);

private slots:
    void performRealTimeAnalysis();
    void cleanupOldSnapshots();

private:
    FluentBundleAnalyzer();
    void updateMetrics();
    void detectChanges();
    QString generateReportHtml(const FluentBundleMetrics& metrics,
                              const QList<FluentOptimizationSuggestion>& suggestions);
    
private:
    AnalysisConfig m_config;
    std::unordered_map<QString, FluentModuleInfo> m_modules;
    FluentBundleMetrics m_currentMetrics;
    FluentBundleMetrics m_previousMetrics;
    
    // Real-time monitoring
    bool m_realTimeMonitoring{false};
    QTimer* m_monitoringTimer;
    
    // Historical data
    std::unordered_map<QString, FluentBundleMetrics> m_snapshots;
    QStringList m_optimizationHistory;
    
    // Thread safety
    mutable QMutex m_mutex;
    
    // Analysis cache
    std::chrono::steady_clock::time_point m_lastAnalysis;
    QList<FluentOptimizationSuggestion> m_cachedSuggestions;
    bool m_cacheValid{false};
};

// Bundle optimization executor
class FluentBundleOptimizer : public QObject {
    Q_OBJECT

public:
    explicit FluentBundleOptimizer(QObject* parent = nullptr);
    
    // Optimization strategies
    void optimizeForSize();
    void optimizeForSpeed();
    void optimizeForMemory();
    void optimizeForBandwidth();
    
    // Specific optimizations
    void removeUnusedModules();
    void enableLazyLoading(const QStringList& modules);
    void enableTreeShaking(const QStringList& modules);
    void optimizeCompression();
    void splitLargeModules(size_t threshold = 1024 * 1024);
    void mergeTinyModules(size_t threshold = 10 * 1024);
    
    // Batch optimization
    void executeOptimizationPlan(const QList<FluentOptimizationSuggestion>& suggestions);
    void createOptimizationPlan();
    
    // Results tracking
    struct OptimizationResult {
        QString optimization;
        bool success{false};
        size_t sizeBefore{0};
        size_t sizeAfter{0};
        size_t savings{0};
        std::chrono::milliseconds executionTime{0};
        QString error;
    };
    
    QList<OptimizationResult> getOptimizationResults() const { return m_results; }
    void clearResults() { m_results.clear(); }

signals:
    void optimizationStarted(const QString& optimization);
    void optimizationCompleted(const OptimizationResult& result);
    void optimizationFailed(const QString& optimization, const QString& error);

private:
    QList<OptimizationResult> m_results;
    FluentBundleAnalyzer* m_analyzer;
};

} // namespace FluentQt::Core
