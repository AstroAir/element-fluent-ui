// examples/TimelineShowcaseExample.cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>
#include <QTimer>
#include <QRandomGenerator>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>

// FluentQt Timeline Components
#include "FluentQt/Components/FluentTimeline.h"
#include "FluentQt/Components/FluentTimelineItem.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class TimelineShowcaseWindow : public QMainWindow {
    Q_OBJECT

public:
    TimelineShowcaseWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupExamples();
        setupControls();
        
        // Apply Fluent theme
        auto& theme = Styling::FluentTheme::instance();
        setStyleSheet(QString("QMainWindow { background-color: %1; }")
                     .arg(theme.color("backgroundPrimary").name()));
        
        setWindowTitle("FluentQt Timeline Showcase");
        resize(1200, 800);
    }

private slots:
    void onOrientationChanged() {
        auto orientation = m_orientationCombo->currentData().value<Components::FluentTimelineOrientation>();
        m_basicTimeline->setOrientation(orientation);
        m_projectTimeline->setOrientation(orientation);
    }
    
    void onAlignmentChanged() {
        auto alignment = m_alignmentCombo->currentData().value<Components::FluentTimelineAlignment>();
        m_basicTimeline->setAlignment(alignment);
        m_projectTimeline->setAlignment(alignment);
    }
    
    void onStyleChanged() {
        auto style = m_styleCombo->currentData().value<Components::FluentTimelineStyle>();
        m_basicTimeline->setTimelineStyle(style);
        m_projectTimeline->setTimelineStyle(style);
    }
    
    void onAnimatedChanged(bool animated) {
        m_basicTimeline->setAnimated(animated);
        m_projectTimeline->setAnimated(animated);
        m_interactiveTimeline->setAnimated(animated);
    }
    
    void onScrollableChanged(bool scrollable) {
        m_basicTimeline->setScrollable(scrollable);
        m_projectTimeline->setScrollable(scrollable);
    }
    
    void onShowConnectorsChanged(bool show) {
        m_basicTimeline->setShowConnectors(show);
        m_projectTimeline->setShowConnectors(show);
        m_interactiveTimeline->setShowConnectors(show);
    }
    
    void onItemSpacingChanged(int spacing) {
        m_basicTimeline->setItemSpacing(spacing);
        m_projectTimeline->setItemSpacing(spacing);
    }
    
    void addRandomItem() {
        static int itemCounter = 0;
        static QStringList titles = {
            "New Feature Implementation", "Bug Fix", "Code Review", 
            "Testing Phase", "Documentation Update", "Performance Optimization",
            "Security Audit", "User Interface Polish", "Database Migration"
        };
        
        static QStringList descriptions = {
            "Implementing new functionality as requested",
            "Fixing critical issues found in production",
            "Reviewing code changes for quality assurance",
            "Comprehensive testing of new features",
            "Updating documentation for clarity",
            "Optimizing performance bottlenecks"
        };
        
        QString title = titles[QRandomGenerator::global()->bounded(titles.size())];
        QString description = descriptions[QRandomGenerator::global()->bounded(descriptions.size())];
        
        auto* item = new Components::FluentTimelineItem(
            QString("%1 #%2").arg(title).arg(++itemCounter), 
            m_interactiveTimeline
        );
        item->setDescription(description);
        item->setDateTime(QDateTime::currentDateTime().addSecs(itemCounter * 3600));
        
        // Random state
        auto states = {
            Components::FluentTimelineItemState::Pending,
            Components::FluentTimelineItemState::Current,
            Components::FluentTimelineItemState::Completed,
            Components::FluentTimelineItemState::Warning
        };
        auto state = *std::next(states.begin(), QRandomGenerator::global()->bounded(qsizetype(states.size())));
        item->setItemState(state);
        
        m_interactiveTimeline->addItem(item);
        m_interactiveTimeline->animateToItem(item);
    }
    
    void clearItems() {
        m_interactiveTimeline->clearItems();
    }
    
    void expandAllItems() {
        m_interactiveTimeline->expandAll();
    }
    
    void collapseAllItems() {
        m_interactiveTimeline->collapseAll();
    }
    
    void onTimelineItemClicked(Components::FluentTimelineItem* item, int index) {
        m_statusLabel->setText(QString("Clicked: %1 (Index: %2)").arg(item->title()).arg(index));
        
        // Highlight clicked item temporarily
        QTimer::singleShot(2000, [this]() {
            m_statusLabel->setText("Ready");
        });
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        auto* mainLayout = new QHBoxLayout(centralWidget);
        
        // Create splitter for resizable panels
        auto* splitter = new QSplitter(Qt::Horizontal, this);
        mainLayout->addWidget(splitter);
        
        // Left panel - Examples
        auto* examplesWidget = new QWidget();
        auto* examplesLayout = new QVBoxLayout(examplesWidget);
        
        // Tab widget for different examples
        m_tabWidget = new QTabWidget();
        examplesLayout->addWidget(m_tabWidget);
        
        // Status bar
        m_statusLabel = new QLabel("Ready");
        examplesLayout->addWidget(m_statusLabel);
        
        splitter->addWidget(examplesWidget);
        
        // Right panel - Controls
        auto* controlsWidget = new QWidget();
        controlsWidget->setMaximumWidth(300);
        controlsWidget->setMinimumWidth(250);
        setupControlsPanel(controlsWidget);
        splitter->addWidget(controlsWidget);
        
        // Set splitter proportions
        splitter->setStretchFactor(0, 3);
        splitter->setStretchFactor(1, 1);
    }
    
    void setupControlsPanel(QWidget* parent) {
        auto* layout = new QVBoxLayout(parent);
        
        // Timeline Configuration
        auto* configGroup = new QGroupBox("Timeline Configuration");
        auto* configLayout = new QVBoxLayout(configGroup);
        
        // Orientation
        configLayout->addWidget(new QLabel("Orientation:"));
        m_orientationCombo = new QComboBox();
        m_orientationCombo->addItem("Vertical", QVariant::fromValue(Components::FluentTimelineOrientation::Vertical));
        m_orientationCombo->addItem("Horizontal", QVariant::fromValue(Components::FluentTimelineOrientation::Horizontal));
        connect(m_orientationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &TimelineShowcaseWindow::onOrientationChanged);
        configLayout->addWidget(m_orientationCombo);
        
        // Alignment
        configLayout->addWidget(new QLabel("Alignment:"));
        m_alignmentCombo = new QComboBox();
        m_alignmentCombo->addItem("Left", QVariant::fromValue(Components::FluentTimelineAlignment::Left));
        m_alignmentCombo->addItem("Right", QVariant::fromValue(Components::FluentTimelineAlignment::Right));
        m_alignmentCombo->addItem("Center", QVariant::fromValue(Components::FluentTimelineAlignment::Center));
        m_alignmentCombo->addItem("Alternate", QVariant::fromValue(Components::FluentTimelineAlignment::Alternate));
        connect(m_alignmentCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &TimelineShowcaseWindow::onAlignmentChanged);
        configLayout->addWidget(m_alignmentCombo);
        
        // Style
        configLayout->addWidget(new QLabel("Style:"));
        m_styleCombo = new QComboBox();
        m_styleCombo->addItem("Default", QVariant::fromValue(Components::FluentTimelineStyle::Default));
        m_styleCombo->addItem("Compact", QVariant::fromValue(Components::FluentTimelineStyle::Compact));
        m_styleCombo->addItem("Detailed", QVariant::fromValue(Components::FluentTimelineStyle::Detailed));
        m_styleCombo->addItem("Minimal", QVariant::fromValue(Components::FluentTimelineStyle::Minimal));
        connect(m_styleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &TimelineShowcaseWindow::onStyleChanged);
        configLayout->addWidget(m_styleCombo);
        
        // Checkboxes
        m_animatedCheck = new QCheckBox("Animated");
        m_animatedCheck->setChecked(true);
        connect(m_animatedCheck, &QCheckBox::toggled, this, &TimelineShowcaseWindow::onAnimatedChanged);
        configLayout->addWidget(m_animatedCheck);
        
        m_scrollableCheck = new QCheckBox("Scrollable");
        connect(m_scrollableCheck, &QCheckBox::toggled, this, &TimelineShowcaseWindow::onScrollableChanged);
        configLayout->addWidget(m_scrollableCheck);
        
        m_connectorsCheck = new QCheckBox("Show Connectors");
        m_connectorsCheck->setChecked(true);
        connect(m_connectorsCheck, &QCheckBox::toggled, this, &TimelineShowcaseWindow::onShowConnectorsChanged);
        configLayout->addWidget(m_connectorsCheck);
        
        // Item spacing
        configLayout->addWidget(new QLabel("Item Spacing:"));
        m_spacingSpin = new QSpinBox();
        m_spacingSpin->setRange(0, 50);
        m_spacingSpin->setValue(16);
        connect(m_spacingSpin, QOverload<int>::of(&QSpinBox::valueChanged),
                this, &TimelineShowcaseWindow::onItemSpacingChanged);
        configLayout->addWidget(m_spacingSpin);
        
        layout->addWidget(configGroup);
        
        // Interactive Controls
        auto* interactiveGroup = new QGroupBox("Interactive Controls");
        auto* interactiveLayout = new QVBoxLayout(interactiveGroup);
        
        auto* addButton = new Components::FluentButton("Add Random Item");
        connect(addButton, &Components::FluentButton::clicked, this, &TimelineShowcaseWindow::addRandomItem);
        interactiveLayout->addWidget(addButton);
        
        auto* clearButton = new Components::FluentButton("Clear All Items");
        connect(clearButton, &Components::FluentButton::clicked, this, &TimelineShowcaseWindow::clearItems);
        interactiveLayout->addWidget(clearButton);
        
        auto* expandButton = new Components::FluentButton("Expand All");
        connect(expandButton, &Components::FluentButton::clicked, this, &TimelineShowcaseWindow::expandAllItems);
        interactiveLayout->addWidget(expandButton);
        
        auto* collapseButton = new Components::FluentButton("Collapse All");
        connect(collapseButton, &Components::FluentButton::clicked, this, &TimelineShowcaseWindow::collapseAllItems);
        interactiveLayout->addWidget(collapseButton);
        
        layout->addWidget(interactiveGroup);
        
        layout->addStretch();
    }
    
    void setupExamples() {
        setupBasicExample();
        setupProjectExample();
        setupInteractiveExample();
        setupVariantsExample();
    }
    
    void setupControls() {
        // Initialize controls with current timeline settings
        // This will be called after examples are set up
    }
    
    void setupBasicExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        layout->addWidget(new QLabel("Basic Timeline Example"));
        
        m_basicTimeline = new Components::FluentTimeline(widget);
        
        // Add basic items
        m_basicTimeline->addTextItem("Project Started", "Initial project setup and planning");
        m_basicTimeline->addTextItem("Development Phase", "Core functionality implementation");
        m_basicTimeline->addTextItem("Testing Phase", "Quality assurance and bug fixes");
        m_basicTimeline->addTextItem("Release", "Product launch and deployment");
        
        layout->addWidget(m_basicTimeline);
        m_tabWidget->addTab(widget, "Basic");
    }
    
    void setupProjectExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        layout->addWidget(new QLabel("Project Timeline Example"));
        
        m_projectTimeline = Components::FluentTimeline::createDetailedTimeline(widget);
        
        // Add project milestones and tasks
        auto* kickoff = Components::FluentTimelineItem::createMilestone(
            "Project Kickoff", 
            QDateTime::fromString("2024-01-15", "yyyy-MM-dd")
        );
        kickoff->setItemState(Components::FluentTimelineItemState::Completed);
        m_projectTimeline->addItem(kickoff);
        
        auto* phase1 = Components::FluentTimelineItem::createTask(
            "Phase 1: Core Development",
            Components::FluentTimelineItemState::Completed
        );
        phase1->setDescription("Implement core functionality and basic UI");
        m_projectTimeline->addItem(phase1);
        
        auto* phase2 = Components::FluentTimelineItem::createTask(
            "Phase 2: Advanced Features",
            Components::FluentTimelineItemState::Current
        );
        phase2->setDescription("Add advanced features and integrations");
        m_projectTimeline->addItem(phase2);
        
        auto* phase3 = Components::FluentTimelineItem::createTask(
            "Phase 3: Testing & Polish",
            Components::FluentTimelineItemState::Pending
        );
        phase3->setDescription("Quality assurance and final polish");
        m_projectTimeline->addItem(phase3);
        
        auto* release = Components::FluentTimelineItem::createMilestone(
            "Release v1.0",
            QDateTime::fromString("2024-06-01", "yyyy-MM-dd")
        );
        release->setItemState(Components::FluentTimelineItemState::Pending);
        m_projectTimeline->addItem(release);
        
        layout->addWidget(m_projectTimeline);
        m_tabWidget->addTab(widget, "Project");
    }
    
    void setupInteractiveExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        layout->addWidget(new QLabel("Interactive Timeline Example"));
        
        m_interactiveTimeline = new Components::FluentTimeline(widget);
        m_interactiveTimeline->setScrollable(true);
        
        // Connect interaction signals
        connect(m_interactiveTimeline, &Components::FluentTimeline::itemClicked,
                this, &TimelineShowcaseWindow::onTimelineItemClicked);
        
        // Add some initial items
        for (int i = 0; i < 5; ++i) {
            addRandomItem();
        }
        
        layout->addWidget(m_interactiveTimeline);
        m_tabWidget->addTab(widget, "Interactive");
    }
    
    void setupVariantsExample() {
        auto* widget = new QWidget();
        auto* layout = new QVBoxLayout(widget);
        
        layout->addWidget(new QLabel("Timeline Variants"));
        
        // Compact timeline
        auto* compactCard = new Components::FluentCard("Compact Timeline");
        auto* compactTimeline = Components::FluentTimeline::createCompactTimeline();
        compactTimeline->addTextItem("Step 1", "First step");
        compactTimeline->addTextItem("Step 2", "Second step");
        compactTimeline->addTextItem("Step 3", "Third step");
        compactCard->setContentWidget(compactTimeline);
        layout->addWidget(compactCard);
        
        // Horizontal timeline
        auto* horizontalCard = new Components::FluentCard("Horizontal Timeline");
        auto* horizontalTimeline = Components::FluentTimeline::createHorizontalTimeline();
        horizontalTimeline->addTextItem("Start", "Beginning");
        horizontalTimeline->addTextItem("Middle", "Progress");
        horizontalTimeline->addTextItem("End", "Completion");
        horizontalCard->setContentWidget(horizontalTimeline);
        layout->addWidget(horizontalCard);
        
        layout->addStretch();
        m_tabWidget->addTab(widget, "Variants");
    }

private:
    QTabWidget* m_tabWidget;
    QLabel* m_statusLabel;
    
    // Timeline instances
    Components::FluentTimeline* m_basicTimeline;
    Components::FluentTimeline* m_projectTimeline;
    Components::FluentTimeline* m_interactiveTimeline;
    
    // Controls
    QComboBox* m_orientationCombo;
    QComboBox* m_alignmentCombo;
    QComboBox* m_styleCombo;
    QCheckBox* m_animatedCheck;
    QCheckBox* m_scrollableCheck;
    QCheckBox* m_connectorsCheck;
    QSpinBox* m_spacingSpin;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Initialize FluentQt theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setAccentColor(Styling::FluentAccentColor::Blue);
    
    TimelineShowcaseWindow window;
    window.show();
    
    return app.exec();
}

#include "TimelineShowcaseExample.moc"
