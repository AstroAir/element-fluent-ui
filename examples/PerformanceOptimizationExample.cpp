// examples/PerformanceOptimizationExample.cpp
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QCoreApplication>
#include <QDateTime>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QMutex>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QRunnable>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QThread>
#include <QThreadPool>
#include <QTimer>
#include <QVBoxLayout>

#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentChartView.h"
#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class PerformanceOptimizationExample : public QMainWindow {
    Q_OBJECT

public:
    explicit PerformanceOptimizationExample(QWidget* parent = nullptr);

private slots:
    // Performance monitoring
    void onFrameRateChanged(double fps);
    void onPerformanceWarning(const QString& message);
    void updatePerformanceMetrics();
    void togglePerformanceMonitoring(bool enabled);

    // Memory management
    void runMemoryStressTest();
    void runGarbageCollection();
    void analyzeMemoryUsage();
    void toggleMemoryOptimization(bool enabled);

    // Large dataset handling
    void generateLargeDataset();
    void enableVirtualScrolling(bool enabled);
    void toggleLazyLoading(bool enabled);
    void benchmarkDataOperations();

    // Animation optimization
    void toggleAnimationOptimization(bool enabled);
    void adjustAnimationQuality(int quality);
    void runAnimationBenchmark();

    // Rendering optimization
    void toggleHardwareAcceleration(bool enabled);
    void adjustRenderingQuality(int quality);
    void runRenderingBenchmark();

    // Threading and concurrency
    void runMultithreadedOperation();
    void adjustThreadPoolSize(int size);
    void runConcurrencyBenchmark();

    // Profiling and debugging
    void startProfiling();
    void stopProfiling();
    void exportProfilingData();
    void clearProfilingData();

private:
    void setupUI();
    void setupPerformanceMonitoring();
    void setupMemoryManagement();
    void setupDatasetHandling();
    void setupAnimationOptimization();
    void setupRenderingOptimization();
    void setupThreadingControls();
    void setupProfilingTools();
    void setupConnections();
    void setupTheme();

    // Tab setup methods
    void setupPerformanceTab(QWidget* tab);
    void setupMemoryTab(QWidget* tab);
    void setupDatasetTab(QWidget* tab);
    void setupOptimizationTab(QWidget* tab);
    void setupProfilingTab(QWidget* tab);

    // Helper methods
    void createPerformanceMetricsPanel();
    void createMemoryManagementPanel();
    void createDatasetPanel();
    void createOptimizationPanel();
    void createProfilingPanel();
    void populateLargeDataset(int itemCount);
    void simulateHeavyComputation();
    void profileOperation(const QString& operation, std::function<void()> func);
    void logPerformanceEvent(const QString& event);
    void updateMemoryDisplay();
    void updateFPSDisplay();
    void optimizeForLowEndDevice();
    void optimizeForHighEndDevice();

private:
    // Main UI components
    QWidget* m_centralWidget{nullptr};
    QTabWidget* m_mainTabs{nullptr};
    QSplitter* m_mainSplitter{nullptr};

    // Performance monitoring
    QGroupBox* m_performanceGroup{nullptr};
    QLabel* m_fpsLabel{nullptr};
    QLabel* m_memoryLabel{nullptr};
    QLabel* m_cpuLabel{nullptr};
    QProgressBar* m_performanceBar{nullptr};
    QCheckBox* m_monitoringEnabled{nullptr};
    Components::FluentChartView* m_performanceChart{nullptr};

    // Memory management
    QGroupBox* m_memoryGroup{nullptr};
    QLabel* m_heapSizeLabel{nullptr};
    QLabel* m_allocatedLabel{nullptr};
    QLabel* m_availableLabel{nullptr};
    QProgressBar* m_memoryUsageBar{nullptr};
    Components::FluentButton* m_gcButton{nullptr};
    Components::FluentButton* m_memoryTestButton{nullptr};
    QCheckBox* m_memoryOptimization{nullptr};

    // Large dataset handling
    QGroupBox* m_datasetGroup{nullptr};
    Components::FluentTreeView* m_largeTreeView{nullptr};
    QTableWidget* m_largeTableWidget{nullptr};
    QListWidget* m_largeListWidget{nullptr};
    QSpinBox* m_datasetSizeSpinBox{nullptr};
    QCheckBox* m_virtualScrolling{nullptr};
    QCheckBox* m_lazyLoading{nullptr};
    Components::FluentButton* m_generateDataButton{nullptr};
    Components::FluentButton* m_benchmarkButton{nullptr};

    // Animation optimization
    QGroupBox* m_animationGroup{nullptr};
    QSlider* m_animationQualitySlider{nullptr};
    QCheckBox* m_animationOptimization{nullptr};
    QCheckBox* m_hardwareAcceleration{nullptr};
    Components::FluentButton* m_animationBenchmarkButton{nullptr};
    QLabel* m_animationQualityLabel{nullptr};

    // Rendering optimization
    QGroupBox* m_renderingGroup{nullptr};
    QSlider* m_renderingQualitySlider{nullptr};
    QCheckBox* m_antialiasing{nullptr};
    QCheckBox* m_textureOptimization{nullptr};
    Components::FluentButton* m_renderingBenchmarkButton{nullptr};
    QLabel* m_renderingQualityLabel{nullptr};

    // Threading and concurrency
    QGroupBox* m_threadingGroup{nullptr};
    QSpinBox* m_threadPoolSizeSpinBox{nullptr};
    QLabel* m_activeThreadsLabel{nullptr};
    Components::FluentButton* m_multithreadTestButton{nullptr};
    Components::FluentButton* m_concurrencyBenchmarkButton{nullptr};
    QProgressBar* m_threadingProgressBar{nullptr};

    // Profiling and debugging
    QGroupBox* m_profilingGroup{nullptr};
    Components::FluentButton* m_startProfilingButton{nullptr};
    Components::FluentButton* m_stopProfilingButton{nullptr};
    Components::FluentButton* m_exportProfilingButton{nullptr};
    Components::FluentButton* m_clearProfilingButton{nullptr};
    QTextEdit* m_profilingLog{nullptr};
    QListWidget* m_performanceEvents{nullptr};

    // Performance monitoring system
    Core::FluentPerformanceMonitor* m_performanceMonitor{nullptr};
    QTimer* m_metricsUpdateTimer{nullptr};
    QElapsedTimer m_operationTimer;

    // Data management
    QStandardItemModel* m_largeDataModel{nullptr};
    QList<QVariantMap> m_largeDataset;

    // State variables
    bool m_performanceMonitoringEnabled{true};
    bool m_memoryOptimizationEnabled{false};
    bool m_virtualScrollingEnabled{false};
    bool m_lazyLoadingEnabled{false};
    bool m_animationOptimizationEnabled{false};
    bool m_hardwareAccelerationEnabled{true};
    bool m_profilingActive{false};
    int m_animationQuality{100};
    int m_renderingQuality{100};
    int m_threadPoolSize{4};

    // Performance metrics
    double m_currentFPS{60.0};
    size_t m_memoryUsage{0};
    double m_cpuUsage{0.0};
    QStringList m_performanceLog;
    QMap<QString, qint64> m_profilingResults;

    // Threading
    QThreadPool* m_threadPool{nullptr};
    QMutex m_dataMutex;

    // Clipboard for data export
    QClipboard* m_clipboard{nullptr};
};

// Constructor implementation
PerformanceOptimizationExample::PerformanceOptimizationExample(QWidget* parent)
    : QMainWindow(parent),
      m_performanceMonitor(&Core::FluentPerformanceMonitor::instance()),
      m_threadPool(QThreadPool::globalInstance()),
      m_clipboard(QApplication::clipboard()) {
    setWindowTitle("FluentQt Performance and Optimization Example");
    setMinimumSize(1200, 800);
    resize(1400, 900);

    setupUI();
    setupPerformanceMonitoring();
    setupMemoryManagement();
    setupDatasetHandling();
    setupAnimationOptimization();
    setupRenderingOptimization();
    setupThreadingControls();
    setupProfilingTools();
    setupConnections();
    setupTheme();

    // Initialize performance monitoring
    m_performanceMonitor->startFrameRateMonitoring();

    // Start metrics update timer
    m_metricsUpdateTimer = new QTimer(this);
    m_metricsUpdateTimer->setInterval(1000);  // Update every second
    m_metricsUpdateTimer->start();

    logPerformanceEvent("Performance optimization example initialized");
}

void PerformanceOptimizationExample::setupUI() {
    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);

    auto* mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    // Create main tabs
    m_mainTabs = new QTabWidget;
    m_mainTabs->setTabPosition(QTabWidget::North);
    m_mainTabs->setMovable(true);
    m_mainTabs->setTabsClosable(false);

    // Create tab pages
    auto* performanceTab = new QWidget;
    auto* memoryTab = new QWidget;
    auto* datasetTab = new QWidget;
    auto* optimizationTab = new QWidget;
    auto* profilingTab = new QWidget;

    m_mainTabs->addTab(performanceTab, "Performance Monitoring");
    m_mainTabs->addTab(memoryTab, "Memory Management");
    m_mainTabs->addTab(datasetTab, "Large Datasets");
    m_mainTabs->addTab(optimizationTab, "Optimization");
    m_mainTabs->addTab(profilingTab, "Profiling & Debug");

    mainLayout->addWidget(m_mainTabs);

    // Setup each tab
    setupPerformanceTab(performanceTab);
    setupMemoryTab(memoryTab);
    setupDatasetTab(datasetTab);
    setupOptimizationTab(optimizationTab);
    setupProfilingTab(profilingTab);
}

void PerformanceOptimizationExample::setupPerformanceTab(QWidget* tab) {
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    // Performance metrics panel
    createPerformanceMetricsPanel();
    layout->addWidget(m_performanceGroup);

    // Performance chart
    m_performanceChart = new Components::FluentChartView;
    m_performanceChart->setTitle("Real-time Performance Metrics");
    m_performanceChart->setMinimumHeight(300);
    layout->addWidget(m_performanceChart);

    // Controls
    auto* controlsLayout = new QHBoxLayout;

    m_monitoringEnabled = new QCheckBox("Enable Performance Monitoring");
    m_monitoringEnabled->setChecked(m_performanceMonitoringEnabled);
    controlsLayout->addWidget(m_monitoringEnabled);

    auto* optimizeButton = new Components::FluentButton("Auto-Optimize");
    optimizeButton->setToolTip(
        "Automatically optimize settings based on current performance");
    controlsLayout->addWidget(optimizeButton);

    controlsLayout->addStretch();
    layout->addLayout(controlsLayout);

    layout->addStretch();
}

void PerformanceOptimizationExample::setupMemoryTab(QWidget* tab) {
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    // Memory management panel
    createMemoryManagementPanel();
    layout->addWidget(m_memoryGroup);

    // Memory controls
    auto* controlsLayout = new QHBoxLayout;

    m_gcButton = new Components::FluentButton("Force Garbage Collection");
    m_gcButton->setToolTip("Trigger garbage collection to free unused memory");
    controlsLayout->addWidget(m_gcButton);

    m_memoryTestButton = new Components::FluentButton("Run Memory Stress Test");
    m_memoryTestButton->setToolTip(
        "Test memory allocation and deallocation performance");
    controlsLayout->addWidget(m_memoryTestButton);

    m_memoryOptimization = new QCheckBox("Enable Memory Optimization");
    m_memoryOptimization->setChecked(m_memoryOptimizationEnabled);
    controlsLayout->addWidget(m_memoryOptimization);

    controlsLayout->addStretch();
    layout->addLayout(controlsLayout);

    // Memory usage visualization
    auto* memoryChart = new Components::FluentChartView;
    memoryChart->setTitle("Memory Usage Over Time");
    memoryChart->setMinimumHeight(250);
    layout->addWidget(memoryChart);

    layout->addStretch();
}

void PerformanceOptimizationExample::setupDatasetTab(QWidget* tab) {
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    // Dataset controls
    createDatasetPanel();
    layout->addWidget(m_datasetGroup);

    // Large dataset demonstration
    auto* datasetSplitter = new QSplitter(Qt::Horizontal);

    // Tree view for hierarchical data
    m_largeTreeView = new Components::FluentTreeView;
    m_largeTreeView->setMinimumWidth(300);
    datasetSplitter->addWidget(m_largeTreeView);

    // Table view for tabular data
    m_largeTableWidget = new QTableWidget;
    m_largeTableWidget->setMinimumWidth(400);
    datasetSplitter->addWidget(m_largeTableWidget);

    // List view for simple data
    m_largeListWidget = new QListWidget;
    m_largeListWidget->setMinimumWidth(200);
    datasetSplitter->addWidget(m_largeListWidget);

    layout->addWidget(datasetSplitter);
}

void PerformanceOptimizationExample::setupOptimizationTab(QWidget* tab) {
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    // Animation optimization
    createOptimizationPanel();
    layout->addWidget(m_animationGroup);

    // Rendering optimization
    layout->addWidget(m_renderingGroup);

    // Threading controls
    layout->addWidget(m_threadingGroup);

    layout->addStretch();
}

void PerformanceOptimizationExample::setupProfilingTab(QWidget* tab) {
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    // Profiling controls
    createProfilingPanel();
    layout->addWidget(m_profilingGroup);

    // Profiling log and events
    auto* logSplitter = new QSplitter(Qt::Horizontal);

    m_profilingLog = new QTextEdit;
    m_profilingLog->setReadOnly(true);
    m_profilingLog->setMinimumHeight(200);
    m_profilingLog->setPlaceholderText("Profiling data will appear here...");
    logSplitter->addWidget(m_profilingLog);

    m_performanceEvents = new QListWidget;
    m_performanceEvents->setMinimumWidth(300);
    logSplitter->addWidget(m_performanceEvents);

    layout->addWidget(logSplitter);
}

void PerformanceOptimizationExample::createPerformanceMetricsPanel() {
    m_performanceGroup = new QGroupBox("Real-time Performance Metrics");
    auto* layout = new QGridLayout(m_performanceGroup);

    // FPS display
    layout->addWidget(new QLabel("Frame Rate:"), 0, 0);
    m_fpsLabel = new QLabel("60.0 FPS");
    m_fpsLabel->setStyleSheet("font-weight: bold; color: #0078d4;");
    layout->addWidget(m_fpsLabel, 0, 1);

    // Memory display
    layout->addWidget(new QLabel("Memory Usage:"), 1, 0);
    m_memoryLabel = new QLabel("0 MB");
    m_memoryLabel->setStyleSheet("font-weight: bold; color: #107c10;");
    layout->addWidget(m_memoryLabel, 1, 1);

    // CPU display
    layout->addWidget(new QLabel("CPU Usage:"), 2, 0);
    m_cpuLabel = new QLabel("0%");
    m_cpuLabel->setStyleSheet("font-weight: bold; color: #d13438;");
    layout->addWidget(m_cpuLabel, 2, 1);

    // Performance bar
    layout->addWidget(new QLabel("Overall Performance:"), 3, 0);
    m_performanceBar = new QProgressBar;
    m_performanceBar->setRange(0, 100);
    m_performanceBar->setValue(100);
    m_performanceBar->setTextVisible(true);
    layout->addWidget(m_performanceBar, 3, 1);
}

void PerformanceOptimizationExample::createMemoryManagementPanel() {
    m_memoryGroup = new QGroupBox("Memory Management");
    auto* layout = new QGridLayout(m_memoryGroup);

    // Heap size
    layout->addWidget(new QLabel("Heap Size:"), 0, 0);
    m_heapSizeLabel = new QLabel("0 MB");
    layout->addWidget(m_heapSizeLabel, 0, 1);

    // Allocated memory
    layout->addWidget(new QLabel("Allocated:"), 1, 0);
    m_allocatedLabel = new QLabel("0 MB");
    layout->addWidget(m_allocatedLabel, 1, 1);

    // Available memory
    layout->addWidget(new QLabel("Available:"), 2, 0);
    m_availableLabel = new QLabel("0 MB");
    layout->addWidget(m_availableLabel, 2, 1);

    // Memory usage bar
    layout->addWidget(new QLabel("Usage:"), 3, 0);
    m_memoryUsageBar = new QProgressBar;
    m_memoryUsageBar->setRange(0, 100);
    m_memoryUsageBar->setValue(0);
    layout->addWidget(m_memoryUsageBar, 3, 1);
}

void PerformanceOptimizationExample::createDatasetPanel() {
    m_datasetGroup = new QGroupBox("Large Dataset Configuration");
    auto* layout = new QGridLayout(m_datasetGroup);

    // Dataset size control
    layout->addWidget(new QLabel("Dataset Size:"), 0, 0);
    m_datasetSizeSpinBox = new QSpinBox;
    m_datasetSizeSpinBox->setRange(1000, 1000000);
    m_datasetSizeSpinBox->setValue(10000);
    m_datasetSizeSpinBox->setSuffix(" items");
    layout->addWidget(m_datasetSizeSpinBox, 0, 1);

    // Virtual scrolling
    m_virtualScrolling = new QCheckBox("Enable Virtual Scrolling");
    m_virtualScrolling->setChecked(m_virtualScrollingEnabled);
    m_virtualScrolling->setToolTip(
        "Use virtual scrolling for better performance with large datasets");
    layout->addWidget(m_virtualScrolling, 1, 0, 1, 2);

    // Lazy loading
    m_lazyLoading = new QCheckBox("Enable Lazy Loading");
    m_lazyLoading->setChecked(m_lazyLoadingEnabled);
    m_lazyLoading->setToolTip(
        "Load data on-demand to reduce initial load time");
    layout->addWidget(m_lazyLoading, 2, 0, 1, 2);

    // Control buttons
    auto* buttonLayout = new QHBoxLayout;

    m_generateDataButton = new Components::FluentButton("Generate Dataset");
    m_generateDataButton->setToolTip("Generate large dataset for testing");
    buttonLayout->addWidget(m_generateDataButton);

    m_benchmarkButton = new Components::FluentButton("Run Benchmark");
    m_benchmarkButton->setToolTip("Benchmark data operations performance");
    buttonLayout->addWidget(m_benchmarkButton);

    buttonLayout->addStretch();
    layout->addLayout(buttonLayout, 3, 0, 1, 2);
}

void PerformanceOptimizationExample::createOptimizationPanel() {
    // Animation optimization
    m_animationGroup = new QGroupBox("Animation Optimization");
    auto* animLayout = new QGridLayout(m_animationGroup);

    // Animation quality slider
    animLayout->addWidget(new QLabel("Animation Quality:"), 0, 0);
    m_animationQualitySlider = new QSlider(Qt::Horizontal);
    m_animationQualitySlider->setRange(10, 100);
    m_animationQualitySlider->setValue(m_animationQuality);
    animLayout->addWidget(m_animationQualitySlider, 0, 1);

    m_animationQualityLabel = new QLabel("100%");
    animLayout->addWidget(m_animationQualityLabel, 0, 2);

    // Animation optimization checkbox
    m_animationOptimization = new QCheckBox("Enable Animation Optimization");
    m_animationOptimization->setChecked(m_animationOptimizationEnabled);
    animLayout->addWidget(m_animationOptimization, 1, 0, 1, 3);

    // Hardware acceleration
    m_hardwareAcceleration = new QCheckBox("Hardware Acceleration");
    m_hardwareAcceleration->setChecked(m_hardwareAccelerationEnabled);
    animLayout->addWidget(m_hardwareAcceleration, 2, 0, 1, 3);

    // Animation benchmark button
    m_animationBenchmarkButton =
        new Components::FluentButton("Run Animation Benchmark");
    animLayout->addWidget(m_animationBenchmarkButton, 3, 0, 1, 3);

    // Rendering optimization
    m_renderingGroup = new QGroupBox("Rendering Optimization");
    auto* renderLayout = new QGridLayout(m_renderingGroup);

    // Rendering quality slider
    renderLayout->addWidget(new QLabel("Rendering Quality:"), 0, 0);
    m_renderingQualitySlider = new QSlider(Qt::Horizontal);
    m_renderingQualitySlider->setRange(25, 100);
    m_renderingQualitySlider->setValue(m_renderingQuality);
    renderLayout->addWidget(m_renderingQualitySlider, 0, 1);

    m_renderingQualityLabel = new QLabel("100%");
    renderLayout->addWidget(m_renderingQualityLabel, 0, 2);

    // Antialiasing
    m_antialiasing = new QCheckBox("Enable Antialiasing");
    m_antialiasing->setChecked(true);
    renderLayout->addWidget(m_antialiasing, 1, 0, 1, 3);

    // Texture optimization
    m_textureOptimization = new QCheckBox("Texture Optimization");
    m_textureOptimization->setChecked(true);
    renderLayout->addWidget(m_textureOptimization, 2, 0, 1, 3);

    // Rendering benchmark button
    m_renderingBenchmarkButton =
        new Components::FluentButton("Run Rendering Benchmark");
    renderLayout->addWidget(m_renderingBenchmarkButton, 3, 0, 1, 3);

    // Threading controls
    m_threadingGroup = new QGroupBox("Threading and Concurrency");
    auto* threadLayout = new QGridLayout(m_threadingGroup);

    // Thread pool size
    threadLayout->addWidget(new QLabel("Thread Pool Size:"), 0, 0);
    m_threadPoolSizeSpinBox = new QSpinBox;
    m_threadPoolSizeSpinBox->setRange(1, QThread::idealThreadCount() * 2);
    m_threadPoolSizeSpinBox->setValue(m_threadPoolSize);
    threadLayout->addWidget(m_threadPoolSizeSpinBox, 0, 1);

    // Active threads display
    threadLayout->addWidget(new QLabel("Active Threads:"), 1, 0);
    m_activeThreadsLabel = new QLabel("0");
    threadLayout->addWidget(m_activeThreadsLabel, 1, 1);

    // Threading progress bar
    threadLayout->addWidget(new QLabel("Progress:"), 2, 0);
    m_threadingProgressBar = new QProgressBar;
    m_threadingProgressBar->setRange(0, 100);
    m_threadingProgressBar->setValue(0);
    threadLayout->addWidget(m_threadingProgressBar, 2, 1);

    // Threading test buttons
    auto* threadButtonLayout = new QHBoxLayout;

    m_multithreadTestButton = new Components::FluentButton("Multithread Test");
    threadButtonLayout->addWidget(m_multithreadTestButton);

    m_concurrencyBenchmarkButton =
        new Components::FluentButton("Concurrency Benchmark");
    threadButtonLayout->addWidget(m_concurrencyBenchmarkButton);

    threadLayout->addLayout(threadButtonLayout, 3, 0, 1, 2);
}

void PerformanceOptimizationExample::createProfilingPanel() {
    m_profilingGroup = new QGroupBox("Profiling and Debugging Tools");
    auto* layout = new QGridLayout(m_profilingGroup);

    // Profiling control buttons
    auto* buttonLayout = new QHBoxLayout;

    m_startProfilingButton = new Components::FluentButton("Start Profiling");
    m_startProfilingButton->setToolTip("Begin performance profiling session");
    buttonLayout->addWidget(m_startProfilingButton);

    m_stopProfilingButton = new Components::FluentButton("Stop Profiling");
    m_stopProfilingButton->setEnabled(false);
    m_stopProfilingButton->setToolTip("End current profiling session");
    buttonLayout->addWidget(m_stopProfilingButton);

    m_exportProfilingButton = new Components::FluentButton("Export Data");
    m_exportProfilingButton->setToolTip("Export profiling data to file");
    buttonLayout->addWidget(m_exportProfilingButton);

    m_clearProfilingButton = new Components::FluentButton("Clear Data");
    m_clearProfilingButton->setToolTip("Clear all profiling data");
    buttonLayout->addWidget(m_clearProfilingButton);

    buttonLayout->addStretch();
    layout->addLayout(buttonLayout, 0, 0, 1, 2);
}

// Performance monitoring slot implementations
void PerformanceOptimizationExample::onFrameRateChanged(double fps) {
    m_currentFPS = fps;
    updateFPSDisplay();

    // Update performance bar based on FPS
    int performanceScore = static_cast<int>((fps / 60.0) * 100);
    m_performanceBar->setValue(qMax(0, qMin(100, performanceScore)));

    // Log significant FPS changes
    static double lastLoggedFPS = 60.0;
    if (qAbs(fps - lastLoggedFPS) > 5.0) {
        logPerformanceEvent(QString("FPS changed to %1").arg(fps, 0, 'f', 1));
        lastLoggedFPS = fps;
    }
}

void PerformanceOptimizationExample::onPerformanceWarning(
    const QString& message) {
    logPerformanceEvent(QString("WARNING: %1").arg(message));

    // Add to performance events list
    auto* item =
        new QListWidgetItem(QString("[%1] %2")
                                .arg(QTime::currentTime().toString("hh:mm:ss"))
                                .arg(message));
    item->setIcon(QIcon(":/icons/warning.png"));
    m_performanceEvents->addItem(item);

    // Auto-scroll to latest event
    m_performanceEvents->scrollToBottom();
}

void PerformanceOptimizationExample::updatePerformanceMetrics() {
    if (!m_performanceMonitoringEnabled) {
        return;
    }

    // Update memory display
    updateMemoryDisplay();

    // Simulate CPU usage (in a real app, this would use actual system APIs)
    m_cpuUsage = 5.0 + (QRandomGenerator::global()->generateDouble() * 20.0);
    m_cpuLabel->setText(QString("%1%").arg(m_cpuUsage, 0, 'f', 1));

    // Update active threads count
    m_activeThreadsLabel->setText(
        QString::number(m_threadPool->activeThreadCount()));
}

void PerformanceOptimizationExample::togglePerformanceMonitoring(bool enabled) {
    m_performanceMonitoringEnabled = enabled;

    if (enabled) {
        m_performanceMonitor->startFrameRateMonitoring();
        m_metricsUpdateTimer->start();
        logPerformanceEvent("Performance monitoring enabled");
    } else {
        m_performanceMonitor->stopFrameRateMonitoring();
        m_metricsUpdateTimer->stop();
        logPerformanceEvent("Performance monitoring disabled");
    }
}

// Memory management slot implementations
void PerformanceOptimizationExample::runMemoryStressTest() {
    profileOperation("Memory Stress Test", [this]() {
        logPerformanceEvent("Starting memory stress test...");

        // Allocate and deallocate memory in chunks
        QList<QByteArray*> allocations;

        for (int i = 0; i < 1000; ++i) {
            // Allocate 1MB chunks
            auto* data = new QByteArray(1024 * 1024, 'A' + (i % 26));
            allocations.append(data);

            // Periodically clean up some allocations
            if (i % 100 == 0 && !allocations.isEmpty()) {
                delete allocations.takeFirst();
            }

            // Update progress
            if (i % 50 == 0) {
                QCoreApplication::processEvents();
            }
        }

        // Clean up remaining allocations
        qDeleteAll(allocations);
        allocations.clear();

        logPerformanceEvent("Memory stress test completed");
    });
}

void PerformanceOptimizationExample::runGarbageCollection() {
    profileOperation("Garbage Collection", [this]() {
        logPerformanceEvent("Forcing garbage collection...");

        // In Qt/C++, we don't have automatic garbage collection,
        // but we can simulate cleanup operations

        // Clear any cached data
        m_largeDataset.clear();

        // Force update of all widgets to release any cached resources
        update();

        // Process any pending deleteLater() calls
        QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);

        logPerformanceEvent("Garbage collection completed");
    });
}

void PerformanceOptimizationExample::analyzeMemoryUsage() {
    profileOperation("Memory Analysis", [this]() {
        logPerformanceEvent("Analyzing memory usage...");

        // Simulate memory analysis
        size_t estimatedUsage = 0;

        // Estimate widget memory usage
        estimatedUsage += sizeof(*this);
        estimatedUsage += m_largeDataset.size() * sizeof(QVariantMap);

        // Estimate model memory usage
        if (m_largeDataModel) {
            estimatedUsage += m_largeDataModel->rowCount() *
                              m_largeDataModel->columnCount() *
                              64;  // Rough estimate
        }

        m_memoryUsage = estimatedUsage;
        updateMemoryDisplay();

        QString analysis =
            QString(
                "Memory analysis complete:\n"
                "Estimated usage: %1 MB\n"
                "Dataset items: %2\n"
                "Model rows: %3")
                .arg(m_memoryUsage / (1024 * 1024))
                .arg(m_largeDataset.size())
                .arg(m_largeDataModel ? m_largeDataModel->rowCount() : 0);

        m_profilingLog->append(analysis);
        logPerformanceEvent("Memory analysis completed");
    });
}

void PerformanceOptimizationExample::toggleMemoryOptimization(bool enabled) {
    m_memoryOptimizationEnabled = enabled;

    if (enabled) {
        logPerformanceEvent("Memory optimization enabled");
        // Implement memory optimization strategies
        optimizeForLowEndDevice();
    } else {
        logPerformanceEvent("Memory optimization disabled");
        optimizeForHighEndDevice();
    }
}

// Large dataset handling slot implementations
void PerformanceOptimizationExample::generateLargeDataset() {
    int itemCount = m_datasetSizeSpinBox->value();

    profileOperation("Dataset Generation", [this, itemCount]() {
        logPerformanceEvent(
            QString("Generating dataset with %1 items...").arg(itemCount));

        populateLargeDataset(itemCount);

        logPerformanceEvent(
            QString("Dataset generation completed: %1 items").arg(itemCount));
    });
}

void PerformanceOptimizationExample::enableVirtualScrolling(bool enabled) {
    m_virtualScrollingEnabled = enabled;

    if (enabled) {
        logPerformanceEvent("Virtual scrolling enabled");
        // Configure widgets for virtual scrolling
        m_largeTableWidget->setVerticalScrollMode(
            QAbstractItemView::ScrollPerPixel);
        m_largeListWidget->setVerticalScrollMode(
            QAbstractItemView::ScrollPerPixel);
    } else {
        logPerformanceEvent("Virtual scrolling disabled");
        m_largeTableWidget->setVerticalScrollMode(
            QAbstractItemView::ScrollPerItem);
        m_largeListWidget->setVerticalScrollMode(
            QAbstractItemView::ScrollPerItem);
    }
}

void PerformanceOptimizationExample::toggleLazyLoading(bool enabled) {
    m_lazyLoadingEnabled = enabled;

    if (enabled) {
        logPerformanceEvent("Lazy loading enabled");
        m_largeTreeView->setLazyLoadingEnabled(true);
    } else {
        logPerformanceEvent("Lazy loading disabled");
        m_largeTreeView->setLazyLoadingEnabled(false);
    }
}

void PerformanceOptimizationExample::benchmarkDataOperations() {
    profileOperation("Data Operations Benchmark", [this]() {
        logPerformanceEvent("Starting data operations benchmark...");

        // Test data insertion
        QElapsedTimer timer;
        timer.start();

        m_largeListWidget->clear();
        for (int i = 0; i < 10000; ++i) {
            m_largeListWidget->addItem(QString("Benchmark Item %1").arg(i));
            if (i % 1000 == 0) {
                QCoreApplication::processEvents();
            }
        }

        qint64 insertTime = timer.elapsed();

        // Test data access
        timer.restart();
        for (int i = 0; i < 1000; ++i) {
            auto* item =
                m_largeListWidget->item(QRandomGenerator::global()->bounded(
                    m_largeListWidget->count()));
            if (item) {
                QString text = item->text();  // Access data
                Q_UNUSED(text)
            }
        }
        qint64 accessTime = timer.elapsed();

        // Test data removal
        timer.restart();
        while (m_largeListWidget->count() > 5000) {
            delete m_largeListWidget->takeItem(0);
            if (m_largeListWidget->count() % 1000 == 0) {
                QCoreApplication::processEvents();
            }
        }
        qint64 removeTime = timer.elapsed();

        QString results = QString(
                              "Data Operations Benchmark Results:\n"
                              "Insert 10,000 items: %1ms\n"
                              "Access 1,000 items: %2ms\n"
                              "Remove 5,000 items: %3ms")
                              .arg(insertTime)
                              .arg(accessTime)
                              .arg(removeTime);

        m_profilingLog->append(results);
        logPerformanceEvent("Data operations benchmark completed");
    });
}

// Animation optimization slot implementations
void PerformanceOptimizationExample::toggleAnimationOptimization(bool enabled) {
    m_animationOptimizationEnabled = enabled;

    if (enabled) {
        logPerformanceEvent("Animation optimization enabled");
        // Reduce animation quality based on performance
        if (m_currentFPS < 30) {
            m_animationQualitySlider->setValue(50);
            adjustAnimationQuality(50);
        }
    } else {
        logPerformanceEvent("Animation optimization disabled");
        m_animationQualitySlider->setValue(100);
        adjustAnimationQuality(100);
    }
}

void PerformanceOptimizationExample::adjustAnimationQuality(int quality) {
    m_animationQuality = quality;
    m_animationQualityLabel->setText(QString("%1%").arg(quality));

    logPerformanceEvent(QString("Animation quality set to %1%").arg(quality));

    // Apply animation quality settings to FluentAnimator
    // This would typically involve adjusting animation durations and effects
}

void PerformanceOptimizationExample::runAnimationBenchmark() {
    profileOperation("Animation Benchmark", [this]() {
        logPerformanceEvent("Starting animation benchmark...");

        // Create test animations
        QList<QPropertyAnimation*> animations;

        for (int i = 0; i < 50; ++i) {
            auto* animation =
                new QPropertyAnimation(m_animationBenchmarkButton, "geometry");
            animation->setDuration(1000);
            animation->setStartValue(m_animationBenchmarkButton->geometry());
            animation->setEndValue(
                m_animationBenchmarkButton->geometry().adjusted(10, 10, 10,
                                                                10));
            animations.append(animation);
        }

        // Measure animation performance
        QElapsedTimer timer;
        timer.start();

        for (auto* animation : animations) {
            animation->start();
        }

        // Wait for animations to complete
        QEventLoop loop;
        QTimer::singleShot(1100, &loop, &QEventLoop::quit);
        loop.exec();

        qint64 animationTime = timer.elapsed();

        // Clean up
        qDeleteAll(animations);

        QString results = QString(
                              "Animation Benchmark Results:\n"
                              "50 concurrent animations: %1ms\n"
                              "Average FPS during test: %2")
                              .arg(animationTime)
                              .arg(m_currentFPS, 0, 'f', 1);

        m_profilingLog->append(results);
        logPerformanceEvent("Animation benchmark completed");
    });
}

// Rendering optimization slot implementations
void PerformanceOptimizationExample::toggleHardwareAcceleration(bool enabled) {
    m_hardwareAccelerationEnabled = enabled;

    if (enabled) {
        logPerformanceEvent("Hardware acceleration enabled");
        // Enable hardware acceleration for widgets
        setAttribute(Qt::WA_NativeWindow, true);
    } else {
        logPerformanceEvent("Hardware acceleration disabled");
        setAttribute(Qt::WA_NativeWindow, false);
    }
}

void PerformanceOptimizationExample::adjustRenderingQuality(int quality) {
    m_renderingQuality = quality;
    m_renderingQualityLabel->setText(QString("%1%").arg(quality));

    logPerformanceEvent(QString("Rendering quality set to %1%").arg(quality));

    // Apply rendering quality settings
    if (quality < 50) {
        m_antialiasing->setChecked(false);
        m_textureOptimization->setChecked(true);
    } else if (quality < 75) {
        m_antialiasing->setChecked(true);
        m_textureOptimization->setChecked(true);
    } else {
        m_antialiasing->setChecked(true);
        m_textureOptimization->setChecked(false);
    }
}

void PerformanceOptimizationExample::runRenderingBenchmark() {
    profileOperation("Rendering Benchmark", [this]() {
        logPerformanceEvent("Starting rendering benchmark...");

        // Force multiple repaints to test rendering performance
        QElapsedTimer timer;
        timer.start();

        for (int i = 0; i < 100; ++i) {
            update();
            QCoreApplication::processEvents();
            QThread::msleep(10);
        }

        qint64 renderTime = timer.elapsed();

        QString results = QString(
                              "Rendering Benchmark Results:\n"
                              "100 forced repaints: %1ms\n"
                              "Average render time: %2ms")
                              .arg(renderTime)
                              .arg(renderTime / 100.0, 0, 'f', 2);

        m_profilingLog->append(results);
        logPerformanceEvent("Rendering benchmark completed");
    });
}

// Threading and concurrency slot implementations
void PerformanceOptimizationExample::runMultithreadedOperation() {
    logPerformanceEvent("Starting multithreaded operation...");

    m_threadingProgressBar->setValue(0);

    // Create multiple tasks to run in parallel
    for (int i = 0; i < 10; ++i) {
        QRunnable* task = QRunnable::create([this, i]() {
            // Simulate work
            for (int j = 0; j < 1000000; ++j) {
                volatile double result = qSin(i * j * 0.001);
                Q_UNUSED(result)
            }

            // Update progress on main thread
            QMetaObject::invokeMethod(
                this,
                [this]() {
                    int currentValue = m_threadingProgressBar->value();
                    m_threadingProgressBar->setValue(currentValue + 10);

                    if (m_threadingProgressBar->value() >= 100) {
                        logPerformanceEvent(
                            "Multithreaded operation completed");
                    }
                },
                Qt::QueuedConnection);
        });

        m_threadPool->start(task);
    }
}

void PerformanceOptimizationExample::adjustThreadPoolSize(int size) {
    m_threadPoolSize = size;
    m_threadPool->setMaxThreadCount(size);

    logPerformanceEvent(QString("Thread pool size set to %1").arg(size));
}

void PerformanceOptimizationExample::runConcurrencyBenchmark() {
    profileOperation("Concurrency Benchmark", [this]() {
        logPerformanceEvent("Starting concurrency benchmark...");

        QElapsedTimer timer;
        timer.start();

        // Test with different thread counts
        QList<int> threadCounts = {1, 2, 4, 8};

        for (int threadCount : threadCounts) {
            m_threadPool->setMaxThreadCount(threadCount);

            QElapsedTimer threadTimer;
            threadTimer.start();

            // Run parallel tasks
            QAtomicInt completedTasks(0);
            const int totalTasks = 20;

            for (int i = 0; i < totalTasks; ++i) {
                QRunnable* task = QRunnable::create([&completedTasks]() {
                    // Simulate CPU-intensive work
                    volatile double result = 0;
                    for (int j = 0; j < 100000; ++j) {
                        result += qSin(j * 0.001);
                    }
                    completedTasks.fetchAndAddOrdered(1);
                });

                m_threadPool->start(task);
            }

            // Wait for all tasks to complete
            while (completedTasks.loadAcquire() < totalTasks) {
                QThread::msleep(10);
                QCoreApplication::processEvents();
            }

            qint64 threadTime = threadTimer.elapsed();

            QString result = QString("Threads: %1, Time: %2ms")
                                 .arg(threadCount)
                                 .arg(threadTime);
            m_profilingLog->append(result);
        }

        qint64 totalTime = timer.elapsed();

        QString results =
            QString("Concurrency Benchmark completed in %1ms").arg(totalTime);
        m_profilingLog->append(results);
        logPerformanceEvent("Concurrency benchmark completed");
    });
}

// Profiling and debugging slot implementations
void PerformanceOptimizationExample::startProfiling() {
    m_profilingActive = true;
    m_startProfilingButton->setEnabled(false);
    m_stopProfilingButton->setEnabled(true);

    m_profilingResults.clear();

    logPerformanceEvent("Profiling session started");
    m_profilingLog->append("=== PROFILING SESSION STARTED ===");
}

void PerformanceOptimizationExample::stopProfiling() {
    m_profilingActive = false;
    m_startProfilingButton->setEnabled(true);
    m_stopProfilingButton->setEnabled(false);

    logPerformanceEvent("Profiling session stopped");
    m_profilingLog->append("=== PROFILING SESSION STOPPED ===");

    // Display profiling summary
    QString summary = "Profiling Summary:\n";
    for (auto it = m_profilingResults.begin(); it != m_profilingResults.end();
         ++it) {
        summary += QString("%1: %2ms\n").arg(it.key()).arg(it.value());
    }
    m_profilingLog->append(summary);
}

void PerformanceOptimizationExample::exportProfilingData() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "Export Profiling Data", "profiling_data.txt",
        "Text Files (*.txt)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << m_profilingLog->toPlainText();

            logPerformanceEvent(
                QString("Profiling data exported to %1").arg(fileName));
        } else {
            QMessageBox::warning(this, "Export Error",
                                 "Failed to save profiling data.");
        }
    }
}

void PerformanceOptimizationExample::clearProfilingData() {
    m_profilingLog->clear();
    m_performanceEvents->clear();
    m_profilingResults.clear();
    m_performanceLog.clear();

    logPerformanceEvent("Profiling data cleared");
}

// Helper method implementations
void PerformanceOptimizationExample::setupPerformanceMonitoring() {
    // Connect performance monitor signals
    connect(m_performanceMonitor,
            &Core::FluentPerformanceMonitor::frameRateChanged, this,
            &PerformanceOptimizationExample::onFrameRateChanged);
    connect(m_performanceMonitor,
            &Core::FluentPerformanceMonitor::performanceWarning, this,
            &PerformanceOptimizationExample::onPerformanceWarning);
}

void PerformanceOptimizationExample::setupMemoryManagement() {
    // Initialize memory management components
    m_largeDataModel = new QStandardItemModel(this);
}

void PerformanceOptimizationExample::setupDatasetHandling() {
    // Configure tree view for large datasets
    std::vector<Components::FluentTreeColumn> columns = {
        {"Name", 200, true, true, Qt::AlignLeft},
        {"Type", 100, true, true, Qt::AlignLeft},
        {"Size", 80, true, true, Qt::AlignRight},
        {"Modified", 150, true, true, Qt::AlignLeft}};
    m_largeTreeView->setColumns(columns);

    // Configure table widget
    m_largeTableWidget->setColumnCount(4);
    QStringList headers = {"Name", "Type", "Size", "Modified"};
    m_largeTableWidget->setHorizontalHeaderLabels(headers);
}

void PerformanceOptimizationExample::setupAnimationOptimization() {
    // Animation optimization is handled in the UI setup
}

void PerformanceOptimizationExample::setupRenderingOptimization() {
    // Rendering optimization is handled in the UI setup
}

void PerformanceOptimizationExample::setupThreadingControls() {
    // Set initial thread pool configuration
    m_threadPool->setMaxThreadCount(m_threadPoolSize);
}

void PerformanceOptimizationExample::setupProfilingTools() {
    // Profiling tools are handled in the UI setup
}

void PerformanceOptimizationExample::setupConnections() {
    // Performance monitoring connections
    connect(m_monitoringEnabled, &QCheckBox::toggled, this,
            &PerformanceOptimizationExample::togglePerformanceMonitoring);
    connect(m_metricsUpdateTimer, &QTimer::timeout, this,
            &PerformanceOptimizationExample::updatePerformanceMetrics);

    // Memory management connections
    connect(m_gcButton, &Components::FluentButton::clicked, this,
            &PerformanceOptimizationExample::runGarbageCollection);
    connect(m_memoryTestButton, &Components::FluentButton::clicked, this,
            &PerformanceOptimizationExample::runMemoryStressTest);
    connect(m_memoryOptimization, &QCheckBox::toggled, this,
            &PerformanceOptimizationExample::toggleMemoryOptimization);

    // Dataset handling connections
    connect(m_generateDataButton, &Components::FluentButton::clicked, this,
            &PerformanceOptimizationExample::generateLargeDataset);
    connect(m_benchmarkButton, &Components::FluentButton::clicked, this,
            &PerformanceOptimizationExample::benchmarkDataOperations);
    connect(m_virtualScrolling, &QCheckBox::toggled, this,
            &PerformanceOptimizationExample::enableVirtualScrolling);
    connect(m_lazyLoading, &QCheckBox::toggled, this,
            &PerformanceOptimizationExample::toggleLazyLoading);

    // Animation optimization connections
    connect(m_animationOptimization, &QCheckBox::toggled, this,
            &PerformanceOptimizationExample::toggleAnimationOptimization);
    connect(m_animationQualitySlider, &QSlider::valueChanged, this,
            &PerformanceOptimizationExample::adjustAnimationQuality);
    connect(m_animationBenchmarkButton, &Components::FluentButton::clicked,
            this, &PerformanceOptimizationExample::runAnimationBenchmark);

    // Rendering optimization connections
    connect(m_hardwareAcceleration, &QCheckBox::toggled, this,
            &PerformanceOptimizationExample::toggleHardwareAcceleration);
    connect(m_renderingQualitySlider, &QSlider::valueChanged, this,
            &PerformanceOptimizationExample::adjustRenderingQuality);
    connect(m_renderingBenchmarkButton, &Components::FluentButton::clicked,
            this, &PerformanceOptimizationExample::runRenderingBenchmark);

    // Threading connections
    connect(m_threadPoolSizeSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged), this,
            &PerformanceOptimizationExample::adjustThreadPoolSize);
    connect(m_multithreadTestButton, &Components::FluentButton::clicked, this,
            &PerformanceOptimizationExample::runMultithreadedOperation);
    connect(m_concurrencyBenchmarkButton, &Components::FluentButton::clicked,
            this, &PerformanceOptimizationExample::runConcurrencyBenchmark);

    // Profiling connections
    connect(m_startProfilingButton, &Components::FluentButton::clicked, this,
            &PerformanceOptimizationExample::startProfiling);
    connect(m_stopProfilingButton, &Components::FluentButton::clicked, this,
            &PerformanceOptimizationExample::stopProfiling);
    connect(m_exportProfilingButton, &Components::FluentButton::clicked, this,
            &PerformanceOptimizationExample::exportProfilingData);
    connect(m_clearProfilingButton, &Components::FluentButton::clicked, this,
            &PerformanceOptimizationExample::clearProfilingData);
}

void PerformanceOptimizationExample::setupTheme() {
    auto& theme = Styling::FluentTheme::instance();
    // Theme will be applied automatically to FluentQt components
}

void PerformanceOptimizationExample::populateLargeDataset(int itemCount) {
    // Clear existing data
    m_largeDataset.clear();
    m_largeTreeView->clear();
    m_largeTableWidget->setRowCount(0);
    m_largeListWidget->clear();

    // Generate dataset
    for (int i = 0; i < itemCount; ++i) {
        QVariantMap item;
        item["name"] = QString("Item %1").arg(i + 1);
        item["type"] = (i % 3 == 0) ? "Folder" : "File";
        item["size"] = QRandomGenerator::global()->bounded(1024, 1024 * 1024);
        item["modified"] = QDateTime::currentDateTime().addDays(
            -QRandomGenerator::global()->bounded(365));

        m_largeDataset.append(item);

        // Add to tree view (with lazy loading consideration)
        if (!m_lazyLoadingEnabled || i < 1000) {
            auto* treeItem =
                m_largeTreeView->addTopLevelItem(item["name"].toString());
            treeItem->setText(1, item["type"].toString());
            treeItem->setText(
                2, QString("%1 KB").arg(item["size"].toInt() / 1024));
            treeItem->setText(
                3, item["modified"].toDateTime().toString("yyyy-MM-dd"));
        }

        // Add to table widget (with virtual scrolling consideration)
        if (!m_virtualScrollingEnabled || i < 1000) {
            int row = m_largeTableWidget->rowCount();
            m_largeTableWidget->insertRow(row);
            m_largeTableWidget->setItem(
                row, 0, new QTableWidgetItem(item["name"].toString()));
            m_largeTableWidget->setItem(
                row, 1, new QTableWidgetItem(item["type"].toString()));
            m_largeTableWidget->setItem(
                row, 2,
                new QTableWidgetItem(
                    QString("%1 KB").arg(item["size"].toInt() / 1024)));
            m_largeTableWidget->setItem(
                row, 3,
                new QTableWidgetItem(
                    item["modified"].toDateTime().toString("yyyy-MM-dd")));
        }

        // Add to list widget
        if (!m_virtualScrollingEnabled || i < 1000) {
            m_largeListWidget->addItem(QString("%1 (%2)").arg(
                item["name"].toString(), item["type"].toString()));
        }

        // Process events periodically to keep UI responsive
        if (i % 100 == 0) {
            QCoreApplication::processEvents();
        }
    }
}

void PerformanceOptimizationExample::profileOperation(
    const QString& operation, std::function<void()> func) {
    if (m_profilingActive) {
        m_performanceMonitor->beginProfile(operation);
    }

    QElapsedTimer timer;
    timer.start();

    func();

    qint64 elapsed = timer.elapsed();

    if (m_profilingActive) {
        m_performanceMonitor->endProfile(operation);
        m_profilingResults[operation] = elapsed;

        QString profileEntry =
            QString("[%1] %2: %3ms")
                .arg(QTime::currentTime().toString("hh:mm:ss"))
                .arg(operation)
                .arg(elapsed);
        m_profilingLog->append(profileEntry);
    }
}

void PerformanceOptimizationExample::logPerformanceEvent(const QString& event) {
    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp, event);

    m_performanceLog.append(logEntry);

    // Add to performance events list
    auto* item = new QListWidgetItem(logEntry);
    m_performanceEvents->addItem(item);

    // Auto-scroll to latest event
    m_performanceEvents->scrollToBottom();

    // Limit log size to prevent memory issues
    if (m_performanceLog.size() > 1000) {
        m_performanceLog.removeFirst();
    }

    if (m_performanceEvents->count() > 1000) {
        delete m_performanceEvents->takeItem(0);
    }
}

void PerformanceOptimizationExample::updateMemoryDisplay() {
    // Simulate memory usage (in a real app, this would use actual memory APIs)
    size_t simulatedUsage =
        m_largeDataset.size() * 64 +  // Dataset memory
        (m_largeDataModel ? m_largeDataModel->rowCount() * 256
                          : 0) +  // Model memory
        1024 * 1024 * 50;         // Base application memory (50MB)

    m_memoryUsage = simulatedUsage;

    m_memoryLabel->setText(QString("%1 MB").arg(m_memoryUsage / (1024 * 1024)));
    m_heapSizeLabel->setText(
        QString("%1 MB").arg((m_memoryUsage * 1.5) / (1024 * 1024)));
    m_allocatedLabel->setText(
        QString("%1 MB").arg(m_memoryUsage / (1024 * 1024)));
    m_availableLabel->setText(
        QString("%1 MB").arg((m_memoryUsage * 0.3) / (1024 * 1024)));

    // Update memory usage bar (assuming 500MB as maximum)
    int percentage =
        static_cast<int>((m_memoryUsage / (500.0 * 1024 * 1024)) * 100);
    m_memoryUsageBar->setValue(qMin(100, percentage));
}

void PerformanceOptimizationExample::updateFPSDisplay() {
    m_fpsLabel->setText(QString("%1 FPS").arg(m_currentFPS, 0, 'f', 1));

    // Update FPS color based on performance
    QString color = "#0078d4";  // Blue (good)
    if (m_currentFPS < 30) {
        color = "#d13438";  // Red (poor)
    } else if (m_currentFPS < 45) {
        color = "#ff8c00";  // Orange (fair)
    } else if (m_currentFPS < 55) {
        color = "#107c10";  // Green (good)
    }

    m_fpsLabel->setStyleSheet(
        QString("font-weight: bold; color: %1;").arg(color));
}

void PerformanceOptimizationExample::optimizeForLowEndDevice() {
    logPerformanceEvent("Optimizing for low-end device...");

    // Reduce animation quality
    m_animationQualitySlider->setValue(50);
    adjustAnimationQuality(50);

    // Reduce rendering quality
    m_renderingQualitySlider->setValue(50);
    adjustRenderingQuality(50);

    // Enable virtual scrolling
    m_virtualScrolling->setChecked(true);
    enableVirtualScrolling(true);

    // Enable lazy loading
    m_lazyLoading->setChecked(true);
    toggleLazyLoading(true);

    // Reduce thread pool size
    m_threadPoolSizeSpinBox->setValue(2);
    adjustThreadPoolSize(2);

    logPerformanceEvent("Low-end device optimization applied");
}

void PerformanceOptimizationExample::optimizeForHighEndDevice() {
    logPerformanceEvent("Optimizing for high-end device...");

    // Maximize animation quality
    m_animationQualitySlider->setValue(100);
    adjustAnimationQuality(100);

    // Maximize rendering quality
    m_renderingQualitySlider->setValue(100);
    adjustRenderingQuality(100);

    // Disable virtual scrolling for smoother experience
    m_virtualScrolling->setChecked(false);
    enableVirtualScrolling(false);

    // Disable lazy loading for immediate access
    m_lazyLoading->setChecked(false);
    toggleLazyLoading(false);

    // Maximize thread pool size
    m_threadPoolSizeSpinBox->setValue(QThread::idealThreadCount());
    adjustThreadPoolSize(QThread::idealThreadCount());

    logPerformanceEvent("High-end device optimization applied");
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("FluentQt Performance Optimization Example");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("FluentQt Examples");

    PerformanceOptimizationExample window;
    window.show();

    return app.exec();
}

#include "PerformanceOptimizationExample.moc"
