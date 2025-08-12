// examples/NotificationExample.cpp
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentNotification.h"
#include "FluentQt/Styling/FluentTheme.h"

using namespace FluentQt;

class NotificationExample : public QMainWindow {
    Q_OBJECT

public:
    NotificationExample(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupConnections();
        setupTheme();
    }

private slots:
    void showInfoNotification();
    void showSuccessNotification();
    void showWarningNotification();
    void showErrorNotification();
    void showCustomNotification();
    void showNotificationWithActions();
    void showPersistentNotification();
    void clearAllNotifications();
    void updateManagerSettings();
    void toggleTheme();

private:
    void setupUI();
    void setupConnections();
    void setupTheme();
    void createBasicNotificationControls();
    void createAdvancedNotificationControls();
    void createManagerControls();
    void createExampleNotifications();

private:
    // UI Components
    QWidget* m_centralWidget{nullptr};
    QVBoxLayout* m_mainLayout{nullptr};

    // Basic notification controls
    QGroupBox* m_basicGroup{nullptr};
    QLineEdit* m_titleEdit{nullptr};
    QTextEdit* m_messageEdit{nullptr};
    QSpinBox* m_durationSpin{nullptr};
    QCheckBox* m_closableCheck{nullptr};
    QCheckBox* m_persistentCheck{nullptr};

    // Advanced controls
    QGroupBox* m_advancedGroup{nullptr};
    QPushButton* m_actionsButton{nullptr};
    QPushButton* m_persistentButton{nullptr};

    // Manager controls
    QGroupBox* m_managerGroup{nullptr};
    QComboBox* m_positionCombo{nullptr};
    QComboBox* m_animationCombo{nullptr};
    QSpinBox* m_maxVisibleSpin{nullptr};
    QSpinBox* m_spacingSpin{nullptr};

    // Example notifications
    QGroupBox* m_examplesGroup{nullptr};

    // Theme controls
    QPushButton* m_themeButton{nullptr};

    // Manager reference
    Components::FluentNotificationManager* m_manager{nullptr};
};

void NotificationExample::setupUI() {
    setWindowTitle("FluentQt Notification Example");
    setMinimumSize(800, 600);

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setSpacing(16);
    m_mainLayout->setContentsMargins(16, 16, 16, 16);

    createBasicNotificationControls();
    createAdvancedNotificationControls();
    createManagerControls();
    createExampleNotifications();

    // Theme toggle button
    m_themeButton = new QPushButton("Toggle Dark/Light Theme", this);
    m_mainLayout->addWidget(m_themeButton);

    m_mainLayout->addStretch();

    // Get manager instance
    m_manager = &Components::FluentNotificationManager::instance();
}

void NotificationExample::createBasicNotificationControls() {
    m_basicGroup = new QGroupBox("Basic Notifications", this);
    auto* layout = new QGridLayout(m_basicGroup);

    // Title input
    layout->addWidget(new QLabel("Title:"), 0, 0);
    m_titleEdit = new QLineEdit("Sample Notification", this);
    layout->addWidget(m_titleEdit, 0, 1, 1, 2);

    // Message input
    layout->addWidget(new QLabel("Message:"), 1, 0);
    m_messageEdit = new QTextEdit(
        "This is a sample notification message that demonstrates the FluentQt "
        "notification system.",
        this);
    m_messageEdit->setMaximumHeight(80);
    layout->addWidget(m_messageEdit, 1, 1, 1, 2);

    // Duration
    layout->addWidget(new QLabel("Duration (ms):"), 2, 0);
    m_durationSpin = new QSpinBox(this);
    m_durationSpin->setRange(0, 30000);
    m_durationSpin->setValue(5000);
    m_durationSpin->setSpecialValueText("Persistent");
    layout->addWidget(m_durationSpin, 2, 1);

    // Options
    m_closableCheck = new QCheckBox("Closable", this);
    m_closableCheck->setChecked(true);
    layout->addWidget(m_closableCheck, 2, 2);

    m_persistentCheck = new QCheckBox("Persistent", this);
    layout->addWidget(m_persistentCheck, 3, 1);

    // Notification type buttons
    auto* buttonLayout = new QHBoxLayout();

    auto* infoBtn = new Components::FluentButton("Info", this);
    auto* successBtn = new Components::FluentButton("Success", this);
    auto* warningBtn = new Components::FluentButton("Warning", this);
    auto* errorBtn = new Components::FluentButton("Error", this);
    auto* customBtn = new Components::FluentButton("Custom", this);

    successBtn->setStyle(Components::FluentButtonStyle::Accent);
    warningBtn->setStyle(Components::FluentButtonStyle::Standard);
    errorBtn->setStyle(Components::FluentButtonStyle::Standard);

    buttonLayout->addWidget(infoBtn);
    buttonLayout->addWidget(successBtn);
    buttonLayout->addWidget(warningBtn);
    buttonLayout->addWidget(errorBtn);
    buttonLayout->addWidget(customBtn);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout, 4, 0, 1, 3);

    m_mainLayout->addWidget(m_basicGroup);

    // Connect buttons
    connect(infoBtn, &QPushButton::clicked, this,
            &NotificationExample::showInfoNotification);
    connect(successBtn, &QPushButton::clicked, this,
            &NotificationExample::showSuccessNotification);
    connect(warningBtn, &QPushButton::clicked, this,
            &NotificationExample::showWarningNotification);
    connect(errorBtn, &QPushButton::clicked, this,
            &NotificationExample::showErrorNotification);
    connect(customBtn, &QPushButton::clicked, this,
            &NotificationExample::showCustomNotification);
}

void NotificationExample::createAdvancedNotificationControls() {
    m_advancedGroup = new QGroupBox("Advanced Notifications", this);
    auto* layout = new QHBoxLayout(m_advancedGroup);

    m_actionsButton = new Components::FluentButton("With Actions", this);
    m_persistentButton = new Components::FluentButton("Persistent", this);

    layout->addWidget(m_actionsButton);
    layout->addWidget(m_persistentButton);
    layout->addStretch();

    m_mainLayout->addWidget(m_advancedGroup);
}

void NotificationExample::createManagerControls() {
    m_managerGroup = new QGroupBox("Notification Manager Settings", this);
    auto* layout = new QGridLayout(m_managerGroup);

    // Position
    layout->addWidget(new QLabel("Position:"), 0, 0);
    m_positionCombo = new QComboBox(this);
    m_positionCombo->addItems({"Top Right", "Top Center", "Top Left",
                               "Bottom Right", "Bottom Center", "Bottom Left",
                               "Center"});
    layout->addWidget(m_positionCombo, 0, 1);

    // Animation
    layout->addWidget(new QLabel("Animation:"), 0, 2);
    m_animationCombo = new QComboBox(this);
    m_animationCombo->addItems({"Slide", "Fade", "Scale", "Bounce"});
    layout->addWidget(m_animationCombo, 0, 3);

    // Max visible
    layout->addWidget(new QLabel("Max Visible:"), 1, 0);
    m_maxVisibleSpin = new QSpinBox(this);
    m_maxVisibleSpin->setRange(1, 10);
    m_maxVisibleSpin->setValue(5);
    layout->addWidget(m_maxVisibleSpin, 1, 1);

    // Spacing
    layout->addWidget(new QLabel("Spacing:"), 1, 2);
    m_spacingSpin = new QSpinBox(this);
    m_spacingSpin->setRange(0, 50);
    m_spacingSpin->setValue(8);
    layout->addWidget(m_spacingSpin, 1, 3);

    // Clear all button
    auto* clearBtn = new Components::FluentButton("Clear All", this);
    clearBtn->setStyle(Components::FluentButtonStyle::Standard);
    layout->addWidget(clearBtn, 2, 0, 1, 4);

    m_mainLayout->addWidget(m_managerGroup);

    connect(clearBtn, &QPushButton::clicked, this,
            &NotificationExample::clearAllNotifications);
}

void NotificationExample::createExampleNotifications() {
    m_examplesGroup = new QGroupBox("Example Scenarios", this);
    auto* layout = new QHBoxLayout(m_examplesGroup);

    auto* downloadBtn = new Components::FluentButton("Download Complete", this);
    auto* updateBtn = new Components::FluentButton("Update Available", this);
    auto* errorBtn = new Components::FluentButton("Connection Error", this);
    auto* batchBtn = new Components::FluentButton("Show Batch", this);

    layout->addWidget(downloadBtn);
    layout->addWidget(updateBtn);
    layout->addWidget(errorBtn);
    layout->addWidget(batchBtn);
    layout->addStretch();

    m_mainLayout->addWidget(m_examplesGroup);

    // Connect example buttons
    connect(downloadBtn, &QPushButton::clicked, [this]() {
        auto* notification =
            m_manager->showSuccess("Download Complete",
                                   "Your file has been downloaded successfully "
                                   "to the Downloads folder.");
        notification->addAction(
            "Open Folder", []() { qDebug() << "Opening downloads folder..."; });
        notification->addAction(
            "View File", []() { qDebug() << "Opening downloaded file..."; });
    });

    connect(updateBtn, &QPushButton::clicked, [this]() {
        auto* notification =
            m_manager->showInfo("Update Available",
                                "A new version of the application is "
                                "available. Would you like to update now?");
        notification->addAction("Update Now",
                                []() { qDebug() << "Starting update..."; });
        notification->addAction("Later",
                                []() { qDebug() << "Update postponed"; });
        notification->setPersistent(true);
    });

    connect(errorBtn, &QPushButton::clicked, [this]() {
        auto* notification = m_manager->showError(
            "Connection Error",
            "Failed to connect to the server. Please check your internet "
            "connection and try again.");
        notification->addAction("Retry",
                                []() { qDebug() << "Retrying connection..."; });
        notification->addAction(
            "Settings", []() { qDebug() << "Opening network settings..."; });
    });

    connect(batchBtn, &QPushButton::clicked, [this]() {
        // Show multiple notifications to demonstrate queuing
        m_manager->showInfo("Processing", "Starting batch operation...");

        QTimer::singleShot(500, [this]() {
            m_manager->showInfo("Progress", "Processing item 1 of 5...");
        });

        QTimer::singleShot(1000, [this]() {
            m_manager->showInfo("Progress", "Processing item 3 of 5...");
        });

        QTimer::singleShot(1500, [this]() {
            m_manager->showWarning("Warning",
                                   "Item 4 skipped due to validation error.");
        });

        QTimer::singleShot(2000, [this]() {
            m_manager->showSuccess("Complete",
                                   "Batch operation completed successfully!");
        });
    });
}

void NotificationExample::setupConnections() {
    connect(m_themeButton, &QPushButton::clicked, this,
            &NotificationExample::toggleTheme);
    connect(m_actionsButton, &QPushButton::clicked, this,
            &NotificationExample::showNotificationWithActions);
    connect(m_persistentButton, &QPushButton::clicked, this,
            &NotificationExample::showPersistentNotification);

    // Manager settings
    connect(m_positionCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &NotificationExample::updateManagerSettings);
    connect(m_animationCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &NotificationExample::updateManagerSettings);
    connect(m_maxVisibleSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &NotificationExample::updateManagerSettings);
    connect(m_spacingSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &NotificationExample::updateManagerSettings);
}

void NotificationExample::setupTheme() {
    auto& theme = Styling::FluentTheme::instance();
    theme.setMode(Styling::FluentThemeMode::System);
}

// Notification methods
void NotificationExample::showInfoNotification() {
    auto* notification =
        m_manager->showInfo(m_titleEdit->text(), m_messageEdit->toPlainText(),
                            m_durationSpin->value());
    notification->setClosable(m_closableCheck->isChecked());
    notification->setPersistent(m_persistentCheck->isChecked());
}

void NotificationExample::showSuccessNotification() {
    auto* notification = m_manager->showSuccess(m_titleEdit->text(),
                                                m_messageEdit->toPlainText(),
                                                m_durationSpin->value());
    notification->setClosable(m_closableCheck->isChecked());
    notification->setPersistent(m_persistentCheck->isChecked());
}

void NotificationExample::showWarningNotification() {
    auto* notification = m_manager->showWarning(m_titleEdit->text(),
                                                m_messageEdit->toPlainText(),
                                                m_durationSpin->value());
    notification->setClosable(m_closableCheck->isChecked());
    notification->setPersistent(m_persistentCheck->isChecked());
}

void NotificationExample::showErrorNotification() {
    auto* notification =
        m_manager->showError(m_titleEdit->text(), m_messageEdit->toPlainText(),
                             m_durationSpin->value());
    notification->setClosable(m_closableCheck->isChecked());
    notification->setPersistent(m_persistentCheck->isChecked());
}

void NotificationExample::showCustomNotification() {
    auto* notification = new Components::FluentNotification(
        Components::FluentNotificationType::Custom, m_titleEdit->text(),
        m_messageEdit->toPlainText());
    notification->setDuration(m_durationSpin->value());
    notification->setClosable(m_closableCheck->isChecked());
    notification->setPersistent(m_persistentCheck->isChecked());
    notification->setIcon(QIcon(":/icons/custom-icon.svg"));
    notification->showAnimated();
}

void NotificationExample::showNotificationWithActions() {
    auto* notification = m_manager->showInfo(
        "Action Required",
        "Please choose an action to continue with the operation.");

    notification->addAction("Accept",
                            []() { qDebug() << "User accepted the action"; });

    notification->addAction("Decline",
                            []() { qDebug() << "User declined the action"; });

    notification->addAction(
        "More Info", []() { qDebug() << "User requested more information"; });

    notification->setPersistent(true);
}

void NotificationExample::showPersistentNotification() {
    auto* notification = m_manager->showWarning(
        "Important Notice",
        "This notification will remain visible until you manually close it.");
    notification->setPersistent(true);
    notification->setDuration(0);  // No auto-hide
}

void NotificationExample::clearAllNotifications() { m_manager->clear(); }

void NotificationExample::updateManagerSettings() {
    // Update position
    Components::FluentNotificationPosition position =
        static_cast<Components::FluentNotificationPosition>(
            m_positionCombo->currentIndex());
    m_manager->setPosition(position);

    // Update animation
    Components::FluentNotificationAnimation animation =
        static_cast<Components::FluentNotificationAnimation>(
            m_animationCombo->currentIndex());
    m_manager->setAnimation(animation);

    // Update max visible
    m_manager->setMaxVisible(m_maxVisibleSpin->value());

    // Update spacing
    m_manager->setSpacing(m_spacingSpin->value());
}

void NotificationExample::toggleTheme() {
    auto& theme = Styling::FluentTheme::instance();
    auto newMode = (theme.mode() == Styling::FluentThemeMode::Light)
                       ? Styling::FluentThemeMode::Dark
                       : Styling::FluentThemeMode::Light;
    theme.setMode(newMode);
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    NotificationExample example;
    example.show();

    return app.exec();
}

#include "NotificationExample.moc"
