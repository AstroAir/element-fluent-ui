// include/FluentQt/Animation/FluentAnimationPerformanceManager.h
#pragma once

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QMutex>
#include <QMap>
#include <QQueue>
#include <QLocale>
#include <QScreen>
#include <QApplication>
#include <chrono>
#include <memory>
#include <atomic>

#include "FluentAdvancedAnimator.h"

namespace FluentQt::Animation {

// Performance metrics
struct FluentAnimationMetrics {
    double averageFPS{60.0};
    double frameTime{16.67}; // milliseconds
    int droppedFrames{0};
    int totalFrames{0};
    double cpuUsage{0.0};
    double memoryUsage{0.0};
    int activeAnimations{0};
    int queuedAnimations{0};
    std::chrono::steady_clock::time_point lastUpdate;

    // Performance scores (0-100)
    double performanceScore{100.0};
    double smoothnessScore{100.0};
    double responsiveScore{100.0};
};

// Performance optimization strategies
enum class FluentOptimizationStrategy {
    None,                   // No optimization
    ReduceQuality,          // Reduce animation quality
    SkipFrames,             // Skip intermediate frames
    SimplifyEffects,        // Use simpler effects
    BatchAnimations,        // Batch similar animations
    UseHardwareAccel,       // Enable hardware acceleration
    ReduceDuration,         // Shorten animation durations
    PrioritizeVisible,      // Prioritize visible animations
    AdaptiveFrameRate,      // Adapt frame rate to performance
    Custom                  // Custom optimization strategy
};

// Performance configuration
struct FluentPerformanceConfig {
    FluentAnimationPerformance targetPerformance{FluentAnimationPerformance::Adaptive};
    double targetFPS{60.0};
    double minFPS{30.0};
    double maxCPUUsage{70.0};
    double maxMemoryUsage{100.0}; // MB
    int maxConcurrentAnimations{20};
    bool enableHardwareAcceleration{true};
    bool enablePerformanceMonitoring{true};
    bool enableAdaptiveOptimization{true};
    bool respectPowerSaving{true};
    bool respectAccessibilityPreferences{true};
    QList<FluentOptimizationStrategy> optimizationStrategies;
    QMap<QString, QVariant> customSettings;
};

// Animation priority levels
enum class FluentAnimationPriority {
    Critical,               // Must not be skipped (accessibility feedback)
    High,                   // Important user feedback
    Normal,                 // Standard animations
    Low,                    // Nice-to-have animations
    Background              // Background/ambient animations
};

// Performance-aware animation request
struct FluentAnimationRequest {
    QWidget* target{nullptr};
    QString animationType;
    FluentAnimationPriority priority{FluentAnimationPriority::Normal};
    std::chrono::milliseconds duration{300};
    QVariant startValue;
    QVariant endValue;
    QString property;
    FluentAnimationPerformance requiredPerformance{FluentAnimationPerformance::Medium};
    bool canBeSkipped{true};
    bool canBeSimplified{true};
    QMap<QString, QVariant> parameters;
    std::function<void()> onComplete;
    std::function<void()> onSkipped;
};

// Advanced animation performance manager
class FluentAnimationPerformanceManager : public QObject {
    Q_OBJECT

public:
    static FluentAnimationPerformanceManager& instance();

    // Configuration
    void setPerformanceConfig(const FluentPerformanceConfig& config);
    FluentPerformanceConfig getPerformanceConfig() const { return m_config; }
    void setTargetPerformance(FluentAnimationPerformance performance);
    FluentAnimationPerformance getTargetPerformance() const { return m_config.targetPerformance; }

    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    bool isPerformanceMonitoringEnabled() const { return m_performanceMonitoringEnabled; }
    FluentAnimationMetrics getCurrentMetrics() const;
    FluentAnimationMetrics getAverageMetrics(std::chrono::seconds duration = std::chrono::seconds(10)) const;

    // Animation management
    int requestAnimation(const FluentAnimationRequest& request);
    void cancelAnimation(int requestId);
    void pauseAnimation(int requestId);
    void resumeAnimation(int requestId);
    void setAnimationPriority(int requestId, FluentAnimationPriority priority);

    // Performance optimization
    void optimizePerformance();
    void enableOptimizationStrategy(FluentOptimizationStrategy strategy, bool enabled = true);
    bool isOptimizationStrategyEnabled(FluentOptimizationStrategy strategy) const;
    void setCustomOptimizationFunction(std::function<void(const FluentAnimationMetrics&)> optimizer);

    // Hardware acceleration
    void enableHardwareAcceleration(QWidget* widget, bool enabled = true);
    bool isHardwareAccelerationSupported() const;
    bool isHardwareAccelerationEnabled(QWidget* widget) const;
    void optimizeForHardwareAcceleration();

    // Adaptive performance
    void enableAdaptivePerformance(bool enabled = true);
    bool isAdaptivePerformanceEnabled() const { return m_adaptivePerformanceEnabled; }
    void updatePerformanceBasedOnMetrics();
    void adaptToSystemPerformance();

    // Cultural and accessibility integration
    void setCulturalAnimationStyle(FluentCulturalAnimationStyle style);
    FluentCulturalAnimationStyle getCulturalAnimationStyle() const { return m_culturalStyle; }
    void setAccessibilityAnimationMode(FluentAccessibilityAnimationMode mode);
    FluentAccessibilityAnimationMode getAccessibilityAnimationMode() const { return m_accessibilityMode; }
    void adaptAnimationsForCulture(const QLocale& locale);
    void adaptAnimationsForAccessibility();

    // Performance analysis
    double calculatePerformanceScore() const;
    double calculateSmoothnesScore() const;
    double calculateResponsivenessScore() const;
    QStringList getPerformanceRecommendations() const;
    QMap<QString, double> getDetailedMetrics() const;

    // Animation queue management
    void setMaxConcurrentAnimations(int maxAnimations);
    int getMaxConcurrentAnimations() const { return m_config.maxConcurrentAnimations; }
    int getActiveAnimationCount() const;
    int getQueuedAnimationCount() const;
    void clearAnimationQueue();

    // Power management
    void enablePowerSavingMode(bool enabled = true);
    bool isPowerSavingModeEnabled() const { return m_powerSavingEnabled; }
    void adaptToBatteryLevel(int batteryLevel);
    void adaptToThermalState(int thermalState);

signals:
    void performanceChanged(const FluentAnimationMetrics& metrics);
    void performanceOptimized(FluentOptimizationStrategy strategy);
    void animationSkipped(int requestId, const QString& reason);
    void animationSimplified(int requestId, const QString& changes);
    void hardwareAccelerationChanged(bool enabled);
    void adaptivePerformanceTriggered(FluentAnimationPerformance newLevel);
    void culturalStyleChanged(FluentCulturalAnimationStyle style);
    void accessibilityModeChanged(FluentAccessibilityAnimationMode mode);

private slots:
    void onPerformanceTimer();
    void onOptimizationTimer();
    void onSystemPerformanceChanged();
    void onAccessibilitySettingsChanged();

private:
    FluentAnimationPerformanceManager();

    // Performance monitoring
    void updateMetrics();
    void calculateFrameRate();
    void monitorSystemResources();
    void detectPerformanceIssues();

    // Optimization algorithms
    void applyReduceQualityStrategy();
    void applySkipFramesStrategy();
    void applySimplifyEffectsStrategy();
    void applyBatchAnimationsStrategy();
    void applyReduceDurationStrategy();
    void applyPrioritizeVisibleStrategy();
    void applyAdaptiveFrameRateStrategy();

    // Animation processing
    void processAnimationQueue();
    bool shouldSkipAnimation(const FluentAnimationRequest& request) const;
    FluentAnimationRequest simplifyAnimation(const FluentAnimationRequest& request) const;
    void executeAnimation(const FluentAnimationRequest& request);

    // Advanced batching system
    struct BatchedAnimationGroup {
        QList<int> requestIds;
        QString groupType;
        QWidget* commonTarget{nullptr};
        std::chrono::milliseconds totalDuration{0};
        std::unique_ptr<QParallelAnimationGroup> animationGroup;
    };

    std::unique_ptr<BatchedAnimationGroup> createBatchedAnimationGroup(const QList<int>& requestIds);
    void executeBatchedAnimation(std::unique_ptr<BatchedAnimationGroup> batchedGroup);
    void optimizeBatchedAnimations();

    // Cultural and accessibility adaptation
    void applyCulturalAnimationStyle();
    void applyAccessibilityAnimationMode();
    std::chrono::milliseconds getCulturalDuration(std::chrono::milliseconds baseDuration) const;
    QEasingCurve getCulturalEasing(const QEasingCurve& baseEasing) const;

    // Hardware acceleration helpers
    void detectHardwareCapabilities();
    void configureHardwareAcceleration(QWidget* widget);
    bool isWidgetSuitableForHardwareAccel(QWidget* widget) const;

    // System integration
    void connectToSystemSignals();
    void updateFromSystemSettings();
    bool isReducedMotionPreferred() const;
    bool isPowerSavingActive() const;

private:
    FluentPerformanceConfig m_config;

    // Performance monitoring
    bool m_performanceMonitoringEnabled{false};
    QTimer* m_performanceTimer;
    QTimer* m_optimizationTimer;
    QElapsedTimer m_frameTimer;
    FluentAnimationMetrics m_currentMetrics;
    QQueue<FluentAnimationMetrics> m_metricsHistory;

    // Animation management
    QMap<int, FluentAnimationRequest> m_animationRequests;
    QQueue<int> m_animationQueue;
    QSet<int> m_activeAnimations;
    QSet<int> m_pausedAnimations;
    int m_nextRequestId{1};

    // Performance optimization
    bool m_adaptivePerformanceEnabled{true};
    bool m_powerSavingEnabled{false};
    QSet<FluentOptimizationStrategy> m_enabledStrategies;
    std::function<void(const FluentAnimationMetrics&)> m_customOptimizer;

    // Cultural and accessibility
    FluentCulturalAnimationStyle m_culturalStyle{FluentCulturalAnimationStyle::Adaptive};
    FluentAccessibilityAnimationMode m_accessibilityMode{FluentAccessibilityAnimationMode::Full};

    // Hardware acceleration
    QSet<QWidget*> m_hardwareAcceleratedWidgets;
    bool m_hardwareAccelerationSupported{false};

    // System integration
    bool m_systemReducedMotion{false};
    bool m_systemPowerSaving{false};
    int m_systemBatteryLevel{100};
    int m_systemThermalState{0};

    // Thread safety
    mutable QMutex m_metricsMutex;
    mutable QMutex m_queueMutex;
    mutable QMutex m_configMutex;

    // Performance tracking
    std::atomic<double> m_currentFPS{60.0};
    std::atomic<int> m_frameCount{0};
    std::atomic<int> m_droppedFrames{0};
};

// Performance utility functions
namespace FluentAnimationPerformanceUtils {
    // Performance analysis
    bool isPerformanceAcceptable(const FluentAnimationMetrics& metrics);
    FluentAnimationPerformance recommendPerformanceLevel(const FluentAnimationMetrics& metrics);
    QStringList analyzePerformanceBottlenecks(const FluentAnimationMetrics& metrics);

    // Optimization helpers
    std::chrono::milliseconds optimizeDuration(std::chrono::milliseconds duration, FluentAnimationPerformance performance);
    QEasingCurve optimizeEasing(const QEasingCurve& easing, FluentAnimationPerformance performance);
    bool shouldUseHardwareAcceleration(QWidget* widget, FluentAnimationPerformance performance);

    // Cultural adaptation helpers
    std::chrono::milliseconds getCulturalDuration(std::chrono::milliseconds baseDuration, FluentCulturalAnimationStyle style);
    QEasingCurve getCulturalEasing(const QEasingCurve& baseEasing, FluentCulturalAnimationStyle style);
    double getCulturalIntensity(double baseIntensity, FluentCulturalAnimationStyle style);

    // Accessibility helpers
    bool shouldSkipForAccessibility(FluentAccessibilityAnimationMode mode, FluentAnimationPriority priority);
    std::chrono::milliseconds getAccessibleDuration(std::chrono::milliseconds baseDuration, FluentAccessibilityAnimationMode mode);
    QEasingCurve getAccessibleEasing(const QEasingCurve& baseEasing, FluentAccessibilityAnimationMode mode);

    // System integration helpers
    bool isReducedMotionPreferred();
    bool isPowerSavingActive();
    int getBatteryLevel();
    int getThermalState();
    double getSystemPerformanceScore();
}

} // namespace FluentQt::Animation
