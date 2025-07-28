// examples/advanced_demo.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTimer>
#include <QRandomGenerator>
#include <QStandardItemModel>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentNavigationView.h"
#include "FluentQt/Components/FluentTabView.h"
#include "FluentQt/Components/FluentTreeView.h"
#include "FluentQt/Components/FluentCalendar.h"
#include "FluentQt/Components/FluentContextMenu.h"
#include "FluentQt/Components/FluentRichTextEditor.h"
#include "FluentQt/Components/FluentChartView.h"
#include "FluentQt/Components/FluentSplitter.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class AdvancedFluentDemo : public QMainWindow {
    Q_OBJECT

public:
    AdvancedFluentDemo(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupTheme();
        setupNavigationView();
        setupDemoPages();
        setupMenus();
        setupStatusBar();
        
        setWindowTitle("FluentQt Advanced Demo - Complete Component Suite");
        setMinimumSize(1400, 900);
        resize(1600, 1000);
    }

private slots:
    void toggleTheme() {
        auto& theme = Styling::FluentTheme::instance();
        auto newMode = (theme.mode() == Styling::FluentThemeMode::Light) 
            ? Styling::FluentThemeMode::Dark 
            : Styling::FluentThemeMode::Light;
        theme.setMode(newMode);
    }

    void showAbout() {
        auto result = Components::FluentContentDialog::showMessageDialog(
            this,
            "About FluentQt",
            "A comprehensive Fluent Design component library for Qt 6.x\n\n"
            "Features:\n"
            "• 30+ production-ready components\n"
            "• High-performance 60+ FPS animations\n"
            "• Full accessibility support\n"
            "• Dynamic theming system\n"
            "• Cross-platform compatibility\n"
            "• Modern C++20 architecture",
            "OK",
            "Documentation",
            "GitHub"
        );
        
        if (result == Components::FluentDialogResult::Secondary) {
            // Open documentation
            statusBar()->showMessage("Opening documentation...", 2000);
        } else if (result == Components::FluentDialogResult::Close) {
            // Open GitHub
            statusBar()->showMessage("Opening GitHub repository...", 2000);
        }
    }

private:
    void setupTheme() {
        auto& theme = Styling::FluentTheme::instance();
        theme.setMode(Styling::FluentThemeMode::System);
        theme.setAccentColor(Styling::FluentAccentColor::Blue);
    }

    void setupNavigationView() {
        m_navigationView = new Components::FluentNavigationView(this);
        setCentralWidget(m_navigationView);

        // Add navigation items
        Components::FluentNavItem homeItem;
        homeItem.text = "Home";
        homeItem.icon = QIcon(":/icons/home.png");
        homeItem.tag = "home";
        homeItem.content = createHomePage();
        m_navigationView->addNavigationItem(homeItem);

        Components::FluentNavItem tabsItem;
        tabsItem.text = "Tabs & Trees";
        tabsItem.icon = QIcon(":/icons/tabs.png");
        tabsItem.tag = "tabs";
        tabsItem.content = createTabsPage();
        m_navigationView->addNavigationItem(tabsItem);

        Components::FluentNavItem calendarItem;
        calendarItem.text = "Calendar";
        calendarItem.icon = QIcon(":/icons/calendar.png");
        calendarItem.tag = "calendar";
        calendarItem.content = createCalendarPage();
        m_navigationView->addNavigationItem(calendarItem);

        Components::FluentNavItem editorItem;
        editorItem.text = "Rich Text";
        editorItem.icon = QIcon(":/icons/edit.png");
        editorItem.tag = "editor";
        editorItem.content = createEditorPage();
        m_navigationView->addNavigationItem(editorItem);

        Components::FluentNavItem chartsItem;
        chartsItem.text = "Charts";
        chartsItem.icon = QIcon(":/icons/chart.png");
        chartsItem.tag = "charts";
        chartsItem.content = createChartsPage();
        m_navigationView->addNavigationItem(chartsItem);

        Components::FluentNavItem layoutItem;
        layoutItem.text = "Layouts";
        layoutItem.icon = QIcon(":/icons/layout.png");
        layoutItem.tag = "layouts";
        layoutItem.content = createLayoutsPage();
        m_navigationView->addNavigationItem(layoutItem);

        m_navigationView->setSelectedItemTag("home");
    }

    void setupDemoPages() {
        // Pages are created in setupNavigationView
    }

    void setupMenus() {
        // File menu
        auto* fileMenu = menuBar()->addMenu("&File");
        fileMenu->addAction("&New", this, []() {}, QKeySequence::New);
        fileMenu->addAction("&Open", this, []() {}, QKeySequence::Open);
        fileMenu->addAction("&Save", this, []() {}, QKeySequence::Save);
        fileMenu->addSeparator();
        fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);

        // View menu
        auto* viewMenu = menuBar()->addMenu("&View");
        viewMenu->addAction("Toggle &Theme", this, &AdvancedFluentDemo::toggleTheme, QKeySequence("Ctrl+T"));
        viewMenu->addAction("&Full Screen", this, []() {}, QKeySequence::FullScreen);
        viewMenu->addSeparator();
        viewMenu->addAction("&Zoom In", this, []() {}, QKeySequence::ZoomIn);
        viewMenu->addAction("Zoom &Out", this, []() {}, QKeySequence::ZoomOut);
        viewMenu->addAction("&Reset Zoom", this, []() {}, QKeySequence("Ctrl+0"));

        // Help menu
        auto* helpMenu = menuBar()->addMenu("&Help");
        helpMenu->addAction("&About", this, &AdvancedFluentDemo::showAbout);
        helpMenu->addAction("About &Qt", qApp, &QApplication::aboutQt);
    }

    void setupStatusBar() {
        statusBar()->showMessage("Ready", 2000);
        
        // Add theme indicator
        auto* themeLabel = new QLabel("Theme: System");
        statusBar()->addPermanentWidget(themeLabel);
        
        connect(&Styling::FluentTheme::instance(), &Styling::FluentTheme::modeChanged,
                [themeLabel](Styling::FluentThemeMode mode) {
                    QString modeText;
                    switch (mode) {
                        case Styling::FluentThemeMode::Light: modeText = "Light"; break;
                        case Styling::FluentThemeMode::Dark: modeText = "Dark"; break;
                        case Styling::FluentThemeMode::System: modeText = "System"; break;
                    }
                    themeLabel->setText(QString("Theme: %1").arg(modeText));
                });
    }

    QWidget* createHomePage() {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);

        // Welcome section
        auto* welcomeCard = new Components::FluentCard("Welcome to FluentQt Advanced Demo", page);
        welcomeCard->setSubtitle("Explore the complete component suite with advanced features");
        welcomeCard->setElevation(Components::FluentCardElevation::Medium);

        // Feature grid
        auto* featuresLayout = new QGridLayout();
        
        auto* performanceCard = new Components::FluentCard("High Performance", page);
        performanceCard->setSubtitle("60+ FPS animations with optimized rendering pipeline");
        
        auto* componentsCard = new Components::FluentCard("30+ Components", page);
        componentsCard->setSubtitle("Complete suite of production-ready UI components");
        
        auto* accessibilityCard = new Components::FluentCard("Accessibility", page);
        accessibilityCard->setSubtitle("Full screen reader and keyboard navigation support");
        
        auto* themingCard = new Components::FluentCard("Dynamic Theming", page);
        themingCard->setSubtitle("Light/dark themes with customizable accent colors");
        
        featuresLayout->addWidget(performanceCard, 0, 0);
        featuresLayout->addWidget(componentsCard, 0, 1);
        featuresLayout->addWidget(accessibilityCard, 1, 0);
        featuresLayout->addWidget(themingCard, 1, 1);

        // Action buttons
        auto* actionLayout = new QHBoxLayout();
        
        auto* docsButton = new Components::FluentButton("Documentation", page);
        docsButton->setButtonStyle(Components::FluentButtonStyle::Primary);
        docsButton->setIcon(QIcon(":/icons/book.png"));
        
        auto* githubButton = new Components::FluentButton("GitHub", page);
        githubButton->setButtonStyle(Components::FluentButtonStyle::Outline);
        githubButton->setIcon(QIcon(":/icons/github.png"));
        
        auto* themeButton = new Components::FluentButton("Toggle Theme", page);
        themeButton->setButtonStyle(Components::FluentButtonStyle::Subtle);
        themeButton->setIcon(QIcon(":/icons/theme.png"));
        connect(themeButton, &Components::FluentButton::clicked, this, &AdvancedFluentDemo::toggleTheme);
        
        actionLayout->addWidget(docsButton);
        actionLayout->addWidget(githubButton);
        actionLayout->addWidget(themeButton);
        actionLayout->addStretch();

        layout->addWidget(welcomeCard);
        layout->addLayout(featuresLayout);
        layout->addLayout(actionLayout);
        layout->addStretch();

        return page;
    }

    QWidget* createTabsPage() {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);

        // Tab view demo
        auto* tabView = new Components::FluentTabView(page);
        tabView->setTabStyle(Components::FluentTabStyle::Underline);
        tabView->setTabsClosable(true);
        tabView->setTabsMovable(true);
        tabView->setShowAddButton(true);

        // Tree view tab
        auto* treeView = new Components::FluentTreeView(tabView);
        treeView->setFilteringEnabled(true);
        treeView->setDragDropEnabled(true);
        
        // Add sample tree data
        auto* rootItem = treeView->addTopLevelItem("Root Node");
        rootItem->setIcon(QIcon(":/icons/folder.png"));
        
        for (int i = 0; i < 5; ++i) {
            auto* childItem = treeView->addChildItem(rootItem, QString("Child %1").arg(i + 1));
            childItem->setIcon(QIcon(":/icons/file.png"));
            
            for (int j = 0; j < 3; ++j) {
                auto* grandChild = treeView->addChildItem(childItem, QString("Grandchild %1.%2").arg(i + 1).arg(j + 1));
                grandChild->setIcon(QIcon(":/icons/item.png"));
            }
        }

        treeView->expandAll();
        
        Components::FluentTabData treeTabData;
        treeTabData.text = "Tree View";
        treeTabData.icon = QIcon(":/icons/tree.png");
        treeTabData.content = treeView;
        tabView->addTab(treeTabData);

        // List view tab
        auto* listWidget = new QWidget();
        auto* listLayout = new QVBoxLayout(listWidget);
        
        auto* listView = new Components::FluentListView(listWidget);
        listView->setAlternatingRowColors(true);
        listView->setMultiSelection(true);
        
        for (int i = 0; i < 20; ++i) {
            listView->addItem(QIcon(":/icons/item.png"), QString("List Item %1").arg(i + 1));
        }
        
        listLayout->addWidget(listView);
        
        Components::FluentTabData listTabData;
        listTabData.text = "List View";
        listTabData.icon = QIcon(":/icons/list.png");
        listTabData.content = listWidget;
        tabView->addTab(listTabData);

        // Settings tab
        auto* settingsWidget = new QWidget();
        auto* settingsLayout = new QVBoxLayout(settingsWidget);
        
        auto* settingsCard = new Components::FluentCard("Tab Settings", settingsWidget);
        auto* settingsContent = new QWidget();
        auto* settingsContentLayout = new QVBoxLayout(settingsContent);
        
        auto* styleCombo = new Components::FluentComboBox(settingsContent);
        styleCombo->addItems({"Standard", "Pill", "Underline", "Compact"});
        styleCombo->setCurrentIndex(2); // Underline
        
        auto* positionCombo = new Components::FluentComboBox(settingsContent);
        positionCombo->addItems({"Top", "Bottom", "Left", "Right"});
        
        auto* closableCheck = new Components::FluentCheckBox("Tabs closable", settingsContent);
        closableCheck->setChecked(true);
        
        auto* movableCheck = new Components::FluentCheckBox("Tabs movable", settingsContent);
        movableCheck->setChecked(true);
        
        settingsContentLayout->addWidget(new QLabel("Tab Style:"));
        settingsContentLayout->addWidget(styleCombo);
        settingsContentLayout->addWidget(new QLabel("Tab Position:"));
        settingsContentLayout->addWidget(positionCombo);
        settingsContentLayout->addWidget(closableCheck);
        settingsContentLayout->addWidget(movableCheck);
        settingsContentLayout->addStretch();
        
        settingsContent->setLayout(settingsContentLayout);
        settingsCard->setContentWidget(settingsContent);
        settingsLayout->addWidget(settingsCard);
        settingsLayout->addStretch();
        
        Components::FluentTabData settingsTabData;
        settingsTabData.text = "Settings";
        settingsTabData.icon = QIcon(":/icons/settings.png");
        settingsTabData.content = settingsWidget;
        tabView->addTab(settingsTabData);

        layout->addWidget(tabView);
        return page;
    }

    QWidget* createCalendarPage() {
        auto* page = new QWidget();
        auto* layout = new QHBoxLayout(page);

        // Calendar widget
        auto* calendar = new Components::FluentCalendar(page);
        calendar->setSelectionMode(Components::FluentCalendarSelectionMode::RangeSelection);
        calendar->setShowWeekNumbers(true);
        
        // Add some holidays
        calendar->addHoliday(QDate::currentDate().addDays(7), "Important Meeting");
        calendar->addHoliday(QDate::currentDate().addDays(14), "Project Deadline");
        calendar->addHoliday(QDate::currentDate().addDays(21), "Team Event");

        // Calendar controls
        auto* controlsCard = new Components::FluentCard("Calendar Controls", page);
        auto* controlsWidget = new QWidget();
        auto* controlsLayout = new QVBoxLayout(controlsWidget);
        
        auto* selectionModeCombo = new Components::FluentComboBox(controlsWidget);
        selectionModeCombo->addItems({"Single", "Multiple", "Range", "Week", "Month"});
        selectionModeCombo->setCurrentIndex(2); // Range
        
        auto* weekNumbersCheck = new Components::FluentCheckBox("Show week numbers", controlsWidget);
        weekNumbersCheck->setChecked(true);
        
        auto* todayButton = new Components::FluentButton("Go to Today", controlsWidget);
        todayButton->setButtonStyle(Components::FluentButtonStyle::Primary);
        connect(todayButton, &Components::FluentButton::clicked, [calendar]() {
            calendar->showToday();
        });
        
        auto* datePicker = new Components::FluentDatePicker(controlsWidget);
        datePicker->setDate(QDate::currentDate());
        
        controlsLayout->addWidget(new QLabel("Selection Mode:"));
        controlsLayout->addWidget(selectionModeCombo);
        controlsLayout->addWidget(weekNumbersCheck);
        controlsLayout->addWidget(todayButton);
        controlsLayout->addWidget(new QLabel("Date Picker:"));
        controlsLayout->addWidget(datePicker);
        controlsLayout->addStretch();
        
        controlsWidget->setLayout(controlsLayout);
        controlsCard->setContentWidget(controlsWidget);
        controlsCard->setFixedWidth(300);

        layout->addWidget(calendar);
        layout->addWidget(controlsCard);
        
        return page;
    }

    QWidget* createEditorPage() {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);

        auto* editor = new Components::FluentRichTextEditor(page);
        editor->setShowFormatBar(true);
        editor->setHtml(R"(
            <h1>Welcome to FluentQt Rich Text Editor</h1>
            <p>This is a <strong>comprehensive</strong> rich text editor with full formatting support.</p>
            <p>Features include:</p>
            <ul>
                <li>Font family and size selection</li>
                <li><strong>Bold</strong>, <em>italic</em>, and <u>underline</u> formatting</li>
                <li>Text and background <span style="color: blue;">color</span> support</li>
                <li>Paragraph alignment and indentation</li>
                <li>Lists and tables</li>
                <li>Image and link insertion</li>
                <li>Find and replace functionality</li>
                <li>Export to PDF and HTML</li>
            </ul>
            <p>Try out the formatting toolbar above!</p>
        )");

        layout->addWidget(editor);
        return page;
    }

    QWidget* createChartsPage() {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);

        auto* splitter = new Components::FluentSplitter(Qt::Horizontal, page);
        
        // Chart view
        auto* chartView = new Components::FluentChartView(splitter);
        chartView->setTitle("Sample Data Visualization");
        chartView->setAnimationEnabled(true);
        chartView->setZoomEnabled(true);
        chartView->setPanEnabled(true);
        
        // Add sample data
        Components::FluentChartSeries lineSeries;
        lineSeries.name = "Sales";
        lineSeries.type = Components::FluentChartType::Line;
        lineSeries.color = QColor(65, 105, 225);
        
        for (int i = 0; i < 12; ++i) {
            double value = 100 + 50 * qSin(i * M_PI / 6) + QRandomGenerator::global()->bounded(20);
            lineSeries.data.append(QPointF(i, value));
        }
        
        chartView->addSeries(lineSeries);
        
        Components::FluentChartSeries barSeries;
        barSeries.name = "Revenue";
        barSeries.type = Components::FluentChartType::Bar;
        barSeries.color = QColor(255, 140, 0);
        
        for (int i = 0; i < 12; ++i) {
            double value = 80 + 30 * qCos(i * M_PI / 4) + QRandomGenerator::global()->bounded(15);
            barSeries.data.append(QPointF(i, value));
        }
        
        chartView->addSeries(barSeries);
        
        // Chart controls
        auto* controlsCard = new Components::FluentCard("Chart Controls", splitter);
        auto* controlsWidget = new QWidget();
        auto* controlsLayout = new QVBoxLayout(controlsWidget);
        
        auto* chartTypeCombo = new Components::FluentComboBox(controlsWidget);
        chartTypeCombo->addItems({"Line", "Bar", "Area", "Pie", "Scatter"});
        
        auto* animationCheck = new Components::FluentCheckBox("Animation enabled", controlsWidget);
        animationCheck->setChecked(true);
        
        auto* zoomCheck = new Components::FluentCheckBox("Zoom enabled", controlsWidget);
        zoomCheck->setChecked(true);
        
        auto* panCheck = new Components::FluentCheckBox("Pan enabled", controlsWidget);
        panCheck->setChecked(true);
        
        auto* refreshButton = new Components::FluentButton("Refresh Data", controlsWidget);
        refreshButton->setButtonStyle(Components::FluentButtonStyle::Primary);
        
        auto* exportButton = new Components::FluentButton("Export Chart", controlsWidget);
        exportButton->setButtonStyle(Components::FluentButtonStyle::Outline);
        
        controlsLayout->addWidget(new QLabel("Chart Type:"));
        controlsLayout->addWidget(chartTypeCombo);
        controlsLayout->addWidget(animationCheck);
        controlsLayout->addWidget(zoomCheck);
        controlsLayout->addWidget(panCheck);
        controlsLayout->addWidget(refreshButton);
        controlsLayout->addWidget(exportButton);
        controlsLayout->addStretch();
        
        controlsWidget->setLayout(controlsLayout);
        controlsCard->setContentWidget(controlsWidget);
        
        splitter->addWidget(chartView);
        splitter->addWidget(controlsCard);
        splitter->setProportionalSizes({0.75, 0.25});
        
        layout->addWidget(splitter);
        return page;
    }

    QWidget* createLayoutsPage() {
        auto* page = new QWidget();
        auto* layout = new QVBoxLayout(page);

        // Multi-level splitter demo
        auto* mainSplitter = new Components::FluentSplitter(Qt::Horizontal, page);
        mainSplitter->setAnimatedResize(true);
        mainSplitter->setCollapsible(true);
        
        // Left panel
        auto* leftPanel = new Components::FluentCard("Left Panel", mainSplitter);
        leftPanel->setSubtitle("Collapsible side panel");
        leftPanel->setMinimumWidth(200);
        
        // Center area with vertical splitter
        auto* centerSplitter = new Components::FluentSplitter(Qt::Vertical, mainSplitter);
        
        auto* topCenter = new Components::FluentCard("Top Center", centerSplitter);
        topCenter->setSubtitle("Main content area");
        
        auto* bottomCenter = new Components::FluentCard("Bottom Center", centerSplitter);
        bottomCenter->setSubtitle("Secondary content area");
        
        centerSplitter->addWidget(topCenter);
        centerSplitter->addWidget(bottomCenter);
        centerSplitter->setProportionalSizes({0.7, 0.3});
        
        // Right panel
        auto* rightPanel = new Components::FluentCard("Right Panel", mainSplitter);
        rightPanel->setSubtitle("Properties panel");
        rightPanel->setMinimumWidth(250);
        
        // Add splitter controls
        auto* controlsWidget = new QWidget();
        auto* controlsLayout = new QVBoxLayout(controlsWidget);
        
        auto* orientationCombo = new Components::FluentComboBox(controlsWidget);
        orientationCombo->addItems({"Horizontal", "Vertical"});
        
        auto* animatedCheck = new Components::FluentCheckBox("Animated resize", controlsWidget);
        animatedCheck->setChecked(true);
        
        auto* collapsibleCheck = new Components::FluentCheckBox("Collapsible", controlsWidget);
        collapsibleCheck->setChecked(true);
        
        auto* resetButton = new Components::FluentButton("Reset Layout", controlsWidget);
        resetButton->setButtonStyle(Components::FluentButtonStyle::Primary);
        connect(resetButton, &Components::FluentButton::clicked, [mainSplitter]() {
            mainSplitter->distributeEvenly(true);
        });
        
        auto* saveButton = new Components::FluentButton("Save State", controlsWidget);
        auto* restoreButton = new Components::FluentButton("Restore State", controlsWidget);
        
        controlsLayout->addWidget(new QLabel("Orientation:"));
        controlsLayout->addWidget(orientationCombo);
        controlsLayout->addWidget(animatedCheck);
        controlsLayout->addWidget(collapsibleCheck);
        controlsLayout->addWidget(resetButton);
        controlsLayout->addWidget(saveButton);
        controlsLayout->addWidget(restoreButton);
        controlsLayout->addStretch();
        
        controlsWidget->setLayout(controlsLayout);
        rightPanel->setContentWidget(controlsWidget);
        
        mainSplitter->addWidget(leftPanel);
        mainSplitter->addWidget(centerSplitter);
        mainSplitter->addWidget(rightPanel);
        mainSplitter->setProportionalSizes({0.2, 0.6, 0.2});
        
        layout->addWidget(mainSplitter);
        return page;
    }

private:
    Components::FluentNavigationView* m_navigationView{nullptr};
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set high DPI attributes
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    // Set application properties
    app.setApplicationName("FluentQt Advanced Demo");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("FluentQt");
    app.setOrganizationDomain("fluentqt.org");

    // Initialize FluentQt theme system
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);

    // Show main window
    AdvancedFluentDemo demo;
    demo.show();

    return app.exec();
}

#include "advanced_demo.moc"