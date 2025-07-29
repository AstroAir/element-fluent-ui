// src/Tools/FluentAnimationDesigner.cpp
#include "FluentQt/Tools/FluentAnimationDesigner.h"
#include "FluentQt/Core/FluentPerformance.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QClipboard>
#include <QHeaderView>
#include <QGraphicsProxyWidget>
#include <QGraphicsRectItem>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>

namespace FluentQt::Tools {

// FluentAnimationPreset implementation
QList<FluentAnimationPreset> FluentAnimationPreset::getBuiltInPresets() {
    QList<FluentAnimationPreset> presets;
    
    // Fluent Design Standard Presets
    FluentAnimationPreset fadeIn;
    fadeIn.name = "Fade In";
    fadeIn.description = "Standard fade in animation with Fluent timing";
    fadeIn.category = "Entrance";
    fadeIn.config.duration = std::chrono::milliseconds(300);
    fadeIn.config.easing = Animation::FluentEasing::EaseOutCubic;
    fadeIn.codeTemplate = R"(
auto animation = Animation::FluentAnimator::fadeIn(target);
animation->start();
)";
    presets.append(fadeIn);
    
    FluentAnimationPreset slideUp;
    slideUp.name = "Slide Up";
    slideUp.description = "Slide up entrance with spring physics";
    slideUp.category = "Entrance";
    slideUp.config.duration = std::chrono::milliseconds(400);
    slideUp.config.easing = Animation::FluentEasing::EaseOutBack;
    slideUp.codeTemplate = R"(
auto animation = Animation::FluentAnimator::slideUp(target, 30);
animation->start();
)";
    presets.append(slideUp);
    
    FluentAnimationPreset scaleIn;
    scaleIn.name = "Scale In";
    scaleIn.description = "Scale in with bounce effect";
    scaleIn.category = "Entrance";
    scaleIn.config.duration = std::chrono::milliseconds(500);
    scaleIn.config.easing = Animation::FluentEasing::EaseOutBack;
    scaleIn.codeTemplate = R"(
auto animation = Animation::FluentAnimator::scaleIn(target);
animation->start();
)";
    presets.append(scaleIn);
    
    FluentAnimationPreset hoverGlow;
    hoverGlow.name = "Hover Glow";
    hoverGlow.description = "Subtle glow effect for hover states";
    hoverGlow.category = "Interaction";
    hoverGlow.config.duration = std::chrono::milliseconds(150);
    hoverGlow.config.easing = Animation::FluentEasing::EaseOutQuad;
    hoverGlow.codeTemplate = R"(
auto animation = Animation::FluentAnimator::hoverEffect(target);
animation->start();
)";
    presets.append(hoverGlow);
    
    FluentAnimationPreset ripple;
    ripple.name = "Ripple Effect";
    ripple.description = "Material Design inspired ripple for buttons";
    ripple.category = "Interaction";
    ripple.config.duration = std::chrono::milliseconds(400);
    ripple.config.easing = Animation::FluentEasing::EaseOutCubic;
    ripple.codeTemplate = R"(
auto animation = Animation::FluentAnimator::rippleEffect(target, center);
animation->start();
)";
    presets.append(ripple);
    
    return presets;
}

FluentAnimationPreset FluentAnimationPreset::createCustomPreset(const QString& name, 
                                                               const Animation::FluentAnimationConfig& config) {
    FluentAnimationPreset preset;
    preset.name = name;
    preset.description = "Custom animation preset";
    preset.category = "Custom";
    preset.config = config;
    return preset;
}

// FluentAnimationDesigner implementation
FluentAnimationDesigner::FluentAnimationDesigner(QWidget* parent)
    : QMainWindow(parent)
    , m_previewTimer(new QTimer(this))
    , m_animator(std::make_unique<Animation::FluentAnimator>())
    , m_physicsAnimator(std::make_unique<Animation::FluentPhysicsAnimator>())
{
    setWindowTitle("Fluent Animation Designer");
    setMinimumSize(1200, 800);
    
    setupUI();
    setupMenuBar();
    setupToolBar();
    
    // Load built-in presets
    m_presets = FluentAnimationPreset::getBuiltInPresets();
    
    // Setup preview timer
    m_previewTimer->setInterval(16); // 60 FPS
    connect(m_previewTimer, &QTimer::timeout, this, &FluentAnimationDesigner::onPreviewUpdate);
    
    // Load default project
    newProject();
}

void FluentAnimationDesigner::setupUI() {
    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(m_mainSplitter);
    
    // Setup panels
    setupPropertyPanel();
    setupPreviewPanel();
    setupTimelinePanel();
    setupPresetPanel();
    setupCodePanel();
    
    // Create right splitter for preview and code
    m_rightSplitter = new QSplitter(Qt::Vertical);
    m_rightSplitter->addWidget(m_previewPanel);
    
    // Create tab widget for timeline, presets, and code
    QTabWidget* bottomTabs = new QTabWidget();
    bottomTabs->addTab(m_timelinePanel, "Timeline");
    bottomTabs->addTab(m_presetPanel, "Presets");
    bottomTabs->addTab(m_codeTabWidget, "Code");
    
    m_rightSplitter->addWidget(bottomTabs);
    
    // Add to main splitter
    m_mainSplitter->addWidget(m_propertyPanel);
    m_mainSplitter->addWidget(m_rightSplitter);
    
    // Set splitter proportions
    m_mainSplitter->setSizes({300, 900});
    m_rightSplitter->setSizes({400, 400});
}

void FluentAnimationDesigner::setupMenuBar() {
    QMenuBar* menuBar = this->menuBar();
    
    // File menu
    QMenu* fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("&New Project", this, &FluentAnimationDesigner::newProject, QKeySequence::New);
    fileMenu->addAction("&Open Project...", [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Project", "", "Animation Projects (*.fap)");
        if (!fileName.isEmpty()) {
            openProject(fileName);
        }
    }, QKeySequence::Open);
    fileMenu->addAction("&Save Project...", [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Project", "", "Animation Projects (*.fap)");
        if (!fileName.isEmpty()) {
            saveProject(fileName);
        }
    }, QKeySequence::Save);
    fileMenu->addSeparator();
    fileMenu->addAction("&Export Animation...", [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Export Animation", "", "C++ Files (*.cpp)");
        if (!fileName.isEmpty()) {
            exportAnimation("cpp");
        }
    });
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);
    
    // Edit menu
    QMenu* editMenu = menuBar->addMenu("&Edit");
    editMenu->addAction("&Add Layer", [this]() {
        addAnimationLayer("New Layer");
    });
    editMenu->addAction("&Remove Layer", [this]() {
        if (!m_currentLayer.isEmpty()) {
            removeAnimationLayer(m_currentLayer);
        }
    });
    
    // Animation menu
    QMenu* animationMenu = menuBar->addMenu("&Animation");
    animationMenu->addAction("&Play", this, &FluentAnimationDesigner::playAnimation, QKeySequence("Space"));
    animationMenu->addAction("&Pause", this, &FluentAnimationDesigner::pauseAnimation);
    animationMenu->addAction("&Stop", this, &FluentAnimationDesigner::stopAnimation, QKeySequence("Escape"));
    
    // Tools menu
    QMenu* toolsMenu = menuBar->addMenu("&Tools");
    toolsMenu->addAction("Animation &Profiler", [this]() {
        auto* profiler = new FluentAnimationProfiler();
        profiler->show();
    });
    toolsMenu->addAction("Animation &Cookbook", [this]() {
        auto* cookbook = new FluentAnimationCookbook();
        cookbook->show();
    });
    
    // Help menu
    QMenu* helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&About", [this]() {
        QMessageBox::about(this, "About Fluent Animation Designer",
                          "Fluent Animation Designer v1.0\n\n"
                          "A visual tool for creating and testing Fluent Design animations.\n"
                          "Built with FluentQt framework.");
    });
}

void FluentAnimationDesigner::setupToolBar() {
    QToolBar* toolBar = addToolBar("Main");
    
    // Playback controls
    m_playButton = new QPushButton("Play");
    m_pauseButton = new QPushButton("Pause");
    m_stopButton = new QPushButton("Stop");
    
    connect(m_playButton, &QPushButton::clicked, this, &FluentAnimationDesigner::playAnimation);
    connect(m_pauseButton, &QPushButton::clicked, this, &FluentAnimationDesigner::pauseAnimation);
    connect(m_stopButton, &QPushButton::clicked, this, &FluentAnimationDesigner::stopAnimation);
    
    toolBar->addWidget(m_playButton);
    toolBar->addWidget(m_pauseButton);
    toolBar->addWidget(m_stopButton);
    toolBar->addSeparator();
    
    // Timeline controls
    m_timelineSlider = new QSlider(Qt::Horizontal);
    m_timelineSlider->setRange(0, 1000);
    m_timelineSlider->setValue(0);
    m_timeLabel = new QLabel("0:00 / 1:00");
    
    connect(m_timelineSlider, &QSlider::valueChanged, [this](int value) {
        m_currentTime = (value / 1000.0) * m_totalDuration;
        updatePreview();
        
        int seconds = m_currentTime / 1000;
        int totalSeconds = m_totalDuration / 1000;
        m_timeLabel->setText(QString("%1:%2 / %3:%4")
                            .arg(seconds / 60)
                            .arg(seconds % 60, 2, 10, QChar('0'))
                            .arg(totalSeconds / 60)
                            .arg(totalSeconds % 60, 2, 10, QChar('0')));
    });
    
    toolBar->addWidget(new QLabel("Timeline:"));
    toolBar->addWidget(m_timelineSlider);
    toolBar->addWidget(m_timeLabel);
}

void FluentAnimationDesigner::newProject() {
    // Clear current project
    m_timelineItems.clear();
    m_currentLayer = "Default";
    m_currentTime = 0;
    m_totalDuration = 1000;
    
    // Add default layer
    addAnimationLayer("Default");
    
    // Update UI
    updatePropertyBrowser();
    updateTimeline();
    updatePreview();
    
    setWindowTitle("Fluent Animation Designer - New Project");
}

void FluentAnimationDesigner::playAnimation() {
    if (!m_previewTimer->isActive()) {
        m_previewTimer->start();
        m_playButton->setEnabled(false);
        m_pauseButton->setEnabled(true);
        m_stopButton->setEnabled(true);
    }
}

void FluentAnimationDesigner::pauseAnimation() {
    if (m_previewTimer->isActive()) {
        m_previewTimer->stop();
        m_playButton->setEnabled(true);
        m_pauseButton->setEnabled(false);
    }
}

void FluentAnimationDesigner::stopAnimation() {
    m_previewTimer->stop();
    m_currentTime = 0;
    m_timelineSlider->setValue(0);
    updatePreview();
    
    m_playButton->setEnabled(true);
    m_pauseButton->setEnabled(false);
    m_stopButton->setEnabled(false);
}

void FluentAnimationDesigner::onPreviewUpdate() {
    m_currentTime += 16; // 16ms per frame (60 FPS)
    
    if (m_currentTime >= m_totalDuration) {
        stopAnimation();
        return;
    }
    
    // Update timeline slider
    int sliderValue = (m_currentTime * 1000) / m_totalDuration;
    m_timelineSlider->setValue(sliderValue);
    
    updatePreview();
}

void FluentAnimationDesigner::generateCode() {
    QString cppCode = R"(
// Generated Fluent Animation Code
#include "FluentQt/Animation/FluentAnimator.h"

void animateWidget(QWidget* target) {
    using namespace FluentQt::Animation;
    
    FluentAnimationConfig config;
    config.duration = std::chrono::milliseconds(%1);
    config.easing = FluentEasing::%2;
    
    auto animation = FluentAnimator::%3(target, config);
    animation->start();
}
)";
    
    // Fill in the template with current settings
    QString easingName = "EaseOutCubic"; // Default
    QString animationType = "fadeIn"; // Default
    int duration = 300; // Default
    
    if (!m_timelineItems.isEmpty()) {
        const auto& item = m_timelineItems.first();
        duration = item.duration;
        // Convert easing enum to string
        easingName = "EaseOutCubic"; // Simplified for now
        animationType = item.property.contains("opacity") ? "fadeIn" : "slideUp";
    }
    
    QString finalCode = cppCode.arg(duration).arg(easingName).arg(animationType);
    
    m_cppCodeEdit->setPlainText(finalCode);
    
    emit codeGenerated(finalCode);
}

} // namespace FluentQt::Tools
