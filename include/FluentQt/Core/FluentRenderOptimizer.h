// include/FluentQt/Core/FluentRenderOptimizer.h
#pragma once

#include <QCache>
#include <QMutex>
#include <QObject>
#include <QPixmap>
#include <QRect>
#include <QRegion>
#include <QTimer>
#include <QWidget>
#include <chrono>
#include <memory>
#include <unordered_map>

namespace FluentQt::Core {

// Render cache entry
struct RenderCacheEntry {
    QPixmap pixmap;
    std::chrono::steady_clock::time_point timestamp;
    QSize size;
    qreal devicePixelRatio;
    QString styleHash;
    bool isDirty{false};
};

// Dirty region tracking
class FluentDirtyRegionTracker {
public:
    void markDirty(const QRect& rect);
    void markDirty(const QRegion& region);
    void markClean();

    QRegion getDirtyRegion() const { return m_dirtyRegion; }
    bool isDirty() const { return !m_dirtyRegion.isEmpty(); }
    bool isRectDirty(const QRect& rect) const;

    // Optimization: merge small adjacent dirty rects
    void optimizeDirtyRegion();

private:
    QRegion m_dirtyRegion;
    mutable QMutex m_mutex;
};

// Render optimization strategies
enum class FluentRenderStrategy {
    Immediate,  // Render immediately (default)
    Deferred,   // Defer rendering to next frame
    Cached,     // Use cached rendering when possible
    Adaptive    // Automatically choose based on performance
};

// Render quality levels
enum class FluentRenderQuality {
    High,     // Full quality rendering
    Medium,   // Reduced quality for better performance
    Low,      // Minimal quality for maximum performance
    Adaptive  // Automatically adjust based on performance
};

// Render optimizer for components
class FluentRenderOptimizer : public QObject {
    Q_OBJECT

public:
    static FluentRenderOptimizer& instance();

    // Cache management
    void setCacheEnabled(bool enabled) { m_cacheEnabled = enabled; }
    bool isCacheEnabled() const { return m_cacheEnabled; }
    void setCacheSize(int maxCost) { m_renderCache.setMaxCost(maxCost); }
    void clearCache();

    // Render caching
    bool getCachedRender(QWidget* widget, QPixmap& pixmap);
    void setCachedRender(QWidget* widget, const QPixmap& pixmap);
    void invalidateCache(QWidget* widget);

    // Dirty region tracking
    FluentDirtyRegionTracker* getDirtyTracker(QWidget* widget);
    void markWidgetDirty(QWidget* widget, const QRect& rect = QRect());
    void markWidgetClean(QWidget* widget);

    // Render strategy
    void setRenderStrategy(FluentRenderStrategy strategy) {
        m_renderStrategy = strategy;
    }
    FluentRenderStrategy getRenderStrategy() const { return m_renderStrategy; }

    // Render quality
    void setRenderQuality(FluentRenderQuality quality) {
        m_renderQuality = quality;
    }
    FluentRenderQuality getRenderQuality() const { return m_renderQuality; }

    // Performance-based optimization
    void enableAdaptiveOptimization(bool enable = true) {
        m_adaptiveOptimization = enable;
    }
    bool isAdaptiveOptimizationEnabled() const {
        return m_adaptiveOptimization;
    }

    // Render scheduling
    void scheduleRender(QWidget* widget, int priority = 0);
    void cancelScheduledRender(QWidget* widget);
    void processScheduledRenders();

    // Performance metrics
    struct RenderMetrics {
        int totalRenders{0};
        int cachedRenders{0};
        int skippedRenders{0};
        std::chrono::milliseconds totalRenderTime{0};
        std::chrono::milliseconds averageRenderTime{0};
        double cacheHitRate{0.0};
    };

    RenderMetrics getMetrics() const { return m_metrics; }
    void resetMetrics();

    // Optimization hints
    bool shouldSkipRender(QWidget* widget) const;
    bool shouldUseCache(QWidget* widget) const;
    bool shouldDeferRender(QWidget* widget) const;
    QRect getOptimalRenderRect(QWidget* widget) const;

    // Widget registration for optimization
    void registerWidget(QWidget* widget);
    void unregisterWidget(QWidget* widget);

signals:
    void renderScheduled(QWidget* widget);
    void renderCompleted(QWidget* widget, std::chrono::milliseconds duration);
    void cacheHit(QWidget* widget);
    void cacheMiss(QWidget* widget);
    void metricsUpdated(const RenderMetrics& metrics);

private slots:
    void onWidgetDestroyed(QObject* widget);
    void processRenderQueue();
    void updateAdaptiveSettings();

private:
    FluentRenderOptimizer();
    QString generateCacheKey(QWidget* widget) const;
    QString generateStyleHash(QWidget* widget) const;
    bool isRenderCacheValid(QWidget* widget,
                            const RenderCacheEntry& entry) const;
    void updateMetrics(bool cacheHit, std::chrono::milliseconds renderTime);

private:
    // Cache system
    QCache<QString, RenderCacheEntry> m_renderCache;
    bool m_cacheEnabled{true};

    // Dirty region tracking
    std::unordered_map<QWidget*, std::unique_ptr<FluentDirtyRegionTracker>>
        m_dirtyTrackers;

    // Render strategy and quality
    FluentRenderStrategy m_renderStrategy{FluentRenderStrategy::Adaptive};
    FluentRenderQuality m_renderQuality{FluentRenderQuality::Adaptive};
    bool m_adaptiveOptimization{true};

    // Render scheduling
    struct ScheduledRender {
        QWidget* widget;
        int priority;
        std::chrono::steady_clock::time_point scheduledTime;
    };

    QList<ScheduledRender> m_renderQueue;
    QTimer* m_renderTimer;

    // Performance tracking
    RenderMetrics m_metrics;
    mutable QMutex m_metricsMutex;

    // Adaptive optimization
    QTimer* m_adaptiveTimer;
    double m_currentFrameRate{60.0};

    // Thread safety
    mutable QMutex m_cacheMutex;
    mutable QMutex m_trackerMutex;
    mutable QMutex m_queueMutex;
};

// RAII render profiler
class FluentRenderProfiler {
public:
    explicit FluentRenderProfiler(QWidget* widget);
    ~FluentRenderProfiler();

private:
    QWidget* m_widget;
    std::chrono::steady_clock::time_point m_startTime;
};

// Optimized paint device for caching
class FluentCachedPaintDevice {
public:
    explicit FluentCachedPaintDevice(QWidget* widget);

    QPainter* begin();
    void end();

    QPixmap getPixmap() const { return m_pixmap; }
    bool isValid() const { return !m_pixmap.isNull(); }

private:
    QWidget* m_widget;
    QPixmap m_pixmap;
    std::unique_ptr<QPainter> m_painter;
};

// Render optimization utilities
class FluentRenderUtils {
public:
    // Efficient region operations
    static QRegion optimizeRegion(const QRegion& region);
    static QRect boundingRect(const QRegion& region);
    static bool shouldMergeRects(const QRect& rect1, const QRect& rect2);

    // Performance-aware rendering
    static void renderWithQuality(QPainter* painter,
                                  FluentRenderQuality quality,
                                  std::function<void()> renderFunc);

    // Cache key generation
    static QString generateWidgetHash(QWidget* widget);
    static QString generateStyleHash(QWidget* widget);

    // Render optimization checks
    static bool isWidgetVisible(QWidget* widget);
    static bool isWidgetObscured(QWidget* widget);
    static QRect getVisibleRect(QWidget* widget);

    // Memory-efficient pixmap operations
    static QPixmap createOptimizedPixmap(const QSize& size,
                                         qreal devicePixelRatio);
    static void optimizePixmap(QPixmap& pixmap);
};

// Convenience macros
#define FLUENT_PROFILE_RENDER(widget) \
    FluentQt::Core::FluentRenderProfiler _renderProfiler(widget)

#define FLUENT_CACHED_RENDER(widget, renderCode)                             \
    do {                                                                     \
        auto& optimizer = FluentQt::Core::FluentRenderOptimizer::instance(); \
        QPixmap cachedPixmap;                                                \
        if (!optimizer.getCachedRender(widget, cachedPixmap)) {              \
            FluentQt::Core::FluentCachedPaintDevice device(widget);          \
            if (auto* painter = device.begin()) {                            \
                renderCode;                                                  \
                device.end();                                                \
                optimizer.setCachedRender(widget, device.getPixmap());       \
            }                                                                \
        }                                                                    \
    } while (0)

}  // namespace FluentQt::Core
