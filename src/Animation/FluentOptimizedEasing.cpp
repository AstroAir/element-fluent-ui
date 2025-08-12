#include "FluentQt/Animation/FluentOptimizedEasing.h"
#include <QApplication>
#include <QScreen>
#include <QSettings>
#include <QtMath>
#include <algorithm>

using namespace std::chrono_literals;

namespace FluentQt::Animation {

QEasingCurve FluentOptimizedEasing::createOptimizedCurve(
    CurveType type, const OptimizationConfig& config) {
    QEasingCurve curve;

    switch (type) {
        case CurveType::PerceptualEaseOut:
            curve.setCustomType([](qreal t) { return perceptualEasing(t); });
            break;

        case CurveType::PerceptualEaseIn:
            curve.setCustomType(
                [](qreal t) { return 1.0 - perceptualEasing(1.0 - t); });
            break;

        case CurveType::PerceptualEaseInOut:
            curve.setCustomType([](qreal t) {
                if (t < 0.5) {
                    return 0.5 * (1.0 - perceptualEasing(1.0 - 2.0 * t));
                } else {
                    return 0.5 + 0.5 * perceptualEasing(2.0 * t - 1.0);
                }
            });
            break;

        case CurveType::FastEaseOut:
            // Optimized for 60fps performance
            curve.setType(QEasingCurve::OutQuart);
            break;

        case CurveType::FastEaseIn:
            curve.setType(QEasingCurve::InQuart);
            break;

        case CurveType::FastEaseInOut:
            curve.setType(QEasingCurve::InOutQuart);
            break;

        case CurveType::AccessibleEaseOut:
            // Gentler curves for reduced motion
            curve.setType(QEasingCurve::OutQuad);
            break;

        case CurveType::AccessibleEaseIn:
            curve.setType(QEasingCurve::InQuad);
            break;

        case CurveType::AccessibleEaseInOut:
            curve.setType(QEasingCurve::InOutQuad);
            break;

        case CurveType::MaterialStandard:
            curve.setCustomType(
                [](qreal t) { return materialStandardEasing(t); });
            break;

        case CurveType::MaterialDecelerate:
            curve.setType(QEasingCurve::OutCubic);
            break;

        case CurveType::MaterialAccelerate:
            curve.setType(QEasingCurve::InCubic);
            break;

        case CurveType::FluentStandard:
            curve.setCustomType(
                [](qreal t) { return fluentStandardEasing(t); });
            break;

        case CurveType::FluentEmphasized:
            curve.setType(QEasingCurve::OutBack);
            break;

        case CurveType::FluentSubtle:
            curve.setType(QEasingCurve::OutSine);
            break;

        case CurveType::SpringBounce:
            curve.setCustomType(
                [](qreal t) { return springEasing(t, 300.0, 30.0); });
            break;

        case CurveType::DampedSpring:
            curve.setCustomType(
                [](qreal t) { return springEasing(t, 200.0, 40.0); });
            break;

        case CurveType::ElasticBounce:
            curve.setType(QEasingCurve::OutElastic);
            break;

        case CurveType::AntiJank:
            curve.setCustomType([](qreal t) { return antiJankEasing(t); });
            break;

        case CurveType::HighPerformance:
            curve.setType(QEasingCurve::Linear);  // Fastest to compute
            break;

        case CurveType::BatteryOptimized:
            curve.setCustomType(
                [](qreal t) { return batteryOptimizedEasing(t); });
            break;

        default:
            curve.setType(QEasingCurve::OutCubic);
            break;
    }

    // Apply optimizations based on configuration
    if (config.enableAntiJank) {
        curve = applyAntiJankOptimization(curve);
    }

    if (config.enablePerceptualOptimization) {
        curve = applyPerceptualOptimization(curve);
    }

    if (config.enableBatteryOptimization) {
        curve = applyBatteryOptimization(curve);
    }

    if (config.respectReducedMotion) {
        QSettings settings;
        bool reducedMotion =
            settings.value("accessibility/reducedMotion", false).toBool();
        if (reducedMotion) {
            curve = applyAccessibilityOptimization(curve);
        }
    }

    return curve;
}

QEasingCurve FluentOptimizedEasing::createPerceptuallyLinear(
    const OptimizationConfig& config) {
    Q_UNUSED(config)

    // Create a curve that appears linear to human perception
    QEasingCurve curve;
    curve.setCustomType([](qreal t) {
        // Apply gamma correction for perceptual linearity
        const double gamma = 2.2;
        return qPow(t, 1.0 / gamma);
    });

    return curve;
}

QEasingCurve FluentOptimizedEasing::createAntiJankCurve(
    const OptimizationConfig& config) {
    Q_UNUSED(config)

    QEasingCurve curve;
    curve.setCustomType([](qreal t) { return antiJankEasing(t); });
    return curve;
}

QEasingCurve FluentOptimizedEasing::createBatteryOptimizedCurve(
    const OptimizationConfig& config) {
    Q_UNUSED(config)

    QEasingCurve curve;
    curve.setCustomType([](qreal t) { return batteryOptimizedEasing(t); });
    return curve;
}

std::chrono::milliseconds FluentOptimizedEasing::calculateOptimalDuration(
    double distance, CurveType curveType, const OptimizationConfig& config) {
    // Base duration calculation based on distance
    double baseDuration = qSqrt(distance) * 10.0;  // Square root scaling

    // Adjust based on curve type
    switch (curveType) {
        case CurveType::FastEaseOut:
        case CurveType::FastEaseIn:
        case CurveType::FastEaseInOut:
        case CurveType::HighPerformance:
            baseDuration *= 0.8;  // Faster animations
            break;

        case CurveType::AccessibleEaseOut:
        case CurveType::AccessibleEaseIn:
        case CurveType::AccessibleEaseInOut:
            baseDuration *= 1.5;  // Slower for accessibility
            break;

        case CurveType::SpringBounce:
        case CurveType::DampedSpring:
        case CurveType::ElasticBounce:
            baseDuration *= 1.2;  // Longer for physics-based animations
            break;

        default:
            break;
    }

    // Adjust for target FPS
    if (config.targetFPS < 60.0) {
        baseDuration *= (60.0 / config.targetFPS);
    }

    // Clamp to reasonable bounds
    baseDuration = qBound(50.0, baseDuration,
                          static_cast<double>(config.maxDuration.count()));

    return std::chrono::milliseconds(static_cast<int>(baseDuration));
}

// Private implementation functions
double FluentOptimizedEasing::perceptualEasing(double t) {
    // Perceptually optimized easing based on human motion perception
    // Uses a modified cubic curve that accounts for visual perception
    const double a = 0.25;
    const double b = 0.1;
    const double c = 0.25;

    return a * t * t * t + b * t * t + c * t + (1.0 - a - b - c) * t;
}

double FluentOptimizedEasing::antiJankEasing(double t) {
    // Anti-jank easing designed to minimize frame drops
    // Uses a smooth curve with minimal computational complexity
    if (t <= 0.0)
        return 0.0;
    if (t >= 1.0)
        return 1.0;

    // Simple quadratic that's fast to compute
    return t * t * (3.0 - 2.0 * t);
}

double FluentOptimizedEasing::batteryOptimizedEasing(double t) {
    // Battery-optimized easing with minimal CPU usage
    // Uses linear interpolation with slight smoothing
    if (t <= 0.0)
        return 0.0;
    if (t >= 1.0)
        return 1.0;

    // Minimal computation while maintaining smoothness
    return t * t;
}

double FluentOptimizedEasing::materialStandardEasing(double t) {
    // Material Design standard easing curve
    // Cubic bezier approximation: cubic-bezier(0.4, 0.0, 0.2, 1.0)
    if (t <= 0.0)
        return 0.0;
    if (t >= 1.0)
        return 1.0;

    const double c1 = 0.4;
    const double c2 = 0.0;
    const double c3 = 0.2;
    const double c4 = 1.0;

    // Simplified cubic bezier calculation
    double u = 1.0 - t;
    return 3.0 * u * u * t * c1 + 3.0 * u * t * t * c3 + t * t * t * c4;
}

double FluentOptimizedEasing::fluentStandardEasing(double t) {
    // Fluent Design standard easing curve
    // Optimized for Fluent Design motion principles
    if (t <= 0.0)
        return 0.0;
    if (t >= 1.0)
        return 1.0;

    // Fluent-specific curve with subtle acceleration
    return 1.0 - qPow(1.0 - t, 3.0);
}

double FluentOptimizedEasing::springEasing(double t, double stiffness,
                                           double damping) {
    // Physics-based spring easing
    if (t <= 0.0)
        return 0.0;
    if (t >= 1.0)
        return 1.0;

    double omega = qSqrt(stiffness);
    double zeta = damping / (2.0 * qSqrt(stiffness));

    if (zeta < 1.0) {
        // Underdamped
        double omegaD = omega * qSqrt(1.0 - zeta * zeta);
        return 1.0 - qExp(-zeta * omega * t) * qCos(omegaD * t);
    } else {
        // Overdamped or critically damped
        return 1.0 - qExp(-omega * t);
    }
}

QEasingCurve FluentOptimizedEasing::applyAntiJankOptimization(
    const QEasingCurve& curve) {
    // Apply anti-jank optimizations to existing curve
    QEasingCurve optimized = curve;

    // Simplify complex curves to reduce computation
    if (calculateCurveComplexity(curve) > 0.8) {
        // Replace with simpler equivalent
        optimized.setType(QEasingCurve::OutCubic);
    }

    return optimized;
}

QEasingCurve FluentOptimizedEasing::applyPerceptualOptimization(
    const QEasingCurve& curve) {
    // Apply perceptual optimizations
    Q_UNUSED(curve)

    // For now, return the original curve
    // In a full implementation, this would apply gamma correction
    // and other perceptual adjustments
    return curve;
}

QEasingCurve FluentOptimizedEasing::applyBatteryOptimization(
    const QEasingCurve& curve) {
    // Apply battery optimizations
    Q_UNUSED(curve)

    // Simplify to reduce CPU usage
    QEasingCurve optimized;
    optimized.setType(QEasingCurve::OutQuad);  // Simple and efficient
    return optimized;
}

QEasingCurve FluentOptimizedEasing::applyAccessibilityOptimization(
    const QEasingCurve& curve) {
    // Apply accessibility optimizations for reduced motion
    Q_UNUSED(curve)

    QEasingCurve accessible;
    accessible.setType(QEasingCurve::Linear);  // Simplest motion
    return accessible;
}

double FluentOptimizedEasing::calculateCurveComplexity(
    const QEasingCurve& curve) {
    // Estimate computational complexity of the curve
    switch (curve.type()) {
        case QEasingCurve::Linear:
            return 0.1;
        case QEasingCurve::InQuad:
        case QEasingCurve::OutQuad:
        case QEasingCurve::InOutQuad:
            return 0.3;
        case QEasingCurve::InCubic:
        case QEasingCurve::OutCubic:
        case QEasingCurve::InOutCubic:
            return 0.5;
        case QEasingCurve::InElastic:
        case QEasingCurve::OutElastic:
        case QEasingCurve::InOutElastic:
            return 0.9;
        case QEasingCurve::Custom:
            return 0.8;  // Assume high complexity for custom curves
        default:
            return 0.6;
    }
}

bool FluentOptimizedEasing::willCauseFrmeDrops(const QEasingCurve& curve,
                                               double targetFPS) {
    // Estimate if curve will cause frame drops
    double complexity = calculateCurveComplexity(curve);
    double cpuUsage = estimateCPUUsage(curve);

    // Simple heuristic: high complexity + high CPU usage + high target FPS =
    // potential frame drops
    return (complexity > 0.7 && cpuUsage > 0.6 && targetFPS > 30.0);
}

double FluentOptimizedEasing::estimateCPUUsage(const QEasingCurve& curve) {
    // Estimate CPU usage for the curve
    return calculateCurveComplexity(curve) * 0.8;  // Simplified estimation
}

// FluentTimingOptimizer implementation
std::chrono::milliseconds FluentTimingOptimizer::optimizeDuration(
    std::chrono::milliseconds originalDuration, const QString& animationType,
    const FluentOptimizedEasing::OptimizationConfig& config) {
    double duration = originalDuration.count();

    // Adjust based on animation type
    if (animationType == "fade") {
        duration *= 0.8;  // Fades can be faster
    } else if (animationType == "slide") {
        duration *= 1.1;  // Slides need more time
    } else if (animationType == "scale") {
        duration *= 0.9;  // Scales can be slightly faster
    }

    // Adjust for target FPS
    if (config.targetFPS < 60.0) {
        duration *= (60.0 / config.targetFPS);
    }

    // Make frame-perfect
    int durationMs = static_cast<int>(duration);
    return calculateFramePerfectDuration(std::chrono::milliseconds(durationMs),
                                         config.targetFPS);
}

std::chrono::milliseconds FluentTimingOptimizer::calculateFramePerfectDuration(
    std::chrono::milliseconds desiredDuration, double targetFPS) {
    double frameTime = 1000.0 / targetFPS;  // Frame time in milliseconds
    double frames = desiredDuration.count() / frameTime;

    // Round to nearest frame
    int perfectFrames = static_cast<int>(qRound(frames));
    int perfectDuration = static_cast<int>(perfectFrames * frameTime);

    return std::chrono::milliseconds(perfectDuration);
}

std::chrono::milliseconds FluentTimingOptimizer::getOptimalUpdateInterval(
    const FluentOptimizedEasing::OptimizationConfig& config) {
    if (config.enableBatteryOptimization) {
        return 33ms;  // 30 FPS for battery saving
    } else if (config.targetFPS >= 60.0) {
        return 16ms;  // 60 FPS
    } else {
        int interval = static_cast<int>(1000.0 / config.targetFPS);
        return std::chrono::milliseconds(interval);
    }
}

}  // namespace FluentQt::Animation
