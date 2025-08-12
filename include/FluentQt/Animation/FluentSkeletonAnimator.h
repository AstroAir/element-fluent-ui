// include/FluentQt/Animation/FluentSkeletonAnimator.h
#ifndef FLUENTSKELETONANIMATOR_H
#define FLUENTSKELETONANIMATOR_H

#include <QGraphicsOpacityEffect>
#include <QLinearGradient>
#include <QObject>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QWidget>
#include <memory>

#include "FluentAnimator.h"

namespace FluentQt::Animation {

// Skeleton animation types
enum class FluentSkeletonType {
    Text,    // Text line skeleton
    Avatar,  // Circular avatar skeleton
    Image,   // Rectangular image skeleton
    Card,    // Card layout skeleton
    List,    // List item skeleton
    Table,   // Table row skeleton
    Chart,   // Chart/graph skeleton
    Custom   // Custom skeleton shape
};

// Loading animation patterns
enum class FluentLoadingPattern {
    Wave,     // Wave shimmer effect
    Pulse,    // Pulsing opacity
    Slide,    // Sliding highlight
    Breathe,  // Breathing scale effect
    Rotate,   // Rotating spinner
    Dots,     // Animated dots
    Progress  // Progress bar
};

// Skeleton configuration
struct FluentSkeletonConfig {
    FluentSkeletonType type{FluentSkeletonType::Text};
    FluentLoadingPattern pattern{FluentLoadingPattern::Wave};
    QColor baseColor{220, 220, 220};
    QColor highlightColor{245, 245, 245};
    int animationDuration{1500};
    int animationDelay{0};
    bool enableShimmer{true};
    bool enableFadeIn{true};
    qreal cornerRadius{4.0};
    QSize customSize{100, 20};

    // Predefined configurations
    static FluentSkeletonConfig textLine() {
        FluentSkeletonConfig config;
        config.type = FluentSkeletonType::Text;
        config.customSize = QSize(200, 16);
        return config;
    }

    static FluentSkeletonConfig avatar() {
        FluentSkeletonConfig config;
        config.type = FluentSkeletonType::Avatar;
        config.customSize = QSize(40, 40);
        config.cornerRadius = 20.0;
        return config;
    }

    static FluentSkeletonConfig card() {
        FluentSkeletonConfig config;
        config.type = FluentSkeletonType::Card;
        config.customSize = QSize(300, 200);
        config.cornerRadius = 8.0;
        return config;
    }
};

// Skeleton widget for loading states
class FluentSkeletonWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(
        qreal shimmerPosition READ shimmerPosition WRITE setShimmerPosition)
    Q_PROPERTY(qreal pulseOpacity READ pulseOpacity WRITE setPulseOpacity)

public:
    explicit FluentSkeletonWidget(const FluentSkeletonConfig& config = {},
                                  QWidget* parent = nullptr);
    ~FluentSkeletonWidget() = default;

    void setConfig(const FluentSkeletonConfig& config);
    FluentSkeletonConfig config() const { return m_config; }

    void startAnimation();
    void stopAnimation();
    bool isAnimating() const { return m_animating; }

    // Property accessors for animations
    qreal shimmerPosition() const { return m_shimmerPosition; }
    void setShimmerPosition(qreal position);

    qreal pulseOpacity() const { return m_pulseOpacity; }
    void setPulseOpacity(qreal opacity);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void setupAnimation();
    void paintSkeleton(QPainter& painter);
    void paintShimmer(QPainter& painter);

    FluentSkeletonConfig m_config;
    std::unique_ptr<QPropertyAnimation> m_shimmerAnimation;
    std::unique_ptr<QPropertyAnimation> m_pulseAnimation;

    qreal m_shimmerPosition{0.0};
    qreal m_pulseOpacity{1.0};
    bool m_animating{false};
};

// Advanced loading indicator with multiple patterns
class FluentLoadingIndicator : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal rotationAngle READ rotationAngle WRITE setRotationAngle)
    Q_PROPERTY(qreal progress READ progress WRITE setProgress)

public:
    explicit FluentLoadingIndicator(
        FluentLoadingPattern pattern = FluentLoadingPattern::Rotate,
        QWidget* parent = nullptr);
    ~FluentLoadingIndicator() = default;

    void setPattern(FluentLoadingPattern pattern);
    FluentLoadingPattern pattern() const { return m_pattern; }

    void setSize(const QSize& size);
    void setColors(const QColor& primary, const QColor& secondary = QColor());

    void startAnimation();
    void stopAnimation();
    bool isAnimating() const { return m_animating; }

    // Property accessors
    qreal rotationAngle() const { return m_rotationAngle; }
    void setRotationAngle(qreal angle);

    qreal progress() const { return m_progress; }
    void setProgress(qreal progress);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void setupAnimation();
    void paintSpinner(QPainter& painter);
    void paintDots(QPainter& painter);
    void paintProgress(QPainter& painter);
    void paintWave(QPainter& painter);

    FluentLoadingPattern m_pattern;
    std::unique_ptr<QPropertyAnimation> m_rotationAnimation;
    std::unique_ptr<QPropertyAnimation> m_progressAnimation;
    std::unique_ptr<QSequentialAnimationGroup> m_dotsAnimation;

    qreal m_rotationAngle{0.0};
    qreal m_progress{0.0};
    bool m_animating{false};

    QColor m_primaryColor{0, 120, 215};
    QColor m_secondaryColor{200, 200, 200};
    QSize m_indicatorSize{32, 32};
};

// Skeleton animator for managing multiple skeleton widgets
class FluentSkeletonAnimator : public QObject {
    Q_OBJECT

public:
    explicit FluentSkeletonAnimator(QObject* parent = nullptr);
    ~FluentSkeletonAnimator() = default;

    // Skeleton management
    FluentSkeletonWidget* createSkeleton(
        QWidget* parent, const FluentSkeletonConfig& config = {});
    void addSkeleton(FluentSkeletonWidget* skeleton);
    void removeSkeleton(FluentSkeletonWidget* skeleton);
    void clearSkeletons();

    // Batch operations
    void startAllAnimations();
    void stopAllAnimations();
    void setStaggerDelay(int delayMs);

    // Layout helpers
    QWidget* createSkeletonLayout(QWidget* parent,
                                  const QList<FluentSkeletonConfig>& configs);
    QWidget* createTextBlockSkeleton(QWidget* parent, int lineCount = 3,
                                     int lineWidth = 200);
    QWidget* createCardSkeleton(QWidget* parent, bool withAvatar = true,
                                bool withImage = true);
    QWidget* createListSkeleton(QWidget* parent, int itemCount = 5);
    QWidget* createTableSkeleton(QWidget* parent, int rowCount = 5,
                                 int columnCount = 3);

    // Transition animations
    void fadeInContent(QWidget* content, QWidget* skeleton, int duration = 300);
    void slideInContent(QWidget* content, QWidget* skeleton,
                        int duration = 300);
    void morphToContent(QWidget* content, QWidget* skeleton,
                        int duration = 400);

signals:
    void skeletonAnimationStarted(FluentSkeletonWidget* skeleton);
    void skeletonAnimationFinished(FluentSkeletonWidget* skeleton);
    void allAnimationsFinished();

private slots:
    void onSkeletonAnimationFinished();

private:
    QList<FluentSkeletonWidget*> m_skeletons;
    int m_staggerDelay{100};
    int m_finishedCount{0};
};

// Data visualization animator for charts and graphs
class FluentDataVisualizationAnimator : public QObject {
    Q_OBJECT

public:
    explicit FluentDataVisualizationAnimator(QObject* parent = nullptr);
    ~FluentDataVisualizationAnimator() = default;

    // Chart animations
    void animateBarChart(QWidget* chart, const QList<qreal>& values,
                         int duration = 1000);
    void animateLineChart(QWidget* chart, const QList<QPointF>& points,
                          int duration = 1500);
    void animatePieChart(QWidget* chart, const QList<qreal>& values,
                         int duration = 1200);
    void animateAreaChart(QWidget* chart, const QList<QPointF>& points,
                          int duration = 1500);

    // Data transition animations
    void animateDataUpdate(QWidget* chart, const QVariantList& oldData,
                           const QVariantList& newData, int duration = 800);
    void animateDataInsertion(QWidget* chart, int index, const QVariant& value,
                              int duration = 600);
    void animateDataRemoval(QWidget* chart, int index, int duration = 400);

    // Interactive animations
    void animateHover(QWidget* element, const QPointF& position,
                      int duration = 200);
    void animateSelection(QWidget* element, bool selected, int duration = 300);
    void animateZoom(QWidget* chart, const QRectF& zoomArea,
                     int duration = 500);

    // Loading animations for data
    void showDataLoadingSkeleton(QWidget* chart,
                                 const FluentSkeletonConfig& config = {});
    void hideDataLoadingSkeleton(QWidget* chart, int duration = 300);

signals:
    void chartAnimationStarted(QWidget* chart);
    void chartAnimationFinished(QWidget* chart);
    void dataAnimationProgress(qreal progress);

private:
    struct ChartAnimation {
        QWidget* chart;
        std::unique_ptr<QPropertyAnimation> animation;
        QVariantList data;
        bool active;
    };

    QList<ChartAnimation> m_activeAnimations;
    QMap<QWidget*, FluentSkeletonWidget*> m_chartSkeletons;

    void setupChartAnimation(QWidget* chart, const QVariantList& data,
                             int duration);
    void updateChartData(QWidget* chart, qreal progress);
};

}  // namespace FluentQt::Animation

#endif  // FLUENTSKELETONANIMATOR_H
