// src/Core/FluentPerformance.cpp
#include "FluentQt/Core/FluentPerformance.h"
#include <QApplication>
#include <QProcess>
#include <QDebug>

namespace FluentQt::Core {

FluentPerformanceMonitor& FluentPerformanceMonitor::instance() {
    static FluentPerformanceMonitor instance;
    return instance;
}

void FluentPerformanceMonitor::startFrameRateMonitoring() {
    if (!m_frameTimer) {
        m_frameTimer = new QTimer(this);
        connect(m_frameTimer, &QTimer::timeout, this, &FluentPerformanceMonitor::updateFrameRate);
    }
    
    if (!m_performanceTimer) {
        m_performanceTimer = new QTimer(this);
        connect(m_performanceTimer, &QTimer::timeout, this, &FluentPerformanceMonitor::checkPerformance);
    }
    
    m_frameTimeTracker.start();
    m_frameTimer->start(16); // ~60 FPS monitoring
    m_performanceTimer->start(1000); // Check performance every second
    m_frameCount = 0;
}

void FluentPerformanceMonitor::stopFrameRateMonitoring() {
    if (m_frameTimer) {
        m_frameTimer->stop();
    }
    if (m_performanceTimer) {
        m_performanceTimer->stop();
    }
}

size_t FluentPerformanceMonitor::currentMemoryUsage() const {
    // Simple memory usage estimation
    // In a real implementation, this would use platform-specific APIs
    return 0; // Placeholder
}

void FluentPerformanceMonitor::beginProfile(const QString& operation) {
    m_profileTimers[operation].start();
}

void FluentPerformanceMonitor::endProfile(const QString& operation) {
    auto it = m_profileTimers.find(operation);
    if (it != m_profileTimers.end()) {
        auto elapsed = std::chrono::milliseconds(it->second.elapsed());
        m_profileResults[operation] = elapsed;
    }
}

std::chrono::milliseconds FluentPerformanceMonitor::getProfileTime(const QString& operation) const {
    auto it = m_profileResults.find(operation);
    return (it != m_profileResults.end()) ? it->second : std::chrono::milliseconds(0);
}

bool FluentPerformanceMonitor::shouldSkipAnimation() const {
    return m_currentFPS < 30.0;
}

bool FluentPerformanceMonitor::shouldReduceEffects() const {
    return m_currentFPS < 45.0;
}

int FluentPerformanceMonitor::recommendedAnimationDuration() const {
    if (m_currentFPS >= 60.0) {
        return 250; // Full duration
    } else if (m_currentFPS >= 30.0) {
        return 150; // Reduced duration
    } else {
        return 50;  // Minimal duration
    }
}

void FluentPerformanceMonitor::updateFrameRate() {
    m_frameCount++;
    
    if (m_frameTimeTracker.elapsed() >= 1000) { // Update every second
        m_currentFPS = (m_frameCount * 1000.0) / m_frameTimeTracker.elapsed();
        emit frameRateChanged(m_currentFPS);
        
        m_frameCount = 0;
        m_frameTimeTracker.restart();
    }
}

void FluentPerformanceMonitor::checkPerformance() {
    if (m_currentFPS < 20.0) {
        emit performanceWarning("Very low frame rate detected. Consider reducing visual effects.");
    } else if (m_currentFPS < 30.0) {
        emit performanceWarning("Low frame rate detected. Some animations may be skipped.");
    }
    
    // Check memory usage (placeholder)
    size_t memUsage = currentMemoryUsage();
    if (memUsage > 500 * 1024 * 1024) { // 500MB threshold
        emit performanceWarning("High memory usage detected.");
    }
}

} // namespace FluentQt::Core
