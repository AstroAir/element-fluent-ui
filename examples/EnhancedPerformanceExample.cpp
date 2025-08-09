// examples/EnhancedPerformanceExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTextEdit>
#include <QTimer>
#include <QGroupBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QCheckBox>

#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Core/FluentLazyComponent.h"
#include "FluentQt/Core/FluentBundleAnalyzer.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"

using namespace FluentQt;

// Example heavy component for lazy loading demonstration
class HeavyComponent : public QWidget {
    Q_OBJECT

public:
    explicit HeavyComponent(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        simulateHeavyInitialization();
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        auto* label = new QLabel("Heavy Component Loaded!", this);
        label->setStyleSheet("font-size: 18px; font-weight: bold; color: #0078d4;");
        
        auto* textEdit = new QTextEdit(this);
        textEdit->setPlainText("This is a heavy component that takes time to load.\n"
                              "It simulates a complex widget with lots of data processing.");
        
        layout->addWidget(label);
        layout->addWidget(textEdit);
    }
    
    void simulateHeavyInitialization() {
        // Simulate heavy initialization
        QTimer::singleShot(1000, this, [this]() {
            qDebug() << "Heavy component initialization completed";
        });
    }
};

class PerformanceMonitorWidget : public QWidget {
    Q_OBJECT

public:
    explicit PerformanceMonitorWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        connectSignals();
        startMonitoring();
    }

private slots:
    void updateMetrics(const Core::PerformanceMetrics& metrics) {
        m_fpsLabel->setText(QString("FPS: %1").arg(metrics.frameRate, 0, 'f', 1));
        m_memoryLabel->setText(QString("Memory: %1 MB").arg(metrics.memoryUsage / (1024 * 1024)));
        m_cpuLabel->setText(QString("CPU: %1%").arg(metrics.cpuUsage, 0, 'f', 1));
        
        m_fpsBar->setValue(static_cast<int>(metrics.frameRate));
        m_memoryBar->setValue(static_cast<int>(metrics.memoryUsage / (1024 * 1024)));
        m_cpuBar->setValue(static_cast<int>(metrics.cpuUsage));
        
        // Update performance score
        auto& monitor = Core::FluentPerformanceMonitor::instance();
        double score = monitor.calculateOverallPerformanceScore();
        m_scoreLabel->setText(QString("Performance Score: %1/100").arg(score, 0, 'f', 1));
        
        // Update suggestions
        auto suggestions = monitor.generateGlobalOptimizationSuggestions();
        m_suggestionsText->clear();
        for (const QString& suggestion : suggestions) {
            m_suggestionsText->append("• " + suggestion);
        }
    }
    
    void onPerformanceWarning(const QString& message) {
        m_warningsText->append(QString("[WARNING] %1").arg(message));
    }
    
    void onPerformanceCritical(const QString& message) {
        m_warningsText->append(QString("[CRITICAL] %1").arg(message));
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        // Performance metrics group
        auto* metricsGroup = new QGroupBox("Performance Metrics", this);
        auto* metricsLayout = new QGridLayout(metricsGroup);
        
        m_fpsLabel = new QLabel("FPS: 0.0", this);
        m_memoryLabel = new QLabel("Memory: 0 MB", this);
        m_cpuLabel = new QLabel("CPU: 0%", this);
        m_scoreLabel = new QLabel("Performance Score: 100/100", this);
        
        m_fpsBar = new QProgressBar(this);
        m_fpsBar->setRange(0, 60);
        m_memoryBar = new QProgressBar(this);
        m_memoryBar->setRange(0, 1024); // 1GB
        m_cpuBar = new QProgressBar(this);
        m_cpuBar->setRange(0, 100);
        
        metricsLayout->addWidget(m_fpsLabel, 0, 0);
        metricsLayout->addWidget(m_fpsBar, 0, 1);
        metricsLayout->addWidget(m_memoryLabel, 1, 0);
        metricsLayout->addWidget(m_memoryBar, 1, 1);
        metricsLayout->addWidget(m_cpuLabel, 2, 0);
        metricsLayout->addWidget(m_cpuBar, 2, 1);
        metricsLayout->addWidget(m_scoreLabel, 3, 0, 1, 2);
        
        // Optimization suggestions group
        auto* suggestionsGroup = new QGroupBox("Optimization Suggestions", this);
        auto* suggestionsLayout = new QVBoxLayout(suggestionsGroup);
        
        m_suggestionsText = new QTextEdit(this);
        m_suggestionsText->setMaximumHeight(100);
        m_suggestionsText->setReadOnly(true);
        suggestionsLayout->addWidget(m_suggestionsText);
        
        // Warnings group
        auto* warningsGroup = new QGroupBox("Performance Warnings", this);
        auto* warningsLayout = new QVBoxLayout(warningsGroup);
        
        m_warningsText = new QTextEdit(this);
        m_warningsText->setMaximumHeight(100);
        m_warningsText->setReadOnly(true);
        warningsLayout->addWidget(m_warningsText);
        
        layout->addWidget(metricsGroup);
        layout->addWidget(suggestionsGroup);
        layout->addWidget(warningsGroup);
    }
    
    void connectSignals() {
        auto& monitor = Core::FluentPerformanceMonitor::instance();
        
        connect(&monitor, &Core::FluentPerformanceMonitor::metricsUpdated,
                this, &PerformanceMonitorWidget::updateMetrics);
        connect(&monitor, &Core::FluentPerformanceMonitor::performanceWarning,
                this, &PerformanceMonitorWidget::onPerformanceWarning);
        connect(&monitor, &Core::FluentPerformanceMonitor::performanceCritical,
                this, &PerformanceMonitorWidget::onPerformanceCritical);
    }
    
    void startMonitoring() {
        auto& monitor = Core::FluentPerformanceMonitor::instance();
        monitor.startFrameRateMonitoring();
        monitor.startCpuMonitoring();
        monitor.startMemoryPressureMonitoring();
    }

private:
    QLabel* m_fpsLabel;
    QLabel* m_memoryLabel;
    QLabel* m_cpuLabel;
    QLabel* m_scoreLabel;
    QProgressBar* m_fpsBar;
    QProgressBar* m_memoryBar;
    QProgressBar* m_cpuBar;
    QTextEdit* m_suggestionsText;
    QTextEdit* m_warningsText;
};

class LazyLoadingDemoWidget : public QWidget {
    Q_OBJECT

public:
    explicit LazyLoadingDemoWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setupUI();
        setupLazyComponents();
    }

private slots:
    void loadHeavyComponent() {
        auto* widget = m_lazyHeavyComponent->get();
        if (widget && !m_componentLoaded) {
            m_componentLayout->addWidget(widget);
            m_componentLoaded = true;
            m_loadButton->setText("Component Loaded");
            m_loadButton->setEnabled(false);
        }
    }
    
    void onComponentReady(HeavyComponent* component) {
        Q_UNUSED(component)
        qDebug() << "Heavy component is ready for use";
    }

private:
    void setupUI() {
        auto* layout = new QVBoxLayout(this);
        
        auto* titleLabel = new QLabel("Lazy Loading Demonstration", this);
        titleLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
        
        m_loadButton = new QPushButton("Load Heavy Component", this);
        connect(m_loadButton, &QPushButton::clicked, this, &LazyLoadingDemoWidget::loadHeavyComponent);
        
        auto* componentGroup = new QGroupBox("Lazy Loaded Component", this);
        m_componentLayout = new QVBoxLayout(componentGroup);
        
        auto* placeholderLabel = new QLabel("Component will be loaded here...", this);
        placeholderLabel->setAlignment(Qt::AlignCenter);
        placeholderLabel->setStyleSheet("color: #666; font-style: italic;");
        m_componentLayout->addWidget(placeholderLabel);
        
        layout->addWidget(titleLabel);
        layout->addWidget(m_loadButton);
        layout->addWidget(componentGroup);
        layout->addStretch();
    }
    
    void setupLazyComponents() {
        // Configure lazy loading
        Core::FluentLazyLoadConfig config;
        config.strategy = Core::FluentLazyLoadStrategy::OnDemand;
        config.enableCaching = true;
        config.loadDelay = std::chrono::milliseconds(500);
        
        // Create lazy component
        m_lazyHeavyComponent = new Core::FluentLazyComponent<HeavyComponent>("heavy_component", config, this);
        
        connect(m_lazyHeavyComponent, &Core::FluentLazyComponent<HeavyComponent>::componentReady,
                this, &LazyLoadingDemoWidget::onComponentReady);
    }

private:
    QPushButton* m_loadButton;
    QVBoxLayout* m_componentLayout;
    Core::FluentLazyComponent<HeavyComponent>* m_lazyHeavyComponent;
    bool m_componentLoaded = false;
};

class EnhancedPerformanceMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit EnhancedPerformanceMainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupBundleAnalyzer();
    }

private:
    void setupUI() {
        setWindowTitle("Enhanced Performance Monitoring Demo");
        setMinimumSize(800, 600);
        
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto* layout = new QHBoxLayout(centralWidget);
        
        // Left side: Performance monitoring
        auto* performanceWidget = new PerformanceMonitorWidget(this);
        
        // Right side: Lazy loading demo
        auto* lazyLoadingWidget = new LazyLoadingDemoWidget(this);
        
        layout->addWidget(performanceWidget, 1);
        layout->addWidget(lazyLoadingWidget, 1);
    }
    
    void setupBundleAnalyzer() {
        auto& analyzer = Core::FluentBundleAnalyzer::instance();
        
        // Register some example modules
        analyzer.registerModule("FluentCore", "src/Core/", 150 * 1024);
        analyzer.registerModule("FluentComponents", "src/Components/", 300 * 1024);
        analyzer.registerModule("FluentStyling", "src/Styling/", 100 * 1024);
        analyzer.registerModule("FluentAnimation", "src/Animation/", 80 * 1024);
        
        // Mark core modules
        analyzer.markModuleAsCore("FluentCore", true);
        
        // Set up dependencies
        analyzer.setModuleDependencies("FluentComponents", {"FluentCore", "FluentStyling"});
        analyzer.setModuleDependencies("FluentAnimation", {"FluentCore"});
        
        // Analyze bundle
        auto metrics = analyzer.analyzeBundleSize();
        qDebug() << "Bundle analysis completed. Total size:" << metrics.totalSize << "bytes";
        
        // Generate optimization suggestions
        auto suggestions = analyzer.generateOptimizationSuggestions();
        qDebug() << "Generated" << suggestions.size() << "optimization suggestions";
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    EnhancedPerformanceMainWindow window;
    window.show();
    
    return app.exec();
}

#include "EnhancedPerformanceExample.moc"
