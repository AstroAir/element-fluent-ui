// include/FluentQt/Components/FluentDotProgress.h
#pragma once

#include <QWidget>
#include <QColor>
#include <QPropertyAnimation>
#include <QTimer>
#include <QSequentialAnimationGroup>
#include <QPainter>
#include <QResizeEvent>
#include <QPaintEvent>

namespace FluentQt::Components {

/**
 * @brief Dot-based progress indicator following Fluent Design principles
 * 
 * FluentDotProgress provides a modern dot-based progress indicator with:
 * - Smooth dot animations using Fluent Design motion tokens
 * - Proper color theming with semantic color support
 * - Accessibility features and ARIA attributes
 * - Multiple visual styles (Linear, Circular, Wave)
 * - Configurable dot count and spacing
 * - High DPI support and crisp rendering
 */
class FluentDotProgress : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY(QColor activeColor READ activeColor WRITE setActiveColor NOTIFY activeColorChanged)
    Q_PROPERTY(QColor inactiveColor READ inactiveColor WRITE setInactiveColor NOTIFY inactiveColorChanged)
    Q_PROPERTY(int dotCount READ dotCount WRITE setDotCount NOTIFY dotCountChanged)
    Q_PROPERTY(int dotSize READ dotSize WRITE setDotSize NOTIFY dotSizeChanged)
    Q_PROPERTY(int dotSpacing READ dotSpacing WRITE setDotSpacing NOTIFY dotSpacingChanged)
    Q_PROPERTY(bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(int animationDuration READ animationDuration WRITE setAnimationDuration NOTIFY animationDurationChanged)
    Q_PROPERTY(ProgressStyle style READ style WRITE setStyle NOTIFY styleChanged)

public:
    /**
     * @brief Visual styles for dot progress
     */
    enum class ProgressStyle {
        Linear,     // Linear arrangement of dots (default)
        Circular,   // Circular arrangement of dots
        Wave,       // Wave animation effect
        Pulse       // Pulsing dots effect
    };
    Q_ENUM(ProgressStyle)

    /**
     * @brief Constructs a FluentDotProgress widget
     * @param parent Parent widget (optional)
     */
    explicit FluentDotProgress(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~FluentDotProgress() override = default;

    // Progress value (0.0 to 1.0)
    qreal progress() const noexcept { return m_progress; }
    void setProgress(qreal progress, bool animated = true);

    // Colors
    QColor activeColor() const noexcept { return m_activeColor; }
    void setActiveColor(const QColor& color);

    QColor inactiveColor() const noexcept { return m_inactiveColor; }
    void setInactiveColor(const QColor& color);

    // Appearance
    int dotCount() const noexcept { return m_dotCount; }
    void setDotCount(int count);

    int dotSize() const noexcept { return m_dotSize; }
    void setDotSize(int size);

    int dotSpacing() const noexcept { return m_dotSpacing; }
    void setDotSpacing(int spacing);

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
    static FluentDotProgress* createLinear(int dotCount = 5, QWidget* parent = nullptr);
    static FluentDotProgress* createCircular(int dotCount = 8, QWidget* parent = nullptr);
    static FluentDotProgress* createWave(int dotCount = 7, QWidget* parent = nullptr);
    static FluentDotProgress* createAccessible(int dotCount = 5, QWidget* parent = nullptr);

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
    void dotCountChanged(int count);
    void dotSizeChanged(int size);
    void dotSpacingChanged(int spacing);
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
    void calculateDotPositions();
    void paintLinearStyle(QPainter& painter);
    void paintCircularStyle(QPainter& painter);
    void paintWaveStyle(QPainter& painter);
    void paintPulseStyle(QPainter& painter);
    void paintDot(QPainter& painter, const QPointF& position, qreal opacity, qreal scale = 1.0);
    QColor interpolateColor(const QColor& from, const QColor& to, qreal factor) const;

private:
    // Progress state
    qreal m_progress{0.0};
    qreal m_displayProgress{0.0};  // For animation
    
    // Appearance
    QColor m_activeColor;
    QColor m_inactiveColor;
    int m_dotCount{5};
    int m_dotSize{6};
    int m_dotSpacing{8};
    ProgressStyle m_style{ProgressStyle::Linear};
    
    // Animation
    bool m_animated{true};
    int m_animationDuration{0};  // 0 = use Fluent tokens
    std::unique_ptr<QPropertyAnimation> m_progressAnimation;
    
    // Indeterminate mode
    bool m_indeterminate{false};
    qreal m_indeterminatePhase{0.0};
    std::unique_ptr<QTimer> m_indeterminateTimer;
    
    // Layout
    QList<QPointF> m_dotPositions;
    bool m_layoutDirty{true};
    
    // Caching for performance
    mutable QSize m_cachedSizeHint;
    mutable bool m_sizeHintValid{false};
};

}  // namespace FluentQt::Components
