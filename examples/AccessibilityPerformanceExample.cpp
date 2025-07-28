// examples/AccessibilityPerformanceExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QSlider>
#include <QSpinBox>
#include <QProgressBar>
#include <QTimer>
#include <QElapsedTimer>
#include <QScrollArea>
#include <QSplitter>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QRandomGenerator>
#include <QDateTime>
#include <QAccessible>
#include <QScreen>
#include <QGuiApplication>
#include <QThread>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Accessibility/FluentAccessible.h"

using namespace FluentQt;

class AccessibilityPerformanceExample : public QMainWindow {
    Q_OBJECT

public:
    AccessibilityPerformanceExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupAccessibility();
        setupPerformanceMonitoring();
        setupConnections();
        setupTheme();
        initializeDemo();
    }

private slots:
    void toggleHighContrast();
    void adjustFontSize(int size);
    void toggleAnimations(bool enabled);
    void toggleScreenReaderMode(bool enabled);
    void announceToScreenReader();
    void onFrameRateChanged(double fps);
    void onPerformanceWarning(const QString& message);
    void runPerformanceTest();
    void updatePerformanceMetrics();
    void simulateHeavyOperation();
    void toggleResponsiveMode();
    void adjustAnimationSpeed(int speed);
    void resetAccessibilitySettings();

private:
    void setupUI();
    void setupAccessibility();
    void setupPerformanceMonitoring();
    void setupConnections();
    void setupTheme();
    void initializeDemo();
    
    // Content creation methods
    QWidget* createAccessibilityPanel();
    QWidget* createPerformancePanel();
    QWidget* createResponsivePanel();
    QWidget* createDemoContentPanel();
    
    // Helper methods
    void updateAccessibilityFeatures();
    void updatePerformanceDisplay();
    void createAccessibleContent();
    void simulateResponsiveBreakpoints();
    void profileOperation(const QString& operation, std::function<void()> func);

private:
    // Main layout components
    QSplitter* m_mainSplitter{nullptr};
    QScrollArea* m_scrollArea{nullptr};
    
    // Accessibility controls
    QGroupBox* m_accessibilityGroup{nullptr};
    Components::FluentCheckBox* m_highContrastCheck{nullptr};
    Components::FluentCheckBox* m_screenReaderCheck{nullptr};
    Components::FluentCheckBox* m_animationsCheck{nullptr};
    QSlider* m_fontSizeSlider{nullptr};
    QLabel* m_fontSizeLabel{nullptr};
    Components::FluentButton* m_announceButton{nullptr};
    Components::FluentButton* m_resetAccessibilityButton{nullptr};
    
    // Performance monitoring
    QGroupBox* m_performanceGroup{nullptr};
    QLabel* m_fpsLabel{nullptr};
    QLabel* m_memoryLabel{nullptr};
    QLabel* m_profileLabel{nullptr};
    QProgressBar* m_performanceBar{nullptr};
    Components::FluentButton* m_performanceTestButton{nullptr};
    Components::FluentButton* m_heavyOperationButton{nullptr};
    QListWidget* m_performanceLog{nullptr};
    
    // Responsive design controls
    QGroupBox* m_responsiveGroup{nullptr};
    Components::FluentComboBox* m_breakpointCombo{nullptr};
    QSlider* m_animationSpeedSlider{nullptr};
    QLabel* m_animationSpeedLabel{nullptr};
    Components::FluentCheckBox* m_responsiveCheck{nullptr};
    
    // Demo content area
    QWidget* m_demoContent{nullptr};
    QGridLayout* m_demoLayout{nullptr};
    QList<Components::FluentCard*> m_demoCards;
    QList<Components::FluentButton*> m_demoButtons;
    
    // Performance monitoring
    Core::FluentPerformanceMonitor* m_performanceMonitor{nullptr};
    QTimer* m_metricsTimer{nullptr};
    QElapsedTimer m_operationTimer;
    
    // State variables
    bool m_highContrastMode{false};
    bool m_screenReaderMode{false};
    bool m_animationsEnabled{true};
    bool m_responsiveMode{true};
    int m_currentFontSize{14};
    int m_animationSpeed{100}; // Percentage
    QString m_currentBreakpoint{"Desktop"};
    
    // Performance data
    double m_currentFPS{60.0};
    size_t m_memoryUsage{0};
    QStringList m_performanceMessages;
};

void AccessibilityPerformanceExample::setupUI() {
    setWindowTitle("FluentQt Accessibility and Performance Example");
    setMinimumSize(1200, 800);
    resize(1400, 900);
    
    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(m_mainSplitter);
    
    // Left panel - Controls
    auto* controlsWidget = new QWidget();
    auto* controlsLayout = new QVBoxLayout(controlsWidget);
    controlsLayout->setSpacing(10);
    
    // Add control panels
    controlsLayout->addWidget(createAccessibilityPanel());
    controlsLayout->addWidget(createPerformancePanel());
    controlsLayout->addWidget(createResponsivePanel());
    controlsLayout->addStretch();
    
    // Right panel - Demo content with scroll area
    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_demoContent = createDemoContentPanel();
    m_scrollArea->setWidget(m_demoContent);
    
    // Add to splitter
    m_mainSplitter->addWidget(controlsWidget);
    m_mainSplitter->addWidget(m_scrollArea);
    m_mainSplitter->setSizes({400, 1000});
    
    // Set up accessibility for main components
    Accessibility::setAccessibleName(this, "Accessibility and Performance Demo");
    Accessibility::setAccessibleDescription(this, "Demonstration of FluentQt accessibility features and performance monitoring");
    Accessibility::setAccessibleRole(controlsWidget, QAccessible::ToolBar);
    Accessibility::setAccessibleRole(m_scrollArea, QAccessible::ScrollBar);
}

QWidget* AccessibilityPerformanceExample::createAccessibilityPanel() {
    m_accessibilityGroup = new QGroupBox("Accessibility Features");
    auto* layout = new QVBoxLayout(m_accessibilityGroup);
    
    // High contrast mode
    m_highContrastCheck = new Components::FluentCheckBox("High Contrast Mode");
    Accessibility::setAccessibleName(m_highContrastCheck, "High Contrast Mode Toggle");
    Accessibility::setAccessibleDescription(m_highContrastCheck, "Enable high contrast colors for better visibility");
    layout->addWidget(m_highContrastCheck);
    
    // Screen reader mode
    m_screenReaderCheck = new Components::FluentCheckBox("Screen Reader Mode");
    Accessibility::setAccessibleName(m_screenReaderCheck, "Screen Reader Mode Toggle");
    Accessibility::setAccessibleDescription(m_screenReaderCheck, "Enable enhanced screen reader support");
    layout->addWidget(m_screenReaderCheck);
    
    // Animation control
    m_animationsCheck = new Components::FluentCheckBox("Enable Animations");
    m_animationsCheck->setChecked(true);
    Accessibility::setAccessibleName(m_animationsCheck, "Animation Toggle");
    Accessibility::setAccessibleDescription(m_animationsCheck, "Enable or disable animations for reduced motion");
    layout->addWidget(m_animationsCheck);
    
    // Font size control
    auto* fontLayout = new QHBoxLayout();
    fontLayout->addWidget(new QLabel("Font Size:"));
    
    m_fontSizeSlider = new QSlider(Qt::Horizontal);
    m_fontSizeSlider->setRange(10, 24);
    m_fontSizeSlider->setValue(14);
    Accessibility::setAccessibleName(m_fontSizeSlider, "Font Size Slider");
    Accessibility::setAccessibleDescription(m_fontSizeSlider, "Adjust font size from 10 to 24 pixels");
    
    m_fontSizeLabel = new QLabel("14px");
    m_fontSizeLabel->setMinimumWidth(40);
    
    fontLayout->addWidget(m_fontSizeSlider);
    fontLayout->addWidget(m_fontSizeLabel);
    layout->addLayout(fontLayout);
    
    // Screen reader announcement
    m_announceButton = new Components::FluentButton("Test Screen Reader");
    Accessibility::setAccessibleName(m_announceButton, "Test Screen Reader Announcement");
    Accessibility::setAccessibleDescription(m_announceButton, "Send a test message to screen readers");
    layout->addWidget(m_announceButton);
    
    // Reset button
    m_resetAccessibilityButton = new Components::FluentButton("Reset Settings");
    Accessibility::setAccessibleName(m_resetAccessibilityButton, "Reset Accessibility Settings");
    Accessibility::setAccessibleDescription(m_resetAccessibilityButton, "Reset all accessibility settings to defaults");
    layout->addWidget(m_resetAccessibilityButton);
    
    return m_accessibilityGroup;
}

QWidget* AccessibilityPerformanceExample::createPerformancePanel() {
    m_performanceGroup = new QGroupBox("Performance Monitoring");
    auto* layout = new QVBoxLayout(m_performanceGroup);
    
    // Performance metrics display
    m_fpsLabel = new QLabel("FPS: 60.0");
    m_fpsLabel->setStyleSheet("font-weight: bold; color: #0078d4;");
    Accessibility::setAccessibleName(m_fpsLabel, "Frames Per Second Display");
    layout->addWidget(m_fpsLabel);
    
    m_memoryLabel = new QLabel("Memory: 0 MB");
    Accessibility::setAccessibleName(m_memoryLabel, "Memory Usage Display");
    layout->addWidget(m_memoryLabel);
    
    m_profileLabel = new QLabel("Last Operation: 0ms");
    Accessibility::setAccessibleName(m_profileLabel, "Operation Time Display");
    layout->addWidget(m_profileLabel);
    
    // Performance indicator
    m_performanceBar = new QProgressBar();
    m_performanceBar->setRange(0, 100);
    m_performanceBar->setValue(100);
    m_performanceBar->setFormat("Performance: %p%");
    Accessibility::setAccessibleName(m_performanceBar, "Performance Indicator");
    Accessibility::setAccessibleDescription(m_performanceBar, "Shows current application performance level");
    layout->addWidget(m_performanceBar);
    
    // Performance test buttons
    m_performanceTestButton = new Components::FluentButton("Run Performance Test");
    Accessibility::setAccessibleName(m_performanceTestButton, "Run Performance Test");
    Accessibility::setAccessibleDescription(m_performanceTestButton, "Execute a comprehensive performance test");
    layout->addWidget(m_performanceTestButton);
    
    m_heavyOperationButton = new Components::FluentButton("Simulate Heavy Load");
    Accessibility::setAccessibleName(m_heavyOperationButton, "Simulate Heavy Load");
    Accessibility::setAccessibleDescription(m_heavyOperationButton, "Simulate a CPU-intensive operation");
    layout->addWidget(m_heavyOperationButton);
    
    // Performance log
    auto* logLabel = new QLabel("Performance Log:");
    layout->addWidget(logLabel);
    
    m_performanceLog = new QListWidget();
    m_performanceLog->setMaximumHeight(100);
    Accessibility::setAccessibleName(m_performanceLog, "Performance Log");
    Accessibility::setAccessibleDescription(m_performanceLog, "List of performance events and warnings");
    layout->addWidget(m_performanceLog);
    
    return m_performanceGroup;
}

QWidget* AccessibilityPerformanceExample::createResponsivePanel() {
    m_responsiveGroup = new QGroupBox("Responsive Design");
    auto* layout = new QVBoxLayout(m_responsiveGroup);

    // Responsive mode toggle
    m_responsiveCheck = new Components::FluentCheckBox("Responsive Mode");
    m_responsiveCheck->setChecked(true);
    Accessibility::setAccessibleName(m_responsiveCheck, "Responsive Mode Toggle");
    Accessibility::setAccessibleDescription(m_responsiveCheck, "Enable responsive layout adjustments");
    layout->addWidget(m_responsiveCheck);

    // Breakpoint simulation
    auto* breakpointLayout = new QHBoxLayout();
    breakpointLayout->addWidget(new QLabel("Breakpoint:"));

    m_breakpointCombo = new Components::FluentComboBox();
    m_breakpointCombo->addItems({"Mobile (320px)", "Tablet (768px)", "Desktop (1024px)", "Large (1440px)"});
    m_breakpointCombo->setCurrentIndex(2); // Desktop
    Accessibility::setAccessibleName(m_breakpointCombo, "Breakpoint Selector");
    Accessibility::setAccessibleDescription(m_breakpointCombo, "Select screen size breakpoint for testing");

    breakpointLayout->addWidget(m_breakpointCombo);
    layout->addLayout(breakpointLayout);

    // Animation speed control
    auto* speedLayout = new QHBoxLayout();
    speedLayout->addWidget(new QLabel("Animation Speed:"));

    m_animationSpeedSlider = new QSlider(Qt::Horizontal);
    m_animationSpeedSlider->setRange(25, 200);
    m_animationSpeedSlider->setValue(100);
    Accessibility::setAccessibleName(m_animationSpeedSlider, "Animation Speed Slider");
    Accessibility::setAccessibleDescription(m_animationSpeedSlider, "Adjust animation speed from 25% to 200%");

    m_animationSpeedLabel = new QLabel("100%");
    m_animationSpeedLabel->setMinimumWidth(40);

    speedLayout->addWidget(m_animationSpeedSlider);
    speedLayout->addWidget(m_animationSpeedLabel);
    layout->addLayout(speedLayout);

    return m_responsiveGroup;
}

QWidget* AccessibilityPerformanceExample::createDemoContentPanel() {
    auto* contentWidget = new QWidget();
    m_demoLayout = new QGridLayout(contentWidget);
    m_demoLayout->setSpacing(15);

    // Create demo cards with various content
    for (int i = 0; i < 12; ++i) {
        auto* card = new Components::FluentCard(contentWidget);
        card->setTitle(QString("Demo Card %1").arg(i + 1));
        card->setSubtitle(QString("Accessibility and performance demo content"));
        card->setMinimumSize(200, 150);

        // Set up accessibility
        Accessibility::setAccessibleName(card, QString("Demo Card %1").arg(i + 1));
        Accessibility::setAccessibleDescription(card, QString("Interactive demo card number %1 for testing accessibility features").arg(i + 1));
        Accessibility::setAccessibleRole(card, QAccessible::Button);

        // Add content to card
        auto* cardLayout = new QVBoxLayout(card);

        auto* button = new Components::FluentButton(QString("Action %1").arg(i + 1), card);
        Accessibility::setAccessibleName(button, QString("Action Button %1").arg(i + 1));
        Accessibility::setAccessibleDescription(button, QString("Perform action %1 with accessibility support").arg(i + 1));

        auto* progress = new Components::FluentProgressBar(card);
        progress->setValue(QRandomGenerator::global()->bounded(20, 100));
        Accessibility::setAccessibleName(progress, QString("Progress Indicator %1").arg(i + 1));
        Accessibility::setAccessibleDescription(progress, QString("Shows progress for task %1").arg(i + 1));

        cardLayout->addWidget(button);
        cardLayout->addWidget(progress);

        m_demoCards.append(card);
        m_demoButtons.append(button);

        // Add to grid layout
        int row = i / 3;
        int col = i % 3;
        m_demoLayout->addWidget(card, row, col);
    }

    return contentWidget;
}

void AccessibilityPerformanceExample::setupAccessibility() {
    // Enable accessibility if not already active
    if (!QAccessible::isActive()) {
        QAccessible::setActive(true);
    }

    // Check if high contrast mode is already enabled system-wide
    if (Accessibility::isHighContrastMode()) {
        m_highContrastMode = true;
        m_highContrastCheck->setChecked(true);
        updateAccessibilityFeatures();
    }

    // Set up keyboard navigation
    setTabOrder(m_highContrastCheck, m_screenReaderCheck);
    setTabOrder(m_screenReaderCheck, m_animationsCheck);
    setTabOrder(m_animationsCheck, m_fontSizeSlider);
    setTabOrder(m_fontSizeSlider, m_announceButton);
    setTabOrder(m_announceButton, m_resetAccessibilityButton);
}

void AccessibilityPerformanceExample::setupPerformanceMonitoring() {
    // Get performance monitor instance
    m_performanceMonitor = &Core::FluentPerformanceMonitor::instance();

    // Start monitoring
    m_performanceMonitor->startFrameRateMonitoring();

    // Set up metrics update timer
    m_metricsTimer = new QTimer(this);
    m_metricsTimer->setInterval(1000); // Update every second
    m_metricsTimer->start();
}

void AccessibilityPerformanceExample::setupConnections() {
    // Accessibility controls
    connect(m_highContrastCheck, &Components::FluentCheckBox::toggled, this, &AccessibilityPerformanceExample::toggleHighContrast);
    connect(m_screenReaderCheck, &Components::FluentCheckBox::toggled, this, &AccessibilityPerformanceExample::toggleScreenReaderMode);
    connect(m_animationsCheck, &Components::FluentCheckBox::toggled, this, &AccessibilityPerformanceExample::toggleAnimations);
    connect(m_fontSizeSlider, &QSlider::valueChanged, this, &AccessibilityPerformanceExample::adjustFontSize);
    connect(m_announceButton, &Components::FluentButton::clicked, this, &AccessibilityPerformanceExample::announceToScreenReader);
    connect(m_resetAccessibilityButton, &Components::FluentButton::clicked, this, &AccessibilityPerformanceExample::resetAccessibilitySettings);

    // Performance monitoring
    connect(m_performanceMonitor, &Core::FluentPerformanceMonitor::frameRateChanged, this, &AccessibilityPerformanceExample::onFrameRateChanged);
    connect(m_performanceMonitor, &Core::FluentPerformanceMonitor::performanceWarning, this, &AccessibilityPerformanceExample::onPerformanceWarning);
    connect(m_performanceTestButton, &Components::FluentButton::clicked, this, &AccessibilityPerformanceExample::runPerformanceTest);
    connect(m_heavyOperationButton, &Components::FluentButton::clicked, this, &AccessibilityPerformanceExample::simulateHeavyOperation);
    connect(m_metricsTimer, &QTimer::timeout, this, &AccessibilityPerformanceExample::updatePerformanceMetrics);

    // Responsive design controls
    connect(m_responsiveCheck, &Components::FluentCheckBox::toggled, this, &AccessibilityPerformanceExample::toggleResponsiveMode);
    connect(m_breakpointCombo, QOverload<int>::of(&Components::FluentComboBox::currentIndexChanged), [this](int index) {
        QStringList breakpoints = {"Mobile", "Tablet", "Desktop", "Large"};
        m_currentBreakpoint = breakpoints[index];
        simulateResponsiveBreakpoints();
    });
    connect(m_animationSpeedSlider, &QSlider::valueChanged, this, &AccessibilityPerformanceExample::adjustAnimationSpeed);

    // Demo button connections
    for (int i = 0; i < m_demoButtons.size(); ++i) {
        connect(m_demoButtons[i], &Components::FluentButton::clicked, [this, i]() {
            profileOperation(QString("Button Click %1").arg(i + 1), [this, i]() {
                // Simulate some work
                QThread::msleep(QRandomGenerator::global()->bounded(10, 50));

                // Announce action to screen reader if enabled
                if (m_screenReaderMode) {
                    Accessibility::announceToScreenReader(QString("Action %1 completed").arg(i + 1));
                }
            });
        });
    }
}

void AccessibilityPerformanceExample::setupTheme() {
    auto& theme = Styling::FluentTheme::instance();
    // Theme will be applied automatically to FluentQt components
}

void AccessibilityPerformanceExample::initializeDemo() {
    // Set initial values
    updateAccessibilityFeatures();
    updatePerformanceDisplay();
    simulateResponsiveBreakpoints();

    // Add initial performance log entry
    m_performanceLog->addItem(QString("[%1] Application started - Accessibility and Performance monitoring active")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}

// Slot implementations
void AccessibilityPerformanceExample::toggleHighContrast() {
    m_highContrastMode = m_highContrastCheck->isChecked();
    updateAccessibilityFeatures();

    // Announce change to screen reader
    if (m_screenReaderMode) {
        QString message = m_highContrastMode ? "High contrast mode enabled" : "High contrast mode disabled";
        Accessibility::announceToScreenReader(message);
    }

    // Log the change
    m_performanceLog->addItem(QString("[%1] High contrast mode %2")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(m_highContrastMode ? "enabled" : "disabled"));
}

void AccessibilityPerformanceExample::adjustFontSize(int size) {
    m_currentFontSize = size;
    m_fontSizeLabel->setText(QString("%1px").arg(size));
    updateAccessibilityFeatures();

    // Announce change to screen reader
    if (m_screenReaderMode) {
        Accessibility::announceToScreenReader(QString("Font size changed to %1 pixels").arg(size));
    }
}

void AccessibilityPerformanceExample::toggleAnimations(bool enabled) {
    m_animationsEnabled = enabled;
    updateAccessibilityFeatures();

    // Announce change to screen reader
    if (m_screenReaderMode) {
        QString message = enabled ? "Animations enabled" : "Animations disabled for reduced motion";
        Accessibility::announceToScreenReader(message);
    }

    // Log the change
    m_performanceLog->addItem(QString("[%1] Animations %2")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(enabled ? "enabled" : "disabled"));
}

void AccessibilityPerformanceExample::toggleScreenReaderMode(bool enabled) {
    m_screenReaderMode = enabled;
    updateAccessibilityFeatures();

    // Announce the mode change
    if (enabled) {
        Accessibility::announceToScreenReader("Screen reader mode enabled. Enhanced accessibility features are now active.");
    }

    // Log the change
    m_performanceLog->addItem(QString("[%1] Screen reader mode %2")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(enabled ? "enabled" : "disabled"));
}

void AccessibilityPerformanceExample::announceToScreenReader() {
    QString message = QString("Accessibility test announcement at %1. Current settings: High contrast %2, Animations %3, Font size %4 pixels.")
                     .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                     .arg(m_highContrastMode ? "enabled" : "disabled")
                     .arg(m_animationsEnabled ? "enabled" : "disabled")
                     .arg(m_currentFontSize);

    Accessibility::announceToScreenReader(message);

    // Also log the announcement
    m_performanceLog->addItem(QString("[%1] Screen reader test: %2")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(message));
}

void AccessibilityPerformanceExample::onFrameRateChanged(double fps) {
    m_currentFPS = fps;
    updatePerformanceDisplay();

    // Update performance bar based on FPS
    int performancePercent = static_cast<int>((fps / 60.0) * 100);
    performancePercent = qBound(0, performancePercent, 100);
    m_performanceBar->setValue(performancePercent);

    // Change color based on performance
    QString color = "green";
    if (fps < 30) color = "red";
    else if (fps < 45) color = "orange";

    m_performanceBar->setStyleSheet(QString("QProgressBar::chunk { background-color: %1; }").arg(color));
}

void AccessibilityPerformanceExample::onPerformanceWarning(const QString& message) {
    // Add to performance log
    m_performanceLog->addItem(QString("[%1] WARNING: %2")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(message));

    // Scroll to bottom to show latest message
    m_performanceLog->scrollToBottom();

    // Announce critical performance issues to screen reader
    if (m_screenReaderMode && message.contains("Very low")) {
        Accessibility::announceToScreenReader("Performance warning: Very low frame rate detected");
    }
}

void AccessibilityPerformanceExample::runPerformanceTest() {
    profileOperation("Performance Test", [this]() {
        // Simulate various operations
        for (int i = 0; i < 1000; ++i) {
            // Simulate some computational work
            volatile double result = 0;
            for (int j = 0; j < 1000; ++j) {
                result += qSin(i * j * 0.001);
            }
        }

        // Update UI elements
        for (auto* card : m_demoCards) {
            card->update();
        }
    });

    // Log test completion
    auto testTime = m_performanceMonitor->getProfileTime("Performance Test");
    m_performanceLog->addItem(QString("[%1] Performance test completed in %2ms")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(testTime.count()));

    // Announce completion to screen reader
    if (m_screenReaderMode) {
        Accessibility::announceToScreenReader(QString("Performance test completed in %1 milliseconds").arg(testTime.count()));
    }
}

void AccessibilityPerformanceExample::updatePerformanceMetrics() {
    updatePerformanceDisplay();

    // Simulate memory usage (in a real app, this would use actual memory APIs)
    m_memoryUsage = QRandomGenerator::global()->bounded(50, 200) * 1024 * 1024; // 50-200 MB
    m_memoryLabel->setText(QString("Memory: %1 MB").arg(m_memoryUsage / (1024 * 1024)));
}

void AccessibilityPerformanceExample::simulateHeavyOperation() {
    profileOperation("Heavy Operation", [this]() {
        // Simulate a CPU-intensive task
        QThread::msleep(100); // Simulate blocking operation

        // Perform some calculations
        volatile double result = 0;
        for (int i = 0; i < 10000; ++i) {
            for (int j = 0; j < 100; ++j) {
                result += qSin(i) * qCos(j);
            }
        }
    });

    auto operationTime = m_performanceMonitor->getProfileTime("Heavy Operation");
    m_performanceLog->addItem(QString("[%1] Heavy operation completed in %2ms")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(operationTime.count()));

    if (m_screenReaderMode) {
        Accessibility::announceToScreenReader("Heavy operation completed");
    }
}

void AccessibilityPerformanceExample::toggleResponsiveMode() {
    m_responsiveMode = m_responsiveCheck->isChecked();
    simulateResponsiveBreakpoints();

    m_performanceLog->addItem(QString("[%1] Responsive mode %2")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(m_responsiveMode ? "enabled" : "disabled"));
}

void AccessibilityPerformanceExample::adjustAnimationSpeed(int speed) {
    m_animationSpeed = speed;
    m_animationSpeedLabel->setText(QString("%1%").arg(speed));

    // In a real implementation, this would adjust animation durations globally
    m_performanceLog->addItem(QString("[%1] Animation speed set to %2%")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(speed));
}

void AccessibilityPerformanceExample::resetAccessibilitySettings() {
    // Reset all accessibility settings to defaults
    m_highContrastCheck->setChecked(false);
    m_screenReaderCheck->setChecked(false);
    m_animationsCheck->setChecked(true);
    m_fontSizeSlider->setValue(14);

    m_highContrastMode = false;
    m_screenReaderMode = false;
    m_animationsEnabled = true;
    m_currentFontSize = 14;

    updateAccessibilityFeatures();

    m_performanceLog->addItem(QString("[%1] Accessibility settings reset to defaults")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));

    Accessibility::announceToScreenReader("Accessibility settings have been reset to default values");
}

// Helper method implementations
void AccessibilityPerformanceExample::updateAccessibilityFeatures() {
    auto& theme = Styling::FluentTheme::instance();

    // Apply high contrast mode
    if (m_highContrastMode) {
        theme.setHighContrastMode(true);
        // Update all demo cards for high contrast
        for (auto* card : m_demoCards) {
            Accessibility::updateForHighContrast(card);
        }
    } else {
        theme.setHighContrastMode(false);
    }

    // Apply font size changes
    QFont appFont = QApplication::font();
    appFont.setPointSize(m_currentFontSize);

    // Apply to demo content
    for (auto* button : m_demoButtons) {
        button->setFont(appFont);
    }

    // Update font size label
    m_fontSizeLabel->setText(QString("%1px").arg(m_currentFontSize));

    // Force update of all widgets
    update();
}

void AccessibilityPerformanceExample::updatePerformanceDisplay() {
    m_fpsLabel->setText(QString("FPS: %1").arg(m_currentFPS, 0, 'f', 1));

    // Update profile label with last operation time
    if (m_performanceMonitor->getProfileTime("Button Click 1").count() != 0) {
        auto lastTime = m_performanceMonitor->getProfileTime("Button Click 1");
        m_profileLabel->setText(QString("Last Operation: %1ms").arg(lastTime.count()));
    }
}

void AccessibilityPerformanceExample::simulateResponsiveBreakpoints() {
    if (!m_responsiveMode) return;

    // Simulate different screen sizes by adjusting layout
    int columns = 3; // Default desktop
    int cardWidth = 200;

    if (m_currentBreakpoint == "Mobile") {
        columns = 1;
        cardWidth = 280;
    } else if (m_currentBreakpoint == "Tablet") {
        columns = 2;
        cardWidth = 240;
    } else if (m_currentBreakpoint == "Large") {
        columns = 4;
        cardWidth = 180;
    }

    // Rearrange demo cards
    for (int i = 0; i < m_demoCards.size(); ++i) {
        int row = i / columns;
        int col = i % columns;

        m_demoLayout->removeWidget(m_demoCards[i]);
        m_demoLayout->addWidget(m_demoCards[i], row, col);

        m_demoCards[i]->setMinimumWidth(cardWidth);
        m_demoCards[i]->setMaximumWidth(cardWidth + 50);
    }

    // Announce layout change to screen reader
    if (m_screenReaderMode) {
        Accessibility::announceToScreenReader(QString("Layout changed to %1 breakpoint with %2 columns")
                                            .arg(m_currentBreakpoint).arg(columns));
    }
}

void AccessibilityPerformanceExample::profileOperation(const QString& operation, std::function<void()> func) {
    m_performanceMonitor->beginProfile(operation);
    func();
    m_performanceMonitor->endProfile(operation);

    auto time = m_performanceMonitor->getProfileTime(operation);
    m_profileLabel->setText(QString("Last Operation: %1ms").arg(time.count()));
}

// Main function
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("FluentQt Accessibility and Performance Example");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("FluentQt Examples");

    // Enable accessibility
    QAccessible::setActive(true);

    // Initialize FluentQt theme
    auto& theme = Styling::FluentTheme::instance();

    AccessibilityPerformanceExample window;
    window.show();

    return app.exec();
}

#include "AccessibilityPerformanceExample.moc"
