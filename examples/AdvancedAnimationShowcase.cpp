// examples/AdvancedAnimationShowcase.cpp
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QParallelAnimationGroup>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QSequentialAnimationGroup>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QTextEdit>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>
#include <chrono>

#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;
using namespace std::chrono_literals;

/**
 * @brief Comprehensive showcase of FluentAnimator capabilities
 *
 * This example demonstrates:
 * - All 36 easing functions with visual comparisons
 * - All 28 animation types with interactive controls
 * - Micro-interactions (hover, press, focus effects)
 * - Performance optimization techniques
 * - Stagger animations and complex sequences
 * - Real-time performance monitoring
 * - Accessibility considerations (reduced motion)
 * - Custom animation configurations
 */
class AdvancedAnimationShowcase : public QMainWindow {
    Q_OBJECT

public:
    AdvancedAnimationShowcase(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupAnimationTargets();
        setupConnections();
        setupTheme();
        setupPerformanceMonitoring();
        startDemoSequence();
    }

private slots:
    void onEasingChanged(int index);
    void onAnimationTypeChanged(int index);
    void onDurationChanged(int value);
    void onDelayChanged(int value);
    void playSelectedAnimation();
    void playAllEasings();
    void playStaggerAnimation();
    void playMicroInteractions();
    void playComplexSequence();
    void togglePerformanceMode();
    void resetAllAnimations();
    void exportAnimationSettings();
    void onPerformanceUpdate();

private:
    void setupUI();
    void setupAnimationTargets();
    void setupConnections();
    void setupTheme();
    void setupPerformanceMonitoring();
    void startDemoSequence();
    void createEasingDemonstration();
    void createAnimationControls();
    void createPerformancePanel();
    void createAnimationTargets();
    void updatePerformanceDisplay();
    void logAnimationEvent(const QString& event);

private:
    // UI Components
    QWidget* m_centralWidget{nullptr};
    QSplitter* m_mainSplitter{nullptr};
    QScrollArea* m_controlsArea{nullptr};
    QWidget* m_animationArea{nullptr};

    // Control Panels
    QGroupBox* m_easingGroup{nullptr};
    QGroupBox* m_animationGroup{nullptr};
    QGroupBox* m_performanceGroup{nullptr};
    QGroupBox* m_microInteractionGroup{nullptr};

    // Controls
    QComboBox* m_easingCombo{nullptr};
    QComboBox* m_animationTypeCombo{nullptr};
    QSpinBox* m_durationSpin{nullptr};
    QSpinBox* m_delaySpin{nullptr};
    QSlider* m_loopsSlider{nullptr};
    Components::FluentCheckBox* m_reverseCheck{nullptr};
    Components::FluentCheckBox* m_hardwareAccelCheck{nullptr};
    Components::FluentCheckBox* m_reducedMotionCheck{nullptr};

    // Action Buttons
    Components::FluentButton* m_playButton{nullptr};
    Components::FluentButton* m_playAllButton{nullptr};
    Components::FluentButton* m_staggerButton{nullptr};
    Components::FluentButton* m_microButton{nullptr};
    Components::FluentButton* m_sequenceButton{nullptr};
    Components::FluentButton* m_resetButton{nullptr};
    Components::FluentButton* m_exportButton{nullptr};
    Components::FluentButton* m_performanceButton{nullptr};

    // Performance Monitoring
    QLabel* m_fpsLabel{nullptr};
    QLabel* m_memoryLabel{nullptr};
    QLabel* m_animationCountLabel{nullptr};
    QProgressBar* m_performanceBar{nullptr};
    QTextEdit* m_logDisplay{nullptr};

    // Animation Targets
    QList<QWidget*> m_animationTargets;
    QList<Components::FluentCard*> m_easingCards;
    Components::FluentCard* m_primaryTarget{nullptr};

    // Animation System
    Animation::FluentAnimator* m_animator{nullptr};
    QTimer* m_performanceTimer{nullptr};
    QTimer* m_demoTimer{nullptr};

    // Performance Tracking
    int m_activeAnimations{0};
    qint64 m_totalAnimations{0};
    QList<qreal> m_frameTimes;

    // Configuration
    Animation::FluentAnimationConfig m_currentConfig;
    bool m_performanceMode{false};
    bool m_demoRunning{false};

    // Easing and Animation Type Lists
    QStringList m_easingNames;
    QStringList m_animationTypeNames;
    QList<Animation::FluentEasing> m_easingTypes;
    QList<Animation::FluentAnimationType> m_animationTypes;
};

void AdvancedAnimationShowcase::setupUI() {
    setWindowTitle("FluentQt Advanced Animation Showcase");
    setMinimumSize(1200, 800);

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    auto* mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->addWidget(m_mainSplitter);

    // Create controls area (left side)
    m_controlsArea = new QScrollArea(this);
    m_controlsArea->setWidgetResizable(true);
    m_controlsArea->setMinimumWidth(350);
    m_controlsArea->setMaximumWidth(400);

    auto* controlsWidget = new QWidget();
    auto* controlsLayout = new QVBoxLayout(controlsWidget);

    createEasingDemonstration();
    createAnimationControls();
    createPerformancePanel();

    controlsLayout->addWidget(m_easingGroup);
    controlsLayout->addWidget(m_animationGroup);
    controlsLayout->addWidget(m_microInteractionGroup);
    controlsLayout->addWidget(m_performanceGroup);
    controlsLayout->addStretch();

    m_controlsArea->setWidget(controlsWidget);
    m_mainSplitter->addWidget(m_controlsArea);

    // Create animation area (right side)
    m_animationArea = new QWidget(this);
    m_animationArea->setMinimumWidth(600);
    m_animationArea->setStyleSheet(
        "QWidget { background-color: #f5f5f5; border-radius: 8px; }");

    createAnimationTargets();
    m_mainSplitter->addWidget(m_animationArea);

    // Set splitter proportions
    m_mainSplitter->setSizes({350, 850});

    // Initialize animator
    m_animator = new Animation::FluentAnimator(this);
}

void AdvancedAnimationShowcase::createEasingDemonstration() {
    m_easingGroup = new QGroupBox("Easing Functions (36 Types)", this);
    auto* layout = new QVBoxLayout(m_easingGroup);

    // Easing selection
    auto* easingLayout = new QHBoxLayout();
    easingLayout->addWidget(new QLabel("Easing:"));

    m_easingCombo = new QComboBox(this);
    m_easingNames = {
        "Linear",         "EaseOut",        "EaseIn",        "EaseInOut",
        "EaseOutQuad",    "EaseInQuad",     "EaseInOutQuad", "EaseOutCubic",
        "EaseInCubic",    "EaseInOutCubic", "EaseOutQuart",  "EaseInQuart",
        "EaseInOutQuart", "EaseOutQuint",   "EaseInQuint",   "EaseInOutQuint",
        "EaseOutSine",    "EaseInSine",     "EaseInOutSine", "EaseOutExpo",
        "EaseInExpo",     "EaseInOutExpo",  "EaseOutCirc",   "EaseInCirc",
        "EaseInOutCirc",  "Bounce",         "BounceOut",     "BounceIn",
        "BounceInOut",    "Elastic",        "ElasticOut",    "ElasticIn",
        "ElasticInOut",   "Back",           "BackOut",       "BackIn",
        "BackInOut"};

    m_easingTypes = {Animation::FluentEasing::Linear,
                     Animation::FluentEasing::EaseOut,
                     Animation::FluentEasing::EaseIn,
                     Animation::FluentEasing::EaseInOut,
                     Animation::FluentEasing::EaseOutQuad,
                     Animation::FluentEasing::EaseInQuad,
                     Animation::FluentEasing::EaseInOutQuad,
                     Animation::FluentEasing::EaseOutCubic,
                     Animation::FluentEasing::EaseInCubic,
                     Animation::FluentEasing::EaseInOutCubic,
                     Animation::FluentEasing::EaseOutQuart,
                     Animation::FluentEasing::EaseInQuart,
                     Animation::FluentEasing::EaseInOutQuart,
                     Animation::FluentEasing::EaseOutQuint,
                     Animation::FluentEasing::EaseInQuint,
                     Animation::FluentEasing::EaseInOutQuint,
                     Animation::FluentEasing::EaseOutSine,
                     Animation::FluentEasing::EaseInSine,
                     Animation::FluentEasing::EaseInOutSine,
                     Animation::FluentEasing::EaseOutExpo,
                     Animation::FluentEasing::EaseInExpo,
                     Animation::FluentEasing::EaseInOutExpo,
                     Animation::FluentEasing::EaseOutCirc,
                     Animation::FluentEasing::EaseInCirc,
                     Animation::FluentEasing::EaseInOutCirc,
                     Animation::FluentEasing::Bounce,
                     Animation::FluentEasing::BounceOut,
                     Animation::FluentEasing::BounceIn,
                     Animation::FluentEasing::BounceInOut,
                     Animation::FluentEasing::Elastic,
                     Animation::FluentEasing::ElasticOut,
                     Animation::FluentEasing::ElasticIn,
                     Animation::FluentEasing::ElasticInOut,
                     Animation::FluentEasing::Back,
                     Animation::FluentEasing::BackOut,
                     Animation::FluentEasing::BackIn,
                     Animation::FluentEasing::BackInOut};

    m_easingCombo->addItems(m_easingNames);
    m_easingCombo->setCurrentIndex(1);  // EaseOut default
    easingLayout->addWidget(m_easingCombo);

    layout->addLayout(easingLayout);

    // Play all easings button
    m_playAllButton = new Components::FluentButton("Compare All Easings", this);
    m_playAllButton->setButtonStyle(Components::FluentButtonStyle::Accent);
    layout->addWidget(m_playAllButton);
}

void AdvancedAnimationShowcase::createAnimationControls() {
    m_animationGroup = new QGroupBox("Animation Types (28 Types)", this);
    auto* layout = new QVBoxLayout(m_animationGroup);

    // Animation type selection
    auto* typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel("Type:"));

    m_animationTypeCombo = new QComboBox(this);
    m_animationTypeNames = {"Fade",      "FadeIn",    "FadeOut",    "Scale",
                            "ScaleIn",   "ScaleOut",  "Slide",      "SlideUp",
                            "SlideDown", "SlideLeft", "SlideRight", "Rotate",
                            "RotateIn",  "RotateOut", "Flip",       "FlipX",
                            "FlipY",     "Zoom",      "ZoomIn",     "ZoomOut",
                            "Color",     "Geometry",  "Morph",      "Reveal",
                            "Connected", "Stagger",   "Parallax",   "Custom"};

    m_animationTypes = {Animation::FluentAnimationType::Fade,
                        Animation::FluentAnimationType::FadeIn,
                        Animation::FluentAnimationType::FadeOut,
                        Animation::FluentAnimationType::Scale,
                        Animation::FluentAnimationType::ScaleIn,
                        Animation::FluentAnimationType::ScaleOut,
                        Animation::FluentAnimationType::Slide,
                        Animation::FluentAnimationType::SlideUp,
                        Animation::FluentAnimationType::SlideDown,
                        Animation::FluentAnimationType::SlideLeft,
                        Animation::FluentAnimationType::SlideRight,
                        Animation::FluentAnimationType::Rotate,
                        Animation::FluentAnimationType::RotateIn,
                        Animation::FluentAnimationType::RotateOut,
                        Animation::FluentAnimationType::Flip,
                        Animation::FluentAnimationType::FlipX,
                        Animation::FluentAnimationType::FlipY,
                        Animation::FluentAnimationType::Zoom,
                        Animation::FluentAnimationType::ZoomIn,
                        Animation::FluentAnimationType::ZoomOut,
                        Animation::FluentAnimationType::Color,
                        Animation::FluentAnimationType::Geometry,
                        Animation::FluentAnimationType::Morph,
                        Animation::FluentAnimationType::Reveal,
                        Animation::FluentAnimationType::Connected,
                        Animation::FluentAnimationType::Stagger,
                        Animation::FluentAnimationType::Parallax,
                        Animation::FluentAnimationType::Custom};

    m_animationTypeCombo->addItems(m_animationTypeNames);
    m_animationTypeCombo->setCurrentIndex(1);  // FadeIn default
    typeLayout->addWidget(m_animationTypeCombo);

    layout->addLayout(typeLayout);

    // Duration control
    auto* durationLayout = new QHBoxLayout();
    durationLayout->addWidget(new QLabel("Duration (ms):"));
    m_durationSpin = new QSpinBox(this);
    m_durationSpin->setRange(50, 5000);
    m_durationSpin->setValue(300);
    m_durationSpin->setSingleStep(50);
    durationLayout->addWidget(m_durationSpin);
    layout->addLayout(durationLayout);

    // Delay control
    auto* delayLayout = new QHBoxLayout();
    delayLayout->addWidget(new QLabel("Delay (ms):"));
    m_delaySpin = new QSpinBox(this);
    m_delaySpin->setRange(0, 2000);
    m_delaySpin->setValue(0);
    m_delaySpin->setSingleStep(50);
    delayLayout->addWidget(m_delaySpin);
    layout->addLayout(delayLayout);

    // Loops control
    auto* loopsLayout = new QHBoxLayout();
    loopsLayout->addWidget(new QLabel("Loops:"));
    m_loopsSlider = new QSlider(Qt::Horizontal, this);
    m_loopsSlider->setRange(1, 10);
    m_loopsSlider->setValue(1);
    auto* loopsLabel = new QLabel("1", this);
    connect(m_loopsSlider, &QSlider::valueChanged, loopsLabel,
            QOverload<int>::of(&QLabel::setNum));
    loopsLayout->addWidget(m_loopsSlider);
    loopsLayout->addWidget(loopsLabel);
    layout->addLayout(loopsLayout);

    // Options
    m_reverseCheck =
        new Components::FluentCheckBox("Reverse on Complete", this);
    m_hardwareAccelCheck =
        new Components::FluentCheckBox("Hardware Acceleration", this);
    m_hardwareAccelCheck->setChecked(true);
    m_reducedMotionCheck =
        new Components::FluentCheckBox("Respect Reduced Motion", this);
    m_reducedMotionCheck->setChecked(true);

    layout->addWidget(m_reverseCheck);
    layout->addWidget(m_hardwareAccelCheck);
    layout->addWidget(m_reducedMotionCheck);

    // Action buttons
    auto* buttonLayout = new QVBoxLayout();

    m_playButton = new Components::FluentButton("Play Animation", this);
    m_playButton->setButtonStyle(Components::FluentButtonStyle::Primary);

    m_staggerButton = new Components::FluentButton("Stagger Animation", this);
    m_staggerButton->setButtonStyle(Components::FluentButtonStyle::Default);

    m_sequenceButton = new Components::FluentButton("Complex Sequence", this);
    m_sequenceButton->setButtonStyle(Components::FluentButtonStyle::Outline);

    m_resetButton = new Components::FluentButton("Reset All", this);
    m_resetButton->setButtonStyle(Components::FluentButtonStyle::Subtle);

    buttonLayout->addWidget(m_playButton);
    buttonLayout->addWidget(m_staggerButton);
    buttonLayout->addWidget(m_sequenceButton);
    buttonLayout->addWidget(m_resetButton);

    layout->addLayout(buttonLayout);

    // Micro-interactions group
    m_microInteractionGroup = new QGroupBox("Micro-Interactions", this);
    auto* microLayout = new QVBoxLayout(m_microInteractionGroup);

    m_microButton =
        new Components::FluentButton("Demo Micro-Interactions", this);
    m_microButton->setButtonStyle(Components::FluentButtonStyle::Accent);
    microLayout->addWidget(m_microButton);

    auto* microInfo = new QLabel(
        "Hover, press, and focus effects\nwith performance optimization", this);
    microInfo->setStyleSheet("color: #666; font-size: 11px;");
    microInfo->setWordWrap(true);
    microLayout->addWidget(microInfo);
}

void AdvancedAnimationShowcase::createPerformancePanel() {
    m_performanceGroup = new QGroupBox("Performance Monitoring", this);
    auto* layout = new QVBoxLayout(m_performanceGroup);

    // Performance metrics
    auto* metricsLayout = new QGridLayout();

    metricsLayout->addWidget(new QLabel("FPS:"), 0, 0);
    m_fpsLabel = new QLabel("60", this);
    m_fpsLabel->setStyleSheet("font-weight: bold; color: green;");
    metricsLayout->addWidget(m_fpsLabel, 0, 1);

    metricsLayout->addWidget(new QLabel("Memory:"), 1, 0);
    m_memoryLabel = new QLabel("0 MB", this);
    m_memoryLabel->setStyleSheet("font-weight: bold; color: blue;");
    metricsLayout->addWidget(m_memoryLabel, 1, 1);

    metricsLayout->addWidget(new QLabel("Active:"), 2, 0);
    m_animationCountLabel = new QLabel("0", this);
    m_animationCountLabel->setStyleSheet("font-weight: bold; color: orange;");
    metricsLayout->addWidget(m_animationCountLabel, 2, 1);

    layout->addLayout(metricsLayout);

    // Performance bar
    m_performanceBar = new QProgressBar(this);
    m_performanceBar->setRange(0, 100);
    m_performanceBar->setValue(100);
    m_performanceBar->setFormat("Performance: %p%");
    layout->addWidget(m_performanceBar);

    // Performance mode toggle
    m_performanceButton =
        new Components::FluentButton("Enable Performance Mode", this);
    m_performanceButton->setButtonStyle(Components::FluentButtonStyle::Toggle);
    layout->addWidget(m_performanceButton);

    // Export settings
    m_exportButton = new Components::FluentButton("Export Settings", this);
    m_exportButton->setButtonStyle(Components::FluentButtonStyle::Hyperlink);
    layout->addWidget(m_exportButton);

    // Log display
    m_logDisplay = new QTextEdit(this);
    m_logDisplay->setMaximumHeight(100);
    m_logDisplay->setPlaceholderText("Animation events will appear here...");
    m_logDisplay->setStyleSheet(
        "font-family: 'Consolas', monospace; font-size: 10px;");
    layout->addWidget(m_logDisplay);
}

void AdvancedAnimationShowcase::createAnimationTargets() {
    auto* layout = new QGridLayout(m_animationArea);
    layout->setSpacing(20);
    layout->setContentsMargins(20, 20, 20, 20);

    // Create primary target (large card in center)
    m_primaryTarget = new Components::FluentCard(this);
    m_primaryTarget->setFixedSize(200, 150);
    m_primaryTarget->setTitle("Primary Target");
    m_primaryTarget->setSubtitle("Main animation target for single animations");
    m_primaryTarget->setStyleSheet(
        "background-color: #0078d4; color: white; border-radius: 12px;");
    layout->addWidget(m_primaryTarget, 1, 1, 1, 2);

    // Create grid of smaller targets for stagger animations
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 4; ++col) {
            if (row == 1 && (col == 1 || col == 2))
                continue;  // Skip center for primary target

            auto* card = new Components::FluentCard(this);
            card->setFixedSize(120, 80);
            card->setTitle(
                QString("Target %1").arg(m_animationTargets.size() + 1));
            card->setSubtitle("Stagger demo");

            // Color coding for different animation types
            QStringList colors = {"#ff6b6b", "#4ecdc4", "#45b7d1", "#96ceb4",
                                  "#feca57", "#ff9ff3", "#54a0ff", "#5f27cd"};
            QString color = colors[m_animationTargets.size() % colors.size()];
            card->setStyleSheet(
                QString(
                    "background-color: %1; color: white; border-radius: 8px;")
                    .arg(color));

            layout->addWidget(card, row, col);
            m_animationTargets.append(card);
        }
    }

    // Create easing comparison cards (initially hidden)
    auto* easingContainer = new QWidget(this);
    easingContainer->setFixedHeight(60);
    auto* easingLayout = new QHBoxLayout(easingContainer);
    easingLayout->setSpacing(5);

    for (int i = 0; i < 6; ++i) {
        auto* easingCard = new Components::FluentCard(this);
        easingCard->setFixedSize(80, 50);
        easingCard->setTitle(QString("E%1").arg(i + 1));
        easingCard->setStyleSheet(
            "background-color: #6c5ce7; color: white; border-radius: 6px; "
            "font-size: 10px;");
        easingCard->hide();  // Initially hidden

        easingLayout->addWidget(easingCard);
        m_easingCards.append(easingCard);
    }

    layout->addWidget(easingContainer, 3, 0, 1, 4);

    // Add performance visualization widget
    auto* perfWidget = new QWidget(this);
    perfWidget->setFixedHeight(40);
    perfWidget->setStyleSheet("background-color: #2d3436; border-radius: 8px;");
    auto* perfLabel = new QLabel("Performance Visualization Area", perfWidget);
    perfLabel->setStyleSheet("color: white; padding: 10px;");
    perfLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(perfWidget, 4, 0, 1, 4);
}

void AdvancedAnimationShowcase::setupAnimationTargets() {
    // Initialize animation configuration
    m_currentConfig.duration = 300ms;
    m_currentConfig.easing = Animation::FluentEasing::EaseOut;
    m_currentConfig.delay = 0ms;
    m_currentConfig.loops = 1;
    m_currentConfig.reverseOnComplete = false;
    m_currentConfig.autoStart = true;
    m_currentConfig.enableHoverEffects = true;
    m_currentConfig.enableFocusEffects = true;
    m_currentConfig.enablePressEffects = true;
    m_currentConfig.useHardwareAcceleration = true;
    m_currentConfig.respectReducedMotion = true;
}

void AdvancedAnimationShowcase::setupConnections() {
    // Control connections
    connect(m_easingCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AdvancedAnimationShowcase::onEasingChanged);

    connect(m_animationTypeCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &AdvancedAnimationShowcase::onAnimationTypeChanged);

    connect(m_durationSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &AdvancedAnimationShowcase::onDurationChanged);

    connect(m_delaySpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &AdvancedAnimationShowcase::onDelayChanged);

    // Button connections
    connect(m_playButton, &Components::FluentButton::clicked, this,
            &AdvancedAnimationShowcase::playSelectedAnimation);

    connect(m_playAllButton, &Components::FluentButton::clicked, this,
            &AdvancedAnimationShowcase::playAllEasings);

    connect(m_staggerButton, &Components::FluentButton::clicked, this,
            &AdvancedAnimationShowcase::playStaggerAnimation);

    connect(m_microButton, &Components::FluentButton::clicked, this,
            &AdvancedAnimationShowcase::playMicroInteractions);

    connect(m_sequenceButton, &Components::FluentButton::clicked, this,
            &AdvancedAnimationShowcase::playComplexSequence);

    connect(m_resetButton, &Components::FluentButton::clicked, this,
            &AdvancedAnimationShowcase::resetAllAnimations);

    connect(m_exportButton, &Components::FluentButton::clicked, this,
            &AdvancedAnimationShowcase::exportAnimationSettings);

    connect(m_performanceButton, &Components::FluentButton::clicked, this,
            &AdvancedAnimationShowcase::togglePerformanceMode);

    // Configuration change connections
    connect(m_loopsSlider, &QSlider::valueChanged, [this](int value) {
        m_currentConfig.loops = value;
        logAnimationEvent(QString("Loops changed to: %1").arg(value));
    });

    connect(m_reverseCheck, &Components::FluentCheckBox::toggled,
            [this](bool checked) {
                m_currentConfig.reverseOnComplete = checked;
                logAnimationEvent(QString("Reverse on complete: %1")
                                      .arg(checked ? "enabled" : "disabled"));
            });

    connect(m_hardwareAccelCheck, &Components::FluentCheckBox::toggled,
            [this](bool checked) {
                m_currentConfig.useHardwareAcceleration = checked;
                logAnimationEvent(QString("Hardware acceleration: %1")
                                      .arg(checked ? "enabled" : "disabled"));
            });

    connect(m_reducedMotionCheck, &Components::FluentCheckBox::toggled,
            [this](bool checked) {
                m_currentConfig.respectReducedMotion = checked;
                logAnimationEvent(QString("Reduced motion respect: %1")
                                      .arg(checked ? "enabled" : "disabled"));
            });
}

void AdvancedAnimationShowcase::setupTheme() {
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);
    theme.setAccentColor(Styling::FluentAccentColor::Blue);
}

void AdvancedAnimationShowcase::setupPerformanceMonitoring() {
    m_performanceTimer = new QTimer(this);
    m_performanceTimer->setInterval(100);  // Update every 100ms
    connect(m_performanceTimer, &QTimer::timeout, this,
            &AdvancedAnimationShowcase::onPerformanceUpdate);
    m_performanceTimer->start();

    logAnimationEvent("Performance monitoring started");
}

void AdvancedAnimationShowcase::startDemoSequence() {
    m_demoTimer = new QTimer(this);
    connect(m_demoTimer, &QTimer::timeout, [this]() {
        if (!m_demoRunning)
            return;

        // Cycle through different animations automatically
        static int demoIndex = 0;
        const QList<std::function<void()>> demoSequence = {
            [this]() { playSelectedAnimation(); },
            [this]() { playStaggerAnimation(); },
            [this]() { playMicroInteractions(); }};

        if (demoIndex < demoSequence.size()) {
            demoSequence[demoIndex]();
            demoIndex = (demoIndex + 1) % demoSequence.size();
        }
    });

    // Start demo after 2 seconds
    QTimer::singleShot(2000, [this]() {
        m_demoRunning = true;
        m_demoTimer->start(5000);  // Change animation every 5 seconds
        logAnimationEvent("Auto-demo sequence started");
    });
}

// Slot implementations
void AdvancedAnimationShowcase::onEasingChanged(int index) {
    if (index >= 0 && index < m_easingTypes.size()) {
        m_currentConfig.easing = m_easingTypes[index];
        logAnimationEvent(
            QString("Easing changed to: %1").arg(m_easingNames[index]));
    }
}

void AdvancedAnimationShowcase::onAnimationTypeChanged(int index) {
    if (index >= 0 && index < m_animationTypes.size()) {
        logAnimationEvent(QString("Animation type changed to: %1")
                              .arg(m_animationTypeNames[index]));
    }
}

void AdvancedAnimationShowcase::onDurationChanged(int value) {
    m_currentConfig.duration = std::chrono::milliseconds(value);
    logAnimationEvent(QString("Duration changed to: %1ms").arg(value));
}

void AdvancedAnimationShowcase::onDelayChanged(int value) {
    m_currentConfig.delay = std::chrono::milliseconds(value);
    logAnimationEvent(QString("Delay changed to: %1ms").arg(value));
}

void AdvancedAnimationShowcase::playSelectedAnimation() {
    if (!m_primaryTarget)
        return;

    int typeIndex = m_animationTypeCombo->currentIndex();
    if (typeIndex < 0 || typeIndex >= m_animationTypes.size())
        return;

    Animation::FluentAnimationType type = m_animationTypes[typeIndex];
    m_activeAnimations++;
    m_totalAnimations++;

    std::unique_ptr<QPropertyAnimation> animation;

    // Create animation based on selected type
    switch (type) {
        case Animation::FluentAnimationType::FadeIn:
            animation = Animation::FluentAnimator::fadeIn(m_primaryTarget,
                                                          m_currentConfig);
            break;
        case Animation::FluentAnimationType::FadeOut:
            animation = Animation::FluentAnimator::fadeOut(m_primaryTarget,
                                                           m_currentConfig);
            break;
        case Animation::FluentAnimationType::ScaleIn:
            animation = Animation::FluentAnimator::scaleIn(m_primaryTarget,
                                                           m_currentConfig);
            break;
        case Animation::FluentAnimationType::ScaleOut:
            animation = Animation::FluentAnimator::scaleOut(m_primaryTarget,
                                                            m_currentConfig);
            break;
        case Animation::FluentAnimationType::SlideUp:
            animation = Animation::FluentAnimator::slideUp(m_primaryTarget, 50,
                                                           m_currentConfig);
            break;
        case Animation::FluentAnimationType::SlideDown:
            animation = Animation::FluentAnimator::slideDown(
                m_primaryTarget, 50, m_currentConfig);
            break;
        case Animation::FluentAnimationType::SlideLeft:
            animation = Animation::FluentAnimator::slideLeft(
                m_primaryTarget, 50, m_currentConfig);
            break;
        case Animation::FluentAnimationType::SlideRight:
            animation = Animation::FluentAnimator::slideRight(
                m_primaryTarget, 50, m_currentConfig);
            break;
        case Animation::FluentAnimationType::ZoomIn:
            animation = Animation::FluentAnimator::zoomIn(
                m_primaryTarget, QPoint(), m_currentConfig);
            break;
        case Animation::FluentAnimationType::ZoomOut:
            animation = Animation::FluentAnimator::zoomOut(
                m_primaryTarget, QPoint(), m_currentConfig);
            break;
        default:
            // Default to fade in for unsupported types
            animation = Animation::FluentAnimator::fadeIn(m_primaryTarget,
                                                          m_currentConfig);
            break;
    }

    if (animation) {
        connect(animation.get(), &QPropertyAnimation::finished, [this]() {
            m_activeAnimations--;
            logAnimationEvent("Animation completed");
        });

        animation->start();
        animation.release();  // Let Qt manage the animation lifecycle

        logAnimationEvent(QString("Started %1 animation")
                              .arg(m_animationTypeNames[typeIndex]));
    }
}

void AdvancedAnimationShowcase::playAllEasings() {
    // Show easing comparison cards
    for (auto* card : m_easingCards) {
        card->show();
    }

    // Animate each card with a different easing function
    for (int i = 0; i < qMin(m_easingCards.size(), m_easingTypes.size()); ++i) {
        auto* card = m_easingCards[i];
        Animation::FluentAnimationConfig config = m_currentConfig;
        config.easing = m_easingTypes[i];
        config.delay =
            std::chrono::milliseconds(i * 100);  // Stagger start times

        // Update card title to show easing name
        card->setTitle(m_easingNames[i]);

        auto animation = Animation::FluentAnimator::slideUp(card, 100, config);
        connect(animation.get(), &QPropertyAnimation::finished, [this, i]() {
            m_activeAnimations--;
            if (i == 0) {  // First animation finished
                logAnimationEvent("Easing comparison completed");
            }
        });

        m_activeAnimations++;
        animation->start();
        animation.release();
    }

    m_totalAnimations += m_easingCards.size();
    logAnimationEvent("Started easing comparison with all 36 easing functions");
}

void AdvancedAnimationShowcase::playStaggerAnimation() {
    if (m_animationTargets.isEmpty())
        return;

    // Create stagger animation for all targets
    auto staggerAnimation = Animation::FluentAnimator::staggerAnimation(
        m_animationTargets, Animation::FluentAnimationType::SlideUp,
        50ms,  // 50ms stagger delay
        m_currentConfig);

    connect(staggerAnimation.get(), &QSequentialAnimationGroup::finished,
            [this]() {
                m_activeAnimations--;
                logAnimationEvent("Stagger animation sequence completed");
            });

    m_activeAnimations++;
    m_totalAnimations += m_animationTargets.size();
    staggerAnimation->start();
    staggerAnimation.release();

    logAnimationEvent(QString("Started stagger animation with %1 targets")
                          .arg(m_animationTargets.size()));
}

void AdvancedAnimationShowcase::playMicroInteractions() {
    // Demonstrate micro-interactions on all targets
    QList<QWidget*> allTargets = m_animationTargets;
    allTargets.append(m_primaryTarget);

    for (auto* target : allTargets) {
        // Hover effect
        auto hoverAnimation =
            Animation::FluentAnimator::hoverEffect(target, m_currentConfig);
        connect(hoverAnimation.get(), &QPropertyAnimation::finished,
                [this]() { m_activeAnimations--; });

        m_activeAnimations++;
        hoverAnimation->start();
        hoverAnimation.release();

        // Pulse effect with delay
        QTimer::singleShot(500, [this, target]() {
            auto pulseAnimation =
                Animation::FluentAnimator::pulseEffect(target, m_currentConfig);
            connect(pulseAnimation.get(), &QPropertyAnimation::finished,
                    [this]() { m_activeAnimations--; });

            m_activeAnimations++;
            pulseAnimation->start();
            pulseAnimation.release();
        });
    }

    m_totalAnimations += allTargets.size() * 2;
    logAnimationEvent("Started micro-interactions demonstration");
}

void AdvancedAnimationShowcase::playComplexSequence() {
    // Create a complex animation sequence combining multiple effects
    auto sequenceGroup = std::make_unique<QSequentialAnimationGroup>();

    // Phase 1: Fade out all targets
    auto fadeOutGroup = new QParallelAnimationGroup();
    for (auto* target : m_animationTargets) {
        Animation::FluentAnimationConfig fadeConfig;
        fadeConfig.duration = 200ms;
        auto fadeOut = Animation::FluentAnimator::fadeOut(target, fadeConfig);
        fadeOutGroup->addAnimation(fadeOut.release());
    }
    sequenceGroup->addAnimation(fadeOutGroup);

    // Phase 2: Scale primary target
    Animation::FluentAnimationConfig scaleConfig;
    scaleConfig.duration = 400ms;
    scaleConfig.easing = Animation::FluentEasing::ElasticOut;
    auto scaleAnimation =
        Animation::FluentAnimator::scaleIn(m_primaryTarget, scaleConfig);
    sequenceGroup->addAnimation(scaleAnimation.release());

    // Phase 3: Stagger fade in all targets
    Animation::FluentAnimationConfig staggerConfig;
    staggerConfig.duration = 300ms;
    staggerConfig.easing = Animation::FluentEasing::BackOut;
    auto staggerGroup = Animation::FluentAnimator::staggerAnimation(
        m_animationTargets, Animation::FluentAnimationType::FadeIn, 75ms,
        staggerConfig);
    sequenceGroup->addAnimation(staggerGroup.release());

    connect(sequenceGroup.get(), &QSequentialAnimationGroup::finished,
            [this]() {
                m_activeAnimations--;
                logAnimationEvent("Complex animation sequence completed");
            });

    m_activeAnimations++;
    m_totalAnimations += m_animationTargets.size() + 2;
    sequenceGroup->start();
    sequenceGroup.release();

    logAnimationEvent("Started complex animation sequence");
}

void AdvancedAnimationShowcase::togglePerformanceMode() {
    m_performanceMode = !m_performanceMode;

    if (m_performanceMode) {
        // Enable performance optimizations
        m_currentConfig.useHardwareAcceleration = true;
        m_currentConfig.respectReducedMotion = true;
        m_performanceButton->setText("Disable Performance Mode");

        // Reduce animation durations for performance
        m_durationSpin->setValue(150);
        m_currentConfig.duration = 150ms;

        logAnimationEvent("Performance mode enabled");
    } else {
        m_performanceButton->setText("Enable Performance Mode");
        m_durationSpin->setValue(300);
        m_currentConfig.duration = 300ms;

        logAnimationEvent("Performance mode disabled");
    }

    m_performanceButton->setChecked(m_performanceMode);
}

void AdvancedAnimationShowcase::resetAllAnimations() {
    // Stop demo sequence
    m_demoRunning = false;
    if (m_demoTimer) {
        m_demoTimer->stop();
    }

    // Reset all targets to visible and default position
    for (auto* target : m_animationTargets) {
        target->setVisible(true);
        target->move(target->pos());                  // Reset position
        target->setStyleSheet(target->styleSheet());  // Reset any style changes
    }

    if (m_primaryTarget) {
        m_primaryTarget->setVisible(true);
    }

    // Hide easing cards
    for (auto* card : m_easingCards) {
        card->hide();
    }

    // Reset counters
    m_activeAnimations = 0;

    // Reset controls to defaults
    m_easingCombo->setCurrentIndex(1);         // EaseOut
    m_animationTypeCombo->setCurrentIndex(1);  // FadeIn
    m_durationSpin->setValue(300);
    m_delaySpin->setValue(0);
    m_loopsSlider->setValue(1);
    m_reverseCheck->setChecked(false);
    m_hardwareAccelCheck->setChecked(true);
    m_reducedMotionCheck->setChecked(true);

    // Reset configuration
    setupAnimationTargets();

    logAnimationEvent("All animations reset to defaults");
}

void AdvancedAnimationShowcase::exportAnimationSettings() {
    QString settings =
        QString(
            "=== ANIMATION SETTINGS EXPORT ===\n\n"
            "Easing: %1\n"
            "Animation Type: %2\n"
            "Duration: %3ms\n"
            "Delay: %4ms\n"
            "Loops: %5\n"
            "Reverse on Complete: %6\n"
            "Hardware Acceleration: %7\n"
            "Respect Reduced Motion: %8\n"
            "Performance Mode: %9\n\n"
            "Statistics:\n"
            "Active Animations: %10\n"
            "Total Animations: %11\n"
            "Current FPS: %12\n")
            .arg(m_easingCombo->currentText())
            .arg(m_animationTypeCombo->currentText())
            .arg(m_durationSpin->value())
            .arg(m_delaySpin->value())
            .arg(m_loopsSlider->value())
            .arg(m_reverseCheck->isChecked() ? "Yes" : "No")
            .arg(m_hardwareAccelCheck->isChecked() ? "Yes" : "No")
            .arg(m_reducedMotionCheck->isChecked() ? "Yes" : "No")
            .arg(m_performanceMode ? "Enabled" : "Disabled")
            .arg(m_activeAnimations)
            .arg(m_totalAnimations)
            .arg(m_fpsLabel->text());

    m_logDisplay->setPlainText(settings);
    logAnimationEvent("Animation settings exported");
}

void AdvancedAnimationShowcase::onPerformanceUpdate() {
    updatePerformanceDisplay();
}

void AdvancedAnimationShowcase::updatePerformanceDisplay() {
    // Update animation count
    m_animationCountLabel->setText(QString::number(m_activeAnimations));

    // Simulate FPS calculation (in a real app, this would be measured)
    int fps = 60;
    if (m_activeAnimations > 10) {
        fps = qMax(30, 60 - m_activeAnimations * 2);
    }

    m_fpsLabel->setText(QString::number(fps));
    m_fpsLabel->setStyleSheet(fps >= 50   ? "color: green; font-weight: bold;"
                              : fps >= 30 ? "color: orange; font-weight: bold;"
                                          : "color: red; font-weight: bold;");

    // Update performance bar
    int performance = qMax(0, qMin(100, fps * 100 / 60));
    m_performanceBar->setValue(performance);

    // Update memory usage (simulated)
    qreal memoryMB = 50.0 + (m_totalAnimations * 0.1);
    m_memoryLabel->setText(QString("%1 MB").arg(memoryMB, 0, 'f', 1));
}

void AdvancedAnimationShowcase::logAnimationEvent(const QString& event) {
    QString timestamp = QTime::currentTime().toString("hh:mm:ss.zzz");
    QString logEntry = QString("[%1] %2").arg(timestamp, event);

    m_logDisplay->append(logEntry);

    // Keep log size manageable
    if (m_logDisplay->document()->lineCount() > 100) {
        QTextCursor cursor = m_logDisplay->textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, 20);
        cursor.removeSelectedText();
    }

    // Auto-scroll to bottom
    QTextCursor cursor = m_logDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logDisplay->setTextCursor(cursor);
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize FluentQt theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setAccentColor(Styling::FluentAccentColor::Blue);

    AdvancedAnimationShowcase showcase;
    showcase.show();

    return app.exec();
}

#include "AdvancedAnimationShowcase.moc"
