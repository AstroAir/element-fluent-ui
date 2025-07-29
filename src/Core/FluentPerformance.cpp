// src/Core/FluentPerformance.cpp
#include "FluentQt/Core/FluentPerformance.h"
#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QMutexLocker>
#include <QSysInfo>
#include <algorithm>

#ifdef Q_OS_WIN
#include <windows.h>
#include <psapi.h>
#elif defined(Q_OS_LINUX)
#include <unistd.h>
#include <fstream>
#elif defined(Q_OS_MACOS)
#include <mach/mach.h>
#include <sys/sysctl.h>
#endif

namespace FluentQt::Core {

FluentPerformanceMonitor& FluentPerformanceMonitor::instance() {
    static FluentPerformanceMonitor instance;
    return instance;
}

FluentPerformanceMonitor::FluentPerformanceMonitor() {
    initializeMonitoring();
}

void FluentPerformanceMonitor::initializeMonitoring() {
    // Initialize timers
    m_frameTimer = new QTimer(this);
    m_performanceTimer = new QTimer(this);
    m_metricsTimer = new QTimer(this);
    m_cpuTimer = new QTimer(this);

    // Connect signals
    connect(m_frameTimer, &QTimer::timeout, this, &FluentPerformanceMonitor::updateFrameRate);
    connect(m_performanceTimer, &QTimer::timeout, this, &FluentPerformanceMonitor::checkPerformance);
    connect(m_metricsTimer, &QTimer::timeout, this, &FluentPerformanceMonitor::updateMetrics);
    connect(m_cpuTimer, &QTimer::timeout, this, &FluentPerformanceMonitor::updateCpuUsage);

    // Initialize metrics
    m_metrics = PerformanceMetrics{};
    trackMemoryUsage();
}

void FluentPerformanceMonitor::startFrameRateMonitoring() {
    m_frameTimeTracker.start();
    m_frameTimer->start(16); // ~60 FPS
    m_performanceTimer->start(1000); // Check every second
    m_metricsTimer->start(500); // Update metrics twice per second
    m_frameCount = 0;
}

void FluentPerformanceMonitor::stopFrameRateMonitoring() {
    m_frameTimer->stop();
    m_performanceTimer->stop();
    m_metricsTimer->stop();
}

void FluentPerformanceMonitor::startCpuMonitoring() {
    m_cpuTimer->start(2000); // Update CPU usage every 2 seconds
}

void FluentPerformanceMonitor::stopCpuMonitoring() {
    m_cpuTimer->stop();
}

// Component registration and monitoring
void FluentPerformanceMonitor::registerComponent(const QString& componentName, QObject* component) {
    QMutexLocker locker(&m_componentMutex);
    m_registeredComponents[componentName] = component;
    m_componentData[componentName] = ComponentPerformanceData{componentName};
}

void FluentPerformanceMonitor::unregisterComponent(const QString& componentName) {
    QMutexLocker locker(&m_componentMutex);
    m_registeredComponents.erase(componentName);
    m_componentData.erase(componentName);
}

void FluentPerformanceMonitor::recordComponentRender(const QString& componentName, std::chrono::milliseconds renderTime) {
    QMutexLocker locker(&m_componentMutex);
    auto& data = m_componentData[componentName];
    data.renderCount++;
    data.totalRenderTime += renderTime;
    data.averageRenderTime = data.totalRenderTime / data.renderCount;
    data.maxRenderTime = std::max(data.maxRenderTime, renderTime);

    // Check for performance issues
    if (renderTime > std::chrono::milliseconds(16)) { // > 16ms = < 60fps
        data.hasPerformanceIssues = true;
        emit componentPerformanceIssue(componentName,
            QString("Slow render: %1ms").arg(renderTime.count()));
    }
}

ComponentPerformanceData FluentPerformanceMonitor::getComponentData(const QString& componentName) const {
    QMutexLocker locker(&m_componentMutex);
    auto it = m_componentData.find(componentName);
    return (it != m_componentData.end()) ? it->second : ComponentPerformanceData{componentName};
}

QList<ComponentPerformanceData> FluentPerformanceMonitor::getAllComponentData() const {
    QMutexLocker locker(&m_componentMutex);
    QList<ComponentPerformanceData> result;
    for (const auto& [name, data] : m_componentData) {
        result.append(data);
    }
    return result;
}

// Memory tracking
size_t FluentPerformanceMonitor::currentMemoryUsage() const {
#ifdef Q_OS_WIN
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
#elif defined(Q_OS_LINUX)
    std::ifstream file("/proc/self/status");
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            return std::stoul(line.substr(7)) * 1024; // Convert KB to bytes
        }
    }
#elif defined(Q_OS_MACOS)
    task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &size) == KERN_SUCCESS) {
        return info.resident_size;
    }
#endif
    return 0;
}

void FluentPerformanceMonitor::trackMemoryUsage() {
    m_metrics.memoryUsage = currentMemoryUsage();
    m_metrics.peakMemoryUsage = std::max(m_metrics.peakMemoryUsage, m_metrics.memoryUsage);
}

void FluentPerformanceMonitor::resetPeakMemoryUsage() {
    m_metrics.peakMemoryUsage = m_metrics.memoryUsage;
}

// Profiling
void FluentPerformanceMonitor::beginProfile(const QString& operation) {
    QMutexLocker locker(&m_profileMutex);
    m_profileTimers[operation].start();
}

void FluentPerformanceMonitor::endProfile(const QString& operation) {
    QMutexLocker locker(&m_profileMutex);
    auto it = m_profileTimers.find(operation);
    if (it != m_profileTimers.end()) {
        auto elapsed = std::chrono::milliseconds(it->second.elapsed());
        m_profileResults[operation] = elapsed;
    }
}

std::chrono::milliseconds FluentPerformanceMonitor::getProfileTime(const QString& operation) const {
    QMutexLocker locker(&m_profileMutex);
    auto it = m_profileResults.find(operation);
    return (it != m_profileResults.end()) ? it->second : std::chrono::milliseconds(0);
}

void FluentPerformanceMonitor::clearProfileData() {
    QMutexLocker locker(&m_profileMutex);
    m_profileTimers.clear();
    m_profileResults.clear();
}

// Benchmarking
void FluentPerformanceMonitor::runBenchmark(const QString& benchmarkName, std::function<void()> benchmark) {
    QElapsedTimer timer;
    size_t initialMemory = currentMemoryUsage();

    timer.start();
    benchmark();
    auto duration = std::chrono::milliseconds(timer.elapsed());

    size_t finalMemory = currentMemoryUsage();
    size_t memoryAllocated = (finalMemory > initialMemory) ? finalMemory - initialMemory : 0;

    BenchmarkResult result{
        benchmarkName,
        duration,
        1000.0 / duration.count(), // Operations per second
        memoryAllocated,
        duration < std::chrono::milliseconds(100), // Pass if < 100ms
        QString("Duration: %1ms, Memory: %2 bytes").arg(duration.count()).arg(memoryAllocated)
    };

    QMutexLocker locker(&m_benchmarkMutex);
    m_benchmarkResults.append(result);
    emit benchmarkCompleted(result);
}

void FluentPerformanceMonitor::runComponentBenchmark(const QString& componentName) {
    Q_UNUSED(componentName)
    // This would run component-specific benchmarks
    // Implementation depends on component type
    // TODO: Implement component-specific benchmarks
}

QList<BenchmarkResult> FluentPerformanceMonitor::getBenchmarkResults() const {
    QMutexLocker locker(&m_benchmarkMutex);
    return m_benchmarkResults;
}

void FluentPerformanceMonitor::clearBenchmarkResults() {
    QMutexLocker locker(&m_benchmarkMutex);
    m_benchmarkResults.clear();
}

// Optimization hints
bool FluentPerformanceMonitor::shouldSkipAnimation() const {
    return m_metrics.frameRate < LOW_FPS_THRESHOLD || m_metrics.isLowPerformanceMode;
}

bool FluentPerformanceMonitor::shouldReduceEffects() const {
    return m_metrics.frameRate < 45.0 || m_metrics.isLowPerformanceMode;
}

int FluentPerformanceMonitor::recommendedAnimationDuration() const {
    if (m_metrics.isLowPerformanceMode) {
        return 50;
    } else if (m_metrics.frameRate >= 60.0) {
        return 250; // Full duration
    } else if (m_metrics.frameRate >= 30.0) {
        return 150; // Reduced duration
    } else {
        return 50;  // Minimal duration
    }
}

bool FluentPerformanceMonitor::shouldUseLowQualityRendering() const {
    return m_metrics.frameRate < CRITICAL_FPS_THRESHOLD ||
           m_metrics.cpuUsage > HIGH_CPU_THRESHOLD ||
           m_metrics.isLowPerformanceMode;
}

int FluentPerformanceMonitor::recommendedMaxAnimations() const {
    if (m_metrics.isLowPerformanceMode) {
        return 1;
    } else if (m_metrics.frameRate >= 60.0) {
        return 10;
    } else if (m_metrics.frameRate >= 30.0) {
        return 5;
    } else {
        return 2;
    }
}

// Performance mode management
void FluentPerformanceMonitor::enableLowPerformanceMode(bool enable) {
    if (m_metrics.isLowPerformanceMode != enable) {
        m_metrics.isLowPerformanceMode = enable;
        if (enable) {
            optimizeForLowPerformance();
        }
        emit metricsUpdated(m_metrics);
    }
}

// Timer callbacks
void FluentPerformanceMonitor::updateFrameRate() {
    m_frameCount++;

    if (m_frameTimeTracker.elapsed() >= 1000) { // Update every second
        m_metrics.frameRate = (m_frameCount * 1000.0) / m_frameTimeTracker.elapsed();
        emit frameRateChanged(m_metrics.frameRate);

        m_frameCount = 0;
        m_frameTimeTracker.restart();
    }
}

void FluentPerformanceMonitor::checkPerformance() {
    detectPerformanceIssues();

    if (m_metrics.frameRate < CRITICAL_FPS_THRESHOLD) {
        emit performanceCritical("Critical frame rate detected. Enabling low performance mode.");
        enableLowPerformanceMode(true);
    } else if (m_metrics.frameRate < LOW_FPS_THRESHOLD) {
        emit performanceWarning("Low frame rate detected. Consider reducing visual effects.");
    }

    if (m_metrics.memoryUsage > HIGH_MEMORY_THRESHOLD) {
        emit performanceWarning(QString("High memory usage: %1 MB")
            .arg(m_metrics.memoryUsage / (1024 * 1024)));
    }

    if (m_metrics.cpuUsage > HIGH_CPU_THRESHOLD) {
        emit performanceWarning(QString("High CPU usage: %1%").arg(m_metrics.cpuUsage));
    }
}

void FluentPerformanceMonitor::updateMetrics() {
    trackMemoryUsage();
    m_metrics.renderCalls++; // This would be updated by components
    emit metricsUpdated(m_metrics);
}

void FluentPerformanceMonitor::updateCpuUsage() {
    // Platform-specific CPU usage calculation
#ifdef Q_OS_WIN
    // Windows implementation would go here
    m_metrics.cpuUsage = 0.0; // Placeholder
#elif defined(Q_OS_LINUX)
    // Linux implementation would go here
    m_metrics.cpuUsage = 0.0; // Placeholder
#elif defined(Q_OS_MACOS)
    // macOS implementation would go here
    m_metrics.cpuUsage = 0.0; // Placeholder
#endif
}

// Private helper methods
void FluentPerformanceMonitor::detectPerformanceIssues() {
    // Check for performance issues across all components
    QMutexLocker locker(&m_componentMutex);
    for (auto& [name, data] : m_componentData) {
        if (data.averageRenderTime > std::chrono::milliseconds(16)) {
            data.hasPerformanceIssues = true;
        }
    }
}

void FluentPerformanceMonitor::optimizeForLowPerformance() {
    // Automatically optimize settings for low performance
    // This could disable certain effects, reduce animation quality, etc.
    qDebug() << "FluentPerformanceMonitor: Optimizing for low performance";
}

} // namespace FluentQt::Core
