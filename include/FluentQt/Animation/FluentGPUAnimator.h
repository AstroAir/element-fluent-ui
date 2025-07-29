// include/FluentQt/Animation/FluentGPUAnimator.h
#ifndef FLUENTGPUANIMATOR_H
#define FLUENTGPUANIMATOR_H

#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QVector3D>
#include <QTimer>
#include <QElapsedTimer>
#include <memory>

#include "FluentAnimator.h"

namespace FluentQt::Animation {

// GPU acceleration capabilities
enum class FluentGPUCapability {
    OpenGL_3_3,
    OpenGL_4_0,
    OpenGL_4_5,
    Vulkan_1_0,
    Vulkan_1_1,
    Vulkan_1_2,
    DirectX_11,
    DirectX_12,
    Metal_2_0
};

// Shader-based effect types
enum class FluentShaderEffect {
    Blur,
    Glow,
    Ripple,
    Dissolve,
    Morph,
    Particle,
    Distortion,
    ChromaticAberration,
    Bloom,
    MotionBlur
};

// High refresh rate support
enum class FluentRefreshRate {
    Standard_60Hz,
    High_90Hz,
    Ultra_120Hz,
    Gaming_144Hz,
    Professional_240Hz,
    Adaptive
};

// GPU animation configuration
struct FluentGPUAnimationConfig {
    FluentGPUCapability requiredCapability{FluentGPUCapability::OpenGL_3_3};
    FluentRefreshRate targetRefreshRate{FluentRefreshRate::Adaptive};
    bool enableVSync{true};
    bool enableMultisampling{true};
    int multisampleCount{4};
    bool enableHDR{false};
    bool enableComputeShaders{false};
    float qualityScale{1.0f}; // 0.5 = half resolution, 2.0 = double resolution
    
    // Performance settings
    bool enableFramePacing{true};
    bool enableGPUProfiling{false};
    int maxConcurrentAnimations{16};
    
    // Default high-performance configuration
    static FluentGPUAnimationConfig highPerformance() {
        FluentGPUAnimationConfig config;
        config.requiredCapability = FluentGPUCapability::OpenGL_4_0;
        config.targetRefreshRate = FluentRefreshRate::Ultra_120Hz;
        config.enableComputeShaders = true;
        config.qualityScale = 1.0f;
        config.maxConcurrentAnimations = 32;
        return config;
    }
    
    // Mobile-optimized configuration
    static FluentGPUAnimationConfig mobile() {
        FluentGPUAnimationConfig config;
        config.requiredCapability = FluentGPUCapability::OpenGL_3_3;
        config.targetRefreshRate = FluentRefreshRate::Standard_60Hz;
        config.enableMultisampling = false;
        config.qualityScale = 0.75f;
        config.maxConcurrentAnimations = 8;
        return config;
    }
};

// Shader program wrapper
class FluentShaderProgram {
public:
    FluentShaderProgram();
    ~FluentShaderProgram();
    
    bool loadFromFiles(const QString& vertexPath, const QString& fragmentPath);
    bool loadFromSource(const QString& vertexSource, const QString& fragmentSource);
    
    void bind();
    void release();
    
    void setUniform(const QString& name, float value);
    void setUniform(const QString& name, const QVector3D& value);
    void setUniform(const QString& name, const QMatrix4x4& value);
    void setUniform(const QString& name, QOpenGLTexture* texture, int unit = 0);
    
    bool isValid() const { return m_program && m_program->isLinked(); }
    
private:
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    QMap<QString, int> m_uniformLocations;
};

// GPU-accelerated animation renderer
class FluentGPURenderer : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
    
public:
    explicit FluentGPURenderer(QWidget* parent = nullptr);
    ~FluentGPURenderer() override;
    
    // Configuration
    void setGPUConfig(const FluentGPUAnimationConfig& config);
    FluentGPUAnimationConfig gpuConfig() const { return m_config; }
    
    // Capability detection
    static QList<FluentGPUCapability> detectCapabilities();
    static bool isCapabilitySupported(FluentGPUCapability capability);
    static FluentRefreshRate detectRefreshRate();
    
    // Animation management
    void addAnimatedWidget(QWidget* widget, FluentShaderEffect effect = FluentShaderEffect::Blur);
    void removeAnimatedWidget(QWidget* widget);
    void clearAnimatedWidgets();
    
    // Shader effects
    void applyShaderEffect(QWidget* widget, FluentShaderEffect effect, const QVariantMap& parameters = {});
    void removeShaderEffect(QWidget* widget);
    
    // Performance monitoring
    float getCurrentFPS() const { return m_currentFPS; }
    float getFrameTime() const { return m_frameTime; }
    int getActiveAnimationCount() const { return m_activeAnimations.size(); }
    
    // High refresh rate support
    void setTargetRefreshRate(FluentRefreshRate rate);
    void enableAdaptiveRefreshRate(bool enable);

signals:
    void fpsChanged(float fps);
    void frameTimeChanged(float frameTime);
    void gpuMemoryUsageChanged(int megabytes);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    
    void timerEvent(QTimerEvent* event) override;

private slots:
    void updateAnimations();
    void checkPerformance();

private:
    struct AnimatedWidget {
        QWidget* widget;
        FluentShaderEffect effect;
        QVariantMap parameters;
        QOpenGLTexture* texture;
        QMatrix4x4 transform;
        float opacity;
        bool needsUpdate;
    };
    
    struct ShaderEffectData {
        FluentShaderProgram* program;
        QOpenGLBuffer vertexBuffer;
        QOpenGLVertexArrayObject vao;
        QVariantMap defaultParameters;
    };
    
    // Initialization
    void initializeShaders();
    void initializeBuffers();
    void setupFramebuffer();
    
    // Rendering
    void renderWidget(const AnimatedWidget& widget);
    void renderShaderEffect(const AnimatedWidget& widget, const ShaderEffectData& effectData);
    void renderToTexture(QWidget* widget, QOpenGLTexture* texture);
    
    // Shader management
    void loadShaderEffect(FluentShaderEffect effect);
    QString getShaderPath(FluentShaderEffect effect, bool isVertex) const;
    
    // Performance optimization
    void optimizeForRefreshRate();
    void adjustQualityBasedOnPerformance();
    void updateFramePacing();
    
    FluentGPUAnimationConfig m_config;
    QMap<QWidget*, AnimatedWidget> m_activeAnimations;
    QMap<FluentShaderEffect, ShaderEffectData> m_shaderEffects;
    
    // OpenGL resources
    QOpenGLBuffer m_quadBuffer;
    QOpenGLVertexArrayObject m_quadVAO;
    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewMatrix;
    
    // Performance monitoring
    QTimer* m_animationTimer;
    QTimer* m_performanceTimer;
    QElapsedTimer m_frameTimer;
    float m_currentFPS{60.0f};
    float m_frameTime{16.67f};
    int m_frameCount{0};
    
    // Refresh rate management
    int m_targetInterval{16}; // 60 FPS default
    bool m_adaptiveRefreshRate{true};
    
    // Quality scaling
    float m_currentQualityScale{1.0f};
    bool m_qualityAdjustmentEnabled{true};
};

// GPU-accelerated animator
class FluentGPUAnimator : public QObject {
    Q_OBJECT
    
public:
    explicit FluentGPUAnimator(QObject* parent = nullptr);
    ~FluentGPUAnimator() = default;
    
    // GPU renderer management
    void setRenderer(FluentGPURenderer* renderer);
    FluentGPURenderer* renderer() const { return m_renderer; }
    
    // High-performance animations
    void animateWithShader(QWidget* target, FluentShaderEffect effect, 
                          const FluentAnimationConfig& config = {},
                          const QVariantMap& shaderParams = {});
    
    void animateBlur(QWidget* target, float fromRadius, float toRadius,
                    const FluentAnimationConfig& config = {});
    
    void animateGlow(QWidget* target, const QColor& glowColor, float intensity,
                    const FluentAnimationConfig& config = {});
    
    void animateRipple(QWidget* target, const QPointF& center, float maxRadius,
                      const FluentAnimationConfig& config = {});
    
    void animateDissolve(QWidget* target, float dissolveAmount,
                        const FluentAnimationConfig& config = {});
    
    void animateParticles(QWidget* target, int particleCount, const QVector3D& gravity,
                         const FluentAnimationConfig& config = {});
    
    // Complex GPU-accelerated transitions
    void morphBetweenWidgets(QWidget* from, QWidget* to, 
                            const FluentAnimationConfig& config = {});
    
    void pageTransitionWithShader(QWidget* fromPage, QWidget* toPage,
                                 FluentShaderEffect effect,
                                 const FluentAnimationConfig& config = {});
    
    // Performance optimization
    void enableBatching(bool enable) { m_batchingEnabled = enable; }
    void setMaxConcurrentAnimations(int max) { m_maxConcurrentAnimations = max; }
    
    // Capability queries
    static bool isGPUAccelerationAvailable();
    static QStringList getSupportedShaderEffects();
    static FluentGPUCapability getMaxSupportedCapability();

signals:
    void animationStarted(QWidget* target, FluentShaderEffect effect);
    void animationFinished(QWidget* target, FluentShaderEffect effect);
    void performanceWarning(const QString& message);

private slots:
    void onAnimationFinished();
    void onPerformanceChanged();

private:
    FluentGPURenderer* m_renderer{nullptr};
    bool m_batchingEnabled{true};
    int m_maxConcurrentAnimations{16};
    
    struct GPUAnimation {
        QWidget* target;
        FluentShaderEffect effect;
        QVariantMap parameters;
        std::unique_ptr<QPropertyAnimation> animation;
        bool active;
    };
    
    QList<GPUAnimation> m_activeGPUAnimations;
    
    void startGPUAnimation(const GPUAnimation& animation);
    void stopGPUAnimation(QWidget* target);
    void updateShaderParameters(QWidget* target, const QVariantMap& params);
};

} // namespace FluentQt::Animation

#endif // FLUENTGPUANIMATOR_H
