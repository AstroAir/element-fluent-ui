// include/FluentQt/Performance/FluentPerformanceDashboard.h
#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QScrollArea>
#include <QSplitter>
#include <QTimer>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QAreaSeries>
#include <QBarSeries>
#include <QBarSet>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QLegend>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QMutex>
#include <QMap>
#include <QQueue>
#include <memory>

#include "FluentAdvancedPerformanceAnalytics.h"

QT_CHARTS_USE_NAMESPACE

namespace FluentQt::Performance {

// Dashboard widget types
enum class FluentDashboardWidgetType {
    MetricCard,             // Single metric display card
    LineChart,              // Time series line chart
    AreaChart,              // Area chart for cumulative metrics
    BarChart,               // Bar chart for categorical data
    GaugeChart,             // Circular gauge for single values
    HeatMap,                // Heat map for correlation data
    Table,                  // Tabular data display
    Tree,                   // Hierarchical data display
    Alert,                  // Alert/notification widget
    Custom                  // Custom widget type
};

// Dashboard themes
enum class FluentDashboardTheme {
    Light,                  // Light theme
    Dark,                   // Dark theme
    HighContrast,           // High contrast theme
    System,                 // Follow system theme
    Custom                  // Custom theme
};

// Widget configuration
struct FluentDashboardWidgetConfig {
    FluentDashboardWidgetType type{FluentDashboardWidgetType::MetricCard};
    QString title;
    QString subtitle;
    QStringList metrics;
    QSize preferredSize{200, 150};
    bool isResizable{true};
    bool showLegend{true};
    bool showGrid{true};
    bool enableAnimation{true};
    std::chrono::milliseconds updateInterval{1000};
    QMap<QString, QVariant> customProperties;
};

// Dashboard layout configuration
struct FluentDashboardLayoutConfig {
    int columns{3};
    int spacing{16};
    QMargins margins{16, 16, 16, 16};
    bool enableAutoLayout{true};
    bool enableResponsiveLayout{true};
    QSize minimumWidgetSize{150, 100};
    QSize maximumWidgetSize{400, 300};
    QString layoutAlgorithm{"grid"}; // grid, flow, masonry, custom
};

// Performance metric card widget
class FluentMetricCard : public QWidget {
    Q_OBJECT

public:
    explicit FluentMetricCard(const QString& title, const QString& metric, QWidget* parent = nullptr);
    
    void setTitle(const QString& title);
    void setMetric(const QString& metric);
    void setValue(double value);
    void setUnit(const QString& unit);
    void setTrend(double trend); // Positive = up, negative = down
    void setStatus(const QString& status); // good, warning, critical
    void setIcon(const QString& iconPath);
    void enableAnimation(bool enabled = true);
    
    double getValue() const { return m_value; }
    QString getMetric() const { return m_metric; }
    QString getStatus() const { return m_status; }

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void setupUI();
    void updateDisplay();
    void animateValueChange(double newValue);
    QColor getStatusColor() const;
    QString formatValue(double value) const;

private:
    QString m_title;
    QString m_metric;
    QString m_unit;
    QString m_status{"good"};
    QString m_iconPath;
    double m_value{0.0};
    double m_previousValue{0.0};
    double m_trend{0.0};
    bool m_animationEnabled{true};
    
    QLabel* m_titleLabel;
    QLabel* m_valueLabel;
    QLabel* m_unitLabel;
    QLabel* m_trendLabel;
    QLabel* m_iconLabel;
    
    QPropertyAnimation* m_valueAnimation;
    QGraphicsDropShadowEffect* m_shadowEffect;
};

// Performance chart widget
class FluentPerformanceChart : public QChartView {
    Q_OBJECT

public:
    explicit FluentPerformanceChart(FluentDashboardWidgetType chartType, QWidget* parent = nullptr);
    
    void setTitle(const QString& title);
    void addMetric(const QString& metricName, const QColor& color = QColor());
    void removeMetric(const QString& metricName);
    void updateMetric(const QString& metricName, double value, const QDateTime& timestamp = QDateTime::currentDateTime());
    void setTimeRange(std::chrono::seconds range);
    void setMaxDataPoints(int maxPoints);
    void enableAnimation(bool enabled = true);
    void setTheme(FluentDashboardTheme theme);
    
    void clearData();
    void exportChart(const QString& filePath, const QString& format = "png");

private:
    void setupChart();
    void updateChart();
    void applyTheme();
    void cleanupOldData();

private:
    FluentDashboardWidgetType m_chartType;
    FluentDashboardTheme m_theme{FluentDashboardTheme::System};
    QChart* m_chart;
    QMap<QString, QLineSeries*> m_lineSeries;
    QMap<QString, QAreaSeries*> m_areaSeries;
    QMap<QString, QBarSeries*> m_barSeries;
    QMap<QString, QColor> m_metricColors;
    QValueAxis* m_yAxis;
    QDateTimeAxis* m_xAxis;
    
    std::chrono::seconds m_timeRange{300}; // 5 minutes
    int m_maxDataPoints{300};
    bool m_animationEnabled{true};
    
    QTimer* m_cleanupTimer;
};

// Performance table widget
class FluentPerformanceTable : public QTableWidget {
    Q_OBJECT

public:
    explicit FluentPerformanceTable(QWidget* parent = nullptr);
    
    void setMetrics(const QStringList& metrics);
    void updateMetric(const QString& metric, const QVariant& value);
    void addComponent(const QString& componentName);
    void removeComponent(const QString& componentName);
    void updateComponent(const QString& componentName, const QMap<QString, QVariant>& data);
    void setTheme(FluentDashboardTheme theme);
    void enableSorting(bool enabled = true);
    void enableFiltering(bool enabled = true);
    
    void exportData(const QString& filePath, const QString& format = "csv");

private:
    void setupTable();
    void applyTheme();
    void updateRowColors();

private:
    FluentDashboardTheme m_theme{FluentDashboardTheme::System};
    QStringList m_metrics;
    QMap<QString, int> m_componentRows;
    bool m_sortingEnabled{true};
    bool m_filteringEnabled{true};
};

// Alert notification widget
class FluentAlertWidget : public QWidget {
    Q_OBJECT

public:
    enum class AlertType {
        Info,
        Warning,
        Critical,
        Success
    };

    explicit FluentAlertWidget(AlertType type, const QString& message, QWidget* parent = nullptr);
    
    void setMessage(const QString& message);
    void setType(AlertType type);
    void setAutoHide(bool autoHide, std::chrono::milliseconds duration = std::chrono::milliseconds(5000));
    void setActionButton(const QString& text, std::function<void()> action);

signals:
    void dismissed();
    void actionTriggered();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void setupUI();
    void applyStyle();
    void animateIn();
    void animateOut();

private:
    AlertType m_type;
    QString m_message;
    bool m_autoHide{true};
    std::chrono::milliseconds m_duration{5000};
    
    QLabel* m_iconLabel;
    QLabel* m_messageLabel;
    QPushButton* m_actionButton;
    QPushButton* m_closeButton;
    
    QTimer* m_autoHideTimer;
    QPropertyAnimation* m_slideAnimation;
    QPropertyAnimation* m_opacityAnimation;
};

// Main performance dashboard
class FluentPerformanceDashboard : public QMainWindow {
    Q_OBJECT

public:
    explicit FluentPerformanceDashboard(QWidget* parent = nullptr);
    ~FluentPerformanceDashboard();
    
    // Dashboard configuration
    void setTheme(FluentDashboardTheme theme);
    FluentDashboardTheme getTheme() const { return m_theme; }
    void setLayoutConfig(const FluentDashboardLayoutConfig& config);
    FluentDashboardLayoutConfig getLayoutConfig() const { return m_layoutConfig; }
    void setUpdateInterval(std::chrono::milliseconds interval);
    
    // Widget management
    void addWidget(const QString& id, FluentDashboardWidgetType type, const FluentDashboardWidgetConfig& config);
    void removeWidget(const QString& id);
    void updateWidget(const QString& id, const FluentDashboardWidgetConfig& config);
    void moveWidget(const QString& id, int row, int column);
    void resizeWidget(const QString& id, const QSize& size);
    
    // Predefined dashboard layouts
    void loadOverviewDashboard();
    void loadDetailedDashboard();
    void loadDeveloperDashboard();
    void loadMinimalDashboard();
    void loadCustomDashboard(const QString& configPath);
    
    // Data management
    void connectToAnalytics(FluentAdvancedPerformanceAnalytics* analytics);
    void startMonitoring();
    void stopMonitoring();
    void pauseMonitoring();
    void resumeMonitoring();
    
    // Export and import
    void exportDashboard(const QString& filePath);
    void importDashboard(const QString& filePath);
    void exportData(const QString& filePath, const QString& format = "json");
    void takeScreenshot(const QString& filePath);
    
    // Customization
    void enableCustomization(bool enabled = true);
    void setCustomizationMode(bool enabled);
    void saveLayout(const QString& name);
    void loadLayout(const QString& name);
    QStringList getSavedLayouts() const;
    
    // Alerts and notifications
    void showAlert(FluentAlertWidget::AlertType type, const QString& message);
    void clearAlerts();
    void enableAlerts(bool enabled = true);
    void setAlertPosition(Qt::Corner corner);

public slots:
    void onMetricsUpdated(const FluentAdvancedMetrics& metrics);
    void onAnomalyDetected(const QString& anomaly, double severity);
    void onOptimizationFound(const QString& opportunity, double potential);
    void onSystemHealthChanged(const QString& status, double score);

private slots:
    void onUpdateTimer();
    void onThemeChanged();
    void onLayoutChanged();
    void onWidgetCustomized();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupDashboardArea();
    void applyTheme();
    void updateLayout();
    void updateAllWidgets();
    
    // Widget creation helpers
    FluentMetricCard* createMetricCard(const FluentDashboardWidgetConfig& config);
    FluentPerformanceChart* createChart(const FluentDashboardWidgetConfig& config);
    FluentPerformanceTable* createTable(const FluentDashboardWidgetConfig& config);
    QWidget* createCustomWidget(const FluentDashboardWidgetConfig& config);
    
    // Layout management
    void arrangeWidgets();
    void calculateOptimalLayout();
    void applyResponsiveLayout();
    
    // Data processing
    void processMetricsUpdate(const FluentAdvancedMetrics& metrics);
    void updateMetricCards(const FluentAdvancedMetrics& metrics);
    void updateCharts(const FluentAdvancedMetrics& metrics);
    void updateTables(const FluentAdvancedMetrics& metrics);
    
    // Alert management
    void processAlert(FluentAlertWidget::AlertType type, const QString& message);
    void positionAlert(FluentAlertWidget* alert);
    void cleanupExpiredAlerts();

private:
    FluentDashboardTheme m_theme{FluentDashboardTheme::System};
    FluentDashboardLayoutConfig m_layoutConfig;
    FluentAdvancedPerformanceAnalytics* m_analytics{nullptr};
    
    // UI components
    QWidget* m_centralWidget;
    QScrollArea* m_scrollArea;
    QWidget* m_dashboardWidget;
    QGridLayout* m_dashboardLayout;
    QSplitter* m_mainSplitter;
    
    // Widgets
    QMap<QString, QWidget*> m_widgets;
    QMap<QString, FluentDashboardWidgetConfig> m_widgetConfigs;
    QMap<QString, QPoint> m_widgetPositions;
    
    // Metric cards
    QMap<QString, FluentMetricCard*> m_metricCards;
    
    // Charts
    QMap<QString, FluentPerformanceChart*> m_charts;
    
    // Tables
    QMap<QString, FluentPerformanceTable*> m_tables;
    
    // Alerts
    QList<FluentAlertWidget*> m_activeAlerts;
    Qt::Corner m_alertPosition{Qt::TopRightCorner};
    bool m_alertsEnabled{true};
    
    // Monitoring
    bool m_monitoringActive{false};
    bool m_monitoringPaused{false};
    QTimer* m_updateTimer;
    std::chrono::milliseconds m_updateInterval{1000};
    
    // Customization
    bool m_customizationEnabled{false};
    bool m_customizationMode{false};
    QMap<QString, FluentDashboardLayoutConfig> m_savedLayouts;
    
    // Performance optimization
    QMutex m_updateMutex;
    std::atomic<bool> m_updateInProgress{false};
    
    // Menu and toolbar actions
    QAction* m_startAction;
    QAction* m_stopAction;
    QAction* m_pauseAction;
    QAction* m_exportAction;
    QAction* m_importAction;
    QAction* m_customizeAction;
    QAction* m_themeAction;
    QAction* m_fullscreenAction;
};

// Dashboard factory for creating predefined dashboards
class FluentDashboardFactory {
public:
    static FluentPerformanceDashboard* createOverviewDashboard(QWidget* parent = nullptr);
    static FluentPerformanceDashboard* createDetailedDashboard(QWidget* parent = nullptr);
    static FluentPerformanceDashboard* createDeveloperDashboard(QWidget* parent = nullptr);
    static FluentPerformanceDashboard* createMinimalDashboard(QWidget* parent = nullptr);
    static FluentPerformanceDashboard* createCustomDashboard(const QString& configPath, QWidget* parent = nullptr);
    
    // Widget factory methods
    static FluentMetricCard* createFPSCard(QWidget* parent = nullptr);
    static FluentMetricCard* createMemoryCard(QWidget* parent = nullptr);
    static FluentMetricCard* createCPUCard(QWidget* parent = nullptr);
    static FluentPerformanceChart* createPerformanceChart(QWidget* parent = nullptr);
    static FluentPerformanceTable* createComponentTable(QWidget* parent = nullptr);
    
    // Configuration helpers
    static FluentDashboardWidgetConfig createMetricCardConfig(const QString& title, const QString& metric);
    static FluentDashboardWidgetConfig createChartConfig(const QString& title, const QStringList& metrics);
    static FluentDashboardWidgetConfig createTableConfig(const QString& title, const QStringList& columns);
};

// Dashboard utility functions
namespace FluentDashboardUtils {
    // Theme utilities
    QColor getThemeColor(FluentDashboardTheme theme, const QString& colorRole);
    QString getThemeStyleSheet(FluentDashboardTheme theme);
    QFont getThemeFont(FluentDashboardTheme theme, const QString& fontRole);
    
    // Layout utilities
    QSize calculateOptimalWidgetSize(const QSize& containerSize, int widgetCount, int columns);
    QPoint calculateWidgetPosition(int index, int columns, const QSize& widgetSize, int spacing);
    int calculateOptimalColumns(const QSize& containerSize, const QSize& widgetSize);
    
    // Data formatting utilities
    QString formatMetricValue(double value, const QString& unit);
    QString formatTrend(double trend);
    QString formatDuration(std::chrono::milliseconds duration);
    QString formatBytes(qint64 bytes);
    QString formatPercentage(double percentage);
    
    // Export utilities
    void exportWidgetAsImage(QWidget* widget, const QString& filePath);
    void exportChartAsImage(QChart* chart, const QString& filePath);
    void exportTableAsCSV(QTableWidget* table, const QString& filePath);
    void exportDashboardAsHTML(FluentPerformanceDashboard* dashboard, const QString& filePath);
}

} // namespace FluentQt::Performance
