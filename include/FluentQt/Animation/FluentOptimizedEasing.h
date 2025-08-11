#pragma once

#include <QEasingCurve>
#include <QPointF>
#include <functional>
#include <chrono>

namespace FluentQt::Animation {

/**
 * @brief Optimized easing curves designed for smooth, jank-free animations
 * 
 * This class provides perceptually optimized easing curves that reduce
 * animation jank and provide smoother transitions across different devices.
 */
class FluentOptimizedEasing {
public:
    enum class CurveType {
        // Perceptually optimized curves
        PerceptualEaseOut,      // Optimized for human perception
        PerceptualEaseIn,       // Smooth acceleration
        PerceptualEaseInOut,    // Balanced acceleration/deceleration
        
        // Performance optimized curves
        FastEaseOut,            // Optimized for 60fps
        FastEaseIn,             // Quick response
        FastEaseInOut,          // Balanced performance
        
        // Accessibility optimized curves
        AccessibleEaseOut,      // Reduced motion compliant
        AccessibleEaseIn,       // Gentle acceleration
        AccessibleEaseInOut,    // Smooth transitions
        
        // Material Design inspired
        MaterialStandard,       // Material Design standard curve
        MaterialDecelerate,     // Material decelerate curve
        MaterialAccelerate,     // Material accelerate curve
        
        // Fluent Design optimized
        FluentStandard,         // Fluent Design standard
        FluentEmphasized,       // Emphasized motion
        FluentSubtle,           // Subtle motion
        
        // Physics-based
        SpringBounce,           // Natural spring motion
        DampedSpring,           // Damped spring motion
        ElasticBounce,          // Elastic bounce effect
        
        // Custom optimized
        AntiJank,               // Specifically designed to reduce jank
        HighPerformance,        // Optimized for performance
        BatteryOptimized        // Optimized for battery life
    };

    /**
     * @brief Configuration for easing curve optimization
     */
    struct OptimizationConfig {
        double targetFPS = 60.0;                    // Target frame rate
        bool enableAntiJank = true;                 // Enable anti-jank optimizations
        bool enablePerceptualOptimization = true;   // Enable perceptual optimizations
        bool respectReducedMotion = true;           // Respect accessibility preferences
        bool enableBatteryOptimization = false;    // Optimize for battery life
        double devicePixelRatio = 1.0;             // Device pixel ratio for optimization
        std::chrono::milliseconds maxDuration{500}; // Maximum recommended duration
    };

    /**
     * @brief Create an optimized easing curve
     */
    static QEasingCurve createOptimizedCurve(
        CurveType type, 
        const OptimizationConfig& config = {}
    );

    /**
     * @brief Create a perceptually linear easing curve
     * 
     * This creates an easing curve that appears linear to human perception,
     * accounting for the non-linear way humans perceive motion.
     */
    static QEasingCurve createPerceptuallyLinear(
        const OptimizationConfig& config = {}
    );

    /**
     * @brief Create an anti-jank easing curve
     * 
     * This curve is specifically designed to minimize frame drops and
     * provide smooth animation even on lower-end devices.
     */
    static QEasingCurve createAntiJankCurve(
        const OptimizationConfig& config = {}
    );

    /**
     * @brief Create a battery-optimized easing curve
     * 
     * This curve reduces CPU/GPU usage while maintaining visual quality.
     */
    static QEasingCurve createBatteryOptimizedCurve(
        const OptimizationConfig& config = {}
    );

    /**
     * @brief Optimize an existing easing curve
     */
    static QEasingCurve optimizeCurve(
        const QEasingCurve& originalCurve,
        const OptimizationConfig& config = {}
    );

    /**
     * @brief Calculate optimal animation duration based on distance and curve
     */
    static std::chrono::milliseconds calculateOptimalDuration(
        double distance,
        CurveType curveType,
        const OptimizationConfig& config = {}
    );

    /**
     * @brief Check if a curve is optimized for the given configuration
     */
    static bool isCurveOptimized(
        const QEasingCurve& curve,
        const OptimizationConfig& config = {}
    );

    /**
     * @brief Get recommended curve for specific use cases
     */
    static CurveType getRecommendedCurve(
        const QString& useCase,
        const OptimizationConfig& config = {}
    );

private:
    // Internal optimization functions
    static double perceptualEasing(double t);
    static double antiJankEasing(double t);
    static double batteryOptimizedEasing(double t);
    static double materialStandardEasing(double t);
    static double fluentStandardEasing(double t);
    static double springEasing(double t, double stiffness, double damping);
    
    // Optimization utilities
    static QEasingCurve applyAntiJankOptimization(const QEasingCurve& curve);
    static QEasingCurve applyPerceptualOptimization(const QEasingCurve& curve);
    static QEasingCurve applyBatteryOptimization(const QEasingCurve& curve);
    static QEasingCurve applyAccessibilityOptimization(const QEasingCurve& curve);
    
    // Performance analysis
    static double calculateCurveComplexity(const QEasingCurve& curve);
    static bool willCauseFrmeDrops(const QEasingCurve& curve, double targetFPS);
    static double estimateCPUUsage(const QEasingCurve& curve);
};

/**
 * @brief Timing function optimizer for smooth animations
 */
class FluentTimingOptimizer {
public:
    /**
     * @brief Optimize animation timing based on content and device capabilities
     */
    static std::chrono::milliseconds optimizeDuration(
        std::chrono::milliseconds originalDuration,
        const QString& animationType,
        const FluentOptimizedEasing::OptimizationConfig& config = {}
    );

    /**
     * @brief Calculate frame-perfect timing
     */
    static std::chrono::milliseconds calculateFramePerfectDuration(
        std::chrono::milliseconds desiredDuration,
        double targetFPS = 60.0
    );

    /**
     * @brief Get optimal update interval for animation
     */
    static std::chrono::milliseconds getOptimalUpdateInterval(
        const FluentOptimizedEasing::OptimizationConfig& config = {}
    );
};

} // namespace FluentQt::Animation
