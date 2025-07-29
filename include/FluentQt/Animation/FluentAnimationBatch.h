// include/FluentQt/Animation/FluentAnimationBatch.h
#ifndef FLUENTANIMATIONBATCH_H
#define FLUENTANIMATIONBATCH_H

#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QElapsedTimer>
#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QHash>
#include <QVariant>
#include <memory>
#include <atomic>

#include "FluentAnimator.h"

namespace FluentQt::Animation {

// Performance levels for adaptive quality
enum class FluentPerformanceLevel {
    Ultra,      // Maximum quality, all effects enabled
    High,       // High quality with some optimizations
    Medium,     // Balanced quality and performance
    Low,        // Performance optimized, reduced effects
    Minimal     // Minimal animations for low-end devices
};

// Animation batch configuration
struct FluentBatchConfig {
    int maxConcurrentAnimations{16};
    int maxBatchSize{32};
    int updateInterval{16}; // 60 FPS
    bool enableBatching{true};
    bool enablePrioritization{true};
    bool enableAdaptiveQuality{true};
    FluentPerformanceLevel performanceLevel{FluentPerformanceLevel::High};
    
    // Mobile optimizations
    bool enableMobileOptimizations{false};
    bool reducedMotionMode{false};
    bool powerSavingMode{false};
    
    // Memory management
    bool enableObjectPooling{true};
    int maxPoolSize{64};
    bool enableZeroAllocation{false};
};

// Animation priority levels
enum class FluentAnimationPriority {
    Critical,   // UI feedback, user interactions
    High,       // Important state changes
    Normal,     // Standard animations
    Low,        // Background animations
    Deferred    // Can be skipped if performance is poor
};

// Batched animation item
struct FluentBatchItem {
    QObject* target{nullptr};
    QByteArray property;
    QVariant startValue;
    QVariant endValue;
    FluentAnimationConfig config;
    FluentAnimationPriority priority{FluentAnimationPriority::Normal};
    std::function<void()> onFinished;
    bool active{false};
    int batchId{-1};
    qint64 startTime{0};
    
    // Zero-allocation optimization
    bool usePreallocated{false};
    void* preallocatedAnimation{nullptr};
};

// Animation batch manager
class FluentAnimationBatch : public QObject {
    Q_OBJECT
    
public:
    explicit FluentAnimationBatch(QObject* parent = nullptr);
    ~FluentAnimationBatch() override;
    
    // Configuration
    void setConfig(const FluentBatchConfig& config);
    FluentBatchConfig config() const { return m_config; }
    
    // Batch management
    int addAnimation(QObject* target, const QByteArray& property,
                    const QVariant& from, const QVariant& to,
                    const FluentAnimationConfig& config = {},
                    FluentAnimationPriority priority = FluentAnimationPriority::Normal);
    
    void removeAnimation(int batchId);
    void clearBatch();
    
    // Batch operations
    void startBatch();
    void pauseBatch();
    void stopBatch();
    bool isBatchActive() const { return m_batchActive; }
    
    // Performance monitoring
    void setPerformanceLevel(FluentPerformanceLevel level);
    FluentPerformanceLevel performanceLevel() const { return m_config.performanceLevel; }
    
    double getCurrentFPS() const { return m_currentFPS; }
    int getActiveAnimationCount() const { return m_activeAnimations.size(); }
    int getQueuedAnimationCount() const { return m_animationQueue.size(); }
    
    // Mobile optimizations
    void enableMobileMode(bool enable);
    void enablePowerSavingMode(bool enable);
    void setReducedMotionMode(bool enable);
    
    // Zero-allocation mode
    void enableZeroAllocationMode(bool enable);
    void preallocateAnimations(int count);

signals:
    void batchStarted();
    void batchFinished();
    void performanceLevelChanged(FluentPerformanceLevel level);
    void frameRateChanged(double fps);
    void animationFinished(int batchId);

private slots:
    void updateBatch();
    void onAnimationFinished();
    void checkPerformance();

private:
    void processBatch();
    void optimizeForPerformance();
    void adjustQualityLevel();
    void prioritizeAnimations();
    void cleanupFinishedAnimations();
    
    // Object pooling
    QPropertyAnimation* getPooledAnimation();
    void returnToPool(QPropertyAnimation* animation);
    void initializePool();
    void clearPool();
    
    FluentBatchConfig m_config;
    QTimer* m_updateTimer;
    QTimer* m_performanceTimer;
    QElapsedTimer m_frameTimer;
    
    // Animation management
    QQueue<FluentBatchItem> m_animationQueue;
    QHash<int, FluentBatchItem> m_activeAnimations;
    QHash<QPropertyAnimation*, int> m_animationToBatchId;
    
    // Performance monitoring
    std::atomic<bool> m_batchActive{false};
    std::atomic<double> m_currentFPS{60.0};
    std::atomic<int> m_frameCount{0};
    int m_nextBatchId{1};
    
    // Object pooling
    QQueue<QPropertyAnimation*> m_animationPool;
    QMutex m_poolMutex;
    
    // Zero-allocation optimization
    QVector<QPropertyAnimation> m_preallocatedAnimations;
    QVector<bool> m_preallocatedInUse;
    int m_nextPreallocatedIndex{0};
};

// Adaptive quality manager
class FluentAdaptiveQuality : public QObject {
    Q_OBJECT
    
public:
    explicit FluentAdaptiveQuality(QObject* parent = nullptr);
    ~FluentAdaptiveQuality() = default;
    
    // Quality management
    void setTargetFPS(double fps) { m_targetFPS = fps; }
    double targetFPS() const { return m_targetFPS; }
    
    void setQualityThresholds(double lowFPS, double highFPS);
    void enableAdaptiveMode(bool enable) { m_adaptiveMode = enable; }
    
    // Performance monitoring
    void reportFrameTime(double frameTime);
    void reportMemoryUsage(int megabytes);
    void reportAnimationCount(int count);
    
    // Quality recommendations
    FluentPerformanceLevel recommendedLevel() const;
    FluentAnimationConfig optimizeConfig(const FluentAnimationConfig& config) const;
    bool shouldSkipAnimation(FluentAnimationPriority priority) const;

signals:
    void qualityLevelChanged(FluentPerformanceLevel level);
    void performanceAlert(const QString& message);

private slots:
    void analyzePerformance();

private:
    void updateQualityLevel();
    
    QTimer* m_analysisTimer;
    double m_targetFPS{60.0};
    double m_lowFPSThreshold{30.0};
    double m_highFPSThreshold{55.0};
    bool m_adaptiveMode{true};
    
    // Performance history
    QQueue<double> m_frameTimeHistory;
    QQueue<int> m_memoryHistory;
    QQueue<int> m_animationCountHistory;
    
    FluentPerformanceLevel m_currentLevel{FluentPerformanceLevel::High};
    int m_performanceScore{100};
};

// Mobile-optimized animator
class FluentMobileAnimator : public QObject {
    Q_OBJECT
    
public:
    explicit FluentMobileAnimator(QObject* parent = nullptr);
    ~FluentMobileAnimator() = default;
    
    // Mobile-specific animations
    std::unique_ptr<QPropertyAnimation> createMobileOptimizedAnimation(
        QObject* target, const QByteArray& property,
        const QVariant& from, const QVariant& to,
        const FluentAnimationConfig& config = {});
    
    // Touch-optimized effects
    std::unique_ptr<QPropertyAnimation> touchFeedback(QWidget* target, const QPointF& touchPoint);
    std::unique_ptr<QPropertyAnimation> swipeTransition(QWidget* target, Qt::Orientation direction);
    std::unique_ptr<QPropertyAnimation> pullToRefresh(QWidget* target, double pullDistance);
    
    // Battery optimization
    void enableBatteryOptimization(bool enable) { m_batteryOptimization = enable; }
    void setPowerLevel(int percentage);
    
    // Device capability detection
    static bool isLowEndDevice();
    static bool hasHardwareAcceleration();
    static int getDeviceMemoryMB();
    static double getScreenRefreshRate();

private:
    FluentAnimationConfig optimizeForMobile(const FluentAnimationConfig& config) const;
    bool shouldReduceQuality() const;
    
    bool m_batteryOptimization{false};
    int m_batteryLevel{100};
    bool m_isLowEndDevice{false};
};

// Zero-allocation animation system
class FluentZeroAllocAnimator : public QObject {
    Q_OBJECT
    
public:
    explicit FluentZeroAllocAnimator(QObject* parent = nullptr);
    ~FluentZeroAllocAnimator() = default;
    
    // Pre-allocation
    void preallocate(int animationCount, int groupCount = 0);
    void clear();
    
    // Zero-allocation animation creation
    QPropertyAnimation* getAnimation(QObject* target, const QByteArray& property);
    QParallelAnimationGroup* getParallelGroup();
    QSequentialAnimationGroup* getSequentialGroup();
    
    // Return to pool
    void returnAnimation(QPropertyAnimation* animation);
    void returnGroup(QAbstractAnimationGroup* group);
    
    // Statistics
    int getAllocatedCount() const { return m_allocatedAnimations.size(); }
    int getAvailableCount() const { return m_availableAnimations.size(); }
    double getPoolUtilization() const;

private:
    void initializePools();
    
    // Object pools
    QVector<std::unique_ptr<QPropertyAnimation>> m_allocatedAnimations;
    QVector<std::unique_ptr<QParallelAnimationGroup>> m_allocatedParallelGroups;
    QVector<std::unique_ptr<QSequentialAnimationGroup>> m_allocatedSequentialGroups;
    
    QQueue<QPropertyAnimation*> m_availableAnimations;
    QQueue<QParallelAnimationGroup*> m_availableParallelGroups;
    QQueue<QSequentialAnimationGroup*> m_availableSequentialGroups;
    
    QMutex m_poolMutex;
    int m_maxAnimations{64};
    int m_maxGroups{16};
};

} // namespace FluentQt::Animation

#endif // FLUENTANIMATIONBATCH_H
