// examples/ElaWidgetToolsOptimizationDemo.cpp
// Comprehensive demo showcasing ElaWidgetTools-inspired optimizations

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QTimer>
#include <QVBoxLayout>

// FluentQt Core with ElaWidgetTools enhancements
#include "FluentQt/Core/FluentComponent.h"
#include "FluentQt/Core/FluentErrorBoundary.h"
#include "FluentQt/Styling/FluentTheme.h"

// Enhanced Animation System
#include "FluentQt/Animation/FluentAnimator.h"

// Enhanced Components
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentEnhancedScrollBar.h"
#include "FluentQt/Components/FluentLoadingIndicator.h"

using namespace FluentQt;

class ElaOptimizationDemo : public QMainWindow {
    Q_OBJECT

public:
    explicit ElaOptimizationDemo(QWidget* parent = nullptr);

private slots:
    void demonstrateAnimations();
    void demonstrateLoadingStates();
    void demonstrateErrorBoundary();
    void demonstrateSmoothScrolling();
    void toggleTheme();
    void simulateLoadingError();

private:
    void setupUI();
    void setupAnimationDemo();
    void setupLoadingDemo();
    void setupErrorBoundaryDemo();
    void setupScrollingDemo();

    // UI Components
    QWidget* m_centralWidget{nullptr};
    QVBoxLayout* m_mainLayout{nullptr};

    // Animation Demo
    QGroupBox* m_animationGroup{nullptr};
    Components::FluentButton* m_animateButton{nullptr};
    Components::FluentCard* m_animationCard{nullptr};

    // Loading Demo
    QGroupBox* m_loadingGroup{nullptr};
    Components::FluentLoadingIndicator* m_loadingIndicator{nullptr};
    Core::FluentErrorBoundary* m_errorBoundary{nullptr};
    QPushButton* m_startLoadingButton{nullptr};
    QPushButton* m_errorButton{nullptr};

    // Scrolling Demo
    QGroupBox* m_scrollingGroup{nullptr};
    Components::FluentEnhancedScrollBar* m_enhancedScrollBar{nullptr};
    QScrollArea* m_scrollArea{nullptr};

    // Theme controls
    QPushButton* m_themeToggle{nullptr};

    // Animation objects
    std::unique_ptr<Animation::FluentAnimator> m_animator;
    QTimer* m_loadingTimer{nullptr};
};

ElaOptimizationDemo::ElaOptimizationDemo(QWidget* parent)
    : QMainWindow(parent),
      m_animator(std::make_unique<Animation::FluentAnimator>(this)) {
    setWindowTitle("ElaWidgetTools Optimization Demo - FluentQt Enhanced");
    setMinimumSize(1000, 700);

    setupUI();

    // Initialize loading timer
    m_loadingTimer = new QTimer(this);
    m_loadingTimer->setSingleShot(true);
    connect(m_loadingTimer, &QTimer::timeout, [this]() {
        m_loadingIndicator->stop();
        m_errorBoundary->clearError();
    });
}

void ElaOptimizationDemo::setupUI() {
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    auto* titleLabel =
        new QLabel("ElaWidgetTools-Inspired FluentQt Optimizations", this);
    titleLabel->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: #0078d4; margin-bottom: "
        "10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(titleLabel);

    // Theme toggle
    m_themeToggle = new QPushButton("Toggle Dark/Light Theme", this);
    connect(m_themeToggle, &QPushButton::clicked, this,
            &ElaOptimizationDemo::toggleTheme);
    m_mainLayout->addWidget(m_themeToggle);

    // Create demo sections
    setupAnimationDemo();
    setupLoadingDemo();
    setupErrorBoundaryDemo();
    setupScrollingDemo();

    m_mainLayout->addStretch();
}

void ElaOptimizationDemo::setupAnimationDemo() {
    m_animationGroup = new QGroupBox("Enhanced Animation System", this);
    auto* layout = new QVBoxLayout(m_animationGroup);

    auto* description = new QLabel(
        "ElaWidgetTools-inspired smooth animations with proper easing curves "
        "and timing:",
        this);
    description->setWordWrap(true);
    layout->addWidget(description);

    // Animation controls
    auto* controlsLayout = new QHBoxLayout();
    m_animateButton =
        new Components::FluentButton("Demonstrate Animations", this);
    connect(m_animateButton, &Components::FluentButton::clicked, this,
            &ElaOptimizationDemo::demonstrateAnimations);
    controlsLayout->addWidget(m_animateButton);

    // Animation target
    m_animationCard = new Components::FluentCard(this);
    m_animationCard->setTitle("Animation Target");
    m_animationCard->setDescription(
        "Watch smooth ElaWidgetTools-style animations");
    m_animationCard->setFixedSize(300, 150);

    // Enable smooth transitions on the card
    m_animationCard->setSmoothTransitions(true);
    m_animationCard->setTransitionDuration(300);

    controlsLayout->addWidget(m_animationCard);
    layout->addLayout(controlsLayout);

    m_mainLayout->addWidget(m_animationGroup);
}

void ElaOptimizationDemo::setupLoadingDemo() {
    m_loadingGroup = new QGroupBox("Enhanced Loading System", this);
    auto* layout = new QVBoxLayout(m_loadingGroup);

    auto* description = new QLabel(
        "Improved loading indicators with timeout handling and error "
        "boundaries:",
        this);
    description->setWordWrap(true);
    layout->addWidget(description);

    // Loading controls
    auto* controlsLayout = new QHBoxLayout();
    m_startLoadingButton = new QPushButton("Start Loading", this);
    connect(m_startLoadingButton, &QPushButton::clicked, this,
            &ElaOptimizationDemo::demonstrateLoadingStates);
    controlsLayout->addWidget(m_startLoadingButton);

    m_errorButton = new QPushButton("Simulate Error", this);
    connect(m_errorButton, &QPushButton::clicked, this,
            &ElaOptimizationDemo::simulateLoadingError);
    controlsLayout->addWidget(m_errorButton);

    // Loading indicator with enhanced timing
    m_loadingIndicator = new Components::FluentLoadingIndicator(this);
    m_loadingIndicator->setLoadingType(Components::FluentLoadingType::Spinner);
    m_loadingIndicator->setText("Loading with enhanced performance...");
    m_loadingIndicator->setLoadingTimeout(5000);  // 5 second timeout

    controlsLayout->addWidget(m_loadingIndicator);
    layout->addLayout(controlsLayout);

    m_mainLayout->addWidget(m_loadingGroup);
}

void ElaOptimizationDemo::setupErrorBoundaryDemo() {
    // Error boundary wraps the loading demo
    m_errorBoundary = new Core::FluentErrorBoundary(this);
    m_errorBoundary->setShowRetryButton(true);
    m_errorBoundary->setLoadingTimeout(10000);  // 10 second timeout

    // Set up error boundary with loading indicator
    m_loadingIndicator->setErrorBoundary(m_errorBoundary);

    connect(m_errorBoundary, &Core::FluentErrorBoundary::retryRequested,
            [this]() { demonstrateLoadingStates(); });
}

void ElaOptimizationDemo::setupScrollingDemo() {
    m_scrollingGroup = new QGroupBox("Enhanced Smooth Scrolling", this);
    auto* layout = new QVBoxLayout(m_scrollingGroup);

    auto* description = new QLabel(
        "ElaWidgetTools-inspired smooth scrolling with expand-on-hover "
        "effects:",
        this);
    description->setWordWrap(true);
    layout->addWidget(description);

    // Create scroll area with enhanced scroll bar
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setFixedHeight(200);

    // Create content for scrolling
    auto* scrollContent = new QWidget();
    auto* contentLayout = new QVBoxLayout(scrollContent);

    for (int i = 0; i < 20; ++i) {
        auto* item = new QLabel(
            QString("Scroll Item %1 - Enhanced smooth scrolling demo")
                .arg(i + 1));
        item->setStyleSheet(
            "padding: 10px; border: 1px solid #ccc; margin: 2px;");
        contentLayout->addWidget(item);
    }

    m_scrollArea->setWidget(scrollContent);

    // Replace default scroll bar with enhanced version
    m_enhancedScrollBar =
        new Components::FluentEnhancedScrollBar(Qt::Vertical, this);
    m_enhancedScrollBar->setSmoothScrolling(true);
    m_enhancedScrollBar->setExpandOnHover(true);
    m_enhancedScrollBar->setSpeedLimit(15.0);

    m_scrollArea->setVerticalScrollBar(m_enhancedScrollBar);

    layout->addWidget(m_scrollArea);
    m_mainLayout->addWidget(m_scrollingGroup);
}

void ElaOptimizationDemo::demonstrateAnimations() {
    // Demonstrate various ElaWidgetTools-inspired animations

    // Smooth scale animation
    auto scaleAnimation = Animation::FluentAnimator::scaleIn(
        m_animationCard,
        {.duration = 400ms, .easing = Animation::FluentEasing::EaseOutCubic});

    // Smooth fade animation
    auto fadeAnimation = Animation::FluentAnimator::fadeOut(
        m_animationCard,
        {.duration = 300ms, .easing = Animation::FluentEasing::EaseInOutSine});

    // Chain animations
    connect(scaleAnimation.get(), &QPropertyAnimation::finished,
            [this, fadePtr = fadeAnimation.get()]() { fadePtr->start(); });

    connect(fadeAnimation.get(), &QPropertyAnimation::finished, [this]() {
        // Fade back in
        auto fadeIn = Animation::FluentAnimator::fadeIn(
            m_animationCard, {.duration = 300ms,
                              .easing = Animation::FluentEasing::EaseOutCubic});
        fadeIn->start();
    });

    scaleAnimation->start();
}

void ElaOptimizationDemo::demonstrateLoadingStates() {
    m_errorBoundary->clearError();
    m_loadingIndicator->start();

    // Simulate loading completion after 3 seconds
    m_loadingTimer->start(3000);
}

void ElaOptimizationDemo::simulateLoadingError() {
    m_loadingIndicator->stop();
    m_errorBoundary->catchError(
        "Simulated network error occurred. Click retry to try again.",
        Core::FluentErrorBoundary::ErrorType::NetworkError);
}

void ElaOptimizationDemo::demonstrateSmoothScrolling() {
    // Demonstrate smooth scrolling
    if (m_enhancedScrollBar) {
        m_enhancedScrollBar->smoothScrollTo(m_enhancedScrollBar->maximum() / 2,
                                            500);
    }
}

void ElaOptimizationDemo::toggleTheme() {
    auto& theme = Styling::FluentTheme::instance();
    auto newMode = (theme.mode() == Styling::FluentThemeMode::Light)
                       ? Styling::FluentThemeMode::Dark
                       : Styling::FluentThemeMode::Light;
    theme.setMode(newMode);
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize FluentQt theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::Light);

    ElaOptimizationDemo demo;
    demo.show();

    return app.exec();
}

#include "ElaWidgetToolsOptimizationDemo.moc"
