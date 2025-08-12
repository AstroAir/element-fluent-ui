/**
 * @file Animation.h
 * @brief Comprehensive header for FluentQt animation system
 *
 * This header includes all animation-related classes and utilities for
 * creating smooth, performant animations in FluentQt applications.
 * It provides access to the animation engine, easing functions,
 * performance optimization, and specialized animation types.
 *
 * @author Max Qian
 * @copyright MIT License
 * @version 1.0.0
 */

#pragma once

// Core Animation System
#include "FluentQt/Animation/FluentAnimationPerformanceManager.h"
#include "FluentQt/Animation/FluentAnimator.h"

// Advanced Animation Features
#include "FluentQt/Animation/FluentAdvancedAnimator.h"
#include "FluentQt/Animation/FluentAdvancedEasing.h"
#include "FluentQt/Animation/FluentAnimationBatch.h"
#include "FluentQt/Animation/FluentGestureAnimations.h"

// Specialized Animators
#include "FluentQt/Animation/FluentGPUAnimator.h"
#include "FluentQt/Animation/FluentPhysicsAnimator.h"
#include "FluentQt/Animation/FluentSkeletonAnimator.h"

// Accessibility Integration
#include "FluentQt/Accessibility/FluentAccessibleAnimator.h"

/**
 * @namespace FluentQt::Animation
 * @brief Namespace containing all animation functionality
 *
 * The Animation namespace provides a comprehensive animation system
 * designed for smooth, performant UI animations that follow Fluent
 * Design principles. It includes:
 *
 * **Core Animation Features:**
 * - FluentAnimator - Main animation engine
 * - 36 easing functions (linear to advanced elastic/bounce)
 * - 28+ animation types (fade, slide, scale, rotate, etc.)
 * - Hardware acceleration support
 * - Reduced motion accessibility support
 *
 * **Animation Types:**
 * - Fade animations (in/out with various directions)
 * - Slide animations (up/down/left/right)
 * - Scale animations (in/out with different origins)
 * - Rotation animations (clockwise/counterclockwise)
 * - Morph animations (shape transformations)
 * - Reveal animations (Fluent signature effect)
 * - Connected animations (for navigation)
 * - Stagger animations (for lists/grids)
 *
 * **Micro-interactions:**
 * - Hover effects
 * - Press feedback
 * - Focus indication
 * - Attention effects (pulse, shake, bounce)
 * - Loading states
 *
 * **Performance Features:**
 * - GPU acceleration when available
 * - Automatic performance monitoring
 * - Frame rate optimization
 * - Memory usage tracking
 * - Battery-aware animations
 *
 * **Accessibility:**
 * - Respects system reduced motion settings
 * - Alternative feedback for motion-sensitive users
 * - Screen reader compatible animations
 * - High contrast mode support
 *
 * @code
 * // Basic animation usage
 * auto* animator = new FluentQt::Animation::FluentAnimator(this);
 *
 * // Fade in animation
 * auto* fadeIn = animator->fadeIn(widget, 300, FluentEasing::CubicOut);
 * fadeIn->start();
 *
 * // Slide animation with callback
 * auto* slideUp = animator->slideUp(widget, 250);
 * connect(slideUp, &QPropertyAnimation::finished, [=]() {
 *     qDebug() << "Animation completed!";
 * });
 * slideUp->start();
 *
 * // Micro-interaction
 * animator->hoverEffect(button);
 * animator->pressEffect(button);
 * @endcode
 */
namespace FluentQt::Animation {

/**
 * @enum FluentAnimationType
 * @brief Available animation types
 */
enum class FluentAnimationType {
    // Fade animations
    FadeIn,
    FadeOut,
    FadeInUp,
    FadeInDown,
    FadeInLeft,
    FadeInRight,

    // Slide animations
    SlideUp,
    SlideDown,
    SlideLeft,
    SlideRight,
    SlideInUp,
    SlideInDown,
    SlideInLeft,
    SlideInRight,

    // Scale animations
    ScaleIn,
    ScaleOut,
    ScaleInCenter,
    ScaleInTopLeft,
    ScaleInTopRight,
    ScaleInBottomLeft,
    ScaleInBottomRight,

    // Rotation animations
    RotateIn,
    RotateOut,
    RotateClockwise,
    RotateCounterClockwise,

    // Special effects
    Reveal,
    Morph,
    Bounce,
    Pulse,
    Shake,
    Wobble,
    Flip,

    // Connected animations
    Connected,
    Stagger
};

/**
 * @enum FluentEasing
 * @brief Easing functions for animations
 */
enum class FluentEasing {
    // Basic easing
    Linear,

    // Quadratic
    QuadIn,
    QuadOut,
    QuadInOut,

    // Cubic
    CubicIn,
    CubicOut,
    CubicInOut,

    // Quartic
    QuartIn,
    QuartOut,
    QuartInOut,

    // Quintic
    QuintIn,
    QuintOut,
    QuintInOut,

    // Sinusoidal
    SineIn,
    SineOut,
    SineInOut,

    // Exponential
    ExpoIn,
    ExpoOut,
    ExpoInOut,

    // Circular
    CircIn,
    CircOut,
    CircInOut,

    // Back
    BackIn,
    BackOut,
    BackInOut,

    // Elastic
    ElasticIn,
    ElasticOut,
    ElasticInOut,

    // Bounce
    BounceIn,
    BounceOut,
    BounceInOut
};

/**
 * @enum FluentAnimationDirection
 * @brief Animation direction for directional animations
 */
enum class FluentAnimationDirection {
    Up,
    Down,
    Left,
    Right,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Center
};

/**
 * @struct FluentAnimationConfig
 * @brief Configuration for animations
 */
struct FluentAnimationConfig {
    int duration = 300;  ///< Animation duration in milliseconds
    FluentEasing easing = FluentEasing::CubicOut;  ///< Easing function
    int delay = 0;  ///< Delay before starting in milliseconds
    bool respectReducedMotion =
        true;  ///< Honor accessibility reduced motion setting
    bool useHardwareAcceleration =
        true;                      ///< Use GPU acceleration when available
    double opacity = 1.0;          ///< Target opacity (for fade animations)
    QPoint offset = QPoint(0, 0);  ///< Offset for slide animations
    double scale = 1.0;            ///< Scale factor for scale animations
    double rotation = 0.0;         ///< Rotation angle in degrees
    FluentAnimationDirection direction =
        FluentAnimationDirection::Up;  ///< Animation direction
};

/**
 * @brief Initialize the animation system
 *
 * This function initializes the FluentQt animation system, sets up
 * performance monitoring, and configures accessibility settings.
 *
 * @return true if initialization was successful
 */
bool initializeAnimation();

/**
 * @brief Create an animator instance
 *
 * @param parent Parent object for the animator
 * @return Pointer to a new FluentAnimator instance
 */
FluentAnimator* createAnimator(QObject* parent = nullptr);

/**
 * @brief Get the global animation performance manager
 * @return Reference to the performance manager
 */
FluentAnimationPerformanceManager& performanceManager();

/**
 * @brief Check if reduced motion is enabled
 * @return true if reduced motion is enabled (accessibility setting)
 */
bool isReducedMotionEnabled();

/**
 * @brief Set global reduced motion override
 * @param enabled Whether to enable reduced motion globally
 */
void setReducedMotionEnabled(bool enabled);

/**
 * @brief Check if hardware acceleration is available
 * @return true if GPU acceleration is available
 */
bool isHardwareAccelerationAvailable();

/**
 * @brief Get the recommended duration for an animation type
 * @param type The animation type
 * @return Recommended duration in milliseconds
 */
int getRecommendedDuration(FluentAnimationType type);

/**
 * @brief Get the recommended easing for an animation type
 * @param type The animation type
 * @return Recommended easing function
 */
FluentEasing getRecommendedEasing(FluentAnimationType type);

/**
 * @brief Create a default configuration for an animation type
 * @param type The animation type
 * @return Default configuration for the animation
 */
FluentAnimationConfig getDefaultConfig(FluentAnimationType type);

/**
 * @brief Convert Qt easing curve to FluentEasing
 * @param curve Qt easing curve
 * @return Equivalent FluentEasing value
 */
FluentEasing fromQtEasing(QEasingCurve::Type curve);

/**
 * @brief Convert FluentEasing to Qt easing curve
 * @param easing FluentEasing value
 * @return Equivalent Qt easing curve
 */
QEasingCurve::Type toQtEasing(FluentEasing easing);

}  // namespace FluentQt::Animation
