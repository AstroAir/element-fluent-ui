// include/FluentQt/Components/FluentProgressBar.h
#pragma once

#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QTimer>
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentComponent.h"

namespace FluentQt::Components {

enum class FluentProgressBarType {
    Determinate,    // Shows specific progress (0-100%)
    Indeterminate,  // Shows ongoing activity without specific progress
    Ring,           // Circular progress indicator
    Dots            // Animated dots indicator
};

enum class FluentProgressBarSize { Small, Medium, Large };

enum class FluentProgressBarState {
    Normal,   // Default state
    Success,  // Success state (green)
    Warning,  // Warning state (yellow/orange)
    Error,    // Error state (red)
    Paused    // Paused state (gray)
};

enum class FluentProgressBarVisualStyle {
    Standard,  // Standard progress bar
    Gradient,  // Gradient fill
    Glow,      // Glow effect
    Pulse      // Pulsing animation
};

class FluentProgressBar : public Core::FluentComponent {
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(FluentProgressBarType progressType READ progressType WRITE
                   setProgressType NOTIFY progressTypeChanged)
    Q_PROPERTY(FluentProgressBarSize progressSize READ progressSize WRITE
                   setProgressSize NOTIFY progressSizeChanged)
    Q_PROPERTY(FluentProgressBarState progressState READ progressState WRITE
                   setProgressState NOTIFY progressStateChanged)
    Q_PROPERTY(FluentProgressBarVisualStyle visualStyle READ visualStyle WRITE
                   setVisualStyle NOTIFY visualStyleChanged)
    Q_PROPERTY(bool textVisible READ isTextVisible WRITE setTextVisible NOTIFY
                   textVisibleChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(qreal animationProgress READ animationProgress WRITE
                   setAnimationProgress NOTIFY animationProgressChanged)
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY
                   accentColorChanged)
    Q_PROPERTY(bool glowEnabled READ isGlowEnabled WRITE setGlowEnabled NOTIFY
                   glowEnabledChanged)
    Q_PROPERTY(qreal glowIntensity READ glowIntensity WRITE setGlowIntensity
                   NOTIFY glowIntensityChanged)
    Q_PROPERTY(bool revealEnabled READ isRevealEnabled WRITE setRevealEnabled
                   NOTIFY revealEnabledChanged)
    Q_PROPERTY(qreal revealProgress READ revealProgress WRITE setRevealProgress
                   NOTIFY revealProgressChanged)

public:
    explicit FluentProgressBar(QWidget* parent = nullptr);
    explicit FluentProgressBar(FluentProgressBarType type,
                               QWidget* parent = nullptr);
    ~FluentProgressBar() override;

    // Value properties
    int value() const;
    void setValue(int value);

    int minimum() const;
    void setMinimum(int minimum);

    int maximum() const;
    void setMaximum(int maximum);

    void setRange(int minimum, int maximum);

    // Text properties
    QString text() const;
    void setText(const QString& text);

    bool isTextVisible() const;
    void setTextVisible(bool visible);

    // Type and appearance
    FluentProgressBarType progressType() const;
    void setProgressType(FluentProgressBarType type);

    FluentProgressBarSize progressSize() const;
    void setProgressSize(FluentProgressBarSize size);

    FluentProgressBarState progressState() const;
    void setProgressState(FluentProgressBarState state);

    FluentProgressBarVisualStyle visualStyle() const;
    void setVisualStyle(FluentProgressBarVisualStyle style);

    // Animation
    bool isAnimated() const;
    void setAnimated(bool animated);
    qreal animationProgress() const;
    void setAnimationProgress(qreal progress);

    // Color customization
    QColor accentColor() const;
    void setAccentColor(const QColor& color);

    // Visual effects
    bool isGlowEnabled() const;
    void setGlowEnabled(bool enabled);
    qreal glowIntensity() const;
    void setGlowIntensity(qreal intensity);

    bool isRevealEnabled() const;
    void setRevealEnabled(bool enabled);
    qreal revealProgress() const;
    void setRevealProgress(qreal progress);

    // Utility methods
    qreal percentage() const;
    bool isComplete() const;
    void reset();

    // State queries
    bool isRunning() const;
    bool isPaused() const;
    bool isIndeterminate() const;

    // Color helpers
    QColor getStateColor() const;
    QColor getProgressColor() const;
    QColor getBackgroundColor() const;

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void start();
    void stop();
    void pause();
    void resume();
    void setProgress(int value, bool animated = true);
    void incrementProgress(int delta = 1);
    void setComplete();
    void setError(const QString& errorMessage = QString());

signals:
    void valueChanged(int value);
    void minimumChanged(int minimum);
    void maximumChanged(int maximum);
    void textChanged(const QString& text);
    void progressTypeChanged(FluentProgressBarType type);
    void progressSizeChanged(FluentProgressBarSize size);
    void progressStateChanged(FluentProgressBarState state);
    void visualStyleChanged(FluentProgressBarVisualStyle style);
    void textVisibleChanged(bool visible);
    void animatedChanged(bool animated);
    void animationProgressChanged(qreal progress);
    void accentColorChanged(const QColor& color);
    void glowEnabledChanged(bool enabled);
    void glowIntensityChanged(qreal intensity);
    void revealEnabledChanged(bool enabled);
    void revealProgressChanged(qreal progress);
    void finished();
    void started();
    void paused();
    void resumed();
    void errorOccurred(const QString& message);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;

private slots:
    void updateAnimation();
    void onAnimationStep();
    void updateColors();
    void onGlowAnimationFinished();
    void onRevealAnimationFinished();

private:
    void setupAnimations();
    void setupTimer();
    void setupGlowEffect();
    void updateGeometry();
    void updateTextContent();
    void updateVisualEffects();

    // Drawing methods
    void drawDeterminateBar(QPainter& painter, const QRect& rect);
    void drawIndeterminateBar(QPainter& painter, const QRect& rect);
    void drawRingProgress(QPainter& painter, const QRect& rect);
    void drawDotsProgress(QPainter& painter, const QRect& rect);
    void drawProgressText(QPainter& painter, const QRect& rect);
    void drawGlowEffect(QPainter& painter, const QRect& rect);
    void drawRevealEffect(QPainter& painter, const QRect& rect);
    void drawGradientFill(QPainter& painter, const QRect& rect, qreal progress);

    // Helper methods
    QRect progressRect() const;
    QRect textRect() const;
    int getBarHeight() const;
    int getRingSize() const;

private:
    // Core properties
    int m_value{0};
    int m_minimum{0};
    int m_maximum{100};
    QString m_text;
    FluentProgressBarType m_progressType{FluentProgressBarType::Determinate};
    FluentProgressBarSize m_progressSize{FluentProgressBarSize::Medium};
    FluentProgressBarState m_progressState{FluentProgressBarState::Normal};
    FluentProgressBarVisualStyle m_visualStyle{
        FluentProgressBarVisualStyle::Standard};
    bool m_textVisible{true};
    bool m_animated{true};
    QColor m_accentColor;

    // Visual effects
    bool m_glowEnabled{false};
    qreal m_glowIntensity{0.5};
    bool m_revealEnabled{false};
    qreal m_revealProgress{0.0};
    QPointF m_revealCenter;

    // Animation state
    bool m_isRunning{false};
    bool m_isPaused{false};
    qreal m_animationProgress{0.0};  // For indeterminate animations
    int m_animationDirection{1};     // 1 for forward, -1 for backward

    // Animation components
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_valueAnimation{nullptr};
    QPropertyAnimation* m_indeterminateAnimation{nullptr};
    QPropertyAnimation* m_glowAnimation{nullptr};
    QPropertyAnimation* m_revealAnimation{nullptr};
    QTimer* m_animationTimer{nullptr};

    // Visual effects
    QGraphicsDropShadowEffect* m_glowEffect{nullptr};

    // Cached values
    QString m_cachedText;
    QRect m_cachedProgressRect;
    QRect m_cachedTextRect;
};

}  // namespace FluentQt::Components
