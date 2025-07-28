// examples/RealWorldApplicationExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QStackedWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QLabel>
#include <QTextEdit>
#include <QListWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QHeaderView>
#include <QTimer>
#include <QDateTime>
#include <QRandomGenerator>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QSlider>
#include <QSpinBox>
#include <QGroupBox>
#include <QScrollArea>
#include <QSettings>
#include <QInputDialog>
#include <QFormLayout>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentPanel.h"
#include "FluentQt/Components/FluentNavigationView.h"
#include "FluentQt/Components/FluentTabView.h"
#include "FluentQt/Components/FluentSplitter.h"
#include "FluentQt/Components/FluentTextInput.h"
#include "FluentQt/Components/FluentComboBox.h"
#include "FluentQt/Components/FluentCheckBox.h"
#include "FluentQt/Components/FluentProgressBar.h"
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Components/FluentChartView.h"
#include "FluentQt/Components/FluentRichTextEditor.h"
#include "FluentQt/Components/FluentCalendar.h"
#include "FluentQt/Components/FluentBreadcrumb.h"
#include "FluentQt/Styling/FluentTheme.h"
#include "FluentQt/Animation/FluentAnimator.h"
#include "FluentQt/Core/FluentPerformance.h"
#include "FluentQt/Accessibility/FluentAccessible.h"

using namespace FluentQt;

/**
 * Real-world Application Example: Project Management Dashboard
 * 
 * This example demonstrates a complete mini-application that showcases:
 * - Professional application architecture
 * - Integration of multiple FluentQt components
 * - Real-world data management patterns
 * - Modern UI/UX design principles
 * - Performance optimization
 * - Accessibility support
 * - Responsive design
 */
class ProjectManagementApp : public QMainWindow {
    Q_OBJECT

public:
    ProjectManagementApp(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupApplication();
        setupMenuBar();
        setupToolBar();
        setupStatusBar();
        setupMainInterface();
        setupConnections();
        setupTheme();
        loadApplicationData();
        setupPerformanceMonitoring();
        setupAccessibility();
    }

    ~ProjectManagementApp() {
        saveApplicationSettings();
    }

private slots:
    // File operations
    void newProject();
    void openProject();
    void saveProject();
    void exportProject();
    void importData();
    
    // View operations
    void toggleTheme();
    void toggleFullScreen();
    void showPreferences();
    void showAbout();
    
    // Navigation
    void onNavigationChanged(int index);
    void onTabChanged(int index);
    
    // Project management
    void addTask();
    void editTask();
    void deleteTask();
    void markTaskComplete();
    void updateTaskProgress();
    
    // Team management
    void addTeamMember();
    void editTeamMember();
    void removeTeamMember();
    void assignTask();
    
    // Analytics and reporting
    void generateReport();
    void exportAnalytics();
    void refreshDashboard();
    void updateMetrics();
    
    // Real-time updates
    void onDataChanged();
    void onNotificationReceived();
    void onPerformanceUpdate();

private:
    void setupApplication();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupMainInterface();
    void setupConnections();
    void setupTheme();
    void setupPerformanceMonitoring();
    void setupAccessibility();
    
    // Data management
    void loadApplicationData();
    void saveApplicationSettings();
    void loadProjectData();
    void saveProjectData();
    
    // UI creation methods
    QWidget* createDashboardPage();
    QWidget* createProjectsPage();
    QWidget* createTasksPage();
    QWidget* createTeamPage();
    QWidget* createAnalyticsPage();
    QWidget* createCalendarPage();
    QWidget* createSettingsPage();
    
    // Dashboard components
    QWidget* createMetricsOverview();
    QWidget* createRecentActivity();
    QWidget* createQuickActions();
    QWidget* createProjectStatus();
    
    // Task management components
    QWidget* createTaskList();
    QWidget* createTaskDetails();
    QWidget* createTaskFilters();
    
    // Team management components
    QWidget* createTeamOverview();
    QWidget* createMemberDetails();
    QWidget* createTeamChat();
    
    // Analytics components
    QWidget* createPerformanceCharts();
    QWidget* createProgressReports();
    QWidget* createTimeTracking();
    
    // Helper methods
    void updateDashboardMetrics();
    void refreshTaskList();
    void refreshTeamData();
    void updateProjectProgress();
    void showNotification(const QString& message, const QString& type = "info");
    void animatePageTransition();
    void profileOperation(const QString& operation, std::function<void()> func);

private:
    // Main navigation and layout
    Components::FluentNavigationView* m_navigationView{nullptr};
    Components::FluentTabView* m_mainTabs{nullptr};
    Components::FluentSplitter* m_mainSplitter{nullptr};
    
    // Menu and toolbar
    QMenuBar* m_menuBar{nullptr};
    QToolBar* m_toolBar{nullptr};
    QStatusBar* m_statusBar{nullptr};
    
    // Status bar components
    QLabel* m_statusLabel{nullptr};
    QLabel* m_projectLabel{nullptr};
    QLabel* m_userLabel{nullptr};
    Components::FluentProgressBar* m_operationProgress{nullptr};
    
    // Dashboard components
    QWidget* m_dashboardPage{nullptr};
    QGridLayout* m_metricsLayout{nullptr};
    QList<Components::FluentCard*> m_metricCards;
    QListWidget* m_activityList{nullptr};
    Components::FluentPanel* m_quickActionsPanel{nullptr};
    
    // Project management
    QWidget* m_projectsPage{nullptr};
    QWidget* m_tasksPage{nullptr};
    QTableWidget* m_taskTable{nullptr};
    Components::FluentTextInput* m_taskSearch{nullptr};
    Components::FluentComboBox* m_taskFilter{nullptr};
    Components::FluentRichTextEditor* m_taskEditor{nullptr};
    
    // Team management
    QWidget* m_teamPage{nullptr};
    Components::FluentTreeView* m_teamTree{nullptr};
    QTextEdit* m_teamChat{nullptr};
    Components::FluentCard* m_memberCard{nullptr};
    
    // Analytics
    QWidget* m_analyticsPage{nullptr};
    Components::FluentChartView* m_performanceChart{nullptr};
    Components::FluentChartView* m_progressChart{nullptr};
    QTableWidget* m_timeTrackingTable{nullptr};
    
    // Calendar and scheduling
    QWidget* m_calendarPage{nullptr};
    Components::FluentCalendar* m_calendar{nullptr};
    QListWidget* m_eventsList{nullptr};
    
    // Settings
    QWidget* m_settingsPage{nullptr};
    Components::FluentComboBox* m_themeCombo{nullptr};
    Components::FluentCheckBox* m_notificationsCheck{nullptr};
    Components::FluentCheckBox* m_animationsCheck{nullptr};
    QSlider* m_refreshIntervalSlider{nullptr};
    
    // Data and state management
    QTimer* m_refreshTimer{nullptr};
    QTimer* m_metricsTimer{nullptr};
    QTimer* m_notificationTimer{nullptr};
    
    // Application state
    QString m_currentProject{"Sample Project"};
    QString m_currentUser{"John Doe"};
    bool m_darkMode{false};
    bool m_notificationsEnabled{true};
    bool m_animationsEnabled{true};
    int m_refreshInterval{30}; // seconds
    
    // Performance monitoring
    Core::FluentPerformanceMonitor* m_performanceMonitor{nullptr};
    
    // Sample data
    struct Task {
        QString id;
        QString title;
        QString description;
        QString assignee;
        QString status;
        int progress;
        QDateTime dueDate;
        QString priority;
    };
    
    struct TeamMember {
        QString id;
        QString name;
        QString role;
        QString email;
        QString status;
        QStringList skills;
        int tasksAssigned;
    };
    
    QList<Task> m_tasks;
    QList<TeamMember> m_teamMembers;
    QStringList m_recentActivities;
    
    // Metrics data
    struct ProjectMetrics {
        int totalTasks{0};
        int completedTasks{0};
        int overdueTasks{0};
        int teamMembers{0};
        double projectProgress{0.0};
        int hoursLogged{0};
        double budget{0.0};
        double budgetUsed{0.0};
    } m_metrics;
};

void ProjectManagementApp::setupApplication() {
    setWindowTitle("FluentQt Project Management - Real-world Application Example");
    setMinimumSize(1400, 900);
    resize(1600, 1000);
    
    // Set application icon and properties
    setWindowIcon(QIcon(":/icons/app.png"));
    
    // Enable high DPI support
    setAttribute(Qt::WA_AcceptTouchEvents);
    
    // Set up application settings
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    
    m_darkMode = settings.value("darkMode", false).toBool();
    m_notificationsEnabled = settings.value("notifications", true).toBool();
    m_animationsEnabled = settings.value("animations", true).toBool();
    m_refreshInterval = settings.value("refreshInterval", 30).toInt();
}

void ProjectManagementApp::setupMenuBar() {
    m_menuBar = menuBar();
    
    // File menu
    auto* fileMenu = m_menuBar->addMenu("&File");
    
    auto* newAction = fileMenu->addAction("&New Project");
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip("Create a new project");
    connect(newAction, &QAction::triggered, this, &ProjectManagementApp::newProject);
    
    auto* openAction = fileMenu->addAction("&Open Project");
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip("Open an existing project");
    connect(openAction, &QAction::triggered, this, &ProjectManagementApp::openProject);
    
    auto* saveAction = fileMenu->addAction("&Save Project");
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip("Save the current project");
    connect(saveAction, &QAction::triggered, this, &ProjectManagementApp::saveProject);
    
    fileMenu->addSeparator();
    
    auto* exportAction = fileMenu->addAction("&Export...");
    exportAction->setStatusTip("Export project data");
    connect(exportAction, &QAction::triggered, this, &ProjectManagementApp::exportProject);
    
    auto* importAction = fileMenu->addAction("&Import...");
    importAction->setStatusTip("Import project data");
    connect(importAction, &QAction::triggered, this, &ProjectManagementApp::importData);
    
    fileMenu->addSeparator();
    
    auto* exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip("Exit the application");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    // View menu
    auto* viewMenu = m_menuBar->addMenu("&View");
    
    auto* themeAction = viewMenu->addAction("Toggle &Theme");
    themeAction->setShortcut(QKeySequence("Ctrl+T"));
    themeAction->setStatusTip("Switch between light and dark themes");
    connect(themeAction, &QAction::triggered, this, &ProjectManagementApp::toggleTheme);
    
    auto* fullScreenAction = viewMenu->addAction("&Full Screen");
    fullScreenAction->setShortcut(QKeySequence::FullScreen);
    fullScreenAction->setStatusTip("Toggle full screen mode");
    fullScreenAction->setCheckable(true);
    connect(fullScreenAction, &QAction::triggered, this, &ProjectManagementApp::toggleFullScreen);
    
    viewMenu->addSeparator();
    
    auto* preferencesAction = viewMenu->addAction("&Preferences");
    preferencesAction->setShortcut(QKeySequence::Preferences);
    preferencesAction->setStatusTip("Open application preferences");
    connect(preferencesAction, &QAction::triggered, this, &ProjectManagementApp::showPreferences);
    
    // Help menu
    auto* helpMenu = m_menuBar->addMenu("&Help");
    
    auto* aboutAction = helpMenu->addAction("&About");
    aboutAction->setStatusTip("Show application information");
    connect(aboutAction, &QAction::triggered, this, &ProjectManagementApp::showAbout);
}

void ProjectManagementApp::setupToolBar() {
    m_toolBar = addToolBar("Main");
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // Quick action buttons
    auto* newProjectBtn = new Components::FluentButton("New Project");
    newProjectBtn->setIcon(QIcon(":/icons/new.png"));
    connect(newProjectBtn, &Components::FluentButton::clicked, this, &ProjectManagementApp::newProject);
    m_toolBar->addWidget(newProjectBtn);

    auto* addTaskBtn = new Components::FluentButton("Add Task");
    addTaskBtn->setIcon(QIcon(":/icons/add.png"));
    connect(addTaskBtn, &Components::FluentButton::clicked, this, &ProjectManagementApp::addTask);
    m_toolBar->addWidget(addTaskBtn);

    auto* refreshBtn = new Components::FluentButton("Refresh");
    refreshBtn->setIcon(QIcon(":/icons/refresh.png"));
    connect(refreshBtn, &Components::FluentButton::clicked, this, &ProjectManagementApp::refreshDashboard);
    m_toolBar->addWidget(refreshBtn);

    m_toolBar->addSeparator();

    auto* themeBtn = new Components::FluentButton("Toggle Theme");
    themeBtn->setIcon(QIcon(":/icons/theme.png"));
    connect(themeBtn, &Components::FluentButton::clicked, this, &ProjectManagementApp::toggleTheme);
    m_toolBar->addWidget(themeBtn);
}

void ProjectManagementApp::setupStatusBar() {
    m_statusBar = statusBar();

    // Status message
    m_statusLabel = new QLabel("Ready");
    m_statusBar->addWidget(m_statusLabel);

    // Project info
    m_projectLabel = new QLabel(QString("Project: %1").arg(m_currentProject));
    m_statusBar->addPermanentWidget(m_projectLabel);

    // User info
    m_userLabel = new QLabel(QString("User: %1").arg(m_currentUser));
    m_statusBar->addPermanentWidget(m_userLabel);

    // Operation progress
    m_operationProgress = new Components::FluentProgressBar();
    m_operationProgress->setVisible(false);
    m_operationProgress->setMaximumWidth(200);
    m_statusBar->addPermanentWidget(m_operationProgress);
}

void ProjectManagementApp::setupMainInterface() {
    // Create main navigation view
    m_navigationView = new Components::FluentNavigationView(this);
    m_navigationView->setHeader("Project Management");
    m_navigationView->setDisplayMode(Components::FluentNavigationDisplayMode::Auto);
    m_navigationView->setBackButtonVisible(false);
    m_navigationView->setSettingsVisible(true);

    setCentralWidget(m_navigationView);

    // Add navigation items
    Components::FluentNavigationItem dashboardItem;
    dashboardItem.text = "Dashboard";
    dashboardItem.icon = QIcon(":/icons/dashboard.png");
    dashboardItem.tag = "dashboard";
    dashboardItem.content = createDashboardPage();
    m_navigationView->addNavigationItem(dashboardItem);

    Components::FluentNavigationItem projectsItem;
    projectsItem.text = "Projects";
    projectsItem.icon = QIcon(":/icons/projects.png");
    projectsItem.tag = "projects";
    projectsItem.content = createProjectsPage();
    m_navigationView->addNavigationItem(projectsItem);

    Components::FluentNavigationItem tasksItem;
    tasksItem.text = "Tasks";
    tasksItem.icon = QIcon(":/icons/tasks.png");
    tasksItem.tag = "tasks";
    tasksItem.content = createTasksPage();
    m_navigationView->addNavigationItem(tasksItem);

    Components::FluentNavigationItem teamItem;
    teamItem.text = "Team";
    teamItem.icon = QIcon(":/icons/team.png");
    teamItem.tag = "team";
    teamItem.content = createTeamPage();
    m_navigationView->addNavigationItem(teamItem);

    Components::FluentNavigationItem analyticsItem;
    analyticsItem.text = "Analytics";
    analyticsItem.icon = QIcon(":/icons/analytics.png");
    analyticsItem.tag = "analytics";
    analyticsItem.content = createAnalyticsPage();
    m_navigationView->addNavigationItem(analyticsItem);

    Components::FluentNavigationItem calendarItem;
    calendarItem.text = "Calendar";
    calendarItem.icon = QIcon(":/icons/calendar.png");
    calendarItem.tag = "calendar";
    calendarItem.content = createCalendarPage();
    m_navigationView->addNavigationItem(calendarItem);

    Components::FluentNavigationItem settingsItem;
    settingsItem.text = "Settings";
    settingsItem.icon = QIcon(":/icons/settings.png");
    settingsItem.tag = "settings";
    settingsItem.content = createSettingsPage();
    m_navigationView->addNavigationItem(settingsItem);

    // Set initial selection
    m_navigationView->setSelectedIndex(0);
}

QWidget* ProjectManagementApp::createDashboardPage() {
    m_dashboardPage = new QWidget();
    auto* layout = new QVBoxLayout(m_dashboardPage);
    layout->setSpacing(20);

    // Welcome header
    auto* welcomeCard = new Components::FluentCard("Welcome to Project Management Dashboard");
    welcomeCard->setSubtitle(QString("Managing %1 - %2").arg(m_currentProject, QDateTime::currentDateTime().toString("MMMM dd, yyyy")));
    layout->addWidget(welcomeCard);

    // Main dashboard content
    auto* contentSplitter = new Components::FluentSplitter(Qt::Horizontal);

    // Left side - Metrics and quick actions
    auto* leftPanel = new QWidget();
    auto* leftLayout = new QVBoxLayout(leftPanel);

    leftLayout->addWidget(createMetricsOverview());
    leftLayout->addWidget(createQuickActions());

    // Right side - Activity and project status
    auto* rightPanel = new QWidget();
    auto* rightLayout = new QVBoxLayout(rightPanel);

    rightLayout->addWidget(createRecentActivity());
    rightLayout->addWidget(createProjectStatus());

    contentSplitter->addWidget(leftPanel);
    contentSplitter->addWidget(rightPanel);
    contentSplitter->setSizes({600, 400});

    layout->addWidget(contentSplitter);
    return m_dashboardPage;
}

QWidget* ProjectManagementApp::createMetricsOverview() {
    auto* metricsPanel = new Components::FluentPanel("Project Metrics");
    metricsPanel->setPanelType(Components::FluentPanelType::Card);
    metricsPanel->setElevation(Components::FluentPanelElevation::Medium);

    m_metricsLayout = new QGridLayout();
    auto* panelLayout = metricsPanel->createVerticalLayout();
    panelLayout->addLayout(m_metricsLayout);

    // Create metric cards
    QStringList metricTitles = {"Total Tasks", "Completed", "In Progress", "Team Members", "Project Progress", "Hours Logged"};
    QStringList metricValues = {"24", "18", "6", "8", "75%", "156"};
    QStringList metricColors = {"#0078d4", "#107c10", "#ff8c00", "#5c2d91", "#d13438", "#00bcf2"};

    for (int i = 0; i < metricTitles.size(); ++i) {
        auto* metricCard = new Components::FluentCard();
        metricCard->setTitle(metricTitles[i]);
        metricCard->setSubtitle(metricValues[i]);
        metricCard->setMinimumSize(150, 100);

        // Add visual indicator
        auto* cardLayout = new QVBoxLayout(metricCard);
        auto* valueLabel = new QLabel(metricValues[i]);
        valueLabel->setStyleSheet(QString("font-size: 24px; font-weight: bold; color: %1;").arg(metricColors[i]));
        valueLabel->setAlignment(Qt::AlignCenter);
        cardLayout->addWidget(valueLabel);

        m_metricCards.append(metricCard);
        m_metricsLayout->addWidget(metricCard, i / 3, i % 3);
    }

    return metricsPanel;
}

QWidget* ProjectManagementApp::createRecentActivity() {
    auto* activityPanel = new Components::FluentPanel("Recent Activity");
    activityPanel->setPanelType(Components::FluentPanelType::Surface);

    auto* panelLayout = activityPanel->createVerticalLayout();

    m_activityList = new QListWidget();
    m_activityList->setMaximumHeight(200);

    // Add sample activities
    m_activityList->addItem("📝 Task 'UI Design Review' completed by Sarah");
    m_activityList->addItem("👥 New team member 'Mike Johnson' added");
    m_activityList->addItem("📊 Weekly report generated");
    m_activityList->addItem("⚠️ Task 'Database Migration' is overdue");
    m_activityList->addItem("✅ Milestone 'Phase 1' completed");

    panelLayout->addWidget(m_activityList);

    return activityPanel;
}

QWidget* ProjectManagementApp::createQuickActions() {
    m_quickActionsPanel = new Components::FluentPanel("Quick Actions");
    m_quickActionsPanel->setPanelType(Components::FluentPanelType::Card);

    auto* panelLayout = m_quickActionsPanel->createVerticalLayout();

    auto* addTaskBtn = new Components::FluentButton("Add New Task");
    addTaskBtn->setIcon(QIcon(":/icons/add.png"));
    connect(addTaskBtn, &Components::FluentButton::clicked, this, &ProjectManagementApp::addTask);

    auto* addMemberBtn = new Components::FluentButton("Add Team Member");
    addMemberBtn->setIcon(QIcon(":/icons/person.png"));
    connect(addMemberBtn, &Components::FluentButton::clicked, this, &ProjectManagementApp::addTeamMember);

    auto* generateReportBtn = new Components::FluentButton("Generate Report");
    generateReportBtn->setIcon(QIcon(":/icons/report.png"));
    connect(generateReportBtn, &Components::FluentButton::clicked, this, &ProjectManagementApp::generateReport);

    panelLayout->addWidget(addTaskBtn);
    panelLayout->addWidget(addMemberBtn);
    panelLayout->addWidget(generateReportBtn);

    return m_quickActionsPanel;
}

QWidget* ProjectManagementApp::createProjectStatus() {
    auto* statusPanel = new Components::FluentPanel("Project Status");
    statusPanel->setPanelType(Components::FluentPanelType::Surface);

    auto* panelLayout = statusPanel->createVerticalLayout();

    // Progress overview
    auto* progressLabel = new QLabel("Overall Progress: 75%");
    progressLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    panelLayout->addWidget(progressLabel);

    auto* progressBar = new Components::FluentProgressBar();
    progressBar->setValue(75);
    panelLayout->addWidget(progressBar);

    // Key metrics
    auto* metricsText = new QTextEdit();
    metricsText->setMaximumHeight(100);
    metricsText->setReadOnly(true);
    metricsText->setHtml(R"(
        <b>Key Metrics:</b><br>
        • Budget Used: $45,000 / $60,000<br>
        • Timeline: On track (2 weeks remaining)<br>
        • Quality Score: 4.2/5.0<br>
        • Team Satisfaction: 87%
    )");
    panelLayout->addWidget(metricsText);

    return statusPanel;
}

QWidget* ProjectManagementApp::createTasksPage() {
    m_tasksPage = new QWidget();
    auto* layout = new QVBoxLayout(m_tasksPage);

    // Task management header
    auto* headerLayout = new QHBoxLayout();

    m_taskSearch = new Components::FluentTextInput();
    m_taskSearch->setPlaceholderText("Search tasks...");

    m_taskFilter = new Components::FluentComboBox();
    m_taskFilter->addItems({"All Tasks", "Active", "Completed", "Overdue", "High Priority"});

    auto* addTaskBtn = new Components::FluentButton("Add Task");
    connect(addTaskBtn, &Components::FluentButton::clicked, this, &ProjectManagementApp::addTask);

    headerLayout->addWidget(new QLabel("Tasks:"));
    headerLayout->addWidget(m_taskSearch);
    headerLayout->addWidget(m_taskFilter);
    headerLayout->addStretch();
    headerLayout->addWidget(addTaskBtn);

    layout->addLayout(headerLayout);

    // Task list and details splitter
    auto* taskSplitter = new Components::FluentSplitter(Qt::Horizontal);

    // Task table
    m_taskTable = new QTableWidget(0, 6);
    m_taskTable->setHorizontalHeaderLabels({"Title", "Assignee", "Status", "Progress", "Due Date", "Priority"});
    m_taskTable->horizontalHeader()->setStretchLastSection(true);
    m_taskTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_taskTable->setAlternatingRowColors(true);

    // Task editor
    m_taskEditor = new Components::FluentRichTextEditor();
    m_taskEditor->setMinimumWidth(300);

    taskSplitter->addWidget(m_taskTable);
    taskSplitter->addWidget(m_taskEditor);
    taskSplitter->setSizes({700, 300});

    layout->addWidget(taskSplitter);

    return m_tasksPage;
}

QWidget* ProjectManagementApp::createTeamPage() {
    m_teamPage = new QWidget();
    auto* layout = new QVBoxLayout(m_teamPage);

    // Team management header
    auto* headerLayout = new QHBoxLayout();
    headerLayout->addWidget(new QLabel("Team Management"));
    headerLayout->addStretch();

    auto* addMemberBtn = new Components::FluentButton("Add Member");
    connect(addMemberBtn, &Components::FluentButton::clicked, this, &ProjectManagementApp::addTeamMember);
    headerLayout->addWidget(addMemberBtn);

    layout->addLayout(headerLayout);

    // Team content splitter
    auto* teamSplitter = new Components::FluentSplitter(Qt::Horizontal);

    // Team tree view
    m_teamTree = new Components::FluentTreeView();
    m_teamTree->setMinimumWidth(250);

    // Add sample team structure using FluentTreeView's native API
    // Note: Using QTreeWidget temporarily for demonstration
    auto* treeWidget = new QTreeWidget();
    treeWidget->setHeaderLabel("Team Structure");

    auto* rootItem = new QTreeWidgetItem(treeWidget, QStringList("Development Team"));
    auto* frontendItem = new QTreeWidgetItem(rootItem, QStringList("Frontend"));
    new QTreeWidgetItem(frontendItem, QStringList("Sarah Johnson - Lead"));
    new QTreeWidgetItem(frontendItem, QStringList("Mike Chen - Developer"));

    auto* backendItem = new QTreeWidgetItem(rootItem, QStringList("Backend"));
    new QTreeWidgetItem(backendItem, QStringList("David Smith - Lead"));
    new QTreeWidgetItem(backendItem, QStringList("Lisa Wang - Developer"));

    auto* qaItem = new QTreeWidgetItem(rootItem, QStringList("QA"));
    new QTreeWidgetItem(qaItem, QStringList("Tom Brown - QA Lead"));

    treeWidget->expandAll();

    // Member details and chat
    auto* rightPanel = new QWidget();
    auto* rightLayout = new QVBoxLayout(rightPanel);

    // Member details card
    m_memberCard = new Components::FluentCard("Team Member Details");
    m_memberCard->setSubtitle("Select a team member to view details");
    m_memberCard->setMinimumHeight(200);

    // Team chat
    auto* chatPanel = new Components::FluentPanel("Team Chat");
    chatPanel->setPanelType(Components::FluentPanelType::Surface);
    auto* chatLayout = chatPanel->createVerticalLayout();

    m_teamChat = new QTextEdit();
    m_teamChat->setMaximumHeight(150);
    m_teamChat->setReadOnly(true);
    m_teamChat->setHtml(R"(
        <b>Sarah:</b> UI mockups are ready for review<br>
        <b>Mike:</b> Great! I'll check them out<br>
        <b>David:</b> API endpoints are deployed to staging<br>
        <b>Lisa:</b> Running integration tests now
    )");

    auto* chatInput = new Components::FluentTextInput();
    chatInput->setPlaceholderText("Type a message...");

    chatLayout->addWidget(m_teamChat);
    chatLayout->addWidget(chatInput);

    rightLayout->addWidget(m_memberCard);
    rightLayout->addWidget(chatPanel);

    teamSplitter->addWidget(treeWidget);
    teamSplitter->addWidget(rightPanel);
    teamSplitter->setSizes({300, 500});

    layout->addWidget(teamSplitter);

    return m_teamPage;
}

QWidget* ProjectManagementApp::createAnalyticsPage() {
    m_analyticsPage = new QWidget();
    auto* layout = new QVBoxLayout(m_analyticsPage);

    // Analytics header
    auto* headerLayout = new QHBoxLayout();
    headerLayout->addWidget(new QLabel("Project Analytics"));
    headerLayout->addStretch();

    auto* exportBtn = new Components::FluentButton("Export Analytics");
    connect(exportBtn, &Components::FluentButton::clicked, this, &ProjectManagementApp::exportAnalytics);
    headerLayout->addWidget(exportBtn);

    layout->addLayout(headerLayout);

    // Analytics content
    auto* analyticsSplitter = new Components::FluentSplitter(Qt::Vertical);

    // Charts section
    auto* chartsWidget = new QWidget();
    auto* chartsLayout = new QHBoxLayout(chartsWidget);

    // Performance chart
    auto* performancePanel = new Components::FluentPanel("Performance Trends");
    auto* performancePanelLayout = performancePanel->createVerticalLayout();

    m_performanceChart = new Components::FluentChartView();
    m_performanceChart->setMinimumHeight(200);
    performancePanelLayout->addWidget(m_performanceChart);

    // Progress chart
    auto* progressPanel = new Components::FluentPanel("Progress Overview");
    auto* progressPanelLayout = progressPanel->createVerticalLayout();

    m_progressChart = new Components::FluentChartView();
    m_progressChart->setMinimumHeight(200);
    progressPanelLayout->addWidget(m_progressChart);

    chartsLayout->addWidget(performancePanel);
    chartsLayout->addWidget(progressPanel);

    // Time tracking table
    auto* timeTrackingPanel = new Components::FluentPanel("Time Tracking");
    auto* timeTrackingLayout = timeTrackingPanel->createVerticalLayout();

    m_timeTrackingTable = new QTableWidget(0, 5);
    m_timeTrackingTable->setHorizontalHeaderLabels({"Date", "Task", "Member", "Hours", "Description"});
    m_timeTrackingTable->horizontalHeader()->setStretchLastSection(true);
    m_timeTrackingTable->setMaximumHeight(200);

    // Add sample time tracking data
    for (int i = 0; i < 5; ++i) {
        int row = m_timeTrackingTable->rowCount();
        m_timeTrackingTable->insertRow(row);
        m_timeTrackingTable->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().addDays(-i).toString("yyyy-MM-dd")));
        m_timeTrackingTable->setItem(row, 1, new QTableWidgetItem(QString("Task %1").arg(i + 1)));
        m_timeTrackingTable->setItem(row, 2, new QTableWidgetItem(QString("Member %1").arg(i % 3 + 1)));
        m_timeTrackingTable->setItem(row, 3, new QTableWidgetItem(QString("%1h").arg(QRandomGenerator::global()->bounded(2, 8))));
        m_timeTrackingTable->setItem(row, 4, new QTableWidgetItem("Development work"));
    }

    timeTrackingLayout->addWidget(m_timeTrackingTable);

    analyticsSplitter->addWidget(chartsWidget);
    analyticsSplitter->addWidget(timeTrackingPanel);
    analyticsSplitter->setSizes({400, 200});

    layout->addWidget(analyticsSplitter);

    return m_analyticsPage;
}

QWidget* ProjectManagementApp::createCalendarPage() {
    m_calendarPage = new QWidget();
    auto* layout = new QVBoxLayout(m_calendarPage);

    // Calendar header
    auto* headerLayout = new QHBoxLayout();
    headerLayout->addWidget(new QLabel("Project Calendar"));
    headerLayout->addStretch();

    layout->addLayout(headerLayout);

    // Calendar and events splitter
    auto* calendarSplitter = new Components::FluentSplitter(Qt::Horizontal);

    // Calendar widget
    auto* calendarPanel = new Components::FluentPanel("Calendar");
    auto* calendarPanelLayout = calendarPanel->createVerticalLayout();

    m_calendar = new Components::FluentCalendar();
    m_calendar->setMinimumSize(400, 300);
    calendarPanelLayout->addWidget(m_calendar);

    // Events list
    auto* eventsPanel = new Components::FluentPanel("Upcoming Events");
    auto* eventsPanelLayout = eventsPanel->createVerticalLayout();

    m_eventsList = new QListWidget();
    m_eventsList->addItem("📅 Sprint Planning - Tomorrow 9:00 AM");
    m_eventsList->addItem("🎯 Milestone Review - Friday 2:00 PM");
    m_eventsList->addItem("👥 Team Standup - Daily 10:00 AM");
    m_eventsList->addItem("📊 Client Demo - Next Monday 3:00 PM");
    m_eventsList->addItem("🔄 Retrospective - Next Friday 4:00 PM");

    eventsPanelLayout->addWidget(m_eventsList);

    calendarSplitter->addWidget(calendarPanel);
    calendarSplitter->addWidget(eventsPanel);
    calendarSplitter->setSizes({500, 300});

    layout->addWidget(calendarSplitter);

    return m_calendarPage;
}

QWidget* ProjectManagementApp::createSettingsPage() {
    m_settingsPage = new QWidget();
    auto* layout = new QVBoxLayout(m_settingsPage);

    // Settings header
    auto* headerLayout = new QHBoxLayout();
    headerLayout->addWidget(new QLabel("Application Settings"));
    headerLayout->addStretch();

    layout->addLayout(headerLayout);

    // Settings content
    auto* settingsScroll = new QScrollArea();
    auto* settingsWidget = new QWidget();
    auto* settingsLayout = new QVBoxLayout(settingsWidget);

    // Appearance settings
    auto* appearanceGroup = new QGroupBox("Appearance");
    auto* appearanceLayout = new QFormLayout(appearanceGroup);

    m_themeCombo = new Components::FluentComboBox();
    m_themeCombo->addItems({"System", "Light", "Dark"});
    m_themeCombo->setCurrentIndex(m_darkMode ? 2 : 1);

    appearanceLayout->addRow("Theme:", m_themeCombo);

    // Behavior settings
    auto* behaviorGroup = new QGroupBox("Behavior");
    auto* behaviorLayout = new QFormLayout(behaviorGroup);

    m_notificationsCheck = new Components::FluentCheckBox("Enable notifications");
    m_notificationsCheck->setChecked(m_notificationsEnabled);

    m_animationsCheck = new Components::FluentCheckBox("Enable animations");
    m_animationsCheck->setChecked(m_animationsEnabled);

    behaviorLayout->addRow(m_notificationsCheck);
    behaviorLayout->addRow(m_animationsCheck);

    // Performance settings
    auto* performanceGroup = new QGroupBox("Performance");
    auto* performanceLayout = new QFormLayout(performanceGroup);

    m_refreshIntervalSlider = new QSlider(Qt::Horizontal);
    m_refreshIntervalSlider->setRange(10, 300);
    m_refreshIntervalSlider->setValue(m_refreshInterval);
    m_refreshIntervalSlider->setTickPosition(QSlider::TicksBelow);
    m_refreshIntervalSlider->setTickInterval(30);

    auto* refreshLabel = new QLabel(QString("Refresh interval: %1 seconds").arg(m_refreshInterval));
    connect(m_refreshIntervalSlider, &QSlider::valueChanged, [refreshLabel](int value) {
        refreshLabel->setText(QString("Refresh interval: %1 seconds").arg(value));
    });

    performanceLayout->addRow(refreshLabel);
    performanceLayout->addRow(m_refreshIntervalSlider);

    // Add groups to layout
    settingsLayout->addWidget(appearanceGroup);
    settingsLayout->addWidget(behaviorGroup);
    settingsLayout->addWidget(performanceGroup);
    settingsLayout->addStretch();

    settingsScroll->setWidget(settingsWidget);
    settingsScroll->setWidgetResizable(true);

    layout->addWidget(settingsScroll);

    return m_settingsPage;
}

// Slot implementations
void ProjectManagementApp::setupConnections() {
    // Navigation connections
    connect(m_navigationView, &Components::FluentNavigationView::selectionChanged,
            this, &ProjectManagementApp::onNavigationChanged);

    // Timer connections
    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(m_refreshInterval * 1000);
    connect(m_refreshTimer, &QTimer::timeout, this, &ProjectManagementApp::refreshDashboard);
    m_refreshTimer->start();

    m_metricsTimer = new QTimer(this);
    m_metricsTimer->setInterval(10000); // Update metrics every 10 seconds
    connect(m_metricsTimer, &QTimer::timeout, this, &ProjectManagementApp::updateMetrics);
    m_metricsTimer->start();

    m_notificationTimer = new QTimer(this);
    m_notificationTimer->setInterval(60000); // Check for notifications every minute
    connect(m_notificationTimer, &QTimer::timeout, this, &ProjectManagementApp::onNotificationReceived);
    m_notificationTimer->start();
}

void ProjectManagementApp::setupTheme() {
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(m_darkMode ? Styling::FluentThemeMode::Dark : Styling::FluentThemeMode::Light);
}

void ProjectManagementApp::setupPerformanceMonitoring() {
    m_performanceMonitor = &Core::FluentPerformanceMonitor::instance();
    // Note: Using basic performance monitoring without advanced features
}

void ProjectManagementApp::setupAccessibility() {
    // Enable accessibility
    QAccessible::setActive(true);

    // Set accessible properties for main window
    setAccessibleName("Project Management Application");
    setAccessibleDescription("A comprehensive project management dashboard with team collaboration features");
}

void ProjectManagementApp::loadApplicationData() {
    // Load sample tasks
    m_tasks.clear();
    for (int i = 1; i <= 10; ++i) {
        Task task;
        task.id = QString("TASK-%1").arg(i, 3, 10, QChar('0'));
        task.title = QString("Task %1: Sample Work Item").arg(i);
        task.description = QString("This is a sample task description for task %1").arg(i);
        task.assignee = QString("Team Member %1").arg((i % 4) + 1);
        task.status = (i % 3 == 0) ? "Completed" : (i % 3 == 1) ? "In Progress" : "Not Started";
        task.progress = (i % 3 == 0) ? 100 : QRandomGenerator::global()->bounded(20, 80);
        task.dueDate = QDateTime::currentDateTime().addDays(QRandomGenerator::global()->bounded(1, 30));
        task.priority = (i % 3 == 0) ? "High" : (i % 3 == 1) ? "Medium" : "Low";
        m_tasks.append(task);
    }

    // Load sample team members
    m_teamMembers.clear();
    QStringList names = {"Sarah Johnson", "Mike Chen", "David Smith", "Lisa Wang", "Tom Brown", "Emma Davis"};
    QStringList roles = {"Frontend Lead", "Frontend Developer", "Backend Lead", "Backend Developer", "QA Lead", "UI/UX Designer"};

    for (int i = 0; i < names.size(); ++i) {
        TeamMember member;
        member.id = QString("MEMBER-%1").arg(i + 1);
        member.name = names[i];
        member.role = roles[i];
        member.email = QString("%1@company.com").arg(names[i].toLower().replace(" ", "."));
        member.status = (i % 3 == 0) ? "Available" : (i % 3 == 1) ? "Busy" : "Away";
        member.skills = {"JavaScript", "React", "Node.js", "Python", "Qt", "C++"};
        member.tasksAssigned = QRandomGenerator::global()->bounded(2, 8);
        m_teamMembers.append(member);
    }

    // Update metrics
    updateDashboardMetrics();
    refreshTaskList();
}

// Slot implementations
void ProjectManagementApp::newProject() {
    profileOperation("New Project", [this]() {
        QString projectName = QInputDialog::getText(this, "New Project", "Enter project name:");
        if (!projectName.isEmpty()) {
            m_currentProject = projectName;
            m_projectLabel->setText(QString("Project: %1").arg(m_currentProject));
            showNotification(QString("Created new project: %1").arg(projectName), "success");
            refreshDashboard();
        }
    });
}

void ProjectManagementApp::openProject() {
    profileOperation("Open Project", [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Project", "", "Project Files (*.proj)");
        if (!fileName.isEmpty()) {
            showNotification("Project opened successfully", "success");
            refreshDashboard();
        }
    });
}

void ProjectManagementApp::saveProject() {
    profileOperation("Save Project", [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Project", "", "Project Files (*.proj)");
        if (!fileName.isEmpty()) {
            showNotification("Project saved successfully", "success");
        }
    });
}

void ProjectManagementApp::exportProject() {
    profileOperation("Export Project", [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Export Project", "", "CSV Files (*.csv)");
        if (!fileName.isEmpty()) {
            showNotification("Project data exported successfully", "success");
        }
    });
}

void ProjectManagementApp::importData() {
    profileOperation("Import Data", [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "Import Data", "", "CSV Files (*.csv)");
        if (!fileName.isEmpty()) {
            showNotification("Data imported successfully", "success");
            refreshDashboard();
        }
    });
}

void ProjectManagementApp::toggleTheme() {
    m_darkMode = !m_darkMode;
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(m_darkMode ? Styling::FluentThemeMode::Dark : Styling::FluentThemeMode::Light);

    if (m_themeCombo) {
        m_themeCombo->setCurrentIndex(m_darkMode ? 2 : 1);
    }

    showNotification(QString("Switched to %1 theme").arg(m_darkMode ? "dark" : "light"), "info");
}

void ProjectManagementApp::toggleFullScreen() {
    if (isFullScreen()) {
        showNormal();
        showNotification("Exited full screen mode", "info");
    } else {
        showFullScreen();
        showNotification("Entered full screen mode", "info");
    }
}

void ProjectManagementApp::showPreferences() {
    m_navigationView->setSelectedIndex(6); // Settings page
    showNotification("Opened application preferences", "info");
}

void ProjectManagementApp::showAbout() {
    QMessageBox::about(this, "About Project Management App",
        "FluentQt Real-world Application Example\n\n"
        "This comprehensive example demonstrates:\n"
        "• Professional application architecture\n"
        "• Integration of multiple FluentQt components\n"
        "• Real-world data management patterns\n"
        "• Modern UI/UX design principles\n"
        "• Performance optimization\n"
        "• Accessibility support\n"
        "• Responsive design\n\n"
        "Built with FluentQt and Qt 6.x");
}

void ProjectManagementApp::onNavigationChanged(int index) {
    profileOperation("Navigation Change", [this, index]() {
        animatePageTransition();

        QStringList pageNames = {"Dashboard", "Projects", "Tasks", "Team", "Analytics", "Calendar", "Settings"};
        if (index >= 0 && index < pageNames.size()) {
            m_statusLabel->setText(QString("Viewing %1").arg(pageNames[index]));

            // Note: Screen reader announcement would be implemented here
        }
    });
}

void ProjectManagementApp::addTask() {
    profileOperation("Add Task", [this]() {
        // Simple task creation dialog
        bool ok;
        QString taskTitle = QInputDialog::getText(this, "Add Task", "Enter task title:", QLineEdit::Normal, "", &ok);
        if (ok && !taskTitle.isEmpty()) {
            Task newTask;
            newTask.id = QString("TASK-%1").arg(m_tasks.size() + 1, 3, 10, QChar('0'));
            newTask.title = taskTitle;
            newTask.description = "New task description";
            newTask.assignee = "Unassigned";
            newTask.status = "Not Started";
            newTask.progress = 0;
            newTask.dueDate = QDateTime::currentDateTime().addDays(7);
            newTask.priority = "Medium";

            m_tasks.append(newTask);
            refreshTaskList();
            updateDashboardMetrics();

            showNotification(QString("Added new task: %1").arg(taskTitle), "success");
        }
    });
}

void ProjectManagementApp::addTeamMember() {
    profileOperation("Add Team Member", [this]() {
        bool ok;
        QString memberName = QInputDialog::getText(this, "Add Team Member", "Enter member name:", QLineEdit::Normal, "", &ok);
        if (ok && !memberName.isEmpty()) {
            TeamMember newMember;
            newMember.id = QString("MEMBER-%1").arg(m_teamMembers.size() + 1);
            newMember.name = memberName;
            newMember.role = "Team Member";
            newMember.email = QString("%1@company.com").arg(memberName.toLower().replace(" ", "."));
            newMember.status = "Available";
            newMember.skills = {"General"};
            newMember.tasksAssigned = 0;

            m_teamMembers.append(newMember);
            refreshTeamData();
            updateDashboardMetrics();

            showNotification(QString("Added new team member: %1").arg(memberName), "success");
        }
    });
}

void ProjectManagementApp::generateReport() {
    profileOperation("Generate Report", [this]() {
        m_operationProgress->setVisible(true);
        m_operationProgress->setValue(0);

        // Simulate report generation
        QTimer* progressTimer = new QTimer(this);
        int progress = 0;

        connect(progressTimer, &QTimer::timeout, [this, progressTimer, &progress]() {
            progress += 10;
            m_operationProgress->setValue(progress);

            if (progress >= 100) {
                progressTimer->stop();
                progressTimer->deleteLater();
                m_operationProgress->setVisible(false);
                showNotification("Report generated successfully", "success");
            }
        });

        progressTimer->start(100);
    });
}

void ProjectManagementApp::refreshDashboard() {
    profileOperation("Refresh Dashboard", [this]() {
        updateDashboardMetrics();
        refreshTaskList();
        refreshTeamData();

        // Add new activity
        QString activity = QString("🔄 Dashboard refreshed at %1").arg(QDateTime::currentDateTime().toString("hh:mm:ss"));
        m_activityList->insertItem(0, activity);

        // Keep only last 10 activities
        while (m_activityList->count() > 10) {
            delete m_activityList->takeItem(m_activityList->count() - 1);
        }

        showNotification("Dashboard refreshed", "info");
    });
}

void ProjectManagementApp::updateMetrics() {
    // Update project metrics
    m_metrics.totalTasks = m_tasks.size();
    m_metrics.completedTasks = std::count_if(m_tasks.begin(), m_tasks.end(),
        [](const Task& task) { return task.status == "Completed"; });
    m_metrics.overdueTasks = std::count_if(m_tasks.begin(), m_tasks.end(),
        [](const Task& task) { return task.dueDate < QDateTime::currentDateTime() && task.status != "Completed"; });
    m_metrics.teamMembers = m_teamMembers.size();

    if (m_metrics.totalTasks > 0) {
        m_metrics.projectProgress = (double)m_metrics.completedTasks / m_metrics.totalTasks * 100.0;
    }

    updateDashboardMetrics();
}

void ProjectManagementApp::updateDashboardMetrics() {
    if (m_metricCards.size() >= 6) {
        // Update metric card values
        QStringList newValues = {
            QString::number(m_metrics.totalTasks),
            QString::number(m_metrics.completedTasks),
            QString::number(m_metrics.totalTasks - m_metrics.completedTasks),
            QString::number(m_metrics.teamMembers),
            QString("%1%").arg(m_metrics.projectProgress, 0, 'f', 1),
            QString("%1h").arg(m_metrics.hoursLogged)
        };

        for (int i = 0; i < qMin(m_metricCards.size(), newValues.size()); ++i) {
            // Update the card subtitle
            m_metricCards[i]->setSubtitle(newValues[i]);
        }
    }
}

void ProjectManagementApp::refreshTaskList() {
    if (!m_taskTable) return;

    m_taskTable->setRowCount(m_tasks.size());

    for (int i = 0; i < m_tasks.size(); ++i) {
        const Task& task = m_tasks[i];
        m_taskTable->setItem(i, 0, new QTableWidgetItem(task.title));
        m_taskTable->setItem(i, 1, new QTableWidgetItem(task.assignee));
        m_taskTable->setItem(i, 2, new QTableWidgetItem(task.status));
        m_taskTable->setItem(i, 3, new QTableWidgetItem(QString("%1%").arg(task.progress)));
        m_taskTable->setItem(i, 4, new QTableWidgetItem(task.dueDate.toString("yyyy-MM-dd")));
        m_taskTable->setItem(i, 5, new QTableWidgetItem(task.priority));
    }
}

void ProjectManagementApp::refreshTeamData() {
    // Update team-related displays
    if (m_memberCard) {
        m_memberCard->setSubtitle(QString("%1 team members").arg(m_teamMembers.size()));
    }
}

void ProjectManagementApp::onNotificationReceived() {
    // Simulate random notifications
    QStringList notifications = {
        "New task assigned to you",
        "Meeting reminder: Daily standup in 15 minutes",
        "Project milestone completed",
        "New team member joined the project",
        "Weekly report is ready for review"
    };

    if (m_notificationsEnabled && QRandomGenerator::global()->bounded(100) < 20) { // 20% chance
        QString notification = notifications[QRandomGenerator::global()->bounded(notifications.size())];
        showNotification(notification, "info");
    }
}

void ProjectManagementApp::onPerformanceUpdate() {
    // Performance monitoring would be implemented here
    // Note: Using simplified performance tracking
}

void ProjectManagementApp::animatePageTransition() {
    if (m_animationsEnabled) {
        // Simple fade animation for page transitions
        Animation::FluentAnimationConfig config;
        config.duration = std::chrono::milliseconds(200);
        auto fadeAnimation = Animation::FluentAnimator::fadeIn(centralWidget(), config);
        fadeAnimation->start();
    }
}

void ProjectManagementApp::showNotification(const QString& message, const QString& type) {
    // Show notification in status bar
    QString icon = (type == "success") ? "✅" : (type == "warning") ? "⚠️" : "ℹ️";
    m_statusLabel->setText(QString("%1 %2").arg(icon, message));

    // Clear notification after 3 seconds
    QTimer::singleShot(3000, [this]() {
        m_statusLabel->setText("Ready");
    });
}

void ProjectManagementApp::profileOperation(const QString& operation, std::function<void()> func) {
    if (m_performanceMonitor) {
        m_performanceMonitor->beginProfile(operation);
        func();
        m_performanceMonitor->endProfile(operation);
    } else {
        func();
    }
}

void ProjectManagementApp::saveApplicationSettings() {
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("darkMode", m_darkMode);
    settings.setValue("notifications", m_notificationsEnabled);
    settings.setValue("animations", m_animationsEnabled);
    settings.setValue("refreshInterval", m_refreshInterval);
}

// Placeholder implementations for remaining slots
void ProjectManagementApp::editTask() { showNotification("Edit task functionality", "info"); }
void ProjectManagementApp::deleteTask() { showNotification("Delete task functionality", "info"); }
void ProjectManagementApp::markTaskComplete() { showNotification("Mark task complete functionality", "info"); }
void ProjectManagementApp::updateTaskProgress() { showNotification("Update task progress functionality", "info"); }
void ProjectManagementApp::editTeamMember() { showNotification("Edit team member functionality", "info"); }
void ProjectManagementApp::removeTeamMember() { showNotification("Remove team member functionality", "info"); }
void ProjectManagementApp::assignTask() { showNotification("Assign task functionality", "info"); }
void ProjectManagementApp::exportAnalytics() { showNotification("Export analytics functionality", "info"); }
void ProjectManagementApp::onTabChanged(int index) { Q_UNUSED(index) }
void ProjectManagementApp::onDataChanged() { refreshDashboard(); }

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Note: High DPI attributes are automatically enabled in Qt 6

    // Set application properties
    app.setApplicationName("FluentQt Project Management");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("FluentQt Examples");
    app.setOrganizationDomain("fluentqt.org");

    // Initialize FluentQt theme system
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);

    // Show main application
    ProjectManagementApp app_window;
    app_window.show();

    return app.exec();
}

// Note: MOC file will be generated during compilation
