// examples/DataVisualizationExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QTimer>
#include <QRandomGenerator>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QSplitter>
#include <QProgressBar>
#include <QSlider>
#include <QTime>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Components/FluentChartView.h"
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Animation/FluentAnimator.h"

using namespace FluentQt;

class DataVisualizationExample : public QMainWindow {
    Q_OBJECT

public:
    DataVisualizationExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupData();
        setupConnections();
        setupTheme();
        startDataSimulation();
    }

private slots:
    void updateChartData();
    void onChartTypeChanged();
    void onDataSourceChanged();
    void onTreeItemClicked(const QModelIndex& index);
    void refreshData();
    void exportData();
    void toggleRealTimeUpdates();
    void onDataPointsChanged(int points);
    void onUpdateIntervalChanged(int interval);
    void toggleTheme();

private:
    void setupUI();
    void setupData();
    void setupConnections();
    void setupTheme();
    void startDataSimulation();
    void createControlPanel();
    void createChartArea();
    void createDataTreeView();
    void createStatusPanel();
    void populateTreeModel();
    void generateRandomData();
    void updateTreeData();
    void animateChartUpdate();

private:
    // UI Components
    QWidget* m_centralWidget{nullptr};
    QSplitter* m_mainSplitter{nullptr};
    QSplitter* m_rightSplitter{nullptr};
    
    // Control Panel
    QGroupBox* m_controlGroup{nullptr};
    Components::FluentComboBox* m_chartTypeCombo{nullptr};
    Components::FluentComboBox* m_dataSourceCombo{nullptr};
    QSpinBox* m_dataPointsSpin{nullptr};
    QSlider* m_updateIntervalSlider{nullptr};
    Components::FluentButton* m_refreshButton{nullptr};
    Components::FluentButton* m_exportButton{nullptr};
    Components::FluentButton* m_realTimeButton{nullptr};
    Components::FluentButton* m_themeButton{nullptr};
    
    // Chart Area
    Components::FluentPanel* m_chartPanel{nullptr};
    Components::FluentChartView* m_chartView{nullptr};
    QLabel* m_chartTitleLabel{nullptr};
    
    // Data Tree View
    Components::FluentPanel* m_treePanel{nullptr};
    Components::FluentTreeView* m_treeView{nullptr};
    QStandardItemModel* m_treeModel{nullptr};
    
    // Status Panel
    Components::FluentPanel* m_statusPanel{nullptr};
    QLabel* m_statusLabel{nullptr};
    QProgressBar* m_dataProgress{nullptr};
    QLabel* m_dataCountLabel{nullptr};
    QLabel* m_lastUpdateLabel{nullptr};
    
    // Data and Animation
    QTimer* m_updateTimer{nullptr};
    QVector<QPointF> m_chartData;
    Animation::FluentAnimator* m_animator{nullptr};
    bool m_realTimeEnabled{true};
    int m_currentDataPoints{50};
    QString m_currentChartType{"Line Chart"};
    QString m_currentDataSource{"Sales Data"};
};

void DataVisualizationExample::setupUI() {
    setWindowTitle("FluentQt Data Visualization Example");
    setMinimumSize(1200, 800);
    
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    // Create main splitter for layout
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    auto* mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->addWidget(m_mainSplitter);
    
    createControlPanel();
    createChartArea();
    createDataTreeView();
    createStatusPanel();
    
    // Setup splitter layout
    auto* leftWidget = new QWidget();
    auto* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->addWidget(m_controlGroup);
    leftLayout->addWidget(m_statusPanel);
    leftLayout->addStretch();
    
    m_rightSplitter = new QSplitter(Qt::Vertical);
    m_rightSplitter->addWidget(m_chartPanel);
    m_rightSplitter->addWidget(m_treePanel);
    
    m_mainSplitter->addWidget(leftWidget);
    m_mainSplitter->addWidget(m_rightSplitter);
    
    // Set splitter proportions
    m_mainSplitter->setSizes({300, 900});
    m_rightSplitter->setSizes({500, 300});
    
    // Initialize animator
    m_animator = new Animation::FluentAnimator(this);
}

void DataVisualizationExample::createControlPanel() {
    m_controlGroup = new QGroupBox("Data Controls", this);
    auto* layout = new QGridLayout(m_controlGroup);
    
    // Chart type selection
    layout->addWidget(new QLabel("Chart Type:"), 0, 0);
    m_chartTypeCombo = new Components::FluentComboBox(this);
    m_chartTypeCombo->addItems({"Line Chart", "Bar Chart", "Area Chart", "Scatter Plot", "Pie Chart"});
    layout->addWidget(m_chartTypeCombo, 0, 1);
    
    // Data source selection
    layout->addWidget(new QLabel("Data Source:"), 1, 0);
    m_dataSourceCombo = new Components::FluentComboBox(this);
    m_dataSourceCombo->addItems({"Sales Data", "Performance Metrics", "User Analytics", "Financial Data", "System Metrics"});
    layout->addWidget(m_dataSourceCombo, 1, 1);
    
    // Data points control
    layout->addWidget(new QLabel("Data Points:"), 2, 0);
    m_dataPointsSpin = new QSpinBox(this);
    m_dataPointsSpin->setRange(10, 200);
    m_dataPointsSpin->setValue(50);
    layout->addWidget(m_dataPointsSpin, 2, 1);
    
    // Update interval control
    layout->addWidget(new QLabel("Update Interval:"), 3, 0);
    m_updateIntervalSlider = new QSlider(Qt::Horizontal, this);
    m_updateIntervalSlider->setRange(100, 5000);
    m_updateIntervalSlider->setValue(1000);
    layout->addWidget(m_updateIntervalSlider, 3, 1);
    
    // Control buttons
    auto* buttonLayout = new QVBoxLayout();
    
    m_refreshButton = new Components::FluentButton("Refresh Data", this);
    m_refreshButton->setButtonStyle(Components::FluentButtonStyle::Primary);
    buttonLayout->addWidget(m_refreshButton);
    
    m_exportButton = new Components::FluentButton("Export Data", this);
    m_exportButton->setButtonStyle(Components::FluentButtonStyle::Accent);
    buttonLayout->addWidget(m_exportButton);
    
    m_realTimeButton = new Components::FluentButton("Pause Updates", this);
    m_realTimeButton->setButtonStyle(Components::FluentButtonStyle::Subtle);
    buttonLayout->addWidget(m_realTimeButton);
    
    m_themeButton = new Components::FluentButton("Toggle Theme", this);
    buttonLayout->addWidget(m_themeButton);
    
    layout->addLayout(buttonLayout, 4, 0, 1, 2);
}

void DataVisualizationExample::createChartArea() {
    m_chartPanel = new Components::FluentPanel("Data Visualization", this);
    m_chartPanel->setPanelType(Components::FluentPanelType::Card);
    m_chartPanel->setElevation(Components::FluentPanelElevation::Medium);
    
    auto* layout = m_chartPanel->createVerticalLayout();
    
    // Chart title
    m_chartTitleLabel = new QLabel("Sales Data - Line Chart", this);
    auto& theme = Styling::FluentTheme::instance();
    m_chartTitleLabel->setFont(theme.subtitleFont());
    m_chartTitleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_chartTitleLabel);
    
    // Chart view
    m_chartView = new Components::FluentChartView(this);
    m_chartView->setMinimumHeight(400);
    layout->addWidget(m_chartView);
}

void DataVisualizationExample::createDataTreeView() {
    m_treePanel = new Components::FluentPanel("Data Explorer", this);
    m_treePanel->setPanelType(Components::FluentPanelType::Surface);
    
    auto* layout = m_treePanel->createVerticalLayout();
    
    // Tree view
    m_treeView = new Components::FluentTreeView(this);
    m_treeView->setMinimumHeight(200);
    layout->addWidget(m_treeView);
    
    // Setup tree model - FluentTreeView uses its own item management
    // We'll populate it directly using FluentTreeView methods
}

void DataVisualizationExample::createStatusPanel() {
    m_statusPanel = new Components::FluentPanel("Status", this);
    m_statusPanel->setPanelType(Components::FluentPanelType::Surface);
    
    auto* layout = m_statusPanel->createVerticalLayout();
    
    m_statusLabel = new QLabel("Ready", this);
    layout->addWidget(m_statusLabel);
    
    m_dataProgress = new QProgressBar(this);
    m_dataProgress->setVisible(false);
    layout->addWidget(m_dataProgress);
    
    m_dataCountLabel = new QLabel("Data Points: 0", this);
    layout->addWidget(m_dataCountLabel);
    
    m_lastUpdateLabel = new QLabel("Last Update: Never", this);
    layout->addWidget(m_lastUpdateLabel);
}

void DataVisualizationExample::setupData() {
    generateRandomData();
    populateTreeModel();
}

void DataVisualizationExample::setupConnections() {
    // Chart type and data source changes
    connect(m_chartTypeCombo, &Components::FluentComboBox::currentIndexChanged,
            this, &DataVisualizationExample::onChartTypeChanged);
    connect(m_dataSourceCombo, &Components::FluentComboBox::currentIndexChanged,
            this, &DataVisualizationExample::onDataSourceChanged);

    // Data controls
    connect(m_dataPointsSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &DataVisualizationExample::onDataPointsChanged);
    connect(m_updateIntervalSlider, &QSlider::valueChanged,
            this, &DataVisualizationExample::onUpdateIntervalChanged);

    // Buttons
    connect(m_refreshButton, &Components::FluentButton::clicked,
            this, &DataVisualizationExample::refreshData);
    connect(m_exportButton, &Components::FluentButton::clicked,
            this, &DataVisualizationExample::exportData);
    connect(m_realTimeButton, &Components::FluentButton::clicked,
            this, &DataVisualizationExample::toggleRealTimeUpdates);
    connect(m_themeButton, &Components::FluentButton::clicked,
            this, &DataVisualizationExample::toggleTheme);

    // Tree view
    connect(m_treeView, &Components::FluentTreeView::itemClicked,
            this, [this](Components::FluentTreeItem* item, int column) {
                Q_UNUSED(column)
                if (item) {
                    m_statusLabel->setText(QString("Selected: %1").arg(item->text(0)));
                    // Animate selection with scale effect
                    Animation::FluentAnimationConfig pulseConfig;
                    pulseConfig.duration = std::chrono::milliseconds(200);
                    auto pulseAnimation = Animation::FluentAnimator::scaleIn(m_treeView, pulseConfig);
                    pulseAnimation->start();
                }
            });
}

void DataVisualizationExample::setupTheme() {
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);
    theme.setAccentColor(Styling::FluentAccentColor::Blue);
}

void DataVisualizationExample::startDataSimulation() {
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &DataVisualizationExample::updateChartData);
    m_updateTimer->start(1000); // Start with 1 second interval
}

void DataVisualizationExample::generateRandomData() {
    m_chartData.clear();
    auto* generator = QRandomGenerator::global();

    for (int i = 0; i < m_currentDataPoints; ++i) {
        qreal x = i;
        qreal y = 50 + generator->bounded(100) - 50; // Random value between 0-100

        // Add some trend based on data source
        if (m_currentDataSource == "Sales Data") {
            y += i * 0.5; // Upward trend
        } else if (m_currentDataSource == "Performance Metrics") {
            y += qSin(i * 0.1) * 20; // Sinusoidal pattern
        } else if (m_currentDataSource == "User Analytics") {
            y += generator->bounded(20) - 10; // More random
        }

        m_chartData.append(QPointF(x, qMax(0.0, qMin(100.0, y))));
    }

    // Update chart
    if (m_chartView) {
        // Create a series for the chart data
        Components::FluentChartSeries series;
        series.name = m_currentDataSource;
        series.data = m_chartData.toList();
        series.color = QColor(0, 120, 215); // Blue color

        // Map chart type string to enum
        if (m_currentChartType == "Line Chart") {
            series.type = Components::FluentChartType::Line;
        } else if (m_currentChartType == "Bar Chart") {
            series.type = Components::FluentChartType::Bar;
        } else if (m_currentChartType == "Area Chart") {
            series.type = Components::FluentChartType::Area;
        } else if (m_currentChartType == "Scatter Plot") {
            series.type = Components::FluentChartType::Scatter;
        } else if (m_currentChartType == "Pie Chart") {
            series.type = Components::FluentChartType::Pie;
        } else {
            series.type = Components::FluentChartType::Line;
        }

        // Clear existing series and add new one
        m_chartView->clearSeries();
        m_chartView->addSeries(series);
        animateChartUpdate();
    }

    // Update status
    m_dataCountLabel->setText(QString("Data Points: %1").arg(m_chartData.size()));
    m_lastUpdateLabel->setText(QString("Last Update: %1").arg(QTime::currentTime().toString()));
}

void DataVisualizationExample::populateTreeModel() {
    // Clear existing items
    m_treeView->clear();

    // Set up columns
    std::vector<Components::FluentTreeColumn> columns = {
        {"Name", 150, true, true, Qt::AlignLeft},
        {"Value", 100, true, true, Qt::AlignRight},
        {"Type", 80, true, true, Qt::AlignCenter},
        {"Last Updated", 120, true, true, Qt::AlignCenter}
    };
    m_treeView->setColumns(columns);

    // Create data categories
    auto* salesRoot = m_treeView->addTopLevelItem("Sales Data");
    auto* performanceRoot = m_treeView->addTopLevelItem("Performance Metrics");
    auto* analyticsRoot = m_treeView->addTopLevelItem("User Analytics");

    // Add sales data items
    for (int i = 0; i < 5; ++i) {
        auto* item = m_treeView->addChildItem(salesRoot, QString("Sales Region %1").arg(i + 1));
        item->setText(1, QString("$%1K").arg(QRandomGenerator::global()->bounded(100, 500)));
        item->setText(2, "Currency");
        item->setText(3, QTime::currentTime().toString());
    }

    // Add performance metrics
    QStringList metrics = {"CPU Usage", "Memory Usage", "Disk I/O", "Network Latency", "Response Time"};
    for (int i = 0; i < metrics.size(); ++i) {
        auto* item = m_treeView->addChildItem(performanceRoot, metrics[i]);
        item->setText(1, QString("%1%").arg(QRandomGenerator::global()->bounded(10, 95)));
        item->setText(2, "Percentage");
        item->setText(3, QTime::currentTime().toString());
    }

    // Add analytics data
    QStringList analyticsItems = {"Page Views", "Unique Visitors", "Bounce Rate", "Session Duration", "Conversion Rate"};
    for (int i = 0; i < analyticsItems.size(); ++i) {
        auto* item = m_treeView->addChildItem(analyticsRoot, analyticsItems[i]);
        item->setText(1, QString("%1").arg(QRandomGenerator::global()->bounded(1000, 10000)));
        item->setText(2, "Count");
        item->setText(3, QTime::currentTime().toString());
    }

    // Expand all items
    m_treeView->expandAll();
}

void DataVisualizationExample::updateChartData() {
    if (!m_realTimeEnabled) return;

    // Show progress during update
    m_dataProgress->setVisible(true);
    m_dataProgress->setValue(0);
    m_statusLabel->setText("Updating data...");

    // Simulate data loading with progress
    QTimer::singleShot(100, [this]() {
        m_dataProgress->setValue(33);
        QTimer::singleShot(100, [this]() {
            m_dataProgress->setValue(66);
            QTimer::singleShot(100, [this]() {
                m_dataProgress->setValue(100);

                // Generate new data
                generateRandomData();
                updateTreeData();

                // Hide progress and update status
                QTimer::singleShot(200, [this]() {
                    m_dataProgress->setVisible(false);
                    m_statusLabel->setText("Data updated successfully");
                });
            });
        });
    });
}

void DataVisualizationExample::updateTreeData() {
    // Update existing tree data with new values
    for (int i = 0; i < m_treeModel->rowCount(); ++i) {
        auto* rootItem = m_treeModel->item(i);
        for (int j = 0; j < rootItem->rowCount(); ++j) {
            auto* valueItem = rootItem->child(j, 1);
            auto* updatedItem = rootItem->child(j, 3);

            if (valueItem && updatedItem) {
                // Generate new random value based on type
                QString currentValue = valueItem->text();
                if (currentValue.contains("$")) {
                    valueItem->setText(QString("$%1K").arg(QRandomGenerator::global()->bounded(100, 500)));
                } else if (currentValue.contains("%")) {
                    valueItem->setText(QString("%1%").arg(QRandomGenerator::global()->bounded(10, 95)));
                } else {
                    valueItem->setText(QString("%1").arg(QRandomGenerator::global()->bounded(1000, 10000)));
                }
                updatedItem->setText(QTime::currentTime().toString());
            }
        }
    }
}

void DataVisualizationExample::animateChartUpdate() {
    if (m_chartView) {
        // Animate chart appearance with simple fade effect
        Animation::FluentAnimationConfig config;
        config.duration = std::chrono::milliseconds(150);
        config.easing = Animation::FluentEasing::EaseOutQuad;

        auto fadeOut = Animation::FluentAnimator::fadeOut(m_chartView, config);
        connect(fadeOut.get(), &QPropertyAnimation::finished, [this]() {
            Animation::FluentAnimationConfig fadeInConfig;
            fadeInConfig.duration = std::chrono::milliseconds(200);
            fadeInConfig.easing = Animation::FluentEasing::EaseInQuad;

            auto fadeIn = Animation::FluentAnimator::fadeIn(m_chartView, fadeInConfig);
            fadeIn->start();
        });
        fadeOut->start();
    }
}

// Slot implementations
void DataVisualizationExample::onChartTypeChanged() {
    m_currentChartType = m_chartTypeCombo->currentText();
    m_chartTitleLabel->setText(QString("%1 - %2").arg(m_currentDataSource, m_currentChartType));

    if (m_chartView) {
        // Update chart by regenerating data with new chart type
        updateChartData();
        animateChartUpdate();
    }
}

void DataVisualizationExample::onDataSourceChanged() {
    m_currentDataSource = m_dataSourceCombo->currentText();
    m_chartTitleLabel->setText(QString("%1 - %2").arg(m_currentDataSource, m_currentChartType));
    generateRandomData();
}

void DataVisualizationExample::onTreeItemClicked(const QModelIndex& index) {
    // This method is no longer used since we handle tree clicks with a lambda
    // in setupConnections(), but keeping it for potential future use
    Q_UNUSED(index)
}

void DataVisualizationExample::refreshData() {
    m_statusLabel->setText("Refreshing all data...");
    generateRandomData();
    populateTreeModel();

    // Animate refresh button
    Animation::FluentAnimationConfig pressConfig;
    pressConfig.duration = std::chrono::milliseconds(100);
    auto pressAnimation = Animation::FluentAnimator::scaleOut(m_refreshButton, pressConfig);
    pressAnimation->start();
}

void DataVisualizationExample::exportData() {
    m_statusLabel->setText("Exporting data to CSV...");

    // Simulate export process
    QTimer::singleShot(1000, [this]() {
        m_statusLabel->setText("Data exported successfully!");
    });

    // Animate export button
    m_animator->pulseEffect(m_exportButton);
}

void DataVisualizationExample::toggleRealTimeUpdates() {
    m_realTimeEnabled = !m_realTimeEnabled;

    if (m_realTimeEnabled) {
        m_realTimeButton->setText("Pause Updates");
        m_realTimeButton->setButtonStyle(Components::FluentButtonStyle::Subtle);
        m_updateTimer->start();
        m_statusLabel->setText("Real-time updates enabled");
    } else {
        m_realTimeButton->setText("Resume Updates");
        m_realTimeButton->setButtonStyle(Components::FluentButtonStyle::Primary);
        m_updateTimer->stop();
        m_statusLabel->setText("Real-time updates paused");
    }
}

void DataVisualizationExample::onDataPointsChanged(int points) {
    m_currentDataPoints = points;
    generateRandomData();
}

void DataVisualizationExample::onUpdateIntervalChanged(int interval) {
    if (m_updateTimer) {
        m_updateTimer->setInterval(interval);
    }
}

void DataVisualizationExample::toggleTheme() {
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
    theme.setAccentColor(Styling::FluentAccentColor::Blue);

    DataVisualizationExample example;
    example.show();

    return app.exec();
}
