# FluentToast Component

The `FluentToast` component provides elegant, non-intrusive notifications that appear temporarily to inform users about actions, status changes, or important information. It implements Microsoft's Fluent Design principles with smooth animations, flexible positioning, and comprehensive customization options.

## Overview

FluentToast offers:
- **Multiple toast types** (Info, Success, Warning, Error, Custom)
- **Flexible positioning** (Top-left, Top-center, Top-right, Bottom-left, Bottom-center, Bottom-right, Center)
- **Auto-dismiss functionality** with configurable duration
- **Action buttons** for user interaction
- **Rich content support** (title, message, icons, progress indicators)
- **Smooth animations** with stacking behavior
- **Pause on hover** functionality
- **Accessibility support** with screen reader announcements

## Basic Usage

### Simple Toast Notifications

```cpp
#include "FluentQt/Components/FluentToast.h"
#include "FluentQt/Components/FluentToastManager.h"

// Using the global toast manager (recommended)
FluentToastGlobal::showSuccess("Success", "Operation completed successfully!");
FluentToastGlobal::showError("Error", "Something went wrong.");
FluentToastGlobal::showWarning("Warning", "Please check your input.");
FluentToastGlobal::showInfo("Info", "New update available.");

// Using custom icons
QIcon customIcon(":/icons/custom.png");
FluentToastGlobal::showCustom(customIcon, "Custom", "Custom notification message.");
```

### Creating Toast Instances

```cpp
// Create individual toast instances
auto* toast = FluentToast::createSuccess("Task Complete", "Your file has been saved successfully.");
toast->show();

// Create with custom configuration
FluentToastConfig config;
config.type = FluentToastType::Info;
config.position = FluentToastPosition::TopCenter;
config.duration = 3000; // 3 seconds
config.closable = true;
config.showProgress = true;

auto* customToast = new FluentToast(config);
customToast->setTitle("Custom Toast");
customToast->setMessage("This is a customized toast notification.");
customToast->show();
```

## Toast Types and Styling

### Built-in Toast Types

```cpp
// Success toast (green theme)
auto* successToast = FluentToast::createSuccess(
    "Upload Complete", 
    "Your file has been uploaded successfully."
);

// Error toast (red theme)
auto* errorToast = FluentToast::createError(
    "Connection Failed", 
    "Unable to connect to the server. Please try again."
);

// Warning toast (orange theme)
auto* warningToast = FluentToast::createWarning(
    "Storage Almost Full", 
    "You have less than 10% storage space remaining."
);

// Info toast (blue theme)
auto* infoToast = FluentToast::createInfo(
    "New Feature Available", 
    "Check out the new timeline component in the latest update."
);

// Custom toast with custom styling
FluentToastConfig customConfig;
customConfig.type = FluentToastType::Custom;
customConfig.customBackgroundColor = QColor("#6A4C93");
customConfig.customTextColor = QColor("#FFFFFF");
customConfig.customIcon = QIcon(":/icons/star.png");

auto* customToast = new FluentToast(customConfig);
customToast->setTitle("Premium Feature");
customToast->setMessage("You've unlocked a premium feature!");
```

## Positioning and Layout

### Toast Positioning

```cpp
// Using FluentToastManager for positioned toasts
auto& manager = FluentToastManager::instance();

// Top positions
manager.showInfoAt(FluentToastPosition::TopLeft, "Top Left", "Message");
manager.showInfoAt(FluentToastPosition::TopCenter, "Top Center", "Message");
manager.showInfoAt(FluentToastPosition::TopRight, "Top Right", "Message");

// Bottom positions
manager.showInfoAt(FluentToastPosition::BottomLeft, "Bottom Left", "Message");
manager.showInfoAt(FluentToastPosition::BottomCenter, "Bottom Center", "Message");
manager.showInfoAt(FluentToastPosition::BottomRight, "Bottom Right", "Message");

// Center position
manager.showInfoAt(FluentToastPosition::Center, "Center", "Important message");
```

### Toast Manager Configuration

```cpp
// Configure the global toast manager
FluentToastManagerConfig managerConfig;
managerConfig.defaultPosition = FluentToastPosition::TopRight;
managerConfig.maxVisible = 5;           // Maximum visible toasts
managerConfig.maxQueued = 20;           // Maximum queued toasts
managerConfig.stackSpacing = 8;         // Spacing between stacked toasts
managerConfig.screenMargin = 16;        // Margin from screen edges
managerConfig.allowDuplicates = false;  // Prevent duplicate toasts
managerConfig.stackToasts = true;       // Stack toasts or replace

auto& manager = FluentToastManager::instance();
manager.setConfiguration(managerConfig);

// Set parent widget for positioning (optional)
manager.setParentWidget(mainWindow);
```

## Interactive Features

### Action Buttons

```cpp
auto* toast = FluentToast::createInfo("File Download", "Download completed successfully.");

// Add action buttons
toast->addAction("Open", [=]() {
    // Open the downloaded file
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
});

toast->addAction("Show in Folder", [=]() {
    // Show file in folder
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(filePath).dir().path()));
}, false); // false = not primary button

toast->addAction(QIcon(":/icons/share.png"), "Share", [=]() {
    // Share the file
    shareFile(filePath);
}, true); // true = primary button

toast->show();
```

### Progress Indicators

```cpp
auto* progressToast = FluentToast::createInfo("Uploading File", "Please wait...");
progressToast->setShowProgress(true);
progressToast->setProgressRange(0, 100);
progressToast->setPersistent(true); // Don't auto-dismiss

// Update progress
QTimer* progressTimer = new QTimer();
int progress = 0;
connect(progressTimer, &QTimer::timeout, [=]() mutable {
    progress += 10;
    progressToast->setProgress(progress);
    
    if (progress >= 100) {
        progressTimer->stop();
        progressToast->setTitle("Upload Complete");
        progressToast->setMessage("File uploaded successfully!");
        progressToast->setPersistent(false);
        progressToast->setDuration(3000);
        progressTimer->deleteLater();
    }
});
progressTimer->start(200);

progressToast->show();
```

### Pause on Hover

```cpp
auto* toast = FluentToast::createWarning("System Update", "A system update is available.");
toast->setPauseOnHover(true); // Pause auto-dismiss timer on hover
toast->setDuration(5000);

// Handle timer events
connect(toast, &FluentToast::timerPaused, [=]() {
    qDebug() << "Toast timer paused";
});

connect(toast, &FluentToast::timerResumed, [=]() {
    qDebug() << "Toast timer resumed";
});

toast->show();
```

## Advanced Configuration

### Custom Animations

```cpp
FluentToastConfig config;
config.animation = FluentToastAnimation::Slide;  // Slide in/out
config.animationDuration = 300;
config.easingCurve = QEasingCurve::OutCubic;

auto* toast = new FluentToast(config);
toast->setTitle("Animated Toast");
toast->setMessage("This toast slides in smoothly.");
toast->show();
```

### Persistent Toasts

```cpp
// Create a persistent toast that requires manual dismissal
auto* persistentToast = FluentToast::createError(
    "Critical Error", 
    "A critical error occurred. Please contact support."
);
persistentToast->setPersistent(true);  // Won't auto-dismiss
persistentToast->setClosable(true);    // Show close button

// Add action to handle the error
persistentToast->addAction("Contact Support", [=]() {
    QDesktopServices::openUrl(QUrl("mailto:support@example.com"));
});

persistentToast->show();
```

### Custom Content and Styling

```cpp
FluentToastConfig config;
config.type = FluentToastType::Custom;
config.autoCalculateColors = false;
config.customBackgroundColor = QColor("#2D3748");
config.customTextColor = QColor("#E2E8F0");
config.customBorderColor = QColor("#4A5568");
config.maxWidth = 500;

auto* customToast = new FluentToast(config);
customToast->setTitle("Custom Styled Toast");
customToast->setMessage("This toast has custom colors and styling.");

// Set custom icon
QIcon customIcon = QIcon(":/icons/notification.png");
customToast->setIcon(customIcon);

customToast->show();
```

## Event Handling

### Toast Events

```cpp
auto* toast = FluentToast::createSuccess("Operation Complete", "Task finished successfully.");

// Handle toast events
connect(toast, &FluentToast::aboutToShow, [=]() {
    qDebug() << "Toast about to show";
});

connect(toast, &FluentToast::shown, [=]() {
    qDebug() << "Toast shown";
});

connect(toast, &FluentToast::aboutToHide, [=]() {
    qDebug() << "Toast about to hide";
});

connect(toast, &FluentToast::hidden, [=]() {
    qDebug() << "Toast hidden";
});

connect(toast, &FluentToast::dismissed, [=]() {
    qDebug() << "Toast dismissed by user";
});

connect(toast, &FluentToast::clicked, [=]() {
    qDebug() << "Toast clicked";
    // Handle toast click
});

connect(toast, &FluentToast::actionTriggered, [=](const QString& actionText) {
    qDebug() << "Action triggered:" << actionText;
});

toast->show();
```

### Manager Events

```cpp
auto& manager = FluentToastManager::instance();

// Handle manager events
connect(&manager, &FluentToastManager::toastShown, [=](FluentToast* toast) {
    qDebug() << "Toast shown:" << toast->title();
});

connect(&manager, &FluentToastManager::toastHidden, [=](FluentToast* toast) {
    qDebug() << "Toast hidden:" << toast->title();
});

connect(&manager, &FluentToastManager::allToastsHidden, [=]() {
    qDebug() << "All toasts hidden";
});
```

## Queue Management

### Controlling Toast Queue

```cpp
auto& manager = FluentToastManager::instance();

// Show multiple toasts (they will be queued if max visible is exceeded)
for (int i = 0; i < 10; ++i) {
    manager.showInfo(QString("Toast %1").arg(i), "This is a queued toast.");
}

// Pause queue processing
manager.pauseQueue();

// Resume queue processing
manager.resumeQueue();

// Clear all queued toasts
manager.clearQueue();

// Hide all visible toasts
manager.hideAll();

// Get queue information
int visibleCount = manager.visibleCount();
int queuedCount = manager.queuedCount();
qDebug() << "Visible:" << visibleCount << "Queued:" << queuedCount;
```

## Accessibility

### Screen Reader Support

```cpp
auto* toast = FluentToast::createInfo("Accessibility Example", "This toast is accessible.");

// Accessibility is automatically configured, but can be customized
toast->setAccessibleName("Information notification: Accessibility Example");
toast->setAccessibleDescription("Toast notification with accessibility support");

// The toast will automatically announce to screen readers when shown
toast->show();
```

## Integration Examples

### With File Operations

```cpp
class FileManager : public QObject {
public:
    void saveFile(const QString& fileName, const QByteArray& data) {
        // Show progress toast
        auto* progressToast = FluentToast::createInfo("Saving File", "Saving " + fileName + "...");
        progressToast->setShowProgress(true);
        progressToast->setPersistent(true);
        progressToast->show();
        
        // Simulate file saving with progress updates
        auto* timer = new QTimer(this);
        int progress = 0;
        connect(timer, &QTimer::timeout, [=]() mutable {
            progress += 20;
            progressToast->setProgress(progress);
            
            if (progress >= 100) {
                timer->stop();
                progressToast->hide();
                
                // Show completion toast
                auto* successToast = FluentToast::createSuccess("File Saved", fileName + " saved successfully.");
                successToast->addAction("Open", [=]() {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
                });
                successToast->show();
                
                timer->deleteLater();
            }
        });
        timer->start(500);
    }
};
```

### With Network Operations

```cpp
class NetworkManager : public QObject {
public:
    void downloadFile(const QUrl& url) {
        auto* downloadToast = FluentToast::createInfo("Download Started", "Downloading file...");
        downloadToast->setShowProgress(true);
        downloadToast->setPersistent(true);
        
        downloadToast->addAction("Cancel", [=]() {
            // Cancel download
            cancelDownload();
            downloadToast->hide();
        });
        
        downloadToast->show();
        
        // Connect to download progress
        connect(this, &NetworkManager::downloadProgress, [=](int percent) {
            downloadToast->setProgress(percent);
        });
        
        connect(this, &NetworkManager::downloadFinished, [=](bool success) {
            downloadToast->hide();
            
            if (success) {
                FluentToastGlobal::showSuccess("Download Complete", "File downloaded successfully.");
            } else {
                auto* errorToast = FluentToast::createError("Download Failed", "Failed to download file.");
                errorToast->addAction("Retry", [=]() {
                    downloadFile(url);
                });
                errorToast->show();
            }
        });
        
        startDownload(url);
    }
    
signals:
    void downloadProgress(int percent);
    void downloadFinished(bool success);
    
private:
    void startDownload(const QUrl& url) { /* Implementation */ }
    void cancelDownload() { /* Implementation */ }
};
```

## Best Practices

1. **Use appropriate toast types** for different message categories (Success for confirmations, Error for failures, etc.)

2. **Keep messages concise** - toasts should be quickly readable

3. **Provide actions when appropriate** - allow users to act on the notification

4. **Use progress indicators** for long-running operations

5. **Don't overuse toasts** - too many notifications can be overwhelming

6. **Consider persistence** for critical messages that require user attention

7. **Test with screen readers** to ensure accessibility

8. **Use consistent positioning** throughout your application

9. **Handle queue overflow** gracefully with appropriate limits

10. **Provide clear action labels** that describe what will happen

## API Reference

For complete API documentation, see the header files:
- `FluentQt/Components/FluentToast.h`
- `FluentQt/Components/FluentToastManager.h`

## Standalone Examples Collection

### Example 1: Smart Notification System

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTimer>
#include <QRandomGenerator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "FluentQt/Components/FluentToast.h"
#include "FluentQt/Components/FluentToastManager.h"
#include "FluentQt/Components/FluentButton.h"
#include "FluentQt/Components/FluentCard.h"
#include "FluentQt/Components/FluentTextInput.h"

class SmartNotificationDemo : public QMainWindow {
    Q_OBJECT
public:
    SmartNotificationDemo(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupNotificationSystem();
        connectSignals();
    }

private slots:
    void showBasicNotifications() {
        // Demonstrate different toast types
        FluentToastGlobal::showInfo("Information", "This is an informational message.");

        QTimer::singleShot(1000, []() {
            FluentToastGlobal::showSuccess("Success", "Operation completed successfully!");
        });

        QTimer::singleShot(2000, []() {
            FluentToastGlobal::showWarning("Warning", "Please review your settings.");
        });

        QTimer::singleShot(3000, []() {
            FluentToastGlobal::showError("Error", "Failed to connect to server.");
        });
    }

    void showInteractiveNotifications() {
        // Toast with action buttons
        auto* toast = FluentToast::createInfo("New Update Available",
                                             "Version 2.1.0 is ready to install.");

        toast->addAction("Install Now", [this]() {
            showInstallProgress();
        });

        toast->addAction("Later", []() {
            FluentToastGlobal::showInfo("Reminder", "Update postponed. We'll remind you later.");
        });

        toast->setDuration(0); // Don't auto-dismiss
        toast->show();
    }

    void showInstallProgress() {
        auto* progressToast = FluentToast::createCustom(
            QIcon(":/icons/download.png"),
            "Installing Update",
            "Downloading and installing version 2.1.0..."
        );

        progressToast->setShowProgress(true);
        progressToast->setDuration(0); // Manual dismiss
        progressToast->setClosable(false); // Can't close during install

        // Simulate installation progress
        auto* timer = new QTimer(this);
        int progress = 0;

        connect(timer, &QTimer::timeout, [=]() mutable {
            progress += QRandomGenerator::global()->bounded(5, 15);
            progressToast->setProgress(progress);

            if (progress >= 100) {
                timer->stop();
                timer->deleteLater();
                progressToast->close();

                // Show completion notification
                auto* completeToast = FluentToast::createSuccess(
                    "Update Complete",
                    "Version 2.1.0 has been installed successfully."
                );

                completeToast->addAction("Restart Now", []() {
                    FluentToastGlobal::showInfo("Restarting", "Application will restart in 3 seconds...");
                });

                completeToast->addAction("Restart Later", []() {
                    FluentToastGlobal::showWarning("Restart Required",
                                                  "Please restart the application to complete the update.");
                });

                completeToast->show();
            }
        });

        timer->start(200);
        progressToast->show();
    }

    void showSmartNotifications() {
        // Context-aware notifications
        simulateFileOperation();

        QTimer::singleShot(2000, [this]() {
            simulateNetworkEvent();
        });

        QTimer::singleShot(4000, [this]() {
            simulateSystemEvent();
        });
    }

    void simulateFileOperation() {
        auto* toast = FluentToast::createInfo("File Operation", "Copying 150 files...");
        toast->setShowProgress(true);
        toast->setDuration(0);

        // Simulate file copying
        auto* timer = new QTimer(this);
        int filesProcessed = 0;
        const int totalFiles = 150;

        connect(timer, &QTimer::timeout, [=]() mutable {
            filesProcessed += QRandomGenerator::global()->bounded(1, 8);
            int progress = (filesProcessed * 100) / totalFiles;

            toast->setProgress(progress);
            toast->setMessage(QString("Copying %1 of %2 files...").arg(filesProcessed).arg(totalFiles));

            if (filesProcessed >= totalFiles) {
                timer->stop();
                timer->deleteLater();
                toast->close();

                // Show completion with action
                auto* completeToast = FluentToast::createSuccess(
                    "Copy Complete",
                    QString("%1 files copied successfully.").arg(totalFiles)
                );

                completeToast->addAction("Open Folder", []() {
                    FluentToastGlobal::showInfo("Opening", "File explorer opened.");
                });

                completeToast->show();
            }
        });

        timer->start(100);
        toast->show();
    }

    void simulateNetworkEvent() {
        // Network connectivity notification
        auto* toast = FluentToast::createWarning("Network Issue", "Connection unstable. Retrying...");
        toast->setDuration(0);

        // Simulate reconnection attempts
        auto* timer = new QTimer(this);
        int attempts = 0;

        connect(timer, &QTimer::timeout, [=]() mutable {
            attempts++;

            if (attempts < 3) {
                toast->setMessage(QString("Connection unstable. Retry attempt %1/3...").arg(attempts));
            } else {
                timer->stop();
                timer->deleteLater();
                toast->close();

                // Show success
                auto* successToast = FluentToast::createSuccess(
                    "Connected",
                    "Network connection restored."
                );
                successToast->show();
            }
        });

        timer->start(1500);
        toast->show();
    }

    void simulateSystemEvent() {
        // System maintenance notification
        auto* toast = FluentToast::createCustom(
            QIcon(":/icons/maintenance.png"),
            "System Maintenance",
            "Scheduled maintenance will begin in 10 minutes."
        );

        toast->addAction("Postpone", []() {
            FluentToastGlobal::showInfo("Postponed", "Maintenance postponed for 1 hour.");
        });

        toast->addAction("Start Now", []() {
            auto* maintenanceToast = FluentToast::createInfo(
                "Maintenance Mode",
                "System is now in maintenance mode. Some features may be unavailable."
            );
            maintenanceToast->setDuration(0);
            maintenanceToast->show();
        });

        toast->setDuration(10000); // 10 seconds
        toast->show();
    }

    void showCustomToast() {
        QString title = m_titleInput->text();
        QString message = m_messageInput->text();

        if (title.isEmpty()) title = "Custom Notification";
        if (message.isEmpty()) message = "This is a custom notification message.";

        FluentToastType type = static_cast<FluentToastType>(m_typeCombo->currentIndex());
        FluentToastPosition position = static_cast<FluentToastPosition>(m_positionCombo->currentIndex());

        FluentToastConfig config;
        config.type = type;
        config.position = position;
        config.duration = m_durationSpin->value() * 1000;
        config.closable = m_closableCheck->isChecked();
        config.showProgress = m_progressCheck->isChecked();

        auto* toast = new FluentToast(title, message, config);

        if (m_actionCheck->isChecked()) {
            toast->addAction("Action", []() {
                FluentToastGlobal::showInfo("Action", "Custom action triggered!");
            });
        }

        toast->show();
    }

private:
    void setupUI() {
        auto* centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        auto* layout = new QVBoxLayout(centralWidget);

        // Title
        auto* titleLabel = new QLabel("Smart Notification System Demo");
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 20px;");

        // Demo buttons
        auto* demoCard = new FluentCard("Notification Demos");
        auto* demoLayout = new QGridLayout;

        auto* basicButton = new FluentButton("Basic Notifications");
        basicButton->setButtonStyle(FluentButtonStyle::Primary);
        connect(basicButton, &FluentButton::clicked, this, &SmartNotificationDemo::showBasicNotifications);

        auto* interactiveButton = new FluentButton("Interactive Notifications");
        interactiveButton->setButtonStyle(FluentButtonStyle::Accent);
        connect(interactiveButton, &FluentButton::clicked, this, &SmartNotificationDemo::showInteractiveNotifications);

        auto* smartButton = new FluentButton("Smart Notifications");
        smartButton->setButtonStyle(FluentButtonStyle::Default);
        connect(smartButton, &FluentButton::clicked, this, &SmartNotificationDemo::showSmartNotifications);

        demoLayout->addWidget(basicButton, 0, 0);
        demoLayout->addWidget(interactiveButton, 0, 1);
        demoLayout->addWidget(smartButton, 0, 2);

        demoCard->setContentWidget(new QWidget);
        demoCard->contentWidget()->setLayout(demoLayout);

        // Custom toast creator
        auto* customCard = new FluentCard("Create Custom Toast");
        auto* customLayout = new QGridLayout;

        // Title input
        customLayout->addWidget(new QLabel("Title:"), 0, 0);
        m_titleInput = new FluentTextInput();
        m_titleInput->setPlaceholderText("Enter toast title");
        customLayout->addWidget(m_titleInput, 0, 1, 1, 2);

        // Message input
        customLayout->addWidget(new QLabel("Message:"), 1, 0);
        m_messageInput = new FluentTextInput();
        m_messageInput->setPlaceholderText("Enter toast message");
        customLayout->addWidget(m_messageInput, 1, 1, 1, 2);

        // Type selection
        customLayout->addWidget(new QLabel("Type:"), 2, 0);
        m_typeCombo = new QComboBox;
        m_typeCombo->addItems({"Info", "Success", "Warning", "Error", "Custom"});
        customLayout->addWidget(m_typeCombo, 2, 1);

        // Position selection
        customLayout->addWidget(new QLabel("Position:"), 2, 2);
        m_positionCombo = new QComboBox;
        m_positionCombo->addItems({"Top Left", "Top Center", "Top Right",
                                  "Bottom Left", "Bottom Center", "Bottom Right", "Center"});
        m_positionCombo->setCurrentIndex(1); // Top Center
        customLayout->addWidget(m_positionCombo, 2, 3);

        // Duration
        customLayout->addWidget(new QLabel("Duration (s):"), 3, 0);
        m_durationSpin = new QSpinBox;
        m_durationSpin->setRange(0, 60);
        m_durationSpin->setValue(5);
        m_durationSpin->setSpecialValueText("No auto-dismiss");
        customLayout->addWidget(m_durationSpin, 3, 1);

        // Options
        m_closableCheck = new QCheckBox("Closable");
        m_closableCheck->setChecked(true);
        customLayout->addWidget(m_closableCheck, 3, 2);

        m_progressCheck = new QCheckBox("Show Progress");
        customLayout->addWidget(m_progressCheck, 3, 3);

        m_actionCheck = new QCheckBox("Add Action Button");
        customLayout->addWidget(m_actionCheck, 4, 0);

        // Create button
        auto* createButton = new FluentButton("Create Toast");
        createButton->setButtonStyle(FluentButtonStyle::Primary);
        connect(createButton, &FluentButton::clicked, this, &SmartNotificationDemo::showCustomToast);
        customLayout->addWidget(createButton, 4, 2, 1, 2);

        customCard->setContentWidget(new QWidget);
        customCard->contentWidget()->setLayout(customLayout);

        layout->addWidget(titleLabel);
        layout->addWidget(demoCard);
        layout->addWidget(customCard);
        layout->addStretch();
    }

    void setupNotificationSystem() {
        // Configure global toast manager
        FluentToastManager::instance().setMaxToasts(5);
        FluentToastManager::instance().setDefaultPosition(FluentToastPosition::TopRight);
        FluentToastManager::instance().setDefaultDuration(4000);
    }

    void connectSignals() {
        // Connect to toast manager signals for logging
        connect(&FluentToastManager::instance(), &FluentToastManager::toastShown,
                [](FluentToast* toast) {
                    qDebug() << "Toast shown:" << toast->title();
                });

        connect(&FluentToastManager::instance(), &FluentToastManager::toastClosed,
                [](FluentToast* toast) {
                    qDebug() << "Toast closed:" << toast->title();
                });
    }

    FluentTextInput* m_titleInput;
    FluentTextInput* m_messageInput;
    QComboBox* m_typeCombo;
    QComboBox* m_positionCombo;
    QSpinBox* m_durationSpin;
    QCheckBox* m_closableCheck;
    QCheckBox* m_progressCheck;
    QCheckBox* m_actionCheck;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    SmartNotificationDemo demo;
    demo.resize(800, 600);
    demo.show();

    return app.exec();
}

#include "smart_notifications.moc"
```

## See Also

- [FluentNotification](FluentNotification.md) - For banner-style notifications
- [FluentDialog](FluentDialog.md) - For modal dialogs
- [FluentProgressBar](FluentProgressBar.md) - For progress indication
- [FluentButton](FluentButton.md) - For action buttons within toasts
