# FluentNotification Component

The `FluentNotification` component is an enhanced notification widget that implements Microsoft's Fluent Design principles. It provides rich content support, smooth animations, and comprehensive notification management for user feedback and system messages.

## Overview

FluentNotification offers:
- **Multiple notification types** (info, success, warning, error)
- **Rich content support** with icons, actions, and custom widgets
- **Smart positioning** with automatic stacking
- **Smooth animations** for show/hide transitions
- **Auto-dismiss timing** with user interaction handling
- **Action buttons** for user responses
- **Theme integration** with automatic styling
- **Accessibility support** with screen reader announcements

## Basic Usage

### Simple Notifications

```cpp
#include "FluentQt/Components/FluentNotification.h"

// Show a simple info notification
FluentNotification::showInfo("Document saved successfully");

// Show different notification types
FluentNotification::showSuccess("Operation completed");
FluentNotification::showWarning("Please check your input");
FluentNotification::showError("Failed to connect to server");

// Show with custom duration
FluentNotification::showInfo("This message will disappear in 3 seconds", 3000);
```

### Notifications with Actions

```cpp
// Notification with action buttons
auto* notification = FluentNotification::showWarning("Unsaved changes detected");
notification->addAction("Save", [=]() {
    saveDocument();
    notification->dismiss();
});
notification->addAction("Discard", [=]() {
    discardChanges();
    notification->dismiss();
});
notification->addAction("Cancel", [=]() {
    notification->dismiss();
});
```

## Notification Types

### Information Notifications

```cpp
// Basic info notification
FluentNotification::showInfo("New version available");

// Info with details
auto* notification = new FluentNotification(FluentNotificationType::Info);
notification->setTitle("Update Available");
notification->setMessage("Version 2.1.0 is now available for download");
notification->setIcon(QIcon(":/icons/update"));
notification->show();
```

### Success Notifications

```cpp
// Success notification
FluentNotification::showSuccess("File uploaded successfully");

// Success with progress information
auto* notification = new FluentNotification(FluentNotificationType::Success);
notification->setTitle("Upload Complete");
notification->setMessage("MyDocument.pdf has been uploaded to the cloud");
notification->setDetailText("Size: 2.3 MB • Time: 00:15");
notification->show();
```

### Warning Notifications

```cpp
// Warning notification
FluentNotification::showWarning("Low disk space");

// Warning with action
auto* notification = FluentNotification::showWarning("Your trial expires in 3 days");
notification->addAction("Upgrade Now", [=]() {
    showUpgradeDialog();
});
notification->addAction("Remind Later", [=]() {
    scheduleReminder();
});
```

### Error Notifications

```cpp
// Error notification
FluentNotification::showError("Connection failed");

// Error with retry action
auto* notification = FluentNotification::showError("Failed to save document");
notification->addAction("Retry", [=]() {
    retrySaveOperation();
});
notification->addAction("Save As...", [=]() {
    showSaveAsDialog();
});
```

## Rich Content

### Notifications with Icons

```cpp
auto* notification = new FluentNotification(FluentNotificationType::Info);
notification->setTitle("Email Received");
notification->setMessage("You have a new message from John Doe");
notification->setIcon(QIcon(":/icons/email"));
notification->setIconSize(QSize(24, 24));
notification->show();
```

### Notifications with Images

```cpp
auto* notification = new FluentNotification(FluentNotificationType::Info);
notification->setTitle("Photo Upload Complete");
notification->setMessage("Your photo has been uploaded to the gallery");

// Set thumbnail image
QPixmap thumbnail = loadThumbnail("photo.jpg");
notification->setImage(thumbnail);
notification->setImageSize(QSize(64, 64));
notification->show();
```

### Custom Content Notifications

```cpp
auto* notification = new FluentNotification(FluentNotificationType::Info);
notification->setTitle("Download Progress");

// Create custom progress widget
auto* progressWidget = new QWidget;
auto* layout = new QVBoxLayout(progressWidget);

auto* progressBar = new QProgressBar;
progressBar->setValue(75);

auto* speedLabel = new QLabel("1.2 MB/s • 30 seconds remaining");
speedLabel->setStyleSheet("color: #666; font-size: 11px;");

layout->addWidget(progressBar);
layout->addWidget(speedLabel);

notification->setCustomWidget(progressWidget);
notification->show();
```

## Positioning and Stacking

### Notification Positioning

```cpp
// Set global notification position
FluentNotification::setGlobalPosition(FluentNotificationPosition::TopRight);
FluentNotification::setGlobalPosition(FluentNotificationPosition::BottomLeft);
FluentNotification::setGlobalPosition(FluentNotificationPosition::Center);

// Custom position with offset
FluentNotification::setGlobalPosition(FluentNotificationPosition::TopRight);
FluentNotification::setGlobalOffset(QPoint(20, 50));

// Per-notification positioning
auto* notification = new FluentNotification(FluentNotificationType::Info);
notification->setPosition(FluentNotificationPosition::BottomCenter);
notification->show();
```

### Notification Stacking

```cpp
// Configure stacking behavior
FluentNotification::setMaxVisibleNotifications(5);
FluentNotification::setStackingDirection(FluentStackingDirection::TopToBottom);
FluentNotification::setStackingSpacing(8);

// Auto-collapse older notifications
FluentNotification::setAutoCollapseEnabled(true);
FluentNotification::setAutoCollapseCount(3);  // Collapse after 3 notifications
```

## Timing and Persistence

### Auto-Dismiss Timing

```cpp
// Set default auto-dismiss duration
FluentNotification::setDefaultDuration(4000);  // 4 seconds

// Per-notification duration
auto* notification = FluentNotification::showInfo("Custom duration");
notification->setDuration(2000);  // 2 seconds

// Persistent notification (no auto-dismiss)
auto* notification = FluentNotification::showWarning("Action required");
notification->setPersistent(true);
```

### User Interaction Handling

```cpp
auto* notification = FluentNotification::showInfo("Hover to pause auto-dismiss");

// Pause auto-dismiss on hover
notification->setPauseOnHover(true);

// Dismiss on click
notification->setDismissOnClick(true);

// Handle user interactions
connect(notification, &FluentNotification::clicked, [=]() {
    qDebug() << "Notification clicked";
});

connect(notification, &FluentNotification::dismissed, [=]() {
    qDebug() << "Notification dismissed";
});
```

## Styling and Theming

### Appearance Customization

```cpp
auto* notification = new FluentNotification(FluentNotificationType::Info);

// Background and border
notification->setBackgroundColor(QColor(255, 255, 255));
notification->setBorderColor(QColor(0, 120, 215));
notification->setBorderWidth(2);
notification->setCornerRadius(6);

// Text styling
notification->setTitleColor(QColor(0, 0, 0));
notification->setMessageColor(QColor(64, 64, 64));
notification->setTitleFont(QFont("Segoe UI", 10, QFont::Bold));
notification->setMessageFont(QFont("Segoe UI", 9));

// Shadow effect
notification->setShadowEnabled(true);
notification->setShadowColor(QColor(0, 0, 0, 30));
notification->setShadowOffset(QPoint(0, 4));
notification->setShadowBlurRadius(12);
```

### Theme Integration

```cpp
#include "FluentQt/Styling/FluentTheme.h"

auto* notification = new FluentNotification(FluentNotificationType::Success);

// Use theme colors
auto& theme = FluentTheme::instance();
notification->setBackgroundColor(theme.color("notificationBackground"));
notification->setTitleColor(theme.color("notificationTitle"));
notification->setMessageColor(theme.color("notificationMessage"));

// Respond to theme changes
connect(&theme, &FluentTheme::themeChanged, [=]() {
    notification->setBackgroundColor(theme.color("notificationBackground"));
    notification->setTitleColor(theme.color("notificationTitle"));
    notification->setMessageColor(theme.color("notificationMessage"));
});
```

## Animations

### Show/Hide Animations

```cpp
auto* notification = new FluentNotification(FluentNotificationType::Info);

// Enable animations
notification->setAnimated(true);
notification->setShowAnimationDuration(300);
notification->setHideAnimationDuration(200);

// Animation types
notification->setShowAnimation(FluentNotificationAnimation::SlideIn);
notification->setHideAnimation(FluentNotificationAnimation::SlideOut);

// Alternative animations
notification->setShowAnimation(FluentNotificationAnimation::FadeIn);
notification->setShowAnimation(FluentNotificationAnimation::ScaleIn);

// Custom easing
notification->setAnimationEasing(FluentEasing::CubicOut);
```

### Stacking Animations

```cpp
// Configure stacking animations
FluentNotification::setStackingAnimationEnabled(true);
FluentNotification::setStackingAnimationDuration(250);
FluentNotification::setStackingAnimationEasing(FluentEasing::CubicOut);

// Custom stacking effects
FluentNotification::setStackingEffect(FluentStackingEffect::Slide);
FluentNotification::setStackingEffect(FluentStackingEffect::Cascade);
FluentNotification::setStackingEffect(FluentStackingEffect::Accordion);
```

## Integration Examples

### Application Notification System

```cpp
class NotificationManager : public QObject
{
    Q_OBJECT

public:
    static NotificationManager& instance() {
        static NotificationManager instance;
        return instance;
    }
    
    void showSaveSuccess(const QString& filename) {
        auto* notification = FluentNotification::showSuccess("Document saved");
        notification->setMessage(QString("'%1' has been saved successfully").arg(filename));
        notification->setIcon(QIcon(":/icons/save"));
        
        notification->addAction("Open Folder", [=]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(filename).absolutePath()));
        });
    }
    
    void showNetworkError(const QString& operation) {
        auto* notification = FluentNotification::showError("Network Error");
        notification->setMessage(QString("Failed to %1. Please check your connection.").arg(operation));
        notification->setIcon(QIcon(":/icons/network-error"));
        notification->setPersistent(true);
        
        notification->addAction("Retry", [=]() {
            retryOperation(operation);
            notification->dismiss();
        });
        
        notification->addAction("Work Offline", [=]() {
            enableOfflineMode();
            notification->dismiss();
        });
    }
    
    void showUpdateAvailable(const QString& version) {
        auto* notification = FluentNotification::showInfo("Update Available");
        notification->setMessage(QString("Version %1 is ready to install").arg(version));
        notification->setIcon(QIcon(":/icons/update"));
        notification->setDuration(0);  // Persistent
        
        notification->addAction("Install Now", [=]() {
            startUpdate();
            notification->dismiss();
        });
        
        notification->addAction("Install Later", [=]() {
            scheduleUpdate();
            notification->dismiss();
        });
        
        notification->addAction("Skip Version", [=]() {
            skipVersion(version);
            notification->dismiss();
        });
    }
    
    void showBulkOperationProgress(int completed, int total, const QString& operation) {
        // Remove existing progress notification
        if (m_progressNotification) {
            m_progressNotification->dismiss();
        }
        
        m_progressNotification = new FluentNotification(FluentNotificationType::Info);
        m_progressNotification->setTitle(QString("%1 in Progress").arg(operation));
        m_progressNotification->setMessage(QString("%1 of %2 items completed").arg(completed).arg(total));
        m_progressNotification->setIcon(QIcon(":/icons/progress"));
        m_progressNotification->setPersistent(true);
        
        // Create progress widget
        auto* progressWidget = new QWidget;
        auto* layout = new QVBoxLayout(progressWidget);
        
        auto* progressBar = new QProgressBar;
        progressBar->setMaximum(total);
        progressBar->setValue(completed);
        
        auto* detailLabel = new QLabel(QString("%1% complete").arg(completed * 100 / total));
        detailLabel->setStyleSheet("color: #666; font-size: 11px;");
        
        layout->addWidget(progressBar);
        layout->addWidget(detailLabel);
        
        m_progressNotification->setCustomWidget(progressWidget);
        
        if (completed < total) {
            m_progressNotification->addAction("Cancel", [=]() {
                cancelBulkOperation();
                m_progressNotification->dismiss();
                m_progressNotification = nullptr;
            });
        } else {
            // Operation completed
            m_progressNotification->setType(FluentNotificationType::Success);
            m_progressNotification->setTitle(QString("%1 Completed").arg(operation));
            m_progressNotification->setMessage(QString("All %1 items processed successfully").arg(total));
            m_progressNotification->setPersistent(false);
            m_progressNotification->setDuration(3000);
            m_progressNotification = nullptr;
        }
        
        m_progressNotification->show();
    }

private:
    NotificationManager() {
        // Configure global notification settings
        FluentNotification::setGlobalPosition(FluentNotificationPosition::TopRight);
        FluentNotification::setGlobalOffset(QPoint(20, 20));
        FluentNotification::setMaxVisibleNotifications(5);
        FluentNotification::setDefaultDuration(4000);
    }

private:
    FluentNotification* m_progressNotification{nullptr};
};
```

### Email Client Notifications

```cpp
class EmailNotificationManager : public QObject
{
    Q_OBJECT

public:
    EmailNotificationManager(QObject* parent = nullptr) : QObject(parent) {
        setupNotificationSettings();
    }
    
    void showNewEmailNotification(const EmailMessage& email) {
        auto* notification = new FluentNotification(FluentNotificationType::Info);
        notification->setTitle("New Email");
        notification->setMessage(QString("From: %1").arg(email.sender));
        notification->setDetailText(email.subject);
        notification->setIcon(QIcon(":/icons/email"));
        
        // Add sender avatar if available
        if (!email.senderAvatar.isNull()) {
            notification->setImage(email.senderAvatar);
            notification->setImageSize(QSize(40, 40));
        }
        
        notification->addAction("Reply", [=]() {
            openReplyWindow(email);
            notification->dismiss();
        });
        
        notification->addAction("Mark as Read", [=]() {
            markAsRead(email.id);
            notification->dismiss();
        });
        
        notification->show();
    }
    
    void showEmailSentNotification(const QString& recipient) {
        auto* notification = FluentNotification::showSuccess("Email sent");
        notification->setMessage(QString("Your message to %1 has been sent").arg(recipient));
        notification->setIcon(QIcon(":/icons/email-sent"));
        
        notification->addAction("View in Sent", [=]() {
            showSentFolder();
        });
    }
    
    void showSyncStatusNotification(int newEmails, int totalEmails) {
        if (newEmails > 0) {
            auto* notification = FluentNotification::showInfo("Sync Complete");
            notification->setMessage(QString("%1 new emails received").arg(newEmails));
            notification->setDetailText(QString("Total: %1 emails").arg(totalEmails));
            notification->setIcon(QIcon(":/icons/sync"));
            
            notification->addAction("View Inbox", [=]() {
                showInbox();
            });
        }
    }
    
    void showAttachmentDownloadNotification(const QString& filename, bool success) {
        if (success) {
            auto* notification = FluentNotification::showSuccess("Download Complete");
            notification->setMessage(QString("'%1' has been downloaded").arg(filename));
            notification->setIcon(QIcon(":/icons/download"));
            
            notification->addAction("Open File", [=]() {
                QDesktopServices::openUrl(QUrl::fromLocalFile(getDownloadPath(filename)));
            });
            
            notification->addAction("Show in Folder", [=]() {
                showInFileManager(getDownloadPath(filename));
            });
        } else {
            auto* notification = FluentNotification::showError("Download Failed");
            notification->setMessage(QString("Failed to download '%1'").arg(filename));
            notification->setIcon(QIcon(":/icons/download-error"));
            
            notification->addAction("Retry", [=]() {
                retryDownload(filename);
            });
        }
    }

private:
    void setupNotificationSettings() {
        // Position notifications in bottom-right for email app
        FluentNotification::setGlobalPosition(FluentNotificationPosition::BottomRight);
        FluentNotification::setGlobalOffset(QPoint(20, 20));
        
        // Allow more notifications for email app
        FluentNotification::setMaxVisibleNotifications(8);
        
        // Longer duration for email notifications
        FluentNotification::setDefaultDuration(6000);
        
        // Enable sound for new email notifications
        connect(this, &EmailNotificationManager::newEmailReceived, [=]() {
            if (m_soundEnabled) {
                playNotificationSound();
            }
        });
    }

private:
    bool m_soundEnabled{true};
};
```

### System Status Notifications

```cpp
class SystemStatusNotifications : public QObject
{
    Q_OBJECT

public:
    SystemStatusNotifications(QObject* parent = nullptr) : QObject(parent) {
        setupMonitoring();
    }

private:
    void setupMonitoring() {
        // Monitor disk space
        auto* diskTimer = new QTimer(this);
        connect(diskTimer, &QTimer::timeout, this, &SystemStatusNotifications::checkDiskSpace);
        diskTimer->start(60000);  // Check every minute
        
        // Monitor memory usage
        auto* memoryTimer = new QTimer(this);
        connect(memoryTimer, &QTimer::timeout, this, &SystemStatusNotifications::checkMemoryUsage);
        memoryTimer->start(30000);  // Check every 30 seconds
        
        // Monitor network connectivity
        auto* networkTimer = new QTimer(this);
        connect(networkTimer, &QTimer::timeout, this, &SystemStatusNotifications::checkNetworkStatus);
        networkTimer->start(10000);  // Check every 10 seconds
    }
    
    void checkDiskSpace() {
        qint64 freeSpace = getFreeSpace();
        qint64 totalSpace = getTotalSpace();
        double usagePercent = (double)(totalSpace - freeSpace) / totalSpace * 100;
        
        if (usagePercent > 90 && !m_diskSpaceWarningShown) {
            auto* notification = FluentNotification::showWarning("Low Disk Space");
            notification->setMessage(QString("Only %1 GB remaining on your system drive")
                                   .arg(freeSpace / 1024 / 1024 / 1024));
            notification->setIcon(QIcon(":/icons/disk"));
            notification->setPersistent(true);
            
            notification->addAction("Clean Up", [=]() {
                openDiskCleanup();
                notification->dismiss();
            });
            
            notification->addAction("Ignore", [=]() {
                m_diskSpaceWarningShown = true;
                notification->dismiss();
            });
            
            m_diskSpaceWarningShown = true;
        }
    }
    
    void checkMemoryUsage() {
        double memoryUsage = getMemoryUsagePercent();
        
        if (memoryUsage > 85 && !m_memoryWarningShown) {
            auto* notification = FluentNotification::showWarning("High Memory Usage");
            notification->setMessage(QString("Memory usage is at %1%").arg(qRound(memoryUsage)));
            notification->setIcon(QIcon(":/icons/memory"));
            
            notification->addAction("Task Manager", [=]() {
                openTaskManager();
            });
            
            // Auto-dismiss after showing task manager option
            notification->setDuration(5000);
            m_memoryWarningShown = true;
            
            // Reset warning flag after some time
            QTimer::singleShot(300000, [=]() {  // 5 minutes
                m_memoryWarningShown = false;
            });
        }
    }
    
    void checkNetworkStatus() {
        bool isConnected = isNetworkConnected();
        
        if (!isConnected && m_wasConnected) {
            auto* notification = FluentNotification::showError("Network Disconnected");
            notification->setMessage("Lost connection to the internet");
            notification->setIcon(QIcon(":/icons/network-disconnected"));
            notification->setPersistent(true);
            
            notification->addAction("Troubleshoot", [=]() {
                openNetworkTroubleshooter();
            });
            
            m_disconnectionNotification = notification;
        } else if (isConnected && !m_wasConnected && m_disconnectionNotification) {
            // Dismiss disconnection notification
            m_disconnectionNotification->dismiss();
            m_disconnectionNotification = nullptr;
            
            // Show reconnection notification
            auto* notification = FluentNotification::showSuccess("Network Reconnected");
            notification->setMessage("Internet connection has been restored");
            notification->setIcon(QIcon(":/icons/network-connected"));
            notification->setDuration(3000);
        }
        
        m_wasConnected = isConnected;
    }

private:
    bool m_diskSpaceWarningShown{false};
    bool m_memoryWarningShown{false};
    bool m_wasConnected{true};
    FluentNotification* m_disconnectionNotification{nullptr};
};
```

## Accessibility

FluentNotification provides comprehensive accessibility support:

```cpp
auto* notification = new FluentNotification(FluentNotificationType::Info);

// Accessible properties (automatically set)
notification->setAccessibleName("Notification");
notification->setAccessibleDescription("System notification with information");

// Screen reader support
notification->setAccessibleRole(QAccessible::AlertMessage);

// Keyboard navigation
notification->setKeyboardNavigationEnabled(true);

// Screen reader announcements
connect(notification, &FluentNotification::shown, [=]() {
    QString announcement = QString("%1: %2").arg(notification->title()).arg(notification->message());
    notification->announceToScreenReader(announcement);
});
```

## Best Practices

### Notification Design Guidelines
1. **Use appropriate types** - Match notification type to message severity
2. **Keep messages concise** - Provide clear, actionable information
3. **Limit notification frequency** - Avoid overwhelming users
4. **Provide relevant actions** - Include helpful action buttons
5. **Support dismissal** - Allow users to dismiss notifications

### Performance Considerations
- Use efficient stacking algorithms
- Cache notification content when possible
- Optimize animations for smooth interaction
- Implement proper cleanup for dismissed notifications

### Accessibility Guidelines
- Support keyboard navigation
- Provide screen reader announcements
- Use sufficient color contrast
- Include meaningful notification content
- Test with assistive technologies

## Signals Reference

```cpp
// Visibility signals
void shown();
void dismissed();
void expired();

// Interaction signals
void clicked();
void actionClicked(const QString& actionText);
void hovered(bool isHovered);

// Animation signals
void showAnimationStarted();
void showAnimationFinished();
void hideAnimationStarted();
void hideAnimationFinished();
```

## See Also

- [FluentTooltip](FluentTooltip.md) - For contextual help information
- [FluentButton](FluentButton.md) - For notification action buttons
- [FluentProgressBar](FluentProgressBar.md) - For progress notifications
- [API Reference - FluentNotification](../API_REFERENCE.md#fluentnotification) - Complete API documentation
