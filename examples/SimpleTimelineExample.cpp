// examples/SimpleTimelineExample.cpp
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentTimeline.h"
#include "FluentQt/Components/FluentTimelineItem.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class SimpleTimelineWindow : public QMainWindow {
    Q_OBJECT

public:
    SimpleTimelineWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        createTimelineContent();

        // Apply Fluent theme
        auto& theme = Styling::FluentTheme::instance();
        setStyleSheet(QString("QMainWindow { background-color: %1; }")
                          .arg(theme.color("backgroundPrimary").name()));

        setWindowTitle("Simple Timeline Example");
        resize(800, 600);
    }

private slots:
    void simulateProgress() {
        static int currentStep = 0;

        if (currentStep < m_timelineItems.size()) {
            // Mark current step as completed
            if (currentStep > 0) {
                m_timelineItems[currentStep - 1]->setItemState(
                    Components::FluentTimelineItemState::Completed);
            }

            // Mark current step as current
            m_timelineItems[currentStep]->setItemState(
                Components::FluentTimelineItemState::Current);

            // Animate to current item
            m_timeline->animateToItem(m_timelineItems[currentStep]);

            currentStep++;

            // Schedule next step
            if (currentStep < m_timelineItems.size()) {
                QTimer::singleShot(2000, this,
                                   &SimpleTimelineWindow::simulateProgress);
            } else {
                // Mark last item as completed
                m_timelineItems[currentStep - 1]->setItemState(
                    Components::FluentTimelineItemState::Completed);
                m_progressButton->setText("Restart Simulation");
                m_progressButton->setEnabled(true);
                currentStep = 0;
            }
        }
    }

    void resetTimeline() {
        // Reset all items to pending state
        for (auto* item : m_timelineItems) {
            item->setItemState(Components::FluentTimelineItemState::Pending);
        }

        m_progressButton->setText("Start Progress Simulation");
        m_timeline->setCurrentIndex(0);
    }

    void onItemClicked(Components::FluentTimelineItem* item,
                       [[maybe_unused]] int index) {
        m_statusLabel->setText(QString("Selected: %1").arg(item->title()));

        // Toggle item state on click
        auto currentState = item->itemState();
        switch (currentState) {
            case Components::FluentTimelineItemState::Pending:
                item->setItemState(
                    Components::FluentTimelineItemState::Current);
                break;
            case Components::FluentTimelineItemState::Current:
                item->setItemState(
                    Components::FluentTimelineItemState::Completed);
                break;
            case Components::FluentTimelineItemState::Completed:
                item->setItemState(
                    Components::FluentTimelineItemState::Pending);
                break;
            default:
                item->setItemState(
                    Components::FluentTimelineItemState::Pending);
                break;
        }
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto* mainLayout = new QVBoxLayout(centralWidget);

        // Title
        auto* titleLabel = new QLabel("Software Development Timeline");
        titleLabel->setStyleSheet(
            "font-size: 18px; font-weight: bold; margin: 10px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Timeline container
        auto* timelineCard = new Components::FluentCard("Project Progress");
        m_timeline = new Components::FluentTimeline();
        m_timeline->setScrollable(true);

        // Connect timeline signals
        connect(m_timeline, &Components::FluentTimeline::itemClicked, this,
                &SimpleTimelineWindow::onItemClicked);

        timelineCard->setContentWidget(m_timeline);
        mainLayout->addWidget(timelineCard);

        // Controls
        auto* controlsLayout = new QHBoxLayout();

        m_progressButton =
            new Components::FluentButton("Start Progress Simulation");
        m_progressButton->setButtonStyle(
            Components::FluentButtonStyle::Primary);
        connect(m_progressButton, &Components::FluentButton::clicked, [this]() {
            if (m_progressButton->text() == "Restart Simulation") {
                resetTimeline();
            } else {
                m_progressButton->setText("Simulating...");
                m_progressButton->setEnabled(false);
                simulateProgress();
            }
        });
        controlsLayout->addWidget(m_progressButton);

        auto* resetButton = new Components::FluentButton("Reset Timeline");
        connect(resetButton, &Components::FluentButton::clicked, this,
                &SimpleTimelineWindow::resetTimeline);
        controlsLayout->addWidget(resetButton);

        controlsLayout->addStretch();

        // Status label
        m_statusLabel =
            new QLabel("Click on timeline items to change their state");
        m_statusLabel->setStyleSheet("color: #666; font-style: italic;");
        controlsLayout->addWidget(m_statusLabel);

        mainLayout->addLayout(controlsLayout);
    }

    void createTimelineContent() {
        // Planning Phase
        auto* planning = Components::FluentTimelineItem::createMilestone(
            "Project Planning",
            QDateTime::fromString("2024-01-01", "yyyy-MM-dd"));
        planning->setDescription(
            "Define project scope, requirements, and timeline");
        planning->setItemState(Components::FluentTimelineItemState::Pending);
        m_timeline->addItem(planning);
        m_timelineItems.append(planning);

        // Design Phase
        auto* design = Components::FluentTimelineItem::createTask(
            "UI/UX Design", Components::FluentTimelineItemState::Pending);
        design->setDescription("Create wireframes, mockups, and design system");
        design->setDateTime(QDateTime::fromString("2024-01-15", "yyyy-MM-dd"));
        m_timeline->addItem(design);
        m_timelineItems.append(design);

        // Development Phase
        auto* development = Components::FluentTimelineItem::createTask(
            "Core Development", Components::FluentTimelineItemState::Pending);
        development->setDescription(
            "Implement core functionality and features");
        development->setDateTime(
            QDateTime::fromString("2024-02-01", "yyyy-MM-dd"));
        development->setExpandable(true);

        // Add expanded content for development
        auto* devDetails = new QWidget();
        auto* devLayout = new QVBoxLayout(devDetails);
        devLayout->addWidget(new QLabel("Development Tasks:"));
        devLayout->addWidget(new QLabel("• Frontend implementation"));
        devLayout->addWidget(new QLabel("• Backend API development"));
        devLayout->addWidget(new QLabel("• Database design"));
        devLayout->addWidget(new QLabel("• Integration testing"));
        development->setExpandedWidget(devDetails);

        m_timeline->addItem(development);
        m_timelineItems.append(development);

        // Testing Phase
        auto* testing = Components::FluentTimelineItem::createTask(
            "Quality Assurance", Components::FluentTimelineItemState::Pending);
        testing->setDescription("Comprehensive testing and bug fixes");
        testing->setDateTime(QDateTime::fromString("2024-03-01", "yyyy-MM-dd"));
        m_timeline->addItem(testing);
        m_timelineItems.append(testing);

        // Deployment Phase
        auto* deployment = Components::FluentTimelineItem::createEvent(
            "Production Deployment",
            "Deploy application to production environment",
            QDateTime::fromString("2024-03-15", "yyyy-MM-dd"));
        deployment->setItemState(Components::FluentTimelineItemState::Pending);
        m_timeline->addItem(deployment);
        m_timelineItems.append(deployment);

        // Launch Milestone
        auto* launch = Components::FluentTimelineItem::createMilestone(
            "Product Launch",
            QDateTime::fromString("2024-04-01", "yyyy-MM-dd"));
        launch->setDescription(
            "Official product launch and marketing campaign");
        launch->setItemState(Components::FluentTimelineItemState::Pending);
        m_timeline->addItem(launch);
        m_timelineItems.append(launch);

        // Post-launch Support
        auto* support = Components::FluentTimelineItem::createNote(
            "Post-Launch Support",
            "Monitor performance, gather feedback, and provide ongoing "
            "support");
        support->setDateTime(QDateTime::fromString("2024-04-15", "yyyy-MM-dd"));
        support->setItemState(Components::FluentTimelineItemState::Pending);
        m_timeline->addItem(support);
        m_timelineItems.append(support);

        // Set first item as current
        m_timeline->setCurrentIndex(0);
    }

private:
    Components::FluentTimeline* m_timeline;
    Components::FluentButton* m_progressButton;
    QLabel* m_statusLabel;
    QList<Components::FluentTimelineItem*> m_timelineItems;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Initialize FluentQt theme
    auto& theme = Styling::FluentTheme::instance();
    theme.setAccentColor(Styling::FluentAccentColor::Blue);

    SimpleTimelineWindow window;
    window.show();

    return app.exec();
}

#include "SimpleTimelineExample.moc"
