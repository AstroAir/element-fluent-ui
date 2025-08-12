// include/FluentQt/Components/FluentProgressBar.h
#pragma once

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
    Q_PROPERTY(bool textVisible READ isTextVisible WRITE setTextVisible NOTIFY
                   textVisibleChanged)
    Q_PROPERTY(
        bool animated READ isAnimated WRITE setAnimated NOTIFY animatedChanged)
    Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY
                   accentColorChanged)

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

    // Animation
    bool isAnimated() const;
    void setAnimated(bool animated);

    // Color customization
    QColor accentColor() const;
    void setAccentColor(const QColor& color);

    // Utility methods
    qreal percentage() const;
    bool isComplete() const;
    void reset();

    // Size hints
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void start();
    void stop();
    void pause();
    void resume();

signals:
    void valueChanged(int value);
    void minimumChanged(int minimum);
    void maximumChanged(int maximum);
    void textChanged(const QString& text);
    void progressTypeChanged(FluentProgressBarType type);
    void progressSizeChanged(FluentProgressBarSize size);
    void textVisibleChanged(bool visible);
    void animatedChanged(bool animated);
    void accentColorChanged(const QColor& color);
    void finished();
    void started();
    void paused();
    void resumed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void updateAnimation();
    void onAnimationStep();
    void updateColors();

private:
    void setupAnimations();
    void setupTimer();
    void updateGeometry();
    void updateTextContent();

    // Drawing methods
    void drawDeterminateBar(QPainter& painter, const QRect& rect);
    void drawIndeterminateBar(QPainter& painter, const QRect& rect);
    void drawRingProgress(QPainter& painter, const QRect& rect);
    void drawDotsProgress(QPainter& painter, const QRect& rect);
    void drawProgressText(QPainter& painter, const QRect& rect);

    // Helper methods
    QRect progressRect() const;
    QRect textRect() const;
    int getBarHeight() const;
    int getRingSize() const;
    QColor getProgressColor() const;
    QColor getBackgroundColor() const;

private:
    // Core properties
    int m_value{0};
    int m_minimum{0};
    int m_maximum{100};
    QString m_text;
    FluentProgressBarType m_progressType{FluentProgressBarType::Determinate};
    FluentProgressBarSize m_progressSize{FluentProgressBarSize::Medium};
    bool m_textVisible{true};
    bool m_animated{true};
    QColor m_accentColor;

    // Animation state
    bool m_isRunning{false};
    bool m_isPaused{false};
    qreal m_animationProgress{0.0};  // For indeterminate animations
    int m_animationDirection{1};     // 1 for forward, -1 for backward

    // Animation components
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QPropertyAnimation* m_valueAnimation{nullptr};
    QPropertyAnimation* m_indeterminateAnimation{nullptr};
    QTimer* m_animationTimer{nullptr};

    // Cached values
    QString m_cachedText;
    QRect m_cachedProgressRect;
    QRect m_cachedTextRect;
};

}  // namespace FluentQt::Components
