// include/FluentQt/Core/FluentPerformance.h
#pragma once

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <chrono>
#include <unordered_map>
#include <functional>

namespace FluentQt::Core {

class FluentPerformanceMonitor : public QObject {
    Q_OBJECT

public:
    static FluentPerformanceMonitor& instance();

    // Frame rate monitoring
    void startFrameRateMonitoring();
    void stopFrameRateMonitoring();
    double currentFrameRate() const { return m_currentFPS; }

    // Memory usage tracking
    void trackMemoryUsage();
    size_t currentMemoryUsage() const;

    // Performance profiling
    void beginProfile(const QString& operation);
    void endProfile(const QString& operation);
    std::chrono::milliseconds getProfileTime(const QString& operation) const;

    // Render optimization hints
    bool shouldSkipAnimation() const;
    bool shouldReduceEffects() const;
    int recommendedAnimationDuration() const;

signals:
    void frameRateChanged(double fps);
    void performanceWarning(const QString& message);

private slots:
    void updateFrameRate();
    void checkPerformance();

private:
    FluentPerformanceMonitor() = default;
    void initializeMonitoring();

private:
    QTimer* m_frameTimer;
    QTimer* m_performanceTimer;
    QElapsedTimer m_frameTimeTracker;
    
    double m_currentFPS{60.0};
    int m_frameCount{0};
    
    std::unordered_map<QString, QElapsedTimer> m_profileTimers;
    std::unordered_map<QString, std::chrono::milliseconds> m_profileResults;
};

// RAII Performance Profiler
class FluentProfiler {
public:
    explicit FluentProfiler(const QString& operation)
        : m_operation(operation) {
        FluentPerformanceMonitor::instance().beginProfile(m_operation);
    }
    
    ~FluentProfiler() {
        FluentPerformanceMonitor::instance().endProfile(m_operation);
    }

private:
    QString m_operation;
};

#define FLUENT_PROFILE(operation) \
    FluentQt::Core::FluentProfiler _profiler(operation)

} // namespace FluentQt::Core