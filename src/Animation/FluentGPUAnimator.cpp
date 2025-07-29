// src/Animation/FluentGPUAnimator.cpp
#include "FluentQt/Animation/FluentGPUAnimator.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

namespace FluentQt::Animation {

// FluentShaderProgram implementation
FluentShaderProgram::FluentShaderProgram()
    : m_program(std::make_unique<QOpenGLShaderProgram>())
{
}

FluentShaderProgram::~FluentShaderProgram() = default;

bool FluentShaderProgram::loadFromFiles(const QString& vertexPath, const QString& fragmentPath) {
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexPath)) {
        qWarning() << "Failed to load vertex shader:" << vertexPath;
        return false;
    }
    
    if (!m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentPath)) {
        qWarning() << "Failed to load fragment shader:" << fragmentPath;
        return false;
    }
    
    if (!m_program->link()) {
        qWarning() << "Failed to link shader program:" << m_program->log();
        return false;
    }
    
    return true;
}

bool FluentShaderProgram::loadFromSource(const QString& vertexSource, const QString& fragmentSource) {
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource)) {
        qWarning() << "Failed to compile vertex shader:" << m_program->log();
        return false;
    }
    
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSource)) {
        qWarning() << "Failed to compile fragment shader:" << m_program->log();
        return false;
    }
    
    if (!m_program->link()) {
        qWarning() << "Failed to link shader program:" << m_program->log();
        return false;
    }
    
    return true;
}

void FluentShaderProgram::bind() {
    if (m_program) {
        m_program->bind();
    }
}

void FluentShaderProgram::release() {
    if (m_program) {
        m_program->release();
    }
}

void FluentShaderProgram::setUniform(const QString& name, float value) {
    if (m_program) {
        m_program->setUniformValue(name.toUtf8().constData(), value);
    }
}

void FluentShaderProgram::setUniform(const QString& name, const QVector3D& value) {
    if (m_program) {
        m_program->setUniformValue(name.toUtf8().constData(), value);
    }
}

void FluentShaderProgram::setUniform(const QString& name, const QMatrix4x4& value) {
    if (m_program) {
        m_program->setUniformValue(name.toUtf8().constData(), value);
    }
}

void FluentShaderProgram::setUniform(const QString& name, QOpenGLTexture* texture, int unit) {
    if (m_program && texture) {
        texture->bind(unit);
        m_program->setUniformValue(name.toUtf8().constData(), unit);
    }
}

// FluentGPURenderer implementation
FluentGPURenderer::FluentGPURenderer(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_animationTimer(new QTimer(this))
    , m_performanceTimer(new QTimer(this))
{
    // Set OpenGL format for high performance
    QSurfaceFormat format;
    format.setVersion(4, 0);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4); // 4x MSAA
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSwapInterval(1); // VSync enabled by default
    setFormat(format);
    
    // Setup timers
    m_animationTimer->setInterval(16); // 60 FPS default
    connect(m_animationTimer, &QTimer::timeout, this, &FluentGPURenderer::updateAnimations);
    
    m_performanceTimer->setInterval(1000); // Check performance every second
    connect(m_performanceTimer, &QTimer::timeout, this, &FluentGPURenderer::checkPerformance);
    
    m_animationTimer->start();
    m_performanceTimer->start();
}

FluentGPURenderer::~FluentGPURenderer() {
    makeCurrent();
    
    // Cleanup OpenGL resources
    for (auto& widget : m_activeAnimations) {
        delete widget.texture;
    }
    
    for (auto& effect : m_shaderEffects) {
        delete effect.program;
    }
    
    doneCurrent();
}

void FluentGPURenderer::setGPUConfig(const FluentGPUAnimationConfig& config) {
    m_config = config;
    
    // Update refresh rate
    setTargetRefreshRate(config.targetRefreshRate);
    
    // Update quality scale
    m_currentQualityScale = config.qualityScale;
    
    // Update VSync
    QSurfaceFormat format = this->format();
    format.setSwapInterval(config.enableVSync ? 1 : 0);
    setFormat(format);
    
    update();
}

QList<FluentGPUCapability> FluentGPURenderer::detectCapabilities() {
    QList<FluentGPUCapability> capabilities;
    
    QOpenGLContext* context = QOpenGLContext::currentContext();
    if (!context) {
        return capabilities;
    }
    
    QOpenGLFunctions* functions = context->functions();
    if (!functions) {
        return capabilities;
    }
    
    // Check OpenGL version
    QPair<int, int> version = context->format().version();
    
    if (version >= qMakePair(3, 3)) {
        capabilities.append(FluentGPUCapability::OpenGL_3_3);
    }
    if (version >= qMakePair(4, 0)) {
        capabilities.append(FluentGPUCapability::OpenGL_4_0);
    }
    if (version >= qMakePair(4, 5)) {
        capabilities.append(FluentGPUCapability::OpenGL_4_5);
    }
    
    // TODO: Add Vulkan, DirectX, Metal detection
    
    return capabilities;
}

bool FluentGPURenderer::isCapabilitySupported(FluentGPUCapability capability) {
    auto capabilities = detectCapabilities();
    return capabilities.contains(capability);
}

FluentRefreshRate FluentGPURenderer::detectRefreshRate() {
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) {
        return FluentRefreshRate::Standard_60Hz;
    }
    
    qreal refreshRate = screen->refreshRate();
    
    if (refreshRate >= 240.0) {
        return FluentRefreshRate::Professional_240Hz;
    } else if (refreshRate >= 144.0) {
        return FluentRefreshRate::Gaming_144Hz;
    } else if (refreshRate >= 120.0) {
        return FluentRefreshRate::Ultra_120Hz;
    } else if (refreshRate >= 90.0) {
        return FluentRefreshRate::High_90Hz;
    } else {
        return FluentRefreshRate::Standard_60Hz;
    }
}

void FluentGPURenderer::addAnimatedWidget(QWidget* widget, FluentShaderEffect effect) {
    if (!widget) return;
    
    makeCurrent();
    
    AnimatedWidget animWidget;
    animWidget.widget = widget;
    animWidget.effect = effect;
    animWidget.texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    animWidget.opacity = 1.0f;
    animWidget.needsUpdate = true;
    
    m_activeAnimations[widget] = animWidget;
    
    // Load shader effect if not already loaded
    if (!m_shaderEffects.contains(effect)) {
        loadShaderEffect(effect);
    }
    
    doneCurrent();
}

void FluentGPURenderer::removeAnimatedWidget(QWidget* widget) {
    auto it = m_activeAnimations.find(widget);
    if (it != m_activeAnimations.end()) {
        makeCurrent();
        delete it->texture;
        m_activeAnimations.erase(it);
        doneCurrent();
    }
}

void FluentGPURenderer::clearAnimatedWidgets() {
    makeCurrent();
    
    for (auto& widget : m_activeAnimations) {
        delete widget.texture;
    }
    m_activeAnimations.clear();
    
    doneCurrent();
}

void FluentGPURenderer::setTargetRefreshRate(FluentRefreshRate rate) {
    int interval;
    
    switch (rate) {
        case FluentRefreshRate::Professional_240Hz:
            interval = 4; // ~240 FPS
            break;
        case FluentRefreshRate::Gaming_144Hz:
            interval = 7; // ~144 FPS
            break;
        case FluentRefreshRate::Ultra_120Hz:
            interval = 8; // ~120 FPS
            break;
        case FluentRefreshRate::High_90Hz:
            interval = 11; // ~90 FPS
            break;
        case FluentRefreshRate::Standard_60Hz:
            interval = 16; // ~60 FPS
            break;
        case FluentRefreshRate::Adaptive:
        default:
            interval = 16; // Start with 60 FPS, adjust based on performance
            m_adaptiveRefreshRate = true;
            break;
    }
    
    m_targetInterval = interval;
    m_animationTimer->setInterval(interval);
}

void FluentGPURenderer::enableAdaptiveRefreshRate(bool enable) {
    m_adaptiveRefreshRate = enable;
    if (enable) {
        setTargetRefreshRate(FluentRefreshRate::Adaptive);
    }
}

void FluentGPURenderer::initializeGL() {
    initializeOpenGLFunctions();
    
    // Enable features
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    
    if (m_config.enableMultisampling) {
        glEnable(GL_MULTISAMPLE);
    }
    
    // Initialize shaders and buffers
    initializeShaders();
    initializeBuffers();
    
    // Setup projection matrix
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.ortho(0, width(), height(), 0, -1, 1);
    
    m_viewMatrix.setToIdentity();
}

void FluentGPURenderer::resizeGL(int w, int h) {
    glViewport(0, 0, w * m_currentQualityScale, h * m_currentQualityScale);
    
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.ortho(0, w, h, 0, -1, 1);
}

void FluentGPURenderer::paintGL() {
    FLUENT_PROFILE("FluentGPURenderer::paintGL");
    
    m_frameTimer.start();
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render all animated widgets
    for (auto& widget : m_activeAnimations) {
        if (widget.widget->isVisible()) {
            renderWidget(widget);
        }
    }
    
    // Update performance metrics
    m_frameTime = m_frameTimer.elapsed();
    m_frameCount++;
}

void FluentGPURenderer::updateAnimations() {
    // Update widget textures if needed
    for (auto& widget : m_activeAnimations) {
        if (widget.needsUpdate) {
            renderToTexture(widget.widget, widget.texture);
            widget.needsUpdate = false;
        }
    }
    
    update(); // Trigger paintGL
}

void FluentGPURenderer::checkPerformance() {
    // Calculate FPS
    m_currentFPS = m_frameCount;
    m_frameCount = 0;
    
    emit fpsChanged(m_currentFPS);
    emit frameTimeChanged(m_frameTime);
    
    // Adaptive quality adjustment
    if (m_adaptiveRefreshRate) {
        optimizeForRefreshRate();
    }
    
    if (m_qualityAdjustmentEnabled) {
        adjustQualityBasedOnPerformance();
    }
}

} // namespace FluentQt::Animation
