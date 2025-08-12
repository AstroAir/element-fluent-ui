// include/FluentQt/Animation/FluentAnimator.h
#pragma once

#include <QColor>
#include <QEasingCurve>
#include <QGraphicsEffect>
#include <QObject>
#include <QParallelAnimationGroup>
#include <QPoint>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QWidget>
#include <chrono>
#include <functional>
#include <memory>

using namespace std::chrono_literals;

namespace FluentQt::Animation {

enum class FluentEasing {
    Linear,
    EaseOut,
    EaseIn,
    EaseInOut,
    EaseOutQuad,
    EaseInQuad,
    EaseInOutQuad,
    EaseOutCubic,
    EaseInCubic,
    EaseInOutCubic,
    EaseOutQuart,
    EaseInQuart,
    EaseInOutQuart,
    EaseOutQuint,
    EaseInQuint,
    EaseInOutQuint,
    EaseOutSine,
    EaseInSine,
    EaseInOutSine,
    EaseOutExpo,
    EaseInExpo,
    EaseInOutExpo,
    EaseOutCirc,
    EaseInCirc,
    EaseInOutCirc,
    Bounce,
    BounceOut,
    BounceIn,
    BounceInOut,
    Elastic,
    ElasticOut,
    ElasticIn,
    ElasticInOut,
    Back,
    BackOut,
    BackIn,
    BackInOut,
    // Aliases expected by tests
    QuadIn,
    QuadOut,
    QuadInOut,
    CubicIn,
    CubicOut,
    CubicInOut,
    QuartIn,
    QuartOut,
    QuartInOut,
    QuintIn,
    QuintOut,
    QuintInOut,
    SineIn,
    SineOut,
    SineInOut,
    ExpoIn,
    ExpoOut,
    ExpoInOut,
    CircIn,
    CircOut,
    CircInOut
};
enum class FluentAnimationType {
    Fade,
    FadeIn,
    FadeOut,
    Scale,
    ScaleIn,
    ScaleOut,
    Slide,
    SlideUp,
    SlideDown,
    SlideLeft,
    SlideRight,
    Rotate,
    RotateIn,
    RotateOut,
    Flip,
    FlipX,
    FlipY,
    Zoom,
    ZoomIn,
    ZoomOut,
    Color,
    Geometry,
    Morph,
    Reveal,
    Connected,
    Stagger,
    Parallax,
    Custom
};

struct FluentAnimationConfig {
    std::chrono::milliseconds duration{200ms};
    FluentEasing easing{FluentEasing::EaseOut};
    std::chrono::milliseconds delay{0ms};
    int loops{1};
    bool reverseOnComplete{false};
    bool autoStart{true};
    bool deleteOnFinish{false};
    qreal startValue{0.0};
    qreal endValue{1.0};

    // Micro-interaction settings
    bool enableHoverEffects{true};
    bool enableFocusEffects{true};
    bool enablePressEffects{true};

    // Performance settings
    bool useHardwareAcceleration{true};
    bool respectReducedMotion{true};

    // Default constructor for empty brace initialization
    FluentAnimationConfig() = default;
};

class FluentAnimator : public QObject {
    Q_OBJECT

public:
    explicit FluentAnimator(QObject* parent = nullptr);
    ~FluentAnimator() = default;

    // Static convenience methods with ElaWidgetTools-inspired smooth animations
    static std::unique_ptr<QPropertyAnimation> fadeIn(
        QWidget* target, const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> fadeOut(
        QWidget* target, const FluentAnimationConfig& config = {});

    // Overloads expected by tests: direct duration + easing
    static std::unique_ptr<QPropertyAnimation> fadeIn(QWidget* target,
                                                      int durationMs,
                                                      FluentEasing easing);

    static std::unique_ptr<QPropertyAnimation> fadeOut(QWidget* target,
                                                       int durationMs,
                                                       FluentEasing easing);

    // ElaWidgetTools-inspired smooth scroll animations
    static std::unique_ptr<QPropertyAnimation> smoothScroll(
        QWidget* target, int fromValue, int toValue, int durationMs = 300,
        FluentEasing easing = FluentEasing::EaseOutSine);

    // Enhanced expand/collapse animations with proper timing
    static std::unique_ptr<QPropertyAnimation> expandAnimation(
        QWidget* target, bool expand = true, int durationMs = 350,
        FluentEasing easing = FluentEasing::EaseOutCubic);

    static std::unique_ptr<QPropertyAnimation> scaleIn(
        QWidget* target, const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> scaleOut(
        QWidget* target, const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> slideIn(
        QWidget* target, const QPoint& from,
        const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> slideOut(
        QWidget* target, const QPoint& to,
        const FluentAnimationConfig& config = {});

    // Rotation animations
    static std::unique_ptr<QPropertyAnimation> rotateIn(
        QWidget* target, qreal fromAngle = -180.0,
        const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> rotateOut(
        QWidget* target, qreal toAngle = 180.0,
        const FluentAnimationConfig& config = {});

    // Flip animations
    static std::unique_ptr<QSequentialAnimationGroup> flipX(
        QWidget* target, const FluentAnimationConfig& config = {});

    static std::unique_ptr<QSequentialAnimationGroup> flipY(
        QWidget* target, const FluentAnimationConfig& config = {});

    // Zoom animations
    static std::unique_ptr<QPropertyAnimation> zoomIn(
        QWidget* target, const QPoint& center = {},
        const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> zoomOut(
        QWidget* target, const QPoint& center = {},
        const FluentAnimationConfig& config = {});

    // Directional slide animations
    static std::unique_ptr<QPropertyAnimation> slideUp(
        QWidget* target, int distance = 50,
        const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> slideDown(
        QWidget* target, int distance = 50,
        const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> slideLeft(
        QWidget* target, int distance = 50,
        const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> slideRight(
        QWidget* target, int distance = 50,
        const FluentAnimationConfig& config = {});

    // Color transition animation
    static std::unique_ptr<QPropertyAnimation> colorTransition(
        QObject* target, const QByteArray& property, const QColor& from,
        const QColor& to, const FluentAnimationConfig& config = {});

    // Morph animation (geometry + opacity)
    static std::unique_ptr<QParallelAnimationGroup> morphAnimation(
        QWidget* target, const QRect& fromGeometry, const QRect& toGeometry,
        const FluentAnimationConfig& config = {});

    // Reveal animation (Fluent Design signature)
    static std::unique_ptr<QSequentialAnimationGroup> revealAnimation(
        QWidget* target, const QPoint& center = {},
        const FluentAnimationConfig& config = {});

    // Connected animations (for navigations)
    static std::unique_ptr<QParallelAnimationGroup> connectedAnimation(
        QWidget* from, QWidget* to, const FluentAnimationConfig& config = {});

    // Stagger animations for lists
    template <typename Container>
    static std::unique_ptr<QSequentialAnimationGroup> staggerAnimation(
        const Container& widgets, FluentAnimationType type,
        std::chrono::milliseconds staggerDelay = 50ms,
        const FluentAnimationConfig& config = {});

    // Micro-interaction animations
    static std::unique_ptr<QPropertyAnimation> hoverEffect(
        QWidget* target, const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> pressEffect(
        QWidget* target, const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> focusEffect(
        QWidget* target, const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> pulseEffect(
        QWidget* target, const FluentAnimationConfig& config = {});

    static std::unique_ptr<QPropertyAnimation> shakeEffect(
        QWidget* target, const FluentAnimationConfig& config = {});

    // Ripple effect for buttons and interactive elements
    static std::unique_ptr<QSequentialAnimationGroup> rippleEffect(
        QWidget* target, const QPoint& center = {},
        const FluentAnimationConfig& config = {});

    // Parallax animation
    static std::unique_ptr<QPropertyAnimation> parallaxEffect(
        QWidget* target, const QPoint& offset,
        const FluentAnimationConfig& config = {});

    // Loading animations
    static std::unique_ptr<QPropertyAnimation> spinnerAnimation(
        QWidget* target, const FluentAnimationConfig& config = {});

    static std::unique_ptr<QSequentialAnimationGroup> breathingAnimation(
        QWidget* target, const FluentAnimationConfig& config = {});

    static QEasingCurve::Type toQtEasing(FluentEasing easing);

signals:
    void animationStarted();
    void animationFinished();

private:
    static void setupAnimation(QPropertyAnimation* animation,
                               const FluentAnimationConfig& config);
    static void applyMicroInteractionSettings(
        QPropertyAnimation* animation, const FluentAnimationConfig& config);
    static bool shouldRespectReducedMotion();
};

// Implementation of stagger animation template
template <typename Container>
std::unique_ptr<QSequentialAnimationGroup> FluentAnimator::staggerAnimation(
    const Container& widgets, FluentAnimationType type,
    std::chrono::milliseconds staggerDelay,
    const FluentAnimationConfig& config) {
    auto group = std::make_unique<QSequentialAnimationGroup>();

    for (auto* widget : widgets) {
        auto animation = [&]() -> std::unique_ptr<QPropertyAnimation> {
            switch (type) {
                case FluentAnimationType::Fade:
                    return fadeIn(widget, config);
                case FluentAnimationType::Scale:
                    return scaleIn(widget, config);
                // Add other cases...
                default:
                    return fadeIn(widget, config);
            }
        }();

        // Add delay between animations
        if (group->animationCount() > 0) {
            QTimer::singleShot(staggerDelay.count(), [&]() {
                group->addAnimation(animation.release());
            });
        } else {
            group->addAnimation(animation.release());
        }
    }

    return group;
}

// Animation management utility
class FluentAnimationManager : public QObject {
    Q_OBJECT

public:
    static FluentAnimationManager& instance();

    void registerAnimation(QAbstractAnimation* animation);
    void pauseAllAnimations();
    void resumeAllAnimations();
    void stopAllAnimations();
    int activeAnimationCount() const;

private:
    FluentAnimationManager() = default;

    QSet<QAbstractAnimation*> m_activeAnimations;
    QSet<QAbstractAnimation*> m_pausedAnimations;
};

}  // namespace FluentQt::Animation
