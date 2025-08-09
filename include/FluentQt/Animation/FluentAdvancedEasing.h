// include/FluentQt/Animation/FluentAdvancedEasing.h
#pragma once

#include <QEasingCurve>
#include <QPointF>
#include <QColor>
#include <QTransform>
#include <QVariant>
#include <QLocale>
#include <QMap>
#include <QList>
#include <functional>
#include <memory>
#include <cmath>

namespace FluentQt::Animation {

// Advanced easing types beyond Qt's standard curves
enum class FluentAdvancedEasingType {
    // Fluent Design specific
    FluentAccelerate,       // Fluent accelerate curve
    FluentDecelerate,       // Fluent decelerate curve
    FluentStandard,         // Fluent standard curve
    FluentEmphasized,       // Fluent emphasized curve
    
    // Physics-based
    Spring,                 // Spring physics
    Bounce,                 // Bounce physics
    Gravity,                // Gravity simulation
    Friction,               // Friction simulation
    
    // Perceptual
    Perceptual,             // Perceptually uniform
    Anticipation,           // Anticipation curve
    Overshoot,              // Overshoot curve
    
    // Cultural
    Western,                // Western animation preference
    Eastern,                // Eastern animation preference
    Minimal,                // Minimal movement
    Expressive,             // Expressive movement
    
    // Accessibility
    Reduced,                // Reduced motion
    Enhanced,               // Enhanced for accessibility
    HighContrast,           // High contrast compatible
    
    // Custom
    Bezier,                 // Custom Bezier curve
    Parametric,             // Parametric curve
    Custom                  // Custom function
};

// Interpolation types for complex values
enum class FluentInterpolationType {
    Linear,                 // Linear interpolation
    Smooth,                 // Smooth interpolation
    Discrete,               // Discrete steps
    Spline,                 // Spline interpolation
    Bezier,                 // Bezier interpolation
    Hermite,                // Hermite interpolation
    Catmull,                // Catmull-Rom spline
    Custom                  // Custom interpolation
};

// Easing configuration
struct FluentEasingConfig {
    FluentAdvancedEasingType type{FluentAdvancedEasingType::FluentStandard};
    
    // Physics parameters
    double mass{1.0};
    double stiffness{300.0};
    double damping{30.0};
    double friction{0.7};
    double gravity{9.81};
    double bounciness{0.8};
    
    // Bezier control points (for custom Bezier curves)
    QPointF controlPoint1{0.25, 0.1};
    QPointF controlPoint2{0.25, 1.0};
    
    // Parametric function (for parametric curves)
    std::function<double(double)> parametricFunction;
    
    // Cultural adaptation
    QLocale locale;
    bool adaptToCulture{false};
    
    // Accessibility adaptation
    bool respectReducedMotion{true};
    bool enhanceForAccessibility{false};
    
    // Performance optimization
    bool enableCaching{true};
    int cacheSize{1000};
    
    // Custom properties
    QMap<QString, QVariant> customProperties;
};

// Advanced easing curve with cultural and accessibility awareness
class FluentAdvancedEasingCurve {
public:
    FluentAdvancedEasingCurve(FluentAdvancedEasingType type = FluentAdvancedEasingType::FluentStandard);
    FluentAdvancedEasingCurve(const FluentEasingConfig& config);
    FluentAdvancedEasingCurve(const QEasingCurve& qtCurve);
    
    // Configuration
    void setType(FluentAdvancedEasingType type);
    FluentAdvancedEasingType getType() const { return m_config.type; }
    void setConfig(const FluentEasingConfig& config);
    FluentEasingConfig getConfig() const { return m_config; }
    
    // Easing calculation
    double valueForProgress(double progress) const;
    QList<double> valuesForProgresses(const QList<double>& progresses) const;
    
    // Physics-based easing
    void setPhysicsParameters(double mass, double stiffness, double damping);
    void setSpringParameters(double stiffness, double damping);
    void setBounceParameters(double bounciness, double friction);
    void setGravityParameters(double gravity, double friction);
    
    // Custom curves
    void setBezierControlPoints(const QPointF& cp1, const QPointF& cp2);
    void setParametricFunction(std::function<double(double)> function);
    void setCustomFunction(std::function<double(double)> function);
    
    // Cultural adaptation
    void adaptForCulture(const QLocale& locale);
    void setWesternStyle();
    void setEasternStyle();
    void setMinimalStyle();
    void setExpressiveStyle();
    
    // Accessibility adaptation
    void adaptForAccessibility(bool reducedMotion = false, bool enhanced = false);
    void setReducedMotionMode(bool enabled);
    void setEnhancedAccessibilityMode(bool enabled);
    
    // Conversion
    QEasingCurve toQEasingCurve() const;
    static FluentAdvancedEasingCurve fromQEasingCurve(const QEasingCurve& curve);
    
    // Utility
    FluentAdvancedEasingCurve reversed() const;
    FluentAdvancedEasingCurve scaled(double factor) const;
    FluentAdvancedEasingCurve combined(const FluentAdvancedEasingCurve& other, double weight = 0.5) const;
    
    // Performance
    void enableCaching(bool enabled = true);
    void clearCache();
    int getCacheSize() const;

private:
    // Easing implementations
    double fluentAccelerate(double t) const;
    double fluentDecelerate(double t) const;
    double fluentStandard(double t) const;
    double fluentEmphasized(double t) const;
    double springEasing(double t) const;
    double bounceEasing(double t) const;
    double gravityEasing(double t) const;
    double frictionEasing(double t) const;
    double perceptualEasing(double t) const;
    double anticipationEasing(double t) const;
    double overshootEasing(double t) const;
    double bezierEasing(double t) const;
    
    // Cultural adaptations
    double adaptForWesternCulture(double value) const;
    double adaptForEasternCulture(double value) const;
    double adaptForMinimalStyle(double value) const;
    double adaptForExpressiveStyle(double value) const;
    
    // Accessibility adaptations
    double adaptForReducedMotion(double value) const;
    double adaptForEnhancedAccessibility(double value) const;
    
    // Utility functions
    double cubicBezier(double t, double p1, double p2) const;
    double clamp(double value, double min = 0.0, double max = 1.0) const;
    
    // Caching
    mutable QMap<double, double> m_cache;
    mutable bool m_cacheEnabled{true};

private:
    FluentEasingConfig m_config;
};

// Advanced interpolator for complex value types
template<typename T>
class FluentAdvancedInterpolator {
public:
    FluentAdvancedInterpolator(FluentInterpolationType type = FluentInterpolationType::Linear);
    
    // Configuration
    void setType(FluentInterpolationType type);
    FluentInterpolationType getType() const { return m_type; }
    void setEasingCurve(const FluentAdvancedEasingCurve& curve);
    
    // Interpolation
    T interpolate(const T& from, const T& to, double progress) const;
    QList<T> interpolateSequence(const QList<T>& keyframes, double progress) const;
    T interpolateSpline(const QList<T>& controlPoints, double progress) const;
    
    // Keyframe animation
    void addKeyframe(double time, const T& value);
    void removeKeyframe(double time);
    void clearKeyframes();
    T valueAtTime(double time) const;
    
    // Custom interpolation
    void setCustomInterpolator(std::function<T(const T&, const T&, double)> interpolator);

private:
    // Type-specific interpolation implementations
    T linearInterpolate(const T& from, const T& to, double progress) const;
    T smoothInterpolate(const T& from, const T& to, double progress) const;
    T discreteInterpolate(const T& from, const T& to, double progress) const;
    T splineInterpolate(const QList<T>& points, double progress) const;
    T bezierInterpolate(const QList<T>& points, double progress) const;
    T hermiteInterpolate(const QList<T>& points, double progress) const;
    T catmullInterpolate(const QList<T>& points, double progress) const;

private:
    FluentInterpolationType m_type;
    FluentAdvancedEasingCurve m_easingCurve;
    QMap<double, T> m_keyframes;
    std::function<T(const T&, const T&, double)> m_customInterpolator;
};

// Specialized interpolators for common types
using FluentColorInterpolator = FluentAdvancedInterpolator<QColor>;
using FluentPointInterpolator = FluentAdvancedInterpolator<QPointF>;
using FluentSizeInterpolator = FluentAdvancedInterpolator<QSizeF>;
using FluentRectInterpolator = FluentAdvancedInterpolator<QRectF>;
using FluentTransformInterpolator = FluentAdvancedInterpolator<QTransform>;

// Color interpolation specializations
template<>
class FluentAdvancedInterpolator<QColor> {
public:
    enum class ColorSpace {
        RGB,        // RGB color space
        HSV,        // HSV color space
        HSL,        // HSL color space
        LAB,        // LAB color space
        LCH         // LCH color space
    };
    
    FluentAdvancedInterpolator(FluentInterpolationType type = FluentInterpolationType::Linear);
    
    void setColorSpace(ColorSpace colorSpace);
    ColorSpace getColorSpace() const { return m_colorSpace; }
    
    QColor interpolate(const QColor& from, const QColor& to, double progress) const;
    QList<QColor> generateGradient(const QColor& from, const QColor& to, int steps) const;
    QList<QColor> generatePalette(const QList<QColor>& keyColors, int totalColors) const;

private:
    QColor interpolateRGB(const QColor& from, const QColor& to, double progress) const;
    QColor interpolateHSV(const QColor& from, const QColor& to, double progress) const;
    QColor interpolateHSL(const QColor& from, const QColor& to, double progress) const;
    QColor interpolateLAB(const QColor& from, const QColor& to, double progress) const;
    QColor interpolateLCH(const QColor& from, const QColor& to, double progress) const;

private:
    FluentInterpolationType m_type;
    ColorSpace m_colorSpace{ColorSpace::RGB};
    FluentAdvancedEasingCurve m_easingCurve;
};

// Easing curve factory
class FluentEasingFactory {
public:
    // Standard Fluent curves
    static FluentAdvancedEasingCurve createFluentAccelerate();
    static FluentAdvancedEasingCurve createFluentDecelerate();
    static FluentAdvancedEasingCurve createFluentStandard();
    static FluentAdvancedEasingCurve createFluentEmphasized();
    
    // Physics-based curves
    static FluentAdvancedEasingCurve createSpring(double stiffness = 300.0, double damping = 30.0);
    static FluentAdvancedEasingCurve createBounce(double bounciness = 0.8);
    static FluentAdvancedEasingCurve createGravity(double gravity = 9.81);
    static FluentAdvancedEasingCurve createFriction(double friction = 0.7);
    
    // Cultural curves
    static FluentAdvancedEasingCurve createWesternCurve();
    static FluentAdvancedEasingCurve createEasternCurve();
    static FluentAdvancedEasingCurve createMinimalCurve();
    static FluentAdvancedEasingCurve createExpressiveCurve();
    
    // Accessibility curves
    static FluentAdvancedEasingCurve createReducedMotionCurve();
    static FluentAdvancedEasingCurve createEnhancedAccessibilityCurve();
    static FluentAdvancedEasingCurve createHighContrastCurve();
    
    // Custom curves
    static FluentAdvancedEasingCurve createBezier(const QPointF& cp1, const QPointF& cp2);
    static FluentAdvancedEasingCurve createParametric(std::function<double(double)> function);
    static FluentAdvancedEasingCurve createFromPoints(const QList<QPointF>& points);
    
    // Curve combinations
    static FluentAdvancedEasingCurve combineCurves(const FluentAdvancedEasingCurve& curve1, 
                                                  const FluentAdvancedEasingCurve& curve2, 
                                                  double weight = 0.5);
    static FluentAdvancedEasingCurve sequenceCurves(const QList<FluentAdvancedEasingCurve>& curves);
    
    // Curve analysis
    static double calculateCurveComplexity(const FluentAdvancedEasingCurve& curve);
    static bool isCurveAccessible(const FluentAdvancedEasingCurve& curve);
    static QList<QPointF> sampleCurve(const FluentAdvancedEasingCurve& curve, int samples = 100);
};

// Easing utility functions
namespace FluentEasingUtils {
    // Curve analysis
    double calculateCurveLength(const FluentAdvancedEasingCurve& curve, int samples = 1000);
    double calculateCurveComplexity(const FluentAdvancedEasingCurve& curve);
    QPointF calculateCurveDerivative(const FluentAdvancedEasingCurve& curve, double t);
    double calculateCurveVelocity(const FluentAdvancedEasingCurve& curve, double t);
    
    // Curve transformation
    FluentAdvancedEasingCurve scaleCurve(const FluentAdvancedEasingCurve& curve, double factor);
    FluentAdvancedEasingCurve offsetCurve(const FluentAdvancedEasingCurve& curve, double offset);
    FluentAdvancedEasingCurve reverseCurve(const FluentAdvancedEasingCurve& curve);
    FluentAdvancedEasingCurve mirrorCurve(const FluentAdvancedEasingCurve& curve);
    
    // Cultural adaptation
    FluentAdvancedEasingCurve adaptCurveForCulture(const FluentAdvancedEasingCurve& curve, const QLocale& locale);
    double getCulturalSpeedFactor(const QLocale& locale);
    double getCulturalIntensityFactor(const QLocale& locale);
    
    // Accessibility adaptation
    FluentAdvancedEasingCurve adaptCurveForAccessibility(const FluentAdvancedEasingCurve& curve, bool reducedMotion);
    bool isCurveAccessibilityCompliant(const FluentAdvancedEasingCurve& curve);
    FluentAdvancedEasingCurve simplifyForAccessibility(const FluentAdvancedEasingCurve& curve);
    
    // Performance optimization
    FluentAdvancedEasingCurve optimizeCurveForPerformance(const FluentAdvancedEasingCurve& curve);
    int calculateOptimalCacheSize(const FluentAdvancedEasingCurve& curve);
    bool shouldUseCaching(const FluentAdvancedEasingCurve& curve);
    
    // Interpolation utilities
    template<typename T>
    T lerp(const T& a, const T& b, double t);
    
    template<typename T>
    T smoothstep(const T& a, const T& b, double t);
    
    template<typename T>
    T smootherstep(const T& a, const T& b, double t);
    
    // Specialized interpolation
    QColor lerpColor(const QColor& a, const QColor& b, double t, FluentAdvancedInterpolator<QColor>::ColorSpace colorSpace = FluentAdvancedInterpolator<QColor>::ColorSpace::RGB);
    QPointF lerpPoint(const QPointF& a, const QPointF& b, double t);
    QTransform lerpTransform(const QTransform& a, const QTransform& b, double t);
}

} // namespace FluentQt::Animation
