// include/FluentQt/Core/FluentPerformance.h
#pragma once

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QMutex>
#include <QThread>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <memory>
#include <atomic>

namespace FluentQt::Core {

// Performance metrics structure
struct PerformanceMetrics {
    double frameRate{60.0};
    size_t memoryUsage{0};
    size_t peakMemoryUsage{0};
    int activeAnimations{0};
    int renderCalls{0};
    std::chrono::milliseconds averageRenderTime{0};
    std::chrono::milliseconds maxRenderTime{0};
    double cpuUsage{0.0};
    bool isLowPerformanceMode{false};
};

// Component-specific performance data
struct ComponentPerformanceData {
    QString componentName;
    int renderCount{0};
    std::chrono::milliseconds totalRenderTime{0};
    std::chrono::milliseconds averageRenderTime{0};
    std::chrono::milliseconds maxRenderTime{0};
    size_t memoryFootprint{0};
    int animationCount{0};
    bool hasPerformanceIssues{false};
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
    void recordComponentRender(const QString& componentName, std::chrono::milliseconds renderTime);
    ComponentPerformanceData getComponentData(const QString& componentName) const;
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
    void runBenchmark(const QString& benchmarkName, std::function<void()> benchmark);
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
    bool isLowPerformanceModeEnabled() const { return m_metrics.isLowPerformanceMode; }

    // CPU monitoring
    void startCpuMonitoring();
    void stopCpuMonitoring();
    double currentCpuUsage() const { return m_metrics.cpuUsage; }

signals:
    void frameRateChanged(double fps);
    void performanceWarning(const QString& message);
    void performanceCritical(const QString& message);
    void metricsUpdated(const PerformanceMetrics& metrics);
    void componentPerformanceIssue(const QString& componentName, const QString& issue);
    void benchmarkCompleted(const BenchmarkResult& result);

private slots:
    void updateFrameRate();
    void checkPerformance();
    void updateMetrics();
    void updateCpuUsage();

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
    QElapsedTimer m_frameTimeTracker;

    // Metrics and data
    PerformanceMetrics m_metrics;
    std::atomic<int> m_frameCount{0};

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
    static constexpr size_t HIGH_MEMORY_THRESHOLD = 512 * 1024 * 1024; // 512MB
    static constexpr double HIGH_CPU_THRESHOLD = 80.0;
};

// Enhanced RAII Performance Profiler
class FluentProfiler {
public:
    explicit FluentProfiler(const QString& operation)
        : m_operation(operation) {
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
        : m_componentName(componentName)
        , m_timer() {
        m_timer.start();
    }

    ~FluentComponentProfiler() {
        auto elapsed = std::chrono::milliseconds(m_timer.elapsed());
        FluentPerformanceMonitor::instance().recordComponentRender(m_componentName, elapsed);
    }

private:
    QString m_componentName;
    QElapsedTimer m_timer;
};

// Memory usage tracker
class FluentMemoryTracker {
public:
    FluentMemoryTracker() {
        m_initialMemory = FluentPerformanceMonitor::instance().currentMemoryUsage();
    }

    ~FluentMemoryTracker() {
        auto currentMemory = FluentPerformanceMonitor::instance().currentMemoryUsage();
        if (currentMemory > m_initialMemory + 1024 * 1024) { // 1MB threshold
            auto& monitor = FluentPerformanceMonitor::instance();
            emit monitor.performanceWarning(
                QString("Memory leak detected: %1 MB allocated")
                .arg((currentMemory - m_initialMemory) / (1024 * 1024))
            );
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

} // namespace FluentQt::Core