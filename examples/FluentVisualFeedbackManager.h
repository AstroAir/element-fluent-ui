// examples/FluentVisualFeedbackManager.h
// Visual feedback manager for enhanced user experience

#pragma once

#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QGraphicsColorizeEffect>
#include <QTimer>
#include <QEasingCurve>
#include <QColor>
#include <QRect>

namespace FluentQt::Examples {

/**
 * @brief Visual feedback manager for enhanced user interactions
 * 
 * Provides comprehensive visual feedback including:
 * - Theme transition animations
 * - Loading states and progress indicators
 * - Confirmation messages and status updates
 * - Hover and focus effects
 * - Success/error visual feedback
 */
class FluentVisualFeedbackManager : public QObject {
    Q_OBJECT

public:
    enum class FeedbackType {
        Success,
        Warning,
        Error,
        Info,
        Loading,
        Progress
    };

    enum class AnimationType {
        FadeIn,
        FadeOut,
        SlideIn,
        SlideOut,
        Bounce,
        Pulse,
        Shake,
        Glow,
        Scale,
        Rotate
    };

    explicit FluentVisualFeedbackManager(QObject* parent = nullptr);
    ~FluentVisualFeedbackManager() override;

    // Theme transition effects
    void animateThemeTransition(QWidget* widget, int duration = 300);
    void animateColorTransition(QWidget* widget, const QColor& fromColor, const QColor& toColor, int duration = 300);
    void animateOpacityTransition(QWidget* widget, qreal fromOpacity, qreal toOpacity, int duration = 300);

    // Component feedback effects
    void showSuccessFeedback(QWidget* widget, const QString& message = QString());
    void showErrorFeedback(QWidget* widget, const QString& message = QString());
    void showWarningFeedback(QWidget* widget, const QString& message = QString());
    void showInfoFeedback(QWidget* widget, const QString& message = QString());

    // Loading and progress effects
    void showLoadingState(QWidget* widget, const QString& message = "Loading...");
    void hideLoadingState(QWidget* widget);
    void updateProgress(QWidget* widget, int percentage);
    void showProgressFeedback(QWidget* widget, int progress, const QString& message = QString());

    // Interaction effects
    void animateButtonPress(QWidget* button);
    void animateHoverEffect(QWidget* widget, bool entering);
    void animateFocusEffect(QWidget* widget, bool focused);
    void animateSelectionEffect(QWidget* widget, bool selected);

    // Custom animations
    void animateWidget(QWidget* widget, AnimationType type, int duration = 300);
    void animateProperty(QWidget* widget, const QByteArray& property, const QVariant& startValue, const QVariant& endValue, int duration = 300);
    void animateGeometry(QWidget* widget, const QRect& startGeometry, const QRect& endGeometry, int duration = 300);

    // Confirmation and status effects
    void showConfirmationEffect(QWidget* widget, bool confirmed);
    void showStatusChange(QWidget* widget, const QString& status, FeedbackType type);
    void showValidationFeedback(QWidget* widget, bool valid, const QString& message = QString());

    // Utility methods
    void setDefaultDuration(int duration) { m_defaultDuration = duration; }
    int defaultDuration() const { return m_defaultDuration; }
    
    void setDefaultEasing(const QEasingCurve& easing) { m_defaultEasing = easing; }
    QEasingCurve defaultEasing() const { return m_defaultEasing; }

    // Global effects
    void pauseAllAnimations();
    void resumeAllAnimations();
    void stopAllAnimations();
    void clearAllEffects();

public slots:
    void onThemeChanged();
    void onComponentStateChanged(QWidget* widget, const QString& state);

signals:
    void animationStarted(QWidget* widget, AnimationType type);
    void animationFinished(QWidget* widget, AnimationType type);
    void feedbackShown(QWidget* widget, FeedbackType type, const QString& message);

private slots:
    void onAnimationFinished();
    void onEffectFinished();

private:
    void setupAnimation(QPropertyAnimation* animation, QWidget* widget, const QByteArray& property);
    void setupEffect(QGraphicsEffect* effect, QWidget* widget);
    void cleanupAnimation(QPropertyAnimation* animation);
    void cleanupEffect(QGraphicsEffect* effect);

    QGraphicsOpacityEffect* createOpacityEffect(QWidget* widget);
    QGraphicsColorizeEffect* createColorizeEffect(QWidget* widget, const QColor& color);
    QPropertyAnimation* createPropertyAnimation(QWidget* widget, const QByteArray& property);
    QSequentialAnimationGroup* createSequentialGroup();
    QParallelAnimationGroup* createParallelGroup();

    void animateFadeIn(QWidget* widget, int duration);
    void animateFadeOut(QWidget* widget, int duration);
    void animateSlideIn(QWidget* widget, int duration);
    void animateSlideOut(QWidget* widget, int duration);
    void animateBounce(QWidget* widget, int duration);
    void animatePulse(QWidget* widget, int duration);
    void animateShake(QWidget* widget, int duration);
    void animateGlow(QWidget* widget, int duration);
    void animateScale(QWidget* widget, int duration);
    void animateRotate(QWidget* widget, int duration);

    void showTemporaryMessage(QWidget* widget, const QString& message, FeedbackType type);
    void updateWidgetStyle(QWidget* widget, FeedbackType type);
    void restoreWidgetStyle(QWidget* widget);

    // Animation management
    QList<QPropertyAnimation*> m_activeAnimations;
    QList<QGraphicsEffect*> m_activeEffects;
    QHash<QWidget*, QPropertyAnimation*> m_widgetAnimations;
    QHash<QWidget*, QGraphicsEffect*> m_widgetEffects;
    QHash<QWidget*, QString> m_originalStyles;

    // Settings
    int m_defaultDuration{300};
    QEasingCurve m_defaultEasing{QEasingCurve::InOutQuad};

    // Timers
    QTimer* m_cleanupTimer{nullptr};
    QTimer* m_effectTimer{nullptr};

    // State tracking
    bool m_animationsPaused{false};
    QHash<QWidget*, FeedbackType> m_widgetStates;
};

/**
 * @brief Helper class for creating complex animation sequences
 */
class FluentAnimationSequence : public QObject {
    Q_OBJECT

public:
    explicit FluentAnimationSequence(QObject* parent = nullptr);
    ~FluentAnimationSequence() override;

    // Sequence building
    FluentAnimationSequence* addFadeIn(QWidget* widget, int duration = 300);
    FluentAnimationSequence* addFadeOut(QWidget* widget, int duration = 300);
    FluentAnimationSequence* addSlide(QWidget* widget, const QPoint& from, const QPoint& to, int duration = 300);
    FluentAnimationSequence* addScale(QWidget* widget, qreal fromScale, qreal toScale, int duration = 300);
    FluentAnimationSequence* addDelay(int milliseconds);
    FluentAnimationSequence* addCustomAnimation(QPropertyAnimation* animation);

    // Parallel animations
    FluentAnimationSequence* addParallelGroup();
    FluentAnimationSequence* endParallelGroup();

    // Execution
    void start();
    void stop();
    void pause();
    void resume();

    // Configuration
    void setLoopCount(int count);
    void setDirection(QAbstractAnimation::Direction direction);

signals:
    void sequenceStarted();
    void sequenceFinished();
    void sequencePaused();
    void sequenceResumed();

private slots:
    void onGroupFinished();

private:
    QSequentialAnimationGroup* m_mainGroup{nullptr};
    QParallelAnimationGroup* m_currentParallelGroup{nullptr};
    QList<QAbstractAnimation*> m_animations;
    bool m_inParallelGroup{false};
};

} // namespace FluentQt::Examples
