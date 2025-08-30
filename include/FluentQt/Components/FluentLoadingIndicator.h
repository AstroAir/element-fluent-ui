// include/FluentQt/Components/FluentLoadingIndicator.h
#pragma once

#include <QPropertyAnimation>
#include <QTimer>
#include <memory>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Core/FluentErrorBoundary.h"

namespace FluentQt::Components {

enum class FluentLoadingType {
    Spinner,  // Rotating circle
    Dots,     // Bouncing dots
    Pulse,    // Pulsing circle
    Bars,     // Animated bars
    Ring,     // Ring with rotating segment
    Wave      // Wave animation
};

enum class FluentLoadingSize {
    Small,      // 16x16
    Medium,     // 24x24
    Large,      // 32x32
    ExtraLarge  // 48x48
};

enum class FluentLoadingComplexity {
    Simple,  // Lightweight mode with basic animations (QWidget-based behavior)
    Full     // Full-featured mode with advanced animations and error handling
};

class FluentLoadingIndicator : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(FluentLoadingType loadingType READ loadingType WRITE
                   setLoadingType NOTIFY loadingTypeChanged)
    Q_PROPERTY(FluentLoadingSize loadingSize READ loadingSize WRITE
                   setLoadingSize NOTIFY loadingSizeChanged)
    Q_PROPERTY(FluentLoadingComplexity complexity READ complexity WRITE
                   setComplexity NOTIFY complexityChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(
        bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(int speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool textVisible READ isTextVisible WRITE setTextVisible NOTIFY
                   textVisibleChanged)
    Q_PROPERTY(qreal animationProgress READ animationProgress WRITE
                   setAnimationProgress NOTIFY animationProgressChanged)

public:
    explicit FluentLoadingIndicator(QWidget* parent = nullptr);
    explicit FluentLoadingIndicator(FluentLoadingType type,
                                    QWidget* parent = nullptr);
    explicit FluentLoadingIndicator(FluentLoadingType type,
                                    FluentLoadingSize size,
                                    QWidget* parent = nullptr);
    ~FluentLoadingIndicator() override;

    // Type and size
    FluentLoadingType loadingType() const;
    void setLoadingType(FluentLoadingType type);

    FluentLoadingSize loadingSize() const;
    void setLoadingSize(FluentLoadingSize size);

    // Complexity mode
    FluentLoadingComplexity complexity() const;
    void setComplexity(FluentLoadingComplexity complexity);

    // Appearance
    QColor color() const;
    void setColor(const QColor& color);

    // Animation control
    bool isRunning() const;
    void setRunning(bool running);

    int speed() const;  // Animation speed (1-10, default 5)
    void setSpeed(int speed);

    // Text
    QString text() const;
    void setText(const QString& text);

    bool isTextVisible() const;
    void setTextVisible(bool visible);

    // Animation progress
    qreal animationProgress() const;
    void setAnimationProgress(qreal progress);

    // Error boundary integration (ElaWidgetTools-inspired)
    void setErrorBoundary(Core::FluentErrorBoundary* boundary);
    Core::FluentErrorBoundary* errorBoundary() const { return m_errorBoundary; }

    void setLoadingTimeout(int timeoutMs);
    int loadingTimeout() const { return m_loadingTimeoutMs; }

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // Convenience factory methods (from Simple variant)
    static FluentLoadingIndicator* createSpinner(
        FluentLoadingSize size = FluentLoadingSize::Medium,
        QWidget* parent = nullptr);
    static FluentLoadingIndicator* createDots(
        FluentLoadingSize size = FluentLoadingSize::Medium,
        QWidget* parent = nullptr);
    static FluentLoadingIndicator* createPulse(
        FluentLoadingSize size = FluentLoadingSize::Medium,
        QWidget* parent = nullptr);
    static FluentLoadingIndicator* createSimple(
        FluentLoadingType type,
        FluentLoadingSize size = FluentLoadingSize::Medium,
        QWidget* parent = nullptr);

public slots:
    void start();
    void stop();

signals:
    void loadingTypeChanged(FluentLoadingType type);
    void loadingSizeChanged(FluentLoadingSize size);
    void complexityChanged(FluentLoadingComplexity complexity);
    void colorChanged(const QColor& color);
    void runningChanged(bool running);
    void speedChanged(int speed);
    void textChanged(const QString& text);
    void textVisibleChanged(bool visible);
    void animationProgressChanged(qreal progress);
    void started();
    void stopped();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void updateAnimation();
    void onAnimationStep();

private:
    void setupAnimations();
    void updateColors();
    void updateGeometry();

    // Drawing methods
    void drawSpinner(QPainter& painter, const QRect& rect);
    void drawDots(QPainter& painter, const QRect& rect);
    void drawPulse(QPainter& painter, const QRect& rect);
    void drawBars(QPainter& painter, const QRect& rect);
    void drawRing(QPainter& painter, const QRect& rect);
    void drawWave(QPainter& painter, const QRect& rect);
    void drawText(QPainter& painter, const QRect& rect);

    // Helper methods
    QRect indicatorRect() const;
    QRect textRect() const;
    int getIndicatorSize() const;
    QColor getIndicatorColor() const;
    int getAnimationDuration() const;

private:
    FluentLoadingType m_loadingType{FluentLoadingType::Spinner};
    FluentLoadingSize m_loadingSize{FluentLoadingSize::Medium};
    FluentLoadingComplexity m_complexity{FluentLoadingComplexity::Full};
    QColor m_color;
    bool m_running{false};
    int m_speed{5};  // 1-10 scale
    QString m_text;
    bool m_textVisible{true};

    // Animation state
    qreal m_animationProgress{0.0};
    qreal m_rotationAngle{0.0};
    QList<qreal> m_dotPhases;   // For dots animation
    QList<qreal> m_barHeights;  // For bars animation

    // Animation components
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QTimer* m_animationTimer{nullptr};
    QPropertyAnimation* m_rotationAnimation{nullptr};
    QPropertyAnimation* m_pulseAnimation{nullptr};

    // Error boundary integration
    Core::FluentErrorBoundary* m_errorBoundary{nullptr};
    int m_loadingTimeoutMs{30000};  // 30 seconds default

    // Auto-start behavior
    bool m_autoStart{true};
};

}  // namespace FluentQt::Components
