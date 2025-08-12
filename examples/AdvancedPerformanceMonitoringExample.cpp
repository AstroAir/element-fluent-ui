// examples/AdvancedPerformanceMonitoringExample.cpp
#include <QApplication>
#include <QChart>
#include <QChartView>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeAxis>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineSeries>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QValueAxis>

#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Performance/FluentAdvancedPerformanceAnalytics.h"
#include "FluentQt/Performance/FluentPerformanceDashboard.h"
#include "FluentQt/Performance/FluentSystemIntegration.h"

using namespace FluentQt;

class PerformanceControlPanel : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceControlPanel(QWidget* parent = nullptr)
        : QWidget(parent) {
        setupUI();
        connectSignals();
        initializeControls();
    }

signals:
    void monitoringLevelChanged(Performance::FluentMonitoringLevel level);
    void analyticsConfigChanged(
        const Performance::FluentAnalyticsConfig& config);
    void systemConfigChanged(
        const Performance::FluentSystemIntegrationConfig& config);
    void dashboardThemeChanged(Performance::FluentDashboardTheme theme);

private slots:
    void onMonitoringLevelChanged() {
        auto level = static_cast<Performance::FluentMonitoringLevel>(
            m_monitoringLevelCombo->currentData().toInt());
        emit monitoringLevelChanged(level);
    }

    void onAnalyticsConfigChanged() {
        Performance::FluentAnalyticsConfig config;
        config.enableAnomalyDetection = m_anomalyDetectionCheck->isChecked();
        config.enablePatternRecognition =
            m_patternRecognitionCheck->isChecked();
        config.enableOptimizationAnalysis =
            m_optimizationAnalysisCheck->isChecked();
        config.enableMachineLearning = m_machineLearningCheck->isChecked();
        config.predictionHorizon =
            std::chrono::seconds(m_predictionHorizonSpin->value());
        config.anomalyThreshold = m_anomalyThresholdSlider->value() / 10.0;

        emit analyticsConfigChanged(config);
    }

    void onSystemConfigChanged() {
        Performance::FluentSystemIntegrationConfig config;
        config.monitorCPU = m_monitorCPUCheck->isChecked();
        config.monitorMemory = m_monitorMemoryCheck->isChecked();
        config.monitorDisk = m_monitorDiskCheck->isChecked();
        config.monitorNetwork = m_monitorNetworkCheck->isChecked();
        config.monitorGPU = m_monitorGPUCheck->isChecked();
        config.monitorBattery = m_monitorBatteryCheck->isChecked();
        config.updateInterval =
            std::chrono::milliseconds(m_updateIntervalSpin->value());

        emit systemConfigChanged(config);
    }

    void onDashboardThemeChanged() {
        auto theme = static_cast<Performance::FluentDashboardTheme>(
            m_themeCombo->currentData().toInt());
        emit dashboardThemeChanged(theme);
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // Monitoring Level
        auto* monitoringGroup = new QGroupBox("Monitoring Level", this);
        auto* monitoringLayout = new QVBoxLayout(monitoringGroup);

        m_monitoringLevelCombo = new QComboBox(this);
        m_monitoringLevelCombo->addItem(
            "Basic",
            static_cast<int>(Performance::FluentMonitoringLevel::Basic));
        m_monitoringLevelCombo->addItem(
            "Standard",
            static_cast<int>(Performance::FluentMonitoringLevel::Standard));
        m_monitoringLevelCombo->addItem(
            "Detailed",
            static_cast<int>(Performance::FluentMonitoringLevel::Detailed));
        m_monitoringLevelCombo->addItem(
            "Comprehensive",
            static_cast<int>(
                Performance::FluentMonitoringLevel::Comprehensive));
        m_monitoringLevelCombo->addItem(
            "Debug",
            static_cast<int>(Performance::FluentMonitoringLevel::Debug));

        monitoringLayout->addWidget(m_monitoringLevelCombo);

        // Analytics Configuration
        auto* analyticsGroup = new QGroupBox("Analytics Configuration", this);
        auto* analyticsLayout = new QVBoxLayout(analyticsGroup);

        m_anomalyDetectionCheck =
            new QCheckBox("Enable Anomaly Detection", this);
        m_patternRecognitionCheck =
            new QCheckBox("Enable Pattern Recognition", this);
        m_optimizationAnalysisCheck =
            new QCheckBox("Enable Optimization Analysis", this);
        m_machineLearningCheck = new QCheckBox("Enable Machine Learning", this);

        auto* predictionLayout = new QHBoxLayout();
        predictionLayout->addWidget(
            new QLabel("Prediction Horizon (s):", this));
        m_predictionHorizonSpin = new QSpinBox(this);
        m_predictionHorizonSpin->setRange(10, 3600);
        m_predictionHorizonSpin->setValue(60);
        predictionLayout->addWidget(m_predictionHorizonSpin);

        auto* thresholdLayout = new QHBoxLayout();
        thresholdLayout->addWidget(new QLabel("Anomaly Threshold:", this));
        m_anomalyThresholdSlider = new QSlider(Qt::Horizontal, this);
        m_anomalyThresholdSlider->setRange(10, 50);
        m_anomalyThresholdSlider->setValue(20);
        m_anomalyThresholdLabel = new QLabel("2.0", this);
        thresholdLayout->addWidget(m_anomalyThresholdSlider);
        thresholdLayout->addWidget(m_anomalyThresholdLabel);

        analyticsLayout->addWidget(m_anomalyDetectionCheck);
        analyticsLayout->addWidget(m_patternRecognitionCheck);
        analyticsLayout->addWidget(m_optimizationAnalysisCheck);
        analyticsLayout->addWidget(m_machineLearningCheck);
        analyticsLayout->addLayout(predictionLayout);
        analyticsLayout->addLayout(thresholdLayout);

        // System Monitoring Configuration
        auto* systemGroup = new QGroupBox("System Monitoring", this);
        auto* systemLayout = new QVBoxLayout(systemGroup);

        m_monitorCPUCheck = new QCheckBox("Monitor CPU", this);
        m_monitorMemoryCheck = new QCheckBox("Monitor Memory", this);
        m_monitorDiskCheck = new QCheckBox("Monitor Disk", this);
        m_monitorNetworkCheck = new QCheckBox("Monitor Network", this);
        m_monitorGPUCheck = new QCheckBox("Monitor GPU", this);
        m_monitorBatteryCheck = new QCheckBox("Monitor Battery", this);

        auto* intervalLayout = new QHBoxLayout();
        intervalLayout->addWidget(new QLabel("Update Interval (ms):", this));
        m_updateIntervalSpin = new QSpinBox(this);
        m_updateIntervalSpin->setRange(100, 10000);
        m_updateIntervalSpin->setValue(1000);
        intervalLayout->addWidget(m_updateIntervalSpin);

        systemLayout->addWidget(m_monitorCPUCheck);
        systemLayout->addWidget(m_monitorMemoryCheck);
        systemLayout->addWidget(m_monitorDiskCheck);
        systemLayout->addWidget(m_monitorNetworkCheck);
        systemLayout->addWidget(m_monitorGPUCheck);
        systemLayout->addWidget(m_monitorBatteryCheck);
        systemLayout->addLayout(intervalLayout);

        // Dashboard Theme
        auto* themeGroup = new QGroupBox("Dashboard Theme", this);
        auto* themeLayout = new QVBoxLayout(themeGroup);

        m_themeCombo = new QComboBox(this);
        m_themeCombo->addItem(
            "Light",
            static_cast<int>(Performance::FluentDashboardTheme::Light));
        m_themeCombo->addItem(
            "Dark", static_cast<int>(Performance::FluentDashboardTheme::Dark));
        m_themeCombo->addItem(
            "High Contrast",
            static_cast<int>(Performance::FluentDashboardTheme::HighContrast));
        m_themeCombo->addItem(
            "System",
            static_cast<int>(Performance::FluentDashboardTheme::System));

        themeLayout->addWidget(m_themeCombo);

        layout->addWidget(monitoringGroup);
        layout->addWidget(analyticsGroup);
        layout->addWidget(systemGroup);
        layout->addWidget(themeGroup);
        layout->addStretch();
    }

    void connectSignals() {
        connect(m_monitoringLevelCombo,
                QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                &PerformanceControlPanel::onMonitoringLevelChanged);

        connect(m_anomalyDetectionCheck, &QCheckBox::toggled, this,
                &PerformanceControlPanel::onAnalyticsConfigChanged);
        connect(m_patternRecognitionCheck, &QCheckBox::toggled, this,
                &PerformanceControlPanel::onAnalyticsConfigChanged);
        connect(m_optimizationAnalysisCheck, &QCheckBox::toggled, this,
                &PerformanceControlPanel::onAnalyticsConfigChanged);
        connect(m_machineLearningCheck, &QCheckBox::toggled, this,
                &PerformanceControlPanel::onAnalyticsConfigChanged);
        connect(m_predictionHorizonSpin,
                QOverload<int>::of(&QSpinBox::valueChanged), this,
                &PerformanceControlPanel::onAnalyticsConfigChanged);

        connect(m_anomalyThresholdSlider, &QSlider::valueChanged,
                [this](int value) {
                    m_anomalyThresholdLabel->setText(
                        QString::number(value / 10.0, 'f', 1));
                    onAnalyticsConfigChanged();
                });

        connect(m_monitorCPUCheck, &QCheckBox::toggled, this,
                &PerformanceControlPanel::onSystemConfigChanged);
        connect(m_monitorMemoryCheck, &QCheckBox::toggled, this,
                &PerformanceControlPanel::onSystemConfigChanged);
        connect(m_monitorDiskCheck, &QCheckBox::toggled, this,
                &PerformanceControlPanel::onSystemConfigChanged);
        connect(m_monitorNetworkCheck, &QCheckBox::toggled, this,
                &PerformanceControlPanel::onSystemConfigChanged);
        connect(m_monitorGPUCheck, &QCheckBox::toggled, this,
                &PerformanceControlPanel::onSystemConfigChanged);
        connect(m_monitorBatteryCheck, &QCheckBox::toggled, this,
                &PerformanceControlPanel::onSystemConfigChanged);
        connect(m_updateIntervalSpin,
                QOverload<int>::of(&QSpinBox::valueChanged), this,
                &PerformanceControlPanel::onSystemConfigChanged);

        connect(m_themeCombo,
                QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                &PerformanceControlPanel::onDashboardThemeChanged);
    }

    void initializeControls() {
        // Set default values
        m_monitoringLevelCombo->setCurrentIndex(1);  // Standard
        m_anomalyDetectionCheck->setChecked(true);
        m_patternRecognitionCheck->setChecked(true);
        m_optimizationAnalysisCheck->setChecked(true);
        m_machineLearningCheck->setChecked(false);

        m_monitorCPUCheck->setChecked(true);
        m_monitorMemoryCheck->setChecked(true);
        m_monitorDiskCheck->setChecked(true);
        m_monitorNetworkCheck->setChecked(true);
        m_monitorGPUCheck->setChecked(true);
        m_monitorBatteryCheck->setChecked(true);

        m_themeCombo->setCurrentIndex(3);  // System
    }

private:
    // Monitoring Level
    QComboBox* m_monitoringLevelCombo;

    // Analytics Configuration
    QCheckBox* m_anomalyDetectionCheck;
    QCheckBox* m_patternRecognitionCheck;
    QCheckBox* m_optimizationAnalysisCheck;
    QCheckBox* m_machineLearningCheck;
    QSpinBox* m_predictionHorizonSpin;
    QSlider* m_anomalyThresholdSlider;
    QLabel* m_anomalyThresholdLabel;

    // System Monitoring
    QCheckBox* m_monitorCPUCheck;
    QCheckBox* m_monitorMemoryCheck;
    QCheckBox* m_monitorDiskCheck;
    QCheckBox* m_monitorNetworkCheck;
    QCheckBox* m_monitorGPUCheck;
    QCheckBox* m_monitorBatteryCheck;
    QSpinBox* m_updateIntervalSpin;

    // Dashboard Theme
    QComboBox* m_themeCombo;
};

class SystemInfoWidget : public QWidget {
    Q_OBJECT

public:
    explicit SystemInfoWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        updateSystemInfo();
    }

public slots:
    void updateSystemInfo() {
        auto& systemManager =
            Performance::FluentSystemIntegrationManager::instance();
        auto systemInfo = systemManager.getSystemInfo();

        m_cpuLabel->setText(QString("CPU: %1 (%2 cores, %3 threads)")
                                .arg(systemInfo.cpuModel)
                                .arg(systemInfo.cpuCores)
                                .arg(systemInfo.cpuThreads));

        m_memoryLabel->setText(
            QString("Memory: %1 GB")
                .arg(systemInfo.totalMemory / (1024.0 * 1024.0 * 1024.0), 0,
                     'f', 1));

        m_osLabel->setText(QString("OS: %1 %2 (%3)")
                               .arg(systemInfo.osName)
                               .arg(systemInfo.osVersion)
                               .arg(systemInfo.osArchitecture));

        m_gpuLabel->setText(
            QString("GPU: %1 (%2 GB)")
                .arg(systemInfo.gpuModel)
                .arg(systemInfo.gpuMemory / (1024.0 * 1024.0 * 1024.0), 0, 'f',
                     1));

        // Update disk information
        m_diskTree->clear();
        for (auto it = systemInfo.diskSizes.begin();
             it != systemInfo.diskSizes.end(); ++it) {
            auto* item = new QTreeWidgetItem(m_diskTree);
            item->setText(0, it.key());
            item->setText(
                1, QString("%1 GB").arg(it.value() / (1024.0 * 1024.0 * 1024.0),
                                        0, 'f', 1));
            item->setText(
                2, QString("%1 GB").arg(systemInfo.diskFreeSpace[it.key()] /
                                            (1024.0 * 1024.0 * 1024.0),
                                        0, 'f', 1));
            item->setText(3, systemInfo.diskTypes[it.key()]);
        }

        // Update network information
        m_networkTree->clear();
        for (const auto& interface : systemInfo.networkInterfaces) {
            auto* item = new QTreeWidgetItem(m_networkTree);
            item->setText(0, interface);
            item->setText(1, systemInfo.networkTypes[interface]);
            item->setText(2, systemInfo.networkStatus[interface]
                                 ? "Connected"
                                 : "Disconnected");
        }
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);

        // System Information
        auto* systemGroup = new QGroupBox("System Information", this);
        auto* systemLayout = new QVBoxLayout(systemGroup);

        m_cpuLabel = new QLabel("CPU: Loading...", this);
        m_memoryLabel = new QLabel("Memory: Loading...", this);
        m_osLabel = new QLabel("OS: Loading...", this);
        m_gpuLabel = new QLabel("GPU: Loading...", this);

        systemLayout->addWidget(m_cpuLabel);
        systemLayout->addWidget(m_memoryLabel);
        systemLayout->addWidget(m_osLabel);
        systemLayout->addWidget(m_gpuLabel);

        // Disk Information
        auto* diskGroup = new QGroupBox("Disk Information", this);
        auto* diskLayout = new QVBoxLayout(diskGroup);

        m_diskTree = new QTreeWidget(this);
        m_diskTree->setHeaderLabels({"Drive", "Size", "Free", "Type"});
        diskLayout->addWidget(m_diskTree);

        // Network Information
        auto* networkGroup = new QGroupBox("Network Information", this);
        auto* networkLayout = new QVBoxLayout(networkGroup);

        m_networkTree = new QTreeWidget(this);
        m_networkTree->setHeaderLabels({"Interface", "Type", "Status"});
        networkLayout->addWidget(m_networkTree);

        layout->addWidget(systemGroup);
        layout->addWidget(diskGroup);
        layout->addWidget(networkGroup);
    }

private:
    QLabel* m_cpuLabel;
    QLabel* m_memoryLabel;
    QLabel* m_osLabel;
    QLabel* m_gpuLabel;
    QTreeWidget* m_diskTree;
    QTreeWidget* m_networkTree;
};

class AdvancedPerformanceMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AdvancedPerformanceMainWindow(QWidget* parent = nullptr)
        : QMainWindow(parent) {
        setupUI();
        setupMenuBar();
        setupToolBar();
        setupStatusBar();
        connectSignals();
        initializeMonitoring();
    }

private slots:
    void onStartMonitoring() {
        m_analytics->startAnalytics();
        m_systemManager->startMonitoring();
        m_dashboard->startMonitoring();

        m_startAction->setEnabled(false);
        m_stopAction->setEnabled(true);
        m_pauseAction->setEnabled(true);

        statusBar()->showMessage("Performance monitoring started");
    }

    void onStopMonitoring() {
        m_analytics->stopAnalytics();
        m_systemManager->stopMonitoring();
        m_dashboard->stopMonitoring();

        m_startAction->setEnabled(true);
        m_stopAction->setEnabled(false);
        m_pauseAction->setEnabled(false);

        statusBar()->showMessage("Performance monitoring stopped");
    }

    void onPauseMonitoring() {
        m_systemManager->pauseMonitoring();
        m_dashboard->pauseMonitoring();

        m_pauseAction->setText(m_systemManager->isMonitoring() ? "Resume"
                                                               : "Pause");
        statusBar()->showMessage(m_systemManager->isMonitoring()
                                     ? "Performance monitoring resumed"
                                     : "Performance monitoring paused");
    }

    void onExportData() {
        QString fileName = QFileDialog::getSaveFileName(
            this, "Export Performance Data", "", "JSON Files (*.json)");
        if (!fileName.isEmpty()) {
            m_analytics->exportAnalyticsData(fileName);
            m_systemManager->exportMetrics(
                fileName.replace(".json", "_system.json"));
            QMessageBox::information(this, "Export Complete",
                                     "Performance data exported successfully.");
        }
    }

    void onImportData() {
        QString fileName = QFileDialog::getOpenFileName(
            this, "Import Performance Data", "", "JSON Files (*.json)");
        if (!fileName.isEmpty()) {
            m_analytics->importAnalyticsData(fileName);
            QMessageBox::information(this, "Import Complete",
                                     "Performance data imported successfully.");
        }
    }

    void onAnalyticsUpdated(const Performance::FluentAdvancedMetrics& metrics) {
        // Update status bar with key metrics
        statusBar()->showMessage(
            QString("FPS: %1 | CPU: %2% | Memory: %3% | Score: %4")
                .arg(metrics.baseMetrics.frameRate, 0, 'f', 1)
                .arg(metrics.baseMetrics.cpuUsage, 0, 'f', 1)
                .arg(metrics.baseMetrics.memoryUsage / (1024.0 * 1024.0), 0,
                     'f', 1)
                .arg(metrics.userExperienceScore, 0, 'f', 1));
    }

    void onAnomalyDetected(const QString& anomaly, double severity) {
        QString message = QString("Anomaly detected: %1 (Severity: %2)")
                              .arg(anomaly)
                              .arg(severity, 0, 'f', 2);
        m_dashboard->showAlert(
            Performance::FluentAlertWidget::AlertType::Warning, message);

        if (m_systemTrayIcon && m_systemTrayIcon->isVisible()) {
            m_systemTrayIcon->showMessage("Performance Alert", message,
                                          QSystemTrayIcon::Warning, 5000);
        }
    }

    void onOptimizationFound(const QString& opportunity, double potential) {
        QString message =
            QString("Optimization opportunity: %1 (Potential: %2% improvement)")
                .arg(opportunity)
                .arg(potential, 0, 'f', 1);
        m_dashboard->showAlert(Performance::FluentAlertWidget::AlertType::Info,
                               message);
    }

private:
    void setupUI() {
        setWindowTitle("FluentQt Advanced Performance Monitoring");
        setMinimumSize(1400, 900);

        // Create main splitter
        m_mainSplitter = new QSplitter(Qt::Horizontal, this);
        setCentralWidget(m_mainSplitter);

        // Left panel - Control and system info
        auto* leftWidget = new QWidget(this);
        leftWidget->setMaximumWidth(350);
        auto* leftLayout = new QVBoxLayout(leftWidget);

        // Control panel
        m_controlPanel = new PerformanceControlPanel(this);
        leftLayout->addWidget(m_controlPanel);

        // System info
        m_systemInfoWidget = new SystemInfoWidget(this);
        leftLayout->addWidget(m_systemInfoWidget);

        // Right panel - Dashboard
        m_dashboard =
            Performance::FluentDashboardFactory::createDetailedDashboard(this);

        m_mainSplitter->addWidget(leftWidget);
        m_mainSplitter->addWidget(m_dashboard);
        m_mainSplitter->setStretchFactor(0, 0);
        m_mainSplitter->setStretchFactor(1, 1);
    }

    void setupMenuBar() {
        auto* fileMenu = menuBar()->addMenu("&File");

        m_exportAction = fileMenu->addAction("&Export Data...");
        m_exportAction->setShortcut(QKeySequence::Save);
        connect(m_exportAction, &QAction::triggered, this,
                &AdvancedPerformanceMainWindow::onExportData);

        m_importAction = fileMenu->addAction("&Import Data...");
        m_importAction->setShortcut(QKeySequence::Open);
        connect(m_importAction, &QAction::triggered, this,
                &AdvancedPerformanceMainWindow::onImportData);

        fileMenu->addSeparator();
        fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);

        auto* monitoringMenu = menuBar()->addMenu("&Monitoring");

        m_startAction = monitoringMenu->addAction("&Start");
        m_startAction->setShortcut(QKeySequence("Ctrl+S"));
        connect(m_startAction, &QAction::triggered, this,
                &AdvancedPerformanceMainWindow::onStartMonitoring);

        m_stopAction = monitoringMenu->addAction("S&top");
        m_stopAction->setShortcut(QKeySequence("Ctrl+T"));
        m_stopAction->setEnabled(false);
        connect(m_stopAction, &QAction::triggered, this,
                &AdvancedPerformanceMainWindow::onStopMonitoring);

        m_pauseAction = monitoringMenu->addAction("&Pause");
        m_pauseAction->setShortcut(QKeySequence("Ctrl+P"));
        m_pauseAction->setEnabled(false);
        connect(m_pauseAction, &QAction::triggered, this,
                &AdvancedPerformanceMainWindow::onPauseMonitoring);
    }

    void setupToolBar() {
        auto* toolBar = addToolBar("Main");
        toolBar->addAction(m_startAction);
        toolBar->addAction(m_stopAction);
        toolBar->addAction(m_pauseAction);
        toolBar->addSeparator();
        toolBar->addAction(m_exportAction);
        toolBar->addAction(m_importAction);
    }

    void setupStatusBar() {
        statusBar()->showMessage("Ready");

        // System tray icon
        if (QSystemTrayIcon::isSystemTrayAvailable()) {
            m_systemTrayIcon = new QSystemTrayIcon(this);
            m_systemTrayIcon->setIcon(QIcon(":/icons/performance.png"));
            m_systemTrayIcon->setToolTip("FluentQt Performance Monitor");
            m_systemTrayIcon->show();
        }
    }

    void connectSignals() {
        // Control panel signals
        connect(m_controlPanel,
                &PerformanceControlPanel::analyticsConfigChanged,
                [this](const Performance::FluentAnalyticsConfig& config) {
                    m_analytics->setAnalyticsConfig(config);
                });

        connect(
            m_controlPanel, &PerformanceControlPanel::systemConfigChanged,
            [this](const Performance::FluentSystemIntegrationConfig& config) {
                m_systemManager->setConfig(config);
            });

        connect(m_controlPanel, &PerformanceControlPanel::dashboardThemeChanged,
                [this](Performance::FluentDashboardTheme theme) {
                    m_dashboard->setTheme(theme);
                });

        // Analytics signals
        connect(
            m_analytics,
            &Performance::FluentAdvancedPerformanceAnalytics::analyticsUpdated,
            this, &AdvancedPerformanceMainWindow::onAnalyticsUpdated);
        connect(
            m_analytics,
            &Performance::FluentAdvancedPerformanceAnalytics::anomalyDetected,
            this, &AdvancedPerformanceMainWindow::onAnomalyDetected);
        connect(m_analytics,
                &Performance::FluentAdvancedPerformanceAnalytics::
                    optimizationOpportunityFound,
                this, &AdvancedPerformanceMainWindow::onOptimizationFound);

        // System info update timer
        auto* systemInfoTimer = new QTimer(this);
        connect(systemInfoTimer, &QTimer::timeout, m_systemInfoWidget,
                &SystemInfoWidget::updateSystemInfo);
        systemInfoTimer->start(30000);  // Update every 30 seconds
    }

    void initializeMonitoring() {
        // Initialize analytics
        m_analytics =
            &Performance::FluentAdvancedPerformanceAnalytics::instance();
        m_analytics->enableAnalytics(true);

        // Initialize system manager
        m_systemManager =
            &Performance::FluentSystemIntegrationManager::instance();

        // Connect analytics to system manager
        m_systemManager->connectToAnalytics(m_analytics);
        m_dashboard->connectToAnalytics(m_analytics);

        // Load detailed dashboard
        m_dashboard->loadDetailedDashboard();
    }

private:
    QSplitter* m_mainSplitter;
    PerformanceControlPanel* m_controlPanel;
    SystemInfoWidget* m_systemInfoWidget;
    Performance::FluentPerformanceDashboard* m_dashboard;

    Performance::FluentAdvancedPerformanceAnalytics* m_analytics;
    Performance::FluentSystemIntegrationManager* m_systemManager;

    QSystemTrayIcon* m_systemTrayIcon{nullptr};

    // Actions
    QAction* m_startAction;
    QAction* m_stopAction;
    QAction* m_pauseAction;
    QAction* m_exportAction;
    QAction* m_importAction;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("FluentQt Advanced Performance Monitor");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("FluentQt");

    AdvancedPerformanceMainWindow window;
    window.show();

    return app.exec();
}

#include "AdvancedPerformanceMonitoringExample.moc"
