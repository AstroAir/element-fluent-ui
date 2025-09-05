// include/FluentQt/Components/FluentCircularProgress.h
#pragma once

#include <QWidget>
#include <QColor>
#include <QPropertyAnimation>
#include <QTimer>
#include <QPainter>
#include <QResizeEvent>
#include <QPaintEvent>

namespace FluentQt::Components {

/**
 * @brief Circular progress indicator following Fluent Design principles
 * 
 * FluentCircularProgress provides a modern circular progress indicator with:
 * - Smooth animations using Fluent Design motion tokens
 * - Proper color theming with semantic color support
 * - Accessibility features and ARIA attributes
 * - Multiple visual styles (Ring, Filled, Segmented)
 * - High DPI support and crisp rendering
 */
class FluentCircularProgress : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(QColor activeColor READ activeColor WRITE setActiveColor NOTIFY activeColorChanged)
    Q_PROPERTY(QColor inactiveColor READ inactiveColor WRITE setInactiveColor NOTIFY inactiveColorChanged)
    Q_PROPERTY(int strokeWidth READ strokeWidth WRITE setStrokeWidth NOTIFY strokeWidthChanged)
    Q_PROPERTY(bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(int animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)
    Q_PROPERTY(ProgressStyle style READ style WRITE setStyle NOTIFY styleChanged)

public:
    /**
     * @brief Visual styles for circular progress
     */
    enum class ProgressStyle {
        Ring,       // Thin ring progress (default Fluent style)
        Filled,     // Filled circle progress
        Segmented,  // Segmented ring for better accessibility
        Gradient    // Gradient ring with enhanced visual appeal
    };
    Q_ENUM(ProgressStyle)

    /**
     * @brief Constructs a FluentCircularProgress widget
     * @param parent Parent widget (optional)
     */
    explicit FluentCircularProgress(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~FluentCircularProgress() override = default;

    // Progress value (0.0 to 1.0)
    qreal progress() const noexcept { return m_progress; }
    void setProgress(qreal progress, bool animated = true);

    // Colors
    QColor activeColor() const noexcept { return m_activeColor; }
    void setActiveColor(const QColor& color);

    QColor inactiveColor() const noexcept { return m_inactiveColor; }
    void setInactiveColor(const QColor& color);

    // Appearance
    int strokeWidth() const noexcept { return m_strokeWidth; }
    void setStrokeWidth(int width);

    ProgressStyle style() const noexcept { return m_style; }
    void setStyle(ProgressStyle style);

    // Animation
    bool isAnimated() const noexcept { return m_animated; }
    void setAnimated(bool animated);

    int animationDuration() const noexcept { return m_animationDuration; }
    void setAnimationDuration(int duration);

    // Size management
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Factory methods for common configurations
    static FluentCircularProgress* createRing(int size = 24, QWidget* parent = nullptr);
    static FluentCircularProgress* createFilled(int size = 32, QWidget* parent = nullptr);
    static FluentCircularProgress* createSegmented(int size = 28, QWidget* parent = nullptr);
    static FluentCircularProgress* createAccessible(int size = 32, QWidget* parent = nullptr);

public slots:
    /**
     * @brief Starts an indeterminate progress animation
     */
    void startIndeterminate();

    /**
     * @brief Stops indeterminate animation and returns to determinate mode
     */
    void stopIndeterminate();

    /**
     * @brief Resets progress to 0
     */
    void reset();

signals:
    void progressChanged(qreal progress);
    void activeColorChanged(const QColor& color);
    void inactiveColorChanged(const QColor& color);
    void strokeWidthChanged(int width);
    void animatedChanged(bool animated);
    void animationDurationChanged(int duration);
    void styleChanged(ProgressStyle style);
    void indeterminateStarted();
    void indeterminateStopped();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onProgressAnimationValueChanged(const QVariant& value);
    void onIndeterminateTimer();

private:
    void initializeComponent();
    void updateColors();
    void updateAccessibilityInfo();
    void paintRingStyle(QPainter& painter, const QRectF& rect);
    void paintFilledStyle(QPainter& painter, const QRectF& rect);
    void paintSegmentedStyle(QPainter& painter, const QRectF& rect);
    void paintGradientStyle(QPainter& painter, const QRectF& rect);
    QRectF calculateProgressRect() const;
    QPen createProgressPen(const QColor& color) const;
    QConicalGradient createProgressGradient(const QRectF& rect) const;

private:
    // Progress state
    qreal m_progress{0.0};
    qreal m_displayProgress{0.0};  // For animation
    
    // Appearance
    QColor m_activeColor;
    QColor m_inactiveColor;
    int m_strokeWidth{3};
    ProgressStyle m_style{ProgressStyle::Ring};
    
    // Animation
    bool m_animated{true};
    int m_animationDuration{0};  // 0 = use Fluent tokens
    std::unique_ptr<QPropertyAnimation> m_progressAnimation;
    
    // Indeterminate mode
    bool m_indeterminate{false};
    qreal m_indeterminateAngle{0.0};
    std::unique_ptr<QTimer> m_indeterminateTimer;
    
    // Caching for performance
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
};

}  // namespace FluentQt::Components
