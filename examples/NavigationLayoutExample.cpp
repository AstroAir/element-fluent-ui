// examples/NavigationLayoutExample.cpp
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QProgressBar>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Components/FluentNavigationView.h"
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentSplitter.h"
#include "FluentQt/Components/FluentTabView.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class NavigationLayoutExample : public QMainWindow {
    Q_OBJECT

public:
    NavigationLayoutExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupNavigationItems();
        setupTabContent();
        setupConnections();
        setupTheme();
        loadSampleData();
    }

private slots:
    void onNavigationSelectionChanged(int index);
    void onTabChanged(int index);
    void onSplitterMoved(int pos, int index);
    void toggleNavigationPane();
    void addNewTab();
    void closeTab(int index);
    void refreshDashboard();
    void updateLayoutSettings();
    void animateContentTransition();

private:
    void setupUI();
    void setupNavigationItems();
    void setupTabContent();
    void setupConnections();
    void setupTheme();
    void loadSampleData();

    // Content creation methods
    QWidget* createDashboardContent();
    QWidget* createDocumentsContent();
    QWidget* createSettingsContent();
    QWidget* createAnalyticsContent();
    QWidget* createHelpContent();

    // Tab content creation
    QWidget* createOverviewTab();
    QWidget* createDataTab();
    QWidget* createChartsTab();
    QWidget* createReportsTab();

    // Helper methods
    void createSampleChart(QWidget* parent);
    void createSampleTable(QWidget* parent);
    void createSampleForm(QWidget* parent);
    void updateDashboardData();

private:
    // Main Navigation
    Components::FluentNavigationView* m_navigationView{nullptr};

    // Tab Views for different sections
    Components::FluentTabView* m_dashboardTabs{nullptr};
    Components::FluentTabView* m_documentsTabs{nullptr};

    // Splitters for layout management
    Components::FluentSplitter* m_mainSplitter{nullptr};
    Components::FluentSplitter* m_contentSplitter{nullptr};
    Components::FluentSplitter* m_detailSplitter{nullptr};

    // Content widgets
    QWidget* m_dashboardContent{nullptr};
    QWidget* m_documentsContent{nullptr};
    QWidget* m_settingsContent{nullptr};
    QWidget* m_analyticsContent{nullptr};
    QWidget* m_helpContent{nullptr};

    // Dashboard components
    QLabel* m_welcomeLabel{nullptr};
    QLabel* m_statsLabel{nullptr};
    QProgressBar* m_progressBar{nullptr};
    QListWidget* m_recentItems{nullptr};
    QTextEdit* m_notesArea{nullptr};

    // Settings components
    Components::FluentComboBox* m_themeCombo{nullptr};
    Components::FluentCheckBox* m_animationsCheck{nullptr};
    QSlider* m_opacitySlider{nullptr};
    QSpinBox* m_refreshInterval{nullptr};

    // Data and timers
    QTimer* m_dashboardTimer{nullptr};
    QTimer* m_animationTimer{nullptr};
    Animation::FluentAnimator* m_animator{nullptr};

    // State
    int m_currentNavigationIndex{0};
    bool m_animationsEnabled{true};
    double m_contentOpacity{1.0};
};

void NavigationLayoutExample::setupUI() {
    setWindowTitle("FluentQt Navigation and Layout Example");
    setMinimumSize(1200, 800);
    resize(1400, 900);

    // Create main navigation view
    m_navigationView = new Components::FluentNavigationView(this);
    m_navigationView->setHeader("FluentQt Demo");
    m_navigationView->setDisplayMode(
        Components::FluentNavigationDisplayMode::Auto);
    m_navigationView->setBackButtonVisible(false);
    m_navigationView->setSettingsVisible(true);

    setCentralWidget(m_navigationView);

    // Initialize animator
    m_animator = new Animation::FluentAnimator(this);

    // Setup timers
    m_dashboardTimer = new QTimer(this);
    m_dashboardTimer->setInterval(5000);  // Update every 5 seconds

    m_animationTimer = new QTimer(this);
    m_animationTimer->setInterval(100);
    m_animationTimer->setSingleShot(true);
}

void NavigationLayoutExample::setupNavigationItems() {
    // Dashboard item
    Components::FluentNavigationItem dashboardItem;
    dashboardItem.text = "Dashboard";
    dashboardItem.icon = QIcon(":/icons/dashboard.png");
    dashboardItem.tag = "dashboard";
    dashboardItem.content = createDashboardContent();
    m_navigationView->addNavigationItem(dashboardItem);

    // Documents item
    Components::FluentNavigationItem documentsItem;
    documentsItem.text = "Documents";
    documentsItem.icon = QIcon(":/icons/documents.png");
    documentsItem.tag = "documents";
    documentsItem.content = createDocumentsContent();
    m_navigationView->addNavigationItem(documentsItem);

    // Analytics item
    Components::FluentNavigationItem analyticsItem;
    analyticsItem.text = "Analytics";
    analyticsItem.icon = QIcon(":/icons/analytics.png");
    analyticsItem.tag = "analytics";
    analyticsItem.content = createAnalyticsContent();
    m_navigationView->addNavigationItem(analyticsItem);

    // Settings item
    Components::FluentNavigationItem settingsItem;
    settingsItem.text = "Settings";
    settingsItem.icon = QIcon(":/icons/settings.png");
    settingsItem.tag = "settings";
    settingsItem.content = createSettingsContent();
    m_navigationView->addNavigationItem(settingsItem);

    // Help item
    Components::FluentNavigationItem helpItem;
    helpItem.text = "Help";
    helpItem.icon = QIcon(":/icons/help.png");
    helpItem.tag = "help";
    helpItem.content = createHelpContent();
    m_navigationView->addNavigationItem(helpItem);

    // Set initial selection
    m_navigationView->setSelectedIndex(0);
}

QWidget* NavigationLayoutExample::createDashboardContent() {
    m_dashboardContent = new QWidget();
    auto* layout = new QVBoxLayout(m_dashboardContent);

    // Create dashboard tabs
    m_dashboardTabs = new Components::FluentTabView(m_dashboardContent);
    m_dashboardTabs->setTabPosition(Components::FluentTabPosition::Top);
    m_dashboardTabs->setTabStyle(Components::FluentTabStyle::Standard);
    m_dashboardTabs->setShowAddButton(true);
    m_dashboardTabs->setTabsClosable(true);

    // Add initial tabs
    Components::FluentTabData overviewTab;
    overviewTab.text = "Overview";
    overviewTab.icon = QIcon(":/icons/overview.png");
    overviewTab.content = createOverviewTab();
    overviewTab.closeable = false;  // Don't allow closing the main tab
    m_dashboardTabs->addTab(overviewTab);

    Components::FluentTabData dataTab;
    dataTab.text = "Data";
    dataTab.icon = QIcon(":/icons/data.png");
    dataTab.content = createDataTab();
    m_dashboardTabs->addTab(dataTab);

    Components::FluentTabData chartsTab;
    chartsTab.text = "Charts";
    chartsTab.icon = QIcon(":/icons/chart.png");
    chartsTab.content = createChartsTab();
    m_dashboardTabs->addTab(chartsTab);

    layout->addWidget(m_dashboardTabs);
    return m_dashboardContent;
}

QWidget* NavigationLayoutExample::createDocumentsContent() {
    m_documentsContent = new QWidget();
    auto* layout = new QVBoxLayout(m_documentsContent);

    // Create documents tabs
    m_documentsTabs = new Components::FluentTabView(m_documentsContent);
    m_documentsTabs->setTabPosition(Components::FluentTabPosition::Top);
    m_documentsTabs->setTabStyle(Components::FluentTabStyle::Pill);

    // Create main splitter for documents area
    m_contentSplitter =
        new Components::FluentSplitter(Qt::Horizontal, m_documentsContent);
    m_contentSplitter->setCollapsible(true);
    m_contentSplitter->setAnimatedResize(true);

    // Left panel - File browser
    auto* filePanel =
        new Components::FluentPanel("File Browser", m_documentsContent);
    filePanel->setPanelType(Components::FluentPanelType::Sidebar);
    auto* filePanelLayout = filePanel->createVerticalLayout();

    auto* fileTree = new QTreeWidget(filePanel);
    fileTree->setHeaderLabel("Documents");
    fileTree->setMinimumWidth(200);

    // Add sample file structure
    auto* rootItem = new QTreeWidgetItem(fileTree, QStringList("My Documents"));
    auto* projectsItem = new QTreeWidgetItem(rootItem, QStringList("Projects"));
    new QTreeWidgetItem(projectsItem, QStringList("FluentQt Demo"));
    new QTreeWidgetItem(projectsItem, QStringList("Mobile App"));
    auto* reportsItem = new QTreeWidgetItem(rootItem, QStringList("Reports"));
    new QTreeWidgetItem(reportsItem, QStringList("Q1 Analysis"));
    new QTreeWidgetItem(reportsItem, QStringList("User Feedback"));

    fileTree->expandAll();
    filePanelLayout->addWidget(fileTree);

    // Right panel - Document content
    auto* contentPanel =
        new Components::FluentPanel("Document Content", m_documentsContent);
    contentPanel->setPanelType(Components::FluentPanelType::Content);
    auto* contentPanelLayout = contentPanel->createVerticalLayout();

    auto* documentArea = new QTextEdit(contentPanel);
    documentArea->setPlainText(
        "Welcome to the Documents section!\n\n"
        "This area demonstrates advanced layout capabilities using "
        "FluentSplitter components.\n\n"
        "Features shown:\n"
        "• Resizable panels with animated transitions\n"
        "• Collapsible sidebar with file browser\n"
        "• Responsive layout that adapts to window size\n"
        "• Smooth splitter handles with hover effects\n"
        "• Multiple tab styles and positions\n\n"
        "Try resizing the panels by dragging the splitter handles, or "
        "collapse/expand panels using the controls.");
    contentPanelLayout->addWidget(documentArea);

    m_contentSplitter->addWidget(filePanel);
    m_contentSplitter->addWidget(contentPanel);
    m_contentSplitter->setSizes({250, 550});

    // Add documents tab
    Components::FluentTabData documentsTab;
    documentsTab.text = "Documents";
    documentsTab.content = m_contentSplitter;
    documentsTab.closeable = false;
    m_documentsTabs->addTab(documentsTab);

    layout->addWidget(m_documentsTabs);
    return m_documentsContent;
}

QWidget* NavigationLayoutExample::createSettingsContent() {
    m_settingsContent = new QWidget();
    auto* layout = new QVBoxLayout(m_settingsContent);

    // Create settings panel
    auto* settingsPanel =
        new Components::FluentPanel("Application Settings", m_settingsContent);
    settingsPanel->setPanelType(Components::FluentPanelType::Card);
    settingsPanel->setElevation(Components::FluentPanelElevation::Medium);

    auto* settingsLayout = settingsPanel->createVerticalLayout();

    // Theme settings
    auto* themeGroup = new QGroupBox("Theme Settings", settingsPanel);
    auto* themeLayout = new QGridLayout(themeGroup);

    themeLayout->addWidget(new QLabel("Theme:"), 0, 0);
    m_themeCombo = new Components::FluentComboBox(themeGroup);
    m_themeCombo->addItems({"Light", "Dark", "Auto"});
    m_themeCombo->setCurrentIndex(0);
    themeLayout->addWidget(m_themeCombo, 0, 1);

    themeLayout->addWidget(new QLabel("Enable Animations:"), 1, 0);
    m_animationsCheck =
        new Components::FluentCheckBox("Enable smooth animations", themeGroup);
    m_animationsCheck->setChecked(true);
    themeLayout->addWidget(m_animationsCheck, 1, 1);

    themeLayout->addWidget(new QLabel("Content Opacity:"), 2, 0);
    m_opacitySlider = new QSlider(Qt::Horizontal, themeGroup);
    m_opacitySlider->setRange(50, 100);
    m_opacitySlider->setValue(100);
    themeLayout->addWidget(m_opacitySlider, 2, 1);

    settingsLayout->addWidget(themeGroup);

    // Performance settings
    auto* perfGroup = new QGroupBox("Performance Settings", settingsPanel);
    auto* perfLayout = new QGridLayout(perfGroup);

    perfLayout->addWidget(new QLabel("Refresh Interval (seconds):"), 0, 0);
    m_refreshInterval = new QSpinBox(perfGroup);
    m_refreshInterval->setRange(1, 60);
    m_refreshInterval->setValue(5);
    perfLayout->addWidget(m_refreshInterval, 0, 1);

    auto* refreshButton =
        new Components::FluentButton("Apply Settings", perfGroup);
    perfLayout->addWidget(refreshButton, 1, 0, 1, 2);

    settingsLayout->addWidget(perfGroup);

    // Layout settings
    auto* layoutGroup = new QGroupBox("Layout Settings", settingsPanel);
    auto* layoutGroupLayout = new QVBoxLayout(layoutGroup);

    auto* resetLayoutButton =
        new Components::FluentButton("Reset Layout to Default", layoutGroup);
    auto* saveLayoutButton =
        new Components::FluentButton("Save Current Layout", layoutGroup);
    auto* loadLayoutButton =
        new Components::FluentButton("Load Saved Layout", layoutGroup);

    layoutGroupLayout->addWidget(resetLayoutButton);
    layoutGroupLayout->addWidget(saveLayoutButton);
    layoutGroupLayout->addWidget(loadLayoutButton);

    settingsLayout->addWidget(layoutGroup);

    // Connect settings controls
    connect(refreshButton, &Components::FluentButton::clicked, this,
            &NavigationLayoutExample::updateLayoutSettings);
    connect(m_animationsCheck, &Components::FluentCheckBox::toggled,
            [this](bool enabled) { m_animationsEnabled = enabled; });
    connect(m_opacitySlider, &QSlider::valueChanged, [this](int value) {
        m_contentOpacity = value / 100.0;
        // Apply opacity to content areas
        if (m_dashboardContent) {
            m_dashboardContent->setWindowOpacity(m_contentOpacity);
        }
    });

    layout->addWidget(settingsPanel);
    layout->addStretch();

    return m_settingsContent;
}

QWidget* NavigationLayoutExample::createAnalyticsContent() {
    m_analyticsContent = new QWidget();
    auto* layout = new QVBoxLayout(m_analyticsContent);

    // Create analytics splitter for complex layout
    m_detailSplitter =
        new Components::FluentSplitter(Qt::Vertical, m_analyticsContent);
    m_detailSplitter->setResizeMode(
        Components::FluentSplitterResizeMode::Animated);

    // Top section - Charts and metrics
    auto* metricsPanel =
        new Components::FluentPanel("Analytics Dashboard", m_analyticsContent);
    metricsPanel->setPanelType(Components::FluentPanelType::Surface);
    auto* metricsLayout = metricsPanel->createHorizontalLayout();

    // Create sample metrics cards
    for (int i = 0; i < 4; ++i) {
        auto* metricCard = new Components::FluentCard(metricsPanel);
        metricCard->setTitle(QString("Metric %1").arg(i + 1));
        metricCard->setSubtitle("Performance indicator");
        metricCard->setMinimumSize(150, 100);

        auto* cardLayout = new QVBoxLayout(metricCard);
        auto* valueLabel = new QLabel(
            QString("%1%").arg(QRandomGenerator::global()->bounded(100)),
            metricCard);
        valueLabel->setStyleSheet(
            "font-size: 24px; font-weight: bold; color: #0078d4;");
        valueLabel->setAlignment(Qt::AlignCenter);
        cardLayout->addWidget(valueLabel);

        metricsLayout->addWidget(metricCard);
    }

    // Bottom section - Data table
    auto* dataPanel =
        new Components::FluentPanel("Detailed Analytics", m_analyticsContent);
    dataPanel->setPanelType(Components::FluentPanelType::Content);
    auto* dataLayout = dataPanel->createVerticalLayout();

    auto* dataTable = new QTableWidget(10, 5, dataPanel);
    dataTable->setHorizontalHeaderLabels(
        {"Date", "Users", "Sessions", "Bounce Rate", "Revenue"});
    dataTable->horizontalHeader()->setStretchLastSection(true);

    // Fill with sample data
    for (int row = 0; row < 10; ++row) {
        dataTable->setItem(
            row, 0,
            new QTableWidgetItem(
                QDateTime::currentDateTime().addDays(-row).toString(
                    "yyyy-MM-dd")));
        dataTable->setItem(
            row, 1,
            new QTableWidgetItem(QString::number(
                QRandomGenerator::global()->bounded(1000, 5000))));
        dataTable->setItem(
            row, 2,
            new QTableWidgetItem(QString::number(
                QRandomGenerator::global()->bounded(500, 2000))));
        dataTable->setItem(row, 3,
                           new QTableWidgetItem(QString("%1%").arg(
                               QRandomGenerator::global()->bounded(20, 80))));
        dataTable->setItem(
            row, 4,
            new QTableWidgetItem(QString("$%1").arg(
                QRandomGenerator::global()->bounded(100, 1000))));
    }

    dataLayout->addWidget(dataTable);

    m_detailSplitter->addWidget(metricsPanel);
    m_detailSplitter->addWidget(dataPanel);
    m_detailSplitter->setSizes({300, 400});

    layout->addWidget(m_detailSplitter);
    return m_analyticsContent;
}

QWidget* NavigationLayoutExample::createHelpContent() {
    m_helpContent = new QWidget();
    auto* layout = new QVBoxLayout(m_helpContent);

    auto* helpPanel =
        new Components::FluentPanel("Help & Documentation", m_helpContent);
    helpPanel->setPanelType(Components::FluentPanelType::Card);

    auto* helpLayout = helpPanel->createVerticalLayout();

    auto* helpText = new QTextEdit(helpPanel);
    helpText->setHtml(R"(
        <h2>FluentQt Navigation and Layout Example</h2>
        <p>This example demonstrates the powerful navigation and layout capabilities of FluentQt.</p>

        <h3>Components Demonstrated:</h3>
        <ul>
            <li><strong>FluentNavigationView</strong> - Responsive navigation with auto-collapsing sidebar</li>
            <li><strong>FluentTabView</strong> - Multiple tab styles and positions</li>
            <li><strong>FluentSplitter</strong> - Animated, responsive splitter panels</li>
            <li><strong>FluentPanel</strong> - Various panel types and elevations</li>
            <li><strong>FluentCard</strong> - Information cards with consistent styling</li>
        </ul>

        <h3>Features:</h3>
        <ul>
            <li>Responsive design that adapts to window size</li>
            <li>Smooth animations and transitions</li>
            <li>Collapsible panels and navigation</li>
            <li>Multiple layout configurations</li>
            <li>Theme integration and customization</li>
        </ul>

        <h3>Try These Actions:</h3>
        <ul>
            <li>Resize the window to see responsive behavior</li>
            <li>Navigate between different sections</li>
            <li>Add and close tabs in the Dashboard</li>
            <li>Drag splitter handles to resize panels</li>
            <li>Adjust settings to see real-time changes</li>
        </ul>

        <p><em>This example showcases how FluentQt components work together to create sophisticated,
        modern application layouts with minimal code.</em></p>
    )");
    helpText->setReadOnly(true);

    helpLayout->addWidget(helpText);
    layout->addWidget(helpPanel);

    return m_helpContent;
}

QWidget* NavigationLayoutExample::createOverviewTab() {
    auto* overviewWidget = new QWidget();
    auto* layout = new QHBoxLayout(overviewWidget);

    // Left side - Welcome and stats
    auto* leftPanel = new Components::FluentPanel("Welcome", overviewWidget);
    leftPanel->setPanelType(Components::FluentPanelType::Card);
    auto* leftLayout = leftPanel->createVerticalLayout();

    m_welcomeLabel = new QLabel("Welcome to FluentQt Dashboard!", leftPanel);
    m_welcomeLabel->setStyleSheet(
        "font-size: 18px; font-weight: bold; margin: 10px;");
    leftLayout->addWidget(m_welcomeLabel);

    m_statsLabel = new QLabel("Loading statistics...", leftPanel);
    leftLayout->addWidget(m_statsLabel);

    m_progressBar = new QProgressBar(leftPanel);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(75);
    leftLayout->addWidget(m_progressBar);

    auto* refreshButton =
        new Components::FluentButton("Refresh Dashboard", leftPanel);
    connect(refreshButton, &Components::FluentButton::clicked, this,
            &NavigationLayoutExample::refreshDashboard);
    leftLayout->addWidget(refreshButton);

    leftLayout->addStretch();

    // Right side - Recent items and notes
    auto* rightPanel =
        new Components::FluentPanel("Recent Activity", overviewWidget);
    rightPanel->setPanelType(Components::FluentPanelType::Surface);
    auto* rightLayout = rightPanel->createVerticalLayout();

    m_recentItems = new QListWidget(rightPanel);
    m_recentItems->setMaximumHeight(150);
    rightLayout->addWidget(m_recentItems);

    auto* notesLabel = new QLabel("Quick Notes:", rightPanel);
    rightLayout->addWidget(notesLabel);

    m_notesArea = new QTextEdit(rightPanel);
    m_notesArea->setMaximumHeight(100);
    m_notesArea->setPlainText("Enter your notes here...");
    rightLayout->addWidget(m_notesArea);

    layout->addWidget(leftPanel);
    layout->addWidget(rightPanel);

    return overviewWidget;
}

QWidget* NavigationLayoutExample::createDataTab() {
    auto* dataWidget = new QWidget();
    auto* layout = new QVBoxLayout(dataWidget);

    auto* dataPanel =
        new Components::FluentPanel("Data Management", dataWidget);
    dataPanel->setPanelType(Components::FluentPanelType::Content);
    auto* dataLayout = dataPanel->createVerticalLayout();

    // Create sample data table
    auto* dataTable = new QTableWidget(8, 4, dataPanel);
    dataTable->setHorizontalHeaderLabels({"ID", "Name", "Value", "Status"});

    // Fill with sample data
    QStringList statuses = {"Active", "Pending", "Completed", "Error"};
    for (int row = 0; row < 8; ++row) {
        dataTable->setItem(row, 0,
                           new QTableWidgetItem(QString("ID_%1").arg(row + 1)));
        dataTable->setItem(
            row, 1, new QTableWidgetItem(QString("Item %1").arg(row + 1)));
        dataTable->setItem(
            row, 2,
            new QTableWidgetItem(QString::number(
                QRandomGenerator::global()->bounded(100, 1000))));
        dataTable->setItem(
            row, 3, new QTableWidgetItem(statuses[row % statuses.size()]));
    }

    dataTable->horizontalHeader()->setStretchLastSection(true);
    dataLayout->addWidget(dataTable);

    // Add control buttons
    auto* buttonLayout = new QHBoxLayout();
    auto* addButton = new Components::FluentButton("Add Item", dataPanel);
    auto* editButton = new Components::FluentButton("Edit Selected", dataPanel);
    auto* deleteButton =
        new Components::FluentButton("Delete Selected", dataPanel);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();

    dataLayout->addLayout(buttonLayout);
    layout->addWidget(dataPanel);

    return dataWidget;
}

QWidget* NavigationLayoutExample::createChartsTab() {
    auto* chartsWidget = new QWidget();
    auto* layout = new QGridLayout(chartsWidget);

    // Create sample chart placeholders
    for (int i = 0; i < 4; ++i) {
        auto* chartCard = new Components::FluentCard(chartsWidget);
        chartCard->setTitle(QString("Chart %1").arg(i + 1));
        chartCard->setSubtitle("Sample visualization");
        chartCard->setMinimumSize(200, 150);

        // Add a simple visual representation
        auto* chartLayout = new QVBoxLayout(chartCard);
        auto* chartLabel = new QLabel("📊 Chart Visualization", chartCard);
        chartLabel->setAlignment(Qt::AlignCenter);
        chartLabel->setStyleSheet("font-size: 16px; color: #666;");
        chartLayout->addWidget(chartLabel);

        layout->addWidget(chartCard, i / 2, i % 2);
    }

    return chartsWidget;
}

QWidget* NavigationLayoutExample::createReportsTab() {
    auto* reportsWidget = new QWidget();
    auto* layout = new QVBoxLayout(reportsWidget);

    auto* reportsPanel = new Components::FluentPanel("Reports", reportsWidget);
    reportsPanel->setPanelType(Components::FluentPanelType::Card);
    auto* reportsLayout = reportsPanel->createVerticalLayout();

    auto* reportsText = new QTextEdit(reportsPanel);
    reportsText->setHtml(R"(
        <h3>Available Reports</h3>
        <ul>
            <li><strong>Daily Summary</strong> - Overview of daily activities</li>
            <li><strong>Weekly Analysis</strong> - Comprehensive weekly breakdown</li>
            <li><strong>Monthly Report</strong> - Monthly performance metrics</li>
            <li><strong>Custom Report</strong> - User-defined report parameters</li>
        </ul>

        <h3>Report Generation</h3>
        <p>Select a report type and click Generate to create a new report.</p>
    )");
    reportsText->setReadOnly(true);
    reportsLayout->addWidget(reportsText);

    auto* generateButton =
        new Components::FluentButton("Generate Report", reportsPanel);
    reportsLayout->addWidget(generateButton);

    layout->addWidget(reportsPanel);
    return reportsWidget;
}

void NavigationLayoutExample::setupTabContent() {
    // Tab content is created dynamically in the create methods
    // This method can be used for additional tab setup if needed
}

void NavigationLayoutExample::setupConnections() {
    // Navigation view connections
    connect(m_navigationView,
            &Components::FluentNavigationView::selectionChanged, this,
            &NavigationLayoutExample::onNavigationSelectionChanged);

    // Dashboard tabs connections
    connect(m_dashboardTabs, &Components::FluentTabView::currentChanged, this,
            &NavigationLayoutExample::onTabChanged);
    connect(m_dashboardTabs, &Components::FluentTabView::addTabRequested, this,
            &NavigationLayoutExample::addNewTab);
    connect(m_dashboardTabs, &Components::FluentTabView::tabCloseRequested,
            this, &NavigationLayoutExample::closeTab);

    // Splitter connections
    if (m_contentSplitter) {
        connect(m_contentSplitter, &Components::FluentSplitter::splitterMoved,
                this, &NavigationLayoutExample::onSplitterMoved);
    }

    // Timer connections
    connect(m_dashboardTimer, &QTimer::timeout, this,
            &NavigationLayoutExample::updateDashboardData);
    connect(m_animationTimer, &QTimer::timeout, this,
            &NavigationLayoutExample::animateContentTransition);

    // Start dashboard updates
    m_dashboardTimer->start();
}

void NavigationLayoutExample::setupTheme() {
    auto& theme = Styling::FluentTheme::instance();
    // Theme will be applied automatically to FluentQt components
}

void NavigationLayoutExample::loadSampleData() {
    // Load sample data for recent items
    if (m_recentItems) {
        m_recentItems->addItem("📄 Document_2024.pdf");
        m_recentItems->addItem("📊 Analytics_Report.xlsx");
        m_recentItems->addItem("🖼️ UI_Mockup.png");
        m_recentItems->addItem("📝 Meeting_Notes.txt");
        m_recentItems->addItem("💾 Project_Backup.zip");
    }

    // Update initial dashboard data
    updateDashboardData();
}

// Slot implementations
void NavigationLayoutExample::onNavigationSelectionChanged(int index) {
    m_currentNavigationIndex = index;

    if (m_animationsEnabled) {
        animateContentTransition();
    }

    // Update window title based on selection
    QStringList sections = {"Dashboard", "Documents", "Analytics", "Settings",
                            "Help"};
    if (index >= 0 && index < sections.size()) {
        setWindowTitle(
            QString("FluentQt Navigation Example - %1").arg(sections[index]));
    }
}

void NavigationLayoutExample::onTabChanged(int index) {
    // Handle tab changes with animation if enabled
    if (m_animationsEnabled && m_dashboardTabs) {
        Animation::FluentAnimationConfig config;
        config.duration = std::chrono::milliseconds(250);
        config.easing = Animation::FluentEasing::EaseOutQuad;

        auto* currentWidget = m_dashboardTabs->currentWidget();
        if (currentWidget) {
            auto fadeAnimation =
                Animation::FluentAnimator::fadeIn(currentWidget, config);
            fadeAnimation->start();
        }
    }
}

void NavigationLayoutExample::onSplitterMoved(int pos, int index) {
    // Handle splitter movement - could save layout state here
    Q_UNUSED(pos)
    Q_UNUSED(index)
}

void NavigationLayoutExample::toggleNavigationPane() {
    if (m_navigationView) {
        m_navigationView->togglePane();
    }
}

void NavigationLayoutExample::addNewTab() {
    if (!m_dashboardTabs)
        return;

    // Create a new reports tab
    Components::FluentTabData newTab;
    newTab.text = QString("Report %1").arg(m_dashboardTabs->count() + 1);
    newTab.icon = QIcon(":/icons/report.png");
    newTab.content = createReportsTab();
    newTab.closeable = true;

    m_dashboardTabs->addTab(newTab);
    m_dashboardTabs->setCurrentIndex(m_dashboardTabs->count() - 1);
}

void NavigationLayoutExample::closeTab(int index) {
    if (m_dashboardTabs && index > 0) {  // Don't allow closing the first tab
        m_dashboardTabs->removeTab(index);
    }
}

void NavigationLayoutExample::refreshDashboard() {
    updateDashboardData();

    if (m_progressBar) {
        // Animate progress bar
        m_progressBar->setValue(0);

        auto* timer = new QTimer(this);
        timer->setSingleShot(false);
        timer->setInterval(50);

        int progress = 0;
        connect(timer, &QTimer::timeout, [this, timer, &progress]() {
            progress += 5;
            m_progressBar->setValue(progress);
            if (progress >= 100) {
                timer->stop();
                timer->deleteLater();
            }
        });

        timer->start();
    }
}

void NavigationLayoutExample::updateLayoutSettings() {
    if (m_refreshInterval) {
        m_dashboardTimer->setInterval(m_refreshInterval->value() * 1000);
    }

    // Apply other settings changes
    if (m_themeCombo) {
        // Theme switching would be implemented here
    }
}

void NavigationLayoutExample::animateContentTransition() {
    if (!m_animationsEnabled)
        return;

    // Animate content transition
    Animation::FluentAnimationConfig config;
    config.duration = std::chrono::milliseconds(300);
    config.easing = Animation::FluentEasing::EaseOutQuad;

    auto* currentContent = m_navigationView->currentContent();
    if (currentContent) {
        auto fadeAnimation =
            Animation::FluentAnimator::fadeIn(currentContent, config);
        fadeAnimation->start();
    }
}

void NavigationLayoutExample::updateDashboardData() {
    if (m_statsLabel) {
        QString stats =
            QString(
                "📊 Active Users: %1\n"
                "📈 Sessions Today: %2\n"
                "💰 Revenue: $%3\n"
                "⏱️ Last Updated: %4")
                .arg(QRandomGenerator::global()->bounded(1000, 5000))
                .arg(QRandomGenerator::global()->bounded(500, 2000))
                .arg(QRandomGenerator::global()->bounded(1000, 10000))
                .arg(QDateTime::currentDateTime().toString("hh:mm:ss"));

        m_statsLabel->setText(stats);
    }

    if (m_progressBar) {
        int newValue = QRandomGenerator::global()->bounded(60, 100);
        m_progressBar->setValue(newValue);
    }
}

// Main function
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("FluentQt Navigation and Layout Example");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("FluentQt Examples");

    // Initialize FluentQt theme
    auto& theme = Styling::FluentTheme::instance();
    // Theme will be initialized automatically

    NavigationLayoutExample window;
    window.show();

    return app.exec();
}

#include "NavigationLayoutExample.moc"
