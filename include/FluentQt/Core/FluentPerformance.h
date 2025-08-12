// include/FluentQt/Core/FluentPerformance.h
#pragma once

#include <QElapsedTimer>
#include <QMutex>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>

namespace FluentQt::Core {

// Enhanced performance metrics structure
struct PerformanceMetrics {
    double frameRate{60.0};
    double targetFrameRate{60.0};
    size_t memoryUsage{0};
    size_t peakMemoryUsage{0};
    size_t memoryLeaks{0};
    int activeAnimations{0};
    int renderCalls{0};
    int skippedFrames{0};
    std::chrono::milliseconds averageRenderTime{0};
    std::chrono::milliseconds maxRenderTime{0};
    std::chrono::milliseconds minRenderTime{std::chrono::milliseconds::max()};
    double cpuUsage{0.0};
    double gpuUsage{0.0};
    double memoryPressure{0.0};
    double thermalState{0.0};
    bool isLowPerformanceMode{false};
    bool isThrottling{false};

    // Network and I/O metrics
    size_t networkBytesReceived{0};
    size_t networkBytesSent{0};
    std::chrono::milliseconds networkLatency{0};
    size_t diskReads{0};
    size_t diskWrites{0};

    // Component-specific metrics
    int componentCount{0};
    int visibleComponents{0};
    int animatingComponents{0};

    // User interaction metrics
    std::chrono::milliseconds inputLatency{0};
    double scrollPerformance{100.0};
    int droppedInputEvents{0};

    // Battery and power metrics
    double batteryLevel{100.0};
    bool isLowPowerMode{false};
    double powerConsumption{0.0};
};

// Enhanced component-specific performance data
struct ComponentPerformanceData {
    QString componentName;
    QString componentType;
    int renderCount{0};
    int updateCount{0};
    int layoutCount{0};
    std::chrono::milliseconds totalRenderTime{0};
    std::chrono::milliseconds averageRenderTime{0};
    std::chrono::milliseconds maxRenderTime{0};
    std::chrono::milliseconds minRenderTime{std::chrono::milliseconds::max()};
    std::chrono::milliseconds totalUpdateTime{0};
    std::chrono::milliseconds totalLayoutTime{0};
    size_t memoryFootprint{0};
    size_t peakMemoryFootprint{0};
    int animationCount{0};
    int activeAnimationCount{0};
    bool hasPerformanceIssues{false};
    bool isVisible{true};
    bool isLazyLoaded{false};
    std::chrono::steady_clock::time_point creationTime;
    std::chrono::steady_clock::time_point lastRenderTime;

    // Performance scores (0-100)
    double renderPerformanceScore{100.0};
    double memoryEfficiencyScore{100.0};
    double overallPerformanceScore{100.0};

    // Optimization suggestions
    QStringList optimizationSuggestions;

    // Usage statistics
    int userInteractions{0};
    std::chrono::milliseconds totalInteractionTime{0};
    double averageInteractionLatency{0.0};
};

// Performance benchmark results
struct BenchmarkResult {
    QString testName;
    std::chrono::milliseconds duration;
    double operationsPerSecond;
    size_t memoryAllocated;
    bool passed;
    QString details;
};

class FluentPerformanceMonitor : public QObject {
    Q_OBJECT

public:
    static FluentPerformanceMonitor& instance();

    // Enhanced frame rate monitoring
    void startFrameRateMonitoring();
    void stopFrameRateMonitoring();
    double currentFrameRate() const { return m_metrics.frameRate; }

    // Comprehensive metrics
    const PerformanceMetrics& currentMetrics() const { return m_metrics; }

    // Component-specific monitoring
    void registerComponent(const QString& componentName, QObject* component);
    void unregisterComponent(const QString& componentName);
    void recordComponentRender(const QString& componentName,
                               std::chrono::milliseconds renderTime);
    ComponentPerformanceData getComponentData(
        const QString& componentName) const;
    QList<ComponentPerformanceData> getAllComponentData() const;

    // Memory usage tracking
    void trackMemoryUsage();
    size_t currentMemoryUsage() const;
    size_t peakMemoryUsage() const { return m_metrics.peakMemoryUsage; }
    void resetPeakMemoryUsage();

    // Performance profiling
    void beginProfile(const QString& operation);
    void endProfile(const QString& operation);
    std::chrono::milliseconds getProfileTime(const QString& operation) const;
    void clearProfileData();

    // Benchmarking system
    void runBenchmark(const QString& benchmarkName,
                      std::function<void()> benchmark);
    void runComponentBenchmark(const QString& componentName);
    QList<BenchmarkResult> getBenchmarkResults() const;
    void clearBenchmarkResults();

    // Render optimization hints
    bool shouldSkipAnimation() const;
    bool shouldReduceEffects() const;
    int recommendedAnimationDuration() const;
    bool shouldUseLowQualityRendering() const;
    int recommendedMaxAnimations() const;

    // Performance mode management
    void enableLowPerformanceMode(bool enable = true);
    bool isLowPerformanceModeEnabled() const {
        return m_metrics.isLowPerformanceMode;
    }

    // CPU monitoring
    void startCpuMonitoring();
    void stopCpuMonitoring();
    double currentCpuUsage() const { return m_metrics.cpuUsage; }

    // GPU monitoring
    void startGpuMonitoring();
    void stopGpuMonitoring();
    double currentGpuUsage() const { return m_metrics.gpuUsage; }

    // Memory pressure monitoring
    void startMemoryPressureMonitoring();
    void stopMemoryPressureMonitoring();
    double currentMemoryPressure() const { return m_metrics.memoryPressure; }

    // Thermal monitoring
    void startThermalMonitoring();
    void stopThermalMonitoring();
    double currentThermalState() const { return m_metrics.thermalState; }

    // Network monitoring
    void startNetworkMonitoring();
    void stopNetworkMonitoring();
    std::chrono::milliseconds currentNetworkLatency() const {
        return m_metrics.networkLatency;
    }

    // Input latency monitoring
    void recordInputEvent(const QString& eventType);
    void recordInputLatency(std::chrono::milliseconds latency);
    std::chrono::milliseconds averageInputLatency() const {
        return m_metrics.inputLatency;
    }

    // Battery monitoring
    void startBatteryMonitoring();
    void stopBatteryMonitoring();
    double currentBatteryLevel() const { return m_metrics.batteryLevel; }
    bool isLowPowerMode() const { return m_metrics.isLowPowerMode; }

    // Advanced component monitoring
    void recordComponentUpdate(const QString& componentName,
                               std::chrono::milliseconds updateTime);
    void recordComponentLayout(const QString& componentName,
                               std::chrono::milliseconds layoutTime);
    void recordComponentInteraction(const QString& componentName,
                                    std::chrono::milliseconds responseTime);
    void setComponentVisibility(const QString& componentName, bool visible);
    void markComponentAsLazyLoaded(const QString& componentName,
                                   bool lazyLoaded);

    // Performance scoring
    double calculateComponentPerformanceScore(
        const QString& componentName) const;
    double calculateOverallPerformanceScore() const;
    QStringList generateOptimizationSuggestions(
        const QString& componentName) const;
    QStringList generateGlobalOptimizationSuggestions() const;

signals:
    void frameRateChanged(double fps);
    void performanceWarning(const QString& message);
    void performanceCritical(const QString& message);
    void metricsUpdated(const PerformanceMetrics& metrics);
    void componentPerformanceIssue(const QString& componentName,
                                   const QString& issue);
    void benchmarkCompleted(const BenchmarkResult& result);

    // Enhanced monitoring signals
    void memoryPressureChanged(double pressure);
    void thermalStateChanged(double temperature);
    void batteryLevelChanged(double level);
    void lowPowerModeChanged(bool enabled);
    void networkLatencyChanged(std::chrono::milliseconds latency);
    void inputLatencyWarning(std::chrono::milliseconds latency);
    void componentOptimizationSuggestion(const QString& componentName,
                                         const QStringList& suggestions);
    void performanceScoreChanged(const QString& componentName, double score);
    void memoryLeakDetected(const QString& componentName, size_t leakSize);
    void throttlingDetected(const QString& reason);
    void performanceDegradation(const QString& metric,
                                double degradationPercent);

private slots:
    void updateFrameRate();
    void checkPerformance();
    void updateMetrics();
    void updateCpuUsage();
    void updateGpuUsage();
    void updateMemoryPressure();
    void updateThermalState();
    void updateNetworkMetrics();
    void updateBatteryStatus();
    void analyzePerformanceTrends();
    void detectPerformanceAnomalies();
    void cleanupStaleData();

private:
    FluentPerformanceMonitor();
    void initializeMonitoring();
    void detectPerformanceIssues();
    void optimizeForLowPerformance();

private:
    // Timers and monitoring
    QTimer* m_frameTimer;
    QTimer* m_performanceTimer;
    QTimer* m_metricsTimer;
    QTimer* m_cpuTimer;
    QTimer* m_gpuTimer;
    QTimer* m_memoryPressureTimer;
    QTimer* m_thermalTimer;
    QTimer* m_networkTimer;
    QTimer* m_batteryTimer;
    QTimer* m_analysisTimer;
    QElapsedTimer m_frameTimeTracker;

    // Metrics and data
    PerformanceMetrics m_metrics;
    PerformanceMetrics m_previousMetrics;
    std::atomic<int> m_frameCount{0};

    // Performance history for trend analysis
    QList<PerformanceMetrics> m_metricsHistory;
    static constexpr int MAX_HISTORY_SIZE = 100;

    // Input event tracking
    QList<std::chrono::steady_clock::time_point> m_inputEventTimes;
    QList<std::chrono::milliseconds> m_inputLatencies;

    // Component tracking
    mutable QMutex m_componentMutex;
    std::unordered_map<QString, ComponentPerformanceData> m_componentData;
    std::unordered_map<QString, QObject*> m_registeredComponents;

    // Profiling data
    mutable QMutex m_profileMutex;
    std::unordered_map<QString, QElapsedTimer> m_profileTimers;
    std::unordered_map<QString, std::chrono::milliseconds> m_profileResults;

    // Benchmark data
    mutable QMutex m_benchmarkMutex;
    QList<BenchmarkResult> m_benchmarkResults;

    // Performance thresholds
    static constexpr double LOW_FPS_THRESHOLD = 30.0;
    static constexpr double CRITICAL_FPS_THRESHOLD = 15.0;
    static constexpr size_t HIGH_MEMORY_THRESHOLD = 512 * 1024 * 1024;  // 512MB
    static constexpr double HIGH_CPU_THRESHOLD = 80.0;
    static constexpr double HIGH_GPU_THRESHOLD = 85.0;
    static constexpr double HIGH_MEMORY_PRESSURE_THRESHOLD = 0.8;
    static constexpr double HIGH_THERMAL_THRESHOLD = 80.0;
    static constexpr std::chrono::milliseconds HIGH_NETWORK_LATENCY_THRESHOLD{
        100};
    static constexpr std::chrono::milliseconds HIGH_INPUT_LATENCY_THRESHOLD{
        16};  // 60fps = 16ms
    static constexpr double LOW_BATTERY_THRESHOLD = 20.0;
    static constexpr size_t MEMORY_LEAK_THRESHOLD = 10 * 1024 * 1024;  // 10MB

    // Performance monitoring state
    bool m_gpuMonitoringEnabled{false};
    bool m_memoryPressureMonitoringEnabled{false};
    bool m_thermalMonitoringEnabled{false};
    bool m_networkMonitoringEnabled{false};
    bool m_batteryMonitoringEnabled{false};

    // Platform-specific monitoring handles
    void* m_platformMonitoringHandle{nullptr};

    // Helper methods
    void initializePlatformMonitoring();
    void cleanupPlatformMonitoring();
    double calculatePerformanceTrend(const QString& metric) const;
    bool detectPerformanceAnomaly(const PerformanceMetrics& current,
                                  const PerformanceMetrics& baseline) const;
};

// Enhanced RAII Performance Profiler
class FluentProfiler {
public:
    explicit FluentProfiler(const QString& operation) : m_operation(operation) {
        FluentPerformanceMonitor::instance().beginProfile(m_operation);
    }

    ~FluentProfiler() {
        FluentPerformanceMonitor::instance().endProfile(m_operation);
    }

    // Get elapsed time so far
    std::chrono::milliseconds elapsed() const {
        return FluentPerformanceMonitor::instance().getProfileTime(m_operation);
    }

private:
    QString m_operation;
};

// Component-specific profiler
class FluentComponentProfiler {
public:
    explicit FluentComponentProfiler(const QString& componentName)
        : m_componentName(componentName), m_timer() {
        m_timer.start();
    }

    ~FluentComponentProfiler() {
        auto elapsed = std::chrono::milliseconds(m_timer.elapsed());
        FluentPerformanceMonitor::instance().recordComponentRender(
            m_componentName, elapsed);
    }

private:
    QString m_componentName;
    QElapsedTimer m_timer;
};

// Memory usage tracker
class FluentMemoryTracker {
public:
    FluentMemoryTracker() {
        m_initialMemory =
            FluentPerformanceMonitor::instance().currentMemoryUsage();
    }

    ~FluentMemoryTracker() {
        auto currentMemory =
            FluentPerformanceMonitor::instance().currentMemoryUsage();
        if (currentMemory > m_initialMemory + 1024 * 1024) {  // 1MB threshold
            auto& monitor = FluentPerformanceMonitor::instance();
            emit monitor.performanceWarning(
                QString("Memory leak detected: %1 MB allocated")
                    .arg((currentMemory - m_initialMemory) / (1024 * 1024)));
        }
    }

private:
    size_t m_initialMemory;
};

// Convenience macros
#define FLUENT_PROFILE(operation) \
    FluentQt::Core::FluentProfiler _profiler(operation)

#define FLUENT_PROFILE_COMPONENT(componentName) \
    FluentQt::Core::FluentComponentProfiler _componentProfiler(componentName)

#define FLUENT_TRACK_MEMORY() \
    FluentQt::Core::FluentMemoryTracker _memoryTracker()

}  // namespace FluentQt::Core
